// InspectionPriorityDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectionPriorityDlg.h"
#include "afxdialogex.h"


// CInspectionPriorityDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInspectionPriorityDlg, CDialog)

CInspectionPriorityDlg::CInspectionPriorityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectionPriorityDlg::IDD, pParent)
{
	int i;

	for (i = 0; i < MAX_INSPECTION_TYPE; i++)
		m_iPriority[i] = -1;
}

CInspectionPriorityDlg::~CInspectionPriorityDlg()
{
}

void CInspectionPriorityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_1, m_iPriority[0]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_2, m_iPriority[1]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_3, m_iPriority[2]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_4, m_iPriority[3]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_5, m_iPriority[4]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_6, m_iPriority[5]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_7, m_iPriority[6]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_8, m_iPriority[7]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_9, m_iPriority[8]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_10, m_iPriority[9]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_11, m_iPriority[10]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_12, m_iPriority[11]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_13, m_iPriority[12]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_14, m_iPriority[13]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_15, m_iPriority[14]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_16, m_iPriority[15]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_17, m_iPriority[16]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_18, m_iPriority[17]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_19, m_iPriority[18]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_20, m_iPriority[19]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_21, m_iPriority[20]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_22, m_iPriority[21]);
	DDX_CBIndex(pDX, IDC_COMBO_INSPECTION_PRIORITY_23, m_iPriority[22]);
}


BEGIN_MESSAGE_MAP(CInspectionPriorityDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CInspectionPriorityDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CInspectionPriorityDlg::OnBnClickedCancel)

	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_1, OnSelchangeComboPriority_1)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_2, OnSelchangeComboPriority_2)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_3, OnSelchangeComboPriority_3)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_4, OnSelchangeComboPriority_4)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_5, OnSelchangeComboPriority_5)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_6, OnSelchangeComboPriority_6)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_7, OnSelchangeComboPriority_7)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_8, OnSelchangeComboPriority_8)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_9, OnSelchangeComboPriority_9)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_10, OnSelchangeComboPriority_10)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_11, OnSelchangeComboPriority_11)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_12, OnSelchangeComboPriority_12)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_13, OnSelchangeComboPriority_13)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_14, OnSelchangeComboPriority_14)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_15, OnSelchangeComboPriority_15)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_16, OnSelchangeComboPriority_16)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_17, OnSelchangeComboPriority_17)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_18, OnSelchangeComboPriority_18)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_19, OnSelchangeComboPriority_19)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_20, OnSelchangeComboPriority_20)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_21, OnSelchangeComboPriority_21)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_22, OnSelchangeComboPriority_22)
	ON_CBN_SELCHANGE(IDC_COMBO_INSPECTION_PRIORITY_23, OnSelchangeComboPriority_23)

	ON_BN_CLICKED(IDC_BUTTON_SET_DEFECT_PRIORITY, &CInspectionPriorityDlg::OnBnClickedButtonSetDefectPriority1)
END_MESSAGE_MAP()


// CInspectionPriorityDlg 메시지 처리기입니다.


BOOL CInspectionPriorityDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_3, _T(g_strInspectionTypeName[0].c_str())); 
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_4, _T(g_strInspectionTypeName[1].c_str())); 
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_5, _T(g_strInspectionTypeName[2].c_str())); 
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_6, _T(g_strInspectionTypeName[3].c_str())); 
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_7, _T(g_strInspectionTypeName[4].c_str())); 
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_8, _T(g_strInspectionTypeName[5].c_str())); 
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_9, _T(g_strInspectionTypeName[6].c_str())); 
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_10, _T(g_strInspectionTypeName[7].c_str())); 
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_11, _T(g_strInspectionTypeName[8].c_str())); 
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_12, _T(g_strInspectionTypeName[9].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_13, _T(g_strInspectionTypeName[10].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_14, _T(g_strInspectionTypeName[11].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_15, _T(g_strInspectionTypeName[12].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_16, _T(g_strInspectionTypeName[13].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_17, _T(g_strInspectionTypeName[14].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_18, _T(g_strInspectionTypeName[15].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_19, _T(g_strInspectionTypeName[16].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_20, _T(g_strInspectionTypeName[17].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_21, _T(g_strInspectionTypeName[18].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_22, _T(g_strInspectionTypeName[19].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_23, _T(g_strInspectionTypeName[20].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_24, _T(g_strInspectionTypeName[21].c_str()));
	SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_25, _T(g_strInspectionTypeName[22].c_str()));

	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
		m_iPriority[i] = THEAPP.Struct_PreferenceStruct.iInspectionTypePriority[i];

	ChangeLanguage();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CInspectionPriorityDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("검사 대상 우선 순위 설정"));
		SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_1, _T("검사 대상"));
		SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_2, _T("우선 순위"));
		SetDlgItemText(IDC_BUTTON_SET_DEFECT_PRIORITY, _T("불량 항목 우선 순위 및 표시 색상 설정"));
		SetDlgItemText(IDOK, _T("저장"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Set inspection type priority"));
		SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_1, _T("Inspection type"));
		SetDlgItemText(IDC_STATIC_PRIORITYINSPECTIONDLG_2, _T("Priority"));
		SetDlgItemText(IDC_BUTTON_SET_DEFECT_PRIORITY, _T("Set defect type priority and display color"));
		SetDlgItemText(IDOK, _T("Save"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}


void CInspectionPriorityDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();

	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		THEAPP.Struct_PreferenceStruct.iInspectionTypePriority[i] = m_iPriority[i];
	}

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";

	CString sDefectInfoFileName;
	sDefectInfoFileName = strModelFolder + "DefectInfo.ini";

	CIniFileCS INI(sDefectInfoFileName);

	CString strSection, sTemp;

	strSection = "InspectionType_Priority";
	for (int j = 0; j < MAX_INSPECTION_TYPE; j++)
	{
		sTemp.Format("INSPECTION_TYPE_PRIORITY_%d", j + 1);
		INI.Set_Integer(strSection, sTemp, m_iPriority[j]);
	}

	CDialog::OnOK();
}


void CInspectionPriorityDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_1()
{
	ChangePriority(0);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_2()
{
	ChangePriority(1);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_3()
{
	ChangePriority(2);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_4()
{
	ChangePriority(3);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_5()
{
	ChangePriority(4);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_6()
{
	ChangePriority(5);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_7()
{
	ChangePriority(6);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_8()
{
	ChangePriority(7);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_9()
{
	ChangePriority(8);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_10()
{
	ChangePriority(9);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_11()
{
	ChangePriority(10);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_12()
{
	ChangePriority(11);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_13()
{
	ChangePriority(12);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_14()
{
	ChangePriority(13);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_15()
{
	ChangePriority(14);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_16()
{
	ChangePriority(15);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_17()
{
	ChangePriority(16);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_18()
{
	ChangePriority(17);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_19()
{
	ChangePriority(18);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_20()
{
	ChangePriority(19);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_21()
{
	ChangePriority(20);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_22()
{
	ChangePriority(21);
}

void CInspectionPriorityDlg::OnSelchangeComboPriority_23()
{
	ChangePriority(22);
}

void CInspectionPriorityDlg::ChangePriority(int iSelectedIndex)
{
	int i;
	int temp[MAX_INSPECTION_TYPE];

	for (i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		if (i == iSelectedIndex)
			continue;
		temp[i] = m_iPriority[i];
	}

	for (i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		if (i == iSelectedIndex)
			continue;

		if (temp[i] >= m_iPriority[iSelectedIndex])
			--(temp[i]);
	}

	UpdateData(TRUE);

	for (i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		if (i == iSelectedIndex)
			continue;

		if (temp[i] >= m_iPriority[iSelectedIndex])
			++(temp[i]);
	}

	for (i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		if (i == iSelectedIndex)
			continue;

		m_iPriority[i] = temp[i];
	}

	UpdateData(FALSE);
}


void CInspectionPriorityDlg::OnBnClickedButtonSetDefectPriority1()
{
	CDefectPriorityDlg dlg;
	dlg.SetInspectionType(0);
	dlg.DoModal();
}
