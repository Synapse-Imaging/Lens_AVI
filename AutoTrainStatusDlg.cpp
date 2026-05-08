// AutoTrainStatusDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "AutoTrainStatusDlg.h"
#include "afxdialogex.h"


// CAutoTrainStatusDlg 대화 상자

IMPLEMENT_DYNAMIC(CAutoTrainStatusDlg, CDialogEx)

CAutoTrainStatusDlg::CAutoTrainStatusDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AUTO_TRAIN_STATUS_DIALOG, pParent)
{

}

CAutoTrainStatusDlg::~CAutoTrainStatusDlg()
{
}

void CAutoTrainStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAutoTrainStatusDlg, CDialogEx)
END_MESSAGE_MAP()


// CAutoTrainStatusDlg 메시지 처리기


BOOL CAutoTrainStatusDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_NCLBUTTONDOWN:
		SetActiveWindow();
		return FALSE;

	case WM_KEYDOWN:
		if ((int)pMsg->wParam == VK_ESCAPE || (int)pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CAutoTrainStatusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_AutoTrainGridCtrl.AttachGrid(this, IDC_GRID_AUTO_TRAIN_STATUS);

	UpdateStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAutoTrainStatusDlg::UpdateStatus()
{
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		m_AutoTrainGridCtrl.QuickSetText(0, -1, "      영상번호       ");
		m_AutoTrainGridCtrl.QuickSetText(1, -1, "     자동학습 옵션 체크       ");
	}
	else
	{
		m_AutoTrainGridCtrl.QuickSetText(0, -1, "    Image Number   ");
		m_AutoTrainGridCtrl.QuickSetText(1, -1, "  Auto train option check  ");
	}

	m_AutoTrainGridCtrl.BestFit(0, 1, -1, UG_BESTFIT_TOPHEADINGS);
	m_AutoTrainGridCtrl.RedrawAll();

	int i, j;
	BOOL bAutoTrainChecked[MAX_IMAGE_TAB];

	for (i = 0; i < MAX_IMAGE_TAB; i++)
		bAutoTrainChecked[i] = FALSE;

	GTRegion* pRegion;
	int iNoInspectROI;

	for (i = 0; i < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); i++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(i);
		if (pRegion == NULL)
			continue;

		if (pRegion->miInspectionType >= INSPECTION_TYPE_FIRST && pRegion->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			for (j = 0; j < MAX_INSPECTION_TAB; j++)
			{
				if (pRegion->m_AlgorithmParam[j].m_bUseImageCompare && pRegion->m_AlgorithmParam[j].m_bUseImageCompareAutoTrain)
				{
					if (pRegion->miTeachImageIndex > 0)
						bAutoTrainChecked[pRegion->miTeachImageIndex - 1] = TRUE;
				}
			}
		}
	}

	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (bAutoTrainChecked[i])
			m_AutoTrainGridCtrl.QuickSetText(1, i, " O ");
		else
			m_AutoTrainGridCtrl.QuickSetText(1, i, "");
	}
}
