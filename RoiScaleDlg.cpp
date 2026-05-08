// RoiScaleDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "RoiScaleDlg.h"
#include "afxdialogex.h"


// CRoiScaleDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRoiScaleDlg, CDialog)

CRoiScaleDlg::CRoiScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoiScaleDlg::IDD, pParent)
{
	m_dEditScale = 1.0;

	m_bCheckApplyVisionROI = FALSE;
}

CRoiScaleDlg::~CRoiScaleDlg()
{
}

void CRoiScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_APPLY_VISION_ROI, m_bCheckApplyVisionROI);
	DDX_Text(pDX, IDC_EDIT_SCALE, m_dEditScale);
}


BEGIN_MESSAGE_MAP(CRoiScaleDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRoiScaleDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRoiScaleDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CRoiScaleDlg 메시지 처리기입니다.


BOOL CRoiScaleDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CRoiScaleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CRoiScaleDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}


void CRoiScaleDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}
