#pragma once


// CAlgorithmHeightMeasureDlg 대화 상자입니다.

class CAlgorithmHeightMeasureDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmHeightMeasureDlg)

public:
	CAlgorithmHeightMeasureDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmHeightMeasureDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_HEIGHT_MEASURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseHeightMeasure;
	int m_iRadioHeightMeasureType;
};
