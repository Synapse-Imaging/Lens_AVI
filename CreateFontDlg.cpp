// CreateFontDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "CreateFontDlg.h"
#include "afxdialogex.h"


// CCreateFontDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCreateFontDlg, CDialog)

CCreateFontDlg::CCreateFontDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateFontDlg::IDD, pParent)
{
	m_sFontName = _T("");
}

CCreateFontDlg::~CCreateFontDlg()
{
}

void CCreateFontDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FONTFILENAME, m_sFontName);
}


BEGIN_MESSAGE_MAP(CCreateFontDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCreateFontDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCreateFontDlg 메시지 처리기입니다.


void CCreateFontDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnOK();
}
