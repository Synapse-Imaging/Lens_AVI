#pragma once


// CAlgorithmROIAlignDlg 대화 상자입니다.

class CAlgorithmROIAlignDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmROIAlignDlg)

public:
	CAlgorithmROIAlignDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmROIAlignDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_ROI_ALIGN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUseROIAlign;
	int m_iEditROIAlignXSearchMargin;
	int m_iEditROIAlignYSearchMargin;
	int m_iEditROIAlignMatchingAngleRange;
	double m_dEditROIAlignMatchingScaleMin;
	double m_dEditROIAlignMatchingScaleMax;
	int m_iEditROIAlignMatchingMinContrast;
	BOOL m_bCheckROIAlignUsePosX;
	BOOL m_bCheckROIAlignUsePosY;
	afx_msg void OnBnClickedButtonTestMatchingModel();
	BOOL m_bCheckROIAlignInspectShift;
	int m_iEditROIAlignMatchingPLevel;
	double m_dEditROIAlignShiftTop;
	double m_dEditROIAlignShiftBottom;
	double m_dEditROIAlignShiftLeft;
	double m_dEditROIAlignShiftRight;
	BOOL m_bCheckROIAlignLocalAlignUse;
	afx_msg void OnBnClickedButtonCreateMatchingModel();
	double m_dEditROIAlignMatchingScore;
	BOOL m_bCheckROIColorImage;

	BOOL m_bCheckROIAlignMultiModuleInspect;
	int m_iEditROIAlignMultiModuleXNumber;
	int m_iEditROIAlignMultiModuleYNumber;
	int m_iEditROIAlignMultiModuleXPitch;
	int m_iEditROIAlignMultiModuleYPitch;
	int m_iRadioROIAlignMultiModuleAlignTarget;
	int m_iRadioROIAlignMultiModuleVision;

	afx_msg void OnBnClickedButtonTestMatchingMultiModule();
	afx_msg void OnBnClickedCheckRoiAlignLocalAlignUse();
	afx_msg void OnBnClickedCheckRoiAlignMultiModuleInspect();

	CString strLog;
};
