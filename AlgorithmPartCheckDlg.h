#pragma once


// CAlgorithmPartCheckDlg 대화 상자입니다.

class CAlgorithmPartCheckDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmPartCheckDlg)

public:
	CAlgorithmPartCheckDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmPartCheckDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_PART_CHECK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

	CAlgorithmParam TempAlgoParam;

public:
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUsePartCheck;
	afx_msg void OnBnClickedButtonCreateMatchingModel();
	afx_msg void OnBnClickedButtonTestMatchingModel();
	BOOL m_bCheckPartCheckMultipleModel;
	int m_iEditPartCheckMatchingAngleRange;
	int m_iEditPartCheckMatchingPLevel;
	int m_iEditPartCheckMatchingScore;
	int m_iEditPartCheckMatchingSearchMarginX;
	int m_iEditPartCheckMatchingSearchMarginY;
	int m_iEditPartCheckMatchingSearchMarginX2;
	int m_iEditPartCheckMatchingSearchMarginY2;
	afx_msg void OnBnClickedButtonDeleteMatchingModel();
	BOOL m_bCheckPartCheckModelExist;
	BOOL m_bCheckPartCheckInspectExistence;
	BOOL m_bCheckPartCheckBlob;
	BOOL m_bCheckPartCheckInspectShift;
	int m_iEditPartCheckShiftX;
	int m_iEditPartCheckShiftY;
	double m_dEditPartCheckRotationAngle;
	BOOL m_bCheckPartCheckLocalAlignUse;

	BOOL m_bCheckPartCheckApplyComp;
	BOOL m_bCheckPartCheckSaveLog;

	BOOL m_bCheckPartCheckMeasureEdge;
	int	m_iEditPartCheckLegXSize;
	int	m_iEditPartCheckLegYSize;
	double m_dEditPartCheckEdgeMeasueSmFactor;
	double m_dEditPartCheckEdgeMeasueEdgeStr;
	BOOL m_bCheckPartCheckInspectLegDamage;

	BOOL m_bCheckPartCheckMultiModuleInspect;
	int m_iEditPartCheckMultiModuleXNumber;
	int m_iEditPartCheckMultiModuleYNumber;
	int m_iEditPartCheckMultiModuleXPitch;
	int m_iEditPartCheckMultiModuleYPitch;

	BOOL m_bCheckPartCheckPickerPosUse;
	BOOL m_bCheckPartCheckMultiModuleMoveOffset;
	int m_iRadioPartCheckMultiModuleTarget;

	afx_msg void OnBnClickedButtonTestMatchingMultiModule();
	afx_msg void OnBnClickedButtonMultiModuleMoveOffset();

	CString strLog;
};
