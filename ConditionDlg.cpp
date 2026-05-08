// ConditionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "ConditionDlg.h"
#include "afxdialogex.h"


// CConditionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CConditionDlg, CDialog)

CConditionDlg::CConditionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConditionDlg::IDD, pParent)
{

}

CConditionDlg::~CConditionDlg()
{
}

void CConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConditionDlg, CDialog)
END_MESSAGE_MAP()


// CConditionDlg 메시지 처리기입니다.

BOOL CConditionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Grid.AttachGrid(this, IDC_GRID_INSPECTION_CONDITION);

	int i;

	for (i = 0; i < MAX_DEFECT_NAME; i++)
	{
		m_Grid.QuickSetText(0, i, THEAPP.m_strDefectName[i]);

		// Default
		m_Grid.QuickSetText(2, i, "0");
		m_Grid.QuickSetText(3, i, "0");
		m_Grid.QuickSetText(4, i, "0");
		m_Grid.QuickSetText(5, i, "100");
		m_Grid.QuickSetText(6, i, "2");
		m_Grid.QuickSetText(7, i, "1");
	}

	ChangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CConditionDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		m_Grid.QuickSetText(0, -1, "                     불량 항목                    ");
		m_Grid.QuickSetText(2, -1, "최소 면적");
		m_Grid.QuickSetText(3, -1, "중간 면적");
		m_Grid.QuickSetText(4, -1, "최대 면적");
		m_Grid.QuickSetText(5, -1, "허용 수량 (면적< 최소)");
		m_Grid.QuickSetText(6, -1, "허용 수량 (면적>=최소)");
		m_Grid.QuickSetText(7, -1, "허용 수량 (면적>=중간)");
	}
	else
	{
		m_Grid.QuickSetText(0, -1, "                    Defect name                   ");
		m_Grid.QuickSetText(2, -1, "Min area");
		m_Grid.QuickSetText(3, -1, "Mid area");
		m_Grid.QuickSetText(4, -1, "Max area");
		m_Grid.QuickSetText(5, -1, "allow Qty(Area< Min)");
		m_Grid.QuickSetText(6, -1, "allow Qty(Area>=Min)");
		m_Grid.QuickSetText(7, -1, "allow Qty(Area>=Mid)");
	}

	m_Grid.RedrawAll();
}