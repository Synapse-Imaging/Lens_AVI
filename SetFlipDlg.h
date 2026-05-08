#pragma once


// CSetFlipDlg 대화 상자

class CSetFlipDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetFlipDlg)

public:
	CSetFlipDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CSetFlipDlg();

	int m_iRadioLeftRightFlip;

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SET_FLIP_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
