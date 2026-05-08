// SerialPortCS.cpp: implementation of the CSerialPortCS class.
//
#include "stdafx.h"
#include "SerialPortCS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define BUFFER_SIZE		1024

CSerialPortCS::CSerialPortCS()
{
	m_nInterval = 10;

	m_hWnd = NULL;
	m_hComm = NULL;

	memset(&m_ovRead, 0, sizeof(OVERLAPPED));
	memset(&m_ovWrite, 0, sizeof(OVERLAPPED));

	m_bConnected = FALSE;
	m_pRecvThread = NULL;

	m_nPortID = 0;
}

CSerialPortCS::~CSerialPortCS()
{
}

BOOL CSerialPortCS::Open_Port(int nPortID, int nBaud, int nData, int nStop, int nParity, HWND hWnd)
{
#ifndef INLINE_MODE
	return FALSE;
#endif

	if (m_bConnected) return FALSE;
	m_ovRead.Offset = 0;
	m_ovRead.OffsetHigh = 0;
	m_ovRead.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
	if (m_ovRead.hEvent == NULL) return FALSE;

	m_ovWrite.Offset = 0;
	m_ovWrite.OffsetHigh = 0;
	m_ovWrite.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
	if (m_ovWrite.hEvent == NULL) return FALSE;

	CString strName;
	if (nPortID > 0 && nPortID < 257) {
		m_nPortID = nPortID;
		if (m_nPortID < 10) strName.Format(_T("COM%d"), m_nPortID);
		else strName.Format(_T("\\\\.\\COM%d"), m_nPortID);
	}
	else {
		//AfxMessageBox("Port Number Error");
		return FALSE;
	}

	m_hComm = CreateFile(strName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);

	if (m_hComm == INVALID_HANDLE_VALUE) {
		//AfxMessageBox("Port Open Error");
		return FALSE;
	}

	SetCommMask(m_hComm, EV_RXCHAR);
	SetupComm(m_hComm, BUFFER_SIZE, BUFFER_SIZE);
	PurgeComm(m_hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	COMMTIMEOUTS CommTimeOuts;
	if (!GetCommTimeouts(m_hComm, &CommTimeOuts)) return FALSE;

	CommTimeOuts.ReadIntervalTimeout = m_nInterval;	// 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;	// CBR_9600/dwBaud*2;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;

	if (!SetCommTimeouts(m_hComm, &CommTimeOuts)) return FALSE;

	DCB dcb;
	if (!GetCommState(m_hComm, &dcb)) return FALSE;

	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = (DWORD)nBaud;
	dcb.ByteSize = (BYTE)nData;
	dcb.StopBits = Get_StopBit(nStop);
	dcb.Parity = Get_ParityBit(nParity);

	if (!SetCommState(m_hComm, &dcb)) return FALSE;

	m_hWnd = hWnd;

	m_bConnected = TRUE;
	m_pRecvThread = AfxBeginThread(Receive_Thread, this);

	return TRUE;
}

BOOL CSerialPortCS::Close_Port()
{
	if (!m_bConnected)
		return FALSE;

	m_bConnected = FALSE;
	DWORD dwRet = WaitForSingleObject(m_pRecvThread->m_hThread, INFINITE);

	if (dwRet == WAIT_OBJECT_0) {
		m_hWnd = NULL;
		m_nPortID = 0;
		m_pRecvThread = NULL;

		SetCommMask(m_hComm, 0);
		PurgeComm(m_hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

		CloseHandle(m_ovRead.hEvent);
		CloseHandle(m_ovWrite.hEvent);

		CloseHandle(m_hComm);

		return TRUE;
	}
	else
		return FALSE;
}

BYTE CSerialPortCS::Get_StopBit(int nStop)
{
	if (nStop == 15) return ONE5STOPBITS;
	else if (nStop == 2) return TWOSTOPBITS;
	else return ONESTOPBIT;
}

BYTE CSerialPortCS::Get_ParityBit(int nParity)
{
	if (nParity == 1) return ODDPARITY;
	else if (nParity == 2) return EVENPARITY;
	else if (nParity == 3) return MARKPARITY;
	else if (nParity == 4) return SPACEPARITY;
	else return NOPARITY;
}

void CSerialPortCS::Write_String(CString strSend)
{
	DWORD dwWritten, dwErrorFlags, dwTransfered;
	COMSTAT comstat;

	int nLength = strSend.GetLength();

	if (!WriteFile(m_hComm, strSend, nLength, &dwWritten, &m_ovWrite)) {
		if (GetLastError() == ERROR_IO_PENDING) {
			while (!GetOverlappedResult(m_hComm, &m_ovWrite, &dwTransfered, TRUE)) {
				if (GetLastError() != ERROR_IO_INCOMPLETE) {
					ClearCommError(m_hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else {
			dwWritten = 0;
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}
}

void CSerialPortCS::Read_String()
{
	DWORD dwRead, dwTransfered, dwErrorFlags;
	COMSTAT comstat;

	ClearCommError(m_hComm, &dwErrorFlags, &comstat);
	if (comstat.cbInQue < 1) return;

	char chRecv[BUFFER_SIZE] = { 0 };

	if (!ReadFile(m_hComm, chRecv, BUFFER_SIZE, &dwRead, &m_ovRead)) {
		if (GetLastError() == ERROR_IO_PENDING) {
			while (!GetOverlappedResult(m_hComm, &m_ovRead, &dwTransfered, TRUE)) {
				if (GetLastError() != ERROR_IO_INCOMPLETE) {
					ClearCommError(m_hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
			CString strRecv = (CString)chRecv;
			if (m_hWnd) SendMessage(m_hWnd, UM_PORT_RECEIVE, (WPARAM)m_nPortID, (LPARAM)&strRecv);
		}
		else {
			dwRead = 0;
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Thread Function

UINT CSerialPortCS::Receive_Thread(LPVOID lpVoid)
{
	OVERLAPPED ov;
	DWORD dwEventMask;

	CSerialPortCS *pSerialPort = (CSerialPortCS*)lpVoid;

	memset(&ov, 0, sizeof(OVERLAPPED));
	ov.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);

	while (pSerialPort->m_bConnected) {
		dwEventMask = 0;
		WaitCommEvent(pSerialPort->m_hComm, &dwEventMask, &ov);

		if ((dwEventMask & EV_RXCHAR) == EV_RXCHAR) {
			pSerialPort->Read_String();
		}
		Sleep(1);
	}
	CloseHandle(ov.hEvent);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
