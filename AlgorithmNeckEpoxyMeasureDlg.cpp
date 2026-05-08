// AlgorithmNeckEpoxyMeasureDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmNeckEpoxyMeasureDlg.h"
#include "afxdialogex.h"


// CAlgorithmNeckEpoxyMeasureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmNeckEpoxyMeasureDlg, CDialog)

CAlgorithmNeckEpoxyMeasureDlg::CAlgorithmNeckEpoxyMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmNeckEpoxyMeasureDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseNeckEpoxyMeasure = FALSE;
	m_iEditNeckEpoxyMeasureTopEdgeStr = 5;
	m_iEditNeckEpoxyMeasureBottomEdgeStr = 5;
	m_iEditNeckEpoxyMeasureRightEdgeStr = 10;

	m_iEditNeckEpoxyToleranceHeightMax = 0;
	m_iEditNeckEpoxyToleranceHeightMin = 0;
	m_iEditNeckEpoxyToleranceLength = 0;
	m_iEditNeckEpoxyToleranceTopPosOffset = 0;
	m_iEditNeckEpoxyToleranceBottomPosOffset = 0;

	m_iRadioNeckEpoxyInspectMethod = NECK_EPOXY_INSPECT_EDGE;
}

CAlgorithmNeckEpoxyMeasureDlg::~CAlgorithmNeckEpoxyMeasureDlg()
{
}

void CAlgorithmNeckEpoxyMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_NECK_EPOXY_MEASURE, m_bCheckUseNeckEpoxyMeasure);
	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_MEASURE_TOP_EDGE_STRENGTH, m_iEditNeckEpoxyMeasureTopEdgeStr);
	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_MEASURE_BOTTOM_EDGE_STRENGTH, m_iEditNeckEpoxyMeasureBottomEdgeStr);
	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_MEASURE_RIGHT_EDGE_STRENGTH, m_iEditNeckEpoxyMeasureRightEdgeStr);

	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_TOLERANCE_HEIGHT_MAX, m_iEditNeckEpoxyToleranceHeightMax);
	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_TOLERANCE_HEIGHT_MIN, m_iEditNeckEpoxyToleranceHeightMin);
	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_TOLERANCE_LENGTH, m_iEditNeckEpoxyToleranceLength);
	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_TOLERANCE_TOP_POS_OFFSET, m_iEditNeckEpoxyToleranceTopPosOffset);
	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_TOLERANCE_BOTTOM_POS_OFFSET, m_iEditNeckEpoxyToleranceBottomPosOffset);

	DDX_Radio(pDX, IDC_RADIO_NECK_EPOXY_INSPECT_METHOD_EDGE, m_iRadioNeckEpoxyInspectMethod);
}


BEGIN_MESSAGE_MAP(CAlgorithmNeckEpoxyMeasureDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmNeckEpoxyMeasureDlg 메시지 처리기입니다.

void CAlgorithmNeckEpoxyMeasureDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmNeckEpoxyMeasureDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseNeckEpoxyMeasure = AlgorithmParam.m_bUseNeckEpoxyMeasure;
	m_iEditNeckEpoxyMeasureTopEdgeStr = AlgorithmParam.m_iNeckEpoxyMeasureTopEdgeStr;
	m_iEditNeckEpoxyMeasureBottomEdgeStr = AlgorithmParam.m_iNeckEpoxyMeasureBottomEdgeStr;
	m_iEditNeckEpoxyMeasureRightEdgeStr = AlgorithmParam.m_iNeckEpoxyMeasureRightEdgeStr;

	m_iEditNeckEpoxyToleranceHeightMax = AlgorithmParam.m_iNeckEpoxyToleranceHeightMax;
	m_iEditNeckEpoxyToleranceHeightMin = AlgorithmParam.m_iNeckEpoxyToleranceHeightMin;
	m_iEditNeckEpoxyToleranceLength = AlgorithmParam.m_iNeckEpoxyToleranceLength;
	m_iEditNeckEpoxyToleranceTopPosOffset = AlgorithmParam.m_iNeckEpoxyToleranceTopPosOffset;
	m_iEditNeckEpoxyToleranceBottomPosOffset = AlgorithmParam.m_iNeckEpoxyToleranceBottomPosOffset;

	m_iRadioNeckEpoxyInspectMethod = AlgorithmParam.m_iNeckEpoxyInspectMethod;

	UpdateData(FALSE);
}

void CAlgorithmNeckEpoxyMeasureDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseNeckEpoxyMeasure = m_bCheckUseNeckEpoxyMeasure;
	pAlgorithmParam->m_iNeckEpoxyMeasureTopEdgeStr = m_iEditNeckEpoxyMeasureTopEdgeStr;
	pAlgorithmParam->m_iNeckEpoxyMeasureBottomEdgeStr = m_iEditNeckEpoxyMeasureBottomEdgeStr;
	pAlgorithmParam->m_iNeckEpoxyMeasureRightEdgeStr = m_iEditNeckEpoxyMeasureRightEdgeStr;

	pAlgorithmParam->m_iNeckEpoxyToleranceHeightMax = m_iEditNeckEpoxyToleranceHeightMax;
	pAlgorithmParam->m_iNeckEpoxyToleranceHeightMin = m_iEditNeckEpoxyToleranceHeightMin;
	pAlgorithmParam->m_iNeckEpoxyToleranceLength = m_iEditNeckEpoxyToleranceLength;
	pAlgorithmParam->m_iNeckEpoxyToleranceTopPosOffset = m_iEditNeckEpoxyToleranceTopPosOffset;
	pAlgorithmParam->m_iNeckEpoxyToleranceBottomPosOffset = m_iEditNeckEpoxyToleranceBottomPosOffset;

	pAlgorithmParam->m_iNeckEpoxyInspectMethod = m_iRadioNeckEpoxyInspectMethod;
}


BOOL CAlgorithmNeckEpoxyMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
