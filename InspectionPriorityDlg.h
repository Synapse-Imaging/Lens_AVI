#pragma once

#include "DefectPriorityDlg.h"

// CInspectionPriorityDlg 대화 상자입니다.

class CInspectionPriorityDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectionPriorityDlg)

public:
	CInspectionPriorityDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectionPriorityDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_PRIORITY_INSPECTION_DLG };

	int		m_iPriority[MAX_INSPECTION_TYPE];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnSelchangeComboPriority_1();
	afx_msg void OnSelchangeComboPriority_2();
	afx_msg void OnSelchangeComboPriority_3();
	afx_msg void OnSelchangeComboPriority_4();
	afx_msg void OnSelchangeComboPriority_5();
	afx_msg void OnSelchangeComboPriority_6();
	afx_msg void OnSelchangeComboPriority_7();
	afx_msg void OnSelchangeComboPriority_8();
	afx_msg void OnSelchangeComboPriority_9();
	afx_msg void OnSelchangeComboPriority_10();
	afx_msg void OnSelchangeComboPriority_11();
	afx_msg void OnSelchangeComboPriority_12();
	afx_msg void OnSelchangeComboPriority_13();
	afx_msg void OnSelchangeComboPriority_14();
	afx_msg void OnSelchangeComboPriority_15();
	afx_msg void OnSelchangeComboPriority_16();
	afx_msg void OnSelchangeComboPriority_17();
	afx_msg void OnSelchangeComboPriority_18();
	afx_msg void OnSelchangeComboPriority_19();
	afx_msg void OnSelchangeComboPriority_20();
	afx_msg void OnSelchangeComboPriority_21();
	afx_msg void OnSelchangeComboPriority_22();
	afx_msg void OnSelchangeComboPriority_23();

	afx_msg void OnBnClickedButtonSetDefectPriority1();

	void ChangeLanguage();

protected:
	void ChangePriority(int iSelectedIndex);
};
