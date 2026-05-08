#include "stdafx.h"
#include "uScan.h"
#include "AutoTrainGridCtrl.h"

CAutoTrainGridCtrl::CAutoTrainGridCtrl(void)
{
}

CAutoTrainGridCtrl::~CAutoTrainGridCtrl(void)
{
}

void CAutoTrainGridCtrl::OnSetup()
{
	int iNoRows = MAX_IMAGE_TAB;
	int iNoColumns = 2;

	CUGCell cell;
	CString Temp;
	Temp = "Arial";
	m_NameFont.CreateFont(14, 0, 0, 0, 600, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, Temp);

	GetHeadingDefault(&cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_NameFont);
	cell.SetBackColor(RGB(255, 200, 0));
	cell.SetTextColor(RGB(0, 0, 255));
	SetHeadingDefault(&cell);

	GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_NameFont);
	SetGridDefault(&cell);

	SetSH_Width(0);
	SetTH_Height(16);

	SetUniformRowHeight(TRUE);
	//SetCurrentCellMode(3);

	CString sTemp;

	SetNumberRows(iNoRows);
	SetNumberCols(iNoColumns);

	QuickSetText(0, -1, "      영상번호       ");
	QuickSetText(1, -1, "     자동학습 옵션 체크       ");

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		CString sTemp;
		sTemp.Format("%d", i + 1);
		QuickSetText(0, i, sTemp);
	}

	BestFit(0, iNoColumns - 1, -1, UG_BESTFIT_TOPHEADINGS);

	RedrawAll();
}

BOOL CAutoTrainGridCtrl::IsItemChecked(long lIndex)
{
	CUGCell cell;

	GetCell(0, lIndex, &cell);
	return cell.GetBool();
}

BOOL CAutoTrainGridCtrl::SetItemCheck(long lIndex, BOOL bChecked)
{
	CUGCell cell;
	if (GetCell(0, lIndex, &cell) != UG_SUCCESS) return FALSE;

	cell.SetBool(bChecked);
	if (SetCell(0, lIndex, &cell) != UG_SUCCESS) return FALSE;

	return TRUE;
}

