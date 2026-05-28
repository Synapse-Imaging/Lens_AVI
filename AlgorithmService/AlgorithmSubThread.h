#pragma once
#include "stdafx.h"
#include "..\InspectService.h"
#include "..\Algorithm.h"
#include "..\uScan.h"

inline UINT SaveImageThread(LPVOID lp)
{
	CString strLog;
	SAVE_THREAD_PARAM* pSaveThreadParam = (SAVE_THREAD_PARAM*)lp;

	int iCurVisionCamIdx, iCurInspectionBufferIdx, iCurThreadIdx;
	iCurVisionCamIdx = pSaveThreadParam->iVisionCamIdx;
	iCurInspectionBufferIdx = pSaveThreadParam->iInspectionBufferIdx;
	iCurThreadIdx = pSaveThreadParam->iThreadIdx;

	SAFE_DELETE(pSaveThreadParam);

	Algorithm* pAlgorithm = THEAPP.m_pInspectService->m_pInspectAlgorithm + iCurVisionCamIdx;

	int iInspectionBufferIdx, iPcVisionNo, iVisionCamType, iMzNo, iTrayNo, iModuleNo, iNoInspectImage;
	int i;

	iInspectionBufferIdx = iCurInspectionBufferIdx;

	CString sBarcodeID;
	iPcVisionNo = pAlgorithm->m_iPcVisionNo;
	iVisionCamType = pAlgorithm->m_iVisionCamType;
	iNoInspectImage = pAlgorithm->m_iNoInspectImage;
	iMzNo = pAlgorithm->m_nMzNo[iInspectionBufferIdx];
	iTrayNo = pAlgorithm->m_nTrayNo[iInspectionBufferIdx];
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	iModuleNo = pAlgorithm->m_nModuleNo[iInspectionBufferIdx];

#ifndef INLINE_MODE
	EnterCriticalSection(&THEAPP.m_csBarcodeRead);
#endif
	sBarcodeID = THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1];
#ifndef INLINE_MODE
	LeaveCriticalSection(&THEAPP.m_csBarcodeRead);
#endif

	CString strFileFullName, ImageFileName, sInspectTime;

	HObject HSaveImage[MAX_IMAGE_TAB];

	try
	{
		//Local image copy...

#if defined(SINGLE_LENS) || defined(ASSY_LENS)
		int iImageChType = CHANNEL_TYPE_I;
#else
		int iImageChType = CHANNEL_TYPE_COLOR;
#endif

		for (i = 0; i < iNoInspectImage; i++)
		{
			GenEmptyObj(&(HSaveImage[i]));
			if (THEAPP.m_pGFunction->ValidHImage(pAlgorithm->m_HInspectCImage[iInspectionBufferIdx][iImageChType][i]))
			{
				if (THEAPP.Struct_PreferenceStruct.m_bReduceRawImage)
				{
					if (THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio < 1)
						ZoomImageFactor(pAlgorithm->m_HInspectCImage[iInspectionBufferIdx][iImageChType][i], HSaveImage + i, THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio, THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio, "none");
					else
						CopyImage(pAlgorithm->m_HInspectCImage[iInspectionBufferIdx][iImageChType][i], HSaveImage + i);
				}
				else
					CopyImage(pAlgorithm->m_HInspectCImage[iInspectionBufferIdx][iImageChType][i], HSaveImage + i);
			}
		}

		pAlgorithm->SetCopyThreadRun(iCurThreadIdx, FALSE);
		pAlgorithm->m_evCopyForSavingDone[iCurThreadIdx].SetEvent(); //Signal !!! See Also) #401 AfxBeginThread(SaveImageThread, ...)

	}
	catch (HException& except)
	{
		pAlgorithm->SetCopyThreadRun(iCurThreadIdx, FALSE);
		pAlgorithm->m_evCopyForSavingDone[iCurThreadIdx].SetEvent();

		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AlgorithmThreadDefine::SaveImageThread 메모리 복사 오류] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

	try
	{
		if (THEAPP.Struct_PreferenceStruct.m_bCombineRawImage)
		{
			HObject HConcatImage, HCombineImage;

			GenEmptyObj(&HConcatImage);

			for (i = 0; i < iNoInspectImage; i++)
			{
				if (THEAPP.m_pGFunction->ValidHImage(HSaveImage[i]))
				{
					ConcatObj(HConcatImage, HSaveImage[i], &HConcatImage);
				}
			}

			if (THEAPP.m_pGFunction->ValidHImage(HConcatImage))
				TileImages(HConcatImage, &HCombineImage, MAX_COMBINE_IMAGE_NUMBER, "horizontal");

			auto log_time_start = std::chrono::high_resolution_clock::now();

			if (THEAPP.m_pGFunction->ValidHImage(HCombineImage))
			{
				if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
				{
					THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1]);
					ImageFileName.Format("Module%d_%s_Image%02d", iModuleNo, sVisionCamType_Short, iNoInspectImage);
					strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ImageFileName + "_" + sBarcodeID;

					if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
						WriteImage(HCombineImage, "bmp", 0, HTuple(strFileFullName));
					else
#ifdef INLINE_MODE
						WriteImage(HCombineImage, "jpg", 0, HTuple(strFileFullName));
#else
						WriteImage(HCombineImage, "jpeg 100", 0, HTuple(strFileFullName));
#endif
				}

				if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS)
				{
					THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1]);
					ImageFileName.Format("Module%d_%s_Image%02d", iModuleNo, sVisionCamType_Short, iNoInspectImage);
					strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ImageFileName + "_" + sBarcodeID;

					if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
						WriteImage(HCombineImage, "bmp", 0, HTuple(strFileFullName));
					else
#ifdef INLINE_MODE
						WriteImage(HCombineImage, "jpg", 0, HTuple(strFileFullName));
#else
						WriteImage(HCombineImage, "jpeg 100", 0, HTuple(strFileFullName));
#endif
				}
			}

			ImageFileName.Format("Module%d_%s_Image(ALL)", iModuleNo, sVisionCamType_Short);

			if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS)
				strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ImageFileName + "_" + sBarcodeID;
			else
				strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ImageFileName + "_" + sBarcodeID;

			auto log_time_end = std::chrono::high_resolution_clock::now();
			auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("Save raw image(Thread), Time(ms): %d, File name: %s", log_time_ms, strFileFullName);
			THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
		}
		else
		{
			auto log_time_start = std::chrono::high_resolution_clock::now();
			auto log_time_end = std::chrono::high_resolution_clock::now();
			auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();
			for (i = 0; i < iNoInspectImage; i++)
			{
				if (THEAPP.m_pGFunction->ValidHImage(HSaveImage[i]))
				{
					if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
					{
						THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1]);
						ImageFileName.Format("Module%d_%s_Image%02d", iModuleNo, sVisionCamType_Short, i + 1);
						strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ImageFileName + "_" + sBarcodeID;

						if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
							log_time_start = std::chrono::high_resolution_clock::now();

						if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
							WriteImage(HSaveImage[i], "bmp", 0, HTuple(strFileFullName));
						else
#ifdef INLINE_MODE
							WriteImage(HSaveImage[i], "jpg", 0, HTuple(strFileFullName));
#else
							WriteImage(HSaveImage[i], "jpeg 100", 0, HTuple(strFileFullName));
#endif

						if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
						{
							log_time_end = std::chrono::high_resolution_clock::now();
							log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

							strLog.Format("Save raw image(Thread), Time(ms): %d, File name: %s", log_time_ms, strFileFullName);
							THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
						}
						Sleep(10);
					}

					if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS)
					{
						THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1]);
						ImageFileName.Format("Module%d_%s_Image%02d", iModuleNo, sVisionCamType_Short, i + 1);
						strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ImageFileName + "_" + sBarcodeID;

						if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
							log_time_start = std::chrono::high_resolution_clock::now();

						if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
							WriteImage(HSaveImage[i], "bmp", 0, HTuple(strFileFullName));
						else
#ifdef INLINE_MODE
							WriteImage(HSaveImage[i], "jpg", 0, HTuple(strFileFullName));
#else
							WriteImage(HSaveImage[i], "jpeg 100", 0, HTuple(strFileFullName));
#endif

						if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
						{
							log_time_end = std::chrono::high_resolution_clock::now();
							log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

							strLog.Format("Save raw image(Thread), Time(ms): %d, File name: %s", log_time_ms, strFileFullName);
							THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
						}
						Sleep(10);
					}
				}
			} // end of for

			if (!THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
			{
				ImageFileName.Format("Module%d_%s_Image(ALL)", iModuleNo, sVisionCamType_Short);

				if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS)
					strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ImageFileName + "_" + sBarcodeID;
				else
					strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ImageFileName + "_" + sBarcodeID;

				log_time_end = std::chrono::high_resolution_clock::now();
				log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

				strLog.Format("Save raw image(Thread), Time(ms): %d, File name: %s", log_time_ms, strFileFullName);
				THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
			}
		}

		Sleep(0); //210826 jwj add
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AlgorithmThreadDefine::SaveImageThread 파일 저장 오류] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

	return 1;
}

inline UINT SaveRawImageThread_Cam(LPVOID lp)
{
	CString strLog;

	Algorithm* pAlgorithm = (Algorithm*)lp;

	int nCurQueueIndex = pAlgorithm->m_nRawImageQueueIndex;

	int iNoThreadActiveCheck = 0;
	for (int iCheck = 0; iCheck < 10; iCheck++)
	{
		if (pAlgorithm->m_bRawImageSaveThreadRunning_Cam[nCurQueueIndex])
			++iNoThreadActiveCheck;
		Sleep(1);
	}

	if (iNoThreadActiveCheck > 0)
		return 1;

	pAlgorithm->m_bRawImageSaveThreadRunning_Cam[nCurQueueIndex] = TRUE;

	try
	{
		CString strFileFullName, ImageFileName;
		int i;
		int iPcVisionNo = pAlgorithm->m_iPcVisionNo;
		int iVisionCamType = pAlgorithm->m_iVisionCamType;
		CString sVisionCamType_Short;

		while (TRUE)
		{
			if (pAlgorithm->IsSaveRawImageParamEmpty_Cam(nCurQueueIndex))
			{
				strLog.Format("SaveRawImageThread_Cam end");
				THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

				pAlgorithm->m_bRawImageSaveThreadRunning_Cam[nCurQueueIndex] = FALSE;

				return 1;
			}

			RAW_IMAGE_SAVE_PARAM_CAM* pSaveThreadParam = pAlgorithm->GetNextSaveRawImageParam_Cam(nCurQueueIndex);
			if (pSaveThreadParam == NULL)
				continue;

			sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

			if (THEAPP.Struct_PreferenceStruct.m_bCombineRawImage)
			{
				HObject HConcatImage, HCombineImage;

				GenEmptyObj(&HConcatImage);

				for (i = 0; i < pSaveThreadParam->iNoInspectImage; i++)
				{
					if (THEAPP.m_pGFunction->ValidHImage(pSaveThreadParam->HSaveImage[i]))
					{
						ConcatObj(HConcatImage, pSaveThreadParam->HSaveImage[i], &HConcatImage);
					}
				}

				if (THEAPP.m_pGFunction->ValidHImage(HConcatImage))
					TileImages(HConcatImage, &HCombineImage, MAX_COMBINE_IMAGE_NUMBER, "horizontal");

				auto log_time_start = std::chrono::high_resolution_clock::now();

				if (THEAPP.m_pGFunction->ValidHImage(HCombineImage))
				{
					if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
					{
						if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
						{
							THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);

							ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, pSaveThreadParam->iNoInspectImage);

							strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;

							if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
								WriteImage(HCombineImage, "bmp", 0, HTuple(strFileFullName));
							else
#ifdef INLINE_MODE
								WriteImage(HCombineImage, "jpg", 0, HTuple(strFileFullName));
#else
								WriteImage(HCombineImage, "jpeg 100", 0, HTuple(strFileFullName));
#endif
						}
						else
						{
							if (nCurQueueIndex == 1)
							{
								THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Temp1[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
								ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, pSaveThreadParam->iNoInspectImage);
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp1[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
							}
							else if (nCurQueueIndex == 2)
							{
								THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Temp2[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
								ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, pSaveThreadParam->iNoInspectImage);
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp2[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
							}
							else if (nCurQueueIndex == 3)
							{
								THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Temp3[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
								ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, pSaveThreadParam->iNoInspectImage);
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp3[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
							}
							else
							{
								THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
								ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, pSaveThreadParam->iNoInspectImage);
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
							}

							if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
								WriteImage(HCombineImage, "bmp", 0, HTuple(strFileFullName));
							else
#ifdef INLINE_MODE
								WriteImage(HCombineImage, "jpg", 0, HTuple(strFileFullName));
#else
								WriteImage(HCombineImage, "jpeg 100", 0, HTuple(strFileFullName));
#endif
						}
					}
					else
					{
						if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
						{
							THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
							ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, pSaveThreadParam->iNoInspectImage);
							strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;

							if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
								WriteImage(HCombineImage, "bmp", 0, HTuple(strFileFullName));
							else
#ifdef INLINE_MODE
								WriteImage(HCombineImage, "jpg", 0, HTuple(strFileFullName));
#else
								WriteImage(HCombineImage, "jpeg 100", 0, HTuple(strFileFullName));
#endif
						}
						else
						{
							if (nCurQueueIndex == 1)
							{
								THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
								ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, pSaveThreadParam->iNoInspectImage);
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
							}
							else
							{
								THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
								ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, pSaveThreadParam->iNoInspectImage);
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
							}

							if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
								WriteImage(HCombineImage, "bmp", 0, HTuple(strFileFullName));
							else
#ifdef INLINE_MODE
								WriteImage(HCombineImage, "jpg", 0, HTuple(strFileFullName));
#else
								WriteImage(HCombineImage, "jpeg 100", 0, HTuple(strFileFullName));
#endif
						}
					}
				} // end of for

				ImageFileName.Format("Module%d_%s_Image(ALL)", pSaveThreadParam->iModuleNo, sVisionCamType_Short);

				if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
				{
					if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
						strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
					else
					{
						if (nCurQueueIndex == 1)
							strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp1[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
						else if (nCurQueueIndex == 2)
							strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp2[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
						else if (nCurQueueIndex == 3)
							strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp3[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
						else
							strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
					}
				}
				else
				{
					if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
						strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
					else
					{
						if (nCurQueueIndex == 1)
							strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
						else
							strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
					}
				}

				auto log_time_end = std::chrono::high_resolution_clock::now();
				auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

				strLog.Format("Save raw image(Thread), Time(ms): %d, File name: %s", log_time_ms, strFileFullName);
				THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));

			}
			else
			{
				auto log_time_start = std::chrono::high_resolution_clock::now();
				auto log_time_end = std::chrono::high_resolution_clock::now();
				auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

				for (i = 0; i < pSaveThreadParam->iNoInspectImage; i++)
				{
					if (THEAPP.m_pGFunction->ValidHImage(pSaveThreadParam->HSaveImage[i]))
					{
						if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
						{
							if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
							{
								THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);

								ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, i + 1);

								if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
									log_time_start = std::chrono::high_resolution_clock::now();

								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;

								if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
									WriteImage(pSaveThreadParam->HSaveImage[i], "bmp", 0, HTuple(strFileFullName));
								else
#ifdef INLINE_MODE
									WriteImage(pSaveThreadParam->HSaveImage[i], "jpg", 0, HTuple(strFileFullName));
#else
									WriteImage(pSaveThreadParam->HSaveImage[i], "jpeg 100", 0, HTuple(strFileFullName));
#endif

								if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
								{
									log_time_end = std::chrono::high_resolution_clock::now();
									log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

									strLog.Format("Save raw image(Thread), Time(ms): %d, File name: %s", log_time_ms, strFileFullName);
									THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
								}

								Sleep(10);
							}
							else
							{
								if (nCurQueueIndex == 1)
								{
									THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Temp1[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
									ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, i + 1);
									strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp1[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
								}
								else if (nCurQueueIndex == 2)
								{
									THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Temp2[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
									ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, i + 1);
									strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp2[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
								}
								else if (nCurQueueIndex == 3)
								{
									THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Temp3[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
									ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, i + 1);
									strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp3[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
								}
								else
								{
									THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
									ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, i + 1);
									strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
								}

								if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
									log_time_start = std::chrono::high_resolution_clock::now();

								if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
									WriteImage(pSaveThreadParam->HSaveImage[i], "bmp", 0, HTuple(strFileFullName));
								else
#ifdef INLINE_MODE
									WriteImage(pSaveThreadParam->HSaveImage[i], "jpg", 0, HTuple(strFileFullName));
#else
									WriteImage(pSaveThreadParam->HSaveImage[i], "jpeg 100", 0, HTuple(strFileFullName));
#endif

								if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
								{
									log_time_end = std::chrono::high_resolution_clock::now();
									log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

									strLog.Format("Save raw image(Thread), Time(ms): %d, File name: %s", log_time_ms, strFileFullName);
									THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
								}
								Sleep(10);
							}
						}
						else
						{
							if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
							{
								THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
								ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, i + 1);
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;

								if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
									log_time_start = std::chrono::high_resolution_clock::now();

								if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
									WriteImage(pSaveThreadParam->HSaveImage[i], "bmp", 0, HTuple(strFileFullName));
								else
#ifdef INLINE_MODE
									WriteImage(pSaveThreadParam->HSaveImage[i], "jpg", 0, HTuple(strFileFullName));
#else
									WriteImage(pSaveThreadParam->HSaveImage[i], "jpeg 100", 0, HTuple(strFileFullName));
#endif

								if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
								{
									log_time_end = std::chrono::high_resolution_clock::now();
									log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

									strLog.Format("Save raw image(Thread), Time(ms): %d, File name: %s", log_time_ms, strFileFullName);
									THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
								}

								Sleep(10);
							}
							else
							{
								if (nCurQueueIndex == 1)
								{
									THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
									ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, i + 1);
									strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
								}
								else
								{
									THEAPP.m_FileBase.CreatePath(THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1]);
									ImageFileName.Format("Module%d_%s_Image%02d", pSaveThreadParam->iModuleNo, sVisionCamType_Short, i + 1);
									strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
								}

								if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
									log_time_start = std::chrono::high_resolution_clock::now();

								if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
									WriteImage(pSaveThreadParam->HSaveImage[i], "bmp", 0, HTuple(strFileFullName));
								else
#ifdef INLINE_MODE
									WriteImage(pSaveThreadParam->HSaveImage[i], "jpg", 0, HTuple(strFileFullName));
#else
									WriteImage(pSaveThreadParam->HSaveImage[i], "jpeg 100", 0, HTuple(strFileFullName));
#endif

								if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
								{
									log_time_end = std::chrono::high_resolution_clock::now();
									log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

									strLog.Format("Save raw image(Thread), Time(ms): %d, File name: %s", log_time_ms, strFileFullName);
									THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
								}
								Sleep(10);
							}
						}
					}
				} // end of for

				if (!THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLogType)
				{
					ImageFileName.Format("Module%d_%s_Image(ALL)", pSaveThreadParam->iModuleNo, sVisionCamType_Short);

					if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
					{
						if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
							strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
						else
						{
							if (nCurQueueIndex == 1)
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp1[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
							else if (nCurQueueIndex == 2)
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp2[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
							else if (nCurQueueIndex == 3)
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Temp3[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
							else
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
						}
					}
					else
					{
						if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
							strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
						else
						{
							if (nCurQueueIndex == 1)
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
							else
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_LAS[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;
						}
					}

					log_time_end = std::chrono::high_resolution_clock::now();
					log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

					strLog.Format("Save raw image(Thread), Time(ms): %d, File name: %s", log_time_ms, strFileFullName);
					THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
				}
			}

			for (i = 0; i < pSaveThreadParam->iNoInspectImage; i++)
				GenEmptyObj(&(pSaveThreadParam->HSaveImage[i]));

			SAFE_DELETE(pSaveThreadParam);

			Sleep(0); //210826 jwj add
		}

		return 1;

	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AlgorithmThreadDefine::SaveRawImageThread_Cam 파일 저장 오류] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		pAlgorithm->m_bRawImageSaveThreadRunning_Cam[nCurQueueIndex] = FALSE;

		return 0;
	}
}

inline UINT BarcodeTransferThread(LPVOID lp)
{
	CString strLog;
	BARCODE_TRANSFER_THREAD_PARAM* pBarcodeThreadParam = (BARCODE_TRANSFER_THREAD_PARAM*)lp;

	int iVisionCamType, iScanBufferIdx, iCurGrabCirularBufferIdx;
	iVisionCamType = pBarcodeThreadParam->iVisionCamIdx;
	iScanBufferIdx = pBarcodeThreadParam->iScanBufferIdx;
	iCurGrabCirularBufferIdx = pBarcodeThreadParam->iGrabCircularBufferIdx;

	SAFE_DELETE(pBarcodeThreadParam);

	Algorithm* pAlgorithm = THEAPP.m_pInspectService->m_pInspectAlgorithm + iVisionCamType;

	CString sLotID;
	int iPcVisionNo, iNoInspectImage;
	int iMzNo, iTrayNo, iModuleNo;

	iPcVisionNo = pAlgorithm->m_iPcVisionNo;
	iNoInspectImage = pAlgorithm->m_iNoInspectImage;

	sLotID = pAlgorithm->m_sLotID[iScanBufferIdx];
	iMzNo = pAlgorithm->m_nMzNo[iScanBufferIdx];
	iTrayNo = pAlgorithm->m_nTrayNo[iScanBufferIdx];
	iModuleNo = pAlgorithm->m_nModuleNo[iScanBufferIdx];
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

	BOOL bFound = FALSE;

	GTRegion* pInspectROIRgn;
	HObject HROIHRegion, HBarcodeImage;
	HObject HTempImageR, HTempImageG, HTempImageB;
	HObject HTempImageH, HTempImageS, HTempImageI;

	Hlong lRow1, lRow2, lCol1, lCol2;
	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	HTuple DataCodeHandle, ResultHandles, DecodedDataStrings;
	HObject SymbolXLDs, HFilteredImage;
	int iNoBarcodeString = 0;
	HTuple HStrLength;
	int iLength;

	BOOL bBarcodeRegResult = FALSE;
	CString sBarcodeResult;

	int i, j;

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();

	auto log_time_start = std::chrono::high_resolution_clock::now();
	try
	{
		for (int iImageIdx = 1; iImageIdx <= iNoInspectImage; iImageIdx++)
		{
			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_BARCODE)	// Barcode
					continue;

				for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect == TRUE)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseBarcode)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

							DilationRectangle1(HROIHRegion, &HROIHRegion, THEAPP.Struct_PreferenceStruct.m_iMatchingSearchMargin * 2 + 1, THEAPP.Struct_PreferenceStruct.m_iMatchingSearchMargin * 2 + 1);

							SmallestRectangle1(HROIHRegion, &HlRow1, &HlCol1, &HlRow2, &HlCol2);
							lRow1 = HlRow1.L();
							lRow2 = HlRow2.L();
							lCol1 = HlCol1.L();
							lCol2 = HlCol2.L();

							HTuple HCropImageWidth, HCropImageHeight;
							Hlong lCropImageWidth, lCropImageHeight;
							GetImageSize(pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1], &HCropImageWidth, &HCropImageHeight);
							lCropImageWidth = HCropImageWidth.L();
							lCropImageHeight = HCropImageHeight.L();

							if (lRow1 < 0)
								lRow1 = 0;
							if (lCol1 < 0)
								lCol1 = 0;
							if (lRow2 >= lCropImageHeight)
								lRow2 = lCropImageHeight - 1;
							if (lCol2 >= lCropImageWidth)
								lCol2 = lCropImageWidth - 1;

							// Rgb1ToGray(pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1], &HBarcodeImage);
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType == CHANNEL_TYPE_I)
								Rgb1ToGray(pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1], &HBarcodeImage);
							else if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType == CHANNEL_TYPE_R)
								Decompose3(pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1], &HBarcodeImage, &HTempImageG, &HTempImageB);
							else if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType == CHANNEL_TYPE_G)
								Decompose3(pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1], &HTempImageR, &HBarcodeImage, &HTempImageB);
							else if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType == CHANNEL_TYPE_B)
								Decompose3(pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1], &HTempImageR, &HTempImageG, &HBarcodeImage);
							else if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType == CHANNEL_TYPE_H)
							{
								Decompose3(pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1], &HTempImageR, &HTempImageG, &HTempImageB);
								TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HBarcodeImage, &HTempImageS, &HTempImageI, "hsi");
							}
							else if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType == CHANNEL_TYPE_S)
							{
								Decompose3(pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1], &HTempImageR, &HTempImageG, &HTempImageB);
								TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HBarcodeImage, &HTempImageI, "hsi");
							}
							else
								Rgb1ToGray(pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1], &HBarcodeImage);

							CropRectangle1(HBarcodeImage, &HBarcodeImage, lRow1, lCol1, lRow2, lCol2);

							pAlgorithm->m_evCopyForBarcodeDone[iCurGrabCirularBufferIdx].SetEvent();

							bFound = TRUE;
							break;
						}
					}
				}

				if (bFound)
					break;
			}

			if (bFound)
				break;
		}
	}
	catch (HException& except)
	{
		sBarcodeResult = "NOREAD";

#ifndef INLINE_MODE
		EnterCriticalSection(&THEAPP.m_csBarcodeRead);
#endif
		THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sBarcodeResult;

		THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, sBarcodeResult);

		pAlgorithm->m_evCopyForBarcodeDone[iCurGrabCirularBufferIdx].SetEvent();
		pAlgorithm->m_evBarcodeReadDone[iCurGrabCirularBufferIdx].SetEvent();

#ifndef INLINE_MODE
		LeaveCriticalSection(&THEAPP.m_csBarcodeRead);
#endif
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AlgorithmThreadDefine::BarcodeTransferThread 메모리 복사 오류] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

	if (bFound == FALSE)
		pAlgorithm->m_evCopyForBarcodeDone[iCurGrabCirularBufferIdx].SetEvent();

	CreateDataCode2dModel(HTuple("Data Matrix ECC 200"), HTuple(), HTuple(), &DataCodeHandle);

	try
	{
		// Standard Edition
		SetDataCode2dParam(DataCodeHandle, HTuple("default_parameters"), HTuple("standard_recognition"));

		GenEmptyObj(&SymbolXLDs);
		FindDataCode2d(HBarcodeImage, &SymbolXLDs, DataCodeHandle, HTuple(), HTuple(), &ResultHandles, &DecodedDataStrings);
		iNoBarcodeString = DecodedDataStrings.Length();
		if (iNoBarcodeString > 0)
		{
			TupleStrlen(DecodedDataStrings, &HStrLength);
			iLength = HStrLength[0].I();

			if (iLength >= MIN_BARCODE_CHAR_LENGTH)
			{
				bBarcodeRegResult = TRUE;
			}
		}

		if (bBarcodeRegResult == FALSE)		// Enhanced Edition
		{
			SetDataCode2dParam(DataCodeHandle, HTuple("default_parameters"), HTuple("enhanced_recognition"));

			GenEmptyObj(&SymbolXLDs);
			FindDataCode2d(HBarcodeImage, &SymbolXLDs, DataCodeHandle, HTuple(), HTuple(), &ResultHandles, &DecodedDataStrings);
			iNoBarcodeString = DecodedDataStrings.Length();
			if (iNoBarcodeString > 0)
			{
				TupleStrlen(DecodedDataStrings, &HStrLength);
				iLength = HStrLength[0].I();

				if (iLength >= MIN_BARCODE_CHAR_LENGTH)
				{
					bBarcodeRegResult = TRUE;
				}
			}
		}

		if (bBarcodeRegResult == FALSE)		// Maximum Edition
		{
			SetDataCode2dParam(DataCodeHandle, HTuple("default_parameters"), HTuple("maximum_recognition"));

			GenEmptyObj(&SymbolXLDs);
			FindDataCode2d(HBarcodeImage, &SymbolXLDs, DataCodeHandle, HTuple(), HTuple(), &ResultHandles, &DecodedDataStrings);
			iNoBarcodeString = DecodedDataStrings.Length();
			if (iNoBarcodeString > 0)
			{
				TupleStrlen(DecodedDataStrings, &HStrLength);
				iLength = HStrLength[0].I();

				if (iLength >= MIN_BARCODE_CHAR_LENGTH)
				{
					bBarcodeRegResult = TRUE;
				}
			}
		}
	}
	catch (HException& e)
	{
		bBarcodeRegResult = FALSE;
	}

	ClearDataCode2dModel(DataCodeHandle);

	CString sBarcodeNoReadSaveFolderName = THEAPP.Struct_PreferenceStruct.m_strEtcFolderPath + "CMI_Results\\Barcode_Transfer_NoRead";
	CString sBarcodeFilename;

	if (bBarcodeRegResult)
	{
		sBarcodeResult = DecodedDataStrings[0].S();
		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult = TRUE;
	}
	else
	{
		sBarcodeResult = "NOREAD";

		if (bFound)
		{
			SYSTEMTIME time;
			GetLocalTime(&time);

			sBarcodeFilename.Format("%s\\Barcode_%04d%02d%02d_%02d%02d%02d_%03d", sBarcodeNoReadSaveFolderName,
									time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

			WriteImage(HBarcodeImage, "jpg", 0, HTuple(sBarcodeFilename));
		}
	}

	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("%s/ Barcode transfer read, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, iScanBufferIdx: %d, Barcode: %s", THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo], log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iScanBufferIdx, sBarcodeResult);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));


#ifndef INLINE_MODE
	EnterCriticalSection(&THEAPP.m_csBarcodeRead);
#endif

	THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sBarcodeResult;

	THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, sBarcodeResult);

	pAlgorithm->m_evBarcodeReadDone[iCurGrabCirularBufferIdx].SetEvent();

#ifndef INLINE_MODE
	LeaveCriticalSection(&THEAPP.m_csBarcodeRead);
#endif



	return 1;
}