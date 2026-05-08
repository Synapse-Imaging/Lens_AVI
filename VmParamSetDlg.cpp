// VmParamSetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "VmParamSetDlg.h"
#include "afxdialogex.h"


// CVmParamSetDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CVmParamSetDlg, CDialog)

CVmParamSetDlg::CVmParamSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVmParamSetDlg::IDD, pParent)
{
	m_iEditThrDetect = 10;
	m_iEditMinPixelRatio = 10;
	m_iEditAbsThresMin = 5;
	m_iEditAbsThresStep = 1;
	m_dEditVarThresMin = 0.5;
	m_dEditVarThresStep = 0.2;
	m_iEditIouTarget = 70;
}

CVmParamSetDlg::~CVmParamSetDlg()
{
}

void CVmParamSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_THR_DETECT, m_iEditThrDetect);
	DDX_Text(pDX, IDC_EDIT_MIN_PIXEL_RATIO, m_iEditMinPixelRatio);
	DDX_Text(pDX, IDC_EDIT_ABS_THRES_MIN, m_iEditAbsThresMin);
	DDX_Text(pDX, IDC_EDIT_ABS_THRES_STEP, m_iEditAbsThresStep);
	DDX_Text(pDX, IDC_EDIT_VAR_THRES_MIN, m_dEditVarThresMin);
	DDX_Text(pDX, IDC_EDIT_VAR_THRES_STEP, m_dEditVarThresStep);
	DDX_Text(pDX, IDC_EDIT_IOU_TARGET, m_iEditIouTarget);
}


BEGIN_MESSAGE_MAP(CVmParamSetDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CVmParamSetDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CVmParamSetDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CVmParamSetDlg 메시지 처리기입니다.


BOOL CVmParamSetDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CVmParamSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_iEditThrDetect = THEAPP.Struct_PreferenceStruct.m_iVmOptThrDetect;
	m_iEditMinPixelRatio = THEAPP.Struct_PreferenceStruct.m_iVmOptMinPixelRatio;
	m_iEditAbsThresMin = THEAPP.Struct_PreferenceStruct.m_iVmOptAbsThresMin;
	m_iEditAbsThresStep = THEAPP.Struct_PreferenceStruct.m_iVmOptAbsThresStep;
	m_dEditVarThresMin = THEAPP.Struct_PreferenceStruct.m_dVmOptVarThresMin;
	m_dEditVarThresStep = THEAPP.Struct_PreferenceStruct.m_dVmOptVarThresStep;
	m_iEditIouTarget = THEAPP.Struct_PreferenceStruct.m_iVmOptIouTarget;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CVmParamSetDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	THEAPP.Struct_PreferenceStruct.m_iVmOptThrDetect = m_iEditThrDetect;
	THEAPP.Struct_PreferenceStruct.m_iVmOptMinPixelRatio = m_iEditMinPixelRatio;
	THEAPP.Struct_PreferenceStruct.m_iVmOptAbsThresMin = m_iEditAbsThresMin;
	THEAPP.Struct_PreferenceStruct.m_iVmOptAbsThresStep = m_iEditAbsThresStep;
	THEAPP.Struct_PreferenceStruct.m_dVmOptVarThresMin = m_dEditVarThresMin;
	THEAPP.Struct_PreferenceStruct.m_dVmOptVarThresStep = m_dEditVarThresStep;
	THEAPP.Struct_PreferenceStruct.m_iVmOptIouTarget = m_iEditIouTarget;

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_Preference(strDataFolder + "Preference.ini");
	CString strSection;

	strSection = "IMAGE COMPARE TRAIN OPTION";

	INI_Preference.Set_Integer(strSection, "VM OPT THRES DETECT", m_iEditThrDetect);
	INI_Preference.Set_Integer(strSection, "VM OPT MIN PIXEL RATIO", m_iEditMinPixelRatio);
	INI_Preference.Set_Integer(strSection, "VM OPT ABS THRES MIN", m_iEditAbsThresMin);
	INI_Preference.Set_Integer(strSection, "VM OPT ABS THRES STEP", m_iEditAbsThresStep);
	INI_Preference.Set_Double(strSection, "VM OPT VAR THRES MIN", m_dEditVarThresMin);
	INI_Preference.Set_Double(strSection, "VM OPT VAR THRES STEP", m_dEditVarThresStep);
	INI_Preference.Set_Integer(strSection, "VM OPT IOU TARGET", m_iEditIouTarget);

	CDialog::OnOK();
}


void CVmParamSetDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
