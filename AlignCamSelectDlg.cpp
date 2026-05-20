// AlignCamSelectDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "AlignCamSelectDlg.h"
#include "afxdialogex.h"


// CAlignCamSelectDlg 대화 상자

IMPLEMENT_DYNAMIC(CAlignCamSelectDlg, CDialog)

CAlignCamSelectDlg::CAlignCamSelectDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ALIGN_CAM_SELECT_DLG, pParent)
{
	m_iRadioAlignCamIndex = 0;
}

CAlignCamSelectDlg::~CAlignCamSelectDlg()
{
}

void CAlignCamSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_ALIGN_CAM_1, m_iRadioAlignCamIndex);
}


BEGIN_MESSAGE_MAP(CAlignCamSelectDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAlignCamSelectDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAlignCamSelectDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CAlignCamSelectDlg 메시지 처리기
BOOL CAlignCamSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ChangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAlignCamSelectDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("얼라인 카메라 선택"));
		SetDlgItemText(IDOK, _T("선택"));
		SetDlgItemText(IDCANCEL, _T("취소"));
	}
	else
	{
		this->SetWindowText(_T("Select align camera"));
		SetDlgItemText(IDOK, _T("Select"));
		SetDlgItemText(IDCANCEL, _T("Cancle"));
	}
}


void CAlignCamSelectDlg::OnBnClickedOk()
{
	UpdateData();

	CDialog::OnOK();
}


void CAlignCamSelectDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
