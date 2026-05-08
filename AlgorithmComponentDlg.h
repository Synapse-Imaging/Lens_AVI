#pragma once


// CAlgorithmComponentDlg 대화 상자

class CAlgorithmComponentDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmComponentDlg)

public:
	CAlgorithmComponentDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAlgorithmComponentDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	void CheckParamStatus();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEACH_ALGORITHM_COMPONENT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

	CStaticCS m_LabelDefect_1;
	CStaticCS m_LabelDefect_2;
	CStaticCS m_LabelDefect_3;
	CStaticCS m_LabelDefect_5;

	CStaticCS m_StaticAlignStatus;

public:
	BOOL m_bCheckUseComponent;
	BOOL m_bCheckComponentApply_1;
	BOOL m_bCheckComponentApply_2;
	BOOL m_bCheckComponentApply_3;
	BOOL m_bCheckComponentApply_5;

	double m_dEditComponentScore;
	double m_dEditComponentShiftX;
	double m_dEditComponentShiftY;
	double m_dEditComponentRotationAngle;
	double m_dEditComponentRotationAngle2;
	CComboBox m_cbDefectType_1;
	CComboBox m_cbDefectType_2;
	CComboBox m_cbDefectType_3;
	CComboBox m_cbDefectType_5;

public:
	virtual BOOL OnInitDialog();

};
