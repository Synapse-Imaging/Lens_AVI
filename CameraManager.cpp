#include "stdafx.h"
#include "uScan.h"
#include "CameraManager.h"

UINT DFMProcInspectionThread(LPVOID lp)
{
	CString strLog;

	CCameraManager* pCameraManager = (CCameraManager *)lp;

	CSyncObject* pSyncObjects[] = { &THEAPP.m_eventStop, pCameraManager->GetImageReadyEvent() };

	pCameraManager->GetImageReadyEvent()->ResetEvent();

	while (1)
	{
		/**********************************************************
		* Thread Synchronization
		*	Wait for Conversion Ready !!!
		**********************************************************/
		CMultiLock stopOrImageReady(pSyncObjects, 2);

		DWORD status = stopOrImageReady.Lock(INFINITE, FALSE);

		if (status == WAIT_OBJECT_0) // if first object m_eventStop is signaled then return thread immediately!!!
		{
			stopOrImageReady.Unlock();
			break;
		}

		pCameraManager->DFMGlobalProcInspection();

		pCameraManager->m_eventDFMProcDone.SetEvent();
	}

	strLog.Format("DFMProcInspectionThread Stop");
	THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

	return 1;
}

CCameraManager::CCameraManager(void)
{
	m_pConvertThread = NULL;

	m_bReGrab = FALSE;

	m_bGrabDone = TRUE;

	m_iVisionCamName = VISION_NUMBER_1;

	m_iCamImageSizeX = 1;
	m_iCamImageSizeY = 1;

	m_iDFMStartBufferIndex = 0;

	m_bTeachImageSave = FALSE;

	m_bDFMGrabComPortOpened = FALSE;

	m_iDFMNormalGrabBufferIdx = -1;
	m_iSEQStartBufferIndex = -1;

#ifdef INLINE_MODE
	m_hEventDFMGrabDone = CreateEventW(NULL, FALSE, FALSE, NULL);
	m_hEventCameraGrabDone = CreateEventW(NULL, FALSE, FALSE, NULL);
#endif

	m_iGrabCircularIdx = 0;
	m_iMaxCircularGrab = 1;

	for (int i = 0; i < GRAB_CIRCULAR_MAX; i++)
		m_bGrabCircularBufferCopyDone[i] = FALSE;

	m_bUseCropFov = FALSE;
	m_iCropFovLeftTopX = 0;
	m_iCropFovLeftTopY = 0;
	m_iCropFovSizeX = 1;
	m_iCropFovSizeY = 1;

	bDFMNormalGrabProcStopFlag = FALSE;
	bDFMGrabProcStopFlag = FALSE;
	bCameraGrabProcStopFlag = FALSE;
	m_bGrabIndexMismatchDetected = FALSE;
	m_iGrabIndexMismatchOffset = 0;

	iDFMGrabCountIndex = 0;
	iSeqGrabCountIndex = 0;

	m_sBayerType = "GB";

	bCreatePhSObject = FALSE;

	s_universal_avi = NULL;
}

CCameraManager::~CCameraManager(void)
{
#ifdef INLINE_MODE

	TurnOff_Specular();

	if (m_hEventDFMGrabDone)
	{
		CloseHandle(m_hEventDFMGrabDone);
	}

	if (m_hEventCameraGrabDone)
	{
		CloseHandle(m_hEventCameraGrabDone);
	}

	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[m_iVisionCamName])
	{
		if (m_bMvInit)
		{
			if (m_pcMyCamera != NULL)
			{
				m_pcMyCamera->StopGrabbing();
				m_pcMyCamera->Close();
				delete m_pcMyCamera;
				m_pcMyCamera = NULL;

				for (int i = 0; i < ALIGN_CAM_GRAB_NUMBER; i++)
				{
					if (m_MvGrabBufAddr[i])
						delete[] m_MvGrabBufAddr[i];
				}
			}
		}

		return;
	}

	MdigControl(MilDigitizer, M_GC_STREAMING_STOP, M_DEFAULT);

	int i, j, k;

	for (i = 0; i < GRAB_CIRCULAR_MAX; i++)
	{
		for (j = 0; j < MAX_GRAB_BUFFER; j++)
		{
			for (k = 0; k < 3; k++)
			{
				GenEmptyObj(&(m_hoCallBackImage[i][j][k]));
			}
		}
	}

	for (i = 0; i < m_iMaxCircularGrab; i++)
	{
		for (j = 0; j < g_iVisionMaxGrabBuffer[m_iVisionCamName]; j++)
		{
			MbufFree(MilMonoImageBuf[i][j]);
			MbufFree(MilColorImageBuf[i][j]);
		}
	}

	MbufFree(m_MilWBCoeff);

	MdigFree(MilDigitizer);

	m_bGrabDone = TRUE;

	if (m_bDFMGrabComPortOpened)
		CloseSerial(m_HDFMGrabSerialHandle);

#endif

	SAFE_DELETE(s_universal_avi);
}

void CCameraManager::IncGrabCircularIndex()
{
	++m_iGrabCircularIdx;
	if (m_iGrabCircularIdx >= m_iMaxCircularGrab)
		m_iGrabCircularIdx = 0;
}

void CCameraManager::UnInitGrabInterface()
{
#ifdef INLINE_MODE

	MdigControl(MilDigitizer, M_GC_STREAMING_STOP, M_DEFAULT);

	int i, j, k;

	for (i = 0; i < GRAB_CIRCULAR_MAX; i++)
	{
		for (j = 0; j < MAX_GRAB_BUFFER; j++)
		{
			for (k = 0; k < 3; k++)
			{
				GenEmptyObj(&(m_hoCallBackImage[i][j][k]));
			}
		}
	}

	for (i = 0; i < m_iMaxCircularGrab; i++)
	{
		for (j = 0; j < g_iVisionMaxGrabBuffer[m_iVisionCamName]; j++)
		{
			MbufFree(MilMonoImageBuf[i][j]);
			MbufFree(MilColorImageBuf[i][j]);
		}
	}

	MdigFree(MilDigitizer);

	if (m_bDFMGrabComPortOpened)
		CloseSerial(m_HDFMGrabSerialHandle);

	m_bReGrab = FALSE;

	m_bGrabDone = TRUE;

	m_iDFMStartBufferIndex = 0;

	m_bDFMGrabComPortOpened = FALSE;

	m_iDFMNormalGrabBufferIdx = -1;
	m_iSEQStartBufferIndex = -1;

	m_iGrabCircularIdx = 0;
	m_iMaxCircularGrab = 1;

	for (i = 0; i < GRAB_CIRCULAR_MAX; i++)
		m_bGrabCircularBufferCopyDone[i] = FALSE;

	m_bUseCropFov = FALSE;
	m_iCropFovLeftTopX = 0;
	m_iCropFovLeftTopY = 0;
	m_iCropFovSizeX = 1;
	m_iCropFovSizeY = 1;

	ResetDFMConvertVariable();

	bDFMNormalGrabProcStopFlag = FALSE;
	bDFMGrabProcStopFlag = FALSE;
	bCameraGrabProcStopFlag = FALSE;
	m_bGrabIndexMismatchDetected = FALSE;
	m_iGrabIndexMismatchOffset = 0;

	iDFMGrabCountIndex = 0;
	iSeqGrabCountIndex = 0;

#endif

}

void CCameraManager::SetVisionCamName(int iVisionCamName)
{
	m_iVisionCamName = iVisionCamName;
}

void CCameraManager::SetCameraFovCropInfo(BOOL bUse, int iLTX, int iLTY, int iSizeX, int iSizeY)
{
	m_bUseCropFov = bUse;
	m_iCropFovLeftTopX = iLTX;
	m_iCropFovLeftTopY = iLTY;
	m_iCropFovSizeX = iSizeX;
	m_iCropFovSizeY = iSizeY;
}

bool CCameraManager::InitGrabInterface(MIL_ID MilSystem)
{
#ifdef INLINE_MODE
	MIL_INT SizeX = 0;
	MIL_INT SizeY = 0;
	MIL_INT Band = 3;

	//////////////////////////////////////////////////////////////////////////
	// White Balance
	float WB_Coefficients[3];
	m_MilWBCoeff = MbufAlloc1d(MilSystem, 3, 32 + M_FLOAT, M_ARRAY, M_NULL);

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);

	//////////////////////////////////////////////////////////////////////////
	// Board Set
	for (int i = 0; i < 10; i++)
	{
		CString strDCFPath;
		strDCFPath.Format("\\HW\\Vision_N%d\\%s.dcf", m_iVisionCamName + 1, THEAPP.m_ModelDefineInfo.m_strVisionName[m_iVisionCamName]);

#ifdef POC_TEST
		if (m_iVisionCamName == VISION_NUMBER_1)
			MdigAlloc(MilSystem, M_DEV2, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
		else if (m_iVisionCamName == VISION_NUMBER_2)
			MdigAlloc(MilSystem, M_DEV0, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
#else
		if (m_iVisionCamName == VISION_NUMBER_1)
			MdigAlloc(MilSystem, M_DEV0, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
		else if (m_iVisionCamName == VISION_NUMBER_2)
			MdigAlloc(MilSystem, M_DEV2, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
#endif
		else if (m_iVisionCamName == VISION_NUMBER_3)
			MdigAlloc(MilSystem, M_DEV0, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
		else if (m_iVisionCamName == VISION_NUMBER_4)
			MdigAlloc(MilSystem, M_DEV2, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
		else
			return true;

		if (MilDigitizer != 0)
			break;

		Sleep(50);
	}

	MdigInquire(MilDigitizer, M_SIZE_X, &SizeX);
	MdigInquire(MilDigitizer, M_SIZE_Y, &SizeY);
	//MdigInquire(MilDigitizer, M_SIZE_BAND, &Band);

	m_iMaxCircularGrab = THEAPP.m_ModelDefineInfo.m_iVisionGrabCircular[m_iVisionCamName];

	for (int i = 0; i < m_iMaxCircularGrab; i++)
	{
		for (int j = 0; j < g_iVisionMaxGrabBuffer[m_iVisionCamName]; j++)
		{
			MbufAlloc2d(MilSystem, SizeX, SizeY, 8L + M_UNSIGNED, M_IMAGE + M_DISP + M_GRAB, &(MilMonoImageBuf[i][j]));
			MbufAllocColor(MilSystem, Band, SizeX, SizeY, 8L + M_UNSIGNED, M_IMAGE + M_GRAB + M_PLANAR + M_PROC, &(MilColorImageBuf[i][j]));		// M_IMAGE+M_GRAB
		}
	}
	//MdigControl(MilDigitizer, M_BAYER_CONVERSION, M_ENABLE);


	if (m_iVisionCamName == VISION_NUMBER_1 || m_iVisionCamName == VISION_NUMBER_2 || m_iVisionCamName == VISION_NUMBER_3 || m_iVisionCamName == VISION_NUMBER_4)
	{
		//////////////////////////////////////////////////////////////////////////
		// White Balance
		CString strWhiteBalance, strSection, strKey;
		strWhiteBalance.Format("%s\\HW\\Vision_N%d\\WhiteBalance.ini", strOpticModelFolder, m_iVisionCamName + 1);
		strSection = THEAPP.m_ModelDefineInfo.m_strVisionName[m_iVisionCamName];

		CIniFileCS INI_WhiteBalance(strWhiteBalance);
		WB_Coefficients[0] = INI_WhiteBalance.Get_Float(strSection, "R", 1.0);
		WB_Coefficients[1] = INI_WhiteBalance.Get_Float(strSection, "G", 1.0);
		WB_Coefficients[2] = INI_WhiteBalance.Get_Float(strSection, "B", 1.0);

		MbufPut1d(m_MilWBCoeff, 0L, 3L, WB_Coefficients);
		//MdigControl(MilDigitizer, M_BAYER_COEFFICIENTS_ID, MilWBCoeff);
		//MdigControl(MilDigitizer, M_WHITE_BALANCE, M_ENABLE);

		strSection = "Bayer";
		strKey = "Type";
		m_sBayerType = INI_WhiteBalance.Get_String(strSection, strKey, "GB");
		//////////////////////////////////////////////////////////////////////////

		m_iCamImageSizeX = SizeX;
		m_iCamImageSizeY = SizeY;

		for (int i = 0; i < m_iMaxCircularGrab; i++)
		{
			for (int j = 0; j < g_iVisionMaxGrabBuffer[m_iVisionCamName]; j++)
			{
				MbufClear(MilMonoImageBuf[i][j], 0);
				MbufClear(MilColorImageBuf[i][j], M_RGB888(128, 128, 128));
			}
		}

		BYTE *addr;

		MIL_ID MilImageBand;

		for (int i = 0; i < m_iMaxCircularGrab; i++)
		{
			for (int j = 0; j < g_iVisionMaxGrabBuffer[m_iVisionCamName]; j++)
			{
				MbufChildColor(MilColorImageBuf[i][j], M_RED, &MilImageBand);
				MbufInquire(MilImageBand, M_HOST_ADDRESS, &addr);
				GenImage1Extern(&(m_hoCallBackImage[i][j][0]), "byte", SizeX, SizeY, (Hlong)addr, NULL);

				MbufChildColor(MilColorImageBuf[i][j], M_GREEN, &MilImageBand);
				MbufInquire(MilImageBand, M_HOST_ADDRESS, &addr);
				GenImage1Extern(&(m_hoCallBackImage[i][j][1]), "byte", SizeX, SizeY, (Hlong)addr, NULL);

				MbufChildColor(MilColorImageBuf[i][j], M_BLUE, &MilImageBand);
				MbufInquire(MilImageBand, M_HOST_ADDRESS, &addr);
				GenImage1Extern(&(m_hoCallBackImage[i][j][2]), "byte", SizeX, SizeY, (Hlong)addr, NULL);
			}
		}

		MbufFree(MilImageBand);

		MappControl(M_ERROR, M_PRINT_DISABLE);

		//MdigControl(MilDigitizer, M_GRAB_MODE, M_ASYNCHRONOUS);
		//MdigControl(MilDigitizer, M_GRAB_TRIGGER_ACTIVATION, M_DEFAULT);
		//MdigControl(MilDigitizer, M_GRAB_TRIGGER_STATE, M_ENABLE);
		//MdigControl(MilDigitizer, M_GRAB_TRIGGER_SOURCE, M_TIMER1);
		//MdigControl(MilDigitizer, M_TIMER1 + M_TIMER_TRIGGER_SOURCE, M_AUX_IO0);	//! M_AUX_IO6 : Connector: DB-9 -Pin: 2 +Pin: 7
																	//!  M_AUX_IO6 : Connector: DBHD - 15 (0) - Pin : 9 + Pin : 15
		//MdigControl(MilDigitizer, M_TIMER1 + M_TIMER_STATE, M_ENABLE);
		//MdigControl(MilDigitizer, M_TIMER1 + M_TIMER_TRIGGER_ACTIVATION, M_EDGE_RISING);
		//MdigControl(MilDigitizer, M_TIMER_CLOCK_SOURCE, M_PIXCLK);
		//MdigControl(MilDigitizer, M_TIMER_DELAY, M_DEFAULT);

		MdigControl(MilDigitizer, M_GRAB_CONTINUOUS_END_TRIGGER, M_ENABLE);
		MdigControl(MilDigitizer, M_GC_STREAMING_MODE, M_MANUAL);
		MdigControl(MilDigitizer, M_GC_STREAMING_START, M_DEFAULT);

		/// Specular
		ResetDFMVariable();
		if (m_iVisionCamName == VISION_NUMBER_1)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bUseConvertSingleThread)
			{
				if (m_pConvertThread == NULL)
					m_pConvertThread = AfxBeginThread(DFMProcInspectionThread, this);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

#else
	m_iCamImageSizeX = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[m_iVisionCamName];
	m_iCamImageSizeY = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[m_iVisionCamName];
#endif

	Init4DVision();

	return true;
}

bool CCameraManager::InitGrabInterface_Mono(MIL_ID MilSystem)
{
#ifdef INLINE_MODE
	MIL_INT SizeX = 0;
	MIL_INT SizeY = 0;
	MIL_INT Band = 0;

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);

	//////////////////////////////////////////////////////////////////////////
	// Board Set
	for (int i = 0; i < 10; i++)
	{
		CString strDCFPath;
		strDCFPath.Format("\\HW\\Vision_N%d\\%s.dcf", m_iVisionCamName + 1, THEAPP.m_ModelDefineInfo.m_strVisionName[m_iVisionCamName]);

		if (m_iVisionCamName == VISION_NUMBER_1)
			MdigAlloc(MilSystem, M_DEV0, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
		else if (m_iVisionCamName == VISION_NUMBER_2)
			MdigAlloc(MilSystem, M_DEV2, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
		else if (m_iVisionCamName == VISION_NUMBER_3)
			MdigAlloc(MilSystem, M_DEV0, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
		else if (m_iVisionCamName == VISION_NUMBER_4)
			MdigAlloc(MilSystem, M_DEV1, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
		else if (m_iVisionCamName == VISION_NUMBER_4_2)
		{
			strDCFPath.Format("\\HW\\Vision_N%d\\%s.dcf", VISION_NUMBER_4 + 1, THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_4]);
			MdigAlloc(MilSystem, M_DEV2, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
		}
		else if (m_iVisionCamName == VISION_NUMBER_4_3)
		{
			strDCFPath.Format("\\HW\\Vision_N%d\\TopAlign.dcf", VISION_NUMBER_4 + 1);
			MdigAlloc(MilSystem, M_DEV3, strOpticModelFolder + strDCFPath, M_DEFAULT, &MilDigitizer);
		}
		else
			return true;

		if (MilDigitizer != 0)
			break;

		Sleep(50);
	}

	MdigInquire(MilDigitizer, M_SIZE_X, &SizeX);
	MdigInquire(MilDigitizer, M_SIZE_Y, &SizeY);
	MdigInquire(MilDigitizer, M_SIZE_BAND, &Band);

	m_iMaxCircularGrab = THEAPP.m_ModelDefineInfo.m_iVisionGrabCircular[m_iVisionCamName];

	for (int i = 0; i < m_iMaxCircularGrab; i++)
	{
		for (int j = 0; j < g_iVisionMaxGrabBuffer[m_iVisionCamName]; j++)
		{
			MbufAlloc2d(MilSystem, SizeX, SizeY, 8L + M_UNSIGNED, M_IMAGE + M_DISP + M_GRAB, &(MilMonoImageBuf[i][j]));
		}
	}

	if (m_iVisionCamName == VISION_NUMBER_1 || m_iVisionCamName == VISION_NUMBER_2 || m_iVisionCamName == VISION_NUMBER_3 || m_iVisionCamName == VISION_NUMBER_4)
	{
		m_iCamImageSizeX = SizeX;
		m_iCamImageSizeY = SizeY;

		for (int i = 0; i < m_iMaxCircularGrab; i++)
		{
			for (int j = 0; j < g_iVisionMaxGrabBuffer[m_iVisionCamName]; j++)
			{
				MbufClear(MilMonoImageBuf[i][j], 128);
			}
		}

		BYTE *addr;

		for (int i = 0; i < m_iMaxCircularGrab; i++)
		{
			for (int j = 0; j < g_iVisionMaxGrabBuffer[m_iVisionCamName]; j++)
			{
				MbufInquire(MilMonoImageBuf[i][j], M_HOST_ADDRESS, &addr);
				GenImage1Extern(&(m_hoCallBackImage[i][j][0]), "byte", SizeX, SizeY, (Hlong)addr, NULL);
			}
		}

		MappControl(M_ERROR, M_PRINT_DISABLE);
		MdigControl(MilDigitizer, M_GRAB_CONTINUOUS_END_TRIGGER, M_ENABLE);
		MdigControl(MilDigitizer, M_GC_STREAMING_MODE, M_MANUAL);
		MdigControl(MilDigitizer, M_GC_STREAMING_START, M_DEFAULT);
	}
	//////////////////////////////////////////////////////////////////////////

#else
	m_iCamImageSizeX = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[m_iVisionCamName];
	m_iCamImageSizeY = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[m_iVisionCamName];
#endif

	return true;
}

void CCameraManager::UpdateWhiteBalance(int iVisionNo)
{
	//////////////////////////////////////////////////////////////////////////
	// White Balance
	MbufFree(m_MilWBCoeff);

	float WB_Coefficients[3];
	if (iVisionNo < 2)
		m_MilWBCoeff = MbufAlloc1d(THEAPP.m_MilSystem[0], 3, 32 + M_FLOAT, M_ARRAY, M_NULL);
	else
		m_MilWBCoeff = MbufAlloc1d(THEAPP.m_MilSystem[1], 3, 32 + M_FLOAT, M_ARRAY, M_NULL);

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);

	CString strWhiteBalance, strSection, strKey;
	strWhiteBalance.Format("%s\\HW\\Vision_N%d\\WhiteBalance.ini", strOpticModelFolder, m_iVisionCamName + 1);
	strSection = THEAPP.m_ModelDefineInfo.m_strVisionName[m_iVisionCamName];

	CIniFileCS INI_WhiteBalance(strWhiteBalance);
	WB_Coefficients[0] = INI_WhiteBalance.Get_Float(strSection, "R", 1.0);
	WB_Coefficients[1] = INI_WhiteBalance.Get_Float(strSection, "G", 1.0);
	WB_Coefficients[2] = INI_WhiteBalance.Get_Float(strSection, "B", 1.0);

	MbufPut1d(m_MilWBCoeff, 0L, 3L, WB_Coefficients);
	//MdigControl(MilDigitizer, M_BAYER_COEFFICIENTS_ID, MilWBCoeff);
	//MdigControl(MilDigitizer, M_WHITE_BALANCE, M_ENABLE);

	strSection = "Bayer";
	strKey = "Type";
	m_sBayerType = INI_WhiteBalance.Get_String(strSection, strKey, "GB");
	//////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////
//////			2D Vision (Grab Sequence)
/////////////////////////////////////////////////////

void CCameraManager::GrabErrorPostProcess()
{
	MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
	m_bReGrab = TRUE;
	m_bGrabDone = TRUE;
}

void CCameraManager::MdigProcessGrabErrorPostProcess(int iGrabStartBufferIndex, int iNoGrabImage)
{
	if (bCameraGrabProcStopFlag == FALSE)
	{
		bCameraGrabProcStopFlag = TRUE;
		MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
		MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIndex, iNoGrabImage, M_STOP, M_DEFAULT, CameraGrabProcFunc, this);
	}

	m_bReGrab = TRUE;
	m_bGrabDone = TRUE;
}

/////////////////////////////////////////////////////
//////			Align Vision (SW Trigger)
/////////////////////////////////////////////////////

void __stdcall ImageCallBackEx(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
	if (pData && pFrameInfo)
	{
		CCameraManager* pCameraManager = (CCameraManager*)pUser;
		memcpy(pCameraManager->m_MvGrabBufAddr[pCameraManager->GetMvCamGrabIndex()], pData, pFrameInfo->nWidth * pFrameInfo->nHeight);
		pCameraManager->m_bMvGrabDone = TRUE;
	}
}

bool CCameraManager::InitMvEthernetCam(CString sCameraSerial)
{
	CString sTemp, strErr;
#ifdef INLINE_MODE
	int nRet = CMvCamera::EnumDevices(MV_GIGE_DEVICE, &m_stDevList);
	if (MV_OK != nRet)
		return false;

	//CMvCamera::GetDeviceInfo
	//
	strLog.Format("MV Camera No: %d \n", m_stDevList.nDeviceNum);
	THEAPP.m_log_system->debug("{}", LOG_CSTR(strLog));
	if (sCameraSerial == "1") return false;
	//
	for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++)
	{
		//if (i != iEthernetCamIdx)
		//	continue;

		MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
		sTemp = pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber;

		strLog.Format("Device name : %s \n ", sTemp);
		THEAPP.m_log_system->debug("{}", LOG_CSTR(strLog));

		// if (sTemp != sCameraSerial) continue;
		if (pDeviceInfo == NULL)
			continue;

		m_pcMyCamera = new CMvCamera;
		if (m_pcMyCamera == NULL)
			return false;

		int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[i]);
		if (MV_OK != nRet)
		{
			delete m_pcMyCamera;
			m_pcMyCamera = NULL;
			return false;
		}

		if (m_stDevList.pDeviceInfo[i]->nTLayerType == MV_GIGE_DEVICE)
		{
			unsigned int nPacketSize = 0;
			nRet = m_pcMyCamera->GetOptimalPacketSize(&nPacketSize);
			if (nRet == MV_OK)
			{
				nRet = m_pcMyCamera->SetIntValue("GevSCPSPacketSize", nPacketSize);
				if (nRet != MV_OK)
					return false;
			}
			else
				return false;

			m_pcMyCamera->SetEnumValue("TriggerMode", 1);								// 0: Continuous 1:Trigger
			m_pcMyCamera->SetEnumValue("TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);	//Software Trigger
			nRet = m_pcMyCamera->RegisterImageCallBack(ImageCallBackEx, this);			//CallBack Function;
			if (MV_OK != nRet)
				return false;

			MVCC_INTVALUE_EX stParam = { 0 };
			int SizeX = 0;
			int SizeY = 0;

			m_pcMyCamera->GetIntValue("Width", &stParam);
			SizeX = stParam.nCurValue;

			m_pcMyCamera->GetIntValue("Height", &stParam);
			SizeY = stParam.nCurValue;

			m_iCamImageSizeX = SizeX;
			m_iCamImageSizeY = SizeY;

			for (i = 0; i < ALIGN_CAM_GRAB_NUMBER; i++)
			{
				m_MvGrabBufAddr[i] = new BYTE[SizeX * SizeY];
				GenImage1Extern(&(m_MvhoCallBackImage[i]), "byte", SizeX, SizeY, (Hlong)m_MvGrabBufAddr[i], NULL);
			}

			int nRet = m_pcMyCamera->StartGrabbing();
			if (MV_OK != nRet)
				return false;
		}
	}

	m_bMvInit = TRUE;
#else

	m_iCamImageSizeX = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[m_iVisionCamName];
	m_iCamImageSizeY = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[m_iVisionCamName];

#endif

	return true;
}

BOOL CCameraManager::GrabEthernetCamSWTrg(int iGrabBufIdx)
{
	try
	{
		DWORD dwGrabStart = 0, dwGrabEnd = 0;
		DWORD dwGrabElapsedTime = 0;

		m_bGrabDone = FALSE;
		m_bReGrab = FALSE;

		m_bMvGrabDone = FALSE;
		m_iMvGrabIndex = iGrabBufIdx;

		if (m_pcMyCamera)
			m_pcMyCamera->CommandExecute("TriggerSoftware");

		dwGrabStart = GetTickCount();

		while (1)
		{
			if (m_bMvGrabDone == TRUE)
			{
				if (THEAPP.m_pGFunction->ValidHImage(m_MvhoCallBackImage[iGrabBufIdx]))
				{
					if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)								// 티칭화면이면 ....
					{
						Sleep(100);

						CopyImage(m_MvhoCallBackImage[iGrabBufIdx], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
						CopyImage(m_MvhoCallBackImage[iGrabBufIdx], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
						CopyImage(m_MvhoCallBackImage[iGrabBufIdx], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));
						Compose3(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
						CopyImage(m_MvhoCallBackImage[iGrabBufIdx], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]));
						CopyImage(m_MvhoCallBackImage[iGrabBufIdx], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]));
						CopyImage(m_MvhoCallBackImage[iGrabBufIdx], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));

						THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);

						if (GetTeachImageSave())
							THEAPP.m_pInspectAdminViewDlg->TeachingImageGrabSave();								// 티칭 이미지 저장  => 티칭화면에 있어야만 저장됨.
					}

					m_bGrabDone = TRUE;

					return TRUE;
				}
				else
				{
					m_bReGrab = TRUE;
					m_bGrabDone = TRUE;

					return FALSE;
				}

				break;
			}

			dwGrabEnd = GetTickCount();

			if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
			{
				break;
			}

			Sleep(1);
		}

		return FALSE;
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

		strLog.Format("Halcon Exception [CCameraManager GrabEthernetCamSWTrg] : <%s>%s\n", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		strLog.Format("SW Trg grab fail");
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));
		m_bReGrab = TRUE;
		m_bGrabDone = TRUE;

		return FALSE;
	}
}


BOOL CCameraManager::CameraStartGrab(int iGrabStartBufferIdx, int iSeqAddressIndex, int iNoGrabImage, BOOL bAutoSetting)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[GetVisionCamName()];

	m_bGrabDone = FALSE;
	m_bReGrab = FALSE;

	m_iSEQStartBufferIndex = iGrabStartBufferIdx;

	m_nProcessCount = 0;
	m_nSEQTargetCount = iNoGrabImage;

	bCameraGrabProcStopFlag = FALSE;
	iSeqGrabCountIndex = 0;

	MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, iNoGrabImage, M_START, M_DEFAULT, CameraGrabProcFunc, this);

	// Speedup
	Sleep(1);

	if (!bAutoSetting)
		THEAPP.m_pTriggerManager->GrabSeqFireTriggerByEthernet(GetVisionCamName(), iSeqAddressIndex);
	else
		THEAPP.m_pTriggerManager->GrabFireTriggerByEthernet(GetVisionCamName());

	DWORD ret;

	ret = WaitForSingleObject(m_hEventCameraGrabDone, THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime);

	if (ret == WAIT_FAILED || ret == WAIT_TIMEOUT)
	{
		strLog.Format("%s/ SEQ grabdone timeout", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		if (bCameraGrabProcStopFlag == FALSE)
		{
			bCameraGrabProcStopFlag = TRUE;
			MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
			MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + m_iSEQStartBufferIndex, m_nSEQTargetCount, M_STOP, M_DEFAULT, CameraGrabProcFunc, this);
		}

		strLog.Format("%s/ MdigProcess STOP (SEQ)", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}

	return TRUE;
}

BOOL CCameraManager::CameraStartGrab_NoSeq(int iGrabStartBufferIdx, int iLightPageIdx)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[GetVisionCamName()];

	m_bGrabDone = FALSE;
	m_bReGrab = FALSE;

	m_iSEQStartBufferIndex = iGrabStartBufferIdx;

	m_nProcessCount = 0;
	m_nSEQTargetCount = 1;

	bCameraGrabProcStopFlag = FALSE;
	iSeqGrabCountIndex = 0;

	MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, 1, M_START, M_DEFAULT, CameraGrabProcFunc, this);

	// Speedup
	Sleep(1);

	THEAPP.m_pTriggerManager->FireTrigger(GetVisionCamName(), iLightPageIdx);

	DWORD ret;

	ret = WaitForSingleObject(m_hEventCameraGrabDone, THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime);

	if (ret == WAIT_FAILED || ret == WAIT_TIMEOUT)
	{
		strLog.Format("%s/ SEQ grabdone timeout", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		if (bCameraGrabProcStopFlag == FALSE)
		{
			bCameraGrabProcStopFlag = TRUE;
			MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
			MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + m_iSEQStartBufferIndex, m_nSEQTargetCount, M_STOP, M_DEFAULT, CameraGrabProcFunc, this);
		}

		strLog.Format("%s/ MdigProcess STOP (SEQ)", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}

	return TRUE;
}

BOOL CCameraManager::AutoRunCameraStartGrab(int iGrabStartBufferIdx, int iNoGrabImage)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[GetVisionCamName()];

	m_bGrabDone = FALSE;
	m_bReGrab = FALSE;

	m_iSEQStartBufferIndex = iGrabStartBufferIdx;

	m_nProcessCount = 0;
	m_nSEQTargetCount = iNoGrabImage;

	bCameraGrabProcStopFlag = FALSE;
	iSeqGrabCountIndex = 0;

	MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, iNoGrabImage, M_START, M_DEFAULT, CameraGrabProcFunc, this);

	// Speedup
	Sleep(1);

	THEAPP.m_pTriggerManager->GrabFireTriggerByEthernet(GetVisionCamName());

	DWORD ret;

	ret = WaitForSingleObject(m_hEventCameraGrabDone, THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime);

	if (ret == WAIT_FAILED || ret == WAIT_TIMEOUT)
	{
		strLog.Format("%s/ SEQ grabdone timeout", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		if (bCameraGrabProcStopFlag == FALSE)
		{
			bCameraGrabProcStopFlag = TRUE;
			MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
			MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + m_iSEQStartBufferIndex, m_nSEQTargetCount, M_STOP, M_DEFAULT, CameraGrabProcFunc, this);
		}

		strLog.Format("%s/ MdigProcess STOP (SEQ)", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}

	return TRUE;
}

BOOL CCameraManager::AutoRunCameraGrab_OneGrabFunction_Start(int iGrabStartBufferIdx, int iNoGrabImage)
{
	MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, iNoGrabImage, M_START, M_DEFAULT, CameraGrabProcFunc_OneGrabFunction, this);

	return TRUE;
}

BOOL CCameraManager::AutoRunCameraGrab_OneGrabFunction_Stop(int iGrabStartBufferIdx, int iNoGrabImage)
{
	MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, iNoGrabImage, M_STOP, M_DEFAULT, CameraGrabProcFunc_OneGrabFunction, this);

	return TRUE;
}

BOOL CCameraManager::AutoRunCameraGrab_OneGrabFunction(int iGrabStartBufferIdx, int iNoGrabImage)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[GetVisionCamName()];

	m_bGrabDone = FALSE;
	m_bReGrab = FALSE;

	m_iSEQStartBufferIndex = iGrabStartBufferIdx;

	m_nProcessCount = 0;
	m_nSEQTargetCount = iNoGrabImage;

	bCameraGrabProcStopFlag = FALSE;
	iSeqGrabCountIndex = iGrabStartBufferIdx;

	THEAPP.m_pTriggerManager->GrabFireTriggerByEthernet(GetVisionCamName());

	DWORD ret;

	ret = WaitForSingleObject(m_hEventCameraGrabDone, THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime);

	if (ret == WAIT_FAILED || ret == WAIT_TIMEOUT)
	{
		strLog.Format("%s/ SEQ grabdone timeout", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		m_bGrabIndexMismatchDetected = TRUE;

		if (bCameraGrabProcStopFlag == FALSE)
		{
			bCameraGrabProcStopFlag = TRUE;
			MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
		}

		strLog.Format("%s/ MdigProcess STOP (SEQ)", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}

	return TRUE;
}

BOOL CCameraManager::AutoRunCameraGrab_SingleLens(int iGrabStartBufferIdx, int iNoGrabImage, int iVisionGrabPeriodMsec, int iDualModelData, int iPcVisionNo)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[GetVisionCamName()];

	m_bGrabDone = FALSE;
	m_bReGrab = FALSE;

	m_iSEQStartBufferIndex = iGrabStartBufferIdx;

	m_nProcessCount = 0;
	m_nSEQTargetCount = iNoGrabImage;

	bCameraGrabProcStopFlag = FALSE;
	iSeqGrabCountIndex = iGrabStartBufferIdx;
	
	auto time_start = std::chrono::high_resolution_clock::now();
	auto time_end = std::chrono::high_resolution_clock::now();
	auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();

	int iGrabCnt = 0;

	while (1)
	{
		if (iGrabCnt >= iNoGrabImage)
			break;

		time_start = std::chrono::high_resolution_clock::now();

		THEAPP.m_pTriggerManager->FireTrigger(GetVisionCamName(), THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iLightPageIdx[iGrabStartBufferIdx+ iGrabCnt]);

		while (1)
		{
			time_end = std::chrono::high_resolution_clock::now();
			time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
			if (time_ms >= iVisionGrabPeriodMsec)
				break;

			Sleep(1);
		}

		++iGrabCnt;
	}

	DWORD ret;

	ret = WaitForSingleObject(m_hEventCameraGrabDone, THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime);

	if (ret == WAIT_FAILED || ret == WAIT_TIMEOUT)
	{
		strLog.Format("%s/ SEQ grabdone timeout", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		m_bGrabIndexMismatchDetected = TRUE;

		if (bCameraGrabProcStopFlag == FALSE)
		{
			bCameraGrabProcStopFlag = TRUE;
			MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
		}

		strLog.Format("%s/ MdigProcess STOP (SEQ)", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////
//////			Specular Vision
/////////////////////////////////////////////////////

void CCameraManager::OpenPortDFM()
{
	try
	{
		CString strCOM;
		CloseSerial(m_HDFMGrabSerialHandle);
		strCOM.Format("COM%d", THEAPP.Struct_PreferenceStruct.m_iDFMGrabComPortNumber);
		OpenSerial(HTuple(strCOM), &m_HDFMGrabSerialHandle);
		m_bDFMGrabComPortOpened = TRUE;
		SetSerialParam(m_HDFMGrabSerialHandle, DFM_BAUD_RATE, 8, "none", "none", 1, 1000, "unchanged");

		strLog.Format("COM port re-open success");
		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));
	}
	catch (HException &except)
	{
		strLog.Format("COM port re-open fail");
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));
	}
}

MIL_INT MFTYPE CCameraManager::DFMGrabProcFunc(MIL_INT HookType, MIL_ID HookId, void* HookDataPtr)
{
	CString strLog;

	CCameraManager* pCameraManager = (CCameraManager*)HookDataPtr;

	MIL_INT BufIndex = 0;
	MdigGetHookInfo(HookId, M_MODIFIED_BUFFER + M_BUFFER_INDEX, &BufIndex);

	if (BufIndex != pCameraManager->iDFMGrabCountIndex)
		return 0;

	++(pCameraManager->iDFMGrabCountIndex);

	strLog.Format("DFM Grab Buffer Idx = %d", BufIndex);
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	if (BufIndex == (pCameraManager->m_nDFMTargetCount - 1))
	{
		int iGrabCircularIdx = pCameraManager->GetGrabCircularIndex();

		if (pCameraManager->bDFMGrabProcStopFlag == FALSE)
		{
			pCameraManager->bDFMGrabProcStopFlag = TRUE;
			MdigProcess(pCameraManager->MilDigitizer, pCameraManager->MilMonoImageBuf[iGrabCircularIdx] + pCameraManager->m_iDFMStartBufferIndex, SPV_RAW_IMAGE_NUMBER, M_STOP, M_DEFAULT, DFMGrabProcFunc, pCameraManager);
		}

		strLog.Format("MdigProcess STOP(DFM), iGrabCircularIdx: %d, m_iDFMStartBufferIndex: %d", iGrabCircularIdx, pCameraManager->m_iDFMStartBufferIndex);
		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

		SetEvent(pCameraManager->m_hEventDFMGrabDone);
	}

	return 0;
}

MIL_INT MFTYPE CCameraManager::DFMNormalGrabProcFunc(MIL_INT HookType, MIL_ID HookId, void* HookDataPtr)
{
	CString strLog;

	CCameraManager* pCameraManager = (CCameraManager*)HookDataPtr;

	MIL_INT BufIndex = 0;
	MdigGetHookInfo(HookId, M_MODIFIED_BUFFER + M_BUFFER_INDEX, &BufIndex);

	if (BufIndex != pCameraManager->iDFMGrabCountIndex)
		return 0;

	++(pCameraManager->iDFMGrabCountIndex);

	strLog.Format("Grab Buffer Idx = %d", BufIndex);
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	if (BufIndex == (pCameraManager->m_nDFMTargetCount - 1))
	{
		int iGrabCircularIdx = pCameraManager->GetGrabCircularIndex();

		try
		{
			if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)								// 티칭화면이면 ....
			{
				CopyImage(pCameraManager->m_hoCallBackImage[iGrabCircularIdx][pCameraManager->m_iDFMNormalGrabBufferIdx][0], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
				CopyImage(pCameraManager->m_hoCallBackImage[iGrabCircularIdx][pCameraManager->m_iDFMNormalGrabBufferIdx][1], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
				CopyImage(pCameraManager->m_hoCallBackImage[iGrabCircularIdx][pCameraManager->m_iDFMNormalGrabBufferIdx][2], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));
				Compose3(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
				TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B],
					&(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), "hsi");
				Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));

				THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);

				if (pCameraManager->GetTeachImageSave())
					THEAPP.m_pInspectAdminViewDlg->TeachingImageGrabSave();								// 티칭 이미지 저장  => 티칭화면에 있어야만 저장됨.
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

			strLog.Format("Halcon Exception [CameraManager CallBack [DFMNormalGrabProcFunc] : <%s>%s", sOperatorName, sErrMsg);
			THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
		}

		if (pCameraManager->bDFMNormalGrabProcStopFlag == FALSE)
		{
			pCameraManager->bDFMNormalGrabProcStopFlag = TRUE;
			MdigProcess(pCameraManager->MilDigitizer, pCameraManager->MilMonoImageBuf[iGrabCircularIdx] + pCameraManager->m_iDFMNormalGrabBufferIdx, 1, M_STOP, M_DEFAULT, DFMNormalGrabProcFunc, pCameraManager);
		}
		strLog.Format("MdigProcess STOP(Normal)");
		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

		SetEvent(pCameraManager->m_hEventDFMGrabDone);
	}

	return 0;
}

MIL_INT MFTYPE CCameraManager::CameraGrabProcFunc(MIL_INT HookType, MIL_ID HookId, void* HookDataPtr)
{
	CString strLog;

	CCameraManager* pCameraManager = (CCameraManager*)HookDataPtr;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[pCameraManager->GetVisionCamName()];

	MIL_ID DigitigerID = 0;
	MdigGetHookInfo(HookId, M_DIGITIZER_ID, &DigitigerID);
	if (DigitigerID != pCameraManager->MilDigitizer)
	{
		strLog.Format("%s/ Wrong SEQ Digitiger ID", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

	MIL_INT BufIndex = 0;
	MdigGetHookInfo(HookId, M_MODIFIED_BUFFER + M_BUFFER_INDEX, &BufIndex);

	if (BufIndex != pCameraManager->iSeqGrabCountIndex)
	{
		strLog.Format("%s/ Wrong SEQ grab buffer idx: %d", sVisionCamType_Short, BufIndex);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

	++(pCameraManager->iSeqGrabCountIndex);

	strLog.Format("%s/ SEQ grab buffer idx: %d", sVisionCamType_Short, BufIndex);
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	if (BufIndex == (pCameraManager->m_nSEQTargetCount - 1))
	{
		int iGrabCircularIdx = pCameraManager->GetGrabCircularIndex();

		if (pCameraManager->bCameraGrabProcStopFlag == FALSE)
		{
			pCameraManager->bCameraGrabProcStopFlag = TRUE;
			MdigProcess(pCameraManager->MilDigitizer, pCameraManager->MilMonoImageBuf[iGrabCircularIdx] + pCameraManager->m_iSEQStartBufferIndex, pCameraManager->m_nSEQTargetCount, M_STOP, M_DEFAULT, CameraGrabProcFunc, pCameraManager);
		}

		pCameraManager->m_bGrabDone = TRUE;

		strLog.Format("%s/ MdigProcess STOP(SEQ), iGrabCircularIdx: %d, m_iSEQStartBufferIndex: %d", sVisionCamType_Short, iGrabCircularIdx, pCameraManager->m_iSEQStartBufferIndex);
		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

		SetEvent(pCameraManager->m_hEventCameraGrabDone);
	}

	return 0;
}

MIL_INT MFTYPE CCameraManager::CameraGrabProcFunc_OneGrabFunction(MIL_INT HookType, MIL_ID HookId, void* HookDataPtr)
{
	CString strLog;

	CCameraManager* pCameraManager = (CCameraManager*)HookDataPtr;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[pCameraManager->GetVisionCamName()];

	MIL_ID DigitigerID = 0;
	MdigGetHookInfo(HookId, M_DIGITIZER_ID, &DigitigerID);
	if (DigitigerID != pCameraManager->MilDigitizer)
	{
		strLog.Format("%s/ Wrong SEQ Digitiger ID", sVisionCamType_Short);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

	MIL_INT BufIndex = 0;
	MdigGetHookInfo(HookId, M_MODIFIED_BUFFER + M_BUFFER_INDEX, &BufIndex);

	if (pCameraManager->m_bGrabIndexMismatchDetected && BufIndex != pCameraManager->iSeqGrabCountIndex)
	{
		strLog.Format("%s/ Buffer index mismatch detected, iSeqGrabCountIndex from %d to %d",
			sVisionCamType_Short, pCameraManager->iSeqGrabCountIndex, BufIndex);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		pCameraManager->m_iGrabIndexMismatchOffset = BufIndex - pCameraManager->iSeqGrabCountIndex;
		pCameraManager->iSeqGrabCountIndex = BufIndex;
	}

	if (BufIndex != pCameraManager->iSeqGrabCountIndex)
	{
		strLog.Format("%s/ Wrong SEQ grab buffer idx: %d, iSeqGrabCountIndex: %d, HookType: %d", sVisionCamType_Short, BufIndex, pCameraManager->iSeqGrabCountIndex, HookType);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

	++(pCameraManager->iSeqGrabCountIndex);

	strLog.Format("%s/ Current Buffer: %d, Target Buffer: %d, BufIndex: %d, GrabIndexMismatchOffset: %d, SEQStartBuffferIndex: %d, SEQTargetCount: %d", sVisionCamType_Short, BufIndex - pCameraManager->m_iGrabIndexMismatchOffset, pCameraManager->m_iSEQStartBufferIndex + pCameraManager->m_nSEQTargetCount - 1, BufIndex, pCameraManager->m_iGrabIndexMismatchOffset, pCameraManager->m_iSEQStartBufferIndex, pCameraManager->m_nSEQTargetCount);
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	if (BufIndex - pCameraManager->m_iGrabIndexMismatchOffset == (pCameraManager->m_iSEQStartBufferIndex + pCameraManager->m_nSEQTargetCount - 1))
	{
		int iGrabCircularIdx = pCameraManager->GetGrabCircularIndex();

		if (pCameraManager->bCameraGrabProcStopFlag == FALSE)
		{
			pCameraManager->bCameraGrabProcStopFlag = TRUE;
		}

		pCameraManager->m_bGrabDone = TRUE;

		strLog.Format("%s/ MdigProcess STOP(SEQ), iGrabCircularIdx: %d, m_iSEQStartBufferIndex: %d, HookType: %d", sVisionCamType_Short, iGrabCircularIdx, pCameraManager->m_iSEQStartBufferIndex, HookType);
		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

		SetEvent(pCameraManager->m_hEventCameraGrabDone);
	}

	return 0;
}

int CCameraManager::DFMStartNormalGrab(int iGrabBufferIdx, int iImageIndex, BOOL bUseSpecularLight, BOOL bSingleDualGrabMode)
{
	HTuple HPageCommand;
	CString sPageIndex;

	if (bUseSpecularLight)
	{
		sPageIndex.Format("aA\n");
	}
	else
	{
		sPageIndex.Format("LA%d\n", THEAPP.m_pModelDataManager->m_iLightPageIdx[iImageIndex]);
	}

	HPageCommand = HTuple(sPageIndex);

	int iGrabSuccess = DFM_RET_NO_ERROR;

	m_iDFMNormalGrabBufferIdx = iGrabBufferIdx;

	m_nProcessCount = 0;
	m_nDFMTargetCount = 1;

	strLog.Format("MdigProcess START(Normal)");
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	bDFMNormalGrabProcStopFlag = FALSE;
	iDFMGrabCountIndex = 0;

	MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabBufferIdx, 1, M_START, M_DEFAULT, DFMNormalGrabProcFunc, this);

	// Speedup
	Sleep(1);

	try
	{
		WriteSerial(m_HDFMGrabSerialHandle, HPageCommand.TupleOrds());
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

		strLog.Format("Halcon Exception [CameraManager::DFMStartNormalGrab / Master] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		try
		{
			CString strCOM;
			strCOM.Format("COM%d", THEAPP.Struct_PreferenceStruct.m_iDFMGrabComPortNumber);

			OpenSerial(HTuple(strCOM), &m_HDFMGrabSerialHandle);
			SetSerialParam(m_HDFMGrabSerialHandle, DFM_BAUD_RATE, 8, "none", "none", 1, 1000, "unchanged");

			m_bDFMGrabComPortOpened = TRUE;

			strLog.Format("COM port re-open success");
			THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

			if (bDFMNormalGrabProcStopFlag == FALSE)
			{
				bDFMNormalGrabProcStopFlag = TRUE;
				MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
				MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabBufferIdx, 1, M_STOP, M_DEFAULT, DFMNormalGrabProcFunc, this);
			}

			return DFM_RET_GRABDONE_ERROR;
		}
		catch (HException &except)
		{
			m_bDFMGrabComPortOpened = FALSE;

			strLog.Format("COM port re-open fail");
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

			if (bDFMNormalGrabProcStopFlag == FALSE)
			{
				bDFMNormalGrabProcStopFlag = TRUE;
				MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
				MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabBufferIdx, 1, M_STOP, M_DEFAULT, DFMNormalGrabProcFunc, this);
			}

			return DFM_RET_PORT_ERROR;
		}
	}

	if (bUseSpecularLight)
	{
		try
		{
			sPageIndex.Format("aB\n");
			HPageCommand = HTuple(sPageIndex);

			WriteSerial(m_HDFMGrabSerialHandle, HPageCommand.TupleOrds());
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

			strLog.Format("Halcon Exception [CameraManager::DFMStartNormalGrab / Slave] : <%s>%s", sOperatorName, sErrMsg);
			THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

			try
			{
				CString strCOM;
				strCOM.Format("COM%d", THEAPP.Struct_PreferenceStruct.m_iDFMGrabComPortNumber);

				OpenSerial(HTuple(strCOM), &m_HDFMGrabSerialHandle);
				SetSerialParam(m_HDFMGrabSerialHandle, DFM_BAUD_RATE, 8, "none", "none", 1, 1000, "unchanged");

				m_bDFMGrabComPortOpened = TRUE;

				strLog.Format("COM Port re-open success");
				THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

				if (bDFMNormalGrabProcStopFlag == FALSE)
				{
					bDFMNormalGrabProcStopFlag = TRUE;
					MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
					MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabBufferIdx, 1, M_STOP, M_DEFAULT, DFMNormalGrabProcFunc, this);
				}

				return DFM_RET_GRABDONE_ERROR;
			}
			catch (HException &except)
			{
				m_bDFMGrabComPortOpened = FALSE;

				strLog.Format("COM Port re-open fail");
				THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

				if (bDFMNormalGrabProcStopFlag == FALSE)
				{
					bDFMNormalGrabProcStopFlag = TRUE;
					MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
					MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabBufferIdx, 1, M_STOP, M_DEFAULT, DFMNormalGrabProcFunc, this);
				}

				return DFM_RET_PORT_ERROR;
			}
		}
	}

	DWORD ret;

	if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
		ret = WaitForSingleObject(m_hEventDFMGrabDone, DFM_TEACHING_MODE_GRAB_DONE_WAIT_TIME);
	else
		ret = WaitForSingleObject(m_hEventDFMGrabDone, THEAPP.Struct_PreferenceStruct.m_iDFMGrabDoneWaitTime);

	TurnOff_Specular();

	if (ret == WAIT_FAILED || ret == WAIT_TIMEOUT) {

		strLog.Format("DFM Normal grabdone timeout");
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		if (bDFMNormalGrabProcStopFlag == FALSE)
		{
			bDFMNormalGrabProcStopFlag = TRUE;
			MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
			MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabBufferIdx, 1, M_STOP, M_DEFAULT, DFMNormalGrabProcFunc, this);
		}

		strLog.Format("MdigProcess STOP (DFM)");
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		iGrabSuccess = DFM_RET_GRABDONE_ERROR;
		return iGrabSuccess;
	}

	return iGrabSuccess;
}

int CCameraManager::DFMStartGrab(int iGrabStartBufferIdx, BOOL bSingleDualGrabMode)
{
	int iGrabSuccess = DFM_RET_NO_ERROR;

#ifdef USE_CUDA
	m_iDFMStartBufferIndex = iGrabStartBufferIdx;

	m_nProcessCount = 0;
	m_nDFMTargetCount = SPV_RAW_IMAGE_NUMBER;
	strLog.Format("MdigProcess START (DFM)");
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	bDFMGrabProcStopFlag = FALSE;
	iDFMGrabCountIndex = 0;

	MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, SPV_RAW_IMAGE_NUMBER, M_START, M_DEFAULT, DFMGrabProcFunc, this);

	Sleep(1);

	try
	{
		WriteSerial(m_HDFMGrabSerialHandle, HTuple("9A\n").TupleOrds());
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

		strLog.Format("Halcon Exception [CameraManager::DFMStartGrab / Master] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		try
		{
			CString strCOM;
			strCOM.Format("COM%d", THEAPP.Struct_PreferenceStruct.m_iDFMGrabComPortNumber);

			OpenSerial(HTuple(strCOM), &m_HDFMGrabSerialHandle);
			SetSerialParam(m_HDFMGrabSerialHandle, DFM_BAUD_RATE, 8, "none", "none", 1, 1000, "unchanged");

			m_bDFMGrabComPortOpened = TRUE;

			strLog.Format("COM Port re-open success");
			THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

			if (bDFMGrabProcStopFlag == FALSE)
			{
				bDFMGrabProcStopFlag = TRUE;
				MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
				MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, SPV_RAW_IMAGE_NUMBER, M_STOP, M_DEFAULT, DFMGrabProcFunc, this);
			}

			return DFM_RET_GRABDONE_ERROR;
		}
		catch (HException &except)
		{
			m_bDFMGrabComPortOpened = FALSE;

			strLog.Format("COM Port re-open fail");
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

			if (bDFMGrabProcStopFlag == FALSE)
			{
				bDFMGrabProcStopFlag = TRUE;
				MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
				MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, SPV_RAW_IMAGE_NUMBER, M_STOP, M_DEFAULT, DFMGrabProcFunc, this);
			}

			return DFM_RET_PORT_ERROR;
		}
	}

	try
	{
		WriteSerial(m_HDFMGrabSerialHandle, HTuple("9B\n").TupleOrds());
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

		strLog.Format("Halcon Exception [CameraManager::DFMStartGrab / Master] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		try
		{
			CString strCOM;
			strCOM.Format("COM%d", THEAPP.Struct_PreferenceStruct.m_iDFMGrabComPortNumber);

			OpenSerial(HTuple(strCOM), &m_HDFMGrabSerialHandle);
			SetSerialParam(m_HDFMGrabSerialHandle, DFM_BAUD_RATE, 8, "none", "none", 1, 1000, "unchanged");

			m_bDFMGrabComPortOpened = TRUE;

			strLog.Format("COM Port re-open success");
			THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

			if (bDFMGrabProcStopFlag == FALSE)
			{
				bDFMGrabProcStopFlag = TRUE;
				MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
				MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, SPV_RAW_IMAGE_NUMBER, M_STOP, M_DEFAULT, DFMGrabProcFunc, this);
			}

			return DFM_RET_GRABDONE_ERROR;
		}
		catch (HException &except)
		{
			m_bDFMGrabComPortOpened = FALSE;

			strLog.Format("COM Port re-open fail");
			THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

			if (bDFMGrabProcStopFlag == FALSE)
			{
				bDFMGrabProcStopFlag = TRUE;
				MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
				MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, SPV_RAW_IMAGE_NUMBER, M_STOP, M_DEFAULT, DFMGrabProcFunc, this);
			}

			return DFM_RET_PORT_ERROR;
		}
	}

	DWORD ret;

	if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
		ret = WaitForSingleObject(m_hEventDFMGrabDone, DFM_TEACHING_MODE_GRAB_DONE_WAIT_TIME);
	else
		ret = WaitForSingleObject(m_hEventDFMGrabDone, THEAPP.Struct_PreferenceStruct.m_iDFMGrabDoneWaitTime);

	if (ret == WAIT_FAILED || ret == WAIT_TIMEOUT) {

		strLog.Format("DFM grabdone timeout");
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		if (bDFMGrabProcStopFlag == FALSE)
		{
			bDFMGrabProcStopFlag = TRUE;
			MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
			MdigProcess(MilDigitizer, MilMonoImageBuf[m_iGrabCircularIdx] + iGrabStartBufferIdx, SPV_RAW_IMAGE_NUMBER, M_STOP, M_DEFAULT, DFMGrabProcFunc, this);
		}

		strLog.Format("MdigProcess STOP (DFM)");
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));

		iGrabSuccess = DFM_RET_GRABDONE_ERROR;
		return iGrabSuccess;
	}

#endif

	return iGrabSuccess;
}

BOOL CCameraManager::DFMSetLightIntensity(int iLightValue, int iPortOrder)
{
#ifdef INLINE_MODE

	CString sStr;

	if (iPortOrder == 1)
	{
		try
		{
			sStr.Format(_T("iA%d\n"), iLightValue);

			WriteSerial(m_HDFMGrabSerialHandle, HTuple(sStr).TupleOrds());
			Sleep(300);
		}
		catch (HException &except)
		{
			;
		}
	}
	else if (iPortOrder == 2)
	{
		try
		{
			sStr.Format(_T("iB%d\n"), iLightValue);
			WriteSerial(m_HDFMGrabSerialHandle, HTuple(sStr).TupleOrds());
			Sleep(300);
		}
		catch (HException &except)
		{
			;
		}
	}

#endif

	return TRUE;
}

int CCameraManager::DFMProc(int iPcVisionNo, int iDFMStartGrabBufferIndex, int iVisionCamType)
{
	try
	{
		HObject HSpecularGrayImage[SPV_RAW_IMAGE_NUMBER];
		HObject HSpecularConvNormalImage;

		if (THEAPP.Struct_PreferenceStruct.m_bUseConvertSingleThread)
		{
			HObject HDFMGrabImage[SPV_RAW_IMAGE_NUMBER];
			HObject HColorImage;

			auto log_time_start = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < SPV_RAW_IMAGE_NUMBER; i++)
			{
				Compose3(m_hoCallBackImage[m_iGrabCircularIdx][iDFMStartGrabBufferIndex + i][0], m_hoCallBackImage[m_iGrabCircularIdx][iDFMStartGrabBufferIndex + i][1], m_hoCallBackImage[m_iGrabCircularIdx][iDFMStartGrabBufferIndex + i][2], &HColorImage);
				if (THEAPP.m_pModelDataManager->m_bUseSpecularRoiCrop[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1])
				{
					CropRectangle1(HColorImage,
						&HDFMGrabImage[i],
						THEAPP.m_pModelDataManager->m_lSpecularCropLTPointY_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
						THEAPP.m_pModelDataManager->m_lSpecularCropLTPointX_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
						THEAPP.m_pModelDataManager->m_lSpecularCropRBPointY_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
						THEAPP.m_pModelDataManager->m_lSpecularCropRBPointX_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]);

					if (i == 0 && THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] == 1)
						CopyImage(HColorImage, m_HDFMRawImage + i);

					if (THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] < 1)
					{
						if (i == 0)
							ZoomImageFactor(HColorImage,
								&m_HDFMRawImage[i],
								THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
								THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
								"nearest_neighbor");

						ZoomImageFactor(HDFMGrabImage[i],
							&HDFMGrabImage[i],
							THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
							THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
							"nearest_neighbor");
					}
				}
				else
				{
					CopyImage(HColorImage, HDFMGrabImage + i);

					if (THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] < 1)
					{
						ZoomImageFactor(HDFMGrabImage[i],
							&HDFMGrabImage[i],
							THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
							THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
							"nearest_neighbor");
					}
				}
			}
			auto log_time_end = std::chrono::high_resolution_clock::now();
			auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("DFM Proc copy RawImage, Time(ms): %d", log_time_ms);
			THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));

			BOOL bDFMProcRet = FALSE;

			log_time_start = std::chrono::high_resolution_clock::now();

			bDFMProcRet = THEAPP.m_pCameraManagerSpecularMaster->DFMCalNormal(HDFMGrabImage, HSpecularGrayImage, &HSpecularConvNormalImage, TRUE);

			if (bDFMProcRet == FALSE)
				return 0;
			else
			{
				THEAPP.m_pCameraManagerSpecularMaster->SetGpuProcMode(GPU_PROC_MODE_SPECULAR);

				THEAPP.m_pCameraManagerSpecularMaster->SetSpecularProcParameter(TEACHING_MZ_NO, HSpecularGrayImage, HSpecularConvNormalImage, iPcVisionNo, iVisionCamType, TRUE, FALSE, TRUE);

				THEAPP.m_pCameraManagerSpecularMaster->m_eventImageReady.SetEvent();

				CSingleLock s(&(THEAPP.m_pCameraManagerSpecularMaster->m_eventDFMProcDone));

				s.Lock(); // wait until Proc Done...

				bDFMProcRet = THEAPP.m_pCameraManagerSpecularMaster->m_bDFMProcRet;
			}

			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("DFM Proc conversion, Time(ms): %d", log_time_ms);
			THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));

			if (bDFMProcRet)
				return 1;
			else
				return 0;
		}	// if (THEAPP.Struct_PreferenceStruct.m_bUseConvertSingleThread)
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

		strLog.Format("Halcon Exception [CameraManager DFMProc] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}
}

int CCameraManager::DiffusedProc(int iPcVisionNo, int iDiffusedStartGrabBufferIndex, int iVisionCamType)
{
	try
	{
		HObject HDiffusedGrayImage[DIFFUSED_RAW_IMAGE_NUMBER];

		if (THEAPP.Struct_PreferenceStruct.m_bUseConvertSingleThread)
		{
			HObject HDFMGrabImage[DIFFUSED_RAW_IMAGE_NUMBER];
			HObject HColorImage;

			auto log_time_start = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < DIFFUSED_RAW_IMAGE_NUMBER; i++)
			{
				Compose3(m_hoCallBackImage[m_iGrabCircularIdx][iDiffusedStartGrabBufferIndex + i][0], m_hoCallBackImage[m_iGrabCircularIdx][iDiffusedStartGrabBufferIndex + i][1], m_hoCallBackImage[m_iGrabCircularIdx][iDiffusedStartGrabBufferIndex + i][2], &HColorImage);
				if (THEAPP.m_pModelDataManager->m_bUseSpecularRoiCrop[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1])
				{
					CropRectangle1(HColorImage,
						&HDFMGrabImage[i],
						THEAPP.m_pModelDataManager->m_lSpecularCropLTPointY_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
						THEAPP.m_pModelDataManager->m_lSpecularCropLTPointX_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
						THEAPP.m_pModelDataManager->m_lSpecularCropRBPointY_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
						THEAPP.m_pModelDataManager->m_lSpecularCropRBPointX_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]);

					if (i == 0 && THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] == 1)
						CopyImage(HColorImage, m_HDFMRawImage + i);

					if (THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] < 1)
					{
						if (i == 0)
							ZoomImageFactor(HColorImage,
								&m_HDFMRawImage[i],
								THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
								THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
								"nearest_neighbor");

						ZoomImageFactor(HDFMGrabImage[i],
							&HDFMGrabImage[i],
							THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
							THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
							"nearest_neighbor");
					}
				}
				else
				{
					CopyImage(HColorImage, HDFMGrabImage + i);

					if (THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] < 1)
					{
						ZoomImageFactor(HDFMGrabImage[i],
							&HDFMGrabImage[i],
							THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
							THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
							"nearest_neighbor");
					}
				}
			}
			auto log_time_end = std::chrono::high_resolution_clock::now();
			auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("Diffused Proc copy RawImage, Time(ms): %d", log_time_ms);
			THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));

			BOOL bDFMProcRet = FALSE;

			log_time_start = std::chrono::high_resolution_clock::now();

			bDFMProcRet = THEAPP.m_pCameraManagerSpecularMaster->DiffusedCalNormal(HDFMGrabImage, HDiffusedGrayImage, TRUE);

			if (bDFMProcRet == FALSE)
				return 0;
			else
			{
				THEAPP.m_pCameraManagerSpecularMaster->SetGpuProcMode(GPU_PROC_MODE_DIFFUSED);

				THEAPP.m_pCameraManagerSpecularMaster->SetDiffusedProcParameter(TEACHING_MZ_NO, HDiffusedGrayImage, iPcVisionNo, iVisionCamType, TRUE);

				THEAPP.m_pCameraManagerSpecularMaster->m_eventImageReady.SetEvent();

				CSingleLock s(&(THEAPP.m_pCameraManagerSpecularMaster->m_eventDFMProcDone));

				s.Lock(); // wait until Proc Done...

				bDFMProcRet = THEAPP.m_pCameraManagerSpecularMaster->m_bDFMProcRet;
			}

			log_time_end = std::chrono::high_resolution_clock::now();
			log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

			strLog.Format("Diffused Proc conversion, Time(ms): %d", log_time_ms);
			THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));

			if (bDFMProcRet)
				return 1;
			else
				return 0;
		}	// if (THEAPP.Struct_PreferenceStruct.m_bUseConvertSingleThread)
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

		strLog.Format("Halcon Exception [CameraManager DiffusedProc] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}
}

void CCameraManager::SetSpecularProcParameter(int iMzNo, HObject *pHSpecularGrayRawImage, HObject HSpecularConvNormalImage, int iPcVisionNo, int iVisionCamType, BOOL bUseGPU, BOOL bDualProc, BOOL bLeft)
{
	m_iMzNo = iMzNo;
	m_iPcVisionNo = iPcVisionNo;
	m_iVisionCamType = iVisionCamType;
	m_bUseGPU = bUseGPU;
	m_bDualProc = bDualProc;
	m_bLeft = bLeft;

	// Reference because of thread synchronization

	ho_Hor1 = pHSpecularGrayRawImage[0];
	ho_Hor2 = pHSpecularGrayRawImage[1];
	ho_Hor3 = pHSpecularGrayRawImage[2];
	ho_Hor4 = pHSpecularGrayRawImage[3];
	ho_Vert1 = pHSpecularGrayRawImage[4];
	ho_Vert2 = pHSpecularGrayRawImage[5];
	ho_Vert3 = pHSpecularGrayRawImage[6];
	ho_Vert4 = pHSpecularGrayRawImage[7];

	m_HProcNormalImage = HSpecularConvNormalImage;
}

void CCameraManager::SetDiffusedProcParameter(int iMzNo, HObject *pHDiffusedGrayRawImage, int iPcVisionNo, int iVisionCamType, BOOL bUseGPU)
{
	m_iMzNo = iMzNo;
	m_iPcVisionNo = iPcVisionNo;
	m_iVisionCamType = iVisionCamType;
	m_bUseGPU = bUseGPU;

	// Reference because of thread synchronization

	ho_Diffused1 = pHDiffusedGrayRawImage[0];
	ho_Diffused2 = pHDiffusedGrayRawImage[1];
	ho_Diffused3 = pHDiffusedGrayRawImage[2];
	ho_Diffused4 = pHDiffusedGrayRawImage[3];
}

BOOL CCameraManager::GetSpecularProcParameter(HObject *pHConvColorImage, double dZoomFactor, int iPcVisionNo)
{
	try
	{
		if (m_bDFMProcRet)
		{
			for (int i = 0; i < DFM_USED_CONV_IMAGE_NUMBER; i++)
			{
				if (dZoomFactor < 1)
					ZoomImageSize(m_HDFMConvColorImage[i], &pHConvColorImage[i], THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iPcVisionNo], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iPcVisionNo],
						"nearest_neighbor");
				else
					CopyImage(m_HDFMConvColorImage[i], pHConvColorImage + i);		// Copy
			}
		}

		return m_bDFMProcRet;
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

		strLog.Format("Halcon Exception [CameraManager GetSpecularProcParameter] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL CCameraManager::GetDiffusedProcParameter(HObject *pHConvColorImage, double dZoomFactor, int iPcVisionNo)
{
	try
	{
		if (m_bDFMProcRet)
		{
			if (dZoomFactor < 1)
				ZoomImageSize(m_HDiffusedConvColorImage, pHConvColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iPcVisionNo], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iPcVisionNo],
					"nearest_neighbor");
			else
				CopyImage(m_HDiffusedConvColorImage, pHConvColorImage);		// Copy
		}

		return m_bDFMProcRet;
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

		strLog.Format("Halcon Exception [CameraManager GetDiffusedProcParameter] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL CCameraManager::GetSpecularCropProcParameter(HObject *pHConvColorImage, long LTPointX, long LTPointY, double dZoomFactor, int iPcVisionNo)
{
	try
	{
		long lLTPointX, lLTPointY;
		lLTPointX = LTPointX;
		lLTPointY = LTPointY;

		if (m_bDFMProcRet)
		{
			HObject HTileCropImage;
			HTuple HTileOffsetX, HTileOffsetY;
			HTuple HTileRow1, HTileCol1, HTileRow2, HTileCol2;
			HTuple HImageSizeX, HImageSizeY;
			GetImageSize(m_HDFMRawImage[0], &HImageSizeX, &HImageSizeY);

			for (int i = 0; i < DFM_USED_CONV_IMAGE_NUMBER; i++)
			{
				GenEmptyObj(&HTileCropImage);
				ConcatObj(HTileCropImage, m_HDFMRawImage[0], &HTileCropImage);

				TupleGenConst(0, 0, &HTileOffsetX);
				TupleGenConst(0, 0, &HTileOffsetY);
				TupleGenConst(0, 0, &HTileRow1);
				TupleGenConst(0, 0, &HTileCol1);
				TupleGenConst(0, 0, &HTileRow2);
				TupleGenConst(0, 0, &HTileCol2);

				TupleConcat(HTileOffsetX, 0, &HTileOffsetX);
				TupleConcat(HTileOffsetY, 0, &HTileOffsetY);
				TupleConcat(HTileRow1, -1, &HTileRow1);
				TupleConcat(HTileCol1, -1, &HTileCol1);
				TupleConcat(HTileRow2, -1, &HTileRow2);
				TupleConcat(HTileCol2, -1, &HTileCol2);

				TupleConcat(HTileOffsetX, lLTPointX, &HTileOffsetX);
				TupleConcat(HTileOffsetY, lLTPointY, &HTileOffsetY);
				TupleConcat(HTileRow1, -1, &HTileRow1);
				TupleConcat(HTileCol1, -1, &HTileCol1);
				TupleConcat(HTileRow2, -1, &HTileRow2);
				TupleConcat(HTileCol2, -1, &HTileCol2);

				ConcatObj(HTileCropImage, m_HDFMConvColorImage[i], &HTileCropImage);
				TileImagesOffset(HTileCropImage, &HTileCropImage, HTileOffsetY, HTileOffsetX, HTileRow1, HTileCol1, HTileRow2, HTileCol2, HImageSizeX, HImageSizeY);

				if (dZoomFactor < 1)
					ZoomImageSize(HTileCropImage, &pHConvColorImage[i], THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iPcVisionNo], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iPcVisionNo],
						"nearest_neighbor");
				else
					CopyImage(HTileCropImage, pHConvColorImage + i);		// Copy
			}
		}

		return m_bDFMProcRet;
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

		strLog.Format("Halcon Exception [CCameraManager::GetSpecularCropProcParameter] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL CCameraManager::GetDiffusedCropProcParameter(HObject *pHConvColorImage, long LTPointX, long LTPointY, double dZoomFactor, int iPcVisionNo)
{
	try
	{
		long lLTPointX, lLTPointY;
		lLTPointX = LTPointX;
		lLTPointY = LTPointY;

		if (m_bDFMProcRet)
		{
			HObject HTileCropImage;
			HTuple HTileOffsetX, HTileOffsetY;
			HTuple HTileRow1, HTileCol1, HTileRow2, HTileCol2;
			HTuple HImageSizeX, HImageSizeY;
			GetImageSize(m_HDFMRawImage[0], &HImageSizeX, &HImageSizeY);

			GenEmptyObj(&HTileCropImage);
			ConcatObj(HTileCropImage, m_HDFMRawImage[0], &HTileCropImage);

			TupleGenConst(0, 0, &HTileOffsetX);
			TupleGenConst(0, 0, &HTileOffsetY);
			TupleGenConst(0, 0, &HTileRow1);
			TupleGenConst(0, 0, &HTileCol1);
			TupleGenConst(0, 0, &HTileRow2);
			TupleGenConst(0, 0, &HTileCol2);

			TupleConcat(HTileOffsetX, 0, &HTileOffsetX);
			TupleConcat(HTileOffsetY, 0, &HTileOffsetY);
			TupleConcat(HTileRow1, -1, &HTileRow1);
			TupleConcat(HTileCol1, -1, &HTileCol1);
			TupleConcat(HTileRow2, -1, &HTileRow2);
			TupleConcat(HTileCol2, -1, &HTileCol2);

			TupleConcat(HTileOffsetX, lLTPointX, &HTileOffsetX);
			TupleConcat(HTileOffsetY, lLTPointY, &HTileOffsetY);
			TupleConcat(HTileRow1, -1, &HTileRow1);
			TupleConcat(HTileCol1, -1, &HTileCol1);
			TupleConcat(HTileRow2, -1, &HTileRow2);
			TupleConcat(HTileCol2, -1, &HTileCol2);

			ConcatObj(HTileCropImage, m_HDiffusedConvColorImage, &HTileCropImage);
			TileImagesOffset(HTileCropImage, &HTileCropImage, HTileOffsetY, HTileOffsetX, HTileRow1, HTileCol1, HTileRow2, HTileCol2, HImageSizeX, HImageSizeY);

			if (dZoomFactor < 1)
				ZoomImageSize(HTileCropImage, pHConvColorImage, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iPcVisionNo], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iPcVisionNo],
					"nearest_neighbor");
			else
				CopyImage(HTileCropImage, pHConvColorImage);		// Copy
		}

		return m_bDFMProcRet;
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

		strLog.Format("Halcon Exception [CCameraManager::GetDiffusedCropProcParameter] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

void CCameraManager::SetAffineParameter(HObject *pHGrabColorImage, int iMzNo, int iPcVisionNo, int iNoInspectImage, HTuple *pHMatchingHomMat)
{
	m_iMzNo = iMzNo;
	m_iPcVisionNo = iPcVisionNo;
	m_iNoInspectImage = iNoInspectImage;
	m_pHMatchingHomMat = pHMatchingHomMat;

	for (int iii = 0; iii < iNoInspectImage; iii++)
	{
		m_HGrabColorImage[iii] = pHGrabColorImage[iii];
	}
}

BOOL CCameraManager::GetAffineParameter(HObject *pHAffineTransImage)
{
	try
	{
		if (m_bDFMProcRet)
		{
			for (int i = 0; i < m_iNoInspectImage; i++)
			{
				if (m_bGlobalAlignTransImageFlag[i])
				{
					CopyImage(m_HAffineTransImage[i], pHAffineTransImage + i);		// Copy
				}
			}
		}

		return m_bDFMProcRet;
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

		strLog.Format("Halcon Exception [CameraManager GetAffineParameter] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}


BOOL CCameraManager::DFMGlobalProcInspection()
{
	if (m_iGpuProcMode == GPU_PROC_MODE_SPECULAR)
	{
		HTuple  hv_DeviceHandle;
		BOOL bUseGPU = TRUE;
		int iMzNo = m_iMzNo;
		int iPcVisionNo = m_iPcVisionNo;
		int iVisionCamType = m_iVisionCamType;
		BOOL bDualProc = m_bDualProc;
		BOOL bLeft = m_bLeft;

		try
		{
			HTuple hv_specK[DFM_MAX_SPECULAR_IMAGE_NUMBER];
			HTuple hv_fH[DFM_MAX_SHAPE_IMAGE_NUMBER];
			HTuple hv_fL[DFM_MAX_SHAPE_IMAGE_NUMBER];
			HTuple hv_sigma[DFM_MAX_SHAPE_IMAGE_NUMBER];
			HTuple hv_bw;
			int psx, psy; // phase shift correction
			int tex, tey; // end values for phase period correction

			psx = (int)THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDFMPsx;
			psy = (int)THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDFMPsy;
			tex = (int)THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDFMTex;
			tey = (int)THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDFMTey;

			int iii, jjj;
			for (iii = 0; iii < DFM_MAX_SPECULAR_IMAGE_NUMBER; iii++)
				hv_specK[iii] = HTuple(THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDFMSpecularImageSF[iii]);

			hv_bw = HTuple(THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_iDFMMeanFilterSize);

			for (iii = 0; iii < DFM_MAX_SHAPE_IMAGE_NUMBER; iii++)
			{
				hv_fH[iii] = HTuple(THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDFMHigh[iii]);
				hv_fL[iii] = HTuple(THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDFMLow[iii]);
				hv_sigma[iii] = HTuple(THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDFMDerivativeSmFactor[iii]);
			}

			//////////////////////////////////////////////////////////////////////////

			BOOL bDebugSave = FALSE;

			GetImageSize(ho_Hor1, &HInputImageSizeX, &HInputImageSizeY);
			iInputImageSizeX = HInputImageSizeX.L();
			iInputImageSizeY = HInputImageSizeY.L();

			if (bDebugSave)
			{
				WriteImage(ho_Hor1, "bmp", 0, "c:\\DualTest\\Auto_SpecularRaw_1");
				WriteImage(ho_Hor2, "bmp", 0, "c:\\DualTest\\Auto_SpecularRaw_2");
				WriteImage(ho_Hor3, "bmp", 0, "c:\\DualTest\\Auto_SpecularRaw_3");
				WriteImage(ho_Hor4, "bmp", 0, "c:\\DualTest\\Auto_SpecularRaw_4");
				WriteImage(ho_Vert1, "bmp", 0, "c:\\DualTest\\Auto_SpecularRaw_5");
				WriteImage(ho_Vert2, "bmp", 0, "c:\\DualTest\\Auto_SpecularRaw_6");
				WriteImage(ho_Vert3, "bmp", 0, "c:\\DualTest\\Auto_SpecularRaw_7");
				WriteImage(ho_Vert4, "bmp", 0, "c:\\DualTest\\Auto_SpecularRaw_8");
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			// Phase/FC Calculation

			hv_w = iInputImageSizeX;
			hv_h = iInputImageSizeY;

			GenImageConst(&ho_ImagePhX, "real", hv_w, hv_h);
			GenImageConst(&ho_ImagePhY, "real", hv_w, hv_h);
			GenImageConst(&ho_ImageFCX, "byte", hv_w, hv_h);
			GenImageConst(&ho_ImageFCY, "byte", hv_w, hv_h);

			GetImagePointer1(ho_Hor1, &hIm1x, &hv_Type, &hv_Width, &hv_Height);
			pIm1x = (UINT8 *)hIm1x.L();
			GetImagePointer1(ho_Hor2, &hIm2x, &hv_Type, &hv_Width, &hv_Height);
			pIm2x = (UINT8 *)hIm2x.L();
			GetImagePointer1(ho_Hor3, &hIm3x, &hv_Type, &hv_Width, &hv_Height);
			pIm3x = (UINT8 *)hIm3x.L();
			GetImagePointer1(ho_Hor4, &hIm4x, &hv_Type, &hv_Width, &hv_Height);
			pIm4x = (UINT8 *)hIm4x.L();

			GetImagePointer1(ho_ImagePhX, &hImPhx, &hv_Type, &hv_Width, &hv_Height);
			pImPhx = (float *)hImPhx.L();
			GetImagePointer1(ho_ImageFCX, &hImFCx, &hv_Type, &hv_Width, &hv_Height);
			pImFCx = (UINT8 *)hImFCx.L();

			GetImagePointer1(ho_Vert1, &hIm1y, &hv_Type, &hv_Width, &hv_Height);
			pIm1y = (UINT8 *)hIm1y.L();
			GetImagePointer1(ho_Vert2, &hIm2y, &hv_Type, &hv_Width, &hv_Height);
			pIm2y = (UINT8 *)hIm2y.L();
			GetImagePointer1(ho_Vert3, &hIm3y, &hv_Type, &hv_Width, &hv_Height);
			pIm3y = (UINT8 *)hIm3y.L();
			GetImagePointer1(ho_Vert4, &hIm4y, &hv_Type, &hv_Width, &hv_Height);
			pIm4y = (UINT8 *)hIm4y.L();

			GetImagePointer1(ho_ImagePhY, &hImPhy, &hv_Type, &hv_Width, &hv_Height);
			pImPhy = (float *)hImPhy.L();
			GetImagePointer1(ho_ImageFCY, &hImFCy, &hv_Type, &hv_Width, &hv_Height);
			pImFCy = (UINT8 *)hImFCy.L();
			//////////////////////////////////////////////////////////////////////////

			int h = (int)hv_h.L();
			int w = (int)hv_w.L();

			int iOrgCamImageWidth, iOrgCamImageHeight;
			iOrgCamImageWidth = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iPcVisionNo];
			iOrgCamImageHeight = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iPcVisionNo];

			if (iOrgCamImageWidth <= 0 || iOrgCamImageHeight <= 0)
			{
				m_bDFMProcRet = FALSE;

				return FALSE;
			}

			hv_MP = ((hv_w*hv_h) / 1024.0) / 1024.0;

			double texd = tex * (PI / 180.0f); 	// convert degrees to radians
			double teyd = tey * (PI / 180.0f); 	// convert degrees to radians

			float fSigma = (float)hv_sigma[0].D();
			float fHs, fLs;

			GenImageConst(&m_HProcShapeImage1, "byte", hv_w, hv_h);
			GetImagePointer1(m_HProcShapeImage1, &hImResi_1, &hv_Type, &hv_Width, &hv_Height);
			pImResi_1 = (UINT8 *)hImResi_1.L();

			fHs = (float)hv_fH[0].D();
			fLs = (float)hv_fL[0].D();

			s_universal_avi->Specular_CUDA(pIm1x, pIm2x, pIm3x, pIm4x, pIm1y, pIm2y, pIm3y, pIm4y,
				pImPhx, pImFCx, pImPhy, pImFCy,
				pImResi_1,
				w, h,
				psx, psy, texd, teyd,
				fSigma, fHs, fLs
			);

			GenImageConst(&m_HProcShapeImage2, "byte", hv_w, hv_h);
			GetImagePointer1(m_HProcShapeImage2, &hImResi_2, &hv_Type, &hv_Width, &hv_Height);
			pImResi_2 = (UINT8 *)hImResi_2.L();

			fHs = (float)hv_fH[1].D();
			fLs = (float)hv_fL[1].D();

			s_universal_avi->Specular_CUDA_GetCurvature(pImResi_2, w, h, fHs, fLs);

			//////////////////////////////////////////////////////////////////////////////////////////////

			// Specular #1
			HTuple hImSpec;

			GenImageConst(&m_HProcSpecularImage, "byte", hv_w, hv_h);
			GetImagePointer1(m_HProcSpecularImage, &hImSpec, &hv_Type, &hv_Width, &hv_Height);
			pImSpec = (UINT8 *)hImSpec.L();

			s_universal_avi->Specular_CUDA_GetSpec(pImSpec, w, h);

			ScaleImage(m_HProcSpecularImage, &m_HProcSpecularImage, hv_specK[0], 0);

			// Phase X
			s_universal_avi->Specular_CUDA_GetPhaseX(pImPhx, w, h);
			ConvertImageType(ho_ImagePhX, &ho_ImagePhX, "byte");

			// Phase Y
			s_universal_avi->Specular_CUDA_GetPhaseY(pImPhy, w, h);
			ConvertImageType(ho_ImagePhY, &ho_ImagePhY, "byte");

			// Image Compose #1
			Compose3(m_HProcSpecularImage, m_HProcShapeImage1, m_HProcShapeImage2, m_HDFMConvColorImage + DFM_IMAGE_COMBINE_CURVATURE);

			// Image Compose #2
			Compose3(m_HProcNormalImage, ho_ImagePhX, ho_ImagePhY, m_HDFMConvColorImage + DFM_IMAGE_COMBINE_PHASE);

			if (bDebugSave)
			{
				CTime ChangeTime = CTime::GetCurrentTime();
				CString strChangeTime;
				strChangeTime.Format("c:\\DualTest\\DFM_Conversion_1_%02d%02d%02d", ChangeTime.GetHour(), ChangeTime.GetMinute(), ChangeTime.GetSecond());
				WriteImage(m_HDFMConvColorImage[DFM_IMAGE_COMBINE_CURVATURE], "bmp", 0, HTuple(strChangeTime));
				strChangeTime.Format("c:\\DualTest\\DFM_Conversion_2_%02d%02d%02d", ChangeTime.GetHour(), ChangeTime.GetMinute(), ChangeTime.GetSecond());
				WriteImage(m_HDFMConvColorImage[DFM_IMAGE_COMBINE_PHASE], "bmp", 0, HTuple(strChangeTime));
			}

			m_bDFMProcRet = TRUE;

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

			strLog.Format("Halcon Exception [CameraManager DFMGlobalProcInspection - Specular Vision] : <%s>%s", sOperatorName, sErrMsg);
			THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

			m_bDFMProcRet = FALSE;

			return FALSE;
		}
	}
	else if (m_iGpuProcMode == GPU_PROC_MODE_DIFFUSED)
	{
		BOOL bUseGPU = TRUE;
		int iMzNo = m_iMzNo;
		int iPcVisionNo = m_iPcVisionNo;
		int iVisionCamType = m_iVisionCamType;

		try
		{
			float sigma, pH, pL;
			// #1 

			GetImagePointer1(ho_Diffused1, &hIm1xD, &hv_TypeD, &hv_WidthD, &hv_HeightD);
			pIm1xD = (UINT8 *)hIm1xD.L();
			GetImagePointer1(ho_Diffused2, &hIm2xD, &hv_TypeD, &hv_WidthD, &hv_HeightD);
			pIm2xD = (UINT8 *)hIm2xD.L();
			GetImagePointer1(ho_Diffused3, &hIm3xD, &hv_TypeD, &hv_WidthD, &hv_HeightD);
			pIm3xD = (UINT8 *)hIm3xD.L();
			GetImagePointer1(ho_Diffused4, &hIm4xD, &hv_TypeD, &hv_WidthD, &hv_HeightD);
			pIm4xD = (UINT8 *)hIm4xD.L();

			GetImageSize(ho_Diffused1, &HInputImageSizeXD, &HInputImageSizeYD);
			iInputImageSizeXD = HInputImageSizeXD.L();
			iInputImageSizeYD = HInputImageSizeYD.L();

			hv_wD = iInputImageSizeXD;
			hv_hD = iInputImageSizeYD;

			int h = (int)hv_hD.L();
			int w = (int)hv_wD.L();

			hv_MP = ((hv_wD * hv_hD) / 1024.0) / 1024.0;

			GenImageConst(&ho_DiffusedResulti, "byte", hv_wD, hv_hD);
			GenImageConst(&ho_ImageC2D, "byte", hv_wD, hv_hD);

			GetImagePointer1(ho_DiffusedResulti, &hImResi, &hv_TypeD, &hv_WidthD, &hv_HeightD);
			pImResi = (UINT8 *)hImResi.L();

			GetImagePointer1(ho_ImageC2D, &hImC2D, &hv_TypeD, &hv_WidthD, &hv_HeightD);
			pImC2D = (UINT8 *)hImC2D.L();

			sigma = (float)THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDiffusedSmFactor[0];
			pH = (float)THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDiffusedHigh[0];
			pL = (float)THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDiffusedLow[0];

			s_universal_avi->Diffuse_CUDA(pIm1xD, pIm2xD, pIm3xD, pIm4xD, pImResi, pImC2D,
				sigma, pH, pL,
				w, h);

			// #2
			GenImageConst(&ho_DiffusedResulti2, "byte", hv_wD, hv_hD);
			GetImagePointer1(ho_DiffusedResulti2, &hImResi2, &hv_TypeD, &hv_WidthD, &hv_HeightD);
			pImResi2 = (UINT8 *)hImResi2.L();

			sigma = (float)THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDiffusedSmFactor[1];
			pH = (float)THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDiffusedHigh[1];
			pL = (float)THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dDiffusedLow[1];

			s_universal_avi->Diffuse_CUDA(pIm1xD, pIm2xD, pIm3xD, pIm4xD, pImResi2, pImC2D,
				sigma, pH, pL,
				w, h);

			Compose3(ho_ImageC2D, ho_DiffusedResulti, ho_DiffusedResulti2, &m_HDiffusedConvColorImage);

			m_bDFMProcRet = TRUE;

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

			strLog.Format("Halcon Exception [CameraManager DFMGlobalProcInspection] : <%s>%s", sOperatorName, sErrMsg);
			THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

			m_bDFMProcRet = FALSE;

			return FALSE;
		}
	}
}

BOOL CCameraManager::DFMCalNormal(HObject *pHGrabColorImage, HObject *pHGrayImage, HObject *pHNormalColorImage, BOOL bColor, int iModuleNo, int iGrabOrder)
{
	try
	{
		int iii, jjj;

		if (bColor)
		{
			for (iii = 0; iii < SPV_RAW_IMAGE_NUMBER; iii++)
			{
				Rgb1ToGray(pHGrabColorImage[iii], pHGrayImage + iii);
			}

			HObject HMultiChImage;
			HObject HDecomposeImage[8][3];
			HObject HNormalChannelImage[3];
			HObject HNormalColorImage;

			for (jjj = 0; jjj < 3; jjj++)
			{
				GenEmptyObj(&HMultiChImage);
				for (iii = 0; iii < SPV_RAW_IMAGE_NUMBER; iii++)
				{
					Decompose3(pHGrabColorImage[iii], &(HDecomposeImage[iii][0]), &(HDecomposeImage[iii][1]), &(HDecomposeImage[iii][2]));
					AppendChannel(HMultiChImage, HDecomposeImage[iii][jjj], &HMultiChImage);
				}
				MeanN(HMultiChImage, HNormalChannelImage + jjj);
			}

			Compose3(HNormalChannelImage[0], HNormalChannelImage[1], HNormalChannelImage[2], &HNormalColorImage);
			CopyImage(HNormalColorImage, pHNormalColorImage);
		}
		else
		{
			for (iii = 0; iii < SPV_RAW_IMAGE_NUMBER; iii++)
			{
				CopyImage(pHGrabColorImage[iii], pHGrayImage + iii);
			}

			HObject HMultiChImage;
			HObject HNormalChannelImage;
			HObject HNormalColorImage;

			GenEmptyObj(&HMultiChImage);
			for (iii = 0; iii < SPV_RAW_IMAGE_NUMBER; iii++)
			{
				AppendChannel(HMultiChImage, pHGrayImage[iii], &HMultiChImage);
			}
			MeanN(HMultiChImage, &HNormalChannelImage);

			Compose3(HNormalChannelImage, HNormalChannelImage, HNormalChannelImage, &HNormalColorImage);
			CopyImage(HNormalColorImage, pHNormalColorImage);
		}

		if (THEAPP.Struct_PreferenceStruct.m_bUse4dSaveRawImage)
		{
			CString sCommonFolderName = THEAPP.Struct_PreferenceStruct.m_strEtcFolderPath + "CMI_Results\\SpecularVision";
			CString sSaveFolderName, sDayFolderName, sSaveFileName;

			SYSTEMTIME time;
			GetLocalTime(&time);

			sDayFolderName.Format("%s\\%04d%02d%02d", sCommonFolderName, time.wYear, time.wMonth, time.wDay);
			THEAPP.m_FileBase.CreateFolder(sDayFolderName);

			sSaveFolderName.Format("%s\\%02d%02d%02d_%03d", sDayFolderName, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
			THEAPP.m_FileBase.CreateFolder(sSaveFolderName);

			for (int i = 0; i < SPV_RAW_IMAGE_NUMBER; i++)
			{
				sSaveFileName.Format("%s\\ModuleNo%d_GrabOrder%d_RawImage_%d", sSaveFolderName, iModuleNo, iGrabOrder, i + 1);
				WriteImage(pHGrabColorImage[i], "jpg", 0, HTuple(sSaveFileName));
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

		strLog.Format("Halcon Exception [CameraManager DFMCalNormal] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL CCameraManager::DiffusedCalNormal(HObject *pHGrabColorImage, HObject *pHGrayImage, BOOL bColor, int iModuleNo, int iGrabOrder)
{
	try
	{
		int iii, jjj;

		if (bColor)
		{
			for (iii = 0; iii < DIFFUSED_RAW_IMAGE_NUMBER; iii++)
			{
				Rgb1ToGray(pHGrabColorImage[iii], pHGrayImage + iii);
			}
		}
		else
		{
			for (iii = 0; iii < DIFFUSED_RAW_IMAGE_NUMBER; iii++)
			{
				CopyImage(pHGrabColorImage[iii], pHGrayImage + iii);
			}
		}

		if (THEAPP.Struct_PreferenceStruct.m_bUse4dSaveRawImage)
		{
			CString sCommonFolderName = THEAPP.Struct_PreferenceStruct.m_strEtcFolderPath + "CMI_Results\\DiffusedVision";
			CString sSaveFolderName, sDayFolderName, sSaveFileName;

			SYSTEMTIME time;
			GetLocalTime(&time);

			sDayFolderName.Format("%s\\%04d%02d%02d", sCommonFolderName, time.wYear, time.wMonth, time.wDay);
			THEAPP.m_FileBase.CreateFolder(sDayFolderName);

			sSaveFolderName.Format("%s\\%02d%02d%02d_%03d", sDayFolderName, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
			THEAPP.m_FileBase.CreateFolder(sSaveFolderName);

			for (int i = 0; i < DIFFUSED_RAW_IMAGE_NUMBER; i++)
			{
				sSaveFileName.Format("%s\\ModuleNo%d_GrabOrder%d_RawImage_%d", sSaveFolderName, iModuleNo, iGrabOrder, i + 1);
				WriteImage(pHGrabColorImage[i], "jpg", 0, HTuple(sSaveFileName));
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

		strLog.Format("Halcon Exception [CameraManager DiffusedCalNormal] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

void CCameraManager::ResetDFMVariable()
{
	int i;

	for (i = 0; i < DFM_USED_CONV_IMAGE_NUMBER; i++)
		GenEmptyObj(m_HDFMConvColorImage + i);

	GenEmptyObj(&m_HDiffusedConvColorImage);

	m_iMzNo = TEACHING_MZ_NO;
	m_iPcVisionNo = VISION_NUMBER_1;
	m_iVisionCamType = VISION_NUMBER_1;
	m_bUseGPU = FALSE;
	m_bDualProc = TRUE;
	m_bLeft = TRUE;
}

BOOL CCameraManager::DFM_ReadConvertImage_TeachMode(int iWhichDFMImage, BOOL bImageSave)
{
	try {
		HObject HNormalImageR, HNormalImageG, HNormalImageB;

		if (THEAPP.m_pModelDataManager->m_bUseSpecularRoiCrop[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1])
		{
			HObject HTileCropImage;
			HTuple HTileOffsetX, HTileOffsetY;
			HTuple HTileRow1, HTileCol1, HTileRow2, HTileCol2;

			GenEmptyObj(&HTileCropImage);
			ConcatObj(HTileCropImage, m_HDFMRawImage[0], &HTileCropImage);
			HTuple HImageSizeX, HImageSizeY;
			GetImageSize(m_HDFMRawImage[0], &HImageSizeX, &HImageSizeY);

			TupleGenConst(0, 0, &HTileOffsetX);
			TupleGenConst(0, 0, &HTileOffsetY);
			TupleGenConst(0, 0, &HTileRow1);
			TupleGenConst(0, 0, &HTileCol1);
			TupleGenConst(0, 0, &HTileRow2);
			TupleGenConst(0, 0, &HTileCol2);

			TupleConcat(HTileOffsetX, 0, &HTileOffsetX);
			TupleConcat(HTileOffsetY, 0, &HTileOffsetY);
			TupleConcat(HTileRow1, -1, &HTileRow1);
			TupleConcat(HTileCol1, -1, &HTileCol1);
			TupleConcat(HTileRow2, -1, &HTileRow2);
			TupleConcat(HTileCol2, -1, &HTileCol2);

			TupleConcat(HTileOffsetX, THEAPP.m_pModelDataManager->m_lSpecularCropLTPointX_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] * THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HTileOffsetX);
			TupleConcat(HTileOffsetY, THEAPP.m_pModelDataManager->m_lSpecularCropLTPointY_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] * THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HTileOffsetY);
			TupleConcat(HTileRow1, -1, &HTileRow1);
			TupleConcat(HTileCol1, -1, &HTileCol1);
			TupleConcat(HTileRow2, -1, &HTileRow2);
			TupleConcat(HTileCol2, -1, &HTileCol2);
			ConcatObj(HTileCropImage, THEAPP.m_pCameraManagerSpecularMaster->m_HDFMConvColorImage[iWhichDFMImage], &HTileCropImage);

			TileImagesOffset(HTileCropImage, &HTileCropImage, HTileOffsetY, HTileOffsetX, HTileRow1, HTileCol1, HTileRow2, HTileCol2, HImageSizeX, HImageSizeY);

			Decompose3(HTileCropImage, &HNormalImageR, &HNormalImageG, &HNormalImageB);
			CopyImage(HNormalImageR, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
			CopyImage(HNormalImageG, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
			CopyImage(HNormalImageB, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));
			CopyImage(HTileCropImage, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
		}
		else
		{
			Decompose3(THEAPP.m_pCameraManagerSpecularMaster->m_HDFMConvColorImage[iWhichDFMImage], &HNormalImageR, &HNormalImageG, &HNormalImageB);
			CopyImage(HNormalImageR, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
			CopyImage(HNormalImageG, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
			CopyImage(HNormalImageB, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));
			CopyImage(THEAPP.m_pCameraManagerSpecularMaster->m_HDFMConvColorImage[iWhichDFMImage], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
		}

		TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B],
			&(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), "hsi");
		Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));

		for (int i = 0; i < CHANNEL_NUM; i++)
		{
			if (THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] < 1)
				ZoomImageFactor(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i],
					&THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i],
					1 / THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
					1 / THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
					"nearest_neighbor");

			CopyImage(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]));
		}

		if (bImageSave)
			THEAPP.m_pInspectAdminViewDlg->TeachingImageGrabSave();								// 티칭 이미지 저장  => 티칭화면에 있어야만 저장됨.

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

		strLog.Format("Halcon Exception [CameraManager::DFM_ReadConvertImage_TeachMode] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

// Tab: 1~8, 9~16
BOOL CCameraManager::DFM_ReadRawImage_TeachMode(int iTeachImageIndex, BOOL bImageSave)
{
	try {
		int iWhichDFMImage = iTeachImageIndex % SPV_RAW_IMAGE_NUMBER;

		HObject HNormalImageR, HNormalImageG, HNormalImageB;
		Decompose3(m_HDFMRawImage[iWhichDFMImage], &HNormalImageR, &HNormalImageG, &HNormalImageB);
		CopyImage(HNormalImageR, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
		CopyImage(HNormalImageG, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
		CopyImage(HNormalImageB, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));
		CopyImage(m_HDFMRawImage[iWhichDFMImage], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));

		TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B],
			&(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), "hsi");
		Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));

		for (int i = 0; i < CHANNEL_NUM; i++)
		{
			CopyImage(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]));
		}

		if (bImageSave)
			THEAPP.m_pInspectAdminViewDlg->TeachingImageGrabSave();								// 티칭 이미지 저장  => 티칭화면에 있어야만 저장됨.

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

		strLog.Format("Halcon Exception [CameraManager::DFM_ReadRawImage_TeachMode] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL CCameraManager::DFM_ReadPageImage_TeachMode(int iTeachImageIndex, BOOL bImageSave)
{
	try {
		HObject HColorImage;

		CopyImage(m_hoCallBackImage[m_iGrabCircularIdx][iTeachImageIndex][0], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
		CopyImage(m_hoCallBackImage[m_iGrabCircularIdx][iTeachImageIndex][1], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
		CopyImage(m_hoCallBackImage[m_iGrabCircularIdx][iTeachImageIndex][2], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));

		Compose3(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B], &HColorImage);
		CopyImage(HColorImage, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));

		TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B],
			&(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), "hsi");
		Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));

		for (int i = 0; i < CHANNEL_NUM; i++)
		{
			CopyImage(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]));
		}

		if (bImageSave)
			THEAPP.m_pInspectAdminViewDlg->TeachingImageGrabSave();								// 티칭 이미지 저장  => 티칭화면에 있어야만 저장됨.

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

		strLog.Format("Halcon Exception [CameraManager::DFM_ReadPageImage_TeachMode] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL CCameraManager::Diffused_ReadConvertImage_TeachMode(int iWhichDFMImage, BOOL bImageSave)
{
	try {
		HObject HNormalImageR, HNormalImageG, HNormalImageB;

		if (THEAPP.m_pModelDataManager->m_bUseSpecularRoiCrop[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1])
		{
			HObject HTileCropImage;
			HTuple HTileOffsetX, HTileOffsetY;
			HTuple HTileRow1, HTileCol1, HTileRow2, HTileCol2;

			GenEmptyObj(&HTileCropImage);
			ConcatObj(HTileCropImage, m_HDFMRawImage[0], &HTileCropImage);
			HTuple HImageSizeX, HImageSizeY;
			GetImageSize(m_HDFMRawImage[0], &HImageSizeX, &HImageSizeY);

			TupleGenConst(0, 0, &HTileOffsetX);
			TupleGenConst(0, 0, &HTileOffsetY);
			TupleGenConst(0, 0, &HTileRow1);
			TupleGenConst(0, 0, &HTileCol1);
			TupleGenConst(0, 0, &HTileRow2);
			TupleGenConst(0, 0, &HTileCol2);

			TupleConcat(HTileOffsetX, 0, &HTileOffsetX);
			TupleConcat(HTileOffsetY, 0, &HTileOffsetY);
			TupleConcat(HTileRow1, -1, &HTileRow1);
			TupleConcat(HTileCol1, -1, &HTileCol1);
			TupleConcat(HTileRow2, -1, &HTileRow2);
			TupleConcat(HTileCol2, -1, &HTileCol2);

			TupleConcat(HTileOffsetX, THEAPP.m_pModelDataManager->m_lSpecularCropLTPointX_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] * THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HTileOffsetX);
			TupleConcat(HTileOffsetY, THEAPP.m_pModelDataManager->m_lSpecularCropLTPointY_Roi[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] * THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HTileOffsetY);
			TupleConcat(HTileRow1, -1, &HTileRow1);
			TupleConcat(HTileCol1, -1, &HTileCol1);
			TupleConcat(HTileRow2, -1, &HTileRow2);
			TupleConcat(HTileCol2, -1, &HTileCol2);
			ConcatObj(HTileCropImage, THEAPP.m_pCameraManagerSpecularMaster->m_HDiffusedConvColorImage, &HTileCropImage);

			TileImagesOffset(HTileCropImage, &HTileCropImage, HTileOffsetY, HTileOffsetX, HTileRow1, HTileCol1, HTileRow2, HTileCol2, HImageSizeX, HImageSizeY);

			Decompose3(HTileCropImage, &HNormalImageR, &HNormalImageG, &HNormalImageB);
			CopyImage(HNormalImageR, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
			CopyImage(HNormalImageG, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
			CopyImage(HNormalImageB, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));
			CopyImage(HTileCropImage, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
		}
		else
		{
			Decompose3(THEAPP.m_pCameraManagerSpecularMaster->m_HDiffusedConvColorImage, &HNormalImageR, &HNormalImageG, &HNormalImageB);
			CopyImage(HNormalImageR, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
			CopyImage(HNormalImageG, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
			CopyImage(HNormalImageB, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));
			CopyImage(THEAPP.m_pCameraManagerSpecularMaster->m_HDiffusedConvColorImage, &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
		}

		TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B],
			&(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), "hsi");
		Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));

		for (int i = 0; i < CHANNEL_NUM; i++)
		{
			if (THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1] < 1)
				ZoomImageFactor(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i],
					&THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i],
					1 / THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
					1 / THEAPP.m_pModelDataManager->m_dSpecularResizeFactor[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1],
					"nearest_neighbor");

			CopyImage(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]));
		}

		if (bImageSave)
			THEAPP.m_pInspectAdminViewDlg->TeachingImageGrabSave();								// 티칭 이미지 저장  => 티칭화면에 있어야만 저장됨.

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

		strLog.Format("Halcon Exception [CameraManager::Diffused_ReadConvertImage_TeachMode] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}


BOOL CCameraManager::ResetDFMConvertVariable()
{
	try
	{
		GenEmptyObj(&ho_Hor1);
		GenEmptyObj(&ho_Hor2);
		GenEmptyObj(&ho_Hor3);
		GenEmptyObj(&ho_Hor4);

		GenEmptyObj(&ho_Vert1);
		GenEmptyObj(&ho_Vert2);
		GenEmptyObj(&ho_Vert3);
		GenEmptyObj(&ho_Vert4);

		GenEmptyObj(&ho_ImagePhX);
		GenEmptyObj(&ho_ImageFCX);
		GenEmptyObj(&ho_ImagePhY);
		GenEmptyObj(&ho_ImageFCY);

		GenEmptyObj(&ho_DerivGaussX);
		GenEmptyObj(&ho_DerivGaussY);

		GenEmptyObj(&m_HProcNormalImage);
		GenEmptyObj(&m_HProcSpecularImage);
		GenEmptyObj(&m_HProcShapeImage1);
		GenEmptyObj(&m_HProcShapeImage2);
		GenEmptyObj(&HMultiChImage);

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

		strLog.Format("Halcon Exception [CameraManager::ResetSpecularVariable] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

void CCameraManager::ResetSpecularTriggerPeriod(int iMzNo)
{
	CString sDFMCommand;

	if (m_bDFMGrabComPortOpened)
	{
		try
		{
			sDFMCommand.Format("tA%d\n", THEAPP.Struct_PreferenceStruct.m_iDFMTriggerPeriod);
			WriteSerial(m_HDFMGrabSerialHandle, HTuple(sDFMCommand).TupleOrds());
			Sleep(300);
			sDFMCommand.Format("tB%d\n", THEAPP.Struct_PreferenceStruct.m_iDFMTriggerPeriod);
			WriteSerial(m_HDFMGrabSerialHandle, HTuple(sDFMCommand).TupleOrds());
		}
		catch (HException &except)
		{
			;
		}
	}
}

void CCameraManager::TurnOff_Specular()
{
	CString sDFMCommand;

	if (m_bDFMGrabComPortOpened)
	{
		Sleep(50);

		sDFMCommand.Format("0A\n");
		try
		{
			WriteSerial(m_HDFMGrabSerialHandle, HTuple(sDFMCommand).TupleOrds());
		}
		catch (HException &except)
		{
			;
		}

		// Speedup
		Sleep(50);
		sDFMCommand.Format("0B\n");

		try
		{
			WriteSerial(m_HDFMGrabSerialHandle, HTuple(sDFMCommand).TupleOrds());
		}
		catch (HException &except)
		{
			;
		}
	}
}

BOOL CCameraManager::CheckCircularBufferCopyStatus()
{
	BOOL bCopyDone = FALSE;
	int iCnt = 0;

	for (int i = 0; i < m_iMaxCircularGrab; i++)
	{
		if (m_bGrabCircularBufferCopyDone[i])
			++iCnt;
	}

	if (iCnt >= (m_iMaxCircularGrab - 1))
		bCopyDone = TRUE;

	return bCopyDone;
}

void CCameraManager::StopDFMProcThread()
{
	THEAPP.m_eventStop.SetEvent();

	if (m_pConvertThread && m_pConvertThread->m_hThread) {
		DWORD dResult = WaitForSingleObject(m_pConvertThread->m_hThread, 2000);

		if (dResult == WAIT_TIMEOUT) { // WAIT_OBJECT_0: successfully done
			if (m_pConvertThread) {
				TerminateThread(DFMProcInspectionThread, 0);

				strLog.Format("DFM Thread Stop, Terminated");
				THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));
			}
		}
		else
		{
			strLog.Format("DFM Thread Stop, Normal");
			THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));
		}
	}

	m_pConvertThread = NULL;
}

void CCameraManager::Init4DVision()
{
#ifdef USE_DIFFUSED
	if (bCreatePhSObject)
	{
		string key_file_name;
		Get_System_Dir(key_file_name);

		s_universal_avi = new S_Universal_AVI(key_file_name);

		std::vector<std::string> gpu_available;
		s_universal_avi->IsGPUAvailable(&gpu_available);

		if (gpu_available.size() > 0)
		{
			s_universal_avi->CUDASetDevice(0);
		}
	}
#endif
}

void CCameraManager::Get_System_Dir(string& key_file_name)
{
	char sys_file[255] = { 0 };
	GetModuleFileNameA(NULL, sys_file, 255);
	string sys_dir(sys_file);
	int pos = sys_dir.find_last_of("\\") + 1;
	int len = sys_dir.length() - pos;
	sys_dir.erase(pos, len);
#ifdef _DEBUG
	key_file_name = sys_dir + "KEY_SN_D.bin";
#else
	key_file_name = sys_dir + "KEY_SN.bin";
#endif // DEBUG
}

void CCameraManager::CalcGaussDeriv1D(HTuple hv_sigma, HTuple *hv_Gauss, HTuple *hv_GaussDeriv, HTuple *hv_Gauss2Deriv, HTuple *hv_ksz)
{

	// Local iconic variables

	// Local control variables
	HTuple  hv_hsz, hv_i, hv_x;

	//** from OpenCV
	(*hv_ksz) = (2 * ((((hv_sigma - 0.8) / 0.3) - 1.).TupleInt())) + 1;
	hv_hsz = (((hv_sigma - 0.8) / 0.3) - 1.).TupleInt();
	//hv_hsz = (hv_sigma*2.).TupleInt();
	//(*hv_ksz) = (hv_hsz * 2) + 1;

	{
		HTuple end_val6 = (*hv_ksz) - 1;
		HTuple step_val6 = 1;
		for (hv_i = 0; hv_i.Continue(end_val6, step_val6); hv_i += step_val6)
		{
			hv_x = hv_i - hv_hsz;
			(*hv_Gauss)[hv_i] = (((((((-hv_x)*hv_x) / 2.) / hv_sigma) / hv_sigma).TupleExp()) / hv_sigma) / (HTuple(6.28).TupleSqrt());
			(*hv_GaussDeriv)[hv_i] = (((-hv_x)*((((((-hv_x)*hv_x) / 2.) / hv_sigma) / hv_sigma).TupleExp())) / hv_sigma) / (HTuple(6.28).TupleSqrt());
			(*hv_Gauss2Deriv)[hv_i] = ((-((hv_x*hv_x) - (hv_sigma*hv_sigma))) / hv_sigma) / hv_sigma;
		}
	}
	return;
}

UINT CCameraManager::OfflineSpecularPhaseConv()
{
	try
	{
		if (m_pConvertThread == NULL)
			m_pConvertThread = AfxBeginThread(DFMProcInspectionThread, this);

		CString sFolderPath;

		BROWSEINFO bi = { 0 };
		bi.lpszTitle = "RawImage 폴더를 선택하세요.";
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;  // 파일 시스템 폴더만 표시하고 새 스타일 사용

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		if (pidl != nullptr)
		{
			char path[MAX_PATH];
			if (SHGetPathFromIDList(pidl, path))
			{
				CoTaskMemFree(pidl);  // 메모리 해제
				sFolderPath = path;
			}
			else
				CoTaskMemFree(pidl);  // 메모리 해제
		}
		else
			return 0;

		HObject HSpecularGrayImage[SPV_RAW_IMAGE_NUMBER];
		HObject HSpecularConvNormalImage;

		HObject HDFMGrabImage[SPV_RAW_IMAGE_NUMBER];
		HObject HColorImage;

		HANDLE hFindFile;
		WIN32_FIND_DATA FindFileData;
		CString FolderName, ImageName, sReadFileName, strRawImageFileFullName;
		BOOL bFileFindFail = FALSE;

		FolderName.Format(sFolderPath + "\\");

		for (int i = 0; i < SPV_RAW_IMAGE_NUMBER; i++)
		{
			ImageName.Format("*_*_RawImage_%d*", i + 1);
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

			ReadImage(HDFMGrabImage + i, (HTuple)strRawImageFileFullName);

			//			ZoomImageFactor(HDFMGrabImage[i], HDFMGrabImage + i, 0.4, 0.4, "constant");
		}

		if (bFileFindFail)
		{
			AfxMessageBox("Raw Image 파일이 없습니다. 폴더를 확인해 주세요.", MB_SYSTEMMODAL);
			return 0;
		}

		BOOL bDFMProcRet = FALSE;

		bDFMProcRet = THEAPP.m_pCameraManager->DFMCalNormal(HDFMGrabImage, HSpecularGrayImage, &HSpecularConvNormalImage, TRUE);

		if (bDFMProcRet == FALSE)
			return 0;

		THEAPP.m_pCameraManager->SetGpuProcMode(GPU_PROC_MODE_SPECULAR);

		THEAPP.m_pCameraManager->SetSpecularProcParameter(TEACHING_MZ_NO, HSpecularGrayImage, HSpecularConvNormalImage, THEAPP.m_iCurTeachVision, THEAPP.m_pCameraManager->GetVisionCamName(), TRUE, FALSE, TRUE);

		THEAPP.m_pCameraManager->m_eventImageReady.SetEvent();

		CSingleLock s(&(THEAPP.m_pCameraManager->m_eventDFMProcDone));

		s.Lock(); // wait until Proc Done...

		HObject HConvImage[DFM_USED_CONV_IMAGE_NUMBER];

		bDFMProcRet = THEAPP.m_pCameraManager->GetSpecularProcParameter(HConvImage, 1, 0);

		if (bDFMProcRet == FALSE)
		{
			AfxMessageBox("변환 오류", MB_SYSTEMMODAL);
			return 0;
		}
		else
		{
			HObject HSpecular, HCurvature1, HCurvature2;
			HObject HCombined2D, HPhaseX, HPhaseY;

			//ZoomImageFactor(HConvImage[0], &(HConvImage[0]), 2.5, 2.5, "constant");
			//ZoomImageFactor(HConvImage[1], &(HConvImage[1]), 2.5, 2.5, "constant");

			Decompose3(HConvImage[0], &HSpecular, &HCurvature1, &HCurvature2);
			Decompose3(HConvImage[1], &HCombined2D, &HPhaseX, &HPhaseY);

			strRawImageFileFullName.Format("%sSpecularComversion_1", FolderName);
			WriteImage(HConvImage[0], "bmp", 0, HTuple(strRawImageFileFullName));

			strRawImageFileFullName.Format("%sSpecularComversion_2", FolderName);
			WriteImage(HConvImage[1], "bmp", 0, HTuple(strRawImageFileFullName));

			strRawImageFileFullName.Format("%sSpecular", FolderName);
			WriteImage(HSpecular, "bmp", 0, HTuple(strRawImageFileFullName));

			strRawImageFileFullName.Format("%sCurvature1", FolderName);
			WriteImage(HCurvature1, "bmp", 0, HTuple(strRawImageFileFullName));

			strRawImageFileFullName.Format("%sCurvature2", FolderName);
			WriteImage(HCurvature2, "bmp", 0, HTuple(strRawImageFileFullName));

			strRawImageFileFullName.Format("%sCombined2D", FolderName);
			WriteImage(HCombined2D, "bmp", 0, HTuple(strRawImageFileFullName));

			strRawImageFileFullName.Format("%sPhaseX", FolderName);
			WriteImage(HPhaseX, "bmp", 0, HTuple(strRawImageFileFullName));

			strRawImageFileFullName.Format("%sPhaseY", FolderName);
			WriteImage(HPhaseY, "bmp", 0, HTuple(strRawImageFileFullName));

			AfxMessageBox("변환 성공", MB_SYSTEMMODAL);
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

		strLog.Format("Halcon Exception [CameraManager::OfflineSpecularPhaseConv] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

}

UINT CCameraManager::OfflineSpecularPhaseConvForSubFolder()
{
	try
	{
		if (m_pConvertThread == NULL)
			m_pConvertThread = AfxBeginThread(DFMProcInspectionThread, this);

		CString sSelectedFolderPath;
		CString sFolderPath;

		BROWSEINFO bi = { 0 };
		bi.lpszTitle = "RawImage 폴더를 선택하세요.";
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;  // 파일 시스템 폴더만 표시하고 새 스타일 사용

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		if (pidl != nullptr)
		{
			char path[MAX_PATH];
			if (SHGetPathFromIDList(pidl, path))
			{
				CoTaskMemFree(pidl);  // 메모리 해제
				sSelectedFolderPath = path;
			}
			else
				CoTaskMemFree(pidl);  // 메모리 해제
		}
		else
			return 0;

		HObject HSpecularGrayImage[SPV_RAW_IMAGE_NUMBER];
		HObject HSpecularConvNormalImage;

		HObject HDFMGrabImage[SPV_RAW_IMAGE_NUMBER];
		HObject HColorImage;

		CFileFind ff;
		CString strSubFolder = sSelectedFolderPath + "\\*.*";

		if (!ff.FindFile(strSubFolder, 0)) return 0;

		HANDLE hFindFile;
		WIN32_FIND_DATA FindFileData;
		CString FolderName, ImageName, sReadFileName, strRawImageFileFullName;
		HObject HConvImage[DFM_USED_CONV_IMAGE_NUMBER];

		BOOL bExist = ff.FindNextFile();

		while (TRUE) {
			if (ff.IsDirectory())
			{
				sFolderPath = ff.GetFileName();

				if (sFolderPath.GetLength() <= 2)
				{
					if (!bExist)
						break;
					bExist = ff.FindNextFile();

					continue;
				}

				BOOL bFileFindFail = FALSE;

				FolderName.Format(sSelectedFolderPath + "\\" + sFolderPath + "\\");

				for (int i = 0; i < SPV_RAW_IMAGE_NUMBER; i++)
				{
					ImageName.Format("*_*_RawImage_%d*", i + 1);
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

					ReadImage(HDFMGrabImage + i, (HTuple)strRawImageFileFullName);
				}

				if (bFileFindFail)
				{
					//AfxMessageBox("Raw Image 파일이 없습니다. 폴더를 확인해 주세요.", MB_SYSTEMMODAL);

					if (!bExist)
						break;
					bExist = ff.FindNextFile();

					continue;
				}

				BOOL bDFMProcRet = FALSE;

				bDFMProcRet = THEAPP.m_pCameraManager->DFMCalNormal(HDFMGrabImage, HSpecularGrayImage, &HSpecularConvNormalImage, TRUE);

				if (bDFMProcRet == FALSE)
				{
					if (!bExist)
						break;
					bExist = ff.FindNextFile();

					continue;
				}

				THEAPP.m_pCameraManager->SetGpuProcMode(GPU_PROC_MODE_SPECULAR);

				THEAPP.m_pCameraManager->SetSpecularProcParameter(TEACHING_MZ_NO, HSpecularGrayImage, HSpecularConvNormalImage, THEAPP.m_iCurTeachVision, THEAPP.m_pCameraManager->GetVisionCamName(), TRUE, FALSE, TRUE);

				THEAPP.m_pCameraManager->m_eventImageReady.SetEvent();

				CSingleLock s(&(THEAPP.m_pCameraManager->m_eventDFMProcDone));

				s.Lock(); // wait until Proc Done...

				bDFMProcRet = THEAPP.m_pCameraManager->GetSpecularProcParameter(HConvImage, 1, 0);

				if (bDFMProcRet == TRUE)
				{
					HObject HSpecular, HCurvature1, HCurvature2;
					HObject HCombined2D, HPhaseX, HPhaseY;

					Decompose3(HConvImage[0], &HSpecular, &HCurvature1, &HCurvature2);
					Decompose3(HConvImage[1], &HCombined2D, &HPhaseX, &HPhaseY);

					strRawImageFileFullName.Format("%sSpecularComversion_1", FolderName);
					WriteImage(HConvImage[0], "bmp", 0, HTuple(strRawImageFileFullName));

					strRawImageFileFullName.Format("%sSpecularComversion_2", FolderName);
					WriteImage(HConvImage[1], "bmp", 0, HTuple(strRawImageFileFullName));

					strRawImageFileFullName.Format("%sSpecular", FolderName);
					WriteImage(HSpecular, "bmp", 0, HTuple(strRawImageFileFullName));

					strRawImageFileFullName.Format("%sCurvature1", FolderName);
					WriteImage(HCurvature1, "bmp", 0, HTuple(strRawImageFileFullName));

					strRawImageFileFullName.Format("%sCurvature2", FolderName);
					WriteImage(HCurvature2, "bmp", 0, HTuple(strRawImageFileFullName));

					strRawImageFileFullName.Format("%sCombined2D", FolderName);
					WriteImage(HCombined2D, "bmp", 0, HTuple(strRawImageFileFullName));

					strRawImageFileFullName.Format("%sPhaseX", FolderName);
					WriteImage(HPhaseX, "bmp", 0, HTuple(strRawImageFileFullName));

					strRawImageFileFullName.Format("%sPhaseY", FolderName);
					WriteImage(HPhaseY, "bmp", 0, HTuple(strRawImageFileFullName));
				}

				Sleep(1000);
			}

			if (!bExist)
				break;
			bExist = ff.FindNextFile();

		}

		AfxMessageBox("변환 완료", MB_SYSTEMMODAL);

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

		strLog.Format("Halcon Exception [CameraManager::OfflineSpecularPhaseConvForSubFolder] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

}

UINT CCameraManager::OfflineDiffusedVisionConv()
{
	try
	{
		if (m_pConvertThread == NULL)
			m_pConvertThread = AfxBeginThread(DFMProcInspectionThread, this);

		CString sFolderPath;

		BROWSEINFO bi = { 0 };
		bi.lpszTitle = "RawImage 폴더를 선택하세요.";
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;  // 파일 시스템 폴더만 표시하고 새 스타일 사용

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		if (pidl != nullptr)
		{
			char path[MAX_PATH];
			if (SHGetPathFromIDList(pidl, path))
			{
				CoTaskMemFree(pidl);  // 메모리 해제
				sFolderPath = path;
			}
			else
				CoTaskMemFree(pidl);  // 메모리 해제
		}
		else
			return 0;

		HObject HDiffusedGrayImage[DIFFUSED_RAW_IMAGE_NUMBER];

		HObject HDFMGrabImage[DIFFUSED_RAW_IMAGE_NUMBER];
		HObject HColorImage;

		HANDLE hFindFile;
		WIN32_FIND_DATA FindFileData;
		CString FolderName, ImageName, sReadFileName, strRawImageFileFullName;
		BOOL bFileFindFail = FALSE;

		FolderName.Format(sFolderPath + "\\");

		for (int i = 0; i < DIFFUSED_RAW_IMAGE_NUMBER; i++)
		{
			ImageName.Format("*_*_RawImage_%d*", i + 1);
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

			ReadImage(HDFMGrabImage + i, (HTuple)strRawImageFileFullName);
		}

		if (bFileFindFail)
		{
			AfxMessageBox("Raw Image 파일이 없습니다. 폴더를 확인해 주세요.", MB_SYSTEMMODAL);
			return 0;
		}

		BOOL bDFMProcRet = FALSE;

		bDFMProcRet = THEAPP.m_pCameraManager->DiffusedCalNormal(HDFMGrabImage, HDiffusedGrayImage, TRUE);

		if (bDFMProcRet == FALSE)
			return 0;

		THEAPP.m_pCameraManager->SetGpuProcMode(GPU_PROC_MODE_DIFFUSED);

		THEAPP.m_pCameraManager->SetDiffusedProcParameter(TEACHING_MZ_NO, HDiffusedGrayImage, THEAPP.m_iCurTeachVision, THEAPP.m_pCameraManager->GetVisionCamName(), TRUE);

		THEAPP.m_pCameraManager->m_eventImageReady.SetEvent();

		CSingleLock s(&(THEAPP.m_pCameraManager->m_eventDFMProcDone));

		s.Lock(); // wait until Proc Done...

		HObject HConvImage[DIFFUSED_USED_CONV_IMAGE_NUMBER];

		bDFMProcRet = THEAPP.m_pCameraManager->GetDiffusedProcParameter(HConvImage, 1, 0);

		if (bDFMProcRet == FALSE)
		{
			AfxMessageBox("변환 오류", MB_SYSTEMMODAL);
			return 0;
		}
		else
		{
			HObject HCombined2D, HCurvature1, HCurvature2;

			Decompose3(HConvImage[0], &HCombined2D, &HCurvature1, &HCurvature2);

			strRawImageFileFullName.Format("%sDiffusedConversion_1", FolderName);
			WriteImage(HConvImage[0], "bmp", 0, HTuple(strRawImageFileFullName));

			strRawImageFileFullName.Format("%sCurvature1", FolderName);
			WriteImage(HCurvature1, "bmp", 0, HTuple(strRawImageFileFullName));

			strRawImageFileFullName.Format("%sCurvature2", FolderName);
			WriteImage(HCurvature2, "bmp", 0, HTuple(strRawImageFileFullName));

			strRawImageFileFullName.Format("%sCombined2D", FolderName);
			WriteImage(HCombined2D, "bmp", 0, HTuple(strRawImageFileFullName));

			AfxMessageBox("변환 성공", MB_SYSTEMMODAL);
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

		strLog.Format("Halcon Exception [CameraManager::OfflineDiffusedVisionConv] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

}

UINT CCameraManager::OfflineDiffusedVisionConvForSubFolder()
{
	try
	{
		if (m_pConvertThread == NULL)
			m_pConvertThread = AfxBeginThread(DFMProcInspectionThread, this);

		CString sSelectedFolderPath;
		CString sFolderPath;

		BROWSEINFO bi = { 0 };
		bi.lpszTitle = "RawImage 폴더를 선택하세요.";
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;  // 파일 시스템 폴더만 표시하고 새 스타일 사용

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		if (pidl != nullptr)
		{
			char path[MAX_PATH];
			if (SHGetPathFromIDList(pidl, path))
			{
				CoTaskMemFree(pidl);  // 메모리 해제
				sSelectedFolderPath = path;
			}
			else
				CoTaskMemFree(pidl);  // 메모리 해제
		}
		else
			return 0;

		HObject HDiffusedGrayImage[DIFFUSED_RAW_IMAGE_NUMBER];

		HObject HDFMGrabImage[DIFFUSED_RAW_IMAGE_NUMBER];
		HObject HColorImage;

		CFileFind ff;
		CString strSubFolder = sSelectedFolderPath + "\\*.*";

		if (!ff.FindFile(strSubFolder, 0)) return 0;

		HANDLE hFindFile;
		WIN32_FIND_DATA FindFileData;
		CString FolderName, ImageName, sReadFileName, strRawImageFileFullName;
		HObject HConvImage[DIFFUSED_USED_CONV_IMAGE_NUMBER];

		BOOL bExist = ff.FindNextFile();

		while (TRUE) {
			if (ff.IsDirectory())
			{
				sFolderPath = ff.GetFileName();

				if (sFolderPath.GetLength() <= 2)
				{
					if (!bExist)
						break;
					bExist = ff.FindNextFile();

					continue;
				}

				BOOL bFileFindFail = FALSE;

				FolderName.Format(sSelectedFolderPath + "\\" + sFolderPath + "\\");

				for (int i = 0; i < DIFFUSED_RAW_IMAGE_NUMBER; i++)
				{
					ImageName.Format("*_*_RawImage_%d*", i + 1);
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

					ReadImage(HDFMGrabImage + i, (HTuple)strRawImageFileFullName);
				}

				if (bFileFindFail)
				{
					//AfxMessageBox("Raw Image 파일이 없습니다. 폴더를 확인해 주세요.", MB_SYSTEMMODAL);

					if (!bExist)
						break;
					bExist = ff.FindNextFile();

					continue;
				}

				BOOL bDFMProcRet = FALSE;

				bDFMProcRet = THEAPP.m_pCameraManager->DiffusedCalNormal(HDFMGrabImage, HDiffusedGrayImage, TRUE);

				if (bDFMProcRet == FALSE)
				{
					if (!bExist)
						break;
					bExist = ff.FindNextFile();

					continue;
				}

				THEAPP.m_pCameraManager->SetGpuProcMode(GPU_PROC_MODE_DIFFUSED);

				THEAPP.m_pCameraManager->SetDiffusedProcParameter(TEACHING_MZ_NO, HDiffusedGrayImage, THEAPP.m_iCurTeachVision, THEAPP.m_pCameraManager->GetVisionCamName(), TRUE);

				THEAPP.m_pCameraManager->m_eventImageReady.SetEvent();

				CSingleLock s(&(THEAPP.m_pCameraManager->m_eventDFMProcDone));

				s.Lock(); // wait until Proc Done...

				bDFMProcRet = THEAPP.m_pCameraManager->GetDiffusedProcParameter(HConvImage, 1, 0);

				if (bDFMProcRet == TRUE)
				{
					HObject HCombined2D, HCurvature1, HCurvature2;

					Decompose3(HConvImage[0], &HCombined2D, &HCurvature1, &HCurvature2);

					strRawImageFileFullName.Format("%sDiffusedConversion_1", FolderName);
					WriteImage(HConvImage[0], "bmp", 0, HTuple(strRawImageFileFullName));

					strRawImageFileFullName.Format("%sCurvature1", FolderName);
					WriteImage(HCurvature1, "bmp", 0, HTuple(strRawImageFileFullName));

					strRawImageFileFullName.Format("%sCurvature2", FolderName);
					WriteImage(HCurvature2, "bmp", 0, HTuple(strRawImageFileFullName));

					strRawImageFileFullName.Format("%sCombined2D", FolderName);
					WriteImage(HCombined2D, "bmp", 0, HTuple(strRawImageFileFullName));
				}

				Sleep(1000);
			}

			if (!bExist)
				break;
			bExist = ff.FindNextFile();

		}

		AfxMessageBox("변환 완료", MB_SYSTEMMODAL);

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

		strLog.Format("Halcon Exception [CameraManager::OfflineDiffusedVisionConvForSubFolder] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}

}
