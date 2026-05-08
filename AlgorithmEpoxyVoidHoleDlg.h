#pragma once


// CAlgorithmEpoxyVoidHoleDlg 대화 상자입니다.

class CAlgorithmEpoxyVoidHoleDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmEpoxyVoidHoleDlg)

public:
	CAlgorithmEpoxyVoidHoleDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmEpoxyVoidHoleDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_EPOXY_VOID_HOLE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseEpoxyVoidHole;

	CComboBox m_cbEpoxyVoidHoleImage;
	double m_dEditEpoxyVoidHoleEdgeStr;
	int m_iEditEpoxyVoidHoleInspectMargin;

	BOOL m_bCheckEpoxyVoidHoleTiltInspect;
	int m_iEditEpoxyVoidHoleTiltTolerance;
	BOOL m_bCheckEpoxyVoidHoleGapInspect;
	int m_iEditEpoxyVoidHoleGapTolerance;
	afx_msg void OnBnClickedMfcbuttonNextProcessing();
};
