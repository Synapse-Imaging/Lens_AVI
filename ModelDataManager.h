#pragma once

#include "FileBase.h"
#include "GTRegion.h"
#include "LightControl.h"
#include "FAIDefine.h"

class CModelDataManager : public CFileBase
{
public:

	static CModelDataManager	*m_pInstance;
	static CModelDataManager* GetInstance(BOOL bShowFlag=FALSE);
	void DeleteInstance();
	CModelDataManager(void);
	~CModelDataManager(void);

	void SetModelIdx(int iIdx) { m_iModelIdx = iIdx; }
	int GetModelIdx() { return m_iModelIdx; }

private:
	int m_iModelIdx;

public:

	void InitModelData();

	CString m_sModelName;
	int m_iModelVersion;
	int m_iTrayArrayX, m_iTrayArrayY;
	int m_iTrayModuleMax;			// 트레이 용량
	int m_iTrayModuleAmt;			// 트레이 상의 시료 개수 = 트레이 용량 - 트레이 bottom right에서 비어있는 시료의 수

	int m_iNoUsedImageGrab;
	double m_dLightZPositionRef[STAGE_NUMBER_MAX];
	double m_dTiltPositionRef[STAGE_NUMBER_MAX];
	double m_dRotatePositionRef[STAGE_NUMBER_MAX];
	double m_dStageXPositionRef[STAGE_NUMBER_MAX];
	double m_dStageYPositionRef[STAGE_NUMBER_MAX];
	std::vector<double> m_dZFocusPosition[STAGE_NUMBER_MAX];		// Each Vision Camera Z Focus Position
	std::vector<double> m_dLightZPosition[STAGE_NUMBER_MAX];		// Each Vision Light Z Position
	std::vector<double> m_dTiltPosition[STAGE_NUMBER_MAX];		// Each Vision Tilt Position
	std::vector<double> m_dRotatePosition[STAGE_NUMBER_MAX];		// Each Vision Rotate Position
	std::vector<double> m_dStageXPosition[STAGE_NUMBER_MAX];		// Each Vision Head X Position
	std::vector<double> m_dStageYPosition[STAGE_NUMBER_MAX];		// Each Vision Stage Y Position

	int m_iTriggerImageNumber;
	int m_iTriggerPeriod;

	BOOL m_bUseDiffusedImage[MAX_IMAGE_TAB];
	BOOL m_bUseSpecularImage[MAX_IMAGE_TAB];
	BOOL m_bUseSeqStartImage[MAX_IMAGE_TAB];
	int m_iSeqAddrIndex[STAGE_NUMBER_MAX][MAX_IMAGE_TAB];

	BOOL m_bUseSpecularRoiCrop[MAX_IMAGE_TAB];
	double m_dSpecularResizeFactor[MAX_IMAGE_TAB];

	long m_lSpecularCropLTPointX_Roi[MAX_IMAGE_TAB];
	long m_lSpecularCropLTPointY_Roi[MAX_IMAGE_TAB];
	long m_lSpecularCropRBPointX_Roi[MAX_IMAGE_TAB];
	long m_lSpecularCropRBPointY_Roi[MAX_IMAGE_TAB];

	int m_iDFMLightIntensity1;
	int m_iDFMLightIntensity2;
	double m_dDFMPsx;
	double m_dDFMPsy;
	double m_dDFMTex;
	double m_dDFMTey;
	double m_dDFMSpecularImageSF[DFM_MAX_SPECULAR_IMAGE_NUMBER];
	int m_iDFMMeanFilterSize;
	double m_dDFMHigh[DFM_MAX_SHAPE_IMAGE_NUMBER];
	double m_dDFMLow[DFM_MAX_SHAPE_IMAGE_NUMBER];
	double m_dDFMDerivativeSmFactor[DFM_MAX_SHAPE_IMAGE_NUMBER];
	double m_dDiffusedHigh[DIFFUSED_MAX_SHAPE_IMAGE_NUMBER];
	double m_dDiffusedLow[DIFFUSED_MAX_SHAPE_IMAGE_NUMBER];
	double m_dDiffusedSmFactor[DIFFUSED_MAX_SHAPE_IMAGE_NUMBER];

	double m_dZigCenterCamZ[STAGE_NUMBER_MAX];
	BOOL m_bUseZigCenterPos[STAGE_NUMBER_MAX];
	int m_iAfZigCenterX[STAGE_NUMBER_MAX];
	int m_iAfZigCenterY[STAGE_NUMBER_MAX];
	int m_iAFZigCenterLightPage[STAGE_NUMBER_MAX];
	double m_dAfZigRotationCenterX[STAGE_NUMBER_MAX];
	double m_dAfZigRotationCenterY[STAGE_NUMBER_MAX];

	int m_iAfLeftTopCornerPointX[STAGE_NUMBER_MAX];
	int m_iAfLeftTopCornerPointY[STAGE_NUMBER_MAX];

	int m_iAfLensStartImageNo;
	int m_iAfLensEndImageNo;
	int m_iAfModuleRotationAngleImageNo;
	int m_iAfEastSideStartImageNo;
	int m_iAfEastSideEndImageNo;
	int m_iAfWestSideStartImageNo;
	int m_iAfWestSideEndImageNo;
	int m_iAfNorthSideStartImageNo;
	int m_iAfNorthSideEndImageNo;
	int m_iAfSouthSideStartImageNo;
	int m_iAfSouthSideEndImageNo;

	// FAI 치수측정
	BOOL m_bMUseFai;
	BOOL m_bMInspectFai[MAX_FAI_ITEM];
	double m_dMNominalFai[MAX_FAI_ITEM];
	double m_dMToleranceMaxFai[MAX_FAI_ITEM];
	double m_dMToleranceMinFai[MAX_FAI_ITEM];
	double m_dMOffsetStg1Fai[MAX_FAI_ITEM];
	double m_dMTiltStg1Fai[MAX_FAI_ITEM];
	double m_dMMultipleStg1Fai[MAX_FAI_ITEM];	// 2025.04.08 - v2011 - LeeGW - CM AKC FAI
	double m_dMOffsetStg2Fai[MAX_FAI_ITEM];
	double m_dMTiltStg2Fai[MAX_FAI_ITEM];
	double m_dMMultipleStg2Fai[MAX_FAI_ITEM];	// 2025.04.08 - v2011 - LeeGW - CM AKC FAI
	int m_iTransferCodeIndexFai[MAX_FAI_ITEM];
	BOOL m_bMultiFai[MAX_FAI_ITEM];

public:
	// L10 Matching (Local Align)
	BOOL m_bUseLocalAlignMatching[MAX_IMAGE_TAB];
	HTuple m_lLAlignModelID[MAX_IMAGE_TAB];
	int m_iLocalMatchingMethod[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingRectLTX[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingRectLTY[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingRectRBX[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingRectRBY[MAX_IMAGE_TAB];

	int m_iLocalMatchingPyramidLevel[MAX_IMAGE_TAB];
	double m_iLocalMatchingAngleRange[MAX_IMAGE_TAB];
	double m_dLocalMatchingScaleMin[MAX_IMAGE_TAB];
	double m_dLocalMatchingScaleMax[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingContrast[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingContrastLow[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingContrastMinSize[MAX_IMAGE_TAB];
	double m_dLocalMatchingScore[MAX_IMAGE_TAB];
	int m_iLocalMatchingInspectionMinContrast[MAX_IMAGE_TAB];
	int m_iLocalMatchingSearchMarginX[MAX_IMAGE_TAB];
	int m_iLocalMatchingSearchMarginY[MAX_IMAGE_TAB];
	BOOL m_bUseMatchingAffineConstant[MAX_IMAGE_TAB];
	double m_dLocalTeachAlignRefX[MAX_IMAGE_TAB], m_dLocalTeachAlignRefY[MAX_IMAGE_TAB];
	BOOL m_bLocalAlignImage[MAX_IMAGE_TAB][MAX_IMAGE_TAB];
	int m_iLocalMatchingProcessChType[MAX_IMAGE_TAB];
	BOOL m_bUseMatchingFilter[MAX_IMAGE_TAB];
	int m_iMatchingFilterType[MAX_IMAGE_TAB];
	double m_dMatchingFilterTypeXSize[MAX_IMAGE_TAB];
	double m_dMatchingFilterTypeYSize[MAX_IMAGE_TAB];
	BOOL m_bUseMatchingAngleComp[MAX_IMAGE_TAB];
	int m_iMatchingAngleRefLine[MAX_IMAGE_TAB];

	BOOL m_bCheckUseMatchingXYComp[MAX_IMAGE_TAB];
	int m_iLocalIndexNumber[MAX_IMAGE_TAB];
	int m_iRadioMatchingXYRefLine[MAX_IMAGE_TAB];

public:
	void SaveModelData();
	BOOL LoadModelSWData();
	BOOL LoadModelSWBaseData();
	BOOL LoadModelHWData();
	BOOL LoadModelHWBaseData();
	void UpdateModelParamVersion();

	void UpdateTeachParamView();
	void ClearAiRetrainFlag();

	void InitLightInfo();
	int LoadLightInfo(CString sLinfoPath);
	void SaveLightInfo(CString sLinfoPath, int iCurPageIndex);
	int LoadPwmLightInfo(CString sLinfoPath);
	void SavePwmLightInfo(CString sLinfoPath);
	int LoadSequenceInfo(CString sLinfoPath);
	void SaveSequenceInfo(CString sLinfoPath);
	int LoadHWModelBase(CString sLInfoPath);
	void SaveHWModelBase(CString sLInfoPath, int iCurStage = -1);
	int LoadSpecularParam(CString sLInfoPath);
	void SaveSpecularParam(CString sLInfoPath);
	int LoadAutoFocus(CString sLInfoPath);
	void SaveAutoFocus(CString sLInfoPath, int iCurStage = -1);
	int LoadMotionMovingPosition(CString sLInfoPath);
	void SaveMotionMovingPosition(CString sLInfoPath, int iCurStage = -1);
	int LoadMotionMovingPosition_Offset(CString sLInfoPath);
	void SaveMotionMovingPosition_Offset(CString sLInfoPath, int iCurStage = -1);

	void InitLightAverageValue();
	int LoadLightAverageValue (CString sLinfoPath);
	void SaveLightAverageValue (CString sLinfoPath);

	//////////////////////////////////////////////////////////////////////////
	/// Save Param Change History
	void SaveChangeParamHistory();
	void CompareParameter(CFile* file, SYSTEMTIME time);

	void INI_CompareNRecord_BOOL(BOOL bNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile* file);
	void INI_CompareNRecord(int iNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile* file);
	void INI_CompareNRecord(double dNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile* file);
	void CompareNRecord_BOOL(BOOL bPrev, BOOL bNow, CString sParam, CString sFixVal, CFile* file);
	void CompareNRecord(int iPrev, int iNow, CString sParam, CString sFixVal, CFile* file);
	void CompareNRecord(double dPrev, double dNow, CString sParam, CString sFixVal, CFile* file);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// Save Teaching Param
	void SaveTeachingParam(int iVisionNo, CString sSaveFolderName, CString sSaveFileName);
	void WriteParameter(int iVisionNo, CFile* file);

	void WriteRecord_BOOL(BOOL bNow, CString sParam, CString sFixVal, CFile* file);
	void WriteRecord(int iNow, CString sParam, CString sFixVal, CFile* file);
	void WriteRecord(double dNow, CString sParam, CString sFixVal, CFile* file);
	//////////////////////////////////////////////////////////////////////////

	void SaveAiRetrainRoi(CString sSaveFileName);

	int GetLastPadID(int iTeachingImageIdx);
	int GetLastLocalAlignID(int iTeachingImageIdx);
	int GetLastDontCareID(int iTeachingImageIdx);
	int GetLastGenerateID(int iTeachingImageIdx);
	int GetLastMaskID(int iTeachingImageIdx);
	int GetLastColorInfoID(int iTeachingImageIdx);
	BOOL CheckInspectionROIID(int iTeachingImageIdx, int iCheckID);
	BOOL CheckLocalAlignID(int iTeachingImageIdx, int iCheckID);
	BOOL CheckDontCareID(int iTeachingImageIdx, int iCheckID);
	BOOL CheckGenerateID(int iTeachingImageIdx, int iCheckID);
	BOOL CheckMaskID(int iTeachingImageIdx, int iCheckID);
	BOOL CheckColorInfoID(int iTeachingImageIdx, int iCheckID);

	int GetLastPointID(int iInspectionType, int iPositionID);
	BOOL CheckPointID(int iInspectionType, int iPositionID, int iCheckNo);

	void UpdatePolygonROIBoundary();

	int GetWhichSpecularImage(int iImageIdx, int *piGrabCycleIndex);
	int GetWhichDiffusedImage(int iImageIdx, int *piGrabCycleIndex);

	int GetSequenceStartGrabBufferIndex(int iStageIdx, int iImageIdx);
	int GetPageIndex(int iStageIdx, int iImageIdx);
	int GetGrabingNumber(int iStageIdx, int iNoCurImageGrab);
	int GetGrabingNumber(int iStageIdx, int iNoCurImageGrab, int iNoUsedImageGrab);

	void GetLinkInfo_LocalAlign(int iSearchImageTabNo, int iSearchROINo, int *piNoLinkROI, int *piImageTabNo, int *piROICenterX, int *piROICenterY, int *piInspectTabNo);
	void GetLinkInfo_DontCare(int iSearchImageTabNo, int iSearchROINo, int *piNoLinkROI, int *piImageTabNo, int *piROICenterX, int *piROICenterY, int *piInspectTabNo);
	void GetLinkInfo_Generate(int iSearchImageTabNo, int iSearchROINo, int *piNoLinkROI, int *piImageTabNo, int *piROICenterX, int *piROICenterY, int *piInspectTabNo);
	void GetLinkInfo_Mask(int iSearchImageTabNo, int iSearchROINo, int *piNoLinkROI, int *piImageTabNo, int *piROICenterX, int *piROICenterY, int *piInspectTabNo);

	void UpdateLinkInfo_LocalAlign(int iSearchImageTabNo, int iSearchROINo, int iNewImageTabNo);
	void UpdateLinkInfo_DontCare(int iSearchImageTabNo, int iSearchROINo, int iNewImageTabNo);
	void UpdateLinkInfo_Generate(int iSearchImageTabNo, int iSearchROINo, int iNewImageTabNo);
	void UpdateLinkInfo_Mask(int iSearchImageTabNo, int iSearchROINo, int iNewImageTabNo);

	void ResetVMThresOptData();
	void UpdateVMThres();

public: 

	//////////////////////////////////////////////////////////////////////////
	///  L10 Light

	int m_iLightPageIdx[MAX_IMAGE_TAB];
	CLightControl m_PageControlData;
	//2017.04.12 hbk
	CString m_strFontName;
	Hlong m_lOCRHandle;
	//////////////////////////////////////////////////////////////////////////

	BOOL m_bUseColorSpace[MAX_IMAGE_TAB];
	void CheckColorSpace();

	int m_iLightChannelRefGV[MAX_IMAGE_TAB][LIGHT_CH_CNT];	// 조명 Calibration: 각 영상별 Page의 조명 Ch1에 대한 GV 기준값
	int m_iLightAverageValueTotal[MAX_IMAGE_TAB];

	int m_iLightValueStart;			// 하한 (조명밝기)
	int m_iLightValueEnd;			// 상한 (조명밝기)
	int m_iLightValueInterval;		// 간격 (조명밝기)
	int m_iLightValueInTol;			// 조명 밝기 허용치
	int m_iTotalImageValueTol;		// 통합 영상 밝기 허용치
	
	double m_dFocusValueStart;
	double m_dFocusValueEnd;
	double m_dFocusValueInterval;
	int m_iSelectFocusTol;
	double m_dFocusZValueTol;
	double m_dFocusEdgeValueTol;

	double	m_dTeachingFocusValue[MAX_IMAGE_TAB];

public:
	GTRegion	*m_pInspectionArea;		// 1개 비전에 티칭된 모든 ROI 포함

protected:
	CFile	m_File;
public:
	BOOL ReadOCR_SVM(void);//2017.04.11 hbk
};

