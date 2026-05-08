// InspectSummary.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectSummary.h"
#include "afxdialogex.h"
#include "IniFileCS.h"

typedef struct _MODEL_LOAD_THREAD_PARAM {
	_MODEL_LOAD_THREAD_PARAM(CInspectSummary *pDlg_I, int iDualModelData_I)
	{
		this->pDlg = pDlg_I;
		this->iDualModelData = iDualModelData_I;
	}

	CInspectSummary *pDlg;
	int iDualModelData;
} MODEL_LOAD_THREAD_PARAM;

UINT ModelLoadThread(LPVOID lp)
{
	CString strLog;

	CString sVisionCamType;

	MODEL_LOAD_THREAD_PARAM* pModelLoadThreadParam = (MODEL_LOAD_THREAD_PARAM*)lp;

	int iDualModelData;
	CInspectSummary* pDlg = (CInspectSummary*)pModelLoadThreadParam->pDlg;
	iDualModelData = pModelLoadThreadParam->iDualModelData;

	SAFE_DELETE(pModelLoadThreadParam);

	BOOL bModelExist1, bModelExist2;
	for (int iPcVisionNo = VISION_NUMBER_MAX - 1; iPcVisionNo >= VISION_NUMBER_1; iPcVisionNo--)
	{
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
		if (sVisionCamType == "Undefine")
			continue;

		bModelExist1 = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->LoadModelSWBaseData();
		bModelExist2 = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->LoadModelSWData();

		THEAPP.bVariationModelSaveCheck[iPcVisionNo] = FALSE;
		THEAPP.bTemplateModelSaveCheck[iPcVisionNo] = FALSE;
	}

	if (!bModelExist1 || !bModelExist2)
	{
		strLog.Format("Vision recipe load fail, Recipe name: %s", THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName);
		THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
	}
	else
	{
		strLog.Format("Vision recipe load complete, Recipe name: %s", THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName);
		THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
	}

	if (!THEAPP.m_pInspectService->GetRunStartStatus())
	{
		for (int iPcVisionNo = VISION_NUMBER_MAX - 1; iPcVisionNo >= VISION_NUMBER_1; iPcVisionNo--)
		{
			sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
			if (sVisionCamType == "Undefine")
				continue;

			bModelExist1 = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->LoadModelHWBaseData();
			bModelExist2 = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->LoadModelHWData();
		}

		if (!bModelExist1 || !bModelExist2)
		{
			strLog.Format("Optical recipe load fail, Recipe name: %s_%s_PC%d", THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
			THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
		}
		else
		{
			strLog.Format("Optical recipe load complete, Recipe name: %s_%s_PC%d", THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
			THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
		}
	}
	else
	{
		strLog.Format("Optical recipe load skip(Run stage is true), Recipe name: %s_%s_PC%d", THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
		THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
	}

#ifdef DEFECT_CONDITION
	BOOL bLoadConditionCheck;
	if (!THEAPP.m_pInspectService->GetRunStartStatus())
	{
		bLoadConditionCheck = THEAPP.CountPerAreaCondition();
		if (bLoadConditionCheck)
		{
			strLog.Format("Count per area condition(json) load complete");
			THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
		}
		else
		{
			THEAPP.LoadCountPerAreaCondition_ini();
			strLog.Format("Count per area condition(ini) load complete");
			THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
		}
	}
#endif

	CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS INI(strStatusFileName);
	CString strSection = "Status";
	INI.Set_String(strSection, "ModelID", THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);

	pDlg->m_evModelLoadDone.SetEvent();

	return 1;
}

// CInspectSummary 대화 상자입니다.
CInspectSummary* CInspectSummary::m_pInstance = NULL;

CInspectSummary* CInspectSummary::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectSummary();
		if (!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_SUMMARY_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}
void CInspectSummary::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectSummary::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CInspectSummary, CDialog)

CInspectSummary::CInspectSummary(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectSummary::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT + 540, VIEW1_DLG3_TOP + 50, VIEW1_DLG3_WIDTH - 550, VIEW1_DLG3_HEIGHT + 100);

	m_bDxRunMacro = FALSE;
	m_bDxAutoSettingZFocus = FALSE;
	m_bDxAutoSettingLight = FALSE;
	m_bUseAutoSettingZFocus = FALSE;
	m_bUseAutoSettingLight = FALSE;
}

CInspectSummary::~CInspectSummary()
{
}

void CInspectSummary::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_DISK_HDDC, m_ctrlProgressHddC);
	DDX_Control(pDX, IDC_PROGRESS_DISK_HDDD, m_ctrlProgressHddD);
	DDX_Check(pDX, IDC_CHECK_RUN_MACRO, m_bDxRunMacro);
	DDX_Check(pDX, IDC_CHK_AUTOSETTING_ZFOCUS, m_bDxAutoSettingZFocus);
	DDX_Check(pDX, IDC_CHK_AUTOSETTING_LIGHT, m_bDxAutoSettingLight);
}


BEGIN_MESSAGE_MAP(CInspectSummary, CDialog)
	//	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_MFCBUTTON_INSPECT_START, &CInspectSummary::OnBnClickedMfcbuttonInspectStart)
	ON_BN_CLICKED(IDC_CHECK_RUN_MACRO, &CInspectSummary::OnBnClickedCheckRunMacro)
	ON_BN_CLICKED(IDC_MFCBUTTON_MACRO_PATH, &CInspectSummary::OnBnClickedMfcbuttonMacroPath)
	ON_BN_CLICKED(IDC_MFCBUTTON_SAVE_PARAM, &CInspectSummary::OnBnClickedMfcbuttonSaveParam)
	ON_BN_CLICKED(IDC_MFCBUTTON_UPDATE_PARAM, &CInspectSummary::OnBnClickedMfcbuttonUpdateParam)
	ON_BN_CLICKED(IDC_CHK_AUTOSETTING_ZFOCUS, &CInspectSummary::OnBnClickedChkAutosettingZfocus)
	ON_BN_CLICKED(IDC_CHK_AUTOSETTING_LIGHT, &CInspectSummary::OnBnClickedChkAutosettingLight)
	ON_WM_TIMER()
END_MESSAGE_MAP()

#include "AdminPasswordDlg.h"

BOOL CInspectSummary::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

#ifdef INLINE_MODE
	GetDlgItem(IDC_MFCBUTTON_SAVE_PARAM)->EnableWindow(FALSE);
#endif
	GetDlgItem(IDC_MFCBUTTON_MACRO_PATH)->ShowWindow(SW_HIDE);

	ChangeLanguage();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CInspectSummary::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (THEAPP.m_pInspectSummary->m_bDxRunMacro)
	{
		KillTimer(0);
		if (THEAPP.g_bMacroStart)
		{
			THEAPP.g_bMacroStart = FALSE;
			OnBnClickedMfcbuttonInspectStart();
		}
		SetTimer(0, 1000, NULL);
		CDialog::OnTimer(nIDEvent);
	}
}

void CInspectSummary::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
#ifdef INLINE_MODE
		SetDlgItemText(IDC_MFCBUTTON_INSPECT_START, _T("검사 다시 시작"));
#else
		SetDlgItemText(IDC_MFCBUTTON_INSPECT_START, _T("오프라인 검사 시작"));
#endif
		SetDlgItemText(IDC_MFCBUTTON_SAVE_PARAM, _T("검사 파라미터 저장"));
		SetDlgItemText(IDC_MFCBUTTON_UPDATE_PARAM, _T("검사 파라미터 업데이트"));
		SetDlgItemText(IDC_GROUPBOX_INSPECTSUMMARYDLG_1, _T("디스크 사용량"));
		SetDlgItemText(IDC_STATIC_INSPECTSUMMARYDLG_1, _T("C:"));
		SetDlgItemText(IDC_STATIC_INSPECTSUMMARYDLG_2, _T("%"));
		SetDlgItemText(IDC_STATIC_INSPECTSUMMARYDLG_3, _T("D:"));
		SetDlgItemText(IDC_STATIC_INSPECTSUMMARYDLG_4, _T("%"));
		SetDlgItemText(IDC_CHECK_RUN_MACRO, _T("오프라인 매크로 모드"));
		SetDlgItemText(IDC_MFCBUTTON_MACRO_PATH, _T("경로"));
		SetDlgItemText(IDC_CHK_AUTOSETTING_ZFOCUS, _T("Z축 자동 셋팅 모드"));
		SetDlgItemText(IDC_CHK_AUTOSETTING_LIGHT, _T("조명 자동 셋팅 모드"));
	}
	else
	{
#ifdef INLINE_MODE
		SetDlgItemText(IDC_MFCBUTTON_INSPECT_START, _T("Re-start inspect"));
#else
		SetDlgItemText(IDC_MFCBUTTON_INSPECT_START, _T("Start offline inspect"));
#endif
		SetDlgItemText(IDC_MFCBUTTON_SAVE_PARAM, _T("Save inspection parameter"));
		SetDlgItemText(IDC_MFCBUTTON_UPDATE_PARAM, _T("Update inspection parameter"));
		SetDlgItemText(IDC_GROUPBOX_INSPECTSUMMARYDLG_1, _T("Disk usage"));
		SetDlgItemText(IDC_STATIC_INSPECTSUMMARYDLG_1, _T("C:"));
		SetDlgItemText(IDC_STATIC_INSPECTSUMMARYDLG_2, _T("%"));
		SetDlgItemText(IDC_STATIC_INSPECTSUMMARYDLG_3, _T("D:"));
		SetDlgItemText(IDC_STATIC_INSPECTSUMMARYDLG_4, _T("%"));
		SetDlgItemText(IDC_CHECK_RUN_MACRO, _T("Macro mode for offline"));
		SetDlgItemText(IDC_MFCBUTTON_MACRO_PATH, _T("Path"));
		SetDlgItemText(IDC_CHK_AUTOSETTING_ZFOCUS, _T("Auto Z Focus Setting Mode"));
		SetDlgItemText(IDC_CHK_AUTOSETTING_LIGHT, _T("Auto Light Setting Mode"));
	}
}

void CInspectSummary::OnBnClickedMfcbuttonInspectStart()
{
	UpdateData(TRUE);

#ifdef INLINE_MODE
	CString strMsg;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMsg.Format("재시작을 요청하시겠습니까?");
	else
		strMsg.Format("Do you want to request a restart?");
	if (AfxMessageBox(strMsg, MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK)
		THEAPP.m_pHandlerService->Set_ReloadRequest(THEAPP.Struct_PreferenceStruct.m_iPCType);

	return;
#endif

	if (THEAPP.m_pInspectAdminViewDlg)
	{
		THEAPP.m_pInspectAdminViewDlg->ClearAlignTabTempRegion();
		GenEmptyObj(&THEAPP.m_pInspectAdminViewDlg->m_pThresRgn);
		THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();
		THEAPP.m_pInspectAdminViewDlg->SaveInspectTabOverLastSelectRegion();
		THEAPP.m_pInspectAdminViewDlg->ClearActiveTRegion();
		THEAPP.m_pInspectAdminViewDlg->ClearAllSelectRegion();
		THEAPP.m_pInspectAdminViewDlg->ClearAllLastSelectRegion();
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion = NULL;
		THEAPP.m_pInspectAdminViewDlg->ClearUpdateView(FALSE);
	}

	THEAPP.m_nOfflineBarcodeImageNo = 1;
	UpdateData(FALSE);

	for (int i = 0; i < MAX_MAGAZINE_NO; i++) //jwj add 20201026
		THEAPP.m_LotstartInfo[i].GetTime();

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_LastStatus(strDataFolder + "LastStatus.txt");
	CString strSection, strKey;

	strSection = "Status";
	strKey = "LotID";
	CString strLastLotID = INI_LastStatus.Get_String(strSection, strKey, "");

	// Dlg 상의 Lot ID를 가져옴
	CString strCurrentLotID;
	THEAPP.m_pInspectResultDlg->GetDlgItemText(IDC_EDIT_LOT_ID, strCurrentLotID);

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
		THEAPP.m_pInspectService->m_sLotID_H[i] = strCurrentLotID;

	if (THEAPP.m_pInspectSummary->m_bDxRunMacro)
	{
		SetTimer(0, 1000, NULL);

		CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
		CIniFileCS INI_RunMacro(strDataFolder + "RunMacro.ini");
		CString strSection2, strKey2, strMacroPath, strMacroLotID;

		strSection2 = "FilePath";
		strKey2.Format("Path_%d", THEAPP.g_nMacroCount);
		strMacroPath = INI_RunMacro.Get_String(strSection2, strKey2, "");
		THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath = strMacroPath;

		strSection2 = "LotID";
		strKey2.Format("ID_%d", THEAPP.g_nMacroCount);
		strMacroLotID = INI_RunMacro.Get_String(strSection2, strKey2, "");
		strCurrentLotID = strMacroLotID;

		for (int i = 0; i < VISION_NUMBER_MAX; i++)
			THEAPP.m_pInspectService->m_sLotID_H[i] = strMacroLotID;

		if (!strMacroLotID.Compare("") || !strMacroPath.Compare(""))
		{
			AfxMessageBox("Macro End");
			THEAPP.g_bMacroStart = FALSE;
			KillTimer(0);
			return;
		}
		THEAPP.g_nMacroCount++;
	}

	THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_LOT_ID, THEAPP.m_pInspectService->m_sLotID_H[0]);

	THEAPP.m_iOfflineCurTrayNo += 1;

	THEAPP.m_pInspectService->ReadyLot(FALSE, 1, TRUE, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], strCurrentLotID, FALSE);

	THEAPP.m_pInspectService->OfflineInspection_Folder(strCurrentLotID);
}

void CInspectSummary::DoEvents(DWORD dwSleep)
{
	MSG msg;
	DWORD dwStart = GetTickCount();

	do {

		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		Sleep(1);

	} while (dwSleep > (GetTickCount() - dwStart));
}

void CInspectSummary::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	lpwndpos->flags |= SWP_NOMOVE;
	CDialog::OnWindowPosChanging(lpwndpos);
}

//////////////////////////////////////////////////////////////////////////
///  Tray 변경 시 진입 함수 (ModuleNo 1: T1, S1, S2, T2, S3, S4, B1 ////  ModuleNo 2: B2)
// Lot 폴더 내의 Tray-xx 폴더 및 하부 폴더를 생성하는 함수 (비전 1번에서는 화면 갱신 처리 추가)
void CInspectSummary::MakeTrayResultSaveFolder(int iPcVisionNo, CString sLotID, int iMzNo, int iTrayNo)
{
	CString sTrayNo;
	sTrayNo.Format("%d", iTrayNo);

	if (iPcVisionNo == VISION_NUMBER_1 || iPcVisionNo == VISION_NUMBER_3)
	{
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_LOT_ID, sLotID);
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_COUNTTRAY, sTrayNo);
	}

	CString RawImageFolderName_Local;
	CString ResultImageFolderName_Local;
	CString ResultLogFolderName_Local;
	CString ImageFolderName_LAS;
	CString ResultLogFolderName_LAS;
	CString RawImageFolderName_Temp1;
	CString RawImageFolderName_Temp2;
	CString RawImageFolderName_Temp3;

	CString FolderSub;

	// CMI_Results
	RawImageFolderName_Local = THEAPP.Struct_PreferenceStruct.m_strRawFolderPath + "CMI_Results";
	ResultImageFolderName_Local = THEAPP.Struct_PreferenceStruct.m_strResultFolderPath + "CMI_Results";
	ResultLogFolderName_Local = THEAPP.Struct_PreferenceStruct.m_strResultLogFolderPath + "CMI_Results";
	ImageFolderName_LAS = THEAPP.m_strChangeLASFolderPath[iMzNo - 1] + "CMI_Results";
	ResultLogFolderName_LAS = THEAPP.m_strChangeLASFolderPath[iMzNo - 1] + "CMI_Results";
	RawImageFolderName_Temp1 = THEAPP.Struct_PreferenceStruct.m_strSaveFolderPathLAS1 + "CMI_Results";
	RawImageFolderName_Temp2 = THEAPP.Struct_PreferenceStruct.m_strSaveFolderPathLAS2 + "CMI_Results";
	RawImageFolderName_Temp3 = THEAPP.Struct_PreferenceStruct.m_strSaveFolderPathLAS3 + "CMI_Results";

	if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(RawImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread && THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultLogFolderName_Local);

	if ((!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS) ||
		(!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS))
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread && THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ResultLogFolderName_LAS);

	// CMI_Results\\yyyy
	FolderSub.Format("\\%d", THEAPP.m_LotstartInfo[iMzNo - 1].m_iCurYear);
	RawImageFolderName_Local = RawImageFolderName_Local + FolderSub;
	ResultImageFolderName_Local = ResultImageFolderName_Local + FolderSub;
	ResultLogFolderName_Local = ResultLogFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;
	ResultLogFolderName_LAS = ResultLogFolderName_LAS + FolderSub;
	RawImageFolderName_Temp1 = RawImageFolderName_Temp1 + FolderSub;
	RawImageFolderName_Temp2 = RawImageFolderName_Temp2 + FolderSub;
	RawImageFolderName_Temp3 = RawImageFolderName_Temp3 + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(RawImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread && THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultLogFolderName_Local);

	if ((!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS) ||
		(!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS))
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread && THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ResultLogFolderName_LAS);

	// CMI_Results\\yyyy\\mm
	FolderSub.Format("\\%d", THEAPP.m_LotstartInfo[iMzNo - 1].m_iCurMonth);
	RawImageFolderName_Local = RawImageFolderName_Local + FolderSub;
	ResultImageFolderName_Local = ResultImageFolderName_Local + FolderSub;
	ResultLogFolderName_Local = ResultLogFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;
	ResultLogFolderName_LAS = ResultLogFolderName_LAS + FolderSub;
	RawImageFolderName_Temp1 = RawImageFolderName_Temp1 + FolderSub;
	RawImageFolderName_Temp2 = RawImageFolderName_Temp2 + FolderSub;
	RawImageFolderName_Temp3 = RawImageFolderName_Temp3 + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(RawImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread && THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultLogFolderName_Local);

	if ((!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS) ||
		(!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS))
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread && THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ResultLogFolderName_LAS);

	// CMI_Results\\yyyy\\mm\\dd
	FolderSub.Format("\\%d", THEAPP.m_LotstartInfo[iMzNo - 1].m_iCurDay);
	RawImageFolderName_Local = RawImageFolderName_Local + FolderSub;
	ResultImageFolderName_Local = ResultImageFolderName_Local + FolderSub;
	ResultLogFolderName_Local = ResultLogFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;
	ResultLogFolderName_LAS = ResultLogFolderName_LAS + FolderSub;
	RawImageFolderName_Temp1 = RawImageFolderName_Temp1 + FolderSub;
	RawImageFolderName_Temp2 = RawImageFolderName_Temp2 + FolderSub;
	RawImageFolderName_Temp3 = RawImageFolderName_Temp3 + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(RawImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread && THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultLogFolderName_Local);

	if ((!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS) ||
		(!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS))
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread && THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ResultLogFolderName_LAS);

	if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
		THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1].Format(ResultLogFolderName_Local);

	if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
		THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1].Format(ResultLogFolderName_LAS);

	// CMI_Results\\yyyy\\mm\\dd\\LotID
	FolderSub.Format("\\%s", sLotID);
	RawImageFolderName_Local = RawImageFolderName_Local + FolderSub;
	ResultImageFolderName_Local = ResultImageFolderName_Local + FolderSub;
	ResultLogFolderName_Local = ResultLogFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;
	ResultLogFolderName_LAS = ResultLogFolderName_LAS + FolderSub;
	RawImageFolderName_Temp1 = RawImageFolderName_Temp1 + FolderSub;
	RawImageFolderName_Temp2 = RawImageFolderName_Temp2 + FolderSub;
	RawImageFolderName_Temp3 = RawImageFolderName_Temp3 + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(RawImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread && THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultLogFolderName_Local);

	if ((!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS) ||
		(!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS))
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread && THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ResultLogFolderName_LAS);

	if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
		THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1] = ResultLogFolderName_Local;

	if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
		THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1] = ResultLogFolderName_LAS;

	// CMI_Results\\yyyy\\mm\\dd\\LotID\Tray-xx
	FolderSub.Format("\\Tray-%d", iTrayNo);
	RawImageFolderName_Local = RawImageFolderName_Local + FolderSub;
	ResultImageFolderName_Local = ResultImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;
	RawImageFolderName_Temp1 = RawImageFolderName_Temp1 + FolderSub;
	RawImageFolderName_Temp2 = RawImageFolderName_Temp2 + FolderSub;
	RawImageFolderName_Temp3 = RawImageFolderName_Temp3 + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(RawImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultImageFolderName_Local);

	if ((!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS) ||
		(!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS))
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
	// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ResultImage
	if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(RawImageFolderName_Local + "\\RawImage");

	if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageThread && THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS + "\\RawImage");

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ResultImageFolderName_Local + "\\ResultImage");

	if (!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS + "\\ResultImage");


	THEAPP.m_FileBase.m_strRawImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(RawImageFolderName_Local + "\\RawImage");
	THEAPP.m_FileBase.m_strRawImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(ImageFolderName_LAS + "\\RawImage");
	THEAPP.m_FileBase.m_strRawImageFolder_Temp1[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(RawImageFolderName_Temp1 + "\\RawImage");
	THEAPP.m_FileBase.m_strRawImageFolder_Temp2[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(RawImageFolderName_Temp2 + "\\RawImage");
	THEAPP.m_FileBase.m_strRawImageFolder_Temp3[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(RawImageFolderName_Temp3 + "\\RawImage");

	THEAPP.m_FileBase.m_strLastRawImageFolder_Local[iPcVisionNo].Format(RawImageFolderName_Local + "\\RawImage");
	THEAPP.m_FileBase.m_strLastRawImageFolder_LAS[iPcVisionNo].Format(ImageFolderName_LAS + "\\RawImage");
	THEAPP.m_FileBase.m_strLastRawImageFolder_Temp1[iPcVisionNo].Format(RawImageFolderName_Temp1 + "\\RawImage");
	THEAPP.m_FileBase.m_strLastRawImageFolder_Temp2[iPcVisionNo].Format(RawImageFolderName_Temp2 + "\\RawImage");
	THEAPP.m_FileBase.m_strLastRawImageFolder_Temp3[iPcVisionNo].Format(RawImageFolderName_Temp3 + "\\RawImage");

	if (THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
	{
		THEAPP.m_FileBase.m_strResultImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(ResultImageFolderName_Local + "\\ResultImage");
		THEAPP.m_FileBase.m_strLastResultImageFolder[iPcVisionNo].Format(ResultImageFolderName_Local + "\\ResultImage");
	}

	if (THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS)
	{
		THEAPP.m_FileBase.m_strResultImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(ImageFolderName_LAS + "\\ResultImage");
		if (THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
			THEAPP.m_FileBase.m_strLastResultImageFolder[iPcVisionNo].Format(ResultImageFolderName_Local + "\\ResultImage");
		else
			THEAPP.m_FileBase.m_strLastResultImageFolder[iPcVisionNo].Format(ImageFolderName_LAS + "\\ResultImage");
	}
}

void CInspectSummary::MakeTrayReviewSaveFolder(int iPcVisionNo, CString sLotID, int iMzNo, int iTrayNo)
{
	CString sTrayNo;
	sTrayNo.Format("%d", iTrayNo);

	CString ReviewImageFolderName_Local;
	CString ImageFolderName_LAS;
	CString FolderSub;

	// CMI_Results
	ReviewImageFolderName_Local = THEAPP.Struct_PreferenceStruct.m_strReviewFolderPath + "CMI_Results";
	ImageFolderName_LAS = THEAPP.m_strChangeLASFolderPath[iMzNo - 1] + "CMI_Results";

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ReviewImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy
	FolderSub.Format("\\%d", THEAPP.m_LotstartInfo[iMzNo - 1].m_iCurYear);
	ReviewImageFolderName_Local = ReviewImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ReviewImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy\\mm
	FolderSub.Format("\\%d", THEAPP.m_LotstartInfo[iMzNo - 1].m_iCurMonth);
	ReviewImageFolderName_Local = ReviewImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ReviewImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy\\mm\\dd
	FolderSub.Format("\\%d", THEAPP.m_LotstartInfo[iMzNo - 1].m_iCurDay);
	ReviewImageFolderName_Local = ReviewImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ReviewImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy\\mm\\dd\\LotID
	FolderSub.Format("\\%s", sLotID);
	ReviewImageFolderName_Local = ReviewImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ReviewImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy\\mm\\dd\\LotID\Tray-xx
	FolderSub.Format("\\Tray-%d", iTrayNo);
	ReviewImageFolderName_Local = ReviewImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ReviewImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ReviewImage
	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ReviewImageFolderName_Local + "\\ReviewImage");

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS + "\\ReviewImage");

	// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ReviewImage
	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ReviewImageFolderName_Local + "\\FAIImage");

	if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread && THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS + "\\FAIImage");

	if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		THEAPP.m_FileBase.m_strReviewImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(ReviewImageFolderName_Local + "\\ReviewImage");

	if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
		THEAPP.m_FileBase.m_strReviewImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(ImageFolderName_LAS + "\\ReviewImage");

	if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		THEAPP.m_FileBase.m_strFAIImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(ReviewImageFolderName_Local + "\\FAIImage");

	if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
		THEAPP.m_FileBase.m_strFAIImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(ImageFolderName_LAS + "\\FAIImage");
}

void CInspectSummary::MakeTrayAdjSaveFolder(int iPcVisionNo, CString sLotID, int iMzNo, int iTrayNo)
{
	CString sTrayNo;
	sTrayNo.Format("%d", iTrayNo);

	CString ADJImageFolderName_Local;
	CString ImageFolderName_LAS;
	CString FolderSub;

	// CMI_Results
	ADJImageFolderName_Local = THEAPP.Struct_PreferenceStruct.m_strAdjFolderPath + "CMI_Results";
	ImageFolderName_LAS = THEAPP.m_strChangeLASFolderPath[iMzNo - 1] + "CMI_Results";

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ADJImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS == TRUE)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy
	FolderSub.Format("\\%d", THEAPP.m_LotstartInfo[iMzNo - 1].m_iCurYear);
	ADJImageFolderName_Local = ADJImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ADJImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS == TRUE)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy\\mm
	FolderSub.Format("\\%d", THEAPP.m_LotstartInfo[iMzNo - 1].m_iCurMonth);
	ADJImageFolderName_Local = ADJImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ADJImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS == TRUE)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy\\mm\\dd
	FolderSub.Format("\\%d", THEAPP.m_LotstartInfo[iMzNo - 1].m_iCurDay);
	ADJImageFolderName_Local = ADJImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ADJImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS == TRUE)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy\\mm\\dd\\LotID
	FolderSub.Format("\\%s", sLotID);
	ADJImageFolderName_Local = ADJImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ADJImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS == TRUE)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	// CMI_Results\\yyyy\\mm\\dd\\LotID\Tray-xx
	FolderSub.Format("\\Tray-%d", iTrayNo);
	ADJImageFolderName_Local = ADJImageFolderName_Local + FolderSub;
	ImageFolderName_LAS = ImageFolderName_LAS + FolderSub;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal)
		THEAPP.m_FileBase.CreateFolder(ADJImageFolderName_Local);

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS == TRUE)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS);

	if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal)
		THEAPP.m_FileBase.m_strAIFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] = ADJImageFolderName_Local;

	if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS)
		THEAPP.m_FileBase.m_strAIFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] = ImageFolderName_LAS;

	// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ADJImage
	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal && THEAPP.Struct_PreferenceStruct.m_bSaveADJ)
		THEAPP.m_FileBase.CreateFolder(ADJImageFolderName_Local + "\\ADJImage");

	if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread && THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS && THEAPP.Struct_PreferenceStruct.m_bSaveADJ)
		THEAPP.m_FileBase.CreateFolder(ImageFolderName_LAS + "\\ADJImage");

	if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal)
		THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(ADJImageFolderName_Local + "\\ADJImage");

	if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS == TRUE)
		THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1].Format(ImageFolderName_LAS + "\\ADJImage");
}

void CInspectSummary::Initialize_TrayStart_Vision_N1(CString sLotID, int iMzNo, int iTrayNo)
{
	// Tray 바뀔 시에 새로운 폴더 생성 / 저장 경로 Set
	MakeTrayResultSaveFolder(VISION_NUMBER_1, sLotID, iMzNo, iTrayNo);
	if (THEAPP.Struct_PreferenceStruct.m_bSaveMDJ)
		MakeTrayReviewSaveFolder(VISION_NUMBER_1, sLotID, iMzNo, iTrayNo);
	if (THEAPP.Struct_PreferenceStruct.m_bSaveADJ || THEAPP.Struct_PreferenceStruct.m_bSaveROIImage)
		MakeTrayAdjSaveFolder(VISION_NUMBER_1, sLotID, iMzNo, iTrayNo);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
	for (int i = 0; i < THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_iTrayModuleMax; i++)
	{
		THEAPP.m_pTrayAdminViewDlg1->m_TrayImagePB.OkNg[i] = DEFECT_TYPE_PROCESSING_ERROR;		// 초기화

		THEAPP.m_pTrayAdminViewDlg1->m_TrayImagePB.ClickRegion[i] = FALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
}

void CInspectSummary::Initialize_TrayStart_Vision_N2(CString sLotID, int iMzNo, int iTrayNo)
{
	// Tray 바뀔 시에 저장 경로 Set
	MakeTrayResultSaveFolder(VISION_NUMBER_2, sLotID, iMzNo, iTrayNo);
	if (THEAPP.Struct_PreferenceStruct.m_bSaveMDJ)
		MakeTrayReviewSaveFolder(VISION_NUMBER_2, sLotID, iMzNo, iTrayNo);
	if (THEAPP.Struct_PreferenceStruct.m_bSaveADJ || THEAPP.Struct_PreferenceStruct.m_bSaveROIImage)
		MakeTrayAdjSaveFolder(VISION_NUMBER_2, sLotID, iMzNo, iTrayNo);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
	for (int i = 0; i < THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_iTrayModuleMax; i++)
	{
		THEAPP.m_pTrayAdminViewDlg2->m_TrayImagePB.OkNg[i] = DEFECT_TYPE_PROCESSING_ERROR;		// 초기화

		THEAPP.m_pTrayAdminViewDlg2->m_TrayImagePB.ClickRegion[i] = FALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
}

void CInspectSummary::Initialize_TrayStart_Vision_N3(CString sLotID, int iMzNo, int iTrayNo)
{
	// Tray 바뀔 시에 새로운 폴더 생성 / 저장 경로 Set
	MakeTrayResultSaveFolder(VISION_NUMBER_3, sLotID, iMzNo, iTrayNo);
	if (THEAPP.Struct_PreferenceStruct.m_bSaveMDJ)
		MakeTrayReviewSaveFolder(VISION_NUMBER_3, sLotID, iMzNo, iTrayNo);
	if (THEAPP.Struct_PreferenceStruct.m_bSaveADJ || THEAPP.Struct_PreferenceStruct.m_bSaveROIImage)
		MakeTrayAdjSaveFolder(VISION_NUMBER_3, sLotID, iMzNo, iTrayNo);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
	for (int i = 0; i < THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_iTrayModuleMax; i++)
	{
		THEAPP.m_pTrayAdminViewDlg3->m_TrayImagePB.OkNg[i] = DEFECT_TYPE_PROCESSING_ERROR;		// 초기화

		THEAPP.m_pTrayAdminViewDlg3->m_TrayImagePB.ClickRegion[i] = FALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
}

void CInspectSummary::Initialize_TrayStart_Vision_N4(CString sLotID, int iMzNo, int iTrayNo)
{
	// Tray 바뀔 시에 저장 경로 Set
	MakeTrayResultSaveFolder(VISION_NUMBER_4, sLotID, iMzNo, iTrayNo);
	if (THEAPP.Struct_PreferenceStruct.m_bSaveMDJ)
		MakeTrayReviewSaveFolder(VISION_NUMBER_4, sLotID, iMzNo, iTrayNo);
	if (THEAPP.Struct_PreferenceStruct.m_bSaveADJ || THEAPP.Struct_PreferenceStruct.m_bSaveROIImage)
		MakeTrayAdjSaveFolder(VISION_NUMBER_4, sLotID, iMzNo, iTrayNo);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
	for (int i = 0; i < THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_iTrayModuleMax; i++)
	{
		THEAPP.m_pTrayAdminViewDlg4->m_TrayImagePB.OkNg[i] = DEFECT_TYPE_PROCESSING_ERROR;		// 초기화

		THEAPP.m_pTrayAdminViewDlg4->m_TrayImagePB.ClickRegion[i] = FALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
}

int CInspectSummary::CheckLotIDAndChangeModel(int iMzNo, CString sLotID, CString sHandlerModelName, CString *psProgramVersion, CString *psModelVersion)
{
	int iSWVersion = THEAPP.GetProgramVersion();
	int iParamVersion = 1000;
	BOOL bModelExistCheck = FALSE;

	auto log_time_start = std::chrono::high_resolution_clock::now();
	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	(*psProgramVersion).Format("%d", iSWVersion);
	*psModelVersion = "1000";

	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
	if (sLotID.GetLength() <= 11)
	{
		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("Model auto load Skip(Test lot), Time(ms): %d, LotID: %s, Port: %d, Dual index: %d, Model name: -", log_time_ms, sLotID, iMzNo, iDualModelData);
		THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));

		return MODEL_AUTO_LOAD_RESULT_SKIP;
	}

	CString strLotID, strLotModelType, strLotConfig;
	int iModelTypeCodeLength, iModelConfigLength;
	strLotID = sLotID;
	strLotID.Delete(0, 3);

	for (int i = 0; i <= MODEL_TYPE_MAX; i++)
	{
		if (i == MODEL_TYPE_MAX)
		{
			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("Model auto load Cancel (Can't find lot code), Time(ms): %d, LotID: %s, Port: %d, Dual index: %d, Model name: -", log_time_ms, sLotID, iMzNo, iDualModelData);
			THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));

			return MODEL_AUTO_LOAD_RESULT_SKIP;
		}

		iModelTypeCodeLength = THEAPP.g_strModelTypeLotCode[i].GetLength();
		strLotModelType = strLotID.Mid(0, iModelTypeCodeLength);

		if (THEAPP.g_strModelTypeLotCode[i] == strLotModelType)
		{
			if (!THEAPP.IsSameModelType(i))
			{
				THEAPP.SetModelType(i);
				THEAPP.ReadInspectionTypeInfoINI();
				THEAPP.ReadOverlayViewportINI();
				THEAPP.ReadDefectReViewInfoINI();
				THEAPP.ReadFaiDefectReViewInfo();
			}

			break;
		}
	}

	strLotConfig = strLotID.Mid(iModelTypeCodeLength, 4 - iModelTypeCodeLength);

	CString strPCNumber, strEquipConfig;
	strPCNumber.Format("PC%d", THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
	CString strIniFile;
	strIniFile.Format("AutoLoadModelConfig_%s.ini", strEquipModel);
	CIniFileCS INI_AutoLoadModelConfig(strDataFolder + strIniFile);
	strEquipConfig = INI_AutoLoadModelConfig.Get_String(strPCNumber, strLotConfig, "X-X-X-X");

	CString strModelFolder, strModelName;
	strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\*.*";

	UINT nModelTimeCheck = 0;
	struct _finddata_t fd;
	intptr_t handle;
	if ((handle = _findfirst(strModelFolder, &fd)) == -1L)
	{
	}
	else
	{
		do
		{
			CString FolderList = fd.name;
			int nFolderCheck = FolderList.Find('.');
			if (nFolderCheck == -1)
			{
				CString strRecipeEquipNoCheck, strRecipeModelTypeCheck, strRecipePCNoCheck, strRecipeConfigCheck, strRecipeDateCheck, strRecipeTimeCheck, strTrash;
				UINT iRecipeTimeCheck;

				strRecipeEquipNoCheck = FolderList.Mid(0, FolderList.Find('_'));

				strTrash = FolderList.Mid(FolderList.Find('_') + 1);
				strRecipeModelTypeCheck = strTrash.Mid(0, strTrash.Find('_'));

				strTrash = strTrash.Mid(strTrash.Find('_') + 1);
				strRecipePCNoCheck = strTrash.Mid(0, strTrash.Find('_'));

				strTrash = strTrash.Mid(strTrash.Find('_') + 1);
				strRecipeConfigCheck = strTrash.Mid(0, strTrash.Find('_'));

				strRecipeDateCheck = strTrash.Mid(strTrash.Find('_') + 1);
				if (strRecipeDateCheck.Find('-') == -1)
					strRecipeTimeCheck = strRecipeDateCheck + "0000";
				else
				{
					strRecipeDateCheck = strRecipeDateCheck.Mid(0, strRecipeDateCheck.Find('-'));
					strRecipeTimeCheck = strTrash.Mid(strTrash.Find('-') + 1);
					strRecipeTimeCheck = strRecipeDateCheck + strRecipeTimeCheck;
				}
				iRecipeTimeCheck = _tcstoul(strRecipeTimeCheck, nullptr, 0);

				if (strRecipeModelTypeCheck == THEAPP.g_strModelTypeName[THEAPP.GetModelType()] &&
					strRecipeEquipNoCheck == THEAPP.Struct_PreferenceStruct.m_strEquipNo &&
					strRecipePCNoCheck == strPCNumber &&
					strRecipeConfigCheck == strEquipConfig)
				{
					bModelExistCheck = TRUE;

					if (iRecipeTimeCheck > nModelTimeCheck)
					{
						strModelName = FolderList;
						nModelTimeCheck = iRecipeTimeCheck;
					}
				}
			}

		} while (_findnext(handle, &fd) == 0);
	}
	_findclose(handle);

	if (bModelExistCheck == FALSE)
	{
		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("Model auto load Fail (Model does not exist), Time(ms): %d, LotID: %s, Port: %d, Dual index: %d, Model name: %s", log_time_ms, sLotID, iMzNo, iDualModelData, strModelName);
		THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));

		if (strModelName.GetLength() < 35)
			THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
		else
			THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 8, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_VISION_MODEL_NAME, strModelName);

		return MODEL_AUTO_LOAD_RESULT_FAIL;
	}

	if (THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName == strModelName)
	{
		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("Model auto load Skip (Lastest model is already loaded), Time(ms): %d, LotID: %s, Port: %d, Dual index: %d, Model name: %s", log_time_ms, sLotID, iMzNo, iDualModelData, strModelName);
		THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));

		if (strModelName.GetLength() < 35)
			THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
		else
			THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 8, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_VISION_MODEL_NAME, strModelName);

		return MODEL_AUTO_LOAD_RESULT_SKIP;
	}

	if (iDualModelData == MODEL_DATA_1)
		iDualModelData = MODEL_DATA_2;
	else
		iDualModelData = MODEL_DATA_1;
	THEAPP.g_iDualModelData[iMzNo - 1] = iDualModelData;

	if (THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName == strModelName)
	{
		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("Model auto load Skip (Lastest model is already loaded), Time(ms): %d, LotID: %s, Port: %d, Dual index: %d, Model name: %s", log_time_ms, sLotID, iMzNo, iDualModelData, strModelName);
		THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));

		if (strModelName.GetLength() < 35)
			THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
		else
			THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 8, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_VISION_MODEL_NAME, strModelName);

		return MODEL_AUTO_LOAD_RESULT_SKIP;
	}

	if (THEAPP.g_iDualModelDataRunCheck[MODEL_DATA_1] == 0)
		THEAPP.g_iDualModelData[iMzNo - 1] = MODEL_DATA_1;
	else
		THEAPP.g_iDualModelData[iMzNo - 1] = MODEL_DATA_2;
	THEAPP.g_iDualModelData[iMzNo - 1] = iDualModelData;

	THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName = strModelName;
	THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_2]->m_sModelName = THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;
	THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_3]->m_sModelName = THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;
	THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_4]->m_sModelName = THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;

	if (strModelName.GetLength() < 35)
		THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
	else
		THEAPP.m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 8, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
	THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_VISION_MODEL_NAME, strModelName);

	MODEL_LOAD_THREAD_PARAM* pModelLoadThreadParam = new MODEL_LOAD_THREAD_PARAM(this, iDualModelData);
	AfxBeginThread(ModelLoadThread, pModelLoadThreadParam);

	CSingleLock s(&m_evModelLoadDone);

	s.Lock();

	THEAPP.UpdateVersionText();

	iParamVersion = THEAPP.GetProgramVersion();
	(*psModelVersion).Format("%d", iParamVersion);

	CString sVisionCamType;
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

	log_time_end = std::chrono::high_resolution_clock::now();
	log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("Model auto load Complete, Time(ms): %d, LotID: %s, Port: %d, Dual index: %d, Model name: %s", log_time_ms, sLotID, iMzNo, iDualModelData, strModelName);
	THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));

	return MODEL_AUTO_LOAD_RESULT_COMPLETE;
}

void CInspectSummary::ShowDiskCapacity()
{
	float TotalBytes, FreeBytes;
	float fTotal, fFree, fUsed;

	ULARGE_INTEGER ulUserFree, ulTotal, ulRealFree;

	CString sDir = "C:\\";
	if (GetDiskFreeSpaceEx(sDir, &ulUserFree, &ulTotal, &ulRealFree))
	{
		TotalBytes = ulTotal.LowPart + (double)ulTotal.HighPart*(double)4294967296;
		fTotal = (float)(TotalBytes / 1024 / 1024 / 1024); // Convert (GB)     
		FreeBytes = ulRealFree.LowPart + (double)ulRealFree.HighPart*(double)4294967296;
		fFree = (float)(FreeBytes / 1024 / 1024 / 1024); // Convert (GB) 
		fUsed = fTotal - fFree;
	}
	int iPos = int(fUsed / fTotal * 100);
	if (iPos < 0)
		iPos = 0;
	if (iPos > 100)
		iPos = 100;
	SetDlgItemInt(IDC_STATIC_DISK_HDDC_PRO, iPos);
	m_ctrlProgressHddC.SetRange(0, 100);
	m_ctrlProgressHddC.SetPos(iPos);
	m_ctrlProgressHddC.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(255, 0, 0));

	sDir = "D:\\";
	if (GetDiskFreeSpaceEx(sDir, &ulUserFree, &ulTotal, &ulRealFree))
	{
		TotalBytes = ulTotal.LowPart + (double)ulTotal.HighPart*(double)4294967296;
		fTotal = (float)(TotalBytes / 1024 / 1024 / 1024); // Convert (GB)     
		FreeBytes = ulRealFree.LowPart + (double)ulRealFree.HighPart*(double)4294967296;
		fFree = (float)(FreeBytes / 1024 / 1024 / 1024); // Convert (GB) 
		fUsed = fTotal - fFree;
	}
	iPos = int(fUsed / fTotal * 100);
	if (iPos < 0)
		iPos = 0;
	if (iPos > 100)
		iPos = 100;
	SetDlgItemInt(IDC_STATIC_DISK_HDDD_PRO, iPos);
	m_ctrlProgressHddD.SetRange(0, 100);
	m_ctrlProgressHddD.SetPos(iPos);
	m_ctrlProgressHddD.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(255, 0, 0));
}


BOOL CInspectSummary::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}


void CInspectSummary::OnBnClickedMfcbuttonSaveParam()
{
	int nRes = 0;
	nRes = AfxMessageBox("현재 검사 파라미터를 파일로 저장하시겠습니까?", MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	CString sSaveRootFolderName = THEAPP.Struct_PreferenceStruct.m_strEtcFolderPath + "CMI_Results\\TeachingParam";

	THEAPP.SaveTeachingParameter(sSaveRootFolderName);

	AfxMessageBox("티칭 파라미터 저장이 완료되었습니다. [CMI_Results]->[TeachingParam] 폴더를 확인해 주십시요", MB_ICONINFORMATION | MB_SYSTEMMODAL);
}


void CInspectSummary::OnBnClickedMfcbuttonUpdateParam()
{
	int nRes = 0;
	nRes = AfxMessageBox("검사 파라미터 버전을 업데이트하시겠습니까?", MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->UpdateModelParamVersion();

	int iParamVersion;
	iParamVersion = THEAPP.GetSwParamVersion();

	CString sMsg;
	sMsg.Format("검사 파라미터 버전 %d로 업데이트되었습니다. 파라미터 변경 내역은 [CMI_Results]->[ParameterChangeHistory] 폴더를 확인해 주십시요.", iParamVersion);

	AfxMessageBox(sMsg, MB_ICONINFORMATION | MB_SYSTEMMODAL);
}



#include "MacroPathList.h"

void CInspectSummary::OnBnClickedMfcbuttonMacroPath()
{
	CMacroPathList dlg;
	dlg.DoModal();
}


void CInspectSummary::OnBnClickedChkAutosettingZfocus()
{
	UpdateData();

	if (m_bDxAutoSettingLight)
	{
		m_bDxAutoSettingLight = FALSE;
		m_bUseAutoSettingLight = FALSE;
	}

	m_bUseAutoSettingZFocus = m_bDxAutoSettingZFocus;

	UpdateData(FALSE);
}


void CInspectSummary::OnBnClickedChkAutosettingLight()
{
	UpdateData();

	if (m_bDxAutoSettingZFocus)
	{
		m_bDxAutoSettingZFocus = FALSE;
		m_bUseAutoSettingZFocus = FALSE;
	}

	m_bUseAutoSettingLight = m_bDxAutoSettingLight;

	UpdateData(FALSE);
}


void CInspectSummary::OnBnClickedCheckRunMacro()
{
	UpdateData();

	m_bUseRunMacro = m_bDxRunMacro;

	if (THEAPP.m_pInspectSummary->m_bUseRunMacro)
		GetDlgItem(IDC_MFCBUTTON_MACRO_PATH)->ShowWindow(SW_SHOW);
	else
		GetDlgItem(IDC_MFCBUTTON_MACRO_PATH)->ShowWindow(SW_HIDE);
}
