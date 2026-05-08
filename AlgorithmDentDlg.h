#pragma once


// CAlgorithmDentDlg 대화 상자입니다.

class CAlgorithmDentDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmDentDlg)

public:
	CAlgorithmDentDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmDentDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_DENT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckUseDent;
	int m_iEditDentMedFilterSize;
	int m_iEditDentDtFilterSize;
	int m_iEditDentBrightDtValue;
	int m_iEditDentDarkDtValue;
	int m_iEditDentCandidateMinArea;
	int m_iEditDentCandidateMinWidth;
	int m_iEditDentCandidateMinDistance;
	int m_iEditDentMinArea;
};
