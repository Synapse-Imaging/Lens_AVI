#pragma once


// CAlgorithmTiltDlg 대화 상자입니다.

class CAlgorithmTiltDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmTiltDlg)

public:
	CAlgorithmTiltDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmTiltDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_TILT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseHomerTilt;
	int m_iEditHomerTiltHomerEdgeStr;
	CComboBox m_cbHomerTiltBenvolioImage;
	int m_iEditHomerTiltBenvolioEdgeStr;
	double m_dEditHomerTiltHomerLength;
	double m_dEditHomerTiltTol;
	double m_dEditHomerTiltDistanceTol;

};
