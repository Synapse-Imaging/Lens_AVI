#pragma once


// CAlgorithmConnectorPinDlg 대화 상자입니다.

class CAlgorithmConnectorPinDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmConnectorPinDlg)

public:
	CAlgorithmConnectorPinDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmConnectorPinDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_CONNECTOR_PIN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUsePinLength;

	int m_iRadioPinPos;
	int m_iEditPinDTFilterSize;
	int m_iEditPinOpeningClosingSize;
	int m_iEditPinMinArea;
	int m_iEditPinDistanceTolerance;
	int m_iEditPinDTValue;
	int m_iEditPinDefectNumberTolerance;
	int m_iEditPinPitch;
};
