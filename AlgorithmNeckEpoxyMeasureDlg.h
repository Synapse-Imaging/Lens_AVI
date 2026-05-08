#pragma once


// CAlgorithmNeckEpoxyMeasureDlg 대화 상자입니다.

class CAlgorithmNeckEpoxyMeasureDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmNeckEpoxyMeasureDlg)

public:
	CAlgorithmNeckEpoxyMeasureDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmNeckEpoxyMeasureDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_NECK_EPOXY_MEASURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseNeckEpoxyMeasure;
	int m_iEditNeckEpoxyMeasureTopEdgeStr;
	int m_iEditNeckEpoxyMeasureBottomEdgeStr;
	int m_iEditNeckEpoxyMeasureRightEdgeStr;

	int m_iEditNeckEpoxyToleranceHeightMax;
	int m_iEditNeckEpoxyToleranceHeightMin;
	int m_iEditNeckEpoxyToleranceLength;
	int m_iEditNeckEpoxyToleranceTopPosOffset;
	int m_iEditNeckEpoxyToleranceBottomPosOffset;

	int m_iRadioNeckEpoxyInspectMethod;

};
