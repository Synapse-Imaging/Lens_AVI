#pragma once


// CAutoFocusDlg 대화 상자

class CAutoFocusDlg : public CDialog
{
	DECLARE_DYNAMIC(CAutoFocusDlg)

public:
	CAutoFocusDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAutoFocusDlg();

	static	CAutoFocusDlg* GetInstance(BOOL bShowFlag = FALSE);
	void	DeleteInstance();

	void	Show();
	void	Hide();

	BOOL	GetShowStatus() { return m_bShowDlg; }

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTO_FOCUS_DLG };
#endif

	CComboBox m_cbAFZigCenterLightPage;
	CButtonCS m_bnRunCalibration;
	CButtonCS m_bnSaveLeftTopCornerPosition;
	CButtonCS m_bnClose;
	CButtonCS m_bnSave;

	double m_dEditZigCenterCamZ;
	BOOL m_bCheckUseZigCenterPos;
	int m_iEditZigCenterX;
	int m_iEditZigCenterY;
	double m_dEditAfZigRotationCenterX;
	double m_dEditAfZigRotationCenterY;
	int m_iEditAfLeftTopCornerX;
	int m_iEditAfLeftTopCornerY;
	double m_dEditAfRotationAngle;

	int m_iEditAfLensStartImageNo;
	int m_iEditAfLensEndImageNo;
	int m_iEditAfModuleRotationAngleImageNo;
	int m_iEditAfEastSideStartImageNo;
	int m_iEditAfWestSideStartImageNo;
	int m_iEditAfNorthSideStartImageNo;
	int m_iEditAfSouthSideStartImageNo;
	int m_iEditAfEastSideEndImageNo;
	int m_iEditAfWestSideEndImageNo;
	int m_iEditAfNorthSideEndImageNo;
	int m_iEditAfSouthSideEndImageNo;

	void UpdateParam();

	CWinThread*	m_pThread;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	afx_msg LRESULT OnAfZigRotCenterResult(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;

	static CAutoFocusDlg*	m_pInstance;

	BOOL		m_bShowDlg;

public:
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonStartCenterCalibration();
	afx_msg void OnBnClickedButtonTopSaveLtPos();
	afx_msg void OnBnClickedButtonSave();

	void ChangeLanguage();
};
