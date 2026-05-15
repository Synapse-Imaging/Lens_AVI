#pragma once

#include "stdafx.h"
#include "..\uScan.h"
#include "..\InspectService.h"

#include "BatchDefine.h"
#include "BatchManager.h"
#include "BatchGrabHelper.h"

UINT BatchGrabThread(LPVOID lp)
{
	BATCH_GRAB_PARAM* pParam = (BATCH_GRAB_PARAM*)lp;
	if (!pParam) return 0;

	CInspectService* pInspectService = CInspectService::GetInstance();
	CBatchManager* pBatchManager = CBatchManager::GetInstance();
	CBatchGrabHelper GrabHelper;

	CString strLog;
	CString sLotID = pParam->sLotID;
	int iMzNo = pParam->iMzNo;
	int iStageNo = pParam->iStageNo;
	int iJigNo = pParam->iJigNo;
	int	iTrayNo = pParam->iTrayNo;
	int iModuleNo = pParam->iModuleNo;
	int iPcVisionNo = pParam->iPcVisionNo;
	int iVisionCamType = pParam->iVisionCamType;
	int iCurCircularGrabIdx = pParam->iCurCircularGrabIdx;

	int iEndGrabIdx = 0;
	int iNoGrabing = 0;
	int iSeqAddrIndex = 0;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	CString sVisionCamType_Comm;
	sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];

	thread_local vector<shared_future<void>> bayerFutures;
	thread_local vector<future<void>> copyFutures;

	try
	{
		auto& batches = pBatchManager->GetBatchInfos(iMzNo, iTrayNo, iModuleNo, iPcVisionNo);

		int iTotalBatchCount = (int)batches.size();

		strLog.Format("%s/ Batch Scan Start, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d, BatchCount: %d",
			sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iCurCircularGrabIdx, iTotalBatchCount);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		auto total_log_time_start = std::chrono::high_resolution_clock::now();
		auto total_log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

#ifdef INLINE_MODE
		GrabHelper.GrabReady(pParam, iSeqAddrIndex, iNoGrabing);
#endif
		for (auto& batch : batches)
		{
			pBatchManager->UpdateBatchStatus(iMzNo, iTrayNo, iModuleNo, iPcVisionNo, batch.iBatchIdx, BATCH_STATUS_GRAB_START);

			strLog.Format("%s/ Batch Grab Start(%d/%d), Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d",
				sVisionCamType_Short, batch.iBatchIdx + 1, iTotalBatchCount, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

			log_time_start = std::chrono::high_resolution_clock::now();

#ifdef INLINE_MODE
			BOOL bSuccess = GrabHelper.GrabStart(pParam, batch, iSeqAddrIndex, iNoGrabing, iEndGrabIdx, bayerFutures);
#else
			BOOL bSuccess = GrabHelper.GrabStart_Offline(pParam, batch, THEAPP.m_pDualCameraManager[pParam->iPcVisionNo]->m_hoCallBackImage[iCurCircularGrabIdx]);
#endif

			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("%s/ Batch Grab End(%d/%d), Time(ms): %lld, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d-%d, Grab: %d-%d",
				sVisionCamType_Short, batch.iBatchIdx + 1, iTotalBatchCount, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo,
				batch.iStartImageIdx + 1, batch.iEndImageIdx + 1, batch.iStartGrabIdx, batch.iEndGrabIdx);
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

			if (pInspectService->GetCycleStopStatus() || pInspectService->GetReloadStatus())
			{
				strLog.Format("%s/ Batch Scan Stopped(%d/%d), Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d",
					sVisionCamType_Short, batch.iBatchIdx + 1, iTotalBatchCount, sLotID, iMzNo, iTrayNo, iModuleNo);
				THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));

				pBatchManager->UpdateBatchStatus(iMzNo, iTrayNo, iModuleNo, iPcVisionNo, iTotalBatchCount - 1, BATCH_STATUS_GRAB_FAILED);

				GrabHelper.GrabEnd(pParam, iNoGrabing, iEndGrabIdx);

				for (auto& future : bayerFutures)
				{
					if (future.valid() && future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
						future.get();
				}
				bayerFutures.clear();

				for (auto& future : copyFutures) {
					if (future.valid() && future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
						future.get();
				}
				copyFutures.clear();

				delete pParam;
				return 0;
			}

			log_time_start = std::chrono::high_resolution_clock::now();

			pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

#ifdef INLINE_MODE
			shared_future<void> currentBayerFuture;
			if (!bayerFutures.empty()) {
				currentBayerFuture = bayerFutures.back();
			}

			copyFutures.push_back(std::async(launch::async, [=, &bSuccess] {
				if (currentBayerFuture.valid())
					currentBayerFuture.wait();

				AlgorithmCopyParam param;
				param.bGrabFail = !bSuccess;
				param.sLotID = sLotID;
				param.iMzNo = iMzNo;
				param.iStageNo = iStageNo;
				param.iJigNo = iJigNo;
				param.sTrayID = _T("NoUse");
				param.iTrayNo = iTrayNo;
				param.iModuleNo = iModuleNo;
				param.iCurCircularGrabIdx = iCurCircularGrabIdx;
				param.iNoGrabRetry = 0;
				param.iNoFocusMoveRetry = 0;
				param.copyMode = AlgorithmCopyParam::DFM;
				param.ppGrabHImage = THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurCircularGrabIdx];
				param.bColor = TRUE;
				param.pBatchInfo = &batch;

				BOOL bCopyRet = pInspectService->m_pInspectAlgorithm[pParam->iVisionCamType].CopyInspectInformation(param);

				if (bCopyRet == FALSE)
				{
					bSuccess = FALSE;
					param.bGrabFail = !bSuccess;
					pInspectService->m_pInspectAlgorithm[pParam->iVisionCamType].CopyInspectInformation(param);
				}

				if (!bSuccess)
				{
					if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
					{
						THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
						THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
					}

					pBatchManager->UpdateBatchStatus(iMzNo, iTrayNo, iModuleNo, iPcVisionNo, batch.iBatchIdx, BATCH_STATUS_GRAB_FAILED);
				}
				else {
					pBatchManager->UpdateBatchStatus(iMzNo, iTrayNo, iModuleNo, iPcVisionNo, batch.iBatchIdx, BATCH_STATUS_GRAB_END);
				}
			}));
#else
			copyFutures.push_back(std::async(launch::async, [=, &bSuccess] {
				AlgorithmCopyParam param;
				param.bGrabFail = !bSuccess;
				param.sLotID = sLotID;
				param.iMzNo = iMzNo;
				param.iStageNo = iStageNo;
				param.iJigNo = iJigNo;
				param.sTrayID = _T("NoUse");
				param.iTrayNo = iTrayNo;
				param.iModuleNo = iModuleNo;
				param.iCurCircularGrabIdx = iCurCircularGrabIdx;
				param.iNoGrabRetry = 0;
				param.iNoFocusMoveRetry = 0;
				param.copyMode = AlgorithmCopyParam::NORMAL;
				param.ppGrabHImage = THEAPP.m_pDualCameraManager[pParam->iPcVisionNo]->m_hoCallBackImage[iCurCircularGrabIdx];
				param.bColor = TRUE;
				param.pBatchInfo = &batch;

				BOOL bCopyRet = pInspectService->m_pInspectAlgorithm[pParam->iVisionCamType].CopyInspectInformation(param);

				if (bCopyRet == FALSE)
				{
					bSuccess = FALSE;
					param.bGrabFail = !bSuccess;
					pInspectService->m_pInspectAlgorithm[pParam->iVisionCamType].CopyInspectInformation(param);
				}

				if (!bSuccess)
				{
					if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
					{
						THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
						THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
					}

					pBatchManager->UpdateBatchStatus(iMzNo, iTrayNo, iModuleNo, iPcVisionNo, batch.iBatchIdx, BATCH_STATUS_GRAB_FAILED);
				}
				else {
					pBatchManager->UpdateBatchStatus(iMzNo, iTrayNo, iModuleNo, iPcVisionNo, batch.iBatchIdx, BATCH_STATUS_GRAB_END);
			}}));

#endif
				log_time_end = std::chrono::high_resolution_clock::now();
				log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

				strLog.Format("%s/ Batch Buffer Copy Done(%d/%d), Time(ms): %lld, LotID: %s, Port: %d, Tray: %d, Module: %d",
					sVisionCamType_Short, batch.iBatchIdx + 1, iTotalBatchCount, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo);
				THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));
		}

		if (THEAPP.Struct_PreferenceStruct.m_bUseGrabHold)
		{
			int iScanBufferIdx;
			BOOL bBufferAvailable = TRUE;

			log_time_start = std::chrono::high_resolution_clock::now();

			while (1)
			{
				bBufferAvailable = TRUE;

				iScanBufferIdx = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iScanBufferIdx;

				if (pInspectService->m_pInspectAlgorithm[iVisionCamType].IsPosProcessingDone(iScanBufferIdx) == FALSE)
				{
					bBufferAvailable = FALSE;
					break;
				}

				++iScanBufferIdx;
				if (iScanBufferIdx >= THEAPP.m_ModelDefineInfo.m_iMaxInspectionBufferCount[iVisionCamType])
					iScanBufferIdx = 0;

				if (bBufferAvailable)
					break;

				Sleep(100);

				if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
				{
					Sleep(10);

					GrabHelper.GrabEnd(pParam, iNoGrabing, iEndGrabIdx);

					for (auto& future : bayerFutures)
					{
						if (future.valid() && future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
							future.get();
					}
					bayerFutures.clear();

					for (auto& future : copyFutures) {
						if (future.valid() && future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
							future.get();
					}
					copyFutures.clear();

					delete pParam;
					return 0;
				}
			}

			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("%s/ Batch Scan Complete Holding, Time(ms): %lld, LotID: %s, Port: %d, Tray: %d, Module: %d",
				sVisionCamType_Short, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));
		}

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);

		total_log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(total_log_time_end - total_log_time_start).count();

		strLog.Format("%s/ Batch Scan Done, Time(ms): %lld, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d, BatchCount: %d",
			sVisionCamType_Short, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iCurCircularGrabIdx, iTotalBatchCount);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;
		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bGrabCircularBufferCopyDone[iCurCircularGrabIdx] = TRUE;

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

#ifdef INLINE_MODE
		GrabHelper.GrabEnd(pParam, iNoGrabing, iEndGrabIdx);
#endif

		for (auto& future : bayerFutures)
		{
			if (future.valid() && future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
				future.get();
		}
		bayerFutures.clear();

		for (auto& future : copyFutures) {
			if (future.valid() && future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
				future.get();
		}
		copyFutures.clear();

		delete pParam;
		return 0;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		strLog.Format("Halcon Exception [BatchGrabThread] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Short);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		delete pParam;
		return 0;
	}
	catch (const std::system_error& except) {
		strLog.Format("System Exception [BatchGrabThread] : %s", except.what());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Short);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		delete pParam;
		return 0;
	}
}