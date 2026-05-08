#pragma once


// CAlgorithmBarcodeDlg 대화 상자입니다.

class CAlgorithmBarcodeDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmBarcodeDlg)

public:
	CAlgorithmBarcodeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmBarcodeDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_BARCODE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	BOOL m_bCheckUseBarcode;
	virtual BOOL OnInitDialog();
	double m_dEditBarcodeShiftToleranceX;
	double m_dEditBarcodeShiftToleranceY;
	BOOL m_bCheckInspectBarcodeShift;
	BOOL m_bCheckInspectBarcodeRotation;
	BOOL m_bCheckUseBarcodeOtherImage;
	int m_iEditBarcodeOtherImageNumber;
};
