#pragma once

#include "GTRegion.h"
#include "BatchService/BatchDefine.h"
#include "FAIDefine.h"
#include "AlgorithmService/AlgorithmContext.h"

// FAI Forward Declaration
class IFAIInspector;

#ifdef USE_SUAKIT
struct TCPParam
{
	int iClientNO;
	BYTE* pbyteImg;
	int imgSize;
	int DeepLearningModelID;
	CString LotID;
	int TrayNO;
	int ModuleNO;
	CString FileName;
	int DefectCnt;
	int milsec;
	int nThreadNO;
};
#endif

//typedef struct
//{
//	int iInspectionBufferIdx;
//} INSPECT_THREAD_PARAM;

typedef struct
{
	int iInspectionBufferIdx;
} INSPECT_THREAD_PARAM;

typedef struct
{
	int iScanBufferIdx;
} SCAN_THREAD_PARAM;

typedef struct _THREAD_PARAM {
	_THREAD_PARAM(int iThreadIdx, int iVisionCamIdx)
	{
		this->iThreadIdx = iThreadIdx;
		this->iVisionCamIdx = iVisionCamIdx;
	}

	int iThreadIdx;
	int iVisionCamIdx;
} THREAD_PARAM;

typedef struct _SAVE_THREAD_PARAM {
	_SAVE_THREAD_PARAM(int iVisionCamIdx, int iInspectionBufferIdx, int iThreadIdx)
	{
		this->iVisionCamIdx = iVisionCamIdx;
		this->iInspectionBufferIdx = iInspectionBufferIdx;
		this->iThreadIdx = iThreadIdx;
	}

	int iVisionCamIdx;
	int iInspectionBufferIdx;
	int iThreadIdx;
} SAVE_THREAD_PARAM;

typedef struct _RAW_IMAGE_SAVE_PARAM_CAM
{
	int iPcVisionNo;
	int iVisionCamType;
	int iMzNo;
	int iTrayNo;
	int iModuleNo;
	int iNoInspectImage;
	CString sBarcodeID;
	HObject HSaveImage[MAX_IMAGE_TAB];
} RAW_IMAGE_SAVE_PARAM_CAM;

typedef struct _BARCODE_TRANSFER_THREAD_PARAM {
	_BARCODE_TRANSFER_THREAD_PARAM(int iVisionCamIdx, int iScanBufferIdx, int iGrabCircularBufferIdx)
	{
		this->iVisionCamIdx = iVisionCamIdx;
		this->iScanBufferIdx = iScanBufferIdx;
		this->iGrabCircularBufferIdx = iGrabCircularBufferIdx;
	}

	int iVisionCamIdx;
	int iScanBufferIdx;
	int iGrabCircularBufferIdx;
} BARCODE_TRANSFER_THREAD_PARAM;

class Algorithm
{
public:

	static Algorithm	*m_pInstance;
	static	Algorithm* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	Algorithm(void);
	~Algorithm(void);

	void Initialize(int iVisionCamType);

	CString strLog;
public:
	int		m_iPcVisionNo;
	int		m_iVisionCamType;
	int		m_iNoInspectImage;

	int		m_iPrevTrayNo;
	CString m_sPrevLotID;

	HObject m_HInspectCImage[INSPECTION_BUFFER_COUNT_MAX][CHANNEL_NUM][MAX_IMAGE_TAB];
	HObject m_HInspectSpecularRawImage[INSPECTION_BUFFER_COUNT_MAX][DFM_VISION_MAX_CONVERSION][SPV_RAW_IMAGE_NUMBER];
	HObject m_HInspectDiffusedRawImage[INSPECTION_BUFFER_COUNT_MAX][DIFFUSED_VISION_MAX_CONVERSION][DIFFUSED_RAW_IMAGE_NUMBER];
	HObject m_HInspectSpecularBackgroundImage[INSPECTION_BUFFER_COUNT_MAX][DFM_VISION_MAX_CONVERSION];
	HObject m_HInspectDiffusedBackgroundImage[INSPECTION_BUFFER_COUNT_MAX][DIFFUSED_VISION_MAX_CONVERSION];

	CCriticalSection	m_csInspectDone[INSPECTION_BUFFER_COUNT_MAX];
	int	m_iInspectBufferStatus[INSPECTION_BUFFER_COUNT_MAX];	// sync
	BOOL IsInspectDone(int iBufferIdx);
	BOOL IsPosProcessingDone(int iBufferIdx);
	void ChangeInspectDone(int iBufferIdx, int iStatus);

	int GetAvailableInspectBufferCount();

	CString m_sLotID[INSPECTION_BUFFER_COUNT_MAX];
	int		m_nMzNo[INSPECTION_BUFFER_COUNT_MAX];
	int		m_nStageNo[INSPECTION_BUFFER_COUNT_MAX];
	int		m_nJigNo[INSPECTION_BUFFER_COUNT_MAX];
	int		m_nTrayNo[INSPECTION_BUFFER_COUNT_MAX];
	int		m_nModuleNo[INSPECTION_BUFFER_COUNT_MAX];
	BOOL	m_bGrabFail[INSPECTION_BUFFER_COUNT_MAX];
	BOOL	m_bMatchingSuccess[INSPECTION_BUFFER_COUNT_MAX];
	double	m_dGlobalAlignDeltaX[INSPECTION_BUFFER_COUNT_MAX][MAX_IMAGE_TAB];
	double	m_dGlobalAlignDeltaY[INSPECTION_BUFFER_COUNT_MAX][MAX_IMAGE_TAB];
	BOOL	m_bLightDisorder[INSPECTION_BUFFER_COUNT_MAX];
	int		m_iNoGrabRetry[INSPECTION_BUFFER_COUNT_MAX];
	int		m_iNoFocusMoveRetry[INSPECTION_BUFFER_COUNT_MAX];

	CCriticalSection	m_csScanBufferIdx;
	int		m_iScanBufferIdx;

	////////////////////////////////// Inspection Thread //////////////////////////////////////////
	CCriticalSection	m_csAlgorithmThreadRunning[NO_MAX_INSPECT_THREAD];
	BOOL m_bAlgorithmThreadRunning[NO_MAX_INSPECT_THREAD];		// sync
	BOOL IsAlgorithmThreadRunning(int iThreadIdx);
	void ChangeAlgorithmThreadRunning(int iThreadIdx, BOOL bRunning);

	CCriticalSection	m_csResultSaveThreadRunning;
	BOOL m_bResultSaveThreadRunning;	// sync
	BOOL IsResultSaveThreadRunning();
	void ChangeResultSaveThreadRunning(BOOL bRunning);

	CSemaphore*	m_pSemaphore;

	CCriticalSection	m_csInspect;
	BOOL IsInspectThreadParamEmpty();
	void AddListInspectThreadParam(INSPECT_THREAD_PARAM *pParam);
	void RemoveListInspectThreadParams();
	INSPECT_THREAD_PARAM* GetNextInspectThreadParam();

	CCriticalSection	m_csScan;
	BOOL IsScanThreadParamEmpty();
	void AddListScanThreadParam(SCAN_THREAD_PARAM *pParam);
	void RemoveListScanThreadParams();
	SCAN_THREAD_PARAM* GetNextScanThreadParam();

	BOOL m_bRawImageSaveThreadRunning_Cam[IMAGE_SAVE_THREAD_MAX_QUEUE];
	CCriticalSection	m_csSaveRawImage_Cam;
	BOOL IsSaveRawImageParamEmpty_Cam(int nIndex);
	void AddListSaveRawImageParam_Cam(RAW_IMAGE_SAVE_PARAM_CAM *pParam, int nIndex);
	void RemoveListSaveRawImageParams_Cam(int nIndex);
	RAW_IMAGE_SAVE_PARAM_CAM* GetNextSaveRawImageParam_Cam(int nIndex);

	int m_nRawImageQueueIndex;
	BOOL bFreezingClearCheck[4];

	BOOL CopyRawImage(int iInspectionBufferIdx);

	////////////////////////////////// Gap ///////////////////////////////////////////////////
	HObject m_HGapMeasureXLD;
	HTuple m_HGapDistanceMeasureResult;

	void InitGapResult();
	//////////////////////////////////////////////////////////////////////////

	////////////////////////////////// Corner ///////////////////////////////////////////////////
	HObject m_HCornerMeasureXLD;
	void InitCornerResult();


	CCriticalSection	m_csScreenUpdate;

	////////////////////////////////// Conformal Mini Distance Measurement ///////////////////////////////////////////////////

	double m_dDistanceMiniConformalHeight;
	double m_dCCMH_CenterX;
	double m_dCCMH_CenterY;
	HObject m_HMiniConformalHeightEndLineXLD;

	void InitMiniConformalHeightResult();
	//////////////////////////////////////////////////////////////////////////

	////////////////////////////////// Conformal Distance Measurement ///////////////////////////////////////////////////

	double m_dDistanceConformalHeight;
	double m_dCCLH_CenterX;
	double m_dCCLH_CenterY;
	HObject m_HConformalHeightEndLineXLD;

	void InitConformalHeightResult();
	//////////////////////////////////////////////////////////////////////////

	////////////////////////////////// Homer Tilt ///////////////////////////////////////////////////
	double m_dHomerTiltHeight;
	double m_dHomerTiltBenvolioDist;

	void InitHomerTiltResult();
	//////////////////////////////////////////////////////////////////////////

	////////////////////////////////// OCR ///////////////////////////////////////////////////
	BOOL m_bOcrRegResult;		// OCR 처리 오류 여부 (검사 시)
	CString m_sOcrResult;		// 바코드 순서 문자 인식 결과 (검사 시)
	CString m_strOCRChar;		// 영상 디스플레이된 위치대로 결과
	HObject m_hConcatCharRgn;	// 프로세싱용
	HObject m_HOCRDetectRgn;	// OCR Seg 결과 (검사 시)
	BOOL m_bManualBarcodeForOcrInspect; //2017.08.08 hbk OCR 메뉴얼검사시 필요한 바코드 검사 

	void InitOcrResult();
	//////////////////////////////////////////////////////////////////////////

	double m_dFitLineRotationAngle;

	double m_dDistanceArray[INSPECTION_BUFFER_COUNT_MAX][5];
	DPOINT m_dDatumBCornerPoint[INSPECTION_BUFFER_COUNT_MAX][4];
	double m_dHeightArray[INSPECTION_BUFFER_COUNT_MAX][20];
	DPOINT m_dHeightMeasureCPointArray[INSPECTION_BUFFER_COUNT_MAX][20];

	// L10
	HObject m_HDefectRgn_DefectName[INSPECTION_BUFFER_COUNT_MAX][MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];						// 검사결과 생성 시 사용 
	HObject m_HDefectRgn_InspectionType[INSPECTION_BUFFER_COUNT_MAX][MAX_INSPECTION_TYPE];									// Result Image 생성 시 사용
	HObject m_HDefectRgn_ImageNo[INSPECTION_BUFFER_COUNT_MAX][MAX_IMAGE_TAB];												// ADJ에서 사용
	HObject m_HMeasureRgn_FAI_Item[INSPECTION_BUFFER_COUNT_MAX][MAX_FAI_ITEM][MAX_FAI_PARAMETER];							// FAI 측정 영역 (Alogrithm)
	HObject m_HDefectRgn_FAI[INSPECTION_BUFFER_COUNT_MAX];																	// 모듈단위 FAI NG 영역
	HObject m_HReviewXLD_FAI_Item[INSPECTION_BUFFER_COUNT_MAX][MAX_FAI_ITEM][MAX_FAI_PARAMETER][MAX_FAI_REVIEW_IMAGE];		// FAI Review Image 측정 영역 (Algorithm, Result)
	POINT	m_iViewportCenter_FAI_Item[INSPECTION_BUFFER_COUNT_MAX][MAX_FAI_ITEM][MAX_FAI_PARAMETER][MAX_FAI_REVIEW_IMAGE];	// FAI Review Image Viewport Center Point (Algorithm, Result)

	HObject m_HDefectRgn_MultiFocus_Ring_Particle[INSPECTION_BUFFER_COUNT_MAX][MAX_IMAGE_TAB][MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];		// Multi-Focus 처리
	HObject m_HDefectRgn_Lens_Ring[INSPECTION_BUFFER_COUNT_MAX];															// Multi-Focus 처리
	HObject m_HDefectRgn_ImageNo_Coax[INSPECTION_BUFFER_COUNT_MAX][MAX_IMAGE_TAB];											// Multi-Focus 처리
	HObject m_HDefectRgn_DefectName_Coax[INSPECTION_BUFFER_COUNT_MAX][MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];				// 검사결과 생성 시 사용 

	int		m_iAlignNoInspectModule[INSPECTION_BUFFER_COUNT_MAX];
	int		m_iAlignNoInspectEmpty[INSPECTION_BUFFER_COUNT_MAX];
	HObject m_HAlignResultRgn[INSPECTION_BUFFER_COUNT_MAX];
	HTuple	m_HAlignModuleExist[INSPECTION_BUFFER_COUNT_MAX];
	HTuple	m_HAlignModuleIsDefect[INSPECTION_BUFFER_COUNT_MAX];
	HTuple	m_HAlignModuleDefectPosX[INSPECTION_BUFFER_COUNT_MAX];
	HTuple	m_HAlignModuleDefectPosY[INSPECTION_BUFFER_COUNT_MAX];
	HTuple	m_HAlignModuleDeltaX[INSPECTION_BUFFER_COUNT_MAX];
	HTuple	m_HAlignModuleDeltaY[INSPECTION_BUFFER_COUNT_MAX];
	HTuple	m_HAlignModuleDeltaAngle[INSPECTION_BUFFER_COUNT_MAX];

	HObject m_HAlignResultRgn_EachImage[ALIGN_CAM_GRAB_NUMBER];

	void InitAlgorithmInspectParam ();

	BOOL ImageAlignShapeMatching(BOOL bImageTransform, int iMzNo, HObject HMatchingImage, HObject *pHAlignImage, HTuple *plAlignModelID, BOOL *pbAlignImageFlag, double dMatchingAngleRange, double dMatchingScaleMin, double dMatchingScaleMax,
		double dMatchingScore, double dTeachAlignRefX, double dTeachAlignRefY, int iMatchingMethod, int iMatchingLTX, int iMatchingLTY, int iMatchingRBX, int iMatchingRBY, int iMatchingSearchMarginX, int iMatchingSearchMarginY, BOOL bMatchingApplyAffineConstant,
		int iPcVisionNo, int iVisionCamType, int iCurThreadIdx, int iMatchingImageIndex, BOOL bUseMatchingXYComp, int iMatchingImageXYRef,int iMatchingXYRefLine, BOOL bUseMatchingAngleComp, int iMatchingAngleRefLine, double *pdRotationAngleDeg, double *pdDeltaX, double *pdDeltaY, HTuple *pHMatchingHomMat=NULL);

	BOOL ImageTransformUsingHomMat(int iMatchingImageIndex, int iAffineTransImageIndex, HObject *pHAlignImage, BOOL *pbAlignImageFlag, int iMatchingMethod, BOOL bMatchingApplyAffineConstant, HTuple *pHMatchingHomMat);

	void SrunmedSLI(float* y, float* smo, long n, int bw_tot, int end_rule, int skip);
	//void Run_Median_Stuetzle(float* data, int length, int window, float* out_data);
	//int Run_Med_Idx(int idx, int cn, int length);

	int  Run_Med_Idx(int idx, int cn, int length, bool isClosed);
	void Run_Med_OneCycle(float* data, int length, int wn, int cn, bool isClosed, bool write_output, float& med_curr, float* out_data, int& pos_out);
	void Run_Median_Stuetzle(float* data, int length, int window, float* out_data, bool isClosed = false);

	BOOL TrainVariationModelWindow(HObject HTrainImage, GTRegion *pInspectROIRgn, int iInspectTabIdx, int iWindowSize);

public:
	BOOL CopyInspectInformation(const AlgorithmCopyParam& param);

	BOOL BlobUnion(HObject *pInputRgn, long lMergeLength);
	BOOL BlobUnionAngle(HObject *pInputRgn, long lMergeLength, long lOrientationMinDiameter);
	BOOL BlobLengthWidthCondition(HObject *pInputRgn, BOOL bUseLengthCondition, BOOL bUseWidthCondition, int iLengthValue, int iWidthValue);

	BOOL GetPreLocalAlignResult(int iMzNo, int iPcVisionNo, CAlgorithmParam AlgorithmParam, HObject *pHROIHRegion, int iThreadIdx, int *piLoaclAlignDeltaX, int *piLoaclAlignDeltaY);
	BOOL ApplyLocalAlignResult(int iMzNo, int iPcVisionNo, CAlgorithmParam AlgorithmParam, HObject *pHROIHRegion, int iThreadIdx, double *dLocalAlignAngle = nullptr, int *iLocalAlignDeltaX = nullptr, int *iLocalAlignDeltaY = nullptr);
	BOOL ApplyDontCareResult(int iMzNo, int iPcVisionNo, CAlgorithmParam AlgorithmParam, HObject *pHROIHRegion, int iThreadIdx);
	BOOL ApplyGenerateResult(int iMzNo, int iPcVisionNo, CAlgorithmParam AlgorithmParam, HObject *pHROIHRegion, int iThreadIdx);
	BOOL ApplyMaskResult(int iMzNo, int iPcVisionNo, CAlgorithmParam AlgorithmParam, HObject *pHROIHRegion, int iThreadIdx);

	BOOL LeastSquareCircleFitting(HTuple X, HTuple Y, double dNormalRadius, double *pdCenterX, double *pdCenterY, double *pdRadius);
	BOOL SurfacePlane_Point(DPOINT dR0, DPOINT dR1, DPOINT dR2, double dH0, double dH1, double dH2, DPOINT dMeasurePoint, double *dGrayHeight);
	BOOL ApproxPlane_NPoint(int iNoRPoint, DPOINT *pdRPoint, double *pdRHeight, double *pdA, double *pdB, double *pdC);
	BOOL SurfacePlane_NPoint(double dA, double dB, double dC, DPOINT dMeasurePoint, double *dGrayHeight);
	BOOL ConvertProfileToByteImage(HObject HProfileImage, HObject *pHByteImage);

	BOOL SelectShapeUsingMinLength(HObject HInputRgn, int iMinLengthPxl, int iMaxLengthPxl, int iRefDir, BOOL bIgnoreHole, HObject *pHSelectedShapeRgn, int *piMinMeasureLength);
	BOOL SelectShapeUsingMaxLength(HObject HInputRgn, int iMinLengthPxl, int iMaxLengthPxl, int iRefDir, BOOL bIgnoreHole, HObject *pHSelectedShapeRgn, int *piMaxMeasureLength);
	BOOL SelectShapeUsingOrthoLength(HObject HInputRgn, HObject HInspectRgn, int iMinLengthPxl, int iMaxLengthPxl, int iRefDir, HObject *pHSelectedShapeRgn, double *pdMaxMeasureLength);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	///
	///   Go 추가 (2016/06/02)
	///
	//////////////////////////////////////////////////////////////////////////
	BOOL GetScaledImage(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHScaledImage);
	BOOL GetEdgeImage(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHEdgeImage);
	BOOL GetPreprocessImage(BOOL bUseDomain, HObject HOrgImage, HObject *pHImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHArithmeticImage);
	BOOL GetMatchingPreprocessImage(HObject HOrgImage, int iImageProcessFilterType, double dImageProcessFilterTypeX, double dImageProcessFilterTypeY, HObject *pHArithmeticImage);
	BOOL GetShapeImage(BOOL bImageCrop, HObject *pHImage, int *piShapeRawImageIdx, HObject HROIRgn, CAlgorithmParam Param, HObject *pHShapeImage);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	///
	///   L10 추가 (2017/01/27)
	///
	//////////////////////////////////////////////////////////////////////////
	BOOL GetInspectArea(HObject HROIRgn, CAlgorithmParam Param, HObject *pHInspectArea);

	HObject InspectAVI(int iInspectBufferIndex, int iThreadIdx, GTRegion *pInspectROIRgn, int iInspectTabIdx, HObject HImage, HObject HROIRgn, CAlgorithmParam AlgorithmParam, HObject *pHResultXLD, HObject *pHInspectImage, HObject HDCImage, HObject *pHMaskRgn = NULL, double *pdResultDataAry = NULL, int iMatchingPosOffsetX = 0, int iMatchingPosOffsetY = 0);

	HObject SurfaceInspectionAlgorithm(int iInspectBufferIndex, HObject HImage, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param, int *piArea, double *pdXLength, double *pdYLength, HObject *pHMaskRgn, double *pdResultDataAry, double *pdLength=NULL, double *pdBlobCenterX=NULL, double *pdBlobCenterY=NULL, double *pdBlobCenterX2 = NULL, double *pdBlobCenterY2 = NULL);
	HObject SurfaceDualInspectionAlgorithm(HObject *pHImage1, HObject *pHImage2, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHMaskRgn=NULL);
	HObject GetNonInspectionAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param);
	
	HObject ImageCompareAlgorithm(int iThreadIdx, int iMzNo, GTRegion *pInspectROIRgn, int iInspectTabIdx, HObject HImage, HObject HROIRgn, CAlgorithmParam Param, int *piArea, double *pdXLength, double *pdYLength, HObject *pHMaskRgn, HObject *pHVariationModelInspectImage);
	HObject ComponentAlgorithm(int iThreadIdx, int iMzNo, GTRegion *pInspectROIRgn, int iInspectTabIdx, HObject HImage, HObject HROIRgn, CAlgorithmParam Param, int *piComponentDefectType, HObject *pHMaskRgn = NULL);
	HObject DefectContidion(int iInspectBufferIndex, HObject HImage, HObject HInspectAreaRgn, HObject HBrightRgn, HObject HDarkRgn, CAlgorithmParam Param, int *piArea, double *pdXLength, double *pdYLength, double *pdLength=NULL);
	HObject DefectContidionDC(HObject HImage, HObject HInspectAreaRgn, HObject HBrightRgn, HObject HDarkRgn, CAlgorithmParam Param, int *piArea, int *piXLength, int *piYLength);
	int FindHistogramPeak(HObject HImage, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param);

	HObject LineFittingAlgorithm(HObject HImage, HObject *pHFitImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, double *pdBurMaxLengthPxl, double *pdChippingMaxLengthPxl, int *piBurArea, int *piChippingArea, int *piFitLineXPos, int *piFitLineYPos);
	BOOL BarcodeAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, int iInspectBufferIndex, HObject *pHBarcodeOtherImage);
	HObject BarcodePrintQualityAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, double *pdPrintQualityGrade, CString *pdPrintQualityValue);
	HObject ConnectorPinDeformAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param);
	HObject DentAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param);
	HObject HeightMeasureAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, double *pdHeight);
	HObject BoundaryInspectionAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, BOOL bTeachMode);
	int BoundaryInspectionAutoCalibration(HObject HImage, HObject HROIRgn, CAlgorithmParam* pCalParam);
	HObject BoundaryInspectionAssessment(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, double* pdScore);
	HObject EdgeMeasureAlgorithm(HObject HImage, HObject *pHOtherImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, int *piFitLineXPos, int *piFitLineYPos, double *pdEdgeCenterX, double *pdEdgeCenterY, double *pdLineStartX = NULL, double *pdLineStartY = NULL, double *pdLineEndX = NULL, double *pdLineEndY = NULL);
	HObject DistanceMeasureAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, double *pdDistanceMeasureValueUm, int *iGapUpperPointX, int *iGapUpperPointY, int *iGapLowerPointX, int *iGapLowerPointY);
	HObject GapMeasureAlgorithm(HObject *pHImage1, HObject *pHImage2, HObject *pHImage3, HObject *pHImage4, HObject *pHImageRetry, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, double *pdGapMeasureValueUm, int *iGapUpperPointX, int *iGapUpperPointY, int *iGapLowerPointX, int *iGapLowerPointY);
	HObject CornerMeasureAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, double dPixelResMm, HObject *pHLineFitXLD, double *pdLineStartX, double *pdLineStartY, double *pdLineEndX, double *pdLineEndY, double *pdDistanceMm);
	HObject HomerTiltAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHBenvolioEdgeMeasureImage);
	HObject EpoxyVoidHoleAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHBenvolioHtccEdgeMeasureImage, BOOL *pbDefect);
	HObject NeckEpoxyMeasureAlgorithm(HObject HImage, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, HObject *pHMaskRgn,
		double *pdEpoxyMeasureValue_TopOffset, double *pdEpoxyMeasureValue_BottomOffset, double *pdEpoxyMeasureValue_Length, double *pdEpoxyMeasureValue_HeightMax, double *pdEpoxyMeasureValue_HeightMin);
	HObject NeckEpoxyCrackAlgorithm(HObject HImage, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, HObject *pHMaskRgn);
	HObject NeckEpoxyCrackAlgorithm_XDir(HObject HImage, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, HObject *pHMaskRgn);

	HObject ROIAlignAlgorithm(HObject HImage, HObject HInspectAreaRgn, CAlgorithmParam Param, HTuple* pHModelID, BOOL* pbShiftResult, double* pdScore, int* piLocalAlignDeltaX = NULL, int* piLocalAlignDeltaY = NULL, double* pdLocalAlignDeltaAngle = NULL, double* pdLocalAlignDeltaAngleFixedPointX = NULL, double* pdLocalAlignDeltaAngleFixedPointY = NULL, int iMatchingPosOffsetX = 0, int iMatchingPosOffsetY = 0, double* pdLocalAlignFindX = NULL, double* pdLocalAlignFindY = NULL);
	HObject ROIAnisoAlignAlgorithm(HObject HImage, HObject HInspectAreaRgn, CAlgorithmParam Param, HTuple* pHModelID, BOOL* pbShiftResult, double* pdScore, int* piLocalAlignDeltaX = NULL, int* piLocalAlignDeltaY = NULL, double* pdLocalAlignDeltaAngle = NULL, double* pdLocalAlignDeltaAngleFixedPointX = NULL, double* pdLocalAlignDeltaAngleFixedPointY = NULL, int iMatchingPosOffsetX = 0, int iMatchingPosOffsetY = 0);// 비등방 CHL
	HObject PartCheckAlgorithm(HTuple HPartModelID, HObject HImage, HObject HROIRgn, CAlgorithmParam Param, int* piLocalAlignDeltaX, int* piLocalAlignDeltaY, double* pdLocalAlignDeltaAngle, double* dPartCheckLocalAlignDeltaAngleFixedPointX, double* dPartCheckLocalAlignDeltaAngleFixedPointY, double* pdShiftX, double* pdShiftY, double* pdScore, HObject* pHAlignRgn, int iMatchingPosOffsetX = 0, int iMatchingPosOffsetY = 0);
	HObject ROIAlignAlgorithm_MultiModule_Align(HObject HImage, HObject HInspectAreaRgn, CAlgorithmParam Param, HTuple* pHModelID, int iNoInspectModule, int iInspectBufferIndex);
	HObject ROIAnisoAlignAlgorithm_MultiModule_Align(HObject HImage, HObject HInspectAreaRgn, CAlgorithmParam Param, HTuple *pHModelID, int iNoInspectModule, int iInspectBufferIndex);
	HObject PartCheckAlgorithm_MultiModule_Align(HTuple HPartModelID, HObject HImage, HObject HROIRgn, CAlgorithmParam Param, int iNoInspectModule, int iInspectBufferIndex);

	HObject StiffenerEpoxyInspectROIAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param);

	int ApplyInspectionCondition(int iVisionCamType, HObject HDefectRgn, HObject *HConcatDefect,
		double dMinAreaCondition, double dMidAreaCondition, double dMaxAreaCondition, int iMinNumAccept, int iMidNumAccept, int iMaxNumAccept,
		CString strPCID, CString strEquipNo, CString strModelName, int iMzNo, CString strLotID, int iTrayNo, int iModuleNo, CString strModuleBarcodeID,
		CString strVisionNameShort, int iPcNo, int iStageNo, CString strInspectionTypeShort, CString strDefectTypeShort);

	BOOL OCRAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param);


	void InspectFAI(BOOL bIsAutoInsp = FALSE, int iInspectionBufferIdx = 0, int iFAIParamNo = 0);

	// 측정 분석
	//double MeasureAnalysis(HObject HImage, HObject HInspROI, int iAnalysisType);

	// 8 Point 사용 4 직선 교점 사각형 중심 좌표 알고리즘
	HObject SquareCenterAlgorithm(double dA1PointX, double dA1PointY, double dA2PointX, double dA2PointY, double dB1PointX, double dB1PointY, double dB2PointX, double dB2PointY,
		double dC1PointX, double dC1PointY, double dC2PointX, double dC2PointY, double dD1PointX, double dD1PointY, double dD2PointX, double dD2PointY, double &dCPointX, double &dCPointY);

	HObject SquareCenterAlgorithm(DPOINT dPoint[8], double &dCPointX, double &dCPointY);

	// 2 Contour 최단거리 좌표 알고리즘
	HObject DistanceMinPointAlgorithm(HObject Contour1, HObject Contour2, double& dC1PointX, double& dC1PointY, double& dC2PointX, double& dC2PointY, double& dDistanceMin);

	// 3 Point & Angle 축변환 알고리즘
	BOOL AxisTransAlgorithm(BOOL bIsTransAxisY, double dRotateAngle, double dOPointX, double dOPointY, double& dAxisXEndX, double& dAxisXEndY, double& dAxisYEndX, double& dAxisYEndY);

	// 이미지 수평 합성 알고리즘
	HObject BlendingAlgorithm_Horizontal(double dRefer1HStartPosX, double dRefer1HEndPosX, double dRefer2HStartPosX, double dRefer2HEndPosX,
		double dImage1HStartPosX, double dImage1HEndPosX, double dImage2HStartPosX, double dImage2HEndPosX, double dImage1VStartPosX, double dImage1VStartPosY, double dImage1VEndPosX, double dImage1VEndPosY,
		double dImage2VStartPosX, double dImage2VStartPosY, double dImage2VEndPosX, double dImage2VEndPosY, double& dBlendResultX, double& dBlendResultY, double& dBlendOffsetLeft, double& dBlendOffsetRight,
		HObject HImage1 = HObject(), HObject HImage2 = HObject());

	void SaveADJImage( int nThreadNO,
		int iInspectionBufferIdx,
		int iVisionCamType,
		HObject HVisionAllDefectRgn,
		int iPcVisionNo,
		int iNoInspectImage,
		int iMzNo,
		int iTrayNo,
		int iModuleNo,
		CString sBarcodeID,
		DWORD iModuleTotalStartTime,
		HObject &HADJFilteredDefectRgn,
		int &iInspectionTypeResult);

#ifdef USE_SUAKIT

	//void ResetADJResult(int iDeepLearningModelNO,int iModuleNO, int nDefectCnt);
	//int AssignDeepLearningModel(int iClientNO, int iVisionCamType,CString sADJInspectionTypeCode,CString sADJDefectDetail);
	
	/*int m_nADJDefectCnt_N1;
	int m_nADJDefectCnt_N2;
	int m_nADJDefectCnt_N3;*/

	//CString GetDeepLearningModelNameFromIndex( int nDeepLearningModelID );
//	CString returnSocketErrorString(int nError);
#endif

	// OCR Log
	void WriteOcrandBarcodeInfo(CFile* file, SYSTEMTIME time);
	int OutPutWidthCharacterCalculate(int iType, CAlgorithmParam Param, long lWholeWidth, double lRowF , double lColF, double lRowS, double lColS);
	HObject CalculateWholeOcrArea(HObject HRectangleWholeArea, CAlgorithmParam Param);

	//210826 jwj add
	CRITICAL_SECTION		m_csRawImageSavingDone;
	void    SetCopyThreadRun(int nThreadIdx, BOOL b) { EnterCriticalSection(&m_csRawImageSavingDone); m_bCopyThreadRun[nThreadIdx] = b; LeaveCriticalSection(&m_csRawImageSavingDone); }
	BOOL    m_bCopyThreadRun[NO_MAX_INSPECT_THREAD];
	//210826 jwj add end

	CEvent	m_evCopyForSavingDone[NO_MAX_INSPECT_THREAD];	//AutoReset 
	CEvent	m_evCopyForBarcodeDone[GRAB_CIRCULAR_MAX];	//AutoReset 
	CEvent	m_evCopyForModuleCheckDone[GRAB_CIRCULAR_MAX];	//AutoReset 
	CEvent m_evBarcodeReadDone[GRAB_CIRCULAR_MAX]; 

protected:
	std::deque<INSPECT_THREAD_PARAM*> m_listInspectThreadParam;
	std::deque<SCAN_THREAD_PARAM*> m_listScanThreadParam;
	std::deque<RAW_IMAGE_SAVE_PARAM_CAM*> m_listRawImageSaveParam_Cam[IMAGE_SAVE_THREAD_MAX_QUEUE];
};

