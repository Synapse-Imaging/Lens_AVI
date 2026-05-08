// AlgorithmGapMeasureDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmGapMeasureDlg.h"
#include "afxdialogex.h"


// CAlgorithmGapMeasureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmGapMeasureDlg, CDialog)

CAlgorithmGapMeasureDlg::CAlgorithmGapMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmGapMeasureDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseGapMeasure = FALSE;
	m_iRadioGapMeasureUpperGv = 0;
	m_iRadioGapMeasureUpperPos = 0;
	m_dEditGapMeasureUpperSmFactor = 1.0;
	m_iEditGapMeasureUpperEdgeStr = 10;
	m_iRadioGapMeasureLowerGv = 0;
	m_iRadioGapMeasureLowerPos = 0;
	m_dEditGapMeasureLowerSmFactor = 1.0;
	m_iEditGapMeasureLowerEdgeStr = 10;

	m_iEditGapMeasureUpperPosOffset = 0;
	m_iEditGapMeasureLowerPosOffset = 0;

	//m_iEditGapMeasureSaveID = 1;

	m_dEditGapMeasureGapInspectionUpper = 1000;
	m_dEditGapMeasureGapInspectionLower = 10;
	m_dEditGapMeasureGapMeasureOffset = 0;
	m_iRadioGapMeasureDir = 0;
	m_bCheckGapMeasureAlarm = FALSE;
	m_iEditGapMeasurePointNumber = 1;
	m_iRadioGapMeasureSelectType = 0;

	m_bCheckGapMeasureUpperLine = FALSE;
	m_bCheckGapMeasureLowerLine = FALSE;

	m_bCheckGapMeasureDiffImage = FALSE;
	m_iEditGapMeasurePosStart = 10;
	m_iEditGapMeasurePosEnd = 20;

	m_bCheckGapMeasureEndRemove = FALSE;
	m_iEditGapMeasureRemoveEndUpper = 0;
	m_iEditGapMeasureRemoveEndLower = 0;

	m_bCheckGapMeasureRetry = FALSE;
	m_iRadioGapMeasureRetryGv = 0;
	m_iRadioGapMeasureRetryPos = 0;
	m_dEditGapMeasureRetrySmFactor = 1.0;
	m_iEditGapMeasureRetryEdgeStr = 10;
	m_iEditGapMeasureRetryPosOffset = 0;
}

CAlgorithmGapMeasureDlg::~CAlgorithmGapMeasureDlg()
{
}

void CAlgorithmGapMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_GAP_MEASURE, m_bCheckUseGapMeasure);
	DDX_Radio(pDX, IDC_RADIO_GAP_MEASURE_UPPER_GV_ANY, m_iRadioGapMeasureUpperGv);
	DDX_Radio(pDX, IDC_RADIO_GAP_MEASURE_UPPER_POS_BEST, m_iRadioGapMeasureUpperPos);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_UPPER_SMOOTHING_FACTOR, m_dEditGapMeasureUpperSmFactor);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_UPPER_EDGE_STRENGTH, m_iEditGapMeasureUpperEdgeStr);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_UPPER_POS_OFFSET, m_iEditGapMeasureUpperPosOffset);

	DDX_Radio(pDX, IDC_RADIO_GAP_MEASURE_LOWER_GV_ANY, m_iRadioGapMeasureLowerGv);
	DDX_Radio(pDX, IDC_RADIO_GAP_MEASURE_LOWER_POS_BEST, m_iRadioGapMeasureLowerPos);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_LOWER_SMOOTHING_FACTOR, m_dEditGapMeasureLowerSmFactor);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_LOWER_EDGE_STRENGTH, m_iEditGapMeasureLowerEdgeStr);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_LOWER_POS_OFFSET, m_iEditGapMeasureLowerPosOffset);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_GAP_INSPECTION_UPPER, m_dEditGapMeasureGapInspectionUpper);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_GAP_INSPECTION_LOWER, m_dEditGapMeasureGapInspectionLower);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_GAP_MEASURE_OFFSET, m_dEditGapMeasureGapMeasureOffset);
	DDX_Radio(pDX, IDC_RADIO_GAP_MEASURE_DIR_X, m_iRadioGapMeasureDir);
	DDX_Check(pDX, IDC_CHECK_GAP_MEASURE_ALARM, m_bCheckGapMeasureAlarm);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_POINT_NUMBER, m_iEditGapMeasurePointNumber);
	DDX_Radio(pDX, IDC_RADIO_GAP_MEASURE_SELECT_MAX, m_iRadioGapMeasureSelectType);
	DDX_Control(pDX, IDC_COMBO_GAP_MEASURE_TYPE, m_cbGapMeasureType);
	DDX_Control(pDX, IDC_COMBO_GAP_MEASURE_IMAGE_NUMBER_1, m_cbGapMeasureImageNo1);
	DDX_Control(pDX, IDC_COMBO_GAP_MEASURE_IMAGE_NUMBER_2, m_cbGapMeasureImageNo2);

	DDX_Check(pDX, IDC_CHECK_GAP_MEASURE_LINE_UPPER, m_bCheckGapMeasureUpperLine);
	DDX_Check(pDX, IDC_CHECK_GAP_MEASURE_LINE_LOWER, m_bCheckGapMeasureLowerLine);

	DDX_Check(pDX, IDC_CHECK_GAP_MEASURE_DIFF_IMAGE, m_bCheckGapMeasureDiffImage);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_POS_START, m_iEditGapMeasurePosStart);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_POS_END, m_iEditGapMeasurePosEnd);
	DDX_Control(pDX, IDC_COMBO_GAP_MEASURE_IMAGE_NUMBER_3, m_cbGapMeasureImageNo3);

	DDX_Check(pDX, IDC_CHECK_GAP_MEASURE_END_REMOVE, m_bCheckGapMeasureEndRemove);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_REMOVE_END_UPPER, m_iEditGapMeasureRemoveEndUpper);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_REMOVE_END_LOWER, m_iEditGapMeasureRemoveEndLower);

	DDX_Check(pDX, IDC_CHECK_GAP_MEASURE_RETRY, m_bCheckGapMeasureRetry);
	DDX_Control(pDX, IDC_COMBO_GAP_MEASURE_RETRY_IMAGE_NUMBER, m_cbGapMeasureRetryImageNo);
	DDX_Radio(pDX, IDC_RADIO_GAP_MEASURE_RETRY_GV_ANY, m_iRadioGapMeasureRetryGv);
	DDX_Radio(pDX, IDC_RADIO_GAP_MEASURE_RETRY_POS_BEST, m_iRadioGapMeasureRetryPos);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_RETRY_SMOOTHING_FACTOR, m_dEditGapMeasureRetrySmFactor);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_RETRY_EDGE_STRENGTH, m_iEditGapMeasureRetryEdgeStr);
	DDX_Text(pDX, IDC_EDIT_GAP_MEASURE_RETRY_POS_OFFSET, m_iEditGapMeasureRetryPosOffset);
}


BEGIN_MESSAGE_MAP(CAlgorithmGapMeasureDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_GAP_MEASURE_DIR_X, &CAlgorithmGapMeasureDlg::OnBnClickedRadioGapMeasureDirX)
	ON_BN_CLICKED(IDC_RADIO_GAP_MEASURE_DIR_Y, &CAlgorithmGapMeasureDlg::OnBnClickedRadioGapMeasureDirY)
END_MESSAGE_MAP()


// CAlgorithmGapMeasureDlg 메시지 처리기입니다.

void CAlgorithmGapMeasureDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmGapMeasureDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseGapMeasure = AlgorithmParam.m_bUseGapMeasure;
	m_iRadioGapMeasureUpperGv = AlgorithmParam.m_iGapMeasureUpperGv;
	m_iRadioGapMeasureUpperPos = AlgorithmParam.m_iGapMeasureUpperPos;
	m_dEditGapMeasureUpperSmFactor = AlgorithmParam.m_dGapMeasureUpperSmFactor;
	m_iEditGapMeasureUpperEdgeStr = AlgorithmParam.m_iGapMeasureUpperEdgeStr;
	m_iRadioGapMeasureLowerGv = AlgorithmParam.m_iGapMeasureLowerGv;
	m_iRadioGapMeasureLowerPos = AlgorithmParam.m_iGapMeasureLowerPos;
	m_dEditGapMeasureLowerSmFactor = AlgorithmParam.m_dGapMeasureLowerSmFactor;
	m_iEditGapMeasureLowerEdgeStr = AlgorithmParam.m_iGapMeasureLowerEdgeStr;
	m_iEditGapMeasureUpperPosOffset = AlgorithmParam.m_iGapMeasureUpperPosOffset;
	m_iEditGapMeasureLowerPosOffset = AlgorithmParam.m_iGapMeasureLowerPosOffset;
	//m_iEditGapMeasureSaveID = AlgorithmParam.m_iGapMeasureSaveID;
	m_dEditGapMeasureGapInspectionUpper = AlgorithmParam.m_dGapMeasureGapInspectionUpper;
	m_dEditGapMeasureGapInspectionLower = AlgorithmParam.m_dGapMeasureGapInspectionLower;
	m_dEditGapMeasureGapMeasureOffset = AlgorithmParam.m_dGapMeasureGapMeasureOffset;
	m_iRadioGapMeasureDir = AlgorithmParam.m_iGapMeasureDir;
	m_bCheckGapMeasureAlarm = AlgorithmParam.m_bGapMeasureAlarm;
	m_iEditGapMeasurePointNumber = AlgorithmParam.m_iGapMeasurePointNumber;
	m_iRadioGapMeasureSelectType = AlgorithmParam.m_iGapMeasureSelectType;
	m_cbGapMeasureType.SetCurSel(AlgorithmParam.m_iGapMeasureSaveID);
	m_cbGapMeasureImageNo1.SetCurSel(AlgorithmParam.m_iGapMeasureImageNo1);
	m_cbGapMeasureImageNo2.SetCurSel(AlgorithmParam.m_iGapMeasureImageNo2);

	m_bCheckGapMeasureUpperLine = AlgorithmParam.m_bGapMeasureUpperLine;
	m_bCheckGapMeasureLowerLine = AlgorithmParam.m_bGapMeasureLowerLine;

	m_bCheckGapMeasureDiffImage = AlgorithmParam.m_bGapMeasureDiffImage;
	m_iEditGapMeasurePosStart = AlgorithmParam.m_iGapMeasurePosStart;
	m_iEditGapMeasurePosEnd = AlgorithmParam.m_iGapMeasurePosEnd;
	m_cbGapMeasureImageNo3.SetCurSel(AlgorithmParam.m_iGapMeasureImageNo3);

	m_bCheckGapMeasureEndRemove = AlgorithmParam.m_bGapMeasureEndRemove;
	m_iEditGapMeasureRemoveEndUpper = AlgorithmParam.m_iGapMeasureRemoveEndUpper;
	m_iEditGapMeasureRemoveEndLower = AlgorithmParam.m_iGapMeasureRemoveEndLower;

	m_bCheckGapMeasureRetry = AlgorithmParam.m_bGapMeasureRetry;
	m_cbGapMeasureRetryImageNo.SetCurSel(AlgorithmParam.m_iGapMeasureRetryImageNo);
	m_iRadioGapMeasureRetryGv = AlgorithmParam.m_iGapMeasureRetryGv;
	m_iRadioGapMeasureRetryPos = AlgorithmParam.m_iGapMeasureRetryPos;
	m_dEditGapMeasureRetrySmFactor = AlgorithmParam.m_dGapMeasureRetrySmFactor;
	m_iEditGapMeasureRetryEdgeStr = AlgorithmParam.m_iGapMeasureRetryEdgeStr;
	m_iEditGapMeasureRetryPosOffset = AlgorithmParam.m_iGapMeasureRetryPosOffset;

	UpdateData(FALSE);
}

void CAlgorithmGapMeasureDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseGapMeasure = m_bCheckUseGapMeasure;
	pAlgorithmParam->m_iGapMeasureUpperGv = m_iRadioGapMeasureUpperGv;
	pAlgorithmParam->m_iGapMeasureUpperPos = m_iRadioGapMeasureUpperPos;
	pAlgorithmParam->m_dGapMeasureUpperSmFactor = m_dEditGapMeasureUpperSmFactor;
	pAlgorithmParam->m_iGapMeasureUpperEdgeStr = m_iEditGapMeasureUpperEdgeStr;
	pAlgorithmParam->m_iGapMeasureLowerGv = m_iRadioGapMeasureLowerGv;
	pAlgorithmParam->m_iGapMeasureLowerPos = m_iRadioGapMeasureLowerPos;
	pAlgorithmParam->m_dGapMeasureLowerSmFactor = m_dEditGapMeasureLowerSmFactor;
	pAlgorithmParam->m_iGapMeasureLowerEdgeStr = m_iEditGapMeasureLowerEdgeStr;
	pAlgorithmParam->m_iGapMeasureUpperPosOffset = m_iEditGapMeasureUpperPosOffset;
	pAlgorithmParam->m_iGapMeasureLowerPosOffset = m_iEditGapMeasureLowerPosOffset;
	//pAlgorithmParam->m_iGapMeasureSaveID = m_iEditGapMeasureSaveID;
	pAlgorithmParam->m_dGapMeasureGapInspectionUpper = m_dEditGapMeasureGapInspectionUpper;
	pAlgorithmParam->m_dGapMeasureGapInspectionLower = m_dEditGapMeasureGapInspectionLower;
	pAlgorithmParam->m_dGapMeasureGapMeasureOffset = m_dEditGapMeasureGapMeasureOffset;
	pAlgorithmParam->m_iGapMeasureDir = m_iRadioGapMeasureDir;
	pAlgorithmParam->m_bGapMeasureAlarm = m_bCheckGapMeasureAlarm;
	pAlgorithmParam->m_iGapMeasurePointNumber = m_iEditGapMeasurePointNumber;
	pAlgorithmParam->m_iGapMeasureSelectType = m_iRadioGapMeasureSelectType;
	pAlgorithmParam->m_iGapMeasureSaveID = m_cbGapMeasureType.GetCurSel();
	pAlgorithmParam->m_iGapMeasureImageNo1 = m_cbGapMeasureImageNo1.GetCurSel();
	pAlgorithmParam->m_iGapMeasureImageNo2 = m_cbGapMeasureImageNo2.GetCurSel();

	pAlgorithmParam->m_bGapMeasureUpperLine = m_bCheckGapMeasureUpperLine;
	pAlgorithmParam->m_bGapMeasureLowerLine = m_bCheckGapMeasureLowerLine;

	pAlgorithmParam->m_bGapMeasureDiffImage = m_bCheckGapMeasureDiffImage;
	pAlgorithmParam->m_iGapMeasurePosStart = m_iEditGapMeasurePosStart;
	pAlgorithmParam->m_iGapMeasurePosEnd = m_iEditGapMeasurePosEnd;
	pAlgorithmParam->m_iGapMeasureImageNo3 = m_cbGapMeasureImageNo3.GetCurSel();

	pAlgorithmParam->m_bGapMeasureEndRemove = m_bCheckGapMeasureEndRemove;
	pAlgorithmParam->m_iGapMeasureRemoveEndUpper = m_iEditGapMeasureRemoveEndUpper;
	pAlgorithmParam->m_iGapMeasureRemoveEndLower = m_iEditGapMeasureRemoveEndLower;

	pAlgorithmParam->m_bGapMeasureRetry = m_bCheckGapMeasureRetry;
	pAlgorithmParam->m_iGapMeasureRetryImageNo = m_cbGapMeasureRetryImageNo.GetCurSel();
	pAlgorithmParam->m_iGapMeasureRetryGv = m_iRadioGapMeasureRetryGv;
	pAlgorithmParam->m_iGapMeasureRetryPos = m_iRadioGapMeasureRetryPos;
	pAlgorithmParam->m_dGapMeasureRetrySmFactor = m_dEditGapMeasureRetrySmFactor;
	pAlgorithmParam->m_iGapMeasureRetryEdgeStr = m_iEditGapMeasureRetryEdgeStr;
	pAlgorithmParam->m_iGapMeasureRetryPosOffset = m_iEditGapMeasureRetryPosOffset;
}


BOOL CAlgorithmGapMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;
	CString sTemp;

	m_cbGapMeasureImageNo1.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbGapMeasureImageNo1.AddString(sTemp);
	}

	m_cbGapMeasureImageNo2.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbGapMeasureImageNo2.AddString(sTemp);
	}

	m_cbGapMeasureImageNo3.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbGapMeasureImageNo3.AddString(sTemp);
	}

	m_cbGapMeasureRetryImageNo.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbGapMeasureRetryImageNo.AddString(sTemp);
	}

	m_cbGapMeasureImageNo1.SetCurSel(0);
	m_cbGapMeasureImageNo2.SetCurSel(0);
	m_cbGapMeasureImageNo3.SetCurSel(0);
	m_cbGapMeasureRetryImageNo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmGapMeasureDlg::OnBnClickedRadioGapMeasureDirX()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	GetDlgItem(IDC_STATIC_ROI_1)->SetWindowText("좌측 Edge 측정 알고리즘");
	GetDlgItem(IDC_STATIC_ROI_2)->SetWindowText("우측 Edge 측정 알고리즘");
}

void CAlgorithmGapMeasureDlg::OnBnClickedRadioGapMeasureDirY()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	GetDlgItem(IDC_STATIC_ROI_1)->SetWindowText("상측 Edge 측정 알고리즘");
	GetDlgItem(IDC_STATIC_ROI_2)->SetWindowText("하측 Edge 측정 알고리즘");
}
