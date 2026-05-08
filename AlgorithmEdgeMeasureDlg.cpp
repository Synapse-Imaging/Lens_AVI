// AlgorithmEdgeMeasureDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmEdgeMeasureDlg.h"
#include "afxdialogex.h"


// CAlgorithmEdgeMeasureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmEdgeMeasureDlg, CDialog)

CAlgorithmEdgeMeasureDlg::CAlgorithmEdgeMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmEdgeMeasureDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseEdgeMeasure = FALSE;
	m_iRadioEdgeMeasureOnePoint = 0;
	m_iEditEdgeMeasureMultiPointNumber = 8;
	m_bCheckEdgeMeasureEndPoint = FALSE;
	m_bCheckEdgeMeasureSlope = FALSE;
	m_bCheckEdgeMeasureReverseGv = FALSE;
	m_bCheckEdgeMeasureFixedEndPoint = FALSE;
	m_iEditEdgeMeasureLeftSideSize = 10;
	m_iEditEdgeMeasureRightSideSize = 10;
	m_iRadioEdgeMeasureDir = 0;
	m_iRadioEdgeMeasureGv = 0;
	m_iRadioEdgeMeasurePos = 0;
	m_bCheckEdgeMeasureLocalAlignUse = FALSE;
	m_dEditEdgeMeasureSmFactor = 1.0;
	m_iEditEdgeMeasureEdgeStr = 10;
	m_iEditEdgeMeasurePosOffset = 0;
	m_bCheckEdgeMeasureUseMaxMinPoint = FALSE;
	m_iEditEdgeMeasurePositionRange = 10;

	m_iRadioEdgeMeasureUseMaxPoint = 0;

	m_bCheckEdgeMeasureUseOther = FALSE;
	m_bCheckEdgeMeasureMakeROIOther = FALSE;
	m_iEditEdgeMeasureMakeROIRangeOther = 0;
	m_iRadioEdgeMeasureGvOther = 0;
	m_iRadioEdgeMeasurePosOther = 0;
	m_dEditEdgeMeasureSmFactorOther = 1.0;
	m_iEditEdgeMeasureEdgeStrOther = 10;
	m_iEditEdgeMeasurePosOffsetOther = 0;
	m_dEditEdgeMeasureAngleDeg = 45;
}

CAlgorithmEdgeMeasureDlg::~CAlgorithmEdgeMeasureDlg()
{
}

void CAlgorithmEdgeMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_EDGE_MEASURE, m_bCheckUseEdgeMeasure);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_ONE_POINT, m_iRadioEdgeMeasureOnePoint);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_MULTI_POINT_NUMBER, m_iEditEdgeMeasureMultiPointNumber);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_END_POINT, m_bCheckEdgeMeasureEndPoint);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_USE_SLOPE, m_bCheckEdgeMeasureSlope);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_USE_REVERSE_GV, m_bCheckEdgeMeasureReverseGv);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT, m_bCheckEdgeMeasureFixedEndPoint);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_LEFT_SIDE_SIZE, m_iEditEdgeMeasureLeftSideSize);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_RIGHT_SIDE_SIZE, m_iEditEdgeMeasureRightSideSize);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_DIR_X, m_iRadioEdgeMeasureDir);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_GV_ANY, m_iRadioEdgeMeasureGv);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_POS_BEST, m_iRadioEdgeMeasurePos);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_LOCAL_ALIGN_USE, m_bCheckEdgeMeasureLocalAlignUse);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_SMOOTHING_FACTOR, m_dEditEdgeMeasureSmFactor);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_EDGE_STRENGTH, m_iEditEdgeMeasureEdgeStr);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_POS_OFFSET, m_iEditEdgeMeasurePosOffset);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT, m_bCheckEdgeMeasureUseMaxMinPoint);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_POSITION_RANGE, m_iEditEdgeMeasurePositionRange);

	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_USE_OTHER_IMAGE, m_bCheckEdgeMeasureUseOther);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_MAKE_ROI_FROM_OTHER_IMAGE, m_bCheckEdgeMeasureMakeROIOther);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_MAKE_ROI_RANGE, m_iEditEdgeMeasureMakeROIRangeOther);
	DDX_Control(pDX, IDC_COMBO_EDGE_MEASURE_OTHER_IMAGE, m_cbEdgeMeasureImageIndexOther);

	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_GV_ANY2, m_iRadioEdgeMeasureGvOther);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_POS_BEST2, m_iRadioEdgeMeasurePosOther);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_SMOOTHING_FACTOR2, m_dEditEdgeMeasureSmFactorOther);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_EDGE_STRENGTH2, m_iEditEdgeMeasureEdgeStrOther);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_POS_OFFSET2, m_iEditEdgeMeasurePosOffsetOther);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_ANGLE_DEG, m_dEditEdgeMeasureAngleDeg);

	DDX_Control(pDX, IDC_COMBO_EDGE_MEASURE_ACCURACY, m_cbEdgeMeasureAccuracy);
}


BEGIN_MESSAGE_MAP(CAlgorithmEdgeMeasureDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_EDGE_MEASURE_ONE_POINT, &CAlgorithmEdgeMeasureDlg::OnBnClickedRadioEdgeMeasureOnePoint)
	ON_BN_CLICKED(IDC_RADIO_EDGE_MEASURE_MULTI_POINT, &CAlgorithmEdgeMeasureDlg::OnBnClickedRadioEdgeMeasureMultiPoint)
	ON_BN_CLICKED(IDC_CHECK_EDGE_MEASURE_END_POINT, &CAlgorithmEdgeMeasureDlg::OnBnClickedCheckEdgeMeasureEndPoint)
END_MESSAGE_MAP()


// CAlgorithmEdgeMeasureDlg 메시지 처리기입니다.

void CAlgorithmEdgeMeasureDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmEdgeMeasureDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseEdgeMeasure = AlgorithmParam.m_bUseEdgeMeasure;
	m_iRadioEdgeMeasureOnePoint = AlgorithmParam.m_iEdgeMeasureOnePoint;
	m_iRadioEdgeMeasureUseMaxPoint = AlgorithmParam.m_iEdgeMeasureUseMaxPoint;
	m_iEditEdgeMeasureMultiPointNumber = AlgorithmParam.m_iEdgeMeasureMultiPointNumber;
	m_bCheckEdgeMeasureEndPoint = AlgorithmParam.m_bEdgeMeasureEndPoint;
	m_bCheckEdgeMeasureSlope = AlgorithmParam.m_bEdgeMeasureSlope;
	m_bCheckEdgeMeasureReverseGv = AlgorithmParam.m_bEdgeMeasureReverseGv;
	m_bCheckEdgeMeasureFixedEndPoint = AlgorithmParam.m_bEdgeMeasureFixedEndPoint;
	m_iEditEdgeMeasureLeftSideSize = AlgorithmParam.m_iEdgeMeasureLeftSideSize;
	m_iEditEdgeMeasureRightSideSize = AlgorithmParam.m_iEdgeMeasureRightSideSize;
	m_iRadioEdgeMeasureDir = AlgorithmParam.m_iEdgeMeasureDir;
	m_iRadioEdgeMeasureGv = AlgorithmParam.m_iEdgeMeasureGv;
	m_iRadioEdgeMeasurePos = AlgorithmParam.m_iEdgeMeasurePos;
	m_bCheckEdgeMeasureLocalAlignUse = AlgorithmParam.m_bEdgeMeasureLocalAlignUse;
	m_dEditEdgeMeasureSmFactor = AlgorithmParam.m_dEdgeMeasureSmFactor;
	m_iEditEdgeMeasureEdgeStr = AlgorithmParam.m_iEdgeMeasureEdgeStr;
	m_iEditEdgeMeasurePosOffset = AlgorithmParam.m_iEdgeMeasurePosOffset;
	m_bCheckEdgeMeasureUseMaxMinPoint = AlgorithmParam.m_bEdgeMeasureUseMaxMinPoint;
	m_iEditEdgeMeasurePositionRange = AlgorithmParam.m_iEdgeMeasurePositionRange;

	m_bCheckEdgeMeasureUseOther = AlgorithmParam.m_bEdgeMeasureUseOther;
	m_bCheckEdgeMeasureMakeROIOther = AlgorithmParam.m_bEdgeMeasureMakeROIOther;
	m_iEditEdgeMeasureMakeROIRangeOther = AlgorithmParam.m_iEdgeMeasureMakeROIRangeOther;
	m_cbEdgeMeasureImageIndexOther.SetCurSel(AlgorithmParam.m_iEdgeMeasureImageIndexOther);
	m_iRadioEdgeMeasureGvOther = AlgorithmParam.m_iEdgeMeasureGvOther;
	m_iRadioEdgeMeasurePosOther = AlgorithmParam.m_iEdgeMeasurePosOther;
	m_dEditEdgeMeasureSmFactorOther = AlgorithmParam.m_dEdgeMeasureSmFactorOther;
	m_iEditEdgeMeasureEdgeStrOther = AlgorithmParam.m_iEdgeMeasureEdgeStrOther;
	m_iEditEdgeMeasurePosOffsetOther = AlgorithmParam.m_iEdgeMeasurePosOffsetOther;
	m_dEditEdgeMeasureAngleDeg = AlgorithmParam.m_dEdgeMeasureAngleDeg;

	m_cbEdgeMeasureAccuracy.SetCurSel(AlgorithmParam.m_iEdgeMeasureAccuracy);

	if (m_iRadioEdgeMeasureOnePoint == 0 || m_bCheckEdgeMeasureEndPoint == 1)
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT)->EnableWindow(TRUE);
	}

	UpdateData(FALSE);
}

void CAlgorithmEdgeMeasureDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseEdgeMeasure = m_bCheckUseEdgeMeasure;
	pAlgorithmParam->m_iEdgeMeasureOnePoint = m_iRadioEdgeMeasureOnePoint;
	pAlgorithmParam->m_iEdgeMeasureMultiPointNumber = m_iEditEdgeMeasureMultiPointNumber;
	pAlgorithmParam->m_bEdgeMeasureEndPoint = m_bCheckEdgeMeasureEndPoint;
	pAlgorithmParam->m_bEdgeMeasureSlope = m_bCheckEdgeMeasureSlope;
	pAlgorithmParam->m_bEdgeMeasureReverseGv = m_bCheckEdgeMeasureReverseGv;
	pAlgorithmParam->m_bEdgeMeasureFixedEndPoint = m_bCheckEdgeMeasureFixedEndPoint;
	pAlgorithmParam->m_iEdgeMeasureLeftSideSize = m_iEditEdgeMeasureLeftSideSize;
	pAlgorithmParam->m_iEdgeMeasureRightSideSize = m_iEditEdgeMeasureRightSideSize;
	pAlgorithmParam->m_iEdgeMeasureDir = m_iRadioEdgeMeasureDir;
	pAlgorithmParam->m_iEdgeMeasureGv = m_iRadioEdgeMeasureGv;
	pAlgorithmParam->m_iEdgeMeasurePos = m_iRadioEdgeMeasurePos;
	pAlgorithmParam->m_bEdgeMeasureLocalAlignUse = m_bCheckEdgeMeasureLocalAlignUse;
	pAlgorithmParam->m_dEdgeMeasureSmFactor = m_dEditEdgeMeasureSmFactor;
	pAlgorithmParam->m_iEdgeMeasureEdgeStr = m_iEditEdgeMeasureEdgeStr;
	pAlgorithmParam->m_iEdgeMeasurePosOffset = m_iEditEdgeMeasurePosOffset;
	pAlgorithmParam->m_bEdgeMeasureUseMaxMinPoint = m_bCheckEdgeMeasureUseMaxMinPoint;
	pAlgorithmParam->m_iEdgeMeasurePositionRange = m_iEditEdgeMeasurePositionRange;
	pAlgorithmParam->m_iEdgeMeasureUseMaxPoint = m_iRadioEdgeMeasureUseMaxPoint;

	pAlgorithmParam->m_bEdgeMeasureUseOther = m_bCheckEdgeMeasureUseOther;
	pAlgorithmParam->m_bEdgeMeasureMakeROIOther = m_bCheckEdgeMeasureMakeROIOther;
	pAlgorithmParam->m_iEdgeMeasureMakeROIRangeOther = m_iEditEdgeMeasureMakeROIRangeOther;
	pAlgorithmParam->m_iEdgeMeasureImageIndexOther = m_cbEdgeMeasureImageIndexOther.GetCurSel();
	pAlgorithmParam->m_iEdgeMeasureGvOther = m_iRadioEdgeMeasureGvOther;
	pAlgorithmParam->m_iEdgeMeasurePosOther = m_iRadioEdgeMeasurePosOther;
	pAlgorithmParam->m_dEdgeMeasureSmFactorOther = m_dEditEdgeMeasureSmFactorOther;
	pAlgorithmParam->m_iEdgeMeasureEdgeStrOther = m_iEditEdgeMeasureEdgeStrOther;
	pAlgorithmParam->m_iEdgeMeasurePosOffsetOther = m_iEditEdgeMeasurePosOffsetOther;
	pAlgorithmParam->m_dEdgeMeasureAngleDeg = m_dEditEdgeMeasureAngleDeg;

	pAlgorithmParam->m_iEdgeMeasureAccuracy = m_cbEdgeMeasureAccuracy.GetCurSel();

	if (m_iRadioEdgeMeasureOnePoint == 0 || m_bCheckEdgeMeasureEndPoint == 1)
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT)->EnableWindow(TRUE);
	}
}


BOOL CAlgorithmEdgeMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	int i;
	CString sTemp;

	m_cbEdgeMeasureImageIndexOther.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbEdgeMeasureImageIndexOther.AddString(sTemp);
	}

	m_cbEdgeMeasureImageIndexOther.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAlgorithmEdgeMeasureDlg::OnBnClickedRadioEdgeMeasureOnePoint()
{
	UpdateData();
	UpdateData(FALSE);
	if (m_iRadioEdgeMeasureOnePoint == 0 || m_bCheckEdgeMeasureEndPoint == 1)
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT)->EnableWindow(TRUE);
	}
}


void CAlgorithmEdgeMeasureDlg::OnBnClickedRadioEdgeMeasureMultiPoint()
{
	UpdateData();
	UpdateData(FALSE);
	if (m_iRadioEdgeMeasureOnePoint == 0 || m_bCheckEdgeMeasureEndPoint == 1)
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT)->EnableWindow(TRUE);
	}
}

void CAlgorithmEdgeMeasureDlg::OnBnClickedCheckEdgeMeasureEndPoint()
{
	UpdateData(TRUE);
	if (m_iRadioEdgeMeasureOnePoint == 0 || m_bCheckEdgeMeasureEndPoint == 1)
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_USE_MAXMIN_POINT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_FIXED_END_POINT)->EnableWindow(TRUE);
	}
}
