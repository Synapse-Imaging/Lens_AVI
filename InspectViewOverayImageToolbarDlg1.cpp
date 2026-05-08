// InspectViewOverayImageToolbarDlg1.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectViewOverayImageToolbarDlg1.h"
#include "afxdialogex.h"

// CInspectViewOverayImageToolbarDlg1 대화 상자입니다.

CInspectViewOverayImageToolbarDlg1* CInspectViewOverayImageToolbarDlg1::m_pInstance = NULL;

CInspectViewOverayImageToolbarDlg1* CInspectViewOverayImageToolbarDlg1::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectViewOverayImageToolbarDlg1();
		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_VIEW_OVERAY_IMAGE1_TOOLBAR_DLG, pFrame->GetActiveView());
			if (bShowFlag)
				m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectViewOverayImageToolbarDlg1::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectViewOverayImageToolbarDlg1::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

BOOL CInspectViewOverayImageToolbarDlg1::ShowWindow(int nCmdShow)
{
	return CDialog::ShowWindow(nCmdShow);
}

IMPLEMENT_DYNAMIC(CInspectViewOverayImageToolbarDlg1, CDialog)

CInspectViewOverayImageToolbarDlg1::CInspectViewOverayImageToolbarDlg1(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectViewOverayImageToolbarDlg1::IDD, pParent)
{
	SetPosition(VIEW1_DLG1_LEFT + VIEW1_DLG1_WIDTH + 285 + 3, VIEW1_DLG1_TOP, (VIEW1_DLG1_WIDTH + 280) / 2 - 18 - 3, VIEW_TOOLBAR_HEIGHT - 4);
}

CInspectViewOverayImageToolbarDlg1::~CInspectViewOverayImageToolbarDlg1()
{
}

void CInspectViewOverayImageToolbarDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInspectViewOverayImageToolbarDlg1, CDialog)
	ON_COMMAND(ID_TB_DRAW, OnTbDraw)
	ON_COMMAND(ID_TB_MOVE, OnTbMove)
	ON_COMMAND(ID_TB_SELECT_PART, OnTbSelectPart)
	ON_COMMAND(ID_TB_ZOOM_IN, OnTbZoomIn)
	ON_COMMAND(ID_TB_ZOOM_OUT, OnTbZoomOut)
	ON_COMMAND(ID_TB_FIT_WIDTH, OnTbFitWidth)
	ON_COMMAND(ID_TB_FIT_HEIGHT, OnTbFitHeight)
	ON_COMMAND(ID_TB_CROSS_BAR, OnTbDispCrossBar)
END_MESSAGE_MAP()


// CInspectViewOverayImageToolbarDlg1 메시지 처리기입니다.

BOOL CInspectViewOverayImageToolbarDlg1::PreTranslateMessage(MSG* pMsg)
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

BOOL CInspectViewOverayImageToolbarDlg1::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Toolbar
	CRect ToolbarBorder(3, 3, 3, 3);
	if (!m_ViewToolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_BORDER | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS, ToolbarBorder) ||
		!m_ViewToolbar.LoadToolBar(IDR_RAWIMAGE_VIEW_TOOLBAR)) {
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

	m_ViewToolbar.SetButtonStyle(0, TBBS_CHECKGROUP);
	m_ViewToolbar.SetButtonStyle(1, TBBS_CHECKGROUP);
	m_ViewToolbar.SetButtonStyle(2, TBBS_CHECKGROUP);

	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_SELECT_PART, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInspectViewOverayImageToolbarDlg1::OnTbDraw()
{
	THEAPP.m_pInspectViewOverayImageDlg1->TbDraw();
}

void CInspectViewOverayImageToolbarDlg1::OnTbMove()
{
	THEAPP.m_pInspectViewOverayImageDlg1->TbMove();
}

void CInspectViewOverayImageToolbarDlg1::OnTbSelectPart()
{
	THEAPP.m_pInspectViewOverayImageDlg1->TbSelectPart();
}

void CInspectViewOverayImageToolbarDlg1::OnTbZoomIn()
{
	THEAPP.m_pInspectViewOverayImageDlg1->TbZoomIn();
}

void CInspectViewOverayImageToolbarDlg1::OnTbZoomOut()
{
	THEAPP.m_pInspectViewOverayImageDlg1->TbZoomOut();
}

void CInspectViewOverayImageToolbarDlg1::OnTbFitWidth()
{
	THEAPP.m_pInspectViewOverayImageDlg1->TbFitWidth();
}

void CInspectViewOverayImageToolbarDlg1::OnTbFitHeight()
{
	THEAPP.m_pInspectViewOverayImageDlg1->TbFitHeight();
}

void CInspectViewOverayImageToolbarDlg1::OnTbDispCrossBar()
{
	THEAPP.m_pInspectViewOverayImageDlg1->OnTbDispCrossBar();
}