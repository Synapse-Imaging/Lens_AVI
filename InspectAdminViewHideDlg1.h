
#ifndef CVIEWHIDE1_H
#define CVIEWHIDE1_H
//////////////////// 헤더 안 내용을 쓴다.

#pragma once

#include "InspectAdminViewHideDlg.h"
#include "GViewPortManager.h"
#include "GTRegion.h"
#include "SelectPartRegion.h"
#include "ToolBarCombo.h"

class CInspectAdminViewHideDlg1 : public CDialog, public IInspectAdminViewHideDlg
{
	private:
	
	CRect	m_ScreenRect;
	static CInspectAdminViewHideDlg1	*m_pInstance;

public:
	static	CInspectAdminViewHideDlg1* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	
	typedef enum { TS_DRAW=0, TS_MOVE, TS_SELECT_PART } eTOOLBAR_STATE;
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	void OverlaySetViewportManager(int iLeft, int iTop, double dZoomRatio);
	void OverlaySetCropViewportManager(CPoint cpCenterPoint, int iWidth, int iHeight);
	void OverlaySetCenterViewportManager(int iCenterX, int iCenterY, double dZoomRatio);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInspectAdminViewHideDlg1)
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
	afx_msg void OnTbDraw();
	afx_msg void OnTbMove();
	afx_msg void OnTbSelectPart();
	afx_msg void OnTbZoomIn();
	afx_msg void OnTbZoomOut();
	afx_msg void OnTbFitWidth();
	afx_msg void OnTbFitHeight();
	afx_msg void OnTbLive();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectAdminViewHideDlg1)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	
	BOOL	m_bDrawActiveTRegion;
	
	// ToolBar
	int				m_iToolBarOffset;
	eTOOLBAR_STATE	m_ToolBarState;
	BOOL			m_bOnMoving;
	
	// Model Data
	
	
	BOOL			m_bDisplayImage;
	
	GTRegion *mpSelectPartTRegion;
	GTRegion *mpActiveTRegion;
	
	CPoint maMVPoints[2];	// View coordinate
	CPoint maMIPoints[2];	// Image coordinate
	
	void DrawSelectPartTRegion();
	void DrawActiveTRegion();
	
	int m_iImageWidth;						// Image Width
	int m_iImageHeight;						// Image Height
	
	double m_dZoomRatio;					// Image Zoom Ratio in Current View
	
	CRect m_ClientRect;						// Size of Current Dialog
	CRect m_ViewRect;						// Image Coordinate in Current View
	
	int m_iScrBarWidth;						// ScrollBar Width
	void ScrollBarSet();
	void ScrollBarPosSet();
	
	GViewportManager m_ViewportManager;		// Viewport Manager
	
	void InitViewRect();
	void UpdateViewportManager();
	void ViewRectSet();
	
	void ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY);

	HICON	m_HIconSelectedDefect;

	void DecideInspectResult(int iInspectionType);
	// L10
	int m_iInspectResult;
	int m_iDefect[MAX_DEFECT_NAME];

// Construction
public:
	////////////////////////////////////////////////////////////////////////////////////////////
	//// Override
	//// MzNo, TrayNo, ModuleNo
	//virtual void SetMzNo(int value) override { m_pMzNo = value; }
	//virtual int GetMzNo() const override { return m_pMzNo; }

	//virtual void SetTrayNo(int value) override { m_pTrayNo = value; }
	//virtual int GetTrayNo() const override { return m_pTrayNo; }

	//virtual void SetModuleNo(int value) override { m_pModuleNo = value; }
	//virtual int GetModuleNo() const override { return m_pModuleNo; }

	//// Barcode Region
	//virtual HObject& GetBarcodeDetectRgn() override { return m_HBarcodeDetectRgn; }
	//virtual void SetBarcodeDetectRgn(const HObject& rgn) override { m_HBarcodeDetectRgn = rgn; }

	//// Barcode Result
	//virtual BOOL GetBarcodeRegResult() const override { return m_bBarcodeRegResult; }
	//virtual void SetBarcodeRegResult(BOOL value) override { m_bBarcodeRegResult = value; }

	//virtual BOOL GetBarcodeShiftNG() const override { return m_bBarcodeShiftNG; }
	//virtual void SetBarcodeShiftNG(BOOL value) override { m_bBarcodeShiftNG = value; }

	//virtual BOOL GetBarcodeRotationNG() const override { return m_bBarcodeRotationNG; }
	//virtual void SetBarcodeRotationNG(BOOL value) override { m_bBarcodeRotationNG = value; }

	//virtual CString GetBarcodeResult() const override { return m_sBarcodeResult; }
	//virtual void SetBarcodeResult(const CString& value) override { m_sBarcodeResult = value; }

	//virtual double GetBarcodePosOffsetX() const override { return m_dBarcodePosOffsetX; }
	//virtual void SetBarcodePosOffsetX(double value) override { m_dBarcodePosOffsetX = value; }

	//virtual double GetBarcodePosOffsetY() const override { return m_dBarcodePosOffsetY; }
	//virtual void SetBarcodePosOffsetY(double value) override { m_dBarcodePosOffsetY = value; }

	//virtual int GetBarcodeRotationResult() const override { return m_iBarcodeRotationResult; }
	//virtual void SetBarcodeRotationResult(int value) override { m_iBarcodeRotationResult = value; }

	//// Defect Sorting Number
	//virtual int GetDefectSortingNumber(int index) const override
	//{
	//	if (index >= 0 && index < MAX_DEFECT_NAME)
	//		return iDefectSortingNumber[index];
	//	return -1;
	//}
	//virtual void SetDefectSortingNumber(int index, int value) override
	//{
	//	if (index >= 0 && index < MAX_DEFECT_NAME)
	//		iDefectSortingNumber[index] = value;
	//}

	//// SpecApplyDefectRgn
	//virtual HObject& GetSpecApplyDefectRgn(int index) override
	//{
	//	return m_HSpecApplyDefectRgn[index];
	//}
	//virtual void SetSpecApplyDefectRgn(int index, const HObject& rgn) override
	//{
	//	if (index >= 0 && index < MAX_DEFECT_NAME)
	//		m_HSpecApplyDefectRgn[index] = rgn;
	//}

	//// RosReviewSpecApplyDefectRgn
	//virtual HObject& GetRosReviewSpecApplyDefectRgn(int inspectionType, int defectName) override
	//{
	//	return m_HRosReviewSpecApplyDefectRgn[inspectionType][defectName];
	//}
	//virtual void SetRosReviewSpecApplyDefectRgn(int inspectionType, int defectName, const HObject& rgn) override
	//{
	//	if (inspectionType >= 0 && inspectionType < MAX_INSPECTION_TYPE &&
	//		defectName >= 0 && defectName < MAX_DEFECT_NAME)
	//		m_HRosReviewSpecApplyDefectRgn[inspectionType][defectName] = rgn;
	//}

	//////////////////////////////////////////////////////////////////////////////////////////
	CInspectAdminViewHideDlg1(CWnd* pParent = NULL);   // standard constructor

	void DrawFrameActiveTRegion();

	void set_display_font (HalconCpp::HTuple WindowHandle, HalconCpp::HTuple Size, HalconCpp::HTuple Font, HalconCpp::HTuple Bold, HalconCpp::HTuple Slant);

	int WriteResultFile(int iInspectionType, int iPcVisionNo, int iVisionCamType, int iResultImageNo, HObject HDisplayImage, HObject HInspectionTypeAlgoDetectionDefectRgn, int *piDefectCodeIndex, CString sBarcodeName);

	int WriteSelectedRosReviewFile(int iPcVisionNo, int iVisionCamType, int iNoInspectionImage, HObject *pHDisplayImage, HObject *pHDefectRgn_ImageNo,CString sBarcodeID, int *piNoSaveReviewImage);

	int WriteSelectedRosReviewFile_FAI(int iMzNo, int iTrayNo, int iModuleNo, int iPcVisionNo, int iVisionCamType, int iNoInspectionImage, HObject *pHDisplayImage, HObject(*ppHReviewRgn_FAI_Item)[10], POINT(*ppiViewportCenter_FAI_Item)[10], CString sBarcodeID, int *piNoSaveReviewImage);

	BOOL ImageCheck( CString sImagePath);

	void Show();
	void Reset();
	void ClearAll();
	
	void LoadScanImage(int nScanNum);
	void MoveToDefectArea();

	void RedrawView();
	void RedrawRect();

	BOOL m_bOnPaintNow;

	//int m_pMzNo;
	//int m_pModuleNo;
	//int	m_pTrayNo;

	HObject m_HResultDefectRgn;
	BOOL m_bLive;

	POINT CropLTPoint,CropRBPoint;
	DPOINT m_dLTPoint, m_dRBPoint;

	// L10
	//int iDefectSortingNumber[MAX_DEFECT_NAME];
	//HObject m_HSpecApplyDefectRgn[MAX_DEFECT_NAME];

	//HObject m_HRosReviewSpecApplyDefectRgn[MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];

	//////////////////////////////////////////////////////////////////////////
	// Barcode
	//BOOL m_bBarcodeRegResult;
	//BOOL m_bBarcodeShiftNG;
	//BOOL m_bBarcodeRotationNG;
	//CString m_sBarcodeResult;
	//double m_dBarcodePosOffsetX;
	//double m_dBarcodePosOffsetY;
	//int m_iBarcodeRotationResult;
	//HObject m_HBarcodeDetectRgn;
	//////////////////////////////////////////////////////////////////////////

	HTuple HideWindowHandle;
	//HTuple HideTextWindowHandle;

	HTuple m_HGapMeasureResult;
	HObject m_HGapMeasureLineXLD;

	double m_dDistanceMiniConformalHeight;
	double m_dCCMH_CenterX;
	double m_dCCMH_CenterY;
	HObject m_HMiniConformalHeightEndLineXLD;

	double m_dDistanceConformalHeight;
	double m_dCCLH_CenterX;
	double m_dCCLH_CenterY;
	HObject m_HConformalHeightEndLineXLD;

	// Dialog Data
	//{{AFX_DATA(CInspectAdminViewHideDlg1)
	enum { IDD = IDD_INSPECT_ADMIN_VIEW_HIDE_DLG1 };
	CScrollBar	m_VScrBar;
	CScrollBar	m_HScrBar;
	//}}AFX_DATA

	CString strLog;
};
#endif