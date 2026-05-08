#pragma once


// CStageSelectDlg 대화 상자

class CStageSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CStageSelectDlg)

public:
	CStageSelectDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CStageSelectDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STAGE_SELECT_DLG };
#endif

	int m_iRadioStageIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void ChangeLanguage();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
