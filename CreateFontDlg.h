#pragma once


// CCreateFontDlg 대화 상자입니다.

class CCreateFontDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateFontDlg)

public:
	CCreateFontDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCreateFontDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATE_FONT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_sFontName;
};
