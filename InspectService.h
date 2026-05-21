#pragma once

#include "Algorithm.h"

typedef struct _RAW_IMAGE_SAVE_PARAM
{
	int iPcVisionNo;
	int iVisionCamType;
	int iMzNo;
	int iTrayNo;
	int iModuleNo;
	int iNoInspectImage;
	CString sBarcodeID;
	HObject HSaveImage[MAX_IMAGE_TAB];
} RAW_IMAGE_SAVE_PARAM;

typedef struct _RESULT_IMAGE_SAVE_PARAM
{
	CString sSavePath;
	HObject HSaveImage;
} RESULT_IMAGE_SAVE_PARAM;

typedef struct _REVIEW_IMAGE_SAVE_PARAM
{
	CString sSavePath;
	HObject HSaveImage;
} REVIEW_IMAGE_SAVE_PARAM;

typedef struct _ADJ_IMAGE_SAVE_PARAM
{
	CString sSavePath;
	HObject HSaveImage;
} ADJ_IMAGE_SAVE_PARAM;


typedef struct _FAI_IMAGE_SAVE_PARAM
{
	CString sSavePath;
	HObject HSaveImage;
} FAI_IMAGE_SAVE_PARAM;

typedef struct _RESULT_LOG_SAVE_PARAM
{
	int iSaveLogType;
	CString strPath;
	CString strType1;
	CString strType2;
	BOOL bType;
	int iPcVisionNo;
	int iMzNo;
	int iTrayNo;
	int iModuleNo;
	int iNoGrabRetry;
	int iNoFocusMoveRetry;
} RESULT_LOG_SAVE_PARAM;

class CInspectService
{
public:
	CInspectService(void);
	~CInspectService(void);

	static CInspectService *m_pInstance;
public:
	static CInspectService *GetInstance();
	void DeleteInstance();

	void InitializeLotData();

	void ReadyLot(BOOL bManual, int iMzNo, BOOL bInitThread, CString sHandlerModelName, CString sCurrentLotID, int iModelAutoLoad);
	void InspectionMove(int iVisionType, int iMzNo=TEACHING_MZ_NO);
	void SaveRawImage(int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo);
	void SaveResultImage(int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo);
	void SaveReviewImage(int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo);
	void SaveADJImage(int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo);
	void SaveFAIImage(int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo);
	void SaveResultLog(int iMzNo, CString sLotID);

	void OfflineInspection_Folder(CString sCurrentLotID);

	CString m_sLotID_H[VISION_NUMBER_MAX + 2];
	int m_iMzNo_H[VISION_NUMBER_MAX + 2];									// for Scan
	int m_iJigNo_H[VISION_NUMBER_MAX + 2];									// for Scan
	int m_iStageNo_H[VISION_NUMBER_MAX + 2];								// for Scan
	double m_dHeight_H[VISION_NUMBER_MAX + 2];								// for Scan
	CString m_sTrayID_H[VISION_NUMBER_MAX + 2];								// for Scan
	int m_iTrayNo_H[VISION_NUMBER_MAX + 2];									// for Scan
	int m_iModuleNo_H[VISION_NUMBER_MAX + 2];								// for Scan
	int m_iTrayNo2_H;														// for Scan
	int m_iModuleNo2_H;														// for Scan
	int m_iAlignVisionSeqNo_H;												// for Scan

	int m_iLotTrayAmt_H[MAX_MAGAZINE_NO];			// for Handler data, added for CMI 3000 2000
	int m_iLotModuleAmt_H[MAX_MAGAZINE_NO];			// for Handler data, added for CMI 3000 2000

	BOOL m_bSaveResultLogThreadDone[MAX_MAGAZINE_NO];			// Log Save Done Flag
	BOOL m_bContDefectLogThreadDone[MAX_MAGAZINE_NO];			// 동일 위치 불량 알람

	int	 m_iOfflineMultiModuleInspectionDoneCnt[VISION_NUMBER_MAX + 2];		// Temporary
	BOOL m_bOfflineModuleInspectDone[VISION_NUMBER_MAX + 2];		// Offline

	BOOL m_bGrabTest_ScanComplete;

	Algorithm *m_pInspectAlgorithm;		// Vision 당 1개씩
	CCriticalSection	m_csSpecularGpuProc;

	// Bottom 1 Z MotionDone Flag
	BOOL m_bZMoveDoneBottom1[10];		// Z MoveDone Flag
	BOOL m_bOnGrabBottom2;

	BOOL m_bOnGrabAngle1;

	// AF
	int m_iAutoFocusModuleCheckResult[VISION_NUMBER_MAX];
	double m_dAutoFocusModuleRotationAngle[VISION_NUMBER_MAX];
	double m_dAutoFocusDeltaX[VISION_NUMBER_MAX];
	double m_dAutoFocusDeltaY[VISION_NUMBER_MAX];

	int m_iPrevTrayNo;

	CCriticalSection	m_csRawImageSaveThreadRunning;
	BOOL m_bRawImageSaveThreadRunning[IMAGE_SAVE_THREAD_MAX_QUEUE];
	void ResetRawImageSaveThreadRunning(int nIndex);
	CCriticalSection	m_csSaveRawImage;
	BOOL IsSaveRawImageParamEmpty(int nIndex);
	void AddListSaveRawImageParam(RAW_IMAGE_SAVE_PARAM *pParam, int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo);
	void RemoveListSaveRawImageParams(int nIndex);
	RAW_IMAGE_SAVE_PARAM* GetNextSaveRawImageParam(int nIndex);

	CCriticalSection	m_csResultImageSaveThreadRunning;
	BOOL m_bResultImageSaveThreadRunning[IMAGE_SAVE_THREAD_MAX_QUEUE];
	void ResetResultImageSaveThreadRunning(int nIndex);
	CCriticalSection	m_csSaveResultImage;
	BOOL IsSaveResultImageParamEmpty(int nIndex);
	void AddListSaveResultImageParam(RESULT_IMAGE_SAVE_PARAM *pParam, int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo);
	void RemoveListSaveResultImageParams(int nIndex);
	RESULT_IMAGE_SAVE_PARAM* GetNextSaveResultImageParam(int nIndex);



	BOOL m_bReviewImageSaveThreadRunning[IMAGE_SAVE_THREAD_MAX_QUEUE];
	CCriticalSection	m_csSaveReviewImage;
	BOOL IsSaveReviewImageParamEmpty(int nIndex);
	void AddListSaveReviewImageParam(REVIEW_IMAGE_SAVE_PARAM *pParam, int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo);
	void RemoveListSaveReviewImageParams(int nIndex);
	REVIEW_IMAGE_SAVE_PARAM* GetNextSaveReviewImageParam(int nIndex);

	BOOL m_bADJImageSaveThreadRunning[IMAGE_SAVE_THREAD_MAX_QUEUE];
	CCriticalSection	m_csSaveADJImage;
	BOOL IsSaveADJImageParamEmpty(int nIndex);
	void AddListSaveADJImageParam(ADJ_IMAGE_SAVE_PARAM *pParam, int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo);
	void RemoveListSaveADJImageParams(int nIndex);
	ADJ_IMAGE_SAVE_PARAM* GetNextSaveADJImageParam(int nIndex);


	BOOL m_bFAIImageSaveThreadRunning[IMAGE_SAVE_THREAD_MAX_QUEUE];
	CCriticalSection	m_csSaveFAIImage;
	BOOL IsSaveFAIImageParamEmpty(int nIndex);
	void AddListSaveFAIImageParam(FAI_IMAGE_SAVE_PARAM *pParam, int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo);
	void RemoveListSaveFAIImageParams(int nIndex);
	FAI_IMAGE_SAVE_PARAM* GetNextSaveFAIImageParam(int nIndex);


	CCriticalSection	m_csResultLogSaveThreadRunning;
	BOOL m_bResultLogSaveThreadRunning;
	void ResetResultLogSaveThreadRunning();
	CCriticalSection	m_csSaveResultLog[MAX_MAGAZINE_NO];
	BOOL IsSaveResultLogParamEmpty(int iMzNo);
	void AddListSaveResultLogParam(RESULT_LOG_SAVE_PARAM *pParam, int iMzNo);
	RESULT_LOG_SAVE_PARAM* GetNextSaveResultLogParam(int iMzNo);

	int m_nRawImageQueueIndex;
	int m_nResultImageQueueIndex;
	int m_nReviewImageQueueIndex;
	int m_nADJImageQueueIndex;
	int m_nFAIImageQueueIndex;

	BOOL bFreezingClearCheck[4];

	BOOL GetCycleStopStatus() { return m_bCycleStopSignaled;	}
	void SetCycleStopStatus(BOOL bStatus) {	m_bCycleStopSignaled = bStatus;	}

	BOOL GetReloadStatus() { return m_bReloadSignaled; }
	void SetReloadStatus(BOOL bStatus) { m_bReloadSignaled = bStatus; }

	BOOL GetRunStartStatus() { return m_bRunStartSignaled; }
	void SetRunStartStatus(BOOL bStatus) { m_bRunStartSignaled = bStatus; }

	CEvent m_evAlignImageCopyDone;

	CString strLog;

	BOOL InspectionStart_Batch(int iVisionCamType); // 2026.01.06 - Batch 검사 - LeeGW

protected:
	void DoEvents(DWORD dwSleep);

	std::deque<RAW_IMAGE_SAVE_PARAM*> m_listRawImageSaveParam[IMAGE_SAVE_THREAD_MAX_QUEUE];
	std::deque<RESULT_IMAGE_SAVE_PARAM*> m_listResultImageSaveParam[IMAGE_SAVE_THREAD_MAX_QUEUE];
	std::deque<REVIEW_IMAGE_SAVE_PARAM*> m_listReviewImageSaveParam[IMAGE_SAVE_THREAD_MAX_QUEUE];
	std::deque<ADJ_IMAGE_SAVE_PARAM*> m_listADJImageSaveParam[IMAGE_SAVE_THREAD_MAX_QUEUE];
	std::deque<FAI_IMAGE_SAVE_PARAM*> m_listFAIImageSaveParam[IMAGE_SAVE_THREAD_MAX_QUEUE];
	std::deque<RESULT_LOG_SAVE_PARAM*> m_listResultLogSaveParam[MAX_MAGAZINE_NO];

	BOOL m_bCycleStopSignaled;
	BOOL m_bReloadSignaled;
	BOOL m_bRunStartSignaled;
};

typedef struct _INSP_THREAD_PARAM {
	_INSP_THREAD_PARAM(int iVisionCamType, CInspectService *ptr) {
		this->iVisionCamType = iVisionCamType;
		pInspectService = ptr;
	}
	CInspectService *pInspectService;
	int iVisionCamType;

} INSP_THREAD_PARAM;

typedef struct _LOT_DEFECT_ALARM_THREAD_PARAM {
	_LOT_DEFECT_ALARM_THREAD_PARAM(int iMzIdx, CString sLotID, CInspectService *ptr) {
		this->iMzIdx = iMzIdx;
		pInspectService = ptr;
		this->sLotID = sLotID;
	}
	CInspectService *pInspectService;
	int iMzIdx;
	CString sLotID;
} LOT_DEFECT_ALARM_THREAD_PARAM;

typedef struct _TRAY_ALIGN_THREAD_PARAM {
	_TRAY_ALIGN_THREAD_PARAM(HObject *pHImage, int iMzNo, int iShipTrayType, int iTrayNo, int iSeqNo, CInspectService *ptr)
	{
		pInspectService = ptr;
		this->pHImage = pHImage;
		this->iMzNo = iMzNo;
		this->iTrayNo = iTrayNo;
		this->iSeqNo = iSeqNo;
		this->iShipTrayType = iShipTrayType;
	}

	CInspectService *pInspectService;
	HObject *pHImage;
	int iMzNo;
	int iTrayNo;
	int iSeqNo;
	int iShipTrayType;
} TRAY_ALIGN_THREAD_PARAM;

typedef struct _LOT_BARCODE_UPDATE_THREAD_PARAM {
	_LOT_BARCODE_UPDATE_THREAD_PARAM(int iMzNo, CString sLotID, CInspectService *ptr) {
		this->iMzNo = iMzNo;
		pInspectService = ptr;
		this->sLotID = sLotID;
	}
	CInspectService *pInspectService;
	int iMzNo;
	CString sLotID;
} LOT_BARCODE_UPDATE_THREAD_PARAM;

typedef struct _MODULE_POSITION_THREAD_PARAM {
	_MODULE_POSITION_THREAD_PARAM(HObject *pHImage, CInspectService *ptr, int iMzNo, int iPcVision, int iStageNo, int iVisionCamType)
	{
		pInspectService = ptr;
		this->pHImage = pHImage;
		this->iMzNo = iMzNo;
		this->iPcVision = iPcVision;
		this->iStageNo = iStageNo;
		this->iVisionCamType = iVisionCamType;
	}

	CInspectService *pInspectService;
	HObject *pHImage;
	int iMzNo;
	int iPcVision;
	int iStageNo;
	int iVisionCamType;
} MODULE_POSITION_THREAD_PARAM;
