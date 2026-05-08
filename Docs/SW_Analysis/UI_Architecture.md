# Universal_AVI (uScan) MFC UI 아키텍처

> 작성일: 2026-03-06

---

## 1. 전체 구조 개요

Universal_AVI는 Windows MFC 기반의 기계 비전 검사 시스템으로, **Document/View 아키텍처**를 기반으로 하며 다수의 독립 다이얼로그가 조합된 복합 UI 구조를 갖는다.

```
CuScanApp (Application)
└── CMainFrame (CFrameWnd)
    ├── CStatusBar (m_wndStatusBar)
    ├── CToolBar (m_wndToolBar)
    └── CuScanView (CFormView) ─── IDD_uScan_FORM
        ├── CPreferenceDlg         (환경설정)
        └── CNewModelDlg           (신규 모델 생성)
```

---

## 2. 핵심 클래스 계층

### 2.1 Application & Frame

| 클래스 | 파일 | 역할 |
|--------|------|------|
| `CuScanApp` | `uScan.h/.cpp` | 앱 진입점, 전역 서비스 인스턴스 소유 |
| `CMainFrame` | `MainFrm.h/.cpp` | 최상위 프레임 윈도우 (CFrameWnd 파생), StatusBar/ToolBar 포함 |
| `CuScanDoc` | `uScanDoc.h/.cpp` | MFC Document (직렬화 구조 — 실질 데이터는 서비스에 있음) |
| `CuScanView` | `uScanView.h/.cpp` | 메인 폼 뷰 (CFormView 파생), 메인 버튼/모드 전환 제공 |

### 2.2 uScanView 주요 버튼 (IDD_uScan_FORM)

- 신규 모델(`OnBnClickedMfcbuttonFormNew`)
- 모델 로드(`OnBnClickedMfcbuttonFormLoad`)
- 모델 저장(`OnBnClickedMfcbuttonFormSave`)
- 티칭 비전 1~4(`OnBnClickedMfcbuttonFormTeachingVisionN1~4`)
- 환경설정(`OnBnClickedMfcbuttonFormPreference`) → `CPreferenceDlg`
- 운영자 등록(`OnBnClickedMfcbuttonFormRegister`)
- AI 설정(`OnBnClickedMfcbuttonFormAisetup`)
- 종료(`OnBnClickedMfcbuttonFormExit`)

---

## 3. 검사 뷰 영역 (Inspect View)

검사 화면은 **뷰어** + **툴바** + **히든 뷰어** 세 가지 계층으로 구성된다. 4개의 카메라 비전을 지원하며 각각 독립 인스턴스로 관리한다.

### 3.1 메인 뷰어

```
CInspectAdminViewDlg         ── IDD_INSPECT_ADMIN_VIEW_DLG
  (싱글턴, GetInstance())
  - HALCON HObject 이미지 버퍼 (Live/Teaching/Barcode/OCR 등)
  - GTRegion 기반 ROI 편집 (Rect/Circle/Polygon/Cross)
  - GViewportManager 로 줌/스크롤 관리
  - 컨텍스트 메뉴: ROI 복사/이동/회전/스케일
  - InspectionType 변형 다이얼로그 포함
    ├── CInspectionTypeExDlg
    ├── CInspectionTypeChsDlg
    └── CInspectionTypeChsWZDlg
```

**툴바 상태 (eTOOLBAR_STATE)**:
`TS_DRAW` / `TS_MOVE` / `TS_SELECT_PART` / `TS_CREATE_ROI_RECT` / `TS_CREATE_ROI_CIRCLE` / `TS_CREATE_ROI_POLYGON` / `TS_CREATE_ROI_CROSS` / `TS_CREATE_AF_ROI`

### 3.2 툴바

```
CInspectAdminViewToolbarDlg  ── IDD_INSPECT_ADMIN_VIEW_TOOLBAR_DLG
  (싱글턴)
  - CToolBarCombo m_ViewToolbar
  - Draw/Move/ROI 생성·삭제, 줌인·아웃, 라이브, CrossBar,
    FitLine/InspectArea/Threshold/Defect 표시 토글
  - 채널 선택: Color/R/G/B/H/S/I
```

### 3.3 히든 뷰어 (검사 결과 렌더링용, 비가시)

```
CInspectAdminViewHideDlg1~4  ── IDD_INSPECT_ADMIN_VIEW_HIDE_DLG1~4
  (싱글턴, 카메라 채널별)
  - 결과 이미지/결과 파일 저장 (WriteResultFile, WriteSelectedRosReviewFile)
  - HALCON 숨김 윈도우 핸들 (HideWindowHandle) 로 연산
  - 바코드/OCR/Gap/Conformal Height 결과 포함
```

### 3.4 오버레이 이미지 뷰어

```
CInspectViewOverayImageDlg1~4  ── IDD_INSPECT_VIEW_OVERAY_IMAGE1~4_DLG
  (싱글턴, 카메라 채널별)
  - 원본 이미지 오버레이 표시 (검사 후 리뷰)
  - HObject m_HRawImage[MAX_IMAGE_TAB]
  - GViewportManager 기반 줌/스크롤

CInspectViewOverayImageToolbarDlg1~4
  - 오버레이 뷰어 전용 툴바
```

### 3.5 트레이 뷰어

```
CTrayAdminViewDlg1~4  ── IDD_TRAYVIEW_ADMIN_DLG1~4
  (싱글턴, 카메라 채널별)
  - CTrayImagePB m_TrayImagePB (트레이 셀 상태 표시)
  - HALCON 기반 트레이 직사각형 그리기 (MakeTrayRectangle)
```

---

## 4. 티칭 탭 영역 (TabControlDlg)

```
CTabControlDlg               ── IDD_TEACHING_TAB_DLG
  (싱글턴, CTabCtrl m_TabControl)
  │
  ├── CThresholdHistogramDlg    ── 임계값/히스토그램 조정
  ├── CLightControlDlg          ── 조명 제어 (페이지별 채널)
  ├── CJogSetDlg                ── 모션 조그 설정
  ├── CInspectionConditionTabDlg── 검사 조건 탭
  │     └── CConditionDlg[MAX_INSPECTION_TYPE]
  ├── CExtraDlg                 ── 기타 설정
  ├── CTeachParamDlg            ── 티칭 파라미터 그리드
  │     └── CTeachParamGridCtrl
  ├── CTeachingAlgorithmTabDlg  ── 알고리즘 탭 (아래 상세)
  ├── CAutoFocusDlg             ── 오토 포커스
  ├── CInspectLibraryDlg        ── 검사 라이브러리
  ├── CGrabSequenceViewDlg      ── 그랩 시퀀스 뷰어
  ├── CMotionControlDlg         ── 모션 컨트롤
  └── CCameraLightManagerDlg    ── 카메라/조명 관리
```

---

## 5. 알고리즘 탭 (TeachingAlgorithmTabDlg)

```
CTeachingAlgorithmTabDlg     ── IDD_TEACH_ALGORITHM_TAB_DLG
  (싱글턴, CTabCtrl m_AlgorithmTabCtrl)
  │
  ├── CAlgorithmImageProcessDlg      이미지 처리
  ├── CAlgorithmDontcareDlg          Don't Care 영역
  ├── CAlgorithmSurfaceDlg           표면 검사
  ├── CAlgorithmSurfaceDualDlg       이중 표면 검사
  ├── CAlgorithmDefectConditionDlg   결함 조건
  ├── CAlgorithmFittingDlg           피팅
  ├── CAlgorithmOCRDlg               OCR
  ├── CAlgorithmBarcodeDlg           바코드
  ├── CAlgorithmConnectorPinDlg      커넥터 핀
  ├── CAlgorithmDentDlg              덴트
  ├── CAlgorithmImageCompareDlg      이미지 비교
  ├── CAlgorithmPartCheckDlg         부품 확인
  ├── CAlgorithmROIAlignDlg          ROI 정렬
  ├── CAlgorithmROIAnisoAlignDlg     ROI 비등방 정렬
  ├── CAlgorithmConcenterMeasureDlg  동심원 측정
  ├── CAlgorithmShapeDlg             형상 검사
  ├── CAlgorithmEdgeMeasureDlg       엣지 측정
  ├── CAlgorithmTiltDlg              기울기 측정
  ├── CAlgorithmEpoxyVoidHoleDlg     에폭시 보이드홀
  ├── CAlgorithmPrintQualityDlg      인쇄 품질
  ├── CAlgorithmStiffenerEpoxyDlg    Stiffener Epoxy
  ├── CAlgorithmDistanceMeasureDlg   거리 측정
  ├── CAlgorithmGapMeasureDlg        갭 측정
  ├── CAlgorithmComponentDlg         부품 검사
  ├── CAlgorithmNeckEpoxyMeasureDlg  Neck Epoxy 측정
  ├── CAlgorithmNeckEpoxyCrackDlg    Neck Epoxy 크랙
  ├── CAlgorithmCornerMeasureDlg     코너 측정
  ├── CAlgorithmHeightMeasureDlg     높이 측정
  └── CAlgorithmBoundaryDlg          경계 검사
```

**버튼 (CTeachingAlgorithmTabDlg)**:
- 전체 테스트 / 비전 테스트 / AI 테스트
- AI 설정(외부 프로세스 실행) / AI 적용
- 저장 후 닫기 / 닫기 / 단위 변환

---

## 6. 모달/보조 다이얼로그 목록

### 6.1 모델 관리
| 클래스 | 역할 |
|--------|------|
| `CNewModelDlg` | 신규 모델 생성 |
| `CLoadModelDlg` | 기존 모델 로드 |
| `CPreferenceDlg` | 전체 환경설정 (경로, 카메라, ADJ, 이미지 저장 옵션 등) |
| `CSelectEquipModelTypeDlg` | 장비 모델 타입 선택 |

### 6.2 검사 결과 / 상태
| 클래스 | 역할 |
|--------|------|
| `CInspectResultDlg` | 검사 결과 요약 (Lot ID, 트레이 카운트, 연결 상태 표시) |
| `CDefectListDlg` | 결함 목록 |
| `CDefectPriorityDlg` | 결함 우선순위 설정 |
| `CInspectLibraryDlg` | 검사 라이브러리 |

### 6.3 ROI 조작
| 클래스 | 역할 |
|--------|------|
| `CRoiAutoCreateDlg` | ROI 자동 생성 |
| `CRoiCopyOptionDlg` | ROI 복사 옵션 |
| `CRoiIdChangeDlg` | ROI ID 변경 |
| `CRoiMoveDlg` | ROI 이동 (mm/pixel) |
| `CRoiScaleDlg` | ROI 스케일 |
| `CRoiSizeDlg` | ROI 크기 변경 |
| `CSetAngleDlg` | ROI 각도 설정 |
| `CSetFlipDlg` | ROI 플립 |

### 6.4 FAI 티칭
| 클래스 | 역할 |
|--------|------|
| `CTeachFaiAkcDlg` | AKC 제품 FAI 티칭 |
| `CTeachFaiBoiDlg` | BOI 제품 FAI 티칭 |
| `CTeachFaiActDlg` | ACT 제품 FAI 티칭 |
| `CTeachFaiAtwDlg` | ATW 제품 FAI 티칭 |
| `CTeachFaiChsKSDlg` | CHS_KS 제품 FAI 티칭 |
| `CTeachFaiChsWZDlg` | CHS_WZ 제품 FAI 티칭 |

### 6.5 조명 관련
| 클래스 | 역할 |
|--------|------|
| `CLightControlDlg` | 조명 제어 (최대 32페이지 채널) |
| `CLightPageControlDlg` | 조명 페이지 컨트롤 |
| `CLightPwmControlDlg` | PWM 조명 제어 |
| `CLightLibraryDlg` | 조명 라이브러리 |
| `CLightAutoCalResultDlg` | 조명 자동 캘리브레이션 결과 |
| `CLightAverageValueDlg` | 조명 평균값 |
| `CCameraLightManagerDlg` | 카메라/조명 통합 관리 |

### 6.6 캘리브레이션 / 포커스
| 클래스 | 역할 |
|--------|------|
| `CAutoFocusDlg` | 오토 포커스 설정 |
| `CFocusAutoCalResultDlg` | 포커스 자동 캘리브레이션 결과 |
| `CGlobalMatchingDlg` | 전역 매칭 설정 |
| `CSPVImageParamDlg` | SPV 이미지 파라미터 |

### 6.7 기타
| 클래스 | 역할 |
|--------|------|
| `CAdminPasswordDlg` | 관리자 비밀번호 |
| `COperator_RegisterDlg` | 운영자 등록 |
| `CThresholdHistogramDlg` | 임계값 히스토그램 |
| `CUnitConvertDlg` | 단위 변환 |
| `CGrabSequenceDlg` | 그랩 시퀀스 설정 |
| `CGrabSequenceViewDlg` | 그랩 시퀀스 뷰어 |
| `CStageSelectDlg` | 스테이지 선택 |
| `CMotionControlDlg` | 모션 컨트롤 |
| `CCreateFontDlg` | 폰트 생성 |
| `CJogSetDlg` | 조그 설정 |
| `CMultiModuleMoveOffsetDlg` | 멀티 모듈 이동 오프셋 |
| `CPGMLanguageSelectDlg` | 언어 선택 |
| `CSetLibraryNameDlg` | 라이브러리 이름 설정 |
| `CSetResultImageDlg` | 결과 이미지 설정 |
| `CExtraDlg` | 기타 설정 |
| `CInspectionPriorityDlg` | 검사 우선순위 |

---

## 7. 싱글턴 패턴 적용 클래스

다음 클래스들은 모두 `GetInstance(BOOL bShowFlag)` / `DeleteInstance()` 패턴을 사용한다:

```
CInspectAdminViewDlg
CInspectAdminViewToolbarDlg
CInspectAdminViewHideDlg1~4
CInspectViewOverayImageDlg1~4
CInspectViewOverayImageToolbarDlg1~4
CTrayAdminViewDlg1~4
CTabControlDlg
CTeachingAlgorithmTabDlg
CInspectionConditionTabDlg
CTeachParamDlg
CInspectResultDlg
CLightControlDlg
```

---

## 8. 커스텀 컨트롤

| 컨트롤 | 파일 | 설명 |
|--------|------|------|
| `CButtonCS` | (SynControls) | 커스텀 버튼 |
| `CLabelCS` / `CStaticCS` | (SynControls) | 커스텀 레이블/스태틱 |
| `CToolBarCombo` | `ToolBarCombo.h` | 툴바 콤보박스 |
| `CTrayImagePB` | `TrayImagePB.h` | 트레이 이미지 픽처박스 |
| `CTeachParamGridCtrl` | `TeachParamGridCtrl.h` | 티칭 파라미터 그리드 |
| `CConditionGridCtrl` | `ConditionGridCtrl.h` | 검사 조건 그리드 |
| `GViewportManager` | `GViewportManager.h` | HALCON 뷰포트 관리 |
| `GTRegion` | `GTRegion.h` | HALCON HObject/HRegion 래퍼 |

---

## 9. UI-서비스 연결 구조

```
CuScanApp (uScan.h)
  │  전역 소유
  ├── CInspectAdminViewDlg     ← Algorithm (검사 엔진)
  ├── CInspectAdminViewHideDlg ← CInspectService (결과 저장)
  ├── CTrayAdminViewDlg        ← CBatchManager (배치 상태)
  ├── CInspectResultDlg        ← 상태 표시 (Lot, ADJ, LAS 연결)
  ├── CTabControlDlg           ← 티칭 전체 탭 컨테이너
  │     └── CTeachingAlgorithmTabDlg  ← Algorithm (파라미터 R/W)
  └── CPreferenceDlg           ← 환경 설정 (경로/하드웨어/ADJ/AI)
```

---

## 10. 화면 레이아웃 개념도

```
┌──────────────────────────────────────────────────────────────────┐
│  CMainFrame (CFrameWnd)                                          │
│  ┌────────────────────────────────────────────────────────────┐  │
│  │  CuScanView (CFormView)  ─ 상단 버튼바                     │  │
│  │  [신규] [로드] [저장] [티칭V1~4] [환경설정] [등록] [종료]  │  │
│  └────────────────────────────────────────────────────────────┘  │
│                                                                  │
│  ┌───────────────────────┐  ┌───────────────────────────────┐   │
│  │ CInspectAdminViewDlg  │  │ CTabControlDlg (티칭 탭)      │   │
│  │ (메인 이미지 뷰어)    │  │ ┌──────────────────────────┐  │   │
│  │ + HALCON 렌더링       │  │ │ Histogram / Light / Jog   │  │   │
│  │ + ROI 편집            │  │ │ Condition / Extra         │  │   │
│  │                       │  │ │ TeachParam / Algorithm    │  │   │
│  └───────────────────────┘  │ │ AutoFocus / Motion / Lib  │  │   │
│                             │ └──────────────────────────┘  │   │
│  ┌───────────────────────┐  └───────────────────────────────┘   │
│  │ CInspectResultDlg     │                                       │
│  │ (Lot/결과 상태)        │  ┌───────────────────────────────┐   │
│  └───────────────────────┘  │ CTrayAdminViewDlg1~4 (트레이)  │   │
│                             └───────────────────────────────┘   │
└──────────────────────────────────────────────────────────────────┘
```

---

## 11. 다국어 지원

모든 주요 다이얼로그 클래스는 `ChangeLanguage()` 메서드를 구현하며, `strLog` / `strDialog` / `strMessageBox` 문자열 멤버를 통해 언어별 텍스트를 관리한다. 언어 전환은 `CPGMLanguageSelectDlg`에서 수행한다.
