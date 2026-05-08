#pragma once


// CMotionControlDlg 대화 상자

class CMotionControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CMotionControlDlg)

public:
	CMotionControlDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMotionControlDlg();

	static	CMotionControlDlg* GetInstance(BOOL bShowFlag = FALSE);
	void	DeleteInstance();

	void	Show();
	void	Hide();

	BOOL	GetShowStatus() { return m_bShowDlg; }

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOTION_CONTROL_DLG };
#endif

	CButtonCS m_bnGetCurrentPos;
	CButtonCS m_bnSetCurrentPosToImage;
	CButtonCS m_bnAMove_Z;
	CButtonCS m_bnJMovePlus_Z;
	CButtonCS m_bnJMoveMinus_Z;
	CButtonCS m_bnAMove_LightZ;
	CButtonCS m_bnJMovePlus_LightZ;
	CButtonCS m_bnJMoveMinus_LightZ;
	CButtonCS m_bnAMove_T;
	CButtonCS m_bnJMovePlus_T;
	CButtonCS m_bnJMoveMinus_T;
	CButtonCS m_bnAMove_R;
	CButtonCS m_bnJMovePlus_R;
	CButtonCS m_bnJMoveMinus_R;
	CButtonCS m_bnAMove_X;
	CButtonCS m_bnJMovePlus_X;
	CButtonCS m_bnJMoveMinus_X;
	CButtonCS m_bnClose;

	double m_dEditCurPos_Z;
	double m_dEditCurPos_LightZ;
	double m_dEditCurPos_T;
	double m_dEditCurPos_R;
	double m_dEditCurPos_X;
	double m_dEditCurPos_Y;

	double m_dEditAMovePos_Z;
	double m_dEditRMovePos_Z;
	double m_dEditAMovePos_LightZ;
	double m_dEditRMovePos_LightZ;
	double m_dEditAMovePos_T;
	double m_dEditRMovePos_T;
	double m_dEditAMovePos_R;
	double m_dEditRMovePos_R;
	double m_dEditAMovePos_X;
	double m_dEditRMovePos_X;
	double m_dEditAMovePos_Y;
	double m_dEditRMovePos_Y;

	double m_dCurZPosition;
	double m_dCurLightZPosition;
	double m_dCurTPosition;
	double m_dCurRPosition;
	double m_dCurXPosition;
	double m_dCurYPosition;

	CComboBox m_cbInspectImageNo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonGetCurrentPosition();
	afx_msg void OnBnClickedButtonAmoveZ();
	afx_msg void OnBnClickedButtonJmovePlusZ();
	afx_msg void OnBnClickedButtonJmoveMinusZ();
	afx_msg void OnBnClickedButtonAmoveLightZ();
	afx_msg void OnBnClickedButtonJmovePlusLightZ();
	afx_msg void OnBnClickedButtonJmoveMinusLightZ();
	afx_msg void OnBnClickedButtonAmoveT();
	afx_msg void OnBnClickedButtonJmovePlusT();
	afx_msg void OnBnClickedButtonJmoveMinusT();
	afx_msg void OnBnClickedButtonAmoveR();
	afx_msg void OnBnClickedButtonJmovePlusR();
	afx_msg void OnBnClickedButtonJmoveMinusR();
	afx_msg void OnBnClickedButtonAmoveX();
	afx_msg void OnBnClickedButtonJmovePlusX();
	afx_msg void OnBnClickedButtonJmoveMinusX();

private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;

	static CMotionControlDlg*	m_pInstance;

	BOOL		m_bShowDlg;

public:
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonAmoveY();
	afx_msg void OnBnClickedButtonJmovePlusY();
	afx_msg void OnBnClickedButtonJmoveMinusY();
	afx_msg void OnBnClickedButtonSetCurrentPositionToImage();

	void ChangeLanguage();
	afx_msg void OnBnClickedButtonSetCurrentZpositionToImage();
};
