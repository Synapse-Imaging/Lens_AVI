#include "StdAfx.h"
#include "AlgorithmHelper.h"

#include "../Algorithm.h"
#include "../BatchService/BatchManager.h"
#include "../S_Universal_AVI.h"
#include "../uScan.h"
#include <AIService/UniversalAIData.h>
#include <LGIT/types.h>
#include <utility>

using namespace AIService::UniversalAIData;

AlgorithmHelper::AlgorithmHelper(Algorithm* pAlgorithm)
	: m_pAlgorithm(pAlgorithm)
{
}

AlgorithmContext AlgorithmHelper::BuildContext(int iCurThreadIdx, int iCurVisionCamIdx, int iCurInspectionBufferIdx)
{
	AlgorithmContext algCtx;
	algCtx.iCurThreadIdx = iCurThreadIdx;
	algCtx.iCurVisionCamIdx = iCurVisionCamIdx;
	algCtx.iCurInspectionBufferIdx = iCurInspectionBufferIdx;

	algCtx.iStartImageIdx = 0;
	algCtx.iEndImageIdx = m_pAlgorithm->m_iNoInspectImage;

	algCtx.iPcVisionNo = m_pAlgorithm->m_iPcVisionNo;
	algCtx.iVisionCamType = m_pAlgorithm->m_iVisionCamType;

	algCtx.bGrabFail = m_pAlgorithm->m_bGrabFail[iCurInspectionBufferIdx];
	algCtx.sLotID = m_pAlgorithm->m_sLotID[iCurInspectionBufferIdx];
	algCtx.iMzNo = m_pAlgorithm->m_nMzNo[iCurInspectionBufferIdx];
	algCtx.iTrayNo = m_pAlgorithm->m_nTrayNo[iCurInspectionBufferIdx];
	algCtx.iModuleNo = m_pAlgorithm->m_nModuleNo[iCurInspectionBufferIdx];
	algCtx.iStageNo = m_pAlgorithm->m_nStageNo[iCurInspectionBufferIdx];
	algCtx.iJigNo = m_pAlgorithm->m_nJigNo[iCurInspectionBufferIdx];
	algCtx.iDualModelData = THEAPP.g_iDualModelData[algCtx.iMzNo - 1];

	algCtx.sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[algCtx.iPcVisionNo];
	algCtx.sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[algCtx.iPcVisionNo][algCtx.iStageNo];

	algCtx.iNoInspectROI = THEAPP.m_pDualModelDataManager[algCtx.iDualModelData][algCtx.iPcVisionNo]
		->m_pInspectionArea->GetChildTRegionCount();

	algCtx.dwModuleTotalStartTime = GetTickCount64();

	return algCtx;
}

void AlgorithmHelper::BuildROIContext(ROIAlgorithmContext& roiCtx)
{
	GTRegion* pROI = roiCtx.pInspectROIRgn;
	int iTabIdx = roiCtx.iTabIdx;
	CAlgorithmParam& param = pROI->m_AlgorithmParam[iTabIdx];

	// ROI Region 획득
	roiCtx.HROIHRegion = pROI->GetROIHRegion(
		THEAPP.m_pCalDataService_N[roiCtx.algCtx.iVisionCamType]);

	// ROI 크기 측정 (AI 이미지 크롭 사이즈 고정용, 얼라인 전 측정)
	HTuple trash;
	SmallestRectangle2(roiCtx.HROIHRegion, &trash, &trash, &trash,
					   &roiCtx.length1, &roiCtx.length2);

	// 전처리 플래그
	roiCtx.bUsePreprocess = param.m_bUseImageProcess || param.m_bUseImageProcessFilter;
	roiCtx.bAllROIInspection = THEAPP.Struct_PreferenceStruct.m_bAllImageAiInspection;
	roiCtx.bEnableROIProcessing = roiCtx.bAllROIInspection
		|| THEAPP.Struct_PreferenceStruct.m_bSaveROIImage
		|| param.m_bUseFilter1
		|| param.m_bUseFilter2
		|| param.m_bUseFilter3;

	// AI 검사 플래그
	roiCtx.bUseAIInspection = param.m_bUseFilter1
		|| param.m_bUseFilter2
		|| param.m_bUseFilter3;

	// 채널 타입 문자열
	roiCtx.channelType = std::string(g_channel_type[param.m_iProcessChType]);

	// Local align angle 문자열 초기화
	roiCtx.localAlignAngleString = "P0000000";

	// AI prefix 문자열 생성
	const AlgorithmContext& algCtx = roiCtx.algCtx;
	std::ostringstream prefix;
	prefix
		<< std::string(THEAPP.Struct_PreferenceStruct.m_strEquipNo)
		<< "_" << std::string(THEAPP.g_strModelTypeName[THEAPP.GetModelType()])
		<< "_" << std::string(algCtx.sLotID)
		<< "_Tray" << algCtx.iTrayNo
		<< "_Module" << algCtx.iModuleNo
		<< "_" << std::string(algCtx.sVisionCamType_Short)
		<< "" << (THEAPP.Struct_PreferenceStruct.m_iPCType + 1)
		<< "_Stage" << (algCtx.iStageNo + 1)
		<< "_Image" << roiCtx.iImageIdx
		<< "_ROI" << pROI->miInspectionROIID
		<< "_" << g_strInspectionTypeName_Short[pROI->miInspectionType - 1]
		<< "_Tab" << (iTabIdx + 1)
		<< "_" << THEAPP.m_strModuleBarcodeID[algCtx.iMzNo - 1][algCtx.iTrayNo - 1][algCtx.iModuleNo - 1];

	roiCtx.prefixString = prefix.str();
}

void AlgorithmHelper::HandleTrayChange(const AlgorithmContext& algCtx)
{
	CString strLog;
	CSingleLock sUpdate(&(m_pAlgorithm->m_csScreenUpdate));
	sUpdate.Lock();

	BOOL bUpdateScreen = FALSE;
	if (m_pAlgorithm->m_iPrevTrayNo != algCtx.iTrayNo)
	{
		bUpdateScreen = TRUE;
		m_pAlgorithm->m_iPrevTrayNo = algCtx.iTrayNo;
	}
	if (m_pAlgorithm->m_sPrevLotID != algCtx.sLotID)
	{
		bUpdateScreen = TRUE;
		m_pAlgorithm->m_sPrevLotID = algCtx.sLotID;
	}

	BOOL bCMIFolderNull;
	if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
		bCMIFolderNull = (THEAPP.m_FileBase.m_strDayResultLogFolder_Local[algCtx.iMzNo - 1] == _T(""));
	else
		bCMIFolderNull = (THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[algCtx.iMzNo - 1] == _T(""));

	if (bUpdateScreen == TRUE || bCMIFolderNull)
	{
		if (algCtx.iPcVisionNo == VISION_NUMBER_1)
			THEAPP.m_pInspectSummary->Initialize_TrayStart_Vision_N1(algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo);
		else if (algCtx.iPcVisionNo == VISION_NUMBER_2)
			THEAPP.m_pInspectSummary->Initialize_TrayStart_Vision_N2(algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo);
		else if (algCtx.iPcVisionNo == VISION_NUMBER_3)
			THEAPP.m_pInspectSummary->Initialize_TrayStart_Vision_N3(algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo);
		else if (algCtx.iPcVisionNo == VISION_NUMBER_4)
			THEAPP.m_pInspectSummary->Initialize_TrayStart_Vision_N4(algCtx.sLotID, algCtx.iMzNo, algCtx.iTrayNo);
	}

	sUpdate.Unlock();
}

void AlgorithmHelper::StoreDefectRegion(int iBufferIdx, int iImageIdx, int iInspectionType, int iDefectType, const HObject& HRegion)
{
	ConcatObj(m_pAlgorithm->m_HDefectRgn_ImageNo[iBufferIdx][iImageIdx - 1],
			  HRegion,
			  &(m_pAlgorithm->m_HDefectRgn_ImageNo[iBufferIdx][iImageIdx - 1]));
	ConcatObj(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][iInspectionType - 1][iDefectType],
			  HRegion,
			  &(m_pAlgorithm->m_HDefectRgn_DefectName[iBufferIdx][iInspectionType - 1][iDefectType]));
}

std::vector<std::string> AlgorithmHelper::GetSaveDirectories(const AlgorithmContext& algCtx)
{
	std::string roiImageSaveLocalDir = std::string(
		THEAPP.m_FileBase.m_strAIFolder_Local[algCtx.iPcVisionNo][algCtx.iMzNo - 1][algCtx.iTrayNo - 1]);
	std::string roiImageSaveLASDir = std::string(
		THEAPP.m_FileBase.m_strAIFolder_LAS[algCtx.iPcVisionNo][algCtx.iMzNo - 1][algCtx.iTrayNo - 1]);

	bool saveLocal = static_cast<bool>(THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal);
	bool saveLAS = static_cast<bool>(THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS);
	bool isSameDirectory = roiImageSaveLocalDir == roiImageSaveLASDir;

	std::vector<std::string> saveDirectories;
	saveDirectories.reserve(2);

	if (isSameDirectory)
	{
		if (saveLocal || saveLAS)
			saveDirectories.push_back(roiImageSaveLocalDir);
	}
	else
	{
		if (saveLocal) saveDirectories.push_back(roiImageSaveLocalDir);
		if (saveLAS)   saveDirectories.push_back(roiImageSaveLASDir);
	}

	return saveDirectories;
}

void AlgorithmHelper::CopyGrabImages(int iScanBufferIdx, const AlgorithmCopyParam& param)
{
	int iPcVisionNo = m_pAlgorithm->m_iVisionCamType;
	int iNoInspect = m_pAlgorithm->m_iNoInspectImage;
	HObject HColorImage;

	int iStartIdx = param.IsBatchMode() ? param.pBatchInfo->iStartImageIdx : 0;
	int iEndIdx = param.IsBatchMode() ? param.pBatchInfo->iEndImageIdx : (iNoInspect - 1);

	CBatchManager* pBatchManager = param.IsBatchMode() ? CBatchManager::GetInstance() : nullptr;
	std::unique_ptr<CSingleLock> pBatchGrabLock;
	if (pBatchManager)
		pBatchGrabLock = std::make_unique<CSingleLock>(&pBatchManager->m_csGrab[iPcVisionNo]);

	switch (param.copyMode)
	{
	case AlgorithmCopyParam::NORMAL:
		for (int i = iStartIdx; i <= iEndIdx; i++)
		{
			if (pBatchGrabLock) pBatchGrabLock->Lock();

			if (param.bColor)
			{
				Compose3(param.ppGrabHImage[i][0], param.ppGrabHImage[i][1],
						 param.ppGrabHImage[i][2], &HColorImage);
				if (pBatchGrabLock) pBatchGrabLock->Unlock();

				CopyImage(HColorImage,
						  &(m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][i]));
			}
			else
			{
				CopyImage(param.ppGrabHImage[i][0],
						  &(m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_I][i])); \
					if (pBatchGrabLock) pBatchGrabLock->Unlock();
			}
		}
		break;

	case AlgorithmCopyParam::DFM:
	{
		int iStageNo = param.iStageNo;
		int iImageCnt = iStartIdx;
		int iGrabCount = param.IsBatchMode() ? param.pBatchInfo->iStartGrabIdx : 0;
		int iSpecularCount = param.IsBatchMode() ? param.pBatchInfo->iStartSpecularIdx : 0;
		int iDiffusedCount = param.IsBatchMode() ? param.pBatchInfo->iStartDiffusedIdx : 0;
		int iNoUsedImageGrab = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;
		int iEndImage = param.IsBatchMode() ? (iEndIdx + 1) : iNoUsedImageGrab;

		while (iImageCnt < iEndImage)
		{
			int iSeqAddr = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iImageCnt];
			int iNoSeq = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iAddrCount[iSeqAddr];

			if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularImage[iImageCnt])
			{
				for (int i = 0; i < SPV_RAW_IMAGE_NUMBER; i++)
				{
					if (pBatchGrabLock) pBatchGrabLock->Lock();
					Compose3(param.ppGrabHImage[iGrabCount][0], param.ppGrabHImage[iGrabCount][1],
							 param.ppGrabHImage[iGrabCount][2], &HColorImage);
					if (pBatchGrabLock) pBatchGrabLock->Unlock();

					if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularRoiCrop[iImageCnt])
					{
						if (i == 0)
						{
							if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt] == 1)
								CopyImage(HColorImage, &m_pAlgorithm->m_HInspectSpecularBackgroundImage[iScanBufferIdx][iSpecularCount]);
							else
								ZoomImageFactor(HColorImage,
												&m_pAlgorithm->m_HInspectSpecularBackgroundImage[iScanBufferIdx][iSpecularCount],
												THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
												THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
												"nearest_neighbor");
						}

						CropRectangle1(HColorImage,
									   &m_pAlgorithm->m_HInspectSpecularRawImage[iScanBufferIdx][iSpecularCount][i],
									   THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropLTPointY_Roi[iImageCnt],
									   THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropLTPointX_Roi[iImageCnt],
									   THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropRBPointY_Roi[iImageCnt],
									   THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropRBPointX_Roi[iImageCnt]);

						if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt] < 1)
							ZoomImageFactor(m_pAlgorithm->m_HInspectSpecularRawImage[iScanBufferIdx][iSpecularCount][i],
											&m_pAlgorithm->m_HInspectSpecularRawImage[iScanBufferIdx][iSpecularCount][i],
											THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
											THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
											"nearest_neighbor");
					}
					else
					{
						if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt] < 1)
							ZoomImageFactor(HColorImage,
											&m_pAlgorithm->m_HInspectSpecularRawImage[iScanBufferIdx][iSpecularCount][i],
											THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
											THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
											"nearest_neighbor");
						else
							CopyImage(HColorImage, &m_pAlgorithm->m_HInspectSpecularRawImage[iScanBufferIdx][iSpecularCount][i]);
					}

					++iGrabCount;
				}
				iImageCnt += DFM_USED_CONV_IMAGE_NUMBER;
				++iSpecularCount;
			}
			else if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseDiffusedImage[iImageCnt])
			{
				for (int i = 0; i < DIFFUSED_RAW_IMAGE_NUMBER; i++)
				{
					if (pBatchGrabLock) pBatchGrabLock->Lock();
					Compose3(param.ppGrabHImage[iGrabCount][0], param.ppGrabHImage[iGrabCount][1],
							 param.ppGrabHImage[iGrabCount][2], &HColorImage);
					if (pBatchGrabLock) pBatchGrabLock->Unlock();

					if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularRoiCrop[iImageCnt])
					{
						if (i == 0)
						{
							if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt] == 1)
								CopyImage(HColorImage, &m_pAlgorithm->m_HInspectDiffusedBackgroundImage[iScanBufferIdx][iDiffusedCount]);
							else
								ZoomImageFactor(HColorImage,
												&m_pAlgorithm->m_HInspectDiffusedBackgroundImage[iScanBufferIdx][iDiffusedCount],
												THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
												THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
												"nearest_neighbor");
						}

						CropRectangle1(HColorImage,
									   &m_pAlgorithm->m_HInspectDiffusedRawImage[iScanBufferIdx][iDiffusedCount][i],
									   THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropLTPointY_Roi[iImageCnt],
									   THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropLTPointX_Roi[iImageCnt],
									   THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropRBPointY_Roi[iImageCnt],
									   THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropRBPointX_Roi[iImageCnt]);

						if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt] < 1)
							ZoomImageFactor(m_pAlgorithm->m_HInspectDiffusedRawImage[iScanBufferIdx][iDiffusedCount][i],
											&m_pAlgorithm->m_HInspectDiffusedRawImage[iScanBufferIdx][iDiffusedCount][i],
											THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
											THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
											"nearest_neighbor");
					}
					else
					{
						if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt] < 1)
							ZoomImageFactor(HColorImage,
											&m_pAlgorithm->m_HInspectDiffusedRawImage[iScanBufferIdx][iDiffusedCount][i],
											THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
											THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageCnt],
											"nearest_neighbor");
						else
							CopyImage(HColorImage, &(m_pAlgorithm->m_HInspectDiffusedRawImage[iScanBufferIdx][iDiffusedCount][i]));
					}

					++iGrabCount;
				}
				iImageCnt += DIFFUSED_USED_CONV_IMAGE_NUMBER;
				++iDiffusedCount;
			}
			else
			{
				for (int i = 0; i < iNoSeq; i++)
				{
					if (pBatchGrabLock) pBatchGrabLock->Lock();
					Compose3(param.ppGrabHImage[iGrabCount][0], param.ppGrabHImage[iGrabCount][1],
							 param.ppGrabHImage[iGrabCount][2], &HColorImage);
					if (pBatchGrabLock) pBatchGrabLock->Unlock();

					CopyImage(HColorImage,
							  &(m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageCnt]));

					++iGrabCount;
					++iImageCnt;
				}
			}
		}

		// DFM GPU 변환
		int iSpecStart = param.IsBatchMode() ? param.pBatchInfo->iStartSpecularIdx : 0;
		int iDiffStart = param.IsBatchMode() ? param.pBatchInfo->iStartDiffusedIdx : 0;
		if (!ProcessImageConversion(iScanBufferIdx, iPcVisionNo,
									param.iMzNo, param.iModuleNo, param.iStageNo,
									iStartIdx, iEndImage, iSpecStart, iDiffStart,
									THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo],
									param.sLotID, param.iTrayNo))
		{
			m_pAlgorithm->m_bGrabFail[iScanBufferIdx] = TRUE;
		}
		break;
	}

	case AlgorithmCopyParam::PROFILE_3D:
	{
		HObject HByteImage;
		int iGrabCount = 0;
		for (int i = 0; i < (iNoInspect / 2); i++)
		{
			Compose3(param.pHIntensityImage[i], param.pHIntensityImage[i],
					 param.pHIntensityImage[i], &HColorImage);
			CopyImage(HColorImage,
					  &(m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iGrabCount++]));

			CopyImage(param.pHProfileImage[i],
					  &(m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_I][i + iNoInspect]));

			m_pAlgorithm->ConvertProfileToByteImage(param.pHProfileImage[i], &HByteImage);
			Compose3(HByteImage, HByteImage, HByteImage, &HColorImage);
			CopyImage(HColorImage,
					  &(m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iGrabCount++]));
		}
		break;
	}

	case AlgorithmCopyParam::MONO_GRAB:
		for (int i = 0; i < iNoInspect; i++)
			CopyImage(param.pHImage[i],
					  &(m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_I][i]));
		break;
	}
}

BOOL AlgorithmHelper::ProcessImageConversion(
	int iScanBufferIdx, int iPcVisionNo,
	int iMzNo, int iModuleNo, int iStageNo,
	int iStartImageIdx, int iEndImageIdx,
	int iStartSpecularIdx, int iStartDiffusedIdx,
	CString sVisionCamType_Short, CString sLotID, int iTrayNo)
{
	CString strLog;
	BOOL bDFMProcRet = TRUE;
	int iSpecularPcVisionNo = VISION_NUMBER_1;

	// Phase 1: Specular CalNormal (CPU) 
	HObject HSpecularGrayImage[DFM_VISION_MAX_CONVERSION][SPV_RAW_IMAGE_NUMBER];
	HObject HSpecularConvNormalImage[DFM_VISION_MAX_CONVERSION];

	int iCurSpecularIdx = iStartSpecularIdx;
	for (int iImageIdx = iStartImageIdx; iImageIdx < iEndImageIdx; )
	{
		if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularImage[iImageIdx])
		{
			bDFMProcRet = THEAPP.m_pDualCameraManager[iPcVisionNo]->DFMCalNormal(
				m_pAlgorithm->m_HInspectSpecularRawImage[iScanBufferIdx][iCurSpecularIdx],
				HSpecularGrayImage[iCurSpecularIdx],
				&(HSpecularConvNormalImage[iCurSpecularIdx]),
				TRUE, iModuleNo, iCurSpecularIdx + 1);

			if (bDFMProcRet == FALSE)
				return FALSE;

			iImageIdx += DFM_USED_CONV_IMAGE_NUMBER;
			++iCurSpecularIdx;
		}
		else
			++iImageIdx;
	}

	// Phase 2: Diffused CalNormal (CPU) 
	HObject HDiffusedGrayImage[DIFFUSED_VISION_MAX_CONVERSION][DIFFUSED_RAW_IMAGE_NUMBER];

	int iCurDiffusedIdx = iStartDiffusedIdx;
	for (int iImageIdx = iStartImageIdx; iImageIdx < iEndImageIdx; )
	{
		if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseDiffusedImage[iImageIdx])
		{
			bDFMProcRet = THEAPP.m_pDualCameraManager[iPcVisionNo]->DiffusedCalNormal(
				m_pAlgorithm->m_HInspectDiffusedRawImage[iScanBufferIdx][iCurDiffusedIdx],
				HDiffusedGrayImage[iCurDiffusedIdx],
				TRUE, iModuleNo, iCurDiffusedIdx + 1);

			if (bDFMProcRet == FALSE)
				return FALSE;

			iImageIdx += DIFFUSED_USED_CONV_IMAGE_NUMBER;
			++iCurDiffusedIdx;
		}
		else
			++iImageIdx;
	}

	// Phase 3: GPU Specular Processing 
	CSingleLock sGpuProc(&(THEAPP.m_pInspectService->m_csSpecularGpuProc));
	sGpuProc.Lock();

	THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->SetGpuProcMode(GPU_PROC_MODE_SPECULAR);

	iCurSpecularIdx = iStartSpecularIdx;
	for (int iImageIdx = iStartImageIdx; iImageIdx < iEndImageIdx; )
	{
		if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularImage[iImageIdx])
		{
			auto dfmStart = std::chrono::high_resolution_clock::now();

			THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->SetSpecularProcParameter(
				iMzNo, HSpecularGrayImage[iCurSpecularIdx],
				HSpecularConvNormalImage[iCurSpecularIdx],
				iPcVisionNo, iPcVisionNo, TRUE, FALSE, TRUE);

			THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->m_eventImageReady.SetEvent();

			CSingleLock s(&(THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->m_eventDFMProcDone));
			s.Lock();

			if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularRoiCrop[iImageIdx])
			{
				THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->m_HDFMRawImage[0] =
					m_pAlgorithm->m_HInspectSpecularBackgroundImage[iScanBufferIdx][iCurSpecularIdx];
				bDFMProcRet = THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->GetSpecularCropProcParameter(
					&m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx],
					THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropLTPointX_Roi[iImageIdx]
					* THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageIdx],
					THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropLTPointY_Roi[iImageIdx]
					* THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageIdx],
					THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageIdx],
					iPcVisionNo);
			}
			else
			{
				bDFMProcRet = THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->GetSpecularProcParameter(
					&m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx],
					THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageIdx],
					iPcVisionNo);
			}

			auto dfmMs = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now() - dfmStart).count();

			if (bDFMProcRet == FALSE)
			{
				strLog.Format("%s/ GPU caculate fail, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Specular: %d",
							  sVisionCamType_Short, dfmMs, sLotID, iMzNo, iTrayNo, iModuleNo, iCurSpecularIdx + 1);
				THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));
				sGpuProc.Unlock();
				return FALSE;
			}

			strLog.Format("%s/ GPU caculate, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Specular: %d",
						  sVisionCamType_Short, dfmMs, sLotID, iMzNo, iTrayNo, iModuleNo, iCurSpecularIdx + 1);
			THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

			iImageIdx += DFM_USED_CONV_IMAGE_NUMBER;
			++iCurSpecularIdx;
		}
		else
			++iImageIdx;
	}

	// Phase 4: GPU Diffused Processing 
	THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->SetGpuProcMode(GPU_PROC_MODE_DIFFUSED);

	iCurDiffusedIdx = iStartDiffusedIdx;
	for (int iImageIdx = iStartImageIdx; iImageIdx < iEndImageIdx; )
	{
		if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseDiffusedImage[iImageIdx])
		{
			auto dfmStart = std::chrono::high_resolution_clock::now();

			THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->SetDiffusedProcParameter(
				iMzNo, HDiffusedGrayImage[iCurDiffusedIdx],
				iPcVisionNo, iPcVisionNo, TRUE);

			THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->m_eventImageReady.SetEvent();

			CSingleLock s(&(THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->m_eventDFMProcDone));
			s.Lock();

			if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularRoiCrop[iImageIdx])
			{
				THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->m_HDFMRawImage[0] =
					m_pAlgorithm->m_HInspectDiffusedBackgroundImage[iScanBufferIdx][iCurDiffusedIdx];
				bDFMProcRet = THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->GetDiffusedCropProcParameter(
					&m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx],
					THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropLTPointX_Roi[iImageIdx]
					* THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageIdx],
					THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_lSpecularCropLTPointY_Roi[iImageIdx]
					* THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageIdx],
					THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageIdx],
					iPcVisionNo);
			}
			else
			{
				bDFMProcRet = THEAPP.m_pDualCameraManager[iSpecularPcVisionNo]->GetDiffusedProcParameter(
					&m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx],
					THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dSpecularResizeFactor[iImageIdx],
					iPcVisionNo);
			}

			auto dfmMs = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now() - dfmStart).count();

			if (bDFMProcRet == FALSE)
			{
				strLog.Format("%s/ GPU caculate fail, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Diffused: %d",
							  sVisionCamType_Short, dfmMs, sLotID, iMzNo, iTrayNo, iModuleNo, iCurDiffusedIdx + 1);
				THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));
				sGpuProc.Unlock();
				return FALSE;
			}

			strLog.Format("%s/ GPU caculate, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Diffused: %d",
						  sVisionCamType_Short, dfmMs, sLotID, iMzNo, iTrayNo, iModuleNo, iCurDiffusedIdx + 1);
			THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

			iImageIdx += DIFFUSED_USED_CONV_IMAGE_NUMBER;
			++iCurDiffusedIdx;
		}
		else
			++iImageIdx;
	}

	sGpuProc.Unlock();

	// Phase 5: Stage 회전 
	if (iStageNo == STAGE_NUMBER_2)
	{
		for (int i = iStartImageIdx; i < iEndImageIdx; i++)
		{
			double dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][i];
			if (dStageT > 10)
			{
				RotateImage(m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][i],
							&(m_pAlgorithm->m_HInspectCImage[iScanBufferIdx][CHANNEL_TYPE_COLOR][i]),
							180, "nearest_neighbor");
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Inspection Helper
//////////////////////////////////////////////////////////////////////////
void AlgorithmHelper::ApplyPreprocessing(ROIAlgorithmContext& roiCtx)
{
	int iBufferIdx = roiCtx.algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = roiCtx.algCtx.iCurThreadIdx;
	int iPcVisionNo = roiCtx.algCtx.iPcVisionNo;
	int iMzNo = roiCtx.algCtx.iMzNo;
	CAlgorithmParam& param = roiCtx.pInspectROIRgn->m_AlgorithmParam[roiCtx.iTabIdx];

	// 1. LocalAlign (angle/delta 출력 포함)
	if (param.m_bUseImageProcessLocalAlign)
		m_pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, param,
											&roiCtx.HROIHRegion, iCurThreadIdx,
											&roiCtx.dLocalAlignAngle, &roiCtx.iLocalAlignDeltaX, &roiCtx.iLocalAlignDeltaY);

	// 2. Generate
	if (param.m_bUseImageProcessGenerate)
		m_pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, param,
										  &roiCtx.HROIHRegion, iCurThreadIdx);

	// 3. DontCare
	if (param.m_bUseImageProcessDontCare || param.m_bUseImageProcessDontCare2)
		m_pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, param,
										  &roiCtx.HROIHRegion, iCurThreadIdx);

	// 4. Mask
	if (param.m_bUseImageProcessMask || param.m_bUseImageProcessMask2)
		m_pAlgorithm->ApplyMaskResult(iMzNo, iPcVisionNo, param,
									  &roiCtx.HMaskRgn, iCurThreadIdx);
	else
		GenEmptyObj(&roiCtx.HMaskRgn);

	// AI 로컬 얼라인 각도 문자열 갱신
	if (roiCtx.dLocalAlignAngle == 0.0)
		roiCtx.localAlignAngleString = "P0000000";
	else
	{
		char sign = (roiCtx.dLocalAlignAngle < 0) ? 'M' : 'P';
		double absAngle = fabs(roiCtx.dLocalAlignAngle);
		int intPart = static_cast<int>(absAngle);
		int decimalPart = static_cast<int>((absAngle - intPart) * 10000);

		std::ostringstream oss;
		oss << sign
			<< std::setw(3) << std::setfill('0') << intPart
			<< std::setw(4) << std::setfill('0') << decimalPart;

		roiCtx.localAlignAngleString = oss.str();
	}
}

void AlgorithmHelper::InspectAlign(ROIAlgorithmContext& roiCtx)
{
	if (roiCtx.iTabIdx != 0)
		return;

	int iBufferIdx = roiCtx.algCtx.iCurInspectionBufferIdx;
	int iImageIdx = roiCtx.iImageIdx;
	CAlgorithmParam& param = roiCtx.pInspectROIRgn->m_AlgorithmParam[roiCtx.iTabIdx];

	BOOL bROIAlignShiftResult = FALSE;
	double dAlignMatchingScore = 0.0;
	HObject HAlignRgn;

	// ROI Align (등방)
	if (param.m_bUseROIAlign && param.m_bROIAlignMultiModuleInspect == FALSE)
	{
		if (param.m_bUseIsoColorImage)
			HAlignRgn = m_pAlgorithm->ROIAlignAlgorithm(
				m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1],
				roiCtx.HROIHRegion, param,
				&(roiCtx.pInspectROIRgn->m_HROIAlignModelID),
				&bROIAlignShiftResult, &dAlignMatchingScore);
		else
			HAlignRgn = m_pAlgorithm->ROIAlignAlgorithm(
				m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChType][iImageIdx - 1],
				roiCtx.HROIHRegion, param,
				&(roiCtx.pInspectROIRgn->m_HROIAlignModelID),
				&bROIAlignShiftResult, &dAlignMatchingScore);

		// Shift NG -> ROI 전체 영역 불량 저장
		if (param.m_bROIAlignInspectShift && bROIAlignShiftResult)
		{
			if (CGFunction::ValidHRegion(roiCtx.HROIHRegion) == TRUE)
				StoreDefectRegion(iBufferIdx, iImageIdx,
								  roiCtx.pInspectROIRgn->miInspectionType,
								  param.m_iDefectType, roiCtx.HROIHRegion);
		}

		if (CGFunction::ValidHRegion(HAlignRgn) == TRUE)
			roiCtx.HROIHRegion = HAlignRgn;
	}

	// ROI AnisoAlign (비등방)
	if (param.m_bUseROIAnisoAlign && param.m_bROIAnisoAlignMultiModuleInspect == FALSE)
	{
		if (param.m_bUseAnisoColorImage)
			HAlignRgn = m_pAlgorithm->ROIAnisoAlignAlgorithm(
				m_pAlgorithm->m_HInspectCImage[iBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1],
				roiCtx.HROIHRegion, param,
				&(roiCtx.pInspectROIRgn->m_HROIAlignModelID),
				&bROIAlignShiftResult, &dAlignMatchingScore);
		else
			HAlignRgn = m_pAlgorithm->ROIAnisoAlignAlgorithm(
				m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChType][iImageIdx - 1],
				roiCtx.HROIHRegion, param,
				&(roiCtx.pInspectROIRgn->m_HROIAlignModelID),
				&bROIAlignShiftResult, &dAlignMatchingScore);

		// Shift NG -> ROI 전체 영역 불량 저장
		if (param.m_bROIAnisoAlignInspectShift && bROIAlignShiftResult)
		{
			if (CGFunction::ValidHRegion(roiCtx.HROIHRegion) == TRUE)
				StoreDefectRegion(iBufferIdx, iImageIdx,
								  roiCtx.pInspectROIRgn->miInspectionType,
								  param.m_iDefectType, roiCtx.HROIHRegion);
		}

		if (CGFunction::ValidHRegion(HAlignRgn) == TRUE)
			roiCtx.HROIHRegion = HAlignRgn;
	}
}

BOOL AlgorithmHelper::InspectCosmetic(ROIAlgorithmContext& roiCtx)
{
	const AlgorithmContext& algCtx = roiCtx.algCtx;
	int iBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iImageIdx = roiCtx.iImageIdx;
	CAlgorithmParam& param = roiCtx.pInspectROIRgn->m_AlgorithmParam[roiCtx.iTabIdx];

	if (roiCtx.bUsePreprocess)
	{
		m_pAlgorithm->GetPreprocessImage(
			TRUE,
			m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChType][iImageIdx - 1],
			m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChType],
			roiCtx.HROIHRegion, param,
			&roiCtx.HPreProcessImage);

#ifndef LGITAI
		roiCtx.HDefectRgn = m_pAlgorithm->InspectAVI(
			iBufferIdx, algCtx.iCurThreadIdx,
			roiCtx.pInspectROIRgn, roiCtx.iTabIdx,
			roiCtx.HPreProcessImage,
			roiCtx.HROIHRegion, param, &roiCtx.HResultXLD,
			m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChType],
			m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChTypeDC][iImageIdx - 1],
			&roiCtx.HMaskRgn);
#endif
	}
	else
	{
#ifndef LGITAI
		roiCtx.HDefectRgn = m_pAlgorithm->InspectAVI(
			iBufferIdx, algCtx.iCurThreadIdx,
			roiCtx.pInspectROIRgn, roiCtx.iTabIdx,
			m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChType][iImageIdx - 1],
			roiCtx.HROIHRegion, param, &roiCtx.HResultXLD,
			m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChType],
			m_pAlgorithm->m_HInspectCImage[iBufferIdx][param.m_iProcessChTypeDC][iImageIdx - 1],
			&roiCtx.HMaskRgn);
#endif
		if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[algCtx.iPcVisionNo])
			return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// AI Helper
//////////////////////////////////////////////////////////////////////////
void AlgorithmHelper::SubmitLGAI(
	ROIAlgorithmContext& roiCtx, ResultCollector& collector,
	const std::string& saveDir, const std::vector<std::string>& saveDirectories)
{
#ifdef LGITAI
	const AlgorithmContext& algCtx = roiCtx.algCtx;
	Algorithm* pAlgorithm = m_pAlgorithm;
	int iCurInspectionBufferIdx = algCtx.iCurInspectionBufferIdx;
	int iCurThreadIdx = algCtx.iCurThreadIdx;
	GTRegion* pInspectROIRgn = roiCtx.pInspectROIRgn;
	int iTabIdx = roiCtx.iTabIdx;
	int iImageIdx = roiCtx.iImageIdx;
	int iROIIndex = roiCtx.iROIIndex;
	int iVisionCamType = algCtx.iVisionCamType;
	HObject& HROIHRegion = roiCtx.HROIHRegion;
	HObject& HPreProcessImage = roiCtx.HPreProcessImage;
	HObject& HDefectRgn = roiCtx.HDefectRgn;
	HObject& HMaskRgn = roiCtx.HMaskRgn;
	HObject& HResultXLD = roiCtx.HResultXLD;
	HTuple& length1 = roiCtx.length1;
	HTuple& length2 = roiCtx.length2;

	std::string pc_id = std::string(THEAPP.GetPCID());
	std::string eq_id = std::string(THEAPP.Struct_PreferenceStruct.m_strEquipNo);
	std::string model_type = std::string(THEAPP.g_strModelTypeName[THEAPP.GetModelType()]);
	std::string vision_name = std::string(CT2A(algCtx.sVisionCamType_Short));
	std::string inspection_type_name = g_strInspectionTypeName_Short[pInspectROIRgn->miInspectionType - 1];
	std::string barcode_id = THEAPP.m_strModuleBarcodeID[algCtx.iMzNo - 1][algCtx.iTrayNo - 1][algCtx.iModuleNo - 1];
	std::string local_align_angle_string = roiCtx.localAlignAngleString;
	std::string channel_type = roiCtx.channelType;

	// ROI 이미지 전처리 수행 여부 확인
	bool all_roi_inspection = roiCtx.bAllROIInspection;
	bool enable_roi_processing = roiCtx.bEnableROIProcessing;

#pragma region Image Preprocessing
	int roi_id = pInspectROIRgn->miInspectionROIID;
	int roi_idx = iROIIndex;
	HObject image = pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1];
	HObject ho_Cropped, ho_Cropped_v2;
	cv::Mat cv_roi_img, cv_roi_img_v2;
	HalconCpp::HTuple raw_image_width, raw_image_height;
	HTuple crop_row1, crop_col1, crop_row2, crop_col2;
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

	// Burr는 패딩을 크게 줌 (ROI보다 실제 검사 영역이 큼)
	int iPadding = 128; // 50
	int dilation = 5; // 50
	if (THEAPP.m_strDefectName[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType] == "Burr") {
		iPadding = 128; // 100
		dilation = 10; // 50
	}

	// ROI 이미지 전처리 비활성화 시 결함 영역 저장 후 이후 단계 패스
	if (!enable_roi_processing) {
		HDefectRgn = pAlgorithm->InspectAVI(
			iCurInspectionBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
			pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1],
			HROIHRegion,
			pInspectROIRgn->m_AlgorithmParam[iTabIdx],
			&HResultXLD,
			pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType],
			pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1],
			&HMaskRgn);
		if (CGFunction::ValidHRegion(HDefectRgn))
		{
			ConcatObj(pAlgorithm->m_HDefectRgn_ImageNo[iCurInspectionBufferIdx][iImageIdx - 1], HDefectRgn, &(pAlgorithm->m_HDefectRgn_ImageNo[iCurInspectionBufferIdx][iImageIdx - 1]));
			ConcatObj(pAlgorithm->m_HDefectRgn_DefectName[iCurInspectionBufferIdx][pInspectROIRgn->miInspectionType - 1][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType],
					  HDefectRgn,
					  &(pAlgorithm->m_HDefectRgn_DefectName[iCurInspectionBufferIdx][pInspectROIRgn->miInspectionType - 1][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType]));
		}
		THEAPP.m_syai_logger->debug("ROI Image Preprocessing is disabled. Skip ROI preprocessing. LotID: {}, Image Idx: {}, Roi Idx: {}, Tab Idx: {}", std::string(algCtx.sLotID), iImageIdx, iROIIndex, iTabIdx);
		return;
	}
	// ROI 이미지 전처리 실패시 결함 영역 저장 후 이후 단계 패스
	if (!AIService::ImageUtile::RoiImagePreprocessing(image, HROIHRegion,
													  length1, length2,
													  ho_Cropped, cv_roi_img,
													  ho_Cropped_v2, cv_roi_img_v2,
													  raw_image_width, raw_image_height,
													  crop_row1, crop_col1, crop_row2, crop_col2,
													  iPadding, dilation, THEAPP.m_syai_logger)) {
		HDefectRgn = pAlgorithm->InspectAVI(
			iCurInspectionBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
			pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1],
			HROIHRegion,
			pInspectROIRgn->m_AlgorithmParam[iTabIdx],
			&HResultXLD,
			pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType],
			pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1],
			&HMaskRgn);
		if (CGFunction::ValidHRegion(HDefectRgn))
		{
			ConcatObj(pAlgorithm->m_HDefectRgn_ImageNo[iCurInspectionBufferIdx][iImageIdx - 1], HDefectRgn, &(pAlgorithm->m_HDefectRgn_ImageNo[iCurInspectionBufferIdx][iImageIdx - 1]));
			ConcatObj(pAlgorithm->m_HDefectRgn_DefectName[iCurInspectionBufferIdx][pInspectROIRgn->miInspectionType - 1][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType],
					  HDefectRgn,
					  &(pAlgorithm->m_HDefectRgn_DefectName[iCurInspectionBufferIdx][pInspectROIRgn->miInspectionType - 1][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType]));
		}
		THEAPP.m_syai_logger->warn("ROI Image Preprocessing failed. Skip AI inspection. LotID: {}, Image Idx: {}, Roi Idx: {}, Tab Idx: {}", std::string(algCtx.sLotID), iImageIdx, iROIIndex, iTabIdx);
		return;
	}
	x1 = static_cast<int>(crop_col1.D());
	y1 = static_cast<int>(crop_row1.D());
	x2 = static_cast<int>(crop_col2.D());
	y2 = static_cast<int>(crop_row2.D());
#pragma endregion Image Preprocessing

	int indpection_type_idx = pInspectROIRgn->miInspectionType - 1;
	// 요청 객체 생성
	HObject temp_mask;
	HalconCpp::GenEmptyRegion(&temp_mask);
	int pc_idx = THEAPP.Struct_PreferenceStruct.m_iPCType + 1;
	int defect_name_index = pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType;
	std::string defect_name = std::string(CT2A(THEAPP.m_strDefectName[defect_name_index]));

	std::string inspector1_key = LGIT::ModelInference::MakeInferenceEngineRunMemoryKey(model_type, iVisionCamType, iImageIdx, roi_id, indpection_type_idx);
	bool is_contain_key = THEAPP.m_lgAIInspector->Keycontain(inspector1_key);

	auto inspection_request = std::make_shared<LGIT::InspectionRequest>(
		saveDir,
		false, false, false, false,
		pc_id,
		eq_id,
		model_type,
		iCurInspectionBufferIdx,
		std::string(algCtx.sLotID), iVisionCamType, algCtx.iMzNo,
		algCtx.iTrayNo, algCtx.iModuleNo,
		vision_name, pc_idx, algCtx.iStageNo + 1,
		iImageIdx, roi_idx, roi_id,
		inspection_type_name, indpection_type_idx, iTabIdx,
		defect_name_index, defect_name,
		local_align_angle_string,
		roiCtx.dLocalAlignAngle,
		roiCtx.iLocalAlignDeltaX,
		roiCtx.iLocalAlignDeltaY,
		channel_type,
		std::move(cv_roi_img_v2),
		std::move(cv_roi_img),
		std::move(temp_mask));
	inspection_request->is_use_local_segmentation = THEAPP.Struct_PreferenceStruct.m_bUseLocalSEG;
	inspection_request->is_use_val = THEAPP.Struct_PreferenceStruct.m_bAISetupUseVAL;

	inspection_request->roi_bbox = LGIT::Model::BoundingBox{ x1, y1, x2, y2 };
	inspection_request->org_image_width = raw_image_width.I();
	inspection_request->org_image_height = raw_image_height.I();
	inspection_request->barcode = barcode_id;

	// AI 검사 조건 처리
	if (is_contain_key && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseFilter1) {
		inspection_request->primary_inspect_enable = true;
	}
	if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseFilter2) {
		inspection_request->secondary_inspect_enable = true;
	}
	if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseFilter3) {
		inspection_request->tertiary_inspect_enable = true;
	}
	if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseFilter4) {
		inspection_request->svm_inspect_enable = true;
	}
	if (!inspection_request->primary_inspect_enable) {
		// AI 검사 조건 안 맞으면 기본 Vision 검사 수행
		auto inspect_avi_start = std::chrono::high_resolution_clock::now();
		if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
			HDefectRgn = pAlgorithm->InspectAVI(
				iCurInspectionBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
				HPreProcessImage,
				HROIHRegion,
				pInspectROIRgn->m_AlgorithmParam[iTabIdx],
				&HResultXLD,
				pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType],
				pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1],
				&HMaskRgn);
		else
			HDefectRgn = pAlgorithm->InspectAVI(
				iCurInspectionBufferIdx, iCurThreadIdx, pInspectROIRgn, iTabIdx,
				pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1],
				HROIHRegion,
				pInspectROIRgn->m_AlgorithmParam[iTabIdx],
				&HResultXLD,
				pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType],
				pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1],
				&HMaskRgn);
		auto inspect_avi_end = std::chrono::high_resolution_clock::now();
		auto inspect_avi_ms = std::chrono::duration_cast<std::chrono::milliseconds>(inspect_avi_start - inspect_avi_end).count();
		inspection_request->inspect_avi_ms = inspect_avi_ms;
		if (CGFunction::ValidHRegion(HDefectRgn)) {
			inspection_request->vision_defect_region = HDefectRgn.Clone();
		}
	}
	uint64_t submit_job_id;
	auto job = THEAPP.m_lg_inspectorManager->submit(inspection_request, submit_job_id);
	collector.lgaiResults.push_back(std::move(job));

#pragma region AI Image Save
	std::ostringstream file_name_stream;
	file_name_stream
		<< roiCtx.prefixString
		<< "_" << local_align_angle_string
		<< "_" << channel_type;
	std::string prefix = file_name_stream.str();

	if (THEAPP.Struct_PreferenceStruct.m_bSaveROIImage &&
		(THEAPP.Struct_PreferenceStruct.m_iSaveROIImageType == 0 ||
		 CGFunction::ValidHRegion(HDefectRgn)))
	{
		// 1) ROI 이미지 저장 enqueue
		for (const auto& dir : saveDirectories)
		{
			THEAPP.m_roi_image_save_thread_pool->enqueue_job_no_return(
				SaveROIImage, dir, prefix, ho_Cropped, ho_Cropped_v2);
		}
	}
#pragma endregion AI Image Save
#endif // LGITAI
}

void AlgorithmHelper::SubmitSYAI(
	ROIAlgorithmContext& roiCtx, ResultCollector& collector,
	const std::vector<std::string>& saveDirectories)
{
#ifdef SYAI
	const AlgorithmContext& algCtx = roiCtx.algCtx;
	Algorithm* pAlgorithm = m_pAlgorithm;
	int iCurInspectionBufferIdx = algCtx.iCurInspectionBufferIdx;
	GTRegion* pInspectROIRgn = roiCtx.pInspectROIRgn;
	int iTabIdx = roiCtx.iTabIdx;
	int iImageIdx = roiCtx.iImageIdx;
	int iROIIndex = roiCtx.iROIIndex;
	int iVisionCamType = algCtx.iVisionCamType;
	int iMzNo = algCtx.iMzNo;
	HObject& HROIHRegion = roiCtx.HROIHRegion;
	HObject& HDefectRgn = roiCtx.HDefectRgn;
	HTuple& length1 = roiCtx.length1;
	HTuple& length2 = roiCtx.length2;

	std::string eq_id = std::string(THEAPP.Struct_PreferenceStruct.m_strEquipNo);
	std::string lot_id = std::string(algCtx.sLotID);
	std::string tray_id = std::to_string(algCtx.iTrayNo);
	std::string barcode_id = THEAPP.m_strModuleBarcodeID[algCtx.iMzNo - 1][algCtx.iTrayNo - 1][algCtx.iModuleNo - 1];
	std::string local_align_angle_string = roiCtx.localAlignAngleString;
	std::string channel_type = roiCtx.channelType;

	std::string camera_type = CT2A(THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iVisionCamType]);
	std::string inspection_type = std::string(g_strInspectionTypeName_Short[pInspectROIRgn->miInspectionType - 1]);
	std::string defect_name = std::string(CT2A(THEAPP.m_strDefectName[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType]));
	std::string defect_type = std::string(CT2A(THEAPP.m_strDefectCode[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType]));

	// ROI 이미지 전처리 수행 여부 확인
	bool all_roi_inspection = roiCtx.bAllROIInspection;
	bool enable_roi_processing = roiCtx.bEnableROIProcessing;

	// ROI 이미지 전처리 비활성화 시 결함 영역 저장 후 이후 단계 패스
	if (!enable_roi_processing) {
		if (CGFunction::ValidHRegion(HDefectRgn))
		{
			ConcatObj(pAlgorithm->m_HDefectRgn_ImageNo[iCurInspectionBufferIdx][iImageIdx - 1], HDefectRgn, &(pAlgorithm->m_HDefectRgn_ImageNo[iCurInspectionBufferIdx][iImageIdx - 1]));
			ConcatObj(pAlgorithm->m_HDefectRgn_DefectName[iCurInspectionBufferIdx][pInspectROIRgn->miInspectionType - 1][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType],
					  HDefectRgn,
					  &(pAlgorithm->m_HDefectRgn_DefectName[iCurInspectionBufferIdx][pInspectROIRgn->miInspectionType - 1][pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iDefectType]));
		}
		THEAPP.m_syai_logger->debug("ROI Image Preprocessing is disabled. Skip ROI preprocessing. LotID: {}, Image Idx: {}, Roi Idx: {}, Tab Idx: {}", std::string(algCtx.sLotID), iImageIdx, iROIIndex, iTabIdx);
		return;
	}

	int roi_id = pInspectROIRgn->miInspectionROIID;
	int roi_idx = iROIIndex;
	HObject image = pAlgorithm->m_HInspectCImage[iCurInspectionBufferIdx][CHANNEL_TYPE_COLOR][iImageIdx - 1];
	HObject ho_Cropped, ho_Cropped_v2;
	cv::Mat cv_roi_img, cv_roi_img_v2;
	HalconCpp::HTuple raw_image_width, raw_image_height;
	HTuple crop_row1, crop_col1, crop_row2, crop_col2;
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	std::ostringstream file_name_stream;
	file_name_stream
		<< roiCtx.prefixString
		<< "_" << local_align_angle_string
		<< "_" << channel_type;
	std::string prefix = file_name_stream.str();

	syai_service::PreprocessingJobInput preprocessing_input;
	preprocessing_input.algorithm = pAlgorithm;
	preprocessing_input.gt_region = pInspectROIRgn;

	preprocessing_input.roi_reg = HROIHRegion;
	preprocessing_input.length1 = length1;
	preprocessing_input.length2 = length2;
	preprocessing_input.defect_reg = HDefectRgn;

	preprocessing_input.lot_id = std::string(algCtx.sLotID);
	preprocessing_input.eq_id = eq_id;
	preprocessing_input.tray_id = tray_id;
	preprocessing_input.barcode_id = barcode_id;
	preprocessing_input.prefix = roiCtx.prefixString;
	preprocessing_input.camera_type = camera_type;
	preprocessing_input.inspection_type = inspection_type;
	preprocessing_input.defect_name = defect_name;
	preprocessing_input.defect_type = defect_type;

	preprocessing_input.enable_roi_processing = enable_roi_processing;

	preprocessing_input.vision_cam_type = iVisionCamType;
	preprocessing_input.buffer_idx = iCurInspectionBufferIdx;
	preprocessing_input.magazine_no = iMzNo;
	preprocessing_input.image_idx = iImageIdx;
	preprocessing_input.roi_idx = iROIIndex;
	preprocessing_input.tab_idx = iTabIdx;

	preprocessing_input.save_prefix = prefix;
	preprocessing_input.enable_save_roi_image = THEAPP.Struct_PreferenceStruct.m_bSaveROIImage;
	preprocessing_input.save_roi_type = THEAPP.Struct_PreferenceStruct.m_iSaveROIImageType;
	preprocessing_input.save_dir_list = saveDirectories;

	preprocessing_input.enable_inspection_ai = roiCtx.bUseAIInspection;

	auto response = THEAPP.m_syai_service->add_job(preprocessing_input);
	collector.syaiResults.push_back(std::move(response));
#endif // SYAI
}

void AlgorithmHelper::CollectLGAIResults(const AlgorithmContext& algCtx, ResultCollector& collector)
{
#ifdef LGITAI
	const int vision_pos = algCtx.iVisionCamType;
	const int tray_no = algCtx.iTrayNo;
	const int magazine_no = algCtx.iMzNo;
	const int thread_id = algCtx.iCurInspectionBufferIdx;

	THEAPP.m_lg_inspector_logger->info(
		"[VP:{} TN:{} MZ:{} TH:{}] Task list 시작. task_count={}",
		vision_pos, tray_no, magazine_no, thread_id, collector.lgaiResults.size());

	for (auto it = collector.lgaiResults.begin(); it != collector.lgaiResults.end(); ) {
		try {
			LGIT::InspectionResultSet result = it->get();
			try {
				auto pROI = THEAPP.m_pDualModelDataManager[algCtx.iDualModelData][algCtx.iPcVisionNo]
					->m_pInspectionArea->GetChildTRegion(result.request->roi_idx);

				auto process_inspection_result = [&](const HObject& region, int inspect_type, int defect_type) {
					HObject restored_region;
					AIService::ImageUtile::RestoreRegionToOriginal(
						region, result.request->roi_bbox, restored_region);

					if (!CGFunction::ValidHRegion(restored_region))
						return;

					auto& algo = m_pAlgorithm;
					int threadBuf = result.request->vision_inspection_thread_index;
					int imgIdx = result.request->image_idx;

					CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
					bool isCHS = (strEquipModel == "CHS-W" || strEquipModel == "CHS-Z" ||
								  strEquipModel == "CHS-K" || strEquipModel == "CHS-S" ||
								  strEquipModel == "CHS-T" || strEquipModel == "CHS-V");

					if (isCHS && g_strInspectionTypeName[inspect_type] == "Lens")
					{
						bool isRing = (imgIdx == 12 || imgIdx == 14 || imgIdx == 16 ||
									   imgIdx == 17 || imgIdx == 19 || imgIdx == 21);
						bool isCoax = (imgIdx == 15 || imgIdx == 13 || imgIdx == 18 || imgIdx == 20);

						if (isRing) {
							ConcatObj(algo->m_HDefectRgn_Lens_Ring[threadBuf], restored_region,
									  &(algo->m_HDefectRgn_Lens_Ring[threadBuf]));
							if (defect_type == DEFECT_CODE_LENS_PARTICLE)
								ConcatObj(algo->m_HDefectRgn_MultiFocus_Ring_Particle[threadBuf][imgIdx - 1][inspect_type - 1][defect_type],
										  restored_region,
										  &(algo->m_HDefectRgn_MultiFocus_Ring_Particle[threadBuf][imgIdx - 1][inspect_type - 1][defect_type]));
							else {
								ConcatObj(algo->m_HDefectRgn_ImageNo[threadBuf][imgIdx - 1], restored_region,
										  &(algo->m_HDefectRgn_ImageNo[threadBuf][imgIdx - 1]));
								ConcatObj(algo->m_HDefectRgn_DefectName[threadBuf][inspect_type - 1][defect_type], restored_region,
										  &(algo->m_HDefectRgn_DefectName[threadBuf][inspect_type - 1][defect_type]));
							}
						}
						else if (isCoax) {
							ConcatObj(algo->m_HDefectRgn_ImageNo_Coax[threadBuf][imgIdx - 1], restored_region,
									  &(algo->m_HDefectRgn_ImageNo_Coax[threadBuf][imgIdx - 1]));
							ConcatObj(algo->m_HDefectRgn_DefectName_Coax[threadBuf][inspect_type - 1][defect_type], restored_region,
									  &(algo->m_HDefectRgn_DefectName_Coax[threadBuf][inspect_type - 1][defect_type]));
						}
						else {
							ConcatObj(algo->m_HDefectRgn_ImageNo[threadBuf][imgIdx - 1], restored_region,
									  &(algo->m_HDefectRgn_ImageNo[threadBuf][imgIdx - 1]));
							ConcatObj(algo->m_HDefectRgn_DefectName[threadBuf][inspect_type - 1][defect_type], restored_region,
									  &(algo->m_HDefectRgn_DefectName[threadBuf][inspect_type - 1][defect_type]));
						}
					}
					else {
						ConcatObj(algo->m_HDefectRgn_ImageNo[threadBuf][imgIdx - 1], restored_region,
								  &(algo->m_HDefectRgn_ImageNo[threadBuf][imgIdx - 1]));
						ConcatObj(algo->m_HDefectRgn_DefectName[threadBuf][inspect_type - 1][defect_type], restored_region,
								  &(algo->m_HDefectRgn_DefectName[threadBuf][inspect_type - 1][defect_type]));
					}
				};

				// 결과 우선순위: tertiary -> secondary -> svm -> primary
				auto processResultAndFeature = [&](const auto& stepResult, const std::string& stepName) {
					THEAPP.m_lg_inspector_logger->info("[VP:{} TN:{} MZ:{} TH:{}] {} 처리 시작",
													   vision_pos, tray_no, magazine_no, thread_id, stepName);

					using T = std::decay_t<decltype(*stepResult)>;

					if constexpr (std::is_same_v<T, LGIT::Model::LAIInspector3Result> ||
								  std::is_same_v<T, LGIT::Model::LAIInspector2Result>) {
						for (const auto& [type, mask] : stepResult->blob_mask_map) {
							if (!AIService::ImageUtile::HasImageData(mask)) continue;
							HObject region = AIService::ImageUtile::Mask2HRegion(mask);
							process_inspection_result(region, pROI->miInspectionType, type);
						}
					}
					else if constexpr (std::is_same_v<T, LGIT::Model::LAIInspectorSVMResult>) {
						cv::Mat theMask = stepResult->blob_ng_mask;
						if (AIService::ImageUtile::HasImageData(theMask)) {
							HObject region = AIService::ImageUtile::Mask2HRegion(theMask);
							process_inspection_result(region, pROI->miInspectionType,
													  pROI->m_AlgorithmParam[result.request->inspection_tab_idx].m_iDefectType);
						}
					}
					else {
						cv::Mat theMask = stepResult->mask;
						if (AIService::ImageUtile::HasImageData(theMask)) {
							HObject region = AIService::ImageUtile::Mask2HRegion(theMask);
							process_inspection_result(region, pROI->miInspectionType,
													  pROI->m_AlgorithmParam[result.request->inspection_tab_idx].m_iDefectType);
						}
					}

					// Variation Feature Log
					if (THEAPP.Struct_PreferenceStruct.m_bUseSaveDefectShapeFeature) {
						CString sModelName = THEAPP.m_pDualModelDataManager[algCtx.iDualModelData][VISION_NUMBER_1]->m_sModelName;
						SYSTEMTIME time;
						GetLocalTime(&time);
						std::shared_ptr<UniversalInfo> universalInfo;
						if (ConvertToUniversalInfo(result, universalInfo) && universalInfo)
						{
							// UniversalInfo 변환 성공 시 추가 처리 가능
						}

						THEAPP.m_variation_feature_log_thread_pool->enqueue_job_no_return(
							[=]() {
								THEAPP.NewSaveVariationFeature_Thread(
									result, sModelName, time, pROI, THEAPP.GetModelType(),
									THEAPP.m_FileBase.m_strResultLogFolder_Local[algCtx.iVisionCamType][algCtx.iMzNo - 1]);
							});
					}
					};

				if (result.tertiary_result && result.tertiary_result->result)
					processResultAndFeature(result.tertiary_result, "tertiary_result");
				else if (result.secondary_result && result.secondary_result->result)
					processResultAndFeature(result.secondary_result, "secondary_result");
				else if (result.svm_result && result.svm_result->result) {
					if (AIService::ImageUtile::HasImageData(result.svm_result->blob_ng_mask))
						processResultAndFeature(result.svm_result, "svm_result");
				}
				else if (result.primary_result && result.primary_result->result && !result.primary_result->mask.empty()) {
					if (AIService::ImageUtile::HasImageData(result.primary_result->mask))
						processResultAndFeature(result.primary_result, "primary_result");
				}

				result.request->release_images();
			}
			catch (const std::exception& e) {
				THEAPP.m_lg_inspector_logger->error("[VP:{} TN:{} MZ:{} TH:{}] 예외 발생: {}",
													vision_pos, tray_no, magazine_no, thread_id, e.what());
			}
			result.clear();
			it = collector.lgaiResults.erase(it);
		}
		catch (const std::exception& e) {
			THEAPP.m_lg_inspector_logger->error("[VP:{} TN:{} MZ:{} TH:{}] Get Error 발생: {}",
												vision_pos, tray_no, magazine_no, thread_id, e.what());
			++it;
		}
	}
	collector.lgaiResults.clear();
#endif // LGITAI
}

void AlgorithmHelper::CollectSYAIResults(ResultCollector& collector,
										 const std::string& localDir, const std::string& lasDir, bool isSameDir, bool saveLocal, bool saveLAS)
{
#ifdef SYAI
	CString strLog;

	// 1단계: Non-blocking 처리
	auto it = collector.syaiResults.begin();
	while (it != collector.syaiResults.end()) {
		if (it->wait_for(std::chrono::milliseconds(5)) == std::future_status::ready) {
			try {
				syai_service::JobInfo result = it->get();
				SaveStepResultMask(result, localDir, lasDir, isSameDir, saveLocal, saveLAS);
				ConcatDefectRegion(m_pAlgorithm, result);
				ConcatDefectResult(m_pAlgorithm, result);
				std::shared_ptr<UniversalInfo> info;
				if (ConvertToUniversalInfo(result, info)) {
					// UniversalInfo 변환 성공 시 추가 처리 가능
				}
				
				result.release();
				it = collector.syaiResults.erase(it);
			}
			catch (const std::exception& e) {
				strLog.Format("Error processing ready SYAI result: %s", e.what());
				THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
				it = collector.syaiResults.erase(it);
			}
		}
		else {
			++it;
		}
	}

	// 2단계: 남은 것들 동기 처리
	if (!collector.syaiResults.empty()) {
		strLog.Format("SYAI: %d results still pending, processing synchronously...",
					  static_cast<int>(collector.syaiResults.size()));
		THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

		for (auto& future_result : collector.syaiResults) {
			try {
				syai_service::JobInfo result = future_result.get();
				SaveStepResultMask(result, localDir, lasDir, isSameDir, saveLocal, saveLAS);
				ConcatDefectRegion(m_pAlgorithm, result);
				ConcatDefectResult(m_pAlgorithm, result);
				std::shared_ptr<UniversalInfo> info;
				if (ConvertToUniversalInfo(result, info)) {
					// UniversalInfo 변환 성공 시 추가 처리 가능
				}
				result.release();
			}
			catch (const std::exception& e) {
				strLog.Format("Error in delayed SYAI processing: %s", e.what());
				THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
			}
		}
	}

	collector.syaiResults.clear();
#endif // SYAI
}

void AlgorithmHelper::SaveROIImage(const std::string& save_dir,
								   const std::string& prefix,
								   HalconCpp::HObject h_image,
								   HalconCpp::HObject h_image_v2) {
	if (h_image.IsInitialized()) {
		bool can_save_image = true;
		std::ostringstream dir_stream;
		dir_stream << save_dir << "\\" << "ROIImage";
		std::filesystem::path dir_path(dir_stream.str());
		if (!std::filesystem::exists(dir_path)) {
			if (!std::filesystem::create_directories(dir_path)) {
				can_save_image = false;
			}
		}
		if (can_save_image) {
			AIService::ImageUtile::SaveImage(dir_path.string(), prefix, h_image);
		}
	}
	if (h_image_v2.IsInitialized()) {
		bool can_save_image = true;
		std::ostringstream dir_stream;
		dir_stream << save_dir << "\\" << "ROIImage";
		std::filesystem::path dir_path(dir_stream.str());
		if (!std::filesystem::exists(dir_path)) {
			if (!std::filesystem::create_directories(dir_path)) {
				can_save_image = false;
			}
		}
		if (can_save_image) {
			AIService::ImageUtile::SaveImage(dir_path.string(), prefix + "_V2", h_image_v2);
		}
	}
}

void AlgorithmHelper::SaveDefectCropImage(const std::string& save_dir,
										  const cv::Size& patch_size,
										  int stride,
										  const std::string& prefix,
										  HalconCpp::HObject h_image,
										  const std::string& barcode_id,
										  const std::string& inspection_type_name) {
	// 현재 시간을 가져옴
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);

	// std::tm 구조체로 변환
	std::tm local_time;
#if defined(_MSC_VER) || defined(__MINGW32__) // MSVC 또는 MinGW를 사용하는 경우
	localtime_s(&local_time, &now_time);
#else // 다른 표준 C++ 컴파일러의 경우
	localtime_r(&now_time, &local_time);
#endif

	if (h_image.IsInitialized()) {
		bool can_save_image = true;
		std::ostringstream dir_stream;
		dir_stream << save_dir << "\\" << "ReqCropImage" << "\\" << barcode_id << "\\" << inspection_type_name;
		std::filesystem::path dir_path(dir_stream.str());
		if (!std::filesystem::exists(dir_path)) {
			if (!std::filesystem::create_directories(dir_path)) {
				can_save_image = false;
			}
		}
	}
}

////////////////////////////////////////////////////////
// SYAI 보관함
////////////////////////////////////////////////////////
#ifdef SYAI
void AlgorithmHelper::SaveSyaiResultDebugInfo(const syai_service::JobInfo& result, const std::string& prefix = "")
{
	CString strLog;

	try {
		std::filesystem::path debug_dir = "C:\\Temp\\debug\\result\\";
		std::filesystem::create_directories(debug_dir);

		auto now = std::chrono::system_clock::now();
		auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
			now.time_since_epoch()).count();

		std::string base_filename = prefix + "syai_result_" + std::to_string(timestamp);

		// Request 정보 저장
		if (result.request.has_value()) {
			const auto& req = result.request.value();

			std::string request_info_file = (debug_dir / (base_filename + "_request_info.txt")).string();
			std::ofstream request_info(request_info_file);

			if (request_info.is_open()) {
				request_info << "=== SYAI Request Information ===" << std::endl;
				request_info << "Inspection ID: " << req.get_inspection_id() << std::endl;
				request_info << "Log ID: " << req.get_log_id() << std::endl;

				// 이미지 저장
				cv::Mat request_image = req.get_image();
				if (!request_image.empty()) {
					std::string image_file = (debug_dir / (base_filename + "_request_image.png")).string();
					cv::imwrite(image_file, request_image);
					request_info << "Image Size: " << request_image.cols << "x"
						<< request_image.rows << std::endl;
				}

				// 마스크 저장 (const 버전 사용)
				if (!req.get_mask().empty()) {
					std::string mask_file = (debug_dir / (base_filename + "_request_mask.png")).string();
					cv::imwrite(mask_file, req.get_mask());
				}

				// ROI 정보
				cv::Rect request_area = req.get_area();
				if (request_area.width > 0 && request_area.height > 0) {
					request_info << "ROI Area: (" << request_area.x << ", " << request_area.y
						<< ", " << request_area.width << ", " << request_area.height << ")" << std::endl;
				}

				request_info.close();
			}
		}

		// Response 정보 저장
		if (result.response.has_value()) {
			const auto& resp = result.response.value();

			std::string response_info_file = (debug_dir / (base_filename + "_response_info.txt")).string();
			std::ofstream response_info(response_info_file);

			if (response_info.is_open()) {
				response_info << "=== SYAI Response Information ===" << std::endl;
				response_info << "Inspection ID: " << resp.get_inspection_id() << std::endl;
				response_info << "Overall Success: " << (resp.get_overall_success() ? "TRUE" : "FALSE") << std::endl;

				const auto& step_list = resp.get_step_names();
				for (int i = 0; i < step_list.size(); ++i) {
					const auto setp_name = step_list.get(i);
					const auto& step_result_opt = resp.get_step_result(setp_name);
					if (!step_result_opt.success) {
						continue;
					}
					std::filesystem::path step_mask_dir = debug_dir / setp_name;
					if (!std::filesystem::exists(step_mask_dir)) {
						if (!std::filesystem::create_directories(step_mask_dir)) {
							continue;
						}
					}

					std::filesystem::path step_mask_file = step_mask_dir / (base_filename + "_response_mask.png");
					cv::Mat step_mask = step_result_opt.mask;
					if (!step_mask.empty()) {
						cv::imwrite(step_mask_file.string(), step_mask);
					}
				}
				response_info.close();
			}
		}

		strLog.Format("SYAI result debug files saved: %s", base_filename.c_str());
		THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
	}
	catch (const std::exception& e) {
		strLog.Format("Failed to save SYAI result debug files: %s", e.what());
		THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
	}
}

void AlgorithmHelper::SaveResultMask(
	std::string local_dir,
	std::string las_dir,
	std::string file_name,
	bool is_same_dir,
	bool save_local,
	bool save_las,
	const cv::Mat& mask)
{
	if (mask.empty()) {
		return;
	}
	if (is_same_dir) {
		if (save_local || save_las) {
			if (!std::filesystem::exists(local_dir)) {
				if (!std::filesystem::create_directories(local_dir)) {
					THEAPP.m_syai_logger->error("[MAIN] [SaveStepResultDebugInfo]Failed to create directory: {}", local_dir);
					return;
				}
			}
			std::filesystem::path save_path = std::filesystem::path(local_dir) / file_name;
			cv::imwrite(save_path.string(), mask);
		}
	}
	else {
		if (save_local) {
			if (!std::filesystem::exists(local_dir)) {
				if (!std::filesystem::create_directories(local_dir)) {
					THEAPP.m_syai_logger->error("[MAIN] [SaveStepResultDebugInfo]Failed to create directory: {}", local_dir);
					return;
				}
			}
			std::filesystem::path save_path = std::filesystem::path(local_dir) / file_name;
			cv::imwrite(save_path.string(), mask);
		}
		if (save_las) {
			if (!std::filesystem::exists(las_dir)) {
				if (!std::filesystem::create_directories(las_dir)) {
					THEAPP.m_syai_logger->error("[MAIN] [SaveStepResultDebugInfo]Failed to create directory: {}", las_dir);
					return;
				}
			}
			std::filesystem::path save_path = std::filesystem::path(las_dir) / file_name;
			cv::imwrite(save_path.string(), mask);
		}
	}
}

void AlgorithmHelper::SaveStepResultMask(
	const syai_service::JobInfo& result,
	const std::string& local_dir,
	const std::string& las_dir,
	bool is_same_dir,
	bool save_local,
	bool save_las) {
	if (!result.request.has_value()) {
		THEAPP.m_syai_logger->error("[MAIN] [SaveStepResultMask] Invalid JobInfo: missing request");
		return;
	}
	if (!result.response.has_value()) {
		THEAPP.m_syai_logger->error("[MAIN] [SaveStepResultMask] Invalid JobInfo: missing response");
		return;
	}
	if (!result.addon.has_value()) {
		THEAPP.m_syai_logger->error("[MAIN] [SaveStepResultMask] Invalid JobInfo: missing addon");
		return;
	}
	// 마스크 파일명 생성
	std::string mask_file_name;
	if (!result.addon->file_name_prefix.empty()) {
		mask_file_name = result.addon->file_name_prefix + "_mask.png";
	}
	else {
		mask_file_name = "syai_result_" + std::string(result.request->get_log_id()) + "_mask.png";
	}

	const auto& req = result.request.value();
	const auto& resp = result.response.value();
	const auto& step_list = resp.get_step_names();

	// 요청 마스크 저장
	std::string req_mask_dir_name = "MaskImage0";
	std::string local_req_dir, las_req_dir;

	if (save_local) {
		if (is_same_dir) {
			local_req_dir = local_dir;
		}
		else {
			local_req_dir = (std::filesystem::path(local_dir) / req_mask_dir_name).string();
		}
	}

	if (save_las && !is_same_dir) {
		las_req_dir = (std::filesystem::path(las_dir) / req_mask_dir_name).string();
	}

	// 요청 마스크 이미지 스레드풀에 작업 추가
	try {
		cv::Mat mask = req.get_mask().clone();
		THEAPP.m_roi_image_save_thread_pool->enqueue_job_no_return(
			SaveResultMask,
			local_req_dir,
			las_req_dir,
			mask_file_name,
			is_same_dir,
			save_local,
			save_las,
			mask);
	}
	catch (...) {

	}

	// 각 스텝별로 마스크 저장
	for (int i = 0; i < step_list.size(); ++i) {
		const auto step_name = step_list.get(i);
		const auto& step_result_opt = resp.get_step_result(step_name);

		if (!step_result_opt.success || step_result_opt.mask.empty()) {
			continue;
		}

		// 경로 최적화: 필요한 경우에만 생성
		std::string local_step_dir, las_step_dir;
		std::string mask_dir_name;
		std::string step_name_str = step_name;
		if (step_name_str.find("CLASSIFICATION1") != std::string::npos) { mask_dir_name = "MaskImage1"; }
		else if (step_name_str.find("CLASSIFICATION2") != std::string::npos) { mask_dir_name = "MaskImage2"; }
		else if (step_name_str.find("SEGMENTATION1") != std::string::npos) { mask_dir_name = "MaskImage3"; }
		else { mask_dir_name = "MaskImage_" + step_name_str; }

		if (save_local) {
			if (is_same_dir) {
				local_step_dir = local_dir;
			}
			else {
				local_step_dir = (std::filesystem::path(local_dir) / mask_dir_name).string();
			}
		}

		if (save_las && !is_same_dir) {
			las_step_dir = (std::filesystem::path(las_dir) / mask_dir_name).string();
		}

		// Step별 마스크 이미지 스레드풀에 작업 추가
		THEAPP.m_roi_image_save_thread_pool->enqueue_job_no_return(
			SaveResultMask,
			local_step_dir,
			las_step_dir,
			mask_file_name,
			is_same_dir,
			save_local,
			save_las,
			step_result_opt.mask.clone()
		);
	}

	// 최종 검사 단계 결과 마스크 저장
	std::string local_step_dir, las_step_dir;

	if (save_local) {
		if (is_same_dir) {
			local_step_dir = local_dir;
		}
		else {
			local_step_dir = (std::filesystem::path(local_dir) / "MaskImageFinal").string();
		}
	}

	if (save_las && !is_same_dir) {
		las_step_dir = (std::filesystem::path(las_dir) / "MaskFinal").string();
	}

	auto last_step_mask = result.get_last_step_mask();
	if (last_step_mask.empty()) {
		THEAPP.m_syai_logger->error("[MAIN] [SaveStepResultMask] No valid last step mask to save.");
		return;
	}
	else {
		THEAPP.m_syai_logger->info("[MAIN] [SaveStepResultMask] Saving last step mask.");
		// 스레드풀에 작업 추가
		THEAPP.m_roi_image_save_thread_pool->enqueue_job_no_return(
			SaveResultMask,
			local_step_dir,
			las_step_dir,
			mask_file_name,
			is_same_dir,
			save_local,
			save_las,
			std::move(last_step_mask)
		);
	}
}

void AlgorithmHelper::ConcatDefectRegionVision(Algorithm* algo, const syai_service::JobInfo& result)
{
	THEAPP.m_syai_logger->debug("[MAIN] [ConcatDefectRegionVision] Start concatenating defect regions for all visions.");
	if (!result.request || !result.request.has_value()) {
		THEAPP.m_syai_logger->error("[MAIN] [ConcatDefectRegion] Invalid JobInfo: missing request");
		return;
	}
	const auto& request = result.request.value();
	HalconCpp::HObject defect_region;
	try {
		defect_region = AIService::ImageUtile::Mask2HRegion(request.get_mask());
	}
	catch (const std::exception& e) {
		THEAPP.m_syai_logger->error("[MAIN] [ConcatDefectRegion] Exception converting mask to HRegion: {}", e.what());
		return;
	}
	HObject restored_region;
	try {
		AIService::ImageUtile::RestoreRegionToOriginal(defect_region, result.request->get_area(), restored_region);
	}
	catch (const std::exception& e) {
		THEAPP.m_syai_logger->error("[MAIN] [ConcatDefectRegion] Exception restoring region to original: {}", e.what());
		return;
	}
	if (!CGFunction::ValidHRegion(restored_region)) {
		THEAPP.m_syai_logger->info("[MAIN] [ConcatDefectRegion] Restored region is invalid.");
		return;
	}
	if (CGFunction::ValidHRegion(restored_region)) {
		int thread_buffer = result.addon->vision_inspection_thread_number;
		int mz_number = algo->m_nMzNo[thread_buffer];
		int vision_number = algo->m_iPcVisionNo;
		int dual_model_data = THEAPP.g_iDualModelData[mz_number - 1];

		int image_index = result.addon->image_index;
		int roi_index = result.addon->roi_index;
		int inspection_tab_index = result.addon->inspection_tab_index;

		auto inspection_roi_rgn = THEAPP.m_pDualModelDataManager[dual_model_data][vision_number]->m_pInspectionArea->GetChildTRegion(roi_index);

		int inspect_type_index = inspection_roi_rgn->miInspectionType - 1;
		int defect_type = inspection_roi_rgn->m_AlgorithmParam[inspection_tab_index].m_iDefectType;

		// 유효한 결함 영역인 경우 처리
		ConcatObj(algo->m_HDefectRgn_ImageNo[thread_buffer][image_index], restored_region, &(algo->m_HDefectRgn_ImageNo[thread_buffer][image_index]));
		ConcatObj(algo->m_HDefectRgn_DefectName[thread_buffer][inspect_type_index][defect_type], restored_region, &(algo->m_HDefectRgn_DefectName[thread_buffer][inspect_type_index][defect_type]));
	}
}


void AlgorithmHelper::ConcatDefectRegion(Algorithm* algo, const syai_service::JobInfo& result)
{
	THEAPP.m_syai_logger->info("[MAIN] [ConcatDefectRegion] Start concatenating defect regions.");
	if (!result.request.has_value()) {
		THEAPP.m_syai_logger->error("[MAIN] [ConcatDefectRegion] Invalid JobInfo: missing request");
		return;
	}
	if (!result.addon.has_value()) {
		THEAPP.m_syai_logger->error("[MAIN] [ConcatDefectRegion] Invalid JobInfo: missing addon");
		return;
	}
	if (!result.response.has_value()) {
		THEAPP.m_syai_logger->error("[MAIN] [ConcatDefectRegion] Invalid JobInfo: missing response");
		ConcatDefectRegionVision(algo, result);
		return;
	}
	cv::Mat mask = result.get_last_step_mask();
	if (mask.empty()) {
		THEAPP.m_syai_logger->info("[MAIN] [ConcatDefectRegion] No defect regions found in the last step mask.");
		ConcatDefectRegionVision(algo, result);
		return;
	}
	if (!AIService::ImageUtile::HasImageData(mask)) {
		THEAPP.m_syai_logger->info("[MAIN] [ConcatDefectRegion] Last step mask has no image data.");
		return;
	}
	HObject region = AIService::ImageUtile::Mask2HRegion(mask);
	if (!CGFunction::ValidHRegion(region)) {
		THEAPP.m_syai_logger->info("[MAIN] [ConcatDefectRegion] Converted HRegion is invalid.");
		ConcatDefectRegionVision(algo, result);
		return;
	}
	HObject restored_region;
	AIService::ImageUtile::RestoreRegionToOriginal(region, result.request->get_area(), restored_region);
	if (!CGFunction::ValidHRegion(restored_region)) {
		THEAPP.m_syai_logger->info("[MAIN] [ConcatDefectRegion] Restored region is invalid.");
		return;
	}
	THEAPP.m_syai_logger->info("[MAIN] [ConcatDefectRegion] Defect regions concatenated successfully.");
	int thread_buffer = result.addon->vision_inspection_thread_number;
	int mz_number = algo->m_nMzNo[thread_buffer];
	int vision_number = algo->m_iPcVisionNo;
	int dual_model_data = THEAPP.g_iDualModelData[mz_number - 1];

	int image_index = result.addon->image_index;
	int roi_index = result.addon->roi_index;
	int inspection_tab_index = result.addon->inspection_tab_index;

	auto inspection_roi_rgn = THEAPP.m_pDualModelDataManager[dual_model_data][vision_number]->m_pInspectionArea->GetChildTRegion(roi_index);

	int inspect_type_index = inspection_roi_rgn->miInspectionType - 1;
	int defect_type = inspection_roi_rgn->m_AlgorithmParam[inspection_tab_index].m_iDefectType;

	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
	if ((strEquipModel == "CHS-W" || strEquipModel == "CHS-Z" || strEquipModel == "CHS-K" || strEquipModel == "CHS-S" || strEquipModel == "CHS-T" || strEquipModel == "CHS-V") &&
		g_strInspectionTypeName[inspection_roi_rgn->miInspectionType] == "Lens") // && Multi-Focus 처리 옵션 추가 필요
	{
		// Ring 조명, Coax 조명을 사용하는 이미지 번호들 옵션 추가 필요
		if (image_index == 12 || image_index == 14 || image_index == 16 ||
			image_index == 17 || image_index == 19 || image_index == 21)	// 렌즈 이미지 중 Ring 조명을 사용하는 모든 이미지
		{
			ConcatObj(algo->m_HDefectRgn_Lens_Ring[thread_buffer], restored_region, &(algo->m_HDefectRgn_Lens_Ring[thread_buffer]));
			if (defect_type == DEFECT_CODE_LENS_PARTICLE &&
				(image_index == 12 || image_index == 14 || image_index == 16 ||
				 image_index == 17 || image_index == 19 || image_index == 21)) // && Multi-Focus 처리를 진행할 모든 이미지
				ConcatObj(algo->m_HDefectRgn_MultiFocus_Ring_Particle[thread_buffer][image_index][inspect_type_index][defect_type], restored_region, &(algo->m_HDefectRgn_MultiFocus_Ring_Particle[thread_buffer][image_index][inspect_type_index][defect_type]));
			else
			{
				ConcatObj(algo->m_HDefectRgn_ImageNo[thread_buffer][image_index], restored_region, &(algo->m_HDefectRgn_ImageNo[thread_buffer][image_index]));
				ConcatObj(algo->m_HDefectRgn_DefectName[thread_buffer][inspect_type_index][defect_type], restored_region, &(algo->m_HDefectRgn_DefectName[thread_buffer][inspect_type_index][defect_type]));
			}
		}
		else if (image_index == 15 || image_index == 13 || image_index == 18 || image_index == 20)	// 렌즈 이미지 중 Coaxial 조명을 사용하는 모든 이미지
		{
			ConcatObj(algo->m_HDefectRgn_ImageNo_Coax[thread_buffer][image_index], restored_region, &(algo->m_HDefectRgn_ImageNo_Coax[thread_buffer][image_index]));
			ConcatObj(algo->m_HDefectRgn_DefectName_Coax[thread_buffer][inspect_type_index][defect_type], restored_region, &(algo->m_HDefectRgn_DefectName_Coax[thread_buffer][inspect_type_index][defect_type]));
		}
		else
		{
			ConcatObj(algo->m_HDefectRgn_ImageNo[thread_buffer][image_index], restored_region, &(algo->m_HDefectRgn_ImageNo[thread_buffer][image_index]));
			ConcatObj(algo->m_HDefectRgn_DefectName[thread_buffer][inspect_type_index][defect_type], restored_region, &(algo->m_HDefectRgn_DefectName[thread_buffer][inspect_type_index][defect_type]));
		}
	}
	else
	{
		ConcatObj(algo->m_HDefectRgn_ImageNo[thread_buffer][image_index], restored_region, &(algo->m_HDefectRgn_ImageNo[thread_buffer][image_index]));
		ConcatObj(algo->m_HDefectRgn_DefectName[thread_buffer][inspect_type_index][defect_type], restored_region, &(algo->m_HDefectRgn_DefectName[thread_buffer][inspect_type_index][defect_type]));
	}
}

void AlgorithmHelper::ConcatDefectResult(Algorithm* algo, const syai_service::JobInfo& result)
{
	// 1. 필수 데이터 유효성 검사
	if (!result.request.has_value() ||
		!result.response.has_value() ||
		!result.addon.has_value()) {
		THEAPP.m_syai_logger->warn("[MAIN] [ConcatDefectResult] Invalid JobInfo: missing required data");
		return;
	}

	const auto& resp = result.response.value();
	const auto& addon = result.addon.value();
	const auto& step_list = resp.get_step_names();

	// 2. 공통 데이터 추출
	int thread_buffer = addon.vision_inspection_thread_number;
	int vision_cam_type = addon.vision_cam_type;

	int mz_no = algo->m_nMzNo[thread_buffer];
	int tray_no = algo->m_nTrayNo[thread_buffer];
	int module_no = algo->m_nModuleNo[thread_buffer];

	// 3. 매핑
	CString* defect_result_arrays[] = {
		&THEAPP.m_sAI1DefectResult[mz_no - 1][tray_no - 1][module_no - 1][vision_cam_type],
		&THEAPP.m_sAI2DefectResult[mz_no - 1][tray_no - 1][module_no - 1][vision_cam_type],
		&THEAPP.m_sAI3DefectResult[mz_no - 1][tray_no - 1][module_no - 1][vision_cam_type]
	};
	constexpr int MAX_AI_STEPS = 3;

	// 4. 각 스텝별 결과 처리
	int step_count = static_cast<int>(step_list.size());
	for (int i = 0; i < step_count && i < MAX_AI_STEPS; ++i) {
		const auto step_name = step_list.get(i);
		const auto& step_result = resp.get_step_result(step_name);

		if (!step_result.success || step_result.mask.data == nullptr || step_result.mask.empty()) {
			continue;
		}
		if (step_result.mask.channels() == 1) { // 단일
			if (cv::countNonZero(step_result.mask) > 0) {
				*defect_result_arrays[i] = _T("N"); // 유효한 결과가 있으면 "N" (NG) 기록
			}
			else {
				*defect_result_arrays[i] = _T("G"); // 유효한 결과가 없으면 "G" (Good) 기록
			}
		}
		else {
			// 다채널 변환
			cv::Mat gray;
			cv::cvtColor(step_result.mask, gray, cv::COLOR_BGR2GRAY);
			if (cv::countNonZero(gray) > 0) {
				*defect_result_arrays[i] = _T("N"); // 유효한 결과가 있으면 "N" (NG) 기록
			}
			else {
				*defect_result_arrays[i] = _T("G"); // 유효한 결과가 없으면 "G" (Good) 기록
			}
		}
	}
}

void AlgorithmHelper::SaveAIImage(
	const std::string& save_dir,
	const cv::Mat& image,
	const cv::Mat& image_v2,
	const cv::Size& patch_size,
	int stride,
	const std::string& prefix,
	const std::string& barcode_id,
	const std::string& inspection_type_name,
	HalconCpp::HObject h_mask_reg,
	cv::Size orignal_image_size,
	cv::Size roi_image_size,
	cv::Point roi_left_top,
	cv::Point roi_right_bottom,
	bool save_train_image = false)
{
	auto mask_image = AIService::ImageUtile::RegionToLabeledImageV2(h_mask_reg,
																	orignal_image_size,
																	roi_image_size,
																	roi_left_top,
																	roi_right_bottom);
	if (!image.empty() && save_train_image && mask_image.has_value()) {
		bool can_save_image = true;
		std::ostringstream dir_stream;
		dir_stream << save_dir << "\\" << "CropImage" << "\\" << barcode_id << "\\" << inspection_type_name;
		std::filesystem::path dir_path(dir_stream.str());
		if (!std::filesystem::exists(dir_path)) {
			if (!std::filesystem::create_directories(dir_path)) {
				can_save_image = false;
			}
		}
		if (can_save_image) {
			//THEAPP.m_syai_service->save_train_image_save(dir_path.string(), image, patch_size, stride, prefix);
			THEAPP.m_syai_service->save_train_image_save_with_mask(dir_path.string(), image, mask_image.value(), patch_size, stride, prefix);
		}
		if (!image_v2.empty() && save_train_image && mask_image.has_value()) {
			bool can_save_image = true;
			std::ostringstream dir_stream;
			dir_stream << save_dir << "\\" << "CropImageV2" << "\\" << barcode_id << "\\" << inspection_type_name;
			std::filesystem::path dir_path(dir_stream.str());
			if (!std::filesystem::exists(dir_path)) {
				if (!std::filesystem::create_directories(dir_path)) {
					can_save_image = false;
				}
			}
			if (can_save_image) {
				//THEAPP.m_syai_service->save_train_image_save(dir_path.string(), image_v2, patch_size, stride, prefix);
				THEAPP.m_syai_service->save_train_image_save_with_mask(dir_path.string(), image_v2, mask_image.value(), patch_size, stride, prefix);
			}
		}
	}
}

bool AlgorithmHelper::ConvertToUniversalInfo(const syai_service::JobInfo& info, std::shared_ptr<UniversalInfo>& outInfo)
{
	if (!info.addon.has_value()) {
		THEAPP.m_syai_logger->warn("[MAIN] [ConvertToUniversalInfo] Invalid JobInfo: missing addon");
		return false;
	}
	if (!info.request.has_value()) {
		THEAPP.m_syai_logger->warn("[MAIN] [ConvertToUniversalInfo] Invalid JobInfo: missing request");
		return false;
	}
	if (!info.response.has_value()) {
		THEAPP.m_syai_logger->warn("[MAIN] [ConvertToUniversalInfo] Invalid JobInfo: missing response");
		return false;
	}
	LoggingInfo logInfo;
	logInfo.equip_no = info.addon->eq_id;
	logInfo.lot_id = info.addon->lot_id;
	logInfo.barcode = info.addon->barcode_id;

	SearchInfo searchInfo;
	searchInfo.buffer_idx = info.addon->vision_inspection_thread_number;
	searchInfo.vision_camera_type = info.addon->vision_cam_type;
	searchInfo.magazine_no = info.addon->magazine_no;
	searchInfo.image_idx = info.addon->image_index;
	searchInfo.roi_idx = info.addon->roi_index;
	searchInfo.tab_idx = info.addon->inspection_tab_index;

	PrimaryInspectionInfo primaryInfo;
	primaryInfo.result = true;
	primaryInfo.defect_mask = info.request->get_mask().clone();

	SVMInfo svmInfo;
	svmInfo.result = false;

	ClassificationInfo clsInfo;
	if (info.response)


	outInfo = std::make_shared<UniversalInfo>();
	outInfo->logging_info = std::move(logInfo);
	return false;
}

#endif // SYAI

bool AlgorithmHelper::ConvertToUniversalInfo(const LGIT::InspectionResultSet& info, std::shared_ptr<UniversalInfo>& outInfo)
{
	LoggingInfo logInfo;
	logInfo.model_name = "";
	logInfo.ai_model_name = "";
	logInfo.equip_no = info.request->equip_no;
	logInfo.lot_id = info.request->lot_id;
	logInfo.barcode = info.request->barcode;

	SearchInfo searchInfo;
	searchInfo.buffer_idx = info.request->vision_inspection_thread_index;
	searchInfo.vision_camera_type = info.request->vision_cam;
	searchInfo.magazine_no = info.request->magazine_no;
	searchInfo.image_idx = info.request->image_idx;
	searchInfo.roi_idx = info.request->roi_idx;
	searchInfo.tab_idx = info.request->inspection_tab_idx;

	PrimaryInspectionInfo primaryInfo;
	if (info.request->primary_inspect_enable && info.primary_result && info.primary_result->result) {
		primaryInfo.result = true;
		primaryInfo.defect_mask = info.primary_result->mask.clone();
	}

	SVMInfo svmInfo;
	if (info.request->svm_inspect_enable && info.svm_result) {
		svmInfo.result = true;
		svmInfo.ng_mask = info.svm_result->blob_ng_mask.clone();
	}

	ClassificationInfo clsInfo;
	if (info.request->secondary_inspect_enable && info.secondary_result) {
		clsInfo.result = true;
		clsInfo.blob_count = info.secondary_result->blob_count;
		clsInfo.score_list = info.secondary_result->score_list;
		clsInfo.ok_mask = info.secondary_result->blob_ok_mask.clone();
		for (const auto& [blob_id, blob_mask] : info.secondary_result->blob_mask_map) {
			clsInfo.ng_mask_map[blob_id] = blob_mask.clone();
		}
	}

	SegmentationInfo segInfo;
	if (info.request->tertiary_inspect_enable && info.tertiary_result) {
		segInfo.result = true;
		for (const auto& [defect_type_idx, defect_mask] : info.tertiary_result->blob_mask_map) {
			segInfo.mask_map[defect_type_idx] = defect_mask.clone();
		}
	}

	InspectionInfo inspInfo;
	inspInfo.primary_info = std::move(primaryInfo);
	inspInfo.svm_info = std::move(svmInfo);
	inspInfo.cls_info = std::move(clsInfo);
	inspInfo.seg_info = std::move(segInfo);

	outInfo = std::make_shared<UniversalInfo>();
	outInfo->logging_info = std::move(logInfo);
	outInfo->search_info = std::move(searchInfo);
	outInfo->inspection_info = std::move(inspInfo);
	return true;
}