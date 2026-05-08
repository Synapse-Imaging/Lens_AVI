// AlgorithmDistanceMeasureDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmDistanceMeasureDlg.h"
#include "afxdialogex.h"


// CAlgorithmDistanceMeasureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmDistanceMeasureDlg, CDialog)

CAlgorithmDistanceMeasureDlg::CAlgorithmDistanceMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmDistanceMeasureDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseDistanceMeasure = FALSE;
	m_iRadioDistanceMeasureUpperGv = 0;
	m_iRadioDistanceMeasureUpperPos = 0;
	m_dEditDistanceMeasureUpperSmFactor = 1.0;
	m_iEditDistanceMeasureUpperEdgeStr = 10;
	m_iRadioDistanceMeasureLowerGv = 0;
	m_iRadioDistanceMeasureLowerPos = 0;
	m_dEditDistanceMeasureLowerSmFactor = 1.0;
	m_iEditDistanceMeasureLowerEdgeStr = 10;

	m_iEditDistanceMeasureUpperPosOffset = 0;
	m_iEditDistanceMeasureLowerPosOffset = 0;
	m_iRadioDistanceMeasureUsageType = 0;

	m_iEditDistanceMeasureSaveID = 1;

	m_dEditDistanceMeasureGapInspectionUpper = 1000;
	m_dEditDistanceMeasureGapInspectionLower = 10;

	m_iRadioDistanceMeasureROIType = 0;
}

CAlgorithmDistanceMeasureDlg::~CAlgorithmDistanceMeasureDlg()
{
}

void CAlgorithmDistanceMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_DISTANCE_MEASURE, m_bCheckUseDistanceMeasure);
	DDX_Radio(pDX, IDC_RADIO_DISTANCE_MEASURE_UPPER_GV_ANY, m_iRadioDistanceMeasureUpperGv);
	DDX_Radio(pDX, IDC_RADIO_DISTANCE_MEASURE_UPPER_POS_BEST, m_iRadioDistanceMeasureUpperPos);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_MEASURE_UPPER_SMOOTHING_FACTOR, m_dEditDistanceMeasureUpperSmFactor);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_MEASURE_UPPER_EDGE_STRENGTH, m_iEditDistanceMeasureUpperEdgeStr);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_MEASURE_UPPER_POS_OFFSET, m_iEditDistanceMeasureUpperPosOffset);

	DDX_Radio(pDX, IDC_RADIO_DISTANCE_MEASURE_LOWER_GV_ANY, m_iRadioDistanceMeasureLowerGv);
	DDX_Radio(pDX, IDC_RADIO_DISTANCE_MEASURE_LOWER_POS_BEST, m_iRadioDistanceMeasureLowerPos);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_MEASURE_LOWER_SMOOTHING_FACTOR, m_dEditDistanceMeasureLowerSmFactor);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_MEASURE_LOWER_EDGE_STRENGTH, m_iEditDistanceMeasureLowerEdgeStr);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_MEASURE_LOWER_POS_OFFSET, m_iEditDistanceMeasureLowerPosOffset);

	DDX_Radio(pDX, IDC_RADIO_DISTANCE_MEASURE_USAGE_TYPE_DISTANCE, m_iRadioDistanceMeasureUsageType);

	DDX_Text(pDX, IDC_EDIT_DISTANCE_MEASURE_SAVE_ID, m_iEditDistanceMeasureSaveID);

	DDX_Text(pDX, IDC_EDIT_DISTANCE_MEASURE_GAP_INSPECTION_UPPER, m_dEditDistanceMeasureGapInspectionUpper);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_MEASURE_GAP_INSPECTION_LOWER, m_dEditDistanceMeasureGapInspectionLower);

	DDX_Radio(pDX, IDC_RADIO_DISTANCE_MEASURE_ROI_TYPE_ONE, m_iRadioDistanceMeasureROIType);
}


BEGIN_MESSAGE_MAP(CAlgorithmDistanceMeasureDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_DISTANCE_MEASURE_USAGE_TYPE_DISTANCE, &CAlgorithmDistanceMeasureDlg::OnBnClickedRadioDistanceMeasureUsageTypeDistance)
	ON_BN_CLICKED(IDC_RADIO_DISTANCE_MEASURE_USAGE_TYPE_ROI, &CAlgorithmDistanceMeasureDlg::OnBnClickedRadioDistanceMeasureUsageTypeRoi)
END_MESSAGE_MAP()


// CAlgorithmDistanceMeasureDlg 메시지 처리기입니다.

void CAlgorithmDistanceMeasureDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmDistanceMeasureDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseDistanceMeasure = AlgorithmParam.m_bUseDistanceMeasure;
	m_iRadioDistanceMeasureUpperGv = AlgorithmParam.m_iDistanceMeasureUpperGv;
	m_iRadioDistanceMeasureUpperPos = AlgorithmParam.m_iDistanceMeasureUpperPos;
	m_dEditDistanceMeasureUpperSmFactor = AlgorithmParam.m_dDistanceMeasureUpperSmFactor;
	m_iEditDistanceMeasureUpperEdgeStr = AlgorithmParam.m_iDistanceMeasureUpperEdgeStr;
	m_iRadioDistanceMeasureLowerGv = AlgorithmParam.m_iDistanceMeasureLowerGv;
	m_iRadioDistanceMeasureLowerPos = AlgorithmParam.m_iDistanceMeasureLowerPos;
	m_dEditDistanceMeasureLowerSmFactor = AlgorithmParam.m_dDistanceMeasureLowerSmFactor;
	m_iEditDistanceMeasureLowerEdgeStr = AlgorithmParam.m_iDistanceMeasureLowerEdgeStr;
	m_iEditDistanceMeasureUpperPosOffset = AlgorithmParam.m_iDistanceMeasureUpperPosOffset;
	m_iEditDistanceMeasureLowerPosOffset = AlgorithmParam.m_iDistanceMeasureLowerPosOffset;
	m_iRadioDistanceMeasureUsageType = AlgorithmParam.m_iDistanceMeasureUsageType;
	m_iEditDistanceMeasureSaveID = AlgorithmParam.m_iDistanceMeasureSaveID;
	m_dEditDistanceMeasureGapInspectionUpper = AlgorithmParam.m_dDistanceMeasureGapInspectionUpper;
	m_dEditDistanceMeasureGapInspectionLower = AlgorithmParam.m_dDistanceMeasureGapInspectionLower;
	m_iRadioDistanceMeasureROIType = AlgorithmParam.m_iDistanceMeasureROIType;

	UpdateData(FALSE);
}

void CAlgorithmDistanceMeasureDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseDistanceMeasure = m_bCheckUseDistanceMeasure;
	pAlgorithmParam->m_iDistanceMeasureUpperGv = m_iRadioDistanceMeasureUpperGv;
	pAlgorithmParam->m_iDistanceMeasureUpperPos = m_iRadioDistanceMeasureUpperPos;
	pAlgorithmParam->m_dDistanceMeasureUpperSmFactor = m_dEditDistanceMeasureUpperSmFactor;
	pAlgorithmParam->m_iDistanceMeasureUpperEdgeStr = m_iEditDistanceMeasureUpperEdgeStr;
	pAlgorithmParam->m_iDistanceMeasureLowerGv = m_iRadioDistanceMeasureLowerGv;
	pAlgorithmParam->m_iDistanceMeasureLowerPos = m_iRadioDistanceMeasureLowerPos;
	pAlgorithmParam->m_dDistanceMeasureLowerSmFactor = m_dEditDistanceMeasureLowerSmFactor;
	pAlgorithmParam->m_iDistanceMeasureLowerEdgeStr = m_iEditDistanceMeasureLowerEdgeStr;
	pAlgorithmParam->m_iDistanceMeasureUpperPosOffset = m_iEditDistanceMeasureUpperPosOffset;
	pAlgorithmParam->m_iDistanceMeasureLowerPosOffset = m_iEditDistanceMeasureLowerPosOffset;
	pAlgorithmParam->m_iDistanceMeasureUsageType = m_iRadioDistanceMeasureUsageType;
	pAlgorithmParam->m_iDistanceMeasureSaveID = m_iEditDistanceMeasureSaveID;
	pAlgorithmParam->m_dDistanceMeasureGapInspectionUpper = m_dEditDistanceMeasureGapInspectionUpper;
	pAlgorithmParam->m_dDistanceMeasureGapInspectionLower = m_dEditDistanceMeasureGapInspectionLower;
	pAlgorithmParam->m_iDistanceMeasureROIType = m_iRadioDistanceMeasureROIType;
}


BOOL CAlgorithmDistanceMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAlgorithmDistanceMeasureDlg::OnBnClickedRadioDistanceMeasureUsageTypeDistance()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	GetDlgItem(IDC_RADIO_DISTANCE_MEASURE_ROI_TYPE_ONE)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_DISTANCE_MEASURE_ROI_TYPE_TWO)->EnableWindow(TRUE);
}


void CAlgorithmDistanceMeasureDlg::OnBnClickedRadioDistanceMeasureUsageTypeRoi()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iRadioDistanceMeasureROIType = 0;
	UpdateData(FALSE);

	GetDlgItem(IDC_RADIO_DISTANCE_MEASURE_ROI_TYPE_ONE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_DISTANCE_MEASURE_ROI_TYPE_TWO)->EnableWindow(FALSE);
}
