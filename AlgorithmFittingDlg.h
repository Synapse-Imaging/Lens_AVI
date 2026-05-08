#pragma once


// CAlgorithmFittingDlg 대화 상자입니다.

class CAlgorithmFittingDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmFittingDlg)

public:
	CAlgorithmFittingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmFittingDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_FITTING_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseLineFitting;
	int m_iEditLineFitContourConnectionLength;
	int m_iEditLineFitMinContourLength;
	BOOL m_bCheckLineFitUseOtherImageFitting;
	int m_iEditLineFitOtherImageNumber;
	double m_dEditLineFitFittingEdgeSF;
	double m_dEditLineFitObjectEdgeSF;
	BOOL m_bCheckLineFitUseBurInspection;
	double m_dEditLineFitBurDefectLength;
	double m_dEditLineFitBurDefectLengthMax;
	BOOL m_bCheckLineFitUseChippingInspection;
	double m_dEditLineFitChippingDefectLength;
	double m_dEditLineFitChippingDefectLengthMax;
	int m_iEditLineFitObjectLowerThres;
	int m_iEditLineFitObjectUpperThres;
	BOOL m_bCheckLineFitUseRotationInspection;
	int m_iRadioLineFitRotationReference;
	double m_dEditLineFitRotationDefectAngle;
	int m_iRadioLineFitEdgeMethod;
	int m_iEditLineFitFittingEdgeHigh;
	int m_iEditLineFitFittingContourConnectionLength;
	int m_iEditLineFitFittingMinContourLength;
	BOOL m_bCheckLineFitLengthSave;
	BOOL m_bCheckNoUseLineFit;

	int m_iEditLineFitEdgeHigh;
	BOOL m_bCheckLineFitLocalAlignUse;
};
