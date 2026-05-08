# FAI 측정 패턴 정리 (AKC 기준)

## 문서 설명
- 추후 FAI 계산 함수 (ModelTpyes 폴더) 를 리팩토링하기 위한 자료 조사
- FAI 계산은 몇 가지의 패턴이 반복되므로, 어떤 패턴이 있는지 조사하여 모듈화 예정

## 범위
- 대상 코드: `FAI/ModelTypes/AKCFAIInspector.cpp`
- 기준 함수: `AKCFAIInspector::Inspect(...)`
- 목적: FAI 항목을 계산 방식(측정 패턴) 단위로 분류해 리팩토링 기준으로 사용

---

## 측정 패턴 목록

| 패턴 ID | 패턴명 | 판정 기준 | 대표 FAI | 짧은 근거 |
|---|---|---|---|---|
| P1 | 점-직선 단일 투영 거리 | 점 1개를 기준선/축에 투영해 거리 1개 저장 | 21, 15, 43, 44, 113 | `DistancePl(...)` 1회 후 바로 `m_dFAIMeasureValue[iFAINumber][0]` 기록 |
| P2 | 선분-축 평균 투영 거리 | 시작점/끝점을 각각 투영 후 평균으로 저장 | 19, 20, 70, 81, 119, 120 | `DistancePl(...)` 2회 + `(start + end) * 0.5` 패턴 |
| P3 | 원 피팅 기반 파생 계산 | 에지 점군으로 원 중심/반경 추정 후 파생값 계산 | 14, 21, 15 | `LeastSquareCircleFitting(...)` 성공 후 중심 기반 거리 계산 + FAI-14 수식 결합 |
| P4 | 영역-영역 최소거리 | 두 Region 간 최소거리 사용 | 126~130 | `DistanceMinPointAlgorithm(...)` 결과 `dDistanceMin` 저장 |
| P5 | 점-점 거리/원 반경 | 점-점 직접 거리 또는 Region 내접원 반경 사용 | 146, 148, 147, 149 | `DistancePp(...)` 또는 `InnerCircle(...)` 결과 사용 |
| P6-A | 리뷰 XLD 생성 | 리뷰용 HALCON XLD Object 생성 | 140~145, ACT Review XLD | `Gen~Xld(...)` 계열 호출로 오브젝트 생성 (`GenCrossContourXld`, `GenContourPolygonXld`) |
| P6-B | 리뷰 XLD 축적 | 생성/측정된 XLD를 리뷰 버퍼에 누적 | 전반 | `ConcatObj(...)`로 `m_HReviewXLD_FAI_Item` 누적 |
| P7 | 선-선 각도 | 두 선분(기준선/피처선) 각도를 계산해 저장 | 10, 29 | `AngleLl(...)` + `TupleDeg(...)` 후 `m_dFAIMeasureValue` 기록 |

---

## 패턴별 코드 근거 위치

### P1. 점-직선 단일 투영 거리
- FAI-21/15: 원 중심을 축에 투영 (`DistancePl`) 후 저장
- FAI-43/44: 원 중심과 datum line 간 거리 (`DistancePl`) 후 저장
- FAI-113: 커넥터 중심 X축 투영 거리 저장

### P2. 선분-축 평균 투영 거리
- FAI-19/20: VCM 선분 양끝을 축에 투영한 평균
- FAI-70/81/119/120: FPCB 각 선분 양끝 투영 평균

### P3. 원 피팅 기반 파생 계산
- 렌즈 내부 점군 결합 → `LeastSquareCircleFitting(...)`
- 피팅 중심으로 FAI-21(X), FAI-15(Y) 계산
- FAI-14는 FAI-21/15 기반 파생 수식으로 최종 계산

### P4. 영역-영역 최소거리
- Welding Spot Region vs Welding Pocket Region 최소거리(`DistanceMinPointAlgorithm`)를 126~130에 순차 기록

### P5. 점-점 거리/원 반경
- FAI-146/148: hole point 간 `DistancePp(...)`
- FAI-147/149: hole region에 `InnerCircle(...)` 적용 후 반경 사용

### P6-A. 리뷰 XLD 생성
- BOI/AKC: `GenCrossContourXld(...)`로 cross XLD 생성
- ACT: `// Review XLD` 블록에서 `GenContourPolygonXld(...)`로 선분 기반 XLD 생성

### P6-B. 리뷰 XLD 축적
- 생성된 XLD 또는 측정 XLD를 `ConcatObj(...)`로 리뷰 버퍼에 누적
- 주요 타깃: `m_HReviewXLD_FAI_Item[...]`

### P7. 선-선 각도
- 두 선분 간 각도 측정 후 도(degree) 변환하여 저장
- `AngleLl(...)` 결과를 `TupleDeg(...)`로 변환 후 `m_dFAIMeasureValue` 기록

---

## 리팩토링 적용 가이드 (1차)
1. P1/P2를 공통 유틸로 우선 분리: "투영거리 1점" / "투영거리 2점 평균"
2. P3는 별도 파이프라인 유지: 점군 수집 → 피팅 → 파생값 계산
3. P4/P5는 알고리즘 래퍼 함수화: 최소거리/반경/점간거리 계산 래핑
4. P6-A/P6-B는 측정 로직과 분리: Review XLD 생성 helper / 축적 helper로 분리
5. P7은 각도 측정 공통 유틸 분리: 선분 2개 입력 → 각도/부호 규칙 처리

---

## 모델별 통합 집계 (BOI / AKC / ACT)

- 대상 코드
	- `FAI/ModelTypes/BOIFAIInspector.cpp`
	- `FAI/ModelTypes/AKCFAIInspector.cpp`
	- `FAI/ModelTypes/ACTFAIInspector.cpp`
- 집계 기준: 각 패턴의 핵심 함수 호출/식(문서 정의) 발생 횟수

| 패턴 ID | BOI | AKC | ACT | 모델별 예시 (1~2줄) |
|---|---:|---:|---:|---|
| P1 (점-직선 단일 투영) | 2 | 20 | 26 | **BOI**: `DistancePl(dMeasurePoint.y, dMeasurePoint.x, ... , &HDistance);`<br>**AKC**: `DistancePl(dCenter_Conn.y, dCenter_Conn.x, ... , &HConnectorX);`<br>**ACT**: `DistancePl(dPoint1.y, dPoint1.x, ... , &HDistance);` |
| P2 (선분-축 평균 투영) | 0 | 6 | 0 | **AKC**: `DistancePl(...,&HStartPosX_FPCB_South); DistancePl(...,&HEndPosX_FPCB_South);`<br>`(HStartPosX_FPCB_South.D() + HEndPosX_FPCB_South.D()) * 0.5` |
| P3 (원 피팅 기반 파생) | 1 | 1 | 0 | **BOI**: `LeastSquareCircleFitting(HEdgeConcatX, HEdgeConcatY, dNormalRadius, ...);`<br>**AKC**: `LeastSquareCircleFitting(HInnerEdgeConcatX, HInnerEdgeConcatY, dInnerNormalRadius, ...);` |
| P4 (영역-영역 최소거리) | 0 | 1 | 0 | **AKC**: `HLineXld = m_pAlgorithm->DistanceMinPointAlgorithm(HSpotXld, HPocketXld, ... , dDistanceMin);` |
| P5 (점-점 거리/원 반경) | 1 | 5 | 0 | **BOI**: `DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);`<br>**AKC**: `DistancePp(...);` / `InnerCircle(..., &HRowA, &HColA, &HRadA);` |
| P6-A (리뷰 XLD 생성: Gen~Xld) | 3 | 28 | 30 | **BOI**: `GenCrossContourXld(&HCircleCPoint, dCircleCenterY, dCircleCenterX, 30, 0);`<br>**AKC**: `GenCrossContourXld(&HCrossXld, ..., 60.0, 0);`<br>**ACT**: `// Review XLD`<br>`GenContourPolygonXld(&HReviewXLD, ...);` |
| P6-B (리뷰 XLD 축적: ConcatObj) | 8 | 45 | 70 | **BOI**: `ConcatObj(..., pStructFAIMeasure->m_HMeasureXLD[...], ...);`<br>**AKC**: `ConcatObj(..., HCrossXld, ...);`<br>**ACT**: `ConcatObj(..., HReviewXLD, ...);` |
| P7 (선-선 각도) | 1 | 0 | 2 | **BOI**: `AngleLl(dXLineStart.y, dXLineStart.x, ... , &hFAI38);`<br>**ACT**: `AngleLl(dLineStart2.y, dLineStart2.x, ... , &HAngle);` |

## 메모
- 본 문서는 AKC Inspect 단일 함수 기준 분류이며, BOI/ACT/ATW는 공통 패턴 재사용 가능성 검토가 필요함.
- FAI 번호 자체보다 "계산 커널" 중심으로 추상화해야 유지보수 비용이 낮아짐.