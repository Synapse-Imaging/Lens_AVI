// InspectViewDlg1.cpp : implementation file
//
#include "stdafx.h"
#include "uscan.h"
#include "InspectViewOverayImageDlg2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInspectViewOverayImageDlg2 dialog

CInspectViewOverayImageDlg2* CInspectViewOverayImageDlg2::m_pInstance = NULL;

CInspectViewOverayImageDlg2* CInspectViewOverayImageDlg2::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectViewOverayImageDlg2();
		if (!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_VIEW_OVERAY_IMAGE2_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectViewOverayImageDlg2::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectViewOverayImageDlg2::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

CInspectViewOverayImageDlg2::CInspectViewOverayImageDlg2(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectViewOverayImageDlg2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInspectViewOverayImageDlg2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetPosition(VIEW1_DLG1_LEFT + VIEW1_DLG1_WIDTH + 285 + (VIEW1_DLG1_WIDTH + 280) / 2 - 8, VIEW1_DLG1_TOP + VIEW_TOOLBAR_HEIGHT, (VIEW1_DLG1_WIDTH + 280) / 2 - 18, (VIEW1_DLG1_HEIGHT + 370) / 2 - 3 - VIEW_TOOLBAR_HEIGHT);

	m_lWindowID = -1;
	m_iToolBarOffset = 0;
	m_iScrBarWidth = 16;

	m_bOnPaintNow = FALSE;
	m_bDrawActiveTRegion = FALSE;

	m_sWindowName = _T("Overlay Image 1st");
	m_iCurResultNo = 1;
	ClearAll();
}

void CInspectViewOverayImageDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInspectViewOverayImageDlg2)
	DDX_Control(pDX, IDC_VSCRBAR, m_VScrBar);
	DDX_Control(pDX, IDC_HSCRBAR, m_HScrBar);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInspectViewOverayImageDlg2, CDialog)
	//{{AFX_MSG_MAP(CInspectViewOverayImageDlg2)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectViewOverayImageDlg2 message handlers

void CInspectViewOverayImageDlg2::ClearAll()
{
	m_dZoomRatio = 0.4;

	m_iImageWidth = GRAB_X_MAX;
	m_iImageHeight = GRAB_Y_MAX;

	mpActiveTRegion = NULL;
	mpSelectPartTRegion = NULL;

	m_ToolBarState = TS_SELECT_PART;
	m_bOnMoving = FALSE;
	m_bDisplayImage = FALSE;

	GenEmptyObj(&m_pHImage);
}

BOOL CInspectViewOverayImageDlg2::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
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

BOOL CInspectViewOverayImageDlg2::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetClientRect(&m_ClientRect);
	m_bDisplayImage = TRUE;
	Reset();

	SetWindowText((LPCTSTR)m_sWindowName);

	m_Menu.LoadMenu(IDR_MENU_RESULT_IMAGE_SELECTION);
	m_iCurResultNo = 1;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CInspectViewOverayImageDlg2::OnEraseBkgnd(CDC *pDC)
{
	pDC->FillSolidRect(m_ClientRect, TS_COLOR_EXTRA_BG);
	return TRUE;
}

void CInspectViewOverayImageDlg2::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (m_lWindowID < 0 || !m_bDisplayImage || m_bOnPaintNow) return;

	m_bOnPaintNow = TRUE;

	CDC* pDC = GetDC();

	POINT ClientOffset;
	ClientOffset.x = 0;
	ClientOffset.y = m_iToolBarOffset;
	pDC->SetViewportOrg(ClientOffset);

	dc.OffsetViewportOrg(0, m_iToolBarOffset);

	HTuple lDC = (INT)(pDC->m_hDC);
	SetWindowDc(m_lWindowID, lDC);

	CRgn ClipRgn;
	ClipRgn.CreateRectRgn(m_ClientRect.left, m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
	pDC->SelectClipRgn(&ClipRgn);

	if (THEAPP.m_pGFunction->ValidHImage(m_pHImage))
		DispObj(m_pHImage, m_lWindowID);

	SetWindowDc(m_lWindowID, 0);

	ReleaseDC(pDC);

	m_bOnPaintNow = FALSE;

	// Do not call CDialog::OnPaint() for painting messages
}

void CInspectViewOverayImageDlg2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_lWindowID < 0 || !m_bDisplayImage) return;

	long CurPos = m_HScrBar.GetScrollPos();
	SCROLLINFO Info;
	m_HScrBar.GetScrollInfo(&Info, SIF_ALL);

	int nViewWidth = RECTWIDTH(&m_ViewRect);

	switch (nSBCode) {
	case SB_LEFT:					//Scroll to far left.
		CurPos = 0;
		break;
	case SB_RIGHT:					//Scroll to far right.
		CurPos = m_iImageWidth - nViewWidth;
		break;
	case SB_LINELEFT:				//Scroll left.
		if (CurPos > 0) CurPos -= 50;
		break;
	case SB_LINERIGHT:				//Scroll right.
		if (CurPos < m_iImageWidth - nViewWidth) CurPos += 50;
		break;
	case SB_PAGELEFT:				//Scroll one page left.
		if (CurPos > 0) CurPos = max(0, CurPos - (int)Info.nPage);
		break;
	case SB_PAGERIGHT:				//Scroll one page right.
		if (CurPos < m_iImageWidth - nViewWidth) CurPos = min(m_iImageWidth - nViewWidth, CurPos + Info.nPage);
		break;
	case SB_THUMBTRACK:
		CurPos = Info.nTrackPos;
		break;
	}
	m_HScrBar.SetScrollPos(CurPos);
	m_ViewRect.right = CurPos + nViewWidth;
	m_ViewRect.left = CurPos;

	UpdateViewportManager();
	InvalidateRect(NULL, TRUE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CInspectViewOverayImageDlg2::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_lWindowID < 0 || !m_bDisplayImage) return;

	long CurPos = m_VScrBar.GetScrollPos();
	SCROLLINFO Info;
	m_VScrBar.GetScrollInfo(&Info, SIF_ALL);

	int nViewHeight = RECTHEIGHT(&m_ViewRect);

	switch (nSBCode) {
	case SB_TOP:				//Scroll to top. 
		CurPos = m_iImageHeight - nViewHeight;
		break;
	case SB_BOTTOM:				//Scroll to bottom.
		CurPos = 0;
		break;
	case SB_LINEUP:				//Scroll one line up.
		if (CurPos > 0) CurPos -= 50;
		break;
	case SB_LINEDOWN:			//Scroll one line down.
		if (CurPos < m_iImageHeight - nViewHeight) CurPos += 50;
		break;
	case SB_PAGEUP:				//Scroll one page up.
		if (CurPos > 0) CurPos = max(0, CurPos - (int)Info.nPage);
		break;
	case SB_PAGEDOWN:			//Scroll one page down.
		if (CurPos < m_iImageHeight - nViewHeight) CurPos = min(m_iImageHeight - nViewHeight, CurPos + Info.nPage);
		break;
	case SB_THUMBTRACK:
		CurPos = Info.nTrackPos;
		break;
	}
	m_VScrBar.SetScrollPos(CurPos);
	m_ViewRect.bottom = CurPos + nViewHeight;
	m_ViewRect.top = CurPos;

	UpdateViewportManager();
	InvalidateRect(NULL, TRUE);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CInspectViewOverayImageDlg2::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bDisplayImage) return;

	POINT VPoint = point;
	POINT IPoint = point;

	VPoint.y -= m_iToolBarOffset;	// Toolbar offset
	IPoint.y -= m_iToolBarOffset;	// Toolbar offset

	m_ViewportManager.VPtoIP(&IPoint, 1);

	maMVPoints[0] = VPoint;
	maMIPoints[0] = IPoint;

	SetCapture();

	switch (m_ToolBarState) {
	case TS_DRAW:

		break;

	case TS_MOVE:
		m_bOnMoving = TRUE;
		PostMessage(WM_SETCURSOR);
		break;

	case TS_SELECT_PART:
		mpSelectPartTRegion = new CSelectPartRegion;
		if (mpSelectPartTRegion) {
			mpSelectPartTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			mpSelectPartTRegion->SetSelect(TRUE, FALSE);
			DrawSelectPartTRegion();
		}
		break;
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CInspectViewOverayImageDlg2::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bDisplayImage) return;

	ReleaseCapture();

	DPOINT dLTPoint, dRBPoint;
	POINT WHPoint;

	switch (m_ToolBarState) {
	case TS_MOVE:
		m_bOnMoving = FALSE;
		break;

	case TS_SELECT_PART:
		if (mpSelectPartTRegion) {
			mpSelectPartTRegion->GetWHPoint(&WHPoint, THEAPP.m_pCalDataService);

			if (WHPoint.x < 10 || WHPoint.y < 10) {
				delete mpSelectPartTRegion;
				mpSelectPartTRegion = NULL;

				InvalidateRect(&m_ClientRect, FALSE);
				break;
			}

			mpSelectPartTRegion->ArrangePoints();

			mpSelectPartTRegion->GetLTPointD(&dLTPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->GetRBPointD(&dRBPoint, THEAPP.m_pCalDataService);

			delete mpSelectPartTRegion;
			mpSelectPartTRegion = NULL;

			if (dLTPoint.x < 0 || dLTPoint.x >= m_iImageWidth || dLTPoint.y < 0 || dLTPoint.y >= m_iImageHeight ||
				dRBPoint.x < 0 || dRBPoint.x >= m_iImageWidth || dRBPoint.y < 0 || dRBPoint.y >= m_iImageHeight)
				break;

			int iSelectImageWidth, iSelectImageHeight;
			double dZoomRatio;

			if (dLTPoint.x < 0) iSelectImageWidth = (int)(dRBPoint.x + 1);
			else if (dRBPoint.x >= m_iImageWidth) iSelectImageWidth = (int)(m_iImageWidth - dLTPoint.x);
			else
				iSelectImageWidth = (int)(dRBPoint.x - dLTPoint.x + 1);

			if (dLTPoint.y < 0) iSelectImageHeight = (int)(dRBPoint.y + 1);
			else if (dRBPoint.y >= m_iImageHeight) iSelectImageHeight = (int)(m_iImageHeight - dLTPoint.y);
			else
				iSelectImageHeight = (int)(dRBPoint.y - dLTPoint.y + 1);

			if (iSelectImageHeight <= iSelectImageWidth)
				dZoomRatio = (float)RECTWIDTH(&m_ClientRect) / (float)iSelectImageWidth;
			else
				dZoomRatio = (float)RECTHEIGHT(&m_ClientRect) / (float)iSelectImageHeight;

			ZoomInSelectedPart(dZoomRatio, (int)dLTPoint.x, (int)dLTPoint.y);
		}
		break;
	}

	int iViewWidth, iViewHeight;

	iViewWidth = (int)(m_iImageWidth * m_ViewportManager.mdZoomRatio + 0.5);
	iViewHeight = (int)(m_iImageHeight * m_ViewportManager.mdZoomRatio + 0.5);

	if (iViewWidth < m_ClientRect.Width() || iViewHeight < m_ClientRect.Height())
		InvalidateRect(&m_ClientRect, TRUE);
	else
		InvalidateRect(&m_ClientRect, FALSE);

	CDialog::OnLButtonUp(nFlags, point);
}

void CInspectViewOverayImageDlg2::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bDisplayImage) return;

	POINT VPoint = point;
	POINT IPoint = point;

	VPoint.y -= m_iToolBarOffset;
	IPoint.y -= m_iToolBarOffset;

	m_ViewportManager.VPtoIP(&IPoint, 1);

	maMVPoints[1] = VPoint;
	maMIPoints[1] = IPoint;

	CString OutTxt;
	OutTxt.Format("%s - (%d, %d), Zoom(%d%%)", m_sWindowName, (int)((point.x / m_dZoomRatio) + m_ViewRect.left), (int)((point.y / m_dZoomRatio) + m_ViewRect.top), (int)(m_dZoomRatio*100.));
	SetWindowText((LPCTSTR)OutTxt);

	int iWidth, iHeight;
	long HCurPos, VCurPos;
	CPoint MVDiffPoint, MIDiffPoint;

	if (IS_SETFLAG(nFlags, MK_LBUTTON)) {
		switch (m_ToolBarState) {
		case TS_MOVE:
			MVDiffPoint.x = (long)((double)(maMVPoints[0].x - maMVPoints[1].x) / m_ViewportManager.mdZoomRatio);	// 20080129 Eunsung
			MVDiffPoint.y = (long)((double)(maMVPoints[0].y - maMVPoints[1].y) / m_ViewportManager.mdZoomRatio);

			m_ViewportManager.miStartXPos += MVDiffPoint.x;
			m_ViewportManager.miStartYPos += MVDiffPoint.y;

			iWidth = (int)((double)RECTWIDTH(&m_ClientRect) / m_ViewportManager.mdZoomRatio);
			iHeight = (int)((double)RECTHEIGHT(&m_ClientRect) / m_ViewportManager.mdZoomRatio);

			if (m_ViewportManager.miStartXPos + iWidth > m_iImageWidth) m_ViewportManager.miStartXPos = m_iImageWidth - iWidth;
			if (m_ViewportManager.miStartYPos + iHeight > m_iImageHeight) m_ViewportManager.miStartYPos = m_iImageHeight - iHeight;

			if (m_ViewportManager.miStartXPos < 0) m_ViewportManager.miStartXPos = 0;
			if (m_ViewportManager.miStartYPos < 0) m_ViewportManager.miStartYPos = 0;

			SetPart(m_lWindowID, m_ViewportManager.miStartYPos, m_ViewportManager.miStartXPos, m_ViewportManager.miStartYPos + iHeight, m_ViewportManager.miStartXPos + iWidth);

			m_ViewRect.top = m_ViewportManager.miStartYPos;
			m_ViewRect.left = m_ViewportManager.miStartXPos;
			m_ViewRect.bottom = m_ViewRect.top + iHeight;
			m_ViewRect.right = m_ViewRect.left + iWidth;

			HCurPos = m_HScrBar.GetScrollPos();
			HCurPos = HCurPos + MVDiffPoint.x;
			if (HCurPos < 0) HCurPos = 0;
			if (HCurPos > m_iImageWidth - RECTWIDTH(&m_ViewRect)) HCurPos = m_iImageWidth - RECTWIDTH(&m_ViewRect);
			m_HScrBar.SetScrollPos(HCurPos);

			VCurPos = m_VScrBar.GetScrollPos();
			VCurPos = VCurPos + MVDiffPoint.y;
			if (VCurPos < 0) VCurPos = 0;
			if (VCurPos > m_iImageHeight - RECTHEIGHT(&m_ViewRect)) VCurPos = m_iImageHeight - RECTHEIGHT(&m_ViewRect);
			m_VScrBar.SetScrollPos(VCurPos);

			InvalidateRect(&m_ClientRect, FALSE);

			break;

		case TS_SELECT_PART:

			if (!mpSelectPartTRegion) return;

			DrawSelectPartTRegion();

			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			DrawSelectPartTRegion();

			break;

		default:
			break;
		}
	}

	maMVPoints[0] = maMVPoints[1];
	maMIPoints[0] = maMIPoints[1];

	CDialog::OnMouseMove(nFlags, point);
}

void CInspectViewOverayImageDlg2::OnDestroy()
{
	CDialog::OnDestroy();

	SAFE_DELETE(mpActiveTRegion);

	if (m_lWindowID > 0)
	{
		HalconCpp::CloseWindow(m_lWindowID);
		m_lWindowID = -1;
	}

	m_Menu.DestroyMenu();
}

void CInspectViewOverayImageDlg2::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	GetClientRect(&m_ClientRect);

	m_ClientRect.top = m_iToolBarOffset;
	m_ClientRect.right = m_ClientRect.right - m_iScrBarWidth;
	m_ClientRect.bottom = m_ClientRect.bottom - m_iScrBarWidth;

	InitViewRect();
	UpdateViewportManager();	// CSKIM
	ScrollBarSet();
	ScrollBarPosSet();

	InvalidateRect(&m_ClientRect, TRUE);
}

BOOL CInspectViewOverayImageDlg2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	ScreenToClient(&pt);

	if (!m_ClientRect.PtInRect(pt))
	{
		return CDialog::OnMouseWheel(nFlags, zDelta, pt);
	}

	if (zDelta <= 0)
	{
		TbZoomIn();
	}
	else
	{
		TbZoomOut();
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CInspectViewOverayImageDlg2::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	switch (m_ToolBarState) {
	case TS_MOVE:
		if (m_bOnMoving)
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_HANDGRAB));
		else
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_HAND));
		return TRUE;

	case TS_SELECT_PART:
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SELECTPART));
		return TRUE;
	}
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CInspectViewOverayImageDlg2::Reset()
{
	InitViewRect();
	UpdateViewportManager();
}

void CInspectViewOverayImageDlg2::InitViewRect()
{
	m_ViewRect.left = 0;
	m_ViewRect.top = 0;
	m_ViewRect.bottom = (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectViewOverayImageDlg2::UpdateViewportManager()
{
	m_ViewportManager.miStartXPos = m_ViewRect.left;
	m_ViewportManager.miStartYPos = m_ViewRect.top;
	m_ViewportManager.miViewWidth = (int)RECTWIDTH(&m_ClientRect);
	m_ViewportManager.miViewHeight = (int)RECTHEIGHT(&m_ClientRect);
	m_ViewportManager.miImageWidth = m_iImageWidth;
	m_ViewportManager.miImageHeight = m_iImageHeight;
	m_ViewportManager.mdZoomRatio = m_dZoomRatio;

	if (m_lWindowID > 0) {
		SetPart(m_lWindowID, m_ViewRect.top, m_ViewRect.left, m_ViewRect.bottom, m_ViewRect.right);
		SetWindowExtents(m_lWindowID, 0, 0, m_ClientRect.Width(), m_ClientRect.Height());
	}
}

void CInspectViewOverayImageDlg2::ViewRectSet()
{
	m_ViewRect.bottom = m_ViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = m_ViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectViewOverayImageDlg2::ScrollBarSet()
{
	SCROLLINFO Info;

	Info.cbSize = sizeof(SCROLLINFO);
	Info.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
	Info.nMin = 0;

	if (m_bDisplayImage) {
		m_VScrBar.GetScrollInfo(&Info);
		Info.nMax = m_iImageHeight;
		Info.nPage = (int)(RECTHEIGHT(&m_ViewRect));
		Info.nPos = m_ViewRect.top;
	}
	else {
		Info.nMax = m_ClientRect.bottom;
		Info.nPage = 10;
	}
	m_VScrBar.SetScrollInfo(&Info);

	if (m_bDisplayImage) {
		m_HScrBar.GetScrollInfo(&Info);
		Info.nMax = m_iImageWidth;
		Info.nPage = (int)(RECTWIDTH(&m_ViewRect));
		Info.nPos = m_ViewRect.left;
	}
	else {
		Info.nMax = m_ClientRect.right;
		Info.nPage = 10;
	}
	m_HScrBar.SetScrollInfo(&Info);
}

void CInspectViewOverayImageDlg2::ScrollBarPosSet()
{
	m_VScrBar.SetWindowPos(&wndTop, m_ClientRect.right, m_ClientRect.top, m_iScrBarWidth, m_ClientRect.bottom - m_iToolBarOffset, SWP_SHOWWINDOW);
	m_VScrBar.ShowScrollBar(TRUE);
	m_HScrBar.SetWindowPos(&wndTop, m_ClientRect.left, m_ClientRect.bottom, m_ClientRect.right, m_iScrBarWidth, SWP_SHOWWINDOW);
	m_HScrBar.ShowScrollBar(TRUE);
}

void CInspectViewOverayImageDlg2::DrawActiveTRegion()
{
	if (m_bDrawActiveTRegion) return;

	m_bDrawActiveTRegion = TRUE;
	CDC *pDC = GetDC();

	POINT ClientOffset;
	ClientOffset.x = 0;
	ClientOffset.y = m_iToolBarOffset;
	pDC->SetViewportOrg(ClientOffset);

	CRgn ClipRgn;

	if (mpActiveTRegion)
	{
		ClipRgn.CreateRectRgn(m_ClientRect.left, m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
		pDC->SelectClipRgn(&ClipRgn);
		mpActiveTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_ACTIVE, FALSE, THEAPP.m_pCalDataService);
	}
	ReleaseDC(pDC);

	m_bDrawActiveTRegion = FALSE;
}

void CInspectViewOverayImageDlg2::DrawSelectPartTRegion()
{
	CDC *pDC = GetDC();

	POINT ClientOffset;
	ClientOffset.x = 0;
	ClientOffset.y = m_iToolBarOffset;
	pDC->SetViewportOrg(ClientOffset);

	CRgn ClipRgn;

	if (mpSelectPartTRegion)
	{
		ClipRgn.CreateRectRgn(m_ClientRect.left, m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
		pDC->SelectClipRgn(&ClipRgn);
		mpSelectPartTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_SELECTPART, FALSE, THEAPP.m_pCalDataService);
	}
	ReleaseDC(pDC);
}

void CInspectViewOverayImageDlg2::ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY)
{
	if (m_bDisplayImage) {
		m_dZoomRatio = dZoomRatio;

		m_ViewRect.top = iStartPosY;
		m_ViewRect.left = iStartPosX;

		ViewRectSet();

		if (m_ViewRect.right >= m_iImageWidth) {
			m_ViewRect.right = m_iImageWidth - 1;
			m_ViewRect.left = m_ViewRect.right - (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
			if (m_ViewRect.left < 0) {
				m_ViewRect.left = 0;
				m_ViewRect.right = (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
			}
		}

		if (m_ViewRect.bottom >= m_iImageHeight) {
			m_ViewRect.bottom = m_iImageHeight - 1;
			m_ViewRect.top = m_ViewRect.bottom - (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
			if (m_ViewRect.top < 0) {
				m_ViewRect.top = 0;
				m_ViewRect.bottom = (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
			}
		}

		ScrollBarSet();

		//		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("%s - Zoom(%d%%)", m_sWindowName, (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

void CInspectViewOverayImageDlg2::TbDraw()
{
	m_ToolBarState = TS_DRAW;
}

void CInspectViewOverayImageDlg2::TbMove()
{
	m_ToolBarState = TS_MOVE;
}

void CInspectViewOverayImageDlg2::TbSelectPart()
{
	m_ToolBarState = TS_SELECT_PART;
}

void CInspectViewOverayImageDlg2::TbZoomIn()
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (int)(m_dZoomRatio * 10) * 0.1;

		if (m_dZoomRatio < 1.0) m_dZoomRatio += 0.1;
		else if (m_dZoomRatio < 2.0) m_dZoomRatio += 0.5;
		else if (m_dZoomRatio < 10.0) m_dZoomRatio += 1.0;
		else m_dZoomRatio = 10.0;

		ViewRectSet();
		ScrollBarSet();

		//		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("%s - Zoom(%d%%)", m_sWindowName, (int)(m_dZoomRatio*100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

void CInspectViewOverayImageDlg2::TbZoomOut()
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (int)(m_dZoomRatio * 10) * 0.1;

		if (m_dZoomRatio > 2.0) m_dZoomRatio -= 1.0;
		else if (m_dZoomRatio > 1.0) m_dZoomRatio -= 0.5;
		else if (m_dZoomRatio > 0.1) m_dZoomRatio -= 0.1;
		else m_dZoomRatio = 0.1;

		CRect TempViewRect;

		TempViewRect = m_ViewRect;

		TempViewRect.bottom = TempViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
		TempViewRect.right = TempViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);

		if (TempViewRect.right >= m_iImageWidth || TempViewRect.bottom >= m_iImageHeight) {
			if (TempViewRect.right >= m_iImageWidth) {
				m_ViewRect.right = m_iImageWidth - 1;
				m_ViewRect.left = m_ViewRect.right - (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
				if (m_ViewRect.left < 0) {
					m_dZoomRatio = (float)RECTWIDTH(&m_ClientRect) / (float)m_iImageWidth;
					m_ViewRect.left = 0;
				}

				if (TempViewRect.bottom >= m_iImageHeight) {
					m_ViewRect.bottom = m_iImageHeight - 1;
					m_ViewRect.top = m_ViewRect.bottom - (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
				}
				else {
					m_ViewRect.bottom = m_ViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
				}
			}

			if (TempViewRect.bottom >= m_iImageHeight) {
				m_ViewRect.bottom = m_iImageHeight - 1;
				m_ViewRect.top = m_ViewRect.bottom - (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);

				if (m_ViewRect.top < 0) {
					m_dZoomRatio = (float)RECTHEIGHT(&m_ClientRect) / (float)m_iImageHeight;
					m_ViewRect.top = 0;
				}

				if (TempViewRect.right >= m_iImageWidth) {
					m_ViewRect.right = m_iImageWidth - 1;
					m_ViewRect.left = m_ViewRect.right - (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
				}
				else {
					m_ViewRect.right = m_ViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
				}
			}
		}
		else {
			ViewRectSet();
		}

		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("%s - Zoom(%d%%)", m_sWindowName, (int)(m_dZoomRatio*100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

void CInspectViewOverayImageDlg2::TbFitWidth()
{
	if (m_bDisplayImage)
	{
		ClearWindow(m_lWindowID);

		m_dZoomRatio = (double)RECTWIDTH(&m_ClientRect) / (double)m_iImageWidth;

		InitViewRect();
		UpdateViewportManager();	// CSKIM
		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("%s - Zoom(%d%%)", m_sWindowName, (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(NULL, TRUE);
	}
}

void CInspectViewOverayImageDlg2::TbFitHeight()
{
	if (m_bDisplayImage)
	{
		ClearWindow(m_lWindowID);

		m_dZoomRatio = (double)RECTHEIGHT(&m_ClientRect) / (double)m_iImageHeight;

		InitViewRect();
		UpdateViewportManager();	// CSKIM
		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("%s - Zoom(%d%%)", m_sWindowName, (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(NULL, TRUE);
	}
}

BOOL CInspectViewOverayImageDlg2::ShowWindow(int nCmdShow)
{
	return CDialog::ShowWindow(nCmdShow);
}

int CInspectViewOverayImageDlg2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1) return -1;

	HTuple tWnd = (INT)(m_hWnd);
	HTuple Hm_lWindowID;
	NewExternWindow(tWnd, 0, 0, m_iImageWidth, m_iImageHeight, &Hm_lWindowID);
	m_lWindowID = Hm_lWindowID.L();

	return 0;
}

void CInspectViewOverayImageDlg2::LoadScanImage(int nScanNum)
{

}

void CInspectViewOverayImageDlg2::MoveToDefectArea()
{

}

void CInspectViewOverayImageDlg2::RedrawView()
{
	InvalidateRect(NULL, FALSE);
}

void CInspectViewOverayImageDlg2::RedrawRect()
{
	InvalidateRect(&m_ClientRect, FALSE);
}

void CInspectViewOverayImageDlg2::UpdateView(BOOL bDelete)
{
	InvalidateRect(&m_ClientRect, bDelete);
}

void CInspectViewOverayImageDlg2::CallFitImageHeight()
{
	TbFitHeight();
}

void CInspectViewOverayImageDlg2::CallFitImageWidth()
{
	TbFitWidth();
}

void CInspectViewOverayImageDlg2::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu *pSubMenu;
	pSubMenu = m_Menu.GetSubMenu(0);

	int i;

	for (i = 0; i < MAX_RESULT_IMAGE; i++)
	{
		pSubMenu->EnableMenuItem(ID_MENU_RESULT_1 + i, MF_DISABLED);
		pSubMenu->CheckMenuItem(ID_MENU_RESULT_1 + i, MF_UNCHECKED);
	}

	for (i = 0; i < MAX_RESULT_IMAGE; i++)
	{
		pSubMenu->EnableMenuItem(ID_MENU_RESULT_1 + i, MF_ENABLED);
	}

	if (m_iCurResultNo >= 1 && m_iCurResultNo <= MAX_RESULT_IMAGE)
		pSubMenu->CheckMenuItem(ID_MENU_RESULT_1 + m_iCurResultNo - 1, MF_CHECKED);

	int iPrevResultNo = m_iCurResultNo;

	CPoint ClientPoint;
	ClientPoint = point;
	ClientToScreen(&ClientPoint);

	UINT uMessage = pSubMenu->TrackPopupMenu(TPM_TOPALIGN | TPM_RETURNCMD, point.x, point.y, this);

	switch (uMessage)
	{
	case ID_MENU_RESULT_1:
		m_iCurResultNo = 1;
		break;

	case ID_MENU_RESULT_2:
		m_iCurResultNo = 2;
		break;

	case ID_MENU_RESULT_3:
		m_iCurResultNo = 3;
		break;

	case ID_MENU_RESULT_4:
		m_iCurResultNo = 4;
		break;

	case ID_MENU_RESULT_5:
		m_iCurResultNo = 5;
		break;

	case ID_MENU_RESULT_6:
		m_iCurResultNo = 6;
		break;

	case ID_MENU_RESULT_7:
		m_iCurResultNo = 7;
		break;

	case ID_MENU_RESULT_8:
		m_iCurResultNo = 8;
		break;

	case ID_MENU_RESULT_9:
		m_iCurResultNo = 9;
		break;

	case ID_MENU_RESULT_10:
		m_iCurResultNo = 10;
		break;

	case ID_MENU_RESULT_11:
		m_iCurResultNo = 11;
		break;

	case ID_MENU_RESULT_12:
		m_iCurResultNo = 12;
		break;

	case ID_MENU_RESULT_13:
		m_iCurResultNo = 13;
		break;

	case ID_MENU_RESULT_14:
		m_iCurResultNo = 14;
		break;

	case ID_MENU_RESULT_15:
		m_iCurResultNo = 15;
		break;

	case ID_MENU_RESULT_16:
		m_iCurResultNo = 16;
		break;

	case ID_MENU_RESULT_17:
		m_iCurResultNo = 17;
		break;

	case ID_MENU_RESULT_18:
		m_iCurResultNo = 18;
		break;

	default:
		break;
	}

	if (iPrevResultNo != m_iCurResultNo)
	{
		UpdateResultImage();
	}

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectViewOverayImageDlg2::UpdateResultImage()
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[THEAPP.m_iLastSelectVisionCamType];

	if (THEAPP.m_iTeachingModuleNo == TEACHING_MODULE_NO_UNDEFINED || THEAPP.m_iLastSelectVisionCamType < 0 || THEAPP.m_iLastSelectPcVision < 0)
		return;

	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;
	CString strResultFileFullName, ResultFileName, sReadFileName;
	HTuple HlImageWidth, HlImageHeight;

	GenEmptyObj(&m_pHImage);

	ResultFileName.Format("Module%d_%s_Result%d_*", THEAPP.m_iTeachingModuleNo, sVisionCamType_Short, m_iCurResultNo);
	sReadFileName = THEAPP.m_FileBase.m_strLastResultImageFolder[THEAPP.m_iLastSelectPcVision] + "\\" + ResultFileName;
	if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		strResultFileFullName.Format("%s\\%s", THEAPP.m_FileBase.m_strLastResultImageFolder[THEAPP.m_iLastSelectPcVision], (LPCTSTR)FindFileData.cFileName);

		FindClose(hFindFile);

		ReadImage(&m_pHImage, HTuple(strResultFileFullName));
		GetImageSize(m_pHImage, &HlImageWidth, &HlImageHeight);
		m_iImageWidth = (int)HlImageWidth.L();
		m_iImageHeight = (int)HlImageHeight.L();

		// Need to modify
		m_sWindowName.Format("Result %d", m_iCurResultNo);
		CallFitImageWidth();
	}

}
