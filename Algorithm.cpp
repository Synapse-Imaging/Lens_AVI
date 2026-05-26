#include "stdafx.h"
#include "uScan.h"
#include "math.h"
#include "Algorithm.h"
#include "BOIFAIInspector.h"
#include "ACTFAIInspector.h"
#include "AlgorithmService\AlgorithmPipeline.h"
#include "AlgorithmService\AlgorithmThread.h"
#include "Variation\VariationNormalizer.h"

Algorithm* Algorithm::m_pInstance = NULL;

Algorithm* Algorithm::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new Algorithm();
	}
	return m_pInstance;
}

void Algorithm::DeleteInstance()
{
	SAFE_DELETE(m_pInstance);
}

Algorithm::Algorithm(void)
{
	int i, j, k, l;

	for (i = 0; i < NO_MAX_INSPECT_THREAD; i++)
		m_bAlgorithmThreadRunning[i] = FALSE;

	m_bResultSaveThreadRunning = FALSE;

	m_pSemaphore = NULL;

	m_iPcVisionNo = VISION_NUMBER_1;
	m_iVisionCamType = VISION_NUMBER_1;
	m_iNoInspectImage = 0;
	m_iPrevTrayNo = 0;
	m_sPrevLotID = (_T(""));

	m_iScanBufferIdx = 0;

	for (i = 0; i < INSPECTION_BUFFER_COUNT_MAX; i++)
	{
		for (j = 0; j < CHANNEL_NUM; j++)
		{
			for (k = 0; k < MAX_IMAGE_TAB; k++)
				GenEmptyObj(&(m_HInspectCImage[i][j][k]));
		}

		for (j = 0; j < DFM_VISION_MAX_CONVERSION; j++)
		{
			GenEmptyObj(&(m_HInspectSpecularBackgroundImage[i][j]));

			for (k = 0; k < SPV_RAW_IMAGE_NUMBER; k++)
				GenEmptyObj(&(m_HInspectSpecularRawImage[i][j][k]));
		}

		for (j = 0; j < DIFFUSED_VISION_MAX_CONVERSION; j++)
		{
			GenEmptyObj(&(m_HInspectDiffusedBackgroundImage[i][j]));

			for (k = 0; k < DIFFUSED_RAW_IMAGE_NUMBER; k++)
				GenEmptyObj(&(m_HInspectDiffusedRawImage[i][j][k]));
		}

		m_sLotID[i] = _T("");
		m_nMzNo[i] = -1;
		m_nJigNo[i] = -1;
		m_sTrayID[i] = _T("");
		m_nTrayNo[i] = -1;
		m_nModuleNo[i] = -1;
		m_bGrabFail[i] = FALSE;
		m_iInspectBufferStatus[i] = BUFFER_STATUS_AVAILABLE;
		m_bMatchingSuccess[i] = FALSE;
		m_bLightDisorder[i] = FALSE;
		m_iNoGrabRetry[i] = 0;
		m_iNoFocusMoveRetry[i] = 0;

		GenEmptyObj(&(m_HDefectRgn_Lens_Ring[i]));
		for (j = 0; j < MAX_IMAGE_TAB; j++)
		{
			GenEmptyObj(&(m_HDefectRgn_ImageNo[i][j]));
			GenEmptyObj(&(m_HDefectRgn_ImageNo_Coax[i][j]));

			m_dGlobalAlignDeltaX[i][j] = 0;
			m_dGlobalAlignDeltaX[i][j] = 0;

			for (k = 0; k < MAX_INSPECTION_TYPE; k++)
			{
				GenEmptyObj(&(m_HDefectRgn_InspectionType[i][k]));

				for (l = 0; l < MAX_DEFECT_NAME; l++)
				{
					GenEmptyObj(&(m_HDefectRgn_DefectName[i][k][l]));
					GenEmptyObj(&(m_HDefectRgn_MultiFocus_Ring_Particle[i][j][k][l]));
					GenEmptyObj(&(m_HDefectRgn_DefectName_Coax[i][k][l]));
				}
			}
		}

		GenEmptyObj(&(m_HDefectRgn_FAI[i]));

		for (j = 0; j < MAX_FAI_ITEM; j++)
		{
			for (k = 0; k < MAX_FAI_PARAMETER; k++)
			{
				GenEmptyObj(&(m_HMeasureRgn_FAI_Item[i][j][k]));

				for (l = 0; l < MAX_FAI_REVIEW_IMAGE; l++)
				{
					GenEmptyObj(&(m_HReviewXLD_FAI_Item[i][j][k][l]));
					m_iViewportCenter_FAI_Item[i][j][k][l].x = -1;
					m_iViewportCenter_FAI_Item[i][j][k][l].y = -1;
				}
			}
		}

		m_iAlignNoInspectModule[i] = 0;
		m_iAlignNoInspectEmpty[i] = 0;
		GenEmptyObj(m_HAlignResultRgn + i);
		TupleGenConst(0, 0, m_HAlignModuleExist + i);
		TupleGenConst(0, 0, m_HAlignModuleIsDefect + i);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosX + i);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosY + i);
		TupleGenConst(0, 0, m_HAlignModuleDeltaX + i);
		TupleGenConst(0, 0, m_HAlignModuleDeltaY + i);
		TupleGenConst(0, 0, m_HAlignModuleDeltaAngle + i);
	}

	for (i = 0; i < ALIGN_CAM_GRAB_NUMBER; i++)
		GenEmptyObj(m_HAlignResultRgn_EachImage + i);

	for (i = 0; i < IMAGE_SAVE_THREAD_MAX_QUEUE; i++)
		m_bRawImageSaveThreadRunning_Cam[i] = FALSE;

	for (i = 0; i < RAW_IMAGE_TEMP_DRIVE_NUMBER + 1; i++)
		bFreezingClearCheck[i] = TRUE;

	InitializeCriticalSection(&m_csRawImageSavingDone);

	InitAlgorithmInspectParam();   //////  P : 딜레이 생길때 문제점 해결요망.
}

Algorithm::~Algorithm(void)
{
	SAFE_DELETE(m_pSemaphore);
	RemoveListScanThreadParams();
	RemoveListInspectThreadParams();
	for (int i = 0; i < IMAGE_SAVE_THREAD_MAX_QUEUE; i++)
		RemoveListSaveRawImageParams_Cam(i);
}

void Algorithm::Initialize(int iVisionCamType)
{
	int iNoAssignedThread = THEAPP.m_iVisionInspectThreadNumber[iVisionCamType];
	CString sSemaName;
	sSemaName.Format("Inspection Thread Pool %d", iVisionCamType);

	m_pSemaphore = new CSemaphore(iNoAssignedThread, iNoAssignedThread, sSemaName);
}

BOOL Algorithm::IsInspectDone(int iBufferIdx)
{
	BOOL bDone = FALSE;

	CSingleLock s(&(m_csInspectDone[iBufferIdx]));

	s.Lock();
	if (m_iInspectBufferStatus[iBufferIdx] == BUFFER_STATUS_POSTPROCESSING)
	{
		bDone = TRUE;
	}
	s.Unlock();

	return bDone;
}

BOOL Algorithm::IsPosProcessingDone(int iBufferIdx)
{
	BOOL bDone = FALSE;

	CSingleLock s(&(m_csInspectDone[iBufferIdx]));

	s.Lock();
	if (m_iInspectBufferStatus[iBufferIdx] == BUFFER_STATUS_AVAILABLE)
	{
		bDone = TRUE;
	}
	s.Unlock();

	return bDone;
}

void Algorithm::ChangeInspectDone(int iBufferIdx, int iStatus)
{
	CSingleLock s(&(m_csInspectDone[iBufferIdx]));

	s.Lock();
	m_iInspectBufferStatus[iBufferIdx] = iStatus;
	s.Unlock();
}

int Algorithm::GetAvailableInspectBufferCount()
{

	return 1;
}

BOOL Algorithm::IsAlgorithmThreadRunning(int iThreadIdx)
{
	BOOL isRun = FALSE;

	CSingleLock s(&(m_csAlgorithmThreadRunning[iThreadIdx]));

	s.Lock();
	isRun = m_bAlgorithmThreadRunning[iThreadIdx];
	s.Unlock();

	return isRun;
}

void Algorithm::ChangeAlgorithmThreadRunning(int iThreadIdx, BOOL bRunning)
{
	CSingleLock s(&(m_csAlgorithmThreadRunning[iThreadIdx]));

	s.Lock();
	m_bAlgorithmThreadRunning[iThreadIdx] = bRunning;
	s.Unlock();
}

BOOL Algorithm::IsResultSaveThreadRunning()
{
	BOOL isRun = FALSE;

	CSingleLock s(&(m_csResultSaveThreadRunning));

	s.Lock();
	isRun = m_bResultSaveThreadRunning;
	if (isRun == FALSE)
		m_bResultSaveThreadRunning = TRUE;
	s.Unlock();

	return isRun;
}

void Algorithm::ChangeResultSaveThreadRunning(BOOL bRunning)
{
	CSingleLock s(&(m_csResultSaveThreadRunning));

	s.Lock();
	m_bResultSaveThreadRunning = bRunning;
	s.Unlock();
}

BOOL Algorithm::IsScanThreadParamEmpty()
{
	BOOL isEmpty = TRUE;

	CSingleLock s(&m_csScan);

	s.Lock();
	isEmpty = m_listScanThreadParam.empty();
	s.Unlock();

	return isEmpty;
}

void Algorithm::AddListScanThreadParam(SCAN_THREAD_PARAM *pParam)
{
	CSingleLock s(&m_csScan);

	s.Lock();
	m_listScanThreadParam.push_back(pParam);
	s.Unlock();
}

void Algorithm::RemoveListScanThreadParams()
{
	SCAN_THREAD_PARAM* pModuleInspecParam = NULL;

	CSingleLock s(&m_csScan);

	s.Lock();

	while (!m_listScanThreadParam.empty())
	{
		pModuleInspecParam = NULL;

		pModuleInspecParam = m_listScanThreadParam.front();
		m_listScanThreadParam.pop_front();

		SAFE_DELETE(pModuleInspecParam);
	}

	m_listScanThreadParam.clear();

	s.Unlock();
}

SCAN_THREAD_PARAM* Algorithm::GetNextScanThreadParam()
{
	SCAN_THREAD_PARAM* pModuleInspecParam = NULL;

	CSingleLock s(&m_csScan);

	s.Lock();
	if (!m_listScanThreadParam.empty())
	{
		pModuleInspecParam = m_listScanThreadParam.front();
		m_listScanThreadParam.pop_front();
	}
	s.Unlock();

	return pModuleInspecParam;
}

BOOL Algorithm::IsInspectThreadParamEmpty()
{
	BOOL isEmpty = TRUE;

	CSingleLock s(&m_csInspect);

	s.Lock();
	isEmpty = m_listInspectThreadParam.empty();
	s.Unlock();

	return isEmpty;
}

void Algorithm::AddListInspectThreadParam(INSPECT_THREAD_PARAM *pParam)
{
	CSingleLock s(&m_csInspect);

	s.Lock();
	m_listInspectThreadParam.push_back(pParam);
	s.Unlock();
}

void Algorithm::RemoveListInspectThreadParams()
{
	INSPECT_THREAD_PARAM* pInspecThreadParam = NULL;

	CSingleLock s(&m_csInspect);

	s.Lock();

	while (!m_listInspectThreadParam.empty())
	{
		pInspecThreadParam = NULL;

		pInspecThreadParam = m_listInspectThreadParam.front();
		m_listInspectThreadParam.pop_front();

		SAFE_DELETE(pInspecThreadParam);
	}

	m_listInspectThreadParam.clear();

	s.Unlock();
}

INSPECT_THREAD_PARAM* Algorithm::GetNextInspectThreadParam()
{
	INSPECT_THREAD_PARAM* pInspecThreadParam = NULL;

	CSingleLock s(&m_csInspect);

	s.Lock();
	if (!m_listInspectThreadParam.empty())
	{
		pInspecThreadParam = m_listInspectThreadParam.front();
		m_listInspectThreadParam.pop_front();
	}
	s.Unlock();

	return pInspecThreadParam;
}

BOOL Algorithm::IsSaveRawImageParamEmpty_Cam(int nIndex)
{
	BOOL isEmpty = TRUE;

	CSingleLock s(&m_csSaveRawImage_Cam);

	s.Lock();
	isEmpty = m_listRawImageSaveParam_Cam[nIndex].empty();
	s.Unlock();

	return isEmpty;
}

void Algorithm::AddListSaveRawImageParam_Cam(RAW_IMAGE_SAVE_PARAM_CAM *pParam, int nIndex)
{
	CSingleLock s(&m_csSaveRawImage_Cam);

	s.Lock();
	m_listRawImageSaveParam_Cam[nIndex].push_back(pParam);
	s.Unlock();
}

void Algorithm::RemoveListSaveRawImageParams_Cam(int nIndex)
{
	RAW_IMAGE_SAVE_PARAM_CAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveRawImage_Cam);

	s.Lock();

	while (!m_listRawImageSaveParam_Cam[nIndex].empty())
	{
		pSaveParam = NULL;

		pSaveParam = m_listRawImageSaveParam_Cam[nIndex].front();
		m_listRawImageSaveParam_Cam[nIndex].pop_front();

		SAFE_DELETE(pSaveParam);
	}

	m_listRawImageSaveParam_Cam[nIndex].clear();

	s.Unlock();
}

RAW_IMAGE_SAVE_PARAM_CAM* Algorithm::GetNextSaveRawImageParam_Cam(int nIndex)
{
	RAW_IMAGE_SAVE_PARAM_CAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveRawImage_Cam);

	s.Lock();
	if (!m_listRawImageSaveParam_Cam[nIndex].empty())
	{
		pSaveParam = m_listRawImageSaveParam_Cam[nIndex].front();
		m_listRawImageSaveParam_Cam[nIndex].pop_front();
	}
	s.Unlock();

	return pSaveParam;
}

BOOL Algorithm::CopyRawImage(int iInspectionBufferIdx)
{
	int iPcVisionNo, iVisionCamType, iMzNo, iTrayNo, iModuleNo, iNoInspectImage;
	int i;
	CString sBarcodeID;

	iPcVisionNo = m_iPcVisionNo;
	iVisionCamType = m_iVisionCamType;
	iNoInspectImage = m_iNoInspectImage;
	iMzNo = m_nMzNo[iInspectionBufferIdx];
	iTrayNo = m_nTrayNo[iInspectionBufferIdx];

	iModuleNo = m_nModuleNo[iInspectionBufferIdx];

	sBarcodeID = THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1];

	try
	{
		if (THEAPP.Struct_PreferenceStruct.m_iRawImageThreadType == RAW_IMAGE_SAVE_THREAD_PC)
		{
			RAW_IMAGE_SAVE_PARAM* pSaveThreadIDParam = new RAW_IMAGE_SAVE_PARAM;

			//Local image copy...

			for (i = 0; i < iNoInspectImage; i++)
			{
				GenEmptyObj(&(pSaveThreadIDParam->HSaveImage[i]));

#if defined(SINGLE_LENS) || defined(ASSY_LENS)
				int iImageChType = CHANNEL_TYPE_I;
#else
				int iImageChType = CHANNEL_TYPE_COLOR;
#endif

				if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[iPcVisionNo])
					iImageChType = CHANNEL_TYPE_I;

				if (THEAPP.m_pGFunction->ValidHImage(m_HInspectCImage[iInspectionBufferIdx][iImageChType][i]))
				{
					if (THEAPP.Struct_PreferenceStruct.m_bReduceRawImage)
					{
						if (THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio < 1)
							ZoomImageFactor(m_HInspectCImage[iInspectionBufferIdx][iImageChType][i], pSaveThreadIDParam->HSaveImage + i, THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio, THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio, "none");
						else
							CopyImage(m_HInspectCImage[iInspectionBufferIdx][iImageChType][i], pSaveThreadIDParam->HSaveImage + i);
					}
					else
						CopyImage(m_HInspectCImage[iInspectionBufferIdx][iImageChType][i], pSaveThreadIDParam->HSaveImage + i);
				}
			}

			pSaveThreadIDParam->iPcVisionNo = iPcVisionNo;
			pSaveThreadIDParam->iVisionCamType = iVisionCamType;
			pSaveThreadIDParam->iMzNo = iMzNo;
			pSaveThreadIDParam->iTrayNo = iTrayNo;
			pSaveThreadIDParam->iModuleNo = iModuleNo;
			pSaveThreadIDParam->iNoInspectImage = iNoInspectImage;
			pSaveThreadIDParam->sBarcodeID = sBarcodeID;

			THEAPP.m_pInspectService->AddListSaveRawImageParam(pSaveThreadIDParam, iPcVisionNo, iMzNo, iTrayNo, iModuleNo);

			THEAPP.m_pInspectService->SaveRawImage(iPcVisionNo, iMzNo, iTrayNo, iModuleNo);
		}
		else if (THEAPP.Struct_PreferenceStruct.m_iRawImageThreadType == RAW_IMAGE_SAVE_THREAD_CAMERA)
		{
			RAW_IMAGE_SAVE_PARAM_CAM* pSaveThreadIDParam = new RAW_IMAGE_SAVE_PARAM_CAM;

			//Local image copy...

			for (i = 0; i < iNoInspectImage; i++)
			{
				GenEmptyObj(&(pSaveThreadIDParam->HSaveImage[i]));

#if defined(SINGLE_LENS) || defined(ASSY_LENS)
				int iImageChType = CHANNEL_TYPE_I;
#else
				int iImageChType = CHANNEL_TYPE_COLOR;
#endif

				if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[iPcVisionNo])
					iImageChType = CHANNEL_TYPE_I;

				if (THEAPP.m_pGFunction->ValidHImage(m_HInspectCImage[iInspectionBufferIdx][iImageChType][i]))
				{
					if (THEAPP.Struct_PreferenceStruct.m_bReduceRawImage)
					{
						if (THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio < 1)
							ZoomImageFactor(m_HInspectCImage[iInspectionBufferIdx][iImageChType][i], pSaveThreadIDParam->HSaveImage + i, THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio, THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio, "none");
						else
							CopyImage(m_HInspectCImage[iInspectionBufferIdx][iImageChType][i], pSaveThreadIDParam->HSaveImage + i);
					}
					else
						CopyImage(m_HInspectCImage[iInspectionBufferIdx][iImageChType][i], pSaveThreadIDParam->HSaveImage + i);
				}
			}

			pSaveThreadIDParam->iPcVisionNo = iPcVisionNo;
			pSaveThreadIDParam->iVisionCamType = iVisionCamType;
			pSaveThreadIDParam->iMzNo = iMzNo;
			pSaveThreadIDParam->iTrayNo = iTrayNo;
			pSaveThreadIDParam->iModuleNo = iModuleNo;
			pSaveThreadIDParam->iNoInspectImage = iNoInspectImage;
			pSaveThreadIDParam->sBarcodeID = sBarcodeID;

			if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
			{
				int iTotalQeueCount = 0;
				int iCurImageQueueIndex = 0;

				strLog.Format("RawImage thread list, Queue size: %d", m_listRawImageSaveParam_Cam[0].size());

				iTotalQeueCount = iTotalQeueCount + m_listRawImageSaveParam_Cam[0].size();

				if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
				{
					for (int j = 1; j < THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber; j++)
					{
						strLog.Format(strLog + ", %d", m_listRawImageSaveParam_Cam[j].size());
						iTotalQeueCount = iTotalQeueCount + m_listRawImageSaveParam_Cam[j].size();
					}
				}
				THEAPP.m_log_thread->info("{}", LOG_CSTR(strLog));

				int iThreadQueueNumber;
				if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
				{
					iThreadQueueNumber = THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;
					iCurImageQueueIndex = (iTrayNo - 1) % THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;
				}
				else
					iThreadQueueNumber = 1;

				AddListSaveRawImageParam_Cam(pSaveThreadIDParam, iCurImageQueueIndex);

				for (iCurImageQueueIndex = 0; iCurImageQueueIndex < iThreadQueueNumber; iCurImageQueueIndex++)
				{
					if (m_bRawImageSaveThreadRunning_Cam[iCurImageQueueIndex] == FALSE)
					{
						m_nRawImageQueueIndex = iCurImageQueueIndex;
						AfxBeginThread(SaveRawImageThread_Cam, this);
						Sleep(15);
					}
				}
			}
			else
			{
				BOOL bThreadErrorCheck = FALSE;
				int iCurImageQueueIndex = 0;

				if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
				{
					strLog.Format("RawImage thread list, Queue size: %d", m_listRawImageSaveParam_Cam[0].size());
					if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1)
						strLog.Format(strLog + ", %d", m_listRawImageSaveParam_Cam[1].size());
					if (THEAPP.Struct_PreferenceStruct.m_bUseLAS2)
						strLog.Format(strLog + ", %d", m_listRawImageSaveParam_Cam[2].size());
					if (THEAPP.Struct_PreferenceStruct.m_bUseLAS3)
						strLog.Format(strLog + ", %d", m_listRawImageSaveParam_Cam[3].size());
					THEAPP.m_log_thread->info("{}", LOG_CSTR(strLog));

					for (i = 0; i < RAW_IMAGE_TEMP_DRIVE_NUMBER + 1; i++)
					{
						if (i == 0 && m_listRawImageSaveParam_Cam[i].size() == 0)
							bFreezingClearCheck[i] = TRUE;

						if (i > 0 && bFreezingClearCheck[0] == TRUE && m_listRawImageSaveParam_Cam[i].size() == 0)
							bFreezingClearCheck[i] = TRUE;
					}

					if (bFreezingClearCheck[0] == FALSE || m_listRawImageSaveParam_Cam[0].size() >= 30)
					{
						bThreadErrorCheck = TRUE;
						bFreezingClearCheck[0] = FALSE;

						if (bFreezingClearCheck[1] == TRUE && bThreadErrorCheck == TRUE && THEAPP.Struct_PreferenceStruct.m_bUseLAS1 && m_listRawImageSaveParam_Cam[1].size() < 30)
						{
							bThreadErrorCheck = FALSE;
							iCurImageQueueIndex = 1;
						}
						else
						{
							if (m_listRawImageSaveParam_Cam[1].size() >= 30)
								bFreezingClearCheck[1] = FALSE;
						}

						if (bFreezingClearCheck[2] == TRUE && bThreadErrorCheck == TRUE && THEAPP.Struct_PreferenceStruct.m_bUseLAS2 && m_listRawImageSaveParam_Cam[2].size() < 30)
						{
							bThreadErrorCheck = FALSE;
							iCurImageQueueIndex = 2;
						}
						else
						{
							if (m_listRawImageSaveParam_Cam[2].size() >= 30)
								bFreezingClearCheck[2] = FALSE;
						}

						if (bFreezingClearCheck[3] == TRUE && bThreadErrorCheck == TRUE && THEAPP.Struct_PreferenceStruct.m_bUseLAS3 && m_listRawImageSaveParam_Cam[3].size() < 30)
						{
							bThreadErrorCheck = FALSE;
							iCurImageQueueIndex = 3;
						}
						else
						{
							if (m_listRawImageSaveParam_Cam[3].size() >= 30)
								bFreezingClearCheck[3] = FALSE;
						}

						if (bThreadErrorCheck == TRUE)
						{
							if (m_listRawImageSaveParam_Cam[0].size() < 20)
								iCurImageQueueIndex = 0;
							else if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1 && m_listRawImageSaveParam_Cam[1].size() < 20)
								iCurImageQueueIndex = 1;
							else if (THEAPP.Struct_PreferenceStruct.m_bUseLAS2 && m_listRawImageSaveParam_Cam[2].size() < 20)
								iCurImageQueueIndex = 2;
							else if (THEAPP.Struct_PreferenceStruct.m_bUseLAS3 && m_listRawImageSaveParam_Cam[3].size() < 20)
								iCurImageQueueIndex = 3;
							else
								iCurImageQueueIndex = 0;
						}
					}

					AddListSaveRawImageParam_Cam(pSaveThreadIDParam, iCurImageQueueIndex);

					for (iCurImageQueueIndex = 0; iCurImageQueueIndex < RAW_IMAGE_TEMP_DRIVE_NUMBER + 1; iCurImageQueueIndex++)
					{
						if (m_bRawImageSaveThreadRunning_Cam[iCurImageQueueIndex] == FALSE)
						{
							m_nRawImageQueueIndex = iCurImageQueueIndex;
							AfxBeginThread(SaveRawImageThread_Cam, this);
							Sleep(15);
						}
					}
				}
				else
				{
					strLog.Format("RawImage thread list, Queue size: %d", m_listRawImageSaveParam_Cam[0].size());
					strLog.Format(strLog + ", %d", m_listRawImageSaveParam_Cam[1].size());
					THEAPP.m_log_thread->info("{}", LOG_CSTR(strLog));

					for (i = 0; i < 2; i++)
					{
						if (i == 0 && m_listRawImageSaveParam_Cam[i].size() == 0)
							bFreezingClearCheck[i] = TRUE;

						if (i > 0 && bFreezingClearCheck[0] == TRUE && m_listRawImageSaveParam_Cam[i].size() == 0)
							bFreezingClearCheck[i] = TRUE;
					}

					if (bFreezingClearCheck[0] == FALSE || m_listRawImageSaveParam_Cam[0].size() >= 30)
					{
						bThreadErrorCheck = TRUE;
						bFreezingClearCheck[0] = FALSE;

						if (bFreezingClearCheck[1] == TRUE && bThreadErrorCheck == TRUE && m_listRawImageSaveParam_Cam[1].size() < 60)
						{
							bThreadErrorCheck = FALSE;
							iCurImageQueueIndex = 1;
						}
						else
						{
							if (m_listRawImageSaveParam_Cam[1].size() >= 60)
								bFreezingClearCheck[1] = FALSE;
						}

						if (bThreadErrorCheck == TRUE)
						{
							if (m_listRawImageSaveParam_Cam[0].size() < 20)
								iCurImageQueueIndex = 0;
							else if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1 && m_listRawImageSaveParam_Cam[1].size() < 45)
								iCurImageQueueIndex = 1;
							else
								iCurImageQueueIndex = 0;
						}
					}

					AddListSaveRawImageParam_Cam(pSaveThreadIDParam, iCurImageQueueIndex);

					for (iCurImageQueueIndex = 0; iCurImageQueueIndex < 2; iCurImageQueueIndex++)
					{
						if (m_bRawImageSaveThreadRunning_Cam[iCurImageQueueIndex] == FALSE)
						{
							m_nRawImageQueueIndex = iCurImageQueueIndex;
							AfxBeginThread(SaveRawImageThread_Cam, this);
							Sleep(15);
						}
					}
				}
			}
		}

		return TRUE;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [Algorithm::CopyRawImage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::CopyInspectInformation(const AlgorithmCopyParam& param)
{
	AlgorithmPipeline pipeline(this);
	int iScanBufferIdx = pipeline.PrepareAlgorithmBuffer(param);
	if (iScanBufferIdx < 0) return FALSE;

	if (param.IsFirstEntry())
	{
		ChangeInspectDone(iScanBufferIdx, BUFFER_STATUS_INSPECTING);

		AddListScanThreadParam(new SCAN_THREAD_PARAM{ iScanBufferIdx });
		AddListInspectThreadParam(new INSPECT_THREAD_PARAM{ iScanBufferIdx });

		for (int i = 0; i < THEAPP.m_iVisionInspectThreadNumber[m_iPcVisionNo]; i++)
		{
			if (!IsAlgorithmThreadRunning(i))
			{
				THREAD_PARAM* p = new THREAD_PARAM(i, m_iPcVisionNo);
				AfxBeginThread(param.IsBatchMode() ? AlgorithmThread_Batch : AlgorithmThread, p);
			}
		}
	}
	return TRUE;
}

void Algorithm::InitAlgorithmInspectParam()
{
	InitOcrResult();

	InitHomerTiltResult();

	InitConformalHeightResult();
	InitMiniConformalHeightResult();

	m_bManualBarcodeForOcrInspect = FALSE;
}

void Algorithm::InitGapResult()
{
	TupleGenConst(0, 0, &m_HGapDistanceMeasureResult);
	GenEmptyObj(&m_HGapMeasureXLD);
}

void Algorithm::InitCornerResult()
{
	GenEmptyObj(&m_HCornerMeasureXLD);
}

void Algorithm::InitHomerTiltResult()
{
	m_dHomerTiltHeight = 0;
	m_dHomerTiltBenvolioDist = 0;
}

void Algorithm::InitMiniConformalHeightResult()
{
	m_dDistanceMiniConformalHeight = 0.0;
	m_dCCMH_CenterX = 0.0;
	m_dCCMH_CenterY = 0.0;
	GenEmptyObj(&m_HMiniConformalHeightEndLineXLD);
}

void Algorithm::InitConformalHeightResult()
{
	m_dDistanceConformalHeight = 0.0;
	m_dCCLH_CenterX = 0.0;
	m_dCCLH_CenterY = 0.0;
	GenEmptyObj(&m_HConformalHeightEndLineXLD);
}

void Algorithm::InitOcrResult()
{
	m_bOcrRegResult = FALSE;
	m_sOcrResult = _T("NOREAD");
	m_strOCRChar = _T("");

	GenEmptyObj(&m_hConcatCharRgn);
	GenEmptyObj(&m_HOCRDetectRgn);
}

BOOL Algorithm::ImageAlignShapeMatching(BOOL bImageTransform, int iMzNo, HObject HMatchingImage, HObject *pHAlignImage, HTuple *plAlignModelID, BOOL *pbAlignImageFlag, double dMatchingAngleRange, double dMatchingScaleMin, double dMatchingScaleMax,
	double dMatchingScore, double dTeachAlignRefX, double dTeachAlignRefY, int iMatchingMethod, int iMatchingLTX, int iMatchingLTY, int iMatchingRBX, int iMatchingRBY, int iMatchingSearchMarginX, int iMatchingSearchMarginY, BOOL bMatchingApplyAffineConstant,
	int iPcVisionNo, int iVisionCamType, int iCurThreadIdx, int iMatchingImageIndex, BOOL bUseMatchingXYComp, int iMatchingImageXYRef, int iMatchingXYRefLine, BOOL bUseMatchingAngleComp, int iMatchingAngleRefLine, double *pdRotationAngleDeg, double *pdDeltaX, double *pdDeltaY, HTuple *pHMatchingHomMat)
{
	try
	{
		BOOL bDebugSave = FALSE;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		if (bDebugSave)
		{
			WriteImage(HMatchingImage, "bmp", 0, "c:\\DualTest\\MatchingImage");
		}

		*pdRotationAngleDeg = 0;
		*pdDeltaX = 0;
		*pdDeltaY = 0;

		HTuple dAngleRangeRad;
		TupleRad(dMatchingAngleRange, &dAngleRangeRad);

		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, Scale, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate;

		double dTransX, dTransY, dRotationAngle;
		HObject HModelContour, HModelRgn, HMatchingImageReduced;
		HTuple HomMat2D;

		if (*plAlignModelID >= 0)
		{
			if (iMatchingMethod == ALIGN_MATCHING_TEMPLATE)
			{
				GenRectangle1(&HModelRgn, iMatchingLTY, iMatchingLTX, iMatchingRBY, iMatchingRBX);
				DilationRectangle1(HModelRgn, &HModelRgn, iMatchingSearchMarginX * 2 + 1, iMatchingSearchMarginY * 2 + 1);
			}
			else if (iMatchingMethod == ALIGN_MATCHING_SHAPE)
			{
				GetShapeModelContours(&HModelContour, *plAlignModelID, 1);
				GenRegionContourXld(HModelContour, &HModelRgn, "filled");
				Union1(HModelRgn, &HModelRgn);
				ShapeTrans(HModelRgn, &HModelRgn, "rectangle1");
				DilationRectangle1(HModelRgn, &HModelRgn, iMatchingSearchMarginX * 2 + 1, iMatchingSearchMarginY * 2 + 1);
				MoveRegion(HModelRgn, &HModelRgn, (Hlong)dTeachAlignRefY, (Hlong)dTeachAlignRefX);
			}
			else if (iMatchingMethod == ALIGN_MATCHING_PERSPECTIVE)
			{
				GetDeformableModelContours(&HModelContour, *plAlignModelID, 1);
				GenRegionContourXld(HModelContour, &HModelRgn, "filled");
				Union1(HModelRgn, &HModelRgn);
				ShapeTrans(HModelRgn, &HModelRgn, "rectangle1");
				DilationRectangle1(HModelRgn, &HModelRgn, iMatchingSearchMarginX * 2 + 1, iMatchingSearchMarginY * 2 + 1);
				MoveRegion(HModelRgn, &HModelRgn, (Hlong)dTeachAlignRefY, (Hlong)dTeachAlignRefX);
			}

			ReduceDomain(HMatchingImage, HModelRgn, &HMatchingImageReduced);

			if (bDebugSave)
			{
				WriteImage(HMatchingImageReduced, "bmp", 0, "c:\\DualTest\\HMatchingImageReduced");
			}

			Row = HTuple(0.0);
			Column = HTuple(0.0);
			Angle = HTuple(0.0);
			Scale = HTuple(0.0);
			Score = HTuple(0.0);

			int iNoTeachNumLevel = 0;
			HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

			if (iMatchingMethod == ALIGN_MATCHING_TEMPLATE)
			{
				GetNccModelParams(*plAlignModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &Metric);

				iNoTeachNumLevel = NumLevels[0].L();
				if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_TEMPALTE)
					iNoTeachNumLevel = MAX_PYRAMID_LEVEL_TEMPALTE;
				else
					iNoTeachNumLevel = 0;

				FindNccModel(HMatchingImageReduced,
					*plAlignModelID,							// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,							// Extent of the rotation angles.
					dMatchingScore,								// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"true",										// Subpixel accuracy
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&Score);									// Score of the found instances of the model.
			}
			else if (iMatchingMethod == ALIGN_MATCHING_SHAPE)
			{
				GetShapeModelParams(*plAlignModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

				iNoTeachNumLevel = NumLevels[0].L();
				if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_SHAPE)
					iNoTeachNumLevel = MAX_PYRAMID_LEVEL_SHAPE;
				else
					iNoTeachNumLevel = 0;

				FindScaledShapeModel(HMatchingImageReduced,
					*plAlignModelID,							// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,							// Extent of the rotation angles.
					dMatchingScaleMin,
					dMatchingScaleMax,
					dMatchingScore,								// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&Scale,
					&Score);									// Score of the found instances of the model.
			}
			else if (iMatchingMethod == ALIGN_MATCHING_PERSPECTIVE)
			{
				GetDeformableModelParams(*plAlignModelID, "num_levels", &NumLevels);

				iNoTeachNumLevel = NumLevels[0].L();
				if (iNoTeachNumLevel > 7)
					iNoTeachNumLevel = 7;
				else
					iNoTeachNumLevel = 0;

				FindPlanarUncalibDeformableModel(HMatchingImageReduced,
					*plAlignModelID,							// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,							// Extent of the rotation angles.
					dMatchingScaleMin,
					dMatchingScaleMax,
					dMatchingScaleMin,
					dMatchingScaleMax,
					dMatchingScore,								// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
					HTuple(),
					HTuple(),
					&HomMat2D,									// Homographies.
					&Score);									// Score of the found instances of the model.
			}

			HTuple HlNoFoundNumber;
			TupleLength(Score, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			if (lNoFoundNumber <= 0)
			{
				return FALSE;
			}

			if (iMatchingMethod == ALIGN_MATCHING_TEMPLATE || iMatchingMethod == ALIGN_MATCHING_SHAPE)
			{
				if (bUseMatchingAngleComp)
				{
					dTransX = dTeachAlignRefX - Column[0].D();
					dTransY = dTeachAlignRefY - Row[0].D();
					dRotationAngle = -1.0 * Angle[0].D();

					HomMat2dIdentity(&HomMat2DIdentity);
					HomMat2dTranslate(HomMat2DIdentity, dTransY, dTransX, &HomMat2DTranslate);
					HomMat2dRotate(HomMat2DTranslate, dRotationAngle, dTeachAlignRefY, dTeachAlignRefX, &HomMat2DRotate);

					//////////////////////////////////////////////////////////////////////////

					int iNoEdgePoint = 0;
					DPOINT dEdgePoint[2];
					DPOINT dRefLinePoint[2];

					HObject HMatchingPreAlignImage, HMatchingPreAlignGrayImage;	// Color

					if (bMatchingApplyAffineConstant)
						AffineTransImage(pHAlignImage[iMatchingImageIndex], &HMatchingPreAlignImage, HomMat2DRotate, "constant", "false");		// "constant"
					else
						AffineTransImage(pHAlignImage[iMatchingImageIndex], &HMatchingPreAlignImage, HomMat2DRotate, "nearest_neighbor", "false");		// "constant"

					Rgb1ToGray(HMatchingPreAlignImage, &HMatchingPreAlignGrayImage);

					int iNoInspectROI = 0;
					iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();

					GTRegion *pInspectROIRgn;
					HObject HROIHRegion;
					HObject pHResultXLD;
					int iEdgeMeasureAlignPosX, iEdgeMeasureAlignPosY;
					double dEdgeCenterX, dEdgeCenterY;
					HTuple HAdditionalAngle;
					double dRotationAngleDegree;

					for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
					{
						pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

						if (pInspectROIRgn == NULL)
							continue;

						if (pInspectROIRgn->miTeachImageIndex != (iMatchingImageIndex + 1))
							continue;

						if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN_ANGLE)
							continue;

						for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
						{
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
							{
								HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

								iEdgeMeasureAlignPosX = iEdgeMeasureAlignPosY = INVALID_ALIGN_RESULT;

								if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseEdgeMeasure)
								{
									dEdgeCenterX = dEdgeCenterY = -1;

									EdgeMeasureAlgorithm(HMatchingPreAlignGrayImage, NULL, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &pHResultXLD, &iEdgeMeasureAlignPosX, &iEdgeMeasureAlignPosY, &dEdgeCenterX, &dEdgeCenterY);

									if (dEdgeCenterX >= 0 && dEdgeCenterY >= 0)
									{
										dEdgePoint[iNoEdgePoint].x = dEdgeCenterX;
										dEdgePoint[iNoEdgePoint].y = dEdgeCenterY;

										++iNoEdgePoint;

										break;
									}
								}
							}
						}

						if (iNoEdgePoint == 2)
							break;
					}

					if (iNoEdgePoint != 2)
					{
						if (bImageTransform)
						{
							for (int i = 0; i < MAX_IMAGE_TAB; i++)
							{
								if (pbAlignImageFlag[i] == TRUE)
								{
									if (THEAPP.m_pGFunction->ValidHImage(pHAlignImage[i]))
									{
										if (bMatchingApplyAffineConstant)
											AffineTransImage(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "constant", "false");		// "constant"
										else
											AffineTransImage(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "nearest_neighbor", "false");		// "constant"
									}
								}
							}
						}
					}
					else
					{
						if (iMatchingAngleRefLine == MATCHING_ANGLE_REF_LINE_X)
						{
							dRefLinePoint[0] = dEdgePoint[0];
							dRefLinePoint[1].x = dEdgePoint[1].x;
							dRefLinePoint[1].y = dEdgePoint[0].y;
						}
						else if (iMatchingAngleRefLine == MATCHING_ANGLE_REF_LINE_Y)
						{
							dRefLinePoint[0] = dEdgePoint[0];
							dRefLinePoint[1].y = dEdgePoint[1].y;
							dRefLinePoint[1].x = dEdgePoint[0].x;
						}

						AngleLl(dRefLinePoint[0].y, dRefLinePoint[0].x, dRefLinePoint[1].y, dRefLinePoint[1].x, dEdgePoint[0].y, dEdgePoint[0].x, dEdgePoint[1].y, dEdgePoint[1].x, &HAdditionalAngle);

						TupleDeg(HAdditionalAngle, &HAdditionalAngle);
						dRotationAngleDegree = HAdditionalAngle[0].D();

						if (fabs(dRotationAngleDegree) >= 90)
						{
							if (dRotationAngleDegree >= 0)
								dRotationAngleDegree = 180.0 - dRotationAngleDegree;
							else
								dRotationAngleDegree = 180.0 + dRotationAngleDegree;
						}

						TupleRad(HTuple(dRotationAngleDegree), &HAdditionalAngle);

						Angle = Angle + HAdditionalAngle;

						dRotationAngle = -1.0 * Angle[0].D();
						HomMat2dRotate(HomMat2DTranslate, dRotationAngle, dTeachAlignRefY, dTeachAlignRefX, &HomMat2DRotate);

						if (bImageTransform)
						{
							for (int i = 0; i < MAX_IMAGE_TAB; i++)
							{
								if (pbAlignImageFlag[i] == TRUE)
								{
									if (THEAPP.m_pGFunction->ValidHImage(pHAlignImage[i]))
									{
										if (bMatchingApplyAffineConstant)
											AffineTransImage(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "constant", "false");		// "constant"
										else
											AffineTransImage(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "nearest_neighbor", "false");		// "constant"
									}
								}
							}
						}
					}

					TupleDeg(Angle, &Angle);
					*pdRotationAngleDeg = Angle[0].D();	// ROI to Image

					*pdDeltaX = dTransX;		// Image to ROI
					*pdDeltaY = dTransY;		// Image to ROI
				}
				else if (bUseMatchingXYComp)
				{
					dTransX = dTeachAlignRefX - Column[0].D();
					dTransY = dTeachAlignRefY - Row[0].D();
					dRotationAngle = -1.0 * Angle[0].D();

					HomMat2dIdentity(&HomMat2DIdentity);
					HomMat2dTranslate(HomMat2DIdentity, dTransY, dTransX, &HomMat2DTranslate);
					HomMat2dRotate(HomMat2DTranslate, dRotationAngle, dTeachAlignRefY, dTeachAlignRefX, &HomMat2DRotate);

					//////////////////////////////////////////////////////////////////////////

					int iNoEdgePoint = 0;
					DPOINT dEdgePoint[2];
					DPOINT dRefLinePoint[2];

					HObject HMatchingPreAlignImage, HMatchingPreAlignGrayImage;	// Color

					if (bMatchingApplyAffineConstant)
						AffineTransImage(pHAlignImage[iMatchingImageIndex], &HMatchingPreAlignImage, HomMat2DRotate, "constant", "false");		// "constant"
					else
						AffineTransImage(pHAlignImage[iMatchingImageIndex], &HMatchingPreAlignImage, HomMat2DRotate, "nearest_neighbor", "false");		// "constant"

					Rgb1ToGray(HMatchingPreAlignImage, &HMatchingPreAlignGrayImage);

					int iNoInspectROI = 0;
					iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();

					GTRegion *pInspectROIRgn;
					HObject HROIHRegion;
					HObject pHResultXLD;
					int iEdgeMeasureAlignPosX, iEdgeMeasureAlignPosY;
					double dEdgeCenterX, dEdgeCenterY;
					HTuple HAdditionalAngle;
					double dRotationAngleDegree;
					//////////////////////////////
					POINT LTPoint;

					for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
					{
						pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

						if (pInspectROIRgn == NULL)
							continue;

						if (pInspectROIRgn->miTeachImageIndex != (iMatchingImageIndex + 1))
							continue;

						if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_GLOBAL_ALIGN)
						{
							pInspectROIRgn->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService_N[iVisionCamType]);
						}

						if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
							continue;

						for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
						{
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect && iMatchingImageXYRef == pInspectROIRgn->miLocalAlignID)
							{
								HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

								iEdgeMeasureAlignPosX = iEdgeMeasureAlignPosY = INVALID_ALIGN_RESULT;

								if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseEdgeMeasure)
								{
									dEdgeCenterX = dEdgeCenterY = -1;

									EdgeMeasureAlgorithm(HMatchingPreAlignGrayImage, NULL, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &pHResultXLD, &iEdgeMeasureAlignPosX, &iEdgeMeasureAlignPosY, &dEdgeCenterX, &dEdgeCenterY);

									iNoEdgePoint = 1;
								}
							}
						}

						//if (iNoEdgePoint==1)
						//	break;
					}

					dTransX = dTeachAlignRefX - Column[0].D();
					dTransY = dTeachAlignRefY - Row[0].D();
					dRotationAngle = -1.0 * Angle[0].D();


					if (iMatchingXYRefLine == MATCHING_ANGLE_REF_LINE_X)
						dTransX = dEdgeCenterX;
					else
						dTransY = dEdgeCenterY;

					TupleDeg(Angle, &Angle);
					*pdRotationAngleDeg = Angle[0].D();	// ROI to Image

					*pdDeltaX = dTransX;		// Image to ROI
					*pdDeltaY = dTransY;		// Image to ROI

					HomMat2dIdentity(&HomMat2DIdentity);
					HomMat2dTranslate(HomMat2DIdentity, dTransY, dTransX, &HomMat2DTranslate);
					HomMat2dRotate(HomMat2DTranslate, dRotationAngle, dTeachAlignRefY, dTeachAlignRefX, &HomMat2DRotate);

					if (bImageTransform)
					{
						for (int i = 0; i < MAX_IMAGE_TAB; i++)
						{
							if (pbAlignImageFlag[i] == TRUE)
							{
								if (THEAPP.m_pGFunction->ValidHImage(pHAlignImage[i]))
								{
									if (bMatchingApplyAffineConstant)
										AffineTransImage(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "constant", "false");		// "constant"
									else
										AffineTransImage(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "nearest_neighbor", "false");		// "constant"
								}
							}
						}
					}
				}
				else
				{
					dTransX = dTeachAlignRefX - Column[0].D();
					dTransY = dTeachAlignRefY - Row[0].D();
					dRotationAngle = -1.0 * Angle[0].D();

					TupleDeg(Angle, &Angle);
					*pdRotationAngleDeg = Angle[0].D();	// ROI to Image

					*pdDeltaX = dTransX;		// Image to ROI
					*pdDeltaY = dTransY;		// Image to ROI

					HomMat2dIdentity(&HomMat2DIdentity);
					HomMat2dTranslate(HomMat2DIdentity, dTransY, dTransX, &HomMat2DTranslate);
					HomMat2dRotate(HomMat2DTranslate, dRotationAngle, dTeachAlignRefY, dTeachAlignRefX, &HomMat2DRotate);

					if (bImageTransform)
					{
						for (int i = 0; i < MAX_IMAGE_TAB; i++)
						{
							if (pbAlignImageFlag[i] == TRUE)
							{
								if (THEAPP.m_pGFunction->ValidHImage(pHAlignImage[i]))
								{
									if (bMatchingApplyAffineConstant)
										AffineTransImage(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "constant", "false");		// "constant"
									else
										AffineTransImage(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "nearest_neighbor", "false");		// "constant"
								}
							}
						}
					}
				}

				if (pHMatchingHomMat != NULL)
					*pHMatchingHomMat = HomMat2DRotate;
			}
			else if (iMatchingMethod == ALIGN_MATCHING_PERSPECTIVE)
			{
				HomMat2dInvert(HomMat2D, &HomMat2DRotate);

				if (bImageTransform)
				{
					for (int i = 0; i < MAX_IMAGE_TAB; i++)
					{
						if (pbAlignImageFlag[i] == TRUE)
						{
							if (THEAPP.m_pGFunction->ValidHImage(pHAlignImage[i]))
								ProjectiveTransImage(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "bilinear", "false", "false");	// "nearest_neighbor"
						}
					}
				}

				if (pHMatchingHomMat != NULL)
					*pHMatchingHomMat = HomMat2DRotate;
			}
		}
		else
			return FALSE;

		return TRUE;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [Algorithm::ImageAlignShapeMatching] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::ImageTransformUsingHomMat(int iMatchingImageIndex, int iAffineTransImageIndex, HObject *pHAlignImage, BOOL *pbAlignImageFlag, int iMatchingMethod, BOOL bMatchingApplyAffineConstant, HTuple *pHMatchingHomMat)
{
	try
	{
		int i;

		if (iMatchingMethod == ALIGN_MATCHING_TEMPLATE || iMatchingMethod == ALIGN_MATCHING_SHAPE)
		{
			for (i = 0; i < MAX_IMAGE_TAB; i++)
			{
				if (pbAlignImageFlag[i] == TRUE)
				{
					if (iAffineTransImageIndex == i)
					{
						if (THEAPP.m_pGFunction->ValidHImage(*pHAlignImage))
						{
							if (bMatchingApplyAffineConstant)
								AffineTransImage(*pHAlignImage, pHAlignImage, pHMatchingHomMat[iMatchingImageIndex], "constant", "false");		// "constant"
							else
								AffineTransImage(*pHAlignImage, pHAlignImage, pHMatchingHomMat[iMatchingImageIndex], "nearest_neighbor", "false");		// "constant"
						}
					}
				}
			}
		}
		else if (iMatchingMethod == ALIGN_MATCHING_PERSPECTIVE)
		{
			for (i = 0; i < MAX_IMAGE_TAB; i++)
			{
				if (pbAlignImageFlag[i] == TRUE)
				{
					if (iAffineTransImageIndex == i)
					{
						if (THEAPP.m_pGFunction->ValidHImage(*pHAlignImage))
							ProjectiveTransImage(*pHAlignImage, pHAlignImage, pHMatchingHomMat[iMatchingImageIndex], "bilinear", "false", "false");	// "nearest_neighbor"
					}
				}
			}
		}

		return TRUE;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [Algorithm::ImageTransformUsingHomMat] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::BlobUnion(HObject *pInputRgn, long lMergeLength)
{
	try
	{
		long lDilationSize;

		lDilationSize = lMergeLength / 2;
		if ((lMergeLength % 2) == 1)
			lDilationSize += 1;

		HObject HConnInputRgn, HUnionInputRgn, HDilatedRgn, HSelectedRgn;

		Connection(*pInputRgn, &HConnInputRgn);

		Union1(*pInputRgn, &HUnionInputRgn);
		DilationCircle(HUnionInputRgn, &HDilatedRgn, (double)lDilationSize + 0.5);
		Connection(HDilatedRgn, &HDilatedRgn);

		Hlong  lNoObj, lNoBlob;
		HObject HUnionRgn;
		HTuple HlNoBlob;

		CountObj(HDilatedRgn, &HlNoBlob);
		lNoBlob = HlNoBlob.L();

		GenEmptyObj(pInputRgn);

		int i, iIndex1, iIndex2;
		HObject HConnectRgn, HObjectSelected1, HObjectSelected2, HConnectLineRgn;
		HTuple HRow, HCol;
		double dMinDistance;
		Hlong lRow1, lRow2, lCol1, lCol2;
		HTuple HlRow1, HlRow2, HlCol1, HlCol2, HdMinDistance;

		for (i = 1; i <= lNoBlob; i++)
		{
			SelectObj(HDilatedRgn, &HSelectedRgn, i);
			SelectShapeProto(HConnInputRgn, HSelectedRgn, &HUnionRgn, "overlaps_abs", 1, MAX_DEF);

			lNoObj = 0;
			HTuple HlNoObj;
			CountObj(HUnionRgn, &HlNoObj);
			lNoObj = HlNoObj.L();

			if (lNoObj > 1)
			{
				GenEmptyObj(&HConnectRgn);
				for (iIndex1 = 1; iIndex1 <= (lNoObj - 1); iIndex1++)
				{
					SelectObj(HUnionRgn, &HObjectSelected1, iIndex1);

					for (iIndex2 = iIndex1 + 1; iIndex2 <= lNoObj; iIndex2++)
					{
						SelectObj(HUnionRgn, &HObjectSelected2, iIndex2);

						DistanceRrMin(HObjectSelected1, HObjectSelected2, &HdMinDistance, &HlRow1, &HlCol1, &HlRow2, &HlCol2);
						dMinDistance = HdMinDistance.D();
						lRow1 = HlRow1.L();
						lCol1 = HlCol1.L();
						lRow2 = HlRow2.L();
						lCol2 = HlCol2.L();

						if (dMinDistance <= (double)lMergeLength)
						{
							TupleGenConst(0, 0, &HRow);
							TupleGenConst(0, 0, &HCol);
							TupleConcat(HRow, HTuple(lRow1), &HRow);
							TupleConcat(HRow, HTuple(lRow2), &HRow);
							TupleConcat(HCol, HTuple(lCol1), &HCol);
							TupleConcat(HCol, HTuple(lCol2), &HCol);
							GenRegionPolygon(&HConnectLineRgn, HRow, HCol);
							ConcatObj(HConnectRgn, HConnectLineRgn, &HConnectRgn);
						}
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HConnectRgn) == TRUE)
					ConcatObj(HUnionRgn, HConnectRgn, &HUnionRgn);

				Union1(HUnionRgn, &HUnionRgn);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HUnionRgn))
				ConcatObj(*pInputRgn, HUnionRgn, pInputRgn);
		}

		return TRUE;

	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [Algorithm::BlobUnion] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::BlobUnionAngle(HObject *pInputRgn, long lMergeLength, long lOrientationMinDiameter)
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (bDebugSave)
		{
			WriteRegion(*pInputRgn, "c:\\DualTest\\BlobUnion.reg");
		}

		long lDilationSize;

		lDilationSize = lMergeLength / 2;
		if ((lMergeLength % 2) == 1)
			lDilationSize += 1;

		HObject HConnInputRgn, HUnionInputRgn, HDilatedRgn, HSelectedRgn;

		Connection(*pInputRgn, &HConnInputRgn);

		Union1(*pInputRgn, &HUnionInputRgn);
		DilationCircle(HUnionInputRgn, &HDilatedRgn, (double)lDilationSize + 0.5);
		Connection(HDilatedRgn, &HDilatedRgn);

		Hlong lNoBlob, lNoObj;
		HObject HUnionRgn;

		HTuple HlNoBlob;
		CountObj(HDilatedRgn, &HlNoBlob);
		lNoBlob = HlNoBlob.L();

		GenEmptyObj(pInputRgn);

		int i, iIndex1, iIndex2;
		HObject HConnectRgn, HObjectSelected1, HObjectSelected2, HConnectLineRgn;
		HTuple HRow, HCol;
		double dMinDistance;
		Hlong lRow1, lRow2, lCol1, lCol2;
		HTuple HdMinDistance, HlRow1, HlRow2, HlCol1, HlCol2;

		Hlong lMaxLengthIndex;
		HTuple HRow1, HRow2, HCol1, HCol2, HDiameter;
		HTuple HIndex, HInverted;
		double dMaxLengthDiameter;
		HObject HMaxDiameterRgn, HDirectionalSelectedRgn, HOrientedRectRgn;
		double dRect2CenterY, dRect2CenterX, dPhi, dLength1, dLength2;
		HTuple HdRect2CenterY, HdRect2CenterX, HdPhi, HdLength1, HdLength2;
		Hlong lNoSelectedObj;

		double dHalfLengthTol = 200.0;
		double dHalfWidthTol = 15.0;

		for (i = 1; i <= lNoBlob; i++)
		{
			SelectObj(HDilatedRgn, &HSelectedRgn, i);
			SelectShapeProto(HConnInputRgn, HSelectedRgn, &HUnionRgn, "overlaps_abs", 1, MAX_DEF);

			lNoObj = 0;
			HTuple HlNoObj;
			CountObj(HUnionRgn, &HlNoObj);
			lNoObj = HlNoObj.L();

			if (lNoObj > 1)
			{
				DiameterRegion(HUnionRgn, &HRow1, &HCol1, &HRow2, &HCol2, &HDiameter);
				TupleSortIndex(HDiameter, &HIndex);
				TupleInverse(HIndex, &HInverted);

				lMaxLengthIndex = HInverted[0].L();
				dMaxLengthDiameter = HDiameter[lMaxLengthIndex].D();
				if (dMaxLengthDiameter < (double)lOrientationMinDiameter)
					continue;

				Union1(HUnionRgn, &HMaxDiameterRgn);
				SmallestRectangle2(HMaxDiameterRgn, &HdRect2CenterY, &HdRect2CenterX, &HdPhi, &HdLength1, &HdLength2);

				dRect2CenterY = HdRect2CenterY.D();
				dRect2CenterX = HdRect2CenterX.D();
				dPhi = HdPhi.D();
				dLength1 = HdLength1.D();
				dLength2 = HdLength2.D();

				GenRectangle2(&HOrientedRectRgn, dRect2CenterY, dRect2CenterX, dPhi, dHalfLengthTol, dHalfWidthTol);

				GenEmptyObj(&HDirectionalSelectedRgn);
				SelectShapeProto(HUnionRgn, HOrientedRectRgn, &HDirectionalSelectedRgn, "overlaps_abs", 1, MAX_DEF);

				lNoSelectedObj = 0;
				HTuple HlNoSelectedObj;
				CountObj(HDirectionalSelectedRgn, &HlNoSelectedObj);
				lNoSelectedObj = HlNoSelectedObj.L();

				if (lNoSelectedObj <= 1)
				{
					ConcatObj(*pInputRgn, HUnionRgn, pInputRgn);
					continue;
				}

				GenEmptyObj(&HConnectRgn);
				for (iIndex1 = 1; iIndex1 <= (lNoSelectedObj - 1); iIndex1++)
				{
					SelectObj(HDirectionalSelectedRgn, &HObjectSelected1, iIndex1);
					for (iIndex2 = iIndex1 + 1; iIndex2 <= lNoSelectedObj; iIndex2++)
					{
						SelectObj(HDirectionalSelectedRgn, &HObjectSelected2, iIndex2);
						DistanceRrMin(HObjectSelected1, HObjectSelected2, &HdMinDistance, &HlRow1, &HlCol1, &HlRow2, &HlCol2);
						dMinDistance = HdMinDistance.D();
						lRow1 = HlRow1.L();
						lCol1 = HlCol1.L();
						lRow2 = HlRow2.L();
						lCol2 = HlCol2.L();

						if (dMinDistance <= (double)lMergeLength)
						{
							TupleGenConst(0, 0, &HRow);
							TupleGenConst(0, 0, &HCol);
							TupleConcat(HRow, HTuple(lRow1), &HRow);
							TupleConcat(HRow, HTuple(lRow2), &HRow);
							TupleConcat(HCol, HTuple(lCol1), &HCol);
							TupleConcat(HCol, HTuple(lCol2), &HCol);
							GenRegionPolygon(&HConnectLineRgn, HRow, HCol);
							ConcatObj(HConnectRgn, HConnectLineRgn, &HConnectRgn);
						}
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HConnectRgn) == TRUE)
					ConcatObj(HDirectionalSelectedRgn, HConnectRgn, &HDirectionalSelectedRgn);

				Union1(HDirectionalSelectedRgn, &HDirectionalSelectedRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HDirectionalSelectedRgn))
					ConcatObj(*pInputRgn, HDirectionalSelectedRgn, pInputRgn);
			}
			else
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HUnionRgn))
					ConcatObj(*pInputRgn, HUnionRgn, pInputRgn);
			}
		}

		return TRUE;

	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [Algorithm::BlobUnionAngle] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::BlobLengthWidthCondition(HObject *pInputRgn, BOOL bUseLengthCondition, BOOL bUseWidthCondition, int iLengthValue, int iWidthValue)
{
	try
	{
		HObject HConnInputRgn, HSelectedRgn;
		Hlong lNoBlob, lNoObj;

		Connection(*pInputRgn, &HConnInputRgn);
		HTuple HlNoBlob;
		CountObj(HConnInputRgn, &HlNoBlob);
		lNoBlob = HlNoBlob.L();
		GenEmptyObj(pInputRgn);

		int i;
		Hlong lRow1, lRow2, lCol1, lCol2;
		HTuple HlRow1, HlRow2, HlCol1, HlCol2;
		double dBlobLength, dBlobWidth;
		HTuple HdBlobLength, HdBlobWidth;
		double dCircleRow, dCircleCol, dCircleRadius;
		HTuple HdCircleRow, HdCircleCol, HdCircleRadius;
		BOOL bLengthConditionPass, bWidthConditionPass;

		for (i = 1; i <= lNoBlob; i++)
		{
			SelectObj(HConnInputRgn, &HSelectedRgn, i);

			if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
				continue;

			bLengthConditionPass = TRUE;
			bWidthConditionPass = TRUE;

			DiameterRegion(HSelectedRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2, &HdBlobLength);
			lRow1 = HlRow1.L();
			lCol1 = HlCol1.L();
			lRow2 = HlRow2.L();
			lCol2 = HlCol2.L();
			dBlobLength = HdBlobLength.D();

			InnerCircle(HSelectedRgn, &HdCircleRow, &HdCircleCol, &HdCircleRadius);
			dCircleRow = HdCircleRow.D();
			dCircleCol = HdCircleCol.D();
			dCircleRadius = HdCircleRadius.D();

			dBlobWidth = dCircleRadius * 2.0;

			if (bUseLengthCondition && (dBlobLength < (double)iLengthValue))
				bLengthConditionPass = FALSE;

			if (bUseWidthCondition && (dBlobWidth < (double)iWidthValue))
				bWidthConditionPass = FALSE;

			if (bLengthConditionPass == TRUE && bWidthConditionPass == TRUE)
				ConcatObj(*pInputRgn, HSelectedRgn, pInputRgn);
		}

		return TRUE;

	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [Algorithm::BlobLengthWidthCondition] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

HObject Algorithm::SurfaceInspectionAlgorithm(int iInspectBufferIndex, HObject HImage, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param, int *piArea, double *pdXLength, double *pdYLength, HObject *pHMaskRgn, double *pdResultDataAry, double *pdLength, double *pdBlobCenterX, double *pdBlobCenterY, double *pdBlobCenterX2, double *pdBlobCenterY2)
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HROIRgn, "c:\\DualTest\\HROIRgn.reg");
		}

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		*piArea = 0;
		*pdXLength = 0;
		*pdYLength = 0;

		if (pdLength != NULL)
			*pdLength = 0;

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(HImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);
		pImageData = (BYTE *)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn.reg");
		}

		HObject HImageReduced;

		HObject HProcessImage;

		HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
		long lPeak, lHystUpperThreshold, lHystLowThreshold;

		if (Param.m_bUseImageScaling)
		{
			int iScaleMin, iScaleMax;
			int iScalePeakUpperOffset = 20;
			int iScalePeakLowerOffset = 10;

			if (Param.m_iImageScaleMethod == IMAGE_SCALE_AUTO)
			{
				GrayHisto(HInspectAreaRgn, HImage, &HAbsHisto, &HRelHisto);
				CreateFunct1dArray(HAbsHisto, &HFunction);
				SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
				TupleLastN(HSmoothFunction, 3, &HAbsHisto);
				TupleSortIndex(HAbsHisto, &Indices);
				TupleInverse(Indices, &Inverted);
				lPeak = Inverted[0].L();

				iScaleMax = lPeak + iScalePeakUpperOffset;
				if (iScaleMax > 255)
					iScaleMax = 255;

				iScaleMin = lPeak - iScalePeakLowerOffset;
				if (iScaleMin < 0)
					iScaleMin = 0;
			}
			else
			{
				if (Param.m_iImageScaleMin < 0)
					iScaleMin = 0;
				else if (Param.m_iImageScaleMin > 255)
					iScaleMin = 255;
				else
					iScaleMin = Param.m_iImageScaleMin;

				if (Param.m_iImageScaleMax < 0)
					iScaleMax = 0;
				else if (Param.m_iImageScaleMax > 255)
					iScaleMax = 255;
				else
					iScaleMax = Param.m_iImageScaleMax;

				if (iScaleMax < iScaleMin)
					iScaleMax = iScaleMin;
			}

			THEAPP.m_pGFunction->ScaleImageRange(HImage, &HProcessImage, HTuple(iScaleMin), HTuple(iScaleMax));
			BinomialFilter(HProcessImage, &HProcessImage, 3, 3);
		}
		else if (Param.m_bUseEdgeImage)
		{
			ReduceDomain(HImage, HInspectAreaRgn, &HImageReduced);

			if (Param.m_iEdgeFilterType == IMAGE_EDGE_ANY_DIR)
				SobelAmp(HImageReduced, &HImageReduced, "sum_abs", 3);
			else if (Param.m_iEdgeFilterType == IMAGE_EDGE_X_DIR)
			{
				SobelAmp(HImageReduced, &HImageReduced, "x", 3);
				AbsImage(HImageReduced, &HImageReduced);
			}
			else if (Param.m_iEdgeFilterType == IMAGE_EDGE_Y_DIR)
			{
				SobelAmp(HImageReduced, &HImageReduced, "y", 3);
				AbsImage(HImageReduced, &HImageReduced);
			}

			if (Param.m_dEdgeImageScale != 1)
				ScaleImage(HImageReduced, &HImageReduced, Param.m_dEdgeImageScale, 0);
		}

		if (Param.m_bUseImageScaling)
			ReduceDomain(HProcessImage, HInspectAreaRgn, &HImageReduced);
		else if (Param.m_bUseEdgeImage)
		{
			;
		}
		else
			ReduceDomain(HImage, HInspectAreaRgn, &HImageReduced);

		if (bDebugSave)
		{
			WriteImage(HImageReduced, "bmp", 0, "c:\\DualTest\\InspectImageReduced");
		}

		int i, j, k;
		HObject HBrightRgn, HDarkRgn, HThresRgn;
		HObject HBrightFixedThresholdConnectedRgn, HDarkFixedThresholdConnectedRgn;

		HObject HDTImageReduced, HExpandReduceImage, HPartImage, HMeanImage, HDynThreshRgn;
		HObject HInvertImageReduced;
		HTuple HTuArea, HTuRow, HTuCol;
		HTuple HUsedThreshold;

		int iii;
		Hlong lNoHisto;
		BOOL bPeakFound = FALSE;

		GenEmptyObj(&HBrightRgn);
		GenEmptyObj(&HDarkRgn);

		GenEmptyObj(&HBrightFixedThresholdConnectedRgn);

		if (Param.m_bUseBrightFixedThres)
		{
			Threshold(HImageReduced, &HThresRgn, Param.m_iBrightLowerThres, Param.m_iBrightUpperThres);
			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
			{
				ConcatObj(HBrightRgn, HThresRgn, &HBrightRgn);

				if (Param.m_bUseFTConnected)
				{
					Connection(HThresRgn, &HBrightFixedThresholdConnectedRgn);

					if (Param.m_bUseFTConnectedArea && Param.m_iFTConnectedAreaMin > 0)
						SelectShape(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn, "area", "and", Param.m_iFTConnectedAreaMin, MAX_DEF);
					if (Param.m_bUseFTConnectedLength && Param.m_iFTConnectedLengthMin > 0)
						SelectShape(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn, "max_diameter", "and", Param.m_iFTConnectedLengthMin, MAX_DEF);
					if (Param.m_bUseFTConnectedWidth && Param.m_iFTConnectedWidthMin > 0)
						SelectShape(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn, "inner_radius", "and", Param.m_iFTConnectedWidthMin / 2, MAX_DEF);

					Union1(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn);
				}
			}
		}

		if (Param.m_bUseBrightDT)
		{
			if (Param.m_bUseDTPolarTrans)
			{
				int iPolarResolution = 640;

				HObject HInspectAreaConnRgn, HInspectROIRgn, HOutRgn, HInRgn;
				HTuple HNoProcessRgn;
				long lNoProcessRgn;
				HTuple dInCenterX, dInCenterY, dInRadius;
				HTuple dOutCenterX, dOutCenterY, dOutRadius;
				HTuple iRingSize, iRadius;
				HObject HImagePolar;

				HDTImageReduced = HImageReduced;

				Connection(HInspectAreaRgn, &HInspectAreaConnRgn);
				CountObj(HInspectAreaConnRgn, &HNoProcessRgn);
				lNoProcessRgn = HNoProcessRgn.L();

				for (i = 0; i < lNoProcessRgn; i++)
				{
					SelectObj(HInspectAreaConnRgn, &HInspectROIRgn, i + 1);

					AreaCenter(HInspectROIRgn, &HTuArea, &HTuRow, &HTuCol);
					if (HTuArea[0].L() < 300)
						continue;

					GenEmptyObj(&HDynThreshRgn);

					FillUp(HInspectROIRgn, &HOutRgn);
					Difference(HOutRgn, HInspectROIRgn, &HInRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInRgn))
					{
						SmallestCircle(HInRgn, &dInCenterY, &dInCenterX, &dInRadius);
						SmallestCircle(HOutRgn, &dOutCenterY, &dOutCenterX, &dOutRadius);
						iRingSize = dOutRadius - dInRadius;
					}
					else
					{
						SmallestCircle(HOutRgn, &dOutCenterY, &dOutCenterX, &dOutRadius);
						iRingSize = dOutRadius;
					}

					TupleInt(iRingSize, &iRingSize);
					iRadius = dOutRadius;
					TupleInt(iRadius, &iRadius);

					if (bDebugSave)
					{
						WriteImage(HDTImageReduced, "bmp", 0, "c:\\DualTest\\DTBrightReducedImage");
					}

					HObject HDTPolarTransRgn;
					PolarTransRegion(HInspectAreaRgn, &HDTPolarTransRgn, dOutCenterY, dOutCenterX, 6.28319, 0, iRadius - iRingSize, iRadius, (Hlong)iPolarResolution, iRingSize, "bilinear");

					if (bDebugSave)
					{
						WriteRegion(HDTPolarTransRgn, "c:\\DualTest\\BrightDTPolarTransRgn.reg");
					}

					PolarTransImageExt(HDTImageReduced, &HPartImage, dOutCenterY, dOutCenterX, 6.28319, 0, iRadius - iRingSize, iRadius, (Hlong)iPolarResolution, iRingSize, "bilinear");

					ReduceDomain(HPartImage, HDTPolarTransRgn, &HPartImage);

					if (Param.m_iBrightMedianFilterSize > 0)
						MedianImage(HPartImage, &HPartImage, "circle", Param.m_iBrightMedianFilterSize, "mirrored");
					if (Param.m_iBrightClosingFilterSize > 0)
						GrayClosingShape(HPartImage, &HPartImage, Param.m_iBrightClosingFilterSize, Param.m_iBrightClosingFilterSize, "octagon");

					if (bDebugSave)
					{
						WriteImage(HPartImage, "bmp", 0, "c:\\DualTest\\DTBrightCircleImage");
					}

					int iFilterX, iFilterY;
					iFilterX = Param.m_iBrightDTFilterSize;
					if (iFilterX <= 0)
						iFilterX = 1;
					iFilterY = Param.m_iBrightDTFilterSize2;
					if (iFilterY <= 0)
						iFilterY = 1;

					int iExpandSize;
					if (iFilterX > iFilterY)
						iExpandSize = iFilterX / 2;
					else
						iExpandSize = iFilterY / 2;

					ExpandDomainGray(HPartImage, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HDTPolarTransRgn, &HExpandReduceImage);

					MeanImage(HExpandReduceImage, &HMeanImage, iFilterX, iFilterY);
					DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue, "light");

					if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
					{
						Union1(HDynThreshRgn, &HDynThreshRgn);

						if (bDebugSave)
						{
							WriteRegion(HDynThreshRgn, "c:\\DualTest\\DTBrightCircleResultRgn.reg");
						}

						PolarTransRegionInv(HDynThreshRgn, &HDynThreshRgn, dOutCenterY, dOutCenterX, 6.28319, 0, iRadius - iRingSize, iRadius, (Hlong)iPolarResolution, iRingSize, lImageWidth, lImageHeight, "bilinear");

						if (bDebugSave)
						{
							WriteRegion(HDynThreshRgn, "c:\\DualTest\\DTBrightCircleResultRgn2.reg");
						}

						Intersection(HDynThreshRgn, HInspectAreaRgn, &HDynThreshRgn);

						if (bDebugSave)
						{
							WriteRegion(HDynThreshRgn, "c:\\DualTest\\DTBrightCircleResultRgn3.reg");
						}

						ConcatObj(HBrightRgn, HDynThreshRgn, &HBrightRgn);
					}
				}
			}
			else
			{
				GenEmptyObj(&HDynThreshRgn);

				HDTImageReduced = HImageReduced;
				if (Param.m_iBrightMedianFilterSize > 0)
					MedianImage(HDTImageReduced, &HDTImageReduced, "circle", Param.m_iBrightMedianFilterSize, "mirrored");
				if (Param.m_iBrightClosingFilterSize > 0)
					GrayClosingShape(HDTImageReduced, &HDTImageReduced, Param.m_iBrightClosingFilterSize, Param.m_iBrightClosingFilterSize, "octagon");

				int iFilterX, iFilterY;
				iFilterX = Param.m_iBrightDTFilterSize;
				if (iFilterX <= 0)
					iFilterX = 1;
				iFilterY = Param.m_iBrightDTFilterSize2;
				if (iFilterY <= 0)
					iFilterY = 1;

				if (Param.m_iBrightDTFilterType == 0)
				{
					int iExpandSize;
					if (Param.m_iBrightDTFilterSize > Param.m_iBrightDTFilterSize2)
						iExpandSize = Param.m_iBrightDTFilterSize / 2;
					else
						iExpandSize = Param.m_iBrightDTFilterSize2 / 2;

					ExpandDomainGray(HDTImageReduced, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);

					MeanImage(HExpandReduceImage, &HMeanImage, iFilterX, iFilterY);
					DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue, "light");
				}
				else
				{
					HObject HCropImage, HDomainRgn;
					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					GetDomain(HDTImageReduced, &HDomainRgn);

					HObject HDomainConnRgn;
					Connection(HDomainRgn, &HDomainConnRgn);

					HTuple HlNoBlob;
					Hlong lNoBlob;
					CountObj(HDomainConnRgn, &HlNoBlob);
					lNoBlob = HlNoBlob.L();

					if (lNoBlob == 1)
					{
						SmallestRectangle1(HDomainRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

						CropDomain(HDTImageReduced, &HCropImage);

						HTuple HCropImageWidth, HCropImageHeight;
						Hlong lCropImageWidth, lCropImageHeight;
						GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
						lCropImageWidth = HCropImageWidth.L();
						lCropImageHeight = HCropImageHeight.L();
						if (iFilterX > lCropImageWidth)
							iFilterX = lCropImageWidth;
						if (iFilterY > lCropImageHeight)
							iFilterY = lCropImageHeight;
						MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);
						DynThreshold(HCropImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue, "light");

						MoveRegion(HDynThreshRgn, &HDynThreshRgn, HlLTPointY, HlLTPointX);
						Intersection(HDomainRgn, HDynThreshRgn, &HDynThreshRgn);
					}
					else
					{
						HObject HSelectedRgn, HMeanCropDynThresRgn;

						GenEmptyObj(&HDynThreshRgn);

						for (i = 0; i < lNoBlob; i++)
						{
							SelectObj(HDomainConnRgn, &HSelectedRgn, i + 1);

							SmallestRectangle1(HSelectedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

							CropRectangle1(HImage, &HCropImage, HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX);

							HTuple HCropImageWidth, HCropImageHeight;
							Hlong lCropImageWidth, lCropImageHeight;
							GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
							lCropImageWidth = HCropImageWidth.L();
							lCropImageHeight = HCropImageHeight.L();
							if (iFilterX > lCropImageWidth)
								iFilterX = lCropImageWidth;
							if (iFilterY > lCropImageHeight)
								iFilterY = lCropImageHeight;
							MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);
							DynThreshold(HCropImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue, "light");

							MoveRegion(HDynThreshRgn, &HMeanCropDynThresRgn, HlLTPointY, HlLTPointX);
							Intersection(HSelectedRgn, HMeanCropDynThresRgn, &HMeanCropDynThresRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HMeanCropDynThresRgn) == TRUE)
							{
								ConcatObj(HDynThreshRgn, HMeanCropDynThresRgn, &HDynThreshRgn);
							}
						}

						if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
							Union1(HDynThreshRgn, &HDynThreshRgn);
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
				{
					ConcatObj(HBrightRgn, HDynThreshRgn, &HBrightRgn);
				}
			}
		}

		if (Param.m_bUseBrightUniformityCheck)
		{
			GrayHisto(HInspectAreaRgn, HImageReduced, &HAbsHisto, &HRelHisto);
			CreateFunct1dArray(HAbsHisto, &HFunction);
			SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
			TupleLastN(HSmoothFunction, 3, &HAbsHisto);
			TupleSortIndex(HAbsHisto, &Indices);
			TupleInverse(Indices, &Inverted);

			HTuple HlNoHisto;
			TupleLength(Inverted, &HlNoHisto);
			lNoHisto = HlNoHisto.L();

			bPeakFound = FALSE;
			for (iii = 0; iii < lNoHisto; iii++)
			{
				lPeak = Inverted[iii].L();
				if (lPeak >= Param.m_iBrightUniformityPeakMin && lPeak <= Param.m_iBrightUniformityPeakMax)
				{
					bPeakFound = TRUE;
					break;
				}
			}

			if (bPeakFound)
			{
				lHystUpperThreshold = lPeak + Param.m_iBrightUniformityOffset;
				if (lHystUpperThreshold > 255)
					lHystUpperThreshold = 255;

				lHystLowThreshold = lHystUpperThreshold - Param.m_iBrightUniformityHystOffset;
				if (lHystLowThreshold < 0)
					lHystLowThreshold = 0;

				if (Param.m_iBrightUniformityHystLength > 0)
					HysteresisThreshold(HImageReduced, &HThresRgn, lHystLowThreshold, lHystUpperThreshold, Param.m_iBrightUniformityHystLength);
				else
					Threshold(HImageReduced, &HThresRgn, lHystUpperThreshold, 255);

				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
					ConcatObj(HBrightRgn, HThresRgn, &HBrightRgn);
			}
		}

		if (Param.m_bUseBrightHystThres)
		{
			if (Param.m_iBrightHystSecureThres >= Param.m_iBrightHystPotentialThres)
			{
				HysteresisThreshold(HImageReduced, &HThresRgn, Param.m_iBrightHystPotentialThres, Param.m_iBrightHystSecureThres, Param.m_iBrightHystPotentialLength);
				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
					ConcatObj(HBrightRgn, HThresRgn, &HBrightRgn);
			}
		}

		if (Param.m_bUseBrightBinThres)
		{
			BinaryThreshold(HImageReduced, &HThresRgn, "max_separability", "light", &HUsedThreshold);
			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
				ConcatObj(HBrightRgn, HThresRgn, &HBrightRgn);
		}

		//////////////////////////////////////////////////////////////////////////
		// Peak Test

		if (Param.m_bUsePeak)
		{
			long lDefectMinThres, lDefectMaxThres;
			HObject HThresNormalRgn;

			GrayHisto(HInspectAreaRgn, HImageReduced, &HAbsHisto, &HRelHisto);
			CreateFunct1dArray(HAbsHisto, &HFunction);
			SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
			TupleLastN(HSmoothFunction, 3, &HAbsHisto);
			TupleSortIndex(HAbsHisto, &Indices);
			TupleInverse(Indices, &Inverted);
			lPeak = Inverted[0].L();

			if (lPeak > Param.m_iNormalPeakThresMax || lPeak < Param.m_iNormalPeakThresMin)
			{
				lDefectMinThres = lPeak - Param.m_iAbnormalPeakHistogramOffset;
				if (lDefectMinThres < 0)
					lDefectMinThres = 0;
				lDefectMaxThres = lPeak + Param.m_iAbnormalPeakHistogramOffset;
				if (lDefectMaxThres > 255)
					lDefectMaxThres = 255;

				Threshold(HImageReduced, &HThresRgn, lDefectMinThres, lDefectMaxThres);
				Threshold(HImageReduced, &HThresNormalRgn, Param.m_iNormalPeakThresMin, Param.m_iNormalPeakThresMax);
				Difference(HThresRgn, HThresNormalRgn, &HThresRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
					ConcatObj(HBrightRgn, HThresRgn, &HBrightRgn);
			}
		}

		//////////////////////////////////////////////////////////////////////////

		GenEmptyObj(&HDarkFixedThresholdConnectedRgn);

		if (Param.m_bUseDarkFixedThres)
		{
			Threshold(HImageReduced, &HThresRgn, Param.m_iDarkLowerThres, Param.m_iDarkUpperThres);
			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
			{
				ConcatObj(HDarkRgn, HThresRgn, &HDarkRgn);

				if (Param.m_bUseFTConnected)
				{
					Connection(HThresRgn, &HDarkFixedThresholdConnectedRgn);

					if (Param.m_bUseFTConnectedArea && Param.m_iFTConnectedAreaMin > 0)
						SelectShape(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn, "area", "and", Param.m_iFTConnectedAreaMin, MAX_DEF);
					if (Param.m_bUseFTConnectedLength && Param.m_iFTConnectedLengthMin > 0)
						SelectShape(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn, "max_diameter", "and", Param.m_iFTConnectedLengthMin, MAX_DEF);
					if (Param.m_bUseFTConnectedWidth && Param.m_iFTConnectedWidthMin > 0)
						SelectShape(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn, "inner_radius", "and", Param.m_iFTConnectedWidthMin / 2, MAX_DEF);

					Union1(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn);
				}
			}
		}

		if (Param.m_bUseDarkDT)
		{
			if (Param.m_bUseDTPolarTrans)
			{
				int iPolarResolution = 640;

				HObject HInspectAreaConnRgn, HInspectROIRgn, HOutRgn, HInRgn;
				HTuple HNoProcessRgn;
				long lNoProcessRgn;
				HTuple dInCenterX, dInCenterY, dInRadius;
				HTuple dOutCenterX, dOutCenterY, dOutRadius;
				HTuple iRingSize, iRadius;
				HObject HImagePolar;

				HDTImageReduced = HImageReduced;

				Connection(HInspectAreaRgn, &HInspectAreaConnRgn);
				CountObj(HInspectAreaConnRgn, &HNoProcessRgn);
				lNoProcessRgn = HNoProcessRgn.L();

				for (i = 0; i < lNoProcessRgn; i++)
				{
					SelectObj(HInspectAreaConnRgn, &HInspectROIRgn, i + 1);

					AreaCenter(HInspectROIRgn, &HTuArea, &HTuRow, &HTuCol);
					if (HTuArea[0].L() < 300)
						continue;

					GenEmptyObj(&HDynThreshRgn);

					FillUp(HInspectROIRgn, &HOutRgn);
					Difference(HOutRgn, HInspectROIRgn, &HInRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInRgn))
					{
						SmallestCircle(HInRgn, &dInCenterY, &dInCenterX, &dInRadius);
						SmallestCircle(HOutRgn, &dOutCenterY, &dOutCenterX, &dOutRadius);
						iRingSize = dOutRadius - dInRadius;
					}
					else
					{
						SmallestCircle(HOutRgn, &dOutCenterY, &dOutCenterX, &dOutRadius);
						iRingSize = dOutRadius;
					}

					TupleInt(iRingSize, &iRingSize);
					iRadius = dOutRadius;
					TupleInt(iRadius, &iRadius);

					HObject HDTPolarTransRgn;
					PolarTransRegion(HInspectAreaRgn, &HDTPolarTransRgn, dOutCenterY, dOutCenterX, 6.28319, 0, iRadius - iRingSize, iRadius, (Hlong)iPolarResolution, iRingSize, "bilinear");

					PolarTransImageExt(HDTImageReduced, &HPartImage, dOutCenterY, dOutCenterX, 6.28319, 0, iRadius - iRingSize, iRadius, (Hlong)iPolarResolution, iRingSize, "bilinear");

					ReduceDomain(HPartImage, HDTPolarTransRgn, &HPartImage);

					if (Param.m_iDarkMedianFilterSize > 0)
						MedianImage(HPartImage, &HPartImage, "circle", Param.m_iDarkMedianFilterSize, "mirrored");
					if (Param.m_iDarkClosingFilterSize > 0)
						GrayClosingShape(HPartImage, &HPartImage, Param.m_iDarkClosingFilterSize, Param.m_iDarkClosingFilterSize, "octagon");

					if (bDebugSave)
					{
						WriteImage(HPartImage, "bmp", 0, "c:\\DualTest\\DTDarkCircleImage");
					}

					int iFilterX, iFilterY;
					iFilterX = Param.m_iDarkDTFilterSize;
					if (iFilterX <= 0)
						iFilterX = 1;
					iFilterY = Param.m_iDarkDTFilterSize2;
					if (iFilterY <= 0)
						iFilterY = 1;

					int iExpandSize;
					if (iFilterX > iFilterY)
						iExpandSize = iFilterX / 2;
					else
						iExpandSize = iFilterY / 2;

					ExpandDomainGray(HPartImage, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HDTPolarTransRgn, &HExpandReduceImage);

					MeanImage(HExpandReduceImage, &HMeanImage, iFilterX, iFilterY);
					DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iDarkDTValue, "dark");

					if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
					{
						Union1(HDynThreshRgn, &HDynThreshRgn);

						if (bDebugSave)
						{
							WriteRegion(HDynThreshRgn, "c:\\DualTest\\DTDarkCircleResultRgn.reg");
						}

						PolarTransRegionInv(HDynThreshRgn, &HDynThreshRgn, dOutCenterY, dOutCenterX, 6.28319, 0, iRadius - iRingSize, iRadius, (Hlong)iPolarResolution, iRingSize, lImageWidth, lImageHeight, "bilinear");

						if (bDebugSave)
						{
							WriteRegion(HDynThreshRgn, "c:\\DualTest\\DTDarkCircleResultRgn2.reg");
						}

						Intersection(HDynThreshRgn, HInspectAreaRgn, &HDynThreshRgn);

						if (bDebugSave)
						{
							WriteRegion(HDynThreshRgn, "c:\\DualTest\\DTDarkCircleResultRgn3.reg");
						}

						ConcatObj(HDarkRgn, HDynThreshRgn, &HDarkRgn);
					}
				}
			}
			else
			{
				GenEmptyObj(&HDynThreshRgn);

				HDTImageReduced = HImageReduced;
				if (Param.m_iDarkMedianFilterSize > 0)
					MedianImage(HDTImageReduced, &HDTImageReduced, "circle", Param.m_iDarkMedianFilterSize, "mirrored");
				if (Param.m_iDarkClosingFilterSize > 0)
					GrayClosingShape(HDTImageReduced, &HDTImageReduced, Param.m_iDarkClosingFilterSize, Param.m_iDarkClosingFilterSize, "octagon");

				int iFilterX, iFilterY;
				iFilterX = Param.m_iDarkDTFilterSize;
				if (iFilterX <= 0)
					iFilterX = 1;
				iFilterY = Param.m_iDarkDTFilterSize2;
				if (iFilterY <= 0)
					iFilterY = 1;

				if (Param.m_iDarkDTFilterType == 0)
				{
					int iExpandSize;
					if (Param.m_iDarkDTFilterSize > Param.m_iDarkDTFilterSize2)
						iExpandSize = Param.m_iDarkDTFilterSize / 2;
					else
						iExpandSize = Param.m_iDarkDTFilterSize2 / 2;

					ExpandDomainGray(HDTImageReduced, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);

					MeanImage(HExpandReduceImage, &HMeanImage, iFilterX, iFilterY);
					DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iDarkDTValue, "dark");
				}
				else
				{
					HObject HCropImage, HDomainRgn;
					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					GetDomain(HDTImageReduced, &HDomainRgn);

					HObject HDomainConnRgn;
					Connection(HDomainRgn, &HDomainConnRgn);

					HTuple HlNoBlob;
					Hlong lNoBlob;
					CountObj(HDomainConnRgn, &HlNoBlob);
					lNoBlob = HlNoBlob.L();

					if (lNoBlob == 1)
					{
						SmallestRectangle1(HDomainRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

						CropDomain(HDTImageReduced, &HCropImage);

						HTuple HCropImageWidth, HCropImageHeight;
						Hlong lCropImageWidth, lCropImageHeight;
						GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
						lCropImageWidth = HCropImageWidth.L();
						lCropImageHeight = HCropImageHeight.L();
						if (iFilterX > lCropImageWidth)
							iFilterX = lCropImageWidth;
						if (iFilterY > lCropImageHeight)
							iFilterY = lCropImageHeight;

						MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);
						DynThreshold(HCropImage, HMeanImage, &HDynThreshRgn, Param.m_iDarkDTValue, "dark");

						MoveRegion(HDynThreshRgn, &HDynThreshRgn, HlLTPointY, HlLTPointX);
						Intersection(HDomainRgn, HDynThreshRgn, &HDynThreshRgn);
					}
					else
					{
						HObject HSelectedRgn, HMeanCropDynThresRgn;

						GenEmptyObj(&HDynThreshRgn);

						for (i = 0; i < lNoBlob; i++)
						{
							SelectObj(HDomainConnRgn, &HSelectedRgn, i + 1);

							SmallestRectangle1(HSelectedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

							CropRectangle1(HImage, &HCropImage, HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX);

							HTuple HCropImageWidth, HCropImageHeight;
							Hlong lCropImageWidth, lCropImageHeight;
							GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
							lCropImageWidth = HCropImageWidth.L();
							lCropImageHeight = HCropImageHeight.L();
							if (iFilterX > lCropImageWidth)
								iFilterX = lCropImageWidth;
							if (iFilterY > lCropImageHeight)
								iFilterY = lCropImageHeight;
							MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);
							DynThreshold(HCropImage, HMeanImage, &HDynThreshRgn, Param.m_iDarkDTValue, "dark");

							MoveRegion(HDynThreshRgn, &HMeanCropDynThresRgn, HlLTPointY, HlLTPointX);
							Intersection(HSelectedRgn, HMeanCropDynThresRgn, &HMeanCropDynThresRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HMeanCropDynThresRgn) == TRUE)
							{
								ConcatObj(HDynThreshRgn, HMeanCropDynThresRgn, &HDynThreshRgn);
							}
						}

						if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
							Union1(HDynThreshRgn, &HDynThreshRgn);

					}

				}

				if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
				{
					ConcatObj(HDarkRgn, HDynThreshRgn, &HDarkRgn);
				}
			}
		}

		if (Param.m_bUseDarkUniformityCheck)
		{
			InvertImage(HImageReduced, &HInvertImageReduced);

			GrayHisto(HInspectAreaRgn, HInvertImageReduced, &HAbsHisto, &HRelHisto);
			CreateFunct1dArray(HAbsHisto, &HFunction);
			SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
			TupleLastN(HSmoothFunction, 3, &HAbsHisto);
			TupleSortIndex(HAbsHisto, &Indices);
			TupleInverse(Indices, &Inverted);

			HTuple HlNoHisto;
			TupleLength(Inverted, &HlNoHisto);
			lNoHisto = HlNoHisto.L();

			long lDarkPeakMin, lDarkPeakMax;
			lDarkPeakMin = 255 - Param.m_iDarkUniformityPeakMax;
			if (lDarkPeakMin < 0)
				lDarkPeakMin = 0;

			lDarkPeakMax = 255 - Param.m_iDarkUniformityPeakMin;
			if (lDarkPeakMax < 0)
				lDarkPeakMax = 0;

			bPeakFound = FALSE;
			for (iii = 0; iii < lNoHisto; iii++)
			{
				lPeak = Inverted[iii].L();
				if (lPeak >= lDarkPeakMin && lPeak <= lDarkPeakMax)
				{
					bPeakFound = TRUE;
					break;
				}
			}

			if (bPeakFound)
			{
				lHystUpperThreshold = lPeak + Param.m_iDarkUniformityOffset;
				if (lHystUpperThreshold > 255)
					lHystUpperThreshold = 255;

				lHystLowThreshold = lHystUpperThreshold - Param.m_iDarkUniformityHystOffset;
				if (lHystLowThreshold < 0)
					lHystLowThreshold = 0;

				if (Param.m_iDarkUniformityHystLength > 0)
					HysteresisThreshold(HInvertImageReduced, &HThresRgn, lHystLowThreshold, lHystUpperThreshold, Param.m_iDarkUniformityHystLength);
				else
					Threshold(HInvertImageReduced, &HThresRgn, lHystUpperThreshold, 255);

				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
					ConcatObj(HDarkRgn, HThresRgn, &HDarkRgn);
			}
		}

		if (Param.m_bUseDarkHystThres)
		{
			if (Param.m_iDarkHystSecureThres <= Param.m_iDarkHystPotentialThres)
			{
				int iTempSecureThres, iTempPotentialThres;
				iTempSecureThres = 255 - Param.m_iDarkHystSecureThres;
				if (iTempSecureThres < 0)
					iTempSecureThres = 0;
				if (iTempSecureThres > 255)
					iTempSecureThres = 255;
				iTempPotentialThres = 255 - Param.m_iDarkHystPotentialThres;
				if (iTempPotentialThres < 0)
					iTempPotentialThres = 0;
				if (iTempPotentialThres > 255)
					iTempPotentialThres = 255;

				if (iTempSecureThres >= iTempPotentialThres)
				{
					InvertImage(HImageReduced, &HInvertImageReduced);

					HysteresisThreshold(HInvertImageReduced, &HThresRgn, iTempPotentialThres, iTempSecureThres, Param.m_iDarkHystPotentialLength);
					if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
						ConcatObj(HDarkRgn, HThresRgn, &HDarkRgn);
				}
			}
		}

		if (Param.m_bUseDarkBinThres)
		{
			BinaryThreshold(HImageReduced, &HThresRgn, "max_separability", "dark", &HUsedThreshold);
			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
				ConcatObj(HDarkRgn, HThresRgn, &HDarkRgn);
		}

		//////////////////////////////////////////////////////////////////////////

		GenEmptyObj(&HInvertImageReduced);

		Union1(HBrightRgn, &HBrightRgn);

		if (Param.m_bUseFTConnected)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HBrightFixedThresholdConnectedRgn) == TRUE)
			{
				Connection(HBrightRgn, &HBrightRgn);
				SelectShapeProto(HBrightRgn, HBrightFixedThresholdConnectedRgn, &HBrightRgn, "overlaps_abs", 1, 9999999);
				Union1(HBrightRgn, &HBrightRgn);
			}
			else
				GenEmptyObj(&HBrightRgn);
		}

		Union1(HDarkRgn, &HDarkRgn);

		if (Param.m_bUseFTConnected)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDarkFixedThresholdConnectedRgn) == TRUE)
			{
				Connection(HDarkRgn, &HDarkRgn);
				SelectShapeProto(HDarkRgn, HDarkFixedThresholdConnectedRgn, &HDarkRgn, "overlaps_abs", 1, 9999999);
				Union1(HDarkRgn, &HDarkRgn);
			}
			else
				GenEmptyObj(&HDarkRgn);
		}

		if (Param.m_bUseBarcodeBlob)
		{
			HObject HBarcodeImage;
			POINT BarcodeLTPoint;
			Hlong lRow1, lRow2, lCol1, lCol2;
			HTuple HlRow1, HlRow2, HlCol1, HlCol2;

			SmallestRectangle1(HInspectAreaRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);
			lRow1 = HlRow1.L();
			lRow2 = HlRow2.L();
			lCol1 = HlCol1.L();
			lCol2 = HlCol2.L();

			BarcodeLTPoint.x = lCol1;
			BarcodeLTPoint.y = lRow1;

			CropRectangle1(HImage, &HBarcodeImage, lRow1, lCol1, lRow2, lCol2);

			HTuple DataCodeHandle, ResultHandles, DecodedDataStrings;
			HObject SymbolXLDs, HBarcodeDetectRgn;
			int iNoBarcodeString = 0;
			HTuple HStrLength;
			int iLength;

			BOOL bBarcodeRegResult = FALSE;
			CString sBarcodeResult;

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

			if (bBarcodeRegResult)
			{
				GenRegionContourXld(SymbolXLDs, &HBarcodeDetectRgn, "filled");
				MoveRegion(HBarcodeDetectRgn, &HBarcodeDetectRgn, BarcodeLTPoint.y, BarcodeLTPoint.x);

				Union2(HBrightRgn, HBarcodeDetectRgn, &HBrightRgn);

				if (pdBlobCenterX != NULL && pdBlobCenterY != NULL && pdBlobCenterX2 != NULL && pdBlobCenterY2 != NULL)
				{
					// Get Barcode Rect's Corner Points
					HTuple HPolygonRows, HPolygonCols, HLength;
					GetRegionPolygon(HBarcodeDetectRgn, 5, &HPolygonRows, &HPolygonCols);

					TupleLength(HPolygonRows, &HLength);

					Hlong lNoPolygon;
					lNoPolygon = HLength[0].L();

					TupleRemove(HPolygonRows, HLength - 1, &HPolygonRows);
					TupleRemove(HPolygonCols, HLength - 1, &HPolygonCols);

					HTuple HSelectedPointRow, HSelectedPointCol;

					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(HBarcodeDetectRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

					HTuple HBarcodeLTCornerX, HBarcodeLTCornerY;
					HTuple HBarcodeRTCornerX, HBarcodeRTCornerY;
					HTuple HBarcodeLBCornerX, HBarcodeLBCornerY;
					HTuple HBarcodeRBCornerX, HBarcodeRBCornerY;

					HTuple HDist;
					HTuple HMinDist = 1000000000;

					HSelectedPointRow = HlLTPointY;
					HSelectedPointCol = HlLTPointX;

					for (i = 0; i < (lNoPolygon - 1); i++)
					{
						DistancePp(HSelectedPointRow, HSelectedPointCol, HPolygonRows[i], HPolygonCols[i], &HDist);
						if (HDist < HMinDist)
						{
							HBarcodeLTCornerX = HPolygonCols[i];
							HBarcodeLTCornerY = HPolygonRows[i];

							HMinDist = HDist;
						}
					}

					HMinDist = 1000000000;

					HSelectedPointRow = HlLTPointY;
					HSelectedPointCol = HlRBPointX;

					for (i = 0; i < (lNoPolygon - 1); i++)
					{
						DistancePp(HSelectedPointRow, HSelectedPointCol, HPolygonRows[i], HPolygonCols[i], &HDist);
						if (HDist < HMinDist)
						{
							HBarcodeRTCornerX = HPolygonCols[i];
							HBarcodeRTCornerY = HPolygonRows[i];

							HMinDist = HDist;
						}
					}

					HMinDist = 1000000000;

					HSelectedPointRow = HlRBPointY;
					HSelectedPointCol = HlLTPointX;

					for (i = 0; i < (lNoPolygon - 1); i++)
					{
						DistancePp(HSelectedPointRow, HSelectedPointCol, HPolygonRows[i], HPolygonCols[i], &HDist);
						if (HDist < HMinDist)
						{
							HBarcodeLBCornerX = HPolygonCols[i];
							HBarcodeLBCornerY = HPolygonRows[i];

							HMinDist = HDist;
						}
					}

					HMinDist = 1000000000;

					HSelectedPointRow = HlRBPointY;
					HSelectedPointCol = HlRBPointX;

					for (i = 0; i < (lNoPolygon - 1); i++)
					{
						DistancePp(HSelectedPointRow, HSelectedPointCol, HPolygonRows[i], HPolygonCols[i], &HDist);
						if (HDist < HMinDist)
						{
							HBarcodeRBCornerX = HPolygonCols[i];
							HBarcodeRBCornerY = HPolygonRows[i];

							HMinDist = HDist;
						}
					}

					*pdBlobCenterX = (HBarcodeLTCornerX + HBarcodeLBCornerX) * 0.5;
					*pdBlobCenterY = (HBarcodeLTCornerY + HBarcodeLBCornerY) * 0.5;
					*pdBlobCenterX2 = (HBarcodeRTCornerX + HBarcodeRBCornerX) * 0.5;
					*pdBlobCenterY2 = (HBarcodeRTCornerY + HBarcodeRBCornerY) * 0.5;
				}	// if (pdBlobCenterX != NULL && pdBlobCenterY != NULL && pdBlobCenterX2 != NULL && pdBlobCenterY2 != NULL)
			}
		}

		HObject HNonInspectAreaRgn;
		GenEmptyObj(&HNonInspectAreaRgn);

		if (Param.m_bUseDC)
		{
			HNonInspectAreaRgn = GetNonInspectionAlgorithm(HDCImage, HInspectAreaRgn, Param);
			if (THEAPP.m_pGFunction->ValidHRegion(HNonInspectAreaRgn))
			{
				Difference(HBrightRgn, HNonInspectAreaRgn, &HBrightRgn);
				Difference(HDarkRgn, HNonInspectAreaRgn, &HDarkRgn);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
		{
			if (pHMaskRgn != NULL)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
					Difference(HBrightRgn, *pHMaskRgn, &HBrightRgn);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
		{
			if (pHMaskRgn != NULL)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
					Difference(HDarkRgn, *pHMaskRgn, &HDarkRgn);
			}
		}

		if (pdLength != NULL)
			HDefectRgn = DefectContidion(iInspectBufferIndex, HImage, HInspectAreaRgn, HBrightRgn, HDarkRgn, Param, piArea, pdXLength, pdYLength, pdLength);
		else
			HDefectRgn = DefectContidion(iInspectBufferIndex, HImage, HInspectAreaRgn, HBrightRgn, HDarkRgn, Param, piArea, pdXLength, pdYLength);

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
		{
			Union1(HDefectRgn, &HDefectRgn);

			if (pHMaskRgn != NULL)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
					Difference(HDefectRgn, *pHMaskRgn, &HDefectRgn);
			}
		}

		if (Param.m_bApplyCircleFitting)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HBoundaryRgn, HCircleFitProcessImageReduced;
				int iEdgeThreshold = Param.m_iCircleFittingEdgeStr;
				int iEdgeMargin = Param.m_iCircleFittingEdgeMargin;
				HObject HEdgeXLD, HCircles, HObjectSelected;
				HTuple HNoXLD, Attrib, HContourLength;
				Hlong lNoContours, lMaxLengthIndex;
				HTuple HCircleRow, HCircleColumn, HCircleRadius, HCircleStartPhi, HCircleEndPhi, HCirclePointOrder;

				if (bDebugSave)
				{
					WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
					WriteObject(HDefectRgn, "c:\\DualTest\\HDefectRgn");
				}

				Boundary(HDefectRgn, &HBoundaryRgn, "inner");

				if (iEdgeMargin > 0)
					DilationCircle(HBoundaryRgn, &HBoundaryRgn, (double)iEdgeMargin + 0.5);

				ReduceDomain(HImage, HBoundaryRgn, &HCircleFitProcessImageReduced);

				ThresholdSubPix(HCircleFitProcessImageReduced, &HEdgeXLD, iEdgeThreshold);

				CountObj(HEdgeXLD, &HNoXLD);
				lNoContours = HNoXLD.L();
				if (lNoContours > 0)
				{
					LengthXld(HEdgeXLD, &HContourLength);
					TupleSortIndex(HContourLength, &HContourLength);
					TupleInverse(HContourLength, &HContourLength);
					lMaxLengthIndex = HContourLength[0].L();
					SelectObj(HEdgeXLD, &HCircles, lMaxLengthIndex + 1);

					if (THEAPP.m_pGFunction->ValidHXLD(HCircles))
					{
						FitCircleContourXld(HCircles, "algebraic", 36, 0, 0, 3, 2, &HCircleRow, &HCircleColumn, &HCircleRadius, &HCircleStartPhi, &HCircleEndPhi, &HCirclePointOrder);
						GenCircleContourXld(&HCircles, HCircleRow, HCircleColumn, HCircleRadius, 0, 6.28318, "positive", 1.0);
						GenRegionContourXld(HCircles, &HDefectRgn, "filled");

						if (pdBlobCenterX != NULL && pdBlobCenterY != NULL && pdBlobCenterX2 == NULL && pdBlobCenterY2 == NULL)
						{
							if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
							{
								*pdBlobCenterY = HCircleRow.D();
								*pdBlobCenterX = HCircleColumn.D();
							}
						}

					}
				}
			}
		}
		else
		{
			if (pdBlobCenterX != NULL && pdBlobCenterY != NULL && pdBlobCenterX2 == NULL && pdBlobCenterY2 == NULL)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					HTuple HlArea, HdCenterY, HdCenterX;

					AreaCenter(HDefectRgn, &HlArea, &HdCenterY, &HdCenterX);
					*pdBlobCenterY = HdCenterY.D();
					*pdBlobCenterX = HdCenterX.D();
				}
			}
		}

		if (Param.m_bUseBlobCornerMeasure)
		{
			Hlong lRow1, lRow2, lCol1, lCol2;
			HTuple HlRow1, HlRow2, HlCol1, HlCol2;

			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn))
			{
				ShapeTrans(HDefectRgn, &HDefectRgn, "rectangle2");

				if (pdBlobCenterX != NULL && pdBlobCenterY != NULL && pdBlobCenterX2 != NULL && pdBlobCenterY2 != NULL)
				{
					// Get Barcode Rect's Corner Points
					HTuple HPolygonRows, HPolygonCols, HLength;

					GetRegionPolygon(HDefectRgn, 5, &HPolygonRows, &HPolygonCols);

					TupleLength(HPolygonRows, &HLength);

					Hlong lNoPolygon;
					lNoPolygon = HLength[0].L();

					TupleRemove(HPolygonRows, HLength - 1, &HPolygonRows);
					TupleRemove(HPolygonCols, HLength - 1, &HPolygonCols);

					HTuple HSelectedPointRow, HSelectedPointCol;

					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(HDefectRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

					HTuple HBarcodeLTCornerX, HBarcodeLTCornerY;
					HTuple HBarcodeRTCornerX, HBarcodeRTCornerY;
					HTuple HBarcodeLBCornerX, HBarcodeLBCornerY;
					HTuple HBarcodeRBCornerX, HBarcodeRBCornerY;

					HTuple HDist;
					HTuple HMinDist = 1000000000;

					HSelectedPointRow = HlLTPointY;
					HSelectedPointCol = HlLTPointX;

					for (i = 0; i < (lNoPolygon - 1); i++)
					{
						DistancePp(HSelectedPointRow, HSelectedPointCol, HPolygonRows[i], HPolygonCols[i], &HDist);
						if (HDist < HMinDist)
						{
							HBarcodeLTCornerX = HPolygonCols[i];
							HBarcodeLTCornerY = HPolygonRows[i];

							HMinDist = HDist;
						}
					}

					HMinDist = 1000000000;

					HSelectedPointRow = HlLTPointY;
					HSelectedPointCol = HlRBPointX;

					for (i = 0; i < (lNoPolygon - 1); i++)
					{
						DistancePp(HSelectedPointRow, HSelectedPointCol, HPolygonRows[i], HPolygonCols[i], &HDist);
						if (HDist < HMinDist)
						{
							HBarcodeRTCornerX = HPolygonCols[i];
							HBarcodeRTCornerY = HPolygonRows[i];

							HMinDist = HDist;
						}
					}

					HMinDist = 1000000000;

					HSelectedPointRow = HlRBPointY;
					HSelectedPointCol = HlLTPointX;

					for (i = 0; i < (lNoPolygon - 1); i++)
					{
						DistancePp(HSelectedPointRow, HSelectedPointCol, HPolygonRows[i], HPolygonCols[i], &HDist);
						if (HDist < HMinDist)
						{
							HBarcodeLBCornerX = HPolygonCols[i];
							HBarcodeLBCornerY = HPolygonRows[i];

							HMinDist = HDist;
						}
					}

					HMinDist = 1000000000;

					HSelectedPointRow = HlRBPointY;
					HSelectedPointCol = HlRBPointX;

					for (i = 0; i < (lNoPolygon - 1); i++)
					{
						DistancePp(HSelectedPointRow, HSelectedPointCol, HPolygonRows[i], HPolygonCols[i], &HDist);
						if (HDist < HMinDist)
						{
							HBarcodeRBCornerX = HPolygonCols[i];
							HBarcodeRBCornerY = HPolygonRows[i];

							HMinDist = HDist;
						}
					}

					*pdBlobCenterX = (HBarcodeLTCornerX + HBarcodeLBCornerX) * 0.5;
					*pdBlobCenterY = (HBarcodeLTCornerY + HBarcodeLBCornerY) * 0.5;
					*pdBlobCenterX2 = (HBarcodeRTCornerX + HBarcodeRBCornerX) * 0.5;
					*pdBlobCenterY2 = (HBarcodeRTCornerY + HBarcodeRBCornerY) * 0.5;
				}	// if (pdBlobCenterX != NULL && pdBlobCenterY != NULL && pdBlobCenterX2 != NULL && pdBlobCenterY2 != NULL)
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////

		if (bDebugSave)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn))
				WriteRegion(HDefectRgn, "c:\\DualTest\\HDefectRgn.reg");
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::SurfaceInspectionAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

int Algorithm::FindHistogramPeak(HObject HImage, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param)
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HROIRgn, "c:\\DualTest\\HROIRgn.reg");
		}

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(HImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);
		pImageData = (BYTE *)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return -1;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn.reg");
		}

		HObject HImageReduced;

		HObject HProcessImage;

		HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
		long lPeak, lHystUpperThreshold, lHystLowThreshold;

		if (Param.m_bUseImageScaling)
		{
			int iScaleMin, iScaleMax;
			int iScalePeakUpperOffset = 20;
			int iScalePeakLowerOffset = 10;

			if (Param.m_iImageScaleMethod == IMAGE_SCALE_AUTO)
			{
				GrayHisto(HInspectAreaRgn, HImage, &HAbsHisto, &HRelHisto);
				CreateFunct1dArray(HAbsHisto, &HFunction);
				SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
				TupleLastN(HSmoothFunction, 3, &HAbsHisto);
				TupleSortIndex(HAbsHisto, &Indices);
				TupleInverse(Indices, &Inverted);
				lPeak = Inverted[0].L();

				iScaleMax = lPeak + iScalePeakUpperOffset;
				if (iScaleMax > 255)
					iScaleMax = 255;

				iScaleMin = lPeak - iScalePeakLowerOffset;
				if (iScaleMin < 0)
					iScaleMin = 0;
			}
			else
			{
				if (Param.m_iImageScaleMin < 0)
					iScaleMin = 0;
				else if (Param.m_iImageScaleMin > 255)
					iScaleMin = 255;
				else
					iScaleMin = Param.m_iImageScaleMin;

				if (Param.m_iImageScaleMax < 0)
					iScaleMax = 0;
				else if (Param.m_iImageScaleMax > 255)
					iScaleMax = 255;
				else
					iScaleMax = Param.m_iImageScaleMax;

				if (iScaleMax < iScaleMin)
					iScaleMax = iScaleMin;
			}

			THEAPP.m_pGFunction->ScaleImageRange(HImage, &HProcessImage, HTuple(iScaleMin), HTuple(iScaleMax));
			BinomialFilter(HProcessImage, &HProcessImage, 3, 3);
		}
		else if (Param.m_bUseEdgeImage)
		{
			ReduceDomain(HImage, HInspectAreaRgn, &HImageReduced);

			if (Param.m_iEdgeFilterType == IMAGE_EDGE_ANY_DIR)
				SobelAmp(HImageReduced, &HImageReduced, "sum_abs", 3);
			else if (Param.m_iEdgeFilterType == IMAGE_EDGE_X_DIR)
			{
				SobelAmp(HImageReduced, &HImageReduced, "x", 3);
				AbsImage(HImageReduced, &HImageReduced);
			}
			else if (Param.m_iEdgeFilterType == IMAGE_EDGE_Y_DIR)
			{
				SobelAmp(HImageReduced, &HImageReduced, "y", 3);
				AbsImage(HImageReduced, &HImageReduced);
			}

			if (Param.m_dEdgeImageScale != 1)
				ScaleImage(HImageReduced, &HImageReduced, Param.m_dEdgeImageScale, 0);
		}

		if (Param.m_bUseImageScaling)
			ReduceDomain(HProcessImage, HInspectAreaRgn, &HImageReduced);
		else if (Param.m_bUseEdgeImage)
		{
			;
		}
		else
			ReduceDomain(HImage, HInspectAreaRgn, &HImageReduced);

		if (bDebugSave)
		{
			WriteImage(HImageReduced, "bmp", 0, "c:\\DualTest\\InspectImageReduced");
		}

		int i, j, k;
		HObject HBrightRgn, HDarkRgn, HThresRgn;
		HObject HBrightFixedThresholdConnectedRgn, HDarkFixedThresholdConnectedRgn;

		HObject HDTImageReduced, HExpandReduceImage, HPartImage, HMeanImage, HDynThreshRgn;
		HObject HInvertImageReduced;
		HTuple HTuArea, HTuRow, HTuCol;

		int iii;
		Hlong lNoHisto;
		BOOL bPeakFound = FALSE;

		int iPeakValue = -1;

		GrayHisto(HInspectAreaRgn, HImageReduced, &HAbsHisto, &HRelHisto);
		CreateFunct1dArray(HAbsHisto, &HFunction);
		SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
		TupleLastN(HSmoothFunction, 3, &HAbsHisto);
		TupleSortIndex(HAbsHisto, &Indices);
		TupleInverse(Indices, &Inverted);

		HTuple HlNoHisto;
		TupleLength(Inverted, &HlNoHisto);
		lNoHisto = HlNoHisto.L();

		for (iii = 0; iii < lNoHisto; iii++)
		{
			lPeak = Inverted[iii].L();
			if (lPeak >= Param.m_iNormalPeakThresMin && lPeak <= Param.m_iNormalPeakThresMax)
			{
				iPeakValue = lPeak;
				break;
			}
		}

		return iPeakValue;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::FindHistogramPeak] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return -1;
	}
}

HObject Algorithm::SurfaceDualInspectionAlgorithm(HObject *pHImage1, HObject *pHImage2, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHMaskRgn)
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (bDebugSave)
		{
			WriteImage(*pHImage1, "bmp", 0, "c:\\DualTest\\DualInspectImage1");
			WriteImage(*pHImage2, "bmp", 0, "c:\\DualTest\\DualInspectImage2");
			WriteRegion(HROIRgn, "c:\\DualTest\\HROIRgn.reg");
		}

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		int piArea = 0;
		double pdXLength = 0;
		double pdYLength = 0;

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(*pHImage1, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);
		pImageData = (BYTE *)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		int i, j, k, iii;
		HObject HBrightRgn1, HBrightRgn2, HDarkRgn, HThresRgn;
		HObject HDTImageReduced, HExpandReduceImage, HPartImage, HMeanImage, HDynThreshRgn;

		GenEmptyObj(&HBrightRgn1);
		GenEmptyObj(&HBrightRgn2);
		GenEmptyObj(&HDarkRgn);

		HObject HImageReduced;

		//////////////////////////////////////////////////	Image 1

		ReduceDomain(*pHImage1, HInspectAreaRgn, &HImageReduced);

		if (bDebugSave)
		{
			WriteImage(HImageReduced, "bmp", 0, "c:\\DualTest\\InspectImageReduced_1");
		}

		if (Param.m_bUseBrightDT_1)
		{
			if (Param.m_bUseDTPolarTrans_1)
			{
				int iPolarResolution = 640;

				HObject HInspectAreaConnRgn, HInspectROIRgn, HOutRgn, HInRgn;
				HTuple HNoProcessRgn;
				long lNoProcessRgn;
				HTuple dInCenterX, dInCenterY, dInRadius;
				HTuple dOutCenterX, dOutCenterY, dOutRadius;
				HTuple iRingSize, iRadius;
				HObject HImagePolar;

				HDTImageReduced = HImageReduced;

				Connection(HInspectAreaRgn, &HInspectAreaConnRgn);
				CountObj(HInspectAreaConnRgn, &HNoProcessRgn);
				lNoProcessRgn = HNoProcessRgn.L();

				for (i = 0; i < lNoProcessRgn; i++)
				{
					SelectObj(HInspectAreaConnRgn, &HInspectROIRgn, i + 1);
					if (THEAPP.m_pGFunction->ValidHRegion(HInspectROIRgn) == FALSE)
						continue;

					GenEmptyObj(&HDynThreshRgn);

					FillUp(HInspectROIRgn, &HOutRgn);
					Difference(HOutRgn, HInspectROIRgn, &HInRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInRgn))
					{
						SmallestCircle(HInRgn, &dInCenterY, &dInCenterX, &dInRadius);
						SmallestCircle(HOutRgn, &dOutCenterY, &dOutCenterX, &dOutRadius);
						iRingSize = dOutRadius - dInRadius;
					}
					else
					{
						SmallestCircle(HOutRgn, &dOutCenterY, &dOutCenterX, &dOutRadius);
						iRingSize = dOutRadius;
					}

					TupleInt(iRingSize, &iRingSize);
					iRadius = dOutRadius;
					TupleInt(iRadius, &iRadius);

					PolarTransImage(HDTImageReduced, &HImagePolar, dOutCenterY, dOutCenterX, (Hlong)iPolarResolution, iRadius + 5);
					CropPart(HImagePolar, &HPartImage, iRadius - iRingSize, 0, iPolarResolution, iRingSize);

					if (Param.m_iBrightMedianFilterSize_1 > 0)
						MedianImage(HPartImage, &HPartImage, "circle", Param.m_iBrightMedianFilterSize_1, "mirrored");
					if (Param.m_iBrightClosingFilterSize_1 > 0)
						GrayClosingShape(HPartImage, &HPartImage, Param.m_iBrightClosingFilterSize_1, Param.m_iBrightClosingFilterSize_1, "octagon");

					int iFilterX, iFilterY;
					iFilterX = Param.m_iBrightDTFilterSize_1;
					if (iFilterX <= 0)
						iFilterX = 1;
					iFilterY = Param.m_iBrightDTFilterSize2_1;
					if (iFilterY <= 0)
						iFilterY = 1;

					MeanImage(HPartImage, &HMeanImage, iFilterX, iFilterY);

					if (Param.m_iSurfaceDualDefectType_1 == DUAL_BLOB_BRIGHT_DEFECT)
						DynThreshold(HPartImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_1, "light");
					else
						DynThreshold(HPartImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_1, "dark");

					if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
					{
						Union1(HDynThreshRgn, &HDynThreshRgn);

						PolarTransRegionInv(HDynThreshRgn, &HDynThreshRgn, dOutCenterY, dOutCenterX, 6.28319, 0, iRadius - iRingSize, iRadius, (Hlong)iPolarResolution, iRingSize, lImageWidth, lImageHeight, "nearest_neighbor");

						Intersection(HDynThreshRgn, HInspectAreaRgn, &HDynThreshRgn);

						ConcatObj(HBrightRgn1, HDynThreshRgn, &HBrightRgn1);
					}
				}
			}
			else
			{
				GenEmptyObj(&HDynThreshRgn);

				HDTImageReduced = HImageReduced;
				if (Param.m_iBrightMedianFilterSize_1 > 0)
					MedianImage(HDTImageReduced, &HDTImageReduced, "circle", Param.m_iBrightMedianFilterSize_1, "mirrored");
				if (Param.m_iBrightClosingFilterSize_1 > 0)
					GrayClosingShape(HDTImageReduced, &HDTImageReduced, Param.m_iBrightClosingFilterSize_1, Param.m_iBrightClosingFilterSize_1, "octagon");

				int iFilterX, iFilterY;
				iFilterX = Param.m_iBrightDTFilterSize_1;
				if (iFilterX <= 0)
					iFilterX = 1;
				iFilterY = Param.m_iBrightDTFilterSize2_1;
				if (iFilterY <= 0)
					iFilterY = 1;

				if (Param.m_iBrightDTFilterType_1 == 0)
				{
					int iExpandSize;
					if (Param.m_iBrightDTFilterSize_1 > Param.m_iBrightDTFilterSize2_1)
						iExpandSize = Param.m_iBrightDTFilterSize_1 / 2;
					else
						iExpandSize = Param.m_iBrightDTFilterSize2_1 / 2;

					ExpandDomainGray(HDTImageReduced, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);

					MeanImage(HExpandReduceImage, &HMeanImage, iFilterX, iFilterY);

					if (Param.m_iSurfaceDualDefectType_1 == DUAL_BLOB_BRIGHT_DEFECT)
						DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_1, "light");
					else
						DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_1, "dark");

				}
				else
				{
					HObject HCropImage, HDomainRgn;
					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					GetDomain(HDTImageReduced, &HDomainRgn);

					HObject HDomainConnRgn;
					Connection(HDomainRgn, &HDomainConnRgn);

					HTuple HlNoBlob;
					Hlong lNoBlob;
					CountObj(HDomainConnRgn, &HlNoBlob);
					lNoBlob = HlNoBlob.L();

					if (lNoBlob == 1)
					{
						SmallestRectangle1(HDomainRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

						CropDomain(HDTImageReduced, &HCropImage);

						HTuple HCropImageWidth, HCropImageHeight;
						Hlong lCropImageWidth, lCropImageHeight;
						GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
						lCropImageWidth = HCropImageWidth.L();
						lCropImageHeight = HCropImageHeight.L();
						if (iFilterX > lCropImageWidth)
							iFilterX = lCropImageWidth;
						if (iFilterY > lCropImageHeight)
							iFilterY = lCropImageHeight;
						MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);

						if (Param.m_iSurfaceDualDefectType_1 == DUAL_BLOB_BRIGHT_DEFECT)
							DynThreshold(HCropImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_1, "light");
						else
							DynThreshold(HCropImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_1, "dark");

						MoveRegion(HDynThreshRgn, &HDynThreshRgn, HlLTPointY, HlLTPointX);
						Intersection(HDomainRgn, HDynThreshRgn, &HDynThreshRgn);
					}
					else
					{
						HObject HSelectedRgn, HMeanCropDynThresRgn;

						GenEmptyObj(&HDynThreshRgn);

						for (i = 0; i < lNoBlob; i++)
						{
							SelectObj(HDomainConnRgn, &HSelectedRgn, i + 1);

							SmallestRectangle1(HSelectedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

							CropRectangle1(*pHImage1, &HCropImage, HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX);

							HTuple HCropImageWidth, HCropImageHeight;
							Hlong lCropImageWidth, lCropImageHeight;
							GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
							lCropImageWidth = HCropImageWidth.L();
							lCropImageHeight = HCropImageHeight.L();
							if (iFilterX > lCropImageWidth)
								iFilterX = lCropImageWidth;
							if (iFilterY > lCropImageHeight)
								iFilterY = lCropImageHeight;
							MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);

							if (Param.m_iSurfaceDualDefectType_1 == DUAL_BLOB_BRIGHT_DEFECT)
								DynThreshold(HCropImage, HMeanImage, &HMeanCropDynThresRgn, Param.m_iBrightDTValue_1, "light");
							else
								DynThreshold(HCropImage, HMeanImage, &HMeanCropDynThresRgn, Param.m_iBrightDTValue_1, "dark");

							MoveRegion(HMeanCropDynThresRgn, &HMeanCropDynThresRgn, HlLTPointY, HlLTPointX);
							Intersection(HDomainRgn, HMeanCropDynThresRgn, &HMeanCropDynThresRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HMeanCropDynThresRgn) == TRUE)
							{
								ConcatObj(HDynThreshRgn, HMeanCropDynThresRgn, &HDynThreshRgn);
							}
						}
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
				{
					ConcatObj(HBrightRgn1, HDynThreshRgn, &HBrightRgn1);
				}
			}
		}

		if (Param.m_bUseBrightFixedThres_1)
		{
			Threshold(HImageReduced, &HThresRgn, Param.m_iBrightLowerThres_1, Param.m_iBrightUpperThres_1);
			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
				ConcatObj(HBrightRgn1, HThresRgn, &HBrightRgn1);
		}

		if (Param.m_bUseBrightHystThres_1)
		{
			if (Param.m_iSurfaceDualDefectType_1 == DUAL_BLOB_BRIGHT_DEFECT)
			{
				if (Param.m_iBrightHystSecureThres_1 >= Param.m_iBrightHystPotentialThres_1)
				{
					HysteresisThreshold(HImageReduced, &HThresRgn, Param.m_iBrightHystPotentialThres_1, Param.m_iBrightHystSecureThres_1, Param.m_iBrightHystPotentialLength_1);
					if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
						ConcatObj(HBrightRgn1, HThresRgn, &HBrightRgn1);
				}
			}
			else
			{
				if (Param.m_iBrightHystSecureThres_1 <= Param.m_iBrightHystPotentialThres_1)
				{
					int iTempSecureThres, iTempPotentialThres;
					iTempSecureThres = 255 - Param.m_iBrightHystSecureThres_1;
					if (iTempSecureThres < 0)
						iTempSecureThres = 0;
					if (iTempSecureThres > 255)
						iTempSecureThres = 255;
					iTempPotentialThres = 255 - Param.m_iBrightHystPotentialThres_1;
					if (iTempPotentialThres < 0)
						iTempPotentialThres = 0;
					if (iTempPotentialThres > 255)
						iTempPotentialThres = 255;

					if (iTempSecureThres >= iTempPotentialThres)
					{
						HObject HInvertImageReduced;

						InvertImage(HImageReduced, &HInvertImageReduced);

						HysteresisThreshold(HInvertImageReduced, &HThresRgn, iTempPotentialThres, iTempSecureThres, Param.m_iBrightHystPotentialLength_1);
						if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
							ConcatObj(HBrightRgn1, HThresRgn, &HBrightRgn1);
					}
				}
			}
		}

		Union1(HBrightRgn1, &HBrightRgn1);

		//////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////	Image 2

		ReduceDomain(*pHImage2, HInspectAreaRgn, &HImageReduced);

		if (bDebugSave)
		{
			WriteImage(HImageReduced, "bmp", 0, "c:\\DualTest\\InspectImageReduced_2");
		}

		if (Param.m_bUseBrightDT_2)
		{
			if (Param.m_bUseDTPolarTrans_2)
			{
				int iPolarResolution = 640;

				HObject HInspectAreaConnRgn, HInspectROIRgn, HOutRgn, HInRgn;
				HTuple HNoProcessRgn;
				long lNoProcessRgn;
				HTuple dInCenterX, dInCenterY, dInRadius;
				HTuple dOutCenterX, dOutCenterY, dOutRadius;
				HTuple iRingSize, iRadius;
				HObject HImagePolar;

				HDTImageReduced = HImageReduced;

				Connection(HInspectAreaRgn, &HInspectAreaConnRgn);
				CountObj(HInspectAreaConnRgn, &HNoProcessRgn);
				lNoProcessRgn = HNoProcessRgn.L();

				for (i = 0; i < lNoProcessRgn; i++)
				{
					SelectObj(HInspectAreaConnRgn, &HInspectROIRgn, i + 1);
					if (THEAPP.m_pGFunction->ValidHRegion(HInspectROIRgn) == FALSE)
						continue;

					GenEmptyObj(&HDynThreshRgn);

					FillUp(HInspectROIRgn, &HOutRgn);
					Difference(HOutRgn, HInspectROIRgn, &HInRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInRgn))
					{
						SmallestCircle(HInRgn, &dInCenterY, &dInCenterX, &dInRadius);
						SmallestCircle(HOutRgn, &dOutCenterY, &dOutCenterX, &dOutRadius);
						iRingSize = dOutRadius - dInRadius;
					}
					else
					{
						SmallestCircle(HOutRgn, &dOutCenterY, &dOutCenterX, &dOutRadius);
						iRingSize = dOutRadius;
					}

					TupleInt(iRingSize, &iRingSize);
					iRadius = dOutRadius;
					TupleInt(iRadius, &iRadius);

					PolarTransImage(HDTImageReduced, &HImagePolar, dOutCenterY, dOutCenterX, (Hlong)iPolarResolution, iRadius + 5);
					CropPart(HImagePolar, &HPartImage, iRadius - iRingSize, 0, iPolarResolution, iRingSize);

					if (Param.m_iBrightMedianFilterSize_2 > 0)
						MedianImage(HPartImage, &HPartImage, "circle", Param.m_iBrightMedianFilterSize_2, "mirrored");
					if (Param.m_iBrightClosingFilterSize_2 > 0)
						GrayClosingShape(HPartImage, &HPartImage, Param.m_iBrightClosingFilterSize_2, Param.m_iBrightClosingFilterSize_2, "octagon");

					int iFilterX, iFilterY;
					iFilterX = Param.m_iBrightDTFilterSize_2;
					if (iFilterX <= 0)
						iFilterX = 1;
					iFilterY = Param.m_iBrightDTFilterSize2_2;
					if (iFilterY <= 0)
						iFilterY = 1;

					MeanImage(HPartImage, &HMeanImage, iFilterX, iFilterY);

					if (Param.m_iSurfaceDualDefectType_2 == DUAL_BLOB_BRIGHT_DEFECT)
						DynThreshold(HPartImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_2, "light");
					else
						DynThreshold(HPartImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_2, "dark");

					if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
					{
						Union1(HDynThreshRgn, &HDynThreshRgn);

						PolarTransRegionInv(HDynThreshRgn, &HDynThreshRgn, dOutCenterY, dOutCenterX, 6.28319, 0, iRadius - iRingSize, iRadius, (Hlong)iPolarResolution, iRingSize, lImageWidth, lImageHeight, "nearest_neighbor");

						Intersection(HDynThreshRgn, HInspectAreaRgn, &HDynThreshRgn);

						ConcatObj(HBrightRgn2, HDynThreshRgn, &HBrightRgn2);
					}
				}
			}
			else
			{
				GenEmptyObj(&HDynThreshRgn);

				HDTImageReduced = HImageReduced;
				if (Param.m_iBrightMedianFilterSize_2 > 0)
					MedianImage(HDTImageReduced, &HDTImageReduced, "circle", Param.m_iBrightMedianFilterSize_2, "mirrored");
				if (Param.m_iBrightClosingFilterSize_2 > 0)
					GrayClosingShape(HDTImageReduced, &HDTImageReduced, Param.m_iBrightClosingFilterSize_2, Param.m_iBrightClosingFilterSize_2, "octagon");

				int iFilterX, iFilterY;
				iFilterX = Param.m_iBrightDTFilterSize_2;
				if (iFilterX <= 0)
					iFilterX = 1;
				iFilterY = Param.m_iBrightDTFilterSize2_2;
				if (iFilterY <= 0)
					iFilterY = 1;

				if (Param.m_iBrightDTFilterType_2 == 0)
				{
					int iExpandSize;
					if (Param.m_iBrightDTFilterSize_2 > Param.m_iBrightDTFilterSize2_2)
						iExpandSize = Param.m_iBrightDTFilterSize_2 / 2;
					else
						iExpandSize = Param.m_iBrightDTFilterSize2_2 / 2;

					ExpandDomainGray(HDTImageReduced, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);

					MeanImage(HExpandReduceImage, &HMeanImage, iFilterX, iFilterY);

					if (Param.m_iSurfaceDualDefectType_2 == DUAL_BLOB_BRIGHT_DEFECT)
						DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_2, "light");
					else
						DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_2, "dark");
				}
				else
				{
					HObject HCropImage, HDomainRgn;
					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					GetDomain(HDTImageReduced, &HDomainRgn);

					HObject HDomainConnRgn;
					Connection(HDomainRgn, &HDomainConnRgn);

					HTuple HlNoBlob;
					Hlong lNoBlob;
					CountObj(HDomainConnRgn, &HlNoBlob);
					lNoBlob = HlNoBlob.L();

					if (lNoBlob == 1)
					{
						SmallestRectangle1(HDomainRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

						CropDomain(HDTImageReduced, &HCropImage);

						HTuple HCropImageWidth, HCropImageHeight;
						Hlong lCropImageWidth, lCropImageHeight;
						GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
						lCropImageWidth = HCropImageWidth.L();
						lCropImageHeight = HCropImageHeight.L();
						if (iFilterX > lCropImageWidth)
							iFilterX = lCropImageWidth;
						if (iFilterY > lCropImageHeight)
							iFilterY = lCropImageHeight;
						MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);

						if (Param.m_iSurfaceDualDefectType_2 == DUAL_BLOB_BRIGHT_DEFECT)
							DynThreshold(HCropImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_2, "light");
						else
							DynThreshold(HCropImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValue_2, "dark");

						MoveRegion(HDynThreshRgn, &HDynThreshRgn, HlLTPointY, HlLTPointX);
						Intersection(HDomainRgn, HDynThreshRgn, &HDynThreshRgn);
					}
					else
					{
						HObject HSelectedRgn, HMeanCropDynThresRgn;

						GenEmptyObj(&HDynThreshRgn);

						for (i = 0; i < lNoBlob; i++)
						{
							SelectObj(HDomainConnRgn, &HSelectedRgn, i + 1);

							SmallestRectangle1(HSelectedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

							CropRectangle1(*pHImage2, &HCropImage, HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX);

							HTuple HCropImageWidth, HCropImageHeight;
							Hlong lCropImageWidth, lCropImageHeight;
							GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
							lCropImageWidth = HCropImageWidth.L();
							lCropImageHeight = HCropImageHeight.L();
							if (iFilterX > lCropImageWidth)
								iFilterX = lCropImageWidth;
							if (iFilterY > lCropImageHeight)
								iFilterY = lCropImageHeight;
							MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);

							if (Param.m_iSurfaceDualDefectType_2 == DUAL_BLOB_BRIGHT_DEFECT)
								DynThreshold(HCropImage, HMeanImage, &HMeanCropDynThresRgn, Param.m_iBrightDTValue_2, "light");
							else
								DynThreshold(HCropImage, HMeanImage, &HMeanCropDynThresRgn, Param.m_iBrightDTValue_2, "dark");

							MoveRegion(HMeanCropDynThresRgn, &HMeanCropDynThresRgn, HlLTPointY, HlLTPointX);
							Intersection(HDomainRgn, HMeanCropDynThresRgn, &HMeanCropDynThresRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HMeanCropDynThresRgn) == TRUE)
							{
								ConcatObj(HDynThreshRgn, HMeanCropDynThresRgn, &HDynThreshRgn);
							}
						}

						if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
							Union1(HDynThreshRgn, &HDynThreshRgn);
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
				{
					ConcatObj(HBrightRgn2, HDynThreshRgn, &HBrightRgn2);
				}
			}
		}

		if (Param.m_bUseBrightFixedThres_2)
		{
			Threshold(HImageReduced, &HThresRgn, Param.m_iBrightLowerThres_2, Param.m_iBrightUpperThres_2);
			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
				ConcatObj(HBrightRgn2, HThresRgn, &HBrightRgn2);
		}

		if (Param.m_bUseBrightHystThres_2)
		{
			if (Param.m_iSurfaceDualDefectType_1 == DUAL_BLOB_BRIGHT_DEFECT)
			{
				if (Param.m_iBrightHystSecureThres_2 >= Param.m_iBrightHystPotentialThres_2)
				{
					HysteresisThreshold(HImageReduced, &HThresRgn, Param.m_iBrightHystPotentialThres_2, Param.m_iBrightHystSecureThres_2, Param.m_iBrightHystPotentialLength_2);
					if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
						ConcatObj(HBrightRgn2, HThresRgn, &HBrightRgn2);
				}
			}
			else
			{
				if (Param.m_iBrightHystSecureThres_2 <= Param.m_iBrightHystPotentialThres_2)
				{
					int iTempSecureThres, iTempPotentialThres;
					iTempSecureThres = 255 - Param.m_iBrightHystSecureThres_2;
					if (iTempSecureThres < 0)
						iTempSecureThres = 0;
					if (iTempSecureThres > 255)
						iTempSecureThres = 255;
					iTempPotentialThres = 255 - Param.m_iBrightHystPotentialThres_2;
					if (iTempPotentialThres < 0)
						iTempPotentialThres = 0;
					if (iTempPotentialThres > 255)
						iTempPotentialThres = 255;

					if (iTempSecureThres >= iTempPotentialThres)
					{
						HObject HInvertImageReduced;

						InvertImage(HImageReduced, &HInvertImageReduced);

						HysteresisThreshold(HInvertImageReduced, &HThresRgn, iTempPotentialThres, iTempSecureThres, Param.m_iBrightHystPotentialLength_2);
						if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
							ConcatObj(HBrightRgn2, HThresRgn, &HBrightRgn2);
					}
				}
			}
		}

		Union1(HBrightRgn2, &HBrightRgn2);

		//////////////////////////////////////////////////////////////////////////////

		HObject HNonInspectAreaRgn;
		GenEmptyObj(&HNonInspectAreaRgn);

		if (Param.m_bUseDC)
		{
			HNonInspectAreaRgn = GetNonInspectionAlgorithm(HDCImage, HInspectAreaRgn, Param);
			if (THEAPP.m_pGFunction->ValidHRegion(HNonInspectAreaRgn))
			{
				Difference(HBrightRgn1, HNonInspectAreaRgn, &HBrightRgn1);
				Difference(HBrightRgn2, HNonInspectAreaRgn, &HBrightRgn2);
			}
		}

		HObject HBrightRgn;
		GenEmptyObj(&HBrightRgn);

		if (Param.m_bDualBlobDefectConditionEach)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn1) == TRUE)
			{
				if (pHMaskRgn != NULL)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
						Difference(HBrightRgn1, *pHMaskRgn, &HBrightRgn1);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn2) == TRUE)
			{
				if (pHMaskRgn != NULL)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
						Difference(HBrightRgn2, *pHMaskRgn, &HBrightRgn2);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
			{
				if (pHMaskRgn != NULL)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
						Difference(HDarkRgn, *pHMaskRgn, &HDarkRgn);
				}
			}

			HObject HDefectRgn_1, HDefectRgn_2;

			HDefectRgn_1 = DefectContidion(0, *pHImage1, HInspectAreaRgn, HBrightRgn1, HDarkRgn, Param, &piArea, &pdXLength, &pdYLength);
			HDefectRgn_2 = DefectContidion(0, *pHImage1, HInspectAreaRgn, HBrightRgn2, HDarkRgn, Param, &piArea, &pdXLength, &pdYLength);

			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn_1) == TRUE)
			{
				Union1(HDefectRgn_1, &HDefectRgn_1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn_2) == TRUE)
			{
				Union1(HDefectRgn_2, &HDefectRgn_2);
			}

			if (Param.m_iDualBlobProcessType == 0)
				Intersection(HDefectRgn_1, HDefectRgn_2, &HDefectRgn);
			else
				Union2(HDefectRgn_1, HDefectRgn_2, &HDefectRgn);

			FillUp(HDefectRgn, &HDefectRgn);
		}
		else
		{
			if (Param.m_iDualBlobProcessType == 0)
				Intersection(HBrightRgn1, HBrightRgn2, &HBrightRgn);
			else
				Union2(HBrightRgn1, HBrightRgn2, &HBrightRgn);

			if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
			{
				if (pHMaskRgn != NULL)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
						Difference(HBrightRgn, *pHMaskRgn, &HBrightRgn);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
			{
				if (pHMaskRgn != NULL)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
						Difference(HDarkRgn, *pHMaskRgn, &HDarkRgn);
				}
			}

			HDefectRgn = DefectContidion(0, *pHImage1, HInspectAreaRgn, HBrightRgn, HDarkRgn, Param, &piArea, &pdXLength, &pdYLength);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
		{
			Union1(HDefectRgn, &HDefectRgn);

			if (pHMaskRgn != NULL)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
					Difference(HDefectRgn, *pHMaskRgn, &HDefectRgn);
			}
		}

		if (bDebugSave)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn))
				WriteRegion(HDefectRgn, "c:\\DualTest\\HDefectRgn.reg");
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::SurfaceDualInspectionAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::GetNonInspectionAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(HImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);
		pImageData = (BYTE *)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		HInspectAreaRgn = HROIRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		HObject HImageReduced;
		HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
		long lPeak, lHystUpperThreshold, lHystLowThreshold;

		ReduceDomain(HImage, HInspectAreaRgn, &HImageReduced);

		int i, j, k;
		HObject HBrightRgn, HDarkRgn, HThresRgn;

		HObject HDTImageReduced, HExpandReduceImage, HPartImage, HMeanImage, HDynThreshRgn;
		HObject HInvertImageReduced;

		int iii;
		Hlong lNoHisto;
		BOOL bPeakFound = FALSE;

		GenEmptyObj(&HBrightRgn);
		GenEmptyObj(&HDarkRgn);

		if (Param.m_bUseBrightDTDC)
		{
			GenEmptyObj(&HDynThreshRgn);

			HDTImageReduced = HImageReduced;
			if (Param.m_iBrightMedianFilterSizeDC > 0)
				MedianImage(HDTImageReduced, &HDTImageReduced, "circle", Param.m_iBrightMedianFilterSizeDC, "mirrored");
			if (Param.m_iBrightClosingFilterSizeDC > 0)
				GrayClosingShape(HDTImageReduced, &HDTImageReduced, Param.m_iBrightClosingFilterSizeDC, Param.m_iBrightClosingFilterSizeDC, "octagon");

			int iFilterX, iFilterY;
			iFilterX = Param.m_iBrightDTFilterSizeDC;
			if (iFilterX <= 0)
				iFilterX = 1;
			iFilterY = Param.m_iBrightDTFilterSizeDC2;
			if (iFilterY <= 0)
				iFilterY = 1;

			if (Param.m_iBrightDTFilterTypeDC == 0)
			{
				int iExpandSize;
				if (Param.m_iBrightDTFilterSizeDC > Param.m_iBrightDTFilterSizeDC2)
					iExpandSize = Param.m_iBrightDTFilterSizeDC / 2;
				else
					iExpandSize = Param.m_iBrightDTFilterSizeDC2 / 2;

				ExpandDomainGray(HDTImageReduced, &HExpandReduceImage, iExpandSize);
				ReduceDomain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);

				MeanImage(HExpandReduceImage, &HMeanImage, iFilterX, iFilterY);
				DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValueDC, "light");
			}
			else
			{
				HObject HCropImage, HDomainRgn;
				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
				GetDomain(HDTImageReduced, &HDomainRgn);

				HObject HDomainConnRgn;
				Connection(HDomainRgn, &HDomainConnRgn);

				HTuple HlNoBlob;
				Hlong lNoBlob;
				CountObj(HDomainConnRgn, &HlNoBlob);
				lNoBlob = HlNoBlob.L();

				if (lNoBlob == 1)
				{
					SmallestRectangle1(HDomainRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

					CropDomain(HDTImageReduced, &HCropImage);

					HTuple HCropImageWidth, HCropImageHeight;
					Hlong lCropImageWidth, lCropImageHeight;
					GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
					lCropImageWidth = HCropImageWidth.L();
					lCropImageHeight = HCropImageHeight.L();
					if (iFilterX > lCropImageWidth)
						iFilterX = lCropImageWidth;
					if (iFilterY > lCropImageHeight)
						iFilterY = lCropImageHeight;
					MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);
					DynThreshold(HCropImage, HMeanImage, &HDynThreshRgn, Param.m_iBrightDTValueDC, "light");

					MoveRegion(HDynThreshRgn, &HDynThreshRgn, HlLTPointY, HlLTPointX);
					Intersection(HDomainRgn, HDynThreshRgn, &HDynThreshRgn);
				}
				else
				{
					HObject HSelectedRgn, HMeanCropDynThresRgn;

					GenEmptyObj(&HDynThreshRgn);

					for (i = 0; i < lNoBlob; i++)
					{
						SelectObj(HDomainConnRgn, &HSelectedRgn, i + 1);

						SmallestRectangle1(HSelectedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

						CropRectangle1(HImage, &HCropImage, HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX);

						HTuple HCropImageWidth, HCropImageHeight;
						Hlong lCropImageWidth, lCropImageHeight;
						GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
						lCropImageWidth = HCropImageWidth.L();
						lCropImageHeight = HCropImageHeight.L();
						if (iFilterX > lCropImageWidth)
							iFilterX = lCropImageWidth;
						if (iFilterY > lCropImageHeight)
							iFilterY = lCropImageHeight;
						MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);
						DynThreshold(HCropImage, HMeanImage, &HMeanCropDynThresRgn, Param.m_iBrightDTValueDC, "light");

						MoveRegion(HMeanCropDynThresRgn, &HMeanCropDynThresRgn, HlLTPointY, HlLTPointX);
						Intersection(HDomainRgn, HMeanCropDynThresRgn, &HMeanCropDynThresRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HMeanCropDynThresRgn) == TRUE)
						{
							ConcatObj(HDynThreshRgn, HMeanCropDynThresRgn, &HDynThreshRgn);
						}
					}

					if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
						Union1(HDynThreshRgn, &HDynThreshRgn);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
			{
				ConcatObj(HBrightRgn, HDynThreshRgn, &HBrightRgn);
			}
		}

		if (Param.m_bUseBrightFixedThresDC)
		{
			Threshold(HImageReduced, &HThresRgn, Param.m_iBrightLowerThresDC, Param.m_iBrightUpperThresDC);
			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
				ConcatObj(HBrightRgn, HThresRgn, &HBrightRgn);
		}

		if (Param.m_bUseBrightUniformityCheckDC)
		{
			GrayHisto(HInspectAreaRgn, HImageReduced, &HAbsHisto, &HRelHisto);
			CreateFunct1dArray(HAbsHisto, &HFunction);
			SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
			TupleLastN(HSmoothFunction, 3, &HAbsHisto);
			TupleSortIndex(HAbsHisto, &Indices);
			TupleInverse(Indices, &Inverted);

			HTuple HlNoHisto;
			TupleLength(Inverted, &HlNoHisto);
			lNoHisto = HlNoHisto.L();

			bPeakFound = FALSE;
			for (iii = 0; iii < lNoHisto; iii++)
			{
				lPeak = Inverted[iii].L();
				if (lPeak >= Param.m_iBrightUniformityPeakMinDC && lPeak <= Param.m_iBrightUniformityPeakMaxDC)
				{
					bPeakFound = TRUE;
					break;
				}
			}

			if (bPeakFound)
			{
				lHystUpperThreshold = lPeak + Param.m_iBrightUniformityOffsetDC;
				if (lHystUpperThreshold > 255)
					lHystUpperThreshold = 255;

				lHystLowThreshold = lHystUpperThreshold - Param.m_iBrightUniformityHystOffsetDC;
				if (lHystLowThreshold < 0)
					lHystLowThreshold = 0;

				if (Param.m_iBrightUniformityHystLengthDC > 0)
					HysteresisThreshold(HImageReduced, &HThresRgn, lHystLowThreshold, lHystUpperThreshold, Param.m_iBrightUniformityHystLengthDC);
				else
					Threshold(HImageReduced, &HThresRgn, lHystUpperThreshold, 255);

				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
					ConcatObj(HBrightRgn, HThresRgn, &HBrightRgn);
			}
		}

		if (Param.m_bUseDarkDTDC)
		{
			GenEmptyObj(&HDynThreshRgn);

			HDTImageReduced = HImageReduced;
			if (Param.m_iDarkMedianFilterSizeDC > 0)
				MedianImage(HDTImageReduced, &HDTImageReduced, "circle", Param.m_iDarkMedianFilterSizeDC, "mirrored");
			if (Param.m_iDarkClosingFilterSizeDC > 0)
				GrayClosingShape(HDTImageReduced, &HDTImageReduced, Param.m_iDarkClosingFilterSizeDC, Param.m_iDarkClosingFilterSizeDC, "octagon");

			int iFilterX, iFilterY;
			iFilterX = Param.m_iDarkDTFilterSizeDC;
			if (iFilterX <= 0)
				iFilterX = 1;
			iFilterY = Param.m_iDarkDTFilterSizeDC2;
			if (iFilterY <= 0)
				iFilterY = 1;

			if (Param.m_iDarkDTFilterTypeDC == 0)
			{
				int iExpandSize;
				if (Param.m_iDarkDTFilterSizeDC > Param.m_iDarkDTFilterSizeDC2)
					iExpandSize = Param.m_iDarkDTFilterSizeDC / 2;
				else
					iExpandSize = Param.m_iDarkDTFilterSizeDC2 / 2;

				ExpandDomainGray(HDTImageReduced, &HExpandReduceImage, iExpandSize);
				ReduceDomain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);

				MeanImage(HExpandReduceImage, &HMeanImage, iFilterX, iFilterY);
				DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iDarkDTValueDC, "dark");
			}
			else
			{
				HObject HCropImage, HDomainRgn;
				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
				GetDomain(HDTImageReduced, &HDomainRgn);

				HObject HDomainConnRgn;
				Connection(HDomainRgn, &HDomainConnRgn);

				HTuple HlNoBlob;
				Hlong lNoBlob;
				CountObj(HDomainConnRgn, &HlNoBlob);
				lNoBlob = HlNoBlob.L();

				if (lNoBlob == 1)
				{
					SmallestRectangle1(HDomainRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

					CropDomain(HDTImageReduced, &HCropImage);

					HTuple HCropImageWidth, HCropImageHeight;
					Hlong lCropImageWidth, lCropImageHeight;
					GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
					lCropImageWidth = HCropImageWidth.L();
					lCropImageHeight = HCropImageHeight.L();
					if (iFilterX > lCropImageWidth)
						iFilterX = lCropImageWidth;
					if (iFilterY > lCropImageHeight)
						iFilterY = lCropImageHeight;

					MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);
					DynThreshold(HCropImage, HMeanImage, &HDynThreshRgn, Param.m_iDarkDTValueDC, "dark");

					MoveRegion(HDynThreshRgn, &HDynThreshRgn, HlLTPointY, HlLTPointX);
					Intersection(HDomainRgn, HDynThreshRgn, &HDynThreshRgn);
				}
				else
				{
					HObject HSelectedRgn, HMeanCropDynThresRgn;

					GenEmptyObj(&HDynThreshRgn);

					for (i = 0; i < lNoBlob; i++)
					{
						SelectObj(HDomainConnRgn, &HSelectedRgn, i + 1);

						SmallestRectangle1(HSelectedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

						CropRectangle1(HImage, &HCropImage, HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX);

						HTuple HCropImageWidth, HCropImageHeight;
						Hlong lCropImageWidth, lCropImageHeight;
						GetImageSize(HCropImage, &HCropImageWidth, &HCropImageHeight);
						lCropImageWidth = HCropImageWidth.L();
						lCropImageHeight = HCropImageHeight.L();
						if (iFilterX > lCropImageWidth)
							iFilterX = lCropImageWidth;
						if (iFilterY > lCropImageHeight)
							iFilterY = lCropImageHeight;

						MeanImage(HCropImage, &HMeanImage, iFilterX, iFilterY);
						DynThreshold(HCropImage, HMeanImage, &HMeanCropDynThresRgn, Param.m_iDarkDTValueDC, "dark");

						MoveRegion(HMeanCropDynThresRgn, &HMeanCropDynThresRgn, HlLTPointY, HlLTPointX);
						Intersection(HDomainRgn, HMeanCropDynThresRgn, &HMeanCropDynThresRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HMeanCropDynThresRgn) == TRUE)
						{
							ConcatObj(HDynThreshRgn, HMeanCropDynThresRgn, &HDynThreshRgn);
						}
					}

					if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
						Union1(HDynThreshRgn, &HDynThreshRgn);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
			{
				ConcatObj(HDarkRgn, HDynThreshRgn, &HDarkRgn);
			}
		}

		if (Param.m_bUseDarkFixedThresDC)
		{
			Threshold(HImageReduced, &HThresRgn, Param.m_iDarkLowerThresDC, Param.m_iDarkUpperThresDC);
			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
				ConcatObj(HDarkRgn, HThresRgn, &HDarkRgn);
		}

		if (Param.m_bUseDarkUniformityCheckDC)
		{
			InvertImage(HImageReduced, &HInvertImageReduced);

			GrayHisto(HInspectAreaRgn, HInvertImageReduced, &HAbsHisto, &HRelHisto);
			CreateFunct1dArray(HAbsHisto, &HFunction);
			SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
			TupleLastN(HSmoothFunction, 3, &HAbsHisto);
			TupleSortIndex(HAbsHisto, &Indices);
			TupleInverse(Indices, &Inverted);

			HTuple HlNoHisto;
			TupleLength(Inverted, &HlNoHisto);
			lNoHisto = HlNoHisto.L();

			long lDarkPeakMin, lDarkPeakMax;
			lDarkPeakMin = 255 - Param.m_iDarkUniformityPeakMaxDC;
			if (lDarkPeakMin < 0)
				lDarkPeakMin = 0;

			lDarkPeakMax = 255 - Param.m_iDarkUniformityPeakMinDC;
			if (lDarkPeakMax < 0)
				lDarkPeakMax = 0;

			bPeakFound = FALSE;
			for (iii = 0; iii < lNoHisto; iii++)
			{
				lPeak = Inverted[iii].L();
				if (lPeak >= lDarkPeakMin && lPeak <= lDarkPeakMax)
				{
					bPeakFound = TRUE;
					break;
				}
			}

			if (bPeakFound)
			{
				lHystUpperThreshold = lPeak + Param.m_iDarkUniformityOffsetDC;
				if (lHystUpperThreshold > 255)
					lHystUpperThreshold = 255;

				lHystLowThreshold = lHystUpperThreshold - Param.m_iDarkUniformityHystOffsetDC;
				if (lHystLowThreshold < 0)
					lHystLowThreshold = 0;

				if (Param.m_iDarkUniformityHystLengthDC > 0)
					HysteresisThreshold(HInvertImageReduced, &HThresRgn, lHystLowThreshold, lHystUpperThreshold, Param.m_iDarkUniformityHystLengthDC);
				else
					Threshold(HInvertImageReduced, &HThresRgn, lHystUpperThreshold, 255);

				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
					ConcatObj(HDarkRgn, HThresRgn, &HDarkRgn);
			}
		}

		//////////////////////////////////////////////////////////////////////////

		GenEmptyObj(&HInvertImageReduced);

		Union1(HBrightRgn, &HBrightRgn);
		Union1(HDarkRgn, &HDarkRgn);

		int iTempArea, iTempXLength, iTempYLength;

		HDefectRgn = DefectContidionDC(HImage, HInspectAreaRgn, HBrightRgn, HDarkRgn, Param, &iTempArea, &iTempXLength, &iTempYLength);

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
		{
			Union1(HDefectRgn, &HDefectRgn);

			if (Param.m_iDontcareRegionMargin > 0)
			{
				DilationCircle(HDefectRgn, &HDefectRgn, (double)Param.m_iDontcareRegionMargin + 0.5);
			}
			else if (Param.m_iDontcareRegionMargin < 0)
			{
				int iTempErosionMargin;
				iTempErosionMargin = -Param.m_iDontcareRegionMargin;
				ErosionCircle(HDefectRgn, &HDefectRgn, (double)iTempErosionMargin + 0.5);
			}
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::GetNonInspectionAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::ImageCompareAlgorithm(int iThreadIdx, int iMzNo, GTRegion* pInspectROIRgn, int iInspectTabIdx, HObject HImage, HObject HROIRgn, CAlgorithmParam Param, int* piArea, double* pdXLength, double* pdYLength, HObject* pHMaskRgn, HObject* pHVariationModelInspectImage)
{
	try
	{
		BOOL bDebugSave = FALSE;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		*piArea = 0;
		*pdXLength = 0;
		*pdYLength = 0;

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);

		HTuple HlOriginLTPointY, HlOriginLTPointX, HlOriginRBPointY, HlOriginRBPointX;
		SmallestRectangle1(HInspectAreaRgn, &HlOriginLTPointY, &HlOriginLTPointX, &HlOriginRBPointY, &HlOriginRBPointX);

		HTuple HomMat2DRotate, HomMat2DRotate2;
		HObject HCropImage;
		GTRegion* pLocalAlignROIRgn;
		double dDeltaAngle = 0;
		BOOL bLocalAlignROIFindCheck = FALSE;
		if (Param.m_bUseImageProcessLocalAlign &&
			Param.m_iImageProcessLocalAlignROIType == LOCAL_ALIGN_MATCHING_ROI_TYPE_MATCHING)
		{
			int iNoInspectROI, iLocalAlignImageNo;
			if (!THEAPP.bImageCompareAutoTrain)
			{
				if (!THEAPP.bImageCompareTeachingDetect)
					iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][m_iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();
				else
					iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][THEAPP.m_iCurTeachVision]->m_pInspectionArea->GetChildTRegionCount();
			}
			else
				iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

			// 1. Reference ROI 기준으로 회전하기 위해 링크되어있는 local align 탐색
			iLocalAlignImageNo = Param.m_iImageProcessLocalAlignImageIndex + 1;
			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				if (!THEAPP.bImageCompareAutoTrain)
				{
					if (!THEAPP.bImageCompareTeachingDetect)
						pLocalAlignROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][m_iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);
					else
						pLocalAlignROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][THEAPP.m_iCurTeachVision]->m_pInspectionArea->GetChildTRegion(iROIIndex);
				}
				else
					pLocalAlignROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pLocalAlignROIRgn == NULL)
					continue;

				if (pLocalAlignROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				if (pLocalAlignROIRgn->miTeachImageIndex == iLocalAlignImageNo)
				{
					if (pLocalAlignROIRgn->miLocalAlignID == Param.m_iImageProcessLocalAlignROINo)
					{
						bLocalAlignROIFindCheck = TRUE;
						break;
					}
				}
			}

			// 2. 연산량 감소를 위해 전체 영상에서 검사 ROI 영역 크롭
			ReduceDomain(HImage, HInspectAreaRgn, &HCropImage);
			CropDomain(HCropImage, &HCropImage);
		}
		else
		{
			ReduceDomain(HImage, HInspectAreaRgn, &HCropImage);
			CropDomain(HCropImage, &HCropImage);
		}

		HObject HDomainRgn;
		GetDomain(HCropImage, &HDomainRgn);

		if (bDebugSave)
			WriteImage(HCropImage, "bmp", 0, "c:\\DualTest\\HCropImage");

		if (bLocalAlignROIFindCheck)
			dDeltaAngle = -pLocalAlignROIRgn->m_dLocalAlignDeltaAngle[iThreadIdx];

		// 3. 이동 및 회전 변환 행렬 계산
		int iTempY = 0;
		int iTempX = 0;
		if (dDeltaAngle != 0)
		{
			HTuple HArea, HCenterX, HCenterY;
			AreaCenter(HCropImage, &HArea, &HCenterY, &HCenterX);

			HomMat2dIdentity(&HomMat2DRotate);
			HomMat2dRotate(HomMat2DRotate, dDeltaAngle, HCenterY, HCenterX, &HomMat2DRotate);
			AffineTransImage(HCropImage, &HCropImage, HomMat2DRotate, "nearest_neighbor", "false");
			AffineTransRegion(HDomainRgn, &HDomainRgn, HomMat2DRotate, "nearest_neighbor");

			HomMat2dIdentity(&HomMat2DRotate2);
			HomMat2dRotate(HomMat2DRotate2, -dDeltaAngle, HCenterY, HCenterX, &HomMat2DRotate2);

			// 4. Local align을 통해 회전된 ROI의 크기 보정
			int CropLTPointY, CropLTPointX, CropRBPointY, CropRBPointX;
			HObject HResizeInspectAreaRgn;
			CropLTPointY = (HCenterY - (HlOriginRBPointY - HlOriginLTPointY) / 2).D();
			CropRBPointY = (HCenterY + (HlOriginRBPointY - HlOriginLTPointY) / 2).D();
			CropLTPointX = (HCenterX - (HlOriginRBPointX - HlOriginLTPointX) / 2).D();
			CropRBPointX = (HCenterX + (HlOriginRBPointX - HlOriginLTPointX) / 2).D();

			int iShiftPixelY, iShiftPixelX;
			iShiftPixelY = CropLTPointY;
			iShiftPixelX = CropLTPointX;
			MoveRegion(HDomainRgn, &HDomainRgn, -iShiftPixelY, -iShiftPixelX);

			if (CropLTPointY != 0)
			{
				CropRBPointY -= CropLTPointY;
				CropLTPointY = 0;
			}
			if (CropLTPointX != 0)
			{
				CropRBPointX -= CropLTPointX;
				CropLTPointX = 0;
			}

			if (pInspectROIRgn->m_iVarModelImageSizeY[iInspectTabIdx] >= 0 &&
				pInspectROIRgn->m_iVarModelImageSizeX[iInspectTabIdx] >= 0 &&
				(CropRBPointY != pInspectROIRgn->m_iVarModelImageSizeY[iInspectTabIdx] ||
				 CropRBPointX != pInspectROIRgn->m_iVarModelImageSizeX[iInspectTabIdx]))
			{
				iTempY = (CropRBPointY - pInspectROIRgn->m_iVarModelImageSizeY[iInspectTabIdx]) / 2;
				CropLTPointY += iTempY;
				CropRBPointY -= iTempY;

				iTempX = (CropRBPointX - pInspectROIRgn->m_iVarModelImageSizeX[iInspectTabIdx]) / 2;
				CropLTPointX += iTempX;
				CropRBPointX -= iTempX;
			}

			GenRectangle1(&HResizeInspectAreaRgn, CropLTPointY, CropLTPointX, CropRBPointY, CropRBPointX);
			ReduceDomain(HCropImage, HResizeInspectAreaRgn, &HCropImage);
			CropDomain(HCropImage, &HCropImage);
		}

		if (bDebugSave)
			WriteImage(HCropImage, "bmp", 0, "c:\\DualTest\\HCropImage");

		BOOL bApplyAutoTrain = FALSE;
		if (THEAPP.Struct_PreferenceStruct.m_iVmTrain == VM_TRAIN_ALL ||
			(THEAPP.Struct_PreferenceStruct.m_iVmTrain == VM_TRAIN_AUTO_OPTION && Param.m_bUseImageCompareAutoTrain))
			bApplyAutoTrain = TRUE;

		if (Param.m_bUseImageCompareEdgeImage)
			SobelAmp(HCropImage, &HCropImage, "sum_abs", 3);

		// 5. 크롭된 영역 검사
		HTuple HImageWidth, HImageHeight;
		GetImageSize(HCropImage, &HImageWidth, &HImageHeight);

		Hlong lImageWidth, lImageHeight;
		lImageWidth = HImageWidth[0].L();
		lImageHeight = HImageHeight[0].L();

		if (THEAPP.bImageCompareAutoTrain)
		{
			if (pInspectROIRgn->m_HVarModelID[iInspectTabIdx] < 0)
			{
				if (Param.m_bUseImageCompareWindow)
				{
					CreateVariationModel(HImageWidth, HImageHeight, "byte", "direct", &pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);
					GenEmptyObj(&(pInspectROIRgn->m_HVarMeanImage[iInspectTabIdx]));
					GenEmptyObj(&(pInspectROIRgn->m_HVarStdevImage[iInspectTabIdx]));
					pInspectROIRgn->m_iVarNoTrainSample[iInspectTabIdx] = 0;
				}
				else
					CreateVariationModel(HImageWidth, HImageHeight, "byte", "standard", &pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);

				pInspectROIRgn->m_iVarModelImageSizeX[iInspectTabIdx] = HImageWidth[0].L();
				pInspectROIRgn->m_iVarModelImageSizeY[iInspectTabIdx] = HImageHeight[0].L();
			}
			else
			{
				HObject HLowerThresImage, HUpperThresImage;
				HTuple HLowerThresImageWidth, HLowerThresImageHeight;
				GetThreshImagesVariationModel(&HLowerThresImage, &HUpperThresImage, pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);
				GetImageSize(HLowerThresImage, &HLowerThresImageWidth, &HLowerThresImageHeight);
				pInspectROIRgn->m_iVarModelImageSizeX[iInspectTabIdx] = HLowerThresImageWidth[0].L();
				pInspectROIRgn->m_iVarModelImageSizeY[iInspectTabIdx] = HLowerThresImageHeight[0].L();

				if ((pInspectROIRgn->m_iVarModelImageSizeX[iInspectTabIdx] != HImageWidth[0].L()) || (pInspectROIRgn->m_iVarModelImageSizeY[iInspectTabIdx] != HImageHeight[0].L()))
				{
					if (pInspectROIRgn->m_iVarModelImageSizeX > 0 && pInspectROIRgn->m_iVarModelImageSizeY > 0)
						ZoomImageSize(HCropImage, &HCropImage, pInspectROIRgn->m_iVarModelImageSizeX[iInspectTabIdx], pInspectROIRgn->m_iVarModelImageSizeY[iInspectTabIdx], "nearest_neighbor");
				}
			}

			if (Param.m_bUseImageCompareWindow)
				TrainVariationModelWindow(HCropImage, pInspectROIRgn, iInspectTabIdx, Param.m_iImageCompareWindowSize);
			else
				TrainVariationModel(HCropImage, pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);

			HTuple HAbsThreshold, HVarThreshold;
			TupleGenConst(0, 0, &HAbsThreshold);
			TupleGenConst(0, 0, &HVarThreshold);

			TupleConcat(HAbsThreshold, HTuple(Param.m_iImageCompareBrightAbs), &HAbsThreshold);
			TupleConcat(HAbsThreshold, HTuple(Param.m_iImageCompareDarkAbs), &HAbsThreshold);

			TupleConcat(HVarThreshold, HTuple(Param.m_dImageCompareBrightVar), &HVarThreshold);
			TupleConcat(HVarThreshold, HTuple(Param.m_dImageCompareDarkVar), &HVarThreshold);

			if (Param.m_bUseImageCompareWindow)
				PrepareDirectVariationModel(pInspectROIRgn->m_HVarMeanImage[iInspectTabIdx], pInspectROIRgn->m_HVarStdevImage[iInspectTabIdx], pInspectROIRgn->m_HVarModelID[iInspectTabIdx], HAbsThreshold, HVarThreshold);
			else
				PrepareVariationModel(pInspectROIRgn->m_HVarModelID[iInspectTabIdx], HAbsThreshold, HVarThreshold);

			THEAPP.bVariationModelSaveCheck[m_iVisionCamType] = TRUE;

			return HDefectRgn;
		}

		if (bApplyAutoTrain)
		{
			if (pInspectROIRgn->m_HVarModelID[iInspectTabIdx] < 0)
			{
				if (Param.m_bUseImageCompareWindow)
				{
					CreateVariationModel(HImageWidth, HImageHeight, "byte", "direct", &pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);
					GenEmptyObj(&(pInspectROIRgn->m_HVarMeanImage[iInspectTabIdx]));
					GenEmptyObj(&(pInspectROIRgn->m_HVarStdevImage[iInspectTabIdx]));
					pInspectROIRgn->m_iVarNoTrainSample[iInspectTabIdx] = 0;
				}
				else
					CreateVariationModel(HImageWidth, HImageHeight, "byte", "standard", &pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);

				pInspectROIRgn->m_iVarModelImageSizeX[iInspectTabIdx] = HImageWidth[0].L();
				pInspectROIRgn->m_iVarModelImageSizeY[iInspectTabIdx] = HImageHeight[0].L();

				if (Param.m_bUseImageCompareWindow)
					TrainVariationModelWindow(HCropImage, pInspectROIRgn, iInspectTabIdx, Param.m_iImageCompareWindowSize);
				else
					TrainVariationModel(HCropImage, pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);

				return HDefectRgn;
			}
		}

		int iVarModelImageSizeX, iVarModelImageSizeY;
		iVarModelImageSizeX = pInspectROIRgn->m_iVarModelImageSizeX[iInspectTabIdx];
		iVarModelImageSizeY = pInspectROIRgn->m_iVarModelImageSizeY[iInspectTabIdx];
		if ((iVarModelImageSizeX != lImageWidth) || (iVarModelImageSizeY != lImageHeight))
		{
			if (iVarModelImageSizeX > 0 && iVarModelImageSizeY > 0)
				ZoomImageSize(HCropImage, &HCropImage, iVarModelImageSizeX, iVarModelImageSizeY, "nearest_neighbor");
		}

		// 밝기 편차 보정
		if (Param.m_bUseImageCompareNormalizer)
		{
			HObject HMeanImage, HVarImage, HNormalized;
			GetVariationModel(&HMeanImage, &HVarImage, pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);

			HObject HROIMask;
			GenRectangle1(&HROIMask, 0, 0, iVarModelImageSizeY - 1, iVarModelImageSizeX - 1);

			VariationNormalizerParam var_norm_param;
			var_norm_param.sigma_b = Param.m_iImageCompareNormalizeWeight;
	
			CVariationNormalizer normalizer;
			normalizer.ApplyAdaptiveNormalization(HCropImage, HMeanImage, HROIMask, var_norm_param, &HCropImage);
		}
		
		*pHVariationModelInspectImage = HCropImage;

		if (pInspectROIRgn->m_HVarModelID[iInspectTabIdx] < 0)
			return HDefectRgn;

		if (bDebugSave)
			WriteImage(HCropImage, "bmp", 0, "c:\\DualTest\\HCropImage2");

		if (THEAPP.Struct_PreferenceStruct.m_bUseVmThresOpt)
		{
			HObject HMeanImage, HVarImage;
			HObject HDiffSignedImage, HLowVarMaskRgn, HHighVarMaskRgn, HBrightLowVarMaskRgn, HBrightHighVarMaskRgn, HDarkLowVarMaskRgn, HDarkHighVarMaskRgn, HBrightDiffRgn, HDarkDiffRgn;
			HTuple HRow, HCol, HGrayLow, HGrayHigh, HVarHigh, HAbsDiffLow, HAbsDiffHigh, HRatioDiff;

			GetVariationModel(&HMeanImage, &HVarImage, pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);
			ConvertImageType(HVarImage, &HVarImage, "byte");

			SubImage(HCropImage, HMeanImage, &HDiffSignedImage, 1, 128);
			Threshold(HDiffSignedImage, &HBrightDiffRgn, 129, 255);
			Threshold(HDiffSignedImage, &HDarkDiffRgn, 0, 127);

			Threshold(HVarImage, &HLowVarMaskRgn, 0, THEAPP.Struct_PreferenceStruct.m_iVmSplitThres);
			Threshold(HVarImage, &HHighVarMaskRgn, THEAPP.Struct_PreferenceStruct.m_iVmSplitThres + 1, 255);

			Intersection(HDomainRgn, HLowVarMaskRgn, &HLowVarMaskRgn);
			Intersection(HDomainRgn, HHighVarMaskRgn, &HHighVarMaskRgn);

			Intersection(HLowVarMaskRgn, HBrightDiffRgn, &HBrightLowVarMaskRgn);
			Intersection(HHighVarMaskRgn, HBrightDiffRgn, &HBrightHighVarMaskRgn);
			Intersection(HLowVarMaskRgn, HDarkDiffRgn, &HDarkLowVarMaskRgn);
			Intersection(HHighVarMaskRgn, HDarkDiffRgn, &HDarkHighVarMaskRgn);

			GetRegionPoints(HBrightLowVarMaskRgn, &HRow, &HCol);
			GetGrayval(HDiffSignedImage, HRow, HCol, &HGrayLow);
			TupleAbs(HGrayLow - 128, &HAbsDiffLow);
			TupleConcat(pInspectROIRgn->m_HBrightAbsDiffs[iInspectTabIdx], HAbsDiffLow, &pInspectROIRgn->m_HBrightAbsDiffs[iInspectTabIdx]);

			GetRegionPoints(HBrightHighVarMaskRgn, &HRow, &HCol);
			GetGrayval(HDiffSignedImage, HRow, HCol, &HGrayHigh);
			GetGrayval(HVarImage, HRow, HCol, &HVarHigh);
			TupleAbs(HGrayHigh - 128, &HAbsDiffHigh);
			TupleDiv(HAbsDiffHigh, HVarHigh, &HRatioDiff);
			TupleConcat(pInspectROIRgn->m_HBrightRatioDiffs[iInspectTabIdx], HRatioDiff, &pInspectROIRgn->m_HBrightRatioDiffs[iInspectTabIdx]);

			GetRegionPoints(HDarkLowVarMaskRgn, &HRow, &HCol);
			GetGrayval(HDiffSignedImage, HRow, HCol, &HGrayLow);
			TupleAbs(HGrayLow - 128, &HAbsDiffLow);
			TupleConcat(pInspectROIRgn->m_HDarkAbsDiffs[iInspectTabIdx], HAbsDiffLow, &pInspectROIRgn->m_HDarkAbsDiffs[iInspectTabIdx]);

			GetRegionPoints(HDarkHighVarMaskRgn, &HRow, &HCol);
			GetGrayval(HDiffSignedImage, HRow, HCol, &HGrayHigh);
			GetGrayval(HVarImage, HRow, HCol, &HVarHigh);
			TupleAbs(HGrayHigh - 128, &HAbsDiffHigh);
			TupleDiv(HAbsDiffHigh, HVarHigh, &HRatioDiff);
			TupleConcat(pInspectROIRgn->m_HDarkRatioDiffs[iInspectTabIdx], HRatioDiff, &pInspectROIRgn->m_HDarkRatioDiffs[iInspectTabIdx]);
		}

		HObject HBrightRgn, HDarkRgn, HThresRgn;
		HObject HTempBrightRgn, HTempDarkRgn;
		HObject HModInspectRgn;

		GenEmptyObj(&HBrightRgn);
		GenEmptyObj(&HDarkRgn);
		GenEmptyObj(&HThresRgn);

		HTuple HAbsThreshold, HVarThreshold;
		TupleGenConst(0, 0, &HAbsThreshold);
		TupleGenConst(0, 0, &HVarThreshold);

		TupleConcat(HAbsThreshold, HTuple(Param.m_iImageCompareBrightAbs), &HAbsThreshold);
		TupleConcat(HAbsThreshold, HTuple(Param.m_iImageCompareDarkAbs), &HAbsThreshold);

		TupleConcat(HVarThreshold, HTuple(Param.m_dImageCompareBrightVar), &HVarThreshold);
		TupleConcat(HVarThreshold, HTuple(Param.m_dImageCompareDarkVar), &HVarThreshold);

		if (Param.m_bUseImageCompareWindow)
			PrepareDirectVariationModel(pInspectROIRgn->m_HVarMeanImage[iInspectTabIdx], pInspectROIRgn->m_HVarStdevImage[iInspectTabIdx], pInspectROIRgn->m_HVarModelID[iInspectTabIdx], HAbsThreshold, HVarThreshold);
		else
			PrepareVariationModel(pInspectROIRgn->m_HVarModelID[iInspectTabIdx], HAbsThreshold, HVarThreshold);

		if (Param.m_bUseImageCompareBright == TRUE && Param.m_bUseImageCompareDark == TRUE)
		{
			CompareExtVariationModel(HCropImage, &HThresRgn, pInspectROIRgn->m_HVarModelID[iInspectTabIdx], "light_dark");
			SelectObj(HThresRgn, &HTempBrightRgn, 1);
			SelectObj(HThresRgn, &HTempDarkRgn, 2);

			if (THEAPP.m_pGFunction->ValidHRegion(HTempBrightRgn))
				ConcatObj(HBrightRgn, HTempBrightRgn, &HBrightRgn);

			if (THEAPP.m_pGFunction->ValidHRegion(HTempDarkRgn))
				ConcatObj(HDarkRgn, HTempDarkRgn, &HDarkRgn);
		}
		else if (Param.m_bUseImageCompareBright == TRUE && Param.m_bUseImageCompareDark == FALSE)
		{
			CompareExtVariationModel(HCropImage, &HTempBrightRgn, pInspectROIRgn->m_HVarModelID[iInspectTabIdx], "light");

			if (THEAPP.m_pGFunction->ValidHRegion(HTempBrightRgn))
				ConcatObj(HBrightRgn, HTempBrightRgn, &HBrightRgn);
		}
		else if (Param.m_bUseImageCompareBright == FALSE && Param.m_bUseImageCompareDark == TRUE)
		{
			CompareExtVariationModel(HCropImage, &HTempDarkRgn, pInspectROIRgn->m_HVarModelID[iInspectTabIdx], "dark");

			if (THEAPP.m_pGFunction->ValidHRegion(HTempDarkRgn))
				ConcatObj(HDarkRgn, HTempDarkRgn, &HDarkRgn);
		}

		if (Param.m_bUseImageCompareMod)
		{
			GenRectangle1(&HModInspectRgn, Param.m_iImageCompareModTop, Param.m_iImageCompareModLeft, iVarModelImageSizeY - 1 - Param.m_iImageCompareModBottom, iVarModelImageSizeX - 1 - Param.m_iImageCompareModRight);

			if (THEAPP.m_pGFunction->ValidHRegion(HModInspectRgn))
			{
				TupleGenConst(0, 0, &HAbsThreshold);
				TupleGenConst(0, 0, &HVarThreshold);

				TupleConcat(HAbsThreshold, HTuple(Param.m_iImageCompareModBrightAbs), &HAbsThreshold);
				TupleConcat(HAbsThreshold, HTuple(Param.m_iImageCompareModDarkAbs), &HAbsThreshold);

				TupleConcat(HVarThreshold, HTuple(Param.m_dImageCompareModBrightVar), &HVarThreshold);
				TupleConcat(HVarThreshold, HTuple(Param.m_dImageCompareModDarkVar), &HVarThreshold);

				if (Param.m_bUseImageCompareWindow)
					PrepareDirectVariationModel(pInspectROIRgn->m_HVarMeanImage[iInspectTabIdx], pInspectROIRgn->m_HVarStdevImage[iInspectTabIdx], pInspectROIRgn->m_HVarModelID[iInspectTabIdx], HAbsThreshold, HVarThreshold);
				else
					PrepareVariationModel(pInspectROIRgn->m_HVarModelID[iInspectTabIdx], HAbsThreshold, HVarThreshold);

				if (Param.m_bUseImageCompareModBright == TRUE && Param.m_bUseImageCompareModDark == TRUE)
				{
					CompareExtVariationModel(HCropImage, &HThresRgn, pInspectROIRgn->m_HVarModelID[iInspectTabIdx], "light_dark");
					SelectObj(HThresRgn, &HTempBrightRgn, 1);
					SelectObj(HThresRgn, &HTempDarkRgn, 2);

					Intersection(HTempBrightRgn, HModInspectRgn, &HTempBrightRgn);
					Intersection(HTempDarkRgn, HModInspectRgn, &HTempDarkRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HTempBrightRgn))
						ConcatObj(HBrightRgn, HTempBrightRgn, &HBrightRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HTempDarkRgn))
						ConcatObj(HDarkRgn, HTempDarkRgn, &HDarkRgn);
				}
				else if (Param.m_bUseImageCompareModBright == TRUE && Param.m_bUseImageCompareModDark == FALSE)
				{
					CompareExtVariationModel(HCropImage, &HTempBrightRgn, pInspectROIRgn->m_HVarModelID[iInspectTabIdx], "light");

					Intersection(HTempBrightRgn, HModInspectRgn, &HTempBrightRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HTempBrightRgn))
						ConcatObj(HBrightRgn, HTempBrightRgn, &HBrightRgn);
				}
				else if (Param.m_bUseImageCompareModBright == FALSE && Param.m_bUseImageCompareModDark == TRUE)
				{
					CompareExtVariationModel(HCropImage, &HTempDarkRgn, pInspectROIRgn->m_HVarModelID[iInspectTabIdx], "dark");

					Intersection(HTempDarkRgn, HModInspectRgn, &HTempDarkRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HTempDarkRgn))
						ConcatObj(HDarkRgn, HTempDarkRgn, &HDarkRgn);
				}
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
		{
			Union1(HBrightRgn, &HBrightRgn);

			if (dDeltaAngle != 0)
				AffineTransRegion(HBrightRgn, &HBrightRgn, HomMat2DRotate2, "nearest_neighbor");

			HTuple HlInspectionLTPointY, HlInspectionLTPointX, Trash;
			SmallestRectangle1(HInspectAreaRgn, &HlInspectionLTPointY, &HlInspectionLTPointX, &Trash, &Trash);
			MoveRegion(HBrightRgn, &HBrightRgn, HlInspectionLTPointY + iTempY, HlInspectionLTPointX + iTempX);

			if (pHMaskRgn != NULL)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
					Difference(HBrightRgn, *pHMaskRgn, &HBrightRgn);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
		{
			Union1(HDarkRgn, &HDarkRgn);

			if (dDeltaAngle != 0)
				AffineTransRegion(HDarkRgn, &HDarkRgn, HomMat2DRotate2, "nearest_neighbor");

			HTuple HlInspectionLTPointY, HlInspectionLTPointX, Trash;
			SmallestRectangle1(HInspectAreaRgn, &HlInspectionLTPointY, &HlInspectionLTPointX, &Trash, &Trash);
			MoveRegion(HDarkRgn, &HDarkRgn, HlInspectionLTPointY + iTempY, HlInspectionLTPointX + iTempX);

			if (pHMaskRgn != NULL)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
					Difference(HDarkRgn, *pHMaskRgn, &HDarkRgn);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == FALSE && THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == FALSE)
			return HDefectRgn;

		HDefectRgn = DefectContidion(0, HImage, HInspectAreaRgn, HBrightRgn, HDarkRgn, Param, piArea, pdXLength, pdYLength);

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
		{
			Union1(HDefectRgn, &HDefectRgn);

			if (pHMaskRgn != NULL)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(*pHMaskRgn) == TRUE)
					Difference(HDefectRgn, *pHMaskRgn, &HDefectRgn);
			}

			if (bApplyAutoTrain && THEAPP.Struct_PreferenceStruct.m_bVmTrainAll)
			{
				if (Param.m_bUseImageCompareWindow)
					TrainVariationModelWindow(HCropImage, pInspectROIRgn, iInspectTabIdx, Param.m_iImageCompareWindowSize);
				else
					TrainVariationModel(HCropImage, pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);
			}
		}
		else
		{
			if (bApplyAutoTrain)
			{
				if (Param.m_bUseImageCompareWindow)
					TrainVariationModelWindow(HCropImage, pInspectROIRgn, iInspectTabIdx, Param.m_iImageCompareWindowSize);
				else
					TrainVariationModel(HCropImage, pInspectROIRgn->m_HVarModelID[iInspectTabIdx]);
			}
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ImageCompareAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::ComponentAlgorithm(int iThreadIdx, int iMzNo, GTRegion *pInspectROIRgn, int iInspectTabIdx, HObject HImage, HObject HROIRgn, CAlgorithmParam Param, int *piComponentDefectType, HObject *pHMaskRgn)
{
	try
	{
		BOOL bDebugSave = FALSE;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		*piComponentDefectType = -1;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		int iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][m_iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();

		GTRegion *pLocalAlignROIRgn;

		BOOL bFound = FALSE;
		int iLocalAlignImageNo = Param.m_iImageProcessLocalAlignImageIndex + 1;

		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			pLocalAlignROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][m_iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pLocalAlignROIRgn == NULL)
				continue;

			if (pLocalAlignROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
				continue;

			if (pLocalAlignROIRgn->miTeachImageIndex == iLocalAlignImageNo)
			{
				if (pLocalAlignROIRgn->miLocalAlignID == Param.m_iImageProcessLocalAlignROINo)
				{
					bFound = TRUE;
					break;
				}
			}
		}

		if (bFound == FALSE)
			return HDefectRgn;

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\HImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\HInspectAreaRgn.reg");
		}

		HObject HAffineCropRgn, HAffineCropImage, HAffineTransImage, HCropInspectAreaRgn;
		Hlong lCropLTPointY, lCropLTPointX, lCropRBPointY, lCropRBPointX;
		HTuple HomMat2DLocal;
		double dDeltaX, dDeltaY, dDeltaAngle;

		dDeltaX = pLocalAlignROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx] + pLocalAlignROIRgn->m_iLocalAlignDeltaX[iThreadIdx];
		dDeltaY = pLocalAlignROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx] + pLocalAlignROIRgn->m_iLocalAlignDeltaY[iThreadIdx];
		VectorAngleToRigid(pLocalAlignROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx], pLocalAlignROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx], 0, dDeltaY, dDeltaX, 0, &HomMat2DLocal);
		AffineTransRegion(HInspectAreaRgn, &HInspectAreaRgn, HomMat2DLocal, "nearest_neighbor");

		if (bDebugSave)
		{
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\HInspectAreaRgn_AfterTranslate.reg");
		}

		if (Param.m_bComponentApply_1)
		{
			if (pLocalAlignROIRgn->m_dLocalAlignMatchingScore[iThreadIdx] < 0 || pLocalAlignROIRgn->m_iLocalAlignDeltaX[iThreadIdx] == INVALID_ALIGN_RESULT)
			{
				*piComponentDefectType = Param.m_iComponentDefectType1;
				return HInspectAreaRgn;
			}
		}

		if (Param.m_bComponentApply_2)
		{
			if (pLocalAlignROIRgn->m_dLocalAlignMatchingScore[iThreadIdx] < Param.m_dComponentScore)
			{
				*piComponentDefectType = Param.m_iComponentDefectType2;
				return HInspectAreaRgn;
			}
		}

		HTuple HAngleDeg, HFiducialAngleDeg;
		double dAngleDeg, dFiducialAngleDeg;

		if (Param.m_bComponentApply_3)
		{
			if (fabs(pLocalAlignROIRgn->m_iLocalAlignDeltaX[iThreadIdx]) > Param.m_dComponentShiftX)
			{
				*piComponentDefectType = Param.m_iComponentDefectType3;
				return HInspectAreaRgn;
			}

			if (fabs(pLocalAlignROIRgn->m_iLocalAlignDeltaY[iThreadIdx]) > Param.m_dComponentShiftY)
			{
				*piComponentDefectType = Param.m_iComponentDefectType3;
				return HInspectAreaRgn;
			}

			TupleDeg(HTuple(pLocalAlignROIRgn->m_dLocalAlignDeltaAngle[iThreadIdx]), &HAngleDeg);
			dAngleDeg = HAngleDeg[0].D();

			if (fabs(dAngleDeg) > Param.m_dComponentRotationAngle)
			{
				*piComponentDefectType = Param.m_iComponentDefectType3;
				return HInspectAreaRgn;
			}
		}

		if (Param.m_bComponentApply_5)
		{
			double dAngle;
			HTuple HDistance, HAngle;
			DPOINT dPoint1, dPoint2;
			DPOINT dLineStart, dLineEnd;
			DPOINT dLineStart2, dLineEnd2;
			HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
			SmallestRectangle1(HInspectAreaRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);


			CenterlineMeasureStruct& teachingFaiMeasure = CFAIDataManager::GetInstance().GetTeachingMeasure();
			dLineStart.x = teachingFaiMeasure.m_dFiducialMark_PointX[0];
			dLineStart.y = teachingFaiMeasure.m_dFiducialMark_PointY[0];
			dLineEnd.x = teachingFaiMeasure.m_dFiducialMark_PointX[1];
			dLineEnd.y = teachingFaiMeasure.m_dFiducialMark_PointY[1];

			dLineStart2.x = (double)HlLTPointX.L();
			dLineStart2.y = (double)HlLTPointY.L();
			dLineEnd2.x = (double)HlRBPointX.L();
			dLineEnd2.y = (double)HlRBPointY.L();


			AngleLx(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HFiducialAngleDeg);
			//TupleAbs(HFiducialAngleDeg, &HFiducialAngleDeg);
			if (HFiducialAngleDeg[0].D() > PI / 2)
				HFiducialAngleDeg = PI - HFiducialAngleDeg;

			dFiducialAngleDeg = HFiducialAngleDeg.D();
			TupleDeg(HTuple(pLocalAlignROIRgn->m_dLocalAlignDeltaAngle[iThreadIdx]), &HAngleDeg);
			//TupleAbs(HAngleDeg, &HAngleDeg);
			dAngleDeg = HAngleDeg[0].D();

			if (fabs(dFiducialAngleDeg - dAngleDeg) > Param.m_dComponentRotationAngle2)
			{
				*piComponentDefectType = Param.m_iComponentDefectType5;
				return HInspectAreaRgn;
			}

			//if (fabs(dAngleDeg) > Param.m_dComponentRotationAngle)
			//{
			//	*piComponentDefectType = Param.m_iComponentDefectType3;
			//	return HInspectAreaRgn;
			//}
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ComponentAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::DefectContidion(int iInspectBufferIndex, HObject HImage, HObject HInspectAreaRgn, HObject HBrightRgn, HObject HDarkRgn, CAlgorithmParam Param, int *piArea, double *pdXLength, double *pdYLength, double *pdLength)
{
	try
	{
		BOOL bDebugSave = FALSE;

		int i;
		int iMinLength, iMaxLength;

		if (pdLength != NULL)
			*pdLength = 0;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		if (Param.m_iThresholdOpeningSize > 0)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn))
				OpeningCircle(HBrightRgn, &HBrightRgn, (double)Param.m_iThresholdOpeningSize + 0.5);

			if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn))
				OpeningCircle(HDarkRgn, &HDarkRgn, (double)Param.m_iThresholdOpeningSize + 0.5);
		}

		if (Param.m_iThresholdClosingSize > 0)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn))
				ClosingCircle(HBrightRgn, &HBrightRgn, (double)Param.m_iThresholdClosingSize + 0.5);

			if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn))
				ClosingCircle(HDarkRgn, &HDarkRgn, (double)Param.m_iThresholdClosingSize + 0.5);
		}

		double dScratchMaxRadius;

		if (Param.m_bCombineBrightDarkBlob)
		{
			Union2(HBrightRgn, HDarkRgn, &HBrightRgn);
			if (Param.m_bMaxBlob)
			{
				Connection(HBrightRgn, &HBrightRgn);
				SelectShapeStd(HBrightRgn, &HBrightRgn, "max_area", 70);
			}

			GenEmptyObj(&HDarkRgn);
		}
		else
		{
			if (Param.m_bMaxBlob)
			{
				HObject HConcatObj;
				GenEmptyObj(&HConcatObj);

				if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn))
				{
					Connection(HBrightRgn, &HBrightRgn);
					ConcatObj(HConcatObj, HBrightRgn, &HConcatObj);
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn))
				{
					Connection(HDarkRgn, &HDarkRgn);
					ConcatObj(HConcatObj, HDarkRgn, &HConcatObj);
				}

				SelectShapeStd(HConcatObj, &HBrightRgn, "max_area", 70);

				GenEmptyObj(&HDarkRgn);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
		{
			if (Param.m_iCandidateMinArea > 1)
			{
				Connection(HBrightRgn, &HBrightRgn);
				SelectShape(HBrightRgn, &HBrightRgn, "area", "and", Param.m_iCandidateMinArea, MAX_DEF);

				if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
					Union1(HBrightRgn, &HBrightRgn);
			}

			if (Param.m_bClosingOpening)
			{
				if (Param.m_iClosingSize > 0)
				{
					if (Param.m_bApplyOneDirMorp)
					{
						if (Param.m_iOneDirMorpXClosing == 0)	// X Dir
						{
							ClosingRectangle1(HBrightRgn, &HBrightRgn, Param.m_iClosingSize * 2 + 1, 1);
						}
						else	// Y Dir
						{
							ClosingRectangle1(HBrightRgn, &HBrightRgn, 1, Param.m_iClosingSize * 2 + 1);
						}
					}
					else
					{
						ClosingCircle(HBrightRgn, &HBrightRgn, (double)Param.m_iClosingSize + 0.5);
					}
				}
				if (Param.m_iOpeningSize > 0)
				{
					if (Param.m_bApplyOneDirMorp)
					{
						if (Param.m_iOneDirMorpXOpening == 0)	// X Dir
						{
							OpeningRectangle1(HBrightRgn, &HBrightRgn, Param.m_iOpeningSize * 2 + 1, 1);
						}
						else	// Y Dir
						{
							OpeningRectangle1(HBrightRgn, &HBrightRgn, 1, Param.m_iOpeningSize * 2 + 1);
						}
					}
					else
					{
						OpeningCircle(HBrightRgn, &HBrightRgn, (double)Param.m_iOpeningSize + 0.5);
					}

					if (Param.m_iCandidateMinArea > 1)
					{
						Connection(HBrightRgn, &HBrightRgn);
						SelectShape(HBrightRgn, &HBrightRgn, "area", "and", Param.m_iCandidateMinArea, MAX_DEF);

						if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
							Union1(HBrightRgn, &HBrightRgn);
					}
				}
			}
			else
			{
				if (Param.m_iOpeningSize > 0)
				{
					if (Param.m_bApplyOneDirMorp)
					{
						if (Param.m_iOneDirMorpXOpening == 0)	// X Dir
						{
							OpeningRectangle1(HBrightRgn, &HBrightRgn, Param.m_iOpeningSize * 2 + 1, 1);
						}
						else	// Y Dir
						{
							OpeningRectangle1(HBrightRgn, &HBrightRgn, 1, Param.m_iOpeningSize * 2 + 1);
						}
					}
					else
					{
						OpeningCircle(HBrightRgn, &HBrightRgn, (double)Param.m_iOpeningSize + 0.5);
					}

					if (Param.m_iCandidateMinArea > 1)
					{
						Connection(HBrightRgn, &HBrightRgn);
						SelectShape(HBrightRgn, &HBrightRgn, "area", "and", Param.m_iCandidateMinArea, MAX_DEF);

						if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
							Union1(HBrightRgn, &HBrightRgn);
					}
				}
				if (Param.m_iClosingSize > 0)
				{
					if (Param.m_bApplyOneDirMorp)
					{
						if (Param.m_iOneDirMorpXClosing == 0)	// X Dir
						{
							ClosingRectangle1(HBrightRgn, &HBrightRgn, Param.m_iClosingSize * 2 + 1, 1);
						}
						else	// Y Dir
						{
							ClosingRectangle1(HBrightRgn, &HBrightRgn, 1, Param.m_iClosingSize * 2 + 1);
						}
					}
					else
					{
						ClosingCircle(HBrightRgn, &HBrightRgn, (double)Param.m_iClosingSize + 0.5);
					}
				}
			}

			if (Param.m_bFillup || Param.m_bUseConnection)
				Connection(HBrightRgn, &HBrightRgn);

			if (Param.m_bFillup)
			{
				if (Param.m_iHoleFillCondition == HOLE_FILL_ALL)
					FillUp(HBrightRgn, &HBrightRgn);
				else if (Param.m_iHoleFillCondition == HOLE_FILL_AREA)
					FillUpShape(HBrightRgn, &HBrightRgn, "area", 1, Param.m_iFillHoleArea);
			}

			if (Param.m_bUseConnection)
			{
				SelectShape(HBrightRgn, &HBrightRgn, "area", "and", Param.m_iConnectionMinSize, MAX_DEF);
				SelectShape(HBrightRgn, &HBrightRgn, "width", "and", Param.m_iConnectionMinXLength, MAX_DEF);

				if (Param.m_bUseAngle)
				{
					dScratchMaxRadius = (double)Param.m_iConnectionMaxWidth * 0.5;
					SelectShape(HBrightRgn, &HBrightRgn, "inner_radius", "and", 0, dScratchMaxRadius);
					BlobUnionAngle(&HBrightRgn, Param.m_iConnectionLength, 10);
				}
				else
				{
					BlobUnion(&HBrightRgn, Param.m_iConnectionLength);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
			{
				Union1(HBrightRgn, &HBrightRgn);
				ConcatObj(HDefectRgn, HBrightRgn, &HDefectRgn);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
		{
			if (Param.m_iCandidateMinArea > 1)
			{
				Connection(HDarkRgn, &HDarkRgn);
				SelectShape(HDarkRgn, &HDarkRgn, "area", "and", Param.m_iCandidateMinArea, MAX_DEF);

				if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
					Union1(HDarkRgn, &HDarkRgn);
			}

			if (Param.m_bClosingOpening)
			{
				if (Param.m_iClosingSize > 0)
				{
					if (Param.m_bApplyOneDirMorp)
					{
						if (Param.m_iOneDirMorpXClosing == 0)	// X Dir
						{
							ClosingRectangle1(HDarkRgn, &HDarkRgn, Param.m_iClosingSize * 2 + 1, 1);
						}
						else	// Y Dir
						{
							ClosingRectangle1(HDarkRgn, &HDarkRgn, 1, Param.m_iClosingSize * 2 + 1);
						}
					}
					else
					{
						ClosingCircle(HDarkRgn, &HDarkRgn, (double)Param.m_iClosingSize + 0.5);
					}
				}
				if (Param.m_iOpeningSize > 0)
				{
					if (Param.m_bApplyOneDirMorp)
					{
						if (Param.m_iOneDirMorpXOpening == 0)	// X Dir
						{
							OpeningRectangle1(HDarkRgn, &HDarkRgn, Param.m_iOpeningSize * 2 + 1, 1);
						}
						else	// Y Dir
						{
							OpeningRectangle1(HDarkRgn, &HDarkRgn, 1, Param.m_iOpeningSize * 2 + 1);
						}
					}
					else
					{
						OpeningCircle(HDarkRgn, &HDarkRgn, (double)Param.m_iOpeningSize + 0.5);
					}

					if (Param.m_iCandidateMinArea > 1)
					{
						Connection(HDarkRgn, &HDarkRgn);
						SelectShape(HDarkRgn, &HDarkRgn, "area", "and", Param.m_iCandidateMinArea, MAX_DEF);

						if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
							Union1(HDarkRgn, &HDarkRgn);
					}
				}
			}
			else
			{
				if (Param.m_iOpeningSize > 0)
				{
					if (Param.m_bApplyOneDirMorp)
					{
						if (Param.m_iOneDirMorpXOpening == 0)	// X Dir
						{
							OpeningRectangle1(HDarkRgn, &HDarkRgn, Param.m_iOpeningSize * 2 + 1, 1);
						}
						else	// Y Dir
						{
							OpeningRectangle1(HDarkRgn, &HDarkRgn, 1, Param.m_iOpeningSize * 2 + 1);
						}
					}
					else
					{
						OpeningCircle(HDarkRgn, &HDarkRgn, (double)Param.m_iOpeningSize + 0.5);
					}

					if (Param.m_iCandidateMinArea > 1)
					{
						Connection(HDarkRgn, &HDarkRgn);
						SelectShape(HDarkRgn, &HDarkRgn, "area", "and", Param.m_iCandidateMinArea, MAX_DEF);

						if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
							Union1(HDarkRgn, &HDarkRgn);
					}
				}
				if (Param.m_iClosingSize > 0)
				{
					if (Param.m_bApplyOneDirMorp)
					{
						if (Param.m_iOneDirMorpXClosing == 0)	// X Dir
						{
							ClosingRectangle1(HDarkRgn, &HDarkRgn, Param.m_iClosingSize * 2 + 1, 1);
						}
						else	// Y Dir
						{
							ClosingRectangle1(HDarkRgn, &HDarkRgn, 1, Param.m_iClosingSize * 2 + 1);
						}
					}
					else
					{
						ClosingCircle(HDarkRgn, &HDarkRgn, (double)Param.m_iClosingSize + 0.5);
					}
				}
			}

			if (Param.m_bFillup || Param.m_bUseConnection)
				Connection(HDarkRgn, &HDarkRgn);

			if (Param.m_bFillup)
			{
				if (Param.m_iHoleFillCondition == HOLE_FILL_ALL)
					FillUp(HDarkRgn, &HDarkRgn);
				else if (Param.m_iHoleFillCondition == HOLE_FILL_AREA)
					FillUpShape(HDarkRgn, &HDarkRgn, "area", 1, Param.m_iFillHoleArea);
			}

			if (Param.m_bUseConnection)
			{
				SelectShape(HDarkRgn, &HDarkRgn, "area", "and", Param.m_iConnectionMinSize, MAX_DEF);
				SelectShape(HDarkRgn, &HDarkRgn, "width", "and", Param.m_iConnectionMinXLength, MAX_DEF);

				if (Param.m_bUseAngle)
				{
					dScratchMaxRadius = (double)Param.m_iConnectionMaxWidth * 0.5;
					SelectShape(HDarkRgn, &HDarkRgn, "inner_radius", "and", 0, dScratchMaxRadius);
					BlobUnionAngle(&HDarkRgn, Param.m_iConnectionLength, 10);
				}
				else
				{
					BlobUnion(&HDarkRgn, Param.m_iConnectionLength);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
			{
				Union1(HDarkRgn, &HDarkRgn);
				ConcatObj(HDefectRgn, HDarkRgn, &HDefectRgn);
			}
		}

		if (bDebugSave)
		{
			WriteRegion(HDefectRgn, "c:\\DualTest\\DefectCandidateRgn");
		}

		Connection(HDefectRgn, &HDefectRgn);

		if (Param.m_bUseConvex)
		{
			ShapeTrans(HDefectRgn, &HDefectRgn, "convex");
		}

		if (Param.m_bUseRect)
		{
			ShapeTrans(HDefectRgn, &HDefectRgn, "rectangle2");
		}

		HObject HDefectCandidateConvexRgn;
		GenEmptyObj(&HDefectCandidateConvexRgn);

		if (Param.m_bUseSubConvex)
		{
			HObject HDefectConRgn, HConvexRgn, HConvexSubRgn, HSelectedRgn;

			Connection(HDefectRgn, &HDefectConRgn);

			Hlong lNoDefect;
			HTuple HlNoDefect;
			CountObj(HDefectConRgn, &HlNoDefect);
			lNoDefect = HlNoDefect.L();

			GenEmptyObj(&HDefectRgn);

			for (i = 1; i <= lNoDefect; i++)
			{
				SelectObj(HDefectConRgn, &HSelectedRgn, i);

				if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
					continue;

				ShapeTrans(HSelectedRgn, &HConvexRgn, "convex");
				Difference(HConvexRgn, HSelectedRgn, &HConvexSubRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HConvexSubRgn) == TRUE)
				{
					ConcatObj(HDefectCandidateConvexRgn, HConvexRgn, &HDefectCandidateConvexRgn);

					Connection(HConvexSubRgn, &HConvexSubRgn);
					SelectShapeStd(HConvexSubRgn, &HConvexSubRgn, "max_area", 70);
					ConcatObj(HDefectRgn, HConvexSubRgn, &HDefectRgn);
				}
			}
		}

		if (Param.m_bDefectConditionUnionBlob)
		{
			Union1(HDefectRgn, &HDefectRgn);
		}

		if (Param.m_bUseDefectConditionArea && Param.m_iDefectConditionAreaMin > 0 && Param.m_iDefectConditionAreaMax > 0 && Param.m_iDefectConditionAreaMin <= Param.m_iDefectConditionAreaMax)
			SelectShape(HDefectRgn, &HDefectRgn, "area", "and", Param.m_iDefectConditionAreaMin, Param.m_iDefectConditionAreaMax);

		if (Param.m_iLengthWidthCondition == CONDITION_AND)
		{
			if (Param.m_bUseDefectConditionLength && Param.m_iDefectConditionLengthMin > 0 && Param.m_iDefectConditionLengthMax > 0 && Param.m_iDefectConditionLengthMin <= Param.m_iDefectConditionLengthMax)
				SelectShape(HDefectRgn, &HDefectRgn, "max_diameter", "and", Param.m_iDefectConditionLengthMin, Param.m_iDefectConditionLengthMax);
			if (Param.m_bUseDefectConditionWidth && Param.m_iDefectConditionWidthMin > 0 && Param.m_iDefectConditionWidthMax > 0 && Param.m_iDefectConditionWidthMin <= Param.m_iDefectConditionWidthMax)
				SelectShape(HDefectRgn, &HDefectRgn, "inner_radius", "and", Param.m_iDefectConditionWidthMin / 2, Param.m_iDefectConditionWidthMax / 2);
		}
		else
		{
			BOOL bApplyCondition = FALSE;

			if (Param.m_bUseDefectConditionLength && Param.m_iDefectConditionLengthMin > 0 && Param.m_iDefectConditionLengthMax > 0 && Param.m_iDefectConditionLengthMin <= Param.m_iDefectConditionLengthMax)
				bApplyCondition = TRUE;
			if (Param.m_bUseDefectConditionWidth && Param.m_iDefectConditionWidthMin > 0 && Param.m_iDefectConditionWidthMax > 0 && Param.m_iDefectConditionWidthMin <= Param.m_iDefectConditionWidthMax)
				bApplyCondition = TRUE;

			if (bApplyCondition)
			{
				HObject HLengthConditionPassRgn, HWidthConditionPassRgn;

				GenEmptyObj(&HLengthConditionPassRgn);
				GenEmptyObj(&HWidthConditionPassRgn);

				if (Param.m_bUseDefectConditionLength && Param.m_iDefectConditionLengthMin > 0 && Param.m_iDefectConditionLengthMax > 0 && Param.m_iDefectConditionLengthMin <= Param.m_iDefectConditionLengthMax)
					SelectShape(HDefectRgn, &HLengthConditionPassRgn, "max_diameter", "and", Param.m_iDefectConditionLengthMin, Param.m_iDefectConditionLengthMax);
				if (Param.m_bUseDefectConditionWidth && Param.m_iDefectConditionWidthMin > 0 && Param.m_iDefectConditionWidthMax > 0 && Param.m_iDefectConditionWidthMin <= Param.m_iDefectConditionWidthMax)
					SelectShape(HDefectRgn, &HWidthConditionPassRgn, "inner_radius", "and", Param.m_iDefectConditionWidthMin / 2, Param.m_iDefectConditionWidthMax / 2);

				GenEmptyObj(&HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HLengthConditionPassRgn) == TRUE)
					ConcatObj(HDefectRgn, HLengthConditionPassRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HWidthConditionPassRgn) == TRUE)
					ConcatObj(HDefectRgn, HWidthConditionPassRgn, &HDefectRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					if (Param.m_bDefectConditionUnionBlob == FALSE)
					{
						Union1(HDefectRgn, &HDefectRgn);
						Connection(HDefectRgn, &HDefectRgn);
					}
				}
			}
		}

		if (Param.m_bUseDefectConditionMean && (Param.m_iDefectConditionMeanMin >= 0 && Param.m_iDefectConditionMeanMin <= 255 && Param.m_iDefectConditionMeanMax >= 0 && Param.m_iDefectConditionMeanMax <= 255 && Param.m_iDefectConditionMeanMin <= Param.m_iDefectConditionMeanMax))
			SelectGray(HDefectRgn, HImage, &HDefectRgn, "mean", "and", (double)Param.m_iDefectConditionMeanMin, (double)Param.m_iDefectConditionMeanMax);
		if (Param.m_bUseDefectConditionStdev && Param.m_dDefectConditionStdevMin > 0 && Param.m_dDefectConditionStdevMax > 0 && Param.m_dDefectConditionStdevMin <= Param.m_dDefectConditionStdevMax)
			SelectGray(HDefectRgn, HImage, &HDefectRgn, "deviation", "and", Param.m_dDefectConditionStdevMin, Param.m_dDefectConditionStdevMax);
		if (Param.m_bUseDefectConditionCircularity && Param.m_dDefectConditionCircularityMin >= 0 && Param.m_dDefectConditionCircularityMax >= 0 &&
			Param.m_dDefectConditionCircularityMin <= 1 && Param.m_dDefectConditionCircularityMax <= 1 && Param.m_dDefectConditionCircularityMin <= Param.m_dDefectConditionCircularityMax)
			SelectShape(HDefectRgn, &HDefectRgn, "circularity", "and", Param.m_dDefectConditionCircularityMin, Param.m_dDefectConditionCircularityMax);
		if (Param.m_bUseDefectConditionRectangularity && Param.m_dDefectConditionRectangularityMin >= 0 && Param.m_dDefectConditionRectangularityMax >= 0 &&
			Param.m_dDefectConditionRectangularityMin <= 1 && Param.m_dDefectConditionRectangularityMax <= 1 && Param.m_dDefectConditionRectangularityMin <= Param.m_dDefectConditionRectangularityMax)
			SelectShape(HDefectRgn, &HDefectRgn, "rectangularity", "and", Param.m_dDefectConditionRectangularityMin, Param.m_dDefectConditionRectangularityMax);
		if (Param.m_bUseDefectConditionConvexity && Param.m_dDefectConditionConvexityMin >= 0 && Param.m_dDefectConditionConvexityMax >= 0 &&
			Param.m_dDefectConditionConvexityMin <= 1 && Param.m_dDefectConditionConvexityMax <= 1 && Param.m_dDefectConditionConvexityMin <= Param.m_dDefectConditionConvexityMax)
			SelectShape(HDefectRgn, &HDefectRgn, "convexity", "and", Param.m_dDefectConditionConvexityMin, Param.m_dDefectConditionConvexityMax);
		if (Param.m_bUseDefectConditionEllipseRatio && Param.m_dDefectConditionEllipseRatioMin >= 1.0 && Param.m_dDefectConditionEllipseRatioMax >= 1.0 &&
			Param.m_dDefectConditionEllipseRatioMin <= Param.m_dDefectConditionEllipseRatioMax)
			SelectShape(HDefectRgn, &HDefectRgn, "anisometry", "and", Param.m_dDefectConditionEllipseRatioMin, Param.m_dDefectConditionEllipseRatioMax);

		if (Param.m_bUseDefectConditionContrast)
		{
			if (bDebugSave)
			{
				WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
				WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
				WriteRegion(HDefectRgn, "c:\\DualTest\\DefectRgn");
			}

			HObject HDefectConRgn;

			if (Param.m_bDefectConditionUnionBlob == FALSE)
				Connection(HDefectRgn, &HDefectConRgn);
			else
				Union1(HDefectRgn, &HDefectConRgn);

			Hlong lNoDefect;
			HTuple HlNoDefect;
			CountObj(HDefectConRgn, &HlNoDefect);
			lNoDefect = HlNoDefect.L();

			HObject HSelectedRgn, HFillupRgn, HDilatedRgn, HBGRgn, HBGRgnImageReduced, HDefectBoundaryRgn;
			double dBgGray, dDefectGray, dSd, dContrast;
			int iBGMinThres, iBGMaxThres;

			int iContrastCheckWidthPxl = 3;

			GenEmptyObj(&HDefectRgn);

			for (i = 1; i <= lNoDefect; i++)
			{
				SelectObj(HDefectConRgn, &HSelectedRgn, i);

				if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
					continue;

				FillUp(HSelectedRgn, &HFillupRgn);
				DilationCircle(HFillupRgn, &HDilatedRgn, (double)iContrastCheckWidthPxl + 0.5);
				Difference(HDilatedRgn, HFillupRgn, &HBGRgn);
				Intersection(HBGRgn, HInspectAreaRgn, &HBGRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HBGRgn) == FALSE)	// Contrast 측정 불가
				{
					ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
					continue;
				}

				HTuple HdBgGray, HdSd;
				Intensity(HBGRgn, HImage, &HdBgGray, &HdSd);
				dBgGray = HdBgGray.D();
				dSd = HdSd.D();

				DilationCircle(HBGRgn, &HDilatedRgn, (double)iContrastCheckWidthPxl + 0.5);
				Intersection(HSelectedRgn, HDilatedRgn, &HDefectBoundaryRgn);

				HTuple HdDefectGray;
				Intensity(HDefectBoundaryRgn, HImage, &HdDefectGray, &HdSd);
				dDefectGray = HdDefectGray.D();
				dSd = HdSd.D();

				dContrast = fabs(dBgGray - dDefectGray);

				if (dContrast >= Param.m_iDefectConditionContrastMin && dContrast <= Param.m_iDefectConditionContrastMax)
					ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
			}
		}

		if (Param.m_bUseDefectConditionAreaRatio && Param.m_iDefectConditionAreaRatioMin >= 0 && Param.m_iDefectConditionAreaRatioMax <= 100 &&
			Param.m_iDefectConditionAreaRatioMin <= Param.m_iDefectConditionAreaRatioMax)
		{
			HObject HUnionRgn;
			Hlong lBlobArea, lROIArea;
			double dBlobCenterX, dBlobCenterY;
			HTuple HlROIArea, HlBlobArea, HdBlobCenterX, HdBlobCenterY;
			int iAreaRatio;

			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				Union1(HDefectRgn, &HUnionRgn);

				AreaCenter(HUnionRgn, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
				lBlobArea = HlBlobArea.L();
				dBlobCenterY = HdBlobCenterY.D();
				dBlobCenterX = HdBlobCenterX.D();
				AreaCenter(HInspectAreaRgn, &HlROIArea, &HdBlobCenterY, &HdBlobCenterX);
				lROIArea = HlROIArea.L();
				dBlobCenterY = HdBlobCenterY.D();
				dBlobCenterX = HdBlobCenterX.D();
				if (lROIArea > 0)
				{
					iAreaRatio = (int)((double)lBlobArea / (double)lROIArea * 100.0);
					if (iAreaRatio < 0)
						iAreaRatio = 0;
					if (iAreaRatio > 100)
						iAreaRatio = 100;

					if ((iAreaRatio >= Param.m_iDefectConditionAreaRatioMin && iAreaRatio <= Param.m_iDefectConditionAreaRatioMax) == FALSE)
						GenEmptyObj(&HDefectRgn);
				}
			}
		}

		if (Param.m_bUseDefectConditionAnisometry && Param.m_dDefectConditionAnisometryMin >= 1.0 && Param.m_dDefectConditionAnisometryMax >= 1.0 && Param.m_dDefectConditionAnisometryMin <= Param.m_dDefectConditionAnisometryMax)
		{
			Hlong lNoBlob;
			Hlong lRow1, lRow2, lCol1, lCol2;
			HTuple HlRow1, HlRow2, HlCol1, HlCol2;
			double dBlobLength, dBlobWidth;
			HTuple HdBlobLength, HdBlobWidth;
			double dCircleRow, dCircleCol, dCircleRadius, dRatio;
			HTuple HdCircleRow, HdCircleCol, HdCircleRadius, HdRatio;
			HObject HConnInputRgn, HSelectedRgn;

			if (Param.m_bDefectConditionUnionBlob == FALSE)
				Connection(HDefectRgn, &HConnInputRgn);
			else
				Union1(HDefectRgn, &HConnInputRgn);

			HTuple HlNoBlob;
			CountObj(HConnInputRgn, &HlNoBlob);
			lNoBlob = HlNoBlob.L();

			GenEmptyObj(&HDefectRgn);

			for (i = 1; i <= lNoBlob; i++)
			{
				SelectObj(HConnInputRgn, &HSelectedRgn, i);

				if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
					continue;

				DiameterRegion(HSelectedRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2, &HdBlobLength);
				lRow1 = HlRow1.L();
				lCol1 = HlCol1.L();
				lRow2 = HlRow2.L();
				lCol2 = HlCol2.L();
				dBlobLength = HdBlobLength.D();

				InnerCircle(HSelectedRgn, &HdCircleRow, &HdCircleCol, &HdCircleRadius);
				dCircleRow = HdCircleRow.D();
				dCircleCol = HdCircleCol.D();
				dCircleRadius = HdCircleRadius.D();

				dBlobWidth = dCircleRadius * 2.0;

				if (dBlobWidth <= 0)
					continue;

				dRatio = dBlobLength / dBlobWidth;

				if (dRatio <= Param.m_dDefectConditionAnisometryMax && dRatio >= Param.m_dDefectConditionAnisometryMin)
					ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
			}
		}

		if (Param.m_bUseDefectConditionInnerDist && Param.m_iDefectConditionInnerDist >= 0)	// 내측 기준선
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HInspectAreaFillRgn, HInspectAreaInnerRgn;

				Hlong lNoBlob;
				HObject HInspectAreaConnRgn;
				HObject HConnInputRgn, HSelectedRgn, HDiffRgn;

				Connection(HInspectAreaRgn, &HInspectAreaConnRgn);
				HTuple HlNoBlob;
				CountObj(HInspectAreaConnRgn, &HlNoBlob);
				lNoBlob = HlNoBlob.L();

				if (lNoBlob == 1)		// 검사영역 1개 있을때만 사용
				{
					FillUp(HInspectAreaRgn, &HInspectAreaFillRgn);
					Difference(HInspectAreaFillRgn, HInspectAreaRgn, &HInspectAreaInnerRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaInnerRgn) == TRUE)
					{
						DilationCircle(HInspectAreaInnerRgn, &HInspectAreaInnerRgn, (double)Param.m_iDefectConditionInnerDist + 0.5);

						if (Param.m_bDefectConditionUnionBlob == FALSE)
							Connection(HDefectRgn, &HConnInputRgn);
						else
							Union1(HDefectRgn, &HConnInputRgn);

						CountObj(HConnInputRgn, &HlNoBlob);
						lNoBlob = HlNoBlob.L();

						GenEmptyObj(&HDefectRgn);

						for (i = 1; i <= lNoBlob; i++)
						{
							SelectObj(HConnInputRgn, &HSelectedRgn, i);

							if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
								continue;

							Difference(HSelectedRgn, HInspectAreaInnerRgn, &HDiffRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HDiffRgn) == TRUE)
								ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
						}
					}
				}
			}
		}

		if (Param.m_bUseDefectConditionOuterDist && Param.m_iDefectConditionOuterDist >= 0)		// 외측 기준선
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				if (Param.m_bUseSubConvex)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(HDefectCandidateConvexRgn) == TRUE)
					{
						HObject HInspectAreaOuterRgn;

						Union1(HDefectCandidateConvexRgn, &HInspectAreaOuterRgn);
						ErosionCircle(HInspectAreaOuterRgn, &HInspectAreaOuterRgn, (double)Param.m_iDefectConditionOuterDist + 0.5);

						if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaOuterRgn) == TRUE)
						{
							SelectShapeProto(HDefectRgn, HInspectAreaOuterRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
						}
					}
				}
				else
				{
					HObject HInspectAreaOuterRgn;

					FillUp(HInspectAreaRgn, &HInspectAreaOuterRgn);
					ErosionCircle(HInspectAreaOuterRgn, &HInspectAreaOuterRgn, (double)Param.m_iDefectConditionOuterDist + 0.5);

					if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaOuterRgn) == TRUE)
					{
						SelectShapeProto(HDefectRgn, HInspectAreaOuterRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
					}
				}
			}
		}

		if (Param.m_bDefectConditionBoundaryConnection)		// 외측 기준선 연결성
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HInspectAreaOrgRgn, HInspectAreaRectRgn;

				FillUp(HInspectAreaRgn, &HInspectAreaOrgRgn);
				ShapeTrans(HInspectAreaOrgRgn, &HInspectAreaRectRgn, "rectangle1");

				Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX, lCenterX, lCenterY;
				Hlong lOrgLTPointY, lOrgLTPointX, lOrgRBPointY, lOrgRBPointX;
				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

				SmallestRectangle1(HInspectAreaRectRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

				lOrgLTPointY = HlLTPointY.L();
				lOrgLTPointX = HlLTPointX.L();
				lOrgRBPointY = HlRBPointY.L();
				lOrgRBPointX = HlRBPointX.L();

				lCenterX = (lOrgLTPointX + lOrgRBPointX) / 2;
				lCenterY = (lOrgLTPointY + lOrgRBPointY) / 2;

				if (Param.m_bDefectConditionBoundaryMarginHCenter && Param.m_iDefectConditionBoundaryMarginHCenter > 0)
				{
					lLTPointY = lCenterY - Param.m_iDefectConditionBoundaryMarginHCenter;
					lLTPointX = lOrgLTPointX;
					lRBPointY = lCenterY + Param.m_iDefectConditionBoundaryMarginHCenter;
					lRBPointX = lOrgRBPointX;

					HObject HBoundaryCheckRgn;
					GenEmptyObj(&HBoundaryCheckRgn);

					if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
					{
						GenRectangle1(&HBoundaryCheckRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

						if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
						{
							if (Param.m_iDefectConditionBoundaryMarginMinConnection > 1)
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnection, 999999999);
							else
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 99999999);
						}

					}
				}

				if (Param.m_bDefectConditionBoundaryMarginVCenter && Param.m_iDefectConditionBoundaryMarginVCenter > 0)
				{
					lLTPointY = lOrgLTPointY;
					lLTPointX = lCenterX - Param.m_iDefectConditionBoundaryMarginVCenter;
					lRBPointY = lOrgRBPointY;
					lRBPointX = lCenterX + Param.m_iDefectConditionBoundaryMarginVCenter;

					HObject HBoundaryCheckRgn;
					GenEmptyObj(&HBoundaryCheckRgn);

					if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
					{
						GenRectangle1(&HBoundaryCheckRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

						if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
						{
							if (Param.m_iDefectConditionBoundaryMarginMinConnection > 1)
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnection, 999999999);
							else
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 99999999);
						}

					}
				}

				if (Param.m_bDefectConditionBoundaryMarginHCenter == FALSE && Param.m_bDefectConditionBoundaryMarginVCenter == FALSE)
				{
					if (Param.m_iBoundaryConnectionCondition == CONDITION_OR)
					{
						lLTPointY = lOrgLTPointY;
						lLTPointX = lOrgLTPointX;
						lRBPointY = lOrgRBPointY;
						lRBPointX = lOrgRBPointX;

						if (Param.m_bDefectConditionBoundaryMarginTop && Param.m_iDefectConditionBoundaryMarginTop > 0)
							lLTPointY = lOrgLTPointY + Param.m_iDefectConditionBoundaryMarginTop;
						if (Param.m_bDefectConditionBoundaryMarginBottom && Param.m_iDefectConditionBoundaryMarginBottom > 0)
							lRBPointY = lOrgRBPointY - Param.m_iDefectConditionBoundaryMarginBottom;
						if (Param.m_bDefectConditionBoundaryMarginLeft && Param.m_iDefectConditionBoundaryMarginLeft > 0)
							lLTPointX = lOrgLTPointX + Param.m_iDefectConditionBoundaryMarginLeft;
						if (Param.m_bDefectConditionBoundaryMarginRight && Param.m_iDefectConditionBoundaryMarginRight > 0)
							lRBPointX = lOrgRBPointX - Param.m_iDefectConditionBoundaryMarginRight;

						HObject HInsideDiffRgn, HBoundaryCheckRgn;
						GenEmptyObj(&HInsideDiffRgn);
						GenEmptyObj(&HBoundaryCheckRgn);

						if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
							GenRectangle1(&HInsideDiffRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

						if (THEAPP.m_pGFunction->ValidHRegion(HInsideDiffRgn))
							Difference(HInspectAreaOrgRgn, HInsideDiffRgn, &HBoundaryCheckRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
						{
							if (Param.m_iDefectConditionBoundaryMarginMinConnection > 1)
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnection, 999999999);
							else
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 99999999);
						}
					}
					else
					{
						HObject HInsideDiffRgn, HBoundaryCheckRgn;

						if (Param.m_bDefectConditionBoundaryMarginTop && Param.m_iDefectConditionBoundaryMarginTop > 0)
						{
							lLTPointY = lOrgLTPointY;
							lLTPointX = lOrgLTPointX;
							lRBPointY = lOrgRBPointY;
							lRBPointX = lOrgRBPointX;

							lLTPointY = lOrgLTPointY + Param.m_iDefectConditionBoundaryMarginTop;

							GenEmptyObj(&HInsideDiffRgn);
							GenEmptyObj(&HBoundaryCheckRgn);

							if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
								GenRectangle1(&HInsideDiffRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

							if (THEAPP.m_pGFunction->ValidHRegion(HInsideDiffRgn))
								Difference(HInspectAreaOrgRgn, HInsideDiffRgn, &HBoundaryCheckRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
							{
								if (Param.m_iDefectConditionBoundaryMarginMinConnection > 1)
									SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnection, 999999999);
								else
									SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 999999999);
							}
						}

						if (Param.m_bDefectConditionBoundaryMarginBottom && Param.m_iDefectConditionBoundaryMarginBottom > 0)
						{
							lLTPointY = lOrgLTPointY;
							lLTPointX = lOrgLTPointX;
							lRBPointY = lOrgRBPointY;
							lRBPointX = lOrgRBPointX;

							lRBPointY = lOrgRBPointY - Param.m_iDefectConditionBoundaryMarginBottom;

							GenEmptyObj(&HInsideDiffRgn);
							GenEmptyObj(&HBoundaryCheckRgn);

							if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
								GenRectangle1(&HInsideDiffRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

							if (THEAPP.m_pGFunction->ValidHRegion(HInsideDiffRgn))
								Difference(HInspectAreaOrgRgn, HInsideDiffRgn, &HBoundaryCheckRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
							{
								if (Param.m_iDefectConditionBoundaryMarginMinConnection > 1)
									SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnection, 999999999);
								else
									SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 999999999);
							}
						}

						if (Param.m_bDefectConditionBoundaryMarginLeft && Param.m_iDefectConditionBoundaryMarginLeft > 0)
						{
							lLTPointY = lOrgLTPointY;
							lLTPointX = lOrgLTPointX;
							lRBPointY = lOrgRBPointY;
							lRBPointX = lOrgRBPointX;

							lLTPointX = lOrgLTPointX + Param.m_iDefectConditionBoundaryMarginLeft;

							GenEmptyObj(&HInsideDiffRgn);
							GenEmptyObj(&HBoundaryCheckRgn);

							if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
								GenRectangle1(&HInsideDiffRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

							if (THEAPP.m_pGFunction->ValidHRegion(HInsideDiffRgn))
								Difference(HInspectAreaOrgRgn, HInsideDiffRgn, &HBoundaryCheckRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
							{
								if (Param.m_iDefectConditionBoundaryMarginMinConnection > 1)
									SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnection, 999999999);
								else
									SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 999999999);
							}
						}

						if (Param.m_bDefectConditionBoundaryMarginRight && Param.m_iDefectConditionBoundaryMarginRight > 0)
						{
							lLTPointY = lOrgLTPointY;
							lLTPointX = lOrgLTPointX;
							lRBPointY = lOrgRBPointY;
							lRBPointX = lOrgRBPointX;

							lRBPointX = lOrgRBPointX - Param.m_iDefectConditionBoundaryMarginRight;

							GenEmptyObj(&HInsideDiffRgn);
							GenEmptyObj(&HBoundaryCheckRgn);

							if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
								GenRectangle1(&HInsideDiffRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

							if (THEAPP.m_pGFunction->ValidHRegion(HInsideDiffRgn))
								Difference(HInspectAreaOrgRgn, HInsideDiffRgn, &HBoundaryCheckRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
							{
								if (Param.m_iDefectConditionBoundaryMarginMinConnection > 1)
									SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnection, 999999999);
								else
									SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 999999999);
							}
						}
					}
				}
			}
		}

		if (Param.m_bDefectConditionBoundaryConnectionAny)		// 외측 기준선 연결성
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				if (Param.m_iDefectConditionBoundaryConnectionOuter == ROI_CONNECTION_OUTER)
				{
					HObject HInspectAreaOrgRgn, HInspectAreaOrgErosionRgn, HBoundaryCheckRgn;

					FillUp(HInspectAreaRgn, &HInspectAreaOrgRgn);

					if (Param.m_iDefectConditionBoundaryMarginAny > 0)
					{
						ErosionCircle(HInspectAreaOrgRgn, &HInspectAreaOrgErosionRgn, (double)Param.m_iDefectConditionBoundaryMarginAny + 0.5);
						Difference(HInspectAreaOrgRgn, HInspectAreaOrgErosionRgn, &HBoundaryCheckRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
						{
							if (Param.m_iDefectConditionBoundaryMarginMinConnectionAny >= 1)
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnectionAny, 999999999);
							else if (Param.m_iDefectConditionBoundaryMarginMinConnectionAny == 0)
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 0, 0);
						}
					}
				}
				else
				{
					HObject HInspectAreaOrgRgn, HInspectAreaOrgFillRgn, HInspectAreaOrgDilatedRgn, HBoundaryCheckRgn;

					FillUp(HInspectAreaRgn, &HInspectAreaOrgRgn);
					Difference(HInspectAreaOrgRgn, HInspectAreaRgn, &HInspectAreaOrgFillRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaOrgFillRgn) == TRUE)
					{
						FillUp(HInspectAreaOrgFillRgn, &HInspectAreaOrgRgn);

						if (Param.m_iDefectConditionBoundaryMarginAny > 0)
						{
							DilationCircle(HInspectAreaOrgRgn, &HInspectAreaOrgDilatedRgn, (double)Param.m_iDefectConditionBoundaryMarginAny + 0.5);
							Difference(HInspectAreaOrgDilatedRgn, HInspectAreaOrgRgn, &HBoundaryCheckRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
							{
								if (Param.m_iDefectConditionBoundaryMarginMinConnectionAny >= 1)
									SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnectionAny, 999999999);
								else if (Param.m_iDefectConditionBoundaryMarginMinConnectionAny == 0)
									SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 0, 0);
							}
						}
					}
				}
			}
		}

		if (Param.m_bDefectConditionBoundaryConnection2Side)		// 외측 기준선 연결성 (2면이상)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HInspectAreaOrgRgn, HInspectAreaRectRgn;

				FillUp(HInspectAreaRgn, &HInspectAreaOrgRgn);
				ShapeTrans(HInspectAreaOrgRgn, &HInspectAreaRectRgn, "rectangle1");

				Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
				Hlong lOrgLTPointY, lOrgLTPointX, lOrgRBPointY, lOrgRBPointX;
				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

				SmallestRectangle1(HInspectAreaRectRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

				lOrgLTPointY = HlLTPointY.L();
				lOrgLTPointX = HlLTPointX.L();
				lOrgRBPointY = HlRBPointY.L();
				lOrgRBPointX = HlRBPointX.L();

				HObject HTopLineRgn, HBottomLineRgn, HLeftLineRgn, HRightLineRgn;

				lLTPointY = lOrgLTPointY + Param.m_iDefectConditionBoundaryMarginTop;
				lRBPointY = lOrgRBPointY - Param.m_iDefectConditionBoundaryMarginBottom;
				lLTPointX = lOrgLTPointX + Param.m_iDefectConditionBoundaryMarginLeft;
				lRBPointX = lOrgRBPointX - Param.m_iDefectConditionBoundaryMarginRight;

				GenRectangle1(&HTopLineRgn, lLTPointY, lLTPointX + 1, lLTPointY, lRBPointX - 1);
				GenRectangle1(&HBottomLineRgn, lRBPointY, lLTPointX + 1, lRBPointY, lRBPointX - 1);
				GenRectangle1(&HLeftLineRgn, lLTPointY + 1, lLTPointX, lRBPointY - 1, lLTPointX);
				GenRectangle1(&HRightLineRgn, lLTPointY + 1, lRBPointX, lRBPointY - 1, lRBPointX);

				HObject HTopBottomRgn, HTopLeftRgn, HTopRightRgn, HBottomLeftRgn, HBottomRightRgn, HLeftRightRgn;
				HObject HTempConnRgn;

				GenEmptyObj(&HTopBottomRgn);
				SelectShapeProto(HDefectRgn, HTopLineRgn, &HTopBottomRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HTopBottomRgn, HBottomLineRgn, &HTopBottomRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HTopLeftRgn);
				SelectShapeProto(HDefectRgn, HTopLineRgn, &HTopLeftRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HTopLeftRgn, HLeftLineRgn, &HTopLeftRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HTopRightRgn);
				SelectShapeProto(HDefectRgn, HTopLineRgn, &HTopRightRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HTopRightRgn, HRightLineRgn, &HTopRightRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HBottomLeftRgn);
				SelectShapeProto(HDefectRgn, HBottomLineRgn, &HBottomLeftRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HBottomLeftRgn, HLeftLineRgn, &HBottomLeftRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HBottomRightRgn);
				SelectShapeProto(HDefectRgn, HBottomLineRgn, &HBottomRightRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HBottomRightRgn, HRightLineRgn, &HBottomRightRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HLeftRightRgn);
				SelectShapeProto(HDefectRgn, HLeftLineRgn, &HLeftRightRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HLeftRightRgn, HRightLineRgn, &HLeftRightRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HDefectRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HTopBottomRgn))
					ConcatObj(HDefectRgn, HTopBottomRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HTopLeftRgn))
					ConcatObj(HDefectRgn, HTopLeftRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HTopRightRgn))
					ConcatObj(HDefectRgn, HTopRightRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HBottomLeftRgn))
					ConcatObj(HDefectRgn, HBottomLeftRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HBottomRightRgn))
					ConcatObj(HDefectRgn, HBottomRightRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HLeftRightRgn))
					ConcatObj(HDefectRgn, HLeftRightRgn, &HDefectRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn))
				{
					if (Param.m_bDefectConditionUnionBlob == FALSE)
					{
						Union1(HDefectRgn, &HDefectRgn);
						Connection(HDefectRgn, &HDefectRgn);
					}
				}
			}
		}

		if (Param.m_bUseDefectConditionBlob && Param.m_iDefectConditionBlobMin >= 1 && Param.m_iDefectConditionBlobMax >= 1 && Param.m_iDefectConditionBlobMin <= Param.m_iDefectConditionBlobMax)
		{
			Hlong lNoDefectBlob = 0;
			HTuple HlNoDefectBlob;
			CountObj(HDefectRgn, &HlNoDefectBlob);
			lNoDefectBlob = HlNoDefectBlob.L();

			if (lNoDefectBlob<Param.m_iDefectConditionBlobMin || lNoDefectBlob>Param.m_iDefectConditionBlobMax)
				GenEmptyObj(&HDefectRgn);
		}

		if (Param.m_bUseDefectConditionGrad)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HRectInspectRgn, HGradCropImage, HGradImage;
				HObject HInspectMovedRgn, HDefectMovedRgn;
				ShapeTrans(HInspectAreaRgn, &HRectInspectRgn, "rectangle1");

				Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

				SmallestRectangle1(HRectInspectRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

				lLTPointY = HlLTPointY.L();
				lLTPointX = HlLTPointX.L();
				lRBPointY = HlRBPointY.L();
				lRBPointX = HlRBPointX.L();

				CropRectangle1(HImage, &HGradCropImage, lLTPointY, lLTPointX, lRBPointY, lRBPointX);
				SobelAmp(HGradCropImage, &HGradImage, "sum_abs", 3);

				MoveRegion(HInspectAreaRgn, &HInspectMovedRgn, -lLTPointY, -lLTPointX);
				Boundary(HInspectMovedRgn, &HInspectMovedRgn, "inner");
				MoveRegion(HDefectRgn, &HDefectMovedRgn, -lLTPointY, -lLTPointX);

				HObject HDefectConRgn;

				if (Param.m_bDefectConditionUnionBlob == FALSE)
					Connection(HDefectMovedRgn, &HDefectConRgn);
				else
					Union1(HDefectMovedRgn, &HDefectConRgn);

				Hlong lNoDefect;
				HTuple HlNoDefect;
				CountObj(HDefectConRgn, &HlNoDefect);
				lNoDefect = HlNoDefect.L();

				HObject HSelectedRgn, HSelectedBoundaryRgn, HGradCheckRgn;

				GenEmptyObj(&HDefectRgn);

				for (i = 1; i <= lNoDefect; i++)
				{
					SelectObj(HDefectConRgn, &HSelectedRgn, i);

					if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
						continue;

					Boundary(HSelectedRgn, &HSelectedBoundaryRgn, "inner");
					Difference(HSelectedBoundaryRgn, HInspectMovedRgn, &HGradCheckRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HGradCheckRgn) == FALSE)	// 측정 불가
					{
						ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
						continue;
					}

					HTuple HdGrad, HdSd;
					double dGrad;

					Intensity(HGradCheckRgn, HGradImage, &HdGrad, &HdSd);
					dGrad = HdGrad.D();

					if (dGrad >= (double)Param.m_iDefectConditionGradMin && dGrad <= (double)Param.m_iDefectConditionGradMax)
						ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
					MoveRegion(HDefectRgn, &HDefectRgn, lLTPointY, lLTPointX);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		////  Area, XLength, YLength 불량 조건 적용 전에 Union Blob의 Area, XLength, YLength
		Hlong lDefectArea;
		double dDefectCenterX, dDefectCenterY;
		HTuple HlDefectArea, HdDefectCenterX, HdDefectCenterY;
		Hlong lDefectRow1, lDefectRow2, lDefectCol1, lDefectCol2, lDefectXLength, lDefectYLength;
		HObject HMeasureDefectRgn, MaxAreaRgn, MaxWidthRgn, MaxHeightRgn;
		HTuple HIndices, HInverted;
		long lMaxIndex;
		HTuple HDefectRow1, HDefectCol1, HDefectRow2, HDefectCol2;
		HTuple HXLength, HYLength;

		HObject HTempDefectRgn;

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
		{
			if (Param.m_bDefectConditionUnionBlob == FALSE)
				Connection(HDefectRgn, &HMeasureDefectRgn);
			else
				Union1(HDefectRgn, &HMeasureDefectRgn);

			// Max Area
			SelectShapeStd(HMeasureDefectRgn, &MaxAreaRgn, "max_area", 70);
			AreaCenter(MaxAreaRgn, &HlDefectArea, &HdDefectCenterY, &HdDefectCenterX);
			lDefectArea = HlDefectArea.L();
			dDefectCenterY = HdDefectCenterY.D();
			dDefectCenterX = HdDefectCenterX.D();
			*piArea = lDefectArea;

			//VER 1.1.3.3 modified start
			if (pdLength != NULL)
			{
				try
				{
					const int CONERLENGTH_LEFT_TOP = 0;
					const int CONERLENGTH_RIGHT_TOP = 1;
					const int CONERLENGTH_LEFT_BOTTOM = 2;
					const int CONERLENGTH_RIGHT_BOTTOM = 3;

					if (Param.m_bDefectConditionConerLengthUse && THEAPP.m_pGFunction->ValidHRegion(MaxAreaRgn))
					{
						HTuple htTempROIRectLTx, htTempROIRectLTy, htTempROIRectRBx, htTempROIRectRBy;
						HTuple htConerBlobMinDist, htConerBlobMaxDist;
						SmallestRectangle1(HInspectAreaRgn, &htTempROIRectLTy, &htTempROIRectLTx, &htTempROIRectRBy, &htTempROIRectRBx);
						HTuple htCornerX, htCornerY;
						switch (Param.m_nDefectConditionConerLengthDir) //선택된 코너와 Blob Region까지의 최대,최소 거리 계산
						{
						case CONERLENGTH_LEFT_TOP:
							DistancePr(MaxAreaRgn, htTempROIRectLTy, htTempROIRectLTx, &htConerBlobMinDist, &htConerBlobMaxDist);
							htCornerX = htTempROIRectLTx;
							htCornerY = htTempROIRectLTy;
							break;
						case CONERLENGTH_RIGHT_TOP:
							DistancePr(MaxAreaRgn, htTempROIRectLTy, htTempROIRectRBx, &htConerBlobMinDist, &htConerBlobMaxDist);
							htCornerX = htTempROIRectRBx;
							htCornerY = htTempROIRectLTy;
							break;
						case CONERLENGTH_LEFT_BOTTOM:
							DistancePr(MaxAreaRgn, htTempROIRectRBy, htTempROIRectLTx, &htConerBlobMinDist, &htConerBlobMaxDist);
							htCornerX = htTempROIRectLTx;
							htCornerY = htTempROIRectRBy;
							break;
						case CONERLENGTH_RIGHT_BOTTOM:
							DistancePr(MaxAreaRgn, htTempROIRectRBy, htTempROIRectRBx, &htConerBlobMinDist, &htConerBlobMaxDist);
							htCornerX = htTempROIRectRBx;
							htCornerY = htTempROIRectRBy;
							break;
						}

						//로그에 먼저 기록 후 불량 검출 조건 확인 뒤 검출 조건 해당하지 않으면 불량 후보에서 제거
						if (Param.m_bDefectConditionConerLengthMaxUse = FALSE) //if-else문 Ver 1.1.3.4 modified
							*pdLength = htConerBlobMaxDist.D() - htConerBlobMinDist.D();
						else
						{
							HObject HTempCircleForMaxDistRgn, HTempMaxDistInterRgn;
							HTuple htTempCenterX, htTempCenterY, htTempArea;
							*pdLength = htConerBlobMaxDist.D();
							//GenCircle(&HTempCircleForMaxDistRgn, htCornerY, htCornerX, *pdLength);

							//Boundary(HTempCircleForMaxDistRgn, &HTempCircleForMaxDistRgn, "inner");
							////HDefectRgn = HTempCircleForMaxDistRgn;
							/*Intersection(HTempCircleForMaxDistRgn, MaxAreaRgn, &HTempMaxDistInterRgn);
							if (THEAPP.m_pGFunction->ValidHRegion(HTempMaxDistInterRgn))
							{
								AreaCenter(HTempMaxDistInterRgn, &htTempCenterY, &htTempCenterX, &htTempArea);
								GenRegionLine(&HDefectRgn, htTempCenterY, htTempCenterX, htCornerY, htCornerX);
							}
							else
							{
								DilationCircle(HTempCircleForMaxDistRgn, &HTempCircleForMaxDistRgn, 0.5);
								Intersection(HTempCircleForMaxDistRgn, MaxAreaRgn, &HTempMaxDistInterRgn);
								if (THEAPP.m_pGFunction->ValidHRegion(HTempMaxDistInterRgn))
								{
									AreaCenter(HTempMaxDistInterRgn, &htTempCenterY, &htTempCenterX, &htTempArea);
									GenRegionLine(&HDefectRgn, htTempCenterY, htTempCenterX, htCornerY, htCornerX);
								}
							}*/
						}

						if (*pdLength < Param.m_nDefectConditionConerLengthMin || *pdLength > Param.m_nDefectConditionConerLengthMax)
						{
							GenEmptyObj(&HDefectRgn);
						}

						if (bDebugSave)
						{
							WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
							WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn.reg");
							WriteRegion(MaxAreaRgn, "c:\\DualTest\\MaxAreaRgn.reg");

							/*CString strJwjTest;
							strJwjTest.Format("Prot Length : %.8f / RBy %.4f / LTx %.4f ", *pdLength, htTempROIRectRBy.D(), htTempROIRectLTx.D());
							AfxMessageBox(strJwjTest);*/
						}
					}
				}
				catch (HException &except)
				{
					HTuple HExp;
					HTuple HOperatorName, HErrMsg;
					except.ToHTuple(&HExp);
					except.GetExceptionData(HExp, "operator", &HOperatorName);
					except.GetExceptionData(HExp, "error_message", &HErrMsg);

					CString str;
					str.Format("Halcon Exception [Algorithm::DefectContidion_CornerDist] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
					AfxMessageBox(str);
				}
			}
			//VER 1.1.3.3 modified end

			double dOrthoLength;

			SmallestRectangle1(HMeasureDefectRgn, &HDefectRow1, &HDefectCol1, &HDefectRow2, &HDefectCol2);

			if (Param.m_bDefectConditionOrthoLength && (Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_LEFT || Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_RIGHT))
			{
				BOOL bProcRet = SelectShapeUsingOrthoLength(HDefectRgn, HInspectAreaRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iOrthoLengthRefSide, &HTempDefectRgn, &dOrthoLength);
				if (bProcRet == FALSE)
				{
					if (Param.m_iXLengthCondition == LENGTH_MAX)
					{
						if (Param.m_bXYLengthMaxLineLength)
						{
							SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HTempDefectRgn, &iMaxLength);
							*pdXLength = (double)iMaxLength;
						}
						else
						{
							// Max Width
							HXLength = HDefectCol2 - HDefectCol1 + 1;
							TupleSortIndex(HXLength, &HIndices);
							TupleInverse(HIndices, &HInverted);
							lMaxIndex = HInverted[0].L();
							int iTempXLength;
							iTempXLength = HXLength[lMaxIndex];
							*pdXLength = (double)iTempXLength;
						}
					}
					else if (Param.m_iXLengthCondition == LENGTH_MIN)
					{
						SelectShapeUsingMinLength(HDefectRgn, 1, 999999, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMinLength);
						*pdXLength = (double)iMinLength;
					}
				}
				else
				{
					*pdXLength = dOrthoLength;
				}
			}
			else
			{
				if (Param.m_iXLengthCondition == LENGTH_MAX)
				{
					if (Param.m_bXYLengthMaxLineLength)
					{
						SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HTempDefectRgn, &iMaxLength);
						*pdXLength = (double)iMaxLength;
					}
					else
					{
						// Max Width
						HXLength = HDefectCol2 - HDefectCol1 + 1;
						TupleSortIndex(HXLength, &HIndices);
						TupleInverse(HIndices, &HInverted);
						lMaxIndex = HInverted[0].L();
						int iTempXLength;
						iTempXLength = HXLength[lMaxIndex];
						*pdXLength = (double)iTempXLength;
					}
				}
				else if (Param.m_iXLengthCondition == LENGTH_MIN)
				{
					SelectShapeUsingMinLength(HDefectRgn, 1, 999999, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMinLength);
					*pdXLength = (double)iMinLength;
				}
			}

			if (Param.m_bDefectConditionOrthoLength && (Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_TOP || Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_BOTTOM))
			{
				BOOL bProcRet = SelectShapeUsingOrthoLength(HDefectRgn, HInspectAreaRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iOrthoLengthRefSide, &HTempDefectRgn, &dOrthoLength);
				if (bProcRet == FALSE)
				{
					if (Param.m_iYLengthCondition == LENGTH_MAX)
					{
						if (Param.m_bXYLengthMaxLineLength)
						{
							SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HTempDefectRgn, &iMaxLength);
							*pdYLength = (double)iMaxLength;
						}
						else
						{
							// Max Length
							HYLength = HDefectRow2 - HDefectRow1 + 1;
							TupleSortIndex(HYLength, &HIndices);
							TupleInverse(HIndices, &HInverted);
							lMaxIndex = HInverted[0].L();
							int iTempYLength;
							iTempYLength = HYLength[lMaxIndex];
							*pdYLength = (double)iTempYLength;
						}
					}
					else if (Param.m_iYLengthCondition == LENGTH_MIN)
					{
						SelectShapeUsingMinLength(HDefectRgn, 1, 999999, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMinLength);
						*pdYLength = (double)iMinLength;
					}
				}
				else
				{
					*pdYLength = dOrthoLength;
				}
			}
			else
			{
				if (Param.m_iYLengthCondition == LENGTH_MAX)
				{
					if (Param.m_bXYLengthMaxLineLength)
					{
						SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HTempDefectRgn, &iMaxLength);
						*pdYLength = (double)iMaxLength;
					}
					else
					{
						// Max Length
						HYLength = HDefectRow2 - HDefectRow1 + 1;
						TupleSortIndex(HYLength, &HIndices);
						TupleInverse(HIndices, &HInverted);
						lMaxIndex = HInverted[0].L();
						int iTempYLength;
						iTempYLength = HYLength[lMaxIndex];
						*pdYLength = (double)iTempYLength;
					}
				}
				else if (Param.m_iYLengthCondition == LENGTH_MIN)
				{
					SelectShapeUsingMinLength(HDefectRgn, 1, 999999, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMinLength);
					*pdYLength = (double)iMinLength;
				}
			}

		}
		//////////////////////////////////////////////////////////////////////////

		double dOrthoLength;

		if (Param.m_iXYLengthCondition == CONDITION_AND)
		{
			if (Param.m_bUseDefectConditionXLength && Param.m_iDefectConditionXLengthMin > 0 && Param.m_iDefectConditionXLengthMax > 0 && Param.m_iDefectConditionXLengthMin <= Param.m_iDefectConditionXLengthMax)
			{
				if (Param.m_bDefectConditionOrthoLength && (Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_LEFT || Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_RIGHT))
				{
					BOOL bProcRet = SelectShapeUsingOrthoLength(HDefectRgn, HInspectAreaRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iOrthoLengthRefSide, &HDefectRgn, &dOrthoLength);
					if (bProcRet == FALSE)
					{
						if (Param.m_iXLengthCondition == LENGTH_MAX)
						{
							if (Param.m_bXYLengthMaxLineLength)
							{
								SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMaxLength);
							}
							else
							{
								SelectShape(HDefectRgn, &HDefectRgn, "width", "and", Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax);
							}
						}
						else if (Param.m_iXLengthCondition == LENGTH_MIN)
							SelectShapeUsingMinLength(HDefectRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMinLength);
					}
				}
				else
				{
					if (Param.m_iXLengthCondition == LENGTH_MAX)
					{
						if (Param.m_bXYLengthMaxLineLength)
						{
							SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMaxLength);
						}
						else
						{
							SelectShape(HDefectRgn, &HDefectRgn, "width", "and", Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax);
						}
					}
					else if (Param.m_iXLengthCondition == LENGTH_MIN)
						SelectShapeUsingMinLength(HDefectRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMinLength);
				}
			}

			if (Param.m_bUseDefectConditionYLength && Param.m_iDefectConditionYLengthMin > 0 && Param.m_iDefectConditionYLengthMax > 0 && Param.m_iDefectConditionYLengthMin <= Param.m_iDefectConditionYLengthMax)
			{
				if (Param.m_bDefectConditionOrthoLength && (Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_TOP || Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_BOTTOM))
				{
					BOOL bProcRet = SelectShapeUsingOrthoLength(HDefectRgn, HInspectAreaRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iOrthoLengthRefSide, &HDefectRgn, &dOrthoLength);
					if (bProcRet == FALSE)
					{
						if (Param.m_iYLengthCondition == LENGTH_MAX)
						{
							if (Param.m_bXYLengthMaxLineLength)
							{
								SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMaxLength);
							}
							else
							{
								SelectShape(HDefectRgn, &HDefectRgn, "height", "and", Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax);
							}
						}
						else if (Param.m_iYLengthCondition == LENGTH_MIN)
							SelectShapeUsingMinLength(HDefectRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMinLength);
					}
				}
				else
				{
					if (Param.m_iYLengthCondition == LENGTH_MAX)
					{
						if (Param.m_bXYLengthMaxLineLength)
						{
							SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMaxLength);
						}
						else
						{
							SelectShape(HDefectRgn, &HDefectRgn, "height", "and", Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax);
						}
					}
					else if (Param.m_iYLengthCondition == LENGTH_MIN)
						SelectShapeUsingMinLength(HDefectRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HDefectRgn, &iMinLength);
				}
			}
		}
		else
		{
			BOOL bApplyCondition = FALSE;

			if (Param.m_bUseDefectConditionXLength && Param.m_iDefectConditionXLengthMin > 0 && Param.m_iDefectConditionXLengthMax > 0 && Param.m_iDefectConditionXLengthMin <= Param.m_iDefectConditionXLengthMax)
				bApplyCondition = TRUE;
			if (Param.m_bUseDefectConditionYLength && Param.m_iDefectConditionYLengthMin > 0 && Param.m_iDefectConditionYLengthMax > 0 && Param.m_iDefectConditionYLengthMin <= Param.m_iDefectConditionYLengthMax)
				bApplyCondition = TRUE;

			if (bApplyCondition)
			{
				HObject HXLengthConditionPassRgn, HYLengthConditionPassRgn;

				GenEmptyObj(&HXLengthConditionPassRgn);
				GenEmptyObj(&HYLengthConditionPassRgn);

				if (Param.m_bUseDefectConditionXLength && Param.m_iDefectConditionXLengthMin > 0 && Param.m_iDefectConditionXLengthMax > 0 && Param.m_iDefectConditionXLengthMin <= Param.m_iDefectConditionXLengthMax)
				{
					if (Param.m_bDefectConditionOrthoLength && (Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_LEFT || Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_RIGHT))
					{
						BOOL bProcRet = SelectShapeUsingOrthoLength(HDefectRgn, HInspectAreaRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iOrthoLengthRefSide, &HXLengthConditionPassRgn, &dOrthoLength);
						if (bProcRet == FALSE)
						{
							if (Param.m_iXLengthCondition == LENGTH_MAX)
							{
								if (Param.m_bXYLengthMaxLineLength)
								{
									SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HXLengthConditionPassRgn, &iMaxLength);
								}
								else
								{
									SelectShape(HDefectRgn, &HXLengthConditionPassRgn, "width", "and", Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax);
								}
							}
							else if (Param.m_iXLengthCondition == LENGTH_MIN)
								SelectShapeUsingMinLength(HDefectRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HXLengthConditionPassRgn, &iMinLength);
						}
					}
					else
					{
						if (Param.m_iXLengthCondition == LENGTH_MAX)
						{
							if (Param.m_bXYLengthMaxLineLength)
							{
								SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HXLengthConditionPassRgn, &iMaxLength);
							}
							else
							{
								SelectShape(HDefectRgn, &HXLengthConditionPassRgn, "width", "and", Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax);
							}
						}
						else if (Param.m_iXLengthCondition == LENGTH_MIN)
							SelectShapeUsingMinLength(HDefectRgn, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, Param.m_iXLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HXLengthConditionPassRgn, &iMinLength);
					}
				}

				if (Param.m_bUseDefectConditionYLength && Param.m_iDefectConditionYLengthMin > 0 && Param.m_iDefectConditionYLengthMax > 0 && Param.m_iDefectConditionYLengthMin <= Param.m_iDefectConditionYLengthMax)
				{
					if (Param.m_bDefectConditionOrthoLength && (Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_TOP || Param.m_iOrthoLengthRefSide == ORTHO_LENGTH_BOTTOM))
					{
						BOOL bProcRet = SelectShapeUsingOrthoLength(HDefectRgn, HInspectAreaRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iOrthoLengthRefSide, &HYLengthConditionPassRgn, &dOrthoLength);
						if (bProcRet == FALSE)
						{
							if (Param.m_iYLengthCondition == LENGTH_MAX)
							{
								if (Param.m_bXYLengthMaxLineLength)
								{
									SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HYLengthConditionPassRgn, &iMaxLength);
								}
								else
								{
									SelectShape(HDefectRgn, &HYLengthConditionPassRgn, "height", "and", Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax);
								}
							}
							else if (Param.m_iYLengthCondition == LENGTH_MIN)
								SelectShapeUsingMinLength(HDefectRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HYLengthConditionPassRgn, &iMinLength);
						}
					}
					else
					{
						if (Param.m_iYLengthCondition == LENGTH_MAX)
						{
							if (Param.m_bXYLengthMaxLineLength)
							{
								SelectShapeUsingMaxLength(HDefectRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HYLengthConditionPassRgn, &iMaxLength);
							}
							else
							{
								SelectShape(HDefectRgn, &HYLengthConditionPassRgn, "height", "and", Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax);
							}
						}
						else if (Param.m_iYLengthCondition == LENGTH_MIN)
							SelectShapeUsingMinLength(HDefectRgn, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, Param.m_iYLengthConditionMinRef, Param.m_bXYLengthMeasureIgnoreHole, &HYLengthConditionPassRgn, &iMinLength);
					}
				}

				GenEmptyObj(&HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HXLengthConditionPassRgn) == TRUE)
					ConcatObj(HDefectRgn, HXLengthConditionPassRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HYLengthConditionPassRgn) == TRUE)
					ConcatObj(HDefectRgn, HYLengthConditionPassRgn, &HDefectRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					if (Param.m_bDefectConditionUnionBlob == FALSE)
					{
						Union1(HDefectRgn, &HDefectRgn);
						Connection(HDefectRgn, &HDefectRgn);
					}
				}
			}
		}

		if (Param.m_bUseDefectConditionHole)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HDefectUnionRgn, HFillUnionRgn;
				Union1(HDefectRgn, &HDefectUnionRgn);
				FillUpShape(HDefectUnionRgn, &HFillUnionRgn, "area", Param.m_iDefectConditionHoleArea, 10000000);
				Difference(HFillUnionRgn, HDefectUnionRgn, &HDefectRgn);
				Connection(HDefectRgn, &HDefectRgn);
			}
		}

		if (Param.m_bDefectConditionInnerOuterBoundaryConnection)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HInspectAreaBoundaryRgn, HInspectAreaBoundaryCenterRgn, HDefectBlobRgn, HDefectBlobDilatedRgn;
				HObject HROIBoundaryIntersectRgn;

				Boundary(HInspectAreaRgn, &HInspectAreaBoundaryRgn, "inner");
				Skeleton(HInspectAreaRgn, &HInspectAreaBoundaryCenterRgn);

				HObject HDefectConnRgn;
				Connection(HDefectRgn, &HDefectConnRgn);

				SelectShapeProto(HDefectConnRgn, HInspectAreaBoundaryCenterRgn, &HDefectConnRgn, "overlaps_abs", 1, 9999999);

				HTuple HNoDefectBlob;
				Hlong lNoDefectBlob;
				CountObj(HDefectConnRgn, &HNoDefectBlob);
				lNoDefectBlob = HNoDefectBlob[0].L();

				GenEmptyObj(&HDefectRgn);

				for (i = 1; i <= lNoDefectBlob; i++)
				{
					SelectObj(HDefectConnRgn, &HDefectBlobRgn, i);

					if (THEAPP.m_pGFunction->ValidHRegion(HDefectBlobRgn) == FALSE)
						continue;

					DilationCircle(HDefectBlobRgn, &HDefectBlobDilatedRgn, 1.5);
					Intersection(HInspectAreaBoundaryRgn, HDefectBlobDilatedRgn, &HROIBoundaryIntersectRgn);

					Connection(HROIBoundaryIntersectRgn, &HROIBoundaryIntersectRgn);

					HTuple HNoBoundaryBlob;
					Hlong lNoBoundaryBlob;
					CountObj(HROIBoundaryIntersectRgn, &HNoBoundaryBlob);
					lNoBoundaryBlob = HNoBoundaryBlob[0].L();

					if (lNoBoundaryBlob >= 2)
						ConcatObj(HDefectRgn, HDefectBlobRgn, &HDefectRgn);
				}
			}
		}

		//if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn)==TRUE)
		//{
		//	Union1(HDefectRgn, &HDefectRgn);
		//	Intersection(HInspectAreaRgn, HDefectRgn, &HDefectRgn);
		//	Connection(HDefectRgn, &HDefectRgn);
		//}

		if (Param.m_bUseDefectConditionXLengthSum && Param.m_iDefectConditionXLengthSumMin > 0 && Param.m_iDefectConditionXLengthSumMax > 0 && Param.m_iDefectConditionXLengthSumMin <= Param.m_iDefectConditionXLengthSumMax)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HDefectUnionRgn, HClosingRgn;
				Union1(HDefectRgn, &HDefectUnionRgn);
				ClosingRectangle1(HDefectUnionRgn, &HClosingRgn, 1, 5000);
				Connection(HClosingRgn, &HClosingRgn);

				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX, HBlobLength, HSumLength;
				Hlong lSumLength;

				SmallestRectangle1(HClosingRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
				TupleSub(HlRBPointX, HlLTPointX, &HBlobLength);
				TupleSum(HBlobLength, &HSumLength);
				lSumLength = HSumLength.L();

				if (lSumLength<Param.m_iDefectConditionXLengthSumMin || lSumLength>Param.m_iDefectConditionXLengthSumMax)
					GenEmptyObj(&HDefectRgn);
			}
		}

		if (Param.m_bUseDefectConditionYLengthSum && Param.m_iDefectConditionYLengthSumMin > 0 && Param.m_iDefectConditionYLengthSumMax > 0 && Param.m_iDefectConditionYLengthSumMin <= Param.m_iDefectConditionYLengthSumMax)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HDefectUnionRgn, HClosingRgn;
				Union1(HDefectRgn, &HDefectUnionRgn);
				ClosingRectangle1(HDefectUnionRgn, &HClosingRgn, 5000, 1);
				Connection(HClosingRgn, &HClosingRgn);

				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX, HBlobLength, HSumLength;
				Hlong lSumLength;

				SmallestRectangle1(HClosingRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
				TupleSub(HlRBPointY, HlLTPointY, &HBlobLength);
				TupleSum(HBlobLength, &HSumLength);
				lSumLength = HSumLength.L();

				if (lSumLength<Param.m_iDefectConditionYLengthSumMin || lSumLength>Param.m_iDefectConditionYLengthSumMax)
					GenEmptyObj(&HDefectRgn);
			}
		}

		if ((Param.m_bUseDefectConditionShiftX && Param.m_iDefectConditionShiftXMin >= 0 && Param.m_iDefectConditionShiftXMax >= 0 && Param.m_iDefectConditionShiftXMin <= Param.m_iDefectConditionShiftXMax) == TRUE ||
			(Param.m_bUseDefectConditionShiftY && Param.m_iDefectConditionShiftYMin >= 0 && Param.m_iDefectConditionShiftYMax >= 0 && Param.m_iDefectConditionShiftYMin <= Param.m_iDefectConditionShiftYMax) == TRUE)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				Hlong lShiftTempArea;
				double dShiftInspectCenterX, dShiftInspectCenterY, dShiftDefectCenterX, dShiftDefectCenterY;
				double dShiftValueX, dShiftValueY;
				HTuple HlShiftTempArea, HdShiftInspectCenterY, HdShiftInspectCenterX, HdShiftDefectCenterY, HdShiftDefectCenterX;

				AreaCenter(HInspectAreaRgn, &HlShiftTempArea, &HdShiftInspectCenterY, &HdShiftInspectCenterX);
				lShiftTempArea = HlShiftTempArea.L();
				dShiftInspectCenterY = HdShiftInspectCenterY.D();
				dShiftInspectCenterX = HdShiftInspectCenterX.D();

				HObject HUnionRgn;
				Union1(HDefectRgn, &HUnionRgn);

				AreaCenter(HUnionRgn, &HlShiftTempArea, &HdShiftDefectCenterY, &HdShiftDefectCenterX);
				lShiftTempArea = HlShiftTempArea.L();
				dShiftDefectCenterY = HdShiftDefectCenterY.D();
				dShiftDefectCenterX = HdShiftDefectCenterX.D();

				dShiftValueX = fabs(dShiftInspectCenterX - dShiftDefectCenterX);
				dShiftValueY = fabs(dShiftInspectCenterY - dShiftDefectCenterY);

				if (Param.m_bUseDefectConditionShiftX)
				{
					if ((dShiftValueX >= Param.m_iDefectConditionShiftXMin && dShiftValueX <= Param.m_iDefectConditionShiftXMax) == FALSE)
						GenEmptyObj(&HDefectRgn);
				}

				if (Param.m_bUseDefectConditionShiftY)
				{
					if ((dShiftValueY >= Param.m_iDefectConditionShiftYMin && dShiftValueY <= Param.m_iDefectConditionShiftYMax) == FALSE)
						GenEmptyObj(&HDefectRgn);
				}
			}
		}

		//220302 jwj add VER 1.1.3.2
		if (Param.m_bDefectConditionOuterDiff)
		{
			HObject HDefectConRgn;

			if (Param.m_bDefectConditionUnionBlob == FALSE)
				Connection(HDefectRgn, &HDefectConRgn);
			else
				Union1(HDefectRgn, &HDefectConRgn);

			HObject HOrgDefectConRgn = HDefectRgn;
			try {
				//#ifndef INLINE_MODE
				//			BOOL bPreDebugSave = bDebugSave;
				//			if (bPreDebugSave == FALSE)
				//				bDebugSave = TRUE;
				//#endif
				int nInterval = Param.m_nDefectConditionOuterDiffInterval;
				int nMinDifference = Param.m_nDefectConditionOuterDiffMinDiff;
				int nReatMaxCnt = Param.m_nDefectConditionOuterDiffMaxRepeat;

				int nDir = Param.m_nOuterDiffUpper;

				const int OUTER_DIFF_UPPER_SIDE = 0;
				const int OUTER_DIFF_LOWER_SIDE = 1;
				const int OUTER_DIFF_LEFT_SIDE = 2;
				const int OUTER_DIFF_RIGHT_SIDE = 3;

				if (bDebugSave)
				{
					WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
					WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn.reg");
					WriteRegion(HDefectRgn, "c:\\DualTest\\DefectRgn.reg");
				}

				Hlong lNoDefect;
				HTuple HlNoDefect;
				CountObj(HDefectConRgn, &HlNoDefect);
				lNoDefect = HlNoDefect.L();

				HObject HSelectedRgn;

				GenEmptyObj(&HDefectRgn);

				for (i = 1; i <= lNoDefect; i++)
				{
					SelectObj(HDefectConRgn, &HSelectedRgn, i);

					if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
						continue;

					if (bDebugSave)
					{
						WriteRegion(HSelectedRgn, "c:\\DualTest\\HSelectedRgn.reg");
					}

					HObject HBlobRectRgn, HOuterIntersectionRgn, HOuterRectRgn, HValidRectRgn, HValidInterRgn;

					HTuple htLTx, htLTy, htRBx, htRBy;
					HTuple htOuterLTx, htOuterLTy, htOuterRBx, htOuterRBy;

					SmallestRectangle1(HSelectedRgn, &htLTy, &htLTx, &htRBy, &htRBx); //불량 Region 외접 최소사각형

					switch (nDir) //불량 Region 상,하,좌,우 중 최외곽을 연장한 Line 생성
					{
					case OUTER_DIFF_UPPER_SIDE:
						GenRectangle1(&HBlobRectRgn, htLTy, htLTx, htLTy, htRBx);
						break;
					case OUTER_DIFF_LOWER_SIDE:
						GenRectangle1(&HBlobRectRgn, htRBy, htLTx, htRBy, htRBx);
						break;
					case OUTER_DIFF_LEFT_SIDE:
						GenRectangle1(&HBlobRectRgn, htLTy, htLTx, htRBy, htLTx);
						break;
					case OUTER_DIFF_RIGHT_SIDE:
						GenRectangle1(&HBlobRectRgn, htLTy, htRBx, htRBy, htRBx);
						break;
					}

					if (bDebugSave)
					{
						WriteRegion(HBlobRectRgn, "c:\\DualTest\\HBlobRectRgn.reg");
					}

					Intersection(HBlobRectRgn, HSelectedRgn, &HOuterIntersectionRgn); //최외곽 Line과 불량 Region 교집합영역 생성
					Union1(HOuterIntersectionRgn, &HOuterIntersectionRgn); //최외곽 Region(돌출된 후보영역)이 돌출부 2개이상일 경우 예외날 경우 방지(예로 같은 높이의 쌍봉우리)

					if (bDebugSave)
					{
						WriteRegion(HOuterIntersectionRgn, "c:\\DualTest\\HOuterIntersectionRgn.reg");
					}

					SmallestRectangle1(HOuterIntersectionRgn, &htOuterLTy, &htOuterLTx, &htOuterRBy, &htOuterRBx); //최외곽 Region의 내접하는 최소사각형 생성

					HTuple htTempLTx, htTempLTy, htTempRBx, htTempRBy;

					htTempLTx = 0;
					htTempLTy = 0;
					htTempRBx = 0;
					htTempRBy = 0;
					int nTempDist1 = 0;
					int nTempDist2 = 0;
					HTuple htDist;

					//거리 계산
					switch (nDir)
					{
					case OUTER_DIFF_UPPER_SIDE:
						nTempDist1 = htOuterLTy.L();
						nTempDist2 = htOuterLTy.L();
						break;
					case OUTER_DIFF_LOWER_SIDE:
						nTempDist1 = htOuterRBy.L();
						nTempDist2 = htOuterRBy.L();
						break;
					case OUTER_DIFF_LEFT_SIDE:
						nTempDist1 = htOuterLTx.L();
						nTempDist2 = htOuterLTx.L();
						break;
					case OUTER_DIFF_RIGHT_SIDE:
						nTempDist1 = htOuterRBx.L();
						nTempDist2 = htOuterRBx.L();
						break;
					}

					//반복횟수(nReatMaxCnt)만큼 특정 간격(nInterval)만큼 shift하여 높이의 차이가 얼마나 나는지 계산 (abs함수 안되서 제곱으로 크기비교함)
					for (int nRepeatCnt = 1; nRepeatCnt <= nReatMaxCnt; nRepeatCnt++)
					{
						int nValidRectLTx;
						int nValidRectLTy;
						int nValidRectRBx;
						int nValidRectRBy;

						//반복횟수(nReatMaxCnt)만큼 특정 간격(nInterval)만큼 shift한 Line생성
						switch (nDir)
						{
						case OUTER_DIFF_UPPER_SIDE: //UPPER_SIDE or LOWER_SIDE
						case OUTER_DIFF_LOWER_SIDE:
							nValidRectLTy = htLTy.L();
							(htOuterLTx.L() - nInterval * nRepeatCnt) <= htLTx.L() ? nValidRectLTx = htLTx.L() : nValidRectLTx = (htOuterLTx.L() - nInterval * nRepeatCnt);
							nValidRectRBy = htRBy.L();
							(htOuterLTx.L() - nInterval * nRepeatCnt) <= htLTx.L() ? nValidRectRBx = htLTx.L() : nValidRectRBx = (htOuterLTx.L() - nInterval * nRepeatCnt);
							break;
						case OUTER_DIFF_LEFT_SIDE: //LEFT_SIDE or RIGHT_SIDE
						case OUTER_DIFF_RIGHT_SIDE:
							(htOuterLTy.L() - nInterval * nRepeatCnt) <= htLTy.L() ? nValidRectLTy = htLTy.L() : nValidRectLTy = (htOuterLTy.L() - nInterval * nRepeatCnt);
							nValidRectLTx = htLTx.L();
							(htOuterLTy.L() - nInterval * nRepeatCnt) <= htLTy.L() ? nValidRectRBy = htLTy.L() : nValidRectRBy = (htOuterLTy.L() - nInterval * nRepeatCnt);
							nValidRectRBx = htRBx.L();
							break;
						}

						GenRectangle1(&HValidRectRgn, nValidRectLTy, nValidRectLTx, nValidRectRBy, nValidRectRBx);

						if (bDebugSave)
						{
							CString strTempFileName;
							strTempFileName.Format("c:\\DualTest\\HValidRectRgn_%d.reg", nRepeatCnt);
							WriteRegion(HValidRectRgn, (LPCSTR)strTempFileName);
						}

						Intersection(HValidRectRgn, HSelectedRgn, &HValidInterRgn);
						if (THEAPP.m_pGFunction->ValidHRegion(HValidInterRgn))
						{
							Union1(HValidInterRgn, &HValidInterRgn);

							if (bDebugSave)
							{
								CString strTempFileName;
								strTempFileName.Format("c:\\DualTest\\HValidInterRgn_%d.reg", nRepeatCnt);
								WriteRegion(HValidInterRgn, (LPCSTR)strTempFileName);
							}

							SmallestRectangle1(HValidInterRgn, &htTempLTy, &htTempLTx, &htTempRBy, &htTempRBx);

							//거리 계산
							switch (nDir)
							{
							case OUTER_DIFF_UPPER_SIDE:
								htDist = nTempDist1 - htTempLTy;
								nTempDist1 = htTempLTy.L();
								break;
							case OUTER_DIFF_LOWER_SIDE:
								htDist = nTempDist1 - htTempRBy;
								nTempDist1 = htTempRBy.L();
								break;
							case OUTER_DIFF_LEFT_SIDE:
								htDist = nTempDist1 - htTempLTx;
								nTempDist1 = htTempLTx.L();
								break;
							case OUTER_DIFF_RIGHT_SIDE:
								htDist = nTempDist1 - htTempRBx;
								nTempDist1 = htTempRBx.L();
								break;
							}

							/*	CString strTestMessage;
								strTestMessage.Format("hDist = %d nMinDifference = %d", htDist.L(), nMinDifference);
								AfxMessageBox(strTestMessage);*/
							if (htDist*htDist >= (nMinDifference)*(nMinDifference))
							{
								ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
								break;
							}
						}

						switch (nDir)
						{
						case OUTER_DIFF_UPPER_SIDE: //UPPER_SIDE or LOWER_SIDE
						case OUTER_DIFF_LOWER_SIDE:
							nValidRectLTy = htLTy.L();
							(htOuterRBx + nInterval * nRepeatCnt) >= htRBx.L() ? nValidRectLTx = htRBx.L() : nValidRectLTx = (htOuterRBx + nInterval * nRepeatCnt);
							nValidRectRBy = htRBy.L();
							(htOuterRBx + nInterval * nRepeatCnt) >= htRBx.L() ? nValidRectRBx = htRBx.L() : nValidRectRBx = (htOuterRBx + nInterval * nRepeatCnt);
							break;
						case OUTER_DIFF_LEFT_SIDE: //LEFT_SIDE or RIGHT_SIDE
						case OUTER_DIFF_RIGHT_SIDE:
							(htOuterRBy.L() + nInterval * nRepeatCnt) >= htRBy.L() ? nValidRectLTy = htRBy.L() : nValidRectLTy = (htOuterRBy.L() + nInterval * nRepeatCnt);
							nValidRectLTx = htLTx.L();
							(htOuterRBy.L() + nInterval * nRepeatCnt) >= htRBy.L() ? nValidRectRBy = htRBy.L() : nValidRectRBy = (htOuterRBy.L() + nInterval * nRepeatCnt);
							nValidRectRBx = htRBx.L();
							break;
						}

						GenRectangle1(&HValidRectRgn, nValidRectLTy, nValidRectLTx, nValidRectRBy, nValidRectRBx);

						if (bDebugSave)
						{
							CString strTempFileName;
							strTempFileName.Format("c:\\DualTest\\HValidRectRgn2_%d.reg", nRepeatCnt);
							WriteRegion(HValidRectRgn, (LPCSTR)strTempFileName);
						}

						Intersection(HValidRectRgn, HSelectedRgn, &HValidInterRgn);
						if (THEAPP.m_pGFunction->ValidHRegion(HValidInterRgn))
						{
							Union1(HValidInterRgn, &HValidInterRgn);

							if (bDebugSave)
							{
								CString strTempFileName;
								strTempFileName.Format("c:\\DualTest\\HValidInterRgn2_%d.reg", nRepeatCnt);
								WriteRegion(HValidInterRgn, (LPCSTR)strTempFileName);
							}

							SmallestRectangle1(HValidInterRgn, &htTempLTy, &htTempLTx, &htTempRBy, &htTempRBx);

							//거리 계산
							switch (nDir)
							{
							case OUTER_DIFF_UPPER_SIDE:
								htDist = nTempDist2 - htTempLTy;
								nTempDist2 = htTempLTy.L();
								break;
							case OUTER_DIFF_LOWER_SIDE:
								htDist = nTempDist2 - htTempRBy;
								nTempDist2 = htTempRBy.L();
								break;
							case OUTER_DIFF_LEFT_SIDE:
								htDist = nTempDist2 - htTempLTx;
								nTempDist2 = htTempLTx.L();
								break;
							case OUTER_DIFF_RIGHT_SIDE:
								htDist = nTempDist2 - htTempRBx;
								nTempDist2 = htTempRBx.L();
								break;
							}

							/*CString strTestMessage;
							strTestMessage.Format("2 hDist = %d nMinDifference = %d", htDist.L(), nMinDifference);
							AfxMessageBox(strTestMessage);*/
							if (htDist*htDist >= (nMinDifference)*(nMinDifference))
							{
								ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
								break;
							}
						}
					}
				}

				//#ifndef INLINE_MODE
				//			bDebugSave = bPreDebugSave;
				//#endif
			}
			catch (HException &except)
			{
				HTuple HExp;
				HTuple HOperatorName, HErrMsg;
				except.ToHTuple(&HExp);
				except.GetExceptionData(HExp, "operator", &HOperatorName);
				except.GetExceptionData(HExp, "error_message", &HErrMsg);

				strLog.Format("Halcon Exception [Algorithm::DefectContidion_OuterDifference] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
				THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

				HDefectRgn = HOrgDefectConRgn;
			}
		}
		//220302 jwj end VER 1.1.3.2

		if (Param.m_bDefectConditionDilation)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				if (Param.m_iDefectConditionDilationSize > 0)
				{
					DilationCircle(HDefectRgn, &HDefectRgn, (double)Param.m_iDefectConditionDilationSize + 0.5);
				}
				else if (Param.m_iDefectConditionDilationSize < 0)
				{
					int iTempErosionMargin;
					iTempErosionMargin = -Param.m_iDefectConditionDilationSize;
					ErosionCircle(HDefectRgn, &HDefectRgn, (double)iTempErosionMargin + 0.5);
				}
			}
		}

		if (Param.m_bUseDefectConditionUseZeroArea)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				GenEmptyObj(&HDefectRgn);
			else
			{
				HDefectRgn = HInspectAreaRgn;
			}
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::DefectContidion] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::DefectContidionDC(HObject HImage, HObject HInspectAreaRgn, HObject HBrightRgn, HObject HDarkRgn, CAlgorithmParam Param, int *piArea, int *piXLength, int *piYLength)
{
	try
	{
		BOOL bDebugSave = FALSE;

		int i;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		double dScratchMaxRadius;

		if (Param.m_bCombineBrightDarkBlobDC)
		{
			Union2(HBrightRgn, HDarkRgn, &HBrightRgn);
			if (Param.m_bMaxBlobDC)
			{
				Connection(HBrightRgn, &HBrightRgn);
				SelectShapeStd(HBrightRgn, &HBrightRgn, "max_area", 70);
			}

			GenEmptyObj(&HDarkRgn);
		}
		else
		{
			if (Param.m_bMaxBlobDC)
			{
				HObject HConcatObj;
				GenEmptyObj(&HConcatObj);

				if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn))
				{
					Connection(HBrightRgn, &HBrightRgn);
					ConcatObj(HConcatObj, HBrightRgn, &HConcatObj);
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn))
				{
					Connection(HDarkRgn, &HDarkRgn);
					ConcatObj(HConcatObj, HDarkRgn, &HConcatObj);
				}

				SelectShapeStd(HConcatObj, &HBrightRgn, "max_area", 70);

				GenEmptyObj(&HDarkRgn);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
		{
			if (Param.m_iCandidateMinAreaDC > 1)
			{
				Connection(HBrightRgn, &HBrightRgn);
				SelectShape(HBrightRgn, &HBrightRgn, "area", "and", Param.m_iCandidateMinAreaDC, MAX_DEF);

				if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
					Union1(HBrightRgn, &HBrightRgn);
			}

			if (Param.m_bClosingOpeningDC)
			{
				if (Param.m_iClosingSizeDC > 0)
					ClosingCircle(HBrightRgn, &HBrightRgn, (double)Param.m_iClosingSizeDC + 0.5);
				if (Param.m_iOpeningSizeDC > 0)
				{
					OpeningCircle(HBrightRgn, &HBrightRgn, (double)Param.m_iOpeningSizeDC + 0.5);

					if (Param.m_iCandidateMinAreaDC > 1)
					{
						Connection(HBrightRgn, &HBrightRgn);
						SelectShape(HBrightRgn, &HBrightRgn, "area", "and", Param.m_iCandidateMinAreaDC, MAX_DEF);

						if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
							Union1(HBrightRgn, &HBrightRgn);
					}
				}
			}
			else
			{
				if (Param.m_iOpeningSizeDC > 0)
				{
					OpeningCircle(HBrightRgn, &HBrightRgn, (double)Param.m_iOpeningSizeDC + 0.5);

					if (Param.m_iCandidateMinAreaDC > 1)
					{
						Connection(HBrightRgn, &HBrightRgn);
						SelectShape(HBrightRgn, &HBrightRgn, "area", "and", Param.m_iCandidateMinAreaDC, MAX_DEF);

						if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
							Union1(HBrightRgn, &HBrightRgn);
					}
				}
				if (Param.m_iClosingSizeDC > 0)
					ClosingCircle(HBrightRgn, &HBrightRgn, (double)Param.m_iClosingSizeDC + 0.5);
			}

			if (Param.m_bFillupDC)
			{
				Connection(HBrightRgn, &HBrightRgn);
				FillUp(HBrightRgn, &HBrightRgn);
			}

			if (Param.m_bUseConvexDC)
			{
				Union1(HBrightRgn, &HBrightRgn);
				ShapeTrans(HBrightRgn, &HBrightRgn, "convex");
			}

			if (Param.m_bUseConnectionDC)
			{
				Connection(HBrightRgn, &HBrightRgn);

				SelectShape(HBrightRgn, &HBrightRgn, "area", "and", Param.m_iConnectionMinSizeDC, MAX_DEF);
				SelectShape(HBrightRgn, &HBrightRgn, "width", "and", Param.m_iConnectionMinXLengthDC, MAX_DEF);

				if (Param.m_bUseAngleDC)
				{
					dScratchMaxRadius = (double)Param.m_iConnectionMaxWidthDC * 0.5;
					SelectShape(HBrightRgn, &HBrightRgn, "inner_radius", "and", 0, dScratchMaxRadius);
					BlobUnionAngle(&HBrightRgn, Param.m_iConnectionLengthDC, 10);
				}
				else
				{
					BlobUnion(&HBrightRgn, Param.m_iConnectionLengthDC);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
			{
				Union1(HBrightRgn, &HBrightRgn);
				ConcatObj(HDefectRgn, HBrightRgn, &HDefectRgn);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
		{
			if (Param.m_iCandidateMinAreaDC > 1)
			{
				Connection(HDarkRgn, &HDarkRgn);
				SelectShape(HDarkRgn, &HDarkRgn, "area", "and", Param.m_iCandidateMinAreaDC, MAX_DEF);

				if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
					Union1(HDarkRgn, &HDarkRgn);
			}

			if (Param.m_bClosingOpeningDC)
			{
				if (Param.m_iClosingSizeDC > 0)
					ClosingCircle(HDarkRgn, &HDarkRgn, (double)Param.m_iClosingSizeDC + 0.5);
				if (Param.m_iOpeningSizeDC > 0)
				{
					OpeningCircle(HDarkRgn, &HDarkRgn, (double)Param.m_iOpeningSizeDC + 0.5);

					if (Param.m_iCandidateMinAreaDC > 1)
					{
						Connection(HDarkRgn, &HDarkRgn);
						SelectShape(HDarkRgn, &HDarkRgn, "area", "and", Param.m_iCandidateMinAreaDC, MAX_DEF);

						if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
							Union1(HDarkRgn, &HDarkRgn);
					}
				}
			}
			else
			{
				if (Param.m_iOpeningSizeDC > 0)
				{
					OpeningCircle(HDarkRgn, &HDarkRgn, (double)Param.m_iOpeningSizeDC + 0.5);

					if (Param.m_iCandidateMinAreaDC > 1)
					{
						Connection(HDarkRgn, &HDarkRgn);
						SelectShape(HDarkRgn, &HDarkRgn, "area", "and", Param.m_iCandidateMinAreaDC, MAX_DEF);

						if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
							Union1(HDarkRgn, &HDarkRgn);
					}
				}
				if (Param.m_iClosingSizeDC > 0)
					ClosingCircle(HDarkRgn, &HDarkRgn, (double)Param.m_iClosingSizeDC + 0.5);
			}

			if (Param.m_bFillupDC)
			{
				Connection(HDarkRgn, &HDarkRgn);
				FillUp(HDarkRgn, &HDarkRgn);
			}

			if (Param.m_bUseConvexDC)
			{
				Union1(HDarkRgn, &HDarkRgn);
				ShapeTrans(HDarkRgn, &HDarkRgn, "convex");
			}

			if (Param.m_bUseConnectionDC)
			{
				Connection(HDarkRgn, &HDarkRgn);

				SelectShape(HDarkRgn, &HDarkRgn, "area", "and", Param.m_iConnectionMinSizeDC, MAX_DEF);
				SelectShape(HDarkRgn, &HDarkRgn, "width", "and", Param.m_iConnectionMinXLengthDC, MAX_DEF);

				if (Param.m_bUseAngleDC)
				{
					dScratchMaxRadius = (double)Param.m_iConnectionMaxWidthDC * 0.5;
					SelectShape(HDarkRgn, &HDarkRgn, "inner_radius", "and", 0, dScratchMaxRadius);
					BlobUnionAngle(&HDarkRgn, Param.m_iConnectionLengthDC, 10);
				}
				else
				{
					BlobUnion(&HDarkRgn, Param.m_iConnectionLengthDC);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
			{
				Union1(HDarkRgn, &HDarkRgn);
				ConcatObj(HDefectRgn, HDarkRgn, &HDefectRgn);
			}
		}

		if (bDebugSave)
		{
			WriteRegion(HDefectRgn, "c:\\DualTest\\DefectCandidateRgn");
		}

		Connection(HDefectRgn, &HDefectRgn);
		if (Param.m_bUseDefectConditionAreaDC && Param.m_iDefectConditionAreaMinDC > 0 && Param.m_iDefectConditionAreaMaxDC > 0 && Param.m_iDefectConditionAreaMinDC <= Param.m_iDefectConditionAreaMaxDC)
			SelectShape(HDefectRgn, &HDefectRgn, "area", "and", Param.m_iDefectConditionAreaMinDC, Param.m_iDefectConditionAreaMaxDC);

		if (Param.m_iLengthWidthConditionDC == CONDITION_AND)
		{
			if (Param.m_bUseDefectConditionLengthDC && Param.m_iDefectConditionLengthMinDC > 0 && Param.m_iDefectConditionLengthMaxDC > 0 && Param.m_iDefectConditionLengthMinDC <= Param.m_iDefectConditionLengthMaxDC)
				SelectShape(HDefectRgn, &HDefectRgn, "max_diameter", "and", Param.m_iDefectConditionLengthMinDC, Param.m_iDefectConditionLengthMaxDC);
			if (Param.m_bUseDefectConditionWidthDC && Param.m_iDefectConditionWidthMinDC > 0 && Param.m_iDefectConditionWidthMaxDC > 0 && Param.m_iDefectConditionWidthMinDC <= Param.m_iDefectConditionWidthMaxDC)
				SelectShape(HDefectRgn, &HDefectRgn, "inner_radius", "and", Param.m_iDefectConditionWidthMinDC / 2, Param.m_iDefectConditionWidthMaxDC / 2);
		}
		else
		{
			BOOL bApplyCondition = FALSE;

			if (Param.m_bUseDefectConditionLengthDC && Param.m_iDefectConditionLengthMinDC > 0 && Param.m_iDefectConditionLengthMaxDC > 0 && Param.m_iDefectConditionLengthMinDC <= Param.m_iDefectConditionLengthMaxDC)
				bApplyCondition = TRUE;
			if (Param.m_bUseDefectConditionWidthDC && Param.m_iDefectConditionWidthMinDC > 0 && Param.m_iDefectConditionWidthMaxDC > 0 && Param.m_iDefectConditionWidthMinDC <= Param.m_iDefectConditionWidthMaxDC)
				bApplyCondition = TRUE;

			if (bApplyCondition)
			{
				HObject HLengthConditionPassRgn, HWidthConditionPassRgn;

				GenEmptyObj(&HLengthConditionPassRgn);
				GenEmptyObj(&HWidthConditionPassRgn);

				if (Param.m_bUseDefectConditionLengthDC && Param.m_iDefectConditionLengthMinDC > 0 && Param.m_iDefectConditionLengthMaxDC > 0 && Param.m_iDefectConditionLengthMinDC <= Param.m_iDefectConditionLengthMaxDC)
					SelectShape(HDefectRgn, &HLengthConditionPassRgn, "max_diameter", "and", Param.m_iDefectConditionLengthMinDC, Param.m_iDefectConditionLengthMaxDC);
				if (Param.m_bUseDefectConditionWidthDC && Param.m_iDefectConditionWidthMinDC > 0 && Param.m_iDefectConditionWidthMaxDC > 0 && Param.m_iDefectConditionWidthMinDC <= Param.m_iDefectConditionWidthMaxDC)
					SelectShape(HDefectRgn, &HWidthConditionPassRgn, "inner_radius", "and", Param.m_iDefectConditionWidthMinDC / 2, Param.m_iDefectConditionWidthMaxDC / 2);

				GenEmptyObj(&HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HLengthConditionPassRgn) == TRUE)
					ConcatObj(HDefectRgn, HLengthConditionPassRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HWidthConditionPassRgn) == TRUE)
					ConcatObj(HDefectRgn, HWidthConditionPassRgn, &HDefectRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					Union1(HDefectRgn, &HDefectRgn);
					Connection(HDefectRgn, &HDefectRgn);
				}
			}
		}

		if (Param.m_bUseDefectConditionMeanDC && (Param.m_iDefectConditionMeanMinDC >= 0 && Param.m_iDefectConditionMeanMinDC <= 255 && Param.m_iDefectConditionMeanMaxDC >= 0 && Param.m_iDefectConditionMeanMaxDC <= 255 && Param.m_iDefectConditionMeanMinDC <= Param.m_iDefectConditionMeanMaxDC))
			SelectGray(HDefectRgn, HImage, &HDefectRgn, "mean", "and", (double)Param.m_iDefectConditionMeanMinDC, (double)Param.m_iDefectConditionMeanMaxDC);
		if (Param.m_bUseDefectConditionStdevDC && Param.m_dDefectConditionStdevMinDC > 0 && Param.m_dDefectConditionStdevMaxDC > 0 && Param.m_dDefectConditionStdevMinDC <= Param.m_dDefectConditionStdevMaxDC)
			SelectGray(HDefectRgn, HImage, &HDefectRgn, "deviation", "and", Param.m_dDefectConditionStdevMinDC, Param.m_dDefectConditionStdevMaxDC);
		if (Param.m_bUseDefectConditionCircularityDC && Param.m_dDefectConditionCircularityMinDC >= 0 && Param.m_dDefectConditionCircularityMaxDC >= 0 &&
			Param.m_dDefectConditionCircularityMinDC <= 1 && Param.m_dDefectConditionCircularityMaxDC <= 1 && Param.m_dDefectConditionCircularityMinDC <= Param.m_dDefectConditionCircularityMaxDC)
			SelectShape(HDefectRgn, &HDefectRgn, "circularity", "and", Param.m_dDefectConditionCircularityMinDC, Param.m_dDefectConditionCircularityMaxDC);
		if (Param.m_bUseDefectConditionRectangularityDC && Param.m_dDefectConditionRectangularityMinDC >= 0 && Param.m_dDefectConditionRectangularityMaxDC >= 0 &&
			Param.m_dDefectConditionRectangularityMinDC <= 1 && Param.m_dDefectConditionRectangularityMaxDC <= 1 && Param.m_dDefectConditionRectangularityMinDC <= Param.m_dDefectConditionRectangularityMaxDC)
			SelectShape(HDefectRgn, &HDefectRgn, "rectangularity", "and", Param.m_dDefectConditionRectangularityMinDC, Param.m_dDefectConditionRectangularityMaxDC);
		if (Param.m_bUseDefectConditionConvexityDC && Param.m_dDefectConditionConvexityMinDC >= 0 && Param.m_dDefectConditionConvexityMaxDC >= 0 &&
			Param.m_dDefectConditionConvexityMinDC <= 1 && Param.m_dDefectConditionConvexityMaxDC <= 1 && Param.m_dDefectConditionConvexityMinDC <= Param.m_dDefectConditionConvexityMaxDC)
			SelectShape(HDefectRgn, &HDefectRgn, "convexity", "and", Param.m_dDefectConditionConvexityMinDC, Param.m_dDefectConditionConvexityMaxDC);
		if (Param.m_bUseDefectConditionEllipseRatioDC && Param.m_dDefectConditionEllipseRatioMinDC >= 1.0 && Param.m_dDefectConditionEllipseRatioMaxDC >= 1.0 &&
			Param.m_dDefectConditionEllipseRatioMinDC <= Param.m_dDefectConditionEllipseRatioMaxDC)
			SelectShape(HDefectRgn, &HDefectRgn, "anisometry", "and", Param.m_dDefectConditionEllipseRatioMinDC, Param.m_dDefectConditionEllipseRatioMaxDC);
		if (Param.m_bUseDefectConditionContrastDC)
		{
			HObject HDefectConRgn;
			Connection(HDefectRgn, &HDefectConRgn);

			Hlong lNoDefect;
			HTuple HlNoDefect;
			CountObj(HDefectConRgn, &HlNoDefect);
			lNoDefect = HlNoDefect.L();

			HObject HSelectedRgn, HFillupRgn, HDilatedRgn, HBGRgn, HBGRgnImageReduced, HDefectBoundaryRgn;
			double dBgGray, dDefectGray, dSd, dContrast;
			int iBGMinThres, iBGMaxThres;

			int iContrastCheckWidthPxl = 3;

			GenEmptyObj(&HDefectRgn);

			for (i = 1; i <= lNoDefect; i++)
			{
				SelectObj(HDefectConRgn, &HSelectedRgn, i);

				if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
					continue;

				FillUp(HSelectedRgn, &HFillupRgn);
				DilationCircle(HFillupRgn, &HDilatedRgn, (double)iContrastCheckWidthPxl + 0.5);
				Difference(HDilatedRgn, HFillupRgn, &HBGRgn);
				Intersection(HBGRgn, HInspectAreaRgn, &HBGRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HBGRgn) == FALSE)	// Contrast 측정 불가
				{
					ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
					continue;
				}

				HTuple HdBgGray, HdSd;
				Intensity(HBGRgn, HImage, &HdBgGray, &HdSd);
				dBgGray = HdBgGray.D();
				dSd = HdSd.D();

				DilationCircle(HBGRgn, &HDilatedRgn, (double)iContrastCheckWidthPxl + 0.5);
				Intersection(HSelectedRgn, HDilatedRgn, &HDefectBoundaryRgn);


				HTuple HdDefectGray;
				Intensity(HDefectBoundaryRgn, HImage, &HdDefectGray, &HdSd);
				dDefectGray = HdDefectGray.D();
				dSd = HdSd.D();

				dContrast = fabs(dBgGray - dDefectGray);

				if (dContrast >= Param.m_iDefectConditionContrastMinDC && dContrast <= Param.m_iDefectConditionContrastMaxDC)
					ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
			}
		}

		if (Param.m_bUseDefectConditionAreaRatioDC && Param.m_iDefectConditionAreaRatioMinDC >= 0 && Param.m_iDefectConditionAreaRatioMaxDC <= 100 &&
			Param.m_iDefectConditionAreaRatioMinDC <= Param.m_iDefectConditionAreaRatioMaxDC)
		{
			HObject HUnionRgn;
			Hlong lBlobArea, lROIArea;
			double dBlobCenterX, dBlobCenterY;
			HTuple HlROIArea, HlBlobArea, HdBlobCenterX, HdBlobCenterY;
			int iAreaRatio;

			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				Union1(HDefectRgn, &HUnionRgn);

				AreaCenter(HUnionRgn, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
				lBlobArea = HlBlobArea.L();
				dBlobCenterY = HdBlobCenterY.D();
				dBlobCenterX = HdBlobCenterX.D();
				AreaCenter(HInspectAreaRgn, &HlROIArea, &HdBlobCenterY, &HdBlobCenterX);
				lROIArea = HlROIArea.L();
				dBlobCenterY = HdBlobCenterY.D();
				dBlobCenterX = HdBlobCenterX.D();
				if (lROIArea > 0)
				{
					iAreaRatio = (int)((double)lBlobArea / (double)lROIArea * 100.0);
					if (iAreaRatio < 0)
						iAreaRatio = 0;
					if (iAreaRatio > 100)
						iAreaRatio = 100;

					if ((iAreaRatio >= Param.m_iDefectConditionAreaRatioMinDC && iAreaRatio <= Param.m_iDefectConditionAreaRatioMaxDC) == FALSE)
						GenEmptyObj(&HDefectRgn);
				}
			}
		}

		if (Param.m_bUseDefectConditionAnisometryDC && Param.m_dDefectConditionAnisometryMinDC >= 1.0 && Param.m_dDefectConditionAnisometryMaxDC >= 1.0 && Param.m_dDefectConditionAnisometryMinDC <= Param.m_dDefectConditionAnisometryMaxDC)
		{
			Hlong lNoBlob;
			Hlong lRow1, lRow2, lCol1, lCol2;
			HTuple HlRow1, HlRow2, HlCol1, HlCol2;
			double dBlobLength, dBlobWidth;
			HTuple HdBlobLength, HdBlobWidth;
			double dCircleRow, dCircleCol, dCircleRadius, dRatio;
			HTuple HdCircleRow, HdCircleCol, HdCircleRadius, HdRatio;
			HObject HConnInputRgn, HSelectedRgn;

			Connection(HDefectRgn, &HConnInputRgn);
			HTuple HlNoBlob;
			CountObj(HConnInputRgn, &HlNoBlob);
			lNoBlob = HlNoBlob.L();

			GenEmptyObj(&HDefectRgn);

			for (i = 1; i <= lNoBlob; i++)
			{
				SelectObj(HConnInputRgn, &HSelectedRgn, i);

				if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
					continue;

				DiameterRegion(HSelectedRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2, &HdBlobLength);
				lRow1 = HlRow1.L();
				lCol1 = HlCol1.L();
				lRow2 = HlRow2.L();
				lCol2 = HlCol2.L();
				dBlobLength = HdBlobLength.D();

				InnerCircle(HSelectedRgn, &HdCircleRow, &HdCircleCol, &HdCircleRadius);
				dCircleRow = HdCircleRow.D();
				dCircleCol = HdCircleCol.D();
				dCircleRadius = HdCircleRadius.D();

				dBlobWidth = dCircleRadius * 2.0;

				if (dBlobWidth <= 0)
					continue;

				dRatio = dBlobLength / dBlobWidth;

				if (dRatio <= Param.m_dDefectConditionAnisometryMaxDC && dRatio >= Param.m_dDefectConditionAnisometryMinDC)
					ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
			}
		}

		if (Param.m_bUseDefectConditionInnerDistDC && Param.m_iDefectConditionInnerDistDC >= 0)	// 내측 기준선
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HInspectAreaFillRgn, HInspectAreaInnerRgn;

				Hlong lNoBlob;
				HObject HInspectAreaConnRgn;
				HObject HConnInputRgn, HSelectedRgn, HDiffRgn;

				Connection(HInspectAreaRgn, &HInspectAreaConnRgn);
				HTuple HlNoBlob;
				CountObj(HInspectAreaConnRgn, &HlNoBlob);
				lNoBlob = HlNoBlob.L();

				if (lNoBlob == 1)		// 검사영역 1개 있을때만 사용
				{
					FillUp(HInspectAreaRgn, &HInspectAreaFillRgn);
					Difference(HInspectAreaFillRgn, HInspectAreaRgn, &HInspectAreaInnerRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaInnerRgn) == FALSE)	// 내부 홀이 없는 검사영역인 경우 && 중심으로부터 일정거리 내에 있으면 불량
					{
						Hlong lTempArea;
						double dTempCenterX, dTempCenterY;
						HObject HROICenterRgn;
						HTuple HlTempArea, HdTempCenterX, HdTempCenterY;

						AreaCenter(HInspectAreaRgn, &HlTempArea, &HdTempCenterY, &HdTempCenterX);
						lTempArea = HlTempArea.L();
						dTempCenterY = HdTempCenterY.D();
						dTempCenterX = HdTempCenterX.D();

						GenRegionPoints(&HROICenterRgn, (int)dTempCenterY, (int)dTempCenterX);

						DilationCircle(HROICenterRgn, &HInspectAreaInnerRgn, (double)Param.m_iDefectConditionInnerDistDC + 0.5);

						Connection(HDefectRgn, &HConnInputRgn);
						HTuple HlNoBlob;
						CountObj(HConnInputRgn, &HlNoBlob);
						lNoBlob = HlNoBlob.L();

						GenEmptyObj(&HDefectRgn);

						for (i = 1; i <= lNoBlob; i++)
						{
							SelectObj(HConnInputRgn, &HSelectedRgn, i);

							if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
								continue;

							Intersection(HSelectedRgn, HInspectAreaInnerRgn, &HDiffRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HDiffRgn) == TRUE)
								ConcatObj(HDefectRgn, HSelectedRgn, &HDefectRgn);
						}
					}
				}
			}
		}

		if (Param.m_bUseDefectConditionOuterDistDC && Param.m_iDefectConditionOuterDistDC >= 0)		// 외측 기준선
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HInspectAreaOuterRgn;

				FillUp(HInspectAreaRgn, &HInspectAreaOuterRgn);
				ErosionCircle(HInspectAreaOuterRgn, &HInspectAreaOuterRgn, (double)Param.m_iDefectConditionOuterDistDC + 0.5);

				if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaOuterRgn) == TRUE)
				{
					SelectShapeProto(HDefectRgn, HInspectAreaOuterRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
				}
				else
				{
					GenEmptyObj(&HDefectRgn);
				}
			}
		}

		if (Param.m_bDefectConditionBoundaryConnectionDC)		// 외측 기준선 연결성
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HInspectAreaOrgRgn, HInspectAreaRectRgn;

				FillUp(HInspectAreaRgn, &HInspectAreaOrgRgn);
				ShapeTrans(HInspectAreaOrgRgn, &HInspectAreaRectRgn, "rectangle1");

				Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
				Hlong lOrgLTPointY, lOrgLTPointX, lOrgRBPointY, lOrgRBPointX;
				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

				SmallestRectangle1(HInspectAreaRectRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

				lOrgLTPointY = HlLTPointY.L();
				lOrgLTPointX = HlLTPointX.L();
				lOrgRBPointY = HlRBPointY.L();
				lOrgRBPointX = HlRBPointX.L();

				if (Param.m_iBoundaryConnectionConditionDC == CONDITION_OR)
				{
					lLTPointY = lOrgLTPointY;
					lLTPointX = lOrgLTPointX;
					lRBPointY = lOrgRBPointY;
					lRBPointX = lOrgRBPointX;

					if (Param.m_bDefectConditionBoundaryMarginTopDC && Param.m_iDefectConditionBoundaryMarginTopDC > 0)
						lLTPointY = lOrgLTPointY + Param.m_iDefectConditionBoundaryMarginTopDC;
					if (Param.m_bDefectConditionBoundaryMarginBottomDC && Param.m_iDefectConditionBoundaryMarginBottomDC > 0)
						lRBPointY = lOrgRBPointY - Param.m_iDefectConditionBoundaryMarginBottomDC;
					if (Param.m_bDefectConditionBoundaryMarginLeftDC && Param.m_iDefectConditionBoundaryMarginLeftDC > 0)
						lLTPointX = lOrgLTPointX + Param.m_iDefectConditionBoundaryMarginLeftDC;
					if (Param.m_bDefectConditionBoundaryMarginRightDC && Param.m_iDefectConditionBoundaryMarginRightDC > 0)
						lRBPointX = lOrgRBPointX - Param.m_iDefectConditionBoundaryMarginRightDC;

					HObject HInsideDiffRgn, HBoundaryCheckRgn;
					GenEmptyObj(&HInsideDiffRgn);
					GenEmptyObj(&HBoundaryCheckRgn);

					if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
						GenRectangle1(&HInsideDiffRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

					if (THEAPP.m_pGFunction->ValidHRegion(HInsideDiffRgn))
						Difference(HInspectAreaOrgRgn, HInsideDiffRgn, &HBoundaryCheckRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
					{
						if (Param.m_iDefectConditionBoundaryMarginMinConnectionDC > 1)
							SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnectionDC, 9999999);
						else
							SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
					}
				}
				else
				{
					HObject HInsideDiffRgn, HBoundaryCheckRgn;

					if (Param.m_bDefectConditionBoundaryMarginTopDC && Param.m_iDefectConditionBoundaryMarginTopDC > 0)
					{
						lLTPointY = lOrgLTPointY;
						lLTPointX = lOrgLTPointX;
						lRBPointY = lOrgRBPointY;
						lRBPointX = lOrgRBPointX;

						lLTPointY = lOrgLTPointY + Param.m_iDefectConditionBoundaryMarginTopDC;

						GenEmptyObj(&HInsideDiffRgn);
						GenEmptyObj(&HBoundaryCheckRgn);

						if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
							GenRectangle1(&HInsideDiffRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

						if (THEAPP.m_pGFunction->ValidHRegion(HInsideDiffRgn))
							Difference(HInspectAreaOrgRgn, HInsideDiffRgn, &HBoundaryCheckRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
						{
							if (Param.m_iDefectConditionBoundaryMarginMinConnectionDC > 1)
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnectionDC, 9999999);
							else
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
						}
					}

					if (Param.m_bDefectConditionBoundaryMarginBottomDC && Param.m_iDefectConditionBoundaryMarginBottomDC > 0)
					{
						lLTPointY = lOrgLTPointY;
						lLTPointX = lOrgLTPointX;
						lRBPointY = lOrgRBPointY;
						lRBPointX = lOrgRBPointX;

						lRBPointY = lOrgRBPointY - Param.m_iDefectConditionBoundaryMarginBottomDC;

						GenEmptyObj(&HInsideDiffRgn);
						GenEmptyObj(&HBoundaryCheckRgn);

						if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
							GenRectangle1(&HInsideDiffRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

						if (THEAPP.m_pGFunction->ValidHRegion(HInsideDiffRgn))
							Difference(HInspectAreaOrgRgn, HInsideDiffRgn, &HBoundaryCheckRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
						{
							if (Param.m_iDefectConditionBoundaryMarginMinConnectionDC > 1)
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnectionDC, 9999999);
							else
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
						}
					}

					if (Param.m_bDefectConditionBoundaryMarginLeftDC && Param.m_iDefectConditionBoundaryMarginLeftDC > 0)
					{
						lLTPointY = lOrgLTPointY;
						lLTPointX = lOrgLTPointX;
						lRBPointY = lOrgRBPointY;
						lRBPointX = lOrgRBPointX;

						lLTPointX = lOrgLTPointX + Param.m_iDefectConditionBoundaryMarginLeftDC;

						GenEmptyObj(&HInsideDiffRgn);
						GenEmptyObj(&HBoundaryCheckRgn);

						if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
							GenRectangle1(&HInsideDiffRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

						if (THEAPP.m_pGFunction->ValidHRegion(HInsideDiffRgn))
							Difference(HInspectAreaOrgRgn, HInsideDiffRgn, &HBoundaryCheckRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
						{
							if (Param.m_iDefectConditionBoundaryMarginMinConnectionDC > 1)
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnectionDC, 9999999);
							else
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
						}
					}

					if (Param.m_bDefectConditionBoundaryMarginRightDC && Param.m_iDefectConditionBoundaryMarginRightDC > 0)
					{
						lLTPointY = lOrgLTPointY;
						lLTPointX = lOrgLTPointX;
						lRBPointY = lOrgRBPointY;
						lRBPointX = lOrgRBPointX;

						lRBPointX = lOrgRBPointX - Param.m_iDefectConditionBoundaryMarginRightDC;

						GenEmptyObj(&HInsideDiffRgn);
						GenEmptyObj(&HBoundaryCheckRgn);

						if (lLTPointY <= lRBPointY && lLTPointX <= lRBPointX)
							GenRectangle1(&HInsideDiffRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

						if (THEAPP.m_pGFunction->ValidHRegion(HInsideDiffRgn))
							Difference(HInspectAreaOrgRgn, HInsideDiffRgn, &HBoundaryCheckRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryCheckRgn) == TRUE)
						{
							if (Param.m_iDefectConditionBoundaryMarginMinConnectionDC > 1)
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", Param.m_iDefectConditionBoundaryMarginMinConnectionDC, 9999999);
							else
								SelectShapeProto(HDefectRgn, HBoundaryCheckRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
						}
					}
				}
			}
		}

		if (Param.m_bDefectConditionBoundaryConnection2SideDC)		// 외측 기준선 연결성 (2면이상)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				HObject HInspectAreaOrgRgn, HInspectAreaRectRgn;

				FillUp(HInspectAreaRgn, &HInspectAreaOrgRgn);
				ShapeTrans(HInspectAreaOrgRgn, &HInspectAreaRectRgn, "rectangle1");

				Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
				Hlong lOrgLTPointY, lOrgLTPointX, lOrgRBPointY, lOrgRBPointX;
				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

				SmallestRectangle1(HInspectAreaRectRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

				lOrgLTPointY = HlLTPointY.L();
				lOrgLTPointX = HlLTPointX.L();
				lOrgRBPointY = HlRBPointY.L();
				lOrgRBPointX = HlRBPointX.L();

				HObject HTopLineRgn, HBottomLineRgn, HLeftLineRgn, HRightLineRgn;

				lLTPointY = lOrgLTPointY + Param.m_iDefectConditionBoundaryMarginTopDC;
				lRBPointY = lOrgRBPointY - Param.m_iDefectConditionBoundaryMarginBottomDC;
				lLTPointX = lOrgLTPointX + Param.m_iDefectConditionBoundaryMarginLeftDC;
				lRBPointX = lOrgRBPointX - Param.m_iDefectConditionBoundaryMarginRightDC;

				GenRectangle1(&HTopLineRgn, lLTPointY, lLTPointX + 1, lLTPointY, lRBPointX - 1);
				GenRectangle1(&HBottomLineRgn, lRBPointY, lLTPointX + 1, lRBPointY, lRBPointX - 1);
				GenRectangle1(&HLeftLineRgn, lLTPointY + 1, lLTPointX, lRBPointY - 1, lLTPointX);
				GenRectangle1(&HRightLineRgn, lLTPointY + 1, lRBPointX, lRBPointY - 1, lRBPointX);

				HObject HTopBottomRgn, HTopLeftRgn, HTopRightRgn, HBottomLeftRgn, HBottomRightRgn, HLeftRightRgn;
				HObject HTempConnRgn;

				GenEmptyObj(&HTopBottomRgn);
				SelectShapeProto(HDefectRgn, HTopLineRgn, &HTopBottomRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HTopBottomRgn, HBottomLineRgn, &HTopBottomRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HTopLeftRgn);
				SelectShapeProto(HDefectRgn, HTopLineRgn, &HTopLeftRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HTopLeftRgn, HLeftLineRgn, &HTopLeftRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HTopRightRgn);
				SelectShapeProto(HDefectRgn, HTopLineRgn, &HTopRightRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HTopRightRgn, HRightLineRgn, &HTopRightRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HBottomLeftRgn);
				SelectShapeProto(HDefectRgn, HBottomLineRgn, &HBottomLeftRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HBottomLeftRgn, HLeftLineRgn, &HBottomLeftRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HBottomRightRgn);
				SelectShapeProto(HDefectRgn, HBottomLineRgn, &HBottomRightRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HBottomRightRgn, HRightLineRgn, &HBottomRightRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HLeftRightRgn);
				SelectShapeProto(HDefectRgn, HLeftLineRgn, &HLeftRightRgn, "overlaps_abs", 1, 9999999);
				SelectShapeProto(HLeftRightRgn, HRightLineRgn, &HLeftRightRgn, "overlaps_abs", 1, 9999999);

				GenEmptyObj(&HDefectRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HTopBottomRgn))
					ConcatObj(HDefectRgn, HTopBottomRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HTopLeftRgn))
					ConcatObj(HDefectRgn, HTopLeftRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HTopRightRgn))
					ConcatObj(HDefectRgn, HTopRightRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HBottomLeftRgn))
					ConcatObj(HDefectRgn, HBottomLeftRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HBottomRightRgn))
					ConcatObj(HDefectRgn, HBottomRightRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HLeftRightRgn))
					ConcatObj(HDefectRgn, HLeftRightRgn, &HDefectRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn))
				{
					Union1(HDefectRgn, &HDefectRgn);
					Connection(HDefectRgn, &HDefectRgn);
				}
			}
		}

		if (Param.m_bUseDefectConditionBlobDC && Param.m_iDefectConditionBlobMinDC >= 1 && Param.m_iDefectConditionBlobMaxDC >= 1 && Param.m_iDefectConditionBlobMinDC <= Param.m_iDefectConditionBlobMaxDC)
		{
			Hlong lNoDefectBlob = 0;
			HTuple HlNoDefectBlob;
			CountObj(HDefectRgn, &HlNoDefectBlob);
			lNoDefectBlob = HlNoDefectBlob.L();

			if (lNoDefectBlob<Param.m_iDefectConditionBlobMinDC || lNoDefectBlob>Param.m_iDefectConditionBlobMaxDC)
				GenEmptyObj(&HDefectRgn);
		}

		if ((Param.m_bUseDefectConditionShiftXDC && Param.m_iDefectConditionShiftXMinDC >= 0 && Param.m_iDefectConditionShiftXMaxDC >= 0 && Param.m_iDefectConditionShiftXMinDC <= Param.m_iDefectConditionShiftXMaxDC) == TRUE ||
			(Param.m_bUseDefectConditionShiftYDC && Param.m_iDefectConditionShiftYMinDC >= 0 && Param.m_iDefectConditionShiftYMaxDC >= 0 && Param.m_iDefectConditionShiftYMinDC <= Param.m_iDefectConditionShiftYMaxDC) == TRUE)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				Hlong lShiftTempArea;
				double dShiftInspectCenterX, dShiftInspectCenterY, dShiftDefectCenterX, dShiftDefectCenterY;
				double dShiftValueX, dShiftValueY;
				HTuple HlShiftTempArea, HdShiftInspectCenterY, HdShiftInspectCenterX, HdShiftDefectCenterY, HdShiftDefectCenterX;

				AreaCenter(HInspectAreaRgn, &HlShiftTempArea, &HdShiftInspectCenterY, &HdShiftInspectCenterX);
				lShiftTempArea = HlShiftTempArea.L();
				dShiftInspectCenterY = HdShiftInspectCenterY.D();
				dShiftInspectCenterX = HdShiftInspectCenterX.D();

				HObject HUnionRgn;
				Union1(HDefectRgn, &HUnionRgn);

				AreaCenter(HUnionRgn, &HlShiftTempArea, &HdShiftDefectCenterY, &HdShiftDefectCenterX);
				lShiftTempArea = HlShiftTempArea.L();
				dShiftDefectCenterY = HdShiftDefectCenterY.D();
				dShiftDefectCenterX = HdShiftDefectCenterX.D();

				dShiftValueX = fabs(dShiftInspectCenterX - dShiftDefectCenterX);
				dShiftValueY = fabs(dShiftInspectCenterY - dShiftDefectCenterY);

				if (Param.m_bUseDefectConditionShiftXDC)
				{
					if ((dShiftValueX >= Param.m_iDefectConditionShiftXMinDC && dShiftValueX <= Param.m_iDefectConditionShiftXMaxDC) == FALSE)
						GenEmptyObj(&HDefectRgn);
				}

				if (Param.m_bUseDefectConditionShiftYDC)
				{
					if ((dShiftValueY >= Param.m_iDefectConditionShiftYMinDC && dShiftValueY <= Param.m_iDefectConditionShiftYMaxDC) == FALSE)
						GenEmptyObj(&HDefectRgn);
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		////  Area, XLength, YLength 불량 조건 적용 전에 Union Blob의 Area, XLength, YLength
		Hlong lDefectArea;
		double dDefectCenterX, dDefectCenterY;
		HTuple HlDefectArea, HdDefectCenterX, HdDefectCenterY;
		Hlong lDefectRow1, lDefectRow2, lDefectCol1, lDefectCol2, lDefectXLength, lDefectYLength;
		HObject HMeasureDefectRgn, MaxAreaRgn, MaxWidthRgn, MaxHeightRgn;
		HTuple HIndices, HInverted;
		long lMaxIndex;
		HTuple HDefectRow1, HDefectCol1, HDefectRow2, HDefectCol2;
		HTuple HXLength, HYLength;

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
		{
			Connection(HDefectRgn, &HMeasureDefectRgn);

			// Max Area
			SelectShapeStd(HMeasureDefectRgn, &MaxAreaRgn, "max_area", 70);
			AreaCenter(MaxAreaRgn, &HlDefectArea, &HdDefectCenterY, &HdDefectCenterX);
			lDefectArea = HlDefectArea.L();
			dDefectCenterY = HdDefectCenterY.D();
			dDefectCenterX = HdDefectCenterX.D();
			*piArea = lDefectArea;

			SmallestRectangle1(HMeasureDefectRgn, &HDefectRow1, &HDefectCol1, &HDefectRow2, &HDefectCol2);

			// Max Width
			HXLength = HDefectCol2 - HDefectCol1 + 1;
			TupleSortIndex(HXLength, &HIndices);
			TupleInverse(HIndices, &HInverted);
			lMaxIndex = HInverted[0].L();
			*piXLength = HXLength[lMaxIndex];

			// Max Length
			HYLength = HDefectRow2 - HDefectRow1 + 1;
			TupleSortIndex(HYLength, &HIndices);
			TupleInverse(HIndices, &HInverted);
			lMaxIndex = HInverted[0].L();
			*piYLength = HYLength[lMaxIndex];
		}
		//////////////////////////////////////////////////////////////////////////

		if (Param.m_iXYLengthConditionDC == CONDITION_AND)
		{
			if (Param.m_bUseDefectConditionXLengthDC && Param.m_iDefectConditionXLengthMinDC > 0 && Param.m_iDefectConditionXLengthMaxDC > 0 && Param.m_iDefectConditionXLengthMinDC <= Param.m_iDefectConditionXLengthMaxDC)
				SelectShape(HDefectRgn, &HDefectRgn, "width", "and", Param.m_iDefectConditionXLengthMinDC, Param.m_iDefectConditionXLengthMaxDC);
			if (Param.m_bUseDefectConditionYLengthDC && Param.m_iDefectConditionYLengthMinDC > 0 && Param.m_iDefectConditionYLengthMaxDC > 0 && Param.m_iDefectConditionYLengthMinDC <= Param.m_iDefectConditionYLengthMaxDC)
				SelectShape(HDefectRgn, &HDefectRgn, "height", "and", Param.m_iDefectConditionYLengthMinDC, Param.m_iDefectConditionYLengthMaxDC);
		}
		else
		{
			BOOL bApplyCondition = FALSE;

			if (Param.m_bUseDefectConditionXLengthDC && Param.m_iDefectConditionXLengthMinDC > 0 && Param.m_iDefectConditionXLengthMaxDC > 0 && Param.m_iDefectConditionXLengthMinDC <= Param.m_iDefectConditionXLengthMaxDC)
				bApplyCondition = TRUE;
			if (Param.m_bUseDefectConditionYLengthDC && Param.m_iDefectConditionYLengthMinDC > 0 && Param.m_iDefectConditionYLengthMaxDC > 0 && Param.m_iDefectConditionYLengthMinDC <= Param.m_iDefectConditionYLengthMaxDC)
				bApplyCondition = TRUE;

			if (bApplyCondition)
			{
				HObject HXLengthConditionPassRgn, HYLengthConditionPassRgn;

				GenEmptyObj(&HXLengthConditionPassRgn);
				GenEmptyObj(&HYLengthConditionPassRgn);

				if (Param.m_bUseDefectConditionXLengthDC && Param.m_iDefectConditionXLengthMinDC > 0 && Param.m_iDefectConditionXLengthMaxDC > 0 && Param.m_iDefectConditionXLengthMinDC <= Param.m_iDefectConditionXLengthMaxDC)
					SelectShape(HDefectRgn, &HXLengthConditionPassRgn, "width", "and", Param.m_iDefectConditionXLengthMinDC, Param.m_iDefectConditionXLengthMaxDC);
				if (Param.m_bUseDefectConditionYLengthDC && Param.m_iDefectConditionYLengthMinDC > 0 && Param.m_iDefectConditionYLengthMaxDC > 0 && Param.m_iDefectConditionYLengthMinDC <= Param.m_iDefectConditionYLengthMaxDC)
					SelectShape(HDefectRgn, &HYLengthConditionPassRgn, "height", "and", Param.m_iDefectConditionYLengthMinDC, Param.m_iDefectConditionYLengthMaxDC);

				GenEmptyObj(&HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HXLengthConditionPassRgn) == TRUE)
					ConcatObj(HDefectRgn, HXLengthConditionPassRgn, &HDefectRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HYLengthConditionPassRgn) == TRUE)
					ConcatObj(HDefectRgn, HYLengthConditionPassRgn, &HDefectRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					Union1(HDefectRgn, &HDefectRgn);
					Connection(HDefectRgn, &HDefectRgn);
				}
			}
		}

		if (Param.m_bUseDefectConditionUseZeroAreaDC)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				GenEmptyObj(&HDefectRgn);
			else
			{
				HDefectRgn = HInspectAreaRgn;
			}
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::DefectContidionDC] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

BOOL Algorithm::GetScaledImage(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHScaledImage)
{
	try
	{
		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return FALSE;

		HObject HProcessImage;

		HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
		long lPeak, lHystUpperThreshold, lHystLowThreshold;

		int iScaleMin, iScaleMax;
		int iScalePeakUpperOffset = 20;
		int iScalePeakLowerOffset = 10;

		if (0)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\ScaleSurface");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\ScaleInsepctArea.reg");
		}

		if (Param.m_iImageScaleMethod == IMAGE_SCALE_AUTO)
		{
			GrayHisto(HInspectAreaRgn, HImage, &HAbsHisto, &HRelHisto);
			CreateFunct1dArray(HAbsHisto, &HFunction);
			SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
			TupleLastN(HSmoothFunction, 3, &HAbsHisto);
			TupleSortIndex(HAbsHisto, &Indices);
			TupleInverse(Indices, &Inverted);
			lPeak = Inverted[0].L();

			iScaleMax = lPeak + iScalePeakUpperOffset;
			if (iScaleMax > 255)
				iScaleMax = 255;

			iScaleMin = lPeak - iScalePeakLowerOffset;
			if (iScaleMin < 0)
				iScaleMin = 0;
		}
		else
		{
			if (Param.m_iImageScaleMin < 0)
				iScaleMin = 0;
			else if (Param.m_iImageScaleMin > 255)
				iScaleMin = 255;
			else
				iScaleMin = Param.m_iImageScaleMin;

			if (Param.m_iImageScaleMax < 0)
				iScaleMax = 0;
			else if (Param.m_iImageScaleMax > 255)
				iScaleMax = 255;
			else
				iScaleMax = Param.m_iImageScaleMax;

			if (iScaleMax < iScaleMin)
				iScaleMax = iScaleMin;
		}

		THEAPP.m_pGFunction->ScaleImageRange(HImage, &HProcessImage, HTuple(iScaleMin), HTuple(iScaleMax));
		BinomialFilter(HProcessImage, &HProcessImage, 3, 3);

		CopyImage(HProcessImage, pHScaledImage);

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::GetScaledImage] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::GetEdgeImage(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHEdgeImage)
{
	try
	{
		HObject HProcessImage;

		if (Param.m_iEdgeFilterType == IMAGE_EDGE_ANY_DIR)
			SobelAmp(HImage, &HProcessImage, "sum_abs", 3);
		else if (Param.m_iEdgeFilterType == IMAGE_EDGE_X_DIR)
		{
			SobelAmp(HImage, &HProcessImage, "x", 3);
			AbsImage(HProcessImage, &HProcessImage);
		}
		else if (Param.m_iEdgeFilterType == IMAGE_EDGE_Y_DIR)
		{
			SobelAmp(HImage, &HProcessImage, "y", 3);
			AbsImage(HProcessImage, &HProcessImage);
		}

		if (Param.m_dEdgeImageScale != 1)
			ScaleImage(HProcessImage, &HProcessImage, Param.m_dEdgeImageScale, 0);

		CopyImage(HProcessImage, pHEdgeImage);

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::GetEdgeImage] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::GetPreprocessImage(BOOL bUseDomain, HObject HOrgImage, HObject *pHImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHArithmeticImage)
{
	try
	{
		bUseDomain = TRUE;

		BOOL bDebugSave = FALSE;

		if (bDebugSave)
			WriteImage(HOrgImage, "bmp", 0, "c:\\DualTest\\ProcessingOrgImage");

		HObject HImageReduced1, HImageReduced2, HImageReduced3, HImageReduced4;
		int iExpandSize;
		HObject HExpandReduceImage;
		HObject HInspectAreaRgn;
		HObject HMaxImage, HMinImage;

		GenEmptyObj(&HInspectAreaRgn);

		GenEmptyObj(pHArithmeticImage);

		if (Param.m_bUseImageProcess)
		{
			if (bUseDomain)
			{
				GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
				if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
				{
					CopyImage(pHImage[Param.m_iImageProcessArithmeticImage1], pHArithmeticImage);
					return FALSE;
				}

				if (bDebugSave)
					WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");

				ReduceDomain(pHImage[Param.m_iImageProcessArithmeticImage1], HInspectAreaRgn, &HImageReduced1);
				ReduceDomain(pHImage[Param.m_iImageProcessArithmeticImage2], HInspectAreaRgn, &HImageReduced2);
				ReduceDomain(pHImage[Param.m_iImageProcessArithmeticImage3], HInspectAreaRgn, &HImageReduced3);
				ReduceDomain(pHImage[Param.m_iImageProcessArithmeticImage4], HInspectAreaRgn, &HImageReduced4);
			}
			else
			{
				CopyImage(pHImage[Param.m_iImageProcessArithmeticImage1], &HImageReduced1);
				CopyImage(pHImage[Param.m_iImageProcessArithmeticImage2], &HImageReduced2);
				CopyImage(pHImage[Param.m_iImageProcessArithmeticImage3], &HImageReduced3);
				CopyImage(pHImage[Param.m_iImageProcessArithmeticImage4], &HImageReduced4);
			}

			switch (Param.m_iImageProcessArithmeticType)
			{
			case IMAGE_PROCESS_ARITHMETIC_ADD:
				AddImage(HImageReduced1, HImageReduced2, pHArithmeticImage, Param.m_dImageProcessArithmeticMulti, (double)Param.m_iImageProcessArithmeticAdd);
				break;

			case IMAGE_PROCESS_ARITHMETIC_SUB:
				SubImage(HImageReduced1, HImageReduced2, pHArithmeticImage, Param.m_dImageProcessArithmeticMulti, (double)Param.m_iImageProcessArithmeticAdd);
				break;

			case IMAGE_PROCESS_ARITHMETIC_MAX:
			{
				MaxImage(HImageReduced1, HImageReduced2, &HMaxImage);
				MaxImage(HMaxImage, HImageReduced3, &HMaxImage);
				MaxImage(HMaxImage, HImageReduced4, pHArithmeticImage);
				break;
			}

			case IMAGE_PROCESS_ARITHMETIC_MIN:
			{
				MinImage(HImageReduced1, HImageReduced2, &HMinImage);
				MinImage(HMinImage, HImageReduced3, &HMinImage);
				MinImage(HMinImage, HImageReduced4, pHArithmeticImage);
				break;
			}

			case IMAGE_PROCESS_ARITHMETIC_DIFF_ABS:
			{
				MaxImage(HImageReduced1, HImageReduced2, &HMaxImage);
				MaxImage(HMaxImage, HImageReduced3, &HMaxImage);
				MaxImage(HMaxImage, HImageReduced4, &HMaxImage);

				MinImage(HImageReduced1, HImageReduced2, &HMinImage);
				MinImage(HMinImage, HImageReduced3, &HMinImage);
				MinImage(HMinImage, HImageReduced4, &HMinImage);

				SubImage(HMaxImage, HMinImage, pHArithmeticImage, Param.m_dImageProcessArithmeticMulti, 0);

				break;
			}

			default:
				CopyImage(pHImage[Param.m_iImageProcessArithmeticImage1], pHArithmeticImage);
				break;
			}

			if (Param.m_bUseImageProcessFilter)
			{
				if ((Param.m_iImageProcessFilterType1 == FILTER_TYPE_NOT_USED || Param.m_iImageProcessFilterType1 == FILTER_TYPE_MEDIAN || Param.m_iImageProcessFilterType1 == FILTER_TYPE_SHARPENING) == FALSE)
				{
					if (Param.m_iImageProcessFilterType1X > Param.m_iImageProcessFilterType1Y)
						iExpandSize = Param.m_iImageProcessFilterType1X / 2 + 1;
					else
						iExpandSize = Param.m_iImageProcessFilterType1Y / 2 + 1;

					ExpandDomainGray(*pHArithmeticImage, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HInspectAreaRgn, pHArithmeticImage);
				}

				switch (Param.m_iImageProcessFilterType1)
				{
				case FILTER_TYPE_NOT_USED:
					break;
				case FILTER_TYPE_MEAN:
					MeanImage(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y);
					break;
				case FILTER_TYPE_MEDIAN:
					MedianRect(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y);
					break;
				case FILTER_TYPE_GAUSS:
					BinomialFilter(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y);
					break;
				case FILTER_TYPE_SCALE_MAX:
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_EMPHASIZE:
					Emphasize(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y, 1.0);
					break;
				case FILTER_TYPE_GRAY_OPENING:
					GrayOpeningShape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y, "octagon");
					break;
				case FILTER_TYPE_GRAY_CLOSING:
					GrayClosingShape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y, "octagon");
					break;
				case FILTER_TYPE_EXPONENT:
					PowImage(*pHArithmeticImage, pHArithmeticImage, Param.m_iImageProcessFilterType1X);
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_LOG:
					LogImage(*pHArithmeticImage, pHArithmeticImage, "e");
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_ANISO_DIFFUSION:
					AnisotropicDiffusion(*pHArithmeticImage, pHArithmeticImage, "perona-malik", (int)Param.m_iImageProcessFilterType1X, 1, (int)Param.m_iImageProcessFilterType1Y);
					break;
				case FILTER_TYPE_SHARPENING:
					ShockFilter(*pHArithmeticImage, pHArithmeticImage, 0.5, 10, "canny", 1.5);
					break;
				default:
					break;
				}

				if ((Param.m_iImageProcessFilterType2 == FILTER_TYPE_NOT_USED || Param.m_iImageProcessFilterType2 == FILTER_TYPE_MEDIAN || Param.m_iImageProcessFilterType2 == FILTER_TYPE_SHARPENING) == FALSE)
				{
					if (Param.m_iImageProcessFilterType2X > Param.m_iImageProcessFilterType2Y)
						iExpandSize = Param.m_iImageProcessFilterType2X / 2 + 1;
					else
						iExpandSize = Param.m_iImageProcessFilterType2Y / 2 + 1;

					ExpandDomainGray(*pHArithmeticImage, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HInspectAreaRgn, pHArithmeticImage);
				}

				switch (Param.m_iImageProcessFilterType2)
				{
				case FILTER_TYPE_NOT_USED:
					break;
				case FILTER_TYPE_MEAN:
					MeanImage(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y);
					break;
				case FILTER_TYPE_MEDIAN:
					MedianRect(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y);
					break;
				case FILTER_TYPE_GAUSS:
					BinomialFilter(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y);
					break;
				case FILTER_TYPE_SCALE_MAX:
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_EMPHASIZE:
					Emphasize(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y, 1.0);
					break;
				case FILTER_TYPE_GRAY_OPENING:
					GrayOpeningShape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y, "octagon");
					break;
				case FILTER_TYPE_GRAY_CLOSING:
					GrayClosingShape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y, "octagon");
					break;
				case FILTER_TYPE_EXPONENT:
					PowImage(*pHArithmeticImage, pHArithmeticImage, Param.m_iImageProcessFilterType2X);
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_LOG:
					LogImage(*pHArithmeticImage, pHArithmeticImage, "e");
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_ANISO_DIFFUSION:
					AnisotropicDiffusion(*pHArithmeticImage, pHArithmeticImage, "perona-malik", (int)Param.m_iImageProcessFilterType2X, 1, (int)Param.m_iImageProcessFilterType2Y);
					break;
				case FILTER_TYPE_SHARPENING:
					ShockFilter(*pHArithmeticImage, pHArithmeticImage, 0.5, 10, "canny", 1.5);
					break;
				default:
					break;
				}

				if ((Param.m_iImageProcessFilterType3 == FILTER_TYPE_NOT_USED || Param.m_iImageProcessFilterType3 == FILTER_TYPE_MEDIAN || Param.m_iImageProcessFilterType3 == FILTER_TYPE_SHARPENING) == FALSE)
				{
					if (Param.m_iImageProcessFilterType3X > Param.m_iImageProcessFilterType3Y)
						iExpandSize = Param.m_iImageProcessFilterType3X / 2 + 1;
					else
						iExpandSize = Param.m_iImageProcessFilterType3Y / 2 + 1;

					ExpandDomainGray(*pHArithmeticImage, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HInspectAreaRgn, pHArithmeticImage);
				}

				switch (Param.m_iImageProcessFilterType3)
				{
				case FILTER_TYPE_NOT_USED:
					break;
				case FILTER_TYPE_MEAN:
					MeanImage(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y);
					break;
				case FILTER_TYPE_MEDIAN:
					MedianRect(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y);
					break;
				case FILTER_TYPE_GAUSS:
					BinomialFilter(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y);
					break;
				case FILTER_TYPE_SCALE_MAX:
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_EMPHASIZE:
					Emphasize(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y, 1.0);
					break;
				case FILTER_TYPE_GRAY_OPENING:
					GrayOpeningShape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y, "octagon");
					break;
				case FILTER_TYPE_GRAY_CLOSING:
					GrayClosingShape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y, "octagon");
					break;
				case FILTER_TYPE_EXPONENT:
					PowImage(*pHArithmeticImage, pHArithmeticImage, Param.m_iImageProcessFilterType3X);
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_LOG:
					LogImage(*pHArithmeticImage, pHArithmeticImage, "e");
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_ANISO_DIFFUSION:
					AnisotropicDiffusion(*pHArithmeticImage, pHArithmeticImage, "perona-malik", (int)Param.m_iImageProcessFilterType3X, 1, (int)Param.m_iImageProcessFilterType3Y);
					break;
				case FILTER_TYPE_SHARPENING:
					ShockFilter(*pHArithmeticImage, pHArithmeticImage, 0.5, 10, "canny", 1.5);
					break;
				default:
					break;
				}
			}
		}
		else
		{
			if (Param.m_bUseImageProcessFilter)
			{
				if (bUseDomain)
				{
					GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
					if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
					{
						CopyImage(HOrgImage, pHArithmeticImage);
						return FALSE;
					}

					if (bDebugSave)
						WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");

					ReduceDomain(HOrgImage, HInspectAreaRgn, &HImageReduced1);
				}
				else
				{
					HImageReduced1 = HOrgImage;
				}

				if ((Param.m_iImageProcessFilterType1 == FILTER_TYPE_NOT_USED || Param.m_iImageProcessFilterType1 == FILTER_TYPE_MEDIAN || Param.m_iImageProcessFilterType1 == FILTER_TYPE_SHARPENING) == FALSE)
				{
					if (Param.m_iImageProcessFilterType1X > Param.m_iImageProcessFilterType1Y)
						iExpandSize = Param.m_iImageProcessFilterType1X / 2 + 1;
					else
						iExpandSize = Param.m_iImageProcessFilterType1Y / 2 + 1;

					ExpandDomainGray(HImageReduced1, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HInspectAreaRgn, &HImageReduced1);
				}

				switch (Param.m_iImageProcessFilterType1)
				{
				case FILTER_TYPE_NOT_USED:
					break;
				case FILTER_TYPE_MEAN:
					MeanImage(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y);
					break;
				case FILTER_TYPE_MEDIAN:
					MedianRect(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y);
					break;
				case FILTER_TYPE_GAUSS:
					BinomialFilter(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y);
					break;
				case FILTER_TYPE_SCALE_MAX:
					ScaleImageMax(HImageReduced1, pHArithmeticImage);
					break;
				case FILTER_TYPE_EMPHASIZE:
					Emphasize(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y, 1.0);
					break;
				case FILTER_TYPE_GRAY_OPENING:
					GrayOpeningShape(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y, "octagon");
					break;
				case FILTER_TYPE_GRAY_CLOSING:
					GrayClosingShape(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y, "octagon");
					break;
				case FILTER_TYPE_EXPONENT:
					PowImage(HImageReduced1, pHArithmeticImage, Param.m_iImageProcessFilterType1X);
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_LOG:
					LogImage(HImageReduced1, pHArithmeticImage, "e");
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_ANISO_DIFFUSION:
					AnisotropicDiffusion(HImageReduced1, pHArithmeticImage, "perona-malik", (int)Param.m_iImageProcessFilterType1X, 1, (int)Param.m_iImageProcessFilterType1Y);
					break;
				case FILTER_TYPE_SHARPENING:
					ShockFilter(HImageReduced1, pHArithmeticImage, 0.5, 10, "canny", 1.5);
					break;
				default:
					break;
				}

				if (Param.m_iImageProcessFilterType1 == FILTER_TYPE_NOT_USED)
					*pHArithmeticImage = HImageReduced1;

				if ((Param.m_iImageProcessFilterType2 == FILTER_TYPE_NOT_USED || Param.m_iImageProcessFilterType2 == FILTER_TYPE_MEDIAN || Param.m_iImageProcessFilterType2 == FILTER_TYPE_SHARPENING) == FALSE)
				{
					if (Param.m_iImageProcessFilterType2X > Param.m_iImageProcessFilterType2Y)
						iExpandSize = Param.m_iImageProcessFilterType2X / 2 + 1;
					else
						iExpandSize = Param.m_iImageProcessFilterType2Y / 2 + 1;

					ExpandDomainGray(*pHArithmeticImage, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HInspectAreaRgn, pHArithmeticImage);
				}

				switch (Param.m_iImageProcessFilterType2)
				{
				case FILTER_TYPE_NOT_USED:
					break;
				case FILTER_TYPE_MEAN:
					MeanImage(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y);
					break;
				case FILTER_TYPE_MEDIAN:
					MedianRect(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y);
					break;
				case FILTER_TYPE_GAUSS:
					BinomialFilter(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y);
					break;
				case FILTER_TYPE_SCALE_MAX:
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_EMPHASIZE:
					Emphasize(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y, 1.0);
					break;
				case FILTER_TYPE_GRAY_OPENING:
					GrayOpeningShape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y, "octagon");
					break;
				case FILTER_TYPE_GRAY_CLOSING:
					GrayClosingShape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y, "octagon");
					break;
				case FILTER_TYPE_EXPONENT:
					PowImage(*pHArithmeticImage, pHArithmeticImage, Param.m_iImageProcessFilterType2X);
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_LOG:
					LogImage(*pHArithmeticImage, pHArithmeticImage, "e");
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_ANISO_DIFFUSION:
					AnisotropicDiffusion(*pHArithmeticImage, pHArithmeticImage, "perona-malik", (int)Param.m_iImageProcessFilterType2X, 1, (int)Param.m_iImageProcessFilterType2Y);
					break;
				case FILTER_TYPE_SHARPENING:
					ShockFilter(*pHArithmeticImage, pHArithmeticImage, 0.5, 10, "canny", 1.5);
					break;
				default:
					break;
				}

				if (Param.m_iImageProcessFilterType1 == FILTER_TYPE_NOT_USED && Param.m_iImageProcessFilterType2 == FILTER_TYPE_NOT_USED)
					*pHArithmeticImage = HImageReduced1;

				if ((Param.m_iImageProcessFilterType3 == FILTER_TYPE_NOT_USED || Param.m_iImageProcessFilterType3 == FILTER_TYPE_MEDIAN || Param.m_iImageProcessFilterType3 == FILTER_TYPE_SHARPENING) == FALSE)
				{
					if (Param.m_iImageProcessFilterType3X > Param.m_iImageProcessFilterType3Y)
						iExpandSize = Param.m_iImageProcessFilterType3X / 2 + 1;
					else
						iExpandSize = Param.m_iImageProcessFilterType3Y / 2 + 1;

					ExpandDomainGray(*pHArithmeticImage, &HExpandReduceImage, iExpandSize);
					ReduceDomain(HExpandReduceImage, HInspectAreaRgn, pHArithmeticImage);
				}

				switch (Param.m_iImageProcessFilterType3)
				{
				case FILTER_TYPE_NOT_USED:
					break;
				case FILTER_TYPE_MEAN:
					MeanImage(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y);
					break;
				case FILTER_TYPE_MEDIAN:
					MedianRect(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y);
					break;
				case FILTER_TYPE_GAUSS:
					BinomialFilter(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y);
					break;
				case FILTER_TYPE_SCALE_MAX:
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_EMPHASIZE:
					Emphasize(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y, 1.0);
					break;
				case FILTER_TYPE_GRAY_OPENING:
					GrayOpeningShape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y, "octagon");
					break;
				case FILTER_TYPE_GRAY_CLOSING:
					GrayClosingShape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y, "octagon");
					break;
				case FILTER_TYPE_EXPONENT:
					PowImage(*pHArithmeticImage, pHArithmeticImage, Param.m_iImageProcessFilterType3X);
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_LOG:
					LogImage(*pHArithmeticImage, pHArithmeticImage, "e");
					ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
					break;
				case FILTER_TYPE_ANISO_DIFFUSION:
					AnisotropicDiffusion(*pHArithmeticImage, pHArithmeticImage, "perona-malik", (int)Param.m_iImageProcessFilterType3X, 1, (int)Param.m_iImageProcessFilterType3Y);
					break;
				case FILTER_TYPE_SHARPENING:
					ShockFilter(*pHArithmeticImage, pHArithmeticImage, 0.5, 10, "canny", 1.5);
					break;
				default:
					break;
				}
			}
		}

		if (bDebugSave)
			WriteImage(*pHArithmeticImage, "bmp", 0, "c:\\DualTest\\ProcessingAfterImage");

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::GetPreprocessImage] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::GetMatchingPreprocessImage(HObject HOrgImage, int iImageProcessFilterType, double dImageProcessFilterTypeX, double dImageProcessFilterTypeY, HObject *pHArithmeticImage)
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (bDebugSave)
			WriteImage(HOrgImage, "bmp", 0, "c:\\DualTest\\ProcessingOrgImage");

		switch (iImageProcessFilterType)
		{
		case FILTER_TYPE_NOT_USED:
			CopyImage(HOrgImage, pHArithmeticImage);
			break;
		case FILTER_TYPE_MEAN:
			MeanImage(HOrgImage, pHArithmeticImage, (int)dImageProcessFilterTypeX, (int)dImageProcessFilterTypeY);
			break;
		case FILTER_TYPE_MEDIAN:
			MedianRect(HOrgImage, pHArithmeticImage, (int)dImageProcessFilterTypeX, (int)dImageProcessFilterTypeY);
			break;
		case FILTER_TYPE_GAUSS:
			BinomialFilter(HOrgImage, pHArithmeticImage, (int)dImageProcessFilterTypeX, (int)dImageProcessFilterTypeY);
			break;
		case FILTER_TYPE_SCALE_MAX:
			ScaleImageMax(HOrgImage, pHArithmeticImage);
			break;
		case FILTER_TYPE_EMPHASIZE:
			Emphasize(HOrgImage, pHArithmeticImage, (int)dImageProcessFilterTypeX, (int)dImageProcessFilterTypeY, 1.0);
			break;
		case FILTER_TYPE_GRAY_OPENING:
			EquHistoImage(HOrgImage, pHArithmeticImage);
			break;
		case FILTER_TYPE_GRAY_CLOSING:
			Illuminate(HOrgImage, pHArithmeticImage, (int)dImageProcessFilterTypeX, (int)dImageProcessFilterTypeY, 0.7);
			break;
		case FILTER_TYPE_EXPONENT:
			PowImage(HOrgImage, pHArithmeticImage, dImageProcessFilterTypeX);
			ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
			break;
		case FILTER_TYPE_LOG:
			LogImage(HOrgImage, pHArithmeticImage, "e");
			ScaleImageMax(*pHArithmeticImage, pHArithmeticImage);
			break;
		case FILTER_TYPE_ANISO_DIFFUSION:
			AnisotropicDiffusion(HOrgImage, pHArithmeticImage, "perona-malik", (int)dImageProcessFilterTypeX, 1, (int)dImageProcessFilterTypeY);
			break;
		default:
			CopyImage(HOrgImage, pHArithmeticImage);
			break;
		}

		if (bDebugSave)
			WriteImage(*pHArithmeticImage, "bmp", 0, "c:\\DualTest\\ProcessingAfterImage");

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::GetMatchingPreprocessImage] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///
///   L10 추가 (2017/01/27)
///
//////////////////////////////////////////////////////////////////////////

BOOL Algorithm::GetInspectArea(HObject HROIRgn, CAlgorithmParam Param, HObject *pHInspectArea)
{
	try
	{
		HObject HMarginRgn1, HMarginRgn2;
		Hlong lArea1, lArea2;
		double dCenterX, dCenterY;
		HTuple HlArea1, HlArea2, HdCenterX, HdCenterY;
		HObject HInspectRgn;
		int iMargin;

		GenEmptyObj(pHInspectArea);

		HObject HSelectedRgn;
		HTuple HlNoObj;
		Hlong lNoObj = 0;
		CountObj(HROIRgn, &HlNoObj);
		lNoObj = HlNoObj.L();

		for (int j = 0; j < lNoObj; j++)
		{
			SelectObj(HROIRgn, &HSelectedRgn, j + 1);

			for (int i = 0; i < 3; i++)
			{
				if (Param.m_bUseInspectArea[i])
				{
					if (Param.m_iInspectAreaContour1Margin[i] >= 0)
					{
						DilationCircle(HSelectedRgn, &HMarginRgn1, (double)Param.m_iInspectAreaContour1Margin[i] + 0.5);
					}
					else
					{
						iMargin = abs(Param.m_iInspectAreaContour1Margin[i]);
						ErosionCircle(HSelectedRgn, &HMarginRgn1, (double)iMargin + 0.5);
					}

					if (Param.m_iInspectAreaContour2Margin[i] >= 0)
					{
						DilationCircle(HSelectedRgn, &HMarginRgn2, (double)Param.m_iInspectAreaContour2Margin[i] + 0.5);
					}
					else
					{
						iMargin = abs(Param.m_iInspectAreaContour2Margin[i]);
						ErosionCircle(HSelectedRgn, &HMarginRgn2, (double)iMargin + 0.5);
					}

					AreaCenter(HMarginRgn1, &HlArea1, &HdCenterY, &HdCenterX);
					AreaCenter(HMarginRgn2, &HlArea2, &HdCenterY, &HdCenterX);
					lArea1 = HlArea1[0].L();
					lArea2 = HlArea2[0].L();

					if (lArea1 >= lArea2)
						Difference(HMarginRgn1, HMarginRgn2, &HInspectRgn);
					else
						Difference(HMarginRgn2, HMarginRgn1, &HInspectRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInspectRgn) == TRUE)
						ConcatObj(*pHInspectArea, HInspectRgn, pHInspectArea);
				}
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(*pHInspectArea) == TRUE)
			Union1(*pHInspectArea, pHInspectArea);

		return TRUE;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [Algorithm::GetInspectArea] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

HObject Algorithm::LineFittingAlgorithm(HObject HImage, HObject *pHFitImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, double *pdBurMaxLengthPxl, double *pdChippingMaxLengthPxl, int *piBurArea, int *piChippingArea, int *piFitLineXPos, int *piFitLineYPos)
{
	try
	{
		BOOL bDebugSave = FALSE;

		*pdBurMaxLengthPxl = 0;
		*pdChippingMaxLengthPxl = 0;
		*piBurArea = 0;
		*piChippingArea = 0;

		BOOL bNoUseFit = Param.m_bNoUseLineFit;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		GenEmptyObj(pHLineFitXLD);

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(HImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

		pImageData = (BYTE*)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");

			if (pHFitImage == NULL)	// Other image is not used...
				WriteImage(HImage, "bmp", 0, "c:\\DualTest\\FitImage");
			else
				WriteImage(*pHFitImage, "bmp", 0, "c:\\DualTest\\FitImage");
		}

		HObject HProcessImage;

		HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
		long lPeak, lHystUpperThreshold, lHystLowThreshold;

		if (Param.m_bUseImageScaling)
		{
			int iScaleMin, iScaleMax;
			int iScalePeakUpperOffset = 20;
			int iScalePeakLowerOffset = 10;

			if (Param.m_iImageScaleMethod == IMAGE_SCALE_AUTO)
			{
				GrayHisto(HInspectAreaRgn, HImage, &HAbsHisto, &HRelHisto);
				CreateFunct1dArray(HAbsHisto, &HFunction);
				SmoothFunct1dGauss(HFunction, 10.0, &HSmoothFunction);
				TupleLastN(HSmoothFunction, 3, &HAbsHisto);
				TupleSortIndex(HAbsHisto, &Indices);
				TupleInverse(Indices, &Inverted);
				lPeak = Inverted[0].L();

				iScaleMax = lPeak + iScalePeakUpperOffset;
				if (iScaleMax > 255)
					iScaleMax = 255;

				iScaleMin = lPeak - iScalePeakLowerOffset;
				if (iScaleMin < 0)
					iScaleMin = 0;
			}
			else
			{
				if (Param.m_iImageScaleMin < 0)
					iScaleMin = 0;
				else if (Param.m_iImageScaleMin > 255)
					iScaleMin = 255;
				else
					iScaleMin = Param.m_iImageScaleMin;

				if (Param.m_iImageScaleMax < 0)
					iScaleMax = 0;
				else if (Param.m_iImageScaleMax > 255)
					iScaleMax = 255;
				else
					iScaleMax = Param.m_iImageScaleMax;

				if (iScaleMax < iScaleMin)
					iScaleMax = iScaleMin;
			}

			THEAPP.m_pGFunction->ScaleImageRange(HImage, &HProcessImage, HTuple(iScaleMin), HTuple(iScaleMax));
			BinomialFilter(HProcessImage, &HProcessImage, 3, 3);
			MedianImage(HProcessImage, &HProcessImage, "circle", 2, "mirrored");
		}

		int iFItEdgeMethod = Param.m_iLineFitEdgeMethod;
		double dFitEdgeSigma = Param.m_dLineFitFittingEdgeSF;
		int iFitEdgeStrengthHigh = Param.m_iLineFitFittingEdgeHigh;
		int iFitEdgeStrengthLow = iFitEdgeStrengthHigh - 10;
		if (iFitEdgeStrengthLow <= 5)
			iFitEdgeStrengthLow = iFitEdgeStrengthHigh;
		int iFitMinContourLength = Param.m_iLineFitFittingMinContourLength;
		int iFitUnionContourLength = Param.m_iLineFitFittingContourConnectionLength;

		int iObjectMinThres = Param.m_iLineFitObjectLowerThres;
		int iObjectMaxThres = Param.m_iLineFitObjectUpperThres;
		double dInspectEdgeSigma = Param.m_dLineFitObjectEdgeSF;
		int iEdgeStrengthHigh = Param.m_iLineFitEdgeHigh;
		int iEdgeStrengthLow = iEdgeStrengthHigh - 10;
		if (iEdgeStrengthLow <= 5)
			iEdgeStrengthLow = iEdgeStrengthHigh;
		int iMinContourLength = Param.m_iLineFitMinContourLength;
		int iUnionContourLength = Param.m_iLineFitContourConnectionLength;

		BOOL bBurDetection = Param.m_bLineFitUseBurInspection;
		double dBurDefectProjectionLength = Param.m_dLineFitBurDefectLength;
		double dBurDefectProjectionLengthMax = Param.m_dLineFitBurDefectLengthMax;
		BOOL bChippingDetection = Param.m_bLineFitUseChippingInspection;
		double dChippingDefectProjectionLength = Param.m_dLineFitChippingDefectLength;
		double dChippingDefectProjectionLengthMax = Param.m_dLineFitChippingDefectLengthMax;

		BOOL bRotationAngleDetection = Param.m_bLineFitUseRotationInspection;
		int iRotationReference = Param.m_iLineFitRotationReference;		// 0: Horizontal Line, 1: Vertical Line, 시계방향: -Angle 
		double dRotationDefectAngle = Param.m_dLineFitRotationDefectAngle;

		double dRotationAngleDegree;

		HObject  Image, ROI, ImageReduced, HObjectRgn;
		HObject  Edges, DerivGauss, SelectedEdges, UnionContours;
		HObject  MaxLengthXLD, RegressContour, ROIErodedRgn;
		HObject  Contour, TempRgn, RegressRgn, BoundaryRgn;
		HObject  DiffRgn, DiffConnRgn, DiffRegressRgn, DiffRegressConnRgn;
		HObject  RegressRgn_1, RegressRgn_2, IntersectRgn_1, IntersectRgn_2;
		HObject  NeckRegressRgn, BurRegressRgn, BoundaryRgn_1, BoundaryRgn_2;
		HObject  BurBoundaryRgn, NeckBoundaryRgn, BurDefectRgn, SelectedBurDefectRgn;
		HObject  NeckDefectRgn, SelectedNeckDefectRgn, FitImage;
		HObject  FitImageReduced;

		//		HTuple  XLDLength, Indices, Inverted, RowBegin, ColBegin;
		HTuple  XLDLength, RowBegin, ColBegin;
		HTuple  RowEnd, ColEnd, Nr, Nc, Dist, RowEdge, ColEdge;
		HTuple  NoBoundaryPoints, i, RowProj, ColProj, Distance, DefectProjectionLength;
		HTuple  Area_1, Row, Column, Area_2;

		// Line Fitting

		if (pHFitImage == NULL)	// Other image is not used...
		{
			if (Param.m_bUseImageScaling)
				ReduceDomain(HProcessImage, HInspectAreaRgn, &FitImageReduced);
			else
				ReduceDomain(HImage, HInspectAreaRgn, &FitImageReduced);
		}
		else
			ReduceDomain(*pHFitImage, HInspectAreaRgn, &FitImageReduced);

		if (iFItEdgeMethod == 0)
		{
			DerivateGauss(FitImageReduced, &DerivGauss, dFitEdgeSigma, "laplace");
			ZeroCrossingSubPix(DerivGauss, &Edges);
		}
		else
		{
			EdgesSubPix(FitImageReduced, &Edges, "canny", dFitEdgeSigma, iFitEdgeStrengthLow, iFitEdgeStrengthHigh);
		}
		SelectContoursXld(Edges, &SelectedEdges, "contour_length", iFitMinContourLength, 99999999, -0.5, 0.5);
		SelectContoursXld(SelectedEdges, &SelectedEdges, "open", iFitMinContourLength / 2, 200, -0.5, 0.5);
		UnionAdjacentContoursXld(SelectedEdges, &UnionContours, iFitUnionContourLength, 1, "attr_keep");

		if (THEAPP.m_pGFunction->ValidHXLD(UnionContours) == FALSE)
			return HDefectRgn;

		LengthXld(UnionContours, &XLDLength);
		TupleSortIndex(XLDLength, &Indices);
		TupleInverse(Indices, &Inverted);
		SelectObj(UnionContours, &MaxLengthXLD, HTuple(Inverted[0]) + 1);

		FitLineContourXld(MaxLengthXLD, "tukey", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

		Hlong lNoPoint;
		HTuple HlNoPoint;
		TupleLength(RowBegin, &HlNoPoint);
		lNoPoint = HlNoPoint.L();
		if (lNoPoint <= 0)
		{
			return HDefectRgn;
		}

		// Extension of Fit Line Points to ROI Boundary

		Hlong lRow1, lRow2, lCol1, lCol2, lROIWidth, lROIHeight;
		HTuple HlRow1, HlRow2, HlCol1, HlCol2;

		SmallestRectangle1(HInspectAreaRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

		lRow1 = HlRow1.L();
		lCol1 = HlCol1.L();
		lRow2 = HlRow2.L();
		lCol2 = HlCol2.L();

		lROIWidth = lCol2 - lCol1;
		lROIHeight = lRow2 - lRow1;

		BOOL bLongWidthROI = FALSE;

		if (lROIHeight >= lROIWidth)
			bLongWidthROI = FALSE;
		else
			bLongWidthROI = TRUE;

		double dLineStartX, dLineStartY, dLineEndX, dLineEndY;
		double dFitLineStartX, dFitLineStartY, dFitLineEndX, dFitLineEndY;
		double dIntersectX, dIntersectY;
		Hlong IsParallel;
		HTuple HdIntersectX, HdIntersectY, HIsParallel;

		dFitLineStartX = ColBegin[0].D();
		dFitLineStartY = RowBegin[0].D();
		dFitLineEndX = ColEnd[0].D();
		dFitLineEndY = RowEnd[0].D();

		if (Param.m_bLineFitLocalAlignUse)
		{
			if (bLongWidthROI)
				*piFitLineYPos = (int)((dFitLineStartY + dFitLineEndY)*0.5 + 0.5);
			else
				*piFitLineXPos = (int)((dFitLineStartX + dFitLineEndX)*0.5 + 0.5);
		}

		if (bLongWidthROI)
		{
			dLineStartX = lCol1;
			dLineStartY = lRow1;
			dLineEndX = lCol1;
			dLineEndY = lRow2;

			IntersectionLl(dLineStartY, dLineStartX, dLineEndY, dLineEndX, dFitLineStartY, dFitLineStartX, dFitLineEndY, dFitLineEndX, &HdIntersectY, &HdIntersectX, &HIsParallel);
			dIntersectX = HdIntersectX.D();
			dIntersectY = HdIntersectY.D();
			IsParallel = HIsParallel.L();

			if (IsParallel > 0)
				return HDefectRgn;

			RowBegin = dIntersectY;
			ColBegin = dIntersectX;

			dLineStartX = lCol2;
			dLineStartY = lRow1;
			dLineEndX = lCol2;
			dLineEndY = lRow2;

			IntersectionLl(dLineStartY, dLineStartX, dLineEndY, dLineEndX, dFitLineStartY, dFitLineStartX, dFitLineEndY, dFitLineEndX, &HdIntersectY, &HdIntersectX, &HIsParallel);
			dIntersectX = HdIntersectX.D();
			dIntersectY = HdIntersectY.D();
			IsParallel = HIsParallel.L();
			if (IsParallel > 0)
				return HDefectRgn;

			RowEnd = dIntersectY;
			ColEnd = dIntersectX;
		}
		else
		{
			dLineStartX = lCol1;
			dLineStartY = lRow1;
			dLineEndX = lCol2;
			dLineEndY = lRow1;

			IntersectionLl(dLineStartY, dLineStartX, dLineEndY, dLineEndX, dFitLineStartY, dFitLineStartX, dFitLineEndY, dFitLineEndX, &HdIntersectY, &HdIntersectX, &HIsParallel);
			dIntersectX = HdIntersectX.D();
			dIntersectY = HdIntersectY.D();
			IsParallel = HIsParallel.L();
			if (IsParallel > 0)
				return HDefectRgn;

			RowBegin = dIntersectY;
			ColBegin = dIntersectX;

			dLineStartX = lCol1;
			dLineStartY = lRow2;
			dLineEndX = lCol2;
			dLineEndY = lRow2;

			IntersectionLl(dLineStartY, dLineStartX, dLineEndY, dLineEndX, dFitLineStartY, dFitLineStartX, dFitLineEndY, dFitLineEndX, &HdIntersectY, &HdIntersectX, &HIsParallel);
			dIntersectX = HdIntersectX.D();
			dIntersectY = HdIntersectY.D();
			IsParallel = HIsParallel.L();
			if (IsParallel > 0)
				return HDefectRgn;

			RowEnd = dIntersectY;
			ColEnd = dIntersectX;
		}

		if (bNoUseFit == FALSE)
		{
			GenContourPolygonXld(&RegressContour, HTuple(RowBegin[0]).TupleConcat(HTuple(RowEnd[0])), HTuple(ColBegin[0]).TupleConcat(HTuple(ColEnd[0])));

			ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
		}

		if (Param.m_bLineFitLocalAlignUse)
		{
			return HDefectRgn;
		}

		HTuple HAngle;

		if (bRotationAngleDetection)
		{
			if (iRotationReference == 0)
				AngleLl(0, 0, 0, 500, RowBegin[0], ColBegin[0], RowEnd[0], ColEnd[0], &HAngle);
			else if (iRotationReference == 1)
				AngleLl(0, 0, 500, 0, RowBegin[0], ColBegin[0], RowEnd[0], ColEnd[0], &HAngle);

			TupleDeg(HAngle, &HAngle);
			dRotationAngleDegree = HAngle[0].D();

			if (fabs(dRotationAngleDegree) >= 90)
			{
				if (dRotationAngleDegree >= 0)
					dRotationAngleDegree = 180.0 - dRotationAngleDegree;
				else
					dRotationAngleDegree = 180.0 + dRotationAngleDegree;
			}

			m_dFitLineRotationAngle = dRotationAngleDegree;
		}

		if (Param.m_bUseImageScaling)
			ReduceDomain(HProcessImage, HInspectAreaRgn, &ImageReduced);
		else
			ReduceDomain(HImage, HInspectAreaRgn, &ImageReduced);

		// 대상 Blob 얻기

		Threshold(ImageReduced, &HObjectRgn, iObjectMinThres, iObjectMaxThres);
		if (THEAPP.m_pGFunction->ValidHRegion(HObjectRgn) == FALSE)
			return HDefectRgn;

		FillUp(HObjectRgn, &HObjectRgn);
		Connection(HObjectRgn, &HObjectRgn);
		SelectShapeStd(HObjectRgn, &HObjectRgn, "max_area", 70);

		// Boundary Edge Detection (Boundary Defect)

		int iLineFitObjectPos = LINE_FIT_XDIR_OBJECT_TOP;

		if (bBurDetection || bChippingDetection)
		{
			if (iFItEdgeMethod == 0)
			{
				DerivateGauss(ImageReduced, &DerivGauss, dInspectEdgeSigma, "laplace");
				ZeroCrossingSubPix(DerivGauss, &Edges);
			}
			else
			{
				EdgesSubPix(ImageReduced, &Edges, "canny", dInspectEdgeSigma, iEdgeStrengthLow, iEdgeStrengthHigh);
			}

			SelectContoursXld(Edges, &SelectedEdges, "contour_length", iMinContourLength, 99999999, -0.5, 0.5);
			SelectContoursXld(SelectedEdges, &SelectedEdges, "open", iMinContourLength / 2, 200, -0.5, 0.5);
			UnionAdjacentContoursXld(SelectedEdges, &UnionContours, iUnionContourLength, 1, "attr_keep");

			if (THEAPP.m_pGFunction->ValidHXLD(UnionContours) == FALSE)
				return HDefectRgn;

			LengthXld(UnionContours, &XLDLength);
			TupleSortIndex(XLDLength, &Indices);
			TupleInverse(Indices, &Inverted);
			SelectObj(UnionContours, &MaxLengthXLD, HTuple(Inverted[0]) + 1);

			if (bNoUseFit == TRUE)
			{
				Hlong lContourPoint;
				HTuple HTempRow, HTempCol;

				GetContourXld(MaxLengthXLD, &HTempRow, &HTempCol);

				HTuple HlContourPoint;
				TupleLength(HTempRow, &HlContourPoint);
				lContourPoint = HlContourPoint.L();

				RowBegin[0] = HTempRow[0];
				RowEnd[0] = HTempRow[lContourPoint - 1];
				ColBegin[0] = HTempCol[0];
				ColEnd[0] = HTempCol[lContourPoint - 1];

				GenContourPolygonXld(&RegressContour, HTuple(RowBegin[0]).TupleConcat(HTuple(RowEnd[0])), HTuple(ColBegin[0]).TupleConcat(HTuple(ColEnd[0])));

				ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
			}

			ConcatObj(*pHLineFitXLD, MaxLengthXLD, pHLineFitXLD);

			// Check the position relation between object and fitting line

			dFitLineStartX = ColBegin[0].D();
			dFitLineStartY = RowBegin[0].D();
			dFitLineEndX = ColEnd[0].D();
			dFitLineEndY = RowEnd[0].D();

			double dFitLineWidth, dFitLineHeight, dFitLineCenter;
			dFitLineWidth = fabs(dFitLineEndX - dFitLineStartX);
			dFitLineHeight = fabs(dFitLineEndY - dFitLineStartY);

			Hlong lObjectArea;
			double dObjectCenterX, dObjectCenterY;
			HTuple HlObjectArea, HdObjectCenterY, HdObjectCenterX;
			AreaCenter(HObjectRgn, &HlObjectArea, &HdObjectCenterY, &HdObjectCenterX);
			lObjectArea = HlObjectArea.L();
			dObjectCenterY = HdObjectCenterY.D();
			dObjectCenterX = HdObjectCenterX.D();

			if (dFitLineWidth >= dFitLineHeight)	// Fitting Line => X방향, Object: 상/하
			{
				dFitLineCenter = (dFitLineStartY + dFitLineEndY)*0.5;

				if (dObjectCenterY <= dFitLineCenter)
					iLineFitObjectPos = LINE_FIT_XDIR_OBJECT_TOP;
				else
					iLineFitObjectPos = LINE_FIT_XDIR_OBJECT_BOTTOM;
			}
			else									// Fitting Line => Y방향, Object: 좌/우
			{
				dFitLineCenter = (dFitLineStartX + dFitLineEndX)*0.5;

				if (dObjectCenterX <= dFitLineCenter)
					iLineFitObjectPos = LINE_FIT_YDIR_OBJECT_LEFT;
				else
					iLineFitObjectPos = LINE_FIT_YDIR_OBJECT_RIGHT;
			}

			// Distance Measure: Boundary to Fitting Line

			ErosionCircle(HInspectAreaRgn, &ROIErodedRgn, 1.5);

			GetContourXld(MaxLengthXLD, &RowEdge, &ColEdge);
			TupleLength(RowEdge, &NoBoundaryPoints);

			// Bur
			HObject BurDefectPosRgn, HBurAreaCheckRgn;
			GenEmptyObj(&BurDefectPosRgn);
			GenEmptyObj(&HBurAreaCheckRgn);

			double dDistancePxl, dLinePointPos, dContourPointPos;
			BOOL bMaxLengthFlag = FALSE;
			HObject HMaxLengthRgn;

			GenEmptyObj(&HMaxLengthRgn);

			if (bBurDetection)
			{
				DefectProjectionLength = HTuple(dBurDefectProjectionLength);

				for (i = 0; i <= NoBoundaryPoints - 1; i += 1)
				{
					bMaxLengthFlag = FALSE;

					ProjectionPl(HTuple(RowEdge[i]), HTuple(ColEdge[i]), HTuple(RowBegin[0]), HTuple(ColBegin[0]), HTuple(RowEnd[0]), HTuple(ColEnd[0]), &RowProj, &ColProj);
					DistancePp(HTuple(RowEdge[i]), HTuple(ColEdge[i]), RowProj, ColProj, &Distance);

					if (iLineFitObjectPos == LINE_FIT_XDIR_OBJECT_TOP)
					{
						dLinePointPos = RowProj[0].D();
						dContourPointPos = RowEdge[i].D();

						if (dLinePointPos < dContourPointPos)
						{
							dDistancePxl = Distance[0].D();
							if (dDistancePxl > *pdBurMaxLengthPxl)
							{
								*pdBurMaxLengthPxl = dDistancePxl;
								bMaxLengthFlag = TRUE;
							}

							GenContourPolygonXld(&Contour, HTuple(RowEdge[i]).TupleConcat(RowProj), HTuple(ColEdge[i]).TupleConcat(ColProj));
							GenRegionContourXld(Contour, &TempRgn, "filled");

							if (THEAPP.m_pGFunction->ValidHRegion(TempRgn))
							{
								if (bMaxLengthFlag)
									HMaxLengthRgn = TempRgn;

								if (Distance > DefectProjectionLength)
									ConcatObj(BurDefectPosRgn, TempRgn, &BurDefectPosRgn);

								ConcatObj(HBurAreaCheckRgn, TempRgn, &HBurAreaCheckRgn);
							}
						}
					}
					else if (iLineFitObjectPos == LINE_FIT_XDIR_OBJECT_BOTTOM)
					{
						dLinePointPos = RowProj[0].D();
						dContourPointPos = RowEdge[i].D();

						if (dLinePointPos > dContourPointPos)
						{
							dDistancePxl = Distance[0].D();
							if (dDistancePxl > *pdBurMaxLengthPxl)
							{
								*pdBurMaxLengthPxl = dDistancePxl;
								bMaxLengthFlag = TRUE;
							}

							GenContourPolygonXld(&Contour, HTuple(RowEdge[i]).TupleConcat(RowProj), HTuple(ColEdge[i]).TupleConcat(ColProj));
							GenRegionContourXld(Contour, &TempRgn, "filled");

							if (THEAPP.m_pGFunction->ValidHRegion(TempRgn))
							{
								if (bMaxLengthFlag)
									HMaxLengthRgn = TempRgn;

								if (Distance > DefectProjectionLength)
									ConcatObj(BurDefectPosRgn, TempRgn, &BurDefectPosRgn);

								ConcatObj(HBurAreaCheckRgn, TempRgn, &HBurAreaCheckRgn);
							}
						}
					}
					else if (iLineFitObjectPos == LINE_FIT_YDIR_OBJECT_LEFT)
					{
						dLinePointPos = ColProj[0].D();
						dContourPointPos = ColEdge[i].D();

						if (dLinePointPos < dContourPointPos)
						{
							dDistancePxl = Distance[0].D();
							if (dDistancePxl > *pdBurMaxLengthPxl)
							{
								*pdBurMaxLengthPxl = dDistancePxl;
								bMaxLengthFlag = TRUE;
							}

							GenContourPolygonXld(&Contour, HTuple(RowEdge[i]).TupleConcat(RowProj), HTuple(ColEdge[i]).TupleConcat(ColProj));
							GenRegionContourXld(Contour, &TempRgn, "filled");

							if (THEAPP.m_pGFunction->ValidHRegion(TempRgn))
							{
								if (bMaxLengthFlag)
									HMaxLengthRgn = TempRgn;

								if (Distance > DefectProjectionLength)
									ConcatObj(BurDefectPosRgn, TempRgn, &BurDefectPosRgn);

								ConcatObj(HBurAreaCheckRgn, TempRgn, &HBurAreaCheckRgn);
							}
						}
					}
					else if (iLineFitObjectPos == LINE_FIT_YDIR_OBJECT_RIGHT)
					{
						dLinePointPos = ColProj[0].D();
						dContourPointPos = ColEdge[i].D();

						if (dLinePointPos > dContourPointPos)
						{
							dDistancePxl = Distance[0].D();
							if (dDistancePxl > *pdBurMaxLengthPxl)
							{
								*pdBurMaxLengthPxl = dDistancePxl;
								bMaxLengthFlag = TRUE;
							}

							GenContourPolygonXld(&Contour, HTuple(RowEdge[i]).TupleConcat(RowProj), HTuple(ColEdge[i]).TupleConcat(ColProj));
							GenRegionContourXld(Contour, &TempRgn, "filled");

							if (THEAPP.m_pGFunction->ValidHRegion(TempRgn))
							{
								if (bMaxLengthFlag)
									HMaxLengthRgn = TempRgn;

								if (Distance > DefectProjectionLength)
									ConcatObj(BurDefectPosRgn, TempRgn, &BurDefectPosRgn);

								ConcatObj(HBurAreaCheckRgn, TempRgn, &HBurAreaCheckRgn);
							}
						}
					}
				}

				if (*pdBurMaxLengthPxl > dBurDefectProjectionLengthMax)
					GenEmptyObj(&BurDefectPosRgn);
				else
					Union1(BurDefectPosRgn, &BurDefectPosRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HBurAreaCheckRgn))
					Union1(HBurAreaCheckRgn, &HBurAreaCheckRgn);
			}

			// Chipping
			HObject ChippingDefectPosRgn, HChippingAreaCheckRgn;
			GenEmptyObj(&ChippingDefectPosRgn);
			GenEmptyObj(&HChippingAreaCheckRgn);

			if (bChippingDetection)
			{
				DefectProjectionLength = HTuple(dChippingDefectProjectionLength);

				for (i = 0; i <= NoBoundaryPoints - 1; i += 1)
				{
					bMaxLengthFlag = FALSE;

					ProjectionPl(HTuple(RowEdge[i]), HTuple(ColEdge[i]), HTuple(RowBegin[0]), HTuple(ColBegin[0]), HTuple(RowEnd[0]), HTuple(ColEnd[0]), &RowProj, &ColProj);
					DistancePp(HTuple(RowEdge[i]), HTuple(ColEdge[i]), RowProj, ColProj, &Distance);

					if (iLineFitObjectPos == LINE_FIT_XDIR_OBJECT_TOP)
					{
						dLinePointPos = RowProj[0].D();
						dContourPointPos = RowEdge[i].D();

						if (dLinePointPos > dContourPointPos)
						{
							dDistancePxl = Distance[0].D();
							if (dDistancePxl > *pdChippingMaxLengthPxl)
							{
								*pdChippingMaxLengthPxl = dDistancePxl;
								bMaxLengthFlag = TRUE;
							}

							GenContourPolygonXld(&Contour, HTuple(RowEdge[i]).TupleConcat(RowProj), HTuple(ColEdge[i]).TupleConcat(ColProj));
							GenRegionContourXld(Contour, &TempRgn, "filled");

							if (THEAPP.m_pGFunction->ValidHRegion(TempRgn))
							{
								if (bMaxLengthFlag)
									HMaxLengthRgn = TempRgn;

								if (Distance > DefectProjectionLength)
									ConcatObj(ChippingDefectPosRgn, TempRgn, &ChippingDefectPosRgn);

								ConcatObj(HChippingAreaCheckRgn, TempRgn, &HChippingAreaCheckRgn);
							}
						}
					}
					else if (iLineFitObjectPos == LINE_FIT_XDIR_OBJECT_BOTTOM)
					{
						dLinePointPos = RowProj[0].D();
						dContourPointPos = RowEdge[i].D();

						if (dLinePointPos < dContourPointPos)
						{
							dDistancePxl = Distance[0].D();
							if (dDistancePxl > *pdChippingMaxLengthPxl)
							{
								*pdChippingMaxLengthPxl = dDistancePxl;
								bMaxLengthFlag = TRUE;
							}

							GenContourPolygonXld(&Contour, HTuple(RowEdge[i]).TupleConcat(RowProj), HTuple(ColEdge[i]).TupleConcat(ColProj));
							GenRegionContourXld(Contour, &TempRgn, "filled");

							// TEST (Projection Line Display)
							//ConcatObj(*pHLineFitXLD, Contour, pHLineFitXLD);

							if (THEAPP.m_pGFunction->ValidHRegion(TempRgn))
							{
								if (bMaxLengthFlag)
									HMaxLengthRgn = TempRgn;

								if (Distance > DefectProjectionLength)
									ConcatObj(ChippingDefectPosRgn, TempRgn, &ChippingDefectPosRgn);

								ConcatObj(HChippingAreaCheckRgn, TempRgn, &HChippingAreaCheckRgn);
							}
						}
					}
					else if (iLineFitObjectPos == LINE_FIT_YDIR_OBJECT_LEFT)
					{
						dLinePointPos = ColProj[0].D();
						dContourPointPos = ColEdge[i].D();

						if (dLinePointPos > dContourPointPos)
						{
							dDistancePxl = Distance[0].D();
							if (dDistancePxl > *pdChippingMaxLengthPxl)
							{
								*pdChippingMaxLengthPxl = dDistancePxl;
								bMaxLengthFlag = TRUE;
							}

							GenContourPolygonXld(&Contour, HTuple(RowEdge[i]).TupleConcat(RowProj), HTuple(ColEdge[i]).TupleConcat(ColProj));
							GenRegionContourXld(Contour, &TempRgn, "filled");

							if (THEAPP.m_pGFunction->ValidHRegion(TempRgn))
							{
								if (bMaxLengthFlag)
									HMaxLengthRgn = TempRgn;

								if (Distance > DefectProjectionLength)
									ConcatObj(ChippingDefectPosRgn, TempRgn, &ChippingDefectPosRgn);

								ConcatObj(HChippingAreaCheckRgn, TempRgn, &HChippingAreaCheckRgn);
							}
						}
					}
					else if (iLineFitObjectPos == LINE_FIT_YDIR_OBJECT_RIGHT)
					{
						dLinePointPos = ColProj[0].D();
						dContourPointPos = ColEdge[i].D();

						if (dLinePointPos < dContourPointPos)
						{
							dDistancePxl = Distance[0].D();
							if (dDistancePxl > *pdChippingMaxLengthPxl)
							{
								*pdChippingMaxLengthPxl = dDistancePxl;
								bMaxLengthFlag = TRUE;
							}

							GenContourPolygonXld(&Contour, HTuple(RowEdge[i]).TupleConcat(RowProj), HTuple(ColEdge[i]).TupleConcat(ColProj));
							GenRegionContourXld(Contour, &TempRgn, "filled");

							if (THEAPP.m_pGFunction->ValidHRegion(TempRgn))
							{
								if (bMaxLengthFlag)
									HMaxLengthRgn = TempRgn;

								if (Distance > DefectProjectionLength)
									ConcatObj(ChippingDefectPosRgn, TempRgn, &ChippingDefectPosRgn);

								ConcatObj(HChippingAreaCheckRgn, TempRgn, &HChippingAreaCheckRgn);
							}
						}
					}
				}

				if (*pdChippingMaxLengthPxl > dChippingDefectProjectionLengthMax)
					GenEmptyObj(&ChippingDefectPosRgn);
				else
					Union1(ChippingDefectPosRgn, &ChippingDefectPosRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HChippingAreaCheckRgn))
					Union1(HChippingAreaCheckRgn, &HChippingAreaCheckRgn);
			}

			// Bur/Chipping Position Detection
/*
			GenRegionContourXld(RegressContour, &RegressRgn, "margin");	// Fit Line Rgn
			GenRegionPolygon(&BoundaryRgn, RowEdge, ColEdge);				// Contour Rgn
			Difference(ROIErodedRgn, BoundaryRgn, &DiffRgn);
			SetSystem("tsp_neighborhood", 4);
			Connection(DiffRgn, &DiffConnRgn);
			SetSystem("tsp_neighborhood", 8);
			Difference(ROIErodedRgn, RegressRgn, &DiffRegressRgn);
			SetSystem("tsp_neighborhood", 4);
			Connection(DiffRegressRgn, &DiffRegressConnRgn);
			SetSystem("tsp_neighborhood", 8);
			SelectObj(DiffRegressConnRgn, &RegressRgn_1, 1);
			SelectObj(DiffRegressConnRgn, &RegressRgn_2, 2);
			Intersection(RegressRgn_1, HObjectRgn, &IntersectRgn_1);
			Intersection(RegressRgn_2, HObjectRgn, &IntersectRgn_2);
			AreaCenter(IntersectRgn_1, &Area_1, &Row, &Column);
			AreaCenter(IntersectRgn_2, &Area_2, &Row, &Column);
			if (Area_1>Area_2)
			{
				SelectObj(RegressRgn_1, &NeckRegressRgn, 1);
				SelectObj(RegressRgn_2, &BurRegressRgn, 1);
			}
			else
			{
				SelectObj(RegressRgn_2, &NeckRegressRgn, 1);
				SelectObj(RegressRgn_1, &BurRegressRgn, 1);
			}
			SelectObj(DiffConnRgn, &BoundaryRgn_1, 1);
			SelectObj(DiffConnRgn, &BoundaryRgn_2, 2);
			Intersection(BoundaryRgn_1, HObjectRgn, &IntersectRgn_1);
			Intersection(BoundaryRgn_2, HObjectRgn, &IntersectRgn_2);
			AreaCenter(IntersectRgn_1, &Area_1, &Row, &Column);
			AreaCenter(IntersectRgn_2, &Area_2, &Row, &Column);
			if (Area_1>Area_2)
			{
				SelectObj(BoundaryRgn_1, &BurBoundaryRgn, 1);
				SelectObj(BoundaryRgn_2, &NeckBoundaryRgn, 1);
			}
			else
			{
				SelectObj(BoundaryRgn_2, &BurBoundaryRgn, 1);
				SelectObj(BoundaryRgn_1, &NeckBoundaryRgn, 1);
			}
*/
			Hlong lDefectArea;
			double dDefectCenterX, dDefectCenterY;
			HTuple HlDefectArea, HdDefectCenterX, HdDefectCenterY;

			if (bBurDetection)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(BurDefectPosRgn))
				{
					Connection(BurDefectPosRgn, &BurDefectPosRgn);
					ConcatObj(HDefectRgn, BurDefectPosRgn, &HDefectRgn);
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HBurAreaCheckRgn))
				{
					if (THEAPP.m_pGFunction->ValidHRegion(HMaxLengthRgn))
					{
						Connection(HBurAreaCheckRgn, &HBurAreaCheckRgn);
						SelectShapeProto(HBurAreaCheckRgn, HMaxLengthRgn, &HBurAreaCheckRgn, "overlaps_abs", 1, 9999999);

						if (THEAPP.m_pGFunction->ValidHRegion(HBurAreaCheckRgn))
						{
							AreaCenter(HBurAreaCheckRgn, &HlDefectArea, &HdDefectCenterY, &HdDefectCenterX);
							lDefectArea = HlDefectArea.L();
							dDefectCenterY = HdDefectCenterY.D();
							dDefectCenterX = HdDefectCenterX.D();
							*piBurArea = lDefectArea;
						}
					}
					else
					{
						Connection(HBurAreaCheckRgn, &HBurAreaCheckRgn);
						SelectShapeStd(HBurAreaCheckRgn, &HBurAreaCheckRgn, "max_area", 70);

						if (THEAPP.m_pGFunction->ValidHRegion(HBurAreaCheckRgn))
						{
							AreaCenter(HBurAreaCheckRgn, &HlDefectArea, &HdDefectCenterY, &HdDefectCenterX);
							lDefectArea = HlDefectArea.L();
							dDefectCenterY = HdDefectCenterY.D();
							dDefectCenterX = HdDefectCenterX.D();
							*piBurArea = lDefectArea;
						}
					}
				}
			}

			if (bChippingDetection)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(ChippingDefectPosRgn))
				{
					Connection(ChippingDefectPosRgn, &ChippingDefectPosRgn);
					ConcatObj(HDefectRgn, ChippingDefectPosRgn, &HDefectRgn);
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HChippingAreaCheckRgn))
				{
					if (THEAPP.m_pGFunction->ValidHRegion(HMaxLengthRgn))
					{
						Connection(HChippingAreaCheckRgn, &HChippingAreaCheckRgn);
						SelectShapeProto(HChippingAreaCheckRgn, HMaxLengthRgn, &HChippingAreaCheckRgn, "overlaps_abs", 1, 9999999);

						if (THEAPP.m_pGFunction->ValidHRegion(HChippingAreaCheckRgn))
						{
							AreaCenter(HChippingAreaCheckRgn, &HlDefectArea, &HdDefectCenterY, &HdDefectCenterX);
							lDefectArea = HlDefectArea.L();
							dDefectCenterY = HdDefectCenterY.D();
							dDefectCenterX = HdDefectCenterX.D();
							*piChippingArea = lDefectArea;
						}
					}
					else
					{
						Connection(HChippingAreaCheckRgn, &HChippingAreaCheckRgn);
						SelectShapeStd(HChippingAreaCheckRgn, &HChippingAreaCheckRgn, "max_area", 70);

						if (THEAPP.m_pGFunction->ValidHRegion(HChippingAreaCheckRgn))
						{
							AreaCenter(HChippingAreaCheckRgn, &HlDefectArea, &HdDefectCenterY, &HdDefectCenterX);
							lDefectArea = HlDefectArea.L();
							dDefectCenterY = HdDefectCenterY.D();
							dDefectCenterX = HdDefectCenterX.D();
							*piChippingArea = lDefectArea;
						}
					}
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				Union1(HDefectRgn, &HDefectRgn);
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::LineFittingAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::ConnectorPinDeformAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn.reg");
		}

		int iPinDetectMargin = Param.m_iPinPitch;		// Inspection Margin
		int iPinLengthDefectTolerance = Param.m_iPinDefectNumberTolerance;
		int iDTMeanFilterSize = Param.m_iPinDTFilterSize;
		int iDTValue = Param.m_iPinDTValue;

		HObject HInspectAreaConnRgn;
		Connection(HInspectAreaRgn, &HInspectAreaConnRgn);

		HTuple HNoTeachingPin;
		Hlong lNoTeachingPin;
		CountObj(HInspectAreaConnRgn, &HNoTeachingPin);
		lNoTeachingPin = HNoTeachingPin.L();

		HObject HPinDetectRgn;
		ShapeTrans(HInspectAreaRgn, &HPinDetectRgn, "rectangle1");

		if (Param.m_iPinPos == CONNECTOR_PIN_TOP || Param.m_iPinPos == CONNECTOR_PIN_BOTTOM)
			DilationRectangle1(HPinDetectRgn, &HPinDetectRgn, 5, iPinDetectMargin * 2 + 1);
		else
			DilationRectangle1(HPinDetectRgn, &HPinDetectRgn, iPinDetectMargin * 2 + 1, 5);

		HObject  HImageReduced;
		ReduceDomain(HImage, HPinDetectRgn, &HImageReduced);

		HObject HImageMean, HPinRgn;
		MeanImage(HImageReduced, &HImageMean, iDTMeanFilterSize, iDTMeanFilterSize);
		DynThreshold(HImageReduced, HImageMean, &HPinRgn, iDTValue, "light");

		if (THEAPP.m_pGFunction->ValidHRegion(HPinRgn) == FALSE)
		{
			HDefectRgn = HInspectAreaRgn;
			return HDefectRgn;
		}

		FillUp(HPinRgn, &HPinRgn);
		OpeningCircle(HPinRgn, &HPinRgn, (double)Param.m_iPinOpeningClosingSize + 0.5);
		ClosingCircle(HPinRgn, &HPinRgn, (double)(Param.m_iPinOpeningClosingSize + 1) + 0.5);
		Connection(HPinRgn, &HPinRgn);
		SelectShape(HPinRgn, &HPinRgn, "area", "and", Param.m_iPinMinArea, (Hlong)99999999999);

		int i;
		HObject HTeachingPinRgn, HTeachingPinDilatedRgn, HSelectedPinRgn;
		HTuple HTeachingPinArea, HTeachingPinCenterX, HTeachingPinCenterY;
		Hlong lTeachingPinArea;
		double dTeachingPinCenterX, dTeachingPinCenterY;
		HTuple HSelectedPinArea, HSelectedPinCenterX, HSelectedPinCenterY;
		HTuple HPinDistanceX, HPinDistanceY, HIndexX, HIndexY;
		Hlong lMinDistanceBlobIndex;
		double dPinDeformLength;
		HObject HMinDistPinRgn;

		int iNoPinDefect = 0;

		for (i = 0; i < lNoTeachingPin; i++)
		{
			SelectObj(HInspectAreaConnRgn, &HTeachingPinRgn, i + 1);

			if (THEAPP.m_pGFunction->ValidHRegion(HTeachingPinRgn) == FALSE)
				continue;

			AreaCenter(HTeachingPinRgn, &HTeachingPinArea, &HTeachingPinCenterY, &HTeachingPinCenterX);
			lTeachingPinArea = HTeachingPinArea.L();
			dTeachingPinCenterX = HTeachingPinCenterX.D();
			dTeachingPinCenterY = HTeachingPinCenterY.D();

			if (Param.m_iPinPos == CONNECTOR_PIN_TOP || Param.m_iPinPos == CONNECTOR_PIN_BOTTOM)
				DilationRectangle1(HTeachingPinRgn, &HTeachingPinDilatedRgn, 1, iPinDetectMargin * 2 + 1);
			else
				DilationRectangle1(HTeachingPinRgn, &HTeachingPinDilatedRgn, iPinDetectMargin * 2 + 1, 1);

			SelectShapeProto(HPinRgn, HTeachingPinDilatedRgn, &HSelectedPinRgn, "overlaps_abs", 1, 9999999);

			if (THEAPP.m_pGFunction->ValidHRegion(HSelectedPinRgn) == FALSE)
			{
				if (bDebugSave)
				{
					WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
					WriteRegion(HTeachingPinRgn, "c:\\DualTest\\HTeachingPinRgn.reg");
					WriteRegion(HTeachingPinDilatedRgn, "c:\\DualTest\\HTeachingPinDilatedRgn.reg");
					WriteRegion(HPinRgn, "c:\\DualTest\\HPinRgn.reg");
				}

				ConcatObj(HDefectRgn, HTeachingPinRgn, &HDefectRgn);

				++iNoPinDefect;

				continue;
			}

			AreaCenter(HSelectedPinRgn, &HSelectedPinArea, &HSelectedPinCenterY, &HSelectedPinCenterX);

			TupleSub(HSelectedPinCenterX, HTeachingPinCenterX, &HPinDistanceX);
			TupleSub(HSelectedPinCenterY, HTeachingPinCenterY, &HPinDistanceY);

			TupleAbs(HPinDistanceX, &HPinDistanceX);
			TupleAbs(HPinDistanceY, &HPinDistanceY);

			TupleSortIndex(HPinDistanceX, &HIndexX);
			TupleSortIndex(HPinDistanceY, &HIndexY);

			if (Param.m_iPinPos == CONNECTOR_PIN_TOP || Param.m_iPinPos == CONNECTOR_PIN_BOTTOM)
			{
				lMinDistanceBlobIndex = HIndexY[0].L();
				dPinDeformLength = HPinDistanceY[lMinDistanceBlobIndex].D();
			}
			else
			{
				lMinDistanceBlobIndex = HIndexX[0].L();
				dPinDeformLength = HPinDistanceX[lMinDistanceBlobIndex].D();
			}

			if (dPinDeformLength > Param.m_iPinDistanceTolerance)
			{
				SelectObj(HSelectedPinRgn, &HMinDistPinRgn, lMinDistanceBlobIndex + 1);

				ConcatObj(HDefectRgn, HMinDistPinRgn, &HDefectRgn);

				++iNoPinDefect;
			}
		}

		if (iNoPinDefect < iPinLengthDefectTolerance)
		{
			GenEmptyObj(&HDefectRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			Union1(HDefectRgn, &HDefectRgn);

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ConnectorPinDeformAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::DentAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		HObject HImageReduced;
		ReduceDomain(HImage, HInspectAreaRgn, &HImageReduced);

		int i, j, k;
		HObject HBrightRgn, HDarkRgn;
		HObject HDTImageReduced, HExpandReduceImage, HPartImage, HMeanImage, HDynThreshRgn;

		GenEmptyObj(&HBrightRgn);
		GenEmptyObj(&HDynThreshRgn);

		HDTImageReduced = HImageReduced;
		if (Param.m_iDentMedFilterSize > 0)
			MedianImage(HDTImageReduced, &HDTImageReduced, "circle", Param.m_iDentMedFilterSize, "mirrored");

		ExpandDomainGray(HDTImageReduced, &HExpandReduceImage, Param.m_iDentDtFilterSize / 2);
		ReduceDomain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);
		MeanImage(HExpandReduceImage, &HMeanImage, Param.m_iDentDtFilterSize, Param.m_iDentDtFilterSize);

		DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iDentBrightDtValue, "light");
		Connection(HDynThreshRgn, &HDynThreshRgn);
		SelectShape(HDynThreshRgn, &HDynThreshRgn, "area", "and", Param.m_iDentCandidateMinArea, MAX_DEF);
		SelectShape(HDynThreshRgn, &HDynThreshRgn, "inner_radius", "and", Param.m_iDentCandidateMinWidth / 2, 999999);
		OpeningCircle(HDynThreshRgn, &HDynThreshRgn, 1.5);
		ClosingCircle(HDynThreshRgn, &HDynThreshRgn, 1.5);

		if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
		{
			Union1(HDynThreshRgn, &HBrightRgn);
			Connection(HBrightRgn, &HBrightRgn);
		}

		GenEmptyObj(&HDarkRgn);
		GenEmptyObj(&HDynThreshRgn);

		DynThreshold(HExpandReduceImage, HMeanImage, &HDynThreshRgn, Param.m_iDentDarkDtValue, "dark");
		Connection(HDynThreshRgn, &HDynThreshRgn);
		SelectShape(HDynThreshRgn, &HDynThreshRgn, "area", "and", Param.m_iDentCandidateMinArea, MAX_DEF);
		SelectShape(HDynThreshRgn, &HDynThreshRgn, "inner_radius", "and", Param.m_iDentCandidateMinWidth / 2, 999999);
		OpeningCircle(HDynThreshRgn, &HDynThreshRgn, 1.5);
		ClosingCircle(HDynThreshRgn, &HDynThreshRgn, 1.5);

		if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
		{
			Union1(HDynThreshRgn, &HDarkRgn);
			Connection(HDarkRgn, &HDarkRgn);
		}

		double dDilationSize;
		dDilationSize = (double)Param.m_iDentCandidateMinDistance*0.5 + 1.5;

		Hlong lNoBrightBlob, lNoDarkBlob;
		HTuple HlNoDarkBlob, HlNoBrightBlob;
		CountObj(HBrightRgn, &HlNoBrightBlob);
		CountObj(HDarkRgn, &HlNoDarkBlob);
		lNoDarkBlob = HlNoDarkBlob.L();
		lNoBrightBlob = HlNoBrightBlob.L();

		if (lNoBrightBlob <= 0 || lNoDarkBlob <= 0)
		{
			return HDefectRgn;
		}

		HObject HBrightSelectedRgn, HBrightDilatedRgn, HDarkSelectedRgn, HDarkDilatedRgn, HIntersectRgn, HRegionUnion;
		Hlong lDefectArea;
		double dCenterX, dCenterY;
		HTuple HlDefectArea, HdCenterX, HdCenterY;

		for (i = 1; i <= lNoBrightBlob; i++)
		{
			SelectObj(HBrightRgn, &HBrightSelectedRgn, i);

			if (THEAPP.m_pGFunction->ValidHRegion(HBrightSelectedRgn) == FALSE)
				continue;

			DilationCircle(HBrightSelectedRgn, &HBrightDilatedRgn, dDilationSize);

			for (j = 1; j <= lNoDarkBlob; j++)
			{
				SelectObj(HDarkRgn, &HDarkSelectedRgn, j);
				if (THEAPP.m_pGFunction->ValidHRegion(HDarkSelectedRgn) == FALSE)
					continue;

				DilationCircle(HDarkSelectedRgn, &HDarkDilatedRgn, dDilationSize);

				Intersection(HBrightDilatedRgn, HDarkDilatedRgn, &HIntersectRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == TRUE)
				{
					Union2(HBrightSelectedRgn, HDarkSelectedRgn, &HRegionUnion);
					ClosingCircle(HRegionUnion, &HRegionUnion, dDilationSize);
					FillUp(HRegionUnion, &HRegionUnion);

					AreaCenter(HRegionUnion, &HlDefectArea, &HdCenterY, &HdCenterX);
					if (HlDefectArea.L() >= Param.m_iDentMinArea)
					{
						ShapeTrans(HRegionUnion, &HRegionUnion, "ellipse");
						ConcatObj(HDefectRgn, HRegionUnion, &HDefectRgn);
					}
				}
			}
		}


		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			Union1(HDefectRgn, &HDefectRgn);

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::DentAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::HeightMeasureAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, double *pdHeight)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "ima", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		HObject HImageReduced;
		ReduceDomain(HImage, HInspectAreaRgn, &HImageReduced);

		HObject HInvalidLaserRgn, HValidLaserRgn;
		Threshold(HImageReduced, &HInvalidLaserRgn, INVALID_LASER_DATA, INVALID_LASER_DATA);
		Difference(HInspectAreaRgn, HInvalidLaserRgn, &HValidLaserRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HValidLaserRgn) == FALSE)
			return HDefectRgn;

		HTuple HAvg, HSd;
		HTuple HMin, HMax, HRange;
		double dAvg, dMax;

		if (Param.m_iHeightMeasureType == 0)
		{
			Intensity(HValidLaserRgn, HImage, &HAvg, &HSd);
			dAvg = HAvg.D();
			*pdHeight = dAvg * 0.01 * 0.001;
		}
		else if (Param.m_iHeightMeasureType == 1)
		{
			MinMaxGray(HValidLaserRgn, HImage, 5, &HMin, &HMax, &HRange);
			dMax = HMax.D();
			*pdHeight = dMax * 0.01 * 0.001;
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::HeightMeasureAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::BoundaryInspectionAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, BOOL bTeachMode)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "ima", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		HObject HInspectContour;
		HTuple HRow, HCol, HNoTuple, HRadius, HAngle, HImageWidth, HImageHeight, HLength, Indices, Inverted;
		Hlong lNoTuple, lSelectedIndex;

		GenContoursSkeletonXld(HInspectAreaRgn, &HInspectContour, 1, "generalize1");
		UnionAdjacentContoursXld(HInspectContour, &HInspectContour, 10, 1, "attr_keep");
		LengthXld(HInspectContour, &HLength);
		TupleSortIndex(HLength, &Indices);
		TupleInverse(Indices, &Inverted);
		lSelectedIndex = Inverted[0].L();
		SelectObj(HInspectContour, &HInspectContour, lSelectedIndex + 1);

		CountObj(HInspectContour, &HNoTuple);
		if (HNoTuple > 1)
			return HDefectRgn;

		GetContourXld(HInspectContour, &HRow, &HCol);

		Hlong lNoPoint;

		TupleLength(HRow, &HNoTuple);
		lNoPoint = HNoTuple.L();
		if (lNoPoint <= 0)
			return HDefectRgn;

		HTuple HIsClosed;
		bool bIsClosed;
		TestClosedXld(HInspectContour, &HIsClosed);
		if (HIsClosed == 0)
			bIsClosed = false;
		else
			bIsClosed = true;

		//if (HRow[0] > HRow[lNoPoint - 1])
		//{
		//	TupleInverse(HRow, &HRow);
		//	TupleInverse(HCol, &HCol);
		//	GenContourPolygonXld(&HInspectContour, HRow, HCol);
		//}

		GetContourAngleXld(HInspectContour, "abs", "range", 3, &HAngle);

		GetImageSize(HImage, &HImageWidth, &HImageHeight);

		HTuple HRowE, HColE, HEdgeDev, HEdgeRoiDev;

		TupleGenConst(0, 0, &HRowE);
		TupleGenConst(0, 0, &HColE);
		TupleGenConst(0, 0, &HEdgeDev);
		TupleGenConst(0, 0, &HEdgeRoiDev);

		Hlong lNoSampling = Param.m_iBoundaryEdgeMeasureSampling;
		if (lNoSampling < 1)
			lNoSampling = 1;
		if (lNoSampling > 50)
			lNoSampling = 50;
		Hlong lMeasureLength = Param.m_iBoundaryEdgeMeasureSensorLength;
		int    SmMax = (int)((double)lMeasureLength / 2.0 / 2.0);
		double dEdgeMeasureSmFactor = Param.m_dBoundaryEdgeMeasureSmFactor;
		if (dEdgeMeasureSmFactor > SmMax)
			dEdgeMeasureSmFactor = SmMax;
		double dEdgeStr = Param.m_dBoundaryEdgeMeasureEdgeStr;
		int iEdgeDir = Param.m_iBoundaryEdgeMeasureDir;
		int iEdgeMeasurePos = Param.m_iBoundaryEdgeMeasurePos;
		int iEdgeMeasureGv = Param.m_iBoundaryEdgeMeasureGv;

		HTuple HAngleP, MeasureHandle, hv_RowP1, hv_ColP1;
		HTuple RowEdge, ColumnEdge, Amplitude, Distance, HNoEdge, HShift, HPrevShift, HPrevAbsShift;
		Hlong NoEdge, MaxAmpEdgeIndex;
		HTuple HGenRowEdge, HGenColEdge;
		int i;

		HPrevShift = -1;
		HPrevAbsShift = -1;

		for (i = 0; i < lNoPoint; i += lNoSampling)
		{
			if (iEdgeDir)
				HAngleP = HAngle[i] - (PI / 2.);
			else
				HAngleP = HAngle[i] + (PI / 2.);

			GenMeasureRectangle2(HRow[i], HCol[i], HAngleP, lMeasureLength / 2, 2.5, HImageWidth, HImageHeight, "nearest_neighbor", &MeasureHandle);

			hv_RowP1 = HTuple(HRow[i]) + (lMeasureLength / 2 * (HAngleP.TupleSin()));
			hv_ColP1 = HTuple(HCol[i]) - (lMeasureLength / 2 * (HAngleP.TupleCos()));

			if (iEdgeMeasurePos == 0)
			{
				if (iEdgeMeasureGv == 0)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "all", "all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 1)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "negative", "all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 2)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "positive", "all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			}
			else if (iEdgeMeasurePos == 1)
			{
				if (iEdgeMeasureGv == 0)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "all", "first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 1)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "negative", "first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 2)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "positive", "first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			}
			else if (iEdgeMeasurePos == 2)
			{
				if (iEdgeMeasureGv == 0)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "all", "last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 1)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "negative", "last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 2)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "positive", "last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			}

			TupleLength(Amplitude, &HNoEdge);
			NoEdge = HNoEdge.L();
			if (NoEdge > 0)
			{
				TupleAbs(Amplitude, &Amplitude);
				TupleSortIndex(Amplitude, &Indices);
				TupleInverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();

				DistancePp(hv_RowP1, hv_ColP1, RowEdge[MaxAmpEdgeIndex], ColumnEdge[MaxAmpEdgeIndex], &HShift);

				TupleConcat(HRowE, RowEdge[MaxAmpEdgeIndex], &HRowE);
				TupleConcat(HColE, ColumnEdge[MaxAmpEdgeIndex], &HColE);
				TupleConcat(HEdgeDev, HShift, &HEdgeDev);

				HPrevShift = HShift;

				DistancePp(HRow[i], HCol[i], RowEdge[MaxAmpEdgeIndex], ColumnEdge[MaxAmpEdgeIndex], &HShift);
				TupleConcat(HEdgeRoiDev, HShift, &HEdgeRoiDev);

				HPrevAbsShift = HShift;
			}
			else
			{
				if (HPrevShift > -1)
				{
					HGenRowEdge = hv_RowP1 - HPrevShift * HAngleP.TupleSin();
					HGenColEdge = hv_ColP1 + HPrevShift * HAngleP.TupleCos();

					TupleConcat(HRowE, HGenRowEdge, &HRowE);
					TupleConcat(HColE, HGenColEdge, &HColE);
					TupleConcat(HEdgeDev, HPrevShift, &HEdgeDev);
					TupleConcat(HEdgeRoiDev, HPrevAbsShift, &HEdgeRoiDev);
				}
			}

			CloseMeasure(MeasureHandle);
		}

		Hlong lNoProcessedPoint;

		TupleLength(HEdgeDev, &HNoTuple);
		lNoProcessedPoint = HNoTuple.L();
		if (lNoProcessedPoint <= 0)
			return HDefectRgn;

		float* pData, * pMedian, * pOut;
		THEAPP.m_pGFunction->ConvertTupleToFloat(HEdgeDev, &pData);

		pMedian = (float*)malloc(lNoProcessedPoint * sizeof(float));
		pOut = (float*)malloc(lNoProcessedPoint * sizeof(float));

		int sz1;
		double dSpecPTDepth, dSpecMBDepth, dSpecLength, dSh;
		BOOL bDefect;
		HObject HDefectSubRgn;

		HTuple HEdgeDist, HMedianDist;

		TupleGenConst(0, 0, &HEdgeDist);
		TupleGenConst(0, 0, &HMedianDist);

		for (int iIndex = 0; iIndex < 2; iIndex++)
		{
			if (Param.m_bBoundaryCondition[iIndex])
			{
				sz1 = Param.m_iBoundaryMedianFilterSize[iIndex] / lNoSampling;

				Run_Median_Stuetzle(pData, (int)lNoProcessedPoint, sz1, pMedian, bIsClosed);

				dSpecPTDepth = Param.m_dBoundaryDefectDepth[iIndex];
				dSpecMBDepth = Param.m_dBoundaryDefectMBDepth[iIndex];
				dSpecLength = Param.m_dBoundaryDefectLength[iIndex];

				bDefect = FALSE;

				if (Param.m_bBoundaryUseRoiConnected)
				{
					/// 외각 라인과의 연결성 개선
// 후보군 그룹 탐색
					double dCandidateThreshold = 10.0;
					if (dCandidateThreshold >= dSpecPTDepth)
						dCandidateThreshold = dSpecPTDepth / 2;
					double dConnectedThreshold = dSpecPTDepth - 20.0;
					if (dConnectedThreshold <= dCandidateThreshold)
						dConnectedThreshold = dSpecPTDepth - ((dSpecPTDepth - dCandidateThreshold) / 2);

					int startIdx = 0;
					BOOL inGroup = FALSE;
					BOOL defectFound = FALSE;
					BOOL dConnectedFound = FALSE;
					for (i = 0; i < lNoProcessedPoint; i++)
					{
						if (bDebugSave && iIndex == 0)
						{
							TupleConcat(HEdgeDist, pData[i], &HEdgeDist);
							TupleConcat(HMedianDist, pMedian[i], &HMedianDist);
						}

						dSh = pData[i] - pMedian[i];
						if (dSh >= dCandidateThreshold || dSh <= -dCandidateThreshold)
						{
							if (!inGroup)
							{
								startIdx = i;
								inGroup = TRUE;
							}

							// 후보군중에 FPCB와 연결성이 있는 포인트가 있는지 판단
							if ((dSh <= dConnectedThreshold && dSh >= dCandidateThreshold) || (dSh >= -dConnectedThreshold && dSh <= -dCandidateThreshold))
								dConnectedFound = TRUE;

							// 후보군중에 불량 스펙에 만족하는 포인트가 있는지 판단
							if (dSh >= dSpecPTDepth || dSh <= -dSpecMBDepth)
								defectFound = TRUE;
						}
						else
						{
							// 후보군 그룹이 시작된 상태라면 불량 여부 판단, 후보군 그룹이 시작되지 않았다면 해당 포인트 양품 처리
							if (inGroup)
							{
								// 후보군중에 불량 스펙에 만족하는 포인트가 있다면 해당 후보군 전체 불량 처리
								if (defectFound && dConnectedFound)
								{
									for (int j = startIdx; j < i; j++)
									{
										*(pOut + j) = 1;
										bDefect = TRUE;
									}
								}
								else
								{
									for (int j = startIdx; j < i; j++)
										*(pOut + j) = 0;
								}
								inGroup = FALSE;
								dConnectedFound = FALSE;
								defectFound = FALSE;
							}
							else
								*(pOut + i) = 0;
						}
					}
					// lNoProcessedPoint for문이 끝난 후 그룹이 시작된 상태의 끝 처리
					if (inGroup)
					{
						if (defectFound)
						{
							for (i = startIdx; i < lNoProcessedPoint; i++)
							{
								*(pOut + i) = 1;
								bDefect = TRUE;
							}
						}
						else
						{
							for (i = startIdx; i < lNoProcessedPoint; i++)
								*(pOut + i) = 0;
						}
					}
				}
				else
				{
					for (i = 0; i < lNoProcessedPoint; i++)
					{
						if (bDebugSave)
						{
							if (iIndex == 0)
							{
								TupleConcat(HEdgeDist, pData[i], &HEdgeDist);
								TupleConcat(HMedianDist, pMedian[i], &HMedianDist);
							}
						}

						dSh = pData[i] - pMedian[i];
						if (dSh >= dSpecPTDepth || dSh <= -dSpecMBDepth)
						{
							*(pOut + i) = 1;
							bDefect = TRUE;
						}
						else
						{
							*(pOut + i) = 0;
						}
					}
				}

				if (bDebugSave)
				{
					if (iIndex == 0)
					{
						WriteTuple(HEdgeDist, "c:\\DualTest\\HEdgeDist.tup");
						WriteTuple(HMedianDist, "c:\\DualTest\\HMedianDist.tup");
					}
				}

				if (bDefect)
				{
					HTuple POut_tuple((float*)pOut, lNoProcessedPoint);

					HTuple hv_RowO, hv_ColO;
					TupleSelectMask(HRowE, POut_tuple, &hv_RowO);
					TupleSelectMask(HColE, POut_tuple, &hv_ColO);

					GenRegionPoints(&HDefectSubRgn, hv_RowO, hv_ColO);

					if (lNoSampling > 1)
					{
						HObject HDefectCandidateRgn, HLineRgn;
						GenEmptyObj(&HDefectCandidateRgn);

						for (i = 0; i < (lNoProcessedPoint - 1); i++)
						{
							if (*(pOut + i) == 1 && *(pOut + i + 1) == 1)
							{
								GenRegionLine(&HLineRgn, HRowE[i], HColE[i], HRowE[i + 1], HColE[i + 1]);
								ConcatObj(HDefectCandidateRgn, HLineRgn, &HDefectCandidateRgn);
							}
						}

						if (THEAPP.m_pGFunction->ValidHRegion(HDefectCandidateRgn))
						{
							Union1(HDefectCandidateRgn, &HDefectCandidateRgn);
							Union2(HDefectSubRgn, HDefectCandidateRgn, &HDefectSubRgn);
						}
					}

					if (Param.m_bBoundaryUseConnection[iIndex])
						BlobUnion(&HDefectSubRgn, Param.m_iBoundaryConnectionLength[iIndex]);

					Connection(HDefectSubRgn, &HDefectSubRgn);
					SelectShape(HDefectSubRgn, &HDefectSubRgn, "area", "and", Hlong(dSpecLength), MAX_DEF);

					Union2(HDefectRgn, HDefectSubRgn, &HDefectRgn);
				}
			}
		}

		if (Param.m_bBoundaryCondition[2])
		{
			TupleLength(HEdgeRoiDev, &HNoTuple);
			lNoProcessedPoint = HNoTuple.L();
			if (lNoProcessedPoint > 0)
			{
				if (bDebugSave)
				{
					WriteTuple(HEdgeRoiDev, "c:\\DualTest\\HEdgeRoiDev.tup");
				}

				float* pOutAbs;
				pOutAbs = (float*)malloc(lNoProcessedPoint * sizeof(float));

				dSpecPTDepth = Param.m_dBoundaryDefectDepth[2];
				dSpecLength = Param.m_dBoundaryDefectLength[2];

				bDefect = FALSE;

				for (i = 0; i < lNoProcessedPoint; i++)
				{
					dSh = HEdgeRoiDev[i].D();
					if (dSh >= dSpecPTDepth)
					{
						*(pOutAbs + i) = 1;
						bDefect = TRUE;
					}
					else
					{
						*(pOutAbs + i) = 0;
					}
				}

				if (bDefect)
				{
					HTuple POut_tuple2((float*)pOutAbs, lNoProcessedPoint);

					HTuple hv_RowO, hv_ColO;
					TupleSelectMask(HRowE, POut_tuple2, &hv_RowO);
					TupleSelectMask(HColE, POut_tuple2, &hv_ColO);

					GenRegionPoints(&HDefectSubRgn, hv_RowO, hv_ColO);

					if (lNoSampling > 1)
					{
						HObject HDefectCandidateRgn, HLineRgn;
						GenEmptyObj(&HDefectCandidateRgn);

						for (i = 0; i < (lNoProcessedPoint - 1); i++)
						{
							if (*(pOutAbs + i) == 1 && *(pOutAbs + i + 1) == 1)
							{
								GenRegionLine(&HLineRgn, HRowE[i], HColE[i], HRowE[i + 1], HColE[i + 1]);
								ConcatObj(HDefectCandidateRgn, HLineRgn, &HDefectCandidateRgn);
							}
						}

						if (THEAPP.m_pGFunction->ValidHRegion(HDefectCandidateRgn))
						{
							Union1(HDefectCandidateRgn, &HDefectCandidateRgn);
							Union2(HDefectSubRgn, HDefectCandidateRgn, &HDefectSubRgn);
						}
					}

					Connection(HDefectSubRgn, &HDefectSubRgn);
					SelectShape(HDefectSubRgn, &HDefectSubRgn, "area", "and", Hlong(dSpecLength), MAX_DEF);

					Union2(HDefectRgn, HDefectSubRgn, &HDefectRgn);
				}

				free(pOutAbs);
			}
		}

		if (bTeachMode == TRUE && Param.m_iBoundaryDisplay == 1)
			GenRegionPolygon(&HDefectRgn, HRowE, HColE);

		free(pData);
		free(pMedian);
		free(pOut);

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::BoundaryInspectionAlgorithm] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

int Algorithm::BoundaryInspectionAutoCalibration(HObject HImage, HObject HROIRgn, CAlgorithmParam* pCalParam)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, *pCalParam, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return -1;

		if (bDebugSave)
		{
			WriteImage(HImage, "ima", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		HObject HInspectContour;
		HTuple HRow, HCol, HNoTuple, HRadius, HAngle, HImageWidth, HImageHeight, HLength, Indices, Inverted;
		Hlong lNoTuple, lSelectedIndex;

		GenContoursSkeletonXld(HInspectAreaRgn, &HInspectContour, 1, "generalize1");
		UnionAdjacentContoursXld(HInspectContour, &HInspectContour, 10, 1, "attr_keep");
		LengthXld(HInspectContour, &HLength);
		TupleSortIndex(HLength, &Indices);
		TupleInverse(Indices, &Inverted);
		lSelectedIndex = Inverted[0].L();
		SelectObj(HInspectContour, &HInspectContour, lSelectedIndex + 1);

		CountObj(HInspectContour, &HNoTuple);
		if (HNoTuple > 1)
			return -2;

		GetContourXld(HInspectContour, &HRow, &HCol);

		Hlong lNoPoint;

		TupleLength(HRow, &HNoTuple);
		lNoPoint = HNoTuple.L();
		if (lNoPoint <= 0)
			return -3;

		GetContourAngleXld(HInspectContour, "abs", "range", 3, &HAngle);

		GetImageSize(HImage, &HImageWidth, &HImageHeight);

		Hlong lNoSampling = pCalParam->m_iBoundaryEdgeMeasureSampling;
		if (lNoSampling < 1)
			lNoSampling = 1;
		if (lNoSampling > 50)
			lNoSampling = 50;
		Hlong lMeasureLength = pCalParam->m_iBoundaryEdgeMeasureSensorLength;
		double dEdgeMeasureSmFactor = pCalParam->m_dBoundaryEdgeMeasureSmFactor;
		if (dEdgeMeasureSmFactor > 7)
			dEdgeMeasureSmFactor = 7.0;
		double dEdgeStr = pCalParam->m_dBoundaryEdgeMeasureEdgeStr;
		int iEdgeDir = pCalParam->m_iBoundaryEdgeMeasureDir;
		int iEdgeMeasurePos = pCalParam->m_iBoundaryEdgeMeasurePos;
		int iEdgeMeasureGv = pCalParam->m_iBoundaryEdgeMeasureGv;

		Hlong nC = lNoPoint;

		int CheckStep_min = 5;
		int CheckStep_max = 20;
		int Sample_min = 50;
		int Sample_max = 300;

		int n_step = min(CheckStep_max, max(CheckStep_min, (int)(nC / 100)));
		int n_sample = min(Sample_max, max(Sample_min, (int)(nC / n_step)));
		if (n_sample > nC)
			n_sample = nC;
		n_step = max(1, (int)(nC / n_sample));

		double dCalEdgeStr = 5.0;

		int    vote_swap = 0;
		HTuple Amps_D0, Amps_D1;
		HTuple MH0, MH1;
		HTuple HEdgePolarity;

		HTuple HAbsAmp;
		int MaxAmpEdgeIndex;

		for (int k = 0; k < n_sample; ++k)
		{
			int idx = k * n_step;
			if (idx >= nC)
				break;

			double row_k = HRow[idx].D();
			double col_k = HCol[idx].D();
			double ang_k = HAngle[idx].D();

			double a0 = ang_k + PI / 2.;
			GenMeasureRectangle2(row_k, col_k, a0, lMeasureLength / 2, 2.5, HImageWidth, HImageHeight, "nearest_neighbor", &MH0);

			HTuple RE0, CE0, Amp0, D0;
			try {
				MeasurePos(HImage, MH0, 2.0, dCalEdgeStr, "all", "all", &RE0, &CE0, &Amp0, &D0);
			}
			catch (...) {}

			if (RE0.Length() > 0)
			{
				vote_swap += 1;
				TupleAbs(Amp0, &HAbsAmp);
				TupleSortIndex(HAbsAmp, &Indices);
				TupleInverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();
				TupleConcat(Amps_D0, Amp0[MaxAmpEdgeIndex], &Amps_D0);
			}
			CloseMeasure(MH0);

			double a1 = ang_k - PI / 2.0;
			GenMeasureRectangle2(row_k, col_k, a1, lMeasureLength / 2, 2.5, HImageWidth, HImageHeight, "nearest_neighbor", &MH1);

			HTuple RE1, CE1, Amp1, D1;
			try {
				MeasurePos(HImage, MH1, 2.0, dCalEdgeStr, "all", "all", &RE1, &CE1, &Amp1, &D1);
			}
			catch (...) {}

			if (RE1.Length() > 0)
			{
				vote_swap -= 1;
				TupleAbs(Amp1, &HAbsAmp);
				TupleSortIndex(HAbsAmp, &Indices);
				TupleInverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();
				TupleConcat(Amps_D1, Amp1[MaxAmpEdgeIndex], &Amps_D1);
			}
			CloseMeasure(MH1);
		}

		HTuple WinAmps;
		if (vote_swap >= 0) {
			iEdgeDir = 0;
			WinAmps = Amps_D0;
		}
		else {
			iEdgeDir = 1;
			WinAmps = Amps_D1;
		}

		if (WinAmps.Length() > 0)
		{
			HTuple AmpSum;
			TupleSum(WinAmps, &AmpSum);
			double s = AmpSum[0].D();

			if (s > 0)
				iEdgeMeasureGv = 2;
			else if (s < 0)
				iEdgeMeasureGv = 1;
			else
				iEdgeMeasureGv = 0;
		}
		else
		{
			iEdgeMeasureGv = 0;
		}

		if (iEdgeMeasureGv == 2)  HEdgePolarity = HTuple("positive");
		else if (iEdgeMeasureGv == 1)  HEdgePolarity = HTuple("negative");
		else if (iEdgeMeasureGv == 0)  HEdgePolarity = HTuple("all");

		HTuple MH;
		HTuple AllAbsAmps;
		double dInitSmFactor = 1.0;

		for (int k = 0; k < n_sample; ++k)
		{
			int idx = k * n_step;
			if (idx >= nC)
				break;

			double row_k = HRow[idx].D();
			double col_k = HCol[idx].D();
			double ang_k = HAngle[idx].D();
			double a = (iEdgeDir == 0) ? ang_k + PI / 2.0 : ang_k - PI / 2.0;

			GenMeasureRectangle2(row_k, col_k, a, lMeasureLength / 2, 2.5, HImageWidth, HImageHeight, "nearest_neighbor", &MH);

			HTuple RE, CE, Amp, D;
			try {
				MeasurePos(HImage, MH, dInitSmFactor, dCalEdgeStr, HEdgePolarity, "all", &RE, &CE, &Amp, &D);
			}
			catch (...) {}

			if (RE.Length() > 0)
			{
				HTuple AmpAbs;
				TupleAbs(Amp, &AmpAbs);

				HTuple AmpMax;
				TupleMax(AmpAbs, &AmpMax);

				TupleConcat(AllAbsAmps, AmpMax, &AllAbsAmps);
			}
			CloseMeasure(MH);
		}

		if (AllAbsAmps.Length() > 0)
		{
			HTuple Sorted;
			TupleSort(AllAbsAmps, &Sorted);
			int idx20 = max(0, (int)(Sorted.Length() * 0.2));
			double val20 = Sorted[idx20].D();
			int ival20 = (int)val20;
			dEdgeStr = (double)max(5, ival20);
		}
		else
		{
			dEdgeStr = pCalParam->m_dBoundaryEdgeMeasureEdgeStr;
		}

		int SmMax = (int)((double)lMeasureLength / 2.0 / 2.0);
		int BestAny = -1;
		int BestSingle = -1;

		for (int sm_try = 1; sm_try <= SmMax; ++sm_try)
		{
			int cnt_single = 0, cnt_any = 0;

			for (int k = 0; k < n_sample; ++k)
			{
				int idx = k * n_step;
				if (idx >= nC)
					break;

				double row_k = HRow[idx].D();
				double col_k = HCol[idx].D();
				double ang_k = HAngle[idx].D();
				double a = (iEdgeDir == 0) ? ang_k + PI / 2.0 : ang_k - PI / 2.0;

				GenMeasureRectangle2(row_k, col_k, a, lMeasureLength / 2, 2.5, HImageWidth, HImageHeight, "nearest_neighbor", &MH);

				HTuple RE, CE, Amp, D;
				try {
					MeasurePos(HImage, MH, (double)sm_try, dEdgeStr, HEdgePolarity, "all", &RE, &CE, &Amp, &D);
				}
				catch (...) {}

				Hlong ne = RE.Length();
				if (ne == 1)
					cnt_single++;
				if (ne > 0)
					cnt_any++;

				CloseMeasure(MH);
			}

			if (cnt_any > BestAny)
			{
				BestAny = cnt_any;
				BestSingle = cnt_single;
				dEdgeMeasureSmFactor = (double)sm_try;
			}
			else if (cnt_any == BestAny)
			{
				if (cnt_single > BestSingle)
				{
					BestSingle = cnt_single;
					dEdgeMeasureSmFactor = (double)sm_try;
				}
			}
		}

		HTuple Dist_to_first, Dist_to_last;
		int    cnt_multi = 0;

		for (int k = 0; k < n_sample; ++k)
		{
			int idx = k * n_step;
			if (idx >= nC)
				break;

			double row_k = HRow[idx].D();
			double col_k = HCol[idx].D();
			double ang_k = HAngle[idx].D();
			double a = (iEdgeDir == 0) ? ang_k + PI / 2.0 : ang_k - PI / 2.0;

			GenMeasureRectangle2(row_k, col_k, a, lMeasureLength / 2, 2.5, HImageWidth, HImageHeight, "nearest_neighbor", &MH);

			HTuple RE, CE, Amp, D;
			try {
				MeasurePos(HImage, MH, dEdgeMeasureSmFactor, dEdgeStr, HEdgePolarity, "all", &RE, &CE, &Amp, &D);
			}
			catch (...) {}

			Hlong ne = RE.Length();
			if (ne >= 2)
			{
				cnt_multi++;

				HTuple d_first, d_last;
				DistancePp(row_k, col_k, RE[0].D(), CE[0].D(), &d_first);
				DistancePp(row_k, col_k, RE[ne - 1].D(), CE[ne - 1].D(), &d_last);

				TupleConcat(Dist_to_first, d_first, &Dist_to_first);
				TupleConcat(Dist_to_last, d_last, &Dist_to_last);
			}
			CloseMeasure(MH);
		}

		if (cnt_multi == 0)
		{
			iEdgeMeasurePos = 0;
		}
		else
		{
			HTuple MaxF, MinF, MaxL, MinL;
			TupleMax(Dist_to_first, &MaxF);
			TupleMin(Dist_to_first, &MinF);
			TupleMax(Dist_to_last, &MaxL);
			TupleMin(Dist_to_last, &MinL);

			double rangeF = MaxF[0].D() - MinF[0].D();
			double rangeL = MaxL[0].D() - MinL[0].D();

			iEdgeMeasurePos = (rangeF <= rangeL) ? 1 : 2;
		}

		pCalParam->m_dBoundaryEdgeMeasureSmFactor = dEdgeMeasureSmFactor;
		pCalParam->m_dBoundaryEdgeMeasureEdgeStr = dEdgeStr;
		pCalParam->m_iBoundaryEdgeMeasureDir = iEdgeDir;
		pCalParam->m_iBoundaryEdgeMeasurePos = iEdgeMeasurePos;
		pCalParam->m_iBoundaryEdgeMeasureGv = iEdgeMeasureGv;

		return 0;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::BoundaryInspectionAutoCalibration] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return -4;
	}
}

HObject Algorithm::BoundaryInspectionAssessment(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, double* pdScore)
{
	try
	{
		double tStartTime = GetTickCount();

		BOOL bDebugSave = FALSE;

		*pdScore = 0;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "ima", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		HObject HInspectContour;
		HTuple HRow, HCol, HNoTuple, HRadius, HAngle, HImageWidth, HImageHeight, HLength, Indices, Inverted;
		Hlong lNoTuple, lSelectedIndex;

		GenContoursSkeletonXld(HInspectAreaRgn, &HInspectContour, 1, "generalize1");
		UnionAdjacentContoursXld(HInspectContour, &HInspectContour, 10, 1, "attr_keep");
		LengthXld(HInspectContour, &HLength);
		TupleSortIndex(HLength, &Indices);
		TupleInverse(Indices, &Inverted);
		lSelectedIndex = Inverted[0].L();
		SelectObj(HInspectContour, &HInspectContour, lSelectedIndex + 1);

		CountObj(HInspectContour, &HNoTuple);
		if (HNoTuple > 1)
			return HDefectRgn;

		GetContourXld(HInspectContour, &HRow, &HCol);

		Hlong lNoPoint;

		TupleLength(HRow, &HNoTuple);
		lNoPoint = HNoTuple.L();
		if (lNoPoint <= 0)
			return HDefectRgn;

		HTuple HIsClosed;
		bool bIsClosed;
		TestClosedXld(HInspectContour, &HIsClosed);
		if (HIsClosed == 0)
			bIsClosed = false;
		else
			bIsClosed = true;

		GetContourAngleXld(HInspectContour, "abs", "range", 3, &HAngle);

		GetImageSize(HImage, &HImageWidth, &HImageHeight);

		HTuple HRowE, HColE, HEdgeDev, HEdgeRoiDev;

		TupleGenConst(0, 0, &HRowE);
		TupleGenConst(0, 0, &HColE);
		TupleGenConst(0, 0, &HEdgeDev);
		TupleGenConst(0, 0, &HEdgeRoiDev);

		Hlong lNoSampling = Param.m_iBoundaryEdgeMeasureSampling;
		if (lNoSampling < 1)
			lNoSampling = 1;
		if (lNoSampling > 50)
			lNoSampling = 50;
		Hlong lMeasureLength = Param.m_iBoundaryEdgeMeasureSensorLength;
		int    SmMax = (int)((double)lMeasureLength / 2.0 / 2.0);
		double dEdgeMeasureSmFactor = Param.m_dBoundaryEdgeMeasureSmFactor;
		if (dEdgeMeasureSmFactor > SmMax)
			dEdgeMeasureSmFactor = SmMax;
		double dEdgeStr = Param.m_dBoundaryEdgeMeasureEdgeStr;
		int iEdgeDir = Param.m_iBoundaryEdgeMeasureDir;
		int iEdgeMeasurePos = Param.m_iBoundaryEdgeMeasurePos;
		int iEdgeMeasureGv = Param.m_iBoundaryEdgeMeasureGv;

		HTuple HAngleP, MeasureHandle, hv_RowP1, hv_ColP1;
		HTuple RowEdge, ColumnEdge, Amplitude, Distance, HNoEdge, HShift, HPrevShift, HPrevAbsShift;
		Hlong NoEdge, MaxAmpEdgeIndex;
		HTuple HGenRowEdge, HGenColEdge;
		int i;
		int iNoCntDet, iNoCntMiss;
		HTuple HDetectFlag;

		TupleGenConst(0, 0, &HDetectFlag);
		iNoCntDet = iNoCntMiss = 0;
		HPrevShift = -1;
		HPrevAbsShift = -1;

		for (i = 0; i < lNoPoint; i += lNoSampling)
		{
			if (iEdgeDir)
				HAngleP = HAngle[i] - (PI / 2.);
			else
				HAngleP = HAngle[i] + (PI / 2.);

			GenMeasureRectangle2(HRow[i], HCol[i], HAngleP, lMeasureLength / 2, 2.5, HImageWidth, HImageHeight, "nearest_neighbor", &MeasureHandle);

			hv_RowP1 = HTuple(HRow[i]) + (lMeasureLength / 2 * (HAngleP.TupleSin()));
			hv_ColP1 = HTuple(HCol[i]) - (lMeasureLength / 2 * (HAngleP.TupleCos()));

			if (iEdgeMeasurePos == 0)
			{
				if (iEdgeMeasureGv == 0)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "all", "all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 1)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "negative", "all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 2)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "positive", "all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			}
			else if (iEdgeMeasurePos == 1)
			{
				if (iEdgeMeasureGv == 0)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "all", "first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 1)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "negative", "first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 2)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "positive", "first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			}
			else if (iEdgeMeasurePos == 2)
			{
				if (iEdgeMeasureGv == 0)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "all", "last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 1)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "negative", "last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				else if (iEdgeMeasureGv == 2)
					MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, dEdgeStr, "positive", "last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			}

			TupleLength(Amplitude, &HNoEdge);
			NoEdge = HNoEdge.L();
			if (NoEdge > 0)
			{
				TupleAbs(Amplitude, &Amplitude);
				TupleSortIndex(Amplitude, &Indices);
				TupleInverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();

				DistancePp(hv_RowP1, hv_ColP1, RowEdge[MaxAmpEdgeIndex], ColumnEdge[MaxAmpEdgeIndex], &HShift);

				TupleConcat(HRowE, RowEdge[MaxAmpEdgeIndex], &HRowE);
				TupleConcat(HColE, ColumnEdge[MaxAmpEdgeIndex], &HColE);
				TupleConcat(HEdgeDev, HShift, &HEdgeDev);

				HPrevShift = HShift;

				DistancePp(HRow[i], HCol[i], RowEdge[MaxAmpEdgeIndex], ColumnEdge[MaxAmpEdgeIndex], &HShift);
				TupleConcat(HEdgeRoiDev, HShift, &HEdgeRoiDev);

				HPrevAbsShift = HShift;

				++iNoCntDet;
				TupleConcat(HDetectFlag, 1, &HDetectFlag);
			}
			else
			{
				if (HPrevShift > -1)
				{
					HGenRowEdge = hv_RowP1 - HPrevShift * HAngleP.TupleSin();
					HGenColEdge = hv_ColP1 + HPrevShift * HAngleP.TupleCos();

					TupleConcat(HRowE, HGenRowEdge, &HRowE);
					TupleConcat(HColE, HGenColEdge, &HColE);
					TupleConcat(HEdgeDev, HPrevShift, &HEdgeDev);
					TupleConcat(HEdgeRoiDev, HPrevAbsShift, &HEdgeRoiDev);

					TupleConcat(HDetectFlag, 0, &HDetectFlag);
				}
				else
				{
					++iNoCntMiss;
				}
			}

			CloseMeasure(MeasureHandle);
		}

		Hlong lNoProcessedPoint;

		TupleLength(HEdgeDev, &HNoTuple);
		lNoProcessedPoint = HNoTuple.L();
		if (lNoProcessedPoint <= 0)
			return HDefectRgn;

		double dScoreA, dScoreB, dScoreC;

		HTuple HNoFlag;
		Hlong lNoFlag;
		TupleLength(HDetectFlag, &HNoFlag);
		lNoFlag = HNoFlag.L();

		int iNoCntTotalInsp = lNoFlag + iNoCntMiss;
		double dDetectRate = (double)iNoCntDet * 100.0 / (double)max(1, iNoCntTotalInsp);
		dScoreA = dDetectRate;

		HTuple HEqual, HValidEdgeDev;
		TupleEqualElem(HDetectFlag, 1, &HEqual);
		TupleSelectMask(HEdgeDev, HEqual, &HValidEdgeDev);

		HTuple HNoValidEdgeDev;
		Hlong lNoValidEdgeDev;
		TupleLength(HValidEdgeDev, &HNoValidEdgeDev);
		lNoValidEdgeDev = HNoValidEdgeDev.L();

		HTuple HValidEdgeDist, HValidEdgeDistMean, HValidEdgeDistStd;
		double dValidEdgeDistMean, dValidEdgeDistStd;
		double dDistRef, dDistStdRef;
		dDistRef = 5.0;
		dDistStdRef = 5.0;

		dValidEdgeDistMean = 0;
		dValidEdgeDistStd = 0;

		if (lNoValidEdgeDev > 1)
		{
			TupleSub(lMeasureLength / 2, HValidEdgeDev, &HValidEdgeDist);
			TupleMean(HValidEdgeDist, &HValidEdgeDistMean);
			TupleDeviation(HValidEdgeDist, &HValidEdgeDistStd);
			dValidEdgeDistMean = HValidEdgeDistMean.D();
			dValidEdgeDistStd = HValidEdgeDistStd.D();

			dScoreB = 100. * exp(-fabs(dValidEdgeDistMean) / dDistRef);
			dScoreC = 100. * exp(-dValidEdgeDistStd / dDistStdRef);
		}
		else
		{
			dScoreB = 0;
			dScoreC = 0;
		}

		*pdScore = (dScoreA + dScoreB + dScoreC) / 3.0;

		GenRegionPolygon(&HDefectRgn, HRowE, HColE);

		double tEndTime = GetTickCount();
		double tElapsedTime = tEndTime - tStartTime;

		CString sDecision;

		if (*pdScore >= 80)
			sDecision = "적합";
		else if (*pdScore >= 60)
			sDecision = "검토 필요";
		else
			sDecision = "부적합";

		CString sMsg;
		sMsg.Format("###     Edge 적합성     ###\r\nScore = %d (%s)\r\n\r\n  - Edge 검출율 = %d (%%)\r\n  - Edge/ROI간 평균거리 = %d (픽셀)\r\n  - Edge/ROI간 거리 표준편차 = %d (픽셀)\r\n  - Edge 검출 연산 시간 = %.2lf (mSec)", (int)*pdScore, sDecision, (int)dDetectRate, (int)dValidEdgeDistMean, (int)dValidEdgeDistStd, tElapsedTime);
		AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::BoundaryInspectionAssessment] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::ROIAlignAlgorithm(HObject HImage, HObject HInspectAreaRgn, CAlgorithmParam Param, HTuple *pHModelID, BOOL *pbShiftResult, double *pdScore, int *piLocalAlignDeltaX, int *piLocalAlignDeltaY, double *pdLocalAlignDeltaAngle, double *pdLocalAlignDeltaAngleFixedPointX, double *pdLocalAlignDeltaAngleFixedPointY, int iMatchingPosOffsetX, int iMatchingPosOffsetY, double *pdLocalAlignFindX, double *pdLocalAlignFindY)
{
	*pdScore = -1;

	try
	{
		BOOL bDebugSave = FALSE;

		HObject HAlignRgn;
		GenEmptyObj(&HAlignRgn);

		*pbShiftResult = FALSE;

		if (*pHModelID < 0)
		{
			return HAlignRgn;
		}

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		HObject HMatchingStartPosRgn;
		MoveRegion(HInspectAreaRgn, &HMatchingStartPosRgn, iMatchingPosOffsetY, iMatchingPosOffsetX);

		HTuple dAngleRangeRad;
		TupleRad((double)Param.m_iROIAlignMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		Hlong lROIArea;
		double dROICenterX, dROICenterY;
		HTuple HdROICenterX, HdROICenterY, HlROIArea;
		HObject HInspectAreaRectRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaRectRgn, "rectangle1");
		AreaCenter(HInspectAreaRectRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
		lROIArea = HlROIArea.L();
		dROICenterY = HdROICenterY.D();
		dROICenterX = HdROICenterX.D();

		int iSearchMarginX = Param.m_iROIAlignXSearchMargin;
		if (iSearchMarginX < 0)
			iSearchMarginX = 0;
		int iSearchMarginY = Param.m_iROIAlignYSearchMargin;
		if (iSearchMarginY < 0)
			iSearchMarginY = 0;

		HObject HInspectAreaDilatedRgn;
		DilationRectangle1(HMatchingStartPosRgn, &HInspectAreaDilatedRgn, iSearchMarginX * 2 + 1, iSearchMarginY * 2 + 1);

		HObject HMatchingImageReduced;
		ReduceDomain(HImage, HInspectAreaDilatedRgn, &HMatchingImageReduced);

		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, Scale, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;

		double dTransX, dTransY;
		HObject HModelContour;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Scale = HTuple(0.0);
		Score = HTuple(0.0);

		int iNoTeachNumLevel = 0;
		HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

		GetShapeModelParams(*pHModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

		iNoTeachNumLevel = NumLevels[0].L();
		if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_SHAPE)
			iNoTeachNumLevel = MAX_PYRAMID_LEVEL_SHAPE;
		else
			iNoTeachNumLevel = Param.m_iROIAlignMatchingPLevel;

		FindScaledShapeModel(HMatchingImageReduced,
			*pHModelID,									// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,							// Extent of the rotation angles.
			Param.m_dROIAlignMatchingScaleMin,
			Param.m_dROIAlignMatchingScaleMax,
			Param.m_dROIAlignMatchingScore,				// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
			iNoTeachNumLevel,							// Number of pyramid levels used in the matching
			0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&Scale,
			&Score);									// Score of the found instances of the model.

		HTuple HlNoFoundNumber;
		TupleLength(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();

		if (lNoFoundNumber <= 0)
		{
			return HAlignRgn;
		}

		double dDeltaX, dDeltaY, dAngle, dScale, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();
		dScale = Scale[0].D();

		*pdScore = dScore;

		double dAbs;
		HObject HResultXLD;

		if (Param.m_bROIAlignInspectShift)
		{
			double dShiftX, dShiftY;
			dShiftX = dDeltaX - dROICenterX;
			dShiftY = dDeltaY - dROICenterY;

			if (dShiftX >= 0)
			{
				if (dShiftX > Param.m_dROIAlignShiftRight)
					*pbShiftResult = TRUE;
			}
			else
			{
				if (fabs(dShiftX) > Param.m_dROIAlignShiftLeft)
					*pbShiftResult = TRUE;
			}

			if (dShiftY >= 0)
			{
				if (dShiftY > Param.m_dROIAlignShiftBottom)
					*pbShiftResult = TRUE;
			}
			else
			{
				if (fabs(dShiftY) > Param.m_dROIAlignShiftTop)
					*pbShiftResult = TRUE;
			}
		}

		long lMoveX, lMoveY;

		//if (Param.m_iROIAlignMatchingAngleRange == 0 && Param.m_dROIAlignMatchingScaleMin == 1 && Param.m_dROIAlignMatchingScaleMax == 1)
		//{
		//	if (Param.m_bROIAlignUsePosX)
		//	{
		//		dTransX = dDeltaX - dROICenterX;
		//		dAbs = fabs(dTransX);
		//		if (dAbs > (double)iSearchMarginX)
		//			dTransX = 0;
		//	}
		//	else
		//	{
		//		dTransX = 0;
		//	}

		//	if (Param.m_bROIAlignUsePosY)
		//	{
		//		dTransY = dDeltaY - dROICenterY;
		//		dAbs = fabs(dTransY);
		//		if (dAbs > (double)iSearchMarginY)
		//			dTransY = 0;
		//	}
		//	else
		//	{
		//		dTransY = 0;
		//	}

		//	lMoveX = (long)(dTransX + 0.5);
		//	lMoveY = (long)(dTransY + 0.5);

		//	MoveRegion(HInspectAreaRgn, &HAlignRgn, lMoveY, lMoveX);

		//	dAngle = 0;
		//}
		//else
		//{
		HomMat2dIdentity(&HomMat2DIdentity);
		HomMat2dTranslate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
		HomMat2dRotate(HomMat2DTranslate, dAngle, dDeltaY, dDeltaX, &HomMat2DRotate);
		HomMat2dScale(HomMat2DRotate, dScale, dScale, dDeltaY, dDeltaX, &HomMat2DScale);

		GetShapeModelContours(&HModelContour, *pHModelID, 1);
		AffineTransContourXld(HModelContour, &HResultXLD, HomMat2DScale);
		GenRegionContourXld(HResultXLD, &HAlignRgn, "filled");

		dTransX = dDeltaX - dROICenterX;
		dTransY = dDeltaY - dROICenterY;
		lMoveX = (long)(dTransX + 0.5);
		lMoveY = (long)(dTransY + 0.5);
		//}

		if (Param.m_bROIAlignLocalAlignUse && piLocalAlignDeltaX != NULL && piLocalAlignDeltaY != NULL && pdLocalAlignDeltaAngle != NULL)
		{
			*piLocalAlignDeltaX = lMoveX;
			*piLocalAlignDeltaY = lMoveY;
			*pdLocalAlignDeltaAngle = dAngle;
			*pdLocalAlignDeltaAngleFixedPointX = dROICenterX;
			*pdLocalAlignDeltaAngleFixedPointY = dROICenterY;
		}

		if (pdLocalAlignFindX != NULL && pdLocalAlignFindY != NULL)
		{
			*pdLocalAlignFindX = dDeltaX;
			*pdLocalAlignFindY = dDeltaY;
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
			Union1(HAlignRgn, &HAlignRgn);

		return HAlignRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ROIAlignAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HAlignRgn;
		GenEmptyObj(&HAlignRgn);
		return HAlignRgn;
	}
}

HObject Algorithm::ROIAnisoAlignAlgorithm(HObject HImage, HObject HInspectAreaRgn, CAlgorithmParam Param, HTuple *pHModelID, BOOL *pbShiftResult, double *pdScore, int *piLocalAlignDeltaX, int *piLocalAlignDeltaY, double *pdLocalAlignDeltaAngle, double *pdLocalAlignDeltaAngleFixedPointX, double *pdLocalAlignDeltaAngleFixedPointY, int iMatchingPosOffsetX, int iMatchingPosOffsetY)
{
	*pdScore = -1;

	try
	{
		BOOL bDebugSave = FALSE;

		HObject HAlignRgn;
		GenEmptyObj(&HAlignRgn);

		*pbShiftResult = FALSE;

		if (*pHModelID < 0)
		{
			return HAlignRgn;
		}

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn.reg");
		}

		HObject HMatchingStartPosRgn;
		MoveRegion(HInspectAreaRgn, &HMatchingStartPosRgn, iMatchingPosOffsetY, iMatchingPosOffsetX);

		HTuple dAngleRangeRad;
		TupleRad((double)Param.m_iROIAnisoAlignMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		Hlong lROIArea;
		double dROICenterX, dROICenterY;
		HTuple HdROICenterX, HdROICenterY, HlROIArea;
		HObject HInspectAreaRectRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaRectRgn, "rectangle1");
		AreaCenter(HInspectAreaRectRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
		lROIArea = HlROIArea.L();
		dROICenterY = HdROICenterY.D();
		dROICenterX = HdROICenterX.D();

		int iSearchMarginX = Param.m_iROIAnisoAlignXSearchMargin;
		if (iSearchMarginX < 0)
			iSearchMarginX = 0;
		int iSearchMarginY = Param.m_iROIAnisoAlignYSearchMargin;
		if (iSearchMarginY < 0)
			iSearchMarginY = 0;

		HObject HInspectAreaDilatedRgn;
		DilationRectangle1(HMatchingStartPosRgn, &HInspectAreaDilatedRgn, iSearchMarginX * 2 + 1, iSearchMarginY * 2 + 1);

		HObject HMatchingImageReduced;
		ReduceDomain(HImage, HInspectAreaDilatedRgn, &HMatchingImageReduced);

		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, ScaleX, ScaleY, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;

		double dTransX, dTransY;
		HObject HModelContour;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		ScaleX = HTuple(0.0);
		ScaleY = HTuple(0.0);
		Score = HTuple(0.0);

		int iNoTeachNumLevel = 0;
		HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

		GetShapeModelParams(*pHModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

		iNoTeachNumLevel = NumLevels[0].L();
		if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_SHAPE)
			iNoTeachNumLevel = MAX_PYRAMID_LEVEL_SHAPE;
		else
			iNoTeachNumLevel = Param.m_iROIAnisoAlignMatchingPLevel;

		FindAnisoShapeModel(HMatchingImageReduced,
			*pHModelID,							// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,
			Param.m_dROIAnisoAlignMatchingScaleMinY,
			Param.m_dROIAnisoAlignMatchingScaleMaxY,
			Param.m_dROIAnisoAlignMatchingScaleMinX,
			Param.m_dROIAnisoAlignMatchingScaleMaxX,
			Param.m_dROIAnisoAlignMatchingScore,		// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
			iNoTeachNumLevel,							// Number of pyramid levels used in the matching
			0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&ScaleY,
			&ScaleX,
			&Score);									// Score of the found instances of the model.

		HTuple HlNoFoundNumber;
		TupleLength(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();

		if (lNoFoundNumber <= 0)
		{
			return HAlignRgn;
		}

		double dDeltaX, dDeltaY, dAngle, dScaleX, dScaleY, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();
		dScaleX = ScaleX[0].D();
		dScaleY = ScaleY[0].D();

		*pdScore = dScore;

		double dAbs;
		HObject HResultXLD;

		if (Param.m_bROIAnisoAlignInspectShift)
		{
			double dShiftX, dShiftY;
			dShiftX = dDeltaX - dROICenterX;
			dShiftY = dDeltaY - dROICenterY;

			if (dShiftX >= 0)
			{
				if (dShiftX > Param.m_dROIAnisoAlignShiftRight)
					*pbShiftResult = TRUE;
			}
			else
			{
				if (fabs(dShiftX) > Param.m_dROIAnisoAlignShiftLeft)
					*pbShiftResult = TRUE;
			}

			if (dShiftY >= 0)
			{
				if (dShiftY > Param.m_dROIAnisoAlignShiftBottom)
					*pbShiftResult = TRUE;
			}
			else
			{
				if (fabs(dShiftY) > Param.m_dROIAnisoAlignShiftTop)
					*pbShiftResult = TRUE;
			}
		}

		long lMoveX, lMoveY;

		//if (Param.m_iROIAnisoAlignMatchingAngleRange == 0 && Param.m_dROIAnisoAlignMatchingScaleMinY == 1 && Param.m_dROIAnisoAlignMatchingScaleMaxY == 1)
		//{
		//	if (Param.m_bROIAnisoAlignUsePosX)
		//	{
		//		dTransX = dDeltaX - dROICenterX;
		//		dAbs = fabs(dTransX);
		//		if (dAbs > (double)iSearchMarginX)
		//			dTransX = 0;
		//	}
		//	else
		//	{
		//		dTransX = 0;
		//	}

		//	if (Param.m_bROIAnisoAlignUsePosY)
		//	{
		//		dTransY = dDeltaY - dROICenterY;
		//		dAbs = fabs(dTransY);
		//		if (dAbs > (double)iSearchMarginY)
		//			dTransY = 0;
		//	}
		//	else
		//	{
		//		dTransY = 0;
		//	}

		//	lMoveX = (long)(dTransX + 0.5);
		//	lMoveY = (long)(dTransY + 0.5);

		//	MoveRegion(HInspectAreaRgn, &HAlignRgn, lMoveY, lMoveX);

		//	dAngle = 0;
		//}
		//else
		//{
		HomMat2dIdentity(&HomMat2DIdentity);
		HomMat2dTranslate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
		HomMat2dRotate(HomMat2DTranslate, dAngle, dDeltaY, dDeltaX, &HomMat2DRotate);
		HomMat2dScale(HomMat2DRotate, dScaleY, dScaleX, dDeltaY, dDeltaX, &HomMat2DScale);

		GetShapeModelContours(&HModelContour, *pHModelID, 1);
		AffineTransContourXld(HModelContour, &HResultXLD, HomMat2DScale);
		GenRegionContourXld(HResultXLD, &HAlignRgn, "filled");

		dTransX = dDeltaX - dROICenterX;
		dTransY = dDeltaY - dROICenterY;
		lMoveX = (long)(dTransX + 0.5);
		lMoveY = (long)(dTransY + 0.5);
		//}

		if (Param.m_bROIAnisoAlignLocalAlignUse && piLocalAlignDeltaX != NULL && piLocalAlignDeltaY != NULL && pdLocalAlignDeltaAngle != NULL)
		{
			*piLocalAlignDeltaX = lMoveX;
			*piLocalAlignDeltaY = lMoveY;
			*pdLocalAlignDeltaAngle = dAngle;
			*pdLocalAlignDeltaAngleFixedPointX = dROICenterX;
			*pdLocalAlignDeltaAngleFixedPointY = dROICenterY;
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
			Union1(HAlignRgn, &HAlignRgn);

		return HAlignRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ROIAnisoAlignAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HAlignRgn;
		GenEmptyObj(&HAlignRgn);
		return HAlignRgn;
	}
}

HObject Algorithm::ROIAlignAlgorithm_MultiModule_Align(HObject HImage, HObject HInspectAreaRgn, CAlgorithmParam Param, HTuple* pHModelID, int iNoInspectModule, int iInspectBufferIndex)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HAlignRgn;
		GenEmptyObj(&HAlignRgn);

		BOOL pbModuleExist[MAX_MODULE_TRAY];
		BOOL pbModuleIsDefect[MAX_MODULE_TRAY];
		int piModuleDefectPosX[MAX_MODULE_TRAY], piModuleDefectPosY[MAX_MODULE_TRAY];
		double pdDeltaX[MAX_MODULE_TRAY], pdDeltaY[MAX_MODULE_TRAY], pdDeltaAngle[MAX_MODULE_TRAY];

		int iInspectBufferIndexTemp = iInspectBufferIndex;
		if (iInspectBufferIndexTemp == -1) iInspectBufferIndexTemp = 0;

		GenEmptyObj(m_HAlignResultRgn + iInspectBufferIndexTemp);							// Align result rgn
		m_iAlignNoInspectModule[iInspectBufferIndexTemp] = iNoInspectModule;				// No. modules to Inspect

		int i, j, k;

		for (i = 0; i < Param.m_iROIAlignMultiModuleYNumber; i++)
		{
			for (j = 0; j < Param.m_iROIAlignMultiModuleXNumber; j++)
			{
				pbModuleExist[i * Param.m_iROIAlignMultiModuleXNumber + j] = FALSE;
				pbModuleIsDefect[i * Param.m_iROIAlignMultiModuleXNumber + j] = FALSE;
				piModuleDefectPosX[i * Param.m_iROIAlignMultiModuleXNumber + j] = 0;
				piModuleDefectPosY[i * Param.m_iROIAlignMultiModuleXNumber + j] = 0;
				pdDeltaX[i * Param.m_iROIAlignMultiModuleXNumber + j] = 0;
				pdDeltaY[i * Param.m_iROIAlignMultiModuleXNumber + j] = 0;
				pdDeltaAngle[i * Param.m_iROIAlignMultiModuleXNumber + j] = 0;
			}
		}

		if (*pHModelID < 0)
		{
			return HInspectAreaRgn;
		}

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		HTuple dAngleRangeRad;
		TupleRad((double)Param.m_iROIAlignMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		Hlong lROIArea;
		double dROICenterX, dROICenterY;
		HTuple HdROICenterX, HdROICenterY, HlROIArea;
		HObject HInspectAreaRectRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaRectRgn, "rectangle1");
		AreaCenter(HInspectAreaRectRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
		lROIArea = HlROIArea.L();
		dROICenterY = HdROICenterY.D();
		dROICenterX = HdROICenterX.D();

		int iSearchMarginX = Param.m_iROIAlignXSearchMargin;
		if (iSearchMarginX < 0)
			iSearchMarginX = 0;
		int iSearchMarginY = Param.m_iROIAlignYSearchMargin;
		if (iSearchMarginY < 0)
			iSearchMarginY = 0;

		HObject HInspectAreaDilatedRgn, HModuleSearchRgn;
		DilationRectangle1(HInspectAreaRgn, &HInspectAreaDilatedRgn, iSearchMarginX * 2 + 1, iSearchMarginY * 2 + 1);

		HObject HModelContour, HModelAffineXLD, HModelAffineRgn;
		GetShapeModelContours(&HModelContour, *pHModelID, 1);

		HObject HMatchingImageReduced;
		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, Scale, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;
		double dDeltaX, dDeltaY, dAngleRad, dAngleDeg, dScale, dScore;

		double dTransX, dTransY;

		int iNoInspectedModule = 0;
		BOOL bFinish = FALSE;
		Hlong iMoveX, iMoveY;

		BOOL bModulePositionError;
		HTuple HLTPointY, HLTPointX, HRBPointY, HRBPointX;
		double dSearchCenterX, dSearchCenterY;
		Hlong lCropLTPointY, lCropLTPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

		for (i = 0; i < Param.m_iROIAlignMultiModuleYNumber; i++)
		{
			for (j = 0; j < Param.m_iROIAlignMultiModuleXNumber; j++)
			{
				iMoveX = j * Param.m_iROIAlignMultiModuleXPitch;
				iMoveY = i * Param.m_iROIAlignMultiModuleYPitch;

				MoveRegion(HInspectAreaDilatedRgn, &HModuleSearchRgn, iMoveY, iMoveX);
				ReduceDomain(HImage, HModuleSearchRgn, &HMatchingImageReduced);
				CropDomain(HMatchingImageReduced, &HMatchingImageReduced);
				SmallestRectangle1(HModuleSearchRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
				lCropLTPointY = HlLTPointY.L();
				lCropLTPointX = HlLTPointX.L();

				Row = HTuple(0.0);
				Column = HTuple(0.0);
				Angle = HTuple(0.0);
				Scale = HTuple(0.0);
				Score = HTuple(0.0);

				int iNoTeachNumLevel = 0;
				HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

				GetShapeModelParams(*pHModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

				iNoTeachNumLevel = NumLevels[0].L();
				if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_SHAPE)
					iNoTeachNumLevel = MAX_PYRAMID_LEVEL_SHAPE;
				else
					iNoTeachNumLevel = Param.m_iROIAlignMatchingPLevel;

				FindScaledShapeModel(HMatchingImageReduced,
					*pHModelID,									// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad * 2.0,							// Extent of the rotation angles.
					Param.m_dROIAlignMatchingScaleMin,
					Param.m_dROIAlignMatchingScaleMax,
					Param.m_dROIAlignMatchingScore,				// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"least_squares",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&Scale,
					&Score);									// Score of the found instances of the model.

				HTuple HlNoFoundNumber;
				TupleLength(Score, &HlNoFoundNumber);
				lNoFoundNumber = HlNoFoundNumber.L();

				if (lNoFoundNumber <= 0)
				{
					pbModuleExist[i * Param.m_iROIAlignMultiModuleXNumber + j] = FALSE;

					AreaCenter(HModuleSearchRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
					dSearchCenterY = HdROICenterY.D();
					dSearchCenterX = HdROICenterX.D();

					HomMat2dIdentity(&HomMat2DIdentity);
					HomMat2dTranslate(HomMat2DIdentity, (int)dSearchCenterY, (int)dSearchCenterX, &HomMat2DTranslate);
					AffineTransContourXld(HModelContour, &HModelAffineXLD, HomMat2DTranslate);
					GenRegionContourXld(HModelAffineXLD, &HModelAffineRgn, "filled");
					ConcatObj(HAlignRgn, HModelAffineRgn, &HAlignRgn);		// 모듈이 없는 경우 불량처리로 저장 
					ConcatObj(*(m_HAlignResultRgn + iInspectBufferIndexTemp), HModelAffineRgn, m_HAlignResultRgn + iInspectBufferIndexTemp);		// 모듈이 없는 경우 불량처리로 저장 

					// 불량여부, 이미지 위치 기록
					pbModuleIsDefect[i * Param.m_iROIAlignMultiModuleXNumber + j] = TRUE;
					SmallestRectangle1(HModelAffineRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX);
					piModuleDefectPosX[i * Param.m_iROIAlignMultiModuleXNumber + j] = HLTPointX[0].L();
					piModuleDefectPosY[i * Param.m_iROIAlignMultiModuleXNumber + j] = HLTPointY[0].L();
				}
				else
				{
					dDeltaX = Column[0].D();
					dDeltaY = Row[0].D();
					dAngleRad = Angle[0].D();
					dScore = Score[0].D();
					dScale = Scale[0].D();

					dDeltaX = dDeltaX + lCropLTPointX;
					dDeltaY = dDeltaY + lCropLTPointY;

					pbModuleExist[i * Param.m_iROIAlignMultiModuleXNumber + j] = TRUE;
					pdDeltaX[i * Param.m_iROIAlignMultiModuleXNumber + j] = dDeltaX - dROICenterX - (double)iMoveX;
					pdDeltaY[i * Param.m_iROIAlignMultiModuleXNumber + j] = dDeltaY - dROICenterY - (double)iMoveY;

					TupleDeg(Angle, &Angle);
					dAngleDeg = Angle[0].D();
					pdDeltaAngle[i * Param.m_iROIAlignMultiModuleXNumber + j] = dAngleDeg;

					if (Param.m_bROIAlignInspectShift)
					{
						bModulePositionError = FALSE;

						if (pdDeltaX[iNoInspectedModule] >= 0)	// Pixel
						{
							if (pdDeltaX[iNoInspectedModule] > Param.m_dROIAlignShiftRight)
								bModulePositionError = TRUE;
						}
						else
						{
							if (fabs(pdDeltaX[iNoInspectedModule]) > Param.m_dROIAlignShiftLeft)
								bModulePositionError = TRUE;
						}

						if (pdDeltaY[iNoInspectedModule] >= 0)
						{
							if (pdDeltaY[iNoInspectedModule] > Param.m_dROIAlignShiftBottom)
								bModulePositionError = TRUE;
						}
						else
						{
							if (fabs(pdDeltaY[iNoInspectedModule]) > Param.m_dROIAlignShiftTop)
								bModulePositionError = TRUE;
						}

						if (bModulePositionError)
						{
							HomMat2dIdentity(&HomMat2DIdentity);
							HomMat2dTranslate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
							HomMat2dRotate(HomMat2DTranslate, dAngleRad, dDeltaY, dDeltaX, &HomMat2DRotate);
							HomMat2dScale(HomMat2DRotate, dScale, dScale, dDeltaY, dDeltaX, &HomMat2DScale);
							AffineTransContourXld(HModelContour, &HModelAffineXLD, HomMat2DScale);
							GenRegionContourXld(HModelAffineXLD, &HModelAffineRgn, "filled");
							ConcatObj(HAlignRgn, HModelAffineRgn, &HAlignRgn);		// 모듈이 매칭된 경우에는 Shift 발생한 경우에 저장 
							ConcatObj(*(m_HAlignResultRgn + iInspectBufferIndexTemp), HModelAffineRgn, m_HAlignResultRgn + iInspectBufferIndexTemp);	// 모듈이 매칭된 경우에는 Align result rgn, Shift 발생한 경우에 저장 

							// 불량여부, 이미지 위치 기록
							pbModuleIsDefect[i * Param.m_iROIAlignMultiModuleXNumber + j] = TRUE;
							SmallestRectangle1(HModelAffineRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX);
							piModuleDefectPosX[i * Param.m_iROIAlignMultiModuleXNumber + j] = HLTPointX[0].L();
							piModuleDefectPosY[i * Param.m_iROIAlignMultiModuleXNumber + j] = HLTPointY[0].L();
						}
					}
				}

				++iNoInspectedModule;

				if (iNoInspectedModule >= iNoInspectModule)
				{
					bFinish = TRUE;
					break;
				}
			}

			if (bFinish)
				break;
		}

		TupleGenConst(0, 0, m_HAlignModuleExist + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);

		for (i = 0; i < iNoInspectModule; i++)
		{
			if (pbModuleExist[i])
				TupleConcat(*(m_HAlignModuleExist + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleExist + iInspectBufferIndexTemp);
			else
				TupleConcat(*(m_HAlignModuleExist + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleExist + iInspectBufferIndexTemp);

			if (pbModuleIsDefect[i])
				TupleConcat(*(m_HAlignModuleIsDefect + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
			else
				TupleConcat(*(m_HAlignModuleIsDefect + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleIsDefect + iInspectBufferIndexTemp);

			TupleConcat(*(m_HAlignModuleDefectPosX + iInspectBufferIndexTemp), HTuple(piModuleDefectPosX[i]), m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDefectPosY + iInspectBufferIndexTemp), HTuple(piModuleDefectPosY[i]), m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);

			TupleConcat(*(m_HAlignModuleDeltaX + iInspectBufferIndexTemp), HTuple(pdDeltaX[i] * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * 0.001), m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaY + iInspectBufferIndexTemp), HTuple(pdDeltaY[i] * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * 0.001), m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp), HTuple(pdDeltaAngle[i]), m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
			Union1(HAlignRgn, &HAlignRgn);

		return HAlignRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ROIAlignAlgorithm_MultiModule_Align] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		int iInspectBufferIndexTemp = iInspectBufferIndex;
		if (iInspectBufferIndexTemp == -1) iInspectBufferIndexTemp = 0;
		TupleGenConst(0, 0, m_HAlignModuleExist + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);

		for (int i = 0; i < iNoInspectModule; i++)
		{
			TupleConcat(*(m_HAlignModuleExist + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleExist + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleIsDefect + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDefectPosX + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDefectPosY + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaX + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaY + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);
		}

		HObject HAlignRgn;
		GenEmptyObj(&HAlignRgn);
		return HAlignRgn;
	}
}

HObject Algorithm::ROIAnisoAlignAlgorithm_MultiModule_Align(HObject HImage, HObject HInspectAreaRgn, CAlgorithmParam Param, HTuple *pHModelID, int iNoInspectModule, int iInspectBufferIndex)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HAlignRgn;
		GenEmptyObj(&HAlignRgn);

		BOOL pbModuleExist[MAX_MODULE_TRAY];
		BOOL pbModuleIsDefect[MAX_MODULE_TRAY];
		int piModuleDefectPosX[MAX_MODULE_TRAY], piModuleDefectPosY[MAX_MODULE_TRAY];
		double pdDeltaX[MAX_MODULE_TRAY], pdDeltaY[MAX_MODULE_TRAY], pdDeltaAngle[MAX_MODULE_TRAY];

		int iInspectBufferIndexTemp = iInspectBufferIndex;
		if (iInspectBufferIndexTemp == -1) iInspectBufferIndexTemp = 0;
		GenEmptyObj(m_HAlignResultRgn + iInspectBufferIndexTemp);							// Align result rgn
		m_iAlignNoInspectModule[iInspectBufferIndexTemp] = iNoInspectModule;				// No. modules to Inspect

		int i, j, k;

		for (i = 0; i < Param.m_iROIAnisoAlignMultiModuleYNumber; i++)
		{
			for (j = 0; j < Param.m_iROIAnisoAlignMultiModuleXNumber; j++)
			{
				pbModuleExist[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = FALSE;
				pbModuleIsDefect[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = FALSE;
				piModuleDefectPosX[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = 0;
				piModuleDefectPosY[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = 0;
				pdDeltaX[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = 0;
				pdDeltaY[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = 0;
				pdDeltaAngle[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = 0;
			}
		}

		if (*pHModelID < 0)
		{
			return HInspectAreaRgn;
		}

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		HTuple dAngleRangeRad;
		TupleRad((double)Param.m_iROIAnisoAlignMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		Hlong lROIArea;
		double dROICenterX, dROICenterY;
		HTuple HdROICenterX, HdROICenterY, HlROIArea;
		HObject HInspectAreaRectRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaRectRgn, "rectangle1");
		AreaCenter(HInspectAreaRectRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
		lROIArea = HlROIArea.L();
		dROICenterY = HdROICenterY.D();
		dROICenterX = HdROICenterX.D();

		int iSearchMarginX = Param.m_iROIAnisoAlignXSearchMargin;
		if (iSearchMarginX < 0)
			iSearchMarginX = 0;
		int iSearchMarginY = Param.m_iROIAnisoAlignYSearchMargin;
		if (iSearchMarginY < 0)
			iSearchMarginY = 0;

		HObject HInspectAreaDilatedRgn, HModuleSearchRgn;
		DilationRectangle1(HInspectAreaRgn, &HInspectAreaDilatedRgn, iSearchMarginX * 2 + 1, iSearchMarginY * 2 + 1);

		HObject HModelContour, HModelAffineXLD, HModelAffineRgn;
		GetShapeModelContours(&HModelContour, *pHModelID, 1);

		HObject HMatchingImageReduced;
		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, ScaleX, ScaleY, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;
		double dDeltaX, dDeltaY, dAngleRad, dAngleDeg, dScaleX, dScaleY, dScore;

		double dTransX, dTransY;

		int iNoInspectedModule = 0;
		BOOL bFinish = FALSE;
		Hlong iMoveX, iMoveY;

		BOOL bModulePositionError;
		HTuple HLTPointY, HLTPointX, HRBPointY, HRBPointX;
		double dSearchCenterX, dSearchCenterY;
		Hlong lCropLTPointY, lCropLTPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

		int iNoTeachNumLevel = 0;
		HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

		GetShapeModelParams(*pHModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

		iNoTeachNumLevel = NumLevels[0].L();
		if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_SHAPE)
			iNoTeachNumLevel = MAX_PYRAMID_LEVEL_SHAPE;
		else
			iNoTeachNumLevel = Param.m_iROIAnisoAlignMatchingPLevel;

		for (i = 0; i < Param.m_iROIAnisoAlignMultiModuleYNumber; i++)
		{
			for (j = 0; j < Param.m_iROIAnisoAlignMultiModuleXNumber; j++)
			{
				iMoveX = j * Param.m_iROIAnisoAlignMultiModuleXPitch;
				iMoveY = i * Param.m_iROIAnisoAlignMultiModuleYPitch;

				MoveRegion(HInspectAreaDilatedRgn, &HModuleSearchRgn, iMoveY, iMoveX);
				ReduceDomain(HImage, HModuleSearchRgn, &HMatchingImageReduced);
				CropDomain(HMatchingImageReduced, &HMatchingImageReduced);
				SmallestRectangle1(HModuleSearchRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
				lCropLTPointY = HlLTPointY.L();
				lCropLTPointX = HlLTPointX.L();

				Row = HTuple(0.0);
				Column = HTuple(0.0);
				Angle = HTuple(0.0);
				ScaleX = HTuple(0.0);
				ScaleY = HTuple(0.0);
				Score = HTuple(0.0);

				FindAnisoShapeModel(HMatchingImageReduced,
					*pHModelID,							// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,
					Param.m_dROIAnisoAlignMatchingScaleMinY,
					Param.m_dROIAnisoAlignMatchingScaleMaxY,
					Param.m_dROIAnisoAlignMatchingScaleMinX,
					Param.m_dROIAnisoAlignMatchingScaleMaxX,
					Param.m_dROIAnisoAlignMatchingScore,		// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&ScaleY,
					&ScaleX,
					&Score);									// Score of the found instances of the model.

				HTuple HlNoFoundNumber;
				TupleLength(Score, &HlNoFoundNumber);
				lNoFoundNumber = HlNoFoundNumber.L();

				if (lNoFoundNumber <= 0)
				{
					pbModuleExist[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = FALSE;

					AreaCenter(HModuleSearchRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
					dSearchCenterY = HdROICenterY.D();
					dSearchCenterX = HdROICenterX.D();

					HomMat2dIdentity(&HomMat2DIdentity);
					HomMat2dTranslate(HomMat2DIdentity, (int)dSearchCenterY, (int)dSearchCenterX, &HomMat2DTranslate);
					AffineTransContourXld(HModelContour, &HModelAffineXLD, HomMat2DTranslate);
					GenRegionContourXld(HModelAffineXLD, &HModelAffineRgn, "filled");
					ConcatObj(HAlignRgn, HModelAffineRgn, &HAlignRgn);		// 모듈이 없는 경우 불량처리로 저장 
					ConcatObj(*(m_HAlignResultRgn + iInspectBufferIndexTemp), HModelAffineRgn, m_HAlignResultRgn + iInspectBufferIndexTemp);		// 모듈이 없는 경우 불량처리로 저장 

					// 불량여부, 이미지 위치 기록
					pbModuleIsDefect[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = TRUE;
					SmallestRectangle1(HModelAffineRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX);
					piModuleDefectPosX[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = HLTPointX[0].L();
					piModuleDefectPosY[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = HLTPointY[0].L();
				}
				else
				{
					dDeltaX = Column[0].D();
					dDeltaY = Row[0].D();
					dAngleRad = Angle[0].D();
					dScore = Score[0].D();
					dScaleX = ScaleX[0].D();
					dScaleY = ScaleY[0].D();

					dDeltaX = dDeltaX + lCropLTPointX;
					dDeltaY = dDeltaY + lCropLTPointY;

					pbModuleExist[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = TRUE;
					pdDeltaX[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = dDeltaX - dROICenterX - (double)iMoveX;
					pdDeltaY[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = dDeltaY - dROICenterY - (double)iMoveY;

					TupleDeg(Angle, &Angle);
					dAngleDeg = Angle[0].D();
					pdDeltaAngle[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = dAngleDeg;

					if (Param.m_bROIAnisoAlignInspectShift)
					{
						bModulePositionError = FALSE;

						if (pdDeltaX[iNoInspectedModule] >= 0)	// Pixel
						{
							if (pdDeltaX[iNoInspectedModule] > Param.m_dROIAnisoAlignShiftRight)
								bModulePositionError = TRUE;
						}
						else
						{
							if (fabs(pdDeltaX[iNoInspectedModule]) > Param.m_dROIAnisoAlignShiftLeft)
								bModulePositionError = TRUE;
						}

						if (pdDeltaY[iNoInspectedModule] >= 0)
						{
							if (pdDeltaY[iNoInspectedModule] > Param.m_dROIAnisoAlignShiftBottom)
								bModulePositionError = TRUE;
						}
						else
						{
							if (fabs(pdDeltaY[iNoInspectedModule]) > Param.m_dROIAnisoAlignShiftTop)
								bModulePositionError = TRUE;
						}

						if (bModulePositionError)
						{
							HomMat2dIdentity(&HomMat2DIdentity);
							HomMat2dTranslate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
							HomMat2dRotate(HomMat2DTranslate, dAngleRad, dDeltaY, dDeltaX, &HomMat2DRotate);
							HomMat2dScale(HomMat2DRotate, dScaleY, dScaleX, dDeltaY, dDeltaX, &HomMat2DScale);
							AffineTransContourXld(HModelContour, &HModelAffineXLD, HomMat2DScale);
							GenRegionContourXld(HModelAffineXLD, &HModelAffineRgn, "filled");
							ConcatObj(HAlignRgn, HModelAffineRgn, &HAlignRgn);		// 모듈이 매칭된 경우에는 Shift 발생한 경우에 저장 
							ConcatObj(*(m_HAlignResultRgn + iInspectBufferIndexTemp), HModelAffineRgn, m_HAlignResultRgn + iInspectBufferIndexTemp);	// 모듈이 매칭된 경우에는 Align result rgn, Shift 발생한 경우에 저장 

							// 불량여부, 이미지 위치 기록
							pbModuleIsDefect[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = TRUE;
							SmallestRectangle1(HModelAffineRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX);
							piModuleDefectPosX[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = HLTPointX[0].L();
							piModuleDefectPosY[i*Param.m_iROIAnisoAlignMultiModuleXNumber + j] = HLTPointY[0].L();
						}
					}
				}

				++iNoInspectedModule;

				if (iNoInspectedModule >= iNoInspectModule)
				{
					bFinish = TRUE;
					break;
				}
			}

			if (bFinish)
				break;
		}

		TupleGenConst(0, 0, m_HAlignModuleExist + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);

		for (i = 0; i < iNoInspectModule; i++)
		{
			if (pbModuleExist[i])
				TupleConcat(*(m_HAlignModuleExist + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleExist + iInspectBufferIndexTemp);
			else
				TupleConcat(*(m_HAlignModuleExist + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleExist + iInspectBufferIndexTemp);

			if (pbModuleIsDefect[i])
				TupleConcat(*(m_HAlignModuleIsDefect + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
			else
				TupleConcat(*(m_HAlignModuleIsDefect + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleIsDefect + iInspectBufferIndexTemp);

			TupleConcat(*(m_HAlignModuleDefectPosX + iInspectBufferIndexTemp), HTuple(piModuleDefectPosX[i]), m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDefectPosY + iInspectBufferIndexTemp), HTuple(piModuleDefectPosY[i]), m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);

			TupleConcat(*(m_HAlignModuleDeltaX + iInspectBufferIndexTemp), HTuple(pdDeltaX[i] * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * 0.001), m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaY + iInspectBufferIndexTemp), HTuple(pdDeltaY[i] * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * 0.001), m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp), HTuple(pdDeltaAngle[i]), m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
			Union1(HAlignRgn, &HAlignRgn);

		return HAlignRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ROIAnisoAlignAlgorithm_MultiModule_Align] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		int iInspectBufferIndexTemp = iInspectBufferIndex;
		if (iInspectBufferIndexTemp == -1) iInspectBufferIndexTemp = 0;
		TupleGenConst(0, 0, m_HAlignModuleExist + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);

		for (int i = 0; i < iNoInspectModule; i++)
		{
			TupleConcat(*(m_HAlignModuleExist + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleExist + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleIsDefect + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDefectPosX + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDefectPosY + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaX + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaY + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);
		}

		HObject HAlignRgn;
		GenEmptyObj(&HAlignRgn);
		return HAlignRgn;
	}
}

HObject Algorithm::PartCheckAlgorithm_MultiModule_Align(HTuple HPartModelID, HObject HImage, HObject HROIRgn, CAlgorithmParam Param, int iNoInspectModule, int iInspectBufferIndex)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HAlignRgn;
		GenEmptyObj(&HAlignRgn);

		BOOL pbModuleExist[MAX_MODULE_TRAY];
		BOOL pbModuleIsDefect[MAX_MODULE_TRAY];
		int piModuleDefectPosX[MAX_MODULE_TRAY], piModuleDefectPosY[MAX_MODULE_TRAY];
		double pdDeltaX[MAX_MODULE_TRAY], pdDeltaY[MAX_MODULE_TRAY], pdDeltaAngle[MAX_MODULE_TRAY];

		int iInspectBufferIndexTemp = iInspectBufferIndex;
		if (iInspectBufferIndexTemp == -1) iInspectBufferIndexTemp = 0;

		m_iAlignNoInspectModule[iInspectBufferIndexTemp] = Param.m_iPartCheckMultiModuleYNumber * Param.m_iPartCheckMultiModuleXNumber;

		int i, j, k;

		for (int ij = 0; ij < Param.m_iPartCheckMultiModuleYNumber * Param.m_iPartCheckMultiModuleXNumber; ij++)
		{
			pbModuleExist[ij] = FALSE;
			pbModuleIsDefect[ij] = FALSE;
			piModuleDefectPosX[ij] = 0;
			piModuleDefectPosY[ij] = 0;
			pdDeltaX[ij] = 0;
			pdDeltaY[ij] = 0;
			pdDeltaAngle[ij] = 0;
		}

		if (HPartModelID < 0)
		{
			return HAlignRgn;
		}

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HAlignRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		// Model Ref: G-Center of Domain
		double dTeachAlignRefX, dTeachAlignRefY;
		HTuple HlArea, HdTeachAlignRefY, HdTeachAlignRefX;
		AreaCenter(HInspectAreaRgn, &HlArea, &HdTeachAlignRefY, &HdTeachAlignRefX);
		dTeachAlignRefY = HdTeachAlignRefY.D();
		dTeachAlignRefX = HdTeachAlignRefX.D();

		int iSearchMarginLeft = Param.m_iPartCheckMatchingSearchMarginX;
		if (iSearchMarginLeft < 0)
			iSearchMarginLeft = 0;
		int iSearchMarginRight = Param.m_iPartCheckMatchingSearchMarginX2;
		if (iSearchMarginRight < 0)
			iSearchMarginRight = 0;
		int iSearchMarginTop = Param.m_iPartCheckMatchingSearchMarginY;
		if (iSearchMarginTop < 0)
			iSearchMarginTop = 0;
		int iSearchMarginBottom = Param.m_iPartCheckMatchingSearchMarginY2;
		if (iSearchMarginBottom < 0)
			iSearchMarginBottom = 0;

		HObject HInspectAreaDilatedRgn, HModuleSearchRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaDilatedRgn, "rectangle1");

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

		HObject HBlobCheckRgn;
		SmallestRectangle1(HInspectAreaRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
		lLTPointY = HlLTPointY.L();
		lLTPointX = HlLTPointX.L();
		MoveRegion(HInspectAreaRgn, &HBlobCheckRgn, -lLTPointY, -lLTPointX);

		Hlong lImageWidth, lImageHeight;
		HTuple HImageWidth, HImageHeight;
		GetImageSize(HImage, &HImageWidth, &HImageHeight);
		lImageWidth = HImageWidth.L();
		lImageHeight = HImageHeight.L();

		Hlong lCropLTPointX, lCropLTPointY, lCropRBPointX, lCropRBPointY;
		HObject HMatchingImageReduced;

		HTuple dAngleRangeRad;
		TupleRad(Param.m_iPartCheckMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		Hlong lROIArea;
		double dROICenterX, dROICenterY;
		HTuple HdROICenterX, HdROICenterY, HlROIArea;

		Hlong lNoFoundNumber;
		HTuple Row, Column, Angle, Score;

		int iNoTeachNumLevel = 0;
		iNoTeachNumLevel = Param.m_iPartCheckMatchingPLevel;

		int iNoInspectedModule = 0;
		BOOL bFinish = FALSE;
		Hlong iMoveX, iMoveY;

		BOOL bModulePositionError;
		HTuple HLTPointY, HLTPointX, HRBPointY, HRBPointX;
		double dSearchCenterX, dSearchCenterY;

		double dDeltaX, dDeltaY, dAngle, dScore, dXShift, dYShift;
		HTuple HAngleDeg;
		double dAngleDeg;
		HTuple HomMat2DRotate;
		double dMatchingScoreTolerance;
		HObject HAffineTransRgn;

		dMatchingScoreTolerance = (double)Param.m_iPartCheckMatchingScore * 0.01;

		int iDirectionAxis;
		for (int ij = 0; ij < Param.m_iPartCheckMultiModuleYNumber * Param.m_iPartCheckMultiModuleXNumber; ij++)
		{
			// Tray 안착 순서에 따라서 옵션을 다르게 처리해야함
			// if (y 방향으로 먼저 진행한다면)
			{
				iDirectionAxis = Param.m_iPartCheckMultiModuleYNumber;
				i = ij % iDirectionAxis;
				j = ij / iDirectionAxis;
			}
			// else if (x 방향으로 먼저 진행한다면)
			// {
			//	iDirectionAxis = m_iPartCheckMultiModuleXNumber;
			//	i = ij / iDirectionAxis;
			//	j = ij % iDirectionAxis;
			// }

			if (Param.m_bPartCheckMultiModuleMoveOffset)
			{
				iMoveX = Param.m_iPartCheckMoveOffsetX[i][j];
				iMoveY = Param.m_iPartCheckMoveOffsetY[i][j];
			}
			else
			{
				iMoveX = j * Param.m_iPartCheckMultiModuleXPitch;
				iMoveY = i * Param.m_iPartCheckMultiModuleYPitch;
			}

			// if (이미지 기준 시작 모듈이 좌 하단 (BOI))
			{
				iMoveX = iMoveX * 1;
				iMoveY = iMoveY * -1;
			}
			// else if (이미지 기준 시작 모듈이 좌 상단)
			// {
			//	iMoveX = iMoveX * 1;
			//	iMoveY = iMoveY * 1;
			// }
			// else if (이미지 기준 시작 모듈이 우 하단)
			// {
			//	iMoveX = iMoveX * 1;
			//	iMoveY = iMoveY * 1;
			// }
			// else if (이미지 기준 시작 모듈이 우 상단)
			// {
			//	iMoveX = iMoveX * 1;
			//	iMoveY = iMoveY * 1;
			// }

			MoveRegion(HInspectAreaDilatedRgn, &HModuleSearchRgn, iMoveY, iMoveX);

			SmallestRectangle1(HModuleSearchRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
			lLTPointY = HlLTPointY.L();
			lLTPointX = HlLTPointX.L();
			lRBPointY = HlRBPointY.L();
			lRBPointX = HlRBPointX.L();

			if (Param.m_bPartCheckBlob)
			{
				int iSurfaceInspectionArea;
				double dSurfaceInspectioXLength, dSurfaceInspectionYLength;
				HObject HPartDetectRgn;
				HObject HPartInspectImage;

				CropRectangle1(HImage, &HPartInspectImage, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

				GenEmptyObj(&HPartDetectRgn);
				HPartDetectRgn = SurfaceInspectionAlgorithm(iInspectBufferIndexTemp, HPartInspectImage, HPartInspectImage, HBlobCheckRgn, Param, &iSurfaceInspectionArea, &dSurfaceInspectioXLength, &dSurfaceInspectionYLength, NULL, NULL);

				if (THEAPP.m_pGFunction->ValidHRegion(HPartDetectRgn) == FALSE)
				{
					pbModuleExist[ij] = FALSE;

					ConcatObj(HAlignRgn, HModuleSearchRgn, &HAlignRgn);		// 모듈이 없는 경우 불량처리로 저장 
					ConcatObj(*(m_HAlignResultRgn + iInspectBufferIndexTemp), HModuleSearchRgn, m_HAlignResultRgn + iInspectBufferIndexTemp);		// 모듈이 없는 경우 불량처리로 저장 

					// 불량여부, 이미지 위치 기록
					pbModuleIsDefect[ij] = TRUE;
					SmallestRectangle1(HModuleSearchRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX);
					piModuleDefectPosX[ij] = HLTPointX[0].L();
					piModuleDefectPosY[ij] = HLTPointY[0].L();

					continue;
				}
			}

			lCropLTPointX = lLTPointX - iSearchMarginLeft;
			if (lCropLTPointX < 0)
				lCropLTPointX = 0;
			lCropLTPointY = lLTPointY - iSearchMarginTop;
			if (lCropLTPointY < 0)
				lCropLTPointY = 0;
			lCropRBPointX = lRBPointX + iSearchMarginRight;
			if (lCropRBPointX >= lImageWidth)
				lCropRBPointX = lImageWidth - 1;
			lCropRBPointY = lRBPointY + iSearchMarginBottom;
			if (lCropRBPointY >= lImageHeight)
				lCropRBPointY = lImageHeight - 1;

			CropRectangle1(HImage, &HMatchingImageReduced, lCropLTPointY, lCropLTPointX, lCropRBPointY, lCropRBPointX);

			if (bDebugSave)
			{
				WriteImage(HMatchingImageReduced, "bmp", 0, "c:\\DualTest\\CropInspectImage");
			}

			Row = HTuple(0.0);
			Column = HTuple(0.0);
			Angle = HTuple(0.0);
			Score = HTuple(0.0);

			if (Param.m_bPartCheckMultipleModel)
			{
				FindNccModel(HMatchingImageReduced,
					HPartModelID[ij],	// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad * 2.0,							// Extent of the rotation angles.
					dMatchingScoreTolerance,					// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"true",										// Subpixel accuracy
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&Score);									// Score of the found instances of the model.
			}
			else
			{
				FindNccModel(HMatchingImageReduced,
					HPartModelID,								// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad * 2.0,							// Extent of the rotation angles.
					dMatchingScoreTolerance,					// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"true",										// Subpixel accuracy
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&Score);									// Score of the found instances of the model.
			}

			HTuple HlNoFoundNumber;
			TupleLength(Score, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			if (lNoFoundNumber <= 0)		// Empty
			{
				if (Param.m_bPartCheckBlob == FALSE)
				{
					pbModuleExist[ij] = FALSE;

					ConcatObj(HAlignRgn, HModuleSearchRgn, &HAlignRgn);		// 모듈이 없는 경우 불량처리로 저장 
					ConcatObj(*(m_HAlignResultRgn + iInspectBufferIndexTemp), HModuleSearchRgn, m_HAlignResultRgn + iInspectBufferIndexTemp);		// 모듈이 없는 경우 불량처리로 저장 

					// 불량여부, 이미지 위치 기록
					pbModuleIsDefect[ij] = TRUE;
					SmallestRectangle1(HModuleSearchRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX);
					piModuleDefectPosX[ij] = HLTPointX[0].L();
					piModuleDefectPosY[ij] = HLTPointY[0].L();
				}
				else
				{
					dDeltaX = 0;
					dDeltaY = 0;
					dAngle = 0;
					dScore = 0;

					dDeltaX = dDeltaX + lCropLTPointX;
					dDeltaY = dDeltaY + lCropLTPointY;

					pdDeltaX[ij] = dDeltaX - (dTeachAlignRefX + (double)iMoveX);
					pdDeltaY[ij] = dDeltaY - (dTeachAlignRefY + (double)iMoveY);
					dAngleDeg = 0;
					pdDeltaAngle[ij] = dAngleDeg;

					pbModuleExist[ij] = TRUE;

					bModulePositionError = TRUE;

					if (bModulePositionError)
					{
						HalconCpp::GenEmptyObj(&HAffineTransRgn);
						VectorAngleToRigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);
						AffineTransRegion(HInspectAreaRgn, &HAffineTransRgn, HomMat2DRotate, "nearest_neighbor");

						ConcatObj(HAlignRgn, HAffineTransRgn, &HAlignRgn);		// 모듈이 매칭된 경우에는 Shift 발생한 경우에 저장 
						ConcatObj(m_HAlignResultRgn_EachImage[0], HAffineTransRgn, &(m_HAlignResultRgn_EachImage[0]));	// 모듈이 매칭된 경우에는 Align result rgn, Shift 발생한 경우에 저장 

						// 불량여부, 이미지 위치 기록
						pbModuleIsDefect[ij] = TRUE;
						SmallestRectangle1(HAffineTransRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX);
						piModuleDefectPosX[ij] = HLTPointX[0].L();
						piModuleDefectPosY[ij] = HLTPointY[0].L();
					}
				}
			}
			else
			{
				dDeltaX = Column[0].D();
				dDeltaY = Row[0].D();
				dAngle = Angle[0].D();
				dScore = Score[0].D();

				dDeltaX = dDeltaX + lCropLTPointX;
				dDeltaY = dDeltaY + lCropLTPointY;

				pdDeltaX[ij] = dDeltaX - (dTeachAlignRefX + (double)iMoveX);
				pdDeltaY[ij] = dDeltaY - (dTeachAlignRefY + (double)iMoveY);
				TupleDeg(Angle, &HAngleDeg);
				dAngleDeg = HAngleDeg[0].D();
				pdDeltaAngle[ij] = dAngleDeg;

				pbModuleExist[ij] = TRUE;

				bModulePositionError = FALSE;
				if (fabs(pdDeltaX[ij]) > (double)Param.m_iPartCheckShiftX ||
					fabs(pdDeltaY[ij]) > (double)Param.m_iPartCheckShiftY ||
					fabs(dAngleDeg) > Param.m_dPartCheckRotationAngle)
					bModulePositionError = TRUE;

				if (bModulePositionError)
				{
					HalconCpp::GenEmptyObj(&HAffineTransRgn);
					VectorAngleToRigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);
					AffineTransRegion(HInspectAreaRgn, &HAffineTransRgn, HomMat2DRotate, "nearest_neighbor");

					ConcatObj(HAlignRgn, HAffineTransRgn, &HAlignRgn);		// 모듈이 매칭된 경우에는 Shift 발생한 경우에 저장 
					ConcatObj(m_HAlignResultRgn_EachImage[0], HAffineTransRgn, &(m_HAlignResultRgn_EachImage[0]));	// 모듈이 매칭된 경우에는 Align result rgn, Shift 발생한 경우에 저장 

					// 불량여부, 이미지 위치 기록
					pbModuleIsDefect[ij] = TRUE;
					SmallestRectangle1(HAffineTransRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX);
					piModuleDefectPosX[ij] = HLTPointX[0].L();
					piModuleDefectPosY[ij] = HLTPointY[0].L();
				}
			}
		}

		TupleGenConst(0, 0, m_HAlignModuleExist + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
		TupleGenConst(0, 0, m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);

		BOOL bRawImageSaveFlag = FALSE;

		for (i = 0; i < Param.m_iPartCheckMultiModuleYNumber * Param.m_iPartCheckMultiModuleXNumber; i++)
		{
			if (pbModuleExist[i])
				TupleConcat(*(m_HAlignModuleExist + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleExist + iInspectBufferIndexTemp);
			else
				TupleConcat(*(m_HAlignModuleExist + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleExist + iInspectBufferIndexTemp);

			if (pbModuleIsDefect[i])
				TupleConcat(*(m_HAlignModuleIsDefect + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
			else
				TupleConcat(*(m_HAlignModuleIsDefect + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleIsDefect + iInspectBufferIndexTemp);

			TupleConcat(*(m_HAlignModuleDefectPosX + iInspectBufferIndexTemp), HTuple(piModuleDefectPosX[i]), m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDefectPosY + iInspectBufferIndexTemp), HTuple(piModuleDefectPosY[i]), m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);

			TupleConcat(*(m_HAlignModuleDeltaX + iInspectBufferIndexTemp), HTuple(pdDeltaX[i] * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * 0.001), m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaY + iInspectBufferIndexTemp), HTuple(pdDeltaY[i] * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * 0.001), m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp), HTuple(pdDeltaAngle[i]), m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);
		}

#ifdef INLINE_MODE

		if (THEAPP.m_iModeSwitch != MODE_ADMIN_TEACH_VIEW)
		{
			if (bRawImageSaveFlag && iInspectBufferIndex >= 0)
			{
				CString sImageSaveFolderName = THEAPP.Struct_PreferenceStruct.m_strEtcFolderPath + "CMI_Results\\LoadTrayAlign";
				CString sImageFilename, FolderSub;

				SYSTEMTIME time;
				GetLocalTime(&time);

				FolderSub.Format("\\%04d%02d%02d", time.wYear, time.wMonth, time.wDay);
				THEAPP.m_FileBase.CreateFolder(sImageSaveFolderName + FolderSub);

				sImageFilename.Format("%s%s\\LoadTrayAlign_%04d%02d%02d_%02d%02d%02d", sImageSaveFolderName, FolderSub,
					time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

				try
				{
					WriteImage(HImage, "jpg", 0, HTuple(sImageFilename));
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

					strLog.Format("Halcon Exception [Loading Align NG Image Save] : <%s>%s", sOperatorName, sErrMsg);
					THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
				}
			}
		}

#endif

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
			Union1(HAlignRgn, &HAlignRgn);

		return HAlignRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::PartCheckAlgorithm_MultiModule_Align] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		int iInspectBufferIndexTemp = iInspectBufferIndex;
		if (iInspectBufferIndexTemp == -1) iInspectBufferIndexTemp = 0;

		for (int i = 0; i < iNoInspectModule; i++)
		{
			TupleConcat(*(m_HAlignModuleExist + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleExist + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleIsDefect + iInspectBufferIndexTemp), HTuple(1), m_HAlignModuleIsDefect + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDefectPosX + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDefectPosX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDefectPosY + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDefectPosY + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaX + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDeltaX + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaY + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDeltaY + iInspectBufferIndexTemp);
			TupleConcat(*(m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp), HTuple(0), m_HAlignModuleDeltaAngle + iInspectBufferIndexTemp);
		}

		HObject HAlignRgn;
		GenEmptyObj(&HAlignRgn);
		return HAlignRgn;
	}
}

HObject Algorithm::StiffenerEpoxyInspectROIAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param)
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HROIRgn, "c:\\DualTest\\HROIRgn.reg");
		}

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		//////////////////////////////////////////////////////////////////////////
		// Parameters
		int iThresholdParam_StiffenerRegion = Param.m_iThresholdParam_StiffenerRegion;						// Stiffener Outer boundary 획득을 위한 threshold
		double dErosionParam_StiffenerBoundary = Param.m_dErosionParam_StiffenerBoundary;					// Stiffener Inner boundary 획득을 위한 erosion parameter
		int iThresholdParam_SheildcanRegion = Param.m_iThresholdParam_SheildcanRegion;						// ShieldCan Inner region 획득을 위한 threshold
		double dDilationParam_SheildcanBoundary = Param.m_dDilationParam_SheildcanBoundary;					// ShieldCan boundary 획득을 위한 dilation parameter
		double dOpeningParam_CornerBoundary = Param.m_dOpeningParam_CornerBoundary;							// Corner boundary 오차를 줄이기 위한 opening parameter
		//////////////////////////////////////////////////////////////////////////

		HTuple	Trash;
		HTuple	Width, Height;
		GetImageSize(HImage, &Width, &Height);																	// 

		//FillUp을 위한 우측 기준선 자동 설정
		HObject Region, RegionFillUp, ConnectedRegions, ObjectSelected, Contours;
		HTuple Area, AreaMax, Indices, Column, ColumnMax;
		Threshold(HImage, &Region, 250, 255);																// SheildCan 우측(상단에서 봤을 때) 기준 선 찾기위한 threshold
		FillUp(Region, &RegionFillUp);																		// ''
		Connection(RegionFillUp, &ConnectedRegions);														// ''
		AreaCenter(ConnectedRegions, &Area, &Trash, &Trash);												// ''
		TupleMax(Area, &AreaMax);																			// ''
		TupleFind(Area, AreaMax, &Indices);																	// ''
		SelectObj(ConnectedRegions, &ObjectSelected, Indices + 1);											// ''
		GenContourRegionXld(ObjectSelected, &Contours, "border");											// ''
		GetContourXld(Contours, &Trash, &Column);															// ''
		TupleMax(Column, &ColumnMax);																		// FillUp을 위한 SheildCan 우측 기준 선

		HObject HRectangle, HImagePainted;
		GenRectangle1(&HRectangle, 0, ColumnMax - dDilationParam_SheildcanBoundary, Height, Width);			// Region을 닫아서 FillUp하기 위한 rectangle 생성
		PaintRegion(HRectangle, HImage, &HImagePainted, 255, "fill");

		HObject	HFixedThRegion, HRegion1, HRegion1Closing, HRegion1FillUp, HConnectedRegion1;
		HObject	HObjectSelected1, HRegion1Erosion, HRegion1Moved, HRegion1Union;
		HTuple	Area1, Area1Max, Indices1;

		Threshold(HImagePainted, &HFixedThRegion, iThresholdParam_StiffenerRegion, 255);										// Stiffener region 생성
		/// Dynamic Threshold 추가
		HObject HMeanImage, HDynThreshRgn;
		MeanImage(HImagePainted, &HMeanImage, Param.m_iDTFilterX_StiffenerRegion, Param.m_iDTFilterY_StiffenerRegion);
		DynThreshold(HImagePainted, HMeanImage, &HDynThreshRgn, Param.m_iDTValue_StiffenerRegion, "light");
		Union2(HFixedThRegion, HDynThreshRgn, &HRegion1);																								//

		ClosingCircle(HRegion1, &HRegion1Closing, (double)Param.m_iClosing_StiffenerRegion + 0.5);				// ''
		FillUp(HRegion1Closing, &HRegion1FillUp);																// ''
		Connection(HRegion1FillUp, &HConnectedRegion1);															// ''
		AreaCenter(HConnectedRegion1, &Area1, &Trash, &Trash);													// ''
		TupleMax(Area1, &Area1Max);																				// ''
		TupleFind(Area1, Area1Max, &Indices1);																	// ''
		SelectObj(HConnectedRegion1, &HObjectSelected1, Indices1 + 1);											// ''
		ErosionCircle(HObjectSelected1, &HRegion1Erosion, dErosionParam_StiffenerBoundary);						// Stiffener inner boundary까지 erosion
		MoveRegion(HRegion1Erosion, &HRegion1Moved, 0, 0);														// Stiffener 각 side의 두께 차이로 발생하는 오차를 줄이기 위한 region move (일단 생략)
		Union2(HRectangle, HRegion1Moved, &HRegion1Union);														// 

		HObject	HImageReduced, HRegion2, HConnectedRegion2, HObjectSelected2;
		HObject	HRegion2FillUp, HRegion2Dilation;
		HTuple	Area2, Area2Max, Indices2;

		Threshold(HImage, &HRegion2, 0, iThresholdParam_SheildcanRegion);								// SheildCan region 생성
		Connection(HRegion2, &HConnectedRegion2);																// ''
		SelectShape(HConnectedRegion2, &HConnectedRegion2, "column2", "and", 10, Width - 10);					// ''
		AreaCenter(HConnectedRegion2, &Area2, &Trash, &Trash);													// ''
		TupleMax(Area2, &Area2Max);																				// ''
		TupleFind(Area2, Area2Max, &Indices2);																	// ''
		SelectObj(HConnectedRegion2, &HObjectSelected2, Indices2 + 1);											// ''
		FillUp(HObjectSelected2, &HRegion2FillUp);																// ''
		DilationCircle(HRegion2FillUp, &HRegion2Dilation, dDilationParam_SheildcanBoundary);					// SheildCan outer boundary까지 dilation

		HObject HRegion2Rect, HRegion2Opening, HRegion2Moved, HRegion2Union;
		HTuple	Row, Phi, Length1, Length2;
		SmallestRectangle2(HRegion2Dilation, &Row, &Column, &Phi, &Length1, &Length2);							// SheildCan corner boundary 오차를 줄이한 opening
		GenRectangle2(&HRegion2Rect, Row, Column, Phi, Length1, Length2);										// ''
		OpeningCircle(HRegion2Rect, &HRegion2Opening, dOpeningParam_CornerBoundary);							// ''
		MoveRegion(HRegion2Opening, &HRegion2Moved, 0, 0);														// SheildCan 각 side의 두께 차이로 발생하는 오차를 줄이기 위한 region move (일단 생략)
		Union2(HRegion2Moved, HRectangle, &HRegion2Union);														// 

		HObject	HRegionDifference;

		Difference(HRegion1Union, HRegion2Union, &HRegionDifference);											// 전체 Epoxy 검사 region 생성
		Intersection(HRegionDifference, HInspectAreaRgn, &HDefectRgn);											// 선택한 영역 내 epoxy 검사 ROI 추출

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::StiffenerEpoxyInspectROIAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::InspectAVI(int iInspectBufferIndex, int iThreadIdx, GTRegion *pInspectROIRgn, int iInspectTabIdx, HObject HImage, HObject HROIRgn, CAlgorithmParam AlgorithmParam, HObject *pHResultXLD, HObject *pHInspectImage, HObject HDCImage, HObject *pHMaskRgn, double *pdResultDataAry, int iMatchingPosOffsetX, int iMatchingPosOffsetY)
{
	try
	{
		int iMzNo, iTrayNo, iModuleNo;

		if (iInspectBufferIndex < 0)
		{
			iMzNo = 1;
			iTrayNo = 1;
			iModuleNo = 1;
		}
		else
		{
			iMzNo = m_nMzNo[iInspectBufferIndex];
			iTrayNo = m_nTrayNo[iInspectBufferIndex];
			iModuleNo = m_nModuleNo[iInspectBufferIndex];
		}

		CMeasureData *pMeasureData;
		int iInspectBufferIndexTemp = iInspectBufferIndex;
		if (iInspectBufferIndexTemp == -1) iInspectBufferIndexTemp = 0;

		pMeasureData = &(pInspectROIRgn->m_MeasureData[iInspectBufferIndexTemp][iInspectTabIdx]);
		pMeasureData->Reset();

		HObject HDefectAllRgn;
		GenEmptyObj(&HDefectAllRgn);

		HObject HSurfaceAlgoDefectRgn;
		HObject HBenvolioAttachRemoveRgn;
		HObject HEpoxyVoidHoleInspectRgn;
		HObject HStiffenerEpoxyInspectRgn;

		int iSurfaceInspectionArea;
		double dSurfaceInspectioXLength, dSurfaceInspectionYLength;
		//VER 1.1.3.2 add
		double dSurfaceInspectionLength = 0;
		//VER 1.1.3.2 end

		iSurfaceInspectionArea = 0;
		dSurfaceInspectioXLength = dSurfaceInspectionYLength = 0;

		if (AlgorithmParam.m_bUseSurfaceInspection)
		{
			if (AlgorithmParam.m_bUseEpoxyVoidHole)
			{
				BOOL bBenvolioHtccTiltDefect = FALSE;

				HEpoxyVoidHoleInspectRgn = EpoxyVoidHoleAlgorithm(HImage, HROIRgn, AlgorithmParam, &(pHInspectImage[AlgorithmParam.m_iEpoxyVoidHoleImage]), &bBenvolioHtccTiltDefect);

				if (THEAPP.m_pGFunction->ValidHRegion(HEpoxyVoidHoleInspectRgn) == TRUE)
				{
					if (bBenvolioHtccTiltDefect)		// Defect 처리
						Union1(HEpoxyVoidHoleInspectRgn, &HSurfaceAlgoDefectRgn);
					else
						HSurfaceAlgoDefectRgn = SurfaceInspectionAlgorithm(iInspectBufferIndexTemp, HImage, HDCImage, HEpoxyVoidHoleInspectRgn, AlgorithmParam, &iSurfaceInspectionArea, &dSurfaceInspectioXLength, &dSurfaceInspectionYLength, pHMaskRgn, NULL);
				}
			}
			else if (AlgorithmParam.m_bUseStiffenerEpoxy)
			{
				HStiffenerEpoxyInspectRgn = StiffenerEpoxyInspectROIAlgorithm(HImage, HROIRgn, AlgorithmParam);

				if (THEAPP.m_pGFunction->ValidHRegion(HStiffenerEpoxyInspectRgn) == TRUE)
				{
					HSurfaceAlgoDefectRgn = SurfaceInspectionAlgorithm(iInspectBufferIndexTemp, HImage, HDCImage, HStiffenerEpoxyInspectRgn, AlgorithmParam, &iSurfaceInspectionArea, &dSurfaceInspectioXLength, &dSurfaceInspectionYLength, pHMaskRgn, NULL);
				}
			}
			else
			{
				if (AlgorithmParam.m_bUseSurfaceInspectEachROI)
				{
					Hlong lNoEachROI;
					HObject HROIConnRgn, HEachROIRgn, HEachROIDefectRgn;
					int iNoEachROIDefect = 0;

					GenEmptyObj(&HSurfaceAlgoDefectRgn);

					Connection(HROIRgn, &HROIConnRgn);

					HTuple HlNoEachROI;
					CountObj(HROIConnRgn, &HlNoEachROI);
					lNoEachROI = HlNoEachROI.L();

					for (int iROI = 0; iROI < lNoEachROI; iROI++)
					{
						SelectObj(HROIConnRgn, &HEachROIRgn, iROI + 1);

						if (THEAPP.m_pGFunction->ValidHRegion(HEachROIRgn) == FALSE)
							continue;

						HEachROIDefectRgn = SurfaceInspectionAlgorithm(iInspectBufferIndexTemp, HImage, HDCImage, HEachROIRgn, AlgorithmParam, &iSurfaceInspectionArea, &dSurfaceInspectioXLength, &dSurfaceInspectionYLength, pHMaskRgn, NULL);

						if (THEAPP.m_pGFunction->ValidHRegion(HEachROIDefectRgn) == TRUE)		// If defect exists...
						{
							++iNoEachROIDefect;
							ConcatObj(HSurfaceAlgoDefectRgn, HEachROIDefectRgn, &HSurfaceAlgoDefectRgn);
						}
					}

					if (iNoEachROIDefect >= AlgorithmParam.m_iSurfaceInspectEachROINgNumber)
					{
						if (THEAPP.m_pGFunction->ValidHRegion(HSurfaceAlgoDefectRgn) == TRUE)
							Union1(HSurfaceAlgoDefectRgn, &HSurfaceAlgoDefectRgn);
					}
					else
						GenEmptyObj(&HSurfaceAlgoDefectRgn);
				}
				else
				{
					double dBlobCenterX, dBlobCenterY, dBlobCenterX2, dBlobCenterY2;
					dBlobCenterX = dBlobCenterY = dBlobCenterX2 = dBlobCenterY2 = -1;

					if (AlgorithmParam.m_bUseBarcodeBlob || AlgorithmParam.m_bUseBlobCornerMeasure)
					{
						HSurfaceAlgoDefectRgn = SurfaceInspectionAlgorithm(iInspectBufferIndexTemp, HImage, HDCImage, HROIRgn, AlgorithmParam, &iSurfaceInspectionArea, &dSurfaceInspectioXLength, &dSurfaceInspectionYLength, pHMaskRgn, pdResultDataAry, &dSurfaceInspectionLength, &dBlobCenterX, &dBlobCenterY, &dBlobCenterX2, &dBlobCenterY2);

						pMeasureData->m_dEdgeLineStartX = dBlobCenterX;
						pMeasureData->m_dEdgeLineStartY = dBlobCenterY;
						pMeasureData->m_dEdgeLineEndX = dBlobCenterX2;
						pMeasureData->m_dEdgeLineEndY = dBlobCenterY2;
					}
					else
					{
						if (!AlgorithmParam.m_bPartCheckBlob)
							HSurfaceAlgoDefectRgn = SurfaceInspectionAlgorithm(iInspectBufferIndexTemp, HImage, HDCImage, HROIRgn, AlgorithmParam, &iSurfaceInspectionArea, &dSurfaceInspectioXLength, &dSurfaceInspectionYLength, pHMaskRgn, pdResultDataAry, &dSurfaceInspectionLength, &dBlobCenterX, &dBlobCenterY);

						pMeasureData->m_dEdgeLineStartX = dBlobCenterX;
						pMeasureData->m_dEdgeLineStartY = dBlobCenterY;
						pMeasureData->m_dEdgeLineEndX = dBlobCenterX;
						pMeasureData->m_dEdgeLineEndY = dBlobCenterY;
					}
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HSurfaceAlgoDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HSurfaceAlgoDefectRgn, &HDefectAllRgn);

			pMeasureData->m_iSurfaceInspectionArea = iSurfaceInspectionArea;
			pMeasureData->m_dSurfaceInspectionXLength = dSurfaceInspectioXLength;
			pMeasureData->m_dSurfaceInspectionYLength = dSurfaceInspectionYLength;
		}

		HObject HSurfaceDualAlgoDefectRgn;

		if (AlgorithmParam.m_bUseSurfaceDualInspection)
		{
			HSurfaceDualAlgoDefectRgn = SurfaceDualInspectionAlgorithm(&(pHInspectImage[AlgorithmParam.m_iSurfaceDualImageIndex_1]), &(pHInspectImage[AlgorithmParam.m_iSurfaceDualImageIndex_2]), HDCImage, HROIRgn, AlgorithmParam, pHMaskRgn);

			if (THEAPP.m_pGFunction->ValidHRegion(HSurfaceDualAlgoDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HSurfaceDualAlgoDefectRgn, &HDefectAllRgn);
		}

		HObject HImageCompareAlgoDefectRgn;
		HObject HComponentDefectRgn;

		if (AlgorithmParam.m_bUseComponent)
		{
			HComponentDefectRgn = ComponentAlgorithm(iThreadIdx, iMzNo, pInspectROIRgn, iInspectTabIdx, HImage, HROIRgn, AlgorithmParam, &(pMeasureData->m_iComponentDefectType), pHMaskRgn);

			if (THEAPP.m_pGFunction->ValidHRegion(HComponentDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HComponentDefectRgn, &HDefectAllRgn);
		}
		else
		{
			if (AlgorithmParam.m_bUseImageCompare)
			{
				HObject HVariationModelInspectImage;
				GenEmptyObj(&HVariationModelInspectImage);

				HImageCompareAlgoDefectRgn = ImageCompareAlgorithm(iThreadIdx, iMzNo, pInspectROIRgn, iInspectTabIdx, HImage, HROIRgn, AlgorithmParam, &iSurfaceInspectionArea, &dSurfaceInspectioXLength, &dSurfaceInspectionYLength, pHMaskRgn, &HVariationModelInspectImage);

				if (THEAPP.m_pGFunction->ValidHRegion(HImageCompareAlgoDefectRgn) == TRUE)
				{
					ConcatObj(HDefectAllRgn, HImageCompareAlgoDefectRgn, &HDefectAllRgn);

					if (THEAPP.Struct_PreferenceStruct.m_bUseSaveDefectShapeFeature)
					{
						if (THEAPP.m_pGFunction->ValidHImage(HVariationModelInspectImage) == TRUE)
							CopyImage(HVariationModelInspectImage, &(pInspectROIRgn->m_HVariationInspectImage[iInspectBufferIndexTemp]));
					}
				}

				pMeasureData->m_iSurfaceInspectionArea = iSurfaceInspectionArea;
				pMeasureData->m_dSurfaceInspectionXLength = dSurfaceInspectioXLength;
				pMeasureData->m_dSurfaceInspectionYLength = dSurfaceInspectionYLength;
			}
		}

		HObject HFitAlgoDefectRgn;
		double dBurMaxLengthPxl, dChippingMaxLengthPxl;
		int iBurArea, iChippingArea;
		int iLineFitAlignPosX, iLineFitAlignPosY;

		iLineFitAlignPosX = iLineFitAlignPosY = INVALID_ALIGN_RESULT;

		if (AlgorithmParam.m_bUseLineFitting)
		{
			if (AlgorithmParam.m_bLineFitUseOtherImageFitting)
				HFitAlgoDefectRgn = LineFittingAlgorithm(HImage, &(pHInspectImage[AlgorithmParam.m_iLineFitOtherImageNumber - 1]), HROIRgn, AlgorithmParam, pHResultXLD, &dBurMaxLengthPxl, &dChippingMaxLengthPxl, &iBurArea, &iChippingArea, &iLineFitAlignPosX, &iLineFitAlignPosY);
			else
				HFitAlgoDefectRgn = LineFittingAlgorithm(HImage, NULL, HROIRgn, AlgorithmParam, pHResultXLD, &dBurMaxLengthPxl, &dChippingMaxLengthPxl, &iBurArea, &iChippingArea, &iLineFitAlignPosX, &iLineFitAlignPosY);

			if (AlgorithmParam.m_bLineFitLocalAlignUse)
			{
				pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] = iLineFitAlignPosX;
				pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] = iLineFitAlignPosY;
				pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx] = NOT_ANGLE_LINE;
				pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx] = NOT_ANGLE_LINE;
			}

			pMeasureData->m_dLineFittingBurLength = dBurMaxLengthPxl;
			pMeasureData->m_dLineFittingChippingLength = dChippingMaxLengthPxl;
			pMeasureData->m_iLineFittingBurArea = iBurArea;
			pMeasureData->m_iLineFittingChippingArea = iChippingArea;

			if (THEAPP.m_pGFunction->ValidHRegion(HFitAlgoDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HFitAlgoDefectRgn, &HDefectAllRgn);
		}

		HObject HEdgeMeasureAlgoDefectRgn;
		int iEdgeMeasureAlignPosX, iEdgeMeasureAlignPosY;
		double dEdgeCenterX, dEdgeCenterY;
		double dEdgeLineStartX, dEdgeLineStartY, dEdgeLineEndX, dEdgeLineEndY;

		iEdgeMeasureAlignPosX = iEdgeMeasureAlignPosY = INVALID_ALIGN_RESULT;
		dEdgeLineStartX = dEdgeLineStartY = dEdgeLineEndX = dEdgeLineEndY = INVALID_ALIGN_RESULT;

		if (AlgorithmParam.m_bUseEdgeMeasure)
		{
			dEdgeCenterX = dEdgeCenterY = -1;

			if (AlgorithmParam.m_bEdgeMeasureUseOther && AlgorithmParam.m_bUseImageProcessFilter)
			{
				HObject HPreProcessImage;
				GetPreprocessImage(TRUE, pHInspectImage[AlgorithmParam.m_iEdgeMeasureImageIndexOther], pHInspectImage, HROIRgn, AlgorithmParam, &HPreProcessImage);
				HEdgeMeasureAlgoDefectRgn = EdgeMeasureAlgorithm(HImage, &HPreProcessImage, HROIRgn, AlgorithmParam, pHResultXLD, &iEdgeMeasureAlignPosX, &iEdgeMeasureAlignPosY, &dEdgeCenterX, &dEdgeCenterY, &dEdgeLineStartX, &dEdgeLineStartY, &dEdgeLineEndX, &dEdgeLineEndY);
			}
			else
				HEdgeMeasureAlgoDefectRgn = EdgeMeasureAlgorithm(HImage, &(pHInspectImage[AlgorithmParam.m_iEdgeMeasureImageIndexOther]), HROIRgn, AlgorithmParam, pHResultXLD, &iEdgeMeasureAlignPosX, &iEdgeMeasureAlignPosY, &dEdgeCenterX, &dEdgeCenterY, &dEdgeLineStartX, &dEdgeLineStartY, &dEdgeLineEndX, &dEdgeLineEndY);

			if (AlgorithmParam.m_bEdgeMeasureLocalAlignUse)
			{
				if (AlgorithmParam.m_iEdgeMeasureOnePoint == 1)		// Line
				{
					pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] = dEdgeLineStartX;
					pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] = dEdgeLineStartY;
					pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx] = dEdgeLineEndX;
					pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx] = dEdgeLineEndY;
				}
				else
				{
					pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] = iEdgeMeasureAlignPosX;
					pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] = iEdgeMeasureAlignPosY;
					pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx] = NOT_ANGLE_LINE;
					pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx] = NOT_ANGLE_LINE;
				}
			}

			if (AlgorithmParam.m_iEdgeMeasureOnePoint == 1)		// Line
			{
				if (dEdgeLineStartX == INVALID_ALIGN_RESULT)	// Fail
				{
					pMeasureData->m_dEdgeLineStartX = -1;
					pMeasureData->m_dEdgeLineStartY = -1;
					pMeasureData->m_dEdgeLineEndX = -1;
					pMeasureData->m_dEdgeLineEndY = -1;
				}
				else
				{
					pMeasureData->m_dEdgeLineStartX = dEdgeLineStartX;
					pMeasureData->m_dEdgeLineStartY = dEdgeLineStartY;
					pMeasureData->m_dEdgeLineEndX = dEdgeLineEndX;
					pMeasureData->m_dEdgeLineEndY = dEdgeLineEndY;
				}
			}
			else   // Point
			{
				if (dEdgeCenterX == -1)	// Fail
				{
					pMeasureData->m_dEdgeLineStartX = -1;
					pMeasureData->m_dEdgeLineStartY = -1;
					pMeasureData->m_dEdgeLineEndX = -1;
					pMeasureData->m_dEdgeLineEndY = -1;
				}
				else
				{
					if (AlgorithmParam.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						pMeasureData->m_dEdgeLineStartX = dEdgeCenterX - ONE_POINT_EDGE_EXTEND_PXL;
						pMeasureData->m_dEdgeLineStartY = dEdgeCenterY;
						pMeasureData->m_dEdgeLineEndX = dEdgeCenterX + ONE_POINT_EDGE_EXTEND_PXL;
						pMeasureData->m_dEdgeLineEndY = dEdgeCenterY;
					}
					else
					{
						pMeasureData->m_dEdgeLineStartX = dEdgeCenterX;
						pMeasureData->m_dEdgeLineStartY = dEdgeCenterY - ONE_POINT_EDGE_EXTEND_PXL;
						pMeasureData->m_dEdgeLineEndX = dEdgeCenterX;
						pMeasureData->m_dEdgeLineEndY = dEdgeCenterY + ONE_POINT_EDGE_EXTEND_PXL;
					}
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HEdgeMeasureAlgoDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HEdgeMeasureAlgoDefectRgn, &HDefectAllRgn);
		}

		///////////////////////

		HObject HDistanceMeasureAlgoDefectRgn;
		double dDistanceMeasureValueUm;
		HObject HGapLineResultXLD;
		int iGapUpperPointX, iGapUpperPointY, iGapLowerPointX, iGapLowerPointY;

		if (AlgorithmParam.m_bUseDistanceMeasure)
		{
			GenEmptyObj(&HGapLineResultXLD);

			iGapUpperPointX = iGapUpperPointY = iGapLowerPointX = iGapLowerPointY = 0;

			HDistanceMeasureAlgoDefectRgn = DistanceMeasureAlgorithm(HImage, HROIRgn, AlgorithmParam, &HGapLineResultXLD, &dDistanceMeasureValueUm, &iGapUpperPointX, &iGapUpperPointY, &iGapLowerPointX, &iGapLowerPointY);

			if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
			{
				if (AlgorithmParam.m_iDistanceMeasureUsageType == 0 && dDistanceMeasureValueUm >= 0)
				{
					TupleConcat(m_HGapDistanceMeasureResult, HTuple(dDistanceMeasureValueUm), &m_HGapDistanceMeasureResult);
					ConcatObj(m_HGapMeasureXLD, HGapLineResultXLD, &m_HGapMeasureXLD);
				}
			}
			else
			{
				if (AlgorithmParam.m_iDistanceMeasureUsageType == 0)
				{
					if (AlgorithmParam.m_iDistanceMeasureSaveID >= 1 && AlgorithmParam.m_iDistanceMeasureSaveID <= DISTANCE_MEASURE_POINT_NUMBER)
					{
						if (dDistanceMeasureValueUm >= 0)
						{
							THEAPP.m_StructDistanceMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bMeasureDone[AlgorithmParam.m_iDistanceMeasureSaveID - 1] = TRUE;
							THEAPP.m_StructDistanceMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iSaveID[AlgorithmParam.m_iDistanceMeasureSaveID - 1] = AlgorithmParam.m_iDistanceMeasureSaveID;
							THEAPP.m_StructDistanceMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dDistanceMeasureValueUm[AlgorithmParam.m_iDistanceMeasureSaveID - 1] = dDistanceMeasureValueUm;
							THEAPP.m_StructDistanceMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iDistanceUpperImagePoint[AlgorithmParam.m_iDistanceMeasureSaveID - 1].x = iGapUpperPointX;
							THEAPP.m_StructDistanceMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iDistanceUpperImagePoint[AlgorithmParam.m_iDistanceMeasureSaveID - 1].y = iGapUpperPointY;
							THEAPP.m_StructDistanceMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iDistanceLowerImagePoint[AlgorithmParam.m_iDistanceMeasureSaveID - 1].x = iGapLowerPointX;
							THEAPP.m_StructDistanceMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iDistanceLowerImagePoint[AlgorithmParam.m_iDistanceMeasureSaveID - 1].y = iGapLowerPointY;
						}
					}
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDistanceMeasureAlgoDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HDistanceMeasureAlgoDefectRgn, &HDefectAllRgn);
		}

		double dGapMeasureValueUm;

		if (AlgorithmParam.m_bUseGapMeasure)
		{
			GenEmptyObj(&HGapLineResultXLD);

			dGapMeasureValueUm = -1;
			iGapUpperPointX = iGapUpperPointY = iGapLowerPointX = iGapLowerPointY = 0;

			if (AlgorithmParam.m_bUseImageProcessFilter)
			{
				HObject HPreProcessImage1, HPreProcessImage2, HPreProcessImage3;
				GetPreprocessImage(TRUE, pHInspectImage[AlgorithmParam.m_iGapMeasureImageNo1], pHInspectImage, HROIRgn, AlgorithmParam, &HPreProcessImage1);
				GetPreprocessImage(TRUE, pHInspectImage[AlgorithmParam.m_iGapMeasureImageNo2], pHInspectImage, HROIRgn, AlgorithmParam, &HPreProcessImage2);
				if (AlgorithmParam.m_bGapMeasureDiffImage)
					GetPreprocessImage(TRUE, pHInspectImage[AlgorithmParam.m_iGapMeasureImageNo3], pHInspectImage, HROIRgn, AlgorithmParam, &HPreProcessImage3);

				if (AlgorithmParam.m_bGapMeasureDiffImage)
					HDistanceMeasureAlgoDefectRgn = GapMeasureAlgorithm(&HPreProcessImage1, &HPreProcessImage2, &HPreProcessImage3, &(pHInspectImage[AlgorithmParam.m_iEdgeMeasureImageIndexOther]), &(pHInspectImage[AlgorithmParam.m_iGapMeasureRetryImageNo]), HROIRgn, AlgorithmParam, &HGapLineResultXLD, &dGapMeasureValueUm, &iGapUpperPointX, &iGapUpperPointY, &iGapLowerPointX, &iGapLowerPointY);
				else
					HDistanceMeasureAlgoDefectRgn = GapMeasureAlgorithm(&HPreProcessImage1, &HPreProcessImage2, NULL, &(pHInspectImage[AlgorithmParam.m_iEdgeMeasureImageIndexOther]), &(pHInspectImage[AlgorithmParam.m_iGapMeasureRetryImageNo]), HROIRgn, AlgorithmParam, &HGapLineResultXLD, &dGapMeasureValueUm, &iGapUpperPointX, &iGapUpperPointY, &iGapLowerPointX, &iGapLowerPointY);
			}
			else
				HDistanceMeasureAlgoDefectRgn = GapMeasureAlgorithm(&(pHInspectImage[AlgorithmParam.m_iGapMeasureImageNo1]), &(pHInspectImage[AlgorithmParam.m_iGapMeasureImageNo2]), &(pHInspectImage[AlgorithmParam.m_iGapMeasureImageNo3]), &(pHInspectImage[AlgorithmParam.m_iEdgeMeasureImageIndexOther]), &(pHInspectImage[AlgorithmParam.m_iGapMeasureRetryImageNo]), HROIRgn, AlgorithmParam, &HGapLineResultXLD, &dGapMeasureValueUm, &iGapUpperPointX, &iGapUpperPointY, &iGapLowerPointX, &iGapLowerPointY);

			if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
			{
				if (dGapMeasureValueUm > 0)
				{
					TupleConcat(m_HGapDistanceMeasureResult, HTuple(dGapMeasureValueUm), &m_HGapDistanceMeasureResult);
					ConcatObj(m_HGapMeasureXLD, HGapLineResultXLD, &m_HGapMeasureXLD);
				}
			}
			else
			{
				if (AlgorithmParam.m_bGapMeasureAlarm)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(HDistanceMeasureAlgoDefectRgn) == TRUE)
						THEAPP.m_bHandlerAlarmFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = TRUE;
				}

				if (AlgorithmParam.m_iGapMeasureSaveID >= 0 && AlgorithmParam.m_iGapMeasureSaveID < GAP_MEASURE_POINT_NUMBER)
				{
					if (dGapMeasureValueUm >= 0)
					{
						THEAPP.m_StructModuleGapMeasureXY[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bMeasureDone[AlgorithmParam.m_iGapMeasureSaveID] = TRUE;
						THEAPP.m_StructModuleGapMeasureXY[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iSaveID[AlgorithmParam.m_iGapMeasureSaveID] = AlgorithmParam.m_iGapMeasureSaveID;
						THEAPP.m_StructModuleGapMeasureXY[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dGapMeasureValueMm[AlgorithmParam.m_iGapMeasureSaveID] = dGapMeasureValueUm * 0.001;

						if (THEAPP.m_pGFunction->ValidHRegion(HDistanceMeasureAlgoDefectRgn) == TRUE)
							THEAPP.m_StructModuleGapMeasureXY[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bGapResultNG[AlgorithmParam.m_iGapMeasureSaveID] = TRUE;
					}
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDistanceMeasureAlgoDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HDistanceMeasureAlgoDefectRgn, &HDefectAllRgn);
		}

		// Corner Measure

		HObject HCornerMeasureAlgoDefectRgn;
		double dCornerDistanceMeasureValueMm;
		HObject HCornerLineResultXLD;
		double dCornerStartX, dCornerStartY, dCornerEndX, dCornerEndY;
		double dPixelResMm = THEAPP.m_pCalDataService_N[VISION_NUMBER_1]->GetPixelSize() * 0.001;

		if (AlgorithmParam.m_bUseCornerMeasure)
		{
			GenEmptyObj(&HCornerLineResultXLD);

			dCornerStartX = dCornerStartY = dCornerEndX = dCornerEndY = 0;

			HCornerMeasureAlgoDefectRgn = CornerMeasureAlgorithm(HImage, HROIRgn, AlgorithmParam, dPixelResMm, &HCornerLineResultXLD, &dCornerStartX, &dCornerStartY, &dCornerEndX, &dCornerEndY, &dCornerDistanceMeasureValueMm);

			if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
			{
				if (THEAPP.m_pGFunction->ValidHXLD(HCornerLineResultXLD))
				{
					ConcatObj(m_HCornerMeasureXLD, HCornerLineResultXLD, &m_HCornerMeasureXLD);
				}
			}
			else
			{
				if (AlgorithmParam.m_iCornerMeasureSaveID >= 1 && AlgorithmParam.m_iCornerMeasureSaveID <= CORNER_MEASURE_POINT_NUMBER)
				{
					THEAPP.m_StructModuleCornerMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dMeasureValueMm[AlgorithmParam.m_iCornerMeasureSaveID - 1] = dCornerDistanceMeasureValueMm;
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HCornerMeasureAlgoDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HCornerMeasureAlgoDefectRgn, &HDefectAllRgn);
		}

		///////////////////////

		HObject HConnectorPinDefectRgn;

		if (AlgorithmParam.m_bUsePinLength)
		{
			HConnectorPinDefectRgn = ConnectorPinDeformAlgorithm(HImage, HROIRgn, AlgorithmParam);

			if (THEAPP.m_pGFunction->ValidHRegion(HConnectorPinDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HConnectorPinDefectRgn, &HDefectAllRgn);
		}

		HObject HDentDefectRgn;

		if (AlgorithmParam.m_bUseDent)
		{
			HDentDefectRgn = DentAlgorithm(HImage, HROIRgn, AlgorithmParam);

			if (THEAPP.m_pGFunction->ValidHRegion(HDentDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HDentDefectRgn, &HDefectAllRgn);
		}

		HObject HHomerTiltDefectRgn;

		if (AlgorithmParam.m_bUseHomerTilt)
		{
			InitHomerTiltResult();

			HHomerTiltDefectRgn = HomerTiltAlgorithm(HImage, HROIRgn, AlgorithmParam, &(pHInspectImage[AlgorithmParam.m_iHomerTiltBenvolioImage]));

			if (THEAPP.m_pGFunction->ValidHRegion(HHomerTiltDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HHomerTiltDefectRgn, &HDefectAllRgn);
		}

		HObject HPartCheckDefectRgn;
		int iPartCheckLocalAlignDeltaX, iPartCheckLocalAlignDeltaY;
		double dPartCheckLocalAlignDeltaAngle, dPartCheckLocalAlignDeltaAngleFixedPointX, dPartCheckLocalAlignDeltaAngleFixedPointY;
		iPartCheckLocalAlignDeltaX = iPartCheckLocalAlignDeltaY = INVALID_ALIGN_RESULT;
		dPartCheckLocalAlignDeltaAngle = dPartCheckLocalAlignDeltaAngleFixedPointX = dPartCheckLocalAlignDeltaAngleFixedPointY = INVALID_ALIGN_RESULT;

		double dPartCheckShiftX, dPartCheckShiftY, dPartCheckScore;
		HObject HPartCheckAlignRgn;

		if (AlgorithmParam.m_bUsePartCheck == TRUE && AlgorithmParam.m_bPartCheckMultiModuleInspect == FALSE)
		{
			GenEmptyObj(&HPartCheckAlignRgn);

			HPartCheckDefectRgn = PartCheckAlgorithm(pInspectROIRgn->m_HPartModelID, HImage, HROIRgn, AlgorithmParam, &iPartCheckLocalAlignDeltaX, &iPartCheckLocalAlignDeltaY, &dPartCheckLocalAlignDeltaAngle, &dPartCheckLocalAlignDeltaAngleFixedPointX, &dPartCheckLocalAlignDeltaAngleFixedPointY, &dPartCheckShiftX, &dPartCheckShiftY, &dPartCheckScore, &HPartCheckAlignRgn, iMatchingPosOffsetX, iMatchingPosOffsetY);

			if (AlgorithmParam.m_bPartCheckLocalAlignUse)
			{
				pInspectROIRgn->m_dLocalAlignMatchingScore[iThreadIdx] = dPartCheckScore * 100.0;
				pInspectROIRgn->m_iLocalAlignDeltaX[iThreadIdx] = iPartCheckLocalAlignDeltaX;
				pInspectROIRgn->m_iLocalAlignDeltaY[iThreadIdx] = iPartCheckLocalAlignDeltaY;
				pInspectROIRgn->m_dLocalAlignDeltaAngle[iThreadIdx] = dPartCheckLocalAlignDeltaAngle;
				pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx] = dPartCheckLocalAlignDeltaAngleFixedPointX;
				pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx] = dPartCheckLocalAlignDeltaAngleFixedPointY;

				if (THEAPP.m_pGFunction->ValidHRegion(HPartCheckAlignRgn))
					pInspectROIRgn->m_HLocalAlignShapeRgn[iThreadIdx] = HPartCheckAlignRgn;
			}

			pMeasureData->m_dPartCheckShiftX = dPartCheckShiftX;
			pMeasureData->m_dPartCheckShiftY = dPartCheckShiftY;

			if (THEAPP.m_pGFunction->ValidHRegion(HPartCheckDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HPartCheckDefectRgn, &HDefectAllRgn);
		}

		HObject HNeckEpoxyDefectRgn;
		double dEpoxyMeasureValue_TopOffset = 0;
		double dEpoxyMeasureValue_BottomOffset = 0;
		double dEpoxyMeasureValue_Length = 0;
		double dEpoxyMeasureValue_HeightMax = 0;
		double dEpoxyMeasureValue_HeightMin = 0;

		if (AlgorithmParam.m_bUseNeckEpoxyMeasure)
		{
			HNeckEpoxyDefectRgn = NeckEpoxyMeasureAlgorithm(HImage, HDCImage, HROIRgn, AlgorithmParam, pHResultXLD, pHMaskRgn, &dEpoxyMeasureValue_TopOffset, &dEpoxyMeasureValue_BottomOffset, &dEpoxyMeasureValue_Length, &dEpoxyMeasureValue_HeightMax, &dEpoxyMeasureValue_HeightMin);

			if (THEAPP.m_pGFunction->ValidHRegion(HNeckEpoxyDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HNeckEpoxyDefectRgn, &HDefectAllRgn);
		}

		HObject HNeckEpoxyCrackRgn;

		if (AlgorithmParam.m_bUseNeckEpoxyCrack)
		{
			if (AlgorithmParam.m_bNeckEpoxyCrackUseXDirMode == FALSE)
				HNeckEpoxyCrackRgn = NeckEpoxyCrackAlgorithm(HImage, HDCImage, HROIRgn, AlgorithmParam, pHResultXLD, pHMaskRgn);
			else //VER 1.1.3.0 add
				HNeckEpoxyCrackRgn = NeckEpoxyCrackAlgorithm_XDir(HImage, HDCImage, HROIRgn, AlgorithmParam, pHResultXLD, pHMaskRgn);

			if (THEAPP.m_pGFunction->ValidHRegion(HNeckEpoxyCrackRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HNeckEpoxyCrackRgn, &HDefectAllRgn);
		}

		if (AlgorithmParam.m_bUseBarcode)
		{
			BarcodeAlgorithm(HImage, HROIRgn, AlgorithmParam, iInspectBufferIndex, &(pHInspectImage[AlgorithmParam.m_iBarcodeOtherImageNumber - 1]));
		}

		if (AlgorithmParam.m_bUseOCR)
		{
			InitOcrResult();

			OCRAlgorithm(HImage, HROIRgn, AlgorithmParam);
		}

		HObject HBarcodePrintQualityDefectRgn;
		double pdPrintQualityGrade[MAX_PRINT_QUALITY_GRADE];
		CString psPrintQualityValue[MAX_PRINT_QUALITY_GRADE];

		if (AlgorithmParam.m_bUsePrintQuality)
		{
			HBarcodePrintQualityDefectRgn = BarcodePrintQualityAlgorithm(HImage, HROIRgn, AlgorithmParam, pdPrintQualityGrade, psPrintQualityValue);

			pMeasureData->m_bInspectPrintQuality = TRUE;
			for (int ii = 0; ii < MAX_PRINT_QUALITY_GRADE; ii++)
			{
				pMeasureData->m_dDatacodePrintQualityGrade[ii] = pdPrintQualityGrade[ii];
				pMeasureData->m_sDatacodePrintQualityValue[ii] = psPrintQualityValue[ii];
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HBarcodePrintQualityDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HBarcodePrintQualityDefectRgn, &HDefectAllRgn);
		}

		HObject HHeightMeasureRgn;

		if (AlgorithmParam.m_bUseHeightMeasure)
		{
			HHeightMeasureRgn = HeightMeasureAlgorithm(HImage, HROIRgn, AlgorithmParam, &(pMeasureData->m_dHeightMm));

			if (THEAPP.m_pGFunction->ValidHRegion(HHeightMeasureRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HHeightMeasureRgn, &HDefectAllRgn);
		}

		HObject HBoundaryDefectRgn;

		if (AlgorithmParam.m_bUseBoundaryInspection)
		{
			if (iInspectBufferIndex < 0)
				HBoundaryDefectRgn = BoundaryInspectionAlgorithm(HImage, HROIRgn, AlgorithmParam, TRUE);
			else
				HBoundaryDefectRgn = BoundaryInspectionAlgorithm(HImage, HROIRgn, AlgorithmParam, FALSE);

			if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HBoundaryDefectRgn, &HDefectAllRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectAllRgn) == TRUE)
		{
			Union1(HDefectAllRgn, &(pMeasureData->m_HInspectTabDefectRgn));
			Union1(HDefectAllRgn, &HDefectAllRgn);
		}

		HObject HAlignMultiModuleRgn;

		// Align Vision Only (검사할 모듈 갯수가 Input 됨)
		if (AlgorithmParam.m_bUseROIAlign == TRUE && AlgorithmParam.m_bROIAlignMultiModuleInspect == TRUE && AlgorithmParam.m_iROIAlignMultiModuleVision == MULTI_MODULE_VISION_ALIGN)
		{
			GenEmptyObj(&HAlignMultiModuleRgn);

			if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
			{
				int iNoMaxModuleInTray = AlgorithmParam.m_iROIAlignMultiModuleXNumber * AlgorithmParam.m_iROIAlignMultiModuleYNumber;
				if (iNoMaxModuleInTray < 1)
					iNoMaxModuleInTray = 1;
				HAlignMultiModuleRgn = ROIAlignAlgorithm_MultiModule_Align(HImage, HROIRgn, AlgorithmParam, &(pInspectROIRgn->m_HROIAlignModelID), iNoMaxModuleInTray, 0);
			}
			else
			{
				int iNoMaxModuleInTray = AlgorithmParam.m_iROIAlignMultiModuleXNumber * AlgorithmParam.m_iROIAlignMultiModuleYNumber;
				if (iNoMaxModuleInTray < 1)
					iNoMaxModuleInTray = 1;
				HAlignMultiModuleRgn = ROIAlignAlgorithm_MultiModule_Align(HImage, HROIRgn, AlgorithmParam, &(pInspectROIRgn->m_HROIAlignModelID), iNoMaxModuleInTray, iInspectBufferIndex);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HAlignMultiModuleRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HAlignMultiModuleRgn, &HDefectAllRgn);
		}

		// Align Vision Only (검사할 모듈 갯수가 Input 됨)
		if (AlgorithmParam.m_bUseROIAnisoAlign == TRUE && AlgorithmParam.m_bROIAnisoAlignMultiModuleInspect == TRUE && AlgorithmParam.m_iROIAnisoAlignMultiModuleVision == MULTI_MODULE_VISION_ALIGN)
		{
			GenEmptyObj(&HAlignMultiModuleRgn);

			if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
			{
				int iNoMaxModuleInTray = AlgorithmParam.m_iROIAnisoAlignMultiModuleXNumber * AlgorithmParam.m_iROIAnisoAlignMultiModuleYNumber;
				if (iNoMaxModuleInTray < 1)
					iNoMaxModuleInTray = 1;
				HAlignMultiModuleRgn = ROIAnisoAlignAlgorithm_MultiModule_Align(HImage, HROIRgn, AlgorithmParam, &(pInspectROIRgn->m_HROIAlignModelID), iNoMaxModuleInTray, 0);
			}
			else
			{
				int iNoMaxModuleInTray = AlgorithmParam.m_iROIAnisoAlignMultiModuleXNumber * AlgorithmParam.m_iROIAnisoAlignMultiModuleYNumber;
				if (iNoMaxModuleInTray < 1)
					iNoMaxModuleInTray = 1;
				HAlignMultiModuleRgn = ROIAnisoAlignAlgorithm_MultiModule_Align(HImage, HROIRgn, AlgorithmParam, &(pInspectROIRgn->m_HROIAlignModelID), iNoMaxModuleInTray, iInspectBufferIndex);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HAlignMultiModuleRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HAlignMultiModuleRgn, &HDefectAllRgn);
		}

		// Align Vision Only (검사할 모듈 갯수가 Input 됨)
		if (AlgorithmParam.m_bUsePartCheck == TRUE && AlgorithmParam.m_bPartCheckMultiModuleInspect == TRUE)
		{
			GenEmptyObj(&HAlignMultiModuleRgn);

			if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
			{
				// Reset Loading Align Results
				m_iAlignNoInspectModule[0] = 0;
				GenEmptyObj(&(m_HAlignResultRgn_EachImage[0]));
				TupleGenConst(0, 0, m_HAlignModuleExist);
				TupleGenConst(0, 0, m_HAlignModuleIsDefect);
				TupleGenConst(0, 0, m_HAlignModuleDefectPosX);
				TupleGenConst(0, 0, m_HAlignModuleDefectPosY);
				TupleGenConst(0, 0, m_HAlignModuleDeltaX);
				TupleGenConst(0, 0, m_HAlignModuleDeltaY);
				TupleGenConst(0, 0, m_HAlignModuleDeltaAngle);

				int iNoMaxModuleInTray = AlgorithmParam.m_iPartCheckMultiModuleXNumber * AlgorithmParam.m_iPartCheckMultiModuleYNumber;
				if (iNoMaxModuleInTray < 1)
					iNoMaxModuleInTray = 1;
				HAlignMultiModuleRgn = PartCheckAlgorithm_MultiModule_Align(pInspectROIRgn->m_HPartModelID, HImage, HROIRgn, AlgorithmParam, iNoMaxModuleInTray, 0);
			}
			else
			{
				int iNoMaxModuleInTray = AlgorithmParam.m_iPartCheckMultiModuleXNumber * AlgorithmParam.m_iPartCheckMultiModuleYNumber;
				if (iNoMaxModuleInTray < 1)
					iNoMaxModuleInTray = 1;
				HAlignMultiModuleRgn = PartCheckAlgorithm_MultiModule_Align(pInspectROIRgn->m_HPartModelID, HImage, HROIRgn, AlgorithmParam, iNoMaxModuleInTray, iInspectBufferIndex);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignMultiModuleRgn) == TRUE)
			ConcatObj(HDefectAllRgn, HAlignMultiModuleRgn, &HDefectAllRgn);

		return HDefectAllRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::InspectAVI] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

BOOL Algorithm::BarcodeAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, int iInspectBufferIndex, HObject *pHBarcodeOtherImage)
{
	BOOL bDebugSave = FALSE;

	int iMzNo, iTrayNo, iModuleNo;

	if (iInspectBufferIndex < 0)
	{
		iMzNo = 1;
		iTrayNo = 1;
		iModuleNo = 1;
	}
	else
	{
		iMzNo = m_nMzNo[iInspectBufferIndex];
		iTrayNo = m_nTrayNo[iInspectBufferIndex];
		iModuleNo = m_nModuleNo[iInspectBufferIndex];
	}

	CString strPrevBarcodeResult = "NOREAD";
	//210730 jwj add (알고리즘탭에서 바코드 미인식하더라도 앞서 읽은 바코드가 있으면 해당 바코드 사용)
	if ((THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] == "NOREAD" || THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] == "NOINFO") == FALSE)
	{
		strPrevBarcodeResult = THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1];
	}
	//jwj add end

	THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].Reset();

	THEAPP.m_iModuleMixResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = FALSE;
	HObject HInspectAreaRgn;
	GenEmptyObj(&HInspectAreaRgn);

	GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);

	HObject HBarcodeImage, HRetryBarcodeImage;
	POINT BarcodeLTPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;
	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	SmallestRectangle1(HInspectAreaRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);
	lRow1 = HlRow1.L();
	lRow2 = HlRow2.L();
	lCol1 = HlCol1.L();
	lCol2 = HlCol2.L();

	BarcodeLTPoint.x = lCol1;
	BarcodeLTPoint.y = lRow1;

	CropRectangle1(HImage, &HBarcodeImage, lRow1, lCol1, lRow2, lCol2);
	if (Param.m_bUseBarcodeOtherImage)
		CropRectangle1(*pHBarcodeOtherImage, &HRetryBarcodeImage, lRow1, lCol1, lRow2, lCol2);

	if (bDebugSave)
	{
		WriteImage(HBarcodeImage, "bmp", 0, "c:\\DualTest\\BarcodeInspectImage");
	}

	HTuple DataCodeHandle, ResultHandles, DecodedDataStrings;
	HObject SymbolXLDs, HFilteredImage;

	Hlong lArea;
	double dCenterY, dCenterX;
	HTuple HlArea, HdCenterY, HdCenterX;

	HTuple HStrLength;
	int iLength;

	int iNoBarcodeString = 0;
	HObject HBrightRgn;

	CreateDataCode2dModel(HTuple("Data Matrix ECC 200"), HTuple(), HTuple(), &DataCodeHandle);

	THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult = FALSE;

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
				THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult = TRUE;
			}
		}
		else
		{
			if (Param.m_bUseBarcodeOtherImage)
			{
				GenEmptyObj(&SymbolXLDs);
				FindDataCode2d(HRetryBarcodeImage, &SymbolXLDs, DataCodeHandle, HTuple(), HTuple(), &ResultHandles, &DecodedDataStrings);
				iNoBarcodeString = DecodedDataStrings.Length();
				if (iNoBarcodeString > 0)
				{
					TupleStrlen(DecodedDataStrings, &HStrLength);
					iLength = HStrLength[0].I();

					if (iLength >= MIN_BARCODE_CHAR_LENGTH)
					{
						THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult = TRUE;
					}
				}
			}
		}

		if (THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult == FALSE)		// Enhanced Edition
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
					THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult = TRUE;
				}
			}
			else
			{
				if (Param.m_bUseBarcodeOtherImage)
				{
					GenEmptyObj(&SymbolXLDs);
					FindDataCode2d(HRetryBarcodeImage, &SymbolXLDs, DataCodeHandle, HTuple(), HTuple(), &ResultHandles, &DecodedDataStrings);
					iNoBarcodeString = DecodedDataStrings.Length();
					if (iNoBarcodeString > 0)
					{
						TupleStrlen(DecodedDataStrings, &HStrLength);
						iLength = HStrLength[0].I();

						if (iLength >= MIN_BARCODE_CHAR_LENGTH)
						{
							THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult = TRUE;
						}
					}
				}
			}
		}

		if (THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult == FALSE)		// Maximum Edition
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
					THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult = TRUE;
				}
			}
			else
			{
				if (Param.m_bUseBarcodeOtherImage)
				{
					GenEmptyObj(&SymbolXLDs);
					FindDataCode2d(HRetryBarcodeImage, &SymbolXLDs, DataCodeHandle, HTuple(), HTuple(), &ResultHandles, &DecodedDataStrings);
					iNoBarcodeString = DecodedDataStrings.Length();
					if (iNoBarcodeString > 0)
					{
						TupleStrlen(DecodedDataStrings, &HStrLength);
						iLength = HStrLength[0].I();

						if (iLength >= MIN_BARCODE_CHAR_LENGTH)
						{
							THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult = TRUE;
						}
					}
				}
			}
		}
	}
	catch (HException& e)
	{
		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult = FALSE;
	}

	ClearDataCode2dModel(DataCodeHandle);

	CString sBarcodeNoReadSaveFolderName = THEAPP.Struct_PreferenceStruct.m_strEtcFolderPath + "CMI_Results\\Barcode_NoRead";
	CString sBarcodeFilename;

	BYTE *pImageData;
	char type[30];
	Hlong lImageWidth, lImageHeight;
	HTuple HlImageWidth, HlImageHeight, Htype, HpImageData;

	GetImagePointer1(HBarcodeImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

	pImageData = (BYTE *)HpImageData.L();
	lImageWidth = HlImageWidth.L();
	lImageHeight = HlImageHeight.L();

	double dBarcodeRefCenterX, dBarcodeRefCenterY;
	dBarcodeRefCenterX = (double)lImageWidth * 0.5;
	dBarcodeRefCenterY = (double)lImageHeight * 0.5;

	HObject HBarcodeDetectRgn, HBarcodeThresRgn, HTempRgn, HBarcodeBoundaryRgn;
	long lBarcodeHeight, lBarcodeWidth;
	HObject HSE_LHRgn, HSE_LWRgn, HCornerDetectionRgn;
	double dCornerPosX, dCornerPosY;
	HTuple HdCornerPosX, HdCornerPosY;
	double dDistLT, dDistLB, dDistRT, dDistRB, dDistMin;
	int iCornerIndex;

	if (THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult)
	{
		try
		{
			// Barcode 인식

			THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_sBarcodeResult = DecodedDataStrings[0].S();

			GenRegionContourXld(SymbolXLDs, &HBarcodeDetectRgn, "filled");
			MoveRegion(HBarcodeDetectRgn, &THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_HBarcodeDetectRgn, BarcodeLTPoint.y, BarcodeLTPoint.x);

			// Barcode Shift

			if (Param.m_bInspectBarcodeShift)
			{
				AreaCenter(HBarcodeDetectRgn, &HlArea, &HdCenterY, &HdCenterX);
				lArea = HlArea.L();
				dCenterY = HdCenterY.D();
				dCenterX = HdCenterX.D();

				THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dBarcodePosOffsetX = (dCenterX - dBarcodeRefCenterX) * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * 0.001;
				THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dBarcodePosOffsetY = (dCenterY - dBarcodeRefCenterY) * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * 0.001;

				double dAbsShiftX, dAbsShiftY;
				dAbsShiftX = fabs(THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dBarcodePosOffsetX);
				dAbsShiftY = fabs(THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dBarcodePosOffsetY);

				if (dAbsShiftX >= Param.m_dBarcodeShiftToleranceX || dAbsShiftY >= Param.m_dBarcodeShiftToleranceY)
					THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeShiftNG = TRUE;
			}

			// Barcode 각도

			if (Param.m_bInspectBarcodeRotation)
			{
				if (bDebugSave)
				{
					WriteRegion(HBarcodeDetectRgn, "c:\\DualTest\\BarcodeDetectRgn");
				}

				HObject HBarcodeSmoothingImage, HMeanImage, HDynThreshRgn;
				MedianImage(HBarcodeImage, &HBarcodeSmoothingImage, "circle", 3, "mirrored");

				MeanImage(HBarcodeSmoothingImage, &HMeanImage, 101, 101);
				DynThreshold(HBarcodeSmoothingImage, HMeanImage, &HDynThreshRgn, 12, "dark");
				Intersection(HBarcodeDetectRgn, HDynThreshRgn, &HBarcodeThresRgn);

				// BinThreshold(HBarcodeImageReduced, &HBarcodeThresRgn);
				//auto_Threshold(HBarcodeImageReduced, &HBarcodeThresRgn, 3.0);
				//SelectObj(HBarcodeThresRgn, &HBarcodeThresRgn, 1);		// First Peak

				ErosionCircle(HBarcodeDetectRgn, &HTempRgn, 10.5);
				Difference(HBarcodeDetectRgn, HTempRgn, &HBarcodeBoundaryRgn);
				Intersection(HBarcodeThresRgn, HBarcodeBoundaryRgn, &HTempRgn);
				ClosingCircle(HTempRgn, &HTempRgn, 2.5);
				Connection(HTempRgn, &HTempRgn);
				SelectShapeStd(HTempRgn, &HTempRgn, "max_area", 70);

				SmallestRectangle1(HBarcodeDetectRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

				lRow1 = HlRow1.L();
				lCol1 = HlCol1.L();
				lRow2 = HlRow2.L();
				lCol2 = HlCol2.L();

				lBarcodeHeight = lRow2 - lRow1 + 1;
				lBarcodeWidth = lCol2 - lCol1 + 1;

				GenRectangle1(&HSE_LHRgn, 0, 0, lBarcodeHeight / 2, 0);
				GenRectangle1(&HSE_LWRgn, 0, 0, 0, lBarcodeWidth / 2);

				Opening(HTempRgn, HSE_LHRgn, &HCornerDetectionRgn);
				AreaCenter(HCornerDetectionRgn, &HlArea, &HdCenterY, &HdCornerPosX);
				lArea = HlArea.L();
				dCenterY = HdCenterY.D();
				dCornerPosX = HdCornerPosX.D();

				Opening(HTempRgn, HSE_LWRgn, &HCornerDetectionRgn);
				AreaCenter(HCornerDetectionRgn, &HlArea, &HdCornerPosY, &HdCenterX);
				lArea = HlArea.L();
				dCornerPosY = HdCornerPosY.D();
				dCenterX = HdCenterX.D();

				dDistLT = (lRow1 - dCornerPosY)*(lRow1 - dCornerPosY) + (lCol1 - dCornerPosX)*(lCol1 - dCornerPosX);
				dDistLB = (lRow2 - dCornerPosY)*(lRow2 - dCornerPosY) + (lCol1 - dCornerPosX)*(lCol1 - dCornerPosX);
				dDistRT = (lRow1 - dCornerPosY)*(lRow1 - dCornerPosY) + (lCol2 - dCornerPosX)*(lCol2 - dCornerPosX);
				dDistRB = (lRow2 - dCornerPosY)*(lRow2 - dCornerPosY) + (lCol2 - dCornerPosX)*(lCol2 - dCornerPosX);

				dDistMin = dDistLT;
				iCornerIndex = 0;

				if (dDistLB < dDistMin)
				{
					dDistMin = dDistLB;
					iCornerIndex = 1;
				}
				if (dDistRT < dDistMin)
				{
					dDistMin = dDistRT;
					iCornerIndex = 2;
				}
				if (dDistRB < dDistMin)
				{
					dDistMin = dDistRB;
					iCornerIndex = 3;
				}

				if (iCornerIndex == 0)
					THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iBarcodeRotationResult = 0;
				else if (iCornerIndex == 1)
					THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iBarcodeRotationResult = 90;
				else if (iCornerIndex == 2)
					THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iBarcodeRotationResult = 90;
				else if (iCornerIndex == 3)
					THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iBarcodeRotationResult = 180;

				if (THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iBarcodeRotationResult != 0)
					THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRotationNG = TRUE;
			}
		}
		catch (HException& e)
		{
			;
		}
	}
	else
	{
		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_sBarcodeResult = _T("NOREAD");

		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_HBarcodeDetectRgn = HInspectAreaRgn;		// 검사영역으로 대체

		SYSTEMTIME time;
		GetLocalTime(&time);

		sBarcodeFilename.Format("%s\\Barcode_%04d%02d%02d_%02d%02d%02d_%03d", sBarcodeNoReadSaveFolderName,
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

		/*if (THEAPP.Struct_PreferenceStruct.m_iBarcodeNoReadImageType == 0)
			WriteImage(HBarcodeImage, "bmp", 0, HTuple(sBarcodeFilename));
		else
			WriteImage(HBarcodeImage, "jpg", 0, HTuple(sBarcodeFilename));*/
	}

	//jwj add 210730
	if (strPrevBarcodeResult != "NOREAD" && THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult == FALSE)
	{
		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_sBarcodeResult = strPrevBarcodeResult;
		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult = TRUE;
		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRotationNG = FALSE;
		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeShiftNG = FALSE;
		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dBarcodePosOffsetX = 0;
		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dBarcodePosOffsetY = 0;
		THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iBarcodeRotationResult = 0;
	}
	//jwj add end

	return THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult;
}

HObject Algorithm::BarcodePrintQualityAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, double *pdPrintQualityGrade, CString *psPrintQualityValue)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		HObject HBarcodeImage;
		POINT BarcodeLTPoint;
		Hlong lRow1, lRow2, lCol1, lCol2;
		HTuple HlRow1, HlRow2, HlCol1, HlCol2;

		SmallestRectangle1(HInspectAreaRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);
		lRow1 = HlRow1.L();
		lRow2 = HlRow2.L();
		lCol1 = HlCol1.L();
		lCol2 = HlCol2.L();

		BarcodeLTPoint.x = lCol1;
		BarcodeLTPoint.y = lRow1;

		CropRectangle1(HImage, &HBarcodeImage, lRow1, lCol1, lRow2, lCol2);

		if (bDebugSave)
		{
			WriteImage(HBarcodeImage, "bmp", 0, "c:\\DualTest\\BarcodeQualityImage");
		}

		HTuple DataCodeHandle, ResultHandles, DecodedDataStrings;
		HObject SymbolXLDs;
		HTuple HStrLength;
		int iLength;

		int iNoBarcodeString = 0;

		CreateDataCode2dModel(HTuple("Data Matrix ECC 200"), HTuple(), HTuple(), &DataCodeHandle);

		BOOL bBarcodeRegResult = FALSE;
		BOOL bBarcodePrintQualityFail = FALSE;

		HTuple HPrintQualityGrade, HPrintQualityValue, HIsString;

		try
		{
			SetDataCode2dParam(DataCodeHandle, HTuple("default_parameters"), HTuple("standard_recognition"));
			SetDataCode2dParam(DataCodeHandle, HTuple("timeout"), HTuple("false"));
			SetDataCode2dParam(DataCodeHandle, HTuple("persistence"), 0);

			if (Param.m_iPrintQualityStrictModel == PQ_STRICT_MODEL_YES)
				SetDataCode2dParam(DataCodeHandle, HTuple("strict_model"), HTuple("yes"));
			else if (Param.m_iPrintQualityStrictModel == PQ_STRICT_MODEL_NO)
				SetDataCode2dParam(DataCodeHandle, HTuple("strict_model"), HTuple("no"));

			if (Param.m_iPrintQualityPolarity == PQ_POLARITY_DARK_ON_LIGHT)
				SetDataCode2dParam(DataCodeHandle, HTuple("polarity"), HTuple("dark_on_light"));
			else if (Param.m_iPrintQualityPolarity == PQ_POLARITY_LIGHT_ON_DARK)
				SetDataCode2dParam(DataCodeHandle, HTuple("polarity"), HTuple("light_on_dark"));
			else if (Param.m_iPrintQualityPolarity == PQ_POLARITY_ANY)
				SetDataCode2dParam(DataCodeHandle, HTuple("polarity"), HTuple("any"));

			if (Param.m_iPrintQualityMirrored == PQ_MIRRORED_YES)
				SetDataCode2dParam(DataCodeHandle, HTuple("mirrored"), HTuple("yes"));
			else if (Param.m_iPrintQualityMirrored == PQ_MIRRORED_NO)
				SetDataCode2dParam(DataCodeHandle, HTuple("mirrored"), HTuple("no"));
			else if (Param.m_iPrintQualityMirrored == PQ_MIRRORED_ANY)
				SetDataCode2dParam(DataCodeHandle, HTuple("mirrored"), HTuple("any"));

			SetDataCode2dParam(DataCodeHandle, HTuple("contrast_min"), Param.m_iPrintQualityContrastMin);

			if (Param.m_iPrintQualityContrastTolerance == PQ_CONTRAST_TOLERANCE_LOW)
				SetDataCode2dParam(DataCodeHandle, HTuple("contrast_tolerance"), HTuple("low"));
			else if (Param.m_iPrintQualityContrastTolerance == PQ_CONTRAST_TOLERANCE_HIGH)
				SetDataCode2dParam(DataCodeHandle, HTuple("contrast_tolerance"), HTuple("high"));
			else if (Param.m_iPrintQualityContrastTolerance == PQ_CONTRAST_TOLERANCE_ANY)
				SetDataCode2dParam(DataCodeHandle, HTuple("contrast_tolerance"), HTuple("any"));

			if (Param.m_iPrintQualityStrictQuietZone == PQ_STRICT_QUIET_ZONE_YES)
				SetDataCode2dParam(DataCodeHandle, HTuple("strict_quiet_zone"), HTuple("yes"));
			else if (Param.m_iPrintQualityStrictQuietZone == PQ_STRICT_QUIET_ZONE_NO)
				SetDataCode2dParam(DataCodeHandle, HTuple("strict_quiet_zone"), HTuple("no"));

			SetDataCode2dParam(DataCodeHandle, HTuple("symbol_cols_min"), Param.m_iPrintQualitySymbolColsMin);
			SetDataCode2dParam(DataCodeHandle, HTuple("symbol_rows_min"), Param.m_iPrintQualitySymbolRowsMin);
			SetDataCode2dParam(DataCodeHandle, HTuple("symbol_cols_max"), Param.m_iPrintQualitySymbolColsMax);
			SetDataCode2dParam(DataCodeHandle, HTuple("symbol_rows_max"), Param.m_iPrintQualitySymbolRowsMax);

			if (Param.m_iPrintQualitySymbolShape == PQ_SYMBOL_SHAPE_RECTANGLE)
				SetDataCode2dParam(DataCodeHandle, HTuple("symbol_shape"), HTuple("rectangle"));
			else if (Param.m_iPrintQualitySymbolShape == PQ_SYMBOL_SHAPE_SQUARE)
				SetDataCode2dParam(DataCodeHandle, HTuple("symbol_shape"), HTuple("square"));
			else if (Param.m_iPrintQualitySymbolShape == PQ_SYMBOL_SHAPE_ANY)
				SetDataCode2dParam(DataCodeHandle, HTuple("symbol_shape"), HTuple("any"));

			SetDataCode2dParam(DataCodeHandle, HTuple("module_size_min"), Param.m_iPrintQualityModuleSizeMin);
			SetDataCode2dParam(DataCodeHandle, HTuple("module_size_max"), Param.m_iPrintQualityModuleSizeMax);

			if (Param.m_iPrintQualitySmallModulesRobustness == PQ_SMALL_MODULES_ROBUSTNESS_LOW)
				SetDataCode2dParam(DataCodeHandle, HTuple("small_modules_robustness"), HTuple("low"));
			else if (Param.m_iPrintQualitySmallModulesRobustness == PQ_SMALL_MODULES_ROBUSTNESS_HIGH)
				SetDataCode2dParam(DataCodeHandle, HTuple("small_modules_robustness"), HTuple("high"));

			if (Param.m_iPrintQualityModuleGrid == PQ_MODULE_GRID_FIXED)
				SetDataCode2dParam(DataCodeHandle, HTuple("module_grid"), HTuple("fixed"));
			else if (Param.m_iPrintQualityModuleGrid == PQ_MODULE_GRID_VARIABLE)
				SetDataCode2dParam(DataCodeHandle, HTuple("module_grid"), HTuple("variable"));
			else if (Param.m_iPrintQualityModuleGrid == PQ_MODULE_GRID_ANY)
				SetDataCode2dParam(DataCodeHandle, HTuple("module_grid"), HTuple("any"));

			if (Param.m_iPrintQualityModuleGapMin == PQ_MODULE_GAP_NO)
				SetDataCode2dParam(DataCodeHandle, HTuple("module_gap_min"), HTuple("no"));
			else if (Param.m_iPrintQualityModuleGapMin == PQ_MODULE_GAP_SMALL)
				SetDataCode2dParam(DataCodeHandle, HTuple("module_gap_min"), HTuple("small"));
			else if (Param.m_iPrintQualityModuleGapMin == PQ_MODULE_GAP_BIG)
				SetDataCode2dParam(DataCodeHandle, HTuple("module_gap_min"), HTuple("big"));

			if (Param.m_iPrintQualityModuleGapMax == PQ_MODULE_GAP_NO)
				SetDataCode2dParam(DataCodeHandle, HTuple("module_gap_max"), HTuple("no"));
			else if (Param.m_iPrintQualityModuleGapMax == PQ_MODULE_GAP_SMALL)
				SetDataCode2dParam(DataCodeHandle, HTuple("module_gap_max"), HTuple("small"));
			else if (Param.m_iPrintQualityModuleGapMax == PQ_MODULE_GAP_BIG)
				SetDataCode2dParam(DataCodeHandle, HTuple("module_gap_max"), HTuple("big"));

			if (Param.m_iPrintQualityFinderPatternTolerance == PQ_FINDER_PATTERN_TOLERANCE_LOW)
				SetDataCode2dParam(DataCodeHandle, HTuple("finder_pattern_tolerance"), HTuple("low"));
			else if (Param.m_iPrintQualityFinderPatternTolerance == PQ_FINDER_PATTERN_TOLERANCE_HIGH)
				SetDataCode2dParam(DataCodeHandle, HTuple("finder_pattern_tolerance"), HTuple("high"));
			else if (Param.m_iPrintQualityFinderPatternTolerance == PQ_FINDER_PATTERN_TOLERANCE_ANY)
				SetDataCode2dParam(DataCodeHandle, HTuple("finder_pattern_tolerance"), HTuple("any"));

			SetDataCode2dParam(DataCodeHandle, HTuple("slant_max"), Param.m_dPrintQualitySlantMax);

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
		catch (HException& e)
		{
			bBarcodeRegResult = FALSE;
		}

		if (bBarcodeRegResult)
		{
			HTuple HNoQualityValue;
			long lGrade;
			CString sLabel, sValue;
			double dGrade, dValue;

			GetDataCode2dResults(DataCodeHandle, ResultHandles, "quality_isoiec_tr_29158", &HPrintQualityGrade);
			TupleLength(HPrintQualityGrade, &HNoQualityValue);
			Hlong lNoGradeItem = HNoQualityValue.L();

			for (int i = 0; i < MAX_PRINT_QUALITY_GRADE; i++)
			{
				pdPrintQualityGrade[i] = HPrintQualityGrade[i].D();
			}

			GetDataCode2dResults(DataCodeHandle, ResultHandles, "quality_isoiec_tr_29158_values", &HPrintQualityValue);
			TupleLength(HPrintQualityValue, &HNoQualityValue);
			Hlong lNoValueItem = HNoQualityValue.L();

			for (int i = 0; i < MAX_PRINT_QUALITY_GRADE; i++)
			{
				TupleIsStringElem(HPrintQualityValue[i], &HIsString);

				if (HIsString == 1)
					psPrintQualityValue[i] = HPrintQualityValue[i].S();
				else
				{
					dValue = HPrintQualityValue[i].D();
					psPrintQualityValue[i].Format("%.2lf", dValue);
				}
			}

			if (Param.m_bPrintQualityOverallQuality)	// overall quality
			{
				lGrade = (long)pdPrintQualityGrade[0];

				if (lGrade <= Param.m_iPrintQualityGradeOverallQuality)
					bBarcodePrintQualityFail = TRUE;
			}

			//if (Param.m_bPrintQualityUnusedErrorCorrection)
			//{
			//	lGrade = HPrintQualityGrade[7].L();

			//	if (lGrade <= Param.m_iPrintQualityGradeUnusedErrorCorrection)
			//		bBarcodePrintQualityFail = TRUE;
			//}

			if (bBarcodePrintQualityFail)
				ConcatObj(HDefectRgn, HInspectAreaRgn, &HDefectRgn);
		}
		else
		{
			ConcatObj(HDefectRgn, HInspectAreaRgn, &HDefectRgn);
		}

		ClearDataCode2dModel(DataCodeHandle);

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			Union1(HDefectRgn, &HDefectRgn);

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::BarcodePrintQualityAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::PartCheckAlgorithm(HTuple HPartModelID, HObject HImage, HObject HROIRgn, CAlgorithmParam Param, int *piLocalAlignDeltaX, int *piLocalAlignDeltaY, double *pdLocalAlignDeltaAngle, double *pdLocalAlignDeltaAngleFixedPointX, double *pdLocalAlignDeltaAngleFixedPointY, double *pdShiftX, double *pdShiftY, double *pdScore, HObject *pHAlignRgn, int iMatchingPosOffsetX, int iMatchingPosOffsetY)
{
	*pdScore = -1;

	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		GenEmptyObj(pHAlignRgn);

		*pdShiftX = 0;
		*pdShiftY = 0;

		if (HPartModelID < 0)
		{
			return HDefectRgn;
		}

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			// WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn.reg");
		}

		// Model Ref: G-Center of Domain
		double dTeachAlignRefX, dTeachAlignRefY;
		HTuple HlArea, HdTeachAlignRefY, HdTeachAlignRefX;
		AreaCenter(HInspectAreaRgn, &HlArea, &HdTeachAlignRefY, &HdTeachAlignRefX);
		dTeachAlignRefY = HdTeachAlignRefY.D();
		dTeachAlignRefX = HdTeachAlignRefX.D();

		int iSearchMarginLeft = Param.m_iPartCheckMatchingSearchMarginX;
		if (iSearchMarginLeft < 0)
			iSearchMarginLeft = 0;
		int iSearchMarginRight = Param.m_iPartCheckMatchingSearchMarginX2;
		if (iSearchMarginRight < 0)
			iSearchMarginRight = 0;
		int iSearchMarginTop = Param.m_iPartCheckMatchingSearchMarginY;
		if (iSearchMarginTop < 0)
			iSearchMarginTop = 0;
		int iSearchMarginBottom = Param.m_iPartCheckMatchingSearchMarginY2;
		if (iSearchMarginBottom < 0)
			iSearchMarginBottom = 0;

		HObject HInspectAreaDilatedRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaDilatedRgn, "rectangle1");

		HObject HMatchingStartPosRgn;
		MoveRegion(HInspectAreaDilatedRgn, &HMatchingStartPosRgn, iMatchingPosOffsetY, iMatchingPosOffsetX);

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		SmallestRectangle1(HMatchingStartPosRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
		lLTPointY = HlLTPointY.L();
		lLTPointX = HlLTPointX.L();
		lRBPointY = HlRBPointY.L();
		lRBPointX = HlRBPointX.L();

		Hlong lImageWidth, lImageHeight;
		HTuple HImageWidth, HImageHeight;
		GetImageSize(HImage, &HImageWidth, &HImageHeight);
		lImageWidth = HImageWidth.L();
		lImageHeight = HImageHeight.L();

		Hlong lCropLTPointX, lCropLTPointY, lCropRBPointX, lCropRBPointY;

		lCropLTPointX = lLTPointX - iSearchMarginLeft;
		if (lCropLTPointX < 0)
			lCropLTPointX = 0;
		lCropLTPointY = lLTPointY - iSearchMarginTop;
		if (lCropLTPointY < 0)
			lCropLTPointY = 0;
		lCropRBPointX = lRBPointX + iSearchMarginRight;
		if (lCropRBPointX >= lImageWidth)
			lCropRBPointX = lImageWidth - 1;
		lCropRBPointY = lRBPointY + iSearchMarginBottom;
		if (lCropRBPointY >= lImageHeight)
			lCropRBPointY = lImageHeight - 1;

		HObject HMatchingImageReduced;
		CropRectangle1(HImage, &HMatchingImageReduced, lCropLTPointY, lCropLTPointX, lCropRBPointY, lCropRBPointX);

		if (bDebugSave)
		{
			WriteNccModel(HPartModelID, "c:\\DualTest\\PartCheckNccModel");
			WriteImage(HMatchingImageReduced, "bmp", 0, "C:\\DualTest\\HPartCheckCropImage");
		}

		HTuple dAngleRangeRad;
		TupleRad(Param.m_iPartCheckMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		Hlong lNoFoundNumber;
		HTuple Row, Column, Angle, Score;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Score = HTuple(0.0);

		HTuple NumLevels, AngleStart, AngleExtent, AngleStep, Metric;
		GetNccModelParams(HPartModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &Metric);

		int iNoTeachNumLevel = 0;
		iNoTeachNumLevel = NumLevels[0].L();
		if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_TEMPALTE)
			iNoTeachNumLevel = MAX_PYRAMID_LEVEL_TEMPALTE;
		else
			iNoTeachNumLevel = Param.m_iPartCheckMatchingPLevel;

		double dMinScore;
		dMinScore = 0.1;

		FindNccModel(HMatchingImageReduced,
			HPartModelID,								// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,							// Extent of the rotation angles.
			// 0.1,										// Minumum score of the instances of the model to be found
			dMinScore,									// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"true",										// Subpixel accuracy
			iNoTeachNumLevel,							// Number of pyramid levels used in the matching
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&Score);									// Score of the found instances of the model.

		HTuple HlNoFoundNumber;
		TupleLength(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();

		if (lNoFoundNumber <= 0)
		{
			if (Param.m_bPartCheckInspectExistence)
				HDefectRgn = HInspectAreaRgn;
		}
		else
		{
			double dDeltaX, dDeltaY, dAngle, dScore, dXShift, dYShift;
			HTuple HAngleDeg;
			double dAngleDeg;
			HTuple HomMat2DRotate;
			double dMatchingScoreTolerance;
			HObject HAffineTransRgn;

			dDeltaX = Column[0].D();
			dDeltaY = Row[0].D();
			dAngle = Angle[0].D();
			dScore = Score[0].D();

			*pdScore = dScore;

			dDeltaX = dDeltaX + lCropLTPointX;
			dDeltaY = dDeltaY + lCropLTPointY;

			GenEmptyObj(&HAffineTransRgn);

			VectorAngleToRigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);
			AffineTransRegion(HInspectAreaRgn, &HAffineTransRgn, HomMat2DRotate, "nearest_neighbor");

			dMatchingScoreTolerance = (double)Param.m_iPartCheckMatchingScore * 0.01;

			if (Param.m_bPartCheckInspectExistence)
			{
				if (dScore < dMatchingScoreTolerance)
					HDefectRgn = HAffineTransRgn;
			}

			dXShift = dDeltaX - dTeachAlignRefX;
			dYShift = dDeltaY - dTeachAlignRefY;
			TupleDeg(Angle, &HAngleDeg);
			dAngleDeg = HAngleDeg[0].D();

			if (Param.m_bPartCheckInspectShift)
			{
				if (fabs(dXShift) > (double)Param.m_iPartCheckShiftX || fabs(dYShift) > (double)Param.m_iPartCheckShiftY || fabs(dAngleDeg) > (double)Param.m_dPartCheckRotationAngle)
					HDefectRgn = HAffineTransRgn;
			}

			if (Param.m_bPartCheckLocalAlignUse)
			{
				*piLocalAlignDeltaX = (long)(dXShift + 0.5);
				*piLocalAlignDeltaY = (long)(dYShift + 0.5);
				*pdLocalAlignDeltaAngle = dAngle;
				*pdLocalAlignDeltaAngleFixedPointX = dTeachAlignRefX;
				*pdLocalAlignDeltaAngleFixedPointY = dTeachAlignRefY;
				*pHAlignRgn = HAffineTransRgn;
			}

			if (Param.m_bPartCheckPickerPosUse)
			{
				*piLocalAlignDeltaX = (long)(dXShift + 0.5);
				*piLocalAlignDeltaY = (long)(dYShift + 0.5);
				*pdLocalAlignDeltaAngle = dAngleDeg;

				if (fabs(dXShift) > (double)Param.m_iPartCheckShiftX || fabs(dYShift) > (double)Param.m_iPartCheckShiftY || fabs(dAngleDeg) > (double)Param.m_dPartCheckRotationAngle)
					HDefectRgn = HAffineTransRgn;
				else
					GenEmptyObj(&HDefectRgn);
			}

			*pdShiftX = dXShift;
			*pdShiftY = dYShift;
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::PartCheckAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::EdgeMeasureAlgorithm(HObject HImage, HObject *pHOtherImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, int *piFitLineXPos, int *piFitLineYPos, double *pdEdgeCenterX, double *pdEdgeCenterY, double *pdLineStartX, double *pdLineStartY, double *pdLineEndX, double *pdLineEndY)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		GenEmptyObj(pHLineFitXLD);

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(HImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

		pImageData = (BYTE*)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		// Edge Measure

		double dEdgeMeasureCenterX, dEdgeMeasureCenterY;
		int iROISizeX, iROISizeY;

		Hlong lROIRow1, lROIRow2, lROICol1, lROICol2;
		HTuple HlROIRow1, HlROIRow2, HlROICol1, HlROICol2;

		HTuple  MeasureHandle;
		HTuple RowEdge, ColumnEdge, Amplitude, Distance, Indices, Inverted;
		Hlong NoEdge, MaxAmpEdgeIndex;

		double dEgePosX, dEgePosY;
		int iEdgePosX, iEdgePosY;
		HObject RegressContour;
		HObject HSubRectRgn, HSubRgn;

		HTuple HNoEdge;

		HObject HOtherRegressContour;
		double dOtherEdgeCenterX, dOtherEdgeCenterY;
		int iOtherFitLineXPos, iOtherFitLineYPos;
		int iNewROILineMinusPos, iNewROILinePlusPos;

		GenEmptyObj(&HOtherRegressContour);
		dOtherEdgeCenterX = dOtherEdgeCenterY = -1;
		iOtherFitLineXPos = iOtherFitLineYPos = -1;
		iNewROILineMinusPos = iNewROILinePlusPos = -1;

		CString sInterpolation;

		if (Param.m_iEdgeMeasureAccuracy == INTERPOLATION_NEAREST)
			sInterpolation = "nearest_neighbor";
		else if (Param.m_iEdgeMeasureAccuracy == INTERPOLATION_BILINEAR)
			sInterpolation = "bilinear";
		else if (Param.m_iEdgeMeasureAccuracy == INTERPOLATION_BICUBIC)
			sInterpolation = "bicubic";

		double dMeasureAngleRad = 0;

		if (Param.m_bEdgeMeasureSlope)
		{
			HTuple HAngleRad;
			TupleRad(Param.m_dEdgeMeasureAngleDeg, &HAngleRad);
			dMeasureAngleRad = HAngleRad[0].D();
		}

		//	if (Param.m_bEdgeMeasureUseOther && THEAPP.m_pGFunction->ValidHImage(*pHOtherImage))		// Other Image Edge Measurement
		if (0)
		{
			SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

			lROIRow1 = HlROIRow1.L();
			lROICol1 = HlROICol1.L();
			lROIRow2 = HlROIRow2.L();
			lROICol2 = HlROICol2.L();

			dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
			dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

			iROISizeX = lROICol2 - lROICol1;
			iROISizeY = lROIRow2 - lROIRow1;

			if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2 + dMeasureAngleRad, iROISizeY / 2, iROISizeX / 2, lImageWidth,
					lImageHeight, HTuple(sInterpolation), &MeasureHandle);

				if (Param.m_iEdgeMeasurePosOther == 0)
				{
					if (Param.m_iEdgeMeasureGvOther == 0)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 1)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 2)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePosOther == 1)
				{
					if (Param.m_iEdgeMeasureGvOther == 0)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 1)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 2)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePosOther == 2)
				{
					if (Param.m_iEdgeMeasureGvOther == 0)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 1)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 2)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}
			else		// X Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 + dMeasureAngleRad, iROISizeX / 2, iROISizeY / 2, lImageWidth,
					lImageHeight, HTuple(sInterpolation), &MeasureHandle);

				if (Param.m_iEdgeMeasurePosOther == 0)
				{
					if (Param.m_iEdgeMeasureGvOther == 0)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 1)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 2)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePosOther == 1)
				{
					if (Param.m_iEdgeMeasureGvOther == 0)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 1)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 2)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePosOther == 2)
				{
					if (Param.m_iEdgeMeasureGvOther == 0)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 1)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGvOther == 2)
						MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}

			CloseMeasure(MeasureHandle);

			TupleLength(Amplitude, &HNoEdge);
			NoEdge = HNoEdge.L();
			if (NoEdge > 0)
			{
				TupleAbs(Amplitude, &Amplitude);
				TupleSortIndex(Amplitude, &Indices);
				TupleInverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();

				if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
				{
					dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
					iEdgePosY = (int)dEgePosY + Param.m_iEdgeMeasurePosOffsetOther;

					GenContourPolygonXld(&HOtherRegressContour, HTuple(iEdgePosY).TupleConcat(HTuple(iEdgePosY)), HTuple(lROICol1).TupleConcat(HTuple(lROICol2)));

					dOtherEdgeCenterX = (double)(lROICol1 + lROICol2) * 0.5;
					dOtherEdgeCenterY = dEgePosY + (double)Param.m_iEdgeMeasurePosOffsetOther;
				}
				else
				{
					dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
					iEdgePosX = (int)dEgePosX + Param.m_iEdgeMeasurePosOffsetOther;

					GenContourPolygonXld(&HOtherRegressContour, HTuple(lROIRow1).TupleConcat(HTuple(lROIRow2)), HTuple(iEdgePosX).TupleConcat(HTuple(iEdgePosX)));

					dOtherEdgeCenterX = dEgePosX + (double)Param.m_iEdgeMeasurePosOffsetOther;
					dOtherEdgeCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;
				}

				if (Param.m_bEdgeMeasureLocalAlignUse)
				{
					if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						iOtherFitLineYPos = iEdgePosY;
					}
					else
					{
						iOtherFitLineXPos = iEdgePosX;
					}
				}

				if (Param.m_bEdgeMeasureMakeROIOther)
				{
					if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						iNewROILineMinusPos = iEdgePosY - Param.m_iEdgeMeasureMakeROIRangeOther;
						iNewROILinePlusPos = iEdgePosY + Param.m_iEdgeMeasureMakeROIRangeOther;
					}
					else
					{
						iNewROILineMinusPos = iEdgePosX - Param.m_iEdgeMeasureMakeROIRangeOther;
						iNewROILinePlusPos = iEdgePosX + Param.m_iEdgeMeasureMakeROIRangeOther;
					}
				}
			}
		}		// if (Param.m_bEdgeMeasureUseOther && THEAPP.m_pGFunction->ValidHImage(*pHOtherImage))

//		if (Param.m_bEdgeMeasureMakeROIOther && Param.m_bEdgeMeasureUseOther && iNewROILineMinusPos!=-1)
		if (0)
		{
			SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

			lROIRow1 = HlROIRow1.L();
			lROICol1 = HlROICol1.L();
			lROIRow2 = HlROIRow2.L();
			lROICol2 = HlROICol2.L();

			HObject HNewRectRgn;

			if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
			{
				GenRectangle1(&HNewRectRgn, iNewROILineMinusPos, lROICol1, iNewROILinePlusPos, lROICol2);
				Intersection(HInspectAreaRgn, HNewRectRgn, &HInspectAreaRgn);
			}
			else
			{
				GenRectangle1(&HNewRectRgn, lROIRow1, iNewROILineMinusPos, lROIRow2, iNewROILinePlusPos);
				Intersection(HInspectAreaRgn, HNewRectRgn, &HInspectAreaRgn);
			}
		}


		BOOL bMeasureFail = FALSE;
		BOOL bMeasureRetry = FALSE;
		if (Param.m_bEdgeMeasureUseOther)
			bMeasureRetry = TRUE;

		if (Param.m_iEdgeMeasureOnePoint == 0)		// One Point
		{
			SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

			lROIRow1 = HlROIRow1.L();
			lROICol1 = HlROICol1.L();
			lROIRow2 = HlROIRow2.L();
			lROICol2 = HlROICol2.L();

			dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
			dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

			iROISizeX = lROICol2 - lROICol1;
			iROISizeY = lROIRow2 - lROIRow1;

			if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2 + dMeasureAngleRad, iROISizeY / 2, iROISizeX / 2, lImageWidth,
					lImageHeight, HTuple(sInterpolation), &MeasureHandle);

				if (Param.m_iEdgeMeasurePos == 0)
				{
					if (Param.m_iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePos == 1)
				{
					if (Param.m_iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePos == 2)
				{
					if (Param.m_iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}
			else		// X Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 + dMeasureAngleRad, iROISizeX / 2, iROISizeY / 2, lImageWidth,
					lImageHeight, HTuple(sInterpolation), &MeasureHandle);

				if (Param.m_iEdgeMeasurePos == 0)
				{
					if (Param.m_iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePos == 1)
				{
					if (Param.m_iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePos == 2)
				{
					if (Param.m_iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}

			CloseMeasure(MeasureHandle);

			TupleLength(Amplitude, &HNoEdge);
			NoEdge = HNoEdge.L();
			if (NoEdge <= 0)
			{
				if (bMeasureRetry)
					bMeasureFail = TRUE;
				else
					return HDefectRgn;
			}
			else
			{
				TupleAbs(Amplitude, &Amplitude);
				TupleSortIndex(Amplitude, &Indices);
				TupleInverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();

				if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
				{
					dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();	// 2025.04.08 - v2011 - LeeGW - 1 Point 시에 양방향 좌표 모두 사용
					dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
					iEdgePosX = (int)dEgePosX;
					iEdgePosY = (int)dEgePosY + Param.m_iEdgeMeasurePosOffset;

					// GenContourPolygonXld(&RegressContour, HTuple(iEdgePosY).TupleConcat(HTuple(iEdgePosY)), HTuple(lROICol1).TupleConcat(HTuple(lROICol2)));
					GenCrossContourXld(&RegressContour, iEdgePosY, iEdgePosX, 60.0, 0);	// 2025.04.08 - v2011 - LeeGW - 1 Point 시에 Cross Xld 사용

					//*pdEdgeCenterX = (double)(lROICol1 + lROICol2) * 0.5;
					*pdEdgeCenterX = dEgePosX;	// 2025.04.08 - v2011 - LeeGW - 1 Point 시에 양방향 좌표 모두 사용
					*pdEdgeCenterY = dEgePosY + (double)Param.m_iEdgeMeasurePosOffset;
				}
				else
				{
					dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
					dEgePosY = RowEdge[MaxAmpEdgeIndex].D();	// 2025.04.08 - v2011 - LeeGW - 1 Point 시에 양방향 좌표 모두 사용
					iEdgePosX = (int)dEgePosX + Param.m_iEdgeMeasurePosOffset;
					iEdgePosY = (int)dEgePosY;

					// GenContourPolygonXld(&RegressContour, HTuple(lROIRow1).TupleConcat(HTuple(lROIRow2)), HTuple(iEdgePosX).TupleConcat(HTuple(iEdgePosX)));
					GenCrossContourXld(&RegressContour, iEdgePosY, iEdgePosX, 60.0, 0);	// 2025.04.08 - v2011 - LeeGW - 1 Point 시에 Cross Xld 사용

					*pdEdgeCenterX = dEgePosX + (double)Param.m_iEdgeMeasurePosOffset;
					*pdEdgeCenterY = dEgePosY;	// 2025.04.08 - v2011 - LeeGW - 1 Point 시에 양방향 좌표 모두 사용
					//*pdEdgeCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;
				}

				ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);

				if (Param.m_bEdgeMeasureLocalAlignUse)
				{
					if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						*piFitLineYPos = iEdgePosY;
					}
					else
					{
						*piFitLineXPos = iEdgePosX;
					}
				}
			}
		}		// if (Param.m_iEdgeMeasureOnePoint == 0)
		else                    // Multi-Points
		{
			if (Param.m_iEdgeMeasureMultiPointNumber <= 0)
				Param.m_iEdgeMeasureMultiPointNumber = 2;

			SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

			lROIRow1 = HlROIRow1.L();
			lROICol1 = HlROICol1.L();
			lROIRow2 = HlROIRow2.L();
			lROICol2 = HlROICol2.L();

			int iPitch;

			if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
				iPitch = (lROICol2 - lROICol1) / Param.m_iEdgeMeasureMultiPointNumber;
			else
				iPitch = (lROIRow2 - lROIRow1) / Param.m_iEdgeMeasureMultiPointNumber;

			int iSubLTPointX, iSubLTPointY, iSubRBPointX, iSubRBPointY;
			int i;

			HTuple HEdgePosX, HEdgePosY;
			TupleGenConst(0, 0, &HEdgePosX);
			TupleGenConst(0, 0, &HEdgePosY);

			int iNoEdgeMeasure = 2;

			if (Param.m_bEdgeMeasureFixedEndPoint)
				iNoEdgeMeasure = 2;
			else
				iNoEdgeMeasure = Param.m_iEdgeMeasureMultiPointNumber;

			for (i = 0; i < iNoEdgeMeasure; i++)
			{
				if (Param.m_bEdgeMeasureFixedEndPoint)
				{
					if (i == 0)		// Left
					{
						if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
						{
							iSubLTPointX = HlROICol1;
							iSubRBPointX = HlROICol1 + Param.m_iEdgeMeasureLeftSideSize;
							iSubLTPointY = lROIRow1;
							iSubRBPointY = lROIRow2;
						}
						else        // Top
						{
							iSubLTPointX = lROICol1;
							iSubRBPointX = lROICol2;
							iSubLTPointY = lROIRow1;
							iSubRBPointY = lROIRow1 + Param.m_iEdgeMeasureLeftSideSize;
						}
					}
					else             // Right, Bottom
					{
						if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
						{
							iSubLTPointX = HlROICol2 - Param.m_iEdgeMeasureRightSideSize;
							iSubRBPointX = HlROICol2;
							iSubLTPointY = lROIRow1;
							iSubRBPointY = lROIRow2;
						}
						else
						{
							iSubLTPointX = lROICol1;
							iSubRBPointX = lROICol2;
							iSubLTPointY = lROIRow2 - Param.m_iEdgeMeasureRightSideSize;
							iSubRBPointY = lROIRow2;
						}
					}
				}
				else
				{
					if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						iSubLTPointX = HlROICol1 + i * iPitch;
						iSubRBPointX = HlROICol1 + (i + 1) * iPitch;
						iSubLTPointY = lROIRow1;
						iSubRBPointY = lROIRow2;
					}
					else
					{
						iSubLTPointX = lROICol1;
						iSubRBPointX = lROICol2;
						iSubLTPointY = lROIRow1 + i * iPitch;
						iSubRBPointY = lROIRow1 + (i + 1) * iPitch;
					}
				}

				//// 검사영역에 맞게 조정

				if (Param.m_bEdgeMeasureSlope)
				{
					GenRectangle1(&HSubRectRgn, iSubLTPointY, iSubLTPointX, iSubRBPointY, iSubRBPointX);
					Intersection(HInspectAreaRgn, HSubRectRgn, &HSubRgn);

					SmallestRectangle1(HSubRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

					iSubLTPointY = HlROIRow1.L();
					iSubLTPointX = HlROICol1.L();
					iSubRBPointY = HlROIRow2.L();
					iSubRBPointX = HlROICol2.L();
				}
				///

				dEdgeMeasureCenterX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
				dEdgeMeasureCenterY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;

				iROISizeX = iSubRBPointX - iSubLTPointX;
				iROISizeY = iSubRBPointY - iSubLTPointY;

				if (iROISizeX < 5 || iROISizeY < 5)
					continue;

				if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
				{
					GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2 + dMeasureAngleRad, iROISizeY / 2, iROISizeX / 2, lImageWidth,
						lImageHeight, HTuple(sInterpolation), &MeasureHandle);

					if (Param.m_iEdgeMeasurePos == 0)
					{
						if (Param.m_iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGv == 1)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
						else if (Param.m_iEdgeMeasureGv == 2)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}

					}
					else if (Param.m_iEdgeMeasurePos == 1)
					{
						if (Param.m_iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGv == 1)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
						else if (Param.m_iEdgeMeasureGv == 2)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
					}
					else if (Param.m_iEdgeMeasurePos == 2)
					{
						if (Param.m_iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGv == 1)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
						else if (Param.m_iEdgeMeasureGv == 2)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
					}
				}
				else		// X Dir
				{
					GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 + dMeasureAngleRad, iROISizeX / 2, iROISizeY / 2, lImageWidth,
						lImageHeight, HTuple(sInterpolation), &MeasureHandle);

					if (Param.m_iEdgeMeasurePos == 0)
					{
						if (Param.m_iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGv == 1)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
						else if (Param.m_iEdgeMeasureGv == 2)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
					}
					else if (Param.m_iEdgeMeasurePos == 1)
					{
						if (Param.m_iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGv == 1)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
						else if (Param.m_iEdgeMeasureGv == 2)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
					}
					else if (Param.m_iEdgeMeasurePos == 2)
					{
						if (Param.m_iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGv == 1)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
						else if (Param.m_iEdgeMeasureGv == 2)
						{
							if (Param.m_bEdgeMeasureFixedEndPoint && Param.m_bEdgeMeasureReverseGv && i > 0)
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else
							{
								MeasurePos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
					}
				}

				CloseMeasure(MeasureHandle);

				TupleLength(Amplitude, &HNoEdge);
				NoEdge = HNoEdge.L();
				if (NoEdge <= 0)
				{
					continue;
				}
				else
				{
					TupleAbs(Amplitude, &Amplitude);
					TupleSortIndex(Amplitude, &Indices);
					TupleInverse(Indices, &Inverted);
					MaxAmpEdgeIndex = Inverted[0].L();

					if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
						dEgePosY = dEgePosY + (double)Param.m_iEdgeMeasurePosOffset;
						dEgePosX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
					}
					else
					{
						dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
						dEgePosX = dEgePosX + Param.m_iEdgeMeasurePosOffset;
						dEgePosY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;
					}

					TupleConcat(HEdgePosX, dEgePosX, &HEdgePosX);
					TupleConcat(HEdgePosY, dEgePosY, &HEdgePosY);
				}

			}  // for (i = 0; i < iNoEdgeMeasure; i++)

			HTuple HlNoFoundNumber;
			Hlong lNoFoundNumber;
			TupleLength(HEdgePosY, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			HObject HEdgePolygonContour;

			if (lNoFoundNumber >= 2)
			{
				HTuple RowBegin, ColBegin, RowEnd, ColEnd, Nr, Nc, Dist;

				if (Param.m_bEdgeMeasureEndPoint || Param.m_bEdgeMeasureFixedEndPoint)
				{
					if (pdLineStartX != NULL)
					{
						*pdLineStartX = HEdgePosX[0].D();
						*pdLineStartY = HEdgePosY[0].D();
						*pdLineEndX = HEdgePosX[lNoFoundNumber - 1].D();
						*pdLineEndY = HEdgePosY[lNoFoundNumber - 1].D();
					}

					GenContourPolygonXld(&RegressContour, HTuple(HEdgePosY[0]).TupleConcat(HTuple(HEdgePosY[lNoFoundNumber - 1])), HTuple(HEdgePosX[0]).TupleConcat(HTuple(HEdgePosX[lNoFoundNumber - 1])));

					ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
				}
				else if (Param.m_bEdgeMeasureUseMaxMinPoint)
				{
					HTuple HMedian, HMin, HMax;
					HTuple HSelectedEdgePosY, HSelectedEdgePosX;

					TupleGenConst(0, 0, &HSelectedEdgePosX);
					TupleGenConst(0, 0, &HSelectedEdgePosY);

					if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						TupleMedian(HEdgePosY, &HMedian);

						HMin = HMedian - Param.m_iEdgeMeasurePositionRange;
						HMax = HMedian + Param.m_iEdgeMeasurePositionRange;

						for (i = 0; i < lNoFoundNumber; i++)
						{
							if (HEdgePosY[i] >= HMin && HEdgePosY[i] <= HMax)
							{
								TupleConcat(HSelectedEdgePosX, HEdgePosX[i], &HSelectedEdgePosX);
								TupleConcat(HSelectedEdgePosY, HEdgePosY[i], &HSelectedEdgePosY);
							}
						}

					}
					else		// X Dir
					{
						TupleMedian(HEdgePosX, &HMedian);

						HMin = HMedian - Param.m_iEdgeMeasurePositionRange;
						HMax = HMedian + Param.m_iEdgeMeasurePositionRange;

						for (i = 0; i < lNoFoundNumber; i++)
						{
							if (HEdgePosX[i] >= HMin && HEdgePosX[i] <= HMax)
							{
								TupleConcat(HSelectedEdgePosX, HEdgePosX[i], &HSelectedEdgePosX);
								TupleConcat(HSelectedEdgePosY, HEdgePosY[i], &HSelectedEdgePosY);
							}
						}
					}

					HTuple HlNoFoundNumber;
					Hlong lNoFoundNumber;
					TupleLength(HEdgePosY, &HlNoFoundNumber);
					lNoFoundNumber = HlNoFoundNumber.L();

					if (lNoFoundNumber >= 2)
					{
						GenContourPolygonXld(&HEdgePolygonContour, HSelectedEdgePosY, HSelectedEdgePosX);
						FitLineContourXld(HEdgePolygonContour, "regression", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

						TupleLength(RowBegin, &HlNoFoundNumber);
						lNoFoundNumber = HlNoFoundNumber.L();

						if (lNoFoundNumber > 0)
						{
							if (pdLineStartX != NULL)
							{
								*pdLineStartX = ColBegin[0].D();
								*pdLineStartY = RowBegin[0].D();
								*pdLineEndX = ColEnd[0].D();
								*pdLineEndY = RowEnd[0].D();
							}

							GenContourPolygonXld(&RegressContour, HTuple(RowBegin[0]).TupleConcat(HTuple(RowEnd[0])), HTuple(ColBegin[0]).TupleConcat(HTuple(ColEnd[0])));

							ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
						}
					}
				}
				else
				{
					GenContourPolygonXld(&HEdgePolygonContour, HEdgePosY, HEdgePosX);
					FitLineContourXld(HEdgePolygonContour, "regression", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

					TupleLength(RowBegin, &HlNoFoundNumber);
					lNoFoundNumber = HlNoFoundNumber.L();

					if (lNoFoundNumber > 0)
					{
						if (pdLineStartX != NULL)
						{
							*pdLineStartX = ColBegin[0].D();
							*pdLineStartY = RowBegin[0].D();
							*pdLineEndX = ColEnd[0].D();
							*pdLineEndY = RowEnd[0].D();
						}

						GenContourPolygonXld(&RegressContour, HTuple(RowBegin[0]).TupleConcat(HTuple(RowEnd[0])), HTuple(ColBegin[0]).TupleConcat(HTuple(ColEnd[0])));

						ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
					}
				}
			} // if (lNoFoundNumber >= 2)
			else
			{
				if (bMeasureRetry)
					bMeasureFail = TRUE;
			}

		}   // else

		if (bMeasureFail)
		{

			if (Param.m_iEdgeMeasureOnePoint == 0)		// One Point
			{
				SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

				lROIRow1 = HlROIRow1.L();
				lROICol1 = HlROICol1.L();
				lROIRow2 = HlROIRow2.L();
				lROICol2 = HlROICol2.L();

				dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
				dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

				iROISizeX = lROICol2 - lROICol1;
				iROISizeY = lROIRow2 - lROIRow1;

				if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
				{
					GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2 + dMeasureAngleRad, iROISizeY / 2, iROISizeX / 2, lImageWidth,
						lImageHeight, HTuple(sInterpolation), &MeasureHandle);

					if (Param.m_iEdgeMeasurePosOther == 0)
					{
						if (Param.m_iEdgeMeasureGvOther == 0)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 1)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 2)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iEdgeMeasurePosOther == 1)
					{
						if (Param.m_iEdgeMeasureGvOther == 0)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 1)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 2)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iEdgeMeasurePosOther == 2)
					{
						if (Param.m_iEdgeMeasureGvOther == 0)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 1)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 2)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}
				else		// X Dir
				{
					GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 + dMeasureAngleRad, iROISizeX / 2, iROISizeY / 2, lImageWidth,
						lImageHeight, HTuple(sInterpolation), &MeasureHandle);

					if (Param.m_iEdgeMeasurePosOther == 0)
					{
						if (Param.m_iEdgeMeasureGvOther == 0)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 1)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 2)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iEdgeMeasurePosOther == 1)
					{
						if (Param.m_iEdgeMeasureGvOther == 0)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 1)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 2)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iEdgeMeasurePosOther == 2)
					{
						if (Param.m_iEdgeMeasureGvOther == 0)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 1)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iEdgeMeasureGvOther == 2)
							MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}

				CloseMeasure(MeasureHandle);

				TupleLength(Amplitude, &HNoEdge);
				NoEdge = HNoEdge.L();
				if (NoEdge <= 0)
				{
					return HDefectRgn;
				}
				else
				{
					TupleAbs(Amplitude, &Amplitude);
					TupleSortIndex(Amplitude, &Indices);
					TupleInverse(Indices, &Inverted);
					MaxAmpEdgeIndex = Inverted[0].L();

					if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
						iEdgePosY = (int)dEgePosY + Param.m_iEdgeMeasurePosOffsetOther;

						GenContourPolygonXld(&RegressContour, HTuple(iEdgePosY).TupleConcat(HTuple(iEdgePosY)), HTuple(lROICol1).TupleConcat(HTuple(lROICol2)));

						*pdEdgeCenterX = (double)(lROICol1 + lROICol2) * 0.5;
						*pdEdgeCenterY = dEgePosY + (double)Param.m_iEdgeMeasurePosOffsetOther;
					}
					else
					{
						dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
						iEdgePosX = (int)dEgePosX + Param.m_iEdgeMeasurePosOffsetOther;

						GenContourPolygonXld(&RegressContour, HTuple(lROIRow1).TupleConcat(HTuple(lROIRow2)), HTuple(iEdgePosX).TupleConcat(HTuple(iEdgePosX)));

						*pdEdgeCenterX = dEgePosX + (double)Param.m_iEdgeMeasurePosOffsetOther;
						*pdEdgeCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;
					}

					ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);

					if (Param.m_bEdgeMeasureLocalAlignUse)
					{
						if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
						{
							*piFitLineYPos = iEdgePosY;
						}
						else
						{
							*piFitLineXPos = iEdgePosX;
						}
					}
				}
			}		// if (Param.m_iEdgeMeasureOnePoint == 0)
			else                    // Multi-Points
			{
				if (Param.m_iEdgeMeasureMultiPointNumber <= 0)
					Param.m_iEdgeMeasureMultiPointNumber = 2;

				SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

				lROIRow1 = HlROIRow1.L();
				lROICol1 = HlROICol1.L();
				lROIRow2 = HlROIRow2.L();
				lROICol2 = HlROICol2.L();

				int iPitch;

				if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					iPitch = (lROICol2 - lROICol1) / Param.m_iEdgeMeasureMultiPointNumber;
				else
					iPitch = (lROIRow2 - lROIRow1) / Param.m_iEdgeMeasureMultiPointNumber;

				int iSubLTPointX, iSubLTPointY, iSubRBPointX, iSubRBPointY;
				int i;

				HTuple HEdgePosX, HEdgePosY;
				TupleGenConst(0, 0, &HEdgePosX);
				TupleGenConst(0, 0, &HEdgePosY);

				int iNoEdgeMeasure = 2;

				if (Param.m_bEdgeMeasureFixedEndPoint)
					iNoEdgeMeasure = 2;
				else
					iNoEdgeMeasure = Param.m_iEdgeMeasureMultiPointNumber;

				for (i = 0; i < iNoEdgeMeasure; i++)
				{
					if (Param.m_bEdgeMeasureFixedEndPoint)
					{
						if (i == 0)		// Left
						{
							if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
							{
								iSubLTPointX = HlROICol1;
								iSubRBPointX = HlROICol1 + Param.m_iEdgeMeasureLeftSideSize;
								iSubLTPointY = lROIRow1;
								iSubRBPointY = lROIRow2;
							}
							else        // Top
							{
								iSubLTPointX = lROICol1;
								iSubRBPointX = lROICol2;
								iSubLTPointY = lROIRow1;
								iSubRBPointY = lROIRow1 + Param.m_iEdgeMeasureLeftSideSize;
							}
						}
						else             // Right, Bottom
						{
							if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
							{
								iSubLTPointX = HlROICol2 - Param.m_iEdgeMeasureRightSideSize;
								iSubRBPointX = HlROICol2;
								iSubLTPointY = lROIRow1;
								iSubRBPointY = lROIRow2;
							}
							else
							{
								iSubLTPointX = lROICol1;
								iSubRBPointX = lROICol2;
								iSubLTPointY = lROIRow2 - Param.m_iEdgeMeasureRightSideSize;
								iSubRBPointY = lROIRow2;
							}
						}
					}
					else
					{
						if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
						{
							iSubLTPointX = HlROICol1 + i * iPitch;
							iSubRBPointX = HlROICol1 + (i + 1) * iPitch;
							iSubLTPointY = lROIRow1;
							iSubRBPointY = lROIRow2;
						}
						else
						{
							iSubLTPointX = lROICol1;
							iSubRBPointX = lROICol2;
							iSubLTPointY = lROIRow1 + i * iPitch;
							iSubRBPointY = lROIRow1 + (i + 1) * iPitch;
						}
					}

					//// 검사영역에 맞게 조정

					if (Param.m_bEdgeMeasureSlope)
					{
						GenRectangle1(&HSubRectRgn, iSubLTPointY, iSubLTPointX, iSubRBPointY, iSubRBPointX);
						Intersection(HInspectAreaRgn, HSubRectRgn, &HSubRgn);

						SmallestRectangle1(HSubRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

						iSubLTPointY = HlROIRow1.L();
						iSubLTPointX = HlROICol1.L();
						iSubRBPointY = HlROIRow2.L();
						iSubRBPointX = HlROICol2.L();
					}
					///

					dEdgeMeasureCenterX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
					dEdgeMeasureCenterY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;

					iROISizeX = iSubRBPointX - iSubLTPointX;
					iROISizeY = iSubRBPointY - iSubLTPointY;

					if (iROISizeX < 5 || iROISizeY < 5)
						continue;

					if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2 + dMeasureAngleRad, iROISizeY / 2, iROISizeX / 2, lImageWidth,
							lImageHeight, HTuple(sInterpolation), &MeasureHandle);

						if (Param.m_iEdgeMeasurePosOther == 0)
						{
							if (Param.m_iEdgeMeasureGvOther == 0)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 1)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 2)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (Param.m_iEdgeMeasurePosOther == 1)
						{
							if (Param.m_iEdgeMeasureGvOther == 0)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 1)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 2)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (Param.m_iEdgeMeasurePosOther == 2)
						{
							if (Param.m_iEdgeMeasureGvOther == 0)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 1)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 2)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
					}
					else		// X Dir
					{
						GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 + dMeasureAngleRad, iROISizeX / 2, iROISizeY / 2, lImageWidth,
							lImageHeight, HTuple(sInterpolation), &MeasureHandle);

						if (Param.m_iEdgeMeasurePosOther == 0)
						{
							if (Param.m_iEdgeMeasureGvOther == 0)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 1)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 2)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (Param.m_iEdgeMeasurePosOther == 1)
						{
							if (Param.m_iEdgeMeasureGvOther == 0)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 1)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 2)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (Param.m_iEdgeMeasurePosOther == 2)
						{
							if (Param.m_iEdgeMeasureGvOther == 0)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "all",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 1)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "negative",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iEdgeMeasureGvOther == 2)
								MeasurePos(*pHOtherImage, MeasureHandle, Param.m_dEdgeMeasureSmFactorOther, (double)Param.m_iEdgeMeasureEdgeStrOther, "positive",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
					}

					CloseMeasure(MeasureHandle);

					TupleLength(Amplitude, &HNoEdge);
					NoEdge = HNoEdge.L();
					if (NoEdge <= 0)
					{
						continue;
					}
					else
					{
						TupleAbs(Amplitude, &Amplitude);
						TupleSortIndex(Amplitude, &Indices);
						TupleInverse(Indices, &Inverted);
						MaxAmpEdgeIndex = Inverted[0].L();

						if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
						{
							dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
							dEgePosY = dEgePosY + (double)Param.m_iEdgeMeasurePosOffsetOther;
							dEgePosX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
						}
						else
						{
							dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
							dEgePosX = dEgePosX + Param.m_iEdgeMeasurePosOffsetOther;
							dEgePosY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;
						}

						TupleConcat(HEdgePosX, dEgePosX, &HEdgePosX);
						TupleConcat(HEdgePosY, dEgePosY, &HEdgePosY);
					}

				}  // for (i = 0; i < iNoEdgeMeasure; i++)

				HTuple HlNoFoundNumber;
				Hlong lNoFoundNumber;
				TupleLength(HEdgePosY, &HlNoFoundNumber);
				lNoFoundNumber = HlNoFoundNumber.L();

				HObject HEdgePolygonContour;

				if (lNoFoundNumber >= 2)
				{
					HTuple RowBegin, ColBegin, RowEnd, ColEnd, Nr, Nc, Dist;

					if (Param.m_bEdgeMeasureEndPoint || Param.m_bEdgeMeasureFixedEndPoint)
					{
						if (pdLineStartX != NULL)
						{
							*pdLineStartX = HEdgePosX[0].D();
							*pdLineStartY = HEdgePosY[0].D();
							*pdLineEndX = HEdgePosX[lNoFoundNumber - 1].D();
							*pdLineEndY = HEdgePosY[lNoFoundNumber - 1].D();
						}

						GenContourPolygonXld(&RegressContour, HTuple(HEdgePosY[0]).TupleConcat(HTuple(HEdgePosY[lNoFoundNumber - 1])), HTuple(HEdgePosX[0]).TupleConcat(HTuple(HEdgePosX[lNoFoundNumber - 1])));

						ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
					}
					else if (Param.m_bEdgeMeasureUseMaxMinPoint)
					{
						HTuple HMedian, HMin, HMax;
						HTuple HSelectedEdgePosY, HSelectedEdgePosX;

						TupleGenConst(0, 0, &HSelectedEdgePosX);
						TupleGenConst(0, 0, &HSelectedEdgePosY);

						if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
						{
							TupleMedian(HEdgePosY, &HMedian);

							HMin = HMedian - Param.m_iEdgeMeasurePositionRange;
							HMax = HMedian + Param.m_iEdgeMeasurePositionRange;

							for (i = 0; i < lNoFoundNumber; i++)
							{
								if (HEdgePosY[i] >= HMin && HEdgePosY[i] <= HMax)
								{
									TupleConcat(HSelectedEdgePosX, HEdgePosX[i], &HSelectedEdgePosX);
									TupleConcat(HSelectedEdgePosY, HEdgePosY[i], &HSelectedEdgePosY);
								}
							}

						}
						else		// X Dir
						{
							TupleMedian(HEdgePosX, &HMedian);

							HMin = HMedian - Param.m_iEdgeMeasurePositionRange;
							HMax = HMedian + Param.m_iEdgeMeasurePositionRange;

							for (i = 0; i < lNoFoundNumber; i++)
							{
								if (HEdgePosX[i] >= HMin && HEdgePosX[i] <= HMax)
								{
									TupleConcat(HSelectedEdgePosX, HEdgePosX[i], &HSelectedEdgePosX);
									TupleConcat(HSelectedEdgePosY, HEdgePosY[i], &HSelectedEdgePosY);
								}
							}
						}

						HTuple HlNoFoundNumber;
						Hlong lNoFoundNumber;
						TupleLength(HEdgePosY, &HlNoFoundNumber);
						lNoFoundNumber = HlNoFoundNumber.L();

						if (lNoFoundNumber >= 2)
						{
							GenContourPolygonXld(&HEdgePolygonContour, HSelectedEdgePosY, HSelectedEdgePosX);
							FitLineContourXld(HEdgePolygonContour, "regression", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

							TupleLength(RowBegin, &HlNoFoundNumber);
							lNoFoundNumber = HlNoFoundNumber.L();

							if (lNoFoundNumber > 0)
							{
								if (pdLineStartX != NULL)
								{
									*pdLineStartX = ColBegin[0].D();
									*pdLineStartY = RowBegin[0].D();
									*pdLineEndX = ColEnd[0].D();
									*pdLineEndY = RowEnd[0].D();
								}

								GenContourPolygonXld(&RegressContour, HTuple(RowBegin[0]).TupleConcat(HTuple(RowEnd[0])), HTuple(ColBegin[0]).TupleConcat(HTuple(ColEnd[0])));

								ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
							}
						}
					}
					else
					{
						GenContourPolygonXld(&HEdgePolygonContour, HEdgePosY, HEdgePosX);
						FitLineContourXld(HEdgePolygonContour, "regression", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

						TupleLength(RowBegin, &HlNoFoundNumber);
						lNoFoundNumber = HlNoFoundNumber.L();

						if (lNoFoundNumber > 0)
						{
							if (pdLineStartX != NULL)
							{
								*pdLineStartX = ColBegin[0].D();
								*pdLineStartY = RowBegin[0].D();
								*pdLineEndX = ColEnd[0].D();
								*pdLineEndY = RowEnd[0].D();
							}

							GenContourPolygonXld(&RegressContour, HTuple(RowBegin[0]).TupleConcat(HTuple(RowEnd[0])), HTuple(ColBegin[0]).TupleConcat(HTuple(ColEnd[0])));

							ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
						}
					}
				} // if (lNoFoundNumber >= 2)

			}   // else

		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::EdgeMeasureAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::DistanceMeasureAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, double *pdDistanceMeasureValueUm, int *iGapUpperPointX, int *iGapUpperPointY, int *iGapLowerPointX, int *iGapLowerPointY)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		GenEmptyObj(pHLineFitXLD);

		*pdDistanceMeasureValueUm = -1;

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(HImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

		pImageData = (BYTE*)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn.reg");
		}

		HObject HInspectAreaConnRgn, HUpperInspectAreaRgn, HLowerInspectAreaRgn;

		Hlong lNoROI;
		HTuple HNoROI;

		Connection(HInspectAreaRgn, &HInspectAreaConnRgn);
		CountObj(HInspectAreaConnRgn, &HNoROI);
		lNoROI = HNoROI.L();

		if (Param.m_iDistanceMeasureROIType == 1)
		{
			if (lNoROI == 2)
			{
				SortRegion(HInspectAreaConnRgn, &HInspectAreaConnRgn, "upper_left", "true", "row");
				SelectObj(HInspectAreaConnRgn, &HUpperInspectAreaRgn, 1);
				SelectObj(HInspectAreaConnRgn, &HLowerInspectAreaRgn, 2);
			}
			else
				return HDefectRgn;
		}
		else if (Param.m_iDistanceMeasureROIType == 0)
		{
			if (lNoROI != 1)
			{
				return HDefectRgn;
			}
		}

		if (Param.m_iDistanceMeasureUsageType == 1)
		{
			if (lNoROI != 1)
			{
				return HDefectRgn;
			}
		}

		double dEdgeMeasureCenterX, dEdgeMeasureCenterY;
		int iROISizeX, iROISizeY;

		Hlong lROIRow1, lROIRow2, lROICol1, lROICol2;
		HTuple HlROIRow1, HlROIRow2, HlROICol1, HlROICol2;

		if (Param.m_iDistanceMeasureROIType == 0)
			SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);
		else
			SmallestRectangle1(HUpperInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

		lROIRow1 = HlROIRow1.L();
		lROICol1 = HlROICol1.L();
		lROIRow2 = HlROIRow2.L();
		lROICol2 = HlROICol2.L();

		dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
		dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

		iROISizeX = lROICol2 - lROICol1;
		iROISizeY = lROIRow2 - lROIRow1;

		DPOINT dEdgePosY_LT, dEdgePosY_RB;

		// Edge Measure (Upper)

		HTuple  MeasureHandle;
		HTuple RowEdge, ColumnEdge, Amplitude, Distance, Indices, Inverted;
		Hlong NoEdge, MaxAmpEdgeIndex;

		GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
			lImageHeight, "nearest_neighbor", &MeasureHandle);

		if (Param.m_iDistanceMeasureUpperPos == 0)
		{
			if (Param.m_iDistanceMeasureUpperGv == 0)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureUpperSmFactor, (double)Param.m_iDistanceMeasureUpperEdgeStr, "all",
					"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureUpperGv == 1)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureUpperSmFactor, (double)Param.m_iDistanceMeasureUpperEdgeStr, "negative",
					"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureUpperGv == 2)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureUpperSmFactor, (double)Param.m_iDistanceMeasureUpperEdgeStr, "positive",
					"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
		}
		else if (Param.m_iDistanceMeasureUpperPos == 1)
		{
			if (Param.m_iDistanceMeasureUpperGv == 0)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureUpperSmFactor, (double)Param.m_iDistanceMeasureUpperEdgeStr, "all",
					"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureUpperGv == 1)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureUpperSmFactor, (double)Param.m_iDistanceMeasureUpperEdgeStr, "negative",
					"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureUpperGv == 2)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureUpperSmFactor, (double)Param.m_iDistanceMeasureUpperEdgeStr, "positive",
					"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
		}
		else if (Param.m_iDistanceMeasureUpperPos == 2)
		{
			if (Param.m_iDistanceMeasureUpperGv == 0)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureUpperSmFactor, (double)Param.m_iDistanceMeasureUpperEdgeStr, "all",
					"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureUpperGv == 1)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureUpperSmFactor, (double)Param.m_iDistanceMeasureUpperEdgeStr, "negative",
					"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureUpperGv == 2)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureUpperSmFactor, (double)Param.m_iDistanceMeasureUpperEdgeStr, "positive",
					"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
		}

		CloseMeasure(MeasureHandle);

		double dUpperEdgePosY, dLowerEdgePosY;
		HObject HUpperRegressContour, HLowerRegressContour;

		HTuple HNoEdge;
		TupleLength(Amplitude, &HNoEdge);
		NoEdge = HNoEdge.L();
		if (NoEdge <= 0)
		{
			return HDefectRgn;
		}
		else
		{
			TupleAbs(Amplitude, &Amplitude);
			TupleSortIndex(Amplitude, &Indices);
			TupleInverse(Indices, &Inverted);
			MaxAmpEdgeIndex = Inverted[0].L();

			dUpperEdgePosY = RowEdge[MaxAmpEdgeIndex].D() + (double)Param.m_iDistanceMeasureUpperPosOffset;
			GenContourPolygonXld(&HUpperRegressContour, HTuple(dUpperEdgePosY).TupleConcat(HTuple(dUpperEdgePosY)), HTuple(lROICol1).TupleConcat(HTuple(lROICol2)));

			dEdgePosY_LT.x = lROICol1;
			dEdgePosY_LT.y = dUpperEdgePosY;
		}

		// Edge Measure (Lower)

		if (Param.m_iDistanceMeasureROIType == 0)
			SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);
		else
			SmallestRectangle1(HLowerInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

		lROIRow1 = HlROIRow1.L();
		lROICol1 = HlROICol1.L();
		lROIRow2 = HlROIRow2.L();
		lROICol2 = HlROICol2.L();

		dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
		dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

		iROISizeX = lROICol2 - lROICol1;
		iROISizeY = lROIRow2 - lROIRow1;

		GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
			lImageHeight, "nearest_neighbor", &MeasureHandle);

		if (Param.m_iDistanceMeasureLowerPos == 0)
		{
			if (Param.m_iDistanceMeasureLowerGv == 0)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureLowerSmFactor, (double)Param.m_iDistanceMeasureLowerEdgeStr, "all",
					"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureLowerGv == 1)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureLowerSmFactor, (double)Param.m_iDistanceMeasureLowerEdgeStr, "negative",
					"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureLowerGv == 2)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureLowerSmFactor, (double)Param.m_iDistanceMeasureLowerEdgeStr, "positive",
					"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
		}
		else if (Param.m_iDistanceMeasureLowerPos == 1)
		{
			if (Param.m_iDistanceMeasureLowerGv == 0)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureLowerSmFactor, (double)Param.m_iDistanceMeasureLowerEdgeStr, "all",
					"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureLowerGv == 1)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureLowerSmFactor, (double)Param.m_iDistanceMeasureLowerEdgeStr, "negative",
					"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureLowerGv == 2)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureLowerSmFactor, (double)Param.m_iDistanceMeasureLowerEdgeStr, "positive",
					"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
		}
		else if (Param.m_iDistanceMeasureLowerPos == 2)
		{
			if (Param.m_iDistanceMeasureLowerGv == 0)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureLowerSmFactor, (double)Param.m_iDistanceMeasureLowerEdgeStr, "all",
					"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureLowerGv == 1)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureLowerSmFactor, (double)Param.m_iDistanceMeasureLowerEdgeStr, "negative",
					"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
			else if (Param.m_iDistanceMeasureLowerGv == 2)
				MeasurePos(HImage, MeasureHandle, Param.m_dDistanceMeasureLowerSmFactor, (double)Param.m_iDistanceMeasureLowerEdgeStr, "positive",
					"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
		}

		CloseMeasure(MeasureHandle);

		TupleLength(Amplitude, &HNoEdge);
		NoEdge = HNoEdge.L();
		if (NoEdge <= 0)
		{
			return HDefectRgn;
		}
		else
		{
			TupleAbs(Amplitude, &Amplitude);
			TupleSortIndex(Amplitude, &Indices);
			TupleInverse(Indices, &Inverted);
			MaxAmpEdgeIndex = Inverted[0].L();

			dLowerEdgePosY = RowEdge[MaxAmpEdgeIndex].D() + (double)Param.m_iDistanceMeasureLowerPosOffset;
			GenContourPolygonXld(&HLowerRegressContour, HTuple(dLowerEdgePosY).TupleConcat(HTuple(dLowerEdgePosY)), HTuple(lROICol1).TupleConcat(HTuple(lROICol2)));

			dEdgePosY_RB.x = lROICol2;
			dEdgePosY_RB.y = dLowerEdgePosY;
		}

		*pdDistanceMeasureValueUm = fabs(dLowerEdgePosY - dUpperEdgePosY) * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize();

		HTuple HEdgePoint_X, HEdgePoint_Y;
		TupleGenConst(0, 0, &HEdgePoint_X);
		TupleGenConst(0, 0, &HEdgePoint_Y);

		if (Param.m_iDistanceMeasureUsageType == 1)
		{
			TupleConcat(HEdgePoint_X, HTuple(dEdgePosY_LT.x), &HEdgePoint_X);
			TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_LT.y), &HEdgePoint_Y);
			TupleConcat(HEdgePoint_X, HTuple(dEdgePosY_RB.x), &HEdgePoint_X);
			TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_LT.y), &HEdgePoint_Y);
			TupleConcat(HEdgePoint_X, HTuple(dEdgePosY_RB.x), &HEdgePoint_X);
			TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_RB.y), &HEdgePoint_Y);
			TupleConcat(HEdgePoint_X, HTuple(dEdgePosY_LT.x), &HEdgePoint_X);
			TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_RB.y), &HEdgePoint_Y);
			TupleConcat(HEdgePoint_X, HTuple(dEdgePosY_LT.x), &HEdgePoint_X);
			TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_LT.y), &HEdgePoint_Y);

			GenContourPolygonXld(pHLineFitXLD, HEdgePoint_Y, HEdgePoint_X);

			GenRegionContourXld(*pHLineFitXLD, &HDefectRgn, "filled");
		}
		else
		{
			double dLineXPos;
			dLineXPos = (double)(lROICol1 + lROICol2) * 0.5;

			TupleConcat(HEdgePoint_X, HTuple(dLineXPos), &HEdgePoint_X);
			TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_LT.y), &HEdgePoint_Y);
			TupleConcat(HEdgePoint_X, HTuple(dLineXPos), &HEdgePoint_X);
			TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_RB.y), &HEdgePoint_Y);

			GenContourPolygonXld(pHLineFitXLD, HEdgePoint_Y, HEdgePoint_X);

			*iGapUpperPointX = (int)dLineXPos;
			*iGapLowerPointX = (int)dLineXPos;
			*iGapUpperPointY = (int)dEdgePosY_LT.y;
			*iGapLowerPointY = (int)dEdgePosY_RB.y;

			if (*pdDistanceMeasureValueUm > Param.m_dDistanceMeasureGapInspectionUpper || *pdDistanceMeasureValueUm < Param.m_dDistanceMeasureGapInspectionLower)
				GenRegionContourXld(*pHLineFitXLD, &HDefectRgn, "filled");
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::DistanceMeasureAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::GapMeasureAlgorithm(HObject *pHImage1, HObject *pHImage2, HObject *pHImage3, HObject *pHImage4, HObject *pHImageRetry, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, double *pdGapMeasureValueUm, int *iGapUpperPointX, int *iGapUpperPointY, int *iGapLowerPointX, int *iGapLowerPointY)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		GenEmptyObj(pHLineFitXLD);

		int i, j;

		*pdGapMeasureValueUm = 0;

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(*pHImage1, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

		pImageData = (BYTE*)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (Param.m_bGapMeasureUpperLine == TRUE && Param.m_bGapMeasureLowerLine == TRUE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(*pHImage1, "bmp", 0, "c:\\DualTest\\InspectImage_1");
			WriteImage(*pHImage2, "bmp", 0, "c:\\DualTest\\InspectImage_2");
			WriteObject(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		BOOL bUseGapMeasureRetry = FALSE;
		if ((Param.m_bGapMeasureUpperLine || Param.m_bGapMeasureLowerLine) && Param.m_bGapMeasureRetry)
			bUseGapMeasureRetry = TRUE;

		Hlong lROIRow1, lROIRow2, lROICol1, lROICol2;
		HTuple HlROIRow1, HlROIRow2, HlROICol1, HlROICol2;

		SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

		lROIRow1 = HlROIRow1.L();
		lROICol1 = HlROICol1.L();
		lROIRow2 = HlROIRow2.L();
		lROICol2 = HlROICol2.L();

		HObject HEdgeMeasureAlgoDefectRgn;
		int iEdgeMeasureAlignPosX, iEdgeMeasureAlignPosY;
		double dEdgeCenterX, dEdgeCenterY;
		double dEdgeLineStartX, dEdgeLineStartY, dEdgeLineEndX, dEdgeLineEndY;

		iEdgeMeasureAlignPosX = iEdgeMeasureAlignPosY = INVALID_ALIGN_RESULT;
		dEdgeLineStartX = dEdgeLineStartY = dEdgeLineEndX = dEdgeLineEndY = INVALID_ALIGN_RESULT;

		HObject HPreProcessImage, HResultXLD;
		HTuple HDistance;
		double dDistance;
		HObject HCutRgn;
		double dCutPoint;

		if (Param.m_bGapMeasureUpperLine)
		{
			dEdgeCenterX = dEdgeCenterY = -1;

			HEdgeMeasureAlgoDefectRgn = EdgeMeasureAlgorithm(*pHImage1, pHImage4, HInspectAreaRgn, Param, &HResultXLD, &iEdgeMeasureAlignPosX, &iEdgeMeasureAlignPosY, &dEdgeCenterX, &dEdgeCenterY, &dEdgeLineStartX, &dEdgeLineStartY, &dEdgeLineEndX, &dEdgeLineEndY);

			if (Param.m_iEdgeMeasureOnePoint == 1)		// Line
			{
				if (dEdgeLineStartX == INVALID_ALIGN_RESULT)	// Fail
				{
					*pdGapMeasureValueUm = 0;
					Union1(HInspectAreaRgn, &HDefectRgn);

					return HDefectRgn;
				}
			}
			else  // Point
			{
				if (dEdgeCenterX == -1)	// Fail
				{
					*pdGapMeasureValueUm = 0;
					Union1(HInspectAreaRgn, &HDefectRgn);

					return HDefectRgn;
				}

				if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
				{
					dEdgeLineStartX = dEdgeCenterX - ONE_POINT_EDGE_EXTEND_PXL;
					dEdgeLineStartY = dEdgeCenterY;
					dEdgeLineEndX = dEdgeCenterX + ONE_POINT_EDGE_EXTEND_PXL;
					dEdgeLineEndY = dEdgeCenterY;
				}
				else
				{
					dEdgeLineStartX = dEdgeCenterX;
					dEdgeLineStartY = dEdgeCenterY - ONE_POINT_EDGE_EXTEND_PXL;
					dEdgeLineEndX = dEdgeCenterX;
					dEdgeLineEndY = dEdgeCenterY + ONE_POINT_EDGE_EXTEND_PXL;
				}
			}

			if (Param.m_iGapMeasureDir == 1)		// Y Dir
			{
				if (dEdgeLineStartY >= dEdgeLineEndY)
					dCutPoint = dEdgeLineEndY;
				else
					dCutPoint = dEdgeLineStartY;

				GenRectangle1(&HCutRgn, lROIRow1, lROICol1, (Hlong)dCutPoint, lROICol2);
			}
			else
			{
				if (dEdgeLineStartX >= dEdgeLineEndX)
					dCutPoint = dEdgeLineEndX;
				else
					dCutPoint = dEdgeLineStartX;

				GenRectangle1(&HCutRgn, lROIRow1, lROICol1, lROIRow2, (Hlong)dCutPoint);
			}

			Difference(HInspectAreaRgn, HCutRgn, &HInspectAreaRgn);
		}

		if (Param.m_bGapMeasureLowerLine)
		{
			dEdgeCenterX = dEdgeCenterY = -1;

			HEdgeMeasureAlgoDefectRgn = EdgeMeasureAlgorithm(*pHImage2, pHImage4, HInspectAreaRgn, Param, &HResultXLD, &iEdgeMeasureAlignPosX, &iEdgeMeasureAlignPosY, &dEdgeCenterX, &dEdgeCenterY, &dEdgeLineStartX, &dEdgeLineStartY, &dEdgeLineEndX, &dEdgeLineEndY);

			if (Param.m_iEdgeMeasureOnePoint == 1)		// Line
			{
				if (dEdgeLineStartX == INVALID_ALIGN_RESULT)	// Fail
				{
					*pdGapMeasureValueUm = 0;
					Union1(HInspectAreaRgn, &HDefectRgn);

					return HDefectRgn;
				}
			}
			else  // Point
			{
				if (dEdgeCenterX == -1)	// Fail
				{
					*pdGapMeasureValueUm = 0;
					Union1(HInspectAreaRgn, &HDefectRgn);

					return HDefectRgn;
				}

				if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
				{
					dEdgeLineStartX = dEdgeCenterX - ONE_POINT_EDGE_EXTEND_PXL;
					dEdgeLineStartY = dEdgeCenterY;
					dEdgeLineEndX = dEdgeCenterX + ONE_POINT_EDGE_EXTEND_PXL;
					dEdgeLineEndY = dEdgeCenterY;
				}
				else
				{
					dEdgeLineStartX = dEdgeCenterX;
					dEdgeLineStartY = dEdgeCenterY - ONE_POINT_EDGE_EXTEND_PXL;
					dEdgeLineEndX = dEdgeCenterX;
					dEdgeLineEndY = dEdgeCenterY + ONE_POINT_EDGE_EXTEND_PXL;
				}
			}

			if (Param.m_iGapMeasureDir == 1)		// Y Dir
			{
				if (dEdgeLineStartY <= dEdgeLineEndY)
					dCutPoint = dEdgeLineEndY;
				else
					dCutPoint = dEdgeLineStartY;

				GenRectangle1(&HCutRgn, (Hlong)dCutPoint, lROICol1, lROIRow2, lROICol2);
			}
			else
			{
				if (dEdgeLineStartX <= dEdgeLineEndX)
					dCutPoint = dEdgeLineEndX;
				else
					dCutPoint = dEdgeLineStartX;

				GenRectangle1(&HCutRgn, lROIRow1, (Hlong)dCutPoint, lROIRow2, lROICol2);
			}

			Difference(HInspectAreaRgn, HCutRgn, &HInspectAreaRgn);
		}

		double dEdgeMeasureCenterX, dEdgeMeasureCenterY;
		int iROISizeX, iROISizeY;

		double dEgePosX, dEgePosY;
		int iEdgePosX, iEdgePosY;

		HTuple  MeasureHandle;
		HTuple RowEdge, ColumnEdge, Amplitude, Distance, Indices, Inverted;
		Hlong NoEdge, MaxAmpEdgeIndex;

		double dUpperEdgePosX, dLowerEdgePosX, dUpperEdgePosY, dLowerEdgePosY;
		HObject HUpperRegressContour, HLowerRegressContour;

		HTuple HNoEdge;
		HObject HGapMeasureLineXLD;

		double dAvgGapMeasureValueUm = 0;

		SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

		lROIRow1 = HlROIRow1.L();
		lROICol1 = HlROICol1.L();
		lROIRow2 = HlROIRow2.L();
		lROICol2 = HlROICol2.L();

		int iPitch;

		if (Param.m_iGapMeasureDir == 1)		// Y Dir
			iPitch = (lROICol2 - lROICol1) / Param.m_iGapMeasurePointNumber;
		else
			iPitch = (lROIRow2 - lROIRow1) / Param.m_iGapMeasurePointNumber;

		int iSubLTPointX, iSubLTPointY, iSubRBPointX, iSubRBPointY;

		HObject HSubRectRgn, HSubRgn;

		int iNoEdgeMeasure = 1;
		iNoEdgeMeasure = Param.m_iGapMeasurePointNumber;

		double dGapMeasureValueUm, dMaxGapMeasureValueUm, dMinGapMeasureValueUm;
		BOOL bUseDiffImage = FALSE;
		HObject *pHCurMeasureImage;

		dMaxGapMeasureValueUm = -1;
		dMinGapMeasureValueUm = 100000000;

		HTuple HEdgePoint_X, HEdgePoint_Y;
		TupleGenConst(0, 0, &HEdgePoint_X);
		TupleGenConst(0, 0, &HEdgePoint_Y);

		for (i = 0; i < iNoEdgeMeasure; i++)
		{
			if (Param.m_iGapMeasureDir == 1)		// Y Dir
			{
				iSubLTPointX = HlROICol1 + i * iPitch;
				iSubRBPointX = HlROICol1 + (i + 1) * iPitch;
				iSubLTPointY = lROIRow1;
				iSubRBPointY = lROIRow2;

				if (Param.m_bGapMeasureEndRemove)
				{
					if (iSubLTPointX >= HlROICol1 && iSubRBPointX <= (HlROICol1 + Param.m_iGapMeasureRemoveEndUpper))
						continue;

					if (iSubLTPointX >= (HlROICol2 - Param.m_iGapMeasureRemoveEndLower) && iSubRBPointX <= HlROICol2)
						continue;
				}

				if (Param.m_bGapMeasureDiffImage)
				{
					if (iSubLTPointX >= (HlROICol1 + Param.m_iGapMeasurePosStart) && iSubRBPointX <= (HlROICol1 + Param.m_iGapMeasurePosEnd))
						bUseDiffImage = TRUE;
					else
						bUseDiffImage = FALSE;
				}
			}
			else
			{
				iSubLTPointX = lROICol1;
				iSubRBPointX = lROICol2;
				iSubLTPointY = lROIRow1 + i * iPitch;
				iSubRBPointY = lROIRow1 + (i + 1) * iPitch;

				if (Param.m_bGapMeasureEndRemove)
				{
					if (iSubLTPointY >= lROIRow1 && iSubRBPointY <= (lROIRow1 + Param.m_iGapMeasureRemoveEndUpper))
						continue;

					if (iSubLTPointY >= (lROIRow2 - Param.m_iGapMeasureRemoveEndLower) && iSubRBPointY <= lROIRow2)
						continue;
				}

				if (Param.m_bGapMeasureDiffImage)
				{
					if (iSubLTPointY >= (lROIRow1 + Param.m_iGapMeasurePosStart) && iSubRBPointY <= (lROIRow1 + Param.m_iGapMeasurePosEnd))
						bUseDiffImage = TRUE;
					else
						bUseDiffImage = FALSE;
				}
			}

			dEdgeMeasureCenterX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
			dEdgeMeasureCenterY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;

			iROISizeX = iSubRBPointX - iSubLTPointX;
			iROISizeY = iSubRBPointY - iSubLTPointY;

			if (iROISizeX < 5 || iROISizeY < 5)
				continue;

			// Edge Measure (1)

			if (Param.m_bGapMeasureUpperLine == FALSE)
			{
				if (bUseDiffImage)
					pHCurMeasureImage = pHImage3;
				else
					pHCurMeasureImage = pHImage1;

				if (Param.m_iGapMeasureDir == 1)		// Y Dir
				{
					GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
						lImageHeight, "nearest_neighbor", &MeasureHandle);

					if (Param.m_iGapMeasureUpperPos == 0)
					{
						if (Param.m_iGapMeasureUpperGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iGapMeasureUpperPos == 1)
					{
						if (Param.m_iGapMeasureUpperGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iGapMeasureUpperPos == 2)
					{
						if (Param.m_iGapMeasureUpperGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}
				else		// X Dir
				{
					GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14, iROISizeX / 2, iROISizeY / 2, lImageWidth,
						lImageHeight, "nearest_neighbor", &MeasureHandle);

					if (Param.m_iGapMeasureUpperPos == 0)
					{
						if (Param.m_iGapMeasureUpperGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iGapMeasureUpperPos == 1)
					{
						if (Param.m_iGapMeasureUpperGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iGapMeasureUpperPos == 2)
					{
						if (Param.m_iGapMeasureUpperGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureUpperGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureUpperSmFactor, (double)Param.m_iGapMeasureUpperEdgeStr, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}

				CloseMeasure(MeasureHandle);

				TupleLength(Amplitude, &HNoEdge);
				NoEdge = HNoEdge.L();
				if (NoEdge <= 0)
				{
					if (bUseGapMeasureRetry)
					{
						pHCurMeasureImage = pHImageRetry;

						if (Param.m_iGapMeasureDir == 1)		// Y Dir
						{
							GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
								lImageHeight, "nearest_neighbor", &MeasureHandle);

							if (Param.m_iGapMeasureRetryPos == 0)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else if (Param.m_iGapMeasureRetryPos == 1)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else if (Param.m_iGapMeasureRetryPos == 2)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
						else		// X Dir
						{
							GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14, iROISizeX / 2, iROISizeY / 2, lImageWidth,
								lImageHeight, "nearest_neighbor", &MeasureHandle);

							if (Param.m_iGapMeasureRetryPos == 0)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else if (Param.m_iGapMeasureRetryPos == 1)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else if (Param.m_iGapMeasureRetryPos == 2)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}

						CloseMeasure(MeasureHandle);

						TupleLength(Amplitude, &HNoEdge);
						NoEdge = HNoEdge.L();
						if (NoEdge <= 0)
							continue;
						else
						{
							TupleAbs(Amplitude, &Amplitude);
							TupleSortIndex(Amplitude, &Indices);
							TupleInverse(Indices, &Inverted);
							MaxAmpEdgeIndex = Inverted[0].L();

							if (Param.m_iGapMeasureDir == 1)		// Y Dir
							{
								dUpperEdgePosY = RowEdge[MaxAmpEdgeIndex].D() + (double)Param.m_iGapMeasureRetryPosOffset;
							}
							else
							{
								dUpperEdgePosX = ColumnEdge[MaxAmpEdgeIndex].D() + (double)Param.m_iGapMeasureRetryPosOffset;
							}
						}
					}
					else
						continue;
				}
				else
				{
					TupleAbs(Amplitude, &Amplitude);
					TupleSortIndex(Amplitude, &Indices);
					TupleInverse(Indices, &Inverted);
					MaxAmpEdgeIndex = Inverted[0].L();

					if (Param.m_iGapMeasureDir == 1)		// Y Dir
					{
						dUpperEdgePosY = RowEdge[MaxAmpEdgeIndex].D() + (double)Param.m_iGapMeasureUpperPosOffset;
					}
					else
					{
						dUpperEdgePosX = ColumnEdge[MaxAmpEdgeIndex].D() + (double)Param.m_iGapMeasureUpperPosOffset;
					}
				}
			}

			// Edge Measure (2)

			if (Param.m_bGapMeasureLowerLine == FALSE)
			{
				if (bUseDiffImage)
					pHCurMeasureImage = pHImage3;
				else
					pHCurMeasureImage = pHImage2;

				if (Param.m_iGapMeasureDir == 1)		// Y Dir
				{
					GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
						lImageHeight, "nearest_neighbor", &MeasureHandle);

					if (Param.m_iGapMeasureLowerPos == 0)
					{
						if (Param.m_iGapMeasureLowerGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iGapMeasureLowerPos == 1)
					{
						if (Param.m_iGapMeasureLowerGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iGapMeasureLowerPos == 2)
					{
						if (Param.m_iGapMeasureLowerGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}
				else		// X Dir
				{
					GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14, iROISizeX / 2, iROISizeY / 2, lImageWidth,
						lImageHeight, "nearest_neighbor", &MeasureHandle);

					if (Param.m_iGapMeasureLowerPos == 0)
					{
						if (Param.m_iGapMeasureLowerGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iGapMeasureLowerPos == 1)
					{
						if (Param.m_iGapMeasureLowerGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iGapMeasureLowerPos == 2)
					{
						if (Param.m_iGapMeasureLowerGv == 0)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 1)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iGapMeasureLowerGv == 2)
							MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureLowerSmFactor, (double)Param.m_iGapMeasureLowerEdgeStr, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}

				CloseMeasure(MeasureHandle);

				TupleLength(Amplitude, &HNoEdge);
				NoEdge = HNoEdge.L();
				if (NoEdge <= 0)
				{
					if (bUseGapMeasureRetry)
					{
						pHCurMeasureImage = pHImageRetry;

						if (Param.m_iGapMeasureDir == 1)		// Y Dir
						{
							GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
								lImageHeight, "nearest_neighbor", &MeasureHandle);

							if (Param.m_iGapMeasureRetryPos == 0)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else if (Param.m_iGapMeasureRetryPos == 1)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else if (Param.m_iGapMeasureRetryPos == 2)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}
						else		// X Dir
						{
							GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14, iROISizeX / 2, iROISizeY / 2, lImageWidth,
								lImageHeight, "nearest_neighbor", &MeasureHandle);

							if (Param.m_iGapMeasureRetryPos == 0)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else if (Param.m_iGapMeasureRetryPos == 1)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
							else if (Param.m_iGapMeasureRetryPos == 2)
							{
								if (Param.m_iGapMeasureRetryGv == 0)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "all",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 1)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "negative",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
								else if (Param.m_iGapMeasureRetryGv == 2)
									MeasurePos(*pHCurMeasureImage, MeasureHandle, Param.m_dGapMeasureRetrySmFactor, (double)Param.m_iGapMeasureRetryEdgeStr, "positive",
										"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							}
						}

						CloseMeasure(MeasureHandle);

						TupleLength(Amplitude, &HNoEdge);
						NoEdge = HNoEdge.L();
						if (NoEdge <= 0)
							continue;
						else
						{
							TupleAbs(Amplitude, &Amplitude);
							TupleSortIndex(Amplitude, &Indices);
							TupleInverse(Indices, &Inverted);
							MaxAmpEdgeIndex = Inverted[0].L();

							if (Param.m_iGapMeasureDir == 1)		// Y Dir
							{
								dLowerEdgePosY = RowEdge[MaxAmpEdgeIndex].D() + (double)Param.m_iGapMeasureRetryPosOffset;
							}
							else
							{
								dLowerEdgePosX = ColumnEdge[MaxAmpEdgeIndex].D() + (double)Param.m_iGapMeasureRetryPosOffset;
							}
						}
					}
					else
						continue;
				}
				else
				{
					TupleAbs(Amplitude, &Amplitude);
					TupleSortIndex(Amplitude, &Indices);
					TupleInverse(Indices, &Inverted);
					MaxAmpEdgeIndex = Inverted[0].L();

					if (Param.m_iGapMeasureDir == 1)		// Y Dir
					{
						dLowerEdgePosY = RowEdge[MaxAmpEdgeIndex].D() + (double)Param.m_iGapMeasureLowerPosOffset;
					}
					else
					{
						dLowerEdgePosX = ColumnEdge[MaxAmpEdgeIndex].D() + (double)Param.m_iGapMeasureLowerPosOffset;
					}
				}
			}

			if (Param.m_bGapMeasureUpperLine == FALSE && Param.m_bGapMeasureLowerLine == FALSE)
			{
				// Add Condition
				if (Param.m_iGapMeasureDir == 1)		// Y Dir
				{
					if (dLowerEdgePosY <= dUpperEdgePosY)
					{
						dUpperEdgePosY = dLowerEdgePosY;
					}
				}
				else
				{
					if (dLowerEdgePosX <= dUpperEdgePosX)
					{
						dUpperEdgePosX = dLowerEdgePosX;
					}
				}

				if (Param.m_iGapMeasureDir == 1)		// Y Dir
					dGapMeasureValueUm = fabs(dLowerEdgePosY - dUpperEdgePosY) * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize();
				else
					dGapMeasureValueUm = fabs(dLowerEdgePosX - dUpperEdgePosX) * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize();
			}
			else
			{
				double dMeasurePointX, dMeasurePointY;
				HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
				double dMeasureLineStartX, dMeasureLineStartY, dMeasureLineEndX, dMeasureLineEndY;

				if (Param.m_bGapMeasureUpperLine)
				{
					if (Param.m_iGapMeasureDir == 1)		// Y Dir
					{
						dMeasurePointX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
						dMeasurePointY = dLowerEdgePosY;

						dMeasureLineStartX = dMeasurePointX;
						dMeasureLineStartY = dMeasurePointY;
						dMeasureLineEndX = dMeasurePointX;
						dMeasureLineEndY = dMeasurePointY - 1000;
						if (dMeasureLineEndY < 0)
							dMeasureLineEndY = 0;
					}
					else
					{
						dMeasurePointX = dLowerEdgePosX;
						dMeasurePointY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;

						dMeasureLineStartX = dMeasurePointX;
						dMeasureLineStartY = dMeasurePointY;
						dMeasureLineEndX = dMeasurePointX - 1000;
						dMeasureLineEndY = dMeasurePointY;
						if (dMeasureLineEndX < 0)
							dMeasureLineEndX = 0;
					}

					DistancePl(dMeasurePointY, dMeasurePointX, dEdgeLineStartY, dEdgeLineStartX, dEdgeLineEndY, dEdgeLineEndX, &HDistance);
					dDistance = HDistance[0].D();

					dGapMeasureValueUm = dDistance * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize();

					IntersectionLines(dEdgeLineStartY, dEdgeLineStartX, dEdgeLineEndY, dEdgeLineEndX, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

					HTuple HlNoFoundNumber;
					Hlong lNoFoundNumber;
					TupleLength(HIntersectPointX, &HlNoFoundNumber);
					lNoFoundNumber = HlNoFoundNumber.L();
					if (lNoFoundNumber <= 0)
						continue;

					if (Param.m_iGapMeasureDir == 1)		// Y Dir
					{
						dUpperEdgePosY = HIntersectPointY[0].D();
					}
					else
					{
						dUpperEdgePosX = HIntersectPointX[0].D();
					}
				}

				if (Param.m_bGapMeasureLowerLine)
				{
					if (Param.m_iGapMeasureDir == 1)		// Y Dir
					{
						dMeasurePointX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
						dMeasurePointY = dUpperEdgePosY;

						dMeasureLineStartX = dMeasurePointX;
						dMeasureLineStartY = dMeasurePointY;
						dMeasureLineEndX = dMeasurePointX;
						dMeasureLineEndY = dMeasurePointY + 1000;
					}
					else
					{
						dMeasurePointX = dUpperEdgePosX;
						dMeasurePointY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;

						dMeasureLineStartX = dMeasurePointX;
						dMeasureLineStartY = dMeasurePointY;
						dMeasureLineEndX = dMeasurePointX + 1000;
						dMeasureLineEndY = dMeasurePointY;
					}

					DistancePl(dMeasurePointY, dMeasurePointX, dEdgeLineStartY, dEdgeLineStartX, dEdgeLineEndY, dEdgeLineEndX, &HDistance);
					dDistance = HDistance[0].D();

					dGapMeasureValueUm = dDistance * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize();

					IntersectionLines(dEdgeLineStartY, dEdgeLineStartX, dEdgeLineEndY, dEdgeLineEndX, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

					HTuple HlNoFoundNumber;
					Hlong lNoFoundNumber;
					TupleLength(HIntersectPointX, &HlNoFoundNumber);
					lNoFoundNumber = HlNoFoundNumber.L();
					if (lNoFoundNumber <= 0)
						continue;

					if (Param.m_iGapMeasureDir == 1)		// Y Dir
					{
						dLowerEdgePosY = HIntersectPointY[0].D();
					}
					else
					{
						dLowerEdgePosX = HIntersectPointX[0].D();
					}
				}
			}

			double dLineXPos, dLineYPos;

			TupleGenConst(0, 0, &HEdgePoint_X);
			TupleGenConst(0, 0, &HEdgePoint_Y);

			if (Param.m_iGapMeasureDir == 1)		// Y Dir
			{
				dLineXPos = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
				TupleConcat(HEdgePoint_Y, HTuple(dUpperEdgePosY), &HEdgePoint_Y);
				TupleConcat(HEdgePoint_Y, HTuple(dLowerEdgePosY), &HEdgePoint_Y);

				if (dGapMeasureValueUm < 0.00001)
				{
					TupleConcat(HEdgePoint_X, HTuple(dLineXPos - 10), &HEdgePoint_X);
					TupleConcat(HEdgePoint_X, HTuple(dLineXPos + 10), &HEdgePoint_X);
				}
				else
				{
					TupleConcat(HEdgePoint_X, HTuple(dLineXPos), &HEdgePoint_X);
					TupleConcat(HEdgePoint_X, HTuple(dLineXPos), &HEdgePoint_X);
				}
			}
			else
			{
				dLineYPos = (double)(iSubLTPointY + iSubRBPointY) * 0.5;
				TupleConcat(HEdgePoint_X, HTuple(dUpperEdgePosX), &HEdgePoint_X);
				TupleConcat(HEdgePoint_X, HTuple(dLowerEdgePosX), &HEdgePoint_X);

				if (dGapMeasureValueUm < 0.00001)
				{
					TupleConcat(HEdgePoint_Y, HTuple(dLineYPos - 10), &HEdgePoint_Y);
					TupleConcat(HEdgePoint_Y, HTuple(dLineYPos + 10), &HEdgePoint_Y);
				}
				else
				{
					TupleConcat(HEdgePoint_Y, HTuple(dLineYPos), &HEdgePoint_Y);
					TupleConcat(HEdgePoint_Y, HTuple(dLineYPos), &HEdgePoint_Y);
				}
			}

			if (Param.m_iGapMeasureSelectType == GAP_MEASURE_SELECT_MAX)
			{
				if (dGapMeasureValueUm > dMaxGapMeasureValueUm)
				{
					GenContourPolygonXld(&HGapMeasureLineXLD, HEdgePoint_Y, HEdgePoint_X);

					GenEmptyObj(pHLineFitXLD);
					ConcatObj(*pHLineFitXLD, HGapMeasureLineXLD, pHLineFitXLD);

					dMaxGapMeasureValueUm = dGapMeasureValueUm;

					if (Param.m_iGapMeasureDir == 1)		// Y Dir
					{
						*iGapUpperPointX = (int)dLineXPos;
						*iGapLowerPointX = (int)dLineXPos;
						*iGapUpperPointY = (int)dUpperEdgePosY;
						*iGapLowerPointY = (int)dLowerEdgePosY;
					}
					else
					{
						*iGapUpperPointX = (int)dUpperEdgePosX;
						*iGapLowerPointX = (int)dLowerEdgePosX;
						*iGapUpperPointY = (int)dLineYPos;
						*iGapLowerPointY = (int)dLineYPos;
					}
				}
			}
			else
			{
				if (dGapMeasureValueUm < dMinGapMeasureValueUm)
				{
					GenContourPolygonXld(&HGapMeasureLineXLD, HEdgePoint_Y, HEdgePoint_X);

					GenEmptyObj(pHLineFitXLD);
					ConcatObj(*pHLineFitXLD, HGapMeasureLineXLD, pHLineFitXLD);

					dMinGapMeasureValueUm = dGapMeasureValueUm;

					if (Param.m_iGapMeasureDir == 1)		// Y Dir
					{
						*iGapUpperPointX = (int)dLineXPos;
						*iGapLowerPointX = (int)dLineXPos;
						*iGapUpperPointY = (int)dUpperEdgePosY;
						*iGapLowerPointY = (int)dLowerEdgePosY;
					}
					else
					{
						*iGapUpperPointX = (int)dUpperEdgePosX;
						*iGapLowerPointX = (int)dLowerEdgePosX;
						*iGapUpperPointY = (int)dLineYPos;
						*iGapLowerPointY = (int)dLineYPos;
					}
				}
			}
		}

		if (Param.m_iGapMeasureSelectType == GAP_MEASURE_SELECT_MAX)
		{
			if (dMaxGapMeasureValueUm >= 0)
				*pdGapMeasureValueUm = dMaxGapMeasureValueUm + Param.m_dGapMeasureGapMeasureOffset;
			else
				*pdGapMeasureValueUm = 0;

			if (*pdGapMeasureValueUm < 0)
				*pdGapMeasureValueUm = 0;
		}
		else
		{
			if (dMinGapMeasureValueUm < 100000000)
				*pdGapMeasureValueUm = dMinGapMeasureValueUm + Param.m_dGapMeasureGapMeasureOffset;
			else
				*pdGapMeasureValueUm = 0;

			if (*pdGapMeasureValueUm < 0)
				*pdGapMeasureValueUm = 0;
		}

		if (*pdGapMeasureValueUm > Param.m_dGapMeasureGapInspectionUpper || *pdGapMeasureValueUm < Param.m_dGapMeasureGapInspectionLower)
		{
			if (THEAPP.m_pGFunction->ValidHXLD(*pHLineFitXLD))
			{
				GenRegionContourXld(*pHLineFitXLD, &HDefectRgn, "filled");
				Union1(HDefectRgn, &HDefectRgn);
			}
			else
			{
				Union1(HInspectAreaRgn, &HDefectRgn);
			}
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::GapMeasureAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::CornerMeasureAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, double dPixelResMm, HObject *pHLineFitXLD, double *pdLineStartX, double *pdLineStartY, double *pdLineEndX, double *pdLineEndY, double *pdDistanceMm)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		GenEmptyObj(pHLineFitXLD);

		*pdDistanceMm = -1;

		*pdLineStartX = *pdLineStartY = *pdLineEndX = *pdLineEndY = -1;

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(HImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

		pImageData = (BYTE*)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		Hlong lROIRow1, lROIRow2, lROICol1, lROICol2;
		HTuple HlROIRow1, HlROIRow2, HlROICol1, HlROICol2;

		HObject HXDirInspectAreaRgn, HYDirInspectAreaRgn;

		SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

		lROIRow1 = HlROIRow1.L();
		lROICol1 = HlROICol1.L();
		lROIRow2 = HlROIRow2.L();
		lROICol2 = HlROICol2.L();

		POINT LTPoint, LBPoint, RTPoint, RBPoint;
		LTPoint.x = lROICol1;
		LTPoint.y = lROIRow1;
		LBPoint.x = lROICol1;
		LBPoint.y = lROIRow2;
		RTPoint.x = lROICol2;
		RTPoint.y = lROIRow1;
		RBPoint.x = lROICol2;
		RBPoint.y = lROIRow2;

		HTuple HNoObj;
		Connection(HInspectAreaRgn, &HInspectAreaRgn);
		CountObj(HInspectAreaRgn, &HNoObj);
		if (HNoObj != 2)
			return HDefectRgn;

		HObject HRgn1, HRgn2;
		long lROIWidth1, lROIWidth2;
		SelectObj(HInspectAreaRgn, &HRgn1, 1);
		SmallestRectangle1(HRgn1, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);
		lROIWidth1 = HlROICol2[0].L() - HlROICol1[0].L();
		SelectObj(HInspectAreaRgn, &HRgn2, 2);
		SmallestRectangle1(HRgn2, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);
		lROIWidth2 = HlROICol2[0].L() - HlROICol1[0].L();

		if (lROIWidth1 > lROIWidth2)
		{
			HXDirInspectAreaRgn = HRgn2;
			HYDirInspectAreaRgn = HRgn1;
		}
		else
		{
			HXDirInspectAreaRgn = HRgn1;
			HYDirInspectAreaRgn = HRgn2;
		}

		if (Param.m_iCornerMeasureMultiPointNumber <= 0)
			Param.m_iCornerMeasureMultiPointNumber = 2;

		//////////////////////////////////
		/////		START
		/////////////////////////////////

		double dEdgeMeasureCenterX, dEdgeMeasureCenterY;
		int iROISizeX, iROISizeY;

		HTuple  MeasureHandle;
		HTuple RowEdge, ColumnEdge, Amplitude, Distance, Indices, Inverted;
		Hlong NoEdge, MaxAmpEdgeIndex;

		double dEgePosX, dEgePosY;
		int iEdgePosX, iEdgePosY;
		HObject RegressContour;

		HTuple HNoEdge;

		int iPitch;

		int iSubLTPointX, iSubLTPointY, iSubRBPointX, iSubRBPointY;
		int i;

		HTuple HEdgePosX, HEdgePosY;

		HTuple HlNoFoundNumber;
		Hlong lNoFoundNumber;
		HObject HEdgePolygonContour;

		int iEdgeMeasureDir;

		double dXDirLineStartX, dXDirLineStartY, dXDirLineEndX, dXDirLineEndY;
		double dYDirLineStartX, dYDirLineStartY, dYDirLineEndX, dYDirLineEndY;

		// Edge Measure (X)

		iEdgeMeasureDir = 0;
		int iEdgeMeasurePos = Param.m_iCornerMeasurePos[0];
		int iEdgeMeasureGv = Param.m_iCornerMeasureGv[0];
		double dEdgeMeasureSmFactor = Param.m_dCornerMeasureSmFactor[0];
		int iEdgeMeasureEdgeStr = Param.m_iCornerMeasureEdgeStr[0];
		int iEdgeMeasurePosOffset = Param.m_iCornerMeasurePosOffset[0];

		SmallestRectangle1(HXDirInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

		lROIRow1 = HlROIRow1.L();
		lROICol1 = HlROICol1.L();
		lROIRow2 = HlROIRow2.L();
		lROICol2 = HlROICol2.L();

		if (iEdgeMeasureDir == 1)		// Y Dir
			iPitch = (lROICol2 - lROICol1) / Param.m_iCornerMeasureMultiPointNumber;
		else
			iPitch = (lROIRow2 - lROIRow1) / Param.m_iCornerMeasureMultiPointNumber;

		TupleGenConst(0, 0, &HEdgePosX);
		TupleGenConst(0, 0, &HEdgePosY);

		for (i = 0; i < Param.m_iCornerMeasureMultiPointNumber; i++)
		{
			if (iEdgeMeasureDir == 1)		// Y Dir
			{
				iSubLTPointX = HlROICol1 + i * iPitch;
				iSubRBPointX = HlROICol1 + (i + 1) * iPitch;
				iSubLTPointY = lROIRow1;
				iSubRBPointY = lROIRow2;
			}
			else
			{
				iSubLTPointX = lROICol1;
				iSubRBPointX = lROICol2;
				iSubLTPointY = lROIRow1 + i * iPitch;
				iSubRBPointY = lROIRow1 + (i + 1) * iPitch;
			}

			dEdgeMeasureCenterX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
			dEdgeMeasureCenterY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;

			iROISizeX = iSubRBPointX - iSubLTPointX;
			iROISizeY = iSubRBPointY - iSubLTPointY;

			if (iEdgeMeasureDir == 1)		// Y Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
					lImageHeight, "nearest_neighbor", &MeasureHandle);

				if (iEdgeMeasurePos == 0)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 1)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 2)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}
			else		// X Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14, iROISizeX / 2, iROISizeY / 2, lImageWidth,
					lImageHeight, "nearest_neighbor", &MeasureHandle);

				if (iEdgeMeasurePos == 0)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 1)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 2)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}

			CloseMeasure(MeasureHandle);

			TupleLength(Amplitude, &HNoEdge);
			NoEdge = HNoEdge.L();
			if (NoEdge <= 0)
			{
				continue;
			}
			else
			{
				TupleAbs(Amplitude, &Amplitude);
				TupleSortIndex(Amplitude, &Indices);
				TupleInverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();

				if (iEdgeMeasureDir == 1)		// Y Dir
				{
					dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
					dEgePosY = dEgePosY + (double)iEdgeMeasurePosOffset;
					dEgePosX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
				}
				else
				{
					dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
					dEgePosX = dEgePosX + iEdgeMeasurePosOffset;
					dEgePosY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;
				}

				TupleConcat(HEdgePosX, dEgePosX, &HEdgePosX);
				TupleConcat(HEdgePosY, dEgePosY, &HEdgePosY);
			}

		}  // for (i = 0; i < iNoEdgeMeasure; i++)

		TupleLength(HEdgePosY, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();

		if (lNoFoundNumber >= 2)
		{
			HTuple RowBegin, ColBegin, RowEnd, ColEnd, Nr, Nc, Dist;
			HTuple HMedian, HMin, HMax;
			HTuple HSelectedEdgePosY, HSelectedEdgePosX;

			TupleGenConst(0, 0, &HSelectedEdgePosX);
			TupleGenConst(0, 0, &HSelectedEdgePosY);

			if (iEdgeMeasureDir == 1)		// Y Dir
			{
				TupleMedian(HEdgePosY, &HMedian);

				HMin = HMedian - Param.m_iCornerMeasurePositionRange;
				HMax = HMedian + Param.m_iCornerMeasurePositionRange;

				for (i = 0; i < lNoFoundNumber; i++)
				{
					if (HEdgePosY[i] >= HMin && HEdgePosY[i] <= HMax)
					{
						TupleConcat(HSelectedEdgePosX, HEdgePosX[i], &HSelectedEdgePosX);
						TupleConcat(HSelectedEdgePosY, HEdgePosY[i], &HSelectedEdgePosY);
					}
				}

			}
			else		// X Dir
			{
				TupleMedian(HEdgePosX, &HMedian);

				HMin = HMedian - Param.m_iCornerMeasurePositionRange;
				HMax = HMedian + Param.m_iCornerMeasurePositionRange;

				for (i = 0; i < lNoFoundNumber; i++)
				{
					if (HEdgePosX[i] >= HMin && HEdgePosX[i] <= HMax)
					{
						TupleConcat(HSelectedEdgePosX, HEdgePosX[i], &HSelectedEdgePosX);
						TupleConcat(HSelectedEdgePosY, HEdgePosY[i], &HSelectedEdgePosY);
					}
				}
			}

			HTuple HlNoFoundNumber;
			Hlong lNoFoundNumber;
			TupleLength(HEdgePosY, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			if (lNoFoundNumber >= 2)
			{
				GenContourPolygonXld(&HEdgePolygonContour, HSelectedEdgePosY, HSelectedEdgePosX);
				FitLineContourXld(HEdgePolygonContour, "tukey", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

				TupleLength(RowBegin, &HlNoFoundNumber);
				lNoFoundNumber = HlNoFoundNumber.L();

				if (lNoFoundNumber > 0)
				{
					dXDirLineStartX = ColBegin[0].D();
					dXDirLineStartY = RowBegin[0].D();
					dXDirLineEndX = ColEnd[0].D();
					dXDirLineEndY = RowEnd[0].D();

					GenContourPolygonXld(&RegressContour, HTuple(RowBegin[0]).TupleConcat(HTuple(RowEnd[0])), HTuple(ColBegin[0]).TupleConcat(HTuple(ColEnd[0])));

					ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
				}
			}
		}
		else
			return HDefectRgn;

		// Edge Measure (Y)

		iEdgeMeasureDir = 1;
		iEdgeMeasurePos = Param.m_iCornerMeasurePos[1];
		iEdgeMeasureGv = Param.m_iCornerMeasureGv[1];
		dEdgeMeasureSmFactor = Param.m_dCornerMeasureSmFactor[1];
		iEdgeMeasureEdgeStr = Param.m_iCornerMeasureEdgeStr[1];
		iEdgeMeasurePosOffset = Param.m_iCornerMeasurePosOffset[1];

		SmallestRectangle1(HYDirInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

		lROIRow1 = HlROIRow1.L();
		lROICol1 = HlROICol1.L();
		lROIRow2 = HlROIRow2.L();
		lROICol2 = HlROICol2.L();

		if (iEdgeMeasureDir == 1)		// Y Dir
			iPitch = (lROICol2 - lROICol1) / Param.m_iCornerMeasureMultiPointNumber;
		else
			iPitch = (lROIRow2 - lROIRow1) / Param.m_iCornerMeasureMultiPointNumber;

		TupleGenConst(0, 0, &HEdgePosX);
		TupleGenConst(0, 0, &HEdgePosY);

		for (i = 0; i < Param.m_iCornerMeasureMultiPointNumber; i++)
		{
			if (iEdgeMeasureDir == 1)		// Y Dir
			{
				iSubLTPointX = HlROICol1 + i * iPitch;
				iSubRBPointX = HlROICol1 + (i + 1) * iPitch;
				iSubLTPointY = lROIRow1;
				iSubRBPointY = lROIRow2;
			}
			else
			{
				iSubLTPointX = lROICol1;
				iSubRBPointX = lROICol2;
				iSubLTPointY = lROIRow1 + i * iPitch;
				iSubRBPointY = lROIRow1 + (i + 1) * iPitch;
			}

			dEdgeMeasureCenterX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
			dEdgeMeasureCenterY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;

			iROISizeX = iSubRBPointX - iSubLTPointX;
			iROISizeY = iSubRBPointY - iSubLTPointY;

			if (iEdgeMeasureDir == 1)		// Y Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
					lImageHeight, "nearest_neighbor", &MeasureHandle);

				if (iEdgeMeasurePos == 0)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 1)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 2)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}
			else		// X Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14, iROISizeX / 2, iROISizeY / 2, lImageWidth,
					lImageHeight, "nearest_neighbor", &MeasureHandle);

				if (iEdgeMeasurePos == 0)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 1)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 2)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}

			CloseMeasure(MeasureHandle);

			TupleLength(Amplitude, &HNoEdge);
			NoEdge = HNoEdge.L();
			if (NoEdge <= 0)
			{
				continue;
			}
			else
			{
				TupleAbs(Amplitude, &Amplitude);
				TupleSortIndex(Amplitude, &Indices);
				TupleInverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();

				if (iEdgeMeasureDir == 1)		// Y Dir
				{
					dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
					dEgePosY = dEgePosY + (double)iEdgeMeasurePosOffset;
					dEgePosX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
				}
				else
				{
					dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
					dEgePosX = dEgePosX + iEdgeMeasurePosOffset;
					dEgePosY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;
				}

				TupleConcat(HEdgePosX, dEgePosX, &HEdgePosX);
				TupleConcat(HEdgePosY, dEgePosY, &HEdgePosY);
			}

		}  // for (i = 0; i < iNoEdgeMeasure; i++)

		TupleLength(HEdgePosY, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();

		if (lNoFoundNumber >= 2)
		{
			HTuple RowBegin, ColBegin, RowEnd, ColEnd, Nr, Nc, Dist;
			HTuple HMedian, HMin, HMax;
			HTuple HSelectedEdgePosY, HSelectedEdgePosX;

			TupleGenConst(0, 0, &HSelectedEdgePosX);
			TupleGenConst(0, 0, &HSelectedEdgePosY);

			if (iEdgeMeasureDir == 1)		// Y Dir
			{
				TupleMedian(HEdgePosY, &HMedian);

				HMin = HMedian - Param.m_iCornerMeasurePositionRange;
				HMax = HMedian + Param.m_iCornerMeasurePositionRange;

				for (i = 0; i < lNoFoundNumber; i++)
				{
					if (HEdgePosY[i] >= HMin && HEdgePosY[i] <= HMax)
					{
						TupleConcat(HSelectedEdgePosX, HEdgePosX[i], &HSelectedEdgePosX);
						TupleConcat(HSelectedEdgePosY, HEdgePosY[i], &HSelectedEdgePosY);
					}
				}

			}
			else		// X Dir
			{
				TupleMedian(HEdgePosX, &HMedian);

				HMin = HMedian - Param.m_iCornerMeasurePositionRange;
				HMax = HMedian + Param.m_iCornerMeasurePositionRange;

				for (i = 0; i < lNoFoundNumber; i++)
				{
					if (HEdgePosX[i] >= HMin && HEdgePosX[i] <= HMax)
					{
						TupleConcat(HSelectedEdgePosX, HEdgePosX[i], &HSelectedEdgePosX);
						TupleConcat(HSelectedEdgePosY, HEdgePosY[i], &HSelectedEdgePosY);
					}
				}
			}

			HTuple HlNoFoundNumber;
			Hlong lNoFoundNumber;
			TupleLength(HEdgePosY, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			if (lNoFoundNumber >= 2)
			{
				GenContourPolygonXld(&HEdgePolygonContour, HSelectedEdgePosY, HSelectedEdgePosX);
				FitLineContourXld(HEdgePolygonContour, "tukey", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

				TupleLength(RowBegin, &HlNoFoundNumber);
				lNoFoundNumber = HlNoFoundNumber.L();

				if (lNoFoundNumber > 0)
				{
					dYDirLineStartX = ColBegin[0].D();
					dYDirLineStartY = RowBegin[0].D();
					dYDirLineEndX = ColEnd[0].D();
					dYDirLineEndY = RowEnd[0].D();

					GenContourPolygonXld(&RegressContour, HTuple(RowBegin[0]).TupleConcat(HTuple(RowEnd[0])), HTuple(ColBegin[0]).TupleConcat(HTuple(ColEnd[0])));

					ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
				}
			}
		}
		else
			return HDefectRgn;

		HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
		IntersectionLines(dXDirLineStartY, dXDirLineStartX, dXDirLineEndY, dXDirLineEndX, dYDirLineStartY, dYDirLineStartX, dYDirLineEndY, dYDirLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		DPOINT IntersectPoint;
		IntersectPoint.x = HIntersectPointX[0].D();
		IntersectPoint.y = HIntersectPointY[0].D();

		int iWhichCorner = 0;	// 0:LT, 1:LB, 2:RT, 3:RB
		double dDist, dMinDist;

		dMinDist = (LTPoint.x - IntersectPoint.x) * (LTPoint.x - IntersectPoint.x) + (LTPoint.y - IntersectPoint.y) * (LTPoint.y - IntersectPoint.y);

		dDist = (LBPoint.x - IntersectPoint.x) * (LBPoint.x - IntersectPoint.x) + (LBPoint.y - IntersectPoint.y) * (LBPoint.y - IntersectPoint.y);
		if (dDist < dMinDist)
		{
			dMinDist = dDist;
			iWhichCorner = 1;
		}

		dDist = (RTPoint.x - IntersectPoint.x) * (RTPoint.x - IntersectPoint.x) + (RTPoint.y - IntersectPoint.y) * (RTPoint.y - IntersectPoint.y);
		if (dDist < dMinDist)
		{
			dMinDist = dDist;
			iWhichCorner = 2;
		}

		dDist = (RBPoint.x - IntersectPoint.x) * (RBPoint.x - IntersectPoint.x) + (RBPoint.y - IntersectPoint.y) * (RBPoint.y - IntersectPoint.y);
		if (dDist < dMinDist)
		{
			dMinDist = dDist;
			iWhichCorner = 3;
		}

		HTuple HMeasureAngleDeg, HMeasureAngleRad;

		if (iWhichCorner == 0)
		{
			HMeasureAngleDeg = -45;
		}
		else if (iWhichCorner == 1)
		{
			HMeasureAngleDeg = 45;
		}
		else if (iWhichCorner == 2)
		{
			HMeasureAngleDeg = -135;
		}
		else if (iWhichCorner == 3)
		{
			HMeasureAngleDeg = 135;
		}

		TupleRad(HMeasureAngleDeg, &HMeasureAngleRad);

		GenMeasureRectangle2(IntersectPoint.y, IntersectPoint.x, HMeasureAngleRad, 60, 20, lImageWidth, lImageHeight, "nearest_neighbor", &MeasureHandle);
		MeasurePos(HImage, MeasureHandle, Param.m_dCornerMeasureTargetSmFactor, (double)Param.m_iCornerMeasureTargetEdgeStr, "positive", "first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);

		CloseMeasure(MeasureHandle);

		DPOINT dTargetPoint;

		TupleLength(Amplitude, &HNoEdge);
		NoEdge = HNoEdge.L();
		if (NoEdge <= 0)
		{
			GenRectangle1(&HDefectRgn, IntersectPoint.y - 10, IntersectPoint.x - 10, IntersectPoint.y + 10, IntersectPoint.x + 10);
		}
		else
		{
			dTargetPoint.x = ColumnEdge[0].D();
			dTargetPoint.y = RowEdge[0].D();

			dDist = (dTargetPoint.x - IntersectPoint.x) * (dTargetPoint.x - IntersectPoint.x) + (dTargetPoint.y - IntersectPoint.y) * (dTargetPoint.y - IntersectPoint.y);
			dDist = sqrt(dDist);

			*pdLineStartX = IntersectPoint.x;
			*pdLineStartY = IntersectPoint.y;
			*pdLineEndX = dTargetPoint.x;
			*pdLineEndY = dTargetPoint.y;

			*pdDistanceMm = dDist * dPixelResMm;

			GenEmptyObj(pHLineFitXLD);

			GenContourPolygonXld(&RegressContour, HTuple(IntersectPoint.y).TupleConcat(HTuple(dXDirLineStartY)).TupleConcat(HTuple(dXDirLineEndY)), HTuple(IntersectPoint.x).TupleConcat(HTuple(dXDirLineStartX)).TupleConcat(HTuple(dXDirLineEndX)));
			ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);

			GenContourPolygonXld(&RegressContour, HTuple(IntersectPoint.y).TupleConcat(HTuple(dYDirLineStartY)).TupleConcat(HTuple(dYDirLineEndY)), HTuple(IntersectPoint.x).TupleConcat(HTuple(dYDirLineStartX)).TupleConcat(HTuple(dYDirLineEndX)));
			ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);

			GenContourPolygonXld(&RegressContour, HTuple(dTargetPoint.y).TupleConcat(HTuple(IntersectPoint.y)), HTuple(dTargetPoint.x).TupleConcat(HTuple(IntersectPoint.x)));
			ConcatObj(*pHLineFitXLD, RegressContour, pHLineFitXLD);

			if (*pdDistanceMm < Param.m_dCornerMeasureTargetDistanceSpec)
			{
				GenRectangle1(&HDefectRgn, dTargetPoint.y - 10, dTargetPoint.x - 10, dTargetPoint.y + 10, dTargetPoint.x + 10);
			}
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::CornerMeasureAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::HomerTiltAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHBenvolioEdgeMeasureImage)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\HomerTiltImage");
			WriteImage(*pHBenvolioEdgeMeasureImage, "bmp", 0, "c:\\DualTest\\HomerTiltBenvolioImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		Connection(HInspectAreaRgn, &HInspectAreaRgn);

		HTuple HNoROI;
		CountObj(HInspectAreaRgn, &HNoROI);

		if (HNoROI[0].L() != 2)
			return HDefectRgn;

		HObject HHomerInspectRgn, HBenvolioInspectRgn;
		HObject HTempRgn1, HTempRgn2;
		HTuple HArea, HCenterX, HCenterY;
		double dTempCenterY1, dTempCenterY2;

		SelectObj(HInspectAreaRgn, &HTempRgn1, 1);
		SelectObj(HInspectAreaRgn, &HTempRgn2, 2);

		AreaCenter(HTempRgn1, &HArea, &HCenterY, &HCenterX);
		dTempCenterY1 = HCenterY[0].D();
		AreaCenter(HTempRgn2, &HArea, &HCenterY, &HCenterX);
		dTempCenterY2 = HCenterY[0].D();

		if (dTempCenterY1 < dTempCenterY2)
		{
			HHomerInspectRgn = HTempRgn1;
			HBenvolioInspectRgn = HTempRgn2;
		}
		else
		{
			HHomerInspectRgn = HTempRgn2;
			HBenvolioInspectRgn = HTempRgn1;
		}

		if (bDebugSave)
		{
			WriteRegion(HHomerInspectRgn, "c:\\DualTest\\HomerInspectRgn");
			WriteRegion(HBenvolioInspectRgn, "c:\\DualTest\\BenvolioInspectRgn");
		}

		HObject HHomerImageReduced;
		ReduceDomain(HImage, HHomerInspectRgn, &HHomerImageReduced);

		//////////////////////////////////////////////////////////////////////////
		// Homer Boundary
		//////////////////////////////////////////////////////////////////////////

		HObject  Edges, DerivGauss, SelectedEdges, UnionContours, MaxLengthXLD;
		HTuple  XLDLength, RowBegin, ColBegin;
		HTuple  RowEnd, ColEnd, Nr, Nc, Dist, RowEdge, ColEdge, Indices, Inverted;
		int iFitEdgeStrengthHigh = Param.m_iHomerTiltHomerEdgeStr;
		int iFitEdgeStrengthLow = iFitEdgeStrengthHigh - 10;
		if (iFitEdgeStrengthLow <= 5)
			iFitEdgeStrengthLow = iFitEdgeStrengthHigh;

		EdgesSubPix(HHomerImageReduced, &Edges, "canny", 3.0, iFitEdgeStrengthLow, iFitEdgeStrengthHigh);
		SelectContoursXld(Edges, &SelectedEdges, "contour_length", 100, 99999999, -0.5, 0.5);
		SelectContoursXld(SelectedEdges, &SelectedEdges, "open", 50, 200, -0.5, 0.5);
		UnionAdjacentContoursXld(SelectedEdges, &UnionContours, 30, 1, "attr_keep");
		if (THEAPP.m_pGFunction->ValidHXLD(UnionContours) == FALSE)
			return HDefectRgn;

		LengthXld(UnionContours, &XLDLength);
		TupleSortIndex(XLDLength, &Indices);
		TupleInverse(Indices, &Inverted);
		SelectObj(UnionContours, &MaxLengthXLD, HTuple(Inverted[0]) + 1);

		FitLineContourXld(MaxLengthXLD, "tukey", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

		Hlong lNoPoint;
		HTuple HlNoPoint;
		TupleLength(RowBegin, &HlNoPoint);
		lNoPoint = HlNoPoint.L();
		if (lNoPoint <= 0)
			return HDefectRgn;

		// Extension of Fit Line Points to ROI Boundary

		Hlong lRow1, lRow2, lCol1, lCol2, lROIWidth, lROIHeight;
		HTuple HlRow1, HlRow2, HlCol1, HlCol2;

		SmallestRectangle1(HHomerInspectRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

		lRow1 = HlRow1.L();
		lCol1 = HlCol1.L();
		lRow2 = HlRow2.L();
		lCol2 = HlCol2.L();

		lROIWidth = lCol2 - lCol1;
		lROIHeight = lRow2 - lRow1;

		double dLineStartX, dLineStartY, dLineEndX, dLineEndY;
		double dFitLineStartX, dFitLineStartY, dFitLineEndX, dFitLineEndY;
		double dIntersectX, dIntersectY;
		Hlong IsParallel;
		HTuple HdIntersectX, HdIntersectY, HIsParallel;

		dFitLineStartX = ColBegin[0].D();
		dFitLineStartY = RowBegin[0].D();
		dFitLineEndX = ColEnd[0].D();
		dFitLineEndY = RowEnd[0].D();

		dLineStartX = lCol1;
		dLineStartY = lRow1;
		dLineEndX = lCol1;
		dLineEndY = lRow2;

		IntersectionLl(dLineStartY, dLineStartX, dLineEndY, dLineEndX, dFitLineStartY, dFitLineStartX, dFitLineEndY, dFitLineEndX, &HdIntersectY, &HdIntersectX, &HIsParallel);
		dIntersectX = HdIntersectX.D();
		dIntersectY = HdIntersectY.D();
		IsParallel = HIsParallel.L();

		if (IsParallel > 0)
			return HDefectRgn;

		RowBegin = dIntersectY;
		ColBegin = dIntersectX;

		dLineStartX = lCol2;
		dLineStartY = lRow1;
		dLineEndX = lCol2;
		dLineEndY = lRow2;

		IntersectionLl(dLineStartY, dLineStartX, dLineEndY, dLineEndX, dFitLineStartY, dFitLineStartX, dFitLineEndY, dFitLineEndX, &HdIntersectY, &HdIntersectX, &HIsParallel);
		dIntersectX = HdIntersectX.D();
		dIntersectY = HdIntersectY.D();
		IsParallel = HIsParallel.L();
		if (IsParallel > 0)
			return HDefectRgn;

		RowEnd = dIntersectY;
		ColEnd = dIntersectX;

		HObject RegressContour;
		GenContourPolygonXld(&RegressContour, HTuple(RowBegin[0]).TupleConcat(HTuple(RowEnd[0])), HTuple(ColBegin[0]).TupleConcat(HTuple(ColEnd[0])));

		double dHomerStartX, dHomerEndX, dHomerStartY, dHomerEndY, dHomerTiltHeight;

		dHomerStartX = ColBegin[0].D();
		dHomerEndX = ColEnd[0].D();
		dHomerStartY = RowBegin[0].D();
		dHomerEndY = RowEnd[0].D();

		HTuple HAngleRad, HTangent;
		AngleLx(dHomerStartY, dHomerStartX, dHomerEndY, dHomerEndX, &HAngleRad);
		TupleAbs(HAngleRad, &HAngleRad);
		if (HAngleRad[0].D() > PI / 2)
			HAngleRad = PI - HAngleRad;

		TupleTan(HAngleRad, &HTangent);

		dHomerTiltHeight = HTangent[0].D() * Param.m_dHomerTiltHomerLength;

		//////////////////////////////////////////////////////////////////////////
		// Distance between Homer and Benvolio
		//////////////////////////////////////////////////////////////////////////

		HTuple HImageWidth, HImageHeight;
		GetImageSize(HImage, &HImageWidth, &HImageHeight);

		double dEdgeMeasureCenterX, dEdgeMeasureCenterY;
		int iROISizeX, iROISizeY;

		Hlong lROIRow1, lROIRow2, lROICol1, lROICol2;
		HTuple HlROIRow1, HlROIRow2, HlROICol1, HlROICol2;

		SmallestRectangle1(HBenvolioInspectRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

		lROIRow1 = HlROIRow1.L();
		lROICol1 = HlROICol1.L();
		lROIRow2 = HlROIRow2.L();
		lROICol2 = HlROICol2.L();

		dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
		dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

		iROISizeX = lROICol2 - lROICol1;
		iROISizeY = lROIRow2 - lROIRow1;

		HTuple  MeasureHandle;
		HTuple ColumnEdge, Amplitude, Distance;
		Hlong NoEdge, MaxAmpEdgeIndex;

		GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, HImageWidth,
			HImageHeight, "nearest_neighbor", &MeasureHandle);

		MeasurePos(*pHBenvolioEdgeMeasureImage, MeasureHandle, 1.0, (double)Param.m_iHomerTiltBenvolioEdgeStr, "all",
			"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);

		CloseMeasure(MeasureHandle);

		double dBenvolioEdgePosY;

		HTuple HNoEdge;
		TupleLength(Amplitude, &HNoEdge);
		NoEdge = HNoEdge.L();
		if (NoEdge <= 0)
		{
			return HDefectRgn;
		}
		else
		{
			TupleAbs(Amplitude, &Amplitude);
			TupleSortIndex(Amplitude, &Indices);
			TupleInverse(Indices, &Inverted);
			MaxAmpEdgeIndex = Inverted[0].L();

			dBenvolioEdgePosY = RowEdge[MaxAmpEdgeIndex].D();
		}

		double dBenvolioHomerDistance, dTempDist1, dTempDist2;

		dTempDist1 = fabs(dBenvolioEdgePosY - dHomerStartY);
		dTempDist2 = fabs(dBenvolioEdgePosY - dHomerEndY);

		if (dTempDist1 > dTempDist2)
			dBenvolioHomerDistance = dTempDist1;
		else
			dBenvolioHomerDistance = dTempDist2;

		m_dHomerTiltHeight = dHomerTiltHeight;
		m_dHomerTiltBenvolioDist = dBenvolioHomerDistance;

		if (dHomerTiltHeight > Param.m_dHomerTiltTol || dBenvolioHomerDistance > Param.m_dHomerTiltDistanceTol)
		{
			GenRegionContourXld(RegressContour, &HDefectRgn, "filled");
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			Union1(HDefectRgn, &HDefectRgn);

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::HomerTiltAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::EpoxyVoidHoleAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHBenvolioHtccEdgeMeasureImage, BOOL *pbDefect)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		*pbDefect = FALSE;

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		HObject HEdgeMeasureImage;
		CopyImage(*pHBenvolioHtccEdgeMeasureImage, &HEdgeMeasureImage);

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteImage(HEdgeMeasureImage, "bmp", 0, "c:\\DualTest\\EdgeMeasureImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		Connection(HInspectAreaRgn, &HInspectAreaRgn);

		Hlong lNoROI;
		HTuple HNoROI;
		CountObj(HInspectAreaRgn, &HNoROI);
		lNoROI = HNoROI[0].L();

		if (lNoROI < 3)
			return HDefectRgn;

		int i;
		HTuple HArea, HCenterX, HCenterY;
		HTuple Indices, Inverted;
		HObject HLeftEdgeMeasureRgn, HRightEdgeMeasureRgn, HEpoxyInspectRgn;
		HObject HSelectedRgn;

		AreaCenter(HInspectAreaRgn, &HArea, &HCenterY, &HCenterX);

		TupleSortIndex(HCenterX, &Indices);
		SelectObj(HInspectAreaRgn, &HLeftEdgeMeasureRgn, HTuple(Indices[0]) + 1);
		SelectObj(HInspectAreaRgn, &HRightEdgeMeasureRgn, HTuple(Indices[lNoROI - 1]) + 1);

		GenEmptyObj(&HEpoxyInspectRgn);
		for (i = 0; i < lNoROI; i++)
		{
			if (i == 0 || i == (lNoROI - 1))
				continue;

			SelectObj(HInspectAreaRgn, &HSelectedRgn, HTuple(Indices[i]) + 1);

			if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
				continue;

			ConcatObj(HEpoxyInspectRgn, HSelectedRgn, &HEpoxyInspectRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HEpoxyInspectRgn))
			Union1(HEpoxyInspectRgn, &HEpoxyInspectRgn);
		else
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteRegion(HLeftEdgeMeasureRgn, "c:\\DualTest\\LeftEdgeMeasureRgn");
			WriteRegion(HRightEdgeMeasureRgn, "c:\\DualTest\\RightEdgeMeasureRgn");
			WriteRegion(HEpoxyInspectRgn, "c:\\DualTest\\EpoxyInspectRgn");
		}

		//////////////////////////////////////////////////////////////////////////
		// Edge Points
		//////////////////////////////////////////////////////////////////////////

		HTuple HImageWidth, HImageHeight;
		GetImageSize(HImage, &HImageWidth, &HImageHeight);

		double dEdgeMeasureCenterX, dEdgeMeasureCenterY;
		int iROISizeX, iROISizeY;

		Hlong lROIRow1, lROIRow2, lROICol1, lROICol2;
		HTuple HlROIRow1, HlROIRow2, HlROICol1, HlROICol2;

		HTuple  MeasureHandle;
		HTuple RowEdge, ColumnEdge, Amplitude, Distance;
		Hlong NoEdge, FirstAmpEdgeIndex, SecondAmpEdgeIndex;

		double dEdgePosY_1, dEdgePosY_2;
		DPOINT dEdgePosY_LT, dEdgePosY_LB, dEdgePosY_RT, dEdgePosY_RB;
		HTuple HNoEdge;

		HObject HUpperMeasureRgn, HLowerMeasureRgn;
		Hlong lCenterYPos;

		// Left
		SmallestRectangle1(HLeftEdgeMeasureRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

		lROIRow1 = HlROIRow1.L();
		lROICol1 = HlROICol1.L();
		lROIRow2 = HlROIRow2.L();
		lROICol2 = HlROICol2.L();

		dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
		dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

		iROISizeX = lROICol2 - lROICol1;
		iROISizeY = lROIRow2 - lROIRow1;

		GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, HImageWidth, HImageHeight, "nearest_neighbor", &MeasureHandle);
		MeasurePos(HEdgeMeasureImage, MeasureHandle, 1.0, Param.m_dEpoxyVoidHoleEdgeStr, "positive", "all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
		CloseMeasure(MeasureHandle);

		TupleLength(Amplitude, &HNoEdge);
		NoEdge = HNoEdge.L();
		if (NoEdge < 1)
		{
			return HDefectRgn;
		}
		else
		{
			TupleAbs(Amplitude, &Amplitude);
			TupleSortIndex(Amplitude, &Indices);
			TupleInverse(Indices, &Inverted);
			FirstAmpEdgeIndex = Inverted[0].L();

			dEdgePosY_LT.y = RowEdge[FirstAmpEdgeIndex].D();
			dEdgePosY_LT.x = dEdgeMeasureCenterX;
		}

		GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, HImageWidth, HImageHeight, "nearest_neighbor", &MeasureHandle);
		MeasurePos(HEdgeMeasureImage, MeasureHandle, 1.0, Param.m_dEpoxyVoidHoleEdgeStr, "negative", "all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
		CloseMeasure(MeasureHandle);

		TupleLength(Amplitude, &HNoEdge);
		NoEdge = HNoEdge.L();
		if (NoEdge < 1)
		{
			return HDefectRgn;
		}
		else
		{
			TupleAbs(Amplitude, &Amplitude);
			TupleSortIndex(Amplitude, &Indices);
			TupleInverse(Indices, &Inverted);
			FirstAmpEdgeIndex = Inverted[0].L();

			dEdgePosY_LB.y = RowEdge[FirstAmpEdgeIndex].D();
			dEdgePosY_LB.x = dEdgeMeasureCenterX;
		}

		// Right
		SmallestRectangle1(HRightEdgeMeasureRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

		lROIRow1 = HlROIRow1.L();
		lROICol1 = HlROICol1.L();
		lROIRow2 = HlROIRow2.L();
		lROICol2 = HlROICol2.L();

		dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
		dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

		iROISizeX = lROICol2 - lROICol1;
		iROISizeY = lROIRow2 - lROIRow1;

		GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, HImageWidth, HImageHeight, "nearest_neighbor", &MeasureHandle);
		MeasurePos(HEdgeMeasureImage, MeasureHandle, 1.0, Param.m_dEpoxyVoidHoleEdgeStr, "positive", "all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
		CloseMeasure(MeasureHandle);

		TupleLength(Amplitude, &HNoEdge);
		NoEdge = HNoEdge.L();
		if (NoEdge < 1)
		{
			return HDefectRgn;
		}
		else
		{
			TupleAbs(Amplitude, &Amplitude);
			TupleSortIndex(Amplitude, &Indices);
			TupleInverse(Indices, &Inverted);
			FirstAmpEdgeIndex = Inverted[0].L();

			dEdgePosY_RT.y = RowEdge[FirstAmpEdgeIndex].D();
			dEdgePosY_RT.x = dEdgeMeasureCenterX;
		}

		GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, HImageWidth, HImageHeight, "nearest_neighbor", &MeasureHandle);
		MeasurePos(HEdgeMeasureImage, MeasureHandle, 1.0, Param.m_dEpoxyVoidHoleEdgeStr, "negative", "all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
		CloseMeasure(MeasureHandle);

		TupleLength(Amplitude, &HNoEdge);
		NoEdge = HNoEdge.L();
		if (NoEdge < 1)
		{
			return HDefectRgn;
		}
		else
		{
			TupleAbs(Amplitude, &Amplitude);
			TupleSortIndex(Amplitude, &Indices);
			TupleInverse(Indices, &Inverted);
			FirstAmpEdgeIndex = Inverted[0].L();

			dEdgePosY_RB.y = RowEdge[FirstAmpEdgeIndex].D();
			dEdgePosY_RB.x = dEdgeMeasureCenterX;
		}

		double dLeftHeight, dRightHeight, dLeftRightDeviation, dMinHeight;

		dLeftHeight = fabs(dEdgePosY_LT.y - dEdgePosY_LB.y);
		dRightHeight = fabs(dEdgePosY_RT.y - dEdgePosY_RB.y);
		if (dLeftHeight < dRightHeight)
			dMinHeight = dLeftHeight;
		else
			dMinHeight = dRightHeight;
		dLeftRightDeviation = fabs(dLeftHeight - dRightHeight);

		HTuple HEdgePoint_X, HEdgePoint_Y;
		TupleGenConst(0, 0, &HEdgePoint_X);
		TupleGenConst(0, 0, &HEdgePoint_Y);

		TupleConcat(HEdgePoint_X, HTuple(dEdgePosY_LT.x), &HEdgePoint_X);
		TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_LT.y), &HEdgePoint_Y);
		TupleConcat(HEdgePoint_X, HTuple(dEdgePosY_RT.x), &HEdgePoint_X);
		TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_RT.y), &HEdgePoint_Y);
		TupleConcat(HEdgePoint_X, HTuple(dEdgePosY_RB.x), &HEdgePoint_X);
		TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_RB.y), &HEdgePoint_Y);
		TupleConcat(HEdgePoint_X, HTuple(dEdgePosY_LB.x), &HEdgePoint_X);
		TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_LB.y), &HEdgePoint_Y);
		TupleConcat(HEdgePoint_X, HTuple(dEdgePosY_LT.x), &HEdgePoint_X);
		TupleConcat(HEdgePoint_Y, HTuple(dEdgePosY_LT.y), &HEdgePoint_Y);

		HObject HGapMeasureRgn;
		GenContourPolygonXld(&HGapMeasureRgn, HEdgePoint_Y, HEdgePoint_X);
		GenRegionContourXld(HGapMeasureRgn, &HGapMeasureRgn, "filled");

		if (bDebugSave)
		{
			WriteRegion(HGapMeasureRgn, "c:\\DualTest\\GapMeasureRgn");
		}

		//////////////////////////////////////////////////////////////////////////

		if (Param.m_bEpoxyVoidHoleTiltInspect)
		{
			if (dLeftRightDeviation > Param.m_iEpoxyVoidHoleTiltTolerance)
			{
				*pbDefect = TRUE;

				return HGapMeasureRgn;
			}
		}

		if (Param.m_bEpoxyVoidHoleGapInspect)
		{
			if (dMinHeight < Param.m_iEpoxyVoidHoleGapTolerance)
			{
				*pbDefect = TRUE;

				return HGapMeasureRgn;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		if (THEAPP.m_pGFunction->ValidHRegion(HGapMeasureRgn) == TRUE)
			Intersection(HGapMeasureRgn, HEpoxyInspectRgn, &HDefectRgn);
		else
			return HDefectRgn;

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
		{
			Union1(HDefectRgn, &HDefectRgn);

			if (Param.m_iEpoxyVoidHoleInspectMargin > 0)
			{
				ErosionRectangle1(HDefectRgn, &HDefectRgn, 1, Param.m_iEpoxyVoidHoleInspectMargin * 2 + 1);
			}
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::EpoxyVoidHoleAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::NeckEpoxyMeasureAlgorithm(HObject HImage, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, HObject *pHMaskRgn,
	double *pdEpoxyMeasureValue_TopOffset, double *pdEpoxyMeasureValue_BottomOffset, double *pdEpoxyMeasureValue_Length, double *pdEpoxyMeasureValue_HeightMax, double *pdEpoxyMeasureValue_HeightMin)
{
	try
	{
		BOOL bDebugSave = FALSE;

		*pdEpoxyMeasureValue_TopOffset = 0;
		*pdEpoxyMeasureValue_BottomOffset = 0;
		*pdEpoxyMeasureValue_Length = 0;
		*pdEpoxyMeasureValue_HeightMax = 0;
		*pdEpoxyMeasureValue_HeightMin = 0;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		GenEmptyObj(pHLineFitXLD);

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(HImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

		pImageData = (BYTE*)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		double dEpoxyMeasureValue_TopPos = 0;
		double dEpoxyMeasureValue_BottomPos = 0;
		double dEpoxyMeasureValue_TopOffset = 0;
		double dEpoxyMeasureValue_BottomOffset = 0;
		double dEpoxyMeasureValue_Length = 0;
		double dEpoxyMeasureValue_HeightMax = 0;
		double dEpoxyMeasureValue_HeightMin = 0;

		HObject HSurfaceAlgoDefectRgn;
		GenEmptyObj(&HSurfaceAlgoDefectRgn);

		if (Param.m_iNeckEpoxyInspectMethod == NECK_EPOXY_INSPECT_EDGE)
		{
			HObject HEdgeMeasureRgn;
			Hlong lInspectROIRow1, lInspectROIRow2, lInspectROICol1, lInspectROICol2;
			HTuple HlROIRow1, HlROIRow2, HlROICol1, HlROICol2;
			Hlong lROIRow1, lROIRow2, lROICol1, lROICol2;
			int iInspectROISizeX, iInspectROISizeY;

			SmallestRectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);
			lInspectROIRow1 = HlROIRow1.L();
			lInspectROICol1 = HlROICol1.L();
			lInspectROIRow2 = HlROIRow2.L();
			lInspectROICol2 = HlROICol2.L();

			iInspectROISizeX = lInspectROICol2 - lInspectROICol1;
			iInspectROISizeY = lInspectROIRow2 - lInspectROIRow1;

			double dEdgeMeasureCenterX, dEdgeMeasureCenterY;
			int iROISizeX, iROISizeY;

			HTuple  MeasureHandle;
			HTuple RowEdge, ColumnEdge, Amplitude, Distance, Indices, Inverted;
			Hlong lNoEdge, MaxAmpEdgeIndex;

			double dEgePosX, dEgePosY;
			int iEdgePosX, iEdgePosY;
			HObject RegressContour;
			HTuple HNoEdge;

			// Edge Measure (Top / Y-Dir)

			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			GenRectangle1(&HEdgeMeasureRgn, lInspectROIRow1 + 2, lInspectROICol1, lInspectROIRow1 + iInspectROISizeX * 3, lInspectROICol1 + iInspectROISizeX / 3);
			/////////////////////////////////////////////////////////////////////////////////////////////////////////

			SmallestRectangle1(HEdgeMeasureRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

			lROIRow1 = HlROIRow1.L();
			lROICol1 = HlROICol1.L();
			lROIRow2 = HlROIRow2.L();
			lROICol2 = HlROICol2.L();

			dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
			dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

			iROISizeX = lROICol2 - lROICol1;
			iROISizeY = lROIRow2 - lROIRow1;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			int iEdgeMeasureDir = 1;
			int iEdgeMeasurePos = 0;
			int iEdgeMeasureGv = 2;
			double dEdgeMeasureSmFactor = 1.0;
			int iEdgeMeasureEdgeStr = Param.m_iNeckEpoxyMeasureTopEdgeStr;
			/////////////////////////////////////////////////////////////////////////////////////////////////////////

			if (iEdgeMeasureDir == 1)		// Y Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
					lImageHeight, "nearest_neighbor", &MeasureHandle);

				if (iEdgeMeasurePos == 0)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 1)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 2)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}
			else		// X Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14, iROISizeX / 2, iROISizeY / 2, lImageWidth,
					lImageHeight, "nearest_neighbor", &MeasureHandle);

				if (iEdgeMeasurePos == 0)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 1)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 2)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}

			CloseMeasure(MeasureHandle);

			TupleLength(Amplitude, &HNoEdge);
			lNoEdge = HNoEdge.L();
			if (lNoEdge <= 0)
			{
				dEpoxyMeasureValue_TopPos = lInspectROIRow1 + 2;
				dEpoxyMeasureValue_TopOffset = 0;
			}
			else
			{
				TupleAbs(Amplitude, &Amplitude);
				TupleSortIndex(Amplitude, &Indices);
				TupleInverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();

				if (iEdgeMeasureDir == 1)		// Y Dir
				{
					dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
					iEdgePosY = (int)dEgePosY;

					dEpoxyMeasureValue_TopPos = dEgePosY;
					dEpoxyMeasureValue_TopOffset = dEgePosY - (double)lInspectROIRow1;
				}
				else
				{
					dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
					iEdgePosX = (int)dEgePosX;
				}
			}

			// Edge Measure (Bottom / Y-Dir)

			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			GenRectangle1(&HEdgeMeasureRgn, lInspectROIRow2 - iInspectROISizeX * 3, lInspectROICol1, lInspectROIRow2 - 2, lInspectROICol1 + iInspectROISizeX / 3);
			/////////////////////////////////////////////////////////////////////////////////////////////////////////

			SmallestRectangle1(HEdgeMeasureRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

			lROIRow1 = HlROIRow1.L();
			lROICol1 = HlROICol1.L();
			lROIRow2 = HlROIRow2.L();
			lROICol2 = HlROICol2.L();

			dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
			dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

			iROISizeX = lROICol2 - lROICol1;
			iROISizeY = lROIRow2 - lROIRow1;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			iEdgeMeasureDir = 1;
			iEdgeMeasurePos = 0;
			iEdgeMeasureGv = 1;
			dEdgeMeasureSmFactor = 1.0;
			iEdgeMeasureEdgeStr = Param.m_iNeckEpoxyMeasureBottomEdgeStr;
			/////////////////////////////////////////////////////////////////////////////////////////////////////////

			if (iEdgeMeasureDir == 1)		// Y Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
					lImageHeight, "nearest_neighbor", &MeasureHandle);

				if (iEdgeMeasurePos == 0)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 1)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 2)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}
			else		// X Dir
			{
				GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14, iROISizeX / 2, iROISizeY / 2, lImageWidth,
					lImageHeight, "nearest_neighbor", &MeasureHandle);

				if (iEdgeMeasurePos == 0)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 1)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (iEdgeMeasurePos == 2)
				{
					if (iEdgeMeasureGv == 0)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 1)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (iEdgeMeasureGv == 2)
						MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
							"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}

			CloseMeasure(MeasureHandle);

			TupleLength(Amplitude, &HNoEdge);
			lNoEdge = HNoEdge.L();
			if (lNoEdge <= 0)
			{
				dEpoxyMeasureValue_BottomPos = lInspectROIRow2 - 2;
				dEpoxyMeasureValue_BottomOffset = 0;
			}
			else
			{
				TupleAbs(Amplitude, &Amplitude);
				TupleSortIndex(Amplitude, &Indices);
				TupleInverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();

				if (iEdgeMeasureDir == 1)		// Y Dir
				{
					dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
					iEdgePosY = (int)dEgePosY;

					dEpoxyMeasureValue_BottomPos = dEgePosY;
					dEpoxyMeasureValue_BottomOffset = (double)lInspectROIRow2 - dEgePosY;
				}
				else
				{
					dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
					iEdgePosX = (int)dEgePosX;
				}
			}

			dEpoxyMeasureValue_Length = dEpoxyMeasureValue_BottomPos - dEpoxyMeasureValue_TopPos;

			// Edge Measure (Height / X-Dir)

			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			int iHeightMeasureStep = 10;
			int iHeightMeasureLeft = lInspectROICol1 + 5;
			int iHeightMeasureRight = lInspectROICol2;
			int iHeightMeasureHalfWidth = 10;
			/////////////////////////////////////////////////////////////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			iEdgeMeasureDir = 0;
			iEdgeMeasurePos = 1;
			iEdgeMeasureGv = 1;
			dEdgeMeasureSmFactor = 1.0;
			iEdgeMeasureEdgeStr = Param.m_iNeckEpoxyMeasureRightEdgeStr;
			/////////////////////////////////////////////////////////////////////////////////////////////////////////

			double dEpoxyHeight;
			HTuple HEpoxyHeight;
			TupleGenConst(0, 0, &HEpoxyHeight);

			for (int iHeightMeasurePos = (int)dEpoxyMeasureValue_TopPos; iHeightMeasurePos <= (int)dEpoxyMeasureValue_BottomPos; iHeightMeasurePos += iHeightMeasureStep)
			{
				/////////////////////////////////////////////////////////////////////////////////////////////////////////
				GenRectangle1(&HEdgeMeasureRgn, (int)iHeightMeasurePos - iHeightMeasureHalfWidth, iHeightMeasureLeft, (int)iHeightMeasurePos + iHeightMeasureHalfWidth, iHeightMeasureRight);
				/////////////////////////////////////////////////////////////////////////////////////////////////////////

				SmallestRectangle1(HEdgeMeasureRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

				lROIRow1 = HlROIRow1.L();
				lROICol1 = HlROICol1.L();
				lROIRow2 = HlROIRow2.L();
				lROICol2 = HlROICol2.L();

				dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
				dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

				iROISizeX = lROICol2 - lROICol1;
				iROISizeY = lROIRow2 - lROIRow1;

				if (iEdgeMeasureDir == 1)		// Y Dir
				{
					GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2, iROISizeY / 2, iROISizeX / 2, lImageWidth,
						lImageHeight, "nearest_neighbor", &MeasureHandle);

					if (iEdgeMeasurePos == 0)
					{
						if (iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 1)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 2)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (iEdgeMeasurePos == 1)
					{
						if (iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 1)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 2)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (iEdgeMeasurePos == 2)
					{
						if (iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 1)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 2)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}
				else		// X Dir
				{
					GenMeasureRectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14, iROISizeX / 2, iROISizeY / 2, lImageWidth,
						lImageHeight, "nearest_neighbor", &MeasureHandle);

					if (iEdgeMeasurePos == 0)
					{
						if (iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 1)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 2)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (iEdgeMeasurePos == 1)
					{
						if (iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 1)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 2)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (iEdgeMeasurePos == 2)
					{
						if (iEdgeMeasureGv == 0)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 1)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (iEdgeMeasureGv == 2)
							MeasurePos(HImage, MeasureHandle, dEdgeMeasureSmFactor, (double)iEdgeMeasureEdgeStr, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}

				CloseMeasure(MeasureHandle);

				TupleLength(Amplitude, &HNoEdge);
				lNoEdge = HNoEdge.L();
				if (lNoEdge > 0)
				{
					TupleAbs(Amplitude, &Amplitude);
					TupleSortIndex(Amplitude, &Indices);
					TupleInverse(Indices, &Inverted);
					MaxAmpEdgeIndex = Inverted[0].L();

					if (iEdgeMeasureDir == 1)		// Y Dir
					{
						;
					}
					else
					{
						dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
						dEpoxyHeight = dEgePosX - (double)lInspectROICol1;
						if (dEpoxyHeight < 0)
							dEpoxyHeight = 0;

						TupleConcat(HEpoxyHeight, HTuple(dEpoxyHeight), &HEpoxyHeight);
					}
				}
			}

			TupleLength(HEpoxyHeight, &HNoEdge);
			lNoEdge = HNoEdge.L();

			if (lNoEdge > 0)
			{
				HTuple HMin, HMax;

				TupleMin(HEpoxyHeight, &HMin);
				dEpoxyMeasureValue_HeightMin = HMin[0].D();
				TupleMax(HEpoxyHeight, &HMax);
				dEpoxyMeasureValue_HeightMax = HMax[0].D();
			}

			BOOL bDefect = FALSE;

			if (dEpoxyMeasureValue_HeightMax >= Param.m_iNeckEpoxyToleranceHeightMax)
				bDefect = TRUE;
			if (dEpoxyMeasureValue_HeightMin < Param.m_iNeckEpoxyToleranceHeightMin)
				bDefect = TRUE;
			if (dEpoxyMeasureValue_Length < Param.m_iNeckEpoxyToleranceLength)
				bDefect = TRUE;
			if (dEpoxyMeasureValue_TopOffset >= Param.m_iNeckEpoxyToleranceTopPosOffset)
				bDefect = TRUE;
			if (dEpoxyMeasureValue_BottomOffset >= Param.m_iNeckEpoxyToleranceBottomPosOffset)
				bDefect = TRUE;

			if (bDefect)
				HDefectRgn = HInspectAreaRgn;

			*pdEpoxyMeasureValue_TopOffset = dEpoxyMeasureValue_TopOffset;
			*pdEpoxyMeasureValue_BottomOffset = dEpoxyMeasureValue_BottomOffset;
			*pdEpoxyMeasureValue_Length = dEpoxyMeasureValue_Length;
			*pdEpoxyMeasureValue_HeightMax = dEpoxyMeasureValue_HeightMax;
			*pdEpoxyMeasureValue_HeightMin = dEpoxyMeasureValue_HeightMin;
		}
		else if (Param.m_iNeckEpoxyInspectMethod == NECK_EPOXY_INSPECT_BLOB)
		{
			//if (Param.m_bUseSurfaceInspection)
			//{
			int iSurfaceInspectionArea;
			double dSurfaceInspectioXLength, dSurfaceInspectionYLength;

			HSurfaceAlgoDefectRgn = SurfaceInspectionAlgorithm(0, HImage, HDCImage, HROIRgn, Param, &iSurfaceInspectionArea, &dSurfaceInspectioXLength, &dSurfaceInspectionYLength, pHMaskRgn, NULL);

			if (THEAPP.m_pGFunction->ValidHRegion(HSurfaceAlgoDefectRgn) == TRUE)
			{
				Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
				Hlong lROIUpperY, lROILowerY, lCurYPos;
				HObject HSurfaceAlgoDefectMaxRgn;

				// ROI
				SmallestRectangle1(HROIRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
				lROIUpperY = HlLTPointY[0].L();
				lROILowerY = HlRBPointY[0].L();

				// All Blob
				SmallestRectangle1(HSurfaceAlgoDefectRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
				lLTPointY = HlLTPointY[0].L();
				lLTPointX = HlLTPointX[0].L();
				lRBPointY = HlRBPointY[0].L();
				lRBPointX = HlRBPointX[0].L();

				dEpoxyMeasureValue_TopOffset = (double)(lLTPointY - lROIUpperY + 1);
				if (dEpoxyMeasureValue_TopOffset < 0)
					dEpoxyMeasureValue_TopOffset = 0;

				dEpoxyMeasureValue_BottomOffset = (double)(lROILowerY - lRBPointY + 1);
				if (dEpoxyMeasureValue_BottomOffset < 0)
					dEpoxyMeasureValue_BottomOffset = 0;

				dEpoxyMeasureValue_HeightMax = (double)(lRBPointX - lLTPointX + 1);

				HTuple HRow, HColumnBegin, HColumnEnd;
				Hlong iRegionPointNum;
				Hlong lSum, lMinEpoxyHeight, lEpoxyHeight;
				Hlong lBlobLength;

				lBlobLength = lRBPointY - lLTPointY;

				GetRegionRuns(HSurfaceAlgoDefectRgn, &HRow, &HColumnBegin, &HColumnEnd);
				iRegionPointNum = HRow.Length();

				lMinEpoxyHeight = 100000000;

				if (iRegionPointNum > 0)
				{
					lSum = 0;

					for (int iii = 0; iii < iRegionPointNum; iii++)
					{
						lCurYPos = HRow[iii].L();

						lEpoxyHeight = HColumnEnd[iii].L() - lLTPointX + 1;
						lSum += lEpoxyHeight;

						if (lCurYPos > lBlobLength / 10 && lCurYPos < (lBlobLength - lBlobLength / 10))
						{
							if (lEpoxyHeight < lMinEpoxyHeight)
								lMinEpoxyHeight = lEpoxyHeight;
						}
					}

					lSum = lSum / iRegionPointNum;

					dEpoxyMeasureValue_HeightMin = (double)lMinEpoxyHeight;
				}

				Connection(HSurfaceAlgoDefectRgn, &HSurfaceAlgoDefectMaxRgn);
				SelectShapeStd(HSurfaceAlgoDefectMaxRgn, &HSurfaceAlgoDefectMaxRgn, "max_area", 70);

				// Max Blob
				SmallestRectangle1(HSurfaceAlgoDefectMaxRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
				lLTPointY = HlLTPointY.L();
				lLTPointX = HlLTPointX.L();
				lRBPointY = HlRBPointY.L();
				lRBPointX = HlRBPointX.L();

				dEpoxyMeasureValue_Length = (double)(lRBPointY - lLTPointY + 1);
			}

			BOOL bDefect = FALSE;

			if (dEpoxyMeasureValue_HeightMax >= Param.m_iNeckEpoxyToleranceHeightMax)
				bDefect = TRUE;
			if (dEpoxyMeasureValue_HeightMin < Param.m_iNeckEpoxyToleranceHeightMin)
				bDefect = TRUE;
			if (dEpoxyMeasureValue_Length < Param.m_iNeckEpoxyToleranceLength)
				bDefect = TRUE;
			if (dEpoxyMeasureValue_TopOffset >= Param.m_iNeckEpoxyToleranceTopPosOffset)
				bDefect = TRUE;
			if (dEpoxyMeasureValue_BottomOffset >= Param.m_iNeckEpoxyToleranceBottomPosOffset)
				bDefect = TRUE;

			if (bDefect)
				HDefectRgn = HSurfaceAlgoDefectRgn;

			*pdEpoxyMeasureValue_TopOffset = dEpoxyMeasureValue_TopOffset;
			*pdEpoxyMeasureValue_BottomOffset = dEpoxyMeasureValue_BottomOffset;
			*pdEpoxyMeasureValue_Length = dEpoxyMeasureValue_Length;
			*pdEpoxyMeasureValue_HeightMax = dEpoxyMeasureValue_HeightMax;
			*pdEpoxyMeasureValue_HeightMin = dEpoxyMeasureValue_HeightMin;
			//			}
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::NeckEpoxyMeasureAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::NeckEpoxyCrackAlgorithm(HObject HImage, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, HObject *pHMaskRgn)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		GenEmptyObj(pHLineFitXLD);

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(HImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

		pImageData = (BYTE*)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

		OpeningCircle(HInspectAreaRgn, &HInspectAreaRgn, (double)Param.m_iNeckEpoxyCrackOpeningSize + 0.5);

		HObject HUpperBarRgn, HLowerBarRgn, HRightBarRgn;
		HObject HBoundaryRgn, HBoundaryInspectRgn;
		HObject HInspectAreaConnRgn, HSelectedRgn;

		Connection(HInspectAreaRgn, &HInspectAreaConnRgn);
		SelectShape(HInspectAreaConnRgn, &HInspectAreaConnRgn, "height", "and", (double)Param.m_iNeckEpoxyCrackYLength, 99999);

		Hlong lNoBlob;
		HTuple HlNoBlob;
		CountObj(HInspectAreaConnRgn, &HlNoBlob);
		lNoBlob = HlNoBlob.L();

		GenEmptyObj(&HBoundaryInspectRgn);

		for (int i = 0; i < lNoBlob; i++)
		{
			SelectObj(HInspectAreaConnRgn, &HSelectedRgn, i + 1);

			SmallestRectangle1(HSelectedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
			lLTPointY = HlLTPointY[0].L();
			lLTPointX = HlLTPointX[0].L();
			lRBPointY = HlRBPointY[0].L();
			lRBPointX = HlRBPointX[0].L();

			GenRectangle1(&HUpperBarRgn, lLTPointY, lLTPointX, lLTPointY, lRBPointX);
			GenRectangle1(&HLowerBarRgn, lRBPointY, lLTPointX, lRBPointY, lRBPointX);
			GenRectangle1(&HRightBarRgn, lLTPointY, lRBPointX - 3, lRBPointY, lRBPointX);

			Boundary(HSelectedRgn, &HBoundaryRgn, "inner");
			Difference(HBoundaryRgn, HUpperBarRgn, &HBoundaryRgn);
			Difference(HBoundaryRgn, HLowerBarRgn, &HBoundaryRgn);
			Connection(HBoundaryRgn, &HBoundaryRgn);
			SelectShapeProto(HBoundaryRgn, HRightBarRgn, &HBoundaryRgn, "overlaps_abs", 1, MAX_DEF);
			DilationRectangle1(HBoundaryRgn, &HBoundaryRgn, Param.m_iNeckEpoxyCrackBoundaryWidth * 2 + 1, 1);

			if (Param.m_iNeckEpoxyCrackDir == NECK_EPOXY_CRACK_DIR_LEFT)
				Intersection(HBoundaryRgn, HInspectAreaRgn, &HBoundaryRgn);
			else if (Param.m_iNeckEpoxyCrackDir == NECK_EPOXY_CRACK_DIR_RIGHT)
			{
				Difference(HBoundaryRgn, HInspectAreaRgn, &HBoundaryRgn);
				Connection(HBoundaryRgn, &HBoundaryRgn);
				SelectShapeStd(HBoundaryRgn, &HBoundaryRgn, "max_area", 70);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryRgn))
				ConcatObj(HBoundaryInspectRgn, HBoundaryRgn, &HBoundaryInspectRgn);
		}

		int iSurfaceInspectionArea;
		double dSurfaceInspectioXLength, dSurfaceInspectionYLength;

		if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryInspectRgn))
		{
			Union1(HBoundaryInspectRgn, &HBoundaryInspectRgn);
			HDefectRgn = SurfaceInspectionAlgorithm(0, HImage, HDCImage, HBoundaryInspectRgn, Param, &iSurfaceInspectionArea, &dSurfaceInspectioXLength, &dSurfaceInspectionYLength, pHMaskRgn, NULL);
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::NeckEpoxyCrackAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

HObject Algorithm::NeckEpoxyCrackAlgorithm_XDir(HObject HImage, HObject HDCImage, HObject HROIRgn, CAlgorithmParam Param, HObject *pHLineFitXLD, HObject *pHMaskRgn)
{
	try
	{
		BOOL bDebugSave = FALSE;

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		GenEmptyObj(pHLineFitXLD);

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(HImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

		pImageData = (BYTE*)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		if (bDebugSave)
		{
			WriteImage(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

		OpeningCircle(HInspectAreaRgn, &HInspectAreaRgn, (double)Param.m_iNeckEpoxyCrackOpeningSize + 0.5);

		HObject HUpperBarRgn, HLowerBarRgn, HRightBarRgn;
		HObject HBoundaryRgn, HBoundaryInspectRgn;
		HObject HInspectAreaConnRgn, HSelectedRgn;

		Connection(HInspectAreaRgn, &HInspectAreaConnRgn);
		SelectShape(HInspectAreaConnRgn, &HInspectAreaConnRgn, "width", "and", (double)Param.m_iNeckEpoxyCrackYLength, 99999);

		Hlong lNoBlob;
		HTuple HlNoBlob;
		CountObj(HInspectAreaConnRgn, &HlNoBlob);
		lNoBlob = HlNoBlob.L();

		GenEmptyObj(&HBoundaryInspectRgn);

		for (int i = 0; i < lNoBlob; i++)
		{
			SelectObj(HInspectAreaConnRgn, &HSelectedRgn, i + 1);

			SmallestRectangle1(HSelectedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
			lLTPointY = HlLTPointY[0].L();
			lLTPointX = HlLTPointX[0].L();
			lRBPointY = HlRBPointY[0].L();
			lRBPointX = HlRBPointX[0].L();

			GenRectangle1(&HUpperBarRgn, lLTPointY, lRBPointX, lRBPointY, lRBPointX); //Right Bar
			GenRectangle1(&HLowerBarRgn, lLTPointY, lLTPointX, lRBPointY, lLTPointX); //Left Bar
			GenRectangle1(&HRightBarRgn, lRBPointY - 3, lLTPointX, lRBPointY, lRBPointX); //Lower Bar

			Boundary(HSelectedRgn, &HBoundaryRgn, "inner");
			Difference(HBoundaryRgn, HUpperBarRgn, &HBoundaryRgn);
			Difference(HBoundaryRgn, HLowerBarRgn, &HBoundaryRgn);
			Connection(HBoundaryRgn, &HBoundaryRgn);
			SelectShapeProto(HBoundaryRgn, HRightBarRgn, &HBoundaryRgn, "overlaps_abs", 1, MAX_DEF);
			DilationRectangle1(HBoundaryRgn, &HBoundaryRgn, Param.m_iNeckEpoxyCrackBoundaryWidth * 2 + 1, 1);

			if (Param.m_iNeckEpoxyCrackDir == NECK_EPOXY_CRACK_DIR_LEFT)
				Intersection(HBoundaryRgn, HInspectAreaRgn, &HBoundaryRgn);
			else if (Param.m_iNeckEpoxyCrackDir == NECK_EPOXY_CRACK_DIR_RIGHT)
			{
				Difference(HBoundaryRgn, HInspectAreaRgn, &HBoundaryRgn);
				Connection(HBoundaryRgn, &HBoundaryRgn);
				SelectShapeStd(HBoundaryRgn, &HBoundaryRgn, "max_area", 70);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryRgn))
				ConcatObj(HBoundaryInspectRgn, HBoundaryRgn, &HBoundaryInspectRgn);
		}

		int iSurfaceInspectionArea;
		double dSurfaceInspectioXLength, dSurfaceInspectionYLength;

		if (THEAPP.m_pGFunction->ValidHRegion(HBoundaryInspectRgn))
		{
			Union1(HBoundaryInspectRgn, &HBoundaryInspectRgn);
			HDefectRgn = SurfaceInspectionAlgorithm(0, HImage, HDCImage, HBoundaryInspectRgn, Param, &iSurfaceInspectionArea, &dSurfaceInspectioXLength, &dSurfaceInspectionYLength, pHMaskRgn, NULL);
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::NeckEpoxyCrackAlgorithm_XDir] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

// HObject *HConcatDefect: 검사 조건 만족한 모든 Blob Union
int Algorithm::ApplyInspectionCondition(int iVisionCamType, HObject HDefectRgn, HObject *HConcatDefect,
	double dMinAreaCondition, double dMidAreaCondition, double dMaxAreaCondition, int iMinNumAccept, int iMidNumAccept, int iMaxNumAccept,
	CString strPCID, CString strEquipNo, CString strModelName, int iMzNo, CString strLotID, int iTrayNo, int iModuleNo, CString strModuleBarcodeID, CString strVisionName_Short,
	int iPcNo, int iStageNo, CString strInspectionTypeShort, CString strDefectTypeShort)
{
	try
	{
		HObject HSelectedRgn;
		Hlong lNoDefect;
		HTuple Area, Row, Column;
		int TopCountSmall, TopCountMid, TopCountBiG;

		int iSortingDefectNumber = -1;		// 불량이면 1

		HObject HDefectConnRgn;
		Connection(HDefectRgn, &HDefectConnRgn);
		HTuple HlNoDefect;
		CountObj(HDefectConnRgn, &HlNoDefect);		// Number of Defect Blob 
		lNoDefect = HlNoDefect.L();
		int i;

		HObject HMinAreaDefectRgn;
		GenEmptyObj(&HMinAreaDefectRgn);

		for (i = 0; i < lNoDefect; i++)
		{
			SelectObj(HDefectConnRgn, &HSelectedRgn, i + 1);

			if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
				continue;

			AreaCenter(HSelectedRgn, &Area, &Row, &Column);
			double dArea = ceil((Area[0].D() * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelArea()) / 1000) / 1000;		////////// 단위 mm2으로 변환

			if (dArea >= dMinAreaCondition) // Defect Area >= Min Area
				ConcatObj(HMinAreaDefectRgn, HSelectedRgn, &HMinAreaDefectRgn);
		}

		// Defect Counting & Get Defect Region

		GenEmptyObj(HConcatDefect);	// 불량 검사 조건에 만족하는 전체 불량

		HObject HTopDefectRgn;
		GenEmptyObj(&HTopDefectRgn);

		HObject HMinDefectRgn, HMidDefectRgn, HMaxDefectRgn;
		Hlong lNoMinAreaDefect;

		GenEmptyObj(&HMinDefectRgn);
		GenEmptyObj(&HMidDefectRgn);
		GenEmptyObj(&HMaxDefectRgn);

		lNoMinAreaDefect = 0;
		HTuple HlNoMinAreaDefect;
		CountObj(HMinAreaDefectRgn, &HlNoMinAreaDefect);
		lNoMinAreaDefect = HlNoMinAreaDefect.L();

		TopCountSmall = 0;
		TopCountMid = 0;
		TopCountBiG = 0;

		for (i = 0; i < lNoMinAreaDefect; i++)
		{
			SelectObj(HMinAreaDefectRgn, &HSelectedRgn, i + 1);

			if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
				continue;

			AreaCenter(HSelectedRgn, &Area, &Row, &Column);
			double dArea = ceil((Area[0].D() * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelArea()) / 1000) / 1000;	////////// unit: mm2

			if (dArea >= dMinAreaCondition)
			{
				TopCountSmall++;
				ConcatObj(HMinDefectRgn, HSelectedRgn, &HMinDefectRgn);
			}
			if (dArea >= dMidAreaCondition)
			{
				TopCountMid++;
				ConcatObj(HMidDefectRgn, HSelectedRgn, &HMidDefectRgn);
			}
			if (dArea >= dMaxAreaCondition)
			{
				TopCountBiG++;
				ConcatObj(HMaxDefectRgn, HSelectedRgn, &HMaxDefectRgn);
			}
		}

		if (TopCountBiG > 0)
		{
			ConcatObj(HTopDefectRgn, HMaxDefectRgn, &HTopDefectRgn);
		}
		if (TopCountMid > iMaxNumAccept)
		{
			ConcatObj(HTopDefectRgn, HMidDefectRgn, &HTopDefectRgn);
		}
		if (TopCountSmall > iMidNumAccept)
		{
			ConcatObj(HTopDefectRgn, HMinDefectRgn, &HTopDefectRgn);
		}

		CString strResult = "G";
		if (THEAPP.m_pGFunction->ValidHRegion(HTopDefectRgn))
		{
			Union1(HTopDefectRgn, HConcatDefect);

			iSortingDefectNumber = 1;
			strResult = "NG";
		}

		SYSTEMTIME time;
		GetLocalTime(&time);
		CString sDay;
		sDay.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
		CString sTime;
		sTime.Format("%s %02d:%02d:%02d", sDay, time.wHour, time.wMinute, time.wSecond);

		CString sRunInfo;
		sRunInfo.Format("%s\t%s\t%s\t%s\t%s\t%d\t%s\t%d\t%d\t%s",
			sDay,
			sTime,
			strPCID,
			strEquipNo,
			strModelName,
			iMzNo,
			strLotID,
			iTrayNo,
			iModuleNo,
			strModuleBarcodeID);

		CString sInspectROIInfo;
		sInspectROIInfo.Format("%s%d\t%d\t%s\t%s",
			strVisionName_Short, iPcNo,
			iStageNo,
			strInspectionTypeShort,
			strDefectTypeShort);

		CString sInspectionConditionResult;
		sInspectionConditionResult.Format("%s\t%d\t%.3lf\t%d\t%d\t%.3lf\t%d\t%d\t%.3lf\t%d",
			strResult,
			lNoMinAreaDefect,
			dMinAreaCondition,
			iMidNumAccept,
			TopCountSmall,
			dMidAreaCondition,
			iMaxNumAccept,
			TopCountMid,
			dMaxAreaCondition,
			TopCountBiG);

		CString sInspectConditionLog;
		sInspectConditionLog.Format("%s\t%s\t%s",
			sRunInfo,
			sInspectROIInfo,
			sInspectionConditionResult
		);

		if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveInspectConditionLog(THEAPP.m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sInspectConditionLog);
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveInspectConditionLog(THEAPP.m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sInspectConditionLog);
		}
		else
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_INSPECT_CONDITION;
				pSaveLogThreadIDParam_Edge->strPath = THEAPP.m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
				pSaveLogThreadIDParam_Edge->strType1 = sInspectConditionLog;
				pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
			}
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_INSPECT_CONDITION;
				pSaveLogThreadIDParam_Edge->strPath = THEAPP.m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
				pSaveLogThreadIDParam_Edge->strType1 = sInspectConditionLog;
				pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
			}
		}

		return iSortingDefectNumber;	// 불량 1, 양품 -1
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [Algorithm::ApplyInspectionCondition] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return -2;
	}

}

BOOL Algorithm::OCRAlgorithm(HObject HImage, HObject HROIRgn, CAlgorithmParam Param)
{
	try
	{


		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::OCRAlgorithm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);

		return FALSE;
	}
}

void Algorithm::WriteOcrandBarcodeInfo(CFile* file, SYSTEMTIME time)
{
}

int Algorithm::OutPutWidthCharacterCalculate(int iType, CAlgorithmParam Param, long lWholeWidth, double lRowF, double lColF, double lRowS, double lColS)
{
	try
	{
		int iLineAveVal = 0;

		if (iType == DEF_OCR_FIRSTLINE)
		{
			BOOL bFirstSecondSameFlag = FALSE;
			if (lColF < Param.m_lCharPlateWidth && lColF > 0)
			{
				lColF = Param.m_lCharPlateWidth;
				if (lColF <= 0)
				{
					strLog.Format("OutPutWidthCharacterCalculate lColF FirstLine1 <= 0");
					THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
					return -1;
				}
			}
			else if (lColF < 1)
			{
				bFirstSecondSameFlag = TRUE;
			}

			if ((Param.m_lCharPlateWidth * 4) >= abs(lColS - lColF))
			{
				if (bFirstSecondSameFlag)
				{
					if (lColS < lWholeWidth - Param.m_lCharPlateWidth)
					{
						iLineAveVal = (abs(lColS - lColF)) / 4;
						if (iLineAveVal <= 0)
						{
							strLog.Format("OutPutWidthCharacterCalculate iFLineAveVal_3_Number <= 0");
							THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
							return -1;
						}
					}
					else
					{
						iLineAveVal = abs(lColS - lColF) / 5;
						if (iLineAveVal <= 0)
						{
							strLog.Format("OutPutWidthCharacterCalculate iFLineAveVal_5_Number <= 0");
							THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
							return -1;
						}
					}
				}
				else
				{
					if (lColF > Param.m_lCharPlateWidth * 2)
					{
						if (lColS < lWholeWidth - Param.m_lCharPlateWidth)
						{
							iLineAveVal = (abs(lColS - lColF)) / 3;
							if (iLineAveVal <= 0)
							{
								strLog.Format("OutPutWidthCharacterCalculate iFLineAveVal_3_Number <= 0");
								THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
								return -1;
							}
						}
						else
						{
							iLineAveVal = (abs(lColS - lColF)) / 4;
							if (iLineAveVal <= 0)
							{
								strLog.Format("OutPutWidthCharacterCalculate iFLineAveVal_4_Number <= 0");
								THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
								HObject HDefectRgn;
								GenEmptyObj(&HDefectRgn);
								return -1;
							}
						}
					}
					else
					{
						if (lColS < lWholeWidth - Param.m_lCharPlateWidth)
						{
							iLineAveVal = (abs(lColS - lColF)) / 4;
							if (iLineAveVal <= 0)
							{
								strLog.Format("OutPutWidthCharacterCalculate iFLineAveVal_3_Number <= 0");
								THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
								return -1;
							}
						}
						else
						{
							iLineAveVal = abs(lColS - lColF) / 5;
							if (iLineAveVal <= 0)
							{
								strLog.Format("OutPutWidthCharacterCalculate iFLineAveVal_5_Number <= 0");
								THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
								return -1;
							}
						}
					}

				}
			}
			else
			{
				iLineAveVal = abs(lColS - lColF) / 5;
				if (iLineAveVal <= 0)
				{
					strLog.Format("OutPutWidthCharacterCalculate iFLineAveVal_5_Number <= 0");
					THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
					return -1;
				}
			}
		}
		else
		{
			if ((Param.m_lCharPlateWidth * 5) > abs(lColS - lColF))
			{
				if (lColS < lWholeWidth - Param.m_lCharPlateWidth)
				{
					iLineAveVal = (abs(lColS - lColF)) / 4;
					if (iLineAveVal <= 0)
					{
						strLog.Format("OutPutWidthCharacterCalculate Second or Third_4_Number <= 0");
						THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
						return -1;
					}
				}
				else
				{
					iLineAveVal = (abs(lColS - lColF)) / 5;
					if (iLineAveVal <= 0)
					{
						strLog.Format("OutPutWidthCharacterCalculate Second or Third_5_Number <= 0");
						THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
						return -1;
					}
				}
			}
			else
			{
				iLineAveVal = (abs(lColS)) / 6;
				if (iLineAveVal <= 0)
				{
					strLog.Format("OutPutWidthCharacterCalculate Second or Third <= 0");
					THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
					return -1;
				}
			}
		}

		return iLineAveVal;
	}
	catch (...)
	{
	}
}

HObject Algorithm::CalculateWholeOcrArea(HObject HRectangleWholeArea, CAlgorithmParam Param)
{
	try
	{
		HObject HRectangleWholeArea;
		GenEmptyObj(&HRectangleWholeArea);

		return HRectangleWholeArea;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::CalculateWholeOcrArea] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

BOOL Algorithm::GetShapeImage(BOOL bImageCrop, HObject *pHImage, int *piShapeRawImageIdx, HObject HROIRgn, CAlgorithmParam Param, HObject *pHShapeImage)
{
	try
	{
		auto log_time_start = std::chrono::high_resolution_clock::now();

		BOOL bDebugSave = FALSE;

		//////////////////////////////////////////////////////////////////////////
		///  Parameters

		int iGrayThresHigh, iGrayThresLow;
		double dZoomFactor;
		double dfH, dfL;
		double dSigma;
		int iBinominalFilterSize;

		// thresholds (high and low)
		iGrayThresLow = 80;
		iGrayThresHigh = 170;

		// zoom factor
		dZoomFactor = Param.m_dShapeImageReduceRatio;

		// limit curvature for ImageConverted
		dfH = 0.07;
		dfL = -0.07;

		// sigma for curvature smoothing
		dSigma = Param.m_dShapeCurvatureSmFactor * dZoomFactor;
		//		dSigma = 0.8 * dZoomFactor;

				// binominal filter width

		double dTemp = (double)Param.m_iShapeIlluminationFilterX * dZoomFactor;
		iBinominalFilterSize = (int)(dTemp + 0.5);
		if (iBinominalFilterSize % 2 == 0)
			iBinominalFilterSize += 1;

		//////////////////////////////////////////////////////////////////////////

		HObject HImageReduced1, HImageReduced2, HImageReduced3, HImageReduced4;
		HObject ImageIlluminate1, ImageIlluminate2, ImageIlluminate3, ImageIlluminate4;

		GenEmptyObj(pHShapeImage);

		HObject HInspectAreaRgn;
		GenEmptyObj(&HInspectAreaRgn);

		GetInspectArea(HROIRgn, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
		{
			GenImageProto(pHImage[piShapeRawImageIdx[0]], pHShapeImage, 127);
			return FALSE;
		}

		HTuple HImageWidth, HImageHeight;
		GetImageSize(pHImage[piShapeRawImageIdx[0]], &HImageWidth, &HImageHeight);

		HObject HRectTransRgn;
		ShapeTrans(HInspectAreaRgn, &HRectTransRgn, "rectangle1");

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

		SmallestRectangle1(HRectTransRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

		lLTPointY = HlLTPointY.L();
		lLTPointX = HlLTPointX.L();
		lRBPointY = HlRBPointY.L();
		lRBPointX = HlRBPointX.L();

		ReduceDomain(pHImage[piShapeRawImageIdx[0]], HRectTransRgn, &HImageReduced1);
		ReduceDomain(pHImage[piShapeRawImageIdx[1]], HRectTransRgn, &HImageReduced2);
		ReduceDomain(pHImage[piShapeRawImageIdx[2]], HRectTransRgn, &HImageReduced3);
		ReduceDomain(pHImage[piShapeRawImageIdx[3]], HRectTransRgn, &HImageReduced4);

		CropDomain(HImageReduced1, &HImageReduced1);
		CropDomain(HImageReduced2, &HImageReduced2);
		CropDomain(HImageReduced3, &HImageReduced3);
		CropDomain(HImageReduced4, &HImageReduced4);

		//BinomialFilter(HImageReduced1, &ImageIlluminate1, iBinominalFilterSize, iBinominalFilterSize);
		//BinomialFilter(HImageReduced2, &ImageIlluminate2, iBinominalFilterSize, iBinominalFilterSize);
		//BinomialFilter(HImageReduced3, &ImageIlluminate3, iBinominalFilterSize, iBinominalFilterSize);
		//BinomialFilter(HImageReduced4, &ImageIlluminate4, iBinominalFilterSize, iBinominalFilterSize);

		//0605
		MeanImage(HImageReduced1, &ImageIlluminate1, iBinominalFilterSize, iBinominalFilterSize);
		MeanImage(HImageReduced2, &ImageIlluminate2, iBinominalFilterSize, iBinominalFilterSize);
		MeanImage(HImageReduced3, &ImageIlluminate3, iBinominalFilterSize, iBinominalFilterSize);
		MeanImage(HImageReduced4, &ImageIlluminate4, iBinominalFilterSize, iBinominalFilterSize);

		if (dZoomFactor < 1)
		{
			ZoomImageFactor(ImageIlluminate1, &ImageIlluminate1, dZoomFactor, dZoomFactor, "bilinear");
			ZoomImageFactor(ImageIlluminate2, &ImageIlluminate2, dZoomFactor, dZoomFactor, "bilinear");
			ZoomImageFactor(ImageIlluminate3, &ImageIlluminate3, dZoomFactor, dZoomFactor, "bilinear");
			ZoomImageFactor(ImageIlluminate4, &ImageIlluminate4, dZoomFactor, dZoomFactor, "bilinear");
		}

		HObject HShapeProcessImage;
		GenEmptyObj(&HShapeProcessImage);
		ConcatObj(HShapeProcessImage, ImageIlluminate1, &HShapeProcessImage);
		ConcatObj(HShapeProcessImage, ImageIlluminate2, &HShapeProcessImage);
		ConcatObj(HShapeProcessImage, ImageIlluminate3, &HShapeProcessImage);
		ConcatObj(HShapeProcessImage, ImageIlluminate4, &HShapeProcessImage);

		HTuple hv_Tilts, hv_Slants;

		// LED Position (Top, Bottom, Left, Right)
		hv_Tilts.Clear();
		hv_Tilts[0] = 90;
		hv_Tilts[1] = 270;
		hv_Tilts[2] = 180;
		hv_Tilts[3] = 0;

		// LED Angle
		hv_Slants.Clear();
		hv_Slants[0] = HTuple(Param.m_dShapeLedAngle);
		hv_Slants[1] = HTuple(Param.m_dShapeLedAngle);
		hv_Slants[2] = HTuple(Param.m_dShapeLedAngle);
		hv_Slants[3] = HTuple(Param.m_dShapeLedAngle);

		HObject HHeightFieldImage, HGradientImage, HAlbedoImage, HMCurvatureImage;

		PhotometricStereo(HShapeProcessImage, &HHeightFieldImage, &HGradientImage, &HAlbedoImage, hv_Slants, hv_Tilts, "height_field", "poisson", HTuple(), HTuple());
		DerivateGauss(HHeightFieldImage, &HMCurvatureImage, dSigma, "mean_curvature");

		ScaleImage(HMCurvatureImage, &HMCurvatureImage, dZoomFactor, 0);
		InvertImage(HMCurvatureImage, &HMCurvatureImage);

		double dMulti, dAdd;
		dMulti = 255.0 / (dfH - dfL);
		dAdd = -dMulti * dfL;

		ScaleImage(HMCurvatureImage, &HMCurvatureImage, dMulti, dAdd);
		ConvertImageType(HMCurvatureImage, &HMCurvatureImage, "byte");

		if (dZoomFactor < 1)
			ZoomImageFactor(HMCurvatureImage, &HMCurvatureImage, 1.0 / dZoomFactor, 1.0 / dZoomFactor, "bilinear");

		MeanImage(HMCurvatureImage, &HMCurvatureImage, 3, 3);

		HObject HOrgImage, HTileInputImage, HReconstructImage;

		GenImageProto(pHImage[piShapeRawImageIdx[0]], &HOrgImage, 127);
		GenEmptyObj(&HTileInputImage);
		ConcatObj(HTileInputImage, HOrgImage, &HTileInputImage);
		ConcatObj(HTileInputImage, HMCurvatureImage, &HTileInputImage);

		HTuple HOffsetX, HOffsetY, HDefault;

		TupleGenConst(0, 0, &HOffsetX);
		TupleConcat(HOffsetX, HTuple(0), &HOffsetX);
		TupleConcat(HOffsetX, HTuple(lLTPointX), &HOffsetX);

		TupleGenConst(0, 0, &HOffsetY);
		TupleConcat(HOffsetY, HTuple(0), &HOffsetY);
		TupleConcat(HOffsetY, HTuple(lLTPointY), &HOffsetY);

		TupleGenConst(0, 0, &HDefault);
		TupleConcat(HDefault, HTuple(-1), &HDefault);
		TupleConcat(HDefault, HTuple(-1), &HDefault);

		TileImagesOffset(HTileInputImage, &HReconstructImage, HOffsetY, HOffsetX, HDefault, HDefault, HDefault, HDefault, HImageWidth, HImageHeight);

		if (bDebugSave)
		{
			WriteImage(HMCurvatureImage, "bmp", 0, "c:\\DualTest\\ShapeImage");
			WriteRegion(HInspectAreaRgn, "C:\\DualTest\\ShapeInspectArea.reg");
		}

		if (bImageCrop)
		{
			HObject HHighThresRgn, HLowThresRgn, HThresRgn;

			//Threshold(HReconstructImage, &HLowThresRgn, 0, iGrayThresLow);
			//Threshold(HReconstructImage, &HHighThresRgn, iGrayThresHigh, 255);
			//Union2(HHighThresRgn, HLowThresRgn, &HThresRgn);
			//Complement(HThresRgn, &HThresRgn);
			//PaintRegion(HThresRgn, HReconstructImage, &HMCurvatureImage, 127, "fill");

			HObject HCropRectRgn;

			ShapeTrans(HInspectAreaRgn, &HCropRectRgn, "rectangle1");
			ReduceDomain(HReconstructImage, HCropRectRgn, pHShapeImage);
			CropDomain(*pHShapeImage, pHShapeImage);
		}
		else
		{
			if (Param.m_iShapeImageMargin > 0)
				ErosionCircle(HInspectAreaRgn, &HInspectAreaRgn, (double)Param.m_iShapeImageMargin + 0.5);

			ReduceDomain(HReconstructImage, HInspectAreaRgn, pHShapeImage);
		}

		if (bDebugSave)
		{
			WriteImage(*pHShapeImage, "bmp", 0, "c:\\DualTest\\ShapeImage");
		}
		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("Get shape image(CPU), Time(ms): %d", log_time_ms);
		THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));

		return TRUE;
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

		strLog.Format("Halcon Exception [Algorithm::GetShapeImage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::GetPreLocalAlignResult(int iMzNo, int iPcVisionNo, CAlgorithmParam AlgorithmParam, HObject *pHROIHRegion, int iThreadIdx, int *piLocalAlignDeltaX, int *piLocalAlignDeltaY)
{
	try
	{
		*piLocalAlignDeltaX = 0;
		*piLocalAlignDeltaY = 0;

		BOOL bDebugSave = FALSE;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		int iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return TRUE;

		GTRegion *pInspectROIRgn;

		BOOL bFound = FALSE;

		int iLocalAlignImageNo = AlgorithmParam.m_iImageProcessLocalAlignImageIndex + 1;

		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
				continue;

			if (pInspectROIRgn->miTeachImageIndex == iLocalAlignImageNo)
			{
				if (pInspectROIRgn->miLocalAlignID == AlgorithmParam.m_iImageProcessLocalAlignROINo)
				{
					for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
						{
							bFound = TRUE;
							break;
						}
					}

					if (bFound)
						break;
				}
			}
		}

		if (bFound)
		{
			if (pInspectROIRgn->m_iLocalAlignDeltaX[iThreadIdx] != INVALID_ALIGN_RESULT)
				*piLocalAlignDeltaX = pInspectROIRgn->m_iLocalAlignDeltaX[iThreadIdx];

			if (pInspectROIRgn->m_iLocalAlignDeltaY[iThreadIdx] != INVALID_ALIGN_RESULT)
				*piLocalAlignDeltaY = pInspectROIRgn->m_iLocalAlignDeltaY[iThreadIdx];
		}

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::GetPreLocalAlignResult] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::ApplyLocalAlignResult(int iMzNo, int iPcVisionNo, CAlgorithmParam AlgorithmParam, HObject *pHROIHRegion, int iThreadIdx, double *dLocalAlignAngle, int *iLocalAlignDeltaX, int *iLocalAlignDeltaY)
{
	try
	{
		BOOL bDebugSave = FALSE;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		int iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return TRUE;

		GTRegion *pInspectROIRgn;
		GTRegion *pInspectROIRgn2;

		BOOL bFound = FALSE;
		BOOL bFoundPost = FALSE;

		int iLocalAlignImageNo = AlgorithmParam.m_iImageProcessLocalAlignImageIndex + 1;

		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
				continue;

			if (pInspectROIRgn->miTeachImageIndex == iLocalAlignImageNo)
			{
				if (pInspectROIRgn->miLocalAlignID == AlgorithmParam.m_iImageProcessLocalAlignROINo)
				{
					for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
						{
							bFound = TRUE;
							break;
						}
					}

					if (bFound)
						break;
				}
			}
		}

		int iPostLocalAlignImageNo = AlgorithmParam.m_iImageProcessPostLocalAlignImageIndex + 1;

		if (AlgorithmParam.m_bUseImageProcessPostLocalAlign)
		{
			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn2 = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn2 == NULL)
					continue;

				if (pInspectROIRgn2->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				if (pInspectROIRgn2->miTeachImageIndex == iPostLocalAlignImageNo)
				{
					if (pInspectROIRgn2->miLocalAlignID == AlgorithmParam.m_iImageProcessPostLocalAlignROINo)
					{
						for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
						{
							if (pInspectROIRgn2->m_AlgorithmParam[iTabIdx].m_bInspect)
							{
								bFoundPost = TRUE;
								break;
							}
						}

						if (bFoundPost)
							break;
					}
				}
			}
		}

		if (bFound)
		{
			if (AlgorithmParam.m_iImageProcessLocalAlignROIType == LOCAL_ALIGN_MATCHING_ROI_TYPE_MATCHING)
			{
				if (AlgorithmParam.m_iImageProcessLocalAlignMatchingApplyMethod == LOCAL_ALIGN_MATCHING_ROI_POSITION)
				{
					if (AlgorithmParam.m_bImageProcessLocalAlignPosX || AlgorithmParam.m_bImageProcessLocalAlignPosY || AlgorithmParam.m_bImageProcessLocalAlignAngle)	// Delta X/Y
					{
						HTuple HomMat2DLocal;
						double dDeltaX, dDeltaY, dDeltaAngle;
						dDeltaX = dDeltaY = dDeltaAngle = 0;

						if (AlgorithmParam.m_bImageProcessLocalAlignPosX && (pInspectROIRgn->m_iLocalAlignDeltaX[iThreadIdx] != INVALID_ALIGN_RESULT))
							dDeltaX = pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx] + pInspectROIRgn->m_iLocalAlignDeltaX[iThreadIdx];
						else
							dDeltaX = pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx];

						if (AlgorithmParam.m_bImageProcessLocalAlignPosY && (pInspectROIRgn->m_iLocalAlignDeltaY[iThreadIdx] != INVALID_ALIGN_RESULT))
							dDeltaY = pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx] + pInspectROIRgn->m_iLocalAlignDeltaY[iThreadIdx];
						else
							dDeltaY = pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx];

						if (AlgorithmParam.m_bImageProcessLocalAlignAngle && (pInspectROIRgn->m_dLocalAlignDeltaAngle[iThreadIdx] != INVALID_ALIGN_RESULT))
							dDeltaAngle = pInspectROIRgn->m_dLocalAlignDeltaAngle[iThreadIdx];

						if (iLocalAlignDeltaX != nullptr && iLocalAlignDeltaY != nullptr)
						{
							*iLocalAlignDeltaX = pInspectROIRgn->m_iLocalAlignDeltaX[iThreadIdx];
							*iLocalAlignDeltaY = pInspectROIRgn->m_iLocalAlignDeltaY[iThreadIdx];
						}

						if (dLocalAlignAngle != nullptr)
						{
							HTuple HAngleDeg;
							TupleDeg(HTuple(dDeltaAngle), &HAngleDeg);
							*dLocalAlignAngle = HAngleDeg[0].D();
						}

						VectorAngleToRigid(pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx], pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx], 0, dDeltaY, dDeltaX, dDeltaAngle, &HomMat2DLocal);
						AffineTransRegion(*pHROIHRegion, pHROIHRegion, HomMat2DLocal, "nearest_neighbor");
					}
				}
				else		// LOCAL_ALIGN_MATCHING_ROI_SHAPE
				{
					*pHROIHRegion = pInspectROIRgn->m_HLocalAlignShapeRgn[iThreadIdx];

					if (bDebugSave)
						WriteRegion(*pHROIHRegion, "c:\\DualTest\\LocalAlignShapeRgn.reg");
				}
			}
			else if (AlgorithmParam.m_iImageProcessLocalAlignROIType == LOCAL_ALIGN_MATCHING_ROI_TYPE_LINE_FIT)
			{
				Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
				HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

				SmallestRectangle1(*pHROIHRegion, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

				lLTPointY = HlLTPointY.L();
				lLTPointX = HlLTPointX.L();
				lRBPointY = HlRBPointY.L();
				lRBPointX = HlRBPointX.L();

				if (pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx] == NOT_ANGLE_LINE && pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx] == NOT_ANGLE_LINE)
				{
					Hlong lOffset = 0;

					if (AlgorithmParam.m_iImageProcessLocalAlignFitPos == LINE_FIT_XDIR_OBJECT_TOP)
					{
						if (pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT)
						{
							lOffset = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] - lLTPointY;
							MoveRegion(*pHROIHRegion, pHROIHRegion, lOffset, 0);
						}
					}
					else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos == LINE_FIT_XDIR_OBJECT_BOTTOM)
					{
						if (pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT)
						{
							lOffset = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] - lRBPointY;
							MoveRegion(*pHROIHRegion, pHROIHRegion, lOffset, 0);
						}
					}
					else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos == LINE_FIT_YDIR_OBJECT_LEFT)
					{
						if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT)
						{
							lOffset = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] - lLTPointX;
							MoveRegion(*pHROIHRegion, pHROIHRegion, 0, lOffset);
						}
					}
					else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos == LINE_FIT_YDIR_OBJECT_RIGHT)
					{
						if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT)
						{
							lOffset = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] - lRBPointX;
							MoveRegion(*pHROIHRegion, pHROIHRegion, 0, lOffset);
						}
					}
				}	// Horizontal/Vertial Line
				else
				{
					if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT &&
						pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT &&
						pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx] != INVALID_ALIGN_RESULT &&
						pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx] != INVALID_ALIGN_RESULT)
					{
						double dLineStartX, dLineStartY, dLineEndX, dLineEndY;

						dLineStartX = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx];
						dLineStartY = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx];
						dLineEndX = pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx];
						dLineEndY = pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx];

						POINT lLTPoint, lRTPoint, lLBPoint, lRBPoint;	// 4 Points(Polygon) of ROI
						double dIntersectPointX_1, dIntersectPointY_1, dIntersectPointX_2, dIntersectPointY_2;
						HTuple HIntersectX, HIntersectY, HParallel;

						HObject HPolygonRgn;
						HTuple HRows, HCols;

						GenEmptyObj(&HPolygonRgn);

						lLTPoint.x = lLTPointX;
						lLTPoint.y = lLTPointY;
						lRTPoint.x = lRBPointX;
						lRTPoint.y = lLTPointY;
						lLBPoint.x = lLTPointX;
						lLBPoint.y = lRBPointY;
						lRBPoint.x = lRBPointX;
						lRBPoint.y = lRBPointY;

						if (AlgorithmParam.m_iImageProcessLocalAlignFitPos == LINE_FIT_XDIR_OBJECT_TOP)
						{
							IntersectionLl(lLTPoint.y, lLTPoint.x, lLBPoint.y, lLBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
							dIntersectPointY_1 = HIntersectY[0].D();
							dIntersectPointX_1 = HIntersectX[0].D();

							IntersectionLl(lRTPoint.y, lRTPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
							dIntersectPointY_2 = HIntersectY[0].D();
							dIntersectPointX_2 = HIntersectX[0].D();

							lLTPoint.x = (int)(dIntersectPointX_1 + 0.5);
							lLTPoint.y = (int)(dIntersectPointY_1 + 0.5);
							lRTPoint.x = (int)(dIntersectPointX_2 + 0.5);
							lRTPoint.y = (int)(dIntersectPointY_2 + 0.5);
						}
						else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos == LINE_FIT_XDIR_OBJECT_BOTTOM)
						{
							IntersectionLl(lLTPoint.y, lLTPoint.x, lLBPoint.y, lLBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
							dIntersectPointY_1 = HIntersectY[0].D();
							dIntersectPointX_1 = HIntersectX[0].D();

							IntersectionLl(lRTPoint.y, lRTPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
							dIntersectPointY_2 = HIntersectY[0].D();
							dIntersectPointX_2 = HIntersectX[0].D();

							lLBPoint.x = (int)(dIntersectPointX_1 + 0.5);
							lLBPoint.y = (int)(dIntersectPointY_1 + 0.5);
							lRBPoint.x = (int)(dIntersectPointX_2 + 0.5);
							lRBPoint.y = (int)(dIntersectPointY_2 + 0.5);
						}
						else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos == LINE_FIT_YDIR_OBJECT_LEFT)
						{
							IntersectionLl(lLTPoint.y, lLTPoint.x, lRTPoint.y, lRTPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
							dIntersectPointY_1 = HIntersectY[0].D();
							dIntersectPointX_1 = HIntersectX[0].D();

							IntersectionLl(lLBPoint.y, lLBPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
							dIntersectPointY_2 = HIntersectY[0].D();
							dIntersectPointX_2 = HIntersectX[0].D();

							lLTPoint.x = (int)(dIntersectPointX_1 + 0.5);
							lLTPoint.y = (int)(dIntersectPointY_1 + 0.5);
							lLBPoint.x = (int)(dIntersectPointX_2 + 0.5);
							lLBPoint.y = (int)(dIntersectPointY_2 + 0.5);
						}
						else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos == LINE_FIT_YDIR_OBJECT_RIGHT)
						{
							IntersectionLl(lLTPoint.y, lLTPoint.x, lRTPoint.y, lRTPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
							dIntersectPointY_1 = HIntersectY[0].D();
							dIntersectPointX_1 = HIntersectX[0].D();

							IntersectionLl(lLBPoint.y, lLBPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
							dIntersectPointY_2 = HIntersectY[0].D();
							dIntersectPointX_2 = HIntersectX[0].D();

							lRTPoint.x = (int)(dIntersectPointX_1 + 0.5);
							lRTPoint.y = (int)(dIntersectPointY_1 + 0.5);
							lRBPoint.x = (int)(dIntersectPointX_2 + 0.5);
							lRBPoint.y = (int)(dIntersectPointY_2 + 0.5);
						}

						TupleGenConst(0, 0, &HRows);
						TupleGenConst(0, 0, &HCols);

						TupleConcat(HRows, lLTPoint.y, &HRows);
						TupleConcat(HCols, lLTPoint.x, &HCols);
						TupleConcat(HRows, lRTPoint.y, &HRows);
						TupleConcat(HCols, lRTPoint.x, &HCols);
						TupleConcat(HRows, lRBPoint.y, &HRows);
						TupleConcat(HCols, lRBPoint.x, &HCols);
						TupleConcat(HRows, lLBPoint.y, &HRows);
						TupleConcat(HCols, lLBPoint.x, &HCols);

						GenRegionPolygonFilled(&HPolygonRgn, HRows, HCols);

						Intersection(HPolygonRgn, *pHROIHRegion, pHROIHRegion);
					}
				}
			}

			if (AlgorithmParam.m_bUseImageProcessLocalAlignAddLineFit)
			{
				BOOL bFoundNew = FALSE;
				int iLocalAlignImageNo_2 = AlgorithmParam.m_iImageProcessLocalAlignImageIndex_2 + 1;

				for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
				{
					pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

					if (pInspectROIRgn == NULL)
						continue;

					if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
						continue;

					if (pInspectROIRgn->miTeachImageIndex == iLocalAlignImageNo_2)
					{
						if (pInspectROIRgn->miLocalAlignID == AlgorithmParam.m_iImageProcessLocalAlignROINo_2)
						{
							for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
							{
								if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
								{
									bFoundNew = TRUE;
									break;
								}
							}

							if (bFoundNew)
								break;
						}
					}
				}

				if (bFoundNew)
				{
					Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

					SmallestRectangle1(*pHROIHRegion, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

					lLTPointY = HlLTPointY.L();
					lLTPointX = HlLTPointX.L();
					lRBPointY = HlRBPointY.L();
					lRBPointX = HlRBPointX.L();

					if (pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx] == NOT_ANGLE_LINE && pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx] == NOT_ANGLE_LINE)
					{
						Hlong lOffset = 0;

						if (AlgorithmParam.m_iImageProcessLocalAlignFitPos_2 == LINE_FIT_XDIR_OBJECT_TOP)
						{
							if (pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT)
							{
								lOffset = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] - lLTPointY;
								MoveRegion(*pHROIHRegion, pHROIHRegion, lOffset, 0);
							}
						}
						else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos_2 == LINE_FIT_XDIR_OBJECT_BOTTOM)
						{
							if (pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT)
							{
								lOffset = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] - lRBPointY;
								MoveRegion(*pHROIHRegion, pHROIHRegion, lOffset, 0);
							}
						}
						else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos_2 == LINE_FIT_YDIR_OBJECT_LEFT)
						{
							if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT)
							{
								lOffset = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] - lLTPointX;
								MoveRegion(*pHROIHRegion, pHROIHRegion, 0, lOffset);
							}
						}
						else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos_2 == LINE_FIT_YDIR_OBJECT_RIGHT)
						{
							if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT)
							{
								lOffset = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] - lRBPointX;
								MoveRegion(*pHROIHRegion, pHROIHRegion, 0, lOffset);
							}
						}
					}	// Horizontal/Vertial Line
					else
					{
						if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT &&
							pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT &&
							pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx] != INVALID_ALIGN_RESULT &&
							pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx] != INVALID_ALIGN_RESULT)
						{
							double dLineStartX, dLineStartY, dLineEndX, dLineEndY;

							dLineStartX = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx];
							dLineStartY = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx];
							dLineEndX = pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx];
							dLineEndY = pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx];

							POINT lLTPoint, lRTPoint, lLBPoint, lRBPoint;	// 4 Points(Polygon) of ROI
							double dIntersectPointX_1, dIntersectPointY_1, dIntersectPointX_2, dIntersectPointY_2;
							HTuple HIntersectX, HIntersectY, HParallel;

							HObject HPolygonRgn;
							HTuple HRows, HCols;

							GenEmptyObj(&HPolygonRgn);

							lLTPoint.x = lLTPointX;
							lLTPoint.y = lLTPointY;
							lRTPoint.x = lRBPointX;
							lRTPoint.y = lLTPointY;
							lLBPoint.x = lLTPointX;
							lLBPoint.y = lRBPointY;
							lRBPoint.x = lRBPointX;
							lRBPoint.y = lRBPointY;

							if (AlgorithmParam.m_iImageProcessLocalAlignFitPos_2 == LINE_FIT_XDIR_OBJECT_TOP)
							{
								IntersectionLl(lLTPoint.y, lLTPoint.x, lLBPoint.y, lLBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
								dIntersectPointY_1 = HIntersectY[0].D();
								dIntersectPointX_1 = HIntersectX[0].D();

								IntersectionLl(lRTPoint.y, lRTPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
								dIntersectPointY_2 = HIntersectY[0].D();
								dIntersectPointX_2 = HIntersectX[0].D();

								lLTPoint.x = (int)(dIntersectPointX_1 + 0.5);
								lLTPoint.y = (int)(dIntersectPointY_1 + 0.5);
								lRTPoint.x = (int)(dIntersectPointX_2 + 0.5);
								lRTPoint.y = (int)(dIntersectPointY_2 + 0.5);
							}
							else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos_2 == LINE_FIT_XDIR_OBJECT_BOTTOM)
							{
								IntersectionLl(lLTPoint.y, lLTPoint.x, lLBPoint.y, lLBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
								dIntersectPointY_1 = HIntersectY[0].D();
								dIntersectPointX_1 = HIntersectX[0].D();

								IntersectionLl(lRTPoint.y, lRTPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
								dIntersectPointY_2 = HIntersectY[0].D();
								dIntersectPointX_2 = HIntersectX[0].D();

								lLBPoint.x = (int)(dIntersectPointX_1 + 0.5);
								lLBPoint.y = (int)(dIntersectPointY_1 + 0.5);
								lRBPoint.x = (int)(dIntersectPointX_2 + 0.5);
								lRBPoint.y = (int)(dIntersectPointY_2 + 0.5);
							}
							else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos_2 == LINE_FIT_YDIR_OBJECT_LEFT)
							{
								IntersectionLl(lLTPoint.y, lLTPoint.x, lRTPoint.y, lRTPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
								dIntersectPointY_1 = HIntersectY[0].D();
								dIntersectPointX_1 = HIntersectX[0].D();

								IntersectionLl(lLBPoint.y, lLBPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
								dIntersectPointY_2 = HIntersectY[0].D();
								dIntersectPointX_2 = HIntersectX[0].D();

								lLTPoint.x = (int)(dIntersectPointX_1 + 0.5);
								lLTPoint.y = (int)(dIntersectPointY_1 + 0.5);
								lLBPoint.x = (int)(dIntersectPointX_2 + 0.5);
								lLBPoint.y = (int)(dIntersectPointY_2 + 0.5);
							}
							else if (AlgorithmParam.m_iImageProcessLocalAlignFitPos_2 == LINE_FIT_YDIR_OBJECT_RIGHT)
							{
								IntersectionLl(lLTPoint.y, lLTPoint.x, lRTPoint.y, lRTPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
								dIntersectPointY_1 = HIntersectY[0].D();
								dIntersectPointX_1 = HIntersectX[0].D();

								IntersectionLl(lLBPoint.y, lLBPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
								dIntersectPointY_2 = HIntersectY[0].D();
								dIntersectPointX_2 = HIntersectX[0].D();

								lRTPoint.x = (int)(dIntersectPointX_1 + 0.5);
								lRTPoint.y = (int)(dIntersectPointY_1 + 0.5);
								lRBPoint.x = (int)(dIntersectPointX_2 + 0.5);
								lRBPoint.y = (int)(dIntersectPointY_2 + 0.5);
							}

							TupleGenConst(0, 0, &HRows);
							TupleGenConst(0, 0, &HCols);

							TupleConcat(HRows, lLTPoint.y, &HRows);
							TupleConcat(HCols, lLTPoint.x, &HCols);
							TupleConcat(HRows, lRTPoint.y, &HRows);
							TupleConcat(HCols, lRTPoint.x, &HCols);
							TupleConcat(HRows, lRBPoint.y, &HRows);
							TupleConcat(HCols, lRBPoint.x, &HCols);
							TupleConcat(HRows, lLBPoint.y, &HRows);
							TupleConcat(HCols, lLBPoint.x, &HCols);

							GenRegionPolygonFilled(&HPolygonRgn, HRows, HCols);

							Intersection(HPolygonRgn, *pHROIHRegion, pHROIHRegion);
						}
					}

				}	// if (bFoundNew)

			}	// if (AlgorithmParam.m_bUseImageProcessLocalAlignAddLineFit)

			if (bFoundPost) //VER 1.1.4.3 add
			{
				HTuple HomMat2DLocal;
				double dDeltaX, dDeltaY, dDeltaAngle;
				dDeltaX = dDeltaY = dDeltaAngle = 0;
				HTuple htLTy, htLTx, htRBy, htRBx, htCenterX, htCenterY;
				SmallestRectangle1(*pHROIHRegion, &htLTy, &htLTx, &htRBy, &htRBx);

				if (pInspectROIRgn2->m_iLocalAlignDeltaX[iThreadIdx] != INVALID_ALIGN_RESULT)
					dDeltaX = pInspectROIRgn2->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx] + pInspectROIRgn2->m_iLocalAlignDeltaX[iThreadIdx];
				else
					dDeltaX = pInspectROIRgn2->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx];

				if (pInspectROIRgn2->m_iLocalAlignDeltaY[iThreadIdx] != INVALID_ALIGN_RESULT)
					dDeltaY = pInspectROIRgn2->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx] + pInspectROIRgn2->m_iLocalAlignDeltaY[iThreadIdx];
				else
					dDeltaY = pInspectROIRgn2->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx];

				if (pInspectROIRgn2->m_dLocalAlignDeltaAngle[iThreadIdx] != INVALID_ALIGN_RESULT)
					dDeltaAngle = pInspectROIRgn2->m_dLocalAlignDeltaAngle[iThreadIdx];

				if (AlgorithmParam.m_iImageProcessPostLocalAlignFitPos == POST_LOCAL_ALIGN_ROTATION_CENTER_MODELCENTER)
					VectorAngleToRigid(dDeltaY, dDeltaX, 0, dDeltaY, dDeltaX, dDeltaAngle, &HomMat2DLocal);
				else
				{
					if (AlgorithmParam.m_iImageProcessPostLocalAlignFitPos == POST_LOCAL_ALIGN_ROTATION_CENTER_LEFTTOP)
					{
						htCenterY = htLTy;
						htCenterX = htLTx;
					}
					else if (AlgorithmParam.m_iImageProcessPostLocalAlignFitPos == POST_LOCAL_ALIGN_ROTATION_CENTER_RIGHTTOP)
					{
						htCenterY = htLTy;
						htCenterX = htRBx;
					}
					else if (AlgorithmParam.m_iImageProcessPostLocalAlignFitPos == POST_LOCAL_ALIGN_ROTATION_CENTER_LEFTBOTTOM)
					{
						htCenterY = htRBy;
						htCenterX = htLTx;
					}
					else if (AlgorithmParam.m_iImageProcessPostLocalAlignFitPos == POST_LOCAL_ALIGN_ROTATION_CENTER_RIGHTBOTTOM)
					{
						htCenterY = htRBy;
						htCenterX = htRBx;
					}
					VectorAngleToRigid(htCenterY, htCenterX, 0, htLTy, htRBx, dDeltaAngle, &HomMat2DLocal);
				}

				AffineTransRegion(*pHROIHRegion, pHROIHRegion, HomMat2DLocal, "nearest_neighbor");
			}
		}

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ApplyLocalAlignResult] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::ApplyDontCareResult(int iMzNo, int iPcVisionNo, CAlgorithmParam AlgorithmParam, HObject *pHROIHRegion, int iThreadIdx)
{
	try
	{
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		int iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return TRUE;

		GTRegion *pInspectROIRgn;

		BOOL bFound;
		int iDontCareImageNo;
		int iROIIndex;

		int iMargin;
		HObject HMarginRgn1, HMarginRgn2, HInspectRgn;
		Hlong lArea1, lArea2;
		double dCenterX, dCenterY;
		HTuple HlArea1, HlArea2, HdCenterX, HdCenterY;

		if (AlgorithmParam.m_bUseImageProcessDontCare)
		{
			bFound = FALSE;
			iDontCareImageNo = AlgorithmParam.m_iImageProcessDontCareImageIndex + 1;

			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_DONTCARE)
					continue;

				if (pInspectROIRgn->miTeachImageIndex == iDontCareImageNo)
				{
					if (pInspectROIRgn->miDontCareID == AlgorithmParam.m_iImageProcessDontCareROINo)
					{
						for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
						{
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
							{
								bFound = TRUE;
								break;
							}
						}

						if (bFound)
							break;
					}
				}
			}

			if (bFound)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(pInspectROIRgn->m_HDontCareResultRgn[iThreadIdx]))
				{
					if (AlgorithmParam.m_iImageProcessDontCareMargin >= 0)
					{
						DilationCircle(pInspectROIRgn->m_HDontCareResultRgn[iThreadIdx], &HMarginRgn1, (double)AlgorithmParam.m_iImageProcessDontCareMargin + 0.5);
					}
					else
					{
						iMargin = abs(AlgorithmParam.m_iImageProcessDontCareMargin);
						ErosionCircle(pInspectROIRgn->m_HDontCareResultRgn[iThreadIdx], &HMarginRgn1, (double)iMargin + 0.5);
					}

					if (AlgorithmParam.m_iImageProcessDontCareMarginInner >= 0)
					{
						DilationCircle(pInspectROIRgn->m_HDontCareResultRgn[iThreadIdx], &HMarginRgn2, (double)AlgorithmParam.m_iImageProcessDontCareMarginInner + 0.5);
					}
					else
					{
						iMargin = abs(AlgorithmParam.m_iImageProcessDontCareMarginInner);
						ErosionCircle(pInspectROIRgn->m_HDontCareResultRgn[iThreadIdx], &HMarginRgn2, (double)iMargin + 0.5);
					}

					AreaCenter(HMarginRgn1, &HlArea1, &HdCenterY, &HdCenterX);
					AreaCenter(HMarginRgn2, &HlArea2, &HdCenterY, &HdCenterX);
					lArea1 = HlArea1.L();
					lArea2 = HlArea2.L();
					dCenterY = HdCenterY.D();
					dCenterX = HdCenterX.D();

					if (lArea1 >= lArea2)
						Difference(HMarginRgn1, HMarginRgn2, &HInspectRgn);
					else
						Difference(HMarginRgn2, HMarginRgn1, &HInspectRgn);

					Difference(*pHROIHRegion, HInspectRgn, pHROIHRegion);
				}
			}
		}

		if (AlgorithmParam.m_bUseImageProcessDontCare2)
		{
			bFound = FALSE;
			iDontCareImageNo = AlgorithmParam.m_iImageProcessDontCareImageIndex2 + 1;

			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_DONTCARE)
					continue;

				if (pInspectROIRgn->miTeachImageIndex == iDontCareImageNo)
				{
					if (pInspectROIRgn->miDontCareID == AlgorithmParam.m_iImageProcessDontCareROINo2)
					{
						for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
						{
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
							{
								bFound = TRUE;
								break;
							}
						}

						if (bFound)
							break;
					}
				}
			}

			if (bFound)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(pInspectROIRgn->m_HDontCareResultRgn[iThreadIdx]))
				{
					if (AlgorithmParam.m_iImageProcessDontCareMargin2 >= 0)
					{
						DilationCircle(pInspectROIRgn->m_HDontCareResultRgn[iThreadIdx], &HMarginRgn1, (double)AlgorithmParam.m_iImageProcessDontCareMargin2 + 0.5);
					}
					else
					{
						iMargin = abs(AlgorithmParam.m_iImageProcessDontCareMargin2);
						ErosionCircle(pInspectROIRgn->m_HDontCareResultRgn[iThreadIdx], &HMarginRgn1, (double)iMargin + 0.5);
					}

					if (AlgorithmParam.m_iImageProcessDontCareMargin2Inner >= 0)
					{
						DilationCircle(pInspectROIRgn->m_HDontCareResultRgn[iThreadIdx], &HMarginRgn2, (double)AlgorithmParam.m_iImageProcessDontCareMargin2Inner + 0.5);
					}
					else
					{
						iMargin = abs(AlgorithmParam.m_iImageProcessDontCareMargin2Inner);
						ErosionCircle(pInspectROIRgn->m_HDontCareResultRgn[iThreadIdx], &HMarginRgn2, (double)iMargin + 0.5);
					}

					AreaCenter(HMarginRgn1, &HlArea1, &HdCenterY, &HdCenterX);
					AreaCenter(HMarginRgn2, &HlArea2, &HdCenterY, &HdCenterX);
					lArea1 = HlArea1.L();
					lArea2 = HlArea2.L();
					dCenterY = HdCenterY.D();
					dCenterX = HdCenterX.D();

					if (lArea1 >= lArea2)
						Difference(HMarginRgn1, HMarginRgn2, &HInspectRgn);
					else
						Difference(HMarginRgn2, HMarginRgn1, &HInspectRgn);

					Difference(*pHROIHRegion, HInspectRgn, pHROIHRegion);
				}
			}
		}

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ApplyDontCareResult] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::ApplyGenerateResult(int iMzNo, int iPcVisionNo, CAlgorithmParam AlgorithmParam, HObject *pHROIHRegion, int iThreadIdx)
{
	try
	{
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		int iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return TRUE;

		GTRegion *pInspectROIRgn;

		BOOL bFound = FALSE;
		int iGenerateImageNo = AlgorithmParam.m_iImageProcessGenerateImageIndex + 1;

		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_GENERATE)
				continue;

			if (pInspectROIRgn->miTeachImageIndex == iGenerateImageNo)
			{
				if (pInspectROIRgn->miGenerateID == AlgorithmParam.m_iImageProcessGenerateROINo)
				{
					for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
						{
							bFound = TRUE;
							break;
						}
					}

					if (bFound)
						break;
				}
			}
		}

		if (bFound)
		{
			if (AlgorithmParam.m_bUseImageProcessGenerateInspectFlag)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(pInspectROIRgn->m_HGenerateResultRgn[iThreadIdx]) == FALSE)
					GenEmptyObj(pHROIHRegion);
			}
			else
			{
				if (THEAPP.m_pGFunction->ValidHRegion(pInspectROIRgn->m_HGenerateResultRgn[iThreadIdx]) == TRUE)
				{
					if (AlgorithmParam.m_iImageProcessGenerateMargin >= 0)
						DilationCircle(pInspectROIRgn->m_HGenerateResultRgn[iThreadIdx], pHROIHRegion, (double)AlgorithmParam.m_iImageProcessGenerateMargin + 0.5);
					else if (AlgorithmParam.m_iImageProcessGenerateMargin < 0)
						ErosionCircle(pInspectROIRgn->m_HGenerateResultRgn[iThreadIdx], pHROIHRegion, (double)AlgorithmParam.m_iImageProcessGenerateMargin + 0.5);
				}
				else
					GenEmptyObj(pHROIHRegion);
			}
		}

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ApplyGenerateResult] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::ApplyMaskResult(int iMzNo, int iPcVisionNo, CAlgorithmParam AlgorithmParam, HObject *pHROIHRegion, int iThreadIdx)
{
	GenEmptyObj(pHROIHRegion);

	try
	{
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		int iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return TRUE;

		GTRegion *pInspectROIRgn;
		BOOL bFound;
		int iMaskImageNo;

		if (AlgorithmParam.m_bUseImageProcessMask)
		{
			bFound = FALSE;
			iMaskImageNo = AlgorithmParam.m_iImageProcessMaskImageIndex + 1;

			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_MASK)
					continue;

				if (pInspectROIRgn->miTeachImageIndex == iMaskImageNo)
				{
					if (pInspectROIRgn->miMaskID == AlgorithmParam.m_iImageProcessMaskROINo)
					{
						for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
						{
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
							{
								bFound = TRUE;
								break;
							}
						}

						if (bFound)
							break;
					}
				}
			}

			if (bFound)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(pInspectROIRgn->m_HMaskResultRgn[iThreadIdx]) == TRUE)
					Union2(*pHROIHRegion, pInspectROIRgn->m_HMaskResultRgn[iThreadIdx], pHROIHRegion);
			}
		}

		if (AlgorithmParam.m_bUseImageProcessMask2)
		{
			bFound = FALSE;
			iMaskImageNo = AlgorithmParam.m_iImageProcessMaskImageIndex2 + 1;

			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_MASK)
					continue;

				if (pInspectROIRgn->miTeachImageIndex == iMaskImageNo)
				{
					if (pInspectROIRgn->miMaskID == AlgorithmParam.m_iImageProcessMaskROINo2)
					{
						for (int iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
						{
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
							{
								bFound = TRUE;
								break;
							}
						}

						if (bFound)
							break;
					}
				}
			}

			if (bFound)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(pInspectROIRgn->m_HMaskResultRgn[iThreadIdx]) == TRUE)
					Union2(*pHROIHRegion, pInspectROIRgn->m_HMaskResultRgn[iThreadIdx], pHROIHRegion);
			}
		}

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ApplyMaskResult] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::LeastSquareCircleFitting(HTuple X, HTuple Y, double dNormalRadius, double *pdCenterX, double *pdCenterY, double *pdRadius)
{
	*pdCenterX = -1;
	*pdCenterY = -1;
	*pdRadius = 0;

	try
	{
		//////////////////////////////////////////////////////////////////////////
		/// Least-Square Hole Center Search
		//////////////////////////////////////////////////////////////////////////

		HTuple  x_appr, num, B, A, Result, unknowns, redundancy;
		HTuple  Iter, NumberOfMaxIterations, dx_hat, w, invBBT;
		HTuple  i, w1_a, w1_b, w2_a, w2_b, w3_a, w3_b, BBT, ATinvBBT;
		HTuple  ATinvBBTA, invATinvBBTA, neg_invATinvBBTA, neg_invATinvBBTAAT;
		HTuple  neg_invATinvBBTAAT_invBBT, M_x_appr, x_hat, x_appr_tmp;
		HTuple  Tdx_hat, Adx_hat, Adx_hatw, neg_invBBT, k_hat, v_hat;
		HTuple  Pow2, Mean, MeanValues;

		HTuple HMatchingCenterX, HMatchingCenterY;
		BOOL bLMSSuccess = FALSE;

		unknowns = 2;
		NumberOfMaxIterations = 8;
		//Approximations
		//x_appr.Reset();
		TupleGenConst(0, 0, &x_appr);
		x_appr.Append(X.TupleMean());
		x_appr.Append(Y.TupleMean());
		x_appr.Append(dNormalRadius);
		//Create matrices
		num = X.TupleLength();
		//Initialize matrices A and B
		CreateMatrix(num, num * 2, 0, &B);
		CreateMatrix(num, 2, 0, &A);
		Result = 1;
		redundancy = num - unknowns;

		Iter = 0;
		while ((Result > 1e-6) && (Iter < NumberOfMaxIterations))
		{
			Iter += 1;
			if (Iter > 1)
			{
				ClearMatrix((dx_hat.TupleConcat(w)).TupleConcat(invBBT));
			}
			//Set matrices A and B with values
			for (i = 0; i <= num - 1; i += 1)
			{
				SetValueMatrix(B, i.TupleConcat(i), (i * 2).TupleConcat((i * 2) + 1), (2 * (HTuple(X[i]) - HTuple(x_appr[0]))).TupleConcat(2 * (HTuple(Y[i]) - HTuple(x_appr[1]))));
				SetValueMatrix(A, i.TupleConcat(i), (HTuple(0).Append(1)), (-2 * (HTuple(X[i]) - HTuple(x_appr[0]))).TupleConcat(-2 * (HTuple(Y[i]) - HTuple(x_appr[1]))));
			}
			//Calculate w = (X-x_appr[0])^2 + (Y-x_appr[1])^2 - x_appr[2]^2
			CreateMatrix(num, 1, X - HTuple(x_appr[0]), &w1_a);
			MultElementMatrix(w1_a, w1_a, &w1_b);
			CreateMatrix(num, 1, Y - HTuple(x_appr[1]), &w2_a);
			MultElementMatrix(w2_a, w2_a, &w2_b);
			CreateMatrix(num, 1, HTuple(x_appr[2]), &w3_a);
			MultElementMatrix(w3_a, w3_a, &w3_b);
			AddMatrixMod(w1_b, w2_b);
			SubMatrix(w1_b, w3_b, &w);
			ClearMatrix((w1_a.TupleConcat(w2_a)).TupleConcat(w3_a));
			ClearMatrix((w1_b.TupleConcat(w2_b)).TupleConcat(w3_b));
			//Adjustment
			//Calculate dx_hat = -inv(A' * inv(B*B') * A) * A' * inv(B*B') * w
			MultMatrix(B, B, "ABT", &BBT);
			InvertMatrix(BBT, "symmetric", 0, &invBBT);
			MultMatrix(A, invBBT, "ATB", &ATinvBBT);
			MultMatrix(ATinvBBT, A, "AB", &ATinvBBTA);
			InvertMatrix(ATinvBBTA, "symmetric", 0, &invATinvBBTA);
			ScaleMatrix(invATinvBBTA, -1, &neg_invATinvBBTA);
			MultMatrix(neg_invATinvBBTA, A, "ABT", &neg_invATinvBBTAAT);
			MultMatrix(neg_invATinvBBTAAT, invBBT, "AB", &neg_invATinvBBTAAT_invBBT);
			MultMatrix(neg_invATinvBBTAAT_invBBT, w, "AB", &dx_hat);
			//Calculate x_hat = M_x_appr + dx_hat
			CreateMatrix(2, 1, HTuple(x_appr[0]).TupleConcat(HTuple(x_appr[1])), &M_x_appr);
			AddMatrix(M_x_appr, dx_hat, &x_hat);
			GetFullMatrix(x_hat, &x_appr_tmp);
			x_appr = x_appr_tmp.TupleConcat(HTuple(x_appr[2]));
			GetFullMatrix(dx_hat, &Tdx_hat);
			Result = (Tdx_hat.TupleFabs()).TupleMax();
			ClearMatrix((((BBT.TupleConcat(ATinvBBT)).TupleConcat(ATinvBBTA)).TupleConcat(invATinvBBTA)).TupleConcat(neg_invATinvBBTA));
			ClearMatrix(neg_invATinvBBTAAT.TupleConcat(neg_invATinvBBTAAT_invBBT));
			ClearMatrix(M_x_appr.TupleConcat(x_hat));
		}

		if (Iter < NumberOfMaxIterations)
		{
			//Adjustment of a hole is successful.  The hole is displayed.
			//Calculate k_hat = -inv(B*B') -(A * dx_hat + w)
			MultMatrix(A, dx_hat, "AB", &Adx_hat);
			AddMatrix(Adx_hat, w, &Adx_hatw);
			ScaleMatrix(invBBT, -1, &neg_invBBT);
			MultMatrix(neg_invBBT, Adx_hatw, "AB", &k_hat);
			//Calculate v_hat = B * k_hat
			MultMatrix(B, k_hat, "ATB", &v_hat);
			PowScalarElementMatrix(v_hat, 2, &Pow2);
			MeanMatrix(Pow2, "full", &Mean);
			SqrtMatrixMod(Mean);
			GetFullMatrix(Mean, &MeanValues);
			ClearMatrix((((((Adx_hat.TupleConcat(Adx_hatw)).TupleConcat(neg_invBBT)).TupleConcat(k_hat)).TupleConcat(v_hat)).TupleConcat(Pow2)).TupleConcat(Mean));
			*pdCenterX = x_appr[0].D();
			*pdCenterY = x_appr[1].D();
			*pdRadius = x_appr[2].D();

			bLMSSuccess = TRUE;
		}
		else
		{
			bLMSSuccess = FALSE;
		}

		ClearMatrix((dx_hat.TupleConcat(w)).TupleConcat(invBBT));
		ClearMatrix(A.TupleConcat(B));

		return bLMSSuccess;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::LeastSquareCircleFitting] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::SurfacePlane_Point(DPOINT dR0, DPOINT dR1, DPOINT dR2, double dH0, double dH1, double dH2, DPOINT dMeasurePoint, double *dGrayHeight)
{
	double x1 = dR0.x;
	double y1 = dR0.y;
	double z1 = dH0;

	double x2 = dR1.x;
	double y2 = dR1.y;
	double z2 = dH1;

	double x3 = dR2.x;
	double y3 = dR2.y;
	double z3 = dH2;

	double a = (y2 - y1)*(z3 - z1) - (y3 - y1)*(z2 - z1);
	double b = (x3 - x2)*(z2 - z1) - (x2 - x1)*(z3 - z2);
	double c = (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1);
	double d = a * x1 + b * y1 + c * z1;

	if (c != 0)
	{
		*dGrayHeight = (d - a * dMeasurePoint.x - b * dMeasurePoint.y) / c;		// Gray Height in the Reference Plane
		return TRUE;
	}
	else
		return FALSE;

}

BOOL Algorithm::ApproxPlane_NPoint(int iNoRPoint, DPOINT *pdRPoint, double *pdRHeight, double *pdA, double *pdB, double *pdC)
{
	try
	{
		int i;
		HTuple HMatValue, HVectorValue, HPlaneResultValue;

		TupleGenConst(HTuple(iNoRPoint * 3), HTuple(1.0), &HMatValue);
		for (i = 0; i < iNoRPoint; i++)
		{
			HMatValue[i * 3] = pdRPoint[i].x;
			HMatValue[i * 3 + 1] = pdRPoint[i].y;
			HMatValue[i * 3 + 2] = 1.0;
		}

		TupleGenConst(HTuple(iNoRPoint), HTuple(1.0), &HVectorValue);
		for (i = 0; i < iNoRPoint; i++)
		{
			HVectorValue[i] = pdRHeight[i];
		}

		HTuple HMatrixID, HVectorID, HInvertMatrixID, HMatrixMultiID;

		CreateMatrix(iNoRPoint, 3, HMatValue, &HMatrixID);
		CreateMatrix(iNoRPoint, 1, HVectorValue, &HVectorID);

		InvertMatrix(HMatrixID, "general", 2.2204e-16, &HInvertMatrixID);
		MultMatrix(HInvertMatrixID, HVectorID, "AB", &HMatrixMultiID);

		GetFullMatrix(HMatrixMultiID, &HPlaneResultValue);

		*pdA = HPlaneResultValue[0].D();
		*pdB = HPlaneResultValue[1].D();
		*pdC = HPlaneResultValue[2].D();

		ClearMatrix(HMatrixID);
		ClearMatrix(HVectorID);
		ClearMatrix(HInvertMatrixID);
		ClearMatrix(HMatrixMultiID);

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ApproxPlane_NPoint] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::SurfacePlane_NPoint(double dA, double dB, double dC, DPOINT dMeasurePoint, double *dGrayHeight)
{
	try
	{
		*dGrayHeight = dA * dMeasurePoint.x + dB * dMeasurePoint.y + dC;

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::SurfacePlane_NPoint] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::ConvertProfileToByteImage(HObject HProfileImage, HObject *pHByteImage)
{
	GenImageConst(pHByteImage, "byte", IMAGE_WIDTH_LASER, IMAGE_HEIGHT_LASER);

	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(HProfileImage) == FALSE)
			return FALSE;

		HObject HInvalidLaserRgn, HDomainRgn, HValidLaserRgn, HImageReduced;

		Threshold(HProfileImage, &HInvalidLaserRgn, INVALID_LASER_DATA, INVALID_LASER_DATA);
		GetDomain(HProfileImage, &HDomainRgn);
		Difference(HDomainRgn, HInvalidLaserRgn, &HValidLaserRgn);

		HTuple HMin, HMax, HRange;
		MinMaxGray(HValidLaserRgn, HProfileImage, 0, &HMin, &HMax, &HRange);

		if (HMax == HMin)
			return FALSE;

		THEAPP.m_pGFunction->ScaleImageRange(HProfileImage, pHByteImage, HMin, HMax);
		ConvertImageType(*pHByteImage, pHByteImage, "byte");

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::ConvertProfileToByteImage] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::SelectShapeUsingMinLength(HObject HInputRgn, int iMinLengthPxl, int iMaxLengthPxl, int iRefDir, BOOL bIgnoreHole, HObject *pHSelectedShapeRgn, int *piMinMeasureLength)
{
	try
	{
		HObject HInputMeasureRgn;
		CopyObj(HInputRgn, &HInputMeasureRgn, 1, -1);

		GenEmptyObj(pHSelectedShapeRgn);
		*piMinMeasureLength = 1000000;

		HTuple HNoBlob;
		long lNoBlob;

		CountObj(HInputMeasureRgn, &HNoBlob);
		lNoBlob = HNoBlob.L();

		if (lNoBlob <= 0)
			return TRUE;

		int i, j;
		HObject HSelectedRgn, HMeasureRectRgn, HOnePixelRgn, HOnePixelRgn_NonBlob;

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		Hlong lBlobWidth, lBlobHeight;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		Hlong lMinBlobLength, lOnePixelLength;

		GenEmptyObj(pHSelectedShapeRgn);

		for (i = 0; i < lNoBlob; i++)
		{
			SelectObj(HInputMeasureRgn, &HSelectedRgn, i + 1);

			SmallestRectangle1(HSelectedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
			lLTPointY = HlLTPointY.L();
			lLTPointX = HlLTPointX.L();
			lRBPointY = HlRBPointY.L();
			lRBPointX = HlRBPointX.L();

			lBlobWidth = lRBPointX - lLTPointX + 1;
			lBlobHeight = lRBPointY - lLTPointY + 1;

			lMinBlobLength = 10000;

			if (iRefDir == LENGTH_MIN_TOP)
			{
				for (j = 0; j < lBlobWidth; j++)
				{
					GenRectangle1(&HMeasureRectRgn, lLTPointY, lLTPointX + j, lRBPointY, lLTPointX + j);
					Intersection(HMeasureRectRgn, HSelectedRgn, &HOnePixelRgn);
					Difference(HMeasureRectRgn, HOnePixelRgn, &HOnePixelRgn_NonBlob);

					if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn))
					{
						if (bIgnoreHole)
						{
							SmallestRectangle1(HOnePixelRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
							lOnePixelLength = HlRBPointY.L() - lLTPointY + 1;
						}
						else
						{
							if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn_NonBlob))
							{
								SmallestRectangle1(HOnePixelRgn_NonBlob, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
								lOnePixelLength = HlLTPointY.L() - lLTPointY;
							}
							else
							{
								lOnePixelLength = lBlobHeight;
							}
						}

						if (lOnePixelLength < lMinBlobLength)
						{
							lMinBlobLength = lOnePixelLength;
						}
					}
				}
			}
			else if (iRefDir == LENGTH_MIN_BOTTOM)
			{
				for (j = 0; j < lBlobWidth; j++)
				{
					GenRectangle1(&HMeasureRectRgn, lLTPointY, lLTPointX + j, lRBPointY, lLTPointX + j);
					Intersection(HMeasureRectRgn, HSelectedRgn, &HOnePixelRgn);
					Difference(HMeasureRectRgn, HOnePixelRgn, &HOnePixelRgn_NonBlob);

					if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn))
					{
						if (bIgnoreHole)
						{
							SmallestRectangle1(HOnePixelRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
							lOnePixelLength = lRBPointY - HlLTPointY.L() + 1;
						}
						else
						{
							if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn_NonBlob))
							{
								SmallestRectangle1(HOnePixelRgn_NonBlob, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
								lOnePixelLength = lRBPointY - HlRBPointY.L();
							}
							else
							{
								lOnePixelLength = lBlobHeight;
							}
						}

						if (lOnePixelLength < lMinBlobLength)
						{
							lMinBlobLength = lOnePixelLength;
						}
					}
				}
			}
			else if (iRefDir == LENGTH_MIN_LEFT)
			{
				for (j = 0; j < lBlobHeight; j++)
				{
					GenRectangle1(&HMeasureRectRgn, lLTPointY + j, lLTPointX, lLTPointY + j, lRBPointX);
					Intersection(HMeasureRectRgn, HSelectedRgn, &HOnePixelRgn);
					Difference(HMeasureRectRgn, HOnePixelRgn, &HOnePixelRgn_NonBlob);

					if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn))
					{
						if (bIgnoreHole)
						{
							SmallestRectangle1(HOnePixelRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
							lOnePixelLength = HlRBPointX.L() - lLTPointX + 1;
						}
						else
						{
							if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn_NonBlob))
							{
								SmallestRectangle1(HOnePixelRgn_NonBlob, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
								lOnePixelLength = HlLTPointX.L() - lLTPointX;
							}
							else
							{
								lOnePixelLength = lBlobWidth;
							}
						}

						if (lOnePixelLength < lMinBlobLength)
						{
							lMinBlobLength = lOnePixelLength;
						}
					}
				}
			}
			else if (iRefDir == LENGTH_MIN_RIGHT)
			{
				for (j = 0; j < lBlobHeight; j++)
				{
					GenRectangle1(&HMeasureRectRgn, lLTPointY + j, lLTPointX, lLTPointY + j, lRBPointX);
					Intersection(HMeasureRectRgn, HSelectedRgn, &HOnePixelRgn);
					Difference(HMeasureRectRgn, HOnePixelRgn, &HOnePixelRgn_NonBlob);

					if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn))
					{
						if (bIgnoreHole)
						{
							SmallestRectangle1(HOnePixelRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
							lOnePixelLength = lRBPointX - HlLTPointX.L() + 1;
						}
						else
						{
							if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn_NonBlob))
							{
								SmallestRectangle1(HOnePixelRgn_NonBlob, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
								lOnePixelLength = lRBPointX - HlRBPointX.L();
							}
							else
							{
								lOnePixelLength = lBlobWidth;
							}
						}

						if (lOnePixelLength < lMinBlobLength)
						{
							lMinBlobLength = lOnePixelLength;
						}
					}
				}
			}

			if (lMinBlobLength >= iMinLengthPxl && lMinBlobLength <= iMaxLengthPxl)
			{
				ConcatObj(*pHSelectedShapeRgn, HSelectedRgn, pHSelectedShapeRgn);
			}

			if (lMinBlobLength < *piMinMeasureLength)
			{
				*piMinMeasureLength = lMinBlobLength;
			}
		}

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::SelectShapeUsingMinLength] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::SelectShapeUsingMaxLength(HObject HInputRgn, int iMinLengthPxl, int iMaxLengthPxl, int iRefDir, BOOL bIgnoreHole, HObject *pHSelectedShapeRgn, int *piMaxMeasureLength)
{
	try
	{
		HObject HInputMeasureRgn;
		CopyObj(HInputRgn, &HInputMeasureRgn, 1, -1);

		GenEmptyObj(pHSelectedShapeRgn);
		*piMaxMeasureLength = 0;

		HTuple HNoBlob;
		long lNoBlob;

		CountObj(HInputMeasureRgn, &HNoBlob);
		lNoBlob = HNoBlob.L();

		if (lNoBlob <= 0)
			return TRUE;

		int i, j;
		HObject HSelectedRgn, HMeasureRectRgn, HOnePixelRgn, HOnePixelRgn_NonBlob;

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		Hlong lBlobWidth, lBlobHeight;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		Hlong lMaxBlobLength, lOnePixelLength;

		GenEmptyObj(pHSelectedShapeRgn);

		for (i = 0; i < lNoBlob; i++)
		{
			SelectObj(HInputMeasureRgn, &HSelectedRgn, i + 1);

			SmallestRectangle1(HSelectedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
			lLTPointY = HlLTPointY.L();
			lLTPointX = HlLTPointX.L();
			lRBPointY = HlRBPointY.L();
			lRBPointX = HlRBPointX.L();

			lBlobWidth = lRBPointX - lLTPointX + 1;
			lBlobHeight = lRBPointY - lLTPointY + 1;

			lMaxBlobLength = 0;

			if (iRefDir == LENGTH_MIN_TOP || iRefDir == LENGTH_MIN_BOTTOM)
			{
				for (j = 0; j < lBlobWidth; j++)
				{
					GenRectangle1(&HMeasureRectRgn, lLTPointY, lLTPointX + j, lRBPointY, lLTPointX + j);
					Intersection(HMeasureRectRgn, HSelectedRgn, &HOnePixelRgn);
					Difference(HMeasureRectRgn, HOnePixelRgn, &HOnePixelRgn_NonBlob);

					if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn))
					{
						if (bIgnoreHole)
						{
							SmallestRectangle1(HOnePixelRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
							lOnePixelLength = HlRBPointY.L() - lLTPointY + 1;
						}
						else
						{
							if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn_NonBlob))
							{
								SmallestRectangle1(HOnePixelRgn_NonBlob, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
								lOnePixelLength = HlLTPointY.L() - lLTPointY;
							}
							else
							{
								lOnePixelLength = lBlobHeight;
							}
						}

						if (lOnePixelLength > lMaxBlobLength)
						{
							lMaxBlobLength = lOnePixelLength;
						}
					}
				}
			}
			else if (iRefDir == LENGTH_MIN_LEFT || iRefDir == LENGTH_MIN_RIGHT)
			{
				for (j = 0; j < lBlobHeight; j++)
				{
					GenRectangle1(&HMeasureRectRgn, lLTPointY + j, lLTPointX, lLTPointY + j, lRBPointX);
					Intersection(HMeasureRectRgn, HSelectedRgn, &HOnePixelRgn);
					Difference(HMeasureRectRgn, HOnePixelRgn, &HOnePixelRgn_NonBlob);

					if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn))
					{
						if (bIgnoreHole)
						{
							SmallestRectangle1(HOnePixelRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
							lOnePixelLength = HlRBPointX.L() - lLTPointX + 1;
						}
						else
						{
							if (THEAPP.m_pGFunction->ValidHRegion(HOnePixelRgn_NonBlob))
							{
								SmallestRectangle1(HOnePixelRgn_NonBlob, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
								lOnePixelLength = HlLTPointX.L() - lLTPointX;
							}
							else
							{
								lOnePixelLength = lBlobWidth;
							}
						}

						if (lOnePixelLength > lMaxBlobLength)
						{
							lMaxBlobLength = lOnePixelLength;
						}
					}
				}
			}

			if (lMaxBlobLength >= iMinLengthPxl && lMaxBlobLength <= iMaxLengthPxl)
			{
				ConcatObj(*pHSelectedShapeRgn, HSelectedRgn, pHSelectedShapeRgn);
			}

			if (lMaxBlobLength > *piMaxMeasureLength) //jwj modify (org : if (lMaxBlobLength < *piMaxMeasureLength) )
			{
				*piMaxMeasureLength = lMaxBlobLength;
			}
		}

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::SelectShapeUsingMaxLength] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL Algorithm::SelectShapeUsingOrthoLength(HObject HInputRgn, HObject HInspectRgn, int iMinLengthPxl, int iMaxLengthPxl, int iRefDir, HObject *pHSelectedShapeRgn, double *pdMaxMeasureLength)
{
	try
	{
		HObject HInputMeasureRgn;
		CopyObj(HInputRgn, &HInputMeasureRgn, 1, -1);

		GenEmptyObj(pHSelectedShapeRgn);
		*pdMaxMeasureLength = 0;

		HTuple HNoBlob;
		long lNoBlob;

		CountObj(HInputMeasureRgn, &HNoBlob);
		lNoBlob = HNoBlob.L();

		if (lNoBlob <= 0)
			return TRUE;

		int i, j;
		HObject HSelectedRgn;

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		Hlong lBlobWidth, lBlobHeight;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		double dMaxBlobLength;

		// Find Ref Line
		HTuple HPolygonRows, HPolygonCols, HLength;
		GetRegionPolygon(HInspectRgn, 5, &HPolygonRows, &HPolygonCols);

		TupleLength(HPolygonRows, &HLength);

		if (HLength < 5)
		{
			return FALSE;
		}

		Hlong lNoPolygon;
		lNoPolygon = HLength[0].L();

		TupleRemove(HPolygonRows, HLength - 1, &HPolygonRows);
		TupleRemove(HPolygonCols, HLength - 1, &HPolygonCols);

		HTuple HSelectedPointRow1, HSelectedPointCol1, HSelectedPointRow2, HSelectedPointCol2;

		SmallestRectangle1(HInspectRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

		if (iRefDir == ORTHO_LENGTH_TOP)
		{
			HSelectedPointRow1 = HlLTPointY;
			HSelectedPointCol1 = HlLTPointX;
			HSelectedPointRow2 = HlLTPointY;
			HSelectedPointCol2 = HlRBPointX;
		}
		else if (iRefDir == ORTHO_LENGTH_BOTTOM)
		{
			HSelectedPointRow1 = HlRBPointY;
			HSelectedPointCol1 = HlLTPointX;
			HSelectedPointRow2 = HlRBPointY;
			HSelectedPointCol2 = HlRBPointX;
		}
		else if (iRefDir == ORTHO_LENGTH_LEFT)
		{
			HSelectedPointRow1 = HlLTPointY;
			HSelectedPointCol1 = HlLTPointX;
			HSelectedPointRow2 = HlRBPointY;
			HSelectedPointCol2 = HlLTPointX;
		}
		else if (iRefDir == ORTHO_LENGTH_RIGHT)
		{
			HSelectedPointRow1 = HlLTPointY;
			HSelectedPointCol1 = HlRBPointX;
			HSelectedPointRow2 = HlRBPointY;
			HSelectedPointCol2 = HlRBPointX;
		}

		HTuple HRefLinePointX[2], HRefLinePointY[2];

		HTuple HDist;
		HTuple HMinDist = 1000000000;

		for (i = 0; i < (lNoPolygon - 1); i++)
		{
			DistancePp(HSelectedPointRow1, HSelectedPointCol1, HPolygonRows[i], HPolygonCols[i], &HDist);
			if (HDist < HMinDist)
			{
				HRefLinePointX[0] = HPolygonCols[i];
				HRefLinePointY[0] = HPolygonRows[i];

				HMinDist = HDist;
			}
		}

		HMinDist = 1000000000;

		for (i = 0; i < (lNoPolygon - 1); i++)
		{
			DistancePp(HSelectedPointRow2, HSelectedPointCol2, HPolygonRows[i], HPolygonCols[i], &HDist);
			if (HDist < HMinDist)
			{
				HRefLinePointX[1] = HPolygonCols[i];
				HRefLinePointY[1] = HPolygonRows[i];

				HMinDist = HDist;
			}
		}

		//
		HTuple HDistanceMin, HDistanceMax;

		GenEmptyObj(pHSelectedShapeRgn);

		for (i = 0; i < lNoBlob; i++)
		{
			SelectObj(HInputMeasureRgn, &HSelectedRgn, i + 1);

			DistanceLr(HSelectedRgn, HRefLinePointY[0], HRefLinePointX[0], HRefLinePointY[1], HRefLinePointX[1], &HDistanceMin, &HDistanceMax);

			dMaxBlobLength = HDistanceMax[0].D();

			if (dMaxBlobLength >= iMinLengthPxl && dMaxBlobLength <= iMaxLengthPxl)
			{
				ConcatObj(*pHSelectedShapeRgn, HSelectedRgn, pHSelectedShapeRgn);
			}

			if (dMaxBlobLength > *pdMaxMeasureLength)
			{
				*pdMaxMeasureLength = dMaxBlobLength;
			}
		}

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::SelectShapeUsingOrthoLength] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

void Algorithm::SaveADJImage(int nThreadNO,
	int iInspectionBufferIdx,
	int iVisionCamType,
	HObject HVisionAllDefectRgn,
	int iPcVisionNo,
	int iNoInspectImage,
	int iMzNo,
	int iTrayNo,
	int iModuleNo,
	CString sBarcodeID,
	DWORD iModuleTotalStartTime,
	HObject &HADJFilteredDefectRgn,
	int &iInspectionTypeResult)
{
	try
	{
		CString sVisionCamType_Short;
		sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

		BOOL bSpecialDefect = FALSE;
		CString sSpecialDefectTransCode = _T("N");
		int iDefectTransCode;
		CInspectService* pInspectService = THEAPP.m_pInspectService;

		int iCurDeepLearningModel = 0;
		int iLastDeepLearningModel = 0;
		BOOL bIsTimeOut = FALSE;

		int iROIIndex, iTabIdx;
		int iNoInspectROI = 0;
		int iImageIndex = -1;
		BOOL bAdjSave = FALSE;
		GTRegion *pInspectROIRgn;
		HObject HROIRgn, HADJDefectRgn;
		int iImageIndexToSave;
		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		HObject HVisionAllDefectConnRgn, HADJSaveDefectRgn, HADJSaveImage, HADJSaveRegion;
		GenEmptyObj(&HADJSaveRegion);
		HObject HVisionAllDefectRgn_EachImage[MAX_IMAGE_TAB];
		HObject HDefectRgn_EachImage[MAX_IMAGE_TAB];
		Hlong lBlobArea;
		double dBlobCenterX, dBlobCenterY;
		HTuple HlROIArea, HlBlobArea, HdBlobCenterX, HdBlobCenterY;
		int iCamImageWidth, iCamImageHeight;
		CString Filename, sADJDefectCode, sADJInspectionTypeCode, sADJDefectDetail;
		BOOL bTempSpecialDefect;
		CString sTempSpecialDefectTransCode, sADJTimeStamp;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		SYSTEMTIME time;

		iCamImageWidth = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iPcVisionNo];
		iCamImageHeight = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iPcVisionNo];

		Connection(HVisionAllDefectRgn, &HVisionAllDefectConnRgn);

		iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();

		//////////////////////////////////////////////////////////////////////////
		/// Use or not

		HObject HSubVisionAllDefectRgn;
		int iImageIdx;
		BOOL bApplyADJ = FALSE;

		HObject HIntersectRgn;
		Hlong lMaxBlobArea;
		HObject HMaxAreaBlobRgn, HEachImageDefectRgn, HRepBlobRgn;
		int iMaxDefectInspectonTypeIndex, iMaxDefectDefectNameIndex;

		HTuple HNoDefect;
		Hlong lNoDefect;
		Hlong lNoDefectCount;

		CString sDefectName, sDefectDetail;
		CString sInspectionType;
		int iType;
		CString sDefectResult, sDefectCodeResult;
		HObject HImgDump_save;
		HTuple HDefectName;
		HObject HConnRgn, HRosDisplayRgn, HCircleTransRgn, HSelectedRgn;
		HTuple HdCircleRow, HdCircleCol, HdCircleRadius;
		CPoint cpDefect[1000];
		HTuple HlArea, HdCenterX, HdCenterY;
		int iSelectedImage = -1;

		///// Remove overlapped defects

		HObject HTempRgn1, HTempRgn2;
		HObject HTotalDefectRgn;
		Hlong lMaxArea, lMaxAreaImageIndex;

		GenEmptyObj(&HTotalDefectRgn);

		//for (int i = 0; i < iNoInspectImage; i++)
		//{
		//	if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][i]) == FALSE)
		//		continue;

		//	ConcatObj(HTotalDefectRgn, pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][i], &HTotalDefectRgn);
		//}

		for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
		{
			for (int j = 0; j < MAX_DEFECT_NAME; j++)
			{
				if (nThreadNO == 0)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[i][j]) == TRUE)
					{
						ConcatObj(HTotalDefectRgn, THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[i][j], &HTotalDefectRgn);
					}
				}
				else if (nThreadNO == 1)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[i][j]) == TRUE)
					{
						ConcatObj(HTotalDefectRgn, THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[i][j], &HTotalDefectRgn);
					}
				}
				else if (nThreadNO == 2)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg3->m_HRosReviewSpecApplyDefectRgn[i][j]) == TRUE)
					{
						ConcatObj(HTotalDefectRgn, THEAPP.m_pInspectAdminViewHideDlg3->m_HRosReviewSpecApplyDefectRgn[i][j], &HTotalDefectRgn);
					}
				}
				else if (nThreadNO == 3)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg4->m_HRosReviewSpecApplyDefectRgn[i][j]) == TRUE)
					{
						ConcatObj(HTotalDefectRgn, THEAPP.m_pInspectAdminViewHideDlg4->m_HRosReviewSpecApplyDefectRgn[i][j], &HTotalDefectRgn);
					}
				}
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HTotalDefectRgn) == TRUE)
		{
			Union1(HTotalDefectRgn, &HTotalDefectRgn);
			Connection(HTotalDefectRgn, &HTotalDefectRgn);

			CountObj(HTotalDefectRgn, &HNoDefect);
			lNoDefectCount = HNoDefect[0].I();
			////////// 불량 ROI 순차적
			for (int DefectIndex = 1; DefectIndex <= lNoDefectCount; DefectIndex++)
			{
				lMaxArea = 0;
				bApplyADJ = FALSE;
				SelectObj(HTotalDefectRgn, &HSelectedRgn, DefectIndex);

				AreaCenter(HSelectedRgn, &HlArea, &HdCenterY, &HdCenterX);

				///////// ROI 정보
				for (int j = 0; j < iNoInspectImage; j++)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][j]) == FALSE)
						continue;

					Connection(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][j], &HTempRgn1);

					SelectShapeProto(HTempRgn1, HSelectedRgn, &HTempRgn2, "overlaps_abs", 1, MAX_DEF);

					if (THEAPP.m_pGFunction->ValidHRegion(HTempRgn2) == FALSE)
						continue;

					Union1(HTempRgn2, &HTempRgn2);

					AreaCenter(HTempRgn2, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
					lBlobArea = HlBlobArea.L();

					if (lBlobArea > lMaxArea)
					{
						lMaxArea = lBlobArea;
						lMaxAreaImageIndex = j;
					}
				}
				///////// ROI 이미지 탭 정보

				lMaxBlobArea = 0;
				GenEmptyObj(&HMaxAreaBlobRgn);
				GenEmptyObj(&HEachImageDefectRgn);

				iMaxDefectInspectonTypeIndex = -1;
				iMaxDefectDefectNameIndex = -1;

				///////// 불량 정보
				for (int x = 0; x < MAX_INSPECTION_TYPE; x++)
				{
					for (int y = 0; y < MAX_DEFECT_NAME; y++)
					{
						switch (nThreadNO)
						{
						case 0:
							if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
								continue;
							Intersection(HSelectedRgn, THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
							if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
								continue;
							Connection(THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[x][y], &HConnRgn);
							break;
						case 1:
							if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
								continue;
							Intersection(HSelectedRgn, THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
							if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
								continue;
							Connection(THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[x][y], &HConnRgn);
							break;
						case 2:
							if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg3->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
								continue;
							Intersection(HSelectedRgn, THEAPP.m_pInspectAdminViewHideDlg3->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
							if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
								continue;
							Connection(THEAPP.m_pInspectAdminViewHideDlg3->m_HRosReviewSpecApplyDefectRgn[x][y], &HConnRgn);
							break;
						case 3:
							if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg4->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
								continue;
							Intersection(HSelectedRgn, THEAPP.m_pInspectAdminViewHideDlg4->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
							if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
								continue;
							Connection(THEAPP.m_pInspectAdminViewHideDlg4->m_HRosReviewSpecApplyDefectRgn[x][y], &HConnRgn);
							break;
						default:
							break;
						}


						SelectShapeProto(HConnRgn, HIntersectRgn, &HRosDisplayRgn, "overlaps_abs", 1, MAX_DEF);

						ConcatObj(HEachImageDefectRgn, HRosDisplayRgn, &HEachImageDefectRgn);	// Display
						Union1(HRosDisplayRgn, &HRosDisplayRgn);

						AreaCenter(HRosDisplayRgn, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
						lBlobArea = HlBlobArea.L();

						if (lBlobArea > lMaxBlobArea)
						{
							HMaxAreaBlobRgn = HRosDisplayRgn;		// Display
							lMaxBlobArea = lBlobArea;

							iMaxDefectInspectonTypeIndex = x;
							iMaxDefectDefectNameIndex = y;
						}
					}
				} // 불량 정보

				CString strTempTypeCode = _T("");
				CString strTempDefectDetail = _T("");
				strTempTypeCode = g_strInspectionTypeName_Short[iMaxDefectInspectonTypeIndex].c_str();
				strTempDefectDetail = THEAPP.GetDefectCode(iMaxDefectDefectNameIndex, &iDefectTransCode);

				CString strTempADJDefectName;

				CString strTempVisionName_Short = sVisionCamType_Short;

				if (strTempVisionName_Short == "B3")
					strTempVisionName_Short = "B2";
				if (strTempVisionName_Short == "T3")
					strTempVisionName_Short = "T2";

				strTempADJDefectName.Format("%s_%s_%s", strTempTypeCode, strTempVisionName_Short, strTempDefectDetail);

				for (int nTempADJIdx = 0; nTempADJIdx < MAX_DEFECT_NAME; nTempADJIdx++)
				{
					if (THEAPP.m_strADJSaveDefectList[nTempADJIdx] == "ALL") //20200512
					{
						if (THEAPP.m_pGFunction->ValidHRegion(HMaxAreaBlobRgn) == TRUE)
						{
							sADJInspectionTypeCode = strTempTypeCode;
							sADJDefectDetail = strTempDefectDetail;
							bApplyADJ = TRUE;
							break;
						}
					}

					if (strTempADJDefectName == THEAPP.m_strADJSaveDefectList[nTempADJIdx])
					{
						if (THEAPP.m_pGFunction->ValidHRegion(HMaxAreaBlobRgn) == TRUE)
						{
							sADJInspectionTypeCode = strTempTypeCode;
							sADJDefectDetail = strTempDefectDetail;
							bApplyADJ = TRUE;
							break;
						}
					}
				}

				CString strCameraIndex;

				if (bApplyADJ == TRUE)
				{
					HADJSaveImage = HMaxAreaBlobRgn;
					HADJSaveRegion = HMaxAreaBlobRgn;

					bAdjSave = FALSE;
					for (int i = 0; i < THEAPP.m_iAdjSelectImageCount; i++)
					{
						if (bAdjSave == TRUE) break;
						for (int j = 0; j < MAX_INSPECTION_TYPE; j++)
						{
							strCameraIndex = sVisionCamType_Short;
							if (sADJInspectionTypeCode == THEAPP.m_Struct_AdjImageInfo[i].m_sInspectionType && strTempDefectDetail == THEAPP.m_Struct_AdjImageInfo[i].m_sDefectName && THEAPP.m_Struct_AdjImageInfo[i].m_sVisionCam == strCameraIndex && lMaxAreaImageIndex == THEAPP.m_Struct_AdjImageInfo[i].m_iImageCount)
							{
								iImageIndex = THEAPP.m_Struct_AdjImageInfo[i].m_iImageIndex[0];
								bAdjSave = TRUE;
								break;
							}
							else
							{
								iImageIndex = lMaxAreaImageIndex;
							}
						}
					}

					if (iImageIndex == -1)iImageIndexToSave = lMaxAreaImageIndex;
					else iImageIndexToSave = iImageIndex;

					lLTPointY = (int)HdBlobCenterY.D() - THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2;
					lLTPointX = (int)HdBlobCenterX.D() - THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2;
					lRBPointY = (int)HdBlobCenterY.D() + THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2 - 1;
					lRBPointX = (int)HdBlobCenterX.D() + THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2 - 1;

					if (lLTPointX < 0)
					{
						lLTPointX = 0;
						lRBPointX = THEAPP.Struct_PreferenceStruct.m_iADJImageSize - 1;
					}

					if (lRBPointX >= (iCamImageWidth - 1))
					{
						lRBPointX = iCamImageWidth - 1;
						lLTPointX = iCamImageWidth - THEAPP.Struct_PreferenceStruct.m_iADJImageSize;
					}

					if (lLTPointY < 0)
					{
						lLTPointY = 0;
						lRBPointY = THEAPP.Struct_PreferenceStruct.m_iADJImageSize - 1;
					}

					if (lRBPointY >= (iCamImageHeight - 1))
					{
						lRBPointY = iCamImageHeight - 1;
						lLTPointY = iCamImageHeight - 1 - THEAPP.Struct_PreferenceStruct.m_iADJImageSize + 1;
					}

					CropRectangle1(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HInspectCImage[iInspectionBufferIdx][CHANNEL_TYPE_COLOR][iImageIndexToSave], &HADJSaveImage, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

					GetLocalTime(&time);
					sADJTimeStamp.Format("%04d%02d%02d_%02d%02d%02d%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

					Filename.Format("T%02d_M%02d_%s_%s_%s_%s_Img%02d_%s", iTrayNo, iModuleNo, sVisionCamType_Short, strTempTypeCode, strTempDefectDetail, sADJTimeStamp, iImageIndexToSave + 1, sBarcodeID);

					try
					{
						if (THEAPP.Struct_PreferenceStruct.m_bSaveADJ)
						{
							CString strSaveFileName;
							if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread)
							{
								if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal)
								{
									auto log_time_start = std::chrono::high_resolution_clock::now();
									WriteImage(HADJSaveImage, "jpeg 100", 0, HTuple(THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename));
									auto log_time_end = std::chrono::high_resolution_clock::now();
									auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

									strSaveFileName = THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename;
									strLog.Format("Save ADJ image, Time(ms): %d, File name: %s", log_time_ms, strSaveFileName);
									THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
								}

								if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS)
								{
									auto log_time_start = std::chrono::high_resolution_clock::now();
									WriteImage(HADJSaveImage, "jpeg 100", 0, HTuple(THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename));
									auto log_time_end = std::chrono::high_resolution_clock::now();
									auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

									strSaveFileName = THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename;
									strLog.Format("Save ADJ image, Time(ms): %d, File name: %s", log_time_ms, strSaveFileName);
									THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
								}
							}
							else
							{
								int nCurQueueIndex;
								if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
									nCurQueueIndex = (iTrayNo - 1) % THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;
								else
									nCurQueueIndex = 0;

								if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal)
								{
									ADJ_IMAGE_SAVE_PARAM* pSaveThreadIDParam = new ADJ_IMAGE_SAVE_PARAM;
									CopyImage(HADJSaveImage, &(pSaveThreadIDParam->HSaveImage));
									pSaveThreadIDParam->sSavePath = THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename;
									THEAPP.m_pInspectService->AddListSaveADJImageParam(pSaveThreadIDParam, iPcVisionNo, iMzNo, iTrayNo, iModuleNo);
								}

								if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS)
								{
									ADJ_IMAGE_SAVE_PARAM* pSaveThreadIDParam_Dual = new ADJ_IMAGE_SAVE_PARAM;
									CopyImage(HADJSaveImage, &(pSaveThreadIDParam_Dual->HSaveImage));
									pSaveThreadIDParam_Dual->sSavePath = THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename;
									THEAPP.m_pInspectService->AddListSaveADJImageParam(pSaveThreadIDParam_Dual, iPcVisionNo, iMzNo, iTrayNo, iModuleNo);
								}
							}
						}
					}
					catch (HException &except)
					{
						HTuple HExp;
						HTuple HOperatorName, HErrMsg;
						CString sOperatorName, sErrMsg;
						except.ToHTuple(&HExp);
						except.GetExceptionData(HExp, "operator", &HOperatorName);
						except.GetExceptionData(HExp, "error_message", &HErrMsg);
						sOperatorName = HOperatorName.S();
						sErrMsg = HErrMsg.S();

						strLog.Format("Halcon Exception [Algorithm::SaveADJImage::WriteImage(N%d)] : <%s>%s", nThreadNO + 1, sOperatorName, sErrMsg);
						THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
					}
				}
				if (bIsTimeOut)	//ADJ 모듈 time out
					break;
			} // 불량 Index

		}  //if (THEAPP.m_pGFunction->ValidHRegion(HTotalDefectRgn)==TRUE)


	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [Algorithm::SaveADJImage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}

void Algorithm::SrunmedSLI(float* y, float* smo, long n, int bw_tot, int end_rule, int skip)
{
	/*
		 *  Computes "Running Median" smoother ("Stuetzle" algorithm) with medians of 'band'

		 *  Input:
		 *	y(n)	- responses in order of increasing predictor values
		 *	n	- number of observations
		 *	bw_tot	- span of running medians (MUST be ODD !!)
		 *	end_rule -- 0: Keep original data at ends {j; j < b2 | j > n-b2}
		 *		     -- 1: Constant ends = median(y[1],..,y[bw]) "robust"
		 *  Output:
		 *	smo(n)	- smoothed data output (should be allocated)

		 * NOTE:  The 'end' values are just copied !! this is fast but not too nice !
		 */

		 /* Local variables */
	double rmed, rmin, temp, rnew, yout, yi;
	double rbe, rtb, rse, yin, rts, fa, fb;
	int imin, ismo, i, j, first, last, band2, kminus, kplus, shift, bw, step, num_meds, last_med, a, b, x;

	step = skip + 1;
	bw = (((bw_tot - 1) / 2) / step) * 2 + 1;
	if (n < bw_tot)
		return;
	//std::cout << "new: bw=" << bw << ", radius=" << bw / 2 << ", step=" << step << "\n";

	if (n < bw)
		return;
	//double *scrat = (double *)malloc(bw, sizeof(double));
	//was
	double *scrat = (double *)malloc((unsigned)bw * sizeof(double));

	//if ((bw-1)*step+1 > n)
	//	error(_("bandwidth/span of running medians is larger than n"));

	/* 1. Compute  'rmed' := Median of the first 'band' values
	   ======================================================== */

	shift = 0;

	for (i = 0; i < bw; ++i)
		scrat[i] = y[i*step + shift];

	/* find minimal value  rmin = scrat[imin] <= scrat[j] */
	rmin = scrat[0]; imin = 0;
	for (i = 1; i < bw; ++i)
		if (scrat[i] < rmin) {
			rmin = scrat[i]; imin = i;
		}
	/* swap scrat[0] <-> scrat[imin] */
	temp = scrat[0]; scrat[0] = rmin; scrat[imin] = temp;
	/* sort the rest of of scrat[] by bubble (?) sort -- */
	for (i = 2; i < bw; ++i) {
		if (scrat[i] < scrat[i - 1]) {/* find the proper place for scrat[i] */
			temp = scrat[i];
			j = i;
			do {
				scrat[j] = scrat[j - 1];
				--j;
			} while (scrat[j - 1] > temp); /* now:  scrat[j-1] <= temp */
			scrat[j] = temp;
		}
	}
	band2 = bw / 2;
	rmed = scrat[band2];/* == Median( y[(1:band2)-1] ) */
	/* "malloc" had  free( (char*) scrat);*/ /*-- release scratch memory --*/
	if (shift == 0) {
		if (end_rule == 0) { /*-- keep DATA at end values */
			for (i = 0; i < band2*step; ++i)
				smo[i] = y[i];
		}
		else { /* if(end_rule == 1)  copy median to CONSTANT end values */
			for (i = 0; i < band2*step; ++i)
				smo[i] = (float)rmed;
		}
	}
	smo[band2*step + shift] = (float)rmed;
	num_meds = 1;
	band2++; /* = bw / 2 + 1*/;

	/* Big	FOR Loop: RUNNING median, update the median 'rmed'
	   ======================================================= */
	for (first = step + shift, last = bw * step + shift, ismo = band2 * step + shift;
		last < n;
		first += step, last += step, ismo += step) {

		yin = y[last];
		yout = y[first - step];
		rnew = rmed; /* New median = old one   in all the simple cases --*/

		if (yin < rmed) {
			if (yout >= rmed) {
				kminus = 0;
				if (yout > rmed) {/*	--- yin < rmed < yout --- */
					//if (print_level >= 2) REprintf(": yin < rmed < yout ");
					rnew = yin;/* was -rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi < rmed) {
							++kminus;
							if (yi > rnew)	rnew = yi;
						}
					}
					if (kminus < band2)		rnew = rmed;
				}
				else {/*		--- yin < rmed = yout --- */
					rse = rts = yin;/* was -rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi <= rmed) {
							if (yi < rmed) {
								++kminus;
								if (yi > rts)	rts = yi;
								if (yi > rse)	rse = yi;
							}
							else		rse = yi;

						}
					}
					rnew = (kminus == band2) ? rts : rse;
				}
			} /* else: both  yin, yout < rmed -- nothing to do .... */
		}
		else if (yin != rmed) { /* yin > rmed */
			if (yout <= rmed) {
				kplus = 0;
				if (yout < rmed) {/* -- yout < rmed < yin --- */
					rnew = yin; /* was rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi > rmed) {
							++kplus;
							if (yi < rnew)	rnew = yi;
						}
					}
					if (kplus < band2)	rnew = rmed;
				}
				else { /* -- yout = rmed < yin --- */
					rbe = rtb = yin; /* was rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi >= rmed) {
							if (yi > rmed) {
								++kplus;
								if (yi < rtb)	rtb = yi;
								if (yi < rbe)	rbe = yi;
							}
							else		rbe = yi;
						}
					}
					rnew = (kplus == band2) ? rtb : rbe;
				}
			} /* else: both  yin, yout > rmed --> nothing to do */
		} /* else: yin == rmed -- nothing to do .... */
		rmed = rnew;
		smo[ismo] = (float)rmed;
		num_meds++;
	} /*     END FOR ------------ big Loop -------------------- */

	free(scrat);

	band2 = bw / 2;
	last_med = band2 * step + (num_meds - 1)*step;

	if (end_rule == 0) { /*-- keep DATA at end values */
		for (i = last_med + 1; i < n; ++i)
			smo[i] = y[i];
	}
	else { /* if(end_rule == 1)  copy median to CONSTANT end values */
		for (i = last_med + 1; i < n; ++i)
			smo[i] = smo[last_med];
	}

	if (skip > 0) {
		for (i = 0; i < num_meds - 1; i++) {
			for (j = 1; j < step; j++) {
				x = (band2 + i)*step + j;
				a = (band2 + i)*step;
				b = (band2 + i + 1)*step;
				fa = smo[a];
				fb = smo[b];
				smo[x] = (float)(fa + j * (fb - fa) / step); //b-a=step, x-a=j
			}
		}
	}
} /* SrunmedSLI */

//void Algorithm::Run_Median_Stuetzle(float* data, int length, int window, float* out_data)
//{
//	int wn = (window % 2 == 0) ? window - 1 : window;
//	int cn = wn / 2;
//
//	std::vector<float> init_win(wn);
//	for (int k = 0; k < wn; k++)
//	{
//		init_win[k] = data[Run_Med_Idx(k, cn, length)];
//	}
//	std::vector<float> sorted_win = init_win;
//	std::sort(sorted_win.begin(), sorted_win.end());
//
//	float med_curr = sorted_win[cn];
//	int   pos_out = 0;
//	out_data[pos_out++] = med_curr;
//
//	for (int i = 1; i < length; i++)
//	{
//		float d_in = data[Run_Med_Idx(i + cn + cn, cn, length)];
//		float d_out = data[Run_Med_Idx(i - 1, cn, length)];
//
//		if (d_in < med_curr && d_out >= med_curr)
//		{
//			int   med_max_cnt = 0;
//			float med_max = d_in;
//
//			for (int p = i - cn; p <= i + cn; p++)
//			{
//				float data_p = data[Run_Med_Idx(p + cn, cn, length)];
//				if (data_p < med_curr)
//				{
//					if (data_p > med_max)
//						med_max = data_p;
//					med_max_cnt++;
//				}
//			}
//			if (med_max_cnt > cn)
//				med_curr = med_max;
//		}
//		else if (d_in > med_curr && d_out <= med_curr)
//		{
//			int   med_min_cnt = 0;
//			float med_min = d_in;
//
//			for (int p = i - cn; p <= i + cn; p++)
//			{
//				float data_p = data[Run_Med_Idx(p + cn, cn, length)];
//				if (data_p > med_curr)
//				{
//					if (data_p < med_min)
//						med_min = data_p;
//					med_min_cnt++;
//				}
//			}
//			if (med_min_cnt > cn)
//				med_curr = med_min;
//		}
//
//		out_data[pos_out++] = med_curr;
//	}
//}

//void Algorithm::Run_Median_Stuetzle(float* data, int length, int window, float* out_data)
//{
//	int wn;
//	if (window % 2 == 0)
//	{
//		wn = window - 1;
//	}
//	else
//	{
//		wn = window;
//	}
//	int cn = int(wn / 2);
//
//	int pos_out = 0;
//	float med_curr = data[0];
//	out_data[pos_out++] = med_curr;
//
//	int idx;
//	for (int i = 0; i < length - wn + cn * 2; i += 1)
//	{
//		idx = Run_Med_Idx(i + wn, cn, length);
//		float d_in = data[idx];
//		idx = Run_Med_Idx(i, cn, length);
//		float d_out = data[idx];
//
//		int med_cnt = 0;
//		float med_val = d_in;
//
//		if (d_in < med_curr && d_out >= med_curr)
//		{
//			for (int p = i + 1; p <= i + wn; p += 1)
//			{
//				idx = Run_Med_Idx(p, cn, length);
//				float data_p = data[idx];
//				if (data_p < med_curr)
//				{
//					if (data_p > med_val)
//					{
//						med_val = data_p;
//					}
//					med_cnt += 1;
//				}
//			}
//
//			if (med_cnt > cn)
//			{
//				med_curr = med_val;
//			}
//		}
//		else if (d_in > med_curr && d_out <= med_curr)
//		{
//			for (int p = i + 1; p <= i + wn; p += 1)
//			{
//				idx = Run_Med_Idx(p, cn, length);
//				float data_p = data[idx];
//				if (data_p > med_curr)
//				{
//					if (data_p < med_val)
//					{
//						med_val = data_p;
//					}
//					med_cnt += 1;
//				}
//			}
//
//			if (med_cnt > cn)
//			{
//				med_curr = med_val;
//			}
//		}
//
//		out_data[pos_out++] = med_curr;
//	}
//}
//
//int Algorithm::Run_Med_Idx(int idx, int cn, int length)
//{
//	int real_idx = idx - cn;
//
//	if (real_idx < 0)
//	{
//		int mirror_idx = -real_idx;
//		if (mirror_idx >= length)
//			mirror_idx = length - 1; 
//		return mirror_idx;
//	}
//	else if (real_idx >= length)
//	{
//		int mirror_idx = 2 * (length - 1) - real_idx;
//		if (mirror_idx < 0)
//			mirror_idx = 0;
//		if (mirror_idx >= length)
//			mirror_idx = length - 1;
//		return mirror_idx;
//	}
//	else
//	{
//		return real_idx;
//	}
//}

int Algorithm::Run_Med_Idx(int idx, int cn, int length, bool isClosed)
{
	int real_idx = idx - cn;

	if (real_idx < 0)
	{
		if (isClosed)
		{
			int wrap_idx = real_idx + length;
			if (wrap_idx < 0)
				wrap_idx = 0;
			return wrap_idx;
		}
		else
		{
			int mirror_idx = -real_idx;
			if (mirror_idx >= length)
				mirror_idx = length - 1;
			return mirror_idx;
		}
	}
	else if (real_idx >= length)
	{
		if (isClosed)
		{
			int wrap_idx = real_idx - length;
			if (wrap_idx >= length)
				wrap_idx = length - 1;
			return wrap_idx;
		}
		else
		{
			int mirror_idx = 2 * (length - 1) - real_idx;
			if (mirror_idx < 0)
				mirror_idx = 0;
			if (mirror_idx >= length)
				mirror_idx = length - 1;
			return mirror_idx;
		}
	}
	else
	{
		return real_idx;
	}
}

void Algorithm::Run_Med_OneCycle(float* data, int length, int wn, int cn, bool isClosed, bool write_output, float& med_curr, float* out_data, int& pos_out)
{
	int idx;
	for (int i = 0; i < length - wn + cn * 2; i += 1)
	{
		idx = Run_Med_Idx(i + wn, cn, length, isClosed);
		float d_in = data[idx];
		idx = Run_Med_Idx(i, cn, length, isClosed);
		float d_out = data[idx];

		int   med_cnt = 0;
		float med_val = d_in;

		if (d_in < med_curr && d_out >= med_curr)
		{
			for (int p = i + 1; p <= i + wn; p += 1)
			{
				idx = Run_Med_Idx(p, cn, length, isClosed);
				float data_p = data[idx];
				if (data_p < med_curr)
				{
					if (data_p > med_val)
						med_val = data_p;
					med_cnt += 1;
				}
			}
			if (med_cnt > cn)
				med_curr = med_val;
		}
		else if (d_in > med_curr && d_out <= med_curr)
		{
			for (int p = i + 1; p <= i + wn; p += 1)
			{
				idx = Run_Med_Idx(p, cn, length, isClosed);
				float data_p = data[idx];
				if (data_p > med_curr)
				{
					if (data_p < med_val)
						med_val = data_p;
					med_cnt += 1;
				}
			}
			if (med_cnt > cn)
				med_curr = med_val;
		}

		if (write_output)
			out_data[pos_out++] = med_curr;
	}
}

void Algorithm::Run_Median_Stuetzle(float* data, int length, int window, float* out_data, bool isClosed)
{
	int wn = (window % 2 == 0) ? window - 1 : window;
	int cn = int(wn / 2);
	float med_curr;

	if (isClosed)
	{
		std::vector<float> init_win;
		init_win.reserve(wn);
		for (int k = 0; k < wn; k++)
		{
			int idx = Run_Med_Idx(k, cn, length, true);
			init_win.push_back(data[idx]);
		}
		int mid = static_cast<int>(init_win.size()) / 2;
		std::nth_element(init_win.begin(), init_win.begin() + mid, init_win.end());
		med_curr = init_win[mid];
	}
	else
	{
		med_curr = data[0];
	}

	if (isClosed)
	{
		int dummy_pos = 0;
		Run_Med_OneCycle(data, length, wn, cn,
			isClosed, /*write_output=*/false,
			med_curr, nullptr, dummy_pos);
	}

	int pos_out = 0;
	out_data[pos_out++] = med_curr;     // 첫 번째 출력

	Run_Med_OneCycle(data, length, wn, cn, isClosed, true, med_curr, out_data, pos_out);
}

BOOL Algorithm::TrainVariationModelWindow(HObject HTrainImage, GTRegion *pInspectROIRgn, int iInspectTabIdx, int iWindowSize)
{
	try
	{
		++(pInspectROIRgn->m_iVarNoTrainSample[iInspectTabIdx]);

		FullDomain(HTrainImage, &HTrainImage);

		int iNoData = pInspectROIRgn->m_iVarNoTrainSample[iInspectTabIdx];

		if ((THEAPP.m_pGFunction->ValidHImage(pInspectROIRgn->m_HVarMeanImage[iInspectTabIdx]) == FALSE || THEAPP.m_pGFunction->ValidHImage(pInspectROIRgn->m_HVarStdevImage[iInspectTabIdx]) == FALSE) && iNoData == 1)
		{
			CopyImage(HTrainImage, &(pInspectROIRgn->m_HVarMeanImage[iInspectTabIdx]));
			GenImageProto(HTrainImage, &(pInspectROIRgn->m_HVarStdevImage[iInspectTabIdx]), 0);
		}
		else
		{
			int iDen;

			if (iNoData >= iWindowSize)
				iDen = iWindowSize;
			else
				iDen = iNoData;

			HTuple HPrevMeanData, HPrevStdevData, HTrainData;
			BYTE *pPrevMeanData, *pPrevStdevData, *pTrainData;
			Hlong lImageWidth, lImageHeight;
			HTuple Htype, HlImageWidth, HlImageHeight;

			GetImagePointer1(HTrainImage, &HTrainData, &Htype, &HlImageWidth, &HlImageHeight);
			pTrainData = (BYTE *)HTrainData.L();
			lImageWidth = HlImageWidth.L();
			lImageHeight = HlImageHeight.L();

			GetImagePointer1(pInspectROIRgn->m_HVarMeanImage[iInspectTabIdx], &HPrevMeanData, &Htype, &HlImageWidth, &HlImageHeight);
			pPrevMeanData = (BYTE *)HPrevMeanData.L();

			GetImagePointer1(pInspectROIRgn->m_HVarStdevImage[iInspectTabIdx], &HPrevStdevData, &Htype, &HlImageWidth, &HlImageHeight);
			pPrevStdevData = (BYTE *)HPrevStdevData.L();

			int i, j, k;
			double dNewData, dNewMean, dNewStdev, dPrevVariance;

			for (i = 0; i < (lImageWidth*lImageHeight); i++)
			{
				dNewData = pTrainData[i];

				dNewMean = ((double)(iDen - 1) * pPrevMeanData[i] + dNewData) / (double)iDen;

				dPrevVariance = pPrevStdevData[i] * pPrevStdevData[i];
				dNewStdev = ((double)(iDen - 1) * dPrevVariance + (dNewData - pPrevMeanData[i]) * (dNewData - dNewMean)) / (double)iDen;
				dNewStdev = sqrt(dNewStdev);

				if (dNewMean < 0)
					dNewMean = 0;
				if (dNewMean > 255)
					dNewMean = 255;
				if (dNewStdev < 0)
					dNewStdev = 0;
				if (dNewStdev > 255)
					dNewStdev = 255;

				pPrevMeanData[i] = (BYTE)dNewMean;
				pPrevStdevData[i] = (BYTE)dNewStdev;
			}
		}

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [Algorithm::TrainVariationModelWindow] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}
void Algorithm::InspectFAI(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo)
{
	if (THEAPP.m_pFAIInspector != nullptr)
	{
		// TODO : Class 멤버로 Algorithm 포인터(혹은 참조)를 가지게 하려 했으나, 의도대로 동작하지 않음.
		// 아래 함수(SetAlgorithm)로 문제 우회
		THEAPP.m_pFAIInspector->SetAlgorithm(this);	// Algorithm 포인터 갱신용 함수 호출
		THEAPP.m_pFAIInspector->Inspect(bIsAutoInsp, iInspectionBufferIdx, iFAIParamNo);
	}
}

HObject Algorithm::SquareCenterAlgorithm(DPOINT dPoint[8], double &dCPointX, double &dCPointY)
{
	HTuple HRow1, HRow2, HRow3, HRow4;
	HTuple HCol1, HCol2, HCol3, HCol4;
	HTuple HOverlap1, HOverlap2, HOverlap3, HOverlap4;
	HTuple HCenterX, HCenterY;
	HObject HCenterXld;
	GenEmptyObj(&HCenterXld);

	// P1 : A-B
	IntersectionLines(dPoint[0].y, dPoint[0].x, dPoint[1].y, dPoint[1].x, dPoint[2].y, dPoint[2].x, dPoint[3].y, dPoint[3].x, &HRow1, &HCol1, &HOverlap1);

	// P2 : B-C
	IntersectionLines(dPoint[2].y, dPoint[2].x, dPoint[3].y, dPoint[3].x, dPoint[4].y, dPoint[4].x, dPoint[5].y, dPoint[5].x, &HRow2, &HCol2, &HOverlap2);

	// P3 : C-D
	IntersectionLines(dPoint[4].y, dPoint[4].x, dPoint[5].y, dPoint[5].x, dPoint[6].y, dPoint[6].x, dPoint[7].y, dPoint[7].x, &HRow3, &HCol3, &HOverlap3);

	// P4 : D-A
	IntersectionLines(dPoint[6].y, dPoint[6].x, dPoint[7].y, dPoint[7].x, dPoint[0].y, dPoint[0].x, dPoint[1].y, dPoint[1].x, &HRow4, &HCol4, &HOverlap4);

	HCenterX = (HCol1 + HCol2 + HCol3 + HCol4) / 4.0;
	HCenterY = (HRow1 + HRow2 + HRow3 + HRow4) / 4.0;

	dCPointX = HCenterX.D();
	dCPointY = HCenterY.D();

	GenCrossContourXld(&HCenterXld, HCenterY, HCenterX, 30, 0);

	return HCenterXld;

}

HObject Algorithm::SquareCenterAlgorithm(double dA1PointX, double dA1PointY, double dA2PointX, double dA2PointY, double dB1PointX, double dB1PointY, double dB2PointX, double dB2PointY,
	double dC1PointX, double dC1PointY, double dC2PointX, double dC2PointY, double dD1PointX, double dD1PointY, double dD2PointX, double dD2PointY, double &dCPointX, double &dCPointY)
{
	HTuple HRow1, HRow2, HRow3, HRow4;
	HTuple HCol1, HCol2, HCol3, HCol4;
	HTuple HOverlap1, HOverlap2, HOverlap3, HOverlap4;
	HTuple HCenterX, HCenterY;
	HObject HCenterXld;
	GenEmptyObj(&HCenterXld);

	// P1 : A-B
	IntersectionLines(dA1PointY, dA1PointX, dA2PointY, dA2PointX, dB1PointY, dB1PointX, dB2PointY, dB2PointX, &HRow1, &HCol1, &HOverlap1);

	// P2 : B-C
	IntersectionLines(dB1PointY, dB1PointX, dB2PointY, dB2PointX, dC1PointY, dC1PointX, dC2PointY, dC2PointX, &HRow2, &HCol2, &HOverlap2);

	// P3 : C-D
	IntersectionLines(dC1PointY, dC1PointX, dC2PointY, dC2PointX, dD1PointY, dD1PointX, dD2PointY, dD2PointX, &HRow3, &HCol3, &HOverlap3);

	// P4 : D-A
	IntersectionLines(dD1PointY, dD1PointX, dD2PointY, dD2PointX, dA1PointY, dA1PointX, dA2PointY, dA2PointX, &HRow4, &HCol4, &HOverlap4);

	HCenterX = (HCol1 + HCol2 + HCol3 + HCol4) / 4.0;
	HCenterY = (HRow1 + HRow2 + HRow3 + HRow4) / 4.0;

	dCPointX = HCenterX.D();
	dCPointY = HCenterY.D();

	GenCrossContourXld(&HCenterXld, HCenterY, HCenterX, 30, 0);

	return HCenterXld;

}

HObject Algorithm::DistanceMinPointAlgorithm(HObject Contour1, HObject Contour2, double& dC1PointX, double& dC1PointY, double& dC2PointX, double& dC2PointY, double& dDistanceMin)
{

	// 초기화
	HTuple HDistanceMin, HMinIndex;
	HDistanceMin = 99999;
	HMinIndex = 0;

	// Contour1 포인트
	HTuple HPointX, HPointY;
	GetContourXld(Contour1, &HPointY, &HPointX);

	// Contour1의 각 점에 대해 Contour2와의 최단거리 계산
	for (int i = 0; i < HPointX.Length(); i++)
	{
		HTuple HMin, HMax;
		DistancePc(Contour2, HPointY[i].D(), HPointX[i].D(), &HMin, &HMax);

		if (HMin < HDistanceMin)
		{
			HDistanceMin = HMin;
			HMinIndex = i;
		}
	}

	// Contour1 최단거리 좌표
	dC1PointX = HPointX[HMinIndex];
	dC1PointY = HPointY[HMinIndex];

	// 초기화
	HDistanceMin = 99999;
	HMinIndex = 0;

	// Contour2 포인트
	GetContourXld(Contour2, &HPointY, &HPointX);

	// Contour2의 각 점에 대해 Contour1 최단거리 좌표와 최단거리 계산
	for (int i = 0; i < HPointX.Length(); i++)
	{
		HTuple HDistance;
		DistancePp(dC1PointY, dC1PointX, HPointY[i], HPointX[i], &HDistance);

		if (HDistance < HDistanceMin)
		{
			HDistanceMin = HDistance;
			HMinIndex = i;
		}
	}

	// Contour2 최단거리 좌표
	dC2PointX = HPointX[HMinIndex];
	dC2PointY = HPointY[HMinIndex];

	// 최단거리
	dDistanceMin = HDistanceMin.D();

	// 최단거리 XLD
	HObject HXLDLine, HXLDCross1, HXLDCross2;
	GenContourPolygonXld(&HXLDLine, HTuple(dC1PointY).TupleConcat(dC2PointY), HTuple(dC1PointX).TupleConcat(dC2PointX));

	// START
	// 2025.03.24 - LeeGW
	GenCrossContourXld(&HXLDCross1, dC1PointY, dC1PointX, 30, 0);
	GenCrossContourXld(&HXLDCross2, dC2PointY, dC2PointX, 30, 0);
	ConcatObj(HXLDLine, HXLDCross1, &HXLDLine);
	ConcatObj(HXLDLine, HXLDCross2, &HXLDLine);
	// END

	return HXLDLine;

}

BOOL Algorithm::AxisTransAlgorithm(BOOL bIsTransAxisY, double dRotateAngle, double dOPointX, double dOPointY, double& dAxisXEndX, double& dAxisXEndY, double& dAxisYEndX, double& dAxisYEndY)
{
	// Rad 값 변환 
	if (bIsTransAxisY == TRUE)
	{
		HTuple RotateAngleAxisY, RotateAngleAxisX;
		HTuple HomMat2D, HomMat2D_AxisX, HomMat2D_AxisY;
		HTuple axisYEndY, axisYEndX, axisXEndY, axisXEndX;
		HTuple Angle90 = dRotateAngle < 0 ? 90.0 : -90.0;

		RotateAngleAxisY = dRotateAngle * HTuple(PI / 180);	// Y축
		RotateAngleAxisX = (Angle90 + dRotateAngle) * HTuple(PI / 180); // X축

		// 회전 행렬 생성
		HomMat2dIdentity(&HomMat2D);

		// Y축
		HomMat2dRotate(HomMat2D, RotateAngleAxisY, dOPointX, dOPointY, &HomMat2D_AxisY);
		AffineTransPoint2d(HomMat2D_AxisY, dAxisYEndX, dAxisYEndY, &axisYEndX, &axisYEndY);

		// X축
		HomMat2dRotate(HomMat2D, RotateAngleAxisX, dOPointX, dOPointY, &HomMat2D_AxisX);
		AffineTransPoint2d(HomMat2D_AxisX, dAxisYEndX, dAxisYEndY, &axisXEndX, &axisXEndY);

		dAxisXEndX = axisXEndX.D();
		dAxisXEndY = axisXEndY.D();
		dAxisYEndX = axisYEndX.D();
		dAxisYEndY = axisYEndY.D();
	}
	else
	{
		// 추후 X축 회전 축 구현
	}

	return TRUE;
}

HObject Algorithm::BlendingAlgorithm_Horizontal(double dRefer1HStartPosX, double dRefer1HEndPosX, double dRefer2HStartPosX, double dRefer2HEndPosX,
	double dImage1HStartPosX, double dImage1HEndPosX, double dImage2HStartPosX, double dImage2HEndPosX, double dImage1VStartPosX, double dImage1VStartPosY, double dImage1VEndPosX, double dImage1VEndPosY,
	double dImage2VStartPosX, double dImage2VStartPosY, double dImage2VEndPosX, double dImage2VEndPosY, double& dBlendResultX, double& dBlendResultY, double& dBlendOffsetLeft, double& dBlendOffsetRight,
	HObject HImage1, HObject HImage2)
{
	HObject HBlendImage, HResizedImage, HAlignedImage, HAlpha;

	HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DResize;
	HTuple HBlendResultX, HBlendResultY;
	HTuple HWidth, HHeight;

	double dReferDistX = 0, dShiftX = 0, dShiftY = 0, dRotateAngle = 0;
	double dImage1Gradient = 0, dImage1Intercept = 0, dImage2Gradient = 0, dImage2Intercept = 0;
	double dRefer1X = 0, dRefer2X = 0, dImage1X = 0, dImage2X = 0, dImage1Y = 0, dImage2Y = 0;

	GenEmptyObj(&HBlendImage);

	// 이미지2 수평 이동량 계산
	if ((dImage1HStartPosX > 0) && (dImage1HEndPosX > 0) && (dImage2HStartPosX > 0) && (dImage2HEndPosX > 0) &&
		(dRefer1HStartPosX > 0) && (dRefer1HEndPosX > 0) && (dRefer2HStartPosX > 0) && (dRefer2HEndPosX > 0))
	{
		dImage1X = (dImage1HStartPosX + dImage1HEndPosX) * 0.5;
		dImage2X = (dImage2HStartPosX + dImage2HEndPosX) * 0.5;

		dRefer1X = (dRefer1HStartPosX + dRefer1HEndPosX) * 0.5;
		dRefer2X = (dRefer2HStartPosX + dRefer2HEndPosX) * 0.5;

		dReferDistX = abs(dRefer2X - dRefer1X);
		dShiftX = dImage1X + dReferDistX - dImage2X;	// 최종 X 이동량
	}

	if ((dImage1VStartPosX > 0) && (dImage1VStartPosY > 0) && (dImage1VEndPosX > 0) && (dImage1VEndPosY > 0) &&
		(dImage2VStartPosX > 0) && (dImage2VStartPosY > 0) && (dImage2VEndPosX > 0) && (dImage2VEndPosY > 0))
	{
		// 이미지1과 이미지2의 직선 기울기 계산
		dImage1Gradient = (dImage1VEndPosY - dImage1VStartPosY) / (dImage1VEndPosX - dImage1VStartPosX);
		dImage1Intercept = dImage1VStartPosY - (dImage1Gradient * dImage1VStartPosX);

		dImage2Gradient = (dImage2VEndPosY - dImage2VStartPosY) / (dImage2VEndPosX - dImage2VStartPosX);
		dImage2Intercept = dImage2VStartPosY - (dImage2Gradient * dImage2VStartPosX);

		// ShiftX 위치에서의 Y 좌표 차이 계산 (수직 이동량)
		dImage1Y = dImage1Gradient * dRefer2X + dImage1Intercept;
		dImage2Y = dImage2Gradient * dRefer2X + dImage2Intercept;
		dShiftY = dImage1Y - dImage2Y;  // 최종 Y 이동량

		// 기울기 차이를 이용한 회전 각도 보정
		dRotateAngle = atan((dImage1Gradient - dImage2Gradient) / (1 + dImage1Gradient * dImage2Gradient));	// 최종 회전 각도
	}

	// 좌표 변환 적용 (이동 + 회전)
	HomMat2dIdentity(&HomMat2DIdentity);
	HomMat2dTranslate(HomMat2DIdentity, dShiftX, dShiftY, &HomMat2DTranslate);
	HomMat2dRotate(HomMat2DTranslate, dRotateAngle, dRefer2X, dImage1Y, &HomMat2DRotate);
	AffineTransPoint2d(HomMat2DRotate, dBlendResultX, dBlendResultY, &HBlendResultX, &HBlendResultY);

	// 합성 Point & 참조 Point 간의 차이 만큼 좌표이동
	dBlendOffsetLeft = dRefer1X - dImage1X;
	dBlendOffsetRight = dRefer2X - dImage2X;

	dBlendResultX = HBlendResultX.D() + dBlendOffsetLeft;
	dBlendResultY = HBlendResultY.D();

	// 이미지 변환 적용 (이동 + 회전)
	BOOL bDebugSave = FALSE;
	if (bDebugSave)
	{
		if ((THEAPP.m_pGFunction->ValidHImage(HImage1) == TRUE) && (THEAPP.m_pGFunction->ValidHImage(HImage2) == TRUE))
		{
			HomMat2dIdentity(&HomMat2DIdentity);
			HomMat2dTranslate(HomMat2DIdentity, dShiftY, dShiftX, &HomMat2DTranslate);
			HomMat2dRotate(HomMat2DTranslate, dRotateAngle, dRefer2X, dImage1Y, &HomMat2DRotate);
			AffineTransImage(HImage2, &HAlignedImage, HomMat2DRotate, "constant", "true");

			GetImageSize(HAlignedImage, &HWidth, &HHeight);

			HomMat2dIdentity(&HomMat2DResize);
			AffineTransImageSize(HImage1, &HResizedImage, HomMat2DResize, "constant", HWidth, HHeight);

			// 이미지 블렌딩
			AddImage(HResizedImage, HAlignedImage, &HBlendImage, 0.5, 10.0);	// 겹치는 부분 외에 검정색으로 나와서 수정 필요

			WriteImage(HResizedImage, "bmp", 0, "D:\\HResizedImage");
			WriteImage(HAlignedImage, "bmp", 0, "D:\\AlignedImage");
			WriteImage(HBlendImage, "bmp", 0, "D:\\BlendImage");
		}
	}

	return HBlendImage;
}
