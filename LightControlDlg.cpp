// LightControlDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LightControlDlg.h"
#include "afxdialogex.h"
#include "IniFileCS.h"

#include "LightPageControlDlg.h"
#include "LightPwmControlDlg.h"

// CLightControlDlg 대화 상자입니다.
CLightControlDlg* CLightControlDlg::m_pInstance = NULL;

CLightControlDlg* CLightControlDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CLightControlDlg();
		if (!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_LIGHT_CONTROL_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CLightControlDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CLightControlDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CLightControlDlg, CDialog)

CLightControlDlg::CLightControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightControlDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT + 120 + VIEW1_DLG3_WIDTH - 550 + 10, VIEW1_DLG3_TOP + 50 + 50, VIEW1_DLG3_WIDTH - 630, VIEW1_DLG3_HEIGHT + 170);

	m_iTeachingGrab = -1;

	for (int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
		m_bCheckPage[i] = FALSE;

	m_dCurZPosition = 5.0;
	m_dCurLightZPosition = 0.0;
	m_dCurTPosition = 0.0;
	m_dCurRPosition = 0.0;
	m_dCurXPosition = 0.0;
	m_dCurYPosition = 0.0;

	m_bDFMManualGrabDone = TRUE;
}

CLightControlDlg::~CLightControlDlg()
{
}

void CLightControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_SET_MATCHING_PARAM, m_bnSetMatchingParam);
	DDX_Control(pDX, IDC_BUTTON_SET_FAI, m_bnSetFai);
	DDX_Control(pDX, IDC_BUTTON_CAMERA_LIGHT_MANAGER, m_bnCameraLightManager);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_LIGHTVALUE, m_bnChangeLight);
	DDX_Control(pDX, IDC_BUTTON_GRAB_SEQUENCE, m_bnGrabSequence);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_SPECULAR, m_bnChangeSpecular);
	DDX_Control(pDX, IDC_BUTTON_AUTO_FOCUS, m_bnAutoFocus);
	DDX_Control(pDX, IDC_BUTTON_MOTION_CONTROL, m_bnMotionControl);
	DDX_Control(pDX, IDC_BUTTON_MOVE_Z_POS, m_bnMoveZPos);
	DDX_Control(pDX, IDC_GRAB, m_bnImageGrab);
	DDX_Check(pDX, IDC_CHECK_PAGE_1, m_bCheckPage[0]);
	DDX_Check(pDX, IDC_CHECK_PAGE_2, m_bCheckPage[1]);
	DDX_Check(pDX, IDC_CHECK_PAGE_3, m_bCheckPage[2]);
	DDX_Check(pDX, IDC_CHECK_PAGE_4, m_bCheckPage[3]);
	DDX_Check(pDX, IDC_CHECK_PAGE_5, m_bCheckPage[4]);
	DDX_Check(pDX, IDC_CHECK_PAGE_6, m_bCheckPage[5]);
	DDX_Check(pDX, IDC_CHECK_PAGE_7, m_bCheckPage[6]);
	DDX_Check(pDX, IDC_CHECK_PAGE_8, m_bCheckPage[7]);
	DDX_Check(pDX, IDC_CHECK_PAGE_9, m_bCheckPage[8]);
	DDX_Check(pDX, IDC_CHECK_PAGE_10, m_bCheckPage[9]);
	DDX_Check(pDX, IDC_CHECK_PAGE_11, m_bCheckPage[10]);
	DDX_Check(pDX, IDC_CHECK_PAGE_12, m_bCheckPage[11]);
	DDX_Check(pDX, IDC_CHECK_PAGE_13, m_bCheckPage[12]);
	DDX_Check(pDX, IDC_CHECK_PAGE_14, m_bCheckPage[13]);
	DDX_Check(pDX, IDC_CHECK_PAGE_15, m_bCheckPage[14]);
	DDX_Check(pDX, IDC_CHECK_PAGE_16, m_bCheckPage[15]);
	DDX_Check(pDX, IDC_CHECK_PAGE_17, m_bCheckPage[16]);
	DDX_Check(pDX, IDC_CHECK_PAGE_18, m_bCheckPage[17]);
	DDX_Check(pDX, IDC_CHECK_PAGE_19, m_bCheckPage[18]);
	DDX_Check(pDX, IDC_CHECK_PAGE_20, m_bCheckPage[19]);
	DDX_Check(pDX, IDC_CHECK_PAGE_21, m_bCheckPage[20]);
	DDX_Check(pDX, IDC_CHECK_PAGE_22, m_bCheckPage[21]);
	DDX_Check(pDX, IDC_CHECK_PAGE_23, m_bCheckPage[22]);
	DDX_Check(pDX, IDC_CHECK_PAGE_24, m_bCheckPage[23]);
	DDX_Check(pDX, IDC_CHECK_PAGE_25, m_bCheckPage[24]);
	DDX_Check(pDX, IDC_CHECK_PAGE_26, m_bCheckPage[25]);
	DDX_Check(pDX, IDC_CHECK_PAGE_27, m_bCheckPage[26]);
	DDX_Check(pDX, IDC_CHECK_PAGE_28, m_bCheckPage[27]);
	DDX_Check(pDX, IDC_CHECK_PAGE_29, m_bCheckPage[28]);
	DDX_Check(pDX, IDC_CHECK_PAGE_30, m_bCheckPage[29]);
	DDX_Check(pDX, IDC_CHECK_PAGE_31, m_bCheckPage[30]);
	DDX_Check(pDX, IDC_CHECK_PAGE_32, m_bCheckPage[31]);
}


BEGIN_MESSAGE_MAP(CLightControlDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_MATCHING_PARAM, &CLightControlDlg::OnBnClickedButtonSetMatchingParam)
	ON_BN_CLICKED(IDC_BUTTON_SET_FAI, &CLightControlDlg::OnBnClickedButtonSetFai)
	ON_BN_CLICKED(IDC_BUTTON_CAMERA_LIGHT_MANAGER, &CLightControlDlg::OnBnClickedButtonCameraLightManager)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_LIGHTVALUE, &CLightControlDlg::OnBnClickedButtonChangeLightvalue)
	ON_BN_CLICKED(IDC_BUTTON_GRAB_SEQUENCE, &CLightControlDlg::OnBnClickedButtonGrabSequence)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_SPECULAR, &CLightControlDlg::OnBnClickedButtonChangeSpecular)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_FOCUS, &CLightControlDlg::OnBnClickedButtonAutoFocus)
	ON_BN_CLICKED(IDC_BUTTON_MOTION_CONTROL, &CLightControlDlg::OnBnClickedButtonMotionControl)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_Z_POS, &CLightControlDlg::OnBnClickedButtonMoveZPos)
	ON_BN_CLICKED(IDC_GRAB, &CLightControlDlg::OnBnClickedGrab)
	ON_BN_CLICKED(IDC_CHECK_PAGE_1, &CLightControlDlg::OnBnClickedCheckPage1)
	ON_BN_CLICKED(IDC_CHECK_PAGE_2, &CLightControlDlg::OnBnClickedCheckPage2)
	ON_BN_CLICKED(IDC_CHECK_PAGE_3, &CLightControlDlg::OnBnClickedCheckPage3)
	ON_BN_CLICKED(IDC_CHECK_PAGE_4, &CLightControlDlg::OnBnClickedCheckPage4)
	ON_BN_CLICKED(IDC_CHECK_PAGE_5, &CLightControlDlg::OnBnClickedCheckPage5)
	ON_BN_CLICKED(IDC_CHECK_PAGE_6, &CLightControlDlg::OnBnClickedCheckPage6)
	ON_BN_CLICKED(IDC_CHECK_PAGE_7, &CLightControlDlg::OnBnClickedCheckPage7)
	ON_BN_CLICKED(IDC_CHECK_PAGE_8, &CLightControlDlg::OnBnClickedCheckPage8)
	ON_BN_CLICKED(IDC_CHECK_PAGE_9, &CLightControlDlg::OnBnClickedCheckPage9)
	ON_BN_CLICKED(IDC_CHECK_PAGE_10, &CLightControlDlg::OnBnClickedCheckPage10)
	ON_BN_CLICKED(IDC_CHECK_PAGE_11, &CLightControlDlg::OnBnClickedCheckPage11)
	ON_BN_CLICKED(IDC_CHECK_PAGE_12, &CLightControlDlg::OnBnClickedCheckPage12)
	ON_BN_CLICKED(IDC_CHECK_PAGE_13, &CLightControlDlg::OnBnClickedCheckPage13)
	ON_BN_CLICKED(IDC_CHECK_PAGE_14, &CLightControlDlg::OnBnClickedCheckPage14)
	ON_BN_CLICKED(IDC_CHECK_PAGE_15, &CLightControlDlg::OnBnClickedCheckPage15)
	ON_BN_CLICKED(IDC_CHECK_PAGE_16, &CLightControlDlg::OnBnClickedCheckPage16)
	ON_BN_CLICKED(IDC_CHECK_PAGE_17, &CLightControlDlg::OnBnClickedCheckPage17)
	ON_BN_CLICKED(IDC_CHECK_PAGE_18, &CLightControlDlg::OnBnClickedCheckPage18)
	ON_BN_CLICKED(IDC_CHECK_PAGE_19, &CLightControlDlg::OnBnClickedCheckPage19)
	ON_BN_CLICKED(IDC_CHECK_PAGE_20, &CLightControlDlg::OnBnClickedCheckPage20)
	ON_BN_CLICKED(IDC_CHECK_PAGE_21, &CLightControlDlg::OnBnClickedCheckPage21)
	ON_BN_CLICKED(IDC_CHECK_PAGE_22, &CLightControlDlg::OnBnClickedCheckPage22)
	ON_BN_CLICKED(IDC_CHECK_PAGE_23, &CLightControlDlg::OnBnClickedCheckPage23)
	ON_BN_CLICKED(IDC_CHECK_PAGE_24, &CLightControlDlg::OnBnClickedCheckPage24)
	ON_BN_CLICKED(IDC_CHECK_PAGE_25, &CLightControlDlg::OnBnClickedCheckPage25)
	ON_BN_CLICKED(IDC_CHECK_PAGE_26, &CLightControlDlg::OnBnClickedCheckPage26)
	ON_BN_CLICKED(IDC_CHECK_PAGE_27, &CLightControlDlg::OnBnClickedCheckPage27)
	ON_BN_CLICKED(IDC_CHECK_PAGE_28, &CLightControlDlg::OnBnClickedCheckPage28)
	ON_BN_CLICKED(IDC_CHECK_PAGE_29, &CLightControlDlg::OnBnClickedCheckPage29)
	ON_BN_CLICKED(IDC_CHECK_PAGE_30, &CLightControlDlg::OnBnClickedCheckPage30)
	ON_BN_CLICKED(IDC_CHECK_PAGE_31, &CLightControlDlg::OnBnClickedCheckPage31)
	ON_BN_CLICKED(IDC_CHECK_PAGE_32, &CLightControlDlg::OnBnClickedCheckPage32)

END_MESSAGE_MAP()


// CLightControlDlg 메시지 처리기입니다.
BOOL CLightControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bnSetMatchingParam.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnSetFai.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnCameraLightManager.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x9C, 0x9A, 0xB6), 0, 0);
	m_bnChangeLight.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnGrabSequence.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnChangeSpecular.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnAutoFocus.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x9C, 0x9A, 0xB6), 0, 0);
	m_bnMotionControl.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnMoveZPos.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnImageGrab.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);

	ChangeLanguage();

	SetDlgStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLightControlDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		SetDlgItemText(IDC_GROUPBOX_LIGHTCONTROLDLG_1, _T("조명 선택"));
		SetDlgItemText(IDC_CHECK_PAGE_1, _T("페이지 1"));
		SetDlgItemText(IDC_CHECK_PAGE_2, _T("페이지 2"));
		SetDlgItemText(IDC_CHECK_PAGE_3, _T("페이지 3"));
		SetDlgItemText(IDC_CHECK_PAGE_4, _T("페이지 4"));
		SetDlgItemText(IDC_CHECK_PAGE_5, _T("페이지 5"));
		SetDlgItemText(IDC_CHECK_PAGE_6, _T("페이지 6"));
		SetDlgItemText(IDC_CHECK_PAGE_7, _T("페이지 7"));
		SetDlgItemText(IDC_CHECK_PAGE_8, _T("페이지 8"));
		SetDlgItemText(IDC_CHECK_PAGE_9, _T("페이지 9"));
		SetDlgItemText(IDC_CHECK_PAGE_10, _T("페이지 10"));
		SetDlgItemText(IDC_CHECK_PAGE_11, _T("페이지 11"));
		SetDlgItemText(IDC_CHECK_PAGE_12, _T("페이지 12"));
		SetDlgItemText(IDC_CHECK_PAGE_13, _T("페이지 13"));
		SetDlgItemText(IDC_CHECK_PAGE_14, _T("페이지 14"));
		SetDlgItemText(IDC_CHECK_PAGE_15, _T("페이지 15"));
		SetDlgItemText(IDC_CHECK_PAGE_16, _T("페이지 16"));
		SetDlgItemText(IDC_CHECK_PAGE_17, _T("페이지 17"));
		SetDlgItemText(IDC_CHECK_PAGE_18, _T("페이지 18"));
		SetDlgItemText(IDC_CHECK_PAGE_19, _T("페이지 19"));
		SetDlgItemText(IDC_CHECK_PAGE_20, _T("페이지 20"));
		SetDlgItemText(IDC_CHECK_PAGE_21, _T("페이지 21"));
		SetDlgItemText(IDC_CHECK_PAGE_22, _T("페이지 22"));
		SetDlgItemText(IDC_CHECK_PAGE_23, _T("페이지 23"));
		SetDlgItemText(IDC_CHECK_PAGE_24, _T("페이지 24"));
		SetDlgItemText(IDC_CHECK_PAGE_25, _T("페이지 25"));
		SetDlgItemText(IDC_CHECK_PAGE_26, _T("페이지 26"));
		SetDlgItemText(IDC_CHECK_PAGE_27, _T("페이지 27"));
		SetDlgItemText(IDC_CHECK_PAGE_28, _T("페이지 28"));
		SetDlgItemText(IDC_CHECK_PAGE_29, _T("페이지 29"));
		SetDlgItemText(IDC_CHECK_PAGE_30, _T("페이지 30"));
		SetDlgItemText(IDC_CHECK_PAGE_31, _T("페이지 31"));
		SetDlgItemText(IDC_CHECK_PAGE_32, _T("페이지 32"));
		SetDlgItemText(IDC_BUTTON_SET_MATCHING_PARAM, _T("영상 매칭 파라미터 설정"));
		SetDlgItemText(IDC_BUTTON_SET_FAI, _T("FAI 검사 스펙 설정"));
		SetDlgItemText(IDC_BUTTON_CAMERA_LIGHT_MANAGER, _T("카메라, 조명 설정"));
		SetDlgItemText(IDC_BUTTON_CHANGE_LIGHTVALUE, _T("조명 페이지 설정"));
		SetDlgItemText(IDC_BUTTON_GRAB_SEQUENCE, _T("그랩 시퀀스 설정"));
		SetDlgItemText(IDC_BUTTON_CHANGE_SPECULAR, _T("4D 비전 파라미터 설정"));
		SetDlgItemText(IDC_BUTTON_AUTO_FOCUS, _T("오토 포커스 설정"));
		SetDlgItemText(IDC_BUTTON_MOTION_CONTROL, _T("모션 이동 제어 화면"));
		SetDlgItemText(IDC_BUTTON_MOVE_Z_POS, _T("그랩 위치 이동"));
		SetDlgItemText(IDC_GRAB, _T("영상 그랩"));
	}
	else
	{
		SetDlgItemText(IDC_GROUPBOX_LIGHTCONTROLDLG_1, _T("Select page"));
		SetDlgItemText(IDC_CHECK_PAGE_1, _T("Page 1"));
		SetDlgItemText(IDC_CHECK_PAGE_2, _T("Page 2"));
		SetDlgItemText(IDC_CHECK_PAGE_3, _T("Page 3"));
		SetDlgItemText(IDC_CHECK_PAGE_4, _T("Page 4"));
		SetDlgItemText(IDC_CHECK_PAGE_5, _T("Page 5"));
		SetDlgItemText(IDC_CHECK_PAGE_6, _T("Page 6"));
		SetDlgItemText(IDC_CHECK_PAGE_7, _T("Page 7"));
		SetDlgItemText(IDC_CHECK_PAGE_8, _T("Page 8"));
		SetDlgItemText(IDC_CHECK_PAGE_9, _T("Page 9"));
		SetDlgItemText(IDC_CHECK_PAGE_10, _T("Page 10"));
		SetDlgItemText(IDC_CHECK_PAGE_11, _T("Page 11"));
		SetDlgItemText(IDC_CHECK_PAGE_12, _T("Page 12"));
		SetDlgItemText(IDC_CHECK_PAGE_13, _T("Page 13"));
		SetDlgItemText(IDC_CHECK_PAGE_14, _T("Page 14"));
		SetDlgItemText(IDC_CHECK_PAGE_15, _T("Page 15"));
		SetDlgItemText(IDC_CHECK_PAGE_16, _T("Page 16"));
		SetDlgItemText(IDC_CHECK_PAGE_17, _T("Page 17"));
		SetDlgItemText(IDC_CHECK_PAGE_18, _T("Page 18"));
		SetDlgItemText(IDC_CHECK_PAGE_19, _T("Page 19"));
		SetDlgItemText(IDC_CHECK_PAGE_20, _T("Page 20"));
		SetDlgItemText(IDC_CHECK_PAGE_21, _T("Page 21"));
		SetDlgItemText(IDC_CHECK_PAGE_22, _T("Page 22"));
		SetDlgItemText(IDC_CHECK_PAGE_23, _T("Page 23"));
		SetDlgItemText(IDC_CHECK_PAGE_24, _T("Page 24"));
		SetDlgItemText(IDC_CHECK_PAGE_25, _T("Page 25"));
		SetDlgItemText(IDC_CHECK_PAGE_26, _T("Page 26"));
		SetDlgItemText(IDC_CHECK_PAGE_27, _T("Page 27"));
		SetDlgItemText(IDC_CHECK_PAGE_28, _T("Page 28"));
		SetDlgItemText(IDC_CHECK_PAGE_29, _T("Page 29"));
		SetDlgItemText(IDC_CHECK_PAGE_30, _T("Page 30"));
		SetDlgItemText(IDC_CHECK_PAGE_31, _T("Page 31"));
		SetDlgItemText(IDC_CHECK_PAGE_32, _T("Page 32"));
		SetDlgItemText(IDC_BUTTON_SET_MATCHING_PARAM, _T("Set image match parameter"));
		SetDlgItemText(IDC_BUTTON_SET_FAI, _T("Set the FAI measure spec"));
		SetDlgItemText(IDC_BUTTON_CAMERA_LIGHT_MANAGER, _T("Set the Camera and light"));
		SetDlgItemText(IDC_BUTTON_CHANGE_LIGHTVALUE, _T("Set the light page"));
		SetDlgItemText(IDC_BUTTON_GRAB_SEQUENCE, _T("Set the grab sequence"));
		SetDlgItemText(IDC_BUTTON_CHANGE_SPECULAR, _T("Set 4D vision parameter"));
		SetDlgItemText(IDC_BUTTON_AUTO_FOCUS, _T("Set the auto focus"));
		SetDlgItemText(IDC_BUTTON_MOTION_CONTROL, _T("Motion move control"));
		SetDlgItemText(IDC_BUTTON_MOVE_Z_POS, _T("Grab position move"));
		SetDlgItemText(IDC_GRAB, _T("Image grab"));
	}
}

void CLightControlDlg::SetDlgStatus()
{
#if defined(SINGLE_LENS) || defined(ASSY_LENS)
	GetDlgItem(IDC_BUTTON_SET_FAI)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CAMERA_LIGHT_MANAGER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GRAB_SEQUENCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CHANGE_SPECULAR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_AUTO_FOCUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MOTION_CONTROL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MOVE_Z_POS)->EnableWindow(FALSE);
#endif
}

BOOL CLightControlDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

UINT LightControlAMoveRequest_Thread(LPVOID lp)
{
	CString strLog, strMessageBox;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[THEAPP.m_pCameraManager->GetVisionCamName()];

	CLightControlDlg* pLightControlDlg = (CLightControlDlg*)lp;

	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return 0;

	int iJigNo;
	if (THEAPP.m_pCameraManager->GetVisionCamName() == VISION_NUMBER_1 || THEAPP.m_pCameraManager->GetVisionCamName() == VISION_NUMBER_2)
		iJigNo = (THEAPP.Struct_PreferenceStruct.m_iPCType * 2) + 1;
	else
		iJigNo = (THEAPP.Struct_PreferenceStruct.m_iPCType * 2) + 2;

	THEAPP.m_pHandlerService->m_bMotionMoveComplete[THEAPP.m_pCameraManager->GetVisionCamName()] = FALSE;

	THEAPP.m_pHandlerService->Set_AMoveRequest(THEAPP.m_pCameraManager->GetVisionCamName(), THEAPP.m_iCurStageIndex, iJigNo, pLightControlDlg->m_dCurZPosition, pLightControlDlg->m_dCurLightZPosition, pLightControlDlg->m_dCurXPosition, pLightControlDlg->m_dCurYPosition, pLightControlDlg->m_dCurTPosition, pLightControlDlg->m_dCurRPosition);

	DWORD dwMoveStart = 0, dwMoveEnd = 0;

	dwMoveStart = GetTickCount();

	while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[THEAPP.m_pCameraManager->GetVisionCamName()]) // 무브 컴플리트 기다림
	{
		dwMoveEnd = GetTickCount();

		if ((dwMoveEnd - dwMoveStart) > 5000)
		{
			strLog.Format("%s/ Manual move complete timeout", sVisionCamType_Short);
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("핸들러로부터 모션 완료 신호가 없습니다. 핸들러 상태 확인해 주세요.");
			else
				strMessageBox.Format("No move complete signal recieved from the handler. Check the hanlder SW status.");
			AfxMessageBox(strMessageBox, MB_SYSTEMMODAL | MB_ICONERROR);
			return 0;
		}

		Sleep(1);
	}

	THEAPP.m_pHandlerService->Set_PositionRequest(THEAPP.m_pCameraManager->GetVisionCamName(), THEAPP.m_iCurStageIndex, iJigNo);

	return 1;
}

UINT SequenceGrabThread(LPVOID lp)
{
	CString strLog;

	CLightControlDlg* pLightControlDlg = (CLightControlDlg*)lp;

	int iImageIndex = pLightControlDlg->m_iTeachingGrab - 1;
	if (iImageIndex < 0)
		iImageIndex = 0;

	int iStartGrabBufferIndex = iImageIndex;

	BOOL bGrabSuccess = FALSE;

	THEAPP.m_pCameraManager->SetTeachImageSave(TRUE);

	auto log_time_start = std::chrono::high_resolution_clock::now();

	if (THEAPP.Struct_PreferenceStruct.m_iTeachingCameraLightType == TEACHING_CAMERA_LIGHT_PAGE)
	{
		int iAddrSeq[LIGHTCTRL_PAGE_COUNT];
		for (int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
			iAddrSeq[i] = THEAPP.m_pModelDataManager->m_iLightPageIdx[iImageIndex];
		THEAPP.m_pModelDataManager->m_PageControlData.SetGrabSequence(THEAPP.m_iCurVisionCamType, LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, iAddrSeq, 1);

		bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iStartGrabBufferIndex, LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, 1);

		for (int i = iStartGrabBufferIndex; i < (iStartGrabBufferIndex + 1); i++)
		{
			if (THEAPP.m_pCameraManager->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pCameraManager->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
		}
	}
	else
	{
		iStartGrabBufferIndex = THEAPP.m_pModelDataManager->GetSequenceStartGrabBufferIndex(THEAPP.m_iCurStageIndex, iImageIndex);

		bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iStartGrabBufferIndex, THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex], THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]);

		for (int i = iStartGrabBufferIndex; i < (iStartGrabBufferIndex + THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]); i++)
		{
			if (THEAPP.m_pCameraManager->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pCameraManager->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
		}
	}

	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("SEQ Grab, Time(ms): %d", log_time_ms);
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	if (bGrabSuccess)
	{
		int iGrabCircularIdx = THEAPP.m_pCameraManager->GetGrabCircularIndex();

		if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)								// 티칭화면이면 ....
		{
			if (THEAPP.m_pCameraManager->GetCropFovUse())
			{
				CropPart(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][0], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]), THEAPP.m_pCameraManager->GetCropFovLTY(), THEAPP.m_pCameraManager->GetCropFovLTX(), THEAPP.m_pCameraManager->GetCropFovSizeX(), THEAPP.m_pCameraManager->GetCropFovSizeY());
				CropPart(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][1], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]), THEAPP.m_pCameraManager->GetCropFovLTY(), THEAPP.m_pCameraManager->GetCropFovLTX(), THEAPP.m_pCameraManager->GetCropFovSizeX(), THEAPP.m_pCameraManager->GetCropFovSizeY());
				CropPart(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][2], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]), THEAPP.m_pCameraManager->GetCropFovLTY(), THEAPP.m_pCameraManager->GetCropFovLTX(), THEAPP.m_pCameraManager->GetCropFovSizeX(), THEAPP.m_pCameraManager->GetCropFovSizeY());

				Compose3(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
				TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B],
					&(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), "hsi");
				Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));
			}
			else
			{
				CopyImage(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][0], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
				CopyImage(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][1], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
				CopyImage(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][2], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));

				Compose3(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
				TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B],
					&(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), "hsi");
				Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));
			}

			for (int i = 0; i < CHANNEL_NUM; i++)
			{
				CopyImage(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]));
			}

			THEAPP.m_pInspectAdminViewDlg->UpdateView();

			if (THEAPP.m_pCameraManager->GetTeachImageSave())
				THEAPP.m_pInspectAdminViewDlg->TeachingImageGrabSave();								// 티칭 이미지 저장  => 티칭화면에 있어야만 저장됨.
		}
	}

	pLightControlDlg->m_bDFMManualGrabDone = TRUE;

	return 1;
}

UINT SingleGrabThread(LPVOID lp)
{
	CString strLog;

	CLightControlDlg* pLightControlDlg = (CLightControlDlg*)lp;

	int iImageIndex = pLightControlDlg->m_iTeachingGrab - 1;
	if (iImageIndex < 0)
		iImageIndex = 0;

	int iStartGrabBufferIndex = iImageIndex;

	BOOL bGrabSuccess = FALSE;

	THEAPP.m_pCameraManager->SetTeachImageSave(TRUE);

	auto log_time_start = std::chrono::high_resolution_clock::now();

	bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab_NoSeq(iStartGrabBufferIndex, THEAPP.m_pModelDataManager->m_iLightPageIdx[iImageIndex]);
	   
	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("SEQ Grab, Time(ms): %d", log_time_ms);
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	if (bGrabSuccess)
	{
		int iGrabCircularIdx = THEAPP.m_pCameraManager->GetGrabCircularIndex();

		if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)								// 티칭화면이면 ....
		{
			if (THEAPP.m_pCameraManager->GetCropFovUse())
			{
				CropPart(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][0], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), THEAPP.m_pCameraManager->GetCropFovLTY(), THEAPP.m_pCameraManager->GetCropFovLTX(), THEAPP.m_pCameraManager->GetCropFovSizeX(), THEAPP.m_pCameraManager->GetCropFovSizeY());
			}
			else
			{
				CopyImage(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][0], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));
			}

			CopyImage(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]));

			THEAPP.m_pInspectAdminViewDlg->UpdateView();

			if (THEAPP.m_pCameraManager->GetTeachImageSave())
				THEAPP.m_pInspectAdminViewDlg->TeachingImageGrabSave(FALSE);								// 티칭 이미지 저장  => 티칭화면에 있어야만 저장됨.
		}
	}

	pLightControlDlg->m_bDFMManualGrabDone = TRUE;

	return 1;
}

UINT DFMGrabThread(LPVOID lp)
{
	CString strLog;

	CLightControlDlg* pLightControlDlg = (CLightControlDlg*)lp;

	int iImageIndex = pLightControlDlg->m_iTeachingGrab - 1;
	if (iImageIndex < 0)
		iImageIndex = 0;

	BOOL bGrabSuccess = FALSE;

	auto log_time_start = std::chrono::high_resolution_clock::now();

	int iStartGrabBufferIndex = 0;

	bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iStartGrabBufferIndex, THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex], THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]);

	for (int i = iStartGrabBufferIndex; i < (iStartGrabBufferIndex + THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]); i++)
	{
		if (THEAPP.m_pCameraManager->m_sBayerType == "GB")
			MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
		else if (THEAPP.m_pCameraManager->m_sBayerType == "BG")
			MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_BG);
		else
			MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
	}

	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("DFM Grab, Time(ms): %d", log_time_ms);
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	int iWhichSpecularImage, iGrabCycleIndex;

	iGrabCycleIndex = 0;

	if (bGrabSuccess)
	{
		iWhichSpecularImage = THEAPP.m_pModelDataManager->GetWhichSpecularImage(iImageIndex, &iGrabCycleIndex);

		log_time_start = std::chrono::high_resolution_clock::now();
		THEAPP.m_pCameraManager->DFMProc(THEAPP.m_iCurTeachVision, 0, THEAPP.m_pCameraManager->GetVisionCamName());			// Grab Buffer 0번에다 RawImage Grab
		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("DFM Proc, Time(ms): %d", log_time_ms);
		THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));

		THEAPP.m_pCameraManager->DFM_ReadConvertImage_TeachMode(iWhichSpecularImage, TRUE);

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}

	pLightControlDlg->m_bDFMManualGrabDone = TRUE;

	return 1;
}

UINT DFMNormalGrabThread(LPVOID lp)
{
	CString strLog;

	CLightControlDlg* pLightControlDlg = (CLightControlDlg*)lp;

	int iImageIndex = pLightControlDlg->m_iTeachingGrab - 1;
	if (iImageIndex < 0)
		iImageIndex = 0;

	THEAPP.m_pCameraManager->SetTeachImageSave(TRUE);

	int iGrabSuccess = DFM_RET_NO_ERROR;

	auto log_time_start = std::chrono::high_resolution_clock::now();
	iGrabSuccess = THEAPP.m_pCameraManager->DFMStartNormalGrab(iImageIndex, iImageIndex, TRUE);
	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("DFM Normal Grab, Time(ms): %d", log_time_ms);
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	if (iGrabSuccess == DFM_RET_NO_ERROR)
	{
		log_time_start = std::chrono::high_resolution_clock::now();
		if (THEAPP.m_pCameraManager->m_sBayerType == "GB")
			MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][iImageIndex], THEAPP.m_pCameraManager->MilColorImageBuf[0][iImageIndex], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
		else if (THEAPP.m_pCameraManager->m_sBayerType == "BG")
			MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][iImageIndex], THEAPP.m_pCameraManager->MilColorImageBuf[0][iImageIndex], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_BG);
		else
			MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][iImageIndex], THEAPP.m_pCameraManager->MilColorImageBuf[0][iImageIndex], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);

		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("DFM Normal conversion, Time(ms): %d", log_time_ms);
		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

		THEAPP.m_pCameraManager->DFM_ReadPageImage_TeachMode(iImageIndex, TRUE);

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}

	pLightControlDlg->m_bDFMManualGrabDone = TRUE;

	return 1;
}

UINT DiffusedVisionGrabThread(LPVOID lp)
{
	CString strLog;

	CLightControlDlg* pLightControlDlg = (CLightControlDlg*)lp;

	int iImageIndex = pLightControlDlg->m_iTeachingGrab - 1;
	if (iImageIndex < 0)
		iImageIndex = 0;

	BOOL bGrabSuccess = FALSE;

	auto log_time_start = std::chrono::high_resolution_clock::now();

	int iStartGrabBufferIndex = 0;

	bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iStartGrabBufferIndex, THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex], THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]);

	for (int i = iStartGrabBufferIndex; i < (iStartGrabBufferIndex + THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]); i++)
	{
		if (THEAPP.m_pCameraManager->m_sBayerType == "GB")
			MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
		else if (THEAPP.m_pCameraManager->m_sBayerType == "BG")
			MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_BG);
		else
			MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
	}

	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("Diffused Grab, Time(ms): %d", log_time_ms);
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	int iWhichDiffusedImage, iGrabCycleIndex;

	iGrabCycleIndex = 0;

	if (bGrabSuccess)
	{
		iWhichDiffusedImage = THEAPP.m_pModelDataManager->GetWhichDiffusedImage(iImageIndex, &iGrabCycleIndex);

		log_time_start = std::chrono::high_resolution_clock::now();
		THEAPP.m_pCameraManager->DiffusedProc(THEAPP.m_iCurTeachVision, 0, THEAPP.m_pCameraManager->GetVisionCamName());			// Grab Buffer 0번에다 RawImage Grab
		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("Diffused Proc, Time(ms): %d", log_time_ms);
		THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));

		THEAPP.m_pCameraManager->Diffused_ReadConvertImage_TeachMode(iWhichDiffusedImage, TRUE);

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}

	pLightControlDlg->m_bDFMManualGrabDone = TRUE;

	return 1;
}

void CLightControlDlg::OnBnClickedButtonSetMatchingParam()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	int iCurImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;

	CGlobalMatchingDlg dlg;

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iCurImageIndex]))
		CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iCurImageIndex], &(dlg.m_HOrgTeachingImage));

	dlg.DoModal();
}


// TODO : Dlg 관리클래스 필요할듯
#include "TeachFaiChsKSDlg.h"
#include "TeachFaiChsWZDlg.h"
#include "TeachFaiAkcDlg.h"
#include "TeachFaiBoiDlg.h"
#include "TeachFaiActDlg.h"
#include "TeachFaiAtwDlg.h"
#include "TeachFaiBosDlg.h"
#include "TeachFaiRenoDlg.h"
#include "TeachFaiKriosDlg.h"

void CLightControlDlg::OnBnClickedButtonSetFai()
{
	CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
	if (strModelType == _T("CHS-W") || strModelType == _T("CHS-Z"))
	{
		CTeachFaiChsWZDlg dlg;
		dlg.DoModal();
	}
	else if (strModelType == _T("CHS-K") || strModelType == _T("CHS-S") || strModelType == _T("CHS-T") || strModelType == _T("CHS-V"))
	{
		CTeachFaiChsKSDlg dlg;
		dlg.DoModal();
	}
	else if (strModelType == _T("AKC"))
	{
		CTeachFaiAkcDlg dlg;
		dlg.DoModal();
	}
	else if (strModelType == _T("BOI"))
	{
		CTeachFaiBoiDlg dlg;
		dlg.DoModal();
	}
	else if (strModelType == _T("ACT"))
	{
		CTeachFaiActDlg dlg;
		dlg.DoModal();
	}
	else if (strModelType == _T("ATW-D"))
	{
		CTeachFaiAtwDlg dlg;
		dlg.DoModal();
	}
	else if (strModelType == _T("BOS"))
	{
		CTeachFaiBosDlg dlg;
		dlg.DoModal();
	}
	else if (strModelType == _T("RENO"))
	{
		CTeachFaiRenoDlg dlg;
		dlg.DoModal();
	}
	else if (strModelType == _T("KRIOS"))
	{
		CTeachFaiKriosDlg dlg;
		dlg.DoModal();
	}
}

void CLightControlDlg::UpdateLightValue(int iTabIdx)
{
	int iImageIdx;

	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_pCameraManager->GetVisionCamName()])
		iImageIdx = 0;
	else
	{
		if (iTabIdx == 0)
			iImageIdx = 0;
		else
			iImageIdx = iTabIdx - 1;
	}

	int iLightPageIndex = THEAPP.m_pModelDataManager->m_iLightPageIdx[iImageIdx];

	for (int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
		m_bCheckPage[i] = FALSE;

	m_bCheckPage[iLightPageIndex] = TRUE;

	UpdateData(FALSE);
}

void CLightControlDlg::OnBnClickedButtonCameraLightManager()
{
	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_pCameraManager->GetVisionCamName()])
	{
		CLightPwmControlDlg dlg;
		dlg.DoModal();
	}
	else
		THEAPP.m_pTabControlDlg->ShowCameraLightManagerDlg();
}

#include "LightPageControlDlg.h"
void CLightControlDlg::OnBnClickedButtonChangeLightvalue()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_pCameraManager->GetVisionCamName()])
	{
		CLightPwmControlDlg dlg;
		dlg.DoModal();
	}
	else
	{
		CLightPageControlDlg dlg;
		dlg.DoModal();
	}
}

#include "GrabSequenceDlg.h"
void CLightControlDlg::OnBnClickedButtonGrabSequence()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_pCameraManager->GetVisionCamName()])
	{
		CLightPwmControlDlg dlg;
		dlg.DoModal();
	}
	else
	{
		CGrabSequenceDlg dlg;
		dlg.DoModal();
	}
}

#include "SPVImageParamDlg.h"
void CLightControlDlg::OnBnClickedButtonChangeSpecular()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_pCameraManager->GetVisionCamName()])
	{
		CLightPwmControlDlg dlg;
		dlg.DoModal();
	}
	else
	{
		CSPVImageParamDlg dlg;
		dlg.DoModal();
	}
}

void CLightControlDlg::OnBnClickedButtonAutoFocus()
{
	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_pCameraManager->GetVisionCamName()])
	{
		CLightPwmControlDlg dlg;
		dlg.DoModal();
	}
	else
		THEAPP.m_pTabControlDlg->ShowAutoFocusDlg();
}

void CLightControlDlg::OnBnClickedButtonMotionControl()
{
	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_pCameraManager->GetVisionCamName()])
		return;

	THEAPP.m_pTabControlDlg->ShowMotionControlDlg();
}

void CLightControlDlg::OnBnClickedButtonMoveZPos()
{
#ifdef INLINE_MODE
	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_pCameraManager->GetVisionCamName()])
		return;

	THEAPP.m_pTabControlDlg->m_pJogSetDlg->UpdateData(TRUE);

	if (THEAPP.m_pTabControlDlg->m_iCurrentTab > THEAPP.m_pModelDataManager->m_iNoUsedImageGrab)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("셋팅된 영상이 아닙니다. 포지션 셋팅 후 진행해주세요.");
		else
			strMessageBox.Format("The current image tab has not been position set. Proceed after completing the position setup.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

		return;
	}

	m_dCurZPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageZPos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	m_dCurLightZPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageLightZPosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageLightZPos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	m_dCurTPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageTiltPosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageTiltPos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	m_dCurRPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageRotatePosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageRotatePos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	m_dCurXPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageStageXPosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageStageXPos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	m_dCurYPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageStageYPosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageStageYPos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];

	AfxBeginThread(LightControlAMoveRequest_Thread, this);
#endif
}

void CLightControlDlg::OnBnClickedGrab()
{
	if (THEAPP.m_pModelDataManager->m_bUseSpecularImage[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1])
		GrabDeflectometry();
	else if (THEAPP.m_pModelDataManager->m_bUseDiffusedImage[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1])
		GrabDiffusedVision();
	else
	{
		if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_pCameraManager->GetVisionCamName()])
			GrabCamera_SwTrg();
		else
			GrabSequence();
	}
}

void CLightControlDlg::PageChecked(int iPageNumber)
{
	for (int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
		m_bCheckPage[i] = FALSE;

	m_bCheckPage[iPageNumber - 1] = TRUE;

	UpdateData(FALSE);

	THEAPP.m_pModelDataManager->m_iLightPageIdx[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] = iPageNumber - 1;
}

void CLightControlDlg::OnBnClickedCheckPage1()
{
	PageChecked(1);
}

void CLightControlDlg::OnBnClickedCheckPage2()
{
	PageChecked(2);
}


void CLightControlDlg::OnBnClickedCheckPage3()
{
	PageChecked(3);
}

void CLightControlDlg::OnBnClickedCheckPage4()
{
	PageChecked(4);
}


void CLightControlDlg::OnBnClickedCheckPage5()
{
	PageChecked(5);
}


void CLightControlDlg::OnBnClickedCheckPage6()
{
	PageChecked(6);
}

void CLightControlDlg::OnBnClickedCheckPage7()
{
	PageChecked(7);
}

void CLightControlDlg::OnBnClickedCheckPage8()
{
	PageChecked(8);
}

void CLightControlDlg::OnBnClickedCheckPage9()
{
	PageChecked(9);
}

void CLightControlDlg::OnBnClickedCheckPage10()
{
	PageChecked(10);
}

void CLightControlDlg::OnBnClickedCheckPage11()
{
	PageChecked(11);
}

void CLightControlDlg::OnBnClickedCheckPage12()
{
	PageChecked(12);
}

void CLightControlDlg::OnBnClickedCheckPage13()
{
	PageChecked(13);
}

void CLightControlDlg::OnBnClickedCheckPage14()
{
	PageChecked(14);
}

void CLightControlDlg::OnBnClickedCheckPage15()
{
	PageChecked(15);
}

void CLightControlDlg::OnBnClickedCheckPage16()
{
	PageChecked(16);
}

void CLightControlDlg::OnBnClickedCheckPage17()
{
	PageChecked(17);
}

void CLightControlDlg::OnBnClickedCheckPage18()
{
	PageChecked(18);
}

void CLightControlDlg::OnBnClickedCheckPage19()
{
	PageChecked(19);
}

void CLightControlDlg::OnBnClickedCheckPage20()
{
	PageChecked(20);
}

void CLightControlDlg::OnBnClickedCheckPage21()
{
	PageChecked(21);
}

void CLightControlDlg::OnBnClickedCheckPage22()
{
	PageChecked(22);
}


void CLightControlDlg::OnBnClickedCheckPage23()
{
	PageChecked(23);
}

void CLightControlDlg::OnBnClickedCheckPage24()
{
	PageChecked(24);
}


void CLightControlDlg::OnBnClickedCheckPage25()
{
	PageChecked(25);
}


void CLightControlDlg::OnBnClickedCheckPage26()
{
	PageChecked(26);
}

void CLightControlDlg::OnBnClickedCheckPage27()
{
	PageChecked(27);
}

void CLightControlDlg::OnBnClickedCheckPage28()
{
	PageChecked(28);
}

void CLightControlDlg::OnBnClickedCheckPage29()
{
	PageChecked(29);
}

void CLightControlDlg::OnBnClickedCheckPage30()
{
	PageChecked(30);
}

void CLightControlDlg::OnBnClickedCheckPage31()
{
	PageChecked(31);
}

void CLightControlDlg::OnBnClickedCheckPage32()
{
	PageChecked(32);
}

void CLightControlDlg::GrabCamera_SwTrg()
{
#ifdef INLINE_MODE
	THEAPP.m_pInspectAdminViewDlg->StopLive();
	Sleep(100);

	DWORD dwGrabStart = 0, dwGrabEnd = 0;
	DWORD dwGrabElapsedTime = 0;

	BOOL bGrabSuccess = FALSE;

	if (THEAPP.m_pTabControlDlg->m_iCurrentTab >= 1 && THEAPP.m_pTabControlDlg->m_iCurrentTab <= MAX_IMAGE_TAB)
	{
		m_iTeachingGrab = THEAPP.m_pTabControlDlg->m_iCurrentTab;

		THEAPP.m_pCameraManager->SetTeachImageSave(TRUE);

		int iVisionCamIdx = THEAPP.m_pCameraManager->GetVisionCamName();

		THEAPP.m_pLightPwmManager[iVisionCamIdx]->LightOn();

		bGrabSuccess = THEAPP.m_pCameraManager->GrabEthernetCamSWTrg(0);

		if (bGrabSuccess)
		{
			for (int i = 0; i < CHANNEL_NUM; i++)
			{
				CopyImage(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]));
			}
		}

		THEAPP.m_pLightPwmManager[iVisionCamIdx]->LightOff();
	}

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
#endif
}

void CLightControlDlg::GrabSequence()
{
#ifdef INLINE_MODE
	if (m_bDFMManualGrabDone == FALSE)
		return;

	THEAPP.m_pInspectAdminViewDlg->StopLive();
	Sleep(100);

	m_iTeachingGrab = THEAPP.m_pTabControlDlg->m_iCurrentTab;
	if (m_iTeachingGrab <= 0)
	{
		m_bDFMManualGrabDone = TRUE;
		return;
	}

	m_bDFMManualGrabDone = FALSE;

#if defined(SINGLE_LENS) || defined(ASSY_LENS)
	AfxBeginThread(SingleGrabThread, this);
#else
	AfxBeginThread(SequenceGrabThread, this);
#endif

#endif
}

void CLightControlDlg::GrabDeflectometry()
{
#ifdef INLINE_MODE

	if (m_bDFMManualGrabDone == FALSE)
		return;

	THEAPP.m_pInspectAdminViewDlg->StopLive();
	Sleep(100);

	m_iTeachingGrab = THEAPP.m_pTabControlDlg->m_iCurrentTab;
	if (m_iTeachingGrab <= 0)
	{
		m_bDFMManualGrabDone = TRUE;
		return;
	}

	m_bDFMManualGrabDone = FALSE;

	AfxBeginThread(DFMGrabThread, this);

#endif
}

void CLightControlDlg::GrabDiffusedVision()
{
#ifdef INLINE_MODE

	if (m_bDFMManualGrabDone == FALSE)
		return;

	THEAPP.m_pInspectAdminViewDlg->StopLive();
	Sleep(100);

	m_iTeachingGrab = THEAPP.m_pTabControlDlg->m_iCurrentTab;
	if (m_iTeachingGrab <= 0)
	{
		m_bDFMManualGrabDone = TRUE;
		return;
	}

	m_bDFMManualGrabDone = FALSE;

	AfxBeginThread(DiffusedVisionGrabThread, this);

#endif
}

BOOL CLightControlDlg::MoveZigRotationCenterPos(double dZPos)
{
#ifdef INLINE_MODE
	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("오토 포커스 설정 창의 카메라 Z축 이동 값으로 이동하오니 Z축 값이 유효한지 확인해 주세요. 진행하시겠습니까?");
	else
		strMessageBox.Format("The camera will move to the Z-axis position set in the Auto Focus dialog.\nCheck the Z-axis value. Continue?");
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return FALSE;

	THEAPP.m_pTabControlDlg->m_pJogSetDlg->UpdateData(TRUE);

	m_dCurZPosition = dZPos;
	m_dCurLightZPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageLightZPosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageLightZPos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	m_dCurTPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageTiltPosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageTiltPos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	m_dCurRPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageRotatePosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageRotatePos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	m_dCurXPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageStageXPosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageStageXPos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	m_dCurYPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageStageYPosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageStageYPos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];

	AfxBeginThread(LightControlAMoveRequest_Thread, this);

	return TRUE;
#endif
	return TRUE;
}
