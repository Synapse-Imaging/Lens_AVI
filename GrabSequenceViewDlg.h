#pragma once


// CGrabSequenceViewDlg 대화 상자

class CGrabSequenceViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CGrabSequenceViewDlg)

public:
	CGrabSequenceViewDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CGrabSequenceViewDlg();

	static	CGrabSequenceViewDlg* GetInstance(BOOL bShowFlag = FALSE);
	void	DeleteInstance();

	void	Show();
	void	Hide();

	BOOL	GetShowStatus() { return m_bShowDlg; }


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GRAB_SEQUENCE_VIEW_DLG };
#endif

	int m_iEditAddrCount[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT];
	CString m_sEditDesc[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();

private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;

	static CGrabSequenceViewDlg*	m_pInstance;

	BOOL		m_bShowDlg;
};
