#pragma once


// CLightPwmControlDlg 대화 상자

class CLightPwmControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightPwmControlDlg)

public:
	CLightPwmControlDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLightPwmControlDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHT_PWM_CONTROL_DLG };
#endif

	CSliderCtrl m_SliderLight1;
	CSliderCtrl m_SliderLight2;
	CSliderCtrl m_SliderLight3;
	CSliderCtrl m_SliderLight4;
	int m_iEditLight1;
	int m_iEditLight2;
	int m_iEditLight3;
	int m_iEditLight4;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

protected:
	CButtonCS m_bnChangeLight;
	CButtonCS m_bnExit;
	CButtonCS m_bnLightOn;
	CButtonCS m_bnLightOff;

protected:
	void UpdateLightChannelValue();

public:
	afx_msg void OnChangeEditLight1();
	afx_msg void OnChangeEditLight2();
	afx_msg void OnChangeEditLight3();
	afx_msg void OnChangeEditLight4();

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonChangeLightvalue();
	int m_iEditComPortNumber;

	afx_msg void OnBnClickedButtonLightOn();
	afx_msg void OnBnClickedButtonLightOff();
};
