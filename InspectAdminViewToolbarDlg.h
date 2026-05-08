#pragma once

#include "ToolBarCombo.h"

// CInspectAdminViewToolbarDlg 대화 상자입니다.

class CInspectAdminViewToolbarDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectAdminViewToolbarDlg)

public:
	static	CInspectAdminViewToolbarDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();

	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	void Show();

	void SetToolBarStateDraw();
	void UpdateToolbarStatusTeachMode();

	void StartLive();
	void StopLive();
	void SetDisplayChannel(int iDisplayChannel);

	BOOL	m_bLive;

public:
	CInspectAdminViewToolbarDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectAdminViewToolbarDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSPECT_ADMIN_VIEW_TOOLBAR_DLG };

protected:
	// ToolBar
	CToolBarCombo	m_ViewToolbar;

	BOOL	m_bDisplayFitLine;
	BOOL	m_bDisplayInspectArea;
	BOOL	m_bDisplayThresholdArea;
	BOOL	m_bDisplayDefectArea;
	BOOL	m_bROILock;
	BOOL	m_bAlignDisplay;
	int		m_caliper_control_flag;
	BOOL	m_bDispCrossBar;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnTbDraw();
	afx_msg void OnTbMove();
	afx_msg void OnTbSelectPart();
	afx_msg void OnTbROIRect();
	afx_msg void OnTbROICircle();
	afx_msg void OnTbROIPolygon();
	afx_msg void OnTbROICross();
	afx_msg void OnTbROIDelete();
	afx_msg void OnTbZoomIn();
	afx_msg void OnTbZoomOut();
	afx_msg void OnTbFitWidth();
	afx_msg void OnTbFitHeight();
	afx_msg void OnTbLive();
	afx_msg void OnTbDispCrossBar();
	afx_msg void OnTbFitLineDisplay();
	afx_msg void OnTbInspectAreaDisplay();
	afx_msg void OnTbThresholdAreaDisplay();
	afx_msg void OnTbDefectAreaDisplay();
	afx_msg void OnTb2pCaliper();
	afx_msg void OnTbROILock();
	afx_msg void OnTbAlignDisplay();
	afx_msg void OnTbDisplayColor();
	afx_msg void OnTbDisplayR();
	afx_msg void OnTbDisplayG();
	afx_msg void OnTbDisplayB();
	afx_msg void OnTbDisplayH();
	afx_msg void OnTbDisplayS();
	afx_msg void OnTbDisplayI();

	DECLARE_MESSAGE_MAP()

private:
	CRect	m_ScreenRect;
	static CInspectAdminViewToolbarDlg	*m_pInstance;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
};
