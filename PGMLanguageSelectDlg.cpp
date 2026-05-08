// PGMLanguageSelectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "PGMLanguageSelectDlg.h"
#include "afxdialogex.h"


// CPGMLanguageSelect 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPGMLanguageSelectDlg, CDialog)

CPGMLanguageSelectDlg::CPGMLanguageSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPGMLanguageSelectDlg::IDD, pParent)
{
}

CPGMLanguageSelectDlg::~CPGMLanguageSelectDlg()
{
}

void CPGMLanguageSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_PGM_LANGUGAGE_KOREAN, m_buttonPGMLanguageKorean);
	DDX_Control(pDX, IDC_BUTTON_PGM_LANGUGAGE_ENGLISH, m_buttonPGMLanguageEnglish);
	DDX_Check(pDX, IDC_CHECK_PGM_LANGUAGE_DIALOG_HIDE, m_bCheckPGMLanguageDialogHide);
}


BEGIN_MESSAGE_MAP(CPGMLanguageSelectDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PGM_LANGUGAGE_KOREAN, &CPGMLanguageSelectDlg::OnBnClickedButtonPgmLangugageKorean)
	ON_BN_CLICKED(IDC_BUTTON_PGM_LANGUGAGE_ENGLISH, &CPGMLanguageSelectDlg::OnBnClickedButtonPgmLangugageEnglish)
END_MESSAGE_MAP()


// CPGMLanguageSelectDlg 메시지 처리기입니다.
BOOL CPGMLanguageSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		m_buttonPGMLanguageKorean.Init_Ctrl(_T("Arial"), 9, TRUE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
		m_buttonPGMLanguageEnglish.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(255, 255, 255), 0, 0);
	}
	else
	{
		m_buttonPGMLanguageKorean.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(255, 255, 255), 0, 0);
		m_buttonPGMLanguageEnglish.Init_Ctrl(_T("Arial"), 9, TRUE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPGMLanguageSelectDlg::OnBnClickedButtonPgmLangugageKorean()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	THEAPP.m_iPGMLanguageSelect = 0;

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_PGMLanguageSelect(strDataFolder + "PGMLanguageSelect.ini");
	CString strSection, strKey;

	strSection = "LANGUAGE";
	strKey = "Select";
	INI_PGMLanguageSelect.Set_Integer(strSection, strKey, THEAPP.m_iPGMLanguageSelect);

	strSection = "DIALOG POPUP";
	strKey = "Hide";
	INI_PGMLanguageSelect.Set_Bool(strSection, strKey, m_bCheckPGMLanguageDialogHide);
	THEAPP.m_bPGMLanguageDialogHide = m_bCheckPGMLanguageDialogHide;

	m_buttonPGMLanguageEnglish.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(255, 255, 255), 0, 0);
	m_buttonPGMLanguageKorean.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	CDialog::OnOK();
}


void CPGMLanguageSelectDlg::OnBnClickedButtonPgmLangugageEnglish()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	THEAPP.m_iPGMLanguageSelect = 1;

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_PGMLanguageSelect(strDataFolder + "PGMLanguageSelect.ini");
	CString strSection, strKey;

	strSection = "LANGUAGE";
	strKey = "Select";
	INI_PGMLanguageSelect.Set_Integer(strSection, strKey, THEAPP.m_iPGMLanguageSelect);

	strSection = "DIALOG POPUP";
	strKey = "Hide";
	INI_PGMLanguageSelect.Set_Bool(strSection, strKey, m_bCheckPGMLanguageDialogHide);
	THEAPP.m_bPGMLanguageDialogHide = m_bCheckPGMLanguageDialogHide;

	m_buttonPGMLanguageKorean.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(255, 255, 255), 0, 0);
	m_buttonPGMLanguageEnglish.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	CDialog::OnOK();
}
