// SetFlipDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "SetFlipDlg.h"
#include "afxdialogex.h"


// CSetFlipDlg 대화 상자

IMPLEMENT_DYNAMIC(CSetFlipDlg, CDialog)

CSetFlipDlg::CSetFlipDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SET_FLIP_DLG, pParent)
{
	m_iRadioLeftRightFlip = 0;
}

CSetFlipDlg::~CSetFlipDlg()
{
}

void CSetFlipDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Radio(pDX, IDC_RADIO_LEFTRIGHT_FLIP, m_iRadioLeftRightFlip);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetFlipDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSetFlipDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSetFlipDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSetFlipDlg 메시지 처리기


void CSetFlipDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	CDialog::OnOK();
}


void CSetFlipDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
