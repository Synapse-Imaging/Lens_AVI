// AlgorithmSurfaceDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmSurfaceDlg.h"
#include "afxdialogex.h"


// CAlgorithmSurfaceDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmSurfaceDlg, CDialog)

CAlgorithmSurfaceDlg::CAlgorithmSurfaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmSurfaceDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseSurfaceInspection = FALSE;

	m_bCheckUseSurfaceMeasureSaveLog = FALSE;

	m_bCheckUseSurfaceInspectEachROI = FALSE;
	m_iEditSurfaceInspectEachROINgNumber = 1;

	m_bCheckApplyCircleFitting = FALSE;
	m_iEditCircleFittingEdgeStr = 30;
	m_iEditCircleFittingEdgeMargin = 7;

	m_bCheckUseDTPolarTrans = FALSE;

	m_bCheckUseFTConnected = FALSE;
	m_bCheckUseFTConnectedArea = TRUE;
	m_bCheckUseFTConnectedLength = FALSE;
	m_bCheckUseFTConnectedWidth = FALSE;
	m_iEditFTConnectedAreaMin = 100;
	m_iEditFTConnectedLengthMin = 10;
	m_iEditFTConnectedWidthMin = 5;
	m_bCheckUseBarcodeBlob = FALSE;
	m_bCheckUseBlobCornerMeasure = FALSE;
	m_bCheckUseSaveBNEOverflowTop = FALSE;
	m_bCheckUseSaveBNEOverflowBottom = FALSE;

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
	m_bCheckUseBrightHystThres = FALSE;
	m_iEditBrightHystSecureThres = 0;
	m_iEditBrightHystPotentialThres = 0;
	m_iEditBrightHystPotentialLength = 0;

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
	m_bCheckUseDarkHystThres = FALSE;
	m_iEditDarkHystSecureThres = 0;
	m_iEditDarkHystPotentialThres = 0;
	m_iEditDarkHystPotentialLength = 0;
	m_bCheckUseBrightBinThres = FALSE;
	m_bCheckUseDarkBinThres = FALSE;

	m_bCheckUseImageScaling = FALSE;
	m_iRadioImageScaleMethod = 0;
	m_iEditImageScaleMin = 0;
	m_iEditImageScaleMax = 0;

	m_bCheckUseEdgeImage = FALSE;
	m_iRadioEdgeFilterType = 0;
	m_dEditEdgeImageScale = 1.0;

	m_bCheckUsePeak = FALSE;
	m_iEditNormalPeakThresMin = 0;
	m_iEditNormalPeakThresMax = 0;
	m_sEditPeakValue = _T("");
	m_iEditAbnormalPeakHistogramOffset = 0;

	m_bCheckUseHSV = FALSE;
	miEditLowerHue = 0;
	miEditUpperHue = 360;
	miEditLowerSaturation = 0;
	miEditUpperSaturation = 100;
}

CAlgorithmSurfaceDlg::~CAlgorithmSurfaceDlg()
{
	for (int i = 0; i < CHANNEL_NUM; i++)
		GenEmptyObj(&(m_HOrgScanImage[i]));
}

void CAlgorithmSurfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_FIXED_THRESHOLD, m_bCheckUseBrightFixedThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_LOWER_THRES, m_iEditBrightLowerThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UPPER_THRES, m_iEditBrightUpperThres);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_DT, m_bCheckUseBrightDT);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_MED_FILTER_SIZE, m_iEditBrightMedianFilterSize);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_CLOSING_FILTER_SIZE, m_iEditBrightClosingFilterSize);
	DDX_Radio(pDX, IDC_RADIO_BRIGHT_DT_FILTER_TYPE_MEAN, m_iRadioBrightDTFilterType);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_FILTER_SIZE, m_iEditBrightDTFilterSize);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_FILTER_SIZE2, m_iEditBrightDTFilterSize2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_VALUE, m_iEditBrightDTValue);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_UNIFORMITY_CHECK, m_bCheckUseBrightUniformityCheck);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_PEAK_MIN, m_iEditBrightUniformityPeakMin);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_PEAK_MAX, m_iEditBrightUniformityPeakMax);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_OFFSET, m_iEditBrightUniformityOffset);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_HYST_LENGTH, m_iEditBrightUniformityHystLength);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_HYST_OFFSET, m_iEditBrightUniformityHystOffset);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_HYST, m_bCheckUseBrightHystThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_SECURE_THRES, m_iEditBrightHystSecureThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_POTENTIAL_THRES, m_iEditBrightHystPotentialThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_POTENTIAL_LENGTH, m_iEditBrightHystPotentialLength);

	DDX_Check(pDX, IDC_CHECK_USE_DARK_FIXED_THRESHOLD, m_bCheckUseDarkFixedThres);
	DDX_Text(pDX, IDC_EDIT_DARK_LOWER_THRES, m_iEditDarkLowerThres);
	DDX_Text(pDX, IDC_EDIT_DARK_UPPER_THRES, m_iEditDarkUpperThres);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_DT, m_bCheckUseDarkDT);
	DDX_Text(pDX, IDC_EDIT_DARK_MED_FILTER_SIZE, m_iEditDarkMedianFilterSize);
	DDX_Text(pDX, IDC_EDIT_DARK_CLOSING_FILTER_SIZE, m_iEditDarkClosingFilterSize);
	DDX_Radio(pDX, IDC_RADIO_DARK_DT_FILTER_TYPE_MEAN, m_iRadioDarkDTFilterType);
	DDX_Text(pDX, IDC_EDIT_DARK_DT_FILTER_SIZE, m_iEditDarkDTFilterSize);
	DDX_Text(pDX, IDC_EDIT_DARK_DT_FILTER_SIZE2, m_iEditDarkDTFilterSize2);
	DDX_Text(pDX, IDC_EDIT_DARK_DT_VALUE, m_iEditDarkDTValue);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_UNIFORMITY_CHECK, m_bCheckUseDarkUniformityCheck);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_PEAK_MIN, m_iEditDarkUniformityPeakMin);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_PEAK_MAX, m_iEditDarkUniformityPeakMax);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_OFFSET, m_iEditDarkUniformityOffset);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_HYST_LENGTH, m_iEditDarkUniformityHystLength);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_HYST_OFFSET, m_iEditDarkUniformityHystOffset);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_HYST, m_bCheckUseDarkHystThres);
	DDX_Text(pDX, IDC_EDIT_DARK_HYST_SECURE_THRES, m_iEditDarkHystSecureThres);
	DDX_Text(pDX, IDC_EDIT_DARK_HYST_POTENTIAL_THRES, m_iEditDarkHystPotentialThres);
	DDX_Text(pDX, IDC_EDIT_DARK_HYST_POTENTIAL_LENGTH, m_iEditDarkHystPotentialLength);

	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_BIN_THRESHOLD, m_bCheckUseBrightBinThres);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_BIN_THRESHOLD, m_bCheckUseDarkBinThres);

	DDX_Control(pDX, IDC_BUTTON_DISPLAY_SCALED_IMAGE, m_bnDisplayScaledImage);
	DDX_Control(pDX, IDC_BUTTON_DISPLAY_EDGE_IMAGE, m_bnDisplayEdgeImage);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_SCALING, m_bCheckUseImageScaling);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_SCALE_AUTO, m_iRadioImageScaleMethod);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SCAN_MIN, m_iEditImageScaleMin);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SCAN_MAX, m_iEditImageScaleMax);

	DDX_Check(pDX, IDC_CHECK_USE_EDGE_IMAGE, m_bCheckUseEdgeImage);
	DDX_Radio(pDX, IDC_RADIO_EDGE_FILTER_ANY, m_iRadioEdgeFilterType);
	DDX_Text(pDX, IDC_EDIT_EDGE_IMAGE_SCALE, m_dEditEdgeImageScale);

	DDX_Check(pDX, IDC_CHECK_USE_SURFACE_INSPECTION, m_bCheckUseSurfaceInspection);
	DDX_Check(pDX, IDC_CHECK_USE_PEAK, m_bCheckUsePeak);
	DDX_Text(pDX, IDC_EDIT_NORMAL_PEAK_MIN_THRES, m_iEditNormalPeakThresMin);
	DDX_Text(pDX, IDC_EDIT_NORMAL_PEAK_MAX_THRES, m_iEditNormalPeakThresMax);
	DDX_Text(pDX, IDC_EDIT_PEAK_VALUE, m_sEditPeakValue);
	DDX_Text(pDX, IDC_EDIT_ABNORMAL_PEAK_HISTOGRAM_OFFSET, m_iEditAbnormalPeakHistogramOffset);

	DDX_Check(pDX, IDC_CHECK_USE_MEASURE_SAVE_LOG, m_bCheckUseSurfaceMeasureSaveLog);

	DDX_Check(pDX, IDC_CHECK_USE_INSPECT_EACH_ROI, m_bCheckUseSurfaceInspectEachROI);
	DDX_Text(pDX, IDC_EDIT_INSPECT_EACH_ROI_NG_NUMBER, m_iEditSurfaceInspectEachROINgNumber);

	DDX_Check(pDX, IDC_CHECK_APPLY_CIRCLE_FITTING, m_bCheckApplyCircleFitting);
	DDX_Text(pDX, IDC_EDIT_CIRCLE_FITTING_EDGE_STRENGTH, m_iEditCircleFittingEdgeStr);
	DDX_Text(pDX, IDC_EDIT_CIRCLE_FITTING_EDGE_MARGIN, m_iEditCircleFittingEdgeMargin);

	DDX_Check(pDX, IDC_CHECK_USE_DT_POLAR_TRANS, m_bCheckUseDTPolarTrans);

	DDX_Check(pDX, IDC_CHECK_USE_FT_CONNECTED, m_bCheckUseFTConnected);
	DDX_Check(pDX, IDC_CHECK_FT_CONNECTED_AREA, m_bCheckUseFTConnectedArea);
	DDX_Check(pDX, IDC_CHECK_FT_CONNECTED_LENGTH, m_bCheckUseFTConnectedLength);
	DDX_Check(pDX, IDC_CHECK_FT_CONNECTED_WIDTH, m_bCheckUseFTConnectedWidth);
	DDX_Text(pDX, IDC_EDIT_FT_CONNECTED_AREA_MIN, m_iEditFTConnectedAreaMin);
	DDX_Text(pDX, IDC_EDIT_FT_CONNECTED_LENGTH_MIN, m_iEditFTConnectedLengthMin);
	DDX_Text(pDX, IDC_EDIT_FT_CONNECTED_WIDTH_MIN, m_iEditFTConnectedWidthMin);
	DDX_Check(pDX, IDC_CHECK_USE_BARCODE_BLOB, m_bCheckUseBarcodeBlob);
	DDX_Check(pDX, IDC_CHECK_USE_BLOB_CORNER_MEASURE, m_bCheckUseBlobCornerMeasure);

	DDX_Check(pDX, IDC_CHECK_USE_SAVE_BNE_OVERFLOW_TOP, m_bCheckUseSaveBNEOverflowTop);
	DDX_Check(pDX, IDC_CHECK_USE_SAVE_BNE_OVERFLOW_BOTTOM, m_bCheckUseSaveBNEOverflowBottom);

	DDX_Check(pDX, IDC_CHECK_USE_HSV, m_bCheckUseHSV);
	DDX_Text(pDX, IDC_EDIT_HUE_MIN, miEditLowerHue);
	DDX_Text(pDX, IDC_EDIT_HUE_MAX, miEditUpperHue);
	DDX_Text(pDX, IDC_EDIT_SATURATION_MIN, miEditLowerSaturation);
	DDX_Text(pDX, IDC_EDIT_SATURATION_MAX, miEditUpperSaturation);
}


BEGIN_MESSAGE_MAP(CAlgorithmSurfaceDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAY_SCALED_IMAGE, &CAlgorithmSurfaceDlg::OnBnClickedButtonDisplayScaledImage)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_PEAK, &CAlgorithmSurfaceDlg::OnBnClickedButtonShowPeak)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAY_EDGE_IMAGE, &CAlgorithmSurfaceDlg::OnBnClickedButtonDisplayEdgeImage)
	ON_BN_CLICKED(IDC_MFCBUTTON_NEXT_PROCESSING, &CAlgorithmSurfaceDlg::OnBnClickedMfcbuttonNextProcessing)
END_MESSAGE_MAP()


// CAlgorithmSurfaceDlg 메시지 처리기입니다.

BOOL CAlgorithmSurfaceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_bnDisplayScaledImage.Set_Text(_T("영상확인"));
	m_bnDisplayScaledImage.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	//m_bnDisplayEdgeImage.Set_Text(_T("영상확인"));
	m_bnDisplayEdgeImage.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmSurfaceDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;

	for (int i = 0; i < CHANNEL_NUM; i++)
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][m_iSelectedImageType]))
			CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][m_iSelectedImageType], &(m_HOrgScanImage[i]));
	}
}

void CAlgorithmSurfaceDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseSurfaceInspection = AlgorithmParam.m_bUseSurfaceInspection;

	m_bCheckUseSurfaceMeasureSaveLog = AlgorithmParam.m_bUseSurfaceMeasureSaveLog;

	m_bCheckUseSurfaceInspectEachROI = AlgorithmParam.m_bUseSurfaceInspectEachROI;
	m_iEditSurfaceInspectEachROINgNumber = AlgorithmParam.m_iSurfaceInspectEachROINgNumber;

	m_bCheckApplyCircleFitting = AlgorithmParam.m_bApplyCircleFitting;
	m_iEditCircleFittingEdgeStr = AlgorithmParam.m_iCircleFittingEdgeStr;
	m_iEditCircleFittingEdgeMargin = AlgorithmParam.m_iCircleFittingEdgeMargin;

	m_bCheckUseDTPolarTrans = AlgorithmParam.m_bUseDTPolarTrans;

	m_bCheckUseFTConnected = AlgorithmParam.m_bUseFTConnected;
	m_bCheckUseFTConnectedArea = AlgorithmParam.m_bUseFTConnectedArea;
	m_bCheckUseFTConnectedLength = AlgorithmParam.m_bUseFTConnectedLength;
	m_bCheckUseFTConnectedWidth = AlgorithmParam.m_bUseFTConnectedWidth;
	m_iEditFTConnectedAreaMin = AlgorithmParam.m_iFTConnectedAreaMin;
	m_iEditFTConnectedLengthMin = AlgorithmParam.m_iFTConnectedLengthMin;
	m_iEditFTConnectedWidthMin = AlgorithmParam.m_iFTConnectedWidthMin;
	m_bCheckUseBarcodeBlob = AlgorithmParam.m_bUseBarcodeBlob;
	m_bCheckUseBlobCornerMeasure = AlgorithmParam.m_bUseBlobCornerMeasure;
	m_bCheckUseSaveBNEOverflowTop = AlgorithmParam.m_bUseSaveBNEOverflowTop;
	m_bCheckUseSaveBNEOverflowBottom = AlgorithmParam.m_bUseSaveBNEOverflowBottom;

	m_bCheckUseBrightFixedThres = AlgorithmParam.m_bUseBrightFixedThres;
	m_iEditBrightLowerThres = AlgorithmParam.m_iBrightLowerThres;
	m_iEditBrightUpperThres = AlgorithmParam.m_iBrightUpperThres;
	m_bCheckUseBrightDT = AlgorithmParam.m_bUseBrightDT;
	m_iEditBrightMedianFilterSize = AlgorithmParam.m_iBrightMedianFilterSize;
	m_iEditBrightClosingFilterSize = AlgorithmParam.m_iBrightClosingFilterSize;
	m_iRadioBrightDTFilterType = AlgorithmParam.m_iBrightDTFilterType;
	m_iEditBrightDTFilterSize = AlgorithmParam.m_iBrightDTFilterSize;
	m_iEditBrightDTFilterSize2 = AlgorithmParam.m_iBrightDTFilterSize2;
	m_iEditBrightDTValue = AlgorithmParam.m_iBrightDTValue;
	m_bCheckUseBrightUniformityCheck = AlgorithmParam.m_bUseBrightUniformityCheck;
	m_iEditBrightUniformityPeakMin = AlgorithmParam.m_iBrightUniformityPeakMin;
	m_iEditBrightUniformityPeakMax = AlgorithmParam.m_iBrightUniformityPeakMax;
	m_iEditBrightUniformityOffset = AlgorithmParam.m_iBrightUniformityOffset;
	m_iEditBrightUniformityHystLength = AlgorithmParam.m_iBrightUniformityHystLength;
	m_iEditBrightUniformityHystOffset = AlgorithmParam.m_iBrightUniformityHystOffset;
	m_bCheckUseBrightHystThres = AlgorithmParam.m_bUseBrightHystThres;
	m_iEditBrightHystSecureThres = AlgorithmParam.m_iBrightHystSecureThres;
	m_iEditBrightHystPotentialThres = AlgorithmParam.m_iBrightHystPotentialThres;
	m_iEditBrightHystPotentialLength = AlgorithmParam.m_iBrightHystPotentialLength;

	m_bCheckUseDarkFixedThres = AlgorithmParam.m_bUseDarkFixedThres;
	m_iEditDarkLowerThres = AlgorithmParam.m_iDarkLowerThres;
	m_iEditDarkUpperThres = AlgorithmParam.m_iDarkUpperThres;
	m_bCheckUseDarkDT = AlgorithmParam.m_bUseDarkDT;
	m_iEditDarkMedianFilterSize = AlgorithmParam.m_iDarkMedianFilterSize;
	m_iEditDarkClosingFilterSize = AlgorithmParam.m_iDarkClosingFilterSize;
	m_iRadioDarkDTFilterType = AlgorithmParam.m_iDarkDTFilterType;
	m_iEditDarkDTFilterSize = AlgorithmParam.m_iDarkDTFilterSize;
	m_iEditDarkDTFilterSize2 = AlgorithmParam.m_iDarkDTFilterSize2;
	m_iEditDarkDTValue = AlgorithmParam.m_iDarkDTValue;
	m_bCheckUseDarkUniformityCheck = AlgorithmParam.m_bUseDarkUniformityCheck;
	m_iEditDarkUniformityPeakMin = AlgorithmParam.m_iDarkUniformityPeakMin;
	m_iEditDarkUniformityPeakMax = AlgorithmParam.m_iDarkUniformityPeakMax;
	m_iEditDarkUniformityOffset = AlgorithmParam.m_iDarkUniformityOffset;
	m_iEditDarkUniformityHystLength = AlgorithmParam.m_iDarkUniformityHystLength;
	m_iEditDarkUniformityHystOffset = AlgorithmParam.m_iDarkUniformityHystOffset;
	m_bCheckUseDarkHystThres = AlgorithmParam.m_bUseDarkHystThres;
	m_iEditDarkHystSecureThres = AlgorithmParam.m_iDarkHystSecureThres;
	m_iEditDarkHystPotentialThres = AlgorithmParam.m_iDarkHystPotentialThres;
	m_iEditDarkHystPotentialLength = AlgorithmParam.m_iDarkHystPotentialLength;
	m_bCheckUseBrightBinThres = AlgorithmParam.m_bUseBrightBinThres;
	m_bCheckUseDarkBinThres = AlgorithmParam.m_bUseDarkBinThres;

	m_bCheckUseImageScaling = AlgorithmParam.m_bUseImageScaling;
	m_iRadioImageScaleMethod = AlgorithmParam.m_iImageScaleMethod;
	m_iEditImageScaleMin = AlgorithmParam.m_iImageScaleMin;
	m_iEditImageScaleMax = AlgorithmParam.m_iImageScaleMax;

	m_bCheckUseEdgeImage = AlgorithmParam.m_bUseEdgeImage;
	m_iRadioEdgeFilterType = AlgorithmParam.m_iEdgeFilterType;
	m_dEditEdgeImageScale = AlgorithmParam.m_dEdgeImageScale;

	m_bCheckUsePeak = AlgorithmParam.m_bUsePeak;
	m_iEditNormalPeakThresMin = AlgorithmParam.m_iNormalPeakThresMin;
	m_iEditNormalPeakThresMax = AlgorithmParam.m_iNormalPeakThresMax;
	m_iEditAbnormalPeakHistogramOffset = AlgorithmParam.m_iAbnormalPeakHistogramOffset;

	m_bCheckUseHSV = AlgorithmParam.m_bUseHSV;
	miEditLowerHue = AlgorithmParam.miLowerHue;
	miEditUpperHue = AlgorithmParam.miUpperHue;
	miEditLowerSaturation = AlgorithmParam.miLowerSaturation;
	miEditUpperSaturation = AlgorithmParam.miUpperSaturation;
	UpdateData(FALSE);
}

void CAlgorithmSurfaceDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseSurfaceInspection = m_bCheckUseSurfaceInspection;

	pAlgorithmParam->m_bUseSurfaceMeasureSaveLog = m_bCheckUseSurfaceMeasureSaveLog;

	pAlgorithmParam->m_bUseSurfaceInspectEachROI = m_bCheckUseSurfaceInspectEachROI;
	pAlgorithmParam->m_iSurfaceInspectEachROINgNumber = m_iEditSurfaceInspectEachROINgNumber;

	pAlgorithmParam->m_bApplyCircleFitting = m_bCheckApplyCircleFitting;
	pAlgorithmParam->m_iCircleFittingEdgeStr = m_iEditCircleFittingEdgeStr;
	pAlgorithmParam->m_iCircleFittingEdgeMargin = m_iEditCircleFittingEdgeMargin;

	pAlgorithmParam->m_bUseDTPolarTrans = m_bCheckUseDTPolarTrans;

	pAlgorithmParam->m_bUseFTConnected = m_bCheckUseFTConnected;
	pAlgorithmParam->m_bUseFTConnectedArea = m_bCheckUseFTConnectedArea;
	pAlgorithmParam->m_bUseFTConnectedLength = m_bCheckUseFTConnectedLength;
	pAlgorithmParam->m_bUseFTConnectedWidth = m_bCheckUseFTConnectedWidth;
	pAlgorithmParam->m_iFTConnectedAreaMin = m_iEditFTConnectedAreaMin;
	pAlgorithmParam->m_iFTConnectedLengthMin = m_iEditFTConnectedLengthMin;
	pAlgorithmParam->m_iFTConnectedWidthMin = m_iEditFTConnectedWidthMin;
	pAlgorithmParam->m_bUseBarcodeBlob = m_bCheckUseBarcodeBlob;
	pAlgorithmParam->m_bUseBlobCornerMeasure = m_bCheckUseBlobCornerMeasure;

	pAlgorithmParam->m_bUseSaveBNEOverflowTop = m_bCheckUseSaveBNEOverflowTop;
	pAlgorithmParam->m_bUseSaveBNEOverflowBottom = m_bCheckUseSaveBNEOverflowBottom;

	pAlgorithmParam->m_bUseBrightFixedThres = m_bCheckUseBrightFixedThres;
	pAlgorithmParam->m_iBrightLowerThres = m_iEditBrightLowerThres;
	pAlgorithmParam->m_iBrightUpperThres = m_iEditBrightUpperThres;
	pAlgorithmParam->m_bUseBrightDT = m_bCheckUseBrightDT;
	pAlgorithmParam->m_iBrightMedianFilterSize = m_iEditBrightMedianFilterSize;
	pAlgorithmParam->m_iBrightClosingFilterSize = m_iEditBrightClosingFilterSize;
	pAlgorithmParam->m_iBrightDTFilterType = m_iRadioBrightDTFilterType;
	pAlgorithmParam->m_iBrightDTFilterSize = m_iEditBrightDTFilterSize;
	pAlgorithmParam->m_iBrightDTFilterSize2 = m_iEditBrightDTFilterSize2;
	pAlgorithmParam->m_iBrightDTValue = m_iEditBrightDTValue;
	pAlgorithmParam->m_bUseBrightUniformityCheck = m_bCheckUseBrightUniformityCheck;
	pAlgorithmParam->m_iBrightUniformityPeakMin = m_iEditBrightUniformityPeakMin;
	pAlgorithmParam->m_iBrightUniformityPeakMax = m_iEditBrightUniformityPeakMax;
	pAlgorithmParam->m_iBrightUniformityOffset = m_iEditBrightUniformityOffset;
	pAlgorithmParam->m_iBrightUniformityHystLength = m_iEditBrightUniformityHystLength;
	pAlgorithmParam->m_iBrightUniformityHystOffset = m_iEditBrightUniformityHystOffset;
	pAlgorithmParam->m_bUseBrightHystThres = m_bCheckUseBrightHystThres;
	pAlgorithmParam->m_iBrightHystSecureThres = m_iEditBrightHystSecureThres;
	pAlgorithmParam->m_iBrightHystPotentialThres = m_iEditBrightHystPotentialThres;
	pAlgorithmParam->m_iBrightHystPotentialLength = m_iEditBrightHystPotentialLength;

	pAlgorithmParam->m_bUseDarkFixedThres = m_bCheckUseDarkFixedThres;
	pAlgorithmParam->m_iDarkLowerThres = m_iEditDarkLowerThres;
	pAlgorithmParam->m_iDarkUpperThres = m_iEditDarkUpperThres;
	pAlgorithmParam->m_bUseDarkDT = m_bCheckUseDarkDT;
	pAlgorithmParam->m_iDarkMedianFilterSize = m_iEditDarkMedianFilterSize;
	pAlgorithmParam->m_iDarkClosingFilterSize = m_iEditDarkClosingFilterSize;
	pAlgorithmParam->m_iDarkDTFilterType = m_iRadioDarkDTFilterType;
	pAlgorithmParam->m_iDarkDTFilterSize = m_iEditDarkDTFilterSize;
	pAlgorithmParam->m_iDarkDTFilterSize2 = m_iEditDarkDTFilterSize2;
	pAlgorithmParam->m_iDarkDTValue = m_iEditDarkDTValue;
	pAlgorithmParam->m_bUseDarkUniformityCheck = m_bCheckUseDarkUniformityCheck;
	pAlgorithmParam->m_iDarkUniformityPeakMin = m_iEditDarkUniformityPeakMin;
	pAlgorithmParam->m_iDarkUniformityPeakMax = m_iEditDarkUniformityPeakMax;
	pAlgorithmParam->m_iDarkUniformityOffset = m_iEditDarkUniformityOffset;
	pAlgorithmParam->m_iDarkUniformityHystLength = m_iEditDarkUniformityHystLength;
	pAlgorithmParam->m_iDarkUniformityHystOffset = m_iEditDarkUniformityHystOffset;
	pAlgorithmParam->m_bUseDarkHystThres = m_bCheckUseDarkHystThres;
	pAlgorithmParam->m_iDarkHystSecureThres = m_iEditDarkHystSecureThres;
	pAlgorithmParam->m_iDarkHystPotentialThres = m_iEditDarkHystPotentialThres;
	pAlgorithmParam->m_iDarkHystPotentialLength = m_iEditDarkHystPotentialLength;
	pAlgorithmParam->m_bUseBrightBinThres = m_bCheckUseBrightBinThres;
	pAlgorithmParam->m_bUseDarkBinThres = m_bCheckUseDarkBinThres;

	pAlgorithmParam->m_bUseImageScaling = m_bCheckUseImageScaling;
	pAlgorithmParam->m_iImageScaleMethod = m_iRadioImageScaleMethod;
	pAlgorithmParam->m_iImageScaleMin = m_iEditImageScaleMin;
	pAlgorithmParam->m_iImageScaleMax = m_iEditImageScaleMax;

	pAlgorithmParam->m_bUseEdgeImage = m_bCheckUseEdgeImage;
	pAlgorithmParam->m_iEdgeFilterType = m_iRadioEdgeFilterType;
	pAlgorithmParam->m_dEdgeImageScale = m_dEditEdgeImageScale;

	pAlgorithmParam->m_bUsePeak = m_bCheckUsePeak;
	pAlgorithmParam->m_iNormalPeakThresMin = m_iEditNormalPeakThresMin;
	pAlgorithmParam->m_iNormalPeakThresMax = m_iEditNormalPeakThresMax;
	pAlgorithmParam->m_iAbnormalPeakHistogramOffset = m_iEditAbnormalPeakHistogramOffset;

	pAlgorithmParam->m_bUseHSV = m_bCheckUseHSV;
	pAlgorithmParam->miLowerHue = miEditLowerHue;
	pAlgorithmParam->miUpperHue = miEditUpperHue;
	pAlgorithmParam->miLowerSaturation = miEditLowerSaturation;
	pAlgorithmParam->miUpperSaturation = miEditUpperSaturation;
}

void CAlgorithmSurfaceDlg::OnBnClickedButtonDisplayScaledImage()
{
	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];

	UpdateData(TRUE);

	AlgorithmParam.m_iImageScaleMethod = m_iRadioImageScaleMethod;
	AlgorithmParam.m_iImageScaleMin = m_iEditImageScaleMin;
	AlgorithmParam.m_iImageScaleMax = m_iEditImageScaleMax;

	BOOL bRet;
	HObject HScaledImage;

	HObject HROIHRegion;
	HROIHRegion = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

	bRet = THEAPP.m_pAlgorithm->GetScaledImage(m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], HROIHRegion, AlgorithmParam, &HScaledImage);

	if (bRet)
	{
		CopyImage(HScaledImage, &THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]);

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
}

void CAlgorithmSurfaceDlg::OnBnClickedButtonShowPeak()
{
	int iPeakValue = -1;
	iPeakValue = THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetPeak();

	if (iPeakValue == -1)
	{
		UpdateData(TRUE);
		UpdateData(FALSE);

		m_sEditPeakValue.Format("Fail");
		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);
		UpdateData(FALSE);

		m_sEditPeakValue.Format("%d", iPeakValue);
		UpdateData(FALSE);
	}
}

void CAlgorithmSurfaceDlg::OnBnClickedButtonDisplayEdgeImage()
{
	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];

	UpdateData(TRUE);

	AlgorithmParam.m_iEdgeFilterType = m_iRadioEdgeFilterType;
	AlgorithmParam.m_dEdgeImageScale = m_dEditEdgeImageScale;

	BOOL bRet;
	HObject HEdgeImage;

	HObject HROIHRegion;
	HROIHRegion = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

	bRet = THEAPP.m_pAlgorithm->GetEdgeImage(m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], HROIHRegion, AlgorithmParam, &HEdgeImage);

	if (bRet)
	{
		CopyImage(HEdgeImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
}

void CAlgorithmSurfaceDlg::OnBnClickedMfcbuttonNextProcessing()
{
	THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->UpdateAlgorithmTab(10);
}
