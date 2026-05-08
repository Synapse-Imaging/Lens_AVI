#pragma once


// CAlgorithmSurfaceDlg 대화 상자입니다.

class CAlgorithmSurfaceDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmSurfaceDlg)

public:
	CAlgorithmSurfaceDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmSurfaceDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);
	
	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	HObject		m_HOrgScanImage[CHANNEL_NUM];

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_SURFACE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUseSurfaceInspection;

	BOOL m_bCheckUseSurfaceMeasureSaveLog;

	BOOL m_bCheckUseSurfaceInspectEachROI;
	int m_iEditSurfaceInspectEachROINgNumber;

	BOOL m_bCheckApplyCircleFitting;
	int m_iEditCircleFittingEdgeStr;
	int m_iEditCircleFittingEdgeMargin;

	BOOL m_bCheckUseDTPolarTrans;

	BOOL m_bCheckUseFTConnected;
	BOOL m_bCheckUseFTConnectedArea;
	BOOL m_bCheckUseFTConnectedLength;
	BOOL m_bCheckUseFTConnectedWidth;
	int m_iEditFTConnectedAreaMin;
	int m_iEditFTConnectedLengthMin;
	int m_iEditFTConnectedWidthMin;
	BOOL m_bCheckUseBarcodeBlob;
	BOOL m_bCheckUseBlobCornerMeasure;
	BOOL m_bCheckUseSaveBNEOverflowTop;
	BOOL m_bCheckUseSaveBNEOverflowBottom;

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
	BOOL m_bCheckUseBrightHystThres;
	int m_iEditBrightHystSecureThres;
	int m_iEditBrightHystPotentialThres;
	int m_iEditBrightHystPotentialLength;

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
	BOOL m_bCheckUseDarkHystThres;
	int m_iEditDarkHystSecureThres;
	int m_iEditDarkHystPotentialThres;
	int m_iEditDarkHystPotentialLength;
	BOOL m_bCheckUseBrightBinThres;
	BOOL m_bCheckUseDarkBinThres;

	BOOL m_bCheckUsePeak;
	int m_iEditNormalPeakThresMin;
	int m_iEditNormalPeakThresMax;
	CString m_sEditPeakValue;
	int m_iEditAbnormalPeakHistogramOffset;

	BOOL m_bCheckUseImageScaling;
	int m_iRadioImageScaleMethod;
	int m_iEditImageScaleMin;
	int m_iEditImageScaleMax;

	BOOL m_bCheckUseEdgeImage;
	int m_iRadioEdgeFilterType;
	double m_dEditEdgeImageScale;

protected:
	CButtonCS m_bnDisplayScaledImage;
	CButtonCS m_bnDisplayEdgeImage;

	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

public:
	afx_msg void OnBnClickedButtonDisplayScaledImage();
	afx_msg void OnBnClickedButtonShowPeak();
	afx_msg void OnBnClickedButtonDisplayEdgeImage();
	afx_msg void OnBnClickedMfcbuttonNextProcessing();

	BOOL m_bCheckUseHSV;
	int miEditLowerHue;
	int miEditUpperHue;
	int miEditLowerSaturation;
	int miEditUpperSaturation;
};
