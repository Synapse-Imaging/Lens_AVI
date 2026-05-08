#pragma once


// CAlgorithmConcenterMeasureDlg 대화 상자입니다.

class CAlgorithmConcenterMeasureDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmConcenterMeasureDlg)

public:
	CAlgorithmConcenterMeasureDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmConcenterMeasureDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_CONCENTER_MEASURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUseConcenter;
	//CComboBox m_cbConcenterInnerCircleImageIndex;
	int m_iEditConcenterInnerCircleDiameter;
	double m_dEditConcenterMatchingScaleMin;
	double m_dEditConcenterMatchingScaleMax;
	int m_iEditConcenterMatchingMinContrast;
	//int m_iEditConcenterCenterDiffSpec;
	int m_iEditConcenterMatchingScore;
	double m_dEditConcenterCenterDiffSpecX;
	double m_dEditConcenterCenterDiffSpecY;

	afx_msg void OnBnClickedButtonTestInnerCircle();

	CString strLog;
};
