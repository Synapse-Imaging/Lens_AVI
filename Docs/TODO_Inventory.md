# Universal_AVI TODO 인벤토리

> 생성일: 2026-03-12
> 검색 범위: 프로젝트 루트 `*.cpp`, `*.h` (Include/ 외부 라이브러리 제외)
> 총 TODO 수: 55개

---

## 1. 기술 부채 — 실제 조치 필요 (10건)

개발자가 직접 작성한 TODO로, 향후 리팩토링 또는 수정이 필요한 항목입니다.

| # | 파일 | 줄 | 우선순위 | 내용 |
|---|---|---|---|---|
| 1 | `Define.h` | 328 | **High** | 조건부 컴파일(`#ifdef UAVI_*`)로 분기된 `MAX_TRAY_LOT`, `MAX_MODULE_TRAY` 상수를 모델별로 분리해야 함. FAI 정의와 dangling 관계 존재 |
| 2 | `FAI/FAIInspectorFactory.cpp` | 20 | **Medium** | `ReplaceFAIInspector()`에서 `new`로 할당 후 `delete`로 해제 — 스마트 포인터(`unique_ptr`) 전환 고려 (260304, jhkim) |
| 3 | `FAI/ModelData/FAIDataManager.cpp` | 150 | **Medium** | `TODO2` 미완성 — position → FAI 번호 매핑 로직에서 모델 정의에 따라 FAI 번호를 동적으로 결정할 수 있도록 개선 필요 |
| 4 | `FAI/ModelData/FAIMeasureData.h` | 33 | **Low** | CHS 전용 측정 포인트(`m_dFAI7_WestPoint` 등)가 공통 `FAIMeasureData` 구조체에 혼재 — CHS 전용 구조체로 분리 고려 |
| 5 | `FAI/ModelData/FAIModelConfig.h` | 44 | **Low** | `FAIItemDescriptor::logHeaderNames`가 `std::vector<LPCTSTR>`로 선언되어 있으나 실제 사용은 단일 값 — `LPCTSTR` 단일 자료형으로 수정 검토 |
| 6 | `FAI/ModelData/FAIModelConfig.h` | 56 | **Low** | `FAIModelConfig::extraLogHeaders`를 벡터로 선언. 모델별 추가 LOG 헤더 지원 필요 시 확장 가능성 검토 |
| 7 | `ModelTypeList.h` | 12 | **Medium** | INI 파일(`ModelNameCode.ini`)과 SW 코드의 모델 타입 정의가 불일치할 경우를 방지하는 방안 필요. 모델 타입 추가 시 INI 파일도 자동 동기화 필요 |
| 8 | `LGIT/ModelInferenceSVM.cpp` | 166–167 | **Low** | `'-' → '_'` 치환 임시 코드 주석 처리됨 — 정식 적용 또는 삭제 결정 필요 |
| 9 | `LGIT/ModelInferenceVAL.cpp` | 121–122 | **Low** | 동일한 `'-' → '_'` 치환 임시 코드 (ModelInferenceSVM과 동일) — 정식 적용 또는 삭제 결정 필요 |
| 10 | `Enscrypt.h` | 74 | **Low** | `certutil -hashfile S_Universal_AVI.dll md5` — 파일 해시 검증 절차 문서화 또는 자동화 필요 |

---

## 2. MFC 자동 생성 보일러플레이트 (41건)

Visual Studio가 이벤트 핸들러/초기화 함수 생성 시 자동으로 삽입하는 TODO 주석입니다.
별도 코드 작성 없이 빈 핸들러로 남겨진 경우가 대부분이며, **일반적으로 별도 조치 불필요**합니다.

| 파일 | 줄 번호 | 내용 |
|---|---|---|
| `AlgorithmComponentDlg.cpp` | 175 | 추가 초기화 작업 |
| `AutoFocusDlg.cpp` | 441 | 컨트롤 알림 처리기 |
| `GrabSequenceDlg.cpp` | 411 | 컨트롤 알림 처리기 |
| `GrabSequenceViewDlg.cpp` | 165 | 메시지 처리기 |
| `HWinPanel.cpp` | 67 | 메시지 처리기 |
| `InspectAdminViewDlg.cpp` | 5712 | 커맨드 핸들러 |
| `InspectAdminViewToolbarDlg.cpp` | 422 | 커맨드 핸들러 |
| `InspectionTypeExDlg.cpp` | 81 | 특수화 코드 |
| `InspectLibraryDlg.cpp` | 169, 612, 632 | 메시지/컨트롤 알림 처리기 |
| `LightLibraryDlg.cpp` | 138 | 메시지 처리기 |
| `MainFrm.cpp` | 46, 75, 93 | 초기화, 툴바, Window 클래스 |
| `MotionControlDlg.cpp` | 271 | 컨트롤 알림 처리기 |
| `RoiAutoCreateDlg.cpp` | 98 | 컨트롤 알림 처리기 |
| `RoiCopyOptionDlg.cpp` | 99, 106 | 컨트롤 알림 처리기 |
| `StageSelectDlg.cpp` | 55, 85 | 컨트롤 알림 처리기 |
| `TeachFaiAtwDlg.cpp` | 72, 85, 135, 231 | 초기화/컨트롤 알림 처리기 |
| `ThresholdHistogramDlg.cpp` | 144, 289 | 초기화/메시지 처리기 |
| `uScan.cpp` | 74, 524, 1696, 10481, 10646, 10656 | 초기화, 문자열, 특수화 코드 |
| `uScanDoc.cpp` | 32, 45, 60, 64 | 초기화/로드/저장 |
| `uScanView.cpp` | 58, 209, 2174, 2218 | 초기화/컨트롤 알림 처리기 |

---

## 3. 주석 처리된 임시 코드 (4건)

현재 비활성화 상태이나, 정식 적용 또는 삭제 결정이 필요한 코드입니다.

| 파일 | 줄 | 상태 | 설명 |
|---|---|---|---|
| `LGIT/ModelInferenceSVM.cpp` | 155–175 | 주석 처리됨 | `product_name`에서 `'-'`를 `'_'`로 치환하는 임시 로직 (`#pragma region 임시`) |
| `LGIT/ModelInferenceVAL.cpp` | 110–130 | 주석 처리됨 | 동일한 치환 로직 (`ModelInferenceSVM`과 중복) |

---

## 4. 우선순위별 작업 요약

### High (즉시 검토)
- `Define.h:328` — 조건부 컴파일 모델 분리 (FAI 정의와 dangling 관계)

### Medium (단기 개선)
- `FAI/FAIInspectorFactory.cpp:20` — 스마트 포인터 전환
- `FAI/ModelData/FAIDataManager.cpp:150` — `TODO2` 로직 완성
- `ModelTypeList.h:12` — INI ↔ SW 모델 타입 동기화 방안

### Low (중장기 리팩토링)
- `FAI/ModelData/FAIMeasureData.h:33` — CHS 전용 멤버 분리
- `FAI/ModelData/FAIModelConfig.h:44,56` — 자료형/벡터 구조 개선
- `LGIT/ModelInferenceSVM.cpp`, `ModelInferenceVAL.cpp` — 임시 코드 정리
- `Enscrypt.h:74` — 해시 검증 절차 자동화

---

## 5. 개정 내역

| 날짜 | 작성자 | 내용 |
|---|---|---|
| 2026-03-12 | 비전SW 김준호 | 최초 작성 — 전체 TODO 인벤토리 수집 및 분류 |
