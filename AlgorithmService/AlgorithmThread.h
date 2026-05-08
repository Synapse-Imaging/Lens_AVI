#pragma once
#include "stdafx.h"
#include "AlgorithmPipeline.h"
#include "..\BatchService\BatchManager.h"
#include "..\Algorithm.h"
#include "..\GFunction.h"
#include "..\uScan.h"
#include "AlgorithmSubThread.h"

// 서브 스레드 선언(정의는 따로)
UINT SaveImageThread(LPVOID lp);
UINT SaveRawImageThread_Cam(LPVOID lp);
UINT BarcodeTransferThread(LPVOID lp);

//////////////////////////////////////////////////////////////////////////
// 일반 검사 스레드
//////////////////////////////////////////////////////////////////////////
UINT AlgorithmThread(LPVOID lp)
{
	CString strLog;

	THREAD_PARAM* pThreadParam = (THREAD_PARAM*)lp;
	int iCurThreadIdx = pThreadParam->iThreadIdx;
	int iCurVisionCamIdx = pThreadParam->iVisionCamIdx;
	SAFE_DELETE(pThreadParam);

	Algorithm* pAlgorithm = THEAPP.m_pInspectService->m_pInspectAlgorithm + iCurVisionCamIdx;
	CSingleLock sSema(pAlgorithm->m_pSemaphore);

	AlgorithmPipeline pipeline(pAlgorithm);
	AlgorithmHelper helper(pAlgorithm);
	AlgorithmContext algCtx;

	int iPcVisionNo = pAlgorithm->m_iPcVisionNo;
	int iVisionCamType = pAlgorithm->m_iVisionCamType;

	CString sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int iCurInspectionBufferIdx = -1;

	try
	{
		pAlgorithm->ChangeAlgorithmThreadRunning(iCurThreadIdx, TRUE);

		strLog.Format("%s/ Algorithm Thread-%d start", sVisionCamType_Short, iCurThreadIdx);
		THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

		while (TRUE)
		{
			sSema.Lock();

			// 큐가 비어있으면 스레드 종료
			if (pAlgorithm->IsInspectThreadParamEmpty())
			{
				sSema.Unlock();
				pAlgorithm->ChangeAlgorithmThreadRunning(iCurThreadIdx, FALSE);

				strLog.Format("%s/ Algorithm Thread-%d end", sVisionCamType_Short, iCurThreadIdx);
				THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

				return 1;
			}

			INSPECT_THREAD_PARAM* pInspectThreadParam = pAlgorithm->GetNextInspectThreadParam();
			if (pInspectThreadParam == NULL)
			{
				Sleep(5);
				sSema.Unlock();

				strLog.Format("InspectionThreadList critical error");
				THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
				continue;
			}

			iCurInspectionBufferIdx = pInspectThreadParam->iInspectionBufferIdx;
			SAFE_DELETE(pInspectThreadParam);

			algCtx = helper.BuildContext(iCurThreadIdx, iCurVisionCamIdx, iCurInspectionBufferIdx);

			strLog.Format("%s/ Algorithm start, Time(ms): -, LotID: %s, Tray: %d, Module: %d, iInspectionBufferIdx: %d, iCurThreadIdx: %d",
				algCtx.sVisionCamType_Short, algCtx.sLotID, algCtx.iTrayNo, algCtx.iModuleNo, iCurInspectionBufferIdx, iCurThreadIdx);
			THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

			pAlgorithm->m_bMatchingSuccess[iCurInspectionBufferIdx] = FALSE;
			pAlgorithm->m_bLightDisorder[iCurInspectionBufferIdx] = FALSE;

			// Tray 변경 처리
			helper.HandleTrayChange(algCtx);

			// Grab Fail 체크
			if (algCtx.bGrabFail)
			{
				CString strLog;
				strLog.Format("%s/ Grab fail, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, iInspectionBufferIdx: %d, iCurThreadIdx: %d",
					algCtx.sVisionCamType_Short, algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, algCtx.iCurInspectionBufferIdx, algCtx.iCurThreadIdx);
				THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

				pipeline.ProcessResultSave(algCtx);

				Sleep(5);
				sSema.Unlock();
				continue;
			}

			// Phase 1: 이미지 전처리
			pipeline.ProcessRawImageSave(algCtx);
			pipeline.ProcessGlobalAlign(algCtx, TRUE);
			pipeline.ProcessHsiTrans(algCtx);

			// 매칭 실패 체크
			if (pAlgorithm->m_bMatchingSuccess[iCurInspectionBufferIdx] == FALSE)
			{
				CString strLog;
				strLog.Format("%s/ Matching fail, Module: %d, iInspectionBufferIdx: %d, iCurThreadIdx: %d",
					algCtx.sVisionCamType_Short, algCtx.iModuleNo, algCtx.iCurInspectionBufferIdx, algCtx.iCurThreadIdx);
				THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));

				pipeline.ProcessResultSave(algCtx);

				Sleep(5);
				sSema.Unlock();
				continue;
			}

			// Phase 2: ROI 전처리
			pipeline.ProcessLocalAlign(algCtx);
			pipeline.ProcessFitting(algCtx);
			pipeline.ProcessFittingAdd(algCtx);
			pipeline.ProcessDontCare(algCtx);
			pipeline.ProcessGenerate(algCtx);
			pipeline.ProcessMask(algCtx);

			// Phase 3: 검사
			pipeline.ProcessFAIMeasure(algCtx, TRUE);
			pipeline.ProcessBarcodeOcr(algCtx);
			pipeline.ProcessInspection(algCtx, TRUE);
			pipeline.ProcessMonitoringLog(algCtx);

			// Phase 4: 후처리
			pipeline.PostProcess(algCtx);
			pipeline.ProcessDefectMerge(algCtx);

			// Phase 5: 결과처리
			pipeline.ProcessResultSave(algCtx);

			Sleep(5);
			sSema.Unlock();
		}

		pAlgorithm->ChangeAlgorithmThreadRunning(iCurThreadIdx, FALSE);
		return 0;
	}
	catch (HException& except)
	{
		HTuple HExp, HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [InspectThread] : <%s>%s (iCurThreadIdx: %d)", sOperatorName, sErrMsg, iCurThreadIdx);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		pAlgorithm->m_bMatchingSuccess[iCurInspectionBufferIdx] = FALSE;
		pipeline.ProcessResultSave(algCtx);

		sSema.Unlock();
		pAlgorithm->ChangeAlgorithmThreadRunning(iCurThreadIdx, FALSE);

		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// 배치 검사 스레드
//////////////////////////////////////////////////////////////////////////
UINT AlgorithmThread_Batch(LPVOID lp)
{
	CString strLog;

	BATCH_INSPECT_PARAM* pParam = (BATCH_INSPECT_PARAM*)lp;
	int iCurThreadIdx = pParam->iThreadIdx;
	int iCurVisionCamIdx = pParam->iVisionCamIdx;
	SAFE_DELETE(pParam);

	CBatchManager* batchManager = CBatchManager::GetInstance();
	Algorithm* pAlgorithm = THEAPP.m_pInspectService->m_pInspectAlgorithm + iCurVisionCamIdx;
	CSingleLock sSema(pAlgorithm->m_pSemaphore);

	AlgorithmPipeline pipeline(pAlgorithm);
	AlgorithmHelper helper(pAlgorithm);
	AlgorithmContext algCtx;

	int iPcVisionNo = pAlgorithm->m_iPcVisionNo;
	int iVisionCamType = pAlgorithm->m_iVisionCamType;

	CString sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int iCurInspectionBufferIdx = -1;

	try
	{
		pAlgorithm->ChangeAlgorithmThreadRunning(iCurThreadIdx, TRUE);

		strLog.Format("%s/ Batch Algorithm Thread-%d start", sVisionCamType_Short, iCurThreadIdx);
		THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

		while (TRUE)
		{
			sSema.Lock();

			if (pAlgorithm->IsInspectThreadParamEmpty())
			{
				sSema.Unlock();
				pAlgorithm->ChangeAlgorithmThreadRunning(iCurThreadIdx, FALSE);

				strLog.Format("%s/ Algorithm Thread-%d end", sVisionCamType_Short, iCurThreadIdx);
				THEAPP.m_log_thread->info("{}", LOG_CSTR(strLog));

				return 1;
			}

			INSPECT_THREAD_PARAM* pInspectThreadParam = pAlgorithm->GetNextInspectThreadParam();
			if (pInspectThreadParam == NULL)
			{
				Sleep(5);
				sSema.Unlock();

				strLog.Format("InspectionThreadList critical error");
				THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
				continue;
			}

			iCurInspectionBufferIdx = pInspectThreadParam->iInspectionBufferIdx;
			SAFE_DELETE(pInspectThreadParam);

			ResultCollector collector;

			algCtx = helper.BuildContext(iCurThreadIdx, iCurVisionCamIdx, iCurInspectionBufferIdx);

			strLog.Format("%s/ Algorithm start, Time(ms): -, LotID: %s, Tray: %d, Module: %d, iInspectionBufferIdx: %d, iCurThreadIdx: %d",
				algCtx.sVisionCamType_Short, algCtx.sLotID, algCtx.iTrayNo, algCtx.iModuleNo, iCurInspectionBufferIdx, iCurThreadIdx);
			THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

			pAlgorithm->m_bMatchingSuccess[iCurInspectionBufferIdx] = FALSE;
			pAlgorithm->m_bLightDisorder[iCurInspectionBufferIdx] = FALSE;

			int iLastBatchIdx = batchManager->GetLastBatchIndex(algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iPcVisionNo);

			//////////////////////////////////////////////////////////////////////////
			// 배치 루프
			//////////////////////////////////////////////////////////////////////////
			for (int iBatchIdx = 0; iBatchIdx <= iLastBatchIdx; iBatchIdx++)
			{
				// 그랩 완료 대기
				while (batchManager->GetGrabDoneIndex(algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iPcVisionNo) < iBatchIdx)
				{
					if (THEAPP.m_pInspectService->GetCycleStopStatus() || THEAPP.m_pInspectService->GetReloadStatus())
					{
						strLog.Format("%s/ Batch Inspect Stopped(%d/%d), Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d",
							sVisionCamType_Short, iBatchIdx + 1, iLastBatchIdx + 1, algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo);
						THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));

						pAlgorithm->m_bMatchingSuccess[iCurInspectionBufferIdx] = FALSE;

						pipeline.ProcessResultSave(algCtx);

						sSema.Unlock();
						pAlgorithm->ChangeAlgorithmThreadRunning(iCurThreadIdx, FALSE);

						return 0;
					}

					Sleep(10);
				}

				// 배치 정보로 이미지 범위 갱신
				BATCH_INFO batchInfo = batchManager->GetBatchInfo(algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iPcVisionNo, iBatchIdx);
				algCtx.iStartImageIdx = batchInfo.iStartImageIdx;
				algCtx.iEndImageIdx = batchInfo.iEndImageIdx + 1;
				algCtx.bGrabFail = pAlgorithm->m_bGrabFail[iCurInspectionBufferIdx];

				BOOL bFirstEntry = (iBatchIdx == 0) ? TRUE : FALSE;
				BOOL bLastEntry = batchManager->IsLastBatch(algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iPcVisionNo, iBatchIdx);
				BOOL bIsGrabFail = algCtx.bGrabFail || batchManager->IsGrabFailed(algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iPcVisionNo);

				// Tray 변경 처리 (첫 배치만)
				if (iBatchIdx == 0)
					helper.HandleTrayChange(algCtx);

				// Grab Fail 체크
				if (bIsGrabFail)
				{
					strLog.Format("%s/ Grab fail, LotID: %s, Port: %d, Tray: %d, Module: %d, Batch: %d/%d",
						algCtx.sVisionCamType_Short, algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iBatchIdx + 1, iLastBatchIdx + 1);
					THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

					if (batchManager->UpdateBatchStatus(algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iPcVisionNo, iBatchIdx, BATCH_STATUS_GRAB_FAILED))
					{
						pipeline.ProcessResultSave(algCtx);

						Sleep(5);
						sSema.Unlock();
						break;
					}
					continue;
				}

				// 원본 영상 저장
				if (!bIsGrabFail && bLastEntry)
					pipeline.ProcessRawImageSave(algCtx);

				// Phase 1: Global Align + HSI
				pipeline.ProcessGlobalAlign(algCtx, bFirstEntry);
				pipeline.ProcessHsiTrans(algCtx);

				BOOL bIsMatchingFail = pAlgorithm->m_bMatchingSuccess[iCurInspectionBufferIdx] == FALSE ||
					batchManager->IsMatchingFailed(algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iPcVisionNo);

				// 매칭 실패 체크
				if (bIsMatchingFail)
				{
					strLog.Format("%s/ Matching fail, Module: %d, Batch: %d/%d", sVisionCamType_Short, algCtx.iModuleNo, iBatchIdx + 1, iLastBatchIdx + 1);
					THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

					pAlgorithm->m_bMatchingSuccess[iCurInspectionBufferIdx] = FALSE;

					if (batchManager->UpdateBatchStatus(algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iPcVisionNo, iBatchIdx, BATCH_STATUS_MATCHING_FAILED))
					{
						pipeline.ProcessResultSave(algCtx);

						Sleep(5);
						sSema.Unlock();
						break;
					}
					continue;
				}

				// Phase 2: ROI 전처리
				pipeline.ProcessLocalAlign(algCtx);
				pipeline.ProcessFitting(algCtx);
				pipeline.ProcessFittingAdd(algCtx);
				pipeline.ProcessDontCare(algCtx);
				pipeline.ProcessGenerate(algCtx);
				pipeline.ProcessMask(algCtx);

				// Phase 3: 검사
				pipeline.ProcessFAIMeasure(algCtx, bFirstEntry);
				pipeline.ProcessBarcodeOcr(algCtx);
				pipeline.ProcessInspection(algCtx, bFirstEntry);
				pipeline.ProcessMonitoringLog(algCtx);

				// Phase 4: 후처리
				if (bLastEntry)
				{
					pipeline.PostProcess(algCtx);
					pipeline.ProcessDefectMerge(algCtx);
				}

				// Phase 5: 결과처리
				if (batchManager->UpdateBatchStatus(algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iPcVisionNo, iBatchIdx, BATCH_STATUS_INSPECT_COMPLETE))
				{
					pipeline.ProcessResultSave(algCtx);
				}

			} // for (iBatchIdx)

			Sleep(5);
			sSema.Unlock();
		}

		pAlgorithm->ChangeAlgorithmThreadRunning(iCurThreadIdx, FALSE);
		return 0;
	}
	catch (HException& except)
	{
		HTuple HExp, HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [BatchInspectThread_Pipeline] : <%s>%s (iCurThreadIdx: %d)", sOperatorName, sErrMsg, iCurThreadIdx);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		pAlgorithm->m_bMatchingSuccess[iCurInspectionBufferIdx] = FALSE;
		pipeline.ProcessResultSave(algCtx);

		sSema.Unlock();
		pAlgorithm->ChangeAlgorithmThreadRunning(iCurThreadIdx, FALSE);

		return 0;
	}
}
