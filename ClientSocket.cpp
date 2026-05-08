// ClientSocket.cpp : implementation file

#include "stdafx.h"
#include "ClientSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket()
{
	m_byteReceive = NULL;
	//2003/05/08 KJW
	m_nReceiveLen = 0;
	m_nReceiveSize = MAX_RECEIVE_LEN;
	m_byteReceive = new BYTE[m_nReceiveSize];
	m_iClientIdx = 0;
}

CClientSocket::~CClientSocket()
{
	if (m_byteReceive)
		delete m_byteReceive;
}

BOOL CClientSocket::Create(LPSTR szAddress, UINT nPortNo, CWnd* pParentWnd, int nReceiveSize)
{

	if (!CSocket::Create())
	{
		//AfxMessageBox("Error : CSocket::Create() in socket");	//RYU 2005/07/21
		return FALSE;
	}

	if (!CSocket::Connect(szAddress, nPortNo))
	{
		CSocket::Close();
		//AfxMessageBox("Error : CSocket::Connect() in socket");	//RYU 2005/07/21
		return FALSE;
	}

	m_wndParent = pParentWnd;

	/*
	m_nReceiveLen  = 0;

	m_nReceiveSize = nReceiveSize;
	m_byteReceive = new BYTE[m_nReceiveSize];
	*/

	return TRUE;
}

BOOL CClientSocket::ConnectLan(int iClientIdx, LPSTR szAddress, UINT nPortNo, CWnd* pParentWnd)
{
	if (pParentWnd)
	{
		CSocket::Close();

		if (!CSocket::Create())
		{
			//AfxMessageBox("Error : CSocket::Create() in socket");	//RYU 2005/07/21
			return FALSE;
		}

		if (!CSocket::Connect(szAddress, nPortNo))
		{
			CSocket::Close();
			//AfxMessageBox("Error : CSocket::Connect() in socket");	//RYU 2005/07/21
			return FALSE;
		}

		m_iClientIdx = iClientIdx;
		m_wndParent = pParentWnd;
	}

	return TRUE;
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions

void CClientSocket::OnReceive(int nErrorCode)
{
	// 데이터 읽기
	for (int i = 0; i < MAX_RECEIVE_LEN; i++)
		m_byteReceive[i] = NULL;
	m_nReceiveLen = Receive(m_byteReceive, m_nReceiveSize);

	// 통지 : SCM_RECEIVE
	m_wndParent->SendMessage(SCM_RECEIVE, WPARAM(nErrorCode), LPARAM(this));

	CSocket::OnReceive(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode)
{
	m_wndParent->SendMessage(SCM_CLOSE, WPARAM(nErrorCode), LPARAM(this));

	CSocket::OnClose(nErrorCode);
}
