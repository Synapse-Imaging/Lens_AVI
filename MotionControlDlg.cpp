// MotionControlDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "MotionControlDlg.h"
#include "afxdialogex.h"


// CMotionControlDlg 대화 상자

IMPLEMENT_DYNAMIC(CMotionControlDlg, CDialog)

CMotionControlDlg* CMotionControlDlg::m_pInstance = NULL;

CMotionControlDlg* CMotionControlDlg::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CMotionControlDlg;

		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

			m_pInstance->Create(IDD_MOTION_CONTROL_DLG, pFrame->GetActiveView());
		}
	}

	return m_pInstance;
}

void CMotionControlDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

void CMotionControlDlg::Show()
{
	SetWindowPos(&wndTopMost, 1000, 5, 0, 0, SWP_NOSIZE);
	ShowWindow(SW_SHOW);

	m_bShowDlg = TRUE;

#ifdef INLINE_MODE
	int iJigNo;
	if (THEAPP.m_pCameraManager->GetVisionCamName() == VISION_NUMBER_1 || THEAPP.m_pCameraManager->GetVisionCamName() == VISION_NUMBER_2)
		iJigNo = (THEAPP.Struct_PreferenceStruct.m_iPCType * 2) + 1;
	else
		iJigNo = (THEAPP.Struct_PreferenceStruct.m_iPCType * 2) + 2;

	THEAPP.m_pHandlerService->Set_PositionRequest(THEAPP.m_pCameraManager->GetVisionCamName(), THEAPP.m_iCurStageIndex, iJigNo);
#endif
}

void CMotionControlDlg::Hide()
{
	ShowWindow(SW_HIDE);

	m_bShowDlg = FALSE;
}

UINT AMoveRequest_Thread(LPVOID lp)
{
	CString strLog;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[THEAPP.m_pCameraManager->GetVisionCamName()];

	CMotionControlDlg* pMotionControlDlg = (CMotionControlDlg*)lp;

	DWORD dwMoveStart = 0, dwMoveEnd = 0;

	int iJigNo;
	if (THEAPP.m_pCameraManager->GetVisionCamName() == VISION_NUMBER_1 || THEAPP.m_pCameraManager->GetVisionCamName() == VISION_NUMBER_2)
		iJigNo = (THEAPP.Struct_PreferenceStruct.m_iPCType * 2) + 1;
	else
		iJigNo = (THEAPP.Struct_PreferenceStruct.m_iPCType * 2) + 2;

	THEAPP.m_pHandlerService->m_bMotionMoveComplete[THEAPP.m_pCameraManager->GetVisionCamName()] = FALSE;
	THEAPP.m_pHandlerService->Set_AMoveRequest(THEAPP.m_pCameraManager->GetVisionCamName(), THEAPP.m_iCurStageIndex, iJigNo, pMotionControlDlg->m_dCurZPosition, pMotionControlDlg->m_dCurLightZPosition, pMotionControlDlg->m_dCurXPosition, pMotionControlDlg->m_dCurYPosition, pMotionControlDlg->m_dCurTPosition, pMotionControlDlg->m_dCurRPosition);

	dwMoveStart = GetTickCount();

	while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[THEAPP.m_pCameraManager->GetVisionCamName()]) // 무브 컴플리트 기다림
	{
		dwMoveEnd = GetTickCount();

		if ((dwMoveEnd - dwMoveStart) > 5000)
		{
			strLog.Format("%s/ Manual move complete timeout", sVisionCamType_Short);
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

			AfxMessageBox("핸들러로부터 모션 완료 신호가 없습니다. 핸들러 상태 확인해 주세요", MB_SYSTEMMODAL | MB_ICONERROR);
			return 0;
		}

		Sleep(1);
	}

	THEAPP.m_pHandlerService->Set_PositionRequest(THEAPP.m_pCameraManager->GetVisionCamName(), THEAPP.m_iCurStageIndex, iJigNo);

	return 1;
}

CMotionControlDlg::CMotionControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MOTION_CONTROL_DLG, pParent)
{
	m_dEditCurPos_Z = 0.0;
	m_dEditCurPos_LightZ = 0.0;
	m_dEditAMovePos_Z = 5.0;
	m_dEditRMovePos_Z = 0.1;
	m_dEditAMovePos_LightZ = 5.0;
	m_dEditRMovePos_LightZ = 0.1;
	m_dEditCurPos_T = 0.0;
	m_dEditAMovePos_T = 5.0;
	m_dEditRMovePos_T = 0.1;
	m_dEditCurPos_R = 0.0;
	m_dEditAMovePos_R = 5.0;
	m_dEditRMovePos_R = 0.1;
	m_dEditCurPos_X = 0.0;
	m_dEditAMovePos_X = 5.0;
	m_dEditRMovePos_X = 0.1;
	m_dEditCurPos_Y = 0.0;
	m_dEditAMovePos_Y = 5.0;
	m_dEditRMovePos_Y = 0.1;

	m_bShowDlg = FALSE;
}

CMotionControlDlg::~CMotionControlDlg()
{
}

void CMotionControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_GET_CURRENT_POSITION, m_bnGetCurrentPos);
	DDX_Control(pDX, IDC_BUTTON_SET_CURRENT_POSITION_TO_IMAGE, m_bnSetCurrentPosToImage);
	DDX_Control(pDX, IDC_BUTTON_AMOVE_Z, m_bnAMove_Z);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_PLUS_Z, m_bnJMovePlus_Z);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_MINUS_Z, m_bnJMoveMinus_Z);
	DDX_Control(pDX, IDC_BUTTON_AMOVE_LIGHT_Z, m_bnAMove_LightZ);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_PLUS_LIGHT_Z, m_bnJMovePlus_LightZ);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_MINUS_LIGHT_Z, m_bnJMoveMinus_LightZ);
	DDX_Control(pDX, IDC_BUTTON_AMOVE_T, m_bnAMove_T);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_PLUS_T, m_bnJMovePlus_T);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_MINUS_T, m_bnJMoveMinus_T);
	DDX_Control(pDX, IDC_BUTTON_AMOVE_R, m_bnAMove_R);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_PLUS_R, m_bnJMovePlus_R);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_MINUS_R, m_bnJMoveMinus_R);
	DDX_Control(pDX, IDC_BUTTON_AMOVE_X, m_bnAMove_X);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_PLUS_X, m_bnJMovePlus_X);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_MINUS_X, m_bnJMoveMinus_X);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_bnClose);

	DDX_Text(pDX, IDC_EDIT_CURRENT_POSITION_Z, m_dEditCurPos_Z);
	DDX_Text(pDX, IDC_EDIT_CURRENT_POSITION_LIGHT_Z, m_dEditCurPos_LightZ);
	DDX_Text(pDX, IDC_EDIT_CURRENT_POSITION_T, m_dEditCurPos_T);
	DDX_Text(pDX, IDC_EDIT_CURRENT_POSITION_R, m_dEditCurPos_R);
	DDX_Text(pDX, IDC_EDIT_CURRENT_POSITION_X, m_dEditCurPos_X);
	DDX_Text(pDX, IDC_EDIT_CURRENT_POSITION_Y, m_dEditCurPos_Y);

	DDX_Text(pDX, IDC_EDIT_AMOVE_POSITION_Z, m_dEditAMovePos_Z);
	DDX_Text(pDX, IDC_EDIT_JMOVE_POSITION_Z, m_dEditRMovePos_Z);
	DDX_Text(pDX, IDC_EDIT_AMOVE_LIGHT_Z, m_dEditAMovePos_LightZ);
	DDX_Text(pDX, IDC_EDIT_JMOVE_LIGHT_Z, m_dEditRMovePos_LightZ);
	DDX_Text(pDX, IDC_EDIT_AMOVE_POSITION_T, m_dEditAMovePos_T);
	DDX_Text(pDX, IDC_EDIT_JMOVE_POSITION_T, m_dEditRMovePos_T);
	DDX_Text(pDX, IDC_EDIT_AMOVE_POSITION_R, m_dEditAMovePos_R);
	DDX_Text(pDX, IDC_EDIT_JMOVE_POSITION_R, m_dEditRMovePos_R);
	DDX_Text(pDX, IDC_EDIT_AMOVE_POSITION_X, m_dEditAMovePos_X);
	DDX_Text(pDX, IDC_EDIT_JMOVE_POSITION_X, m_dEditRMovePos_X);
	DDX_Text(pDX, IDC_EDIT_AMOVE_POSITION_Y, m_dEditAMovePos_Y);
	DDX_Text(pDX, IDC_EDIT_JMOVE_POSITION_Y, m_dEditRMovePos_Y);

	DDX_Control(pDX, IDC_COMBO_INSPECT_IMAGE, m_cbInspectImageNo);
}


BEGIN_MESSAGE_MAP(CMotionControlDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_GET_CURRENT_POSITION, &CMotionControlDlg::OnBnClickedButtonGetCurrentPosition)
	ON_BN_CLICKED(IDC_BUTTON_SET_CURRENT_ZPOSITION_TO_IMAGE, &CMotionControlDlg::OnBnClickedButtonSetCurrentZpositionToImage)
	ON_BN_CLICKED(IDC_BUTTON_SET_CURRENT_POSITION_TO_IMAGE, &CMotionControlDlg::OnBnClickedButtonSetCurrentPositionToImage)
	ON_BN_CLICKED(IDC_BUTTON_AMOVE_Z, &CMotionControlDlg::OnBnClickedButtonAmoveZ)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_PLUS_Z, &CMotionControlDlg::OnBnClickedButtonJmovePlusZ)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_MINUS_Z, &CMotionControlDlg::OnBnClickedButtonJmoveMinusZ)
	ON_BN_CLICKED(IDC_BUTTON_AMOVE_LIGHT_Z, &CMotionControlDlg::OnBnClickedButtonAmoveLightZ)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_PLUS_LIGHT_Z, &CMotionControlDlg::OnBnClickedButtonJmovePlusLightZ)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_MINUS_LIGHT_Z, &CMotionControlDlg::OnBnClickedButtonJmoveMinusLightZ)
	ON_BN_CLICKED(IDC_BUTTON_AMOVE_T, &CMotionControlDlg::OnBnClickedButtonAmoveT)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_PLUS_T, &CMotionControlDlg::OnBnClickedButtonJmovePlusT)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_MINUS_T, &CMotionControlDlg::OnBnClickedButtonJmoveMinusT)
	ON_BN_CLICKED(IDC_BUTTON_AMOVE_R, &CMotionControlDlg::OnBnClickedButtonAmoveR)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_PLUS_R, &CMotionControlDlg::OnBnClickedButtonJmovePlusR)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_MINUS_R, &CMotionControlDlg::OnBnClickedButtonJmoveMinusR)
	ON_BN_CLICKED(IDC_BUTTON_AMOVE_X, &CMotionControlDlg::OnBnClickedButtonAmoveX)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_PLUS_X, &CMotionControlDlg::OnBnClickedButtonJmovePlusX)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_MINUS_X, &CMotionControlDlg::OnBnClickedButtonJmoveMinusX)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CMotionControlDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_AMOVE_Y, &CMotionControlDlg::OnBnClickedButtonAmoveY)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_PLUS_Y, &CMotionControlDlg::OnBnClickedButtonJmovePlusY)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_MINUS_Y, &CMotionControlDlg::OnBnClickedButtonJmoveMinusY)
END_MESSAGE_MAP()

BOOL CMotionControlDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_NCLBUTTONDOWN:
		SetActiveWindow();
		return FALSE;

	case WM_KEYDOWN:
		if ((int)pMsg->wParam == VK_ESCAPE || (int)pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}

	case WM_SYSKEYDOWN:
		if (pMsg->wParam == VK_F4)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// CMotionControlDlg 메시지 처리기
BOOL CMotionControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTopMost, 1000, 5, 0, 0, SWP_NOSIZE);

	m_bnGetCurrentPos.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnSetCurrentPosToImage.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnAMove_Z.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnJMovePlus_Z.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnJMoveMinus_Z.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_bnAMove_LightZ.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(255, 255, 255), 0, 0);
	m_bnJMovePlus_LightZ.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(255, 255, 255), 0, 0);
	m_bnJMoveMinus_LightZ.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(255, 255, 255), 0, 0);

	m_bnAMove_T.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(128, 255, 0), 0, 0);
	m_bnJMovePlus_T.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(128, 255, 0), 0, 0);
	m_bnJMoveMinus_T.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(128, 255, 0), 0, 0);

	m_bnAMove_R.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(192, 192, 192), 0, 0);
	m_bnJMovePlus_R.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(192, 192, 192), 0, 0);
	m_bnJMoveMinus_R.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(192, 192, 192), 0, 0);

	m_bnAMove_X.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0, 255, 255), 0, 0);
	m_bnJMovePlus_X.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0, 255, 255), 0, 0);
	m_bnJMoveMinus_X.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0, 255, 255), 0, 0);

	m_bnClose.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	ChangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CMotionControlDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("핸들러 모션 제어"));
		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_1, _T("현재 위치"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_1, _T("Camera Z(mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_2, _T("Light Z(mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_3, _T("Stage Tilt(deg)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_4, _T("Zig Rotate(deg)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_5, _T("Stage X(mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_6, _T("Stage Y(mm)"));

		CString sTemp;
		m_cbInspectImageNo.ResetContent();
		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			sTemp.Format("영상 %d", j + 1);
			m_cbInspectImageNo.AddString(sTemp);
		}
		m_cbInspectImageNo.SetCurSel(0);
		SetDlgItemText(IDC_BUTTON_GET_CURRENT_POSITION, _T("현재 위치 수신"));
		SetDlgItemText(IDC_BUTTON_SET_CURRENT_ZPOSITION_TO_IMAGE, _T("검사 영상에 카메라 Z 값 적용"));
		SetDlgItemText(IDC_BUTTON_SET_CURRENT_POSITION_TO_IMAGE, _T("검사 영상에 전체 값 적용"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_2, _T("카메라 Z축 이동 (mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_7, _T("절대값: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_Z, _T("절대값 이동"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_8, _T("상대값: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_Z, _T("+ Z 이동"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_Z, _T("- Z 이동"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_3, _T("조명 Z축 이동 (mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_9, _T("절대값: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_LIGHT_Z, _T("절대값 이동"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_10, _T("상대값: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_LIGHT_Z, _T("+ Z 이동"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_LIGHT_Z, _T("- Z 이동"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_4, _T("스테이지 T축 회전 (도)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_11, _T("절대값: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_T, _T("절대값 회전"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_12, _T("상대값: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_T, _T("+ T 회전"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_T, _T("- T 회전"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_5, _T("지그 R축 회전 (도)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_13, _T("절대값: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_R, _T("절대값 회전"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_14, _T("상대값: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_R, _T("+ R 회전"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_R, _T("- R 회전"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_6, _T("스테이지 X축 이동 (mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_15, _T("절대값: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_X, _T("절대값 이동"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_16, _T("상대값: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_X, _T("+ X 이동"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_X, _T("- X 이동"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_7, _T("스테이지 Y축 이동 (mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_17, _T("절대값: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_Y, _T("절대값 이동"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_18, _T("상대값: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_Y, _T("+ Y 이동"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_Y, _T("- Y 이동"));

		SetDlgItemText(IDC_BUTTON_CLOSE, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Handler motion control"));
		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_1, _T("Current position"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_1, _T("Camera Z(mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_2, _T("Light Z(mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_3, _T("Stage Tilt(deg)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_4, _T("Zig Rotate(deg)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_5, _T("Stage X(mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_6, _T("Stage Y(mm)"));

		CString sTemp;
		m_cbInspectImageNo.ResetContent();
		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			sTemp.Format("Image %d", j + 1);
			m_cbInspectImageNo.AddString(sTemp);
		}
		m_cbInspectImageNo.SetCurSel(0);
		SetDlgItemText(IDC_BUTTON_GET_CURRENT_POSITION, _T("Receive cur pos"));
		SetDlgItemText(IDC_BUTTON_SET_CURRENT_ZPOSITION_TO_IMAGE, _T("Apply Cam-Z to images."));
		SetDlgItemText(IDC_BUTTON_SET_CURRENT_POSITION_TO_IMAGE, _T("Apply all position to image"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_2, _T("Move to camera Z (mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_7, _T("Absolute: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_Z, _T("Move to abs"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_8, _T("Relative: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_Z, _T("+ Z move"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_Z, _T("- Z move"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_3, _T("Move to light Z (mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_9, _T("Absolute: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_LIGHT_Z, _T("Move to abs"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_10, _T("Relative: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_LIGHT_Z, _T("+ Z move"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_LIGHT_Z, _T("- Z move"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_4, _T("Rotate to stage T (deg)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_11, _T("Absolute: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_T, _T("Rotate to abs"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_12, _T("Relative: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_T, _T("+ T rotate"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_T, _T("- T rotate"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_5, _T("Rotate to jig R (deg)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_13, _T("Absolute: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_R, _T("Rotate to abs"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_14, _T("Relative: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_R, _T("+ T rotate"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_R, _T("- T rotate"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_6, _T("Move to stage X (mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_15, _T("Absolute: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_X, _T("Move to abs"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_16, _T("Relative: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_X, _T("+ X move"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_X, _T("- X move"));

		SetDlgItemText(IDC_GROUP_MOTIONCONTROLDLG_7, _T("Move to stage Y (mm)"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_17, _T("Absolute: "));
		SetDlgItemText(IDC_BUTTON_AMOVE_Y, _T("Move to abs"));
		SetDlgItemText(IDC_STATIC_MOTIONCONTROLDLG_18, _T("Relative: "));
		SetDlgItemText(IDC_BUTTON_JMOVE_PLUS_Y, _T("+ Y move"));
		SetDlgItemText(IDC_BUTTON_JMOVE_MINUS_Y, _T("- Y move"));

		SetDlgItemText(IDC_BUTTON_CLOSE, _T("Close"));
	}
}

void CMotionControlDlg::OnBnClickedButtonGetCurrentPosition()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE
	int iJigNo;
	if (THEAPP.m_pCameraManager->GetVisionCamName() == VISION_NUMBER_1 || THEAPP.m_pCameraManager->GetVisionCamName() == VISION_NUMBER_2)
		iJigNo = (THEAPP.Struct_PreferenceStruct.m_iPCType * 2) + 1;
	else
		iJigNo = (THEAPP.Struct_PreferenceStruct.m_iPCType * 2) + 2;

	THEAPP.m_pHandlerService->Set_PositionRequest(THEAPP.m_pCameraManager->GetVisionCamName(), THEAPP.m_iCurStageIndex, iJigNo);
#endif
}

void CMotionControlDlg::OnBnClickedButtonAmoveZ()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = m_dEditAMovePos_Z;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);

#endif
}


void CMotionControlDlg::OnBnClickedButtonJmovePlusZ()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = m_dEditCurPos_Z + m_dEditRMovePos_Z;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}


void CMotionControlDlg::OnBnClickedButtonJmoveMinusZ()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = m_dEditCurPos_Z - m_dEditRMovePos_Z;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}

void CMotionControlDlg::OnBnClickedButtonAmoveLightZ()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);

	double dTemp;
	dTemp = m_dEditAMovePos_LightZ;
	if (dTemp < 0)
		return;

	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = m_dEditAMovePos_LightZ;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);

#endif
}


void CMotionControlDlg::OnBnClickedButtonJmovePlusLightZ()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = m_dEditCurPos_LightZ + m_dEditRMovePos_LightZ;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}


void CMotionControlDlg::OnBnClickedButtonJmoveMinusLightZ()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);

	double dTemp;
	dTemp = m_dEditCurPos_LightZ - m_dEditRMovePos_LightZ;
	if (dTemp < 0)
		return;

	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = m_dEditCurPos_LightZ - m_dEditRMovePos_LightZ;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}

void CMotionControlDlg::OnBnClickedButtonAmoveT()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = m_dEditAMovePos_T;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);

#endif
}


void CMotionControlDlg::OnBnClickedButtonJmovePlusT()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = m_dEditCurPos_T + m_dEditRMovePos_T;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}


void CMotionControlDlg::OnBnClickedButtonJmoveMinusT()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = m_dEditCurPos_T - m_dEditRMovePos_T;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}

void CMotionControlDlg::OnBnClickedButtonAmoveR()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = m_dEditAMovePos_R;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);

#endif
}


void CMotionControlDlg::OnBnClickedButtonJmovePlusR()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = m_dEditCurPos_R + m_dEditRMovePos_R;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}


void CMotionControlDlg::OnBnClickedButtonJmoveMinusR()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = m_dEditCurPos_R - m_dEditRMovePos_R;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}

void CMotionControlDlg::OnBnClickedButtonAmoveX()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = m_dEditAMovePos_X;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);

#endif
}


void CMotionControlDlg::OnBnClickedButtonJmovePlusX()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = m_dEditCurPos_X + m_dEditRMovePos_X;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}


void CMotionControlDlg::OnBnClickedButtonJmoveMinusX()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = m_dEditCurPos_X - m_dEditRMovePos_X;
	m_dCurYPosition = MOTION_NOUSE;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}

void CMotionControlDlg::OnBnClickedButtonClose()
{
	Hide();
}


void CMotionControlDlg::OnBnClickedButtonAmoveY()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = m_dEditAMovePos_Y;

	AfxBeginThread(AMoveRequest_Thread, this);

#endif
}


void CMotionControlDlg::OnBnClickedButtonJmovePlusY()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = m_dEditCurPos_Y + m_dEditRMovePos_Y;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}


void CMotionControlDlg::OnBnClickedButtonJmoveMinusY()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = MOTION_NOUSE;
	m_dCurLightZPosition = MOTION_NOUSE;
	m_dCurTPosition = MOTION_NOUSE;
	m_dCurRPosition = MOTION_NOUSE;
	m_dCurXPosition = MOTION_NOUSE;
	m_dCurYPosition = m_dEditCurPos_Y - m_dEditRMovePos_Y;

	AfxBeginThread(AMoveRequest_Thread, this);
#endif
}


void CMotionControlDlg::OnBnClickedButtonSetCurrentPositionToImage()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	UpdateData();

	int iInspectImageIndex = m_cbInspectImageNo.GetCurSel();

	THEAPP.m_pTabControlDlg->m_pJogSetDlg->SetMotionParameter(iInspectImageIndex, m_dEditCurPos_Z, m_dEditCurPos_LightZ, m_dEditCurPos_T, m_dEditCurPos_R, m_dEditCurPos_X, m_dEditCurPos_Y);
	THEAPP.m_pTabControlDlg->m_pCameraLightManagerDlg->SetMotionParameter(iInspectImageIndex, m_dEditCurPos_Z, m_dEditCurPos_LightZ, m_dEditCurPos_T, m_dEditCurPos_R, m_dEditCurPos_X, m_dEditCurPos_Y);

	CString strVisionModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	CString strMotionMovingPosition;
	strMotionMovingPosition.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition.ini", strOpticModelFolder, THEAPP.m_pCameraManager->GetVisionCamName() + 1);

	CString strSection, strKey;
	CIniFileCS INI_MotionMovingPosition(strMotionMovingPosition);

	strSection.Format("Moving Position %d", THEAPP.m_iCurStageIndex + 1);
	strKey.Format("Z-%d", iInspectImageIndex + 1);
	INI_MotionMovingPosition.Set_Double(strSection, strKey, m_dEditCurPos_Z);

	CString strMotionMovingPosition_Offset;
	strMotionMovingPosition_Offset.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition_Offset.ini", strOpticModelFolder, THEAPP.m_pCameraManager->GetVisionCamName() + 1);

	CIniFileCS INI_MotionMovingPosition_Offset(strMotionMovingPosition_Offset);

	strSection.Format("Moving Position %d", THEAPP.m_iCurStageIndex + 1);
	strKey.Format("LZ-%d", iInspectImageIndex);
	INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dEditCurPos_LightZ);
	strKey.Format("T-%d", iInspectImageIndex);
	INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dEditCurPos_T);
	strKey.Format("R-%d", iInspectImageIndex);
	INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dEditCurPos_R);
	strKey.Format("X-%d", iInspectImageIndex);
	INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dEditCurPos_X);
	strKey.Format("Y-%d", iInspectImageIndex);
	INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dEditCurPos_Y);
}

void CMotionControlDlg::OnBnClickedButtonSetCurrentZpositionToImage()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	UpdateData();

	int iInspectImageIndex = m_cbInspectImageNo.GetCurSel();

	THEAPP.m_pTabControlDlg->m_pJogSetDlg->SetMotionParameter(iInspectImageIndex, m_dEditCurPos_Z, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE);
	THEAPP.m_pTabControlDlg->m_pCameraLightManagerDlg->SetMotionParameter(iInspectImageIndex, m_dEditCurPos_Z, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE);

	CString strVisionModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	CString strMotionMovingPosition;
	strMotionMovingPosition.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition.ini", strOpticModelFolder, THEAPP.m_pCameraManager->GetVisionCamName() + 1);

	CIniFileCS INI_MotionMovingPosition(strMotionMovingPosition);
	CString strSection, strKey;
	strSection.Format("Moving Position %d", THEAPP.m_iCurStageIndex + 1);
	strKey.Format("Z-%d", iInspectImageIndex + 1);
	INI_MotionMovingPosition.Set_Double(strSection, strKey, m_dEditCurPos_Z);
}
