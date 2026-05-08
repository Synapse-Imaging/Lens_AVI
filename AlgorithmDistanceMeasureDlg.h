#pragma once


// CAlgorithmDistanceMeasureDlg 대화 상자입니다.

class CAlgorithmDistanceMeasureDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmDistanceMeasureDlg)

public:
	CAlgorithmDistanceMeasureDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmDistanceMeasureDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_DISTANCE_MEASURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseDistanceMeasure;
	int m_iRadioDistanceMeasureUpperGv;
	int m_iRadioDistanceMeasureUpperPos;
	double m_dEditDistanceMeasureUpperSmFactor;
	int m_iEditDistanceMeasureUpperEdgeStr;
	int m_iRadioDistanceMeasureLowerGv;
	int m_iRadioDistanceMeasureLowerPos;
	double m_dEditDistanceMeasureLowerSmFactor;
	int m_iEditDistanceMeasureLowerEdgeStr;
	int m_iEditDistanceMeasureUpperPosOffset;
	int m_iEditDistanceMeasureLowerPosOffset;
	int m_iRadioDistanceMeasureUsageType;
	int m_iEditDistanceMeasureSaveID;
	double m_dEditDistanceMeasureGapInspectionUpper;
	double m_dEditDistanceMeasureGapInspectionLower;
	int m_iRadioDistanceMeasureROIType;
	afx_msg void OnBnClickedRadioDistanceMeasureUsageTypeDistance();
	afx_msg void OnBnClickedRadioDistanceMeasureUsageTypeRoi();
};
