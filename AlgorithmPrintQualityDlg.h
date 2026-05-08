#pragma once


// CAlgorithmPrintQualityDlg 대화 상자입니다.

class CAlgorithmPrintQualityDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmPrintQualityDlg)

public:
	CAlgorithmPrintQualityDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmPrintQualityDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_PRINT_QUALITY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUsePrintQuality;

	CComboBox m_cbPrintQualityStrictModel;
	CComboBox m_cbPrintQualityPolarity;
	CComboBox m_cbPrintQualityMirrored;

	int m_iEditPrintQualityContrastMin;

	CComboBox m_cbPrintQualityContrastTolerance;
	CComboBox m_cbPrintQualityStrictQuietZone;

	int m_iEditPrintQualitySymbolColsMin;
	int m_iEditPrintQualitySymbolRowsMin;
	int m_iEditPrintQualitySymbolColsMax;
	int m_iEditPrintQualitySymbolRowsMax;

	CComboBox m_cbPrintQualitySymbolShape;

	int m_iEditPrintQualityModuleSizeMin;
	int m_iEditPrintQualityModuleSizeMax;

	CComboBox m_cbPrintQualitySmallModulesRobustness;
	CComboBox m_cbPrintQualityModuleGrid;
	CComboBox m_cbPrintQualityGapMin;
	CComboBox m_cbPrintQualityGapMax;
	CComboBox m_cbPrintQualityFinderPatternTolerance;

	double m_dEditPrintQualitySlantMax;

	BOOL m_bCheckPrintQualityOverallQuality;
	int m_iEditPrintQualityGradeOverallQuality;
	BOOL m_bCheckPrintQualityUnusedErrorCorrection;
	int m_iEditPrintQualityGradeUnusedErrorCorrection;

};
