// ExtraDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "ExtraDlg.h"
#include "afxdialogex.h"


// CExtraDlg 대화 상자입니다.

CExtraDlg* CExtraDlg::m_pInstance = NULL;

CExtraDlg* CExtraDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CExtraDlg();
		if (!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_EXTRA_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CExtraDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CExtraDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CExtraDlg, CDialog)

CExtraDlg::CExtraDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExtraDlg::IDD, pParent)
{
	//SetPosition(VIEW1_DLG3_LEFT+120+VIEW1_DLG3_WIDTH-550+50, VIEW1_DLG3_TOP+50+50, VIEW1_DLG3_WIDTH-668, VIEW1_DLG3_HEIGHT+170);
	SetPosition(VIEW1_DLG3_LEFT + 85, VIEW1_DLG3_TOP + 50 + 50 + VIEW1_DLG3_HEIGHT + 170 + VIEW1_DLG3_HEIGHT + 120 + 2, VIEW1_DLG3_WIDTH - 95, VIEW1_DLG1_HEIGHT + 370 - VIEW_TOOLBAR_HEIGHT - (VIEW1_DLG3_HEIGHT + 170 + VIEW1_DLG3_HEIGHT + 130));

	m_iEditLightValueStart = 0;
	m_iEditLightValueEnd = 0;
	m_iEditLightValueInterval = 0;
	m_iEditLightValueInTol = 0;
	m_iEditTotalImageValueTol = 0;
	m_dEditFocusValueStart = 0.0;
	m_dEditFocusValueEnd = 0.0;
	m_dEditFocusValueInterval = 0.0;
	m_iRadioSelectFocusTol = 0;
	m_dEditFocusZValueTol = 0.0;
	m_dEditFocusEdgeValueTol = 0.0;
}

CExtraDlg::~CExtraDlg()
{
}

void CExtraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_LIGHT_CAL_TEST, m_bnAutoCalTestLight);
	DDX_Control(pDX, IDC_BUTTON_LIGHT_MASTER_GV, m_bnLightMasterGV);
	DDX_Control(pDX, IDC_BUTTON_FOCUS_CAL_TEST, m_bnAutoCalTestFocus);
	DDX_Control(pDX, IDC_BUTTON_SAVE_SETTING, m_bnSaveSetting);
	DDX_Text(pDX, IDC_EDIT_LIGHT_VALUE_START, m_iEditLightValueStart);
	DDX_Text(pDX, IDC_EDIT_LIGHT_VALUE_END, m_iEditLightValueEnd);
	DDX_Text(pDX, IDC_EDIT_LIGHT_VALUE_INTERVAL, m_iEditLightValueInterval);
	DDX_Text(pDX, IDC_EDIT_LIGHT_VALUE_TOL, m_iEditLightValueInTol);
	DDX_Text(pDX, IDC_EDIT_TOTALIMAGE_VALUE_TOL, m_iEditTotalImageValueTol);
	DDX_Text(pDX, IDC_EDIT_FOCUS_VALUE_START, m_dEditFocusValueStart);
	DDX_Text(pDX, IDC_EDIT_FOCUS_VALUE_END, m_dEditFocusValueEnd);
	DDX_Text(pDX, IDC_EDIT_FOCUS_VALUE_INTERVAL, m_dEditFocusValueInterval);
	DDX_Radio(pDX, IDC_RADIO_Z_VALUE, m_iRadioSelectFocusTol);
	DDX_Text(pDX, IDC_EDIT_FOCUS_ZVALUE_TOL, m_dEditFocusZValueTol);
	DDX_Text(pDX, IDC_EDIT_FOCUS_EDGEVALUE_TOL, m_dEditFocusEdgeValueTol);
	DDX_Control(pDX, IDC_PROGRESS_AUTO_CAL, m_ctrlProgressAutoCal);
}


BEGIN_MESSAGE_MAP(CExtraDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_SETTING, &CExtraDlg::OnBnClickedButtonSaveSetting)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_MASTER_GV, &CExtraDlg::OnBnClickedButtonLightMasterGV)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_CAL_TEST, &CExtraDlg::OnBnClickedButtonLightCalTest)
	ON_BN_CLICKED(IDC_BUTTON_FOCUS_CAL_TEST, &CExtraDlg::OnBnClickedButtonFocusCalTest)
END_MESSAGE_MAP()


// CExtraDlg 메시지 처리기입니다.


BOOL CExtraDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CExtraDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//m_bnAutoCalTestLight.Set_Text(_T("조명 오토 캘 실행"));
	m_bnAutoCalTestLight.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	//m_bnLightMasterGV.Set_Text(_T("마스터 샘플 GV 자동 계산"));
	m_bnLightMasterGV.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	//m_bnAutoCalTestFocus.Set_Text(_T("포커스 오토 캘 실행"));
	m_bnAutoCalTestFocus.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	//m_bnSaveSetting.Set_Text(_T("변경 내용 저장"));
	m_bnSaveSetting.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_ctrlProgressAutoCal.SetRange(0, 100);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CExtraDlg::LoadViewParam()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	m_iEditLightValueStart = THEAPP.m_pModelDataManager->m_iLightValueStart;
	m_iEditLightValueEnd = THEAPP.m_pModelDataManager->m_iLightValueEnd;
	m_iEditLightValueInterval = THEAPP.m_pModelDataManager->m_iLightValueInterval;
	m_iEditLightValueInTol = THEAPP.m_pModelDataManager->m_iLightValueInTol;
	m_iEditTotalImageValueTol = THEAPP.m_pModelDataManager->m_iTotalImageValueTol;
	m_dEditFocusValueStart = THEAPP.m_pModelDataManager->m_dFocusValueStart;
	m_dEditFocusValueEnd = THEAPP.m_pModelDataManager->m_dFocusValueEnd;
	m_dEditFocusValueInterval = THEAPP.m_pModelDataManager->m_dFocusValueInterval;
	m_iRadioSelectFocusTol = THEAPP.m_pModelDataManager->m_iSelectFocusTol;
	m_dEditFocusZValueTol = THEAPP.m_pModelDataManager->m_dFocusZValueTol;
	m_dEditFocusEdgeValueTol = THEAPP.m_pModelDataManager->m_dFocusEdgeValueTol;

	UpdateData(FALSE);
}

void CExtraDlg::UpdateViewParam()
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}

void CExtraDlg::OnBnClickedButtonSaveSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	UpdateViewParam();

	UpdateData();

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	CString strAutoCal;
	strAutoCal.Format("%s\\HW\\Vision_N%d\\AutoCal.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	CIniFileCS INI_AutoCal(strAutoCal);

	CString strSection, strKey;
	strSection = "Auto Cal Setting";

	THEAPP.m_pModelDataManager->m_iLightValueStart = m_iEditLightValueStart;
	strKey.Format("m_iLightValueStart");
	INI_AutoCal.Set_Integer(strSection, strKey, THEAPP.m_pModelDataManager->m_iLightValueStart);

	THEAPP.m_pModelDataManager->m_iLightValueEnd = m_iEditLightValueEnd;
	strKey.Format("m_iLightValueEnd");
	INI_AutoCal.Set_Integer(strSection, strKey, THEAPP.m_pModelDataManager->m_iLightValueEnd);

	THEAPP.m_pModelDataManager->m_iLightValueInterval = m_iEditLightValueInterval;
	strKey.Format("m_iLightValueInterval");
	INI_AutoCal.Set_Integer(strSection, strKey, THEAPP.m_pModelDataManager->m_iLightValueInterval);

	THEAPP.m_pModelDataManager->m_iLightValueInTol = m_iEditLightValueInTol;
	strKey.Format("m_iLightValueInTol");
	INI_AutoCal.Set_Integer(strSection, strKey, THEAPP.m_pModelDataManager->m_iLightValueInTol);

	THEAPP.m_pModelDataManager->m_iTotalImageValueTol = m_iEditTotalImageValueTol;
	strKey.Format("m_iTotalImageValueTol");
	INI_AutoCal.Set_Integer(strSection, strKey, THEAPP.m_pModelDataManager->m_iTotalImageValueTol);

	THEAPP.m_pModelDataManager->m_dFocusValueStart = m_dEditFocusValueStart;
	strKey.Format("m_dFocusValueStart");
	INI_AutoCal.Set_Double(strSection, strKey, THEAPP.m_pModelDataManager->m_dFocusValueStart);

	THEAPP.m_pModelDataManager->m_dFocusValueEnd = m_dEditFocusValueEnd;
	strKey.Format("m_dFocusValueEnd");
	INI_AutoCal.Set_Double(strSection, strKey, THEAPP.m_pModelDataManager->m_dFocusValueEnd);

	THEAPP.m_pModelDataManager->m_dFocusValueInterval = m_dEditFocusValueInterval;
	strKey.Format("m_dFocusValueInterval");
	INI_AutoCal.Set_Double(strSection, strKey, THEAPP.m_pModelDataManager->m_dFocusValueInterval);

	THEAPP.m_pModelDataManager->m_iSelectFocusTol = m_iRadioSelectFocusTol;
	strKey.Format("m_iSelectFocusTol");
	INI_AutoCal.Set_Integer(strSection, strKey, THEAPP.m_pModelDataManager->m_iSelectFocusTol);

	THEAPP.m_pModelDataManager->m_dFocusZValueTol = m_dEditFocusZValueTol;
	strKey.Format("m_dFocusZValueTol");
	INI_AutoCal.Set_Double(strSection, strKey, THEAPP.m_pModelDataManager->m_dFocusZValueTol);

	THEAPP.m_pModelDataManager->m_dFocusEdgeValueTol = m_dEditFocusEdgeValueTol;
	strKey.Format("m_dFocusEdgeValueTol");
	INI_AutoCal.Set_Double(strSection, strKey, THEAPP.m_pModelDataManager->m_dFocusEdgeValueTol);
}

#include "LightAutoCalResultDlg.h"

void CExtraDlg::OnBnClickedButtonLightCalTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	CString strAutoCal;
	strAutoCal.Format("%s\\HW\\Vision_N%d\\AutoCal.ini", strOpticModelFolder, THEAPP.m_iCurTeachVision + 1);

	THEAPP.m_pModelDataManager->LoadLightAverageValue(strAutoCal);

	THEAPP.m_pAutoCalService->InitAutoCalResult();
	THEAPP.m_pAutoCalService->AutoCalLightStart(THEAPP.m_iCurTeachVision);
}

void CExtraDlg::OnBnClickedButtonLightMasterGV()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	THEAPP.m_pAutoCalService->InitAutoCalResult();
	THEAPP.m_pAutoCalService->AutoCalMasterGvStart(THEAPP.m_iCurTeachVision);
}

#include "FocusAutoCalResultDlg.h"

void CExtraDlg::OnBnClickedButtonFocusCalTest()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	THEAPP.m_pAutoCalService->InitAutoCalResult();
	THEAPP.m_pAutoCalService->AutoCalFocusStart(THEAPP.m_iCurTeachVision);
}

void CExtraDlg::SetTeachModeControl()
{
	int iVisionCamIdx = THEAPP.m_pCameraManager->GetVisionCamName();

	GetDlgItem(IDC_BUTTON_SAVE_SETTING)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_LIGHT_MASTER_GV)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_LIGHT_CAL_TEST)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_FOCUS_CAL_TEST)->EnableWindow(FALSE);
}
