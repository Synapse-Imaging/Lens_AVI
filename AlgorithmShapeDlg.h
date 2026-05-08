#pragma once


// CAlgorithmShapeDlg 대화 상자입니다.

class CAlgorithmShapeDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmShapeDlg)

public:
	CAlgorithmShapeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmShapeDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_SHAPE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

	HObject		m_HShapeImage;
	HObject		m_HRawImageTop;
	HObject		m_HRawImageBottom;
	HObject		m_HRawImageLeft;
	HObject		m_HRawImageRight;
	CHWinPanel m_HWinPanelShape;
	CHWinPanel m_HWinPanelRawImageTop;
	CHWinPanel m_HWinPanelRawImageBottom;
	CHWinPanel m_HWinPanelRawImageLeft;
	CHWinPanel m_HWinPanelRawImageRight;

	CLabelCS m_StaticShapeImageTitle;
	CLabelCS m_StaticRawImageTopTitle;
	CLabelCS m_StaticRawImageBottomTitle;
	CLabelCS m_StaticRawImageLeftTitle;
	CLabelCS m_StaticRawImageRightTitle;

	CButtonCS m_bnShowShapeImage;
	CButtonCS m_bnSaveShapeImage;

public:
	BOOL m_bCheckUseShape;
	virtual BOOL OnInitDialog();
	int m_iEditShapeIlluminationFilterX;
	int m_iEditShapeIlluminationFilterY;
	double m_dEditShapeIlluminationScaleFactor;
	double m_dEditShapeCurvatureSmFactor;
	afx_msg void OnBnClickedButtonShowShapeImage();
	afx_msg void OnPaint();
	double m_dEditShapeScalingMulti;
	double m_dEditShapeScalingAdd;
	double m_dEditShapeImageReduceRatio;
	double m_dEditShapeLedAngle;
	int m_iEditShapeImageMargin;
	afx_msg void OnBnClickedMfcbuttonNextProcessing2();
	afx_msg void OnBnClickedMfcbuttonNextProcessing3();
	afx_msg void OnBnClickedButtonSaveShapeImage();

	CString strLog;
};
