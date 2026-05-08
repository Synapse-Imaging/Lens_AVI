
#ifndef CVIEWOV2_H
#define CVIEWOV2_H

#include "GViewPortManager.h"
#include "GTRegion.h"
#include "SelectPartRegion.h"
// InspectViewDlg1 대화 상자입니다.

class CInspectViewOverayImageDlg2 : public CDialog
{
	private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;
	static CInspectViewOverayImageDlg2	*m_pInstance;

public:
	static	CInspectViewOverayImageDlg2* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	
	typedef enum { TS_DRAW=0, TS_MOVE, TS_SELECT_PART } eTOOLBAR_STATE;
	
	void TbDraw();
	void TbMove();
	void TbSelectPart();
	void TbZoomIn();
	void TbZoomOut();
	void TbFitWidth();
	void TbFitHeight();

// Construction
public:
	CInspectViewOverayImageDlg2(CWnd* pParent = NULL);   // standard constructor

	int m_iImageWidth;						// Image Width
	int m_iImageHeight;					// Image Height
	void CallFitImageHeight();
	void CallFitImageWidth();
	void UpdateResultImage();

	HObject m_pHImage;
	CString m_sWindowName;

	int m_iCurResultNo; //1부터 Max Result No까지
	CMenu m_Menu;

// Dialog Data
	//{{AFX_DATA(CInspectViewOverayImageDlg2)
	enum { IDD = IDD_INSPECT_VIEW_OVERAY_IMAGE2_DLG };
	CScrollBar	m_VScrBar;
	CScrollBar	m_HScrBar;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectViewOverayImageDlg2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInspectViewOverayImageDlg2)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	BOOL	m_bOnPaintNow;
	BOOL	m_bDrawActiveTRegion;
	
	// ToolBar
	int				m_iToolBarOffset;
	eTOOLBAR_STATE	m_ToolBarState;
	BOOL			m_bOnMoving;
	
	// Model Data
	BOOL			m_bDisplayImage;
	
	GTRegion *mpSelectPartTRegion;
	GTRegion *mpActiveTRegion;
	
	//	HObject m_HImage;
	
	CPoint maMVPoints[2];	// View coordinate
	CPoint maMIPoints[2];	// Image coordinate
	
	void DrawSelectPartTRegion();
	void DrawActiveTRegion();
		
	int m_iGerberImageWidth;				// Gerber Image Width
	int m_iGerberImageHeight;				// Gerber Image Height
	
	double m_dZoomRatio;					// Image Zoom Ratio in Current View
	
	CRect m_ClientRect;						// Size of Current Dialog
	CRect m_ViewRect;						// Image Coordinate in Current View
	
	int m_iScrBarWidth;						// ScrollBar Width
	void ScrollBarSet();
	void ScrollBarPosSet();
	
	GViewportManager m_ViewportManager;		// Viewport Manager
	
	void InitViewRect();
	void UpdateViewportManager();
	
	Hlong m_lWindowID;
	
	void ViewRectSet();
	
	void ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY);

	HICON	m_HIconSelectedDefect;

public:
	void Show();
	BOOL ShowWindow(int nCmdShow);
	void Reset();
	void ClearAll();
	
	void LoadScanImage(int nScanNum);
	void MoveToDefectArea();

	void RedrawView();
	void RedrawRect();
	void UpdateView(BOOL bDelete);
};
#endif