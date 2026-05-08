// StageSelectDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "StageSelectDlg.h"
#include "afxdialogex.h"


// CStageSelectDlg 대화 상자

IMPLEMENT_DYNAMIC(CStageSelectDlg, CDialog)

CStageSelectDlg::CStageSelectDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_STAGE_SELECT_DLG, pParent)
{
	m_iRadioStageIndex = 0;
}

CStageSelectDlg::~CStageSelectDlg()
{
}

void CStageSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_STAGE_1, m_iRadioStageIndex);
}


BEGIN_MESSAGE_MAP(CStageSelectDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CStageSelectDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CStageSelectDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CStageSelectDlg 메시지 처리기
BOOL CStageSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifdef POC_TEST
	GetDlgItem(IDC_RADIO_STAGE_2)->EnableWindow(FALSE);
#endif

	ChangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CStageSelectDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("스테이지 선택"));
		SetDlgItemText(IDC_RADIO_STAGE_1, _T("스테이지 1"));
		SetDlgItemText(IDC_RADIO_STAGE_2, _T("스테이지 2"));
		SetDlgItemText(IDOK, _T("선택"));
		SetDlgItemText(IDCANCEL, _T("취소"));
	}
	else
	{
		this->SetWindowText(_T("Select stage"));
		SetDlgItemText(IDC_RADIO_STAGE_1, _T("Stage 1"));
		SetDlgItemText(IDC_RADIO_STAGE_2, _T("Stage 2"));
		SetDlgItemText(IDOK, _T("Select"));
		SetDlgItemText(IDCANCEL, _T("Cancle"));
	}
}


void CStageSelectDlg::OnBnClickedOk()
{
	UpdateData();

	CDialog::OnOK();
}


void CStageSelectDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
