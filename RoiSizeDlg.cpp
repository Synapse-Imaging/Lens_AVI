// RoiSizeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "RoiSizeDlg.h"
#include "afxdialogex.h"


// CRoiSizeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRoiSizeDlg, CDialog)

CRoiSizeDlg::CRoiSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoiSizeDlg::IDD, pParent)
{
	m_bPixelData = FALSE;

	m_dEditSizeX = 0.0;
	m_dEditSizeY = 0.0;
}

CRoiSizeDlg::~CRoiSizeDlg()
{
}

void CRoiSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SIZE_X, m_dEditSizeX);
	DDX_Text(pDX, IDC_EDIT_SIZE_Y, m_dEditSizeY);
}


BEGIN_MESSAGE_MAP(CRoiSizeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRoiSizeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRoiSizeDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CRoiSizeDlg 메시지 처리기입니다.


BOOL CRoiSizeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CRoiSizeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if (m_bPixelData)
	{
		GetDlgItem(IDC_STATIC_X)->SetWindowText("X 방향 (픽셀)");
		GetDlgItem(IDC_STATIC_Y)->SetWindowText("Y 방향 (픽셀)");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CRoiSizeDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	CDialog::OnOK();
}


void CRoiSizeDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
