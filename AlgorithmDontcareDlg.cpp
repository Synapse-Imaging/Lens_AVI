// AlgorithmDontcareDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmDontcareDlg.h"
#include "afxdialogex.h"


// CAlgorithmDontcareDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmDontcareDlg, CDialog)

CAlgorithmDontcareDlg::CAlgorithmDontcareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmDontcareDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_iRadioProcessChType = CHANNEL_TYPE_I;
	m_bCheckUseDC = FALSE;

	m_bCheckUseBrightFixedThres = FALSE;
	m_iEditBrightLowerThres = 0;
	m_iEditBrightUpperThres = 0;
	m_bCheckUseBrightDT = FALSE;
	m_iEditBrightMedianFilterSize = 0;
	m_iEditBrightClosingFilterSize = 0;
	m_iRadioBrightDTFilterType = 0;
	m_iEditBrightDTFilterSize = 0;
	m_iEditBrightDTFilterSize2 = 0;
	m_iEditBrightDTValue = 0;
	m_bCheckUseBrightUniformityCheck = FALSE;
	m_iEditBrightUniformityPeakMin = 0;
	m_iEditBrightUniformityPeakMax = 255;
	m_iEditBrightUniformityOffset = 0;
	m_iEditBrightUniformityHystLength = 0;
	m_iEditBrightUniformityHystOffset = 0;

	m_bCheckUseDarkFixedThres = FALSE;
	m_iEditDarkLowerThres = 0;
	m_iEditDarkUpperThres = 0;
	m_bCheckUseDarkDT = FALSE;
	m_iEditDarkMedianFilterSize = 0;
	m_iEditDarkClosingFilterSize = 0;
	m_iRadioDarkDTFilterType = 0;
	m_iEditDarkDTFilterSize = 0;
	m_iEditDarkDTFilterSize2 = 0;
	m_iEditDarkDTValue = 0;
	m_bCheckUseDarkUniformityCheck = FALSE;
	m_iEditDarkUniformityOffset = 0;
	m_iEditDarkUniformityPeakMin = 0;
	m_iEditDarkUniformityPeakMax = 255;
	m_iEditDarkUniformityHystLength = 0;
	m_iEditDarkUniformityHystOffset = 0;

	m_bCheckCombineBrightDarkBlob = FALSE;
	m_bCheckMaxBlob = FALSE;
	m_iEditOpeningSize = 0;
	m_iEditClosingSize = 0;
	m_bCheckFillup = FALSE;
	m_bCheckConvex = FALSE;
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
	m_iRadioLengthWidthCondition = 0;
	m_iRadioXYLengthCondition = 0;

	m_iRadioBoundaryConnectionCondition = 0;
	m_bCheckDefectConditionBoundaryConnection2Side = FALSE;

	m_iEditDontcareRegionMargin = 2;
}

CAlgorithmDontcareDlg::~CAlgorithmDontcareDlg()
{
}

void CAlgorithmDontcareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO_PROCESS_IMAGE_COLOR, m_iRadioProcessChType);
	DDX_Check(pDX, IDC_CHECK_USE_DONTCARE, m_bCheckUseDC);

	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_FIXED_THRESHOLD, m_bCheckUseBrightFixedThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_LOWER_THRES, m_iEditBrightLowerThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UPPER_THRES, m_iEditBrightUpperThres);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_DT, m_bCheckUseBrightDT);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_MED_FILTER_SIZE, m_iEditBrightMedianFilterSize);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_CLOSING_FILTER_SIZE, m_iEditBrightClosingFilterSize);
	DDX_Radio(pDX, IDC_RADIO_BRIGHT_DT_FILTER_TYPE_MEAN_DC, m_iRadioBrightDTFilterType);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_FILTER_SIZE, m_iEditBrightDTFilterSize);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_FILTER_SIZE3, m_iEditBrightDTFilterSize2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_VALUE, m_iEditBrightDTValue);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_UNIFORMITY_CHECK, m_bCheckUseBrightUniformityCheck);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_PEAK_MIN, m_iEditBrightUniformityPeakMin);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_PEAK_MAX, m_iEditBrightUniformityPeakMax);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_OFFSET, m_iEditBrightUniformityOffset);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_HYST_LENGTH, m_iEditBrightUniformityHystLength);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_HYST_OFFSET, m_iEditBrightUniformityHystOffset);

	DDX_Check(pDX, IDC_CHECK_USE_DARK_FIXED_THRESHOLD, m_bCheckUseDarkFixedThres);
	DDX_Text(pDX, IDC_EDIT_DARK_LOWER_THRES, m_iEditDarkLowerThres);
	DDX_Text(pDX, IDC_EDIT_DARK_UPPER_THRES, m_iEditDarkUpperThres);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_DT, m_bCheckUseDarkDT);
	DDX_Text(pDX, IDC_EDIT_DARK_MED_FILTER_SIZE, m_iEditDarkMedianFilterSize);
	DDX_Text(pDX, IDC_EDIT_DARK_CLOSING_FILTER_SIZE, m_iEditDarkClosingFilterSize);
	DDX_Radio(pDX, IDC_RADIO_DARK_DT_FILTER_TYPE_MEAN_DC, m_iRadioDarkDTFilterType);
	DDX_Text(pDX, IDC_EDIT_DARK_DT_FILTER_SIZE, m_iEditDarkDTFilterSize);
	DDX_Text(pDX, IDC_EDIT_DARK_DT_FILTER_SIZE3, m_iEditDarkDTFilterSize2);
	DDX_Text(pDX, IDC_EDIT_DARK_DT_VALUE, m_iEditDarkDTValue);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_UNIFORMITY_CHECK, m_bCheckUseDarkUniformityCheck);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_PEAK_MIN, m_iEditDarkUniformityPeakMin);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_PEAK_MAX, m_iEditDarkUniformityPeakMax);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_OFFSET, m_iEditDarkUniformityOffset);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_HYST_LENGTH, m_iEditDarkUniformityHystLength);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_HYST_OFFSET, m_iEditDarkUniformityHystOffset);

	DDX_Check(pDX, IDC_CHECK_COMBINE_BLOB, m_bCheckCombineBrightDarkBlob);
	DDX_Check(pDX, IDC_CHECK_MAX_BLOB, m_bCheckMaxBlob);
	DDX_Text(pDX, IDC_EDIT_OPENING_SIZE, m_iEditOpeningSize);
	DDX_Text(pDX, IDC_EDIT_CLOSING_SIZE, m_iEditClosingSize);
	DDX_Check(pDX, IDC_CHECK_FILL_UP, m_bCheckFillup);
	DDX_Check(pDX, IDC_CHECK_CONVEX, m_bCheckConvex);
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
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BLOB_MAX, m_iEditDefectConditionBlobMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_SHIFT_X_MAX, m_iEditDefectConditionShiftXMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_SHIFT_Y_MAX, m_iEditDefectConditionShiftYMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_OUTER_DIST, m_iEditDefectConditionOuterDist);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_INNER_DIST, m_iEditDefectConditionInnerDist);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_TOP, m_iEditDefectConditionBoundaryMarginTop);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_BOTTOM, m_iEditDefectConditionBoundaryMarginBottom);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_LEFT, m_iEditDefectConditionBoundaryMarginLeft);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_RIGHT, m_iEditDefectConditionBoundaryMarginRight);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_BOUNDARY_MARGIN_MIN_CONNECTION, m_iEditDefectConditionBoundaryMarginMinConnection);
	DDX_Text(pDX, IDC_EDIT_CANDIDATE_MIN_AREA, m_iEditCandidateMinArea);
	DDX_Check(pDX, IDC_CHECK_CLOSING_OPENING, m_bCheckClosingOpening);
	DDX_Radio(pDX, IDC_RADIO_LENGTH_WIDTH_CONDITION_AND, m_iRadioLengthWidthCondition);
	DDX_Radio(pDX, IDC_RADIO_XY_LENGTH_CONDITION_AND, m_iRadioXYLengthCondition);
	DDX_Radio(pDX, IDC_RADIO_BOUNDARY_CONNECTION_CONDITION_AND, m_iRadioBoundaryConnectionCondition);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_BOUNDARY_CONNECTION_2SIDE, m_bCheckDefectConditionBoundaryConnection2Side);

	DDX_Text(pDX, IDC_EDIT_DONTCARE_REGION_MARGIN, m_iEditDontcareRegionMargin);
}


BEGIN_MESSAGE_MAP(CAlgorithmDontcareDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_AREA_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonAreaMin)
	ON_BN_CLICKED(IDC_BUTTON_AREA_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonAreaMax)
	ON_BN_CLICKED(IDC_BUTTON_LENGTH_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_LENGTH_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonWidthMin)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonWidthMax)
	ON_BN_CLICKED(IDC_BUTTON_MEAN_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonMeanMin)
	ON_BN_CLICKED(IDC_BUTTON_MEAN_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonMeanMax)
	ON_BN_CLICKED(IDC_BUTTON_STDEV_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonStdevMin)
	ON_BN_CLICKED(IDC_BUTTON_STDEV_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonStdevMax)
	ON_BN_CLICKED(IDC_BUTTON_ANISOMETRY_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonAnisometryMin)
	ON_BN_CLICKED(IDC_BUTTON_ANISOMETRY_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonAnisometryMax)
	ON_BN_CLICKED(IDC_BUTTON_CIRCULARITY_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonCircularityMin)
	ON_BN_CLICKED(IDC_BUTTON_CIRCULARITY_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonCircularityMax)
	ON_BN_CLICKED(IDC_BUTTON_RECTANGULARITY_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonRectangularityMin)
	ON_BN_CLICKED(IDC_BUTTON_RECTANGULARITY_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonRectangularityMax)
	ON_BN_CLICKED(IDC_BUTTON_CONVEXITY_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonConvexityMin)
	ON_BN_CLICKED(IDC_BUTTON_CONVEXITY_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonConvexityMax)
	ON_BN_CLICKED(IDC_BUTTON_ELLIPSE_RATIO_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonEllipseRatioMin)
	ON_BN_CLICKED(IDC_BUTTON_ELLIPSE_RATIO_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonEllipseRatioMax)
	ON_BN_CLICKED(IDC_BUTTON_AREA_RATIO_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonAreaRatioMin)
	ON_BN_CLICKED(IDC_BUTTON_AREA_RATIO_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonAreaRatioMax)
	ON_BN_CLICKED(IDC_BUTTON_CONTRAST_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonContrastMin)
	ON_BN_CLICKED(IDC_BUTTON_CONTRAST_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonContrastMax)
	ON_BN_CLICKED(IDC_BUTTON_BLOB_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonBlobMin)
	ON_BN_CLICKED(IDC_BUTTON_BLOB_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonBlobMax)
	ON_BN_CLICKED(IDC_BUTTON_SHIFT_X_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonShiftXMin)
	ON_BN_CLICKED(IDC_BUTTON_SHIFT_X_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonShiftXMax)
	ON_BN_CLICKED(IDC_BUTTON_SHIFT_Y_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonShiftYMin)
	ON_BN_CLICKED(IDC_BUTTON_SHIFT_Y_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonShiftYMax)
	ON_BN_CLICKED(IDC_BUTTON_X_LENGTH_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonXLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_X_LENGTH_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonXLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_Y_LENGTH_MIN, &CAlgorithmDontcareDlg::OnBnClickedButtonYLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_Y_LENGTH_MAX, &CAlgorithmDontcareDlg::OnBnClickedButtonYLengthMax)
	ON_BN_CLICKED(IDC_MFCBUTTON_NEXT_PROCESSING, &CAlgorithmDontcareDlg::OnBnClickedMfcbuttonNextProcessing)
END_MESSAGE_MAP()


// CAlgorithmDontcareDlg 메시지 처리기입니다.
void CAlgorithmDontcareDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmDontcareDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_iRadioProcessChType = AlgorithmParam.m_iProcessChTypeDC;
	m_bCheckUseDC = AlgorithmParam.m_bUseDC;

	m_bCheckUseBrightFixedThres = AlgorithmParam.m_bUseBrightFixedThresDC;
	m_iEditBrightLowerThres = AlgorithmParam.m_iBrightLowerThresDC;
	m_iEditBrightUpperThres = AlgorithmParam.m_iBrightUpperThresDC;
	m_bCheckUseBrightDT = AlgorithmParam.m_bUseBrightDTDC;
	m_iEditBrightMedianFilterSize = AlgorithmParam.m_iBrightMedianFilterSizeDC;
	m_iEditBrightClosingFilterSize = AlgorithmParam.m_iBrightClosingFilterSizeDC;
	m_iRadioBrightDTFilterType = AlgorithmParam.m_iBrightDTFilterTypeDC;
	m_iEditBrightDTFilterSize = AlgorithmParam.m_iBrightDTFilterSizeDC;
	m_iEditBrightDTFilterSize2 = AlgorithmParam.m_iBrightDTFilterSizeDC2;
	m_iEditBrightDTValue = AlgorithmParam.m_iBrightDTValueDC;
	m_bCheckUseBrightUniformityCheck = AlgorithmParam.m_bUseBrightUniformityCheckDC;
	m_iEditBrightUniformityPeakMin = AlgorithmParam.m_iBrightUniformityPeakMinDC;
	m_iEditBrightUniformityPeakMax = AlgorithmParam.m_iBrightUniformityPeakMaxDC;
	m_iEditBrightUniformityOffset = AlgorithmParam.m_iBrightUniformityOffsetDC;
	m_iEditBrightUniformityHystLength = AlgorithmParam.m_iBrightUniformityHystLengthDC;
	m_iEditBrightUniformityHystOffset = AlgorithmParam.m_iBrightUniformityHystOffsetDC;

	m_bCheckUseDarkFixedThres = AlgorithmParam.m_bUseDarkFixedThresDC;
	m_iEditDarkLowerThres = AlgorithmParam.m_iDarkLowerThresDC;
	m_iEditDarkUpperThres = AlgorithmParam.m_iDarkUpperThresDC;
	m_bCheckUseDarkDT = AlgorithmParam.m_bUseDarkDTDC;
	m_iEditDarkMedianFilterSize = AlgorithmParam.m_iDarkMedianFilterSizeDC;
	m_iEditDarkClosingFilterSize = AlgorithmParam.m_iDarkClosingFilterSizeDC;
	m_iRadioDarkDTFilterType = AlgorithmParam.m_iDarkDTFilterTypeDC;
	m_iEditDarkDTFilterSize = AlgorithmParam.m_iDarkDTFilterSizeDC;
	m_iEditDarkDTFilterSize2 = AlgorithmParam.m_iDarkDTFilterSizeDC2;
	m_iEditDarkDTValue = AlgorithmParam.m_iDarkDTValueDC;
	m_bCheckUseDarkUniformityCheck = AlgorithmParam.m_bUseDarkUniformityCheckDC;
	m_iEditDarkUniformityPeakMin = AlgorithmParam.m_iDarkUniformityPeakMinDC;
	m_iEditDarkUniformityPeakMax = AlgorithmParam.m_iDarkUniformityPeakMaxDC;
	m_iEditDarkUniformityOffset = AlgorithmParam.m_iDarkUniformityOffsetDC;
	m_iEditDarkUniformityHystLength = AlgorithmParam.m_iDarkUniformityHystLengthDC;
	m_iEditDarkUniformityHystOffset = AlgorithmParam.m_iDarkUniformityHystOffsetDC;

	m_iEditCandidateMinArea = AlgorithmParam.m_iCandidateMinAreaDC;
	m_bCheckCombineBrightDarkBlob = AlgorithmParam.m_bCombineBrightDarkBlobDC;
	m_bCheckMaxBlob = AlgorithmParam.m_bMaxBlobDC;
	m_bCheckClosingOpening = AlgorithmParam.m_bClosingOpeningDC;
	m_iEditOpeningSize = AlgorithmParam.m_iOpeningSizeDC;
	m_iEditClosingSize = AlgorithmParam.m_iClosingSizeDC;
	m_bCheckFillup = AlgorithmParam.m_bFillupDC;
	m_bCheckConvex = AlgorithmParam.m_bUseConvexDC;
	m_bCheckUseConnection = AlgorithmParam.m_bUseConnectionDC;
	m_iEditConnectionMinSize = AlgorithmParam.m_iConnectionMinSizeDC;
	m_iEditConnectionMinXLength = AlgorithmParam.m_iConnectionMinXLengthDC;
	m_iEditConnectionLength = AlgorithmParam.m_iConnectionLengthDC;
	m_bCheckUseAngle = AlgorithmParam.m_bUseAngleDC;
	m_iEditConnectionMaxWidth = AlgorithmParam.m_iConnectionMaxWidthDC;

	m_bCheckDefectConditionArea = AlgorithmParam.m_bUseDefectConditionAreaDC;
	m_bCheckDefectConditionLength = AlgorithmParam.m_bUseDefectConditionLengthDC;
	m_bCheckDefectConditionWidth = AlgorithmParam.m_bUseDefectConditionWidthDC;
	m_bCheckDefectConditionXLength = AlgorithmParam.m_bUseDefectConditionXLengthDC;
	m_bCheckDefectConditionYLength = AlgorithmParam.m_bUseDefectConditionYLengthDC;
	m_bCheckDefectConditionMean = AlgorithmParam.m_bUseDefectConditionMeanDC;
	m_bCheckDefectConditionStdev = AlgorithmParam.m_bUseDefectConditionStdevDC;
	m_bCheckDefectConditionAnisometry = AlgorithmParam.m_bUseDefectConditionAnisometryDC;
	m_bCheckDefectConditionCircularity = AlgorithmParam.m_bUseDefectConditionCircularityDC;
	m_bCheckDefectConditionRectangularity = AlgorithmParam.m_bUseDefectConditionRectangularityDC;
	m_bCheckDefectConditionConvexity = AlgorithmParam.m_bUseDefectConditionConvexityDC;
	m_bCheckDefectConditionEllipseRatio = AlgorithmParam.m_bUseDefectConditionEllipseRatioDC;
	m_bCheckDefectConditionAreaRatio = AlgorithmParam.m_bUseDefectConditionAreaRatioDC;
	m_bCheckDefectConditionContrast = AlgorithmParam.m_bUseDefectConditionContrastDC;
	m_bCheckDefectConditionBlob = AlgorithmParam.m_bUseDefectConditionBlobDC;
	m_bCheckDefectConditionShiftX = AlgorithmParam.m_bUseDefectConditionShiftXDC;
	m_bCheckDefectConditionShiftY = AlgorithmParam.m_bUseDefectConditionShiftYDC;
	m_bCheckDefectConditionOuterDist = AlgorithmParam.m_bUseDefectConditionOuterDistDC;
	m_bCheckDefectConditionInnerDist = AlgorithmParam.m_bUseDefectConditionInnerDistDC;
	m_bCheckDefectConditionUseZeroArea = AlgorithmParam.m_bUseDefectConditionUseZeroAreaDC;
	m_bCheckDefectConditionBoundaryConnection = AlgorithmParam.m_bDefectConditionBoundaryConnectionDC;
	m_bCheckDefectConditionBoundaryMarginTop = AlgorithmParam.m_bDefectConditionBoundaryMarginTopDC;
	m_bCheckDefectConditionBoundaryMarginBottom = AlgorithmParam.m_bDefectConditionBoundaryMarginBottomDC;
	m_bCheckDefectConditionBoundaryMarginLeft = AlgorithmParam.m_bDefectConditionBoundaryMarginLeftDC;
	m_bCheckDefectConditionBoundaryMarginRight = AlgorithmParam.m_bDefectConditionBoundaryMarginRightDC;

	m_iEditDefectConditionAreaMin = AlgorithmParam.m_iDefectConditionAreaMinDC;
	m_iEditDefectConditionLengthMin = AlgorithmParam.m_iDefectConditionLengthMinDC;
	m_iEditDefectConditionWidthMin = AlgorithmParam.m_iDefectConditionWidthMinDC;
	m_iEditDefectConditionXLengthMin = AlgorithmParam.m_iDefectConditionXLengthMinDC;
	m_iEditDefectConditionYLengthMin = AlgorithmParam.m_iDefectConditionYLengthMinDC;
	m_iEditDefectConditionMeanMin = AlgorithmParam.m_iDefectConditionMeanMinDC;
	m_dEditDefectConditionStdevMin = AlgorithmParam.m_dDefectConditionStdevMinDC;
	m_dEditDefectConditionAnisometryMin = AlgorithmParam.m_dDefectConditionAnisometryMinDC;
	m_dEditDefectConditionCircularityMin = AlgorithmParam.m_dDefectConditionCircularityMinDC;
	m_dEditDefectConditionRectangularityMin = AlgorithmParam.m_dDefectConditionRectangularityMinDC;
	m_dEditDefectConditionConvexityMin = AlgorithmParam.m_dDefectConditionConvexityMinDC;
	m_dEditDefectConditionEllipseRatioMin = AlgorithmParam.m_dDefectConditionEllipseRatioMinDC;
	m_iEditDefectConditionAreaRatioMin = AlgorithmParam.m_iDefectConditionAreaRatioMinDC;
	m_iEditDefectConditionContrastMin = AlgorithmParam.m_iDefectConditionContrastMinDC;
	m_iEditDefectConditionBlobMin = AlgorithmParam.m_iDefectConditionBlobMinDC;
	m_iEditDefectConditionShiftXMin = AlgorithmParam.m_iDefectConditionShiftXMinDC;
	m_iEditDefectConditionShiftYMin = AlgorithmParam.m_iDefectConditionShiftYMinDC;

	m_iEditDefectConditionAreaMax = AlgorithmParam.m_iDefectConditionAreaMaxDC;
	m_iEditDefectConditionLengthMax = AlgorithmParam.m_iDefectConditionLengthMaxDC;
	m_iEditDefectConditionWidthMax = AlgorithmParam.m_iDefectConditionWidthMaxDC;
	m_iEditDefectConditionXLengthMax = AlgorithmParam.m_iDefectConditionXLengthMaxDC;
	m_iEditDefectConditionYLengthMax = AlgorithmParam.m_iDefectConditionYLengthMaxDC;
	m_iEditDefectConditionMeanMax = AlgorithmParam.m_iDefectConditionMeanMaxDC;
	m_dEditDefectConditionStdevMax = AlgorithmParam.m_dDefectConditionStdevMaxDC;
	m_dEditDefectConditionAnisometryMax = AlgorithmParam.m_dDefectConditionAnisometryMaxDC;
	m_dEditDefectConditionCircularityMax = AlgorithmParam.m_dDefectConditionCircularityMaxDC;
	m_dEditDefectConditionRectangularityMax = AlgorithmParam.m_dDefectConditionRectangularityMaxDC;
	m_dEditDefectConditionConvexityMax = AlgorithmParam.m_dDefectConditionConvexityMaxDC;
	m_dEditDefectConditionEllipseRatioMax = AlgorithmParam.m_dDefectConditionEllipseRatioMaxDC;
	m_iEditDefectConditionAreaRatioMax = AlgorithmParam.m_iDefectConditionAreaRatioMaxDC;
	m_iEditDefectConditionContrastMax = AlgorithmParam.m_iDefectConditionContrastMaxDC;
	m_iEditDefectConditionBlobMax = AlgorithmParam.m_iDefectConditionBlobMaxDC;
	m_iEditDefectConditionShiftXMax = AlgorithmParam.m_iDefectConditionShiftXMaxDC;
	m_iEditDefectConditionShiftYMax = AlgorithmParam.m_iDefectConditionShiftYMaxDC;

	m_iEditDefectConditionOuterDist = AlgorithmParam.m_iDefectConditionOuterDistDC;
	m_iEditDefectConditionInnerDist = AlgorithmParam.m_iDefectConditionInnerDistDC;

	m_iEditDefectConditionBoundaryMarginTop = AlgorithmParam.m_iDefectConditionBoundaryMarginTopDC;
	m_iEditDefectConditionBoundaryMarginBottom = AlgorithmParam.m_iDefectConditionBoundaryMarginBottomDC;
	m_iEditDefectConditionBoundaryMarginLeft = AlgorithmParam.m_iDefectConditionBoundaryMarginLeftDC;
	m_iEditDefectConditionBoundaryMarginRight = AlgorithmParam.m_iDefectConditionBoundaryMarginRightDC;
	m_iEditDefectConditionBoundaryMarginMinConnection = AlgorithmParam.m_iDefectConditionBoundaryMarginMinConnectionDC;

	m_iRadioLengthWidthCondition = AlgorithmParam.m_iLengthWidthConditionDC;
	m_iRadioXYLengthCondition = AlgorithmParam.m_iXYLengthConditionDC;

	m_iRadioBoundaryConnectionCondition = AlgorithmParam.m_iBoundaryConnectionConditionDC;
	m_bCheckDefectConditionBoundaryConnection2Side = AlgorithmParam.m_bDefectConditionBoundaryConnection2SideDC;

	m_iEditDontcareRegionMargin = AlgorithmParam.m_iDontcareRegionMargin;

	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_iProcessChTypeDC = m_iRadioProcessChType;
	pAlgorithmParam->m_bUseDC = m_bCheckUseDC;

	pAlgorithmParam->m_bUseBrightFixedThresDC = m_bCheckUseBrightFixedThres;
	pAlgorithmParam->m_iBrightLowerThresDC = m_iEditBrightLowerThres;
	pAlgorithmParam->m_iBrightUpperThresDC = m_iEditBrightUpperThres;
	pAlgorithmParam->m_bUseBrightDTDC = m_bCheckUseBrightDT;
	pAlgorithmParam->m_iBrightMedianFilterSizeDC = m_iEditBrightMedianFilterSize;
	pAlgorithmParam->m_iBrightClosingFilterSizeDC = m_iEditBrightClosingFilterSize;
	pAlgorithmParam->m_iBrightDTFilterTypeDC = m_iRadioBrightDTFilterType;
	pAlgorithmParam->m_iBrightDTFilterSizeDC = m_iEditBrightDTFilterSize;
	pAlgorithmParam->m_iBrightDTFilterSizeDC2 = m_iEditBrightDTFilterSize2;
	pAlgorithmParam->m_iBrightDTValueDC = m_iEditBrightDTValue;
	pAlgorithmParam->m_bUseBrightUniformityCheckDC = m_bCheckUseBrightUniformityCheck;
	pAlgorithmParam->m_iBrightUniformityPeakMinDC = m_iEditBrightUniformityPeakMin;
	pAlgorithmParam->m_iBrightUniformityPeakMaxDC = m_iEditBrightUniformityPeakMax;
	pAlgorithmParam->m_iBrightUniformityOffsetDC = m_iEditBrightUniformityOffset;
	pAlgorithmParam->m_iBrightUniformityHystLengthDC = m_iEditBrightUniformityHystLength;
	pAlgorithmParam->m_iBrightUniformityHystOffsetDC = m_iEditBrightUniformityHystOffset;

	pAlgorithmParam->m_bUseDarkFixedThresDC = m_bCheckUseDarkFixedThres;
	pAlgorithmParam->m_iDarkLowerThresDC = m_iEditDarkLowerThres;
	pAlgorithmParam->m_iDarkUpperThresDC = m_iEditDarkUpperThres;
	pAlgorithmParam->m_bUseDarkDTDC = m_bCheckUseDarkDT;
	pAlgorithmParam->m_iDarkMedianFilterSizeDC = m_iEditDarkMedianFilterSize;
	pAlgorithmParam->m_iDarkClosingFilterSizeDC = m_iEditDarkClosingFilterSize;
	pAlgorithmParam->m_iDarkDTFilterTypeDC = m_iRadioDarkDTFilterType;
	pAlgorithmParam->m_iDarkDTFilterSizeDC = m_iEditDarkDTFilterSize;
	pAlgorithmParam->m_iDarkDTFilterSizeDC2 = m_iEditDarkDTFilterSize2;
	pAlgorithmParam->m_iDarkDTValueDC = m_iEditDarkDTValue;
	pAlgorithmParam->m_bUseDarkUniformityCheckDC = m_bCheckUseDarkUniformityCheck;
	pAlgorithmParam->m_iDarkUniformityPeakMinDC = m_iEditDarkUniformityPeakMin;
	pAlgorithmParam->m_iDarkUniformityPeakMaxDC = m_iEditDarkUniformityPeakMax;
	pAlgorithmParam->m_iDarkUniformityOffsetDC = m_iEditDarkUniformityOffset;
	pAlgorithmParam->m_iDarkUniformityHystLengthDC = m_iEditDarkUniformityHystLength;
	pAlgorithmParam->m_iDarkUniformityHystOffsetDC = m_iEditDarkUniformityHystOffset;

	pAlgorithmParam->m_iCandidateMinAreaDC = m_iEditCandidateMinArea;
	pAlgorithmParam->m_bCombineBrightDarkBlobDC = m_bCheckCombineBrightDarkBlob;
	pAlgorithmParam->m_bMaxBlobDC = m_bCheckMaxBlob;
	pAlgorithmParam->m_bClosingOpeningDC = m_bCheckClosingOpening;
	pAlgorithmParam->m_iOpeningSizeDC = m_iEditOpeningSize;
	pAlgorithmParam->m_iClosingSizeDC = m_iEditClosingSize;
	pAlgorithmParam->m_bFillupDC = m_bCheckFillup;
	pAlgorithmParam->m_bUseConvexDC = m_bCheckConvex;
	pAlgorithmParam->m_bUseConnectionDC = m_bCheckUseConnection;
	pAlgorithmParam->m_iConnectionMinSizeDC = m_iEditConnectionMinSize;
	pAlgorithmParam->m_iConnectionMinXLengthDC = m_iEditConnectionMinXLength;
	pAlgorithmParam->m_iConnectionLengthDC = m_iEditConnectionLength;
	pAlgorithmParam->m_bUseAngleDC = m_bCheckUseAngle;
	pAlgorithmParam->m_iConnectionMaxWidthDC = m_iEditConnectionMaxWidth;

	pAlgorithmParam->m_bUseDefectConditionAreaDC = m_bCheckDefectConditionArea;
	pAlgorithmParam->m_bUseDefectConditionLengthDC = m_bCheckDefectConditionLength;
	pAlgorithmParam->m_bUseDefectConditionWidthDC = m_bCheckDefectConditionWidth;
	pAlgorithmParam->m_bUseDefectConditionXLengthDC = m_bCheckDefectConditionXLength;
	pAlgorithmParam->m_bUseDefectConditionYLengthDC = m_bCheckDefectConditionYLength;
	pAlgorithmParam->m_bUseDefectConditionMeanDC = m_bCheckDefectConditionMean;
	pAlgorithmParam->m_bUseDefectConditionStdevDC = m_bCheckDefectConditionStdev;
	pAlgorithmParam->m_bUseDefectConditionAnisometryDC = m_bCheckDefectConditionAnisometry;
	pAlgorithmParam->m_bUseDefectConditionCircularityDC = m_bCheckDefectConditionCircularity;
	pAlgorithmParam->m_bUseDefectConditionRectangularityDC = m_bCheckDefectConditionRectangularity;
	pAlgorithmParam->m_bUseDefectConditionConvexityDC = m_bCheckDefectConditionConvexity;
	pAlgorithmParam->m_bUseDefectConditionEllipseRatioDC = m_bCheckDefectConditionEllipseRatio;
	pAlgorithmParam->m_bUseDefectConditionAreaRatioDC = m_bCheckDefectConditionAreaRatio;
	pAlgorithmParam->m_bUseDefectConditionContrastDC = m_bCheckDefectConditionContrast;
	pAlgorithmParam->m_bUseDefectConditionBlobDC = m_bCheckDefectConditionBlob;
	pAlgorithmParam->m_bUseDefectConditionShiftXDC = m_bCheckDefectConditionShiftX;
	pAlgorithmParam->m_bUseDefectConditionShiftYDC = m_bCheckDefectConditionShiftY;
	pAlgorithmParam->m_bUseDefectConditionOuterDistDC = m_bCheckDefectConditionOuterDist;
	pAlgorithmParam->m_bUseDefectConditionInnerDistDC = m_bCheckDefectConditionInnerDist;
	pAlgorithmParam->m_bUseDefectConditionUseZeroAreaDC = m_bCheckDefectConditionUseZeroArea;
	pAlgorithmParam->m_bDefectConditionBoundaryConnectionDC = m_bCheckDefectConditionBoundaryConnection;
	pAlgorithmParam->m_bDefectConditionBoundaryMarginTopDC = m_bCheckDefectConditionBoundaryMarginTop;
	pAlgorithmParam->m_bDefectConditionBoundaryMarginBottomDC = m_bCheckDefectConditionBoundaryMarginBottom;
	pAlgorithmParam->m_bDefectConditionBoundaryMarginLeftDC = m_bCheckDefectConditionBoundaryMarginLeft;
	pAlgorithmParam->m_bDefectConditionBoundaryMarginRightDC = m_bCheckDefectConditionBoundaryMarginRight;

	pAlgorithmParam->m_iDefectConditionAreaMinDC = m_iEditDefectConditionAreaMin;
	pAlgorithmParam->m_iDefectConditionLengthMinDC = m_iEditDefectConditionLengthMin;
	pAlgorithmParam->m_iDefectConditionWidthMinDC = m_iEditDefectConditionWidthMin;
	pAlgorithmParam->m_iDefectConditionXLengthMinDC = m_iEditDefectConditionXLengthMin;
	pAlgorithmParam->m_iDefectConditionYLengthMinDC = m_iEditDefectConditionYLengthMin;
	pAlgorithmParam->m_iDefectConditionMeanMinDC = m_iEditDefectConditionMeanMin;
	pAlgorithmParam->m_dDefectConditionStdevMinDC = m_dEditDefectConditionStdevMin;
	pAlgorithmParam->m_dDefectConditionAnisometryMinDC = m_dEditDefectConditionAnisometryMin;
	pAlgorithmParam->m_dDefectConditionCircularityMinDC = m_dEditDefectConditionCircularityMin;
	pAlgorithmParam->m_dDefectConditionRectangularityMinDC = m_dEditDefectConditionRectangularityMin;
	pAlgorithmParam->m_dDefectConditionConvexityMinDC = m_dEditDefectConditionConvexityMin;
	pAlgorithmParam->m_dDefectConditionEllipseRatioMinDC = m_dEditDefectConditionEllipseRatioMin;
	pAlgorithmParam->m_iDefectConditionAreaRatioMinDC = m_iEditDefectConditionAreaRatioMin;
	pAlgorithmParam->m_iDefectConditionContrastMinDC = m_iEditDefectConditionContrastMin;
	pAlgorithmParam->m_iDefectConditionBlobMinDC = m_iEditDefectConditionBlobMin;
	pAlgorithmParam->m_iDefectConditionShiftXMinDC = m_iEditDefectConditionShiftXMin;
	pAlgorithmParam->m_iDefectConditionShiftYMinDC = m_iEditDefectConditionShiftYMin;

	pAlgorithmParam->m_iDefectConditionAreaMaxDC = m_iEditDefectConditionAreaMax;
	pAlgorithmParam->m_iDefectConditionLengthMaxDC = m_iEditDefectConditionLengthMax;
	pAlgorithmParam->m_iDefectConditionWidthMaxDC = m_iEditDefectConditionWidthMax;
	pAlgorithmParam->m_iDefectConditionXLengthMaxDC = m_iEditDefectConditionXLengthMax;
	pAlgorithmParam->m_iDefectConditionYLengthMaxDC = m_iEditDefectConditionYLengthMax;
	pAlgorithmParam->m_iDefectConditionMeanMaxDC = m_iEditDefectConditionMeanMax;
	pAlgorithmParam->m_dDefectConditionStdevMaxDC = m_dEditDefectConditionStdevMax;
	pAlgorithmParam->m_dDefectConditionAnisometryMaxDC = m_dEditDefectConditionAnisometryMax;
	pAlgorithmParam->m_dDefectConditionCircularityMaxDC = m_dEditDefectConditionCircularityMax;
	pAlgorithmParam->m_dDefectConditionRectangularityMaxDC = m_dEditDefectConditionRectangularityMax;
	pAlgorithmParam->m_dDefectConditionConvexityMaxDC = m_dEditDefectConditionConvexityMax;
	pAlgorithmParam->m_dDefectConditionEllipseRatioMaxDC = m_dEditDefectConditionEllipseRatioMax;
	pAlgorithmParam->m_iDefectConditionAreaRatioMaxDC = m_iEditDefectConditionAreaRatioMax;
	pAlgorithmParam->m_iDefectConditionContrastMaxDC = m_iEditDefectConditionContrastMax;
	pAlgorithmParam->m_iDefectConditionBlobMaxDC = m_iEditDefectConditionBlobMax;
	pAlgorithmParam->m_iDefectConditionShiftXMaxDC = m_iEditDefectConditionShiftXMax;
	pAlgorithmParam->m_iDefectConditionShiftYMaxDC = m_iEditDefectConditionShiftYMax;

	pAlgorithmParam->m_iDefectConditionOuterDistDC = m_iEditDefectConditionOuterDist;
	pAlgorithmParam->m_iDefectConditionInnerDistDC = m_iEditDefectConditionInnerDist;

	pAlgorithmParam->m_iDefectConditionBoundaryMarginTopDC = m_iEditDefectConditionBoundaryMarginTop;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginBottomDC = m_iEditDefectConditionBoundaryMarginBottom;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginLeftDC = m_iEditDefectConditionBoundaryMarginLeft;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginRightDC = m_iEditDefectConditionBoundaryMarginRight;
	pAlgorithmParam->m_iDefectConditionBoundaryMarginMinConnectionDC = m_iEditDefectConditionBoundaryMarginMinConnection;

	pAlgorithmParam->m_iLengthWidthConditionDC = m_iRadioLengthWidthCondition;
	pAlgorithmParam->m_iXYLengthConditionDC = m_iRadioXYLengthCondition;

	pAlgorithmParam->m_iBoundaryConnectionConditionDC = m_iRadioBoundaryConnectionCondition;
	pAlgorithmParam->m_bDefectConditionBoundaryConnection2SideDC = m_bCheckDefectConditionBoundaryConnection2Side;

	pAlgorithmParam->m_iDontcareRegionMargin = m_iEditDontcareRegionMargin;
}

BOOL CAlgorithmDontcareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetDlgStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmDontcareDlg::SetDlgStatus()
{
#if defined(SINGLE_LENS) || defined(ASSY_LENS)
	m_iRadioProcessChType = CHANNEL_TYPE_I;

	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_COLOR)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_R)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_G)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_B)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_H)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_S)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_I)->EnableWindow(FALSE);
#endif
}

void CAlgorithmDontcareDlg::OnBnClickedButtonAreaMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonAreaMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaMax = 10000000;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionLengthMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionLengthMax = 100000;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonWidthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionWidthMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonWidthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionWidthMax = 100000;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonXLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionXLengthMin = 1;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonXLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionXLengthMax = 100000;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonYLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionYLengthMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonYLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionYLengthMax = 100000;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonMeanMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionMeanMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonMeanMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionMeanMax = 255;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonStdevMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionStdevMin = 0.1;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonStdevMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionStdevMax = 1000;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonAnisometryMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionAnisometryMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonAnisometryMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionAnisometryMax = 1000;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonCircularityMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionCircularityMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonCircularityMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionCircularityMax = 1;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonRectangularityMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionRectangularityMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonRectangularityMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionRectangularityMax = 1;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonConvexityMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionConvexityMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonConvexityMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionConvexityMax = 1;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonEllipseRatioMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionEllipseRatioMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDontcareDlg::OnBnClickedButtonEllipseRatioMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionEllipseRatioMax = 1000;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonAreaRatioMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaRatioMin = 1;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonAreaRatioMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaRatioMax = 100;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonContrastMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionContrastMin = 0;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonContrastMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionContrastMax = 255;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonBlobMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionBlobMin = 1;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonBlobMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionBlobMax = 1000;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonShiftXMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionShiftXMin = 0;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonShiftXMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionShiftXMax = 1000;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonShiftYMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionShiftYMin = 0;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedButtonShiftYMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionShiftYMax = 1000;
	UpdateData(FALSE);
}

void CAlgorithmDontcareDlg::OnBnClickedMfcbuttonNextProcessing()
{
	THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->UpdateAlgorithmTab(8);
}
