#include "stdafx.h"
#include "uScan.h"
#include "InspectService.h"
#include "IniFileCS.h"

#include "BatchService\BatchDefine.h"
#include "BatchService\BatchManager.h"
#include "BatchService\BatchGrabThread.h"

#include <fstream>

static CRITICAL_SECTION LoadCompleteCriticalSection;

CInspectService *CInspectService::m_pInstance = NULL;

CInspectService *CInspectService::GetInstance()
{
	if (!m_pInstance) {
		m_pInstance = new CInspectService();
	}
	return m_pInstance;
}

void CInspectService::DeleteInstance()
{
	if (m_pInstance) delete m_pInstance;
	m_pInstance = NULL;
}

CInspectService::CInspectService(void)
{
	InitializeCriticalSection(&LoadCompleteCriticalSection);

	int i, j;

	for (i = 0; i < MAX_MAGAZINE_NO; i++)
	{
		m_iLotTrayAmt_H[i] = 0;
		m_iLotModuleAmt_H[i] = 0;

		m_bSaveResultLogThreadDone[i] = TRUE;
		m_bContDefectLogThreadDone[i] = TRUE;
	}

	for (i = 0; i < VISION_NUMBER_MAX; i++)
	{
		m_sLotID_H[i] = _T("");
		m_iMzNo_H[i] = 1;
		m_iJigNo_H[i] = 1;
		m_iTrayNo_H[i] = -1;
		m_iModuleNo_H[i] = -1;
	}

	m_iAlignVisionSeqNo_H = 1;

	for (i = 0; i < VISION_NUMBER_MAX; i++)
		m_bOfflineModuleInspectDone[i] = FALSE;

	m_pInspectAlgorithm = new Algorithm[VISION_NUMBER_MAX];

	for (i = 0; i < VISION_NUMBER_MAX; i++)
		m_pInspectAlgorithm[i].Initialize(i);

	for (i = 0; i < VISION_NUMBER_MAX; i++)
	{
		m_iAutoFocusModuleCheckResult[i] = TOP_MODULE_POS_CHECK_NOT_READY;
		m_dAutoFocusModuleRotationAngle[i] = 0;
		m_dAutoFocusDeltaX[i] = 0;
		m_dAutoFocusDeltaY[i] = 0;
	}

	m_iPrevTrayNo = -1;

	for (i = 0; i < IMAGE_SAVE_THREAD_MAX_QUEUE; i++)
	{
		m_bRawImageSaveThreadRunning[i] = FALSE;
		m_bResultImageSaveThreadRunning[i] = FALSE;
		m_bReviewImageSaveThreadRunning[i] = FALSE;
		m_bADJImageSaveThreadRunning[i] = FALSE;
		m_bFAIImageSaveThreadRunning[i] = FALSE;
	}

	m_bResultLogSaveThreadRunning = FALSE;

	for (i = 0; i < RAW_IMAGE_TEMP_DRIVE_NUMBER + 1; i++)
		bFreezingClearCheck[i] = TRUE;

	/////////////////////////////////////////////////////////////////////////
	// 연속 Lot 처리를 위해 변경

	InitializeLotData();

	m_bCycleStopSignaled = FALSE;
	m_bReloadSignaled = FALSE;
	m_bRunStartSignaled = FALSE;
}

CInspectService::~CInspectService(void)
{
	DeleteCriticalSection(&LoadCompleteCriticalSection);

	for (int i = 0; i < IMAGE_SAVE_THREAD_MAX_QUEUE; i++)
	{
		RemoveListSaveRawImageParams(i);
		RemoveListSaveResultImageParams(i);
		RemoveListSaveReviewImageParams(i);
		RemoveListSaveADJImageParams(i);
		RemoveListSaveFAIImageParams(i);
	}

	if (m_pInspectAlgorithm)
	{
		delete[] m_pInspectAlgorithm;
		m_pInspectAlgorithm = NULL;
	}
}

void CInspectService::ResetRawImageSaveThreadRunning(int nIndex)
{
	CSingleLock s(&m_csRawImageSaveThreadRunning);

	s.Lock();
	m_bRawImageSaveThreadRunning[nIndex] = FALSE;
	s.Unlock();
}

BOOL CInspectService::IsSaveRawImageParamEmpty(int nIndex)
{
	BOOL isEmpty = TRUE;

	CSingleLock s(&m_csSaveRawImage);

	s.Lock();
	isEmpty = m_listRawImageSaveParam[nIndex].empty();
	s.Unlock();

	return isEmpty;
}

void CInspectService::AddListSaveRawImageParam(RAW_IMAGE_SAVE_PARAM *pParam, int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo)
{
	CSingleLock s(&m_csSaveRawImage);

	int iCurImageQueueIndex = 0;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
	{
		if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
			iCurImageQueueIndex = (iTrayNo - 1) % THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;

		int iTotalQeueCount = 0;

		strLog.Format("RawImage Thread List, Queue Size: %d", m_listRawImageSaveParam[0].size());

		iTotalQeueCount = iTotalQeueCount + m_listRawImageSaveParam[0].size();

		if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
		{
			for (int i = 1; i < THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber; i++)
			{
				strLog.Format(strLog + ", %d", m_listRawImageSaveParam[i].size());
				iTotalQeueCount = iTotalQeueCount + m_listRawImageSaveParam[i].size();
			}
		}
		THEAPP.m_log_thread->info("{}", LOG_CSTR(strLog));
	}
	else
	{
		BOOL bThreadErrorCheck = FALSE;

		if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
		{
			strLog.Format("RawImage Thread List, Queue Size: %d", m_listRawImageSaveParam[0].size());
			if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1)
				strLog.Format(strLog + ", %d", m_listRawImageSaveParam[1].size());
			if (THEAPP.Struct_PreferenceStruct.m_bUseLAS2)
				strLog.Format(strLog + ", %d", m_listRawImageSaveParam[2].size());
			if (THEAPP.Struct_PreferenceStruct.m_bUseLAS3)
				strLog.Format(strLog + ", %d", m_listRawImageSaveParam[3].size());
			THEAPP.m_log_thread->info("{}", LOG_CSTR(strLog));

			for (int i = 0; i < RAW_IMAGE_TEMP_DRIVE_NUMBER + 1; i++)
			{
				if (i == 0 && m_listRawImageSaveParam[i].size() == 0)
					bFreezingClearCheck[i] = TRUE;

				if (i > 0 && bFreezingClearCheck[0] == TRUE && m_listRawImageSaveParam[i].size() == 0)
					bFreezingClearCheck[i] = TRUE;
			}

			if (bFreezingClearCheck[0] == FALSE || m_listRawImageSaveParam[0].size() >= 30)
			{
				bThreadErrorCheck = TRUE;
				bFreezingClearCheck[0] = FALSE;

				if (bFreezingClearCheck[1] == TRUE && bThreadErrorCheck == TRUE && THEAPP.Struct_PreferenceStruct.m_bUseLAS1 && m_listRawImageSaveParam[1].size() < 30)
				{
					bThreadErrorCheck = FALSE;
					iCurImageQueueIndex = 1;
				}
				else
				{
					if (m_listRawImageSaveParam[1].size() >= 30)
						bFreezingClearCheck[1] = FALSE;
				}

				if (bFreezingClearCheck[2] == TRUE && bThreadErrorCheck == TRUE && THEAPP.Struct_PreferenceStruct.m_bUseLAS2 && m_listRawImageSaveParam[2].size() < 30)
				{
					bThreadErrorCheck = FALSE;
					iCurImageQueueIndex = 2;
				}
				else
				{
					if (m_listRawImageSaveParam[2].size() >= 30)
						bFreezingClearCheck[2] = FALSE;
				}

				if (bFreezingClearCheck[3] == TRUE && bThreadErrorCheck == TRUE && THEAPP.Struct_PreferenceStruct.m_bUseLAS3 && m_listRawImageSaveParam[3].size() < 30)
				{
					bThreadErrorCheck = FALSE;
					iCurImageQueueIndex = 3;
				}
				else
				{
					if (m_listRawImageSaveParam[3].size() >= 30)
						bFreezingClearCheck[3] = FALSE;
				}

				if (bThreadErrorCheck == TRUE)
				{
					if (m_listRawImageSaveParam[0].size() < 20)
						iCurImageQueueIndex = 0;
					else if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1 && m_listRawImageSaveParam[1].size() < 20)
						iCurImageQueueIndex = 1;
					else if (THEAPP.Struct_PreferenceStruct.m_bUseLAS2 && m_listRawImageSaveParam[2].size() < 20)
						iCurImageQueueIndex = 2;
					else if (THEAPP.Struct_PreferenceStruct.m_bUseLAS3 && m_listRawImageSaveParam[3].size() < 20)
						iCurImageQueueIndex = 3;
					else
						iCurImageQueueIndex = 0;
				}
			}
		}
		else
		{
			strLog.Format("RawImage Thread List, Queue Size: %d", m_listRawImageSaveParam[0].size());
			strLog.Format(strLog + ", %d", m_listRawImageSaveParam[1].size());
			THEAPP.m_log_thread->info("{}", LOG_CSTR(strLog));

			for (int i = 0; i < 2; i++)
			{
				if (i == 0 && m_listRawImageSaveParam[i].size() == 0)
					bFreezingClearCheck[i] = TRUE;

				if (i > 0 && bFreezingClearCheck[0] == TRUE && m_listRawImageSaveParam[i].size() == 0)
					bFreezingClearCheck[i] = TRUE;
			}

			if (bFreezingClearCheck[0] == FALSE || m_listRawImageSaveParam[0].size() >= 30)
			{
				bThreadErrorCheck = TRUE;
				bFreezingClearCheck[0] = FALSE;

				if (bFreezingClearCheck[1] == TRUE && bThreadErrorCheck == TRUE && m_listRawImageSaveParam[1].size() < 60)
				{
					bThreadErrorCheck = FALSE;
					iCurImageQueueIndex = 1;
				}
				else
				{
					if (m_listRawImageSaveParam[1].size() >= 60)
						bFreezingClearCheck[1] = FALSE;
				}

				if (bThreadErrorCheck == TRUE)
				{
					if (m_listRawImageSaveParam[0].size() < 20)
						iCurImageQueueIndex = 0;
					else if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1 && m_listRawImageSaveParam[1].size() < 45)
						iCurImageQueueIndex = 1;
					else
						iCurImageQueueIndex = 0;
				}
			}
		}
	}

	s.Lock();
	m_listRawImageSaveParam[iCurImageQueueIndex].push_back(pParam);
	s.Unlock();
}

void CInspectService::RemoveListSaveRawImageParams(int nIndex)
{
	RAW_IMAGE_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveRawImage);

	s.Lock();

	while (!m_listRawImageSaveParam[nIndex].empty())
	{
		pSaveParam = NULL;

		pSaveParam = m_listRawImageSaveParam[nIndex].front();
		m_listRawImageSaveParam[nIndex].pop_front();

		SAFE_DELETE(pSaveParam);
	}

	m_listRawImageSaveParam[nIndex].clear();

	s.Unlock();
}

RAW_IMAGE_SAVE_PARAM* CInspectService::GetNextSaveRawImageParam(int nIndex)
{
	RAW_IMAGE_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveRawImage);

	s.Lock();
	if (!m_listRawImageSaveParam[nIndex].empty())
	{
		pSaveParam = m_listRawImageSaveParam[nIndex].front();
		m_listRawImageSaveParam[nIndex].pop_front();
	}
	else //VER 1.1.2.9 add
	{
		m_listRawImageSaveParam[nIndex].clear(); //VER 1155 JWJ
		pSaveParam = NULL;
	}
	s.Unlock();

	return pSaveParam;
}

void CInspectService::ResetResultImageSaveThreadRunning(int nIndex)
{
	CSingleLock s(&m_csResultImageSaveThreadRunning);

	s.Lock();
	m_bResultImageSaveThreadRunning[nIndex] = FALSE;
	s.Unlock();
}

BOOL CInspectService::IsSaveResultImageParamEmpty(int nIndex)
{
	BOOL isEmpty = TRUE;

	CSingleLock s(&m_csSaveResultImage);

	s.Lock();
	isEmpty = m_listResultImageSaveParam[nIndex].empty();
	s.Unlock();

	return isEmpty;
}

void CInspectService::AddListSaveResultImageParam(RESULT_IMAGE_SAVE_PARAM *pParam, int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo)
{
	CString strLog;
	CSingleLock s(&m_csSaveResultImage);

	int iCurImageQueueIndex = 0;
	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
		iCurImageQueueIndex = (iTrayNo - 1) % THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;

	int iTotalQeueCount = 0;

	strLog.Format("ResultImage Thread List, Queue Size: %d", m_listResultImageSaveParam[0].size());

	iTotalQeueCount = iTotalQeueCount + m_listResultImageSaveParam[0].size();

	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
	{
		for (int i = 1; i < THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber; i++)
		{
			strLog.Format(strLog + ", %d", m_listResultImageSaveParam[i].size());
			iTotalQeueCount = iTotalQeueCount + m_listResultImageSaveParam[i].size();
		}
	}

	s.Lock();
	m_listResultImageSaveParam[iCurImageQueueIndex].push_back(pParam);
	s.Unlock();
}

void CInspectService::RemoveListSaveResultImageParams(int nIndex)
{
	RESULT_IMAGE_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveResultImage);

	s.Lock();

	while (!m_listResultImageSaveParam[nIndex].empty())
	{
		pSaveParam = NULL;

		pSaveParam = m_listResultImageSaveParam[nIndex].front();
		m_listResultImageSaveParam[nIndex].pop_front();

		SAFE_DELETE(pSaveParam);
	}

	m_listResultImageSaveParam[nIndex].clear();

	s.Unlock();
}

RESULT_IMAGE_SAVE_PARAM* CInspectService::GetNextSaveResultImageParam(int nIndex)
{
	RESULT_IMAGE_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveResultImage);

	s.Lock();
	if (!m_listResultImageSaveParam[nIndex].empty())
	{
		pSaveParam = m_listResultImageSaveParam[nIndex].front();
		m_listResultImageSaveParam[nIndex].pop_front();
	}
	else //VER 1.1.2.9 add
	{
		m_listResultImageSaveParam[nIndex].clear();
		pSaveParam = NULL;
	}
	s.Unlock();

	return pSaveParam;
}

void CInspectService::ResetResultLogSaveThreadRunning()
{
	CSingleLock s(&m_csResultLogSaveThreadRunning);

	s.Lock();
	m_bResultLogSaveThreadRunning = FALSE;
	s.Unlock();
}

BOOL CInspectService::IsSaveReviewImageParamEmpty(int nIndex)
{
	BOOL isEmpty = TRUE;

	CSingleLock s(&m_csSaveReviewImage);

	s.Lock();
	isEmpty = m_listReviewImageSaveParam[nIndex].empty();
	s.Unlock();

	return isEmpty;
}

void CInspectService::AddListSaveReviewImageParam(REVIEW_IMAGE_SAVE_PARAM *pParam, int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo)
{
	CString strLog;
	CSingleLock s(&m_csSaveReviewImage);

	int iCurImageQueueIndex = 0;
	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
		iCurImageQueueIndex = (iTrayNo - 1) % THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;

	int iTotalQeueCount = 0;

	strLog.Format("ReviewImage Thread List, Queue Size: %d", m_listReviewImageSaveParam[0].size());

	iTotalQeueCount = iTotalQeueCount + m_listReviewImageSaveParam[0].size();

	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
	{
		for (int i = 1; i < THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber; i++)
		{
			strLog.Format(strLog + ", %d", m_listReviewImageSaveParam[i].size());
			iTotalQeueCount = iTotalQeueCount + m_listReviewImageSaveParam[i].size();
		}
	}

	s.Lock();
	m_listReviewImageSaveParam[iCurImageQueueIndex].push_back(pParam);
	s.Unlock();
}

void CInspectService::RemoveListSaveReviewImageParams(int nIndex)
{
	REVIEW_IMAGE_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveReviewImage);

	s.Lock();

	while (!m_listReviewImageSaveParam[nIndex].empty())
	{
		pSaveParam = NULL;

		pSaveParam = m_listReviewImageSaveParam[nIndex].front();
		m_listReviewImageSaveParam[nIndex].pop_front();

		SAFE_DELETE(pSaveParam);
	}

	m_listReviewImageSaveParam[nIndex].clear();

	s.Unlock();
}

REVIEW_IMAGE_SAVE_PARAM* CInspectService::GetNextSaveReviewImageParam(int nIndex)
{
	REVIEW_IMAGE_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveReviewImage);

	s.Lock();
	if (!m_listReviewImageSaveParam[nIndex].empty())
	{
		pSaveParam = m_listReviewImageSaveParam[nIndex].front();
		m_listReviewImageSaveParam[nIndex].pop_front();
	}
	s.Unlock();

	return pSaveParam;
}


/////////////////
BOOL CInspectService::IsSaveFAIImageParamEmpty(int nIndex)
{
	BOOL isEmpty = TRUE;

	CSingleLock s(&m_csSaveFAIImage);

	s.Lock();
	isEmpty = m_listFAIImageSaveParam[nIndex].empty();
	s.Unlock();

	return isEmpty;
}

void CInspectService::AddListSaveFAIImageParam(FAI_IMAGE_SAVE_PARAM *pParam, int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo)
{
	CString strLog;
	CSingleLock s(&m_csSaveFAIImage);

	int iCurImageQueueIndex = 0;
	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
		iCurImageQueueIndex = (iTrayNo - 1) % THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;

	int iTotalQeueCount = 0;

	strLog.Format("FAI Thread List, Queue Size: %d", m_listFAIImageSaveParam[0].size());

	iTotalQeueCount = iTotalQeueCount + m_listFAIImageSaveParam[0].size();

	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
	{
		for (int i = 1; i < THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber; i++)
		{
			strLog.Format(strLog + ", %d", m_listFAIImageSaveParam[i].size());
			iTotalQeueCount = iTotalQeueCount + m_listFAIImageSaveParam[i].size();
		}
	}

	s.Lock();
	m_listFAIImageSaveParam[iCurImageQueueIndex].push_back(pParam);
	s.Unlock();
}

void CInspectService::RemoveListSaveFAIImageParams(int nIndex)
{
	FAI_IMAGE_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveFAIImage);

	s.Lock();

	while (!m_listFAIImageSaveParam[nIndex].empty())
	{
		pSaveParam = NULL;

		pSaveParam = m_listFAIImageSaveParam[nIndex].front();
		m_listFAIImageSaveParam[nIndex].pop_front();

		SAFE_DELETE(pSaveParam);
	}

	m_listFAIImageSaveParam[nIndex].clear();

	s.Unlock();
}

FAI_IMAGE_SAVE_PARAM* CInspectService::GetNextSaveFAIImageParam(int nIndex)
{
	FAI_IMAGE_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveFAIImage);

	s.Lock();
	if (!m_listFAIImageSaveParam[nIndex].empty())
	{
		pSaveParam = m_listFAIImageSaveParam[nIndex].front();
		m_listFAIImageSaveParam[nIndex].pop_front();
	}
	s.Unlock();

	return pSaveParam;
}
/////////////////

BOOL CInspectService::IsSaveADJImageParamEmpty(int nIndex)
{
	BOOL isEmpty = TRUE;

	CSingleLock s(&m_csSaveADJImage);

	s.Lock();
	isEmpty = m_listADJImageSaveParam[nIndex].empty();
	s.Unlock();

	return isEmpty;
}

void CInspectService::AddListSaveADJImageParam(ADJ_IMAGE_SAVE_PARAM *pParam, int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo)
{
	CString strLog;

	CSingleLock s(&m_csSaveADJImage);

	int iCurImageQueueIndex = 0;
	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
		iCurImageQueueIndex = (iTrayNo - 1) % THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;

	int iTotalQeueCount = 0;

	strLog.Format("ADJImage Thread List, Queue Size: %d", m_listADJImageSaveParam[0].size());

	iTotalQeueCount = iTotalQeueCount + m_listADJImageSaveParam[0].size();

	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
	{
		for (int i = 1; i < THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber; i++)
		{
			strLog.Format(strLog + ", %d", m_listADJImageSaveParam[i].size());
			iTotalQeueCount = iTotalQeueCount + m_listADJImageSaveParam[i].size();
		}
	}

	s.Lock();
	m_listADJImageSaveParam[iCurImageQueueIndex].push_back(pParam);
	s.Unlock();
}

void CInspectService::RemoveListSaveADJImageParams(int nIndex)
{
	ADJ_IMAGE_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveADJImage);

	s.Lock();

	while (!m_listADJImageSaveParam[nIndex].empty())
	{
		pSaveParam = NULL;

		pSaveParam = m_listADJImageSaveParam[nIndex].front();
		m_listADJImageSaveParam[nIndex].pop_front();

		SAFE_DELETE(pSaveParam);
	}

	m_listADJImageSaveParam[nIndex].clear();

	s.Unlock();
}

ADJ_IMAGE_SAVE_PARAM* CInspectService::GetNextSaveADJImageParam(int nIndex)
{
	ADJ_IMAGE_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveADJImage);

	s.Lock();
	if (!m_listADJImageSaveParam[nIndex].empty())
	{
		pSaveParam = m_listADJImageSaveParam[nIndex].front();
		m_listADJImageSaveParam[nIndex].pop_front();
	}
	s.Unlock();

	return pSaveParam;
}

BOOL CInspectService::IsSaveResultLogParamEmpty(int iMzNo)
{
	BOOL isEmpty = TRUE;

	CSingleLock s(&m_csSaveResultLog[iMzNo - 1]);

	s.Lock();
	isEmpty = m_listResultLogSaveParam[iMzNo - 1].empty();
	s.Unlock();

	return isEmpty;
}

void CInspectService::AddListSaveResultLogParam(RESULT_LOG_SAVE_PARAM *pParam, int iMzNo)
{
	CSingleLock s(&m_csSaveResultLog[iMzNo - 1]);

	s.Lock();
	m_listResultLogSaveParam[iMzNo - 1].push_back(pParam);
	s.Unlock();
}

RESULT_LOG_SAVE_PARAM* CInspectService::GetNextSaveResultLogParam(int iMzNo)
{
	RESULT_LOG_SAVE_PARAM* pSaveParam = NULL;

	CSingleLock s(&m_csSaveResultLog[iMzNo - 1]);

	s.Lock();
	if (!m_listResultLogSaveParam[iMzNo - 1].empty())
	{
		pSaveParam = m_listResultLogSaveParam[iMzNo - 1].front();
		m_listResultLogSaveParam[iMzNo - 1].pop_front();
	}
	else //VER 1.1.2.9 add
	{
		m_listResultLogSaveParam[iMzNo - 1].clear(); //VER 1155 JWJ
		pSaveParam = NULL;
	}
	s.Unlock();

	return pSaveParam;
}

void CInspectService::InitializeLotData()
{
	int i, j, k, k1;

	for (i = 0; i < IMAGE_SAVE_THREAD_MAX_QUEUE; i++)
	{
		RemoveListSaveRawImageParams(i);
		RemoveListSaveResultImageParams(i);
		RemoveListSaveReviewImageParams(i);
		RemoveListSaveADJImageParams(i);
		RemoveListSaveFAIImageParams(i);
	}

	for (i = 0; i < VISION_NUMBER_MAX; i++)
	{
		m_pInspectAlgorithm[i].m_iScanBufferIdx = 0;
		m_pInspectAlgorithm[i].RemoveListScanThreadParams();
		m_pInspectAlgorithm[i].RemoveListInspectThreadParams();
		m_pInspectAlgorithm[i].m_iPrevTrayNo = 0;
		m_pInspectAlgorithm[i].m_sPrevLotID = _T("");

		for (j = 0; j < IMAGE_SAVE_THREAD_MAX_QUEUE; j++)
		{
			m_pInspectAlgorithm[i].RemoveListSaveRawImageParams_Cam(j);
		}

		for (j = 0; j < NO_MAX_INSPECT_THREAD; j++)
			m_pInspectAlgorithm[i].m_bAlgorithmThreadRunning[j] = FALSE;
	}

	for (k = 0; k < MAX_MAGAZINE_NO; k++)
	{
		THEAPP.m_StructModuleDataInfo[k].Reset();

		for (i = 0; i < MAX_TRAY_LOT; i++)
		{
			THEAPP.m_bLoadingAlignGrabFailFlag[k][i] = FALSE;

			for (j = 0; j < MAX_MODULE_TRAY; j++)
			{
				THEAPP.m_strModuleBarcodeID[k][i][j] = _T("NOINFO");

				THEAPP.m_StructModuleHistoryInfo[k][i][j].Reset();

				THEAPP.m_StructModuleReviewInfo[k][i][j].Reset();
				THEAPP.m_StructModuleDefectIndexInfo[k][i][j].Reset();

				THEAPP.m_StructModuleBarcodeResult[k][i][j].Reset();
				THEAPP.m_StructModulePrintQuality[k][i][j].Reset();

				THEAPP.m_StructModuleCornerMeasure[k][i][j].Reset();
				THEAPP.m_StructDistanceMeasure[k][i][j].Reset();
				THEAPP.m_StructModuleGapMeasureXY[k][i][j].Reset();

				for (int iii = 0; iii < MAX_FAI_PARAMETER; iii++)
					CFAIDataManager::GetInstance().ResetMeasure(k, i, j, iii);

				THEAPP.m_StructModuleContDefect[k][i][j].Reset();

				THEAPP.m_bHandlerAlarmFlag[k][i][j] = FALSE;

				THEAPP.m_dAlignModuleDeltaX[k][i][j] = 0;
				THEAPP.m_dAlignModuleDeltaY[k][i][j] = 0;
				THEAPP.m_dAlignModuleDeltaAngle[k][i][j] = 0;

				for (int m = 0; m < VISION_NUMBER_MAX; m++)
				{
					THEAPP.m_bLoadCompleteFlag[k][i][j][m] = FALSE;
					THEAPP.m_bScanCompleteFlag[k][i][j][m] = FALSE;

					THEAPP.m_sInspectionResult[k][i][j][m] = _T("nan");
					THEAPP.m_sFinalInspectionResult[k][i][j][m] = _T("nan");

					THEAPP.m_sROSDefectResult[k][i][j][m] = _T("");
					THEAPP.m_sAllDefectsResult[k][i][j][m] = _T("");

					THEAPP.m_sAI1DefectResult[k][i][j][m] = _T("");
					THEAPP.m_sAI2DefectResult[k][i][j][m] = _T("");
					THEAPP.m_sAI3DefectResult[k][i][j][m] = _T("");

					THEAPP.m_StructDefectBlobInfo[k][i][j][m].Reset();
				}

				THEAPP.m_iModuleMixResult[k][i][j] = 1;
			}
		}
	}

	THEAPP.m_FileBase.ResetFolderName();
}

// 핸들러로부터 Get_LotStart 콜 되었을 때... 핸들러로부터 정보는 LOT_ID, LoadPortNo, Tray수량, 검사모듈갯수
void CInspectService::ReadyLot(BOOL bManual, int iMzNo, BOOL bInitThread, CString sHandlerModelName, CString sCurrentLotID, int iModelAutoLoad)
{
	CBatchManager *pBatchManager = CBatchManager::GetInstance();
	pBatchManager->ResetAll(iMzNo);

	int i, j, k;

	if (bInitThread)
	{
		for (i = 0; i < VISION_NUMBER_MAX; i++)
		{
			m_pInspectAlgorithm[i].m_iScanBufferIdx = 0;
			m_pInspectAlgorithm[i].RemoveListScanThreadParams();
			m_pInspectAlgorithm[i].RemoveListInspectThreadParams();
			for (j = 0; j < IMAGE_SAVE_THREAD_MAX_QUEUE; j++)
				m_pInspectAlgorithm[i].RemoveListSaveRawImageParams_Cam(j);
			m_pInspectAlgorithm[i].m_iPrevTrayNo = 0;
			m_pInspectAlgorithm[i].m_sPrevLotID = _T("");

			for (j = 0; j < NO_MAX_INSPECT_THREAD; j++)
				m_pInspectAlgorithm[i].m_bAlgorithmThreadRunning[j] = FALSE;
		}
	}

	for (i = 0; i < MAX_TRAY_LOT; i++)
	{
		for (j = 0; j < MAX_MODULE_TRAY; j++)
		{
			THEAPP.m_strModuleBarcodeID[iMzNo - 1][i][j] = _T("NOINFO");
		}
	}

	for (i = 0; i < MAX_TRAY_LOT; i++)
	{
		THEAPP.m_bLoadingAlignGrabFailFlag[iMzNo - 1][i] = FALSE;

		for (j = 0; j < MAX_MODULE_TRAY; j++)
		{
			THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][i][j].Reset();

			THEAPP.m_StructModuleReviewInfo[iMzNo - 1][i][j].Reset();
			THEAPP.m_StructModuleDefectIndexInfo[iMzNo - 1][i][j].Reset();

			THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][i][j].Reset();
			THEAPP.m_StructModulePrintQuality[iMzNo - 1][i][j].Reset();

			THEAPP.m_StructModuleCornerMeasure[iMzNo - 1][i][j].Reset();
			THEAPP.m_StructDistanceMeasure[iMzNo - 1][i][j].Reset();

			THEAPP.m_StructModuleGapMeasureXY[iMzNo - 1][i][j].Reset();

			for (int iii = 0; iii < MAX_FAI_PARAMETER; iii++)
				CFAIDataManager::GetInstance().ResetMeasure(iMzNo - 1, i, j, iii);

			THEAPP.m_StructModuleContDefect[iMzNo - 1][i][j].Reset();

			THEAPP.m_bHandlerAlarmFlag[iMzNo - 1][i][j] = FALSE;

			THEAPP.m_dAlignModuleDeltaX[iMzNo - 1][i][j] = 0;
			THEAPP.m_dAlignModuleDeltaY[iMzNo - 1][i][j] = 0;
			THEAPP.m_dAlignModuleDeltaAngle[iMzNo - 1][i][j] = 0;

			for (k = 0; k < VISION_NUMBER_MAX; k++)
			{
				THEAPP.m_bLoadCompleteFlag[iMzNo - 1][i][j][k] = FALSE;
				THEAPP.m_bScanCompleteFlag[iMzNo - 1][i][j][k] = FALSE;

				THEAPP.m_sInspectionResult[iMzNo - 1][i][j][k] = _T("nan");
				THEAPP.m_sFinalInspectionResult[iMzNo - 1][i][j][k] = _T("nan");

				THEAPP.m_sROSDefectResult[iMzNo - 1][i][j][k] = _T("");
				THEAPP.m_sAllDefectsResult[iMzNo - 1][i][j][k] = _T("");

				THEAPP.m_sAI1DefectResult[iMzNo - 1][i][j][k] = _T("");
				THEAPP.m_sAI2DefectResult[iMzNo - 1][i][j][k] = _T("");
				THEAPP.m_sAI3DefectResult[iMzNo - 1][i][j][k] = _T("");

				THEAPP.m_StructDefectBlobInfo[iMzNo - 1][i][j][k].Reset();
			}

			THEAPP.m_iModuleMixResult[iMzNo - 1][i][j] = 1;
		}
	}

	bool bExistFolder = FALSE;
	if (sCurrentLotID != THEAPP.m_pInspectResultDlg->m_sLastLotID)
	{
#ifndef INLINE_MODE
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_COUNTTRAY, "1");
#endif
		THEAPP.m_iOfflineCurTrayNo = 1;
	}
	else
	{
#ifndef INLINE_MODE
		if (THEAPP.m_iOfflineCurTrayNo != 1)
			bExistFolder = TRUE;
#endif
	}

	if (bManual == FALSE)
	{
		if (sCurrentLotID != THEAPP.m_pInspectResultDlg->m_sLastLotID)
		{
#ifndef INLINE_MODE
			THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_COUNTTRAY, "1");
#endif
			THEAPP.m_iOfflineCurTrayNo = 1;
		}
	}

	THEAPP.m_pInspectResultDlg->m_sLastLotID = sCurrentLotID;

	CString sProgramVersion, sModelVersion;
	int iModelLoadResult = MODEL_AUTO_LOAD_RESULT_SKIP;

	if (iModelAutoLoad)
		iModelLoadResult = THEAPP.m_pInspectSummary->CheckLotIDAndChangeModel(iMzNo, sCurrentLotID, sHandlerModelName, &sProgramVersion, &sModelVersion);

	if (iModelLoadResult == MODEL_AUTO_LOAD_RESULT_FAIL)
		THEAPP.m_pHandlerService->Set_ErrorRequest(HANDLER_ERROR_MODEL_AUTO_LOAD_FAIL);

	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
	if (bManual == FALSE)
	{
		CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
		CIniFileCS TXT_Status(strStatusFileName);
		CString strSection = "Status";
		TXT_Status.Set_String(strSection, "ModelID", THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName);
		TXT_Status.Set_String(strSection, "LotID", sCurrentLotID);
	}

	THEAPP.m_FileBase.ResetMzFolderName(iMzNo);

	CString sLotStartYear, sLotStartMonth, sLotStartDay;
	int iLotStartYear, iLotStartMonth, iLotStartDay;

	SYSTEMTIME time;
	GetLocalTime(&time);
	iLotStartYear = time.wYear;
	iLotStartMonth = time.wMonth;
	iLotStartDay = time.wDay;

	THEAPP.m_LotstartInfo[iMzNo - 1].SetTime(iLotStartYear, iLotStartMonth, iLotStartDay);

#ifdef INLINE_MODE
	THEAPP.CheckExistLogFileAndDelete(sCurrentLotID, iMzNo, THEAPP.Struct_PreferenceStruct.m_iPCType);
#else
	if (bExistFolder != TRUE)
		THEAPP.CheckExistLogFileAndDelete(sCurrentLotID, iMzNo, THEAPP.Struct_PreferenceStruct.m_iPCType);
#endif

#ifdef USE_SUAKIT
	THEAPP.m_TCPClientService.m_nTimeOutCnt = 0;
	//ADJ auto start
	BOOL bConnectExistAndError = FALSE;
	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)	//각각 통신 연결.
	{
		if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[i])
		{
			for (int j = 0; j < THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO; j++)
			{
				if (!THEAPP.m_TCPClientService.m_bConnect[i][j])
				{
					if (THEAPP.m_TCPClientService.m_arrClient[i][j].TCPConnect())
					{
						THEAPP.m_TCPClientService.m_bConnect[i][j] = TRUE;
					}
					else
					{
						THEAPP.m_TCPClientService.m_bConnect[i][j] = FALSE;
						bConnectExistAndError = TRUE;
					}
				}
			}
			if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[i] && !THEAPP.m_TCPClientService.m_bConnect[i][0])
				THEAPP.m_pHandlerService->Set_ErrorRequest(HANDLER_ERROR_ADJ_CONNECT);
		}
	}

	if (bConnectExistAndError)
	{
		for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)	//각각 통신 연결.
		{
			for (int j = 0; j < MAX_MULTI_CONNECTION_NUM; j++)
				THEAPP.m_TCPClientService.m_arrClient[i][j].TCPReset();
		}
		THEAPP.m_TCPClientService.ReStart();
		Sleep(100);
		THEAPP.m_TCPClientService.Initialize();
	}
#endif

#ifdef INLINE_MODE
	// PyADJ On/Off 확인 TRUE: On, FALSE: Off
	if (THEAPP.Struct_PreferenceStruct.m_bUseLocalADJ)
	{
		BOOL bPyADJConnectSuccess = TRUE;
		bPyADJConnectSuccess = THEAPP.ConnectStatusPyADJ();
		if (!bPyADJConnectSuccess)
		{
			Sleep(1000);
			BOOL bPyADJConnectSuccess = TRUE;
			bPyADJConnectSuccess = THEAPP.ConnectStatusPyADJ();

			if (!bPyADJConnectSuccess && THEAPP.Struct_PreferenceStruct.m_bUseADJConnectionCheck)
				THEAPP.m_pHandlerService->Set_ErrorRequest(HANDLER_ERROR_ADJ_CONNECT);
		}
	}

	// LASAggregator On/Off 확인 TRUE: On, FALSE: Off
	if (THEAPP.Struct_PreferenceStruct.m_bUseLASConnectionCheck)
	{
		BOOL bLASAggregatorConnectSuccess = TRUE;
		bLASAggregatorConnectSuccess = THEAPP.ConnectStatusLASAggregator();
		if (!bLASAggregatorConnectSuccess)
		{
			Sleep(1000);
			BOOL bLASAggregatorConnectSuccess = TRUE;
			bLASAggregatorConnectSuccess = THEAPP.ConnectStatusLASAggregator();

			if (!bLASAggregatorConnectSuccess)
				THEAPP.m_pHandlerService->Set_ErrorRequest(HANDLER_ERROR_LASAGGREGATOR_CONNECT);
		}
	}

	THEAPP.g_iGrabFailCount[iMzNo - 1] = 0;
	THEAPP.g_iDualModelDataRunCheck[iDualModelData]++;

	if (bManual == FALSE)
		THEAPP.m_pHandlerService->Set_LotReady(sCurrentLotID, iMzNo);

	THEAPP.m_pInspectService->SetRunStartStatus(TRUE);

	CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS INI(strStatusFileName);
	CString strSection = "Status";
	INI.Set_Bool(strSection, "RunStart", TRUE);

	string sAutoMode = "";
	int iLotEndTimes = 1;
	int iRepeatTimes = 1;
	if (THEAPP.strAutoSettingMode == "")
	{
		try {
			CString FolderName = "c:\\AutoSettings\\json\\";
			CString jsonFileName = FolderName + "Vision_Start.json";
			std::ifstream file(jsonFileName);
			if (file)
			{
				nlohmann::json JAutoSetting;
				file >> JAutoSetting;

				string strKey_Mode = CT2A("Mode");
				if (JAutoSetting.contains(strKey_Mode))
				{
					sAutoMode = JAutoSetting[strKey_Mode];
					THEAPP.strAutoSettingMode = sAutoMode.c_str();
				}

				string strKey_LotEnd_Times = CT2A("LotEnd_Times");
				if (JAutoSetting.contains(strKey_LotEnd_Times))
					iLotEndTimes = JAutoSetting[strKey_LotEnd_Times];

				string strKey_Repeat_Times = CT2A("Repeat_Times");
				if (JAutoSetting.contains(strKey_Repeat_Times))
					iRepeatTimes = JAutoSetting[strKey_Repeat_Times];

				THEAPP.iAutoSettingCountEnd = iLotEndTimes * iRepeatTimes;
			}
			file.close();
			DeleteFileA((LPCSTR)jsonFileName);
		}
		catch (const std::exception& e)
		{
			// 에러 전송
			sAutoMode = "";
			THEAPP.strAutoSettingMode = sAutoMode.c_str();

			iLotEndTimes = 1;
			iRepeatTimes = 1;
			THEAPP.iAutoSettingCountEnd = iLotEndTimes * iRepeatTimes;
		}
	}
#endif
}

UINT InspectionThread_Align(LPVOID lp)
{
	CString strLog;
	INSP_THREAD_PARAM* pThreadItem = (INSP_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	int iVisionCamType = pThreadItem->iVisionCamType;
	SAFE_DELETE(pThreadItem);

	int iPcVisionNo = VISION_NUMBER_1;

	if (iVisionCamType == VISION_NUMBER_1)
		iPcVisionNo = VISION_NUMBER_1;
	else if (iVisionCamType == VISION_NUMBER_2)
		iPcVisionNo = VISION_NUMBER_2;
	else if (iVisionCamType == VISION_NUMBER_3)
		iPcVisionNo = VISION_NUMBER_3;
	else if (iVisionCamType == VISION_NUMBER_4)
		iPcVisionNo = VISION_NUMBER_4;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int i;
	int iTrayNo, iModuleNo;
	CString sLotID;
	int iMzNo;

	int iAlignVisionSeqNo = pInspectService->m_iAlignVisionSeqNo_H;
	int iGrabBufferIndex = iAlignVisionSeqNo - 1;

	sLotID = pInspectService->m_sLotID_H[iVisionCamType];
	iMzNo = pInspectService->m_iMzNo_H[iVisionCamType];
	iTrayNo = pInspectService->m_iTrayNo_H[iVisionCamType];
	iModuleNo = pInspectService->m_iModuleNo_H[iVisionCamType];

	try
	{
		if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_sModelName == ".")
			return 0;

#ifdef INLINE_MODE
		THEAPP.m_pLightPwmManager[iPcVisionNo]->LightOn();
#endif

		strLog.Format("%s/ Tray %d align inspection start", sVisionCamType_Short, iTrayNo);
		THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

		auto log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		BOOL bGrabFail = FALSE;

		////////////////////////////// 스캔 시작 ////////////////////////////////////

		int iRetryCnt;

#ifdef INLINE_MODE

		log_time_start = std::chrono::high_resolution_clock::now();

		BOOL bGrabSuccess = FALSE;

		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bAlignGrabFail[iGrabBufferIndex] = FALSE;

		iRetryCnt = 0;

		int K = 0;
		for (K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
		{
			++iRetryCnt;

			bGrabSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]->GrabEthernetCamSWTrg(0);

			if (bGrabSuccess)
				break;

		}	// Grab Retry

		if (!bGrabSuccess)
			bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();
		strLog.Format("%s/ Grab, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Grab count: %d", sVisionCamType_Short, log_time_ms, sLotID, iMzNo, iTrayNo, iRetryCnt);
		THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

		if (bGrabFail == TRUE)
		{
			THEAPP.m_bLoadingAlignGrabFailFlag[iMzNo - 1][iTrayNo - 1] = bGrabFail;

			strLog.Format("%s/ Grab fail(No signal), Time(ms): -, LotID: %s, Port: %d, Tray: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo);
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));
		}

		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bAlignGrabFail[iGrabBufferIndex] = bGrabFail;

		bGrabFail = FALSE;
		for (i = 0; i < ALIGN_CAM_GRAB_NUMBER; i++)
		{
			if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bAlignGrabFail[i])
				bGrabFail = TRUE;
		}

#else	// Offline Mode =>

		CString FolderName, ImageName;

		if (THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath != "")
		{
			FolderName.Format(THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath + "\\");
		}
		else
		{
			AfxMessageBox("디버깅 영상 폴더가 지정되지 않았습니다.", MB_SYSTEMMODAL);
			return 0;
		}

		bGrabFail = TRUE;

		char chSep = '_';
		CString sFileBarcode = _T("NOREAD");

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

				int iNoInspectImage = ALIGN_CAM_GRAB_NUMBER;

				int iNoImageYDir, iModular;
				int iCropImageSizeX, iCropImageSizeY;

				iNoImageYDir = iNoInspectImage / MAX_COMBINE_IMAGE_NUMBER;
				iModular = iNoInspectImage % MAX_COMBINE_IMAGE_NUMBER;
				if (iModular > 0)
					iNoImageYDir += 1;

				ImageName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, iNoInspectImage);

				sReadFileName = FolderName + ImageName;

				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);

					FindClose(hFindFile);

					ReadImage(&HCombineImage, (HTuple)strRawImageFileFullName);

					if (THEAPP.m_pGFunction->ValidHImage(HCombineImage) == TRUE)
					{
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImageSize(HCombineImage, &htImageWidth, &htImageHeight);
						int wd = (int)htImageWidth.L();
						int ht = (int)htImageHeight.L();

						iCropImageSizeX = wd / MAX_COMBINE_IMAGE_NUMBER;
						iCropImageSizeY = ht / iNoImageYDir;

						POINT CropLTPoint, CropRBPoint;
						int iImageIndexX, iImageIndexY;
						HObject HCropImage;

						for (i = 0; i < iNoInspectImage; i++)
						{
							iImageIndexX = i % MAX_COMBINE_IMAGE_NUMBER;
							iImageIndexY = i / MAX_COMBINE_IMAGE_NUMBER;

							CropLTPoint.x = iImageIndexX * iCropImageSizeX;
							CropRBPoint.x = iImageIndexX * iCropImageSizeX + iCropImageSizeX - 1;
							CropLTPoint.y = iImageIndexY * iCropImageSizeY;
							CropRBPoint.y = iImageIndexY * iCropImageSizeY + iCropImageSizeY - 1;

							CropRectangle1(HCombineImage, THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage + i, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

							if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i]))
							{
								HTuple HNoCh;
								Hlong lNoCh;

								CountChannels(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i], &HNoCh);
								lNoCh = HNoCh[0].L();
								if (lNoCh == 3)
									Rgb1ToGray(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage + i);

								HTuple htImageHeight = 0;
								HTuple htImageWidth = 0;
								HalconCpp::GetImageSize(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i], &htImageWidth, &htImageHeight);

								if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
								{
									ZoomImageSize(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i], &(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i]), THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");
								}
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
				{
					bFileFindFail = TRUE;
				}

				if (bFileFindFail == FALSE)
				{
					bGrabFail = FALSE;
				}

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

				strLog.Format("Halcon Exception [Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
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

				for (int i = 0; i < THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab; i++)
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

					ReadImage(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage + i, (HTuple)strRawImageFileFullName);

					HTuple HNoCh;
					Hlong lNoCh;

					CountChannels(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i], &HNoCh);
					lNoCh = HNoCh[0].L();
					if (lNoCh == 3)
						Rgb1ToGray(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage + i);

					try
					{
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImageSize(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i], &htImageWidth, &htImageHeight);

						if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
						{
							ZoomImageSize(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i], &(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage[i]), THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");
						}
					}
					catch (HException& except)
					{
						;
					}
				}

				if (bFileFindFail == FALSE)
				{
					bGrabFail = FALSE;
				}

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

				strLog.Format("Halcon Exception [Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
				THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
			}

		}

#endif	// Offline Mode

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		BOOL bCopyRet;

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = 1;
		param.iJigNo = 0;
		param.sTrayID = _T("NoUse");
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.copyMode = AlgorithmCopyParam::MONO_GRAB;
		param.pHImage = THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MvhoCallBackImage;
		param.bColor = FALSE;

		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = ALIGN_CAM_GRAB_NUMBER;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			param.bGrabFail = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].m_bGrabFail[0] = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}

#ifndef INLINE_MODE

#ifdef SIMULATOR_MODE
		if (THEAPP.m_StructSimulationParam.m_bImageLoad == FALSE)
			pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#else
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

#endif

#ifdef INLINE_MODE
		THEAPP.m_pLightPwmManager[iPcVisionNo]->LightOff();
#endif

		return 1;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectionThread_Align] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif
		return 0;
	}
}

UINT InspectionThread_Inspect(LPVOID lp)
{
	CString strLog;

	INSP_THREAD_PARAM* pThreadItem = (INSP_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	int iVisionCamType = pThreadItem->iVisionCamType;
	SAFE_DELETE(pThreadItem);

	int iPcVisionNo = VISION_NUMBER_1;

	if (iVisionCamType == VISION_NUMBER_1)
		iPcVisionNo = VISION_NUMBER_1;
	else if (iVisionCamType == VISION_NUMBER_2)
		iPcVisionNo = VISION_NUMBER_2;
	else if (iVisionCamType == VISION_NUMBER_3)
		iPcVisionNo = VISION_NUMBER_3;
	else if (iVisionCamType == VISION_NUMBER_4)
		iPcVisionNo = VISION_NUMBER_4;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int i;
	CString sLotID;
	int iMzNo, iJigNo, iTrayNo, iModuleNo;
	int iStageNo = STAGE_NUMBER_1;
	sLotID = pInspectService->m_sLotID_H[iVisionCamType];
	iMzNo = pInspectService->m_iMzNo_H[iVisionCamType];
	iJigNo = pInspectService->m_iJigNo_H[iVisionCamType];
	iTrayNo = pInspectService->m_iTrayNo_H[iVisionCamType];
	iModuleNo = pInspectService->m_iModuleNo_H[iVisionCamType];
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

	CString sVisionCamType_Comm;
	sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];

#ifdef INLINE_MODE
	iStageNo = pInspectService->m_iStageNo_H[iVisionCamType];
#endif

	BOOL bInspectionGrab = FALSE;

	int iTotalGrabRetry = 0;
	int iTotalFocusMoveRetry = 0;

	try
	{
		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_sModelName == ".")
			return 0;

		int iCurGrabCircularIndex = THEAPP.m_pDualCameraManager[iPcVisionNo]->GetGrabCircularIndex();

		strLog.Format("%s/ Scan start, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iCurGrabCircularIndex);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		auto total_log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		BOOL bGrabFail = FALSE;
		BOOL bGrabSuccess = TRUE;

		MIL_DOUBLE tMilGrabStart = 0, tMilGrabEnd = 0;

		int iSeqAddrIndex = 0;
		int iNoSeqGrabImage = 1;
		int iNextSeqAddrIndex = 0;

		int iLotModuleIndex, iStageIndex;

		BOOL bSingleDualGrabMode = TRUE;
		////////////////////////////// 스캔 시작 ////////////////////////////////////

#ifdef INLINE_MODE

		bInspectionGrab = TRUE;

		double dPrevCamZ, dPrevLightZ, dPrevHeadX, dPrevStageY, dPrevStageT, dPrevStageR;
		dPrevCamZ = dPrevLightZ = dPrevHeadX = dPrevStageY = dPrevStageT = dPrevStageR = -100000000;
		double dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		DWORD dwZMoveStart = 0, dwZMoveEnd = 0;
		DWORD dwSeqChangeStart = 0, dwSeqChangeEnd = 0;
		BOOL bFocusMoveSuccess = FALSE;

		int iGrabCount = 0;

		int iWSSDelayTime = 0;
		BOOL bAddWSSPostDelay = TRUE;

		// Change Grab Sequence Address
		iSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];
		THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iSeqAddrIndex);

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab)
				break;

			if (iGrabCount >= g_iVisionMaxGrabBuffer[iVisionCamType])
				break;

			dCamZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dZFocusPosition[iStageNo][iNoCurImageGrab];
			dLightZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dLightZPosition[iStageNo][iNoCurImageGrab];
			dHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][iNoCurImageGrab];
			dStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][iNoCurImageGrab];
			dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
			dStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][iNoCurImageGrab];

			dwSeqChangeStart = GetTickCount();

			if (fabs(dCamZ - dPrevCamZ) > Z_MOVE_OFFSET || fabs(dLightZ - dPrevLightZ) > Z_MOVE_OFFSET || fabs(dHeadX - dPrevHeadX) > Z_MOVE_OFFSET || fabs(dStageY - dPrevStageY) > Z_MOVE_OFFSET || fabs(dStageT - dPrevStageT) > Z_MOVE_OFFSET || fabs(dStageR - dPrevStageR) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iVisionCamType, iStageNo, iJigNo, dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR);
				pInspectService->SetCycleStopStatus(FALSE);
				pInspectService->SetReloadStatus(FALSE);

				iNextSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];

				if (iSeqAddrIndex != iNextSeqAddrIndex)
				{
					THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iNextSeqAddrIndex, TRUE);
					bAddWSSPostDelay = TRUE;
				}

				dwZMoveStart = GetTickCount();

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType]) // 무브 컴플리트 기다림
				{
					if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
					{
						Sleep(10);

						return 0;
					}

					Sleep(1);
				}

				dPrevCamZ = dCamZ;
				dPrevLightZ = dLightZ;
				dPrevHeadX = dHeadX;
				dPrevStageY = dStageY;
				dPrevStageT = dStageT;
				dPrevStageR = dStageR;
			}
			else
			{
				iNextSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];

				if (iSeqAddrIndex != iNextSeqAddrIndex)
				{
					THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iNextSeqAddrIndex, TRUE, TRUE);
					bAddWSSPostDelay = TRUE;
				}
			}

			iRetryCnt = 0;
			int K = 0;

			bGrabSuccess = FALSE;

			iSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];
			iNoSeqGrabImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iAddrCount[iSeqAddrIndex];
			iWSSDelayTime = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iWSSDelayTime[iSeqAddrIndex];

			if (bAddWSSPostDelay && iWSSDelayTime > 0)
				Sleep(iWSSDelayTime);
			bAddWSSPostDelay = FALSE;

#ifdef USE_MATROX_TIMER
			MappTimer(M_DEFAULT, M_TIMER_READ + M_SYNCHRONOUS, &tMilGrabStart);
#else
			log_time_start = std::chrono::high_resolution_clock::now();
#endif

			for (K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				if (K > 0)
					++iTotalGrabRetry;

				bGrabSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraStartGrab(iGrabCount, iNoSeqGrabImage);

				if (bGrabSuccess)
					break;
			}	// Grab Retry

			if (bGrabSuccess == FALSE)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

#ifdef USE_MATROX_TIMER
			MappTimer(M_DEFAULT, M_TIMER_READ + M_SYNCHRONOUS, &tMilGrabEnd);
			strLog.Format("%s/ Grab, Time(ms): %.0lf, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, Grab count: %d", sVisionCamType_Comm, (tMilGrabEnd - tMilGrabStart) * 1000.0, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iRetryCnt);
#else
			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();
			strLog.Format("%s/ Grab, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, Grab count: %d", sVisionCamType_Comm, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iRetryCnt);
#endif
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

			if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularImage[iNoCurImageGrab] == TRUE)
			{
				iNoCurImageGrab += DFM_USED_CONV_IMAGE_NUMBER;
				iGrabCount += SPV_RAW_IMAGE_NUMBER;
			}
			else if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseDiffusedImage[iNoCurImageGrab] == TRUE)
			{
				iNoCurImageGrab += DIFFUSED_USED_CONV_IMAGE_NUMBER;
				iGrabCount += DIFFUSED_RAW_IMAGE_NUMBER;
			}
			else
			{
				iNoCurImageGrab += iNoSeqGrabImage;
				iGrabCount += iNoSeqGrabImage;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		if (bGrabFail == TRUE)
		{
			strLog.Format("%s/ Grab Fail(No signal), Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Comm, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));
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

					return 0;
				}
			}

			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("%s/ Grab Complete Holding, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Comm, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));
		}

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		auto total_log_time_end = std::chrono::high_resolution_clock::now();
		auto total_log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(total_log_time_end - total_log_time_start).count();
		strLog.Format("%s/ Scan done, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d", sVisionCamType_Short, total_log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iCurGrabCircularIndex);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		for (i = 0; i < iGrabCount; i++)
		{
			if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
		}

#else	// Offline Mode =>

		HObject HInspectImage[MAX_IMAGE_TAB][3];

		for (int iii = 0; iii < MAX_IMAGE_TAB; iii++)
		{
			for (int jjj = 0; jjj < 3; jjj++)
				GenEmptyObj(&(HInspectImage[iii][jjj]));
		}

		CString FolderName, ImageName;

		if (THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath != "")
			FolderName.Format(THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath + "\\");
		else
		{
			AfxMessageBox("디버깅 영상 폴더가 지정되지 않았습니다.", MB_SYSTEMMODAL);
			return 0;
		}

		bGrabFail = TRUE;

		char chSep = '_';
		CString sFileBarcode = _T("NOREAD");

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

				int iNoInspectImage = 0;
				iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

				int iNoImageYDir, iModular;
				int iCropImageSizeX, iCropImageSizeY;

				iNoImageYDir = iNoInspectImage / MAX_COMBINE_IMAGE_NUMBER;
				iModular = iNoInspectImage % MAX_COMBINE_IMAGE_NUMBER;
				if (iModular > 0)
					iNoImageYDir += 1;

				ImageName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, iNoInspectImage);
				sReadFileName = FolderName + ImageName;

				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);

					FindClose(hFindFile);

					ReadImage(&HCombineImage, (HTuple)strRawImageFileFullName);

					AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);

					if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
						sFileBarcode.TrimRight(".bmp");
					else
						sFileBarcode.TrimRight(".jpg");

					EnterCriticalSection(&THEAPP.m_csBarcodeRead);

					THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;

					LeaveCriticalSection(&THEAPP.m_csBarcodeRead);

					if (THEAPP.m_pGFunction->ValidHImage(HCombineImage) == TRUE)
					{
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImageSize(HCombineImage, &htImageWidth, &htImageHeight);
						int wd = (int)htImageWidth.L();
						int ht = (int)htImageHeight.L();

						iCropImageSizeX = wd / MAX_COMBINE_IMAGE_NUMBER;
						iCropImageSizeY = ht / iNoImageYDir;

						POINT CropLTPoint, CropRBPoint;
						int iImageIndexX, iImageIndexY;
						HObject HCropImage;

						for (i = 0; i < iNoInspectImage; i++)
						{
							iImageIndexX = i % MAX_COMBINE_IMAGE_NUMBER;
							iImageIndexY = i / MAX_COMBINE_IMAGE_NUMBER;

							CropLTPoint.x = iImageIndexX * iCropImageSizeX;
							CropRBPoint.x = iImageIndexX * iCropImageSizeX + iCropImageSizeX - 1;
							CropLTPoint.y = iImageIndexY * iCropImageSizeY;
							CropRBPoint.y = iImageIndexY * iCropImageSizeY + iCropImageSizeY - 1;

							CropRectangle1(HCombineImage, &HColorImage, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

							if (THEAPP.m_pGFunction->ValidHImage(HColorImage))
							{
								HTuple htImageHeight = 0;
								HTuple htImageWidth = 0;
								HalconCpp::GetImageSize(HColorImage, &htImageWidth, &htImageHeight);

								if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
									ZoomImageSize(HColorImage, &HColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");

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
				HTuple HExp;
				HTuple HOperatorName, HErrMsg;
				CString sOperatorName, sErrMsg;
				except.ToHTuple(&HExp);
				except.GetExceptionData(HExp, "operator", &HOperatorName);
				except.GetExceptionData(HExp, "error_message", &HErrMsg);
				sOperatorName = HOperatorName.S();
				sErrMsg = HErrMsg.S();

				strLog.Format("Halcon Exception [InspectService::InspectionThread_Inspect Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
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

				for (int i = 0; i < THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab; i++)
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
						HTuple htImgPtrR, htImgPtrG, htImgPtrB;
						HTuple htType = 0;
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImagePointer3(HColorImage, &htImgPtrR, &htImgPtrG, &htImgPtrB, &htType, &htImageWidth, &htImageHeight);

						if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
							ZoomImageSize(HColorImage, &HColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");
					}
					catch (HException &except)
					{
						;
					}

					if (i == (THEAPP.m_nOfflineBarcodeImageNo - 1))		// default: if (i == 0)
					{
						AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);

						if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
							sFileBarcode.TrimRight(".bmp");
						else
							sFileBarcode.TrimRight(".jpg");

						EnterCriticalSection(&THEAPP.m_csBarcodeRead);

						THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;

						LeaveCriticalSection(&THEAPP.m_csBarcodeRead);
					}

					Decompose3(HColorImage, &(HInspectImage[i][0]), &(HInspectImage[i][1]), &(HInspectImage[i][2]));
				}

				if (bFileFindFail == FALSE)
					bGrabFail = FALSE;
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

				strLog.Format("Halcon Exception [InspectService::InspectionThread_Inspect Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
				THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
			}
		}

		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

#endif	// Offline Mode

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		BOOL bCopyRet;
		int i;

#ifdef INLINE_MODE
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

		if (bGrabFail)
		{
			if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
			{
				THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
			}
		}

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = iStageNo;
		param.iJigNo = iJigNo;
		param.sTrayID = _T("NoUse");
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.iCurCircularGrabIdx = iCurGrabCircularIndex;
		param.iNoGrabRetry = iTotalGrabRetry;
		param.iNoFocusMoveRetry = iTotalFocusMoveRetry;
		param.copyMode = AlgorithmCopyParam::DFM;
		param.ppGrabHImage = THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex];
		param.bColor = TRUE;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
			{
				THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
			}

			param.bGrabFail = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}
#else
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = iStageNo;
		param.iJigNo = iJigNo;
		param.sTrayID = _T("NoUse");
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.iCurCircularGrabIdx = iCurGrabCircularIndex;
		param.iNoGrabRetry = iTotalGrabRetry;
		param.iNoFocusMoveRetry = iTotalFocusMoveRetry;
		param.copyMode = AlgorithmCopyParam::NORMAL;
		param.ppGrabHImage = HInspectImage;
		param.bColor = TRUE;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			param.bGrabFail = TRUE;;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}
#endif

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bGrabCircularBufferCopyDone[iCurGrabCircularIndex] = TRUE;

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectService::InspectionThread_Inspect] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

#ifdef INLINE_MODE
		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
#endif
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		return 0;
	}
}

UINT InspectionThread_Inspect_OneGrabFunction(LPVOID lp)
{
	CString strLog;

	INSP_THREAD_PARAM* pThreadItem = (INSP_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	int iVisionCamType = pThreadItem->iVisionCamType;
	SAFE_DELETE(pThreadItem);

	int iPcVisionNo = VISION_NUMBER_1;

	if (iVisionCamType == VISION_NUMBER_1)
		iPcVisionNo = VISION_NUMBER_1;
	else if (iVisionCamType == VISION_NUMBER_2)
		iPcVisionNo = VISION_NUMBER_2;
	else if (iVisionCamType == VISION_NUMBER_3)
		iPcVisionNo = VISION_NUMBER_3;
	else if (iVisionCamType == VISION_NUMBER_4)
		iPcVisionNo = VISION_NUMBER_4;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int i;
	CString sLotID;
	int iMzNo, iJigNo, iTrayNo, iModuleNo;
	int iStageNo = STAGE_NUMBER_1;
	sLotID = pInspectService->m_sLotID_H[iVisionCamType];
	iMzNo = pInspectService->m_iMzNo_H[iVisionCamType];
	iJigNo = pInspectService->m_iJigNo_H[iVisionCamType];
	iTrayNo = pInspectService->m_iTrayNo_H[iVisionCamType];
	iModuleNo = pInspectService->m_iModuleNo_H[iVisionCamType];
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

#ifdef INLINE_MODE
	iStageNo = pInspectService->m_iStageNo_H[iVisionCamType];
#endif

	CString sVisionCamType_Comm;
	sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];

	BOOL bInspectionGrab = FALSE;

	int iTotalGrabRetry = 0;
	int iTotalFocusMoveRetry = 0;

	try
	{
		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_sModelName == ".")
			return 0;

		int iCurGrabCircularIndex = THEAPP.m_pDualCameraManager[iPcVisionNo]->GetGrabCircularIndex();

		strLog.Format("%s/ Scan start, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iCurGrabCircularIndex);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		auto total_log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		BOOL bGrabFail = FALSE;
		BOOL bGrabSuccess = TRUE;

		MIL_DOUBLE tMilGrabStart = 0, tMilGrabEnd = 0;

		int iSeqAddrIndex = 0;
		int iNoSeqGrabImage = 1;
		int iNextSeqAddrIndex = 0;

		int iLotModuleIndex, iStageIndex;

		BOOL bSingleDualGrabMode = TRUE;
		////////////////////////////// 스캔 시작 ////////////////////////////////////

#ifdef INLINE_MODE

		bInspectionGrab = TRUE;

		double dPrevCamZ, dPrevLightZ, dPrevHeadX, dPrevStageY, dPrevStageT, dPrevStageR;
		dPrevCamZ = dPrevLightZ = dPrevHeadX = dPrevStageY = dPrevStageT = dPrevStageR = -100000000;
		double dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		DWORD dwZMoveStart = 0, dwZMoveEnd = 0;
		DWORD dwSeqChangeStart = 0, dwSeqChangeEnd = 0;
		BOOL bFocusMoveSuccess = FALSE;

		int iGrabCount = 0;
		int iNoGrabing = 0;
		int iEndGrabIndex = 0;

		int iWSSDelayTime = 0;
		BOOL bAddWSSPostDelay = TRUE;

		iNoGrabing = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->GetGrabingNumber(iStageNo, iNoCurImageGrab);
		THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Start(iGrabCount, iNoGrabing);

		// Change Grab Sequence Address
		iSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];
		THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iSeqAddrIndex);

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab)
				break;

			if (iGrabCount >= g_iVisionMaxGrabBuffer[iVisionCamType])
				break;

			dCamZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dZFocusPosition[iStageNo][iNoCurImageGrab];
			dLightZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dLightZPosition[iStageNo][iNoCurImageGrab];
			dHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][iNoCurImageGrab];
			dStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][iNoCurImageGrab];
			dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
			dStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][iNoCurImageGrab];

			dwSeqChangeStart = GetTickCount();

			if (fabs(dCamZ - dPrevCamZ) > Z_MOVE_OFFSET || fabs(dLightZ - dPrevLightZ) > Z_MOVE_OFFSET || fabs(dHeadX - dPrevHeadX) > Z_MOVE_OFFSET || fabs(dStageY - dPrevStageY) > Z_MOVE_OFFSET || fabs(dStageT - dPrevStageT) > Z_MOVE_OFFSET || fabs(dStageR - dPrevStageR) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iVisionCamType, iStageNo, iJigNo, dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR);
				pInspectService->SetCycleStopStatus(FALSE);
				pInspectService->SetReloadStatus(FALSE);

				iNextSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];

				if (iSeqAddrIndex != iNextSeqAddrIndex)
				{
					THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iNextSeqAddrIndex, TRUE);
					bAddWSSPostDelay = TRUE;
				}

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType]) // 무브 컴플리트 기다림
				{
					if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
					{
						THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
						Sleep(10);

						return 0;
					}

					Sleep(1);
				}

				dPrevCamZ = dCamZ;
				dPrevLightZ = dLightZ;
				dPrevHeadX = dHeadX;
				dPrevStageY = dStageY;
				dPrevStageT = dStageT;
				dPrevStageR = dStageR;
			}
			else
			{
				iNextSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];

				if (iSeqAddrIndex != iNextSeqAddrIndex)
				{
					THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iNextSeqAddrIndex, TRUE, TRUE);
					bAddWSSPostDelay = TRUE;
				}
			}

			iRetryCnt = 0;
			int K = 0;

			bGrabSuccess = FALSE;

			iSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];
			iNoSeqGrabImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iAddrCount[iSeqAddrIndex];
			iWSSDelayTime = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iWSSDelayTime[iSeqAddrIndex];

			if (bAddWSSPostDelay && iWSSDelayTime > 0)
				Sleep(iWSSDelayTime);
			bAddWSSPostDelay = FALSE;

#ifdef USE_MATROX_TIMER
			MappTimer(M_DEFAULT, M_TIMER_READ + M_SYNCHRONOUS, &tMilGrabStart);
#else
			log_time_start = std::chrono::high_resolution_clock::now();
#endif

			for (K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				if (K > 0)
					++iTotalGrabRetry;

				bGrabSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction(iGrabCount, iNoSeqGrabImage);

				if (bGrabSuccess)
					break;
				else
				{
					if (K < (THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo - 1))
					{
						THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
						iNoGrabing = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->GetGrabingNumber(iStageNo, iNoCurImageGrab);
						THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Start(iGrabCount, iNoGrabing);
						iEndGrabIndex = iGrabCount;
					}
				}
			}	// Grab Retry

			if (bGrabSuccess == FALSE)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

#ifdef USE_MATROX_TIMER
			MappTimer(M_DEFAULT, M_TIMER_READ + M_SYNCHRONOUS, &tMilGrabEnd);
			strLog.Format("%s/ Grab, Time(ms): %.0lf, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, Grab count: %d", sVisionCamType_Comm, (tMilGrabEnd - tMilGrabStart) * 1000.0, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iRetryCnt);
#else
			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();
			strLog.Format("%s/ Grab, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, Grab count: %d", sVisionCamType_Comm, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iRetryCnt);
#endif
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

			if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularImage[iNoCurImageGrab] == TRUE)
			{
				iNoCurImageGrab += DFM_USED_CONV_IMAGE_NUMBER;
				iGrabCount += SPV_RAW_IMAGE_NUMBER;
			}
			else if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseDiffusedImage[iNoCurImageGrab] == TRUE)
			{
				iNoCurImageGrab += DIFFUSED_USED_CONV_IMAGE_NUMBER;
				iGrabCount += DIFFUSED_RAW_IMAGE_NUMBER;
			}
			else
			{
				iNoCurImageGrab += iNoSeqGrabImage;
				iGrabCount += iNoSeqGrabImage;
			}

			if (bGrabSuccess == FALSE)
			{
				THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
				iNoGrabing = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->GetGrabingNumber(iStageNo, iNoCurImageGrab);
				THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Start(iGrabCount, iNoGrabing);
				iEndGrabIndex = iGrabCount;
			}
		}

		THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bGrabIndexMismatchDetected = FALSE;
		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_iGrabIndexMismatchOffset = 0;

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		if (bGrabFail == TRUE)
		{
			strLog.Format("%s/ Grab fail(No signal), Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));
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

					return 0;
				}
			}

			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("%s/ Scan complete holding, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Comm, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));
		}

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		auto total_log_time_end = std::chrono::high_resolution_clock::now();
		auto total_log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(total_log_time_end - total_log_time_start).count();
		strLog.Format("%s/ Scan done, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d", sVisionCamType_Short, total_log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iCurGrabCircularIndex);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		for (i = 0; i < iGrabCount; i++)
		{
			if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
		}

#else	// Offline Mode =>
		HObject HInspectImage[MAX_IMAGE_TAB][3];

		for (int iii = 0; iii < MAX_IMAGE_TAB; iii++)
		{
			for (int jjj = 0; jjj < 3; jjj++)
				GenEmptyObj(&(HInspectImage[iii][jjj]));
		}

		CString FolderName, ImageName;

		if (THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath != "")
			FolderName.Format(THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath + "\\");
		else
		{
			AfxMessageBox("디버깅 영상 폴더가 지정되지 않았습니다.", MB_SYSTEMMODAL);
			return 0;
		}

		bGrabFail = TRUE;

		char chSep = '_';
		CString sFileBarcode = _T("NOREAD");

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

				int iNoInspectImage = 0;
				iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

				int iNoImageYDir, iModular;
				int iCropImageSizeX, iCropImageSizeY;

				iNoImageYDir = iNoInspectImage / MAX_COMBINE_IMAGE_NUMBER;
				iModular = iNoInspectImage % MAX_COMBINE_IMAGE_NUMBER;
				if (iModular > 0)
					iNoImageYDir += 1;

				ImageName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, iNoInspectImage);
				sReadFileName = FolderName + ImageName;

				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);

					FindClose(hFindFile);

					ReadImage(&HCombineImage, (HTuple)strRawImageFileFullName);

					AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);

					if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
						sFileBarcode.TrimRight(".bmp");
					else
						sFileBarcode.TrimRight(".jpg");

					EnterCriticalSection(&THEAPP.m_csBarcodeRead);

					THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;

					LeaveCriticalSection(&THEAPP.m_csBarcodeRead);

					if (THEAPP.m_pGFunction->ValidHImage(HCombineImage) == TRUE)
					{
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImageSize(HCombineImage, &htImageWidth, &htImageHeight);
						int wd = (int)htImageWidth.L();
						int ht = (int)htImageHeight.L();

						iCropImageSizeX = wd / MAX_COMBINE_IMAGE_NUMBER;
						iCropImageSizeY = ht / iNoImageYDir;

						POINT CropLTPoint, CropRBPoint;
						int iImageIndexX, iImageIndexY;
						HObject HCropImage;

						for (i = 0; i < iNoInspectImage; i++)
						{
							iImageIndexX = i % MAX_COMBINE_IMAGE_NUMBER;
							iImageIndexY = i / MAX_COMBINE_IMAGE_NUMBER;

							CropLTPoint.x = iImageIndexX * iCropImageSizeX;
							CropRBPoint.x = iImageIndexX * iCropImageSizeX + iCropImageSizeX - 1;
							CropLTPoint.y = iImageIndexY * iCropImageSizeY;
							CropRBPoint.y = iImageIndexY * iCropImageSizeY + iCropImageSizeY - 1;

							CropRectangle1(HCombineImage, &HColorImage, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

							if (THEAPP.m_pGFunction->ValidHImage(HColorImage))
							{
								HTuple htImageHeight = 0;
								HTuple htImageWidth = 0;
								HalconCpp::GetImageSize(HColorImage, &htImageWidth, &htImageHeight);

								if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
									ZoomImageSize(HColorImage, &HColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");

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
				HTuple HExp;
				HTuple HOperatorName, HErrMsg;
				CString sOperatorName, sErrMsg;
				except.ToHTuple(&HExp);
				except.GetExceptionData(HExp, "operator", &HOperatorName);
				except.GetExceptionData(HExp, "error_message", &HErrMsg);
				sOperatorName = HOperatorName.S();
				sErrMsg = HErrMsg.S();

				strLog.Format("Halcon Exception [InspectService::InspectThread_Inspect_OneGrabFunction Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
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

				for (int i = 0; i < THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab; i++)
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
						HTuple htImgPtrR, htImgPtrG, htImgPtrB;
						HTuple htType = 0;
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImagePointer3(HColorImage, &htImgPtrR, &htImgPtrG, &htImgPtrB, &htType, &htImageWidth, &htImageHeight);

						if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
							ZoomImageSize(HColorImage, &HColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");
					}
					catch (HException &except)
					{
						;
					}

					if (i == (THEAPP.m_nOfflineBarcodeImageNo - 1))		// default: if (i == 0)
					{
						AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);

						if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
							sFileBarcode.TrimRight(".bmp");
						else
							sFileBarcode.TrimRight(".jpg");

						EnterCriticalSection(&THEAPP.m_csBarcodeRead);

						THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;

						LeaveCriticalSection(&THEAPP.m_csBarcodeRead);
					}

					Decompose3(HColorImage, &(HInspectImage[i][0]), &(HInspectImage[i][1]), &(HInspectImage[i][2]));
				}

				if (bFileFindFail == FALSE)
					bGrabFail = FALSE;
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

				strLog.Format("Halcon Exception [InspectService::InspectThread_Inspect_OneGrabFunction Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
				THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
			}
		}

		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

#endif	// Offline Mode

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		BOOL bCopyRet;
		int i;

#ifdef INLINE_MODE
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

		if (bGrabFail)
		{
			if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
			{
				THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
			}
		}

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = iStageNo;
		param.iJigNo = iJigNo;
		param.sTrayID = _T("NoUse");
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.iCurCircularGrabIdx = iCurGrabCircularIndex;
		param.iNoGrabRetry = iTotalGrabRetry;
		param.iNoFocusMoveRetry = iTotalFocusMoveRetry;
		param.copyMode = AlgorithmCopyParam::DFM;
		param.ppGrabHImage = THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex];
		param.bColor = TRUE;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
			{
				THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
			}

			param.bGrabFail = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}
#else
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = iStageNo;
		param.iJigNo = iJigNo;
		param.sTrayID = _T("NoUse");
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.iCurCircularGrabIdx = iCurGrabCircularIndex;
		param.iNoGrabRetry = iTotalGrabRetry;
		param.iNoFocusMoveRetry = iTotalFocusMoveRetry;
		param.copyMode = AlgorithmCopyParam::NORMAL;
		param.ppGrabHImage = HInspectImage;
		param.bColor = TRUE;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			param.bGrabFail = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}
#endif

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bGrabCircularBufferCopyDone[iCurGrabCircularIndex] = TRUE;

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectService::InspectThread_Inspect_OneGrabFunction] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

#ifdef INLINE_MODE
		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
#endif
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		return 0;
	}
}

UINT InspectionThread_Inspect_AutoSettingZ(LPVOID lp)
{
	CString strLog;

	INSP_THREAD_PARAM* pThreadItem = (INSP_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	int iVisionCamType = pThreadItem->iVisionCamType;
	SAFE_DELETE(pThreadItem);

	int iPcVisionNo = VISION_NUMBER_1;

	if (iVisionCamType == VISION_NUMBER_1)
		iPcVisionNo = VISION_NUMBER_1;
	else if (iVisionCamType == VISION_NUMBER_2)
		iPcVisionNo = VISION_NUMBER_2;
	else if (iVisionCamType == VISION_NUMBER_3)
		iPcVisionNo = VISION_NUMBER_3;
	else if (iVisionCamType == VISION_NUMBER_4)
		iPcVisionNo = VISION_NUMBER_4;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int i;
	CString sLotID;
	int iMzNo, iJigNo, iTrayNo, iModuleNo;
	int iStageNo = STAGE_NUMBER_1;
	sLotID = pInspectService->m_sLotID_H[iVisionCamType];
	iMzNo = pInspectService->m_iMzNo_H[iVisionCamType];
	iJigNo = pInspectService->m_iJigNo_H[iVisionCamType];
	iTrayNo = pInspectService->m_iTrayNo_H[iVisionCamType];
	iModuleNo = pInspectService->m_iModuleNo_H[iVisionCamType];
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

#ifdef INLINE_MODE
	iStageNo = pInspectService->m_iStageNo_H[iVisionCamType];
#endif

	CString sVisionCamType_Comm;
	sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];

	BOOL bInspectionGrab = FALSE;

	int iTotalGrabRetry = 0;
	int iTotalFocusMoveRetry = 0;

	try
	{
		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_sModelName == ".")
			return 0;

		int iCurGrabCircularIndex = THEAPP.m_pDualCameraManager[iPcVisionNo]->GetGrabCircularIndex();

		strLog.Format("%s/ Auto setting Z scan start", sVisionCamType_Comm);
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		BOOL bGrabFail = FALSE;
		BOOL bGrabSuccess = TRUE;

		int iSeqAddrIndex = 0;
		int iNoSeqGrabImage = 1;
		int iNextSeqAddrIndex = 0;

		int iLotModuleIndex, iStageIndex;

		BOOL bSingleDualGrabMode = TRUE;
		////////////////////////////// 스캔 시작 ////////////////////////////////////

		bInspectionGrab = TRUE;

		double dLightZ, dHeadX, dStageY, dStageT, dStageR;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		DWORD dwZMoveStart = 0, dwZMoveEnd = 0;

		int iGrabCount = 0;

		// Change Grab Sequence Address
		iSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];
		THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iSeqAddrIndex, TRUE);

		// Json 파일 관련
		int iASFocusStep;
		double dASFocusMin, dASFocusMax, dASFocusPitch;

		CString FolderName = "c:\\AutoSettings\\";
		THEAPP.m_FileBase.CreateFolder(FolderName + sLotID);
		CString jsonFileName = FolderName + "json\\FocusSetting_CHS-K.json";
		std::ifstream file(jsonFileName);
		if (!file)
		{
			strLog.Format("%s/ Auto setting Z scan cancel(can't find json file)", sVisionCamType_Short);
			THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));

			THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
			if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
				THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
			THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

			THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");
			if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
				THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");

			return 0;
		}

		nlohmann::json JAutoSettingFocus;
		file >> JAutoSettingFocus;

		file.close();

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab)
				break;

			if (iGrabCount >= g_iVisionMaxGrabBuffer[iVisionCamType])
				break;

			// Json 파일 리드
			CString strCameraIndex;
			strCameraIndex.Format("%s%d-%d", sVisionCamType_Short, THEAPP.Struct_PreferenceStruct.m_iPCType + 1, iStageNo + 1);
			string strKey_Camera = CT2A(strCameraIndex);

			CString strImageIndex;
			strImageIndex.Format("%d", iNoCurImageGrab + 1);
			string strKey_Image = CT2A(strImageIndex);
			if (!JAutoSettingFocus[strKey_Camera][strKey_Image].contains("Z_Pitch"))
			{
				iNoCurImageGrab = iNoCurImageGrab + 1;
				continue;
			}

			// j.is_array() && j.size() == 2 && j[0].is_number() && j[1].is_number()
			dASFocusMin = round(JAutoSettingFocus[strKey_Camera][strKey_Image]["Z_Range"][0] * 100) / 100;
			dASFocusMax = round(JAutoSettingFocus[strKey_Camera][strKey_Image]["Z_Range"][1] * 100) / 100;
			dASFocusPitch = JAutoSettingFocus[strKey_Camera][strKey_Image]["Z_Pitch"];
			iASFocusStep = (dASFocusMax - dASFocusMin) / dASFocusPitch;

			dLightZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dLightZPosition[iStageNo][iNoCurImageGrab];
			dHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][iNoCurImageGrab];
			dStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][iNoCurImageGrab];
			dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
			dStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][iNoCurImageGrab];

			iNextSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];

			if (iSeqAddrIndex != iNextSeqAddrIndex)
				THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iNextSeqAddrIndex, TRUE);

			for (int iFocusSetIdx = 0; iFocusSetIdx <= iASFocusStep; iFocusSetIdx++)
			{
				double dCamZValue = dASFocusMin + dASFocusPitch * iFocusSetIdx;

				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iVisionCamType, iStageNo, iJigNo, dCamZValue, dLightZ, dHeadX, dStageY, dStageT, dStageR);
				pInspectService->SetCycleStopStatus(FALSE);
				pInspectService->SetReloadStatus(FALSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType]) // 무브 컴플리트 기다림
				{
					if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
					{
						Sleep(10);

						return 0;
					}

					Sleep(1);
				}

				bGrabSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraStartGrab(iFocusSetIdx, 1);
				// bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iVisionCamType, iFocusSetIdx, THEAPP.m_pModelDataManager->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab], 1, TRUE);

				if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "GB")
					MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iFocusSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iFocusSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
				else if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "BG")
					MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iFocusSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iFocusSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_BG);
				else
					MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iFocusSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iFocusSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);

				HObject m_pHZFocusImage;
				Compose3(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iFocusSetIdx][0], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iFocusSetIdx][1], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iFocusSetIdx][2], &m_pHZFocusImage);

				if (iStageNo == STAGE_NUMBER_2)
				{
					dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
					if (dStageT > 10)
						RotateImage(m_pHZFocusImage, &m_pHZFocusImage, 180, "nearest_neighbor");
				}

				CString FileName;
				FileName.Format("%s%s\\Module%d_%s%d_Stage%d_Image%02d_%s_ZFocus_%.4lf", FolderName, sLotID, iModuleNo, sVisionCamType_Short, THEAPP.Struct_PreferenceStruct.m_iPCType + 1, iStageNo + 1, iNoCurImageGrab + 1, THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1], dCamZValue);

				WriteImage(m_pHZFocusImage, "jpg", 0, HTuple(FileName));
			}

			iNoCurImageGrab = iNoCurImageGrab + 1;
		}

		strLog.Format("%s/ Auto setting Z scan done", sVisionCamType_Short);
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectService::InspectionThread_Inspect_AutoSettingZ] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");

		return 0;
	}
}

UINT InspectionThread_Inspect_AutoSettingL_Single(LPVOID lp)
{
	CString strLog;

	INSP_THREAD_PARAM* pThreadItem = (INSP_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	int iVisionCamType = pThreadItem->iVisionCamType;
	SAFE_DELETE(pThreadItem);

	int iPcVisionNo = VISION_NUMBER_1;

	if (iVisionCamType == VISION_NUMBER_1)
		iPcVisionNo = VISION_NUMBER_1;
	else if (iVisionCamType == VISION_NUMBER_2)
		iPcVisionNo = VISION_NUMBER_2;
	else if (iVisionCamType == VISION_NUMBER_3)
		iPcVisionNo = VISION_NUMBER_3;
	else if (iVisionCamType == VISION_NUMBER_4)
		iPcVisionNo = VISION_NUMBER_4;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int i;
	CString sLotID;
	int iMzNo, iJigNo, iTrayNo, iModuleNo;
	int iStageNo = STAGE_NUMBER_1;
	sLotID = pInspectService->m_sLotID_H[iVisionCamType];
	iMzNo = pInspectService->m_iMzNo_H[iVisionCamType];
	iJigNo = pInspectService->m_iJigNo_H[iVisionCamType];
	iTrayNo = pInspectService->m_iTrayNo_H[iVisionCamType];
	iModuleNo = pInspectService->m_iModuleNo_H[iVisionCamType];
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

#ifdef INLINE_MODE
	iStageNo = pInspectService->m_iStageNo_H[iVisionCamType];
#endif

	CString sVisionCamType_Comm;
	sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];

	BOOL bInspectionGrab = FALSE;

	int iTotalGrabRetry = 0;
	int iTotalFocusMoveRetry = 0;

	try
	{
		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_sModelName == ".")
			return 0;

		int iCurGrabCircularIndex = THEAPP.m_pDualCameraManager[iPcVisionNo]->GetGrabCircularIndex();

		strLog.Format("%s/ Auto setting L-Single scan start", sVisionCamType_Short);
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		BOOL bGrabFail = FALSE;
		BOOL bGrabSuccess = TRUE;

		int iNoSeqGrabImage = 1;
		int iNextSeqAddrIndex = 0;

		int iLotModuleIndex, iStageIndex;

		BOOL bSingleDualGrabMode = TRUE;
		////////////////////////////// 스캔 시작 ////////////////////////////////////

		bInspectionGrab = TRUE;

		double dPrevCamZ, dPrevLightZ, dPrevHeadX, dPrevStageY, dPrevStageT, dPrevStageR;
		dPrevCamZ = dPrevLightZ = dPrevHeadX = dPrevStageY = dPrevStageT = dPrevStageR = -100000000;
		double dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		DWORD dwZMoveStart = 0, dwZMoveEnd = 0;
		DWORD dwSeqChangeStart = 0, dwSeqChangeEnd = 0;

		int iGrabCount = 0;

		// Change Grab Sequence Address
		THEAPP.m_pTriggerManager->AutoLightSequenceSet(iVisionCamType, LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, 1);
		THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, TRUE);

		// Json 파일 관련
		int iASLightStep, iASLightMin, iASLightMax, iASLightPitch;

		CString FolderName = "c:\\AutoSettings\\";
		THEAPP.m_FileBase.CreateFolder(FolderName + sLotID);
		CString jsonFileName = FolderName + "json\\LightSetting_S_CHS-K.json";
		std::ifstream file(jsonFileName);
		if (!file)
		{
			strLog.Format("%s/ Auto setting L-Single scan cancel(can't find json file)", sVisionCamType_Short);
			THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));

			THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
			if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
				THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
			THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

			THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");
			if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
				THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");

			return 0;
		}

		nlohmann::json JAutoSettingLight;
		file >> JAutoSettingLight;

		file.close();

		// 조명 채널 관련
		CString strChNameFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LightChannel.ini";
		CIniFileCS INI_LIGHTCH(strChNameFileName);
		CString sSection, sKey, sLightChannelName;

		sSection = THEAPP.m_ModelDefineInfo.m_strVisionName[iPcVisionNo];

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab)
				break;

			dCamZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dZFocusPosition[iStageNo][iNoCurImageGrab];
			dLightZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dLightZPosition[iStageNo][iNoCurImageGrab];
			dHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][iNoCurImageGrab];
			dStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][iNoCurImageGrab];
			dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
			dStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][iNoCurImageGrab];

			if (fabs(dCamZ - dPrevCamZ) > Z_MOVE_OFFSET || fabs(dLightZ - dPrevLightZ) > Z_MOVE_OFFSET || fabs(dHeadX - dPrevHeadX) > Z_MOVE_OFFSET || fabs(dStageY - dPrevStageY) > Z_MOVE_OFFSET || fabs(dStageT - dPrevStageT) > Z_MOVE_OFFSET || fabs(dStageR - dPrevStageR) > Z_MOVE_OFFSET)
			{
				dPrevCamZ = dCamZ;
				dPrevLightZ = dLightZ;
				dPrevHeadX = dHeadX;
				dPrevStageY = dStageY;
				dPrevStageT = dStageT;
				dPrevStageR = dStageR;

				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iVisionCamType, iStageNo, iJigNo, dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR);
				pInspectService->SetCycleStopStatus(FALSE);
				pInspectService->SetReloadStatus(FALSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType]) // 무브 컴플리트 기다림
				{
					if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
					{
						Sleep(10);

						return 0;
					}

					Sleep(1);
				}
			}

			// Json 파일 리드
			CString strCameraIndex;
			strCameraIndex.Format("%s%d-%d", sVisionCamType_Short, THEAPP.Struct_PreferenceStruct.m_iPCType + 1, iStageNo + 1);
			string strKey_Camera = CT2A(strCameraIndex);

			CString strImageIndex;
			strImageIndex.Format("%d", iNoCurImageGrab + 1);
			string strKey_Image = CT2A(strImageIndex);

			if (!JAutoSettingLight[strKey_Camera].contains(strKey_Image))
			{
				iNoCurImageGrab = iNoCurImageGrab + 1;
				continue;
			}

			for (int iLIGHTCHIdx = 0; iLIGHTCHIdx < LIGHTCTRL_PAGE_COUNT; iLIGHTCHIdx++)
			{
				sKey.Format("%d", iLIGHTCHIdx + 1);
				sLightChannelName = INI_LIGHTCH.Get_String(sSection, sKey, "Unused");
				string strKey_LightChannelName = CT2A(sLightChannelName);

				if (!JAutoSettingLight[strKey_Camera][strKey_Image].contains(strKey_LightChannelName))
					continue;

				// j.is_array() && j.size() == 2 && j[0].is_number() && j[1].is_number()
				iASLightMin = JAutoSettingLight[strKey_Camera][strKey_Image][strKey_LightChannelName][0];
				iASLightMax = JAutoSettingLight[strKey_Camera][strKey_Image][strKey_LightChannelName][1];
				iASLightPitch = JAutoSettingLight[strKey_Camera][strKey_Image][strKey_LightChannelName][2];
				iASLightStep = (iASLightMax - iASLightMin) / iASLightPitch;

				for (int iLightSetIdx = 0; iLightSetIdx <= iASLightStep; iLightSetIdx++)
				{
					int iLightValue = iASLightMin + iASLightPitch * iLightSetIdx;
					if (iLightValue >= 1000)
						iLightValue = 999;
					if (iLIGHTCHIdx == 0)
						THEAPP.m_pTriggerManager->AutoLightPageSet_Single(iVisionCamType, 0, iLIGHTCHIdx, iLightValue, TRUE);
					else
						THEAPP.m_pTriggerManager->AutoLightPageSet_Single(iVisionCamType, 0, iLIGHTCHIdx, iLightValue, FALSE);

					bGrabSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraStartGrab(iLightSetIdx, 1);
					// bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iLightSetIdx, LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, 1, TRUE);

					if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "GB")
						MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iLightSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iLightSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
					else if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "BG")
						MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iLightSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iLightSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_BG);
					else
						MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iLightSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iLightSetIdx], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);

					HObject m_pHZFocusImage;
					Compose3(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iLightSetIdx][0], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iLightSetIdx][1], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iLightSetIdx][2], &m_pHZFocusImage);

					if (iStageNo == STAGE_NUMBER_2)
					{
						dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
						if (dStageT > 10)
							RotateImage(m_pHZFocusImage, &m_pHZFocusImage, 180, "nearest_neighbor");
					}

					CString FolderName = "c:\\AutoSettings\\";
					CString FileName;
					FileName.Format("%s%s\\Module%d_%s%d_Stage%d_Image%02d_%s_Light_CH%02d_%d", FolderName, sLotID, iModuleNo, sVisionCamType_Short, THEAPP.Struct_PreferenceStruct.m_iPCType + 1, iStageNo + 1, iNoCurImageGrab + 1, THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1], iLIGHTCHIdx + 1, iLightValue);

					WriteImage(m_pHZFocusImage, "jpg", 0, HTuple(FileName));
				}
			}

			iNoCurImageGrab = iNoCurImageGrab + 1;
		}

		strLog.Format("%s/ Auto setting L-Single scan done", sVisionCamType_Short);
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectService::InspectionThread_Inspect_AutoSettingL_Single] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");

		return 0;
	}
}

UINT InspectionThread_Inspect_AutoSettingL_Multi(LPVOID lp)
{
	CString strLog;

	INSP_THREAD_PARAM* pThreadItem = (INSP_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	int iVisionCamType = pThreadItem->iVisionCamType;
	SAFE_DELETE(pThreadItem);

	int iPcVisionNo = VISION_NUMBER_1;

	if (iVisionCamType == VISION_NUMBER_1)
		iPcVisionNo = VISION_NUMBER_1;
	else if (iVisionCamType == VISION_NUMBER_2)
		iPcVisionNo = VISION_NUMBER_2;
	else if (iVisionCamType == VISION_NUMBER_3)
		iPcVisionNo = VISION_NUMBER_3;
	else if (iVisionCamType == VISION_NUMBER_4)
		iPcVisionNo = VISION_NUMBER_4;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int i;
	CString sLotID;
	int iMzNo, iJigNo, iTrayNo, iModuleNo;
	int iStageNo = STAGE_NUMBER_1;
	sLotID = pInspectService->m_sLotID_H[iVisionCamType];
	iMzNo = pInspectService->m_iMzNo_H[iVisionCamType];
	iJigNo = pInspectService->m_iJigNo_H[iVisionCamType];
	iTrayNo = pInspectService->m_iTrayNo_H[iVisionCamType];
	iModuleNo = pInspectService->m_iModuleNo_H[iVisionCamType];
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

#ifdef INLINE_MODE
	iStageNo = pInspectService->m_iStageNo_H[iVisionCamType];
#endif

	CString sVisionCamType_Comm;
	sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];

	BOOL bInspectionGrab = FALSE;

	int iTotalGrabRetry = 0;
	int iTotalFocusMoveRetry = 0;

	try
	{
		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_sModelName == ".")
			return 0;

		int iCurGrabCircularIndex = THEAPP.m_pDualCameraManager[iPcVisionNo]->GetGrabCircularIndex();

		strLog.Format("%s/ Auto setting L-Multi scan start", sVisionCamType_Short);
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		BOOL bGrabFail = FALSE;
		BOOL bGrabSuccess = TRUE;

		int iNoSeqGrabImage = 1;
		int iNextSeqAddrIndex = 0;

		int iLotModuleIndex, iStageIndex;

		BOOL bSingleDualGrabMode = TRUE;
		////////////////////////////// 스캔 시작 ////////////////////////////////////

		bInspectionGrab = TRUE;

		double dPrevCamZ, dPrevLightZ, dPrevHeadX, dPrevStageY, dPrevStageT, dPrevStageR;
		dPrevCamZ = dPrevLightZ = dPrevHeadX = dPrevStageY = dPrevStageT = dPrevStageR = -100000000;
		double dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		DWORD dwZMoveStart = 0, dwZMoveEnd = 0;
		DWORD dwSeqChangeStart = 0, dwSeqChangeEnd = 0;

		int iGrabCount = 0;

		// Change Grab Sequence Address
		THEAPP.m_pTriggerManager->AutoLightSequenceSet(iVisionCamType, LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, 1);
		THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, TRUE);

		// Json 파일 관련
		int iASLightStep, iASLightMin, iASLightMax, iASLightPitch;

		CString FolderName = "c:\\AutoSettings\\";
		THEAPP.m_FileBase.CreateFolder(FolderName + sLotID);
		CString jsonFileName = FolderName + "json\\LightSetting_M_CHS-K.json";
		std::ifstream file(jsonFileName);
		if (!file)
		{
			strLog.Format("%s/ Auto setting L-Multi scan cancel(can't find json file)", sVisionCamType_Short);
			THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));

			THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
			if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
				THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
			THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

			THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");
			if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
				THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");

			return 0;
		}

		nlohmann::json JAutoSettingLight;
		file >> JAutoSettingLight;

		file.close();

		// 조명 채널 관련
		CString strChNameFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LightChannel.ini";
		CIniFileCS INI_LIGHTCH(strChNameFileName);
		CString sSection, sKey, sLightChannelName;

		sSection = THEAPP.m_ModelDefineInfo.m_strVisionName[iPcVisionNo];

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab)
				break;

			dCamZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dZFocusPosition[iStageNo][iNoCurImageGrab];
			dLightZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dLightZPosition[iStageNo][iNoCurImageGrab];
			dHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][iNoCurImageGrab];
			dStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][iNoCurImageGrab];
			dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
			dStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][iNoCurImageGrab];

			if (fabs(dCamZ - dPrevCamZ) > Z_MOVE_OFFSET || fabs(dLightZ - dPrevLightZ) > Z_MOVE_OFFSET || fabs(dHeadX - dPrevHeadX) > Z_MOVE_OFFSET || fabs(dStageY - dPrevStageY) > Z_MOVE_OFFSET || fabs(dStageT - dPrevStageT) > Z_MOVE_OFFSET || fabs(dStageR - dPrevStageR) > Z_MOVE_OFFSET)
			{
				dPrevCamZ = dCamZ;
				dPrevLightZ = dLightZ;
				dPrevHeadX = dHeadX;
				dPrevStageY = dStageY;
				dPrevStageT = dStageT;
				dPrevStageR = dStageR;

				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iVisionCamType, iStageNo, iJigNo, dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR);
				pInspectService->SetCycleStopStatus(FALSE);
				pInspectService->SetReloadStatus(FALSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType]) // 무브 컴플리트 기다림
				{
					if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
					{
						Sleep(10);

						return 0;
					}

					Sleep(1);
				}
			}

			// Json 파일 리드
			CString strCameraIndex;
			strCameraIndex.Format("%s%d-%d", sVisionCamType_Short, THEAPP.Struct_PreferenceStruct.m_iPCType + 1, iStageNo + 1);
			string strKey_Camera = CT2A(strCameraIndex);

			CString strImageIndex;
			strImageIndex.Format("%d", iNoCurImageGrab + 1);
			string strKey_Image = CT2A(strImageIndex);
			if (!JAutoSettingLight[strKey_Camera].contains(strKey_Image))
			{
				iNoCurImageGrab = iNoCurImageGrab + 1;
				continue;
			}

			for (int iCaseNo = 0; iCaseNo < 20; iCaseNo++)
			{
				CString strCaseNo;
				strCaseNo.Format("case%d", iCaseNo + 1);
				string strKey_CaseNo = CT2A(strCaseNo);
				if (!JAutoSettingLight[strKey_Camera][strKey_Image].contains(strKey_CaseNo))
					break;

				std::vector<std::pair<int, int>> vLightGroup;
				for (int iLIGHTCHIdx = 0; iLIGHTCHIdx < LIGHTCTRL_PAGE_COUNT; iLIGHTCHIdx++)
				{
					sKey.Format("%d", iLIGHTCHIdx + 1);
					sLightChannelName = INI_LIGHTCH.Get_String(sSection, sKey, "Unused");
					string strKey_LightChannelName = CT2A(sLightChannelName);

					int iLightValue = 0;
					if (JAutoSettingLight[strKey_Camera][strKey_Image][strKey_CaseNo].contains(strKey_LightChannelName))
					{
						iLightValue = JAutoSettingLight[strKey_Camera][strKey_Image][strKey_CaseNo][strKey_LightChannelName];
						if (iLightValue >= 1000)
							iLightValue = 999;

						vLightGroup.push_back({ iLIGHTCHIdx, iLightValue });
					}
				}

				THEAPP.m_pTriggerManager->AutoLightPageSet_Multi(0, 0, vLightGroup);

				bGrabSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraStartGrab(iCaseNo, 1);

				if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "GB")
					MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iCaseNo], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iCaseNo], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
				else if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "BG")
					MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iCaseNo], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iCaseNo], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_BG);
				else
					MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iCaseNo], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iCaseNo], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);

				HObject m_pHZFocusImage;
				Compose3(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iCaseNo][0], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iCaseNo][1], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iCaseNo][2], &m_pHZFocusImage);

				if (iStageNo == STAGE_NUMBER_2)
				{
					dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
					if (dStageT > 10)
						RotateImage(m_pHZFocusImage, &m_pHZFocusImage, 180, "nearest_neighbor");
				}

				CString FolderName = "c:\\AutoSettings\\";
				CString FileName;
				FileName.Format("%s%s\\Module%d_%s%d_Stage%d_Image%02d_%s_Light_Case%02d", FolderName, sLotID, iModuleNo, sVisionCamType_Short, THEAPP.Struct_PreferenceStruct.m_iPCType + 1, iStageNo + 1, iNoCurImageGrab + 1, THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1], iCaseNo + 1);

				WriteImage(m_pHZFocusImage, "jpg", 0, HTuple(FileName));
			}

			iNoCurImageGrab = iNoCurImageGrab + 1;
		}

		strLog.Format("%s/ Auto setting L-Multi scan done", sVisionCamType_Short);
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");

		return 0;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectService::InspectionThread_Inspect_AutoSettingL_Multi] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, "G", "G");

		return 0;
	}
}

UINT AutoFocusThread(LPVOID lp)
{
	CString strLog;

	MODULE_POSITION_THREAD_PARAM* pThreadItem = (MODULE_POSITION_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	HObject *pHModulePosCheckColorImage = pThreadItem->pHImage;
	int iMzNo = pThreadItem->iMzNo;
	int iPcVisionNo = pThreadItem->iPcVision;
	int iStageNo = pThreadItem->iStageNo;
	int iVisionCamType = pThreadItem->iVisionCamType;
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
	SAFE_DELETE(pThreadItem);

	CString sVisionCamType_Comm;
	sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];

	HObject HGrayImage;
	Rgb1ToGray(*pHModulePosCheckColorImage, &HGrayImage);

	try
	{
		auto log_time_start = std::chrono::high_resolution_clock::now();

		DPOINT dNorthLineStartPoint, dNorthLineEndPoint;
		DPOINT dWestLineStartPoint, dWestLineEndPoint;

		dNorthLineStartPoint.x = dNorthLineStartPoint.y = dNorthLineEndPoint.x = dNorthLineEndPoint.y = -1;
		dWestLineStartPoint.x = dWestLineStartPoint.y = dWestLineEndPoint.x = dWestLineEndPoint.y = -1;

		int iImageIdx = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfModuleRotationAngleImageNo;
		int iTabIdx = 0;
		int iCurInspectionBufferIdx = TEACHING_INSPECT_BUFFER_INDEX;
		int iCurThreadIdx = TEACHING_THREAD_INDEX;

		GTRegion *pInspectROIRgn, *pAlignROIRgn;
		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HAlignRgn;

		BOOL bTempROIAlignShiftResult = FALSE;
		int iLoaclAlignDeltaX, iLoaclAlignDeltaY;
		double dLoaclAlignDeltaAngle, dLoaclAlignDeltaAngleFixedPointX, dLoaclAlignDeltaAngleFixedPointY;
		iLoaclAlignDeltaX = iLoaclAlignDeltaY = INVALID_ALIGN_RESULT;
		dLoaclAlignDeltaAngle = INVALID_ALIGN_RESULT;

		int iMatchingPosOffsetX, iMatchingPosOffsetY;
		iMatchingPosOffsetX = iMatchingPosOffsetY = 0;

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();

		BOOL bAlignFound = FALSE;

		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			pAlignROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);
			if (pAlignROIRgn == NULL)
				continue;

			if (pAlignROIRgn->miTeachImageIndex != iImageIdx)
				continue;

			if (pAlignROIRgn->miInspectionType != INSPECTION_TYPE_AF_ALIGN)
				continue;

			if (pAlignROIRgn->m_AlgorithmParam[2].m_bInspect == TRUE)
			{
				pAlignROIRgn->ResetLocalAlignResult(iCurThreadIdx);

				HROIHRegion = pAlignROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

				if (pAlignROIRgn->m_AlgorithmParam[2].m_bUsePartCheck)
				{
					THEAPP.m_pAlgorithm->InspectAVI(-1, iCurThreadIdx, pAlignROIRgn, 2, HGrayImage, HROIHRegion, pAlignROIRgn->m_AlgorithmParam[2], &HResultXLD, pHModulePosCheckColorImage, HGrayImage);

					bAlignFound = TRUE;
				}

				break;
			}
		}

		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
				continue;

			if (((pInspectROIRgn->miInspectionType == INSPECTION_TYPE_AF_NORTH_LINE) ||
				(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_AF_WEST_LINE)) == FALSE)
				continue;

			if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect == TRUE)
			{
				HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

				if (bAlignFound)
				{
					HTuple HomMat2DLocal;
					double dDeltaX, dDeltaY, dDeltaAngle;
					dDeltaX = dDeltaY = dDeltaAngle = 0;

					if (pAlignROIRgn->m_iLocalAlignDeltaX[iCurThreadIdx] != INVALID_ALIGN_RESULT)
						dDeltaX = pAlignROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iCurThreadIdx] + pAlignROIRgn->m_iLocalAlignDeltaX[iCurThreadIdx];
					else
						dDeltaX = pAlignROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iCurThreadIdx];

					if (pAlignROIRgn->m_iLocalAlignDeltaY[iCurThreadIdx] != INVALID_ALIGN_RESULT)
						dDeltaY = pAlignROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iCurThreadIdx] + pAlignROIRgn->m_iLocalAlignDeltaY[iCurThreadIdx];
					else
						dDeltaY = pAlignROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iCurThreadIdx];

					if (pAlignROIRgn->m_dLocalAlignDeltaAngle[iCurThreadIdx] != INVALID_ALIGN_RESULT)
						dDeltaAngle = pAlignROIRgn->m_dLocalAlignDeltaAngle[iCurThreadIdx];

					VectorAngleToRigid(pAlignROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iCurThreadIdx], pAlignROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iCurThreadIdx], 0, dDeltaY, dDeltaX, dDeltaAngle, &HomMat2DLocal);
					AffineTransRegion(HROIHRegion, &HROIHRegion, HomMat2DLocal, "nearest_neighbor");
				}

				THEAPP.m_pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, HGrayImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, pHModulePosCheckColorImage, HGrayImage);

				if (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
				{
					if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_AF_NORTH_LINE)
					{
						dNorthLineStartPoint.x = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX;
						dNorthLineStartPoint.y = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY;
						dNorthLineEndPoint.x = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX;
						dNorthLineEndPoint.y = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY;
					}
					else if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_AF_WEST_LINE)
					{
						dWestLineStartPoint.x = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX;
						dWestLineStartPoint.y = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY;
						dWestLineEndPoint.x = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX;
						dWestLineEndPoint.y = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY;
					}
				}
			}
		}

		HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
		DPOINT dIntersectPoint;
		HTuple HAngle, HAngleDeg;
		HTuple HTransX, HTransY;
		int iTransX, iTransY;

		if (dNorthLineStartPoint.x >= 0 && dNorthLineEndPoint.x >= 0 && dWestLineStartPoint.x >= 0 && dWestLineEndPoint.x >= 0)
		{
			IntersectionLines(dNorthLineStartPoint.y, dNorthLineStartPoint.x, dNorthLineEndPoint.y, dNorthLineEndPoint.x, dWestLineStartPoint.y, dWestLineStartPoint.x, dWestLineEndPoint.y, dWestLineEndPoint.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);
			dIntersectPoint.x = HIntersectPointX[0].D();
			dIntersectPoint.y = HIntersectPointY[0].D();

			AngleLl(dNorthLineStartPoint.y, dNorthLineStartPoint.x, dNorthLineStartPoint.y, dNorthLineEndPoint.x, dNorthLineStartPoint.y, dNorthLineStartPoint.x, dNorthLineEndPoint.y, dNorthLineEndPoint.x, &HAngle);

			TupleDeg(HAngle, &HAngleDeg);
			pInspectService->m_dAutoFocusModuleRotationAngle[iPcVisionNo] = HAngleDeg[0].D();

			HTuple HomMat2DIdentity, HomMat2DRotate;
			HomMat2dIdentity(&HomMat2DIdentity);
			HomMat2dRotate(HomMat2DIdentity, -HAngle, THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dAfZigRotationCenterY[iStageNo], THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dAfZigRotationCenterX[iStageNo], &HomMat2DRotate);

			AffineTransPixel(HomMat2DRotate, dIntersectPoint.y, dIntersectPoint.x, &HTransY, &HTransX);

			iTransX = (int)HTransX[0].D();
			iTransY = (int)HTransY[0].D();

			pInspectService->m_dAutoFocusDeltaX[iPcVisionNo] = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfLeftTopCornerPointX[iStageNo] - iTransX;
			pInspectService->m_dAutoFocusDeltaY[iPcVisionNo] = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfLeftTopCornerPointY[iStageNo] - iTransY;

			pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] = TOP_MODULE_POS_CHECK_RESULT_GOOD;
		}
		else
		{
			pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] = TOP_MODULE_POS_CHECK_RESULT_NG;
		}

		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("%s/ Auto focus done, Time(ms): %d, Result: %d, Rotation(deg): %.0lf, DeltaX(pixel): %d, DeltaY(pixel): %d", sVisionCamType_Comm, log_time_ms, pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo], pInspectService->m_dAutoFocusModuleRotationAngle[iPcVisionNo], (int)pInspectService->m_dAutoFocusDeltaX[iPcVisionNo], (int)pInspectService->m_dAutoFocusDeltaY[iPcVisionNo]);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		return 1;
	}
	catch (HException &except)
	{
		pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] = TOP_MODULE_POS_CHECK_RESULT_NG;

		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [InspectService::AutoFocusThread] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}
}

UINT InspectionThread_AutoFocus_Inspect(LPVOID lp)
{
	CString strLog;

	INSP_THREAD_PARAM* pThreadItem = (INSP_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	int iVisionCamType = pThreadItem->iVisionCamType;
	SAFE_DELETE(pThreadItem);

	int iPcVisionNo = VISION_NUMBER_1;

	if (iVisionCamType == VISION_NUMBER_1)
		iPcVisionNo = VISION_NUMBER_1;
	else if (iVisionCamType == VISION_NUMBER_2)
		iPcVisionNo = VISION_NUMBER_2;
	else if (iVisionCamType == VISION_NUMBER_3)
		iPcVisionNo = VISION_NUMBER_3;
	else if (iVisionCamType == VISION_NUMBER_4)
		iPcVisionNo = VISION_NUMBER_4;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int i;
	CString sLotID;
	int iMzNo, iJigNo, iTrayNo, iModuleNo;
	int iStageNo = STAGE_NUMBER_1;
	double dLensHeightDev;
	sLotID = pInspectService->m_sLotID_H[iVisionCamType];
	iMzNo = pInspectService->m_iMzNo_H[iVisionCamType];
	iJigNo = pInspectService->m_iJigNo_H[iVisionCamType];
	iTrayNo = pInspectService->m_iTrayNo_H[iVisionCamType];
	iModuleNo = pInspectService->m_iModuleNo_H[iVisionCamType];
	dLensHeightDev = pInspectService->m_dHeight_H[iVisionCamType];
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

#ifdef INLINE_MODE
	iStageNo = pInspectService->m_iStageNo_H[iVisionCamType];
#endif

	CString sVisionCamType_Comm;
	sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];

	BOOL bInspectionGrab = FALSE;

	int iTotalGrabRetry = 0;
	int iTotalFocusMoveRetry = 0;

	try
	{
		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_sModelName == ".")
			return 0;

		int iCurGrabCircularIndex = THEAPP.m_pDualCameraManager[iPcVisionNo]->GetGrabCircularIndex();

		strLog.Format("%s/ Scan start, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iCurGrabCircularIndex);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		auto total_log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		BOOL bGrabFail = FALSE;
		BOOL bGrabSuccess = TRUE;

		MIL_DOUBLE tMilGrabStart = 0, tMilGrabEnd = 0;

		int iSeqAddrIndex = 0;
		int iNoSeqGrabImage = 1;
		int iNextSeqAddrIndex = 0;

		int iLotModuleIndex, iStageIndex;

		BOOL bSingleDualGrabMode = TRUE;
		////////////////////////////// 스캔 시작 ////////////////////////////////////

#ifdef INLINE_MODE

		bInspectionGrab = TRUE;

		double dPrevCamZ, dPrevLightZ, dPrevHeadX, dPrevStageY, dPrevStageT, dPrevStageR;
		dPrevCamZ = dPrevLightZ = dPrevHeadX = dPrevStageY = dPrevStageT = dPrevStageR = -100000000;
		double dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		DWORD dwZMoveStart = 0, dwZMoveEnd = 0;
		DWORD dwSeqChangeStart = 0, dwSeqChangeEnd = 0;
		BOOL bFocusMoveSuccess = FALSE;

		int iGrabCount = 0;

		int iWSSDelayTime = 0;
		BOOL bAddWSSPostDelay = TRUE;

		// Auto Focus
		double dDeltaCamZ = 0;
		double dDeltaAngleDeg = 0;
		BOOL bModuleRotationCheck = FALSE;
		HObject HModulePositionCheckImage;

		// Change Grab Sequence Address
		iSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];
		THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iSeqAddrIndex);

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab)
				break;

			if (iGrabCount >= g_iVisionMaxGrabBuffer[iVisionCamType])
				break;

			dDeltaCamZ = 0;
			dDeltaAngleDeg = 0;

			if ((iNoCurImageGrab + 1) >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfLensStartImageNo && (iNoCurImageGrab + 1) <= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfLensEndImageNo)
			{
				dDeltaCamZ = dLensHeightDev;

				if (fabs(dDeltaCamZ) > AF_MAX_DEVIATION_MM)
					dDeltaCamZ = 0;
			}

			if (bModuleRotationCheck)
			{
				if ((iNoCurImageGrab + 1) >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfEastSideStartImageNo && (iNoCurImageGrab + 1) <= (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfEastSideEndImageNo))
				{
					while (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_NOT_READY)
					{
						Sleep(1);

						if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
						{
							Sleep(10);

							return 0;
						}
					}

					if (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_RESULT_GOOD)
					{
						dDeltaCamZ = pInspectService->m_dAutoFocusDeltaX[iPcVisionNo] * 0.005;

						strLog.Format("%s/ Auto focus, Direction: East, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, DeltaZ(mm): %.3lf", sVisionCamType_Comm, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, dDeltaCamZ);
						THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

						if (fabs(dDeltaCamZ) > AF_MAX_DEVIATION_MM)
							dDeltaCamZ = 0;

						dDeltaAngleDeg = pInspectService->m_dAutoFocusModuleRotationAngle[iPcVisionNo];
					}
				}
				else if ((iNoCurImageGrab + 1) >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfWestSideStartImageNo && (iNoCurImageGrab + 1) <= (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfWestSideEndImageNo))
				{
					while (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_NOT_READY)
					{
						Sleep(1);

						if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
						{
							Sleep(10);

							return 0;
						}
					}

					if (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_RESULT_GOOD)
					{
						dDeltaCamZ = -pInspectService->m_dAutoFocusDeltaX[iPcVisionNo] * 0.005;

						strLog.Format("%s/ Auto focus, Direction: West, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, DeltaZ(mm): %.3lf", sVisionCamType_Comm, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, dDeltaCamZ);
						THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

						if (fabs(dDeltaCamZ) > AF_MAX_DEVIATION_MM)
							dDeltaCamZ = 0;

						dDeltaAngleDeg = pInspectService->m_dAutoFocusModuleRotationAngle[iPcVisionNo];
					}
				}
				else if ((iNoCurImageGrab + 1) >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfSouthSideStartImageNo && (iNoCurImageGrab + 1) <= (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfSouthSideEndImageNo))
				{
					while (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_NOT_READY)
					{
						Sleep(1);

						if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
						{
							Sleep(10);

							return 0;
						}
					}

					if (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_RESULT_GOOD)
					{
						dDeltaCamZ = pInspectService->m_dAutoFocusDeltaY[iPcVisionNo] * 0.005;

						strLog.Format("%s/ Auto focus, Direction: South, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, DeltaZ(mm): %.3lf", sVisionCamType_Comm, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, dDeltaCamZ);
						THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

						if (fabs(dDeltaCamZ) > AF_MAX_DEVIATION_MM)
							dDeltaCamZ = 0;

						dDeltaAngleDeg = pInspectService->m_dAutoFocusModuleRotationAngle[iPcVisionNo];
					}
				}
				else if ((iNoCurImageGrab + 1) >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfNorthSideStartImageNo && (iNoCurImageGrab + 1) <= (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfNorthSideEndImageNo))
				{
					while (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_NOT_READY)
					{
						Sleep(1);

						if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
						{
							Sleep(10);

							return 0;
						}
					}

					if (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_RESULT_GOOD)
					{
						dDeltaCamZ = -pInspectService->m_dAutoFocusDeltaY[iPcVisionNo] * 0.005;

						strLog.Format("%s/ Auto focus, Direction: North, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, DeltaZ(mm): %.3lf", sVisionCamType_Comm, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, dDeltaCamZ);
						THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

						if (fabs(dDeltaCamZ) > AF_MAX_DEVIATION_MM)
							dDeltaCamZ = 0;

						dDeltaAngleDeg = pInspectService->m_dAutoFocusModuleRotationAngle[iPcVisionNo];
					}
				}

			}

			double tGrabStart = 0, tGrabEnd = 0;

			dCamZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dZFocusPosition[iStageNo][iNoCurImageGrab];
			dLightZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dLightZPosition[iStageNo][iNoCurImageGrab];
			dHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][iNoCurImageGrab];
			dStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][iNoCurImageGrab];
			dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
			dStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][iNoCurImageGrab];

			dwSeqChangeStart = GetTickCount();

			if (fabs(dCamZ - dPrevCamZ) > Z_MOVE_OFFSET || fabs(dLightZ - dPrevLightZ) > Z_MOVE_OFFSET || fabs(dHeadX - dPrevHeadX) > Z_MOVE_OFFSET || fabs(dStageY - dPrevStageY) > Z_MOVE_OFFSET || fabs(dStageT - dPrevStageT) > Z_MOVE_OFFSET || fabs(dStageR - dPrevStageR) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iVisionCamType, iStageNo, iJigNo, dCamZ + dDeltaCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR + dDeltaAngleDeg);
				pInspectService->SetCycleStopStatus(FALSE);
				pInspectService->SetReloadStatus(FALSE);

				iNextSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];

				if (iSeqAddrIndex != iNextSeqAddrIndex)
				{
					THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iNextSeqAddrIndex, TRUE);
					bAddWSSPostDelay = TRUE;
				}

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType]) // 무브 컴플리트 기다림
				{
					if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
					{
						Sleep(10);

						return 0;
					}

					Sleep(1);
				}

				dPrevCamZ = dCamZ;
				dPrevLightZ = dLightZ;
				dPrevHeadX = dHeadX;
				dPrevStageY = dStageY;
				dPrevStageT = dStageT;
				dPrevStageR = dStageR;
			}
			else
			{
				iNextSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];

				if (iSeqAddrIndex != iNextSeqAddrIndex)
				{
					THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iNextSeqAddrIndex, TRUE, TRUE);
					bAddWSSPostDelay = TRUE;
				}
			}

			iRetryCnt = 0;
			int K = 0;

			bGrabSuccess = FALSE;

			iSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];
			iNoSeqGrabImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iAddrCount[iSeqAddrIndex];
			iWSSDelayTime = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iWSSDelayTime[iSeqAddrIndex];

			if (bAddWSSPostDelay && iWSSDelayTime > 0)
				Sleep(iWSSDelayTime);
			bAddWSSPostDelay = FALSE;

#ifdef USE_MATROX_TIMER
			MappTimer(M_DEFAULT, M_TIMER_READ + M_SYNCHRONOUS, &tMilGrabStart);
#else
			log_time_start = std::chrono::high_resolution_clock::now();
#endif

			for (K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				if (K > 0)
					++iTotalGrabRetry;

				bGrabSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraStartGrab(iGrabCount, iNoSeqGrabImage);

				if (bGrabSuccess)
					break;
			}	// Grab Retry

			if (bGrabSuccess == FALSE)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

#ifdef USE_MATROX_TIMER
			MappTimer(M_DEFAULT, M_TIMER_READ + M_SYNCHRONOUS, &tMilGrabEnd);
			strLog.Format("%s/ Grab, Time(ms): %.0lf, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, Grab count: %d", sVisionCamType_Comm, (tMilGrabEnd - tMilGrabStart) * 1000.0, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iRetryCnt);
#else
			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();
			strLog.Format("%s/ Grab, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, Grab count: %d", sVisionCamType_Comm, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iRetryCnt);
#endif
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

			if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularImage[iNoCurImageGrab] == TRUE)
			{
				iNoCurImageGrab += DFM_USED_CONV_IMAGE_NUMBER;
				iGrabCount += SPV_RAW_IMAGE_NUMBER;
			}
			else if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseDiffusedImage[iNoCurImageGrab] == TRUE)
			{
				iNoCurImageGrab += DIFFUSED_USED_CONV_IMAGE_NUMBER;
				iGrabCount += DIFFUSED_RAW_IMAGE_NUMBER;
			}
			else
			{
				iNoCurImageGrab += iNoSeqGrabImage;
				iGrabCount += iNoSeqGrabImage;
			}

			if (bModuleRotationCheck == FALSE && THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfModuleRotationAngleImageNo <= iNoCurImageGrab)
			{
				int iDiff;
				iDiff = iNoCurImageGrab - THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfModuleRotationAngleImageNo;

				if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "GB")
					MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
				else if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "BG")
					MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_BG);
				else
					MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);

				HObject HTempGrabImage;

				Compose3(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iGrabCount - 1 - iDiff][0],
					THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iGrabCount - 1 - iDiff][1],
					THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iGrabCount - 1 - iDiff][2],
					&HTempGrabImage);

				CopyImage(HTempGrabImage, &HModulePositionCheckImage);

				MODULE_POSITION_THREAD_PARAM* pModulePosParam = new MODULE_POSITION_THREAD_PARAM(&HModulePositionCheckImage, pInspectService, iMzNo, iPcVisionNo, iStageNo, iVisionCamType);
				AfxBeginThread(AutoFocusThread, LPVOID(pModulePosParam));

				bModuleRotationCheck = TRUE;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		if (bGrabFail == TRUE)
		{
			strLog.Format("%s/ Grab fail(No signal), Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));
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

					return 0;
				}
			}

			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("%s/ Scan complete holding, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Comm, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));
		}

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		auto total_log_time_end = std::chrono::high_resolution_clock::now();
		auto total_log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(total_log_time_end - total_log_time_start).count();
		strLog.Format("%s/ Scan done, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d", sVisionCamType_Short, total_log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iCurGrabCircularIndex);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		for (i = 0; i < iGrabCount; i++)
		{
			if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
		}

#else	// Offline Mode =>

		HObject HInspectImage[MAX_IMAGE_TAB][3];

		for (int iii = 0; iii < MAX_IMAGE_TAB; iii++)
		{
			for (int jjj = 0; jjj < 3; jjj++)
				GenEmptyObj(&(HInspectImage[iii][jjj]));
		}

		CString FolderName, ImageName;

		if (THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath != "")
			FolderName.Format(THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath + "\\");
		else
		{
			AfxMessageBox("디버깅 영상 폴더가 지정되지 않았습니다.", MB_SYSTEMMODAL);
			return 0;
		}

		bGrabFail = TRUE;

		char chSep = '_';
		CString sFileBarcode = _T("NOREAD");

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

				int iNoInspectImage = 0;
				iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

				int iNoImageYDir, iModular;
				int iCropImageSizeX, iCropImageSizeY;

				iNoImageYDir = iNoInspectImage / MAX_COMBINE_IMAGE_NUMBER;
				iModular = iNoInspectImage % MAX_COMBINE_IMAGE_NUMBER;
				if (iModular > 0)
					iNoImageYDir += 1;

				ImageName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, iNoInspectImage);
				sReadFileName = FolderName + ImageName;

				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);

					FindClose(hFindFile);

					ReadImage(&HCombineImage, (HTuple)strRawImageFileFullName);

					AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);

					if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
						sFileBarcode.TrimRight(".bmp");
					else
						sFileBarcode.TrimRight(".jpg");

					EnterCriticalSection(&THEAPP.m_csBarcodeRead);

					THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;

					LeaveCriticalSection(&THEAPP.m_csBarcodeRead);

					if (THEAPP.m_pGFunction->ValidHImage(HCombineImage) == TRUE)
					{
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImageSize(HCombineImage, &htImageWidth, &htImageHeight);
						int wd = (int)htImageWidth.L();
						int ht = (int)htImageHeight.L();

						iCropImageSizeX = wd / MAX_COMBINE_IMAGE_NUMBER;
						iCropImageSizeY = ht / iNoImageYDir;

						POINT CropLTPoint, CropRBPoint;
						int iImageIndexX, iImageIndexY;
						HObject HCropImage;

						for (i = 0; i < iNoInspectImage; i++)
						{
							iImageIndexX = i % MAX_COMBINE_IMAGE_NUMBER;
							iImageIndexY = i / MAX_COMBINE_IMAGE_NUMBER;

							CropLTPoint.x = iImageIndexX * iCropImageSizeX;
							CropRBPoint.x = iImageIndexX * iCropImageSizeX + iCropImageSizeX - 1;
							CropLTPoint.y = iImageIndexY * iCropImageSizeY;
							CropRBPoint.y = iImageIndexY * iCropImageSizeY + iCropImageSizeY - 1;

							CropRectangle1(HCombineImage, &HColorImage, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

							if (THEAPP.m_pGFunction->ValidHImage(HColorImage))
							{
								HTuple htImageHeight = 0;
								HTuple htImageWidth = 0;
								HalconCpp::GetImageSize(HColorImage, &htImageWidth, &htImageHeight);

								if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
									ZoomImageSize(HColorImage, &HColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");

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
				HTuple HExp;
				HTuple HOperatorName, HErrMsg;
				CString sOperatorName, sErrMsg;
				except.ToHTuple(&HExp);
				except.GetExceptionData(HExp, "operator", &HOperatorName);
				except.GetExceptionData(HExp, "error_message", &HErrMsg);
				sOperatorName = HOperatorName.S();
				sErrMsg = HErrMsg.S();

				strLog.Format("Halcon Exception [InspectService::InspectionThread_AutoFocus_Inspect Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
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

				for (int i = 0; i < THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab; i++)
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
						HTuple htImgPtrR, htImgPtrG, htImgPtrB;
						HTuple htType = 0;
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImagePointer3(HColorImage, &htImgPtrR, &htImgPtrG, &htImgPtrB, &htType, &htImageWidth, &htImageHeight);

						if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
							ZoomImageSize(HColorImage, &HColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");
					}
					catch (HException &except)
					{
						;
					}

					if (i == (THEAPP.m_nOfflineBarcodeImageNo - 1))		// default: if (i == 0)
					{
						AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);

						if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
							sFileBarcode.TrimRight(".bmp");
						else
							sFileBarcode.TrimRight(".jpg");

						EnterCriticalSection(&THEAPP.m_csBarcodeRead);

						THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;

						LeaveCriticalSection(&THEAPP.m_csBarcodeRead);
					}

					Decompose3(HColorImage, &(HInspectImage[i][0]), &(HInspectImage[i][1]), &(HInspectImage[i][2]));
				}

				if (bFileFindFail == FALSE)
					bGrabFail = FALSE;
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

				strLog.Format("Halcon Exception [InspectService::InspectionThread_AutoFocus_Inspect Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
				THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
			}
		}

		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

#endif	// Offline Mode

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		BOOL bCopyRet;
		int i;

#ifdef INLINE_MODE
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

		if (bGrabFail)
		{
			if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
			{
				THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
			}
		}

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = iStageNo;
		param.iJigNo = iJigNo;
		param.sTrayID = _T("NoUse");
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.iCurCircularGrabIdx = iCurGrabCircularIndex;
		param.iNoGrabRetry = iTotalGrabRetry;
		param.iNoFocusMoveRetry = iTotalFocusMoveRetry;
		param.copyMode = AlgorithmCopyParam::DFM;
		param.ppGrabHImage = THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex];
		param.bColor = TRUE;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
			{
				THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
			}

			param.bGrabFail = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}
#else
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = iStageNo;
		param.iJigNo = iJigNo;
		param.sTrayID = _T("NoUse");
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.iCurCircularGrabIdx = iCurGrabCircularIndex;
		param.iNoGrabRetry = iTotalGrabRetry;
		param.iNoFocusMoveRetry = iTotalFocusMoveRetry;
		param.copyMode = AlgorithmCopyParam::NORMAL;
		param.ppGrabHImage = HInspectImage;
		param.bColor = TRUE;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			param.bGrabFail = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}
#endif

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bGrabCircularBufferCopyDone[iCurGrabCircularIndex] = TRUE;

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectService::InspectionThread_AutoFocus_Inspect] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

#ifdef INLINE_MODE
		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
#endif
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionCamType] = TRUE;

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		return 0;
	}
}

UINT InspectionThread_AutoFocus_Inspect_OneGrabFunction(LPVOID lp)
{
	CString strLog;

	INSP_THREAD_PARAM* pThreadItem = (INSP_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	int iVisionCamType = pThreadItem->iVisionCamType;
	SAFE_DELETE(pThreadItem);

	int iPcVisionNo = VISION_NUMBER_1;

	if (iVisionCamType == VISION_NUMBER_1)
		iPcVisionNo = VISION_NUMBER_1;
	else if (iVisionCamType == VISION_NUMBER_2)
		iPcVisionNo = VISION_NUMBER_2;
	else if (iVisionCamType == VISION_NUMBER_3)
		iPcVisionNo = VISION_NUMBER_3;
	else if (iVisionCamType == VISION_NUMBER_4)
		iPcVisionNo = VISION_NUMBER_4;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iVisionCamType];

	int i;
	CString sLotID;
	int iMzNo, iJigNo, iTrayNo, iModuleNo;
	int iStageNo = STAGE_NUMBER_1;
	double dLensHeightDev;
	sLotID = pInspectService->m_sLotID_H[iVisionCamType];
	iMzNo = pInspectService->m_iMzNo_H[iVisionCamType];
	iJigNo = pInspectService->m_iJigNo_H[iVisionCamType];
	iTrayNo = pInspectService->m_iTrayNo_H[iVisionCamType];
	iModuleNo = pInspectService->m_iModuleNo_H[iVisionCamType];
	dLensHeightDev = pInspectService->m_dHeight_H[iVisionCamType];
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

#ifdef INLINE_MODE
	iStageNo = pInspectService->m_iStageNo_H[iVisionCamType];
#endif

	CString sVisionCamType_Comm;
	sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionCamType][iStageNo];

	BOOL bInspectionGrab = FALSE;

	int iTotalGrabRetry = 0;
	int iTotalFocusMoveRetry = 0;

	try
	{
		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_sModelName == ".")
			return 0;

		int iCurGrabCircularIndex = THEAPP.m_pDualCameraManager[iPcVisionNo]->GetGrabCircularIndex();

		strLog.Format("%s/ Scan start, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iCurGrabCircularIndex);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		auto total_log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		BOOL bGrabFail = FALSE;
		BOOL bGrabSuccess = TRUE;

		MIL_DOUBLE tMilGrabStart = 0, tMilGrabEnd = 0;

		int iSeqAddrIndex = 0;
		int iNoSeqGrabImage = 1;
		int iNextSeqAddrIndex = 0;

		int iLotModuleIndex, iStageIndex;

		BOOL bSingleDualGrabMode = TRUE;
		////////////////////////////// 스캔 시작 ////////////////////////////////////

#ifdef INLINE_MODE

		bInspectionGrab = TRUE;

		double dPrevCamZ, dPrevLightZ, dPrevHeadX, dPrevStageY, dPrevStageT, dPrevStageR;
		dPrevCamZ = dPrevLightZ = dPrevHeadX = dPrevStageY = dPrevStageT = dPrevStageR = -100000000;
		double dCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		DWORD dwZMoveStart = 0, dwZMoveEnd = 0;
		DWORD dwSeqChangeStart = 0, dwSeqChangeEnd = 0;
		BOOL bFocusMoveSuccess = FALSE;

		int iGrabCount = 0;
		int iNoGrabing = 0;
		int iEndGrabIndex = 0;

		int iWSSDelayTime = 0;
		BOOL bAddWSSPostDelay = TRUE;

		// Auto Focus
		double dDeltaCamZ = 0;
		double dDeltaAngleDeg = 0;
		BOOL bModuleRotationCheck = FALSE;
		HObject HModulePositionCheckImage;

		iNoGrabing = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->GetGrabingNumber(iStageNo, iNoCurImageGrab);
		THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Start(iGrabCount, iNoGrabing);

		// Change Grab Sequence Address
		iSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];
		THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iSeqAddrIndex);

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab)
				break;

			if (iGrabCount >= g_iVisionMaxGrabBuffer[iVisionCamType])
				break;

			dDeltaCamZ = 0;
			dDeltaAngleDeg = 0;

			if ((iNoCurImageGrab + 1) >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfLensStartImageNo && (iNoCurImageGrab + 1) <= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfLensEndImageNo)
			{
				dDeltaCamZ = dLensHeightDev;

				if (fabs(dDeltaCamZ) > AF_MAX_DEVIATION_MM)
					dDeltaCamZ = 0;
			}

			if (bModuleRotationCheck)
			{
				pInspectService->SetCycleStopStatus(FALSE);
				pInspectService->SetReloadStatus(FALSE);

				if ((iNoCurImageGrab + 1) >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfEastSideStartImageNo && (iNoCurImageGrab + 1) <= (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfEastSideEndImageNo))
				{
					while (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_NOT_READY)
					{
						Sleep(1);

						if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
						{
							THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
							Sleep(10);

							return 0;
						}
					}

					if (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_RESULT_GOOD)
					{
						dDeltaCamZ = pInspectService->m_dAutoFocusDeltaX[iPcVisionNo] * 0.005;

						strLog.Format("%s/ Auto focus, Direction: East, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, DeltaZ(mm): %.3lf", sVisionCamType_Comm, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, dDeltaCamZ);
						THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

						if (fabs(dDeltaCamZ) > AF_MAX_DEVIATION_MM)
							dDeltaCamZ = 0;

						dDeltaAngleDeg = pInspectService->m_dAutoFocusModuleRotationAngle[iPcVisionNo];
					}
				}
				else if ((iNoCurImageGrab + 1) >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfWestSideStartImageNo && (iNoCurImageGrab + 1) <= (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfWestSideEndImageNo))
				{
					while (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_NOT_READY)
					{
						Sleep(1);

						if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
						{
							THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
							Sleep(10);

							return 0;
						}
					}

					if (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_RESULT_GOOD)
					{
						dDeltaCamZ = -pInspectService->m_dAutoFocusDeltaX[iPcVisionNo] * 0.005;

						strLog.Format("%s/ Auto focus, Direction: West, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, DeltaZ(mm) %.3lf", sVisionCamType_Comm, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, dDeltaCamZ);
						THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

						if (fabs(dDeltaCamZ) > AF_MAX_DEVIATION_MM)
							dDeltaCamZ = 0;

						dDeltaAngleDeg = pInspectService->m_dAutoFocusModuleRotationAngle[iPcVisionNo];
					}
				}
				else if ((iNoCurImageGrab + 1) >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfSouthSideStartImageNo && (iNoCurImageGrab + 1) <= (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfSouthSideEndImageNo))
				{
					while (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_NOT_READY)
					{
						Sleep(1);

						if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
						{
							THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
							Sleep(10);

							return 0;
						}
					}

					if (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_RESULT_GOOD)
					{
						dDeltaCamZ = pInspectService->m_dAutoFocusDeltaY[iPcVisionNo] * 0.005;

						strLog.Format("%s/ Auto focus, Direction: South, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, DeltaZ((mm): %.3lf", sVisionCamType_Comm, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, dDeltaCamZ);
						THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

						if (fabs(dDeltaCamZ) > AF_MAX_DEVIATION_MM)
							dDeltaCamZ = 0;

						dDeltaAngleDeg = pInspectService->m_dAutoFocusModuleRotationAngle[iPcVisionNo];
					}
				}
				else if ((iNoCurImageGrab + 1) >= THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfNorthSideStartImageNo && (iNoCurImageGrab + 1) <= (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfNorthSideEndImageNo))
				{
					while (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_NOT_READY)
					{
						Sleep(1);

						if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
						{
							THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
							Sleep(10);

							return 0;
						}
					}

					if (pInspectService->m_iAutoFocusModuleCheckResult[iPcVisionNo] == TOP_MODULE_POS_CHECK_RESULT_GOOD)
					{
						dDeltaCamZ = -pInspectService->m_dAutoFocusDeltaY[iPcVisionNo] * 0.005;

						strLog.Format("%s/ Auto focus, Direction: North, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, DeltaZ(mm): %.3lf", sVisionCamType_Comm, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, dDeltaCamZ);
						THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

						if (fabs(dDeltaCamZ) > AF_MAX_DEVIATION_MM)
							dDeltaCamZ = 0;

						dDeltaAngleDeg = pInspectService->m_dAutoFocusModuleRotationAngle[iPcVisionNo];
					}
				}
			}

			double tGrabStart = 0, tGrabEnd = 0;

			dCamZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dZFocusPosition[iStageNo][iNoCurImageGrab];
			dLightZ = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dLightZPosition[iStageNo][iNoCurImageGrab];
			dHeadX = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageXPosition[iStageNo][iNoCurImageGrab];
			dStageY = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dStageYPosition[iStageNo][iNoCurImageGrab];
			dStageT = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dTiltPosition[iStageNo][iNoCurImageGrab];
			dStageR = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dRotatePosition[iStageNo][iNoCurImageGrab];

			dwSeqChangeStart = GetTickCount();

			if (fabs(dCamZ - dPrevCamZ) > Z_MOVE_OFFSET || fabs(dLightZ - dPrevLightZ) > Z_MOVE_OFFSET || fabs(dHeadX - dPrevHeadX) > Z_MOVE_OFFSET || fabs(dStageY - dPrevStageY) > Z_MOVE_OFFSET || fabs(dStageT - dPrevStageT) > Z_MOVE_OFFSET || fabs(dStageR - dPrevStageR) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iVisionCamType, iStageNo, iJigNo, dCamZ + dDeltaCamZ, dLightZ, dHeadX, dStageY, dStageT, dStageR + dDeltaAngleDeg);
				pInspectService->SetCycleStopStatus(FALSE);
				pInspectService->SetReloadStatus(FALSE);

				iNextSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];

				if (iSeqAddrIndex != iNextSeqAddrIndex)
				{
					THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iNextSeqAddrIndex, TRUE);
					bAddWSSPostDelay = TRUE;
				}

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iVisionCamType]) // 무브 컴플리트 기다림
				{
					if (pInspectService->GetCycleStopStatus() == TRUE || pInspectService->GetReloadStatus() == TRUE)
					{
						THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
						Sleep(10);

						return 0;
					}

					Sleep(1);
				}

				dPrevCamZ = dCamZ;
				dPrevLightZ = dLightZ;
				dPrevHeadX = dHeadX;
				dPrevStageY = dStageY;
				dPrevStageT = dStageT;
				dPrevStageR = dStageR;
			}
			else
			{
				iNextSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];

				if (iSeqAddrIndex != iNextSeqAddrIndex)
				{
					THEAPP.m_pTriggerManager->GrabSeqChangeByEthernet(iVisionCamType, iNextSeqAddrIndex, TRUE, TRUE);
					bAddWSSPostDelay = TRUE;
				}
			}

			iRetryCnt = 0;
			int K = 0;

			bGrabSuccess = FALSE;

			iSeqAddrIndex = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iSeqAddrIndex[iStageNo][iNoCurImageGrab];
			iNoSeqGrabImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iAddrCount[iSeqAddrIndex];
			iWSSDelayTime = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_PageControlData.m_iWSSDelayTime[iSeqAddrIndex];

			if (bAddWSSPostDelay && iWSSDelayTime > 0)
				Sleep(iWSSDelayTime);
			bAddWSSPostDelay = FALSE;

#ifdef USE_MATROX_TIMER
			MappTimer(M_DEFAULT, M_TIMER_READ + M_SYNCHRONOUS, &tMilGrabStart);
#else
			log_time_start = std::chrono::high_resolution_clock::now();
#endif

			for (K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				if (K > 0)
					++iTotalGrabRetry;

				bGrabSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction(iGrabCount, iNoSeqGrabImage);

				if (bGrabSuccess)
					break;
				else
				{
					if (K < (THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo - 1))
					{
						THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
						iNoGrabing = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->GetGrabingNumber(iStageNo, iNoCurImageGrab);
						THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Start(iGrabCount, iNoGrabing);
						iEndGrabIndex = iGrabCount;
					}
				}
			}	// Grab Retry

			if (bGrabSuccess == FALSE)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

#ifdef USE_MATROX_TIMER
			MappTimer(M_DEFAULT, M_TIMER_READ + M_SYNCHRONOUS, &tMilGrabEnd);
			strLog.Format("%s/ Grab, Time(ms): %.0lf, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, Grab count: %d", sVisionCamType_Comm, (tMilGrabEnd - tMilGrabStart) * 1000.0, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iRetryCnt);
#else
			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();
			strLog.Format("%s/ Grab, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, Grab count: %d", sVisionCamType_Comm, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, iRetryCnt);
#endif
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

			if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseSpecularImage[iNoCurImageGrab] == TRUE)
			{
				iNoCurImageGrab += DFM_USED_CONV_IMAGE_NUMBER;
				iGrabCount += SPV_RAW_IMAGE_NUMBER;
			}
			else if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bUseDiffusedImage[iNoCurImageGrab] == TRUE)
			{
				iNoCurImageGrab += DIFFUSED_USED_CONV_IMAGE_NUMBER;
				iGrabCount += DIFFUSED_RAW_IMAGE_NUMBER;
			}
			else
			{
				iNoCurImageGrab += iNoSeqGrabImage;
				iGrabCount += iNoSeqGrabImage;
			}

			if (bGrabSuccess == FALSE)
			{
				THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
				iNoGrabing = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->GetGrabingNumber(iStageNo, iNoCurImageGrab);
				THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Start(iGrabCount, iNoGrabing);
				iEndGrabIndex = iGrabCount;
			}
			else
			{
				if (bModuleRotationCheck == FALSE && THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfModuleRotationAngleImageNo <= iNoCurImageGrab)
				{
					int iDiff;
					iDiff = iNoCurImageGrab - THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iAfModuleRotationAngleImageNo;

					if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "GB")
						MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
					else if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "BG")
						MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_BG);
					else
						MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][iGrabCount - 1 - iDiff], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);

					HObject HTempGrabImage;

					Compose3(THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iGrabCount - 1 - iDiff][0],
						THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iGrabCount - 1 - iDiff][1],
						THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex][iGrabCount - 1 - iDiff][2],
						&HTempGrabImage);

					CopyImage(HTempGrabImage, &HModulePositionCheckImage);

					MODULE_POSITION_THREAD_PARAM* pModulePosParam = new MODULE_POSITION_THREAD_PARAM(&HModulePositionCheckImage, pInspectService, iMzNo, iPcVisionNo, iStageNo, iVisionCamType);
					AfxBeginThread(AutoFocusThread, LPVOID(pModulePosParam));

					bModuleRotationCheck = TRUE;
				}
			}
		}

		THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(iEndGrabIndex, iNoGrabing);
		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bGrabIndexMismatchDetected = FALSE;
		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_iGrabIndexMismatchOffset = 0;

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		if (bGrabFail == TRUE)
		{
			strLog.Format("%s/ Grab fail(No signal), Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));
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

					return 0;
				}
			}

			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("%s/ Scan complete holding, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Comm, sLotID, iMzNo, iTrayNo, iModuleNo, log_time_ms);
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));
		}

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionCamType] = TRUE;

		auto total_log_time_end = std::chrono::high_resolution_clock::now();
		auto total_log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(total_log_time_end - total_log_time_start).count();
		strLog.Format("%s/ Scan done, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d", sVisionCamType_Short, total_log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iCurGrabCircularIndex);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		for (i = 0; i < iGrabCount; i++)
		{
			if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pDualCameraManager[iPcVisionNo]->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pDualCameraManager[iPcVisionNo]->MilMonoImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->MilColorImageBuf[iCurGrabCircularIndex][i], THEAPP.m_pDualCameraManager[iPcVisionNo]->m_MilWBCoeff, M_BAYER_GB);
		}

#else	// Offline Mode =>

		HObject HInspectImage[MAX_IMAGE_TAB][3];

		for (int iii = 0; iii < MAX_IMAGE_TAB; iii++)
		{
			for (int jjj = 0; jjj < 3; jjj++)
				GenEmptyObj(&(HInspectImage[iii][jjj]));
		}

		CString FolderName, ImageName;

		if (THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath != "")
			FolderName.Format(THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath + "\\");
		else
		{
			AfxMessageBox("디버깅 영상 폴더가 지정되지 않았습니다.", MB_SYSTEMMODAL);
			return 0;
		}

		bGrabFail = TRUE;

		char chSep = '_';
		CString sFileBarcode = _T("NOREAD");

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

				int iNoInspectImage = 0;
				iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

				int iNoImageYDir, iModular;
				int iCropImageSizeX, iCropImageSizeY;

				iNoImageYDir = iNoInspectImage / MAX_COMBINE_IMAGE_NUMBER;
				iModular = iNoInspectImage % MAX_COMBINE_IMAGE_NUMBER;
				if (iModular > 0)
					iNoImageYDir += 1;

				ImageName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, iNoInspectImage);
				sReadFileName = FolderName + ImageName;

				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);

					FindClose(hFindFile);

					ReadImage(&HCombineImage, (HTuple)strRawImageFileFullName);

					AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);

					if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
						sFileBarcode.TrimRight(".bmp");
					else
						sFileBarcode.TrimRight(".jpg");

					EnterCriticalSection(&THEAPP.m_csBarcodeRead);

					THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;

					LeaveCriticalSection(&THEAPP.m_csBarcodeRead);

					if (THEAPP.m_pGFunction->ValidHImage(HCombineImage) == TRUE)
					{
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImageSize(HCombineImage, &htImageWidth, &htImageHeight);
						int wd = (int)htImageWidth.L();
						int ht = (int)htImageHeight.L();

						iCropImageSizeX = wd / MAX_COMBINE_IMAGE_NUMBER;
						iCropImageSizeY = ht / iNoImageYDir;

						POINT CropLTPoint, CropRBPoint;
						int iImageIndexX, iImageIndexY;
						HObject HCropImage;

						for (i = 0; i < iNoInspectImage; i++)
						{
							iImageIndexX = i % MAX_COMBINE_IMAGE_NUMBER;
							iImageIndexY = i / MAX_COMBINE_IMAGE_NUMBER;

							CropLTPoint.x = iImageIndexX * iCropImageSizeX;
							CropRBPoint.x = iImageIndexX * iCropImageSizeX + iCropImageSizeX - 1;
							CropLTPoint.y = iImageIndexY * iCropImageSizeY;
							CropRBPoint.y = iImageIndexY * iCropImageSizeY + iCropImageSizeY - 1;

							CropRectangle1(HCombineImage, &HColorImage, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

							if (THEAPP.m_pGFunction->ValidHImage(HColorImage))
							{
								HTuple htImageHeight = 0;
								HTuple htImageWidth = 0;
								HalconCpp::GetImageSize(HColorImage, &htImageWidth, &htImageHeight);

								if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
									ZoomImageSize(HColorImage, &HColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");

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
				HTuple HExp;
				HTuple HOperatorName, HErrMsg;
				CString sOperatorName, sErrMsg;
				except.ToHTuple(&HExp);
				except.GetExceptionData(HExp, "operator", &HOperatorName);
				except.GetExceptionData(HExp, "error_message", &HErrMsg);
				sOperatorName = HOperatorName.S();
				sErrMsg = HErrMsg.S();

				strLog.Format("Halcon Exception [InspectService::InspectionThread_AutoFocus_Inspect_OneGrabFunction Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
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

				for (int i = 0; i < THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab; i++)
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
						HTuple htImgPtrR, htImgPtrG, htImgPtrB;
						HTuple htType = 0;
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImagePointer3(HColorImage, &htImgPtrR, &htImgPtrG, &htImgPtrB, &htType, &htImageWidth, &htImageHeight);

						if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
							ZoomImageSize(HColorImage, &HColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");
					}
					catch (HException &except)
					{
						;
					}

					if (i == (THEAPP.m_nOfflineBarcodeImageNo - 1))		// default: if (i == 0)
					{
						AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);

						if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
							sFileBarcode.TrimRight(".bmp");
						else
							sFileBarcode.TrimRight(".jpg");

						EnterCriticalSection(&THEAPP.m_csBarcodeRead);

						THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;

						LeaveCriticalSection(&THEAPP.m_csBarcodeRead);
					}

					Decompose3(HColorImage, &(HInspectImage[i][0]), &(HInspectImage[i][1]), &(HInspectImage[i][2]));
				}

				if (bFileFindFail == FALSE)
					bGrabFail = FALSE;
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

				strLog.Format("Halcon Exception [InspectService::InspectionThread_AutoFocus_Inspect_OneGrabFunction Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
				THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
			}
		}

		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionCamType] = TRUE;

#endif	// Offline Mode

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		BOOL bCopyRet;
		int i;

#ifdef INLINE_MODE
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

		if (bGrabFail)
		{
			if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
			{
				THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
			}
		}

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = iStageNo;
		param.iJigNo = iJigNo;
		param.sTrayID = _T("NoUse");
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.iCurCircularGrabIdx = iCurGrabCircularIndex;
		param.iNoGrabRetry = iTotalGrabRetry;
		param.iNoFocusMoveRetry = iTotalFocusMoveRetry;
		param.copyMode = AlgorithmCopyParam::DFM;
		param.ppGrabHImage = THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex];
		param.bColor = TRUE;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
			{
				THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
			}

			param.bGrabFail = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}
#else
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = iStageNo;
		param.iJigNo = iJigNo;
		param.sTrayID = _T("NoUse");
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.iCurCircularGrabIdx = iCurGrabCircularIndex;
		param.iNoGrabRetry = iTotalGrabRetry;
		param.iNoFocusMoveRetry = iTotalFocusMoveRetry;
		param.copyMode = AlgorithmCopyParam::NORMAL;
		param.ppGrabHImage = HInspectImage;
		param.bColor = TRUE;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			param.bGrabFail = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}
#endif

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bGrabCircularBufferCopyDone[iCurGrabCircularIndex] = TRUE;

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectService::InspectionThread_AutoFocus_Inspect_OneGrabFunction] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

#ifdef INLINE_MODE
		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
#endif
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionCamType] = TRUE;

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		return 0;
	}
}

UINT InspectionThread_SingleLens(LPVOID lp)
{
	CString strLog;

	INSP_THREAD_PARAM* pThreadItem = (INSP_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	int iVisionCamType = pThreadItem->iVisionCamType;
	SAFE_DELETE(pThreadItem);

	int iPcVisionNo = VISION_NUMBER_1;

	if (iVisionCamType == VISION_NUMBER_1)
		iPcVisionNo = VISION_NUMBER_1;
	else if (iVisionCamType == VISION_NUMBER_2)
		iPcVisionNo = VISION_NUMBER_2;
	else if (iVisionCamType == VISION_NUMBER_3)
		iPcVisionNo = VISION_NUMBER_3;
	else if (iVisionCamType == VISION_NUMBER_4)
		iPcVisionNo = VISION_NUMBER_4;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	int i;
	CString sLotID, sTrayID;
	int iMzNo, iJigNo, iTrayNo, iModuleNo;
	int iStageNo = STAGE_NUMBER_1;
	sLotID = pInspectService->m_sLotID_H[iVisionCamType];
	iMzNo = pInspectService->m_iMzNo_H[iVisionCamType];
	sTrayID = pInspectService->m_sTrayID_H[iVisionCamType];
	iJigNo = pInspectService->m_iJigNo_H[iVisionCamType];
	iTrayNo = pInspectService->m_iTrayNo_H[iVisionCamType];
	iModuleNo = pInspectService->m_iModuleNo_H[iVisionCamType];
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

#ifdef INLINE_MODE
	iStageNo = pInspectService->m_iStageNo_H[iVisionCamType];
#endif

	CString sVisionCamType_Comm;
	sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];
	
	try
	{
		if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_sModelName == ".")
			return 0;

		int iCurGrabCircularIndex = THEAPP.m_pDualCameraManager[iPcVisionNo]->GetGrabCircularIndex();

		strLog.Format("%s/ Scan start, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iCurGrabCircularIndex);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

		auto total_log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_start = std::chrono::high_resolution_clock::now();
		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		BOOL bGrabFail = FALSE;
		BOOL bGrabSuccess = TRUE;

		int iNoSeqGrabImage = 1;

		int iLotModuleIndex, iStageIndex;

		////////////////////////////// 스캔 시작 ////////////////////////////////////

#ifdef INLINE_MODE
		
		int iNoCurImageGrab = 0;
		int iRetryCnt;
		BOOL bFocusMoveSuccess = FALSE;

		int iGrabCount = 0;
		int iNoGrabing = 0;
		int iTriggerVisionGrabNumber = 4;
		int iTriggerPeriodMsec = 25;
		const DWORD TRIGGER_TIMEOUT_MS = 500;

		iTriggerVisionGrabNumber = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iTriggerImageNumber;
		iTriggerPeriodMsec = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iTriggerPeriod;
		iNoGrabing = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iNoUsedImageGrab;
		THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Start(iGrabCount, iNoGrabing);

		g_objAJinAXL.ResetTriggerEvent(iVisionCamType);

		THEAPP.m_pHandlerService->Set_TriggerRequest(sVisionCamType_Comm, sLotID, iMzNo, sTrayID, iTrayNo, iModuleNo);

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iNoUsedImageGrab)
				break;

			if (iGrabCount >= g_iVisionMaxGrabBuffer[iVisionCamType])
				break;
			
			bGrabSuccess = FALSE;
					   
			log_time_start = std::chrono::high_resolution_clock::now();

			HANDLE hTrigger = g_objAJinAXL.GetTriggerEvent(iVisionCamType);
			DWORD dwWait = ::WaitForSingleObject(hTrigger, TRIGGER_TIMEOUT_MS);

			if (dwWait == WAIT_TIMEOUT)
			{
				strLog.Format("%s/ Trigger TIMEOUT (%d ms), LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d",
					sVisionCamType_Comm, TRIGGER_TIMEOUT_MS,
					sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1);
				THEAPP.m_log_inspection->error("{}", LOG_CSTR(strLog));

				bGrabFail = TRUE;
				break;
			}
			else if (dwWait != WAIT_OBJECT_0)
			{
				strLog.Format("%s/ Trigger WAIT ERROR (%lu), GetLastError: %lu", sVisionCamType_Comm, dwWait, ::GetLastError());
				THEAPP.m_log_inspection->error("{}", LOG_CSTR(strLog));

				bGrabFail = TRUE;
				break;
			}

			bGrabSuccess = THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_SingleLens(iGrabCount, iTriggerVisionGrabNumber, iTriggerPeriodMsec, iDualModelData, iPcVisionNo);

			if (bGrabSuccess == FALSE)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();
			strLog.Format("%s/ Grab, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Image: %d, Grab Success: %s", sVisionCamType_Comm, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iNoCurImageGrab + 1, bGrabSuccess ? "OK" : "NG");
		
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));

			iNoCurImageGrab += iTriggerVisionGrabNumber;
			iGrabCount += iTriggerVisionGrabNumber;

			if (bGrabSuccess == FALSE)
			{
				break;
			}
		}

		THEAPP.m_pDualCameraManager[iPcVisionNo]->AutoRunCameraGrab_OneGrabFunction_Stop(0, iNoGrabing);
		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bGrabIndexMismatchDetected = FALSE;
		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_iGrabIndexMismatchOffset = 0;

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		if (bGrabFail == TRUE)
		{
			strLog.Format("%s/ Grab fail(No signal), Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));
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

					return 0;
				}
			}

			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("%s/ Scan complete holding, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Comm, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo);
			THEAPP.m_log_scan->info("{}", LOG_CSTR(strLog));
		}

		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

		auto total_log_time_end = std::chrono::high_resolution_clock::now();
		auto total_log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(total_log_time_end - total_log_time_start).count();
		strLog.Format("%s/ Scan done, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, iCurGrabCircularIndex: %d", sVisionCamType_Short, total_log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, iCurGrabCircularIndex);
		THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

#else	// Offline Mode =>
		HObject HInspectImage[MAX_IMAGE_TAB][3];

		for (int iii = 0; iii < MAX_IMAGE_TAB; iii++)
		{
			for (int jjj = 0; jjj < 3; jjj++)
				GenEmptyObj(&(HInspectImage[iii][jjj]));
		}

		CString FolderName, ImageName;

		if (THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath != "")
			FolderName.Format(THEAPP.Struct_PreferenceStruct.m_strOfflineTestFolderPath + "\\");
		else
		{
			AfxMessageBox("디버깅 영상 폴더가 지정되지 않았습니다.", MB_SYSTEMMODAL);
			return 0;
		}

		bGrabFail = TRUE;

		char chSep = '_';
		CString sFileBarcode = _T("NOREAD");

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

				int iNoInspectImage = 0;
				iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

				int iNoImageYDir, iModular;
				int iCropImageSizeX, iCropImageSizeY;

				iNoImageYDir = iNoInspectImage / MAX_COMBINE_IMAGE_NUMBER;
				iModular = iNoInspectImage % MAX_COMBINE_IMAGE_NUMBER;
				if (iModular > 0)
					iNoImageYDir += 1;

				ImageName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, iNoInspectImage);
				sReadFileName = FolderName + ImageName;

				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);

					FindClose(hFindFile);

					ReadImage(&HCombineImage, (HTuple)strRawImageFileFullName);

					AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);

					if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
						sFileBarcode.TrimRight(".bmp");
					else
						sFileBarcode.TrimRight(".jpg");

					EnterCriticalSection(&THEAPP.m_csBarcodeRead);

					THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;

					LeaveCriticalSection(&THEAPP.m_csBarcodeRead);

					if (THEAPP.m_pGFunction->ValidHImage(HCombineImage) == TRUE)
					{
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImageSize(HCombineImage, &htImageWidth, &htImageHeight);
						int wd = (int)htImageWidth.L();
						int ht = (int)htImageHeight.L();

						iCropImageSizeX = wd / MAX_COMBINE_IMAGE_NUMBER;
						iCropImageSizeY = ht / iNoImageYDir;

						POINT CropLTPoint, CropRBPoint;
						int iImageIndexX, iImageIndexY;
						HObject HCropImage;

						for (i = 0; i < iNoInspectImage; i++)
						{
							iImageIndexX = i % MAX_COMBINE_IMAGE_NUMBER;
							iImageIndexY = i / MAX_COMBINE_IMAGE_NUMBER;

							CropLTPoint.x = iImageIndexX * iCropImageSizeX;
							CropRBPoint.x = iImageIndexX * iCropImageSizeX + iCropImageSizeX - 1;
							CropLTPoint.y = iImageIndexY * iCropImageSizeY;
							CropRBPoint.y = iImageIndexY * iCropImageSizeY + iCropImageSizeY - 1;

							CropRectangle1(HCombineImage, &HColorImage, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

							if (THEAPP.m_pGFunction->ValidHImage(HColorImage))
							{
								HTuple htImageHeight = 0;
								HTuple htImageWidth = 0;
								HalconCpp::GetImageSize(HColorImage, &htImageWidth, &htImageHeight);

								if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
									ZoomImageSize(HColorImage, &HColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");

								CopyImage(HColorImage, &(HInspectImage[i][0]));
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
				HTuple HExp;
				HTuple HOperatorName, HErrMsg;
				CString sOperatorName, sErrMsg;
				except.ToHTuple(&HExp);
				except.GetExceptionData(HExp, "operator", &HOperatorName);
				except.GetExceptionData(HExp, "error_message", &HErrMsg);
				sOperatorName = HOperatorName.S();
				sErrMsg = HErrMsg.S();

				strLog.Format("Halcon Exception [InspectService::InspectionThread_SingleLens Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
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

				for (int i = 0; i < THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab; i++)
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
						HTuple htImgPtrR, htImgPtrG, htImgPtrB;
						HTuple htType = 0;
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImagePointer3(HColorImage, &htImgPtrR, &htImgPtrG, &htImgPtrB, &htType, &htImageWidth, &htImageHeight);

						if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType])
							ZoomImageSize(HColorImage, &HColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iVisionCamType], "bicubic");
					}
					catch (HException &except)
					{
						;
					}

					if (i == (THEAPP.m_nOfflineBarcodeImageNo - 1))		// default: if (i == 0)
					{
						AfxExtractSubString(sFileBarcode, FindFileData.cFileName, 3, chSep);

						if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
							sFileBarcode.TrimRight(".bmp");
						else
							sFileBarcode.TrimRight(".jpg");

						EnterCriticalSection(&THEAPP.m_csBarcodeRead);

						THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sFileBarcode;

						LeaveCriticalSection(&THEAPP.m_csBarcodeRead);
					}

					CopyImage(HColorImage, &(HInspectImage[i][0]));
				}

				if (bFileFindFail == FALSE)
					bGrabFail = FALSE;
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

				strLog.Format("Halcon Exception [InspectService::InspectionThread_SingleLens Debug Read Image] : <%s>%s", sOperatorName, sErrMsg);
				THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
			}
		}

		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

#endif	// Offline Mode

		//////////////////////////////////////////////////////////////////////////
		//	Grab 완료 => 알고리즘 Thread 시작
		//////////////////////////////////////////////////////////////////////////

		BOOL bCopyRet;
		int i;

#ifdef INLINE_MODE
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iNoUsedImageGrab;

		if (bGrabFail)
		{
			if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
			{
				THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
			}
		}

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = iStageNo;
		param.iJigNo = iJigNo;
		param.sTrayID = sTrayID;
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.iCurCircularGrabIdx = iCurGrabCircularIndex;
		param.iNoGrabRetry = 0;
		param.iNoFocusMoveRetry = 0;
		param.copyMode = AlgorithmCopyParam::NORMAL;
		param.ppGrabHImage = THEAPP.m_pDualCameraManager[iPcVisionNo]->m_hoCallBackImage[iCurGrabCircularIndex];
		param.bColor = FALSE;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])	// Barcode
			{
				THEAPP.m_pHandlerService->Set_BarcodeResult(sLotID, iMzNo, iTrayNo, iModuleNo, "NOREAD");
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = "NOREAD";
			}

			param.bGrabFail = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}
#else
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iPcVisionNo = iPcVisionNo;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iVisionCamType = iVisionCamType;
		pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

		AlgorithmCopyParam param;
		param.bGrabFail = bGrabFail;
		param.sLotID = sLotID;
		param.iMzNo = iMzNo;
		param.iStageNo = iStageNo;
		param.iJigNo = iJigNo;
		param.sTrayID = sTrayID;
		param.iTrayNo = iTrayNo;
		param.iModuleNo = iModuleNo;
		param.iCurCircularGrabIdx = iCurGrabCircularIndex;
		param.iNoGrabRetry = 0;
		param.iNoFocusMoveRetry = 0;
		param.copyMode = AlgorithmCopyParam::NORMAL;
		param.ppGrabHImage = HInspectImage;
		param.bColor = FALSE;

		bCopyRet = pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);

		if (bCopyRet == FALSE)
		{
			param.bGrabFail = TRUE;
			pInspectService->m_pInspectAlgorithm[iVisionCamType].CopyInspectInformation(param);
		}
#endif

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		THEAPP.m_pDualCameraManager[iPcVisionNo]->m_bGrabCircularBufferCopyDone[iCurGrabCircularIndex] = TRUE;

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectService::InspectionThread_SingleLens] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

#ifdef INLINE_MODE
		THEAPP.m_pHandlerService->Set_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
		if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
			THEAPP.m_pHandlerService->Save_ScanComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm);
#endif
		THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

#ifndef INLINE_MODE
#ifndef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		return 0;
	}
}

UINT SaveRawImageThread(LPVOID lp)
{
	CString strLog;

	CInspectService* pInspectService = (CInspectService*)lp;

	int nCurQueueIndex = pInspectService->m_nRawImageQueueIndex;

	int iNoThreadActiveCheck = 0;
	for (int iCheck = 0; iCheck < 10; iCheck++)
	{
		if (pInspectService->m_bRawImageSaveThreadRunning[nCurQueueIndex])
			++iNoThreadActiveCheck;
		Sleep(1);
	}

	if (iNoThreadActiveCheck > 0)
		return 1;

	pInspectService->m_bRawImageSaveThreadRunning[nCurQueueIndex] = TRUE;

	try
	{
		CString strFileFullName, ImageFileName;
		int i;

		while (TRUE)
		{
			if (pInspectService->IsSaveRawImageParamEmpty(nCurQueueIndex))
			{
				strLog.Format("SaveRawImageThread end");
				THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

				pInspectService->ResetRawImageSaveThreadRunning(nCurQueueIndex);
				//pInspectService->m_bRawImageSaveThreadRunning[nCurQueueIndex] = FALSE; (위의함수에서 FALSE됌)
				return 1;
			}

			RAW_IMAGE_SAVE_PARAM* pSaveThreadParam = pInspectService->GetNextSaveRawImageParam(nCurQueueIndex);
			if (pSaveThreadParam == NULL)
			{
				strLog.Format("SaveRawImageThread pSaveThreadParam == NULL");
				THEAPP.m_log_thread->warn("{}", LOG_CSTR(strLog));

				continue;
			}

			CString sVisionCamType_Short;
			sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[pSaveThreadParam->iVisionCamType];

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

							//WriteObject(HCombineImage, HTuple(strFileFullName));

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
				}

				ImageFileName.Format("Module%d_%s_Image(ALL)", pSaveThreadParam->iModuleNo, sVisionCamType_Short);

				if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
				{
					CString strFileFullName;
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
								strFileFullName = THEAPP.m_FileBase.m_strRawImageFolder_Local[pSaveThreadParam->iPcVisionNo][pSaveThreadParam->iMzNo - 1][pSaveThreadParam->iTrayNo - 1] + "\\" + ImageFileName + "_" + pSaveThreadParam->sBarcodeID;

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

					if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
					{
						CString strFileFullName;
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

		strLog.Format("Halcon Exception [InspectService::SaveRawImageThread 파일 저장 오류] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		pInspectService->ResetRawImageSaveThreadRunning(nCurQueueIndex);

		return 0;
	}
}

UINT SaveResultImageThread(LPVOID lp)
{
	CString strLog;

	CInspectService* pInspectService = (CInspectService*)lp;

	int nCurQueueIndex = pInspectService->m_nResultImageQueueIndex;

	int iNoThreadActiveCheck = 0;
	for (int iCheck = 0; iCheck < 10; iCheck++)
	{
		if (pInspectService->m_bResultImageSaveThreadRunning[nCurQueueIndex])
			++iNoThreadActiveCheck;
		Sleep(1);
	}

	if (iNoThreadActiveCheck > 0)
		return 1;

	pInspectService->m_bResultImageSaveThreadRunning[nCurQueueIndex] = TRUE;

	try
	{
		int iSaveCnt = 0;

		while (TRUE)
		{
			if (pInspectService->IsSaveResultImageParamEmpty(nCurQueueIndex))
			{
				strLog.Format("SaveResultImageThread end");
				THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

				pInspectService->ResetResultImageSaveThreadRunning(nCurQueueIndex);

				return 1;
			}

			RESULT_IMAGE_SAVE_PARAM* pSaveThreadParam = pInspectService->GetNextSaveResultImageParam(nCurQueueIndex);
			if (pSaveThreadParam == NULL)
			{
				strLog.Format("SaveResultImageThread pSaveThreadParam == NULL");
				THEAPP.m_log_thread->warn("{}", LOG_CSTR(strLog));

				continue;
			}

			if (THEAPP.m_pGFunction->ValidHImage(pSaveThreadParam->HSaveImage))
			{
				THEAPP.m_FileBase.CreatePath(pSaveThreadParam->sSavePath, TRUE);
				WriteImage(pSaveThreadParam->HSaveImage, "jpg", 0xffffff, HTuple(pSaveThreadParam->sSavePath));
			}

			GenEmptyObj(&(pSaveThreadParam->HSaveImage));

			SAFE_DELETE(pSaveThreadParam);

			if ((iSaveCnt % 2) == 0)
				Sleep(5);

			++iSaveCnt;

			if (iSaveCnt > 100000)
				iSaveCnt = 0;
		}

		pInspectService->ResetResultImageSaveThreadRunning(nCurQueueIndex);
		return 1;
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

		strLog.Format("Halcon Exception [InspectService::SaveResultImageThread 파일 저장 오류] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		pInspectService->ResetResultImageSaveThreadRunning(nCurQueueIndex);

		return 0;
	}
}

UINT SaveReviewImageThread(LPVOID lp)
{
	CString strLog;

	CInspectService* pInspectService = (CInspectService*)lp;

	int nCurQueueIndex = pInspectService->m_nReviewImageQueueIndex;

	int iNoThreadActiveCheck = 0;
	for (int iCheck = 0; iCheck < 10; iCheck++)
	{
		if (pInspectService->m_bReviewImageSaveThreadRunning[nCurQueueIndex])
			++iNoThreadActiveCheck;
		Sleep(1);
	}

	if (iNoThreadActiveCheck > 0)
		return 1;

	pInspectService->m_bReviewImageSaveThreadRunning[nCurQueueIndex] = TRUE;

	CString strSaveFileName;
	try
	{
		int iSaveCnt = 0;

		while (TRUE)
		{
			if (pInspectService->IsSaveReviewImageParamEmpty(nCurQueueIndex))
			{
				strLog.Format("SaveReviewImageThread end");
				THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

				pInspectService->m_bReviewImageSaveThreadRunning[nCurQueueIndex] = FALSE;

				return 1;
			}

			REVIEW_IMAGE_SAVE_PARAM* pSaveThreadParam = pInspectService->GetNextSaveReviewImageParam(nCurQueueIndex);
			if (pSaveThreadParam == NULL)
				continue;

			if (THEAPP.m_pGFunction->ValidHImage(pSaveThreadParam->HSaveImage))
			{
				THEAPP.m_FileBase.CreatePath(pSaveThreadParam->sSavePath, TRUE);

				auto log_time_start = std::chrono::high_resolution_clock::now();
				WriteImage(pSaveThreadParam->HSaveImage, "jpg", 0, HTuple(pSaveThreadParam->sSavePath));
				auto log_time_end = std::chrono::high_resolution_clock::now();
				auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

				strLog.Format("Save review image(Thread), Time(ms): %d, File name: %s", log_time_ms, pSaveThreadParam->sSavePath);
				THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
			}

			GenEmptyObj(&(pSaveThreadParam->HSaveImage));

			SAFE_DELETE(pSaveThreadParam);

			if ((iSaveCnt % 2) == 0)
				Sleep(5);

			++iSaveCnt;

			if (iSaveCnt > 100000)
				iSaveCnt = 0;
		}

		return 1;
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

		strLog.Format("Halcon Exception [InspectService::SaveReviewImageThread] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		pInspectService->m_bReviewImageSaveThreadRunning[nCurQueueIndex] = FALSE;

		return 0;
	}
}

UINT SaveFAIImageThread(LPVOID lp)
{
	CString strLog;

	CInspectService* pInspectService = (CInspectService*)lp;

	int nCurQueueIndex = pInspectService->m_nFAIImageQueueIndex;

	int iNoThreadActiveCheck = 0;
	for (int iCheck = 0; iCheck < 10; iCheck++)
	{
		if (pInspectService->m_bFAIImageSaveThreadRunning[nCurQueueIndex])
			++iNoThreadActiveCheck;
		Sleep(1);
	}

	if (iNoThreadActiveCheck > 0)
		return 1;

	pInspectService->m_bFAIImageSaveThreadRunning[nCurQueueIndex] = TRUE;

	CString strSaveFileName;
	try
	{
		int iSaveCnt = 0;

		while (TRUE)
		{
			if (pInspectService->IsSaveFAIImageParamEmpty(nCurQueueIndex))
			{
				strLog.Format("SaveFAIImageThread end");
				THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

				pInspectService->m_bFAIImageSaveThreadRunning[nCurQueueIndex] = FALSE;

				return 1;
			}

			FAI_IMAGE_SAVE_PARAM* pSaveThreadParam = pInspectService->GetNextSaveFAIImageParam(nCurQueueIndex);
			if (pSaveThreadParam == NULL)
				continue;

			if (THEAPP.m_pGFunction->ValidHImage(pSaveThreadParam->HSaveImage))
			{
				THEAPP.m_FileBase.CreatePath(pSaveThreadParam->sSavePath, TRUE);

				auto log_time_start = std::chrono::high_resolution_clock::now();
				WriteImage(pSaveThreadParam->HSaveImage, "jpg", 0, HTuple(pSaveThreadParam->sSavePath));
				auto log_time_end = std::chrono::high_resolution_clock::now();
				auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

				strLog.Format("Save FAI image(Thread), Time(ms): %d, File name: %s", log_time_ms, pSaveThreadParam->sSavePath);
				THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
			}

			GenEmptyObj(&(pSaveThreadParam->HSaveImage));

			SAFE_DELETE(pSaveThreadParam);

			if ((iSaveCnt % 2) == 0)
				Sleep(5);

			++iSaveCnt;

			if (iSaveCnt > 100000)
				iSaveCnt = 0;
		}

		return 1;
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

		strLog.Format("Halcon Exception [InspectService::SaveFAIImageThread] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		pInspectService->m_bFAIImageSaveThreadRunning[nCurQueueIndex] = FALSE;

		return 0;
	}
}

UINT SaveADJImageThread(LPVOID lp)
{
	CString strLog;

	CInspectService* pInspectService = (CInspectService*)lp;

	int nCurQueueIndex = pInspectService->m_nADJImageQueueIndex;

	int iNoThreadActiveCheck = 0;
	for (int iCheck = 0; iCheck < 10; iCheck++)
	{
		if (pInspectService->m_bADJImageSaveThreadRunning[nCurQueueIndex])
			++iNoThreadActiveCheck;
		Sleep(1);
	}

	if (iNoThreadActiveCheck > 0)
		return 1;

	pInspectService->m_bADJImageSaveThreadRunning[nCurQueueIndex] = TRUE;

	CString strSaveFileName;
	try
	{
		int iSaveCnt = 0;

		while (TRUE)
		{
			if (pInspectService->IsSaveADJImageParamEmpty(nCurQueueIndex))
			{
				strLog.Format("SaveADJImageThread end");
				THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

				pInspectService->m_bADJImageSaveThreadRunning[nCurQueueIndex] = FALSE;

				return 1;
			}

			ADJ_IMAGE_SAVE_PARAM* pSaveThreadParam = pInspectService->GetNextSaveADJImageParam(nCurQueueIndex);
			if (pSaveThreadParam == NULL)
				continue;

			if (THEAPP.m_pGFunction->ValidHImage(pSaveThreadParam->HSaveImage))
			{
				THEAPP.m_FileBase.CreatePath(pSaveThreadParam->sSavePath, TRUE);

				auto log_time_start = std::chrono::high_resolution_clock::now();
				WriteImage(pSaveThreadParam->HSaveImage, "jpg", 0, HTuple(pSaveThreadParam->sSavePath));
				auto log_time_end = std::chrono::high_resolution_clock::now();
				auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

				strLog.Format("Save ADJ image(Thread), Time(ms): %d, File name: %s", log_time_ms, pSaveThreadParam->sSavePath);
				THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
			}

			GenEmptyObj(&(pSaveThreadParam->HSaveImage));

			SAFE_DELETE(pSaveThreadParam);

			if ((iSaveCnt % 2) == 0)
				Sleep(5);

			++iSaveCnt;

			if (iSaveCnt > 100000)
				iSaveCnt = 0;
		}

		return 1;
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

		strLog.Format("Halcon Exception [InspectService::SaveADJImageThread] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		pInspectService->m_bADJImageSaveThreadRunning[nCurQueueIndex] = FALSE;

		return 0;
	}
}

UINT SaveResultLogThread(LPVOID lp)
{
	CString strLog;

	LOT_BARCODE_UPDATE_THREAD_PARAM* pThreadItem = (LOT_BARCODE_UPDATE_THREAD_PARAM*)lp;
	CInspectService* pInspectService = pThreadItem->pInspectService;
	int iMzNo = pThreadItem->iMzNo;
	CString sLotID = pThreadItem->sLotID;
	SAFE_DELETE(pThreadItem);

	CString sSaveData;
	CString sSite, sEqpID, sEqpName, sPara, sToolCavity;

	pInspectService->m_bSaveResultLogThreadDone[iMzNo - 1] = FALSE;

	try
	{
		int iSaveCnt = 0;

		while (TRUE)
		{
			if (pInspectService->IsSaveResultLogParamEmpty(iMzNo))
			{
				pInspectService->m_bSaveResultLogThreadDone[iMzNo - 1] = TRUE;

				return 1;
			}

			RESULT_LOG_SAVE_PARAM* pSaveThreadParam = pInspectService->GetNextSaveResultLogParam(iMzNo);
			if (pSaveThreadParam == NULL)
			{
				strLog.Format("SaveResultLogThread pSaveThreadParam == NULL");
				THEAPP.m_log_thread->warn("{}", LOG_CSTR(strLog));

				continue;
			}

			//////////////////////////////////////////////////////////////////////////

			switch (pSaveThreadParam->iSaveLogType)
			{
			case SAVE_LOG_TYPE_DAY_SUMMARY:
				THEAPP.SaveDaySummaryINI(pSaveThreadParam->strPath, pSaveThreadParam->strType1, pSaveThreadParam->strType2, pSaveThreadParam->bType, pSaveThreadParam->iMzNo, pSaveThreadParam->iNoGrabRetry, pSaveThreadParam->iNoFocusMoveRetry);
				break;
			case SAVE_LOG_TYPE_LOT_RESULT:
				THEAPP.SaveLotResultLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1, pSaveThreadParam->iMzNo);
				break;
			case SAVE_LOG_TYPE_LOT_RESULT_LASPC:
				THEAPP.SaveLotResultLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1, pSaveThreadParam->iMzNo);
				break;
			case SAVE_LOG_TYPE_DAY_LOT_RESULT:
				THEAPP.SaveDayLotResultLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1, pSaveThreadParam->iMzNo);
				break;
#ifdef USE_SUAKIT
			case SAVE_LOG_TYPE_ADJ_DAY_SUMMARY:
				THEAPP.SaveADJDaySummaryINI(pSaveThreadParam->strPath, pSaveThreadParam->strType1, pSaveThreadParam->bType, pSaveThreadParam->iMzNo);
				break;
			case SAVE_LOG_TYPE_ADJ_LOT_RESULT:
				THEAPP.SaveADJLotResultLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1, pSaveThreadParam->strType2, pSaveThreadParam->bType, THEAPP.Struct_PreferenceStruct.m_iPCType, pSaveThreadParam->iPcVisionNo, pSaveThreadParam->iMzNo);
				break;
			case SAVE_LOG_TYPE_ADJ_DAY_LOT_RESULT:
				THEAPP.SaveADJDayLotResultLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1, THEAPP.Struct_PreferenceStruct.m_iPCType, pSaveThreadParam->iMzNo);
				break;
			case SAVE_LOG_TYPE_ADJ_TOTAL_IMAGE_SUMMARY:
				THEAPP.SaveADJTotalImageSummaryINI(pSaveThreadParam->strPath, pSaveThreadParam->strType1, pSaveThreadParam->bType, pSaveThreadParam->iMzNo);
				break;
			case SAVE_LOG_TYPE_ADJ_DAY_SUMMARY_SKIP_RATE:
				THEAPP.SaveADJDaySummarySkipRateINI(pSaveThreadParam->strPath, pSaveThreadParam->strType1, pSaveThreadParam->bType, pSaveThreadParam->iMzNo);
				break;
			case SAVE_LOG_TYPE_ONLY_ADJ_LOT_RESULT:
				THEAPP.SaveOnlyADJLotResultLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1, pSaveThreadParam->strType2, pSaveThreadParam->bType, THEAPP.Struct_PreferenceStruct.m_iPCType, pSaveThreadParam->iPcVisionNo, pSaveThreadParam->iMzNo);
				break;
			case SAVE_LOG_TYPE_ONLY_ADJ_DAY_LOT_RESULT:
				THEAPP.SaveOnlyADJDayLotResultLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1, THEAPP.Struct_PreferenceStruct.m_iPCType, pSaveThreadParam->iMzNo);
				break;
#endif
			case SAVE_LOG_TYPE_MONITORING_GV_EDGE:
				THEAPP.SaveMonitorGVEdgeLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1);
				break;
			case SAVE_LOG_TYPE_MONITORING_CONT_DEFECT:
				THEAPP.SaveMonitorContDefectLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1);
				break;
			case SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE:
				THEAPP.NewSaveDefectVariationFeatureLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1);
				break;
			case SAVE_LOG_TYPE_INSPECT_CONDITION:
				THEAPP.SaveInspectConditionLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1);
				break;
			case SAVE_LOG_TYPE_ALL_DEFECTS_RESULT:
				THEAPP.SaveAllDefectsResultLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1);
				break;
			case SAVE_LOG_TYPE_CONTACT_POINT:
				THEAPP.SaveContactPointLog(pSaveThreadParam->strPath, pSaveThreadParam->strType1, pSaveThreadParam->iMzNo);
				break;
			default:
				break;
			}

			if (pSaveThreadParam)
			{
				delete pSaveThreadParam;
				pSaveThreadParam = NULL;
			}

			if ((iSaveCnt % 2) == 0)
				Sleep(5);

			++iSaveCnt;

			if (iSaveCnt > 100000)
				iSaveCnt = 0;
		}

		pInspectService->m_bSaveResultLogThreadDone[iMzNo - 1] = TRUE;

		return 1;
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

		strLog.Format("Halcon Exception [InspectService::SaveResultLogThread 파일 저장 오류] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		pInspectService->m_bSaveResultLogThreadDone[iMzNo - 1] = TRUE;

		return 0;
	}
}

void CInspectService::SaveRawImage(int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo)
{
	int iThreadQueueNumber;
	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
		iThreadQueueNumber = THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;
	else
		iThreadQueueNumber = 1;

	if (!THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
	{
		for (int iCurImageQueueIndex = 0; iCurImageQueueIndex < iThreadQueueNumber; iCurImageQueueIndex++)
		{
			if (m_bRawImageSaveThreadRunning[iCurImageQueueIndex] == FALSE)
			{
				m_nRawImageQueueIndex = iCurImageQueueIndex;
				AfxBeginThread(SaveRawImageThread, this);
				Sleep(15);
			}
		}
	}
	else
	{
		for (int iCurImageQueueIndex = 0; iCurImageQueueIndex < RAW_IMAGE_TEMP_DRIVE_NUMBER + 1; iCurImageQueueIndex++)
		{
			if (m_bRawImageSaveThreadRunning[iCurImageQueueIndex] == FALSE)
			{
				m_nRawImageQueueIndex = iCurImageQueueIndex;
				AfxBeginThread(SaveRawImageThread, this);
				Sleep(15);
			}
		}
	}
}

void CInspectService::SaveResultImage(int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo)
{
	int iThreadQueueNumber;
	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
		iThreadQueueNumber = THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;
	else
		iThreadQueueNumber = 1;

	for (int iCurImageQueueIndex = 0; iCurImageQueueIndex < iThreadQueueNumber; iCurImageQueueIndex++)
	{
		if (m_bResultImageSaveThreadRunning[iCurImageQueueIndex] == FALSE)
		{
			m_nResultImageQueueIndex = iCurImageQueueIndex;
			AfxBeginThread(SaveResultImageThread, this);
			Sleep(15);
		}
	}
}

void CInspectService::SaveReviewImage(int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo)
{
	int iThreadQueueNumber;
	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
		iThreadQueueNumber = THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;
	else
		iThreadQueueNumber = 1;

	for (int iCurImageQueueIndex = 0; iCurImageQueueIndex < THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber; iCurImageQueueIndex++)
	{
		if (m_bReviewImageSaveThreadRunning[iCurImageQueueIndex] == FALSE)
		{
			m_nReviewImageQueueIndex = iCurImageQueueIndex;
			AfxBeginThread(SaveReviewImageThread, this);
			Sleep(15);
		}
	}
}


void CInspectService::SaveFAIImage(int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo)
{
	int iThreadQueueNumber;
	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
		iThreadQueueNumber = THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;
	else
		iThreadQueueNumber = 1;

	for (int iCurImageQueueIndex = 0; iCurImageQueueIndex < THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber; iCurImageQueueIndex++)
	{
		if (m_bFAIImageSaveThreadRunning[iCurImageQueueIndex] == FALSE)
		{
			m_nFAIImageQueueIndex = iCurImageQueueIndex;
			AfxBeginThread(SaveFAIImageThread, this);
			Sleep(15);
		}
	}
}

void CInspectService::SaveADJImage(int iPcVisionNo, int iMzNo, int iTrayNo, int iModuleNo)
{
	int iThreadQueueNumber;
	if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
		iThreadQueueNumber = THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;
	else
		iThreadQueueNumber = 1;

	for (int iCurImageQueueIndex = 0; iCurImageQueueIndex < THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber; iCurImageQueueIndex++)
	{
		if (m_bADJImageSaveThreadRunning[iCurImageQueueIndex] == FALSE)
		{
			m_nADJImageQueueIndex = iCurImageQueueIndex;
			AfxBeginThread(SaveADJImageThread, this);
			Sleep(15);
		}
	}
}

void CInspectService::SaveResultLog(int iMzNo, CString sLotID)
{
	LOT_BARCODE_UPDATE_THREAD_PARAM* pParam = new LOT_BARCODE_UPDATE_THREAD_PARAM(iMzNo, sLotID, this);

	AfxBeginThread(SaveResultLogThread, LPVOID(pParam));
}

void CInspectService::InspectionMove(int iVisionType, int iMzNo)
{
	INSP_THREAD_PARAM* pParam = new INSP_THREAD_PARAM(iVisionType, this);

	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[iVisionType])
		AfxBeginThread(InspectionThread_Align, LPVOID(pParam));
	else
	{
#ifdef INLINE_MODE
		int iCurGrabCircularIndex;
		THEAPP.m_pDualCameraManager[iVisionType]->IncGrabCircularIndex();
		iCurGrabCircularIndex = THEAPP.m_pDualCameraManager[iVisionType]->GetGrabCircularIndex();
		THEAPP.m_pDualCameraManager[iVisionType]->m_bGrabCircularBufferCopyDone[iCurGrabCircularIndex] = FALSE;

		m_iAutoFocusModuleCheckResult[iVisionType] = TOP_MODULE_POS_CHECK_NOT_READY;

		if (THEAPP.strAutoSettingMode == "Light-S")
		{
			THEAPP.iAutoSettingCountCurrent++;
			AfxBeginThread(InspectionThread_Inspect_AutoSettingL_Single, LPVOID(pParam));
		}
		else if (THEAPP.strAutoSettingMode == "Light-M")
		{
			THEAPP.iAutoSettingCountCurrent++;
			AfxBeginThread(InspectionThread_Inspect_AutoSettingL_Multi, LPVOID(pParam));
		}
		else if (THEAPP.strAutoSettingMode == "Focus")
		{
			THEAPP.iAutoSettingCountCurrent++;
			AfxBeginThread(InspectionThread_Inspect_AutoSettingZ, LPVOID(pParam));
		}
		else
		{
#endif
#ifdef SINGLE_LENS
			AfxBeginThread(InspectionThread_SingleLens, LPVOID(pParam));
#else
			if (THEAPP.Struct_PreferenceStruct.m_bUseBatchInspection)
			{
				InspectionStart_Batch(iVisionType);
			}
			else
			{
				if (THEAPP.Struct_PreferenceStruct.m_bUseAutoFocus)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bUseFastGrab)
						AfxBeginThread(InspectionThread_AutoFocus_Inspect_OneGrabFunction, LPVOID(pParam));
					else
						AfxBeginThread(InspectionThread_AutoFocus_Inspect, LPVOID(pParam));
				}
				else
				{
					if (THEAPP.Struct_PreferenceStruct.m_bUseFastGrab)
						AfxBeginThread(InspectionThread_Inspect_OneGrabFunction, LPVOID(pParam));
					else
						AfxBeginThread(InspectionThread_Inspect, LPVOID(pParam));
				}
			}
#endif
#ifdef INLINE_MODE
		}
#endif
	}
}

BOOL CInspectService::InspectionStart_Batch(int iVisionCamType)
{
	int i;
	CString sLotID;
	int iMzNo, iStageNo, iJigNo, iTrayNo, iModuleNo, iPcVisionNo;

	iPcVisionNo = iVisionCamType;
	sLotID = m_sLotID_H[iPcVisionNo];
	iMzNo = m_iMzNo_H[iPcVisionNo];
#ifdef INLINE_MODE
	iStageNo = m_iStageNo_H[iPcVisionNo];
#else
	iStageNo = 0;
#endif
	iJigNo = m_iJigNo_H[iPcVisionNo];
	iTrayNo = m_iTrayNo_H[iPcVisionNo];
	iModuleNo = m_iModuleNo_H[iPcVisionNo];

	CString sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	CBatchManager *pBatchManager = CBatchManager::GetInstance();
	pBatchManager->Reset(iMzNo, iTrayNo, iModuleNo, iPcVisionNo);

	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
	if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_sModelName == ".")
	{
		strLog.Format("%s/ Batch Inspection Failed(No Model)", sVisionCamType_Short);
		THEAPP.m_log_inspection->error("{}", LOG_CSTR(strLog));
		return FALSE;
	}

	int iNoUsedImageGrab = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iNoUsedImageGrab;

	pBatchManager->SplitBatches_AxisMove(iMzNo, iTrayNo, iModuleNo, iPcVisionNo, iStageNo, iNoUsedImageGrab);

	BATCH_GRAB_PARAM* pParam = new BATCH_GRAB_PARAM();
	pParam->sLotID = sLotID;
	pParam->iMzNo = iMzNo;
	pParam->iJigNo = iJigNo;
	pParam->iTrayNo = iTrayNo;
	pParam->iModuleNo = iModuleNo;
	pParam->iPcVisionNo = iPcVisionNo;
	pParam->iVisionCamType = iVisionCamType;
	pParam->iStageNo = iStageNo;
	pParam->iCurCircularGrabIdx = THEAPP.m_pDualCameraManager[iPcVisionNo]->GetGrabCircularIndex();  // 실제 값

	SetCycleStopStatus(FALSE);
	SetReloadStatus(FALSE);

	AfxBeginThread(BatchGrabThread, LPVOID(pParam));

	strLog.Format("%s/ Batch Inspection Start, LotID: %s, Tray: %d, Module: %d, Stage: %d",
		sVisionCamType_Short, sLotID, iTrayNo, iModuleNo, iStageNo);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

	return TRUE;
}

void CInspectService::OfflineInspection_Folder(CString sCurrentLotID)
{
	int iMzNo = TEACHING_MZ_NO;
	int iTrayNo = THEAPP.m_iOfflineCurTrayNo;
	int iModuleNo;
	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

	if (THEAPP.Struct_PreferenceStruct.m_bUseVmThresOpt)
	{
		for (int iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->ResetVMThresOptData();
	}

	for (iModuleNo = 1; iModuleNo <= THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_iTrayModuleMax; iModuleNo++)
	{
		for (int iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
		{
			THEAPP.m_pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = FALSE;
			if (!THEAPP.Struct_PreferenceStruct.m_bUseVision[iPcVisionNo])
			{
				m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
				continue;
			}

			THEAPP.m_pInspectService->m_iTrayNo_H[iPcVisionNo] = iTrayNo;
			THEAPP.m_pInspectService->m_iModuleNo_H[iPcVisionNo] = iModuleNo;
			THEAPP.m_bLoadCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;
			THEAPP.m_pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = FALSE;
			THEAPP.m_pInspectService->InspectionMove(iPcVisionNo, iMzNo);				// 스캔과 검사 시작
		}

		while (!(m_bOfflineModuleInspectDone[VISION_NUMBER_1] && m_bOfflineModuleInspectDone[VISION_NUMBER_2] && m_bOfflineModuleInspectDone[VISION_NUMBER_3] && m_bOfflineModuleInspectDone[VISION_NUMBER_4]))
		{
			DoEvents(10);
		}

		DoEvents(20);
	}

	if (THEAPP.Struct_PreferenceStruct.m_bUseVmThresOpt)
	{
		for (int iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
			THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->UpdateVMThres();
	}

	THEAPP.m_pInspectService->SaveResultLog(iMzNo, sCurrentLotID);

	if (THEAPP.m_pInspectSummary->m_bDxRunMacro)
	{
		DoEvents(1000);
		THEAPP.g_bMacroStart = TRUE;
	}
}

void CInspectService::DoEvents(DWORD dwSleep)
{
	MSG msg;
	DWORD dwStart = GetTickCount();

	do {

		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		Sleep(1);

	} while (dwSleep > (GetTickCount() - dwStart));
}