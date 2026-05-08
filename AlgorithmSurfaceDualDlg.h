#pragma once


// CAlgorithmSurfaceDualDlg 대화 상자입니다.

class CAlgorithmSurfaceDualDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmSurfaceDualDlg)

public:
	CAlgorithmSurfaceDualDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmSurfaceDualDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);
	
	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	HObject		m_HOrgScanImage[CHANNEL_NUM];

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_SURFACE_DUAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUseSurfaceDualInspection;
	CComboBox m_cbSurfaceDualImageIndex_1;
	CComboBox m_cbSurfaceDualImageIndex_2;

	int m_iRadioSurfaceDualDefectType_1;
	int m_iRadioSurfaceDualDefectType_2;

	BOOL m_bCheckUseBrightFixedThres_1;
	int m_iEditBrightLowerThres_1;
	int m_iEditBrightUpperThres_1;
	BOOL m_bCheckUseBrightDT_1;
	int m_iEditBrightMedianFilterSize_1;
	int m_iEditBrightClosingFilterSize_1;
	int m_iRadioBrightDTFilterType_1;
	int m_iEditBrightDTFilterSize_1;
	int m_iEditBrightDTFilterSize2_1;
	int m_iEditBrightDTValue_1;
	BOOL m_bCheckUseDTPolarTrans_1;
	BOOL m_bCheckUseBrightHystThres_1;
	int m_iEditBrightHystSecureThres_1;
	int m_iEditBrightHystPotentialThres_1;
	int m_iEditBrightHystPotentialLength_1;

	BOOL m_bCheckUseBrightFixedThres_2;
	int m_iEditBrightLowerThres_2;
	int m_iEditBrightUpperThres_2;
	BOOL m_bCheckUseBrightDT_2;
	int m_iEditBrightMedianFilterSize_2;
	int m_iEditBrightClosingFilterSize_2;
	int m_iRadioBrightDTFilterType_2;
	int m_iEditBrightDTFilterSize_2;
	int m_iEditBrightDTFilterSize2_2;
	int m_iEditBrightDTValue_2;
	BOOL m_bCheckUseDTPolarTrans_2;
	BOOL m_bCheckUseBrightHystThres_2;
	int m_iEditBrightHystSecureThres_2;
	int m_iEditBrightHystPotentialThres_2;
	int m_iEditBrightHystPotentialLength_2;

	int m_iRadioDualBlobProcessType;
	BOOL m_bCheckDualBlobDefectConditionEach;

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

public:
	afx_msg void OnBnClickedMfcbuttonNextProcessing();
};
