// Operator_Register.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "Operator_RegisterDlg.h"
#include "afxdialogex.h"


// COperator_Register 대화 상자입니다.

IMPLEMENT_DYNAMIC(COperator_RegisterDlg, CDialog)

COperator_RegisterDlg::COperator_RegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COperator_RegisterDlg::IDD, pParent)
{
	m_strLogin_Mode = _T("");
	m_strOperator = _T("");
	m_strPwd = _T("");
}

COperator_RegisterDlg::~COperator_RegisterDlg()
{
}

void COperator_RegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COperator_RegisterDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COperator_RegisterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &COperator_RegisterDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// COperator_Register 메시지 처리기입니다.
BOOL COperator_RegisterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ChangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void COperator_RegisterDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("관리자 패스워드 변경"));
		SetDlgItemText(IDC_STATIC_ADMINPASSWORDDLG_1, _T("현재 비밀번호:"));
		SetDlgItemText(IDC_STATIC_ADMINPASSWORDDLG_2, _T("새로운 비밀번호:"));
		SetDlgItemText(IDC_STATIC_ADMINPASSWORDDLG_3, _T("새로운 비밀번호 재확인:"));
		SetDlgItemText(IDOK, _T("변경"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Change admin password"));
		SetDlgItemText(IDC_STATIC_ADMINPASSWORDDLG_1, _T("Current Password:"));
		SetDlgItemText(IDC_STATIC_ADMINPASSWORDDLG_2, _T("New Password:"));
		SetDlgItemText(IDC_STATIC_ADMINPASSWORDDLG_3, _T("Check new password:"));
		SetDlgItemText(IDOK, _T("Change"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}


void COperator_RegisterDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nResOper = 0;
	int nResAdmin = 0;

	CString strPassWord;

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_RegisterInfo(strDataFolder + "RegisterInfo.ini");
	CString strSection, strKey;

	strSection = "PASSWORD";
	strKey = "Pwd";
	strPassWord = INI_RegisterInfo.Get_String(strSection, strKey, "1111");

	GetDlgItem(IDC_EDIT_CURRENT_PASSWORD)->GetWindowText(m_strLogin_Mode);
	m_strLogin_Mode.TrimLeft();
	m_strLogin_Mode.TrimRight();

	if (strPassWord != m_strLogin_Mode)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("비밀번호 불일치");
		else
			strMessageBox.Format("Password is incorrect");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

		return;
	}

	GetDlgItem(IDC_EDIT_CHANGE_PASSWORD)->GetWindowText(m_strOperator);
	m_strOperator.TrimLeft();
	m_strOperator.TrimRight();

	GetDlgItem(IDC_EDIT_CHECK_PASSWORD)->GetWindowText(m_strPwd);
	m_strPwd.TrimLeft();
	m_strPwd.TrimRight();

	if (m_strOperator != m_strPwd)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("새로운 비밀번호 불일치");
		else
			strMessageBox.Format("New password is incorrect");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

		return;
	}

	INI_RegisterInfo.Set_String(strSection, strKey, m_strOperator);

	CDialog::OnOK();
}


void COperator_RegisterDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
