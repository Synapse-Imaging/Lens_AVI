// AlgorithmDefectConditionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmDefectConditionDlg.h"
#include "afxdialogex.h"


// CAlgorithmDefectConditionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmDefectConditionDlg, CDialog)

CAlgorithmDefectConditionDlg::CAlgorithmDefectConditionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmDefectConditionDlg::IDD, pParent)
	, m_bCheckDefectConditionOuterDiff(FALSE)
	, m_nEditDefectConditionOuterDiffInterval(5)
	, m_nEditDefectConditionOuterDiffMinDiff(3)
	, m_nRadioOuterDiffUpper(0)
	, m_nEditDefectConditionOuterDiffMaxRepeat(2)
	, m_nRadioConerLengthDir(0)
	, m_bCheckConerLengthUse(FALSE)
	, m_nEditDefectConditionConerLengthMin(1)
	, m_nEditDefectConditionConerLengthMax(65535)
	, m_bCheckConerLengthMaxUse(FALSE)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckCombineBrightDarkBlob = FALSE;
	m_bCheckMaxBlob = FALSE;
	m_iEditOpeningSize = 0;
	m_iEditClosingSize = 0;
	m_iEditThresholdOpeningSize = 0;
	m_iEditThresholdClosingSize = 0;
	m_bCheckFillup = FALSE;
	m_bCheckConvex = FALSE;
	m_bCheckSubConvex = FALSE;
	m_bCheckRect = FALSE;
	m_bCheckUseConnection = FALSE;
	m_iEditConnectionMinSize = 0;
	m_iEditConnectionMinXLength = 0;
	m_iEditConnectionLength = 0;
	m_bCheckUseAngle = FALSE;
	m_iEditConnectionMaxWidth = 0;

	m_iEditDefectConditionAreaMin = 0;
	m_iEditDefectConditionLengthMin = 0;
	m_iEditDefectConditionWidthMin = 0;
	m_iEditDefectConditionXLengthMin = 0;
	m_iEditDefectConditionYLengthMin = 0;
	m_iEditDefectConditionMeanMin = 0;
	m_dEditDefectConditionStdevMin = 0;
	m_dEditDefectConditionAnisometryMin = 0;
	m_dEditDefectConditionCircularityMin = 0;
	m_dEditDefectConditionRectangularityMin = 0;
	m_dEditDefectConditionConvexityMin = 0;
	m_dEditDefectConditionEllipseRatioMin = 0;
	m_iEditDefectConditionAreaRatioMin = 0;
	m_iEditDefectConditionContrastMin = 10;
	m_iEditDefectConditionGradMin = 20;
	m_iEditDefectConditionBlobMin = 1;
	m_iEditDefectConditionShiftXMin = 0;
	m_iEditDefectConditionShiftYMin = 0;

	m_iEditDefectConditionAreaMax = 0;
	m_iEditDefectConditionLengthMax = 0;
	m_iEditDefectConditionWidthMax = 0;
	m_iEditDefectConditionXLengthMax = 0;
	m_iEditDefectConditionYLengthMax = 0;
	m_iEditDefectConditionMeanMax = 0;
	m_dEditDefectConditionStdevMax = 0;
	m_dEditDefectConditionAnisometryMax = 0;
	m_dEditDefectConditionCircularityMax = 0;
	m_dEditDefectConditionRectangularityMax = 0;
	m_dEditDefectConditionConvexityMax = 0;
	m_dEditDefectConditionEllipseRatioMax = 0;
	m_iEditDefectConditionAreaRatioMax = 50;
	m_iEditDefectConditionContrastMax = 255;
	m_iEditDefectConditionGradMax = 255;
	m_iEditDefectConditionBlobMax = 1000;
	m_iEditDefectConditionShiftXMax = 1000;
	m_iEditDefectConditionShiftYMax = 1000;

	m_iEditDefectConditionOuterDist = 0;
	m_iEditDefectConditionInnerDist = 0;

	m_iEditDefectConditionBoundaryMarginTop = 5;
	m_iEditDefectConditionBoundaryMarginBottom = 5;
	m_iEditDefectConditionBoundaryMarginLeft = 5;
	m_iEditDefectConditionBoundaryMarginRight = 5;
	m_iEditDefectConditionBoundaryMarginMinConnection = 1;

	m_iEditCandidateMinArea = 1;
	m_bCheckClosingOpening = FALSE;
	m_bCheckApplyOneDirMorp = FALSE;
	m_iRadioOneDirMorpXOpening = 0;
	m_iRadioOneDirMorpXClosing = 0;
	m_iRadioLengthWidthCondition = 0;
	m_iRadioXYLengthCondition = 0;

	m_iRadioBoundaryConnectionCondition = 0;
	m_bCheckDefectConditionBoundaryConnection2Side = FALSE;
	m_bCheckDefectConditionInnerOuterBoundaryConnection = FALSE;

	m_iRadioXLengthCondition = LENGTH_MAX;
	m_iRadioXLengthConditionMinRef = LENGTH_MIN_LEFT;
	m_iRadioYLengthCondition = LENGTH_MAX;
	m_iRadioYLengthConditionMinRef = LENGTH_MIN_BOTTOM;

	m_iRadioHoleFillCondition = HOLE_FILL_ALL;
	m_iEditFillHoleArea = 20;

	m_bCheckXYLengthMeasureIgnoreHole = FALSE;
	m_bCheckXYLengthMaxLineLength = FALSE;

	m_bCheckDefectConditionBoundaryConnectionAny = FALSE;
	m_iRadioBoundaryConnectionOuter = ROI_CONNECTION_OUTER;
	m_iEditDefectConditionBoundaryMarginAny = 5;
	m_iEditDefectConditionBoundaryMarginMinConnectionAny = 1;

	m_bCheckDefectConditionHole = FALSE;
	m_iEditDefectConditionHoleArea = 5;

	m_bCheckDefectConditionUnionBlob = FALSE;
	m_bCheckDefectConditionDilation = FALSE;
	m_iEditDefectConditionDilationSize = 0;

	m_bCheckDefectConditionXLengthSum = FALSE;
	m_iEditDefectConditionXLengthSumMin = 0;
	m_iEditDefectConditionXLengthSumMax = 0;
	m_bCheckDefectConditionYLengthSum = FALSE;
	m_iEditDefectConditionYLengthSumMin = 0;
	m_iEditDefectConditionYLengthSumMax = 0;

	m_bCheckDefectConditionOuterDiff = FALSE;
	m_nEditDefectConditionOuterDiffInterval = 5;
	m_nEditDefectConditionOuterDiffMinDiff = 3;
	m_nRadioOuterDiffUpper = 0;
	m_nEditDefectConditionOuterDiffMaxRepeat = 2;

	m_nRadioConerLengthDir = 0;
	m_bCheckConerLengthUse = FALSE;
	m_nEditDefectConditionConerLengthMin = 1;
	m_nEditDefectConditionConerLengthMax = 65535;

	m_bCheckDefectConditionOrthoLength = FALSE;
	m_iRadioOrthoLengthRefSide = ORTHO_LENGTH_TOP;
}

CAlgorithmDefectConditionDlg::~CAlgorithmDefectConditionDlg()
{
}

void CAlgorithmDefectConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_COMBINE_BLOB, m_bCheckCombineBrightDarkBlob);
	DDX_Check(pDX, IDC_CHECK_MAX_BLOB, m_bCheckMaxBlob);
	DDX_Text(pDX, IDC_EDIT_OPENING_SIZE, m_iEditOpeningSize);
	DDX_Text(pDX, IDC_EDIT_CLOSING_SIZE, m_iEditClosingSize);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD_OPENING_SIZE, m_iEditThresholdOpeningSize);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD_CLOSING_SIZE, m_iEditThresholdClosingSize);
	DDX_Check(pDX, IDC_CHECK_FILL_UP, m_bCheckFillup);
	DDX_Check(pDX, IDC_CHECK_CONVEX, m_bCheckConvex);
	DDX_Check(pDX, IDC_CHECK_SUB_CONVEX, m_bCheckSubConvex);
	DDX_Check(pDX, IDC_CHECK_RECT, m_bCheckRect);
	DDX_Check(pDX, IDC_CHECK_USE_CONNECTION, m_bCheckUseConnection);
	DDX_Text(pDX, IDC_EDIT_CONNECT_MIN_SIZE, m_iEditConnectionMinSize);
	DDX_Text(pDX, IDC_EDIT_CONNECT_MIN_X_LENGTH, m_iEditConnectionMinXLength);
	DDX_Text(pDX, IDC_EDIT_CONNECT_LENGTH, m_iEditConnectionLength);
	DDX_Check(pDX, IDC_CHECK_USE_ANGLE, m_bCheckUseAngle);
	DDX_Text(pDX, IDC_EDIT_CONNECT_MAX_WIDTH, m_iEditConnectionMaxWidth);

	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_AREA, m_bCheckDefectConditionArea);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_LENGTH, m_bCheckDefectConditionLength);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_WIDTH, m_bCheckDefectConditionWidth);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_X_LENGTH, m_bCheckDefectConditionXLength);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_Y_LENGTH, m_bCheckDefectConditionYLength);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_MEAN, m_bCheckDefectConditionMean);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_STDEV, m_bCheckDefectConditionStdev);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_ANISOMETRY, m_bCheckDefectConditionAnisometry);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_CIRCULARITY, m_bCheckDefectConditionCircularity);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_RECTANGULARITY, m_bCheckDefectConditionRectangularity);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_CONVEXITY, m_bCheckDefectConditionConvexity);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_ELLIPSE_RATIO, m_bCheckDefectConditionEllipseRatio);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_AREA_RATIO, m_bCheckDefectConditionAreaRatio);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_CONTRAST, m_bCheckDefectConditionContrast);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_GRAD, m_bCheckDefectConditionGrad);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BLOB, m_bCheckDefectConditionBlob);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_SHIFT_X, m_bCheckDefectConditionShiftX);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_SHIFT_Y, m_bCheckDefectConditionShiftY);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_OUTER_DIST, m_bCheckDefectConditionOuterDist);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_INNER_DIST, m_bCheckDefectConditionInnerDist);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_USE_ZERO_AREA, m_bCheckDefectConditionUseZeroArea);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BOUNDARY_CONNECTION, m_bCheckDefectConditionBoundaryConnection);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BOUNDARY_MARGIN_TOP, m_bCheckDefectConditionBoundaryMarginTop);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BOUNDARY_MARGIN_BOTTOM, m_bCheckDefectConditionBoundaryMarginBottom);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BOUNDARY_MARGIN_LEFT, m_bCheckDefectConditionBoundaryMarginLeft);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BOUNDARY_MARGIN_RIGHT, m_bCheckDefectConditionBoundaryMarginRight);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BOUNDARY_MARGIN_HCENTER, m_bCheckDefectConditionBoundaryMarginHCenter);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BOUNDARY_MARGIN_VCENTER, m_bCheckDefectConditionBoundaryMarginVCenter);

	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_AREA_MIN, m_iEditDefectConditionAreaMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_LENGTH_MIN, m_iEditDefectConditionLengthMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_WIDTH_MIN, m_iEditDefectConditionWidthMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_X_LENGTH_MIN, m_iEditDefectConditionXLengthMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_Y_LENGTH_MIN, m_iEditDefectConditionYLengthMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_MEAN_MIN, m_iEditDefectConditionMeanMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_STDEV_MIN, m_dEditDefectConditionStdevMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_ANISOMETRY_MIN, m_dEditDefectConditionAnisometryMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CIRCULARITY_MIN, m_dEditDefectConditionCircularityMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_RECTANGULARITY_MIN, m_dEditDefectConditionRectangularityMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CONVEXITY_MIN, m_dEditDefectConditionConvexityMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_ELLIPSE_RATIO_MIN, m_dEditDefectConditionEllipseRatioMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_AREA_RATIO_MIN, m_iEditDefectConditionAreaRatioMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CONTRAST_MIN, m_iEditDefectConditionContrastMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_GRAD_MIN, m_iEditDefectConditionGradMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BLOB_MIN, m_iEditDefectConditionBlobMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_SHIFT_X_MIN, m_iEditDefectConditionShiftXMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_SHIFT_Y_MIN, m_iEditDefectConditionShiftYMin);

	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_AREA_MAX, m_iEditDefectConditionAreaMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_LENGTH_MAX, m_iEditDefectConditionLengthMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_WIDTH_MAX, m_iEditDefectConditionWidthMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_X_LENGTH_MAX, m_iEditDefectConditionXLengthMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_Y_LENGTH_MAX, m_iEditDefectConditionYLengthMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_MEAN_MAX, m_iEditDefectConditionMeanMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_STDEV_MAX, m_dEditDefectConditionStdevMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_ANISOMETRY_MAX, m_dEditDefectConditionAnisometryMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CIRCULARITY_MAX, m_dEditDefectConditionCircularityMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_RECTANGULARITY_MAX, m_dEditDefectConditionRectangularityMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CONVEXITY_MAX, m_dEditDefectConditionConvexityMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_ELLIPSE_RATIO_MAX, m_dEditDefectConditionEllipseRatioMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_AREA_RATIO_MAX, m_iEditDefectConditionAreaRatioMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CONTRAST_MAX, m_iEditDefectConditionContrastMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_GRAD_MAX, m_iEditDefectConditionGradMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BLOB_MAX, m_iEditDefectConditionBlobMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_SHIFT_X_MAX, m_iEditDefectConditionShiftXMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_SHIFT_Y_MAX, m_iEditDefectConditionShiftYMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_OUTER_DIST, m_iEditDefectConditionOuterDist);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_INNER_DIST, m_iEditDefectConditionInnerDist);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_TOP, m_iEditDefectConditionBoundaryMarginTop);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_BOTTOM, m_iEditDefectConditionBoundaryMarginBottom);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_LEFT, m_iEditDefectConditionBoundaryMarginLeft);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_RIGHT, m_iEditDefectConditionBoundaryMarginRight);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_HCENTER, m_iEditDefectConditionBoundaryMarginHCenter);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_VCENTER, m_iEditDefectConditionBoundaryMarginVCenter);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_MIN_CONNECTION, m_iEditDefectConditionBoundaryMarginMinConnection);
	DDX_Text(pDX, IDC_EDIT_CANDIDATE_MIN_AREA, m_iEditCandidateMinArea);
	DDX_Check(pDX, IDC_CHECK_CLOSING_OPENING, m_bCheckClosingOpening);
	DDX_Radio(pDX, IDC_RADIO_LENGTH_WIDTH_CONDITION_AND, m_iRadioLengthWidthCondition);
	DDX_Radio(pDX, IDC_RADIO_XY_LENGTH_CONDITION_AND, m_iRadioXYLengthCondition);
	DDX_Radio(pDX, IDC_RADIO_BOUNDARY_CONNECTION_CONDITION_AND, m_iRadioBoundaryConnectionCondition);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BOUNDARY_CONNECTION_2SIDE, m_bCheckDefectConditionBoundaryConnection2Side);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_INNER_OUTER_BOUNDARY_CONNECTION, m_bCheckDefectConditionInnerOuterBoundaryConnection);
	DDX_Radio(pDX, IDC_RADIO_ONE_DIR_MORP_X_OPENING, m_iRadioOneDirMorpXOpening);
	DDX_Radio(pDX, IDC_RADIO_ONE_DIR_MORP_X_CLOSING, m_iRadioOneDirMorpXClosing);
	DDX_Check(pDX, IDC_CHECK_APPLY_ONE_DIR_MORP, m_bCheckApplyOneDirMorp);
	DDX_Radio(pDX, IDC_RADIO_X_LENGTH_CONDITION_MAX, m_iRadioXLengthCondition);
	DDX_Radio(pDX, IDC_RADIO_X_LENGTH_CONDITION_MIN_REF_TOP, m_iRadioXLengthConditionMinRef);
	DDX_Radio(pDX, IDC_RADIO_Y_LENGTH_CONDITION_MAX, m_iRadioYLengthCondition);
	DDX_Radio(pDX, IDC_RADIO_Y_LENGTH_CONDITION_MIN_REF_TOP, m_iRadioYLengthConditionMinRef);
	DDX_Radio(pDX, IDC_RADIO_HOLE_FILL_ALL, m_iRadioHoleFillCondition);
	DDX_Text(pDX, IDC_EDIT_FILL_HOLE_AREA, m_iEditFillHoleArea);
	DDX_Check(pDX, IDC_CHECK_XY_LENGTH_MEASURE_HOLE, m_bCheckXYLengthMeasureIgnoreHole);
	DDX_Check(pDX, IDC_CHECK_XY_LENGTH_MAX_LINE_LENGTH, m_bCheckXYLengthMaxLineLength);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BOUNDARY_CONNECTION_ANY, m_bCheckDefectConditionBoundaryConnectionAny);
	DDX_Radio(pDX, IDC_RADIO_BOUNDARY_CONNECTION_OUTER, m_iRadioBoundaryConnectionOuter);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_ANY, m_iEditDefectConditionBoundaryMarginAny);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_MIN_CONNECTION_ANY, m_iEditDefectConditionBoundaryMarginMinConnectionAny);

	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_HOLE, m_bCheckDefectConditionHole);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_HOLE_AREA, m_iEditDefectConditionHoleArea);

	DDX_Check(pDX, IDC_CHECK_UNION_BLOB, m_bCheckDefectConditionUnionBlob);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_DILATION, m_bCheckDefectConditionDilation);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_DILATION, m_iEditDefectConditionDilationSize);

	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_X_LENGTH_SUM, m_bCheckDefectConditionXLengthSum);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_Y_LENGTH_SUM, m_bCheckDefectConditionYLengthSum);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_X_LENGTH_SUM_MIN, m_iEditDefectConditionXLengthSumMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_Y_LENGTH_SUM_MIN, m_iEditDefectConditionYLengthSumMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_X_LENGTH_SUM_MAX, m_iEditDefectConditionXLengthSumMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_Y_LENGTH_SUM_MAX, m_iEditDefectConditionYLengthSumMax);

	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_OUTERDIFFERENCE, m_bCheckDefectConditionOuterDiff);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_OUTERDIFFERENCE_INTERVAL, m_nEditDefectConditionOuterDiffInterval);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_OUTERDIFFERENCE_DIFF, m_nEditDefectConditionOuterDiffMinDiff);
	DDX_Radio(pDX, IDC_RADIO_OUTERDIFF_UPPER, m_nRadioOuterDiffUpper);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_OUTERDIFFERENCE_MAXREPEAT, m_nEditDefectConditionOuterDiffMaxRepeat);
	DDX_Radio(pDX, IDC_RADIO_CONERLENGTH_LEFTTOP, m_nRadioConerLengthDir);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_USE_CONER_LENGTH, m_bCheckConerLengthUse);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CORNERLENGTH_MIN, m_nEditDefectConditionConerLengthMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CORNERLENGTH_MAX, m_nEditDefectConditionConerLengthMax);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_USE_CONER_LENGTH2, m_bCheckConerLengthMaxUse);

	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_ORTHO_LENGTH, m_bCheckDefectConditionOrthoLength);
	DDX_Radio(pDX, IDC_RADIO_ORTHO_LENGTH_REF_SIDE_TOP, m_iRadioOrthoLengthRefSide);
}


BEGIN_MESSAGE_MAP(CAlgorithmDefectConditionDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_AREA_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonAreaMin)
	ON_BN_CLICKED(IDC_BUTTON_AREA_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonAreaMax)
	ON_BN_CLICKED(IDC_BUTTON_LENGTH_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_LENGTH_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonWidthMin)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonWidthMax)
	ON_BN_CLICKED(IDC_BUTTON_MEAN_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonMeanMin)
	ON_BN_CLICKED(IDC_BUTTON_MEAN_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonMeanMax)
	ON_BN_CLICKED(IDC_BUTTON_STDEV_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonStdevMin)
	ON_BN_CLICKED(IDC_BUTTON_STDEV_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonStdevMax)
	ON_BN_CLICKED(IDC_BUTTON_ANISOMETRY_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonAnisometryMin)
	ON_BN_CLICKED(IDC_BUTTON_ANISOMETRY_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonAnisometryMax)
	ON_BN_CLICKED(IDC_BUTTON_CIRCULARITY_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonCircularityMin)
	ON_BN_CLICKED(IDC_BUTTON_CIRCULARITY_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonCircularityMax)
	ON_BN_CLICKED(IDC_BUTTON_RECTANGULARITY_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonRectangularityMin)
	ON_BN_CLICKED(IDC_BUTTON_RECTANGULARITY_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonRectangularityMax)
	ON_BN_CLICKED(IDC_BUTTON_CONVEXITY_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonConvexityMin)
	ON_BN_CLICKED(IDC_BUTTON_CONVEXITY_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonConvexityMax)
	ON_BN_CLICKED(IDC_BUTTON_ELLIPSE_RATIO_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonEllipseRatioMin)
	ON_BN_CLICKED(IDC_BUTTON_ELLIPSE_RATIO_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonEllipseRatioMax)
	ON_BN_CLICKED(IDC_BUTTON_AREA_RATIO_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonAreaRatioMin)
	ON_BN_CLICKED(IDC_BUTTON_AREA_RATIO_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonAreaRatioMax)
	ON_BN_CLICKED(IDC_BUTTON_CONTRAST_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonContrastMin)
	ON_BN_CLICKED(IDC_BUTTON_CONTRAST_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonContrastMax)
	ON_BN_CLICKED(IDC_BUTTON_GRAD_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonGradMin)
	ON_BN_CLICKED(IDC_BUTTON_GRAD_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonGradMax)
	ON_BN_CLICKED(IDC_BUTTON_BLOB_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonBlobMin)
	ON_BN_CLICKED(IDC_BUTTON_BLOB_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonBlobMax)
	ON_BN_CLICKED(IDC_BUTTON_SHIFT_X_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonShiftXMin)
	ON_BN_CLICKED(IDC_BUTTON_SHIFT_X_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonShiftXMax)
	ON_BN_CLICKED(IDC_BUTTON_SHIFT_Y_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonShiftYMin)
	ON_BN_CLICKED(IDC_BUTTON_SHIFT_Y_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonShiftYMax)
	ON_BN_CLICKED(IDC_BUTTON_X_LENGTH_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonXLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_X_LENGTH_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonXLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_Y_LENGTH_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonYLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_Y_LENGTH_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonYLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_X_LENGTH_SUM_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonXLengthSumMin)
	ON_BN_CLICKED(IDC_BUTTON_X_LENGTH_SUM_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonXLengthSumMax)
	ON_BN_CLICKED(IDC_BUTTON_Y_LENGTH_SUM_MIN, &CAlgorithmDefectConditionDlg::OnBnClickedButtonYLengthSumMin)
	ON_BN_CLICKED(IDC_BUTTON_Y_LENGTH_SUM_MAX, &CAlgorithmDefectConditionDlg::OnBnClickedButtonYLengthSumMax)
END_MESSAGE_MAP()


// CAlgorithmDefectConditionDlg 메시지 처리기입니다.
void CAlgorithmDefectConditionDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmDefectConditionDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_iEditCandidateMinArea = AlgorithmParam.m_iCandidateMinArea;
	m_bCheckCombineBrightDarkBlob = AlgorithmParam.m_bCombineBrightDarkBlob;
	m_bCheckMaxBlob = AlgorithmParam.m_bMaxBlob;
	m_bCheckClosingOpening = AlgorithmParam.m_bClosingOpening;
	m_bCheckApplyOneDirMorp = AlgorithmParam.m_bApplyOneDirMorp;
	m_iRadioOneDirMorpXOpening = AlgorithmParam.m_iOneDirMorpXOpening;
	m_iRadioOneDirMorpXClosing = AlgorithmParam.m_iOneDirMorpXClosing;
	m_iEditOpeningSize = AlgorithmParam.m_iOpeningSize;
	m_iEditClosingSize = AlgorithmParam.m_iClosingSize;
	m_iEditThresholdOpeningSize = AlgorithmParam.m_iThresholdOpeningSize;
	m_iEditThresholdClosingSize = AlgorithmParam.m_iThresholdClosingSize;
	m_bCheckFillup = AlgorithmParam.m_bFillup;
	m_bCheckConvex = AlgorithmParam.m_bUseConvex;
	m_bCheckSubConvex = AlgorithmParam.m_bUseSubConvex;
	m_bCheckRect = AlgorithmParam.m_bUseRect;
	m_bCheckUseConnection = AlgorithmParam.m_bUseConnection;
	m_iEditConnectionMinSize = AlgorithmParam.m_iConnectionMinSize;
	m_iEditConnectionMinXLength = AlgorithmParam.m_iConnectionMinXLength;
	m_iEditConnectionLength = AlgorithmParam.m_iConnectionLength;
	m_bCheckUseAngle = AlgorithmParam.m_bUseAngle;
	m_iEditConnectionMaxWidth = AlgorithmParam.m_iConnectionMaxWidth;
	m_iRadioHoleFillCondition = AlgorithmParam.m_iHoleFillCondition;
	m_iEditFillHoleArea = AlgorithmParam.m_iFillHoleArea;
	m_bCheckXYLengthMeasureIgnoreHole = AlgorithmParam.m_bXYLengthMeasureIgnoreHole;
	m_bCheckXYLengthMaxLineLength = AlgorithmParam.m_bXYLengthMaxLineLength;
	m_bCheckDefectConditionBoundaryConnectionAny = AlgorithmParam.m_bDefectConditionBoundaryConnectionAny;
	m_iRadioBoundaryConnectionOuter = AlgorithmParam.m_iDefectConditionBoundaryConnectionOuter;
	m_iEditDefectConditionBoundaryMarginAny = AlgorithmParam.m_iDefectConditionBoundaryMarginAny;
	m_iEditDefectConditionBoundaryMarginMinConnectionAny = AlgorithmParam.m_iDefectConditionBoundaryMarginMinConnectionAny;
	m_bCheckDefectConditionHole = AlgorithmParam.m_bUseDefectConditionHole;
	m_iEditDefectConditionHoleArea = AlgorithmParam.m_iDefectConditionHoleArea;
	m_bCheckDefectConditionUnionBlob = AlgorithmParam.m_bDefectConditionUnionBlob;
	m_bCheckDefectConditionDilation = AlgorithmParam.m_bDefectConditionDilation;

	//VER 1.1.3.2 add
	m_bCheckDefectConditionOuterDiff = AlgorithmParam.m_bDefectConditionOuterDiff;
	m_nEditDefectConditionOuterDiffInterval = AlgorithmParam.m_nDefectConditionOuterDiffInterval;
	m_nEditDefectConditionOuterDiffMinDiff = AlgorithmParam.m_nDefectConditionOuterDiffMinDiff;
	m_nRadioOuterDiffUpper = AlgorithmParam.m_nOuterDiffUpper;
	m_nEditDefectConditionOuterDiffMaxRepeat = AlgorithmParam.m_nDefectConditionOuterDiffMaxRepeat;
	//VER 1.1.3.2 end

	//VER 1.1.3.3 add
	m_nRadioConerLengthDir = AlgorithmParam.m_nDefectConditionConerLengthDir;
	m_bCheckConerLengthUse = AlgorithmParam.m_bDefectConditionConerLengthUse;
	m_nEditDefectConditionConerLengthMin = AlgorithmParam.m_nDefectConditionConerLengthMin;
	m_nEditDefectConditionConerLengthMax = AlgorithmParam.m_nDefectConditionConerLengthMax;
	//VER 1.1.3.3 end

	//VER 1.1.3.4 add
	m_bCheckConerLengthMaxUse = AlgorithmParam.m_bDefectConditionConerLengthMaxUse;
	//VER 1.1.3.4 end

	m_iEditDefectConditionDilationSize = AlgorithmParam.m_iDefectConditionDilationSize;
	m_bCheckDefectConditionXLengthSum = AlgorithmParam.m_bUseDefectConditionXLengthSum;
	m_iEditDefectConditionXLengthSumMin = AlgorithmParam.m_iDefectConditionXLengthSumMin;
	m_iEditDefectConditionXLengthSumMax = AlgorithmParam.m_iDefectConditionXLengthSumMax;
	m_bCheckDefectConditionYLengthSum = AlgorithmParam.m_bUseDefectConditionYLengthSum;
	m_iEditDefectConditionYLengthSumMin = AlgorithmParam.m_iDefectConditionYLengthSumMin;
	m_iEditDefectConditionYLengthSumMax = AlgorithmParam.m_iDefectConditionYLengthSumMax;

	m_bCheckDefectConditionArea = AlgorithmParam.m_bUseDefectConditionArea;
	m_bCheckDefectConditionLength = AlgorithmParam.m_bUseDefectConditionLength;
	m_bCheckDefectConditionWidth = AlgorithmParam.m_bUseDefectConditionWidth;
	m_bCheckDefectConditionXLength = AlgorithmParam.m_bUseDefectConditionXLength;
	m_bCheckDefectConditionYLength = AlgorithmParam.m_bUseDefectConditionYLength;
	m_bCheckDefectConditionMean = AlgorithmParam.m_bUseDefectConditionMean;
	m_bCheckDefectConditionStdev = AlgorithmParam.m_bUseDefectConditionStdev;
	m_bCheckDefectConditionAnisometry = AlgorithmParam.m_bUseDefectConditionAnisometry;
	m_bCheckDefectConditionCircularity = AlgorithmParam.m_bUseDefectConditionCircularity;
	m_bCheckDefectConditionRectangularity = AlgorithmParam.m_bUseDefectConditionRectangularity;
	m_bCheckDefectConditionConvexity = AlgorithmParam.m_bUseDefectConditionConvexity;
	m_bCheckDefectConditionEllipseRatio = AlgorithmParam.m_bUseDefectConditionEllipseRatio;
	m_bCheckDefectConditionAreaRatio = AlgorithmParam.m_bUseDefectConditionAreaRatio;
	m_bCheckDefectConditionContrast = AlgorithmParam.m_bUseDefectConditionContrast;
	m_bCheckDefectConditionGrad = AlgorithmParam.m_bUseDefectConditionGrad;
	m_bCheckDefectConditionBlob = AlgorithmParam.m_bUseDefectConditionBlob;
	m_bCheckDefectConditionShiftX = AlgorithmParam.m_bUseDefectConditionShiftX;
	m_bCheckDefectConditionShiftY = AlgorithmParam.m_bUseDefectConditionShiftY;
	m_bCheckDefectConditionOuterDist = AlgorithmParam.m_bUseDefectConditionOuterDist;
	m_bCheckDefectConditionInnerDist = AlgorithmParam.m_bUseDefectConditionInnerDist;
	m_bCheckDefectConditionUseZeroArea = AlgorithmParam.m_bUseDefectConditionUseZeroArea;
	m_bCheckDefectConditionBoundaryConnection = AlgorithmParam.m_bDefectConditionBoundaryConnection;
	m_bCheckDefectConditionBoundaryMarginTop = AlgorithmParam.m_bDefectConditionBoundaryMarginTop;
	m_bCheckDefectConditionBoundaryMarginBottom = AlgorithmParam.m_bDefectConditionBoundaryMarginBottom;
	m_bCheckDefectConditionBoundaryMarginLeft = AlgorithmParam.m_bDefectConditionBoundaryMarginLeft;
	m_bCheckDefectConditionBoundaryMarginRight = AlgorithmParam.m_bDefectConditionBoundaryMarginRight;
	m_bCheckDefectConditionBoundaryMarginHCenter = AlgorithmParam.m_bDefectConditionBoundaryMarginHCenter;
	m_bCheckDefectConditionBoundaryMarginVCenter = AlgorithmParam.m_bDefectConditionBoundaryMarginVCenter;

	m_iEditDefectConditionAreaMin = AlgorithmParam.m_iDefectConditionAreaMin;
	m_iEditDefectConditionLengthMin = AlgorithmParam.m_iDefectConditionLengthMin;
	m_iEditDefectConditionWidthMin = AlgorithmParam.m_iDefectConditionWidthMin;
	m_iEditDefectConditionXLengthMin = AlgorithmParam.m_iDefectConditionXLengthMin;
	m_iEditDefectConditionYLengthMin = AlgorithmParam.m_iDefectConditionYLengthMin;
	m_iEditDefectConditionMeanMin = AlgorithmParam.m_iDefectConditionMeanMin;
	m_dEditDefectConditionStdevMin = AlgorithmParam.m_dDefectConditionStdevMin;
	m_dEditDefectConditionAnisometryMin = AlgorithmParam.m_dDefectConditionAnisometryMin;
	m_dEditDefectConditionCircularityMin = AlgorithmParam.m_dDefectConditionCircularityMin;
	m_dEditDefectConditionRectangularityMin = AlgorithmParam.m_dDefectConditionRectangularityMin;
	m_dEditDefectConditionConvexityMin = AlgorithmParam.m_dDefectConditionConvexityMin;
	m_dEditDefectConditionEllipseRatioMin = AlgorithmParam.m_dDefectConditionEllipseRatioMin;
	m_iEditDefectConditionAreaRatioMin = AlgorithmParam.m_iDefectConditionAreaRatioMin;
	m_iEditDefectConditionContrastMin = AlgorithmParam.m_iDefectConditionContrastMin;
	m_iEditDefectConditionGradMin = AlgorithmParam.m_iDefectConditionGradMin;
	m_iEditDefectConditionBlobMin = AlgorithmParam.m_iDefectConditionBlobMin;
	m_iEditDefectConditionShiftXMin = AlgorithmParam.m_iDefectConditionShiftXMin;
	m_iEditDefectConditionShiftYMin = AlgorithmParam.m_iDefectConditionShiftYMin;

	m_iEditDefectConditionAreaMax = AlgorithmParam.m_iDefectConditionAreaMax;
	m_iEditDefectConditionLengthMax = AlgorithmParam.m_iDefectConditionLengthMax;
	m_iEditDefectConditionWidthMax = AlgorithmParam.m_iDefectConditionWidthMax;
	m_iEditDefectConditionXLengthMax = AlgorithmParam.m_iDefectConditionXLengthMax;
	m_iEditDefectConditionYLengthMax = AlgorithmParam.m_iDefectConditionYLengthMax;
	m_iEditDefectConditionMeanMax = AlgorithmParam.m_iDefectConditionMeanMax;
	m_dEditDefectConditionStdevMax = AlgorithmParam.m_dDefectConditionStdevMax;
	m_dEditDefectConditionAnisometryMax = AlgorithmParam.m_dDefectConditionAnisometryMax;
	m_dEditDefectConditionCircularityMax = AlgorithmParam.m_dDefectConditionCircularityMax;
	m_dEditDefectConditionRectangularityMax = AlgorithmParam.m_dDefectConditionRectangularityMax;
	m_dEditDefectConditionConvexityMax = AlgorithmParam.m_dDefectConditionConvexityMax;
	m_dEditDefectConditionEllipseRatioMax = AlgorithmParam.m_dDefectConditionEllipseRatioMax;
	m_iEditDefectConditionAreaRatioMax = AlgorithmParam.m_iDefectConditionAreaRatioMax;
	m_iEditDefectConditionContrastMax = AlgorithmParam.m_iDefectConditionContrastMax;
	m_iEditDefectConditionGradMax = AlgorithmParam.m_iDefectConditionGradMax;
	m_iEditDefectConditionBlobMax = AlgorithmParam.m_iDefectConditionBlobMax;
	m_iEditDefectConditionShiftXMax = AlgorithmParam.m_iDefectConditionShiftXMax;
	m_iEditDefectConditionShiftYMax = AlgorithmParam.m_iDefectConditionShiftYMax;

	m_iEditDefectConditionOuterDist = AlgorithmParam.m_iDefectConditionOuterDist;
	m_iEditDefectConditionInnerDist = AlgorithmParam.m_iDefectConditionInnerDist;

	m_iEditDefectConditionBoundaryMarginTop = AlgorithmParam.m_iDefectConditionBoundaryMarginTop;
	m_iEditDefectConditionBoundaryMarginBottom = AlgorithmParam.m_iDefectConditionBoundaryMarginBottom;
	m_iEditDefectConditionBoundaryMarginLeft = AlgorithmParam.m_iDefectConditionBoundaryMarginLeft;
	m_iEditDefectConditionBoundaryMarginRight = AlgorithmParam.m_iDefectConditionBoundaryMarginRight;
	m_iEditDefectConditionBoundaryMarginHCenter = AlgorithmParam.m_iDefectConditionBoundaryMarginHCenter;
	m_iEditDefectConditionBoundaryMarginVCenter = AlgorithmParam.m_iDefectConditionBoundaryMarginVCenter;
	m_iEditDefectConditionBoundaryMarginMinConnection = AlgorithmParam.m_iDefectConditionBoundaryMarginMinConnection;

	m_iRadioLengthWidthCondition = AlgorithmParam.m_iLengthWidthCondition;
	m_iRadioXYLengthCondition = AlgorithmParam.m_iXYLengthCondition;

	m_iRadioBoundaryConnectionCondition = AlgorithmParam.m_iBoundaryConnectionCondition;
	m_bCheckDefectConditionBoundaryConnection2Side = AlgorithmParam.m_bDefectConditionBoundaryConnection2Side;
	m_bCheckDefectConditionInnerOuterBoundaryConnection = AlgorithmParam.m_bDefectConditionInnerOuterBoundaryConnection;

	m_iRadioXLengthCondition = AlgorithmParam.m_iXLengthCondition;
	m_iRadioXLengthConditionMinRef = AlgorithmParam.m_iXLengthConditionMinRef;
	m_iRadioYLengthCondition = AlgorithmParam.m_iYLengthCondition;
	m_iRadioYLengthConditionMinRef = AlgorithmParam.m_iYLengthConditionMinRef;

	m_bCheckDefectConditionOrthoLength = AlgorithmParam.m_bDefectConditionOrthoLength;
	m_iRadioOrthoLengthRefSide = AlgorithmParam.m_iOrthoLengthRefSide;

	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_iCandidateMinArea = m_iEditCandidateMinArea;
	pAlgorithmParam->m_bCombineBrightDarkBlob = m_bCheckCombineBrightDarkBlob;
	pAlgorithmParam->m_bMaxBlob = m_bCheckMaxBlob;
	pAlgorithmParam->m_bClosingOpening = m_bCheckClosingOpening;
	pAlgorithmParam->m_bApplyOneDirMorp = m_bCheckApplyOneDirMorp;
	pAlgorithmParam->m_iOneDirMorpXOpening = m_iRadioOneDirMorpXOpening;
	pAlgorithmParam->m_iOneDirMorpXClosing = m_iRadioOneDirMorpXClosing;
	pAlgorithmParam->m_iOpeningSize = m_iEditOpeningSize;
	pAlgorithmParam->m_iClosingSize = m_iEditClosingSize;
	pAlgorithmParam->m_iThresholdOpeningSize = m_iEditThresholdOpeningSize;
	pAlgorithmParam->m_iThresholdClosingSize = m_iEditThresholdClosingSize;
	pAlgorithmParam->m_bFillup = m_bCheckFillup;
	pAlgorithmParam->m_bUseConvex = m_bCheckConvex;
	pAlgorithmParam->m_bUseSubConvex = m_bCheckSubConvex;
	pAlgorithmParam->m_bUseRect = m_bCheckRect;
	pAlgorithmParam->m_bUseConnection = m_bCheckUseConnection;
	pAlgorithmParam->m_iConnectionMinSize = m_iEditConnectionMinSize;
	pAlgorithmParam->m_iConnectionMinXLength = m_iEditConnectionMinXLength;
	pAlgorithmParam->m_iConnectionLength = m_iEditConnectionLength;
	pAlgorithmParam->m_bUseAngle = m_bCheckUseAngle;
	pAlgorithmParam->m_iConnectionMaxWidth = m_iEditConnectionMaxWidth;
	pAlgorithmParam->m_iHoleFillCondition = m_iRadioHoleFillCondition;
	pAlgorithmParam->m_iFillHoleArea = m_iEditFillHoleArea;
	pAlgorithmParam->m_bXYLengthMeasureIgnoreHole = m_bCheckXYLengthMeasureIgnoreHole;
	pAlgorithmParam->m_bXYLengthMaxLineLength = m_bCheckXYLengthMaxLineLength;
	pAlgorithmParam->m_bDefectConditionBoundaryConnectionAny = m_bCheckDefectConditionBoundaryConnectionAny;
	pAlgorithmParam->m_iDefectConditionBoundaryConnectionOuter = m_iRadioBoundaryConnectionOuter;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginAny = m_iEditDefectConditionBoundaryMarginAny;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginMinConnectionAny = m_iEditDefectConditionBoundaryMarginMinConnectionAny;
	pAlgorithmParam->m_bUseDefectConditionHole = m_bCheckDefectConditionHole;
	pAlgorithmParam->m_iDefectConditionHoleArea = m_iEditDefectConditionHoleArea;
	pAlgorithmParam->m_bDefectConditionUnionBlob = m_bCheckDefectConditionUnionBlob;
	pAlgorithmParam->m_bDefectConditionDilation = m_bCheckDefectConditionDilation;

	//VER 1.1.3.2 add
	pAlgorithmParam->m_bDefectConditionOuterDiff = m_bCheckDefectConditionOuterDiff;
	pAlgorithmParam->m_nDefectConditionOuterDiffInterval = m_nEditDefectConditionOuterDiffInterval;
	pAlgorithmParam->m_nDefectConditionOuterDiffMinDiff = m_nEditDefectConditionOuterDiffMinDiff;
	pAlgorithmParam->m_nOuterDiffUpper = m_nRadioOuterDiffUpper;
	pAlgorithmParam->m_nDefectConditionOuterDiffMaxRepeat = m_nEditDefectConditionOuterDiffMaxRepeat;
	//VER 1.1.3.2 end

	//VER 1.1.3.3 add
	pAlgorithmParam->m_nDefectConditionConerLengthDir = m_nRadioConerLengthDir;
	pAlgorithmParam->m_bDefectConditionConerLengthUse = m_bCheckConerLengthUse;
	pAlgorithmParam->m_nDefectConditionConerLengthMin = m_nEditDefectConditionConerLengthMin;
	pAlgorithmParam->m_nDefectConditionConerLengthMax = m_nEditDefectConditionConerLengthMax;
	//VER 1.1.3.3 end

	//VER 1.1.3.4 add
	pAlgorithmParam->m_bDefectConditionConerLengthMaxUse = m_bCheckConerLengthMaxUse;
	//VER 1.1.3.4 end

	pAlgorithmParam->m_iDefectConditionDilationSize = m_iEditDefectConditionDilationSize;
	pAlgorithmParam->m_bUseDefectConditionXLengthSum = m_bCheckDefectConditionXLengthSum;
	pAlgorithmParam->m_iDefectConditionXLengthSumMin = m_iEditDefectConditionXLengthSumMin;
	pAlgorithmParam->m_iDefectConditionXLengthSumMax = m_iEditDefectConditionXLengthSumMax;
	pAlgorithmParam->m_bUseDefectConditionYLengthSum = m_bCheckDefectConditionYLengthSum;
	pAlgorithmParam->m_iDefectConditionYLengthSumMin = m_iEditDefectConditionYLengthSumMin;
	pAlgorithmParam->m_iDefectConditionYLengthSumMax = m_iEditDefectConditionYLengthSumMax;

	pAlgorithmParam->m_bUseDefectConditionArea = m_bCheckDefectConditionArea;
	pAlgorithmParam->m_bUseDefectConditionLength = m_bCheckDefectConditionLength;
	pAlgorithmParam->m_bUseDefectConditionWidth = m_bCheckDefectConditionWidth;
	pAlgorithmParam->m_bUseDefectConditionXLength = m_bCheckDefectConditionXLength;
	pAlgorithmParam->m_bUseDefectConditionYLength = m_bCheckDefectConditionYLength;
	pAlgorithmParam->m_bUseDefectConditionMean = m_bCheckDefectConditionMean;
	pAlgorithmParam->m_bUseDefectConditionStdev = m_bCheckDefectConditionStdev;
	pAlgorithmParam->m_bUseDefectConditionAnisometry = m_bCheckDefectConditionAnisometry;
	pAlgorithmParam->m_bUseDefectConditionCircularity = m_bCheckDefectConditionCircularity;
	pAlgorithmParam->m_bUseDefectConditionRectangularity = m_bCheckDefectConditionRectangularity;
	pAlgorithmParam->m_bUseDefectConditionConvexity = m_bCheckDefectConditionConvexity;
	pAlgorithmParam->m_bUseDefectConditionEllipseRatio = m_bCheckDefectConditionEllipseRatio;
	pAlgorithmParam->m_bUseDefectConditionAreaRatio = m_bCheckDefectConditionAreaRatio;
	pAlgorithmParam->m_bUseDefectConditionContrast = m_bCheckDefectConditionContrast;
	pAlgorithmParam->m_bUseDefectConditionGrad = m_bCheckDefectConditionGrad;
	pAlgorithmParam->m_bUseDefectConditionBlob = m_bCheckDefectConditionBlob;
	pAlgorithmParam->m_bUseDefectConditionShiftX = m_bCheckDefectConditionShiftX;
	pAlgorithmParam->m_bUseDefectConditionShiftY = m_bCheckDefectConditionShiftY;
	pAlgorithmParam->m_bUseDefectConditionOuterDist = m_bCheckDefectConditionOuterDist;
	pAlgorithmParam->m_bUseDefectConditionInnerDist = m_bCheckDefectConditionInnerDist;
	pAlgorithmParam->m_bUseDefectConditionUseZeroArea = m_bCheckDefectConditionUseZeroArea;
	pAlgorithmParam->m_bDefectConditionBoundaryConnection = m_bCheckDefectConditionBoundaryConnection;
	pAlgorithmParam->m_bDefectConditionBoundaryMarginTop = m_bCheckDefectConditionBoundaryMarginTop;
	pAlgorithmParam->m_bDefectConditionBoundaryMarginBottom = m_bCheckDefectConditionBoundaryMarginBottom;
	pAlgorithmParam->m_bDefectConditionBoundaryMarginLeft = m_bCheckDefectConditionBoundaryMarginLeft;
	pAlgorithmParam->m_bDefectConditionBoundaryMarginRight = m_bCheckDefectConditionBoundaryMarginRight;
	pAlgorithmParam->m_bDefectConditionBoundaryMarginHCenter = m_bCheckDefectConditionBoundaryMarginHCenter;
	pAlgorithmParam->m_bDefectConditionBoundaryMarginVCenter = m_bCheckDefectConditionBoundaryMarginVCenter;

	pAlgorithmParam->m_iDefectConditionAreaMin = m_iEditDefectConditionAreaMin;
	pAlgorithmParam->m_iDefectConditionLengthMin = m_iEditDefectConditionLengthMin;
	pAlgorithmParam->m_iDefectConditionWidthMin = m_iEditDefectConditionWidthMin;
	pAlgorithmParam->m_iDefectConditionXLengthMin = m_iEditDefectConditionXLengthMin;
	pAlgorithmParam->m_iDefectConditionYLengthMin = m_iEditDefectConditionYLengthMin;
	pAlgorithmParam->m_iDefectConditionMeanMin = m_iEditDefectConditionMeanMin;
	pAlgorithmParam->m_dDefectConditionStdevMin = m_dEditDefectConditionStdevMin;
	pAlgorithmParam->m_dDefectConditionAnisometryMin = m_dEditDefectConditionAnisometryMin;
	pAlgorithmParam->m_dDefectConditionCircularityMin = m_dEditDefectConditionCircularityMin;
	pAlgorithmParam->m_dDefectConditionRectangularityMin = m_dEditDefectConditionRectangularityMin;
	pAlgorithmParam->m_dDefectConditionConvexityMin = m_dEditDefectConditionConvexityMin;
	pAlgorithmParam->m_dDefectConditionEllipseRatioMin = m_dEditDefectConditionEllipseRatioMin;
	pAlgorithmParam->m_iDefectConditionAreaRatioMin = m_iEditDefectConditionAreaRatioMin;
	pAlgorithmParam->m_iDefectConditionContrastMin = m_iEditDefectConditionContrastMin;
	pAlgorithmParam->m_iDefectConditionGradMin = m_iEditDefectConditionGradMin;
	pAlgorithmParam->m_iDefectConditionBlobMin = m_iEditDefectConditionBlobMin;
	pAlgorithmParam->m_iDefectConditionShiftXMin = m_iEditDefectConditionShiftXMin;
	pAlgorithmParam->m_iDefectConditionShiftYMin = m_iEditDefectConditionShiftYMin;

	pAlgorithmParam->m_iDefectConditionAreaMax = m_iEditDefectConditionAreaMax;
	pAlgorithmParam->m_iDefectConditionLengthMax = m_iEditDefectConditionLengthMax;
	pAlgorithmParam->m_iDefectConditionWidthMax = m_iEditDefectConditionWidthMax;
	pAlgorithmParam->m_iDefectConditionXLengthMax = m_iEditDefectConditionXLengthMax;
	pAlgorithmParam->m_iDefectConditionYLengthMax = m_iEditDefectConditionYLengthMax;
	pAlgorithmParam->m_iDefectConditionMeanMax = m_iEditDefectConditionMeanMax;
	pAlgorithmParam->m_dDefectConditionStdevMax = m_dEditDefectConditionStdevMax;
	pAlgorithmParam->m_dDefectConditionAnisometryMax = m_dEditDefectConditionAnisometryMax;
	pAlgorithmParam->m_dDefectConditionCircularityMax = m_dEditDefectConditionCircularityMax;
	pAlgorithmParam->m_dDefectConditionRectangularityMax = m_dEditDefectConditionRectangularityMax;
	pAlgorithmParam->m_dDefectConditionConvexityMax = m_dEditDefectConditionConvexityMax;
	pAlgorithmParam->m_dDefectConditionEllipseRatioMax = m_dEditDefectConditionEllipseRatioMax;
	pAlgorithmParam->m_iDefectConditionAreaRatioMax = m_iEditDefectConditionAreaRatioMax;
	pAlgorithmParam->m_iDefectConditionContrastMax = m_iEditDefectConditionContrastMax;
	pAlgorithmParam->m_iDefectConditionGradMax = m_iEditDefectConditionGradMax;
	pAlgorithmParam->m_iDefectConditionBlobMax = m_iEditDefectConditionBlobMax;
	pAlgorithmParam->m_iDefectConditionShiftXMax = m_iEditDefectConditionShiftXMax;
	pAlgorithmParam->m_iDefectConditionShiftYMax = m_iEditDefectConditionShiftYMax;

	pAlgorithmParam->m_iDefectConditionOuterDist = m_iEditDefectConditionOuterDist;
	pAlgorithmParam->m_iDefectConditionInnerDist = m_iEditDefectConditionInnerDist;

	pAlgorithmParam->m_iDefectConditionBoundaryMarginTop = m_iEditDefectConditionBoundaryMarginTop;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginBottom = m_iEditDefectConditionBoundaryMarginBottom;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginLeft = m_iEditDefectConditionBoundaryMarginLeft;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginRight = m_iEditDefectConditionBoundaryMarginRight;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginHCenter = m_iEditDefectConditionBoundaryMarginHCenter;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginVCenter = m_iEditDefectConditionBoundaryMarginVCenter;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginMinConnection = m_iEditDefectConditionBoundaryMarginMinConnection;

	pAlgorithmParam->m_iLengthWidthCondition = m_iRadioLengthWidthCondition;
	pAlgorithmParam->m_iXYLengthCondition = m_iRadioXYLengthCondition;

	pAlgorithmParam->m_iBoundaryConnectionCondition = m_iRadioBoundaryConnectionCondition;
	pAlgorithmParam->m_bDefectConditionBoundaryConnection2Side = m_bCheckDefectConditionBoundaryConnection2Side;
	pAlgorithmParam->m_bDefectConditionInnerOuterBoundaryConnection = m_bCheckDefectConditionInnerOuterBoundaryConnection;

	pAlgorithmParam->m_iXLengthCondition = m_iRadioXLengthCondition;
	pAlgorithmParam->m_iXLengthConditionMinRef = m_iRadioXLengthConditionMinRef;
	pAlgorithmParam->m_iYLengthCondition = m_iRadioYLengthCondition;
	pAlgorithmParam->m_iYLengthConditionMinRef = m_iRadioYLengthConditionMinRef;

	pAlgorithmParam->m_bDefectConditionOrthoLength = m_bCheckDefectConditionOrthoLength;
	pAlgorithmParam->m_iOrthoLengthRefSide = m_iRadioOrthoLengthRefSide;

}

BOOL CAlgorithmDefectConditionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonAreaMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonAreaMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaMax = 10000000;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionLengthMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionLengthMax = 100000;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonWidthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionWidthMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonWidthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionWidthMax = 100000;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonXLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionXLengthMin = 1;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonXLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionXLengthMax = 100000;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonYLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionYLengthMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonYLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionYLengthMax = 100000;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonMeanMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionMeanMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonMeanMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionMeanMax = 255;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonStdevMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionStdevMin = 0.1;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonStdevMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionStdevMax = 1000;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonAnisometryMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionAnisometryMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonAnisometryMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionAnisometryMax = 1000;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonCircularityMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionCircularityMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonCircularityMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionCircularityMax = 1;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonRectangularityMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionRectangularityMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonRectangularityMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionRectangularityMax = 1;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonConvexityMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionConvexityMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonConvexityMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionConvexityMax = 1;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonEllipseRatioMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionEllipseRatioMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDefectConditionDlg::OnBnClickedButtonEllipseRatioMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionEllipseRatioMax = 1000;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonAreaRatioMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaRatioMin = 1;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonAreaRatioMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaRatioMax = 100;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonContrastMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionContrastMin = 0;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonContrastMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionContrastMax = 255;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonGradMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionGradMin = 0;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonGradMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionGradMax = 255;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonBlobMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionBlobMin = 1;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonBlobMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionBlobMax = 1000;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonShiftXMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionShiftXMin = 0;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonShiftXMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionShiftXMax = 1000;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonShiftYMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionShiftYMin = 0;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonShiftYMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionShiftYMax = 1000;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonXLengthSumMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionXLengthSumMin = 1;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonXLengthSumMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionXLengthSumMax = 100000;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonYLengthSumMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionYLengthSumMin = 1;
	UpdateData(FALSE);
}

void CAlgorithmDefectConditionDlg::OnBnClickedButtonYLengthSumMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionYLengthSumMax = 100000;
	UpdateData(FALSE);
}
