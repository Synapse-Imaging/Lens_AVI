#pragma once


// CAlgorithmGapMeasureDlg 대화 상자입니다.

class CAlgorithmGapMeasureDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmGapMeasureDlg)

public:
	CAlgorithmGapMeasureDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmGapMeasureDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_GAP_MEASURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseGapMeasure;
	int m_iRadioGapMeasureUpperGv;
	int m_iRadioGapMeasureUpperPos;
	double m_dEditGapMeasureUpperSmFactor;
	int m_iEditGapMeasureUpperEdgeStr;
	int m_iRadioGapMeasureLowerGv;
	int m_iRadioGapMeasureLowerPos;
	double m_dEditGapMeasureLowerSmFactor;
	int m_iEditGapMeasureLowerEdgeStr;
	int m_iEditGapMeasureUpperPosOffset;
	int m_iEditGapMeasureLowerPosOffset;
//	int m_iEditGapMeasureSaveID;
	double m_dEditGapMeasureGapInspectionUpper;
	double m_dEditGapMeasureGapInspectionLower;
	double m_dEditGapMeasureGapMeasureOffset;
	int m_iRadioGapMeasureDir;
	BOOL m_bCheckGapMeasureAlarm;
	int m_iEditGapMeasurePointNumber;

	int m_iRadioGapMeasureSelectType;
	CComboBox m_cbGapMeasureType;
	CComboBox m_cbGapMeasureImageNo1;
	CComboBox m_cbGapMeasureImageNo2;

	BOOL m_bCheckGapMeasureUpperLine;
	BOOL m_bCheckGapMeasureLowerLine;

	BOOL m_bCheckGapMeasureDiffImage;
	int m_iEditGapMeasurePosStart;
	int m_iEditGapMeasurePosEnd;
	CComboBox m_cbGapMeasureImageNo3;

	BOOL m_bCheckGapMeasureEndRemove;
	int m_iEditGapMeasureRemoveEndUpper;
	int m_iEditGapMeasureRemoveEndLower;

	BOOL m_bCheckGapMeasureRetry;
	CComboBox m_cbGapMeasureRetryImageNo;
	int m_iRadioGapMeasureRetryGv;
	int m_iRadioGapMeasureRetryPos;
	double m_dEditGapMeasureRetrySmFactor;
	int m_iEditGapMeasureRetryEdgeStr;
	int m_iEditGapMeasureRetryPosOffset;

	afx_msg void OnBnClickedRadioGapMeasureDirX();
	afx_msg void OnBnClickedRadioGapMeasureDirY();
};
