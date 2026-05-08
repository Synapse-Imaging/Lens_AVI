#pragma once

#include "ConditionGridCtrl.h"

// CConditionDlg 대화 상자입니다.

class CConditionDlg : public CDialog
{
	DECLARE_DYNAMIC(CConditionDlg)

public:
	CConditionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CConditionDlg();

	CConditionGridCtrl m_Grid;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONDITION_DIALOG };

	virtual BOOL OnInitDialog();

	void ChangeLanguage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
