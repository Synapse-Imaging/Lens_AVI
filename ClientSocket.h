#if !defined(AFX_CLIENTSOCKET_H__2A49E7C6_67E3_11D3_8401_006097663D30__INCLUDED_)
#define AFX_CLIENTSOCKET_H__2A49E7C6_67E3_11D3_8401_006097663D30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSocket.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// Client 소켓 Event
#define	SCM_CLOSE	WM_USER+20	// Client 소켓 해제
#define	SCM_RECEIVE	WM_USER+30	// Client로 부터 수신되는 데이터

/////////////////////////////////////////////////////////////////////////////
// 	ON_MESSAGE(SCM_CLOSE,   OnClientClose)		// Client 소켓 해제
// 	ON_MESSAGE(SCM_RECEIVE, OnClientReceive)	// Client로 부터 수신되는 데이터
// 	afx_msg LRESULT OnClientClose(WPARAM wParam, LPARAM lClient);	// Client 소켓 해제
// 	afx_msg LRESULT OnClientReceive(WPARAM wParam, LPARAM lClient);	// Client로 부터 수신되는 데이터

/////////////////////////////////////////////////////////////////////////////
// CClientSocket command target

#define MAX_RECEIVE_LEN	4096
//#define MAX_RECEIVE_LEN	40960

class CClientSocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CClientSocket();
	virtual ~CClientSocket();

	BOOL Create(LPSTR szAddress, UINT nPortNo, CWnd* pParentWnd, int nReceiveDataSize=MAX_RECEIVE_LEN);

	//2003/05/10 KJW
	BOOL ConnectLan(int iClientIdx, LPSTR szAddress, UINT nPortNo, CWnd* pParentWnd);

// Overrides
public:
	
	BYTE*	m_byteReceive;	// 수신 버퍼
	int		m_nReceiveLen;	// 수신 데이터 길이

	int GetClientIndex() { return m_iClientIdx;  }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	int		m_nReceiveSize;	// 수신 데이터의 최대 길이
	CWnd*	m_wndParent;	// Parent 윈도우
	int		m_iClientIdx;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__2A49E7C6_67E3_11D3_8401_006097663D30__INCLUDED_)
