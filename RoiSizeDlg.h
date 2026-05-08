#pragma once


// CRoiSizeDlg 대화 상자입니다.

class CRoiSizeDlg : public CDialog
{
	DECLARE_DYNAMIC(CRoiSizeDlg)

public:
	CRoiSizeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRoiSizeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ROI_SIZE_DLG };

	void SetPixelData(BOOL bPixel) { m_bPixelData = bPixel; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	double m_dEditSizeX;
	double m_dEditSizeY;

protected:
	BOOL m_bPixelData;
};
