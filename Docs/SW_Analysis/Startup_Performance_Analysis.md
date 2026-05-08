# Startup Performance Analysis — `InitInstance()`

> **Analysis scope**: `uScan.cpp` `CuScanApp::InitInstance()` (line 441)  
> **Target**: 프로그램 최초 실행 시점부터 SW 준비 완료(TrayAdmin 다이얼로그 표시, `Initialize_Variables` 완료)까지  
> **Method**: 정적 코드 분석 (실제 타이밍 미측정, 추정치 포함)

---

## 1. SW 준비 완료 기준

`InitInstance()` 내에서 다음 시점을 "SW 준비 완료"로 정의한다:

- `Initialize_Variables(FALSE)` 호출 완료 (line ~1019)
- 모든 TrayAdminViewDlg 표시 완료 (line ~1017)
- INLINE_MODE 한정: HandlerService TCP 연결 완료 + 카메라 초기화 완료

이 시점 이후는 UI 버튼 라벨 갱신, 폴더 생성, 네트워크 연결 확인 등 부가 작업이며,  
실제 SW 동작에는 영향이 없다.

---

## 2. 단계별 시작 시퀀스 및 병목 분석

### Phase 0 — 시스템 초기화 (무시 가능)

| 위치 | 작업 | 예상 시간 |
|------|------|-----------|
| line 443 | `AfxSocketInit()` — Winsock 초기화 | < 1ms |
| line 461~475 | `InitializeCriticalSection()` ×12 | < 1ms |
| line 477~488 | HALCON `SetSystem` ×9, `ResetObjDb` | < 5ms |
| line ~490~540 | MFC 프레임워크 초기화(`AfxOleInit` 등) | < 5ms |

총 예상: **< 10ms**

---

### Phase 1 — INI 설정 일괄 읽기 (line 541~553)

```
ReadPGMLanguageSelectINI()    ← PGMLanguageSelect.ini, 3 calls
ReadModelNameCodeINI()        ← ModelNameCode.ini, MODEL_TYPE_MAX × 1 calls
ReadEquipModelTypeINI()       ← EquipModelType.ini, 1 call
ReadEquipModelInfoINI()       ← ModelInfo.ini, ~128 calls
ReadInspectionTypeInfoINI()   ← InspectionTypeInfo.ini, ~46 calls
ReadPreferenceINI()           ← Preference.ini, 266 calls  ← 최다
ReadOverlayViewportINI()      ← OverlayViewport.ini, ~288 calls
ReadDefectReViewInfoINI()     ← DefectReViewInfo.ini, 최대 400 calls
ReadFaiDefectReViewInfo()     ← FaiDefectReViewInfo.ini, 최대 400 calls
ReadADJReViewInfoINI()        ← AdjImageInfo_NoUse.ini, 최대 400 calls (Nan시 즉시 탈출)
```

**Phase 1 합계: 최대 ~1,500+ `GetPrivateProfile*` disk I/O 호출**

> `CIniFileCS`는 파일 캐싱 없음 — 매 `Get_*` 호출마다 파일 open/read/close 발생.  
> HDD 기준 1 call ≈ 0.1~0.5ms → 최대 **750ms** 예상.  
> SSD 기준 ≈ 0.01~0.05ms → **15~75ms** 예상.

#### 특이사항: 언어 선택 다이얼로그 (`ReadPGMLanguageSelectINI`)

```cpp
if (!m_bPGMLanguageDialogHide && PGMLanguageSelectDlg.DoModal() != IDOK)
    return;
```

`m_bPGMLanguageDialogHide = FALSE`인 경우, **사용자 입력을 기다리는 모달 다이얼로그**가 표시되어  
무제한 블로킹 발생. 운영 환경에서는 `TRUE`로 설정되어 있어야 함.

---

### Phase 2 — VisionLogger 초기화 (line 553)

```
InitializeVisionLogger()
  ├─ LogSetting.ini, ~18 INI calls
  ├─ CleanupLogsByFilenameDate() × 9  ← 카테고리별 로그 폴더 디렉토리 열거
  │    각 카테고리: filesystem::directory_iterator(folder) → 파일 목록 정렬 후 오래된 파일 삭제
  └─ GetOrCreateCategoryLogger() × 9  ← spdlog rotating file 생성
```

**예상 시간**: 로그 파일이 많을 경우 디렉토리 열거 비용 증가.  
로그 폴더 비어있으면 < 10ms, 파일 수백 개면 최대 **수십 ms 수준**.

---

### Phase 3 — MFC 윈도우 생성 (line ~576~648)

```
ProcessShellCommand()     ← MainFrame + View + 모든 Child Dialog 생성
  └─ OnCreate() 체인 → Dialog 인스턴스 생성 ×~20 개
```

MFC 윈도우/다이얼로그 생성은 순수 메모리 연산.  
**예상 시간**: < 50ms

---

### Phase 4 — 더미 이미지 생성 (line 681)

```cpp
WriteDummyImage()
  GenImageConst("byte", DUMMY_IMAGE_SIZE, DUMMY_IMAGE_SIZE)  × 3
  WriteImage("bmp", ..., "D:\\Dummy_V*_Image*")              × 45 × 3 = 135 호출
```

**HALCON `WriteImage` 135회 → D: 드라이브에 BMP 파일 기록**

- `DUMMY_IMAGE_SIZE`가 큰 이미지라면 파일 크기에 비례하여 I/O 시간 증가
- D: 드라이브가 HDD라면 시퀀셜 쓰기 기준 **100ms~수백 ms** 가능성
- `WriteImage`는 HALCON 인코딩 + 파일 write flush 포함

> **주의**: 매 시작 시마다 동일한 더미 파일을 덮어쓰므로, 실제로 1회 생성 후  
> 존재 여부를 확인하고 스킵하는 최적화 가능.

---

### Phase 5 — 객체 할당 (line 686~705)

```
new CModelDataManager × MODEL_DATA_MAX × VISION_NUMBER_MAX  (최대 8개)
Algorithm::GetInstance()   ← 싱글톤 생성
new CCameraManager × VISION_NUMBER_MAX  (4개)
HandlerService 설정
```

순수 힙 할당 및 생성자 호출.  
**예상 시간**: < 10ms

---

### Phase 6 — TCP 핸들러 연결 ⚠️ 주요 병목 (line 762~768)

```cpp
BOOL bConnectCheck = FALSE;
bConnectCheck = m_pHandlerService->Initialize_TcpHandler(FALSE);

if (!bConnectCheck)
{
    m_pHandlerService->Terminate_TcpHandler();
    m_pHandlerService->Initialize_TcpHandler(TRUE);  // 재시도
}
```

- `Initialize_TcpHandler(FALSE)`: 핸들러 장비(TCP 서버)에 접속 시도
- **연결 실패 시** `Terminate_TcpHandler()` 후 `Initialize_TcpHandler(TRUE)` 재시도
- TCP connect timeout = OS 기본값 (Windows: 20~75초) 또는 소켓 설정값

**예상 시간**:
- 장비 연결 정상: < 100ms
- 연결 실패 1회: OS timeout 전까지 블로킹, 재시도 포함 **수 초 ~ 수십 초** 가능

> OFFLINE_MODE에서는 핸들러 장비가 없으므로 반드시 timeout 발생.  
> `FALSE` 파라미터와 `TRUE` 파라미터의 동작 차이를 확인하여 OFFLINE 분기 최적화 필요.

---

### Phase 7 — 모델 자동 감지 + `LoadModelSWBaseData` / `LoadModelSWData` ⚠️⚠️ 최대 병목 (line 779~871)

```
LastStatus.txt → 마지막 모델명 읽기 (INI Get_String 1 call)
모델 타입 자동 감지 루프 (string 비교, 빠름)
  ├─ 타입 변경 시: ReadInspectionTypeInfoINI + ReadOverlayViewportINI + ReadDefectReViewInfoINI 재호출
  │   (~700 INI calls 추가)
  └─ 변경 없으면 스킵

for iDualModelData × iPcVisionNo:   // MODEL_DATA_MAX × VISION_NUMBER_MAX (최대 8번)
    LoadModelSWBaseData()           // ~4,891 INI calls/카메라
    LoadModelSWData()               // HALCON 모델 로드 + Shape Model 재계산
```

**`LoadModelSWBaseData()` 상세**:
- `ModelBase.ini`: 기본 4 calls + 45 tab × 75 keys = 3,375 calls
- `FaiMeasureSpec_DFA.ini`: 1 + 151 FAI items × 10 = 1,511 calls
- **카메라당 ~4,891 호출 × 최대 8 = ~39,128 호출 총계**
- HDD 기준: **최대 20초+**, SSD 기준: **2~5초**

**`LoadModelSWData()` 상세** (별도 분석문서 `LoadModelSW_Performance_Analysis.md` 참조):
- `GTRegion::Load()` 재귀 — ROI당 `AlgorithmParam::Load()` ×45
- `CreateAnisoShapeModelXld` / `CreateScaledShapeModelXld` — ROI당 HALCON Shape Model 피라미드 재계산
- `DeserializeVariationModel`, `DeserializeNccModel` — ROI당 HALCON 역직렬화
- `TupleConcat` O(N²) 루프 — N=1000점일 때 50만 번 복사
- **카메라당 수 초 ~ 수십 초 × 최대 8 = 전체 기동 시간의 80% 이상 예상**

---

### Phase 8 — INLINE_MODE 전용: 하드웨어 초기화 ⚠️ (line 873~912)

```
MappAlloc(M_NULL, ...)            ← Matrox Imaging Library 글로벌 초기화
MsysAlloc(M_SYSTEM_RAPIXOCXP, M_DEV0, ...)   ← CXP 프레임그래버 보드 0 초기화
MsysAlloc(M_SYSTEM_RAPIXOCXP, M_DEV1, ...)   ← CXP 프레임그래버 보드 1 초기화 (BOI/BOS 전용)
per camera:
    InitMvEthernetCam() 또는 InitGrabInterface()  ← 카메라 firmware 핸드셰이크
m_pTriggerManager->Initialize()   ← 트리거 HW 연결
```

- MIL `MsysAlloc`: 하드웨어 드라이버 초기화, **1~5초** 예상
- `InitGrabInterface`: 카메라별 펌웨어 통신 — 카메라 부팅 상태에 따라 **수 초** 가능
- Align 카메라: `InitMvEthernetCam()` → 이더넷 연결 시도, timeout 위험

**예상 시간**: **3~10초** (카메라 4대 기준)

> OFFLINE_MODE에서는 Phase 8 전체가 스킵됨.

---

### Phase 9 — HW 모델 로드 (line 925~933)

```
for iPcVisionNo (최대 4):
    LoadModelHWBaseData()   ← HW 관련 INI 읽기
    LoadModelHWData()       ← HW 캘리브레이션 데이터 로드
```

- `LoadModelHWBaseData.md`, `LoadModel_OK_Performance_Analysis.md` 참조
- **예상 시간**: < 1초 (VISION_NUMBER_MAX=4 기준)

---

### Phase 10 — 부가 데이터 로드 (line 956~982)

```
CountPerAreaCondition()         ← JSON 파일 파싱 (조건 미존재시 INI 폴백)
LoadCountPerAreaCondition_ini() ← INI fallback
m_pInspectLibraryDataManager->Initialize()  ← InspectLibrary.dat 바이너리 읽기
m_pLightLibraryDataManager->Initialize()    ← LightLibrary.dat 바이너리 읽기
```

**예상 시간**: < 100ms (바이너리 파일 단순 읽기)

---

### Phase 11 — 다이얼로그 표시 + `Initialize_Variables` (line 984~1019)

```
×4 InspectAdminViewHideDlg::Show()   ← ShowWindow
×4 TrayAdminViewDlg::Show()          ← ShowWindow (Undefine 카메라 스킵)
Initialize_Variables(FALSE)          ← FAI Inspector 생성, 변수 초기화 (I/O 없음)
```

**예상 시간**: < 50ms

---

### Phase 12 — 후처리 (SW 준비 완료 이후)

SW 준비 완료 이후 수행되는 작업 (비차단적이지 않음에 주의):

```
ConnectStatusLAS()                   ← LAS 서버 연결 시도 (조건부)
ConnectStatusPyADJ() + Sleep(1000)   ← PyADJ 연결 시도 (실패 시 Sleep 1초)
ConnectStatusLASAggregator() + Sleep(1000) ← LAS Aggregator (실패 시 Sleep 1초)
ThreadPool 생성 (ROI 이미지 저장용, Variation Feature 로그용)
폴더 생성 × ~10                       ← CreateFolder (빠름)
```

> **주의**: PyADJ, LASAggregator 연결 실패 시 각 `Sleep(1000)` 1초 블로킹.  
> 두 개 모두 실패하면 **+2초** 추가.

---

## 3. 병목 요약 (심각도 순)

| 순위 | 위치 | 원인 | 예상 시간 (SSD/HDD) | 심각도 |
|------|------|------|---------------------|--------|
| 1 | `LoadModelSWData()` ×8 (line 841) | HALCON Shape Model 재계산, 역직렬화 | 수 초 ~ **수십 초** / 동일 | 🔴 **최고** |
| 2 | `LoadModelSWBaseData()` ×8 (line 840) | `CIniFileCS` 무캐싱: ~39,128 disk I/O | **2~5초** / 20초+ | 🔴 **최고** |
| 3 | `Initialize_TcpHandler()` ×2 (line 762) | TCP 연결 timeout (OFFLINE/장비 꺼짐) | 수 초 ~ 수십 초 | 🟠 **높음** |
| 4 | INLINE: `MappAlloc`/`MsysAlloc`/카메라 init (line 874~902) | 하드웨어 드라이버 초기화 | 3~10초 | 🟠 **높음** |
| 5 | `ReadPreferenceINI()` (line ~546) | `Preference.ini` 266 disk I/O | **13ms~130ms** / 최대 130ms | 🟡 **중간** |
| 6 | `WriteDummyImage()` (line 681) | HALCON `WriteImage` ×135 회 | **50ms~500ms** | 🟡 **중간** |
| 7 | Phase 1 전체 INI 읽기 (line 541~550) | 1,500+ disk I/O | **15ms~750ms** | 🟡 **중간** |
| 8 | `InitializeVisionLogger()` (line 553) | 디렉토리 열거 ×9 | < 50ms (파일 적을때) | 🟢 **낮음** |
| 9 | 언어 선택 다이얼로그 (line ~10978) | `DoModal()` — 사용자 입력 대기 | 무제한 (미설정시) | 🟠 **운영 위험** |
| 10 | PyADJ/LASAggregator 연결 실패 (line 1137~1154) | `Sleep(1000)` ×2 | **최대 +2초** | 🟢 **낮음** |

---

## 4. 전체 기동 시간 추정

### OFFLINE 모드 (모델 있음, SSD, 하드웨어 없음)

| 단계 | 추정 시간 |
|------|-----------|
| Phase 0-2 (시스템/INI/로거) | 50~200ms |
| Phase 3 (윈도우 생성) | 20~50ms |
| Phase 4 (더미 이미지) | 50~200ms |
| Phase 5 (객체 할당) | 5~10ms |
| Phase 6 (TCP 연결 실패) | **10~30초** (OFFLINE시 timeout) |
| Phase 7 (`LoadModelSWBaseData` ×8) | **2~5초** |
| Phase 7 (`LoadModelSWData` ×8) | **10~60초** (모델 크기 의존) |
| Phase 9-11 (HW 로드, 라이브러리, 다이얼로그) | 1~2초 |
| **합계** | **~25~100초** |

> Phase 6의 TCP timeout이 OFFLINE 환경에서 가장 큰 단발성 블로킹 원인.  
> TCP timeout을 짧게 설정하거나 OFFLINE_MODE 시 TCP 연결을 스킵하면 대폭 단축 가능.

### INLINE 모드 (모델 있음, SSD, 하드웨어 정상)

| 단계 | 추정 시간 |
|------|-----------|
| Phase 0-5 | 300ms 내외 |
| Phase 6 (TCP 연결 성공) | < 200ms |
| Phase 7 (SW 모델 로드 ×8) | **12~65초** |
| Phase 8 (HW 카메라 init) | **3~10초** |
| Phase 9-11 | 1~2초 |
| **합계** | **~20~80초** |

---

## 5. 개선 제안

### 5-1. `CIniFileCS` 캐싱 (최우선 — Phase 1, 7 공통 효과)

`CIniFileCS` 생성자에서 `GetPrivateProfileSection()` 로 섹션 전체를 한 번에 읽어  
`std::map<CString, CString>` 으로 캐싱하면, 이후 모든 `Get_*` 호출이 메모리 조회로 변경됨.  
→ Phase 1: 1,500+ I/O → 파일당 1 I/O  
→ Phase 7 `LoadModelSWBaseData`: ~39,128 I/O → 파일당 1~3 I/O

### 5-2. `LoadModelSWData` 비동기화 (Phase 7)

`LoadModelSWData`의 HALCON Shape Model 재계산을 백그라운드 스레드로 분리하여  
SW 준비 완료 후 모델이 로드되는 방식으로 변경하면 사용자에게 보이는 기동 시간 대폭 단축.  
단, 모델 로드 완료 전 검사 실행을 막는 락(Lock) 메커니즘 필요.

### 5-3. TCP connect timeout 단축 (Phase 6)

OFFLINE_MODE 또는 `Initialize_TcpHandler(FALSE)` 첫 번째 시도에서  
소켓의 `connect_timeout` 을 짧게(500ms~2초)로 설정하면  
현재 수십 초 블로킹이 1~4초로 단축 가능.

### 5-4. `WriteDummyImage` 조건부 실행 (Phase 4)

```cpp
// 파일이 이미 존재하면 스킵
BOOL bDummyExists = TRUE;
for (i = 0; i < MAX_IMAGE_TAB; i++) {
    ImageFileName.Format("D:\\Dummy_V1_Image%d", i + 1);
    if (!PathFileExists(ImageFileName + ".bmp")) { bDummyExists = FALSE; break; }
}
if (!bDummyExists) WriteDummyImage();
```

### 5-5. 언어 선택 다이얼로그 운영 환경 점검 (Phase 1)

운영 장비에서 `PGMLanguageSelect.ini` 의 `[DIALOG POPUP] Hide=1` 이 반드시 설정되어 있는지 확인.

---

## 6. 관련 분석 문서

| 문서 | 내용 |
|------|------|
| `LoadModelDlg_Performance_Analysis.md` | `OnSelchangeListModelnames()` 5× INI disk I/O 분석 |
| `LoadModelSW_Performance_Analysis.md` | `LoadModelSWBaseData` / `LoadModelSWData` 상세 분석 |
| `LoadModel_OK_Performance_Analysis.md` | 수동 모델 로드 OK 버튼 이후 전체 체인 분석 |
