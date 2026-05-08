// InspectViewDlg1.cpp : implementation file
//
#include "stdafx.h"
#include "uscan.h"
#include "InspectAdminViewDlg.h"
#include "InspectionTypeExDlg.h"
#include "InspectionTypeChsDlg.h"
#include "InspectionTypeChsWZDlg.h"
#ifdef SNZEROMQ
#include <snzeromq.client/Helpers/ConvertHelper.h>
#include <snzeromq.client/Helpers/ContourProcessHelper.h>
#endif // SNZEROMQ

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	struct InspectionTypeResult
	{
		int inspectionType = 0;
		int faiRoiNumber = 0;
		bool genLineRegion = false;
		int positionId = -1;
		int pointId = -1;
		bool hasPositionPoint = false;
	};

	bool ShowInspectionTypeDialog(InspectionTypeResult& out, bool fromPolygon, CWnd* parent)
	{
		CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

		if (strModelType == _T("CHS-W") || strModelType == _T("CHS-Z"))
		{
			CInspectionTypeChsWZDlg dlg(parent);
			dlg.m_bFromPolygon = fromPolygon;
			if (dlg.DoModal() == IDCANCEL)
				return false;

			out.inspectionType = dlg.m_iRadioInspectionType;
			out.faiRoiNumber = dlg.m_iRadioFaiRoiNumber;
			out.genLineRegion = (dlg.m_bGenLineRegion == TRUE);
			out.hasPositionPoint = false;
			return true;
		}
		else if (strModelType == _T("CHS-K") || strModelType == _T("CHS-S") || strModelType == _T("CHS-T") || strModelType == _T("CHS-V"))
		{
			CInspectionTypeChsDlg dlg(parent);
			dlg.m_bFromPolygon = fromPolygon;
			if (dlg.DoModal() == IDCANCEL)
				return false;

			out.inspectionType = dlg.m_iRadioInspectionType;
			out.faiRoiNumber = dlg.m_iRadioFaiRoiNumber;
			out.genLineRegion = (dlg.m_bGenLineRegion == TRUE);
			out.hasPositionPoint = false;
			return true;
		}
		else
		{
			CInspectionTypeExDlg dlg(parent);
			dlg.m_bFromPolygon = fromPolygon;
			if (dlg.DoModal() == IDCANCEL)
				return false;

			out.inspectionType = dlg.m_iRadioInspectionType;
			out.faiRoiNumber = dlg.m_iRadioFaiRoiNumber;
			out.genLineRegion = (dlg.m_bGenLineRegion == TRUE);
			out.positionId = dlg.m_iPositionID;
			out.pointId = dlg.m_iPointID;
			out.hasPositionPoint = true;
			return true;
		}
	}

	bool IsChsWzModel()
	{
		CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
		return strModelType == _T("CHS-W") || strModelType == _T("CHS-Z");
	}

	bool IsChsKstvModel()
	{
		CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
		return strModelType == _T("CHS-K") || strModelType == _T("CHS-S") || strModelType == _T("CHS-T") || strModelType == _T("CHS-V");
	}

	bool IsNonChsModel()
	{
		return !IsChsWzModel() && !IsChsKstvModel();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInspectAdminViewDlg dialog

CInspectAdminViewDlg* CInspectAdminViewDlg::m_pInstance = NULL;

CInspectAdminViewDlg* CInspectAdminViewDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectAdminViewDlg();
		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_ADMIN_VIEW_DLG, pFrame->GetActiveView());
			if (bShowFlag)
				m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectAdminViewDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectAdminViewDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

CInspectAdminViewDlg::CInspectAdminViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectAdminViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInspectAdminViewDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP + VIEW_TOOLBAR_HEIGHT, VIEW1_DLG1_WIDTH + 280, VIEW1_DLG1_HEIGHT + 370 - VIEW_TOOLBAR_HEIGHT);

	m_lWindowID = -1;
	m_iToolBarOffset = 0;
	m_iScrBarWidth = 16;

	m_bOnPaintNow = FALSE;
	StartTime1 = 0;
	EndTime1 = 0;
	m_bOnOff = FALSE;
	m_bLive = FALSE;
	m_bDispCrossBar = FALSE;
	m_bTestThread = FALSE;

	m_bDisplayFitLine = TRUE;
	m_bDisplayInspectArea = TRUE;
	m_bDisplayThresholdArea = TRUE;
	m_bDisplayDefectArea = TRUE;
	m_bROILock = TRUE;
	m_bAlignDisplay = TRUE;

	m_bDispROIGv = FALSE;
	m_bDispFAIMeasureValue = FALSE;

	ClearAll();
}

void CInspectAdminViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInspectAdminViewDlg)
	DDX_Control(pDX, IDC_VSCRBAR, m_VScrBar);
	DDX_Control(pDX, IDC_HSCRBAR, m_HScrBar);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInspectAdminViewDlg, CDialog)
	//{{AFX_MSG_MAP(CInspectAdminViewDlg)
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
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_VISION_1, OnMenuAllRoiCopyVision1)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_VISION_2, OnMenuAllRoiCopyVision2)
	ON_COMMAND(ID_MENU_ROI_COPY_VISION_1, OnMenuRoiCopyVision1)
	ON_COMMAND(ID_MENU_ROI_COPY_VISION_2, OnMenuRoiCopyVision2)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_0, OnMenuRoiCopyImage0)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_1, OnMenuRoiCopyImage1)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_2, OnMenuRoiCopyImage2)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_3, OnMenuRoiCopyImage3)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_4, OnMenuRoiCopyImage4)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_5, OnMenuRoiCopyImage5)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_6, OnMenuRoiCopyImage6)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_7, OnMenuRoiCopyImage7)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_8, OnMenuRoiCopyImage8)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_9, OnMenuRoiCopyImage9)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_10, OnMenuRoiCopyImage10)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_11, OnMenuRoiCopyImage11)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_12, OnMenuRoiCopyImage12)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_13, OnMenuRoiCopyImage13)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_14, OnMenuRoiCopyImage14)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_15, OnMenuRoiCopyImage15)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_16, OnMenuRoiCopyImage16)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_17, OnMenuRoiCopyImage17)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_18, OnMenuRoiCopyImage18)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_19, OnMenuRoiCopyImage19)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_20, OnMenuRoiCopyImage20)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_21, OnMenuRoiCopyImage21)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_22, OnMenuRoiCopyImage22)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_23, OnMenuRoiCopyImage23)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_24, OnMenuRoiCopyImage24)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_25, OnMenuRoiCopyImage25)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_26, OnMenuRoiCopyImage26)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_27, OnMenuRoiCopyImage27)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_28, OnMenuRoiCopyImage28)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_29, OnMenuRoiCopyImage29)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_30, OnMenuRoiCopyImage30)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_31, OnMenuRoiCopyImage31)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_32, OnMenuRoiCopyImage32)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_33, OnMenuRoiCopyImage33)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_34, OnMenuRoiCopyImage34)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_35, OnMenuRoiCopyImage35)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_36, OnMenuRoiCopyImage36)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_37, OnMenuRoiCopyImage37)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_38, OnMenuRoiCopyImage38)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_39, OnMenuRoiCopyImage39)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_40, OnMenuRoiCopyImage40)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_41, OnMenuRoiCopyImage41)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_42, OnMenuRoiCopyImage42)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_43, OnMenuRoiCopyImage43)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_44, OnMenuRoiCopyImage44)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_45, OnMenuRoiCopyImage45)

	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_0, OnMenuAllRoiCopyImage0)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_1, OnMenuAllRoiCopyImage1)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_2, OnMenuAllRoiCopyImage2)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_3, OnMenuAllRoiCopyImage3)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_4, OnMenuAllRoiCopyImage4)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_5, OnMenuAllRoiCopyImage5)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_6, OnMenuAllRoiCopyImage6)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_7, OnMenuAllRoiCopyImage7)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_8, OnMenuAllRoiCopyImage8)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_9, OnMenuAllRoiCopyImage9)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_10, OnMenuAllRoiCopyImage10)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_11, OnMenuAllRoiCopyImage11)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_12, OnMenuAllRoiCopyImage12)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_13, OnMenuAllRoiCopyImage13)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_14, OnMenuAllRoiCopyImage14)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_15, OnMenuAllRoiCopyImage15)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_16, OnMenuAllRoiCopyImage16)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_17, OnMenuAllRoiCopyImage17)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_18, OnMenuAllRoiCopyImage18)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_19, OnMenuAllRoiCopyImage19)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_20, OnMenuAllRoiCopyImage20)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_21, OnMenuAllRoiCopyImage21)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_22, OnMenuAllRoiCopyImage22)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_23, OnMenuAllRoiCopyImage23)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_24, OnMenuAllRoiCopyImage24)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_25, OnMenuAllRoiCopyImage25)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_26, OnMenuAllRoiCopyImage26)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_27, OnMenuAllRoiCopyImage27)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_28, OnMenuAllRoiCopyImage28)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_29, OnMenuAllRoiCopyImage29)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_30, OnMenuAllRoiCopyImage30)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_31, OnMenuAllRoiCopyImage31)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_32, OnMenuAllRoiCopyImage32)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_33, OnMenuAllRoiCopyImage33)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_34, OnMenuAllRoiCopyImage34)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_35, OnMenuAllRoiCopyImage35)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_36, OnMenuAllRoiCopyImage36)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_37, OnMenuAllRoiCopyImage37)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_38, OnMenuAllRoiCopyImage38)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_39, OnMenuAllRoiCopyImage39)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_40, OnMenuAllRoiCopyImage40)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_41, OnMenuAllRoiCopyImage41)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_42, OnMenuAllRoiCopyImage42)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_43, OnMenuAllRoiCopyImage43)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_44, OnMenuAllRoiCopyImage44)
	ON_COMMAND(ID_MENU_ALL_ROI_COPY_IMAGE_45, OnMenuAllRoiCopyImage45)

	ON_COMMAND(ID_MENU_ROI_GROUPING, OnMenuRoiGrouping)
	ON_COMMAND(ID_MENU_ROI_TYPE_CHANGE, OnMenuRoiChange)
	ON_COMMAND(ID_MENU_ROI_ELLIPSE, OnMenuRoiElipse)
	ON_COMMAND(ID_MENU_ALL_ROI_MOVE, OnMenuAllRoiMove)
	ON_COMMAND(ID_MENU_ALL_ROI_SCALE, OnMenuAllRoiScale)
	ON_COMMAND(ID_MENU_ROI_SIZE_CHANGE_MM, OnMenuROISizeChangeMM)
	ON_COMMAND(ID_MENU_ROI_SIZE_CHANGE_PIXEL, OnMenuROISizeChangePIXEL)
	ON_COMMAND(ID_MENU_ROI_MOVE_CHANGE_MM, OnMenuROIMoveChangeMM)
	ON_COMMAND(ID_MENU_ROI_MOVE_CHANGE_PIXEL, OnMenuROIMoveChangePIXEL)
	ON_COMMAND(ID_MENU_ROI_GV_DISPLAY, OnMenuROIGvDisplay)
	ON_COMMAND(ID_MENU_ROI_ID_CHANGE, OnMenuROIIDChange)
	ON_COMMAND(ID_MENU_LOAD_IMAGE, OnMenuLoadImage)
	ON_COMMAND(ID_MENU_SAVE_IMAGE, OnMenuSaveAsTeachingImage)
	ON_COMMAND(ID_MENU_DELETE_ROI_TAB, OnMenuDeleteRoiTab)
	ON_COMMAND(ID_MENU_DELETE_ROI_ALL, OnMenuDeleteRoiAll)
	ON_COMMAND(ID_MENU_LINK_INFO, OnMenuLinkInfo)
	ON_COMMAND(ID_MENU_ROI_ROTATE, OnMenuRoiRotate)
	ON_COMMAND(ID_MENU_FAI_MEASURE_DISPLAY, OnMenuFAIMeasureDisplay)
	ON_COMMAND(ID_MENU_INSPECT_LIBRARY, OnMenuInspectLibrary)
	ON_COMMAND(ID_MENU_GRAB_SEQUENCE_VIEW, OnMenuGrabSequenceView)
	ON_COMMAND(ID_MENU_ROI_FLIP, OnMenuRoiFlip)
	ON_COMMAND(ID_MENU_AI_RETRAIN_ROI, OnMenuAiRetrainRoi)
	ON_COMMAND(ID_MENU_VM_AUTO_TRAIN_CHECK_ALL, OnMenuVmAutoTrainCheckAll)
	ON_COMMAND(ID_MENU_VM_AUTO_TRAIN_UNCHECK_ALL, OnMenuVmAutoTrainUncheckAll)
	ON_COMMAND(ID_MENU_VM_AUTO_TRAIN_STATUS, OnMenuVmAutoTrainStatus)
	ON_COMMAND(ID_MENU_VM_ALL_MODEL_DELETE, OnMenuVmAllModelDelete)

	ON_WM_CREATE()
	//}}AFX_MSG_MAP

	ON_MESSAGE(UM_2DFRAME_READY, On2DFrameReady)
	ON_MESSAGE(UM_RS_GOTO_DEFECT, OnEventGoToDefect)
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectAdminViewDlg message handlers

void CInspectAdminViewDlg::ClearAll()
{
	m_iRoiCopyImageIndex = -1;

	m_dZoomRatio = 0.8;

	m_iImageWidth = 1000;
	m_iImageHeight = 1000;

	mpActiveTRegion = NULL;
	mpSelectPartTRegion = NULL;
	mpLastSelectedRegion = NULL;

	m_ToolBarState = TS_SELECT_PART;
	m_bOnMoving = FALSE;
	m_bDisplayImage = FALSE;

	for (int k = 0; k < CHANNEL_NUM; k++)
	{
		for (int kk = 0; kk < MAX_IMAGE_TAB; kk++)
			GenEmptyObj(&(m_HTeachingImage[k][kk]));
	}

	GenEmptyObj(&m_HCrossBarXLD);
	GenEmptyObj(&m_HMultiModuleCropRgn);
	GenEmptyObj(&m_HAutoFocusCheckRgn);
	GenEmptyObj(&m_HShapeModelContrastRgn);
	GenEmptyObj(&m_pThresRgn);
	GenEmptyObj(&m_HInspectAreaRgn);

	for (int i = 0; i < CHANNEL_NUM; i++)
		GenEmptyObj(&(m_pHLiveImage[i]));

	m_caliper_control_flag = 0;
	m_iNoCaliperPoint = 0;
	mpLineSP = CPoint(0, 0);
	mpLineEP = CPoint(0, 0);

	m_bPolygonMode = FALSE;
	m_iPolygonCnt = 0;

	GenEmptyObj(&m_HOCRDetectRgn);

	m_iDisplayChannelType = CHANNEL_TYPE_COLOR;
}

BOOL CInspectAdminViewDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_NCLBUTTONDOWN:
		SetActiveWindow();
		return TRUE;
	case WM_KEYDOWN:
	{
		if ((pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			|| (pMsg->wParam == VK_F4))
			return TRUE;

		if ((pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN) == TRUE
			&& (mpActiveTRegion && m_bROILock == FALSE) == FALSE)
			return FALSE;

		if (GetKeyState(VK_CONTROL) < 0)
		{
			if (mpActiveTRegion && m_bROILock == FALSE)
			{
				switch (pMsg->wParam)
				{
				case VK_UP:
					ScaleActiveRegion(ROI_KEY_UP, TRUE);
					UpdateView(FALSE);
					return TRUE;
				case VK_DOWN:
					ScaleActiveRegion(ROI_KEY_DOWN, TRUE);
					UpdateView(FALSE);
					return TRUE;
				case VK_LEFT:
					ScaleActiveRegion(ROI_KEY_LEFT, TRUE);
					UpdateView(FALSE);
					return TRUE;
				case VK_RIGHT:
					ScaleActiveRegion(ROI_KEY_RIGHT, TRUE);
					UpdateView(FALSE);
					return TRUE;
				}
			}
		}

		if (GetKeyState(VK_SHIFT) < 0)
		{
			if (mpActiveTRegion && m_bROILock == FALSE)
			{
				switch (pMsg->wParam)
				{
				case VK_UP:
					ScaleActiveRegion(ROI_KEY_UP, FALSE);
					UpdateView(FALSE);
					return TRUE;
				case VK_DOWN:
					ScaleActiveRegion(ROI_KEY_DOWN, FALSE);
					UpdateView(FALSE);
					return TRUE;
				case VK_LEFT:
					ScaleActiveRegion(ROI_KEY_LEFT, FALSE);
					UpdateView(FALSE);
					return TRUE;
				case VK_RIGHT:
					ScaleActiveRegion(ROI_KEY_RIGHT, FALSE);
					UpdateView(FALSE);
					return TRUE;
				}
			}
		}

		if (mpActiveTRegion && m_bROILock == FALSE)
		{
			switch (pMsg->wParam)
			{
			case VK_UP:
				MoveActiveRegion(ROI_KEY_UP);
				UpdateView(FALSE);
				return TRUE;
			case VK_DOWN:
				MoveActiveRegion(ROI_KEY_DOWN);
				UpdateView(FALSE);
				return TRUE;
			case VK_LEFT:
				MoveActiveRegion(ROI_KEY_LEFT);
				UpdateView(FALSE);
				return TRUE;
			case VK_RIGHT:
				MoveActiveRegion(ROI_KEY_RIGHT);
				UpdateView(FALSE);
				return TRUE;
			}
		}

		break;
	}

	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CInspectAdminViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_caliper_control_flag = 0;
	m_iNoCaliperPoint = 0;
	mpLineSP = CPoint(0, 0);
	mpLineEP = CPoint(0, 0);

	m_bOnOff = FALSE;
	m_bLive = FALSE;
	m_bDispCrossBar = FALSE;

	GetClientRect(&m_ClientRect);
	Reset();
	m_bDisplayImage = TRUE;

	m_Menu.LoadMenu(IDR_MENU_INSPECT_ADMIN_VIEW);

	GenCrossContourXld(&m_HCrossBarXLD, THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[0] / 2, THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[VISION_NUMBER_1] / 2, (double)THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[0] * 2.0, 0.0);

	ChangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInspectAdminViewDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMenu* pPopup = m_Menu.GetSubMenu(0);

	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		strDialog.Format("원본 영상 - (0, 0) = [0, 0, 0], 배율(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(strDialog);

		pPopup->ModifyMenu(ID_MENU_INSPECT_LIBRARY, MF_BYCOMMAND | MF_STRING, ID_MENU_INSPECT_LIBRARY, _T("검사 라이브러리 설정"));
		pPopup->ModifyMenu(ID_MENU_GRAB_SEQUENCE_VIEW, MF_BYCOMMAND | MF_STRING, ID_MENU_GRAB_SEQUENCE_VIEW, _T("그랩 시퀀스 주소 설정"));

		pPopup->ModifyMenu(3, MF_BYPOSITION | MF_STRING, 3, _T("선택된 ROI 복사(영상탭)"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_0, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_0, _T("기본 정보"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_1, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_1, _T("영상 1"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_2, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_2, _T("영상 2"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_3, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_3, _T("영상 3"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_4, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_4, _T("영상 4"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_5, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_5, _T("영상 5"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_6, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_6, _T("영상 6"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_7, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_7, _T("영상 7"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_8, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_8, _T("영상 8"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_9, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_9, _T("영상 9"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_10, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_10, _T("영상 10"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_11, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_11, _T("영상 11"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_12, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_12, _T("영상 12"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_13, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_13, _T("영상 13"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_14, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_14, _T("영상 14"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_15, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_15, _T("영상 15"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_16, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_16, _T("영상 16"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_17, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_17, _T("영상 17"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_18, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_18, _T("영상 18"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_19, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_19, _T("영상 19"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_20, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_20, _T("영상 20"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_21, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_21, _T("영상 21"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_22, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_22, _T("영상 22"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_23, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_23, _T("영상 23"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_24, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_24, _T("영상 24"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_25, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_25, _T("영상 25"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_26, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_26, _T("영상 26"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_27, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_27, _T("영상 27"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_28, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_28, _T("영상 28"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_29, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_29, _T("영상 29"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_30, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_30, _T("영상 30"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_31, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_31, _T("영상 31"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_32, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_32, _T("영상 32"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_33, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_33, _T("영상 33"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_34, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_34, _T("영상 34"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_35, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_35, _T("영상 35"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_36, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_36, _T("영상 36"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_37, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_37, _T("영상 37"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_38, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_38, _T("영상 38"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_39, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_39, _T("영상 39"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_40, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_40, _T("영상 40"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_41, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_41, _T("영상 41"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_42, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_42, _T("영상 42"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_43, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_43, _T("영상 43"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_44, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_44, _T("영상 44"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_45, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_45, _T("영상 45"));

		pPopup->ModifyMenu(4, MF_BYPOSITION | MF_STRING, 4, _T("영상탭 전체 ROI 복사(영상탭)"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_0, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_0, _T("기본 정보"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_1, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_1, _T("영상 1"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_2, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_2, _T("영상 2"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_3, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_3, _T("영상 3"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_4, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_4, _T("영상 4"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_5, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_5, _T("영상 5"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_6, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_6, _T("영상 6"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_7, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_7, _T("영상 7"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_8, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_8, _T("영상 8"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_9, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_9, _T("영상 9"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_10, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_10, _T("영상 10"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_11, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_11, _T("영상 11"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_12, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_12, _T("영상 12"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_13, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_13, _T("영상 13"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_14, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_14, _T("영상 14"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_15, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_15, _T("영상 15"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_16, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_16, _T("영상 16"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_17, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_17, _T("영상 17"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_18, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_18, _T("영상 18"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_19, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_19, _T("영상 19"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_20, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_20, _T("영상 20"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_21, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_21, _T("영상 21"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_22, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_22, _T("영상 22"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_23, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_23, _T("영상 23"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_24, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_24, _T("영상 24"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_25, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_25, _T("영상 25"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_26, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_26, _T("영상 26"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_27, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_27, _T("영상 27"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_28, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_28, _T("영상 28"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_29, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_29, _T("영상 29"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_30, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_30, _T("영상 30"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_31, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_31, _T("영상 31"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_32, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_32, _T("영상 32"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_33, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_33, _T("영상 33"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_34, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_34, _T("영상 34"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_35, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_35, _T("영상 35"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_36, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_36, _T("영상 36"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_37, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_37, _T("영상 37"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_38, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_38, _T("영상 38"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_39, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_39, _T("영상 39"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_40, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_40, _T("영상 40"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_41, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_41, _T("영상 41"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_42, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_42, _T("영상 42"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_43, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_43, _T("영상 43"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_44, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_44, _T("영상 44"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_45, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_45, _T("영상 45"));

		m_Menu.ModifyMenu(ID_MENU_ROI_GROUPING, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_GROUPING, _T("ROI 합치기"));
		m_Menu.ModifyMenu(ID_MENU_ROI_TYPE_CHANGE, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_TYPE_CHANGE, _T("ROI 용도 변경"));

		pPopup->ModifyMenu(10, MF_BYPOSITION | MF_STRING, 10, _T("ROI 크기/위치 변경"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_MOVE, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_MOVE, _T("모든 ROI 위치 이동"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_SCALE, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_SCALE, _T("모든 다각형 ROI 스케일 변경"));
		m_Menu.ModifyMenu(ID_MENU_ROI_SIZE_CHANGE_MM, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_SIZE_CHANGE_MM, _T("ROI 크기 변경 (mm)"));
		m_Menu.ModifyMenu(ID_MENU_ROI_SIZE_CHANGE_PIXEL, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_SIZE_CHANGE_PIXEL, _T("ROI 크기 변경 (픽셀)"));
		m_Menu.ModifyMenu(ID_MENU_ROI_MOVE_CHANGE_MM, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_MOVE_CHANGE_MM, _T("ROI 위치 이동 (mm)"));
		m_Menu.ModifyMenu(ID_MENU_ROI_MOVE_CHANGE_PIXEL, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_MOVE_CHANGE_PIXEL, _T("ROI 위치 이동 (픽셀)"));

		m_Menu.ModifyMenu(ID_MENU_ROI_GV_DISPLAY, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_GV_DISPLAY, _T("ROI 평균 밝기 디스플레이"));
		m_Menu.ModifyMenu(ID_MENU_FAI_MEASURE_DISPLAY, MF_BYCOMMAND | MF_STRING, ID_MENU_FAI_MEASURE_DISPLAY, _T("FAI 측정값 디스플레이"));
		m_Menu.ModifyMenu(ID_MENU_ROI_ID_CHANGE, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_ID_CHANGE, _T("ROI ID 번호 변경 (최소: 1)"));

		pPopup->ModifyMenu(17, MF_BYPOSITION | MF_STRING, 17, _T("ROI 삭제"));
		m_Menu.ModifyMenu(ID_MENU_DELETE_ROI_TAB, MF_BYCOMMAND | MF_STRING, ID_MENU_DELETE_ROI_TAB, _T("현재 영상탭 모든 ROI 삭제"));
		m_Menu.ModifyMenu(ID_MENU_DELETE_ROI_ALL, MF_BYCOMMAND | MF_STRING, ID_MENU_DELETE_ROI_ALL, _T("현재 비전 모든 ROI 삭제"));

		m_Menu.ModifyMenu(ID_MENU_LOAD_IMAGE, MF_BYCOMMAND | MF_STRING, ID_MENU_LOAD_IMAGE, _T("티칭 영상 불러오기"));
		m_Menu.ModifyMenu(ID_MENU_SAVE_IMAGE, MF_BYCOMMAND | MF_STRING, ID_MENU_SAVE_IMAGE, _T("티칭 영상 저장하기"));
		m_Menu.ModifyMenu(ID_MENU_LINK_INFO, MF_BYCOMMAND | MF_STRING, ID_MENU_LINK_INFO, _T("연결된 ROI 정보 확인"));
		m_Menu.ModifyMenu(ID_MENU_ROI_ELLIPSE, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_ELLIPSE, _T("ROI 타원화"));
		m_Menu.ModifyMenu(ID_MENU_ROI_ROTATE, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_ROTATE, _T("ROI 회전"));
		m_Menu.ModifyMenu(ID_MENU_ROI_FLIP, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_FLIP, _T("ROI 반전"));

		pPopup->ModifyMenu(28, MF_BYPOSITION | MF_STRING, 28, _T("선택한 ROI 복사(비전)"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_VISION_1, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_VISION_1, _T("비전 1"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_VISION_2, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_VISION_2, _T("비전 2"));

		pPopup->ModifyMenu(29, MF_BYPOSITION | MF_STRING, 29, _T("영상탭 전체 ROI 복사(비전)"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_VISION_1, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_VISION_1, _T("비전 1"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_VISION_2, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_VISION_2, _T("비전 2"));

		m_Menu.ModifyMenu(ID_MENU_AI_RETRAIN_ROI, MF_BYCOMMAND | MF_STRING, ID_MENU_AI_RETRAIN_ROI, _T("AI 재학습 ROI 지정/해제"));

		pPopup->ModifyMenu(33, MF_BYPOSITION | MF_STRING, 33, _T("영상비교 자동학습 설정"));
		m_Menu.ModifyMenu(ID_MENU_VM_AUTO_TRAIN_CHECK_ALL, MF_BYCOMMAND | MF_STRING, ID_MENU_VM_AUTO_TRAIN_CHECK_ALL, _T("자동학습 옵션 전체 체크"));
		m_Menu.ModifyMenu(ID_MENU_VM_AUTO_TRAIN_UNCHECK_ALL, MF_BYCOMMAND | MF_STRING, ID_MENU_VM_AUTO_TRAIN_UNCHECK_ALL, _T("자동학습 옵션 전체 해제"));
		m_Menu.ModifyMenu(ID_MENU_VM_AUTO_TRAIN_STATUS, MF_BYCOMMAND | MF_STRING, ID_MENU_VM_AUTO_TRAIN_STATUS, _T("이미지별 자동학습 옵션 설정 확인"));
		m_Menu.ModifyMenu(ID_MENU_VM_ALL_MODEL_DELETE, MF_BYCOMMAND | MF_STRING, ID_MENU_VM_ALL_MODEL_DELETE, _T("[주의!!!] 전체 영상비교 모델 삭제"));
	}
	else
	{
		strDialog.Format("Raw image - (0, 0) = [0, 0, 0], Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(strDialog);

		pPopup->ModifyMenu(ID_MENU_INSPECT_LIBRARY, MF_BYCOMMAND | MF_STRING, ID_MENU_INSPECT_LIBRARY, _T("Set the inspection library"));
		pPopup->ModifyMenu(ID_MENU_GRAB_SEQUENCE_VIEW, MF_BYCOMMAND | MF_STRING, ID_MENU_GRAB_SEQUENCE_VIEW, _T("Set the grab sequence"));

		pPopup->ModifyMenu(3, MF_BYPOSITION | MF_STRING, 3, _T("Selected ROI copy (Image tab)"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_0, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_0, _T("Information"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_1, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_1, _T("Image 1"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_2, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_2, _T("Image 2"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_3, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_3, _T("Image 3"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_4, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_4, _T("Image 4"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_5, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_5, _T("Image 5"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_6, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_6, _T("Image 6"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_7, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_7, _T("Image 7"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_8, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_8, _T("Image 8"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_9, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_9, _T("Image 9"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_10, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_10, _T("Image 10"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_11, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_11, _T("Image 11"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_12, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_12, _T("Image 12"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_13, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_13, _T("Image 13"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_14, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_14, _T("Image 14"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_15, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_15, _T("Image 15"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_16, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_16, _T("Image 16"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_17, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_17, _T("Image 17"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_18, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_18, _T("Image 18"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_19, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_19, _T("Image 19"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_20, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_20, _T("Image 20"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_21, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_21, _T("Image 21"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_22, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_22, _T("Image 22"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_23, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_23, _T("Image 23"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_24, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_24, _T("Image 24"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_25, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_25, _T("Image 25"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_26, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_26, _T("Image 26"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_27, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_27, _T("Image 27"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_28, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_28, _T("Image 28"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_29, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_29, _T("Image 29"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_30, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_30, _T("Image 30"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_31, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_31, _T("Image 31"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_32, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_32, _T("Image 32"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_33, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_33, _T("Image 33"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_34, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_34, _T("Image 34"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_35, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_35, _T("Image 35"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_36, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_36, _T("Image 36"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_37, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_37, _T("Image 37"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_38, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_38, _T("Image 38"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_39, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_39, _T("Image 39"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_40, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_40, _T("Image 40"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_41, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_41, _T("Image 41"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_42, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_42, _T("Image 42"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_43, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_43, _T("Image 43"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_44, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_44, _T("Image 44"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_IMAGE_45, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_IMAGE_45, _T("Image 45"));

		pPopup->ModifyMenu(4, MF_BYPOSITION | MF_STRING, 4, _T("All ROI copy (Image tab)"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_0, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_0, _T("Information"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_1, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_1, _T("Image 1"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_2, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_2, _T("Image 2"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_3, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_3, _T("Image 3"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_4, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_4, _T("Image 4"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_5, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_5, _T("Image 5"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_6, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_6, _T("Image 6"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_7, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_7, _T("Image 7"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_8, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_8, _T("Image 8"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_9, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_9, _T("Image 9"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_10, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_10, _T("Image 10"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_11, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_11, _T("Image 11"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_12, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_12, _T("Image 12"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_13, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_13, _T("Image 13"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_14, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_14, _T("Image 14"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_15, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_15, _T("Image 15"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_16, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_16, _T("Image 16"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_17, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_17, _T("Image 17"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_18, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_18, _T("Image 18"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_19, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_19, _T("Image 19"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_20, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_20, _T("Image 20"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_21, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_21, _T("Image 21"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_22, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_22, _T("Image 22"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_23, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_23, _T("Image 23"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_24, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_24, _T("Image 24"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_25, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_25, _T("Image 25"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_26, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_26, _T("Image 26"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_27, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_27, _T("Image 27"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_28, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_28, _T("Image 28"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_29, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_29, _T("Image 29"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_30, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_30, _T("Image 30"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_31, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_31, _T("Image 31"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_32, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_32, _T("Image 32"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_33, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_33, _T("Image 33"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_34, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_34, _T("Image 34"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_35, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_35, _T("Image 35"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_36, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_36, _T("Image 36"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_37, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_37, _T("Image 37"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_38, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_38, _T("Image 38"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_39, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_39, _T("Image 39"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_40, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_40, _T("Image 40"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_41, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_41, _T("Image 41"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_42, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_42, _T("Image 42"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_43, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_43, _T("Image 43"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_44, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_44, _T("Image 44"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_IMAGE_45, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_IMAGE_45, _T("Image 45"));

		m_Menu.ModifyMenu(ID_MENU_ROI_GROUPING, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_GROUPING, _T("ROI grouping"));
		m_Menu.ModifyMenu(ID_MENU_ROI_TYPE_CHANGE, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_TYPE_CHANGE, _T("Change ROI type"));

		pPopup->ModifyMenu(10, MF_BYPOSITION | MF_STRING, 10, _T("ROI resize / Movement"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_MOVE, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_MOVE, _T("Move All ROIs"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_SCALE, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_SCALE, _T("Change scale of all polygon ROIs"));
		m_Menu.ModifyMenu(ID_MENU_ROI_SIZE_CHANGE_MM, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_SIZE_CHANGE_MM, _T("Resize ROI (mm)"));
		m_Menu.ModifyMenu(ID_MENU_ROI_SIZE_CHANGE_PIXEL, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_SIZE_CHANGE_PIXEL, _T("Resize ROI (pixel)"));
		m_Menu.ModifyMenu(ID_MENU_ROI_MOVE_CHANGE_MM, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_MOVE_CHANGE_MM, _T("Move ROI (mm)"));
		m_Menu.ModifyMenu(ID_MENU_ROI_MOVE_CHANGE_PIXEL, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_MOVE_CHANGE_PIXEL, _T("Move ROI (pixel)"));

		m_Menu.ModifyMenu(ID_MENU_ROI_GV_DISPLAY, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_GV_DISPLAY, _T("Display ROI average GV"));
		m_Menu.ModifyMenu(ID_MENU_FAI_MEASURE_DISPLAY, MF_BYCOMMAND | MF_STRING, ID_MENU_FAI_MEASURE_DISPLAY, _T("Display FAI measure"));
		m_Menu.ModifyMenu(ID_MENU_ROI_ID_CHANGE, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_ID_CHANGE, _T("Change ROI ID (minimum 1)"));

		pPopup->ModifyMenu(17, MF_BYPOSITION | MF_STRING, 17, _T("Delete ROI"));
		m_Menu.ModifyMenu(ID_MENU_DELETE_ROI_TAB, MF_BYCOMMAND | MF_STRING, ID_MENU_DELETE_ROI_TAB, _T("Delete all ROIs in current image tab"));
		m_Menu.ModifyMenu(ID_MENU_DELETE_ROI_ALL, MF_BYCOMMAND | MF_STRING, ID_MENU_DELETE_ROI_ALL, _T("Delete all ROIs in current vision"));

		m_Menu.ModifyMenu(ID_MENU_LOAD_IMAGE, MF_BYCOMMAND | MF_STRING, ID_MENU_LOAD_IMAGE, _T("Load teach image"));
		m_Menu.ModifyMenu(ID_MENU_SAVE_IMAGE, MF_BYCOMMAND | MF_STRING, ID_MENU_SAVE_IMAGE, _T("Save teach image"));
		m_Menu.ModifyMenu(ID_MENU_LINK_INFO, MF_BYCOMMAND | MF_STRING, ID_MENU_LINK_INFO, _T("Check linked ROI info"));
		m_Menu.ModifyMenu(ID_MENU_ROI_ELLIPSE, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_ELLIPSE, _T("Transform ROI to ellipse"));
		m_Menu.ModifyMenu(ID_MENU_ROI_ROTATE, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_ROTATE, _T("Rotate ROI"));
		m_Menu.ModifyMenu(ID_MENU_ROI_FLIP, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_FLIP, _T("Flip ROI"));

		pPopup->ModifyMenu(28, MF_BYPOSITION | MF_STRING, 28, _T("Selected ROI copy (Vision)"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_VISION_1, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_VISION_1, _T("Vision 1"));
		m_Menu.ModifyMenu(ID_MENU_ROI_COPY_VISION_2, MF_BYCOMMAND | MF_STRING, ID_MENU_ROI_COPY_VISION_2, _T("Vision 2"));

		pPopup->ModifyMenu(29, MF_BYPOSITION | MF_STRING, 29, _T("All ROI copy (Vision)"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_VISION_1, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_VISION_1, _T("Vision 1"));
		m_Menu.ModifyMenu(ID_MENU_ALL_ROI_COPY_VISION_2, MF_BYCOMMAND | MF_STRING, ID_MENU_ALL_ROI_COPY_VISION_2, _T("Vision 2"));

		m_Menu.ModifyMenu(ID_MENU_AI_RETRAIN_ROI, MF_BYCOMMAND | MF_STRING, ID_MENU_AI_RETRAIN_ROI, _T("AI retrain ROI on/off"));

		pPopup->ModifyMenu(33, MF_BYPOSITION | MF_STRING, 33, _T("Image compare auto train setting"));
		m_Menu.ModifyMenu(ID_MENU_VM_AUTO_TRAIN_CHECK_ALL, MF_BYCOMMAND | MF_STRING, ID_MENU_VM_AUTO_TRAIN_CHECK_ALL, _T("Check all auto train options"));
		m_Menu.ModifyMenu(ID_MENU_VM_AUTO_TRAIN_UNCHECK_ALL, MF_BYCOMMAND | MF_STRING, ID_MENU_VM_AUTO_TRAIN_UNCHECK_ALL, _T("Uncheck all auto train options"));
		m_Menu.ModifyMenu(ID_MENU_VM_AUTO_TRAIN_STATUS, MF_BYCOMMAND | MF_STRING, ID_MENU_VM_AUTO_TRAIN_STATUS, _T("Display auto train option status for each image"));
		m_Menu.ModifyMenu(ID_MENU_VM_ALL_MODEL_DELETE, MF_BYCOMMAND | MF_STRING, ID_MENU_VM_ALL_MODEL_DELETE, _T("[Caution!!!] Delete all image compare models"));
	}
}


void CInspectAdminViewDlg::AnalyzeAndDisplayDefects(HObject defectRegions, HTuple windowID) const
{
	CString strArea;
	HObject HDefectConnRgn, HSelectedRgn;
	Hlong lNoDefects = 0;
	Hlong lArea;
	double dCompArea;
	double dCenterX, dCenterY;
	HTuple HdCenterX, HdCenterY, HlArea, HdCircleRadius;
	POINT CenterPoint;
	HTuple HlRow1, HlCol1, HlRow2, HlCol2, HdBlobLength;
	double dBlobDiameter;
	double dBlobDiameterX = 0.0, dBlobDiameterY = 0.0;

	Connection(defectRegions, &HDefectConnRgn);
	HTuple HlNoDefects;
	CountObj(HDefectConnRgn, &HlNoDefects);
	lNoDefects = HlNoDefects.L();

	for (int iii = 0; iii < lNoDefects; iii++)
	{
		SelectObj(HDefectConnRgn, &HSelectedRgn, iii + 1);

		if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
			continue;

		AreaCenter(HSelectedRgn, &HlArea, &HdCenterY, &HdCenterX);
		lArea = HlArea.L();
		dCenterX = HdCenterX.D();
		dCenterY = HdCenterY.D();

		dCompArea = ceil(((double)lArea * THEAPP.m_pCalDataService->GetPixelArea()) / 1000) / 1000;

		CenterPoint.x = (long)dCenterX;
		CenterPoint.y = (long)dCenterY;

		DiameterRegion(HSelectedRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2, &HdBlobLength);
		dBlobDiameterX = HdBlobLength.D();

		InnerCircle(HSelectedRgn, &HdCenterY, &HdCenterX, &HdCircleRadius);
		dBlobDiameterY = HdCircleRadius.D();
		dBlobDiameterY = dBlobDiameterY * 2.0;

		// hsv Display
		HObject HColorImageReduced, HColorImageR, HColorImageG, HColorImageB;
		HTuple HdRedVal, HdGreeen, HdBlue, HdSd;
		double dRedVal, dGreenVal, dBlueVal, dMax, dMin, dDiff;
		double dHue = 0.0;
		double dSaturation = 0.0;
		ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_R][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HSelectedRgn, &HColorImageReduced);
		Intensity(HSelectedRgn, HColorImageReduced, &HdRedVal, &HdSd);
		GenEmptyObj(&HColorImageReduced);
		ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_G][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HSelectedRgn, &HColorImageReduced);
		Intensity(HSelectedRgn, HColorImageReduced, &HdGreeen, &HdSd);
		GenEmptyObj(&HColorImageReduced);
		ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_B][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HSelectedRgn, &HColorImageReduced);
		Intensity(HSelectedRgn, HColorImageReduced, &HdBlue, &HdSd);
		GenEmptyObj(&HColorImageReduced);

		dRedVal = HdRedVal.D() / 255;
		dGreenVal = HdGreeen.D() / 255;
		dBlueVal = HdBlue.D() / 255;

		dMax = MAX(dRedVal, dGreenVal);
		dMax = MAX(dMax, dBlueVal);
		dMin = MIN(dRedVal, dGreenVal);
		dMin = MIN(dMin, dBlueVal);

		dDiff = dMax - dMin;

		if (dMax > 1e-12)
			dSaturation = (dDiff / dMax) * 100.0;

		if (fabs(dDiff) > 1e-12)
		{
			if (dRedVal >= dGreenVal && dRedVal >= dBlueVal)
				dHue = fmod(((dGreenVal - dBlueVal) / dDiff), 6.0) * 60.0;
			else if (dGreenVal >= dRedVal && dGreenVal >= dBlueVal)
				dHue = (((dBlueVal - dRedVal) / dDiff) + 2.0) * 60.0;
			else
				dHue = (((dRedVal - dGreenVal) / dDiff) + 4.0) * 60.0;

			if (dHue < 0.0)
				dHue += 360.0;
		}

		strArea.Format("%.03lf mm2(%d,%d,%d Pxl)/(H:%.01lf Deg),(S:%.01lf %%)", dCompArea, lArea, (int)(dBlobDiameterX + 0.5), (int)(dBlobDiameterY + 0.5), dHue, dSaturation);

		SetTposition(windowID, CenterPoint.y, CenterPoint.x);
		WriteString(windowID, (HTuple)strArea);
	}
}


void CInspectAdminViewDlg::AnalyzeAndDisplayAutoFocusCenter(HObject defectRegions, HTuple windowID) const
{
	CString strArea;
	HObject HDefectConnRgn, HSelectedRgn;
	Hlong lNoDefects = 0;
	HTuple HdCenterX, HdCenterY, HlArea;
	double dCenterX, dCenterY;
	POINT CenterPoint;

	Connection(defectRegions, &HDefectConnRgn);
	HTuple HlNoDefects;
	CountObj(HDefectConnRgn, &HlNoDefects);
	lNoDefects = HlNoDefects.L();

	for (int iii = 0; iii < lNoDefects; iii++)
	{
		SelectObj(HDefectConnRgn, &HSelectedRgn, iii + 1);

		if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
			continue;

		AreaCenter(HSelectedRgn, &HlArea, &HdCenterY, &HdCenterX);
		dCenterX = HdCenterX.D();
		dCenterY = HdCenterY.D();
		CenterPoint.x = (long)dCenterX;
		CenterPoint.y = (long)dCenterY;

		strArea.Format("%.0f, %.0F", dCenterX, dCenterY);

		SetTposition(windowID, CenterPoint.y + 5, CenterPoint.x + 5);
		WriteString(windowID, (HTuple)strArea);
	}
}


BOOL CInspectAdminViewDlg::OnEraseBkgnd(CDC* pDC)
{
	pDC->FillSolidRect(m_ClientRect, TS_COLOR_EXTRA_BG);
	return TRUE;
}

void CInspectAdminViewDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (m_lWindowID < 0 || !m_bDisplayImage || m_bOnPaintNow)
		return;

	m_bOnPaintNow = TRUE;

	CDC* pDC = GetDC();

	POINT ClientOffset;
	ClientOffset.x = 0;
	ClientOffset.y = m_iToolBarOffset;
	pDC->SetViewportOrg(ClientOffset);

	dc.OffsetViewportOrg(0, m_iToolBarOffset);

	HTuple lDC = (INT)(pDC->m_hDC);
	SetWindowDc(m_lWindowID, lDC);

	if (m_bLive)
	{
		if (THEAPP.m_pGFunction->ValidHImage(m_pHLiveImage[m_iDisplayChannelType]))
			DispObj(m_pHLiveImage[m_iDisplayChannelType], m_lWindowID);
	}

	CRgn ClipRgn;
	ClipRgn.CreateRectRgn(m_ClientRect.left, m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
	pDC->SelectClipRgn(&ClipRgn);

	if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
	{
		if (m_bLive == FALSE)
		{
			if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)	// Align Tab
			{
				if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[m_iDisplayChannelType][0]))
					DispObj(m_HTeachingImage[m_iDisplayChannelType][0], m_lWindowID);
			}
			else
			{
				if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]))
					DispObj(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], m_lWindowID);
			}
		}

		// Thresholding Area Display
		if (m_bDisplayThresholdArea)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(m_pThresRgn))
			{
				SetColor(m_lWindowID, "red");
				SetDraw(m_lWindowID, "fill");
				DispObj(m_pThresRgn, m_lWindowID);
			}
		}

		// Inspection Area Display
		if (m_bDisplayInspectArea)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(m_HInspectAreaRgn))
			{
				SetColor(m_lWindowID, "cyan");
				SetDraw(m_lWindowID, "margin");
				DispObj(m_HInspectAreaRgn, m_lWindowID);
			}
		}

		if (m_bDisplayFitLine)
		{
			if (m_bLive == FALSE)
			{
				THEAPP.m_pModelDataManager->m_pInspectionArea->Draw(m_lWindowID, &m_ViewportManager, GTR_DS_NORMAL, FALSE, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);

				// Region ROI
				GTRegion* pRegion;
				for (int k = 0; k < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
				{
					pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
					if (pRegion == NULL)
						continue;

					if (pRegion->GetVisible())
					{
						if (pRegion->m_bRegionROI)
						{
							if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == THEAPP.m_pTabControlDlg->m_iCurrentTab)
							{
								SetDraw(m_lWindowID, "margin");

								if ((pRegion->miInspectionType > INSPECTION_TYPE_UNDEFINED && pRegion->miInspectionType < INSPECTION_TYPE_DELETE) ||
									pRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN || pRegion->miInspectionType == INSPECTION_TYPE_DONTCARE ||
									pRegion->miInspectionType == INSPECTION_TYPE_GENERATE || pRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE ||
									pRegion->miInspectionType == INSPECTION_TYPE_MASK ||
									(pRegion->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && pRegion->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) ||
									(pRegion->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && pRegion->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST))		// Inspection ROI
								{
									if (pRegion->mbLastSelected)
										SetColor(m_lWindowID, "red");
									else
									{
										if (pRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
											SetColor(m_lWindowID, "magenta");
										else if (pRegion->miInspectionType == INSPECTION_TYPE_DONTCARE)
											SetRgb(m_lWindowID, 128, 0, 255);
										else if (pRegion->miInspectionType == INSPECTION_TYPE_GENERATE)
											SetRgb(m_lWindowID, 255, 0, 128);
										else if (pRegion->miInspectionType == INSPECTION_TYPE_MASK)
											SetRgb(m_lWindowID, 0, 0, 255);
										else if (pRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE)
											SetColor(m_lWindowID, "cyan");
										else if (pRegion->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && pRegion->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST)
											SetRgb(m_lWindowID, 255, 128, 0);
										else if (pRegion->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && pRegion->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE)
											SetColor(m_lWindowID, "cyan");
										else
											SetColor(m_lWindowID, "green");
									}
								}
								else		// Teaching ROI
								{
									if (pRegion->miInspectionType == INSPECTION_TYPE_DELETE)
										SetColor(m_lWindowID, "blue");
									else if (pRegion->miInspectionType == INSPECTION_TYPE_GLOBAL_ALIGN)
										SetColor(m_lWindowID, "cyan");
									else if (pRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
										SetColor(m_lWindowID, "magenta");
									else if (pRegion->miInspectionType == INSPECTION_TYPE_DONTCARE)
										SetRgb(m_lWindowID, 128, 0, 255);
									else if (pRegion->miInspectionType == INSPECTION_TYPE_GENERATE)
										SetRgb(m_lWindowID, 255, 0, 128);
									else if (pRegion->miInspectionType == INSPECTION_TYPE_MASK)
										SetRgb(m_lWindowID, 0, 0, 255);
									else if (pRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE)
										SetColor(m_lWindowID, "cyan");
									else if (pRegion->miInspectionType == INSPECTION_TYPE_ADD)
										SetColor(m_lWindowID, "orange");
									else if (pRegion->miInspectionType == INSPECTION_TYPE_AUTO_UNDEFINED)
										SetRgb(m_lWindowID, 255, 255, 0);
									else
										SetColor(m_lWindowID, "green");
								}

								if (pRegion == mpActiveTRegion)
								{
									SetLineWidth(m_lWindowID, 2);
									SetLineStyle(m_lWindowID, (HTuple(20).Append(10)));
								}

								if (THEAPP.m_pGFunction->ValidHRegion(pRegion->m_HTeachPolygonRgn))
								{
									DispObj(pRegion->m_HTeachPolygonRgn, m_lWindowID);
								}

								if (pRegion == mpActiveTRegion)
								{
									SetLineWidth(m_lWindowID, 1);
									SetLineStyle(m_lWindowID, HTuple());
								}
							}

							if (pRegion->miInspectionType == INSPECTION_TYPE_LIGHT || pRegion->miInspectionType == INSPECTION_TYPE_FOCUS)
							{
								if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == THEAPP.m_pTabControlDlg->m_iCurrentTab)
								{
									DPOINT dCrossLTPoint, dCrossRBPoint, dCrossCenterPoint;
									POINT CrossWHPoint;
									pRegion->GetLTPointD(&dCrossLTPoint, THEAPP.m_pCalDataService);
									pRegion->GetRBPointD(&dCrossRBPoint, THEAPP.m_pCalDataService);
									pRegion->GetCenterPointD(&dCrossCenterPoint, THEAPP.m_pCalDataService);
									pRegion->GetWHPoint(&CrossWHPoint, THEAPP.m_pCalDataService);

									HalconCpp::SetFont(m_lWindowID, "Arial-Normal-16");

									double dAvgValue, dSd, dMeanAmp, dSdAmp;
									HObject HCrossROIHRegion, HCrossImageReduced, HEdgeAmp;
									HCrossROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);

									if (THEAPP.m_pGFunction->ValidHRegion(HCrossROIHRegion))
									{
										dAvgValue = -100;

										if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)	// Align Tab
										{
											if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[CHANNEL_TYPE_I][0]))
											{
												HTuple HdAvgValue, HdSd, HdMeanAmp, HdSdAmp;
												Intensity(HCrossROIHRegion, m_HTeachingImage[CHANNEL_TYPE_I][0], &HdAvgValue, &HdSd);
												dAvgValue = HdAvgValue.D();
												dSd = HdSd.D();

												ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_I][0], HCrossROIHRegion, &HCrossImageReduced);
												SobelAmp(HCrossImageReduced, &HEdgeAmp, "sum_abs", 3);
												Intensity(HCrossROIHRegion, HEdgeAmp, &HdMeanAmp, &HdSdAmp);
												dMeanAmp = HdMeanAmp.D();
												dSdAmp = HdSdAmp.D();
											}
										}
										else
										{
											if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]))
											{
												HTuple HdAvgValue, HdSd, HdMeanAmp, HdSdAmp;
												Intensity(HCrossROIHRegion, m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HdAvgValue, &HdSd);
												dAvgValue = HdAvgValue.D();
												dSd = HdSd.D();

												ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HCrossROIHRegion, &HCrossImageReduced);
												SobelAmp(HCrossImageReduced, &HEdgeAmp, "sum_abs", 3);
												Intensity(HCrossROIHRegion, HEdgeAmp, &HdMeanAmp, &HdSdAmp);
												dMeanAmp = HdMeanAmp.D();
												dSdAmp = HdSdAmp.D();
											}
										}

										if (dAvgValue >= 0)
										{
											CString strCrossInfo;
											POINT CrossStrPos;

											if (pRegion->miInspectionType == INSPECTION_TYPE_LIGHT)
												strCrossInfo.Format("GV:%d", (int)(dAvgValue + 0.5));
											else
												strCrossInfo.Format("Edge:%.1lf", dMeanAmp);

											CrossStrPos.x = (int)dCrossLTPoint.x;
											CrossStrPos.y = (int)dCrossLTPoint.y;

											SetRgb(m_lWindowID, 0, 255, 0);
											SetTposition(m_lWindowID, CrossStrPos.y - 35, CrossStrPos.x);
											WriteString(m_lWindowID, (HTuple)strCrossInfo);
										}
									}
								}
							}

						}	// if (pRegion->m_bRegionROI)
						else if (pRegion->miInspectionType == INSPECTION_TYPE_CROSS_BAR)
						{
							if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == THEAPP.m_pTabControlDlg->m_iCurrentTab)
							{
								DPOINT dCrossLTPoint, dCrossRBPoint, dCrossCenterPoint;
								POINT CrossWHPoint;
								pRegion->GetLTPointD(&dCrossLTPoint, THEAPP.m_pCalDataService);
								pRegion->GetRBPointD(&dCrossRBPoint, THEAPP.m_pCalDataService);
								pRegion->GetCenterPointD(&dCrossCenterPoint, THEAPP.m_pCalDataService);
								pRegion->GetWHPoint(&CrossWHPoint, THEAPP.m_pCalDataService);

								SetColor(m_lWindowID, "red");

								DispLine(m_lWindowID, dCrossCenterPoint.y, dCrossLTPoint.x, dCrossCenterPoint.y, dCrossRBPoint.x);
								DispLine(m_lWindowID, dCrossLTPoint.y, dCrossCenterPoint.x, dCrossRBPoint.y, dCrossCenterPoint.x);

								HalconCpp::SetFont(m_lWindowID, "Arial-Normal-16");

								double dUmPerPixel = THEAPP.m_pCalDataService->GetPixelSize();
								int iUmLength;

								SetRgb(m_lWindowID, 255, 0, 0);

								CString strCrossInfo;
								POINT CrossStrPos;

								iUmLength = (int)((double)CrossWHPoint.x * dUmPerPixel + 0.5);
								if (THEAPP.m_iPGMLanguageSelect == 0)
									strCrossInfo.Format("%d(픽셀) / %d(um)", CrossWHPoint.x, iUmLength);
								else
									strCrossInfo.Format("%d(pixel) / %d(um)", CrossWHPoint.x, iUmLength);
								CrossStrPos.x = (int)dCrossRBPoint.x;
								CrossStrPos.y = (int)dCrossCenterPoint.y;
								SetTposition(m_lWindowID, CrossStrPos.y - 10, CrossStrPos.x + 15);
								WriteString(m_lWindowID, (HTuple)strCrossInfo);

								iUmLength = (int)((double)CrossWHPoint.y * dUmPerPixel + 0.5);
								if (THEAPP.m_iPGMLanguageSelect == 0)
									strCrossInfo.Format("%d(픽셀) / %d(um)", CrossWHPoint.y, iUmLength);
								else
									strCrossInfo.Format("%d(pixel) / %d(um)", CrossWHPoint.y, iUmLength);
								CrossStrPos.x = (int)dCrossCenterPoint.x;
								CrossStrPos.y = (int)dCrossRBPoint.y;
								SetTposition(m_lWindowID, CrossStrPos.y + 10, CrossStrPos.x);
								WriteString(m_lWindowID, (HTuple)strCrossInfo);

								if (THEAPP.m_iPGMLanguageSelect == 0)
									strCrossInfo.Format("중심(%d,%d)", (int)dCrossCenterPoint.x, (int)dCrossCenterPoint.y);
								else
									strCrossInfo.Format("Center(%d,%d)", (int)dCrossCenterPoint.x, (int)dCrossCenterPoint.y);
								CrossStrPos.x = (int)dCrossCenterPoint.x;
								CrossStrPos.y = (int)dCrossCenterPoint.y;
								SetTposition(m_lWindowID, CrossStrPos.y - 40, CrossStrPos.x - 60);
								WriteString(m_lWindowID, (HTuple)strCrossInfo);

								double dAvgValue, dAvgValueR, dAvgValueG, dAvgValueB, dSd, dMeanAmp, dSdAmp;
								HObject HCrossROIHRegion, HCrossImageReduced, HEdgeAmp;
								HCrossROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);

								if (THEAPP.m_pGFunction->ValidHRegion(HCrossROIHRegion))
								{
									if (m_iDisplayChannelType == CHANNEL_TYPE_COLOR)
									{
										dAvgValueR = dAvgValueG = dAvgValueB = -100;

										if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)	// Align Tab
										{
											if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[CHANNEL_TYPE_COLOR][0]))
											{
												HTuple HdAvgValue, HdSd, HdMeanAmp, HdSdAmp;
												Intensity(HCrossROIHRegion, m_HTeachingImage[CHANNEL_TYPE_R][0], &HdAvgValue, &HdSd);
												dAvgValueR = HdAvgValue.D();
												Intensity(HCrossROIHRegion, m_HTeachingImage[CHANNEL_TYPE_G][0], &HdAvgValue, &HdSd);
												dAvgValueG = HdAvgValue.D();
												Intensity(HCrossROIHRegion, m_HTeachingImage[CHANNEL_TYPE_B][0], &HdAvgValue, &HdSd);
												dAvgValueB = HdAvgValue.D();

												ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_I][0], HCrossROIHRegion, &HCrossImageReduced);
												SobelAmp(HCrossImageReduced, &HEdgeAmp, "sum_abs", 3);
												Intensity(HCrossROIHRegion, HEdgeAmp, &HdMeanAmp, &HdSdAmp);
												dMeanAmp = HdMeanAmp.D();
												dSdAmp = HdSdAmp.D();
											}
										}
										else
										{
											if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]))
											{
												HTuple HdAvgValue, HdSd, HdMeanAmp, HdSdAmp;
												Intensity(HCrossROIHRegion, m_HTeachingImage[CHANNEL_TYPE_R][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HdAvgValue, &HdSd);
												dAvgValueR = HdAvgValue.D();
												Intensity(HCrossROIHRegion, m_HTeachingImage[CHANNEL_TYPE_G][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HdAvgValue, &HdSd);
												dAvgValueG = HdAvgValue.D();
												Intensity(HCrossROIHRegion, m_HTeachingImage[CHANNEL_TYPE_B][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HdAvgValue, &HdSd);
												dAvgValueB = HdAvgValue.D();

												ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HCrossROIHRegion, &HCrossImageReduced);
												SobelAmp(HCrossImageReduced, &HEdgeAmp, "sum_abs", 3);
												Intensity(HCrossROIHRegion, HEdgeAmp, &HdMeanAmp, &HdSdAmp);
												dMeanAmp = HdMeanAmp.D();
												dSdAmp = HdSdAmp.D();
											}
										}

										if (dAvgValueR >= 0 && dAvgValueG >= 0 && dAvgValueB >= 0)
										{
											strCrossInfo.Format("GV(RGB):%d %d %d, Edge:%.1lf", (int)(dAvgValueR + 0.5), (int)(dAvgValueG + 0.5), (int)(dAvgValueB + 0.5), dMeanAmp);
											CrossStrPos.x = (int)dCrossLTPoint.x;
											CrossStrPos.y = (int)dCrossLTPoint.y;

											SetRgb(m_lWindowID, 0, 255, 0);
											SetTposition(m_lWindowID, CrossStrPos.y - 30, CrossStrPos.x);
											WriteString(m_lWindowID, (HTuple)strCrossInfo);
										}
									}
									else
									{
										dAvgValue = -100;

										if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)	// Align Tab
										{
											if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[m_iDisplayChannelType][0]))
											{
												HTuple HdAvgValue, HdSd, HdMeanAmp, HdSdAmp;
												Intensity(HCrossROIHRegion, m_HTeachingImage[m_iDisplayChannelType][0], &HdAvgValue, &HdSd);
												dAvgValue = HdAvgValue.D();
												dSd = HdSd.D();

												ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_I][0], HCrossROIHRegion, &HCrossImageReduced);
												SobelAmp(HCrossImageReduced, &HEdgeAmp, "sum_abs", 3);
												Intensity(HCrossROIHRegion, HEdgeAmp, &HdMeanAmp, &HdSdAmp);
												dMeanAmp = HdMeanAmp.D();
												dSdAmp = HdSdAmp.D();
											}
										}
										else
										{
											if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]))
											{
												HTuple HdAvgValue, HdSd, HdMeanAmp, HdSdAmp;
												Intensity(HCrossROIHRegion, m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HdAvgValue, &HdSd);
												dAvgValue = HdAvgValue.D();
												dSd = HdSd.D();

												ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HCrossROIHRegion, &HCrossImageReduced);
												SobelAmp(HCrossImageReduced, &HEdgeAmp, "sum_abs", 3);
												Intensity(HCrossROIHRegion, HEdgeAmp, &HdMeanAmp, &HdSdAmp);
												dMeanAmp = HdMeanAmp.D();
												dSdAmp = HdSdAmp.D();
											}
										}

										if (dAvgValue >= 0)
										{
											strCrossInfo.Format("GV:%d, Edge:%.1lf", (int)(dAvgValue + 0.5), dMeanAmp);
											CrossStrPos.x = (int)dCrossLTPoint.x;
											CrossStrPos.y = (int)dCrossLTPoint.y;

											SetRgb(m_lWindowID, 0, 255, 0);
											SetTposition(m_lWindowID, CrossStrPos.y - 35, CrossStrPos.x);
											WriteString(m_lWindowID, (HTuple)strCrossInfo);
										}
									}
								}
							}
						}
						else if (pRegion->miInspectionType == INSPECTION_TYPE_LIGHT || pRegion->miInspectionType == INSPECTION_TYPE_FOCUS)
						{
							if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == THEAPP.m_pTabControlDlg->m_iCurrentTab)
							{
								DPOINT dCrossLTPoint, dCrossRBPoint, dCrossCenterPoint;
								POINT CrossWHPoint;
								pRegion->GetLTPointD(&dCrossLTPoint, THEAPP.m_pCalDataService);
								pRegion->GetRBPointD(&dCrossRBPoint, THEAPP.m_pCalDataService);
								pRegion->GetCenterPointD(&dCrossCenterPoint, THEAPP.m_pCalDataService);
								pRegion->GetWHPoint(&CrossWHPoint, THEAPP.m_pCalDataService);

								HalconCpp::SetFont(m_lWindowID, "Arial-Normal-16");

								double dAvgValue, dSd, dMeanAmp, dSdAmp;
								HObject HCrossROIHRegion, HCrossImageReduced, HEdgeAmp;
								HCrossROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);

								if (THEAPP.m_pGFunction->ValidHRegion(HCrossROIHRegion))
								{
									dAvgValue = -100;

									if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)	// Align Tab
									{
										if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[CHANNEL_TYPE_I][0]))
										{
											HTuple HdAvgValue, HdSd, HdMeanAmp, HdSdAmp;
											Intensity(HCrossROIHRegion, m_HTeachingImage[CHANNEL_TYPE_I][0], &HdAvgValue, &HdSd);
											dAvgValue = HdAvgValue.D();
											dSd = HdSd.D();

											ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_I][0], HCrossROIHRegion, &HCrossImageReduced);
											SobelAmp(HCrossImageReduced, &HEdgeAmp, "sum_abs", 3);
											Intensity(HCrossROIHRegion, HEdgeAmp, &HdMeanAmp, &HdSdAmp);
											dMeanAmp = HdMeanAmp.D();
											dSdAmp = HdSdAmp.D();
										}
									}
									else
									{
										if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]))
										{
											HTuple HdAvgValue, HdSd, HdMeanAmp, HdSdAmp;
											Intensity(HCrossROIHRegion, m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HdAvgValue, &HdSd);
											dAvgValue = HdAvgValue.D();
											dSd = HdSd.D();

											ReduceDomain(m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HCrossROIHRegion, &HCrossImageReduced);
											SobelAmp(HCrossImageReduced, &HEdgeAmp, "sum_abs", 3);
											Intensity(HCrossROIHRegion, HEdgeAmp, &HdMeanAmp, &HdSdAmp);
											dMeanAmp = HdMeanAmp.D();
											dSdAmp = HdSdAmp.D();
										}
									}

									if (dAvgValue >= 0)
									{
										CString strCrossInfo;
										POINT CrossStrPos;

										if (pRegion->miInspectionType == INSPECTION_TYPE_LIGHT)
											strCrossInfo.Format("GV:%d", (int)(dAvgValue + 0.5));
										else
											strCrossInfo.Format("Edge:%.1lf", dMeanAmp);

										CrossStrPos.x = (int)dCrossLTPoint.x;
										CrossStrPos.y = (int)dCrossLTPoint.y;

										SetRgb(m_lWindowID, 0, 255, 0);
										SetTposition(m_lWindowID, CrossStrPos.y - 35, CrossStrPos.x);
										WriteString(m_lWindowID, (HTuple)strCrossInfo);
									}
								}
							}
						}

						if (IsNonChsModel() && m_bDispFAIMeasureValue)
						{
							// NOTE : UI 에 FAI 그리는 코드 (FAI 측정값 표시)
							if (pRegion->miInspectionType >= INSPECTION_TYPE_MEASURE_START && pRegion->miInspectionType <= INSPECTION_TYPE_MEASURE_END)
							{
								int iCurTab = THEAPP.m_pTabControlDlg->m_iCurrentTab;

								if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == iCurTab)
								{
									if (pRegion->miInspectionType == INSPECTION_TYPE_MEASURE_POINT)	// 측정 Point
									{
										double dFAIMeasureValueMm = -1;
										int iFAIValueDispMargin = 0;

										// FAI 번호들 가져오기
										int iPositionID = pRegion->miPositionID;
										enMeasurePos enPosThis = ItoM(iPositionID);
										const auto& faiNumbers = CFAIDataManager::GetInstance().GetFAINumbersAtPosition(enPosThis);
										for (int iFAINum : faiNumbers)
										{
											dFAIMeasureValueMm = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[iFAINum][0];

											DPOINT dCenterPoint;
											pRegion->GetLTPointD(&dCenterPoint, THEAPP.m_pCalDataService);

											HalconCpp::SetFont(m_lWindowID, "Arial-Normal-16");

											CString strCrossInfo;
											POINT CrossStrPos;

											// TODO : FAI5-X, FAI5-Y 처럼, FAI가 꼭 단일 숫자는 아닐 수 도 있음.
											// ex1 : FAI5-X, FAI5-Y
											// ex2 : FAI10-1, FAI10-2
											strCrossInfo.Format("FAI#%d: %.3lf(mm)", iFAINum, dFAIMeasureValueMm);

											CrossStrPos.x = (int)dCenterPoint.x;
											CrossStrPos.y = (int)dCenterPoint.y + iFAIValueDispMargin;

											SetRgb(m_lWindowID, 255, 0, 0);
											SetTposition(m_lWindowID, CrossStrPos.y - 10, CrossStrPos.x);
											WriteString(m_lWindowID, (HTuple)strCrossInfo);

											iFAIValueDispMargin += 10;
										}
									}
									else if (pRegion->miInspectionType == INSPECTION_TYPE_BLEND_POINT)	// 합성 Point
									{
										DPOINT dBlendCPoint;
										dBlendCPoint = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dBlendCPoint[THEAPP.m_iCurTeachVision][pRegion->miPositionID][pRegion->miPointID];

										DPOINT dCenterPoint;
										pRegion->GetLTPointD(&dCenterPoint, THEAPP.m_pCalDataService);

										HalconCpp::SetFont(m_lWindowID, "Arial-Normal-16");

										CString strCrossInfo;
										POINT CrossStrPos;

										strCrossInfo.Format("(%.1lf, %.1lf)", dBlendCPoint.x, dBlendCPoint.y);

										CrossStrPos.x = (int)dCenterPoint.x;
										CrossStrPos.y = (int)dCenterPoint.y;

										SetRgb(m_lWindowID, 255, 0, 0);
										SetTposition(m_lWindowID, CrossStrPos.y - 10, CrossStrPos.x);
										WriteString(m_lWindowID, (HTuple)strCrossInfo);
									}
								}
							}
						}

						if (IsChsKstvModel() && m_bDispFAIMeasureValue)
						{
							if (pRegion->miInspectionType >= INSPECTION_TYPE_MEASURE_START && pRegion->miInspectionType <= INSPECTION_TYPE_MEASURE_END)
							{
								double dFAIMeasureValueMm = -1;
								BOOL bAngleMeasure = FALSE;
								int iFAINumber = -1;
								int iCurTab = THEAPP.m_pTabControlDlg->m_iCurrentTab;
								int iMeasureValueIndex = 0;

								CString sFinalFAIName;

								if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == iCurTab)
								{
									switch (pRegion->miInspectionType)
									{
									case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1:
									case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P2:
									case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P3:
									case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P4:
									case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P5:
									case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P6:
									case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P7:
									case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P8:
									{
										if (THEAPP.m_iCurTeachVision == VISION_NUMBER_2)	// Top
										{
											if (pRegion->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1)
											{
												iFAINumber = 5;
												break;
											}
											else
												break;
										}
										else
											break;
									}
									case INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1:
										iFAINumber = 6;
										break;
									case INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P1:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P2:
									{
										iFAINumber = 7;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P2:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P3:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P4:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P5:
									{
										iFAINumber = 9;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1 + 1;
										break;
									}// FAI-11 Skip
									case INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P1:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P2:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P3:
									{
										iFAINumber = 10;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A:
										iFAINumber = 12;
										break;
									case INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A:
										iFAINumber = 13;
										break;
									case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_STEP_P1:
									case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_STEP_P2:
									case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_STEP_P3:
									case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_STEP_P4:
									{
										iFAINumber = 15;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_VCM_SC_STEP_P1;
										break;
									}
									case INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P2:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P3:
									{
										iFAINumber = 18;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_A:
										iFAINumber = 21;
										break;
									case INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_A:
										iFAINumber = 23;
										break;
									case INSPECTION_TYPE_CHS_MEASURE_FAI_24_P1:
										iFAINumber = 24;
										break;
									case INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_25_P2:
									{
										iFAINumber = 25;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P1:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P2:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P3:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P4:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P5:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P6:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P7:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P8:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P9:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P10:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P11:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P12:
									case INSPECTION_TYPE_CHS_MEASURE_DC_1_P13:
									{
										iFAINumber = 84;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_DC_1_P1;
										break;
									}
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P1:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P2:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P3:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P4:
									{
										iFAINumber = 86;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P1:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P2:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P3:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P4:
									{
										iFAINumber = 87;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P1:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P2:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P3:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P4:
									{
										iFAINumber = 88;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_MEASURE_FAI_19_P1:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_19_P2:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_19_P3:
									case INSPECTION_TYPE_CHS_MEASURE_FAI_19_P4:
									{
										iFAINumber = 19;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_19_P1 + 1;
										break;
									}

									default:
										break;
									}

									if (iFAINumber > 0)
									{
										dFAIMeasureValueMm = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[iFAINumber][iMeasureValueIndex];

										DPOINT dCenterPoint;
										pRegion->GetLTPointD(&dCenterPoint, THEAPP.m_pCalDataService);

										HalconCpp::SetFont(m_lWindowID, "Arial-Normal-16");

										CString strCrossInfo;
										POINT CrossStrPos;

										if (bAngleMeasure)
										{
											if (iFAINumber == 85)
												strCrossInfo.Format("D/C_2: %.3lf(Deg)", dFAIMeasureValueMm);
											else
												strCrossInfo.Format("FAI#%d: %.3lf(Deg)", iFAINumber, dFAIMeasureValueMm);
										}
										else
										{
											if (iFAINumber == 84)
												strCrossInfo.Format("D/C_1: %.3lf(mm)", dFAIMeasureValueMm);
											else if (iFAINumber == 86)
												strCrossInfo.Format("FAI16-1: %.3lf(mm)", dFAIMeasureValueMm);
											else if (iFAINumber == 87)
												strCrossInfo.Format("FAI16-2: %.3lf(mm)", dFAIMeasureValueMm);
											else if (iFAINumber == 88)
												strCrossInfo.Format("FAI16-3: %.3lf(mm)", dFAIMeasureValueMm);
											else
												strCrossInfo.Format("FAI#%d: %.3lf(mm)", iFAINumber, dFAIMeasureValueMm);
										}

										CrossStrPos.x = (int)dCenterPoint.x;
										CrossStrPos.y = (int)dCenterPoint.y;

										SetRgb(m_lWindowID, 255, 0, 0);
										SetTposition(m_lWindowID, CrossStrPos.y - 10, CrossStrPos.x);
										WriteString(m_lWindowID, (HTuple)strCrossInfo);

									}
								}
							}
						}

						if (IsChsWzModel() && m_bDispFAIMeasureValue)
						{
							if (pRegion->miInspectionType >= INSPECTION_TYPE_MEASURE_START && pRegion->miInspectionType <= INSPECTION_TYPE_MEASURE_END)
							{
								double dFAIMeasureValueMm = -1;
								BOOL bAngleMeasure = FALSE;
								int iFAINumber = -1;
								int iCurTab = THEAPP.m_pTabControlDlg->m_iCurrentTab;
								int iMeasureValueIndex = 0;

								CString sFinalFAIName;

								if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == iCurTab)
								{
									switch (pRegion->miInspectionType)
									{
									case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P1:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P2:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P3:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P4:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P5:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P6:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P7:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P8:
									{
										if (THEAPP.m_iCurTeachVision == VISION_NUMBER_2)	// Top
										{
											if (pRegion->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P1)
											{
												iFAINumber = 5;
												break;
											}
											else
												break;
										}
										else
											break;
									}
									case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P1:
										iFAINumber = 6;
										break;
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P1:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P2:
									{
										iFAINumber = 3;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P2:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P3:
									{
										iFAINumber = 9;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P1:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P2:
									{
										iFAINumber = 10;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_A:
										iFAINumber = 12;
										break;
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_A:
										iFAINumber = 13;
										break;
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P2:
									{
										iFAINumber = 14;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_B:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_C:
									{
										iFAINumber = 18;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_STEP_P1:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_STEP_P2:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_STEP_P3:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_STEP_P4:
									{
										iFAINumber = 20;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_STEP_P1;
										break;
									}
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P1:
										iFAINumber = 21;
										break;
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P1:
										iFAINumber = 22;
										break;
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P1:
										iFAINumber = 24;
										break;
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P1:
										iFAINumber = 25;
										break;
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P1:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P2:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P3:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P4:
									{
										iFAINumber = 86;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P1:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P2:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P3:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P4:
									{
										iFAINumber = 87;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P1:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P2:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P3:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P4:
									{
										iFAINumber = 88;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P1 + 1;
										break;
									}
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P1:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P2:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P3:
									case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P4:
									{
										iFAINumber = 19;
										iMeasureValueIndex = pRegion->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P1 + 1;
										break;
									}

									default:
										break;
									}

									if (iFAINumber > 0)
									{
										dFAIMeasureValueMm = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[iFAINumber][iMeasureValueIndex];

										DPOINT dCenterPoint;
										pRegion->GetLTPointD(&dCenterPoint, THEAPP.m_pCalDataService);

										HalconCpp::SetFont(m_lWindowID, "Arial-Normal-16");

										CString strCrossInfo;
										POINT CrossStrPos;

										if (bAngleMeasure)
										{
											if (iFAINumber == 84)
												strCrossInfo.Format("D/C_1: %.3lf(mm)", dFAIMeasureValueMm);
											if (iFAINumber == 85)
												strCrossInfo.Format("D/C_2: %.3lf(Deg)", dFAIMeasureValueMm);
											else
												strCrossInfo.Format("FAI#%d: %.3lf(Deg)", iFAINumber, dFAIMeasureValueMm);
										}
										else
										{
											if (iFAINumber == 86)
												strCrossInfo.Format("FAI16-1: %.3lf(mm)", dFAIMeasureValueMm);
											else if (iFAINumber == 87)
												strCrossInfo.Format("FAI16-2: %.3lf(mm)", dFAIMeasureValueMm);
											else if (iFAINumber == 88)
												strCrossInfo.Format("FAI16-3: %.3lf(mm)", dFAIMeasureValueMm);
											else
												strCrossInfo.Format("FAI#%d: %.3lf(mm)", iFAINumber, dFAIMeasureValueMm);
										}

										CrossStrPos.x = (int)dCenterPoint.x;
										CrossStrPos.y = (int)dCenterPoint.y;

										SetRgb(m_lWindowID, 255, 0, 0);
										SetTposition(m_lWindowID, CrossStrPos.y - 10, CrossStrPos.x);
										WriteString(m_lWindowID, (HTuple)strCrossInfo);

									}
								}
							}
						}

						if (m_bDispROIGv)
						{
							if (pRegion->miInspectionType >= INSPECTION_TYPE_FIRST && pRegion->miInspectionType <= INSPECTION_TYPE_LAST)
							{
								if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == THEAPP.m_pTabControlDlg->m_iCurrentTab)
								{
									DPOINT dCrossLTPoint, dCrossRBPoint;
									pRegion->GetLTPointD(&dCrossLTPoint, THEAPP.m_pCalDataService);
									pRegion->GetRBPointD(&dCrossRBPoint, THEAPP.m_pCalDataService);

									HalconCpp::SetFont(m_lWindowID, "Arial-Normal-12");

									CString strCrossInfo;
									POINT CrossStrPos;

									HObject HInspectROIRgn;
									HInspectROIRgn = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);

									double dAvgValueR, dAvgValueG, dAvgValueB, dAvgValueI;
									dAvgValueR = dAvgValueG = dAvgValueB = dAvgValueI = -100;

									HTuple HdAvgValue, HdSd;
									Intensity(HInspectROIRgn, m_HTeachingImage[CHANNEL_TYPE_R][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HdAvgValue, &HdSd);
									dAvgValueR = HdAvgValue.D();
									Intensity(HInspectROIRgn, m_HTeachingImage[CHANNEL_TYPE_G][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HdAvgValue, &HdSd);
									dAvgValueG = HdAvgValue.D();
									Intensity(HInspectROIRgn, m_HTeachingImage[CHANNEL_TYPE_B][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HdAvgValue, &HdSd);
									dAvgValueB = HdAvgValue.D();
									Intensity(HInspectROIRgn, m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HdAvgValue, &HdSd);
									dAvgValueI = HdAvgValue.D();

									if (dAvgValueR >= 0 && dAvgValueG >= 0 && dAvgValueB >= 0 && dAvgValueI >= 0)
									{// chlee
										strCrossInfo.Format("RGBI: %d %d %d %d", (int)(dAvgValueR + 0.5), (int)(dAvgValueG + 0.5), (int)(dAvgValueB + 0.5), (int)(dAvgValueI + 0.5));
										CrossStrPos.x = (int)dCrossLTPoint.x;
										CrossStrPos.y = (int)dCrossLTPoint.y;

										SetRgb(m_lWindowID, 0, 0, 255);
										SetTposition(m_lWindowID, CrossStrPos.y - 35, CrossStrPos.x);
										WriteString(m_lWindowID, (HTuple)strCrossInfo);
									}
								}
							}
						}

					}	// if (pRegion->GetVisible())
				}

				GTRegion* pSelectRgn;
				pSelectRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetCurSelectRegion();
				if (pSelectRgn != NULL)
					pSelectRgn->Draw(m_lWindowID, &m_ViewportManager, GTR_DS_NORMAL, FALSE, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);

				if (mpActiveTRegion)
					mpActiveTRegion->Draw(m_lWindowID, &m_ViewportManager, GTR_DS_NORMAL, TRUE, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);
			}
		}

		if (m_bDisplayDefectArea)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(m_HParameterDefectRgn))
			{
				SetColor(m_lWindowID, "blue");
				SetDraw(m_lWindowID, "margin");
				DispObj(m_HParameterDefectRgn, m_lWindowID);

				SetColor(m_lWindowID, "orange");
				SetDraw(m_lWindowID, "margin");
				DispObj(m_HAIDefectRgn, m_lWindowID);

				HalconCpp::SetFont(m_lWindowID, "Arial-Normal-17");

				SetRgb(m_lWindowID, 255, 0, 0);

				AnalyzeAndDisplayDefects(m_HParameterDefectRgn, m_lWindowID);
				AnalyzeAndDisplayDefects(m_HAIDefectRgn, m_lWindowID);
			}

			if (THEAPP.m_pGFunction->ValidHXLD(m_HParameterDefectXLD))
			{
				SetLineStyle(m_lWindowID, (HTuple(5).Append(5)));
				SetLineWidth(m_lWindowID, HTuple(2));

				SetColor(m_lWindowID, "red");
				DispObj(m_HParameterDefectXLD, m_lWindowID);

				SetLineStyle(m_lWindowID, HTuple());
				SetLineWidth(m_lWindowID, HTuple(1));
			}

			if (THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pAlgorithm->m_HGapMeasureXLD))
			{
				SetColor(m_lWindowID, "yellow");
				DispObj(THEAPP.m_pAlgorithm->m_HGapMeasureXLD, m_lWindowID);

				HalconCpp::SetFont(m_lWindowID, "Arial-Normal-17");
				SetRgb(m_lWindowID, 255, 0, 0);

				HTuple HNoXLD;
				Hlong lNoXLD;
				HObject HEachMeasureResultXLD;

				CountObj(THEAPP.m_pAlgorithm->m_HGapMeasureXLD, &HNoXLD);
				lNoXLD = HNoXLD.L();

				for (int i = 0; i < lNoXLD; i++)
				{
					SelectObj(THEAPP.m_pAlgorithm->m_HGapMeasureXLD, &HEachMeasureResultXLD, i + 1);

					if (THEAPP.m_pGFunction->ValidHXLD(HEachMeasureResultXLD) == FALSE)
						continue;

					double dRow1, dRow2, dCol1, dCol2;
					HTuple HlRow1, HlRow2, HlCol1, HlCol2;

					SmallestRectangle1Xld(HEachMeasureResultXLD, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

					dRow1 = HlRow1.D();
					dCol1 = HlCol1.D();
					dRow2 = HlRow2.D();
					dCol2 = HlCol2.D();

					CString sDistanceMeasureResult;
					sDistanceMeasureResult.Format("Gap: %.1lf(um)", THEAPP.m_pAlgorithm->m_HGapDistanceMeasureResult[i].D());

					SetTposition(m_lWindowID, (long)dRow1 - 100, (long)dCol2 + 10);
					WriteString(m_lWindowID, (HTuple)sDistanceMeasureResult);
				}
			}

			if (THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pAlgorithm->m_HCornerMeasureXLD))
			{
				SetColor(m_lWindowID, "red");
				DispObj(THEAPP.m_pAlgorithm->m_HCornerMeasureXLD, m_lWindowID);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(m_HBarcodeDetectRgn))
			{
				SetColor(m_lWindowID, "green");
				SetDraw(m_lWindowID, "margin");
				DispObj(m_HBarcodeDetectRgn, m_lWindowID);

				HalconCpp::SetFont(m_lWindowID, "Arial-Normal-17");

				SetRgb(m_lWindowID, 255, 0, 0);

				POINT BarcodeLTPoint;
				Hlong lRow1, lRow2, lCol1, lCol2;
				HTuple HlRow1, HlRow2, HlCol1, HlCol2;

				SmallestRectangle1(m_HBarcodeDetectRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

				lRow1 = HlRow1.L();
				lCol1 = HlCol1.L();
				lRow2 = HlRow2.L();
				lCol2 = HlCol2.L();

				BarcodeLTPoint.x = lCol1;
				BarcodeLTPoint.y = lRow1;

				BarcodeLTPoint.y -= 15;

				CString sBarcodeShiftResult, sBarcodeRotationResult, sBarcodeResult;
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strDialog.Format("위치");
				else
					strDialog.Format("Shift");
				if (THEAPP.m_StructModuleBarcodeResult[0][0][0].m_bBarcodeShiftNG)
					sBarcodeShiftResult.Format("%s: NG(%.3lf,%.3lf mm)", strDialog, THEAPP.m_StructModuleBarcodeResult[0][0][0].m_dBarcodePosOffsetX, THEAPP.m_StructModuleBarcodeResult[0][0][0].m_dBarcodePosOffsetY);
				else
					sBarcodeShiftResult.Format("%s: OK(%.3lf,%.3lf mm)", strDialog, THEAPP.m_StructModuleBarcodeResult[0][0][0].m_dBarcodePosOffsetX, THEAPP.m_StructModuleBarcodeResult[0][0][0].m_dBarcodePosOffsetY);

				if (THEAPP.m_iPGMLanguageSelect == 0)
					strDialog.Format("회전");
				else
					strDialog.Format("Rotation");
				if (THEAPP.m_StructModuleBarcodeResult[0][0][0].m_bBarcodeRotationNG)
					sBarcodeRotationResult.Format("%s: NG(%.03f도)", strDialog, THEAPP.m_StructModuleBarcodeResult[0][0][0].m_iBarcodeRotationResult);
				else
					sBarcodeRotationResult.Format("%s: OK(%.03f도)", strDialog, THEAPP.m_StructModuleBarcodeResult[0][0][0].m_iBarcodeRotationResult);

				if (THEAPP.m_iPGMLanguageSelect == 0)
					strDialog.Format("결과");
				else
					strDialog.Format("Result");
				sBarcodeResult.Format("%s: %s, %s, %s", strDialog, THEAPP.m_StructModuleBarcodeResult[0][0][0].m_sBarcodeResult, sBarcodeShiftResult, sBarcodeRotationResult);

				SetTposition(m_lWindowID, BarcodeLTPoint.y, BarcodeLTPoint.x);
				WriteString(m_lWindowID, (HTuple)sBarcodeResult);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(m_HOCRDetectRgn))//OCR View
			{
				SetColor(m_lWindowID, "red");
				SetDraw(m_lWindowID, "margin");
				DispObj(m_HOCRDetectRgn, m_lWindowID);

				Hlong lCount;
				HTuple HlCount;
				CountObj(m_HOCRDetectRgn, &HlCount);
				lCount = HlCount.L();

				SetColor(m_lWindowID, "green");
				HalconCpp::SetFont(m_lWindowID, "Arial-Normal-40");

				HObject HRgn;
				for (int i = 1; i <= lCount; i++)
				{
					SelectObj(m_HOCRDetectRgn, &HRgn, i);
					Hlong lRow1, lColumn1, lRow2, lColumn2;
					HTuple HlRow1, HlRow2, HlColumn1, HlColumn2;

					SmallestRectangle1(HRgn, &HlRow1, &HlColumn1, &HlRow2, &HlColumn2);

					lRow1 = HlRow1.L();
					lColumn1 = HlColumn1.L();
					lRow2 = HlRow2.L();
					lColumn2 = HlColumn2.L();

					SetTposition(m_lWindowID, lRow1 - 10, lColumn1 + 10);

					if (THEAPP.m_pAlgorithm->m_strOCRChar.GetLength() == lCount)
						WriteString(m_lWindowID, (LPCTSTR)THEAPP.m_pAlgorithm->m_strOCRChar.Mid(i - 1, 1));
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(m_HAutoFocusRgn))
			{
				SetColor(m_lWindowID, "blue");
				SetDraw(m_lWindowID, "margin");
				DispObj(m_HAutoFocusRgn, m_lWindowID);

				SetColor(m_lWindowID, "red");
				SetDraw(m_lWindowID, "margin");
				DispObj(m_HAutoFocusCenterRgn, m_lWindowID);

				HalconCpp::SetFont(m_lWindowID, "Arial-Normal-17");

				SetRgb(m_lWindowID, 255, 0, 0);

				AnalyzeAndDisplayAutoFocusCenter(m_HAutoFocusCenterRgn, m_lWindowID);
			}
		}

		/////////////////////////////////////////////////////////
		// Caliper
		/////////////////////////////////////////////////////////

		if (m_caliper_control_flag == 1)
		{
			if (m_iNoCaliperPoint == 2)
				DrawArrow(m_lWindowID);
			else if (m_iNoCaliperPoint == 1)
				THEAPP.m_pGFunction->DisplayCross(m_lWindowID, mpLineSP.x, mpLineSP.y, RGB(0, 255, 0), 50);
		}

		/////////////////////////////////////////////////////////

		// CrossBar
		if (m_bDispCrossBar)
		{
			SetColor(m_lWindowID, "red");
			SetDraw(m_lWindowID, "margin");

			DispObj(m_HCrossBarXLD, m_lWindowID);
		}

		if (m_bDisplayFitLine)
		{
			if (m_bLive == FALSE)
			{
				if (m_bPolygonMode)
				{
					for (int iii = 0; iii < m_iPolygonCnt; iii++)
					{
						THEAPP.m_pGFunction->DisplayCross(m_lWindowID, m_PolygonPt[iii].x, m_PolygonPt[iii].y, RGB(0, 255, 0), 25);
					}
				}
			}
		}

		// AutoFocus Area Display

		if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)	// Align Tab
		{
			if (THEAPP.m_pGFunction->ValidHRegion(m_HAutoFocusCheckRgn))
			{
				SetColor(m_lWindowID, "magenta");
				SetDraw(m_lWindowID, "margin");
				DispObj(m_HAutoFocusCheckRgn, m_lWindowID);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(m_HMultiModuleCropRgn))
			{
				SetLineStyle(m_lWindowID, (HTuple(10).Append(10)));
				SetLineWidth(m_lWindowID, HTuple(5));
				SetColor(m_lWindowID, "red");
				SetDraw(m_lWindowID, "margin");
				DispObj(m_HMultiModuleCropRgn, m_lWindowID);
				SetLineWidth(m_lWindowID, 1);
				SetLineStyle(m_lWindowID, HTuple());
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(m_HShapeModelContrastRgn))
		{
			SetColor(m_lWindowID, "red");
			SetDraw(m_lWindowID, "fill");
			DispObj(m_HShapeModelContrastRgn, m_lWindowID);
		}

	}	// if(THEAPP.m_iModeSwitch==MODE_ADMIN_TEACH_VIEW)

	SetWindowDc(m_lWindowID, 0);
	m_bOnPaintNow = FALSE;
	ReleaseDC(pDC);

	// Do not call CDialog::OnPaint() for painting messages
}

void CInspectAdminViewDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CInspectAdminViewDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CInspectAdminViewDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bDisplayImage) {
		CDialog::OnLButtonDown(nFlags, point);
		return;
	}

	point.y -= m_iToolBarOffset;	// Toolbar offset

	POINT IPoint = point;

	m_ViewportManager.VPtoIP(&IPoint, 1);

	maMVPoints[0] = point;
	maMIPoints[0] = IPoint;

	SetCapture();

	if (m_ToolBarState == TS_MOVE) {

		m_bOnMoving = TRUE;

		PostMessage(WM_SETCURSOR);

		CDialog::OnLButtonDown(nFlags, point);
		return;
	}

	if (m_ToolBarState == TS_SELECT_PART || m_ToolBarState == TS_CREATE_AF_ROI)
	{
		mpSelectPartTRegion = new CSelectPartRegion;

		if (mpSelectPartTRegion)
		{
			mpSelectPartTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			mpSelectPartTRegion->SetSelect(TRUE, FALSE);
			DrawSelectPartTRegion();
		}

		CDialog::OnLButtonDown(nFlags, point);
		return;
	}

	if (THEAPP.m_iModeSwitch != MODE_ADMIN_TEACH_VIEW)
	{
		CDialog::OnLButtonDown(nFlags, point);
		return;
	}

	ClearAllSelectRegion();

	if (m_ToolBarState == TS_DRAW)	// Cross-Bar
	{
		if (m_bDispCrossBar)
		{
			CDialog::OnLButtonDown(nFlags, point);
			return;
		}
	}

	switch (m_ToolBarState)
	{
	case TS_DRAW:
	{
		if (m_caliper_control_flag == 1)
		{
			if (m_iNoCaliperPoint == 0 || m_iNoCaliperPoint == 2)
			{
				mpLineEP = mpLineSP = IPoint;
				m_iNoCaliperPoint = 1;
			}
			else
			{
				mpLineEP = IPoint;
				m_iNoCaliperPoint = 2;
			}

			InvalidateRect(&m_ClientRect, FALSE);

			CDialog::OnLButtonDown(nFlags, point);
			return;
		}

		mpActiveTRegion = NULL;

		if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == FALSE)	// 알고리즘 창이 있을 때는 선택 못함...
		{

			if (!mpActiveTRegion && THEAPP.m_pModelDataManager->m_pInspectionArea)
			{
				mpActiveTRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetTopTRegion(point, &m_ViewportManager, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);
			}

			if (!mpActiveTRegion)
				return;

			if (mpActiveTRegion->m_bRegionROI)
			{
				HTuple IsInside;
				TestRegionPoint(mpActiveTRegion->m_HTeachPolygonRgn, (HTuple)IPoint.y, (HTuple)IPoint.x, &IsInside);

				if (IsInside == 1)
					miActiveCPointIndex = GTR_CP_REGION;
				else
					miActiveCPointIndex = GTR_CP_NONE;
			}
			else
			{
				miActiveCPointIndex = mpActiveTRegion->GetCPointIndex(point, &m_ViewportManager, THEAPP.m_pCalDataService);
			}

			if (miActiveCPointIndex != GTR_CP_NONE)
			{
				ClearAllLastSelectRegion();
				mpActiveTRegion->SetSelect(TRUE, FALSE);

				if (mpActiveTRegion != NULL)
				{
					GenEmptyObj(&m_HInspectAreaRgn);					// 검사 영역 리셋

					if ((mpActiveTRegion->miInspectionType > INSPECTION_TYPE_UNDEFINED && mpActiveTRegion->miInspectionType < INSPECTION_TYPE_DELETE) ||
						mpActiveTRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN || mpActiveTRegion->miInspectionType == INSPECTION_TYPE_DONTCARE ||
						mpActiveTRegion->miInspectionType == INSPECTION_TYPE_GENERATE || mpActiveTRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE ||
						mpActiveTRegion->miInspectionType == INSPECTION_TYPE_MASK ||
						(mpActiveTRegion->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && mpActiveTRegion->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) ||
						mpActiveTRegion->miInspectionType == INSPECTION_TYPE_LIGHT || mpActiveTRegion->miInspectionType == INSPECTION_TYPE_FOCUS || mpActiveTRegion->miInspectionType == INSPECTION_TYPE_COLOR ||
						(mpActiveTRegion->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && mpActiveTRegion->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST))
					{
						delegateCurROITabParamSave.FireEvent((WPARAM)THEAPP.m_pTabControlDlg->m_iCurrentTab);

						mpLastSelectedRegion = mpActiveTRegion;		// 마지막 선택된 ROI / 검사 파라미터 설정용 

						delegateROISelected.FireEvent((WPARAM)THEAPP.m_pTabControlDlg->m_iCurrentTab);
					}
				}

				DrawActiveTRegion();
			}
			else
				mpActiveTRegion = NULL;
		}

		break;
	}	// case TS_DRAW:

	case TS_CREATE_ROI_RECT:
	case TS_CREATE_ROI_CIRCLE:
	case TS_CREATE_ROI_CROSS:
	{
		mpActiveTRegion = new GTRegion;

		if (mpActiveTRegion)
		{
			mpActiveTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
			mpActiveTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			mpActiveTRegion->SetSelect(TRUE, FALSE);
			DrawActiveTRegion();
		}

		break;
	}

	case TS_CREATE_ROI_POLYGON:
	{
		mpActiveTRegion = new GTRegion;

		if (mpActiveTRegion)
		{
			mpActiveTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
			mpActiveTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			mpActiveTRegion->SetSelect(TRUE, FALSE);
			DrawActiveTRegion();
		}

		break;
	}

	default:
		break;
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CInspectAdminViewDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bDisplayImage)
		return;

	ReleaseCapture();

	DPOINT dLTPoint, dRBPoint;
	POINT WHPoint;

	if (m_ToolBarState == TS_MOVE || m_ToolBarState == TS_SELECT_PART || m_ToolBarState == TS_CREATE_AF_ROI)
	{
		switch (m_ToolBarState)
		{
		case TS_MOVE:
			m_bOnMoving = FALSE;
			break;

		case TS_SELECT_PART:
			if (mpSelectPartTRegion)
			{
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

				if (iSelectImageHeight <= iSelectImageWidth)			// 20080129 Eunsung
					dZoomRatio = (float)RECTWIDTH(&m_ClientRect) / (float)iSelectImageWidth;
				else
					dZoomRatio = (float)RECTHEIGHT(&m_ClientRect) / (float)iSelectImageHeight;

				ClearWindow(m_lWindowID);

				ZoomInSelectedPart(dZoomRatio, (int)dLTPoint.x, (int)dLTPoint.y);
			}
			break;

		case TS_CREATE_AF_ROI:
			if (mpSelectPartTRegion)
			{
				mpSelectPartTRegion->GetWHPoint(&WHPoint, THEAPP.m_pCalDataService);

				if (WHPoint.x < 5 || WHPoint.y < 5) {
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

				GenRectangle1(&m_HAutoFocusCheckRgn, dLTPoint.y, dLTPoint.x, dRBPoint.y, dRBPoint.x);

				m_ToolBarState = m_PrevToolBarState;
			}
			break;
		}
	}
	else
	{
		if (THEAPP.m_iModeSwitch != MODE_ADMIN_TEACH_VIEW)
		{
			mpActiveTRegion = NULL;
			return;
		}

		POINT LTPoint, RBPoint;

		HObject HROIHRegion;
		BOOL bDontCareExist, bAddExist;
		HObject HRgn;

		GTRegion* pInspectROI;

		if (mpActiveTRegion)
		{
			switch (m_ToolBarState)
			{
			case TS_DRAW:
			{
				mpActiveTRegion->ArrangePoints();

				if (mpActiveTRegion->GetShape() == TRUE)	// Circle
				{
					mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
					mpActiveTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

					if ((RBPoint.x - LTPoint.x) >= (RBPoint.y - LTPoint.y)) {
						RBPoint.x = LTPoint.x + (RBPoint.x - LTPoint.x);
						RBPoint.y = LTPoint.y + (RBPoint.x - LTPoint.x);
					}
					else {
						RBPoint.x = LTPoint.x + (RBPoint.y - LTPoint.y);
						RBPoint.y = LTPoint.y + (RBPoint.y - LTPoint.y);
					}

					mpActiveTRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
					mpActiveTRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
				}

				UpdateHistogram(mpActiveTRegion);

				break;
			}

			case TS_CREATE_ROI_RECT:
			case TS_CREATE_ROI_CIRCLE:
			{
				mpActiveTRegion->GetWHPoint(&WHPoint, THEAPP.m_pCalDataService);

				if (WHPoint.x < 5 && WHPoint.y < 5)
				{
					SAFE_DELETE(mpActiveTRegion);
					break;
				}

				mpActiveTRegion->ArrangePoints();

				mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

				SAFE_DELETE(mpActiveTRegion);

				InspectionTypeResult inspectionTypeResult;
				if (!ShowInspectionTypeDialog(inspectionTypeResult, true, this))
					break;

				pInspectROI = new GTRegion;

				if (m_ToolBarState == TS_CREATE_ROI_RECT)
					pInspectROI->SetShape(FALSE, FALSE);
				else if (m_ToolBarState == TS_CREATE_ROI_CIRCLE)
				{
					if ((RBPoint.x - LTPoint.x) >= (RBPoint.y - LTPoint.y)) {
						RBPoint.x = LTPoint.x + (RBPoint.x - LTPoint.x);
						RBPoint.y = LTPoint.y + (RBPoint.x - LTPoint.x);
					}
					else {
						RBPoint.x = LTPoint.x + (RBPoint.y - LTPoint.y);
						RBPoint.y = LTPoint.y + (RBPoint.y - LTPoint.y);
					}

					pInspectROI->SetShape(TRUE, FALSE);
				}

				if (inspectionTypeResult.genLineRegion)
				{
					if (m_ToolBarState == TS_CREATE_ROI_RECT)
						GenRectangle1(&HRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
					else if (m_ToolBarState == TS_CREATE_ROI_CIRCLE)
						GenCircle(&HRgn, (LTPoint.y + RBPoint.y) / 2, (LTPoint.x + RBPoint.x) / 2, (RBPoint.y - LTPoint.y) / 2);
					Boundary(HRgn, &HRgn, "inner");
				}

				pInspectROI->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
				pInspectROI->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);

				pInspectROI->miTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;

				pInspectROI->miInspectionType = inspectionTypeResult.inspectionType;
				pInspectROI->miFaiRoiIndex = inspectionTypeResult.faiRoiNumber;

				pInspectROI->SetSelectable(TRUE, FALSE);
				pInspectROI->SetMovable(TRUE, FALSE);

				if (inspectionTypeResult.genLineRegion)
					pInspectROI->SetSizable(FALSE, FALSE);
				else
					pInspectROI->SetSizable(TRUE, FALSE);

				if (pInspectROI->miInspectionType == INSPECTION_TYPE_DELETE)
					pInspectROI->SetLineColor(RGB(0, 0, 255));
				else if (pInspectROI->miInspectionType == INSPECTION_TYPE_GLOBAL_ALIGN)
					pInspectROI->SetLineColor(RGB(0, 255, 255));
				else if (pInspectROI->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
					pInspectROI->SetLineColor(RGB(255, 0, 255));
				else if (pInspectROI->miInspectionType == INSPECTION_TYPE_DONTCARE)
					pInspectROI->SetLineColor(RGB(128, 0, 255));
				else if (pInspectROI->miInspectionType == INSPECTION_TYPE_GENERATE)
					pInspectROI->SetLineColor(RGB(255, 0, 128));
				else if (pInspectROI->miInspectionType == INSPECTION_TYPE_MASK)
					pInspectROI->SetLineColor(RGB(0, 0, 255));
				else if (pInspectROI->miInspectionType == INSPECTION_TYPE_ADD)
					pInspectROI->SetLineColor(RGB(255, 128, 0));
				else if (pInspectROI->miInspectionType == INSPECTION_TYPE_COLOR)
					pInspectROI->SetLineColor(RGB(255, 255, 255));
				else if (pInspectROI->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE)
					pInspectROI->SetLineColor(RGB(0, 255, 255));
				else if (pInspectROI->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && pInspectROI->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE)
					pInspectROI->SetLineColor(RGB(0, 255, 255));
				else if (pInspectROI->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && pInspectROI->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST)
					pInspectROI->SetLineColor(RGB(255, 128, 0));
				else if (pInspectROI->miInspectionType == INSPECTION_TYPE_AUTO_UNDEFINED)
					pInspectROI->SetLineColor(RGB(255, 255, 0));
				else
					pInspectROI->SetLineColor(RGB(0, 255, 0));
				pInspectROI->miLineStyle = PS_SOLID;

				if (inspectionTypeResult.genLineRegion)
				{
					pInspectROI->m_bRegionROI = TRUE;
					CopyObj(HRgn, &(pInspectROI->m_HTeachPolygonRgn), 1, -1);
				}
				else
					pInspectROI->m_bRegionROI = FALSE;

				if ((pInspectROI->miInspectionType == INSPECTION_TYPE_DELETE || pInspectROI->miInspectionType == INSPECTION_TYPE_ADD) == FALSE)
				{
					HROIHRegion = pInspectROI->GetROIHRegion(THEAPP.m_pCalDataService);

					bDontCareExist = FALSE;
					bDontCareExist = CheckDontCareInclusion(HROIHRegion);

					if (bDontCareExist)
					{
						int nRes = 0;
						if (THEAPP.m_iPGMLanguageSelect == 0)
							strMessageBox.Format("새로운 ROI 내에 비검사 ROI가 존재합니다. 비검사 ROI 영역을 제외하시겠습니까?");
						else
							strMessageBox.Format("The new ROI contains an Exclusion ROI. Should the Exclusion ROI area be excluded?");
						nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

						if ((nRes == IDYES))
							GetRegionROIAfterDontCareRemoval(pInspectROI);
					}

					bAddExist = FALSE;
					bAddExist = CheckAddInclusion(HROIHRegion);

					if (bAddExist)
					{
						int nRes = 0;
						if (THEAPP.m_iPGMLanguageSelect == 0)
							strMessageBox.Format("새로운 ROI 내에 추가 ROI가 존재합니다. ROI 영역을 추가하시겠습니까?");
						else
							strMessageBox.Format("The new ROI contains an Inclusion ROI. Should the Inclusion ROI area be included?");
						nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

						if ((nRes == IDYES))
							GetRegionROIAfterAddRemoval(pInspectROI);
					}

				}

				if (pInspectROI->miInspectionType >= INSPECTION_TYPE_FIRST && pInspectROI->miInspectionType <= INSPECTION_TYPE_LAST)
				{
					int iLastInspectionROIID = 0;
					iLastInspectionROIID = THEAPP.m_pModelDataManager->GetLastPadID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->miInspectionROIID = iLastInspectionROIID + 1;
				}

				if (pInspectROI->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
				{
					int iLastLocalAlignID = 0;
					iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->miLocalAlignID = iLastLocalAlignID + 1;
				}

				if (pInspectROI->miInspectionType == INSPECTION_TYPE_DONTCARE)
				{
					int iLastDontCareID = 0;
					iLastDontCareID = THEAPP.m_pModelDataManager->GetLastDontCareID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->miDontCareID = iLastDontCareID + 1;
				}

				if (pInspectROI->miInspectionType == INSPECTION_TYPE_GENERATE)
				{
					int iLastGenerateID = 0;
					iLastGenerateID = THEAPP.m_pModelDataManager->GetLastGenerateID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->miGenerateID = iLastGenerateID + 1;
				}

				if (pInspectROI->miInspectionType == INSPECTION_TYPE_MASK)
				{
					int iLastMaskID = 0;
					iLastMaskID = THEAPP.m_pModelDataManager->GetLastMaskID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->miMaskID = iLastMaskID + 1;
				}

				if (pInspectROI->miInspectionType == INSPECTION_TYPE_COLOR)
				{
					int iLastColorInfoID = 0;
					iLastColorInfoID = THEAPP.m_pModelDataManager->GetLastColorInfoID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->miColorInfoID = iLastColorInfoID + 1;
				}

				if (pInspectROI->miInspectionType == INSPECTION_TYPE_MEASURE_POINT)	// 측정 Point
				{
					if (inspectionTypeResult.hasPositionPoint)
					{
						pInspectROI->miPositionID = inspectionTypeResult.positionId;
						pInspectROI->miPointID = inspectionTypeResult.pointId;
					}
				}

				THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pInspectROI);

				UpdateHistogram(pInspectROI);

				THEAPP.m_pInspectAdminViewToolbarDlg->SetToolBarStateDraw();

				break;
			}

			case TS_CREATE_ROI_POLYGON:

				mpActiveTRegion->ArrangePoints();
				mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);

				SAFE_DELETE(mpActiveTRegion);

				m_PolygonPt[m_iPolygonCnt].x = LTPoint.x;
				m_PolygonPt[m_iPolygonCnt].y = LTPoint.y;
				++m_iPolygonCnt;

				break;

			case TS_CREATE_ROI_CROSS:

				mpActiveTRegion->GetWHPoint(&WHPoint, THEAPP.m_pCalDataService);

				if (WHPoint.x < 2 || WHPoint.y < 2)
				{
					SAFE_DELETE(mpActiveTRegion);
					break;
				}

				mpActiveTRegion->ArrangePoints();

				mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

				SAFE_DELETE(mpActiveTRegion);

				pInspectROI = new GTRegion;
				pInspectROI->SetShape(FALSE, FALSE);
				pInspectROI->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
				pInspectROI->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);

				pInspectROI->miTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;

				pInspectROI->miInspectionType = INSPECTION_TYPE_CROSS_BAR;

				pInspectROI->SetSelectable(TRUE, FALSE);
				pInspectROI->SetMovable(TRUE, FALSE);
				pInspectROI->SetSizable(TRUE, FALSE);
				pInspectROI->miLineStyle = PS_SOLID;

				THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pInspectROI);

				UpdateHistogram(pInspectROI);

				THEAPP.m_pInspectAdminViewToolbarDlg->SetToolBarStateDraw();

				break;
			}
		}	// if (mpActiveTRegion)
		else	// Cross Bar Position
		{
			if (m_bDispCrossBar)
			{
				if (m_ToolBarState == TS_DRAW)
				{
					point.y -= m_iToolBarOffset;	// Toolbar offset
					POINT IPoint = point;
					m_ViewportManager.VPtoIP(&IPoint, 1);
					GenEmptyObj(&m_HCrossBarXLD);
					int iCrossBarLength;
					if (THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[THEAPP.m_iCurTeachVision] >= THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[THEAPP.m_iCurTeachVision])
						iCrossBarLength = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[THEAPP.m_iCurTeachVision];
					else
						iCrossBarLength = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[THEAPP.m_iCurTeachVision];
					GenCrossContourXld(&m_HCrossBarXLD, (double)IPoint.y, (double)IPoint.x, (double)iCrossBarLength * 2.0, 0.0);
				}
			}
		}
	}	// else

	int iViewWidth, iViewHeight;

	iViewWidth = (int)(m_iImageWidth * m_ViewportManager.mdZoomRatio + 0.5);
	iViewHeight = (int)(m_iImageHeight * m_ViewportManager.mdZoomRatio + 0.5);

	if (iViewWidth < m_ClientRect.Width() || iViewHeight < m_ClientRect.Height())
		InvalidateRect(&m_ClientRect, TRUE);
	else
		InvalidateRect(&m_ClientRect, FALSE);

	CDialog::OnLButtonUp(nFlags, point);
}

void CInspectAdminViewDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bDisplayImage)
		return;

	BOOL bDisplayProfileData = FALSE;

	int GrayValue = 0;
	int iRed, iGreen, iBlue;
	double dHeightMm;

	if (bDisplayProfileData == FALSE)
	{
		iRed = iGreen = iBlue = 0;
		COLORREF Color;
		CDC* pDC = GetDC();
		Color = pDC->GetPixel(point.x, point.y);
		iRed = GetRValue(Color);
		iGreen = GetGValue(Color);
		iBlue = GetBValue(Color);
		GrayValue = (int)((iRed + iGreen + iBlue) / 3.0);
		ReleaseDC(pDC);
	}

	point.y -= m_iToolBarOffset;	// Toolbar offset

	POINT VPoint = point;
	POINT IPoint = point;

	m_ViewportManager.VPtoIP(&IPoint, 1);

	maMVPoints[1] = VPoint;
	maMIPoints[1] = IPoint;

	CString OutTxt;

	if (bDisplayProfileData)
	{
		HTuple HGrayValue;

		try
		{
			POINT Pos;
			Pos.x = (int)((point.x / m_dZoomRatio) + m_ViewRect.left);
			Pos.y = (int)((point.y / m_dZoomRatio) + m_ViewRect.top);

			GetGrayval(m_HTeachingImage[CHANNEL_TYPE_I][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], Pos.y, Pos.x, &HGrayValue);

			if (HGrayValue == INVALID_LASER_DATA)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					OutTxt.Format("원본 영상 - (%d, %d) = 잘못된 측정값, 배율(%d%%)", (int)((point.x / m_dZoomRatio) + m_ViewRect.left), (int)((point.y / m_dZoomRatio) + m_ViewRect.top), (int)(m_dZoomRatio * 100.));
				else
					OutTxt.Format("Raw Image - (%d, %d) = Invalid Height, Zoom(%d%%)", (int)((point.x / m_dZoomRatio) + m_ViewRect.left), (int)((point.y / m_dZoomRatio) + m_ViewRect.top), (int)(m_dZoomRatio * 100.));
			}
			else
			{
				dHeightMm = HGrayValue[0].D() * 0.01 * 0.001;
				if (THEAPP.m_iPGMLanguageSelect == 0)
					OutTxt.Format("원본 영상 - (%d, %d) = %.3lf(mm), Zoom(%d%%)", (int)((point.x / m_dZoomRatio) + m_ViewRect.left), (int)((point.y / m_dZoomRatio) + m_ViewRect.top), dHeightMm, (int)(m_dZoomRatio * 100.));
				else
					OutTxt.Format("Raw Image - (%d, %d) = %.3lf(mm), Zoom(%d%%)", (int)((point.x / m_dZoomRatio) + m_ViewRect.left), (int)((point.y / m_dZoomRatio) + m_ViewRect.top), dHeightMm, (int)(m_dZoomRatio * 100.));
			}

			SetWindowText((LPCTSTR)OutTxt);
		}
		catch (HException& except)
		{
		}
	}
	else
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
		{
			if (m_iDisplayChannelType == CHANNEL_TYPE_COLOR)
				OutTxt.Format("원본 영상 - (%d, %d) = [%03d, %03d, %03d], 배율(%d%%)", (int)((point.x / m_dZoomRatio) + m_ViewRect.left), (int)((point.y / m_dZoomRatio) + m_ViewRect.top), iRed, iGreen, iBlue, (int)(m_dZoomRatio * 100.));
			else
				OutTxt.Format("원본 영상 - (%d, %d) = %03d, 배율(%d%%)", (int)((point.x / m_dZoomRatio) + m_ViewRect.left), (int)((point.y / m_dZoomRatio) + m_ViewRect.top), GrayValue, (int)(m_dZoomRatio * 100.));
		}
		else
		{
			if (m_iDisplayChannelType == CHANNEL_TYPE_COLOR)
				OutTxt.Format("Raw Image - (%d, %d) = [%03d, %03d, %03d], Zoom(%d%%)", (int)((point.x / m_dZoomRatio) + m_ViewRect.left), (int)((point.y / m_dZoomRatio) + m_ViewRect.top), iRed, iGreen, iBlue, (int)(m_dZoomRatio * 100.));
			else
				OutTxt.Format("Raw Image - (%d, %d) = %03d, Zoom(%d%%)", (int)((point.x / m_dZoomRatio) + m_ViewRect.left), (int)((point.y / m_dZoomRatio) + m_ViewRect.top), GrayValue, (int)(m_dZoomRatio * 100.));
		}

		SetWindowText((LPCTSTR)OutTxt);
	}

	int iWidth, iHeight;
	long HCurPos, VCurPos;
	CPoint MVDiffPoint, MIDiffPoint;

	if (IS_SETFLAG(nFlags, MK_LBUTTON))
	{
		switch (m_ToolBarState)
		{
		case TS_DRAW:
		{
			if (THEAPP.m_iModeSwitch != MODE_ADMIN_TEACH_VIEW)
				return;

			if (!mpActiveTRegion)
				return;

			if (mpActiveTRegion->m_bRegionROI)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(mpActiveTRegion->m_HTeachPolygonRgn))
				{
					POINT MLTPoint_B, MLTPoint_A;

					DrawActiveTRegion();

					mpActiveTRegion->GetLTPoint(&MLTPoint_B, THEAPP.m_pCalDataService);
					mpActiveTRegion->SetCPoint(miActiveCPointIndex, maMIPoints, THEAPP.m_pCalDataService);
					mpActiveTRegion->GetLTPoint(&MLTPoint_A, THEAPP.m_pCalDataService);

					MIDiffPoint.x = MLTPoint_A.x - MLTPoint_B.x;
					MIDiffPoint.y = MLTPoint_A.y - MLTPoint_B.y;

					MoveRegion(mpActiveTRegion->m_HTeachPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), MIDiffPoint.y, MIDiffPoint.x);

					DrawActiveTRegion();
				}
			}
			else
			{
				DrawActiveTRegion();
				mpActiveTRegion->SetCPoint(miActiveCPointIndex, maMIPoints, THEAPP.m_pCalDataService);
				DrawActiveTRegion();
			}

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
		case TS_CREATE_AF_ROI:
			if (!mpSelectPartTRegion)
				return;

			DrawSelectPartTRegion();

			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			DrawSelectPartTRegion();

			break;

		case TS_CREATE_ROI_RECT:
		case TS_CREATE_ROI_CIRCLE:
		case TS_CREATE_ROI_CROSS:

			if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
			{
				if (!mpActiveTRegion)
					return;

				DrawActiveTRegion();

				mpActiveTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

				DrawActiveTRegion();
			}

			break;

		default:
			break;
		}
	}

	maMVPoints[0] = maMVPoints[1];
	maMIPoints[0] = maMIPoints[1];

	CDialog::OnMouseMove(nFlags, point);
}

void CInspectAdminViewDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bDisplayImage) {
		CDialog::OnRButtonDown(nFlags, point);
		return;
	}

	if (m_bPolygonMode && m_ToolBarState == TS_CREATE_ROI_POLYGON)
	{
		AddPolygonROI();

		m_bPolygonMode = FALSE;
		m_iPolygonCnt = 0;

		THEAPP.m_pInspectAdminViewToolbarDlg->SetToolBarStateDraw();

		InvalidateRect(&m_ClientRect, FALSE);
	}
	else
	{
		CMenu* pSubMenu;
		pSubMenu = m_Menu.GetSubMenu(0);

		if (m_bDispROIGv)
			pSubMenu->CheckMenuItem(ID_MENU_ROI_GV_DISPLAY, MF_CHECKED);
		else
			pSubMenu->CheckMenuItem(ID_MENU_ROI_GV_DISPLAY, MF_UNCHECKED);

		if (m_bDispFAIMeasureValue)
			pSubMenu->CheckMenuItem(ID_MENU_FAI_MEASURE_DISPLAY, MF_CHECKED);
		else
			pSubMenu->CheckMenuItem(ID_MENU_FAI_MEASURE_DISPLAY, MF_UNCHECKED);

		if (THEAPP.Struct_PreferenceStruct.m_bUseVisionCopy)
		{
			pSubMenu->EnableMenuItem(ID_MENU_ALL_ROI_COPY_VISION_1, MF_ENABLED);
			pSubMenu->EnableMenuItem(ID_MENU_ALL_ROI_COPY_VISION_2, MF_ENABLED);
			pSubMenu->EnableMenuItem(ID_MENU_ROI_COPY_VISION_1, MF_ENABLED);
			pSubMenu->EnableMenuItem(ID_MENU_ROI_COPY_VISION_2, MF_ENABLED);
		}
		else
		{
			pSubMenu->EnableMenuItem(ID_MENU_ALL_ROI_COPY_VISION_1, MF_DISABLED);
			pSubMenu->EnableMenuItem(ID_MENU_ALL_ROI_COPY_VISION_2, MF_DISABLED);
			pSubMenu->EnableMenuItem(ID_MENU_ROI_COPY_VISION_1, MF_DISABLED);
			pSubMenu->EnableMenuItem(ID_MENU_ROI_COPY_VISION_2, MF_DISABLED);
		}

		CPoint ClientPoint;
		ClientPoint = point;
		ClientToScreen(&ClientPoint);

		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, ClientPoint.x, ClientPoint.y, this);
	}

	CDialog::OnRButtonDown(nFlags, point);
}


void CInspectAdminViewDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_lWindowID > 0)
	{
		HalconCpp::CloseWindow(m_lWindowID);
		m_lWindowID = -1;
	}

	m_Menu.DestroyMenu();
}

void CInspectAdminViewDlg::OnSize(UINT nType, int cx, int cy)
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

BOOL CInspectAdminViewDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (THEAPP.m_iModeSwitch != MODE_ADMIN_TEACH_VIEW)
	{
		CDialog::OnMouseWheel(nFlags, zDelta, pt);
	}

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

BOOL CInspectAdminViewDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
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

void CInspectAdminViewDlg::Reset()
{
	InitViewRect();
	UpdateViewportManager();
}

void CInspectAdminViewDlg::InitViewRect()
{
	m_ViewRect.left = 0;
	m_ViewRect.top = 0;
	m_ViewRect.bottom = (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectAdminViewDlg::UpdateViewportManager()
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

void CInspectAdminViewDlg::ViewRectSet()
{
	m_ViewRect.bottom = m_ViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = m_ViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectAdminViewDlg::ScrollBarSet()
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

void CInspectAdminViewDlg::ScrollBarPosSet()
{
	m_VScrBar.SetWindowPos(&wndTop, m_ClientRect.right, m_ClientRect.top, m_iScrBarWidth, m_ClientRect.bottom - m_iToolBarOffset, SWP_SHOWWINDOW);
	m_VScrBar.ShowScrollBar(TRUE);
	m_HScrBar.SetWindowPos(&wndTop, m_ClientRect.left, m_ClientRect.bottom, m_ClientRect.right, m_iScrBarWidth, SWP_SHOWWINDOW);
	m_HScrBar.ShowScrollBar(TRUE);
}

void CInspectAdminViewDlg::DrawActiveTRegion()
{
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
		mpActiveTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_ACTIVE, FALSE, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);
	}

	ReleaseDC(pDC);
}

void CInspectAdminViewDlg::DrawSelectPartTRegion()
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
		mpSelectPartTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_SELECTPART, FALSE, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);
	}
	ReleaseDC(pDC);
}

void CInspectAdminViewDlg::ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY)
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

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strDialog.Format("원본 영상 - 배율(%d%%)", (int)(m_dZoomRatio * 100.));
		else
			strDialog.Format("Raw Image - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(strDialog);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

void CInspectAdminViewDlg::SetToolBarStateDraw()
{
	m_ToolBarState = TS_DRAW;
}

#include "RoiCopyOptionDlg.h"

void CInspectAdminViewDlg::OnMenuAllRoiCopyVision1()
{
	if (THEAPP.m_iCurTeachVision == 0)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("동일한 비전을 선택하였습니다. 동일 비전에 대해서는 [전체 ROI 복사 (Vision)] 기능을 사용하세요.");
		else
			strMessageBox.Format("Same vision selected. Use [All ROI copy(Vision)] for same vision.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	CRoiCopyOptionDlg dlg;
	if (dlg.DoModal() == IDOK)
		CopyInspectionAllROI_Vision(0, dlg.m_bCheckCopyImage, dlg.m_bCheckDeleteRoiFirst);
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyVision2()
{
	if (THEAPP.m_iCurTeachVision == 1)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("동일한 비전을 선택하였습니다. 동일 비전에 대해서는 [전체 ROI 복사 (비전)] 기능을 사용하세요.");
		else
			strMessageBox.Format("Same vision selected. Use [All ROI copy(Vision)] for same vision.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	CRoiCopyOptionDlg dlg;
	if (dlg.DoModal() == IDOK)
		CopyInspectionAllROI_Vision(1, dlg.m_bCheckCopyImage, dlg.m_bCheckDeleteRoiFirst);
}

void CInspectAdminViewDlg::OnMenuRoiCopyVision1()
{
	m_iRoiCopyVisionIndex = 0;
	CopyInspectionROI_Vision();
}

void CInspectAdminViewDlg::OnMenuRoiCopyVision2()
{
	m_iRoiCopyVisionIndex = 1;
	CopyInspectionROI_Vision();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage0()
{
	m_iRoiCopyImageIndex = 0;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage1()
{
	m_iRoiCopyImageIndex = 1;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage2()
{
	m_iRoiCopyImageIndex = 2;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage3()
{
	m_iRoiCopyImageIndex = 3;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage4()
{
	m_iRoiCopyImageIndex = 4;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage5()
{
	m_iRoiCopyImageIndex = 5;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage6()
{
	m_iRoiCopyImageIndex = 6;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage7()
{
	m_iRoiCopyImageIndex = 7;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage8()
{
	m_iRoiCopyImageIndex = 8;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage9()
{
	m_iRoiCopyImageIndex = 9;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage10()
{
	m_iRoiCopyImageIndex = 10;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage11()
{
	m_iRoiCopyImageIndex = 11;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage12()
{
	m_iRoiCopyImageIndex = 12;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage13()
{
	m_iRoiCopyImageIndex = 13;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage14()
{
	m_iRoiCopyImageIndex = 14;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage15()
{
	m_iRoiCopyImageIndex = 15;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage16()
{
	m_iRoiCopyImageIndex = 16;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage17()
{
	m_iRoiCopyImageIndex = 17;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage18()
{
	m_iRoiCopyImageIndex = 18;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage19()
{
	m_iRoiCopyImageIndex = 19;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage20()
{
	m_iRoiCopyImageIndex = 20;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage21()
{
	m_iRoiCopyImageIndex = 21;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage22()
{
	m_iRoiCopyImageIndex = 22;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage23()
{
	m_iRoiCopyImageIndex = 23;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage24()
{
	m_iRoiCopyImageIndex = 24;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage25()
{
	m_iRoiCopyImageIndex = 25;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage26()
{
	m_iRoiCopyImageIndex = 26;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage27()
{
	m_iRoiCopyImageIndex = 27;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage28()
{
	m_iRoiCopyImageIndex = 28;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage29()
{
	m_iRoiCopyImageIndex = 29;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage30()
{
	m_iRoiCopyImageIndex = 30;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage31()
{
	m_iRoiCopyImageIndex = 31;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage32()
{
	m_iRoiCopyImageIndex = 32;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage33()
{
	m_iRoiCopyImageIndex = 33;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage34()
{
	m_iRoiCopyImageIndex = 34;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage35()
{
	m_iRoiCopyImageIndex = 35;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage36()
{
	m_iRoiCopyImageIndex = 36;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage37()
{
	m_iRoiCopyImageIndex = 37;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage38()
{
	m_iRoiCopyImageIndex = 38;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage39()
{
	m_iRoiCopyImageIndex = 39;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage40()
{
	m_iRoiCopyImageIndex = 40;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage41()
{
	m_iRoiCopyImageIndex = 41;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage42()
{
	m_iRoiCopyImageIndex = 42;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage43()
{
	m_iRoiCopyImageIndex = 43;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage44()
{
	m_iRoiCopyImageIndex = 44;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage45()
{
	m_iRoiCopyImageIndex = 45;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage0()
{
	m_iRoiCopyImageIndex = 0;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage1()
{
	m_iRoiCopyImageIndex = 1;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage2()
{
	m_iRoiCopyImageIndex = 2;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage3()
{
	m_iRoiCopyImageIndex = 3;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage4()
{
	m_iRoiCopyImageIndex = 4;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage5()
{
	m_iRoiCopyImageIndex = 5;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage6()
{
	m_iRoiCopyImageIndex = 6;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage7()
{
	m_iRoiCopyImageIndex = 7;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage8()
{
	m_iRoiCopyImageIndex = 8;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage9()
{
	m_iRoiCopyImageIndex = 9;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage10()
{
	m_iRoiCopyImageIndex = 10;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage11()
{
	m_iRoiCopyImageIndex = 11;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage12()
{
	m_iRoiCopyImageIndex = 12;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage13()
{
	m_iRoiCopyImageIndex = 13;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage14()
{
	m_iRoiCopyImageIndex = 14;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage15()
{
	m_iRoiCopyImageIndex = 15;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage16()
{
	m_iRoiCopyImageIndex = 16;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage17()
{
	m_iRoiCopyImageIndex = 17;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage18()
{
	m_iRoiCopyImageIndex = 18;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage19()
{
	m_iRoiCopyImageIndex = 19;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage20()
{
	m_iRoiCopyImageIndex = 20;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage21()
{
	m_iRoiCopyImageIndex = 21;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage22()
{
	m_iRoiCopyImageIndex = 22;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage23()
{
	m_iRoiCopyImageIndex = 23;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage24()
{
	m_iRoiCopyImageIndex = 24;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage25()
{
	m_iRoiCopyImageIndex = 25;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage26()
{
	m_iRoiCopyImageIndex = 26;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage27()
{
	m_iRoiCopyImageIndex = 27;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage28()
{
	m_iRoiCopyImageIndex = 28;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage29()
{
	m_iRoiCopyImageIndex = 29;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage30()
{
	m_iRoiCopyImageIndex = 30;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage31()
{
	m_iRoiCopyImageIndex = 31;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage32()
{
	m_iRoiCopyImageIndex = 32;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage33()
{
	m_iRoiCopyImageIndex = 33;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage34()
{
	m_iRoiCopyImageIndex = 34;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage35()
{
	m_iRoiCopyImageIndex = 35;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage36()
{
	m_iRoiCopyImageIndex = 36;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage37()
{
	m_iRoiCopyImageIndex = 37;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage38()
{
	m_iRoiCopyImageIndex = 38;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage39()
{
	m_iRoiCopyImageIndex = 39;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage40()
{
	m_iRoiCopyImageIndex = 40;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage41()
{
	m_iRoiCopyImageIndex = 41;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage42()
{
	m_iRoiCopyImageIndex = 42;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage43()
{
	m_iRoiCopyImageIndex = 43;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage44()
{
	m_iRoiCopyImageIndex = 44;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuAllRoiCopyImage45()
{
	m_iRoiCopyImageIndex = 45;
	CopyAllInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiGrouping()
{
	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("기본 ROI(녹색 ROI)를 한 개 ROI로 그룹핑할 수 있습니다. 진행하시겠습니까?");
	else
		strMessageBox.Format("Undefine ROI (green) can be grouped into one ROI. Continue?");
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	InspectionTypeResult inspectionTypeResult;
	if (!ShowInspectionTypeDialog(inspectionTypeResult, false, this))
		return;

	int i, k;

	HObject HGroupRgn;
	GenEmptyObj(&HGroupRgn);

	PList<GTRegion>* pDeleteTRegionList = new PList<GTRegion>(PLNO_POINTER);

	HObject HROIHRegion;

	GTRegion* pRegion;
	for (k = 0; k < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
		if (pRegion == NULL)
			continue;

		if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
			if (pRegion->miInspectionType == INSPECTION_TYPE_UNDEFINED)
			{
				HROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);
				if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
				{
					ConcatObj(HGroupRgn, HROIHRegion, &HGroupRgn);
					pDeleteTRegionList->Add(pRegion);
				}
			}
		}
	}

	// 제거
	for (i = 0; i < pDeleteTRegionList->miCount; i++)
	{
		GTRegion* pDeleteTRegion = pDeleteTRegionList->Get(i);
		THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(pDeleteTRegion, TRUE);
	}
	delete pDeleteTRegionList;

	if (THEAPP.m_pGFunction->ValidHRegion(HGroupRgn))
		Union1(HGroupRgn, &HGroupRgn);
	else
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("그룹핑할 ROI가 존재하지 않습니다.");
		else
			strMessageBox.Format("No ROIs found for grouping.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	// Grouping ROI -> Polygon ROI

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;
	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	SmallestRectangle1(HGroupRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

	lRow1 = HlRow1.L();
	lCol1 = HlCol1.L();
	lRow2 = HlRow2.L();
	lCol2 = HlCol2.L();

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	GTRegion* pPolygonRgn;

	pPolygonRgn = new GTRegion;
	pPolygonRgn->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pPolygonRgn->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
	pPolygonRgn->SetVisible(TRUE, FALSE);
	pPolygonRgn->SetSelectable(TRUE, FALSE);
	pPolygonRgn->SetMovable(TRUE, FALSE);
	pPolygonRgn->SetSizable(FALSE, FALSE);

	pPolygonRgn->miTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;

	pPolygonRgn->miInspectionType = inspectionTypeResult.inspectionType;
	pPolygonRgn->miFaiRoiIndex = inspectionTypeResult.faiRoiNumber;

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_DELETE)
		pPolygonRgn->SetLineColor(RGB(0, 0, 255));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_GLOBAL_ALIGN)
		pPolygonRgn->SetLineColor(RGB(0, 255, 255));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
		pPolygonRgn->SetLineColor(RGB(255, 0, 255));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_DONTCARE)
		pPolygonRgn->SetLineColor(RGB(128, 0, 255));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_GENERATE)
		pPolygonRgn->SetLineColor(RGB(255, 0, 128));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_MASK)
		pPolygonRgn->SetLineColor(RGB(0, 0, 255));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_ADD)
		pPolygonRgn->SetLineColor(RGB(255, 128, 0));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE)
		pPolygonRgn->SetLineColor(RGB(0, 255, 255));
	else if (pPolygonRgn->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && pPolygonRgn->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE)
		pPolygonRgn->SetLineColor(RGB(0, 255, 255));
	else if (pPolygonRgn->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && pPolygonRgn->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST)
		pPolygonRgn->SetLineColor(RGB(255, 128, 0));
	else
		pPolygonRgn->SetLineColor(RGB(0, 255, 0));

	pPolygonRgn->m_bRegionROI = TRUE;
	CopyObj(HGroupRgn, &(pPolygonRgn->m_HTeachPolygonRgn), 1, -1);

	BOOL bDontCareExist, bAddExist;

	if ((pPolygonRgn->miInspectionType == INSPECTION_TYPE_DELETE || pPolygonRgn->miInspectionType == INSPECTION_TYPE_ADD) == FALSE)
	{
		HROIHRegion = pPolygonRgn->GetROIHRegion(THEAPP.m_pCalDataService);

		bDontCareExist = FALSE;
		bDontCareExist = CheckDontCareInclusion(HROIHRegion);

		if (bDontCareExist)
		{
			int nRes = 0;
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("새로운 ROI 내에 비검사 ROI가 존재합니다. 비검사 ROI 영역을 제외하시겠습니까?");
			else
				strMessageBox.Format("The new ROI contains an Exclusion ROI. Should the Exclusion ROI area be excluded?");
			nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

			if ((nRes == IDYES))
				GetRegionROIAfterDontCareRemoval(pPolygonRgn);
		}

		bAddExist = FALSE;
		bAddExist = CheckAddInclusion(HROIHRegion);

		if (bAddExist)
		{
			int nRes = 0;
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("새로운 ROI 내에 추가 ROI가 존재합니다. ROI 영역을 추가하시겠습니까?");
			else
				strMessageBox.Format("The new ROI contains an Inclusion ROI. Should the Inclusion ROI area be included?");
			nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

			if ((nRes == IDYES))
				GetRegionROIAfterAddRemoval(pPolygonRgn);
		}
	}

	if (pPolygonRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pPolygonRgn->miInspectionType <= INSPECTION_TYPE_LAST)
	{
		int iLastInspectionROIID = 0;
		iLastInspectionROIID = THEAPP.m_pModelDataManager->GetLastPadID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miInspectionROIID = iLastInspectionROIID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		int iLastLocalAlignID = 0;
		iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miLocalAlignID = iLastLocalAlignID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_DONTCARE)
	{
		int iLastDontCareID = 0;
		iLastDontCareID = THEAPP.m_pModelDataManager->GetLastDontCareID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miDontCareID = iLastDontCareID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_GENERATE)
	{
		int iLastGenerateID = 0;
		iLastGenerateID = THEAPP.m_pModelDataManager->GetLastGenerateID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miGenerateID = iLastGenerateID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_MASK)
	{
		int iLastMaskID = 0;
		iLastMaskID = THEAPP.m_pModelDataManager->GetLastMaskID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miMaskID = iLastMaskID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_COLOR)
	{
		int iLastColorInfoID = 0;
		iLastColorInfoID = THEAPP.m_pModelDataManager->GetLastColorInfoID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miColorInfoID = iLastColorInfoID + 1;
	}

	THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pPolygonRgn);

	mpActiveTRegion = pPolygonRgn;

	UpdateHistogram(pPolygonRgn);

	InvalidateRect(&m_ClientRect, FALSE);
}

#include "SetAngleDlg.h"
void CInspectAdminViewDlg::OnMenuRoiRotate()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("변경할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for change.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	CSetAngleDlg dlg;

	if (dlg.DoModal() == IDCANCEL)
		return;

	double dRotationAngleDeg;
	dRotationAngleDeg = dlg.m_dEditAngle;

	HObject HROIHRegion;
	GenEmptyObj(&HROIHRegion);

	GetRegionROIAfterRotation(mpActiveTRegion, dRotationAngleDeg);

	UpdateHistogram(mpActiveTRegion);

	InvalidateRect(&m_ClientRect, FALSE);
}

#include "SetFlipDlg.h"
void CInspectAdminViewDlg::OnMenuRoiFlip()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("변경할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for change.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	CSetFlipDlg dlg;

	if (dlg.DoModal() == IDCANCEL)
		return;

	int iSelectedFlip;
	iSelectedFlip = dlg.m_iRadioLeftRightFlip;

	HObject HPolygonRgn, HPolygonRgn2;
	HPolygonRgn = mpActiveTRegion->GetROIHRegion(THEAPP.m_pCalDataService);

	HTuple RowC, ColC, Area;
	AreaCenter(HPolygonRgn, &Area, &RowC, &ColC);

	int iRowC, iColC;
	if (1) // 이미지 중심을 기준으로 반전
	{
		iRowC = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[THEAPP.m_iCurTeachVision] / 2;
		iColC = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[THEAPP.m_iCurTeachVision] / 2;
	}
	else // 선택된 ROI의 중심을 기준으로 반전 (비활성화)
	{
		iRowC = RowC.D();
		iColC = ColC.D();
	}

	HTuple H, HFlip;
	HomMat2dIdentity(&H);
	if (iSelectedFlip == 0)
		HomMat2dScale(H, 1, -1, iRowC, iColC, &HFlip);
	else
		HomMat2dScale(H, -1, 1, iRowC, iColC, &HFlip);

	AffineTransRegion(HPolygonRgn, &HPolygonRgn2, HFlip, "nearest_neighbor");
	CopyObj(HPolygonRgn2, &(mpActiveTRegion->m_HTeachPolygonRgn), 1, -1);

	UpdateHistogram(mpActiveTRegion);

	InvalidateRect(&m_ClientRect, FALSE);
}

void CInspectAdminViewDlg::OnMenuAiRetrainRoi()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("변경할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for change.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	mpActiveTRegion->mbAiRetrainFlag = !mpActiveTRegion->mbAiRetrainFlag;

	InvalidateRect(&m_ClientRect, FALSE);
}

void CInspectAdminViewDlg::OnMenuVmAutoTrainCheckAll()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("영상비교 사용하는 탭에 모두 자동학습 설정하겠습니까?");
	else
		strMessageBox.Format("Check auto training option for all tabs that use image comparison?");
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if (nRes != IDYES)
		return;

	int i, j;
	GTRegion* pRegion;
	int iNoInspectROI;

	for (i = 0; i < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); i++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(i);
		if (pRegion == NULL)
			continue;

		if (pRegion->miInspectionType >= INSPECTION_TYPE_FIRST && pRegion->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			for (j = 0; j < MAX_INSPECTION_TAB; j++)
			{
				if (pRegion->m_AlgorithmParam[j].m_bUseImageCompare)
					pRegion->m_AlgorithmParam[j].m_bUseImageCompareAutoTrain = TRUE;
			}
		}
	}

	InvalidateRect(&m_ClientRect, FALSE);
}

void CInspectAdminViewDlg::OnMenuVmAutoTrainUncheckAll()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("영상비교 사용하는 탭에 모두 자동학습 해제하겠습니까?");
	else
		strMessageBox.Format("Uncheck auto training option for all tabs that use image comparison?");
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if (nRes != IDYES)
		return;

	int i, j;
	GTRegion* pRegion;
	int iNoInspectROI;

	for (i = 0; i < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); i++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(i);
		if (pRegion == NULL)
			continue;

		if (pRegion->miInspectionType >= INSPECTION_TYPE_FIRST && pRegion->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			for (j = 0; j < MAX_INSPECTION_TAB; j++)
			{
				if (pRegion->m_AlgorithmParam[j].m_bUseImageCompare)
					pRegion->m_AlgorithmParam[j].m_bUseImageCompareAutoTrain = FALSE;
			}
		}
	}

	InvalidateRect(&m_ClientRect, FALSE);
}

#include "AutoTrainStatusDlg.h"
void CInspectAdminViewDlg::OnMenuVmAutoTrainStatus()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	CAutoTrainStatusDlg dlg;
	dlg.DoModal();

	InvalidateRect(&m_ClientRect, FALSE);
}

void CInspectAdminViewDlg::OnMenuVmAllModelDelete()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("[주의!!!] 모든 영상비교 모델을 삭제하겠습니까?");
	else
		strMessageBox.Format("[Caution!!!] Delete all image compare models?");
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL | MB_ICONWARNING);

	if (nRes != IDYES)
		return;

	int i, j;
	GTRegion* pRegion;
	int iNoInspectROI;

	for (i = 0; i < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); i++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(i);
		if (pRegion == NULL)
			continue;

		if (pRegion->miInspectionType >= INSPECTION_TYPE_FIRST && pRegion->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			for (j = 0; j < MAX_INSPECTION_TAB; j++)
			{
				if (pRegion->m_HVarModelID[j] >= 0)
				{
					ClearVariationModel(pRegion->m_HVarModelID[j]);
					pRegion->m_HVarModelID[j] = -1;

					pRegion->m_iVarModelImageSizeX[j] = -1;
					pRegion->m_iVarModelImageSizeY[j] = -1;

					GenEmptyObj(&(pRegion->m_HVarMeanImage[j]));
					GenEmptyObj(&(pRegion->m_HVarStdevImage[j]));
					pRegion->m_iVarNoTrainSample[j] = 0;
				}
			}
		}
	}

	InvalidateRect(&m_ClientRect, FALSE);
}

void CInspectAdminViewDlg::OnMenuRoiChange()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("변경할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for change.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	int iOrgInspectionType = mpActiveTRegion->miInspectionType;

	InspectionTypeResult inspectionTypeResult;
	if (!ShowInspectionTypeDialog(inspectionTypeResult, true, this))
		return;

	HObject HPolygonRgn;

	if (inspectionTypeResult.genLineRegion)
	{
		HPolygonRgn = mpActiveTRegion->GetROIHRegion(THEAPP.m_pCalDataService);
		Boundary(HPolygonRgn, &HPolygonRgn, "inner");

		mpActiveTRegion->m_bRegionROI = TRUE;
		CopyObj(HPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), 1, -1);
	}

	mpActiveTRegion->miInspectionType = inspectionTypeResult.inspectionType;
	mpActiveTRegion->miFaiRoiIndex = inspectionTypeResult.faiRoiNumber;

	if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_DELETE)
		mpActiveTRegion->SetLineColor(RGB(0, 0, 255));
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_GLOBAL_ALIGN)
		mpActiveTRegion->SetLineColor(RGB(0, 255, 255));
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
		mpActiveTRegion->SetLineColor(RGB(255, 0, 255));
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_DONTCARE)
		mpActiveTRegion->SetLineColor(RGB(128, 0, 255));
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_GENERATE)
		mpActiveTRegion->SetLineColor(RGB(255, 0, 128));
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_MASK)
		mpActiveTRegion->SetLineColor(RGB(0, 0, 255));
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_ADD)
		mpActiveTRegion->SetLineColor(RGB(255, 128, 0));
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_COLOR)
		mpActiveTRegion->SetLineColor(RGB(255, 255, 255));
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE)
		mpActiveTRegion->SetLineColor(RGB(0, 255, 255));
	else if (mpActiveTRegion->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && mpActiveTRegion->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE)
		mpActiveTRegion->SetLineColor(RGB(0, 255, 255));
	else if (mpActiveTRegion->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && mpActiveTRegion->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST)
		mpActiveTRegion->SetLineColor(RGB(255, 128, 0));
	else
		mpActiveTRegion->SetLineColor(RGB(0, 255, 0));

	BOOL bDontCareExist, bAddExist;
	HObject HROIHRegion;

	if ((mpActiveTRegion->miInspectionType == INSPECTION_TYPE_DELETE || mpActiveTRegion->miInspectionType == INSPECTION_TYPE_ADD) == FALSE)
	{
		HROIHRegion = mpActiveTRegion->GetROIHRegion(THEAPP.m_pCalDataService);

		bDontCareExist = FALSE;
		bDontCareExist = CheckDontCareInclusion(HROIHRegion);

		if (bDontCareExist)
		{
			int nRes = 0;
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("새로운 ROI 내에 비검사 ROI가 존재합니다. 비검사 ROI 영역을 제외하시겠습니까?");
			else
				strMessageBox.Format("The new ROI contains an Exclusion ROI. Should the Exclusion ROI area be excluded?");
			nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

			if ((nRes == IDYES))
				GetRegionROIAfterDontCareRemoval(mpActiveTRegion);
		}

		bAddExist = FALSE;
		bAddExist = CheckAddInclusion(HROIHRegion);

		if (bAddExist)
		{
			int nRes = 0;
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("새로운 ROI 내에 추가 ROI가 존재합니다. ROI 영역을 추가하시겠습니까?");
			else
				strMessageBox.Format("The new ROI contains an Inclusion ROI. Should the Inclusion ROI area be included?");
			nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

			if ((nRes == IDYES))
				GetRegionROIAfterAddRemoval(mpActiveTRegion);
		}
	}

	if (iOrgInspectionType != mpActiveTRegion->miInspectionType)
	{
		if (mpActiveTRegion->miInspectionType >= INSPECTION_TYPE_FIRST && mpActiveTRegion->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			int iLastInspectionROIID = 0;
			iLastInspectionROIID = THEAPP.m_pModelDataManager->GetLastPadID(mpActiveTRegion->miTeachImageIndex);
			mpActiveTRegion->miLocalAlignID = iLastInspectionROIID + 1;
		}

		if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
		{
			int iLastLocalAlignID = 0;
			iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(mpActiveTRegion->miTeachImageIndex);
			mpActiveTRegion->miLocalAlignID = iLastLocalAlignID + 1;
		}

		if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_DONTCARE)
		{
			int iLastDontCareID = 0;
			iLastDontCareID = THEAPP.m_pModelDataManager->GetLastDontCareID(mpActiveTRegion->miTeachImageIndex);
			mpActiveTRegion->miDontCareID = iLastDontCareID + 1;
		}

		if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_GENERATE)
		{
			int iLastGenerateID = 0;
			iLastGenerateID = THEAPP.m_pModelDataManager->GetLastGenerateID(mpActiveTRegion->miTeachImageIndex);
			mpActiveTRegion->miGenerateID = iLastGenerateID + 1;
		}

		if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_MASK)
		{
			int iLastMaskID = 0;
			iLastMaskID = THEAPP.m_pModelDataManager->GetLastMaskID(mpActiveTRegion->miTeachImageIndex);
			mpActiveTRegion->miMaskID = iLastMaskID + 1;
		}

		if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_COLOR)
		{
			int iLastColorInfoID = 0;
			iLastColorInfoID = THEAPP.m_pModelDataManager->GetLastColorInfoID(mpActiveTRegion->miTeachImageIndex);
			mpActiveTRegion->miColorInfoID = iLastColorInfoID + 1;
		}
	}

	if (inspectionTypeResult.hasPositionPoint &&
		(((mpActiveTRegion->miInspectionType >= INSPECTION_TYPE_FIRST) && (mpActiveTRegion->miInspectionType <= INSPECTION_TYPE_LAST)) ||
		 (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_MEASURE_POINT) || (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_BLEND_POINT)))	// Point
	{
		mpActiveTRegion->miPositionID = inspectionTypeResult.positionId;
		mpActiveTRegion->miPointID = inspectionTypeResult.pointId;
	}

	UpdateHistogram(mpActiveTRegion);

	InvalidateRect(&m_ClientRect, FALSE);
}


void CInspectAdminViewDlg::OnMenuRoiElipse() //VER 1.1.3.5 add
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("변경할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for change.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}
	HObject HROIHRegion;
	GenEmptyObj(&HROIHRegion);
	GetRegionROIAfterEllipse(mpActiveTRegion);
	UpdateHistogram(mpActiveTRegion);
	InvalidateRect(&m_ClientRect, FALSE);
}


#include "RoiMoveDlg.h"

void CInspectAdminViewDlg::OnMenuAllRoiMove()
{
	CRoiMoveDlg dlg;
	dlg.SetPixelData(TRUE);
	dlg.SetEnableAllROI(TRUE);

	if (dlg.DoModal() == IDOK)
	{
		int iMoveXPxl, iMoveYPxl;

		iMoveXPxl = (int)(dlg.m_dEditMoveX + 0.5);
		iMoveYPxl = (int)(dlg.m_dEditMoveY + 0.5);

		if (iMoveXPxl == 0 && iMoveYPxl == 0)
			return;

		GTRegion* pInspectROIRgn;

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return;

		for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (dlg.m_bCheckApplyVisionROI == FALSE)
			{
				if (pInspectROIRgn->miTeachImageIndex != THEAPP.m_pTabControlDlg->m_iCurrentTab)
					continue;
			}

			pInspectROIRgn->MovePixel((double)iMoveXPxl, (double)iMoveYPxl, FALSE, THEAPP.m_pCalDataService);
		}

		InvalidateRect(&m_ClientRect, FALSE);
	}
}

#include "RoiScaleDlg.h"

void CInspectAdminViewDlg::OnMenuAllRoiScale()
{
	CRoiScaleDlg dlg;

	if (dlg.DoModal() == IDOK)
	{
		if (dlg.m_dEditScale == 1)
			return;

		GTRegion* pInspectROIRgn;

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return;

		for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (dlg.m_bCheckApplyVisionROI == FALSE)
			{
				if (pInspectROIRgn->miTeachImageIndex != THEAPP.m_pTabControlDlg->m_iCurrentTab)
					continue;
			}

			pInspectROIRgn->Scale(dlg.m_dEditScale, FALSE, THEAPP.m_pCalDataService);
		}

		InvalidateRect(&m_ClientRect, FALSE);
	}
}

#include "RoiSizeDlg.h"

void CInspectAdminViewDlg::OnMenuROISizeChangeMM()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("변경할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for change.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	if (mpActiveTRegion->m_bRegionROI)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("Region ROI는 수치로 크기를 조정할 수 없습니다.");
		else
			strMessageBox.Format("Region ROI cannot be resized using numerical value.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	DPOINT dWHPoint;
	mpActiveTRegion->GetWHPointM(&dWHPoint);

	CRoiSizeDlg dlg;
	dlg.SetPixelData(FALSE);

	CString sTemp;
	double dTemp;
	sTemp.Format("%.3lf", dWHPoint.x * 0.001);
	dTemp = atof(sTemp);
	dlg.m_dEditSizeX = dTemp;
	sTemp.Format("%.3lf", dWHPoint.y * 0.001);
	dTemp = atof(sTemp);
	dlg.m_dEditSizeY = dTemp;

	if (dlg.DoModal() == IDOK)
	{
		double dNewSizeXMM, dNewSizeYMM;

		dNewSizeXMM = dlg.m_dEditSizeX;
		dNewSizeYMM = dlg.m_dEditSizeY;

		if (dNewSizeXMM <= 0 || dNewSizeYMM <= 0)
			return;

		dWHPoint.x = dNewSizeXMM * 1000.0;
		dWHPoint.y = dNewSizeYMM * 1000.0;

		mpActiveTRegion->SetWHPointM(dWHPoint);

		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::OnMenuROISizeChangePIXEL()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("변경할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for resize.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	if (mpActiveTRegion->m_bRegionROI)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("Region ROI는 수치로 크기를 조정할 수 없습니다.");
		else
			strMessageBox.Format("Region ROI cannot be resized using numerical value.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	POINT WHPoint;
	mpActiveTRegion->GetWHPoint(&WHPoint, THEAPP.m_pCalDataService);

	CRoiSizeDlg dlg;
	dlg.SetPixelData(TRUE);

	CString sTemp;
	int iTemp;
	sTemp.Format("%d", WHPoint.x);
	iTemp = atoi(sTemp);
	dlg.m_dEditSizeX = iTemp;
	sTemp.Format("%d", WHPoint.y);
	iTemp = atof(sTemp);
	dlg.m_dEditSizeY = iTemp;

	if (dlg.DoModal() == IDOK)
	{
		double dNewSizeXPixel, dNewSizeYPixel;

		dNewSizeXPixel = dlg.m_dEditSizeX;
		dNewSizeYPixel = dlg.m_dEditSizeY;

		if (dNewSizeXPixel <= 0 || dNewSizeYPixel <= 0)
			return;

		WHPoint.x = (int)(dNewSizeXPixel + 0.5);
		WHPoint.y = (int)(dNewSizeYPixel + 0.5);

		POINT LTPoint, RBPoint;
		mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
		RBPoint.x = LTPoint.x + WHPoint.x;
		RBPoint.y = LTPoint.y + WHPoint.y;
		mpActiveTRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);

		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::OnMenuROIMoveChangeMM()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("이동할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for move.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	CRoiMoveDlg dlg;
	dlg.SetPixelData(FALSE);
	dlg.SetEnableAllROI(FALSE);

	if (dlg.DoModal() == IDOK)
	{
		double dMoveOffsetXUm, dMoveOffsetYUm;

		dMoveOffsetXUm = dlg.m_dEditMoveX * 1000.0;
		dMoveOffsetYUm = dlg.m_dEditMoveY * 1000.0;

		if (dMoveOffsetXUm == 0 && dMoveOffsetYUm == 0)
			return;

		mpActiveTRegion->MoveUm(dMoveOffsetXUm, dMoveOffsetYUm, FALSE, THEAPP.m_pCalDataService);

		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::OnMenuROIMoveChangePIXEL()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("이동할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for move.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	CRoiMoveDlg dlg;
	dlg.SetPixelData(TRUE);
	dlg.SetEnableAllROI(FALSE);

	if (dlg.DoModal() == IDOK)
	{
		int iMoveOffsetXPixel, iMoveOffsetYPixel;

		iMoveOffsetXPixel = (int)(dlg.m_dEditMoveX + 0.5);
		iMoveOffsetYPixel = (int)(dlg.m_dEditMoveY + 0.5);

		if (iMoveOffsetXPixel == 0 && iMoveOffsetYPixel == 0)
			return;

		mpActiveTRegion->MovePixel(iMoveOffsetXPixel, iMoveOffsetYPixel, FALSE, THEAPP.m_pCalDataService);

		InvalidateRect(&m_ClientRect, FALSE);
	}
}

#include "RoiIdChangeDlg.h"

void CInspectAdminViewDlg::OnMenuROIIDChange()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("ID를 변경할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for ID change.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	if (((mpActiveTRegion->miInspectionType >= INSPECTION_TYPE_FIRST && mpActiveTRegion->miInspectionType <= INSPECTION_TYPE_LAST) ||
		 mpActiveTRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN || mpActiveTRegion->miInspectionType == INSPECTION_TYPE_DONTCARE ||
		 mpActiveTRegion->miInspectionType == INSPECTION_TYPE_GENERATE || mpActiveTRegion->miInspectionType == INSPECTION_TYPE_MASK ||
		 mpActiveTRegion->miInspectionType == INSPECTION_TYPE_COLOR ||
		 mpActiveTRegion->miInspectionType == INSPECTION_TYPE_MEASURE_POINT) == FALSE)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("검사 ROI, 로컬 얼라인 ROI, 검사 제외 ROI, 검사 영역 생성 ROI, 마스크 ROI, 색상 정보 ROI에서만 ID 변경이 가능합니다.");
		else
			strMessageBox.Format("ID change is only supported for\nInspection ROI, Local Align ROI, Region mask ROI, Region generate ROI, Blob mask ROI, Color info ROI");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	CRoiIdChangeDlg dlg;

	dlg.SetROIType(mpActiveTRegion->miInspectionType);
	if (dlg.DoModal() == IDOK)
	{
		if (mpActiveTRegion->miInspectionType >= INSPECTION_TYPE_FIRST && mpActiveTRegion->miInspectionType <= INSPECTION_TYPE_LAST)
			mpActiveTRegion->miInspectionROIID = dlg.m_iEditId;
		else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
			mpActiveTRegion->miLocalAlignID = dlg.m_iEditId;
		else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_DONTCARE)
			mpActiveTRegion->miDontCareID = dlg.m_iEditId;
		else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_GENERATE)
			mpActiveTRegion->miGenerateID = dlg.m_iEditId;
		else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_MASK)
			mpActiveTRegion->miMaskID = dlg.m_iEditId;
		else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_COLOR)
			mpActiveTRegion->miColorInfoID = dlg.m_iEditId;
		else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_MEASURE_POINT)
			mpActiveTRegion->miPointID = dlg.m_iEditId - 1;

		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::OnMenuLoadImage()
{
	try
	{
		PString PathName;
		CString strFileName;
		CString csTmp, csData;

		char szFilter[] = "All Files (*.*)|*.*|Bitmap Files (*.bmp)|*.bmp|JPEG Files (*.jpg)|*.jpg|IMA Files (*.ima)|*.ima|";

		CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter, NULL);
		FileDialog.m_ofn.lpstrTitle = _T("Load Image File");
		FileDialog.m_ofn.lpstrInitialDir = _T("");

		if (FileDialog.DoModal() == IDOK)
		{
			strFileName = FileDialog.GetFileName();
			PathName = (LPCTSTR)FileDialog.GetPathName();
			int nFindSpot = strFileName.Find(".");
			if (nFindSpot == -1) return;
			CString strFileExt = strFileName.Right(3); // 확장자를 얻어온다.
			strFileExt.MakeUpper(); // 대문자를 만들고..

			HObject HLoadImage;
			ReadImage(&HLoadImage, (LPCTSTR)FileDialog.GetPathName());

			HTuple HNoChannel;
			Hlong lNoChannel;
			int iNoLoadImageTab;

			HObject HTempImageR, HTempImageG, HTempImageB, HTempColorImage;

			CountChannels(HLoadImage, &HNoChannel);
			lNoChannel = HNoChannel.L();

			if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
				iNoLoadImageTab = 0;
			else
				iNoLoadImageTab = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;

			if (lNoChannel == 1)
			{
				CopyImage(HLoadImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][iNoLoadImageTab]));
				CopyImage(HLoadImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][iNoLoadImageTab]));
				CopyImage(HLoadImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][iNoLoadImageTab]));
				Compose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][iNoLoadImageTab],
						 THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][iNoLoadImageTab],
						 THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][iNoLoadImageTab],
						 &HTempColorImage);
				CopyImage(HTempColorImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iNoLoadImageTab]));
				CopyImage(HLoadImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][iNoLoadImageTab]));
				CopyImage(HLoadImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][iNoLoadImageTab]));
				CopyImage(HLoadImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iNoLoadImageTab]));
			}
			else
			{
				CopyImage(HLoadImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iNoLoadImageTab]));
				Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iNoLoadImageTab], &HTempImageR, &HTempImageG, &HTempImageB);
				CopyImage(HTempImageR, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][iNoLoadImageTab]));
				CopyImage(HTempImageG, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][iNoLoadImageTab]));
				CopyImage(HTempImageB, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][iNoLoadImageTab]));
				TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][iNoLoadImageTab], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][iNoLoadImageTab], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][iNoLoadImageTab], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][iNoLoadImageTab]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][iNoLoadImageTab]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iNoLoadImageTab]), "hsi");
				Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iNoLoadImageTab], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iNoLoadImageTab]));
			}

			InvalidateRect(&m_ClientRect, TRUE);
		}
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectAdminViewDlg::OnMenuLoadImage] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("이미지 불러오기 실패.");
		else
			strMessageBox.Format("Image load failure");
		AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
	}
}

void CInspectAdminViewDlg::OnMenuSaveAsTeachingImage()
{
	try
	{
		int nRes = 0;
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("현재 디스플레이된 영상을 저장하시겠습니까?");
		else
			strMessageBox.Format("Save the currently displayed image?");
		nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

		if ((nRes != IDYES))
			return;

		CString strModelName, strFolder, strImageFolder, strImageName;

		strModelName = THEAPP.m_pModelDataManager->m_sModelName;
		strFolder = THEAPP.GetWorkingDirectory() + "\\Model\\";

		strImageFolder.Format("%s%s\\SW\\Vision_N%d\\TeachImage\\", strFolder, strModelName, THEAPP.m_iCurTeachVision + 1);

		THEAPP.m_FileBase.CreateFolder(strImageFolder);

		CString sImageFilePath, TeachImageFileName, strTeachFileFullName;

		for (int i = 0; i < MAX_IMAGE_TAB; i++)
		{
			if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]) == FALSE)
				continue;

			TeachImageFileName.Format("Image_%d", i + 1);
			strTeachFileFullName = strImageFolder + "\\" + TeachImageFileName;

			HTuple HImageType;
			GetImageType(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HImageType);

			if (HImageType == "int4")
				WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i], "ima", 0, HTuple(strTeachFileFullName));
			else
				WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], "jpeg 100", 0, HTuple(strTeachFileFullName));
		}
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [InspectAdminViewDlg::OnMenuSaveAsTeachingImage] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("이미지 저장 실패.");
		else
			strMessageBox.Format("Image save failure");
		AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
	}
}

void CInspectAdminViewDlg::OnMenuDeleteRoiAll()
{
	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("모든 ROI를 삭제하시겠습니까?");
	else
		strMessageBox.Format("Delete all ROIs?");
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	mpLastSelectedRegion = NULL;

	for (int iIndex = (iNoInspectROI - 1); iIndex >= 0; iIndex--)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(pInspectROIRgn, FALSE);
	}

	mpActiveTRegion = NULL;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::OnMenuDeleteRoiTab()
{
	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("현재 영상 탭의 모든 ROI를 삭제하시겠습니까?");
	else
		strMessageBox.Format("Delete all ROIs in current image tab?");
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	mpLastSelectedRegion = NULL;

	for (int iIndex = (iNoInspectROI - 1); iIndex >= 0; iIndex--)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != THEAPP.m_pTabControlDlg->m_iCurrentTab)
			continue;

		THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(pInspectROIRgn, FALSE);
	}

	mpActiveTRegion = NULL;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::OnMenuROIGvDisplay()
{
	if (m_bDispROIGv)
		m_bDispROIGv = FALSE;
	else
		m_bDispROIGv = TRUE;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::OnMenuFAIMeasureDisplay()
{
	if (m_bDispFAIMeasureValue)
		m_bDispFAIMeasureValue = FALSE;
	else
		m_bDispFAIMeasureValue = TRUE;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::OnMenuLinkInfo()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("연결 정보를 확인할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for check the link information.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	int iSearchImageTabNo, iSearchROINo;
	int iNoLinkROI;
	int piImageTabNo[100], piROICenterX[100], piROICenterY[100], piInspectTabNo[100];

	iNoLinkROI = 0;

	iSearchImageTabNo = mpActiveTRegion->miTeachImageIndex;

	if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		iSearchROINo = mpActiveTRegion->miLocalAlignID;

		THEAPP.m_pModelDataManager->GetLinkInfo_LocalAlign(iSearchImageTabNo, iSearchROINo, &iNoLinkROI, piImageTabNo, piROICenterX, piROICenterY, piInspectTabNo);

		if (THEAPP.m_iPGMLanguageSelect == 0)
		{
			if (iNoLinkROI == 0)
				strMessageBox.Format("미사용");
			else
			{
				strMessageBox.Format("%d 개의 검사중인 ROI에서 사용중입니다.\n\n", iNoLinkROI);

				for (int i = 0; i < iNoLinkROI; i++)
					strMessageBox.Format("%s%d. 영상 %d번, ROI 중심(%d, %d), 검사탭 %d번\n", strMessageBox, i + 1, piImageTabNo[i], piROICenterX[i], piROICenterY[i], piInspectTabNo[i]);
			}
		}
		else
		{
			if (iNoLinkROI == 0)
				strMessageBox.Format("Unused");
			else
			{
				strMessageBox.Format("Currently used in %d inspection ROIs.\n\n", iNoLinkROI);

				for (int i = 0; i < iNoLinkROI; i++)
					strMessageBox.Format("%s%d. Image %d, ROI center(%d, %d), Inspection tab %d\n", strMessageBox, i + 1, piImageTabNo[i], piROICenterX[i], piROICenterY[i], piInspectTabNo[i]);
			}
		}
		AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
	}
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_DONTCARE)
	{
		iSearchROINo = mpActiveTRegion->miDontCareID;

		THEAPP.m_pModelDataManager->GetLinkInfo_DontCare(iSearchImageTabNo, iSearchROINo, &iNoLinkROI, piImageTabNo, piROICenterX, piROICenterY, piInspectTabNo);

		if (THEAPP.m_iPGMLanguageSelect == 0)
		{
			if (iNoLinkROI == 0)
				strMessageBox.Format("미사용");
			else
			{
				strMessageBox.Format("%d 개의 검사중인 ROI에서 사용중입니다.\n\n", iNoLinkROI);

				for (int i = 0; i < iNoLinkROI; i++)
					strMessageBox.Format("%s%d. 영상 %d번, ROI 중심(%d, %d), 검사탭 %d번\n", strMessageBox, i + 1, piImageTabNo[i], piROICenterX[i], piROICenterY[i], piInspectTabNo[i]);
			}
		}
		else
		{
			if (iNoLinkROI == 0)
				strMessageBox.Format("Unused");
			else
			{
				strMessageBox.Format("Currently used in %d inspection ROIs.\n\n", iNoLinkROI);

				for (int i = 0; i < iNoLinkROI; i++)
					strMessageBox.Format("%s%d. Image %d, ROI center(%d, %d), Inspection tab %d\n", strMessageBox, i + 1, piImageTabNo[i], piROICenterX[i], piROICenterY[i], piInspectTabNo[i]);
			}
		}
		AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
	}
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_GENERATE)
	{
		iSearchROINo = mpActiveTRegion->miGenerateID;

		THEAPP.m_pModelDataManager->GetLinkInfo_Generate(iSearchImageTabNo, iSearchROINo, &iNoLinkROI, piImageTabNo, piROICenterX, piROICenterY, piInspectTabNo);

		if (THEAPP.m_iPGMLanguageSelect == 0)
		{
			if (iNoLinkROI == 0)
				strMessageBox.Format("미사용");
			else
			{
				strMessageBox.Format("%d 개의 검사중인 ROI에서 사용중입니다.\n\n", iNoLinkROI);

				for (int i = 0; i < iNoLinkROI; i++)
					strMessageBox.Format("%s%d. 영상 %d번, ROI 중심(%d, %d), 검사탭 %d번\n", strMessageBox, i + 1, piImageTabNo[i], piROICenterX[i], piROICenterY[i], piInspectTabNo[i]);
			}
		}
		else
		{
			if (iNoLinkROI == 0)
				strMessageBox.Format("Unused");
			else
			{
				strMessageBox.Format("Currently used in %d inspection ROIs.\n\n", iNoLinkROI);

				for (int i = 0; i < iNoLinkROI; i++)
					strMessageBox.Format("%s%d. Image %d, ROI center(%d, %d), Inspection tab %d\n", strMessageBox, i + 1, piImageTabNo[i], piROICenterX[i], piROICenterY[i], piInspectTabNo[i]);
			}
		}
		AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
	}
	else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_MASK)
	{
		iSearchROINo = mpActiveTRegion->miMaskID;

		THEAPP.m_pModelDataManager->GetLinkInfo_Mask(iSearchImageTabNo, iSearchROINo, &iNoLinkROI, piImageTabNo, piROICenterX, piROICenterY, piInspectTabNo);

		if (THEAPP.m_iPGMLanguageSelect == 0)
		{
			if (iNoLinkROI == 0)
				strMessageBox.Format("미사용");
			else
			{
				strMessageBox.Format("%d 개의 검사중인 ROI에서 사용중입니다.\n\n", iNoLinkROI);

				for (int i = 0; i < iNoLinkROI; i++)
					strMessageBox.Format("%s%d. 영상 %d번, ROI 중심(%d, %d), 검사탭 %d번\n", strMessageBox, i + 1, piImageTabNo[i], piROICenterX[i], piROICenterY[i], piInspectTabNo[i]);
			}
		}
		else
		{
			if (iNoLinkROI == 0)
				strMessageBox.Format("Unused");
			else
			{
				strMessageBox.Format("Currently used in %d inspection ROIs.\n\n", iNoLinkROI);

				for (int i = 0; i < iNoLinkROI; i++)
					strMessageBox.Format("%s%d. Image %d, ROI center(%d, %d), Inspection tab %d\n", strMessageBox, i + 1, piImageTabNo[i], piROICenterX[i], piROICenterY[i], piInspectTabNo[i]);
			}
		}
		AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
	}
}

void CInspectAdminViewDlg::TbDraw()
{
	m_ToolBarState = TS_DRAW;
}

void CInspectAdminViewDlg::TbMove()
{
	m_ToolBarState = TS_MOVE;
}

void CInspectAdminViewDlg::TbSelectPart()
{
	m_ToolBarState = TS_SELECT_PART;
}

void CInspectAdminViewDlg::TbROIRect()
{
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	m_ToolBarState = TS_CREATE_ROI_RECT;

	if (m_caliper_control_flag == 1)
	{
		m_caliper_control_flag = 0;
		m_iNoCaliperPoint = 0;
		mpLineSP = CPoint(0, 0);
		mpLineEP = CPoint(0, 0);
		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::TbROICircle()
{
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	m_ToolBarState = TS_CREATE_ROI_CIRCLE;

	if (m_caliper_control_flag == 1)
	{
		m_caliper_control_flag = 0;
		m_iNoCaliperPoint = 0;
		mpLineSP = CPoint(0, 0);
		mpLineEP = CPoint(0, 0);
		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::TbROIPolygon()
{
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	m_ToolBarState = TS_CREATE_ROI_POLYGON;

	m_bPolygonMode = TRUE;
	m_iPolygonCnt = 0;

	if (m_caliper_control_flag == 1)
	{
		m_caliper_control_flag = 0;
		m_iNoCaliperPoint = 0;
		mpLineSP = CPoint(0, 0);
		mpLineEP = CPoint(0, 0);
		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::TbROICross()
{
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	m_ToolBarState = TS_CREATE_ROI_CROSS;

	if (m_caliper_control_flag == 1)
	{
		m_caliper_control_flag = 0;
		m_iNoCaliperPoint = 0;
		mpLineSP = CPoint(0, 0);
		mpLineEP = CPoint(0, 0);
		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::TbROIDelete()
{
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	int nRes = 0;

	if (mpActiveTRegion)
	{
		if (THEAPP.m_pModelDataManager->m_pInspectionArea != NULL)
		{
			if (THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount() > 0)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessageBox.Format("ROI를 삭제하시겠습니까?");
				else
					strMessageBox.Format("Delete this ROI?");
				nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

				if ((nRes == IDYES))
				{
					if (mpActiveTRegion == mpLastSelectedRegion)
						mpLastSelectedRegion = NULL;

					if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
					{
						if (THEAPP.m_iPGMLanguageSelect == 0)
							strMessageBox.Format("Local Align ROI 삭제 시 알고리즘 연결된 검사 ROI에 영향을 줄 수 있습니다. 삭제하시겠습니까?");
						else
							strMessageBox.Format("Deleting a Local Align ROI may affect its linked inspection ROIs. Continue?");
						nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

						if ((nRes == IDYES))
							THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(mpActiveTRegion, FALSE);
					}
					else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_DONTCARE)
					{
						if (THEAPP.m_iPGMLanguageSelect == 0)
							strMessageBox.Format("검사제외 ROI 삭제 시 알고리즘 연결된 검사 ROI에 영향을 줄 수 있습니다. 삭제하시겠습니까?");
						else
							strMessageBox.Format("Deleting a Region mask ROI may affect its linked inspection ROIs. Continue?");
						nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

						if ((nRes == IDYES))
							THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(mpActiveTRegion, FALSE);
					}
					else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_GENERATE)
					{
						if (THEAPP.m_iPGMLanguageSelect == 0)
							strMessageBox.Format("검사영역 생성 ROI 삭제 시 알고리즘 연결된 검사 ROI에 영향을 줄 수 있습니다. 삭제하시겠습니까?");
						else
							strMessageBox.Format("Deleting a Region generate ROI may affect its linked inspection ROIs. Continue?");
						nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

						if ((nRes == IDYES))
							THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(mpActiveTRegion, FALSE);
					}
					else if (mpActiveTRegion->miInspectionType == INSPECTION_TYPE_MASK)
					{
						if (THEAPP.m_iPGMLanguageSelect == 0)
							strMessageBox.Format("Mask ROI 삭제 시 알고리즘 연결된 검사 ROI에 영향을 줄 수 있습니다. 삭제하시겠습니까?");
						else
							strMessageBox.Format("Deleting a Blob generate ROI may affect its linked inspection ROIs. Continue?");
						nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

						if ((nRes == IDYES))
							THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(mpActiveTRegion, FALSE);
					}
					else
						THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(mpActiveTRegion, FALSE);
				}
			}
		}

		InvalidateRect(&m_ClientRect, FALSE);
		mpActiveTRegion = NULL;
	}
}

void CInspectAdminViewDlg::TbZoomIn()
{
	if (m_bDisplayImage) {

		ClearWindow(m_lWindowID);

		m_dZoomRatio = (int)(m_dZoomRatio * 10) * 0.1;

		if (m_dZoomRatio < 1.0) m_dZoomRatio += 0.1;
		else if (m_dZoomRatio < 2.0) m_dZoomRatio += 0.1;
		else if (m_dZoomRatio < 10.0) m_dZoomRatio += 0.1;
		else m_dZoomRatio = 10.0;

		ViewRectSet();
		ScrollBarSet();

		//		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strDialog.Format("원본 영상 - 배율(%d%%)", (int)(m_dZoomRatio * 100.));
		else
			strDialog.Format("Raw Image - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(strDialog);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

void CInspectAdminViewDlg::TbZoomOut()
{
	if (m_bDisplayImage) {

		ClearWindow(m_lWindowID);

		m_dZoomRatio = (int)(m_dZoomRatio * 10) * 0.1;

		if (m_dZoomRatio > 2.0) m_dZoomRatio -= 0.1;
		else if (m_dZoomRatio > 1.0) m_dZoomRatio -= 0.1;
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

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strDialog.Format("원본 영상 - 배율(%d%%)", (int)(m_dZoomRatio * 100.));
		else
			strDialog.Format("Raw Image - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(strDialog);

		InvalidateRect(&m_ClientRect, TRUE);
	}
}

void CInspectAdminViewDlg::TbFitWidth()
{
	if (m_bDisplayImage) {

		ClearWindow(m_lWindowID);

		m_dZoomRatio = (double)RECTWIDTH(&m_ClientRect) / (double)m_iImageWidth;

		InitViewRect();
		UpdateViewportManager();	// CSKIM
		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strDialog.Format("원본 영상 - 배율(%d%%)", (int)(m_dZoomRatio * 100.));
		else
			strDialog.Format("Raw Image - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(strDialog);

		InvalidateRect(NULL, TRUE);
	}
}

void CInspectAdminViewDlg::TbFitHeight()
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

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strDialog.Format("원본 영상 - 배율(%d%%)", (int)(m_dZoomRatio * 100.));
		else
			strDialog.Format("Raw Image - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(strDialog);

		InvalidateRect(NULL, TRUE);
	}
}

int CInspectAdminViewDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1) return -1;

	HTuple tWnd = (INT)(m_hWnd);

	HTuple HWindowID;
	NewExternWindow(tWnd, 0, 0, m_iImageWidth, m_iImageHeight, &HWindowID);
	m_lWindowID = HWindowID.L();

	return 0;
}

LRESULT CInspectAdminViewDlg::On2DFrameReady(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

LRESULT CInspectAdminViewDlg::OnEventGoToDefect(WPARAM wParam, LPARAM lParam)
{


	InvalidateRect(&m_ClientRect, TRUE);
	return 0;
}

void CInspectAdminViewDlg::LoadScanImage(int nScanNum)
{

}

void CInspectAdminViewDlg::MoveToDefectArea()
{

}

void CInspectAdminViewDlg::RedrawView()
{
	InvalidateRect(NULL, FALSE);
}

void CInspectAdminViewDlg::RedrawRect()
{
	InvalidateRect(&m_ClientRect, FALSE);
}

void CInspectAdminViewDlg::UpdateThresRegion(int Min, int Max)
{
	try
	{
		if (!m_bDisplayImage)
			return;

		if (mpActiveTRegion)
		{
			HObject HReducedImage, HROIRgn;

			if (mpActiveTRegion->m_bRegionROI)
			{
				HROIRgn = mpActiveTRegion->m_HTeachPolygonRgn;
			}
			else
			{
				POINT LTPoint, RBPoint;

				mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

				if (mpActiveTRegion->GetShape() == TRUE)
					GenCircle(&HROIRgn, (LTPoint.y + RBPoint.y) / 2, (LTPoint.x + RBPoint.x) / 2, (RBPoint.y - LTPoint.y) / 2);
				else
					GenRectangle1(&HROIRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
			}

			if (m_iDisplayChannelType == CHANNEL_TYPE_COLOR)
			{
				HObject HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS;
				HObject HGrayImage;
				if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
				{
					Decompose3(m_HTeachingImage[m_iDisplayChannelType][0], &HTempImageR, &HTempImageG, &HTempImageB);
					TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");
				}
				else
				{
					Decompose3(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HTempImageR, &HTempImageG, &HTempImageB);
					TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");
				}

				ReduceDomain(HGrayImage, HROIRgn, &HReducedImage);
			}
			else
			{
				if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
					ReduceDomain(m_HTeachingImage[m_iDisplayChannelType][0], HROIRgn, &HReducedImage);
				else
					ReduceDomain(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HROIRgn, &HReducedImage);
			}

			Threshold(HReducedImage, &m_pThresRgn, Min, Max);

			InvalidateRect(&m_ClientRect, FALSE);
		}
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

		strLog.Format("Halcon Exception [InspectAdminViewDlg::UpdateThresRegion] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}

void CInspectAdminViewDlg::TbLive()
{
#ifdef INLINE_MODE
	if (THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetShowStatus() == TRUE)
		return;

	if (m_bLive == FALSE)
	{
		m_bLive = TRUE;
		m_bOnOff = TRUE;

		if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_iCurTeachVision])
			THEAPP.m_pLightPwmManager[THEAPP.m_iCurTeachVision]->LightOn();

		SetTimer(0, THEAPP.Struct_PreferenceStruct.m_iTeachingLiveGrabPeriod, NULL);
	}
	else
	{
		m_bLive = FALSE;
		KillTimer(0);

		if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_iCurTeachVision])
			THEAPP.m_pLightPwmManager[THEAPP.m_iCurTeachVision]->LightOff();
	}
#endif
}

void CInspectAdminViewDlg::TbDispCrossBar()
{
	if (m_bDispCrossBar == FALSE)
	{
		m_bDispCrossBar = TRUE;
	}
	else
	{
		m_bDispCrossBar = FALSE;
	}

	mpActiveTRegion = NULL;

	InvalidateRect(NULL, TRUE);
}

void CInspectAdminViewDlg::TbFitLineDisplay()
{
	if (m_bDisplayFitLine)
		m_bDisplayFitLine = FALSE;
	else
		m_bDisplayFitLine = TRUE;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::TbInspectAreaDisplay()
{
	if (m_bDisplayInspectArea)
		m_bDisplayInspectArea = FALSE;
	else
		m_bDisplayInspectArea = TRUE;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::TbThresholdAreaDisplay()
{
	if (m_bDisplayThresholdArea)
		m_bDisplayThresholdArea = FALSE;
	else
		m_bDisplayThresholdArea = TRUE;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::TbDefectAreaDisplay()
{
	if (m_bDisplayDefectArea)
		m_bDisplayDefectArea = FALSE;
	else
		m_bDisplayDefectArea = TRUE;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::Tb2pCaliper()
{
	// TODO: Add your command handler code here
	if (m_caliper_control_flag == 0)
	{
		m_ToolBarState = TS_DRAW;
		m_caliper_control_flag = 1;
	}
	else if (m_caliper_control_flag == 1)
	{
		m_caliper_control_flag = 0;
		m_iNoCaliperPoint = 0;
		mpLineSP = CPoint(0, 0);
		mpLineEP = CPoint(0, 0);
		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::TbROILock()
{
	if (m_bROILock)
		m_bROILock = FALSE;
	else
		m_bROILock = TRUE;

	BOOL bROIState;
	bROIState = !m_bROILock;

	GTRegion* pRegion;

	for (int i = 0; i < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); i++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(i);
		if (pRegion == NULL)
			continue;

		if (pRegion->miInspectionType == INSPECTION_TYPE_CROSS_BAR)
			continue;

		pRegion->SetMovable(bROIState, FALSE);
		pRegion->SetSizable(bROIState, FALSE);
	}
}

void CInspectAdminViewDlg::TbAlignDisplay()
{
	if (m_bAlignDisplay)
		m_bAlignDisplay = FALSE;
	else
		m_bAlignDisplay = TRUE;

	GTRegion* pRegion;

	for (int i = 0; i < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); i++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(i);
		if (pRegion == NULL)
			continue;

		if (pRegion->miInspectionType == INSPECTION_TYPE_GLOBAL_ALIGN || pRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE)
		{
			pRegion->SetVisible(m_bAlignDisplay, FALSE);
		}
	}

	UpdateView(TRUE);
}

void CInspectAdminViewDlg::UpdateHistogramImage()
{
	HObject HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS;
	HObject HGrayImage;

	int iCurImageIndex;
	if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
		iCurImageIndex = 0;
	else
		iCurImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iDisplayChannelType][iCurImageIndex]))
	{
		if (m_iDisplayChannelType == CHANNEL_TYPE_COLOR)
		{
			Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iCurImageIndex], &HTempImageR, &HTempImageG, &HTempImageB);
			TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");
			THEAPP.m_pTabControlDlg->m_pThresholdHistogramDlg->SetImage(HGrayImage);
		}
		else
			THEAPP.m_pTabControlDlg->m_pThresholdHistogramDlg->SetImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iDisplayChannelType][iCurImageIndex]);
	}
}

void CInspectAdminViewDlg::TbDisplayColor()
{
	m_iDisplayChannelType = CHANNEL_TYPE_COLOR;

	UpdateHistogramImage();
	UpdateView(FALSE);
}

void CInspectAdminViewDlg::TbDisplayR()
{
	m_iDisplayChannelType = CHANNEL_TYPE_R;

	UpdateHistogramImage();
	UpdateView(FALSE);
}

void CInspectAdminViewDlg::TbDisplayG()
{
	m_iDisplayChannelType = CHANNEL_TYPE_G;

	UpdateHistogramImage();
	UpdateView(FALSE);
}

void CInspectAdminViewDlg::TbDisplayB()
{
	m_iDisplayChannelType = CHANNEL_TYPE_B;

	UpdateHistogramImage();
	UpdateView(FALSE);
}

void CInspectAdminViewDlg::TbDisplayH()
{
	m_iDisplayChannelType = CHANNEL_TYPE_H;

	UpdateHistogramImage();
	UpdateView(FALSE);
}

void CInspectAdminViewDlg::TbDisplayS()
{
	m_iDisplayChannelType = CHANNEL_TYPE_S;

	UpdateHistogramImage();
	UpdateView(FALSE);
}

void CInspectAdminViewDlg::TbDisplayI()
{
	m_iDisplayChannelType = CHANNEL_TYPE_I;

	UpdateHistogramImage();
	UpdateView(FALSE);
}

void CInspectAdminViewDlg::UpdateToolbarStatusTeachMode()
{
	m_bLive = FALSE;
	KillTimer(0);

#ifdef INLINE_MODE
	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_iCurTeachVision])
		THEAPP.m_pLightPwmManager[THEAPP.m_iCurTeachVision]->LightOff();
#endif

	m_bDispCrossBar = FALSE;

	m_bROILock = FALSE;
	TbROILock();

	m_bAlignDisplay = FALSE;
	TbAlignDisplay();

	m_caliper_control_flag = 0;

	m_iNoCaliperPoint = 0;
	mpLineSP = CPoint(0, 0);
	mpLineEP = CPoint(0, 0);

	m_bDispROIGv = FALSE;
}

UINT DFMLiveGrabThread(LPVOID lp)
{
	CString strLog;

	CInspectAdminViewDlg* pInspectAdminViewDlg = (CInspectAdminViewDlg*)lp;

	int iImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;
	if (iImageIndex < 0)
		iImageIndex = 0;

	BOOL bGrabSuccess = FALSE;

	int iStartGrabBufferIndex = 0;

	if (THEAPP.m_pModelDataManager->m_bUseSpecularImage[iImageIndex])
	{
		auto log_time_start = std::chrono::high_resolution_clock::now();
		bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iStartGrabBufferIndex, THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex], THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]);
		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("DFM grab, Time(ms): %d", log_time_ms);
		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

		log_time_start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < SPV_RAW_IMAGE_NUMBER; i++)
		{
			if (THEAPP.m_pCameraManager->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pCameraManager->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
		}
		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("DFM conversion, Time(ms): %d", log_time_ms);
		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));
	}
	else
	{
		pInspectAdminViewDlg->m_bOnOff = TRUE;
		return 0;
	}

	int iWhichSpecularImage, iGrabCycleIndex;

	iGrabCycleIndex = 0;

	if (bGrabSuccess)
	{
		iWhichSpecularImage = THEAPP.m_pModelDataManager->GetWhichSpecularImage(iImageIndex, &iGrabCycleIndex);

		THEAPP.m_pCameraManager->DFMProc(THEAPP.m_iCurTeachVision, 0, THEAPP.m_pCameraManager->GetVisionCamName());			// Grab Buffer 0번에다 RawImage Grab

		THEAPP.m_pCameraManager->DFM_ReadConvertImage_TeachMode(iWhichSpecularImage, FALSE);

		THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
	}

	pInspectAdminViewDlg->m_bOnOff = TRUE;

	return 1;
}

UINT DiffusedLiveGrabThread(LPVOID lp)
{
	CString strLog;

	CInspectAdminViewDlg* pInspectAdminViewDlg = (CInspectAdminViewDlg*)lp;

	int iImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;
	if (iImageIndex < 0)
		iImageIndex = 0;

	BOOL bGrabSuccess = FALSE;

	int iStartGrabBufferIndex = 0;

	if (THEAPP.m_pModelDataManager->m_bUseDiffusedImage[iImageIndex])
	{
		auto log_time_start = std::chrono::high_resolution_clock::now();
		bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iStartGrabBufferIndex, THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex], THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]);
		auto log_time_end = std::chrono::high_resolution_clock::now();
		auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("Diffused grab, Time(ms): %d", log_time_ms);
		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

		log_time_start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < DIFFUSED_RAW_IMAGE_NUMBER; i++)
		{
			if (THEAPP.m_pCameraManager->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pCameraManager->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
		}
		log_time_end = std::chrono::high_resolution_clock::now();
		log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

		strLog.Format("Diffused conversion, Time(ms): %d", log_time_ms);
		THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));
	}
	else
	{
		pInspectAdminViewDlg->m_bOnOff = TRUE;
		return 0;
	}

	int iWhichDiffusedImage, iGrabCycleIndex;

	iGrabCycleIndex = 0;

	if (bGrabSuccess)
	{
		iWhichDiffusedImage = THEAPP.m_pModelDataManager->GetWhichDiffusedImage(iImageIndex, &iGrabCycleIndex);

		THEAPP.m_pCameraManager->DiffusedProc(THEAPP.m_iCurTeachVision, 0, THEAPP.m_pCameraManager->GetVisionCamName());			// Grab Buffer 0번에다 RawImage Grab

		THEAPP.m_pCameraManager->Diffused_ReadConvertImage_TeachMode(iWhichDiffusedImage, FALSE);

		THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
	}

	pInspectAdminViewDlg->m_bOnOff = TRUE;

	return 1;
}

UINT SEQLiveGrabThread(LPVOID lp)
{
	CString strLog;

	CInspectAdminViewDlg* pInspectAdminViewDlg = (CInspectAdminViewDlg*)lp;

	int iImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;
	if (iImageIndex < 0)
		iImageIndex = 0;
	int iStartGrabBufferIndex = iImageIndex;

	BOOL bGrabSuccess = FALSE;

	auto log_time_start = std::chrono::high_resolution_clock::now();

	if (THEAPP.Struct_PreferenceStruct.m_iTeachingCameraLightType == TEACHING_CAMERA_LIGHT_PAGE)
	{
		int iAddrSeq[LIGHTCTRL_PAGE_COUNT];
		for (int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
			iAddrSeq[i] = THEAPP.m_pModelDataManager->m_iLightPageIdx[iImageIndex];
		THEAPP.m_pModelDataManager->m_PageControlData.SetGrabSequence(THEAPP.m_iCurVisionCamType, LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, iAddrSeq, 1);

		bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iStartGrabBufferIndex, LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, 1);

		for (int i = iStartGrabBufferIndex; i < (iStartGrabBufferIndex + 1); i++)
		{
			if (THEAPP.m_pCameraManager->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pCameraManager->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
		}
	}
	else
	{
		iStartGrabBufferIndex = THEAPP.m_pModelDataManager->GetSequenceStartGrabBufferIndex(THEAPP.m_iCurStageIndex, iImageIndex);

		bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iStartGrabBufferIndex, THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex], THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]);

		for (int i = iStartGrabBufferIndex; i < (iStartGrabBufferIndex + THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]); i++)
		{
			if (THEAPP.m_pCameraManager->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pCameraManager->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
		}
	}

	auto log_time_end = std::chrono::high_resolution_clock::now();
	auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

	strLog.Format("Live SEQ grab, Time(ms): %d", log_time_ms);
	THEAPP.m_log_scan->debug("{}", LOG_CSTR(strLog));

	if (bGrabSuccess)
	{
		int iGrabCircularIdx = THEAPP.m_pCameraManager->GetGrabCircularIndex();

		if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)								// 티칭화면이면 ....
		{
			if (THEAPP.m_pCameraManager->GetCropFovUse())
			{
				CropPart(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][0], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]), THEAPP.m_pCameraManager->GetCropFovLTY(), THEAPP.m_pCameraManager->GetCropFovLTX(), THEAPP.m_pCameraManager->GetCropFovSizeX(), THEAPP.m_pCameraManager->GetCropFovSizeY());
				CropPart(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][1], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]), THEAPP.m_pCameraManager->GetCropFovLTY(), THEAPP.m_pCameraManager->GetCropFovLTX(), THEAPP.m_pCameraManager->GetCropFovSizeX(), THEAPP.m_pCameraManager->GetCropFovSizeY());
				CropPart(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][2], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]), THEAPP.m_pCameraManager->GetCropFovLTY(), THEAPP.m_pCameraManager->GetCropFovLTX(), THEAPP.m_pCameraManager->GetCropFovSizeX(), THEAPP.m_pCameraManager->GetCropFovSizeY());

				Compose3(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
				TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B],
							 &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), "hsi");
				Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));
			}
			else
			{
				CopyImage(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][0], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
				CopyImage(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][1], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
				CopyImage(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][2], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));

				Compose3(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
				TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B],
							 &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), "hsi");
				Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));
			}

			THEAPP.m_pInspectAdminViewDlg->UpdateView();
		}
	}

	pInspectAdminViewDlg->m_bOnOff = TRUE;

	return 1;
}

void CInspectAdminViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (THEAPP.m_iModeSwitch != MODE_ADMIN_TEACH_VIEW)
		return;

	switch (nIDEvent)
	{
	case 0:
	{
		int iVisionCamIdx = THEAPP.m_pCameraManager->GetVisionCamName();

		if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[iVisionCamIdx])
		{
			if (m_bLive == TRUE)
			{
				if (m_bOnOff == TRUE)
				{
					m_bOnOff = FALSE;
					THEAPP.m_pDualCameraManager[iVisionCamIdx]->GrabEthernetCamSWTrg(0);
					m_bOnOff = TRUE;
				}
			}
		}
		else
		{
			if (THEAPP.m_pModelDataManager->m_bUseSpecularImage[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1])	// Specular
			{
				if (m_bLive == TRUE)
				{
					if (m_bOnOff == TRUE)
					{
						m_bOnOff = FALSE;
						AfxBeginThread(DFMLiveGrabThread, this);
					}
				}
			}
			else if (THEAPP.m_pModelDataManager->m_bUseDiffusedImage[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1])	// Diffused
			{
				if (m_bLive == TRUE)
				{
					if (m_bOnOff == TRUE)
					{
						m_bOnOff = FALSE;
						AfxBeginThread(DiffusedLiveGrabThread, this);
					}
				}
			}
			else
			{
				if (m_bLive == TRUE)
				{
					if (m_bOnOff == TRUE)
					{
						m_bOnOff = FALSE;
						AfxBeginThread(SEQLiveGrabThread, this);
					}
				}
			}
		}

		break;
	}
	}

	CDialog::OnTimer(nIDEvent);
}

void CInspectAdminViewDlg::UpdateView(BOOL bUpdate)
{
	InvalidateRect(&m_ClientRect, bUpdate);
}

void CInspectAdminViewDlg::ClearUpdateView(BOOL bUpdate)
{
	ClearWindow(m_lWindowID);
	InvalidateRect(&m_ClientRect, bUpdate);
}

void CInspectAdminViewDlg::DrawArrow(HTuple pDC)
{
	SetColor(m_lWindowID, "red");
	SetDraw(m_lWindowID, "fill");

	double dArrowSize;
	dArrowSize = 1.0 / m_ViewportManager.mdZoomRatio;
	if (dArrowSize < 0.645)
		dArrowSize = 0.645;
	else if (dArrowSize < 0.87)
		dArrowSize = 0.87;

	DispArrow(m_lWindowID, mpLineSP.y, mpLineSP.x, mpLineEP.y, mpLineEP.x, dArrowSize);
	DispArrow(m_lWindowID, mpLineEP.y, mpLineEP.x, mpLineSP.y, mpLineSP.x, dArrowSize);

	HTuple HAngle;
	double dAngle;
	AngleLl(mpLineSP.y, mpLineSP.x, mpLineSP.y, mpLineEP.x, mpLineSP.y, mpLineSP.x, mpLineEP.y, mpLineEP.x, &HAngle);
	TupleDeg(HAngle, &HAngle);
	dAngle = HAngle[0].D();

	DPOINT RealSP, RealEP;

	int iCameraPixelSize;
	double dUmPerPixel = THEAPP.m_pCalDataService->GetPixelSize();
	DPOINT dMPoint;
	double dMx;

	dMx = (double)mpLineSP.x;
	THEAPP.m_pCalDataService->IPtoRP(dMx, 0.0, &(dMPoint.x), &(dMPoint.y));
	RealSP.x = dMPoint.x;
	RealSP.y = (double)mpLineSP.y * dUmPerPixel;
	//	
	dMx = (double)mpLineEP.x;
	THEAPP.m_pCalDataService->IPtoRP(dMx, 0.0, &(dMPoint.x), &(dMPoint.y));
	RealEP.x = dMPoint.x;
	RealEP.y = (double)mpLineEP.y * dUmPerPixel;

	double dPixelLength = edLineLength(mpLineSP.x, mpLineSP.y, mpLineEP.x, mpLineEP.y);

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("길이: %.1lf um, %d 픽셀, %.2lf 도", edLineLength(RealSP.x, RealSP.y, RealEP.x, RealEP.y), (int)(dPixelLength + 0.5), dAngle);
	else
		strDialog.Format("Length: %.1lf um, %d pixel, %.2lf Deg", edLineLength(RealSP.x, RealSP.y, RealEP.x, RealEP.y), (int)(dPixelLength + 0.5), dAngle);

	HalconCpp::SetFont(m_lWindowID, "Arial-Normal-20");
	SetRgb(m_lWindowID, 255, 0, 0);
	SetTposition(m_lWindowID, m_ViewportManager.miStartYPos + 5, m_ViewportManager.miStartXPos + 5);
	WriteString(m_lWindowID, (HTuple)strDialog);
}

double CInspectAdminViewDlg::edLineLength(double sx, double sy, double ex, double ey)
{
	double num = ((ex - sx) * (ex - sx)) + ((ey - sy) * (ey - sy));
	return sqrt(num);
}

void CInspectAdminViewDlg::StartLive()
{
	if (m_bLive == FALSE)
	{
#ifdef INLINE_MODE
		m_bLive = TRUE;
		if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_iCurTeachVision])
			THEAPP.m_pLightPwmManager[THEAPP.m_iCurTeachVision]->LightOn();

		SetTimer(0, THEAPP.Struct_PreferenceStruct.m_iTeachingLiveGrabPeriod, NULL);
#endif
	}
}

void CInspectAdminViewDlg::StopLive()
{
	m_bLive = FALSE;
	KillTimer(0);

#ifdef INLINE_MODE
	if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[THEAPP.m_iCurTeachVision])
		THEAPP.m_pLightPwmManager[THEAPP.m_iCurTeachVision]->LightOff();
#endif
}

void CInspectAdminViewDlg::ClearAllSelectRegion()
{
	if (THEAPP.m_pModelDataManager->m_pInspectionArea)
		THEAPP.m_pModelDataManager->m_pInspectionArea->ClearAllSelect();
}

void CInspectAdminViewDlg::ClearAllLastSelectRegion()
{
	if (THEAPP.m_pModelDataManager->m_pInspectionArea)
		THEAPP.m_pModelDataManager->m_pInspectionArea->ClearAllLastSelected();
}

void CInspectAdminViewDlg::SaveInspectTabOverLastSelectRegion()
{
	if (mpLastSelectedRegion)
	{
		if ((mpLastSelectedRegion->miInspectionType > INSPECTION_TYPE_UNDEFINED && mpLastSelectedRegion->miInspectionType < INSPECTION_TYPE_DELETE) ||
			mpLastSelectedRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN || mpLastSelectedRegion->miInspectionType == INSPECTION_TYPE_DONTCARE ||
			mpLastSelectedRegion->miInspectionType == INSPECTION_TYPE_GENERATE || mpLastSelectedRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE ||
			mpLastSelectedRegion->miInspectionType == INSPECTION_TYPE_MASK ||
			(mpLastSelectedRegion->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && mpLastSelectedRegion->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) ||
			mpLastSelectedRegion->miInspectionType == INSPECTION_TYPE_LIGHT || mpLastSelectedRegion->miInspectionType == INSPECTION_TYPE_FOCUS || mpLastSelectedRegion->miInspectionType == INSPECTION_TYPE_COLOR ||
			(mpLastSelectedRegion->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && mpLastSelectedRegion->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST))
		{
			delegateCurROITabParamSave.FireEvent((WPARAM)THEAPP.m_pTabControlDlg->m_iCurrentTab);
		}
	}
}

void CInspectAdminViewDlg::ClearActiveTRegion()
{
	mpActiveTRegion = NULL;
}

void CInspectAdminViewDlg::DisplayCross(HDC hDestDC, GViewportManager* pVManager, double dXPos, double dYPos, COLORREF Color, long lRadiusPxl)
{
	POINT CPoint, sPoint;
	POINT Left, Right, Top, Bottom;

	CPoint.x = (long)(dXPos + 0.5);
	CPoint.y = (long)(dYPos + 0.5);

	Left.x = CPoint.x - lRadiusPxl;
	Left.y = CPoint.y;
	Right.x = CPoint.x + lRadiusPxl;
	Right.y = CPoint.y;
	Top.x = CPoint.x;
	Top.y = CPoint.y - lRadiusPxl;
	Bottom.x = CPoint.x;
	Bottom.y = CPoint.y + lRadiusPxl;

	pVManager->IPtoVP(&Left, 1);
	pVManager->IPtoVP(&Right, 1);
	pVManager->IPtoVP(&Top, 1);
	pVManager->IPtoVP(&Bottom, 1);

	HPEN hOldPen, hPen;
	hPen = CreatePen(PS_SOLID, 1, Color);
	hOldPen = (HPEN)SelectObject(hDestDC, hPen);

	MoveToEx(hDestDC, Left.x, Left.y, &sPoint);
	LineTo(hDestDC, Right.x, Right.y);

	MoveToEx(hDestDC, Top.x, Top.y, &sPoint);
	LineTo(hDestDC, Bottom.x, Bottom.y);

	SelectObject(hDestDC, hOldPen);
	DeleteObject(hPen);
}

void CInspectAdminViewDlg::AddPolygonROI()
{
	if (m_iPolygonCnt <= 2)
		return;

	InspectionTypeResult inspectionTypeResult;
	if (!ShowInspectionTypeDialog(inspectionTypeResult, true, this))
		return;

	HTuple HRows, HCols;

	TupleGenConst(0, 0, &HRows);
	TupleGenConst(0, 0, &HCols);

	for (int i = 0; i < m_iPolygonCnt; i++)
	{
		TupleConcat(HRows, m_PolygonPt[i].y, &HRows);
		TupleConcat(HCols, m_PolygonPt[i].x, &HCols);
	}

	HObject HPolygonRgn;

	if (inspectionTypeResult.genLineRegion)
		GenRegionPolygon(&HPolygonRgn, HRows, HCols);
	else
		GenRegionPolygonFilled(&HPolygonRgn, HRows, HCols);

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	SmallestRectangle1(HPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

	lRow1 = HlRow1.L();
	lCol1 = HlCol1.L();
	lRow2 = HlRow2.L();
	lCol2 = HlCol2.L();

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	GTRegion* pPolygonRgn;

	pPolygonRgn = new GTRegion;
	pPolygonRgn->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pPolygonRgn->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
	pPolygonRgn->SetVisible(TRUE, FALSE);
	pPolygonRgn->SetSelectable(TRUE, FALSE);
	pPolygonRgn->SetMovable(TRUE, FALSE);
	pPolygonRgn->SetSizable(FALSE, FALSE);

	pPolygonRgn->miTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;

	pPolygonRgn->miInspectionType = inspectionTypeResult.inspectionType;
	pPolygonRgn->miFaiRoiIndex = inspectionTypeResult.faiRoiNumber;

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_DELETE)
		pPolygonRgn->SetLineColor(RGB(0, 0, 255));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_GLOBAL_ALIGN)
		pPolygonRgn->SetLineColor(RGB(0, 255, 255));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
		pPolygonRgn->SetLineColor(RGB(255, 0, 255));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_DONTCARE)
		pPolygonRgn->SetLineColor(RGB(128, 0, 255));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_GENERATE)
		pPolygonRgn->SetLineColor(RGB(255, 0, 128));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_MASK)
		pPolygonRgn->SetLineColor(RGB(0, 0, 255));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_ADD)
		pPolygonRgn->SetLineColor(RGB(255, 128, 0));
	else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE)
		pPolygonRgn->SetLineColor(RGB(0, 255, 255));
	else if (pPolygonRgn->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && pPolygonRgn->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE)
		pPolygonRgn->SetLineColor(RGB(0, 255, 255));
	else if (pPolygonRgn->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && pPolygonRgn->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST)
		pPolygonRgn->SetLineColor(RGB(255, 128, 0));
	else
		pPolygonRgn->SetLineColor(RGB(0, 255, 0));

	pPolygonRgn->m_bRegionROI = TRUE;
	CopyObj(HPolygonRgn, &(pPolygonRgn->m_HTeachPolygonRgn), 1, -1);

	HObject HROIHRegion;
	BOOL bDontCareExist, bAddExist;

	if ((pPolygonRgn->miInspectionType == INSPECTION_TYPE_DELETE || pPolygonRgn->miInspectionType == INSPECTION_TYPE_ADD) == FALSE)
	{
		HROIHRegion = pPolygonRgn->GetROIHRegion(THEAPP.m_pCalDataService);

		bDontCareExist = FALSE;
		bDontCareExist = CheckDontCareInclusion(HROIHRegion);

		if (bDontCareExist)
		{
			int nRes = 0;
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("새로운 ROI 내에 비검사 ROI가 존재합니다. 비검사 ROI 영역을 제외하시겠습니까?");
			else
				strMessageBox.Format("The new ROI contains an Exclusion ROI. Should the Exclusion ROI area be excluded?");
			nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

			if ((nRes == IDYES))
			{
				GetRegionROIAfterDontCareRemoval(pPolygonRgn);
			}
		}

		bAddExist = FALSE;
		bAddExist = CheckAddInclusion(HROIHRegion);

		if (bAddExist)
		{
			int nRes = 0;
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("새로운 ROI 내에 추가 ROI가 존재합니다. ROI 영역을 추가하시겠습니까?");
			else
				strMessageBox.Format("The new ROI contains an Inclusion ROI. Should the Inclusion ROI area be included?");
			nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

			if ((nRes == IDYES))
			{
				GetRegionROIAfterAddRemoval(pPolygonRgn);
			}
		}
	}

	if (pPolygonRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pPolygonRgn->miInspectionType <= INSPECTION_TYPE_LAST)
	{
		int iLastInspectionROIID = 0;
		iLastInspectionROIID = THEAPP.m_pModelDataManager->GetLastPadID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miInspectionROIID = iLastInspectionROIID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		int iLastLocalAlignID = 0;
		iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miLocalAlignID = iLastLocalAlignID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_DONTCARE)
	{
		int iLastDontCareID = 0;
		iLastDontCareID = THEAPP.m_pModelDataManager->GetLastDontCareID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miDontCareID = iLastDontCareID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_GENERATE)
	{
		int iLastGenerateID = 0;
		iLastGenerateID = THEAPP.m_pModelDataManager->GetLastGenerateID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miGenerateID = iLastGenerateID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_MASK)
	{
		int iLastMaskID = 0;
		iLastMaskID = THEAPP.m_pModelDataManager->GetLastMaskID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miMaskID = iLastMaskID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_COLOR)
	{
		int iLastColorInfoID = 0;
		iLastColorInfoID = THEAPP.m_pModelDataManager->GetLastColorInfoID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->miColorInfoID = iLastColorInfoID + 1;
	}

	if (inspectionTypeResult.hasPositionPoint &&
		(((pPolygonRgn->miInspectionType >= INSPECTION_TYPE_FIRST) && (pPolygonRgn->miInspectionType <= INSPECTION_TYPE_LAST)) ||
		 (pPolygonRgn->miInspectionType == INSPECTION_TYPE_MEASURE_POINT) || (pPolygonRgn->miInspectionType == INSPECTION_TYPE_BLEND_POINT)))	// Point
	{
		pPolygonRgn->miPositionID = inspectionTypeResult.positionId;
		pPolygonRgn->miPointID = inspectionTypeResult.pointId;
	}

	THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pPolygonRgn);

	UpdateHistogram(pPolygonRgn);
}

void CInspectAdminViewDlg::UpdateHistogram(GTRegion* pActiveRegion)
{
	if (pActiveRegion)
	{
		HObject HReducedImage, HROIRgn;

		if (pActiveRegion->m_bRegionROI)
		{
			HROIRgn = pActiveRegion->m_HTeachPolygonRgn;
		}
		else
		{
			POINT LTPoint, RBPoint;
			long lTemp;

			pActiveRegion->ArrangePoints();

			pActiveRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
			pActiveRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

			if (pActiveRegion->GetShape() == TRUE)
				GenCircle(&HROIRgn, (LTPoint.y + RBPoint.y) / 2, (LTPoint.x + RBPoint.x) / 2, (RBPoint.y - LTPoint.y) / 2);
			else
			{
				if (LTPoint.x > RBPoint.x)
				{
					lTemp = LTPoint.x;
					RBPoint.x = LTPoint.x;
					LTPoint.x = lTemp;
				}

				if (LTPoint.y > RBPoint.y)
				{
					lTemp = LTPoint.y;
					RBPoint.y = LTPoint.y;
					LTPoint.y = lTemp;
				}

				GenRectangle1(&HROIRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
			}
		}

		if (m_iDisplayChannelType == CHANNEL_TYPE_COLOR)
		{
			HObject HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS;
			HObject HGrayImage;
			if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
			{
				Decompose3(m_HTeachingImage[m_iDisplayChannelType][0], &HTempImageR, &HTempImageG, &HTempImageB);
				TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");
			}
			else
			{
				Decompose3(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HTempImageR, &HTempImageG, &HTempImageB);
				TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");
			}

			ReduceDomain(HGrayImage, HROIRgn, &HReducedImage);
		}
		else
		{
			if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
				ReduceDomain(m_HTeachingImage[m_iDisplayChannelType][0], HROIRgn, &HReducedImage);
			else
				ReduceDomain(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HROIRgn, &HReducedImage);
		}

		THEAPP.m_pTabControlDlg->m_pThresholdHistogramDlg->SetImage(HReducedImage);
	}
}

void CInspectAdminViewDlg::AddRegionROI(int iThresMin, int iThresMax, BOOL bCheckMinBlob, BOOL bCheckMaxArea, BOOL bCheckFillUp, BOOL bCheckOpeningClosingInverse,
										BOOL bCheckOpening, BOOL bCheckClosing, BOOL bCheckConvex, BOOL bCheckRect, BOOL bCheckDifference, BOOL bCheckBoundary, BOOL bCheckDilation, int iEditMinBlobSize, int iEditOpeningSize, int iEditClosingSize, int iEditBoundaryDilationSize, int iEditDilationSize)
{
	try
	{
		if (!m_bDisplayImage)
			return;

		if (mpActiveTRegion)
		{
			InspectionTypeResult inspectionTypeResult;
			if (!ShowInspectionTypeDialog(inspectionTypeResult, true, this))
				return;

			HObject HReducedImage, HROIRgn, HThresRgn;

			if (mpActiveTRegion->m_bRegionROI)
			{
				HROIRgn = mpActiveTRegion->m_HTeachPolygonRgn;
			}
			else
			{
				POINT LTPoint, RBPoint;

				mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

				if (mpActiveTRegion->GetShape() == TRUE)
					GenCircle(&HROIRgn, (LTPoint.y + RBPoint.y) / 2, (LTPoint.x + RBPoint.x) / 2, (RBPoint.y - LTPoint.y) / 2);
				else
					GenRectangle1(&HROIRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
			}

			if (m_iDisplayChannelType == CHANNEL_TYPE_COLOR)
			{
				HObject HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS;
				HObject HGrayImage;
				if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
				{
					Decompose3(m_HTeachingImage[m_iDisplayChannelType][0], &HTempImageR, &HTempImageG, &HTempImageB);
					TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");
				}
				else
				{
					Decompose3(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HTempImageR, &HTempImageG, &HTempImageB);
					TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");
				}

				ReduceDomain(HGrayImage, HROIRgn, &HReducedImage);
			}
			else
			{
				if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
					ReduceDomain(m_HTeachingImage[m_iDisplayChannelType][0], HROIRgn, &HReducedImage);
				else
					ReduceDomain(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HROIRgn, &HReducedImage);
			}

			Threshold(HReducedImage, &HThresRgn, iThresMin, iThresMax);

			HObject HSelectedRgn, HHoleRgn, HFillRgn, HBoundaryRgn;
			Hlong lNoBlob;

			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn))
			{
				if (bCheckMinBlob)
				{
					Connection(HThresRgn, &HThresRgn);
					SelectShape(HThresRgn, &HThresRgn, "area", "and", iEditMinBlobSize, 9999999999999);
					Union1(HThresRgn, &HThresRgn);
				}

				if (bCheckMaxArea)
				{
					Connection(HThresRgn, &HThresRgn);
					SelectShapeStd(HThresRgn, &HThresRgn, HTuple("max_area"), HTuple(70));
				}

				if (bCheckFillUp)
				{
					FillUp(HThresRgn, &HThresRgn);
				}

				if (bCheckOpeningClosingInverse)
				{
					if (bCheckClosing)
						ClosingCircle(HThresRgn, &HThresRgn, (double)iEditClosingSize + 0.5);
					if (bCheckOpening)
						OpeningCircle(HThresRgn, &HThresRgn, (double)iEditOpeningSize + 0.5);
				}
				else
				{
					if (bCheckOpening)
						OpeningCircle(HThresRgn, &HThresRgn, (double)iEditOpeningSize + 0.5);
					if (bCheckClosing)
						ClosingCircle(HThresRgn, &HThresRgn, (double)iEditClosingSize + 0.5);
				}

				if (bCheckConvex)
				{
					if (bCheckMinBlob)
					{
						Connection(HThresRgn, &HThresRgn);

						HTuple HlNoBlob;
						CountObj(HThresRgn, &HlNoBlob);
						lNoBlob = HlNoBlob.L();

						GenEmptyObj(&HHoleRgn);

						for (int i = 1; i <= lNoBlob; i++)
						{
							SelectObj(HThresRgn, &HSelectedRgn, i);

							FillUp(HSelectedRgn, &HFillRgn);
							Difference(HFillRgn, HSelectedRgn, &HSelectedRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn))
							{
								ShapeTrans(HSelectedRgn, &HSelectedRgn, "convex");
								ConcatObj(HHoleRgn, HSelectedRgn, &HHoleRgn);
							}
						}

						Union1(HHoleRgn, &HHoleRgn);

						ShapeTrans(HThresRgn, &HThresRgn, "convex");
						Union1(HThresRgn, &HThresRgn);

						Difference(HThresRgn, HHoleRgn, &HThresRgn);
					}
					else
						ShapeTrans(HThresRgn, &HThresRgn, "convex");
				}

				if (bCheckRect)
				{
					if (bCheckMinBlob)
					{
						Connection(HThresRgn, &HThresRgn);
						ShapeTrans(HThresRgn, &HThresRgn, "rectangle1");
						Union1(HThresRgn, &HThresRgn);
					}
					else
					{
						ShapeTrans(HThresRgn, &HThresRgn, "rectangle1");
					}
				}

				if (bCheckDifference)
				{
					Difference(HROIRgn, HThresRgn, &HThresRgn);
				}

				if (bCheckBoundary)
				{
					Connection(HThresRgn, &HThresRgn);

					HTuple HlNoBlob;
					CountObj(HThresRgn, &HlNoBlob);
					lNoBlob = HlNoBlob.L();

					GenEmptyObj(&HBoundaryRgn);

					for (int i = 1; i <= lNoBlob; i++)
					{
						SelectObj(HThresRgn, &HSelectedRgn, i);

						FillUp(HSelectedRgn, &HSelectedRgn);
						Boundary(HSelectedRgn, &HSelectedRgn, "inner");
						DilationCircle(HSelectedRgn, &HSelectedRgn, (double)iEditBoundaryDilationSize + 0.5);

						if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn))
						{
							ConcatObj(HBoundaryRgn, HSelectedRgn, &HBoundaryRgn);
						}
					}

					Union1(HBoundaryRgn, &HThresRgn);
				}

				if (bCheckDilation)
				{
					if (iEditDilationSize >= 0)
					{
						DilationCircle(HThresRgn, &HThresRgn, (double)iEditDilationSize + 0.5);
					}
					else
					{
						int iMargin = abs(iEditDilationSize);
						ErosionCircle(HThresRgn, &HThresRgn, (double)iMargin + 0.5);
					}
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn))
			{
				if (THEAPP.m_pModelDataManager->m_pInspectionArea != NULL)
				{
					if (THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount() > 0)
					{
						THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(mpActiveTRegion, FALSE);
						mpActiveTRegion = NULL;
						mpLastSelectedRegion = NULL;
					}

					POINT LTPoint, RBPoint;
					Hlong lRow1, lRow2, lCol1, lCol2;
					HTuple HlRow1, HlRow2, HlCol1, HlCol2;

					SmallestRectangle1(HThresRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

					lRow1 = HlRow1.L();
					lCol1 = HlCol1.L();
					lRow2 = HlRow2.L();
					lCol2 = HlCol2.L();

					LTPoint.x = lCol1;
					LTPoint.y = lRow1;
					RBPoint.x = lCol2;
					RBPoint.y = lRow2;

					GTRegion* pPolygonRgn;

					pPolygonRgn = new GTRegion;
					pPolygonRgn->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
					pPolygonRgn->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
					pPolygonRgn->SetVisible(TRUE, FALSE);
					pPolygonRgn->SetSelectable(TRUE, FALSE);
					pPolygonRgn->SetMovable(TRUE, FALSE);
					pPolygonRgn->SetSizable(FALSE, FALSE);

					pPolygonRgn->miTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;

					pPolygonRgn->miInspectionType = inspectionTypeResult.inspectionType;
					pPolygonRgn->miFaiRoiIndex = inspectionTypeResult.faiRoiNumber;

					if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_DELETE)
						pPolygonRgn->SetLineColor(RGB(0, 0, 255));
					else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_GLOBAL_ALIGN)
						pPolygonRgn->SetLineColor(RGB(0, 255, 255));
					else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
						pPolygonRgn->SetLineColor(RGB(255, 0, 255));
					else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_DONTCARE)
						pPolygonRgn->SetLineColor(RGB(128, 0, 255));
					else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_GENERATE)
						pPolygonRgn->SetLineColor(RGB(255, 0, 128));
					else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_MASK)
						pPolygonRgn->SetLineColor(RGB(0, 0, 255));
					else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_ADD)
						pPolygonRgn->SetLineColor(RGB(255, 128, 0));
					else if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE)
						pPolygonRgn->SetLineColor(RGB(0, 255, 255));
					else if (pPolygonRgn->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && pPolygonRgn->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE)
						pPolygonRgn->SetLineColor(RGB(0, 255, 255));
					else if (pPolygonRgn->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && pPolygonRgn->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST)
						pPolygonRgn->SetLineColor(RGB(255, 128, 0));
					else
						pPolygonRgn->SetLineColor(RGB(0, 255, 0));

					pPolygonRgn->m_bRegionROI = TRUE;

					if (inspectionTypeResult.genLineRegion)
						Boundary(HThresRgn, &HThresRgn, "inner_filled");

					CopyObj(HThresRgn, &(pPolygonRgn->m_HTeachPolygonRgn), 1, -1);

					HObject HROIHRegion;
					BOOL bDontCareExist, bAddExist;

					if ((pPolygonRgn->miInspectionType == INSPECTION_TYPE_DELETE || pPolygonRgn->miInspectionType == INSPECTION_TYPE_ADD) == FALSE)
					{
						HROIHRegion = pPolygonRgn->GetROIHRegion(THEAPP.m_pCalDataService);

						bDontCareExist = FALSE;
						bDontCareExist = CheckDontCareInclusion(HROIHRegion);

						if (bDontCareExist)
						{
							int nRes = 0;
							if (THEAPP.m_iPGMLanguageSelect == 0)
								strMessageBox.Format("새로운 ROI 내에 비검사 ROI가 존재합니다. 비검사 ROI 영역을 제외하시겠습니까?");
							else
								strMessageBox.Format("The new ROI contains an Exclusion ROI. Should the Exclusion ROI area be excluded?");
							nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

							if ((nRes == IDYES))
							{
								GetRegionROIAfterDontCareRemoval(pPolygonRgn);
							}
						}

						bAddExist = FALSE;
						bAddExist = CheckAddInclusion(HROIHRegion);

						if (bAddExist)
						{
							int nRes = 0;
							if (THEAPP.m_iPGMLanguageSelect == 0)
								strMessageBox.Format("새로운 ROI 내에 추가 ROI가 존재합니다. ROI 영역을 추가하시겠습니까?");
							else
								strMessageBox.Format("The new ROI contains an Inclusion ROI. Should the Inclusion ROI area be included?");
							nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

							if ((nRes == IDYES))
							{
								GetRegionROIAfterAddRemoval(pPolygonRgn);
							}
						}
					}

					if (pPolygonRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pPolygonRgn->miInspectionType <= INSPECTION_TYPE_LAST)
					{
						int iLastInspectionROIID = 0;
						iLastInspectionROIID = THEAPP.m_pModelDataManager->GetLastPadID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->miInspectionROIID = iLastInspectionROIID + 1;
					}

					if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
					{
						int iLastLocalAlignID = 0;
						iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->miLocalAlignID = iLastLocalAlignID + 1;
					}

					if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_DONTCARE)
					{
						int iLastDontCareID = 0;
						iLastDontCareID = THEAPP.m_pModelDataManager->GetLastDontCareID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->miDontCareID = iLastDontCareID + 1;
					}

					if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_GENERATE)
					{
						int iLastGenerateID = 0;
						iLastGenerateID = THEAPP.m_pModelDataManager->GetLastGenerateID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->miGenerateID = iLastGenerateID + 1;
					}

					if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_MASK)
					{
						int iLastMaskID = 0;
						iLastMaskID = THEAPP.m_pModelDataManager->GetLastMaskID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->miMaskID = iLastMaskID + 1;
					}

					if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_COLOR)
					{
						int iLastColorInfoID = 0;
						iLastColorInfoID = THEAPP.m_pModelDataManager->GetLastColorInfoID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->miColorInfoID = iLastColorInfoID + 1;
					}

					if (inspectionTypeResult.hasPositionPoint &&
						(((pPolygonRgn->miInspectionType >= INSPECTION_TYPE_FIRST) && (pPolygonRgn->miInspectionType <= INSPECTION_TYPE_LAST)) ||
						 (pPolygonRgn->miInspectionType == INSPECTION_TYPE_MEASURE_POINT) || (pPolygonRgn->miInspectionType == INSPECTION_TYPE_BLEND_POINT)))	// Point
					{
						pPolygonRgn->miPositionID = inspectionTypeResult.positionId;
						pPolygonRgn->miPointID = inspectionTypeResult.pointId;
					}

					THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pPolygonRgn);

					UpdateHistogram(pPolygonRgn);

					GenEmptyObj(&m_pThresRgn);
				}
			}

			InvalidateRect(&m_ClientRect, FALSE);
		}
	}
	catch (HException& except)
	{
		strLog.Format("Halcon Exception [InspectAdminViewDlg::AddRegionROI]");
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}

void CInspectAdminViewDlg::ShowRegionROI(int iThresMin, int iThresMax, BOOL bCheckMinBlob, BOOL bCheckMaxArea, BOOL bCheckFillUp, BOOL bCheckOpeningClosingInverse,
										 BOOL bCheckOpening, BOOL bCheckClosing, BOOL bCheckConvex, BOOL bCheckRect, BOOL bCheckDifference, BOOL bCheckBoundary, BOOL bCheckDilation, int iEditMinBlobSize, int iEditOpeningSize, int iEditClosingSize, int iEditBoundaryDilationSize, int iEditDilationSize)
{
	try
	{
		if (!m_bDisplayImage)
			return;

		if (mpActiveTRegion)
		{
			HObject HReducedImage, HROIRgn, HThresRgn;

			if (mpActiveTRegion->m_bRegionROI)
			{
				HROIRgn = mpActiveTRegion->m_HTeachPolygonRgn;
			}
			else
			{
				POINT LTPoint, RBPoint;

				mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

				if (mpActiveTRegion->GetShape() == TRUE)
					GenCircle(&HROIRgn, (LTPoint.y + RBPoint.y) / 2, (LTPoint.x + RBPoint.x) / 2, (RBPoint.y - LTPoint.y) / 2);
				else
					GenRectangle1(&HROIRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
			}

			if (m_iDisplayChannelType == CHANNEL_TYPE_COLOR)
			{
				HObject HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS;
				HObject HGrayImage;
				if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
				{
					Decompose3(m_HTeachingImage[m_iDisplayChannelType][0], &HTempImageR, &HTempImageG, &HTempImageB);
					TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");
				}
				else
				{
					Decompose3(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], &HTempImageR, &HTempImageG, &HTempImageB);
					TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");
				}

				ReduceDomain(HGrayImage, HROIRgn, &HReducedImage);
			}
			else
			{
				if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
					ReduceDomain(m_HTeachingImage[m_iDisplayChannelType][0], HROIRgn, &HReducedImage);
				else
					ReduceDomain(m_HTeachingImage[m_iDisplayChannelType][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1], HROIRgn, &HReducedImage);
			}

			Threshold(HReducedImage, &HThresRgn, iThresMin, iThresMax);

			HObject HSelectedRgn, HHoleRgn, HFillRgn, HBoundaryRgn;
			Hlong lNoBlob;

			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn))
			{
				if (bCheckMinBlob)
				{
					Connection(HThresRgn, &HThresRgn);
					SelectShape(HThresRgn, &HThresRgn, "area", "and", iEditMinBlobSize, 9999999999999);
					Union1(HThresRgn, &HThresRgn);
				}

				if (bCheckMaxArea)
				{
					Connection(HThresRgn, &HThresRgn);
					SelectShapeStd(HThresRgn, &HThresRgn, HTuple("max_area"), HTuple(70));
				}

				if (bCheckFillUp)
				{
					FillUp(HThresRgn, &HThresRgn);
				}

				if (bCheckOpeningClosingInverse)
				{
					if (bCheckClosing)
						ClosingCircle(HThresRgn, &HThresRgn, (double)iEditClosingSize + 0.5);
					if (bCheckOpening)
						OpeningCircle(HThresRgn, &HThresRgn, (double)iEditOpeningSize + 0.5);
				}
				else
				{
					if (bCheckOpening)
						OpeningCircle(HThresRgn, &HThresRgn, (double)iEditOpeningSize + 0.5);
					if (bCheckClosing)
						ClosingCircle(HThresRgn, &HThresRgn, (double)iEditClosingSize + 0.5);
				}

				if (bCheckConvex)
				{
					if (bCheckMinBlob)
					{
						Connection(HThresRgn, &HThresRgn);

						HTuple HlNoBlob;
						CountObj(HThresRgn, &HlNoBlob);
						lNoBlob = HlNoBlob.L();

						GenEmptyObj(&HHoleRgn);

						for (int i = 1; i <= lNoBlob; i++)
						{
							SelectObj(HThresRgn, &HSelectedRgn, i);

							FillUp(HSelectedRgn, &HFillRgn);
							Difference(HFillRgn, HSelectedRgn, &HSelectedRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn))
							{
								ShapeTrans(HSelectedRgn, &HSelectedRgn, "convex");
								ConcatObj(HHoleRgn, HSelectedRgn, &HHoleRgn);
							}
						}

						Union1(HHoleRgn, &HHoleRgn);

						ShapeTrans(HThresRgn, &HThresRgn, "convex");
						Union1(HThresRgn, &HThresRgn);

						Difference(HThresRgn, HHoleRgn, &HThresRgn);
					}
					else
					{
						ShapeTrans(HThresRgn, &HThresRgn, "convex");
					}
				}

				if (bCheckRect)
				{
					if (bCheckMinBlob)
					{
						Connection(HThresRgn, &HThresRgn);
						ShapeTrans(HThresRgn, &HThresRgn, "rectangle1");
						Union1(HThresRgn, &HThresRgn);
					}
					else
					{
						ShapeTrans(HThresRgn, &HThresRgn, "rectangle1");
					}
				}

				if (bCheckDifference)
				{
					Difference(HROIRgn, HThresRgn, &HThresRgn);
				}

				if (bCheckBoundary)
				{
					Connection(HThresRgn, &HThresRgn);

					HTuple HlNoBlob;
					CountObj(HThresRgn, &HlNoBlob);
					lNoBlob = HlNoBlob.L();

					GenEmptyObj(&HBoundaryRgn);

					for (int i = 1; i <= lNoBlob; i++)
					{
						SelectObj(HThresRgn, &HSelectedRgn, i);

						FillUp(HSelectedRgn, &HSelectedRgn);
						Boundary(HSelectedRgn, &HSelectedRgn, "inner");
						DilationCircle(HSelectedRgn, &HSelectedRgn, (double)iEditBoundaryDilationSize + 0.5);

						if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn))
						{
							ConcatObj(HBoundaryRgn, HSelectedRgn, &HBoundaryRgn);
						}
					}

					Union1(HBoundaryRgn, &HThresRgn);
				}

				if (bCheckDilation)
				{
					if (iEditDilationSize >= 0)
					{
						DilationCircle(HThresRgn, &HThresRgn, (double)iEditDilationSize + 0.5);
					}
					else
					{
						int iMargin = abs(iEditDilationSize);
						ErosionCircle(HThresRgn, &HThresRgn, (double)iMargin + 0.5);
					}
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn))
			{
				m_pThresRgn = HThresRgn;
			}
			else
				GenEmptyObj(&m_pThresRgn);

			InvalidateRect(&m_ClientRect, FALSE);
		}
	}
	catch (HException& except)
	{
		strLog.Format("Halcon Exception [InspectAdminViewDlg::ShowRegionROI] : %s", (CString)(LPCTSTR)except.ErrorMessage());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}

BOOL CInspectAdminViewDlg::CheckDontCareInclusion(HObject HNewRegion)
{
	if (THEAPP.m_pGFunction->ValidHRegion(HNewRegion) == FALSE)
		return FALSE;

	if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
		return FALSE;

	// Region ROI

	HObject HDontCareRgn;
	GenEmptyObj(&HDontCareRgn);

	HObject HROIHRegion;

	GTRegion* pRegion;
	for (int k = 0; k < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
		if (pRegion == NULL)
			continue;

		if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
			if (pRegion->miInspectionType == INSPECTION_TYPE_DELETE)
			{
				HROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);
				if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
					ConcatObj(HDontCareRgn, HROIHRegion, &HDontCareRgn);
			}
		}
	}

	if (THEAPP.m_pGFunction->ValidHRegion(HDontCareRgn))
	{
		HObject HIntersectRgn;

		Union1(HDontCareRgn, &HDontCareRgn);
		Intersection(HDontCareRgn, HNewRegion, &HIntersectRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn))
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

void CInspectAdminViewDlg::GetRegionROIAfterDontCareRemoval(GTRegion* pNewTRegion)
{
	int i;

	HObject HPolygonRgn;
	HPolygonRgn = pNewTRegion->GetROIHRegion(THEAPP.m_pCalDataService);

	HObject HDontCareRgn;
	GenEmptyObj(&HDontCareRgn);

	PList<GTRegion>* pDeleteTRegionList = new PList<GTRegion>(PLNO_POINTER);

	HObject HROIHRegion;
	HObject HIntersectRgn;

	GTRegion* pRegion;
	for (int k = 0; k < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
		if (pRegion == NULL)
			continue;

		if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
			if (pRegion->miInspectionType == INSPECTION_TYPE_DELETE)
			{
				HROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);
				if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
				{
					Intersection(HROIHRegion, HPolygonRgn, &HIntersectRgn);
					if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn))
					{
						ConcatObj(HDontCareRgn, HROIHRegion, &HDontCareRgn);
						pDeleteTRegionList->Add(pRegion);
					}
				}
			}
		}
	}

	// DontCare TRegion 제거
	for (i = 0; i < pDeleteTRegionList->miCount; i++)
	{
		GTRegion* pDeleteTRegion = pDeleteTRegionList->Get(i);
		THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(pDeleteTRegion, TRUE);
	}
	delete pDeleteTRegionList;

	if (THEAPP.m_pGFunction->ValidHRegion(HDontCareRgn))
	{
		Union1(HDontCareRgn, &HDontCareRgn);
		Difference(HPolygonRgn, HDontCareRgn, &HPolygonRgn);
	}

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	SmallestRectangle1(HPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

	lRow1 = HlRow1.L();
	lCol1 = HlCol1.L();
	lRow2 = HlRow2.L();
	lCol2 = HlCol2.L();

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	pNewTRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetVisible(TRUE, FALSE);
	pNewTRegion->SetSelectable(TRUE, FALSE);
	pNewTRegion->SetMovable(TRUE, FALSE);
	pNewTRegion->SetSizable(FALSE, FALSE);

	pNewTRegion->m_bRegionROI = TRUE;
	CopyObj(HPolygonRgn, &(pNewTRegion->m_HTeachPolygonRgn), 1, -1);
}

BOOL CInspectAdminViewDlg::CheckAddInclusion(HObject HNewRegion)
{
	if (THEAPP.m_pGFunction->ValidHRegion(HNewRegion) == FALSE)
		return FALSE;

	if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
		return FALSE;

	// Region ROI

	HObject HAddRgn;
	GenEmptyObj(&HAddRgn);

	HObject HROIHRegion;

	GTRegion* pRegion;
	for (int k = 0; k < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
		if (pRegion == NULL)
			continue;

		if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
			if (pRegion->miInspectionType == INSPECTION_TYPE_ADD)
			{
				HROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);
				if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
					ConcatObj(HAddRgn, HROIHRegion, &HAddRgn);
			}
		}
	}

	if (THEAPP.m_pGFunction->ValidHRegion(HAddRgn))
	{
		HObject HIntersectRgn;

		Union1(HAddRgn, &HAddRgn);
		Intersection(HAddRgn, HNewRegion, &HIntersectRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn))
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

void CInspectAdminViewDlg::GetRegionROIAfterAddRemoval(GTRegion* pNewTRegion)
{
	int i;

	HObject HPolygonRgn;
	HPolygonRgn = pNewTRegion->GetROIHRegion(THEAPP.m_pCalDataService);

	HObject HAddRgn;
	GenEmptyObj(&HAddRgn);

	PList<GTRegion>* pDeleteTRegionList = new PList<GTRegion>(PLNO_POINTER);

	HObject HROIHRegion;
	HObject HIntersectRgn;

	GTRegion* pRegion;
	for (int k = 0; k < THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
		if (pRegion == NULL)
			continue;

		if (pRegion->miTeachImageIndex >= 0 && pRegion->miTeachImageIndex == THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
			if (pRegion->miInspectionType == INSPECTION_TYPE_ADD)
			{
				HROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);
				if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
				{
					Intersection(HROIHRegion, HPolygonRgn, &HIntersectRgn);
					if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn))
					{
						ConcatObj(HAddRgn, HROIHRegion, &HAddRgn);
						pDeleteTRegionList->Add(pRegion);
					}
				}
			}
		}
	}

	// Add TRegion 제거
	for (i = 0; i < pDeleteTRegionList->miCount; i++)
	{
		GTRegion* pDeleteTRegion = pDeleteTRegionList->Get(i);
		THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(pDeleteTRegion, TRUE);
	}
	delete pDeleteTRegionList;

	if (THEAPP.m_pGFunction->ValidHRegion(HAddRgn))
	{
		Union1(HAddRgn, &HAddRgn);
		Union2(HPolygonRgn, HAddRgn, &HPolygonRgn);
	}

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	SmallestRectangle1(HPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

	lRow1 = HlRow1.L();
	lCol1 = HlCol1.L();
	lRow2 = HlRow2.L();
	lCol2 = HlCol2.L();

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	pNewTRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetVisible(TRUE, FALSE);
	pNewTRegion->SetSelectable(TRUE, FALSE);
	pNewTRegion->SetMovable(TRUE, FALSE);
	pNewTRegion->SetSizable(FALSE, FALSE);

	pNewTRegion->m_bRegionROI = TRUE;
	CopyObj(HPolygonRgn, &(pNewTRegion->m_HTeachPolygonRgn), 1, -1);
}

void CInspectAdminViewDlg::GetRegionROIAfterEllipse(GTRegion* pNewTRegion)
{
	int i;

	HObject HPolygonRgn;
	HPolygonRgn = pNewTRegion->GetROIHRegion(THEAPP.m_pCalDataService);

	HTuple htOrgRow, htOrgCol, htOrgRad;
	SmallestCircle(HPolygonRgn, &htOrgRow, &htOrgCol, &htOrgRad);
	GenEllipse(&HPolygonRgn, htOrgRow, htOrgCol, 0, htOrgRad * 1.25, htOrgRad);

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	HalconCpp::SmallestRectangle1(HPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

	lRow1 = HlRow1.L();
	lCol1 = HlCol1.L();
	lRow2 = HlRow2.L();
	lCol2 = HlCol2.L();

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	pNewTRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetVisible(TRUE, FALSE);
	pNewTRegion->SetSelectable(TRUE, FALSE);
	pNewTRegion->SetMovable(TRUE, FALSE);
	pNewTRegion->SetSizable(FALSE, FALSE);

	pNewTRegion->m_bRegionROI = TRUE;
HalconCpp:CopyObj(HPolygonRgn, &(pNewTRegion->m_HTeachPolygonRgn), 1, -1);
}

void CInspectAdminViewDlg::GetRegionROIAfterRotation(GTRegion* pNewTRegion, double dRotationAngle)
{
	int i;

	HObject HPolygonRgn;
	HPolygonRgn = pNewTRegion->GetROIHRegion(THEAPP.m_pCalDataService);

	HTuple HAngleRad;

	TupleRad(HTuple(dRotationAngle), &HAngleRad);

	HTuple HArea, HCenterX, HCenterY;
	AreaCenter(HPolygonRgn, &HArea, &HCenterY, &HCenterX);

	HTuple HomMat2DIdentity, HomMat2DRotate;
	HomMat2dIdentity(&HomMat2DIdentity);
	HomMat2dRotate(HomMat2DIdentity, HAngleRad, HCenterY, HCenterX, &HomMat2DRotate);

	AffineTransRegion(HPolygonRgn, &HPolygonRgn, HomMat2DRotate, "constant");

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	HalconCpp::SmallestRectangle1(HPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

	lRow1 = HlRow1.L();
	lCol1 = HlCol1.L();
	lRow2 = HlRow2.L();
	lCol2 = HlCol2.L();

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	pNewTRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetVisible(TRUE, FALSE);
	pNewTRegion->SetSelectable(TRUE, FALSE);
	pNewTRegion->SetMovable(TRUE, FALSE);
	pNewTRegion->SetSizable(FALSE, FALSE);

	pNewTRegion->m_bRegionROI = TRUE;
HalconCpp:CopyObj(HPolygonRgn, &(pNewTRegion->m_HTeachPolygonRgn), 1, -1);
}

void CInspectAdminViewDlg::ChangeViewport(int iCamImageWidth, int iCamImageHeight)
{
	m_iImageWidth = iCamImageWidth;
	m_iImageHeight = iCamImageHeight;

	InitViewRect();
	UpdateViewportManager();
	ScrollBarSet();
	TbFitHeight();
}

void CInspectAdminViewDlg::CopyInspectionROI()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("복사할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for copy.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	GTRegion* pCopyRegion;

	mpActiveTRegion->Duplicate(&pCopyRegion);

	pCopyRegion->miTeachImageIndex = m_iRoiCopyImageIndex;

	if (THEAPP.m_pTabControlDlg->m_iCurrentTab == m_iRoiCopyImageIndex)		// 동일 영상에 복사
	{
		DPOINT dCenterPoint, dWHPoint, dNewCenterPoint;

		if (pCopyRegion->m_bRegionROI)
		{
			POINT LTPoint, RBPoint;
			Hlong lRow1, lRow2, lCol1, lCol2;

			MoveRegion(pCopyRegion->m_HTeachPolygonRgn, &(pCopyRegion->m_HTeachPolygonRgn), 50, 50);

			HTuple HlRow1, HlRow2, HlCol1, HlCol2;

			SmallestRectangle1(pCopyRegion->m_HTeachPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

			LTPoint.x = HlCol1.L();
			LTPoint.y = HlRow1.L();
			RBPoint.x = HlCol2.L();
			RBPoint.y = HlRow2.L();

			pCopyRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
			pCopyRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
		}
		else
		{
			pCopyRegion->GetCenterPointM(&dCenterPoint);
			pCopyRegion->GetWHPointM(&dWHPoint);

			dNewCenterPoint.x = dCenterPoint.x + dWHPoint.x / 2;
			dNewCenterPoint.y = dCenterPoint.y + dWHPoint.y / 2;

			pCopyRegion->SetCenterPointM(dNewCenterPoint);
		}
	}

	if (pCopyRegion->miInspectionType >= INSPECTION_TYPE_FIRST && pCopyRegion->miInspectionType <= INSPECTION_TYPE_LAST)
	{
		int iLastInspectionROIID = 0;
		iLastInspectionROIID = THEAPP.m_pModelDataManager->GetLastPadID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miInspectionROIID = iLastInspectionROIID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		int iLastLocalAlignID = 0;
		iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miLocalAlignID = iLastLocalAlignID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_DONTCARE)
	{
		int iLastDontCareID = 0;
		iLastDontCareID = THEAPP.m_pModelDataManager->GetLastDontCareID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miDontCareID = iLastDontCareID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_GENERATE)
	{
		int iLastGenerateID = 0;
		iLastGenerateID = THEAPP.m_pModelDataManager->GetLastGenerateID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miGenerateID = iLastGenerateID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_MASK)
	{
		int iLastMaskID = 0;
		iLastMaskID = THEAPP.m_pModelDataManager->GetLastMaskID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miMaskID = iLastMaskID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_COLOR)
	{
		int iLastColorInfoID = 0;
		iLastColorInfoID = THEAPP.m_pModelDataManager->GetLastColorInfoID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miColorInfoID = iLastColorInfoID + 1;
	}

	if ((pCopyRegion->miInspectionType == INSPECTION_TYPE_MEASURE_POINT) || (pCopyRegion->miInspectionType == INSPECTION_TYPE_BLEND_POINT))	// Point
	{
		pCopyRegion->miPositionID = mpActiveTRegion->miPositionID;

		int iLastPointID = 0;
		iLastPointID = THEAPP.m_pModelDataManager->GetLastPointID(pCopyRegion->miInspectionType, pCopyRegion->miPositionID);

		// 최대 포인트 수
		int iMaxPoiintNum = 0;
		switch (pCopyRegion->miInspectionType)
		{
		case INSPECTION_TYPE_MEASURE_POINT:
			iMaxPoiintNum = MAX_MEASURE_POINT_NUM;
			break;
		case INSPECTION_TYPE_BLEND_POINT:
			iMaxPoiintNum = BLEND_POINT_END;
			break;
		}

		if (iLastPointID + 1 < iMaxPoiintNum)
		{
			pCopyRegion->miPointID = iLastPointID + 1;
		}
		else
		{
			AfxMessageBox("선택한 Point가 최대 허용 Ponit를 초과하였습니다.", MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);
		}
	}

	THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pCopyRegion);

	InvalidateRect(&m_ClientRect, TRUE);

	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		if (m_iRoiCopyImageIndex == 0)
			strMessageBox.Format("기본정보 탭에 복사되었습니다.");
		else
			strMessageBox.Format("영상 %d 탭에 복사되었습니다.", m_iRoiCopyImageIndex);
	}
	else
	{
		if (m_iRoiCopyImageIndex == 0)
			strMessageBox.Format("Copied to Information tab.");
		else
			strMessageBox.Format("Copied to Image %d tab.", m_iRoiCopyImageIndex);
	}
	AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void CInspectAdminViewDlg::CopyAllInspectionROI()
{
	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		if (m_iRoiCopyImageIndex == 0)
			strMessageBox.Format("현재 영상탭의 모든 ROI를 기본정보탭으로 이동하겠습니까?");
		else
			strMessageBox.Format("현재 영상탭의 모든 ROI를 영상 %d 탭으로 이동하겠습니까?", m_iRoiCopyImageIndex);
	}
	else
	{
		if (m_iRoiCopyImageIndex == 0)
			strMessageBox.Format("Move all ROIs in the current tab to information tab?");
		else
			strMessageBox.Format("Move all ROIs in the current tab to image tab %d?", m_iRoiCopyImageIndex);
	}
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if (nRes != IDYES)
		return;

	if (THEAPP.m_pTabControlDlg->m_iCurrentTab == m_iRoiCopyImageIndex)		// 동일 영상에 복사
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("동일 영상탭으로는 이동할 수 없습니다.");
		else
			strMessageBox.Format("Can't move to the same image tab.");
		AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("이동할 영상탭의 모든 ROI가 삭제됩니다. 진행하시겠습니까?");
	else
		strMessageBox.Format("All ROIs in the target tab will be deleted. Continue?");
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if (nRes != IDYES)
		return;

	int i, j;
	GTRegion* pInspectROIRgn;
	GTRegion* pCopyRegion;
	int iNoInspectROI;
	int iLastID;

	iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

	// Delete All ROIs of image tab to copy
	if (iNoInspectROI > 0)
	{
		mpLastSelectedRegion = NULL;

		for (int iIndex = (iNoInspectROI - 1); iIndex >= 0; iIndex--)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != m_iRoiCopyImageIndex)
				continue;

			THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(pInspectROIRgn, FALSE);
		}

		mpActiveTRegion = NULL;
	}

	int iSearchImageTabNo, iSearchROINo;

	iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

	if (iNoInspectROI > 0)
	{
		// Update Link ROI Info
		for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != THEAPP.m_pTabControlDlg->m_iCurrentTab)
				continue;

			if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
			{
				iSearchImageTabNo = pInspectROIRgn->miTeachImageIndex;
				iSearchROINo = pInspectROIRgn->miLocalAlignID;

				THEAPP.m_pModelDataManager->UpdateLinkInfo_LocalAlign(iSearchImageTabNo, iSearchROINo, m_iRoiCopyImageIndex);
			}

			if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_DONTCARE)
			{
				iSearchImageTabNo = pInspectROIRgn->miTeachImageIndex;
				iSearchROINo = pInspectROIRgn->miDontCareID;

				THEAPP.m_pModelDataManager->UpdateLinkInfo_DontCare(iSearchImageTabNo, iSearchROINo, m_iRoiCopyImageIndex);
			}

			if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_GENERATE)
			{
				iSearchImageTabNo = pInspectROIRgn->miTeachImageIndex;
				iSearchROINo = pInspectROIRgn->miGenerateID;

				THEAPP.m_pModelDataManager->UpdateLinkInfo_Generate(iSearchImageTabNo, iSearchROINo, m_iRoiCopyImageIndex);
			}

			if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_MASK)
			{
				iSearchImageTabNo = pInspectROIRgn->miTeachImageIndex;
				iSearchROINo = pInspectROIRgn->miMaskID;

				THEAPP.m_pModelDataManager->UpdateLinkInfo_Mask(iSearchImageTabNo, iSearchROINo, m_iRoiCopyImageIndex);
			}
		}

		// Change Image Index
		for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != THEAPP.m_pTabControlDlg->m_iCurrentTab)
				continue;

			pInspectROIRgn->miTeachImageIndex = m_iRoiCopyImageIndex;
		}
	}

	InvalidateRect(&m_ClientRect, TRUE);

	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		if (m_iRoiCopyImageIndex == 0)
			strMessageBox.Format("기본정보 탭에 복사되었습니다.");
		else
			strMessageBox.Format("영상 %d 탭에 복사되었습니다.", m_iRoiCopyImageIndex);
	}
	else
	{
		if (m_iRoiCopyImageIndex == 0)
			strMessageBox.Format("Copied to Information tab.");
		else
			strMessageBox.Format("Copied to Image %d tab.", m_iRoiCopyImageIndex);
	}
	AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void CInspectAdminViewDlg::CopyInspectionROI_Vision()
{
	if (mpActiveTRegion == NULL)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("변경할 ROI가 선택되지 않았습니다.");
		else
			strMessageBox.Format("ROI not selected for Copy.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	if (THEAPP.m_iCurTeachVision == m_iRoiCopyVisionIndex)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("동일한 비전을 선택하였습니다. 동일 비전에 대해서는 [선택된 ROI 복사 (영상탭)] 기능을 사용하세요.");
		else
			strMessageBox.Format("Same vision selected. Use [Selected ROI copy(Image tab)] for same vision.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	GTRegion* pCopyRegion;

	mpActiveTRegion->Duplicate(&pCopyRegion);

	if (pCopyRegion->miInspectionType >= INSPECTION_TYPE_FIRST && pCopyRegion->miInspectionType <= INSPECTION_TYPE_LAST)
	{
		int iLastInspectionROIID = 0;
		iLastInspectionROIID = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][m_iRoiCopyVisionIndex]->GetLastPadID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pCopyRegion->miInspectionROIID = iLastInspectionROIID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		int iLastLocalAlignID = 0;
		iLastLocalAlignID = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][m_iRoiCopyVisionIndex]->GetLastLocalAlignID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miLocalAlignID = iLastLocalAlignID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_DONTCARE)
	{
		int iLastDontCareID = 0;
		iLastDontCareID = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][m_iRoiCopyVisionIndex]->GetLastDontCareID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miDontCareID = iLastDontCareID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_GENERATE)
	{
		int iLastGenerateID = 0;
		iLastGenerateID = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][m_iRoiCopyVisionIndex]->GetLastGenerateID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miGenerateID = iLastGenerateID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_MASK)
	{
		int iLastMaskID = 0;
		iLastMaskID = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][m_iRoiCopyVisionIndex]->GetLastMaskID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miMaskID = iLastMaskID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_COLOR)
	{
		int iLastColorInfoID = 0;
		iLastColorInfoID = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][m_iRoiCopyVisionIndex]->GetLastColorInfoID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miColorInfoID = iLastColorInfoID + 1;
	}

	if ((pCopyRegion->miInspectionType == INSPECTION_TYPE_MEASURE_POINT) || (pCopyRegion->miInspectionType == INSPECTION_TYPE_BLEND_POINT))	// Point
	{
		pCopyRegion->miPositionID = mpActiveTRegion->miPositionID;

		int iLastPointID = 0;
		iLastPointID = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][m_iRoiCopyVisionIndex]->GetLastPointID(pCopyRegion->miInspectionType, pCopyRegion->miPositionID);

		// 최대 포인트 수
		int iMaxPoiintNum = 0;
		switch (pCopyRegion->miInspectionType)
		{
		case INSPECTION_TYPE_MEASURE_POINT:
			iMaxPoiintNum = MAX_MEASURE_POINT_NUM;
			break;
		case INSPECTION_TYPE_BLEND_POINT:
			iMaxPoiintNum = BLEND_POINT_END;
			break;
		}

		if (iLastPointID + 1 < iMaxPoiintNum)
		{
			pCopyRegion->miPointID = iLastPointID + 1;
		}
		else
		{
			AfxMessageBox("선택한 Point가 최대 허용 Ponit를 초과하였습니다.", MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);
		}

	}

	THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][m_iRoiCopyVisionIndex]->m_pInspectionArea->AddChildTRegion(pCopyRegion);

	InvalidateRect(&m_ClientRect, TRUE);

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("비전 %d에 복사되었습니다.", m_iRoiCopyVisionIndex + 1);
	else
		strMessageBox.Format("Copied to Vision %d.", m_iRoiCopyVisionIndex + 1);
	AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void CInspectAdminViewDlg::CopyInspectionAllROI_Vision(int iVisionIndexToCopy, BOOL* pbApplyCopyImage, BOOL bDeleteFirst)
{
	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("비전 %d로 선택한 영상 번호의 모든 ROI를 복사하겠습니까?", iVisionIndexToCopy + 1);
	else
		strMessageBox.Format("Copy all ROIs of the selected images to Vision %d?", iVisionIndexToCopy + 1);
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if (nRes != IDYES)
		return;

	int i, j;
	GTRegion* pInspectROIRgn;
	int iNoInspectROI;

	if (bDeleteFirst)
	{
		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iVisionIndexToCopy]->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI > 0)
		{
			for (int iIndex = (iNoInspectROI - 1); iIndex >= 0; iIndex--)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iVisionIndexToCopy]->m_pInspectionArea->GetChildTRegion(iIndex);
				if (pInspectROIRgn == NULL)
					continue;

				for (i = 0; i < MAX_IMAGE_TAB; i++)
				{
					if (pbApplyCopyImage[i])
					{
						if (pInspectROIRgn->miTeachImageIndex == (i + 1))
						{
							THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iVisionIndexToCopy]->m_pInspectionArea->DeleteChildTRegionPtr(pInspectROIRgn, FALSE);
						}
					}
				}
			}
		}
	}

	GTRegion* pCopyRegion;
	int iLastID;

	iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI > 0)
	{
		for (int iIndex = (iNoInspectROI - 1); iIndex >= 0; iIndex--)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			for (i = 0; i < MAX_IMAGE_TAB; i++)
			{
				if (pbApplyCopyImage[i])
				{
					if (pInspectROIRgn->miTeachImageIndex == (i + 1))
					{
						pInspectROIRgn->Duplicate(&pCopyRegion);

						if (pCopyRegion->miInspectionType >= INSPECTION_TYPE_FIRST && pCopyRegion->miInspectionType <= INSPECTION_TYPE_LAST)
						{
							int iLastInspectionROIID = 0;
							iLastInspectionROIID = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iVisionIndexToCopy]->GetLastPadID(i);
							pCopyRegion->miInspectionROIID = iLastInspectionROIID + 1;
						}

						THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iVisionIndexToCopy]->m_pInspectionArea->AddChildTRegion(pCopyRegion);
					}
				}
			}
		}
	}

	InvalidateRect(&m_ClientRect, TRUE);

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("비전 %d에 복사되었습니다.", iVisionIndexToCopy + 1);
	else
		strMessageBox.Format("Copied to Vision %d.", iVisionIndexToCopy + 1);
	AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void CInspectAdminViewDlg::CreateAutoFocusROI()
{
	m_PrevToolBarState = m_ToolBarState;
	m_ToolBarState = TS_CREATE_AF_ROI;
}

void CInspectAdminViewDlg::DeleteCrossBarROI()
{
	GTRegion* pRegion;
	int i, iNoROI;
	BOOL bCrossBarExist = FALSE;

	if (THEAPP.m_pModelDataManager->m_pInspectionArea != NULL)
	{
		while (1)
		{
			iNoROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

			bCrossBarExist = FALSE;
			for (i = 0; i < iNoROI; i++)
			{
				pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(i);
				if (pRegion == NULL)
					continue;

				if (pRegion->miInspectionType == INSPECTION_TYPE_CROSS_BAR)
				{
					THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(pRegion, FALSE);

					bCrossBarExist = TRUE;
					break;
				}
			}

			if (bCrossBarExist == FALSE)
				break;
		}
	}
}

void CInspectAdminViewDlg::ClearAlignTabTempRegion()
{
	GenEmptyObj(&m_HMultiModuleCropRgn);
	GenEmptyObj(&m_HAutoFocusCheckRgn);
	GenEmptyObj(&m_HShapeModelContrastRgn);
}

void CInspectAdminViewDlg::ResetInspectionResult()
{
	GenEmptyObj(&m_HInspectAreaRgn);
	GenEmptyObj(&m_HParameterDefectRgn);
	GenEmptyObj(&m_HParameterDefectXLD);
	GenEmptyObj(&m_HBarcodeDetectRgn);
	GenEmptyObj(&m_HOCRDetectRgn);
	GenEmptyObj(&m_HAIDefectRgn);
	GenEmptyObj(&m_HAutoFocusRgn);
	GenEmptyObj(&m_HAutoFocusCenterRgn);
	GenEmptyObj(&(THEAPP.m_pAlgorithm->m_HGapMeasureXLD));
	GenEmptyObj(&(THEAPP.m_pAlgorithm->m_HCornerMeasureXLD));

	ResetManualBarcodeFlagForOCR();
}

void CInspectAdminViewDlg::MoveActiveRegion(int iMoveDirection)
{
	if (mpActiveTRegion == NULL)
		return;

	DPOINT dCenterPointM;

	if (mpActiveTRegion->m_bRegionROI)
	{
		if (iMoveDirection == ROI_KEY_UP)
			MoveRegion(mpActiveTRegion->m_HTeachPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), -1, 0);
		else if (iMoveDirection == ROI_KEY_DOWN)
			MoveRegion(mpActiveTRegion->m_HTeachPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), 1, 0);
		else if (iMoveDirection == ROI_KEY_LEFT)
			MoveRegion(mpActiveTRegion->m_HTeachPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), 0, -1);
		else if (iMoveDirection == ROI_KEY_RIGHT)
			MoveRegion(mpActiveTRegion->m_HTeachPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), 0, 1);
	}
	else
	{
		mpActiveTRegion->GetCenterPointM(&dCenterPointM);

		if (iMoveDirection == ROI_KEY_UP)
			dCenterPointM.y -= THEAPP.m_pCalDataService->GetPixelSize();
		else if (iMoveDirection == ROI_KEY_DOWN)
			dCenterPointM.y += THEAPP.m_pCalDataService->GetPixelSize();
		else if (iMoveDirection == ROI_KEY_LEFT)
			dCenterPointM.x -= THEAPP.m_pCalDataService->GetPixelSize();
		else if (iMoveDirection == ROI_KEY_RIGHT)
			dCenterPointM.x += THEAPP.m_pCalDataService->GetPixelSize();

		mpActiveTRegion->SetCenterPointM(dCenterPointM);
	}
}

void CInspectAdminViewDlg::ScaleActiveRegion(int iMoveDirection, BOOL bScaleUp)
{
	if (mpActiveTRegion == NULL)
		return;

	DPOINT dLTPointM, dRBPointM;

	if (mpActiveTRegion->m_bRegionROI == FALSE)
	{
		if (mpActiveTRegion->GetShape() == FALSE)	// Rect
		{
			mpActiveTRegion->GetLTPointM(&dLTPointM);
			mpActiveTRegion->GetRBPointM(&dRBPointM);

			if (iMoveDirection == ROI_KEY_UP)
			{
				if (bScaleUp)
					dLTPointM.y -= THEAPP.m_pCalDataService->GetNominalPixelSize();
				else
					dLTPointM.y += THEAPP.m_pCalDataService->GetNominalPixelSize();
			}
			else if (iMoveDirection == ROI_KEY_DOWN)
			{
				if (bScaleUp)
					dRBPointM.y += THEAPP.m_pCalDataService->GetNominalPixelSize();
				else
					dRBPointM.y -= THEAPP.m_pCalDataService->GetNominalPixelSize();
			}
			else if (iMoveDirection == ROI_KEY_LEFT)
			{
				if (bScaleUp)
					dLTPointM.x -= THEAPP.m_pCalDataService->GetNominalPixelSize();
				else
					dLTPointM.x += THEAPP.m_pCalDataService->GetNominalPixelSize();
			}
			else if (iMoveDirection == ROI_KEY_RIGHT)
			{
				if (bScaleUp)
					dRBPointM.x += THEAPP.m_pCalDataService->GetNominalPixelSize();
				else
					dRBPointM.x -= THEAPP.m_pCalDataService->GetNominalPixelSize();
			}

			mpActiveTRegion->SetLTPointM(dLTPointM);
			mpActiveTRegion->SetRBPointM(dRBPointM);
		}
		else	// Circle
		{
			mpActiveTRegion->GetLTPointM(&dLTPointM);
			mpActiveTRegion->GetRBPointM(&dRBPointM);

			if (iMoveDirection == ROI_KEY_UP)
			{
				if (bScaleUp)
				{
					dLTPointM.y -= THEAPP.m_pCalDataService->GetNominalPixelSize();
					dLTPointM.x -= THEAPP.m_pCalDataService->GetNominalPixelSize();
				}
				else
				{
					dLTPointM.y += THEAPP.m_pCalDataService->GetNominalPixelSize();
					dLTPointM.x += THEAPP.m_pCalDataService->GetNominalPixelSize();
				}
			}
			else if (iMoveDirection == ROI_KEY_DOWN)
			{
				if (bScaleUp)
				{
					dRBPointM.y += THEAPP.m_pCalDataService->GetNominalPixelSize();
					dRBPointM.x += THEAPP.m_pCalDataService->GetNominalPixelSize();
				}
				else
				{
					dRBPointM.y -= THEAPP.m_pCalDataService->GetNominalPixelSize();
					dRBPointM.x -= THEAPP.m_pCalDataService->GetNominalPixelSize();
				}
			}
			else if (iMoveDirection == ROI_KEY_LEFT)
			{
				if (bScaleUp)
				{
					dLTPointM.x -= THEAPP.m_pCalDataService->GetNominalPixelSize();
					dLTPointM.y -= THEAPP.m_pCalDataService->GetNominalPixelSize();
				}
				else
				{
					dLTPointM.x += THEAPP.m_pCalDataService->GetNominalPixelSize();
					dLTPointM.y += THEAPP.m_pCalDataService->GetNominalPixelSize();
				}
			}
			else if (iMoveDirection == ROI_KEY_RIGHT)
			{
				if (bScaleUp)
				{
					dRBPointM.x += THEAPP.m_pCalDataService->GetNominalPixelSize();
					dRBPointM.y += THEAPP.m_pCalDataService->GetNominalPixelSize();
				}
				else
				{
					dRBPointM.x -= THEAPP.m_pCalDataService->GetNominalPixelSize();
					dRBPointM.y -= THEAPP.m_pCalDataService->GetNominalPixelSize();
				}
			}

			mpActiveTRegion->SetLTPointM(dLTPointM);
			mpActiveTRegion->SetRBPointM(dRBPointM);
		}
	}
}

void CInspectAdminViewDlg::ResetManualBarcodeFlagForOCR()
{
	THEAPP.m_pAlgorithm->m_bManualBarcodeForOcrInspect = FALSE;
}

void CInspectAdminViewDlg::TeachingImageGrabSave()
{
	if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
	{
		if (THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_iTeachingGrab > 0)
		{
			if (THEAPP.m_pModelDataManager->m_sModelName == ".")
				return;

			CString strModelName, strFolder, strImageFolder, strImageName;

			strModelName = THEAPP.m_pModelDataManager->m_sModelName;
			strFolder = THEAPP.GetWorkingDirectory() + "\\Model\\";

			strImageFolder.Format("%s%s\\SW\\Vision_N%d\\TeachImage\\", strFolder, strModelName, THEAPP.m_iCurTeachVision + 1);

			strImageName.Format("%sImage_%d", strImageFolder, THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_iTeachingGrab);

			if (THEAPP.m_pGFunction->ValidHImage(m_pHLiveImage[CHANNEL_TYPE_COLOR]))
			{
				if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
					WriteImage(m_pHLiveImage[CHANNEL_TYPE_COLOR], "bmp", 0, HTuple(strImageName));
				else
					WriteImage(m_pHLiveImage[CHANNEL_TYPE_COLOR], "jpeg 100", 0, HTuple(strImageName));
			}

			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_iTeachingGrab = -1;
		}
	}
}

void CInspectAdminViewDlg::LaserScanImageSave(int iStartImageIndex)
{
	if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)
	{
		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return;

		CString strModelName, strFolder, strImageFolder, strImageName;

		strModelName = THEAPP.m_pModelDataManager->m_sModelName;
		strFolder = THEAPP.GetWorkingDirectory() + "\\Model\\";

		strImageFolder.Format("%s%s\\SW\\Vision_N%d\\TeachImage\\", strFolder, strModelName, THEAPP.m_iCurTeachVision + 1);

		for (int i = iStartImageIndex; i <= (iStartImageIndex + 1); i++)
		{
			strImageName.Format("%sImage_%d", strImageFolder, i + 1);

			if (THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[CHANNEL_TYPE_COLOR][i]))
			{
				HTuple HImageType;
				GetImageType(m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HImageType);

				if (HImageType == "int4")
				{
					WriteImage(m_HTeachingImage[CHANNEL_TYPE_I][i], "ima", 0, HTuple(strImageName));
				}
				else
				{
					if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
						WriteImage(m_HTeachingImage[CHANNEL_TYPE_COLOR][i], "bmp", 0, HTuple(strImageName));
					else
						WriteImage(m_HTeachingImage[CHANNEL_TYPE_COLOR][i], "jpeg 100", 0, HTuple(strImageName));
				}
			}
		}
	}
}

void CInspectAdminViewDlg::SetDisplayChannel(int iDisplayChannel)
{
	switch (iDisplayChannel)
	{
	case CHANNEL_TYPE_COLOR:
		TbDisplayColor();
		break;
	case CHANNEL_TYPE_R:
		TbDisplayR();
		break;
	case CHANNEL_TYPE_G:
		TbDisplayG();
		break;
	case CHANNEL_TYPE_B:
		TbDisplayB();
		break;
	case CHANNEL_TYPE_H:
		TbDisplayH();
		break;
	case CHANNEL_TYPE_S:
		TbDisplayS();
		break;
	case CHANNEL_TYPE_I:
		TbDisplayI();
		break;
	default:
		break;
	}
}

void CInspectAdminViewDlg::OnMenuInspectLibrary()
{
	THEAPP.m_pTabControlDlg->ShowInspectLibraryDlg();
}

void CInspectAdminViewDlg::OnMenuGrabSequenceView()
{
	THEAPP.m_pTabControlDlg->ShowGrabSequenceViewDlg();
}

BOOL CInspectAdminViewDlg::DoCheckROISelected()
{
	BOOL bRet = TRUE;

	if (mpActiveTRegion == NULL)
		bRet = FALSE;

	return bRet;
}

DPOINT CInspectAdminViewDlg::GetViewCenter()
{
	DPOINT dCenterPos;

	dCenterPos.x = (m_ViewRect.left + m_ViewRect.right) * 0.5;
	dCenterPos.y = (m_ViewRect.top + m_ViewRect.bottom) * 0.5;

	return dCenterPos;
}

void CInspectAdminViewDlg::AddAutoDrawROI(HRegion HPolygonRgn)
{
	HTuple HRows, HCols;

	TupleGenConst(0, 0, &HRows);
	TupleGenConst(0, 0, &HCols);

	try {
		GetRegionContour(HPolygonRgn, &HRows, &HCols);
	}
	catch (HOperatorException ex)
	{
		OutputDebugString(ex.ErrorMessage());
		// Halcon 라이브러리에서 발생한 오류 처리
		// Console.WriteLine($"Halcon 오류 발생: {}");
		return;
	}
	catch (std::exception ex)
	{
		OutputDebugString(ex.what());
		// 일반적인 예외 처리
		// Console.WriteLine($"기타 오류 발생: {ex.Message}");
		return;
	}

	//GenRegionPolygonFilled(&HPolygonRgn, HRows, HCols);

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	SmallestRectangle1(HPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

	lRow1 = HlRow1.L();
	lCol1 = HlCol1.L();
	lRow2 = HlRow2.L();
	lCol2 = HlCol2.L();

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	// m_bDrawRoi = true;

	//Invalidate();
	GTRegion* pPolygonRgn;

	pPolygonRgn = new GTRegion;
	pPolygonRgn->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pPolygonRgn->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
	pPolygonRgn->SetVisible(TRUE, FALSE);
	pPolygonRgn->SetSelectable(TRUE, FALSE);
	pPolygonRgn->SetMovable(TRUE, FALSE);
	pPolygonRgn->SetSizable(FALSE, FALSE);

	pPolygonRgn->miTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;

	pPolygonRgn->miInspectionType = INSPECTION_TYPE_AUTO_UNDEFINED;
	pPolygonRgn->SetLineColor(RGB(255, 255, 0));

	pPolygonRgn->m_bRegionROI = TRUE;
	CopyObj(HPolygonRgn, &(pPolygonRgn->m_HTeachPolygonRgn), 1, -1);

	HObject HROIHRegion;
	BOOL bDontCareExist, bAddExist;

	if ((pPolygonRgn->miInspectionType == INSPECTION_TYPE_DELETE || pPolygonRgn->miInspectionType == INSPECTION_TYPE_ADD) == FALSE)
	{
		HROIHRegion = pPolygonRgn->GetROIHRegion(THEAPP.m_pCalDataService);

		bDontCareExist = FALSE;
		bDontCareExist = CheckDontCareInclusion(HROIHRegion);

		if (bDontCareExist)
		{
			int nRes = 0;
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("새로운 ROI 내에 비검사 ROI가 존재합니다. 비검사 ROI 영역을 제외하시겠습니까?");
			else
				strMessageBox.Format("The new ROI contains an Exclusion ROI. Should the Exclusion ROI area be excluded?");
			nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);
		}

		bAddExist = FALSE;
		bAddExist = CheckAddInclusion(HROIHRegion);

		if (bAddExist)
		{
			int nRes = 0;
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("새로운 ROI 내에 추가 ROI가 존재합니다. ROI 영역을 추가하시겠습니까?");
			else
				strMessageBox.Format("The new ROI contains an Inclusion ROI. Should the Inclusion ROI area be included?");
			nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);
		}
	}

	if (pPolygonRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pPolygonRgn->miInspectionType <= INSPECTION_TYPE_LAST)
	{
		int iLastInspectionROIID = 0;
		iLastInspectionROIID = THEAPP.m_pModelDataManager->GetLastPadID(pPolygonRgn->miTeachImageIndex);
		pPolygonRgn->miInspectionROIID = iLastInspectionROIID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		int iLastLocalAlignID = 0;
		iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(pPolygonRgn->miTeachImageIndex);
		pPolygonRgn->miLocalAlignID = iLastLocalAlignID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_DONTCARE)
	{
		int iLastDontCareID = 0;
		iLastDontCareID = THEAPP.m_pModelDataManager->GetLastDontCareID(pPolygonRgn->miTeachImageIndex);
		pPolygonRgn->miDontCareID = iLastDontCareID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_GENERATE)
	{
		int iLastGenerateID = 0;
		iLastGenerateID = THEAPP.m_pModelDataManager->GetLastGenerateID(pPolygonRgn->miTeachImageIndex);
		pPolygonRgn->miGenerateID = iLastGenerateID + 1;
	}

	if (pPolygonRgn->miInspectionType == INSPECTION_TYPE_COLOR)
	{
		int iLastColorInfoID = 0;
		iLastColorInfoID = THEAPP.m_pModelDataManager->GetLastColorInfoID(pPolygonRgn->miTeachImageIndex);
		pPolygonRgn->miColorInfoID = iLastColorInfoID + 1;
	}

	THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pPolygonRgn);

	m_iDisplayChannelType = CHANNEL_TYPE_COLOR;
	m_bPolygonMode = FALSE;
	m_iPolygonCnt = 0;

	THEAPP.m_pInspectAdminViewToolbarDlg->SetToolBarStateDraw();

	//UpdateHistogramImage();
	//UpdateView(TRUE);
	//InvalidateRect(&m_ClientRect, FALSE);
	//UpdateHistogram(pPolygonRgn);

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::DeleteAutoCreateROI()
{
	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("용도가 변경되지 않은 ROI가 삭제됩니다. 진행하시겠습니까?");
	else
		strMessageBox.Format("ROIs with unchanged usage will be deleted. Continue?");
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	int iCurTeachImageIndex;
	iCurTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;

	mpLastSelectedRegion = NULL;

	for (int iIndex = (iNoInspectROI - 1); iIndex >= 0; iIndex--)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_AUTO_UNDEFINED)
		{
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != iCurTeachImageIndex)
				continue;

			THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(pInspectROIRgn, FALSE);
		}
	}

	mpActiveTRegion = NULL;

	InvalidateRect(&m_ClientRect, TRUE);
}

// Region의 면적과 중심 좌표를 계산하는 함수
struct RegionData {
	HRegion region;
	double area;
	double row_center;
	double col_center;

	RegionData(const HRegion& reg) {
		HTuple temp_area, temp_row, temp_col;
		AreaCenter(reg, &temp_area, &temp_row, &temp_col);
		area = temp_area.D();
		row_center = temp_row.D();
		col_center = temp_col.D();
		region = reg;
	}
};

// 두 Region이 비슷한지 확인하는 함수
bool AreRegionsSimilar(const RegionData& r1, const RegionData& r2, double area_threshold = 20000.0, double distance_threshold = 100.0) {
	// 면적 차이 비교
	double area_diff = std::abs(r1.area - r2.area);
	if (area_diff > area_threshold) return false;

	// 중심 좌표 거리 비교
	double distance = std::sqrt(std::pow(r1.row_center - r2.row_center, 2) +
								std::pow(r1.col_center - r2.col_center, 2));
	return distance <= distance_threshold;
}

void MergeSimilarRegions(std::list<HRegion>& regions, double min_area_threshold) {
	// Step 1: Region 데이터를 준비
	std::list<RegionData> region_data_list;
	for (const auto& region : regions) {
		region_data_list.emplace_back(region);
	}

	// Step 2: 비슷한 Region 그룹화 및 대체
	std::list<HRegion> merged_regions;

	while (!region_data_list.empty()) {
		// 기준 Region을 가져옴
		RegionData current = region_data_list.front();
		region_data_list.pop_front();

		std::list<RegionData> similar_regions = { current };

		// 기준 Region과 비슷한 Region 찾기
		for (auto it = region_data_list.begin(); it != region_data_list.end();) {
			if (AreRegionsSimilar(current, *it)) {
				similar_regions.push_back(*it);
				it = region_data_list.erase(it);
			}
			else {
				++it;
			}
		}

		// 그룹에서 가장 큰 Region을 선택
		auto largest_region = std::max_element(
			similar_regions.begin(), similar_regions.end(),
			[](const RegionData& r1, const RegionData& r2) { return r1.area < r2.area; });

		merged_regions.push_back(largest_region->region);
	}

	// Step 3: 리전 크기 기준으로 필터링
	std::list<HRegion> filtered_regions;
	for (const auto& region : merged_regions) {
		double area, row, col;
		area = region.AreaCenter(&row, &col); // 면적 계산
		if (area >= min_area_threshold) {    // 최소 면적 조건 확인
			filtered_regions.push_back(region);
		}
	}

	// Step 4: 최종 결과를 원본 리스트에 저장
	regions = std::move(filtered_regions);
}

/*
Region 요소 분리:

	Connection() 함수로 각 Region을 세부 요소로 분리.
	분리된 HRegion 요소 각각에 대해 RegionData를 생성.

비교 범위 확장:

	분리된 요소별로 유사성을 판단하여 병합.

정확도 개선:

	연결된 작은 Region들까지 비교 및 병합할 수 있도록 개선.
*/
void MergeSimilarRegionsV2(std::list<HRegion>& regions, double min_area_threshold) {
	// Step 1: Region 데이터를 준비
	std::list<RegionData> region_data_list;

	for (const auto& region : regions) {
		// 각 Region을 개별 요소로 분리
		HRegion connected_regions = region.Connection();
		HTuple num_regions = connected_regions.CountObj();

		for (int i = 1; i <= num_regions.I(); ++i) {
			HRegion single_region = connected_regions.SelectObj(i);
			region_data_list.emplace_back(single_region);
		}
	}

	// Step 2: 비슷한 Region 그룹화 및 대체
	std::list<HRegion> merged_regions;

	while (!region_data_list.empty()) {
		// 기준 Region을 가져옴
		RegionData current = region_data_list.front();
		region_data_list.pop_front();

		std::list<RegionData> similar_regions = { current };

		// 기준 Region과 비슷한 Region 찾기
		for (auto it = region_data_list.begin(); it != region_data_list.end();) {
			if (AreRegionsSimilar(current, *it)) {
				similar_regions.push_back(*it);
				it = region_data_list.erase(it);
			}
			else {
				++it;
			}
		}

		// 그룹에서 가장 큰 Region을 선택
		auto largest_region = std::max_element(
			similar_regions.begin(), similar_regions.end(),
			[](const RegionData& r1, const RegionData& r2) { return r1.area < r2.area; });

		merged_regions.push_back(largest_region->region);
	}

	// Step 3: 리전 크기 기준으로 필터링
	std::list<HRegion> filtered_regions;
	for (const auto& region : merged_regions) {
		HTuple area, row, col;
		AreaCenter(region, &area, &row, &col); // 면적 계산
		if (area >= min_area_threshold) {     // 최소 면적 조건 확인
			filtered_regions.push_back(region);
		}
	}

	// Step 4: 최종 결과를 원본 리스트에 저장
	regions = std::move(filtered_regions);
}

// 수정된 MergeSimilarRegionsV3 함수
/*
세부 요소 병합:

	그룹 내 모든 요소를 Union2로 병합하여 단일 Region 생성.
	그룹화된 요소 전체를 병합하여 결과 Region 생성.

비교 정확도 향상:

	세부 요소별 병합을 통해 정확도 증가.
	그룹 내 유사 요소들 간의 병합 결과를 최적화.

코드 안정성:

	빈 Region 생성 방식을 개선하여 안정성 강화.
	병합 과정에서 발생 가능한 오류를 줄임.
*/
void MergeSimilarRegionsV3(std::list<HRegion>& regions, double min_area_threshold) {
	// Step 1: Region 데이터를 준비
	std::list<RegionData> region_data_list;

	for (const auto& region : regions) {
		// 각 Region을 개별 요소로 분리
		HRegion connected_regions = region.Connection();
		HTuple num_regions = connected_regions.CountObj();

		for (int i = 1; i <= num_regions.I(); ++i) {
			HRegion single_region = connected_regions.SelectObj(i);
			region_data_list.emplace_back(single_region);
		}
	}

	// Step 2: 비슷한 Region 그룹화 및 대체
	std::list<HRegion> merged_regions;

	while (!region_data_list.empty()) {
		// 기준 Region을 가져옴
		RegionData current = region_data_list.front();
		region_data_list.pop_front();

		std::list<RegionData> similar_regions = { current };

		// 기준 Region과 비슷한 Region 찾기
		for (auto it = region_data_list.begin(); it != region_data_list.end();) {
			// 기준 Region과 비교
			if (AreRegionsSimilar(current, *it)) {
				similar_regions.push_back(*it);
				it = region_data_list.erase(it);
			}
			else {
				++it;
			}
		}

		// 그룹에서 각 요소별로 비교 수행
		HRegion merged_group;
		GenEmptyObj(&merged_group); // 빈 Region 초기화

		for (const auto& region_data : similar_regions) {
			Union2(merged_group, region_data.region, &merged_group);
		}

		merged_regions.push_back(merged_group);
	}

	// Step 3: 리전 크기 기준으로 필터링
	std::list<HRegion> filtered_regions;
	for (const auto& region : merged_regions) {
		HTuple area, row, col;
		AreaCenter(region, &area, &row, &col); // 면적 계산
		if (area >= min_area_threshold) {     // 최소 면적 조건 확인
			filtered_regions.push_back(region);
		}
	}

	// Step 4: 최종 결과를 원본 리스트에 저장
	regions = std::move(filtered_regions);
}

// 작은 리전을 제거하는 함수
HRegion RemoveSmallRegions(const HRegion& inputRegion, double minArea)
{
	HRegion filteredRegion;
	try
	{
		// 연결된 영역으로 분리
		HRegion connectedRegions = inputRegion.Connection();
		HTuple numRegions = connectedRegions.CountObj();

		// 결과 저장용 빈 Region 생성
		GenEmptyRegion(&filteredRegion);

		// 모든 Region 비교 및 필터링
		for (int i = 1; i <= numRegions.I(); ++i)
		{
			HRegion region1 = connectedRegions.SelectObj(i); // 첫 번째 Region 선택

			// 첫 번째 Region이 너무 작으면 건너뜀
			HTuple area1, row1, col1;
			AreaCenter(region1, &area1, &row1, &col1);

			if (area1 < minArea)
				continue; // 최소 크기 조건에 미달 시 스킵

			bool isContained = false;

			// 다른 Region들과 포함 관계 비교
			for (int j = 1; j <= numRegions.I(); ++j)
			{
				if (i == j) continue; // 같은 Region은 비교하지 않음

				HRegion region2 = connectedRegions.SelectObj(j); // 두 번째 Region 선택

				// region1이 region2에 포함되어 있는지 확인
				HRegion intersection = region1.Intersection(region2);
				HTuple intersectionArea;
				AreaCenter(intersection, &intersectionArea, nullptr, nullptr);

				// intersection이 region1과 동일하면 포함됨
				if (intersectionArea == area1)
				{
					isContained = true;
					break;
				}
			}

			// 포함되지 않은 Region만 결과에 추가
			if (!isContained)
			{
				Union2(filteredRegion, region1, &filteredRegion);
			}
		}
	}
	catch (HOperatorException& ex)
	{
		// Halcon 예외 처리
		CString errorMsg;
		errorMsg.Format(_T("Halcon 오류 발생: %s\n"), ex.ErrorMessage().Text());
		OutputDebugString(errorMsg);
		throw; // 예외를 호출자에게 다시 전달
	}

	return filteredRegion;
}

void CInspectAdminViewDlg::SendImageToServer(int imageIndex, int iModelIndex, int channelType)
{
	HImage imageToSend = THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[channelType][imageIndex];
	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[channelType][imageIndex])) // 이미지 초기화 확인
	{
#ifdef SNZEROMQ
		using namespace synapse::network::cpp::zeromq;

		// loopback test
		time_t send_sam2_loop_back_time = std::time(nullptr);
		common::models::SNZeroMQLoopback send_sam2_loop_back(THEAPP.m_zeromq_client_id, send_sam2_loop_back_time);
		common::models::SNZeroMQLoopback receive_sam2_loop_back;
		if (!THEAPP.m_zeromq_client.TryLoopback(common::enums::FeatureType::SAM2, send_sam2_loop_back, receive_sam2_loop_back)) {
			return;
		}

		// 이미지 형 변경
		auto mat_image = THEAPP.m_pGFunction->convert_hobject_to_mat(imageToSend);
		std::vector<uint8_t> encoded_image;
		if (!cv::imencode(".png", mat_image, encoded_image)) {
			return;
		}

		// SAM2 요청 데이터 생성
		common::enums::Sam2ModelType model_type = static_cast<common::enums::Sam2ModelType>(iModelIndex + 2);
		common::models::sam2::RequestSam2 request_sam2(THEAPP.m_zeromq_client_id, common::enums::Sam2DataType::Data, model_type, "");
		common::models::sam2::ResponseSam2 response_sam2;
		uint8_t* response_sam2_data;
		size_t response_sam2_data_size;

		// 이미지 전송 & 결과 수신
		THEAPP.m_zeromq_client.TrySendSAM2ReRequest(request_sam2, encoded_image.data(), encoded_image.size(), response_sam2, response_sam2_data, response_sam2_data_size);

		std::list<HRegion> regions;

		// byte 데이터 역직렬화
		auto vector_data_sets = client::service::helpers::ConvertHelper::BytesToSAM2Points2(response_sam2_data, response_sam2_data_size);

		for (std::list<std::list<std::pair<int, int>>> inner_point_set : vector_data_sets) {
			// 결과 저장용
			std::list<std::list<std::pair<int, int>>> external_contours;
			std::list<std::list<std::pair<int, int>>> internal_contours;

			// 복구 수행
			client::service::helpers::ContourProcessHelper::ClassifyContours(inner_point_set, external_contours, internal_contours);

			HRegion result_region;
			THEAPP.m_pGFunction->convert_list_2_region(external_contours, internal_contours, result_region);

			result_region = RemoveSmallRegions(result_region, 1000); // 너무 작은 요소 삭제
			regions.push_back(result_region);
		}
		MergeSimilarRegionsV3(regions, 5000); // 비슷한 요소 합치기

		for (HRegion region : regions) {
			THEAPP.m_pInspectAdminViewDlg->AddAutoDrawROI(region); // InspectAdminViewDlg의 SetRegion 함수 호출
		}
#endif // SNZEROMQ
		// AddRegionsToListBox();
		// 이미지 전송

		AfxMessageBox(_T("Image sent to server successfully!"), MB_SYSTEMMODAL);
	}
	else
	{
		AfxMessageBox(_T("Error: Image is not initialized."), MB_SYSTEMMODAL);
	}
}
