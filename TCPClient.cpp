// TCPClient.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "WS2_Client.h"
#include "TCPClient.h"

//Recv & Close
static UINT DoReceiveOrClose(LPVOID lParam);

// CTCPClient
IMPLEMENT_DYNAMIC(CTCPClient, CWnd)

CTCPClient::CTCPClient(void)
{
	/*
		WORD sockVersion;
		WSADATA wsaData;

		sockVersion = MAKEWORD(2,2);
		WSAStartup(sockVersion, &wsaData);

	*/
	m_nTotalByteTokenPos = 1;

	InitializeCriticalSection(&CS_TCP_LOG);

	m_HGetReceiveData[CONNECT_EVENT] = CreateEventA(NULL, TRUE, FALSE, 0);
	m_HGetReceiveData[RECV_EVENT] = CreateEventA(NULL, TRUE, FALSE, 0);
	m_HGetReceiveData[SERVER_CLOSE_EVENT] = CreateEventA(NULL, TRUE, FALSE, 0);
	m_HGetReceiveData[RESTART_EVENT] = CreateEventA(NULL, TRUE, FALSE, 0);

	m_threadRecv = nullptr;
	m_bConnect = FALSE;

	m_TCPClient = NULL;

	m_bRecvThreadRun = FALSE;

	m_strCurrentLogPath = "";

}

CTCPClient::~CTCPClient(void)
{
	if (m_threadRecv != nullptr)
	{
		::TerminateThread(m_threadRecv->m_hThread, 0);
		//	CloseHandle(m_threadRecv->m_hThread);
		m_threadRecv = nullptr;
	}
	WSACleanup();
}

BEGIN_MESSAGE_MAP(CTCPClient, CWnd)
END_MESSAGE_MAP()

void CTCPClient::SockInit(void)
{
	WORD sockVersion;
	WSADATA wsaData;

	sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);
	SetSockInit(TRUE);
}
BOOL CTCPClient::TCPClientInit(CString strServerIP, int nPort, CString strClientIP)
{

	if (!m_bSockInit)
	{
		SockInit();
	}

	int rVal = 0;

	m_TCPClient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_TCPClient == SOCKET_ERROR)
	{
		SocketErrorMessage("Failed socket()");
		return FALSE;
	}

	SetServerParam(strServerIP, nPort);
	m_strClientIP = strClientIP;

	//set client
	SOCKADDR_IN serverInfo;

	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin_family = PF_INET;
	serverInfo.sin_port = htons(nPort);
	serverInfo.sin_addr.s_addr = inet_addr(strServerIP);

	//set Client socket option
	unsigned long isNonBockingMode = 1;
	ioctlsocket(m_TCPClient, FIONBIO, &isNonBockingMode);

	int optval = 1;
	setsockopt(m_TCPClient, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval)); //190902 commented
	//setsockopt( m_TCPClient, IPPROTO_TCP, TCP_NODELAY, (const char *)&optval, sizeof(optval)); //190902 commented
	optval = (256 * 256 * 3 * 6) * 5;
	setsockopt(m_TCPClient, SOL_SOCKET, SO_SNDBUF, (const char *)&optval, sizeof(optval));
	setsockopt(m_TCPClient, SOL_SOCKET, SO_RCVBUF, (const char *)&optval, sizeof(optval));
	/*int optval = MAX_BUF_SIZE;

	int optlen = sizeof(optval);

	setsockopt(m_TCPClient , SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval));
	setsockopt(m_TCPClient , SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval));
*/
//set Client socket option end

//Network Event 등록
	WSAEVENT hEvent = WSACreateEvent();
	if (hEvent == WSA_INVALID_EVENT)
	{
		SocketErrorMessage("WSACreateEvent()");
		return FALSE;
	}

	m_WSAEventArr[NET_EVNET_NUM - 1] = hEvent;

	rVal = WSAEventSelect(m_TCPClient, m_WSAEventArr[NET_EVNET_NUM - 1], FD_CONNECT | FD_READ | FD_CLOSE);

	if (rVal == SOCKET_ERROR)
	{
		SocketErrorMessage("WSAEventSelect()");
		return FALSE;
	}

	return TRUE;
}

void CTCPClient::TCPReset(void)
{
	m_bConnect = FALSE;

	m_TCPClient = NULL;

	//WSACleanup();

	//SetSockInit(FALSE);
}

void CTCPClient::SetServerParam(CString strServerIP, int nPort)
{
	m_strServerIP = strServerIP;
	m_nTCPPort = nPort;
}


BOOL CTCPClient::TCPConnect(void)
{
	if (m_TCPClient == NULL)
	{
		if (!TCPClientInit(m_strServerIP, m_nTCPPort, m_strClientIP))	//TO DO: IP PORT ini 바꾸고 되는지 확인.
		{
			CString strErrorLog;
			strErrorLog.Format("[ ADJ ]TCP Initialize Error IP %s Port %d", m_strServerIP, m_nTCPPort);
			TCPSaveLog(strErrorLog);
		}

	}

	sockaddr_in service;

	memset(&service, 0, sizeof(service));
	service.sin_family = PF_INET;
	service.sin_addr.s_addr = inet_addr(m_strClientIP); // bind to specific IP address

	service.sin_port = htons(0);

	int iResult = bind(m_TCPClient, (SOCKADDR *)&service, sizeof(service));

	if (iResult == SOCKET_ERROR)
	{
		SocketErrorMessage("[TCP Error] bind Failed");
		return FALSE;
	}

	int rVal = 0;

	SOCKADDR_IN serverInfo;

	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin_family = PF_INET;
	serverInfo.sin_addr.s_addr = inet_addr(m_strServerIP);
	serverInfo.sin_port = htons(m_nTCPPort);

	rVal = connect(m_TCPClient, (LPSOCKADDR)&serverInfo, sizeof(serverInfo));

	if (WSAGetLastError() == WSAEWOULDBLOCK)
	{
		rVal = TCPNetConnectEventWait();
		if (rVal == SOCKET_ERROR)
		{
			m_bConnect = FALSE;
			return FALSE;
		}
	}
	else
	{
		SocketErrorMessage("[TCP Error] Connect Failed");
		m_bConnect = FALSE;
		return FALSE;
	}

	if (m_bRecvThreadRun == FALSE)
	{
		m_bRecvThreadRun = TRUE;
		m_threadRecv = AfxBeginThread(DoReceiveOrClose, this);
	}

	SetEvent(m_HGetReceiveData[CONNECT_EVENT]);
	m_bConnect = TRUE;
	TCPSaveLog("[TCP]Connect Success");
	return TRUE;
}

void CTCPClient::TCPSend(BYTE* pbyteMessage, int nMessageByte)
{
	int nSendByte = send(this->m_TCPClient, (const char*)pbyteMessage, nMessageByte, 0);

	if (nSendByte <= 0)
	{
		SocketErrorMessage("[TCP] Send Error");

		if (this->m_TCPClient != NULL)
		{
			TCPonClose();
		}
	}
	else if (nSendByte < nMessageByte)
	{
		TCPSaveLog(" logical send proccess is needed ");
	}
}

void CTCPClient::TCPonClose(void)
{
	if (m_TCPClient != NULL)
	{
		WSAResetEvent(m_WSAEventArr[NET_EVNET_NUM - 1]);
		WSACloseEvent(m_WSAEventArr[NET_EVNET_NUM - 1]);
		closesocket(m_TCPClient);
	}
	m_TCPClient = NULL;
	SetEvent(m_HGetReceiveData[SERVER_CLOSE_EVENT]);
	m_bConnect = FALSE;
	TCPSaveLog("[TCP]Disconnected");
}

//void CTCPClient::TCPonReceive( void )
//{
//	const int ONCE_BUF_SIZE = 65535;
//	BYTE pbyteRcvTempBuffer[ ONCE_BUF_SIZE ];
//	BYTE* pbyteRcvBuffer = new BYTE [ MAX_BUF_SIZE ];
//	memset( pbyteRcvBuffer   , 0, MAX_BUF_SIZE );
//	memset( pbyteRcvTempBuffer , 0, ONCE_BUF_SIZE);
//	
//	int nReadLen = 0;
//	int nByteLen = 0;
//	int nTotalByte = 0;
//	CString strLog;
//	CString csToken(_T(""));
//	CString csResultPath(_T(""));
//	BOOL    bIsFirstRecv = TRUE;
//	int nTokenPos = 1 + ( (PACKET_STRING_SIZE + 1 ) * GetTotalByteTokenPos() );
//
//	do 
//	{
//		nReadLen = recv( m_TCPClient, (char*)pbyteRcvTempBuffer, ONCE_BUF_SIZE,0 );
//		
//		if( bIsFirstRecv )
//		{
//			BYTE strTotalSizeTempBuffer[ PACKET_STRING_SIZE ];
//			
//			memcpy( strTotalSizeTempBuffer, &pbyteRcvTempBuffer[ nTokenPos ], PACKET_STRING_SIZE );
//			nTotalByte = atoi( (const char*)strTotalSizeTempBuffer );
//
//			if( nTotalByte <= 0 )
//			{
//				memset( pbyteRcvTempBuffer, 0 , nReadLen );
//				strLog.Format( "[TCP Logical Error] Total Byte <= 0 / Pleas Check Token Position");  
//				TCPSaveLog( strLog );
//				return;
//			}
//			else
//			{
//				bIsFirstRecv = FALSE;
//			}
//
//		}
//
//		if ( nTotalByte >= 0 && nReadLen != SOCKET_ERROR )
//		{
//			memcpy( &pbyteRcvBuffer[ nByteLen ], pbyteRcvTempBuffer, nReadLen );
//			nByteLen += nReadLen;
//			memset( pbyteRcvTempBuffer, 0 , nReadLen );
//		}
//
//		if( nByteLen == nTotalByte )
//		{
//			memset( &pbyteRcvBuffer[ nByteLen + 1 ], 0 , MAX_BUF_SIZE - nByteLen - 1 );
//			break;
//		}
//		else if( nByteLen > nTotalByte)
//		{
//			memcpy( pbyteRcvBuffer, &pbyteRcvBuffer[ nTotalByte ], nByteLen - nTotalByte );
//			memset( &pbyteRcvBuffer[ nByteLen - nTotalByte ], 0, MAX_BUF_SIZE - nByteLen + nTotalByte );
//			strLog.Format ( "[TCP Info] RcvByte (%d) > TotalByte (%d) Duplicated Data",nByteLen,nTotalByte);
//			BYTE strTotalSizeTempBuffer[ PACKET_STRING_SIZE ];
//			memcpy( strTotalSizeTempBuffer, &pbyteRcvTempBuffer[ nTokenPos ], PACKET_STRING_SIZE );
//			nTotalByte = atoi( (const char*)strTotalSizeTempBuffer );
//			TCPSaveLog( strLog );
//		}
//	} while ( nReadLen > 0 );
//	
//	if( nTotalByte <= 1024 )
//	{
//		BYTE tempByteRcvBuffer[ 1024 ] = {0,};
//		memcpy( tempByteRcvBuffer, pbyteRcvBuffer, nTotalByte );
//		m_qRecvBuffer.push( tempByteRcvBuffer );
//		delete pbyteRcvBuffer;
//	}
//	else
//	{
//		m_qRecvBuffer.push( pbyteRcvBuffer );				//ASSERT : memory release after pop...
//	}
//	SetEvent( m_HGetReceiveData[ RECV_EVENT ] );		    //notify receive event to external function
//}


void CTCPClient::TCPonReceive(void)
{
	const int ONCE_BUF_SIZE = 65535;
	//const int ONCE_BUF_SIZE = 610+608;
	BYTE pbyteRcvTempBuffer[ONCE_BUF_SIZE];
	BYTE* m_pbyteRcvBuffer = new BYTE[MAX_BUF_SIZE];
	memset(m_pbyteRcvBuffer, 0, MAX_BUF_SIZE);
	memset(pbyteRcvTempBuffer, 0, ONCE_BUF_SIZE);

	int nReadLen = 0;
	int nByteLen = 0;
	int nTotalByte = 0;
	CString strLog;
	CString csToken(_T(""));
	CString csResultPath(_T(""));
	BOOL    bIsFirstRecv = TRUE;
	int nTokenPos = 1 + ((PACKET_STRING_SIZE + 1) * GetTotalByteTokenPos());
	int nErrCnt = 0;
	int nError = 0;

	TCPSaveLog("[TCPClient::DoReceiveOrClose] 1");

	do
	{
		nReadLen = recv(m_TCPClient, (char*)pbyteRcvTempBuffer, ONCE_BUF_SIZE, 0);

		if (nReadLen <= 0)
		{
			if (nReadLen == 0 && m_bConnect == TRUE)
			{
				strLog.Format("[TCP normal close ] Server socket is closed / recv byte == 0 ");
				TCPSaveLog(strLog);
				TCPonClose();
				SAFE_DELETE(m_pbyteRcvBuffer);
				break;
			}

			nError = WSAGetLastError();
			if (nError == WSAEWOULDBLOCK)
			{
				if (nErrCnt > 30)
				{
					nReadLen = SOCKET_ERROR;
					TCPSaveLog("[TCPClient::DoReceiveOrClose] ERROR 30");
					break;
				}
				Sleep(3);
				nErrCnt++;
				continue;
			}
			strLog.Format("[TCP Error] Client socket is closed / recv byte < 0 ");
			TCPSaveLog(strLog);
			if (m_bConnect == TRUE)
			{
				TCPonClose();
			}
			SAFE_DELETE(m_pbyteRcvBuffer);
			return;
		}

		if (bIsFirstRecv)
		{
			BYTE strTotalSizeTempBuffer[PACKET_STRING_SIZE] = { 0, };
			memcpy(strTotalSizeTempBuffer, &pbyteRcvTempBuffer[nTokenPos], PACKET_STRING_SIZE);

			nTotalByte = atoi((const char*)strTotalSizeTempBuffer);

			if (nTotalByte <= 0)
			{
				memset(pbyteRcvTempBuffer, 0, nReadLen);
				strLog.Format("[TCP Logical Error] Total Byte <= 0 / Pleas Check Token Position");
				if (m_bConnect == TRUE)
				{
					TCPonClose();
				}
				TCPSaveLog(strLog);
				SAFE_DELETE(m_pbyteRcvBuffer);
				return;
			}
			else
			{
				bIsFirstRecv = FALSE;
			}
		}

		if (nTotalByte >= 0 && nReadLen != SOCKET_ERROR)
		{
			memcpy(&m_pbyteRcvBuffer[nByteLen], pbyteRcvTempBuffer, nReadLen);
			nByteLen += nReadLen;
			memset(pbyteRcvTempBuffer, 0, ONCE_BUF_SIZE);
		}

		if (nByteLen == nTotalByte)
		{
			memset(&m_pbyteRcvBuffer[nByteLen + 1], 0, MAX_BUF_SIZE - nByteLen - 1);	//데이터가 정확히 나누어 떨어진 경우 -> m_pbyteRcvBuffer를 검사에 직접 씀
			break;
		}
		else if (nByteLen > nTotalByte)
		{
			BOOL bLoopSwitch = TRUE; //20200308 jwj add : data 가 2유닛 이상 들어올 때 처리를 위해서 추가.
			BOOL bRecvEnd = FALSE; //20200308 jwj add data가 한 유닛을 초과한 데이터를 처리하고 나서 유닛 단위로 나누어 떨어질 때 반복문을 탈출하기 위해서

			while (bLoopSwitch)
			{
				memset(&m_pbyteRcvBuffer[nByteLen], 0, MAX_BUF_SIZE - nByteLen);	//( 지금까지 받은 데이터 이후의 값 초기화 )

				BYTE* m_pbyteRcvBuffer_copy = new BYTE[MAX_BUF_SIZE];				//데이터가 이어져있을 경우 -> m_pbyteRcvBuffer를 검사에 직접 쓰지 않고 원하는 데이터만 memcopy 후 m_pbyteRcvBuffer_copy 버퍼의 포인터를 검사에 씀
				memset(m_pbyteRcvBuffer_copy, 0, MAX_BUF_SIZE);
				memcpy(m_pbyteRcvBuffer_copy, m_pbyteRcvBuffer, nTotalByte);

				m_qRecvBuffer.push(m_pbyteRcvBuffer_copy);				//ASSERT : memory release after pop...

				SetEvent(m_HGetReceiveData[RECV_EVENT]);		    //notify receive event to external function

				memcpy(&m_pbyteRcvBuffer[0], &m_pbyteRcvBuffer[nTotalByte], nByteLen - nTotalByte);				//데이터가 이어져 있을 경우. 1차적으로 받기 원한 데이터를 다 보내고 나면 이제 잘린 데이터를 앞으로 당겨 이어받을 준비를 한다.
				memset(&m_pbyteRcvBuffer[nByteLen - nTotalByte], 0, MAX_BUF_SIZE - nByteLen + nTotalByte);
				strLog.Format("[Info] RcvByte (%d) > TotalByte (%d) Duplicated Data", nByteLen, nTotalByte);

				nByteLen -= nTotalByte; //20200308 add

				BYTE strTotalSizeTempBuffer[PACKET_STRING_SIZE] = { 0, };
				if (nByteLen < nTokenPos) //만약 데이터가 겹쳐 들어왔을 때 앞의 정상적인 데이터 단위까지 처리한 뒤 남은 부분 중 데이터 크기를 읽을 위치까지 읽지 못했다면 데이터를 추가로 받는다.
				{
					nReadLen = recv(m_TCPClient, (char*)pbyteRcvTempBuffer, nTokenPos, 0); //데이터가 유닛단위로 더 들어오는 것을 방지하기위해 tokenpos 만큼만 데이터 수신

					if (nReadLen == SOCKET_ERROR || nReadLen == 0)
					{
						SocketErrorMessage("Failed re recv for re reading data size");
						if (m_bConnect == TRUE)
						{
							TCPonClose();
							SAFE_DELETE(m_pbyteRcvBuffer);
							return;
						}
					}

					memcpy(&m_pbyteRcvBuffer[nByteLen], pbyteRcvTempBuffer, nReadLen);
					nByteLen += nReadLen;
					memset(pbyteRcvTempBuffer, 0, ONCE_BUF_SIZE);
				}

				memcpy(strTotalSizeTempBuffer, &m_pbyteRcvBuffer[nTokenPos], PACKET_STRING_SIZE);

				nTotalByte = atoi((const char*)strTotalSizeTempBuffer);

				TCPSaveLog(strLog);

				if (nTotalByte <= 0) //비정상적인 경우
				{
					memset(m_pbyteRcvBuffer, 0, nReadLen);
					strLog.Format("[TCP Logical Error] Total Byte <= 0 / Pleas Check Token Position");
					if (m_bConnect == TRUE)
					{
						TCPonClose();
					}
					TCPSaveLog(strLog);
					SAFE_DELETE(m_pbyteRcvBuffer);
					return;
				}
				else			      //정상적인 경우
				{
					//nByteLen = nByteLen - nTotalByte;	//현재 여기에 음수값이 들어감 이유를 찾아야함. //191028
					//nByteLen = nByteLen - nResetByte;	//191028 수정 //20200308 commented( 위에서 처리함 )

					if (nByteLen == nTotalByte) //20200306 add
					{
						bRecvEnd = TRUE;
						break;
					}
					else if (nByteLen < nTotalByte)
					{
						bLoopSwitch = FALSE;
					}
				}
			} //while( bLoopSwitch )
			if (bRecvEnd) //data 가 유닛단위로 정확히 들어온 순간 while문 탈출
				break;
		}
	} while (nReadLen > 0 || nError == WSAEWOULDBLOCK);

	if (nReadLen == SOCKET_ERROR)
	{
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			SocketErrorMessage("Failed recv()");
			if (m_bConnect == TRUE)
			{
				TCPonClose();
				SAFE_DELETE(m_pbyteRcvBuffer);
				return;
			}
		}
	}
	TCPSaveLog("[TCPClient::DoReceiveOrClose] 2");
	/*if( nTotalByte <= 1024 ) //20191121 commented local variable byte pointer를 넘기다니..
	{
		BYTE tempByteRcvBuffer[ 1024 ] = {0,};
		memcpy( tempByteRcvBuffer, m_pbyteRcvBuffer, nTotalByte );
		m_qRecvBuffer.push( tempByteRcvBuffer );
		delete m_pbyteRcvBuffer;
	}
	else
	{*/
	m_qRecvBuffer.push(m_pbyteRcvBuffer);				//ASSERT : memory release after pop...
//}
	SetEvent(m_HGetReceiveData[RECV_EVENT]);		    //notify receive event to external function //20190920 commented
}


int CTCPClient::TCPNetConnectEventWait(void)
{
	int rVal = 0;

	int index = WSAWaitForMultipleEvents(NET_EVNET_NUM, m_WSAEventArr, FALSE, NET_EVENT_WAIT_TIME, FALSE);
	if (index == WSA_WAIT_FAILED)
	{
		SocketErrorMessage("WSAWaitForMultipleEvents()");
		return SOCKET_ERROR;
	}
	else if (index == WSA_WAIT_TIMEOUT)
	{
		SocketErrorMessage("WSAWaitForMultipleEvents() timeout");
		return SOCKET_ERROR;
	}

	index -= WSA_WAIT_EVENT_0;

	// 구체적인 네트워크 이벤트 알아내기
	rVal = WSAEnumNetworkEvents(m_TCPClient,
		m_WSAEventArr[index],
		&m_networkEvents);

	if (rVal == SOCKET_ERROR)
	{
		SocketErrorMessage("WSAEnumNetworkEvents()");
		return SOCKET_ERROR;
	}

	if ((m_networkEvents.lNetworkEvents & FD_CONNECT))
	{
		//connect는 비동기 소켓에서 WSAEWOULDBLOCK을 바로 리턴한다.(server와 접속하는데 시간이 걸리기 때문) 따라서 connect 시도시 네트웍 이벤트로 연결이 완료될때까지 이벤트를 기다려 본다.
		if ((m_networkEvents.lNetworkEvents & FD_CONNECT) && (m_networkEvents.iErrorCode[FD_CONNECT_BIT] != 0))
		{
			CString strErrorLog;
			strErrorLog.Format("[TCP]Network Event(Connect) Error Code %d", m_networkEvents.iErrorCode[FD_CONNECT_BIT]);
			SocketErrorMessage(strErrorLog);
			return SOCKET_ERROR;
		}
	}

	return rVal;
}

int CTCPClient::TCPNetRcvCloseEventWait(void)
{
	int rVal = 0;

	int index = WSAWaitForMultipleEvents(NET_EVNET_NUM, m_WSAEventArr, FALSE, NET_EVENT_WAIT_TIME, FALSE);
	if (index == WSA_WAIT_FAILED)
	{
		SocketErrorMessage("WSAWaitForMultipleEvents()");
		return SOCKET_ERROR;
	}
	else if (index == WSA_WAIT_TIMEOUT)
	{
		return rVal;
	}

	index -= WSA_WAIT_EVENT_0;

	// 구체적인 네트워크 이벤트 알아내기
	rVal = WSAEnumNetworkEvents(m_TCPClient,
		m_WSAEventArr[index],
		&m_networkEvents);

	if (rVal == SOCKET_ERROR)
	{
		SocketErrorMessage("WSAEnumNetworkEvents()");
		return SOCKET_ERROR;
	}

	//Read Event 처리
	if ((m_networkEvents.lNetworkEvents & FD_READ))
	{
		//event error check
		if ((m_networkEvents.lNetworkEvents & FD_READ) && (m_networkEvents.iErrorCode[FD_READ_BIT] != 0))
		{
			CString strErrorLog;
			strErrorLog.Format("[TCP]Network Event(Read) Error Code %d", m_networkEvents.iErrorCode[FD_READ_BIT]);
			SocketErrorMessage(strErrorLog);
			return SOCKET_ERROR;
		}
		TCPonReceive();
	}
	// Close Event 처리
	else if (m_networkEvents.lNetworkEvents & FD_CLOSE)
	{
		//event error check
		if ((m_networkEvents.lNetworkEvents & FD_CLOSE) && (m_networkEvents.iErrorCode[FD_CLOSE] != 0))
		{
			CString strErrorLog;
			strErrorLog.Format("[TCP]Network Event(Close) Error Code %d", m_networkEvents.iErrorCode[FD_CLOSE]);
			SocketErrorMessage(strErrorLog);
			TCPonClose();
			return SOCKET_ERROR;
		}
		TCPonClose();
		return SOCKET_CLOSE;
	}

	return rVal;
}

void CTCPClient::SocketErrorMessage(CString strError)
{
	int nLastError = 0;
	nLastError = WSAGetLastError();
	CString strLog;
	strLog.Format("[%s] Error NO : %d", strError, nLastError);
	TCPSaveLog(strLog);
}

void CTCPClient::CreateDirForLog()
{
	CString strPath = GetLogPath();
	CString csPrefix(_T("")), csToken(_T(""));
	int nStart = 0, nEnd;
	while ((nEnd = strPath.Find('/', nStart)) >= 0)
	{
		CString csToken = strPath.Mid(nStart, nEnd - nStart);
		CreateDirectory(csPrefix + csToken, NULL);

		csPrefix += csToken;
		csPrefix += _T("/");
		nStart = nEnd + 1;
	}
	csToken = strPath.Mid(nStart);
	CreateDirectory(csPrefix + csToken, NULL);
}

void CTCPClient::TCPSaveLog(CString strLog)
{
	return; //20201012 jwj add (하드디스크 저장 안되는 문제 때문에)

	EnterCriticalSection(&CS_TCP_LOG);

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strFile;
	strFile.Format("%s/[%02d%02d%02d]TCPlog.txt", GetLogPath(), time.wYear,
		time.wMonth, time.wDay);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_TCP_LOG);
		return;
	}

	try {
		file.SeekToEnd();

		CString strSave;
		strSave.Format("[%04d-%02d-%02d %02d:%02d:%02d %03d] %s\r\n",
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, strLog);

		file.Write(strSave, strSave.GetLength());
		file.Close();

	}
	catch (CFileException *pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_TCP_LOG);
}

UINT DoReceiveOrClose(LPVOID lParam)
{
	CTCPClient* clsClient = (CTCPClient*)lParam;
	int nErrorCheck;

	while (TRUE)
	{
		if (clsClient->GetTcpConnect())
		{
			nErrorCheck = clsClient->TCPNetRcvCloseEventWait();
			if (nErrorCheck == SOCKET_ERROR)
			{
				clsClient->SocketErrorMessage("[TCP] DoReceiveOrClose");
				Sleep(100);
			}
			else if (nErrorCheck == SOCKET_CLOSE || nErrorCheck == 0)
			{
				Sleep(1);
			}
		}
		else
		{
			Sleep(1);
		}
	}

	clsClient->m_bRecvThreadRun = FALSE;
	return 1;
}