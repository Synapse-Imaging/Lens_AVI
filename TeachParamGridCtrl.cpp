#include "stdafx.h"
#include "uScan.h"
#include "TeachParamGridCtrl.h"

CTeachParamGridCtrl::CTeachParamGridCtrl(void)
{
}

CTeachParamGridCtrl::~CTeachParamGridCtrl(void)
{
}

void CTeachParamGridCtrl::OnSetup()
{
	int iNoRows = 31;
	int iNoColumns = 3;

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
	SetCurrentCellMode(3);

	CString sTemp;

	SetNumberRows(iNoRows);
	SetNumberCols(iNoColumns);

	QuickSetText(0, -1, "  알고리즘  ");
	QuickSetText(1, -1, "  밝은불량  ");
	QuickSetText(2, -1, " 어두운불량 ");

	QuickSetText(0, 0, "     영상 전처리      ");
	QuickSetText(0, 1, "   영상 필터링    ");
	QuickSetText(0, 2, " ROI 얼라인 ");
	QuickSetText(0, 3, " 비등방 ROI 얼라인 ");
	QuickSetText(0, 4, " 파트 유무 체크 ");
	QuickSetText(0, 5, " 로컬 얼라인 연결 ");
	QuickSetText(0, 6, " 검사 제외 영역 연결 ");
	QuickSetText(0, 7, " 검사 영역 생성 연결 ");
	QuickSetText(0, 8, " 고정 Threshold ");
	QuickSetText(0, 9, " 동적 Threshold ");
	QuickSetText(0, 10, " 균일도 검사 ");
	QuickSetText(0, 11, " Hysteresis Threshold ");
	QuickSetText(0, 12, " 검사 제외 블랍 ");
	QuickSetText(0, 13, " 엣지 측정 ");
	QuickSetText(0, 14, " 이중 블랍 ");
	QuickSetText(0, 15, " 요철 영상 ");
	QuickSetText(0, 16, " 영상 비교 ");
	QuickSetText(0, 17, " 라인 정렬 ");
	QuickSetText(0, 18, " 문자 인식 ");
	QuickSetText(0, 19, " 바코드 ");
	QuickSetText(0, 20, " 바코드 품질 ");
	QuickSetText(0, 21, " 커넥터 핀 휨 ");
	QuickSetText(0, 22, " 덴트 ");
	QuickSetText(0, 23, " 에폭시 홀 ");
	QuickSetText(0, 24, " 스티프너 에폭시 ");
	QuickSetText(0, 25, " 거리 측정 ");
	QuickSetText(0, 26, " 갭 측정 ");
	QuickSetText(0, 27, " 딥러닝 검사 ");
	QuickSetText(0, 28, " 넥 에폭시 길이 ");
	QuickSetText(0, 29, " 넥 에폭시 들뜸 ");
	QuickSetText(0, 30, " Rx, Tx 측정 ");

	BestFit(0, iNoColumns - 1, -1, UG_BESTFIT_TOPHEADINGS);

	RedrawAll();
}

BOOL CTeachParamGridCtrl::IsItemChecked(long lIndex)
{
	CUGCell cell;

	GetCell(0, lIndex, &cell);
	return cell.GetBool();
}

BOOL CTeachParamGridCtrl::SetItemCheck(long lIndex, BOOL bChecked)
{
	CUGCell cell;
	if (GetCell(0, lIndex, &cell) != UG_SUCCESS) return FALSE;

	cell.SetBool(bChecked);
	if (SetCell(0, lIndex, &cell) != UG_SUCCESS) return FALSE;

	return TRUE;
}

