#pragma once


// CAlgorithmAngleMeasureDlg 대화 상자입니다.

class CAlgorithmAngleMeasureDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmAngleMeasureDlg)

public:
	CAlgorithmAngleMeasureDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmAngleMeasureDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_ANGLE_MEASURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseAngleMeasure;
	int m_iEditAngleMeasureDetectArea11;
	int m_iEditAngleMeasureDetectArea21;
	int m_iEditAngleMeasureDetectArea12;
	int m_iEditAngleMeasureDetectArea22;
	int m_iRadioAngleMeasureDir;
	int m_iRadioAngleMeasureGv1;
	int m_iRadioAngleMeasureGv2;
	int m_iRadioAngleMeasurePos1;
	int m_iRadioAngleMeasurePos2;
	BOOL m_bCheckAngleMeasureLocalAlignUse;
	double m_dEditAngleMeasureSmFactor1;
	double m_dEditAngleMeasureSmFactor2;
	int m_iEditAngleMeasureAngleStr1;
	int m_iEditAngleMeasureAngleStr2;
	int m_iEditAngleMeasurePosOffset1;
	int m_iEditAngleMeasurePosOffset2;
};
