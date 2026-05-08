#pragma once

#include "BatchDefine.h"

class CBatchGrabHelper
{
public:
	CBatchGrabHelper();
	~CBatchGrabHelper();

	BOOL GrabReady(const BATCH_GRAB_PARAM* pParam, int &iSeqAddrIndex, int &iNoGrabing);
	BOOL GrabEnd(const BATCH_GRAB_PARAM* pParam, int iNoGrabing, int iEndGrabIndex);
	BOOL GrabStart(const BATCH_GRAB_PARAM* pParam, BATCH_INFO& batch, int &iSeqAddrIndex, int &iNoGrabing, int &iEndGrabIndex, vector<shared_future<void>> &f);
	BOOL GrabStart_Offline(const BATCH_GRAB_PARAM* pParam, BATCH_INFO& batch, HObject HInspectImage[][3]);

private:
	
	BOOL MoveMotionAndChangeSequence(
		int iPcVisionNo,
		int iVisionCamType,
		int iStageNo,
		int iJigNo,
		int iImageIndex,
		double& dPrevCamZ,
		double& dPrevLightZ,
		double& dPrevHeadX,
		double& dPrevStageY,
		double& dPrevStageT,
		double& dPrevStageR,
		int& iSeqAddrIndex
	);

	BOOL ChangeSequence(
		int iVisionCamType,
		int iPcVisionNo,
		int iStageNo,
		int iImageIndex,
		int& iSeqAddrIndex,
		BOOL bDelay
	);

	BOOL GrabImage(
		int iPcVisionNo,
		int iGrabCount,
		int iNoSeqGrabImage,
		BOOL bUseFastGrab
	);
};