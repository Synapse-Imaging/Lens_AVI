#pragma once


// CLightControlDlg 대화 상자입니다.
#include "LightControl.h"
#include "GlobalMatchingDlg.h"

class CLightControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightControlDlg)

public:
	CLightControlDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightControlDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LIGHT_CONTROL_DLG };
	static CLightControlDlg	*m_pInstance;

public:
	static	CLightControlDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void	Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:

	int m_iTeachingGrab;

	void UpdateLightValue(int iTabIdx);

	void GrabDeflectometry();
	void GrabDiffusedVision();
	void GrabSequence();
	void GrabCamera_SwTrg();

	BOOL MoveZigRotationCenterPos(double dZPos);

	BOOL m_bCheckPage[LIGHTCTRL_PAGE_COUNT];

	double m_dCurZPosition;
	double m_dCurLightZPosition;
	double m_dCurTPosition;
	double m_dCurRPosition;
	double m_dCurXPosition;
	double m_dCurYPosition;

	BOOL m_bDFMManualGrabDone;

protected:
	CButtonCS m_bnSetMatchingParam;
	CButtonCS m_bnSetFai;
	CButtonCS m_bnCameraLightManager;
	CButtonCS m_bnChangeLight;
	CButtonCS m_bnGrabSequence;
	CButtonCS m_bnChangeSpecular;
	CButtonCS m_bnAutoFocus;
	CButtonCS m_bnMotionControl;
	CButtonCS m_bnMoveZPos;
	CButtonCS m_bnImageGrab;

	void PageChecked(int iPageNumber);

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedButtonSetMatchingParam();
	afx_msg void OnBnClickedButtonSetFai();
	afx_msg void OnBnClickedButtonCameraLightManager();
	afx_msg void OnBnClickedButtonChangeLightvalue();
	afx_msg void OnBnClickedButtonGrabSequence();
	afx_msg void OnBnClickedButtonChangeSpecular();
	afx_msg void OnBnClickedButtonAutoFocus();
	afx_msg void OnBnClickedButtonMotionControl();
	afx_msg void OnBnClickedButtonMoveZPos();
	afx_msg void OnBnClickedGrab();

	afx_msg void OnBnClickedCheckPage1();
	afx_msg void OnBnClickedCheckPage2();
	afx_msg void OnBnClickedCheckPage3();
	afx_msg void OnBnClickedCheckPage4();
	afx_msg void OnBnClickedCheckPage5();
	afx_msg void OnBnClickedCheckPage6();
	afx_msg void OnBnClickedCheckPage7();
	afx_msg void OnBnClickedCheckPage8();
	afx_msg void OnBnClickedCheckPage9();
	afx_msg void OnBnClickedCheckPage10();
	afx_msg void OnBnClickedCheckPage11();
	afx_msg void OnBnClickedCheckPage12();
	afx_msg void OnBnClickedCheckPage13();
	afx_msg void OnBnClickedCheckPage14();
	afx_msg void OnBnClickedCheckPage15();
	afx_msg void OnBnClickedCheckPage16();
	afx_msg void OnBnClickedCheckPage17();
	afx_msg void OnBnClickedCheckPage18();
	afx_msg void OnBnClickedCheckPage19();
	afx_msg void OnBnClickedCheckPage20();
	afx_msg void OnBnClickedCheckPage21();
	afx_msg void OnBnClickedCheckPage22();
	afx_msg void OnBnClickedCheckPage23();
	afx_msg void OnBnClickedCheckPage24();
	afx_msg void OnBnClickedCheckPage25();
	afx_msg void OnBnClickedCheckPage26();
	afx_msg void OnBnClickedCheckPage27();
	afx_msg void OnBnClickedCheckPage28();
	afx_msg void OnBnClickedCheckPage29();
	afx_msg void OnBnClickedCheckPage30();
	afx_msg void OnBnClickedCheckPage31();
	afx_msg void OnBnClickedCheckPage32();

	void ChangeLanguage();

	void SetDlgStatus();

	CString strMessageBox;
};
