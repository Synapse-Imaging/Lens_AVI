#pragma once
#include "BatchDefine.h"

class CBatchManager {
private:
	static CBatchManager* m_pInstance;

	mutable CCriticalSection m_csContexts[MAX_MAGAZINE_NO + 1][MAX_TRAY_LOT + 1][MAX_MODULE_TRAY + 1][VISION_NUMBER_MAX + 1];
	std::unique_ptr<BATCH_CONTEXT> m_BatchContexts[MAX_MAGAZINE_NO + 1][MAX_TRAY_LOT + 1][MAX_MODULE_TRAY + 1][VISION_NUMBER_MAX + 1];

public:
	static CBatchManager* GetInstance();
	static void DeleteInstance();

	void SplitBatches_AxisMove(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo, int iStageNo, int iNoUsedImageGrab);

	BOOL UpdateBatchStatus(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo, int iBatchIdx, BATCH_STATUS status);

	BATCH_INFO GetBatchInfo(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo, int iBatchIdx);

	std::vector<BATCH_INFO>& GetBatchInfos(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo)
	{
		CSingleLock lock(&m_csContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo], TRUE);
		return m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->vBatches;
	}
	int GetLastBatchIndex(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo)
		const {
		return m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iLastBatchIdx;
	}
	int GetGrabDoneIndex(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo) const
	{
		CSingleLock lock(&m_csContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo], TRUE);
		return m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iGrabDoneIdx;
	}
	BOOL IsMatchingFailed(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo) const
	{
		CSingleLock lock(&m_csContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo], TRUE);
		return m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->bMatchingFailed;
	}
	BOOL IsGrabFailed(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo) const
	{
		CSingleLock lock(&m_csContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo], TRUE);
		return m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->bGrabFailed;
	}
	BOOL IsLastBatch(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo, int iBatchIdx)	const
	{
		CSingleLock lock(&m_csContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo], TRUE);
		return m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iLastBatchIdx == (iBatchIdx);
	}

	void Reset(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo);
	void ResetAll(int iMzNo);

	CCriticalSection m_csGrab[VISION_NUMBER_MAX + 1];

private:
	CBatchManager();
	~CBatchManager();

	BOOL IsAxisMoved(int iPcVisionNo, int iStageNo, int iPrevIdx, int iCurIdx);
};