#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmParam.h"


CAlgorithmParam::CAlgorithmParam(void)
{
	Reset();
}


CAlgorithmParam::~CAlgorithmParam(void)
{
}

void CAlgorithmParam::Reset()
{
	m_bInspect = FALSE;

	m_bUseFilter1 = FALSE;
	m_bUseFilter2 = FALSE;
	m_bUseFilter3 = FALSE;
	m_bUseFilter4 = FALSE;

	m_bCheckAIUseConsiderImageNo = FALSE;
	m_bCheckAIUseConsiderDefectCode = FALSE;

	m_iDefectType = 0;

	m_bUseInspectArea[0] = TRUE;
	m_iInspectAreaContour1Margin[0] = 0;
	m_iInspectAreaContour2Margin[0] = -5000;
	m_bUseInspectArea[1] = FALSE;
	m_iInspectAreaContour1Margin[1] = 0;
	m_iInspectAreaContour2Margin[1] = -5000;
	m_bUseInspectArea[2] = FALSE;
	m_iInspectAreaContour1Margin[2] = 0;
	m_iInspectAreaContour2Margin[2] = -5000;

	m_bUseSurfaceInspection = FALSE;
	m_bUseSurfaceMeasureSaveLog = FALSE;

	m_bUseSurfaceInspectEachROI = FALSE;
	m_iSurfaceInspectEachROINgNumber = 1;

	m_bApplyCircleFitting = FALSE;
	m_iCircleFittingEdgeStr = 30;
	m_iCircleFittingEdgeMargin = 7;

	m_bUseDTPolarTrans = FALSE;

	m_bUseFTConnected = FALSE;
	m_bUseFTConnectedArea = TRUE;
	m_bUseFTConnectedLength = FALSE;
	m_bUseFTConnectedWidth = FALSE;
	m_iFTConnectedAreaMin = 100;
	m_iFTConnectedLengthMin = 10;
	m_iFTConnectedWidthMin = 5;
	m_bUseBarcodeBlob = FALSE;
	m_bUseBlobCornerMeasure = FALSE;
	m_bUseSaveBNEOverflowTop = FALSE;
	m_bUseSaveBNEOverflowBottom = FALSE;

	m_bUseBrightFixedThres = FALSE;
	m_iBrightLowerThres = 200;
	m_iBrightUpperThres = 255;
	m_bUseBrightDT = FALSE;
	m_iBrightMedianFilterSize = 1;
	m_iBrightClosingFilterSize = 0;
	m_iBrightDTFilterType = 0;
	m_iBrightDTFilterSize = 51;
	m_iBrightDTFilterSize2 = 51;
	m_iBrightDTValue = 20;
	m_bUseBrightUniformityCheck = FALSE;
	m_iBrightUniformityPeakMin = 0;
	m_iBrightUniformityPeakMax = 255;
	m_iBrightUniformityOffset = 40;
	m_iBrightUniformityHystLength = -1;
	m_iBrightUniformityHystOffset = 10;
	m_bUseBrightHystThres = FALSE;
	m_iBrightHystSecureThres = 190;
	m_iBrightHystPotentialThres = 175;
	m_iBrightHystPotentialLength = 5;

	m_bUseDarkFixedThres = FALSE;
	m_iDarkLowerThres = 0;
	m_iDarkUpperThres = 55;
	m_bUseDarkDT = FALSE;
	m_iDarkMedianFilterSize = 1;
	m_iDarkClosingFilterSize = 0;
	m_iDarkDTFilterType = 0;
	m_iDarkDTFilterSize = 51;
	m_iDarkDTFilterSize2 = 51;
	m_iDarkDTValue = 20;
	m_bUseDarkUniformityCheck = FALSE;
	m_iDarkUniformityPeakMin = 0;
	m_iDarkUniformityPeakMax = 255;
	m_iDarkUniformityOffset = 40;
	m_iDarkUniformityHystLength = -1;
	m_iDarkUniformityHystOffset = 10;
	m_bUseDarkHystThres = FALSE;
	m_iDarkHystSecureThres = 50;
	m_iDarkHystPotentialThres = 65;
	m_iDarkHystPotentialLength = 5;
	m_bUseBrightBinThres = FALSE;
	m_bUseDarkBinThres = FALSE;

	m_iCandidateMinArea = 1;
	m_bCombineBrightDarkBlob = FALSE;
	m_bMaxBlob = FALSE;
	m_bClosingOpening = FALSE;
	m_bApplyOneDirMorp = FALSE;
	m_iOneDirMorpXOpening = 0;
	m_iOneDirMorpXClosing = 0;
	m_iOpeningSize = 0;
	m_iClosingSize = 1;
	m_iThresholdOpeningSize = 2;
	m_iThresholdClosingSize = 2;
	m_bFillup = TRUE;
	m_bUseConvex = FALSE;
	m_bUseSubConvex = FALSE;
	m_bUseRect = FALSE;
	m_bUseConnection = FALSE;
	m_iConnectionMinSize = 5;
	m_iConnectionMinXLength = 1;
	m_iConnectionLength = 10;
	m_bUseAngle = FALSE;
	m_iConnectionMaxWidth = 6;
	m_iHoleFillCondition = HOLE_FILL_ALL;
	m_iFillHoleArea = 20;
	m_bXYLengthMeasureIgnoreHole = FALSE;
	m_bXYLengthMaxLineLength = FALSE;
	m_bDefectConditionBoundaryConnectionAny = FALSE;
	m_iDefectConditionBoundaryConnectionOuter = ROI_CONNECTION_OUTER;
	m_iDefectConditionBoundaryMarginAny = 5;
	m_iDefectConditionBoundaryMarginMinConnectionAny = 1;

	m_bUseDefectConditionArea = TRUE;
	m_bUseDefectConditionLength = FALSE;
	m_bUseDefectConditionWidth = FALSE;
	m_bUseDefectConditionXLength = FALSE;
	m_bUseDefectConditionYLength = FALSE;
	m_bUseDefectConditionMean = FALSE;
	m_bUseDefectConditionStdev = FALSE;
	m_bUseDefectConditionAnisometry = FALSE;
	m_bUseDefectConditionCircularity = FALSE;
	m_bUseDefectConditionRectangularity = FALSE;
	m_bUseDefectConditionConvexity = FALSE;
	m_bUseDefectConditionEllipseRatio = FALSE;
	m_bUseDefectConditionAreaRatio = FALSE;
	m_bUseDefectConditionContrast = FALSE;
	m_bUseDefectConditionGrad = FALSE;
	m_bUseDefectConditionBlob = FALSE;
	m_bUseDefectConditionShiftX = FALSE;
	m_bUseDefectConditionShiftY = FALSE;
	m_bUseDefectConditionOuterDist = FALSE;
	m_bUseDefectConditionInnerDist = FALSE;
	m_bUseDefectConditionUseZeroArea = FALSE;
	m_bDefectConditionBoundaryConnection = FALSE;
	m_bDefectConditionBoundaryMarginTop = FALSE;
	m_bDefectConditionBoundaryMarginBottom = FALSE;
	m_bDefectConditionBoundaryMarginLeft = FALSE;
	m_bDefectConditionBoundaryMarginRight = FALSE;
	m_bDefectConditionBoundaryMarginHCenter = FALSE;
	m_bDefectConditionBoundaryMarginVCenter = FALSE;

	m_iDefectConditionAreaMin = 100;
	m_iDefectConditionLengthMin = 10;
	m_iDefectConditionWidthMin = 2;
	m_iDefectConditionXLengthMin = 10;
	m_iDefectConditionYLengthMin = 10;
	m_iDefectConditionMeanMin = 60;
	m_dDefectConditionStdevMin = 1.0;
	m_dDefectConditionAnisometryMin = 1.0;
	m_dDefectConditionCircularityMin = 0.7;
	m_dDefectConditionRectangularityMin = 0.5;
	m_dDefectConditionConvexityMin = 0.8;
	m_dDefectConditionEllipseRatioMin = 1.0;
	m_iDefectConditionAreaRatioMin = 0;
	m_iDefectConditionContrastMin = 10;
	m_iDefectConditionGradMin = 20;
	m_iDefectConditionBlobMin = 1;
	m_iDefectConditionShiftXMin = 0;
	m_iDefectConditionShiftYMin = 0;

	m_iDefectConditionAreaMax = 1000000;
	m_iDefectConditionLengthMax = 100000;
	m_iDefectConditionWidthMax = 100000;
	m_iDefectConditionXLengthMax = 100000;
	m_iDefectConditionYLengthMax = 100000;
	m_iDefectConditionMeanMax = 255;
	m_dDefectConditionStdevMax = 50.0;
	m_dDefectConditionAnisometryMax = 5.0;
	m_dDefectConditionCircularityMax = 1.0;
	m_dDefectConditionRectangularityMax = 1.0;
	m_dDefectConditionConvexityMax = 1.0;
	m_dDefectConditionEllipseRatioMax = 5.0;
	m_iDefectConditionAreaRatioMax = 20;
	m_iDefectConditionContrastMax = 255;
	m_iDefectConditionGradMax = 255;
	m_iDefectConditionBlobMax = 1000;
	m_iDefectConditionShiftXMax = 1000;
	m_iDefectConditionShiftYMax = 1000;

	m_iDefectConditionOuterDist = 0;
	m_iDefectConditionInnerDist = 0;

	m_iDefectConditionBoundaryMarginTop = 5;
	m_iDefectConditionBoundaryMarginBottom = 5;
	m_iDefectConditionBoundaryMarginLeft = 5;
	m_iDefectConditionBoundaryMarginRight = 5;
	m_iDefectConditionBoundaryMarginHCenter = 5;
	m_iDefectConditionBoundaryMarginVCenter = 5;
	m_iDefectConditionBoundaryMarginMinConnection = 1;

	m_bUseImageScaling = FALSE;
	m_iImageScaleMethod = IMAGE_SCALE_AUTO;
	m_iImageScaleMin = 0;
	m_iImageScaleMax = 50;

	m_bUseEdgeImage = FALSE;
	m_iEdgeFilterType = IMAGE_EDGE_X_DIR;
	m_dEdgeImageScale = 1.0;

	m_bUsePeak = FALSE;
	m_iNormalPeakThresMin = 80;
	m_iNormalPeakThresMax = 120;
	m_iAbnormalPeakHistogramOffset = 20;

	m_iLengthWidthCondition = CONDITION_AND;
	m_iXYLengthCondition = CONDITION_AND;

	m_iBoundaryConnectionCondition = CONDITION_OR;
	m_bDefectConditionBoundaryConnection2Side = FALSE;
	m_bDefectConditionInnerOuterBoundaryConnection = FALSE;

	m_iXLengthCondition = LENGTH_MAX;
	m_iXLengthConditionMinRef = LENGTH_MIN_LEFT;
	m_iYLengthCondition = LENGTH_MAX;
	m_iYLengthConditionMinRef = LENGTH_MIN_BOTTOM;

	m_bUseDefectConditionHole = FALSE;
	m_iDefectConditionHoleArea = 5;

	m_bDefectConditionUnionBlob = FALSE;
	m_bDefectConditionDilation = FALSE;
	m_iDefectConditionDilationSize = 0;

	//VER 1.1.3.2 add Verification of height near the protrusion 
	m_bDefectConditionOuterDiff = FALSE;
	m_nDefectConditionOuterDiffInterval = 5;
	m_nDefectConditionOuterDiffMinDiff = 3;
	m_nOuterDiffUpper = 0;
	m_nDefectConditionOuterDiffMaxRepeat = 2;
	//VER 1.1.3.2 end

	//VER 1.1.3.3 add Calculate Blob Length From ConerLength
	m_nDefectConditionConerLengthDir = 0;
	m_bDefectConditionConerLengthUse = FALSE;
	m_nDefectConditionConerLengthMin = 1;
	m_nDefectConditionConerLengthMax = 100000;
	//VER 1.1.3.3 end

	//VER 1.1.3.4 add
	m_bDefectConditionConerLengthMaxUse = FALSE;
	//VER 1.1.3.4 end

	m_bUseDefectConditionXLengthSum = FALSE;
	m_iDefectConditionXLengthSumMin = 10;
	m_iDefectConditionXLengthSumMax = 100000;
	m_bUseDefectConditionYLengthSum = FALSE;
	m_iDefectConditionYLengthSumMin = 10;
	m_iDefectConditionYLengthSumMax = 100000;

	m_bDefectConditionOrthoLength = FALSE;
	m_iOrthoLengthRefSide = ORTHO_LENGTH_TOP;

	// Line Fitting
	m_bUseLineFitting = FALSE;
	m_iLineFitObjectLowerThres = 40;
	m_iLineFitObjectUpperThres = 255;
	m_bLineFitUseOtherImageFitting = FALSE;
	m_iLineFitOtherImageNumber = 1;
	m_dLineFitFittingEdgeSF = 5.0;
	m_dLineFitObjectEdgeSF = 5.0;
	m_iLineFitMinContourLength = 100;
	m_iLineFitContourConnectionLength = 10;
	m_bLineFitUseBurInspection = FALSE;
	m_dLineFitBurDefectLength = 4.0;
	m_dLineFitBurDefectLengthMax = 10000.0;
	m_bLineFitUseChippingInspection = FALSE;
	m_dLineFitChippingDefectLength = 4.0;
	m_dLineFitChippingDefectLengthMax = 10000.0;
	m_bLineFitUseRotationInspection = FALSE;
	m_iLineFitRotationReference = 0;
	m_dLineFitRotationDefectAngle = 1.0;
	m_iLineFitEdgeMethod = 1;
	m_iLineFitFittingEdgeHigh = 30;
	m_iLineFitFittingMinContourLength = 50;
	m_iLineFitFittingContourConnectionLength = 10;
	m_iLineFitEdgeHigh = 20;
	m_bLineFitLengthSave = FALSE;
	m_bNoUseLineFit = FALSE;
	m_bLineFitLocalAlignUse = FALSE;

	m_bUsePinLength = FALSE;
	m_iPinPos = 0;
	m_iPinDTFilterSize = 101;
	m_iPinOpeningClosingSize = 1;
	m_iPinMinArea = 100;
	m_iPinDistanceTolerance = 9;
	m_iPinDTValue = 50;
	m_iPinDefectNumberTolerance = 1;
	m_iPinPitch = 25;

	m_bUseDent = FALSE;
	m_iDentMedFilterSize = 1;
	m_iDentDtFilterSize = 51;
	m_iDentBrightDtValue = 25;
	m_iDentDarkDtValue = 18;
	m_iDentCandidateMinArea = 30;
	m_iDentCandidateMinWidth = 6;
	m_iDentCandidateMinDistance = 3;
	m_iDentMinArea = 300;

	m_bUseImageCompare = FALSE;
	m_bImageCompareSaveFeatureLog = FALSE;
	m_bUseImageCompareWindow = FALSE;
	m_iImageCompareWindowSize = 30;
	m_bUseImageCompareEdgeImage = FALSE;
	m_bUseImageCompareNormalizer = FALSE;
	m_iImageCompareNormalizeWeight = 20;
	m_bUseImageCompareBright = TRUE;
	m_bUseImageCompareDark = TRUE;
	m_iImageCompareBrightAbs = 20;
	m_iImageCompareDarkAbs = 20;
	m_dImageCompareBrightVar = 3.0;
	m_dImageCompareDarkVar = 3.0;
	m_bUseImageCompareAutoTrain = FALSE;
	m_bUseImageCompareMod = FALSE;
	m_bUseImageCompareModBright = TRUE;
	m_bUseImageCompareModDark = TRUE;
	m_iImageCompareModBrightAbs = 50;
	m_iImageCompareModDarkAbs = 50;
	m_dImageCompareModBrightVar = 4;
	m_dImageCompareModDarkVar = 4;
	m_iImageCompareModTop = 0;
	m_iImageCompareModBottom = 0;
	m_iImageCompareModLeft = 0;
	m_iImageCompareModRight = 0;

	m_bUsePartCheck = FALSE;
	m_bPartCheckMultipleModel = FALSE;
	m_iPartCheckMatchingAngleRange = 0;
	m_iPartCheckMatchingPLevel = 0;
	m_iPartCheckMatchingScore = 70;
	m_iPartCheckMatchingSearchMarginX = 50;
	m_iPartCheckMatchingSearchMarginY = 50;
	m_iPartCheckMatchingSearchMarginX2 = 50;
	m_iPartCheckMatchingSearchMarginY2 = 50;
	m_bPartCheckInspectExistence = FALSE;
	m_bPartCheckBlob = FALSE;
	m_bPartCheckInspectShift = FALSE;
	m_iPartCheckShiftX = 10;
	m_iPartCheckShiftY = 10;
	m_dPartCheckRotationAngle = 15;
	m_bPartCheckLocalAlignUse = FALSE;
	m_bPartCheckApplyComp = FALSE;
	m_bPartCheckSaveLog = FALSE;
	m_bPartCheckMeasureEdge = FALSE;
	m_iPartCheckLegXSize = 24;
	m_iPartCheckLegYSize = 80;
	m_dPartCheckEdgeMeasueSmFactor = 1.0;
	m_dPartCheckEdgeMeasueEdgeStr = 10.0;
	m_bPartCheckInspectLegDamage = FALSE;
	m_bPartCheckMultiModuleInspect = FALSE;
	m_iPartCheckMultiModuleXNumber = 1;
	m_iPartCheckMultiModuleYNumber = 1;
	m_iPartCheckMultiModuleXPitch = 1000;
	m_iPartCheckMultiModuleYPitch = 1000;
	m_bPartCheckPickerPosUse = FALSE;
	m_bPartCheckMultiModuleMoveOffset = FALSE;
	m_iPartCheckMultiModuleTarget = MULTI_MODULE_PART_CHECK_MODULE;

	for (int i = 0; i < ALIGN_MODULE_Y; i++)
	{
		for (int j = 0; j < ALIGN_MODULE_X; j++)
		{
			m_iPartCheckMoveOffsetX[i][j] = 0;
			m_iPartCheckMoveOffsetY[i][j] = 0;
		}
	}

	m_bUseConcenter = FALSE;
	m_iConcenterMatchingScore = 50;
	m_iConcenterInnerCircleDiameter = 50;
	m_dConcenterMatchingScaleMin = 0.95;
	m_dConcenterMatchingScaleMax = 1.05;
	m_iConcenterMatchingMinContrast = 15;
	m_iConcenterCenterDiffSpec = 30;
	m_dConcenterCenterDiffSpecX = 30;
	m_dConcenterCenterDiffSpecY = 30;

	m_bUseHomerMark = FALSE;
	m_iHomerMarkPos = 0;
	m_iHomerMarkOrder = 0;

	m_bUseROIAlign = FALSE;
	m_iROIAlignXSearchMargin = 30;
	m_iROIAlignYSearchMargin = 30;
	m_iROIAlignMatchingAngleRange = 0;
	m_dROIAlignMatchingScaleMin = 1.0;
	m_dROIAlignMatchingScaleMax = 1.0;
	m_iROIAlignMatchingMinContrast = 15;
	m_bROIAlignUsePosX = FALSE;
	m_bROIAlignUsePosY = FALSE;
	m_dROIAlignMatchingScore = 0.3;
	m_bROIAlignInspectShift = FALSE;
	m_iROIAlignMatchingPLevel = 0;
	m_dROIAlignShiftTop = 10;
	m_dROIAlignShiftBottom = 10;
	m_dROIAlignShiftLeft = 10;
	m_dROIAlignShiftRight = 10;
	m_bROIAlignLocalAlignUse = FALSE;
	m_bUseIsoColorImage = FALSE;
	m_bROIAlignMultiModuleInspect = FALSE;
	m_iROIAlignMultiModuleXNumber = 1;
	m_iROIAlignMultiModuleYNumber = 1;
	m_iROIAlignMultiModuleXPitch = 1000;
	m_iROIAlignMultiModuleYPitch = 1000;
	m_iROIAlignMultiModuleAlignTarget = MULTI_MODULE_ALIGN_TARGET_MODULE;
	m_iROIAlignMultiModuleVision = MULTI_MODULE_VISION_TOP1;

	m_iProcessChType = CHANNEL_TYPE_I;
	m_iProcessResType = RES_TYPE_5;
	m_bUseImageProcess = FALSE;
	m_iImageProcessArithmeticType = IMAGE_PROCESS_ARITHMETIC_ADD;
	m_iImageProcessArithmeticImage1 = 0;
	m_iImageProcessArithmeticImage2 = 0;
	m_iImageProcessArithmeticImage3 = 0;
	m_iImageProcessArithmeticImage4 = 0;
	m_dImageProcessArithmeticMulti = 1.0;
	m_iImageProcessArithmeticAdd = 0;

	m_bUseImageProcessFilter = FALSE;
	m_iImageProcessFilterType1 = FILTER_TYPE_NOT_USED;
	m_iImageProcessFilterType2 = FILTER_TYPE_NOT_USED;
	m_iImageProcessFilterType3 = FILTER_TYPE_NOT_USED;
	m_iImageProcessFilterType1X = 7;
	m_iImageProcessFilterType2X = 7;
	m_iImageProcessFilterType3X = 7;
	m_iImageProcessFilterType1Y = 7;
	m_iImageProcessFilterType2Y = 7;
	m_iImageProcessFilterType3Y = 7;

	m_bUseImageProcessLocalAlign = FALSE;
	m_iImageProcessLocalAlignImageIndex = 0;
	m_iImageProcessLocalAlignROINo = 1;

	m_bUseImageProcessPostLocalAlign = FALSE;
	m_iImageProcessPostLocalAlignImageIndex = 0;
	m_iImageProcessPostLocalAlignROINo = 1;

	m_bImageProcessLocalAlignPosX = TRUE;
	m_bImageProcessLocalAlignPosY = TRUE;
	m_bImageProcessLocalAlignAngle = TRUE;
	m_iImageProcessLocalAlignFitPos = LOCAL_ALIGN_FIT_LINE_ROI_BOTTOM;
	m_iImageProcessPostLocalAlignFitPos = POST_LOCAL_ALIGN_ROTATION_CENTER_MODELCENTER;
	m_iImageProcessLocalAlignROIType = LOCAL_ALIGN_MATCHING_ROI_TYPE_MATCHING;
	m_iImageProcessLocalAlignMatchingApplyMethod = LOCAL_ALIGN_MATCHING_ROI_POSITION;
	m_bUseImageProcessDontCare = FALSE;
	m_iImageProcessDontCareImageIndex = 0;
	m_iImageProcessDontCareROINo = 1;
	m_iImageProcessDontCareMargin = 0;
	m_iImageProcessDontCareMarginInner = -5000;
	m_bUseImageProcessDontCare2 = FALSE;
	m_iImageProcessDontCareImageIndex2 = 0;
	m_iImageProcessDontCareROINo2 = 1;
	m_iImageProcessDontCareMargin2 = 0;
	m_iImageProcessDontCareMargin2Inner = -5000;
	m_bUseImageProcessGenerate = FALSE;
	m_bUseImageProcessGenerateInspectFlag = FALSE;
	m_iImageProcessGenerateImageIndex = 0;
	m_iImageProcessGenerateROINo = 1;
	m_iImageProcessGenerateMargin = 0;
	m_bUseImageProcessMask = FALSE;
	m_iImageProcessMaskImageIndex = 0;
	m_iImageProcessMaskROINo = 1;
	m_bUseImageProcessMask2 = FALSE;
	m_iImageProcessMaskImageIndex2 = 0;
	m_iImageProcessMaskROINo2 = 1;
	m_bUseImageProcessLocalAlignAddLineFit = FALSE;
	m_iImageProcessLocalAlignImageIndex_2 = 0;
	m_iImageProcessLocalAlignROINo_2 = 1;
	m_iImageProcessLocalAlignFitPos_2 = LOCAL_ALIGN_FIT_LINE_ROI_BOTTOM;

	m_bUseBarcode = FALSE;
	m_dBarcodeShiftToleranceX = 0.2;
	m_dBarcodeShiftToleranceY = 0.2;
	m_bInspectBarcodeShift = TRUE;
	m_bInspectBarcodeRotation = FALSE;
	m_bUseBarcodeOtherImage = FALSE;
	m_iBarcodeOtherImageNumber = 1;

	//2017.04.11 hbk
	m_bUseOCR = FALSE;
	m_lCharPlateWidth = 270;
	m_lMinCharWidth = 20;
	m_lMaxCharWidth = 45;
	m_lMinCharHeight = 70;
	m_lMaxCharHeight = 150;
	m_lMinCharCandidateHeight = 20;
	m_lWholeAreaDivide = 45;
	m_lHorizontalDivide = 50;
	m_lFirstDiffernt = 50;
	m_lSecondDifferent = 50;
	m_ldiff = 5;
	m_iOCRSegmentDilation = 0;

	m_bUseShape = FALSE;
	m_iShapeIlluminationFilterX = 101;
	m_iShapeIlluminationFilterY = 101;
	m_dShapeIlluminationScaleFactor = 1.1;
	m_dShapeCurvatureSmFactor = 3;
	m_dShapeScalingMulti = 2434;
	m_dShapeScalingAdd = 120;
	m_dShapeImageReduceRatio = 2.0;
	m_dShapeLedAngle = 33;
	m_iShapeImageMargin = 0;

	// Don't Care
	m_iProcessChTypeDC = CHANNEL_TYPE_I;
	m_bUseDC = FALSE;

	m_bUseBrightFixedThresDC = FALSE;
	m_iBrightLowerThresDC = 200;
	m_iBrightUpperThresDC = 255;
	m_bUseBrightDTDC = FALSE;
	m_iBrightMedianFilterSizeDC = 1;
	m_iBrightClosingFilterSizeDC = 0;
	m_iBrightDTFilterTypeDC = 0;
	m_iBrightDTFilterSizeDC = 51;
	m_iBrightDTFilterSizeDC2 = 51;
	m_iBrightDTValueDC = 20;
	m_bUseBrightUniformityCheckDC = FALSE;
	m_iBrightUniformityPeakMinDC = 0;
	m_iBrightUniformityPeakMaxDC = 255;
	m_iBrightUniformityOffsetDC = 40;
	m_iBrightUniformityHystLengthDC = -1;
	m_iBrightUniformityHystOffsetDC = 10;

	m_bUseDarkFixedThresDC = FALSE;
	m_iDarkLowerThresDC = 0;
	m_iDarkUpperThresDC = 55;
	m_bUseDarkDTDC = FALSE;
	m_iDarkMedianFilterSizeDC = 1;
	m_iDarkClosingFilterSizeDC = 0;
	m_iDarkDTFilterTypeDC = 0;
	m_iDarkDTFilterSizeDC = 51;
	m_iDarkDTFilterSizeDC2 = 51;
	m_iDarkDTValueDC = 20;
	m_bUseDarkUniformityCheckDC = FALSE;
	m_iDarkUniformityPeakMinDC = 0;
	m_iDarkUniformityPeakMaxDC = 255;
	m_iDarkUniformityOffsetDC = 40;
	m_iDarkUniformityHystLengthDC = -1;
	m_iDarkUniformityHystOffsetDC = 10;

	m_iCandidateMinAreaDC = 1;
	m_bCombineBrightDarkBlobDC = FALSE;
	m_bMaxBlobDC = FALSE;
	m_bClosingOpeningDC = FALSE;
	m_iOpeningSizeDC = 0;
	m_iClosingSizeDC = 1;
	m_bFillupDC = TRUE;
	m_bUseConvexDC = FALSE;
	m_bUseConnectionDC = FALSE;
	m_iConnectionMinSizeDC = 5;
	m_iConnectionMinXLengthDC = 1;
	m_iConnectionLengthDC = 10;
	m_bUseAngleDC = FALSE;
	m_iConnectionMaxWidthDC = 6;

	m_bUseDefectConditionAreaDC = TRUE;
	m_bUseDefectConditionLengthDC = FALSE;
	m_bUseDefectConditionWidthDC = FALSE;
	m_bUseDefectConditionXLengthDC = FALSE;
	m_bUseDefectConditionYLengthDC = FALSE;
	m_bUseDefectConditionMeanDC = FALSE;
	m_bUseDefectConditionStdevDC = FALSE;
	m_bUseDefectConditionAnisometryDC = FALSE;
	m_bUseDefectConditionCircularityDC = FALSE;
	m_bUseDefectConditionRectangularityDC = FALSE;
	m_bUseDefectConditionConvexityDC = FALSE;
	m_bUseDefectConditionEllipseRatioDC = FALSE;
	m_bUseDefectConditionAreaRatioDC = FALSE;
	m_bUseDefectConditionContrastDC = FALSE;
	m_bUseDefectConditionBlobDC = FALSE;
	m_bUseDefectConditionShiftXDC = FALSE;
	m_bUseDefectConditionShiftYDC = FALSE;
	m_bUseDefectConditionOuterDistDC = FALSE;
	m_bUseDefectConditionInnerDistDC = FALSE;
	m_bUseDefectConditionUseZeroAreaDC = FALSE;
	m_bDefectConditionBoundaryConnectionDC = FALSE;
	m_bDefectConditionBoundaryMarginTopDC = FALSE;
	m_bDefectConditionBoundaryMarginBottomDC = FALSE;
	m_bDefectConditionBoundaryMarginLeftDC = FALSE;
	m_bDefectConditionBoundaryMarginRightDC = FALSE;

	m_iDefectConditionAreaMinDC = 100;
	m_iDefectConditionLengthMinDC = 10;
	m_iDefectConditionWidthMinDC = 2;
	m_iDefectConditionXLengthMinDC = 10;
	m_iDefectConditionYLengthMinDC = 10;
	m_iDefectConditionMeanMinDC = 60;
	m_dDefectConditionStdevMinDC = 1.0;
	m_dDefectConditionAnisometryMinDC = 1.0;
	m_dDefectConditionCircularityMinDC = 0.7;
	m_dDefectConditionRectangularityMinDC = 0.5;
	m_dDefectConditionConvexityMinDC = 0.8;
	m_dDefectConditionEllipseRatioMinDC = 1.0;
	m_iDefectConditionAreaRatioMinDC = 0;
	m_iDefectConditionContrastMinDC = 10;
	m_iDefectConditionBlobMinDC = 1;
	m_iDefectConditionShiftXMinDC = 0;
	m_iDefectConditionShiftYMinDC = 0;

	m_iDefectConditionAreaMaxDC = 1000000;
	m_iDefectConditionLengthMaxDC = 100000;
	m_iDefectConditionWidthMaxDC = 100000;
	m_iDefectConditionXLengthMaxDC = 100000;
	m_iDefectConditionYLengthMaxDC = 100000;
	m_iDefectConditionMeanMaxDC = 255;
	m_dDefectConditionStdevMaxDC = 50.0;
	m_dDefectConditionAnisometryMaxDC = 5.0;
	m_dDefectConditionCircularityMaxDC = 1.0;
	m_dDefectConditionRectangularityMaxDC = 1.0;
	m_dDefectConditionConvexityMaxDC = 1.0;
	m_dDefectConditionEllipseRatioMaxDC = 5.0;
	m_iDefectConditionAreaRatioMaxDC = 20;
	m_iDefectConditionContrastMaxDC = 255;
	m_iDefectConditionBlobMaxDC = 1000;
	m_iDefectConditionShiftXMaxDC = 1000;
	m_iDefectConditionShiftYMaxDC = 1000;

	m_iDefectConditionOuterDistDC = 0;
	m_iDefectConditionInnerDistDC = 0;

	m_iDefectConditionBoundaryMarginTopDC = 5;
	m_iDefectConditionBoundaryMarginBottomDC = 5;
	m_iDefectConditionBoundaryMarginLeftDC = 5;
	m_iDefectConditionBoundaryMarginRightDC = 5;
	m_iDefectConditionBoundaryMarginMinConnectionDC = 1;

	m_iLengthWidthConditionDC = CONDITION_AND;
	m_iXYLengthConditionDC = CONDITION_AND;

	m_iBoundaryConnectionConditionDC = CONDITION_OR;
	m_bDefectConditionBoundaryConnection2SideDC = FALSE;

	m_iDontcareRegionMargin = 2;

	m_bUseEdgeMeasure = FALSE;
	m_iEdgeMeasureOnePoint = 0;
	m_iEdgeMeasureMultiPointNumber = 8;
	m_bEdgeMeasureEndPoint = FALSE;
	m_bEdgeMeasureSlope = FALSE;
	m_bEdgeMeasureReverseGv = FALSE;
	m_bEdgeMeasureFixedEndPoint = FALSE;
	m_iEdgeMeasureLeftSideSize = 10;
	m_iEdgeMeasureRightSideSize = 10;
	m_iEdgeMeasureDir = 0;
	m_iEdgeMeasureGv = 0;
	m_iEdgeMeasurePos = 0;
	m_bEdgeMeasureLocalAlignUse = FALSE;
	m_dEdgeMeasureSmFactor = 1.0;
	m_iEdgeMeasureEdgeStr = 10;
	m_iEdgeMeasurePosOffset = 0;
	m_bEdgeMeasureUseMaxMinPoint = FALSE;
	m_iEdgeMeasureUseMaxPoint = 0;
	m_iEdgeMeasurePositionRange = 10;
	m_bEdgeMeasureUseOther = FALSE;
	m_bEdgeMeasureMakeROIOther = FALSE;
	m_iEdgeMeasureMakeROIRangeOther = 7;
	m_iEdgeMeasureImageIndexOther = 0;
	m_iEdgeMeasureGvOther = 0;
	m_iEdgeMeasurePosOther = 0;
	m_dEdgeMeasureSmFactorOther = 1.0;
	m_iEdgeMeasureEdgeStrOther = 10;
	m_iEdgeMeasurePosOffsetOther = 0;
	m_dEdgeMeasureAngleDeg = 45;
	m_iEdgeMeasureAccuracy = INTERPOLATION_NEAREST;

	m_bUseHomerTilt = FALSE;
	m_iHomerTiltHomerEdgeStr = 15;
	m_iHomerTiltBenvolioImage = 4;
	m_iHomerTiltBenvolioEdgeStr = 15;
	m_dHomerTiltHomerLength = 300;
	m_dHomerTiltTol = 10;
	m_dHomerTiltDistanceTol = 1000;

	m_bUseEpoxyVoidHole = FALSE;
	m_iEpoxyVoidHoleImage = 0;
	m_dEpoxyVoidHoleEdgeStr = 10.0;
	m_iEpoxyVoidHoleInspectMargin = 0;
	m_bEpoxyVoidHoleTiltInspect = TRUE;
	m_iEpoxyVoidHoleTiltTolerance = 20;
	m_bEpoxyVoidHoleGapInspect = FALSE;
	m_iEpoxyVoidHoleGapTolerance = 5;

	m_bUsePrintQuality = FALSE;
	m_iPrintQualityContrastMin = 14;
	m_iPrintQualitySymbolColsMin = 18;
	m_iPrintQualitySymbolRowsMin = 18;
	m_iPrintQualitySymbolColsMax = 18;
	m_iPrintQualitySymbolRowsMax = 18;
	m_iPrintQualityModuleSizeMin = 2;
	m_iPrintQualityModuleSizeMax = 18;
	m_dPrintQualitySlantMax = 0.0944843;
	m_bPrintQualityOverallQuality = TRUE;
	m_iPrintQualityGradeOverallQuality = 1;
	m_bPrintQualityUnusedErrorCorrection = TRUE;
	m_iPrintQualityGradeUnusedErrorCorrection = 1;
	m_iPrintQualityStrictModel = PQ_STRICT_MODEL_YES;
	m_iPrintQualityPolarity = PQ_POLARITY_DARK_ON_LIGHT;
	m_iPrintQualityMirrored = PQ_MIRRORED_NO;
	m_iPrintQualityContrastTolerance = PQ_CONTRAST_TOLERANCE_LOW;
	m_iPrintQualityStrictQuietZone = PQ_STRICT_QUIET_ZONE_NO;
	m_iPrintQualitySymbolShape = PQ_SYMBOL_SHAPE_SQUARE;
	m_iPrintQualitySmallModulesRobustness = PQ_SMALL_MODULES_ROBUSTNESS_HIGH;
	m_iPrintQualityModuleGrid = PQ_MODULE_GRID_FIXED;
	m_iPrintQualityModuleGapMin = PQ_MODULE_GAP_NO;
	m_iPrintQualityModuleGapMax = PQ_MODULE_GAP_NO;
	m_iPrintQualityFinderPatternTolerance = PQ_FINDER_PATTERN_TOLERANCE_LOW;

	//비등방
	m_bUseROIAnisoAlign = FALSE;
	m_iROIAnisoAlignXSearchMargin = 30;
	m_iROIAnisoAlignYSearchMargin = 30;
	m_iROIAnisoAlignMatchingAngleRange = 0;
	m_dROIAnisoAlignMatchingScaleMinY = 0.8;
	m_dROIAnisoAlignMatchingScaleMaxY = 1.2;
	m_dROIAnisoAlignMatchingScaleMinX = 0.8;
	m_dROIAnisoAlignMatchingScaleMaxX = 1.2;
	m_iROIAnisoAlignMatchingMinContrast = 15;
	m_bROIAnisoAlignUsePosX = FALSE;
	m_bROIAnisoAlignUsePosY = FALSE;
	m_dROIAnisoAlignMatchingScore = 0.3;
	m_bROIAnisoAlignInspectShift = FALSE;
	m_iROIAnisoAlignMatchingPLevel = 0;
	m_dROIAnisoAlignShiftTop = 10;
	m_dROIAnisoAlignShiftBottom = 10;
	m_dROIAnisoAlignShiftLeft = 10;
	m_dROIAnisoAlignShiftRight = 10;
	m_bROIAnisoAlignLocalAlignUse = FALSE;
	m_bUseAnisoColorImage = FALSE;
	m_bROIAnisoAlignMultiModuleInspect = FALSE;
	m_iROIAnisoAlignMultiModuleXNumber = 1;
	m_iROIAnisoAlignMultiModuleYNumber = 1;
	m_iROIAnisoAlignMultiModuleXPitch = 1000;
	m_iROIAnisoAlignMultiModuleYPitch = 1000;
	m_iROIAnisoAlignMultiModuleAlignTarget = MULTI_MODULE_ALIGN_TARGET_MODULE;
	m_iROIAnisoAlignMultiModuleVision = MULTI_MODULE_VISION_TOP1;

	m_bUseStiffenerEpoxy = FALSE;
	m_iThresholdParam_StiffenerRegion = 100;
	m_iDTFilterX_StiffenerRegion = 31;
	m_iDTFilterY_StiffenerRegion = 31;
	m_iDTValue_StiffenerRegion = 30;
	m_iClosing_StiffenerRegion = 5;
	m_dErosionParam_StiffenerBoundary = 20;
	m_iThresholdParam_SheildcanRegion = 250;
	m_dDilationParam_SheildcanBoundary = 33;
	m_dOpeningParam_CornerBoundary = 65;

	m_bUseDistanceMeasure = FALSE;
	m_iDistanceMeasureUpperGv = 2;
	m_iDistanceMeasureUpperPos = 2;
	m_dDistanceMeasureUpperSmFactor = 1.0;
	m_iDistanceMeasureUpperEdgeStr = 20;
	m_iDistanceMeasureLowerGv = 1;
	m_iDistanceMeasureLowerPos = 1;
	m_dDistanceMeasureLowerSmFactor = 1.0;
	m_iDistanceMeasureLowerEdgeStr = 20;
	m_iDistanceMeasureUpperPosOffset = 0;
	m_iDistanceMeasureLowerPosOffset = 0;
	m_iDistanceMeasureUsageType = 0;
	m_iDistanceMeasureSaveID = 1;
	m_dDistanceMeasureGapInspectionUpper = 1000;
	m_dDistanceMeasureGapInspectionLower = 0;
	m_iDistanceMeasureROIType = 1;

	m_bUseGapMeasure = FALSE;
	m_iGapMeasureUpperGv = 2;
	m_iGapMeasureUpperPos = 2;
	m_dGapMeasureUpperSmFactor = 1.0;
	m_iGapMeasureUpperEdgeStr = 20;
	m_iGapMeasureLowerGv = 1;
	m_iGapMeasureLowerPos = 1;
	m_dGapMeasureLowerSmFactor = 1.0;
	m_iGapMeasureLowerEdgeStr = 20;
	m_iGapMeasureUpperPosOffset = 0;
	m_iGapMeasureLowerPosOffset = 0;
	m_iGapMeasureSaveID = 1;
	m_iGapMeasureImageNo1 = 0;
	m_iGapMeasureImageNo2 = 0;
	m_dGapMeasureGapInspectionUpper = 1000;
	m_dGapMeasureGapInspectionLower = 0;
	m_dGapMeasureGapMeasureOffset = 0;
	m_iGapMeasureDir = 0;
	m_bGapMeasureAlarm = FALSE;
	m_iGapMeasurePointNumber = 1;
	m_iGapMeasureSelectType = GAP_MEASURE_SELECT_MAX;
	m_bGapMeasureUpperLine = FALSE;
	m_bGapMeasureLowerLine = FALSE;
	m_bGapMeasureDiffImage = FALSE;
	m_iGapMeasurePosStart = 10;
	m_iGapMeasurePosEnd = 20;
	m_iGapMeasureImageNo3 = 0;
	m_bGapMeasureEndRemove = FALSE;
	m_iGapMeasureRemoveEndUpper = 50;
	m_iGapMeasureRemoveEndLower = 50;
	m_bGapMeasureRetry = FALSE;
	m_iGapMeasureRetryImageNo = 0;
	m_iGapMeasureRetryGv = 0;
	m_iGapMeasureRetryPos = 0;
	m_dGapMeasureRetrySmFactor = 1.0;
	m_iGapMeasureRetryEdgeStr = 10;
	m_iGapMeasureRetryPosOffset = 0;

	m_bUseDefectConditionXUnder = FALSE;
	m_bUseDefectConditionYUnder = FALSE;

	// Surface Dual Inspection
	m_bUseSurfaceDualInspection = FALSE;
	m_iSurfaceDualImageIndex_1 = 0;
	m_iSurfaceDualImageIndex_2 = 1;

	m_iSurfaceDualDefectType_1 = 0;
	m_iSurfaceDualDefectType_2 = 0;
	m_bUseBrightFixedThres_1 = FALSE;
	m_iBrightLowerThres_1 = 200;
	m_iBrightUpperThres_1 = 255;
	m_bUseBrightDT_1 = FALSE;
	m_iBrightMedianFilterSize_1 = 1;
	m_iBrightClosingFilterSize_1 = 0;
	m_iBrightDTFilterType_1 = 0;
	m_iBrightDTFilterSize_1 = 51;
	m_iBrightDTFilterSize2_1 = 51;
	m_iBrightDTValue_1 = 20;
	m_bUseDTPolarTrans_1 = FALSE;
	m_bUseBrightHystThres_1 = FALSE;
	m_iBrightHystSecureThres_1 = 190;
	m_iBrightHystPotentialThres_1 = 175;
	m_iBrightHystPotentialLength_1 = 5;

	m_bUseBrightFixedThres_2 = FALSE;
	m_iBrightLowerThres_2 = 200;
	m_iBrightUpperThres_2 = 255;
	m_bUseBrightDT_2 = FALSE;
	m_iBrightMedianFilterSize_2 = 1;
	m_iBrightClosingFilterSize_2 = 0;
	m_iBrightDTFilterType_2 = 0;
	m_iBrightDTFilterSize_2 = 51;
	m_iBrightDTFilterSize2_2 = 51;
	m_iBrightDTValue_2 = 20;
	m_bUseDTPolarTrans_2 = FALSE;
	m_bUseBrightHystThres_2 = FALSE;
	m_iBrightHystSecureThres_2 = 190;
	m_iBrightHystPotentialThres_2 = 175;
	m_iBrightHystPotentialLength_2 = 5;

	m_iDualBlobProcessType = 0;
	m_bDualBlobDefectConditionEach = FALSE;

	m_bUseNeckEpoxyMeasure = FALSE;
	m_iNeckEpoxyMeasureTopEdgeStr = 5;
	m_iNeckEpoxyMeasureBottomEdgeStr = 5;
	m_iNeckEpoxyMeasureRightEdgeStr = 10;
	m_iNeckEpoxyToleranceHeightMax = 10000;
	m_iNeckEpoxyToleranceHeightMin = 0;
	m_iNeckEpoxyToleranceLength = 1000;
	m_iNeckEpoxyToleranceTopPosOffset = 100;
	m_iNeckEpoxyToleranceBottomPosOffset = 100;
	m_iNeckEpoxyInspectMethod = NECK_EPOXY_INSPECT_EDGE;

	m_bUseNeckEpoxyCrack = FALSE;
	m_iNeckEpoxyCrackOpeningSize = 2;
	m_iNeckEpoxyCrackYLength = 150;
	m_iNeckEpoxyCrackBoundaryWidth = 4;
	m_iNeckEpoxyCrackDir = NECK_EPOXY_CRACK_DIR_LEFT;

	m_iDnnInspectionType = 0;
	m_iDnnInspectionModel = 0;

	m_bUseCornerMeasure = FALSE;
	m_iCornerMeasureSaveID = 1;
	m_iCornerMeasureMultiPointNumber = 8;
	m_iCornerMeasurePositionRange = 10;
	m_iCornerMeasureGv[0] = m_iCornerMeasureGv[1] = 0;
	m_iCornerMeasurePos[0] = m_iCornerMeasurePos[1] = 0;
	m_dCornerMeasureSmFactor[0] = m_dCornerMeasureSmFactor[1] = 1.0;
	m_iCornerMeasureEdgeStr[0] = m_iCornerMeasureEdgeStr[1] = 10;
	m_iCornerMeasurePosOffset[0] = m_iCornerMeasurePosOffset[1] = 0;
	m_dCornerMeasureTargetSmFactor = 1.0;
	m_iCornerMeasureTargetEdgeStr = 10;
	m_dCornerMeasureTargetDistanceSpec = 0.23;

	m_bNeckEpoxyCrackUseXDirMode = FALSE; //VER 1.1.3.0 add

	// Component
	m_bUseComponent = FALSE;
	m_dComponentScore = 70;
	m_dComponentShiftX = 40;
	m_dComponentShiftY = 40;
	m_dComponentRotationAngle = 10;
	m_bComponentApply_1 = TRUE;
	m_bComponentApply_2 = TRUE;
	m_bComponentApply_3 = TRUE;
	m_bComponentApply_4 = TRUE;
	m_bComponentApply_5 = FALSE;
	m_iComponentDefectType1 = 18;
	m_iComponentDefectType2 = 22;
	m_iComponentDefectType3 = 16;
	m_iComponentDefectType4 = 20;
	m_iComponentDefectType5 = 24;
	m_dComponentRotationAngle2 = 10;

	m_bUseHSV = FALSE;
	miLowerHue = 0;
	miUpperHue = 360;
	miLowerSaturation = 0;
	miUpperSaturation = 100;

	m_bUseHeightMeasure = FALSE;
	m_iHeightMeasureType = 0;

	m_bUseBoundaryInspection = FALSE;
	m_iBoundaryEdgeMeasureDir = 1;
	m_iBoundaryEdgeMeasureGv = 0;
	m_iBoundaryEdgeMeasurePos = 2;
	m_dBoundaryEdgeMeasureSmFactor = 3.0;
	m_dBoundaryEdgeMeasureEdgeStr = 15.0;
	m_iBoundaryEdgeMeasureSampling = 5;
	m_iBoundaryEdgeMeasureSensorLength = 40;
	m_iBoundaryDisplay = 0;
	m_bBoundaryCondition[0] = TRUE;
	m_iBoundaryMedianFilterSize[0] = 201;
	m_dBoundaryDefectDepth[0] = 5.0;
	m_dBoundaryDefectMBDepth[0] = 5.0;
	m_dBoundaryDefectLength[0] = 30.0;
	m_bBoundaryUseConnection[0] = TRUE;
	m_iBoundaryConnectionLength[0] = 11;
	m_bBoundaryCondition[1] = TRUE;
	m_iBoundaryMedianFilterSize[1] = 51;
	m_dBoundaryDefectDepth[1] = 10.0;
	m_dBoundaryDefectMBDepth[1] = 10.0;
	m_dBoundaryDefectLength[1] = 5.0;
	m_bBoundaryUseConnection[1] = TRUE;
	m_iBoundaryConnectionLength[1] = 11;

	m_bBoundaryCondition[2] = TRUE;
	m_dBoundaryDefectDepth[2] = 15.0;
	m_dBoundaryDefectLength[2] = 3.0;

	m_bBoundaryUseRoiConnected = FALSE;
}

CAlgorithmParam& CAlgorithmParam::operator=(CAlgorithmParam& data)
{
	if (this == &data) return *this;

	m_bInspect = data.m_bInspect;

	m_bUseFilter1 = data.m_bUseFilter1;
	m_bUseFilter2 = data.m_bUseFilter2;
	m_bUseFilter3 = data.m_bUseFilter3;
	m_bUseFilter4 = data.m_bUseFilter4;

	m_bCheckAIUseConsiderImageNo = data.m_bCheckAIUseConsiderImageNo;
	m_bCheckAIUseConsiderDefectCode = data.m_bCheckAIUseConsiderDefectCode;

	m_iDefectType = data.m_iDefectType;

	for (int i = 0; i < 3; i++)
	{
		m_bUseInspectArea[i] = data.m_bUseInspectArea[i];
		m_iInspectAreaContour1Margin[i] = data.m_iInspectAreaContour1Margin[i];
		m_iInspectAreaContour2Margin[i] = data.m_iInspectAreaContour2Margin[i];
	}

	m_bUseSurfaceInspection = data.m_bUseSurfaceInspection;
	m_bUseSurfaceMeasureSaveLog = data.m_bUseSurfaceMeasureSaveLog;

	m_bUseSurfaceInspectEachROI = data.m_bUseSurfaceInspectEachROI;
	m_iSurfaceInspectEachROINgNumber = data.m_iSurfaceInspectEachROINgNumber;

	m_bApplyCircleFitting = data.m_bApplyCircleFitting;
	m_iCircleFittingEdgeStr = data.m_iCircleFittingEdgeStr;
	m_iCircleFittingEdgeMargin = data.m_iCircleFittingEdgeMargin;

	m_bUseDTPolarTrans = data.m_bUseDTPolarTrans;

	m_bUseFTConnected = data.m_bUseFTConnected;
	m_bUseFTConnectedArea = data.m_bUseFTConnectedArea;
	m_bUseFTConnectedLength = data.m_bUseFTConnectedLength;
	m_bUseFTConnectedWidth = data.m_bUseFTConnectedWidth;
	m_iFTConnectedAreaMin = data.m_iFTConnectedAreaMin;
	m_iFTConnectedLengthMin = data.m_iFTConnectedLengthMin;
	m_iFTConnectedWidthMin = data.m_iFTConnectedWidthMin;
	m_bUseBarcodeBlob = data.m_bUseBarcodeBlob;
	m_bUseBlobCornerMeasure = data.m_bUseBlobCornerMeasure;
	m_bUseSaveBNEOverflowTop = data.m_bUseSaveBNEOverflowTop;
	m_bUseSaveBNEOverflowBottom = data.m_bUseSaveBNEOverflowBottom;

	m_bUseBrightFixedThres = data.m_bUseBrightFixedThres;
	m_iBrightLowerThres = data.m_iBrightLowerThres;
	m_iBrightUpperThres = data.m_iBrightUpperThres;
	m_bUseBrightDT = data.m_bUseBrightDT;
	m_iBrightMedianFilterSize = data.m_iBrightMedianFilterSize;
	m_iBrightClosingFilterSize = data.m_iBrightClosingFilterSize;
	m_iBrightDTFilterType = data.m_iBrightDTFilterType;
	m_iBrightDTFilterSize = data.m_iBrightDTFilterSize;
	m_iBrightDTFilterSize2 = data.m_iBrightDTFilterSize2;
	m_iBrightDTValue = data.m_iBrightDTValue;
	m_bUseBrightUniformityCheck = data.m_bUseBrightUniformityCheck;
	m_iBrightUniformityPeakMin = data.m_iBrightUniformityPeakMin;
	m_iBrightUniformityPeakMax = data.m_iBrightUniformityPeakMax;
	m_iBrightUniformityOffset = data.m_iBrightUniformityOffset;
	m_iBrightUniformityHystLength = data.m_iBrightUniformityHystLength;
	m_iBrightUniformityHystOffset = data.m_iBrightUniformityHystOffset;
	m_bUseBrightHystThres = data.m_bUseBrightHystThres;
	m_iBrightHystSecureThres = data.m_iBrightHystSecureThres;
	m_iBrightHystPotentialThres = data.m_iBrightHystPotentialThres;
	m_iBrightHystPotentialLength = data.m_iBrightHystPotentialLength;

	m_bUseDarkFixedThres = data.m_bUseDarkFixedThres;
	m_iDarkLowerThres = data.m_iDarkLowerThres;
	m_iDarkUpperThres = data.m_iDarkUpperThres;
	m_bUseDarkDT = data.m_bUseDarkDT;
	m_iDarkMedianFilterSize = data.m_iDarkMedianFilterSize;
	m_iDarkClosingFilterSize = data.m_iDarkClosingFilterSize;
	m_iDarkDTFilterType = data.m_iDarkDTFilterType;
	m_iDarkDTFilterSize = data.m_iDarkDTFilterSize;
	m_iDarkDTFilterSize2 = data.m_iDarkDTFilterSize2;
	m_iDarkDTValue = data.m_iDarkDTValue;
	m_bUseDarkUniformityCheck = data.m_bUseDarkUniformityCheck;
	m_iDarkUniformityPeakMin = data.m_iDarkUniformityPeakMin;
	m_iDarkUniformityPeakMax = data.m_iDarkUniformityPeakMax;
	m_iDarkUniformityOffset = data.m_iDarkUniformityOffset;
	m_iDarkUniformityHystLength = data.m_iDarkUniformityHystLength;
	m_iDarkUniformityHystOffset = data.m_iDarkUniformityHystOffset;
	m_bUseDarkHystThres = data.m_bUseDarkHystThres;
	m_iDarkHystSecureThres = data.m_iDarkHystSecureThres;
	m_iDarkHystPotentialThres = data.m_iDarkHystPotentialThres;
	m_iDarkHystPotentialLength = data.m_iDarkHystPotentialLength;
	m_bUseBrightBinThres = data.m_bUseBrightBinThres;
	m_bUseDarkBinThres = data.m_bUseDarkBinThres;

	m_iCandidateMinArea = data.m_iCandidateMinArea;
	m_bCombineBrightDarkBlob = data.m_bCombineBrightDarkBlob;
	m_bMaxBlob = data.m_bMaxBlob;
	m_bClosingOpening = data.m_bClosingOpening;
	m_bApplyOneDirMorp = data.m_bApplyOneDirMorp;
	m_iOneDirMorpXOpening = data.m_iOneDirMorpXOpening;
	m_iOneDirMorpXClosing = data.m_iOneDirMorpXClosing;
	m_iOpeningSize = data.m_iOpeningSize;
	m_iClosingSize = data.m_iClosingSize;
	m_iThresholdOpeningSize = data.m_iThresholdOpeningSize;
	m_iThresholdClosingSize = data.m_iThresholdClosingSize;
	m_bFillup = data.m_bFillup;
	m_bUseConvex = data.m_bUseConvex;
	m_bUseSubConvex = data.m_bUseSubConvex;
	m_bUseRect = data.m_bUseRect;
	m_bUseConnection = data.m_bUseConnection;
	m_iConnectionMinSize = data.m_iConnectionMinSize;
	m_iConnectionMinXLength = data.m_iConnectionMinXLength;
	m_iConnectionLength = data.m_iConnectionLength;
	m_bUseAngle = data.m_bUseAngle;
	m_iConnectionMaxWidth = data.m_iConnectionMaxWidth;
	m_iHoleFillCondition = data.m_iHoleFillCondition;
	m_iFillHoleArea = data.m_iFillHoleArea;
	m_bXYLengthMeasureIgnoreHole = data.m_bXYLengthMeasureIgnoreHole;
	m_bXYLengthMaxLineLength = data.m_bXYLengthMaxLineLength;
	m_bDefectConditionBoundaryConnectionAny = data.m_bDefectConditionBoundaryConnectionAny;
	m_iDefectConditionBoundaryConnectionOuter = data.m_iDefectConditionBoundaryConnectionOuter;
	m_iDefectConditionBoundaryMarginAny = data.m_iDefectConditionBoundaryMarginAny;
	m_iDefectConditionBoundaryMarginMinConnectionAny = data.m_iDefectConditionBoundaryMarginMinConnectionAny;

	m_bUseDefectConditionArea = data.m_bUseDefectConditionArea;
	m_bUseDefectConditionLength = data.m_bUseDefectConditionLength;
	m_bUseDefectConditionWidth = data.m_bUseDefectConditionWidth;
	m_bUseDefectConditionXLength = data.m_bUseDefectConditionXLength;
	m_bUseDefectConditionYLength = data.m_bUseDefectConditionYLength;
	m_bUseDefectConditionMean = data.m_bUseDefectConditionMean;
	m_bUseDefectConditionStdev = data.m_bUseDefectConditionStdev;
	m_bUseDefectConditionAnisometry = data.m_bUseDefectConditionAnisometry;
	m_bUseDefectConditionCircularity = data.m_bUseDefectConditionCircularity;
	m_bUseDefectConditionRectangularity = data.m_bUseDefectConditionRectangularity;
	m_bUseDefectConditionConvexity = data.m_bUseDefectConditionConvexity;
	m_bUseDefectConditionEllipseRatio = data.m_bUseDefectConditionEllipseRatio;
	m_bUseDefectConditionAreaRatio = data.m_bUseDefectConditionAreaRatio;
	m_bUseDefectConditionContrast = data.m_bUseDefectConditionContrast;
	m_bUseDefectConditionGrad = data.m_bUseDefectConditionGrad;
	m_bUseDefectConditionBlob = data.m_bUseDefectConditionBlob;
	m_bUseDefectConditionShiftX = data.m_bUseDefectConditionShiftX;
	m_bUseDefectConditionShiftY = data.m_bUseDefectConditionShiftY;
	m_bUseDefectConditionOuterDist = data.m_bUseDefectConditionOuterDist;
	m_bUseDefectConditionInnerDist = data.m_bUseDefectConditionInnerDist;
	m_bUseDefectConditionUseZeroArea = data.m_bUseDefectConditionUseZeroArea;
	m_bDefectConditionBoundaryConnection = data.m_bDefectConditionBoundaryConnection;
	m_bDefectConditionBoundaryMarginTop = data.m_bDefectConditionBoundaryMarginTop;
	m_bDefectConditionBoundaryMarginBottom = data.m_bDefectConditionBoundaryMarginBottom;
	m_bDefectConditionBoundaryMarginLeft = data.m_bDefectConditionBoundaryMarginLeft;
	m_bDefectConditionBoundaryMarginRight = data.m_bDefectConditionBoundaryMarginRight;
	m_bDefectConditionBoundaryMarginHCenter = data.m_bDefectConditionBoundaryMarginHCenter;
	m_bDefectConditionBoundaryMarginVCenter = data.m_bDefectConditionBoundaryMarginVCenter;
	m_bUseDefectConditionXUnder = data.m_bUseDefectConditionXUnder;
	m_bUseDefectConditionYUnder = data.m_bUseDefectConditionYUnder;

	m_iDefectConditionAreaMin = data.m_iDefectConditionAreaMin;
	m_iDefectConditionLengthMin = data.m_iDefectConditionLengthMin;
	m_iDefectConditionWidthMin = data.m_iDefectConditionWidthMin;
	m_iDefectConditionXLengthMin = data.m_iDefectConditionXLengthMin;
	m_iDefectConditionYLengthMin = data.m_iDefectConditionYLengthMin;
	m_iDefectConditionMeanMin = data.m_iDefectConditionMeanMin;
	m_dDefectConditionStdevMin = data.m_dDefectConditionStdevMin;
	m_dDefectConditionAnisometryMin = data.m_dDefectConditionAnisometryMin;
	m_dDefectConditionCircularityMin = data.m_dDefectConditionCircularityMin;
	m_dDefectConditionRectangularityMin = data.m_dDefectConditionRectangularityMin;
	m_dDefectConditionConvexityMin = data.m_dDefectConditionConvexityMin;
	m_dDefectConditionEllipseRatioMin = data.m_dDefectConditionEllipseRatioMin;
	m_iDefectConditionAreaRatioMin = data.m_iDefectConditionAreaRatioMin;
	m_iDefectConditionContrastMin = data.m_iDefectConditionContrastMin;
	m_iDefectConditionGradMin = data.m_iDefectConditionGradMin;
	m_iDefectConditionBlobMin = data.m_iDefectConditionBlobMin;
	m_iDefectConditionShiftXMin = data.m_iDefectConditionShiftXMin;
	m_iDefectConditionShiftYMin = data.m_iDefectConditionShiftYMin;

	m_iDefectConditionAreaMax = data.m_iDefectConditionAreaMax;
	m_iDefectConditionLengthMax = data.m_iDefectConditionLengthMax;
	m_iDefectConditionWidthMax = data.m_iDefectConditionWidthMax;
	m_iDefectConditionXLengthMax = data.m_iDefectConditionXLengthMax;
	m_iDefectConditionYLengthMax = data.m_iDefectConditionYLengthMax;
	m_iDefectConditionMeanMax = data.m_iDefectConditionMeanMax;
	m_dDefectConditionStdevMax = data.m_dDefectConditionStdevMax;
	m_dDefectConditionAnisometryMax = data.m_dDefectConditionAnisometryMax;
	m_dDefectConditionCircularityMax = data.m_dDefectConditionCircularityMax;
	m_dDefectConditionRectangularityMax = data.m_dDefectConditionRectangularityMax;
	m_dDefectConditionConvexityMax = data.m_dDefectConditionConvexityMax;
	m_dDefectConditionEllipseRatioMax = data.m_dDefectConditionEllipseRatioMax;
	m_iDefectConditionAreaRatioMax = data.m_iDefectConditionAreaRatioMax;
	m_iDefectConditionContrastMax = data.m_iDefectConditionContrastMax;
	m_iDefectConditionGradMax = data.m_iDefectConditionGradMax;
	m_iDefectConditionBlobMax = data.m_iDefectConditionBlobMax;
	m_iDefectConditionShiftXMax = data.m_iDefectConditionShiftXMax;
	m_iDefectConditionShiftYMax = data.m_iDefectConditionShiftYMax;

	m_iDefectConditionOuterDist = data.m_iDefectConditionOuterDist;
	m_iDefectConditionInnerDist = data.m_iDefectConditionInnerDist;

	m_iDefectConditionBoundaryMarginTop = data.m_iDefectConditionBoundaryMarginTop;
	m_iDefectConditionBoundaryMarginBottom = data.m_iDefectConditionBoundaryMarginBottom;
	m_iDefectConditionBoundaryMarginLeft = data.m_iDefectConditionBoundaryMarginLeft;
	m_iDefectConditionBoundaryMarginRight = data.m_iDefectConditionBoundaryMarginRight;
	m_iDefectConditionBoundaryMarginHCenter = data.m_iDefectConditionBoundaryMarginHCenter;
	m_iDefectConditionBoundaryMarginVCenter = data.m_iDefectConditionBoundaryMarginVCenter;
	m_iDefectConditionBoundaryMarginMinConnection = data.m_iDefectConditionBoundaryMarginMinConnection;

	m_bUseImageScaling = data.m_bUseImageScaling;
	m_iImageScaleMethod = data.m_iImageScaleMethod;
	m_iImageScaleMin = data.m_iImageScaleMin;
	m_iImageScaleMax = data.m_iImageScaleMax;

	m_bUseEdgeImage = data.m_bUseEdgeImage;
	m_iEdgeFilterType = data.m_iEdgeFilterType;
	m_dEdgeImageScale = data.m_dEdgeImageScale;

	m_bUsePeak = data.m_bUsePeak;
	m_iNormalPeakThresMin = data.m_iNormalPeakThresMin;
	m_iNormalPeakThresMax = data.m_iNormalPeakThresMax;
	m_iAbnormalPeakHistogramOffset = data.m_iAbnormalPeakHistogramOffset;

	m_iLengthWidthCondition = data.m_iLengthWidthCondition;
	m_iXYLengthCondition = data.m_iXYLengthCondition;

	m_iBoundaryConnectionCondition = data.m_iBoundaryConnectionCondition;
	m_bDefectConditionBoundaryConnection2Side = data.m_bDefectConditionBoundaryConnection2Side;
	m_bDefectConditionInnerOuterBoundaryConnection = data.m_bDefectConditionInnerOuterBoundaryConnection;

	m_iXLengthCondition = data.m_iXLengthCondition;
	m_iXLengthConditionMinRef = data.m_iXLengthConditionMinRef;
	m_iYLengthCondition = data.m_iYLengthCondition;
	m_iYLengthConditionMinRef = data.m_iYLengthConditionMinRef;

	m_bUseDefectConditionHole = data.m_bUseDefectConditionHole;
	m_iDefectConditionHoleArea = data.m_iDefectConditionHoleArea;

	m_bDefectConditionUnionBlob = data.m_bDefectConditionUnionBlob;
	m_bDefectConditionDilation = data.m_bDefectConditionDilation;
	m_iDefectConditionDilationSize = data.m_iDefectConditionDilationSize;

	//VER 1.1.3.2 add Verification of height near the protrusion 
	m_bDefectConditionOuterDiff = data.m_bDefectConditionOuterDiff;
	m_nDefectConditionOuterDiffInterval = data.m_nDefectConditionOuterDiffInterval;
	m_nDefectConditionOuterDiffMinDiff = data.m_nDefectConditionOuterDiffMinDiff;
	m_nOuterDiffUpper = data.m_nOuterDiffUpper;
	m_nDefectConditionOuterDiffMaxRepeat = data.m_nDefectConditionOuterDiffMaxRepeat;
	//VER 1.1.3.2 end

	//VER 1.1.3.3 add Calculate Blob Length From ConerLength
	m_nDefectConditionConerLengthDir = data.m_nDefectConditionConerLengthDir;
	m_bDefectConditionConerLengthUse = data.m_bDefectConditionConerLengthUse;
	m_nDefectConditionConerLengthMin = data.m_nDefectConditionConerLengthMin;
	m_nDefectConditionConerLengthMax = data.m_nDefectConditionConerLengthMax;
	//VER 1.1.3.3 end

	//VER 1.1.3.4 add
	m_bDefectConditionConerLengthMaxUse = data.m_bDefectConditionConerLengthMaxUse;
	//VER 1.1.3.4 end

	m_bUseDefectConditionXLengthSum = data.m_bUseDefectConditionXLengthSum;
	m_iDefectConditionXLengthSumMin = data.m_iDefectConditionXLengthSumMin;
	m_iDefectConditionXLengthSumMax = data.m_iDefectConditionXLengthSumMax;
	m_bUseDefectConditionYLengthSum = data.m_bUseDefectConditionYLengthSum;
	m_iDefectConditionYLengthSumMin = data.m_iDefectConditionYLengthSumMin;
	m_iDefectConditionYLengthSumMax = data.m_iDefectConditionYLengthSumMax;

	m_bDefectConditionOrthoLength = data.m_bDefectConditionOrthoLength;
	m_iOrthoLengthRefSide = data.m_iOrthoLengthRefSide;

	// Line Fitting

	m_bUseLineFitting = data.m_bUseLineFitting;
	m_iLineFitObjectLowerThres = data.m_iLineFitObjectLowerThres;
	m_iLineFitObjectUpperThres = data.m_iLineFitObjectUpperThres;
	m_bLineFitUseOtherImageFitting = data.m_bLineFitUseOtherImageFitting;
	m_iLineFitOtherImageNumber = data.m_iLineFitOtherImageNumber;
	m_dLineFitFittingEdgeSF = data.m_dLineFitFittingEdgeSF;
	m_dLineFitObjectEdgeSF = data.m_dLineFitObjectEdgeSF;
	m_iLineFitMinContourLength = data.m_iLineFitMinContourLength;
	m_iLineFitContourConnectionLength = data.m_iLineFitContourConnectionLength;
	m_bLineFitUseBurInspection = data.m_bLineFitUseBurInspection;
	m_dLineFitBurDefectLength = data.m_dLineFitBurDefectLength;
	m_dLineFitBurDefectLengthMax = data.m_dLineFitBurDefectLengthMax;
	m_bLineFitUseChippingInspection = data.m_bLineFitUseChippingInspection;
	m_dLineFitChippingDefectLength = data.m_dLineFitChippingDefectLength;
	m_dLineFitChippingDefectLengthMax = data.m_dLineFitChippingDefectLengthMax;
	m_bLineFitUseRotationInspection = data.m_bLineFitUseRotationInspection;
	m_iLineFitRotationReference = data.m_iLineFitRotationReference;
	m_dLineFitRotationDefectAngle = data.m_dLineFitRotationDefectAngle;
	m_iLineFitEdgeMethod = data.m_iLineFitEdgeMethod;
	m_iLineFitFittingEdgeHigh = data.m_iLineFitFittingEdgeHigh;
	m_iLineFitFittingMinContourLength = data.m_iLineFitFittingMinContourLength;
	m_iLineFitFittingContourConnectionLength = data.m_iLineFitFittingContourConnectionLength;
	m_iLineFitEdgeHigh = data.m_iLineFitEdgeHigh;
	m_bLineFitLengthSave = data.m_bLineFitLengthSave;
	m_bNoUseLineFit = data.m_bNoUseLineFit;
	m_bLineFitLocalAlignUse = data.m_bLineFitLocalAlignUse;

	m_bUsePinLength = data.m_bUsePinLength;
	m_iPinPos = data.m_iPinPos;
	m_iPinDTFilterSize = data.m_iPinDTFilterSize;
	m_iPinOpeningClosingSize = data.m_iPinOpeningClosingSize;
	m_iPinMinArea = data.m_iPinMinArea;
	m_iPinDistanceTolerance = data.m_iPinDistanceTolerance;
	m_iPinDTValue = data.m_iPinDTValue;
	m_iPinDefectNumberTolerance = data.m_iPinDefectNumberTolerance;
	m_iPinPitch = data.m_iPinPitch;

	m_bUseDent = data.m_bUseDent;
	m_iDentMedFilterSize = data.m_iDentMedFilterSize;
	m_iDentDtFilterSize = data.m_iDentDtFilterSize;
	m_iDentBrightDtValue = data.m_iDentBrightDtValue;
	m_iDentDarkDtValue = data.m_iDentDarkDtValue;
	m_iDentCandidateMinArea = data.m_iDentCandidateMinArea;
	m_iDentCandidateMinWidth = data.m_iDentCandidateMinWidth;
	m_iDentCandidateMinDistance = data.m_iDentCandidateMinDistance;
	m_iDentMinArea = data.m_iDentMinArea;

	m_bUseImageCompare = data.m_bUseImageCompare;
	m_bImageCompareSaveFeatureLog = data.m_bImageCompareSaveFeatureLog;
	m_bUseImageCompareWindow = data.m_bUseImageCompareWindow;
	m_iImageCompareWindowSize = data.m_iImageCompareWindowSize;
	m_bUseImageCompareEdgeImage = data.m_bUseImageCompareEdgeImage;
	m_bUseImageCompareNormalizer = data.m_bUseImageCompareNormalizer;
	m_iImageCompareNormalizeWeight = data.m_iImageCompareNormalizeWeight;
	m_bUseImageCompareBright = data.m_bUseImageCompareBright;
	m_bUseImageCompareDark = data.m_bUseImageCompareDark;
	m_iImageCompareBrightAbs = data.m_iImageCompareBrightAbs;
	m_iImageCompareDarkAbs = data.m_iImageCompareDarkAbs;
	m_dImageCompareBrightVar = data.m_dImageCompareBrightVar;
	m_dImageCompareDarkVar = data.m_dImageCompareDarkVar;
	m_bUseImageCompareAutoTrain = data.m_bUseImageCompareAutoTrain;
	m_bUseImageCompareMod = data.m_bUseImageCompareMod;
	m_bUseImageCompareModBright = data.m_bUseImageCompareModBright;
	m_bUseImageCompareModDark = data.m_bUseImageCompareModDark;
	m_iImageCompareModBrightAbs = data.m_iImageCompareModBrightAbs;
	m_iImageCompareModDarkAbs = data.m_iImageCompareModDarkAbs;
	m_dImageCompareModBrightVar = data.m_dImageCompareModBrightVar;
	m_dImageCompareModDarkVar = data.m_dImageCompareModDarkVar;
	m_iImageCompareModTop = data.m_iImageCompareModTop;
	m_iImageCompareModBottom = data.m_iImageCompareModBottom;
	m_iImageCompareModLeft = data.m_iImageCompareModLeft;
	m_iImageCompareModRight = data.m_iImageCompareModRight;

	m_bUsePartCheck = data.m_bUsePartCheck;
	m_bPartCheckMultipleModel = data.m_bPartCheckMultipleModel;
	m_iPartCheckMatchingAngleRange = data.m_iPartCheckMatchingAngleRange;
	m_iPartCheckMatchingPLevel = data.m_iPartCheckMatchingPLevel;
	m_iPartCheckMatchingScore = data.m_iPartCheckMatchingScore;
	m_iPartCheckMatchingSearchMarginX = data.m_iPartCheckMatchingSearchMarginX;
	m_iPartCheckMatchingSearchMarginY = data.m_iPartCheckMatchingSearchMarginY;
	m_iPartCheckMatchingSearchMarginX2 = data.m_iPartCheckMatchingSearchMarginX2;
	m_iPartCheckMatchingSearchMarginY2 = data.m_iPartCheckMatchingSearchMarginY2;
	m_bPartCheckInspectExistence = data.m_bPartCheckInspectExistence;
	m_bPartCheckBlob = data.m_bPartCheckBlob;
	m_bPartCheckInspectShift = data.m_bPartCheckInspectShift;
	m_iPartCheckShiftX = data.m_iPartCheckShiftX;
	m_iPartCheckShiftY = data.m_iPartCheckShiftY;
	m_dPartCheckRotationAngle = data.m_dPartCheckRotationAngle;
	m_bPartCheckLocalAlignUse = data.m_bPartCheckLocalAlignUse;
	m_bPartCheckApplyComp = data.m_bPartCheckApplyComp;
	m_bPartCheckSaveLog = data.m_bPartCheckSaveLog;
	m_bPartCheckMeasureEdge = data.m_bPartCheckMeasureEdge;
	m_iPartCheckLegXSize = data.m_iPartCheckLegXSize;
	m_iPartCheckLegYSize = data.m_iPartCheckLegYSize;
	m_dPartCheckEdgeMeasueSmFactor = data.m_dPartCheckEdgeMeasueSmFactor;
	m_dPartCheckEdgeMeasueEdgeStr = data.m_dPartCheckEdgeMeasueEdgeStr;
	m_bPartCheckInspectLegDamage = data.m_bPartCheckInspectLegDamage;
	m_bPartCheckMultiModuleInspect = data.m_bPartCheckMultiModuleInspect;
	m_iPartCheckMultiModuleXNumber = data.m_iPartCheckMultiModuleXNumber;
	m_iPartCheckMultiModuleYNumber = data.m_iPartCheckMultiModuleYNumber;
	m_iPartCheckMultiModuleXPitch = data.m_iPartCheckMultiModuleXPitch;
	m_iPartCheckMultiModuleYPitch = data.m_iPartCheckMultiModuleYPitch;
	m_bPartCheckPickerPosUse = data.m_bPartCheckPickerPosUse;
	m_bPartCheckMultiModuleMoveOffset = data.m_bPartCheckMultiModuleMoveOffset;
	m_iPartCheckMultiModuleTarget = data.m_iPartCheckMultiModuleTarget;

	for (int i = 0; i < ALIGN_MODULE_Y; i++)
	{
		for (int j = 0; j < ALIGN_MODULE_X; j++)
		{
			m_iPartCheckMoveOffsetX[i][j] = data.m_iPartCheckMoveOffsetX[i][j];
			m_iPartCheckMoveOffsetY[i][j] = data.m_iPartCheckMoveOffsetY[i][j];
		}
	}

	m_bUseConcenter = data.m_bUseConcenter;
	m_iConcenterInnerCircleDiameter = data.m_iConcenterInnerCircleDiameter;
	m_iConcenterMatchingScore = data.m_iConcenterMatchingScore;
	m_dConcenterMatchingScaleMin = data.m_dConcenterMatchingScaleMin;
	m_dConcenterMatchingScaleMax = data.m_dConcenterMatchingScaleMax;
	m_iConcenterMatchingMinContrast = data.m_iConcenterMatchingMinContrast;
	m_iConcenterCenterDiffSpec = data.m_iConcenterCenterDiffSpec;
	m_dConcenterCenterDiffSpecX = data.m_dConcenterCenterDiffSpecX;
	m_dConcenterCenterDiffSpecY = data.m_dConcenterCenterDiffSpecY;

	m_bUseHomerMark = data.m_bUseHomerMark;
	m_iHomerMarkPos = data.m_iHomerMarkPos;
	m_iHomerMarkOrder = data.m_iHomerMarkOrder;

	m_bUseROIAlign = data.m_bUseROIAlign;
	m_iROIAlignXSearchMargin = data.m_iROIAlignXSearchMargin;
	m_iROIAlignYSearchMargin = data.m_iROIAlignYSearchMargin;
	m_iROIAlignMatchingAngleRange = data.m_iROIAlignMatchingAngleRange;
	m_dROIAlignMatchingScaleMin = data.m_dROIAlignMatchingScaleMin;
	m_dROIAlignMatchingScaleMax = data.m_dROIAlignMatchingScaleMax;
	m_iROIAlignMatchingMinContrast = data.m_iROIAlignMatchingMinContrast;
	m_bROIAlignUsePosX = data.m_bROIAlignUsePosX;
	m_bROIAlignUsePosY = data.m_bROIAlignUsePosY;
	m_dROIAlignMatchingScore = data.m_dROIAlignMatchingScore;
	m_bROIAlignInspectShift = data.m_bROIAlignInspectShift;
	m_iROIAlignMatchingPLevel = data.m_iROIAlignMatchingPLevel;
	m_dROIAlignShiftTop = data.m_dROIAlignShiftTop;
	m_dROIAlignShiftBottom = data.m_dROIAlignShiftBottom;
	m_dROIAlignShiftLeft = data.m_dROIAlignShiftLeft;
	m_dROIAlignShiftRight = data.m_dROIAlignShiftRight;
	m_bROIAlignLocalAlignUse = data.m_bROIAlignLocalAlignUse;
	m_bUseIsoColorImage = data.m_bUseIsoColorImage;
	m_bROIAlignMultiModuleInspect = data.m_bROIAlignMultiModuleInspect;
	m_iROIAlignMultiModuleXNumber = data.m_iROIAlignMultiModuleXNumber;
	m_iROIAlignMultiModuleYNumber = data.m_iROIAlignMultiModuleYNumber;
	m_iROIAlignMultiModuleXPitch = data.m_iROIAlignMultiModuleXPitch;
	m_iROIAlignMultiModuleYPitch = data.m_iROIAlignMultiModuleYPitch;
	m_iROIAlignMultiModuleAlignTarget = data.m_iROIAlignMultiModuleAlignTarget;
	m_iROIAlignMultiModuleVision = data.m_iROIAlignMultiModuleVision;

	m_iProcessChType = data.m_iProcessChType;
	m_iProcessResType = data.m_iProcessResType;
	m_bUseImageProcess = data.m_bUseImageProcess;
	m_iImageProcessArithmeticType = data.m_iImageProcessArithmeticType;
	m_iImageProcessArithmeticImage1 = data.m_iImageProcessArithmeticImage1;
	m_iImageProcessArithmeticImage2 = data.m_iImageProcessArithmeticImage2;
	m_iImageProcessArithmeticImage3 = data.m_iImageProcessArithmeticImage3;
	m_iImageProcessArithmeticImage4 = data.m_iImageProcessArithmeticImage4;
	m_dImageProcessArithmeticMulti = data.m_dImageProcessArithmeticMulti;
	m_iImageProcessArithmeticAdd = data.m_iImageProcessArithmeticAdd;
	m_bUseImageProcessFilter = data.m_bUseImageProcessFilter;
	m_iImageProcessFilterType1 = data.m_iImageProcessFilterType1;
	m_iImageProcessFilterType2 = data.m_iImageProcessFilterType2;
	m_iImageProcessFilterType3 = data.m_iImageProcessFilterType3;
	m_iImageProcessFilterType1X = data.m_iImageProcessFilterType1X;
	m_iImageProcessFilterType2X = data.m_iImageProcessFilterType2X;
	m_iImageProcessFilterType3X = data.m_iImageProcessFilterType3X;
	m_iImageProcessFilterType1Y = data.m_iImageProcessFilterType1Y;
	m_iImageProcessFilterType2Y = data.m_iImageProcessFilterType2Y;
	m_iImageProcessFilterType3Y = data.m_iImageProcessFilterType3Y;

	m_bUseImageProcessPostLocalAlign = data.m_bUseImageProcessPostLocalAlign;
	m_iImageProcessPostLocalAlignImageIndex = data.m_iImageProcessPostLocalAlignImageIndex;
	m_iImageProcessPostLocalAlignROINo = data.m_iImageProcessPostLocalAlignROINo;

	m_bUseImageProcessLocalAlign = data.m_bUseImageProcessLocalAlign;
	m_iImageProcessLocalAlignImageIndex = data.m_iImageProcessLocalAlignImageIndex;
	m_iImageProcessLocalAlignROINo = data.m_iImageProcessLocalAlignROINo;
	m_bImageProcessLocalAlignPosX = data.m_bImageProcessLocalAlignPosX;
	m_bImageProcessLocalAlignPosY = data.m_bImageProcessLocalAlignPosY;
	m_bImageProcessLocalAlignAngle = data.m_bImageProcessLocalAlignAngle;
	m_iImageProcessLocalAlignFitPos = data.m_iImageProcessLocalAlignFitPos;
	m_iImageProcessPostLocalAlignFitPos = data.m_iImageProcessPostLocalAlignFitPos;
	m_iImageProcessLocalAlignROIType = data.m_iImageProcessLocalAlignROIType;
	m_iImageProcessLocalAlignMatchingApplyMethod = data.m_iImageProcessLocalAlignMatchingApplyMethod;
	m_bUseImageProcessDontCare = data.m_bUseImageProcessDontCare;
	m_iImageProcessDontCareImageIndex = data.m_iImageProcessDontCareImageIndex;
	m_iImageProcessDontCareROINo = data.m_iImageProcessDontCareROINo;
	m_iImageProcessDontCareMargin = data.m_iImageProcessDontCareMargin;
	m_iImageProcessDontCareMarginInner = data.m_iImageProcessDontCareMarginInner;
	m_bUseImageProcessDontCare2 = data.m_bUseImageProcessDontCare2;
	m_iImageProcessDontCareImageIndex2 = data.m_iImageProcessDontCareImageIndex2;
	m_iImageProcessDontCareROINo2 = data.m_iImageProcessDontCareROINo2;
	m_iImageProcessDontCareMargin2 = data.m_iImageProcessDontCareMargin2;
	m_iImageProcessDontCareMargin2Inner = data.m_iImageProcessDontCareMargin2Inner;
	m_bUseImageProcessGenerate = data.m_bUseImageProcessGenerate;
	m_bUseImageProcessGenerateInspectFlag = data.m_bUseImageProcessGenerateInspectFlag;
	m_iImageProcessGenerateImageIndex = data.m_iImageProcessGenerateImageIndex;
	m_iImageProcessGenerateROINo = data.m_iImageProcessGenerateROINo;
	m_iImageProcessGenerateMargin = data.m_iImageProcessGenerateMargin;
	m_bUseImageProcessMask = data.m_bUseImageProcessMask;
	m_iImageProcessMaskImageIndex = data.m_iImageProcessMaskImageIndex;
	m_iImageProcessMaskROINo = data.m_iImageProcessMaskROINo;
	m_bUseImageProcessMask2 = data.m_bUseImageProcessMask2;
	m_iImageProcessMaskImageIndex2 = data.m_iImageProcessMaskImageIndex2;
	m_iImageProcessMaskROINo2 = data.m_iImageProcessMaskROINo2;

	m_bUseImageProcessLocalAlignAddLineFit = data.m_bUseImageProcessLocalAlignAddLineFit;
	m_iImageProcessLocalAlignImageIndex_2 = data.m_iImageProcessLocalAlignImageIndex_2;
	m_iImageProcessLocalAlignROINo_2 = data.m_iImageProcessLocalAlignROINo_2;
	m_iImageProcessLocalAlignFitPos_2 = data.m_iImageProcessLocalAlignFitPos_2;

	m_bUseBarcode = data.m_bUseBarcode;
	m_dBarcodeShiftToleranceX = data.m_dBarcodeShiftToleranceX;
	m_dBarcodeShiftToleranceY = data.m_dBarcodeShiftToleranceY;
	m_bInspectBarcodeShift = data.m_bInspectBarcodeShift;
	m_bInspectBarcodeRotation = data.m_bInspectBarcodeRotation;
	m_bUseBarcodeOtherImage = data.m_bUseBarcodeOtherImage;
	m_iBarcodeOtherImageNumber = data.m_iBarcodeOtherImageNumber;

	//2017.04.11 hbk
	m_bUseOCR = data.m_bUseOCR;
	m_lCharPlateWidth = data.m_lCharPlateWidth;
	m_lMinCharWidth = data.m_lMinCharWidth;
	m_lMaxCharWidth = data.m_lMaxCharWidth;
	m_lMinCharHeight = data.m_lMinCharHeight;
	m_lMaxCharHeight = data.m_lMaxCharHeight;
	m_lMinCharCandidateHeight = data.m_lMinCharCandidateHeight;
	m_lWholeAreaDivide = data.m_lWholeAreaDivide;
	m_lHorizontalDivide = data.m_lHorizontalDivide;
	m_lFirstDiffernt = data.m_lFirstDiffernt;
	m_lSecondDifferent = data.m_lSecondDifferent;
	m_ldiff = data.m_ldiff;
	m_iOCRSegmentDilation = data.m_iOCRSegmentDilation;

	m_bUseShape = data.m_bUseShape;
	m_iShapeIlluminationFilterX = data.m_iShapeIlluminationFilterX;
	m_iShapeIlluminationFilterY = data.m_iShapeIlluminationFilterY;
	m_dShapeIlluminationScaleFactor = data.m_dShapeIlluminationScaleFactor;
	m_dShapeCurvatureSmFactor = data.m_dShapeCurvatureSmFactor;
	m_dShapeScalingMulti = data.m_dShapeScalingMulti;
	m_dShapeScalingAdd = data.m_dShapeScalingAdd;
	m_dShapeImageReduceRatio = data.m_dShapeImageReduceRatio;
	m_dShapeLedAngle = data.m_dShapeLedAngle;
	m_iShapeImageMargin = data.m_iShapeImageMargin;

	// Don't Care
	m_iProcessChTypeDC = data.m_iProcessChTypeDC;
	m_bUseDC = data.m_bUseDC;

	m_bUseBrightFixedThresDC = data.m_bUseBrightFixedThresDC;
	m_iBrightLowerThresDC = data.m_iBrightLowerThresDC;
	m_iBrightUpperThresDC = data.m_iBrightUpperThresDC;
	m_bUseBrightDTDC = data.m_bUseBrightDTDC;
	m_iBrightMedianFilterSizeDC = data.m_iBrightMedianFilterSizeDC;
	m_iBrightClosingFilterSizeDC = data.m_iBrightClosingFilterSizeDC;
	m_iBrightDTFilterTypeDC = data.m_iBrightDTFilterTypeDC;
	m_iBrightDTFilterSizeDC = data.m_iBrightDTFilterSizeDC;
	m_iBrightDTFilterSizeDC2 = data.m_iBrightDTFilterSizeDC2;
	m_iBrightDTValueDC = data.m_iBrightDTValueDC;
	m_bUseBrightUniformityCheckDC = data.m_bUseBrightUniformityCheckDC;
	m_iBrightUniformityPeakMinDC = data.m_iBrightUniformityPeakMinDC;
	m_iBrightUniformityPeakMaxDC = data.m_iBrightUniformityPeakMaxDC;
	m_iBrightUniformityOffsetDC = data.m_iBrightUniformityOffsetDC;
	m_iBrightUniformityHystLengthDC = data.m_iBrightUniformityHystLengthDC;
	m_iBrightUniformityHystOffsetDC = data.m_iBrightUniformityHystOffsetDC;

	m_bUseDarkFixedThresDC = data.m_bUseDarkFixedThresDC;
	m_iDarkLowerThresDC = data.m_iDarkLowerThresDC;
	m_iDarkUpperThresDC = data.m_iDarkUpperThresDC;
	m_bUseDarkDTDC = data.m_bUseDarkDTDC;
	m_iDarkMedianFilterSizeDC = data.m_iDarkMedianFilterSizeDC;
	m_iDarkClosingFilterSizeDC = data.m_iDarkClosingFilterSizeDC;
	m_iDarkDTFilterTypeDC = data.m_iDarkDTFilterTypeDC;
	m_iDarkDTFilterSizeDC = data.m_iDarkDTFilterSizeDC;
	m_iDarkDTFilterSizeDC2 = data.m_iDarkDTFilterSizeDC2;
	m_iDarkDTValueDC = data.m_iDarkDTValueDC;
	m_bUseDarkUniformityCheckDC = data.m_bUseDarkUniformityCheckDC;
	m_iDarkUniformityPeakMinDC = data.m_iDarkUniformityPeakMinDC;
	m_iDarkUniformityPeakMaxDC = data.m_iDarkUniformityPeakMaxDC;
	m_iDarkUniformityOffsetDC = data.m_iDarkUniformityOffsetDC;
	m_iDarkUniformityHystLengthDC = data.m_iDarkUniformityHystLengthDC;
	m_iDarkUniformityHystOffsetDC = data.m_iDarkUniformityHystOffsetDC;

	m_iCandidateMinAreaDC = data.m_iCandidateMinAreaDC;
	m_bCombineBrightDarkBlobDC = data.m_bCombineBrightDarkBlobDC;
	m_bMaxBlobDC = data.m_bMaxBlobDC;
	m_bClosingOpeningDC = data.m_bClosingOpeningDC;
	m_iOpeningSizeDC = data.m_iOpeningSizeDC;
	m_iClosingSizeDC = data.m_iClosingSizeDC;
	m_bFillupDC = data.m_bFillupDC;
	m_bUseConvexDC = data.m_bUseConvexDC;
	m_bUseConnectionDC = data.m_bUseConnectionDC;
	m_iConnectionMinSizeDC = data.m_iConnectionMinSizeDC;
	m_iConnectionMinXLengthDC = data.m_iConnectionMinXLengthDC;
	m_iConnectionLengthDC = data.m_iConnectionLengthDC;
	m_bUseAngleDC = data.m_bUseAngleDC;
	m_iConnectionMaxWidthDC = data.m_iConnectionMaxWidthDC;

	m_bUseDefectConditionAreaDC = data.m_bUseDefectConditionAreaDC;
	m_bUseDefectConditionLengthDC = data.m_bUseDefectConditionLengthDC;
	m_bUseDefectConditionWidthDC = data.m_bUseDefectConditionWidthDC;
	m_bUseDefectConditionXLengthDC = data.m_bUseDefectConditionXLengthDC;
	m_bUseDefectConditionYLengthDC = data.m_bUseDefectConditionYLengthDC;
	m_bUseDefectConditionMeanDC = data.m_bUseDefectConditionMeanDC;
	m_bUseDefectConditionStdevDC = data.m_bUseDefectConditionStdevDC;
	m_bUseDefectConditionAnisometryDC = data.m_bUseDefectConditionAnisometryDC;
	m_bUseDefectConditionCircularityDC = data.m_bUseDefectConditionCircularityDC;
	m_bUseDefectConditionRectangularityDC = data.m_bUseDefectConditionRectangularityDC;
	m_bUseDefectConditionConvexityDC = data.m_bUseDefectConditionConvexityDC;
	m_bUseDefectConditionEllipseRatioDC = data.m_bUseDefectConditionEllipseRatioDC;
	m_bUseDefectConditionAreaRatioDC = data.m_bUseDefectConditionAreaRatioDC;
	m_bUseDefectConditionContrastDC = data.m_bUseDefectConditionContrastDC;
	m_bUseDefectConditionBlobDC = data.m_bUseDefectConditionBlobDC;
	m_bUseDefectConditionShiftXDC = data.m_bUseDefectConditionShiftXDC;
	m_bUseDefectConditionShiftYDC = data.m_bUseDefectConditionShiftYDC;
	m_bUseDefectConditionOuterDistDC = data.m_bUseDefectConditionOuterDistDC;
	m_bUseDefectConditionInnerDistDC = data.m_bUseDefectConditionInnerDistDC;
	m_bUseDefectConditionUseZeroAreaDC = data.m_bUseDefectConditionUseZeroAreaDC;
	m_bDefectConditionBoundaryConnectionDC = data.m_bDefectConditionBoundaryConnectionDC;
	m_bDefectConditionBoundaryMarginTopDC = data.m_bDefectConditionBoundaryMarginTopDC;
	m_bDefectConditionBoundaryMarginBottomDC = data.m_bDefectConditionBoundaryMarginBottomDC;
	m_bDefectConditionBoundaryMarginLeftDC = data.m_bDefectConditionBoundaryMarginLeftDC;
	m_bDefectConditionBoundaryMarginRightDC = data.m_bDefectConditionBoundaryMarginRightDC;

	m_iDefectConditionAreaMinDC = data.m_iDefectConditionAreaMinDC;
	m_iDefectConditionLengthMinDC = data.m_iDefectConditionLengthMinDC;
	m_iDefectConditionWidthMinDC = data.m_iDefectConditionWidthMinDC;
	m_iDefectConditionXLengthMinDC = data.m_iDefectConditionXLengthMinDC;
	m_iDefectConditionYLengthMinDC = data.m_iDefectConditionYLengthMinDC;
	m_iDefectConditionMeanMinDC = data.m_iDefectConditionMeanMinDC;
	m_dDefectConditionStdevMinDC = data.m_dDefectConditionStdevMinDC;
	m_dDefectConditionAnisometryMinDC = data.m_dDefectConditionAnisometryMinDC;
	m_dDefectConditionCircularityMinDC = data.m_dDefectConditionCircularityMinDC;
	m_dDefectConditionRectangularityMinDC = data.m_dDefectConditionRectangularityMinDC;
	m_dDefectConditionConvexityMinDC = data.m_dDefectConditionConvexityMinDC;
	m_dDefectConditionEllipseRatioMinDC = data.m_dDefectConditionEllipseRatioMinDC;
	m_iDefectConditionAreaRatioMinDC = data.m_iDefectConditionAreaRatioMinDC;
	m_iDefectConditionContrastMinDC = data.m_iDefectConditionContrastMinDC;
	m_iDefectConditionBlobMinDC = data.m_iDefectConditionBlobMinDC;
	m_iDefectConditionShiftXMinDC = data.m_iDefectConditionShiftXMinDC;
	m_iDefectConditionShiftYMinDC = data.m_iDefectConditionShiftYMinDC;

	m_iDefectConditionAreaMaxDC = data.m_iDefectConditionAreaMaxDC;
	m_iDefectConditionLengthMaxDC = data.m_iDefectConditionLengthMaxDC;
	m_iDefectConditionWidthMaxDC = data.m_iDefectConditionWidthMaxDC;
	m_iDefectConditionXLengthMaxDC = data.m_iDefectConditionXLengthMaxDC;
	m_iDefectConditionYLengthMaxDC = data.m_iDefectConditionYLengthMaxDC;
	m_iDefectConditionMeanMaxDC = data.m_iDefectConditionMeanMaxDC;
	m_dDefectConditionStdevMaxDC = data.m_dDefectConditionStdevMaxDC;
	m_dDefectConditionAnisometryMaxDC = data.m_dDefectConditionAnisometryMaxDC;
	m_dDefectConditionCircularityMaxDC = data.m_dDefectConditionCircularityMaxDC;
	m_dDefectConditionRectangularityMaxDC = data.m_dDefectConditionRectangularityMaxDC;
	m_dDefectConditionConvexityMaxDC = data.m_dDefectConditionConvexityMaxDC;
	m_dDefectConditionEllipseRatioMaxDC = data.m_dDefectConditionEllipseRatioMaxDC;
	m_iDefectConditionAreaRatioMaxDC = data.m_iDefectConditionAreaRatioMaxDC;
	m_iDefectConditionContrastMaxDC = data.m_iDefectConditionContrastMaxDC;
	m_iDefectConditionBlobMaxDC = data.m_iDefectConditionBlobMaxDC;
	m_iDefectConditionShiftXMaxDC = data.m_iDefectConditionShiftXMaxDC;
	m_iDefectConditionShiftYMaxDC = data.m_iDefectConditionShiftYMaxDC;

	m_iDefectConditionOuterDistDC = data.m_iDefectConditionOuterDistDC;
	m_iDefectConditionInnerDistDC = data.m_iDefectConditionInnerDistDC;

	m_iDefectConditionBoundaryMarginTopDC = data.m_iDefectConditionBoundaryMarginTopDC;
	m_iDefectConditionBoundaryMarginBottomDC = data.m_iDefectConditionBoundaryMarginBottomDC;
	m_iDefectConditionBoundaryMarginLeftDC = data.m_iDefectConditionBoundaryMarginLeftDC;
	m_iDefectConditionBoundaryMarginRightDC = data.m_iDefectConditionBoundaryMarginRightDC;
	m_iDefectConditionBoundaryMarginMinConnectionDC = data.m_iDefectConditionBoundaryMarginMinConnectionDC;

	m_iLengthWidthConditionDC = data.m_iLengthWidthConditionDC;
	m_iXYLengthConditionDC = data.m_iXYLengthConditionDC;

	m_iBoundaryConnectionConditionDC = data.m_iBoundaryConnectionConditionDC;
	m_bDefectConditionBoundaryConnection2SideDC = data.m_bDefectConditionBoundaryConnection2SideDC;

	m_iDontcareRegionMargin = data.m_iDontcareRegionMargin;

	m_bUseEdgeMeasure = data.m_bUseEdgeMeasure;
	m_iEdgeMeasureOnePoint = data.m_iEdgeMeasureOnePoint;
	m_iEdgeMeasureUseMaxPoint = data.m_iEdgeMeasureUseMaxPoint;
	m_iEdgeMeasureMultiPointNumber = data.m_iEdgeMeasureMultiPointNumber;
	m_bEdgeMeasureEndPoint = data.m_bEdgeMeasureEndPoint;
	m_bEdgeMeasureSlope = data.m_bEdgeMeasureSlope;
	m_bEdgeMeasureReverseGv = data.m_bEdgeMeasureReverseGv;
	m_bEdgeMeasureFixedEndPoint = data.m_bEdgeMeasureFixedEndPoint;
	m_iEdgeMeasureLeftSideSize = data.m_iEdgeMeasureLeftSideSize;
	m_iEdgeMeasureRightSideSize = data.m_iEdgeMeasureRightSideSize;
	m_bEdgeMeasureUseMaxMinPoint = data.m_bEdgeMeasureUseMaxMinPoint;
	m_iEdgeMeasurePositionRange = data.m_iEdgeMeasurePositionRange;
	m_iEdgeMeasureDir = data.m_iEdgeMeasureDir;
	m_iEdgeMeasureGv = data.m_iEdgeMeasureGv;
	m_iEdgeMeasurePos = data.m_iEdgeMeasurePos;
	m_bEdgeMeasureLocalAlignUse = data.m_bEdgeMeasureLocalAlignUse;
	m_dEdgeMeasureSmFactor = data.m_dEdgeMeasureSmFactor;
	m_iEdgeMeasureEdgeStr = data.m_iEdgeMeasureEdgeStr;
	m_iEdgeMeasurePosOffset = data.m_iEdgeMeasurePosOffset;
	m_bEdgeMeasureUseOther = data.m_bEdgeMeasureUseOther;
	m_bEdgeMeasureMakeROIOther = data.m_bEdgeMeasureMakeROIOther;
	m_iEdgeMeasureMakeROIRangeOther = data.m_iEdgeMeasureMakeROIRangeOther;
	m_iEdgeMeasureImageIndexOther = data.m_iEdgeMeasureImageIndexOther;
	m_iEdgeMeasureGvOther = data.m_iEdgeMeasureGvOther;
	m_iEdgeMeasurePosOther = data.m_iEdgeMeasurePosOther;
	m_dEdgeMeasureSmFactorOther = data.m_dEdgeMeasureSmFactorOther;
	m_iEdgeMeasureEdgeStrOther = data.m_iEdgeMeasureEdgeStrOther;
	m_iEdgeMeasurePosOffsetOther = data.m_iEdgeMeasurePosOffsetOther;
	m_dEdgeMeasureAngleDeg = data.m_dEdgeMeasureAngleDeg;
	m_iEdgeMeasureAccuracy = data.m_iEdgeMeasureAccuracy;

	m_bUseHomerTilt = data.m_bUseHomerTilt;
	m_iHomerTiltHomerEdgeStr = data.m_iHomerTiltHomerEdgeStr;
	m_iHomerTiltBenvolioImage = data.m_iHomerTiltBenvolioImage;
	m_iHomerTiltBenvolioEdgeStr = data.m_iHomerTiltBenvolioEdgeStr;
	m_dHomerTiltHomerLength = data.m_dHomerTiltHomerLength;
	m_dHomerTiltTol = data.m_dHomerTiltTol;
	m_dHomerTiltDistanceTol = data.m_dHomerTiltDistanceTol;

	m_bUseEpoxyVoidHole = data.m_bUseEpoxyVoidHole;
	m_iEpoxyVoidHoleImage = data.m_iEpoxyVoidHoleImage;
	m_dEpoxyVoidHoleEdgeStr = data.m_dEpoxyVoidHoleEdgeStr;
	m_iEpoxyVoidHoleInspectMargin = data.m_iEpoxyVoidHoleInspectMargin;
	m_bEpoxyVoidHoleTiltInspect = data.m_bEpoxyVoidHoleTiltInspect;
	m_iEpoxyVoidHoleTiltTolerance = data.m_iEpoxyVoidHoleTiltTolerance;
	m_bEpoxyVoidHoleGapInspect = data.m_bEpoxyVoidHoleGapInspect;
	m_iEpoxyVoidHoleGapTolerance = data.m_iEpoxyVoidHoleGapTolerance;

	m_bUsePrintQuality = data.m_bUsePrintQuality;
	m_iPrintQualityContrastMin = data.m_iPrintQualityContrastMin;
	m_iPrintQualitySymbolColsMin = data.m_iPrintQualitySymbolColsMin;
	m_iPrintQualitySymbolRowsMin = data.m_iPrintQualitySymbolRowsMin;
	m_iPrintQualitySymbolColsMax = data.m_iPrintQualitySymbolColsMax;
	m_iPrintQualitySymbolRowsMax = data.m_iPrintQualitySymbolRowsMax;
	m_iPrintQualityModuleSizeMin = data.m_iPrintQualityModuleSizeMin;
	m_iPrintQualityModuleSizeMax = data.m_iPrintQualityModuleSizeMax;
	m_dPrintQualitySlantMax = data.m_dPrintQualitySlantMax;
	m_bPrintQualityOverallQuality = data.m_bPrintQualityOverallQuality;
	m_iPrintQualityGradeOverallQuality = data.m_iPrintQualityGradeOverallQuality;
	m_bPrintQualityUnusedErrorCorrection = data.m_bPrintQualityUnusedErrorCorrection;
	m_iPrintQualityGradeUnusedErrorCorrection = data.m_iPrintQualityGradeUnusedErrorCorrection;
	m_iPrintQualityStrictModel = data.m_iPrintQualityStrictModel;
	m_iPrintQualityPolarity = data.m_iPrintQualityPolarity;
	m_iPrintQualityMirrored = data.m_iPrintQualityMirrored;
	m_iPrintQualityContrastTolerance = data.m_iPrintQualityContrastTolerance;
	m_iPrintQualityStrictQuietZone = data.m_iPrintQualityStrictQuietZone;
	m_iPrintQualitySymbolShape = data.m_iPrintQualitySymbolShape;
	m_iPrintQualitySmallModulesRobustness = data.m_iPrintQualitySmallModulesRobustness;
	m_iPrintQualityModuleGrid = data.m_iPrintQualityModuleGrid;
	m_iPrintQualityModuleGapMin = data.m_iPrintQualityModuleGapMin;
	m_iPrintQualityModuleGapMax = data.m_iPrintQualityModuleGapMax;
	m_iPrintQualityFinderPatternTolerance = data.m_iPrintQualityFinderPatternTolerance;

	m_bUseROIAnisoAlign = data.m_bUseROIAnisoAlign;
	m_iROIAnisoAlignXSearchMargin = data.m_iROIAnisoAlignXSearchMargin;
	m_iROIAnisoAlignYSearchMargin = data.m_iROIAnisoAlignYSearchMargin;
	m_iROIAnisoAlignMatchingAngleRange = data.m_iROIAnisoAlignMatchingAngleRange;
	m_dROIAnisoAlignMatchingScaleMinY = data.m_dROIAnisoAlignMatchingScaleMinY;
	m_dROIAnisoAlignMatchingScaleMaxY = data.m_dROIAnisoAlignMatchingScaleMaxY;
	m_dROIAnisoAlignMatchingScaleMinX = data.m_dROIAnisoAlignMatchingScaleMinX;
	m_dROIAnisoAlignMatchingScaleMaxX = data.m_dROIAnisoAlignMatchingScaleMaxX;
	m_iROIAnisoAlignMatchingMinContrast = data.m_iROIAnisoAlignMatchingMinContrast;
	m_bROIAnisoAlignUsePosX = data.m_bROIAnisoAlignUsePosX;
	m_bROIAnisoAlignUsePosY = data.m_bROIAnisoAlignUsePosY;
	m_bROIAnisoAlignInspectShift = data.m_bROIAnisoAlignInspectShift;
	m_iROIAnisoAlignMatchingPLevel = data.m_iROIAnisoAlignMatchingPLevel;
	m_dROIAnisoAlignShiftTop = data.m_dROIAnisoAlignShiftTop;
	m_dROIAnisoAlignShiftBottom = data.m_dROIAnisoAlignShiftBottom;
	m_dROIAnisoAlignShiftLeft = data.m_dROIAnisoAlignShiftLeft;
	m_dROIAnisoAlignShiftRight = data.m_dROIAnisoAlignShiftRight;
	m_bROIAnisoAlignLocalAlignUse = data.m_bROIAnisoAlignLocalAlignUse;
	m_dROIAnisoAlignMatchingScore = data.m_dROIAnisoAlignMatchingScore;
	m_bUseAnisoColorImage = data.m_bUseAnisoColorImage;
	m_bROIAnisoAlignMultiModuleInspect = data.m_bROIAnisoAlignMultiModuleInspect;
	m_iROIAnisoAlignMultiModuleXNumber = data.m_iROIAnisoAlignMultiModuleXNumber;
	m_iROIAnisoAlignMultiModuleYNumber = data.m_iROIAnisoAlignMultiModuleYNumber;
	m_iROIAnisoAlignMultiModuleXPitch = data.m_iROIAnisoAlignMultiModuleXPitch;
	m_iROIAnisoAlignMultiModuleYPitch = data.m_iROIAnisoAlignMultiModuleYPitch;
	m_iROIAnisoAlignMultiModuleAlignTarget = data.m_iROIAnisoAlignMultiModuleAlignTarget;
	m_iROIAnisoAlignMultiModuleVision = data.m_iROIAnisoAlignMultiModuleVision;

	m_bUseStiffenerEpoxy = data.m_bUseStiffenerEpoxy;
	m_iThresholdParam_StiffenerRegion = data.m_iThresholdParam_StiffenerRegion;
	m_iDTFilterX_StiffenerRegion = data.m_iDTFilterX_StiffenerRegion;
	m_iDTFilterY_StiffenerRegion = data.m_iDTFilterY_StiffenerRegion;
	m_iDTValue_StiffenerRegion = data.m_iDTValue_StiffenerRegion;
	m_iClosing_StiffenerRegion = data.m_iClosing_StiffenerRegion;
	m_dErosionParam_StiffenerBoundary = data.m_dErosionParam_StiffenerBoundary;
	m_iThresholdParam_SheildcanRegion = data.m_iThresholdParam_SheildcanRegion;
	m_dDilationParam_SheildcanBoundary = data.m_dDilationParam_SheildcanBoundary;
	m_dOpeningParam_CornerBoundary = data.m_dOpeningParam_CornerBoundary;

	m_bUseDistanceMeasure = data.m_bUseDistanceMeasure;
	m_iDistanceMeasureUpperGv = data.m_iDistanceMeasureUpperGv;
	m_iDistanceMeasureUpperPos = data.m_iDistanceMeasureUpperPos;
	m_dDistanceMeasureUpperSmFactor = data.m_dDistanceMeasureUpperSmFactor;
	m_iDistanceMeasureUpperEdgeStr = data.m_iDistanceMeasureUpperEdgeStr;
	m_iDistanceMeasureLowerGv = data.m_iDistanceMeasureLowerGv;
	m_iDistanceMeasureLowerPos = data.m_iDistanceMeasureLowerPos;
	m_dDistanceMeasureLowerSmFactor = data.m_dDistanceMeasureLowerSmFactor;
	m_iDistanceMeasureLowerEdgeStr = data.m_iDistanceMeasureLowerEdgeStr;
	m_iDistanceMeasureUpperPosOffset = data.m_iDistanceMeasureUpperPosOffset;
	m_iDistanceMeasureLowerPosOffset = data.m_iDistanceMeasureLowerPosOffset;
	m_iDistanceMeasureUsageType = data.m_iDistanceMeasureUsageType;
	m_iDistanceMeasureSaveID = data.m_iDistanceMeasureSaveID;
	m_dDistanceMeasureGapInspectionUpper = data.m_dDistanceMeasureGapInspectionUpper;
	m_dDistanceMeasureGapInspectionLower = data.m_dDistanceMeasureGapInspectionLower;
	m_iDistanceMeasureROIType = data.m_iDistanceMeasureROIType;

	m_bUseGapMeasure = data.m_bUseGapMeasure;
	m_iGapMeasureUpperGv = data.m_iGapMeasureUpperGv;
	m_iGapMeasureUpperPos = data.m_iGapMeasureUpperPos;
	m_dGapMeasureUpperSmFactor = data.m_dGapMeasureUpperSmFactor;
	m_iGapMeasureUpperEdgeStr = data.m_iGapMeasureUpperEdgeStr;
	m_iGapMeasureLowerGv = data.m_iGapMeasureLowerGv;
	m_iGapMeasureLowerPos = data.m_iGapMeasureLowerPos;
	m_dGapMeasureLowerSmFactor = data.m_dGapMeasureLowerSmFactor;
	m_iGapMeasureLowerEdgeStr = data.m_iGapMeasureLowerEdgeStr;
	m_iGapMeasureUpperPosOffset = data.m_iGapMeasureUpperPosOffset;
	m_iGapMeasureLowerPosOffset = data.m_iGapMeasureLowerPosOffset;
	m_iGapMeasureSaveID = data.m_iGapMeasureSaveID;
	m_iGapMeasureImageNo1 = data.m_iGapMeasureImageNo1;
	m_iGapMeasureImageNo2 = data.m_iGapMeasureImageNo2;
	m_dGapMeasureGapInspectionUpper = data.m_dGapMeasureGapInspectionUpper;
	m_dGapMeasureGapInspectionLower = data.m_dGapMeasureGapInspectionLower;
	m_dGapMeasureGapMeasureOffset = data.m_dGapMeasureGapMeasureOffset;
	m_iGapMeasureDir = data.m_iGapMeasureDir;
	m_bGapMeasureAlarm = data.m_bGapMeasureAlarm;
	m_iGapMeasurePointNumber = data.m_iGapMeasurePointNumber;
	m_iGapMeasureSelectType = data.m_iGapMeasureSelectType;
	m_bGapMeasureUpperLine = data.m_bGapMeasureUpperLine;
	m_bGapMeasureLowerLine = data.m_bGapMeasureLowerLine;
	m_bGapMeasureDiffImage = data.m_bGapMeasureDiffImage;
	m_iGapMeasurePosStart = data.m_iGapMeasurePosStart;
	m_iGapMeasurePosEnd = data.m_iGapMeasurePosEnd;
	m_iGapMeasureImageNo3 = data.m_iGapMeasureImageNo3;
	m_bGapMeasureEndRemove = data.m_bGapMeasureEndRemove;
	m_iGapMeasureRemoveEndUpper = data.m_iGapMeasureRemoveEndUpper;
	m_iGapMeasureRemoveEndLower = data.m_iGapMeasureRemoveEndLower;
	m_bGapMeasureRetry = data.m_bGapMeasureRetry;
	m_iGapMeasureRetryImageNo = data.m_iGapMeasureRetryImageNo;
	m_iGapMeasureRetryGv = data.m_iGapMeasureRetryGv;
	m_iGapMeasureRetryPos = data.m_iGapMeasureRetryPos;
	m_dGapMeasureRetrySmFactor = data.m_dGapMeasureRetrySmFactor;
	m_iGapMeasureRetryEdgeStr = data.m_iGapMeasureRetryEdgeStr;
	m_iGapMeasureRetryPosOffset = data.m_iGapMeasureRetryPosOffset;

	m_bUseSurfaceDualInspection = data.m_bUseSurfaceDualInspection;
	m_iSurfaceDualImageIndex_1 = data.m_iSurfaceDualImageIndex_1;
	m_iSurfaceDualImageIndex_2 = data.m_iSurfaceDualImageIndex_2;
	m_iSurfaceDualDefectType_1 = data.m_iSurfaceDualDefectType_1;
	m_iSurfaceDualDefectType_2 = data.m_iSurfaceDualDefectType_2;

	m_bUseBrightFixedThres_1 = data.m_bUseBrightFixedThres_1;
	m_iBrightLowerThres_1 = data.m_iBrightLowerThres_1;
	m_iBrightUpperThres_1 = data.m_iBrightUpperThres_1;
	m_bUseBrightDT_1 = data.m_bUseBrightDT_1;
	m_iBrightMedianFilterSize_1 = data.m_iBrightMedianFilterSize_1;
	m_iBrightClosingFilterSize_1 = data.m_iBrightClosingFilterSize_1;
	m_iBrightDTFilterType_1 = data.m_iBrightDTFilterType_1;
	m_iBrightDTFilterSize_1 = data.m_iBrightDTFilterSize_1;
	m_iBrightDTFilterSize2_1 = data.m_iBrightDTFilterSize2_1;
	m_iBrightDTValue_1 = data.m_iBrightDTValue_1;
	m_bUseDTPolarTrans_1 = data.m_bUseDTPolarTrans_1;
	m_bUseBrightHystThres_1 = data.m_bUseBrightHystThres_1;
	m_iBrightHystSecureThres_1 = data.m_iBrightHystSecureThres_1;
	m_iBrightHystPotentialThres_1 = data.m_iBrightHystPotentialThres_1;
	m_iBrightHystPotentialLength_1 = data.m_iBrightHystPotentialLength_1;

	m_bUseBrightFixedThres_2 = data.m_bUseBrightFixedThres_2;
	m_iBrightLowerThres_2 = data.m_iBrightLowerThres_2;
	m_iBrightUpperThres_2 = data.m_iBrightUpperThres_2;
	m_bUseBrightDT_2 = data.m_bUseBrightDT_2;
	m_iBrightMedianFilterSize_2 = data.m_iBrightMedianFilterSize_2;
	m_iBrightClosingFilterSize_2 = data.m_iBrightClosingFilterSize_2;
	m_iBrightDTFilterType_2 = data.m_iBrightDTFilterType_2;
	m_iBrightDTFilterSize_2 = data.m_iBrightDTFilterSize_2;
	m_iBrightDTFilterSize2_2 = data.m_iBrightDTFilterSize2_2;
	m_iBrightDTValue_2 = data.m_iBrightDTValue_2;
	m_bUseDTPolarTrans_2 = data.m_bUseDTPolarTrans_2;
	m_bUseBrightHystThres_2 = data.m_bUseBrightHystThres_2;
	m_iBrightHystSecureThres_2 = data.m_iBrightHystSecureThres_2;
	m_iBrightHystPotentialThres_2 = data.m_iBrightHystPotentialThres_2;
	m_iBrightHystPotentialLength_2 = data.m_iBrightHystPotentialLength_2;

	m_iDualBlobProcessType = data.m_iDualBlobProcessType;
	m_bDualBlobDefectConditionEach = data.m_bDualBlobDefectConditionEach;

	m_bUseNeckEpoxyMeasure = data.m_bUseNeckEpoxyMeasure;
	m_iNeckEpoxyMeasureTopEdgeStr = data.m_iNeckEpoxyMeasureTopEdgeStr;
	m_iNeckEpoxyMeasureBottomEdgeStr = data.m_iNeckEpoxyMeasureBottomEdgeStr;
	m_iNeckEpoxyMeasureRightEdgeStr = data.m_iNeckEpoxyMeasureRightEdgeStr;
	m_iNeckEpoxyToleranceHeightMax = data.m_iNeckEpoxyToleranceHeightMax;
	m_iNeckEpoxyToleranceHeightMin = data.m_iNeckEpoxyToleranceHeightMin;
	m_iNeckEpoxyToleranceLength = data.m_iNeckEpoxyToleranceLength;
	m_iNeckEpoxyToleranceTopPosOffset = data.m_iNeckEpoxyToleranceTopPosOffset;
	m_iNeckEpoxyToleranceBottomPosOffset = data.m_iNeckEpoxyToleranceBottomPosOffset;
	m_iNeckEpoxyInspectMethod = data.m_iNeckEpoxyInspectMethod;

	m_bUseNeckEpoxyCrack = data.m_bUseNeckEpoxyCrack;
	m_iNeckEpoxyCrackOpeningSize = data.m_iNeckEpoxyCrackOpeningSize;
	m_bNeckEpoxyCrackUseXDirMode = data.m_bNeckEpoxyCrackUseXDirMode; //VER 1.1.3.0 add

	m_iNeckEpoxyCrackYLength = data.m_iNeckEpoxyCrackYLength;
	m_iNeckEpoxyCrackBoundaryWidth = data.m_iNeckEpoxyCrackBoundaryWidth;
	m_iNeckEpoxyCrackDir = data.m_iNeckEpoxyCrackDir;

	m_iDnnInspectionType = data.m_iDnnInspectionType;
	m_iDnnInspectionModel = data.m_iDnnInspectionModel;

	m_bUseCornerMeasure = data.m_bUseCornerMeasure;
	m_iCornerMeasureSaveID = data.m_iCornerMeasureSaveID;
	m_iCornerMeasureMultiPointNumber = data.m_iCornerMeasureMultiPointNumber;
	m_iCornerMeasurePositionRange = data.m_iCornerMeasurePositionRange;

	for (int i = 0; i < 2; i++)
	{
		m_iCornerMeasureGv[i] = data.m_iCornerMeasureGv[i];
		m_iCornerMeasurePos[i] = data.m_iCornerMeasurePos[i];
		m_dCornerMeasureSmFactor[i] = data.m_dCornerMeasureSmFactor[i];
		m_iCornerMeasureEdgeStr[i] = data.m_iCornerMeasureEdgeStr[i];
		m_iCornerMeasurePosOffset[i] = data.m_iCornerMeasurePosOffset[i];
	}

	m_dCornerMeasureTargetSmFactor = data.m_dCornerMeasureTargetSmFactor;
	m_iCornerMeasureTargetEdgeStr = data.m_iCornerMeasureTargetEdgeStr;
	m_dCornerMeasureTargetDistanceSpec = data.m_dCornerMeasureTargetDistanceSpec;

	m_bUseComponent = data.m_bUseComponent;
	m_dComponentScore = data.m_dComponentScore;
	m_dComponentShiftX = data.m_dComponentShiftX;
	m_dComponentShiftY = data.m_dComponentShiftY;
	m_dComponentRotationAngle = data.m_dComponentRotationAngle;
	m_bComponentApply_1 = data.m_bComponentApply_1;
	m_bComponentApply_2 = data.m_bComponentApply_2;
	m_bComponentApply_3 = data.m_bComponentApply_3;
	m_bComponentApply_4 = data.m_bComponentApply_4;
	m_iComponentDefectType1 = data.m_iComponentDefectType1;
	m_iComponentDefectType2 = data.m_iComponentDefectType2;
	m_iComponentDefectType3 = data.m_iComponentDefectType3;
	m_iComponentDefectType4 = data.m_iComponentDefectType4;

	m_dComponentRotationAngle2 = data.m_dComponentRotationAngle2;
	m_bComponentApply_5 = data.m_bComponentApply_5;
	m_iComponentDefectType5 = data.m_iComponentDefectType5;

	m_bUseHSV = data.m_bUseHSV;
	miLowerHue = data.miLowerHue;
	miUpperHue = data.miUpperHue;
	miLowerSaturation = data.miLowerSaturation;
	miUpperSaturation = data.miUpperSaturation;

	m_bUseHeightMeasure = data.m_bUseHeightMeasure;
	m_iHeightMeasureType = data.m_iHeightMeasureType;

	m_bUseBoundaryInspection = data.m_bUseBoundaryInspection;
	m_iBoundaryEdgeMeasureDir = data.m_iBoundaryEdgeMeasureDir;
	m_iBoundaryEdgeMeasureGv = data.m_iBoundaryEdgeMeasureGv;
	m_iBoundaryEdgeMeasurePos = data.m_iBoundaryEdgeMeasurePos;
	m_dBoundaryEdgeMeasureSmFactor = data.m_dBoundaryEdgeMeasureSmFactor;
	m_dBoundaryEdgeMeasureEdgeStr = data.m_dBoundaryEdgeMeasureEdgeStr;
	m_iBoundaryEdgeMeasureSampling = data.m_iBoundaryEdgeMeasureSampling;
	m_iBoundaryEdgeMeasureSensorLength = data.m_iBoundaryEdgeMeasureSensorLength;
	m_iBoundaryDisplay = data.m_iBoundaryDisplay;

	for (int i = 0; i < 2; i++)
	{
		m_bBoundaryCondition[i] = data.m_bBoundaryCondition[i];
		m_iBoundaryMedianFilterSize[i] = data.m_iBoundaryMedianFilterSize[i];
		m_dBoundaryDefectDepth[i] = data.m_dBoundaryDefectDepth[i];
		m_dBoundaryDefectMBDepth[i] = data.m_dBoundaryDefectMBDepth[i];
		m_dBoundaryDefectLength[i] = data.m_dBoundaryDefectLength[i];
		m_bBoundaryUseConnection[i] = data.m_bBoundaryUseConnection[i];
		m_iBoundaryConnectionLength[i] = data.m_iBoundaryConnectionLength[i];
	}

	m_bBoundaryCondition[2] = data.m_bBoundaryCondition[2];
	m_dBoundaryDefectDepth[2] = data.m_dBoundaryDefectDepth[2];
	m_dBoundaryDefectLength[2] = data.m_dBoundaryDefectLength[2];

	m_bBoundaryUseRoiConnected = data.m_bBoundaryUseRoiConnected;

	return *this;
}

void CAlgorithmParam::Save(HANDLE hFile, int iVisionIdx, int iImageIdx, int iROINo, int iInspectionTabIdx)
{
	DWORD dwBytesWritten;

	WriteFile(hFile, &m_bInspect, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseFilter1, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseFilter2, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseFilter3, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseFilter4, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bCheckAIUseConsiderImageNo, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bCheckAIUseConsiderDefectCode, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDefectType, sizeof(int), &dwBytesWritten, NULL);

	for (int i = 0; i < 3; i++)
	{
		WriteFile(hFile, &(m_bUseInspectArea[i]), sizeof(BOOL), &dwBytesWritten, NULL);
		WriteFile(hFile, &(m_iInspectAreaContour1Margin[i]), sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &(m_iInspectAreaContour2Margin[i]), sizeof(int), &dwBytesWritten, NULL);
	}

	WriteFile(hFile, &m_bUseSurfaceInspection, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseSurfaceMeasureSaveLog, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseSurfaceInspectEachROI, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iSurfaceInspectEachROINgNumber, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bApplyCircleFitting, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iCircleFittingEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iCircleFittingEdgeMargin, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDTPolarTrans, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseFTConnected, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseFTConnectedArea, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseFTConnectedLength, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseFTConnectedWidth, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFTConnectedAreaMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFTConnectedLengthMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFTConnectedWidthMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBarcodeBlob, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBlobCornerMeasure, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseSaveBNEOverflowTop, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseSaveBNEOverflowBottom, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseBrightFixedThres, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightLowerThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUpperThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightDT, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightMedianFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightClosingFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterSize2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTValue, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightUniformityCheck, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityPeakMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityPeakMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityHystLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityHystOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightHystThres, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystSecureThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystPotentialThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystPotentialLength, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDarkFixedThres, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkLowerThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUpperThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDarkDT, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkMedianFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkClosingFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkDTFilterType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkDTFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkDTFilterSize2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkDTValue, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDarkUniformityCheck, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityPeakMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityPeakMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityHystLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityHystOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDarkHystThres, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkHystSecureThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkHystPotentialThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkHystPotentialLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightBinThres, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDarkBinThres, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iCandidateMinArea, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bCombineBrightDarkBlob, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bMaxBlob, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bClosingOpening, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bApplyOneDirMorp, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iOneDirMorpXOpening, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iOneDirMorpXClosing, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iOpeningSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iClosingSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iThresholdOpeningSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iThresholdClosingSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bFillup, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseConvex, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseSubConvex, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseRect, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseConnection, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionMinSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionMinXLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseAngle, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionMaxWidth, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iHoleFillCondition, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFillHoleArea, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bXYLengthMeasureIgnoreHole, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bXYLengthMaxLineLength, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryConnectionAny, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryConnectionOuter, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginAny, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginMinConnectionAny, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDefectConditionArea, sizeof(BOOL), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.bUseDefectConditionArea[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_bUseDefectConditionArea;
	WriteFile(hFile, &m_bUseDefectConditionLength, sizeof(BOOL), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.bUseDefectConditionLength[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_bUseDefectConditionLength;
	WriteFile(hFile, &m_bUseDefectConditionWidth, sizeof(BOOL), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.bUseDefectConditionWidth[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_bUseDefectConditionWidth;
	WriteFile(hFile, &m_bUseDefectConditionXLength, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionYLength, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionMean, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionStdev, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionAnisometry, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionCircularity, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionRectangularity, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionConvexity, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionEllipseRatio, sizeof(BOOL), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.bUseDefectConditionEllipseRatio[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_bUseDefectConditionEllipseRatio;
	WriteFile(hFile, &m_bUseDefectConditionAreaRatio, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionContrast, sizeof(BOOL), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.bUseDefectConditionContrast[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_bUseDefectConditionContrast;
	WriteFile(hFile, &m_bUseDefectConditionGrad, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionBlob, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionShiftX, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionShiftY, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionOuterDist, sizeof(BOOL), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.bUseDefectConditionOuterDist[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_bUseDefectConditionOuterDist;
	WriteFile(hFile, &m_bUseDefectConditionInnerDist, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionUseZeroArea, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryConnection, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryMarginTop, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryMarginBottom, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryMarginLeft, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryMarginRight, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryMarginHCenter, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryMarginVCenter, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDefectConditionAreaMin, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.iDefectConditionAreaMin[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iDefectConditionAreaMin;
	WriteFile(hFile, &m_iDefectConditionLengthMin, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.iDefectConditionLengthMin[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iDefectConditionLengthMin;
	WriteFile(hFile, &m_iDefectConditionWidthMin, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.iDefectConditionWidthMin[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iDefectConditionWidthMin;
	WriteFile(hFile, &m_iDefectConditionXLengthMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionYLengthMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionMeanMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionStdevMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionAnisometryMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionCircularityMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionRectangularityMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionConvexityMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionEllipseRatioMin, sizeof(double), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.dDefectConditionEllipseRatioMin[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_dDefectConditionEllipseRatioMin;
	WriteFile(hFile, &m_iDefectConditionAreaRatioMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionContrastMin, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.iDefectConditionContrastMin[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iDefectConditionContrastMin;
	WriteFile(hFile, &m_iDefectConditionGradMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBlobMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionShiftXMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionShiftYMin, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDefectConditionAreaMax, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.iDefectConditionAreaMax[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iDefectConditionAreaMax;
	WriteFile(hFile, &m_iDefectConditionLengthMax, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.iDefectConditionLengthMax[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iDefectConditionLengthMax;
	WriteFile(hFile, &m_iDefectConditionWidthMax, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.iDefectConditionWidthMax[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iDefectConditionWidthMax;
	WriteFile(hFile, &m_iDefectConditionXLengthMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionYLengthMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionMeanMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionStdevMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionAnisometryMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionCircularityMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionRectangularityMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionConvexityMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionEllipseRatioMax, sizeof(double), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.dDefectConditionEllipseRatioMax[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_dDefectConditionEllipseRatioMax;
	WriteFile(hFile, &m_iDefectConditionAreaRatioMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionContrastMax, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.iDefectConditionContrastMax[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iDefectConditionContrastMax;
	WriteFile(hFile, &m_iDefectConditionGradMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBlobMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionShiftXMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionShiftYMax, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDefectConditionOuterDist, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructDefectCondition.iDefectConditionOuterDist[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iDefectConditionOuterDist;
	WriteFile(hFile, &m_iDefectConditionInnerDist, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDefectConditionBoundaryMarginTop, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginBottom, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginLeft, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginRight, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginHCenter, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginVCenter, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginMinConnection, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseImageScaling, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageScaleMethod, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageScaleMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageScaleMax, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseEdgeImage, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeFilterType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dEdgeImageScale, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUsePeak, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNormalPeakThresMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNormalPeakThresMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iAbnormalPeakHistogramOffset, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iLengthWidthCondition, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iXYLengthCondition, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bDefectConditionBoundaryConnection2Side, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBoundaryConnectionCondition, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionInnerOuterBoundaryConnection, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iXLengthCondition, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iXLengthConditionMinRef, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iYLengthCondition, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iYLengthConditionMinRef, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDefectConditionHole, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionHoleArea, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bDefectConditionUnionBlob, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionDilation, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionDilationSize, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDefectConditionXLengthSum, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionXLengthSumMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionXLengthSumMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionYLengthSum, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionYLengthSumMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionYLengthSumMax, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bDefectConditionOrthoLength, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iOrthoLengthRefSide, sizeof(int), &dwBytesWritten, NULL);

	// Line Fitting

	WriteFile(hFile, &m_bUseLineFitting, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitObjectLowerThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitObjectUpperThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bLineFitUseOtherImageFitting, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitOtherImageNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dLineFitFittingEdgeSF, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dLineFitObjectEdgeSF, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitMinContourLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitContourConnectionLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bLineFitUseBurInspection, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dLineFitBurDefectLength, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dLineFitBurDefectLengthMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bLineFitUseChippingInspection, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dLineFitChippingDefectLength, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dLineFitChippingDefectLengthMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bLineFitUseRotationInspection, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitRotationReference, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dLineFitRotationDefectAngle, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitEdgeMethod, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitFittingEdgeHigh, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitFittingContourConnectionLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitFittingMinContourLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitEdgeHigh, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bLineFitLengthSave, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bNoUseLineFit, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bLineFitLocalAlignUse, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUsePinLength, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPinPos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPinDTFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPinOpeningClosingSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPinMinArea, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPinDistanceTolerance, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPinDTValue, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPinDefectNumberTolerance, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPinPitch, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDent, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDentMedFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDentDtFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDentBrightDtValue, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDentDarkDtValue, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDentCandidateMinArea, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDentCandidateMinWidth, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDentCandidateMinDistance, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDentMinArea, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseImageCompare, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bImageCompareSaveFeatureLog, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageCompareWindow, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageCompareWindowSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageCompareEdgeImage, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageCompareNormalizer, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageCompareNormalizeWeight, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseImageCompareBright, sizeof(BOOL), &dwBytesWritten, NULL);
	THEAPP.m_StructVariationParameter.bUseImageCompareBright[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_bUseImageCompareBright;
	WriteFile(hFile, &m_bUseImageCompareDark, sizeof(BOOL), &dwBytesWritten, NULL);
	THEAPP.m_StructVariationParameter.bUseImageCompareDark[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_bUseImageCompareDark;
	WriteFile(hFile, &m_iImageCompareBrightAbs, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructVariationParameter.iImageCompareBrightAbs[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iImageCompareBrightAbs;
	WriteFile(hFile, &m_iImageCompareDarkAbs, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructVariationParameter.iImageCompareDarkAbs[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iImageCompareDarkAbs;
	WriteFile(hFile, &m_dImageCompareBrightVar, sizeof(double), &dwBytesWritten, NULL);
	THEAPP.m_StructVariationParameter.dImageCompareBrightVar[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_dImageCompareBrightVar;
	WriteFile(hFile, &m_dImageCompareDarkVar, sizeof(double), &dwBytesWritten, NULL);
	THEAPP.m_StructVariationParameter.dImageCompareDarkVar[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_dImageCompareDarkVar;
	WriteFile(hFile, &m_bUseImageCompareAutoTrain, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageCompareMod, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageCompareModBright, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageCompareModDark, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageCompareModBrightAbs, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageCompareModDarkAbs, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dImageCompareModBrightVar, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dImageCompareModDarkVar, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageCompareModTop, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageCompareModBottom, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageCompareModLeft, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageCompareModRight, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUsePartCheck, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckMultipleModel, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMatchingAngleRange, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMatchingPLevel, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMatchingScore, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMatchingSearchMarginX, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMatchingSearchMarginY, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMatchingSearchMarginX2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMatchingSearchMarginY2, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bPartCheckInspectExistence, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckBlob, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckInspectShift, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckShiftX, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckShiftY, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dPartCheckRotationAngle, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckLocalAlignUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckApplyComp, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckSaveLog, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckMeasureEdge, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckLegXSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckLegYSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dPartCheckEdgeMeasueSmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dPartCheckEdgeMeasueEdgeStr, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckInspectLegDamage, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckMultiModuleInspect, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMultiModuleXNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMultiModuleYNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMultiModuleXPitch, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPartCheckMultiModuleYPitch, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckPickerPosUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPartCheckMultiModuleMoveOffset, sizeof(BOOL), &dwBytesWritten, NULL);

	for (int i = 0; i < ALIGN_MODULE_Y; i++)
	{
		for (int j = 0; j < ALIGN_MODULE_X; j++)
		{
			WriteFile(hFile, &(m_iPartCheckMoveOffsetX[i][j]), sizeof(int), &dwBytesWritten, NULL);
			WriteFile(hFile, &(m_iPartCheckMoveOffsetY[i][j]), sizeof(int), &dwBytesWritten, NULL);
		}
	}

	WriteFile(hFile, &m_iPartCheckMultiModuleTarget, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseConcenter, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConcenterMatchingScore, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConcenterInnerCircleDiameter, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dConcenterMatchingScaleMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dConcenterMatchingScaleMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConcenterMatchingMinContrast, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConcenterCenterDiffSpec, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dConcenterCenterDiffSpecX, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dConcenterCenterDiffSpecY, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseHomerMark, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iHomerMarkPos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iHomerMarkOrder, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseROIAlign, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignXSearchMargin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignYSearchMargin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignMatchingAngleRange, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAlignMatchingScaleMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAlignMatchingScaleMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignMatchingMinContrast, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAlignUsePosX, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAlignUsePosY, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAlignMatchingScore, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAlignInspectShift, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignMatchingPLevel, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAlignShiftTop, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAlignShiftBottom, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAlignShiftLeft, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAlignShiftRight, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAlignLocalAlignUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseIsoColorImage, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAlignMultiModuleInspect, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignMultiModuleXNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignMultiModuleYNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignMultiModuleXPitch, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignMultiModuleYPitch, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignMultiModuleAlignTarget, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAlignMultiModuleVision, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iProcessChType, sizeof(int), &dwBytesWritten, NULL);
	THEAPP.m_StructVariationParameter.iProcessChType[iVisionIdx][iImageIdx][iROINo][iInspectionTabIdx] = m_iProcessChType;
	WriteFile(hFile, &m_iProcessResType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageProcess, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessArithmeticType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessArithmeticImage1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessArithmeticImage2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessArithmeticImage3, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessArithmeticImage4, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dImageProcessArithmeticMulti, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessArithmeticAdd, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageProcessFilter, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType3, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType1X, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType2X, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType3X, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType1Y, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType2Y, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType3Y, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseImageProcessLocalAlign, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessLocalAlignImageIndex, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessLocalAlignROINo, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bImageProcessLocalAlignPosX, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bImageProcessLocalAlignPosY, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bImageProcessLocalAlignAngle, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessLocalAlignFitPos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessLocalAlignROIType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessLocalAlignMatchingApplyMethod, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageProcessDontCare, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessDontCareImageIndex, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessDontCareROINo, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessDontCareMargin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessDontCareMarginInner, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageProcessDontCare2, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessDontCareImageIndex2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessDontCareROINo2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessDontCareMargin2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessDontCareMargin2Inner, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageProcessGenerate, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageProcessGenerateInspectFlag, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessGenerateImageIndex, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessGenerateROINo, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessGenerateMargin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageProcessMask, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessMaskImageIndex, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessMaskROINo, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageProcessMask2, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessMaskImageIndex2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessMaskROINo2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseImageProcessLocalAlignAddLineFit, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessLocalAlignImageIndex_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessLocalAlignROINo_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessLocalAlignFitPos_2, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseBarcode, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dBarcodeShiftToleranceX, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dBarcodeShiftToleranceY, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bInspectBarcodeShift, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bInspectBarcodeRotation, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBarcodeOtherImage, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBarcodeOtherImageNumber, sizeof(int), &dwBytesWritten, NULL);

	//2017.04.11 hbk
	WriteFile(hFile, &m_bUseOCR, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_lCharPlateWidth, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_lMinCharWidth, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_lMaxCharWidth, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_lMinCharHeight, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_lMaxCharHeight, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_lMinCharCandidateHeight, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_lWholeAreaDivide, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_lHorizontalDivide, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_lFirstDiffernt, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_lSecondDifferent, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_ldiff, sizeof(long), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iOCRSegmentDilation, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseShape, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iShapeIlluminationFilterX, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iShapeIlluminationFilterY, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dShapeIlluminationScaleFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dShapeCurvatureSmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dShapeScalingMulti, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dShapeScalingAdd, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dShapeImageReduceRatio, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dShapeLedAngle, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iShapeImageMargin, sizeof(int), &dwBytesWritten, NULL);

	// Don't Care
	WriteFile(hFile, &m_iProcessChTypeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightFixedThresDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightLowerThresDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUpperThresDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightDTDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightMedianFilterSizeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightClosingFilterSizeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterTypeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterSizeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterSizeDC2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTValueDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightUniformityCheckDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityPeakMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityPeakMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityOffsetDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityHystLengthDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityHystOffsetDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDarkFixedThresDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkLowerThresDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUpperThresDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDarkDTDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkMedianFilterSizeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkClosingFilterSizeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkDTFilterTypeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkDTFilterSizeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkDTFilterSizeDC2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkDTValueDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDarkUniformityCheckDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityPeakMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityPeakMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityOffsetDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityHystLengthDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityHystOffsetDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iCandidateMinAreaDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bCombineBrightDarkBlobDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bMaxBlobDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bClosingOpeningDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iOpeningSizeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iClosingSizeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bFillupDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseConvexDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseConnectionDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionMinSizeDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionMinXLengthDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionLengthDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseAngleDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionMaxWidthDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionAreaDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionLengthDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionWidthDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionXLengthDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionYLengthDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionMeanDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionStdevDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionAnisometryDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionCircularityDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionRectangularityDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionConvexityDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionEllipseRatioDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionAreaRatioDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionContrastDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionBlobDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionShiftXDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionShiftYDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionOuterDistDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionInnerDistDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionUseZeroAreaDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryConnectionDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryMarginTopDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryMarginBottomDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryMarginLeftDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryMarginRightDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionAreaMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionLengthMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionWidthMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionXLengthMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionYLengthMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionMeanMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionStdevMinDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionAnisometryMinDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionCircularityMinDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionRectangularityMinDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionConvexityMinDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionEllipseRatioMinDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionAreaRatioMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionContrastMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBlobMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionShiftXMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionShiftYMinDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionAreaMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionLengthMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionWidthMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionXLengthMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionYLengthMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionMeanMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionStdevMaxDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionAnisometryMaxDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionCircularityMaxDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionRectangularityMaxDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionConvexityMaxDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionEllipseRatioMaxDC, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionAreaRatioMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionContrastMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBlobMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionShiftXMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionShiftYMaxDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionOuterDistDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionInnerDistDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginTopDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginBottomDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginLeftDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginRightDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionBoundaryMarginMinConnectionDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLengthWidthConditionDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iXYLengthConditionDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionBoundaryConnection2SideDC, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBoundaryConnectionConditionDC, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDontcareRegionMargin, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseEdgeMeasure, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureOnePoint, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureMultiPointNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEdgeMeasureEndPoint, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEdgeMeasureSlope, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEdgeMeasureReverseGv, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEdgeMeasureFixedEndPoint, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureLeftSideSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureRightSideSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEdgeMeasureUseMaxMinPoint, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureUseMaxPoint, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasurePositionRange, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureDir, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureGv, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasurePos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEdgeMeasureLocalAlignUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dEdgeMeasureSmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasurePosOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEdgeMeasureUseOther, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEdgeMeasureMakeROIOther, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureMakeROIRangeOther, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureImageIndexOther, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureGvOther, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasurePosOther, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dEdgeMeasureSmFactorOther, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureEdgeStrOther, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasurePosOffsetOther, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dEdgeMeasureAngleDeg, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureAccuracy, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseHomerTilt, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iHomerTiltHomerEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iHomerTiltBenvolioImage, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iHomerTiltBenvolioEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dHomerTiltHomerLength, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dHomerTiltTol, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dHomerTiltDistanceTol, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseEpoxyVoidHole, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEpoxyVoidHoleImage, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dEpoxyVoidHoleEdgeStr, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEpoxyVoidHoleInspectMargin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEpoxyVoidHoleTiltInspect, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEpoxyVoidHoleTiltTolerance, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEpoxyVoidHoleGapInspect, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEpoxyVoidHoleGapTolerance, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUsePrintQuality, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityContrastMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualitySymbolColsMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualitySymbolRowsMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualitySymbolColsMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualitySymbolRowsMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityModuleSizeMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityModuleSizeMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dPrintQualitySlantMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPrintQualityOverallQuality, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityGradeOverallQuality, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bPrintQualityUnusedErrorCorrection, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityGradeUnusedErrorCorrection, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityStrictModel, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityPolarity, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityMirrored, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityContrastTolerance, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityStrictQuietZone, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualitySymbolShape, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualitySmallModulesRobustness, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityModuleGrid, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityModuleGapMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityModuleGapMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iPrintQualityFinderPatternTolerance, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseROIAnisoAlign, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignXSearchMargin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignYSearchMargin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMatchingAngleRange, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignMatchingScaleMinY, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignMatchingScaleMaxY, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignMatchingScaleMinX, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignMatchingScaleMaxX, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMatchingMinContrast, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAnisoAlignUsePosX, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAnisoAlignUsePosY, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignMatchingScore, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAnisoAlignInspectShift, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMatchingPLevel, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignShiftTop, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignShiftBottom, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignShiftLeft, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignShiftRight, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAnisoAlignLocalAlignUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseAnisoColorImage, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAnisoAlignMultiModuleInspect, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMultiModuleXNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMultiModuleYNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMultiModuleXPitch, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMultiModuleYPitch, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMultiModuleAlignTarget, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMultiModuleVision, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseStiffenerEpoxy, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iThresholdParam_StiffenerRegion, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDTFilterX_StiffenerRegion, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDTFilterY_StiffenerRegion, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDTValue_StiffenerRegion, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iClosing_StiffenerRegion, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dErosionParam_StiffenerBoundary, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iThresholdParam_SheildcanRegion, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDilationParam_SheildcanBoundary, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dOpeningParam_CornerBoundary, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDistanceMeasure, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureUpperGv, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureUpperPos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDistanceMeasureUpperSmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureUpperEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureLowerGv, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureLowerPos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDistanceMeasureLowerSmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureLowerEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureUpperPosOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureLowerPosOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureUsageType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureSaveID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDistanceMeasureGapInspectionUpper, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDistanceMeasureGapInspectionLower, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDistanceMeasureROIType, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseGapMeasure, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureUpperGv, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureUpperPos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dGapMeasureUpperSmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureUpperEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureLowerGv, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureLowerPos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dGapMeasureLowerSmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureLowerEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureUpperPosOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureLowerPosOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureSaveID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureImageNo1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureImageNo2, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_dGapMeasureGapInspectionUpper, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dGapMeasureGapInspectionLower, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dGapMeasureGapMeasureOffset, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureDir, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bGapMeasureAlarm, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasurePointNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureSelectType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bGapMeasureUpperLine, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bGapMeasureLowerLine, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bGapMeasureDiffImage, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasurePosStart, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasurePosEnd, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureImageNo3, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bGapMeasureEndRemove, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureRemoveEndUpper, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureRemoveEndLower, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bGapMeasureRetry, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureRetryImageNo, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureRetryGv, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureRetryPos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dGapMeasureRetrySmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureRetryEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iGapMeasureRetryPosOffset, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDefectConditionXUnder, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionYUnder, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseSurfaceDualInspection, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iSurfaceDualImageIndex_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iSurfaceDualImageIndex_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iSurfaceDualDefectType_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iSurfaceDualDefectType_2, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseBrightFixedThres_1, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightLowerThres_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUpperThres_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightDT_1, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightMedianFilterSize_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightClosingFilterSize_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterType_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterSize_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterSize2_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTValue_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDTPolarTrans_1, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightHystThres_1, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystSecureThres_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystPotentialThres_1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystPotentialLength_1, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseBrightFixedThres_2, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightLowerThres_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUpperThres_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightDT_2, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightMedianFilterSize_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightClosingFilterSize_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterType_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterSize_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterSize2_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTValue_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDTPolarTrans_2, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightHystThres_2, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystSecureThres_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystPotentialThres_2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystPotentialLength_2, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDualBlobProcessType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDualBlobDefectConditionEach, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseNeckEpoxyMeasure, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyMeasureTopEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyMeasureBottomEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyMeasureRightEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyToleranceHeightMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyToleranceHeightMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyToleranceLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyToleranceTopPosOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyToleranceBottomPosOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyInspectMethod, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseNeckEpoxyCrack, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyCrackOpeningSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyCrackYLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyCrackBoundaryWidth, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iNeckEpoxyCrackDir, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDnnInspectionType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDnnInspectionModel, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseCornerMeasure, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iCornerMeasureSaveID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iCornerMeasureMultiPointNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iCornerMeasurePositionRange, sizeof(int), &dwBytesWritten, NULL);

	for (int i = 0; i < 2; i++)
	{
		WriteFile(hFile, &(m_iCornerMeasureGv[i]), sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &(m_iCornerMeasurePos[i]), sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &(m_dCornerMeasureSmFactor[i]), sizeof(double), &dwBytesWritten, NULL);
		WriteFile(hFile, &(m_iCornerMeasureEdgeStr[i]), sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &(m_iCornerMeasurePosOffset[i]), sizeof(int), &dwBytesWritten, NULL);
	}

	WriteFile(hFile, &m_dCornerMeasureTargetSmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iCornerMeasureTargetEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dCornerMeasureTargetDistanceSpec, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseComponent, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dComponentScore, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dComponentShiftX, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dComponentShiftY, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dComponentRotationAngle, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bComponentApply_1, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bComponentApply_2, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bComponentApply_3, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bComponentApply_4, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iComponentDefectType1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iComponentDefectType2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iComponentDefectType3, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iComponentDefectType4, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bNeckEpoxyCrackUseXDirMode, sizeof(BOOL), &dwBytesWritten, NULL); //VER 1.1.3.0 add

	//VER 1.1.3.2 add
	WriteFile(hFile, &m_bDefectConditionOuterDiff, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_nDefectConditionOuterDiffInterval, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_nDefectConditionOuterDiffMinDiff, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_nOuterDiffUpper, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_nDefectConditionOuterDiffMaxRepeat, sizeof(int), &dwBytesWritten, NULL);
	//VER 1.1.3.2 end

	//VER 1.1.3.3 add Calculate Blob Length From ConerLength
	WriteFile(hFile, &m_bDefectConditionConerLengthUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_nDefectConditionConerLengthDir, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_nDefectConditionConerLengthMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_nDefectConditionConerLengthMax, sizeof(int), &dwBytesWritten, NULL);
	//VER 1.1.3.3 end

	//VER 1.1.3.4
	WriteFile(hFile, &m_bDefectConditionConerLengthMaxUse, sizeof(BOOL), &dwBytesWritten, NULL);
	//VER 1.1.3.4 end

	//VER 1.1.4.3
	WriteFile(hFile, &m_bUseImageProcessPostLocalAlign, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessPostLocalAlignImageIndex, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessPostLocalAlignROINo, sizeof(int), &dwBytesWritten, NULL);
	//VER 1.1.4.3 end

	//VER 1.1.4.4
	WriteFile(hFile, &m_iImageProcessPostLocalAlignFitPos, sizeof(int), &dwBytesWritten, NULL);
	//VER 1.1.4.4 end


	WriteFile(hFile, &m_bComponentApply_5, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iComponentDefectType5, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dComponentRotationAngle2, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseHSV, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &miLowerHue, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miUpperHue, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miLowerSaturation, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miUpperSaturation, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseHeightMeasure, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iHeightMeasureType, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseBoundaryInspection, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBoundaryEdgeMeasureDir, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBoundaryEdgeMeasureGv, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBoundaryEdgeMeasurePos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dBoundaryEdgeMeasureSmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dBoundaryEdgeMeasureEdgeStr, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBoundaryEdgeMeasureSampling, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBoundaryEdgeMeasureSensorLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBoundaryDisplay, sizeof(int), &dwBytesWritten, NULL);

	for (int i = 0; i < 2; i++)
	{
		WriteFile(hFile, &m_bBoundaryCondition[i], sizeof(BOOL), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_iBoundaryMedianFilterSize[i], sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_dBoundaryDefectDepth[i], sizeof(double), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_dBoundaryDefectMBDepth[i], sizeof(double), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_dBoundaryDefectLength[i], sizeof(double), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_bBoundaryUseConnection[i], sizeof(BOOL), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_iBoundaryConnectionLength[i], sizeof(int), &dwBytesWritten, NULL);
	}

	WriteFile(hFile, &m_bBoundaryCondition[2], sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dBoundaryDefectDepth[2], sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dBoundaryDefectLength[2], sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bBoundaryUseRoiConnected, sizeof(BOOL), &dwBytesWritten, NULL);
}

BOOL CAlgorithmParam::Load(HANDLE hFile, int iModelVersion, BOOL bCreateReinforceModel)
{
	Reset();

	BOOL bTemp;
	double dTemp;
	int iTemp;

	DWORD dwBytesRead;

	ReadFile(hFile, &m_bInspect, sizeof(BOOL), &dwBytesRead, NULL);

	if (iModelVersion >= 2013)
	{
		ReadFile(hFile, &m_bUseFilter1, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bUseFilter2, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bUseFilter3, sizeof(BOOL), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 2018)
	{
		ReadFile(hFile, &m_bUseFilter4, sizeof(BOOL), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 2024) {
		ReadFile(hFile, &m_bCheckAIUseConsiderImageNo, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bCheckAIUseConsiderDefectCode, sizeof(BOOL), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_iDefectType, sizeof(int), &dwBytesRead, NULL);

	for (int i = 0; i < 3; i++)
	{
		ReadFile(hFile, &(m_bUseInspectArea[i]), sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &(m_iInspectAreaContour1Margin[i]), sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &(m_iInspectAreaContour2Margin[i]), sizeof(int), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_bUseSurfaceInspection, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseSurfaceMeasureSaveLog, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseSurfaceInspectEachROI, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iSurfaceInspectEachROINgNumber, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bApplyCircleFitting, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iCircleFittingEdgeStr, sizeof(int), &dwBytesRead, NULL);
	if (iModelVersion >= 1058)
		ReadFile(hFile, &m_iCircleFittingEdgeMargin, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDTPolarTrans, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseFTConnected, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseFTConnectedArea, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseFTConnectedLength, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseFTConnectedWidth, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iFTConnectedAreaMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iFTConnectedLengthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iFTConnectedWidthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBarcodeBlob, sizeof(BOOL), &dwBytesRead, NULL);

	if (iModelVersion >= 1014)
		ReadFile(hFile, &m_bUseBlobCornerMeasure, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseSaveBNEOverflowTop, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseSaveBNEOverflowBottom, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseBrightFixedThres, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightLowerThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUpperThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightDT, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightMedianFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightClosingFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterType, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterSize2, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iBrightDTValue, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightUniformityCheck, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityPeakMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityPeakMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityHystLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityHystOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightHystThres, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystSecureThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystPotentialThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystPotentialLength, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDarkFixedThres, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkLowerThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUpperThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDarkDT, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkMedianFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkClosingFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkDTFilterType, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkDTFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkDTFilterSize2, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iDarkDTValue, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDarkUniformityCheck, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityPeakMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityPeakMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityHystLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityHystOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDarkHystThres, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkHystSecureThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkHystPotentialThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkHystPotentialLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightBinThres, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDarkBinThres, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iCandidateMinArea, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bCombineBrightDarkBlob, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bMaxBlob, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bClosingOpening, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bApplyOneDirMorp, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iOneDirMorpXOpening, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iOneDirMorpXClosing, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iOpeningSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iClosingSize, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iThresholdOpeningSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iThresholdClosingSize, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bFillup, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseConvex, sizeof(BOOL), &dwBytesRead, NULL);
	if (iModelVersion >= 1102)
		ReadFile(hFile, &m_bUseSubConvex, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseRect, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseConnection, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionMinSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionMinXLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseAngle, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionMaxWidth, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iHoleFillCondition, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iFillHoleArea, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bXYLengthMeasureIgnoreHole, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bXYLengthMaxLineLength, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bDefectConditionBoundaryConnectionAny, sizeof(BOOL), &dwBytesRead, NULL);
	if (iModelVersion >= 1093)
		ReadFile(hFile, &m_iDefectConditionBoundaryConnectionOuter, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginAny, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginMinConnectionAny, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDefectConditionArea, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionLength, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionWidth, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionXLength, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionYLength, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionMean, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionStdev, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionAnisometry, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionCircularity, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionRectangularity, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionConvexity, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionEllipseRatio, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionAreaRatio, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionContrast, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionGrad, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionBlob, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionShiftX, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionShiftY, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionOuterDist, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionInnerDist, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionUseZeroArea, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryConnection, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryMarginTop, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryMarginBottom, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryMarginLeft, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryMarginRight, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryMarginHCenter, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryMarginVCenter, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iDefectConditionAreaMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionLengthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionWidthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionXLengthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionYLengthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionMeanMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionStdevMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionAnisometryMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionCircularityMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionRectangularityMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionConvexityMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionEllipseRatioMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionAreaRatioMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionContrastMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionGradMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBlobMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionShiftXMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionShiftYMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionAreaMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionLengthMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionWidthMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionXLengthMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionYLengthMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionMeanMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionStdevMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionAnisometryMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionCircularityMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionRectangularityMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionConvexityMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionEllipseRatioMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionAreaRatioMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionContrastMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionGradMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBlobMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionShiftXMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionShiftYMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionOuterDist, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionInnerDist, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginTop, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginBottom, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginLeft, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginRight, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginHCenter, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginVCenter, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginMinConnection, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseImageScaling, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageScaleMethod, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageScaleMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageScaleMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseEdgeImage, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeFilterType, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dEdgeImageScale, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUsePeak, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNormalPeakThresMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNormalPeakThresMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iAbnormalPeakHistogramOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLengthWidthCondition, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iXYLengthCondition, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryConnection2Side, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBoundaryConnectionCondition, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionInnerOuterBoundaryConnection, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iXLengthCondition, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iXLengthConditionMinRef, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iYLengthCondition, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iYLengthConditionMinRef, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionHole, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionHoleArea, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bDefectConditionUnionBlob, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionDilation, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionDilationSize, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDefectConditionXLengthSum, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionXLengthSumMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionXLengthSumMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionYLengthSum, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionYLengthSumMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionYLengthSumMax, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bDefectConditionOrthoLength, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iOrthoLengthRefSide, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseLineFitting, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitObjectLowerThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitObjectUpperThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bLineFitUseOtherImageFitting, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitOtherImageNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dLineFitFittingEdgeSF, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dLineFitObjectEdgeSF, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitMinContourLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitContourConnectionLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bLineFitUseBurInspection, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dLineFitBurDefectLength, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dLineFitBurDefectLengthMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bLineFitUseChippingInspection, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dLineFitChippingDefectLength, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dLineFitChippingDefectLengthMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bLineFitUseRotationInspection, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitRotationReference, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dLineFitRotationDefectAngle, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitEdgeMethod, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitFittingEdgeHigh, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitFittingContourConnectionLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitFittingMinContourLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitEdgeHigh, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bLineFitLengthSave, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bNoUseLineFit, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bLineFitLocalAlignUse, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUsePinLength, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPinPos, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPinDTFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPinOpeningClosingSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPinMinArea, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPinDistanceTolerance, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPinDTValue, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPinDefectNumberTolerance, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPinPitch, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDent, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDentMedFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDentDtFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDentBrightDtValue, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDentDarkDtValue, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDentCandidateMinArea, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDentCandidateMinWidth, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDentCandidateMinDistance, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDentMinArea, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseImageCompare, sizeof(BOOL), &dwBytesRead, NULL);

	if (iModelVersion >= 1127)
		ReadFile(hFile, &m_bImageCompareSaveFeatureLog, sizeof(BOOL), &dwBytesRead, NULL);

	if (iModelVersion >= 2007)
	{
		ReadFile(hFile, &m_bUseImageCompareWindow, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageCompareWindowSize, sizeof(int), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 2025)
		ReadFile(hFile, &m_bUseImageCompareEdgeImage, sizeof(BOOL), &dwBytesRead, NULL);

	if (iModelVersion >= 2026)
	{
		ReadFile(hFile, &m_bUseImageCompareNormalizer, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageCompareNormalizeWeight, sizeof(int), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_bUseImageCompareBright, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseImageCompareDark, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageCompareBrightAbs, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageCompareDarkAbs, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dImageCompareBrightVar, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dImageCompareDarkVar, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseImageCompareAutoTrain, sizeof(BOOL), &dwBytesRead, NULL);
	if (iModelVersion >= 1001)
	{
		ReadFile(hFile, &m_bUseImageCompareMod, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bUseImageCompareModBright, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bUseImageCompareModDark, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageCompareModBrightAbs, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageCompareModDarkAbs, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dImageCompareModBrightVar, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dImageCompareModDarkVar, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageCompareModTop, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageCompareModBottom, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageCompareModLeft, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageCompareModRight, sizeof(int), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_bUsePartCheck, sizeof(BOOL), &dwBytesRead, NULL);
	if (iModelVersion >= 1123)
	{
		ReadFile(hFile, &m_bPartCheckMultipleModel, sizeof(BOOL), &dwBytesRead, NULL);
	}
	ReadFile(hFile, &m_iPartCheckMatchingAngleRange, sizeof(int), &dwBytesRead, NULL);
	if (iModelVersion >= 1123)
	{
		ReadFile(hFile, &m_iPartCheckMatchingPLevel, sizeof(int), &dwBytesRead, NULL);
	}
	ReadFile(hFile, &m_iPartCheckMatchingScore, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckMatchingSearchMarginX, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckMatchingSearchMarginY, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckMatchingSearchMarginX2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckMatchingSearchMarginY2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bPartCheckInspectExistence, sizeof(BOOL), &dwBytesRead, NULL);
	if (iModelVersion >= 1084)
	{
		ReadFile(hFile, &m_bPartCheckBlob, sizeof(BOOL), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_bPartCheckInspectShift, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckShiftX, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckShiftY, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dPartCheckRotationAngle, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bPartCheckLocalAlignUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bPartCheckApplyComp, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bPartCheckSaveLog, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bPartCheckMeasureEdge, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckLegXSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckLegYSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dPartCheckEdgeMeasueSmFactor, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dPartCheckEdgeMeasueEdgeStr, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bPartCheckInspectLegDamage, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bPartCheckMultiModuleInspect, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckMultiModuleXNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckMultiModuleYNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckMultiModuleXPitch, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPartCheckMultiModuleYPitch, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bPartCheckPickerPosUse, sizeof(BOOL), &dwBytesRead, NULL);

	if (iModelVersion >= 1079)
	{
		ReadFile(hFile, &m_bPartCheckMultiModuleMoveOffset, sizeof(BOOL), &dwBytesRead, NULL);

		for (int i = 0; i < ALIGN_MODULE_Y; i++)
		{
			if (iModelVersion < 2021 && i == 4)
				break;

			for (int j = 0; j < ALIGN_MODULE_X; j++)
			{
				ReadFile(hFile, &(m_iPartCheckMoveOffsetX[i][j]), sizeof(int), &dwBytesRead, NULL);
				ReadFile(hFile, &(m_iPartCheckMoveOffsetY[i][j]), sizeof(int), &dwBytesRead, NULL);
			}
		}
	}

	if (iModelVersion >= 1123)
	{
		ReadFile(hFile, &m_iPartCheckMultiModuleTarget, sizeof(int), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_bUseConcenter, sizeof(BOOL), &dwBytesRead, NULL);

	if (iModelVersion >= 1002)
	{
		ReadFile(hFile, &m_iConcenterMatchingScore, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iConcenterInnerCircleDiameter, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dConcenterMatchingScaleMin, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dConcenterMatchingScaleMax, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iConcenterMatchingMinContrast, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iConcenterCenterDiffSpec, sizeof(int), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 1020)
	{
		ReadFile(hFile, &m_dConcenterCenterDiffSpecX, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dConcenterCenterDiffSpecY, sizeof(double), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_bUseHomerMark, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iHomerMarkPos, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iHomerMarkOrder, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseROIAlign, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAlignXSearchMargin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAlignYSearchMargin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAlignMatchingAngleRange, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAlignMatchingScaleMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAlignMatchingScaleMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAlignMatchingMinContrast, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAlignUsePosX, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAlignUsePosY, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAlignMatchingScore, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAlignInspectShift, sizeof(BOOL), &dwBytesRead, NULL);
	if (iModelVersion >= 1123)
	{
		ReadFile(hFile, &m_iROIAlignMatchingPLevel, sizeof(int), &dwBytesRead, NULL);
	}
	ReadFile(hFile, &m_dROIAlignShiftTop, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAlignShiftBottom, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAlignShiftLeft, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAlignShiftRight, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAlignLocalAlignUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseIsoColorImage, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAlignMultiModuleInspect, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAlignMultiModuleXNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAlignMultiModuleYNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAlignMultiModuleXPitch, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAlignMultiModuleYPitch, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAlignMultiModuleAlignTarget, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAlignMultiModuleVision, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iProcessChType, sizeof(int), &dwBytesRead, NULL);
	if (m_iProcessChType > CHANNEL_TYPE_I)
		m_iProcessChType = CHANNEL_TYPE_I;

	if (iModelVersion >= 2002)
		ReadFile(hFile, &m_iProcessResType, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseImageProcess, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessArithmeticType, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessArithmeticImage1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessArithmeticImage2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessArithmeticImage3, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessArithmeticImage4, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dImageProcessArithmeticMulti, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessArithmeticAdd, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseImageProcessFilter, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessFilterType1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessFilterType2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessFilterType3, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessFilterType1X, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessFilterType2X, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessFilterType3X, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessFilterType1Y, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessFilterType2Y, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessFilterType3Y, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseImageProcessLocalAlign, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessLocalAlignImageIndex, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessLocalAlignROINo, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bImageProcessLocalAlignPosX, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bImageProcessLocalAlignPosY, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bImageProcessLocalAlignAngle, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iImageProcessLocalAlignFitPos, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessLocalAlignROIType, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessLocalAlignMatchingApplyMethod, sizeof(int), &dwBytesRead, NULL);
	if (m_iImageProcessLocalAlignMatchingApplyMethod == 2)	// v2.0.1.4 LOCAL_ALIGN_MATCHING_IMAGE_ALIGN 삭제
		m_iImageProcessLocalAlignMatchingApplyMethod = 0;

	if (iModelVersion >= 1001 && iModelVersion < 2014)	// v2.0.1.4 LOCAL_ALIGN_MATCHING_IMAGE_ALIGN 삭제(Crop Margin 제거)
		ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseImageProcessDontCare, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessDontCareImageIndex, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessDontCareROINo, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessDontCareMargin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessDontCareMarginInner, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseImageProcessDontCare2, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessDontCareImageIndex2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessDontCareROINo2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessDontCareMargin2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessDontCareMargin2Inner, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseImageProcessGenerate, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseImageProcessGenerateInspectFlag, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessGenerateImageIndex, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessGenerateROINo, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessGenerateMargin, sizeof(int), &dwBytesRead, NULL);

	if (iModelVersion >= 1107)
	{
		ReadFile(hFile, &m_bUseImageProcessMask, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessMaskImageIndex, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessMaskROINo, sizeof(int), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 1117)
	{
		ReadFile(hFile, &m_bUseImageProcessMask2, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessMaskImageIndex2, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessMaskROINo2, sizeof(int), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_bUseImageProcessLocalAlignAddLineFit, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessLocalAlignImageIndex_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessLocalAlignROINo_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessLocalAlignFitPos_2, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseBarcode, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dBarcodeShiftToleranceX, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dBarcodeShiftToleranceY, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bInspectBarcodeShift, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bInspectBarcodeRotation, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBarcodeOtherImage, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBarcodeOtherImageNumber, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseOCR, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_lCharPlateWidth, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_lMinCharWidth, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_lMaxCharWidth, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_lMinCharHeight, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_lMaxCharHeight, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_lMinCharCandidateHeight, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_lWholeAreaDivide, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_lHorizontalDivide, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_lFirstDiffernt, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_lSecondDifferent, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_ldiff, sizeof(long), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iOCRSegmentDilation, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseShape, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iShapeIlluminationFilterX, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iShapeIlluminationFilterY, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dShapeIlluminationScaleFactor, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dShapeCurvatureSmFactor, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dShapeScalingMulti, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dShapeScalingAdd, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dShapeImageReduceRatio, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dShapeLedAngle, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iShapeImageMargin, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iProcessChTypeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightFixedThresDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightLowerThresDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUpperThresDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightDTDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightMedianFilterSizeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightClosingFilterSizeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterTypeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterSizeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterSizeDC2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTValueDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightUniformityCheckDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityPeakMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityPeakMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityOffsetDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityHystLengthDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityHystOffsetDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDarkFixedThresDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkLowerThresDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUpperThresDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDarkDTDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkMedianFilterSizeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkClosingFilterSizeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkDTFilterTypeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkDTFilterSizeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkDTFilterSizeDC2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkDTValueDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDarkUniformityCheckDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityPeakMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityPeakMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityOffsetDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityHystLengthDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityHystOffsetDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iCandidateMinAreaDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bCombineBrightDarkBlobDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bMaxBlobDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bClosingOpeningDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iOpeningSizeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iClosingSizeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bFillupDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseConvexDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseConnectionDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionMinSizeDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionMinXLengthDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionLengthDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseAngleDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionMaxWidthDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionAreaDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionLengthDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionWidthDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionXLengthDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionYLengthDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionMeanDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionStdevDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionAnisometryDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionCircularityDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionRectangularityDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionConvexityDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionEllipseRatioDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionAreaRatioDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionContrastDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionBlobDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionShiftXDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionShiftYDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionOuterDistDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionInnerDistDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionUseZeroAreaDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryConnectionDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryMarginTopDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryMarginBottomDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryMarginLeftDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryMarginRightDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionAreaMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionLengthMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionWidthMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionXLengthMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionYLengthMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionMeanMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionStdevMinDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionAnisometryMinDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionCircularityMinDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionRectangularityMinDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionConvexityMinDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionEllipseRatioMinDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionAreaRatioMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionContrastMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBlobMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionShiftXMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionShiftYMinDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionAreaMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionLengthMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionWidthMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionXLengthMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionYLengthMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionMeanMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionStdevMaxDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionAnisometryMaxDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionCircularityMaxDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionRectangularityMaxDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionConvexityMaxDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionEllipseRatioMaxDC, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionAreaRatioMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionContrastMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBlobMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionShiftXMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionShiftYMaxDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionOuterDistDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionInnerDistDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginTopDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginBottomDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginLeftDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginRightDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionBoundaryMarginMinConnectionDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLengthWidthConditionDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iXYLengthConditionDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionBoundaryConnection2SideDC, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBoundaryConnectionConditionDC, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDontcareRegionMargin, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseEdgeMeasure, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureOnePoint, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureMultiPointNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bEdgeMeasureEndPoint, sizeof(BOOL), &dwBytesRead, NULL);
	if (iModelVersion >= 1004)
		ReadFile(hFile, &m_bEdgeMeasureSlope, sizeof(BOOL), &dwBytesRead, NULL);

	if (iModelVersion >= 1077)
		ReadFile(hFile, &m_bEdgeMeasureReverseGv, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bEdgeMeasureFixedEndPoint, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureLeftSideSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureRightSideSize, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bEdgeMeasureUseMaxMinPoint, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureUseMaxPoint, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iEdgeMeasurePositionRange, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iEdgeMeasureDir, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureGv, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasurePos, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bEdgeMeasureLocalAlignUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dEdgeMeasureSmFactor, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasurePosOffset, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bEdgeMeasureUseOther, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bEdgeMeasureMakeROIOther, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureMakeROIRangeOther, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureImageIndexOther, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureGvOther, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasurePosOther, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dEdgeMeasureSmFactorOther, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureEdgeStrOther, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasurePosOffsetOther, sizeof(int), &dwBytesRead, NULL);

	if (iModelVersion >= 1021)
		ReadFile(hFile, &m_dEdgeMeasureAngleDeg, sizeof(double), &dwBytesRead, NULL);

	if (iModelVersion >= 1024)
		ReadFile(hFile, &m_iEdgeMeasureAccuracy, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseHomerTilt, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iHomerTiltHomerEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iHomerTiltBenvolioImage, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iHomerTiltBenvolioEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dHomerTiltHomerLength, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dHomerTiltTol, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dHomerTiltDistanceTol, sizeof(double), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseEpoxyVoidHole, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEpoxyVoidHoleImage, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dEpoxyVoidHoleEdgeStr, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEpoxyVoidHoleInspectMargin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bEpoxyVoidHoleTiltInspect, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEpoxyVoidHoleTiltTolerance, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bEpoxyVoidHoleGapInspect, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEpoxyVoidHoleGapTolerance, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUsePrintQuality, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityContrastMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualitySymbolColsMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualitySymbolRowsMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualitySymbolColsMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualitySymbolRowsMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityModuleSizeMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityModuleSizeMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dPrintQualitySlantMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bPrintQualityOverallQuality, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityGradeOverallQuality, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bPrintQualityUnusedErrorCorrection, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityGradeUnusedErrorCorrection, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityStrictModel, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityPolarity, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityMirrored, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityContrastTolerance, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityStrictQuietZone, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualitySymbolShape, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualitySmallModulesRobustness, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityModuleGrid, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityModuleGapMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityModuleGapMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iPrintQualityFinderPatternTolerance, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseROIAnisoAlign, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignXSearchMargin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignYSearchMargin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignMatchingAngleRange, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignMatchingScaleMinY, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignMatchingScaleMaxY, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignMatchingScaleMinX, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignMatchingScaleMaxX, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignMatchingMinContrast, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAnisoAlignUsePosX, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAnisoAlignUsePosY, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignMatchingScore, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAnisoAlignInspectShift, sizeof(BOOL), &dwBytesRead, NULL);
	if (iModelVersion >= 1123)
	{
		ReadFile(hFile, &m_iROIAnisoAlignMatchingPLevel, sizeof(int), &dwBytesRead, NULL);
	}
	ReadFile(hFile, &m_dROIAnisoAlignShiftTop, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignShiftBottom, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignShiftLeft, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignShiftRight, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAnisoAlignLocalAlignUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseAnisoColorImage, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAnisoAlignMultiModuleInspect, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignMultiModuleXNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignMultiModuleYNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignMultiModuleXPitch, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignMultiModuleYPitch, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iROIAnisoAlignMultiModuleAlignTarget, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignMultiModuleVision, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseStiffenerEpoxy, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iThresholdParam_StiffenerRegion, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDTFilterX_StiffenerRegion, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDTFilterY_StiffenerRegion, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDTValue_StiffenerRegion, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iClosing_StiffenerRegion, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dErosionParam_StiffenerBoundary, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iThresholdParam_SheildcanRegion, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDilationParam_SheildcanBoundary, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dOpeningParam_CornerBoundary, sizeof(double), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDistanceMeasure, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureUpperGv, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureUpperPos, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDistanceMeasureUpperSmFactor, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureUpperEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureLowerGv, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureLowerPos, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDistanceMeasureLowerSmFactor, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureLowerEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureUpperPosOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureLowerPosOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureUsageType, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureSaveID, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDistanceMeasureGapInspectionUpper, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDistanceMeasureGapInspectionLower, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDistanceMeasureROIType, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseGapMeasure, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iGapMeasureUpperGv, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iGapMeasureUpperPos, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dGapMeasureUpperSmFactor, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iGapMeasureUpperEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iGapMeasureLowerGv, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iGapMeasureLowerPos, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dGapMeasureLowerSmFactor, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iGapMeasureLowerEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iGapMeasureUpperPosOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iGapMeasureLowerPosOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iGapMeasureSaveID, sizeof(int), &dwBytesRead, NULL);
	if (iModelVersion >= 1023)
	{
		ReadFile(hFile, &m_iGapMeasureImageNo1, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iGapMeasureImageNo2, sizeof(int), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_dGapMeasureGapInspectionUpper, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dGapMeasureGapInspectionLower, sizeof(double), &dwBytesRead, NULL);

	if (iModelVersion >= 1043)
		ReadFile(hFile, &m_dGapMeasureGapMeasureOffset, sizeof(double), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iGapMeasureDir, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bGapMeasureAlarm, sizeof(BOOL), &dwBytesRead, NULL);
	if (iModelVersion >= 1003)
		ReadFile(hFile, &m_iGapMeasurePointNumber, sizeof(int), &dwBytesRead, NULL);

	if (iModelVersion >= 1019)
		ReadFile(hFile, &m_iGapMeasureSelectType, sizeof(int), &dwBytesRead, NULL);

	if (iModelVersion >= 1041)
	{
		ReadFile(hFile, &m_bGapMeasureUpperLine, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bGapMeasureLowerLine, sizeof(BOOL), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 1052)
	{
		ReadFile(hFile, &m_bGapMeasureDiffImage, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iGapMeasurePosStart, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iGapMeasurePosEnd, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iGapMeasureImageNo3, sizeof(int), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 1053)
	{
		ReadFile(hFile, &m_bGapMeasureEndRemove, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iGapMeasureRemoveEndUpper, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iGapMeasureRemoveEndLower, sizeof(int), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 1066)
	{
		ReadFile(hFile, &m_bGapMeasureRetry, sizeof(BOOL), &dwBytesRead, NULL);

		if (iModelVersion >= 1068)
			ReadFile(hFile, &m_iGapMeasureRetryImageNo, sizeof(int), &dwBytesRead, NULL);

		ReadFile(hFile, &m_iGapMeasureRetryGv, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iGapMeasureRetryPos, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dGapMeasureRetrySmFactor, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iGapMeasureRetryEdgeStr, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iGapMeasureRetryPosOffset, sizeof(int), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_bUseDefectConditionXUnder, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionYUnder, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseSurfaceDualInspection, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iSurfaceDualImageIndex_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iSurfaceDualImageIndex_2, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iSurfaceDualDefectType_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iSurfaceDualDefectType_2, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseBrightFixedThres_1, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightLowerThres_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUpperThres_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightDT_1, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightMedianFilterSize_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightClosingFilterSize_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterType_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterSize_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterSize2_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTValue_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDTPolarTrans_1, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightHystThres_1, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystSecureThres_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystPotentialThres_1, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystPotentialLength_1, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseBrightFixedThres_2, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightLowerThres_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUpperThres_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightDT_2, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightMedianFilterSize_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightClosingFilterSize_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterType_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterSize_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterSize2_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTValue_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDTPolarTrans_2, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightHystThres_2, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystSecureThres_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystPotentialThres_2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystPotentialLength_2, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iDualBlobProcessType, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDualBlobDefectConditionEach, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseNeckEpoxyMeasure, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyMeasureTopEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyMeasureBottomEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyMeasureRightEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyToleranceHeightMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyToleranceHeightMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyToleranceLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyToleranceTopPosOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyToleranceBottomPosOffset, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iNeckEpoxyInspectMethod, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseNeckEpoxyCrack, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyCrackOpeningSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyCrackYLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iNeckEpoxyCrackBoundaryWidth, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iNeckEpoxyCrackDir, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iDnnInspectionType, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDnnInspectionModel, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseCornerMeasure, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iCornerMeasureSaveID, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iCornerMeasureMultiPointNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iCornerMeasurePositionRange, sizeof(int), &dwBytesRead, NULL);

	for (int i = 0; i < 2; i++)
	{
		ReadFile(hFile, &(m_iCornerMeasureGv[i]), sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &(m_iCornerMeasurePos[i]), sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &(m_dCornerMeasureSmFactor[i]), sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &(m_iCornerMeasureEdgeStr[i]), sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &(m_iCornerMeasurePosOffset[i]), sizeof(int), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_dCornerMeasureTargetSmFactor, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iCornerMeasureTargetEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dCornerMeasureTargetDistanceSpec, sizeof(double), &dwBytesRead, NULL);

	if (iModelVersion >= 1001)
	{
		ReadFile(hFile, &m_bUseComponent, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dComponentScore, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dComponentShiftX, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dComponentShiftY, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dComponentRotationAngle, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bComponentApply_1, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bComponentApply_2, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bComponentApply_3, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bComponentApply_4, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iComponentDefectType1, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iComponentDefectType2, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iComponentDefectType3, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iComponentDefectType4, sizeof(int), &dwBytesRead, NULL);
	}

	if (iModelVersion < 1049 && iModelVersion >= 1033)
	{
		ReadFile(hFile, &bTemp, sizeof(BOOL), &dwBytesRead, NULL);

		for (int i = 0; i < 70; i++)		// MAX_FAI_ITEM=70
		{
			ReadFile(hFile, &bTemp, sizeof(BOOL), &dwBytesRead, NULL);
			ReadFile(hFile, &dTemp, sizeof(double), &dwBytesRead, NULL);
			ReadFile(hFile, &dTemp, sizeof(double), &dwBytesRead, NULL);
			ReadFile(hFile, &dTemp, sizeof(double), &dwBytesRead, NULL);
		}
	}

	ReadFile(hFile, &m_bNeckEpoxyCrackUseXDirMode, sizeof(BOOL), &dwBytesRead, NULL);  //VER 1.1.3.0 add

	ReadFile(hFile, &m_bDefectConditionOuterDiff, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_nDefectConditionOuterDiffInterval, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_nDefectConditionOuterDiffMinDiff, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_nOuterDiffUpper, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_nDefectConditionOuterDiffMaxRepeat, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bDefectConditionConerLengthUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_nDefectConditionConerLengthDir, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_nDefectConditionConerLengthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_nDefectConditionConerLengthMax, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bDefectConditionConerLengthMaxUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseImageProcessPostLocalAlign, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessPostLocalAlignImageIndex, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageProcessPostLocalAlignROINo, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iImageProcessPostLocalAlignFitPos, sizeof(int), &dwBytesRead, NULL);


	if (iModelVersion < 1049 && iModelVersion >= 1037)
	{
		for (int i = 0; i < 70; i++)			// MAX_FAI_ITEM=70
		{
			ReadFile(hFile, &dTemp, sizeof(double), &dwBytesRead, NULL);
			ReadFile(hFile, &dTemp, sizeof(double), &dwBytesRead, NULL);
		}
	}

	if (iModelVersion >= 1131)
	{
		ReadFile(hFile, &m_bComponentApply_5, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iComponentDefectType5, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dComponentRotationAngle2, sizeof(double), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 1132 && iModelVersion < 2016)
		ReadFile(hFile, &bTemp, sizeof(BOOL), &dwBytesRead, NULL);

	if (iModelVersion >= 1138)
	{
		ReadFile(hFile, &m_bUseHSV, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &miLowerHue, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &miUpperHue, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &miLowerSaturation, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &miUpperSaturation, sizeof(int), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 2000)
	{
		ReadFile(hFile, &m_bUseHeightMeasure, sizeof(BOOL), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 2001)
	{
		ReadFile(hFile, &m_iHeightMeasureType, sizeof(int), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 2003)
	{
		ReadFile(hFile, &m_bUseBoundaryInspection, sizeof(BOOL), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 2005)
		ReadFile(hFile, &m_iBoundaryEdgeMeasureDir, sizeof(int), &dwBytesRead, NULL);

	if (iModelVersion >= 2004)
	{
		ReadFile(hFile, &m_iBoundaryEdgeMeasureGv, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iBoundaryEdgeMeasurePos, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dBoundaryEdgeMeasureSmFactor, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_dBoundaryEdgeMeasureEdgeStr, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iBoundaryEdgeMeasureSampling, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iBoundaryEdgeMeasureSensorLength, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iBoundaryDisplay, sizeof(int), &dwBytesRead, NULL);

		for (int i = 0; i < 2; i++)
		{
			ReadFile(hFile, &m_bBoundaryCondition[i], sizeof(BOOL), &dwBytesRead, NULL);
			ReadFile(hFile, &m_iBoundaryMedianFilterSize[i], sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &m_dBoundaryDefectDepth[i], sizeof(double), &dwBytesRead, NULL);

			if (iModelVersion >= 2010)
				ReadFile(hFile, &m_dBoundaryDefectMBDepth[i], sizeof(double), &dwBytesRead, NULL);

			ReadFile(hFile, &m_dBoundaryDefectLength[i], sizeof(double), &dwBytesRead, NULL);
			ReadFile(hFile, &m_bBoundaryUseConnection[i], sizeof(BOOL), &dwBytesRead, NULL);
			ReadFile(hFile, &m_iBoundaryConnectionLength[i], sizeof(int), &dwBytesRead, NULL);
		}

		if (iModelVersion >= 2027)
		{
			ReadFile(hFile, &m_bBoundaryCondition[2], sizeof(BOOL), &dwBytesRead, NULL);
			ReadFile(hFile, &m_dBoundaryDefectDepth[2], sizeof(double), &dwBytesRead, NULL);
			ReadFile(hFile, &m_dBoundaryDefectLength[2], sizeof(double), &dwBytesRead, NULL);

			ReadFile(hFile, &m_bBoundaryUseRoiConnected, sizeof(BOOL), &dwBytesRead, NULL);
		}
	}

	return TRUE;
}