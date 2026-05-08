# CHS FAI 측정 패턴 정리

## 범위
- 대상 코드: `FAI/ModelTypes/CHSFAIInspector.cpp` (또는 CHS 관련 Inspect 구현 파일)
- 기준 함수: `CHSFAIInspector::Inspect(...)` (또는 해당 모델 Inspect 엔트리)
- 목적: FAI 항목을 계산 방식(측정 패턴) 단위로 분류해 리팩토링 기준으로 사용

---

## 측정 패턴 목록

| 패턴 ID | 패턴명 | 판정 기준 | 대표 FAI | 짧은 근거 |
|---|---|---|---|---|
| P1 | 점-직선 단일 투영 거리 | 점 1개를 기준선/축에 투영해 거리 1개 저장 | TBD | `DistancePl(...)` 1회 후 `m_dFAIMeasureValue` 기록 |
| P2 | 선분-축 평균 투영 거리 | 시작점/끝점을 각각 투영 후 평균으로 저장 | TBD | `DistancePl(...)` 2회 + `(start + end) * 0.5` |
| P3 | 원 피팅 기반 파생 계산 | 에지 점군으로 원 중심/반경 추정 후 파생값 계산 | TBD | `LeastSquareCircleFitting(...)` + 파생 수식 |
| P4 | 영역-영역 최소거리 | 두 Region 간 최소거리 사용 | TBD | `DistanceMinPointAlgorithm(...)` 결과 저장 |
| P5 | 점-점 거리/원 반경 | 점-점 직접 거리 또는 Region 내접원 반경 사용 | TBD | `DistancePp(...)` / `InnerCircle(...)` |
| P6-A | 리뷰 XLD 생성 | 리뷰용 HALCON XLD Object 생성 | TBD | `Gen~Xld(...)` 계열 호출로 오브젝트 생성 |
| P6-B | 리뷰 XLD 축적 | 생성/측정된 XLD를 리뷰 버퍼에 누적 | TBD | `ConcatObj(...)`로 `m_HReviewXLD_FAI_Item` 누적 |
| P7 | 선-선 각도 | 두 선분(기준선/피처선) 각도를 계산해 저장 | TBD | `AngleLl(...)` + `TupleDeg(...)` |

---

## 패턴별 코드 근거 위치

### P1. 점-직선 단일 투영 거리
- TBD

### P2. 선분-축 평균 투영 거리
- TBD

### P3. 원 피팅 기반 파생 계산
- TBD

### P4. 영역-영역 최소거리
- TBD

### P5. 점-점 거리/원 반경
- TBD

### P6-A. 리뷰 XLD 생성
- TBD

### P6-B. 리뷰 XLD 축적
- TBD

### P7. 선-선 각도
- TBD

---

## 리팩토링 적용 가이드 (1차)
1. P1/P2를 공통 유틸로 우선 분리: "투영거리 1점" / "투영거리 2점 평균"
2. P3는 별도 파이프라인 유지: 점군 수집 → 피팅 → 파생값 계산
3. P4/P5는 알고리즘 래퍼 함수화: 최소거리/반경/점간거리 계산 래핑
4. P6-A/P6-B는 측정 로직과 분리: Review XLD 생성 helper / 축적 helper로 분리
5. P7은 각도 측정 공통 유틸 분리: 선분 2개 입력 → 각도/부호 규칙 처리

---

## 모델별 통합 집계 (CHS 기준)

- 대상 코드
  - `FAI/ModelTypes/CHSFAIInspector.cpp` (또는 CHS 관련 파일)
- 집계 기준: 각 패턴의 핵심 함수 호출/식(문서 정의) 발생 횟수

| 패턴 ID | CHS | 모델별 예시 (1~2줄) |
|---|---:|---|
| P1 (점-직선 단일 투영) | TBD | TBD |
| P2 (선분-축 평균 투영) | TBD | TBD |
| P3 (원 피팅 기반 파생) | TBD | TBD |
| P4 (영역-영역 최소거리) | TBD | TBD |
| P5 (점-점 거리/원 반경) | TBD | TBD |
| P6-A (리뷰 XLD 생성: Gen~Xld) | TBD | TBD |
| P6-B (리뷰 XLD 축적: ConcatObj) | TBD | TBD |
| P7 (선-선 각도) | TBD | TBD |

## 메모
- 본 문서는 CHS 모델 기준 분류 초안이며, 실제 구현 파일명/함수명에 맞춰 갱신 필요.
- FAI 번호 자체보다 "계산 커널" 중심으로 추상화해야 유지보수 비용이 낮아짐.
