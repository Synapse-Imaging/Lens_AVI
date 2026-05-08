#pragma once

// CGlobalMatchingDlg 대화 상자입니다.

class CGlobalMatchingDlg : public CDialog
{
	DECLARE_DYNAMIC(CGlobalMatchingDlg)

public:
	CGlobalMatchingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGlobalMatchingDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GLOBAL_MATCHING_DLG };

	void LoadViewParam();
	void SetControlState(int iMatchingType);

	BOOL m_bCheckUseMatchingShape;
	BOOL m_bCheckUseMatchingNCC;
	BOOL m_bCheckUseMatchingPerspective;

	int m_iEditMatchingPyramidLevel;
	double m_iEditMatchingAngleRange;
	double m_dEditMatchingScaleMin;
	double m_dEditMatchingScaleMax;
	int m_iEditMatchingTeachingContrast;
	int m_iEditMatchingTeachingContrastLow;
	int m_iEditMatchingTeachingContrastMinSize;
	double m_dEditMatchingScore;
	int m_iEditMatchingInspectionMinContrast;
	int m_iEditMatchingSearchMarginX;
	int m_iEditMatchingSearchMarginY;
	BOOL m_bCheckAlignImage[MAX_IMAGE_TAB];
	int m_iRadioAlignProcessChType;

	HObject m_HOrgTeachingImage;

protected:
	CButtonCS m_bnShowContrast;
	CButtonCS m_bnShowPowImage;
	CButtonCS m_bnChangeParam;
	CButtonCS m_bnChangeAngleComp;
	CButtonCS m_bnCreateGlobalMatchingModel;
	CButtonCS m_bnImageRotation;
	CButtonCS m_bnTestGlobalMatchingModel;
	CButtonCS m_bnAffineImage;
	CButtonCS m_bnChangeXYComp;

	int m_iCurImageIndex;

	int m_iCurrentShapeModel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedButtonCreateGlobalMatchingModel();
	afx_msg void OnBnClickedButtonDeleteGlobalMatchingModel();
	afx_msg void OnBnClickedButtonShowContrast();
		
	afx_msg void OnBnClickedButtonChangeScore();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonImageRotation();
	afx_msg void OnBnClickedButtonTestGlobalMatchingModel();
	afx_msg void OnBnClickedCheckUseShapeMatching();
	afx_msg void OnClickedCheckUseTemplateMatching();
	afx_msg void OnBnClickedCheckUsePerspectiveMatching();
	BOOL m_bCheckUseMatchingFilter;
	CComboBox m_cbMatchingFilterType;
	double m_dEditMatchingFilterTypeXSize;
	double m_dEditMatchingFilterTypeYSize;
	afx_msg void OnBnClickedRadioProcessImageColor();
	afx_msg void OnBnClickedRadioProcessImageR();
	afx_msg void OnBnClickedRadioProcessImageG();
	afx_msg void OnBnClickedRadioProcessImageB();
	afx_msg void OnBnClickedRadioProcessImageH();
	afx_msg void OnBnClickedRadioProcessImageS();
	afx_msg void OnBnClickedRadioProcessImageI();
	afx_msg void OnBnClickedButtonShowPowImage();
	afx_msg void OnBnClickedButtonAffineImage();
	double m_dEditDeltaX;
	double m_dEditDeltaY;
	double m_dEditRotationAngle;

	BOOL m_bCheckUseMatchingAngleComp;
	int m_iRadioMatchingAngleRefLine;
	afx_msg void OnBnClickedButtonChangeAngleComp();

	BOOL m_bCheckUseMatchingAffineConstant;
	afx_msg void OnBnClickedButtonChangeXyComp();
	int m_iLocalIndexNumber;
	BOOL m_bCheckUseMatchingXYComp;
	int m_iRadioMatchingXYRefLine;

	void ChangeLanguage();

	CString strLog;
	CString strMessageBox;
};
