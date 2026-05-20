#pragma once


// CAlignCamSelectDlg 대화 상자

class CAlignCamSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlignCamSelectDlg)

public:
	CAlignCamSelectDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAlignCamSelectDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALIGN_CAM_SELECT_DLG };
#endif

	int m_iRadioAlignCamIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void ChangeLanguage();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
