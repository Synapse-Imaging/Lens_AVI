#pragma once

#include "ToolBarCombo.h"

// CInspectViewOverayImageToolbarDlg3 대화 상자입니다.

class CInspectViewOverayImageToolbarDlg3 : public CDialog
{
	DECLARE_DYNAMIC(CInspectViewOverayImageToolbarDlg3)

public:
	static	CInspectViewOverayImageToolbarDlg3* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();

	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	void	Show();
	BOOL	ShowWindow(int nCmdShow);

public:
	CInspectViewOverayImageToolbarDlg3(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectViewOverayImageToolbarDlg3();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSPECT_VIEW_OVERAY_IMAGE3_TOOLBAR_DLG };

protected:
	CToolBarCombo	m_ViewToolbar;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnTbDraw();
	afx_msg void OnTbMove();
	afx_msg void OnTbSelectPart();
	afx_msg void OnTbZoomIn();
	afx_msg void OnTbZoomOut();
	afx_msg void OnTbFitWidth();
	afx_msg void OnTbFitHeight();
	DECLARE_MESSAGE_MAP()

private:
	CRect	m_ScreenRect;
	static CInspectViewOverayImageToolbarDlg3	*m_pInstance;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
};
