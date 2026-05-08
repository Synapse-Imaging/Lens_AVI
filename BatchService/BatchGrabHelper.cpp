#include "stdafx.h"
#include "..\uScan.h"
#include "..\InspectService.h"
#include "BatchManager.h"
#include "BatchGrabHelper.h"

CBatchGrabHelper::CBatchGrabHelper() {}

CBatchGrabHelper::~CBatchGrabHelper() {}

BOOL CBatchGrabHelper::GrabReady(const BATCH_GRAB_PARAM* pParam, int &iSeqAddrIndex, int &iNoGrabing)
{
	CString sLotID = pParam->sLotID;
	int iMzNo = pParam->iMzNo;
	int iTrayNo = pParam->iTrayNo;
	int iModuleNo = pParam->iModuleNo;
	int iPcVisionNo = pParam->iPcVisionNo;
	int iVisionCamType = pParam->iVisionCamType;
	int iStageNo = pParam->iStageNo;
	int iJigNo = pParam->iJigNo;
	int iCurGrabCircularIndex = pParam->iCurCircularGrabIdx;

	BOOL bUseFastGrab = THEAPP.Struct_PreferenceStruct.m_bUseFastGrab;

	if (bUseFastGrab)
	{
		iNoGrabing = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->GetGrabingNumber(iStageNo, 0);
		THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Start(0, iNoGrabing);
	}

	iSeqAddrIndex =	THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][0];
	THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iSeqAddrIndex, FALSE);

	CString sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	CString strLog;
	strLog.Format("%s/ Grab Ready, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, iNoGrabing: %d",
		sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iNoGrabing);
	THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

	return TRUE;

}

BOOL CBatchGrabHelper::GrabEnd(const BATCH_GRAB_PARAM* pParam, int iNoGrabing, int iEndGrabIndex)
{
	CString sLotID = pParam->sLotID;
	int iMzNo = pParam->iMzNo;
	int iTrayNo = pParam->iTrayNo;
	int iModuleNo = pParam->iModuleNo;
	int iPcVisionNo = pParam->iPcVisionNo;
	int iVisionCamType = pParam->iVisionCamType;
	int iStageNo = pParam->iStageNo;
	int iJigNo = pParam->iJigNo;
	int iCurGrabCircularIndex = pParam->iCurCircularGrabIdx;

	BOOL bUseFastGrab = THEAPP.Struct_PreferenceStruct.m_bUseFastGrab;

	if (bUseFastGrab)
		THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);

	THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bGrabIndexMismatchDetected = FALSE;
	THEAPP.m_pDualCameraManager[iPcVisionNo]->m_iGrabIndexMismatchOffset = 0;

	CString sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	CString strLog;
	strLog.Format("%s/ Grab End, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, iEndGrabIndex: %d, iNoGrabing: %d",
		sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iEndGrabIndex, iNoGrabing);
	THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

	return TRUE;
}

BOOL CBatchGrabHelper::GrabStart(const BATCH_GRAB_PARAM* pParam, BATCH_INFO& batch, int &iSeqAddrIndex, int &iNoGrabing, int &iEndGrabIndex, vector<shared_future<void>> &futures)
{
	CInspectService* pInspectService = CInspectService::GetInstance();
	CBatchManager* pBatchManager = CBatchManager::GetInstance();

	CString sLotID = pParam->sLotID;
	int iMzNo = pParam->iMzNo;
	int iTrayNo = pParam->iTrayNo;
	int iModuleNo = pParam->iModuleNo;
	int iPcVisionNo = pParam->iPcVisionNo;
	int iVisionCamType = pParam->iVisionCamType;
	int iStageNo = pParam->iStageNo;
	int iJigNo = pParam->iJigNo;
	int iCurGrabCircularIndex = pParam->iCurCircularGrabIdx;

	CString strLog;
	CString sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	BOOL bUseFastGrab = THEAPP.Struct_PreferenceStruct.m_bUseFastGrab;
	BOOL bGrabSuccess = FALSE;
	int iNoCurImageGrab = batch.iStartImageIdx;
	int iNoUsedImageGrab = batch.iEndImageIdx + 1;
	int iGrabCount = batch.iStartGrabIdx;
	int iTotalGrabRetry = 0;

	double dPrevCamZ = 0;
	double dPrevLightZ = 0;
	double dPrevHeadX = 0;
	double dPrevStageY = 0;
	double dPrevStageT = 0;
	double dPrevStageR = 0;

	auto log_time_start = std::chrono::high_resolution_clock::now();
	auto log_time_end = std::chrono::high_resolution_clock::now();
	int log_time_ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	while (iNoCurImageGrab < iNoUsedImageGrab)
	{
		log_time_start = std::chrono::high_resolution_clock::now();

		BOOL bMoved = MoveMotionAndChangeSequence(iPcVisionNo, iVisionCamType, iStageNo, iJigNo, iNoCurImageGrab, dPrevCamZ, dPrevLightZ, dPrevHeadX, dPrevStageY, dPrevStageT, dPrevStageR, iSeqAddrIndex);

		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		if (bMoved)
		{
			strLog.Format("%s/ Motion Move, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, CamZ: %.0f, LightZ: %.0f, HeadX: %.0f, StageY: %.0f, StageT: %.0f, StageR: %.0f",
				sVisionCamType_Short, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1,
				dPrevCamZ, dPrevLightZ, dPrevHeadX, dPrevStageY, dPrevStageT, dPrevStageR);
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));
		}

		if (pInspectService->GetCycleStopStatus() || pInspectService->GetReloadStatus())
		{
			strLog.Format("%s/ Grab Stopped, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d",
						  sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1);
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

			return FALSE;
		}

		int iNoSeqGrabImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iAddrCount[iSeqAddrIndex];

		std::unique_ptr<CSingleLock> pBatchGrabLock = std::make_unique<CSingleLock>(&pBatchManager->m_csGrab[iPcVisionNo]);

		log_time_start = std::chrono::high_resolution_clock::now();

		for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)
		{
			if (K > 0)
				++iTotalGrabRetry;

			pBatchGrabLock->Lock();
			bGrabSuccess = GrabImage(iPcVisionNo, iGrabCount, iNoSeqGrabImage, bUseFastGrab);
			pBatchGrabLock->Unlock();

			if (bGrabSuccess)
				break;
			else
			{
				if (bUseFastGrab && K < (THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo - 1))
				{
					THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
					iNoGrabing = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->GetGrabingNumber(iStageNo, iNoCurImageGrab);
					THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Start(iGrabCount, iNoGrabing);
					iEndGrabIndex = iGrabCount;
				}
			}
		}

		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		if (bGrabSuccess)
		{
			strLog.Format("%s/ Grab, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, GrabIdx: %d, Retry: %d",
						  sVisionCamType_Short, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iGrabCount, iTotalGrabRetry);
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));
		}

		if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularImage[iNoCurImageGrab])
		{
			iNoCurImageGrab += DFM_USED_CONV_IMAGE_NUMBER;
			iGrabCount += SPV_RAW_IMAGE_NUMBER;
		}
		else if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseDiffusedImage[iNoCurImageGrab])
		{
			iNoCurImageGrab += DIFFUSED_USED_CONV_IMAGE_NUMBER;
			iGrabCount += DIFFUSED_RAW_IMAGE_NUMBER;
		}
		else
		{
			iNoCurImageGrab += iNoSeqGrabImage;
			iGrabCount += iNoSeqGrabImage;
		}

		if (!bGrabSuccess)
		{
			if (bUseFastGrab)
			{
				THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
				iNoGrabing = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->GetGrabingNumber(iStageNo, iNoCurImageGrab);
				THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Start(iGrabCount, iNoGrabing);
				iEndGrabIndex = iGrabCount;
			}

			strLog.Format("%s/ Grab Fail(No signal), Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, GrabIdx: %d, TotalRetry: %d",
				sVisionCamType_Short, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iGrabCount, iTotalGrabRetry);
			THEAPP.m_log_scan->error("{}", LOG_CSTR(strLog));
		}
	}

	log_time_start = std::chrono::high_resolution_clock::now();

	futures.push_back(std::async(launch::async, [=] {
		std::unique_ptr<CSingleLock> pBatchGrabLock = std::make_unique<CSingleLock>(&pBatchManager->m_csGrab[iPcVisionNo]);
		for (int iGrabIdx = batch.iStartGrabIdx; iGrabIdx <= batch.iEndGrabIdx; iGrabIdx++)
		{
			pBatchGrabLock->Lock();
			if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iGrabIdx],
					THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iGrabIdx],
					THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iGrabIdx],
					THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iGrabIdx],
					THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iGrabIdx],
					THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iGrabIdx],
					THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
			pBatchGrabLock->Unlock();
		}
	 }));

	log_time_end = std::chrono::high_resolution_clock::now();
	log_time_ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("%s/ MIL Buffer Bayer, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, GrabIdx: %d, Retry: %d",
		sVisionCamType_Short, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iGrabCount, iTotalGrabRetry);
	THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));


	if (iTotalGrabRetry > 0)
	{
		strLog.Format("%s/ Grab Retry Warning, LotID: %s, Port: %d, Tray: %d, Module: %d, TotalRetry: %d",
			sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iTotalGrabRetry);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));
	}

	return bGrabSuccess;
}



BOOL CBatchGrabHelper::GrabStart_Offline(const BATCH_GRAB_PARAM* pParam, BATCH_INFO& batch, HObject HInspectImage[][3])
{
	CString strLog;

	int iPcVisionNo = pParam->iPcVisionNo;
	int iVisionCamType = pParam->iVisionCamType;
	int iMzNo = pParam->iMzNo;
	int iTrayNo = pParam->iTrayNo;
	int iModuleNo = pParam->iModuleNo;

	CString sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	BOOL bGrabFail = TRUE;
	char chSep = '_';
	CString sFileBarcode = _T("NOREAD");

	CString FolderName, ImageName;

	if (THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath != "")
		FolderName.Format(THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath + "\\");
	else
	{
		AfxMessageBox("디버깅 영상 폴더가 지정되지 않았습니다.", MB_SYSTEMMODAL);
		return FALSE;
	}

	for (int iii = batch.iStartImageIdx; iii <= batch.iEndImageIdx; iii++)
	{
		for (int jjj = 0; jjj < 3; jjj++)
			GenEmptyObj(&(HInspectImage[iii][jjj]));
	}

	if (THEAPP.Struct_PreferenceStruct.m_bCombineRawImage)
	{
		try
		{
			HANDLE hFindFile;
			WIN32_FIND_DATA FindFileData;
			CString sReadFileName, strRawImageFileFullName;
			BOOL bFileFindFail = FALSE;
			HObject HColorImage;
			HObject HCombineImage;

			int iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

			int iNoImageYDir = iNoInspectImage / MAX_COMBINE_IMAGE_NUMBER;
			if (iNoInspectImage % MAX_COMBINE_IMAGE_NUMBER > 0)
				iNoImageYDir += 1;

			ImageName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, iNoInspectImage);
			sReadFileName = FolderName + ImageName;

			if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
			{
				strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);
				FindClose(hFindFile);

				ReadImage(&HCombineImage, (HTuple)strRawImageFileFullName);

				if (batch.iBatchIdx == 0)
				{
					AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);
					if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
						sFileBarcode.TrimRight(".bmp");
					else
						sFileBarcode.TrimRight(".jpg");

					THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;
				}

				if (THEAPP.m_pGFunction->ValidHImage(HCombineImage) == TRUE)
				{
					HTuple htImageWidth, htImageHeight;
					HalconCpp::GetImageSize(HCombineImage, &htImageWidth, &htImageHeight);
					int wd = (int)htImageWidth.L();
					int ht = (int)htImageHeight.L();

					int iCropImageSizeX = wd / MAX_COMBINE_IMAGE_NUMBER;
					int iCropImageSizeY = ht / iNoImageYDir;

					for (int i = batch.iStartImageIdx; i <= batch.iEndImageIdx; i++)
					{
						int iImageIndexX = i % MAX_COMBINE_IMAGE_NUMBER;
						int iImageIndexY = i / MAX_COMBINE_IMAGE_NUMBER;

						POINT CropLTPoint, CropRBPoint;
						CropLTPoint.x = iImageIndexX * iCropImageSizeX;
						CropRBPoint.x = iImageIndexX * iCropImageSizeX + iCropImageSizeX - 1;
						CropLTPoint.y = iImageIndexY * iCropImageSizeY;
						CropRBPoint.y = iImageIndexY * iCropImageSizeY + iCropImageSizeY - 1;

						CropRectangle1(HCombineImage, &HColorImage, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

						if (THEAPP.m_pGFunction->ValidHImage(HColorImage))
						{
							HalconCpp::GetImageSize(HColorImage, &htImageWidth, &htImageHeight);
							if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] ||
								htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
								ZoomImageSize(HColorImage, &HColorImage,
									THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType],
									THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");

							Decompose3(HColorImage, &(HInspectImage[i][0]), &(HInspectImage[i][1]), &(HInspectImage[i][2]));
						}
						else
						{
							bFileFindFail = TRUE;
							break;
						}
					}
				}
			}
			else
				bFileFindFail = TRUE;

			if (bFileFindFail == FALSE)
				bGrabFail = FALSE;
		}
		catch (HException &except)
		{
			HTuple HExp, HOperatorName, HErrMsg;
			CString sOperatorName, sErrMsg;
			except.ToHTuple(&HExp);
			except.GetExceptionData(HExp, "operator", &HOperatorName);
			except.GetExceptionData(HExp, "error_message", &HErrMsg);
			sOperatorName = HOperatorName.S();
			sErrMsg = HErrMsg.S();

			strLog.Format("Halcon Exception [BatchGrabHelper::ExecuteOfflineBatchGrab CombineImage] : <%s>%s", sOperatorName, sErrMsg);
			THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
		}
	}
	else
	{
		try
		{
			HANDLE hFindFile;
			WIN32_FIND_DATA FindFileData;
			CString sReadFileName, strRawImageFileFullName;
			BOOL bFileFindFail = FALSE;
			HObject HColorImage;

			for (int i = batch.iStartImageIdx; i <= batch.iEndImageIdx; i++)
			{
				if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
					ImageName.Format("Module%d_%s_Image%02d*.bmp", iModuleNo, sVisionCamType_Short, i + 1);
				else
					ImageName.Format("Module%d_%s_Image%02d*.jpg", iModuleNo, sVisionCamType_Short, i + 1);
				sReadFileName = FolderName + ImageName;

				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);
					FindClose(hFindFile);
				}
				else
				{
					bFileFindFail = TRUE;
					break;
				}

				ReadImage(&HColorImage, (HTuple)strRawImageFileFullName);

				try
				{
					HTuple htImageWidth, htImageHeight;
					HalconCpp::GetImageSize(HColorImage, &htImageWidth, &htImageHeight);

					if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] ||
						htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
						ZoomImageSize(HColorImage, &HColorImage,
							THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType],
							THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");
				}
				catch (HException &except) { ; }

				if (batch.iBatchIdx == 0 && i == (THEAPP.m_nOfflineBarcodeImageNo - 1))
				{
					AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);
					if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
						sFileBarcode.TrimRight(".bmp");
					else
						sFileBarcode.TrimRight(".jpg");

					THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;
				}

				Decompose3(HColorImage, &(HInspectImage[i][0]), &(HInspectImage[i][1]), &(HInspectImage[i][2]));
			}

			if (bFileFindFail == FALSE)
				bGrabFail = FALSE;
		}
		catch (HException &except)
		{
			HTuple HExp, HOperatorName, HErrMsg;
			CString sOperatorName, sErrMsg;
			except.ToHTuple(&HExp);
			except.GetExceptionData(HExp, "operator", &HOperatorName);
			except.GetExceptionData(HExp, "error_message", &HErrMsg);
			sOperatorName = HOperatorName.S();
			sErrMsg = HErrMsg.S();

			strLog.Format("Halcon Exception [BatchGrabHelper::ExecuteOfflineBatchGrab] : <%s>%s", sOperatorName, sErrMsg);
			THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
		}
	}

	return !bGrabFail;
}

BOOL CBatchGrabHelper::MoveMotionAndChangeSequence(
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
) {
	CInspectService* pInspectService = CInspectService::GetInstance();

	double dCamZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]
		->m_dZFocusPosition[iStageNo][iImageIndex];
	double dLightZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]
		->m_dLightZPosition[iStageNo][iImageIndex];
	double dHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]
		->m_dStageXPosition[iStageNo][iImageIndex];
	double dStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]
		->m_dStageYPosition[iStageNo][iImageIndex];
	double dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]
		->m_dTiltPosition[iStageNo][iImageIndex];
	double dStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]
		->m_dRotatePosition[iStageNo][iImageIndex];

	if (fabs(dCamZ - dPrevCamZ) > Z_MOVE_OFFSET ||
		fabs(dLightZ - dPrevLightZ) > Z_MOVE_OFFSET ||
		fabs(dHeadX - dPrevHeadX) > Z_MOVE_OFFSET ||
		fabs(dStageY - dPrevStageY) > Z_MOVE_OFFSET ||
		fabs(dStageT - dPrevStageT) > Z_MOVE_OFFSET ||
		fabs(dStageR - dPrevStageR) > Z_MOVE_OFFSET) {

		THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType] = FALSE;
		THEAPP.m_pHandlerService->Set_AMoveRequest(
			iVisionCamType, iStageNo, iJigNo, dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR);

		ChangeSequence(iVisionCamType, iPcVisionNo, iStageNo, iImageIndex, iSeqAddrIndex, FALSE);

		while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType]) {
			if (pInspectService->GetCycleStopStatus() || pInspectService->GetReloadStatus())
				return FALSE;

			Sleep(1);
		}

		dPrevCamZ = dCamZ;
		dPrevLightZ = dLightZ;
		dPrevHeadX = dHeadX;
		dPrevStageY = dStageY;
		dPrevStageT = dStageT;
		dPrevStageR = dStageR;

		return TRUE;
	}
	else
	{
		ChangeSequence(iVisionCamType, iPcVisionNo, iStageNo, iImageIndex, iSeqAddrIndex, TRUE);
	}

	return FALSE;
}

BOOL CBatchGrabHelper::ChangeSequence(
	int iVisionCamType,
	int iPcVisionNo,
	int iStageNo,
	int iImageIndex,
	int& iSeqAddrIndex,
	BOOL bDelay
) {
	int iNextSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]
		->m_iSeqAddrIndex[iStageNo][iImageIndex];

	if (iSeqAddrIndex != iNextSeqAddrIndex) {
		THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iNextSeqAddrIndex, TRUE, bDelay);
		iSeqAddrIndex = iNextSeqAddrIndex;
		return TRUE;
	}

	return FALSE;
}

BOOL CBatchGrabHelper::GrabImage(
	int iPcVisionNo,
	int iGrabCount,
	int iNoSeqGrabImage,
	BOOL bUseFastGrab
) {
	BOOL bSuccess = FALSE;

	if (bUseFastGrab) {
		bSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]
			->AutoRunCameraGrab_OneGrabFunction(iGrabCount, iNoSeqGrabImage);
	}
	else {
		bSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]
			->AutoRunCameraStartGrab(iGrabCount, iNoSeqGrabImage);
	}

	return bSuccess;
}