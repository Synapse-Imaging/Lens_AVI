#pragma once

#include "IniFileCS.h"

class CAlgorithmParam
{
public:
	CAlgorithmParam(void);
	virtual ~CAlgorithmParam(void);

	BOOL m_bInspect;

	BOOL m_bUseFilter1;
	BOOL m_bUseFilter2;
	BOOL m_bUseFilter3;
	BOOL m_bUseFilter4;

	BOOL m_bCheckAIUseConsiderImageNo;
	BOOL m_bCheckAIUseConsiderDefectCode;

	int m_iDefectType;	// 0 ~ 10

	BOOL m_bUseInspectArea[3];
	int	 m_iInspectAreaContour1Margin[3];
	int	 m_iInspectAreaContour2Margin[3];
	
	// Surface Inspection
	BOOL m_bUseSurfaceInspection;

	BOOL m_bUseSurfaceMeasureSaveLog;

	BOOL m_bUseSurfaceInspectEachROI;
	int m_iSurfaceInspectEachROINgNumber;

	BOOL m_bApplyCircleFitting;
	int m_iCircleFittingEdgeStr;
	int m_iCircleFittingEdgeMargin;

	BOOL m_bUseDTPolarTrans;

	BOOL m_bUseFTConnected;
	BOOL m_bUseFTConnectedArea;
	BOOL m_bUseFTConnectedLength;
	BOOL m_bUseFTConnectedWidth;
	int m_iFTConnectedAreaMin;
	int m_iFTConnectedLengthMin;
	int m_iFTConnectedWidthMin;
	BOOL m_bUseBarcodeBlob;
	BOOL m_bUseBlobCornerMeasure;
	BOOL m_bUseSaveBNEOverflowTop;
	BOOL m_bUseSaveBNEOverflowBottom;
	
	BOOL m_bUseBrightFixedThres;
	int m_iBrightLowerThres;
	int m_iBrightUpperThres;
	BOOL m_bUseBrightDT;
	int m_iBrightMedianFilterSize;
	int m_iBrightClosingFilterSize;
	int m_iBrightDTFilterType;
	int m_iBrightDTFilterSize;
	int m_iBrightDTFilterSize2;
	int m_iBrightDTValue;
	BOOL m_bUseBrightUniformityCheck;
	int m_iBrightUniformityPeakMin;
	int m_iBrightUniformityPeakMax;
	int m_iBrightUniformityOffset;
	int m_iBrightUniformityHystLength;
	int m_iBrightUniformityHystOffset;
	BOOL m_bUseBrightHystThres;
	int m_iBrightHystSecureThres;
	int m_iBrightHystPotentialThres;
	int m_iBrightHystPotentialLength;

	BOOL m_bUseDarkFixedThres;
	int m_iDarkLowerThres;
	int m_iDarkUpperThres;
	BOOL m_bUseDarkDT;
	int m_iDarkMedianFilterSize;
	int m_iDarkClosingFilterSize;
	int m_iDarkDTFilterType;
	int m_iDarkDTFilterSize;
	int m_iDarkDTFilterSize2;
	int m_iDarkDTValue;
	BOOL m_bUseDarkUniformityCheck;
	int m_iDarkUniformityPeakMin;
	int m_iDarkUniformityPeakMax;
	int m_iDarkUniformityOffset;
	int m_iDarkUniformityHystLength;
	int m_iDarkUniformityHystOffset;
	BOOL m_bUseDarkHystThres;
	int m_iDarkHystSecureThres;
	int m_iDarkHystPotentialThres;
	int m_iDarkHystPotentialLength;
	BOOL m_bUseBrightBinThres;
	BOOL m_bUseDarkBinThres;

	int m_iCandidateMinArea;
	BOOL m_bCombineBrightDarkBlob;
	BOOL m_bMaxBlob;
	BOOL m_bClosingOpening;
	BOOL m_bApplyOneDirMorp;
	int m_iOneDirMorpXOpening;
	int m_iOneDirMorpXClosing;
	int m_iOpeningSize;
	int m_iClosingSize;
	int m_iThresholdOpeningSize;
	int m_iThresholdClosingSize;
	BOOL m_bFillup;
	BOOL m_bUseConvex;
	BOOL m_bUseSubConvex;
	BOOL m_bUseRect;
	BOOL m_bUseConnection;
	int m_iConnectionMinSize;
	int m_iConnectionMinXLength;
	int m_iConnectionLength;
	BOOL m_bUseAngle;
	int m_iConnectionMaxWidth;
	int m_iHoleFillCondition;
	int m_iFillHoleArea;
	BOOL m_bXYLengthMeasureIgnoreHole;
	BOOL m_bXYLengthMaxLineLength;
	BOOL m_bDefectConditionBoundaryConnectionAny;
	int m_iDefectConditionBoundaryConnectionOuter;
	int m_iDefectConditionBoundaryMarginAny;
	int m_iDefectConditionBoundaryMarginMinConnectionAny;

	BOOL m_bUseDefectConditionArea;
	BOOL m_bUseDefectConditionLength;
	BOOL m_bUseDefectConditionWidth;
	BOOL m_bUseDefectConditionXLength;
	BOOL m_bUseDefectConditionYLength;
	BOOL m_bUseDefectConditionMean;
	BOOL m_bUseDefectConditionStdev;
	BOOL m_bUseDefectConditionAnisometry;
	BOOL m_bUseDefectConditionCircularity;
	BOOL m_bUseDefectConditionRectangularity;
	BOOL m_bUseDefectConditionConvexity;
	BOOL m_bUseDefectConditionEllipseRatio;
	BOOL m_bUseDefectConditionAreaRatio;
	BOOL m_bUseDefectConditionContrast;
	BOOL m_bUseDefectConditionGrad;
	BOOL m_bUseDefectConditionBlob;
	BOOL m_bUseDefectConditionShiftX;
	BOOL m_bUseDefectConditionShiftY;
	BOOL m_bUseDefectConditionOuterDist;
	BOOL m_bUseDefectConditionInnerDist;
	BOOL m_bUseDefectConditionUseZeroArea;
	BOOL m_bDefectConditionBoundaryConnection;
	BOOL m_bDefectConditionBoundaryMarginTop;
	BOOL m_bDefectConditionBoundaryMarginBottom;
	BOOL m_bDefectConditionBoundaryMarginLeft;
	BOOL m_bDefectConditionBoundaryMarginRight;
	BOOL m_bDefectConditionBoundaryMarginHCenter;
	BOOL m_bDefectConditionBoundaryMarginVCenter;
	BOOL m_bUseDefectConditionXUnder;
	BOOL m_bUseDefectConditionYUnder;

	int m_iDefectConditionAreaMin;
	int m_iDefectConditionLengthMin;
	int m_iDefectConditionWidthMin;
	int m_iDefectConditionXLengthMin;
	int m_iDefectConditionYLengthMin;
	int m_iDefectConditionMeanMin;
	double m_dDefectConditionStdevMin;
	double m_dDefectConditionAnisometryMin;
	double m_dDefectConditionCircularityMin;
	double m_dDefectConditionRectangularityMin;
	double m_dDefectConditionConvexityMin;
	double m_dDefectConditionEllipseRatioMin;
	int m_iDefectConditionAreaRatioMin;
	int m_iDefectConditionContrastMin;
	int m_iDefectConditionGradMin;
	int m_iDefectConditionBlobMin;
	int m_iDefectConditionShiftXMin;
	int m_iDefectConditionShiftYMin;

	int m_iDefectConditionAreaMax;
	int m_iDefectConditionLengthMax;
	int m_iDefectConditionWidthMax;
	int m_iDefectConditionXLengthMax;
	int m_iDefectConditionYLengthMax;
	int m_iDefectConditionMeanMax;
	double m_dDefectConditionStdevMax;
	double m_dDefectConditionAnisometryMax;
	double m_dDefectConditionCircularityMax;
	double m_dDefectConditionRectangularityMax;
	double m_dDefectConditionConvexityMax;
	double m_dDefectConditionEllipseRatioMax;
	int m_iDefectConditionAreaRatioMax;
	int m_iDefectConditionContrastMax;
	int m_iDefectConditionGradMax;
	int m_iDefectConditionBlobMax;
	int m_iDefectConditionShiftXMax;
	int m_iDefectConditionShiftYMax;

	int m_iDefectConditionOuterDist;
	int m_iDefectConditionInnerDist;

	int m_iDefectConditionBoundaryMarginTop;
	int m_iDefectConditionBoundaryMarginBottom;
	int m_iDefectConditionBoundaryMarginLeft;
	int m_iDefectConditionBoundaryMarginRight;
	int m_iDefectConditionBoundaryMarginHCenter;
	int m_iDefectConditionBoundaryMarginVCenter;
	int m_iDefectConditionBoundaryMarginMinConnection;

	BOOL m_bUseImageScaling;
	int m_iImageScaleMethod;
	int m_iImageScaleMin;
	int m_iImageScaleMax;

	BOOL m_bUseEdgeImage;
	int m_iEdgeFilterType;
	double m_dEdgeImageScale;

	BOOL m_bUsePeak;
	int m_iNormalPeakThresMin;
	int m_iNormalPeakThresMax;
	int m_iAbnormalPeakHistogramOffset;

	int m_iLengthWidthCondition;
	int m_iXYLengthCondition;

	int m_iBoundaryConnectionCondition;
	BOOL m_bDefectConditionBoundaryConnection2Side;
	BOOL m_bDefectConditionInnerOuterBoundaryConnection;

	int m_iXLengthCondition;
	int m_iXLengthConditionMinRef;
	int m_iYLengthCondition;
	int m_iYLengthConditionMinRef;

	BOOL m_bUseDefectConditionHole;
	int m_iDefectConditionHoleArea;

	BOOL m_bDefectConditionUnionBlob;
	BOOL m_bDefectConditionDilation;
	int m_iDefectConditionDilationSize;

	//VER 1.1.3.2 add
	BOOL m_bDefectConditionOuterDiff;
	int m_nDefectConditionOuterDiffInterval;
	int m_nDefectConditionOuterDiffMinDiff;
	int m_nOuterDiffUpper;
	int m_nDefectConditionOuterDiffMaxRepeat;
	//VER 1.1.3.2 end

	//VER 1.1.3.3 start
	int m_nDefectConditionConerLengthDir;
	BOOL m_bDefectConditionConerLengthUse;
	int m_nDefectConditionConerLengthMin;
	int m_nDefectConditionConerLengthMax;
	//VER 1.1.3.3 end

	//VER 1.1.3.4 add
	BOOL m_bDefectConditionConerLengthMaxUse;
	//VER 1.1.3.4 end

	BOOL m_bUseDefectConditionXLengthSum;
	int m_iDefectConditionXLengthSumMin;
	int m_iDefectConditionXLengthSumMax;
	BOOL m_bUseDefectConditionYLengthSum;
	int m_iDefectConditionYLengthSumMin;
	int m_iDefectConditionYLengthSumMax;

	BOOL m_bDefectConditionOrthoLength;
	int m_iOrthoLengthRefSide;

	// Fitting
	BOOL m_bUseLineFitting;
	int m_iLineFitObjectLowerThres;
	int m_iLineFitObjectUpperThres;
	BOOL m_bLineFitUseOtherImageFitting;
	int m_iLineFitOtherImageNumber;
	double m_dLineFitFittingEdgeSF;
	double m_dLineFitObjectEdgeSF;
	int  m_iLineFitMinContourLength;
	int  m_iLineFitContourConnectionLength;
	BOOL m_bLineFitUseBurInspection;
	double m_dLineFitBurDefectLength;
	double m_dLineFitBurDefectLengthMax;
	BOOL m_bLineFitUseChippingInspection;
	double m_dLineFitChippingDefectLength;
	double m_dLineFitChippingDefectLengthMax;
	BOOL m_bLineFitUseRotationInspection;
	int m_iLineFitRotationReference;
	double m_dLineFitRotationDefectAngle;
	int m_iLineFitEdgeMethod;
	int m_iLineFitFittingEdgeHigh;
	int m_iLineFitFittingContourConnectionLength;
	int m_iLineFitFittingMinContourLength;
	int m_iLineFitEdgeHigh;
	BOOL m_bLineFitLengthSave;
	BOOL m_bNoUseLineFit;
	BOOL m_bLineFitLocalAlignUse;

	// Connector Pin Length
	BOOL m_bUsePinLength;
	int m_iPinPos;
	int m_iPinDTFilterSize;
	int m_iPinOpeningClosingSize;
	int m_iPinMinArea;
	int m_iPinDistanceTolerance;
	int m_iPinDTValue;
	int m_iPinDefectNumberTolerance;
	int m_iPinPitch;

	// Dent
	BOOL m_bUseDent;
	int m_iDentMedFilterSize;
	int m_iDentDtFilterSize;
	int m_iDentBrightDtValue;
	int m_iDentDarkDtValue;
	int m_iDentCandidateMinArea;
	int m_iDentCandidateMinWidth;
	int m_iDentCandidateMinDistance;
	int m_iDentMinArea;

	// Image Compare
	BOOL m_bUseImageCompare;
	BOOL m_bImageCompareSaveFeatureLog;
	BOOL m_bUseImageCompareWindow;
	int m_iImageCompareWindowSize;
	BOOL m_bUseImageCompareEdgeImage;
	BOOL m_bUseImageCompareNormalizer;
	int m_iImageCompareNormalizeWeight;
	BOOL m_bUseImageCompareBright;
	BOOL m_bUseImageCompareDark;
	int m_iImageCompareBrightAbs;
	int m_iImageCompareDarkAbs;
	double m_dImageCompareBrightVar;
	double m_dImageCompareDarkVar;
	BOOL m_bUseImageCompareAutoTrain;
	BOOL m_bUseImageCompareMod;
	BOOL m_bUseImageCompareModBright;
	BOOL m_bUseImageCompareModDark;
	int m_iImageCompareModBrightAbs;
	int m_iImageCompareModDarkAbs;
	double m_dImageCompareModBrightVar;
	double m_dImageCompareModDarkVar;
	int m_iImageCompareModTop;
	int m_iImageCompareModBottom;
	int m_iImageCompareModLeft;
	int m_iImageCompareModRight;

	// Part Check
	BOOL m_bUsePartCheck;
	BOOL m_bPartCheckMultipleModel;
	int m_iPartCheckMatchingAngleRange;
	int m_iPartCheckMatchingPLevel;
	int m_iPartCheckMatchingScore;
	int m_iPartCheckMatchingSearchMarginX;
	int m_iPartCheckMatchingSearchMarginY;
	int m_iPartCheckMatchingSearchMarginX2;
	int m_iPartCheckMatchingSearchMarginY2;
	BOOL m_bPartCheckInspectExistence;
	BOOL m_bPartCheckBlob;
	BOOL m_bPartCheckInspectShift;
	int m_iPartCheckShiftX;
	int m_iPartCheckShiftY;
	double m_dPartCheckRotationAngle;
	BOOL m_bPartCheckLocalAlignUse;
	BOOL m_bPartCheckApplyComp;
	BOOL m_bPartCheckSaveLog;
	BOOL m_bPartCheckMeasureEdge;
	int	m_iPartCheckLegXSize;
	int	m_iPartCheckLegYSize;
	double m_dPartCheckEdgeMeasueSmFactor;
	double m_dPartCheckEdgeMeasueEdgeStr;
	BOOL m_bPartCheckInspectLegDamage;
	BOOL m_bPartCheckMultiModuleInspect;
	int m_iPartCheckMultiModuleXNumber;
	int m_iPartCheckMultiModuleYNumber;
	int m_iPartCheckMultiModuleXPitch;
	int m_iPartCheckMultiModuleYPitch;
	BOOL m_bPartCheckPickerPosUse;
	BOOL m_bPartCheckMultiModuleMoveOffset;
	int m_iPartCheckMoveOffsetX[ALIGN_MODULE_Y][ALIGN_MODULE_X];
	int m_iPartCheckMoveOffsetY[ALIGN_MODULE_Y][ALIGN_MODULE_X];
	int m_iPartCheckMultiModuleTarget;

	// Concenter Measure
	BOOL m_bUseConcenter;
	int m_iConcenterInnerCircleDiameter;
	double m_dConcenterMatchingScaleMin;
	double m_dConcenterMatchingScaleMax;
	int m_iConcenterMatchingMinContrast;
	int m_iConcenterMatchingScore;
	int m_iConcenterCenterDiffSpec;
	double m_dConcenterCenterDiffSpecX;
	double m_dConcenterCenterDiffSpecY;

	// Homer Mark
	BOOL m_bUseHomerMark;
	int m_iHomerMarkPos;
	int m_iHomerMarkOrder;

	// ROI Align
	BOOL m_bUseROIAlign;
	int m_iROIAlignXSearchMargin;
	int m_iROIAlignYSearchMargin;
	int m_iROIAlignMatchingAngleRange;
	double m_dROIAlignMatchingScaleMin;
	double m_dROIAlignMatchingScaleMax;
	int m_iROIAlignMatchingMinContrast;
	BOOL m_bROIAlignUsePosX;
	BOOL m_bROIAlignUsePosY;
	double m_dROIAlignMatchingScore;
	BOOL m_bROIAlignInspectShift;
	int m_iROIAlignMatchingPLevel;
	double m_dROIAlignShiftTop;
	double m_dROIAlignShiftBottom;
	double m_dROIAlignShiftLeft;
	double m_dROIAlignShiftRight;
	BOOL m_bROIAlignLocalAlignUse;
	BOOL m_bUseIsoColorImage;
	BOOL m_bROIAlignMultiModuleInspect;
	int m_iROIAlignMultiModuleXNumber;
	int m_iROIAlignMultiModuleYNumber;
	int m_iROIAlignMultiModuleXPitch;
	int m_iROIAlignMultiModuleYPitch;
	int m_iROIAlignMultiModuleAlignTarget;
	int m_iROIAlignMultiModuleVision;

	// Image Processing
	int m_iProcessChType;
	int m_iProcessResType;
	BOOL m_bUseImageProcess;
	int m_iImageProcessArithmeticType;
	int m_iImageProcessArithmeticImage1;
	int m_iImageProcessArithmeticImage2;
	int m_iImageProcessArithmeticImage3;
	int m_iImageProcessArithmeticImage4;
	double m_dImageProcessArithmeticMulti;
	int m_iImageProcessArithmeticAdd;
	BOOL m_bUseImageProcessFilter;
	int m_iImageProcessFilterType1;
	int m_iImageProcessFilterType2;
	int m_iImageProcessFilterType3;
	double m_iImageProcessFilterType1X;
	double m_iImageProcessFilterType2X;
	double m_iImageProcessFilterType3X;
	double m_iImageProcessFilterType1Y;
	double m_iImageProcessFilterType2Y;
	double m_iImageProcessFilterType3Y;
	BOOL m_bUseImageProcessLocalAlign;
	int m_iImageProcessLocalAlignImageIndex;
	int m_iImageProcessLocalAlignROINo;

	BOOL m_bUseImageProcessPostLocalAlign;
	int m_iImageProcessPostLocalAlignImageIndex;
	int m_iImageProcessPostLocalAlignROINo;
	
	BOOL m_bImageProcessLocalAlignPosX;
	BOOL m_bImageProcessLocalAlignPosY;
	BOOL m_bImageProcessLocalAlignAngle;
	int m_iImageProcessLocalAlignFitPos;
	int m_iImageProcessPostLocalAlignFitPos;
	int m_iImageProcessLocalAlignROIType;
	int m_iImageProcessLocalAlignMatchingApplyMethod;
	BOOL m_bUseImageProcessDontCare;
	int m_iImageProcessDontCareImageIndex;
	int m_iImageProcessDontCareROINo;
	int m_iImageProcessDontCareMargin;
	int m_iImageProcessDontCareMarginInner;
	BOOL m_bUseImageProcessDontCare2;
	int m_iImageProcessDontCareImageIndex2;
	int m_iImageProcessDontCareROINo2;
	int m_iImageProcessDontCareMargin2;
	int m_iImageProcessDontCareMargin2Inner;
	BOOL m_bUseImageProcessGenerate;
	BOOL m_bUseImageProcessGenerateInspectFlag;
	int m_iImageProcessGenerateImageIndex;
	int m_iImageProcessGenerateROINo;
	int m_iImageProcessGenerateMargin;
	BOOL m_bUseImageProcessMask;
	int m_iImageProcessMaskImageIndex;
	int m_iImageProcessMaskROINo;
	BOOL m_bUseImageProcessMask2;
	int m_iImageProcessMaskImageIndex2;
	int m_iImageProcessMaskROINo2;
	BOOL m_bUseImageProcessLocalAlignAddLineFit;
	int m_iImageProcessLocalAlignImageIndex_2;
	int m_iImageProcessLocalAlignROINo_2;
	int m_iImageProcessLocalAlignFitPos_2;

	// OCR //2017.04.11 hbk
	BOOL m_bUseOCR;
	long m_lCharPlateWidth;
	long m_lMinCharWidth;
	long m_lMaxCharWidth;
	long m_lMinCharHeight;
	long m_lMaxCharHeight;
	long m_lMinCharCandidateHeight;
	long m_lWholeAreaDivide;
	long m_lHorizontalDivide;
	long m_lFirstDiffernt;
	long m_lSecondDifferent;
	long m_ldiff;
	int m_iOCRSegmentDilation;

	// Barcode
	BOOL m_bUseBarcode;
	double m_dBarcodeShiftToleranceX;
	double m_dBarcodeShiftToleranceY;
	BOOL m_bInspectBarcodeShift;
	BOOL m_bInspectBarcodeRotation;
	BOOL m_bUseBarcodeOtherImage;
	int m_iBarcodeOtherImageNumber;

	// 요철 영상
	BOOL m_bUseShape;
	int m_iShapeIlluminationFilterX;
	int m_iShapeIlluminationFilterY;
	double m_dShapeIlluminationScaleFactor;
	double m_dShapeCurvatureSmFactor;
	double m_dShapeScalingMulti;
	double m_dShapeScalingAdd;
	double m_dShapeImageReduceRatio;
	double m_dShapeLedAngle;
	int m_iShapeImageMargin;

	// Don't Care
	int m_iProcessChTypeDC;
	BOOL m_bUseDC;

	BOOL m_bUseBrightFixedThresDC;
	int m_iBrightLowerThresDC;
	int m_iBrightUpperThresDC;
	BOOL m_bUseBrightDTDC;
	int m_iBrightMedianFilterSizeDC;
	int m_iBrightClosingFilterSizeDC;
	int m_iBrightDTFilterTypeDC;
	int m_iBrightDTFilterSizeDC;
	int m_iBrightDTFilterSizeDC2;
	int m_iBrightDTValueDC;
	BOOL m_bUseBrightUniformityCheckDC;
	int m_iBrightUniformityPeakMinDC;
	int m_iBrightUniformityPeakMaxDC;
	int m_iBrightUniformityOffsetDC;
	int m_iBrightUniformityHystLengthDC;
	int m_iBrightUniformityHystOffsetDC;

	BOOL m_bUseDarkFixedThresDC;
	int m_iDarkLowerThresDC;
	int m_iDarkUpperThresDC;
	BOOL m_bUseDarkDTDC;
	int m_iDarkMedianFilterSizeDC;
	int m_iDarkClosingFilterSizeDC;
	int m_iDarkDTFilterTypeDC;
	int m_iDarkDTFilterSizeDC;
	int m_iDarkDTFilterSizeDC2;
	int m_iDarkDTValueDC;
	BOOL m_bUseDarkUniformityCheckDC;
	int m_iDarkUniformityPeakMinDC;
	int m_iDarkUniformityPeakMaxDC;
	int m_iDarkUniformityOffsetDC;
	int m_iDarkUniformityHystLengthDC;
	int m_iDarkUniformityHystOffsetDC;

	int m_iCandidateMinAreaDC;
	BOOL m_bCombineBrightDarkBlobDC;
	BOOL m_bMaxBlobDC;
	BOOL m_bClosingOpeningDC;
	int m_iOpeningSizeDC;
	int m_iClosingSizeDC;
	BOOL m_bFillupDC;
	BOOL m_bUseConvexDC;
	BOOL m_bUseConnectionDC;
	int m_iConnectionMinSizeDC;
	int m_iConnectionMinXLengthDC;
	int m_iConnectionLengthDC;
	BOOL m_bUseAngleDC;
	int m_iConnectionMaxWidthDC;

	BOOL m_bUseDefectConditionAreaDC;
	BOOL m_bUseDefectConditionLengthDC;
	BOOL m_bUseDefectConditionWidthDC;
	BOOL m_bUseDefectConditionXLengthDC;
	BOOL m_bUseDefectConditionYLengthDC;
	BOOL m_bUseDefectConditionMeanDC;
	BOOL m_bUseDefectConditionStdevDC;
	BOOL m_bUseDefectConditionAnisometryDC;
	BOOL m_bUseDefectConditionCircularityDC;
	BOOL m_bUseDefectConditionRectangularityDC;
	BOOL m_bUseDefectConditionConvexityDC;
	BOOL m_bUseDefectConditionEllipseRatioDC;
	BOOL m_bUseDefectConditionAreaRatioDC;
	BOOL m_bUseDefectConditionContrastDC;
	BOOL m_bUseDefectConditionBlobDC;
	BOOL m_bUseDefectConditionShiftXDC;
	BOOL m_bUseDefectConditionShiftYDC;
	BOOL m_bUseDefectConditionOuterDistDC;
	BOOL m_bUseDefectConditionInnerDistDC;
	BOOL m_bUseDefectConditionUseZeroAreaDC;
	BOOL m_bDefectConditionBoundaryConnectionDC;
	BOOL m_bDefectConditionBoundaryMarginTopDC;
	BOOL m_bDefectConditionBoundaryMarginBottomDC;
	BOOL m_bDefectConditionBoundaryMarginLeftDC;
	BOOL m_bDefectConditionBoundaryMarginRightDC;

	int m_iDefectConditionAreaMinDC;
	int m_iDefectConditionLengthMinDC;
	int m_iDefectConditionWidthMinDC;
	int m_iDefectConditionXLengthMinDC;
	int m_iDefectConditionYLengthMinDC;
	int m_iDefectConditionMeanMinDC;
	double m_dDefectConditionStdevMinDC;
	double m_dDefectConditionAnisometryMinDC;
	double m_dDefectConditionCircularityMinDC;
	double m_dDefectConditionRectangularityMinDC;
	double m_dDefectConditionConvexityMinDC;
	double m_dDefectConditionEllipseRatioMinDC;
	int m_iDefectConditionAreaRatioMinDC;
	int m_iDefectConditionContrastMinDC;
	int m_iDefectConditionBlobMinDC;
	int m_iDefectConditionShiftXMinDC;
	int m_iDefectConditionShiftYMinDC;

	int m_iDefectConditionAreaMaxDC;
	int m_iDefectConditionLengthMaxDC;
	int m_iDefectConditionWidthMaxDC;
	int m_iDefectConditionXLengthMaxDC;
	int m_iDefectConditionYLengthMaxDC;
	int m_iDefectConditionMeanMaxDC;
	double m_dDefectConditionStdevMaxDC;
	double m_dDefectConditionAnisometryMaxDC;
	double m_dDefectConditionCircularityMaxDC;
	double m_dDefectConditionRectangularityMaxDC;
	double m_dDefectConditionConvexityMaxDC;
	double m_dDefectConditionEllipseRatioMaxDC;
	int m_iDefectConditionAreaRatioMaxDC;
	int m_iDefectConditionContrastMaxDC;
	int m_iDefectConditionBlobMaxDC;
	int m_iDefectConditionShiftXMaxDC;
	int m_iDefectConditionShiftYMaxDC;

	int m_iDefectConditionOuterDistDC;
	int m_iDefectConditionInnerDistDC;

	int m_iDefectConditionBoundaryMarginTopDC;
	int m_iDefectConditionBoundaryMarginBottomDC;
	int m_iDefectConditionBoundaryMarginLeftDC;
	int m_iDefectConditionBoundaryMarginRightDC;
	int m_iDefectConditionBoundaryMarginMinConnectionDC;

	int m_iLengthWidthConditionDC;
	int m_iXYLengthConditionDC;

	int m_iBoundaryConnectionConditionDC;
	BOOL m_bDefectConditionBoundaryConnection2SideDC;

	int m_iDontcareRegionMargin;

	// Edge Measure
	BOOL m_bUseEdgeMeasure;
	int m_iEdgeMeasureOnePoint;
	int m_iEdgeMeasureMultiPointNumber;
	BOOL m_bEdgeMeasureEndPoint;
	BOOL m_bEdgeMeasureSlope;
	BOOL m_bEdgeMeasureReverseGv;
	BOOL m_bEdgeMeasureFixedEndPoint;
	int m_iEdgeMeasureLeftSideSize;
	int m_iEdgeMeasureRightSideSize;
	int m_iEdgeMeasureDir;
	int m_iEdgeMeasureGv;
	int m_iEdgeMeasurePos;
	BOOL m_bEdgeMeasureLocalAlignUse;
	double m_dEdgeMeasureSmFactor;
	int m_iEdgeMeasureEdgeStr;
	int m_iEdgeMeasurePosOffset;
	BOOL m_bEdgeMeasureUseMaxMinPoint;
	int m_iEdgeMeasureUseMaxPoint;
	int m_iEdgeMeasurePositionRange;
	BOOL m_bEdgeMeasureUseOther;
	BOOL m_bEdgeMeasureMakeROIOther;
	int m_iEdgeMeasureMakeROIRangeOther;
	int m_iEdgeMeasureImageIndexOther;
	int m_iEdgeMeasureGvOther;
	int m_iEdgeMeasurePosOther;
	double m_dEdgeMeasureSmFactorOther;
	int m_iEdgeMeasureEdgeStrOther;
	int m_iEdgeMeasurePosOffsetOther;
	double m_dEdgeMeasureAngleDeg;
	int m_iEdgeMeasureAccuracy;


	// Tilt
	BOOL m_bUseHomerTilt;
	int m_iHomerTiltHomerEdgeStr;
	int m_iHomerTiltBenvolioImage;
	int m_iHomerTiltBenvolioEdgeStr;
	double m_dHomerTiltHomerLength;
	double m_dHomerTiltTol;
	double m_dHomerTiltDistanceTol;

	// Epoxy Void Hole
	BOOL m_bUseEpoxyVoidHole;
	int m_iEpoxyVoidHoleImage;
	double m_dEpoxyVoidHoleEdgeStr;
	int m_iEpoxyVoidHoleInspectMargin;
	BOOL m_bEpoxyVoidHoleTiltInspect;
	int m_iEpoxyVoidHoleTiltTolerance;
	BOOL m_bEpoxyVoidHoleGapInspect;
	int m_iEpoxyVoidHoleGapTolerance;

	// Barcode Print Quality
	BOOL m_bUsePrintQuality;
	int m_iPrintQualityContrastMin;
	int m_iPrintQualitySymbolColsMin;
	int m_iPrintQualitySymbolRowsMin;
	int m_iPrintQualitySymbolColsMax;
	int m_iPrintQualitySymbolRowsMax;
	int m_iPrintQualityModuleSizeMin;
	int m_iPrintQualityModuleSizeMax;
	double m_dPrintQualitySlantMax;
	BOOL m_bPrintQualityOverallQuality;
	int m_iPrintQualityGradeOverallQuality;
	BOOL m_bPrintQualityUnusedErrorCorrection;
	int m_iPrintQualityGradeUnusedErrorCorrection;
	int m_iPrintQualityStrictModel;
	int m_iPrintQualityPolarity;
	int m_iPrintQualityMirrored;
	int m_iPrintQualityContrastTolerance;
	int m_iPrintQualityStrictQuietZone;
	int m_iPrintQualitySymbolShape;
	int m_iPrintQualitySmallModulesRobustness;
	int m_iPrintQualityModuleGrid;
	int m_iPrintQualityModuleGapMin;
	int m_iPrintQualityModuleGapMax;
	int m_iPrintQualityFinderPatternTolerance;

	// ROI Align Aniso
	BOOL m_bUseROIAnisoAlign;
	int m_iROIAnisoAlignXSearchMargin;
	int m_iROIAnisoAlignYSearchMargin;
	int m_iROIAnisoAlignMatchingAngleRange;
	double m_dROIAnisoAlignMatchingScaleMinY;
	double m_dROIAnisoAlignMatchingScaleMaxY;
	double m_dROIAnisoAlignMatchingScaleMinX;
	double m_dROIAnisoAlignMatchingScaleMaxX;
	int m_iROIAnisoAlignMatchingMinContrast;
	BOOL m_bROIAnisoAlignUsePosX;
	BOOL m_bROIAnisoAlignUsePosY;
	double m_dROIAnisoAlignMatchingScore;
	BOOL m_bROIAnisoAlignInspectShift;
	int m_iROIAnisoAlignMatchingPLevel;
	double m_dROIAnisoAlignShiftTop;
	double m_dROIAnisoAlignShiftBottom;
	double m_dROIAnisoAlignShiftLeft;
	double m_dROIAnisoAlignShiftRight;
	BOOL m_bROIAnisoAlignLocalAlignUse;
	BOOL m_bUseAnisoColorImage;
	BOOL m_bROIAnisoAlignMultiModuleInspect;
	int m_iROIAnisoAlignMultiModuleXNumber;
	int m_iROIAnisoAlignMultiModuleYNumber;
	int m_iROIAnisoAlignMultiModuleXPitch;
	int m_iROIAnisoAlignMultiModuleYPitch;
	int m_iROIAnisoAlignMultiModuleAlignTarget;
	int m_iROIAnisoAlignMultiModuleVision;

	// Stiffener Epoxy Inspection ROI
	BOOL m_bUseStiffenerEpoxy;
	int m_iThresholdParam_StiffenerRegion;
	int m_iDTFilterX_StiffenerRegion;
	int	m_iDTFilterY_StiffenerRegion;
	int m_iDTValue_StiffenerRegion;
	int	m_iClosing_StiffenerRegion;
	double m_dErosionParam_StiffenerBoundary;
	int m_iThresholdParam_SheildcanRegion;
	double m_dDilationParam_SheildcanBoundary;
	double m_dOpeningParam_CornerBoundary;	

	// Distance Measure
	BOOL m_bUseDistanceMeasure;
	int m_iDistanceMeasureUpperGv;
	int m_iDistanceMeasureUpperPos;
	double m_dDistanceMeasureUpperSmFactor;
	int m_iDistanceMeasureUpperEdgeStr;
	int m_iDistanceMeasureLowerGv;
	int m_iDistanceMeasureLowerPos;
	double m_dDistanceMeasureLowerSmFactor;
	int m_iDistanceMeasureLowerEdgeStr;
	int m_iDistanceMeasureUpperPosOffset;
	int m_iDistanceMeasureLowerPosOffset;
	int m_iDistanceMeasureUsageType;
	int m_iDistanceMeasureSaveID;
	double m_dDistanceMeasureGapInspectionUpper;
	double m_dDistanceMeasureGapInspectionLower;
	int m_iDistanceMeasureROIType;

	// Gap Measure
	BOOL m_bUseGapMeasure;
	int m_iGapMeasureUpperGv;
	int m_iGapMeasureUpperPos;
	double m_dGapMeasureUpperSmFactor;
	int m_iGapMeasureUpperEdgeStr;
	int m_iGapMeasureLowerGv;
	int m_iGapMeasureLowerPos;
	double m_dGapMeasureLowerSmFactor;
	int m_iGapMeasureLowerEdgeStr;
	int m_iGapMeasureUpperPosOffset;
	int m_iGapMeasureLowerPosOffset;
	int m_iGapMeasureSaveID;
	int m_iGapMeasureImageNo1;
	int m_iGapMeasureImageNo2;
	double m_dGapMeasureGapInspectionUpper;
	double m_dGapMeasureGapInspectionLower;
	double m_dGapMeasureGapMeasureOffset;
	int m_iGapMeasureDir;
	BOOL m_bGapMeasureAlarm;
	int m_iGapMeasurePointNumber;
	int m_iGapMeasureSelectType;
	BOOL m_bGapMeasureUpperLine;
	BOOL m_bGapMeasureLowerLine;
	BOOL m_bGapMeasureDiffImage;
	int m_iGapMeasurePosStart;
	int m_iGapMeasurePosEnd;
	int m_iGapMeasureImageNo3;
	BOOL m_bGapMeasureEndRemove;
	int m_iGapMeasureRemoveEndUpper;
	int m_iGapMeasureRemoveEndLower;
	BOOL m_bGapMeasureRetry;
	int m_iGapMeasureRetryImageNo;
	int	m_iGapMeasureRetryGv;
	int	m_iGapMeasureRetryPos;
	int	m_dGapMeasureRetrySmFactor;
	int	m_iGapMeasureRetryEdgeStr;
	int	m_iGapMeasureRetryPosOffset;

	// Surface Dual Inspection
	BOOL m_bUseSurfaceDualInspection;
	int m_iSurfaceDualImageIndex_1;
	int m_iSurfaceDualImageIndex_2;

	int m_iSurfaceDualDefectType_1;
	int m_iSurfaceDualDefectType_2;

	BOOL m_bUseBrightFixedThres_1;
	int m_iBrightLowerThres_1;
	int m_iBrightUpperThres_1;
	BOOL m_bUseBrightDT_1;
	int m_iBrightMedianFilterSize_1;
	int m_iBrightClosingFilterSize_1;
	int m_iBrightDTFilterType_1;
	int m_iBrightDTFilterSize_1;
	int m_iBrightDTFilterSize2_1;
	int m_iBrightDTValue_1;
	BOOL m_bUseDTPolarTrans_1;
	BOOL m_bUseBrightHystThres_1;
	int m_iBrightHystSecureThres_1;
	int m_iBrightHystPotentialThres_1;
	int m_iBrightHystPotentialLength_1;

	BOOL m_bUseBrightFixedThres_2;
	int m_iBrightLowerThres_2;
	int m_iBrightUpperThres_2;
	BOOL m_bUseBrightDT_2;
	int m_iBrightMedianFilterSize_2;
	int m_iBrightClosingFilterSize_2;
	int m_iBrightDTFilterType_2;
	int m_iBrightDTFilterSize_2;
	int m_iBrightDTFilterSize2_2;
	int m_iBrightDTValue_2;
	BOOL m_bUseDTPolarTrans_2;
	BOOL m_bUseBrightHystThres_2;
	int m_iBrightHystSecureThres_2;
	int m_iBrightHystPotentialThres_2;
	int m_iBrightHystPotentialLength_2;
	
	int m_iDualBlobProcessType;
	BOOL m_bDualBlobDefectConditionEach;

	// Neck Epoxy Measure
	BOOL m_bUseNeckEpoxyMeasure;
	int m_iNeckEpoxyMeasureTopEdgeStr;
	int m_iNeckEpoxyMeasureBottomEdgeStr;
	int m_iNeckEpoxyMeasureRightEdgeStr;
	int m_iNeckEpoxyToleranceHeightMax;
	int m_iNeckEpoxyToleranceHeightMin;
	int m_iNeckEpoxyToleranceLength;
	int m_iNeckEpoxyToleranceTopPosOffset;
	int m_iNeckEpoxyToleranceBottomPosOffset;
	int m_iNeckEpoxyInspectMethod;

	// Neck Epoxy Crack
	BOOL m_bUseNeckEpoxyCrack;
	int m_iNeckEpoxyCrackOpeningSize;
	int m_iNeckEpoxyCrackYLength;
	int m_iNeckEpoxyCrackBoundaryWidth;
	int m_iNeckEpoxyCrackDir;
	BOOL m_bNeckEpoxyCrackUseXDirMode; //VER 1.1.3.0 add

	// DNN
	int m_iDnnInspectionType;
	int m_iDnnInspectionModel;

	// Corner Measure
	BOOL m_bUseCornerMeasure;
	int m_iCornerMeasureSaveID;
	int m_iCornerMeasureMultiPointNumber;
	int m_iCornerMeasurePositionRange;
	int m_iCornerMeasureGv[2];
	int m_iCornerMeasurePos[2];
	double m_dCornerMeasureSmFactor[2];
	int m_iCornerMeasureEdgeStr[2];
	int m_iCornerMeasurePosOffset[2];
	double m_dCornerMeasureTargetSmFactor;
	int m_iCornerMeasureTargetEdgeStr;
	double m_dCornerMeasureTargetDistanceSpec;

	// Component
	BOOL m_bUseComponent;
	double m_dComponentScore;
	double m_dComponentShiftX;
	double m_dComponentShiftY;
	double m_dComponentRotationAngle;
	double m_dComponentRotationAngle2;
	BOOL m_bComponentApply_1;
	BOOL m_bComponentApply_2;
	BOOL m_bComponentApply_3;
	BOOL m_bComponentApply_4;
	BOOL m_bComponentApply_5;
	int m_iComponentDefectType1;
	int m_iComponentDefectType2;
	int m_iComponentDefectType3;
	int m_iComponentDefectType4;
	int m_iComponentDefectType5;

	// 색상 검사 기능
	BOOL m_bUseHSV;
	int	miLowerHue;
	int miUpperHue;
	int miLowerSaturation;
	int miUpperSaturation;

	// Height Measure
	BOOL m_bUseHeightMeasure;
	int m_iHeightMeasureType;
	
	// Boundary Inspection
	BOOL m_bUseBoundaryInspection;
	int m_iBoundaryEdgeMeasureDir;
	int m_iBoundaryEdgeMeasureGv;
	int m_iBoundaryEdgeMeasurePos;
	double m_dBoundaryEdgeMeasureSmFactor;
	double m_dBoundaryEdgeMeasureEdgeStr;
	int m_iBoundaryEdgeMeasureSampling;
	int m_iBoundaryEdgeMeasureSensorLength;
	BOOL m_bBoundaryCondition[3];
	int m_iBoundaryMedianFilterSize[2];
	double m_dBoundaryDefectDepth[3];
	double m_dBoundaryDefectMBDepth[2];
	double m_dBoundaryDefectLength[3];
	int m_iBoundaryDisplay;
	BOOL m_bBoundaryUseConnection[2];
	int m_iBoundaryConnectionLength[2];

	BOOL m_bBoundaryUseRoiConnected;

public:
	void Save(HANDLE hFile, int iVisionIdx, int iImageIdx, int iROINo, int iInspectionTabIdx);
	BOOL Load(HANDLE hFile, int iModelVersion, BOOL bCreateReinforceModel);

	void Reset();

	CAlgorithmParam& operator=(CAlgorithmParam& data);
};

