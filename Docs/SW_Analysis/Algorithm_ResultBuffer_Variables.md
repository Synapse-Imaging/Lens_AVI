# Algorithm 결과 버퍼 변수 가이드

> **대상 파일**: `Algorithm.h` L240~L251
> **작성일**: 2026-03-11 | **작성자**: 비전SW 김준호

---

## 개요

`Algorithm` 클래스는 멀티 스레드 검사 결과를 **다차원 HObject 배열**에 누적합니다.
같은 결함 영역 데이터를 **목적별로 다르게 인덱싱**하는 3단계 집계 구조를 사용합니다.

```
[검사 스레드 결과 누적] → _DefectName[buf][type][code]   ← 가장 세분화된 분류
                        ↓ Union 합산
                        → _InspectionType[buf][type]      ← 검사 유형 단위 (Result 이미지)
                        ↓ 이미지 번호별 재분류
                        → _ImageNo[buf][imgNo]             ← ADJ(딥러닝) 이미지 전송용
```

---

## 상수 정의 (관련 크기 참고)

| 상수 | 값 | 정의 위치 | 의미 |
|---|---|---|---|
| `INSPECTION_BUFFER_COUNT_MAX` | 10 | `Define.h:143` | 동시 처리 가능한 검사 버퍼 수 |
| `MAX_INSPECTION_TYPE` | 23 | `Define.h:408` | 검사 유형 수 (바코드, 표면, 에폭시...) |
| `MAX_DEFECT_NAME` | 110 | `Define.h:676` | 결함 코드 최대 수 |
| `MAX_IMAGE_TAB` | 45 | `Define.h:260` | 이미지 탭(번호) 최대 수 |
| `MAX_FAI_ITEM` | 151 | `FAI/FAIDefine.h:52` | FAI 항목 최대 수 (`TOTAL_FAI_NUMBER`) |
| `MAX_FAI_PARAMETER` | 1 | `Define.h:1143` | FAI 파라미터 수 (3→1로 축소됨) |
| `MAX_FAI_REVIEW_IMAGE` | 10 | `FAI/FAIDefine.h:55` | FAI 리뷰 이미지 수 |
| `FAI_PARAMETER_NUMBER_1` | 0 | `FAI/FAIDefine.h:68` | 첫 번째 파라미터 인덱스 |
| `DEFECT_CODE_FAI_NG` | 70 | `Define.h:680` | FAI NG 결함 코드 |

---

## 변수 상세 설명

### 1. `m_HDefectRgn_DefectName[buf][inspType][defectCode]`

```cpp
HObject m_HDefectRgn_DefectName[INSPECTION_BUFFER_COUNT_MAX][MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];
// 크기: [10][23][110]
```

**용도**: 검사 스레드에서 발생한 결함 영역을 **검사 유형 × 결함 코드** 조합으로 누적 저장.
가장 세분화된 결과 버퍼. 나머지 모든 집계 변수의 **원본 데이터 소스**.

**초기화**:
```cpp
// Algorithm.cpp ~L95
GenEmptyObj(&(m_HDefectRgn_DefectName[i][k][l]));
```

**쓰기** (AlgorithmThreadDefine.h 내 검사 스레드):
```cpp
// ROI 검사 알고리즘 실행 후 결함 영역 누적
ConcatObj(
    pAlgorithm->m_HDefectRgn_DefectName[iCurInspectionBufferIdx]
                                        [pInspectROIRgn->miInspectionType - 1]        // 1-based → 0-based
                                        [pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType],
    HDefectRgn,    // 이번 ROI에서 발견된 결함 영역
    &(pAlgorithm->m_HDefectRgn_DefectName[iCurInspectionBufferIdx]
                                          [pInspectROIRgn->miInspectionType - 1]
                                          [pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType])
);
```

**읽기**:
- **결과 이미지 생성**: 검사 유형별 결과 이미지에 결함 영역 overlay
- **ADJ 분류기 전송** (`AlgorithmThreadDefine.h ~L1927`): 딥러닝 AI 재분류 요청 시 결함 이미지 crop
- **FAI NG 표시** (`AlgorithmThreadDefine.h ~L15543`): `DEFECT_CODE_FAI_NG` 코드로 FAI 결과 통합

> **주의**: `miInspectionType`은 1-based이므로 인덱스 접근 시 항상 `-1` 필요.

---

### 2. `m_HDefectRgn_InspectionType[buf][inspType]`

```cpp
HObject m_HDefectRgn_InspectionType[INSPECTION_BUFFER_COUNT_MAX][MAX_INSPECTION_TYPE];
// 크기: [10][23]
```

**용도**: `m_HDefectRgn_DefectName`의 특정 검사 유형 내 **모든 결함 코드를 합산**한 영역.
Result 이미지에 검사 유형 단위 결함을 한 번에 overlay할 때 사용.

**파생 관계**:
```
m_HDefectRgn_DefectName[buf][i][0..MAX_DEFECT_NAME]
    → ConcatObj 합산 → Union1 정규화
    → m_HDefectRgn_InspectionType[buf][i]
```

**생성 코드** (AlgorithmThreadDefine.h ~L15547):
```cpp
for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
{
    GenEmptyObj(&(pAlgorithm->m_HDefectRgn_InspectionType[iCurInspectionBufferIdx][i]));
    for (int j = 0; j < MAX_DEFECT_NAME; j++)
    {
        ConcatObj(pAlgorithm->m_HDefectRgn_InspectionType[iCurInspectionBufferIdx][i],
                  pAlgorithm->m_HDefectRgn_DefectName[iCurInspectionBufferIdx][i][j],
                  &(pAlgorithm->m_HDefectRgn_InspectionType[iCurInspectionBufferIdx][i]));
    }
    if (CGFunction::ValidHRegion(pAlgorithm->m_HDefectRgn_InspectionType[iCurInspectionBufferIdx][i]))
        Union1(pAlgorithm->m_HDefectRgn_InspectionType[iCurInspectionBufferIdx][i],
               &(pAlgorithm->m_HDefectRgn_InspectionType[iCurInspectionBufferIdx][i]));
}
```

**읽기**: Result Image 생성 시 각 검사 유형의 결함 영역을 이미지에 그리기.

---

### 3. `m_HDefectRgn_ImageNo[buf][imageNo]`

```cpp
HObject m_HDefectRgn_ImageNo[INSPECTION_BUFFER_COUNT_MAX][MAX_IMAGE_TAB];
// 크기: [10][45]
```

**용도**: 결함 영역을 **이미지 번호(탭 번호) 기준**으로 재분류.
ADJ(딥러닝 분류기)에 결함 이미지를 전송할 때, 어느 이미지에서 발생한 결함인지 매핑하기 위해 사용.

**쓰기** (AlgorithmThreadDefine.h ~L482):
```cpp
// SYAI 결과 처리 시 이미지 인덱스별 누적
ConcatObj(algo->m_HDefectRgn_ImageNo[thread_buffer][image_index],
          restored_region,
          &(algo->m_HDefectRgn_ImageNo[thread_buffer][image_index]));
```

**읽기** (ADJClientService.cpp ~L1379):
```cpp
// 이미지 번호 iImageNo의 결함 유효성 확인 후 ADJ 전송
if (ValidHRegion(pInspectAlgorithm.m_HDefectRgn_ImageNo[bufIdx][iImageNo]) == FALSE)
    continue;

// 특정 ROI와 교집합으로 해당 이미지의 결함만 추출
Intersection(pInspectAlgorithm.m_HDefectRgn_ImageNo[bufIdx][iImageNo],
             m_HRosReviewSpecApplyDefectRgn[x][y],
             &HIntersectRgn);
```

---

### 4. `m_HMeasureRgn_FAI_Item[buf][faiItem][param]`

```cpp
HObject m_HMeasureRgn_FAI_Item[INSPECTION_BUFFER_COUNT_MAX][MAX_FAI_ITEM][MAX_FAI_PARAMETER];
// 크기: [10][151][1]   — MAX_FAI_PARAMETER = 1 (현재)
```

**용도**: FAI 측정 항목별 **측정 ROI Region** 저장.
Algorithm 화면에서 FAI 측정 영역 시각화, FAI NG 영역 통합에 사용.

**쓰기** (AlgorithmThreadDefine.h ~L11402):
```cpp
// FAI 알고리즘에서 측정 영역 생성 후 누적
ConcatObj(pAlgorithm->m_HMeasureRgn_FAI_Item[iCurInspectionBufferIdx][iFAINumber][iFaiParamIndex],
          HFAIMeasureRgn,    // 이번 FAI 측정에서 사용된 ROI 영역
          &pAlgorithm->m_HMeasureRgn_FAI_Item[iCurInspectionBufferIdx][iFAINumber][iFaiParamIndex]);
```

**읽기** (FAI NG 통합, AlgorithmThreadDefine.h ~L14029):
```cpp
// FAI NG 판정된 항목의 영역을 m_HDefectRgn_FAI에 추가
if (CGFunction::ValidHRegion(pAlgorithm->m_HMeasureRgn_FAI_Item[iCurInspectionBufferIdx][i][FAI_PARAMETER_NUMBER_1]))
    ConcatObj(pAlgorithm->m_HDefectRgn_FAI[iCurInspectionBufferIdx],
              pAlgorithm->m_HMeasureRgn_FAI_Item[iCurInspectionBufferIdx][i][FAI_PARAMETER_NUMBER_1],
              &(pAlgorithm->m_HDefectRgn_FAI[iCurInspectionBufferIdx]));
```

> **주의**: 현재 `MAX_FAI_PARAMETER = 1`이므로 항상 `FAI_PARAMETER_NUMBER_1 (= 0)` 인덱스만 사용.

---

### 5. `m_HDefectRgn_FAI[buf]`

```cpp
HObject m_HDefectRgn_FAI[INSPECTION_BUFFER_COUNT_MAX];
// 크기: [10]
```

**용도**: 모듈 단위 **FAI NG 영역 최종 통합**. 모든 FAI 항목의 NG 영역을 하나로 합산.

**생성 패턴** (AlgorithmThreadDefine.h ~L14029):
```cpp
// [1] 초기화
GenEmptyObj(&(pAlgorithm->m_HDefectRgn_FAI[iCurInspectionBufferIdx]));

// [2] NG 항목의 측정 영역 누적
for (int i = 0; i < MAX_FAI_ITEM; i++)
{
    if (/* FAI-i가 NG */)
    {
        ConcatObj(pAlgorithm->m_HDefectRgn_FAI[iCurInspectionBufferIdx],
                  pAlgorithm->m_HMeasureRgn_FAI_Item[iCurInspectionBufferIdx][i][FAI_PARAMETER_NUMBER_1],
                  &(pAlgorithm->m_HDefectRgn_FAI[iCurInspectionBufferIdx]));
    }
}

// [3] 정규화
if (ValidHRegion(pAlgorithm->m_HDefectRgn_FAI[iCurInspectionBufferIdx]))
    Union1(pAlgorithm->m_HDefectRgn_FAI[iCurInspectionBufferIdx],
           &(pAlgorithm->m_HDefectRgn_FAI[iCurInspectionBufferIdx]));
```

**최종 사용** (AlgorithmThreadDefine.h ~L15543): FAI NG 결과를 표준 결함 버퍼로 이전:
```cpp
// FAI NG 영역을 바코드 검사 유형의 DEFECT_CODE_FAI_NG 코드로 등록
Union1(pAlgorithm->m_HDefectRgn_FAI[iCurInspectionBufferIdx],
       &(pAlgorithm->m_HDefectRgn_DefectName[iCurInspectionBufferIdx]
                                             [INSPECTION_TYPE_BARCODE - 1]
                                             [DEFECT_CODE_FAI_NG]));
```

---

### 6. `m_HReviewXLD_FAI_Item[buf][faiItem][param][reviewImgIdx]`

```cpp
HObject m_HReviewXLD_FAI_Item[INSPECTION_BUFFER_COUNT_MAX][MAX_FAI_ITEM][MAX_FAI_PARAMETER][MAX_FAI_REVIEW_IMAGE];
// 크기: [10][151][1][10]
```

**용도**: FAI 항목별 **리뷰 이미지에 그릴 XLD(Extended Line Description) 오버레이** 저장.
십자 마크, 방향선, 측정 결과 라인 등 시각화 요소를 XLD로 표현.

**초기화**:
```cpp
// Algorithm.cpp ~L112
GenEmptyObj(&(m_HReviewXLD_FAI_Item[i][j][k][l]));
```

**쓰기** (FAI Inspector 구현 내부 — 예: `BOSFAIInspector.cpp`):
```cpp
// HALCON XLD 생성 후 ConcatObj로 누적 (여러 요소 겹쳐 그리기)
HObject HCrossLensCenter;
GenCrossContourXld(&HCrossLensCenter,
                   dInnerCircleCoord.y, dInnerCircleCoord.x,  // row, col (y 먼저!)
                   30,   // 크기
                   0);   // 각도

ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFaiNum][iFAIParamNo][0],
          HCrossLensCenter,
          &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFaiNum][iFAIParamNo][0]);

// 방향선 추가
HObject HDatumELine;
HTuple HLineRows, HLineCols;
TupleConcat(HTuple(startY), HTuple(endY), &HLineRows);
TupleConcat(HTuple(startX), HTuple(endX), &HLineCols);
GenContourPolygonXld(&HDatumELine, HLineRows, HLineCols);

ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFaiNum][iFAIParamNo][0],
          HDatumELine,
          &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFaiNum][iFAIParamNo][0]);
```

**읽기** (AlgorithmThreadDefine.h ~L2156): 리뷰 이미지 저장 스레드에서:
```cpp
// 유효성 확인 후 복사 (저장 스레드로 전달)
if (CGFunction::ValidHXLD(pInspectAlgorithm.m_HReviewXLD_FAI_Item[bufIdx][i][j][k]))
    HReviewXLD_FAI_Item_Copied[i][j * MAX_FAI_REVIEW_IMAGE + k] =
        pInspectAlgorithm.m_HReviewXLD_FAI_Item[bufIdx][i][j][k];
```

> **주의**:
> - HALCON 좌표 컨벤션은 **row(Y) 먼저** — `GenCrossContourXld(obj, row, col, ...)`
> - XLD 유효성은 `ValidHXLD()` 사용 (`ValidHRegion()`과 구분)

---

### 7. `m_iViewportCenter_FAI_Item[buf][faiItem][param][reviewImgIdx]`

```cpp
POINT m_iViewportCenter_FAI_Item[INSPECTION_BUFFER_COUNT_MAX][MAX_FAI_ITEM][MAX_FAI_PARAMETER][MAX_FAI_REVIEW_IMAGE];
// 크기: [10][151][1][10]
```

**용도**: FAI 리뷰 이미지 저장 시 **화면 뷰포트 중심 좌표**를 지정.
리뷰 이미지에서 관심 영역(측정 포인트)이 화면 중앙에 오도록 crop/zoom 좌표 제공.

**초기화**:
```cpp
// Algorithm.cpp ~L113 — (-1, -1) = 미설정 상태
m_iViewportCenter_FAI_Item[i][j][k][l].x = -1;
m_iViewportCenter_FAI_Item[i][j][k][l].y = -1;
```

**쓰기** (FAI Inspector에서 측정 중심점 설정):
```cpp
// 측정 기준점(Datum)을 뷰포트 중심으로 설정
m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFaiNum][iFAIParamNo][0].x
    = (int)dInnerCircleCoord.x;
m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFaiNum][iFAIParamNo][0].y
    = (int)dInnerCircleCoord.y;
```

**읽기**: 저장 스레드에서 뷰포트 좌표와 함께 리뷰 이미지 crop 위치 결정.

---

## Multi-Focus (Coax 동축조명) 전용 변수

Coax 카메라(동축조명)로 Multi-Focus 촬영 시 사용하는 **표준 버퍼의 별도 복사본**.
구조와 사용 패턴은 표준 `m_HDefectRgn_*` 그룹과 동일하지만 Multi-Focus 처리 경로에서만 쓰임.

### 8. `m_HDefectRgn_MultiFocus_Ring_Particle[buf][imgNo][inspType][defectCode]`
```cpp
HObject m_HDefectRgn_MultiFocus_Ring_Particle[INSPECTION_BUFFER_COUNT_MAX][MAX_IMAGE_TAB][MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];
// 크기: [10][45][23][110]
```
- 이미지 번호 차원이 추가된 4D 배열
- `m_HDefectRgn_DefectName`의 Multi-Focus 버전
- 렌즈 링/파티클 결함 검사에 사용

### 9. `m_HDefectRgn_Lens_Ring[buf]`
```cpp
HObject m_HDefectRgn_Lens_Ring[INSPECTION_BUFFER_COUNT_MAX];
```
- 렌즈 링 영역 전체 결함을 모듈 단위로 통합
- `m_HDefectRgn_FAI`와 유사한 역할 (Multi-Focus 경로)

### 10. `m_HDefectRgn_ImageNo_Coax[buf][imgNo]`
```cpp
HObject m_HDefectRgn_ImageNo_Coax[INSPECTION_BUFFER_COUNT_MAX][MAX_IMAGE_TAB];
```
- Coax 이미지 번호별 결함 영역 (`m_HDefectRgn_ImageNo`의 Coax 버전)

### 11. `m_HDefectRgn_DefectName_Coax[buf][inspType][defectCode]`
```cpp
HObject m_HDefectRgn_DefectName_Coax[INSPECTION_BUFFER_COUNT_MAX][MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];
```
- Coax 검사 유형 × 결함 코드별 영역 (`m_HDefectRgn_DefectName`의 Coax 버전)

---

## 전체 데이터 흐름 요약

```
[검사 스레드 - ConcatObj 누적]
    ↓ 각 ROI 알고리즘 실행 시 마다
    m_HDefectRgn_DefectName[buf][type-1][defectCode]  ← 원자 단위
    m_HDefectRgn_ImageNo[buf][imageNo]                ← 이미지 번호 기준
    m_HMeasureRgn_FAI_Item[buf][faiItem][0]           ← FAI 측정 영역
    m_HReviewXLD_FAI_Item[buf][faiItem][0][imgIdx]    ← FAI XLD 오버레이
    m_iViewportCenter_FAI_Item[buf][faiItem][0][imgIdx] ← 뷰포트 중심

[검사 완료 후 집계 단계]
    m_HDefectRgn_DefectName → Union 합산 → m_HDefectRgn_InspectionType  ← Result 이미지용
    m_HMeasureRgn_FAI_Item(NG만) → ConcatObj → m_HDefectRgn_FAI         ← FAI NG 통합
    m_HDefectRgn_FAI → Union1 → m_HDefectRgn_DefectName[BARCODE-1][FAI_NG]  ← 표준 결함 코드 등록

[소비자]
    → Result 이미지 생성: m_HDefectRgn_InspectionType
    → ADJ 딥러닝 전송: m_HDefectRgn_ImageNo (이미지 기준 crop)
    → FAI 결과 판정: m_HDefectRgn_FAI
    → 리뷰 이미지 저장: m_HReviewXLD_FAI_Item + m_iViewportCenter_FAI_Item
```

---

## 공통 사용 패턴 정리

### 초기화
```cpp
GenEmptyObj(&target);                    // HObject 초기화 (빈 객체)
target.x = -1; target.y = -1;           // POINT 초기화 (미설정 표시)
```

### 누적 (Append)
```cpp
ConcatObj(existing, newObj, &existing);  // 기존 + 신규 영역 합산 (중복 포함)
```

### 정규화 (Normalize)
```cpp
Union1(existing, &existing);             // 중복 픽셀 제거, 단일 Region으로 병합
```

### 유효성 검사
```cpp
CGFunction::ValidHRegion(hObj)           // Region 유효성 (비어있지 않음)
CGFunction::ValidHXLD(hObj)              // XLD 유효성 (비어있지 않음)
```
