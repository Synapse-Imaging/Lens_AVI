#pragma once


// CLightAutoCalResultDlg 대화 상자입니다.

class CLightAutoCalResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightAutoCalResultDlg)

public:
	CLightAutoCalResultDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightAutoCalResultDlg();

	void UpdateResultView();

	void SetMasterGVMode(BOOL bMaster) { m_bMasterGV = bMaster; }

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LIGHT_AUTOCAL_RESULT_DLG };
	static CLightAutoCalResultDlg	*m_pInstance;

public:
	static	CLightAutoCalResultDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void	Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CButtonCS m_bnOK;
	CButtonCS m_bnApplyCalResult;

	CStaticCS m_LabelTitleImage;
	CStaticCS m_LabelTitleValue;
	CStaticCS m_LabelTitleCH1;
	CStaticCS m_LabelTitleCH2;
	CStaticCS m_LabelTitleCH3;
	CStaticCS m_LabelTitleCH4;
	CStaticCS m_LabelTitleCH5;
	CStaticCS m_LabelTitleCH6;
	CStaticCS m_LabelTitleCH7;
	CStaticCS m_LabelTitleCH8;
	CStaticCS m_LabelTitleCH9;
	CStaticCS m_LabelTitleCH10;
	CStaticCS m_LabelTitleTotal;

	CStaticCS m_LabelImage[MAX_IMAGE_TAB];
	CStaticCS m_LabelTitleLV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTitleGV[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachCH1_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachCH1_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH1_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH1_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeCH1[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachCH2_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachCH2_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH2_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH2_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeCH2[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachCH3_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachCH3_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH3_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH3_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeCH3[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachCH4_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachCH4_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH4_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH4_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeCH4[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachCH5_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachCH5_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH5_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH5_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeCH5[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachCH6_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachCH6_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH6_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH6_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeCH6[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachCH7_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachCH7_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH7_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH7_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeCH7[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachCH8_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachCH8_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH8_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH8_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeCH8[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachCH9_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachCH9_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH9_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH9_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeCH9[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachCH10_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachCH10_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH10_LV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectCH10_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeCH10[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachTOTAL_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectTOTAL_GV[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudgeTOTAL[MAX_IMAGE_TAB];

	BOOL m_bMasterGV;

public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApplyCalResult();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
