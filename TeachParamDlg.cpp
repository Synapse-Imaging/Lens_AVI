// TeachParamDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TeachParamDlg.h"
#include "afxdialogex.h"


// CTeachParamDlg 대화 상자입니다.

CTeachParamDlg* CTeachParamDlg::m_pInstance = NULL;

CTeachParamDlg* CTeachParamDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance)
	{
		m_pInstance = new CTeachParamDlg();
		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_TEACH_PARAM_DLG, pFrame->GetActiveView());
			if (bShowFlag)
				m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CTeachParamDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CTeachParamDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CTeachParamDlg, CDialog)

CTeachParamDlg::CTeachParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachParamDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT + 120, VIEW1_DLG3_TOP + 50 + 50 + VIEW1_DLG3_HEIGHT + 270 - 100, VIEW1_DLG3_WIDTH - 150, VIEW1_DLG3_HEIGHT - 20 + 25 + 100);

	m_iPrevAlgorithmTabIndex = 0;
	m_iCurrentAlgorithmTabIdx = 0;

	m_bCheckUseInspect = FALSE;

	m_bCheckUseFilter1 = FALSE;
	m_bCheckUseFilter2 = FALSE;
	m_bCheckUseFilter3 = FALSE;
	m_bCheckUseFilter4 = FALSE;

	m_bCheckAIUseConsiderImageNo = FALSE;
	m_bCheckAIUseConsiderDefectCode = FALSE;

	m_bCheckUseInspectArea1 = TRUE;
	m_iEditArea1Margin1 = -2;
	m_iEditArea1Margin2 = 2;

	m_bCheckUseInspectArea2 = FALSE;
	m_iEditArea2Margin1 = 0;
	m_iEditArea2Margin2 = 0;

	m_bCheckUseInspectArea3 = FALSE;
	m_iEditArea3Margin1 = 0;
	m_iEditArea3Margin2 = 0;
}

CTeachParamDlg::~CTeachParamDlg()
{
}

void CTeachParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_ALGORITHM_TAB1, m_ButtonSetAlgorithmTab1);
	DDX_Control(pDX, IDC_BUTTON_ALGORITHM_TAB2, m_ButtonSetAlgorithmTab2);
	DDX_Control(pDX, IDC_BUTTON_ALGORITHM_TAB3, m_ButtonSetAlgorithmTab3);
	DDX_Control(pDX, IDC_BUTTON_ALGORITHM_TAB4, m_ButtonSetAlgorithmTab4);
	DDX_Control(pDX, IDC_BUTTON_ALGORITHM_TAB5, m_ButtonSetAlgorithmTab5);
	DDX_Control(pDX, IDC_BUTTON_ALGORITHM_TAB6, m_ButtonSetAlgorithmTab6);
	DDX_Control(pDX, IDC_BUTTON_ALGORITHM_TAB7, m_ButtonSetAlgorithmTab7);

	DDX_Control(pDX, IDC_STATIC_TEACHPARAMDLG1, m_staticInspectApply);
	DDX_Check(pDX, IDC_CHECK_USE_INSPECT, m_bCheckUseInspect);

	DDX_Control(pDX, IDC_STATIC_TEACHPARAMDLG6, m_staticDefectType);
	DDX_Control(pDX, IDC_COMBO_DEFECT_TYPE, m_ComboDefectType);

	DDX_Check(pDX, IDC_CHECK_USE_FILTER1, m_bCheckUseFilter1);
	DDX_Check(pDX, IDC_CHECK_USE_FILTER2, m_bCheckUseFilter2);
	DDX_Check(pDX, IDC_CHECK_USE_FILTER3, m_bCheckUseFilter3);
	DDX_Check(pDX, IDC_CHECK_USE_FILTER4, m_bCheckUseFilter4);

	DDX_Check(pDX, IDC_CHECK_AISETUP_CONSIDER_ImageNo, m_bCheckAIUseConsiderImageNo);
	DDX_Check(pDX, IDC_CHECK_AISETUP_CONSIDER_DEFECT_CODE, m_bCheckAIUseConsiderDefectCode);

	DDX_Check(pDX, IDC_CHECK_USE_INSPECT_AREA1, m_bCheckUseInspectArea1);
	DDX_Text(pDX, IDC_EDIT_AREA1_MARGIN1, m_iEditArea1Margin1);
	DDX_Text(pDX, IDC_EDIT_AREA1_MARGIN2, m_iEditArea1Margin2);

	DDX_Check(pDX, IDC_CHECK_USE_INSPECT_AREA2, m_bCheckUseInspectArea2);
	DDX_Text(pDX, IDC_EDIT_AREA2_MARGIN1, m_iEditArea2Margin1);
	DDX_Text(pDX, IDC_EDIT_AREA2_MARGIN2, m_iEditArea2Margin2);

	DDX_Check(pDX, IDC_CHECK_USE_INSPECT_AREA3, m_bCheckUseInspectArea3);
	DDX_Text(pDX, IDC_EDIT_AREA3_MARGIN1, m_iEditArea3Margin1);
	DDX_Text(pDX, IDC_EDIT_AREA3_MARGIN2, m_iEditArea3Margin2);

	DDX_Control(pDX, IDC_BUTTON_INSPECT_AREA_UPDATE, m_ButtonInspectAreaUpdate);

	DDX_Control(pDX, IDC_BUTTON_SET_ALGORITHM_PARM, m_ButtonSetAlgorithmParam);

	DDX_Control(pDX, IDC_BUTTON_ALGORITHM_TEST_TAB, m_ButtonAlgorithmTestTab);
	DDX_Control(pDX, IDC_BUTTON_ALGORITHM_TEST_ROI, m_ButtonAlgorithmTestRoi);
	DDX_Control(pDX, IDC_BUTTON_ALGORITHM_TEST_IMAGE, m_ButtonAlgorithmTestImage);
	DDX_Control(pDX, IDC_BUTTON_AISETUP_COLOR_1, m_ButtonSetAISet1);
	DDX_Control(pDX, IDC_BUTTON_AISETUP_COLOR_2, m_ButtonSetAISet2);
	DDX_Control(pDX, IDC_BUTTON_AISETUP_COLOR_3, m_ButtonSetAISet3);

	DDX_Control(pDX, IDC_STATIC_INSPECT_STATUS, m_staticTabInspectApply);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_STATUS_1, m_ButtonInspectStatus[0]);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_STATUS_2, m_ButtonInspectStatus[1]);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_STATUS_3, m_ButtonInspectStatus[2]);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_STATUS_4, m_ButtonInspectStatus[3]);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_STATUS_5, m_ButtonInspectStatus[4]);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_STATUS_6, m_ButtonInspectStatus[5]);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_STATUS_7, m_ButtonInspectStatus[6]);
}


BEGIN_MESSAGE_MAP(CTeachParamDlg, CDialog)
	ON_MESSAGE(UM_TS_ROI_SELECTED, OnEventROISelected)
	ON_MESSAGE(UM_TS_ROI_INSPECTION_TAB_SAVE, OnEventROIInspectionTabSave)

	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM_TAB1, &CTeachParamDlg::OnBnClickedButtonAlgorithmTab1)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM_TAB2, &CTeachParamDlg::OnBnClickedButtonAlgorithmTab2)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM_TAB3, &CTeachParamDlg::OnBnClickedButtonAlgorithmTab3)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM_TAB4, &CTeachParamDlg::OnBnClickedButtonAlgorithmTab4)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM_TAB5, &CTeachParamDlg::OnBnClickedButtonAlgorithmTab5)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM_TAB6, &CTeachParamDlg::OnBnClickedButtonAlgorithmTab6)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM_TAB7, &CTeachParamDlg::OnBnClickedButtonAlgorithmTab7)
	ON_BN_CLICKED(IDC_CHECK_USE_INSPECT, &CTeachParamDlg::OnBnClickedCheckUseInspect)
	ON_BN_CLICKED(IDC_CHECK_USE_FILTER1, &CTeachParamDlg::OnBnClickedCheckUseFilter1)
	ON_BN_CLICKED(IDC_CHECK_USE_FILTER2, &CTeachParamDlg::OnBnClickedCheckUseFilter2)
	ON_BN_CLICKED(IDC_CHECK_USE_FILTER3, &CTeachParamDlg::OnBnClickedCheckUseFilter3)
	ON_BN_CLICKED(IDC_CHECK_USE_FILTER4, &CTeachParamDlg::OnBnClickedCheckUseFilter4)
	ON_BN_CLICKED(IDC_CHECK_AISETUP_CONSIDER_ImageNo, &CTeachParamDlg::OnBnClickedCheckAIUseConsiderImageNo)
	ON_BN_CLICKED(IDC_CHECK_AISETUP_CONSIDER_DEFECT_CODE, &CTeachParamDlg::OnBnClickedCheckAIUseConsiderDefectCode)
	ON_BN_CLICKED(IDC_BUTTON_INSPECT_AREA_UPDATE, &CTeachParamDlg::OnBnClickedButtonInspectAreaUpdate)
	ON_BN_CLICKED(IDC_BUTTON_SET_ALGORITHM_PARM, &CTeachParamDlg::OnBnClickedButtonSetAlgorithmParm)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM_TEST_TAB, &CTeachParamDlg::OnBnClickedButtonAlgorithmTestTab)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM_TEST_ROI, &CTeachParamDlg::OnBnClickedButtonAlgorithmTestRoi)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM_TEST_IMAGE, &CTeachParamDlg::OnBnClickedButtonAlgorithmTestImage)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_COMPARE_TRAIN_TAB, &CTeachParamDlg::OnBnClickedButtonImageCompareTrainTab)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_COMPARE_TRAIN_ROI, &CTeachParamDlg::OnBnClickedButtonImageCompareTrainRoi)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_COMPARE_TRAIN_IMAGE, &CTeachParamDlg::OnBnClickedButtonImageCompareTrainImage)
END_MESSAGE_MAP()


// CTeachParamDlg 메시지 처리기입니다.

int CTeachParamDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CTeachParamDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	THEAPP.m_pInspectAdminViewDlg->delegateROISelected -= this;
	THEAPP.m_pInspectAdminViewDlg->delegateCurROITabParamSave -= this;
}


BOOL CTeachParamDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CTeachParamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_ButtonSetAlgorithmTab1.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(255, 255, 255), RGB(255, 150, 150), 0, 0);
	m_ButtonSetAlgorithmTab2.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(1, 1, 1), RGB(245, 245, 245), 0, 0);
	m_ButtonSetAlgorithmTab3.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(1, 1, 1), RGB(245, 245, 245), 0, 0);
	m_ButtonSetAlgorithmTab4.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(1, 1, 1), RGB(245, 245, 245), 0, 0);
	m_ButtonSetAlgorithmTab5.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(1, 1, 1), RGB(245, 245, 245), 0, 0);
	m_ButtonSetAlgorithmTab6.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(1, 1, 1), RGB(245, 245, 245), 0, 0);
	m_ButtonSetAlgorithmTab7.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(1, 1, 1), RGB(245, 245, 245), 0, 0);

	m_staticInspectApply.Init_Ctrl(_T("Arial Black"), 8, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));

	m_staticDefectType.Init_Ctrl(_T("Arial Black"), 8, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ComboDefectType.SetCurSel(0);

	m_ButtonInspectAreaUpdate.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_TeachParamGridCtrl.AttachGrid(this, IDC_STATIC_ALGORITHM_LIST);

	m_ButtonSetAlgorithmParam.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	m_ButtonAlgorithmTestTab.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_ButtonAlgorithmTestRoi.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_ButtonAlgorithmTestImage.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_staticTabInspectApply.Init_Ctrl(_T("Arial Black"), 8, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		m_ButtonInspectStatus[i].Init_Ctrl(_T("Arial"), 9, FALSE, RGB(1, 1, 1), RGB(1, 1, 1), 0, 0);

	THEAPP.m_pInspectAdminViewDlg->delegateROISelected += new CEventStub(this, UM_TS_ROI_SELECTED);
	THEAPP.m_pInspectAdminViewDlg->delegateCurROITabParamSave += new CEventStub(this, UM_TS_ROI_INSPECTION_TAB_SAVE);

#pragma region AI setup button color init
	m_ButtonSetAISet1.EnableWindowsTheming(FALSE);
	m_ButtonSetAISet1.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	m_ButtonSetAISet1.m_bTransparent = false;
	m_ButtonSetAISet1.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
	m_ButtonSetAISet2.EnableWindowsTheming(FALSE);
	m_ButtonSetAISet2.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	m_ButtonSetAISet2.m_bTransparent = false;
	m_ButtonSetAISet2.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
	m_ButtonSetAISet3.EnableWindowsTheming(FALSE);
	m_ButtonSetAISet3.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	m_ButtonSetAISet3.m_bTransparent = false;
	m_ButtonSetAISet3.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
#pragma endregion

	ChangeLanguage();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTeachParamDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB1, _T("탭 1"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB2, _T("탭 2"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB3, _T("탭 3"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB4, _T("탭 4"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB5, _T("탭 5"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB6, _T("탭 6"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB7, _T("탭 7"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG1, _T("검사 적용"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG2, _T("SVM 적용"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG3, _T("1차망 적용"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG4, _T("2차망 적용"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG5, _T("3차망 적용"));

		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG6, _T("불량 항목"));
		SetDlgItemText(IDC_GROUPBOX_TEACHPARAMDLG1, _T("검사 영역"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG7, _T("마진(픽셀)"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG8, _T("마진(픽셀)"));
		SetDlgItemText(IDC_CHECK_USE_INSPECT_AREA1, _T("영역 1"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG9, _T("~"));
		SetDlgItemText(IDC_CHECK_USE_INSPECT_AREA2, _T("영역 2"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG10, _T("~"));
		SetDlgItemText(IDC_CHECK_USE_INSPECT_AREA3, _T("영역 3"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG11, _T("~"));
		SetDlgItemText(IDC_BUTTON_INSPECT_AREA_UPDATE, _T("검사 영역 업데이트"));
		SetDlgItemText(IDC_STATIC_INSPECT_STATUS, _T("검사 적용 상태"));

		m_TeachParamGridCtrl.QuickSetText(0, -1, "  알고리즘  ");
		m_TeachParamGridCtrl.QuickSetText(1, -1, "  밝은불량  ");
		m_TeachParamGridCtrl.QuickSetText(2, -1, " 어두운불량 ");

		m_TeachParamGridCtrl.QuickSetText(0, 0, "     영상 전처리      ");
		m_TeachParamGridCtrl.QuickSetText(0, 1, "   영상 필터링    ");
		m_TeachParamGridCtrl.QuickSetText(0, 2, " ROI 얼라인 ");
		m_TeachParamGridCtrl.QuickSetText(0, 3, " 비등방 ROI 얼라인 ");
		m_TeachParamGridCtrl.QuickSetText(0, 4, " 파트 유무 체크 ");
		m_TeachParamGridCtrl.QuickSetText(0, 5, " 엣지 측정 ");
		m_TeachParamGridCtrl.QuickSetText(0, 6, " 로컬 얼라인 연결 ");
		m_TeachParamGridCtrl.QuickSetText(0, 7, " 검사 제외 영역 연결 ");
		m_TeachParamGridCtrl.QuickSetText(0, 8, " 검사 영역 생성 연결 ");
		m_TeachParamGridCtrl.QuickSetText(0, 9, " 고정 Threshold ");
		m_TeachParamGridCtrl.QuickSetText(0, 10, " 동적 Threshold ");
		m_TeachParamGridCtrl.QuickSetText(0, 11, " 균일도 검사 ");
		m_TeachParamGridCtrl.QuickSetText(0, 12, " Hysteresis Threshold ");
		m_TeachParamGridCtrl.QuickSetText(0, 13, " 바코드 ");
		m_TeachParamGridCtrl.QuickSetText(0, 14, " 바코드 품질 ");
		m_TeachParamGridCtrl.QuickSetText(0, 15, " 검사 제외 블랍 ");
		m_TeachParamGridCtrl.QuickSetText(0, 16, " 이중 블랍 ");
		m_TeachParamGridCtrl.QuickSetText(0, 17, " 영상 비교 ");
		m_TeachParamGridCtrl.QuickSetText(0, 18, " 요철 영상 ");
		m_TeachParamGridCtrl.QuickSetText(0, 19, " 기울기 측정");
		m_TeachParamGridCtrl.QuickSetText(0, 20, " 거리 측정 ");
		m_TeachParamGridCtrl.QuickSetText(0, 21, " 벌어짐 측정 ");
		m_TeachParamGridCtrl.QuickSetText(0, 22, " 문자 인식 ");
		m_TeachParamGridCtrl.QuickSetText(0, 23, " 라인 정렬 ");
		m_TeachParamGridCtrl.QuickSetText(0, 24, " 경계 검사 ");

		SetDlgItemText(IDC_BUTTON_SET_ALGORITHM_PARM, _T("알고리즘 설정"));
		SetDlgItemText(IDC_GROUPBOX_TEACHPARAMDLG2, _T("영상 비교 학습"));
		SetDlgItemText(IDC_BUTTON_IMAGE_COMPARE_TRAIN_TAB, _T("탭"));
		SetDlgItemText(IDC_BUTTON_IMAGE_COMPARE_TRAIN_ROI, _T("ROI"));
		SetDlgItemText(IDC_BUTTON_IMAGE_COMPARE_TRAIN_IMAGE, _T("영상"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TEST_TAB, _T("탭\n검사"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TEST_ROI, _T("ROI\n전체 검사"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TEST_IMAGE, _T("영상\n전체 검사"));
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB1, _T("Tab 1"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB2, _T("Tab 2"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB3, _T("Tab 3"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB4, _T("Tab 4"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB5, _T("Tab 5"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB6, _T("Tab 6"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TAB7, _T("Tab 7"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG1, _T("Use inspect"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG2, _T("Use SVM"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG3, _T("Use 1st Filter"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG4, _T("Use 2nd Filter"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG5, _T("Use 3rd Filter"));

		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG6, _T("Defect name"));
		SetDlgItemText(IDC_GROUPBOX_TEACHPARAMDLG1, _T("Inspect area"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG7, _T("Margin(pixel)"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG8, _T("Margin(pixel)"));
		SetDlgItemText(IDC_CHECK_USE_INSPECT_AREA1, _T("Area 1"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG9, _T("~"));
		SetDlgItemText(IDC_CHECK_USE_INSPECT_AREA2, _T("Area 2"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG10, _T("~"));
		SetDlgItemText(IDC_CHECK_USE_INSPECT_AREA3, _T("Area 3"));
		SetDlgItemText(IDC_STATIC_TEACHPARAMDLG11, _T("~"));
		SetDlgItemText(IDC_BUTTON_INSPECT_AREA_UPDATE, _T("Inspect area update"));
		SetDlgItemText(IDC_STATIC_INSPECT_STATUS, _T("Inspect status"));

		m_TeachParamGridCtrl.QuickSetText(0, -1, " Algorithm ");
		m_TeachParamGridCtrl.QuickSetText(1, -1, "   Bright  ");
		m_TeachParamGridCtrl.QuickSetText(2, -1, "    Dark   ");

		m_TeachParamGridCtrl.QuickSetText(0, 0, " Image preprocessing ");
		m_TeachParamGridCtrl.QuickSetText(0, 1, " Image filtering ");
		m_TeachParamGridCtrl.QuickSetText(0, 2, " ROI align ");
		m_TeachParamGridCtrl.QuickSetText(0, 3, " Aniso ROI align ");
		m_TeachParamGridCtrl.QuickSetText(0, 4, " Part check ");
		m_TeachParamGridCtrl.QuickSetText(0, 5, " Edge measure ");
		m_TeachParamGridCtrl.QuickSetText(0, 6, " Local align apply ");
		m_TeachParamGridCtrl.QuickSetText(0, 7, " Dont care area apply ");
		m_TeachParamGridCtrl.QuickSetText(0, 8, " Area generate apply ");
		m_TeachParamGridCtrl.QuickSetText(0, 9, " Fixed Threshold ");
		m_TeachParamGridCtrl.QuickSetText(0, 10, " Dynamic Threshold ");
		m_TeachParamGridCtrl.QuickSetText(0, 11, " Uniformity ");
		m_TeachParamGridCtrl.QuickSetText(0, 12, " Hysteresis Threshold ");
		m_TeachParamGridCtrl.QuickSetText(0, 13, " Barcode ");
		m_TeachParamGridCtrl.QuickSetText(0, 14, " Barcode quality ");
		m_TeachParamGridCtrl.QuickSetText(0, 15, " Exclude blob ");
		m_TeachParamGridCtrl.QuickSetText(0, 16, " Dual blob ");
		m_TeachParamGridCtrl.QuickSetText(0, 17, " Image compare ");
		m_TeachParamGridCtrl.QuickSetText(0, 18, " Shape image ");
		m_TeachParamGridCtrl.QuickSetText(0, 19, " Tilt measure");
		m_TeachParamGridCtrl.QuickSetText(0, 20, " Distance measure ");
		m_TeachParamGridCtrl.QuickSetText(0, 21, " Gap measure ");
		m_TeachParamGridCtrl.QuickSetText(0, 22, " OCR ");
		m_TeachParamGridCtrl.QuickSetText(0, 23, " Line fitting ");
		m_TeachParamGridCtrl.QuickSetText(0, 24, " Boundary Inspection ");

		SetDlgItemText(IDC_BUTTON_SET_ALGORITHM_PARM, _T("Set algorithm"));
		SetDlgItemText(IDC_GROUPBOX_TEACHPARAMDLG2, _T("Image compare train"));
		SetDlgItemText(IDC_BUTTON_IMAGE_COMPARE_TRAIN_TAB, _T("Tab"));
		SetDlgItemText(IDC_BUTTON_IMAGE_COMPARE_TRAIN_ROI, _T("ROI"));
		SetDlgItemText(IDC_BUTTON_IMAGE_COMPARE_TRAIN_IMAGE, _T("Image"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TEST_TAB, _T("Tab\nInspection"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TEST_ROI, _T("ROI\nInspection"));
		SetDlgItemText(IDC_BUTTON_ALGORITHM_TEST_IMAGE, _T("Image\nInspection"));
	}

	m_TeachParamGridCtrl.RedrawAll();
}


void CTeachParamDlg::UpdateUsedAlgorithmList(int iTabIdx)
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	CAlgorithmParam AlgorithmParam;
	AlgorithmParam = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx];

	if (AlgorithmParam.m_bUseImageProcess)
		m_TeachParamGridCtrl.QuickSetText(1, 0, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 0, "");

	if (AlgorithmParam.m_bUseImageProcessFilter)
		m_TeachParamGridCtrl.QuickSetText(1, 1, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 1, "");

	if (AlgorithmParam.m_bUseROIAlign)
		m_TeachParamGridCtrl.QuickSetText(1, 2, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 2, "");

	if (AlgorithmParam.m_bUseROIAnisoAlign)
		m_TeachParamGridCtrl.QuickSetText(1, 3, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 3, "");

	if (AlgorithmParam.m_bUsePartCheck)
		m_TeachParamGridCtrl.QuickSetText(1, 4, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 4, "");

	if (AlgorithmParam.m_bUseEdgeMeasure)
		m_TeachParamGridCtrl.QuickSetText(1, 5, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 5, "");

	if (AlgorithmParam.m_bUseImageProcessLocalAlign)
		m_TeachParamGridCtrl.QuickSetText(1, 6, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 6, "");

	if (AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
		m_TeachParamGridCtrl.QuickSetText(1, 7, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 7, "");

	if (AlgorithmParam.m_bUseImageProcessGenerate)
		m_TeachParamGridCtrl.QuickSetText(1, 8, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 8, "");

	if (AlgorithmParam.m_bUseBrightFixedThres)
		m_TeachParamGridCtrl.QuickSetText(1, 9, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 9, "");

	if (AlgorithmParam.m_bUseDarkFixedThres)
		m_TeachParamGridCtrl.QuickSetText(2, 9, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(2, 9, "");

	if (AlgorithmParam.m_bUseBrightDT)
		m_TeachParamGridCtrl.QuickSetText(1, 10, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 10, "");

	if (AlgorithmParam.m_bUseDarkDT)
		m_TeachParamGridCtrl.QuickSetText(2, 10, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(2, 10, "");

	if (AlgorithmParam.m_bUseBrightUniformityCheck)
		m_TeachParamGridCtrl.QuickSetText(1, 11, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 11, "");

	if (AlgorithmParam.m_bUseDarkUniformityCheck)
		m_TeachParamGridCtrl.QuickSetText(2, 11, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(2, 11, "");

	if (AlgorithmParam.m_bUseBrightHystThres)
		m_TeachParamGridCtrl.QuickSetText(1, 12, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 12, "");

	if (AlgorithmParam.m_bUseDarkHystThres)
		m_TeachParamGridCtrl.QuickSetText(2, 12, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(2, 12, "");

	if (AlgorithmParam.m_bUseBarcode)
		m_TeachParamGridCtrl.QuickSetText(1, 13, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 13, "");

	if (AlgorithmParam.m_bUsePrintQuality)
		m_TeachParamGridCtrl.QuickSetText(1, 14, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 14, "");

	if (AlgorithmParam.m_bUseDC)
		m_TeachParamGridCtrl.QuickSetText(1, 15, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 15, "");

	if (AlgorithmParam.m_bUseSurfaceDualInspection)
		m_TeachParamGridCtrl.QuickSetText(1, 16, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 16, "");

	if (AlgorithmParam.m_bUseImageCompare)
		m_TeachParamGridCtrl.QuickSetText(1, 17, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 17, "");

	if (AlgorithmParam.m_bUseShape)
		m_TeachParamGridCtrl.QuickSetText(1, 18, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 18, "");

	if (AlgorithmParam.m_bUseHomerTilt)
		m_TeachParamGridCtrl.QuickSetText(1, 19, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 19, "");

	if (AlgorithmParam.m_bUseDistanceMeasure)
		m_TeachParamGridCtrl.QuickSetText(1, 20, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 20, "");

	if (AlgorithmParam.m_bUseGapMeasure)
		m_TeachParamGridCtrl.QuickSetText(1, 21, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 21, "");

	if (AlgorithmParam.m_bUseOCR)
		m_TeachParamGridCtrl.QuickSetText(1, 22, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 22, "");

	if (AlgorithmParam.m_bUseLineFitting)
		m_TeachParamGridCtrl.QuickSetText(1, 23, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 23, "");

	if (AlgorithmParam.m_bUseBoundaryInspection)
		m_TeachParamGridCtrl.QuickSetText(1, 24, " O ");
	else
		m_TeachParamGridCtrl.QuickSetText(1, 24, "");
}

void CTeachParamDlg::UpdateInspectCheckStatus()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	CAlgorithmParam AlgorithmParam;

	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
	{
		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			m_ButtonInspectStatus[i].Set_Color(RGB(1, 1, 1), RGB(1, 1, 1));
		}
	}
	else
	{
		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			AlgorithmParam = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i];

			if (AlgorithmParam.m_bInspect)
				m_ButtonInspectStatus[i].Set_Color(RGB(255, 255, 255), RGB(1, 1, 1));
			else
				m_ButtonInspectStatus[i].Set_Color(RGB(1, 1, 1), RGB(1, 1, 1));
		}
	}
}

void CTeachParamDlg::OnBnClickedButtonAlgorithmTab1()
{
	m_iCurrentAlgorithmTabIdx = 0;

	if (m_iPrevAlgorithmTabIndex != m_iCurrentAlgorithmTabIdx)
	{
		AlgorithmParamUpDate(TRUE, TRUE, m_iPrevAlgorithmTabIndex);
		AISetUpdate(m_iCurrentAlgorithmTabIdx);
		m_iPrevAlgorithmTabIndex = m_iCurrentAlgorithmTabIdx;
	}

	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();
	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	UpdateUsedAlgorithmList(m_iCurrentAlgorithmTabIdx);

	m_ButtonSetAlgorithmTab1.Set_Color(RGB(255, 255, 255), RGB(255, 150, 150));
	m_ButtonSetAlgorithmTab2.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab3.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab4.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab5.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab6.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab7.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateInspectCheckStatus();
	m_TeachParamGridCtrl.RedrawAll();
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedButtonAlgorithmTab2()
{
	m_iCurrentAlgorithmTabIdx = 1;

	if (m_iPrevAlgorithmTabIndex != m_iCurrentAlgorithmTabIdx)
	{
		AlgorithmParamUpDate(TRUE, TRUE, m_iPrevAlgorithmTabIndex);
		AISetUpdate(m_iCurrentAlgorithmTabIdx);
		m_iPrevAlgorithmTabIndex = m_iCurrentAlgorithmTabIdx;
	}

	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();
	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	UpdateUsedAlgorithmList(m_iCurrentAlgorithmTabIdx);

	m_ButtonSetAlgorithmTab1.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab2.Set_Color(RGB(255, 255, 255), RGB(255, 150, 150));
	m_ButtonSetAlgorithmTab3.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab4.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab5.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab6.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab7.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateInspectCheckStatus();
	m_TeachParamGridCtrl.RedrawAll();
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedButtonAlgorithmTab3()
{
	m_iCurrentAlgorithmTabIdx = 2;

	if (m_iPrevAlgorithmTabIndex != m_iCurrentAlgorithmTabIdx)
	{
		AlgorithmParamUpDate(TRUE, TRUE, m_iPrevAlgorithmTabIndex);
		AISetUpdate(m_iCurrentAlgorithmTabIdx);
		m_iPrevAlgorithmTabIndex = m_iCurrentAlgorithmTabIdx;
	}

	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();
	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	UpdateUsedAlgorithmList(m_iCurrentAlgorithmTabIdx);

	m_ButtonSetAlgorithmTab1.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab2.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab3.Set_Color(RGB(255, 255, 255), RGB(255, 150, 150));
	m_ButtonSetAlgorithmTab4.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab5.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab6.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab7.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateInspectCheckStatus();
	m_TeachParamGridCtrl.RedrawAll();
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedButtonAlgorithmTab4()
{
	m_iCurrentAlgorithmTabIdx = 3;

	if (m_iPrevAlgorithmTabIndex != m_iCurrentAlgorithmTabIdx)
	{
		AlgorithmParamUpDate(TRUE, TRUE, m_iPrevAlgorithmTabIndex);
		AISetUpdate(m_iCurrentAlgorithmTabIdx);
		m_iPrevAlgorithmTabIndex = m_iCurrentAlgorithmTabIdx;
	}

	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();
	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	UpdateUsedAlgorithmList(m_iCurrentAlgorithmTabIdx);

	m_ButtonSetAlgorithmTab1.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab2.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab3.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab4.Set_Color(RGB(255, 255, 255), RGB(255, 150, 150));
	m_ButtonSetAlgorithmTab5.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab6.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab7.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateInspectCheckStatus();
	m_TeachParamGridCtrl.RedrawAll();
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedButtonAlgorithmTab5()
{
	m_iCurrentAlgorithmTabIdx = 4;

	if (m_iPrevAlgorithmTabIndex != m_iCurrentAlgorithmTabIdx)
	{
		AlgorithmParamUpDate(TRUE, TRUE, m_iPrevAlgorithmTabIndex);
		AISetUpdate(m_iCurrentAlgorithmTabIdx);
		m_iPrevAlgorithmTabIndex = m_iCurrentAlgorithmTabIdx;
	}

	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();
	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	UpdateUsedAlgorithmList(m_iCurrentAlgorithmTabIdx);

	m_ButtonSetAlgorithmTab1.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab2.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab3.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab4.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab5.Set_Color(RGB(255, 255, 255), RGB(255, 150, 150));
	m_ButtonSetAlgorithmTab6.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab7.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateInspectCheckStatus();
	m_TeachParamGridCtrl.RedrawAll();
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedButtonAlgorithmTab6()
{
	m_iCurrentAlgorithmTabIdx = 5;

	if (m_iPrevAlgorithmTabIndex != m_iCurrentAlgorithmTabIdx)
	{
		AlgorithmParamUpDate(TRUE, TRUE, m_iPrevAlgorithmTabIndex);
		AISetUpdate(m_iCurrentAlgorithmTabIdx);
		m_iPrevAlgorithmTabIndex = m_iCurrentAlgorithmTabIdx;
	}

	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();
	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	UpdateUsedAlgorithmList(m_iCurrentAlgorithmTabIdx);

	m_ButtonSetAlgorithmTab1.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab2.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab3.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab4.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab5.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab6.Set_Color(RGB(255, 255, 255), RGB(255, 150, 150));
	m_ButtonSetAlgorithmTab7.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	UpdateInspectCheckStatus();
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	m_TeachParamGridCtrl.RedrawAll();
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedButtonAlgorithmTab7()
{
	m_iCurrentAlgorithmTabIdx = 6;

	if (m_iPrevAlgorithmTabIndex != m_iCurrentAlgorithmTabIdx)
	{
		AlgorithmParamUpDate(TRUE, TRUE, m_iPrevAlgorithmTabIndex);
		AISetUpdate(m_iCurrentAlgorithmTabIdx);
		m_iPrevAlgorithmTabIndex = m_iCurrentAlgorithmTabIdx;
	}

	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();
	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	UpdateUsedAlgorithmList(m_iCurrentAlgorithmTabIdx);

	m_ButtonSetAlgorithmTab1.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab2.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab3.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab4.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab5.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab6.Set_Color(RGB(1, 1, 1), RGB(245, 245, 245));
	m_ButtonSetAlgorithmTab7.Set_Color(RGB(255, 255, 255), RGB(255, 150, 150));

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateInspectCheckStatus();
	m_TeachParamGridCtrl.RedrawAll();
	UpdateData(FALSE);
}


void CTeachParamDlg::AlgorithmParamUpDate(BOOL bFlag, BOOL bPrevTab, int iTabIdx)
{
	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	if (bFlag)	// GetParam
	{
		if (bPrevTab)
		{
			UpdateData(bFlag);

			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bInspect = m_bCheckUseInspect;

			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter1 = m_bCheckUseFilter1;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter2 = m_bCheckUseFilter2;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter3 = m_bCheckUseFilter3;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter4 = m_bCheckUseFilter4;

			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bCheckAIUseConsiderImageNo = m_bCheckAIUseConsiderImageNo;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bCheckAIUseConsiderDefectCode = m_bCheckAIUseConsiderDefectCode;

			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iDefectType = m_ComboDefectType.GetCurSel();

			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[0] = m_bCheckUseInspectArea1;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[0] = m_iEditArea1Margin1;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[0] = m_iEditArea1Margin2;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[1] = m_bCheckUseInspectArea2;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[1] = m_iEditArea2Margin1;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[1] = m_iEditArea2Margin2;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[2] = m_bCheckUseInspectArea3;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[2] = m_iEditArea3Margin1;
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[2] = m_iEditArea3Margin2;
		}
		else
		{
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bInspect = m_bCheckUseInspectTemp[iTabIdx];

			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter1 = m_bCheckUseFilter1Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter2 = m_bCheckUseFilter2Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter3 = m_bCheckUseFilter3Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter4 = m_bCheckUseFilter4Temp[iTabIdx];

			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bCheckAIUseConsiderImageNo = m_bCheckAIUseConsiderImageNoTemp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bCheckAIUseConsiderDefectCode = m_bCheckAIUseConsiderDefectCodeTemp[iTabIdx];

			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iDefectType = m_iDefectTypeTemp[iTabIdx];

			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[0] = m_bCheckUseInspectArea1Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[0] = m_iEditArea1Margin1Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[0] = m_iEditArea1Margin2Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[1] = m_bCheckUseInspectArea2Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[1] = m_iEditArea2Margin1Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[1] = m_iEditArea2Margin2Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[2] = m_bCheckUseInspectArea3Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[2] = m_iEditArea3Margin1Temp[iTabIdx];
			THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[2] = m_iEditArea3Margin2Temp[iTabIdx];
		}
	}
	else		// SetParam
	{
		UpdateDefectTypeComboBox(THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType);

		m_bCheckUseInspectTemp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bInspect;

		m_bCheckUseFilter1Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter1;
		m_bCheckUseFilter2Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter2;
		m_bCheckUseFilter3Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter3;
		m_bCheckUseFilter4Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter4;

		m_bCheckAIUseConsiderImageNoTemp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bCheckAIUseConsiderImageNo;
		m_bCheckAIUseConsiderDefectCodeTemp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bCheckAIUseConsiderDefectCode;

		m_iDefectTypeTemp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iDefectType;

		m_bCheckUseInspectArea1Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[0];
		m_iEditArea1Margin1Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[0];
		m_iEditArea1Margin2Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[0];
		m_bCheckUseInspectArea2Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[1];
		m_iEditArea2Margin1Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[1];
		m_iEditArea2Margin2Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[1];
		m_bCheckUseInspectArea3Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[2];
		m_iEditArea3Margin1Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[2];
		m_iEditArea3Margin2Temp[iTabIdx] = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[2];


		m_bCheckUseInspect = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bInspect;

		m_bCheckUseFilter1 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter1;
		m_bCheckUseFilter2 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter2;
		m_bCheckUseFilter3 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter3;
		m_bCheckUseFilter4 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseFilter4;

		m_bCheckAIUseConsiderImageNo = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bCheckAIUseConsiderImageNo;
		m_bCheckAIUseConsiderDefectCode = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bCheckAIUseConsiderDefectCode;

		m_ComboDefectType.SetCurSel(THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iDefectType);

		m_bCheckUseInspectArea1 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[0];
		m_iEditArea1Margin1 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[0];
		m_iEditArea1Margin2 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[0];
		m_bCheckUseInspectArea2 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[1];
		m_iEditArea2Margin1 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[1];
		m_iEditArea2Margin2 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[1];
		m_bCheckUseInspectArea3 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_bUseInspectArea[2];
		m_iEditArea3Margin1 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour1Margin[2];
		m_iEditArea3Margin2 = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx].m_iInspectAreaContour2Margin[2];
	}
}


void CTeachParamDlg::UpdateDefectTypeComboBox(int iDefectType)
{
	m_ComboDefectType.ResetContent();

	for (int i = 0; i < MAX_DEFECT_NAME; i++)
		m_ComboDefectType.AddString(THEAPP.m_strDefectName[i]);
}


void CTeachParamDlg::GetInspectParam(BOOL bAllTab)
{
	if (bAllTab)
	{
		for (int i = 0; i < MAX_INSPECTION_TAB; i++) {
			AlgorithmParamUpDate(TRUE, FALSE, i);
			AISetUpdate(i);
		}
	}

	AlgorithmParamUpDate(TRUE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
}


LRESULT CTeachParamDlg::OnEventROISelected(WPARAM wParam, LPARAM lParam)
{
	int iSelectedROITabIndex;

	iSelectedROITabIndex = (int)wParam;

	if (iSelectedROITabIndex != THEAPP.m_pTabControlDlg->m_iCurrentTab)
		return 1;

	SetParamData(TRUE);

	return 1;
}


LRESULT CTeachParamDlg::OnEventROIInspectionTabSave(WPARAM wParam, LPARAM lParam)
{
	int iSelectedROITabIndex;

	iSelectedROITabIndex = (int)wParam;

	if (iSelectedROITabIndex != THEAPP.m_pTabControlDlg->m_iCurrentTab)
		return 1;

	AlgorithmParamUpDate(TRUE, TRUE, m_iPrevAlgorithmTabIndex);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);

	return 1;
}


void CTeachParamDlg::SetParamData(BOOL bAllTab)
{
	if (bAllTab)
	{
		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			UpdateUsedAlgorithmList(i);
			AlgorithmParamUpDate(FALSE, FALSE, i);
			AISetUpdate(i);
		}
	}

	UpdateUsedAlgorithmList(m_iCurrentAlgorithmTabIdx);
	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateInspectCheckStatus();
	m_TeachParamGridCtrl.RedrawAll();
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedButtonInspectAreaUpdate()
{
	UpdateData(TRUE);

	THEAPP.m_pTabControlDlg->GetROIUpdate(m_iCurrentAlgorithmTabIdx, m_bCheckUseInspectArea1, m_iEditArea1Margin1, m_iEditArea1Margin2, m_bCheckUseInspectArea2, m_iEditArea2Margin1, m_iEditArea2Margin2, m_bCheckUseInspectArea3, m_iEditArea3Margin1, m_iEditArea3Margin2);
}


void CTeachParamDlg::OnBnClickedButtonSetAlgorithmParm()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	AlgorithmParamUpDate(TRUE, FALSE, m_iCurrentAlgorithmTabIdx);		// 화면 파라미터 저장
	AISetUpdate(m_iCurrentAlgorithmTabIdx);

	THEAPP.m_pInspectAdminViewToolbarDlg->SetToolBarStateDraw();

	THEAPP.m_pTabControlDlg->ShowAlgorithmWnd(THEAPP.m_pTabControlDlg->m_iCurrentTab - 1, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion, m_iCurrentAlgorithmTabIdx);
}


void CTeachParamDlg::OnBnClickedButtonAlgorithmTestTab()
{
	double tStartTime = GetTickCount();

	THEAPP.bImageCompareTeachingDetect = TRUE;
	THEAPP.m_pTabControlDlg->TeachAlgorithmTest_Tab(THEAPP.m_pTabControlDlg->m_iCurrentTab - 1, m_iCurrentAlgorithmTabIdx);
	THEAPP.bImageCompareTeachingDetect = FALSE;

	double tEndTime = GetTickCount();
	double tElapsedTime = tEndTime - tStartTime;

	CString sProcTime;
	sProcTime.Format("%.2lf(ms)", tElapsedTime);
	SetDlgItemText(IDC_EDIT_PROC_TIME, sProcTime);
}


void CTeachParamDlg::OnBnClickedButtonAlgorithmTestRoi()
{
	double tStartTime = GetTickCount();

	THEAPP.bImageCompareTeachingDetect = TRUE;
	THEAPP.m_pTabControlDlg->TeachAlgorithmTest_ROI(THEAPP.m_pTabControlDlg->m_iCurrentTab - 1);
	THEAPP.bImageCompareTeachingDetect = FALSE;

	double tEndTime = GetTickCount();
	double tElapsedTime = tEndTime - tStartTime;

	CString sProcTime;
	sProcTime.Format("%.2lf(ms)", tElapsedTime);
	SetDlgItemText(IDC_EDIT_PROC_TIME, sProcTime);
}


void CTeachParamDlg::OnBnClickedButtonAlgorithmTestImage()
{
	double tStartTime = GetTickCount();

	THEAPP.bImageCompareTeachingDetect = TRUE;
	THEAPP.m_pTabControlDlg->TeachAlgorithmTest_Image(THEAPP.m_pTabControlDlg->m_iCurrentTab - 1);
	THEAPP.bImageCompareTeachingDetect = FALSE;

	double tEndTime = GetTickCount();
	double tElapsedTime = tEndTime - tStartTime;

	CString sProcTime;
	sProcTime.Format("%.2lf(ms)", tElapsedTime);
	SetDlgItemText(IDC_EDIT_PROC_TIME, sProcTime);
}


void CTeachParamDlg::OnBnClickedButtonImageCompareTrainTab()
{
	THEAPP.bImageCompareAutoTrain = TRUE;
	THEAPP.m_pTabControlDlg->TeachAlgorithmTest_Tab(THEAPP.m_pTabControlDlg->m_iCurrentTab - 1, m_iCurrentAlgorithmTabIdx);
	THEAPP.bImageCompareAutoTrain = FALSE;
}


void CTeachParamDlg::OnBnClickedButtonImageCompareTrainRoi()
{
	THEAPP.bImageCompareAutoTrain = TRUE;
	THEAPP.m_pTabControlDlg->TeachAlgorithmTest_ROI(THEAPP.m_pTabControlDlg->m_iCurrentTab - 1);
	THEAPP.bImageCompareAutoTrain = FALSE;
}


void CTeachParamDlg::OnBnClickedButtonImageCompareTrainImage()
{
	THEAPP.bImageCompareAutoTrain = TRUE;
	THEAPP.m_pTabControlDlg->TeachAlgorithmTest_Image(THEAPP.m_pTabControlDlg->m_iCurrentTab - 1);
	THEAPP.bImageCompareAutoTrain = FALSE;
}


void CTeachParamDlg::OnBnClickedCheckUseInspect()
{
	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	UpdateData();

	if (m_bCheckUseInspect)
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bInspect = TRUE;
	else
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bInspect = FALSE;

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateInspectCheckStatus();
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedCheckUseFilter1()
{
	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	UpdateData();

	if (m_bCheckUseFilter1)
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bUseFilter1 = TRUE;
	else
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bUseFilter1 = FALSE;

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedCheckUseFilter2()
{
	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	UpdateData();

	if (m_bCheckUseFilter2)
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bUseFilter2 = TRUE;
	else
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bUseFilter2 = FALSE;

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedCheckUseFilter3()
{
	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	UpdateData();

	if (m_bCheckUseFilter3)
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bUseFilter3 = TRUE;
	else
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bUseFilter3 = FALSE;

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateData(FALSE);
}


void CTeachParamDlg::OnBnClickedCheckUseFilter4()
{
	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	UpdateData();

	if (m_bCheckUseFilter4)
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bUseFilter4 = TRUE;
	else
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bUseFilter4 = FALSE;

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateData(FALSE);
}

void CTeachParamDlg::OnBnClickedCheckAIUseConsiderImageNo()
{
	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	UpdateData();

	if (m_bCheckAIUseConsiderImageNo) {
		bool a = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderImageNo;
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderImageNo = TRUE;
		bool b = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderImageNo;

		int n = 1;
	}
	else {
		bool a = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderImageNo;
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderImageNo = FALSE;
		bool b = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderImageNo;

		int n = 1;
	}

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateData(FALSE);
}

void CTeachParamDlg::OnBnClickedCheckAIUseConsiderDefectCode()
{
	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	UpdateData();

	if (m_bCheckAIUseConsiderDefectCode) {
		bool a = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderDefectCode;
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderDefectCode = TRUE;
		bool b = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderDefectCode;
		
		int n = 1;

	}
	else {
		bool a = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderDefectCode;
		THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderDefectCode = FALSE;
		bool b = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[m_iCurrentAlgorithmTabIdx].m_bCheckAIUseConsiderDefectCode;

		int n = 1;
	}

	AlgorithmParamUpDate(FALSE, FALSE, m_iCurrentAlgorithmTabIdx);
	AISetUpdate(m_iCurrentAlgorithmTabIdx);
	UpdateData(FALSE);
}

void CTeachParamDlg::AISetUpdate(int iTabIdx)
{
	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL) {
		m_ButtonSetAISet1.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
		m_ButtonSetAISet2.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
		m_ButtonSetAISet3.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
		m_bCheckUseFilter1 = FALSE;
		m_bCheckUseFilter2 = FALSE;
		m_bCheckUseFilter3 = FALSE;
		return;
	}

	//if ((THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType - 1 >= INSPECTION_TYPE_FIRST &&
	//	 THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType - 1 <= INSPECTION_TYPE_LAST) == FALSE) {
	//	m_ButtonSetAISet1.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
	//	m_ButtonSetAISet2.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
	//	m_ButtonSetAISet3.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
	//	m_bCheckUseFilter1 = FALSE;
	//	m_bCheckUseFilter2 = FALSE;
	//	m_bCheckUseFilter3 = FALSE;
	//	return;
	//}

#ifdef SYAI
	int defect_name_index = m_ComboDefectType.GetCurSel();

	// --- AI 모델 Key 생성
	syai_service::NameBuildOptions build_options;
	build_options.consider_vision_pos = false;
	build_options.consider_inspection_type = true;
	build_options.consider_image_index = m_bCheckAIUseConsiderImageNo ? true : false;
	build_options.consider_defect_type = m_bCheckAIUseConsiderDefectCode ? true : false;
	build_options.ignore_case = false;
	build_options.allow_input_duplicates = false;
	build_options.distinct = true;

	std::string camera_type = (LPCSTR)THEAPP.m_ModelDefineInfo.m_strVisionName_Short[THEAPP.m_iCurVisionCamType];

	std::string inspection_type = std::string(g_strInspectionTypeName_Short[THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType - 1]);
	std::string image_index_str = std::to_string(THEAPP.m_pTabControlDlg->m_iCurrentTab);
	std::string defect_name = std::string(CT2A(THEAPP.m_strDefectName[defect_name_index]));
	std::string defect_type = std::string(CT2A(THEAPP.m_strDefectCode[defect_name_index]));
	std::string model_key = syai_service::ServiceDirector::build_name(camera_type, "", inspection_type, image_index_str, defect_type, build_options);

	std::map<int, int> ai_model_init_status = THEAPP.m_syai_service->get_manager_initialization_status(model_key.c_str());

	if (ai_model_init_status.size() == 0) {
		m_ButtonSetAISet1.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
		m_ButtonSetAISet2.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
		m_ButtonSetAISet3.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
		return;
	}

	// Manager 0 확인
	if (ai_model_init_status.find(0) != ai_model_init_status.end()) {
		m_ButtonSetAISet1.SetFaceColor(BUTTON_YELLOW_GREEN_COLOR);
		THEAPP.m_syai_logger->debug("AI Manager 0 - Priority: " + std::to_string(ai_model_init_status[0]));
	}
	else {
		m_ButtonSetAISet1.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
		THEAPP.m_syai_logger->debug("AI Manager 0 - Not Initialized");
	}

	// Manager 1 확인
	if (ai_model_init_status.find(1) != ai_model_init_status.end()) {
		m_ButtonSetAISet2.SetFaceColor(BUTTON_YELLOW_GREEN_COLOR);
		THEAPP.m_syai_logger->debug("AI Manager 1 - Priority: " + std::to_string(ai_model_init_status[1]));
	}
	else {
		m_ButtonSetAISet2.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
		THEAPP.m_syai_logger->debug("AI Manager 1 - Not Initialized");
	}

	// Manager 2 확인
	if (ai_model_init_status.find(2) != ai_model_init_status.end()) {
		m_ButtonSetAISet3.SetFaceColor(BUTTON_YELLOW_GREEN_COLOR);
		THEAPP.m_syai_logger->debug("AI Manager 2 - Priority: " + std::to_string(ai_model_init_status[2]));
	}
	else {
		m_ButtonSetAISet3.SetFaceColor(BUTTON_LIGHT_RED_COLOR);
		THEAPP.m_syai_logger->debug("AI Manager 2 - Not Initialized");
	}
#endif // SYAI
}
