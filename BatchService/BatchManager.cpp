#include "stdafx.h"
#include "..\uScan.h"

#include "BatchManager.h"

CBatchManager* CBatchManager::m_pInstance = NULL;

CBatchManager* CBatchManager::GetInstance()
{
	if (!m_pInstance) {
		m_pInstance = new CBatchManager();
	}
	return m_pInstance;
}

void CBatchManager::DeleteInstance()
{
	if (m_pInstance) {
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CBatchManager::CBatchManager()
{
	for (int i = 0; i < MAX_MAGAZINE_NO + 1; i++)
		for (int ii = 0; ii < MAX_TRAY_LOT + 1; ii++)
			for (int iii = 0; iii < MAX_MODULE_TRAY + 1; iii++)
				for (int iv = 0; iv < VISION_NUMBER_MAX + 1; iv++)
				{
					CSingleLock lock(&m_csContexts[i][ii][iii][iv]);
					lock.Lock();

					m_BatchContexts[i][ii][iii][iv] = make_unique<BATCH_CONTEXT>();

					lock.Unlock();
				}
}

CBatchManager::~CBatchManager()
{

}
void CBatchManager::SplitBatches_AxisMove(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo, int iStageNo, int iNoUsedImageGrab)
{
	CSingleLock lock(&m_csContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]);
	lock.Lock();

	CString strLog;
	CString sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int iBatchIdx = 0;
	int iCurGrabIdx = 0;
	int iCurSpecularCount = 0;
	int iCurDiffusedCount = 0;
	int iPrevImageIdx = 0;
	int iBatchStartGrabIdx = 0;
	int iNoCurImageGrab = 0;

	BATCH_INFO currentBatch;
	currentBatch.iBatchIdx = iBatchIdx;
	currentBatch.iStartImageIdx = 0;
	currentBatch.iStartGrabIdx = 0;
	currentBatch.status = BATCH_STATUS_IDLE;
	currentBatch.dStartCamZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dZFocusPosition[iStageNo][0];
	currentBatch.dStartLightZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dLightZPosition[iStageNo][0];
	currentBatch.dStartHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][0];
	currentBatch.dStartStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][0];
	currentBatch.dStartStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][0];
	currentBatch.dStartStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][0];
	currentBatch.iStartSpecularIdx = 0;
	currentBatch.iStartDiffusedIdx = 0;

	strLog.Format("%s/ Batch Split Start, Time(ms): -, Port: %d, Tray: %d, Module: %d, TotalImage: %d",
		sVisionCamType_Short, iMzNo, iTrayNo, iModuleNo, iNoUsedImageGrab);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

	while (TRUE)
	{
		if (iNoCurImageGrab >= iNoUsedImageGrab)
			break;

		BOOL bIsIndependent = IsAxisMoved(iPcVisionNo, iStageNo, iPrevImageIdx, iNoCurImageGrab);
		if (bIsIndependent && iNoCurImageGrab > 0)
		{
			currentBatch.iEndImageIdx = iNoCurImageGrab - 1;
			currentBatch.iImageCount = currentBatch.iEndImageIdx - currentBatch.iStartImageIdx + 1;
			currentBatch.iEndGrabIdx = iCurGrabIdx - 1;
			m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->vBatches.push_back(currentBatch);

			strLog.Format("%s/ Batch Split, Port: %d, Tray: %d, Module: %d, Batch: %d, Image: %d-%d, Grab: %d-%d",
				sVisionCamType_Short, iMzNo, iTrayNo, iModuleNo,
				currentBatch.iBatchIdx, currentBatch.iStartImageIdx + 1, currentBatch.iEndImageIdx + 1,
				currentBatch.iStartGrabIdx, currentBatch.iEndGrabIdx);
			THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

			currentBatch.iBatchIdx = ++iBatchIdx;
			currentBatch.iStartImageIdx = iNoCurImageGrab;
			currentBatch.iStartGrabIdx = iCurGrabIdx;

			currentBatch.status = BATCH_STATUS_IDLE;
			currentBatch.dStartCamZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dZFocusPosition[iStageNo][iNoCurImageGrab];
			currentBatch.dStartLightZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dLightZPosition[iStageNo][iNoCurImageGrab];
			currentBatch.dStartHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][iNoCurImageGrab];
			currentBatch.dStartStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][iNoCurImageGrab];
			currentBatch.dStartStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
			currentBatch.dStartStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][iNoCurImageGrab];
			currentBatch.iStartSpecularIdx = iCurSpecularCount;
			currentBatch.iStartDiffusedIdx = iCurDiffusedCount;
		}

		iPrevImageIdx = iNoCurImageGrab;
		if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularImage[iNoCurImageGrab])
		{
			iCurSpecularCount++;
			iNoCurImageGrab += DFM_USED_CONV_IMAGE_NUMBER;
			iCurGrabIdx += SPV_RAW_IMAGE_NUMBER;
		}
		else if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseDiffusedImage[iNoCurImageGrab])
		{
			iCurDiffusedCount++;
			iNoCurImageGrab += DIFFUSED_USED_CONV_IMAGE_NUMBER;
			iCurGrabIdx += DIFFUSED_RAW_IMAGE_NUMBER;
		}
		else
		{
			int iSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];
			int iNoSeqGrabImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iAddrCount[iSeqAddrIndex];

			iNoCurImageGrab += iNoSeqGrabImage;
			iCurGrabIdx += iNoSeqGrabImage;
		}
	}

	currentBatch.iEndImageIdx = iNoUsedImageGrab - 1;
	currentBatch.iImageCount = currentBatch.iEndImageIdx - currentBatch.iStartImageIdx + 1;
	currentBatch.iEndGrabIdx = iCurGrabIdx - 1;

	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->vBatches.push_back(currentBatch);
	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iLastBatchIdx = m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->vBatches.size() - 1;
	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iInspectDoneIdx = -1;
	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iGrabDoneIdx = -1;

	strLog.Format("%s/ Batch Split Done, Port: %d, Tray: %d, Module: %d, BatchCount: %d, ImageCount: %d, GrabCount: %d",
		sVisionCamType_Short, iMzNo, iTrayNo, iModuleNo,
		(int)m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->vBatches.size(), iNoUsedImageGrab, iCurGrabIdx);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

	lock.Unlock();

	return;
}

BOOL CBatchManager::IsAxisMoved(int iPcVisionNo, int iStageNo, int iPrevIdx, int iCurIdx)
{
	if (iPcVisionNo == VISION_NUMBER_2 || iPcVisionNo == VISION_NUMBER_4)
	{
		double dPrevHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][iPrevIdx];
		double dPrevStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][iPrevIdx];
		double dPrevStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iPrevIdx];
		double dPrevStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][iPrevIdx];

		double dCurHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][iCurIdx];
		double dCurStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][iCurIdx];
		double dCurStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iCurIdx];
		double dCurStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][iCurIdx];


		if (fabs(dCurHeadX - dPrevHeadX) > Z_MOVE_OFFSET ||
			fabs(dCurStageY - dPrevStageY) > Z_MOVE_OFFSET ||
			fabs(dCurStageT - dPrevStageT) > Z_MOVE_OFFSET)
			return TRUE;
	}

	return FALSE;
}

BOOL CBatchManager::UpdateBatchStatus(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo, int iBatchIdx, BATCH_STATUS status)
{
	CSingleLock lock(&m_csContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]);
	lock.Lock();

	BOOL bIsComplete = FALSE;

	int iBatchSize = (int)m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->vBatches.size();
	if (iBatchIdx < 0 || iBatchIdx >= iBatchSize)
	{
		lock.Unlock();
		return bIsComplete;
	}

	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->vBatches[iBatchIdx].status = status;

	if (status == BATCH_STATUS_GRAB_FAILED)
		m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->bGrabFailed = true;

	if (status == BATCH_STATUS_MATCHING_FAILED)
		m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->bMatchingFailed = true;

	if (status == BATCH_STATUS_GRAB_END || status == BATCH_STATUS_GRAB_FAILED)
	{
		if (m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iGrabDoneIdx < iBatchIdx)
			m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iGrabDoneIdx = iBatchIdx;
	}

	if (status == BATCH_STATUS_INSPECT_COMPLETE || status == BATCH_STATUS_GRAB_FAILED || status == BATCH_STATUS_MATCHING_FAILED)
	{
		if (m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iInspectDoneIdx < iBatchIdx)
			m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iInspectDoneIdx = iBatchIdx;

		bIsComplete = iBatchIdx >= m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iLastBatchIdx;
	}

	lock.Unlock();
	return bIsComplete;
}

BATCH_INFO CBatchManager::GetBatchInfo(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo, int iBatchIdx)
{
	CSingleLock lock(&m_csContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]);
	lock.Lock();

	BATCH_INFO batch;

	int iBatchSize = (int)m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->vBatches.size();
	if (iBatchIdx >= 0 && iBatchIdx < iBatchSize)
		batch = m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->vBatches[iBatchIdx];

	lock.Unlock();
	return batch;
}

void CBatchManager::Reset(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo)
{
	CSingleLock lock(&m_csContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]);
	lock.Lock();

	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->vBatches.clear();
	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iLastBatchIdx = -1;
	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iInspectDoneIdx = -1;
	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->iGrabDoneIdx = -1;
	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->bGrabFailed = false;
	m_BatchContexts[iMzNo][iTrayNo][iModuleNo][iPcVisionNo]->bMatchingFailed = false;

	lock.Unlock();
}

void CBatchManager::ResetAll(int iMzNo)
{
	for (int ii = 0; ii < MAX_TRAY_LOT + 1; ii++)
		for (int iii = 0; iii < MAX_MODULE_TRAY + 1; iii++)
			for (int iv = 0; iv < VISION_NUMBER_MAX + 1; iv++)
				Reset(iMzNo, ii, iii, iv);
}