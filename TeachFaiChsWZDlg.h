#pragma once


// CTeachFaiChsWZDlg 대화 상자

class CTeachFaiChsWZDlg : public CDialog
{
	DECLARE_DYNAMIC(CTeachFaiChsWZDlg)

public:
	CTeachFaiChsWZDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTeachFaiChsWZDlg();

	CButtonCS m_bnOK;
	CButtonCS m_bnCancel;

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEACH_FAI_CHS_WZ_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	BOOL m_bCheckUseFai;
	BOOL m_bCheckInspectFai[MAX_FAI_ITEM];
	double m_dEditNominalFai[MAX_FAI_ITEM];
	double m_dEditToleranceMaxFai[MAX_FAI_ITEM];
	double m_dEditToleranceMinFai[MAX_FAI_ITEM];
	double m_dEditOffsetStg1Fai[MAX_FAI_ITEM];
	double m_dEditTiltStg1Fai[MAX_FAI_ITEM];
	double m_dEditOffsetStg2Fai[MAX_FAI_ITEM];
	double m_dEditTiltStg2Fai[MAX_FAI_ITEM];
	CComboBox m_cbSpecialNGCode[MAX_FAI_ITEM];
};
