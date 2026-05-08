#pragma once


// CRoiMoveDlg 대화 상자입니다.

class CRoiMoveDlg : public CDialog
{
	DECLARE_DYNAMIC(CRoiMoveDlg)

public:
	CRoiMoveDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRoiMoveDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ROI_MOVE_DLG };

	void SetPixelData(BOOL bPixel) { m_bPixelData = bPixel; }
	void SetEnableAllROI(BOOL bEnable) { m_bEnableAllROI = bEnable; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	double m_dEditMoveX;
	double m_dEditMoveY;
	
protected:
	BOOL m_bPixelData;
	BOOL m_bEnableAllROI;

public:
	BOOL m_bCheckApplyVisionROI;
};
