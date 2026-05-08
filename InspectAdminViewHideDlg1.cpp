// InspectViewDlg1.cpp : implementation file
//
#include "stdafx.h"
#include "uscan.h"
#include "InspectAdminViewHideDlg1.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	bool IsChsWzModelType()
	{
		CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
		return strModelType == _T("CHS-W") || strModelType == _T("CHS-Z");
	}

	bool IsChsKstvModelType()
	{
		CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
		return strModelType == _T("CHS-K") || strModelType == _T("CHS-S") || strModelType == _T("CHS-T") || strModelType == _T("CHS-V");
	}

	bool UseFaiAngleMeasure()
	{
		return IsChsKstvModelType() || IsChsWzModelType();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInspectAdminViewHideDlg1 dialog

CInspectAdminViewHideDlg1* CInspectAdminViewHideDlg1::m_pInstance = NULL;

CInspectAdminViewHideDlg1* CInspectAdminViewHideDlg1::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectAdminViewHideDlg1();
		if (!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_ADMIN_VIEW_HIDE_DLG1, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectAdminViewHideDlg1::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectAdminViewHideDlg1::Show()
{
	SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_NOSIZE);

	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

CInspectAdminViewHideDlg1::CInspectAdminViewHideDlg1(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectAdminViewHideDlg1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInspectAdminViewHideDlg1)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP, VIEW1_DLG1_WIDTH + 280, VIEW1_DLG1_HEIGHT + 370);
	HideWindowHandle = -1;
	m_iToolBarOffset = 0;
	m_iScrBarWidth = 16;

	m_bOnPaintNow = FALSE;
	m_bDrawActiveTRegion = FALSE;
	m_bLive = FALSE;

	ClearAll();
}

void CInspectAdminViewHideDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInspectAdminViewHideDlg1)
	DDX_Control(pDX, IDC_VSCRBAR, m_VScrBar);
	DDX_Control(pDX, IDC_HSCRBAR, m_HScrBar);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInspectAdminViewHideDlg1, CDialog)
	//{{AFX_MSG_MAP(CInspectAdminViewHideDlg1)
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
	ON_COMMAND(ID_TB_DRAW, OnTbDraw)
	ON_COMMAND(ID_TB_MOVE, OnTbMove)
	ON_COMMAND(ID_TB_SELECT_PART, OnTbSelectPart)
	ON_COMMAND(ID_TB_ZOOM_IN, OnTbZoomIn)
	ON_COMMAND(ID_TB_ZOOM_OUT, OnTbZoomOut)
	ON_COMMAND(ID_TB_FIT_WIDTH, OnTbFitWidth)
	ON_COMMAND(ID_TB_FIT_HEIGHT, OnTbFitHeight)
	ON_COMMAND(ID_TB_LIVE, OnTbLive)

	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectAdminViewHideDlg1 message handlers

void CInspectAdminViewHideDlg1::ClearAll()
{
	m_dZoomRatio = 0.4;
	m_iImageWidth = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[VISION_NUMBER_1];
	m_iImageHeight = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[VISION_NUMBER_1];

	mpActiveTRegion = NULL;
	mpSelectPartTRegion = NULL;

	m_ToolBarState = TS_SELECT_PART;
	m_bOnMoving = FALSE;
	m_bDisplayImage = FALSE;
}

BOOL CInspectAdminViewHideDlg1::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
		return TRUE;
	case WM_KEYDOWN:
		if ((int)pMsg->wParam == VK_ESCAPE || (int)pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	case WM_SYSKEYDOWN:
		if (pMsg->wParam == VK_F4)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CInspectAdminViewHideDlg1::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetClientRect(&m_ClientRect);
	Reset();
	m_bDisplayImage = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CInspectAdminViewHideDlg1::OnEraseBkgnd(CDC* pDC)
{
	pDC->FillSolidRect(m_ClientRect, TS_COLOR_EXTRA_BG);
	return TRUE;
}

void CInspectAdminViewHideDlg1::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Do not call CDialog::OnPaint() for painting messages
}

void CInspectAdminViewHideDlg1::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (HideWindowHandle < 0 || !m_bDisplayImage) return;

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

void CInspectAdminViewHideDlg1::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (HideWindowHandle < 0 || !m_bDisplayImage) return;

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

void CInspectAdminViewHideDlg1::OnLButtonDown(UINT nFlags, CPoint point)
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

	switch (m_ToolBarState)
	{
	case TS_DRAW:
	{
		if (THEAPP.m_iModeSwitch != MODE_ADMIN_TEACH_VIEW) { return; }
		if (THEAPP.m_pTabControlDlg->m_TabControl.GetCurSel() != 0) { return; }

		mpActiveTRegion = new GTRegion;		// 엑티브 리전 생성.
		if (mpActiveTRegion)
		{
			mpActiveTRegion->SetLineColor(RGB(255, 0, 0));
			mpActiveTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
			mpActiveTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);
			mpActiveTRegion->SetSelect(TRUE, FALSE);
			DrawActiveTRegion();
		}
		break;
	}

	case TS_MOVE:
	{
		m_bOnMoving = TRUE;
		PostMessage(WM_SETCURSOR);
		break;
	}

	case TS_SELECT_PART:
	{
		mpSelectPartTRegion = new CSelectPartRegion;
		if (mpSelectPartTRegion)
		{
			mpSelectPartTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			mpSelectPartTRegion->SetSelect(TRUE, FALSE);
			DrawSelectPartTRegion();
		}
		break;
	}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CInspectAdminViewHideDlg1::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bDisplayImage) return;

	ReleaseCapture();

	DPOINT dLTPoint, dRBPoint;
	POINT WHPoint;

	switch (m_ToolBarState) {
	case TS_DRAW:
	{
		if (THEAPP.m_iModeSwitch != MODE_ADMIN_TEACH_VIEW) { return; }
		if (THEAPP.m_pTabControlDlg->m_TabControl.GetCurSel() != 0) { return; }
		mpActiveTRegion->GetLTPointD(&dLTPoint, THEAPP.m_pCalDataService);
		mpActiveTRegion->GetRBPointD(&dRBPoint, THEAPP.m_pCalDataService);

		SAFE_DELETE(mpActiveTRegion);
		CropLTPoint.x = (long)dLTPoint.x;
		CropLTPoint.y = (long)dLTPoint.y;
		CropRBPoint.x = (long)dRBPoint.x;
		CropRBPoint.y = (long)dRBPoint.y;

		m_dLTPoint.x = dLTPoint.x;
		m_dLTPoint.y = dLTPoint.y;
		m_dRBPoint.x = dRBPoint.x;
		m_dRBPoint.y = dRBPoint.y;

		break;
	}
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

void CInspectAdminViewHideDlg1::OnMouseMove(UINT nFlags, CPoint point)
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
	OutTxt.Format("Strip View [Top Camera] - (%d, %d), Zoom(%d%%)", (int)((point.x / m_dZoomRatio) + m_ViewRect.left), (int)((point.y / m_dZoomRatio) + m_ViewRect.top), (int)(m_dZoomRatio * 100.));
	SetWindowText((LPCTSTR)OutTxt);

	int iWidth, iHeight;
	long HCurPos, VCurPos;
	CPoint MVDiffPoint, MIDiffPoint;

	if (IS_SETFLAG(nFlags, MK_LBUTTON)) {
		switch (m_ToolBarState) {
		case TS_DRAW:
		{
			if (THEAPP.m_iModeSwitch != MODE_ADMIN_TEACH_VIEW) { return; }
			if (THEAPP.m_pTabControlDlg->m_TabControl.GetCurSel() != 0) { return; }
			DrawActiveTRegion();
			mpActiveTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);
			DrawActiveTRegion();
			break;
		}
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

			SetPart(HideWindowHandle, m_ViewportManager.miStartYPos, m_ViewportManager.miStartXPos, m_ViewportManager.miStartYPos + iHeight, m_ViewportManager.miStartXPos + iWidth);

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

void CInspectAdminViewHideDlg1::OnDestroy()
{
	CDialog::OnDestroy();

	SAFE_DELETE(mpActiveTRegion);

	if (HideWindowHandle > 0)
	{
		HalconCpp::CloseWindow(HideWindowHandle);
		HideWindowHandle = -1;
	}
}

void CInspectAdminViewHideDlg1::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	GetClientRect(&m_ClientRect);

	m_ClientRect.top = m_iToolBarOffset;
	m_ClientRect.right = m_ClientRect.right - m_iScrBarWidth;
	m_ClientRect.bottom = m_ClientRect.bottom - m_iScrBarWidth;

	InitViewRect();
	UpdateViewportManager();
	ScrollBarSet();
	ScrollBarPosSet();

	InvalidateRect(&m_ClientRect, TRUE);
}

BOOL CInspectAdminViewHideDlg1::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta <= 0) SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
	else SendMessage(WM_VSCROLL, SB_PAGEUP, 0);

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CInspectAdminViewHideDlg1::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	switch (m_ToolBarState)
	{
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

void CInspectAdminViewHideDlg1::Reset()
{
	InitViewRect();
	UpdateViewportManager();
}

void CInspectAdminViewHideDlg1::InitViewRect()
{
	m_ViewRect.left = 0;
	m_ViewRect.top = 0;
	m_ViewRect.bottom = (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectAdminViewHideDlg1::UpdateViewportManager()
{
	m_ViewportManager.miStartXPos = m_ViewRect.left;
	m_ViewportManager.miStartYPos = m_ViewRect.top;
	m_ViewportManager.miViewWidth = (int)RECTWIDTH(&m_ClientRect);
	m_ViewportManager.miViewHeight = (int)RECTHEIGHT(&m_ClientRect);
	m_ViewportManager.miImageWidth = m_iImageWidth;
	m_ViewportManager.miImageHeight = m_iImageHeight;
	m_ViewportManager.mdZoomRatio = m_dZoomRatio;

	if (HideWindowHandle > 0) {
		SetPart(HideWindowHandle, m_ViewRect.top, m_ViewRect.left, m_ViewRect.bottom, m_ViewRect.right);
		SetWindowExtents(HideWindowHandle, 0, 0, m_ClientRect.Width(), m_ClientRect.Height());
	}
}

void CInspectAdminViewHideDlg1::OverlaySetViewportManager(int iLeft, int iTop, double dZoomRatio)
{
	m_ViewRect.left = iLeft;
	m_ViewRect.top = iTop;
	m_dZoomRatio = dZoomRatio;
	ViewRectSet();

	m_ViewportManager.miStartXPos = m_ViewRect.left;
	m_ViewportManager.miStartYPos = m_ViewRect.top;
	m_ViewportManager.miViewWidth = (int)RECTWIDTH(&m_ClientRect);
	m_ViewportManager.miViewHeight = (int)RECTHEIGHT(&m_ClientRect);
	m_ViewportManager.miImageWidth = m_iImageWidth;
	m_ViewportManager.miImageHeight = m_iImageHeight;
	m_ViewportManager.mdZoomRatio = m_dZoomRatio;

	if (HideWindowHandle > 0) {
		SetPart(HideWindowHandle, m_ViewRect.top, m_ViewRect.left, m_ViewRect.bottom, m_ViewRect.right);
		SetWindowExtents(HideWindowHandle, 0, 0, m_ClientRect.Width(), m_ClientRect.Height());
	}
}

void CInspectAdminViewHideDlg1::OverlaySetCropViewportManager(CPoint cpCenterPoint, int iWidth, int iHeight)
{
	int iCenter_x = cpCenterPoint.x;
	int iCenter_y = cpCenterPoint.y;

	m_ViewRect.left = iCenter_x - iWidth;
	m_ViewRect.top = iCenter_y - iHeight;
	m_ViewRect.right = iCenter_x + iWidth;
	m_ViewRect.bottom = iCenter_y + iHeight;

	m_ViewportManager.miStartXPos = m_ViewRect.left;
	m_ViewportManager.miStartYPos = m_ViewRect.top;
	m_ViewportManager.miViewWidth = (int)RECTWIDTH(&m_ClientRect);
	m_ViewportManager.miViewHeight = (int)RECTHEIGHT(&m_ClientRect);
	m_ViewportManager.miImageWidth = m_iImageWidth;
	m_ViewportManager.miImageHeight = m_iImageHeight;

	if (HideWindowHandle > 0) {
		SetPart(HideWindowHandle, m_ViewRect.top, m_ViewRect.left, m_ViewRect.bottom, m_ViewRect.right);
		SetWindowExtents(HideWindowHandle, 0, 0, m_ClientRect.Width(), m_ClientRect.Height());
	}
}

void CInspectAdminViewHideDlg1::OverlaySetCenterViewportManager(int iCenterX, int iCenterY, double dZoomRatio)
{
	m_ViewRect.left = iCenterX - (int)(RECTHEIGHT(&m_ClientRect) / dZoomRatio * 0.5);
	m_ViewRect.top = iCenterY - (int)(RECTWIDTH(&m_ClientRect) / dZoomRatio * 0.5);
	if (m_ViewRect.left < 0)
		m_ViewRect.left = 0;
	if (m_ViewRect.top < 0)
		m_ViewRect.top = 0;

	m_dZoomRatio = dZoomRatio;
	ViewRectSet();

	m_ViewportManager.miStartXPos = m_ViewRect.left;
	m_ViewportManager.miStartYPos = m_ViewRect.top;
	m_ViewportManager.miViewWidth = (int)RECTWIDTH(&m_ClientRect);
	m_ViewportManager.miViewHeight = (int)RECTHEIGHT(&m_ClientRect);
	m_ViewportManager.miImageWidth = m_iImageWidth;
	m_ViewportManager.miImageHeight = m_iImageHeight;
	m_ViewportManager.mdZoomRatio = m_dZoomRatio;

	if (HideWindowHandle > 0) {
		SetPart(HideWindowHandle, m_ViewRect.top, m_ViewRect.left, m_ViewRect.bottom, m_ViewRect.right);
		SetWindowExtents(HideWindowHandle, 0, 0, m_ClientRect.Width(), m_ClientRect.Height());
	}
}

void CInspectAdminViewHideDlg1::ViewRectSet()
{
	m_ViewRect.bottom = m_ViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = m_ViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectAdminViewHideDlg1::ScrollBarSet()
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

void CInspectAdminViewHideDlg1::ScrollBarPosSet()
{
	m_VScrBar.SetWindowPos(&wndTop, m_ClientRect.right, m_ClientRect.top, m_iScrBarWidth, m_ClientRect.bottom - m_iToolBarOffset, SWP_SHOWWINDOW);
	m_VScrBar.ShowScrollBar(TRUE);
	m_HScrBar.SetWindowPos(&wndTop, m_ClientRect.left, m_ClientRect.bottom, m_ClientRect.right, m_iScrBarWidth, SWP_SHOWWINDOW);
	m_HScrBar.ShowScrollBar(TRUE);
}

void CInspectAdminViewHideDlg1::DrawActiveTRegion()
{
	if (m_bDrawActiveTRegion) return;

	m_bDrawActiveTRegion = TRUE;
	CDC* pDC = GetDC();

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

void CInspectAdminViewHideDlg1::DrawSelectPartTRegion()
{
	CDC* pDC = GetDC();

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

void CInspectAdminViewHideDlg1::ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY)
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
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Strip View [Top Camera] - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

void CInspectAdminViewHideDlg1::OnTbDraw()
{
	m_ToolBarState = TS_DRAW;
}

void CInspectAdminViewHideDlg1::OnTbMove()
{
	m_ToolBarState = TS_MOVE;
}

void CInspectAdminViewHideDlg1::OnTbSelectPart()
{
	m_ToolBarState = TS_SELECT_PART;
}

void CInspectAdminViewHideDlg1::OnTbZoomIn()
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (int)(m_dZoomRatio * 10) * 0.1;

		if (m_dZoomRatio < 1.0) m_dZoomRatio += 0.1;
		else if (m_dZoomRatio < 2.0) m_dZoomRatio += 0.5;
		else if (m_dZoomRatio < 10.0) m_dZoomRatio += 1.0;
		else m_dZoomRatio = 10.0;

		ViewRectSet();
		ScrollBarSet();
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Strip View [Top Camera] - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

void CInspectAdminViewHideDlg1::OnTbZoomOut()
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
		OutTxt.Format("Strip View [Top Camera] - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

void CInspectAdminViewHideDlg1::OnTbFitWidth()
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (double)RECTWIDTH(&m_ClientRect) / (double)m_iImageWidth;

		InitViewRect();
		UpdateViewportManager();
		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Strip View [Top Camera] - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

void CInspectAdminViewHideDlg1::OnTbFitHeight()
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (double)RECTHEIGHT(&m_ClientRect) / (double)m_iImageHeight;

		InitViewRect();
		UpdateViewportManager();
		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Strip View [Top Camera] - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

int CInspectAdminViewHideDlg1::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1) return -1;

	try
	{
		GetClientRect(&m_ClientRect);
		OpenWindow(0, 0, m_ClientRect.right - m_ClientRect.left + 1, m_ClientRect.bottom - m_ClientRect.top + 1, (Hlong)m_hWnd, "invisible", "", &HideWindowHandle);

		return 0;
	}
	catch (HException& except)
	{
		strLog.Format("Halcon Exception [InspectAdminViewHideDlg1::OnCreate] : %s", (CString)(LPCTSTR)except.ErrorMessage());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return -1;
	}
}

void CInspectAdminViewHideDlg1::LoadScanImage(int nScanNum)
{

}

void CInspectAdminViewHideDlg1::MoveToDefectArea()
{

}

void CInspectAdminViewHideDlg1::RedrawView()
{
	InvalidateRect(NULL, FALSE);
}

void CInspectAdminViewHideDlg1::RedrawRect()
{
	InvalidateRect(&m_ClientRect, FALSE);
}

void CInspectAdminViewHideDlg1::DrawFrameActiveTRegion()
{
	CDC* pDC = GetDC();

	POINT ClientOffset;
	ClientOffset.x = 0;
	ClientOffset.y = m_iToolBarOffset;
	pDC->SetViewportOrg(ClientOffset);

	CRgn ClipRgn;

	if (mpActiveTRegion) {
		ClipRgn.CreateRectRgn(m_ClientRect.left, m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
		pDC->SelectClipRgn(&ClipRgn);
		mpActiveTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_ACTIVE, FALSE, THEAPP.m_pCalDataService);
	}
	ReleaseDC(pDC);
}

void CInspectAdminViewHideDlg1::OnTbLive()
{
}

void CInspectAdminViewHideDlg1::set_display_font(HalconCpp::HTuple WindowHandle, HalconCpp::HTuple Size, HalconCpp::HTuple Font,
	HalconCpp::HTuple Bold, HalconCpp::HTuple Slant)
{
	// Local control variables 
	HTuple  OS, Exception, AllowedFontSizes, Distances;
	HTuple  Indices, Result;

	GetSystem("operating_system", &OS);
	if ((Size == HTuple()) || (Size == (-1)))
	{
		Size = 16;
	}
	if ((OS.TupleSubstr(0, 2)) == HTuple("Win"))
	{
		//set font on Windows systems
		if (((Font == HTuple("mono")) || (Font == HTuple("Courier"))) || (Font == HTuple("courier")))
		{
			Font = "Courier New";
		}
		else if (Font == HTuple("sans"))
		{
			Font = "Arial";
		}
		else if (Font == HTuple("serif"))
		{
			Font = "Times New Roman";
		}
		if (Bold == HTuple("true"))
		{
			Bold = 1;
		}
		else if (Bold == HTuple("false"))
		{
			Bold = 0;
		}
		else
		{
			Exception = "Wrong value of control parameter Bold";
			throw HException(Exception);
		}
		if (Slant == HTuple("true"))
		{
			Slant = 1;
		}
		else if (Slant == HTuple("false"))
		{
			Slant = 0;
		}
		else
		{
			Exception = "Wrong value of control parameter Slant";
			throw HException(Exception);
		}
		try
		{
			//			HalconCpp::SetFont(WindowHandle, ((((((("-"+Font)+"-")+Size)+"-*-")+Slant)+"-*-*-")+Bold)+"-");

			if (Bold == 1)
			{
				if (Slant == 1)
					HalconCpp::SetFont(WindowHandle, (Font + "-BoldItalic-") + Size);
				else
					HalconCpp::SetFont(WindowHandle, (Font + "-Bold-") + Size);
			}
			else
			{
				if (Slant == 1)
					HalconCpp::SetFont(WindowHandle, (Font + "-Italic-") + Size);
				else
					HalconCpp::SetFont(WindowHandle, (Font + "-Normal-") + Size);
			}
		}
		// catch (Exception) 
		catch (HException& HDevExpDefaultException)
		{
			HDevExpDefaultException.ToHTuple(&Exception);
			throw HException(Exception);
		}
	}

	return;
}

// HInspectionTypeAlgoDetectionDefectRgn: Inspection Type별 알고리즘으로 검출된 불량영역
int CInspectAdminViewHideDlg1::WriteResultFile(int iInspectionType, int iPcVisionNo, int iVisionCamType, int iResultImageNo, HObject HDisplayImage, HObject HInspectionTypeAlgoDetectionDefectRgn, int* piDefectCodeIndex, CString sBarcodeName)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	try
	{
		const int STRING_LENGTH_SIZE_CHECK = 600;
		BOOL bResultGood = FALSE;

		*piDefectCodeIndex = -1;

		if (THEAPP.m_pGFunction->ValidHImage(HDisplayImage) == FALSE)
		{
			strLog.Format("CInspectAdminViewHideDlg1::WriteResultFile ==> Invalid Image");
			THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
			return 0;
		}

		CString sDefectResultFile = "";

		CString sInspectionType = g_strInspectionTypeName[iInspectionType].c_str();
		CString sInspectionType_Short = g_strInspectionTypeName_Short[iInspectionType].c_str();
		CString strVisionNameShortcut = sVisionCamType_Short.Left(1);

		BOOL bBarcodeResultNG;
		bBarcodeResultNG = FALSE;

		BOOL bBarcodeResultDisplay = FALSE;
		if (THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo] && iResultImageNo == THEAPP.m_ModelDefineInfo.m_iBarcodeResultImageNo[iPcVisionNo])
			bBarcodeResultDisplay = TRUE;

		BOOL bGapMeasureDisplay = FALSE;

		ClearWindow(HideWindowHandle);

		if (THEAPP.m_pGFunction->ValidHImage(HDisplayImage))
			DispObj(HDisplayImage, HideWindowHandle);

		if (bGapMeasureDisplay)
		{
			if (THEAPP.m_pGFunction->ValidHXLD(m_HGapMeasureLineXLD))
			{
				HObject HEachGapLineXLD;
				HTuple HNoGapLine;
				Hlong lNoGapLine;

				HalconCpp::CountObj(m_HGapMeasureLineXLD, &HNoGapLine);
				lNoGapLine = HNoGapLine.L();

				set_display_font(HideWindowHandle, 25, "mono", "true", "false");

				for (int ijk = 0; ijk < lNoGapLine; ijk++)
				{
					SelectObj(m_HGapMeasureLineXLD, &HEachGapLineXLD, ijk + 1);

					if (THEAPP.m_pGFunction->ValidHXLD(HEachGapLineXLD) == FALSE)
						continue;

					HalconCpp::SetColor(HideWindowHandle, "green");
					HalconCpp::DispObj(HEachGapLineXLD, HideWindowHandle);

					double dRow1, dRow2, dCol1, dCol2;
					HTuple HlRow1, HlRow2, HlCol1, HlCol2;

					SmallestRectangle1Xld(HEachGapLineXLD, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

					dRow1 = HlRow1.D();
					dCol1 = HlCol1.D();
					dRow2 = HlRow2.D();
					dCol2 = HlCol2.D();

					CString sDistanceMeasureResult;
					sDistanceMeasureResult.Format("%.1lf(um)", m_HGapMeasureResult[ijk].D());

					HalconCpp::SetTposition(HideWindowHandle, (long)dRow1 - 60, (long)dCol2 - 120);
					HalconCpp::WriteString(HideWindowHandle, HTuple(sDistanceMeasureResult));
				}
			}
		}

		if (bBarcodeResultDisplay)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(m_HBarcodeDetectRgn))
			{
				if (m_bBarcodeRegResult)
				{
					HalconCpp::SetColor(HideWindowHandle, "magenta");
					HalconCpp::SetDraw(HideWindowHandle, "margin");
					HalconCpp::DispObj(m_HBarcodeDetectRgn, HideWindowHandle);

					set_display_font(HideWindowHandle, 30, "mono", "true", "false");

					HalconCpp::SetColor(HideWindowHandle, "green");

					CString sBarcodeResult;
					HalconCpp::SetTposition(HideWindowHandle, m_ViewRect.top + 200, m_ViewRect.left + 50);
					sBarcodeResult.Format("바코드: %s", m_sBarcodeResult);
					HalconCpp::WriteString(HideWindowHandle, HTuple(sBarcodeResult));

					CString sBarcodeShiftResult, sBarcodeRotationResult;
					if (m_bBarcodeShiftNG)
					{
						bBarcodeResultNG = TRUE;
						sBarcodeShiftResult.Format("위치:NG(%d,%dum)", (int)(m_dBarcodePosOffsetX * 1000.), (int)(m_dBarcodePosOffsetY * 1000.));
					}
					else
						sBarcodeShiftResult.Format("위치:OK(%d,%dum)", (int)(m_dBarcodePosOffsetX * 1000.), (int)(m_dBarcodePosOffsetY * 1000.));

					if (m_bBarcodeRotationNG)
					{
						bBarcodeResultNG = TRUE;
						sBarcodeRotationResult.Format("회전:NG(%d도)", m_iBarcodeRotationResult);
					}
					else
						sBarcodeRotationResult.Format("회전:OK(%d도)", m_iBarcodeRotationResult);


					HalconCpp::SetTposition(HideWindowHandle, m_ViewRect.top + 400, m_ViewRect.left + 50);
					sBarcodeResult.Format("%s, %s", sBarcodeShiftResult, sBarcodeRotationResult);
					HalconCpp::WriteString(HideWindowHandle, HTuple(sBarcodeResult));
				}
				else
				{
					bBarcodeResultNG = TRUE;

					HalconCpp::SetColor(HideWindowHandle, "red");
					HalconCpp::SetDraw(HideWindowHandle, "margin");
					HalconCpp::DispObj(m_HBarcodeDetectRgn, HideWindowHandle);

					set_display_font(HideWindowHandle, 30, "mono", "true", "false");

					HTuple lTempArea, dTempRow, dTempCol;
					AreaCenter(m_HBarcodeDetectRgn, &lTempArea, &dTempRow, &dTempCol);

					//					SetTposition(HideWindowHandle, (Hlong)dTempRow.D(), (Hlong)dTempCol.D());
					HalconCpp::SetTposition(HideWindowHandle, m_ViewRect.top + 200, m_ViewRect.left + 50);
					HalconCpp::WriteString(HideWindowHandle, "바코드: 미인식");
				}
			}
		}

		int iLensDefectNumber;

		HObject DefectRegion;
		double dDefectCenterX, dDefectCenterY;

		int i, j, k;

		HObject HContaminationRgn;
		GenEmptyObj(&HContaminationRgn);
		HTuple HIsContamDefect;

		HObject HDisplayDefectRgn[MAX_DEFECT_NAME];

		if (THEAPP.m_pGFunction->ValidHRegion(HInspectionTypeAlgoDetectionDefectRgn))		// 알고리즘으로 검출된 Defect가 있으면 (불량이 아니더라도 알고리즘 검출 Defect 화면 표시하기 위해 여기로 들어와야 함)
		{
			GenEmptyObj(&DefectRegion);	// Total Defects

			// 검사조건을 통과한 불량 영역
			for (i = 0; i < MAX_DEFECT_NAME; i++)
			{
				if (iDefectSortingNumber[i] > 0)
				{
					ConcatObj(DefectRegion, m_HSpecApplyDefectRgn[i], &DefectRegion);
				}
			}

			HObject SelectRgn, OrgSelectRgn;
			HTuple OriSize, OriArea, Row, Column;
			HTuple CountDefect;
			long lNoDefectCount;
			HObject ConnectionsRegion;

			Connection(DefectRegion, &ConnectionsRegion);
			CountObj(ConnectionsRegion, &CountDefect);		// Total Defect Blobs

			lNoDefectCount = CountDefect[0].I();

			set_display_font(HideWindowHandle, RESULT_VIEW_MEASURE_FONT_SIZE, "mono", "true", "false");
			SetColor(HideWindowHandle, "green");

			// ************ Defect 이 겹치게 그려짐을 막기 위해 Flag 배열을 만듦 ********

			HTuple HSizeArea, HSizeCenterX, HSizeCenterY, HSizeBiggerAreaIndex;
			Hlong lBiggerDefectIndex_i, lBiggerDefectIndex_j;
			BOOL bUseDefectSizeLimit = FALSE;

			if (lNoDefectCount > 0)
			{
				if (THEAPP.Struct_PreferenceStruct.m_bUseResultImageDefectSizeLimit)
				{
					if (lNoDefectCount > THEAPP.Struct_PreferenceStruct.m_iResultImageDefectSizeMax)
					{
						bUseDefectSizeLimit = TRUE;

						AreaCenter(ConnectionsRegion, &HSizeArea, &HSizeCenterY, &HSizeCenterX);
						TupleSortIndex(HSizeArea, &HSizeBiggerAreaIndex);
						TupleInverse(HSizeBiggerAreaIndex, &HSizeBiggerAreaIndex);

						lNoDefectCount = THEAPP.Struct_PreferenceStruct.m_iResultImageDefectSizeMax;
					}
				}

				int* nDefectFlag = NULL;

				nDefectFlag = new int[lNoDefectCount];

				// 그린 것에 대해 Flag 를 on 시킴
				for (i = 0; i < lNoDefectCount; i++)
				{
					nDefectFlag[i] = 0;
				}

				int nRow = 0, nColumn = 0;
				int nCompRow = 0, nCompColumn = 0;

				Point pt1, pt2;
				pt1.x = 0, pt1.y = 0;
				pt2.x = 0, pt2.y = 0;

				double dblDist = 0.0;
				int iXCenterDist, iYCenterDist;
				HTuple CompSize, CompArea, CompRow, CompColumn;

				Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX, lDefectArea, lXLength, lYLength, lOrgDefectArea, lOrgXLength, lOrgYLength;
				HTuple HLTPointY, HLTPointX, HRBPointY, HRBPointX;

				for (i = 0; i < lNoDefectCount; i++)
				{
					if (bUseDefectSizeLimit)
					{
						lBiggerDefectIndex_i = HSizeBiggerAreaIndex[i].L();
						SelectObj(ConnectionsRegion, &OrgSelectRgn, lBiggerDefectIndex_i + 1);
					}
					else
					{
						SelectObj(ConnectionsRegion, &OrgSelectRgn, i + 1);
					}

					if (THEAPP.m_pGFunction->ValidHRegion(OrgSelectRgn) == FALSE)
						continue;

					AreaCenter(OrgSelectRgn, &OriSize, &Row, &Column);

					lOrgDefectArea = OriSize[0].L();
					if (lOrgDefectArea > 0)
					{
						SmallestRectangle1(OrgSelectRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX);
						lLTPointY = HLTPointY[0].L();
						lLTPointX = HLTPointX[0].L();
						lRBPointY = HRBPointY[0].L();
						lRBPointX = HRBPointX[0].L();
						lOrgXLength = lRBPointX - lLTPointX + 1;
						lOrgYLength = lRBPointY - lLTPointY + 1;
					}

					dDefectCenterX = Column[0].D();
					dDefectCenterY = Row[0].D();

					Row += 5; Column += 5;

					nRow = Row[0].D();
					nColumn = Column[0].D();

					pt1.x = nColumn; pt1.y = nRow;

					for (j = 0; j < lNoDefectCount; j++)
					{
						if (i != j)
						{
							if (bUseDefectSizeLimit)
							{
								lBiggerDefectIndex_j = HSizeBiggerAreaIndex[j].L();
								SelectObj(ConnectionsRegion, &SelectRgn, lBiggerDefectIndex_j + 1);
							}
							else
							{
								SelectObj(ConnectionsRegion, &SelectRgn, j + 1);
							}

							if (THEAPP.m_pGFunction->ValidHRegion(SelectRgn) == FALSE)
								continue;

							AreaCenter(SelectRgn, &CompSize, &CompRow, &CompColumn);
							CompRow += 5; CompColumn += 5;

							nCompRow = CompRow[0].D();
							nCompColumn = CompColumn[0].D();

							pt2.x = nCompColumn; pt2.y = nCompRow;

							iXCenterDist = abs(pt1.x - pt2.x);
							iYCenterDist = abs(pt1.y - pt2.y);

							// 중심 간의 거리가 설정 값 보다 작다면
							if (iXCenterDist < 250 && iYCenterDist < THEAPP.Struct_PreferenceStruct.m_iDefectDispDist)
							{
								// 어떠한 것도 string 을 쓰지 않았다면
								if (nDefectFlag[j] == 0)
								{
									// 상측으로 이동시 0 이하의 값이 나오면
									if (pt2.y <= pt1.y)
									{
										if ((CompRow - THEAPP.Struct_PreferenceStruct.m_iDefectDispDist) > 0)
											CompRow = CompRow - THEAPP.Struct_PreferenceStruct.m_iDefectDispDist;
										else
											CompRow = 0;
									}
									else
									{
										CompRow = CompRow + THEAPP.Struct_PreferenceStruct.m_iDefectDispDist;
									}

									//210319 jwj add(결과이미지에 글씨 잘리는 현상 개선)
									if (CompColumn + STRING_LENGTH_SIZE_CHECK > m_ViewportManager.miImageWidth)
										CompColumn -= STRING_LENGTH_SIZE_CHECK;
									//210319 jwj end

									SetTposition(HideWindowHandle, CompRow, CompColumn);
									CompArea = ceil((CompSize[0].D() * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelArea()) / 1000) / 1000;

									//////////////////////////////////////////////////////////////////////////
									SmallestRectangle1(SelectRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX);
									lLTPointY = HLTPointY[0].L();
									lLTPointX = HLTPointX[0].L();
									lRBPointY = HRBPointY[0].L();
									lRBPointX = HRBPointX[0].L();
									lXLength = lRBPointX - lLTPointX + 1;
									lYLength = lRBPointY - lLTPointY + 1;
									lXLength = ceil((double)lXLength * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelSize());
									lYLength = ceil((double)lYLength * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelSize());

									SetColor(HideWindowHandle, "green");

									TestSubsetRegion(SelectRgn, HContaminationRgn, &HIsContamDefect);

									if (THEAPP.Struct_PreferenceStruct.m_iResultImageDefectDisplayType == RESULT_IMAGE_DEFECT_DISPLAY_SIZE)
									{
										if (HIsContamDefect == 1)
										{
											WriteString(HideWindowHandle, CompArea + "mm2");
										}
										else
										{
											CString sMeasureValue;
											sMeasureValue.Format("%d,%d um", lXLength, lYLength);
											WriteString(HideWindowHandle, HTuple(sMeasureValue));
										}
									}
									else
									{
										WriteString(HideWindowHandle, CompArea + "mm2");
									}

									//////////////////////////////////////////////////////////////////////////

									nDefectFlag[j] = 1;
								}
							}
						}
					}

					// 어떠한 것도 string 을 쓰지 않았다면 
					if (nDefectFlag[i] == 0)
					{
						//210319 jwj add(결과이미지에 글씨 잘리는 현상 개선)
						if (Column + STRING_LENGTH_SIZE_CHECK > m_ViewportManager.miImageWidth)
							Column -= STRING_LENGTH_SIZE_CHECK;
						//210319 jwj end

						SetTposition(HideWindowHandle, Row, Column);
						OriArea = ceil((OriSize[0].D() * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelArea()) / 1000) / 1000;

						//////////////////////////////////////////////////////////////////////////
						lOrgXLength = ceil((double)lOrgXLength * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelSize());
						lOrgYLength = ceil((double)lOrgYLength * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelSize());

						SetColor(HideWindowHandle, "green");

						TestSubsetRegion(OrgSelectRgn, HContaminationRgn, &HIsContamDefect);

						if (THEAPP.Struct_PreferenceStruct.m_iResultImageDefectDisplayType == RESULT_IMAGE_DEFECT_DISPLAY_SIZE)
						{
							if (HIsContamDefect == 1)
							{
								WriteString(HideWindowHandle, OriArea + "mm2");
							}
							else
							{
								CString sMeasureValue;
								sMeasureValue.Format("%d,%d um", lOrgXLength, lOrgYLength);
								WriteString(HideWindowHandle, HTuple(sMeasureValue));
							}
						}
						else
						{
							WriteString(HideWindowHandle, OriArea + "mm2");
						}

						//////////////////////////////////////////////////////////////////////////

						nDefectFlag[i] = 1;
					}

					if ((i % 2) == 0)
						Sleep(1);
				}

				SAFE_DELETE(nDefectFlag);
			}

			// Display

			// m_HResultDefectRgn: Final Result (All Defects)
			Union1(DefectRegion, &m_HResultDefectRgn);

			SetColor(HideWindowHandle, "magenta");
			SetDraw(HideWindowHandle, "margin");
			DispObj(HInspectionTypeAlgoDetectionDefectRgn, HideWindowHandle);		// 모든 알고리즘 검출 Defect 디스플레이: 불량으로 검출 안된 영역도 표시함...

			for (i = 0; i < MAX_DEFECT_NAME; i++)
			{
				GenEmptyObj(&(HDisplayDefectRgn[i]));
				m_iDefect[i] = DEFECT_TYPE_GOOD;
			}

			m_iInspectResult = DEFECT_TYPE_GOOD;

			for (i = 0; i < MAX_DEFECT_NAME; i++)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && THEAPP.m_pGFunction->ValidHRegion(m_HSpecApplyDefectRgn[i]))
				{
					Intersection(m_HResultDefectRgn, m_HSpecApplyDefectRgn[i], &(HDisplayDefectRgn[i]));

					if (THEAPP.m_pGFunction->ValidHRegion(HDisplayDefectRgn[i]) == TRUE)
					{
						SetColor(HideWindowHandle, "red");
						DispObj(HDisplayDefectRgn[i], HideWindowHandle);

						if (iDefectSortingNumber[i] > 0)
						{
							m_iInspectResult = i;
							m_iDefect[i] = i;
						}
					}
				}
			}

			DecideInspectResult(iInspectionType);

			/////////// 결과를 text로 표기 /////////////////////////
			set_display_font(HideWindowHandle, OVERLAY_IMAGE_RESULT_FONT_SIZE, "mono", "true", "false");
			SetTposition(HideWindowHandle, m_ViewRect.top + 50, m_ViewRect.left + 50);

			HTuple HDefectName;

			if (m_iInspectResult == DEFECT_TYPE_GOOD)
			{
				if (bBarcodeResultNG)
				{
					HalconCpp::SetColor(HideWindowHandle, "red");

					if (m_bBarcodeRegResult == FALSE)
					{
						HalconCpp::WriteString(HideWindowHandle, "Barcode Read (식각, 오염) NG");
						*piDefectCodeIndex = DEFECT_CODE_BARCODE_READ;
						sDefectResultFile.Format("BA-%s-%s", strVisionNameShortcut, THEAPP.m_strDefectCode[DEFECT_CODE_BARCODE_READ]);
					}
					else if (m_bBarcodeShiftNG == TRUE)
					{
						HalconCpp::WriteString(HideWindowHandle, "Barcode Position NG");
						*piDefectCodeIndex = DEFECT_CODE_BARCODE_SHIFT;
						sDefectResultFile.Format("BA-%s-%s", strVisionNameShortcut, THEAPP.m_strDefectCode[DEFECT_CODE_BARCODE_SHIFT]);
					}
					else if (m_bBarcodeRotationNG == TRUE)
					{
						HalconCpp::WriteString(HideWindowHandle, "Barcode Rotation NG");
						*piDefectCodeIndex = DEFECT_CODE_BARCODE_ROTATION;
						sDefectResultFile.Format("BA-%s-%s", strVisionNameShortcut, THEAPP.m_strDefectCode[DEFECT_CODE_BARCODE_ROTATION]);
					}
				}
				else
				{
					HDefectName = "양품";
					HalconCpp::SetColor(HideWindowHandle, "blue");
					HalconCpp::WriteString(HideWindowHandle, "양품");

					bResultGood = TRUE;
				}
			}
			else
			{
				CString sDefectResult;
				sDefectResult = THEAPP.CheckDefectNameChangeND(sInspectionType, THEAPP.m_strDefectName[m_iInspectResult]);
				HDefectName = HTuple(sDefectResult);
				SetColor(HideWindowHandle, "red");
				WriteString(HideWindowHandle, HDefectName);

				*piDefectCodeIndex = m_iInspectResult;
				sDefectResultFile.Format("%s-%s-%s", sInspectionType_Short, strVisionNameShortcut, THEAPP.m_strDefectCode[m_iInspectResult]);
			}
		} // if (THEAPP.m_pGFunction->ValidHRegion(HInspectionTypeAlgoDetectionDefectRgn))		// 알고리즘으로 검출된 Defect가 있으면
		else		// 양품 (알고리즘으로 검출된 Defect가 없음)
		{
			if (bBarcodeResultNG)
			{
				/////////// 결과를 text로 표기 /////////////////////////
				set_display_font(HideWindowHandle, OVERLAY_IMAGE_RESULT_FONT_SIZE, "mono", "true", "false");
				HalconCpp::SetTposition(HideWindowHandle, m_ViewRect.top + 15, m_ViewRect.left + 50);

				HalconCpp::SetColor(HideWindowHandle, "red");

				if (m_bBarcodeRegResult == FALSE)
				{
					HalconCpp::WriteString(HideWindowHandle, "Barcode Read (식각, 오염) NG");
					*piDefectCodeIndex = DEFECT_CODE_BARCODE_READ;
					sDefectResultFile.Format("BA-%s-%s", strVisionNameShortcut, THEAPP.m_strDefectCode[DEFECT_CODE_BARCODE_READ]);
				}
				else if (m_bBarcodeShiftNG == TRUE)
				{
					HalconCpp::WriteString(HideWindowHandle, "Barcode Position NG");
					*piDefectCodeIndex = DEFECT_CODE_BARCODE_SHIFT;
					sDefectResultFile.Format("BA-%s-%s", strVisionNameShortcut, THEAPP.m_strDefectCode[DEFECT_CODE_BARCODE_SHIFT]);
				}
				else if (m_bBarcodeRotationNG == TRUE)
				{
					HalconCpp::WriteString(HideWindowHandle, "Barcode Rotation NG");
					*piDefectCodeIndex = DEFECT_CODE_BARCODE_ROTATION;
					sDefectResultFile.Format("BA-%s-%s", strVisionNameShortcut, THEAPP.m_strDefectCode[DEFECT_CODE_BARCODE_ROTATION]);
				}
			}
			else
			{
				/////////// 결과를 text로 표기 /////////////////////////
				set_display_font(HideWindowHandle, OVERLAY_IMAGE_RESULT_FONT_SIZE, "mono", "true", "false");
				HalconCpp::SetTposition(HideWindowHandle, m_ViewRect.top + 15, m_ViewRect.left + 50);

				HalconCpp::SetColor(HideWindowHandle, "blue");
				HalconCpp::WriteString(HideWindowHandle, "양품");

				bResultGood = TRUE;
			}
		}

		HObject HImgDump_save;
		DumpWindowImage(&HImgDump_save, HideWindowHandle);

		CString Filename;
		Filename.Format("Module%d_%s_Result%d", m_pModuleNo, sVisionCamType_Short, iResultImageNo + 1);

		if (bResultGood == FALSE)
			Filename += "_NG";
		else
			Filename += "_OK";

		CString strSaveFileName;

		if ((!THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread && !THEAPP.Struct_PreferenceStruct.m_bUseResultImageThreadOnlyGood) ||
			(THEAPP.Struct_PreferenceStruct.m_bUseResultImageThreadOnlyGood && !bResultGood && bBarcodeResultNG))
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
			{
				auto log_time_start = std::chrono::high_resolution_clock::now();
				if (sDefectResultFile.GetLength() == 0)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bSaveResultGoodImage)
						WriteImage(HImgDump_save, "jpg", 0, HTuple(THEAPP.m_FileBase.m_strResultImageFolder_Local[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName));
				}
				else
					WriteImage(HImgDump_save, "jpg", 0, HTuple(THEAPP.m_FileBase.m_strResultImageFolder_Local[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName + "_" + sDefectResultFile));
				auto log_time_end = std::chrono::high_resolution_clock::now();
				auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

				if (sDefectResultFile.GetLength() == 0)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bSaveResultGoodImage)
					{
						strSaveFileName = THEAPP.m_FileBase.m_strResultImageFolder_Local[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName;
						strLog.Format("Save result image, Time(ms): %d, File name: %s", log_time_ms, strSaveFileName);
						THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
					}
				}
				else
				{
					strSaveFileName = THEAPP.m_FileBase.m_strResultImageFolder_Local[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName + "_" + sDefectResultFile;
					strLog.Format("Save result image, Time(ms): %d, File name: %s", log_time_ms, strSaveFileName);
					THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
				}
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS)
			{
				auto log_time_start = std::chrono::high_resolution_clock::now();
				if (sDefectResultFile.GetLength() == 0)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bSaveResultGoodImage)
						WriteImage(HImgDump_save, "jpg", 0, HTuple(THEAPP.m_FileBase.m_strResultImageFolder_LAS[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName));
				}
				else
					WriteImage(HImgDump_save, "jpg", 0, HTuple(THEAPP.m_FileBase.m_strResultImageFolder_LAS[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName + "_" + sDefectResultFile));
				auto log_time_end = std::chrono::high_resolution_clock::now();
				auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

				if (sDefectResultFile.GetLength() == 0)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bSaveResultGoodImage)
					{
						strSaveFileName = THEAPP.m_FileBase.m_strResultImageFolder_LAS[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName;
						strLog.Format("Save result image, Time(ms): %d, File name: %s", log_time_ms, strSaveFileName);
						THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
					}
				}
				else
				{
					strSaveFileName = THEAPP.m_FileBase.m_strResultImageFolder_LAS[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName + "_" + sDefectResultFile;
					strLog.Format("Save result image, Time(ms): %d, File name: %s", log_time_ms, strSaveFileName);
					THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
				}
			}
		}
		else
		{
			if (THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLocal)
			{
				if ((THEAPP.Struct_PreferenceStruct.m_bSaveResultGoodImage && sDefectResultFile.GetLength() == 0) || sDefectResultFile.GetLength() != 0)
				{
					RESULT_IMAGE_SAVE_PARAM* pSaveThreadIDParam = new RESULT_IMAGE_SAVE_PARAM;
					CopyImage(HImgDump_save, &(pSaveThreadIDParam->HSaveImage));
					if (sDefectResultFile.GetLength() == 0)
						pSaveThreadIDParam->sSavePath = THEAPP.m_FileBase.m_strResultImageFolder_Local[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName;
					else
						pSaveThreadIDParam->sSavePath = THEAPP.m_FileBase.m_strResultImageFolder_Local[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName + "_" + sDefectResultFile;
					THEAPP.m_pInspectService->AddListSaveResultImageParam(pSaveThreadIDParam, VISION_NUMBER_1, m_pMzNo, m_pTrayNo, m_pModuleNo);
				}
			}

			if (THEAPP.Struct_PreferenceStruct.m_bResultImageSaveLAS)
			{
				if ((THEAPP.Struct_PreferenceStruct.m_bSaveResultGoodImage && sDefectResultFile.GetLength() == 0) || sDefectResultFile.GetLength() != 0)
				{
					RESULT_IMAGE_SAVE_PARAM* pSaveThreadIDParam_Dual = new RESULT_IMAGE_SAVE_PARAM;
					CopyImage(HImgDump_save, &(pSaveThreadIDParam_Dual->HSaveImage));
					if (sDefectResultFile.GetLength() == 0)
						pSaveThreadIDParam_Dual->sSavePath = THEAPP.m_FileBase.m_strResultImageFolder_LAS[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName;
					else
						pSaveThreadIDParam_Dual->sSavePath = THEAPP.m_FileBase.m_strResultImageFolder_LAS[VISION_NUMBER_1][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeName + "_" + sDefectResultFile;
					THEAPP.m_pInspectService->AddListSaveResultImageParam(pSaveThreadIDParam_Dual, VISION_NUMBER_1, m_pMzNo, m_pTrayNo, m_pModuleNo);
				}
			}
		}

		ClearWindow(HideWindowHandle);

		return 1;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [InspectAdminViewHideDlg1::WriteResultFile] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return -1;
	}
}

void CInspectAdminViewHideDlg1::DecideInspectResult(int iInspectionType)
{
	if (m_iInspectResult == DEFECT_TYPE_GOOD)
		return;

	iInspectionType = 0;

	BOOL bResultSet;
	bResultSet = FALSE;

	int iDefectPriorityNum, iDefectIndex;

	for (iDefectPriorityNum = 0; iDefectPriorityNum < MAX_DEFECT_NAME; iDefectPriorityNum++)
	{
		for (iDefectIndex = 0; iDefectIndex < MAX_DEFECT_NAME; iDefectIndex++) {

			if (THEAPP.Struct_PreferenceStruct.iDefectPriority[iDefectIndex] == iDefectPriorityNum)
			{
				if (m_iDefect[iDefectIndex] >= 0)
				{
					m_iInspectResult = m_iDefect[iDefectIndex];
					bResultSet = TRUE;
				}
			}

			if (bResultSet == TRUE)
				break;
		}

		if (bResultSet == TRUE)
			break;
	}
}

int CInspectAdminViewHideDlg1::WriteSelectedRosReviewFile(int iPcVisionNo, int iVisionCamType, int iNoInspectionImage, HObject* pHDisplayImage, HObject* pHDefectRgn_ImageNo, CString sBarcodeID, int* piNoSaveReviewImage)
{
	(void)iVisionCamType;

	CString sVisionCamType, sVisionCamType_Short;
	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	*piNoSaveReviewImage = 0;

	struct ROS_REVIEW_GROUP_ITEM
	{
		HObject HDefectRgn;
		int iInspectImage = -1;
		int iSelectedImage = -1;
		int iInspectionTypeIndex = -1;
		int iDefectNameIndex = -1;
		int iCenterX = 0;
		int iCenterY = 0;
		int iLeft = 0;
		int iTop = 0;
		double dZoom = 1.0;
		double dArea = 0.0;
		BOOL bUseViewPort = FALSE;
		CString sInspectionTypeName;
		CString sInspectionTypeShort;
		CString sDefectDetail;
		CString sDefectCodeShort;
		CString sDefectFullCode;
		CString sSpec;
	};

	struct ROS_REVIEW_GROUP
	{
		std::vector<int> vecItemIndex;
		int iRepItemIndex = -1;
		int iSelectedImage = -1;
		int iLeft = 0;
		int iTop = 0;
		double dZoom = 1.0;
		BOOL bUseViewPort = FALSE;
		int iRepCenterX = 0;
		int iRepCenterY = 0;
		int iSourceInspectImage = -1;
	};

	std::vector<ROS_REVIEW_GROUP_ITEM> vecReviewItem;
	std::vector<ROS_REVIEW_GROUP> vecReviewGroup;
	std::vector<int> vecGroupCountPerInspectImage(iNoInspectionImage, 0);

	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(pHDisplayImage[0]) == FALSE)
		{
			strLog.Format("CInspectAdminViewHideDlg1::WriteSelectedRosReviewFile ==> Invalid Image");
			THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
			return 0;
		}

		HalconCpp::SetDraw(HideWindowHandle, "margin");
		set_display_font(HideWindowHandle, ROS_IMAGE_REVIEW_FONT_SIZE, "mono", "true", "false");
		HalconCpp::SetLineStyle(HideWindowHandle, HTuple());

		auto ResolveReviewSetting = [&](const CString& sInspectionTypeName,
										const CString& sDefectDetail,
										int iDefaultImage,
										int iCenterX,
										int iCenterY,
										BOOL& bUseViewPortOut,
										int& iSelectedImageOut,
										double& dZoomOut,
										int& iLeftOut,
										int& iTopOut)
			{
				int iFoundReviewInfoIndex = -1;

				bUseViewPortOut = FALSE;
				iSelectedImageOut = iDefaultImage;
				dZoomOut = 1.0;
				iLeftOut = 0;
				iTopOut = 0;

				for (int iReviewInfo = 0; iReviewInfo < THEAPP.m_iMaxRosSaveDefect; iReviewInfo++)
				{
					if (sInspectionTypeName == THEAPP.m_Struct_ReViewImageInfo[iReviewInfo].m_sInspectionType &&
						sDefectDetail == THEAPP.m_Struct_ReViewImageInfo[iReviewInfo].m_sDefectName &&
						sVisionCamType_Short == THEAPP.m_Struct_ReViewImageInfo[iReviewInfo].m_sVisionCam)
					{
						iFoundReviewInfoIndex = iReviewInfo;
						break;
					}
				}

				if (iFoundReviewInfoIndex < 0)
				{
					dZoomOut = 1.0;
					iLeftOut = iCenterX - int((RECTHEIGHT(&m_ClientRect) / dZoomOut) * 0.5);
					iTopOut = iCenterY - int((RECTWIDTH(&m_ClientRect) / dZoomOut) * 0.5);

					if (iLeftOut < 0)
						iLeftOut = 0;
					if (iTopOut < 0)
						iTopOut = 0;
				}
				else
				{
					bUseViewPortOut = (THEAPP.m_Struct_ReViewImageInfo[iFoundReviewInfoIndex].m_iUseViewPortFlag == 1);

					if (THEAPP.m_Struct_ReViewImageInfo[iFoundReviewInfoIndex].m_iImageNo > 0)
						iSelectedImageOut = THEAPP.m_Struct_ReViewImageInfo[iFoundReviewInfoIndex].m_iImageNo - 1;
					else
						iSelectedImageOut = iDefaultImage;

					dZoomOut = THEAPP.m_Struct_ReViewImageInfo[iFoundReviewInfoIndex].m_fZoomRatio;
					if (dZoomOut <= 0.0)
						dZoomOut = 1.0;

					if (bUseViewPortOut)
					{
						iLeftOut = THEAPP.m_Struct_ReViewImageInfo[iFoundReviewInfoIndex].m_iStartX;
						iTopOut = THEAPP.m_Struct_ReViewImageInfo[iFoundReviewInfoIndex].m_iStartY;
					}
					else
					{
						iLeftOut = iCenterX - int((RECTHEIGHT(&m_ClientRect) / dZoomOut) * 0.5);
						iTopOut = iCenterY - int((RECTWIDTH(&m_ClientRect) / dZoomOut) * 0.5);

						if (iLeftOut < 0)
							iLeftOut = 0;
						if (iTopOut < 0)
							iTopOut = 0;
					}
				}

				if (iSelectedImageOut < 0 || iSelectedImageOut >= iNoInspectionImage)
					iSelectedImageOut = iDefaultImage;
			};

		auto IsSameGroup = [&](const ROS_REVIEW_GROUP& groupInfo, const ROS_REVIEW_GROUP_ITEM& itemInfo) -> BOOL
			{
				if (groupInfo.iSelectedImage != itemInfo.iSelectedImage)
					return FALSE;

				if (groupInfo.bUseViewPort || itemInfo.bUseViewPort)
				{
					double dZoomDiff = groupInfo.dZoom - itemInfo.dZoom;
					if (dZoomDiff < 0.0)
						dZoomDiff = -dZoomDiff;

					return (groupInfo.bUseViewPort == itemInfo.bUseViewPort &&
							groupInfo.iLeft == itemInfo.iLeft &&
							groupInfo.iTop == itemInfo.iTop &&
							dZoomDiff < 0.0001);
				}

				double dRangeX = double(groupInfo.iRepCenterX - groupInfo.iLeft) * 0.75;
				double dRangeY = double(groupInfo.iRepCenterY - groupInfo.iTop) * 0.75;

				return (itemInfo.iCenterX > groupInfo.iRepCenterX - dRangeX &&
						itemInfo.iCenterX < groupInfo.iRepCenterX + dRangeX &&
						itemInfo.iCenterY > groupInfo.iRepCenterY - dRangeY &&
						itemInfo.iCenterY < groupInfo.iRepCenterY + dRangeY);
			};

		auto AddOrMergeGroup = [&](int iItemIndex) -> BOOL
			{
				ROS_REVIEW_GROUP_ITEM& itemInfo = vecReviewItem[iItemIndex];

				for (size_t iGroup = 0; iGroup < vecReviewGroup.size(); iGroup++)
				{
					if (IsSameGroup(vecReviewGroup[iGroup], itemInfo) == FALSE)
						continue;

					vecReviewGroup[iGroup].vecItemIndex.push_back(iItemIndex);

					if (vecReviewItem[vecReviewGroup[iGroup].iRepItemIndex].dArea < itemInfo.dArea)
					{
						vecReviewGroup[iGroup].iRepItemIndex = iItemIndex;
						vecReviewGroup[iGroup].iRepCenterX = itemInfo.iCenterX;
						vecReviewGroup[iGroup].iRepCenterY = itemInfo.iCenterY;
						vecReviewGroup[iGroup].iLeft = itemInfo.iLeft;
						vecReviewGroup[iGroup].iTop = itemInfo.iTop;
						vecReviewGroup[iGroup].dZoom = itemInfo.dZoom;
						vecReviewGroup[iGroup].bUseViewPort = itemInfo.bUseViewPort;
						vecReviewGroup[iGroup].iSourceInspectImage = itemInfo.iInspectImage;
					}

					return TRUE;
				}

				if (itemInfo.iInspectImage >= 0 &&
					itemInfo.iInspectImage < (int)vecGroupCountPerInspectImage.size() &&
					vecGroupCountPerInspectImage[itemInfo.iInspectImage] >= THEAPP.Struct_PreferenceStruct.m_iMaxReviewImageNumber)
				{
					return FALSE;
				}

				ROS_REVIEW_GROUP newGroup;
				newGroup.vecItemIndex.push_back(iItemIndex);
				newGroup.iRepItemIndex = iItemIndex;
				newGroup.iSelectedImage = itemInfo.iSelectedImage;
				newGroup.iLeft = itemInfo.iLeft;
				newGroup.iTop = itemInfo.iTop;
				newGroup.dZoom = itemInfo.dZoom;
				newGroup.bUseViewPort = itemInfo.bUseViewPort;
				newGroup.iRepCenterX = itemInfo.iCenterX;
				newGroup.iRepCenterY = itemInfo.iCenterY;
				newGroup.iSourceInspectImage = itemInfo.iInspectImage;
				vecReviewGroup.push_back(newGroup);

				if (itemInfo.iInspectImage >= 0 && itemInfo.iInspectImage < (int)vecGroupCountPerInspectImage.size())
					++vecGroupCountPerInspectImage[itemInfo.iInspectImage];

				return TRUE;
			};

		auto MakeOverlayRegion = [&](HObject HSourceRgn, HObject* pHOverlayRgn)
			{
				HTuple HdCircleRow, HdCircleCol, HdCircleRadius;
				GenEmptyObj(pHOverlayRgn);

				if (THEAPP.Struct_PreferenceStruct.m_iReviewImageDisplayType == 0)
				{
					ShapeTrans(HSourceRgn, pHOverlayRgn, "outer_circle");
					InnerCircle(*pHOverlayRgn, &HdCircleRow, &HdCircleCol, &HdCircleRadius);

					if (HdCircleRadius.D() < ROS_DEFECT_CIRCLE_MIN_RADIUS)
						GenCircle(pHOverlayRgn, HdCircleRow, HdCircleCol, HTuple(ROS_DEFECT_CIRCLE_MIN_RADIUS));

					if (HdCircleRadius.D() > ROS_DEFECT_CIRCLE_MAX_RADIUS)
						GenCircle(pHOverlayRgn, HdCircleRow, HdCircleCol, HTuple(ROS_DEFECT_CIRCLE_MAX_RADIUS));
				}
				else if (THEAPP.Struct_PreferenceStruct.m_iReviewImageDisplayType == 1)
				{
					ShapeTrans(HSourceRgn, pHOverlayRgn, "rectangle1");
				}
				else
				{
					DilationCircle(HSourceRgn, pHOverlayRgn, HTuple(2));
				}
			};

		int i, j, k;
		HObject HIntersectRgn;
		HTuple HlBlobArea, HdBlobCenterX, HdBlobCenterY;
		Hlong lBlobArea, lMaxBlobArea;
		HObject HEachImageDefectRgn;
		HObject HConnRgn, HRosDisplayRgn;
		HTuple HNoDefect;
		Hlong lNoDefect;

		for (int iImageIdx = 0; iImageIdx < iNoInspectionImage; iImageIdx++)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(pHDefectRgn_ImageNo[iImageIdx]) == FALSE)
				continue;

			GenEmptyObj(&HEachImageDefectRgn);

			for (i = 0; i < MAX_INSPECTION_TYPE; i++)
			{
				for (j = 0; j < MAX_DEFECT_NAME; j++)
				{
					Intersection(pHDefectRgn_ImageNo[iImageIdx], m_HRosReviewSpecApplyDefectRgn[i][j], &HIntersectRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
						continue;

					HalconCpp::Connection(m_HRosReviewSpecApplyDefectRgn[i][j], &HConnRgn);
					SelectShapeProto(HConnRgn, HIntersectRgn, &HRosDisplayRgn, "overlaps_abs", 1, MAX_DEF);

					if (THEAPP.m_pGFunction->ValidHRegion(HRosDisplayRgn) == FALSE)
						continue;

					ConcatObj(HEachImageDefectRgn, HRosDisplayRgn, &HEachImageDefectRgn);
				}
			}

			CountObj(HEachImageDefectRgn, &HNoDefect);
			lNoDefect = HNoDefect[0].I();

			if (lNoDefect <= 0)
				continue;

			HTuple HArea, HCenterX, HCenterY;
			HTuple HBiggerAreaIndex;
			Hlong lBiggerDefectIndex;
			HObject HDisplayDefectRgn;

			AreaCenter(HEachImageDefectRgn, &HArea, &HCenterY, &HCenterX);
			TupleSortIndex(HArea, &HBiggerAreaIndex);
			TupleInverse(HBiggerAreaIndex, &HBiggerAreaIndex);

			for (k = 0; k < lNoDefect; k++)
			{
				lBiggerDefectIndex = HBiggerAreaIndex[k].L();
				SelectObj(HEachImageDefectRgn, &HDisplayDefectRgn, lBiggerDefectIndex + 1);

				lMaxBlobArea = 0;
				int iMaxDefectInspectonTypeIndex = -1;
				int iMaxDefectDefectNameIndex = -1;

				for (i = 0; i < MAX_INSPECTION_TYPE; i++)
				{
					for (j = 0; j < MAX_DEFECT_NAME; j++)
					{
						Intersection(HDisplayDefectRgn, m_HRosReviewSpecApplyDefectRgn[i][j], &HIntersectRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn) == FALSE)
							continue;

						AreaCenter(HIntersectRgn, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
						lBlobArea = HlBlobArea.L();

						if (lBlobArea > lMaxBlobArea)
						{
							lMaxBlobArea = lBlobArea;
							iMaxDefectInspectonTypeIndex = i;
							iMaxDefectDefectNameIndex = j;
						}
					}
				}

				if (iMaxDefectInspectonTypeIndex < 0 || iMaxDefectDefectNameIndex < 0)
					continue;

				CString sInspectionTypeName = g_strInspectionTypeName[iMaxDefectInspectonTypeIndex].c_str();
				CString sInspectionTypeName_Short = g_strInspectionTypeName_Short[iMaxDefectInspectonTypeIndex].c_str();
				CString strVisionNameShortcut = sVisionCamType_Short.Left(1);
				CString sDefectDetail;
				CString sDefectDetail_Short;
				CString sDefectFullCode;
				CString strSpec;

				sDefectDetail.Format("%s", THEAPP.m_strDefectName[iMaxDefectDefectNameIndex]);
				sDefectDetail_Short.Format("%s", THEAPP.m_strDefectCode[iMaxDefectDefectNameIndex]);
				sDefectFullCode.Format("%s-%s-%s", sInspectionTypeName_Short, strVisionNameShortcut, sDefectDetail_Short);

				AreaCenter(HDisplayDefectRgn, &HArea, &HCenterY, &HCenterX);

				int iBlobCenterX = int(HCenterX.D());
				int iBlobCenterY = int(HCenterY.D());

				BOOL bUseViewPortCur = FALSE;
				int iSelectedImageCur = iImageIdx;
				int iLeftCur = 0;
				int iTopCur = 0;
				double dZoomCur = 1.0;

				ResolveReviewSetting(sInspectionTypeName,
									 sDefectDetail,
									 iImageIdx,
									 iBlobCenterX,
									 iBlobCenterY,
									 bUseViewPortCur,
									 iSelectedImageCur,
									 dZoomCur,
									 iLeftCur,
									 iTopCur);

				HTuple HLTPointY, HLTPointX, HRBPointY, HRBPointX, HDiameter;
				HTuple HdCircleRow, HdCircleCol, HdCircleRadius;
				double dArea;
				double dXlength;
				double dYLength;
				double dBlobLength;
				double dBlobWidth;

				DiameterRegion(HDisplayDefectRgn, &HLTPointY, &HLTPointX, &HRBPointY, &HRBPointX, &HDiameter);
				dBlobLength = HDiameter.D();
				InnerCircle(HDisplayDefectRgn, &HdCircleRow, &HdCircleCol, &HdCircleRadius);
				dBlobWidth = HdCircleRadius.D() * 2.0;

				dArea = ceil((HArea[0].D() * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelArea()) / 1000) / 1000;
				dXlength = dBlobLength * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelSize() * 0.001;
				dYLength = dBlobWidth * THEAPP.m_pCalDataService_N[iPcVisionNo]->GetPixelSize() * 0.001;
				strSpec.Format("%.3lf(mm2)_%.3lf(mm)_%.3lf(mm)", dArea, dXlength, dYLength);

				ROS_REVIEW_GROUP_ITEM itemInfo;
				itemInfo.HDefectRgn = HDisplayDefectRgn;
				itemInfo.iInspectImage = iImageIdx;
				itemInfo.iSelectedImage = iSelectedImageCur;
				itemInfo.iInspectionTypeIndex = iMaxDefectInspectonTypeIndex;
				itemInfo.iDefectNameIndex = iMaxDefectDefectNameIndex;
				itemInfo.iCenterX = iBlobCenterX;
				itemInfo.iCenterY = iBlobCenterY;
				itemInfo.iLeft = iLeftCur;
				itemInfo.iTop = iTopCur;
				itemInfo.dZoom = dZoomCur;
				itemInfo.dArea = dArea;
				itemInfo.bUseViewPort = bUseViewPortCur;
				itemInfo.sInspectionTypeName = sInspectionTypeName;
				itemInfo.sInspectionTypeShort = sInspectionTypeName_Short;
				itemInfo.sDefectDetail = sDefectDetail;
				itemInfo.sDefectCodeShort = sDefectDetail_Short;
				itemInfo.sDefectFullCode = sDefectFullCode;
				itemInfo.sSpec = strSpec;

				vecReviewItem.push_back(itemInfo);

				if (AddOrMergeGroup((int)vecReviewItem.size() - 1) == FALSE)
					vecReviewItem.pop_back();
			}
		}

		for (size_t iGroup = 0; iGroup < vecReviewGroup.size(); iGroup++)
		{
			const ROS_REVIEW_GROUP& groupInfo = vecReviewGroup[iGroup];
			const ROS_REVIEW_GROUP_ITEM& repItem = vecReviewItem[groupInfo.iRepItemIndex];

			if (groupInfo.iSelectedImage < 0 || groupInfo.iSelectedImage >= iNoInspectionImage)
				continue;

			if (THEAPP.m_pGFunction->ValidHImage(pHDisplayImage[groupInfo.iSelectedImage]) == FALSE)
				continue;

			ClearWindow(HideWindowHandle);
			if (groupInfo.bUseViewPort)
				OverlaySetViewportManager(groupInfo.iLeft, groupInfo.iTop, groupInfo.dZoom);
			else
				OverlaySetCenterViewportManager(groupInfo.iRepCenterX, groupInfo.iRepCenterY, groupInfo.dZoom);

			DispObj(pHDisplayImage[groupInfo.iSelectedImage], HideWindowHandle);

			HObject HDisplayGreenRgn, HDisplayRedRgn, HDisplayBoldRedRgn;
			GenEmptyObj(&HDisplayGreenRgn);
			GenEmptyObj(&HDisplayRedRgn);
			GenEmptyObj(&HDisplayBoldRedRgn);

			for (size_t iItem = 0; iItem < groupInfo.vecItemIndex.size(); iItem++)
			{
				int iItemIndex = groupInfo.vecItemIndex[iItem];
				HObject HOverlayRgn;
				GenEmptyObj(&HOverlayRgn);
				MakeOverlayRegion(vecReviewItem[iItemIndex].HDefectRgn, &HOverlayRgn);

				if (iItemIndex == groupInfo.iRepItemIndex)
					ConcatObj(HDisplayBoldRedRgn, HOverlayRgn, &HDisplayBoldRedRgn);
				else if (vecReviewItem[iItemIndex].sDefectFullCode == repItem.sDefectFullCode)
					ConcatObj(HDisplayRedRgn, HOverlayRgn, &HDisplayRedRgn);
				else
					ConcatObj(HDisplayGreenRgn, HOverlayRgn, &HDisplayGreenRgn);
			}

			Union1(HDisplayGreenRgn, &HDisplayGreenRgn);
			Union1(HDisplayRedRgn, &HDisplayRedRgn);
			Union1(HDisplayBoldRedRgn, &HDisplayBoldRedRgn);

			if (THEAPP.Struct_PreferenceStruct.m_iReviewImageDisplayType == 2)
				SetLineStyle(HideWindowHandle, (HTuple(5).Append(5)));
			else
				SetLineStyle(HideWindowHandle, HTuple());

			SetDraw(HideWindowHandle, "margin");

			SetColor(HideWindowHandle, "green");
			SetLineWidth(HideWindowHandle, 1);
			DispObj(HDisplayGreenRgn, HideWindowHandle);

			SetColor(HideWindowHandle, "red");
			SetLineWidth(HideWindowHandle, 2);
			DispObj(HDisplayRedRgn, HideWindowHandle);

			SetColor(HideWindowHandle, "red");
			SetLineWidth(HideWindowHandle, 5);
			DispObj(HDisplayBoldRedRgn, HideWindowHandle);
			SetLineWidth(HideWindowHandle, 1);

			HObject HImgDump_save;
			GenEmptyObj(&HImgDump_save);
			DumpWindowImage(&HImgDump_save, HideWindowHandle);

			CString Filename;
			Filename.Format("Module%d_%s_Image%02d_%s_rev",
							m_pModuleNo,
							sVisionCamType_Short,
							repItem.iInspectImage + 1,
							sBarcodeID);

			THEAPP.m_sROSDefectResult[m_pMzNo - 1][m_pTrayNo - 1][m_pModuleNo - 1][iPcVisionNo] =
				THEAPP.m_sROSDefectResult[m_pMzNo - 1][m_pTrayNo - 1][m_pModuleNo - 1][iPcVisionNo] + "," + repItem.sDefectFullCode;

			CString strSaveFileName;

			if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread)
			{
				if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
				{
					auto log_time_start = std::chrono::high_resolution_clock::now();
					WriteImage(HImgDump_save, "jpg", 0, HTuple(THEAPP.m_FileBase.m_strReviewImageFolder_Local[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + repItem.sDefectFullCode + "_" + repItem.sSpec + "_" + "Area00" + "_" + repItem.sDefectDetail));
					auto log_time_end = std::chrono::high_resolution_clock::now();
					auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

					strSaveFileName = THEAPP.m_FileBase.m_strReviewImageFolder_Local[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + repItem.sDefectFullCode + "_" + repItem.sSpec + "_" + "Area00" + "_" + repItem.sDefectDetail + ".jpg";
					THEAPP.m_FileBase.ValidateFileWriting(strSaveFileName);
					strLog.Format("Save grouped review image, Time(ms): %d, File name: %s", (int)log_time_ms, strSaveFileName);
					THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
				}

				if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
				{
					auto log_time_start = std::chrono::high_resolution_clock::now();
					WriteImage(HImgDump_save, "jpg", 0, HTuple(THEAPP.m_FileBase.m_strReviewImageFolder_LAS[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + repItem.sDefectFullCode + "_" + repItem.sSpec + "_" + "Area00" + "_" + repItem.sDefectDetail));
					auto log_time_end = std::chrono::high_resolution_clock::now();
					auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

					strSaveFileName = THEAPP.m_FileBase.m_strReviewImageFolder_LAS[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + repItem.sDefectFullCode + "_" + repItem.sSpec + "_" + "Area00" + "_" + repItem.sDefectDetail + ".jpg";
					THEAPP.m_FileBase.ValidateFileWriting(strSaveFileName);
					strLog.Format("Save grouped review image, Time(ms): %d, File name: %s", (int)log_time_ms, strSaveFileName);
					THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
				}
			}
			else
			{
				if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
				{
					REVIEW_IMAGE_SAVE_PARAM* pSaveThreadIDParam = new REVIEW_IMAGE_SAVE_PARAM;
					CopyImage(HImgDump_save, &(pSaveThreadIDParam->HSaveImage));
					pSaveThreadIDParam->sSavePath = THEAPP.m_FileBase.m_strReviewImageFolder_Local[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + repItem.sDefectFullCode + "_" + repItem.sSpec + "_" + "Area00" + "_" + repItem.sDefectDetail;
					THEAPP.m_pInspectService->AddListSaveReviewImageParam(pSaveThreadIDParam, VISION_NUMBER_1, m_pMzNo, m_pTrayNo, m_pModuleNo);
				}

				if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
				{
					REVIEW_IMAGE_SAVE_PARAM* pSaveThreadIDParam_Dual = new REVIEW_IMAGE_SAVE_PARAM;
					CopyImage(HImgDump_save, &(pSaveThreadIDParam_Dual->HSaveImage));
					pSaveThreadIDParam_Dual->sSavePath = THEAPP.m_FileBase.m_strReviewImageFolder_LAS[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + repItem.sDefectFullCode + "_" + repItem.sSpec + "_" + "Area00" + "_" + repItem.sDefectDetail;
					THEAPP.m_pInspectService->AddListSaveReviewImageParam(pSaveThreadIDParam_Dual, VISION_NUMBER_1, m_pMzNo, m_pTrayNo, m_pModuleNo);
				}
			}

			++(*piNoSaveReviewImage);

			ClearWindow(HideWindowHandle);
			GenEmptyObj(&HImgDump_save);
			Sleep(10);
		}

		return 1;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [InspectAdminViewHideDlg1::WriteSelectedRosReviewFile] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}
}

int CInspectAdminViewHideDlg1::WriteSelectedRosReviewFile_FAI(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo, int iVisionCamType, int iNoInspectionImage, HObject* pHDisplayImage, HObject(*ppHReviewRgn_FAI_Item)[10], POINT(*ppiViewportCenter_FAI_Item)[10], CString sBarcodeID, int* piNoSaveReviewImage)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

	*piNoSaveReviewImage = 0;

	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(pHDisplayImage[0]) == FALSE)
		{
			strLog.Format("CInspectAdminViewHideDlg1::WriteSelectedRosReviewFile_FAI ==> Invalid Image");
			THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));
			return 0;
		}

		int i, j, k;
		HObject HIntersectRgn;
		HTuple HlBlobArea, HdBlobCenterX, HdBlobCenterY;
		Hlong lBlobArea, lMaxBlobArea;

		HTuple HNoDefect;
		Hlong lNoDefect;

		int iType;
		CString sDefectResult, sDefectCodeResult;
		HObject HImgDump_save;
		HTuple HDefectName;
		HObject HConnRgn, HRosDisplayRgn, HCircleTransRgn, HSelectedRgn;
		HTuple HdCircleRow, HdCircleCol, HdCircleRadius;

		HalconCpp::SetDraw(HideWindowHandle, "margin");
		set_display_font(HideWindowHandle, ROS_IMAGE_REVIEW_FONT_SIZE, "mono", "true", "false");

		int iSelectedImage = -1;

		HTuple HArea, HCenterX, HCenterY, HLTPointX, HLTPointY, HRBPointX, HRBPointY, CompArea;
		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX, lXLength, lYLength;
		double	dArea;
		int iXlength, iYLength;
		CString strSpec;
		BOOL bAngleMeasure = FALSE;
		int iViewportCenterX, iViewportCenterY;
		int iDisplayImageIndex = -1;
		HObject HContourRgn;

		HObject HConcatImage, HCombineImage;

		//HalconCpp::SetLineStyle(HideWindowHandle, (HTuple(5).Append(5)));
		HalconCpp::SetLineWidth(HideWindowHandle, HTuple(3));

		HalconCpp::SetColor(HideWindowHandle, "red");

		BOOL bFaiResultFlag = TRUE;
		////////////////////////////////////////////////////////////////////////////

		int iNoCheckFaiParameter = MAX_FAI_PARAMETER;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
		const bool useFaiAngleMeasure = UseFaiAngleMeasure();

		for (int iFaiNumber = 0; iFaiNumber < MAX_FAI_ITEM; iFaiNumber++)
		{
			if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMInspectFai[iFaiNumber] == FALSE)
				continue;

			if (THEAPP.Struct_PreferenceStruct.m_bUseSaveFaiReviewImage == FALSE)
			{
				if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1).m_bParamResultNG[iFaiNumber] == FALSE)
					continue;
			}

			if (THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_bGenReviewImage == FALSE)
				continue;

			if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMInspectFai[iFaiNumber] == FALSE)
				continue;

			i = iFaiNumber;

			iNoCheckFaiParameter = 1;

			for (int iFaiParamIndex = 0; iFaiParamIndex < iNoCheckFaiParameter; iFaiParamIndex++)
			{
				bAngleMeasure = useFaiAngleMeasure && (iFaiNumber == 27 || iFaiNumber == 85);

				GenEmptyObj(&HConcatImage);

				for (int iReviewImageIndex = 0; iReviewImageIndex < THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iNoReviewImage; iReviewImageIndex++)
				{
					if (THEAPP.m_pGFunction->ValidHXLD(ppHReviewRgn_FAI_Item[iFaiNumber][iFaiParamIndex * MAX_FAI_REVIEW_IMAGE + iReviewImageIndex]) == FALSE)
						continue;

					if (THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iViewportCenter[iReviewImageIndex] == 1)
					{
						iViewportCenterX = ppiViewportCenter_FAI_Item[iFaiNumber][iFaiParamIndex * MAX_FAI_REVIEW_IMAGE + iReviewImageIndex].x;
						iViewportCenterY = ppiViewportCenter_FAI_Item[iFaiNumber][iFaiParamIndex * MAX_FAI_REVIEW_IMAGE + iReviewImageIndex].y;

						if (iViewportCenterX < 0)
							continue;
					}
					else
					{
						GenRegionContourXld(ppHReviewRgn_FAI_Item[iFaiNumber][iFaiParamIndex * MAX_FAI_REVIEW_IMAGE + iReviewImageIndex], &HContourRgn, "filled");
						Union1(HContourRgn, &HContourRgn);
						AreaCenter(HContourRgn, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
						iViewportCenterX = HdBlobCenterX[0].D();
						iViewportCenterY = HdBlobCenterY[0].D();
					}

					OverlaySetCenterViewportManager(iViewportCenterX, iViewportCenterY, THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_fZoomRatio[iReviewImageIndex]);

					ClearWindow(HideWindowHandle);

					HalconCpp::SetColor(HideWindowHandle, "red");

					iDisplayImageIndex = THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iImageNo[iReviewImageIndex] - 1;

					if (THEAPP.m_pGFunction->ValidHImage(pHDisplayImage[iDisplayImageIndex]))
						HalconCpp::DispObj(pHDisplayImage[iDisplayImageIndex], HideWindowHandle);
					else
						continue;

					HalconCpp::DispObj(ppHReviewRgn_FAI_Item[iFaiNumber][iFaiParamIndex * MAX_FAI_REVIEW_IMAGE + iReviewImageIndex], HideWindowHandle);

					//////////////////////////////////////////////////////////////////////////

					HalconCpp::DumpWindowImage(&HImgDump_save, HideWindowHandle);

					if (THEAPP.m_pGFunction->ValidHImage(HImgDump_save))
					{
						ConcatObj(HConcatImage, HImgDump_save, &HConcatImage);
					}

					ClearWindow(HideWindowHandle);

					Sleep(10);

				}	// for (int iReviewImageIndex = 0; iReviewImageIndex < THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iNoReviewImage; iReviewImageIndex++)

				// Save Review Image

				if (THEAPP.m_pGFunction->ValidHImage(HConcatImage))
				{
					TileImages(HConcatImage, &HCombineImage, NO_X_FAI_REVIEW_IMAGE, "horizontal");

					strSpec.Format("%s", THEAPP.m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_sFaiName);

					if (CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bParamResultNG[iFaiNumber] == TRUE)
					{
						strSpec.Format("%s-NG", strSpec);
					}
					else
					{
						strSpec.Format("%s-OK", strSpec);
					}

					if (bAngleMeasure)
						strSpec.Format("%s_A(%.3lf)", strSpec, CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][0]);
					else
						strSpec.Format("%s_L(%.3lf)", strSpec, CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_dFAIMeasureValue[iFaiNumber][0]);

					SYSTEMTIME time;
					GetLocalTime(&time);
					CString Filename;
					Filename.Format("Module%d_%s_Image00", m_pModuleNo, sVisionCamType_Short);

					CString strTimestamp;
					GetLocalTime(&time);
					strTimestamp.Format("%02d%02d%02d%03d", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

					CString	strFAIPath;
					CString strSaveFileName, strLog;

					if (!THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread)
					{
						if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
						{
							WriteImage(HCombineImage, "jpg", 0, HTuple(THEAPP.m_FileBase.m_strReviewImageFolder_Local[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeID + "_Review_" + strSpec));
							strSaveFileName = THEAPP.m_FileBase.m_strReviewImageFolder_Local[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeID + "_Review_" + strSpec + ".jpg";
							THEAPP.m_FileBase.ValidateFileWriting(strSaveFileName);

							++(*piNoSaveReviewImage);
						}

						if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
						{
							WriteImage(HCombineImage, "jpg", 0, HTuple(THEAPP.m_FileBase.m_strReviewImageFolder_LAS[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeID + "_Review_" + strSpec));
							strSaveFileName = THEAPP.m_FileBase.m_strReviewImageFolder_LAS[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeID + "_Review_" + strSpec + ".jpg";
							THEAPP.m_FileBase.ValidateFileWriting(strSaveFileName);

							//++(*piNoSaveReviewImage);
						}
					}
					else
					{
						int nCurQueueIndex;
						if (THEAPP.Struct_PreferenceStruct.m_bUseMultiQueue)
							nCurQueueIndex = (m_pTrayNo - 1) % THEAPP.Struct_PreferenceStruct.m_iThreadQueueNumber;
						else
							nCurQueueIndex = 0;

						if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLocal)
						{
							REVIEW_IMAGE_SAVE_PARAM* pSaveThreadIDParam = new REVIEW_IMAGE_SAVE_PARAM;
							CopyImage(HCombineImage, &(pSaveThreadIDParam->HSaveImage));
							pSaveThreadIDParam->sSavePath = THEAPP.m_FileBase.m_strReviewImageFolder_Local[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeID + "_Review_" + strSpec;
							THEAPP.m_pInspectService->AddListSaveReviewImageParam(pSaveThreadIDParam, VISION_NUMBER_1, m_pMzNo, m_pTrayNo, m_pModuleNo);

							++(*piNoSaveReviewImage);
						}

						if (THEAPP.Struct_PreferenceStruct.m_bReviewImageSaveLAS)
						{
							REVIEW_IMAGE_SAVE_PARAM* pSaveThreadIDParam_Dual = new REVIEW_IMAGE_SAVE_PARAM;
							CopyImage(HImgDump_save, &(pSaveThreadIDParam_Dual->HSaveImage));
							pSaveThreadIDParam_Dual->sSavePath = THEAPP.m_FileBase.m_strReviewImageFolder_LAS[iPcVisionNo][m_pMzNo - 1][m_pTrayNo - 1] + "\\" + Filename + "_" + sBarcodeID + "_Review_" + strSpec;
							THEAPP.m_pInspectService->AddListSaveReviewImageParam(pSaveThreadIDParam_Dual, VISION_NUMBER_1, m_pMzNo, m_pTrayNo, m_pModuleNo);

							//++(*piNoSaveReviewImage);
						}
					}

				}	// if (THEAPP.m_pGFunction->ValidHImage(HConcatImage))

			}	// for (int iFaiParamIndex = 0; iFaiParamIndex < MAX_FAI_PARAMETER; iFaiParamIndex++)

		}

		return 1;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [InspectAdminViewHideDlg1::WriteSelectedRosReviewFile_FAI] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return -1;
	}
}

BOOL CInspectAdminViewHideDlg1::ImageCheck(CString sImagePath)
{
	try
	{
		HObject HCheckImage;
		ReadImage(&HCheckImage, (HTuple)sImagePath);

		return TRUE;
	}
	catch (HException& except)
	{

		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		return FALSE;
	}
}
