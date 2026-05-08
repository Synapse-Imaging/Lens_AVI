#include "stdafx.h"
#include "uScan.h"
#include "LibraryGridCtrl.h"

CLibraryGridCtrl::CLibraryGridCtrl(void)
{
	m_iListType = LIST_TYPE_INSPECT_LIBRARY;
}

CLibraryGridCtrl::~CLibraryGridCtrl(void)
{
}

void CLibraryGridCtrl::OnSetup()
{
	int iNoRows;
	int iNoColumns;

	CUGCell cell;
	CString sTemp;

	if (m_iListType == LIST_TYPE_INSPECT_LIBRARY)
	{
		iNoRows = 0;
		iNoColumns = 6;

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
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetFont(&m_NameFont);
		cell.SetTextColor(RGB(255, 255, 255));
		cell.SetBackColor(RGB(0, 0, 0));
		SetGridDefault(&cell);

		SetSH_Width(0);
		SetTH_Height(19);

		SetUniformRowHeight(TRUE);
		SetCurrentCellMode(3);
		SetHighlightRow(TRUE);

		SetNumberRows(iNoRows);
		SetNumberCols(iNoColumns);
		CString sTemp;

		QuickSetText(0, -1, "  No.  ");
		QuickSetText(1, -1, " 검사 알고리즘 명칭 ");
		QuickSetText(2, -1, "    검사 1    ");
		QuickSetText(3, -1, "    검사 2    ");
		QuickSetText(4, -1, "    검사 3    ");
		QuickSetText(5, -1, "                알고리즘 설명                       ");

		BestFit(0, iNoColumns - 1, -1, UG_BESTFIT_TOPHEADINGS);
	}
	else if (m_iListType == LIST_TYPE_LIGHT_LIBRARY)
	{
		iNoRows = 0;
		iNoColumns = 3;

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
		cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
		cell.SetFont(&m_NameFont);
		cell.SetTextColor(RGB(255, 255, 255));
		cell.SetBackColor(RGB(0, 0, 0));
		SetGridDefault(&cell);

		SetSH_Width(0);
		SetTH_Height(19);

		SetUniformRowHeight(TRUE);
		SetCurrentCellMode(3);
		SetHighlightRow(TRUE);

		SetNumberRows(iNoRows);
		SetNumberCols(iNoColumns);
		CString sTemp;

		QuickSetText(0, -1, "  No.  ");
		QuickSetText(1, -1, "            조명 명칭            ");
		QuickSetText(2, -1, "                         조명 설명                                ");

		BestFit(0, iNoColumns - 1, -1, UG_BESTFIT_TOPHEADINGS);
	}

	RedrawAll();
}

BOOL CLibraryGridCtrl::IsItemChecked(long lIndex)
{
	CUGCell cell;

	GetCell(0, lIndex, &cell);
	return cell.GetBool();
}

BOOL CLibraryGridCtrl::SetItemCheck(long lIndex, BOOL bChecked)
{
	CUGCell cell;
	if (GetCell(0, lIndex, &cell) != UG_SUCCESS)
		return FALSE;

	cell.SetBool(bChecked);
	if (SetCell(0, lIndex, &cell) != UG_SUCCESS)
		return FALSE;

	return TRUE;
}
void CLibraryGridCtrl::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, BOOL processed)
{
	if (row < 0)
	{
		return;
	}
	if (updn < 1)
	{
		return;
	}

	::PostMessage(m_HWnd, UM_LIST_SELECT, (WPARAM)row, (LPARAM)row);
}

void CLibraryGridCtrl::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	::PostMessage(m_HWnd, UM_LIST_DBCLICK, (WPARAM)row, (LPARAM)row);
}

int CLibraryGridCtrl::OnEditFinish(int col, long row, CWnd* edit, LPCTSTR string, BOOL cancelFlag)
{
	if (col == 0)
	{
		int iNoRow = GetNumberRows();
		BestFit(col, col, iNoRow, UG_BESTFIT_TOPHEADINGS);
		RedrawCol(col);
	}

	return TRUE;
}

long CLibraryGridCtrl::GetCurListNumber()
{
	return GetNumberRows();
}

void CLibraryGridCtrl::OnKeyDown(UINT *vcKey, BOOL processed)
{
	UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);

	long lCurrentRow = GetCurrentRow();
	long iMoveRow;
	long lLength = GetNumberRows();

	if (*vcKey == VK_UP)
	{
		iMoveRow = lCurrentRow - 1;
		if (iMoveRow < 0)
		{
			iMoveRow = 0;
		}
		::PostMessage(m_HWnd, UM_LIST_SELECT, (WPARAM)iMoveRow, (LPARAM)iMoveRow);
	}
	else if (*vcKey == VK_DOWN)
	{
		iMoveRow = lCurrentRow + 1;
		if (iMoveRow > lLength - 1)
		{
			iMoveRow = lLength - 1;
		}
		::PostMessage(m_HWnd, UM_LIST_SELECT, (WPARAM)iMoveRow, (LPARAM)iMoveRow);
	}
}

void CLibraryGridCtrl::MoveCurrent(int iCurDefectIdx)
{
	GotoRow(iCurDefectIdx);
	::PostMessage(m_HWnd, UM_LIST_SELECT, (WPARAM)iCurDefectIdx, (LPARAM)0);
}

void CLibraryGridCtrl::MoveFirst()
{
	MoveCurrentRow(UG_TOP);
	::PostMessage(m_HWnd, UM_LIST_SELECT, (WPARAM)0, (LPARAM)0);
}

void CLibraryGridCtrl::MoveLast()
{
	long lCurrentRow = GetCurrentRow();
	long iMoveRow;
	long lLength = GetNumberRows();
	iMoveRow = lLength - 1;

	MoveCurrentRow(UG_BOTTOM);
	::PostMessage(m_HWnd, UM_LIST_SELECT, (WPARAM)iMoveRow, (LPARAM)iMoveRow);
}

void CLibraryGridCtrl::MovePrev()
{
	long lCurrentRow = GetCurrentRow();
	long iMoveRow;

	iMoveRow = lCurrentRow - 1;
	if (iMoveRow < 0)
		iMoveRow = 0;

	MoveCurrentRow(UG_LINEUP);
	::PostMessage(m_HWnd, UM_LIST_SELECT, (WPARAM)iMoveRow, (LPARAM)iMoveRow);
}

void CLibraryGridCtrl::MoveNext()
{
	long lCurrentRow = GetCurrentRow();
	long iMoveRow;
	long lLength = GetNumberRows();

	iMoveRow = lCurrentRow + 1;
	if (iMoveRow > lLength - 1)
		iMoveRow = lLength - 1;

	MoveCurrentRow(UG_LINEDOWN);
	::PostMessage(m_HWnd, UM_LIST_SELECT, (WPARAM)iMoveRow, (LPARAM)iMoveRow);
}