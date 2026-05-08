#pragma once


// CVmParamSetDlg 대화 상자입니다.

class CVmParamSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CVmParamSetDlg)

public:
	CVmParamSetDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CVmParamSetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VM_PARAM_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

public:
	int m_iEditThrDetect;
	int m_iEditMinPixelRatio;
	int m_iEditAbsThresMin;
	int m_iEditAbsThresStep;
	double m_dEditVarThresMin;
	double m_dEditVarThresStep;
	int m_iEditIouTarget;
};
