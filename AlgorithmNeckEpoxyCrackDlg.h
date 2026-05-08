#pragma once


// CAlgorithmNeckEpoxyCrackDlg 대화 상자입니다.

class CAlgorithmNeckEpoxyCrackDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmNeckEpoxyCrackDlg)

public:
	CAlgorithmNeckEpoxyCrackDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmNeckEpoxyCrackDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_NECK_EPOXY_CRACK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseNeckEpoxyCrack;
	int m_iEditNeckEpoxyCrackOpeningSize;
	int m_iEditNeckEpoxyCrackYLength;
	int m_iEditNeckEpoxyCrackBoundaryWidth;
	int m_iRadioNeckEpoxyCrackDir;

	afx_msg void OnBnClickedMfcbuttonNextProcessing2();
	BOOL m_bCheckNeckEpoxyCrackUseXDirMode;
};
