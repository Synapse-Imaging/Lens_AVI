#pragma once


// CTrayImagePB

class CTrayImagePB : public CStatic
{
	DECLARE_DYNAMIC(CTrayImagePB)

public:
	CTrayImagePB();
	virtual ~CTrayImagePB();

	void Reset();

	int OkNg[MAX_MODULE_TRAY];
	BOOL ClickRegion[MAX_MODULE_TRAY];
	BOOL WrongPointClick;
	CRect SetPictureBoxSize(CRect PbRect);

	HObject RegionErosion;
	HTuple WindowHandle;
	HTuple  Row, Column, Button,Index;

	int m_iVisionCamType;
	int m_iVisionIdx;

	void ClearLastSelectedModule();
	void ReDraw();

	BOOL bOnDrawing;

protected:
	int m_iLastClickRegionIdx;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();

	LRESULT OnDispTray(WPARAM wParam,LPARAM lParam);

	CString strLog;
};


