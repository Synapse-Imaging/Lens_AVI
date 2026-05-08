#pragma once


// CLoadModelDlg 대화 상자입니다.

class CLoadModelDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadModelDlg)

public:
	CLoadModelDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoadModelDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOAD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


public:
	void LoadModelList();
	CListBox m_ModelList;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListModelnames();
	CString m_EditSelectedModel;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString m_EditModelDescription;
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void ChangeLanguage();

	CString strMessageBox;
};
