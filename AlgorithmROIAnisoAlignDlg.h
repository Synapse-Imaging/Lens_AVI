#pragma once


// CAlgorithmROIAnisoAlignDlg 대화 상자입니다.

class CAlgorithmROIAnisoAlignDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmROIAnisoAlignDlg)

public:
	CAlgorithmROIAnisoAlignDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmROIAnisoAlignDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_ROI_ANISO_ALIGN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUseROIAnisoAlign;
	BOOL m_bCheckROIColorImage;
	int m_iEditROIAnisoAlignXSearchMargin;
	int m_iEditROIAnisoAlignYSearchMargin;
	int m_iEditROIAnisoAlignMatchingAngleRange;
	double m_dEditROIAnisoAlignMatchingScaleMinY;
	double m_dEditROIAnisoAlignMatchingScaleMaxY;
	double m_dEditROIAnisoAlignMatchingScaleMinX;
	double m_dEditROIAnisoAlignMatchingScaleMaxX;
	int m_iEditROIAnisoAlignMatchingMinContrast;
	BOOL m_bCheckROIAnisoAlignUsePosX;
	BOOL m_bCheckROIAnisoAlignUsePosY;
	afx_msg void OnBnClickedButtonTestMatchingModel();
	BOOL m_bCheckROIAnisoAlignInspectShift;
	int m_iEditROIAnisoAlignMatchingPLevel;
	double m_dEditROIAnisoAlignShiftTop;
	double m_dEditROIAnisoAlignShiftBottom;
	double m_dEditROIAnisoAlignShiftLeft;
	double m_dEditROIAnisoAlignShiftRight;
	BOOL m_bCheckROIAnisoAlignLocalAlignUse;
	afx_msg void OnBnClickedButtonCreateMatchingModel();
	double m_dEditROIAnisoAlignMatchingScore;

	BOOL m_bCheckROIAlignMultiModuleInspect;
	int m_iEditROIAlignMultiModuleXNumber;
	int m_iEditROIAlignMultiModuleYNumber;
	int m_iEditROIAlignMultiModuleXPitch;
	int m_iEditROIAlignMultiModuleYPitch;

	int m_iRadioROIAnisoAlignMultiModuleAlignTarget;
	int m_iRadioROIAnisoAlignMultiModuleVision;

	afx_msg void OnBnClickedButtonTestMatchingMultiModule();
	afx_msg void OnBnClickedCheckRoiAlignLocalAlignUse();
	afx_msg void OnBnClickedCheckRoiAnisoAlignMultiModuleInspect();

	CString strLog;
};
