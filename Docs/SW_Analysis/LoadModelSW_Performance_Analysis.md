# LoadModelSWBaseData / LoadModelSWData 성능 분석

> **대상 함수**:
> - `ModelDataManager.cpp` — `CModelDataManager::LoadModelSWBaseData()` (line 1745)
> - `ModelDataManager.cpp` — `CModelDataManager::LoadModelSWData()` (line 1558)
> **호출 위치**: `uScan.cpp` line 840-841 (이중 루프 — MODEL_DATA_MAX × VISION_NUMBER_MAX)
> **분석 방법**: 정적 코드 분석

---

## 1. 호출 구조 (uScan.cpp line 832)

```
for iDualModelData in [0, MODEL_DATA_MAX)         // 1 or 2
    for iPcVisionNo in [VISION_NUMBER_MAX-1, 0]   // 최대 4개
        if sVisionCamType == "Undefine": continue

        LoadModelSWBaseData()   // ← 분석 대상 1
        LoadModelSWData()       // ← 분석 대상 2
```

**최악의 경우**: `MODEL_DATA_MAX=2`, `VISION_NUMBER_MAX=4` → **8회 반복 호출**.  
각 함수의 비용이 N배 증폭됨.

---

## 2. `LoadModelSWBaseData()` 병목 분석

### 🔴 문제 1 (압도적 주요): `GetPrivateProfile*` 반복 호출 — 최대 4,891회 / 카메라당

`CIniFileCS`는 내용 캐시 없이 매 `Get_*` 호출마다 Win32 INI API를 독립 실행한다.  
(**매 호출 = 파일 열기 → 읽기 → 닫기**)

#### 파일 1: `ModelBase.ini` → 약 3,380회 호출

| 구간 | 반복 | 호출 수 |
|---|---|---|
| Base 섹션 (VERSION, ParameterVersion, TrayArrayX, TrayArrayY 등) | 1 | ~5 |
| Local Align 섹션 (각 항목 75 keys) × `MAX_IMAGE_TAB=45` | 45 | **3,375** |

Local Align 섹션의 75 keys/탭 구성:

```
m_bUseLocalAlignMatching(1) + m_iLocalMatchingMethod(1) +
LTX/LTY/RBX/RBY(4) + PyramidLevel(1) + AngleRange(1) +
ScaleMin/Max(2) + TeachContrast×3(3) + Score(1) + MinContrast(1) +
SearchMarginX/Y(2) + TeachAlignRefX/Y(2) + AffineConstant(1) +
m_bLocalAlignImage[MAX_IMAGE_TAB=45](45) +     ← 탭 수만큼 내부 중첩
ProcessChType(1) + Filter params(4) + AngleComp(2) + XYComp(3)
= 75 keys/탭 × 45탭 = 3,375 calls
```

#### 파일 2: `FaiMeasureSpec_DFA.ini` → 약 1,511회 호출

| 구간 | 반복 | 호출 수 |
|---|---|---|
| m_bMUseFai | 1 | 1 |
| FAI 항목 10 keys × `MAX_FAI_ITEM=151` | 151 | **1,510** |

**LoadModelSWBaseData() 합계: ~4,891 `GetPrivateProfile*` / 카메라 1개**  
→ 카메라 4개 × 2 세트 = **최대 ~39,128회 disk I/O**

**예상 지연**: SSD 기준 2ms/call → 약 **10~80초** (카메라 수 / 스토리지 속도에 따라)

---

### 🔴 문제 2: HALCON 매칭 모델 파일 로드 — 최대 45회

```cpp
// ModelDataManager.cpp line 1907-1918
for (int i = 0; i < MAX_IMAGE_TAB; i++)
{
    if (m_bUseLocalAlignMatching[i])
    {
        ReadNccModel(...)          // NCC 모델 바이너리 파일 읽기
        // 또는
        ReadShapeModel(...)        // Shape 모델 바이너리 파일 읽기
        // 또는
        ReadDeformableModel(...)   // Deformable 모델 바이너리 파일 읽기

        THEAPP.DoEvents();
    }
}
```

각 모델 파일 1개 = 수백 KB ~ 수 MB 바이너리.  
`ReadShapeModel`은 특히 HALCON 내부에서 피라미드 구조를 재구성해야 하므로 CPU+I/O 비용이 높다.

---

### 🟡 문제 3: `THEAPP.DoEvents()` 비용

모델 로드 루프 중간에 `DoEvents()` 호출이 있다. UI 응답성 유지가 목적이나,  
매 `DoEvents()` 호출마다 Windows 메시지 큐 펌핑이 발생 → 반복이 많을수록 오버헤드 누적.

---

## 3. `LoadModelSWData()` 병목 분석

### 🔴 문제 1 (주요): `GTRegion::Load()` 재귀 호출 — ROI 수 × 무거운 연산

```
m_pInspectionArea->Load(hFile, ...)
    └── 모든 자식 GTRegion 에 대해 재귀:
        ├── ReadFile() × 수십 번 (바이너리 역직렬화)
        ├── for i in MAX_INSPECTION_TAB(45):
        │       m_AlgorithmParam[i].Load(hFile, ...)   ← 알고리즘 파라미터 역직렬화
        │       THEAPP.DoEvents()                       ← 메시지 펌핑
        │       if bUseROIAnisoAlign or bUseROIAlign:
        │           CreateAnisoShapeModelXld(...)       ← ★ HALCON Shape 모델 컴파일 (매우 느림)
        │           or CreateScaledShapeModelXld(...)   ← ★ HALCON Shape 모델 컴파일 (매우 느림)
        ├── for i in MAX_INSPECTION_TAB:
        │       if bVarModelExist:
        │           DeserializeVariationModel(...)      ← HALCON Variation 모델 역직렬화
        │           or ReadVariationModel(...)          ← 파일 읽기
        │           FreadSerializedItem × 2 (Mean/Stdev Image)
        │           DeserializeImage × 2
        └── if bPartModelExist:
                FreadSerializedItem(...)
                DeserializeNccModel(...)               ← NCC 모델 역직렬화
                TupleConcat(...)                       ← HTuple N² 복사
```

---

### 🔴 문제 2: ROI당 `CreateAnisoShapeModelXld` / `CreateScaledShapeModelXld` 호출

```cpp
// GTRegion.cpp line 322-346
if (m_AlgorithmParam[i].m_bUseROIAnisoAlign)
    CreateAnisoShapeModelXld(HInspectAreaXLD, "auto", ...);   // 극히 무거운 연산
else if (m_AlgorithmParam[i].m_bUseROIAlign)
    CreateScaledShapeModelXld(HInspectAreaXLD, "auto", ...);  // 무거운 연산
```

이 함수들은 **HALCON이 내부적으로 Shape Model을 처음부터 계산**한다 (pyramid level, edge detection, angle quantization 등).  
ROI 해상도와 pyramidal level 수에 따라 1개당 **10ms ~ 수백 ms** 소요.  
ROI 수가 수백 개인 레시피에서는 이 단계만으로 **수 초 이상** 지연.

---

### 🔴 문제 3: `TupleConcat` 루프 — O(N²) 메모리 복사

```cpp
// GTRegion.cpp line 233-240
for (int i = 0; i < iRegionPointNum; i++) {
    ReadFile(hFile, &lTemp, sizeof(long), ...);
    TupleConcat(HRow, lTemp, &HRow);           // ← 매번 HTuple 전체 복사
    ReadFile(hFile, &lTemp, sizeof(long), ...);
    TupleConcat(HColumnBegin, lTemp, &HColumnBegin);
    ReadFile(hFile, &lTemp, sizeof(long), ...);
    TupleConcat(HColumnEnd, lTemp, &HColumnEnd);
}
```

`TupleConcat`은 HALCON 내부에서 **새 배열을 할당하고 기존 원소를 복사**한 뒤 추가한다.  
`iRegionPointNum`이 큰 폴리곤 ROI(예: 1000포인트)일 경우 **O(N²) 복사** 발생.  
ex) N=1000: 1,000회 반복 × 평균 500 원소 복사 = 500,000번의 원소 이동.

---

### 🟡 문제 4: `AlgorithmParam::Load()` × MAX_INSPECTION_TAB(45) / ROI

각 ROI에 대해 45개의 검사 탭 알고리즘 파라미터를 순차적으로 읽는다.  
ROI 수가 수백 개라면: 수백 ROI × 45 탭 = **수만 번의 `AlgorithmParam::Load()` 호출**.

---

## 4. 병목 요약표

### `LoadModelSWBaseData()`

| 순위 | 위치 | 원인 | 비중 |
|---|---|---|---|
| 1 | `INI_SWModelBase.Get_*` (Local Align) | 45탭 × 75 keys = **3,375회 disk I/O** | 최대 |
| 2 | `INI_FAIMeasureSpec.Get_*` | 151항목 × 10 keys = **1,510회 disk I/O** | 높음 |
| 3 | `ReadNccModel / ReadShapeModel` | HALCON 모델 파일 읽기 (최대 45회) | 높음 |
| 4 | 외부 루프 (MODEL_DATA_MAX × VISION_NUMBER_MAX) | 최대 8배 증폭 | 곱셈 |

### `LoadModelSWData()`

| 순위 | 위치 | 원인 | 비중 |
|---|---|---|---|
| 1 | `CreateAnisoShapeModelXld / CreateScaledShapeModelXld` | ROI당 Shape Model 재계산 | 최대 |
| 2 | `DeserializeVariationModel / DeserializeNccModel` | HALCON 모델 역직렬화 × ROI수 | 높음 |
| 3 | `TupleConcat` 루프 | O(N²) HTuple 복사 | 중간 |
| 4 | `AlgorithmParam::Load` × 45 × ROI수 | 순차 바이너리 읽기 반복 | 중간 |

---

## 5. 개선 방향 제안

### 단기 개선

**A. `CIniFileCS` 내용 캐싱 — INI 파일을 1회만 읽기**

가장 효과가 큰 개선. `GetPrivateProfileSection`으로 섹션 전체를 한 번에 읽거나,
생성자에서 파일 전체를 `std::map`에 파싱하면 4,891회 → **2회** I/O (파일 2개).

```cpp
// 현재 (각 Get_* 마다 파일 열기)
m_bMUseFai = INI_FAIMeasureSpec.Get_Bool("FAI", "m_bMUseFai", FALSE);
for (int i = 0; i < MAX_FAI_ITEM; i++)
    m_bMInspectFai[i] = INI_FAIMeasureSpec.Get_Bool("FAI", strKey, FALSE);
    // ... × 10

// 개선안: 섹션 전체를 메모리로 1회 읽어 map에 파싱 후 조회
```

**B. `LOCAL_ALIGN` 내부 `m_bLocalAlignImage[45]` 중첩 루프 구조 개선**

현재 45탭 루프 안에서 또 45번 탭 배열을 읽음 → 탭당 45개 여분 키.  
Binary 배열로 한 번에 저장하거나 불필요한 항목 건너뛰기 로직 추가.

### 중기 개선

**C. `CreateAnisoShapeModelXld / CreateScaledShapeModelXld` 결과를 파일로 캐싱**

모델 파라미터가 변경되지 않은 경우 이전에 계산된 Shape Model 파일을 재사용.  
`ReadShapeModel`은 `CreateShapeModel`보다 훨씬 빠르다.

**D. `TupleConcat` 루프 → GenRegionRuns 일괄 변환**

ROI 포인트를 먼저 배열에 수집한 뒤 `GenRegionRuns`를 1회 호출:

```cpp
// 개선 (O(N) — 미리 크기 할당 후 일괄 처리)
std::vector<long> vRow, vColBegin, vColEnd;
vRow.reserve(iRegionPointNum);
for (int i = 0; i < iRegionPointNum; i++) {
    ReadFile(hFile, &lTemp, sizeof(long), ...); vRow.push_back(lTemp);
    ...
}
HTuple HRow(vRow.size(), vRow.data());  // 1회 HTuple 생성
GenRegionRuns(&m_HTeachPolygonRgn, HRow, HColumnBegin, HColumnEnd);
```

**E. 비동기 모델 로드**

LoadModelSWBaseData / LoadModelSWData를 워커 스레드에서 수행하고,
완료 시 UI 스레드에 `PostMessage`로 통보. 초기화 중 UI 프리징 제거.

---

## 6. 관련 파일 목록

| 파일 | 관련 내용 |
|---|---|
| `uScan.cpp` line 832-845 | 외부 이중 루프 — 최대 8회 반복 호출 지점 |
| `ModelDataManager.cpp` line 1558, 1745 | 분석 대상 두 함수 본체 |
| `GTRegion.cpp` line 133 | `GTRegion::Load()` — ROI별 재귀 역직렬화 + HALCON 모델 재계산 |
| `IniFileCS.cpp / .h` | `CIniFileCS::Get_*` — 매 호출마다 Win32 INI API 직접 호출 |
| `Define.h` | `MAX_IMAGE_TAB=45`, `VISION_NUMBER_MAX=4`, `MODEL_DATA_MAX=1~2` |
| `FAIDefine.h` | `MAX_FAI_ITEM=151 (TOTAL_FAI_NUMBER)` |
