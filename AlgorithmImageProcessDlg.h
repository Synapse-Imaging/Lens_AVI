#pragma once


// CAlgorithmImageProcessDlg 대화 상자입니다.

class CAlgorithmImageProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmImageProcessDlg)

public:
	CAlgorithmImageProcessDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmImageProcessDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	HObject m_HImageProcessImage[CHANNEL_NUM][MAX_IMAGE_TAB];

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_IMAGE_PROCESS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CButtonCS m_bnDisplayProcessImage;

	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

public:
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUseImageProcess;
	int m_iRadioImageProcessArithmeticType;
	CComboBox m_cbImageProcessArithmeticImage1;
	CComboBox m_cbImageProcessArithmeticImage2;
	CComboBox m_cbImageProcessArithmeticImage3;
	CComboBox m_cbImageProcessArithmeticImage4;
	int m_iEditImageProcessArithmeticAdd;
	double m_dEditImageProcessArithmeticMulti;
	afx_msg void OnBnClickedButtonDisplayProcessImage();
	int m_iRadioProcessChType;
	int m_iRadioProcessResType;
	afx_msg void OnBnClickedRadioProcessImageR();
	afx_msg void OnBnClickedRadioProcessImageG();
	afx_msg void OnBnClickedRadioProcessImageB();
	afx_msg void OnBnClickedRadioProcessImageH();
	afx_msg void OnBnClickedRadioProcessImageS();
	afx_msg void OnBnClickedRadioProcessImageI();

	BOOL m_bCheckUseImageProcessLocalAlign;
	CComboBox m_cbImageProcessLocalAlignImageIndex;
	int m_iEditImageProcessLocalAlignROINo;
	BOOL m_bCheckImageProcessLocalAlignPosX;
	BOOL m_bCheckImageProcessLocalAlignPosY;
	BOOL m_bCheckImageProcessLocalAlignAngle;
	int m_iRadioImageProcessLocalAlignFitPos;
	int m_iRadioImageProcessLocalAlignROIType;
	int m_iRadioImageProcessLocalAlignMatchingApplyMethod;
	BOOL m_bCheckUseImageProcessFilter;
	CComboBox m_cbImageProcessFilterType1;
	CComboBox m_cbImageProcessFilterType2;
	CComboBox m_cbImageProcessFilterType3;
	double m_iEditImageProcessFilterType1X;
	double m_iEditImageProcessFilterType2X;
	double m_iEditImageProcessFilterType3X;
	double m_iEditImageProcessFilterType1Y;
	double m_iEditImageProcessFilterType2Y;
	double m_iEditImageProcessFilterType3Y;
	BOOL m_bCheckUseImageProcessDontCare;
	CComboBox m_cbImageProcessDontCareImageIndex;
	int m_iEditImageProcessDontCareROINo;
	int m_iEditImageProcessDontCareMargin;
	int m_iEditImageProcessDontCareMarginInner;
	BOOL m_bCheckUseImageProcessDontCare2;
	CComboBox m_cbImageProcessDontCareImageIndex2;
	int m_iEditImageProcessDontCareROINo2;
	int m_iEditImageProcessDontCareMargin2;
	int m_iEditImageProcessDontCareMargin2Inner;
	BOOL m_bCheckUseImageProcessGenerate;
	BOOL m_bCheckUseImageProcessGenerateInspectFlag;
	CComboBox m_cbImageProcessGenerateImageIndex;
	int m_iEditImageProcessGenerateROINo;
	int m_iEditImageProcessGenerateMargin;
	BOOL m_bCheckUseImageProcessMask;
	CComboBox m_cbImageProcessMaskImageIndex;
	int m_iEditImageProcessMaskROINo;
	BOOL m_bCheckUseImageProcessMask2;
	CComboBox m_cbImageProcessMaskImageIndex2;
	int m_iEditImageProcessMaskROINo2;

	BOOL m_bCheckUseImageProcessLocalAlignAddLineFit;
	CComboBox m_cbImageProcessLocalAlignImageIndex_2;
	int m_iEditImageProcessLocalAlignROINo_2;
	int m_iRadioImageProcessLocalAlignFitPos_2;

	BOOL m_bCheckUseImageProcessPostLocalAlign;
	CComboBox m_cbImageProcessPostLocalAlignImageIndex;
	int m_iEditImageProcessPostLocalAlignROINo;
	int m_iRadioImageProcessPostLocalAlignFitPos;
};
