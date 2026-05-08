#pragma once


// CAlgorithmCornerMeasureDlg 대화 상자입니다.

class CAlgorithmCornerMeasureDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmCornerMeasureDlg)

public:
	CAlgorithmCornerMeasureDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmCornerMeasureDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_CORNER_MEASURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseCornerMeasure;
	int m_iEditCornerMeasureSaveID;
	int m_iEditCornerMeasureMultiPointNumber;
	int m_iEditCornerMeasurePositionRange;
	int m_iRadioCornerMeasureGv[2];
	int m_iRadioCornerMeasurePos[2];
	double m_dEditCornerMeasureSmFactor[2];
	int m_iEditCornerMeasureEdgeStr[2];
	int m_iEditCornerMeasurePosOffset[2];

	double m_dEditCornerMeasureTargetSmFactor;
	int m_iEditCornerMeasureTargetEdgeStr;
	double m_dEditCornerMeasureTargetDistanceSpec;

};
