// AdminPasswordDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AdminPasswordDlg.h"
#include "afxdialogex.h"


// CAdminPasswordDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAdminPasswordDlg, CDialog)

CAdminPasswordDlg::CAdminPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdminPasswordDlg::IDD, pParent)
{

	m_sUserID = _T("Admin");
}

CAdminPasswordDlg::~CAdminPasswordDlg()
{
}

void CAdminPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ADMINOPERATOR, m_sUserID);
}


BEGIN_MESSAGE_MAP(CAdminPasswordDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAdminPasswordDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAdminPasswordDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CAdminPasswordDlg 메시지 처리기입니다.
BOOL CAdminPasswordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ChangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CAdminPasswordDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	//	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
	//		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


void CAdminPasswordDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("관리자 전환"));
		SetDlgItemText(IDC_STATIC_MODECHANGEDLG_1, _T("작업자 이름:"));
		SetDlgItemText(IDC_STATIC_MODECHANGEDLG_2, _T("비밀번호:"));
		SetDlgItemText(IDOK, _T("전환"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Switch operator"));
		SetDlgItemText(IDC_STATIC_MODECHANGEDLG_1, _T("Operator Name:"));
		SetDlgItemText(IDC_STATIC_MODECHANGEDLG_2, _T("Password:"));
		SetDlgItemText(IDOK, _T("Switch"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}


void CAdminPasswordDlg::OnBnClickedOk()
{
	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_RegisterInfo(strDataFolder + "RegisterInfo.ini");

	//************************* Text 에서 받아온 값 **************************
	CString strTextInputPassWord = _T("");
	GetDlgItem(IDC_EDIT_ADMINPASSWORD)->GetWindowText(strTextInputPassWord);

	strTextInputPassWord.TrimLeft();
	strTextInputPassWord.TrimRight();
	//*************************************************************************

	//************************* Ini 파일에서 받아온 값 **************************
	CString strRegisterPassWord;
	strRegisterPassWord = _T("");

	strRegisterPassWord = INI_RegisterInfo.Get_String("PASSWORD", "Pwd", "");
	//*************************************************************************

	int nResPassWord = 0;
	nResPassWord = strTextInputPassWord.Compare(strRegisterPassWord);

	if (nResPassWord != 0)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("비밀번호 불일치");
		else
			strMessageBox.Format("Password is incorrect");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

		return;
	}

	CDialog::OnOK();
}


void CAdminPasswordDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
