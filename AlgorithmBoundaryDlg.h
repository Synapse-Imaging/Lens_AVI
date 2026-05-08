#pragma once


// CAlgorithmBoundaryDlg 대화 상자입니다.

class CAlgorithmBoundaryDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmBoundaryDlg)

public:
	CAlgorithmBoundaryDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmBoundaryDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_BOUNDARY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

protected:
	CButtonCS m_bnAutoParamCal;
	CButtonCS m_bnScore;

public:
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUseBoundaryInspection;
	int m_iRadioBoundaryEdgeMeasureDir;
	int m_iRadioBoundaryEdgeMeasureGv;
	int m_iRadioBoundaryEdgeMeasurePos;
	double m_dEditBoundaryEdgeMeasureSmFactor;
	double m_dEditBoundaryEdgeMeasureEdgeStr;
	int m_iEditBoundaryEdgeMeasureSampling;
	int m_iEditBoundaryEdgeMeasureSensorLength;
	int m_iRadioBoundaryDisplay;
	BOOL m_bCheckBoundaryCondition[3];
	int m_iEditBoundaryMedianFilterSize[2];
	double m_dEditBoundaryDefectLength[3];
	double m_dEditBoundaryDefectMBLength[2];
	double m_dEditBoundaryDefectWidth[3];
	BOOL m_bCheckBoundaryUseConnection[2];
	int m_iEditBoundaryConnectionLength[2];

	BOOL m_bCheckBoundaryUseRoiConnected;

	afx_msg void OnBnClickedButtonAutoParamCal();

	CString strLog;
	afx_msg void OnBnClickedButtonScore();
};
