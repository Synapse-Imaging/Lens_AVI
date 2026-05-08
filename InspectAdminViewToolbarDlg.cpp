// InspectAdminViewToolbarDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectAdminViewToolbarDlg.h"
#include "afxdialogex.h"


// CInspectAdminViewToolbarDlg 대화 상자입니다.

CInspectAdminViewToolbarDlg* CInspectAdminViewToolbarDlg::m_pInstance = NULL;

CInspectAdminViewToolbarDlg* CInspectAdminViewToolbarDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectAdminViewToolbarDlg();
		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_ADMIN_VIEW_TOOLBAR_DLG, pFrame->GetActiveView());
			if (bShowFlag)
				m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectAdminViewToolbarDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectAdminViewToolbarDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CInspectAdminViewToolbarDlg, CDialog)

CInspectAdminViewToolbarDlg::CInspectAdminViewToolbarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectAdminViewToolbarDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP, VIEW1_DLG1_WIDTH + 280, VIEW_TOOLBAR_HEIGHT - 4);

	m_bDisplayFitLine = TRUE;
	m_bDisplayInspectArea = TRUE;
	m_bDisplayThresholdArea = TRUE;
	m_bDisplayDefectArea = TRUE;
	m_bROILock = TRUE;
	m_bAlignDisplay = TRUE;

	m_caliper_control_flag = 0;

	m_bLive = FALSE;
	m_bDispCrossBar = FALSE;
}

CInspectAdminViewToolbarDlg::~CInspectAdminViewToolbarDlg()
{
}

void CInspectAdminViewToolbarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInspectAdminViewToolbarDlg, CDialog)
	ON_COMMAND(ID_TB_DRAW, OnTbDraw)
	ON_COMMAND(ID_TB_MOVE, OnTbMove)
	ON_COMMAND(ID_TB_SELECT_PART, OnTbSelectPart)
	ON_COMMAND(ID_TB_ROI_RECT, OnTbROIRect)
	ON_COMMAND(ID_TB_ROI_CIRCLE, OnTbROICircle)
	ON_COMMAND(ID_TB_ROI_POLYGON, OnTbROIPolygon)
	ON_COMMAND(ID_TB_ROI_CROSS, OnTbROICross)
	ON_COMMAND(ID_TB_ROI_DELETE, OnTbROIDelete)
	ON_COMMAND(ID_TB_ZOOM_IN, OnTbZoomIn)
	ON_COMMAND(ID_TB_ZOOM_OUT, OnTbZoomOut)
	ON_COMMAND(ID_TB_FIT_WIDTH, OnTbFitWidth)
	ON_COMMAND(ID_TB_FIT_HEIGHT, OnTbFitHeight)
	ON_COMMAND(ID_TB_LIVE, OnTbLive)
	ON_COMMAND(ID_TB_CROSS_BAR, OnTbDispCrossBar)
	ON_COMMAND(ID_TB_FIT_LINE_DISP, OnTbFitLineDisplay)
	ON_COMMAND(ID_TB_ROI_DISP, OnTbInspectAreaDisplay)
	ON_COMMAND(ID_TB_THRES_DISP, OnTbThresholdAreaDisplay)
	ON_COMMAND(ID_TB_DEFECT_DISP, OnTbDefectAreaDisplay)
	ON_COMMAND(ID_TB_CALIPER, OnTb2pCaliper)
	ON_COMMAND(ID_TB_ROI_LOCK, OnTbROILock)
	ON_COMMAND(ID_TB_ALIGN_DISP, OnTbAlignDisplay)
	ON_COMMAND(ID_TB_COLOR_IMAGE, OnTbDisplayColor)
	ON_COMMAND(ID_TB_R_IMAGE, OnTbDisplayR)
	ON_COMMAND(ID_TB_G_IMAGE, OnTbDisplayG)
	ON_COMMAND(ID_TB_B_IMAGE, OnTbDisplayB)
	ON_COMMAND(ID_TB_H_IMAGE, OnTbDisplayH)
	ON_COMMAND(ID_TB_S_IMAGE, OnTbDisplayS)
	ON_COMMAND(ID_TB_I_IMAGE, OnTbDisplayI)
END_MESSAGE_MAP()


// CInspectAdminViewToolbarDlg 메시지 처리기입니다.

BOOL CInspectAdminViewToolbarDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_NCLBUTTONDOWN:
		SetActiveWindow();
		return TRUE;
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN) return TRUE;
		break;
	case WM_SYSKEYDOWN:
		if (pMsg->wParam == VK_F4) return TRUE;
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CInspectAdminViewToolbarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CRect ToolbarBorder(3, 3, 3, 3);
	if (!m_ViewToolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_BORDER | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS, ToolbarBorder) ||
		!m_ViewToolbar.LoadToolBar(IDR_ADMIN_VIEW_TOOLBAR)) {
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CRect    rcClientOld; // Old Client Rect
	CRect    rcClientNew; // New Client Rect with Tollbar Added
	GetClientRect(rcClientOld); // Retrive the Old Client WindowSize
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNew);

	int iToolBarOffset;
	iToolBarOffset = rcClientNew.top - rcClientOld.top;

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_ViewToolbar.SetButtonStyle(0, TBBS_CHECKGROUP);		// Selection
	m_ViewToolbar.SetButtonStyle(1, TBBS_CHECKGROUP);		// Grab
	m_ViewToolbar.SetButtonStyle(2, TBBS_CHECKGROUP);		// Selected Zoom
	m_ViewToolbar.SetButtonStyle(3, TBBS_CHECKGROUP);		// Rect ROI
	m_ViewToolbar.SetButtonStyle(4, TBBS_CHECKGROUP);		// Circle ROI
	m_ViewToolbar.SetButtonStyle(5, TBBS_CHECKGROUP);		// Polygon ROI
	m_ViewToolbar.SetButtonStyle(6, TBBS_CHECKGROUP);		// Cross ROI

	m_ViewToolbar.SetButtonStyle(16, TBBS_CHECKBOX);		// Live
	m_ViewToolbar.SetButtonStyle(17, TBBS_CHECKBOX);		// CrossBar

	m_ViewToolbar.SetButtonStyle(19, TBBS_CHECKBOX);		// ROI
	m_ViewToolbar.SetButtonStyle(20, TBBS_CHECKBOX);		// Margin
	m_ViewToolbar.SetButtonStyle(21, TBBS_CHECKBOX);		// Threshold
	m_ViewToolbar.SetButtonStyle(22, TBBS_CHECKBOX);		// NG

	m_ViewToolbar.SetButtonStyle(25, TBBS_CHECKBOX);		// ROI Lock
	m_ViewToolbar.SetButtonStyle(26, TBBS_CHECKBOX);		// Align Display

	m_ViewToolbar.SetButtonStyle(28, TBBS_CHECKGROUP);		// RGB
	m_ViewToolbar.SetButtonStyle(29, TBBS_CHECKGROUP);		// R
	m_ViewToolbar.SetButtonStyle(30, TBBS_CHECKGROUP);		// G
	m_ViewToolbar.SetButtonStyle(31, TBBS_CHECKGROUP);		// B
	m_ViewToolbar.SetButtonStyle(32, TBBS_CHECKGROUP);		// H
	m_ViewToolbar.SetButtonStyle(33, TBBS_CHECKGROUP);		// S
	m_ViewToolbar.SetButtonStyle(34, TBBS_CHECKGROUP);		// I

	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	toolbarctrl.SetState(ID_TB_ROI_RECT, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_CIRCLE, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_POLYGON, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_CROSS, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_DELETE, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_LIVE, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_CROSS_BAR, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_FIT_LINE_DISP, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_DISP, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_THRES_DISP, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_DEFECT_DISP, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_CALIPER, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_LOCK, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ALIGN_DISP, TBSTATE_ENABLED);

	toolbarctrl.CheckButton(ID_TB_SELECT_PART, TRUE);

	toolbarctrl.CheckButton(ID_TB_LIVE, FALSE);
	toolbarctrl.CheckButton(ID_TB_CROSS_BAR, FALSE);
	toolbarctrl.CheckButton(ID_TB_FIT_LINE_DISP, m_bDisplayFitLine);
	toolbarctrl.CheckButton(ID_TB_ROI_DISP, m_bDisplayInspectArea);
	toolbarctrl.CheckButton(ID_TB_THRES_DISP, m_bDisplayThresholdArea);
	toolbarctrl.CheckButton(ID_TB_DEFECT_DISP, m_bDisplayDefectArea);
	toolbarctrl.CheckButton(ID_TB_CALIPER, FALSE);
	toolbarctrl.CheckButton(ID_TB_ROI_LOCK, m_bROILock);
	toolbarctrl.CheckButton(ID_TB_ALIGN_DISP, m_bAlignDisplay);

	toolbarctrl.CheckButton(ID_TB_COLOR_IMAGE, TRUE);

	m_caliper_control_flag = 0;

	m_bLive = FALSE;
	m_bDispCrossBar = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CInspectAdminViewToolbarDlg::SetToolBarStateDraw()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	toolbarctrl.CheckButton(ID_TB_DRAW, TRUE);
	toolbarctrl.CheckButton(ID_TB_MOVE, FALSE);
	toolbarctrl.CheckButton(ID_TB_SELECT_PART, FALSE);
	toolbarctrl.CheckButton(ID_TB_ROI_RECT, FALSE);
	toolbarctrl.CheckButton(ID_TB_ROI_CIRCLE, FALSE);
	toolbarctrl.CheckButton(ID_TB_ROI_POLYGON, FALSE);
	toolbarctrl.CheckButton(ID_TB_ROI_CROSS, FALSE);

	THEAPP.m_pInspectAdminViewDlg->SetToolBarStateDraw();
}


void CInspectAdminViewToolbarDlg::OnTbDraw()
{
	THEAPP.m_pInspectAdminViewDlg->TbDraw();
}

void CInspectAdminViewToolbarDlg::OnTbMove()
{
	THEAPP.m_pInspectAdminViewDlg->TbMove();
}

void CInspectAdminViewToolbarDlg::OnTbSelectPart()
{
	THEAPP.m_pInspectAdminViewDlg->TbSelectPart();
}

void CInspectAdminViewToolbarDlg::OnTbROIRect()
{
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	if (m_caliper_control_flag == 1)
	{
		CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
		toolbarctrl.CheckButton(ID_TB_CALIPER, FALSE);

		m_caliper_control_flag = 0;
	}

	THEAPP.m_pInspectAdminViewDlg->TbROIRect();
}

void CInspectAdminViewToolbarDlg::OnTbROICircle()
{
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	if (m_caliper_control_flag == 1)
	{
		CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
		toolbarctrl.CheckButton(ID_TB_CALIPER, FALSE);

		m_caliper_control_flag = 0;
	}

	THEAPP.m_pInspectAdminViewDlg->TbROICircle();
}

void CInspectAdminViewToolbarDlg::OnTbROIPolygon()
{
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	if (m_caliper_control_flag == 1)
	{
		CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
		toolbarctrl.CheckButton(ID_TB_CALIPER, FALSE);

		m_caliper_control_flag = 0;
	}

	THEAPP.m_pInspectAdminViewDlg->TbROIPolygon();
}

void CInspectAdminViewToolbarDlg::OnTbROICross()
{
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	if (m_caliper_control_flag == 1)
	{
		CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
		toolbarctrl.CheckButton(ID_TB_CALIPER, FALSE);

		m_caliper_control_flag = 0;
	}

	THEAPP.m_pInspectAdminViewDlg->TbROICross();
}

void CInspectAdminViewToolbarDlg::OnTbROIDelete()
{
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	THEAPP.m_pInspectAdminViewDlg->TbROIDelete();
}

void CInspectAdminViewToolbarDlg::OnTbZoomIn()
{
	THEAPP.m_pInspectAdminViewDlg->TbZoomIn();
}

void CInspectAdminViewToolbarDlg::OnTbZoomOut()
{
	THEAPP.m_pInspectAdminViewDlg->TbZoomOut();
}

void CInspectAdminViewToolbarDlg::OnTbFitWidth()
{
	THEAPP.m_pInspectAdminViewDlg->TbFitWidth();
}

void CInspectAdminViewToolbarDlg::OnTbFitHeight()
{
	THEAPP.m_pInspectAdminViewDlg->TbFitHeight();
}

void CInspectAdminViewToolbarDlg::OnTbLive()
{
#ifdef INLINE_MODE

	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	if (m_bLive == FALSE)
	{
		toolbarctrl.CheckButton(ID_TB_LIVE, TRUE);
		m_bLive = TRUE;
	}
	else
	{
		toolbarctrl.CheckButton(ID_TB_LIVE, FALSE);
		m_bLive = FALSE;
	}

	THEAPP.m_pInspectAdminViewDlg->TbLive();
#endif
}

void CInspectAdminViewToolbarDlg::OnTbDispCrossBar()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	if (m_bDispCrossBar == FALSE)
	{
		toolbarctrl.CheckButton(ID_TB_CROSS_BAR, TRUE);
		m_bDispCrossBar = TRUE;
	}
	else
	{
		toolbarctrl.CheckButton(ID_TB_CROSS_BAR, FALSE);
		m_bDispCrossBar = FALSE;
	}

	THEAPP.m_pInspectAdminViewDlg->TbDispCrossBar();
}

void CInspectAdminViewToolbarDlg::OnTbFitLineDisplay()
{
	if (m_bDisplayFitLine)
		m_bDisplayFitLine = FALSE;
	else
		m_bDisplayFitLine = TRUE;

	THEAPP.m_pInspectAdminViewDlg->TbFitLineDisplay();
}

void CInspectAdminViewToolbarDlg::OnTbInspectAreaDisplay()
{
	if (m_bDisplayInspectArea)
		m_bDisplayInspectArea = FALSE;
	else
		m_bDisplayInspectArea = TRUE;

	THEAPP.m_pInspectAdminViewDlg->TbInspectAreaDisplay();
}

void CInspectAdminViewToolbarDlg::OnTbThresholdAreaDisplay()
{
	if (m_bDisplayThresholdArea)
		m_bDisplayThresholdArea = FALSE;
	else
		m_bDisplayThresholdArea = TRUE;

	THEAPP.m_pInspectAdminViewDlg->TbThresholdAreaDisplay();
}

void CInspectAdminViewToolbarDlg::OnTbDefectAreaDisplay()
{
	if (m_bDisplayDefectArea)
		m_bDisplayDefectArea = FALSE;
	else
		m_bDisplayDefectArea = TRUE;

	THEAPP.m_pInspectAdminViewDlg->TbDefectAreaDisplay();
}

void CInspectAdminViewToolbarDlg::OnTb2pCaliper()
{
	// TODO: Add your command handler code here
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	if (m_caliper_control_flag == 0)
	{
		toolbarctrl.CheckButton(ID_TB_CALIPER, TRUE);
		toolbarctrl.CheckButton(ID_TB_DRAW, TRUE);
		toolbarctrl.CheckButton(ID_TB_MOVE, FALSE);
		toolbarctrl.CheckButton(ID_TB_SELECT_PART, FALSE);

		m_caliper_control_flag = 1;
	}
	else if (m_caliper_control_flag == 1)
	{
		toolbarctrl.CheckButton(ID_TB_CALIPER, FALSE);

		m_caliper_control_flag = 0;
	}

	THEAPP.m_pInspectAdminViewDlg->Tb2pCaliper();
}

void CInspectAdminViewToolbarDlg::OnTbROILock()
{
	THEAPP.m_pInspectAdminViewDlg->TbROILock();
}

void CInspectAdminViewToolbarDlg::OnTbAlignDisplay()
{
	THEAPP.m_pInspectAdminViewDlg->TbAlignDisplay();
}

void CInspectAdminViewToolbarDlg::OnTbDisplayColor()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_R_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_G_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_B_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_H_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_S_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_I_IMAGE, FALSE);

	THEAPP.m_pInspectAdminViewDlg->TbDisplayColor();
}

void CInspectAdminViewToolbarDlg::OnTbDisplayR()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_COLOR_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_G_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_B_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_H_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_S_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_I_IMAGE, FALSE);

	THEAPP.m_pInspectAdminViewDlg->TbDisplayR();
}

void CInspectAdminViewToolbarDlg::OnTbDisplayG()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_COLOR_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_R_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_B_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_H_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_S_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_I_IMAGE, FALSE);

	THEAPP.m_pInspectAdminViewDlg->TbDisplayG();
}

void CInspectAdminViewToolbarDlg::OnTbDisplayB()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_COLOR_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_R_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_G_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_H_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_S_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_I_IMAGE, FALSE);

	THEAPP.m_pInspectAdminViewDlg->TbDisplayB();
}

void CInspectAdminViewToolbarDlg::OnTbDisplayH()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_COLOR_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_R_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_G_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_B_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_S_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_I_IMAGE, FALSE);

	THEAPP.m_pInspectAdminViewDlg->TbDisplayH();
}

void CInspectAdminViewToolbarDlg::OnTbDisplayS()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_COLOR_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_R_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_G_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_B_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_H_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_I_IMAGE, FALSE);

	THEAPP.m_pInspectAdminViewDlg->TbDisplayS();
}

void CInspectAdminViewToolbarDlg::OnTbDisplayI()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_COLOR_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_R_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_G_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_B_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_H_IMAGE, FALSE);
	toolbarctrl.CheckButton(ID_TB_S_IMAGE, FALSE);

	THEAPP.m_pInspectAdminViewDlg->TbDisplayI();
}

void CInspectAdminViewToolbarDlg::UpdateToolbarStatusTeachMode()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	toolbarctrl.SetState(ID_TB_ROI_RECT, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_CIRCLE, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_POLYGON, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_CROSS, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_DELETE, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_LIVE, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_CROSS_BAR, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_FIT_LINE_DISP, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_DISP, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_THRES_DISP, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_DEFECT_DISP, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_CALIPER, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ROI_LOCK, TBSTATE_ENABLED);
	toolbarctrl.SetState(ID_TB_ALIGN_DISP, TBSTATE_ENABLED);

	toolbarctrl.CheckButton(ID_TB_LIVE, FALSE);
	m_bLive = FALSE;
	toolbarctrl.CheckButton(ID_TB_CROSS_BAR, FALSE);
	m_bDispCrossBar = FALSE;
	toolbarctrl.CheckButton(ID_TB_FIT_LINE_DISP, m_bDisplayFitLine);
	toolbarctrl.CheckButton(ID_TB_ROI_DISP, m_bDisplayInspectArea);
	toolbarctrl.CheckButton(ID_TB_THRES_DISP, m_bDisplayThresholdArea);
	toolbarctrl.CheckButton(ID_TB_DEFECT_DISP, m_bDisplayDefectArea);

	m_bROILock = FALSE;
	toolbarctrl.CheckButton(ID_TB_ROI_LOCK, TRUE);

	m_bAlignDisplay = FALSE;
	toolbarctrl.CheckButton(ID_TB_ALIGN_DISP, TRUE);

	toolbarctrl.CheckButton(ID_TB_CALIPER, FALSE);
	m_caliper_control_flag = 0;

	THEAPP.m_pInspectAdminViewDlg->UpdateToolbarStatusTeachMode();
}

void CInspectAdminViewToolbarDlg::SetDisplayChannel(int iDisplayChannel)
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	switch (iDisplayChannel)
	{
	case CHANNEL_TYPE_COLOR:
		toolbarctrl.CheckButton(ID_TB_COLOR_IMAGE, TRUE);
		THEAPP.m_pInspectAdminViewDlg->TbDisplayColor();
		break;
	case CHANNEL_TYPE_R:
		toolbarctrl.CheckButton(ID_TB_R_IMAGE, TRUE);
		THEAPP.m_pInspectAdminViewDlg->TbDisplayR();
		break;
	case CHANNEL_TYPE_G:
		toolbarctrl.CheckButton(ID_TB_G_IMAGE, TRUE);
		THEAPP.m_pInspectAdminViewDlg->TbDisplayG();
		break;
	case CHANNEL_TYPE_B:
		toolbarctrl.CheckButton(ID_TB_B_IMAGE, TRUE);
		THEAPP.m_pInspectAdminViewDlg->TbDisplayB();
		break;
	case CHANNEL_TYPE_H:
		toolbarctrl.CheckButton(ID_TB_H_IMAGE, TRUE);
		THEAPP.m_pInspectAdminViewDlg->TbDisplayH();
		break;
	case CHANNEL_TYPE_S:
		toolbarctrl.CheckButton(ID_TB_S_IMAGE, TRUE);
		THEAPP.m_pInspectAdminViewDlg->TbDisplayS();
		break;
	case CHANNEL_TYPE_I:
		toolbarctrl.CheckButton(ID_TB_I_IMAGE, TRUE);
		THEAPP.m_pInspectAdminViewDlg->TbDisplayI();
		break;
	default:
		break;
	}
}

void CInspectAdminViewToolbarDlg::StartLive()
{
	if (m_bLive == FALSE)
	{
		CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

		toolbarctrl.CheckButton(ID_TB_LIVE, TRUE);
		m_bLive = TRUE;

		THEAPP.m_pInspectAdminViewDlg->StartLive();
	}
}

void CInspectAdminViewToolbarDlg::StopLive()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	toolbarctrl.CheckButton(ID_TB_LIVE, FALSE);
	m_bLive = FALSE;

	THEAPP.m_pInspectAdminViewDlg->StopLive();
}
