// uScanView.cpp : implementation of the CuScanView class
//

#include "stdafx.h"
#include "uScan.h"

#include "uScanDoc.h"
#include "uScanView.h"
#include "LoadModelDlg.h"

#include "AdminPasswordDlg.h"
#include "Operator_RegisterDlg.h"
#include "IniFileCS.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CuScanView

IMPLEMENT_DYNCREATE(CuScanView, CFormView)

BEGIN_MESSAGE_MAP(CuScanView, CFormView)
	//{{AFX_MSG_MAP(CuScanView)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_NEW, &CuScanView::OnBnClickedMfcbuttonFormNew)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_LOAD, &CuScanView::OnBnClickedMfcbuttonFormLoad)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_SAVE, &CuScanView::OnBnClickedMfcbuttonFormSave)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_TEACHING_VISION_N1, &CuScanView::OnBnClickedMfcbuttonFormTeachingVisionN1)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_TEACHING_VISION_N2, &CuScanView::OnBnClickedMfcbuttonFormTeachingVisionN2)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_TEACHING_VISION_N3, &CuScanView::OnBnClickedMfcbuttonFormTeachingVisionN3)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_TEACHING_VISION_N4, &CuScanView::OnBnClickedMfcbuttonFormTeachingVisionN4)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_PREFERENCE, &CuScanView::OnBnClickedMfcbuttonFormPreference)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_PREV_TRAY, &CuScanView::OnBnClickedMfcbuttonFormPrevTray)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_REGISTER, &CuScanView::OnBnClickedMfcbuttonFormRegister)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_EXIT, &CuScanView::OnBnClickedMfcbuttonFormExit)
	ON_BN_CLICKED(IDC_BUTTON_MODE_CHANGE, &CuScanView::OnBnClickedButtonModeChange)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_AISETUP, &CuScanView::OnBnClickedMfcbuttonFormAisetup)
	ON_MESSAGE(WM_AISETUP_EXITED, &CuScanView::OnAiSetupExited)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CuScanView construction/destruction

CuScanView::CuScanView()
	: CFormView(CuScanView::IDD)
{
	//{{AFX_DATA_INIT(CuScanView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_bTeachingShow = FALSE;
}

CuScanView::~CuScanView()
{

}

void CuScanView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CuScanView)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BUTTON_MODE_CHANGE, m_bnModeChange);
	DDX_Control(pDX, IDC_STATIC_SW_VERSION, m_StaticSPIVersion);
	DDX_Control(pDX, IDC_STATIC_FORM_CURRENT_TEACH_MODULE, m_LabelTeachModule);
	DDX_Control(pDX, IDC_STATIC_FORM_CURRENT_STATUS, m_LabelCurMode);
	DDX_Control(pDX, IDC_STATIC_MAIN_TITLE, m_pStaticMainTitle);
}

void CuScanView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_hHeaderBitmap = NULL;
#ifdef SINGLE_LENS
	ChangeHeaderImage(IDB_BITMAP_HEADER_SLENS);
#elif defined(ASSY_LENS)
	ChangeHeaderImage(IDB_BITMAP_HEADER_ALENS);
#else
	ChangeHeaderImage(IDB_BITMAP_HEADER);
#endif

	m_StartTime = CTime::GetCurrentTime();

	SetStatusText(2);	// Date Time
	SetStatusText(3);	// Run Time

	CRect rect;
	m_pStaticMainTitle.GetWindowRect(rect);
	m_pStaticMainTitle.MoveWindow(rect.left, rect.top, 1918, rect.Height());
	m_pStaticMainTitle.ShowWindow(TRUE);

	SetTimer(0, 1000, NULL);		// Running Time etc...
#ifdef USE_SUAKIT

	THEAPP.m_TCPClientService.Initialize();									//통신쪽 초기화.

	//ADJ auto start

	BOOL bConnectExistAndError = FALSE;
	int	i_ADJMaxConnect = THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO;

	for (int iClientNO = 0; iClientNO < MAX_ADJ_CONNECT_NUM; iClientNO++)	//각각 통신 연결.
	{
		if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
		{
			for (int i = 0; i < i_ADJMaxConnect; i++)
			{
				if (!THEAPP.m_TCPClientService.m_bConnect[iClientNO][i])
				{
					if (THEAPP.m_TCPClientService.m_arrClient[iClientNO][i].TCPConnect())
					{
						THEAPP.m_TCPClientService.m_bConnect[iClientNO][i] = TRUE;
					}
					else
					{
						THEAPP.m_TCPClientService.m_bConnect[iClientNO][i] = FALSE;
						bConnectExistAndError = TRUE;
					}
				}
			}

			CString m_strFailedConnect = "";

			for (int j = i_ADJMaxConnect - 1; j >= 0; j--)
			{
				if (!THEAPP.m_TCPClientService.m_bConnect[iClientNO][j])
				{
					if (m_strFailedConnect == "")
						m_strFailedConnect.Format("#%d", j + 1);
					else
						m_strFailedConnect.Format("#%d,%s", j + 1, m_strFailedConnect);
				}
			}

			if (m_strFailedConnect == "")
			{
				if (iClientNO == 0)
				{
					strLog.Format("ADJ1 connect succeses");
					THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));
				}
				else
				{
					strLog.Format("ADJ2 connect succeses");
					THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));
				}
			}
			else
			{
				if (iClientNO == 0)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
					{
						strLog.Format("ADJ1 connect fail");
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
					}
					else
					{
						strLog.Format("ADJ1 disconnect success");
						THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));
					}
				}
				else
				{
					if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
					{
						strLog.Format("ADJ2 connect fail");
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
					}
					else
					{
						strLog.Format("ADJ2 disconnect success");
						THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));
					}
				}
			}
		}
	}

#endif

	THEAPP.SetADJSaveDefectList();

#ifdef INLINE_MODE
	SetTimer(1, 10000, NULL);		// Status Update to Handler
#endif

#ifdef INLINE_MODE
	SetTimer(3, 60000, NULL);		// Disk Capacity Check
#endif

	m_bnModeChange.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	m_LabelTeachModule.Init_Ctrl(_T("Arial Black"), 13, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));

	m_LabelCurMode.Init_Ctrl(_T("Arial"), 13, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

	ChangeLanguage();

	m_bStatusToggle = FALSE;
}


void CuScanView::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		SetDlgItemText(IDC_MFCBUTTON_FORM_NEW, _T("생성"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_LOAD, _T("불러오기"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_SAVE, _T("저장"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_PREFERENCE, _T("설정"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_PREV_TRAY, _T("리뷰"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_REGISTER, _T("관리자"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_EXIT, _T("종료"));

		if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN)
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_TEACH_MODULE, "티칭 비전");
		else
			UpdateTeachModule();

		if (THEAPP.Struct_PreferenceStruct.m_iVmTrain > VM_TRAIN_NOUSE)
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "자동학습 모드");
		else if (THEAPP.Struct_PreferenceStruct.m_bUseVmThresOpt)
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "최적화 모드");
		else
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "검사 모드");

		SetDlgItemText(IDC_BUTTON_MODE_CHANGE, _T("모드 변경\n(작업자 / 관리자)"));
	}
	else
	{
		SetDlgItemText(IDC_MFCBUTTON_FORM_NEW, _T("New"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_LOAD, _T("Load"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_SAVE, _T("Save"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_PREFERENCE, _T("Preference"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_PREV_TRAY, _T("Review"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_REGISTER, _T("Register"));
		SetDlgItemText(IDC_MFCBUTTON_FORM_EXIT, _T("EXIT"));

		if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN)
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_TEACH_MODULE, "Teaching Vision");
		else
			UpdateTeachModule();

		if (THEAPP.Struct_PreferenceStruct.m_iVmTrain > VM_TRAIN_NOUSE)
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "Auto train");
		else if (THEAPP.Struct_PreferenceStruct.m_bUseVmThresOpt)
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "Optimize");
		else
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "Inspection");

		SetDlgItemText(IDC_BUTTON_MODE_CHANGE, _T("Mode Change\n(Operator / Administrator)"));
	}

	UpdateCurMode();
}

void CuScanView::ShowVersionText()
{
	char AppFilePath[_MAX_PATH * 2];
	strcpy(AppFilePath, AfxGetApp()->m_pszHelpFilePath);

	int slength = strlen(AppFilePath);
	strcpy(AppFilePath + (slength - 4), ".exe");

	DWORD Dummy = NULL;
	BOOL bResult = FALSE;
	HRESULT hResult = S_OK;
	DWORD dwLen = 0;
	DWORD dwError = 0;
	PVOID pVersionData = NULL;
	PVOID pVersionInfo = NULL;

	// 버전정보의 크기를 알아 내고 데이터 버퍼를 할당한다.
	// 파일의 버전정보의 크기가 파일마다 제각기 다르기 땜시...
	dwLen = ::GetFileVersionInfoSize(AppFilePath, &Dummy);
	if (dwLen == 0)
	{
		return;
	}
	// 버전 정보를 담을 버퍼를 할당한다. (수천 바이트가 필요하다)
	pVersionData = ::HeapAlloc(::GetProcessHeap(), 0, dwLen);
	ZeroMemory(pVersionData, dwLen);

	// 버전 정보 데이터를 읽는다.
	bResult = ::GetFileVersionInfo(AppFilePath, NULL, dwLen, pVersionData);
	if (bResult == FALSE)
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return;
	}

	// VS_FIXEDFILEINFO 로 부터 버전정보를 읽는다.
	// (숫자로 된 버전정보는 이 구조체 안에 정보가 담긴다)
	bResult = ::VerQueryValue(pVersionData, TEXT("\\"), &pVersionInfo, (PUINT)&dwLen);
	if (bResult == FALSE)
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return;
	}
	VS_FIXEDFILEINFO* pFileInfo = (VS_FIXEDFILEINFO*)pVersionInfo;
	// 숫자로된 버전 정보를 알아냈으니 필요한 대로 쓰문 되겠다.
	DWORD dwMajorVer = HIWORD(pFileInfo->dwFileVersionMS);
	DWORD dwMinorVer = LOWORD(pFileInfo->dwFileVersionMS);
	DWORD dwReleaseVer = HIWORD(pFileInfo->dwFileVersionLS);
	DWORD dwBuildVer = LOWORD(pFileInfo->dwFileVersionLS);

	CString strVer;

	int iParamVersion = THEAPP.GetSwParamVersion();
	CString sParamVersion;
	sParamVersion.Format("%d", iParamVersion);
	CString sEachParamVersion[4];
	sEachParamVersion[0] = sParamVersion.GetAt(0);
	sEachParamVersion[1] = sParamVersion.GetAt(1);
	sEachParamVersion[2] = sParamVersion.GetAt(2);
	sEachParamVersion[3] = sParamVersion.GetAt(3);

#ifdef INLINE_MODE
	strVer.Format("INLINE  SW %d.%d.%d.%d / Parameter %s.%s.%s.%s (PC %d)", dwMajorVer, dwMinorVer, dwReleaseVer, dwBuildVer, sEachParamVersion[0], sEachParamVersion[1], sEachParamVersion[2], sEachParamVersion[3], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
#else
	strVer.Format("OFFLINE SW %d.%d.%d.%d / Parameter %s.%s.%s.%s (PC %d)", dwMajorVer, dwMinorVer, dwReleaseVer, dwBuildVer, sEachParamVersion[0], sEachParamVersion[1], sEachParamVersion[2], sEachParamVersion[3], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
#endif

	m_StaticSPIVersion.Init_Ctrl(_T("Arial"), 13, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));

	m_StaticSPIVersion.SetWindowText(strVer);
}

void CuScanView::UpdateVersionText()
{
	CString strVer;

	CString sSWVersion;
	sSWVersion.Format("%d", THEAPP.GetProgramVersion());
	CString sEachSWVersion[4];
	sEachSWVersion[0] = sSWVersion.GetAt(0);
	sEachSWVersion[1] = sSWVersion.GetAt(1);
	sEachSWVersion[2] = sSWVersion.GetAt(2);
	sEachSWVersion[3] = sSWVersion.GetAt(3);

	int iParamVersion = THEAPP.GetSwParamVersion();
	CString sParamVersion;
	sParamVersion.Format("%d", iParamVersion);
	CString sEachParamVersion[4];
	sEachParamVersion[0] = sParamVersion.GetAt(0);
	sEachParamVersion[1] = sParamVersion.GetAt(1);
	sEachParamVersion[2] = sParamVersion.GetAt(2);
	sEachParamVersion[3] = sParamVersion.GetAt(3);

#ifdef INLINE_MODE
	strVer.Format("INLINE  SW %d.%d.%d.%d / Parameter %s.%s.%s.%s (PC %d)", sEachSWVersion[0], sEachSWVersion[1], sEachSWVersion[2], sEachSWVersion[3], sEachParamVersion[0], sEachParamVersion[1], sEachParamVersion[2], sEachParamVersion[3], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
#else
	strVer.Format("OFFLINE SW %d.%d.%d.%d / Parameter %s.%s.%s.%s (PC %d)", sEachSWVersion[0], sEachSWVersion[1], sEachSWVersion[2], sEachSWVersion[3], sEachParamVersion[0], sEachParamVersion[1], sEachParamVersion[2], sEachParamVersion[3], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
#endif

	m_StaticSPIVersion.Init_Ctrl(_T("Arial"), 13, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));

	m_StaticSPIVersion.SetWindowText(strVer);
}

/////////////////////////////////////////////////////////////////////////////
// CuScanView diagnostics

#ifdef _DEBUG
void CuScanView::AssertValid() const
{
	CFormView::AssertValid();
}

void CuScanView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

//CuScanDoc* CuScanView::GetDocument() // non-debug version is inline
//{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CuScanDoc)));
//	return (CuScanDoc*)m_pDocument;
//}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CuScanView message handlers

BEGIN_EVENTSINK_MAP(CuScanView, CFormView)
	//{{AFX_EVENTSINK_MAP(CuScanView)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

LRESULT CuScanView::OnEventChangeMode(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

LRESULT CuScanView::OnEventChangeStatus(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

void CuScanView::SetStatusText(int idx, CString szMsg)
{
	CTime time = CTime::GetCurrentTime();

	switch (idx) {	// Modified Go 200511/21
	case 0:		// Model
		szMsg.Format("Model: %s", szMsg);
		GetDlgItem(IDC_STATIC_FORM_CURRENT_MODEL)->SetWindowText(szMsg);
		break;
	case 1:		// PCB Count
		szMsg.Format("%s", szMsg);
		SetTextPCBCount(szMsg);
		break;
	case 2:		// Current time
		szMsg = time.Format("%Y.%m.%d  %H:%M:%S");
		GetDlgItem(IDC_STATIC_FORM_DATE_TIME)->SetWindowText(szMsg);
		break;
	case 3:		// Runnig time
		CTimeSpan timespan = time - m_StartTime;
		szMsg.Format("Running time : %4ld:%02d:%02d", timespan.GetTotalHours(), timespan.GetMinutes(), timespan.GetSeconds());
		GetDlgItem(IDC_STATIC_FORM_RUN_TIME)->SetWindowText(szMsg);
		break;
	}
}

void CuScanView::OnDestroy()
{
	CFormView::OnDestroy();

	if (m_hHeaderBitmap != NULL)
	{
		::DeleteObject(m_hHeaderBitmap);
		m_hHeaderBitmap = NULL;
	}
}

void CuScanView::SetButtonStatus(int nStatus)
{

}

void CuScanView::SetTextPCBCount(CString strPCBCount)
{


}

void CuScanView::SetExitProgram()
{
	KillTimer(0);
	KillTimer(1);
	KillTimer(2);
	KillTimer(3);

	AfxGetMainWnd()->DestroyWindow();
}

void CuScanView::TeachingShow()
{

}

void CuScanView::UpdateTeachModule()
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[THEAPP.m_iCurTeachVision];

	strDialog.Format("%s (Stage %d)", sVisionCamType_Short, THEAPP.m_iCurStageIndex + 1);
	SetDlgItemText(IDC_STATIC_FORM_CURRENT_TEACH_MODULE, strDialog);
}

void CuScanView::UpdateCurMode()
{
	CString sTemp;

	if (THEAPP.Struct_PreferenceStruct.m_iVmTrain > VM_TRAIN_NOUSE)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "자동학습 모드");
		else
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "Auto train");

		SetTimer(2, 2000, NULL);
	}
	else if (THEAPP.Struct_PreferenceStruct.m_bUseVmThresOpt)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "최적화 모드");
		else
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "Optimize");

		SetTimer(2, 2000, NULL);
	}
	else
	{
		KillTimer(2);

		m_LabelCurMode.Init_Ctrl(_T("Arial Black"), 13, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		if (THEAPP.m_iPGMLanguageSelect == 0)
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "검사 모드");
		else
			SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "Inspection");
	}
}

void CuScanView::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 0:
	{
		SetStatusText(2);	// Current Time
		SetStatusText(3);	// Running time
		break;
	}
	case 1:
	{
		if (!THEAPP.m_pHandlerService->Is_Connected())
			THEAPP.m_pHandlerService->Initialize_Handler();
		break;
	}
	case 2:
	{
		if (m_bStatusToggle)
		{
			m_LabelCurMode.Init_Ctrl(_T("Arial Black"), 13, TRUE, RGB(0, 128, 192), RGB(255, 255, 255));
			m_bStatusToggle = FALSE;
		}
		else
		{
			m_LabelCurMode.Init_Ctrl(_T("Arial Black"), 13, TRUE, RGB(255, 255, 255), RGB(0, 128, 192));
			m_bStatusToggle = TRUE;
		}

		break;
	}

#ifdef INLINE_MODE
	case 6:
	{
		KillTimer(6);
	}
#endif

	}

	CFormView::OnTimer(nIDEvent);
}

void CuScanView::OnBnClickedMfcbuttonFormNew()
{
	if (m_pNewModelDlg.DoModal() == IDOK)
	{
		if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN)  // 관리자 모드 아니면 화면 전환 안됨.
		{
#ifdef INLINE_MODE
			strLog.Format("☆☆☆☆☆ New Button click ☆☆☆☆☆");
			THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
#endif

			THEAPP.m_iCurTeachVision = VISION_NUMBER_1;
			THEAPP.m_pModelDataManager = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1];
			UpdateTeachModule();

			THEAPP.m_iModeSwitch = MODE_ADMIN_TEACH_VIEW;
			THEAPP.m_pInspectAdminViewDlg->ClearAll();
			THEAPP.m_pInspectAdminViewDlg->Show();
			THEAPP.m_pInspectAdminViewToolbarDlg->Show();
			THEAPP.m_pInspectAdminViewHideDlg1->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectAdminViewHideDlg2->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectAdminViewHideDlg3->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectAdminViewHideDlg4->ShowWindow(SW_HIDE);
			THEAPP.m_pDefectListDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pTrayAdminViewDlg1->ShowWindow(SW_HIDE);
			THEAPP.m_pTrayAdminViewDlg2->ShowWindow(SW_HIDE);
			THEAPP.m_pTrayAdminViewDlg3->ShowWindow(SW_HIDE);
			THEAPP.m_pTrayAdminViewDlg4->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectSummary->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectResultDlg->ShowWindow(SW_HIDE);
			//////////////////////////////////////////////////////////////////////////
			///  Change View-port
			THEAPP.m_iCurVisionCamType = VISION_NUMBER_1;
			THEAPP.m_pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_1];

			THEAPP.m_pAlgorithm->m_iPcVisionNo = THEAPP.m_iCurTeachVision;
			THEAPP.m_pAlgorithm->m_iVisionCamType = THEAPP.m_iCurVisionCamType;

			THEAPP.m_pInspectAdminViewDlg->ChangeViewport(THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[VISION_NUMBER_1], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[VISION_NUMBER_1]);
			//////////////////////////////////////////////////////////////////////////

			THEAPP.m_pTabControlDlg->Show();
			THEAPP.m_pTabControlDlg->ShowFirstPage(FALSE);
			THEAPP.m_pInspectViewOverayImageToolbarDlg1->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageToolbarDlg2->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageToolbarDlg3->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageToolbarDlg4->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageDlg1->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageDlg2->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageDlg3->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageDlg4->ShowWindow(SW_HIDE);
			THEAPP.m_bShowReviewWindow = FALSE;
		}
	}
}



void CuScanView::OnBnClickedMfcbuttonFormLoad()
{
	int iDualModelData, iPcVisionNo;
	CString sVisionCamType;

	if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN) // 관리자모드 아니면 로드안됨.
	{
		CLoadModelDlg dlgLoadModelDlg;

#ifdef INLINE_MODE
		strLog.Format("☆☆☆☆☆ LOAD Button click ☆☆☆☆☆");
		THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
#endif

		if (dlgLoadModelDlg.DoModal() == IDOK)
		{
			THEAPP.m_pInspectViewOverayImageToolbarDlg1->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageToolbarDlg2->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageToolbarDlg3->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageToolbarDlg4->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageDlg1->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageDlg2->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageDlg3->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewOverayImageDlg4->ShowWindow(SW_HIDE);
			THEAPP.m_bShowReviewWindow = FALSE;

			//////////////////////////////////////////////////////////////////////////

			for (iDualModelData = 0; iDualModelData < MODEL_DATA_MAX; iDualModelData++)
			{
				THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName = dlgLoadModelDlg.m_EditSelectedModel;
				THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_2]->m_sModelName = THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;
				THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_3]->m_sModelName = THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;
				THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_4]->m_sModelName = THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;
			}

			CString strRecipeName = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName;
			CString strEquipModelTypeCheck;
			strEquipModelTypeCheck = strRecipeName.Mid(strRecipeName.Find('_') + 1);
			strEquipModelTypeCheck = strEquipModelTypeCheck.Mid(0, strEquipModelTypeCheck.Find('_'));

			for (int i = 0; i <= MODEL_TYPE_MAX; i++)
			{
				if (i == MODEL_TYPE_MAX)
				{
					if (THEAPP.m_iPGMLanguageSelect == 0)
						strMessageBox.Format("비전 모델명에서 설비 검사 모델 정보를 찾을 수 없습니다. 설정 창의 설비 검사 모델을 직접 선택해주세요.");
					else
						strMessageBox.Format("Equipment inspection model type can't be found in the Vision model name. Select the model type directly in the preference.");
					AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

					strLog.Format("Model type data change fail(manual)(Can't find model type), Recipe type: %s", strEquipModelTypeCheck);
					THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));

					break;
				}

				CString strCheck = THEAPP.g_strModelTypeName[i];
				if (THEAPP.g_strModelTypeName[i] == strEquipModelTypeCheck)
				{
					if (!THEAPP.IsSameModelType(i))
					{
						THEAPP.SetModelType(i);
						THEAPP.ReadInspectionTypeInfoINI();
						THEAPP.ReadOverlayViewportINI();
						THEAPP.ReadDefectReViewInfoINI();
						THEAPP.ReadFaiDefectReViewInfo();

						strLog.Format("Model type data change complete(manual), Recipe type: %s", THEAPP.g_strModelTypeName[i]);
						THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
					}

					break;
				}
			}

			BOOL bModelExist1, bModelExist2;
			for (iDualModelData = 0; iDualModelData < MODEL_DATA_MAX; iDualModelData++)
			{
				for (iPcVisionNo = VISION_NUMBER_MAX - 1; iPcVisionNo >= VISION_NUMBER_1; iPcVisionNo--)
				{
					sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
					if (sVisionCamType == "Undefine")
						continue;

					bModelExist1 = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->LoadModelSWBaseData();
					bModelExist2 = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->LoadModelSWData();

					THEAPP.bVariationModelSaveCheck[iPcVisionNo] = FALSE;
					THEAPP.bTemplateModelSaveCheck[iPcVisionNo] = FALSE;
				}
			}

			if (!bModelExist1 || !bModelExist2)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessageBox.Format("비전 모델을 찾을 수 없습니다. Model 폴더 내 비전 모델을 확인해주세요.");
				else
					strMessageBox.Format("Vision model can't be found. Check the vision model inside the Model folder.");
				AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

				strLog.Format("Vision recipe load fail(manual), Recipe name: %s", THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);
				THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));

				if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName.GetLength() < 35)
					THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				else
					THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 8, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_VISION_MODEL_NAME, THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);
			}
			else
			{
				strLog.Format("Vision recipe load complete(manual), Recipe name: %s", THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);
				THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));

				if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName.GetLength() < 35)
					THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				else
					THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 8, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_VISION_MODEL_NAME, THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);
			}

			for (iPcVisionNo = VISION_NUMBER_MAX - 1; iPcVisionNo >= VISION_NUMBER_1; iPcVisionNo--)
			{
				sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
				if (sVisionCamType == "Undefine")
					continue;

				bModelExist1 = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->LoadModelHWBaseData();
				bModelExist2 = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->LoadModelHWData();
			}

			CString strOpticalModelName;
			strOpticalModelName.Format("%s_%s_PC%d", THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
			if (!bModelExist1 || !bModelExist2)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessageBox.Format("광학 모델을 찾을 수 없습니다. Optical 폴더 내 광학 모델을 확인해주세요.");
				else
					strMessageBox.Format("Optical model can't be found. Check the optical modelinside the Optical folder.");
				AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

				strLog.Format("Optical recipe load fail(manual), Recipe name: %s", strOpticalModelName);
				THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));

				THEAPP.m_pInspectResultDlg->m_LabelOpticalModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_OPTICAL_MODEL_NAME, strOpticalModelName);
			}
			else
			{
				strLog.Format("Optical recipe load complete(manual), Recipe name: %s", strOpticalModelName);
				THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));

				THEAPP.m_pInspectResultDlg->m_LabelOpticalModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_OPTICAL_MODEL_NAME, strOpticalModelName);
			}

#ifdef DEFECT_CONDITION
			BOOL bLoadConditionCheck;
			bLoadConditionCheck = THEAPP.CountPerAreaCondition();
			if (bLoadConditionCheck)
			{
				strLog.Format("Count per area condition(json) load complete(manual)");
				THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				THEAPP.LoadCountPerAreaCondition_ini();
				strLog.Format("Count per area condition(ini) load complete(manual)");
				THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
			}
#endif

			THEAPP.m_pModelDataManager = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1];
			THEAPP.m_pModelDataManager->UpdateTeachParamView();
			//////////////////////////////////////////////////////////////////////////

			THEAPP.Initialize_Variables(TRUE);

			THEAPP.m_pTrayAdminViewDlg1 = CTrayAdminViewDlg1::GetInstance();
			sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_1];
			if (sVisionCamType != "Undefine")
				THEAPP.m_pTrayAdminViewDlg1->Show();
			THEAPP.m_pTrayAdminViewDlg2 = CTrayAdminViewDlg2::GetInstance();
			sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_2];
			if (sVisionCamType != "Undefine")
				THEAPP.m_pTrayAdminViewDlg2->Show();
			THEAPP.m_pTrayAdminViewDlg3 = CTrayAdminViewDlg3::GetInstance();
			sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_3];
			if (sVisionCamType != "Undefine")
				THEAPP.m_pTrayAdminViewDlg3->Show();
			THEAPP.m_pTrayAdminViewDlg4 = CTrayAdminViewDlg4::GetInstance();
			sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_4];
			if (sVisionCamType != "Undefine")
				THEAPP.m_pTrayAdminViewDlg4->Show();

			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("비전 모델 불러오기 완료.");
			else
				strMessageBox.Format("Vision model load complete.");
			AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

			strLog.Format("Vision recipe load complete(manual), recipe name: %s", THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);
			THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
		}

		CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
		CIniFileCS INI(strStatusFileName);
		CString strSection = "Status";
		INI.Set_String(strSection, "ModelID", THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);
	}
}


void CuScanView::OnBnClickedMfcbuttonFormSave()
{
	int iVisionCamType;
	CString sVisionCamType, sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[THEAPP.m_iCurTeachVision];

	if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW) // 티칭모드 아니면 저장안됨.
	{
#ifdef INLINE_MODE
		THEAPP.m_pInspectAdminViewToolbarDlg->StopLive();

		strLog.Format("☆☆☆☆☆ Save Button click ☆☆☆☆☆");
		THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
#endif

		THEAPP.m_iTeachingModuleNo = TEACHING_MODULE_NO_UNDEFINED;

		THEAPP.m_bReviewImageClick = FALSE;

		THEAPP.m_pModelDataManager->UpdatePolygonROIBoundary();

		//////////////////////////////////////////////////////////////////////////
		///  Teaching View Clear
		THEAPP.m_pInspectAdminViewDlg->ClearAlignTabTempRegion();
		GenEmptyObj(&THEAPP.m_pInspectAdminViewDlg->m_pThresRgn);
		THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();

		THEAPP.m_pInspectAdminViewDlg->SaveInspectTabOverLastSelectRegion();

		THEAPP.m_pInspectAdminViewDlg->ClearActiveTRegion();
		THEAPP.m_pInspectAdminViewDlg->ClearAllSelectRegion();
		THEAPP.m_pInspectAdminViewDlg->ClearAllLastSelectRegion();
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion = NULL;

		if (THEAPP.Struct_PreferenceStruct.m_bUseDeleteCrossBar)
			THEAPP.m_pInspectAdminViewDlg->DeleteCrossBarROI();

		THEAPP.m_pModelDataManager->CheckColorSpace();

		THEAPP.SaveAiRetrainRoi(THEAPP.m_iCurTeachVision);

		THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);

		int nRes = 0;
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("비전 모델을 저장하시겠습니까?");
		else
			strMessageBox.Format("Save the vision model?");
		nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

		if ((nRes == IDYES))
		{
			////////////////////////////// Record Parameter changes //////////////////////////////
			// THEAPP.m_pModelDataManager->SaveChangeParamHistory();

			// THEAPP.DoEvents(100);

#ifdef DEFECT_CONDITION
			THEAPP.SaveCountPerAreaCondition();

			THEAPP.DoEvents(100);
#endif
			if (THEAPP.Struct_PreferenceStruct.m_iVmTrain > VM_TRAIN_NOUSE)
				THEAPP.bVariationModelSaveCheck[THEAPP.m_iCurTeachVision] = TRUE;

			THEAPP.m_pModelDataManager->SaveModelData();

			THEAPP.DoEvents(100);

			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("%s 모델 저장 완료", THEAPP.m_ModelDefineInfo.m_strVisionName[THEAPP.m_iCurTeachVision]);
			else
				strMessageBox.Format("%s model save complete", THEAPP.m_ModelDefineInfo.m_strVisionName[THEAPP.m_iCurTeachVision]);
			AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

			strLog.Format("Vision recipe save complete, Vision name: %s", sVisionCamType_Short);
			THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));

			THEAPP.bVariationModelSaveCheck[THEAPP.m_iCurTeachVision] = FALSE;
			THEAPP.bTemplateModelSaveCheck[THEAPP.m_iCurTeachVision] = FALSE;
		}

		THEAPP.m_pInspectAdminViewDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewToolbarDlg->ShowWindow(SW_HIDE);

		THEAPP.m_pInspectAdminViewHideDlg1->ShowWindow(SW_SHOWNA);
		THEAPP.m_pInspectAdminViewHideDlg2->ShowWindow(SW_SHOWNA);
		THEAPP.m_pInspectAdminViewHideDlg3->ShowWindow(SW_SHOWNA);
		THEAPP.m_pInspectAdminViewHideDlg4->ShowWindow(SW_SHOWNA);

		THEAPP.m_pTabControlDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pTabControlDlg->HideAllPage();

		THEAPP.m_pTrayAdminViewDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg4->ShowWindow(SW_HIDE);
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_1];
		if (sVisionCamType != "Undefine")
			THEAPP.m_pTrayAdminViewDlg1->ShowWindow(SW_SHOW);
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_2];
		if (sVisionCamType != "Undefine")
			THEAPP.m_pTrayAdminViewDlg2->ShowWindow(SW_SHOW);
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_3];
		if (sVisionCamType != "Undefine")
			THEAPP.m_pTrayAdminViewDlg3->ShowWindow(SW_SHOW);
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_4];
		if (sVisionCamType != "Undefine")
			THEAPP.m_pTrayAdminViewDlg4->ShowWindow(SW_SHOW);

		THEAPP.m_pDefectListDlg->ShowWindow(SW_SHOW);

		THEAPP.m_pInspectSummary->ShowWindow(SW_SHOW);
		THEAPP.m_pInspectResultDlg->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_SAVE)->EnableWindow(TRUE);

		iVisionCamType = VISION_NUMBER_1;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_1])
			GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N1)->EnableWindow(TRUE);

		iVisionCamType = VISION_NUMBER_2;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_2])
			GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N2)->EnableWindow(TRUE);

		iVisionCamType = VISION_NUMBER_3;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_3])
			GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N3)->EnableWindow(TRUE);

		iVisionCamType = VISION_NUMBER_4;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_4])
			GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N4)->EnableWindow(TRUE);

		GetDlgItem(IDC_MFCBUTTON_FORM_PREV_TRAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_PREFERENCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_REGISTER)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_MODE_CHANGE)->EnableWindow(TRUE);
		THEAPP.m_iModeSwitch = MODE_INSPECT_VIEW_ADMIN;

#ifdef INLINE_MODE
		THEAPP.m_pCameraManager->TurnOff_Specular();
#endif
	}
}

#include "StageSelectDlg.h"
#include <sstream>

void CuScanView::OnBnClickedMfcbuttonFormTeachingVisionN1()
{
#ifdef INLINE_MODE
	strLog.Format("☆☆☆☆☆ VISION 1 TEACHING Button click ☆☆☆☆☆");
	THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
#endif

	if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName == ".")
		return;

	if (THEAPP.m_bShowReviewWindow)
	{
		if (THEAPP.m_iCurSelectedResultModule != SELECTED_RESULT_NONE)
		{
			if (THEAPP.m_iCurSelectedResultModule != SELECTED_RESULT_VISION_N1)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessageBox.Format("선택된 검사 결과와 티칭 비전이 일치하지 않습니다.");
				else
					strMessageBox.Format("The selected module's vision does not match the teaching vision.");
				AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

				THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_NONE;
				return;
			}
		}
	}

	if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN)  // 관리자 모드 아니면 화면 전환 안됨.
	{
#if !defined(SINGLE_LENS) && !defined(ASSY_LENS)
		CStageSelectDlg dlg;
		if (dlg.DoModal() == IDCANCEL)
			return;

		THEAPP.m_iCurStageIndex = dlg.m_iRadioStageIndex;
#else
		THEAPP.m_iCurStageIndex = STAGE_NUMBER_1;
#endif

		THEAPP.m_iCurTeachVision = VISION_NUMBER_1;
		THEAPP.m_pModelDataManager = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1];
		UpdateTeachModule();

		THEAPP.m_pCameraManager = THEAPP.m_pDualCameraManager[VISION_NUMBER_1];
		THEAPP.m_pCameraManager->SetGrabCircularIndex(0);

		THEAPP.m_pCameraManagerSpecularMaster = THEAPP.m_pDualCameraManager[VISION_NUMBER_1];

		THEAPP.m_iModeSwitch = MODE_ADMIN_TEACH_VIEW;

		THEAPP.m_pInspectAdminViewHideDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pDefectListDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectSummary->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectResultDlg->ShowWindow(SW_HIDE);

		THEAPP.m_pInspectAdminViewToolbarDlg->UpdateToolbarStatusTeachMode();
		THEAPP.m_pInspectAdminViewToolbarDlg->Show();
		THEAPP.m_pInspectAdminViewDlg->Show();

		//////////////////////////////////////////////////////////////////////////
		///  Change View-port
		THEAPP.m_iCurVisionCamType = VISION_NUMBER_1;
		THEAPP.m_pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_1];

		THEAPP.m_pAlgorithm->m_iPcVisionNo = THEAPP.m_iCurTeachVision;
		THEAPP.m_pAlgorithm->m_iVisionCamType = THEAPP.m_iCurVisionCamType;

		THEAPP.m_pInspectAdminViewDlg->ChangeViewport(THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[VISION_NUMBER_1], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[VISION_NUMBER_1]);
		//////////////////////////////////////////////////////////////////////////

		THEAPP.m_pTabControlDlg->Show();
		THEAPP.m_pTabControlDlg->m_pExtraDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pJogSetDlg->UpdateDlgStatus();
		THEAPP.m_pTabControlDlg->m_pJogSetDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pCameraLightManagerDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pExtraDlg->SetTeachModeControl();
		THEAPP.m_pTabControlDlg->ShowFirstPage(TRUE);
		THEAPP.m_pTabControlDlg->LoadModelConditionParam();
		THEAPP.m_pModelDataManager->UpdateTeachParamView();
		THEAPP.m_pModelDataManager->ClearAiRetrainFlag();
		GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_PREV_TRAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MODE_CHANGE)->EnableWindow(FALSE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_bShowReviewWindow = FALSE;

		THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_NONE;
	}
}

void CuScanView::OnBnClickedMfcbuttonFormTeachingVisionN2()
{
#ifdef INLINE_MODE
	strLog.Format("☆☆☆☆☆ VISION 2 TEACHING Button click ☆☆☆☆☆");
	THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
#endif

	if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_2]->m_sModelName == ".")
		return;

	if (THEAPP.m_bShowReviewWindow)
	{
		if (THEAPP.m_iCurSelectedResultModule != SELECTED_RESULT_NONE)
		{
			if (THEAPP.m_iCurSelectedResultModule != SELECTED_RESULT_VISION_N2)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessageBox.Format("선택된 검사 결과와 티칭 비전이 일치하지 않습니다.");
				else
					strMessageBox.Format("The selected module's vision does not match the teaching vision.");
				AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

				THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_NONE;
				return;
			}
		}
	}

	if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN)  // 관리자 모드 아니면 화면 전환 안됨.
	{
#if !defined(SINGLE_LENS) && !defined(ASSY_LENS)
		CStageSelectDlg dlg;
		if (dlg.DoModal() == IDCANCEL)
			return;

		THEAPP.m_iCurStageIndex = dlg.m_iRadioStageIndex;
#else
		THEAPP.m_iCurStageIndex = STAGE_NUMBER_1;
#endif

		THEAPP.m_iCurTeachVision = VISION_NUMBER_2;
		THEAPP.m_pModelDataManager = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_2];
		UpdateTeachModule();

		THEAPP.m_pCameraManager = THEAPP.m_pDualCameraManager[VISION_NUMBER_2];
		THEAPP.m_pCameraManager->SetGrabCircularIndex(0);

		THEAPP.m_pCameraManagerSpecularMaster = THEAPP.m_pDualCameraManager[VISION_NUMBER_1];

		THEAPP.m_iModeSwitch = MODE_ADMIN_TEACH_VIEW;

		THEAPP.m_pInspectAdminViewHideDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pDefectListDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectSummary->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectResultDlg->ShowWindow(SW_HIDE);

		THEAPP.m_pInspectAdminViewToolbarDlg->UpdateToolbarStatusTeachMode();
		THEAPP.m_pInspectAdminViewToolbarDlg->Show();
		THEAPP.m_pInspectAdminViewDlg->Show();

		//////////////////////////////////////////////////////////////////////////
		///  Change View-port
		THEAPP.m_iCurVisionCamType = VISION_NUMBER_2;
		THEAPP.m_pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_2];

		THEAPP.m_pAlgorithm->m_iPcVisionNo = THEAPP.m_iCurTeachVision;
		THEAPP.m_pAlgorithm->m_iVisionCamType = THEAPP.m_iCurVisionCamType;

		THEAPP.m_pInspectAdminViewDlg->ChangeViewport(THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[VISION_NUMBER_2], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[VISION_NUMBER_2]);
		//////////////////////////////////////////////////////////////////////////

		THEAPP.m_pTabControlDlg->Show();
		THEAPP.m_pTabControlDlg->m_pExtraDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pJogSetDlg->UpdateDlgStatus();
		THEAPP.m_pTabControlDlg->m_pJogSetDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pCameraLightManagerDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pExtraDlg->SetTeachModeControl();
		THEAPP.m_pTabControlDlg->ShowFirstPage(TRUE);
		THEAPP.m_pTabControlDlg->LoadModelConditionParam();
		THEAPP.m_pModelDataManager->UpdateTeachParamView();
		THEAPP.m_pModelDataManager->ClearAiRetrainFlag();
		GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_PREV_TRAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MODE_CHANGE)->EnableWindow(FALSE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_bShowReviewWindow = FALSE;

		THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_NONE;
	}
}

void CuScanView::OnBnClickedMfcbuttonFormTeachingVisionN3()
{
#ifdef INLINE_MODE
	strLog.Format("☆☆☆☆☆ VISION 3 TEACHING Button click ☆☆☆☆☆");
	THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
#endif

	if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_3]->m_sModelName == ".")
		return;

	if (THEAPP.m_bShowReviewWindow)
	{
		if (THEAPP.m_iCurSelectedResultModule != SELECTED_RESULT_NONE)
		{
			if (THEAPP.m_iCurSelectedResultModule != SELECTED_RESULT_VISION_N3)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessageBox.Format("선택한 모듈의 비전이 티칭 비전과 일치하지 않습니다.");
				else
					strMessageBox.Format("The selected module's vision does not match the teaching vision.");
				AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

				THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_NONE;
				return;
			}
		}
	}

	if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN)  // ������ ��� �ƴϸ� ȭ�� ��ȯ �ȵ�.
	{
#if !defined(SINGLE_LENS) && !defined(ASSY_LENS)
		CStageSelectDlg dlg;
		if (dlg.DoModal() == IDCANCEL)
			return;

		THEAPP.m_iCurStageIndex = dlg.m_iRadioStageIndex;
#else
		THEAPP.m_iCurStageIndex = STAGE_NUMBER_1;
#endif

		THEAPP.m_iCurTeachVision = VISION_NUMBER_3;
		THEAPP.m_pModelDataManager = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_3];
		UpdateTeachModule();

		THEAPP.m_pCameraManager = THEAPP.m_pDualCameraManager[VISION_NUMBER_3];
		THEAPP.m_pCameraManager->SetGrabCircularIndex(0);

		THEAPP.m_pCameraManagerSpecularMaster = THEAPP.m_pDualCameraManager[VISION_NUMBER_1];

		THEAPP.m_iModeSwitch = MODE_ADMIN_TEACH_VIEW;

		THEAPP.m_pInspectAdminViewHideDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pDefectListDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectSummary->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectResultDlg->ShowWindow(SW_HIDE);

		THEAPP.m_pInspectAdminViewToolbarDlg->UpdateToolbarStatusTeachMode();
		THEAPP.m_pInspectAdminViewToolbarDlg->Show();
		THEAPP.m_pInspectAdminViewDlg->Show();

		//////////////////////////////////////////////////////////////////////////
		///  Change View-port
		THEAPP.m_iCurVisionCamType = VISION_NUMBER_3;
		THEAPP.m_pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_3];

		THEAPP.m_pAlgorithm->m_iPcVisionNo = THEAPP.m_iCurTeachVision;
		THEAPP.m_pAlgorithm->m_iVisionCamType = THEAPP.m_iCurVisionCamType;

		THEAPP.m_pInspectAdminViewDlg->ChangeViewport(THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[VISION_NUMBER_3], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[VISION_NUMBER_3]);
		//////////////////////////////////////////////////////////////////////////

		THEAPP.m_pTabControlDlg->Show();
		THEAPP.m_pTabControlDlg->m_pExtraDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pJogSetDlg->UpdateDlgStatus();
		THEAPP.m_pTabControlDlg->m_pJogSetDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pCameraLightManagerDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pExtraDlg->SetTeachModeControl();
		THEAPP.m_pTabControlDlg->ShowFirstPage(TRUE);
		THEAPP.m_pTabControlDlg->LoadModelConditionParam();
		THEAPP.m_pModelDataManager->UpdateTeachParamView();
		THEAPP.m_pModelDataManager->ClearAiRetrainFlag();
		GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_PREV_TRAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MODE_CHANGE)->EnableWindow(FALSE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_bShowReviewWindow = FALSE;

		THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_NONE;
	}
}

#include "AlignCamSelectDlg.h"

void CuScanView::OnBnClickedMfcbuttonFormTeachingVisionN4()
{
#ifdef INLINE_MODE
	strLog.Format("☆☆☆☆☆ VISION 4 TEACHING Button click ☆☆☆☆☆");
	THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
#endif

	if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_4]->m_sModelName == ".")
		return;

	if (THEAPP.m_bShowReviewWindow)
	{
		if (THEAPP.m_iCurSelectedResultModule != SELECTED_RESULT_NONE)
		{
			if (THEAPP.m_iCurSelectedResultModule != SELECTED_RESULT_VISION_N4)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessageBox.Format("선택한 모듈의 비전이 티칭 비전과 일치하지 않습니다.");
				else
					strMessageBox.Format("The selected module's vision does not match the teaching vision.");
				AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

				THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_NONE;
				return;
			}
		}
	}

	if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN)  // ������ ��� �ƴϸ� ȭ�� ��ȯ �ȵ�.
	{
#if defined(SINGLE_LENS)
		THEAPP.m_iCurStageIndex = STAGE_NUMBER_1;
#elif defined(ASSY_LENS)
		THEAPP.m_iCurStageIndex = STAGE_NUMBER_1;

		CAlignCamSelectDlg dlg;
		if (dlg.DoModal() == IDCANCEL)
			return;

		THEAPP.m_iCurAlignCamIndex = dlg.m_iRadioAlignCamIndex;
#else
		CStageSelectDlg dlg;
		if (dlg.DoModal() == IDCANCEL)
			return;

		THEAPP.m_iCurStageIndex = dlg.m_iRadioStageIndex;
#endif

		THEAPP.m_iCurTeachVision = VISION_NUMBER_4;
		THEAPP.m_pModelDataManager = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_4];
		UpdateTeachModule();

#ifdef ASSY_LENS
		if (THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_BOTTOM_1)
			THEAPP.m_pCameraManager = THEAPP.m_pDualCameraManager[VISION_NUMBER_4];
		else if (THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_BOTTOM_2)
			THEAPP.m_pCameraManager = THEAPP.m_pSubCameraManager[SUB_CAM_1];
		else if (THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_TOP)
			THEAPP.m_pCameraManager = THEAPP.m_pSubCameraManager[SUB_CAM_2];
		else
			return;
#else
		THEAPP.m_pCameraManager = THEAPP.m_pDualCameraManager[VISION_NUMBER_4];
#endif

		THEAPP.m_pCameraManager->SetGrabCircularIndex(0);

		THEAPP.m_pCameraManagerSpecularMaster = THEAPP.m_pDualCameraManager[VISION_NUMBER_1];

		THEAPP.m_iModeSwitch = MODE_ADMIN_TEACH_VIEW;

		THEAPP.m_pInspectAdminViewHideDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectAdminViewHideDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pDefectListDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectSummary->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectResultDlg->ShowWindow(SW_HIDE);

		THEAPP.m_pInspectAdminViewToolbarDlg->UpdateToolbarStatusTeachMode();
		THEAPP.m_pInspectAdminViewToolbarDlg->Show();
		THEAPP.m_pInspectAdminViewDlg->Show();

		//////////////////////////////////////////////////////////////////////////
		///  Change View-port
		THEAPP.m_iCurVisionCamType = VISION_NUMBER_4;
		THEAPP.m_pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_4];

		THEAPP.m_pAlgorithm->m_iPcVisionNo = THEAPP.m_iCurTeachVision;
		THEAPP.m_pAlgorithm->m_iVisionCamType = THEAPP.m_iCurVisionCamType;

		THEAPP.m_pInspectAdminViewDlg->ChangeViewport(THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[VISION_NUMBER_4], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[VISION_NUMBER_4]);
		//////////////////////////////////////////////////////////////////////////

		THEAPP.m_pTabControlDlg->Show();
		THEAPP.m_pTabControlDlg->m_pExtraDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pJogSetDlg->UpdateDlgStatus();
		THEAPP.m_pTabControlDlg->m_pJogSetDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pCameraLightManagerDlg->LoadViewParam();
		THEAPP.m_pTabControlDlg->m_pExtraDlg->SetTeachModeControl();
		THEAPP.m_pTabControlDlg->ShowFirstPage(TRUE);
		THEAPP.m_pTabControlDlg->LoadModelConditionParam();
		THEAPP.m_pModelDataManager->UpdateTeachParamView();
		THEAPP.m_pModelDataManager->ClearAiRetrainFlag();
		GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_PREV_TRAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MODE_CHANGE)->EnableWindow(FALSE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_bShowReviewWindow = FALSE;

		THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_NONE;
	}
}

void CuScanView::OnBnClickedMfcbuttonFormPreference()
{
#ifdef INLINE_MODE
	strLog.Format("☆☆☆☆☆ PREFERENCE Button click ☆☆☆☆☆");
	THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
#endif

	PreferenceDlg.m_strEquipNo = THEAPP.Struct_PreferenceStruct.m_strEquipNo;
	PreferenceDlg.m_strEditEquipModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	PreferenceDlg.m_iRadioPCType = THEAPP.Struct_PreferenceStruct.m_iPCType;

	PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_1] = THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_1];
	PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_2] = THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_2];
	PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_3] = THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_3];
	PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_4] = THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_4];

	PreferenceDlg.m_bCheckUseLAS1 = THEAPP.Struct_PreferenceStruct.m_bUseLAS1;
	PreferenceDlg.m_bCheckUseLAS2 = THEAPP.Struct_PreferenceStruct.m_bUseLAS2;
	PreferenceDlg.m_bCheckUseLAS3 = THEAPP.Struct_PreferenceStruct.m_bUseLAS3;

	PreferenceDlg.m_strSaveFolderPathLAS1 = THEAPP.Struct_PreferenceStruct.m_strSaveFolderPathLAS1;
	PreferenceDlg.m_strSaveFolderPathLAS2 = THEAPP.Struct_PreferenceStruct.m_strSaveFolderPathLAS2;
	PreferenceDlg.m_strSaveFolderPathLAS3 = THEAPP.Struct_PreferenceStruct.m_strSaveFolderPathLAS3;

	PreferenceDlg.m_bCheckUseLASConnectionCheck = THEAPP.Struct_PreferenceStruct.m_bUseLASConnectionCheck;

	PreferenceDlg.m_strRawFolderPath = THEAPP.Struct_PreferenceStruct.m_strRawFolderPath;
	PreferenceDlg.m_strResultFolderPath = THEAPP.Struct_PreferenceStruct.m_strResultFolderPath;
	PreferenceDlg.m_strReviewFolderPath = THEAPP.Struct_PreferenceStruct.m_strReviewFolderPath;
	PreferenceDlg.m_strAdjFolderPath = THEAPP.Struct_PreferenceStruct.m_strAdjFolderPath;
	PreferenceDlg.m_strResultLogFolderPath = THEAPP.Struct_PreferenceStruct.m_strResultLogFolderPath;
	PreferenceDlg.m_strEtcFolderPath = THEAPP.Struct_PreferenceStruct.m_strEtcFolderPath;
	PreferenceDlg.m_strOfflineTestFolderPath = THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath;

	PreferenceDlg.m_bCheckRawImageSaveLocal = THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal;
	PreferenceDlg.m_bCheckResultImageSaveLocal = THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal;
	PreferenceDlg.m_bCheckReviewImageSaveLocal = THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal;
	PreferenceDlg.m_bCheckADJImageSaveLocal = THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal;
	PreferenceDlg.m_bCheckResultLogSaveLocal = THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal;

	PreferenceDlg.m_bCheckRawImageSaveLAS = THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS;
	PreferenceDlg.m_bCheckResultImageSaveLAS = THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS;
	PreferenceDlg.m_bCheckReviewImageSaveLAS = THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS;
	PreferenceDlg.m_bCheckADJImageSaveLAS = THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS;
	PreferenceDlg.m_bCheckResultLogSaveLAS = THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS;

	PreferenceDlg.m_bCheckUseRawImageThread = THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread;
	PreferenceDlg.m_bCheckUseResultImageThread = THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread;
	PreferenceDlg.m_bCheckUseReviewImageThread = THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread;
	PreferenceDlg.m_bCheckUseADJImageThread = THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread;
	PreferenceDlg.m_bCheckUseResultLogThread = THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread;

	PreferenceDlg.m_bCheckUseRawImageTempDrive = THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive;

	PreferenceDlg.m_bCheckSaveRawImage = THEAPP.Struct_PreferenceStruct.m_bSaveRawImage;
	PreferenceDlg.m_bRadioRawImageType = THEAPP.Struct_PreferenceStruct.m_bRawImageType;
	PreferenceDlg.m_bCheckReduceRawImage = THEAPP.Struct_PreferenceStruct.m_bReduceRawImage;
	PreferenceDlg.m_dEditRawImageZoomRatio = THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio;
	PreferenceDlg.m_bCheckCombineRawImage = THEAPP.Struct_PreferenceStruct.m_bCombineRawImage;
	PreferenceDlg.m_bCheckSaveResultGoodImage = THEAPP.Struct_PreferenceStruct.m_bSaveResultGoodImage;
	PreferenceDlg.m_bCheckUseResultImageThreadOnlyGood = THEAPP.Struct_PreferenceStruct.m_bUseResultImageThreadOnlyGood;
	PreferenceDlg.m_bRadioRawImageSaveLogType = THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType;
	PreferenceDlg.m_bCheckAvoidDuplicateLotLocal = THEAPP.Struct_PreferenceStruct.m_bAvoidDuplicateLotLocal;
	PreferenceDlg.m_bCheckAvoidDuplicateLotLas = THEAPP.Struct_PreferenceStruct.m_bAvoidDuplicateLotLas;

	PreferenceDlg.m_iRadioRawImageThreadType = THEAPP.Struct_PreferenceStruct.m_iRawImageThreadType;

	PreferenceDlg.m_bCheckUseMultiQueue = THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue;
	PreferenceDlg.m_iEditThreadQueueNumber = THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;

	PreferenceDlg.m_iRadioResultImageDefectDisplayType = THEAPP.Struct_PreferenceStruct.m_iResultImageDefectDisplayType;
	PreferenceDlg.m_bCheckUseResultImageDefectSizeLimit = THEAPP.Struct_PreferenceStruct.m_bUseResultImageDefectSizeLimit;
	PreferenceDlg.m_iEditResultImageDefectSizeMax = THEAPP.Struct_PreferenceStruct.m_iResultImageDefectSizeMax;

	PreferenceDlg.m_bCheckSaveADJ = THEAPP.Struct_PreferenceStruct.m_bSaveADJ;
	PreferenceDlg.m_bCheckUseADJConnectionCheck = THEAPP.Struct_PreferenceStruct.m_bUseADJConnectionCheck;
	PreferenceDlg.m_bCheckUseLocalADJ = THEAPP.Struct_PreferenceStruct.m_bUseLocalADJ;
	PreferenceDlg.m_bCheckUseLocalSEG = THEAPP.Struct_PreferenceStruct.m_bUseLocalSEG;
	PreferenceDlg.m_strEditAdjCurrentIP = THEAPP.Struct_PreferenceStruct.m_strCurrentADJ_IP;
	PreferenceDlg.m_bCheckUseADJ[0] = THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[0];
	PreferenceDlg.m_strADJ_IP[0] = THEAPP.Struct_PreferenceStruct.m_strADJ_IP[0];
	PreferenceDlg.m_nEditAdjPort[0] = THEAPP.Struct_PreferenceStruct.m_iADJ_Port[0];
	PreferenceDlg.m_bCheckUseADJ[1] = THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[1];
	PreferenceDlg.m_strADJ_IP[1] = THEAPP.Struct_PreferenceStruct.m_strADJ_IP[1];
	PreferenceDlg.m_nEditAdjPort[1] = THEAPP.Struct_PreferenceStruct.m_iADJ_Port[1];
	PreferenceDlg.m_bCheckIsSendADJ = THEAPP.Struct_PreferenceStruct.m_bIsSendADJ;
	PreferenceDlg.m_bCheckUseADJMultiimage = THEAPP.Struct_PreferenceStruct.m_bUseADJMultiimage;
	PreferenceDlg.m_bCheckIsApplyADJ = THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ;
	PreferenceDlg.m_bCheckUseADJTimeOut = THEAPP.Struct_PreferenceStruct.m_bUseADJTimeOut;
	PreferenceDlg.m_nADJTimeOut = THEAPP.Struct_PreferenceStruct.m_nADJTimeOut;
	PreferenceDlg.m_nEditADJMultiNetworkNO = THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO;
	PreferenceDlg.m_iEditADJImageSize = THEAPP.Struct_PreferenceStruct.m_iADJImageSize;
	PreferenceDlg.m_iEditADJImageReSize = THEAPP.Struct_PreferenceStruct.m_iADJImageReSize;
	PreferenceDlg.m_iEditAdjMarkingRad = THEAPP.Struct_PreferenceStruct.m_iMarkingImageRadius;
	PreferenceDlg.m_iEditADJCombineDefectDistance = THEAPP.Struct_PreferenceStruct.m_iADJCombineDefectDistance;
	PreferenceDlg.m_bCheckSaveROIImage = THEAPP.Struct_PreferenceStruct.m_bSaveROIImage;
	PreferenceDlg.m_iRadioSaveROIImageType = THEAPP.Struct_PreferenceStruct.m_iSaveROIImageType;
	PreferenceDlg.m_bCheckAISetupUseVAL = THEAPP.Struct_PreferenceStruct.m_bAISetupUseVAL;
	PreferenceDlg.m_bCheckAllImageAi = THEAPP.Struct_PreferenceStruct.m_bAllImageAiInspection;

	PreferenceDlg.m_bCheckSaveMDJ = THEAPP.Struct_PreferenceStruct.m_bSaveMDJ;
	PreferenceDlg.m_iEditMaxReviewImageNumber = THEAPP.Struct_PreferenceStruct.m_iMaxReviewImageNumber;
	PreferenceDlg.m_iEditReviewCombineDefectDistance = THEAPP.Struct_PreferenceStruct.m_iReviewCombineDefectDistance;
	PreferenceDlg.m_bCheckUseSaveFaiReviewImage = THEAPP.Struct_PreferenceStruct.m_bUseSaveFaiReviewImage;

	PreferenceDlg.m_iRadioReviewImageDisplayType = THEAPP.Struct_PreferenceStruct.m_iReviewImageDisplayType;

	PreferenceDlg.m_iEditVision1LightIP1 = THEAPP.Struct_PreferenceStruct.m_iVision1LightIP1;
	PreferenceDlg.m_iEditVision1LightIP2 = THEAPP.Struct_PreferenceStruct.m_iVision1LightIP2;
	PreferenceDlg.m_iEditVision1LightIP3 = THEAPP.Struct_PreferenceStruct.m_iVision1LightIP3;
	PreferenceDlg.m_iEditVision1LightIP4 = THEAPP.Struct_PreferenceStruct.m_iVision1LightIP4;
	PreferenceDlg.m_iEditVision1LightPort = THEAPP.Struct_PreferenceStruct.m_iVision1LightPort;

	PreferenceDlg.m_iEditVision2LightIP1 = THEAPP.Struct_PreferenceStruct.m_iVision2LightIP1;
	PreferenceDlg.m_iEditVision2LightIP2 = THEAPP.Struct_PreferenceStruct.m_iVision2LightIP2;
	PreferenceDlg.m_iEditVision2LightIP3 = THEAPP.Struct_PreferenceStruct.m_iVision2LightIP3;
	PreferenceDlg.m_iEditVision2LightIP4 = THEAPP.Struct_PreferenceStruct.m_iVision2LightIP4;
	PreferenceDlg.m_iEditVision2LightPort = THEAPP.Struct_PreferenceStruct.m_iVision2LightPort;

	PreferenceDlg.m_iEditVision3LightIP1 = THEAPP.Struct_PreferenceStruct.m_iVision3LightIP1;
	PreferenceDlg.m_iEditVision3LightIP2 = THEAPP.Struct_PreferenceStruct.m_iVision3LightIP2;
	PreferenceDlg.m_iEditVision3LightIP3 = THEAPP.Struct_PreferenceStruct.m_iVision3LightIP3;
	PreferenceDlg.m_iEditVision3LightIP4 = THEAPP.Struct_PreferenceStruct.m_iVision3LightIP4;
	PreferenceDlg.m_iEditVision3LightPort = THEAPP.Struct_PreferenceStruct.m_iVision3LightPort;

	PreferenceDlg.m_iEditVision4LightIP1 = THEAPP.Struct_PreferenceStruct.m_iVision4LightIP1;
	PreferenceDlg.m_iEditVision4LightIP2 = THEAPP.Struct_PreferenceStruct.m_iVision4LightIP2;
	PreferenceDlg.m_iEditVision4LightIP3 = THEAPP.Struct_PreferenceStruct.m_iVision4LightIP3;
	PreferenceDlg.m_iEditVision4LightIP4 = THEAPP.Struct_PreferenceStruct.m_iVision4LightIP4;
	PreferenceDlg.m_iEditVision4LightPort = THEAPP.Struct_PreferenceStruct.m_iVision4LightPort;

	PreferenceDlg.m_iEditGrabErrRetryNo = THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo;
	PreferenceDlg.m_iEditGrabDelayTime = THEAPP.Struct_PreferenceStruct.m_iGrabDelayTime;
	PreferenceDlg.m_iEditTriggerSleepTime = THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime;
	PreferenceDlg.m_iEditGrabDoneWaitTime = THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime;
	PreferenceDlg.m_iEditTeachingLiveGrabPeriod = THEAPP.Struct_PreferenceStruct.m_iTeachingLiveGrabPeriod;
	PreferenceDlg.m_iEditLightErrValue = THEAPP.Struct_PreferenceStruct.m_iLightErrValue;
	PreferenceDlg.m_iRadioTeachingCameraLightType = THEAPP.Struct_PreferenceStruct.m_iTeachingCameraLightType;
	PreferenceDlg.m_bCheckUseGrabHold = THEAPP.Struct_PreferenceStruct.m_bUseGrabHold;
	PreferenceDlg.m_iEditFocusMoveNoRetry = THEAPP.Struct_PreferenceStruct.m_iFocusMoveNoRetry;
	PreferenceDlg.m_iEditMoveCompleteWaitTime = THEAPP.Struct_PreferenceStruct.m_iFocusMoveCompleteWaitTime;
	PreferenceDlg.m_bCheckUse4dSaveRawImage = THEAPP.Struct_PreferenceStruct.m_bUse4dSaveRawImage;
	PreferenceDlg.m_bCheckUseAutoFocus = THEAPP.Struct_PreferenceStruct.m_bUseAutoFocus;
	PreferenceDlg.m_bCheckUseFastGrab = THEAPP.Struct_PreferenceStruct.m_bUseFastGrab;
	PreferenceDlg.m_bCheckUseBatchInspection = THEAPP.Struct_PreferenceStruct.m_bUseBatchInspection;
	PreferenceDlg.m_bCheckSendGrabFailError = THEAPP.Struct_PreferenceStruct.m_bSendGrabFailError;
	PreferenceDlg.m_iEditSendGrabFailErrorCount = THEAPP.Struct_PreferenceStruct.m_iSendGrabFailErrorCount;

	PreferenceDlg.m_iDefectDispDist = THEAPP.Struct_PreferenceStruct.m_iDefectDispDist;
	PreferenceDlg.m_iEditMatchingSearchMargin = THEAPP.Struct_PreferenceStruct.m_iMatchingSearchMargin;
	PreferenceDlg.m_iRadioBarcodeNoReadImageType = THEAPP.Struct_PreferenceStruct.m_iBarcodeNoReadImageType;
	PreferenceDlg.m_bCheckUseSaveDefectShapeFeature = THEAPP.Struct_PreferenceStruct.m_bUseSaveDefectShapeFeature;
	PreferenceDlg.m_iEditMaxSameDefectShapeFeature = THEAPP.Struct_PreferenceStruct.m_iMaxSameDefectShapeFeature;
	PreferenceDlg.m_bCheckUseSaveGVEdgeMornitoringLog = THEAPP.Struct_PreferenceStruct.m_bUseSaveGVEdgeMornitoringLog;
	PreferenceDlg.m_bCheckUseGpuAffineTrans = THEAPP.Struct_PreferenceStruct.m_bUseGpuAffineTrans;
	PreferenceDlg.m_bCheckUseDeleteCrossBar = THEAPP.Struct_PreferenceStruct.m_bUseDeleteCrossBar;
	PreferenceDlg.m_iEditSaveRecentlyCompleteInfoNumber = THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber;

	PreferenceDlg.m_bCheckUseContDefectAlarm = THEAPP.Struct_PreferenceStruct.m_bUseContDefectAlarm;
	PreferenceDlg.m_iEditContDefectPosTolerance = THEAPP.Struct_PreferenceStruct.m_iContDefectPosTolerance;
	PreferenceDlg.m_iEditContDefectCount = THEAPP.Struct_PreferenceStruct.m_iContDefectCount;

	PreferenceDlg.m_iEditSam2IP_1 = THEAPP.Struct_PreferenceStruct.m_iSam2IP_1;
	PreferenceDlg.m_iEditSam2IP_2 = THEAPP.Struct_PreferenceStruct.m_iSam2IP_2;
	PreferenceDlg.m_iEditSam2IP_3 = THEAPP.Struct_PreferenceStruct.m_iSam2IP_3;
	PreferenceDlg.m_iEditSam2IP_4 = THEAPP.Struct_PreferenceStruct.m_iSam2IP_4;
	PreferenceDlg.m_iEditSam2Port = THEAPP.Struct_PreferenceStruct.m_iSam2Port;

	PreferenceDlg.m_bCheckUseVisionCopy = THEAPP.Struct_PreferenceStruct.m_bUseVisionCopy;

	PreferenceDlg.m_iRadioVmTrain = THEAPP.Struct_PreferenceStruct.m_iVmTrain;
	PreferenceDlg.m_bCheckVmTrainAll = THEAPP.Struct_PreferenceStruct.m_bVmTrainAll;

	PreferenceDlg.m_bCheckUseVmThresOpt = THEAPP.Struct_PreferenceStruct.m_bUseVmThresOpt;
	PreferenceDlg.m_iEditVmSplitThres = THEAPP.Struct_PreferenceStruct.m_iVmSplitThres;
	PreferenceDlg.m_dEditVmKSigma = THEAPP.Struct_PreferenceStruct.m_dVmKSigma;

	if (PreferenceDlg.DoModal() == IDOK)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("저장하시겠습니까?");
		else
			strMessageBox.Format("Apply changes?");

		if (AfxMessageBox(strMessageBox, MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK)
		{
			CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
			CIniFileCS INI_Preference(strDataFolder + "Preference.ini");
			CString strSection, strKey;

			strSection = "EQUIPMENT";
			INI_Preference.Set_String(strSection, "Equip No", PreferenceDlg.m_strEquipNo);

			strSection = "PC NUMBER";
			INI_Preference.Set_Integer(strSection, "PC No", PreferenceDlg.m_iRadioPCType);

			strSection = "INSPECT VISION";
			INI_Preference.Set_Bool(strSection, "Use Vision1", PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_1]);
			INI_Preference.Set_Bool(strSection, "Use Vision2", PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_2]);
			INI_Preference.Set_Bool(strSection, "Use Vision3", PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_3]);
			INI_Preference.Set_Bool(strSection, "Use Vision4", PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_4]);

			strSection = "FOLDER PATH";
			INI_Preference.Set_Bool(strSection, "Use LAS1", PreferenceDlg.m_bCheckUseLAS1);
			INI_Preference.Set_Bool(strSection, "Use LAS2", PreferenceDlg.m_bCheckUseLAS2);
			INI_Preference.Set_Bool(strSection, "Use LAS3", PreferenceDlg.m_bCheckUseLAS3);

			INI_Preference.Set_String(strSection, "Save Folder Path LAS1", PreferenceDlg.m_strSaveFolderPathLAS1);
			INI_Preference.Set_String(strSection, "Save Folder Path LAS2", PreferenceDlg.m_strSaveFolderPathLAS2);
			INI_Preference.Set_String(strSection, "Save Folder Path LAS3", PreferenceDlg.m_strSaveFolderPathLAS3);

			INI_Preference.Set_Bool(strSection, "Send LAS Connect Error Massage", PreferenceDlg.m_bCheckUseLASConnectionCheck);

			INI_Preference.Set_String(strSection, "Save Raw Folder Path", PreferenceDlg.m_strRawFolderPath);
			INI_Preference.Set_String(strSection, "Save Result Folder Path", PreferenceDlg.m_strResultFolderPath);
			INI_Preference.Set_String(strSection, "Save Review Folder Path", PreferenceDlg.m_strReviewFolderPath);
			INI_Preference.Set_String(strSection, "Save ADJ Folder Path", PreferenceDlg.m_strAdjFolderPath);
			INI_Preference.Set_String(strSection, "Save Result Log Folder Path", PreferenceDlg.m_strResultLogFolderPath);
			INI_Preference.Set_String(strSection, "Save ETC Folder Path", PreferenceDlg.m_strEtcFolderPath);
			INI_Preference.Set_String(strSection, "Offline Test Folder Path", PreferenceDlg.m_strOfflineTestFolderPath);

			strSection = "IMAGE SAVE OPTION";
			INI_Preference.Set_Bool(strSection, "Raw Image Save Local", PreferenceDlg.m_bCheckRawImageSaveLocal);
			INI_Preference.Set_Bool(strSection, "Result Image Save Local", PreferenceDlg.m_bCheckResultImageSaveLocal);
			INI_Preference.Set_Bool(strSection, "Review Image Save Local", PreferenceDlg.m_bCheckReviewImageSaveLocal);
			INI_Preference.Set_Bool(strSection, "ADJ Image Save Local", PreferenceDlg.m_bCheckADJImageSaveLocal);
			INI_Preference.Set_Bool(strSection, "Result Log Save Local", PreferenceDlg.m_bCheckResultLogSaveLocal);

			INI_Preference.Set_Bool(strSection, "Raw Image Save LAS", PreferenceDlg.m_bCheckRawImageSaveLAS);
			INI_Preference.Set_Bool(strSection, "Result Image Save LAS", PreferenceDlg.m_bCheckResultImageSaveLAS);
			INI_Preference.Set_Bool(strSection, "Review Image Save LAS", PreferenceDlg.m_bCheckReviewImageSaveLAS);
			INI_Preference.Set_Bool(strSection, "ADJ Image Save LAS", PreferenceDlg.m_bCheckADJImageSaveLAS);
			INI_Preference.Set_Bool(strSection, "Result Log Save LAS", PreferenceDlg.m_bCheckResultLogSaveLAS);

			INI_Preference.Set_Bool(strSection, "Use Raw Image Thread", PreferenceDlg.m_bCheckUseRawImageThread);
			INI_Preference.Set_Bool(strSection, "Use Result Image Thread", PreferenceDlg.m_bCheckUseResultImageThread);
			INI_Preference.Set_Bool(strSection, "Use Review Image Thread", PreferenceDlg.m_bCheckUseReviewImageThread);
			INI_Preference.Set_Bool(strSection, "Use ADJ Image Thread", PreferenceDlg.m_bCheckUseADJImageThread);
			INI_Preference.Set_Bool(strSection, "Use Result Log Thread", PreferenceDlg.m_bCheckUseResultLogThread);

			INI_Preference.Set_Bool(strSection, "Use Raw Image Temporary Drive", PreferenceDlg.m_bCheckUseRawImageTempDrive);

			INI_Preference.Set_Bool(strSection, "Save Raw Image", PreferenceDlg.m_bCheckSaveRawImage);
			INI_Preference.Set_Bool(strSection, "Raw Image Type(JPG)", PreferenceDlg.m_bRadioRawImageType);
			INI_Preference.Set_Bool(strSection, "Reduce Raw Image", PreferenceDlg.m_bCheckReduceRawImage);
			INI_Preference.Set_Double(strSection, "Reduce Ratio", PreferenceDlg.m_dEditRawImageZoomRatio);
			INI_Preference.Set_Bool(strSection, "Combine Raw Image", PreferenceDlg.m_bCheckCombineRawImage);
			INI_Preference.Set_Bool(strSection, "Save Result Good Image", PreferenceDlg.m_bCheckSaveResultGoodImage);
			INI_Preference.Set_Bool(strSection, "Use Result Image Thread Only Good Sample", PreferenceDlg.m_bCheckUseResultImageThreadOnlyGood);
			INI_Preference.Set_Bool(strSection, "RawImage Save Log Type", PreferenceDlg.m_bRadioRawImageSaveLogType);
			INI_Preference.Set_Bool(strSection, "Avoid Duplicate Lot Local", PreferenceDlg.m_bCheckAvoidDuplicateLotLocal);
			INI_Preference.Set_Bool(strSection, "Avoid Duplicate Lot LAS", PreferenceDlg.m_bCheckAvoidDuplicateLotLas);

			INI_Preference.Set_Integer(strSection, "Raw Image Thread Type", PreferenceDlg.m_iRadioRawImageThreadType);

			if (PreferenceDlg.m_iEditThreadQueueNumber <= 0 ||
				PreferenceDlg.m_iEditThreadQueueNumber > IMAGE_SAVE_THREAD_MAX_QUEUE)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessageBox.Format("저장 쓰레드 리스트 수는 1개부터 %d개까지 가능합니다.");
				else
					strMessageBox.Format("The number of save thread lists must be between 1 and %d.", IMAGE_SAVE_THREAD_MAX_QUEUE);
				AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

				PreferenceDlg.m_iEditThreadQueueNumber = IMAGE_SAVE_THREAD_MAX_QUEUE;
			}
			INI_Preference.Set_Bool(strSection, "Use Multi Queue", PreferenceDlg.m_bCheckUseMultiQueue);
			INI_Preference.Set_Integer(strSection, "Thread Queue Number", PreferenceDlg.m_iEditThreadQueueNumber);

			strSection = "RESULT IMAGE DISPLAY";
			INI_Preference.Set_Integer(strSection, "Result Image Defect Display Type", PreferenceDlg.m_iRadioResultImageDefectDisplayType);
			INI_Preference.Set_Bool(strSection, "Use Result Image Defect Size Limit", PreferenceDlg.m_bCheckUseResultImageDefectSizeLimit);
			INI_Preference.Set_Integer(strSection, "Result Image Defect Size Max", PreferenceDlg.m_iEditResultImageDefectSizeMax);

			strSection = "ADJ OPTION";
			INI_Preference.Set_Bool(strSection, "Save ADJ", PreferenceDlg.m_bCheckSaveADJ);
			INI_Preference.Set_Bool(strSection, "Use ADJ Connection Check", PreferenceDlg.m_bCheckUseADJConnectionCheck);
			INI_Preference.Set_Bool(strSection, "Use Local ADJ", PreferenceDlg.m_bCheckUseLocalADJ);
			INI_Preference.Set_Bool(strSection, "Use Local Segmentation", PreferenceDlg.m_bCheckUseLocalSEG);
			INI_Preference.Set_String(strSection, "AVI IP", PreferenceDlg.m_strEditAdjCurrentIP);
			INI_Preference.Set_Bool(strSection, "Use ADJ1", PreferenceDlg.m_bCheckUseADJ[0]);
			INI_Preference.Set_String(strSection, "ADJ IP1", PreferenceDlg.m_strADJ_IP[0]);
			INI_Preference.Set_Integer(strSection, "ADJ Port1", PreferenceDlg.m_nEditAdjPort[0]);
			INI_Preference.Set_Bool(strSection, "Use ADJ2", PreferenceDlg.m_bCheckUseADJ[1]);
			INI_Preference.Set_String(strSection, "ADJ IP2", PreferenceDlg.m_strADJ_IP[1]);
			INI_Preference.Set_Integer(strSection, "ADJ Port2", PreferenceDlg.m_nEditAdjPort[1]);
			INI_Preference.Set_Bool(strSection, "Send ADJ", PreferenceDlg.m_bCheckIsSendADJ);
			INI_Preference.Set_Bool(strSection, "Use ADJ Multi Image", PreferenceDlg.m_bCheckUseADJMultiimage);
			INI_Preference.Set_Bool(strSection, "Apply ADJ", PreferenceDlg.m_bCheckIsApplyADJ);
			INI_Preference.Set_Bool(strSection, "Use ADJ Timeout", PreferenceDlg.m_bCheckUseADJTimeOut);
			INI_Preference.Set_Integer(strSection, "ADJ Judge Time", PreferenceDlg.m_nADJTimeOut);
			INI_Preference.Set_Integer(strSection, "ADJ Multi Network Number", PreferenceDlg.m_nEditADJMultiNetworkNO);
			INI_Preference.Set_Integer(strSection, "ADJ Image Size", PreferenceDlg.m_iEditADJImageSize);
			INI_Preference.Set_Integer(strSection, "ADJ Image Resize", PreferenceDlg.m_iEditADJImageReSize);
			INI_Preference.Set_Integer(strSection, "ADJ Marking Radius", PreferenceDlg.m_iEditAdjMarkingRad);
			INI_Preference.Set_Integer(strSection, "ADJ Combine Defect Distance", PreferenceDlg.m_iEditADJCombineDefectDistance);
			INI_Preference.Set_Bool(strSection, "Save ROI Image", PreferenceDlg.m_bCheckSaveROIImage);
			INI_Preference.Set_Integer(strSection, "Save ROI Image Type", PreferenceDlg.m_iRadioSaveROIImageType);
			INI_Preference.Set_Bool(strSection, "AI Setup Use Validation", PreferenceDlg.m_bCheckAISetupUseVAL);
			INI_Preference.Set_Bool(strSection, "ALL Image AI Inspection", PreferenceDlg.m_bCheckAllImageAi);
#ifdef USE_SUAKIT
			for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
			{
				if (!PreferenceDlg.m_bCheckUseADJ[i])
				{
					for (int j = 0; j < THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO; j++)
					{
						if (THEAPP.m_TCPClientService.m_bConnect[i][j])
						{
							THEAPP.m_TCPClientService.m_arrClient[i][j].TCPonClose();
							THEAPP.m_TCPClientService.m_bConnect[i][j] = FALSE;
						}
					}
				}
			}
#endif

			strSection = "MDJ OPTION";
			INI_Preference.Set_Bool(strSection, "Save MDJ", PreferenceDlg.m_bCheckSaveMDJ);
			INI_Preference.Set_Integer(strSection, "Max Review Image Number", PreferenceDlg.m_iEditMaxReviewImageNumber);
			INI_Preference.Set_Integer(strSection, "Review Combine Defect Distance", PreferenceDlg.m_iEditReviewCombineDefectDistance);
			INI_Preference.Set_Bool(strSection, "Save FAI Review Image", PreferenceDlg.m_bCheckUseSaveFaiReviewImage);

			INI_Preference.Set_Integer(strSection, "Review Image Display Type", PreferenceDlg.m_iRadioReviewImageDisplayType);

			strSection = "LIGHT CONTROLLER OPTION";
			INI_Preference.Set_Integer(strSection, "Vision1 Light IP1", PreferenceDlg.m_iEditVision1LightIP1);
			INI_Preference.Set_Integer(strSection, "Vision1 Light IP2", PreferenceDlg.m_iEditVision1LightIP2);
			INI_Preference.Set_Integer(strSection, "Vision1 Light IP3", PreferenceDlg.m_iEditVision1LightIP3);
			INI_Preference.Set_Integer(strSection, "Vision1 Light IP4", PreferenceDlg.m_iEditVision1LightIP4);
			INI_Preference.Set_Integer(strSection, "Vision1 Light Port", PreferenceDlg.m_iEditVision1LightPort);

			INI_Preference.Set_Integer(strSection, "Vision2 Light IP1", PreferenceDlg.m_iEditVision2LightIP1);
			INI_Preference.Set_Integer(strSection, "Vision2 Light IP2", PreferenceDlg.m_iEditVision2LightIP2);
			INI_Preference.Set_Integer(strSection, "Vision2 Light IP3", PreferenceDlg.m_iEditVision2LightIP3);
			INI_Preference.Set_Integer(strSection, "Vision2 Light IP4", PreferenceDlg.m_iEditVision2LightIP4);
			INI_Preference.Set_Integer(strSection, "Vision2 Light Port", PreferenceDlg.m_iEditVision2LightPort);

			INI_Preference.Set_Integer(strSection, "Vision3 Light IP1", PreferenceDlg.m_iEditVision3LightIP1);
			INI_Preference.Set_Integer(strSection, "Vision3 Light IP2", PreferenceDlg.m_iEditVision3LightIP2);
			INI_Preference.Set_Integer(strSection, "Vision3 Light IP3", PreferenceDlg.m_iEditVision3LightIP3);
			INI_Preference.Set_Integer(strSection, "Vision3 Light IP4", PreferenceDlg.m_iEditVision3LightIP4);
			INI_Preference.Set_Integer(strSection, "Vision3 Light Port", PreferenceDlg.m_iEditVision3LightPort);

			INI_Preference.Set_Integer(strSection, "Vision4 Light IP1", PreferenceDlg.m_iEditVision4LightIP1);
			INI_Preference.Set_Integer(strSection, "Vision4 Light IP2", PreferenceDlg.m_iEditVision4LightIP2);
			INI_Preference.Set_Integer(strSection, "Vision4 Light IP3", PreferenceDlg.m_iEditVision4LightIP3);
			INI_Preference.Set_Integer(strSection, "Vision4 Light IP4", PreferenceDlg.m_iEditVision4LightIP4);
			INI_Preference.Set_Integer(strSection, "Vision4 Light Port", PreferenceDlg.m_iEditVision4LightPort);

			strSection = "GRAB OPTION";
			INI_Preference.Set_Integer(strSection, "Grab Error Retry Number", PreferenceDlg.m_iEditGrabErrRetryNo);
			INI_Preference.Set_Integer(strSection, "Grab Delay Time", PreferenceDlg.m_iEditGrabDelayTime);
			INI_Preference.Set_Integer(strSection, "Trigger Sleep Time", PreferenceDlg.m_iEditTriggerSleepTime);
			THEAPP.m_pTriggerManager->SetTriggerSleepTime(PreferenceDlg.m_iEditTriggerSleepTime);
			INI_Preference.Set_Integer(strSection, "Grab Done Wait Time", PreferenceDlg.m_iEditGrabDoneWaitTime);
			INI_Preference.Set_Integer(strSection, "Teaching Live Grab Period", PreferenceDlg.m_iEditTeachingLiveGrabPeriod);
			INI_Preference.Set_Integer(strSection, "Light Error Value", PreferenceDlg.m_iEditLightErrValue);
			INI_Preference.Set_Integer(strSection, "Teaching Camera Light Type", PreferenceDlg.m_iRadioTeachingCameraLightType);
			INI_Preference.Set_Bool(strSection, "Use Grab Hold", PreferenceDlg.m_bCheckUseGrabHold);
			INI_Preference.Set_Integer(strSection, "Focus Move Retry Number", PreferenceDlg.m_iEditFocusMoveNoRetry);
			INI_Preference.Set_Integer(strSection, "Focus Move Wait Time", PreferenceDlg.m_iEditMoveCompleteWaitTime);
			INI_Preference.Set_Bool(strSection, "Save 4D Raw Image", PreferenceDlg.m_bCheckUse4dSaveRawImage);
			INI_Preference.Set_Bool(strSection, "Use Auto Focus", PreferenceDlg.m_bCheckUseAutoFocus);
			INI_Preference.Set_Bool(strSection, "Use Fast Grab", PreferenceDlg.m_bCheckUseFastGrab);
			INI_Preference.Set_Bool(strSection, "Use Batch Inspection", PreferenceDlg.m_bCheckUseBatchInspection);
			INI_Preference.Set_Bool(strSection, "Send Grab Fail Error", PreferenceDlg.m_bCheckSendGrabFailError);
			INI_Preference.Set_Integer(strSection, "Send Grab Fail Error Count", PreferenceDlg.m_iEditSendGrabFailErrorCount);

			strSection = "OPTION ETC";
			INI_Preference.Set_Integer(strSection, "Defect Display Dist", PreferenceDlg.m_iDefectDispDist);
			INI_Preference.Set_Integer(strSection, "Matching Search Margin", PreferenceDlg.m_iEditMatchingSearchMargin);
			INI_Preference.Set_Integer(strSection, "Barcode No Read Image Type(JPG)", PreferenceDlg.m_iRadioBarcodeNoReadImageType);
			INI_Preference.Set_Bool(strSection, "Save Defect Shape Feature", PreferenceDlg.m_bCheckUseSaveDefectShapeFeature);
			INI_Preference.Set_Integer(strSection, "Max Same Defect Shape Feature", PreferenceDlg.m_iEditMaxSameDefectShapeFeature);
			INI_Preference.Set_Bool(strSection, "Save GV Edge Mornitoring Log", PreferenceDlg.m_bCheckUseSaveGVEdgeMornitoringLog);
			INI_Preference.Set_Bool(strSection, "Use GPU Affine Trans", PreferenceDlg.m_bCheckUseGpuAffineTrans);
			INI_Preference.Set_Bool(strSection, "Use Delete Cross Bar ROI", PreferenceDlg.m_bCheckUseDeleteCrossBar);
			INI_Preference.Set_Integer(strSection, "Save Recently Complete Info Number", PreferenceDlg.m_iEditSaveRecentlyCompleteInfoNumber);

			strSection = "CONTINUOUS DEFECT ALARM OPTION";
			INI_Preference.Set_Bool(strSection, "Use Continuous Defect Alarm", PreferenceDlg.m_bCheckUseContDefectAlarm);
			INI_Preference.Set_Double(strSection, "Continuous Defect Pos Tolerance", PreferenceDlg.m_iEditContDefectPosTolerance);
			if (PreferenceDlg.m_iEditContDefectCount < 3)
				PreferenceDlg.m_iEditContDefectCount = 3;
			INI_Preference.Set_Double(strSection, "Continuous Defect Count", PreferenceDlg.m_iEditContDefectCount);

			strSection = "SAM2 OPTION";
			INI_Preference.Set_Integer(strSection, "SAM2 IP1", PreferenceDlg.m_iEditSam2IP_1);
			INI_Preference.Set_Integer(strSection, "SAM2 IP2", PreferenceDlg.m_iEditSam2IP_2);
			INI_Preference.Set_Integer(strSection, "SAM2 IP3", PreferenceDlg.m_iEditSam2IP_3);
			INI_Preference.Set_Integer(strSection, "SAM2 IP4", PreferenceDlg.m_iEditSam2IP_4);
			INI_Preference.Set_Integer(strSection, "SAM2 Port", PreferenceDlg.m_iEditSam2Port);

			strSection = "ROI COPY OPTION";
			INI_Preference.Set_Bool(strSection, "Use Vision Copy", PreferenceDlg.m_bCheckUseVisionCopy);

			strSection = "IMAGE COMPARE TRAIN OPTION";
			INI_Preference.Set_Integer(strSection, "VM TRAIN", PreferenceDlg.m_iRadioVmTrain);
			INI_Preference.Set_Bool(strSection, "VM ALL TRAINING", PreferenceDlg.m_bCheckVmTrainAll);

			INI_Preference.Set_Bool(strSection, "USE VM THRES OPT", PreferenceDlg.m_bCheckUseVmThresOpt);
			INI_Preference.Set_Integer(strSection, "VM SPLIT THRES", PreferenceDlg.m_iEditVmSplitThres);
			INI_Preference.Set_Double(strSection, "VM K SIGMA", PreferenceDlg.m_dEditVmKSigma);

			THEAPP.Struct_PreferenceStruct.m_strEquipNo = PreferenceDlg.m_strEquipNo;

			THEAPP.Struct_PreferenceStruct.m_iPCType = PreferenceDlg.m_iRadioPCType;

			THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_1] = PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_1];
			THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_2] = PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_2];
			THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_3] = PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_3];
			THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_4] = PreferenceDlg.m_bCheckUseVision[VISION_NUMBER_4];

			THEAPP.Struct_PreferenceStruct.m_bUseLAS1 = PreferenceDlg.m_bCheckUseLAS1;
			THEAPP.Struct_PreferenceStruct.m_bUseLAS2 = PreferenceDlg.m_bCheckUseLAS2;
			THEAPP.Struct_PreferenceStruct.m_bUseLAS3 = PreferenceDlg.m_bCheckUseLAS3;

			THEAPP.Struct_PreferenceStruct.m_strSaveFolderPathLAS1 = PreferenceDlg.m_strSaveFolderPathLAS1;
			THEAPP.Struct_PreferenceStruct.m_strSaveFolderPathLAS2 = PreferenceDlg.m_strSaveFolderPathLAS2;
			THEAPP.Struct_PreferenceStruct.m_strSaveFolderPathLAS3 = PreferenceDlg.m_strSaveFolderPathLAS3;

			THEAPP.Struct_PreferenceStruct.m_bUseLASConnectionCheck = PreferenceDlg.m_bCheckUseLASConnectionCheck;

			THEAPP.Struct_PreferenceStruct.m_strRawFolderPath = PreferenceDlg.m_strRawFolderPath;
			THEAPP.Struct_PreferenceStruct.m_strResultFolderPath = PreferenceDlg.m_strResultFolderPath;
			THEAPP.Struct_PreferenceStruct.m_strReviewFolderPath = PreferenceDlg.m_strReviewFolderPath;
			THEAPP.Struct_PreferenceStruct.m_strAdjFolderPath = PreferenceDlg.m_strAdjFolderPath;
			THEAPP.Struct_PreferenceStruct.m_strResultLogFolderPath = PreferenceDlg.m_strResultLogFolderPath;
			THEAPP.Struct_PreferenceStruct.m_strEtcFolderPath = PreferenceDlg.m_strEtcFolderPath;
			THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath = PreferenceDlg.m_strOfflineTestFolderPath;

			THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal = PreferenceDlg.m_bCheckRawImageSaveLocal;
			THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal = PreferenceDlg.m_bCheckResultImageSaveLocal;
			THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal = PreferenceDlg.m_bCheckReviewImageSaveLocal;
			THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal = PreferenceDlg.m_bCheckADJImageSaveLocal;
			THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal = PreferenceDlg.m_bCheckResultLogSaveLocal;

			THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS = PreferenceDlg.m_bCheckRawImageSaveLAS;
			THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS = PreferenceDlg.m_bCheckResultImageSaveLAS;
			THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS = PreferenceDlg.m_bCheckReviewImageSaveLAS;
			THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS = PreferenceDlg.m_bCheckADJImageSaveLAS;
			THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS = PreferenceDlg.m_bCheckResultLogSaveLAS;

			THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread = PreferenceDlg.m_bCheckUseRawImageThread;
			THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread = PreferenceDlg.m_bCheckUseResultImageThread;
			THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread = PreferenceDlg.m_bCheckUseReviewImageThread;
			THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread = PreferenceDlg.m_bCheckUseADJImageThread;
			THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread = PreferenceDlg.m_bCheckUseResultLogThread;

			THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive = PreferenceDlg.m_bCheckUseRawImageTempDrive;

			THEAPP.Struct_PreferenceStruct.m_bSaveRawImage = PreferenceDlg.m_bCheckSaveRawImage;
			THEAPP.Struct_PreferenceStruct.m_bRawImageType = PreferenceDlg.m_bRadioRawImageType;
			THEAPP.Struct_PreferenceStruct.m_bReduceRawImage = PreferenceDlg.m_bCheckReduceRawImage;
			THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio = PreferenceDlg.m_dEditRawImageZoomRatio;
			THEAPP.Struct_PreferenceStruct.m_bCombineRawImage = PreferenceDlg.m_bCheckCombineRawImage;
			THEAPP.Struct_PreferenceStruct.m_bSaveResultGoodImage = PreferenceDlg.m_bCheckSaveResultGoodImage;
			THEAPP.Struct_PreferenceStruct.m_bUseResultImageThreadOnlyGood = PreferenceDlg.m_bCheckUseResultImageThreadOnlyGood;
			THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType = PreferenceDlg.m_bRadioRawImageSaveLogType;
			THEAPP.Struct_PreferenceStruct.m_bAvoidDuplicateLotLocal = PreferenceDlg.m_bCheckAvoidDuplicateLotLocal;
			THEAPP.Struct_PreferenceStruct.m_bAvoidDuplicateLotLas = PreferenceDlg.m_bCheckAvoidDuplicateLotLas;

			THEAPP.Struct_PreferenceStruct.m_iRawImageThreadType = PreferenceDlg.m_iRadioRawImageThreadType;

			THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue = PreferenceDlg.m_bCheckUseMultiQueue;
			THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber = PreferenceDlg.m_iEditThreadQueueNumber;

			THEAPP.Struct_PreferenceStruct.m_iResultImageDefectDisplayType = PreferenceDlg.m_iRadioResultImageDefectDisplayType;
			THEAPP.Struct_PreferenceStruct.m_bUseResultImageDefectSizeLimit = PreferenceDlg.m_bCheckUseResultImageDefectSizeLimit;
			THEAPP.Struct_PreferenceStruct.m_iResultImageDefectSizeMax = PreferenceDlg.m_iEditResultImageDefectSizeMax;

			THEAPP.Struct_PreferenceStruct.m_bSaveADJ = PreferenceDlg.m_bCheckSaveADJ;
			THEAPP.Struct_PreferenceStruct.m_bUseADJConnectionCheck = PreferenceDlg.m_bCheckUseADJConnectionCheck;
			THEAPP.Struct_PreferenceStruct.m_bUseLocalADJ = PreferenceDlg.m_bCheckUseLocalADJ;
			THEAPP.Struct_PreferenceStruct.m_bUseLocalSEG = PreferenceDlg.m_bCheckUseLocalSEG;
			THEAPP.Struct_PreferenceStruct.m_strCurrentADJ_IP = PreferenceDlg.m_strEditAdjCurrentIP;
			THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[0] = PreferenceDlg.m_bCheckUseADJ[0];
			THEAPP.Struct_PreferenceStruct.m_strADJ_IP[0] = PreferenceDlg.m_strADJ_IP[0];
			THEAPP.Struct_PreferenceStruct.m_iADJ_Port[0] = PreferenceDlg.m_nEditAdjPort[0];
			THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[1] = PreferenceDlg.m_bCheckUseADJ[1];
			THEAPP.Struct_PreferenceStruct.m_strADJ_IP[1] = PreferenceDlg.m_strADJ_IP[1];
			THEAPP.Struct_PreferenceStruct.m_iADJ_Port[1] = PreferenceDlg.m_nEditAdjPort[1];
			THEAPP.Struct_PreferenceStruct.m_bIsSendADJ = PreferenceDlg.m_bCheckIsSendADJ;
			THEAPP.Struct_PreferenceStruct.m_bUseADJMultiimage = PreferenceDlg.m_bCheckUseADJMultiimage;
			THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ = PreferenceDlg.m_bCheckIsApplyADJ;
			THEAPP.Struct_PreferenceStruct.m_bUseADJTimeOut = PreferenceDlg.m_bCheckUseADJTimeOut;
			THEAPP.Struct_PreferenceStruct.m_nADJTimeOut = PreferenceDlg.m_nADJTimeOut;
			THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO = PreferenceDlg.m_nEditADJMultiNetworkNO;
			THEAPP.Struct_PreferenceStruct.m_iADJImageSize = PreferenceDlg.m_iEditADJImageSize;
			THEAPP.Struct_PreferenceStruct.m_iADJImageReSize = PreferenceDlg.m_iEditADJImageReSize;
			THEAPP.Struct_PreferenceStruct.m_iMarkingImageRadius = PreferenceDlg.m_iEditAdjMarkingRad;
			THEAPP.Struct_PreferenceStruct.m_iADJCombineDefectDistance = PreferenceDlg.m_iEditADJCombineDefectDistance;
			THEAPP.Struct_PreferenceStruct.m_bSaveROIImage = PreferenceDlg.m_bCheckSaveROIImage;
			THEAPP.Struct_PreferenceStruct.m_iSaveROIImageType = PreferenceDlg.m_iRadioSaveROIImageType;
			THEAPP.Struct_PreferenceStruct.m_bAISetupUseVAL = PreferenceDlg.m_bCheckAISetupUseVAL;
			THEAPP.Struct_PreferenceStruct.m_bAllImageAiInspection = PreferenceDlg.m_bCheckAllImageAi;

			THEAPP.Struct_PreferenceStruct.m_bSaveMDJ = PreferenceDlg.m_bCheckSaveMDJ;
			THEAPP.Struct_PreferenceStruct.m_iMaxReviewImageNumber = PreferenceDlg.m_iEditMaxReviewImageNumber;
			THEAPP.Struct_PreferenceStruct.m_iReviewCombineDefectDistance = PreferenceDlg.m_iEditReviewCombineDefectDistance;
			THEAPP.Struct_PreferenceStruct.m_bUseSaveFaiReviewImage = PreferenceDlg.m_bCheckUseSaveFaiReviewImage;

			THEAPP.Struct_PreferenceStruct.m_iReviewImageDisplayType = PreferenceDlg.m_iRadioReviewImageDisplayType;

			THEAPP.Struct_PreferenceStruct.m_iVision1LightIP1 = PreferenceDlg.m_iEditVision1LightIP1;
			THEAPP.Struct_PreferenceStruct.m_iVision1LightIP2 = PreferenceDlg.m_iEditVision1LightIP2;
			THEAPP.Struct_PreferenceStruct.m_iVision1LightIP3 = PreferenceDlg.m_iEditVision1LightIP3;
			THEAPP.Struct_PreferenceStruct.m_iVision1LightIP4 = PreferenceDlg.m_iEditVision1LightIP4;
			THEAPP.Struct_PreferenceStruct.m_iVision1LightPort = PreferenceDlg.m_iEditVision1LightPort;

			THEAPP.Struct_PreferenceStruct.m_iVision2LightIP1 = PreferenceDlg.m_iEditVision2LightIP1;
			THEAPP.Struct_PreferenceStruct.m_iVision2LightIP2 = PreferenceDlg.m_iEditVision2LightIP2;
			THEAPP.Struct_PreferenceStruct.m_iVision2LightIP3 = PreferenceDlg.m_iEditVision2LightIP3;
			THEAPP.Struct_PreferenceStruct.m_iVision2LightIP4 = PreferenceDlg.m_iEditVision2LightIP4;
			THEAPP.Struct_PreferenceStruct.m_iVision2LightPort = PreferenceDlg.m_iEditVision2LightPort;

			THEAPP.Struct_PreferenceStruct.m_iVision3LightIP1 = PreferenceDlg.m_iEditVision3LightIP1;
			THEAPP.Struct_PreferenceStruct.m_iVision3LightIP2 = PreferenceDlg.m_iEditVision3LightIP2;
			THEAPP.Struct_PreferenceStruct.m_iVision3LightIP3 = PreferenceDlg.m_iEditVision3LightIP3;
			THEAPP.Struct_PreferenceStruct.m_iVision3LightIP4 = PreferenceDlg.m_iEditVision3LightIP4;
			THEAPP.Struct_PreferenceStruct.m_iVision3LightPort = PreferenceDlg.m_iEditVision3LightPort;

			THEAPP.Struct_PreferenceStruct.m_iVision4LightIP1 = PreferenceDlg.m_iEditVision4LightIP1;
			THEAPP.Struct_PreferenceStruct.m_iVision4LightIP2 = PreferenceDlg.m_iEditVision4LightIP2;
			THEAPP.Struct_PreferenceStruct.m_iVision4LightIP3 = PreferenceDlg.m_iEditVision4LightIP3;
			THEAPP.Struct_PreferenceStruct.m_iVision4LightIP4 = PreferenceDlg.m_iEditVision4LightIP4;
			THEAPP.Struct_PreferenceStruct.m_iVision4LightPort = PreferenceDlg.m_iEditVision4LightPort;

			THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo = PreferenceDlg.m_iEditGrabErrRetryNo;
			THEAPP.Struct_PreferenceStruct.m_iGrabDelayTime = PreferenceDlg.m_iEditGrabDelayTime;
			THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime = PreferenceDlg.m_iEditTriggerSleepTime;
			THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime = PreferenceDlg.m_iEditGrabDoneWaitTime;
			THEAPP.Struct_PreferenceStruct.m_iTeachingLiveGrabPeriod = PreferenceDlg.m_iEditTeachingLiveGrabPeriod;
			THEAPP.Struct_PreferenceStruct.m_iLightErrValue = PreferenceDlg.m_iEditLightErrValue;
			THEAPP.Struct_PreferenceStruct.m_iTeachingCameraLightType = PreferenceDlg.m_iRadioTeachingCameraLightType;
			THEAPP.Struct_PreferenceStruct.m_bUseGrabHold = PreferenceDlg.m_bCheckUseGrabHold;
			THEAPP.Struct_PreferenceStruct.m_iFocusMoveNoRetry = PreferenceDlg.m_iEditFocusMoveNoRetry;
			THEAPP.Struct_PreferenceStruct.m_iFocusMoveCompleteWaitTime = PreferenceDlg.m_iEditMoveCompleteWaitTime;
			THEAPP.Struct_PreferenceStruct.m_bUse4dSaveRawImage = PreferenceDlg.m_bCheckUse4dSaveRawImage;
			THEAPP.Struct_PreferenceStruct.m_bUseAutoFocus = PreferenceDlg.m_bCheckUseAutoFocus;
			THEAPP.Struct_PreferenceStruct.m_bUseFastGrab = PreferenceDlg.m_bCheckUseFastGrab;
			THEAPP.Struct_PreferenceStruct.m_bUseBatchInspection = PreferenceDlg.m_bCheckUseBatchInspection;
			THEAPP.Struct_PreferenceStruct.m_bSendGrabFailError = PreferenceDlg.m_bCheckSendGrabFailError;
			THEAPP.Struct_PreferenceStruct.m_iSendGrabFailErrorCount = PreferenceDlg.m_iEditSendGrabFailErrorCount;

			THEAPP.Struct_PreferenceStruct.m_iDefectDispDist = PreferenceDlg.m_iDefectDispDist;
			THEAPP.Struct_PreferenceStruct.m_iMatchingSearchMargin = PreferenceDlg.m_iEditMatchingSearchMargin;
			THEAPP.Struct_PreferenceStruct.m_iBarcodeNoReadImageType = PreferenceDlg.m_iRadioBarcodeNoReadImageType;
			THEAPP.Struct_PreferenceStruct.m_bUseSaveDefectShapeFeature = PreferenceDlg.m_bCheckUseSaveDefectShapeFeature;
			THEAPP.Struct_PreferenceStruct.m_iMaxSameDefectShapeFeature = PreferenceDlg.m_iEditMaxSameDefectShapeFeature;
			THEAPP.Struct_PreferenceStruct.m_bUseSaveGVEdgeMornitoringLog = PreferenceDlg.m_bCheckUseSaveGVEdgeMornitoringLog;
			THEAPP.Struct_PreferenceStruct.m_bUseGpuAffineTrans = PreferenceDlg.m_bCheckUseGpuAffineTrans;
			THEAPP.Struct_PreferenceStruct.m_bUseDeleteCrossBar = PreferenceDlg.m_bCheckUseDeleteCrossBar;
			THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber = PreferenceDlg.m_iEditSaveRecentlyCompleteInfoNumber;

			THEAPP.Struct_PreferenceStruct.m_bUseContDefectAlarm = PreferenceDlg.m_bCheckUseContDefectAlarm;
			THEAPP.Struct_PreferenceStruct.m_iContDefectPosTolerance = PreferenceDlg.m_iEditContDefectPosTolerance;
			THEAPP.Struct_PreferenceStruct.m_iContDefectCount = PreferenceDlg.m_iEditContDefectCount;

			THEAPP.Struct_PreferenceStruct.m_iSam2IP_1 = PreferenceDlg.m_iEditSam2IP_1;
			THEAPP.Struct_PreferenceStruct.m_iSam2IP_2 = PreferenceDlg.m_iEditSam2IP_2;
			THEAPP.Struct_PreferenceStruct.m_iSam2IP_3 = PreferenceDlg.m_iEditSam2IP_3;
			THEAPP.Struct_PreferenceStruct.m_iSam2IP_4 = PreferenceDlg.m_iEditSam2IP_4;
			THEAPP.Struct_PreferenceStruct.m_iSam2Port = PreferenceDlg.m_iEditSam2Port;

			THEAPP.Struct_PreferenceStruct.m_bUseVisionCopy = PreferenceDlg.m_bCheckUseVisionCopy;

			THEAPP.Struct_PreferenceStruct.m_iVmTrain = PreferenceDlg.m_iRadioVmTrain;
			THEAPP.Struct_PreferenceStruct.m_bVmTrainAll = PreferenceDlg.m_bCheckVmTrainAll;

			THEAPP.Struct_PreferenceStruct.m_bUseVmThresOpt = PreferenceDlg.m_bCheckUseVmThresOpt;
			THEAPP.Struct_PreferenceStruct.m_iVmSplitThres = PreferenceDlg.m_iEditVmSplitThres;
			THEAPP.Struct_PreferenceStruct.m_dVmKSigma = PreferenceDlg.m_dEditVmKSigma;

			/*if (THEAPP.Struct_PreferenceStruct.m_bUseSubHDD)
			{
				for( int i = 0; i < MAX_MAGAZINE_NO; i++ )
					THEAPP.ChangeHDD(i);
			}*/

			if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS || THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS || THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS ||
				THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS || THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.ConnectStatusLAS(0);

			int iVisionCamType;
			CString sVisionCamType;

			iVisionCamType = VISION_NUMBER_1;
			sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
			if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_1])
				GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N1)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N1)->EnableWindow(FALSE);

			iVisionCamType = VISION_NUMBER_2;
			sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
			if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_2])
				GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N2)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N2)->EnableWindow(FALSE);

			iVisionCamType = VISION_NUMBER_3;
			sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
			if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_3])
				GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N3)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N3)->EnableWindow(FALSE);

			iVisionCamType = VISION_NUMBER_4;
			sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
			if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_4])
				GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N4)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N4)->EnableWindow(FALSE);

			THEAPP.UpdateCurMode();

			strLog.Format("☆☆☆☆☆ Saved Preference Setting ☆☆☆☆☆");
			THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
}

void CuScanView::OnBnClickedMfcbuttonFormExit()
{
	int nRes = 0;
#ifdef INLINE_MODE
	strLog.Format("☆☆☆☆☆ EXIT Button click ☆☆☆☆☆");
	THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));

	BOOL bCanExit = TRUE;
	int iSaveOngoingMzNo = 0;

	for (int i = 0; i < MAX_MAGAZINE_NO; i++)
	{
		if (THEAPP.m_pInspectService->m_bSaveResultLogThreadDone[i] == FALSE || THEAPP.m_pInspectService->m_bContDefectLogThreadDone[i] == FALSE)
		{
			bCanExit = FALSE;
			iSaveOngoingMzNo = i + 1;

			break;
		}
	}

	if (bCanExit == FALSE)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("현재 %d번째 Lot 검사 결과 로그를 저장하는 중입니다. 잠시 후에 다시 종료 시도해 주세요.", iSaveOngoingMzNo);
		else
			strMessageBox.Format("Currently saving the %d-th Lot inspection result log. Please try to exit again later.", iSaveOngoingMzNo);
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

		strLog.Format("Exit program cancel(Saving lot inspection result log)");
		THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));

		return;
	}
#ifndef POC_TEST
	if (THEAPP.m_pInspectService->GetRunStartStatus() == TRUE)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("설비가 오토런이 가동중입니다. 프로그램을 종료하시겠습니까?", iSaveOngoingMzNo);
		else
			strMessageBox.Format("The equipment is running Auto Run. Do you want to exit the program?");
		nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

		if ((nRes == IDYES))
		{
			strLog.Format("Exit program(Running inspection)");
			THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
			SetExitProgram();
		}
		else
		{
			strLog.Format("Exit program cancel(Running inspection)");
			THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));

			return;
		}
	}
	else
	{
#endif
#endif
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("프로그램을 종료하시겠습니까?");
		else
			strMessageBox.Format("Do you want to exit the program?");
		nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

		if ((nRes == IDYES))
		{
			strLog.Format("Exit program");
			THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
			SetExitProgram();
		}
		else
		{
			strLog.Format("Exit program cancel");
			THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));

			return;
		}
#ifdef INLINE_MODE
#ifndef POC_TEST
	}
#endif
#endif
}

void CuScanView::OnBnClickedButtonModeChange()
{
	int iVisionCamType;
	CString sVisionCamType;

	if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_OPERATOR)
	{
		CAdminPasswordDlg dlg;
		if (dlg.DoModal() != IDOK)
			return;
	}

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_RegisterInfo(strDataFolder + "RegisterInfo.ini");

	switch (THEAPP.m_iModeSwitch)
	{
	case MODE_INSPECT_VIEW_ADMIN: /// Op Mode 화면 전환
#ifdef INLINE_MODE
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(FALSE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_SAVE_PARAM)->EnableWindow(FALSE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_UPDATE_PARAM)->EnableWindow(FALSE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_MACRO_PATH)->EnableWindow(FALSE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_CHECK_RUN_MACRO)->EnableWindow(FALSE);
#else
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_SAVE_PARAM)->EnableWindow(TRUE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_UPDATE_PARAM)->EnableWindow(TRUE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_MACRO_PATH)->EnableWindow(TRUE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_CHECK_RUN_MACRO)->EnableWindow(TRUE);
#endif
		GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_SAVE)->EnableWindow(FALSE);

		GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N1)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N2)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N3)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N4)->EnableWindow(FALSE);

		GetDlgItem(IDC_MFCBUTTON_FORM_PREV_TRAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_PREFERENCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_REGISTER)->EnableWindow(FALSE);

		INI_RegisterInfo.Set_String("LOGIN MODE", "Mode", "작업자");
		INI_RegisterInfo.Set_String("OPERATOR", "Operator", "AVI");

		THEAPP.m_iModeSwitch = MODE_INSPECT_VIEW_OPERATOR;
		break;
	case 2: /// Admin Mode 화면 전환
#ifdef INLINE_MODE
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_SAVE_PARAM)->EnableWindow(FALSE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_UPDATE_PARAM)->EnableWindow(FALSE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_MACRO_PATH)->EnableWindow(FALSE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_CHECK_RUN_MACRO)->EnableWindow(FALSE);
#else
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_SAVE_PARAM)->EnableWindow(TRUE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_UPDATE_PARAM)->EnableWindow(TRUE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_MACRO_PATH)->EnableWindow(TRUE);
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_CHECK_RUN_MACRO)->EnableWindow(TRUE);
#endif
		GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_SAVE)->EnableWindow(TRUE);

		iVisionCamType = VISION_NUMBER_1;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_1])
			GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N1)->EnableWindow(TRUE);

		iVisionCamType = VISION_NUMBER_2;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_2])
			GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N2)->EnableWindow(TRUE);

		iVisionCamType = VISION_NUMBER_3;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_3])
			GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N3)->EnableWindow(TRUE);

		iVisionCamType = VISION_NUMBER_4;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		if (sVisionCamType != "Undefine" && THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_4])
			GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N4)->EnableWindow(TRUE);

		GetDlgItem(IDC_MFCBUTTON_FORM_PREV_TRAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_PREFERENCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_REGISTER)->EnableWindow(TRUE);

		INI_RegisterInfo.Set_String("LOGIN MODE", "Mode", "관리자");
		INI_RegisterInfo.Set_String("OPERATOR", "Operator", "Admin");

		THEAPP.m_iModeSwitch = MODE_INSPECT_VIEW_ADMIN;
		break;
	}

	THEAPP.m_pTrayAdminViewDlg1->ShowWindow(SW_HIDE);
	THEAPP.m_pTrayAdminViewDlg2->ShowWindow(SW_HIDE);
	THEAPP.m_pTrayAdminViewDlg3->ShowWindow(SW_HIDE);
	THEAPP.m_pTrayAdminViewDlg4->ShowWindow(SW_HIDE);
	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_1];
	if (sVisionCamType != "Undefine")
		THEAPP.m_pTrayAdminViewDlg1->ShowWindow(SW_SHOW);
	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_2];
	if (sVisionCamType != "Undefine")
		THEAPP.m_pTrayAdminViewDlg2->ShowWindow(SW_SHOW);
	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_3];
	if (sVisionCamType != "Undefine")
		THEAPP.m_pTrayAdminViewDlg3->ShowWindow(SW_SHOW);
	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_4];
	if (sVisionCamType != "Undefine")
		THEAPP.m_pTrayAdminViewDlg4->ShowWindow(SW_SHOW);

	THEAPP.m_pDefectListDlg->ShowWindow(SW_SHOW);

	THEAPP.m_pInspectViewOverayImageToolbarDlg1->ShowWindow(SW_HIDE);
	THEAPP.m_pInspectViewOverayImageToolbarDlg2->ShowWindow(SW_HIDE);
	THEAPP.m_pInspectViewOverayImageToolbarDlg3->ShowWindow(SW_HIDE);
	THEAPP.m_pInspectViewOverayImageToolbarDlg4->ShowWindow(SW_HIDE);
	THEAPP.m_pInspectViewOverayImageDlg1->ShowWindow(SW_HIDE);
	THEAPP.m_pInspectViewOverayImageDlg2->ShowWindow(SW_HIDE);
	THEAPP.m_pInspectViewOverayImageDlg3->ShowWindow(SW_HIDE);
	THEAPP.m_pInspectViewOverayImageDlg4->ShowWindow(SW_HIDE);
	THEAPP.m_bShowReviewWindow = FALSE;
}


void CuScanView::OnBnClickedMfcbuttonFormRegister()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	COperator_RegisterDlg dlg;

	if (dlg.DoModal() != IDOK)
		return;
}


void CuScanView::OnBnClickedMfcbuttonFormPrevTray()
{
#ifdef INLINE_MODE
	strLog.Format("☆☆☆☆☆ REVIEW Button click ☆☆☆☆☆");
	THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
#endif

	if (THEAPP.m_bShowReviewWindow == FALSE)
	{
		THEAPP.m_pInspectViewOverayImageToolbarDlg1->Show();
		THEAPP.m_pInspectViewOverayImageToolbarDlg2->Show();
		THEAPP.m_pInspectViewOverayImageToolbarDlg3->Show();
		THEAPP.m_pInspectViewOverayImageToolbarDlg4->Show();
		THEAPP.m_pInspectViewOverayImageDlg1->Show();
		THEAPP.m_pInspectViewOverayImageDlg2->Show();
		THEAPP.m_pInspectViewOverayImageDlg3->Show();
		THEAPP.m_pInspectViewOverayImageDlg4->Show();
		THEAPP.m_bShowReviewWindow = TRUE;
	}
	else
	{
		THEAPP.m_pInspectViewOverayImageToolbarDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageToolbarDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg1->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg2->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg3->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectViewOverayImageDlg4->ShowWindow(SW_HIDE);
		THEAPP.m_bShowReviewWindow = FALSE;
	}
}

void CuScanView::OnBnClickedMfcbuttonFormAisetup()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strLog; // 로그 변수는 함수 시작 시 선언

	// --- 중복 실행 방지: 이미 AI Setup Wizard가 실행 중인지 확인 ---
	if (ai_setup_process_handle_ != nullptr)
	{
		// 프로세스가 여전히 실행 중인지 확인
		DWORD dwExitCode = 0;
		if (GetExitCodeProcess(ai_setup_process_handle_, &dwExitCode))
		{
			if (dwExitCode == STILL_ACTIVE)
			{
				// 아직 실행 중
				CString strMessage;
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessage.Format(_T("AI Setup Wizard가 이미 실행 중입니다.\n먼저 실행 중인 프로그램을 종료해 주세요."));
				else
					strMessage.Format(_T("AI Setup Wizard is already running.\nPlease close the running program first."));
				AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONWARNING);

				strLog.Format(_T("AI Setup Wizard launch blocked - already running"));
				THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
				return; // 중복 실행 방지
			}
		}

		// 프로세스가 종료되었으면 핸들 정리
		CloseHandle(ai_setup_process_handle_);
		ai_setup_process_handle_ = nullptr;
	}

	std::optional<SHELLEXECUTEINFOA> sei = THEAPP.StartAISetupSW("Basic", -1, "");

	if (sei == std::nullopt)
	{
		// 실행 실패
		CString strMessage;
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessage.Format(_T("AI Setup Wizard 실행에 실패했습니다.\n프로그램 파일이 존재하는지 확인해 주세요."));
		else
			strMessage.Format(_T("Failed to launch AI Setup Wizard.\nPlease check if the program file exists."));
		AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONERROR);
		strLog.Format(_T("AI Setup Wizard launch FAILED"));
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		return;
	}

	// 프로세스 핸들 저장
	ai_setup_process_handle_ = sei->hProcess;

	strLog.Format(_T("AI Setup Wizard launched successfully"));
	THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));

	//// 프로세스 종료를 기다리는 쓰레드 시작
	AfxBeginThread(AiSetupWaitThreadProc, this);
}

UINT __cdecl CuScanView::AiSetupWaitThreadProc(LPVOID pParam)
{
	CuScanView* pView = reinterpret_cast<CuScanView*>(pParam);
	if (!pView)
		return 0;

	HANDLE hProcess = pView->ai_setup_process_handle_;
	if (hProcess == nullptr)
		return 0;

	// 프로세스 종료까지 대기
	WaitForSingleObject(hProcess, INFINITE);

	// View에 "끝났다"는 메시지 전달 (UI 스레드에서 처리)
	// 핸들 정리는 OnAiSetupExited에서 수행
	::PostMessage(pView->GetSafeHwnd(), WM_AISETUP_EXITED, 0, 0);

	return 0;
}

LRESULT CuScanView::OnAiSetupExited(WPARAM wParam, LPARAM lParam)
{
	CString strLog;

	// 프로세스 핸들 정리
	if (ai_setup_process_handle_ != nullptr)
	{
		CloseHandle(ai_setup_process_handle_);
		ai_setup_process_handle_ = nullptr;
	}

	// 1) 일단 종료되었다는 안내 메시지 (필요하면 생략 가능)
	//CString strFinished;
	//if (THEAPP.m_iPGMLanguageSelect == 0)
	//	strFinished = _T("AI Setup Wizard 설정 프로그램이 종료되었습니다.");
	//else
	//	strFinished = _T("AI Setup Wizard has been closed.");

	//AfxMessageBox(strFinished, MB_SYSTEMMODAL | MB_ICONINFORMATION);

	// 2) 재초기화 여부 질문
	CString strQuestion;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strQuestion = _T("설정 UI에서 설정을 완료하셨습니까?\n\n"
						 "완료하셨다면, 새로 설정된 값으로 AI 검사 시스템을 재 초기화 하시겠습니까?");
	else
		strQuestion = _T("Did you finish updating settings in the AI Setup Wizard?\n\n"
						 "If yes, do you want to reinitialize the AI inspection system with the new settings?");

	int nRet = AfxMessageBox(strQuestion,
							 MB_SYSTEMMODAL | MB_ICONQUESTION | MB_YESNO);

	if (nRet == IDYES)
	{
		// 3) 사용자가 재초기화 선택
		if (ReinitializeAiInspection())
		{
			if (THEAPP.m_iPGMLanguageSelect == 0)
				AfxMessageBox(_T("AI 검사 시스템이 새 설정으로 재 초기화되었습니다."),
							  MB_SYSTEMMODAL | MB_ICONINFORMATION);
			else
				AfxMessageBox(_T("AI inspection system has been reinitialized with the new settings."),
							  MB_SYSTEMMODAL | MB_ICONINFORMATION);

			strLog = _T("AI Setup Wizard process exited, AI inspection reinitialized with new settings.");
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		}
		else
		{
			if (THEAPP.m_iPGMLanguageSelect == 0)
				AfxMessageBox(_T("AI 검사 시스템 재 초기화에 실패했습니다.\n로그를 확인해 주세요."),
							  MB_SYSTEMMODAL | MB_ICONERROR);
			else
				AfxMessageBox(_T("Failed to reinitialize AI inspection system.\nPlease check the log."),
							  MB_SYSTEMMODAL | MB_ICONERROR);

			strLog = _T("AI Setup Wizard process exited, but AI inspection reinitialization FAILED.");
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		}
	}
	else
	{
		// 4) 사용자가 재초기화 하지 않겠다고 선택
		strLog = _T("AI Setup Wizard process exited, user chose NOT to reinitialize AI inspection.");
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
	}

	return 0;
}

bool CuScanView::ReinitializeAiInspection()
{
	CString strLog;
	strLog = _T("Reinitializing AI inspection with updated configuration...");
	THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

	// --- 경로 준비 (std::filesystem 사용) ---
	char path_buf[MAX_PATH];
	GetModuleFileNameA(NULL, path_buf, MAX_PATH);
	std::filesystem::path executable_path(path_buf);
	std::filesystem::path folder_path = executable_path.parent_path() / "Data";
	std::filesystem::path config_json_path = folder_path / "ai_inspection_config.json";
	// 구성 파일이 존재하는지 확인
	if (!PathFileExistsA(config_json_path.string().c_str()))
	{
		strLog.Format(_T("AI inspection configuration file not found: %s"), config_json_path.string().c_str());
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		return false;
	}
#ifdef SYAI
	// 구성 파일이 존재하면, 재초기화 로직 수행
	return THEAPP.m_syai_service->re_init_config(config_json_path.string());
#else
	strLog.Format(_T("SYAI 모듈이 포함되지 않은 빌드에서는 AI 검사 재초기화를 지원하지 않습니다."));
	THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
	return false;
#endif // SYAI
}

void CuScanView::ChangeHeaderImage(UINT nBitmapID)
{
	// 이전 비트맵이 있으면 메모리 해제 (누수 방지)
	if (m_hHeaderBitmap != NULL)
	{
		::DeleteObject(m_hHeaderBitmap);
		m_hHeaderBitmap = NULL;
	}

	// 리소스에서 비트맵 로드
	m_hHeaderBitmap = (HBITMAP)::LoadImage(
		AfxGetInstanceHandle(),
		MAKEINTRESOURCE(nBitmapID),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE | LR_CREATEDIBSECTION
	);

	if (m_hHeaderBitmap != NULL)
	{
		// 기존 SetBitmap이 반환하는 핸들은 처음에는 다이얼로그 리소스이므로
		// 우리가 직접 관리하는 핸들만 해제하면 됩니다.
		m_pStaticMainTitle.SetBitmap(m_hHeaderBitmap);
		m_pStaticMainTitle.Invalidate();
	}
}