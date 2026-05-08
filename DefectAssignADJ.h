#pragma once
#include "afxwin.h"


// CDefectAssignADJ 대화 상자입니다.

class CDefectAssignADJ : public CDialogEx
{
	DECLARE_DYNAMIC(CDefectAssignADJ)

public:
	CDefectAssignADJ(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDefectAssignADJ();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DEFECT_ASSIGN_ADJ_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	int nClientNO;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CComboBox m_comboClientNO;

	int m_n2ArrayThreadNO[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM];
	CString m_str2ArrayPos[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM];
	CString m_str2ArrayName[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM];
	BOOL    m_bADJOption[ MAX_ADJ_CONNECT_NUM ][ DEEP_MODEL_NUM ];

	CString m_strDefectPos[DEEP_MODEL_VIEW];
	CString m_strDefectName[DEEP_MODEL_VIEW];
	CComboBox m_comboVision[DEEP_MODEL_VIEW];
	BOOL    m_bCheckADJOption[DEEP_MODEL_VIEW];

	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnCbnSelchangeComboClientno();

	void ChangeLanguage();

	CString strDialog;
};
