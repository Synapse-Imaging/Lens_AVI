#pragma once


// CAlgorithmDontcareDlg 대화 상자입니다.

class CAlgorithmDontcareDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmDontcareDlg)

public:
	CAlgorithmDontcareDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmDontcareDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_DONTCARE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

public:
	virtual BOOL OnInitDialog();
	
	int m_iRadioProcessChType;
	BOOL m_bCheckUseDC;

	BOOL m_bCheckUseBrightFixedThres;
	int m_iEditBrightLowerThres;
	int m_iEditBrightUpperThres;
	BOOL m_bCheckUseBrightDT;
	int m_iEditBrightMedianFilterSize;
	int m_iEditBrightClosingFilterSize;
	int m_iRadioBrightDTFilterType;
	int m_iEditBrightDTFilterSize;
	int m_iEditBrightDTFilterSize2;
	int m_iEditBrightDTValue;
	BOOL m_bCheckUseBrightUniformityCheck;
	int m_iEditBrightUniformityPeakMin;
	int m_iEditBrightUniformityPeakMax;
	int m_iEditBrightUniformityOffset;
	int m_iEditBrightUniformityHystLength;
	int m_iEditBrightUniformityHystOffset;

	BOOL m_bCheckUseDarkFixedThres;
	int m_iEditDarkLowerThres;
	int m_iEditDarkUpperThres;
	BOOL m_bCheckUseDarkDT;
	int m_iEditDarkMedianFilterSize;
	int m_iEditDarkClosingFilterSize;
	int m_iRadioDarkDTFilterType;
	int m_iEditDarkDTFilterSize;
	int m_iEditDarkDTFilterSize2;
	int m_iEditDarkDTValue;
	BOOL m_bCheckUseDarkUniformityCheck;
	int m_iEditDarkUniformityPeakMin;
	int m_iEditDarkUniformityPeakMax;
	int m_iEditDarkUniformityOffset;
	int m_iEditDarkUniformityHystLength;
	int m_iEditDarkUniformityHystOffset;

	BOOL m_bCheckCombineBrightDarkBlob;
	BOOL m_bCheckMaxBlob;
	int m_iEditOpeningSize;
	int m_iEditClosingSize;
	BOOL m_bCheckFillup;
	BOOL m_bCheckConvex;
	BOOL m_bCheckUseConnection;
	int m_iEditConnectionMinSize;
	int m_iEditConnectionMinXLength;
	int m_iEditConnectionLength;
	BOOL m_bCheckUseAngle;
	int m_iEditConnectionMaxWidth;

	BOOL m_bCheckDefectConditionArea;
	BOOL m_bCheckDefectConditionLength;
	BOOL m_bCheckDefectConditionWidth;
	BOOL m_bCheckDefectConditionXLength;
	BOOL m_bCheckDefectConditionYLength;
	BOOL m_bCheckDefectConditionMean;
	BOOL m_bCheckDefectConditionStdev;
	BOOL m_bCheckDefectConditionAnisometry;
	BOOL m_bCheckDefectConditionCircularity;
	BOOL m_bCheckDefectConditionRectangularity;
	BOOL m_bCheckDefectConditionConvexity;
	BOOL m_bCheckDefectConditionEllipseRatio;
	BOOL m_bCheckDefectConditionAreaRatio;
	BOOL m_bCheckDefectConditionContrast;
	BOOL m_bCheckDefectConditionBlob;
	BOOL m_bCheckDefectConditionShiftX;
	BOOL m_bCheckDefectConditionShiftY;
	BOOL m_bCheckDefectConditionOuterDist;
	BOOL m_bCheckDefectConditionInnerDist;
	BOOL m_bCheckDefectConditionUseZeroArea;
	BOOL m_bCheckDefectConditionBoundaryConnection;
	BOOL m_bCheckDefectConditionBoundaryMarginTop;
	BOOL m_bCheckDefectConditionBoundaryMarginBottom;
	BOOL m_bCheckDefectConditionBoundaryMarginLeft;
	BOOL m_bCheckDefectConditionBoundaryMarginRight;

	int m_iEditDefectConditionAreaMin;
	int m_iEditDefectConditionLengthMin;
	int m_iEditDefectConditionWidthMin;
	int m_iEditDefectConditionXLengthMin;
	int m_iEditDefectConditionYLengthMin;
	int m_iEditDefectConditionMeanMin;
	double m_dEditDefectConditionStdevMin;
	double m_dEditDefectConditionAnisometryMin;
	double m_dEditDefectConditionCircularityMin;
	double m_dEditDefectConditionRectangularityMin;
	double m_dEditDefectConditionConvexityMin;
	double m_dEditDefectConditionEllipseRatioMin;
	int m_iEditDefectConditionAreaRatioMin;
	int m_iEditDefectConditionContrastMin;
	int m_iEditDefectConditionBlobMin;
	int m_iEditDefectConditionShiftXMin;
	int m_iEditDefectConditionShiftYMin;

	int m_iEditDefectConditionAreaMax;
	int m_iEditDefectConditionLengthMax;
	int m_iEditDefectConditionWidthMax;
	int m_iEditDefectConditionXLengthMax;
	int m_iEditDefectConditionYLengthMax;
	int m_iEditDefectConditionMeanMax;
	double m_dEditDefectConditionStdevMax;
	double m_dEditDefectConditionAnisometryMax;
	double m_dEditDefectConditionCircularityMax;
	double m_dEditDefectConditionRectangularityMax;
	double m_dEditDefectConditionConvexityMax;
	double m_dEditDefectConditionEllipseRatioMax;
	int m_iEditDefectConditionAreaRatioMax;
	int m_iEditDefectConditionContrastMax;
	int m_iEditDefectConditionBlobMax;
	int m_iEditDefectConditionShiftXMax;
	int m_iEditDefectConditionShiftYMax;

	int m_iEditDefectConditionOuterDist;
	int m_iEditDefectConditionInnerDist;

	int m_iEditDefectConditionBoundaryMarginTop;
	int m_iEditDefectConditionBoundaryMarginBottom;
	int m_iEditDefectConditionBoundaryMarginLeft;
	int m_iEditDefectConditionBoundaryMarginRight;
	int m_iEditDefectConditionBoundaryMarginMinConnection;

	int m_iEditCandidateMinArea;
	BOOL m_bCheckClosingOpening;
	int m_iRadioLengthWidthCondition;
	int m_iRadioXYLengthCondition;

	int m_iRadioBoundaryConnectionCondition;
	BOOL m_bCheckDefectConditionBoundaryConnection2Side;

	int m_iEditDontcareRegionMargin;

public:
	afx_msg void OnBnClickedButtonAreaMin();
	afx_msg void OnBnClickedButtonAreaMax();
	afx_msg void OnBnClickedButtonLengthMin();
	afx_msg void OnBnClickedButtonLengthMax();
	afx_msg void OnBnClickedButtonWidthMin();
	afx_msg void OnBnClickedButtonWidthMax();
	afx_msg void OnBnClickedButtonMeanMin();
	afx_msg void OnBnClickedButtonMeanMax();
	afx_msg void OnBnClickedButtonStdevMin();
	afx_msg void OnBnClickedButtonStdevMax();
	afx_msg void OnBnClickedButtonAnisometryMin();
	afx_msg void OnBnClickedButtonAnisometryMax();
	afx_msg void OnBnClickedButtonCircularityMin();
	afx_msg void OnBnClickedButtonCircularityMax();
	afx_msg void OnBnClickedButtonRectangularityMin();
	afx_msg void OnBnClickedButtonRectangularityMax();
	afx_msg void OnBnClickedButtonConvexityMin();
	afx_msg void OnBnClickedButtonConvexityMax();
	afx_msg void OnBnClickedButtonEllipseRatioMin();
	afx_msg void OnBnClickedButtonEllipseRatioMax();
	afx_msg void OnBnClickedButtonAreaRatioMin();
	afx_msg void OnBnClickedButtonAreaRatioMax();
	afx_msg void OnBnClickedButtonContrastMin();
	afx_msg void OnBnClickedButtonContrastMax();
	afx_msg void OnBnClickedButtonBlobMin();
	afx_msg void OnBnClickedButtonBlobMax();
	afx_msg void OnBnClickedButtonShiftXMin();
	afx_msg void OnBnClickedButtonShiftXMax();
	afx_msg void OnBnClickedButtonShiftYMin();
	afx_msg void OnBnClickedButtonShiftYMax();
	afx_msg void OnBnClickedButtonXLengthMin();
	afx_msg void OnBnClickedButtonXLengthMax();
	afx_msg void OnBnClickedButtonYLengthMin();
	afx_msg void OnBnClickedButtonYLengthMax();

	afx_msg void OnBnClickedMfcbuttonNextProcessing();
};
