#pragma once

#include <afx.h>
#include <afxmt.h>
#include <vector>
#include <atomic>

// 배치 상태
enum BATCH_STATUS {
	BATCH_STATUS_IDLE = 0,
	BATCH_STATUS_GRAB_START,
	BATCH_STATUS_GRAB_END,
	BATCH_STATUS_INSPECT_START,
	BATCH_STATUS_INSPECT_COMPLETE,
	BATCH_STATUS_GRAB_FAILED,
	BATCH_STATUS_MATCHING_FAILED,
};

// 배치 정보
typedef struct _BATCH_INFO {
	int iBatchIdx;
	int iStartImageIdx;
	int iEndImageIdx;
	int iImageCount;
	int iStartGrabIdx;
	int iEndGrabIdx;

	double dStartCamZ;
	double dStartLightZ;
	double dStartHeadX;
	double dStartStageY;
	double dStartStageT;
	double dStartStageR;

	int iStartSpecularIdx;
	int iStartDiffusedIdx;

	BATCH_STATUS status;
	DWORD dwStartTime;
	DWORD dwEndTime;

	_BATCH_INFO()
	{
		iBatchIdx = 0;
		iStartImageIdx = 0;
		iEndImageIdx = 0;
		iImageCount = 0;
		iStartGrabIdx = 0;
		iEndGrabIdx = 0;
		dStartCamZ = 0.0;
		dStartLightZ = 0.0;
		dStartHeadX = 0.0;
		dStartStageY = 0.0;
		dStartStageT = 0.0;
		dStartStageR = 0.0;
		status = BATCH_STATUS_IDLE;
		dwStartTime = 0;
		dwEndTime = 0;
		iStartSpecularIdx = 0;
		iStartDiffusedIdx = 0;
	}
} BATCH_INFO;

// 배치 컨텍스트
typedef struct _BATCH_CONTEXT {
	std::atomic<int> iLastBatchIdx;
	std::atomic<int> iGrabDoneIdx;
	std::atomic<int> iInspectDoneIdx;
	std::atomic<bool> bGrabFailed;
	std::atomic<bool> bMatchingFailed;

	std::vector<BATCH_INFO> vBatches;

	_BATCH_CONTEXT()
	{
		iLastBatchIdx = -1;
		iGrabDoneIdx = -1;
		iInspectDoneIdx = -1;
		bGrabFailed = false;
		bMatchingFailed = false;
	}
} BATCH_CONTEXT;

// 배치 촬상 파라미터
typedef struct _BATCH_GRAB_PARAM {
	CString sLotID;
	int iMzNo;
	int iJigNo;
	int iTrayNo;
	int iModuleNo;
	int iPcVisionNo;
	int iVisionCamType;
	int iStageNo;
	int iCurCircularGrabIdx;

	_BATCH_GRAB_PARAM() 
	{
		sLotID = _T("");
		iMzNo = 0;
		iJigNo = 0;
		iTrayNo = 0;
		iModuleNo = 0;
		iPcVisionNo = 0;
		iVisionCamType = 0;
		iStageNo = 0;
		iCurCircularGrabIdx = 0;
	}
} BATCH_GRAB_PARAM;

// 배치 검사 스레드 파라미터
typedef struct _BATCH_INSPECT_PARAM {
	int iThreadIdx;
	int iVisionCamIdx;
	BATCH_INFO batchInfo;

	_BATCH_INSPECT_PARAM()
	{
		iThreadIdx = 0;
		iVisionCamIdx = 0;
	}
} BATCH_INSPECT_PARAM;
