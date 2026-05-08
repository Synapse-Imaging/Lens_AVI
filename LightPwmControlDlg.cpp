// LightPwmControlDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "LightPwmControlDlg.h"
#include "afxdialogex.h"


// CLightPwmControlDlg 대화 상자

IMPLEMENT_DYNAMIC(CLightPwmControlDlg, CDialog)

CLightPwmControlDlg::CLightPwmControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LIGHT_PWM_CONTROL_DLG, pParent)
{
	m_iEditComPortNumber = 1;

	m_iEditLight1 = 0;
	m_iEditLight2 = 0;
	m_iEditLight3 = 0;
	m_iEditLight4 = 0;
}

CLightPwmControlDlg::~CLightPwmControlDlg()
{
}

void CLightPwmControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_COM_PORT_NUMBER, m_iEditComPortNumber);
	DDX_Control(pDX, IDC_SLIDER_LIGHT1, m_SliderLight1);
	DDX_Text(pDX, IDC_EDIT_LIGHT1, m_iEditLight1);
	DDX_Control(pDX, IDC_SLIDER_LIGHT2, m_SliderLight2);
	DDX_Text(pDX, IDC_EDIT_LIGHT2, m_iEditLight2);
	DDX_Control(pDX, IDC_SLIDER_LIGHT3, m_SliderLight3);
	DDX_Text(pDX, IDC_EDIT_LIGHT3, m_iEditLight3);
	DDX_Control(pDX, IDC_SLIDER_LIGHT4, m_SliderLight4);
	DDX_Text(pDX, IDC_EDIT_LIGHT4, m_iEditLight4);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_LIGHTVALUE, m_bnChangeLight);
	DDX_Control(pDX, IDC_BUTTON_LIGHT_ON, m_bnLightOn);
	DDX_Control(pDX, IDC_BUTTON_LIGHT_OFF, m_bnLightOff);
	DDX_Control(pDX, IDCANCEL, m_bnExit);

}


BEGIN_MESSAGE_MAP(CLightPwmControlDlg, CDialog)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_LIGHT1, &CLightPwmControlDlg::OnChangeEditLight1)
	ON_EN_CHANGE(IDC_EDIT_LIGHT2, &CLightPwmControlDlg::OnChangeEditLight2)
	ON_EN_CHANGE(IDC_EDIT_LIGHT3, &CLightPwmControlDlg::OnChangeEditLight3)
	ON_EN_CHANGE(IDC_EDIT_LIGHT4, &CLightPwmControlDlg::OnChangeEditLight4)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_LIGHTVALUE, &CLightPwmControlDlg::OnBnClickedButtonChangeLightvalue)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_ON, &CLightPwmControlDlg::OnBnClickedButtonLightOn)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_OFF, &CLightPwmControlDlg::OnBnClickedButtonLightOff)
	ON_BN_CLICKED(IDCANCEL, &CLightPwmControlDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLightPwmControlDlg 메시지 처리기

BOOL CLightPwmControlDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CLightPwmControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//////////////////////////////////////////////////////////////////////////
	/// 조명 채널명 
	m_iEditComPortNumber = THEAPP.m_pModelDataManager->m_PageControlData.m_iComPortNumber;

	GetDlgItem(IDC_STATIC_CH1)->SetWindowTextA("Align Ch1");
	GetDlgItem(IDC_STATIC_CH2)->SetWindowTextA("Align Ch2");
	GetDlgItem(IDC_STATIC_CH3)->SetWindowTextA("Align Ch3");
	GetDlgItem(IDC_STATIC_CH4)->SetWindowTextA("Align Ch4");
	m_SliderLight1.SetRange(0, LIGHT_PWM_MAX);
	m_SliderLight2.SetRange(0, LIGHT_PWM_MAX);
	m_SliderLight3.SetRange(0, LIGHT_PWM_MAX);
	m_SliderLight4.SetRange(0, LIGHT_PWM_MAX);
	m_SliderLight1.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[0]);
	m_iEditLight1 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[0];
	m_SliderLight2.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[1]);
	m_iEditLight2 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[1];
	m_SliderLight3.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[2]);
	m_iEditLight3 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[2];
	m_SliderLight4.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[3]);
	m_iEditLight4 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[3];

	//////////////////////////////////////////////////////////////////////////

	m_bnChangeLight.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnExit.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnLightOn.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnLightOff.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLightPwmControlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;

	switch (pSlider->GetDlgCtrlID())
	{
	case IDC_SLIDER_LIGHT1:
		m_iEditLight1 = m_SliderLight1.GetPos();
		break;
	case IDC_SLIDER_LIGHT2:
		m_iEditLight2 = m_SliderLight2.GetPos();
		break;
	case IDC_SLIDER_LIGHT3:
		m_iEditLight3 = m_SliderLight3.GetPos();
		break;
	case IDC_SLIDER_LIGHT4:
		m_iEditLight4 = m_SliderLight4.GetPos();
		break;

	default:
		break;
	}

	UpdateData(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLightPwmControlDlg::OnChangeEditLight1()
{
	UpdateData(TRUE);
	m_SliderLight1.SetPos(m_iEditLight1);
}

void CLightPwmControlDlg::OnChangeEditLight2()
{
	UpdateData(TRUE);
	m_SliderLight2.SetPos(m_iEditLight2);
}

void CLightPwmControlDlg::OnChangeEditLight3()
{
	UpdateData(TRUE);
	m_SliderLight3.SetPos(m_iEditLight3);
}

void CLightPwmControlDlg::OnChangeEditLight4()
{
	UpdateData(TRUE);
	m_SliderLight4.SetPos(m_iEditLight4);
}

void CLightPwmControlDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}

void CLightPwmControlDlg::OnBnClickedButtonChangeLightvalue()
{
	UpdateData(TRUE);

	if (m_iEditLight1 < 0) m_iEditLight1 = 0;
	if (m_iEditLight1 > LIGHT_PWM_MAX) m_iEditLight1 = LIGHT_PWM_MAX;
	if (m_iEditLight2 < 0) m_iEditLight2 = 0;
	if (m_iEditLight2 > LIGHT_PWM_MAX) m_iEditLight2 = LIGHT_PWM_MAX;
	if (m_iEditLight3 < 0) m_iEditLight3 = 0;
	if (m_iEditLight3 > LIGHT_PWM_MAX) m_iEditLight3 = LIGHT_PWM_MAX;
	if (m_iEditLight4 < 0) m_iEditLight4 = 0;
	if (m_iEditLight4 > LIGHT_PWM_MAX) m_iEditLight4 = LIGHT_PWM_MAX;

	int iVisionCamIdx = THEAPP.m_pCameraManager->GetVisionCamName();

	if (THEAPP.m_pModelDataManager->m_PageControlData.m_iComPortNumber != m_iEditComPortNumber)
	{
#ifdef INLINE_MODE
		if (THEAPP.m_pLightPwmManager[iVisionCamIdx]->OpenPort(m_iEditComPortNumber) == FALSE)
		{
			AfxMessageBox("통신 포트 열기 실패!!!. 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
			return;
		}
#endif

		THEAPP.m_pModelDataManager->m_PageControlData.m_iComPortNumber = m_iEditComPortNumber;
	}

	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[0] = m_iEditLight1;
	THEAPP.m_pLightPwmManager[iVisionCamIdx]->SetCh1LightValue(m_iEditLight1);
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[1] = m_iEditLight2;
	THEAPP.m_pLightPwmManager[iVisionCamIdx]->SetCh2LightValue(m_iEditLight2);
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[2] = m_iEditLight3;
	THEAPP.m_pLightPwmManager[iVisionCamIdx]->SetCh3LightValue(m_iEditLight3);
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[0].uiChannel[3] = m_iEditLight4;
	THEAPP.m_pLightPwmManager[iVisionCamIdx]->SetCh4LightValue(m_iEditLight4);
	THEAPP.m_pLightPwmManager[iVisionCamIdx]->LightOn();

	//**********  조명 값을 파일에 써줌
	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);

	CString strInspectLightInfo;
	strInspectLightInfo.Format("%s\\HW\\Vision_N%d\\InspectLightInfo_Pwm.ini", strOpticModelFolder, iVisionCamIdx + 1);

	THEAPP.m_pModelDataManager->SavePwmLightInfo(strInspectLightInfo);
}

void CLightPwmControlDlg::OnBnClickedButtonLightOn()
{
	int iVisionCamIdx = THEAPP.m_pCameraManager->GetVisionCamName();

	THEAPP.m_pLightPwmManager[iVisionCamIdx]->LightOn();
}

void CLightPwmControlDlg::OnBnClickedButtonLightOff()
{
	int iVisionCamIdx = THEAPP.m_pCameraManager->GetVisionCamName();

	THEAPP.m_pLightPwmManager[iVisionCamIdx]->LightOff();
}
