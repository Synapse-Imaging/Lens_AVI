// HandlerService.cpp : implementation file
//
#include "stdafx.h"
#include "uscan.h"
#include "HandlerService.h"
#include "IniFileCS.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
/// - 제어 : IP: 192.168.0.11, PORT (V1:8001, V2:8002, V3:8003)
///	- 비젼1 : IP: 192.168.0.21 Port (H:8001, V:8001) ==> Bottom1, Angle1
///	- 비젼2 : IP: 192.168.0.22 Port (H:8002, V:8002) ==> Top1, Top Angle
///	- 비젼3 : IP: 192.168.0.23 Port (H:8003, V:8003) ==> Top2
///	- 비젼2 - Slave : IP: 192.168.0.24 Port (H:8004, V:8004) ==> Top1, Top Angle
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CHandlerService

IMPLEMENT_DYNAMIC(CHandlerService, CWnd)

CHandlerService* CHandlerService::m_pInstance = NULL;

CHandlerService* CHandlerService::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CHandlerService();
		if (!m_pInstance->m_hWnd)
		{
			CRect r = m_pInstance->GetPosition();
			m_pInstance->CreateEx(0, AfxRegisterWndClass(0), "CHandlerService", 0, r, NULL, 0, NULL);
		}
	}
	return m_pInstance;
}

void CHandlerService::DeleteInstance()
{
	if (m_pInstance->m_hWnd)
		m_pInstance->DestroyWindow();
	if (m_pInstance)
		delete m_pInstance;
	m_pInstance = NULL;
}

CHandlerService::CHandlerService()
{
	m_nLocalPort = 0;
	m_bConnected = FALSE;

	m_iHandlerStatus = H_STATUS_START;

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		m_bMotionMoveComplete[i] = FALSE;
		m_bShiftMoveComplete[i] = FALSE;
		m_iScanCompleteCheckIndex[i] = 0;
		m_iInspectCompleteCheckIndex[i] = 0;
	}

	for (int i = 0; i < (VISION_NUMBER_MAX+TOTAL_SUB_CAM); i++)
		m_bLensMotionMoveComplete[i] = FALSE;

	m_bTurnMoveComplete = FALSE;

	m_sHandler_TCP_IP = "192.168.1.11";
	m_iHandler_TCP_Port = 8010;
	m_strRecvCmd = _T("");
	m_bHandler_TCP_Connect = FALSE;

	for (int i = 0; i < LIGHT_CONTROLLER_NUMBER_MAX; i++)
	{
		m_bTcpConnect[i] = FALSE;
		mp_TcpHandler[i] = NULL;
		m_sTCP_IP[i] = "192.168.0.0";
		m_iTCP_Port[i] = 5000;
	}
}

CHandlerService::~CHandlerService()
{

}

BEGIN_MESSAGE_MAP(CHandlerService, CWnd)
	ON_MESSAGE(UM_CLIENT_CONNECT, OnClientConnect)
	ON_MESSAGE(UM_CLIENT_RECEIVE, OnClientReceive)
	ON_MESSAGE(UM_CLIENT_CLOSE, OnClientClose)
	ON_MESSAGE(SCM_RECEIVE, OnTcpReceive)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHandlerService message handlers

BOOL CHandlerService::Initialize_Handler()
{
	//#ifdef HANDLER_USE
	if (!m_bHandler_TCP_Connect || !m_TCPHandler.Open_Socket(m_sHandler_TCP_IP, m_iHandler_TCP_Port, TRUE, this))	// Connect Retry
		return FALSE;
	m_nLocalPort = m_TCPHandler.Get_LocalPort();
	//#endif

	return TRUE;
}

BOOL CHandlerService::Initialize_TcpHandler(BOOL bRetryCheck)
{
#ifdef HANDLER_USE
	CString sVisionCamType;
	for (int i = 0; i < LIGHT_CONTROLLER_NUMBER_MAX; i++)
	{
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[i];
		if (sVisionCamType == "Undefine" || sVisionCamType == "Align")
			continue;

		mp_TcpHandler[i] = new CClientSocket;

		if (mp_TcpHandler[i])
		{
			char* ch = (LPSTR)(LPCTSTR)m_sTCP_IP[i];
			int iPort = m_iTCP_Port[i];

			if (!m_bTcpConnect[i])
			{
				Sleep(1000);
				m_bTcpConnect[i] = mp_TcpHandler[i]->ConnectLan(i, ch, (UINT)iPort, this);
				if (m_bTcpConnect[i] == FALSE && bRetryCheck)
				{
					strMessageBox.Format("Vision%d 조명 컨트롤러 통신 연결 실패: 조명 컨트롤러 설정을 확인해주세요.", i + 1);
					AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
				}
			}
		}
	}


	BOOL bTcpConnected = TRUE;
	for (int i = 0; i < LIGHT_CONTROLLER_NUMBER_MAX; i++)
	{
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[i];
		if (sVisionCamType == "Undefine" || sVisionCamType == "Align")
			continue;

		if (m_bTcpConnect[i] == FALSE)
			bTcpConnected = FALSE;
	}

	return bTcpConnected;

#else
	return TRUE;
#endif
}

void CHandlerService::Terminate_Handler()
{
	m_bConnected = FALSE;
	m_TCPHandler.Close_Socket();
}

void CHandlerService::Terminate_TcpHandler()
{
#ifdef HANDLER_USE
	CString sVisionCamType;
	for (int i = 0; i < LIGHT_CONTROLLER_NUMBER_MAX; i++)
	{
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[i];
		if (sVisionCamType == "Undefine" || sVisionCamType == "Align")
			continue;
		m_bTcpConnect[i] = FALSE;

		if (mp_TcpHandler[i])
			delete mp_TcpHandler[i];
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////

void CHandlerService::Get_StatusRequest()
{
	Set_StatusReply();
}

void CHandlerService::Get_StatusReply(CString sStatus)
{
	//	m_iHandlerStatus = atoi((LPSTR)(LPCSTR)sStatus);
}

void CHandlerService::Get_StatusUpdate(CString sStatus)
{
	CString strLog;
	m_iHandlerStatus = atoi((LPSTR)(LPCSTR)sStatus);

	strLog.Format("Get_StatusUpdate Handler %d", m_iHandlerStatus);
	THEAPP.m_log_handler->info("{}", LOG_CSTR(strLog));

	if (m_iHandlerStatus == 0)
		strLog.Format("Handler status, NOT READY");
	if (m_iHandlerStatus == 1)
		strLog.Format("Handler status, READY");
	if (m_iHandlerStatus == 2)
		strLog.Format("Handler status, RUN");
	THEAPP.m_log_handler->info("{}", LOG_CSTR(strLog));
	Set_StatusReply();
}

void  CHandlerService::Get_ModeRequest()
{
	Set_ModeReplay(m_iEquipmentMode);
}

void  CHandlerService::Get_ModeReply(int iMode)
{
	m_iEquipmentMode = iMode;
}

void  CHandlerService::Get_ModelReply(int iFlag)
{
	m_iModelCheck = iFlag;
}

#ifdef SINGLE_LENS
void  CHandlerService::Get_LotStart(CString sLotID, CString sMzNo, CString sTrayAmt, CString sModuleAmt, CString sHandlerModelName)
{
	CString strLog;
	int iMzNo;
	iMzNo = atoi((LPSTR)(LPCSTR)sMzNo);

	int iModelAutoLoad;
	iModelAutoLoad = 1;

	THEAPP.m_pInspectService->m_bSaveResultLogThreadDone[iMzNo - 1] = TRUE;
	THEAPP.m_pInspectService->m_bContDefectLogThreadDone[iMzNo - 1] = TRUE;

	int iLotTrayAmt, iLotModuleAmt;
	iLotTrayAmt = atoi((LPSTR)(LPCSTR)sTrayAmt);
	iLotModuleAmt = atoi((LPSTR)(LPCSTR)sModuleAmt);
	THEAPP.m_pInspectService->m_iLotTrayAmt_H[iMzNo - 1] = iLotTrayAmt;
	THEAPP.m_pInspectService->m_iLotModuleAmt_H[iMzNo - 1] = iLotModuleAmt;

	THEAPP.m_pInspectService->SetCycleStopStatus(FALSE);
	THEAPP.m_pInspectService->SetReloadStatus(FALSE);
	THEAPP.m_pInspectService->ReadyLot(FALSE, iMzNo, FALSE, sHandlerModelName, sLotID, iModelAutoLoad);

	strLog.Format("Lot start, LotID: %s", sLotID);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

#ifdef HANDLER_USE

#if !defined(SINGLE_LENS) && !defined(ASSY_LENS)
	for (int i = 0; i < LIGHT_CONTROLLER_NUMBER_MAX; i++)
	{
		CString sVisionCamType;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[i];
		if (THEAPP.Struct_PreferenceStruct.m_bUseVision[i]
			&& sVisionCamType != "Undefine" && sVisionCamType != "Align"
			&& m_bTcpConnect[i] == FALSE)
			THEAPP.m_pHandlerService->Set_ErrorRequest(HANDLER_ERROR_LIGHT_CONTROLLER_CONNECT_FAIL);
	}
#endif

#endif
}


void  CHandlerService::Get_LotReadyDone(CString sLotID, CString sMzNo)
{
	
}

#else
void  CHandlerService::Get_LotStart(CString sLotID, CString sMzNo, CString sTrayAmt, CString sModuleAmt, CString sHandlerModelName, CString sModelAutoLoad)
{
	CString strLog;
	int iMzNo;
	iMzNo = atoi((LPSTR)(LPCSTR)sMzNo);

	int iModelAutoLoad;
	iModelAutoLoad = atoi((LPSTR)(LPCSTR)sModelAutoLoad);

	THEAPP.m_pInspectService->m_bSaveResultLogThreadDone[iMzNo - 1] = TRUE;
	THEAPP.m_pInspectService->m_bContDefectLogThreadDone[iMzNo - 1] = TRUE;

	int iLotTrayAmt, iLotModuleAmt;
	iLotTrayAmt = atoi((LPSTR)(LPCSTR)sTrayAmt);
	iLotModuleAmt = atoi((LPSTR)(LPCSTR)sModuleAmt);
	THEAPP.m_pInspectService->m_iLotTrayAmt_H[iMzNo - 1] = iLotTrayAmt;
	THEAPP.m_pInspectService->m_iLotModuleAmt_H[iMzNo - 1] = iLotModuleAmt;

	THEAPP.m_pInspectService->SetCycleStopStatus(FALSE);
	THEAPP.m_pInspectService->SetReloadStatus(FALSE);
	THEAPP.m_pInspectService->ReadyLot(FALSE, iMzNo, FALSE, sHandlerModelName, sLotID, iModelAutoLoad);

	strLog.Format("Lot start, LotID: %s", sLotID);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

#ifdef HANDLER_USE
	for (int i = 0; i < LIGHT_CONTROLLER_NUMBER_MAX; i++)
	{
		CString sVisionCamType;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[i];
		if (THEAPP.Struct_PreferenceStruct.m_bUseVision[i]
			&& sVisionCamType != "Undefine" && sVisionCamType != "Align"
			&& m_bTcpConnect[i] == FALSE)
			THEAPP.m_pHandlerService->Set_ErrorRequest(HANDLER_ERROR_LIGHT_CONTROLLER_CONNECT_FAIL);
	}
#endif
}

#endif

void  CHandlerService::Get_LotEnd(CString sLotID, CString sMzNo)
{
	CString strLog;
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	int iMzNo;
	iMzNo = atoi((LPSTR)(LPCSTR)sMzNo);
	THEAPP.g_iGrabFailCount[iMzNo - 1] = 0;

	int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
	THEAPP.g_iDualModelDataRunCheck[iDualModelData]--;
	if (THEAPP.g_iDualModelDataRunCheck[iDualModelData] < 0)
		THEAPP.g_iDualModelDataRunCheck[iDualModelData] = 0;

	strLog.Format("Lot end, LotID: %s", sLotID);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

	if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
	{
		auto log_time_start = std::chrono::high_resolution_clock::now();

		CString sLotIDCopy = sLotID;

		EnqueueStatusWrite([=]()
		{
			for (int i = 0; i < VISION_NUMBER_MAX; i++)
			{
				m_csLotStatusWrite[i].Lock();

				CString sVisionCamType_Short, sVision;
				if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
				{
					int iJigNo = (i + 2) / 2;
					sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[i];
					sVision.Format("%s_Jig%d", sVisionCamType_Short, iJigNo);
				}
				else
					sVision = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[i];

				if (sVisionCamType_Short == "UD")
					continue;

				CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus_" + sVision + ".txt";
				CIniFileCS INI(strStatusFileName);

				CString sEndLotIDCheck;
				sEndLotIDCheck = INI.Get_String("LAST SCAN COMPLETE", "LotID", "");
				if (sEndLotIDCheck == sLotIDCopy)
				{
					INI.Set_String("LAST SCAN COMPLETE", "LotID", "");
					INI.Set_String("LAST SCAN COMPLETE", "Stage", "");
					INI.Set_Integer("LAST SCAN COMPLETE", "Magazine", -1);
					INI.Set_Integer("LAST SCAN COMPLETE", "Tray", -1);
					INI.Set_Integer("LAST SCAN COMPLETE", "Module", -1);
				}

				sEndLotIDCheck = INI.Get_String("LAST INSPECT COMPLETE", "LotID", "");
				if (sEndLotIDCheck == sLotIDCopy)
				{
					INI.Set_String("LAST INSPECT COMPLETE", "LotID", "");
					INI.Set_String("LAST INSPECT COMPLETE", "Stage", "");
					INI.Set_Integer("LAST INSPECT COMPLETE", "Magazine", -1);
					INI.Set_Integer("LAST INSPECT COMPLETE", "Tray", -1);
					INI.Set_Integer("LAST INSPECT COMPLETE", "Module", -1);
				}

				for (int j = 0; j < THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber; j++)
				{
					CString sKey;
					sKey.Format("LotID%d", j);
					sEndLotIDCheck = INI.Get_String("RECENTLY SCAN COMPLETE", sKey, "");
					if (sEndLotIDCheck == sLotIDCopy)
					{
						INI.Set_String("RECENTLY SCAN COMPLETE", sKey, "");
						sKey.Format("Stage%d", j);
						INI.Set_String("RECENTLY SCAN COMPLETE", sKey, "");
						sKey.Format("Magazine%d", j);
						INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, -1);
						sKey.Format("TrayID%d", j);
						INI.Set_String("RECENTLY SCAN COMPLETE", sKey, "");
						sKey.Format("Tray%d", j);
						INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, -1);
						sKey.Format("Module%d", j);
						INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, -1);
					}
				}

				for (int j = 0; j < THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber; j++)
				{
					CString sKey;
					sKey.Format("LotID%d", j);
					sEndLotIDCheck = INI.Get_String("RECENTLY INSPECT COMPLETE", sKey, "");
					if (sEndLotIDCheck == sLotIDCopy)
					{
						INI.Set_String("RECENTLY INSPECT COMPLETE", sKey, "");
						sKey.Format("Stage%d", j);
						INI.Set_String("RECENTLY INSPECT COMPLETE", sKey, "");
						sKey.Format("Magazine%d", j);
						INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);
						sKey.Format("Tray%d", j);
						INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);
						sKey.Format("Module%d", j);
						INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);
					}
				}

				m_csLotStatusWrite[i].Unlock();
			}
		});

		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("ALL/ Module recently status logging(Lot end), Time(ms): %d, LotID: %s", log_time_ms, sLotID);
		THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));
	}

	try {
		if (THEAPP.iAutoSettingCountCurrent == THEAPP.iAutoSettingCountEnd)
		{
			CString FolderName = "c:\\AutoSettings\\json\\";
			CString jsonFileName;
			if (THEAPP.strAutoSettingMode == "Light-S")
				jsonFileName = FolderName + "LightSetting_S_CHS-K.json";
			else if (THEAPP.strAutoSettingMode == "Light-M")
				jsonFileName = FolderName + "LightSetting_M_CHS-K.json";
			else if (THEAPP.strAutoSettingMode == "Focus")
				jsonFileName = FolderName + "FocusSetting_CHS-K.json";
			DeleteFileA((LPCSTR)jsonFileName);

			THEAPP.strAutoSettingMode == "";
			THEAPP.iAutoSettingCountCurrent = 0;
			THEAPP.iAutoSettingCountEnd = 1;
		}
	}
	catch (const std::exception& e)
	{
		THEAPP.strAutoSettingMode == "";
		THEAPP.iAutoSettingCountCurrent = 0;
		THEAPP.iAutoSettingCountEnd = 1;
	}
}


void  CHandlerService::Get_CycleStop()
{
	THEAPP.m_pInspectService->SetCycleStopStatus(TRUE);

	// Vision SW 자동 재실행 (추후 시퀀스 추가 후 적용)
	if (0)
	{
		TCHAR szFilePath[MAX_PATH];
		GetModuleFileName(NULL, szFilePath, MAX_PATH);

		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;

		CreateProcess(szFilePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	}
}

void  CHandlerService::Get_LoadComplete(CString sVisionType, CString sJigNo, CString sLotID, CString sMzNo, CString sTrayNo, CString sModuleNo, CString sHeight, CString sBarcode)
{
	int iPcVisionNo, iStageNo;
	CString sVisionCamType_Comm;
	BOOL bVisionFindCheck = FALSE;
	for (iStageNo = 0; iStageNo < STAGE_NUMBER_MAX; iStageNo++)
	{
		for (iPcVisionNo = 0; iPcVisionNo <= VISION_NUMBER_MAX; iPcVisionNo++)
		{
			if (iStageNo == STAGE_NUMBER_MAX - 1 && iPcVisionNo == VISION_NUMBER_MAX)
				return;

			if (iPcVisionNo == VISION_NUMBER_MAX)
				continue;

			sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];
			if (sVisionCamType_Comm == sVisionType)
			{
				bVisionFindCheck = TRUE;
				break;
			}
		}

		if (bVisionFindCheck)
			break;
	}

	int iMzNo, iTrayNo, iModuleNo, iJigNo;
	iMzNo = atoi((LPSTR)(LPCSTR)sMzNo);
	iJigNo = atoi((LPSTR)(LPCSTR)sJigNo);
#ifndef POC_TEST
	if (iJigNo % 2 == 0)
		iPcVisionNo = iPcVisionNo + 2;
#endif

	THEAPP.m_pInspectService->m_sLotID_H[iPcVisionNo] = sLotID;
	THEAPP.m_pInspectService->m_iMzNo_H[iPcVisionNo] = iMzNo;
	THEAPP.m_pInspectService->m_iJigNo_H[iPcVisionNo] = iJigNo;
	THEAPP.m_pInspectService->m_iStageNo_H[iPcVisionNo] = iStageNo;

	if (sVisionType == "T1" || sVisionType == "T2" || sVisionType == "TC")
		THEAPP.m_pInspectService->m_dHeight_H[iPcVisionNo] = atof((LPSTR)(LPCSTR)sHeight);
	else
		THEAPP.m_pInspectService->m_dHeight_H[iPcVisionNo] = 0;

	THEAPP.m_pInspectService->m_iTrayNo_H[iPcVisionNo] = EMPTY_TRAY_MODULE;
	THEAPP.m_pInspectService->m_iModuleNo_H[iPcVisionNo] = EMPTY_TRAY_MODULE;

	iTrayNo = atoi((LPSTR)(LPCSTR)sTrayNo);
	iModuleNo = atoi((LPSTR)(LPCSTR)sModuleNo);

	THEAPP.m_pInspectService->m_iTrayNo_H[iPcVisionNo] = iTrayNo;
	THEAPP.m_pInspectService->m_iModuleNo_H[iPcVisionNo] = iModuleNo;

	THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = FALSE;
	THEAPP.m_bLoadCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

	// if (sBarcode != "")
	//	THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sBarcode;

	THEAPP.m_pInspectService->InspectionMove(iPcVisionNo, iMzNo);				// 스캔과 검사 시작
}

void  CHandlerService::Get_LoadComplete(CString sVisionType, CString sLotID, CString sMzNo, CString sTrayID, CString sTrayNo, CString sModuleNo)
{
	int iPcVisionNo, iStageNo;
	CString sVisionCamType_Comm;
	BOOL bVisionFindCheck = FALSE;
	for (iStageNo = 0; iStageNo < STAGE_NUMBER_MAX; iStageNo++)
	{
		for (iPcVisionNo = 0; iPcVisionNo <= VISION_NUMBER_MAX; iPcVisionNo++)
		{
			if (iStageNo == STAGE_NUMBER_MAX - 1 && iPcVisionNo == VISION_NUMBER_MAX)
				return;

			if (iPcVisionNo == VISION_NUMBER_MAX)
				continue;

			sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];
			if (sVisionCamType_Comm == sVisionType)
			{
				bVisionFindCheck = TRUE;
				break;
			}
		}

		if (bVisionFindCheck)
			break;
	}

	int iMzNo, iTrayNo, iModuleNo;
	iMzNo = atoi((LPSTR)(LPCSTR)sMzNo);

	THEAPP.m_pInspectService->m_sLotID_H[iPcVisionNo] = sLotID;
	THEAPP.m_pInspectService->m_iMzNo_H[iPcVisionNo] = iMzNo;
	THEAPP.m_pInspectService->m_iJigNo_H[iPcVisionNo] = 1;
	THEAPP.m_pInspectService->m_iStageNo_H[iPcVisionNo] = iStageNo;
	THEAPP.m_pInspectService->m_dHeight_H[iPcVisionNo] = 0;

	THEAPP.m_pInspectService->m_iTrayNo_H[iPcVisionNo] = EMPTY_TRAY_MODULE;
	THEAPP.m_pInspectService->m_iModuleNo_H[iPcVisionNo] = EMPTY_TRAY_MODULE;

	iTrayNo = atoi((LPSTR)(LPCSTR)sTrayNo);
	iModuleNo = atoi((LPSTR)(LPCSTR)sModuleNo);

	THEAPP.m_pInspectService->m_sTrayID_H[iPcVisionNo] = sTrayID;
	THEAPP.m_pInspectService->m_iTrayNo_H[iPcVisionNo] = iTrayNo;
	THEAPP.m_pInspectService->m_iModuleNo_H[iPcVisionNo] = iModuleNo;

	THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = FALSE;
	THEAPP.m_bLoadCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = TRUE;

	THEAPP.m_pInspectService->InspectionMove(iPcVisionNo, iMzNo);				// 스캔과 검사 시작
}

void  CHandlerService::Get_TrayLoaded(CString sLotID, CString sMzNo, CString sTrayNo)
{
	int iPcVisionNo, iMzNo, iTrayNo;
	iPcVisionNo = VISION_NUMBER_3;

	THEAPP.m_pInspectService->m_sLotID_H[iPcVisionNo] = sLotID;
	iMzNo = atoi((LPSTR)(LPCSTR)sMzNo);
	THEAPP.m_pInspectService->m_iMzNo_H[iPcVisionNo] = iMzNo;

	THEAPP.m_pInspectService->m_iTrayNo_H[iPcVisionNo] = EMPTY_TRAY_MODULE;
	THEAPP.m_pInspectService->m_iModuleNo_H[iPcVisionNo] = EMPTY_TRAY_MODULE;

	iTrayNo = atoi((LPSTR)(LPCSTR)sTrayNo);
	THEAPP.m_pInspectService->m_iTrayNo_H[iPcVisionNo] = iTrayNo;
	THEAPP.m_pInspectService->m_iModuleNo_H[iPcVisionNo] = 1;

	THEAPP.m_pInspectService->InspectionMove(iPcVisionNo, iMzNo);				// 스캔과 검사 시작
}

void  CHandlerService::Get_BarcodeUpdate(CString sLotID, CString sLotNo, CString sTrayNo, CString sModuleNo, CString sBarcodeID)
{
	return;

	int iMzNo, iTrayNo, iModuleNo;

	iMzNo = atoi((LPSTR)(LPCSTR)sLotNo);
	iTrayNo = atoi((LPSTR)(LPCSTR)sTrayNo);
	iModuleNo = atoi((LPSTR)(LPCSTR)sModuleNo);
	THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1] = sBarcodeID;
}

void  CHandlerService::Get_HistoryUpdate(CString sMzNo, CString sTrayNo, CString sModuleNo, CString sBarcodeID,
										 CString sLoadPicker, CString sLoadPickerIndex, CString sVisionStageNo, CString sVisionStageIndex, CString sUnloadPickerNo,
										 CString sUnloadPickerIndex, CString sJudge, CString sGoodTrayNo, CString sGoodTrayPorket, CString sNGTrayNo, CString sNGTrayPorket)
{
	int iMzNo, iTrayNo, iModuleNo;

	iMzNo = atoi((LPSTR)(LPCSTR)sMzNo);
	iTrayNo = atoi((LPSTR)(LPCSTR)sTrayNo);
	iModuleNo = atoi((LPSTR)(LPCSTR)sModuleNo);

	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iLoadPicker = atoi((LPSTR)(LPCSTR)sLoadPicker);
	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iLoadPickerIndex = atoi((LPSTR)(LPCSTR)sLoadPickerIndex);
	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iVisionStageNo = atoi((LPSTR)(LPCSTR)sVisionStageNo);
	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iVisionStageIndex = atoi((LPSTR)(LPCSTR)sVisionStageIndex);
	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iUnloadPickerNo = atoi((LPSTR)(LPCSTR)sUnloadPickerNo);
	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iUnloadPickerIndex = atoi((LPSTR)(LPCSTR)sUnloadPickerIndex);
	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].sJudge = sJudge;
	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iGoodTrayNo = atoi((LPSTR)(LPCSTR)sGoodTrayNo);
	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iGoodTrayPorket = atoi((LPSTR)(LPCSTR)sGoodTrayPorket);
	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iNGTrayNo = atoi((LPSTR)(LPCSTR)sNGTrayNo);
	THEAPP.m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iNGTrayPorket = atoi((LPSTR)(LPCSTR)sNGTrayPorket);
}

void  CHandlerService::Get_ModuleData_Splite(CString sREVData)
{
	char chSep = '~';
	char chSep2 = ',';
	CString sTemp1, sTemp2, sTemp3, sTemp4, sTemp5, sTemp6, sTemp7, sTemp8, sTemp9, sTemp10, strRecv;
	for (int i = 0; i < 8; i++)
	{
		strRecv = _T("");
		AfxExtractSubString(strRecv, sREVData, i, chSep);
		if (strRecv.GetLength() == 0) continue;
		AfxExtractSubString(sTemp9, strRecv, 0, chSep2);
		AfxExtractSubString(sTemp10, strRecv, 1, chSep2);
		AfxExtractSubString(sTemp1, strRecv, 2, chSep2);			// LotID
		AfxExtractSubString(sTemp2, strRecv, 3, chSep2);			// No (1~40)
		AfxExtractSubString(sTemp3, strRecv, 4, chSep2);			// ModuleID(Barcode)
		AfxExtractSubString(sTemp4, strRecv, 5, chSep2);			// Site
		AfxExtractSubString(sTemp5, strRecv, 6, chSep2);			// EqpID
		AfxExtractSubString(sTemp6, strRecv, 7, chSep2);			// EqpName
		AfxExtractSubString(sTemp7, strRecv, 8, chSep2);			// Para ( . . . )
		AfxExtractSubString(sTemp8, strRecv, 9, chSep2);			// Tool_Cavity

		Get_ModuleData(sTemp1, sTemp2, sTemp3, sTemp4, sTemp5, sTemp6, sTemp7, sTemp8);
	}
}

void  CHandlerService::Get_ModuleData(CString sLotID, CString sNo, CString sBarcodeID, CString sSite, CString sEqpID, CString sEqpName, CString sPara, CString sToolCavity)
{
	int iNo;
	iNo = atoi((LPSTR)(LPCSTR)sNo);

	int iSelectedMzIndex = -1;

	for (int i = 0; i < MAX_MAGAZINE_NO; i++)
	{
		if (sLotID == THEAPP.m_StructModuleDataInfo[i].sLotID)
		{
			iSelectedMzIndex = i;
			break;
		}
	}

	if (iSelectedMzIndex < 0)
	{
		for (int i = 0; i < MAX_MAGAZINE_NO; i++)
		{
			if (THEAPP.m_StructModuleDataInfo[i].bUsed == FALSE)
			{
				iSelectedMzIndex = i;
				THEAPP.m_StructModuleDataInfo[i].sLotID = sLotID;

				THEAPP.m_StructModuleDataInfo[i].bUsed = TRUE;

				break;
			}
		}
	}

	if (iSelectedMzIndex >= 0)
	{
		THEAPP.m_StructModuleDataInfo[iSelectedMzIndex].MesModuleData[iNo - 1].iNo = iNo;
		THEAPP.m_StructModuleDataInfo[iSelectedMzIndex].MesModuleData[iNo - 1].sModuleBarcode = sBarcodeID;
		THEAPP.m_StructModuleDataInfo[iSelectedMzIndex].MesModuleData[iNo - 1].sSite = sSite;
		THEAPP.m_StructModuleDataInfo[iSelectedMzIndex].MesModuleData[iNo - 1].sEqpID = sEqpID;
		THEAPP.m_StructModuleDataInfo[iSelectedMzIndex].MesModuleData[iNo - 1].sEqpName = sEqpName;
		THEAPP.m_StructModuleDataInfo[iSelectedMzIndex].MesModuleData[iNo - 1].sPara = sPara;
		THEAPP.m_StructModuleDataInfo[iSelectedMzIndex].MesModuleData[iNo - 1].sToolCavity = sToolCavity;
	}
}

void  CHandlerService::Get_PositionReply(CString sVisionType, int iJigNo, double dCameraZ, double dLightZ, double dHeadX, double dStageY, double dTilt, double dRotate)
{
	THEAPP.m_pTabControlDlg->m_pMotionControlDlg->m_dEditCurPos_Z = dCameraZ;
	THEAPP.m_pTabControlDlg->m_pMotionControlDlg->m_dEditCurPos_LightZ = dLightZ;
	THEAPP.m_pTabControlDlg->m_pMotionControlDlg->m_dEditCurPos_X = dHeadX;
	THEAPP.m_pTabControlDlg->m_pMotionControlDlg->m_dEditCurPos_Y = dStageY;
	THEAPP.m_pTabControlDlg->m_pMotionControlDlg->m_dEditCurPos_T = dTilt;
	THEAPP.m_pTabControlDlg->m_pMotionControlDlg->m_dEditCurPos_R = dRotate;

	THEAPP.m_pTabControlDlg->m_pMotionControlDlg->UpdateData(FALSE);
}

void  CHandlerService::Get_MoveComplete(CString sVisionType, int iJigNo)
{
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	int iPcVisionNo, iStageNo;
	CString sVisionCamType_Comm;
	BOOL bVisionFindCheck = FALSE;
	for (iStageNo = 0; iStageNo < STAGE_NUMBER_MAX; iStageNo++)
	{
		for (iPcVisionNo = 0; iPcVisionNo <= VISION_NUMBER_MAX; iPcVisionNo++)
		{
			if (iStageNo == STAGE_NUMBER_MAX - 1 && iPcVisionNo == VISION_NUMBER_MAX)
				return;

			if (iPcVisionNo == VISION_NUMBER_MAX)
				continue;

			sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];
			if (sVisionCamType_Comm == sVisionType)
			{
				bVisionFindCheck = TRUE;
				break;
			}
		}

		if (bVisionFindCheck)
			break;
	}

	if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
	{
		if (iJigNo % 2 == 0)
			iPcVisionNo = iPcVisionNo + 2;
	}

	m_bMotionMoveComplete[iPcVisionNo] = TRUE;
}

void  CHandlerService::Get_MoveComplete(CString sVisionType)
{
	int iVisionType = -1;

	if (sVisionType == "TC")
		iVisionType = VISION_NUMBER_1;
	else if (sVisionType == "BC")
		iVisionType = VISION_NUMBER_2;
	else if (sVisionType == "SC")
		iVisionType = VISION_NUMBER_3;
	else if (sVisionType == "B1")
		iVisionType = VISION_NUMBER_4;
	else if (sVisionType == "B2")
		iVisionType = VISION_NUMBER_4_2;
	else if (sVisionType == "TA")
		iVisionType = VISION_NUMBER_4_3;

	m_bLensMotionMoveComplete[iVisionType] = TRUE;
}

void CHandlerService::Get_ShiftComplete(CString sVisionType)
{
	int iPcVisionNo, iStageNo;
	CString sVisionCamType_Comm;
	BOOL bVisionFindCheck = FALSE;
	for (iStageNo = 0; iStageNo < STAGE_NUMBER_MAX; iStageNo++)
	{
		for (iPcVisionNo = 0; iPcVisionNo <= VISION_NUMBER_MAX; iPcVisionNo++)
		{
			if (iStageNo == STAGE_NUMBER_MAX - 1 && iPcVisionNo == VISION_NUMBER_MAX)
				return;

			if (iPcVisionNo == VISION_NUMBER_MAX)
				continue;

			sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];
			if (sVisionCamType_Comm == sVisionType)
			{
				bVisionFindCheck = TRUE;
				break;
			}
		}

		if (bVisionFindCheck)
			break;
	}

	m_bShiftMoveComplete[iPcVisionNo] = TRUE;
}

void CHandlerService::Get_SmoveComplete(CString sVisionType)
{
	int iPcVisionNo, iStageNo;
	CString sVisionCamType_Comm;
	BOOL bVisionFindCheck = FALSE;
	for (iStageNo = 0; iStageNo < STAGE_NUMBER_MAX; iStageNo++)
	{
		for (iPcVisionNo = 0; iPcVisionNo <= VISION_NUMBER_MAX; iPcVisionNo++)
		{
			if (iStageNo == STAGE_NUMBER_MAX - 1 && iPcVisionNo == VISION_NUMBER_MAX)
				return;

			if (iPcVisionNo == VISION_NUMBER_MAX)
				continue;

			sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];
			if (sVisionCamType_Comm == sVisionType)
			{
				bVisionFindCheck = TRUE;
				break;
			}
		}

		if (bVisionFindCheck)
			break;
	}

	m_bShiftMoveComplete[iPcVisionNo] = TRUE;
}

void  CHandlerService::Get_TurnComplete(CString sVisionType)
{
	m_bTurnMoveComplete = TRUE;
}

void  CHandlerService::Get_ReloadComplete(CString sPCType)
{
	if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
		THEAPP.m_pHandlerService->Restart_Inspector(FALSE);

	THEAPP.m_pInspectService->SetReloadStatus(TRUE);
}

void  CHandlerService::Get_NetworkConnect()
{
	BOOL bConnectCheck = TRUE;
	if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLAS || THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS || THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS ||
		THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS || THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
	{
		bConnectCheck = THEAPP.ChangeLAS(0);
		bConnectCheck = THEAPP.ConnectStatusLAS(0);
	}

	if (bConnectCheck == TRUE)
		Set_NetworkConnect(0);
	else
		Set_NetworkConnect(1);
}

/////////////////////////////////////////////////////////////////////
// UDP Socket Message

LRESULT CHandlerService::OnClientConnect(WPARAM wLocalPort, LPARAM lConnect)
{
	UINT nLocalPort = (UINT)wLocalPort;

	if (nLocalPort != m_nLocalPort)
		return 0;

	m_bConnected = (BOOL)lConnect;

	return 0;
}

LRESULT CHandlerService::OnClientReceive(WPARAM wLocalPort, LPARAM lParam)
{
	CString strLog;
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	UINT nLocalPort = (UINT)wLocalPort;
	if (nLocalPort != m_nLocalPort)
		return 0;

	BYTE byRecv[1024] = { 0 };

	int nLen = m_TCPHandler.Read_Socket(byRecv);

	if (nLen < 1)
	{
		strLog.Format("[Handler(V<-H)] Receive data zero (%d)", nLen);
		THEAPP.m_log_handler->warn("{}", LOG_CSTR(strLog));
		return 0;
	}

	CString strRecvSocket;
	strRecvSocket.Format("%s", byRecv);
	m_strRecvCmd += strRecvSocket;

	while (!m_strRecvCmd.IsEmpty())
	{
		int nStart = m_strRecvCmd.Find("@");
		int nEnd = m_strRecvCmd.Find("\n");

		if (nEnd < 0)
			break;	// 버퍼에 들어오는 중...

		if (nStart < 0 || nStart > nEnd)
		{
			strLog.Format("[Handler(V<-H)] <Error> %s : Start: %d, End: %d", m_strRecvCmd, nStart, nEnd);
			THEAPP.m_log_handler->warn("{}", LOG_CSTR(strLog));

			m_strRecvCmd.Delete(0, nEnd + 1);	// 쓰레기값이 채워져 있어서...
			continue;
		}

		CString strRecv = m_strRecvCmd.Mid(nStart + 1, nEnd - nStart - 1);
		m_strRecvCmd.Delete(0, nEnd + 1);

		char chSep = ',';
		CString strCmd, strOp;
		CString sPEEED1, sPEEED2, sPEEED3, sPEEED4, sPEEED5;
		CString sTemp1, sTemp2, sTemp3, sTemp4, sTemp5, sTemp6, sTemp7, sTemp8, sTemp9, sTemp10, sTemp11, sTemp12, sTemp13, sTemp14, sTemp15;
		CString sMzNo, sTrayNo, sModuleNo;

		AfxExtractSubString(strCmd, strRecv, 0, chSep);
		AfxExtractSubString(strOp, strRecv, 1, chSep);

		// Handler Log ////////////////////////////////////////
		strLog.Format("[Handler(V<-H)] %s", strRecv);
		THEAPP.m_log_handler->info("{}", LOG_CSTR(strLog));

		/////////////////////////////////////////////////////////

		if (strCmd == "STATUS") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);
			if (strOp == "REQUEST")	Get_StatusRequest();
			else if (strOp == "REPLY") Get_StatusReply(sTemp1);
			else if (strOp == "UPDATE")	Get_StatusUpdate(sTemp1);
		}

		else if (strCmd == "MODE") {
			if (strOp == "REQUEST") Get_ModeRequest();
		}

		else if (strCmd == "MODEL") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);
			int iFlag = atoi((LPSTR)(LPCSTR)sTemp1);
			if (strOp == "REPLY") Get_ModelReply(iFlag);
		}

#ifdef SINGLE_LENS
		else if (strCmd == "LOT") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);		// Lot ID
			AfxExtractSubString(sTemp2, strRecv, 3, chSep);		// Lot No
			AfxExtractSubString(sTemp3, strRecv, 4, chSep);		// Tray No
			AfxExtractSubString(sTemp4, strRecv, 5, chSep);		// Module No
			AfxExtractSubString(sTemp5, strRecv, 6, chSep);		// Handler Model Name

			if (strOp == "START") Get_LotStart(sTemp1, sTemp2, sTemp3, sTemp4, sTemp5);
			else if (strOp == "END") Get_LotEnd(sTemp1, sTemp2);
			else if (strOp == "RDYDONE") Get_LotReadyDone(sTemp1, sTemp2);
		}
#else
		else if (strCmd == "LOT") {
#ifdef POC_TEST
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);		// Lot ID
			sTemp2 = "1";										// Lot No
			sTemp3 = "1";										// Tray No
			AfxExtractSubString(sTemp4, strRecv, 3, chSep);		// Module No
			AfxExtractSubString(sTemp5, strRecv, 4, chSep);		// Handler Model Name
			sTemp6 = "0";										// Model Auto Load
#else
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);		// Lot ID
			AfxExtractSubString(sTemp2, strRecv, 3, chSep);		// Lot No
			AfxExtractSubString(sTemp3, strRecv, 4, chSep);		// Tray No
			AfxExtractSubString(sTemp4, strRecv, 5, chSep);		// Module No
			AfxExtractSubString(sTemp5, strRecv, 6, chSep);		// Handler Model Name
			AfxExtractSubString(sTemp6, strRecv, 7, chSep);		// Model Auto Load
#endif

			if (strOp == "START") Get_LotStart(sTemp1, sTemp2, sTemp3, sTemp4, sTemp5, sTemp6);
			else if (strOp == "END") Get_LotEnd(sTemp1, sTemp2);
		}
#endif

#ifdef SINGLE_LENS
		else if (strCmd == "LOAD") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);		// VIsion: TC, BC
			AfxExtractSubString(sTemp2, strRecv, 3, chSep);		// MZ ID
			AfxExtractSubString(sTemp3, strRecv, 4, chSep);		// MZ No
			AfxExtractSubString(sTemp4, strRecv, 5, chSep);		// Zig ID
			AfxExtractSubString(sTemp5, strRecv, 6, chSep);		// Zig No
			AfxExtractSubString(sTemp6, strRecv, 7, chSep);		// Lens No

			if (strOp == "COMPLETE") Get_LoadComplete(sTemp1, sTemp2, sTemp3, sTemp4, sTemp5, sTemp6);
		}
#else

		else if (strCmd == "LOAD") {
#ifdef POC_TEST
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);		// VIsion: T1, T2, B1, B2
			sTemp2 = "0";										// Jig No 정보 없음
			AfxExtractSubString(sTemp3, strRecv, 3, chSep);		// Lot ID
			sTemp4 = "1";										// Lot No 정보 없음
			sTemp5 = "1";										// Tray No 정보 없음
			AfxExtractSubString(sTemp6, strRecv, 4, chSep);		// Module No
			sTemp7 = "0";										// Height 정보 없음
			sTemp8 = "";										// Barcode 정보 없음
#else
			if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
			{
				AfxExtractSubString(sTemp1, strRecv, 2, chSep);		// VIsion: T1, T2, B1, B2
				AfxExtractSubString(sTemp2, strRecv, 3, chSep);		// Jig No
				AfxExtractSubString(sTemp3, strRecv, 4, chSep);		// Lot_ID
				AfxExtractSubString(sTemp4, strRecv, 5, chSep);		// Lot No
				AfxExtractSubString(sTemp5, strRecv, 6, chSep);		// Tray No
				AfxExtractSubString(sTemp6, strRecv, 7, chSep);		// Module No
				sTemp7 = "0";										// Height AP-Tech 높이 정보 없음
				sTemp8 = "";										// Barcode AP-Tech 바코드 정보 없음
			}
			else
			{
				AfxExtractSubString(sTemp1, strRecv, 2, chSep);		// VIsion: T1, T2, B1, B2
				sTemp2 = "1";										// Jig No (BOI 전용이므로 0 고정)
				AfxExtractSubString(sTemp3, strRecv, 3, chSep);		// Lot_ID
				AfxExtractSubString(sTemp4, strRecv, 4, chSep);		// Lot No
				AfxExtractSubString(sTemp5, strRecv, 5, chSep);		// Tray No
				AfxExtractSubString(sTemp6, strRecv, 6, chSep);		// Module No
				AfxExtractSubString(sTemp7, strRecv, 7, chSep);		// Height
				AfxExtractSubString(sTemp8, strRecv, 8, chSep);		// Barcode - APTech로부터 받는 바코드
			}
#endif

			if (strOp == "COMPLETE") Get_LoadComplete(sTemp1, sTemp2, sTemp3, sTemp4, sTemp5, sTemp6, sTemp7, sTemp8);
		}
#endif

		else if (strCmd == "BARCODE") {
#ifdef POC_TEST
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);			// Lot ID
			sTemp2 = "1";											// Lot No
			sTemp3 = "1";											// Tray No
			AfxExtractSubString(sTemp4, strRecv, 3, chSep);			// Module No
			AfxExtractSubString(sTemp5, strRecv, 4, chSep);			// Barcode
#else
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);			// Lot ID
			AfxExtractSubString(sTemp2, strRecv, 3, chSep);			// Lot No
			AfxExtractSubString(sTemp3, strRecv, 4, chSep);			// Tray No
			AfxExtractSubString(sTemp4, strRecv, 5, chSep);			// Module No
			AfxExtractSubString(sTemp5, strRecv, 6, chSep);			// Barcode
#endif
			if (strOp == "UPDATE") Get_BarcodeUpdate(sTemp1, sTemp2, sTemp3, sTemp4, sTemp5);
		}

		else if (strCmd == "HISTORY") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);			// PortNo(1~6)
			AfxExtractSubString(sTemp2, strRecv, 3, chSep);			// LoadTrayNo(1~8)
			AfxExtractSubString(sTemp3, strRecv, 4, chSep);			// LoadPocket(1~40)
			AfxExtractSubString(sTemp4, strRecv, 5, chSep);			// Barcode
			AfxExtractSubString(sTemp5, strRecv, 6, chSep);			// LoadPicker(1,2)
			AfxExtractSubString(sTemp6, strRecv, 7, chSep);			// LoadPickerIndex(1~8)
			AfxExtractSubString(sTemp7, strRecv, 8, chSep);			// VisionStageNo(1~4)
			AfxExtractSubString(sTemp8, strRecv, 9, chSep);			// VisionStageIndex(1~8)
			AfxExtractSubString(sTemp9, strRecv, 10, chSep);		// UnloadPickerNo(1,2)
			AfxExtractSubString(sTemp10, strRecv, 11, chSep);		// UnloadPickerIndex(1~8)
			AfxExtractSubString(sTemp11, strRecv, 12, chSep);		// Judge(N,G)
			AfxExtractSubString(sTemp12, strRecv, 13, chSep);		// GoodTrayNo(1~8)
			AfxExtractSubString(sTemp13, strRecv, 14, chSep);		// GoodTrayPorket(1~40)
			AfxExtractSubString(sTemp14, strRecv, 15, chSep);		// NGTrayNo(1~8)
			AfxExtractSubString(sTemp15, strRecv, 16, chSep);		// NGTrayPorket(1~40)
			if (strOp == "REPLY") Get_HistoryUpdate(sTemp1, sTemp2, sTemp3, sTemp4, sTemp5, sTemp6, sTemp7, sTemp8, sTemp9, sTemp10, sTemp11, sTemp12, sTemp13, sTemp14, sTemp15);
		}

		else if (strCmd == "MODULE") {
			if (strOp == "DATA") Get_ModuleData_Splite(strRecv);
		}

		else if (strCmd == "POSITION") {
			int iJigNo;
			double dPositionZ, dLightZ, dStageX, dStageY, dTilt, dRotate;
			if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
			{
				AfxExtractSubString(sTemp1, strRecv, 2, chSep);			// Vision Type (B1, B2, T1, T2)
				AfxExtractSubString(sTemp2, strRecv, 3, chSep);			// Jig No
				AfxExtractSubString(sTemp3, strRecv, 4, chSep);			// Camera Z
				AfxExtractSubString(sTemp4, strRecv, 5, chSep);			// Light(Laser) Z
				AfxExtractSubString(sTemp5, strRecv, 6, chSep);			// Stage X
				AfxExtractSubString(sTemp6, strRecv, 7, chSep);			// Stage Y
				AfxExtractSubString(sTemp7, strRecv, 8, chSep);			// Stage T
				AfxExtractSubString(sTemp8, strRecv, 9, chSep);			// Stage R

				iJigNo = atof((LPSTR)(LPCSTR)sTemp2);
				dPositionZ = atof((LPSTR)(LPCSTR)sTemp3);
				dLightZ = atof((LPSTR)(LPCSTR)sTemp4);
				dStageX = atof((LPSTR)(LPCSTR)sTemp5);
				dStageY = atof((LPSTR)(LPCSTR)sTemp6);
				dTilt = atof((LPSTR)(LPCSTR)sTemp7);
				dRotate = atof((LPSTR)(LPCSTR)sTemp8);
			}
			else
			{
				AfxExtractSubString(sTemp1, strRecv, 2, chSep);			// Vision Type (B1, B2, T1, T2)
				sTemp2 = "0";											// Jig No 정보 없음
				AfxExtractSubString(sTemp3, strRecv, 3, chSep);			// Camera Z
				AfxExtractSubString(sTemp4, strRecv, 4, chSep);			// Light(Laser) Z
				AfxExtractSubString(sTemp5, strRecv, 5, chSep);			// Stage X
				AfxExtractSubString(sTemp6, strRecv, 6, chSep);			// Stage Y
				AfxExtractSubString(sTemp7, strRecv, 7, chSep);			// Stage T
				AfxExtractSubString(sTemp8, strRecv, 8, chSep);			// Stage R

				iJigNo = atof((LPSTR)(LPCSTR)sTemp2);
				dPositionZ = atof((LPSTR)(LPCSTR)sTemp3);
				dLightZ = atof((LPSTR)(LPCSTR)sTemp4);
				dStageX = atof((LPSTR)(LPCSTR)sTemp5);
				dStageY = atof((LPSTR)(LPCSTR)sTemp6);
				dTilt = atof((LPSTR)(LPCSTR)sTemp7);
				dRotate = atof((LPSTR)(LPCSTR)sTemp8);
			}

			if (strOp == "REPLY") Get_PositionReply(sTemp1, iJigNo, dPositionZ, dLightZ, dStageX, dStageY, dTilt, dRotate);
		}

#if defined(SINGLE_LENS) || defined(ASSY_LENS)
		else if (strCmd == "MOVE") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);			// Vision Type (B1, B2, T1, T2)
			if (strOp == "COMPLETE")
			{

			}
		}
#else
		else if (strCmd == "MOVE") {
			if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
			{
				AfxExtractSubString(sTemp1, strRecv, 2, chSep);			// Vision Type (B1, B2, T1, T2)
				AfxExtractSubString(sTemp2, strRecv, 3, chSep);			// Jig No
			}
			else
			{
				AfxExtractSubString(sTemp1, strRecv, 2, chSep);			// Vision Type (B1, B2, T1, T2)
				sTemp2 = "1";											// Jig No (BOI 전용이므로 1 고정)
			}

			int iJigNo = atof((LPSTR)(LPCSTR)sTemp2);
			if (strOp == "COMPLETE")
			{
				if ((strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS") &&
					THEAPP.m_iModeSwitch != MODE_ADMIN_TEACH_VIEW)
				{
					if (iJigNo == 1 || iJigNo == 3)
					{
						Get_MoveComplete(sTemp1, iJigNo);
						Get_MoveComplete(sTemp1, iJigNo + 1);
					}
					else if (iJigNo == 5)
						Get_MoveComplete(sTemp1, iJigNo);
				}
				else
					Get_MoveComplete(sTemp1, iJigNo);
			}
		}
#endif

		else if (strCmd == "SHIFT") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);
			if (strOp == "COMPLETE") Get_ShiftComplete(sTemp1);
		}

		else if (strCmd == "SMOVE") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);
			if (strOp == "COMPLETE") Get_SmoveComplete(sTemp1);
		}

		else if (strCmd == "TURN") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);
			if (strOp == "COMPLETE") Get_TurnComplete(sTemp1);
		}

		else if (strCmd == "RELOAD") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);
			if (strOp == "COMPLETE") Get_ReloadComplete(sTemp1);
		}

		else if (strCmd == "TIME") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);
			if (strOp == "UPDATE") Get_TimeUpdate(sTemp1);
		}

		else if (strCmd == "CYCLE") {
			if (strOp == "STOP")	Get_CycleStop();
		}

		else if (strCmd == "INITIAL") {
			if (strOp == "REQUEST")	Get_CycleStop();
		}

		else if (strCmd == "NETWORK") {
			if (strOp == "REQUEST")	Get_NetworkConnect();
		}

		else if (strCmd == "CONNECT") {
			if (strOp == "REQUEST")	Get_ConnectRequest();
		}

		else if (strCmd == "RUN") {
			if (strOp == "COMPLETE") Get_RunComplete();
		}

		else if (strCmd == "TRAY") {
			AfxExtractSubString(sTemp1, strRecv, 2, chSep);			// Lot ID
			AfxExtractSubString(sTemp2, strRecv, 3, chSep);			// Lot No
			AfxExtractSubString(sTemp3, strRecv, 4, chSep);			// Tray No
			if (strOp == "LOADED") Get_TrayLoaded(sTemp1, sTemp2, sTemp3);
		}

		THEAPP.DoEvents(1);
	}

	return 1;
}

LRESULT CHandlerService::OnClientClose(WPARAM wLocalPort, LPARAM lParam)
{
	UINT nLocalPort = (UINT)wLocalPort;
	if (nLocalPort != m_nLocalPort)
		return 0;

	m_bConnected = FALSE;
	return 0;
}

LRESULT CHandlerService::OnTcpReceive(WPARAM wParam, LPARAM lParam)
{
	CClientSocket* pClientSocket = (CClientSocket*)lParam;
	int iClientIdx = pClientSocket->GetClientIndex();
	if (iClientIdx < 0 || iClientIdx >= LIGHT_CONTROLLER_NUMBER_MAX)
		return 0;

	// Handler Log ////////////////////////////////////////
	CString strLog;
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	// 수신 데이터 가져오기
	CString strRecvData = CString((char*)mp_TcpHandler[iClientIdx]->m_byteReceive, mp_TcpHandler[iClientIdx]->m_nReceiveLen);

	// 누적
	m_strTcpBuffer[iClientIdx] += strRecvData;

	// 메시지 파싱 (구분자: "/0")
	int iPos = -1;
	while ((iPos = m_strTcpBuffer[iClientIdx].Find("/0")) >= 0)
	{
		CString strOneMessage = m_strTcpBuffer[iClientIdx].Left(iPos + 2); // "/0"까지 포함
		m_strTcpBuffer[iClientIdx] = m_strTcpBuffer[iClientIdx].Mid(iPos + 2); // 남은 부분 유지

		// 줄바꿈 제거
		strOneMessage.Replace("\r", "");
		strOneMessage.Replace("\n", "");
		strOneMessage.Trim();

		// 로깅
		if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
			strLog.Format("[Vision%d<-LightController] %s", iClientIdx + 1, strOneMessage);
		else
			strLog.Format("[%s<-LightController] %s", THEAPP.m_ModelDefineInfo.m_strVisionName[iClientIdx], strOneMessage);

		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

void CHandlerService::Set_StatusRequest()
{
	CString	strSendMsg;
	strSendMsg = "@STATUS,REQUEST\n";
	Send_Command(strSendMsg);
}

void CHandlerService::Set_StatusReply()
{
	CString	strSendMsg;
	strSendMsg.Format("@STATUS,REPLY,%d\n", m_iHandlerStatus);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_StatusUpdate(int nStatus)
{
	CString	strSendMsg;
	strSendMsg.Format("@STATUS,UPDATE,%d\n", nStatus);
	Send_Command(strSendMsg, FALSE);
}

void CHandlerService::Set_ModeReplay(int iMode)//Type(0:None, 1:Inspect, 2:Teach, 3:Demo, 4:Test, 5:Debug, 6:Step, 7:Home, 8:Manual)
{
	CString	strSendMsg;
	strSendMsg.Format("@MODE,REPLAY,%d\n", iMode);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_ModeUpdate(int iMode)//Type(0:None, 1:Inspect, 2:Teach, 3:Demo, 4:Test, 5:Debug, 6:Step, 7:Home, 8:Manual)
{
	CString	strSendMsg;
	strSendMsg.Format("@MODE,UPDATE,%d\n", iMode);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_ModelCreate(CString sModelname)
{
	CString	strSendMsg;
	strSendMsg.Format("@MODEL,CREATE,%s\n", sModelname);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_ModelUpdate(CString sModelname)
{
	CString	strSendMsg;
	strSendMsg.Format("@MODEL,UPDATE,%s\n", sModelname);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_LotReply()
{
	CString	strSendMsg;
	strSendMsg.Format("@LOT,REPLY\n");
	Send_Command(strSendMsg);
}

void CHandlerService::Set_LoadReply()
{
	CString	strSendMsg;
	strSendMsg.Format("@LOAD,REPLY\n");
	Send_Command(strSendMsg);
}

void CHandlerService::Set_LotReady(CString sLotID, int iMzNo) // Lot Ready 후 제어에 보낸다.
{
	CString	strSendMsg;
#ifdef POC_TEST
	strSendMsg.Format("@LOT,READY,%s\n", sLotID);
#else
	strSendMsg.Format("@LOT,READY,%s,%d\n", sLotID, iMzNo);
#endif
	Send_Command(strSendMsg);
}

void CHandlerService::Set_ScanComplete(CString sLotID, int iMzNo, int iJigNo, int iTrayNo, int iModuleNo, CString sVisionType)
{
	CString	strSendMsg;
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

#ifdef POC_TEST
	strSendMsg.Format("@SCAN,COMPLETE,%s,%s,%d\n", sVisionType, sLotID, iModuleNo);
#else
	if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
		strSendMsg.Format("@SCAN,COMPLETE,%s,%d,%s,%d,%d,%d\n", sVisionType, iJigNo, sLotID, iMzNo, iTrayNo, iModuleNo);
	else
		strSendMsg.Format("@SCAN,COMPLETE,%s,%s,%d,%d,%d\n", sVisionType, sLotID, iMzNo, iTrayNo, iModuleNo);
#endif

	Send_Command(strSendMsg);
}

void CHandlerService::Set_InspectComplete(CString sLotID, int iMzNo, int iTrayNo, int iModuleNo, CString sVisionType, CString sModuleResult, CString sDefectCode)//Inspection 완료 후 제어에 보낸다.
{
	CString	strSendMsg;

	if (sDefectCode == "G")
		sModuleResult = "G";

#ifdef POC_TEST
	strSendMsg.Format("@INSPECT,COMPLETE,%s,%s,%d,%s,%s\n", sVisionType, sLotID, iModuleNo, sModuleResult, sDefectCode);
#else
	strSendMsg.Format("@INSPECT,COMPLETE,%s,%s,%d,%d,%d,%s,%s\n", sVisionType, sLotID, iMzNo, iTrayNo, iModuleNo, sModuleResult, sDefectCode);
#endif

	Send_Command(strSendMsg);
}

void CHandlerService::Set_AlignReply(CString sLotID)
{
	CString	strSendMsg;

	strSendMsg.Format("@ALIGN,REPLY,%s\n", sLotID);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_LoadingAlignComplete(CString sLotID, int iMzNo, int iTrayNo, CString sTrayStatusResult, CString sModuleAlignResult, double dAvgDeltaXmm, double dAvgDeltaYmm)
{
	CString	strSendMsg;

	strSendMsg.Format("@ALIGN,COMPLETE,A1,%s,%d,%d,%s,%s,%.3lf,%.3lf\n", sLotID, iMzNo, iTrayNo, sTrayStatusResult, sModuleAlignResult, dAvgDeltaXmm, dAvgDeltaYmm);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_ShipTrayAlignComplete(CString sLotID, int iShipTrayType, int iTrayNo, int iSeqNo, double dDeltaXmm, double dDeltaYmm)
{
	CString	strSendMsg;

	strSendMsg.Format("@ALIGN,COMPLETE,A2,%s,%d,%d,%d,%.3lf,%.3lf\n", sLotID, iShipTrayType, iTrayNo, iSeqNo, dDeltaXmm, dDeltaYmm);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_BarcodeResult(CString sLotID, int iMzNo, int iTrayNo, int iModuleNo, CString sBarcodeID)
{
	CString	strSendMsg;

#ifdef POC_TEST
	strSendMsg.Format("@BARCODE,RESULT,%s,%d,%s\n", sLotID, iModuleNo, sBarcodeID);
#else
	strSendMsg.Format("@BARCODE,RESULT,%s,%d,%d,%d,%s\n", sLotID, iMzNo, iTrayNo, iModuleNo, sBarcodeID);
#endif

	Send_Command(strSendMsg);
}

void CHandlerService::Set_AngleResult(int iTrayNo, int iModuleNo, double dBenvolioAngle, CString sVisionType, double dBenvolioDeltaX, double dBenvolioDeltaY) // T1 완료 후 제어에 보낸다.
{
	CString	strSendMsg;

	strSendMsg.Format("@ANGLE,RESULT,%d,%d,%.3lf,%s,%.3lf,%.3lf\n", iTrayNo, iModuleNo, dBenvolioAngle, sVisionType, dBenvolioDeltaX, dBenvolioDeltaY);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_HomerResult(int iTrayNo, int iModuleNo, double dHomerAngle, CString sVisionType, double dHomerDeltaX, double dHomerDeltaY) // T1 완료 후 제어에 보낸다.
{
	CString	strSendMsg;

	strSendMsg.Format("@HOMER,RESULT,%d,%d,%.3lf,%s,%.3lf,%.3lf\n", iTrayNo, iModuleNo, dHomerAngle, sVisionType, dHomerDeltaX, dHomerDeltaY);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_PositionRequest(int iVisionType, int iStageNo, int iJigNo)//현재 모션 위치를 요구한다.
{
	CString	strSendMsg;
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
		strSendMsg.Format("@POSITION,REQUEST,%s,%d\n", THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionType][iStageNo], iJigNo);
	else
		strSendMsg.Format("@POSITION,REQUEST,%s\n", THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionType][iStageNo]);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_AMoveRequest(int iVisionType, int iStageNo, int iJigNo, double dCameraZ, double dLightZ, double dHeadX, double dStageY, double dStageT, double dStageR)
{
	CString	strSendMsg;
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
		strSendMsg.Format("@AMOVE,REQUEST,%s,%d,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf\n", THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionType][iStageNo], iJigNo, dCameraZ, dLightZ, dHeadX, dStageY, dStageT, dStageR);
	else
		strSendMsg.Format("@AMOVE,REQUEST,%s,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf\n", THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionType][iStageNo], dCameraZ, dLightZ, dHeadX, dStageY, dStageT, dStageR);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_ZPosAMoveRequest(double dZposition, int iVisionType)	//절대이동 구동 명령을 보낸다. (""이면 움직이지 않는다)
{
	CString	strSendMsg;

#ifdef SINGLE_LENS
	strSendMsg.Format("@AMOVE,REQUEST,%s,%.3lf\n", THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionType][0], dZposition);
#elif defined(ASSY_LENS)

	if (iVisionType>=VISION_NUMBER_1 && iVisionType <= VISION_NUMBER_3)
		strSendMsg.Format("@AMOVE,REQUEST,%s,%.3lf\n", THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionType][0], dZposition);
	else if (iVisionType == VISION_NUMBER_4)
		strSendMsg.Format("@AMOVE,REQUEST,B1,%.3lf\n", dZposition);
	else if (iVisionType == VISION_NUMBER_4_2)
		strSendMsg.Format("@AMOVE,REQUEST,B2,%.3lf\n", dZposition);
	else if (iVisionType == VISION_NUMBER_4_3)
		strSendMsg.Format("@AMOVE,REQUEST,TA,%.3lf\n", dZposition);

#endif

	Send_Command(strSendMsg);
}

void CHandlerService::Set_ShiftRequest(int iVisionType, CString sLotID, int iMzNo)	//Shift 명령을 보낸다. (Bottom1, Top2)
{
	CString	strSendMsg;

	strSendMsg.Format("@SHIFT,REQUEST,%s\n", THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionType][0]);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_SMoveRequest(double dZposition, int iVisionType, CString sLotID, int iMzNo)	// Shift + 절대이동 구동 명령을 보낸다. (""이면 움직이지 않는다)
{
	CString	strSendMsg;

	strSendMsg.Format("@SMOVE,REQUEST,%s,%.3lf\n", THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionType][0], dZposition);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_ReloadRequest(int iPCType)	// 검사 초기 상태를 요청한다. (PC1, PC2)
{
	CString	strSendMsg;

	strSendMsg.Format("@RELOAD,REQUEST,PC%d\n", iPCType + 1);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_ErrorRequest(int iErrorNo, CString strErrorMsg)
{
	CString	strSendMsg;

	strSendMsg.Format("@ERROR,REQUEST,%d,%s\n", iErrorNo, strErrorMsg);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_AlarmRequest(CString sVisionType, CString sLotID, int iMzNo, int iAlarmNo, CString sDefectName, int iDefectCount)	// 다발 불량
{
	CString	strSendMsg;

	strSendMsg.Format("@ALARM,REQUEST,%s,%s,%d,%d,%s,%d\n", sVisionType, sLotID, iMzNo, iAlarmNo, sDefectName, iDefectCount);

	//Send_Command(strSendMsg);
}

void CHandlerService::Set_TrayComplete(CString sLotID, int iMzNo, int iTrayNo, CString sTrayResult)
{
	CString	strSendMsg;
	strSendMsg.Format("@TRAY,COMPLETE,%s,%d,%d,%s\n", sLotID, iMzNo, iTrayNo, sTrayResult);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_TriggerRequest(CString sVision, CString sLotID, int iMzNo, CString sTrayID, int iZigNo, int iLensNo)
{
	CString	strSendMsg;

	strSendMsg.Format("@TRIGGER,REQUEST,%s,%s,%d,%s,%d,%d\n", sVision, sLotID, iMzNo, sTrayID, iZigNo, iLensNo);

	Send_Command(strSendMsg);
}

/////////////////////////////////////////////////////////////////////
// UDP Socket Send Message

void CHandlerService::Send_Command(CString strSend, BOOL bSaveLog)
{
	CString strLog;
	try
	{
		m_csSendCmd.Lock();

#ifdef HANDLER_USE
		char chSend[1024] = { 0 };
		int nLength = strSend.GetLength();
		memcpy(chSend, (LPSTR)(LPCSTR)strSend, nLength);

		m_TCPHandler.Write_Socket((BYTE*)chSend, nLength);
#endif

		m_csSendCmd.Unlock();

		if (bSaveLog)
		{
			// Handler Log 추가 ///////////////////////////////////
			strSend.TrimRight("\n");
			strLog.Format("[Handler(V->H)] %s", strSend);
			THEAPP.m_log_handler->info("{}", LOG_CSTR(strLog));
			///////////////////////////////////////////////////////
		}
	}
	catch (...)
	{
		// Handler Log 추가 ///////////////////////////////////
		strLog.Format("Send_Command Exception [Handler(V->H)] %s", strSend);
		THEAPP.m_log_handler->warn("{}", LOG_CSTR(strLog));
		///////////////////////////////////////////////////////
	}
}
/////////////////////////////////////////////////////////////////////

void CHandlerService::Send_Tcp(int iLightControllerIndex, CString strSend, BOOL bSaveLog)
{
	CString strLog;
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	try
	{
		m_csSendCmdTcp[iLightControllerIndex].Lock();

#ifdef HANDLER_USE
		int nLength = strSend.GetLength();
		mp_TcpHandler[iLightControllerIndex]->Send((LPCTSTR)strSend, nLength);
#endif

		m_csSendCmdTcp[iLightControllerIndex].Unlock();

		if (bSaveLog)
		{
			// Handler Log 추가 ///////////////////////////////////
			// 줄바꿈 제거
			CString strOneMessage = strSend;
			strOneMessage.Replace("\r", "");
			strOneMessage.Replace("\n", "");
			strOneMessage.Trim();

			if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
				strLog.Format("[Vision%d->Light Controller] %s", iLightControllerIndex + 1, strOneMessage);
			else
				strLog.Format("[%s->Light Controller] %s", THEAPP.m_ModelDefineInfo.m_strVisionName[iLightControllerIndex], strOneMessage);

			THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));
			///////////////////////////////////////////////////////
		}
	}
	catch (...)
	{
		// Handler Log 추가 ///////////////////////////////////
		if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
			strLog.Format("Send_Command Exception [Vision%d->Light Controller] %s", iLightControllerIndex + 1, strSend);
		else
			strLog.Format("Send_Command Exception [%s->Light Controller] %s", THEAPP.m_ModelDefineInfo.m_strVisionName[iLightControllerIndex], strSend);
		THEAPP.m_log_scan->warn("{}", LOG_CSTR(strLog));
		///////////////////////////////////////////////////////
	}
}

#include <winnetwk.h>
#pragma comment(lib, "Mpr")

BOOL CHandlerService::Connect_NetworkDrive(CString strDirve, CString strPath)
{
	TCHAR szRemoteName[128];
	DWORD dwLen = sizeof(szRemoteName);

	DWORD dwReturn = WNetGetConnection(strDirve, szRemoteName, &dwLen);
	if (dwReturn == NO_ERROR) return TRUE;	// Already Connected

	// 	dwReturn = WNetCancelConnection(strDirve, TRUE);
	// 	if (dwReturn != NO_ERROR) return FALSE;	// Already Disconnected

	NETRESOURCE NetR;
	NetR.dwType = RESOURCETYPE_DISK;				// 공유 디스크
	NetR.lpLocalName = (LPSTR)(LPCSTR)strDirve;		// 로컬 드라이브
	NetR.lpRemoteName = (LPSTR)(LPCSTR)strPath;	// 경로
	NetR.lpProvider = NULL;

	DWORD dwFlag = CONNECT_REDIRECT;	// CONNECT_UPDATE_PROFILE;
	dwReturn = WNetAddConnection2(&NetR, "mirero", "Administrator", dwFlag);
	if (dwReturn == NO_ERROR) return TRUE;	// Success Connect
	else return FALSE;						// Fail Connect
}

void CHandlerService::Set_TimeUpdate()
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strTime;
	strTime.Format("%04d-%02d-%02d %02d:%02d:%02d %03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	CString	strSendMsg;

	strSendMsg.Format("@TIME,UPDATE,%s\n", strTime);
	Send_Command(strSendMsg);
}

void CHandlerService::Get_TimeUpdate(CString sTime)
{
	// sTime Format : "2000-01-01 12:30:30 000"
	CString strTemp;
	SYSTEMTIME time;

	strTemp = sTime.Mid(0, 4);  time.wYear = atoi(strTemp);			// Year
	strTemp = sTime.Mid(5, 2);  time.wMonth = atoi(strTemp);		// Month
	strTemp = sTime.Mid(8, 2);  time.wDay = atoi(strTemp);			// Day
	strTemp = sTime.Mid(11, 2); time.wHour = atoi(strTemp);			// Hour
	strTemp = sTime.Mid(14, 2); time.wMinute = atoi(strTemp);		// Minite
	strTemp = sTime.Mid(17, 2); time.wSecond = atoi(strTemp);		// Second
	strTemp = sTime.Mid(20, 3); time.wMilliseconds = atoi(strTemp);	// MilliSecond

	SetLocalTime(&time);
}

void CHandlerService::Set_NetworkConnect(int iConnectStatus)
{
	CString	strSendMsg;
	strSendMsg.Format("@NETWORK,REPLY,%d\n", iConnectStatus);
	Send_Command(strSendMsg);
}

void CHandlerService::Save_ScanComplete(CString sLotID, int iMzNo, int iJigNo, int iTrayNo, int iModuleNo, CString sVisionType)
{
	CString strLog;
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	int iPcVisionNo, iStageNo;
	CString sVisionCamType_Comm;
	BOOL bVisionFindCheck = FALSE;
	for (iStageNo = 0; iStageNo < STAGE_NUMBER_MAX; iStageNo++)
	{
		for (iPcVisionNo = 0; iPcVisionNo <= VISION_NUMBER_MAX; iPcVisionNo++)
		{
			if (iStageNo == STAGE_NUMBER_MAX - 1 && iPcVisionNo == VISION_NUMBER_MAX)
				return;

			if (iPcVisionNo == VISION_NUMBER_MAX)
				continue;

			sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];
			if (sVisionCamType_Comm == sVisionType)
			{
				bVisionFindCheck = TRUE;
				break;
			}
		}

		if (bVisionFindCheck)
			break;
	}

	CString sVision;
	sVision.Format("%s", THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo]);

	if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
	{
		iPcVisionNo = iPcVisionNo + (2 * ((iJigNo - 1) % 2));
		sVision.Format("%s_Jig%d", sVision, ((iJigNo - 1) % 2) + 1);
	}

	auto log_time_start = std::chrono::high_resolution_clock::now();

	EnqueueStatusWrite([=]()
	{
		m_csLotStatusWrite[iPcVisionNo].Lock();

		CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus_" + sVision + ".txt";
		CIniFileCS INI(strStatusFileName);
		INI.Set_String("LAST SCAN COMPLETE", "LotID", sLotID);
		INI.Set_Integer("LAST SCAN COMPLETE", "Stage", iStageNo);
		INI.Set_Integer("LAST SCAN COMPLETE", "Magazine", iMzNo);
		INI.Set_Integer("LAST SCAN COMPLETE", "Tray", iTrayNo);
		INI.Set_Integer("LAST SCAN COMPLETE", "Module", iModuleNo);

		m_iScanCompleteCheckIndex[iPcVisionNo]++;
		if (m_iScanCompleteCheckIndex[iPcVisionNo] >= THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber)
			m_iScanCompleteCheckIndex[iPcVisionNo] = 0; // n개 이상이면 초기화

		CString sKey;
		sKey.Format("LotID%d", m_iScanCompleteCheckIndex[iPcVisionNo]);
		INI.Set_String("RECENTLY SCAN COMPLETE", sKey, sLotID);
		sKey.Format("Stage%d", m_iScanCompleteCheckIndex[iPcVisionNo]);
		INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, iStageNo);
		sKey.Format("Magazine%d", m_iScanCompleteCheckIndex[iPcVisionNo]);
		INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, iMzNo);
		sKey.Format("Tray%d", m_iScanCompleteCheckIndex[iPcVisionNo]);
		INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, iTrayNo);
		sKey.Format("Module%d", m_iScanCompleteCheckIndex[iPcVisionNo]);
		INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, iModuleNo);

		m_csLotStatusWrite[iPcVisionNo].Unlock();
	});

	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("%s/ Module recently status logging(Scan complete), Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d", sVision, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo);
	THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));
}

void CHandlerService::Save_InspectComplete(CString sLotID, int iMzNo, int iJigNo, int iTrayNo, int iModuleNo, CString sVisionType, CString sModuleResult, CString sDefectCode)
{
	CString strLog;
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	int iPcVisionNo, iStageNo;
	CString sVisionCamType_Comm;
	BOOL bVisionFindCheck = FALSE;
	for (iStageNo = 0; iStageNo < STAGE_NUMBER_MAX; iStageNo++)
	{
		for (iPcVisionNo = 0; iPcVisionNo <= VISION_NUMBER_MAX; iPcVisionNo++)
		{
			if (iStageNo == STAGE_NUMBER_MAX - 1 && iPcVisionNo == VISION_NUMBER_MAX)
				return;

			if (iPcVisionNo == VISION_NUMBER_MAX)
				continue;

			sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iPcVisionNo][iStageNo];
			if (sVisionCamType_Comm == sVisionType)
			{
				bVisionFindCheck = TRUE;
				break;
			}
		}

		if (bVisionFindCheck)
			break;
	}

	CString sVision;
	sVision.Format("%s", THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo]);

	if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
	{
		iPcVisionNo = iPcVisionNo + (2 * ((iJigNo - 1) % 2));
		sVision.Format("%s_Jig%d", sVision, ((iJigNo - 1) % 2) + 1);
	}

	auto log_time_start = std::chrono::high_resolution_clock::now();

	EnqueueStatusWrite([=]()
	{
		m_csLotStatusWrite[iPcVisionNo].Lock();

		CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus_" + sVision + ".txt";
		CIniFileCS INI(strStatusFileName);
		INI.Set_String("LAST INSPECT COMPLETE", "LotID", sLotID);
		INI.Set_Integer("LAST INSPECT COMPLETE", "Stage", iStageNo);
		INI.Set_Integer("LAST INSPECT COMPLETE", "Magazine", iMzNo);
		INI.Set_Integer("LAST INSPECT COMPLETE", "Tray", iTrayNo);
		INI.Set_Integer("LAST INSPECT COMPLETE", "Module", iModuleNo);

		m_iInspectCompleteCheckIndex[iPcVisionNo]++;
		if (m_iInspectCompleteCheckIndex[iPcVisionNo] >= THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber)
			m_iInspectCompleteCheckIndex[iPcVisionNo] = 0; // n개 이상이면 초기화

		CString sKey;
		sKey.Format("LotID%d", m_iInspectCompleteCheckIndex[iPcVisionNo]);
		INI.Set_String("RECENTLY INSPECT COMPLETE", sKey, sLotID);
		sKey.Format("Stage%d", m_iInspectCompleteCheckIndex[iPcVisionNo]);
		INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, iStageNo);
		sKey.Format("Magazine%d", m_iInspectCompleteCheckIndex[iPcVisionNo]);
		INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, iMzNo);
		sKey.Format("Tray%d", m_iInspectCompleteCheckIndex[iPcVisionNo]);
		INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, iTrayNo);
		sKey.Format("Module%d", m_iInspectCompleteCheckIndex[iPcVisionNo]);
		INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, iModuleNo);

		m_csLotStatusWrite[iPcVisionNo].Unlock();
	});

	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("%s/ Module recently status logging(Inspect complete), Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d", sVision, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo);
	THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));
}

void CHandlerService::Restart_Inspector(BOOL bAutoRerunCheck)
{
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	int iJigNumMax = 1;
	if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
		iJigNumMax = 2;

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		for (int j = 0; j < iJigNumMax; j++)
		{
			CString sVision;
			sVision.Format("%s", THEAPP.m_ModelDefineInfo.m_strVisionName_Short[i]);

			if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
				sVision.Format("%s_Jig%d", sVision, j + 1);

			CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus_" + sVision + ".txt";
			CIniFileCS INI(strStatusFileName);

			CString strLastScanLotID;
			int iLastScanStage, iLastScanMagazine, iLastScanTray, iLastScanModule;
			strLastScanLotID = INI.Get_String("LAST SCAN COMPLETE", "LotID", "");
			iLastScanStage = INI.Get_Integer("LAST SCAN COMPLETE", "Stage", -1);
			iLastScanMagazine = INI.Get_Integer("LAST SCAN COMPLETE", "Magazine", -1);
			iLastScanTray = INI.Get_Integer("LAST SCAN COMPLETE", "Tray", -1);
			iLastScanModule = INI.Get_Integer("LAST SCAN COMPLETE", "Module", -1);

			if (strLastScanLotID == "" || iLastScanStage < 0 || iLastScanMagazine < 0 || iLastScanTray < 0 || iLastScanModule < 0)
				continue;

			CString strRecentlyScanLotID;
			int iRecentlyScanStage, iRecentlyScanMagazine, iRecentlyScanTray, iRecentlyScanModule;
			CString strRecentlyInspectLotID;
			int iRecentlyInspectStage, iRecentlyInspectMagazine, iRecentlyInspectTray, iRecentlyInspectModule;
			CString sKey;

			for (int k = 0; k < THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber; k++)
			{
				sKey.Format("LotID%d", k);
				strRecentlyScanLotID = INI.Get_String("RECENTLY SCAN COMPLETE", sKey, "");
				sKey.Format("Stage%d", k);
				iRecentlyScanStage = INI.Get_Integer("RECENTLY SCAN COMPLETE", sKey, -1);
				sKey.Format("Magazine%d", k);
				iRecentlyScanMagazine = INI.Get_Integer("RECENTLY SCAN COMPLETE", sKey, -1);
				sKey.Format("Tray%d", k);
				iRecentlyScanTray = INI.Get_Integer("RECENTLY SCAN COMPLETE", sKey, -1);
				sKey.Format("Module%d", k);
				iRecentlyScanModule = INI.Get_Integer("RECENTLY SCAN COMPLETE", sKey, -1);

				if (strRecentlyScanLotID == "" || iRecentlyScanStage < 0 || iRecentlyScanMagazine < 0 || iRecentlyScanTray < 0 || iRecentlyScanModule < 0)
					continue;

				BOOL bFound = FALSE;
				for (int m = 0; m < THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber; m++)
				{
					sKey.Format("LotID%d", m);
					strRecentlyInspectLotID = INI.Get_String("RECENTLY INSPECT COMPLETE", sKey, "");
					sKey.Format("Stage%d", m);
					iRecentlyInspectStage = INI.Get_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);
					sKey.Format("Magazine%d", m);
					iRecentlyInspectMagazine = INI.Get_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);
					sKey.Format("Tray%d", m);
					iRecentlyInspectTray = INI.Get_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);
					sKey.Format("Module%d", m);
					iRecentlyInspectModule = INI.Get_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);

					if (strRecentlyScanLotID == strRecentlyInspectLotID && iRecentlyScanStage == iRecentlyInspectStage && iRecentlyScanMagazine == iRecentlyInspectMagazine && iRecentlyScanTray == iRecentlyInspectTray && iRecentlyScanModule == iRecentlyInspectModule)
					{
						bFound = TRUE;
						break;
					}
				}

				if (!bFound)
					THEAPP.m_pHandlerService->Set_InspectComplete(strRecentlyScanLotID, iRecentlyScanMagazine, iRecentlyScanTray, iRecentlyScanModule, THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[i][iRecentlyScanStage], "N", "RT");
			}
		}
	}
}

void CHandlerService::Get_ConnectRequest()
{
	Set_ConnectReply();
}

void CHandlerService::Set_ConnectReply()
{
	CString	strSendMsg;
	strSendMsg.Format("@CONNECT,REPLY\n");
	Send_Command(strSendMsg);
}

void CHandlerService::Get_RunComplete()
{
	CString strLog;
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	THEAPP.m_pInspectService->SetRunStartStatus(FALSE);

	for (int i = 0; i < MODEL_DATA_MAX; i++)
		THEAPP.g_iDualModelDataRunCheck[i] = 0;

	for (int i = 0; i < MAX_MAGAZINE_NO; i++)
		THEAPP.g_iGrabFailCount[i] = 0;

	CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS INI(strStatusFileName);
	CString strSection = "Status";
	INI.Set_Bool(strSection, "RunStart", FALSE);

	THEAPP.m_pInspectService->SetCycleStopStatus(TRUE);
	THEAPP.m_pInspectService->SetReloadStatus(TRUE);

	if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
	{
		auto log_time_start = std::chrono::high_resolution_clock::now();

		EnqueueStatusWrite([=]()
		{
			for (int i = 0; i < VISION_NUMBER_MAX; i++)
			{
				m_csLotStatusWrite[i].Lock();
				CString sVisionCamType_Short, sVision;
				sVisionCamType_Short.Format("%s", THEAPP.m_ModelDefineInfo.m_strVisionName_Short[i]);

				if (sVisionCamType_Short == "UD")
					continue;

			for (int j = 0; j < 2; j++)
			{
				if (strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS")
					sVision.Format("%s_Jig%d", sVisionCamType_Short, j + 1);
				else
				{
					if (j == 1)
						break;
				}

					CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus_" + sVision + ".txt";
					CIniFileCS INI(strStatusFileName);

					INI.Set_String("LAST SCAN COMPLETE", "LotID", "");
					INI.Set_Integer("LAST SCAN COMPLETE", "Stage", -1);
					INI.Set_Integer("LAST SCAN COMPLETE", "Magazine", -1);
					INI.Set_Integer("LAST SCAN COMPLETE", "Tray", -1);
					INI.Set_Integer("LAST SCAN COMPLETE", "Module", -1);

					INI.Set_String("LAST INSPECT COMPLETE", "LotID", "");
					INI.Set_Integer("LAST INSPECT COMPLETE", "Stage", -1);
					INI.Set_Integer("LAST INSPECT COMPLETE", "Magazine", -1);
					INI.Set_Integer("LAST INSPECT COMPLETE", "Tray", -1);
					INI.Set_Integer("LAST INSPECT COMPLETE", "Module", -1);

					for (int k = 0; k < THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber; k++)
					{
						CString sKey;
						sKey.Format("LotID%d", k);
						INI.Set_String("RECENTLY SCAN COMPLETE", sKey, "");
						sKey.Format("Stage%d", k);
						INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, -1);
						sKey.Format("Magazine%d", k);
						INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, -1);
						sKey.Format("Tray%d", k);
						INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, -1);
						sKey.Format("Module%d", k);
						INI.Set_Integer("RECENTLY SCAN COMPLETE", sKey, -1);
					}

					for (int k = 0; k < THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber; k++)
					{
						CString sKey;
						sKey.Format("LotID%d", k);
						INI.Set_String("RECENTLY INSPECT COMPLETE", sKey, "");
						sKey.Format("Stage%d", k);
						INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);
						sKey.Format("Magazine%d", k);
						INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);
						sKey.Format("Tray%d", k);
						INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);
						sKey.Format("Module%d", k);
						INI.Set_Integer("RECENTLY INSPECT COMPLETE", sKey, -1);
					}
				}

				m_csLotStatusWrite[i].Unlock();
			}
		});

		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("ALL/ Module recently status logging(Run complete), Time(ms): %d", log_time_ms);
		THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));
	}

#ifdef LGITAI
	if (THEAPP.m_lg_inspectorManager) {
		THEAPP.m_lg_inspectorManager->try_model_reload();
	}
#endif
}

void CHandlerService::EnqueueStatusWrite(std::function<void()> task)
{
	CleanupStatusWriteFutures();

	std::lock_guard<std::mutex> lock(m_statusWriteFuturesMutex);
	m_statusWriteFutures.push_back(std::async(std::launch::async, task));
}

void CHandlerService::CleanupStatusWriteFutures()
{
	std::lock_guard<std::mutex> lock(m_statusWriteFuturesMutex);
	m_statusWriteFutures.erase(
		std::remove_if(m_statusWriteFutures.begin(), m_statusWriteFutures.end(),
			[](std::future<void>& f) {
				if (f.valid() && f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
					f.get();
					return true;
				}
				return false;
			}),
		m_statusWriteFutures.end()
	);
}