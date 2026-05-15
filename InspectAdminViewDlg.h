
#ifndef CVIEW1_H
#define CVIEW1_H
//////////////////// 헤더 안 내용을 쓴다.

#pragma once

#define ROI_KEY_UP		0
#define ROI_KEY_DOWN	1
#define ROI_KEY_LEFT	2
#define ROI_KEY_RIGHT	3

#include "GViewPortManager.h"
#include "GTRegion.h"
#include "SelectPartRegion.h"
#include "ToolBarCombo.h"

// TODO : 공통부분 분리하여 관리 예정
#include "InspectionTypeExDlg.h"
#include "InspectionTypeChsDlg.h"
#include "InspectionTypeChsWZDlg.h"

// InspectViewDlg1 대화 상자입니다.

class CInspectAdminViewDlg : public CDialog
{
	private:
	
	CRect	m_ScreenRect;
	static CInspectAdminViewDlg	*m_pInstance;

public:
	static	CInspectAdminViewDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();

	void UpdateView(BOOL bUpdate=TRUE);
	void ClearUpdateView(BOOL bUpdate=TRUE);
	void ChangeViewport(int iCamImageWidth, int iCamImageHeight);

	void StartLive();
	void StopLive();
	
	typedef enum { TS_DRAW=0, TS_MOVE, TS_SELECT_PART, TS_CREATE_ROI_RECT, TS_CREATE_ROI_CIRCLE, TS_CREATE_ROI_POLYGON, TS_CREATE_ROI_CROSS, TS_CREATE_AF_ROI } eTOOLBAR_STATE;
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
// Construction
public:
	CInspectAdminViewDlg(CWnd* pParent = NULL);   // standard constructor

	BOOL	m_bOnPaintNow;

	BOOL m_bTestThread;

	int StartTime1,EndTime1;

	int m_pModuleNo;
	int TrayModuduleNumber;

	BOOL m_bOnOff;
	BOOL m_bLive;

	HObject m_pHLiveImage[CHANNEL_NUM];
	HObject m_HTeachingImage[CHANNEL_NUM][MAX_IMAGE_TAB];

	HObject m_HParameterDefectRgn;
	HObject m_HParameterDefectXLD;

	HObject m_HAIDefectRgn;

	HObject m_HAutoFocusRgn;
	HObject m_HAutoFocusCenterRgn;

	void ResetInspectionResult();

	//////////////////////////////////////////////////////////////////////////
	// Barcode Result
	HObject m_HBarcodeDetectRgn;
	//////////////////////////////////////////////////////////////////////////
	
	////////////////////////////
	// OCR Result
	HObject m_HOCRDetectRgn;//2017.04.11 hbk

	POINT CropLTPoint,CropRBPoint;

	DPOINT m_dLTPoint, m_dRBPoint;

	HObject m_HMultiModuleCropRgn;
	HObject m_HAutoFocusCheckRgn;
	HObject m_HShapeModelContrastRgn;
	HObject m_pThresRgn;
	HObject m_HInspectAreaRgn;
	HTuple m_lWindowID;
	
	void UpdateThresRegion(int Min,int Max);

	// Toolbar
	void TbDraw();
	void TbMove();
	void TbSelectPart();
	void TbROIRect();
	void TbROICircle();
	void TbROIPolygon();
	void TbROICross();
	void TbROIDelete();
	void TbZoomIn();
	void TbZoomOut();
	void TbFitWidth();
	void TbFitHeight();
	void TbLive();
	void TbDispCrossBar();
	void TbFitLineDisplay();
	void TbInspectAreaDisplay();
	void TbThresholdAreaDisplay();
	void TbDefectAreaDisplay();
	void Tb2pCaliper();
	void TbROILock();
	void TbAlignDisplay();
	void TbDisplayColor();
	void TbDisplayR();
	void TbDisplayG();
	void TbDisplayB();
	void TbDisplayH();
	void TbDisplayS();
	void TbDisplayI();
		
// Dialog Data
	//{{AFX_DATA(CInspectAdminViewDlg)
	enum { IDD = IDD_INSPECT_ADMIN_VIEW_DLG };
	CScrollBar	m_VScrBar;
	CScrollBar	m_HScrBar;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectAdminViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInspectAdminViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMenuAllRoiCopyVision1();
	afx_msg void OnMenuAllRoiCopyVision2();
	afx_msg void OnMenuRoiCopyVision1();
	afx_msg void OnMenuRoiCopyVision2();
	afx_msg void OnMenuRoiCopyImage0();
	afx_msg void OnMenuRoiCopyImage1();
	afx_msg void OnMenuRoiCopyImage2();
	afx_msg void OnMenuRoiCopyImage3();
	afx_msg void OnMenuRoiCopyImage4();
	afx_msg void OnMenuRoiCopyImage5();
	afx_msg void OnMenuRoiCopyImage6();
	afx_msg void OnMenuRoiCopyImage7();
	afx_msg void OnMenuRoiCopyImage8();
	afx_msg void OnMenuRoiCopyImage9();
	afx_msg void OnMenuRoiCopyImage10();
	afx_msg void OnMenuRoiCopyImage11();
	afx_msg void OnMenuRoiCopyImage12();
	afx_msg void OnMenuRoiCopyImage13();
	afx_msg void OnMenuRoiCopyImage14();
	afx_msg void OnMenuRoiCopyImage15();
	afx_msg void OnMenuRoiCopyImage16();
	afx_msg void OnMenuRoiCopyImage17();
	afx_msg void OnMenuRoiCopyImage18();
	afx_msg void OnMenuRoiCopyImage19();
	afx_msg void OnMenuRoiCopyImage20();
	afx_msg void OnMenuRoiCopyImage21();
	afx_msg void OnMenuRoiCopyImage22();
	afx_msg void OnMenuRoiCopyImage23();
	afx_msg void OnMenuRoiCopyImage24();
	afx_msg void OnMenuRoiCopyImage25();
	afx_msg void OnMenuRoiCopyImage26();
	afx_msg void OnMenuRoiCopyImage27();
	afx_msg void OnMenuRoiCopyImage28();
	afx_msg void OnMenuRoiCopyImage29();
	afx_msg void OnMenuRoiCopyImage30();
	afx_msg void OnMenuRoiCopyImage31();
	afx_msg void OnMenuRoiCopyImage32();
	afx_msg void OnMenuRoiCopyImage33();
	afx_msg void OnMenuRoiCopyImage34();
	afx_msg void OnMenuRoiCopyImage35();
	afx_msg void OnMenuRoiCopyImage36();
	afx_msg void OnMenuRoiCopyImage37();
	afx_msg void OnMenuRoiCopyImage38();
	afx_msg void OnMenuRoiCopyImage39();
	afx_msg void OnMenuRoiCopyImage40();
	afx_msg void OnMenuRoiCopyImage41();
	afx_msg void OnMenuRoiCopyImage42();
	afx_msg void OnMenuRoiCopyImage43();
	afx_msg void OnMenuRoiCopyImage44();
	afx_msg void OnMenuRoiCopyImage45();
	afx_msg void OnMenuAllRoiCopyImage0();
	afx_msg void OnMenuAllRoiCopyImage1();
	afx_msg void OnMenuAllRoiCopyImage2();
	afx_msg void OnMenuAllRoiCopyImage3();
	afx_msg void OnMenuAllRoiCopyImage4();
	afx_msg void OnMenuAllRoiCopyImage5();
	afx_msg void OnMenuAllRoiCopyImage6();
	afx_msg void OnMenuAllRoiCopyImage7();
	afx_msg void OnMenuAllRoiCopyImage8();
	afx_msg void OnMenuAllRoiCopyImage9();
	afx_msg void OnMenuAllRoiCopyImage10();
	afx_msg void OnMenuAllRoiCopyImage11();
	afx_msg void OnMenuAllRoiCopyImage12();
	afx_msg void OnMenuAllRoiCopyImage13();
	afx_msg void OnMenuAllRoiCopyImage14();
	afx_msg void OnMenuAllRoiCopyImage15();
	afx_msg void OnMenuAllRoiCopyImage16();
	afx_msg void OnMenuAllRoiCopyImage17();
	afx_msg void OnMenuAllRoiCopyImage18();
	afx_msg void OnMenuAllRoiCopyImage19();
	afx_msg void OnMenuAllRoiCopyImage20();
	afx_msg void OnMenuAllRoiCopyImage21();
	afx_msg void OnMenuAllRoiCopyImage22();
	afx_msg void OnMenuAllRoiCopyImage23();
	afx_msg void OnMenuAllRoiCopyImage24();
	afx_msg void OnMenuAllRoiCopyImage25();
	afx_msg void OnMenuAllRoiCopyImage26();
	afx_msg void OnMenuAllRoiCopyImage27();
	afx_msg void OnMenuAllRoiCopyImage28();
	afx_msg void OnMenuAllRoiCopyImage29();
	afx_msg void OnMenuAllRoiCopyImage30();
	afx_msg void OnMenuAllRoiCopyImage31();
	afx_msg void OnMenuAllRoiCopyImage32();
	afx_msg void OnMenuAllRoiCopyImage33();
	afx_msg void OnMenuAllRoiCopyImage34();
	afx_msg void OnMenuAllRoiCopyImage35();
	afx_msg void OnMenuAllRoiCopyImage36();
	afx_msg void OnMenuAllRoiCopyImage37();
	afx_msg void OnMenuAllRoiCopyImage38();
	afx_msg void OnMenuAllRoiCopyImage39();
	afx_msg void OnMenuAllRoiCopyImage40();
	afx_msg void OnMenuAllRoiCopyImage41();
	afx_msg void OnMenuAllRoiCopyImage42();
	afx_msg void OnMenuAllRoiCopyImage43();
	afx_msg void OnMenuAllRoiCopyImage44();
	afx_msg void OnMenuAllRoiCopyImage45();

	afx_msg void OnMenuRoiGrouping();
	afx_msg void OnMenuRoiChange();
	afx_msg void OnMenuRoiElipse();
	afx_msg void OnMenuAllRoiMove();
	afx_msg void OnMenuAllRoiScale();
	afx_msg void OnMenuROISizeChangeMM();
	afx_msg void OnMenuROISizeChangePIXEL();
	afx_msg void OnMenuROIMoveChangeMM();
	afx_msg void OnMenuROIMoveChangePIXEL();
	afx_msg void OnMenuROIIDChange();
	afx_msg void OnMenuROIGvDisplay();
	afx_msg void OnMenuLinkInfo();
	afx_msg void OnMenuLoadImage();
	afx_msg void OnMenuSaveAsTeachingImage();
	afx_msg void OnMenuDeleteRoiAll();
	afx_msg void OnMenuDeleteRoiTab();
	afx_msg void OnMenuRoiRotate();
	afx_msg void OnMenuRoiFlip();
	afx_msg void OnMenuAiRetrainRoi();
	afx_msg void OnMenuVmAutoTrainCheckAll();
	afx_msg void OnMenuVmAutoTrainUncheckAll();
	afx_msg void OnMenuVmAutoTrainStatus();
	afx_msg void OnMenuVmAllModelDelete();
	afx_msg void OnMenuFAIMeasureDisplay();
	afx_msg void OnMenuInspectLibrary();
	afx_msg void OnMenuGrabSequenceView();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT On2DFrameReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventGoToDefect(WPARAM wParam, LPARAM lParam);	
	DECLARE_MESSAGE_MAP()

protected:
	
	CMenu			m_Menu;		
	int				m_iRoiCopyImageIndex;
	int				m_iRoiCopyVisionIndex;

	// ToolBar
	int				m_iToolBarOffset;
	eTOOLBAR_STATE	m_ToolBarState;
	eTOOLBAR_STATE	m_PrevToolBarState;
	BOOL			m_bOnMoving;
	int				m_iDisplayChannelType;
	
	// Model Data
	BOOL			m_bDisplayImage;
	
	GTRegion *mpSelectPartTRegion;
	GTRegion *mpActiveTRegion;
	int miActiveCPointIndex;

	CPoint maMVPoints[2];	// View coordinate
	CPoint maMIPoints[2];	// Image coordinate
	
	void DrawSelectPartTRegion();
	void DrawActiveTRegion();
	
	int m_iImageWidth;						// Image Width
	int m_iImageHeight;						// Image Height
	
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
	
	void MoveActiveRegion(int iMoveDirection);
	void ScaleActiveRegion(int iMoveDirection, BOOL bScaleUp);
	
	void ViewRectSet();
	
	void ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY);

	BOOL	m_bDisplayFitLine;
	BOOL	m_bDisplayInspectArea;
	BOOL	m_bDisplayThresholdArea;
	BOOL	m_bDisplayDefectArea;
	BOOL	m_bROILock;
	BOOL	m_bAlignDisplay;

	int			m_caliper_control_flag;
	int			m_iNoCaliperPoint;
	CPoint	mpLineSP;
	CPoint	mpLineEP;
	void		DrawArrow(HTuple pDC);
	double	edLineLength ( double sx, double sy, double ex, double ey );

	// CrossBar
	BOOL		m_bDispCrossBar;
	HObject		m_HCrossBarXLD;

	// Avg GV Display
	BOOL		m_bDispROIGv;

	// FAI Measure Value Display
	BOOL		m_bDispFAIMeasureValue;

	// DontCare Process
	BOOL CheckDontCareInclusion(HObject HNewRegion);
	void GetRegionROIAfterDontCareRemoval(GTRegion *pNewTRegion);

	// Add Process
	BOOL CheckAddInclusion(HObject HNewRegion);
	void GetRegionROIAfterAddRemoval(GTRegion *pNewTRegion);
	void GetRegionROIAfterEllipse(GTRegion *pNewTRegion); //VER 1.1.3.5 add
	void GetRegionROIAfterRotation(GTRegion *pNewTRegion, double dRotationAngle);

public:

	void CreateAutoFocusROI();
	void DeleteCrossBarROI();
	void ClearAlignTabTempRegion();

	void SetToolBarStateDraw();
	void UpdateToolbarStatusTeachMode();

	void Show();
	void Reset();
	void ClearAll();
	
	void LoadScanImage(int nScanNum);
	void MoveToDefectArea();

	void RedrawView();
	void RedrawRect();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	void ClearAllSelectRegion();
	void ClearAllLastSelectRegion();
	void SaveInspectTabOverLastSelectRegion();
	void ClearActiveTRegion();

	// Polygon
	BOOL m_bPolygonMode;
	int m_iPolygonCnt;
	POINT m_PolygonPt[100];
	void DisplayCross(HDC hDestDC, GViewportManager *pVManager, double dXPos, double dYPos, COLORREF Color, long lRadiusPxl=5);
	void AddPolygonROI();
	void CopyInspectionROI();
	void CopyAllInspectionROI();

	void CopyInspectionROI_Vision();
	void CopyInspectionAllROI_Vision(int iVisionIndexToCopy, BOOL *pbApplyCopyImage, BOOL bDeleteFirst);

	// Histogram
	void UpdateHistogram(GTRegion *pActiveRegion);
	void AddRegionROI(int iThresMin, int iThresMax, BOOL bCheckMinBlob, BOOL bCheckMaxArea, BOOL bCheckFillUp, BOOL bCheckOpeningClosingInverse,
		BOOL bCheckOpening, BOOL bCheckClosing, BOOL bCheckConvex, BOOL bCheckRect, BOOL bCheckDifference, BOOL bCheckBoundary, BOOL bCheckDilation, int iEditMinBlobSize, int iEditOpeningSize, int iEditClosingSize, int iEditBoundaryDilationSize, int iEditDilationSize);
	void ShowRegionROI(int iThresMin, int iThresMax, BOOL bCheckMinBlob, BOOL bCheckMaxArea, BOOL bCheckFillUp, BOOL bCheckOpeningClosingInverse,
		BOOL bCheckOpening, BOOL bCheckClosing, BOOL bCheckConvex, BOOL bCheckRect, BOOL bCheckDifference, BOOL bCheckBoundary, BOOL bCheckDilation, int iEditMinBlobSize, int iEditOpeningSize, int iEditClosingSize, int iEditBoundaryDilationSize, int iEditDilationSize);

	// ROI Selection
	CDelegate delegateROISelected;
	CDelegate delegateCurROITabParamSave;
	GTRegion *mpLastSelectedRegion;

	void ResetManualBarcodeFlagForOCR();
	void TeachingImageGrabSave(BOOL bColor=TRUE);
	void LaserScanImageSave(int iStartImageIndex);
	int GetDisplayChannelType() { return m_iDisplayChannelType; }
	void UpdateHistogramImage();
	void SetDisplayChannel(int iDisplayChannel);
	BOOL GetDisplayFAIMeasureValue() { return m_bDispFAIMeasureValue; }

	BOOL DoCheckROISelected();
	DPOINT GetViewCenter();
	GTRegion* GetSelectedROI() { return mpActiveTRegion; }
	BOOL GetTbLock() { return m_bROILock; }

	// ROI Auto Create  // 241211 kdh
	CRgn m_cRgn;
	HRegion m_hRegion;
	GTRegion *mpAutoCreateRegion;
	bool m_bDrawRoi;
	void AddAutoDrawROI(HRegion HPolygonRgn);
	void DeleteAutoCreateROI();
	void SendImageToServer(int imageIndex, int iModelIndex, int channelType);

	CString strLog;
	CString strDialog;
	CString strMessageBox;
	void ChangeLanguage();

private:
	void AnalyzeAndDisplayDefects(HObject defectRegions, HTuple windowID) const;
	void AnalyzeAndDisplayAutoFocusCenter(HObject defectRegions, HTuple windowID) const;
};
#endif