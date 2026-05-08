// LightAverageValueDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LightAverageValueDlg.h"
#include "afxdialogex.h"


// CLightAverageValueDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLightAverageValueDlg, CDialog)

CLightAverageValueDlg::CLightAverageValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightAverageValueDlg::IDD, pParent)
{
	m_iEditLight1 = 0;
	m_iEditLight2 = 0;
	m_iEditLight3 = 0;
	m_iEditLight4 = 0;
	m_iEditLight5 = 0;
	m_iEditLight6 = 0;
	m_iEditLight7 = 0;
	m_iEditLight8 = 0;
	m_iEditLight9 = 0;
	m_iEditLight10 = 0;
	m_iEditLightTotal = 0;
	m_dEditTeachingFocusValue = 0;
}

CLightAverageValueDlg::~CLightAverageValueDlg()
{
}

void CLightAverageValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE1, m_iEditLight1);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE2, m_iEditLight2);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE3, m_iEditLight3);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE4, m_iEditLight4);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE5, m_iEditLight5);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE6, m_iEditLight6);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE7, m_iEditLight7);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE8, m_iEditLight8);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE9, m_iEditLight9);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE10, m_iEditLight10);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE_TOTAL, m_iEditLightTotal);

	DDX_Control(pDX, IDOK, m_bnOK);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);
	DDX_Text(pDX, IDC_EDIT_FOCUS_VALUE, m_dEditTeachingFocusValue);
}


BEGIN_MESSAGE_MAP(CLightAverageValueDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLightAverageValueDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLightAverageValueDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLightAverageValueDlg 메시지 처리기입니다.


BOOL CLightAverageValueDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	/// 조명 채널명 
	CString strChNameFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LightChannel.ini";
	CIniFileCS INI_LIGHTCH(strChNameFileName);
	CString sSection, sKey;
	sSection = THEAPP.m_ModelDefineInfo.m_strVisionName[THEAPP.m_iCurTeachVision];

	CString sLightChannelName;
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "1", "미사용");
	GetDlgItem(IDC_STATIC_CH1)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "2", "미사용");
	GetDlgItem(IDC_STATIC_CH2)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "3", "미사용");
	GetDlgItem(IDC_STATIC_CH3)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "4", "미사용");
	GetDlgItem(IDC_STATIC_CH4)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "5", "미사용");
	GetDlgItem(IDC_STATIC_CH5)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "6", "미사용");
	GetDlgItem(IDC_STATIC_CH6)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "7", "미사용");
	GetDlgItem(IDC_STATIC_CH7)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "8", "미사용");
	GetDlgItem(IDC_STATIC_CH8)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "9", "미사용");
	GetDlgItem(IDC_STATIC_CH9)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "10", "미사용");
	GetDlgItem(IDC_STATIC_CH10)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "11", "미사용");
	GetDlgItem(IDC_STATIC_CH11)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "12", "미사용");
	GetDlgItem(IDC_STATIC_CH12)->SetWindowTextA(sLightChannelName);

	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "13", "미사용");
	GetDlgItem(IDC_STATIC_CH13)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "14", "미사용");
	GetDlgItem(IDC_STATIC_CH14)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "15", "미사용");
	GetDlgItem(IDC_STATIC_CH15)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "16", "미사용");
	GetDlgItem(IDC_STATIC_CH16)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "17", "미사용");
	GetDlgItem(IDC_STATIC_CH17)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "18", "미사용");
	GetDlgItem(IDC_STATIC_CH18)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "19", "미사용");
	GetDlgItem(IDC_STATIC_CH19)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "20", "미사용");
	GetDlgItem(IDC_STATIC_CH20)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "21", "미사용");
	GetDlgItem(IDC_STATIC_CH21)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "22", "미사용");
	GetDlgItem(IDC_STATIC_CH22)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "23", "미사용");
	GetDlgItem(IDC_STATIC_CH23)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "24", "미사용");
	GetDlgItem(IDC_STATIC_CH24)->SetWindowTextA(sLightChannelName);

	//m_bnOK.Set_Text(_T("확인"));
	m_bnOK.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	//m_bnCancel.Set_Text(_T("취소"));
	m_bnCancel.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_iEditLight1 = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][0];
	m_iEditLight2 = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][1];
	m_iEditLight3 = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][2];
	m_iEditLight4 = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][3];
	m_iEditLight5 = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][4];
	m_iEditLight6 = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][5];
	m_iEditLight7 = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][6];
	m_iEditLight8 = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][7];
	m_iEditLight9 = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][8];
	m_iEditLight10 = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][9];

	m_iEditLightTotal = THEAPP.m_pModelDataManager->m_iLightAverageValueTotal[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];

	m_dEditTeachingFocusValue = THEAPP.m_pModelDataManager->m_dTeachingFocusValue[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CLightAverageValueDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][0] = m_iEditLight1;
	THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][1] = m_iEditLight2;
	THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][2] = m_iEditLight3;
	THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][3] = m_iEditLight4;
	THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][4] = m_iEditLight5;
	THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][5] = m_iEditLight6;
	THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][6] = m_iEditLight7;
	THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][7] = m_iEditLight8;
	THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][8] = m_iEditLight9;
	THEAPP.m_pModelDataManager->m_iLightChannelRefGV[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][9] = m_iEditLight10;
	THEAPP.m_pModelDataManager->m_iLightAverageValueTotal[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] = m_iEditLightTotal;

	THEAPP.m_pModelDataManager->m_dTeachingFocusValue[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] = m_dEditTeachingFocusValue;

	CDialog::OnOK();
}


void CLightAverageValueDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
