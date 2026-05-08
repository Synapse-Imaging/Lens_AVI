// ADJClientService.cpp : 구현 파일입니다.
//


#include "stdafx.h"
#include "uScan.h"
#ifdef USE_SUAKIT
#include "ADJClientService.h"



static UINT TCPPostProcessing_Client(LPVOID lParam);
// CADJClientService

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Rpcrt4.lib") // RPC 라이브러리 링크 (UUID용)

#define DEFAULT_PORT "27015"
#define SERVER_IP "127.0.0.1" // 서버 IP 주소

// CADJClientService

IMPLEMENT_DYNAMIC(CADJClientService, CWnd)

// 251224 KDH LocalADJ START
struct Request {
	std::string request_id; // UUID 문자열로 변경
	int tray_no;
	int module_no;
	std::string lot_id;
	std::string barcode;
	std::string filename;
	std::string defect_name;
	std::vector<unsigned char> image_data;
};

struct Response {
	std::string request_id; // UUID 문자열로 변경
	char result_char;
	float result_float;
};

// Request ID 생성기
std::string generate_request_id() {
	UUID uuid;
	RPC_STATUS status = UuidCreate(&uuid);
	if (status != RPC_S_OK && status != RPC_S_UUID_LOCAL_ONLY && status != RPC_S_UUID_NO_ADDRESS) {
		// UuidCreate 실패 시 대체 ID 생성
		return "uuid-creation-error";
	}
	RPC_CSTR pszUuid = NULL;
	status = UuidToStringA(&uuid, &pszUuid);
	if (status != RPC_S_OK) {
		if (pszUuid != NULL) RpcStringFreeA(&pszUuid);
		return "uuid-string-conversion-error";
	}
	std::string uuid_str(reinterpret_cast<char*>(pszUuid));
	RpcStringFreeA(&pszUuid); // 메모리 해제 필수!
	return uuid_str;
}

bool send_request(SOCKET ConnectSocket, const Request& req) {
	// Request ID 전송 (int)
	if (req.request_id.length() != 36) {
		// 실제로는 UUID 문자열이 항상 36자여야 함.
		// 이 부분은 generate_request_id() 함수가 올바른 형식의 UUID를 반환한다고 가정.
		// 만약 길이가 다르면 오류 처리 또는 서버와의 약속에 따라 패딩/자르기 필요.
		cerr << "Warning: Request ID length is not 36: " << req.request_id.length() << std::endl;
		// 여기서 전송을 중단하거나, 서버가 가변 길이를 처리하도록 프로토콜 수정 필요.
		// 현재 서버는 고정 길이 36을 기대하도록 수정되었음.
		return false;
	}
	send(ConnectSocket, req.request_id.c_str(), 36, 0);

	// tray 크기와 tray 전송
	int tray_size = sizeof(req.tray_no);
	send(ConnectSocket, (char*)&tray_size, sizeof(tray_size), 0);
	send(ConnectSocket, (char*)&req.tray_no, tray_size, 0);

	// module 크기와 module 전송
	int module_size = sizeof(req.module_no);
	send(ConnectSocket, (char*)&module_size, sizeof(module_size), 0);
	send(ConnectSocket, (char*)&req.module_no, module_size, 0);

	// lotid 크기와 lotid 전송
	int lot_id_size = req.lot_id.size();
	send(ConnectSocket, (char*)&lot_id_size, sizeof(lot_id_size), 0);
	send(ConnectSocket, req.lot_id.c_str(), lot_id_size, 0);

	// barcode 크기와 barcode 전송
	int barcode_size = req.barcode.size();
	send(ConnectSocket, (char*)&barcode_size, sizeof(barcode_size), 0);
	send(ConnectSocket, req.barcode.c_str(), barcode_size, 0);

	// filename 크기와 filename 전송
	int filename_size = req.filename.size();
	send(ConnectSocket, (char*)&filename_size, sizeof(filename_size), 0);
	send(ConnectSocket, req.filename.c_str(), filename_size, 0);

	// 불량명 크기와 불량명 전송
	int defect_name_size = req.defect_name.size();
	send(ConnectSocket, (char*)&defect_name_size, sizeof(defect_name_size), 0);
	send(ConnectSocket, req.defect_name.c_str(), defect_name_size, 0);

	// 이미지 크기와 이미지 데이터 전송
	int img_size = req.image_data.size();
	send(ConnectSocket, (char*)&img_size, sizeof(img_size), 0);
	send(ConnectSocket, (char*)req.image_data.data(), img_size, 0);

	return true;
}

Response receive_response(SOCKET ConnectSocket) {
	Response resp = { "error", 'E', 0.0f }; // 기본 오류 값

	int bytes_received;

	// 응답 프로토콜: ID길이(int, 4바이트) + ID문자열(가변) + 결과문자(char, 1바이트) + 결과실수(float, 4바이트)

	// 1. Request ID 문자열의 길이 수신 (int, 4바이트, little-endian 가정)
	int request_id_len = 0;
	if (recv(ConnectSocket, reinterpret_cast<char*>(&request_id_len), sizeof(request_id_len), MSG_WAITALL) != sizeof(request_id_len)) {
		resp.request_id = "recv-id-len-error"; return resp;
	}
	if (request_id_len <= 0 || request_id_len > 100) {
		resp.request_id = "invalid-id-len-error"; return resp;
	}

	// 2. Request ID 문자열 수신
	std::vector<char> id_buffer(request_id_len);
	if (recv(ConnectSocket, id_buffer.data(), request_id_len, MSG_WAITALL) != request_id_len) {
		resp.request_id = "recv-id-data-error"; return resp;
	}
	resp.request_id.assign(id_buffer.begin(), id_buffer.end());

	// 3. 결과 (char + float, 총 5바이트) 수신용 버퍼
	char result_buffer[5]; // 버퍼 크기를 5로 수정!
	if (recv(ConnectSocket, result_buffer, 5, MSG_WAITALL) != 5) {
		resp.result_char = 'E'; resp.result_float = 0.0f; return resp;
	}

	// 버퍼에서 데이터 복사
	resp.result_char = result_buffer[0];
	memcpy(&resp.result_float, result_buffer + 1, sizeof(float));

	return resp;
}
// 250219 KDH LocalADJ END
CADJClientService::CADJClientService()
{
	for (int i = 0; i < MAX_CLIENT_NUM; i++)
	{
		for (int j = 0; j < MAX_MULTI_CONNECTION_NUM; j++)
		{
			m_bConnect[i][j] = FALSE;
			m_bRealConnect[i][j] = FALSE;
			m_bTCPThreadRun[i][j] = FALSE;
		}
	}

	m_strClientIP = "";
	m_nTimeOutCnt = 0;
	m_nClientNO = 0;
	m_iModuleNo_ADJCnt = 0;

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		m_nADJDefectCnt[i] = 0;
	}

	InitializeCriticalSection(&m_csADJChangeStatus);
	InitializeCriticalSection(&m_csFAIADJChangeStatus);
	ResetAdjStatus();
	ResetFAIAdjStatus();
}

CADJClientService::~CADJClientService()
{
}

BEGIN_MESSAGE_MAP(CADJClientService, CWnd)
END_MESSAGE_MAP()

// CADJClientService 메시지 처리기입니다.

void CADJClientService::Initialize()
{
	CString strADJ_IP;
	int     nADJ_Port;
	int		i_ADJMaxConnect = THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO;
	m_strClientIP = THEAPP.Struct_PreferenceStruct.m_strCurrentADJ_IP;

	for (int i = 0; i < MAX_CLIENT_NUM; i++)
	{
		for (int j = 0; j < i_ADJMaxConnect; j++)
		{
			if (m_arrClient[i][j].GetLogPath() == "")
			{
				m_arrClient[i][j].SetLogPath("./Data/TCPLog");
				m_arrClient[i][j].CreateDirForLog();
				m_arrClient[i][j].SetTotalByteTokenPos(10); //20200912
			}

			if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[i])
			{
				strADJ_IP = THEAPP.Struct_PreferenceStruct.m_strADJ_IP[i];
				nADJ_Port = THEAPP.Struct_PreferenceStruct.m_iADJ_Port[i] + (j * 10);

				if (!m_arrClient[i][j].TCPClientInit(strADJ_IP, nADJ_Port, m_strClientIP))
				{
					strLog.Format("TCP initialize error, IP: %s, Port: %d", strADJ_IP, nADJ_Port);
					THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
				}

				if (m_bTCPThreadRun[i][j] == FALSE)
				{
					m_nClientNO = ((i * MAX_MULTI_CONNECTION_NUM) + j);
					m_bTCPThreadRun[i][j] = TRUE;
					AfxBeginThread(TCPPostProcessing_Client, this);
					Sleep(5);
				}
			}
		}
	}
}
void CADJClientService::ReStart(void)
{
	for (int i = 0; i < MAX_CLIENT_NUM; i++)
	{
		for (int j = 0; j < MAX_MULTI_CONNECTION_NUM; j++)
		{
			HANDLE* hEvent = m_arrClient[i][j].GetTCPReceiveHanlde();
			SetEvent(hEvent[RESTART_EVENT]);
			m_arrClient[i][j].ResetTCPReceiveHandle(SERVER_CLOSE_EVENT);	//ASSERT : To reset manual event
		}
	}
}

int CADJClientService::SendImagetoADJ(stTCPPacket stPacket, int nClientNO, int nThreadNO, int nWaitMilsec)
{
	BYTE* pbytePacket = nullptr;
	pbytePacket = new BYTE[MAX_BUF_SIZE];
	//패킷 만들어 보냄.
	int nDataSize = stPacket.MakeImagePacket(pbytePacket);

	//Sleep(3);
	m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].TCPSend(pbytePacket, nDataSize);

	int nDeepLearningModelID = atoi((const char*)stPacket.byteDeepLearningModelID) - 1 + ((nClientNO / MAX_MULTI_CONNECTION_NUM) * DEEP_MODEL_NUM);	//주의 : 수아 모델 ID 는 1~16 모듈넘버 1~42, 트레이넘버 1부터 시작
	int nModuleNo = atoi((const char*)stPacket.byteModuleNo) - 1;
	int nDefectCnt = atoi((const char*)stPacket.byteDefectCnt);			//Defect Cnt 는 0부터 시작
	int nMagazineNo = atoi((const char*)stPacket.byteMagazineNo) - 1;
	int nTrayNo = atoi((const char*)stPacket.byteTrayNo) - 1;

	int dwBytes;

	strLog.Format("Send data thread, Client: %d, Model: %d, Tray: %d, Module: %d, Defect count: %d",
		nClientNO,
		nDeepLearningModelID + 1,
		nTrayNo + 1,
		nModuleNo + 1,
		nDefectCnt + 1);
	THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

	if (stPacket.pByteImageData != NULL)
		free(stPacket.pByteImageData);

	int nModuleTimeOut = 0;

	if (pbytePacket != nullptr)
	{
		delete pbytePacket;
		pbytePacket = nullptr;
	}
	if (THEAPP.Struct_PreferenceStruct.m_bUseADJTimeOut)
	{
		nModuleTimeOut = THEAPP.Struct_PreferenceStruct.m_nADJTimeOut;
	}
	else
	{
		nModuleTimeOut = ADJ_SEND_TIMEOUT;
	}

	const int N_INTERVAL = 5;
	int nMaxRepeatCnt = (int)(nModuleTimeOut / N_INTERVAL);
	int nCnt = 0;

	if (THEAPP.Struct_PreferenceStruct.m_bIsSendADJ)
	{
		while (TRUE)
		{
			Sleep(N_INTERVAL);
			nCnt++;

			if (THEAPP.m_TCPClientService.GetAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt) >= 0)
			{
				int ADJ_Reuslt = THEAPP.m_TCPClientService.GetAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt);
				THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt, RCV_WAIT);
				return ADJ_Reuslt;
			}
			if (GetTickCount() - nWaitMilsec > nModuleTimeOut || nCnt >= nMaxRepeatCnt) //nMaxRepeatCnt 관련 추가 (혹시모를 경우 대비해서 20200919)
			{
				THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt, RCV_TIMEOUT);
				return RCV_TIMEOUT;
			}
		}
	}
	else
		return RCV_TIMEOUT;
}


cv::Mat CADJClientService::SendSegmentImagetoADJ(stTCPPacket stPacket, int nClientNO, int nThreadNO, int nWaitMilsec)
{
	BYTE* pbytePacket = nullptr;
	pbytePacket = new BYTE[MAX_BUF_SIZE];
	//패킷 만들어 보냄.

	int nDataSize = stPacket.MakeImagePacket(pbytePacket);

	//Sleep(3);
	m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].TCPSend(pbytePacket, nDataSize);

	int nDeepLearningModelID = atoi((const char*)stPacket.byteDeepLearningModelID) - 1 + ((nClientNO / MAX_MULTI_CONNECTION_NUM) * DEEP_MODEL_NUM);	//주의 : 수아 모델 ID 는 1~16 모듈넘버 1~42, 트레이넘버 1부터 시작
	int nModuleNo = atoi((const char*)stPacket.byteModuleNo) - 1;
	int nDefectCnt = atoi((const char*)stPacket.byteDefectCnt);			//Defect Cnt 는 0부터 시작
	int nMagazineNo = atoi((const char*)stPacket.byteMagazineNo) - 1;
	int nTrayNo = atoi((const char*)stPacket.byteTrayNo) - 1;

	int dwBytes;

	strLog.Format("Send data thread, Client: %d, Model: %d, Tray: %d, Module: %d, Defect count: %d",
		nClientNO,
		nDeepLearningModelID + 1,
		nTrayNo + 1,
		nModuleNo + 1,
		nDefectCnt + 1);
	THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

	//if (stPacket.pByteImageData != NULL)
	//	free(stPacket.pByteImageData);

	int nModuleTimeOut = 0;

	if (pbytePacket != nullptr)
	{
		delete pbytePacket;
		pbytePacket = nullptr;
	}

	if (THEAPP.Struct_PreferenceStruct.m_bUseADJTimeOut)
	{
		nModuleTimeOut = THEAPP.Struct_PreferenceStruct.m_nADJTimeOut;
	}
	else
	{
		nModuleTimeOut = ADJ_SEND_TIMEOUT;
	}

	const int N_INTERVAL = 5;
	int nMaxRepeatCnt = (int)(nModuleTimeOut / N_INTERVAL);
	int nCnt = 0;

	if (THEAPP.Struct_PreferenceStruct.m_bIsSendADJ)
	{
		while (TRUE)
		{
			Sleep(N_INTERVAL);
			nCnt++;

			if (THEAPP.m_TCPClientService.GetAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt) >= 0)
			{
				THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt, RCV_WAIT);
				/*if (!THEAPP.m_TCPClientService.matMaskImageData[iTrayNo - 1][iModuleNo - 1][iDefectCnt].empty()) {
					cv::Mat cv_temp;
					cv::normalize(THEAPP.m_TCPClientService.matMaskImageData[iTrayNo - 1][iModuleNo - 1][iDefectCnt], cv_temp, 0, 255, cv::NORM_MINMAX, -1);
					cv::imwrite("D:\\TEMP\\TEMP1.jpg", cv_temp);
				}*/
				return std::move(THEAPP.m_TCPClientService.matMaskImageData[nTrayNo][nModuleNo][nDefectCnt]);
			}
			if (GetTickCount() - nWaitMilsec > nModuleTimeOut || nCnt >= nMaxRepeatCnt) //nMaxRepeatCnt 관련 추가 (혹시모를 경우 대비해서 20200919)
			{
				THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt, RCV_TIMEOUT);

				return cv::Mat();
			}
		}
	}
	else
		return cv::Mat();

}


cv::Mat CADJClientService::SendSegmentImagetoADJ(int nThreadNO, int nMzNO, int nTrayNO, int nModuleNO, CString sLotID, int nDefectCount, int nDeepLearningModelID, CString sFileName, int nWaitMilsec, cv::Mat matADJImage)
{
	stTCPPacket tcpPacket;
	CString strModuleNo, strTrayNo, strDefectCnt, strDeepLearningModelID, strPort, strMagazineNo, strImageCount, strModelVersion, strImageWidth, strImageHeight;
	strModuleNo.Format("%d", nModuleNO);
	strTrayNo.Format("%d", nTrayNO);
	strDefectCnt.Format("%d", nDefectCount);
	strMagazineNo.Format("%d", nMzNO);
	strDeepLearningModelID.Format("%d", nDeepLearningModelID);
	strModelVersion.Format("%d", THEAPP.GetProgramVersion());
	strImageWidth.Format("%d", matADJImage.cols);
	strImageHeight.Format("%d", matADJImage.rows);

	int iClientNO = (nTrayNO + nModuleNO + nDefectCount) % 1;


	if (!THEAPP.m_TCPClientService.m_bConnect[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM] ||
		!THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO / MAX_MULTI_CONNECTION_NUM] ||
		!THEAPP.Struct_PreferenceStruct.m_bIsSendADJ)
	{
		return cv::Mat();
	}

	strPort.Format("%d", THEAPP.m_TCPClientService.m_arrClient[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM].GetTCPPort());

	for (int i = 0; i < sFileName.GetLength(); i++)
	{
		tcpPacket.byteFileName[i] = sFileName.GetAt(i);
		if (i + 1 == sFileName.GetLength())
			tcpPacket.byteFileName[i + 1] = NULL;
	}

	for (int i = 0; i < THEAPP.m_TCPClientService.m_strClientIP.GetLength(); i++)
	{
		tcpPacket.byteIP[i] = THEAPP.m_TCPClientService.m_strClientIP.GetAt(i);
		if (i + 1 == THEAPP.m_TCPClientService.m_strClientIP.GetLength())
			tcpPacket.byteIP[i + 1] = NULL;
	}

	for (int i = 0; i < strPort.GetLength(); i++)
	{
		tcpPacket.bytePort[i] = strPort.GetAt(i);
		if (i + 1 == strPort.GetLength())
			tcpPacket.bytePort[i + 1] = NULL;
	}

	for (int i = 0; i < sLotID.GetLength(); i++)
	{
		tcpPacket.byteLotID[i] = sLotID.GetAt(i);
		if (i + 1 == sLotID.GetLength())
			tcpPacket.byteLotID[i + 1] = NULL;
	}

	for (int i = 0; i < strModuleNo.GetLength(); i++)
	{
		tcpPacket.byteModuleNo[i] = strModuleNo.GetAt(i);
		if (i + 1 == strModuleNo.GetLength())
			tcpPacket.byteModuleNo[i + 1] = NULL;
	}

	for (int i = 0; i < strTrayNo.GetLength(); i++)
	{
		tcpPacket.byteTrayNo[i] = strTrayNo.GetAt(i);
		if (i + 1 == strTrayNo.GetLength())
			tcpPacket.byteTrayNo[i + 1] = NULL;
	}

	for (int i = 0; i < strDefectCnt.GetLength(); i++)
	{
		tcpPacket.byteDefectCnt[i] = strDefectCnt.GetAt(i);
		if (i + 1 == strDefectCnt.GetLength())
			tcpPacket.byteDefectCnt[i + 1] = NULL;
	}

	for (int i = 0; i < strDeepLearningModelID.GetLength(); i++)
	{
		tcpPacket.byteDeepLearningModelID[i] = strDeepLearningModelID.GetAt(i);
		if (i + 1 == strDeepLearningModelID.GetLength())
			tcpPacket.byteDeepLearningModelID[i + 1] = NULL;
	}

	for (int i = 0; i < strMagazineNo.GetLength(); i++)
	{
		tcpPacket.byteMagazineNo[i] = strMagazineNo.GetAt(i);
		if (i + 1 == strMagazineNo.GetLength())
			tcpPacket.byteMagazineNo[i + 1] = NULL;
	}

	for (int i = 0; i < strModelVersion.GetLength(); i++)
	{
		tcpPacket.byteModelVersion[i] = strModelVersion.GetAt(i);
		if (i + 1 == strModelVersion.GetLength())
			tcpPacket.byteModelVersion[i + 1] = NULL;
	}

	for (int i = 0; i < strImageWidth.GetLength(); i++)
	{
		tcpPacket.byteImageWidth[i] = strImageWidth.GetAt(i);
		if (i + 1 == strImageWidth.GetLength())
			tcpPacket.byteImageWidth[i + 1] = NULL;
	}

	for (int i = 0; i < strImageHeight.GetLength(); i++)
	{
		tcpPacket.byteImageHeight[i] = strImageHeight.GetAt(i);
		if (i + 1 == strImageHeight.GetLength())
			tcpPacket.byteImageHeight[i + 1] = NULL;
	}

	tcpPacket.pByteImageData = matADJImage.data;
	BYTE* pbytePacket = nullptr;
	pbytePacket = new BYTE[MAX_BUF_SIZE];
	//패킷 만들어 보냄.

	int nDataSize = tcpPacket.MakeImagePacket(pbytePacket);

	//Sleep(3);
	m_arrClient[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM].TCPSend(pbytePacket, nDataSize);

	strLog.Format("Send data thread, Client: %d, Model: %d, Tray: %d, Module: %d, Defect count: %d",
		iClientNO,
		nDeepLearningModelID,
		nTrayNO,
		nModuleNO,
		nDefectCount + 1);
	THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

	//if (stPacket.pByteImageData != NULL)
	//	free(stPacket.pByteImageData);

	int nModuleTimeOut = 0;

	if (pbytePacket != nullptr)
	{
		delete pbytePacket;
		pbytePacket = nullptr;
	}

	if (THEAPP.Struct_PreferenceStruct.m_bUseADJTimeOut)
	{
		nModuleTimeOut = THEAPP.Struct_PreferenceStruct.m_nADJTimeOut;
	}
	else
	{
		nModuleTimeOut = ADJ_SEND_TIMEOUT;
	}

	const int N_INTERVAL = 5;
	int nMaxRepeatCnt = (int)(nModuleTimeOut / N_INTERVAL);
	int nCnt = 0;

	if (THEAPP.Struct_PreferenceStruct.m_bIsSendADJ)
	{
		while (TRUE)
		{
			Sleep(N_INTERVAL);
			nCnt++;

			if (THEAPP.m_TCPClientService.GetAdjStatus(nDeepLearningModelID - 1, nMzNO - 1, nTrayNO - 1, nModuleNO - 1, nDefectCount) >= 0)
			{
				THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModelID - 1, nMzNO - 1, nTrayNO - 1, nModuleNO - 1, nDefectCount, RCV_WAIT);
				if (!THEAPP.m_TCPClientService.matMaskImageData[nTrayNO - 1][nModuleNO - 1][nDefectCount].empty())
				{
					cv::Mat cv_temp;
					cv::normalize(THEAPP.m_TCPClientService.matMaskImageData[nTrayNO - 1][nModuleNO - 1][nDefectCount], cv_temp, 0, 255, cv::NORM_MINMAX, -1);
					//cv::imwrite("D:\\TEMP\\TEMP2.jpg", cv_temp);
				}
				return std::move(THEAPP.m_TCPClientService.matMaskImageData[nTrayNO - 1][nModuleNO - 1][nDefectCount]);
			}
			if (GetTickCount() - nWaitMilsec > nModuleTimeOut || nCnt >= nMaxRepeatCnt) //nMaxRepeatCnt 관련 추가 (혹시모를 경우 대비해서 20200919)
			{
				THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModelID - 1, nMzNO - 1, nTrayNO - 1, nModuleNO - 1, nDefectCount, RCV_TIMEOUT);

				return cv::Mat();
			}
		}
	}
	else
		return cv::Mat();

}

int CADJClientService::SendFAIImagetoADJ(stTCPPacket stPacket, int nClientNO, int nWaitMilsec)
{
	BYTE* pbytePacket = nullptr;
	pbytePacket = new BYTE[MAX_BUF_SIZE];
	//패킷 만들어 보냄.
	int nDataSize = stPacket.MakeFAIImagePacket(pbytePacket);

	//Sleep(3);
	m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].TCPSend(pbytePacket, nDataSize);

	int nDeepLearningModelID = atoi((const char*)stPacket.byteDeepLearningModelID) - 1 + ((nClientNO / MAX_MULTI_CONNECTION_NUM) * DEEP_MODEL_NUM);	//주의 : 수아 모델 ID 는 1~16 모듈넘버 1~42, 트레이넘버 1부터 시작
	int nModuleNo = atoi((const char*)stPacket.byteModuleNo) - 1;
	int nDefectCnt = atoi((const char*)stPacket.byteDefectCnt);			//Defect Cnt 는 0부터 시작
	int nMagazineNo = atoi((const char*)stPacket.byteMagazineNo) - 1;
	int nTrayNo = atoi((const char*)stPacket.byteTrayNo) - 1;

	int dwBytes;

	strLog.Format("Send FAI data thread, Client: %d, Model: %d, Tray: %d, Module: %d, Defect count: %d",
		nClientNO,
		nDeepLearningModelID + 1,
		nTrayNo + 1,
		nModuleNo + 1,
		nDefectCnt + 1);
	THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

	if (stPacket.pByteImageData != NULL)
		free(stPacket.pByteImageData);

	int nModuleTimeOut = 0;

	if (pbytePacket != nullptr)
	{
		delete pbytePacket;
		pbytePacket = nullptr;
	}
	if (THEAPP.Struct_PreferenceStruct.m_bUseADJTimeOut)
	{
		nModuleTimeOut = THEAPP.Struct_PreferenceStruct.m_nADJTimeOut;
	}
	else
	{
		nModuleTimeOut = ADJ_SEND_TIMEOUT;
	}

	const int N_INTERVAL = 5;
	int nMaxRepeatCnt = (int)(nModuleTimeOut / N_INTERVAL);
	int nCnt = 0;

	if (THEAPP.Struct_PreferenceStruct.m_bIsSendADJ)
	{
		while (TRUE)
		{
			Sleep(N_INTERVAL);
			nCnt++;

			if (THEAPP.m_TCPClientService.GetFAIAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt) >= 0)
			{
				int ADJ_Reuslt = THEAPP.m_TCPClientService.GetFAIAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt);
				THEAPP.m_TCPClientService.SetFAIAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt, RCV_WAIT);
				return ADJ_Reuslt;
			}
			if (GetTickCount() - nWaitMilsec > nModuleTimeOut || nCnt >= nMaxRepeatCnt) //nMaxRepeatCnt 관련 추가 (혹시모를 경우 대비해서 20200919)
			{
				THEAPP.m_TCPClientService.SetFAIAdjStatus(nDeepLearningModelID, nMagazineNo, nTrayNo, nModuleNo, nDefectCnt, RCV_TIMEOUT);
				return RCV_TIMEOUT;
			}
		}
	}
	else
		return RCV_TIMEOUT;
}

void CADJClientService::ThreeHandShakingRequest(int nClientNO)
{
	stTCPPacket tcpPacket;
	int nTokenPos = 1 + ((PACKET_STRING_SIZE + 1) * 2); //ADJ의 GetTotalByteTokenPos()값은 2
	CString strByteLength = "614"; //611은 nTokenPos 3은 611자체의 길이
	int curIdx = 1;

	BYTE byteData[1000] = { 0, };
	//CString str = "";
	CString strPort;
	strPort.Format("%d", THEAPP.Struct_PreferenceStruct.m_iADJ_Port[nClientNO / MAX_MULTI_CONNECTION_NUM] + (10 * (nClientNO % MAX_MULTI_CONNECTION_NUM)));

	for (int i = 0; i < strPort.GetLength(); i++)
	{
		tcpPacket.bytePort[i] = strPort.GetAt(i);
		if (i + 1 == strPort.GetLength())
			tcpPacket.bytePort[i + 1] = NULL;
	}

	int nDataSize = tcpPacket.MakeWatchDogPacket(byteData);

	m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].TCPSend(byteData, nDataSize);
}


void CADJClientService::ADJResetRequest(int nClientNO)
{
	int nTokenPos = 1 + ((PACKET_STRING_SIZE + 1) * 10); //ADJ의 GetTotalByteTokenPos()값은 10

	CString strByteLength = "614"; //611은 nTokenPos 3은 611자체의 길이

	BYTE byteData[1000] = { 0, };
	CString str = "";
	CString strRep;
	strRep.Format("@ADJRESET,%d", THEAPP.Struct_PreferenceStruct.m_iADJ_Port[nClientNO / MAX_MULTI_CONNECTION_NUM] + (nClientNO % MAX_MULTI_CONNECTION_NUM * 10));

	memcpy(byteData, (const char*)strRep, strRep.GetLength() + 1);
	memcpy(&byteData[nTokenPos], (const char*)strByteLength, 3);

	m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].TCPSend(byteData, atoi(strByteLength));
}

static UINT TCPPostProcessing_Client(LPVOID lParam)
{
	CString strLog;

	DWORD ret;

	int nClientNo = 0;

	CADJClientService* m_ClientService = (CADJClientService*)lParam;

	int nClientNO = m_ClientService->m_nClientNO;
	HANDLE* hRecvHandle = m_ClientService->m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].GetTCPReceiveHanlde();

	int nTryCnt = 0;

	while (TRUE)
	{
		Sleep(1);

		ret = WaitForMultipleObjects(TCP_EVENT_NUM, hRecvHandle, FALSE, NET_EVENT_WAIT_TIME);

		if (ret == WAIT_FAILED)
		{
			strLog.Format("ADJ%d_%d connect WAIT_FAILED", nClientNO / MAX_MULTI_CONNECTION_NUM + 1, nClientNO % MAX_MULTI_CONNECTION_NUM + 1);
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
			return 0;
		}
		else if (ret == WAIT_ABANDONED)
		{
			strLog.Format("ADJ%d_%d connect WAIT_ABANDONED", nClientNO / MAX_MULTI_CONNECTION_NUM + 1, nClientNO % MAX_MULTI_CONNECTION_NUM + 1);
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
			continue;
		}
		else if (ret == WAIT_TIMEOUT)
		{
			continue;
		}
		else if (ret == WAIT_OBJECT_0 + CONNECT_EVENT)												//Connect Event
		{
			//TO DO: Accept
			strLog.Format("ADJ%d_%d connect success", nClientNO / MAX_MULTI_CONNECTION_NUM + 1, nClientNO % MAX_MULTI_CONNECTION_NUM + 1);
			THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

			m_ClientService->m_bConnect[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM] = TRUE;
			m_ClientService->m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].SetTcpConnect(TRUE);
			m_ClientService->m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].ResetTCPReceiveHandle(CONNECT_EVENT);		//ASSERT : To reset manual event
		}
		else if (ret == WAIT_OBJECT_0 + RECV_EVENT)													//Receive Event
		{
			m_ClientService->m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].ResetTCPReceiveHandle(RECV_EVENT);							//ASSERT : To reset manual event
			//AfxBeginThread( GetTCPReceiveData, (LPVOID)&m_ClientService->m_arrClient[ nClientNo ] );		//20191028 commented

			int nDeepLearningModel = 0;

			BYTE* m_pbyteBuf = nullptr;

			while (!m_ClientService->m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].m_qRecvBuffer.IsEmpty())
			{
				m_pbyteBuf = m_ClientService->m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].m_qRecvBuffer.next();

				stTCPPacket m_pstTCPPacket;
				int nStatus = m_pstTCPPacket.GetPacket(m_pbyteBuf);

				if (nStatus == RECV_WATCHDOG)
				{
					m_ClientService->m_bRealConnect[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM] = TRUE;
					continue;
				}

				CString strIP = (const char*)m_pstTCPPacket.byteIP;
				int nDeepLearningModel = atoi((const char*)m_pstTCPPacket.byteDeepLearningModelID) + ((nClientNO / MAX_MULTI_CONNECTION_NUM) * DEEP_MODEL_NUM);
				int nModuleNO = atoi((const char*)m_pstTCPPacket.byteModuleNo);
				int nTrayNO = atoi((const char*)m_pstTCPPacket.byteTrayNo);
				int nDefectCnt = atoi((const char*)m_pstTCPPacket.byteDefectCnt);
				int nDeepLearningResult = atoi((const char*)m_pstTCPPacket.byteDeepLearningResult);
				int nMagazineNo = atoi((const char*)m_pstTCPPacket.byteMagazineNo);
				//float fMaxClassProb = atof((const char*)m_pstTCPPacket.byteMaxClassProb); //20200809 add //20200912 commented

				/* ADJ 서로 다른 PC에 연결할 시 풀어야함
				for(int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
				{
					if(!strIP.Compare(THEAPP.Struct_PreferenceStruct.m_strADJ_IP[i]))
						nDeepLearningModel += DEEP_MODEL_NUM*i;
				}
				*/
				if (nStatus == RECV_INSPECT_RESULT) {
					if (nDeepLearningModel > 0)
					{
						if (THEAPP.m_TCPClientService.GetAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt) == RCV_WAIT)
						{
							THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt, nDeepLearningResult);

							strLog.Format("Received data, LotID: %s, Tray: %d, Module: %d, Model: %d, Defect count: %d, Result: %d",
								(const char*)m_pstTCPPacket.byteLotID,
								nTrayNO,
								nModuleNO,
								nDeepLearningModel,
								nDefectCnt + 1,
								nDeepLearningResult);
							THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
						}
						else
						{
							THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt, RCV_WAIT);

							strLog.Format("Time out receive, LotID: %s, Tray: %d, Module: %d, Model: %d, Defect count: %d, Result: %d",
								(const char*)m_pstTCPPacket.byteLotID,
								nTrayNO,
								nModuleNO,
								nDeepLearningModel,
								nDefectCnt + 1,
								nDeepLearningResult);
							THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
						}
					}
					else
					{
						THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt, RCV_WAIT);

						strLog.Format("Logical error deepLearning model missmatching data, LotID: %s, Tray: %d, Module: %d, Model: %d, Defect count: %d, Result: %d",
							(const char*)m_pstTCPPacket.byteLotID,
							nTrayNO,
							nModuleNO,
							nDeepLearningModel,
							nDefectCnt + 1,
							nDeepLearningResult);
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
					}
				}
				else if (nStatus == RECV_INSPECTFAI_RESULT)
				{
					if (nDeepLearningModel > 0)
					{
						if (THEAPP.m_TCPClientService.GetFAIAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt) == RCV_WAIT)
						{
							THEAPP.m_TCPClientService.SetFAIAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt, nDeepLearningResult);

							strLog.Format("Received FAI data, LotID: %s, Tray: %d, Module: %d, Model: %d, Defect count: %d, Result: %d",
								(const char*)m_pstTCPPacket.byteLotID,
								nTrayNO,
								nModuleNO,
								nDeepLearningModel,
								nDefectCnt + 1,
								nDeepLearningResult);
							THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
						}
						else
						{
							THEAPP.m_TCPClientService.SetFAIAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt, RCV_WAIT);

							strLog.Format("FAI time out receive, LotID: %s, Tray: %d, Module: %d, Model: %d, Defect count: %d, Result: %d",
								(const char*)m_pstTCPPacket.byteLotID,
								nTrayNO,
								nModuleNO,
								nDeepLearningModel,
								nDefectCnt + 1,
								nDeepLearningResult);
							THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
						}
					}
				}
				else if (nStatus == RECV_INSPECT_SEGMENT_RESULT)
				{
					int nImageWidth = atoi((const char*)m_pstTCPPacket.byteImageWidth);
					int nImageHeight = atoi((const char*)m_pstTCPPacket.byteImageHeight);

					cv::Mat nMaskImageData;
					nMaskImageData.create(nImageHeight, nImageWidth, CV_8UC3);
					memcpy(nMaskImageData.data, m_pstTCPPacket.pByteImageData, nImageWidth*nImageHeight*3);
				/*	cv::imwrite("D:\\TEMP\\TEMP1.jpg", nMaskImageData);
					cv::Mat cv_temp;
					cv::normalize(nMaskImageData, cv_temp, 0, 255, cv::NORM_MINMAX, -1);
					cv::imwrite("D:\\TEMP\\TEMP2.jpg", cv_temp);*/
					if (nDeepLearningModel > 0)
					{
						if (THEAPP.m_TCPClientService.GetAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt) == RCV_WAIT)
						{
							THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt, nDeepLearningResult);
							THEAPP.m_TCPClientService.matMaskImageData[nTrayNO - 1][nModuleNO - 1][nDefectCnt] = nMaskImageData.clone();
							strLog.Format("Received data, LotID: %s, Tray: %d, Module: %d, Model: %d, Defect count: %d, Result: %d",
								(const char*)m_pstTCPPacket.byteLotID,
								nTrayNO,
								nModuleNO,
								nDeepLearningModel,
								nDefectCnt + 1,
								nDeepLearningResult);
							THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
						}
						else
						{
							THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt, RCV_WAIT);

							strLog.Format("Time out receive, LotID: %s, Tray: %d, Module: %d, Model: %d, Defect count: %d, Result: %d",
								(const char*)m_pstTCPPacket.byteLotID,
								nTrayNO,
								nModuleNO,
								nDeepLearningModel,
								nDefectCnt + 1,
								nDeepLearningResult);
							THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
						}
					}
					else
					{
						THEAPP.m_TCPClientService.SetAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt, RCV_WAIT);

						strLog.Format("Logical error deepLearning model missmatching data, LotID: %s, Tray: %d, Module: %d, Model: %d, Defect count: %d, Result: %d",
							(const char*)m_pstTCPPacket.byteLotID,
							nTrayNO,
							nModuleNO,
							nDeepLearningModel,
							nDefectCnt + 1,
							nDeepLearningResult);
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
					}
				}
				else
				{
					THEAPP.m_TCPClientService.SetFAIAdjStatus(nDeepLearningModel - 1, nMagazineNo - 1, nTrayNO - 1, nModuleNO - 1, nDefectCnt, RCV_WAIT);

					strLog.Format("Logical error FAI deepLearning model missmatching data, LotID: %s, Tray: %d, Module: %d, Model: %d, Defect count: %d, Result: %d",
						(const char*)m_pstTCPPacket.byteLotID,
						nTrayNO,
						nModuleNO,
						nDeepLearningModel,
						nDefectCnt + 1,
						nDeepLearningResult);
					THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
				}
			}
		}
		else if (ret == WAIT_OBJECT_0 + SERVER_CLOSE_EVENT)											//Close Event
		{
			strLog.Format("ADJ%d_%d connect SERVER_CLOSE_EVENT", nClientNO / MAX_MULTI_CONNECTION_NUM + 1, nClientNO % MAX_MULTI_CONNECTION_NUM + 1);
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
			//TO DO: Clsoe Event Process

			m_ClientService->m_bConnect[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM] = m_ClientService->m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].GetTcpConnect();

			if (m_ClientService->m_bConnect[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM])
			{
				THEAPP.m_TCPClientService.m_bConnect[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM] = TRUE;
			}
			else
			{
				THEAPP.m_TCPClientService.m_bConnect[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM] = FALSE;
			}
			m_ClientService->m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].ResetTCPReceiveHandle(SERVER_CLOSE_EVENT);	//ASSERT : To reset manual event
		}
		else if (ret == WAIT_OBJECT_0 + RESTART_EVENT)
		{
			m_ClientService->m_bTCPThreadRun[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM] = FALSE;
			m_ClientService->m_arrClient[nClientNO / MAX_MULTI_CONNECTION_NUM][nClientNO % MAX_MULTI_CONNECTION_NUM].ResetTCPReceiveHandle(RESTART_EVENT);	//ASSERT : To reset manual event

			return 0;
		}
	}

	return 1;
}

void CADJClientService::ADJConnectAliveCheck()
{
	int nTryCnt = 0;
	BOOL bReset = FALSE, bConnectionClose = FALSE;
	int i_ADJMaxConnect = THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO;
	DWORD ADJConnectAliveCheckStartTime;
	DWORD ADJConnectAliveCheckingTime;

	EnterCriticalSection(&THEAPP.m_csADJAliveCheck);

	BOOL bIsRealADJConnect = FALSE;

	for (nTryCnt = 0; nTryCnt < 3; nTryCnt++)
	{
		strLog.Format("ADJConnectAliveCheck try %d Start", nTryCnt + 1);
		THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
		bReset = FALSE;
		bConnectionClose = FALSE;

		for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[i])
			{
				for (int j = 0; j < i_ADJMaxConnect; j++)
				{
					ADJConnectAliveCheckStartTime = GetTickCount();

					THEAPP.m_TCPClientService.m_bRealConnect[i][j] = FALSE;

					strLog.Format("ADJConnectAliveCheck, i: %d, j: %d, Connect: %d", i, j, THEAPP.m_TCPClientService.m_bConnect[i][j]);
					THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

					if (THEAPP.m_TCPClientService.m_bConnect[i][j])
					{
						THEAPP.m_TCPClientService.ThreeHandShakingRequest(i*MAX_MULTI_CONNECTION_NUM + j);
					}
				} //for (int j = 0; j < i_ADJMaxConnect; j++)
			} // if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[i])
		} //for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)


		for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[i])
			{
				while (!bIsRealADJConnect && !bReset && !bConnectionClose)
				{
					Sleep(5);

					bIsRealADJConnect = TRUE;
					for (int j = 0; j < i_ADJMaxConnect; j++)
					{
						bIsRealADJConnect *= THEAPP.m_TCPClientService.m_bRealConnect[i][j];
						if (!bIsRealADJConnect)
						{
							break;
						}
					}

					ADJConnectAliveCheckingTime = GetTickCount() - ADJConnectAliveCheckStartTime;

					if (ADJConnectAliveCheckingTime >= 300 && !bIsRealADJConnect && !bReset && !bConnectionClose)
					{
						bReset = TRUE;
						bConnectionClose = TRUE;
						//bReset = TRUE;
						strLog.Format("ADJConnectAliveCheck try %d fail, Time(ms): %d", nTryCnt + 1, ADJConnectAliveCheckingTime);
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
						break;
					}
					if (bIsRealADJConnect)
					{
						strLog.Format("ADJConnectAliveCheck try %d success, Time(ms): %d", nTryCnt + 1, ADJConnectAliveCheckingTime);
						THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
						break;
					}
				}

				if (bConnectionClose)
				{
					for (int k = 0; k < i_ADJMaxConnect; k++)
					{
						THEAPP.m_TCPClientService.m_arrClient[i][k].TCPonClose();
						THEAPP.m_TCPClientService.m_bConnect[i][k] = FALSE;
						Sleep(10);

						bReset = TRUE;
					}
					break;
				}
			}
		}
		if (bIsRealADJConnect)
			break;

		if (bReset)
		{
			strLog.Format("ADJConnectAliveCheck re-start");
			THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
			THEAPP.m_TCPClientService.ReStart();
			strLog.Format("ADJConnectAliveCheck initialize");
			THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
			THEAPP.m_TCPClientService.Initialize();
			strLog.Format("ADJConnectAliveCheck initialize done");
			THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
			Sleep(100);

			for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
			{
				if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[i])
				{
					for (int j = 0; j < i_ADJMaxConnect; j++)
					{
						if (THEAPP.m_TCPClientService.m_arrClient[i][j].TCPConnect())
						{
							Sleep(20);
							strLog.Format("ADJConnectAliveCheck ADJ%d_%d re-connection success", i + 1, j + 1);
							THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
						}
						else
						{
							strLog.Format("ADJConnectAliveCheck ADJ%d_%d re-connection fail", i + 1, j + 1);
							THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
							break;
						}
					}
				}
			}
		}
	}

	LeaveCriticalSection(&THEAPP.m_csADJAliveCheck);
}

int CADJClientService::AssignDeepLearningModel(int iClientNO, int iVisionCamType, CString sADJInspectionTypeCode, CString sADJDefectDetail) //iCLientNO에 따른 모델 배정은 겹치면 안됌.
{
	int nTempModel = 0;

	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bSideTotal[i][j] == TRUE)
			{
				for (int k = 0; k < MAX_DEFECT_CNT; k++)
				{
					if (THEAPP.Struct_PreferenceStruct.m_nADJThreadNO_Integrated[i][j][k] == iVisionCamType
						&& !sADJDefectDetail.Compare(THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Integrated_Name[i][j][k])
						&& !sADJInspectionTypeCode.Compare(THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Integrated_Pos[i][j][k]))
					{
						return i * DEEP_MODEL_NUM + j + 1; // ADJ 서로 다른 PC에 연결할 시 풀어야함
					}
				}
			}
			else
			{
				nTempModel = iVisionCamType; //jwj 20190711 add  ( side 통합 bug fix )

				int a = THEAPP.Struct_PreferenceStruct.m_nADJThreadNO[i][j];
				CString b = THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Name[i][j];
				CString c = THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Pos[i][j];
				if (THEAPP.Struct_PreferenceStruct.m_nADJThreadNO[i][j] == nTempModel
					&& !sADJDefectDetail.Compare(THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Name[i][j])
					&& !sADJInspectionTypeCode.Compare(THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Pos[i][j]))
				{
					return (i * DEEP_MODEL_NUM) + j + 1;	//수아 모델은 1~ 딥러닝모델수*최대 접속 가능한 클라이언트 수 까지 배정, ADJ 서로 다른 PC에 연결할 시 풀어야함
				}
			}
		}
	}
	return 0;
}

int CADJClientService::AssignDeepLearningSegmentModel(int iClientNO, int iVisionCamType, CString sADJInspectionTypeCode, CString sADJDefectDetail) //iCLientNO에 따른 모델 배정은 겹치면 안됌.
{
	int nTempModel = 0;

	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bSideTotal[i][j] == TRUE)
			{
				for (int k = 0; k < MAX_DEFECT_CNT; k++)
				{
					if (THEAPP.Struct_PreferenceStruct.m_nADJThreadNO_Integrated_Segment[i][j][k] == iVisionCamType
						&& !sADJDefectDetail.Compare(THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Integrated_Segment_Name[i][j][k])
						&& !sADJInspectionTypeCode.Compare(THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Integrated_Segment_Pos[i][j][k]))
					{
						return i * DEEP_MODEL_NUM + j + 1; // ADJ 서로 다른 PC에 연결할 시 풀어야함
					}
				}
			}
			else
			{
				nTempModel = iVisionCamType; //jwj 20190711 add  ( side 통합 bug fix )

				int a = THEAPP.Struct_PreferenceStruct.m_nADJThreadNO[i][j];
				CString b = THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Name[i][j];
				CString c = THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Pos[i][j];
				if (THEAPP.Struct_PreferenceStruct.m_nADJThreadNO[i][j] == nTempModel
					&& !sADJDefectDetail.Compare(THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Name[i][j])
					&& !sADJInspectionTypeCode.Compare(THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Pos[i][j]))
				{
					return (i * DEEP_MODEL_NUM) + j + 1;	//수아 모델은 1~ 딥러닝모델수*최대 접속 가능한 클라이언트 수 까지 배정, ADJ 서로 다른 PC에 연결할 시 풀어야함
				}
			}
		}
	}
	return 0;
}

int CADJClientService::AssignDeepLearningFAIModel(int FAI_number) {
	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			for (int k = 0; k < MAX_FAI_ITEM; k++)
			{
				if (THEAPP.Struct_PreferenceStruct.m_nADJFAIModel_Integrated[i][j][k] == FAI_number)
				{
					return i * DEEP_MODEL_NUM + j + 1;
				}
			}
		}
	}
	return 0;
}


CString CADJClientService::GetDeepLearningModelNameFromIndex(int nDeepLearningModelID, int iVisionCamType, CString sADJInspectionTypeCode, CString sADJDefectDetail)
{
	CString strDeepLearningModelName, strCAM;
	int nClientNO = (int)((nDeepLearningModelID - 1) / DEEP_MODEL_NUM);
	BOOL bMultiImage = THEAPP.Struct_PreferenceStruct.m_bSideTotal[nClientNO][(nDeepLearningModelID - 1) % DEEP_MODEL_NUM];

	if (bMultiImage)
	{
		int nCam = iVisionCamType;
		switch (nCam)
		{
		case 0: strCAM = "TOP"; break;
		case 1: strCAM = "BOTTOM"; break;
		default: strCAM = "UNKNOWN"; break;
		}
		strDeepLearningModelName.Format("%s_%s_%s",
			strCAM,
			sADJInspectionTypeCode,
			sADJDefectDetail);
	}
	else
	{
		int nCam = THEAPP.Struct_PreferenceStruct.m_nADJThreadNO[nClientNO][(nDeepLearningModelID - 1) % DEEP_MODEL_NUM];
		switch (nCam)
		{
		case 0: strCAM = "TOP"; break;
		case 1: strCAM = "BOTTOM"; break;
		default: strCAM = "UNKNOWN"; break;
		}

		strDeepLearningModelName.Format("%s_%s_%s",
			strCAM,
			THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Pos[nClientNO][(nDeepLearningModelID - 1) % DEEP_MODEL_NUM],
			THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Name[nClientNO][(nDeepLearningModelID - 1) % DEEP_MODEL_NUM]);
	}
	return strDeepLearningModelName;
}

CString CADJClientService::GetDeepLearningModelNameFromIndex(int nDeepLearningModelID, int iVisionCamType)
{
	CString strDeepLearningModelName, strCAM;
	int nClientNO = (int)((nDeepLearningModelID - 1) / DEEP_MODEL_NUM);

	int nCam = THEAPP.Struct_PreferenceStruct.m_nADJThreadNO[nClientNO][(nDeepLearningModelID - 1) % DEEP_MODEL_NUM];

	strDeepLearningModelName.Format("%s_%s",
		THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Pos[nClientNO][(nDeepLearningModelID - 1) % DEEP_MODEL_NUM],
		THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Name[nClientNO][(nDeepLearningModelID - 1) % DEEP_MODEL_NUM]);

	return strDeepLearningModelName;
}

//250223 KDH ReviewImage 와 동일한 이미지 생성을 위한 수정작업 진행
int CADJClientService::DoDeeplearningInspection(int nThreadNO,
	int iInspectionBufferIdx,
	int iVisionCamType,
	HObject HVisionAllDefectRgn,
	int iPcVisionNo,
	int iNoInspectImage,
	int iMzNo,
	int iTrayNo,
	int iModuleNo,
	CString sLotID,
	CString sBarcodeID,
	DWORD iModuleTotalStartTime,
	HObject(&HADJFilteredDefectRgn)[MAX_IMAGE_TAB],
	int &iInspectionTypeResult)
{
	try
	{
		CString sVisionCamType_Short;
		sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iVisionCamType];

		BOOL bSpecialDefect = FALSE;
		int iSpecialDefectTransCode;
		CInspectService* pInspectService = THEAPP.m_pInspectService;
		//int iInspectionBufferIdx = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iInspectionBufferIdx;
		//DWORD iModuleTotalStartTime = GetTickCount();

		int iCurDeepLearningModel = 0;
		int iLastDeepLearningModel = 0;
		BOOL bIsTimeOut = FALSE;

		m_nADJDefectCnt[nThreadNO] = 0;

		int iROIIndex, iTabIdx;
		int iNoInspectROI = 0;
		int iImageIndex[MAX_IMAGE_COUNT] = { -1, };
		BOOL bAdjSave = FALSE;
		GTRegion *pInspectROIRgn;
		HObject HROIRgn, HADJDefectRgn;
		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		HObject HVisionAllDefectConnRgn, HADJSaveDefectRgn, HADJSaveImage[MAX_IMAGE_COUNT], HADJSaveRegion;
		HObject HVisionAllDefectRgn_EachImage[MAX_IMAGE_TAB];
		HObject HDefectRgn_EachImage[MAX_IMAGE_TAB];
		Hlong lBlobArea;
		double dBlobCenterX, dBlobCenterY;
		HTuple HlROIArea, HlBlobArea, HdBlobCenterX, HdBlobCenterY;
		int iCamImageWidth, iCamImageHeight;
		CString sADJDefectCode, sADJInspectionTypeCode, sADJDefectDetail;
		BOOL bTempSpecialDefect;
		CString sTempSpecialDefectTransCode, sADJTimeStamp;
		int iReviewSaveCnt = 0;

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
		HObject HMaxAreaBlobRgn, HRepBlobRgn;
		int iMaxDefectInspectonTypeIndex, iMaxDefectDefectNameIndex;

		HTuple HNoDefect, HNoImageDefect;
		Hlong lNoDefect;
		Hlong lNoDefectCount, lNoImageDefectCount;

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

		BYTE* pbyteImgData = NULL;

		THEAPP.g_arriThreadCnt[iModuleNo - 1] = 0;
		while (!m_qADJBuffer[iModuleNo - 1].IsEmpty())
			m_qADJBuffer[iModuleNo - 1].next();

		////////////////////////////////////////////////////////////////////////////
		///// Remove overlapped defects

		HObject HTempRgn1, HTempRgn2;
		HObject HTotalDefectRgn[MAX_IMAGE_TAB], HTotalImageDefectRgn;
		Hlong lMaxArea, lMaxAreaImageIndex;

		for (int nTabIdx = 0; nTabIdx < MAX_IMAGE_TAB; nTabIdx++)
			GenEmptyObj(&HTotalDefectRgn[nTabIdx]);

		//for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
		//{
		//	for (int j = 0; j < MAX_DEFECT_NAME; j++)
		//	{
		//		if (nThreadNO == 0)
		//		{
		//			if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[i][j]) == TRUE)
		//			{
		//				ConcatObj(HTotalDefectRgn, THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[i][j], &HTotalDefectRgn);
		//			}
		//		}
		//		else if (nThreadNO == 1)
		//		{
		//			if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[i][j]) == TRUE)
		//			{
		//				ConcatObj(HTotalDefectRgn, THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[i][j], &HTotalDefectRgn);
		//			}
		//		}
		//	}
		//}
		//if (THEAPP.m_pGFunction->ValidHRegion(HTotalDefectRgn) == TRUE)
		//{
		//	Union1(HTotalDefectRgn, &HTotalDefectRgn); //jwj commented //20200527 버그 수정 //20200912 uncommented(PG Down되어 원복)
		//	Connection(HTotalDefectRgn, &HTotalDefectRgn); //jwj commented //20200527 버그 수정 (다른영상 / 다른불량의 불량리전이 겹치면 한 불량항목만 ADJ로가는버그 수정) //20200812 uncommented
		//}

		////////// 불량 ROI 순차적
		strLog.Format("DoDeeplearningInspection start, Total: %d", iNoInspectImage);
		THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
		for (int iImageNo = 0; iImageNo < MAX_IMAGE_TAB; iImageNo++)
		{
			//if (THEAPP.m_pGFunction->ValidHRegion(HTotalDefectRgn)==TRUE)
			//{
			//Union1(HTotalDefectRgn, &HTotalDefectRgn); //jwj commented //20200527 버그 수정 //20200912 uncommented(PG Down되어 원복)
			//Connection(HTotalDefectRgn, &HTotalDefectRgn); //jwj commented //20200527 버그 수정 (다른영상 / 다른불량의 불량리전이 겹치면 한 불량항목만 ADJ로가는버그 수정) //20200812 uncommented

			if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][iImageNo]) == FALSE)
				continue;

			lMaxBlobArea = 0;
			GenEmptyObj(&HMaxAreaBlobRgn);
			GenEmptyObj(&HTotalImageDefectRgn);

			iMaxDefectInspectonTypeIndex = -1;
			iMaxDefectDefectNameIndex = -1;

			///////// 불량 정보  => 불량 종류 중에서 가장 큰 불량 선택
			for (int x = 0; x < MAX_INSPECTION_TYPE; x++)
			{
				for (int y = 0; y < MAX_DEFECT_NAME; y++)
				{
					switch (nThreadNO)
					{
					case 0:
						if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
							continue;
						Intersection(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][iImageNo], THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
						if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
							continue;
						Connection(THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[x][y], &HConnRgn);
						break;

					case 1:
						if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
							continue;
						Intersection(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][iImageNo], THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
						if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
							continue;
						Connection(THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[x][y], &HConnRgn);
						break;

					case 2:
						if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg3->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
							continue;
						Intersection(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][iImageNo], THEAPP.m_pInspectAdminViewHideDlg3->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
						if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
							continue;
						Connection(THEAPP.m_pInspectAdminViewHideDlg3->m_HRosReviewSpecApplyDefectRgn[x][y], &HConnRgn);
						break;

					case 3:
						if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg4->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
							continue;
						Intersection(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][iImageNo], THEAPP.m_pInspectAdminViewHideDlg4->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
						if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
							continue;
						Connection(THEAPP.m_pInspectAdminViewHideDlg4->m_HRosReviewSpecApplyDefectRgn[x][y], &HConnRgn);
						break;

					default:
						strLog.Format("DoDeeplearningInspection, ThreadNo: %d", nThreadNO);
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
						break;
					}

					SelectShapeProto(HConnRgn, HIntersectRgn, &HRosDisplayRgn, "overlaps_abs", 1, MAX_DEF);

					if (THEAPP.m_pGFunction->ValidHRegion(HRosDisplayRgn) == FALSE)		// add
						continue;

					ConcatObj(HTotalImageDefectRgn, HRosDisplayRgn, &HTotalImageDefectRgn);	// Display
					ConcatObj(HTotalDefectRgn[iImageNo], HRosDisplayRgn, &HTotalDefectRgn[iImageNo]);	// Display
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

			if (iMaxDefectInspectonTypeIndex == -1 || iMaxDefectDefectNameIndex == -1) {		// add
				strLog.Format("DoDeeplearningInspection, MaxDefectInspectonTypeIndex: %d, MaxDefectDefectNameIndex: %d", iMaxDefectInspectonTypeIndex, iMaxDefectDefectNameIndex);
				THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
				continue;
			}

			AreaCenter(HTotalImageDefectRgn, &HlArea, &HdCenterY, &HdCenterX);

			HTuple HBiggerAreaIndex;
			Hlong lBiggerDefectIndex;
			TupleSortIndex(HlArea, &HBiggerAreaIndex);
			TupleInverse(HBiggerAreaIndex, &HBiggerAreaIndex);

			HObject HDisplayDefectRgn;
			int iRepBlobCenterX, iRepBlobCenterY;

			CountObj(HTotalImageDefectRgn, &HNoDefect);
			lNoDefectCount = HNoDefect[0].I();

			strLog.Format("DoDeeplearningInspection start, DefectTotal: %d", lNoDefectCount);
			THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

			for (int DefectIndex = 0; DefectIndex < lNoDefectCount; DefectIndex++)
			{
				strLog.Format("DoDeeplearningInspection, DefectIndex: %d, Total: %d", DefectIndex, lNoDefectCount);
				THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
				//lBiggerDefectIndex = HBiggerAreaIndex[DefectIndex].L();
				SelectObj(HTotalImageDefectRgn, &HDisplayDefectRgn, DefectIndex + 1);
				AreaCenter(HDisplayDefectRgn, &HlArea, &HdCenterY, &HdCenterX);
				GenEmptyObj(&HADJSaveRegion);
				HADJSaveRegion = HDisplayDefectRgn;
				////////////////////////////////////////////////////////////////////////////////////////////
				iRepBlobCenterX = HdCenterX.D();
				iRepBlobCenterY = HdCenterY.D();

				bApplyADJ = FALSE;


				if (THEAPP.m_pGFunction->ValidHRegion(HDisplayDefectRgn) == FALSE) {	// add
					strLog.Format("DoDeeplearningInspection ValidHRegion(HSelectedRgn) FALSE");
					THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
					continue;
				}

				/*
				///////// ROI 정보 => 이미지중에서 불량이랑 가장 많이 오버랩되는 영상 선택

					Connection(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][j], &HTempRgn1);

					SelectShapeProto(HTempRgn1, HSelectedRgn, &HTempRgn2, "overlaps_abs", 1, MAX_DEF);

					if (THEAPP.m_pGFunction->ValidHRegion(HTempRgn2)==FALSE)
						continue;

					Union1(HTempRgn2, &HTempRgn2);

					AreaCenter(HTempRgn2, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
					lBlobArea = HlBlobArea.L();

					if (lBlobArea>lMaxArea)
					{
						lMaxArea = lBlobArea;
						lMaxAreaImageIndex = j;
					}
				}

				//if (lMaxAreaImageIndex < 0) {	// add
				//	strLog.Format("DoDeeplearningInspection, MaxAreaImageIndex < 0, %d", lMaxAreaImageIndex);
				//	THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
				//	continue;
				//}

				///////// ROI 이미지 탭 정보
				*/
				lMaxBlobArea = 0;
				iMaxDefectInspectonTypeIndex = -1;
				iMaxDefectDefectNameIndex = -1;


				for (int x = 0; x < MAX_INSPECTION_TYPE; x++)
				{
					for (int y = 0; y < MAX_DEFECT_NAME; y++)
					{
						switch (nThreadNO)
						{
						case 0:
							//if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
							//	continue;
							Intersection(HDisplayDefectRgn, THEAPP.m_pInspectAdminViewHideDlg1->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
							if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
								continue;
							break;

						case 1:
							//if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
							//	continue;
							Intersection(HDisplayDefectRgn, THEAPP.m_pInspectAdminViewHideDlg2->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
							if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
								continue;
							break;

						case 2:
							//if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg3->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
							//	continue;
							Intersection(HDisplayDefectRgn, THEAPP.m_pInspectAdminViewHideDlg3->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
							if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
								continue;
							break;

						case 3:
							//if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg4->m_HRosReviewSpecApplyDefectRgn[x][y]) == FALSE)
							//	continue;
							Intersection(HDisplayDefectRgn, THEAPP.m_pInspectAdminViewHideDlg4->m_HRosReviewSpecApplyDefectRgn[x][y], &HIntersectRgn);
							if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
								continue;
							break;
						}

						AreaCenter(HIntersectRgn, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
						lBlobArea = HlBlobArea.L();

						if (lBlobArea > lMaxBlobArea)
						{
							//HMaxAreaBlobRgn = HIntersectRgn;		// Display
							lMaxBlobArea = lBlobArea;

							iMaxDefectInspectonTypeIndex = x;
							iMaxDefectDefectNameIndex = y;
						}
					}
				}


				if (iMaxDefectInspectonTypeIndex == -1 || iMaxDefectDefectNameIndex == -1)
					continue;

				//GenEmptyObj(&HMaxAreaBlobRgn);
				int iTeachImageNo;
				int iTeachRoiLeft, iTeachRoiTop, iTeachRoiRight, iTeachRoiBottom, iTeachInspectionTabNo;
				int row, column, row1, column1, row2, column2;
				CString sTeachParamValue;
				CAlgorithmParam TeachAlgorithmParam;
				GTRegion *pSelectedROIRgn;

				//strLog.Format("DoDeeplearningInspection region union2");
				//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

				pSelectedROIRgn = THEAPP.GetDefectTeachingInfo(&HDisplayDefectRgn, iVisionCamType, iMzNo, iPcVisionNo, iInspectionBufferIdx, iMaxDefectInspectonTypeIndex + 1, iMaxDefectDefectNameIndex, &iTeachImageNo, &iTeachRoiLeft, &iTeachRoiTop, &iTeachRoiRight, &iTeachRoiBottom, &iTeachInspectionTabNo, &TeachAlgorithmParam);
				if (pSelectedROIRgn == NULL)
				{
					strLog.Format("DoDeeplearningInspection pSelectedROIRgn Null");
					THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
					continue;
				}

				SmallestRectangle1(HDisplayDefectRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

				row1 = HlLTPointY[0].L();
				column1 = HlLTPointX[0].L();
				row2 = HlRBPointY[0].L();
				column2 = HlRBPointX[0].L();

				row1 = row1 - pInspectService->m_pInspectAlgorithm[iVisionCamType].m_dGlobalAlignDeltaY[iInspectionBufferIdx][iTeachImageNo - 1];
				row2 = row2 - pInspectService->m_pInspectAlgorithm[iVisionCamType].m_dGlobalAlignDeltaY[iInspectionBufferIdx][iTeachImageNo - 1];
				column1 = column1 - pInspectService->m_pInspectAlgorithm[iVisionCamType].m_dGlobalAlignDeltaX[iInspectionBufferIdx][iTeachImageNo - 1];
				column2 = column2 - pInspectService->m_pInspectAlgorithm[iVisionCamType].m_dGlobalAlignDeltaX[iInspectionBufferIdx][iTeachImageNo - 1];

				double dZoom = 1;
				if (THEAPP.Struct_PreferenceStruct.m_bReduceRawImage)
					dZoom = THEAPP.Struct_PreferenceStruct.m_dRawImageZoomRatio;

				row1 = row1 * dZoom;
				column1 = column1 * dZoom;
				row2 = row2 * dZoom;
				column2 = column2 * dZoom;

				sTeachParamValue.Format("%d,%d,%d,%d_Tab%d", row1, column1, row2, column2, iTeachInspectionTabNo);

				CString strTempTypeCode = _T("");
				CString strTempDefectDetail = _T("");
				CString sInspectionType_Short = "";
				strTempTypeCode = g_strInspectionTypeName_Short[iMaxDefectInspectonTypeIndex].c_str();
				strTempDefectDetail = THEAPP.GetDefectCode(iMaxDefectDefectNameIndex, &iSpecialDefectTransCode);
				CString strTempVisionName_Short = sVisionCamType_Short;

				CString strTempADJDefectName, strDefectCode, strTempCam, strReviewDefectCode, strVisionDefectCode;


				iType = iMaxDefectInspectonTypeIndex + 1;
				//*** Review Defect Code */
				//250221 KDH DefectCode 수정

				CString strVisionNameShortcut = sVisionCamType_Short.Left(1);
				strTempCam = strVisionNameShortcut;

				strTempADJDefectName.Format("%s_%s_%s", strTempTypeCode, strTempVisionName_Short, strTempDefectDetail);
				strDefectCode.Format("%s_%s_%s", strTempVisionName_Short, strTempTypeCode, strTempDefectDetail);
				strVisionDefectCode.Format("%s_%s_%s", strTempTypeCode, strTempCam, strTempDefectDetail);
				strReviewDefectCode.Format("%s-%s-%s", strTempTypeCode, strTempCam, strTempDefectDetail);
				//strLog.Format("DoDeeplearningInspection 2nd, DefectIndex: %d, DefectCode: %s", DefectIndex, strDefectCode);
				//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
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
				CString ConcatFilename, ConcatReviewImage;
				//strLog.Format("DoDeeplearningInspection 3rd, Check bApplyADJ: %d", bApplyADJ);
				//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
				if (bApplyADJ == TRUE)
				{
					// HADJSaveImage = HMaxAreaBlobRgn; 
					for (int i = 0; i < MAX_IMAGE_COUNT; i++)
						GenEmptyObj(&HADJSaveImage[i]);		// add


					int m_AdjImageCount, m_NowAdjDefectIndex;
					m_AdjImageCount = 1;
					m_NowAdjDefectIndex = -1;
					//m_AdjImageCount = new int(THEAPP.m_iAdjSelectImageCount);
					bAdjSave = FALSE;

					if (THEAPP.Struct_PreferenceStruct.m_bUseADJMultiimage)
					{
						for (int i = 0; i < THEAPP.m_iAdjSelectImageCount; i++)
						{
							strCameraIndex = sVisionCamType_Short;
							if ((sADJInspectionTypeCode == THEAPP.m_Struct_AdjImageInfo[i].m_sInspectionType) &&
								(strTempDefectDetail == THEAPP.m_Struct_AdjImageInfo[i].m_sDefectName) &&
								(strCameraIndex == THEAPP.m_Struct_AdjImageInfo[i].m_sVisionCam))
							{
								m_NowAdjDefectIndex = i;
								m_AdjImageCount = THEAPP.m_Struct_AdjImageInfo[i].m_iImageCount;
								for (int k = 0; k < m_AdjImageCount; k++)
								{
									iImageIndex[k] = THEAPP.m_Struct_AdjImageInfo[i].m_iImageIndex[k];
									bAdjSave = TRUE;
								}
								break;
							}
							else
							{
								m_NowAdjDefectIndex = -1;	// ADJ 영상 이미지 지정 X
								m_AdjImageCount = 1;
								iImageIndex[0] = iImageNo;
							}
							//}
						}
					}
					if (THEAPP.m_iAdjSelectImageCount < 1 || !THEAPP.Struct_PreferenceStruct.m_bUseADJMultiimage)
					{
						m_NowAdjDefectIndex = -1;	// ADJ 영상 이미지 지정 X
						m_AdjImageCount = 1;
						iImageIndex[0] = iImageNo;
					}
					//strLog.Format("DoDeeplearningInspection 4th, ImageCount: %d", m_AdjImageCount);
					//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
					//AreaCenter(HMaxAreaBlobRgn, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);		// add

					lLTPointY = (int)HdBlobCenterY[0].D() - THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2;
					lLTPointX = (int)HdBlobCenterX[0].D() - THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2;
					lRBPointY = (int)HdBlobCenterY[0].D() + THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2 - 1;
					lRBPointX = (int)HdBlobCenterX[0].D() + THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2 - 1;

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
					if (m_NowAdjDefectIndex > -1)
						for (int k = 0; k < m_AdjImageCount; k++)
						{
							CropRectangle1(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HInspectCImage[iInspectionBufferIdx][CHANNEL_TYPE_COLOR][iImageIndex[k]], &HADJSaveImage[k], lLTPointY, lLTPointX, lRBPointY, lRBPointX);
						}
					else
						CropRectangle1(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HInspectCImage[iInspectionBufferIdx][CHANNEL_TYPE_COLOR][iImageNo], &HADJSaveImage[0], lLTPointY, lLTPointX, lRBPointY, lRBPointX);


					if (THEAPP.Struct_PreferenceStruct.m_iADJImageSize != THEAPP.Struct_PreferenceStruct.m_iADJImageReSize)
						if (m_NowAdjDefectIndex > -1)
							for (int k = 0; k < m_AdjImageCount; k++)
							{
								ZoomImageSize(HADJSaveImage[k], &HADJSaveImage[k], THEAPP.Struct_PreferenceStruct.m_iADJImageReSize, THEAPP.Struct_PreferenceStruct.m_iADJImageReSize, "bicubic");
							}
						else
							ZoomImageSize(HADJSaveImage[0], &HADJSaveImage[0], THEAPP.Struct_PreferenceStruct.m_iADJImageReSize, THEAPP.Struct_PreferenceStruct.m_iADJImageReSize, "bicubic");


					/*
					//20200820 add start //불량 리전 빨간색으로 overpaint
					HObject HADJSaveImageCopy;
					CopyImage(HADJSaveImage, &HADJSaveImageCopy);
					HObject HADJOverpaintSaveImage, HDilatedBlobRgn, HTempADJImageR, HTempADJImageG, HTempADJImageB, HTempADJRgnBorder;

					//HDilatedBlobRgn.GenEmptyObj(); HADJOverpaintSaveImage.GenEmptyObj();
					//HTempADJImageR.GenEmptyObj(); HTempADJImageG.GenEmptyObj(); HTempADJImageB.GenEmptyObj();
					GenEmptyObj(&HDilatedBlobRgn);
					GenEmptyObj(&HADJOverpaintSaveImage);
					GenEmptyObj(&HTempADJImageR);
					GenEmptyObj(&HTempADJImageG);
					GenEmptyObj(&HTempADJImageB);
					GenEmptyObj(&HTempADJRgnBorder);

					if (THEAPP.Struct_PreferenceStruct.m_iMarkingImageRadius > 0)
					{
						DilationCircle(HMaxAreaBlobRgn, &HDilatedBlobRgn, HTuple(THEAPP.Struct_PreferenceStruct.m_iMarkingImageRadius));
						Boundary(HDilatedBlobRgn, &HTempADJRgnBorder, "inner");
						DilationCircle(HTempADJRgnBorder, &HTempADJRgnBorder, HTuple(1.5));
						Decompose3(HADJSaveImageCopy, &HTempADJImageR, &HTempADJImageG, &HTempADJImageB);
						MoveRegion(HTempADJRgnBorder, &HTempADJRgnBorder, -lLTPointY, -lLTPointX);

						OverpaintRegion(HTempADJImageR, HTempADJRgnBorder, HTuple(255), "fill");
						OverpaintRegion(HTempADJImageG, HTempADJRgnBorder, HTuple(0), "fill");
						OverpaintRegion(HTempADJImageB, HTempADJRgnBorder, HTuple(0), "fill");
						Compose3(HTempADJImageR, HTempADJImageG, HTempADJImageB, &HADJOverpaintSaveImage);
					}
					//20200820 add end
					*/
					GetLocalTime(&time);
					sADJTimeStamp.Format("%02d%02d%03d", time.wMinute, time.wSecond, time.wMilliseconds);

					for (int i = 0; i < m_AdjImageCount; i++)
					{
						if (m_AdjImageCount > 1)
						{
							if (iImageIndex[i] == lMaxAreaImageIndex)
							{
								ConcatFilename.Format("M%02d_%s_%s_%s_Img%02d(%d)_%s_%s", iModuleNo, sVisionCamType_Short, strVisionDefectCode, sADJTimeStamp, lMaxAreaImageIndex + 1, i + 1, sBarcodeID, sTeachParamValue);
								break;
							}
							else
								ConcatFilename.Format("M%02d_%s_%s_%s_Img%02d(%d)_%s_%s", iModuleNo, sVisionCamType_Short, strVisionDefectCode, sADJTimeStamp, lMaxAreaImageIndex + 1, -1, sBarcodeID, sTeachParamValue);
						}
						else
						{
							ConcatFilename.Format("M%02d_%s_%s_%s_Img%02d_%s_%s", iModuleNo, sVisionCamType_Short, strVisionDefectCode, sADJTimeStamp, iImageNo + 1, sBarcodeID, sTeachParamValue);
						}
					}

					ConcatReviewImage.Format("Module%d_%s_Image%02d_%s_adj_%s_%s", iModuleNo, sVisionCamType_Short, iImageNo + 1, sBarcodeID, strReviewDefectCode, sADJTimeStamp);

					if (m_nADJDefectCnt[nThreadNO] == MAX_DEFECT_CNT)
						m_nADJDefectCnt[nThreadNO] = 0;					//통신 에러로 인해 Count가 초기화 되지 않았을 시 프로그램 down 방지

					//strLog.Format("DoDeeplearningInspection 5th, ImageName: %s", ConcatFilename);
					//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

					BOOL bBreak = FALSE;
					HObject ConcatimageObj;
					GenEmptyObj(&ConcatimageObj);

					for (int i = 0; i < m_AdjImageCount; i++)
					{
						ConcatObj(ConcatimageObj, HADJSaveImage[i], &ConcatimageObj);
					}
					TileImages(ConcatimageObj, &ConcatimageObj, MAX_COMBINE_IMAGE_NUMBER, "horizontal");

					int iClientNO = 0;
					int iDeepLearningModelID = 0;
					BOOL m_bADJRequest = FALSE;

					iCurDeepLearningModel = iDeepLearningModelID = AssignDeepLearningModel(0, iVisionCamType, sADJInspectionTypeCode, sADJDefectDetail);	//ADJ로 보낼 모델인지 아닌지 판별과 동시에 결정. //주의사항 : iCLientNO에 따른 모델 배정은 겹치면 안됌.

					if ((THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[0] + THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[1] + THEAPP.Struct_PreferenceStruct.m_bUseLocalADJ) != 0)
						iClientNO = (iTrayNo + iModuleNo + m_nADJDefectCnt[nThreadNO]) % THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO;

					if (iDeepLearningModelID > DEEP_MODEL_NUM)
						iClientNO += MAX_MULTI_CONNECTION_NUM;


					if (((THEAPP.m_TCPClientService.m_bConnect[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM] && THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO / MAX_MULTI_CONNECTION_NUM]) || THEAPP.Struct_PreferenceStruct.m_bUseLocalADJ)
						&& THEAPP.Struct_PreferenceStruct.m_bIsSendADJ)
					{
						//strLog.Format("DoDeeplearningInspection 6th ADJ send image data make start");
						//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

						//strLog.Format("DoDeeplearningInspection, iCurDeepLearningModel: %d", iCurDeepLearningModel);
						//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
						if (iDeepLearningModelID != 0)
						{
							//m_bADJRequest = TRUE;	//ADJ이미지 의뢰 시 저장 X

							int testADJStartTime = GetTickCount();

							HTuple pImageDataDeepLearningR;
							HTuple pImageDataDeepLearningG;
							HTuple pImageDataDeepLearningB;
							HTuple type;
							HTuple HImageHeight;
							HTuple HImageWidth;

							pImageDataDeepLearningR = 0;
							pImageDataDeepLearningG = 0;
							pImageDataDeepLearningB = 0;
							type = 0;
							HImageHeight = 0;
							HImageWidth = 0;

							if (m_AdjImageCount > 1)
							{
								HalconCpp::GetImagePointer3(ConcatimageObj, &pImageDataDeepLearningR, &pImageDataDeepLearningG, &pImageDataDeepLearningB, &type, &HImageWidth, &HImageHeight);
							}
							else
							{
								HalconCpp::GetImagePointer3(HADJSaveImage[0], &pImageDataDeepLearningR, &pImageDataDeepLearningG, &pImageDataDeepLearningB, &type, &HImageWidth, &HImageHeight);
							}
							pbyteImgData = NULL;
							pbyteImgData = (BYTE*)malloc(sizeof(BYTE)*(HImageWidth.L()*HImageHeight.L() * 3));
							if (pbyteImgData == NULL)
							{
								strLog.Format("DoDeeplearningInspection pbyteImgData is Null");
								THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
								bBreak = TRUE;
								continue;
							}

							BYTE* pByteR;
							BYTE* pByteG;
							BYTE* pByteB;
							Hlong lImageWidth, lImageHeight;
							lImageWidth = HImageWidth.L();
							lImageHeight = HImageHeight.L();
							pByteR = (BYTE *)pImageDataDeepLearningR.L();
							pByteG = (BYTE *)pImageDataDeepLearningG.L();
							pByteB = (BYTE *)pImageDataDeepLearningB.L();

							register BYTE *pR, *pG, *pB;
							/*size_t dataSize = lImageWidth * lImageHeight * 3;
							std::std::vector<unsigned char> vecImageData(dataSize);*/

							for (int j = 0; j < lImageHeight; j++)
							{
								pB = pbyteImgData + j * lImageWidth * 3;
								pG = pbyteImgData + j * lImageWidth * 3 + 1;
								pR = pbyteImgData + j * lImageWidth * 3 + 2;

								for (int i = 0; i < lImageWidth; i++)
								{
									*(pB + i * 3) = *(pByteB)++;  //halcon에서 3채널 이미지 데이터 -> opencv형태(수아킷도 opencv형태)의 3채널 이미지 데이터로 변환
									*(pG + i * 3) = *(pByteG)++;
									*(pR + i * 3) = *(pByteR)++;
								}
							}

							CString strModuleNo, strTrayNo, strDefectCnt, strDeepLearningModelID, strPort, strMagazineNo, strImageCount, strModelVersion, strImageWidth, strImageHeight;
							strModuleNo.Format("%d", iModuleNo);
							strTrayNo.Format("%d", iTrayNo);
							strDefectCnt.Format("%d", m_nADJDefectCnt[nThreadNO]);
							strMagazineNo.Format("%d", iMzNo);
							strModelVersion.Format("%d", THEAPP.GetProgramVersion());
							strImageWidth.Format("%d", lImageWidth);
							strImageHeight.Format("%d", lImageHeight);

							int iSendDeepLearningModelID = 1;
							if (iDeepLearningModelID % DEEP_MODEL_NUM == 0)
								iSendDeepLearningModelID = DEEP_MODEL_NUM; //20200527 버그 수정(딥러닝모델과 같아지면 0이 되버림)
							else
								iSendDeepLearningModelID = iDeepLearningModelID % DEEP_MODEL_NUM;

							strDeepLearningModelID.Format("%d", iSendDeepLearningModelID);
							strPort.Format("%d", THEAPP.m_TCPClientService.m_arrClient[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM].GetTCPPort());

							stTCPPacket tcpPacket[MAX_ADJ_CONNECT_NUM * MAX_MULTI_CONNECTION_NUM];

							for (int i = 0; i < ConcatFilename.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteFileName[i] = ConcatFilename.GetAt(i);
								if (i + 1 == ConcatFilename.GetLength())
									tcpPacket[iClientNO].byteFileName[i + 1] = NULL;
							}

							for (int i = 0; i < THEAPP.m_TCPClientService.m_strClientIP.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteIP[i] = THEAPP.m_TCPClientService.m_strClientIP.GetAt(i);
								if (i + 1 == THEAPP.m_TCPClientService.m_strClientIP.GetLength())
									tcpPacket[iClientNO].byteIP[i + 1] = NULL;
							}

							for (int i = 0; i < strPort.GetLength(); i++)
							{
								tcpPacket[iClientNO].bytePort[i] = strPort.GetAt(i);
								if (i + 1 == strPort.GetLength())
									tcpPacket[iClientNO].bytePort[i + 1] = NULL;
							}

							for (int i = 0; i < sLotID.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteLotID[i] = sLotID.GetAt(i);
								if (i + 1 == sLotID.GetLength())
									tcpPacket[iClientNO].byteLotID[i + 1] = NULL;
							}

							for (int i = 0; i < strModuleNo.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteModuleNo[i] = strModuleNo.GetAt(i);
								if (i + 1 == strModuleNo.GetLength())
									tcpPacket[iClientNO].byteModuleNo[i + 1] = NULL;
							}

							for (int i = 0; i < strTrayNo.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteTrayNo[i] = strTrayNo.GetAt(i);
								if (i + 1 == strTrayNo.GetLength())
									tcpPacket[iClientNO].byteTrayNo[i + 1] = NULL;
							}

							for (int i = 0; i < strDefectCnt.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteDefectCnt[i] = strDefectCnt.GetAt(i);
								if (i + 1 == strDefectCnt.GetLength())
									tcpPacket[iClientNO].byteDefectCnt[i + 1] = NULL;
							}

							for (int i = 0; i < strDeepLearningModelID.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteDeepLearningModelID[i] = strDeepLearningModelID.GetAt(i);
								if (i + 1 == strDeepLearningModelID.GetLength())
									tcpPacket[iClientNO].byteDeepLearningModelID[i + 1] = NULL;
							}

							for (int i = 0; i < strMagazineNo.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteMagazineNo[i] = strMagazineNo.GetAt(i);
								if (i + 1 == strMagazineNo.GetLength())
									tcpPacket[iClientNO].byteMagazineNo[i + 1] = NULL;
							}

							for (int i = 0; i < strModelVersion.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteModelVersion[i] = strModelVersion.GetAt(i);
								if (i + 1 == strModelVersion.GetLength())
									tcpPacket[iClientNO].byteModelVersion[i + 1] = NULL;
							}

							for (int i = 0; i < strImageWidth.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteImageWidth[i] = strImageWidth.GetAt(i);
								if (i + 1 == strImageWidth.GetLength())
									tcpPacket[iClientNO].byteImageWidth[i + 1] = NULL;
							}

							for (int i = 0; i < strImageHeight.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteImageHeight[i] = strImageHeight.GetAt(i);
								if (i + 1 == strImageHeight.GetLength())
									tcpPacket[iClientNO].byteImageHeight[i + 1] = NULL;
							}

							tcpPacket[iClientNO].pByteImageData = pbyteImgData;

							stADJNetworkData *stADJdata = new stADJNetworkData();
							stADJdata->tcpPacket = tcpPacket[iClientNO];
							stADJdata->iClientNO = iClientNO;
							stADJdata->nThreadNO = nThreadNO;
							stADJdata->iModuleTotalStartTime = iModuleTotalStartTime;
							stADJdata->iCurDeepLearningModel = iCurDeepLearningModel;
							stADJdata->iPcVisionNo = iPcVisionNo;
							stADJdata->iMzNo = iMzNo;
							stADJdata->sLotID = sLotID;
							stADJdata->iTrayNo = iTrayNo;
							stADJdata->iModuleNo = iModuleNo;
							stADJdata->iDefectCnt = m_nADJDefectCnt[nThreadNO];
							stADJdata->iDeepLearningModelID = iDeepLearningModelID;
							stADJdata->iVisionCamType = iVisionCamType;
							stADJdata->sBarcodeID = sBarcodeID;
							stADJdata->time = time;
							stADJdata->strDefectCode = strDefectCode;
							stADJdata->sADJInspectionTypeCode = sADJInspectionTypeCode;
							stADJdata->sADJDefectDetail = sADJDefectDetail;
							stADJdata->testADJStartTime = testADJStartTime;
							stADJdata->HADJSaveRegion = HADJSaveRegion;
							stADJdata->HADJFilteredDefectRgn = &HADJFilteredDefectRgn[iImageNo];
							stADJdata->ConcatimageObj = ConcatimageObj;
							stADJdata->ConcatReviewImage = ConcatReviewImage;
							stADJdata->ConcatFilename = ConcatFilename;
							stADJdata->iReviewSaveCnt = &iReviewSaveCnt;
							//stADJdata->vecImageData = vecImageData;
							m_iModuleNo_ADJCnt = iModuleNo;
							m_qADJBuffer[iModuleNo - 1].push(stADJdata);

							/*cv::Mat TestSegMatImage, TestSegMaskImage, tempMat;
							TestSegMatImage.create(lImageWidth, lImageHeight, CV_8UC3);

							std::memcpy(TestSegMatImage.data, pbyteImgData, lImageWidth*lImageHeight*3);
							TestSegMaskImage = SendSegmentImagetoADJ(nThreadNO, iMzNo, iTrayNo, iModuleNo, sLotID, m_nADJDefectCnt[nThreadNO], iDeepLearningModelID, ConcatFilename, iModuleTotalStartTime, TestSegMatImage);
							cv::normalize(TestSegMaskImage, tempMat, 0, 255, cv::NORM_MINMAX, -1);
							string tempimage = THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename + "_HeatMap.jpg";
							cv::imwrite(tempimage, tempMat);*/
							//strLog.Format("%d Before, *ptrThreadCnt: %d", DefectIndex, *ptrThreadCnt);
							//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
							//250219 KDH 멀티통신 쓰레드 생성 계산식 변경
							if (THEAPP.g_arriThreadCnt[iModuleNo - 1] < (THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO * (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[0] + THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[1] + THEAPP.Struct_PreferenceStruct.m_bUseLocalADJ)) * 2) // 쓰레드 생성 개수 제한을 위한 방식
							{
								AfxBeginThread(ADJ_Network_Thread, this);
								THEAPP.g_arriThreadCnt[iModuleNo - 1] = THEAPP.g_arriThreadCnt[iModuleNo - 1] + 1;
							}
							//strLog.Format("%d After, THEAPP.g_arriThreadCnt[iModuleNo]: %d", DefectIndex, THEAPP.g_arriThreadCnt[iModuleNo]);
							//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
							m_nADJDefectCnt[nThreadNO]++; //20200912 원 위치로 원복

							Sleep(1);
							//strLog.Format("DoDeeplearningInspection 6th ADJ send image data make end");
							//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

						}
						Sleep(1);//0614추가 //이거 없으면 정상적으로 Receive가 안되고 adj와 logical한 Time out에 걸림. 이유는 모르겠음. Sleep(1)은 부족
					}	//DeepLearningKIT을 쓰지 않을 때 ADJ에 연결될 클라이언트 수만큼 동작하는 for문에 들어가지 않으므로 괄호를 제외한다.	//20180804 jwj

					try
					{
						if (THEAPP.Struct_PreferenceStruct.m_bSaveADJ)
						{
							//strLog.Format("DoDeeplearningInspection 7th ADJ image save start");
							//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
							DWORD dTStartImageSave = 0, dTEndImageSave = 0;
							CString strSaveFileName;

							if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread)
							{
								if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal && !m_bADJRequest)
								{
									dTStartImageSave = GetTickCount();

									if (m_AdjImageCount > 1)
										WriteImage(ConcatimageObj, "jpeg 100", 0, HTuple(THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename));
									else
										WriteImage(HADJSaveImage[0], "jpeg 100", 0, HTuple(THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename));

									strSaveFileName = THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename;
									dTEndImageSave = GetTickCount();
									strLog.Format("Save ADJ image, Time(ms): %d, FileName: %s", dTEndImageSave - dTStartImageSave, strSaveFileName);
									THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
								}

								if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS && !m_bADJRequest)
								{
									dTStartImageSave = GetTickCount();

									if (m_AdjImageCount > 1)
										WriteImage(ConcatimageObj, "jpeg 100", 0, HTuple(THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename));
									else
										WriteImage(HADJSaveImage[0], "jpeg 100", 0, HTuple(THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename));

									strSaveFileName = THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename;
									dTEndImageSave = GetTickCount();
									strLog.Format("Save ADJ image, Time(ms): %d, FileName: %s", dTEndImageSave - dTStartImageSave, strSaveFileName);
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
									if (m_AdjImageCount > 1)
									{
										CopyImage(ConcatimageObj, &(pSaveThreadIDParam->HSaveImage));
										pSaveThreadIDParam->sSavePath = THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename;
										THEAPP.m_pInspectService->AddListSaveADJImageParam(pSaveThreadIDParam, iPcVisionNo, iMzNo, iTrayNo, iModuleNo);
									}
									else
									{
										CopyImage(HADJSaveImage[0], &(pSaveThreadIDParam->HSaveImage));
										pSaveThreadIDParam->sSavePath = THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename;
										THEAPP.m_pInspectService->AddListSaveADJImageParam(pSaveThreadIDParam, iPcVisionNo, iMzNo, iTrayNo, iModuleNo);
									}
								}

								if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS)
								{
									ADJ_IMAGE_SAVE_PARAM* pSaveThreadIDParam_Dual = new ADJ_IMAGE_SAVE_PARAM;
									if (m_AdjImageCount > 1)
									{
										CopyImage(ConcatimageObj, &(pSaveThreadIDParam_Dual->HSaveImage));
										pSaveThreadIDParam_Dual->sSavePath = THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename;
										THEAPP.m_pInspectService->AddListSaveADJImageParam(pSaveThreadIDParam_Dual, iPcVisionNo, iMzNo, iTrayNo, iModuleNo);
									}
									else
									{
										CopyImage(HADJSaveImage[0], &(pSaveThreadIDParam_Dual->HSaveImage));
										pSaveThreadIDParam_Dual->sSavePath = THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + ConcatFilename;
										THEAPP.m_pInspectService->AddListSaveADJImageParam(pSaveThreadIDParam_Dual, iPcVisionNo, iMzNo, iTrayNo, iModuleNo);
									}
								}
							}
							//strLog.Format("DoDeeplearningInspection 7th ADJ image save end");
							//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
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

						strLog.Format("Halcon Exception [ADJClientService::DoDeeplearningInspection(WriteImage) ResultSave_N%d_ADJ] : <%s>%s", nThreadNO + 1, sOperatorName, sErrMsg);
						THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
						return iReviewSaveCnt;
					}

				}

				if (bIsTimeOut)	//ADJ 모듈 time out
					break;
			} // 불량 Index

		}  //if (THEAPP.m_pGFunction->ValidHRegion(HTotalImageDefectRgn)==TRUE)

		m_nADJDefectCnt[nThreadNO] = 0;
		int nModuleTimeOut = 0;

		if (THEAPP.Struct_PreferenceStruct.m_bUseADJTimeOut)
		{
			nModuleTimeOut = THEAPP.Struct_PreferenceStruct.m_nADJTimeOut;
		}
		else
		{
			nModuleTimeOut = ADJ_SEND_TIMEOUT;
		}


		while (GetTickCount() - iModuleTotalStartTime < nModuleTimeOut + 100)
		{
			Sleep(10);
			//strLog.Format("ptrThreadCnt: %d, THEAPP.g_arriThreadCnt[iModuleNo]: %d", ptrThreadCnt, THEAPP.g_arriThreadCnt[iModuleNo]);
			//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
			if ((m_qADJBuffer[iModuleNo - 1].IsEmpty() && THEAPP.g_arriThreadCnt[iModuleNo - 1] == 0) && THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ)		// ADJ 양품 판정
			{
				strLog.Format("DoDeeplearningInspection request end process start, Module: %d", iModuleNo);
				THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
				BOOL bIsLastGood = TRUE;
				if (THEAPP.bIsTimeOut)
				{
					THEAPP.g_arriThreadCnt[iModuleNo - 1] = 0;
					while (!m_qADJBuffer[iModuleNo - 1].IsEmpty())
						m_qADJBuffer[iModuleNo - 1].next();
				}
				THEAPP.m_TCPClientService.ResetAdjStatus();
				for (int nTabIdx = 0; nTabIdx < MAX_IMAGE_TAB; nTabIdx++)
				{
					CountObj(HTotalDefectRgn[nTabIdx], &HNoDefect);
					lNoDefectCount = HNoDefect[0].I();
					CountObj(HADJFilteredDefectRgn[nTabIdx], &HNoImageDefect);
					lNoImageDefectCount = HNoImageDefect[0].I();

					if (THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ && THEAPP.m_pGFunction->ValidHRegion(HADJFilteredDefectRgn[nTabIdx]))
					{
						//strLog.Format("DoDeeplearningInspection request ending process(1), Module: %d, HTotalDefectRgn[%d] size: %d. HADJFilteredDefectRgn[%d] size: %d", iModuleNo, nTabIdx, lNoDefectCount, nTabIdx, lNoImageDefectCount);
						//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

						Union1(HADJFilteredDefectRgn[nTabIdx], &HADJFilteredDefectRgn[nTabIdx]);
						Difference(HTotalDefectRgn[nTabIdx], HADJFilteredDefectRgn[nTabIdx], &HTotalDefectRgn[nTabIdx]);
						Difference(HVisionAllDefectRgn, HADJFilteredDefectRgn[nTabIdx], &HVisionAllDefectRgn); //0521 jwj add
						if (THEAPP.m_pGFunction->ValidHRegion(HTotalDefectRgn[nTabIdx]) == TRUE)
						{
							//strLog.Format("DoDeeplearningInspection request ending process(2), Module: %d, HTotalDefectRgn[%d] size: %d. HADJFilteredDefectRgn[%d] size: %d", iModuleNo, nTabIdx, lNoDefectCount, nTabIdx, lNoImageDefectCount);
							//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
							bIsLastGood = FALSE;
						}
					}
					else if (THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ && THEAPP.m_pGFunction->ValidHRegion(HTotalDefectRgn[nTabIdx]))
					{
						//strLog.Format("DoDeeplearningInspection request ending process(3), Module: %d, HTotalDefectRgn[%d] size: %d. HADJFilteredDefectRgn[%d] size: %d", iModuleNo, nTabIdx, lNoDefectCount, nTabIdx, lNoImageDefectCount);
						//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
						bIsLastGood = FALSE;
					}
				}

				if (bIsLastGood)
				{
					strLog.Format("DoDeeplearningInspection request end, Module: %d, G", iModuleNo);
					THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
					iInspectionTypeResult = DEFECT_TYPE_GOOD;
				}
				else
				{
					strLog.Format("DoDeeplearningInspection request end, Module: %d, NG", iModuleNo);
					THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
				}
				break;
			}
			else if (!THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ)
			{
				THEAPP.m_TCPClientService.ResetAdjStatus();
				break;
			}
		}
		if (!m_qADJBuffer[iModuleNo - 1].IsEmpty() || THEAPP.g_arriThreadCnt[iModuleNo - 1] != 0)
		{
			THEAPP.g_arriThreadCnt[iModuleNo - 1] = 0;
			while (!m_qADJBuffer[iModuleNo - 1].IsEmpty())
				m_qADJBuffer[iModuleNo - 1].next();
		}
		return iReviewSaveCnt;
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

		strLog.Format("Halcon Exception [ADJClientService::DoDeeplearningInspection] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}

int CADJClientService::DoDeeplearningRosReviewFile_FAI(int nThreadNO,
	int iMzNo,
	int iTrayNo,
	int iModuleNo,
	int iPcVisionNo,
	int iVisionCamType,
	int iNoInspectionImage,
	HObject *pHDisplayImage,
	HObject(*ppHReviewRgn_FAI_Item)[12],
	POINT(*ppiViewportCenter_FAI_Item)[12],
	CString sLotID,
	CString sBarcodeID,
	int *piNoSaveReviewImage,
	DWORD iModuleTotalStartTime)
{
	*piNoSaveReviewImage = 0;

	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];
	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(pHDisplayImage[0]) == FALSE)
		{
			strLog.Format("CADJClientService::DoDeeplearningRosReviewFile_FAI ==> Invalid Image");
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
			return 0;
		}
		//strLog.Format("DoDeeplearningRosReviewFile_FAI 1 start");
		//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
		int i, j, k;
		HObject HIntersectRgn;
		HTuple HlBlobArea, HdBlobCenterX, HdBlobCenterY;
		Hlong lBlobArea, lMaxBlobArea;

		HTuple HNoDefect;
		Hlong lNoDefect;

		int iType;
		CString sDefectResult, sDefectCodeResult;
		HObject HImgDump_save;
		HTuple HDefectName;
		HObject HConnRgn, HRosDisplayRgn, HCircleTransRgn, HSelectedRgn, HADJSaveRgn;
		HTuple HdCircleRow, HdCircleCol, HdCircleRadius;

		HalconCpp::SetDraw(THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle, "margin");
		THEAPP.m_pInspectAdminViewHideDlg1->set_display_font(THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle, ROS_IMAGE_REVIEW_FONT_SIZE, "mono", "true", "false");

		int iSelectedImage = -1;

		HTuple HArea, HCenterX, HCenterY, HLTPointX, HLTPointY, HRBPointX, HRBPointY, CompArea;
		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX, lXLength, lYLength;
		double	dArea;
		int iXlength, iYLength;
		CString strSpec;
		BOOL bAngleMeasure = FALSE;
		int iViewportCenterX, iViewportCenterY;
		int iDisplayImageIndex = -1;
		HObject HContourRgn;
		HObject HConcatImage, HCombineImage;
		int iCamImageWidth, iCamImageHeight;
		BOOL m_bADJRequest = FALSE;

		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		iCamImageWidth = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[iPcVisionNo];
		iCamImageHeight = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[iPcVisionNo];

		//HalconCpp::SetLineStyle(HideWindowHandle, (HTuple(5).Append(5)));
		HalconCpp::SetLineWidth(THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle, HTuple(5));

		HalconCpp::SetColor(THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle, "red");

		////////////////////////////////////////////////////////////////////////////

		int iNoCheckFaiParameter = MAX_FAI_PARAMETER;
		for (int iFaiNumber = 0; iFaiNumber < MAX_FAI_ITEM; iFaiNumber++)
		{
			CString strViewCenterYGroup = "";
			if (THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_bGenReviewImage == FALSE)
				continue;

			if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMInspectFai[iFaiNumber] == FALSE)
				continue;

			i = iFaiNumber;

			if (i >= 81 && i <= 90)		// Tilt CD + Max( Gap ) 
				continue;

			if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMultiFai[i] == TRUE)
				iNoCheckFaiParameter = MAX_FAI_PARAMETER;
			else
				iNoCheckFaiParameter = 1;

			//strLog.Format("DoDeeplearningRosReviewFile_FAI start, FAINumber: %d", iFaiNumber);
			//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

			for (int iFaiParamIndex = 0; iFaiParamIndex < iNoCheckFaiParameter; iFaiParamIndex++)
			{
				if (THEAPP.Struct_PreferenceStruct.m_bUseSaveFaiReviewImage == FALSE)
				{
					if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bDeepLearningResultGood[i] == FALSE)
						continue;
					if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bParamResultNG[iFaiNumber] == FALSE)
						continue;
				}
				if (iFaiNumber == 10 || iFaiNumber == 17 || iFaiNumber == 18 || iFaiNumber == 29 || iFaiNumber == 40 || iFaiNumber == 74 || iFaiNumber == 75 || iFaiNumber == 62 || iFaiNumber == 66)
					bAngleMeasure = TRUE;
				else
					bAngleMeasure = FALSE;


				GenEmptyObj(&HConcatImage);

				CString sADJTimeStamp;
				SYSTEMTIME time;
				GetLocalTime(&time);
				sADJTimeStamp.Format("%02d%02d%03d", time.wMinute, time.wSecond, time.wMilliseconds);

				for (int iReviewImageIndex = 0; iReviewImageIndex < THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iNoReviewImage; iReviewImageIndex++)
				{
					if (THEAPP.m_pGFunction->ValidHXLD(ppHReviewRgn_FAI_Item[iFaiNumber][iFaiParamIndex*MAX_FAI_REVIEW_IMAGE + iReviewImageIndex]) == FALSE)
						continue;
					if (THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iViewportCenter[iReviewImageIndex] == 1)
					{
						iViewportCenterX = ppiViewportCenter_FAI_Item[iFaiNumber][iReviewImageIndex].x;
						iViewportCenterY = ppiViewportCenter_FAI_Item[iFaiNumber][iReviewImageIndex].y;

						if (iViewportCenterX < 0)
							continue;
					}
					else
					{
						GenRegionContourXld(ppHReviewRgn_FAI_Item[iFaiNumber][iReviewImageIndex], &HContourRgn, "filled");
						Union1(HContourRgn, &HContourRgn);
						AreaCenter(HContourRgn, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
						iViewportCenterX = HdBlobCenterX[0].D();
						iViewportCenterY = HdBlobCenterY[0].D();
					}

					THEAPP.m_pInspectAdminViewHideDlg1->OverlaySetCenterViewportManager(iViewportCenterX, iViewportCenterY, THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_fZoomRatio[iReviewImageIndex]);

					if (strViewCenterYGroup == "")
					{
						strViewCenterYGroup.Format("%d", iViewportCenterY);
					}
					else
					{
						strViewCenterYGroup.Format("%s,%d", strViewCenterYGroup, iViewportCenterY);
					}

					ClearWindow(THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle);

					HalconCpp::SetColor(THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle, "red");

					iDisplayImageIndex = THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iImageNo[iReviewImageIndex] - 1;

					if (THEAPP.m_pGFunction->ValidHImage(pHDisplayImage[iDisplayImageIndex]))
						HalconCpp::DispObj(pHDisplayImage[iDisplayImageIndex], THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle);
					else
						continue;

					//HalconCpp::DispObj(ppHReviewRgn_FAI_Item[iFaiNumber][iFaiParamIndex*MAX_FAI_REVIEW_IMAGE + iReviewImageIndex], THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle);//redline

					//HalconCpp::SetColor(THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle, "green");
					//THEAPP.m_pInspectAdminViewHideDlg1->set_display_font(THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle, 80, "mono", "true", "false");
					CString sMeasureValue;

					if (iFaiNumber == 19 || iFaiNumber == 20 || iFaiNumber == 44)
					{
						if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bFAI_ResultNG[iFaiNumber][0])
						{
							sMeasureValue.Format("%.3f um", CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][0]);
						}
						else
						{
							sMeasureValue.Format("%.3f um", CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][1]);
						}
					}
					else
					{
						if (bAngleMeasure)
							sMeasureValue.Format("%.3f deg", CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][0]);
						else
							sMeasureValue.Format("%.3f um", CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][0]);

					}

					//////////////////////////////////////////////////////////////////////////

					HalconCpp::DumpWindowImage(&HImgDump_save, THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle);

					iDisplayImageIndex = THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iImageNo[iReviewImageIndex] - 1;
					if (THEAPP.m_pGFunction->ValidHImage(HImgDump_save))
					{
						//SmallestRectangle1(HImgDump_save, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX); 

						AreaCenter(HImgDump_save, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);		// add

						lLTPointY = (int)iViewportCenterY - THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2;
						lLTPointX = (int)iViewportCenterX - THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2;
						lRBPointY = (int)iViewportCenterY + THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2 - 1;
						lRBPointX = (int)iViewportCenterX + THEAPP.Struct_PreferenceStruct.m_iADJImageSize / 2 - 1;

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

						CropRectangle1(pHDisplayImage[iDisplayImageIndex], &HADJSaveRgn, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

						ConcatObj(HConcatImage, HADJSaveRgn, &HConcatImage);
					}

					ClearWindow(THEAPP.m_pInspectAdminViewHideDlg1->HideWindowHandle);

					Sleep(10);

				}	// for (int iReviewImageIndex = 0; iReviewImageIndex < THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iNoReviewImage; iReviewImageIndex++)

				// Save Review Image

				int iGroupNo = 1;

				if (THEAPP.m_pGFunction->ValidHImage(HConcatImage))
				{
					TileImages(HConcatImage, &HCombineImage, NO_X_FAI_REVIEW_IMAGE, "horizontal");

					if (iFaiNumber == 5 || iFaiNumber == 9 || iFaiNumber == 10 || iFaiNumber == 60 || iFaiNumber == 61 || iFaiNumber == 62 || iFaiNumber == 24 ||
						iFaiNumber == 29 || iFaiNumber == 31 || iFaiNumber == 34 || iFaiNumber == 66 || iFaiNumber == 65 || iFaiNumber == 64 || iFaiNumber == 67)
						iGroupNo = 1;
					else if (iFaiNumber == 11 || iFaiNumber == 13 || iFaiNumber == 14 || iFaiNumber == 26 || iFaiNumber == 27 || iFaiNumber == 32 || iFaiNumber == 33)
						iGroupNo = 2;
					else if (iFaiNumber == 19 || iFaiNumber == 20 || iFaiNumber == 21 || iFaiNumber == 22 || iFaiNumber == 44)
						iGroupNo = 3;

					if (iFaiNumber == 19 || iFaiNumber == 20 || iFaiNumber == 44)
					{
						if (THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_bInspectType == FALSE)
							strSpec.Format("F-%s", THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_sFaiName);
						else
							strSpec.Format("D-%s", THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_sFaiName);

						strSpec.Format("%s-R%d", strSpec, iFaiParamIndex + 1);

						if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bParamResultNG[iFaiNumber] == TRUE)
						{
							strSpec.Format("%s-NG-%d", strSpec, iGroupNo);
						}
						else
						{
							strSpec.Format("%s-OK-%d", strSpec, iGroupNo);
						}

						if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bFAI_ResultNG[iFaiNumber][0])
						{
							if (bAngleMeasure)
								strSpec.Format("%s_A(%.3lf)", strSpec, CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][0]);
							else
								strSpec.Format("%s_L(%.3lf)", strSpec, CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][0]);
						}
						else
						{
							if (bAngleMeasure)
								strSpec.Format("%s_A(%.3lf)", strSpec, CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][1]);
							else
								strSpec.Format("%s_L(%.3lf)", strSpec, CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][1]);
						}
					}
					else
					{
						if (THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_bInspectType == FALSE)
							strSpec.Format("F-%s", THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_sFaiName);
						else
							strSpec.Format("D-%s", THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_sFaiName);

						strSpec.Format("%s-R%d", strSpec, iFaiParamIndex + 1);

						if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bParamResultNG[iFaiNumber] == TRUE)
						{
							strSpec.Format("%s-NG-%d", strSpec, iGroupNo);
						}
						else
						{
							strSpec.Format("%s-OK-%d", strSpec, iGroupNo);
						}

						if (bAngleMeasure)
							strSpec.Format("%s_A(%.3lf)", strSpec, CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][0]);
						else
							strSpec.Format("%s_L(%.3lf)", strSpec, CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][0]);
					}
					CString Filename;
					Filename.Format("M%02d_%s_FAI_%d_%s_%s_Image00", iModuleNo, sVisionCamType_Short, iFaiNumber, strViewCenterYGroup, sADJTimeStamp);

					Filename.Format("%s_%s_%s", Filename, sBarcodeID, strSpec);
					CString strTimestamp;

					//SYSTEMTIME time;
					GetLocalTime(&time);
					strTimestamp.Format("%02d%02d%02d%03d", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

					int iCurDeepLearningModel, iDeepLearningModelID, iClientNO;
					iClientNO = 0;

					iCurDeepLearningModel = iDeepLearningModelID = AssignDeepLearningFAIModel(i);	//ADJ로 보낼 모델인지 아닌지 판별과 동시에 결정. //주의사항 : iCLientNO에 따른 모델 배정은 겹치면 안됌.

					if ((THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[0] + THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[1]) != 0)
						iClientNO = (iTrayNo + iModuleNo + m_nADJDefectCnt[nThreadNO]) % THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO;

					if (iDeepLearningModelID > DEEP_MODEL_NUM)
						iClientNO += MAX_MULTI_CONNECTION_NUM;

					BYTE* pbyteImgData;

					if (THEAPP.m_TCPClientService.m_bConnect[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM] &&
						THEAPP.Struct_PreferenceStruct.m_bIsSendADJ &&
						THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO / MAX_MULTI_CONNECTION_NUM] &&
						THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMultiFai[i] == TRUE)
					{
						if (iDeepLearningModelID != 0)
						{
							strLog.Format("DoDeeplearningRosReviewFile_FAI start, FAINumber: %d, iFaiParamIndex: %d, iNoCheckFaiParameter: %d", iFaiNumber, iFaiParamIndex, iNoCheckFaiParameter);
							THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

							m_bADJRequest = TRUE;	//ADJ이미지 의뢰 시 저장 X

							int testADJStartTime = GetTickCount();

							HTuple pImageDataDeepLearningR;
							HTuple pImageDataDeepLearningG;
							HTuple pImageDataDeepLearningB;
							HTuple type;
							HTuple HImageHeight;
							HTuple HImageWidth;

							pImageDataDeepLearningR = 0;
							pImageDataDeepLearningG = 0;
							pImageDataDeepLearningB = 0;
							type = 0;
							HImageHeight = 0;
							HImageWidth = 0;

							HalconCpp::GetImagePointer3(HCombineImage, &pImageDataDeepLearningR, &pImageDataDeepLearningG, &pImageDataDeepLearningB, &type, &HImageWidth, &HImageHeight);

							/*if (HImageWidth.L() != THEAPP.Struct_PreferenceStruct.m_iADJImageSize || HImageHeight.L() != THEAPP.Struct_PreferenceStruct.m_iADJImageSize)
							{
								strLog.Format("ADJ%d input height or width is invalid value, image size: %d, %d", nThreadNO + 1, HImageWidth.L(), HImageHeight.L());
								THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

								bBreak = TRUE;
								continue;
							}*/

							pbyteImgData = NULL;
							//pbyteImgData = (BYTE*)malloc(sizeof(BYTE)*(HImageWidth.L()*HImageHeight.L() * 3));
							pbyteImgData = (BYTE*)malloc(sizeof(BYTE)*(512 * 512 * 3));
							if (pbyteImgData == NULL)
							{
								strLog.Format("DoDeeplearningInspection pbyteImgData is Null");
								THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
								return 0;
							}
							memset(pbyteImgData, 0, sizeof(BYTE)*(512 * 512 * 3));

							BYTE* pByteR;
							BYTE* pByteG;
							BYTE* pByteB;
							Hlong lImageWidth, lImageHeight;
							lImageWidth = HImageWidth.L();
							lImageHeight = HImageHeight.L();
							pByteR = (BYTE *)pImageDataDeepLearningR.L();
							pByteG = (BYTE *)pImageDataDeepLearningG.L();
							pByteB = (BYTE *)pImageDataDeepLearningB.L();

							register BYTE *pR, *pG, *pB;

							for (int j = 0; j < lImageHeight; j++)
							{
								pB = pbyteImgData + j * lImageWidth * 3;
								pG = pbyteImgData + j * lImageWidth * 3 + 1;
								pR = pbyteImgData + j * lImageWidth * 3 + 2;

								for (int i = 0; i < lImageWidth; i++)
								{
									*(pB + i * 3) = *(pByteB)++;  //halcon에서 3채널 이미지 데이터 -> opencv형태(수아킷도 opencv형태)의 3채널 이미지 데이터로 변환
									*(pG + i * 3) = *(pByteG)++;
									*(pR + i * 3) = *(pByteR)++;
								}
							}


							CString strModuleNo, strTrayNo, strDefectCnt, strDeepLearningModelID, strPort, strMagazineNo, strImageCount, strModelVersion, strImageWidth, strImageHeight;
							strModuleNo.Format("%d", iModuleNo);
							strTrayNo.Format("%d", iTrayNo);
							strDefectCnt.Format("%d", m_nADJDefectCnt[nThreadNO]);
							strMagazineNo.Format("%d", iMzNo);
							strModelVersion.Format("%d", THEAPP.GetProgramVersion());
							//strImageWidth.Format("%d", lImageWidth);
							//strImageHeight.Format("%d", lImageHeight);
							strImageWidth.Format("%d", 512);
							strImageHeight.Format("%d", 512);

							int iSendDeepLearningModelID = 1;
							if (iDeepLearningModelID % DEEP_MODEL_NUM == 0)
								iSendDeepLearningModelID = DEEP_MODEL_NUM; //20200527 버그 수정(딥러닝모델과 같아지면 0이 되버림)
							else
								iSendDeepLearningModelID = iDeepLearningModelID % DEEP_MODEL_NUM;

							strDeepLearningModelID.Format("%d", iSendDeepLearningModelID);
							strPort.Format("%d", THEAPP.m_TCPClientService.m_arrClient[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM].GetTCPPort());

							stTCPPacket tcpPacket[MAX_ADJ_CONNECT_NUM * MAX_MULTI_CONNECTION_NUM];

							CString strTempADJLog;

							for (int i = 0; i < Filename.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteFileName[i] = Filename.GetAt(i);
								if (i + 1 == Filename.GetLength())
									tcpPacket[iClientNO].byteFileName[i + 1] = NULL;
							}

							for (int i = 0; i < THEAPP.m_TCPClientService.m_strClientIP.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteIP[i] = THEAPP.m_TCPClientService.m_strClientIP.GetAt(i);
								if (i + 1 == THEAPP.m_TCPClientService.m_strClientIP.GetLength())
									tcpPacket[iClientNO].byteIP[i + 1] = NULL;
							}

							for (int i = 0; i < strPort.GetLength(); i++)
							{
								tcpPacket[iClientNO].bytePort[i] = strPort.GetAt(i);
								if (i + 1 == strPort.GetLength())
									tcpPacket[iClientNO].bytePort[i + 1] = NULL;
							}

							for (int i = 0; i < sLotID.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteLotID[i] = sLotID.GetAt(i);
								if (i + 1 == sLotID.GetLength())
									tcpPacket[iClientNO].byteLotID[i + 1] = NULL;
							}

							for (int i = 0; i < strModuleNo.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteModuleNo[i] = strModuleNo.GetAt(i);
								if (i + 1 == strModuleNo.GetLength())
									tcpPacket[iClientNO].byteModuleNo[i + 1] = NULL;
							}

							for (int i = 0; i < strTrayNo.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteTrayNo[i] = strTrayNo.GetAt(i);
								if (i + 1 == strTrayNo.GetLength())
									tcpPacket[iClientNO].byteTrayNo[i + 1] = NULL;
							}

							for (int i = 0; i < strDefectCnt.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteDefectCnt[i] = strDefectCnt.GetAt(i);
								if (i + 1 == strDefectCnt.GetLength())
									tcpPacket[iClientNO].byteDefectCnt[i + 1] = NULL;
							}

							for (int i = 0; i < strDeepLearningModelID.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteDeepLearningModelID[i] = strDeepLearningModelID.GetAt(i);
								if (i + 1 == strDeepLearningModelID.GetLength())
									tcpPacket[iClientNO].byteDeepLearningModelID[i + 1] = NULL;
							}

							for (int i = 0; i < strMagazineNo.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteMagazineNo[i] = strMagazineNo.GetAt(i);
								if (i + 1 == strMagazineNo.GetLength())
									tcpPacket[iClientNO].byteMagazineNo[i + 1] = NULL;
							}

							for (int i = 0; i < strModelVersion.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteModelVersion[i] = strModelVersion.GetAt(i);
								if (i + 1 == strModelVersion.GetLength())
									tcpPacket[iClientNO].byteModelVersion[i + 1] = NULL;
							}

							for (int i = 0; i < strImageWidth.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteImageWidth[i] = strImageWidth.GetAt(i);
								if (i + 1 == strImageWidth.GetLength())
									tcpPacket[iClientNO].byteImageWidth[i + 1] = NULL;
							}

							for (int i = 0; i < strImageHeight.GetLength(); i++)
							{
								tcpPacket[iClientNO].byteImageHeight[i] = strImageHeight.GetAt(i);
								if (i + 1 == strImageHeight.GetLength())
									tcpPacket[iClientNO].byteImageHeight[i + 1] = NULL;
							}

							tcpPacket[iClientNO].pByteImageData = pbyteImgData;

							stFAIADJNetworkData *stADJdata = new stFAIADJNetworkData();
							stADJdata->tcpPacket = tcpPacket[iClientNO];
							stADJdata->iClientNO = iClientNO;
							stADJdata->nThreadNO = nThreadNO;
							stADJdata->iModuleTotalStartTime = iModuleTotalStartTime;
							stADJdata->iPcVisionNo = iPcVisionNo;
							stADJdata->iMzNo = iMzNo;
							stADJdata->sLotID = sLotID;
							stADJdata->iTrayNo = iTrayNo;
							stADJdata->iModuleNo = iModuleNo;
							stADJdata->iDefectCnt = m_nADJDefectCnt[nThreadNO];
							stADJdata->iDeepLearningModelID = iDeepLearningModelID;
							stADJdata->sBarcodeID = sBarcodeID;
							stADJdata->time = time;
							stADJdata->testADJStartTime = testADJStartTime;
							stADJdata->iReviewSaveCnt = piNoSaveReviewImage;
							stADJdata->ConcatFilename = Filename;
							stADJdata->iFAI_Number = iFaiNumber;
							stADJdata->HADJSaveRegion = HADJSaveRgn;
							stADJdata->iFaiParamIndex = iFaiParamIndex;
							m_iModuleNo_ADJCnt = iModuleNo;
							m_qFAIADJBuffer[iModuleNo - 1].push(stADJdata);

							//strLog.Format("%d Before, THEAPP.g_arriThreadCnt[iModuleNo-1]: %d", i, THEAPP.g_arriThreadCnt[iModuleNo - 1]);
							//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
							if (THEAPP.g_arriThreadCnt[iModuleNo - 1] < THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO * (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[0] + THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[1]) * 2) // 쓰레드 생성 개수 제한을 위한 방식
							{
								AfxBeginThread(FAI_ADJ_Network_Thread, this);
								THEAPP.g_arriThreadCnt[iModuleNo - 1] = THEAPP.g_arriThreadCnt[iModuleNo - 1] + 1;
							}
							//strLog.Format("%d After, THEAPP.g_arriThreadCnt[iModuleNo]: %d", i, THEAPP.g_arriThreadCnt[iModuleNo - 1]);
							//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
							m_nADJDefectCnt[nThreadNO]++; //20200912 원 위치로 원복

							Sleep(1);
						}
					}

					CString	strFAIPath;
					CString strSaveFileName;
					//strLog.Format("DoDeeplearningRosReviewFile_FAI 3 image save start");
					//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
					if (!THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread)
					{
						if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLocal) // && !m_bADJRequest
						{
							if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1).m_iFAIResultCase[iFaiNumber] == FAI_RESULT_CASE_MIXED &&
								CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1).m_bDoROS == TRUE)
							{
								WriteImage(HCombineImage, "jpeg 100", 0, HTuple(THEAPP.m_FileBase.m_strReviewImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename));
								strSaveFileName = THEAPP.m_FileBase.m_strReviewImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename + ".jpg";
								THEAPP.m_FileBase.ValidateFileWriting(strSaveFileName);

								++(*piNoSaveReviewImage);
							}

							strFAIPath.Format("%s\\%s", THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1], THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_sFaiName);
							THEAPP.m_FileBase.CreateFolder(strFAIPath);

							WriteImage(HCombineImage, "jpeg 100", 0, HTuple(strFAIPath + "\\" + Filename));
							strLog.Format("DoDeeplearningRosReviewFile_FAI image save, FileName: %s", Filename);
							THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
							//strSaveFileName = THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename + ".jpg";
							//THEAPP.m_FileBase.ValidateFileWriting(strSaveFileName);
						}

						if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS)
						{
							if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1).m_iFAIResultCase[iFaiNumber] == FAI_RESULT_CASE_MIXED &&
								CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1).m_bDoROS == TRUE)
							{
								WriteImage(HCombineImage, "jpeg 100", 0, HTuple(THEAPP.m_FileBase.m_strReviewImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename));
								strSaveFileName = THEAPP.m_FileBase.m_strReviewImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename + ".jpg";
								THEAPP.m_FileBase.ValidateFileWriting(strSaveFileName);

								++(*piNoSaveReviewImage);
							}

							strFAIPath.Format("%s\\%s", THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1], THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_sFaiName);
							THEAPP.m_FileBase.CreateFolder(strFAIPath);

							WriteImage(HCombineImage, "jpeg 100", 0, HTuple(strFAIPath + "\\" + Filename));
							strLog.Format("DoDeeplearningRosReviewFile_FAI image save, FileName: %s", Filename);
							THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
							//strSaveFileName = THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename + ".jpg";
							//THEAPP.m_FileBase.ValidateFileWriting(strSaveFileName);
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
							if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1).m_iFAIResultCase[iFaiNumber] == FAI_RESULT_CASE_MIXED &&
								CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1).m_bDoROS == TRUE)
							{
								REVIEW_IMAGE_SAVE_PARAM* pSaveThreadIDParam = new REVIEW_IMAGE_SAVE_PARAM;
								CopyImage(HCombineImage, &(pSaveThreadIDParam->HSaveImage));
								pSaveThreadIDParam->sSavePath = THEAPP.m_FileBase.m_strReviewImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename;
								THEAPP.m_pInspectService->AddListSaveReviewImageParam(pSaveThreadIDParam, VISION_NUMBER_1, iMzNo, iTrayNo, iModuleNo);

								++(*piNoSaveReviewImage);
							}

							FAI_IMAGE_SAVE_PARAM* pSaveThreadIDParam2 = new FAI_IMAGE_SAVE_PARAM;
							CopyImage(HCombineImage, &(pSaveThreadIDParam2->HSaveImage));
							pSaveThreadIDParam2->sSavePath = THEAPP.m_FileBase.m_strADJImageFolder_Local[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename;
							THEAPP.m_pInspectService->AddListSaveFAIImageParam(pSaveThreadIDParam2, VISION_NUMBER_1, iMzNo, iTrayNo, iModuleNo);
						}

						if (THEAPP.Struct_PreferenceStruct.m_bADJImageSaveLAS)
						{
							if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1).m_iFAIResultCase[iFaiNumber] == FAI_RESULT_CASE_MIXED &&
								CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1).m_bDoROS == TRUE)
							{
								REVIEW_IMAGE_SAVE_PARAM* pSaveThreadIDParam_Dual = new REVIEW_IMAGE_SAVE_PARAM;
								CopyImage(HCombineImage, &(pSaveThreadIDParam_Dual->HSaveImage));
								pSaveThreadIDParam_Dual->sSavePath = THEAPP.m_FileBase.m_strReviewImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename;
								THEAPP.m_pInspectService->AddListSaveReviewImageParam(pSaveThreadIDParam_Dual, VISION_NUMBER_1, iMzNo, iTrayNo, iModuleNo);

								++(*piNoSaveReviewImage);
							}

							FAI_IMAGE_SAVE_PARAM* pSaveThreadIDParam_Dual2 = new FAI_IMAGE_SAVE_PARAM;
							CopyImage(HCombineImage, &(pSaveThreadIDParam_Dual2->HSaveImage));
							pSaveThreadIDParam_Dual2->sSavePath = THEAPP.m_FileBase.m_strADJImageFolder_LAS[iPcVisionNo][iMzNo - 1][iTrayNo - 1] + "\\" + Filename;
							THEAPP.m_pInspectService->AddListSaveFAIImageParam(pSaveThreadIDParam_Dual2, VISION_NUMBER_1, iMzNo, iTrayNo, iModuleNo);
						}
					}	// Image Save

					//strLog.Format("DoDeeplearningRosReviewFile_FAI 5 image save end");
					//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

				}// if (THEAPP.m_pGFunction->ValidHImage(HConcatImage))

			}// for (int iFaiParamIndex = 0; iFaiParamIndex < MAX_FAI_PARAMETER; iFaiParamIndex++)

		}


		int nModuleTimeOut = 0;

		if (THEAPP.Struct_PreferenceStruct.m_bUseADJTimeOut)
		{
			nModuleTimeOut = THEAPP.Struct_PreferenceStruct.m_nADJTimeOut;
		}
		else
		{
			nModuleTimeOut = ADJ_SEND_TIMEOUT;
		}

		while (m_bADJRequest || GetTickCount() - iModuleTotalStartTime < nModuleTimeOut + 100)
		{
			Sleep(10);
			//strLog.Format("ptrThreadCnt: %d, THEAPP.g_arriThreadCnt[iModuleNo]: %d", ptrThreadCnt, THEAPP.g_arriThreadCnt[iModuleNo]);
			//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
			if ((m_qFAIADJBuffer[iModuleNo - 1].IsEmpty() && THEAPP.g_arriThreadCnt[iModuleNo - 1] == 0) && THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ)		// ADJ 양품 판정
			{
				strLog.Format("DoDeeplearningInspectionFAI request end");
				THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

				THEAPP.m_TCPClientService.ResetFAIAdjStatus();
				BOOL bDeepLearningResult = TRUE;
				for (int iFaiNumber = 0; iFaiNumber < MAX_FAI_ITEM; iFaiNumber++)
				{
					for (int iFaiParamIndex = 0; iFaiParamIndex < MAX_FAI_PARAMETER; iFaiParamIndex++)
					{
						if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bParamResultNG[iFaiNumber] != FALSE
							&& CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bInspectFail[iPcVisionNo] != FALSE
							&& THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ)
							bDeepLearningResult *= FALSE;
					}
				}
				break;
			}
			else if (!THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ)
			{
				THEAPP.m_TCPClientService.ResetFAIAdjStatus();
				break;
			}
		}
		if (!m_qFAIADJBuffer[iModuleNo - 1].IsEmpty() || THEAPP.g_arriThreadCnt[iModuleNo - 1] != 0)
		{
			THEAPP.g_arriThreadCnt[iModuleNo - 1] = 0;
			while (!m_qADJBuffer[iModuleNo - 1].IsEmpty())
				m_qADJBuffer[iModuleNo - 1].next();
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

		strLog.Format("Halcon Exception [ADJClientService::DoDeeplearningRosReviewFile_FAI] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return -1;
	}
}


int CADJClientService::m_DeepLearningProcessing(int iClientNo,
	int iTrayNo,
	int iModuleNo,
	CString sLotID,
	CString sBarcodeID,
	CString sFilename,
	CString modelname,
	std::vector<unsigned char> DLImg)
{
	CString cstrLog, log;
	bool camEnable = false;
	bool enable_logits = false;
	bool enable_features = false;
	float focal_weight = 0.0;
	int batch_size = 1;
	int judge, judge_list;
	float score;
	BOOL m_bIsConnect = FALSE;
	// 판정 실행
	//KDH

	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	//cv::Mat image = DLImg;
	std::string defectName = modelname;

	// Winsock 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // 에러 처리 추가
		strLog.Format("WSAStartup failed: %s", WSAGetLastError());
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		return -1; // 또는 적절한 에러 코드 반환
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	CString serverPortCString, ADJServerIp;
	serverPortCString.Format("%d", 27015);
	ADJServerIp = THEAPP.Struct_PreferenceStruct.m_strCurrentADJ_IP;

	const char* serverIpChar = (LPCTSTR)THEAPP.Struct_PreferenceStruct.m_strADJ_IP[0];
	const char* serverPortChar = (LPCTSTR)serverPortCString;

	// 서버 주소와 포트 연결
	if (getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result) != 0) { // 에러 처리 추가
		strLog.Format("getaddrinfo failed: %s", WSAGetLastError());
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		WSACleanup();
		return -1;
	}
	// 연결 시도
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) continue;
		if (connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
			closesocket(ConnectSocket); ConnectSocket = INVALID_SOCKET; continue;
		}
		m_bIsConnect = TRUE;
		break; // 연결 성공
	}
	freeaddrinfo(result);

	if (!m_bIsConnect)
	{
		return -1;
	}
	
	ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET || connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		strLog.Format("Unable to connect to server");
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		return -1;
	}
	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		strLog.Format("Unable to connect to server");
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		WSACleanup();
		m_bIsConnect = FALSE;
		return -1;
	}

	// 4. 소켓 타임아웃 설정 (중요!)
	DWORD timeout_ms = 1000; // 1초 타임아웃
	setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));
	setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));

	// 250219 KDH Local ADJ 멀티통신 위한 requestID 값 추가 수정 START
	// 이미지와 불량명 전송
	Request req;
	req.request_id = generate_request_id();
	req.tray_no = iTrayNo;
	req.module_no = iModuleNo;
	req.lot_id = sLotID;
	req.barcode = sBarcodeID;
	req.filename = sFilename;
	req.defect_name = defectName;
	req.image_data = DLImg;

	if (!send_request(ConnectSocket, req) && m_bIsConnect) {
		log.Format(_T("send_request failed. Error: %d"), WSAGetLastError());
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(log));
		closesocket(ConnectSocket);
		WSACleanup();
		m_bIsConnect = FALSE;
		return -1;
	}
	// 검사 결과 수신
	Response resp = receive_response(ConnectSocket);
	// 250219 KDH Local ADJ 멀티통신 위한 requestID 값 추가 수정 END

	if (resp.request_id.find("error") != std::string::npos && m_bIsConnect) {
		log.Format(_T("receive_response failed. Detail: %s"), resp.request_id.c_str());
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(log));
		closesocket(ConnectSocket);
		WSACleanup();
		m_bIsConnect = FALSE;
		return -1;
	}

	// 수신된 ID가 보낸 ID와 일치하는지 확인 (비동기 처리 시 중요)
	if (req.request_id != resp.request_id  && m_bIsConnect) {
		m_bIsConnect = FALSE;
		log.Format(_T("Warning: Mismatched request/response ID. Sent: %s, Rcvd: %s"), CA2T(req.request_id.c_str()), CA2T(resp.request_id.c_str()));
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(log));
	}

	shutdown(ConnectSocket, SD_BOTH);
	closesocket(ConnectSocket);
	WSACleanup();

	std::string letter; // 문자를 저장할 변수
	float number = resp.result_float;
	//KDH

	if (resp.result_char == 'G')
	{
		judge_list = 0;
		score = number;
	}
	else
	{
		judge_list = 1;
		score = number;
	}


	return judge_list;
}

UINT CADJClientService::ADJ_Network_Thread(LPVOID lparam)
{
	CString strLog;
	CADJClientService *ADJClientService = (CADJClientService*)lparam;
	int m_iModuleNo_ADJCnt = ADJClientService->m_iModuleNo_ADJCnt - 1;
	int result = 0;
	//strLog.Format("1: m_iModuleNo_ADJCnt: %d", m_iModuleNo_ADJCnt);
	//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
	while (!ADJClientService->m_qADJBuffer[m_iModuleNo_ADJCnt].IsEmpty())
	{
		stADJNetworkData* newADJdata = NULL;

		newADJdata = ADJClientService->m_qADJBuffer[m_iModuleNo_ADJCnt].next();

		if (newADJdata == nullptr)
			continue;

		stTCPPacket tcpPacket = newADJdata->tcpPacket;
		int iClientNO = newADJdata->iClientNO;
		int nThreadNO = newADJdata->nThreadNO;
		DWORD iModuleTotalStartTime = newADJdata->iModuleTotalStartTime;
		int iMzNo = newADJdata->iMzNo;
		CString sLotID = newADJdata->sLotID;
		int iPcVisionNo = newADJdata->iPcVisionNo;
		int iTrayNo = newADJdata->iTrayNo;
		int iModuleNo = newADJdata->iModuleNo;
		int iDefectCnt = newADJdata->iDefectCnt;
		int iDeepLearningModelID = newADJdata->iDeepLearningModelID;
		int iVisionCamType = newADJdata->iVisionCamType;
		int iCurDeepLearningModel = newADJdata->iCurDeepLearningModel;
		int testADJStartTime = newADJdata->testADJStartTime;
		int &iReviewSaveCnt = *newADJdata->iReviewSaveCnt;
		CString sBarcodeID = newADJdata->sBarcodeID;
		SYSTEMTIME time = newADJdata->time;
		CString ConcatFilename = newADJdata->ConcatFilename;
		CString strDefectCode = newADJdata->strDefectCode;
		CString sADJInspectionTypeCode = newADJdata->sADJInspectionTypeCode;
		CString sADJDefectDetail = newADJdata->sADJDefectDetail;
		HObject ConcatimageObj = newADJdata->ConcatimageObj;
		HObject &HADJSaveRegion = newADJdata->HADJSaveRegion;
		HObject *HADJFilteredDefectRgn = newADJdata->HADJFilteredDefectRgn;
		CString ConcatReviewImage = newADJdata->ConcatReviewImage;
		//std::vector<unsigned char> vecImageData;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		int startT = GetTickCount();

		EnterCriticalSection(&THEAPP.m_csSendADJ[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM]);

		strLog.Format("ADJ send thread #%d start", iClientNO);
		THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

		if (THEAPP.Struct_PreferenceStruct.m_bUseLocalADJ)
		{
			size_t dataSize = atoi((const char*)tcpPacket.byteImageHeight) * atoi((const char*)tcpPacket.byteImageWidth) * 3;
			std::vector<unsigned char> vecImageData(dataSize);
			memcpy(vecImageData.data(), tcpPacket.pByteImageData, dataSize);
			/*
			size_t firstUnderscorePos = ConcatFilename.Find(_T('_'));
			firstUnderscorePos = ConcatFilename.Find(_T('_'), firstUnderscorePos + 1);
			firstUnderscorePos++;
			size_t secondUnderscorePos = firstUnderscorePos;
			for (int i = 0; i < 3; i++)
			{
				secondUnderscorePos = ConcatFilename.Find(_T('_'), secondUnderscorePos + 1);
			}
			std::string strVisionDefectCode = ConcatFilename.Mid(firstUnderscorePos, secondUnderscorePos - firstUnderscorePos);
			*/
			if (vecImageData.empty())
			{
				strLog.Format("DoDeeplearningInspection pbyteImgData is Null");
				THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
				continue;
			}
			result = ADJClientService->m_DeepLearningProcessing(iClientNO, iTrayNo, iModuleNo, sLotID, sBarcodeID, ConcatFilename, strDefectCode, vecImageData);
		}
		else
			result = ADJClientService->SendImagetoADJ(tcpPacket, iClientNO, nThreadNO, iModuleTotalStartTime);

		Sleep(1);
		LeaveCriticalSection(&THEAPP.m_csSendADJ[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM]);

		int elapsedTime = GetTickCount() - startT;

		int m_nADJDefectCnt = atoi((const char*)tcpPacket.byteDefectCnt);

		EnterCriticalSection(&THEAPP.m_csADJResultLog);

		if (m_nADJDefectCnt == 0)
		{
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] = _T("");
			THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] = -1;
			for (int i = 0; i < MAX_MODEL_NUM; i++)
				THEAPP.iADJModelResult[nThreadNO][i][iModuleNo - 1] = -1;
		}

		THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += ConcatFilename + '/';

		int iADJLogingStartTime = GetTickCount();
		CString strResult;
		if (result == 0)
		{
			try
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HADJSaveRegion) && THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ)
					ConcatObj(*HADJFilteredDefectRgn, HADJSaveRegion, HADJFilteredDefectRgn);
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

				strLog.Format("Halcon Exception [ADJClientService::ADJ_Network_Thread] : <%s>%s", sOperatorName, sErrMsg);
				THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
			}

			strResult = "G";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "G/";
			THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] = abs(THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1]);
			THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] = THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] * THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1];

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else if (result == 1)
		{
			//// 250219 KDH SynapseNet (OCC + CLS ) NG 판정 시 Mavin 의뢰를 위한 추가 작업
			//if (THEAPP.Struct_PreferenceStruct.m_bUseLocalADJ && THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO / MAX_MULTI_CONNECTION_NUM])
			//{
			//	cv::Mat cv_segment_result_mask = ADJClientService->SendSegmentImagetoADJ(tcpPacket, iClientNO, nThreadNO, iModuleTotalStartTime);

			//	/*if (!cv_segment_result_mask.empty()) {
			//		cv::Mat cv_temp;
			//		cv::normalize(cv_segment_result_mask, cv_temp, 0, 255, cv::NORM_MINMAX, -1);
			//		string temp = "D:\\TEMP\\" + ConcatFilename + ".jpg";
			//		cv::imwrite(temp, cv_temp);
			//	}*/
			//}

			if (result == 1)
			{
				strResult = "NG";
				THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "NG/";
				THEAPP.strADJDefectCode[nThreadNO][iModuleNo - 1] = strDefectCode;

				if (THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] != 2 && THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] != 3)
				{
					THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] *= 0;
					THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] *= 0;
				}

				GenEmptyObj(&newADJdata->HADJSaveRegion);
			}
			// 250219 KDH End
		}
		else if (result == 2)
		{
			strResult = "R";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "Repair/";
			if (THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] != 3)
			{
				THEAPP.strADJDefectCode[nThreadNO][iModuleNo - 1] = strDefectCode;
				THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] = 2;
				THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] = 2;
			}

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else if (result == 3)
		{
			strResult = "X";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "SPNG/";
			THEAPP.strADJDefectCode[nThreadNO][iModuleNo - 1] = strDefectCode;
			THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] = 3;
			THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] = 3;

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else if (result == RCV_TIMEOUT)
		{
			strResult = "TIME_OUT";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "TO/";
			THEAPP.strADJDefectCode[nThreadNO][iModuleNo - 1] = strDefectCode;
			THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] *= 0;
			THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] *= 0;
			THEAPP.bIsTimeOut = TRUE;

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else if (result == RCV_WAIT)
		{
			strResult = "WAIT_error";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "W/";
			THEAPP.strADJDefectCode[nThreadNO][iModuleNo - 1] = strDefectCode;
			THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] *= 0;
			THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] *= 0;
			THEAPP.bIsTimeOut = TRUE;

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else if (result == RCV_ERROR)
		{
			strResult = "TCP_error";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "E/";
			THEAPP.strADJDefectCode[nThreadNO][iModuleNo - 1] = strDefectCode;
			THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] *= 0;
			THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] *= 0;
			THEAPP.bIsTimeOut = TRUE;

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else
		{
			strResult = "UNKNOWN";
			CString strTemp;
			strTemp.Format("value:%d/", result);
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += strTemp;
			THEAPP.strADJDefectCode[nThreadNO][iModuleNo - 1] = strDefectCode;
			THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] *= 0;
			THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] *= 0;
			THEAPP.bIsTimeOut = TRUE;

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}

		int iADJLogingFLAGENDTime = GetTickCount() - iADJLogingStartTime;

		//20200716 jwj start
		CString strNewAdjLog;
		SYSTEMTIME timeAdj;
		GetLocalTime(&timeAdj);

		strNewAdjLog.Format("%04d-%02d-%02d\t%04d-%02d-%02d %02d:%02d:%02d\t%s\t%s\t%s\t%s\t%d\t%d\t%s\t%s\t%s",
			timeAdj.wYear, timeAdj.wMonth, timeAdj.wDay,
			timeAdj.wYear, timeAdj.wMonth, timeAdj.wDay,
			time.wHour, time.wMinute, time.wSecond,
			THEAPP.GetPCID(),
			THEAPP.Struct_PreferenceStruct.m_strEquipNo,
			sLotID,
			THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName,
			iTrayNo,
			iModuleNo,
			sBarcodeID,
			ADJClientService->GetDeepLearningModelNameFromIndex(iDeepLearningModelID, iVisionCamType, sADJInspectionTypeCode, sADJDefectDetail),
			ConcatFilename
		);

		if (result == 1)
		{
			strNewAdjLog += "\t" + ADJClientService->GetDeepLearningModelNameFromIndex(iDeepLearningModelID, iVisionCamType, sADJInspectionTypeCode, sADJDefectDetail);
			GenEmptyObj(&newADJdata->ConcatimageObj);
			SAFE_DELETE(newADJdata);
		}
		else if (result != 0)
		{
			strNewAdjLog += "\t" + ADJClientService->GetDeepLearningModelNameFromIndex(iDeepLearningModelID, iVisionCamType, sADJInspectionTypeCode, sADJDefectDetail);
			GenEmptyObj(&newADJdata->ConcatimageObj);
			SAFE_DELETE(newADJdata);

			while (!ADJClientService->m_qADJBuffer[m_iModuleNo_ADJCnt].IsEmpty())
			{
				newADJdata = ADJClientService->m_qADJBuffer[m_iModuleNo_ADJCnt].next();

				strDefectCode = newADJdata->strDefectCode;
				ConcatReviewImage = newADJdata->ConcatReviewImage;
				iMzNo = newADJdata->iMzNo;
				sLotID = newADJdata->sLotID;
				iPcVisionNo = newADJdata->iPcVisionNo;
				iTrayNo = newADJdata->iTrayNo;
				ConcatimageObj = newADJdata->ConcatimageObj;
				iModuleNo = newADJdata->iModuleNo;
				iDefectCnt = newADJdata->iDefectCnt;

				GenEmptyObj(&newADJdata->HADJSaveRegion);
				GenEmptyObj(&newADJdata->ConcatimageObj);
				SAFE_DELETE(newADJdata);
			}
		}
		else
		{
			strNewAdjLog += "\tG";

			GenEmptyObj(&newADJdata->ConcatimageObj);
			SAFE_DELETE(newADJdata);
		}

		LeaveCriticalSection(&THEAPP.m_csADJResultLog);

		if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveOnlyADJLotResultLog(THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1], strNewAdjLog, "", FALSE, THEAPP.Struct_PreferenceStruct.m_iPCType, iPcVisionNo, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveOnlyADJLotResultLog(THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1], strNewAdjLog, "", FALSE, THEAPP.Struct_PreferenceStruct.m_iPCType, iPcVisionNo, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveOnlyADJDayLotResultLog(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], strNewAdjLog, THEAPP.Struct_PreferenceStruct.m_iPCType, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveOnlyADJDayLotResultLog(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], strNewAdjLog, THEAPP.Struct_PreferenceStruct.m_iPCType, iMzNo);
		}
		else
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ONLY_ADJ_LOT_RESULT;
				pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1];
				pSaveLogThreadIDParam->strType1 = strNewAdjLog;
				pSaveLogThreadIDParam->strType2 = "";
				pSaveLogThreadIDParam->bType = FALSE;
				pSaveLogThreadIDParam->iPcVisionNo = iPcVisionNo;
				pSaveLogThreadIDParam->iMzNo = iMzNo;
				pSaveLogThreadIDParam->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ONLY_ADJ_LOT_RESULT;
				pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1];
				pSaveLogThreadIDParam_Dual->strType1 = strNewAdjLog;
				pSaveLogThreadIDParam_Dual->strType2 = "";
				pSaveLogThreadIDParam_Dual->bType = FALSE;
				pSaveLogThreadIDParam_Dual->iPcVisionNo = iPcVisionNo;
				pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
				pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ONLY_ADJ_DAY_LOT_RESULT;
				pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
				pSaveLogThreadIDParam->strType1 = strNewAdjLog;
				pSaveLogThreadIDParam->iMzNo = iMzNo;
				pSaveLogThreadIDParam->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ONLY_ADJ_DAY_LOT_RESULT;
				pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
				pSaveLogThreadIDParam_Dual->strType1 = strNewAdjLog;
				pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
				pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
			}
		}

		if (result != 0 && !THEAPP.Struct_PreferenceStruct.m_bSideTotal[iCurDeepLearningModel - 1])
		{
			if (THEAPP.m_nADJDefectPriority[iCurDeepLearningModel - 1] <= THEAPP.m_nADJCurDefectPriority[nThreadNO][iModuleNo - 1])
			{
				THEAPP.m_nADJCurDefectPriority[nThreadNO][iModuleNo - 1] = THEAPP.m_nADJDefectPriority[iCurDeepLearningModel - 1]; //우선순위 높은 디펙트의 "우선순위번호를 찾는다"(모델ID가 아닌 것에 주의)
				THEAPP.m_nADJModelIDApplyPriority[nThreadNO][iModuleNo - 1] = iCurDeepLearningModel;
			}
			else
			{
				THEAPP.m_nADJModelIDApplyPriority[nThreadNO][iModuleNo - 1] = iCurDeepLearningModel;
			}
		}
		else
		{
			THEAPP.m_nADJModelIDApplyPriority[nThreadNO][iModuleNo - 1] = iCurDeepLearningModel;
		}

		// ADJ 서로 다른 PC에 연결할 시 함수내에 매개변수 값 iClientNO 로 변경해야 함
		CString strDeepLearningModel, strDefectCodeSection;
		strDefectCodeSection.Format("DeepLearning_Model_%s", ADJClientService->GetDeepLearningModelNameFromIndex(iDeepLearningModelID, iVisionCamType, sADJInspectionTypeCode, sADJDefectDetail));
		BOOL bDeepLearningResultNG = (result != 0);
		if (THEAPP.Struct_PreferenceStruct.m_bSideTotal[0][(iDeepLearningModelID - 1) % DEEP_MODEL_NUM])
			strDeepLearningModel.Format("DeepLearning_Model_%s", ADJClientService->GetDeepLearningModelNameFromIndex(iDeepLearningModelID, iVisionCamType));

		if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveADJDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], strDefectCodeSection, bDeepLearningResultNG, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveADJDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], strDefectCodeSection, bDeepLearningResultNG, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bSideTotal[0][(iDeepLearningModelID - 1) % DEEP_MODEL_NUM])
			{
				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
					THEAPP.SaveADJDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], strDeepLearningModel, bDeepLearningResultNG, iMzNo);

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
					THEAPP.SaveADJDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], strDeepLearningModel, bDeepLearningResultNG, iMzNo);

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
					THEAPP.SaveADJTotalImageSummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], strDeepLearningModel, bDeepLearningResultNG, iMzNo);

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
					THEAPP.SaveADJTotalImageSummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], strDeepLearningModel, bDeepLearningResultNG, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveADJDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], "ADJ_TOTAL_IMAGE", bDeepLearningResultNG, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveADJDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], "ADJ_TOTAL_IMAGE", bDeepLearningResultNG, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveADJTotalImageSummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], "ADJ_TOTAL_IMAGE", bDeepLearningResultNG, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveADJTotalImageSummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], "ADJ_TOTAL_IMAGE", bDeepLearningResultNG, iMzNo);
		}
		else
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
				pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
				pSaveLogThreadIDParam->strType1 = strDefectCodeSection;
				pSaveLogThreadIDParam->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam->iMzNo = iMzNo;
				pSaveLogThreadIDParam->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
				pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
				pSaveLogThreadIDParam_Dual->strType1 = strDefectCodeSection;
				pSaveLogThreadIDParam_Dual->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
				pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
			}
			if (THEAPP.Struct_PreferenceStruct.m_bSideTotal[0][(iDeepLearningModelID - 1) % DEEP_MODEL_NUM])
			{
				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
					pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
					pSaveLogThreadIDParam->strType1 = strDeepLearningModel;
					pSaveLogThreadIDParam->bType = bDeepLearningResultNG;
					pSaveLogThreadIDParam->iMzNo = iMzNo;
					pSaveLogThreadIDParam->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
				}

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
					pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
					pSaveLogThreadIDParam_Dual->strType1 = strDeepLearningModel;
					pSaveLogThreadIDParam_Dual->bType = bDeepLearningResultNG;
					pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
					pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
				}

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ADJ_TOTAL_IMAGE_SUMMARY;
					pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
					pSaveLogThreadIDParam->strType1 = strDeepLearningModel;
					pSaveLogThreadIDParam->bType = bDeepLearningResultNG;
					pSaveLogThreadIDParam->iMzNo = iMzNo;
					pSaveLogThreadIDParam->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
				}

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ADJ_TOTAL_IMAGE_SUMMARY;
					pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
					pSaveLogThreadIDParam_Dual->strType1 = strDeepLearningModel;
					pSaveLogThreadIDParam_Dual->bType = bDeepLearningResultNG;
					pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
					pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
				}
			}
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
				pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
				pSaveLogThreadIDParam->strType1 = "ADJ_TOTAL_IMAGE";
				pSaveLogThreadIDParam->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam->iMzNo = iMzNo;
				pSaveLogThreadIDParam->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
				pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
				pSaveLogThreadIDParam_Dual->strType1 = "ADJ_TOTAL_IMAGE";
				pSaveLogThreadIDParam_Dual->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
				pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ADJ_TOTAL_IMAGE_SUMMARY;
				pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
				pSaveLogThreadIDParam->strType1 = "ADJ_TOTAL_IMAGE";
				pSaveLogThreadIDParam->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam->iMzNo = iMzNo;
				pSaveLogThreadIDParam->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ADJ_TOTAL_IMAGE_SUMMARY;
				pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
				pSaveLogThreadIDParam_Dual->strType1 = "ADJ_TOTAL_IMAGE";
				pSaveLogThreadIDParam_Dual->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
				pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
			}
		}

		int iModuleTotalEndTime = GetTickCount() - iModuleTotalStartTime;

		strLog.Format("ADJ%d 송수신 시간:%d, Module 단위 ADJ 검사 시간(Timeout 기준): %d, LotID: %s, Model: %d, Tray: %d, Module: %d, Defect count: %d, DefectCode: %s, Result: %s",
			iClientNO,
			elapsedTime,
			iModuleTotalEndTime,
			sLotID,
			iDeepLearningModelID,
			iTrayNo,
			iModuleNo,
			iDefectCnt + 1,
			strDefectCode,
			strResult);
		THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));


		if (result < 0)
		{
			THEAPP.m_TCPClientService.m_nTimeOutCnt++;
			if (THEAPP.m_TCPClientService.m_nTimeOutCnt > 50000) //Time out이 연속 50 이상 발생 하면..
			{
				THEAPP.m_TCPClientService.m_nTimeOutCnt = 0;
				if (THEAPP.m_TCPClientService.m_bConnect[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM])
				{
					for (int i = 0; i < THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO; i++)
					{
						THEAPP.m_TCPClientService.m_arrClient[iClientNO / MAX_MULTI_CONNECTION_NUM][i].TCPonClose();
						THEAPP.m_TCPClientService.m_bConnect[iClientNO / MAX_MULTI_CONNECTION_NUM][i] = FALSE;
					}
				}
			}
		}

		if (result >= 0)
		{
			//strLog.Format("ADJ_Network_Thread Log(0) :: *THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt - 1] : %d, THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt - 1] : %d", THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt - 1], THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt - 1]);
			//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
			strLog.Format("ADJ_Network_Thread Log(1), ADJ request left: %d, ADJ network thread live: %d", ADJClientService->m_qADJBuffer[m_iModuleNo_ADJCnt].size(), THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt]);
			THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
		}
		else
		{
			strLog.Format("ADJ_Network_Thread Log(1), ADJ request left : %d, ADJ timeout", ADJClientService->m_qADJBuffer[m_iModuleNo_ADJCnt].size());
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		}
	}
	if (result >= 0 && THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt] > 0)
		THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt] = THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt] - 1;

	strLog.Format("ADJ_Network_Thread2, ADJ network thread left: %d", THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt]);
	THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
	return 0;
}


UINT CADJClientService::FAI_ADJ_Network_Thread(LPVOID lparam)
{
	CString strLog;
	CADJClientService *ADJClientService = (CADJClientService*)lparam;
	int result = 0;
	int m_iModuleNo_ADJCnt = ADJClientService->m_iModuleNo_ADJCnt - 1;

	//strLog.Format("1: m_iModuleNo_ADJCnt: %d, THEAPP.g_arriThreadCnt: %d", m_iModuleNo_ADJCnt + 1, THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt]);
	//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
	while (!ADJClientService->m_qFAIADJBuffer[m_iModuleNo_ADJCnt].IsEmpty())
	{
		stFAIADJNetworkData* newADJdata = NULL;

		newADJdata = ADJClientService->m_qFAIADJBuffer[m_iModuleNo_ADJCnt].next();

		if (newADJdata == NULL)
			continue;

		stTCPPacket tcpPacket = newADJdata->tcpPacket;
		int iClientNO = newADJdata->iClientNO;
		int nThreadNO = newADJdata->nThreadNO;
		DWORD iModuleTotalStartTime = newADJdata->iModuleTotalStartTime;
		int iMzNo = newADJdata->iMzNo;
		CString sLotID = newADJdata->sLotID;
		int iPcVisionNo = newADJdata->iPcVisionNo;
		int iTrayNo = newADJdata->iTrayNo;
		int iModuleNo = newADJdata->iModuleNo;
		int iDefectCnt = newADJdata->iDefectCnt;
		int iFAI_Number = newADJdata->iFAI_Number;
		int iDeepLearningModelID = newADJdata->iDeepLearningModelID;
		//int iVisionCamType = newADJdata->iVisionCamType;
		//int iCurDeepLearningModel = newADJdata->iCurDeepLearningModel;
		//int testADJStartTime = newADJdata->testADJStartTime;
		int &iReviewSaveCnt = *newADJdata->iReviewSaveCnt;
		CString sBarcodeID = newADJdata->sBarcodeID;
		SYSTEMTIME time = newADJdata->time;
		CString ConcatFilename = newADJdata->ConcatFilename;
		//CString strDefectCode = newADJdata->strDefectCode;
		//CString sADJInspectionTypeCode = newADJdata->sADJInspectionTypeCode;
		//CString sADJDefectDetail = newADJdata->sADJDefectDetail;
		HObject ConcatimageObj = newADJdata->ConcatimageObj;
		HObject &HADJSaveRegion = newADJdata->HADJSaveRegion;
		CString ConcatReviewImage = newADJdata->ConcatReviewImage;
		int iFaiParamIndex = newADJdata->iFaiParamIndex;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		int startT = GetTickCount();

		EnterCriticalSection(&THEAPP.m_csSendADJ[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM]);

		strLog.Format("ADJ send thread #%d start", iClientNO);
		THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

		result = ADJClientService->SendFAIImagetoADJ(tcpPacket, iClientNO, iModuleTotalStartTime);
		Sleep(1);


		LeaveCriticalSection(&THEAPP.m_csSendADJ[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM]);

		int elapsedTime = GetTickCount() - startT;
		int m_nADJDefectCnt = atoi((const char*)tcpPacket.byteDefectCnt);

		EnterCriticalSection(&THEAPP.m_csADJResultLog);

		if (m_nADJDefectCnt == 0)
		{
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] = _T("");
			THEAPP.iADJLogResult[nThreadNO][iModuleNo - 1] = -1;
			for (int i = 0; i < MAX_MODEL_NUM; i++)
				THEAPP.iADJModelResult[nThreadNO][i][iModuleNo - 1] = -1;
		}

		THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += ConcatFilename + '/';

		int iADJLogingStartTime = GetTickCount();
		CString strResult;
		if (result == 0)	// 정상적인 Edge 값 넘어올 때
		{
			CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bDeepLearningResultGood[iFAI_Number] = TRUE;

			strResult = "OK";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "OK/";
			//THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] = THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] * THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1];

			//GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else if (result == 1) // ADJ 정확성 낮은 값, Edge 값 차이가 threshold 보다 높을 경우 
		{
			CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bDeepLearningResultGood[iFAI_Number] = FALSE;

			strResult = "NG";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "NG/";
			//THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] *= 0;

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else if (result == RCV_TIMEOUT)
		{
			strResult = "TIME_OUT";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "TO/";
			THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] *= 0;
			THEAPP.bIsTimeOut = TRUE;

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else if (result == RCV_WAIT)
		{
			strResult = "WAIT_error";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "W/";
			THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] *= 0;
			THEAPP.bIsTimeOut = TRUE;

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else if (result == RCV_ERROR)
		{
			strResult = "TCP_error";
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += "E/";
			THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] *= 0;
			THEAPP.bIsTimeOut = TRUE;

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}
		else
		{
			strResult = "UNKNOWN";
			CString strTemp;
			strTemp.Format("value:%d/", result);
			THEAPP.strADJLog[nThreadNO][iModuleNo - 1] += strTemp;
			THEAPP.iADJModelResult[nThreadNO][iDeepLearningModelID - 1][iModuleNo - 1] *= 0;
			THEAPP.bIsTimeOut = TRUE;

			GenEmptyObj(&newADJdata->HADJSaveRegion);
		}

		int iADJLogingFLAGENDTime = GetTickCount() - iADJLogingStartTime;

		//20200716 jwj start
		CString strNewAdjLog;
		SYSTEMTIME timeAdj;
		GetLocalTime(&timeAdj);

		strNewAdjLog.Format("%04d-%02d-%02d\t%04d-%02d-%02d %02d:%02d:%02d\t%s\t%s\t%s\t%s\t%d\t%d\t%s\t%FAI%d\t%s",
			timeAdj.wYear, timeAdj.wMonth, timeAdj.wDay,
			timeAdj.wYear, timeAdj.wMonth, timeAdj.wDay,
			time.wHour, time.wMinute, time.wSecond,
			THEAPP.GetPCID(),
			THEAPP.Struct_PreferenceStruct.m_strEquipNo,
			sLotID,
			THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName,
			iTrayNo,
			iModuleNo,
			sBarcodeID,
			iFAI_Number,//ADJClientService->GetDeepLearningModelNameFromIndex(iDeepLearningModelID, iVisionCamType, sADJInspectionTypeCode, sADJDefectDetail),
			ConcatFilename
		);

		if (result == 0)
		{
			strNewAdjLog += "\tG";

			GenEmptyObj(&newADJdata->ConcatimageObj);
			SAFE_DELETE(newADJdata);
		}
		else
		{
			strNewAdjLog.Format("%s\tFAI%d", strNewAdjLog, iFAI_Number);

			GenEmptyObj(&newADJdata->ConcatimageObj);
			SAFE_DELETE(newADJdata);
		}

		LeaveCriticalSection(&THEAPP.m_csADJResultLog);

		/*if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveOnlyADJLotResultLog(THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1], strNewAdjLog, "", FALSE, THEAPP.Struct_PreferenceStruct.m_iPCType, iPcVisionNo, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveOnlyADJLotResultLog(THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1], strNewAdjLog, "", FALSE, THEAPP.Struct_PreferenceStruct.m_iPCType, iPcVisionNo, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveOnlyADJDayLotResultLog(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], strNewAdjLog, THEAPP.Struct_PreferenceStruct.m_iPCType, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveOnlyADJDayLotResultLog(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], strNewAdjLog, THEAPP.Struct_PreferenceStruct.m_iPCType, iMzNo);
		}
		else
		{*/
		if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
		{
			RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
			pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ONLY_ADJ_LOT_RESULT;
			pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1];
			pSaveLogThreadIDParam->strType1 = strNewAdjLog;
			pSaveLogThreadIDParam->strType2 = "";
			pSaveLogThreadIDParam->bType = FALSE;
			pSaveLogThreadIDParam->iPcVisionNo = iPcVisionNo;
			pSaveLogThreadIDParam->iMzNo = iMzNo;
			pSaveLogThreadIDParam->iTrayNo = iTrayNo;
			pSaveLogThreadIDParam->iModuleNo = iModuleNo;
			THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
		}

		if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
		{
			RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
			pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ONLY_ADJ_LOT_RESULT;
			pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1];
			pSaveLogThreadIDParam_Dual->strType1 = strNewAdjLog;
			pSaveLogThreadIDParam_Dual->strType2 = "";
			pSaveLogThreadIDParam_Dual->bType = FALSE;
			pSaveLogThreadIDParam_Dual->iPcVisionNo = iPcVisionNo;
			pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
			pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
			pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
			THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
		}

		if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
		{
			RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
			pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ONLY_ADJ_DAY_LOT_RESULT;
			pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
			pSaveLogThreadIDParam->strType1 = strNewAdjLog;
			pSaveLogThreadIDParam->iMzNo = iMzNo;
			pSaveLogThreadIDParam->iTrayNo = iTrayNo;
			pSaveLogThreadIDParam->iModuleNo = iModuleNo;
			THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
		}

		if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
		{
			RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
			pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ONLY_ADJ_DAY_LOT_RESULT;
			pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
			pSaveLogThreadIDParam_Dual->strType1 = strNewAdjLog;
			pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
			pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
			pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
			THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
		}
		//}

		//if (result != 0 && !THEAPP.Struct_PreferenceStruct.m_bSideTotal[iCurDeepLearningModel - 1])
		//{
		//	if (THEAPP.m_nADJDefectPriority[iCurDeepLearningModel - 1] <= THEAPP.m_nADJCurDefectPriority[nThreadNO][iModuleNo - 1])
		//	{
		//		THEAPP.m_nADJCurDefectPriority[nThreadNO][iModuleNo - 1] = THEAPP.m_nADJDefectPriority[iCurDeepLearningModel - 1]; //우선순위 높은 디펙트의 "우선순위번호를 찾는다"(모델ID가 아닌 것에 주의)
		//		THEAPP.m_nADJModelIDApplyPriority[nThreadNO][iModuleNo - 1] = iCurDeepLearningModel;
		//	}
		//	else
		//	{
		//		THEAPP.m_nADJModelIDApplyPriority[nThreadNO][iModuleNo - 1] = iCurDeepLearningModel;
		//	}
		//}
		//else
		//{
		//	THEAPP.m_nADJModelIDApplyPriority[nThreadNO][iModuleNo - 1] = iCurDeepLearningModel;
		//}

		// ADJ 서로 다른 PC에 연결할 시 함수내에 매개변수 값 iClientNO 로 변경해야 함
		CString strDeepLearningModel;
		strDeepLearningModel.Format("DeepLearning_FAI_Model_%d", iFAI_Number);
		BOOL bDeepLearningResultNG = (result != 0);
		//if (THEAPP.Struct_PreferenceStruct.m_bSideTotal[0][(iDeepLearningModelID - 1) % DEEP_MODEL_NUM])
		//	strDeepLearningModel.Format("DeepLearning_Model_%s", ADJClientService->GetDeepLearningModelNameFromIndex(iDeepLearningModelID, iVisionCamType));

		if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveADJDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], strDeepLearningModel, bDeepLearningResultNG, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveADJDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], strDeepLearningModel, bDeepLearningResultNG, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveADJDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], "ADJ_TOTAL_IMAGE", bDeepLearningResultNG, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveADJDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], "ADJ_TOTAL_IMAGE", bDeepLearningResultNG, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				THEAPP.SaveADJTotalImageSummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], "ADJ_TOTAL_IMAGE", bDeepLearningResultNG, iMzNo);

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				THEAPP.SaveADJTotalImageSummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], "ADJ_TOTAL_IMAGE", bDeepLearningResultNG, iMzNo);
		}
		else
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
				pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
				pSaveLogThreadIDParam->strType1 = strDeepLearningModel;
				pSaveLogThreadIDParam->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam->iMzNo = iMzNo;
				pSaveLogThreadIDParam->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
				pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
				pSaveLogThreadIDParam_Dual->strType1 = strDeepLearningModel;
				pSaveLogThreadIDParam_Dual->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
				pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
			}
			if (THEAPP.Struct_PreferenceStruct.m_bSideTotal[0][(iDeepLearningModelID - 1) % DEEP_MODEL_NUM])
			{
				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
					pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
					pSaveLogThreadIDParam->strType1 = strDeepLearningModel;
					pSaveLogThreadIDParam->bType = bDeepLearningResultNG;
					pSaveLogThreadIDParam->iMzNo = iMzNo;
					pSaveLogThreadIDParam->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
				}

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
					pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
					pSaveLogThreadIDParam_Dual->strType1 = strDeepLearningModel;
					pSaveLogThreadIDParam_Dual->bType = bDeepLearningResultNG;
					pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
					pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
				}

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ADJ_TOTAL_IMAGE_SUMMARY;
					pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
					pSaveLogThreadIDParam->strType1 = strDeepLearningModel;
					pSaveLogThreadIDParam->bType = bDeepLearningResultNG;
					pSaveLogThreadIDParam->iMzNo = iMzNo;
					pSaveLogThreadIDParam->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
				}

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ADJ_TOTAL_IMAGE_SUMMARY;
					pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
					pSaveLogThreadIDParam_Dual->strType1 = strDeepLearningModel;
					pSaveLogThreadIDParam_Dual->bType = bDeepLearningResultNG;
					pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
					pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
				}
			}
			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
				pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
				pSaveLogThreadIDParam->strType1 = "ADJ_TOTAL_IMAGE";
				pSaveLogThreadIDParam->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam->iMzNo = iMzNo;
				pSaveLogThreadIDParam->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ADJ_DAY_SUMMARY;
				pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
				pSaveLogThreadIDParam_Dual->strType1 = "ADJ_TOTAL_IMAGE";
				pSaveLogThreadIDParam_Dual->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
				pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_ADJ_TOTAL_IMAGE_SUMMARY;
				pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
				pSaveLogThreadIDParam->strType1 = "ADJ_TOTAL_IMAGE";
				pSaveLogThreadIDParam->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam->iMzNo = iMzNo;
				pSaveLogThreadIDParam->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
			{
				RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
				pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_ADJ_TOTAL_IMAGE_SUMMARY;
				pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
				pSaveLogThreadIDParam_Dual->strType1 = "ADJ_TOTAL_IMAGE";
				pSaveLogThreadIDParam_Dual->bType = bDeepLearningResultNG;
				pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
				pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
				pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
				THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
			}
		}

		int iModuleTotalEndTime = GetTickCount() - iModuleTotalStartTime;

		strLog.Format("ADJ%d 송수신 시간:%d, Module 단위 ADJ 검사 시간(Timeout 기준): %d, LotID: %s, Model: %d, Tray: %d, Module: %d, iFaiParameter: %d, Defect count: %d, FAI_Number: %d, Result: %s",
			iClientNO,
			elapsedTime,
			iModuleTotalEndTime,
			sLotID,
			iDeepLearningModelID,
			iTrayNo,
			iModuleNo,
			iFaiParamIndex,
			iDefectCnt + 1,
			iFAI_Number,
			strResult);
		THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));

		if (result < 0)
		{
			THEAPP.m_TCPClientService.m_nTimeOutCnt++;
			if (THEAPP.m_TCPClientService.m_nTimeOutCnt > 50000) //Time out이 연속 50 이상 발생 하면..
			{
				THEAPP.m_TCPClientService.m_nTimeOutCnt = 0;
				if (THEAPP.m_TCPClientService.m_bConnect[iClientNO / MAX_MULTI_CONNECTION_NUM][iClientNO % MAX_MULTI_CONNECTION_NUM])
				{
					for (int i = 0; i < THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO; i++)
					{
						THEAPP.m_TCPClientService.m_arrClient[iClientNO / MAX_MULTI_CONNECTION_NUM][i].TCPonClose();
						THEAPP.m_TCPClientService.m_bConnect[iClientNO / MAX_MULTI_CONNECTION_NUM][i] = FALSE;
					}
				}
			}
		}

		if (result >= 0)
		{
			//strLog.Format("ADJ_Network_Thread Log(0), THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt - 1]: %d, THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt]: %d", THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt], THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt+1]);
			//THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
			strLog.Format("ADJ_Network_Thread Log(1), ADJ request left: %d, ADJ network thread live: %d", ADJClientService->m_qADJBuffer[m_iModuleNo_ADJCnt].size(), THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt]);
			THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
		}
		else
		{
			strLog.Format("ADJ_Network_Thread Log(1), ADJ request left: %d, ADJ timeout", ADJClientService->m_qADJBuffer[m_iModuleNo_ADJCnt].size());
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		}
	}
	if (THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt] > 0)
		THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt] = THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt] - 1;

	strLog.Format("ADJ_Network_Thread2, ADJ network thread left : %d", THEAPP.g_arriThreadCnt[m_iModuleNo_ADJCnt]);
	THEAPP.m_log_adj->debug("{}", LOG_CSTR(strLog));
	return 0;
}

#endif
