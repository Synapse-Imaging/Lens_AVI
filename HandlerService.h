// HandlerService.h : header file
//
#pragma once

#include "CSClientSocket.h"
#ifdef _DEBUG
	#pragma comment(lib, "CSClientSocketD.lib")
#else
	#pragma comment(lib, "CSClientSocketR.lib")
#endif

#include "ClientSocket.h"

/////////////////////////////////////////////////////////////////////////////
// CHandlerService window

class CHandlerService : public CWnd
{
	DECLARE_DYNAMIC(CHandlerService)

private:
	static CHandlerService *m_pInstance;
	CRect m_ScreenRect;
	
	CRect GetPosition() { return m_ScreenRect; }
	void SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

public:
	static CHandlerService *GetInstance();
	void DeleteInstance();

public:
	CHandlerService();
	virtual ~CHandlerService();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnClientConnect(WPARAM wLocalPort, LPARAM lConnect);
	afx_msg LRESULT OnClientReceive(WPARAM wLocalPort, LPARAM lParam);
	afx_msg LRESULT OnClientClose(WPARAM wLocalPort, LPARAM lParam);
	afx_msg LRESULT OnTcpReceive(WPARAM wLocalPort, LPARAM lParam);

public:
	BOOL Connect_NetworkDrive(CString strDirve, CString strPath);

	void SetHandlerTCP_IP(CString sIP) { m_sHandler_TCP_IP = sIP; }
	void SetHandlerTCP_PORT(int iPort) { m_iHandler_TCP_Port = iPort; }
	void SetHandlerTCP_Connect(BOOL bConnect) { m_bHandler_TCP_Connect = bConnect; }

	void SetTCP_IP(int iLightControllerIndex, CString sIP) { m_sTCP_IP[iLightControllerIndex] = sIP; }
	void SetTCP_PORT(int iLightControllerIndex, int iPort) { m_iTCP_Port[iLightControllerIndex] = iPort; }

	CCriticalSection	m_csSendCmd;
	CCriticalSection	m_csSendCmdTcp[LIGHT_CONTROLLER_NUMBER_MAX];

	void Get_LotStart(CString sLotID, CString sMzNo, CString sTrayAmt, CString sModuleAmt, CString sHandlerModelName, CString sModelAutoLoad);

	void Get_LoadComplete(CString sVisionType, CString sJigNo, CString sLotID, CString sMzNo, CString sTrayNo, CString sModuleNo, CString sHeight, CString sBarcode);

	void Send_Tcp(int iLightControllerIndex, CString strSend, BOOL bSaveLog = TRUE);

	void Get_ModuleData_Splite(CString sREVData);

	void Get_TrayLoaded(CString sLotID, CString sMzNo, CString sTrayNo);

private:
	CString m_sHandler_TCP_IP;
	int m_iHandler_TCP_Port;
	BOOL m_bHandler_TCP_Connect;

	CString m_sTCP_IP[LIGHT_CONTROLLER_NUMBER_MAX];
	int m_iTCP_Port[LIGHT_CONTROLLER_NUMBER_MAX];
	CString m_strTcpBuffer[LIGHT_CONTROLLER_NUMBER_MAX];

	CClientSocketCS m_TCPHandler;
	int		m_nLocalPort;

	BOOL	m_bConnected;		// 제어PC와 연결상태
	CString m_strRecvCmd;		// kcs 추가

	double m_dCurrentXpos;
	double m_dCurrentYpos;

	int m_iHandlerStatus;
	int m_iEquipmentMode;
	int m_iModelCheck;
	double m_dXposition,m_dYposition;
	
	int m_iHandlerProgramMode;
	int m_iShowHideFlag;

	void Get_StatusRequest();	//상태정보를 요청한다.
	void Get_StatusReply(CString sStatus);	//Status(0:No Ready, 1:Ready, 2:Run, 3:Reset, 4~6:Stop, 7:Estop, 8:Warning, 9:FMError)
	void Get_StatusUpdate(CString sStatus);	//Status(0:No Ready, 1:Ready, 2:Run, 3:Reset, 4~6:Stop, 7:Estop, 8:Warning, 9:FMError)

	void Get_ModeRequest();
	void Get_ModeReply(int iMode);

	void Get_ModelReply(int iFlag);

	void Get_LotEnd(CString sLotID, CString sMzNo);

	void  Get_BarcodeUpdate(CString sLotID, CString sLotNo, CString sTrayNo, CString sModuleNo, CString sBarcodeID);

	void  Get_HistoryUpdate(CString sMzNo, CString sTrayNo, CString sModuleNo, CString sBarcodeID,
		CString sLoadPicker, CString sLoadPickerIndex, CString sVisionStageNo, CString sVisionStageIndex, CString sUnloadPickerNo,
		CString sUnloadPickerIndex, CString sJudge, CString sGoodTrayNo, CString sGoodTrayPorket, CString sNGTrayNo, CString sNGTrayPorket);
		
	void  Get_ModuleData(CString sLotID, CString sNo, CString sBarcodeID, CString sSite, CString sEqpID, CString sEqpName, CString sPara, CString sToolCavity);

	void  Get_PositionReply(CString sVisionType, int iJigNo, double dCameraZ, double dLightZ, double dHeadX, double dStageY, double dTilt, double dRotate);

	void Get_MoveComplete(CString sVisionType, int iJigNo);

	void Get_ShiftComplete(CString sVisionType);

	void Get_SmoveComplete(CString sVisionType);

	void Get_TurnComplete(CString sVisionType);

	void Get_ReloadComplete(CString sPCType);

	void Get_CycleStop();

	void Get_NetworkConnect(); //Ver 1026 add

	void Send_Command(CString strSend, BOOL bSaveLog=TRUE);

	////////////////////////////////////////////////////////////////////////// New Controller
	CClientSocket *mp_TcpHandler[LIGHT_CONTROLLER_NUMBER_MAX];
	BOOL m_bTcpConnect[LIGHT_CONTROLLER_NUMBER_MAX];
	//////////////////////////////////////////////////////////////////////////

	int m_iScanCompleteCheckIndex[VISION_NUMBER_MAX];
	int m_iInspectCompleteCheckIndex[VISION_NUMBER_MAX];

	std::vector<std::future<void>> m_statusWriteFutures;
	std::mutex m_statusWriteFuturesMutex;
public:
	BOOL Initialize_Handler();
	BOOL Initialize_TcpHandler(BOOL bRetryCheck);

	void Terminate_Handler();
	void Terminate_TcpHandler();
	BOOL m_bMotionMoveComplete[VISION_NUMBER_MAX];

	BOOL m_bShiftMoveComplete[VISION_NUMBER_MAX];
	BOOL m_bTurnMoveComplete;

	BOOL Is_Connected() { return m_bConnected; }

	int GetHandlerStatus() { return m_iHandlerStatus; }

	void Set_StatusRequest();
	void Set_StatusReply();
	void Set_StatusUpdate(int nStatus);

	void Set_ModeReplay(int iMode);//Type(0:None, 1:Inspect, 2:Teach, 3:Demo, 4:Test, 5:Debug, 6:Step, 7:Home, 8:Manual)
	void Set_ModeUpdate(int iMode);//Type(0:None, 1:Inspect, 2:Teach, 3:Demo, 4:Test, 5:Debug, 6:Step, 7:Home, 8:Manual)

	void Set_ModelCreate(CString sModelname);
	void Set_ModelUpdate(CString sModelname);

	void Set_LotReply();

	void Set_LoadReply();

	void Set_LotReady(CString sLotID, int iMzNo); // Lot Ready 후 제어에 보낸다.

	void Set_ScanComplete(CString sLotID, int iMzNo, int iJigNo, int iTrayNo, int iModuleNo, CString sVisionType);			 //검사영역을 모두 Scan 후 Complete 신호를 보낸다. (NewType)

	void Set_InspectComplete(CString sLotID, int iMzNo, int iTrayNo, int iModuleNo, CString sVisionType, CString sModuleResult, CString sDefectCode);		//Inspection 완료 후 제어에 보낸다.
	
	void Set_AlignReply(CString sLotID);
	void Set_LoadingAlignComplete(CString sLotID, int iMzNo, int iTrayNo, CString sTrayStatusResult, CString sModuleAlignResult, double dAvgDeltaXmm, double dAvgDeltaYmm);
	void Set_ShipTrayAlignComplete(CString sLotID, int iShipTrayType, int iTrayNo, int iSeqNo, double dDeltaXmm, double dDeltaYmm);

	void Set_BarcodeResult(CString sLotID, int iMzNo, int iTrayNo, int iModuleNo, CString sBarcodeID);
	void Set_AngleResult(int iTrayNo, int iModuleNo, double dBenvolioAngle, CString sVisionType, double dBenvolioDeltaX, double dBenvolioDeltaY); // T1 완료 후 제어에 보낸다.
	void Set_HomerResult(int iTrayNo, int iModuleNo, double dHomerAngle, CString sVisionType, double dHomerDeltaX, double dHomerDeltaY); // T1 완료 후 제어에 보낸다.

	void Set_PositionRequest(int iVisionType, int iStageNo, int iJigNo);//현재 모션 Z위치를 요구한다.
	//////////////////////////////////////////////////////////////////////////  L10
	void Set_AMoveRequest(int iVisionType, int iStageNo, int iJigNo, double dCameraZ, double dLightZ, double dHeadX, double dStageY, double dStageT, double dStageR);

	void Set_ShiftRequest(int iVisionType, CString sLotID, int iMzNo);	// Y Shift 명령을 보낸다. (Bottom1, Top2)

	void Set_SMoveRequest(double dZposition, int iVisionType, CString sLotID, int iMzNo);	// Shift + 절대이동 구동 명령을 보낸다. (""이면 움직이지 않는다)

	void Set_ReloadRequest(int iPCType);	// 검사 초기 상태를 요청한다. (PC1, PC2)

	void Set_ErrorRequest(int iErrorNo, CString strErrorMsg = "");

	void Set_AlarmRequest(CString sVisionType, CString sLotID, int iMzNo, int iAlarmNo, CString sDefectName, int iDefectCount);

	void Set_TimeUpdate();

	void Get_TimeUpdate(CString sTime);

	void Set_NetworkConnect(int iConnectStatus);

	void Set_TrayComplete(CString sLotID, int iMzNo, int iTrayNo, CString sTrayResult);

	CCriticalSection	m_csLotStatusWrite[VISION_NUMBER_MAX];
	void Save_ScanComplete(CString sLotID, int iMzNo, int iJigNo, int iTrayNo, int iModuleNo, CString sVisionType);
	void Save_InspectComplete(CString sLotID, int iMzNo, int iJigNo, int iTrayNo, int iModuleNo, CString sVisionType, CString sModuleResult, CString sDefectCode);
	void Restart_Inspector(BOOL bAutoRerunCheck);

	void Get_ConnectRequest();
	void Set_ConnectReply();

	void Get_RunComplete();

	void EnqueueStatusWrite(std::function<void()> task);
	void CleanupStatusWriteFutures();

	CString strMessageBox;
};

/////////////////////////////////////////////////////////////////////////////
