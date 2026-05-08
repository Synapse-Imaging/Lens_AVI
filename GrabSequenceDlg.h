#pragma once


// CGrabSequenceDlg 대화 상자

class CGrabSequenceDlg : public CDialog
{
	DECLARE_DYNAMIC(CGrabSequenceDlg)

public:
	CGrabSequenceDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CGrabSequenceDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GRAB_SEQUENCE_DLG };
#endif

	int m_iEditAddrSeq[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT][LIGHTCTRL_GRAB_ADDR_PAGE_COUNT];
	int m_iEditAddrCount[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT];
	CString m_sEditDesc[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

protected:
	CButtonCS m_bnSave;
	CButtonCS m_bnExit;

public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();

	void ChangeLanguage();
	CString strLog;
	CString strMessageBox;
};
