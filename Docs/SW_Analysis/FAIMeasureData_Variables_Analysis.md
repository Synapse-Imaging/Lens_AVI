# FAIMeasureData.h 측정 변수 사용 분석

> 분석 대상: `FAI/ModelData/FAIMeasureData.h`의 `CenterlineMeasureStruct` 멤버 변수
> 작성일: 2026-03-11
> 작성자: 비전SW 김준호

---

## 1. 분석 대상 변수

```cpp
DPOINT  m_dMeasureCPoint[MEASURE_POS_TOTAL][MAX_MEASURE_POINT_NUM];  // line 27
HObject m_HMeasureXLD   [MEASURE_POS_TOTAL][MAX_MEASURE_POINT_NUM];  // line 28
HObject m_HMeasureRgn   [MEASURE_POS_TOTAL][MAX_MEASURE_POINT_NUM];  // line 29
DPOINT  m_dBlendCPoint  [VISION_NUMBER_MAX][POSITION_BLEND_END][BLEND_POINT_END]; // line 30
```

---

## 2. 관련 상수 정의

| 상수명 | 값 | 정의 위치 | 의미 |
|--------|:---:|---------|------|
| `MEASURE_POS_TOTAL` | 63 | `FAI/FAIDefine.h:280` | 측정 위치 총 개수 (`enMeasurePos::MEASURE_POSITION_END`) |
| `MAX_MEASURE_POINT_NUM` | 8 | `FAI/FAIDefine.h:44` | 위치당 최대 측정점 개수 (원 맞춤 시 최대 8점) |
| `VISION_NUMBER_MAX` | 4 | `Define.h:97` | 최대 비전 카메라 수 |
| `POSITION_BLEND_END` | 8 | `FAI/FAIDefine.h:85` | 블렌드 위치 개수 |
| `BLEND_POINT_END` | 2 | `FAI/FAIDefine.h:103` | 블렌드 포인트 수 (Start/End) |

### enBlendPos 열거형 (`FAI/FAIDefine.h:75~86`)

```
POSITION_BLEND_1_X   (0) — 블렌드 이미지 1, 수평선
POSITION_BLEND_1_Y   (1) — 블렌드 이미지 1, 수직선
POSITION_BLEND_2_X   (2) — 블렌드 이미지 2, 수평선
POSITION_BLEND_2_Y   (3) — 블렌드 이미지 2, 수직선
POSITION_REFER_1_X   (4) — 참조 위치 1, 수평
POSITION_REFER_1_Y   (5) — 참조 위치 1, 수직
POSITION_REFER_2_X   (6) — 참조 위치 2, 수평
POSITION_REFER_2_Y   (7) — 참조 위치 2, 수직
```

### enBlendPoint 열거형

```
BLEND_POINT_ST (0) — 선분 시작점
BLEND_POINT_ED (1) — 선분 끝점
```

---

## 3. 변수별 상세 분석

### 3.1 `m_dMeasureCPoint[MEASURE_POS_TOTAL][MAX_MEASURE_POINT_NUM]`

**타입**: `DPOINT[63][8]`
**의미**: 각 측정 위치에서 감지된 중심점(Center Point) 좌표 배열

**인덱스 의미**:
- `[i]` : 측정 위치 (0~62, `enMeasurePos` 열거형 → `MtoI()` 함수로 변환)
- `[j]` : 해당 위치의 j번째 측정점 (0~7)

**사용 파일 및 역할**:

| 파일 | 쓰기/읽기 | 용도 |
|------|:---------:|------|
| `AlgorithmThreadDefine.h` | 쓰기 | 검사 스레드에서 edge 중심점 저장 |
| `TabControlDlg.cpp` | 쓰기 | 교육(teaching) 모드에서 포인트 저장 |
| `FAI/ModelTypes/BOIFAIInspector.cpp` | 읽기/쓰기 | 렌즈 중심, Datum 포인트 저장 및 계산 |
| `FAI/ModelTypes/ACTFAIInspector.cpp` | 읽기/쓰기 | Fiducial 라인 포인트 저장 및 거리 계산 |
| `FAI/ModelTypes/ATWFAIInspector.cpp` | 읽기/쓰기 | 렌즈 내부 포인트 저장 및 계산 |
| `FAI/ModelTypes/AKCFAIInspector.cpp` | 읽기 | 블렌드 포인트 계산에 활용 |
| `FAI/ModelTypes/BOSFAIInspector.cpp` | 읽기 | Chassis hole 중심점 참조 |

**대표 사용 패턴**:

```cpp
// [쓰기] BOIFAIInspector.cpp - 렌즈 중심점 저장
StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][0].x = dInnerCenterX;
StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][0].y = dInnerCenterY;

// [읽기] ACTFAIInspector.cpp - Fiducial 라인 끝점 읽기 (FAI 계산용)
dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A)][0].x;
dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A)][0].y;
dLineEnd.x   = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A)][1].x;
dLineEnd.y   = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A)][1].y;

// [쓰기] TabControlDlg.cpp - 교육 모드에서 edge 중앙점 저장
CFAIDataManager::GetInstance().GetTeachingMeasure()
    .m_dMeasureCPoint[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].x =
    (m_dEdgeLineStartX + m_dEdgeLineEndX) * 0.5;
```

---

### 3.2 `m_HMeasureXLD[MEASURE_POS_TOTAL][MAX_MEASURE_POINT_NUM]`

**타입**: `HObject[63][8]`
**의미**: 각 측정 위치에서 감지된 HALCON XLD(eXtended Line Description) 선분 객체

> XLD는 sub-pixel 정밀도의 contour 객체. 선분, 원호, 경계선 등 선 기반 측정에 사용됨.

**인덱스 의미**: `m_dMeasureCPoint`와 동일한 구조 (`[측정위치][포인트번호]`)

**사용 파일 및 역할**:

| 파일 | 쓰기/읽기 | 용도 |
|------|:---------:|------|
| `FAI/ModelData/FAIMeasureData.h` | 초기화 | `GenEmptyObj()` 로 빈 객체 초기화 |
| `FAI/ModelTypes/BOIFAIInspector.cpp` | 읽기/쓰기 | 선분 저장 및 리뷰 화면 병합 |
| `FAI/ModelTypes/ACTFAIInspector.cpp` | 읽기/쓰기 | Fiducial 라인 저장 및 표시 |
| `FAI/ModelTypes/ATWFAIInspector.cpp` | 읽기/쓰기 | 렌즈 포인트 선분 저장 및 표시 |
| `TabControlDlg.cpp` | 쓰기 | 교육 모드에서 Contour 저장 |

**대표 사용 패턴**:

```cpp
// [쓰기] BOIFAIInspector.cpp - 선분 객체 저장
StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][0] = hLineWest;
StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][1] = hLineFlex;

// [읽기] BOIFAIInspector.cpp - 리뷰 화면에 선분 병합
ConcatObj(
    m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI5X_ID][iFAIParamNo][0],
    StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][0],
    &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI5X_ID][iFAIParamNo][0]);

// [읽기] ATWFAIInspector.cpp - 루프로 다수 XLD 병합 표시
for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++) {
    ConcatObj(
        m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0],
        pStructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii],
        &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
}
```

---

### 3.3 `m_HMeasureRgn[MEASURE_POS_TOTAL][MAX_MEASURE_POINT_NUM]`

**타입**: `HObject[63][8]`
**의미**: 각 측정 위치에서 감지된 HALCON Region(영역) 객체

> Region은 pixel 집합으로, blob(구멍, 핀, 부품 외곽) 기반 측정에 사용됨.

**인덱스 의미**: `m_dMeasureCPoint`와 동일한 구조 (`[측정위치][포인트번호]`)

**사용 파일 및 역할**:

| 파일 | 쓰기/읽기 | 용도 |
|------|:---------:|------|
| `FAI/ModelData/FAIMeasureData.h` | 초기화 | `GenEmptyObj()` 로 빈 객체 초기화 |
| `AlgorithmThreadDefine.h` | 쓰기 | 검사 스레드에서 측정 영역 저장 |
| `TabControlDlg.cpp` | 쓰기 | 교육 모드에서 측정 영역 저장 |
| `FAI/ModelTypes/BOSFAIInspector.cpp` | 읽기 | Chassis hole 영역 유효성 검증 후 중심점 참조 |

**대표 사용 패턴**:

```cpp
// [읽기] BOSFAIInspector.cpp - Region 유효성 검증 후 중심점 활용
HObject HRgnChaisHoleEast =
    StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0];
if (gFunc.ValidHRegion(HRgnChaisHoleEast) == TRUE)
{
    dBracketHoleEastCenter.x =
        StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0].x;
    dBracketHoleEastCenter.y =
        StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0].y;
}

// [쓰기] TabControlDlg.cpp - 교육 모드에서 측정 영역 누적 저장
ConcatObj(
    CFAIDataManager::GetInstance().GetTeachingMeasure()
        .m_HMeasureRgn[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID],
    HMeasureRetRgn,
    &(CFAIDataManager::GetInstance().GetTeachingMeasure()
        .m_HMeasureRgn[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID]));
```

---

### 3.4 `m_dBlendCPoint[VISION_NUMBER_MAX][POSITION_BLEND_END][BLEND_POINT_END]`

**타입**: `DPOINT[4][8][2]`
**의미**: 각 비전 카메라별 이미지 블렌드(스티칭) 기준 선분의 시작/끝 좌표

**인덱스 의미**:
- `[i]` : 카메라 번호 (0~3, `VISION_NUMBER_MAX`)
- `[j]` : 블렌드 위치 종류 (0~7, `enBlendPos`)
- `[k]` : 선분 시작점(0=ST) 또는 끝점(1=ED)

**사용 파일 및 역할**:

| 파일 | 쓰기/읽기 | 용도 |
|------|:---------:|------|
| `FAI/ModelData/FAIMeasureData.h` | 초기화 | `ResetFAI()` 에서 Reset |
| `AlgorithmThreadDefine.h` | 쓰기 | 검사 스레드에서 블렌드 포인트 저장 |
| `FAI/ModelTypes/AKCFAIInspector.cpp` | 읽기 | 블렌드 좌표로 회전각 계산, UI Cross 표시 |

**대표 사용 패턴**:

```cpp
// [읽기] AKCFAIInspector.cpp - 블렌드 선분 끝점 추출 (회전각 계산)
DPOINT dImage1HStartPoint, dImage1HEndPoint;
dImage1HStartPoint.x =
    StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][BLEND_POINT_ST].x;
dImage1HStartPoint.y =
    StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][BLEND_POINT_ST].y;
dImage1HEndPoint.x =
    StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][BLEND_POINT_ED].x;
dImage1HEndPoint.y =
    StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][BLEND_POINT_ED].y;

// [읽기] AKCFAIInspector.cpp - UI 참조점 Cross 마커 표시
GenCrossContourXld(&HCrossXld,
    StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][0].y,
    StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][0].x,
    60.0, 0);

// [쓰기] AlgorithmThreadDefine.h - 검사 스레드에서 edge 선분 중앙점 저장
measureStruct.m_dBlendCPoint[iPcVisionNo][pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].x =
    (m_dEdgeLineStartX + m_dEdgeLineEndX) * 0.5;
measureStruct.m_dBlendCPoint[iPcVisionNo][pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].y =
    (m_dEdgeLineStartY + m_dEdgeLineEndY) * 0.5;
```

---

## 4. 데이터 흐름 요약

```
검사 스레드 (AlgorithmThreadDefine.h)
    │
    │  edge/blob 감지 결과 저장
    ▼
CenterlineMeasureStruct
  ├── m_dMeasureCPoint  ← 중심점 좌표 (DPOINT[63][8])
  ├── m_HMeasureXLD     ← 감지 선분/Contour (HObject[63][8])
  ├── m_HMeasureRgn     ← 감지 영역/Blob   (HObject[63][8])
  └── m_dBlendCPoint    ← 블렌드 기준선 좌표 (DPOINT[4][8][2])
    │
    ├─── FAI Inspector (읽기) ──────────────────────────────────
    │     ├── BOIFAIInspector  : 렌즈/Datum 중심점, 선분 계산 및 UI 표시
    │     ├── ACTFAIInspector  : Fiducial 라인 거리 계산 및 표시
    │     ├── ATWFAIInspector  : 렌즈 포인트 원 맞춤 계산
    │     ├── AKCFAIInspector  : 블렌드 포인트로 이미지 정렬각 계산
    │     └── BOSFAIInspector  : Chassis hole Region 유효성 검증
    │
    └─── UI 표시 ────────────────────────────────────────────────
          m_HMeasureXLD → ConcatObj → m_HReviewXLD_FAI_Item → 리뷰 화면
```

---

## 5. 변수 간 관계

`m_HMeasureRgn`과 `m_dMeasureCPoint`는 **쌍으로 사용**되는 경우가 많음:
- Region이 유효한(`ValidHRegion()`이 TRUE인) 경우에만 해당 위치의 `m_dMeasureCPoint`를 참조
- Region은 Blob 존재 여부 확인용, CPoint는 실제 좌표 계산용

`m_HMeasureXLD`는 독립적으로 사용:
- 선분을 직접 저장하고 리뷰 화면에 병합 표시
- `m_dMeasureCPoint`와 같은 위치를 인덱싱하지만 별도로 관리

`m_dBlendCPoint`는 **AKC 모델 전용**:
- 다른 Inspector에서는 미사용
- 멀티카메라 이미지 스티칭 시 정렬 기준선 좌표 보관

---

## 6. 파일별 역할 정리

| 파일 | 역할 |
|------|------|
| `FAI/ModelData/FAIMeasureData.h` | 구조체 정의 및 `ResetFAI()` 초기화 |
| `FAI/FAIDefine.h` | `enMeasurePos`, `enBlendPos`, 상수, `MtoI()`/`ItoM()` 변환 함수 |
| `FAI/ModelData/FAIDataManager.h` | `CenterlineMeasureStruct` 접근 인터페이스 (Singleton) |
| `AlgorithmThreadDefine.h` | **핵심 쓰기**: 검사 스레드에서 측정 결과 저장 |
| `TabControlDlg.cpp` | **교육 모드 쓰기**: Teaching 측정 결과 저장 |
| `FAI/ModelTypes/BOIFAIInspector.cpp` | BOI 모델 FAI 계산 (읽기/쓰기) |
| `FAI/ModelTypes/ACTFAIInspector.cpp` | ACT 모델 FAI 계산 (읽기/쓰기) |
| `FAI/ModelTypes/ATWFAIInspector.cpp` | ATW 모델 FAI 계산 (읽기/쓰기) |
| `FAI/ModelTypes/AKCFAIInspector.cpp` | AKC 모델 블렌드 포인트 계산 (읽기) |
| `FAI/ModelTypes/BOSFAIInspector.cpp` | BOS 모델 영역 검증 (읽기) |

---

## 7. 요약 테이블

| 변수 | 타입 | 선언 라인 | 주요 쓰기 | 주요 읽기 | 핵심 용도 |
|------|------|:---------:|---------|---------|---------|
| `m_dMeasureCPoint` | `DPOINT[63][8]` | 27 | `AlgorithmThreadDefine.h`, `TabControlDlg.cpp` | 모든 FAI Inspector | 측정점 중심 좌표 저장/계산 |
| `m_HMeasureXLD` | `HObject[63][8]` | 28 | BOI/ACT/ATW FAI Inspector | BOI/ACT/ATW FAI Inspector | 감지 선분 저장 및 리뷰 UI 표시 |
| `m_HMeasureRgn` | `HObject[63][8]` | 29 | `AlgorithmThreadDefine.h`, `TabControlDlg.cpp` | `BOSFAIInspector` | 감지 영역 저장 및 유효성 검증 |
| `m_dBlendCPoint` | `DPOINT[4][8][2]` | 30 | `AlgorithmThreadDefine.h` | `AKCFAIInspector` | 블렌드 기준 선분 좌표, 이미지 정렬각 계산 |
