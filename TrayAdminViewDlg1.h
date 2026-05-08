#pragma once

#include "TrayAdminViewDlg.h"
#include "TrayImagePB.h"

// CTrayAdminViewDlg1 대화 상자입니다.

class CTrayAdminViewDlg1 : public CDialog, public ITrayAdminViewDlg
{
	DECLARE_DYNAMIC(CTrayAdminViewDlg1)

public:
	CTrayAdminViewDlg1(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTrayAdminViewDlg1();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_TRAYVIEW_ADMIN_DLG1 };
	static CTrayAdminViewDlg1* m_pInstance;

public:
	static	CTrayAdminViewDlg1* GetInstance(BOOL bShowFlag = FALSE);
	void	DeleteInstance();
	void Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
public:
	HTuple DispayViewWindowHandle;

	CRect DiplayRect;
	CTrayImagePB m_TrayImagePB;
	HObject Image, Partitioned, RegionErosion;
	HTuple Width, Height;
	void UpdateTrayDisplay();
	void MakeTrayRectangle();

	virtual CTrayImagePB& GetTrayImagePB() override { return m_TrayImagePB; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CString strLog;
};
