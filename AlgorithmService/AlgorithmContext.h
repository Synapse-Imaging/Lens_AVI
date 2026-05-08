#pragma once

#include <afx.h>
#include <string>
#include <vector>
#include <future>
#include <opencv2/core/mat.hpp>

#include "StdAfx.h"

#include "../BatchService/BatchDefine.h"

#ifdef LGITAI
#include "../LGIT/types.h"
#endif
#ifdef SYAI
#include "../syai_service/ServiceDirector.h"
#endif

class Algorithm;
class GTRegion;

struct AlgorithmContext {
	// 버퍼 인덱스
	int iCurInspectionBufferIdx;
	int iCurThreadIdx;
	int iCurVisionCamIdx;

	// 비전 정보
	int iPcVisionNo;
	int iVisionCamType;
	int iDualModelData;

	// 모듈 정보
	CString sLotID;
	int iStageNo;
	int iMzNo;
	int iJigNo;
	int iTrayNo;
	int iModuleNo;

	// 범위
	int iStartImageIdx;
	int iEndImageIdx;
	int iNoInspectROI;
	
	// 상태
	BOOL bGrabFail;

	// 로깅용
	CString sVisionCamType_Short;
	CString sVisionCamType_Comm;

	// 알고리즘 시간
	DWORD dwModuleTotalStartTime;

	AlgorithmContext()
		: iCurInspectionBufferIdx(-1), iCurThreadIdx(0), iCurVisionCamIdx(0)
		, iPcVisionNo(0), iVisionCamType(0), iDualModelData(0)
		, iStageNo(0), iMzNo(0), iJigNo(0), iTrayNo(0), iModuleNo(0)
		, iStartImageIdx(0), iEndImageIdx(0), iNoInspectROI(0)
		, bGrabFail(FALSE), dwModuleTotalStartTime(0)
	{}
};

// 단일 ROI 검사에 필요한 컨텍스트
struct ROIAlgorithmContext {
	// 기본 참조 
	const AlgorithmContext& algCtx;
	Algorithm* pAlgorithm;
	GTRegion* pInspectROIRgn;
	int iImageIdx;
	int iROIIndex;
	int iTabIdx;

	// 검사 중간 결과
	HObject HROIHRegion;
	HObject HMaskRgn;
	HObject HPreProcessImage;
	HObject HDefectRgn;
	HObject HResultXLD;

	// Local Align 정보
	double dLocalAlignAngle = 0.0;
	int iLocalAlignDeltaX = 0;
	int iLocalAlignDeltaY = 0;

	// ROI 크기 정보
	HTuple length1, length2;

	// AI prefix 문자열
	std::string prefixString;
	std::string localAlignAngleString;
	std::string channelType;

	// 전처리 플래그
	bool bUsePreprocess;
	bool bEnableROIProcessing;
	bool bAllROIInspection;

	// 검사 플래그
	bool bUseAIInspection;
};

struct AlgorithmCopyParam {
	
	BOOL bGrabFail = FALSE;
	CString sLotID;
	int iMzNo = 0, iStageNo = 0, iJigNo = 0, iTrayNo = 0, iModuleNo = 0;
	int iCurCircularGrabIdx = 0;
	int iNoGrabRetry = 0, iNoFocusMoveRetry = 0;

	// 이미지 복사 모드
	enum CopyMode { NORMAL, DFM, PROFILE_3D, MONO_GRAB };
	CopyMode copyMode = NORMAL;

	// 이미지 소스
	HObject(*ppGrabHImage)[3] = nullptr;
	HObject* pHImage = nullptr;
	HObject* pHIntensityImage = nullptr;
	HObject* pHProfileImage = nullptr;
	BOOL bColor = TRUE;

	// 배치
	const BATCH_INFO* pBatchInfo = nullptr;
	BOOL IsBatchMode() const { return pBatchInfo != nullptr; }
	BOOL IsFirstEntry() const { return !IsBatchMode() || pBatchInfo->iBatchIdx == 0; }
};

// AI 검사 결과 수집
struct ResultCollector {
#ifdef LGITAI
	std::vector<std::future<LGIT::InspectionResultSet>> lgaiResults;
#endif
#ifdef SYAI
	std::vector<std::future<syai_service::JobInfo>> syaiResults;
#endif
};