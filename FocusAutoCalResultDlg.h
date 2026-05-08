#pragma once


// CFocusAutoCalResultDlg 대화 상자입니다.

class CFocusAutoCalResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CFocusAutoCalResultDlg)

public:
	CFocusAutoCalResultDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFocusAutoCalResultDlg();

	void UpdateResultView();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FOCUS_AUTOCAL_RESULT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CButtonCS m_bnOK;
	CButtonCS m_bnCancel;

	CStaticCS m_LabelTitle;
	CStaticCS m_LabelTitleZPos;
	CStaticCS m_LabelTitleEdgeValue;
	CStaticCS m_LabelTitleJudge;

	CStaticCS m_LabelTitleTeach[MAX_IMAGE_TAB];
	CStaticCS m_LabelTitleInspect[MAX_IMAGE_TAB];
	CStaticCS m_LabelImageIndex[MAX_IMAGE_TAB];

	CStaticCS m_LabelTeachZPos[MAX_IMAGE_TAB];
	CStaticCS m_LabelTeachEdgeValue[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectZPos[MAX_IMAGE_TAB];
	CStaticCS m_LabelInspectEdgeValue[MAX_IMAGE_TAB];
	CStaticCS m_LabelJudge[MAX_IMAGE_TAB];

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
