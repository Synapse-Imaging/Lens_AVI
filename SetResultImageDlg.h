#pragma once


// CSetResultImageDlg 대화 상자입니다.

class CSetResultImageDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetResultImageDlg)

public:
	CSetResultImageDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetResultImageDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SET_RESULT_IMAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	CComboBox m_cVisionImage[VISION_NUMBER_MAX][MAX_RESULT_IMAGE];

	void ChangeLanguage();

	CString strDialog;
};
