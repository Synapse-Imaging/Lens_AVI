// AlgorithmCornerMeasureDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmCornerMeasureDlg.h"
#include "afxdialogex.h"


// CAlgorithmCornerMeasureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmCornerMeasureDlg, CDialog)

CAlgorithmCornerMeasureDlg::CAlgorithmCornerMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmCornerMeasureDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseCornerMeasure = FALSE;
	m_iEditCornerMeasureSaveID = 1;
	m_iEditCornerMeasureMultiPointNumber = 8;
	m_iEditCornerMeasurePositionRange = 10;
	m_iRadioCornerMeasureGv[0] = m_iRadioCornerMeasureGv[1] = 0;
	m_iRadioCornerMeasurePos[0] = m_iRadioCornerMeasurePos[1] = 0;
	m_dEditCornerMeasureSmFactor[0] = m_dEditCornerMeasureSmFactor[1] = 1.0;
	m_iEditCornerMeasureEdgeStr[0] = m_iEditCornerMeasureEdgeStr[1] = 10;
	m_iEditCornerMeasurePosOffset[0] = m_iEditCornerMeasurePosOffset[1] = 0;

	m_dEditCornerMeasureTargetSmFactor = 1.0;
	m_iEditCornerMeasureTargetEdgeStr = 10;
	m_dEditCornerMeasureTargetDistanceSpec = 0.23;
}

CAlgorithmCornerMeasureDlg::~CAlgorithmCornerMeasureDlg()
{
}

void CAlgorithmCornerMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_CORNER_MEASURE, m_bCheckUseCornerMeasure);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_SAVE_ID, m_iEditCornerMeasureSaveID);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_MULTI_POINT_NUMBER, m_iEditCornerMeasureMultiPointNumber);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_POSITION_RANGE, m_iEditCornerMeasurePositionRange);
	DDX_Radio(pDX, IDC_RADIO_CORNER_MEASURE_GV_ANY_1, m_iRadioCornerMeasureGv[0]);
	DDX_Radio(pDX, IDC_RADIO_CORNER_MEASURE_POS_BEST_1, m_iRadioCornerMeasurePos[0]);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_SMOOTHING_FACTOR_1, m_dEditCornerMeasureSmFactor[0]);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_EDGE_STRENGTH_1, m_iEditCornerMeasureEdgeStr[0]);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_POS_OFFSET_1, m_iEditCornerMeasurePosOffset[0]);
	DDX_Radio(pDX, IDC_RADIO_CORNER_MEASURE_GV_ANY_2, m_iRadioCornerMeasureGv[1]);
	DDX_Radio(pDX, IDC_RADIO_CORNER_MEASURE_POS_BEST_2, m_iRadioCornerMeasurePos[1]);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_SMOOTHING_FACTOR_2, m_dEditCornerMeasureSmFactor[1]);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_EDGE_STRENGTH_2, m_iEditCornerMeasureEdgeStr[1]);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_POS_OFFSET_2, m_iEditCornerMeasurePosOffset[1]);

	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_SMOOTHING_FACTOR_CORNER, m_dEditCornerMeasureTargetSmFactor);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_EDGE_STRENGTH_CORNER, m_iEditCornerMeasureTargetEdgeStr);
	DDX_Text(pDX, IDC_EDIT_CORNER_MEASURE_DISTANCE_SPEC, m_dEditCornerMeasureTargetDistanceSpec);
}


BEGIN_MESSAGE_MAP(CAlgorithmCornerMeasureDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmCornerMeasureDlg 메시지 처리기입니다.

void CAlgorithmCornerMeasureDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmCornerMeasureDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseCornerMeasure = AlgorithmParam.m_bUseCornerMeasure;
	m_iEditCornerMeasureSaveID = AlgorithmParam.m_iCornerMeasureSaveID;
	m_iEditCornerMeasureMultiPointNumber = AlgorithmParam.m_iCornerMeasureMultiPointNumber;
	m_iEditCornerMeasurePositionRange = AlgorithmParam.m_iCornerMeasurePositionRange;

	for (int i = 0; i < 2; i++)
	{
		m_iRadioCornerMeasureGv[i] = AlgorithmParam.m_iCornerMeasureGv[i];
		m_iRadioCornerMeasurePos[i] = AlgorithmParam.m_iCornerMeasurePos[i];
		m_dEditCornerMeasureSmFactor[i] = AlgorithmParam.m_dCornerMeasureSmFactor[i];
		m_iEditCornerMeasureEdgeStr[i] = AlgorithmParam.m_iCornerMeasureEdgeStr[i];
		m_iEditCornerMeasurePosOffset[i] = AlgorithmParam.m_iCornerMeasurePosOffset[i];
	}

	m_dEditCornerMeasureTargetSmFactor = AlgorithmParam.m_dCornerMeasureTargetSmFactor;
	m_iEditCornerMeasureTargetEdgeStr = AlgorithmParam.m_iCornerMeasureTargetEdgeStr;
	m_dEditCornerMeasureTargetDistanceSpec = AlgorithmParam.m_dCornerMeasureTargetDistanceSpec;

	UpdateData(FALSE);
}

void CAlgorithmCornerMeasureDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseCornerMeasure = m_bCheckUseCornerMeasure;
	pAlgorithmParam->m_iCornerMeasureSaveID = m_iEditCornerMeasureSaveID;
	pAlgorithmParam->m_iCornerMeasureMultiPointNumber = m_iEditCornerMeasureMultiPointNumber;
	pAlgorithmParam->m_iCornerMeasurePositionRange = m_iEditCornerMeasurePositionRange;

	for (int i = 0; i < 2; i++)
	{
		pAlgorithmParam->m_iCornerMeasureGv[i] = m_iRadioCornerMeasureGv[i];
		pAlgorithmParam->m_iCornerMeasurePos[i] = m_iRadioCornerMeasurePos[i];
		pAlgorithmParam->m_dCornerMeasureSmFactor[i] = m_dEditCornerMeasureSmFactor[i];
		pAlgorithmParam->m_iCornerMeasureEdgeStr[i] = m_iEditCornerMeasureEdgeStr[i];
		pAlgorithmParam->m_iCornerMeasurePosOffset[i] = m_iEditCornerMeasurePosOffset[i];
	}

	pAlgorithmParam->m_dCornerMeasureTargetSmFactor = m_dEditCornerMeasureTargetSmFactor;
	pAlgorithmParam->m_iCornerMeasureTargetEdgeStr = m_iEditCornerMeasureTargetEdgeStr;
	pAlgorithmParam->m_dCornerMeasureTargetDistanceSpec = m_dEditCornerMeasureTargetDistanceSpec;
}


BOOL CAlgorithmCornerMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
