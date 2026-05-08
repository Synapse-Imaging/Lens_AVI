// HWinPanel.cpp : implementation file
//

#include "stdafx.h"
#include "uScan.h"
#include "HWinPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHWinPanel

CHWinPanel::CHWinPanel()
{
	mhDC = 0;
	mlWindowHandle = -1;
}

CHWinPanel::~CHWinPanel()
{
	DestroyHWindow();
}

void CHWinPanel::CreateHWindow()
{
	RECT rect;
	mhDC = ::GetDC(m_hWnd);
	::GetClientRect(m_hWnd, &rect);
	window_width = rect.right - rect.left + 1;
	window_height = rect.bottom - rect.top + 1;

	NewExternWindow((Hlong)m_hWnd, (Hlong)0, (Hlong)0, (Hlong)window_width, (Hlong)window_height, &mlWindowHandle);

	Hlong lDC;
	lDC = (Hlong)(mhDC);
	SetWindowDc(mlWindowHandle, HTuple(lDC));
}

void CHWinPanel::DestroyHWindow()
{
	if (mlWindowHandle > 0)
	{
		HalconCpp::CloseWindow(mlWindowHandle);
		mlWindowHandle = -1;
	}

	::ReleaseDC(m_hWnd, mhDC);
}

BEGIN_MESSAGE_MAP(CHWinPanel, CStatic)
	//{{AFX_MSG_MAP(CHWinPanel)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHWinPanel message handlers

void CHWinPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here

	// Do not call CStatic::OnPaint() for painting messages
}
