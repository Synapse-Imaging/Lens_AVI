// SerialPortCS.h: interface for the CSerialPortCS class.
//
#pragma once

#define UM_PORT_RECEIVE	WM_USER + 9020

class CSerialPortCS
{
public:
	CSerialPortCS();
	virtual ~CSerialPortCS();
	
private:
	int			m_nPortID;
	int			m_nInterval;	// Char To Char Interval
	HANDLE		m_hComm;
	HWND		m_hWnd;

	OVERLAPPED	m_ovRead;
	OVERLAPPED	m_ovWrite;
	
	BOOL		m_bConnected;
	CWinThread *m_pRecvThread;

	BYTE Get_StopBit(int nStop);
	BYTE Get_ParityBit(int nParity);
	
	void Read_String();
	static UINT Receive_Thread(LPVOID lpVoid);

public:
	int Get_PortID() { return m_nPortID; }
	void Set_Interval(int nInterval) { m_nInterval = nInterval; }	// Open_Port 전에 호출할것
	BOOL Open_Port(int nPortID, int dwBaud, int byData, int byStop, int byParity, HWND hWnd = NULL);
	BOOL Close_Port();
	void Write_String(CString strSend);
};

/////////////////////////////////////////////////////////////////////////////
