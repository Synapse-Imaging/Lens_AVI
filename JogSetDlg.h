#pragma once

// CJogSetDlg 대화 상자입니다.

class CJogSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CJogSetDlg)

public:
	CJogSetDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CJogSetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_JOG_SET_DLG };
		static CJogSetDlg	*m_pInstance;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

	CButtonCS m_bnSavePositionSetting;
	CButtonCS m_bnSaveLightSetting;
	CButtonCS m_bnSavePositionLightSetting;

public:
	static	CJogSetDlg* GetInstance(BOOL bShowFlag=FALSE);
	virtual BOOL OnInitDialog();
	void	DeleteInstance();
	void	Show();

	void LoadViewParam();
	void UpdateViewParam();
	void SetDlgStatus();
	void UpdateDlgStatus();
	void SetMotionParameter(int iInspectImageIndex, double dCurPos_Z, double dCurPos_LightZ, double dCurPos_T, double dCurPos_R, double dCurPos_X, double dCurPos_Y);
	afx_msg void OnBnClickedButtonSavePositionSetting();
	afx_msg void OnBnClickedButtonSaveLightSetting();
	afx_msg void OnBnClickedButtonSavePositionLightSetting();

	CRect	m_ScreenRect;
	CRect	GetPosition() { return m_ScreenRect; }
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	int m_iRadioLastInspectionImageIndex;
	BOOL m_bCheckMatchingImage[MAX_IMAGE_TAB];

	int m_iEditTriggerImageNumber;
	int m_iEditTriggerPeriod;
	int m_iEditTriggerTimeout;

	int m_iEditBa1ImageNumber;
	int m_iEditBa2ImageNumber;
	int m_iEditTaImageNumber;

	double m_dEditImageLightZPosRef;
	double m_dEditImageTiltPosRef;
	double m_dEditImageRotatePosRef;
	double m_dEditImageStageXPosRef;
	double m_dEditImageStageYPosRef;

	double m_dEditImageZPos[MAX_IMAGE_TAB];
	double m_dEditImageLightZPos[MAX_IMAGE_TAB];
	double m_dEditImageTiltPos[MAX_IMAGE_TAB];
	double m_dEditImageRotatePos[MAX_IMAGE_TAB];
	double m_dEditImageStageXPos[MAX_IMAGE_TAB];
	double m_dEditImageStageYPos[MAX_IMAGE_TAB];

	BOOL m_bCheckDiffusedImage[MAX_IMAGE_TAB];
	BOOL m_bCheckSpecularImage[MAX_IMAGE_TAB];
	BOOL m_bCheckSeqStartImage[MAX_IMAGE_TAB];
	CComboBox m_cbSeqAddrIndex[MAX_IMAGE_TAB];
	CComboBox m_cbPageIndex[MAX_IMAGE_TAB];

	void CheckBoxSetDiffused(int iImageIndex, BOOL bCheckBoxStatus);
	void CheckBoxSetSpecular(int iImageIndex, BOOL bCheckBoxStatus);
	void CheckBoxSetSequence(int iImageIndex, BOOL bCheckBoxStatus);
	afx_msg void OnBnClickedCheckDiffusedImage1();
	afx_msg void OnBnClickedCheckDiffusedImage2();
	afx_msg void OnBnClickedCheckDiffusedImage3();
	afx_msg void OnBnClickedCheckDiffusedImage4();
	afx_msg void OnBnClickedCheckDiffusedImage5();
	afx_msg void OnBnClickedCheckDiffusedImage6();
	afx_msg void OnBnClickedCheckDiffusedImage7();
	afx_msg void OnBnClickedCheckDiffusedImage8();
	afx_msg void OnBnClickedCheckDiffusedImage9();
	afx_msg void OnBnClickedCheckDiffusedImage10();
	afx_msg void OnBnClickedCheckDiffusedImage11();
	afx_msg void OnBnClickedCheckDiffusedImage12();
	afx_msg void OnBnClickedCheckDiffusedImage13();
	afx_msg void OnBnClickedCheckDiffusedImage14();
	afx_msg void OnBnClickedCheckDiffusedImage15();
	afx_msg void OnBnClickedCheckDiffusedImage16();
	afx_msg void OnBnClickedCheckDiffusedImage17();
	afx_msg void OnBnClickedCheckDiffusedImage18();
	afx_msg void OnBnClickedCheckDiffusedImage19();
	afx_msg void OnBnClickedCheckDiffusedImage20();
	afx_msg void OnBnClickedCheckDiffusedImage21();
	afx_msg void OnBnClickedCheckDiffusedImage22();
	afx_msg void OnBnClickedCheckDiffusedImage23();
	afx_msg void OnBnClickedCheckDiffusedImage24();
	afx_msg void OnBnClickedCheckDiffusedImage25();
	afx_msg void OnBnClickedCheckDiffusedImage26();
	afx_msg void OnBnClickedCheckDiffusedImage27();
	afx_msg void OnBnClickedCheckDiffusedImage28();
	afx_msg void OnBnClickedCheckDiffusedImage29();
	afx_msg void OnBnClickedCheckDiffusedImage30();
	afx_msg void OnBnClickedCheckDiffusedImage31();
	afx_msg void OnBnClickedCheckDiffusedImage32();
	afx_msg void OnBnClickedCheckDiffusedImage33();
	afx_msg void OnBnClickedCheckDiffusedImage34();
	afx_msg void OnBnClickedCheckDiffusedImage35();
	afx_msg void OnBnClickedCheckDiffusedImage36();
	afx_msg void OnBnClickedCheckDiffusedImage37();
	afx_msg void OnBnClickedCheckDiffusedImage38();
	afx_msg void OnBnClickedCheckDiffusedImage39();
	afx_msg void OnBnClickedCheckDiffusedImage40();
	afx_msg void OnBnClickedCheckDiffusedImage41();
	afx_msg void OnBnClickedCheckDiffusedImage42();
	afx_msg void OnBnClickedCheckDiffusedImage43();
	afx_msg void OnBnClickedCheckDiffusedImage44();
	afx_msg void OnBnClickedCheckDiffusedImage45();
	afx_msg void OnBnClickedCheckSpecularImage1();
	afx_msg void OnBnClickedCheckSpecularImage2();
	afx_msg void OnBnClickedCheckSpecularImage3();
	afx_msg void OnBnClickedCheckSpecularImage4();
	afx_msg void OnBnClickedCheckSpecularImage5();
	afx_msg void OnBnClickedCheckSpecularImage6();
	afx_msg void OnBnClickedCheckSpecularImage7();
	afx_msg void OnBnClickedCheckSpecularImage8();
	afx_msg void OnBnClickedCheckSpecularImage9();
	afx_msg void OnBnClickedCheckSpecularImage10();
	afx_msg void OnBnClickedCheckSpecularImage11();
	afx_msg void OnBnClickedCheckSpecularImage12();
	afx_msg void OnBnClickedCheckSpecularImage13();
	afx_msg void OnBnClickedCheckSpecularImage14();
	afx_msg void OnBnClickedCheckSpecularImage15();
	afx_msg void OnBnClickedCheckSpecularImage16();
	afx_msg void OnBnClickedCheckSpecularImage17();
	afx_msg void OnBnClickedCheckSpecularImage18();
	afx_msg void OnBnClickedCheckSpecularImage19();
	afx_msg void OnBnClickedCheckSpecularImage20();
	afx_msg void OnBnClickedCheckSpecularImage21();
	afx_msg void OnBnClickedCheckSpecularImage22();
	afx_msg void OnBnClickedCheckSpecularImage23();
	afx_msg void OnBnClickedCheckSpecularImage24();
	afx_msg void OnBnClickedCheckSpecularImage25();
	afx_msg void OnBnClickedCheckSpecularImage26();
	afx_msg void OnBnClickedCheckSpecularImage27();
	afx_msg void OnBnClickedCheckSpecularImage28();
	afx_msg void OnBnClickedCheckSpecularImage29();
	afx_msg void OnBnClickedCheckSpecularImage30();
	afx_msg void OnBnClickedCheckSpecularImage31();
	afx_msg void OnBnClickedCheckSpecularImage32();
	afx_msg void OnBnClickedCheckSpecularImage33();
	afx_msg void OnBnClickedCheckSpecularImage34();
	afx_msg void OnBnClickedCheckSpecularImage35();
	afx_msg void OnBnClickedCheckSpecularImage36();
	afx_msg void OnBnClickedCheckSpecularImage37();
	afx_msg void OnBnClickedCheckSpecularImage38();
	afx_msg void OnBnClickedCheckSpecularImage39();
	afx_msg void OnBnClickedCheckSpecularImage40();
	afx_msg void OnBnClickedCheckSpecularImage41();
	afx_msg void OnBnClickedCheckSpecularImage42();
	afx_msg void OnBnClickedCheckSpecularImage43();
	afx_msg void OnBnClickedCheckSpecularImage44();
	afx_msg void OnBnClickedCheckSpecularImage45();
	afx_msg void OnBnClickedCheckSeqStartImage1();
	afx_msg void OnBnClickedCheckSeqStartImage2();
	afx_msg void OnBnClickedCheckSeqStartImage3();
	afx_msg void OnBnClickedCheckSeqStartImage4();
	afx_msg void OnBnClickedCheckSeqStartImage5();
	afx_msg void OnBnClickedCheckSeqStartImage6();
	afx_msg void OnBnClickedCheckSeqStartImage7();
	afx_msg void OnBnClickedCheckSeqStartImage8();
	afx_msg void OnBnClickedCheckSeqStartImage9();
	afx_msg void OnBnClickedCheckSeqStartImage10();
	afx_msg void OnBnClickedCheckSeqStartImage11();
	afx_msg void OnBnClickedCheckSeqStartImage12();
	afx_msg void OnBnClickedCheckSeqStartImage13();
	afx_msg void OnBnClickedCheckSeqStartImage14();
	afx_msg void OnBnClickedCheckSeqStartImage15();
	afx_msg void OnBnClickedCheckSeqStartImage16();
	afx_msg void OnBnClickedCheckSeqStartImage17();
	afx_msg void OnBnClickedCheckSeqStartImage18();
	afx_msg void OnBnClickedCheckSeqStartImage19();
	afx_msg void OnBnClickedCheckSeqStartImage20();
	afx_msg void OnBnClickedCheckSeqStartImage21();
	afx_msg void OnBnClickedCheckSeqStartImage22();
	afx_msg void OnBnClickedCheckSeqStartImage23();
	afx_msg void OnBnClickedCheckSeqStartImage24();
	afx_msg void OnBnClickedCheckSeqStartImage25();
	afx_msg void OnBnClickedCheckSeqStartImage26();
	afx_msg void OnBnClickedCheckSeqStartImage27();
	afx_msg void OnBnClickedCheckSeqStartImage28();
	afx_msg void OnBnClickedCheckSeqStartImage29();
	afx_msg void OnBnClickedCheckSeqStartImage30();
	afx_msg void OnBnClickedCheckSeqStartImage31();
	afx_msg void OnBnClickedCheckSeqStartImage32();
	afx_msg void OnBnClickedCheckSeqStartImage33();
	afx_msg void OnBnClickedCheckSeqStartImage34();
	afx_msg void OnBnClickedCheckSeqStartImage35();
	afx_msg void OnBnClickedCheckSeqStartImage36();
	afx_msg void OnBnClickedCheckSeqStartImage37();
	afx_msg void OnBnClickedCheckSeqStartImage38();
	afx_msg void OnBnClickedCheckSeqStartImage39();
	afx_msg void OnBnClickedCheckSeqStartImage40();
	afx_msg void OnBnClickedCheckSeqStartImage41();
	afx_msg void OnBnClickedCheckSeqStartImage42();
	afx_msg void OnBnClickedCheckSeqStartImage43();
	afx_msg void OnBnClickedCheckSeqStartImage44();
	afx_msg void OnBnClickedCheckSeqStartImage45();

	void PageChange(int iImageIndex);
	afx_msg void OnCbnSelchangeComboPageIndex1();
	afx_msg void OnCbnSelchangeComboPageIndex2();
	afx_msg void OnCbnSelchangeComboPageIndex3();
	afx_msg void OnCbnSelchangeComboPageIndex4();
	afx_msg void OnCbnSelchangeComboPageIndex5();
	afx_msg void OnCbnSelchangeComboPageIndex6();
	afx_msg void OnCbnSelchangeComboPageIndex7();
	afx_msg void OnCbnSelchangeComboPageIndex8();
	afx_msg void OnCbnSelchangeComboPageIndex9();
	afx_msg void OnCbnSelchangeComboPageIndex10();
	afx_msg void OnCbnSelchangeComboPageIndex11();
	afx_msg void OnCbnSelchangeComboPageIndex12();
	afx_msg void OnCbnSelchangeComboPageIndex13();
	afx_msg void OnCbnSelchangeComboPageIndex14();
	afx_msg void OnCbnSelchangeComboPageIndex15();
	afx_msg void OnCbnSelchangeComboPageIndex16();
	afx_msg void OnCbnSelchangeComboPageIndex17();
	afx_msg void OnCbnSelchangeComboPageIndex18();
	afx_msg void OnCbnSelchangeComboPageIndex19();
	afx_msg void OnCbnSelchangeComboPageIndex20();
	afx_msg void OnCbnSelchangeComboPageIndex21();
	afx_msg void OnCbnSelchangeComboPageIndex22();
	afx_msg void OnCbnSelchangeComboPageIndex23();
	afx_msg void OnCbnSelchangeComboPageIndex24();
	afx_msg void OnCbnSelchangeComboPageIndex25();
	afx_msg void OnCbnSelchangeComboPageIndex26();
	afx_msg void OnCbnSelchangeComboPageIndex27();
	afx_msg void OnCbnSelchangeComboPageIndex28();
	afx_msg void OnCbnSelchangeComboPageIndex29();
	afx_msg void OnCbnSelchangeComboPageIndex30();
	afx_msg void OnCbnSelchangeComboPageIndex31();
	afx_msg void OnCbnSelchangeComboPageIndex32();
	afx_msg void OnCbnSelchangeComboPageIndex33();
	afx_msg void OnCbnSelchangeComboPageIndex34();
	afx_msg void OnCbnSelchangeComboPageIndex35();
	afx_msg void OnCbnSelchangeComboPageIndex36();
	afx_msg void OnCbnSelchangeComboPageIndex37();
	afx_msg void OnCbnSelchangeComboPageIndex38();
	afx_msg void OnCbnSelchangeComboPageIndex39();
	afx_msg void OnCbnSelchangeComboPageIndex40();
	afx_msg void OnCbnSelchangeComboPageIndex41();
	afx_msg void OnCbnSelchangeComboPageIndex42();
	afx_msg void OnCbnSelchangeComboPageIndex43();
	afx_msg void OnCbnSelchangeComboPageIndex44();
	afx_msg void OnCbnSelchangeComboPageIndex45();

	void ChangeLanguage();
	CString strLog;
	CString strMessageBox;
};
