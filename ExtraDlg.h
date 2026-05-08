#pragma once


// CExtraDlg 대화 상자입니다.

class CExtraDlg : public CDialog
{
	DECLARE_DYNAMIC(CExtraDlg)

public:
	CExtraDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CExtraDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EXTRA_DLG };
	static CExtraDlg	*m_pInstance;

public:
	static CExtraDlg* GetInstance(BOOL bShowFlag=FALSE);
	void DeleteInstance();
	void Show();

	void LoadViewParam();
	void UpdateViewParam();

	void SetTeachModeControl();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	int m_iEditLightValueStart;
	int m_iEditLightValueEnd;
	int m_iEditLightValueInterval;
	int m_iEditLightValueInTol;
	int m_iEditTotalImageValueTol;

	double m_dEditFocusValueStart;
	double m_dEditFocusValueEnd;
	double m_dEditFocusValueInterval;
	int m_iRadioSelectFocusTol;
	double m_dEditFocusZValueTol;
	double m_dEditFocusEdgeValueTol;

	CProgressCtrl m_ctrlProgressAutoCal;

protected:
	CButtonCS m_bnAutoCalTestLight;
	CButtonCS m_bnLightMasterGV;
	CButtonCS m_bnAutoCalTestFocus;
	CButtonCS m_bnSaveSetting;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSaveSetting();
	afx_msg void OnBnClickedButtonLightMasterGV();
	afx_msg void OnBnClickedButtonLightCalTest();
	afx_msg void OnBnClickedButtonFocusCalTest();
};
