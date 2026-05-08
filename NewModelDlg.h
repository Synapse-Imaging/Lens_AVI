#pragma once


// CNewModelDlg 대화 상자입니다.

class CNewModelDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewModelDlg)

public:
	CNewModelDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNewModelDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_NEW_MODEL_DLG };

public:

	BOOL CheckInfomation();



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	CString m_Edit_strModelName;
	int m_Edit_iTrayArrayX;
	int m_Edit_iTrayArrayY;

	void ChangeLanguage();

	CString strMessageBox;
};
