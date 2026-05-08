# LoadModelDlg — OnBnClickedOk() 호출 체인 성능 분석

> **대상 함수**: `LoadModelDlg.cpp:153` — `CLoadModelDlg::OnBnClickedOk()`  
> **실제 지연 위치**: `uScanView.cpp:607` — `DoModal()` 반환 후 처리 코드  
> **분석 방법**: 정적 코드 분석

---

## 1. 핵심 결론 (요약)

`OnBnClickedOk()` 자체는 `CDialog::OnOK()` 한 줄뿐이라 **즉시 반환**된다.  
지연의 실체는 `uScanView.cpp`에서 `DoModal() == IDOK` 이후 실행되는  
**동기(블로킹) 모델 로드 체인 전체**이다.

```
[OK 버튼 클릭]
    ↓ OnBnClickedOk() → CDialog::OnOK()          — 즉시 반환 (< 1ms)
    ↓ DoModal() 반환 → uScanView.cpp:607

[uScanView.cpp — UI 스레드에서 순차 실행]
    ↓ UI 창 숨기기 × 8                            — 빠름
    ↓ 모델명 복사 × 4                              — 빠름
    ↓ 모델타입 탐색 루프                           — 빠름
    ↓ ReadInspectionTypeInfoINI()   [조건부]       — ★ disk I/O
    ↓ ReadOverlayViewportINI()      [조건부]       — ★ disk I/O
    ↓ ReadDefectReViewInfoINI()     [조건부]       — ★ disk I/O
    ↓ LoadModelSWBaseData() × (MODEL_DATA_MAX × VISION_NUMBER_MAX)  ← 최대 주요 병목
    ↓ LoadModelSWData()    × (MODEL_DATA_MAX × VISION_NUMBER_MAX)   ← 주요 병목
    ↓ LoadModelHWBaseData() × VISION_NUMBER_MAX                     — 보통
    ↓ LoadModelHWData()     × VISION_NUMBER_MAX                     — 보통
    ↓ CountPerAreaCondition() / LoadCountPerAreaCondition_ini()      — 보통
    ↓ UpdateTeachParamView()                       — UI 갱신
    ↓ Initialize_Variables(TRUE)                   — FAI 초기화
    ↓ TrayAdminViewDlg.Show() × 4                  — UI
    ↓ AfxMessageBox("완료")                         — 사용자 확인 대기
    ↓ INI.Set_String(LastStatus.txt)                — 1회 파일 쓰기
```

---

## 2. 단계별 병목 분석

### 🔴 단계 1 (압도적 주요): `LoadModelSWBaseData()` × 최대 8회

> 이미 `LoadModelSW_Performance_Analysis.md`에 상세 분석됨.

- **INI 반복 읽기**: 카메라당 최대 **~4,891회** `GetPrivateProfile*` 독립 disk I/O
  - `ModelBase.ini`: 45탭 × 75 keys = 3,375 + α
  - `FaiMeasureSpec_DFA.ini`: 151항목 × 10 keys = 1,511 + α
- **HALCON 모델 로드**: 탭당 `ReadNccModel` / `ReadShapeModel` (바이너리, 느림)
- **곱셈 인자**: `MODEL_DATA_MAX(1~2)` × `VISION_NUMBER_MAX(4)` → **최대 8배 반복**

| 카메라 수 | 예상 INI 호출 수 | SSD(2ms/call) | HDD(10ms/call) |
|---|---|---|---|
| 1 | ~4,891 | ~10초 | ~49초 |
| 4 | ~19,564 | ~39초 | ~196초 |
| 2세트×4 | ~39,128 | ~78초 | ~391초 |

---

### 🔴 단계 2 (주요): `LoadModelSWData()` × 최대 8회

> 이미 `LoadModelSW_Performance_Analysis.md`에 상세 분석됨.

- **`GTRegion::Load()` 재귀**: ROI 수 × `MAX_INSPECTION_TAB(45)` × `AlgorithmParam::Load()`
- **`CreateAnisoShapeModelXld` / `CreateScaledShapeModelXld`**: ROI Alignment ROI마다 HALCON Shape Model 처음부터 계산 (ROI당 10ms~수백ms)
- **`DeserializeVariationModel` / `DeserializeNccModel`**: ROI × 탭 수 HALCON 역직렬화
- **`TupleConcat` 루프**: 폴리곤 포인트 O(N²) 복사

---

### 🟠 단계 3 (조건부 실행): `ReadInspectionTypeInfoINI` / `ReadOverlayViewportINI` / `ReadDefectReViewInfoINI`

모델 타입이 이전과 다를 경우에만 실행된다 (`!IsSameModelType(i)`).

| 함수 | 파일 | 호출 수 |
|---|---|---|
| `ReadInspectionTypeInfoINI()` | `InspectionTypeInfo_{model}.ini` | `MAX_INSPECTION_TYPE(23)` × 2 keys = **46회** |
| `ReadOverlayViewportINI()` | `OverlayViewport_{model}.ini` | `VISION_NUMBER_MAX(4)` × `MAX_RESULT_IMAGE(18)` × 4 keys = **288회** |
| `ReadDefectReViewInfoINI()` | `ReViewInfo_{model}.ini` | `MAX_REVIEW_SAMPLE(400)` × 1 key (Nan에서 중단) = **최대 400회** |

---

### 🟡 단계 4 (보통): `LoadModelHWBaseData()` × VISION_NUMBER_MAX(4)

카메라당 2개 INI 파일 × 소수 키:
- `CalData.ini` → 1회 (INLINE_MODE에서만)
- `AutoCal.ini` → 11회 (Get_Integer × 7, Get_Double × 4)
- 합계: ~12회 × 4 카메라 = **~48회** (경미)

---

### 🟡 단계 5 (보통): `LoadModelHWData()` × VISION_NUMBER_MAX(4)

카메라당 5~6개 INI 파일 호출 (각 파일 수십 키):
- `LoadHWModelBase()` → `HW/Vision_N{n}/ModelBase.ini`
- `LoadLightInfo()` / `LoadPwmLightInfo()` → 조명 설정 INI
- `LoadSequenceInfo()` → 시퀀스 INI
- `LoadSpecularParam()` → 스페큘러 파라미터 INI
- `LoadAutoFocus()` → AF INI
- `LoadMotionMovingPosition()` / `_Offset()` → 모션 위치 INI
- `LoadLightAverageValue()` → 조명 평균값 INI

**각 INI 파일은 독립 `CIniFileCS` 객체** → 파일당 키 수만큼 disk I/O.  
총 ~수백 회 추가 disk I/O.

---

### 🟡 단계 6 (경미): `CountPerAreaCondition()` / `LoadCountPerAreaCondition_ini()`

- JSON 파일 존재 시: `CountPerAreaCondition()` — 단일 JSON 파일 파싱 (빠름)
- INI 폴백 시: `LoadCountPerAreaCondition_ini()` — 다차원 루프 × `Get_Double/Integer` 반복

---

### 🟡 단계 7 (경미): `Initialize_Variables(TRUE)`

- `TrayImagePB.Reset()` × 4 + `OkNg` 배열 초기화: 메모리 연산, 빠름
- **`ReplaceFAIInspector()`**: FAI Inspector 교체 — 모델 타입별 Inspector 신규 생성  
  내부적으로 힙 할당 + 초기화가 있으나 I/O 없음, 경미

---

## 3. 전체 병목 요약표

| 순위 | 호출 | 원인 | 예상 지연 |
|---|---|---|---|
| 1 | `LoadModelSWBaseData()` × ~8 | INI ~4,891회/카메라 × 8 | **수십 초** |
| 2 | `LoadModelSWData()` × ~8 | HALCON Shape Model 재계산 × ROI 수 | **수 초~수십 초** |
| 3 | `LoadModelHWData()` × 4 | 5~6개 INI 파일 × 다수 키 | 수 초 |
| 4 | `ReadOverlayViewportINI()` | 288회 disk I/O | 수백 ms |
| 5 | `ReadDefectReViewInfoINI()` | 최대 400회 disk I/O | 수백 ms |
| 6 | `LoadModelHWBaseData()` × 4 | ~48회 disk I/O | 경미 |
| 7 | `ReadInspectionTypeInfoINI()` | 46회 disk I/O | 경미 |
| 8 | `Initialize_Variables(TRUE)` | 메모리 연산 + FAI init | 경미 |
| 9 | `OnBnClickedOk()` 자체 | `CDialog::OnOK()` 1줄 | **0ms** |

---

## 4. `OnBnClickedOk()` 자체에 대한 결론

```cpp
void CLoadModelDlg::OnBnClickedOk()
{
    CDialog::OnOK();   // UpdateData(TRUE) + EndDialog(IDOK) — 즉시 반환
}
```

이 함수 자체는 **지연 원인이 아니다.**  
`CDialog::OnOK()`는 `UpdateData(TRUE)` (에디트 컨트롤 → 멤버 변수 복사)와  
`EndDialog(IDOK)` (다이얼로그 닫기) 만 수행하며, 두 작업 모두 sub-millisecond.

**지연의 전체 원인은 `uScanView.cpp`의 `DoModal()` 이후 코드**이며,  
이는 모두 **UI 스레드에서 동기적**으로 실행된다.

---

## 5. 개선 방향 제안

### 핵심: 로드 작업을 워커 스레드로 분리

```
[OK 클릭]
    ↓ CDialog::OnOK()             — 즉시 반환
    ↓ 로딩 프로그레스 다이얼로그 표시
    ↓ std::thread / CWinThread 로 로드 작업 시작
        [워커 스레드]
            LoadModelSWBaseData() × N
            LoadModelSWData()     × N
            LoadModelHWBaseData() × N
            LoadModelHWData()     × N
            ...
        [완료 시] PostMessage → UI 스레드에서 마무리 처리
    ↓ UI 스레드는 메시지 루프 계속 실행 → 응답성 유지
```

### INI 캐싱 개선 (`CIniFileCS`)

`LoadModelSWBaseData()`의 ~4,891회 disk I/O를 2회로 줄임 (2개 파일 각 1회 읽기).  
`LoadModelSW_Performance_Analysis.md` 개선 방향 A 참조.

---

## 6. 관련 파일 목록

| 파일 | 관련 내용 |
|---|---|
| `LoadModelDlg.cpp:153` | `OnBnClickedOk()` — 분석 대상 (자체 비용 없음) |
| `uScanView.cpp:607` | `DoModal()` 이후 실제 지연 발생 구간 |
| `ModelDataManager.cpp` | `LoadModelSWBaseData`, `LoadModelSWData`, `LoadModelHWBaseData`, `LoadModelHWData` |
| `GTRegion.cpp:133` | `GTRegion::Load()` — ROI별 HALCON 모델 재계산 |
| `IniFileCS.cpp` | `CIniFileCS::Get_*` — 매 호출마다 Win32 INI API |
| `uScan.cpp:1495, 10135, 11440` | `ReadDefectReViewInfoINI`, `ReadOverlayViewportINI`, `ReadInspectionTypeInfoINI` |
| `Docs/LoadModelSW_Performance_Analysis.md` | `LoadModelSWBase/SWData` 상세 분석 |
| `Docs/LoadModelDlg_Performance_Analysis.md` | `OnSelchangeListModelnames` 분석 |
