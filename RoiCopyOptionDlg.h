#pragma once


// CRoiCopyOptionDlg 대화 상자

class CRoiCopyOptionDlg : public CDialog
{
	DECLARE_DYNAMIC(CRoiCopyOptionDlg)

public:
	CRoiCopyOptionDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CRoiCopyOptionDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROI_COPY_OPTION_DLG };
#endif

	BOOL m_bCheckCopyImage[MAX_IMAGE_TAB];
	BOOL m_bCheckDeleteRoiFirst;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
