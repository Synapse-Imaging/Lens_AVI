#include "stdafx.h"
#include "AlgorithmPipeline.h"
#include "AlgorithmSubThread.h"
#include "ResultSaveThread.h"
#include "..\BatchService\BatchManager.h"
#include "..\Algorithm.h"
#include "..\GFunction.h"
#include "..\uScan.h"


AlgorithmPipeline::AlgorithmPipeline(Algorithm* pAlgorithm)
	: m_pAlgorithm(pAlgorithm)
	, m_AlgorithmHelper(pAlgorithm)
{
}

//////////////////////////////////////////////////////////////////////////
// Phase 0: 버퍼 준비
//////////////////////////////////////////////////////////////////////////
int AlgorithmPipeline::PrepareAlgorithmBuffer(const AlgorithmCopyParam& param)
{
	int iPcVisionNo = m_pAlgorithm->m_iVisionCamType;

	int iScanBufferIdx = 0;
	CSingleLock s(&m_pAlgorithm->m_csScanBufferIdx);
	s.Lock();
	if (param.IsFirstEntry())
	{
		++m_pAlgorithm->m_iScanBufferIdx;
		if (m_pAlgorithm->m_iScanBufferIdx >= THEAPP.m_ModelDefineInfo.m_iMaxInspectionBufferCount[iPcVisionNo])
			m_pAlgorithm->m_iScanBufferIdx = 0;
	}
	iScanBufferIdx = m_pAlgorithm->m_iScanBufferIdx;
	s.Unlock();

	try
	{
		if (param.IsFirstEntry())
		{
			m_pAlgorithm->m_bGrabFail[iScanBufferIdx] = param.bGrabFail;
			m_pAlgorithm->m_sLotID[iScanBufferIdx] = param.sLotID;
			m_pAlgorithm->m_nMzNo[iScanBufferIdx] = param.iMzNo;
			m_pAlgorithm->m_nStageNo[iScanBufferIdx] = param.iStageNo;
			m_pAlgorithm->m_nJigNo[iScanBufferIdx] = param.iJigNo;
			m_pAlgorithm->m_sTrayID[iScanBufferIdx] = param.sTrayID;
			m_pAlgorithm->m_nTrayNo[iScanBufferIdx] = param.iTrayNo;
			m_pAlgorithm->m_nModuleNo[iScanBufferIdx] = param.iModuleNo;
			m_pAlgorithm->m_iNoGrabRetry[iScanBufferIdx] = param.iNoGrabRetry;
			m_pAlgorithm->m_iNoFocusMoveRetry[iScanBufferIdx] = param.iNoFocusMoveRetry;

			for (int i = 0; i < CHANNEL_NUM; i++)
				for (int j = 0; j < MAX_IMAGE_TAB; j++)
					GenEmptyObj(&(m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][i][j]));

#ifdef INLINE_MODE
			if (param.copyMode == AlgorithmCopyParam::DFM)
			{
				for (int i = 0; i < DFM_VISION_MAX_CONVERSION; i++)
				{
					GenEmptyObj(&(m_pAlgorithm->m_HInspectSpecularBackgroundImage[iScanBufferIdx][i]));
					for (int j = 0; j < SPV_RAW_IMAGE_NUMBER; j++)
						GenEmptyObj(&(m_pAlgorithm->m_HInspectSpecularRawImage[iScanBufferIdx][i][j]));
				}

				for (int i = 0; i < DIFFUSED_VISION_MAX_CONVERSION; i++)
				{
					GenEmptyObj(&(m_pAlgorithm->m_HInspectDiffusedBackgroundImage[iScanBufferIdx][i]));
					for (int j = 0; j < DIFFUSED_RAW_IMAGE_NUMBER; j++)
						GenEmptyObj(&(m_pAlgorithm->m_HInspectDiffusedRawImage[iScanBufferIdx][i][j]));
				}
			}
#endif
		}
		else
		{
			m_pAlgorithm->m_bGrabFail[iScanBufferIdx] |= param.bGrabFail;
			m_pAlgorithm->m_iNoGrabRetry[iScanBufferIdx] += param.iNoGrabRetry;
			m_pAlgorithm->m_iNoFocusMoveRetry[iScanBufferIdx] += param.iNoFocusMoveRetry;
		}

		auto startTime = std::chrono::high_resolution_clock::now();

		if (param.bGrabFail == FALSE)
		{
			m_AlgorithmHelper.CopyGrabImages(iScanBufferIdx, param);

			BOOL bReadBarcode = FALSE;
			if (param.copyMode == AlgorithmCopyParam::NORMAL || param.copyMode == AlgorithmCopyParam::DFM)
			{
				if (param.IsBatchMode())
				{
					CBatchManager* pBatchManager = CBatchManager::GetInstance();
					bReadBarcode = pBatchManager->IsLastBatch(param.iMzNo, param.iTrayNo, param.iModuleNo, iPcVisionNo, param.pBatchInfo->iBatchIdx);
				}
				else
					bReadBarcode = TRUE;
			}

			if (bReadBarcode)
			{
				if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])
				{
					BARCODE_TRANSFER_THREAD_PARAM* pParam =
						new BARCODE_TRANSFER_THREAD_PARAM(iPcVisionNo, iScanBufferIdx, param.iCurCircularGrabIdx);
					AfxBeginThread(BarcodeTransferThread, pParam);

					CSingleLock s(&(m_pAlgorithm->m_evBarcodeReadDone[param.iCurCircularGrabIdx]));
					s.Lock();
				}
			}
		}

		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
		CString strLog;
		strLog.Format("%s/ Buffer copy, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, idx: %d",
					  THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo],
					  elapsed, param.sLotID, param.iMzNo, param.iTrayNo, param.iModuleNo, iScanBufferIdx);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		return iScanBufferIdx;
	}
	catch (HException& except)
	{
		HTuple HExp, HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		CString strLog;
		strLog.Format("Halcon Exception [PrepareInspectionBuffer] : <%s>%s",
					  CString(HOperatorName.S()), CString(HErrMsg.S()));
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		m_pAlgorithm->m_bGrabFail[iScanBufferIdx] = TRUE;
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////////
// Phase 1: 이미지 전처리
//////////////////////////////////////////////////////////////////////////
void AlgorithmPipeline::ProcessRawImageSave(const AlgorithmContext& algCtx)
{
	if (algCtx.bGrabFail)
		return;

	if (THEAPP.Struct_PreferenceStruct.m_bSaveRawImage == FALSE)
		return;

	CString strLog;
	auto startTime = std::chrono::high_resolution_clock::now();

	if (THEAPP.Struct_PreferenceStruct.m_iRawImageThreadType == RAW_IMAGE_SAVE_THREAD_PC ||
		THEAPP.Struct_PreferenceStruct.m_iRawImageThreadType == RAW_IMAGE_SAVE_THREAD_CAMERA)
	{
		m_pAlgorithm->CopyRawImage(algCtx.iCurInspectionBufferIdx);
	}
	else if (THEAPP.Struct_PreferenceStruct.m_iRawImageThreadType == RAW_IMAGE_SAVE_THREAD_THREAD)
	{
		SAVE_THREAD_PARAM* pSaveThreadIDParam = new SAVE_THREAD_PARAM(algCtx.iVisionCamType, algCtx.iCurInspectionBufferIdx, algCtx.iCurThreadIdx);

		EnterCriticalSection(&m_pAlgorithm->m_csRawImageSavingDone);
		int nSaveNotAvailableCnt = 0;
		for (int nThreadIdx = 0; nThreadIdx < THEAPP.m_iVisionInspectThreadNumber[algCtx.iVisionCamType]; nThreadIdx++)
		{
			if (m_pAlgorithm->m_bCopyThreadRun[nThreadIdx] == TRUE)
				nSaveNotAvailableCnt++;
		}
		LeaveCriticalSection(&m_pAlgorithm->m_csRawImageSavingDone);

		if (nSaveNotAvailableCnt < THEAPP.m_iVisionInspectThreadNumber[algCtx.iVisionCamType])
		{
			if (THEAPP.m_iVisionInspectThreadNumber[algCtx.iVisionCamType] >= 3 &&
				THEAPP.m_iVisionInspectThreadNumber[algCtx.iVisionCamType] - nSaveNotAvailableCnt <= 1)
				AfxBeginThread(SaveImageThread, pSaveThreadIDParam, THREAD_PRIORITY_HIGHEST);
			else
				AfxBeginThread(SaveImageThread, pSaveThreadIDParam, THREAD_PRIORITY_ABOVE_NORMAL);
		}


		CSingleLock s(&(m_pAlgorithm->m_evCopyForSavingDone[algCtx.iCurThreadIdx]));
		s.Lock();
		m_pAlgorithm->SetCopyThreadRun(algCtx.iCurThreadIdx, FALSE);
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	strLog.Format("%s/ Copy raw image, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, iInspectionBufferIdx: %d, iCurThreadIdx: %d",
				  algCtx.sVisionCamType_Short, elapsedMs, algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, algCtx.iCurInspectionBufferIdx, algCtx.iCurThreadIdx);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));
}

BOOL AlgorithmPipeline::ProcessGlobalAlign(AlgorithmContext& algCtx, BOOL bInitHomMat)
{
	CString strLog;
	auto startTime = std::chrono::high_resolution_clock::now();

	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;

	double dRotationAngleDeg = 0.0;
	double dDeltaX = 0.0;
	double dDeltaY = 0.0;

	HObject HGrayImage, HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS, HTempImageI;

	m_pAlgorithm->m_bMatchingSuccess[iBufferIdx] = TRUE;

	HTuple pHMatchingHomMat[MAX_IMAGE_TAB];
	if (bInitHomMat)
	{
		for (int i = 0; i < MAX_IMAGE_TAB; i++)
		{
			TupleGenConst(0, 0, pHMatchingHomMat + i);
			m_pAlgorithm->m_dGlobalAlignDeltaX[iBufferIdx][i] = 0;
			m_pAlgorithm->m_dGlobalAlignDeltaY[iBufferIdx][i] = 0;
		}
	}

	for (int i = algCtx.iStartImageIdx; i < algCtx.iEndImageIdx; i++)
	{
		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bUseLocalAlignMatching[i] == FALSE)
			continue;

		int iProcessChType = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLocalMatchingProcessChType[i];

		if (iProcessChType == CHANNEL_TYPE_I)
			Rgb1ToGray(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][i], &HGrayImage);
		else if (iProcessChType == CHANNEL_TYPE_R)
			Decompose3(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][i], &HGrayImage, &HTempImageG, &HTempImageB);
		else if (iProcessChType == CHANNEL_TYPE_G)
			Decompose3(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][i], &HTempImageR, &HGrayImage, &HTempImageB);
		else if (iProcessChType == CHANNEL_TYPE_B)
			Decompose3(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][i], &HTempImageR, &HTempImageG, &HGrayImage);
		else if (iProcessChType == CHANNEL_TYPE_H)
		{
			Decompose3(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][i], &HTempImageR, &HTempImageG, &HTempImageB);
			TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HGrayImage, &HTempImageS, &HTempImageI, "hsi");
		}
		else if (iProcessChType == CHANNEL_TYPE_S)
		{
			Decompose3(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][i], &HTempImageR, &HTempImageG, &HTempImageB);
			TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HGrayImage, &HTempImageI, "hsi");
		}
		else
		{
			CopyImage(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][i], &HGrayImage);
		}

		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bUseMatchingFilter[i])
		{
			m_pAlgorithm->GetMatchingPreprocessImage(HGrayImage,
													 THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iMatchingFilterType[i],
													 THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMatchingFilterTypeXSize[i],
													 THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMatchingFilterTypeYSize[i],
													 &HGrayImage);
		}

		m_pAlgorithm->m_bMatchingSuccess[iBufferIdx] = m_pAlgorithm->ImageAlignShapeMatching(
			TRUE, algCtx.iMzNo, HGrayImage,
			m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR],
			&(THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_lLAlignModelID[i]),
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bLocalAlignImage[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLocalMatchingAngleRange[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dLocalMatchingScaleMin[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dLocalMatchingScaleMax[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dLocalMatchingScore[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dLocalTeachAlignRefX[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dLocalTeachAlignRefY[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLocalMatchingMethod[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLocalMatchingTeachingRectLTX[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLocalMatchingTeachingRectLTY[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLocalMatchingTeachingRectRBX[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLocalMatchingTeachingRectRBY[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLocalMatchingSearchMarginX[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLocalMatchingSearchMarginY[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bUseMatchingAffineConstant[i],
			iPcVisionNo, iVisionCamType, algCtx.iCurThreadIdx, i,
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bCheckUseMatchingXYComp[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLocalIndexNumber[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iRadioMatchingXYRefLine[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bUseMatchingAngleComp[i],
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iMatchingAngleRefLine[i],
			&dRotationAngleDeg, &dDeltaX, &dDeltaY,
			pHMatchingHomMat + i);

		double dDeltaXmm = dDeltaX * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001;
		double dDeltaYmm = dDeltaY * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001;

		strLog.Format("%s/ Global align, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, ImageNo: %d, DeltaX: %.4lf, DeltaY: %.4lf, Rotage: %.4lf",
					  algCtx.sVisionCamType_Short, algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, i + 1, dDeltaXmm, dDeltaYmm, dRotationAngleDeg);
		THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));

		for (int iii = 0; iii < MAX_IMAGE_TAB; iii++)
		{
			if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bLocalAlignImage[i][iii] == TRUE)
			{
				m_pAlgorithm->m_dGlobalAlignDeltaX[iBufferIdx][iii] += dDeltaX;
				m_pAlgorithm->m_dGlobalAlignDeltaY[iBufferIdx][iii] += dDeltaY;
			}
		}

		if (m_pAlgorithm->m_bMatchingSuccess[iBufferIdx] == FALSE)
			break;
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	strLog.Format("%s/ Global align, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, iInspectionBufferIdx: %d, iCurThreadIdx: %d",
				  algCtx.sVisionCamType_Short, elapsedMs, algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo, iBufferIdx, algCtx.iCurThreadIdx);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

	return m_pAlgorithm->m_bMatchingSuccess[iBufferIdx];
}

void AlgorithmPipeline::ProcessHsiTrans(const AlgorithmContext& algCtx)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;

	for (int i = algCtx.iStartImageIdx; i < algCtx.iEndImageIdx; i++)
	{
		Decompose3(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][i],
				   &(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_R][i]),
				   &(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_G][i]),
				   &(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_B][i]));

		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bUseColorSpace[i])
		{
			TransFromRgb(
				m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_R][i],
				m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_G][i],
				m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_B][i],
				&(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_H][i]),
				&(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_S][i]),
				&(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_I][i]),
				"hsi");

			if (!THEAPP.m_ModelDefineInfo.m_bVisionPWM[iPcVisionNo])
				Rgb1ToGray(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][i],
						   &(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_I][i]));
		}
		else
		{
			if (!THEAPP.m_ModelDefineInfo.m_bVisionPWM[iPcVisionNo])
				Rgb1ToGray(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][i],
						   &(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_I][i]));

			GenEmptyObj(&(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_H][i]));
			GenEmptyObj(&(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_S][i]));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Phase 2: ROI 전처리
//////////////////////////////////////////////////////////////////////////
void AlgorithmPipeline::ProcessLocalAlign(const AlgorithmContext& algCtx)
{

	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	BOOL bTempROIAlignShiftResult = FALSE;
	double dAlignMatchingScore;
	int iLoaclAlignDeltaX, iLoaclAlignDeltaY;
	double dLoaclAlignDeltaAngle, dLoaclAlignDeltaAngleFixedPointX, dLoaclAlignDeltaAngleFixedPointY;

	int iMatchingPosOffsetX = 0;
	int iMatchingPosOffsetY = 0;

	HObject HROIHRegion;
	HObject HAlignRgn;
	HObject HResultXLD;
	HObject HPreProcessImage;

	// 2-Pass 구조로 로컬 얼라인 진행
	//// Pass 0: 이미지처리 미사용(선행)
	//// Pass 1: 이미지처리 사용(후행, 선행결과 의존)
	for (int iPass = 0; iPass < 2; iPass++)
	{
		BOOL bUseImageProcess = (iPass == 1);

		for (int iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
		{
			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				GTRegion* pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]
					->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					CAlgorithmParam& AlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iTabIdx];

					if (AlgorithmParam.m_bInspect == FALSE)
						continue;

					if (AlgorithmParam.m_bUseImageProcessLocalAlign != bUseImageProcess)
						continue;

					pInspectROIRgn->ResetLocalAlignResult(iCurThreadIdx);

					iMatchingPosOffsetX = 0;
					iMatchingPosOffsetY = 0;

					if (!bUseImageProcess)
						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

					// ROI Align
					if (AlgorithmParam.m_bUseROIAlign && AlgorithmParam.m_bROIAlignLocalAlignUse && AlgorithmParam.m_bROIAlignMultiModuleInspect == FALSE)
					{
						if (bUseImageProcess)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);
							m_pAlgorithm->GetPreLocalAlignResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx, &iMatchingPosOffsetX, &iMatchingPosOffsetY);
						}

						iLoaclAlignDeltaX = iLoaclAlignDeltaY = INVALID_ALIGN_RESULT;
						dLoaclAlignDeltaAngle = dLoaclAlignDeltaAngleFixedPointX = dLoaclAlignDeltaAngleFixedPointY = INVALID_ALIGN_RESULT;

						if (AlgorithmParam.m_bUseIsoColorImage)
							HAlignRgn = m_pAlgorithm->ROIAlignAlgorithm(
								m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1],
								HROIHRegion, AlgorithmParam, &(pInspectROIRgn->m_HROIAlignModelID),
								&bTempROIAlignShiftResult, &dAlignMatchingScore,
								&iLoaclAlignDeltaX, &iLoaclAlignDeltaY,
								&dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY,
								iMatchingPosOffsetX, iMatchingPosOffsetY);
						else
							HAlignRgn = m_pAlgorithm->ROIAlignAlgorithm(
								m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
								HROIHRegion, AlgorithmParam, &(pInspectROIRgn->m_HROIAlignModelID),
								&bTempROIAlignShiftResult, &dAlignMatchingScore,
								&iLoaclAlignDeltaX, &iLoaclAlignDeltaY,
								&dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY,
								iMatchingPosOffsetX, iMatchingPosOffsetY);

						if (CGFunction::ValidHRegion(HAlignRgn) == TRUE)
						{
							pInspectROIRgn->m_dLocalAlignMatchingScore[iCurThreadIdx] = dAlignMatchingScore * 100.0;
							pInspectROIRgn->m_iLocalAlignDeltaX[iCurThreadIdx] = iLoaclAlignDeltaX;
							pInspectROIRgn->m_iLocalAlignDeltaY[iCurThreadIdx] = iLoaclAlignDeltaY;
							pInspectROIRgn->m_dLocalAlignDeltaAngle[iCurThreadIdx] = dLoaclAlignDeltaAngle;
							pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iCurThreadIdx] = dLoaclAlignDeltaAngleFixedPointX;
							pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iCurThreadIdx] = dLoaclAlignDeltaAngleFixedPointY;
							pInspectROIRgn->m_HLocalAlignShapeRgn[iCurThreadIdx] = HAlignRgn;

							HTuple PhiDeg;
							TupleDeg(HTuple(pInspectROIRgn->m_dLocalAlignDeltaAngle[iCurThreadIdx]), &PhiDeg);
							double dAngle = PhiDeg.D();
							CString strJigAlign;
							strJigAlign.Format("%s Stage %d Jig %d LotID %s Tray %d Module %d DeltaX %.03f DeltaY %.03f DeltaAngle %.02f",
										  algCtx.sVisionCamType_Short,
										  algCtx.iStageNo,
										  algCtx.iJigNo,
										  algCtx.sLotID,
										  algCtx.iTrayNo,
										  algCtx.iModuleNo,
										  pInspectROIRgn->m_iLocalAlignDeltaX[iCurThreadIdx] * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelSize() / 1000,
										  pInspectROIRgn->m_iLocalAlignDeltaY[iCurThreadIdx] * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelSize() / 1000,
										  dAngle);
							THEAPP.m_log_etc->info("{}", LOG_CSTR(strJigAlign));
						}
					}

					// 비등방 (Aniso Align)
					if (AlgorithmParam.m_bUseROIAnisoAlign && AlgorithmParam.m_bROIAnisoAlignLocalAlignUse && AlgorithmParam.m_bROIAnisoAlignMultiModuleInspect == FALSE)
					{
						if (bUseImageProcess)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);
							m_pAlgorithm->GetPreLocalAlignResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx, &iMatchingPosOffsetX, &iMatchingPosOffsetY);
						}

						iLoaclAlignDeltaX = iLoaclAlignDeltaY = INVALID_ALIGN_RESULT;
						dLoaclAlignDeltaAngle = dLoaclAlignDeltaAngleFixedPointX = dLoaclAlignDeltaAngleFixedPointY = INVALID_ALIGN_RESULT;

						if (AlgorithmParam.m_bUseAnisoColorImage)
							HAlignRgn = m_pAlgorithm->ROIAnisoAlignAlgorithm(
								m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1],
								HROIHRegion, AlgorithmParam, &(pInspectROIRgn->m_HROIAlignModelID),
								&bTempROIAlignShiftResult, &dAlignMatchingScore,
								&iLoaclAlignDeltaX, &iLoaclAlignDeltaY,
								&dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY,
								iMatchingPosOffsetX, iMatchingPosOffsetY);
						else
							HAlignRgn = m_pAlgorithm->ROIAnisoAlignAlgorithm(
								m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
								HROIHRegion, AlgorithmParam, &(pInspectROIRgn->m_HROIAlignModelID),
								&bTempROIAlignShiftResult, &dAlignMatchingScore,
								&iLoaclAlignDeltaX, &iLoaclAlignDeltaY,
								&dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY,
								iMatchingPosOffsetX, iMatchingPosOffsetY);

						if (CGFunction::ValidHRegion(HAlignRgn) == TRUE)
						{
							pInspectROIRgn->m_dLocalAlignMatchingScore[iCurThreadIdx] = dAlignMatchingScore * 100.0;
							pInspectROIRgn->m_iLocalAlignDeltaX[iCurThreadIdx] = iLoaclAlignDeltaX;
							pInspectROIRgn->m_iLocalAlignDeltaY[iCurThreadIdx] = iLoaclAlignDeltaY;
							pInspectROIRgn->m_dLocalAlignDeltaAngle[iCurThreadIdx] = dLoaclAlignDeltaAngle;
							pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iCurThreadIdx] = dLoaclAlignDeltaAngleFixedPointX;
							pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iCurThreadIdx] = dLoaclAlignDeltaAngleFixedPointY;
							pInspectROIRgn->m_HLocalAlignShapeRgn[iCurThreadIdx] = HAlignRgn;
						}
					}

					// Template
					if (AlgorithmParam.m_bUsePartCheck && AlgorithmParam.m_bPartCheckLocalAlignUse)
					{
						if (bUseImageProcess)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);
							m_pAlgorithm->GetPreLocalAlignResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx, &iMatchingPosOffsetX, &iMatchingPosOffsetY);
						}

						if (AlgorithmParam.m_bUseImageProcess || AlgorithmParam.m_bUseImageProcessFilter)
						{
							m_pAlgorithm->GetPreprocessImage(TRUE,
															 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
															 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
															 HROIHRegion, AlgorithmParam, &HPreProcessImage);

							m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
													 HPreProcessImage, HROIHRegion, AlgorithmParam, &HResultXLD,
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1],
													 NULL, NULL, iMatchingPosOffsetX, iMatchingPosOffsetY);
						}
						else
						{
							m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
													 HROIHRegion, AlgorithmParam, &HResultXLD,
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1],
													 NULL, NULL, iMatchingPosOffsetX, iMatchingPosOffsetY);
						}
					}

					break;
				}
			}
		}
	}
}

void AlgorithmPipeline::ProcessFitting(const AlgorithmContext& algCtx)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	HObject HROIHRegion;
	HObject HResultXLD;
	HObject HPreProcessImage;

	for (int iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
	{
		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			GTRegion* pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]
				->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
				continue;

			if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
				continue;

			for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
			{
				CAlgorithmParam& AlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iTabIdx];

				if (AlgorithmParam.m_bInspect == FALSE)
					continue;

				HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

				if (AlgorithmParam.m_bUseROIAlign && AlgorithmParam.m_bROIAlignLocalAlignUse && AlgorithmParam.m_bROIAlignMultiModuleInspect == FALSE)
					continue;

				if (AlgorithmParam.m_bUseROIAnisoAlign && AlgorithmParam.m_bROIAnisoAlignLocalAlignUse && AlgorithmParam.m_bROIAnisoAlignMultiModuleInspect == FALSE)
					continue;

				if (AlgorithmParam.m_bUsePartCheck && AlgorithmParam.m_bPartCheckLocalAlignUse)
					continue;

				if (AlgorithmParam.m_bUseImageProcessLocalAlign)
				{
					if (AlgorithmParam.m_bUseImageProcessLocalAlignAddLineFit == FALSE)
						m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);
				}

				if (AlgorithmParam.m_bUseImageProcess || AlgorithmParam.m_bUseImageProcessFilter)
				{
					m_pAlgorithm->GetPreprocessImage(TRUE,
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
													 HROIHRegion, AlgorithmParam, &HPreProcessImage);

					m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
											 HPreProcessImage, HROIHRegion, AlgorithmParam, &HResultXLD,
											 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
											 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
				}
				else
				{
					m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
											 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
											 HROIHRegion, AlgorithmParam, &HResultXLD,
											 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
											 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
				}

				break;
			}
		}
	}
}

void AlgorithmPipeline::ProcessFittingAdd(const AlgorithmContext& algCtx)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	HObject HROIHRegion;
	HObject HResultXLD;
	HObject HPreProcessImage;

	for (int iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
	{
		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			GTRegion* pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]
				->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
				continue;

			if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
				continue;

			for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
			{
				CAlgorithmParam& AlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iTabIdx];

				if (AlgorithmParam.m_bInspect == FALSE)
					continue;

				HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

				if (AlgorithmParam.m_bUseROIAlign && AlgorithmParam.m_bROIAlignMultiModuleInspect == FALSE)
					continue;

				if (AlgorithmParam.m_bUseROIAnisoAlign && AlgorithmParam.m_bROIAnisoAlignMultiModuleInspect == FALSE)
					continue;

				if (AlgorithmParam.m_bUsePartCheck && AlgorithmParam.m_bPartCheckLocalAlignUse)
					continue;

				if (AlgorithmParam.m_bUseImageProcessLocalAlign && AlgorithmParam.m_bUseImageProcessLocalAlignAddLineFit)
					m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);
				else
					continue;

				if (AlgorithmParam.m_bUseImageProcess || AlgorithmParam.m_bUseImageProcessFilter)
				{
					m_pAlgorithm->GetPreprocessImage(TRUE,
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
													 HROIHRegion, AlgorithmParam, &HPreProcessImage);

					m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
											 HPreProcessImage, HROIHRegion, AlgorithmParam, &HResultXLD,
											 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
											 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
				}
				else
				{
					m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
											 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
											 HROIHRegion, AlgorithmParam, &HResultXLD,
											 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
											 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
				}

				break;
			}
		}
	}
}

void AlgorithmPipeline::ProcessDontCare(const AlgorithmContext& algCtx)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	HObject HROIHRegion;
	HObject HResultXLD;
	HObject HPreProcessImage;
	HObject HDontCareResultRgn;

	for (int iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
	{
		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			GTRegion* pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]
				->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
				continue;

			if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_DONTCARE)
				continue;

			for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
			{
				CAlgorithmParam& AlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iTabIdx];

				if (AlgorithmParam.m_bInspect == FALSE)
					continue;

				pInspectROIRgn->ResetDontCareResult(iCurThreadIdx);

				HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

				if (AlgorithmParam.m_bUseImageProcessLocalAlign)
					m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);

				if (AlgorithmParam.m_bUseImageProcess || AlgorithmParam.m_bUseImageProcessFilter)
				{
					m_pAlgorithm->GetPreprocessImage(TRUE,
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
													 HROIHRegion, AlgorithmParam, &HPreProcessImage);

					HDontCareResultRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
																  HPreProcessImage, HROIHRegion, AlgorithmParam, &HResultXLD,
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
				}
				else
				{
					HDontCareResultRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
																  HROIHRegion, AlgorithmParam, &HResultXLD,
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
				}

				if (CGFunction::ValidHRegion(HDontCareResultRgn) == TRUE)
					pInspectROIRgn->m_HDontCareResultRgn[iCurThreadIdx] = HDontCareResultRgn;

				break;
			}
		}
	}
}

void AlgorithmPipeline::ProcessGenerate(const AlgorithmContext& algCtx)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	HObject HROIHRegion;
	HObject HResultXLD;
	HObject HPreProcessImage;
	HObject HGenerateResultRgn;

	for (int iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
	{
		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			GTRegion* pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]
				->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
				continue;

			if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_GENERATE)
				continue;

			for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
			{
				CAlgorithmParam& AlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iTabIdx];

				if (AlgorithmParam.m_bInspect == FALSE)
					continue;

				pInspectROIRgn->ResetGenerateResult(iCurThreadIdx);

				HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

				if (AlgorithmParam.m_bUseImageProcessLocalAlign)
					m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);

				if (AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
					m_pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);

				if (AlgorithmParam.m_bUseImageProcess || AlgorithmParam.m_bUseImageProcessFilter)
				{
					m_pAlgorithm->GetPreprocessImage(TRUE,
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
													 HROIHRegion, AlgorithmParam, &HPreProcessImage);

					HGenerateResultRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
																  HPreProcessImage, HROIHRegion, AlgorithmParam, &HResultXLD,
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
				}
				else
				{
					HGenerateResultRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
																  HROIHRegion, AlgorithmParam, &HResultXLD,
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
				}

				if (CGFunction::ValidHRegion(HGenerateResultRgn) == TRUE)
					pInspectROIRgn->m_HGenerateResultRgn[iCurThreadIdx] = HGenerateResultRgn;
				else
					GenEmptyObj(&(pInspectROIRgn->m_HGenerateResultRgn[iCurThreadIdx]));

				break;
			}
		}
	}
}

void AlgorithmPipeline::ProcessMask(const AlgorithmContext& algCtx)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	HObject HROIHRegion;
	HObject HResultXLD;
	HObject HPreProcessImage;
	HObject HMaskResultRgn;

	for (int iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
	{
		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			GTRegion* pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]
				->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
				continue;

			if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_MASK)
				continue;

			for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
			{
				CAlgorithmParam& AlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iTabIdx];

				if (AlgorithmParam.m_bInspect == FALSE)
					continue;

				pInspectROIRgn->ResetMaskResult(iCurThreadIdx);

				HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

				if (AlgorithmParam.m_bUseImageProcessLocalAlign)
					m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);

				if (AlgorithmParam.m_bUseImageProcessGenerate)
					m_pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);

				if (AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
					m_pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);

				if (AlgorithmParam.m_bUseImageProcess || AlgorithmParam.m_bUseImageProcessFilter)
				{
					m_pAlgorithm->GetPreprocessImage(TRUE,
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
													 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
													 HROIHRegion, AlgorithmParam, &HPreProcessImage);

					HMaskResultRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
															  HPreProcessImage, HROIHRegion, AlgorithmParam, &HResultXLD,
															  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
															  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
				}
				else
				{
					HMaskResultRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
															  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
															  HROIHRegion, AlgorithmParam, &HResultXLD,
															  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
															  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
				}

				if (CGFunction::ValidHRegion(HMaskResultRgn) == TRUE)
					pInspectROIRgn->m_HMaskResultRgn[iCurThreadIdx] = HMaskResultRgn;
				else
					GenEmptyObj(&(pInspectROIRgn->m_HMaskResultRgn[iCurThreadIdx]));

				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Phase 3: 메인 검사
//////////////////////////////////////////////////////////////////////////
#if !defined (UAVI_CHS_KS) & !defined (UAVI_CHS_TV) & !defined (UAVI_CHS_WZ)
void AlgorithmPipeline::ProcessFAIMeasure(const AlgorithmContext& algCtx, BOOL bInitFAIItem)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	int iTrayNo = algCtx.iTrayNo;
	int iModuleNo = algCtx.iModuleNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	int iImageIdx, iROIIndex, iTabIdx;
	int i, j;
	GTRegion* pInspectROIRgn;
	HObject HROIHRegion;
	HObject HResultXLD;
	HObject HPreProcessImage;

	for (int iFaiParamIndex = 0; iFaiParamIndex < MAX_FAI_PARAMETER; iFaiParamIndex++)
	{
		if (bInitFAIItem)
		{
			GenEmptyObj(&(m_pAlgorithm->m_HDefectRgn_FAI[iBufferIdx]));

			for (i = 0; i < MAX_FAI_ITEM; i++)
			{
				GenEmptyObj(&(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex]));
				for (j = 0; j < MAX_FAI_REVIEW_IMAGE; j++)
				{
					GenEmptyObj(&(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][i][iFaiParamIndex][j]));
					m_pAlgorithm->m_iViewportCenter_FAI_Item[iBufferIdx][i][iFaiParamIndex][j].x = -1;
					m_pAlgorithm->m_iViewportCenter_FAI_Item[iBufferIdx][i][iFaiParamIndex][j].y = -1;
				}
			}
		}

		HObject HFAIMeasureRgn;
		HObject HCrossXLD;
		int iFAINumber;

		BOOL bMeasureROIFound;
		HObject HMeasureRetRgn;

		HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
		double dMeasureLineStartX, dMeasureLineStartY, dMeasureLineEndX, dMeasureLineEndY;
		HTuple HlNoFoundNumber;
		Hlong lNoFoundNumber;
		double dIntersectionPosY;

		HTuple HRoiArea, HRoiCenterX, HRoiCenterY;
		DPOINT dRoiCenterPoint;

		int iPocketNumber = 1;
		if ((iModuleNo % 2) == 1)
			iPocketNumber = 1;
		else
			iPocketNumber = 2;

		for (iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]
					->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL) continue;
				if (pInspectROIRgn->miTeachImageIndex != iImageIdx) continue;
				if (pInspectROIRgn->miFaiRoiIndex != iFaiParamIndex) continue;
				if (pInspectROIRgn->miInspectionType < INSPECTION_TYPE_MEASURE_FIRST ||
					pInspectROIRgn->miInspectionType > INSPECTION_TYPE_MEASURE_LAST) continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					CAlgorithmParam& AlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iTabIdx];
					if (AlgorithmParam.m_bInspect == FALSE) continue;

					HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

					if (AlgorithmParam.m_bUseImageProcessLocalAlign)
						m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);
					if (AlgorithmParam.m_bUseImageProcessGenerate)
						m_pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);
					if (AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
						m_pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, AlgorithmParam, &HROIHRegion, iCurThreadIdx);

					if (AlgorithmParam.m_bUseImageProcess || AlgorithmParam.m_bUseImageProcessFilter)
					{
						m_pAlgorithm->GetPreprocessImage(TRUE,
														 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
														 m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
														 HROIHRegion, AlgorithmParam, &HPreProcessImage);
						HMeasureRetRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
																  HPreProcessImage, HROIHRegion, AlgorithmParam, &HResultXLD,
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
					}
					else
					{
						HMeasureRetRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType][iImageIdx - 1],
																  HROIHRegion, AlgorithmParam, &HResultXLD,
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChType],
																  m_pAlgorithm->m_HInspectCImage[iBufferIdx][AlgorithmParam.m_iProcessChTypeDC][iImageIdx - 1]);
					}

					if (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 &&
						pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
					{
						if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_MEASURE_POINT)
						{
							THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex]
								.m_dMeasureCPoint[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].x =
								(pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX +
								 pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
							THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex]
								.m_dMeasureCPoint[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].y =
								(pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY +
								 pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;

							if (CGFunction::ValidHXLD(HResultXLD))
								ConcatObj(THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex]
										  .m_HMeasureXLD[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID],
										  HResultXLD,
										  &(THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex]
											.m_HMeasureXLD[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID]));

							if (CGFunction::ValidHRegion(HMeasureRetRgn))
								ConcatObj(THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex]
										  .m_HMeasureRgn[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID],
										  HMeasureRetRgn,
										  &(THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex]
											.m_HMeasureRgn[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID]));
						}
						else if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_BLEND_POINT)
						{
							THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex]
								.m_dBlendCPoint[iPcVisionNo][pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].x =
								(pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX +
								 pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
							THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex]
								.m_dBlendCPoint[iPcVisionNo][pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].y =
								(pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY +
								 pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
						}
					}
				}
			}
		}

		m_pAlgorithm->InspectFAI(TRUE, iBufferIdx, iFaiParamIndex);

		for (i = 0; i < MAX_FAI_ITEM; i++)
		{
			if (CGFunction::ValidHRegion(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex]))
				Union1(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex], &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex]);
		}
	}
}
#endif

void AlgorithmPipeline::ProcessBarcodeOcr(const AlgorithmContext& algCtx)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	if (!THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])
		return;

	HObject HROIHRegion, HDefectRgn, HResultXLD, HPreProcessImage;

	for (int iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
	{
		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			GTRegion* pROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]
				->m_pInspectionArea->GetChildTRegion(iROIIndex);
			if (pROI == NULL) continue;
			if (pROI->miTeachImageIndex != iImageIdx) continue;
			if (pROI->miInspectionType != INSPECTION_TYPE_BARCODE) continue;

			for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
			{
				CAlgorithmParam& param = pROI->m_AlgorithmParam[iTabIdx];
				if (!param.m_bInspect) continue;

				// 바코드 검사
				if (param.m_bUseBarcode)
				{
					HROIHRegion = pROI->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);
					if (CGFunction::ValidHRegion(HROIHRegion) == FALSE) continue;

					if (param.m_bUseImageProcessLocalAlign)
						m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, param, &HROIHRegion, iCurThreadIdx);
					if (param.m_bUseImageProcessGenerate)
						m_pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, param, &HROIHRegion, iCurThreadIdx);
					if (param.m_bUseImageProcessDontCare || param.m_bUseImageProcessDontCare2)
						m_pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, param, &HROIHRegion, iCurThreadIdx);

					if (param.m_bUseImageProcess || param.m_bUseImageProcessFilter)
					{
						m_pAlgorithm->GetPreprocessImage(TRUE,
														 m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChType][iImageIdx - 1],
														 m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChType],
														 HROIHRegion, param, &HPreProcessImage);
						HDefectRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx,
															  pROI, iTabIdx, HPreProcessImage, HROIHRegion, param, &HResultXLD,
															  m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_I],
															  m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChTypeDC][iImageIdx - 1]);
					}
					else
					{
						HDefectRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx,
															  pROI, iTabIdx,
															  m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_I][iImageIdx - 1],
															  HROIHRegion, param, &HResultXLD,
															  m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_I],
															  m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChTypeDC][iImageIdx - 1]);
					}

					if (CGFunction::ValidHRegion(HDefectRgn) == TRUE)
						m_AlgorithmHelper.StoreDefectRegion(iBufferIdx, iImageIdx, pROI->miInspectionType, param.m_iDefectType, HDefectRgn);
				}

				// OCR 검사
				if (param.m_bUseOCR)
				{
					HROIHRegion = pROI->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);
					if (CGFunction::ValidHRegion(HROIHRegion) == FALSE) continue;

					if (param.m_bUseImageProcessLocalAlign)
						m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, param, &HROIHRegion, iCurThreadIdx);
					if (param.m_bUseImageProcessGenerate)
						m_pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, param, &HROIHRegion, iCurThreadIdx);
					if (param.m_bUseImageProcessDontCare || param.m_bUseImageProcessDontCare2)
						m_pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, param, &HROIHRegion, iCurThreadIdx);

					HDefectRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx,
														  pROI, iTabIdx,
														  m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_I][iImageIdx - 1],
														  HROIHRegion, param, &HResultXLD,
														  m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_I],
														  m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChTypeDC][iImageIdx - 1]);

					if (CGFunction::ValidHRegion(HDefectRgn) == TRUE)
						m_AlgorithmHelper.StoreDefectRegion(iBufferIdx, iImageIdx, pROI->miInspectionType, param.m_iDefectType, HDefectRgn);
				}
			}
		}
	}
}

void AlgorithmPipeline::ProcessInspection(const AlgorithmContext& algCtx, BOOL bInitDefectRgn)
{
	CString strLog;
	auto log_time_start = std::chrono::high_resolution_clock::now();

	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	ResultCollector collector;

	// 불량 영역 초기화
	if (bInitDefectRgn)
	{
		for (int i = 0; i < MAX_IMAGE_TAB; i++)
		{
			GenEmptyObj(&(m_pAlgorithm->m_HDefectRgn_ImageNo[iBufferIdx][i]));
			GenEmptyObj(&(m_pAlgorithm->m_HDefectRgn_ImageNo_Coax[iBufferIdx][i]));
			for (int j = 0; j < MAX_INSPECTION_TYPE; j++)
			{
				for (int k = 0; k < MAX_DEFECT_NAME; k++)
				{
					GenEmptyObj(&(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][j][k]));
					GenEmptyObj(&(m_pAlgorithm->m_HDefectRgn_MultiFocus_Ring_Particle[iBufferIdx][i][j][k]));
					GenEmptyObj(&(m_pAlgorithm->m_HDefectRgn_DefectName_Coax[iBufferIdx][j][k]));
				}
			}
		}
	}

	// 저장 디렉토리 준비
	std::vector<std::string> saveDirectories = m_AlgorithmHelper.GetSaveDirectories(algCtx);

	std::string roiImageSaveLocalDir = std::string(
		THEAPP.m_FileBase.m_strAIFolder_Local[iPcVisionNo][algCtx.iMzNo - 1][algCtx.iTrayNo - 1]);

	std::string roiImageSaveLASDir = std::string(
		THEAPP.m_FileBase.m_strAIFolder_LAS[iPcVisionNo][algCtx.iMzNo - 1][algCtx.iTrayNo - 1]);

	bool saveLocal = static_cast<bool>(THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal);
	bool saveLAS = static_cast<bool>(THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS);
	bool isSameDirectory = (roiImageSaveLocalDir == roiImageSaveLASDir);

	// 메인 검사 루프
	for (int iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
	{
		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			GTRegion* pInspectROIRgn = THEAPP.m_pDualModelDataManager[algCtx.iDualModelData][iPcVisionNo]
				->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;
			if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
				continue;
			if (pInspectROIRgn->miInspectionType < INSPECTION_TYPE_FIRST ||
				pInspectROIRgn->miInspectionType > INSPECTION_TYPE_LAST)
				continue;

			for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
			{
				if (!pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					continue;
				if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_BARCODE)
					continue;

				ROIAlgorithmContext roiCtx{
					algCtx, m_pAlgorithm, pInspectROIRgn,
					iImageIdx, iROIIndex, iTabIdx
				};
				m_AlgorithmHelper.BuildROIContext(roiCtx);

				if (CGFunction::ValidHRegion(roiCtx.HROIHRegion) == FALSE)
					continue;

				m_AlgorithmHelper.ApplyPreprocessing(roiCtx);
				m_AlgorithmHelper.InspectAlign(roiCtx);

				if (!m_AlgorithmHelper.InspectCosmetic(roiCtx))
					continue;

				if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[algCtx.iPcVisionNo])
					continue;

#if defined(LGITAI)
				m_AlgorithmHelper.SubmitLGAI(roiCtx, collector, roiImageSaveLocalDir, saveDirectories);
#elif defined(SYAI)
				m_AlgorithmHelper.SubmitSYAI(roiCtx, collector, saveDirectories);
#else
				if (CGFunction::ValidHRegion(roiCtx.HDefectRgn))
					m_AlgorithmHelper.StoreDefectRegion(iBufferIdx, iImageIdx,
														roiCtx.pInspectROIRgn->miInspectionType,
														roiCtx.pInspectROIRgn->m_AlgorithmParam[roiCtx.iTabIdx].m_iDefectType,
														roiCtx.HDefectRgn);
#endif
			}
		}
	}

#if defined(LGITAI)
	m_AlgorithmHelper.CollectLGAIResults(algCtx, collector);
#elif defined(SYAI)
	m_AlgorithmHelper.CollectSYAIResults(collector, roiImageSaveLocalDir, roiImageSaveLASDir, isSameDirectory, saveLocal, saveLAS);
#endif

	// 검사 완료 로깅
	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		log_time_end - log_time_start).count();

	strLog.Format("%s/ Algorithm(%d-%d), Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, iInspectionBufferIdx: %d, iCurThreadIdx: %d",
				  algCtx.sVisionCamType_Short, 1 + algCtx.iStartImageIdx, algCtx.iEndImageIdx,
				  log_time_ms, algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo,
				  algCtx.iCurInspectionBufferIdx, algCtx.iCurThreadIdx);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));
}

void AlgorithmPipeline::PostProcess(const AlgorithmContext& algCtx)
{
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
	bool isCHS = (strEquipModel == "CHS-W" || strEquipModel == "CHS-Z" ||
				  strEquipModel == "CHS-K" || strEquipModel == "CHS-S" ||
				  strEquipModel == "CHS-T" || strEquipModel == "CHS-V");
	if (!isCHS)
		return;

	int iBufferIdx = algCtx.iCurInspectionBufferIdx;

	// Lens inspection type 인덱스 검색
	int iInspectionTypeLensIndex = -1;
	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		if (g_strInspectionTypeName[i] == "Lens") {
			iInspectionTypeLensIndex = i;
			break;
		}
	}
	if (iInspectionTypeLensIndex < 0)
		return;

	// Phase 1: Ring Multi-Focus Particle 중복 제거 
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		auto& particleRgn = m_pAlgorithm->m_HDefectRgn_MultiFocus_Ring_Particle
			[iBufferIdx][i][iInspectionTypeLensIndex][DEFECT_CODE_LENS_PARTICLE];

		if (CGFunction::ValidHRegion(particleRgn) == FALSE)
			continue;

		Union1(particleRgn, &particleRgn);
		Connection(particleRgn, &particleRgn);

		HTuple HBlobCount;
		CountObj(particleRgn, &HBlobCount);
		Hlong lBlobCount = HBlobCount.L();

		for (Hlong j = 0; j < lBlobCount; j++)
		{
			HObject HSelectedBlob;
			HTuple HSelectedBlobArea, HTrash;
			SelectObj(particleRgn, &HSelectedBlob, j + 1);
			AreaCenter(HSelectedBlob, &HSelectedBlobArea, &HTrash, &HTrash);
			Hlong lSelectedBlobArea = HSelectedBlobArea.L();

			BOOL bAreaFind = FALSE;
			for (int k = 0; k < MAX_IMAGE_TAB; k++)
			{
				if (i == k) continue;
				if (bAreaFind) break;

				auto& otherRgn = m_pAlgorithm->m_HDefectRgn_MultiFocus_Ring_Particle
					[iBufferIdx][k][iInspectionTypeLensIndex][DEFECT_CODE_LENS_PARTICLE];

				if (CGFunction::ValidHRegion(otherRgn) == FALSE)
					continue;

				Union1(otherRgn, &otherRgn);
				Connection(otherRgn, &otherRgn);

				HTuple HBlobCount2;
				CountObj(otherRgn, &HBlobCount2);
				Hlong lBlobCount2 = HBlobCount2.L();

				for (Hlong l = 0; l < lBlobCount2; l++)
				{
					HObject HSelectedBlob2;
					HTuple HSelectedBlobArea2;
					SelectObj(otherRgn, &HSelectedBlob2, l + 1);

					HObject HIntersectBlob;
					Intersection(HSelectedBlob, HSelectedBlob2, &HIntersectBlob);
					if (CGFunction::ValidHRegion(HIntersectBlob) == FALSE)
						continue;

					AreaCenter(HSelectedBlob2, &HSelectedBlobArea2, &HTrash, &HTrash);
					Hlong lSelectedBlobArea2 = HSelectedBlobArea2.L();

					// 현재 blob이 더 크면 -> 현재 이미지에서 제거 (작은 쪽 유지)
					if (lSelectedBlobArea > lSelectedBlobArea2) {
						Difference(particleRgn, HSelectedBlob, &particleRgn);
						bAreaFind = TRUE;
						break;
					}
				}
			}
		}
	}

	// Phase 2: Multi-Focus Particle -> 메인 결함 영역에 병합 
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		auto& particleRgn = m_pAlgorithm->m_HDefectRgn_MultiFocus_Ring_Particle
			[iBufferIdx][i][iInspectionTypeLensIndex][DEFECT_CODE_LENS_PARTICLE];

		Union1(particleRgn, &particleRgn);
		ConcatObj(m_pAlgorithm->m_HDefectRgn_ImageNo[iBufferIdx][i],
				  particleRgn, &(m_pAlgorithm->m_HDefectRgn_ImageNo[iBufferIdx][i]));
		ConcatObj(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][iInspectionTypeLensIndex][DEFECT_CODE_LENS_PARTICLE],
				  particleRgn, &(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][iInspectionTypeLensIndex][DEFECT_CODE_LENS_PARTICLE]));
	}

	// Phase 3: Ring blob을 Coaxial에서 제거 
	Union1(m_pAlgorithm->m_HDefectRgn_Lens_Ring[iBufferIdx],
		   &m_pAlgorithm->m_HDefectRgn_Lens_Ring[iBufferIdx]);
	Connection(m_pAlgorithm->m_HDefectRgn_Lens_Ring[iBufferIdx],
			   &m_pAlgorithm->m_HDefectRgn_Lens_Ring[iBufferIdx]);

	HTuple HRingBlobCount;
	CountObj(m_pAlgorithm->m_HDefectRgn_Lens_Ring[iBufferIdx], &HRingBlobCount);
	Hlong lRingBlobCount = HRingBlobCount.L();

	for (Hlong i = 0; i < lRingBlobCount; i++)
	{
		HObject HSelectedBlob;
		SelectObj(m_pAlgorithm->m_HDefectRgn_Lens_Ring[iBufferIdx], &HSelectedBlob, i + 1);

		// Coaxial ImageNo에서 Ring blob 제거
		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			auto& coaxImgRgn = m_pAlgorithm->m_HDefectRgn_ImageNo_Coax[iBufferIdx][j];
			Union1(coaxImgRgn, &coaxImgRgn);
			Connection(coaxImgRgn, &coaxImgRgn);

			HTuple HCoaxBlobCount;
			CountObj(coaxImgRgn, &HCoaxBlobCount);
			Hlong lCoaxBlobCount = HCoaxBlobCount.L();
			for (Hlong k = 0; k < lCoaxBlobCount; k++)
			{
				HObject HCoaxBlob, HIntersect;
				SelectObj(coaxImgRgn, &HCoaxBlob, k + 1);
				Intersection(HSelectedBlob, HCoaxBlob, &HIntersect);
				if (CGFunction::ValidHRegion(HIntersect) == TRUE)
					Difference(coaxImgRgn, HCoaxBlob, &coaxImgRgn);
			}
		}

		// Coaxial DefectName에서 Ring blob 제거
		for (int k = 0; k < MAX_DEFECT_NAME; k++)
		{
			auto& coaxDefRgn = m_pAlgorithm->m_HDefectRgn_DefectName_Coax[iBufferIdx][iInspectionTypeLensIndex][k];
			Union1(coaxDefRgn, &coaxDefRgn);
			Connection(coaxDefRgn, &coaxDefRgn);

			HTuple HCoaxDefBlobCount;
			CountObj(coaxDefRgn, &HCoaxDefBlobCount);
			Hlong lCoaxDefBlobCount = HCoaxDefBlobCount.L();
			for (Hlong l = 0; l < lCoaxDefBlobCount; l++)
			{
				HObject HCoaxBlob, HIntersect;
				SelectObj(coaxDefRgn, &HCoaxBlob, l + 1);
				Intersection(HSelectedBlob, HCoaxBlob, &HIntersect);
				if (CGFunction::ValidHRegion(HIntersect) == TRUE)
					Difference(coaxDefRgn, HCoaxBlob, &coaxDefRgn);
			}
		}
	}

	// Phase 4: 정리된 Coaxial -> 메인에 병합 
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		Union1(m_pAlgorithm->m_HDefectRgn_ImageNo_Coax[iBufferIdx][i],
			   &m_pAlgorithm->m_HDefectRgn_ImageNo_Coax[iBufferIdx][i]);
		ConcatObj(m_pAlgorithm->m_HDefectRgn_ImageNo[iBufferIdx][i],
				  m_pAlgorithm->m_HDefectRgn_ImageNo_Coax[iBufferIdx][i],
				  &(m_pAlgorithm->m_HDefectRgn_ImageNo[iBufferIdx][i]));
	}
	for (int i = 0; i < MAX_DEFECT_NAME; i++)
	{
		Union1(m_pAlgorithm->m_HDefectRgn_DefectName_Coax[iBufferIdx][iInspectionTypeLensIndex][i],
			   &m_pAlgorithm->m_HDefectRgn_DefectName_Coax[iBufferIdx][iInspectionTypeLensIndex][i]);
		ConcatObj(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][iInspectionTypeLensIndex][i],
				  m_pAlgorithm->m_HDefectRgn_DefectName_Coax[iBufferIdx][iInspectionTypeLensIndex][i],
				  &(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][iInspectionTypeLensIndex][i]));
	}
}

void AlgorithmPipeline::ProcessDefectMerge(const AlgorithmContext& algCtx)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;

	// DefectName Union
	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		for (int j = 0; j < MAX_DEFECT_NAME; j++)
		{
			if (CGFunction::ValidHRegion(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][i][j]) == TRUE)
				Union1(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][i][j],
					   &(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][i][j]));
		}
	}

	// FAI 병합
	if (CGFunction::ValidHRegion(m_pAlgorithm->m_HDefectRgn_FAI[iBufferIdx]))
		Union1(m_pAlgorithm->m_HDefectRgn_FAI[iBufferIdx],
			   &(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][INSPECTION_TYPE_BARCODE - 1][DEFECT_CODE_FAI_NG]));

	// InspectionType 병합
	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		GenEmptyObj(&(m_pAlgorithm->m_HDefectRgn_InspectionType[iBufferIdx][i]));
		for (int j = 0; j < MAX_DEFECT_NAME; j++)
		{
			if (CGFunction::ValidHRegion(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][i][j]) == TRUE)
				ConcatObj(m_pAlgorithm->m_HDefectRgn_InspectionType[iBufferIdx][i],
						  m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][i][j],
						  &(m_pAlgorithm->m_HDefectRgn_InspectionType[iBufferIdx][i]));
		}
		if (CGFunction::ValidHRegion(m_pAlgorithm->m_HDefectRgn_InspectionType[iBufferIdx][i]))
			Union1(m_pAlgorithm->m_HDefectRgn_InspectionType[iBufferIdx][i],
				   &(m_pAlgorithm->m_HDefectRgn_InspectionType[iBufferIdx][i]));
	}

	// ImageNo Union
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (CGFunction::ValidHRegion(m_pAlgorithm->m_HDefectRgn_ImageNo[iBufferIdx][i]) == TRUE)
			Union1(m_pAlgorithm->m_HDefectRgn_ImageNo[iBufferIdx][i],
				   &(m_pAlgorithm->m_HDefectRgn_ImageNo[iBufferIdx][i]));
	}
}

void AlgorithmPipeline::ProcessMonitoringLog(const AlgorithmContext& algCtx)
{
	if (!THEAPP.Struct_PreferenceStruct.m_bUseSaveGVEdgeMornitoringLog)
		return;

	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iDualModelData = algCtx.iDualModelData;
	int iMzNo = algCtx.iMzNo;
	int iTrayNo = algCtx.iTrayNo;
	int iModuleNo = algCtx.iModuleNo;
	int iNoInspectROI = algCtx.iNoInspectROI;
	CString sLotID = algCtx.sLotID;
	CString sVisionCamType_Short = algCtx.sVisionCamType_Short;

	GTRegion* pColorInfoROIRgn;
	int iColorInfpROIID = -1;
	HObject HInspectROIRgn;
	HObject HEdgeImageReduced, HEdgeAmp;
	CString sCamName, sDay, sTime, sColorLog;
	HTuple HdAvgValue, HdSd;
	int iAvgValueI;
	double dEdgeStr;
	iAvgValueI = -100;

	sCamName.Format("%s%d", sVisionCamType_Short, THEAPP.Struct_PreferenceStruct.m_iPCType + 1);

	SYSTEMTIME time;
	GetLocalTime(&time);
	sDay.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
	sTime.Format("%s %02d:%02d:%02d",sDay, time.wHour, time.wMinute, time.wSecond);

	for (int iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
	{
		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			pColorInfoROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pColorInfoROIRgn == NULL)
				continue;

			if (pColorInfoROIRgn->miTeachImageIndex != iImageIdx)
				continue;

			if (pColorInfoROIRgn->miInspectionType != INSPECTION_TYPE_COLOR)
				continue;

			iColorInfpROIID = pColorInfoROIRgn->miColorInfoID;

			if (iColorInfpROIID != -1 && iColorInfpROIID < 100)
			{
				HInspectROIRgn = pColorInfoROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

				if (CGFunction::ValidHRegion(HInspectROIRgn) == FALSE)
					continue;

				Intensity(HInspectROIRgn, m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_I][iImageIdx - 1], &HdAvgValue, &HdSd);
				iAvgValueI = HdAvgValue[0].D();

				ReduceDomain(m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_I][iImageIdx - 1], HInspectROIRgn, &HEdgeImageReduced);
				SobelAmp(HEdgeImageReduced, &HEdgeAmp, "sum_abs", 3);
				Intensity(HInspectROIRgn, HEdgeAmp, &HdAvgValue, &HdSd);
				dEdgeStr = HdAvgValue[0].D();

				sColorLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%d\t%.1lf",
								 sDay, sTime, THEAPP.GetPCID(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, sLotID,
								 THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName,
								 iTrayNo, iModuleNo, THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1],
								 sCamName, iImageIdx, iColorInfpROIID, iAvgValueI, dEdgeStr);

				if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
						THEAPP.SaveMonitorGVEdgeLog(THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1], sColorLog);
					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
						THEAPP.SaveMonitorGVEdgeLog(THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1], sColorLog);
				}
				else
				{
					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_MONITORING_GV_EDGE;
						pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1];
						pSaveLogThreadIDParam->strType1 = sColorLog;
						pSaveLogThreadIDParam->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam->iModuleNo = iModuleNo;
						THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
					}
					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_MONITORING_GV_EDGE;
						pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1];
						pSaveLogThreadIDParam->strType1 = sColorLog;
						pSaveLogThreadIDParam->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam->iModuleNo = iModuleNo;
						THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
					}
				}
			}
		}
	}
}

void AlgorithmPipeline::ProcessResultSave(const AlgorithmContext& algCtx)
{
	DWORD dwModuleTotalEndTime = GetTickCount64();
	DWORD dwTimeMs = dwModuleTotalEndTime - algCtx.dwModuleTotalStartTime;

	CString strLog;
	strLog.Format("%s/ Algorithm Thread, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, iInspectionBufferIdx: %d, iCurThreadIdx: %d",
				  algCtx.sVisionCamType_Short, dwTimeMs, algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo, algCtx.iModuleNo,
				  algCtx.iCurInspectionBufferIdx, algCtx.iCurThreadIdx);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

	m_pAlgorithm->ChangeInspectDone(algCtx.iCurInspectionBufferIdx, BUFFER_STATUS_POSTPROCESSING);

	int iPcVisionNo = m_pAlgorithm->m_iPcVisionNo;
	int iVisionCamType = m_pAlgorithm->m_iVisionCamType;

	if (m_pAlgorithm->IsResultSaveThreadRunning() == TRUE)
		return;

	RESULT_SAVE_THREAD_PARAM* pParam = new RESULT_SAVE_THREAD_PARAM;
	pParam->iPcVisionNo = iPcVisionNo;
	pParam->iVisionCamType = iVisionCamType;

	AfxBeginThread(ResultSaveThread, pParam);
}

////////////////////////////////////////////////////////
// CHS 보관함 - 너무 길어서 따로 보관
////////////////////////////////////////////////////////
#if defined (UAVI_CHS_KS) || defined (UAVI_CHS_TV)
void AlgorithmPipeline::ProcessFAIMeasure(const AlgorithmContext& algCtx, BOOL bInitFAIItem)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	int iTrayNo = algCtx.iTrayNo;
	int iModuleNo = algCtx.iModuleNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	int iImageIdx, iROIIndex, iTabIdx;
	int i, j;
	GTRegion* pInspectROIRgn;
	HObject HROIHRegion, HResultXLD, HPreProcessImage, HMeasureRetRgn, HFAIMeasureRgn, HCrossXLD;
	int iFAINumber;
	HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;

	for (int iFaiParamIndex = 0; iFaiParamIndex < MAX_FAI_PARAMETER; iFaiParamIndex++)
	{
		if (bInitFAIItem)
		{
			GenEmptyObj(&(m_pAlgorithm->m_HDefectRgn_FAI[iBufferIdx]));
			for (i = 0; i < MAX_FAI_ITEM; i++)
			{
				GenEmptyObj(&(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex]));
				for (j = 0; j < MAX_FAI_REVIEW_IMAGE; j++)
				{
					GenEmptyObj(&(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][i][iFaiParamIndex][j]));
					m_pAlgorithm->m_iViewportCenter_FAI_Item[iBufferIdx][i][iFaiParamIndex][j].x = -1;
					m_pAlgorithm->m_iViewportCenter_FAI_Item[iBufferIdx][i][iFaiParamIndex][j].y = -1;
				}
			}
		}

		iFAINumber = 0;

		if (iVisionCamType == VISION_NUMBER_2)
		{
			for (iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
			{
				for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
				{
					pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);
					if (pInspectROIRgn == NULL) continue;
					if (pInspectROIRgn->miTeachImageIndex != iImageIdx) continue;
					if (pInspectROIRgn->miFaiRoiIndex != iFaiParamIndex) continue;
					if (pInspectROIRgn->miInspectionType < INSPECTION_TYPE_DATUM_START || pInspectROIRgn->miInspectionType > INSPECTION_TYPE_DATUM_END) continue;

					for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
								m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
								m_pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
								m_pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
							{
								m_pAlgorithm->GetPreprocessImage(TRUE, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
								HMeasureRetRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
							else
								HMeasureRetRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);

							if (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;

								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 6;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 11;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 12;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 13;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 21;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 23;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 24;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
						}
					}
				}
			}
		}

		for (iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);
				if (pInspectROIRgn == NULL) continue;
				if (pInspectROIRgn->miTeachImageIndex != iImageIdx) continue;
				if (pInspectROIRgn->miFaiRoiIndex != iFaiParamIndex) continue;
				if (pInspectROIRgn->miInspectionType < INSPECTION_TYPE_MEASURE_START || pInspectROIRgn->miInspectionType > INSPECTION_TYPE_MEASURE_END) continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					{
						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
							m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
							m_pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
							m_pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
						{
							m_pAlgorithm->GetPreprocessImage(TRUE, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
							HMeasureRetRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}
						else
							HMeasureRetRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);

						if (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
						{
							if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P8)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 5;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P8)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 6;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 7;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 19;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P2)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 7;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P2)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 9;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
									iFAINumber = 11;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P3)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 9;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][2], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][2]));
									iFAINumber = 11;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][2], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][2]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P4 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P5)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 9;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][3], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][3]));
									iFAINumber = 11;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][3], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][3]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_C)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 9;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 12;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_C)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 10;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 13;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
									iFAINumber = 85;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P3)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 10;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P3)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI18_NeckEpoxyPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI18_NeckEpoxyPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 18;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 21;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
									iFAINumber = 27;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI23_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI23_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 23;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_24_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_24_P2)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_24_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_24_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 24;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_25_P2)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 25;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
									iFAINumber = 85;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_1_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_1_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 86;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_1_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_1_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 86;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_2_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_2_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 87;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_2_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_2_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 87;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_3_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_3_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 88;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_3_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_3_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 88;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_19_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_19_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_19_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_19_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 19;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_19_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_19_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_19_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_19_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 19;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}

							if (iFAINumber > 0)
							{
								GenRegionLine(&HFAIMeasureRgn, pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY, pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX, pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY, pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX);
								ConcatObj(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex], HFAIMeasureRgn, &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex]);
							}
						}
						else
						{
							if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P8)
								iFAINumber = 5;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P8)
								iFAINumber = 6;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P2)
								iFAINumber = 7;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_C)
								iFAINumber = 9;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P3)
								iFAINumber = 10;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P3)
								iFAINumber = 18;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_B)
								iFAINumber = 21;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_B)
								iFAINumber = 23;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_24_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_24_P2)
								iFAINumber = 24;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_25_P2)
								iFAINumber = 25;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P4)
								iFAINumber = 86;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P4)
								iFAINumber = 87;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P4)
								iFAINumber = 88;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_19_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_19_P4)
								iFAINumber = 19;

							if (iFAINumber > 0)
								ConcatObj(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex], HROIHRegion, &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex]);
						}
					}
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Measurement
		double dDistance, dAngle, dMaxDistance, dAvgDistance;
		HTuple HDistance, HAngle;
		DPOINT dPoint1, dPoint2, dPoint3, dPoint4;
		DPOINT dLineStart, dLineEnd;
		DPOINT dLineStart2, dLineEnd2;
		DPOINT dMeasurePoint1, dMeasurePoint2;
		double dDistance1, dDistance2;
		HTuple HTiltP, HTiltPCos;
		double dTiltOffset;

		// Datum
		DPOINT dLinePoint1, dLinePoint2, dLinePoint3, dLinePoint4, dIntersectPoint;

		for (i = 0; i < 8; i += 2)
		{
			m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][i].Reset();

			dLinePoint1 = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[i % 8];
			dLinePoint2 = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[(i + 1) % 8];
			dLinePoint3 = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[(i + 2) % 8];
			dLinePoint4 = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[(i + 3) % 8];

			if (dLinePoint1.x >= 0 && dLinePoint2.x >= 0 && dLinePoint3.x >= 0 && dLinePoint4.x >= 0)
			{
				IntersectionLines(dLinePoint1.y, dLinePoint1.x, dLinePoint2.y, dLinePoint2.x, dLinePoint3.y, dLinePoint3.x, dLinePoint4.y, dLinePoint4.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);
				dIntersectPoint.x = HIntersectPointX[0].D();
				dIntersectPoint.y = HIntersectPointY[0].D();

				m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][i / 2] = dIntersectPoint;
			}
		}

		dLinePoint1 = m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][0];
		dLinePoint2 = m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][2];
		dLinePoint3 = m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][1];
		dLinePoint4 = m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][3];

		if (dLinePoint1.x >= 0 && dLinePoint2.x >= 0 && dLinePoint3.x >= 0 && dLinePoint4.x >= 0)
		{
			IntersectionLines(dLinePoint1.y, dLinePoint1.x, dLinePoint2.y, dLinePoint2.x, dLinePoint3.y, dLinePoint3.x, dLinePoint4.y, dLinePoint4.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);
			dIntersectPoint.x = HIntersectPointX[0].D();
			dIntersectPoint.y = HIntersectPointY[0].D();

			THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint = dIntersectPoint;
		}

		HTuple HInnerEdgeConcatX, HInnerEdgeConcatY;
		HTuple HOuterEdgeConcatX, HOuterEdgeConcatY;
		double dInnerNormalRadius, dOuterNormalRadius;
		int iNoInnerEdge, iNoOuterEdge;
		HTuple HNoCircleFitEdge;
		double dInnerCircleCenterX, dInnerCircleCenterY, dInnerCircleRadius;
		double dOuterCircleCenterX, dOuterCircleCenterY, dOuterCircleRadius;
		BOOL bInnerCircleFound, bOuterCircleFound;
		HObject HCircleXLD;

		bInnerCircleFound = bOuterCircleFound = FALSE;

		// FAI-5
		TupleGenConst(0, 0, &HOuterEdgeConcatX);
		TupleGenConst(0, 0, &HOuterEdgeConcatY);

		for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++)
		{
			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii].y;

			if (dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				TupleConcat(HOuterEdgeConcatX, HTuple(dPoint1.x), &HOuterEdgeConcatX);
				TupleConcat(HOuterEdgeConcatY, HTuple(dPoint1.y), &HOuterEdgeConcatY);
			}
		}

		TupleLength(HOuterEdgeConcatX, &HNoCircleFitEdge);
		iNoOuterEdge = HNoCircleFitEdge.L();

		if (iNoOuterEdge == MAX_FAI_CIRCLE_FIT_POINT)
		{
			dOuterNormalRadius = 0;
			for (int iii = 0; iii < (MAX_FAI_CIRCLE_FIT_POINT / 2); iii++)
			{
				dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii].x;
				dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii].y;

				dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
				dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

				DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance.D();
				dOuterNormalRadius += dDistance;
			}
			dOuterNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
			dOuterNormalRadius *= 0.5;

			bOuterCircleFound = m_pAlgorithm->LeastSquareCircleFitting(HOuterEdgeConcatX, HOuterEdgeConcatY, dOuterNormalRadius, &dOuterCircleCenterX, &dOuterCircleCenterY, &dOuterCircleRadius);
			if (bOuterCircleFound)
			{
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI5_LensBarrelTopEdgePoint.x = dOuterCircleCenterX;
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI5_LensBarrelTopEdgePoint.y = dOuterCircleCenterY;

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[5] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[5] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[5][0] = ((dOuterCircleRadius * 2.0) * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[5];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[5][0] = ((dOuterCircleRadius * 2.0) * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[5];


				GenCircleContourXld(&HCircleXLD, dOuterCircleCenterY, dOuterCircleCenterX, dOuterCircleRadius, 0, 6.28318, "positive", 1);
				GenRegionContourXld(HCircleXLD, &HFAIMeasureRgn, "filled");
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][5][iFaiParamIndex][0], HCircleXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][5][iFaiParamIndex][0]));

				ConcatObj(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][5][iFaiParamIndex], HFAIMeasureRgn, &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][5][iFaiParamIndex]);
			}
		}

		// FAI-6
		TupleGenConst(0, 0, &HInnerEdgeConcatX);
		TupleGenConst(0, 0, &HInnerEdgeConcatY);

		for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++)
		{
			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii].y;

			if (dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				TupleConcat(HInnerEdgeConcatX, HTuple(dPoint1.x), &HInnerEdgeConcatX);
				TupleConcat(HInnerEdgeConcatY, HTuple(dPoint1.y), &HInnerEdgeConcatY);
			}
		}

		TupleLength(HInnerEdgeConcatX, &HNoCircleFitEdge);
		iNoInnerEdge = HNoCircleFitEdge.L();

		if (iNoInnerEdge == MAX_FAI_CIRCLE_FIT_POINT)
		{
			dInnerNormalRadius = 0;
			for (int iii = 0; iii < (MAX_FAI_CIRCLE_FIT_POINT / 2); iii++)
			{
				dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii].x;
				dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii].y;

				dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
				dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

				DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance.D();
				dInnerNormalRadius += dDistance;
			}
			dInnerNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
			dInnerNormalRadius *= 0.5;

			bInnerCircleFound = m_pAlgorithm->LeastSquareCircleFitting(HInnerEdgeConcatX, HInnerEdgeConcatY, dInnerNormalRadius, &dInnerCircleCenterX, &dInnerCircleCenterY, &dInnerCircleRadius);
			if (bInnerCircleFound)
			{
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI6_LensInnerEdgePoint.x = dInnerCircleCenterX;
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI6_LensInnerEdgePoint.y = dInnerCircleCenterY;

				GenCircleContourXld(&HCircleXLD, dInnerCircleCenterY, dInnerCircleCenterX, dInnerCircleRadius, 0, 6.28318, "positive", 1);
				GenRegionContourXld(HCircleXLD, &HFAIMeasureRgn, "filled");
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][6][iFaiParamIndex][1], HCircleXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][6][iFaiParamIndex][1]));

				GenCrossContourXld(&HCrossXLD, dInnerCircleCenterY, dInnerCircleCenterX, 30.0, 0);
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][6][iFaiParamIndex][1], HCrossXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][6][iFaiParamIndex][1]));

				ConcatObj(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][6][iFaiParamIndex], HFAIMeasureRgn, &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][6][iFaiParamIndex]);
			}
		}

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI6_LensInnerEdgePoint.x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI6_LensInnerEdgePoint.y;

		if (dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint2.x >= 0 && dPoint2.y >= 0)
		{
			DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
			dDistance = HDistance[0].D();
			dDistance *= 2.0;

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[6] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[6] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[6][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[6];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[6][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[6];
		}

		// FAI-7
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[1].y;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_EastPoint[0].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_EastPoint[0].y;

		dDistance1 = -1;
		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance1 = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[7] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[7] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[7][1] = (dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[7];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[7][1] = (dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[7];
		}

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_EastPoint[1].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_EastPoint[1].y;

		dDistance2 = -1;
		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance2 = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[7] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[7] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[7][2] = (dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[7];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[7][2] = (dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[7];
		}

		dMaxDistance = -1;
		if (dDistance1 >= 0 && dDistance2 >= 0)
		{
			if (dDistance1 >= dDistance2)
				dMaxDistance = dDistance1;
			else
				dMaxDistance = dDistance2;
		}
		else if (dDistance1 >= 0 && dDistance2 < 0)
			dMaxDistance = dDistance1;
		else if (dDistance2 >= 0 && dDistance1 < 0)
			dMaxDistance = dDistance2;

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[7] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[7] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[7][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[7];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[7][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[7];
		}

		// FAI-9
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[2].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[2].y;

		for (i = 0; i < 5; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[9] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[9] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[9][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[9];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[9][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[9];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 5; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[9] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[9] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[9][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[9];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[9][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[9];
		}

		// FAI-10
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[2].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[2].y;

		for (i = 0; i < 3; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[10] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[10] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[10][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[10];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[10][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[10];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 3; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[10] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[10] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[10][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[10];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[10][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[10];
		}

		// FAI-11
		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		for (i = 0; i < 5; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[i].y;

			if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = fabs(dPoint2.x - dPoint1.x);

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[11] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[11] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[11][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[11];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[11][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[11];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 5; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[11] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[11] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[11][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[11];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[11][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[11];
		}

		// FAI-12
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[2].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[2].y;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[12] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[12] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[12][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[12];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[12][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[12];
		}

		// FAI-13
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[2].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[2].y;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[13] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[13] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[13][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[13];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[13][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[13];
		}

		// FAI-18
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[1].y;

		for (i = 0; i < 3; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI18_NeckEpoxyPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI18_NeckEpoxyPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[18] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[18] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[18][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[18];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[18][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[18];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 3; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[18] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[18] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[18][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[18];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[18][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[18];
		}

		// FAI-21
		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[0].x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[0].y;

		dPoint3.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[1].x;
		dPoint3.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[1].y;

		if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint3.x >= 0 && dPoint3.y >= 0)
		{
			dDistance = fabs((dPoint2.x + dPoint3.x) * 0.5 - dPoint1.x);

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[21] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[21] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[21][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[21];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[21][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[21];
		}

		// FAI-23
		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI23_FlexPoint[0].x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI23_FlexPoint[0].y;

		dPoint3.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI23_FlexPoint[1].x;
		dPoint3.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI23_FlexPoint[1].y;

		if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint3.x >= 0 && dPoint3.y >= 0)
		{
			dDistance = fabs((dPoint2.y + dPoint3.y) * 0.5 - dPoint1.y);

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[23] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[23] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[23][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[23];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[23][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[23];
		}

		// FAI-24
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[1].y;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[24] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[24] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[24][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[24];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[24][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[24];
		}

		// FAI-25
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_NorthPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_NorthPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_NorthPoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_NorthPoint[1].y;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_SouthPoint[0].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_SouthPoint[0].y;

		dDistance1 = -1;
		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance1 = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[25] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[25] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[25][1] = (dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[25];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[25][1] = (dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[25];
		}

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_SouthPoint[1].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_SouthPoint[1].y;

		dDistance2 = -1;
		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance2 = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[25] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[25] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[25][2] = (dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[25];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[25][2] = (dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[25];
		}

		dAvgDistance = -1;
		if (dDistance1 >= 0 && dDistance2 >= 0)
			dAvgDistance = (dDistance1 + dDistance2) * 0.5;
		else if (dDistance1 >= 0 && dDistance2 < 0)
			dAvgDistance = dDistance1;
		else if (dDistance2 >= 0 && dDistance1 < 0)
			dAvgDistance = dDistance2;

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[25] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[25] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[25][0] = (dAvgDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[25];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[25][0] = (dAvgDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[25];
		}

		// D/C_2

		HObject HLineContour1, HLineContour2;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_NorthPoint[0].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_NorthPoint[0].y;
		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_NorthPoint[1].x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_NorthPoint[1].y;
		dPoint3.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_SouthPoint[0].x;
		dPoint3.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_SouthPoint[0].y;
		dPoint4.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_SouthPoint[1].x;
		dPoint4.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_SouthPoint[1].y;

		if (dPoint1.x >= 0 && dPoint2.x >= 0 && dPoint3.x >= 0 && dPoint4.x >= 0)
		{
			DPOINT dM1Point, dM2Point;

			dM1Point.x = (dPoint1.x + dPoint3.x) * 0.5;
			dM1Point.y = (dPoint1.y + dPoint3.y) * 0.5;
			dM2Point.x = (dPoint2.x + dPoint4.x) * 0.5;
			dM2Point.y = (dPoint2.y + dPoint4.y) * 0.5;

			dLineStart.x = dM1Point.x;
			dLineStart.y = dM1Point.y;
			dLineEnd.x = dM2Point.x;
			dLineEnd.y = dM2Point.y;

			dLineStart2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[0].x;
			dLineStart2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[0].y;
			dLineEnd2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[2].x;
			dLineEnd2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[2].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineStart2.y >= 0)
			{
				AngleLl(dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HAngle);

				TupleDeg(HAngle, &HAngle);
				dAngle = HAngle[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[85][0] = dAngle + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[85];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[85][0] = dAngle + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[85];

				GenContourPolygonXld(&HLineContour1, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][85][iFaiParamIndex][0], HLineContour1, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][85][iFaiParamIndex][0]));
				GenContourPolygonXld(&HLineContour2, HTuple(dLineStart2.y).TupleConcat(HTuple(dLineEnd2.y)), HTuple(dLineStart2.x).TupleConcat(HTuple(dLineEnd2.x)));
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][85][iFaiParamIndex][1], HLineContour2, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][85][iFaiParamIndex][1]));
			}
		}

		// FAI-27
		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[0].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[0].y;
		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[1].x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_WestPoint[1].y;
		dPoint3.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_EastPoint[0].x;
		dPoint3.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_EastPoint[0].y;
		dPoint4.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_EastPoint[1].x;
		dPoint4.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI7_EastPoint[1].y;

		if (dPoint1.x >= 0 && dPoint2.x >= 0 && dPoint3.x >= 0 && dPoint4.x >= 0)
		{
			DPOINT dM1Point, dM2Point;

			dM1Point.x = (dPoint1.x + dPoint3.x) * 0.5;
			dM1Point.y = (dPoint1.y + dPoint3.y) * 0.5;
			dM2Point.x = (dPoint2.x + dPoint4.x) * 0.5;
			dM2Point.y = (dPoint2.y + dPoint4.y) * 0.5;

			dLineStart.x = dM1Point.x;
			dLineStart.y = dM1Point.y;
			dLineEnd.x = dM2Point.x;
			dLineEnd.y = dM2Point.y;

			dLineStart2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[0].x;
			dLineStart2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[0].y;
			dLineEnd2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[1].x;
			dLineEnd2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[1].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineStart2.y >= 0)
			{
				AngleLl(dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HAngle);

				TupleDeg(HAngle, &HAngle);
				dAngle = HAngle[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[27][0] = dAngle + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[27];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[27][0] = dAngle + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[27];

				GenContourPolygonXld(&HLineContour1, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][27][iFaiParamIndex][0], HLineContour1, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][27][iFaiParamIndex][0]));
				GenContourPolygonXld(&HLineContour2, HTuple(dLineStart2.y).TupleConcat(HTuple(dLineEnd2.y)), HTuple(dLineStart2.x).TupleConcat(HTuple(dLineEnd2.x)));
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][27][iFaiParamIndex][1], HLineContour2, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][27][iFaiParamIndex][1]));
			}
		}

		// FAI-16-1
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_1_LinePoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_1_LinePoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_1_LinePoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_1_LinePoint[1].y;

		for (i = 0; i < 4; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_1_TopPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_1_TopPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[86] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[86] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[86][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[86];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[86][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[86];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 4; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[86] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[86] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[86][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[86];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[86][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[86];
		}

		// FAI-16-2
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_2_LinePoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_2_LinePoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_2_LinePoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_2_LinePoint[1].y;

		for (i = 0; i < 4; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_2_TopPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_2_TopPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[87] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[87] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[87][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[87];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[87][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[87];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 4; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[87] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[87] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[87][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[87];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[87][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[87];
		}

		// FAI-16-3
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_3_LinePoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_3_LinePoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_3_LinePoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_3_LinePoint[1].y;

		for (i = 0; i < 4; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_3_TopPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI16_3_TopPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[88] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[88] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[88][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[88];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[88][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[88];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 4; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[88] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[88] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[88][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[88];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[88][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[88];
		}

		// FAI-19
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[1].y;

		for (i = 0; i < 4; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_TopPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_TopPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[19] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[19] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[19][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[19];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[19][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[19];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 4; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[19] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[19] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[19][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[19];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[19][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[19];
		}

		for (i = 0; i < MAX_FAI_ITEM; i++)
		{
			if (CGFunction::ValidHRegion(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex]))
				Union1(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex], &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex]);
		}
	}
}
#endif

#if defined (UAVI_CHS_WZ)
void AlgorithmPipeline::ProcessFAIMeasure(const AlgorithmContext& algCtx, BOOL bInitFAIItem)
{
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	int iDualModelData = algCtx.iDualModelData;
	int iPcVisionNo = algCtx.iPcVisionNo;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	int iTrayNo = algCtx.iTrayNo;
	int iModuleNo = algCtx.iModuleNo;
	int iNoInspectROI = algCtx.iNoInspectROI;

	int iImageIdx, iROIIndex, iTabIdx;
	int i, j;
	GTRegion* pInspectROIRgn;
	HObject HROIHRegion, HResultXLD, HPreProcessImage, HMeasureRetRgn, HFAIMeasureRgn, HCrossXLD;
	int iFAINumber;
	HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;

	for (int iFaiParamIndex = 0; iFaiParamIndex < MAX_FAI_PARAMETER; iFaiParamIndex++)
	{
		if (bInitFAIItem)
		{
			GenEmptyObj(&(m_pAlgorithm->m_HDefectRgn_FAI[iBufferIdx]));
			for (i = 0; i < MAX_FAI_ITEM; i++)
			{
				GenEmptyObj(&(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex]));
				for (j = 0; j < MAX_FAI_REVIEW_IMAGE; j++)
				{
					GenEmptyObj(&(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][i][iFaiParamIndex][j]));
					m_pAlgorithm->m_iViewportCenter_FAI_Item[iBufferIdx][i][iFaiParamIndex][j].x = -1;
					m_pAlgorithm->m_iViewportCenter_FAI_Item[iBufferIdx][i][iFaiParamIndex][j].y = -1;
				}
			}
		}

		iFAINumber = 0;

		if (iVisionCamType == VISION_NUMBER_2)
		{
			for (iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
			{
				for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
				{
					pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);
					if (pInspectROIRgn == NULL) continue;
					if (pInspectROIRgn->miTeachImageIndex != iImageIdx) continue;
					if (pInspectROIRgn->miFaiRoiIndex != iFaiParamIndex) continue;
					if (pInspectROIRgn->miInspectionType < INSPECTION_TYPE_DATUM_START || pInspectROIRgn->miInspectionType > INSPECTION_TYPE_DATUM_END) continue;

					for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
								m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
								m_pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
								m_pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
							{
								m_pAlgorithm->GetPreprocessImage(TRUE, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
								HMeasureRetRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
							else
								HMeasureRetRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);

							if (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;

								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 6;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 11;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 12;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 13;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 21;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 22;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 23;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 24;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 25;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
						}
					}
				}
			}
		}

		for (iImageIdx = 1 + algCtx.iStartImageIdx; iImageIdx <= algCtx.iEndImageIdx; iImageIdx++)
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);
				if (pInspectROIRgn == NULL) continue;
				if (pInspectROIRgn->miTeachImageIndex != iImageIdx) continue;
				if (pInspectROIRgn->miFaiRoiIndex != iFaiParamIndex) continue;
				if (pInspectROIRgn->miInspectionType < INSPECTION_TYPE_MEASURE_START || pInspectROIRgn->miInspectionType > INSPECTION_TYPE_MEASURE_END) continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					{
						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
							m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
							m_pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
							m_pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
						{
							m_pAlgorithm->GetPreprocessImage(TRUE, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
							HMeasureRetRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}
						else
							HMeasureRetRgn = m_pAlgorithm->InspectAVI(iBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], m_pAlgorithm->m_HInspectCImage[iBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);

						if (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
						{
							if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P8)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 5;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P8)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 6;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P2)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 3;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 85;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P3)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 9;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 11;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_C)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 9;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 12;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_C)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 10;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 13;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P2)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 10;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
									iFAINumber = 23;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P2)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 14;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P3 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P3].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P3].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 14;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
									iFAINumber = 18;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_C)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI18_NeckEpoxyPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI18_NeckEpoxyPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 18;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P2)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 21;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P2)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI22_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI22_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 22;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
									iFAINumber = 85;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 24;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD))
								{
									iFAINumber = 25;
									ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1]));
								}
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_1_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_1_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD)) { iFAINumber = 86; ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0])); }
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_1_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_1_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD)) { iFAINumber = 86; ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1])); }
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_2_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_2_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD)) { iFAINumber = 87; ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0])); }
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_2_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_2_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD)) { iFAINumber = 87; ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1])); }
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_3_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_3_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD)) { iFAINumber = 88; ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0])); }
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_3_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_3_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD)) { iFAINumber = 88; ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1])); }
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_B)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_A].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_A].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD)) { iFAINumber = 19; ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][0])); }
							}
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P4)
							{
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P1].x = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P1].y = (pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								if (CGFunction::ValidHXLD(HResultXLD)) { iFAINumber = 19; ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1], HResultXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex][1])); }
							}

							if (iFAINumber > 0)
							{
								GenRegionLine(&HFAIMeasureRgn, pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartY, pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineStartX, pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndY, pInspectROIRgn->m_MeasureData[iBufferIdx][iTabIdx].m_dEdgeLineEndX);
								ConcatObj(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex], HFAIMeasureRgn, &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex]);
							}
						}
						else
						{
							if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P8)
								iFAINumber = 5;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P8)
								iFAINumber = 6;
							if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P2)
								iFAINumber = 3;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_C)
								iFAINumber = 9;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P2)
								iFAINumber = 10;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P4)
								iFAINumber = 14;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_C)
								iFAINumber = 18;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P2)
								iFAINumber = 21;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P2)
								iFAINumber = 22;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P4)
								iFAINumber = 24;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P4)
								iFAINumber = 25;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P4)
								iFAINumber = 86;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P4)
								iFAINumber = 87;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P4)
								iFAINumber = 88;
							else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P4)
								iFAINumber = 19;

							if (iFAINumber > 0)
								ConcatObj(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex], HROIHRegion, &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][iFAINumber][iFaiParamIndex]);
						}
					}
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Measurement
		double dDistance, dAngle, dMaxDistance, dAvgDistance;
		HTuple HDistance, HAngle;
		DPOINT dPoint1, dPoint2, dPoint3, dPoint4;
		DPOINT dLineStart, dLineEnd;
		DPOINT dLineStart2, dLineEnd2;
		DPOINT dMeasurePoint1, dMeasurePoint2;
		double dDistance1, dDistance2;
		HTuple HTiltP, HTiltPCos;
		double dTiltOffset;

		// Datum
		DPOINT dLinePoint1, dLinePoint2, dLinePoint3, dLinePoint4, dIntersectPoint;

		for (i = 0; i < 8; i += 2)
		{
			m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][i].Reset();

			dLinePoint1 = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[i % 8];
			dLinePoint2 = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[(i + 1) % 8];
			dLinePoint3 = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[(i + 2) % 8];
			dLinePoint4 = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBLinePoint[(i + 3) % 8];

			if (dLinePoint1.x >= 0 && dLinePoint2.x >= 0 && dLinePoint3.x >= 0 && dLinePoint4.x >= 0)
			{
				IntersectionLines(dLinePoint1.y, dLinePoint1.x, dLinePoint2.y, dLinePoint2.x, dLinePoint3.y, dLinePoint3.x, dLinePoint4.y, dLinePoint4.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);
				dIntersectPoint.x = HIntersectPointX[0].D();
				dIntersectPoint.y = HIntersectPointY[0].D();

				m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][i / 2] = dIntersectPoint;
			}
		}

		dLinePoint1 = m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][0];
		dLinePoint2 = m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][2];
		dLinePoint3 = m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][1];
		dLinePoint4 = m_pAlgorithm->m_dDatumBCornerPoint[iBufferIdx][3];

		if (dLinePoint1.x >= 0 && dLinePoint2.x >= 0 && dLinePoint3.x >= 0 && dLinePoint4.x >= 0)
		{
			IntersectionLines(dLinePoint1.y, dLinePoint1.x, dLinePoint2.y, dLinePoint2.x, dLinePoint3.y, dLinePoint3.x, dLinePoint4.y, dLinePoint4.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);
			dIntersectPoint.x = HIntersectPointX[0].D();
			dIntersectPoint.y = HIntersectPointY[0].D();

			THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint = dIntersectPoint;
		}

		HTuple HInnerEdgeConcatX, HInnerEdgeConcatY;
		HTuple HOuterEdgeConcatX, HOuterEdgeConcatY;
		double dInnerNormalRadius, dOuterNormalRadius;
		int iNoInnerEdge, iNoOuterEdge;
		HTuple HNoCircleFitEdge;
		double dInnerCircleCenterX, dInnerCircleCenterY, dInnerCircleRadius;
		double dOuterCircleCenterX, dOuterCircleCenterY, dOuterCircleRadius;
		BOOL bInnerCircleFound, bOuterCircleFound;
		HObject HCircleXLD;

		bInnerCircleFound = bOuterCircleFound = FALSE;

		// FAI-3
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_WestPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_WestPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_WestPoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_WestPoint[1].y;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_EastPoint[0].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_EastPoint[0].y;

		dDistance1 = -1;
		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance1 = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[3] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[3] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[3][1] = (dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[3];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[3][1] = (dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[3];
		}

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_EastPoint[1].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_EastPoint[1].y;

		dDistance2 = -1;
		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance2 = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[3] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[3] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[3][2] = (dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[3];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[3][2] = (dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[3];
		}

		dAvgDistance = -1;
		if (dDistance1 >= 0 && dDistance2 >= 0)
			dAvgDistance = (dDistance1 + dDistance2) * 0.5;
		else if (dDistance1 >= 0 && dDistance2 < 0)
			dAvgDistance = dDistance1;
		else if (dDistance2 >= 0 && dDistance1 < 0)
			dAvgDistance = dDistance2;

		if (dAvgDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[3] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[3] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[3][0] = (dAvgDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[3];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[3][0] = (dAvgDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[3];
		}

		// FAI-5
		TupleGenConst(0, 0, &HOuterEdgeConcatX);
		TupleGenConst(0, 0, &HOuterEdgeConcatY);

		for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++)
		{
			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii].y;

			if (dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				TupleConcat(HOuterEdgeConcatX, HTuple(dPoint1.x), &HOuterEdgeConcatX);
				TupleConcat(HOuterEdgeConcatY, HTuple(dPoint1.y), &HOuterEdgeConcatY);
			}
		}

		TupleLength(HOuterEdgeConcatX, &HNoCircleFitEdge);
		iNoOuterEdge = HNoCircleFitEdge.L();

		if (iNoOuterEdge == MAX_FAI_CIRCLE_FIT_POINT)
		{
			dOuterNormalRadius = 0;
			for (int iii = 0; iii < (MAX_FAI_CIRCLE_FIT_POINT / 2); iii++)
			{
				dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii].x;
				dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii].y;

				dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
				dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI5_LensBarrelTopEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

				DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance.D();
				dOuterNormalRadius += dDistance;
			}
			dOuterNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
			dOuterNormalRadius *= 0.5;

			bOuterCircleFound = m_pAlgorithm->LeastSquareCircleFitting(HOuterEdgeConcatX, HOuterEdgeConcatY, dOuterNormalRadius, &dOuterCircleCenterX, &dOuterCircleCenterY, &dOuterCircleRadius);
			if (bOuterCircleFound)
			{
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI5_LensBarrelTopEdgePoint.x = dOuterCircleCenterX;
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI5_LensBarrelTopEdgePoint.y = dOuterCircleCenterY;

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[5] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[5] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[5][0] = ((dOuterCircleRadius * 2.0) * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[5];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[5][0] = ((dOuterCircleRadius * 2.0) * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[5];


				GenCircleContourXld(&HCircleXLD, dOuterCircleCenterY, dOuterCircleCenterX, dOuterCircleRadius, 0, 6.28318, "positive", 1);
				GenRegionContourXld(HCircleXLD, &HFAIMeasureRgn, "filled");
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][5][iFaiParamIndex][0], HCircleXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][5][iFaiParamIndex][0]));

				ConcatObj(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][5][iFaiParamIndex], HFAIMeasureRgn, &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][5][iFaiParamIndex]);
			}
		}

		// FAI-6
		TupleGenConst(0, 0, &HInnerEdgeConcatX);
		TupleGenConst(0, 0, &HInnerEdgeConcatY);

		for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++)
		{
			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii].y;

			if (dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				TupleConcat(HInnerEdgeConcatX, HTuple(dPoint1.x), &HInnerEdgeConcatX);
				TupleConcat(HInnerEdgeConcatY, HTuple(dPoint1.y), &HInnerEdgeConcatY);
			}
		}

		TupleLength(HInnerEdgeConcatX, &HNoCircleFitEdge);
		iNoInnerEdge = HNoCircleFitEdge.L();

		if (iNoInnerEdge == MAX_FAI_CIRCLE_FIT_POINT)
		{
			dInnerNormalRadius = 0;
			for (int iii = 0; iii < (MAX_FAI_CIRCLE_FIT_POINT / 2); iii++)
			{
				dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii].x;
				dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii].y;

				dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
				dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI6_LensInnerEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

				DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance.D();
				dInnerNormalRadius += dDistance;
			}
			dInnerNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
			dInnerNormalRadius *= 0.5;

			bInnerCircleFound = m_pAlgorithm->LeastSquareCircleFitting(HInnerEdgeConcatX, HInnerEdgeConcatY, dInnerNormalRadius, &dInnerCircleCenterX, &dInnerCircleCenterY, &dInnerCircleRadius);
			if (bInnerCircleFound)
			{
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI6_LensInnerEdgePoint.x = dInnerCircleCenterX;
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI6_LensInnerEdgePoint.y = dInnerCircleCenterY;

				GenCircleContourXld(&HCircleXLD, dInnerCircleCenterY, dInnerCircleCenterX, dInnerCircleRadius, 0, 6.28318, "positive", 1);
				GenRegionContourXld(HCircleXLD, &HFAIMeasureRgn, "filled");
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][6][iFaiParamIndex][1], HCircleXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][6][iFaiParamIndex][1]));

				GenCrossContourXld(&HCrossXLD, dInnerCircleCenterY, dInnerCircleCenterX, 30.0, 0);
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][6][iFaiParamIndex][1], HCrossXLD, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][6][iFaiParamIndex][1]));

				ConcatObj(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][6][iFaiParamIndex], HFAIMeasureRgn, &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][6][iFaiParamIndex]);
			}
		}

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI6_LensInnerEdgePoint.x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dMeasureFAI6_LensInnerEdgePoint.y;

		if (dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint2.x >= 0 && dPoint2.y >= 0)
		{
			DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
			dDistance = HDistance[0].D();
			dDistance *= 2.0;

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[6] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[6] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[6][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[6];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[6][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[6];
		}

		// FAI-9
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[2].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[2].y;

		for (i = 0; i < 3; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[9] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[9] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[9][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[9];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[9][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[9];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 3; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[9] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[9] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[9][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[9];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[9][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[9];
		}

		// FAI-10
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[2].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[2].y;

		for (i = 0; i < 2; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[10] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[10] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[10][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[10];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[10][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[10];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 2; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[10] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[10] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[10][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[10];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[10][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[10];
		}

		// FAI-11
		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		for (i = 0; i < 3; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_WestPoint[i].y;

			if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = fabs(dPoint2.x - dPoint1.x);

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[11] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[11] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[11][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[11];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[11][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[11];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 3; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[11] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[11] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[11][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[11];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[11][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[11];
		}

		// FAI-12
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[2].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI9_EastPoint[2].y;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[12] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[12] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[12][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[12];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[12][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[12];
		}

		// FAI-13
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[2].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_NorthPoint[2].y;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[13] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[13] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[13][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[13];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[13][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[13];
		}

		// FAI-14
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_NorthPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_NorthPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_NorthPoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_NorthPoint[1].y;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_SouthPoint[0].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_SouthPoint[0].y;

		dDistance1 = -1;
		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance1 = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[14] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[14] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[14][1] = (dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[14];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[14][1] = (dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[14];
		}

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_SouthPoint[1].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_SouthPoint[1].y;

		dDistance2 = -1;
		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance2 = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[14] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[14] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[14][2] = (dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[14];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[14][2] = (dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[14];
		}

		dAvgDistance = -1;
		if (dDistance1 >= 0 && dDistance2 >= 0)
			dAvgDistance = (dDistance1 + dDistance2) * 0.5;
		else if (dDistance1 >= 0 && dDistance2 < 0)
			dAvgDistance = dDistance1;
		else if (dDistance2 >= 0 && dDistance1 < 0)
			dAvgDistance = dDistance2;

		if (dAvgDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[14] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[14] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[14][0] = (dAvgDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[14];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[14][0] = (dAvgDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[14];
		}

		// FAI-18
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_SouthPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_SouthPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_SouthPoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI14_SouthPoint[1].y;

		for (i = 0; i < 3; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI18_NeckEpoxyPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI18_NeckEpoxyPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[18] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[18] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[18][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[18];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[18][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[18];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 3; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[18] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[18] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[18][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[18];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[18][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[18];
		}

		// FAI-21
		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[0].x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[0].y;

		dPoint3.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[1].x;
		dPoint3.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI21_FlexPoint[1].y;

		if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint3.x >= 0 && dPoint3.y >= 0)
		{
			dDistance = fabs((dPoint2.y + dPoint3.y) * 0.5 - dPoint1.y);

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[21] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[21] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[21][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[21];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[21][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[21];
		}

		// FAI-22
		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI22_FlexPoint[0].x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI22_FlexPoint[0].y;

		dPoint3.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI22_FlexPoint[1].x;
		dPoint3.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI22_FlexPoint[1].y;

		if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint3.x >= 0 && dPoint3.y >= 0)
		{
			dDistance = fabs((dPoint2.x + dPoint3.x) * 0.5 - dPoint1.x);

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[22] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[22] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[22][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[22];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[22][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[22];
		}

		// FAI-23
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI10_SouthPoint[1].y;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

		if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
		{
			DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
			dDistance = HDistance[0].D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[23] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[23] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[23][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[23];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[23][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[23];
		}

		// FAI-24
		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[0].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[0].y;
		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[1].x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[1].y;
		dPoint3.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[3].x;
		dPoint3.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[3].y;
		dPoint4.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[4].x;
		dPoint4.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI24_Point[4].y;

		if (dPoint1.x >= 0 && dPoint2.x >= 0 && dPoint3.x >= 0 && dPoint4.x >= 0)
		{
			DPOINT dM1Point, dM2Point;
			dM1Point.x = (dPoint1.x + dPoint2.x) * 0.5;
			dM1Point.y = (dPoint1.y + dPoint2.y) * 0.5;
			dM2Point.x = (dPoint3.x + dPoint4.x) * 0.5;
			dM2Point.y = (dPoint3.y + dPoint4.y) * 0.5;

			dLineStart.x = dM1Point.x;
			dLineStart.y = dM1Point.y;
			dLineEnd.x = dM2Point.x;
			dLineEnd.y = dM2Point.y;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[24] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[24] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[24][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[24];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[24][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[24];
			}
		}

		// FAI-25
		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_Point[0].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_Point[0].y;
		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_Point[1].x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_Point[1].y;
		dPoint3.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_Point[3].x;
		dPoint3.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_Point[3].y;
		dPoint4.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_Point[4].x;
		dPoint4.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI25_Point[4].y;

		if (dPoint1.x >= 0 && dPoint2.x >= 0 && dPoint3.x >= 0 && dPoint4.x >= 0)
		{
			DPOINT dM1Point, dM2Point;
			dM1Point.x = (dPoint1.x + dPoint2.x) * 0.5;
			dM1Point.y = (dPoint1.y + dPoint2.y) * 0.5;
			dM2Point.x = (dPoint3.x + dPoint4.x) * 0.5;
			dM2Point.y = (dPoint3.y + dPoint4.y) * 0.5;

			dLineStart.x = dM1Point.x;
			dLineStart.y = dM1Point.y;
			dLineEnd.x = dM2Point.x;
			dLineEnd.y = dM2Point.y;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dDatumBPoint.y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[25] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[25] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[25][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[25];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[25][0] = (dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[25];
			}
		}

		// D/C_2
		HObject HLineContour1, HLineContour2;

		dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_WestPoint[0].x;
		dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_WestPoint[0].y;
		dPoint2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_WestPoint[1].x;
		dPoint2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_WestPoint[1].y;
		dPoint3.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_EastPoint[0].x;
		dPoint3.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_EastPoint[0].y;
		dPoint4.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_EastPoint[1].x;
		dPoint4.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI3_EastPoint[1].y;

		if (dPoint1.x >= 0 && dPoint2.x >= 0 && dPoint3.x >= 0 && dPoint4.x >= 0)
		{
			DPOINT dM1Point, dM2Point;

			dM1Point.x = (dPoint1.x + dPoint3.x) * 0.5;
			dM1Point.y = (dPoint1.y + dPoint3.y) * 0.5;
			dM2Point.x = (dPoint2.x + dPoint4.x) * 0.5;
			dM2Point.y = (dPoint2.y + dPoint4.y) * 0.5;

			dLineStart.x = dM1Point.x;
			dLineStart.y = dM1Point.y;
			dLineEnd.x = dM2Point.x;
			dLineEnd.y = dM2Point.y;

			dLineStart2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI22_FlexPoint[0].x;
			dLineStart2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI22_FlexPoint[0].y;
			dLineEnd2.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI22_FlexPoint[1].x;
			dLineEnd2.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI22_FlexPoint[1].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineStart2.y >= 0)
			{
				AngleLl(dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HAngle);

				TupleDeg(HAngle, &HAngle);
				dAngle = HAngle[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[85][0] = dAngle + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[85];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[85][0] = dAngle + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[85];

				GenContourPolygonXld(&HLineContour1, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][27][iFaiParamIndex][0], HLineContour1, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][85][iFaiParamIndex][0]));
				GenContourPolygonXld(&HLineContour2, HTuple(dLineStart2.y).TupleConcat(HTuple(dLineEnd2.y)), HTuple(dLineStart2.x).TupleConcat(HTuple(dLineEnd2.x)));
				ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][27][iFaiParamIndex][1], HLineContour2, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][85][iFaiParamIndex][1]));
			}
		}

		// FAI-17-1
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_1_LinePoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_1_LinePoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_1_LinePoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_1_LinePoint[1].y;

		for (i = 0; i < 4; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_1_TopPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_1_TopPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[86] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[86] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[86][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[86];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[86][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[86];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 4; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[86] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[86] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[86][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[86];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[86][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[86];
		}

		// FAI-17-2
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_2_LinePoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_2_LinePoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_2_LinePoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_2_LinePoint[1].y;

		for (i = 0; i < 4; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_2_TopPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_2_TopPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[87] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[87] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[87][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[87];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[87][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[87];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 4; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[87] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[87] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[87][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[87];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[87][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[87];
		}

		// FAI-17-3
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_3_LinePoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_3_LinePoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_3_LinePoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_3_LinePoint[1].y;

		for (i = 0; i < 4; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_3_TopPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI17_3_TopPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[88] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[88] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[88][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[88];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[88][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[88];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 4; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[88] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[88] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[88][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[88];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[88][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[88];
		}

		// FAI-19
		dLineStart.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[0].x;
		dLineStart.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[0].y;
		dLineEnd.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[1].x;
		dLineEnd.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_LinePoint[1].y;

		for (i = 0; i < 4; i++)
		{
			m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = -1;

			dPoint1.x = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_TopPoint[i].x;
			dPoint1.y = THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAI19_TopPoint[i].y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[19] * (3.1415926 / 180.);
				else
					HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[19] * (3.1415926 / 180.);
				HTiltPCos = HTiltP.TupleCos();
				dTiltOffset = HTiltPCos.D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[19][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[19];
				else
					THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[19][1 + i] = (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[19];
			}
		}

		dMaxDistance = -1;
		for (i = 0; i < 4; i++)
		{
			if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] >= 0)
			{
				if (m_pAlgorithm->m_dDistanceArray[iBufferIdx][i] > dMaxDistance)
					dMaxDistance = m_pAlgorithm->m_dDistanceArray[iBufferIdx][i];
			}
		}

		if (dMaxDistance >= 0)
		{
			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg1Fai[19] * (3.1415926 / 180.);
			else
				HTiltP = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMTiltStg2Fai[19] * (3.1415926 / 180.);
			HTiltPCos = HTiltP.TupleCos();
			dTiltOffset = HTiltPCos.D();

			if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[19][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[19];
			else
				THEAPP.m_StructFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFaiParamIndex].m_dFAIMeasureValue[19][0] = (dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001) * dTiltOffset + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[19];
		}

		for (i = 0; i < MAX_FAI_ITEM; i++)
		{
			if (CGFunction::ValidHRegion(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex]))
				Union1(m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex], &m_pAlgorithm->m_HMeasureRgn_FAI_Item[iBufferIdx][i][iFaiParamIndex]);
		}
	}
}
#endif

