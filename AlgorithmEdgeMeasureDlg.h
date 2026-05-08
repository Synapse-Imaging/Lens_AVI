#pragma once


// CAlgorithmEdgeMeasureDlg 대화 상자입니다.

class CAlgorithmEdgeMeasureDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmEdgeMeasureDlg)

public:
	CAlgorithmEdgeMeasureDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmEdgeMeasureDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_EDGE_MEASURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseEdgeMeasure;
	int m_iRadioEdgeMeasureOnePoint;
	int m_iEditEdgeMeasureMultiPointNumber;
	BOOL m_bCheckEdgeMeasureEndPoint;
	BOOL m_bCheckEdgeMeasureSlope;
	BOOL m_bCheckEdgeMeasureReverseGv;
	BOOL m_bCheckEdgeMeasureFixedEndPoint;
	int m_iEditEdgeMeasureLeftSideSize;
	int m_iEditEdgeMeasureRightSideSize;
	int m_iRadioEdgeMeasureDir;
	int m_iRadioEdgeMeasureGv;
	int m_iRadioEdgeMeasurePos;
	BOOL m_bCheckEdgeMeasureLocalAlignUse;
	double m_dEditEdgeMeasureSmFactor;
	int m_iEditEdgeMeasureEdgeStr;
	int m_iEditEdgeMeasurePosOffset;
	afx_msg void OnBnClickedRadioEdgeMeasureOnePoint();
	afx_msg void OnBnClickedRadioEdgeMeasureMultiPoint();
	BOOL m_bCheckEdgeMeasureUseMaxMinPoint;
	int m_iEditEdgeMeasurePositionRange;
	int m_iRadioEdgeMeasureUseMaxPoint;
	afx_msg void OnBnClickedCheckEdgeMeasureEndPoint();

public:
	BOOL m_bCheckEdgeMeasureUseOther;
	BOOL m_bCheckEdgeMeasureMakeROIOther;
	int m_iEditEdgeMeasureMakeROIRangeOther;
	CComboBox m_cbEdgeMeasureImageIndexOther;
	int m_iRadioEdgeMeasureGvOther;
	int m_iRadioEdgeMeasurePosOther;
	double m_dEditEdgeMeasureSmFactorOther;
	int m_iEditEdgeMeasureEdgeStrOther;
	int m_iEditEdgeMeasurePosOffsetOther;

	double m_dEditEdgeMeasureAngleDeg;
	CComboBox m_cbEdgeMeasureAccuracy;
};
