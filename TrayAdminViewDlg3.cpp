// TrayAdminViewDlg3.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TrayAdminViewDlg3.h"
#include "afxdialogex.h"


// CTrayAdminViewDlg3 대화 상자입니다.

CTrayAdminViewDlg3* CTrayAdminViewDlg3::m_pInstance = NULL;

CTrayAdminViewDlg3* CTrayAdminViewDlg3::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance)
	{
		m_pInstance = new CTrayAdminViewDlg3();
		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_TRAYVIEW_ADMIN_DLG3, pFrame->GetActiveView());
			if (bShowFlag)
				m_pInstance->Show();
		}
	}
	return m_pInstance;
}
void CTrayAdminViewDlg3::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CTrayAdminViewDlg3::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);

	this->GetClientRect(&DiplayRect);
	DiplayRect.NormalizeRect();
	this->ClientToScreen(&DiplayRect);	//picture control size
	ScreenToClient(&DiplayRect);
	DiplayRect = m_TrayImagePB.SetPictureBoxSize(DiplayRect);

	MakeTrayRectangle();

	ShowWindow(SW_SHOW);

	m_TrayImagePB.ReDraw();
}


IMPLEMENT_DYNAMIC(CTrayAdminViewDlg3, CDialog)

CTrayAdminViewDlg3::CTrayAdminViewDlg3(CWnd* pParent /*=NULL*/)
	: CDialog(CTrayAdminViewDlg3::IDD, pParent)
{
	if (THEAPP.m_ModelDefineInfo.m_iUseVisionNo == 0)
	{
		SetPosition(2 + (970 / VISION_NUMBER_MAX) * THEAPP.m_ModelDefineInfo.m_iVisionTrayDlgIndex, 160, 970 / VISION_NUMBER_MAX, 910);
		THEAPP.m_ModelDefineInfo.m_iVisionTrayDlgIndex++;
	}
	else
	{
		SetPosition(2 + (970 / THEAPP.m_ModelDefineInfo.m_iUseVisionNo) * THEAPP.m_ModelDefineInfo.m_iVisionTrayDlgIndex, 160, 970 / THEAPP.m_ModelDefineInfo.m_iUseVisionNo, 910);

		if (THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_3] != "Undefine")
			THEAPP.m_ModelDefineInfo.m_iVisionTrayDlgIndex++;
	}
}

CTrayAdminViewDlg3::~CTrayAdminViewDlg3()
{

}

void CTrayAdminViewDlg3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISPLAY_VIEW, m_TrayImagePB);

}


BEGIN_MESSAGE_MAP(CTrayAdminViewDlg3, CDialog)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CTrayAdminViewDlg3 메시지 처리기입니다.


BOOL CTrayAdminViewDlg3::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_TrayImagePB.m_iVisionIdx = VISION_NUMBER_3;
	m_TrayImagePB.m_iVisionCamType = VISION_NUMBER_3;

	if (THEAPP.Struct_PreferenceStruct.m_bUseVisionCopy)
		SetWindowText("Vision Copy #3");
	else
	{
		CString sVisionCamType;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_3];
		SetWindowText(sVisionCamType);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTrayAdminViewDlg3::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	//	lpwndpos->flags |= SWP_NOMOVE; 
	CDialog::OnWindowPosChanging(lpwndpos);
}

void CTrayAdminViewDlg3::UpdateTrayDisplay()
{
	InvalidateRect(false);
}

void CTrayAdminViewDlg3::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
}


BOOL CTrayAdminViewDlg3::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch (pMsg->message)
	{
	case WM_NCLBUTTONDOWN:

	case WM_NCRBUTTONDOWN:
		SetActiveWindow();
		return TRUE;

	case WM_KEYDOWN:
		if ((int)pMsg->wParam == VK_ESCAPE || (int)pMsg->wParam == VK_RETURN)
			return TRUE;

	case WM_SYSKEYDOWN:
		if (pMsg->wParam == VK_F4)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CTrayAdminViewDlg3::MakeTrayRectangle()
{
	try
	{
		if (m_TrayImagePB.WindowHandle > -1)
		{
			return;
			HalconCpp::CloseWindow(m_TrayImagePB.WindowHandle);
			m_TrayImagePB.WindowHandle = -1;
		}

		double width, height;
		width = 70 * THEAPP.m_pModelDataManager->m_iTrayArrayX;
		height = 70 * THEAPP.m_pModelDataManager->m_iTrayArrayY;

		OpenWindow(0, 0, DiplayRect.Width(), DiplayRect.Height(), (long)GetDlgItem(IDC_STATIC_DISPLAY_VIEW)->m_hWnd, "visible", "", &DispayViewWindowHandle);

		GenImageConst(&Image, "byte", width, height);
		PartitionRectangle(Image, &Partitioned, width / THEAPP.m_pModelDataManager->m_iTrayArrayX, height / THEAPP.m_pModelDataManager->m_iTrayArrayY);
		ErosionRectangle1(Partitioned, &RegionErosion, 20, 20);

		SetPart(DispayViewWindowHandle, 0, 0, height, width);

		DispObj(RegionErosion, DispayViewWindowHandle);

		m_TrayImagePB.RegionErosion = RegionErosion;
		m_TrayImagePB.WindowHandle = DispayViewWindowHandle;
	}
	catch (HException& except)
	{
		strLog.Format("Halcon Exception [CTrayAdminViewDlg3::MakeTrayRectangle] : %s", (CString)(LPCTSTR)except.ErrorMessage());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}
