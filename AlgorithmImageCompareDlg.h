#pragma once

#define MAX_VM_IMAGE	4

#define NC_TS_SELECT			0
#define NC_TS_SELECT_PART		1
#define NC_CREATE_ROI_POLYGON	2

// CAlgorithmImageCompareDlg 대화 상자입니다.

class CAlgorithmImageCompareDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmImageCompareDlg)

public:
	CAlgorithmImageCompareDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmImageCompareDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	void UpdateVarModelImage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_IMAGE_COMPARE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

	int	 		miToolState;
	POINT		maMIPoints[2];
	POINT		maMVPoints[2];
	int	 		m_iScanImageWidth;
	int	 		m_iScanImageHeight;
	GTRegion	*mpSelectPartTRegion;

	HObject		m_HMeanImage;
	HObject		m_HVarImage;
	HObject		m_HLowerThresImage;
	HObject		m_HUpperThresImage;

	CHWinPanel m_HWinPanelMean;
	CHWinPanel m_HWinPanelVar;
	CHWinPanel m_HWinPanelLowerThres;
	CHWinPanel m_HWinPanelUpperThres;

	CRect				m_ViewRect[MAX_VM_IMAGE];
	POINT				m_ViewOffset[MAX_VM_IMAGE];
	GViewportManager	mViewportManager;
	double				m_dFitZoomRatio;

	enum eVarImage { eMEAN = 0, eSTDEV, eLTHRES, eUTHRES };
	eVarImage m_enumVmImage;

	CMenu				m_Menu;

	CPoint m_ptPrev;
	BOOL   m_bFirst;

	// Polygon
	BOOL m_bPolygonMode;
	int m_iPolygonCnt;
	POINT m_PolygonPt[100];
	void DisplayCross(HDC hDestDC, GViewportManager* pVManager, double dXPos, double dYPos, COLORREF Color, long lRadiusPxl = 5);
	void AddPolygonROI();
	GTRegion* mpActiveTRegion;

	BOOL m_bDisplayInspectImage;
	HObject m_HInspectImage;
	HObject m_HInspectDomainImage;
	HObject m_HInspectDomainRgn;
	void SetInspectImage();
	HObject GetDefectRgn(BOOL bApplyCondition, CAlgorithmParam AlgorithmParam, double dAbsThrU, double dVarThrU, double dAbsThrL, double dVarThrL, BOOL bUseBright, BOOL bUseDark);

protected:
	void DrawSelectPartTRegion();
	void SetViewportManager();
	void UpdateViewportManager();
	void ClearViewWindow();

public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseImageCompare;
	BOOL m_bCheckImageCompareSaveFeatureLog;
	BOOL m_bCheckUseWindow;
	int m_iEditWindowSize;
	BOOL m_bCheckUseImageCompareEdgeImage;

	BOOL m_bCheckUseImageCompareNormalizer;
	int m_iEditImageCompareNormalizeWeight;

	BOOL m_bCheckUseImageCompareBright;
	BOOL m_bCheckUseImageCompareDark;
	int m_iEditImageCompareBrightAbs;
	int m_iEditImageCompareDarkAbs;
	double m_dEditImageCompareBrightVar;
	double m_dEditImageCompareDarkVar;
	BOOL m_bCheckUseAutoTrain;

	BOOL m_bCheckUseImageCompareMod;
	BOOL m_bCheckUseImageCompareModBright;
	BOOL m_bCheckUseImageCompareModDark;
	int m_iEditImageCompareModBrightAbs;
	int m_iEditImageCompareModDarkAbs;
	double m_dEditImageCompareModBrightVar;
	double m_dEditImageCompareModDarkVar;
	int m_iEditImageCompareModTop;
	int m_iEditImageCompareModBottom;
	int m_iEditImageCompareModLeft;
	int m_iEditImageCompareModRight;

	afx_msg void OnBnClickedButtonTrainCurrentImage();
	afx_msg void OnBnClickedButtonShowModelImage();
	afx_msg void OnBnClickedButtonDeleteModel();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedMfcbuttonNextProcessing();
	afx_msg void OnBnClickedButtonSaveModel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg void OnNcWidthfit();
	afx_msg void OnNc100();
	afx_msg void OnNcSelectPart();
	afx_msg void OnNcChangeImage();
	afx_msg void OnNcDefectMasking();
	afx_msg void OnNcDeleteDefectMasking();
	afx_msg void OnNcParamOpt();
	afx_msg void OnNcParamSet();

	CString strLog;
	afx_msg void OnBnClickedButtonShowNormalizeImage();
};
