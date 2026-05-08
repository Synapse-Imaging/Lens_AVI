#pragma once


// CAlgorithmStiffenerEpoxyDlg 대화 상자입니다.

class CAlgorithmStiffenerEpoxyDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmStiffenerEpoxyDlg)

public:
	CAlgorithmStiffenerEpoxyDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmStiffenerEpoxyDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_STIFFENER_EPOXY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseStiffenerEpoxy;
	int m_iEditThresholdParam_StiffenerRegion;
	int m_iEditDTFilterX_StiffenerRegion;
	int	m_iEditDTFilterY_StiffenerRegion;
	int m_iEditDTValue_StiffenerRegion;
	int	m_iEditClosing_StiffenerRegion;
	double m_dEditErosionParam_StiffenerBoundary;
	int m_iEditThresholdParam_SheildcanRegion;
	double m_dEditDilationParam_SheildcanBoundary;
	double m_dEditOpeningParam_CornerBoundary;
	afx_msg void OnBnClickedMfcbuttonNextProcessing();
};
