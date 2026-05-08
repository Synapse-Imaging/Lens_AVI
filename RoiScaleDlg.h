#pragma once


// CRoiScaleDlg 대화 상자입니다.

class CRoiScaleDlg : public CDialog
{
	DECLARE_DYNAMIC(CRoiScaleDlg)

public:
	CRoiScaleDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRoiScaleDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ROI_SCALE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	BOOL m_bCheckApplyVisionROI;
	double m_dEditScale;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
