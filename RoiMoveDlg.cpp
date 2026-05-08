// RoiMoveDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "RoiMoveDlg.h"
#include "afxdialogex.h"


// CRoiMoveDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRoiMoveDlg, CDialog)

CRoiMoveDlg::CRoiMoveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoiMoveDlg::IDD, pParent)
{
	m_bPixelData = FALSE;
	m_bEnableAllROI = FALSE;

	m_dEditMoveX = 0;
	m_dEditMoveY = 0;

	m_bCheckApplyVisionROI = FALSE;
}

CRoiMoveDlg::~CRoiMoveDlg()
{
}

void CRoiMoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MOVE_X, m_dEditMoveX);
	DDX_Text(pDX, IDC_EDIT_MOVE_Y, m_dEditMoveY);
	DDX_Check(pDX, IDC_CHECK_APPLY_VISION_ROI, m_bCheckApplyVisionROI);
}


BEGIN_MESSAGE_MAP(CRoiMoveDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRoiMoveDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRoiMoveDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CRoiMoveDlg 메시지 처리기입니다.


BOOL CRoiMoveDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CRoiMoveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if (m_bPixelData)
	{
		GetDlgItem(IDC_STATIC_X)->SetWindowText("X 방향 (픽셀)");
		GetDlgItem(IDC_STATIC_Y)->SetWindowText("Y 방향 (픽셀)");
	}

	GetDlgItem(IDC_CHECK_APPLY_VISION_ROI)->EnableWindow(m_bEnableAllROI);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CRoiMoveDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	CDialog::OnOK();
}


void CRoiMoveDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
