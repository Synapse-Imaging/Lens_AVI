// GlobalMatchingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "GlobalMatchingDlg.h"
#include "IniFileCS.h"
#include "afxdialogex.h"


// CGlobalMatchingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGlobalMatchingDlg, CDialog)

CGlobalMatchingDlg::CGlobalMatchingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGlobalMatchingDlg::IDD, pParent)
	, m_iLocalIndexNumber(0)
	, m_bCheckUseMatchingXYComp(FALSE)
	, m_iRadioMatchingXYRefLine(0)
{
	m_bCheckUseMatchingShape = TRUE;
	m_bCheckUseMatchingNCC = FALSE;
	m_bCheckUseMatchingPerspective = FALSE;

	m_iEditMatchingPyramidLevel = 0;
	m_iEditMatchingAngleRange = 5.0;
	m_dEditMatchingScaleMin = 0.98;
	m_dEditMatchingScaleMax = 1.02;
	m_iEditMatchingTeachingContrast = 40;
	m_iEditMatchingTeachingContrastLow = 40;
	m_iEditMatchingTeachingContrastMinSize = 1;
	m_dEditMatchingScore = 0.3;
	m_iEditMatchingInspectionMinContrast = 15;
	m_iEditMatchingSearchMarginX = 300;
	m_iEditMatchingSearchMarginY = 300;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		m_bCheckAlignImage[i] = FALSE;

	m_iCurrentShapeModel = ALIGN_MATCHING_SHAPE;
	m_iRadioAlignProcessChType = CHANNEL_TYPE_I;

	m_bCheckUseMatchingFilter = FALSE;
	m_dEditMatchingFilterTypeXSize = 0;
	m_dEditMatchingFilterTypeYSize = 0;
	m_dEditDeltaX = 0.0;
	m_dEditDeltaY = 0.0;
	m_dEditRotationAngle = 0;

	m_bCheckUseMatchingAngleComp = FALSE;
	m_iRadioMatchingAngleRefLine = MATCHING_ANGLE_REF_LINE_X;

	m_bCheckUseMatchingAffineConstant = FALSE;
}

CGlobalMatchingDlg::~CGlobalMatchingDlg()
{
}

void CGlobalMatchingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MATCHING_ANGLE_RANGE, m_iEditMatchingAngleRange);
	DDX_Text(pDX, IDC_EDIT_MATCHING_P_LEVEL, m_iEditMatchingPyramidLevel);
	DDX_Text(pDX, IDC_EDIT_MATCHING_TEACHING_CONTRAST, m_iEditMatchingTeachingContrast);
	DDX_Text(pDX, IDC_EDIT_MATCHING_TEACHING_CONTRAST_LOW, m_iEditMatchingTeachingContrastLow);
	DDX_Text(pDX, IDC_EDIT_MATCHING_TEACHING_CONTRAST_MIN_SIZE, m_iEditMatchingTeachingContrastMinSize);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SCORE, m_dEditMatchingScore);
	DDX_Text(pDX, IDC_EDIT_MATCHING_INSPECTION_MIN_CONTRAST, m_iEditMatchingInspectionMinContrast);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SEARCH_MARGIN_X, m_iEditMatchingSearchMarginX);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SEARCH_MARGIN_Y, m_iEditMatchingSearchMarginY);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SCALE_MIN, m_dEditMatchingScaleMin);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SCALE_MAX, m_dEditMatchingScaleMax);

	DDX_Control(pDX, IDC_BUTTON_SHOW_CONTRAST, m_bnShowContrast);
	DDX_Control(pDX, IDC_BUTTON_SHOW_POW_IMAGE, m_bnShowPowImage);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_SCORE, m_bnChangeParam);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_ANGLE_COMP, m_bnChangeAngleComp);
	DDX_Control(pDX, IDC_BUTTON_CREATE_GLOBAL_MATCHING_MODEL, m_bnCreateGlobalMatchingModel);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_ROTATION, m_bnImageRotation);
	DDX_Control(pDX, IDC_BUTTON_TEST_GLOBAL_MATCHING_MODEL, m_bnTestGlobalMatchingModel);
	DDX_Control(pDX, IDC_BUTTON_AFFINE_IMAGE, m_bnAffineImage);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_XY_COMP, m_bnChangeXYComp);

	DDX_Check(pDX, IDC_CHECK_IMAGE_1, m_bCheckAlignImage[0]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_2, m_bCheckAlignImage[1]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_3, m_bCheckAlignImage[2]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_4, m_bCheckAlignImage[3]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_5, m_bCheckAlignImage[4]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_6, m_bCheckAlignImage[5]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_7, m_bCheckAlignImage[6]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_8, m_bCheckAlignImage[7]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_9, m_bCheckAlignImage[8]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_10, m_bCheckAlignImage[9]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_11, m_bCheckAlignImage[10]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_12, m_bCheckAlignImage[11]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_13, m_bCheckAlignImage[12]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_14, m_bCheckAlignImage[13]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_15, m_bCheckAlignImage[14]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_16, m_bCheckAlignImage[15]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_17, m_bCheckAlignImage[16]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_18, m_bCheckAlignImage[17]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_19, m_bCheckAlignImage[18]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_20, m_bCheckAlignImage[19]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_21, m_bCheckAlignImage[20]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_22, m_bCheckAlignImage[21]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_23, m_bCheckAlignImage[22]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_24, m_bCheckAlignImage[23]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_25, m_bCheckAlignImage[24]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_26, m_bCheckAlignImage[25]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_27, m_bCheckAlignImage[26]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_28, m_bCheckAlignImage[27]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_29, m_bCheckAlignImage[28]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_30, m_bCheckAlignImage[29]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_31, m_bCheckAlignImage[30]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_32, m_bCheckAlignImage[31]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_33, m_bCheckAlignImage[32]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_34, m_bCheckAlignImage[33]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_35, m_bCheckAlignImage[34]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_36, m_bCheckAlignImage[35]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_37, m_bCheckAlignImage[36]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_38, m_bCheckAlignImage[37]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_39, m_bCheckAlignImage[38]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_40, m_bCheckAlignImage[39]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_41, m_bCheckAlignImage[40]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_42, m_bCheckAlignImage[41]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_43, m_bCheckAlignImage[42]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_44, m_bCheckAlignImage[43]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_45, m_bCheckAlignImage[44]);

	DDX_Check(pDX, IDC_CHECK_USE_SHAPE_MATCHING, m_bCheckUseMatchingShape);
	DDX_Check(pDX, IDC_CHECK_USE_TEMPLATE_MATCHING, m_bCheckUseMatchingNCC);
	DDX_Check(pDX, IDC_CHECK_USE_PERSPECTIVE_MATCHING, m_bCheckUseMatchingPerspective);
	DDX_Radio(pDX, IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_COLOR, m_iRadioAlignProcessChType);
	DDX_Check(pDX, IDC_CHECK_USE_MATCHING_FILTER, m_bCheckUseMatchingFilter);
	DDX_Control(pDX, IDC_COMBO_MATCHING_FILTER_TYPE, m_cbMatchingFilterType);
	DDX_Text(pDX, IDC_EDIT_MATCHING_FILTER_X_SIZE, m_dEditMatchingFilterTypeXSize);
	DDX_Text(pDX, IDC_EDIT_MATCHING_FILTER_Y_SIZE, m_dEditMatchingFilterTypeYSize);
	DDX_Text(pDX, IDC_EDIT_DELTA_X, m_dEditDeltaX);
	DDX_Text(pDX, IDC_EDIT_DELTA_Y, m_dEditDeltaY);
	DDX_Text(pDX, IDC_EDIT_ROTATION_ANGLE, m_dEditRotationAngle);

	DDX_Check(pDX, IDC_CHECK_USE_MATCHING_ANGLE_COMP, m_bCheckUseMatchingAngleComp);
	DDX_Radio(pDX, IDC_RADIO_MATCHING_ANGLE_REF_X, m_iRadioMatchingAngleRefLine);

	DDX_Check(pDX, IDC_CHECK_USE_MATCHING_AFFINE_CONSTANT, m_bCheckUseMatchingAffineConstant);
	DDX_Text(pDX, IDC_EDIT_MATCHING_INDEX_IMAGE, m_iLocalIndexNumber);
	DDX_Check(pDX, IDC_CHECK_USE_MATCHING_XY_COMP, m_bCheckUseMatchingXYComp);
	DDX_Radio(pDX, IDC_RADIO_MATCHING_REF_X, m_iRadioMatchingXYRefLine);
}


BEGIN_MESSAGE_MAP(CGlobalMatchingDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_CONTRAST, &CGlobalMatchingDlg::OnBnClickedButtonShowContrast)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_SCORE, &CGlobalMatchingDlg::OnBnClickedButtonChangeScore)
	ON_BN_CLICKED(IDC_CHECK_USE_SHAPE_MATCHING, &CGlobalMatchingDlg::OnBnClickedCheckUseShapeMatching)
	ON_BN_CLICKED(IDC_CHECK_USE_TEMPLATE_MATCHING, &CGlobalMatchingDlg::OnClickedCheckUseTemplateMatching)
	ON_BN_CLICKED(IDC_CHECK_USE_PERSPECTIVE_MATCHING, &CGlobalMatchingDlg::OnBnClickedCheckUsePerspectiveMatching)
	ON_BN_CLICKED(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_COLOR, &CGlobalMatchingDlg::OnBnClickedRadioProcessImageColor)
	ON_BN_CLICKED(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_R, &CGlobalMatchingDlg::OnBnClickedRadioProcessImageR)
	ON_BN_CLICKED(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_G, &CGlobalMatchingDlg::OnBnClickedRadioProcessImageG)
	ON_BN_CLICKED(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_B, &CGlobalMatchingDlg::OnBnClickedRadioProcessImageB)
	ON_BN_CLICKED(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_H, &CGlobalMatchingDlg::OnBnClickedRadioProcessImageH)
	ON_BN_CLICKED(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_S, &CGlobalMatchingDlg::OnBnClickedRadioProcessImageS)
	ON_BN_CLICKED(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_I, &CGlobalMatchingDlg::OnBnClickedRadioProcessImageI)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_POW_IMAGE, &CGlobalMatchingDlg::OnBnClickedButtonShowPowImage)
	ON_BN_CLICKED(IDC_BUTTON_AFFINE_IMAGE, &CGlobalMatchingDlg::OnBnClickedButtonAffineImage)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_ANGLE_COMP, &CGlobalMatchingDlg::OnBnClickedButtonChangeAngleComp)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_XY_COMP, &CGlobalMatchingDlg::OnBnClickedButtonChangeXyComp)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_GLOBAL_MATCHING_MODEL, &CGlobalMatchingDlg::OnBnClickedButtonCreateGlobalMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_GLOBAL_MATCHING_MODEL, &CGlobalMatchingDlg::OnBnClickedButtonDeleteGlobalMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_TEST_GLOBAL_MATCHING_MODEL, &CGlobalMatchingDlg::OnBnClickedButtonTestGlobalMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_ROTATION, &CGlobalMatchingDlg::OnBnClickedButtonImageRotation)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CGlobalMatchingDlg 메시지 처리기입니다.

BOOL CGlobalMatchingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CGlobalMatchingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_bnShowContrast.Set_Text(_T("매칭 모델 Shape Edge 보기"));
	m_bnShowContrast.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	//m_bnShowPowImage.Set_Text(_T("지수 영상 보기"));
	m_bnShowPowImage.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	//m_bnChangeParam.Set_Text(_T("검사 파라미터 변경"));
	m_bnChangeParam.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	//m_bnChangeAngleComp.Set_Text(_T("회전 재보정 옵션 변경"));
	m_bnChangeAngleComp.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	//m_bnCreateMatchingModel.Set_Text(_T("매칭 모델 생성"));
	m_bnCreateGlobalMatchingModel.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	//m_bnTestMatchingModel.Set_Text(_T("매칭 모델 테스트"));
	m_bnTestGlobalMatchingModel.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	//m_bnImageRotation.Set_Text(_T("오브젝트 수평되도록 영상 회전"));
	m_bnImageRotation.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	//m_bnAffineImage.Set_Text(_T("영상 이동"));
	m_bnAffineImage.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_bnChangeXYComp.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_iCurImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;

	m_bCheckAlignImage[m_iCurImageIndex] = TRUE;

	LoadViewParam();

	// Update View Image Channel
	if (THEAPP.m_pModelDataManager->m_bUseMatchingFilter[m_iCurImageIndex])
	{
		THEAPP.m_pAlgorithm->GetMatchingPreprocessImage(m_HOrgTeachingImage, THEAPP.m_pModelDataManager->m_iMatchingFilterType[m_iCurImageIndex], THEAPP.m_pModelDataManager->m_dMatchingFilterTypeXSize[m_iCurImageIndex], THEAPP.m_pModelDataManager->m_dMatchingFilterTypeYSize[m_iCurImageIndex], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][m_iCurImageIndex]));

		Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][m_iCurImageIndex], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][m_iCurImageIndex]));
		TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][m_iCurImageIndex], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][m_iCurImageIndex], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][m_iCurImageIndex], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][m_iCurImageIndex]), "hsi");
	}

	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]);

	if (m_bCheckUseMatchingShape)
		GetDlgItem(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_COLOR)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_COLOR)->EnableWindow(FALSE);

	ChangeLanguage();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CGlobalMatchingDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("매칭 파라미터 설정"));
		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG1, _T("매칭 알고리즘:"));
		SetDlgItemText(IDC_CHECK_USE_SHAPE_MATCHING, _T("Shape"));
		SetDlgItemText(IDC_CHECK_USE_TEMPLATE_MATCHING, _T("Templete"));
		SetDlgItemText(IDC_CHECK_USE_PERSPECTIVE_MATCHING, _T("Perspective"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG2, _T("영상 전처리"));
		SetDlgItemText(IDC_CHECK_USE_MATCHING_FILTER, _T("전처리 사용"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG1, _T("폭"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG2, _T("높이"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG3, _T("타입"));
		m_cbMatchingFilterType.ResetContent();
		m_cbMatchingFilterType.AddString("미사용");
		m_cbMatchingFilterType.AddString("평균");
		m_cbMatchingFilterType.AddString("미디언");
		m_cbMatchingFilterType.AddString("가우시안 스무딩");
		m_cbMatchingFilterType.AddString("GV 범위를 0~255로 스케일링");
		m_cbMatchingFilterType.AddString("에지 강조");
		m_cbMatchingFilterType.AddString("히스토그램 평활화");
		m_cbMatchingFilterType.AddString("Contrast 향상");
		m_cbMatchingFilterType.AddString("지수영상 (파라미터=지수)");
		m_cbMatchingFilterType.AddString("로그영상");
		m_cbMatchingFilterType.SetCurSel(THEAPP.m_pModelDataManager->m_iMatchingFilterType[m_iCurImageIndex]);
		SetDlgItemText(IDC_BUTTON_SHOW_POW_IMAGE, _T("전처리 확인"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG3, _T("티칭 파라미터"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG4, _T("피라미드 단계(자동=0) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG5, _T("각도 범위(+/-) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG6, _T("도"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG7, _T("스케일 변화:"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG8, _T("~"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG9, _T("최대 콘트라스트(밝기값) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG10, _T("최소 콘트라스트(밝기값) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG11, _T("특징점 최소 픽셀 개수:"));
		SetDlgItemText(IDC_BUTTON_SHOW_CONTRAST, _T("특징점 확인"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG4, _T("검사 파라미터"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG12, _T("매칭 점수(0~1) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG13, _T("최소 콘트라스트(밝기값) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG14, _T("x 매칭 탐색 범위(픽셀) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG15, _T("y 매칭 탐색 범위(픽셀) :"));
		SetDlgItemText(IDC_BUTTON_CHANGE_SCORE, _T("\n검사 파라미터\n변경"));
		SetDlgItemText(IDC_CHECK_USE_MATCHING_AFFINE_CONSTANT, _T("얼라인 적용 영상에 선형보간법 사용"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG5, _T("얼라인 적용 영상"));
		SetDlgItemText(IDC_CHECK_IMAGE_1, _T("1"));
		SetDlgItemText(IDC_CHECK_IMAGE_2, _T("2"));
		SetDlgItemText(IDC_CHECK_IMAGE_3, _T("3"));
		SetDlgItemText(IDC_CHECK_IMAGE_4, _T("4"));
		SetDlgItemText(IDC_CHECK_IMAGE_5, _T("5"));
		SetDlgItemText(IDC_CHECK_IMAGE_6, _T("6"));
		SetDlgItemText(IDC_CHECK_IMAGE_7, _T("7"));
		SetDlgItemText(IDC_CHECK_IMAGE_8, _T("8"));
		SetDlgItemText(IDC_CHECK_IMAGE_9, _T("9"));
		SetDlgItemText(IDC_CHECK_IMAGE_10, _T("10"));
		SetDlgItemText(IDC_CHECK_IMAGE_11, _T("11"));
		SetDlgItemText(IDC_CHECK_IMAGE_12, _T("12"));
		SetDlgItemText(IDC_CHECK_IMAGE_13, _T("13"));
		SetDlgItemText(IDC_CHECK_IMAGE_14, _T("14"));
		SetDlgItemText(IDC_CHECK_IMAGE_15, _T("15"));
		SetDlgItemText(IDC_CHECK_IMAGE_16, _T("16"));
		SetDlgItemText(IDC_CHECK_IMAGE_17, _T("17"));
		SetDlgItemText(IDC_CHECK_IMAGE_18, _T("18"));
		SetDlgItemText(IDC_CHECK_IMAGE_19, _T("19"));
		SetDlgItemText(IDC_CHECK_IMAGE_20, _T("20"));
		SetDlgItemText(IDC_CHECK_IMAGE_21, _T("21"));
		SetDlgItemText(IDC_CHECK_IMAGE_22, _T("22"));
		SetDlgItemText(IDC_CHECK_IMAGE_23, _T("23"));
		SetDlgItemText(IDC_CHECK_IMAGE_24, _T("24"));
		SetDlgItemText(IDC_CHECK_IMAGE_25, _T("25"));
		SetDlgItemText(IDC_CHECK_IMAGE_26, _T("26"));
		SetDlgItemText(IDC_CHECK_IMAGE_27, _T("27"));
		SetDlgItemText(IDC_CHECK_IMAGE_28, _T("28"));
		SetDlgItemText(IDC_CHECK_IMAGE_29, _T("29"));
		SetDlgItemText(IDC_CHECK_IMAGE_30, _T("30"));
		SetDlgItemText(IDC_CHECK_IMAGE_31, _T("31"));
		SetDlgItemText(IDC_CHECK_IMAGE_32, _T("32"));
		SetDlgItemText(IDC_CHECK_IMAGE_33, _T("33"));
		SetDlgItemText(IDC_CHECK_IMAGE_34, _T("34"));
		SetDlgItemText(IDC_CHECK_IMAGE_35, _T("35"));
		SetDlgItemText(IDC_CHECK_IMAGE_36, _T("36"));
		SetDlgItemText(IDC_CHECK_IMAGE_37, _T("37"));
		SetDlgItemText(IDC_CHECK_IMAGE_38, _T("38"));
		SetDlgItemText(IDC_CHECK_IMAGE_39, _T("39"));
		SetDlgItemText(IDC_CHECK_IMAGE_40, _T("40"));
		SetDlgItemText(IDC_CHECK_IMAGE_41, _T("41"));
		SetDlgItemText(IDC_CHECK_IMAGE_42, _T("42"));
		SetDlgItemText(IDC_CHECK_IMAGE_43, _T("43"));
		SetDlgItemText(IDC_CHECK_IMAGE_44, _T("44"));
		SetDlgItemText(IDC_CHECK_IMAGE_45, _T("45"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG6, _T("알고리즘 처리 영상"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_COLOR, _T("컬러"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_R, _T("R"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_G, _T("G"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_B, _T("B"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_H, _T("H"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_S, _T("S"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_I, _T("I"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG7, _T("x/y 개별 보정"));
		SetDlgItemText(IDC_CHECK_USE_MATCHING_XY_COMP, _T("로컬 얼라인 ROI로부터 x/y축 재보정 사용"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG16, _T("기준 라인:"));
		SetDlgItemText(IDC_RADIO_MATCHING_REF_X, _T("수평선"));
		SetDlgItemText(IDC_RADIO_MATCHING_REF_Y, _T("수직선"));
		SetDlgItemText(IDC_BUTTON_CHANGE_XY_COMP, _T("수정"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG8, _T("회전 각도 보정"));
		SetDlgItemText(IDC_CHECK_USE_MATCHING_ANGLE_COMP, _T("얼라인 각도 ROI로부터 회전 각도 재보정 사용"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG17, _T("기준 라인:"));
		SetDlgItemText(IDC_RADIO_MATCHING_ANGLE_REF_X, _T("수평선"));
		SetDlgItemText(IDC_RADIO_MATCHING_ANGLE_REF_Y, _T("수직선"));
		SetDlgItemText(IDC_BUTTON_CHANGE_ANGLE_COMP, _T("수정"));

		SetDlgItemText(IDC_BUTTON_CREATE_GLOBAL_MATCHING_MODEL, _T("매칭 모델 생성"));
		SetDlgItemText(IDC_BUTTON_DELETE_GLOBAL_MATCHING_MODEL, _T("매칭 모델 삭제"));
		SetDlgItemText(IDC_BUTTON_IMAGE_ROTATION, _T("영상 수평 회전(티칭)"));
		SetDlgItemText(IDC_BUTTON_TEST_GLOBAL_MATCHING_MODEL, _T("매칭 테스트"));
		SetDlgItemText(IDC_BUTTON_AFFINE_IMAGE, _T("영상 변환(이동 및 회전)"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG18, _T("x(픽셀)"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG19, _T("y(픽셀)"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG20, _T("회전(도)"));
	}
	else
	{
		this->SetWindowText(_T("Set matching paarameter"));
		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG1, _T("Matching algorithm"));
		SetDlgItemText(IDC_CHECK_USE_SHAPE_MATCHING, _T("Shape"));
		SetDlgItemText(IDC_CHECK_USE_TEMPLATE_MATCHING, _T("Templete"));
		SetDlgItemText(IDC_CHECK_USE_PERSPECTIVE_MATCHING, _T("Perspective"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG2, _T("Image preprocessing"));
		SetDlgItemText(IDC_CHECK_USE_MATCHING_FILTER, _T("Use preprocess"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG1, _T("Width"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG2, _T("Height"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG3, _T("Type"));
		m_cbMatchingFilterType.ResetContent();
		m_cbMatchingFilterType.AddString("Unuse");
		m_cbMatchingFilterType.AddString("Average");
		m_cbMatchingFilterType.AddString("Median");
		m_cbMatchingFilterType.AddString("Gaussian smoothing");
		m_cbMatchingFilterType.AddString("Scale the GV range to 0~255");
		m_cbMatchingFilterType.AddString("Emphasize edges");
		m_cbMatchingFilterType.AddString("Histogram equalization");
		m_cbMatchingFilterType.AddString("Contrast enhancement");
		m_cbMatchingFilterType.AddString("Exponential image(parameter=exponent)");
		m_cbMatchingFilterType.AddString("Log image");
		m_cbMatchingFilterType.SetCurSel(THEAPP.m_pModelDataManager->m_iMatchingFilterType[m_iCurImageIndex]);
		SetDlgItemText(IDC_BUTTON_SHOW_POW_IMAGE, _T("Show preprocess"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG3, _T("Teaching parameter"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG4, _T("Pyramid level(auto=0) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG5, _T("Rotation range(+/-) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG6, _T("deg"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG7, _T("Scale range:"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG8, _T("~"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG9, _T("Max contrast(GV) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG10, _T("Min contrast(GV) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG11, _T("Min Qty of pixels for feature:"));
		SetDlgItemText(IDC_BUTTON_SHOW_CONTRAST, _T("Show\nfeature\npoints"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG4, _T("Inspection parameter"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG12, _T("Matching score(0 ~ 1) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG13, _T("Min contrast(GV) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG14, _T("x search range(pixel) :"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG15, _T("y search range(pixel) :"));
		SetDlgItemText(IDC_BUTTON_CHANGE_SCORE, _T("Inspect\nparameter\nchange"));
		SetDlgItemText(IDC_CHECK_USE_MATCHING_AFFINE_CONSTANT, _T("Use interpolation for align image"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG5, _T("Align image"));
		SetDlgItemText(IDC_CHECK_IMAGE_1, _T("1"));
		SetDlgItemText(IDC_CHECK_IMAGE_2, _T("2"));
		SetDlgItemText(IDC_CHECK_IMAGE_3, _T("3"));
		SetDlgItemText(IDC_CHECK_IMAGE_4, _T("4"));
		SetDlgItemText(IDC_CHECK_IMAGE_5, _T("5"));
		SetDlgItemText(IDC_CHECK_IMAGE_6, _T("6"));
		SetDlgItemText(IDC_CHECK_IMAGE_7, _T("7"));
		SetDlgItemText(IDC_CHECK_IMAGE_8, _T("8"));
		SetDlgItemText(IDC_CHECK_IMAGE_9, _T("9"));
		SetDlgItemText(IDC_CHECK_IMAGE_10, _T("10"));
		SetDlgItemText(IDC_CHECK_IMAGE_11, _T("11"));
		SetDlgItemText(IDC_CHECK_IMAGE_12, _T("12"));
		SetDlgItemText(IDC_CHECK_IMAGE_13, _T("13"));
		SetDlgItemText(IDC_CHECK_IMAGE_14, _T("14"));
		SetDlgItemText(IDC_CHECK_IMAGE_15, _T("15"));
		SetDlgItemText(IDC_CHECK_IMAGE_16, _T("16"));
		SetDlgItemText(IDC_CHECK_IMAGE_17, _T("17"));
		SetDlgItemText(IDC_CHECK_IMAGE_18, _T("18"));
		SetDlgItemText(IDC_CHECK_IMAGE_19, _T("19"));
		SetDlgItemText(IDC_CHECK_IMAGE_20, _T("20"));
		SetDlgItemText(IDC_CHECK_IMAGE_21, _T("21"));
		SetDlgItemText(IDC_CHECK_IMAGE_22, _T("22"));
		SetDlgItemText(IDC_CHECK_IMAGE_23, _T("23"));
		SetDlgItemText(IDC_CHECK_IMAGE_24, _T("24"));
		SetDlgItemText(IDC_CHECK_IMAGE_25, _T("25"));
		SetDlgItemText(IDC_CHECK_IMAGE_26, _T("26"));
		SetDlgItemText(IDC_CHECK_IMAGE_27, _T("27"));
		SetDlgItemText(IDC_CHECK_IMAGE_28, _T("28"));
		SetDlgItemText(IDC_CHECK_IMAGE_29, _T("29"));
		SetDlgItemText(IDC_CHECK_IMAGE_30, _T("30"));
		SetDlgItemText(IDC_CHECK_IMAGE_31, _T("31"));
		SetDlgItemText(IDC_CHECK_IMAGE_32, _T("32"));
		SetDlgItemText(IDC_CHECK_IMAGE_33, _T("33"));
		SetDlgItemText(IDC_CHECK_IMAGE_34, _T("34"));
		SetDlgItemText(IDC_CHECK_IMAGE_35, _T("35"));
		SetDlgItemText(IDC_CHECK_IMAGE_36, _T("36"));
		SetDlgItemText(IDC_CHECK_IMAGE_37, _T("37"));
		SetDlgItemText(IDC_CHECK_IMAGE_38, _T("38"));
		SetDlgItemText(IDC_CHECK_IMAGE_39, _T("39"));
		SetDlgItemText(IDC_CHECK_IMAGE_40, _T("40"));
		SetDlgItemText(IDC_CHECK_IMAGE_41, _T("41"));
		SetDlgItemText(IDC_CHECK_IMAGE_42, _T("42"));
		SetDlgItemText(IDC_CHECK_IMAGE_43, _T("43"));
		SetDlgItemText(IDC_CHECK_IMAGE_44, _T("44"));
		SetDlgItemText(IDC_CHECK_IMAGE_45, _T("45"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG6, _T("Algorithm process image"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_COLOR, _T("Color"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_R, _T("R"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_G, _T("G"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_B, _T("B"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_H, _T("H"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_S, _T("S"));
		SetDlgItemText(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_I, _T("I"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG7, _T("x,y Adjustment"));
		SetDlgItemText(IDC_CHECK_USE_MATCHING_XY_COMP, _T("Use x,y re-adjustment from local align"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG16, _T("Reference line:"));
		SetDlgItemText(IDC_RADIO_MATCHING_REF_X, _T("Horizontal"));
		SetDlgItemText(IDC_RADIO_MATCHING_REF_Y, _T("Vertical"));
		SetDlgItemText(IDC_BUTTON_CHANGE_XY_COMP, _T("Apply"));

		SetDlgItemText(IDC_GROUPBOX_GLOBALMATCHINGDLG8, _T("Rotation angle adjustment"));
		SetDlgItemText(IDC_CHECK_USE_MATCHING_ANGLE_COMP, _T("Use rotation angle re-adjustment from local align"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG17, _T("Reference line:"));
		SetDlgItemText(IDC_RADIO_MATCHING_ANGLE_REF_X, _T("Horizontal"));
		SetDlgItemText(IDC_RADIO_MATCHING_ANGLE_REF_Y, _T("Vertical"));
		SetDlgItemText(IDC_BUTTON_CHANGE_ANGLE_COMP, _T("Apply"));

		SetDlgItemText(IDC_BUTTON_CREATE_GLOBAL_MATCHING_MODEL, _T("Create matching model"));
		SetDlgItemText(IDC_BUTTON_DELETE_GLOBAL_MATCHING_MODEL, _T("Delete matching model"));
		SetDlgItemText(IDC_BUTTON_IMAGE_ROTATION, _T("Horizontal rotation(teaching)"));
		SetDlgItemText(IDC_BUTTON_TEST_GLOBAL_MATCHING_MODEL, _T("Matching test"));
		SetDlgItemText(IDC_BUTTON_AFFINE_IMAGE, _T("Affine image(move, rotation)"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG18, _T("x(pixel)"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG19, _T("y(pixel)"));
		SetDlgItemText(IDC_STATIC_GLOBALMATCHINGDLG20, _T("Rotate(deg)"));
	}
}

void CGlobalMatchingDlg::LoadViewParam()
{
	m_bCheckUseMatchingShape = FALSE;
	m_bCheckUseMatchingNCC = FALSE;
	m_bCheckUseMatchingPerspective = FALSE;

	int iTemp;
	iTemp = THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[m_iCurImageIndex];

	if (iTemp == ALIGN_MATCHING_SHAPE)
		m_bCheckUseMatchingShape = TRUE;
	else if (iTemp == ALIGN_MATCHING_TEMPLATE)
		m_bCheckUseMatchingNCC = TRUE;
	else
		m_bCheckUseMatchingPerspective = TRUE;

	m_iCurrentShapeModel = iTemp;

	m_iEditMatchingPyramidLevel = THEAPP.m_pModelDataManager->m_iLocalMatchingPyramidLevel[m_iCurImageIndex];
	m_iEditMatchingAngleRange = THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex];
	m_dEditMatchingScaleMin = THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[m_iCurImageIndex];
	m_dEditMatchingScaleMax = THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[m_iCurImageIndex];
	m_iEditMatchingTeachingContrast = THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrast[m_iCurImageIndex];
	m_iEditMatchingTeachingContrastLow = THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrastLow[m_iCurImageIndex];
	m_iEditMatchingTeachingContrastMinSize = THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrastMinSize[m_iCurImageIndex];
	m_dEditMatchingScore = THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex];
	m_iEditMatchingInspectionMinContrast = THEAPP.m_pModelDataManager->m_iLocalMatchingInspectionMinContrast[m_iCurImageIndex];
	m_iEditMatchingSearchMarginX = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex];
	m_iEditMatchingSearchMarginY = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex];

	m_bCheckUseMatchingAffineConstant = THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[m_iCurImageIndex];

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		m_bCheckAlignImage[i] = THEAPP.m_pModelDataManager->m_bLocalAlignImage[m_iCurImageIndex][i];

	m_iRadioAlignProcessChType = THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex];
	m_bCheckUseMatchingFilter = THEAPP.m_pModelDataManager->m_bUseMatchingFilter[m_iCurImageIndex];

	m_cbMatchingFilterType.SetCurSel(THEAPP.m_pModelDataManager->m_iMatchingFilterType[m_iCurImageIndex]);
	m_dEditMatchingFilterTypeXSize = THEAPP.m_pModelDataManager->m_dMatchingFilterTypeXSize[m_iCurImageIndex];
	m_dEditMatchingFilterTypeYSize = THEAPP.m_pModelDataManager->m_dMatchingFilterTypeYSize[m_iCurImageIndex];

	m_bCheckUseMatchingAngleComp = THEAPP.m_pModelDataManager->m_bUseMatchingAngleComp[m_iCurImageIndex];
	m_iRadioMatchingAngleRefLine = THEAPP.m_pModelDataManager->m_iMatchingAngleRefLine[m_iCurImageIndex];

	m_bCheckUseMatchingXYComp = THEAPP.m_pModelDataManager->m_bCheckUseMatchingXYComp[m_iCurImageIndex];
	m_iLocalIndexNumber = THEAPP.m_pModelDataManager->m_iLocalIndexNumber[m_iCurImageIndex];

	SetControlState(m_iCurrentShapeModel);

	UpdateData(FALSE);
}

void CGlobalMatchingDlg::SetControlState(int iMatchingType)
{
	if (iMatchingType == ALIGN_MATCHING_SHAPE)
	{
		GetDlgItem(IDC_EDIT_MATCHING_SCALE_MIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MATCHING_SCALE_MAX)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST_LOW)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST_MIN_SIZE)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUTTON_SHOW_CONTRAST)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MATCHING_INSPECTION_MIN_CONTRAST)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUTTON_IMAGE_ROTATION)->EnableWindow(TRUE);
	}
	else if (iMatchingType == ALIGN_MATCHING_TEMPLATE)
	{
		GetDlgItem(IDC_EDIT_MATCHING_SCALE_MIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_SCALE_MAX)->EnableWindow(FALSE);

		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST_LOW)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST_MIN_SIZE)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_SHOW_CONTRAST)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_INSPECTION_MIN_CONTRAST)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_IMAGE_ROTATION)->EnableWindow(FALSE);
	}
	else if (iMatchingType == ALIGN_MATCHING_PERSPECTIVE)
	{
		GetDlgItem(IDC_EDIT_MATCHING_SCALE_MIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MATCHING_SCALE_MAX)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST_LOW)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST_MIN_SIZE)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_SHOW_CONTRAST)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_INSPECTION_MIN_CONTRAST)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_IMAGE_ROTATION)->EnableWindow(FALSE);
	}
}

void CGlobalMatchingDlg::OnBnClickedButtonCreateGlobalMatchingModel()
{
	try
	{
		if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
			return;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex]) == FALSE)
		{
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("티칭 영상 그랩 후 진행해 주세요.");
			else
				strMessageBox.Format("Proceed after grab the teaching image.");
			AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
			return;
		}

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
		{
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("Global align ROI가 존재하지 않습니다.");
			else
				strMessageBox.Format("The global align ROI does not exist.");
			AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
			return;
		}

		int nRes = 0;
		nRes = AfxMessageBox("현재 설정으로 매칭 모델을 새로이 생성하시겠습니까?", MB_YESNO | MB_SYSTEMMODAL);

		if ((nRes != IDYES))
			return;

		UpdateData();

		// Create Matching Model

		GTRegion *pInspectROIRgn;
		HObject HAlignROIRgn, HROIHRegion;

		GenEmptyObj(&HAlignROIRgn);

		for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			// 0번 영상에 있는 Align ROI만 추가

			if (pInspectROIRgn->miTeachImageIndex != (m_iCurImageIndex + 1))
				continue;

			if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_GLOBAL_ALIGN)
				continue;

			HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

			if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
				ConcatObj(HAlignROIRgn, HROIHRegion, &HAlignROIRgn);
			else
				continue;
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignROIRgn))
			Union1(HAlignROIRgn, &HAlignROIRgn);
		else
		{
			AfxMessageBox("매칭 모델 실패: Align ROI가 존재하지 않습니다. Align ROI를 티칭해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] >= 0)
		{
			if (m_iCurrentShapeModel == ALIGN_MATCHING_SHAPE)
				ClearShapeModel(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]);
			else if (m_iCurrentShapeModel == ALIGN_MATCHING_TEMPLATE)
				ClearNccModel(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]);
			else
				ClearDeformableModel(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]);

			THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] = -1;
		}

		// Model Ref: G-Center of Domain
		double dTeachAlignRefX, dTeachAlignRefY;
		Hlong lArea;
		HTuple HlArea, HdTeachAlignRefY, HdTeachAlignRefX;
		AreaCenter(HAlignROIRgn, &HlArea, &HdTeachAlignRefY, &HdTeachAlignRefX);
		lArea = HlArea.L();
		dTeachAlignRefY = HdTeachAlignRefY.D();
		dTeachAlignRefX = HdTeachAlignRefX.D();

		if (lArea < 500)
		{
			AfxMessageBox("매칭 모델 실패: Align ROI가 너무 작습니다. Align ROI를 추가해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		HObject HImageReduced;
		int iTypeTest = THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex];
		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex], HAlignROIRgn, &HImageReduced);

		HTuple dAngleRangeRad;
		TupleRad(THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex], &dAngleRangeRad);

		if (m_bCheckUseMatchingNCC)
		{
			if (m_iEditMatchingPyramidLevel == 0)
				CreateNccModel(HImageReduced, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", "use_polarity", &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
			else
				CreateNccModel(HImageReduced, m_iEditMatchingPyramidLevel, -dAngleRangeRad, dAngleRangeRad*2.0, "auto", "use_polarity", &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
		}
		else if (m_bCheckUseMatchingShape)
		{
			HTuple HContrast;
			TupleGenConst(0, 0, &HContrast);
			TupleConcat(HContrast, m_iEditMatchingTeachingContrastLow, &HContrast);
			TupleConcat(HContrast, m_iEditMatchingTeachingContrast, &HContrast);
			TupleConcat(HContrast, m_iEditMatchingTeachingContrastMinSize, &HContrast);

			if (THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex] == CHANNEL_TYPE_COLOR)
			{
				if (m_iEditMatchingPyramidLevel == 0) //ignore_color_polarity
					CreateScaledShapeModel(HImageReduced, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditMatchingScaleMin, m_dEditMatchingScaleMax, "auto", "auto", "ignore_color_polarity", HContrast, m_iEditMatchingInspectionMinContrast, &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
				else
					CreateScaledShapeModel(HImageReduced, m_iEditMatchingPyramidLevel, -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditMatchingScaleMin, m_dEditMatchingScaleMax, "auto", "auto", "ignore_color_polarity", HContrast, m_iEditMatchingInspectionMinContrast, &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
			}
			else
			{
				if (m_iEditMatchingPyramidLevel == 0)
					CreateScaledShapeModel(HImageReduced, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditMatchingScaleMin, m_dEditMatchingScaleMax, "auto", "auto", "use_polarity", HContrast, m_iEditMatchingInspectionMinContrast, &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
				else
					CreateScaledShapeModel(HImageReduced, m_iEditMatchingPyramidLevel, -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditMatchingScaleMin, m_dEditMatchingScaleMax, "auto", "auto", "use_polarity", HContrast, m_iEditMatchingInspectionMinContrast, &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
			}
		}
		else if (m_bCheckUseMatchingPerspective)
		{
			SetDeformableModelOrigin(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], -dTeachAlignRefY, -dTeachAlignRefX);

			if (m_iEditMatchingPyramidLevel == 0)
				CreatePlanarUncalibDeformableModel(HImageReduced, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditMatchingScaleMin, m_dEditMatchingScaleMax, "auto", m_dEditMatchingScaleMin, m_dEditMatchingScaleMax, "auto", "none", "use_polarity", "auto", "auto", HTuple(), HTuple(), &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
			else
				CreatePlanarUncalibDeformableModel(HImageReduced, m_iEditMatchingPyramidLevel, -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditMatchingScaleMin, m_dEditMatchingScaleMax, "auto", m_dEditMatchingScaleMin, m_dEditMatchingScaleMax, "auto", "none", "use_polarity", "auto", "auto", HTuple(), HTuple(), &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
		}

		Hlong lNoFoundNumber;
		HTuple Row, Column, Angle, Scale, Score;
		HTuple HomMat2D;

		if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] >= 0)
		{
			Row = HTuple(0.0);
			Column = HTuple(0.0);
			Angle = HTuple(0.0);
			Scale = HTuple(0.0);
			Score = HTuple(0.0);

			int iNoTeachNumLevel = 0;
			HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

			if (m_bCheckUseMatchingNCC)
			{
				GetNccModelParams(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &Metric);

				iNoTeachNumLevel = NumLevels[0].L();
				if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_TEMPALTE)
					iNoTeachNumLevel = MAX_PYRAMID_LEVEL_TEMPALTE;
				else
					iNoTeachNumLevel = 0;

				FindNccModel(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex],
					THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex],	// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,							// Extent of the rotation angles.
					m_dEditMatchingScore,						// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"true",										// Subpixel accuracy
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&Score);									// Score of the found instances of the model.
			}
			else if (m_bCheckUseMatchingShape)
			{
				GetShapeModelParams(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

				iNoTeachNumLevel = NumLevels[0].L();
				if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_SHAPE)
					iNoTeachNumLevel = MAX_PYRAMID_LEVEL_SHAPE;
				else
					iNoTeachNumLevel = 0;

				FindScaledShapeModel(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex],
					THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex],	// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,							// Extent of the rotation angles.
					m_dEditMatchingScaleMin,
					m_dEditMatchingScaleMax,
					m_dEditMatchingScore,						// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&Scale,
					&Score);									// Score of the found instances of the model.
			}
			else if (m_bCheckUseMatchingPerspective)
			{
				FindPlanarUncalibDeformableModel(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex],
					THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex],	// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,							// Extent of the rotation angles.
					m_dEditMatchingScaleMin,
					m_dEditMatchingScaleMax,
					m_dEditMatchingScaleMin,
					m_dEditMatchingScaleMax,
					m_dEditMatchingScore,						// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					0,											// Number of pyramid levels used in the matching
					0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
					HTuple(),
					HTuple(),
					&HomMat2D,									// Homographies.
					&Score);									// Score of the found instances of the model.
			}

			HTuple HlNoFoundNumber;
			TupleLength(Score, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			if (lNoFoundNumber > 0)
			{
				if (m_bCheckUseMatchingNCC)
					m_iCurrentShapeModel = ALIGN_MATCHING_TEMPLATE;
				else if (m_bCheckUseMatchingShape)
					m_iCurrentShapeModel = ALIGN_MATCHING_SHAPE;
				else if (m_bCheckUseMatchingPerspective)
					m_iCurrentShapeModel = ALIGN_MATCHING_PERSPECTIVE;

				// Save Parameter

				CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

				CString sModelBasePath;
				sModelBasePath.Format("%s\\SW\\Vision_N%d\\ModelBase.ini", strModelFolder, THEAPP.m_iCurTeachVision + 1);

				CIniFileCS INI(sModelBasePath);
				CString strSection = "Local Align";

				CString sTemp;

				THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[m_iCurImageIndex] = m_iCurrentShapeModel;
				sTemp.Format("m_iLocalMatchingMethod_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, m_iCurrentShapeModel);

				//////////////////////////////////////////////////////////////////////////
				/// For NCC
				HObject HModelRectRgn;
				Hlong lRectLTY, lRectLTX, lRectRBY, lRectRBX;
				HTuple HlRectLTY, HlRectLTX, HlRectRBY, HlRectRBX;

				ShapeTrans(HAlignROIRgn, &HModelRectRgn, "rectangle1");
				SmallestRectangle1(HModelRectRgn, &HlRectLTY, &HlRectLTX, &HlRectRBY, &HlRectRBX);

				lRectLTY = HlRectLTY.L();
				lRectLTX = HlRectLTX.L();
				lRectRBY = HlRectRBY.L();
				lRectRBX = HlRectRBX.L();

				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTX[m_iCurImageIndex] = lRectLTX;
				sTemp.Format("m_iLocalMatchingTeachingRectLTX_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, lRectLTX);
				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTY[m_iCurImageIndex] = lRectLTY;
				sTemp.Format("m_iLocalMatchingTeachingRectLTY_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, lRectLTY);
				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBX[m_iCurImageIndex] = lRectRBX;
				sTemp.Format("m_iLocalMatchingTeachingRectRBX_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, lRectRBX);
				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBY[m_iCurImageIndex] = lRectRBY;
				sTemp.Format("m_iLocalMatchingTeachingRectRBY_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, lRectRBY);
				//////////////////////////////////////////////////////////////////////////

				THEAPP.m_pModelDataManager->m_iLocalMatchingPyramidLevel[m_iCurImageIndex] = m_iEditMatchingPyramidLevel;
				sTemp.Format("m_iLocalMatchingPyramidLevel_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, m_iEditMatchingPyramidLevel);
				THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex] = m_iEditMatchingAngleRange;
				sTemp.Format("m_iLocalMatchingAngleRange_%d", m_iCurImageIndex + 1);
				INI.Set_Double(strSection, sTemp, m_iEditMatchingAngleRange);
				THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[m_iCurImageIndex] = m_dEditMatchingScaleMin;
				sTemp.Format("m_dLocalMatchingScaleMin_%d", m_iCurImageIndex + 1);
				INI.Set_Double(strSection, sTemp, m_dEditMatchingScaleMin);
				THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[m_iCurImageIndex] = m_dEditMatchingScaleMax;
				sTemp.Format("m_dLocalMatchingScaleMax_%d", m_iCurImageIndex + 1);
				INI.Set_Double(strSection, sTemp, m_dEditMatchingScaleMax);

				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrast[m_iCurImageIndex] = m_iEditMatchingTeachingContrast;
				sTemp.Format("m_iLocalMatchingTeachingContrast_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, m_iEditMatchingTeachingContrast);
				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrastLow[m_iCurImageIndex] = m_iEditMatchingTeachingContrastLow;
				sTemp.Format("m_iLocalMatchingTeachingContrastLow_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, m_iEditMatchingTeachingContrastLow);
				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrastMinSize[m_iCurImageIndex] = m_iEditMatchingTeachingContrastMinSize;
				sTemp.Format("m_iLocalMatchingTeachingContrastMinSize_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, m_iEditMatchingTeachingContrastMinSize);

				THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex] = m_dEditMatchingScore;
				sTemp.Format("m_dLocalMatchingScore_%d", m_iCurImageIndex + 1);
				INI.Set_Double(strSection, sTemp, m_dEditMatchingScore);
				THEAPP.m_pModelDataManager->m_iLocalMatchingInspectionMinContrast[m_iCurImageIndex] = m_iEditMatchingInspectionMinContrast;
				sTemp.Format("m_iLocalMatchingInspectionMinContrast_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, m_iEditMatchingInspectionMinContrast);

				THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex] = m_iEditMatchingSearchMarginX;
				sTemp.Format("m_iLocalMatchingSearchMarginX_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, m_iEditMatchingSearchMarginX);
				THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex] = m_iEditMatchingSearchMarginY;
				sTemp.Format("m_iLocalMatchingSearchMarginY_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, m_iEditMatchingSearchMarginY);

				THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[m_iCurImageIndex] = dTeachAlignRefX;
				sTemp.Format("m_dLocalTeachAlignRefX_%d", m_iCurImageIndex + 1);
				INI.Set_Double(strSection, sTemp, dTeachAlignRefX);
				THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[m_iCurImageIndex] = dTeachAlignRefY;
				sTemp.Format("m_dLocalTeachAlignRefY_%d", m_iCurImageIndex + 1);
				INI.Set_Double(strSection, sTemp, dTeachAlignRefY);

				THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[m_iCurImageIndex] = m_bCheckUseMatchingAffineConstant;
				sTemp.Format("m_bUseMatchingAffineConstant_%d", m_iCurImageIndex + 1);
				INI.Set_Bool(strSection, sTemp, m_bCheckUseMatchingAffineConstant);

				for (int j = 0; j < MAX_IMAGE_TAB; j++)
				{
					THEAPP.m_pModelDataManager->m_bLocalAlignImage[m_iCurImageIndex][j] = m_bCheckAlignImage[j];

					sTemp.Format("m_bLocalAlignImage_%d_%d", m_iCurImageIndex + 1, j + 1);
					INI.Set_Bool(strSection, sTemp, m_bCheckAlignImage[j]);
				}

				THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex] = m_iRadioAlignProcessChType;
				sTemp.Format("m_iLocalMatchingProcessChType_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, m_iRadioAlignProcessChType);

				THEAPP.m_pModelDataManager->m_bUseMatchingFilter[m_iCurImageIndex] = m_bCheckUseMatchingFilter;
				sTemp.Format("m_bLocalUseMatchingFilter_%d", m_iCurImageIndex + 1);
				INI.Set_Bool(strSection, sTemp, m_bCheckUseMatchingFilter);

				THEAPP.m_pModelDataManager->m_iMatchingFilterType[m_iCurImageIndex] = m_cbMatchingFilterType.GetCurSel();
				sTemp.Format("m_iMatchingFilterType_%d", m_iCurImageIndex + 1);
				INI.Set_Integer(strSection, sTemp, THEAPP.m_pModelDataManager->m_iMatchingFilterType[m_iCurImageIndex]);

				THEAPP.m_pModelDataManager->m_dMatchingFilterTypeXSize[m_iCurImageIndex] = m_dEditMatchingFilterTypeXSize;
				sTemp.Format("m_dMatchingFilterTypeXSize_%d", m_iCurImageIndex + 1);
				INI.Set_Double(strSection, sTemp, m_dEditMatchingFilterTypeXSize);

				THEAPP.m_pModelDataManager->m_dMatchingFilterTypeYSize[m_iCurImageIndex] = m_dEditMatchingFilterTypeYSize;
				sTemp.Format("m_dMatchingFilterTypeYSize_%d", m_iCurImageIndex + 1);
				INI.Set_Double(strSection, sTemp, m_dEditMatchingFilterTypeYSize);

				//////////////////////////////////////////////////////////////////////////
				// Save Global Align Model
				CString szTemplateFolder, sTemplateFolder;
				szTemplateFolder.Format("%s\\SW\\Vision_N%d\\MatchingModel", strModelFolder, THEAPP.m_iCurTeachVision + 1);

				sTemplateFolder.Format("%s\\LocalAlignModel_%d", szTemplateFolder, m_iCurImageIndex + 1);

				if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] >= 0)
				{
					if (m_bCheckUseMatchingNCC)
						WriteNccModel(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], HTuple(sTemplateFolder));
					else if (m_bCheckUseMatchingShape)
						WriteShapeModel(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], HTuple(sTemplateFolder));
					else if (m_bCheckUseMatchingPerspective)
						WriteDeformableModel(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], HTuple(sTemplateFolder));
				}

				AfxMessageBox("매칭 모델 생성 성공.", MB_SYSTEMMODAL);
			}
			else
			{
				AfxMessageBox("매칭 모델 생성 실패: 인식 실패", MB_SYSTEMMODAL);
			}
		}
		else
			AfxMessageBox("매칭 모델 생성 실패: 모델 생성 실패", MB_SYSTEMMODAL);
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [GlobalMatchingDlg::OnBnClickedButtonCreateMatchingModel] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		AfxMessageBox("매칭 모델 생성 실패: 모델 생성 실패", MB_SYSTEMMODAL);
	}
}

void CGlobalMatchingDlg::OnBnClickedButtonShowContrast()
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
			return;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex]) == FALSE)
		{
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("티칭 영상 그랩 후 진행해 주세요.");
			else
				strMessageBox.Format("Proceed after grab the teaching image.");
			AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
			return;
		}

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
		{
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("Global align ROI가 존재하지 않습니다.");
			else
				strMessageBox.Format("The global align ROI does not exist.");
			AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		GTRegion *pInspectROIRgn;
		HObject HAlignROIRgn, HROIHRegion;

		GenEmptyObj(&HAlignROIRgn);

		for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != (m_iCurImageIndex + 1))
				continue;

			if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_GLOBAL_ALIGN)
				continue;

			HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

			if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
				ConcatObj(HAlignROIRgn, HROIHRegion, &HAlignROIRgn);
			else
				continue;
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignROIRgn))
			Union1(HAlignROIRgn, &HAlignROIRgn);
		else
		{
			AfxMessageBox("매칭 모델 실패: Align ROI가 존재하지 않습니다. Align ROI를 티칭해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		BOOL bViewUpdate = FALSE;

		if (m_iEditMatchingTeachingContrast < 5)
		{
			m_iEditMatchingTeachingContrast = 5;
			bViewUpdate = TRUE;
		}

		if (m_iEditMatchingTeachingContrastLow > m_iEditMatchingTeachingContrast)
		{
			m_iEditMatchingTeachingContrastLow = m_iEditMatchingTeachingContrast;
			bViewUpdate = TRUE;
		}

		if (m_iEditMatchingTeachingContrastMinSize < 0)
		{
			m_iEditMatchingTeachingContrastMinSize = 1;
			bViewUpdate = TRUE;
		}

		if (bViewUpdate)
			UpdateData(FALSE);

		if (bDebugSave)
		{
			WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex], "bmp", 0, "c:\\DualTest\\InspectImage");
			WriteRegion(HAlignROIRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		HObject HFeatureImage, HImageReduced;

		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex], HAlignROIRgn, &HImageReduced);

		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HShapeModelContrastRgn));

		HTuple HContrast;
		TupleGenConst(0, 0, &HContrast);
		TupleConcat(HContrast, m_iEditMatchingTeachingContrastLow, &HContrast);
		TupleConcat(HContrast, m_iEditMatchingTeachingContrast, &HContrast);
		TupleConcat(HContrast, m_iEditMatchingTeachingContrastMinSize, &HContrast);

		//		InspectShapeModel(HImageReduced,&HFeatureImage,&(THEAPP.m_pInspectAdminViewDlg->m_HShapeModelContrastRgn), 1, HContrast);
		InspectShapeModel(HImageReduced, &HFeatureImage, &(THEAPP.m_pInspectAdminViewDlg->m_HShapeModelContrastRgn), 1, HTuple(m_iEditMatchingTeachingContrast));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [GlobalMatchingDlg::OnBnClickedButtonShowContrast] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}

void CGlobalMatchingDlg::OnBnClickedButtonChangeScore()
{
	try
	{
		UpdateData();

		// Save Parameter

		CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

		CString sModelBasePath;
		sModelBasePath.Format("%s\\SW\\Vision_N%d\\ModelBase.ini", strModelFolder, THEAPP.m_iCurTeachVision + 1);

		CIniFileCS INI(sModelBasePath);
		CString strSection = "Local Align";

		CString sTemp;

		THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex] = m_dEditMatchingScore;
		sTemp.Format("m_dLocalMatchingScore_%d", m_iCurImageIndex + 1);
		INI.Set_Double(strSection, sTemp, m_dEditMatchingScore);

		THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex] = m_iEditMatchingSearchMarginX;
		sTemp.Format("m_iLocalMatchingSearchMarginX_%d", m_iCurImageIndex + 1);
		INI.Set_Integer(strSection, sTemp, m_iEditMatchingSearchMarginX);
		THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex] = m_iEditMatchingSearchMarginY;
		sTemp.Format("m_iLocalMatchingSearchMarginY_%d", m_iCurImageIndex + 1);
		INI.Set_Integer(strSection, sTemp, m_iEditMatchingSearchMarginY);

		THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[m_iCurImageIndex] = m_bCheckUseMatchingAffineConstant;
		sTemp.Format("m_bUseMatchingAffineConstant_%d", m_iCurImageIndex + 1);
		INI.Set_Bool(strSection, sTemp, m_bCheckUseMatchingAffineConstant);

		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			THEAPP.m_pModelDataManager->m_bLocalAlignImage[m_iCurImageIndex][j] = m_bCheckAlignImage[j];

			sTemp.Format("m_bLocalAlignImage_%d_%d", m_iCurImageIndex + 1, j + 1);
			INI.Set_Bool(strSection, sTemp, m_bCheckAlignImage[j]);
		}

		THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex] = m_iRadioAlignProcessChType;
		sTemp.Format("m_iLocalMatchingProcessChType_%d", m_iCurImageIndex + 1);
		INI.Set_Integer(strSection, sTemp, m_iRadioAlignProcessChType);

		if (m_bCheckUseMatchingShape)	// for shape matching
		{
			if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] >= 0)
			{
				if (m_iCurrentShapeModel == ALIGN_MATCHING_SHAPE)
				{
					THEAPP.m_pModelDataManager->m_iLocalMatchingInspectionMinContrast[m_iCurImageIndex] = m_iEditMatchingInspectionMinContrast;
					sTemp.Format("m_iLocalMatchingInspectionMinContrast_%d", m_iCurImageIndex + 1);
					INI.Set_Integer(strSection, sTemp, m_iEditMatchingInspectionMinContrast);

					SetShapeModelParam(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], "min_contrast", m_iEditMatchingInspectionMinContrast);

					//////////////////////////////////////////////////////////////////////////
					// Save Global Align Model
					CString szTemplateFolder, sTemplateFolder;
					szTemplateFolder.Format("%s\\SW\\Vision_N%d\\MatchingModel", strModelFolder, THEAPP.m_iCurTeachVision + 1);

					sTemplateFolder.Format("%s\\LocalAlignModel_%d", szTemplateFolder, m_iCurImageIndex + 1);
					WriteShapeModel(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], HTuple(sTemplateFolder));
				}
				else
				{
					AfxMessageBox("현재 매칭 모델과 선택된 매칭 모델이 일치하지 않습니다. 매칭 모델 생성 후 재시도하십시요.", MB_SYSTEMMODAL | MB_ICONINFORMATION);
					return;
				}
			}
		}
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [GlobalMatchingDlg::OnBnClickedButtonChangeScore] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}


void CGlobalMatchingDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HShapeModelContrastRgn));

	CopyImage(m_HOrgTeachingImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][m_iCurImageIndex]));
	Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][m_iCurImageIndex], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][m_iCurImageIndex]));
	TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][m_iCurImageIndex], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][m_iCurImageIndex], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][m_iCurImageIndex], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][m_iCurImageIndex]), "hsi");

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	CDialog::OnClose();
}


void CGlobalMatchingDlg::OnBnClickedButtonImageRotation()
{
	if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
		return;

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex]) == FALSE)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("티칭 영상 그랩 후 진행해 주세요.");
		else
			strMessageBox.Format("Proceed after grab the teaching image.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("Global align ROI가 존재하지 않습니다.");
		else
			strMessageBox.Format("The global align ROI does not exist.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	int nRes = 0;
	nRes = AfxMessageBox("티칭영상이 변경되오니 주의하십시요. 현재 설정으로 영상을 회전하시겠습니까?", MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	UpdateData();

	GTRegion *pInspectROIRgn;
	HObject HAlignROIRgn, HROIHRegion;

	GenEmptyObj(&HAlignROIRgn);

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != (m_iCurImageIndex + 1))
			continue;

		if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_GLOBAL_ALIGN)
			continue;

		HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

		if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
			ConcatObj(HAlignROIRgn, HROIHRegion, &HAlignROIRgn);
		else
			continue;
	}

	if (THEAPP.m_pGFunction->ValidHRegion(HAlignROIRgn))
		Union1(HAlignROIRgn, &HAlignROIRgn);
	else
	{
		AfxMessageBox("Align ROI가 존재하지 않습니다. Align ROI를 티칭해 주세요.", MB_SYSTEMMODAL);
		return;
	}

	double dRect2CenterY, dRect2CenterX, dPhi, dLength1, dLength2;
	HTuple HdRect2CenterY, HdRect2CenterX, HdPhi, HdLength1, HdLength2;

	SmallestRectangle2(HAlignROIRgn, &HdRect2CenterY, &HdRect2CenterX, &HdPhi, &HdLength1, &HdLength2);

	dRect2CenterY = HdRect2CenterY.D();
	dRect2CenterX = HdRect2CenterX.D();
	dPhi = HdPhi.D();
	if (dPhi >= PI / 4)
		dPhi = dPhi - PI / 2;
	else if (dPhi < -PI / 4)
		dPhi = dPhi + PI / 2;
	dLength1 = HdLength1.D();
	dLength2 = HdLength2.D();

	HTuple HomMat2DIdentity, HomMat2DRotate;
	HomMat2dIdentity(&HomMat2DIdentity);
	HomMat2dRotate(HomMat2DIdentity, -dPhi, dRect2CenterY, dRect2CenterX, &HomMat2DRotate);

	CString strModelName, strFolder, strImageFolder, strImageName;

	strModelName = THEAPP.m_pModelDataManager->m_sModelName;
	strFolder = THEAPP.GetWorkingDirectory() + "\\Model\\";

	strImageFolder.Format("%s%s\\SW\\Vision_N%d\\TeachImage\\", strFolder, strModelName, THEAPP.m_iCurTeachVision + 1);

	CString TeachImageFileName, strTeachFileFullName;
	int i;

	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][i]) == FALSE)
			continue;

		if (m_bCheckAlignImage[i] == TRUE)
		{
			if (m_bCheckUseMatchingAffineConstant)
				AffineTransImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]), HomMat2DRotate, "constant", "false");
			else
				AffineTransImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]), HomMat2DRotate, "nearest_neighbor", "false");

			if (i == m_iCurImageIndex)
			{
				if (m_bCheckUseMatchingAffineConstant)
					AffineTransImage(m_HOrgTeachingImage, &m_HOrgTeachingImage, HomMat2DRotate, "constant", "false");
				else
					AffineTransImage(m_HOrgTeachingImage, &m_HOrgTeachingImage, HomMat2DRotate, "nearest_neighbor", "false");
			}

			Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i]));
			TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]), "hsi");

			TeachImageFileName.Format("Image_%d", i + 1);
			strTeachFileFullName = strImageFolder + "\\" + TeachImageFileName;
			WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], "jpeg 100", 0, HTuple(strTeachFileFullName));
		}
	}

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}


void CGlobalMatchingDlg::OnBnClickedButtonTestGlobalMatchingModel()
{
	if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
		return;

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex]) == FALSE)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("티칭 영상 그랩 후 진행해 주세요.");
		else
			strMessageBox.Format("Proceed after grab the teaching image.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("Global align ROI가 존재하지 않습니다.");
		else
			strMessageBox.Format("The global align ROI does not exist.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	try
	{
		UpdateData();

		double dStartTick = 0, dEndTick = 0;

		dStartTick = GetTickCount();

		if (m_bCheckUseMatchingNCC)
		{
			if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] < 0)
			{
				AfxMessageBox("현재 영상 탭에 대한 매칭 모델이 존재하지 않습니다. 먼저 매칭 모델을 생성해 주세요.", MB_SYSTEMMODAL);
				return;
			}

			HTuple dAngleRangeRad;
			TupleRad(THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex], &dAngleRangeRad);

			double dMatchingScore, dTeachAlignRefX, dTeachAlignRefY;
			dMatchingScore = THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex];
			dTeachAlignRefX = THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[m_iCurImageIndex];
			dTeachAlignRefY = THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[m_iCurImageIndex];

			int iMatchingSearchMarginX = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex];
			int iMatchingSearchMarginY = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex];

			Hlong lNoFoundNumber = 0;
			HTuple Row, Column, Angle, Score;
			HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate;

			double dTransX, dTransY, dRotationAngle;
			HObject HModelContour, HModelRgn, HMatchingImageReduced;

			GTRegion *pInspectROIRgn;
			HObject HAlignROIRgn, HROIHRegion;

			GenEmptyObj(&HAlignROIRgn);

			for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
				if (pInspectROIRgn == NULL)
					continue;

				// 0번 영상에 있는 Align ROI만 추가

				if (pInspectROIRgn->miTeachImageIndex != (m_iCurImageIndex + 1))
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_GLOBAL_ALIGN)
					continue;

				HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

				if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
					ConcatObj(HAlignROIRgn, HROIHRegion, &HAlignROIRgn);
				else
					continue;
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HAlignROIRgn))
				Union1(HAlignROIRgn, &HAlignROIRgn);
			else
			{
				AfxMessageBox("Align ROI가 존재하지 않습니다. Align ROI를 티칭해 주세요.", MB_SYSTEMMODAL);
				return;
			}

			ShapeTrans(HAlignROIRgn, &HModelRgn, "rectangle1");
			DilationRectangle1(HModelRgn, &HModelRgn, iMatchingSearchMarginX * 2 + 1, iMatchingSearchMarginY * 2 + 1);
			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex], HModelRgn, &HMatchingImageReduced);

			int iNoTeachNumLevel = 0;
			HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

			GetNccModelParams(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &Metric);

			iNoTeachNumLevel = NumLevels[0].L();
			if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_TEMPALTE)
				iNoTeachNumLevel = MAX_PYRAMID_LEVEL_TEMPALTE;
			else
				iNoTeachNumLevel = 0;

			Row = HTuple(0.0);
			Column = HTuple(0.0);
			Angle = HTuple(0.0);
			Score = HTuple(0.0);

			FindNccModel(HMatchingImageReduced,
				THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex],		// Handle of the model
				-dAngleRangeRad,							// Smallest rotation of the model
				dAngleRangeRad*2.0,							// Extent of the rotation angles.
				dMatchingScore,								// Minumum score of the instances of the model to be found
				1,											// Number of instances of the model to be found
				0.5,										// Maximum overlap of the instances of the model to be found
				"true",										// Subpixel accuracy
				iNoTeachNumLevel,							// Number of pyramid levels used in the matching
				&Row,										// Row coordinate of the found instances of the model.
				&Column,									// Column coordinate of the found instances of the model.
				&Angle,										// Rotation angle of the found instances of the model. (radian)
				&Score);									// Score of the found instances of the model.

			HTuple HlNoFoundNumber;
			TupleLength(Score, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			if (lNoFoundNumber <= 0)
			{
				AfxMessageBox("매칭 실패: 파라미터를 변경하시거나 새로운 매칭 모델을 등록해주세요.", MB_SYSTEMMODAL);
				return;
			}

			double dDeltaX, dDeltaY, dAngle, dScore;
			dDeltaX = Column[0].D();
			dDeltaY = Row[0].D();
			dAngle = Angle[0].D();
			dScore = Score[0].D();

			VectorAngleToRigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);

			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			HObject HResultXLD;
			AffineTransRegion(HAlignROIRgn, &HResultXLD, HomMat2DRotate, "nearest_neighbor");
			GenContourRegionXld(HResultXLD, &HResultXLD, "border");

			if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
				ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			THEAPP.m_pInspectAdminViewDlg->UpdateView();

			dTransX = Column[0].D() - dTeachAlignRefX;
			dTransY = Row[0].D() - dTeachAlignRefY;
			TupleDeg(Angle, &Angle);
			dRotationAngle = Angle[0].D();

			dEndTick = GetTickCount();

			CString sMsg;
			sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), 회전각도(Deg)=(%.2lf), 매칭율(%%)=%.1lf, 매칭시간=%.0lf ms", (int)dTransX, (int)dTransY, dRotationAngle, dScore*100.0, dEndTick - dStartTick);
			AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);
		}
		else if (m_bCheckUseMatchingShape)
		{
			if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] < 0)
			{
				AfxMessageBox("현재 영상 탭에 대한 매칭 모델이 존재하지 않습니다. 먼저 매칭 모델을 생성해 주세요.", MB_SYSTEMMODAL);
				return;
			}

			HTuple dAngleRangeRad;
			TupleRad(THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex], &dAngleRangeRad);

			double dMatchingScaleMin, dMatchingScaleMax, dMatchingScore, dTeachAlignRefX, dTeachAlignRefY;
			dMatchingScaleMin = THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[m_iCurImageIndex];
			dMatchingScaleMax = THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[m_iCurImageIndex];
			dMatchingScore = THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex];
			dTeachAlignRefX = THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[m_iCurImageIndex];
			dTeachAlignRefY = THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[m_iCurImageIndex];

			int iMatchingSearchMarginX = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex];
			int iMatchingSearchMarginY = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex];

			Hlong lNoFoundNumber = 0;
			HTuple Row, Column, Angle, Scale, Score;
			HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate;

			double dTransX, dTransY, dRotationAngle;
			HObject HModelContour, HModelRgn, HMatchingImageReduced;

			GetShapeModelContours(&HModelContour, THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], 1);
			GenRegionContourXld(HModelContour, &HModelRgn, "filled");
			Union1(HModelRgn, &HModelRgn);
			ShapeTrans(HModelRgn, &HModelRgn, "rectangle1");
			DilationRectangle1(HModelRgn, &HModelRgn, iMatchingSearchMarginX * 2 + 1, iMatchingSearchMarginY * 2 + 1);
			MoveRegion(HModelRgn, &HModelRgn, (Hlong)dTeachAlignRefY, (Hlong)dTeachAlignRefX);
			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex], HModelRgn, &HMatchingImageReduced);

			Row = HTuple(0.0);
			Column = HTuple(0.0);
			Angle = HTuple(0.0);
			Scale = HTuple(0.0);
			Score = HTuple(0.0);

			FindScaledShapeModel(HMatchingImageReduced,
				THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex],		// Handle of the model
				-dAngleRangeRad,							// Smallest rotation of the model
				dAngleRangeRad*2.0,							// Extent of the rotation angles.
				dMatchingScaleMin,
				dMatchingScaleMax,
				dMatchingScore,								// Minumum score of the instances of the model to be found
				1,											// Number of instances of the model to be found
				0.5,										// Maximum overlap of the instances of the model to be found
				"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
				0,											// Number of pyramid levels used in the matching
				0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
				&Row,										// Row coordinate of the found instances of the model.
				&Column,									// Column coordinate of the found instances of the model.
				&Angle,										// Rotation angle of the found instances of the model. (radian)
				&Scale,
				&Score);									// Score of the found instances of the model.

			HTuple HlNoFoundNumber;
			TupleLength(Score, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			if (lNoFoundNumber <= 0)
			{
				AfxMessageBox("매칭 실패: 파라미터를 변경하시거나 새로운 매칭 모델을 등록해주세요.", MB_SYSTEMMODAL);
				return;
			}

			double dDeltaX, dDeltaY, dAngle, dScore;
			dDeltaX = Column[0].D();
			dDeltaY = Row[0].D();
			dAngle = Angle[0].D();
			dScore = Score[0].D();

			HomMat2dIdentity(&HomMat2DIdentity);
			HomMat2dTranslate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
			HomMat2dRotate(HomMat2DTranslate, dAngle, dDeltaY, dDeltaX, &HomMat2DRotate);

			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			HObject HResultXLD;
			AffineTransContourXld(HModelContour, &HResultXLD, HomMat2DRotate);

			if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
				ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			THEAPP.m_pInspectAdminViewDlg->UpdateView();

			dTransX = Column[0].D() - dTeachAlignRefX;
			dTransY = Row[0].D() - dTeachAlignRefY;
			TupleDeg(Angle, &Angle);
			dRotationAngle = Angle[0].D();

			dEndTick = GetTickCount();

			CString sMsg;
			sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), 회전각도(Deg)=(%.2lf), 매칭율(%%)=%.1lf, 매칭시간=%.0lf ms", (int)dTransX, (int)dTransY, dRotationAngle, dScore*100.0, dEndTick - dStartTick);
			AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);
		}
		else if (m_bCheckUseMatchingPerspective)
		{
			if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] < 0)
			{
				AfxMessageBox("현재 영상 탭에 대한 매칭 모델이 존재하지 않습니다. 먼저 매칭 모델을 생성해 주세요.", MB_SYSTEMMODAL);
				return;
			}

			HTuple dAngleRangeRad;
			TupleRad(THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex], &dAngleRangeRad);

			double dMatchingScaleMin, dMatchingScaleMax, dMatchingScore, dTeachAlignRefX, dTeachAlignRefY;
			dMatchingScaleMin = THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[m_iCurImageIndex];
			dMatchingScaleMax = THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[m_iCurImageIndex];
			dMatchingScore = THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex];
			dTeachAlignRefX = THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[m_iCurImageIndex];
			dTeachAlignRefY = THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[m_iCurImageIndex];

			int iMatchingSearchMarginX = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex];
			int iMatchingSearchMarginY = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex];

			Hlong lNoFoundNumber = 0;
			HTuple Row, Column, Angle, Scale, Score;
			HTuple HomMat2D;

			double dTransX, dTransY, dRotationAngle;
			HObject HModelContour, HModelRgn, HMatchingImageReduced;

			GetDeformableModelContours(&HModelContour, THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], 1);
			GenRegionContourXld(HModelContour, &HModelRgn, "filled");
			Union1(HModelRgn, &HModelRgn);
			ShapeTrans(HModelRgn, &HModelRgn, "rectangle1");
			DilationRectangle1(HModelRgn, &HModelRgn, iMatchingSearchMarginX * 2 + 1, iMatchingSearchMarginY * 2 + 1);
			MoveRegion(HModelRgn, &HModelRgn, (Hlong)dTeachAlignRefY, (Hlong)dTeachAlignRefX);
			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex], HModelRgn, &HMatchingImageReduced);

			FindPlanarUncalibDeformableModel(HMatchingImageReduced,
				THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex],		// Handle of the model
				-dAngleRangeRad,							// Smallest rotation of the model
				dAngleRangeRad*2.0,							// Extent of the rotation angles.
				dMatchingScaleMin,
				dMatchingScaleMax,
				dMatchingScaleMin,
				dMatchingScaleMax,
				dMatchingScore,								// Minumum score of the instances of the model to be found
				1,											// Number of instances of the model to be found
				0.5,										// Maximum overlap of the instances of the model to be found
				0,											// Number of pyramid levels used in the matching
				0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
				HTuple(),
				HTuple(),
				&HomMat2D,									// Homographies.
				&Score);									// Score of the found instances of the model.

			HTuple HlNoFoundNumber;
			TupleLength(Score, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			if (lNoFoundNumber <= 0)
			{
				AfxMessageBox("매칭 실패: 파라미터를 변경하시거나 새로운 매칭 모델을 등록해주세요.", MB_SYSTEMMODAL);
				return;
			}

			double dScore;
			dScore = Score[0].D();

			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			HObject HResultXLD;
			ProjectiveTransContourXld(HModelContour, &HResultXLD, HomMat2D);

			if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
				ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			THEAPP.m_pInspectAdminViewDlg->UpdateView();

			dEndTick = GetTickCount();

			CString sMsg;
			sMsg.Format("매칭율(%%)=%.1lf, 매칭시간=%.0lf ms", dScore*100.0, dEndTick - dStartTick);
			AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);
		}
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [GlobalMatchingDlg::OnBnClickedButtonTestMatchingModel] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}

void CGlobalMatchingDlg::OnBnClickedCheckUseShapeMatching()
{
	m_bCheckUseMatchingShape = TRUE;
	m_bCheckUseMatchingNCC = FALSE;
	m_bCheckUseMatchingPerspective = FALSE;

	GetDlgItem(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_COLOR)->EnableWindow(TRUE);

	UpdateData(FALSE);

	SetControlState(ALIGN_MATCHING_SHAPE);
}

void CGlobalMatchingDlg::OnClickedCheckUseTemplateMatching()
{
	m_bCheckUseMatchingShape = FALSE;
	m_bCheckUseMatchingNCC = TRUE;
	m_bCheckUseMatchingPerspective = FALSE;

	m_iRadioAlignProcessChType = CHANNEL_TYPE_I;

	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_I);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);

	GetDlgItem(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_COLOR)->EnableWindow(FALSE);

	UpdateData(FALSE);

	SetControlState(ALIGN_MATCHING_TEMPLATE);
}

void CGlobalMatchingDlg::OnBnClickedCheckUsePerspectiveMatching()
{
	m_bCheckUseMatchingShape = FALSE;
	m_bCheckUseMatchingNCC = FALSE;
	m_bCheckUseMatchingPerspective = TRUE;

	m_iRadioAlignProcessChType = CHANNEL_TYPE_I;

	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_I);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);

	GetDlgItem(IDC_RADIO_GLOBAL_MATCHING_PROCESS_IMAGE_COLOR)->EnableWindow(FALSE);

	UpdateData(FALSE);

	SetControlState(ALIGN_MATCHING_PERSPECTIVE);
}

void CGlobalMatchingDlg::OnBnClickedRadioProcessImageColor()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_COLOR);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}

void CGlobalMatchingDlg::OnBnClickedRadioProcessImageR()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_R);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CGlobalMatchingDlg::OnBnClickedRadioProcessImageG()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_G);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CGlobalMatchingDlg::OnBnClickedRadioProcessImageB()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_B);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CGlobalMatchingDlg::OnBnClickedRadioProcessImageH()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_H);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CGlobalMatchingDlg::OnBnClickedRadioProcessImageS()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_S);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CGlobalMatchingDlg::OnBnClickedRadioProcessImageI()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_I);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CGlobalMatchingDlg::OnBnClickedButtonShowPowImage()
{
	UpdateData();

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex]) == FALSE)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("티칭 영상 그랩 후 진행해 주세요.");
		else
			strMessageBox.Format("Proceed after grab the teaching image.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	if (m_bCheckUseMatchingFilter)
	{
		int iMatchingFilterType = m_cbMatchingFilterType.GetCurSel();
		double dEditMatchingFilterTypeXSize = m_dEditMatchingFilterTypeXSize;
		if (dEditMatchingFilterTypeXSize < 1)
			dEditMatchingFilterTypeXSize = 1;
		double dEditMatchingFilterTypeYSize = m_dEditMatchingFilterTypeYSize;
		if (dEditMatchingFilterTypeYSize < 1)
			dEditMatchingFilterTypeYSize = 1;

		THEAPP.m_pAlgorithm->GetMatchingPreprocessImage(m_HOrgTeachingImage, iMatchingFilterType, dEditMatchingFilterTypeXSize, dEditMatchingFilterTypeYSize, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][m_iCurImageIndex]));
	}
	else
	{
		CopyImage(m_HOrgTeachingImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][m_iCurImageIndex]));
	}

	Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][m_iCurImageIndex], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][m_iCurImageIndex]));
	TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][m_iCurImageIndex], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][m_iCurImageIndex], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][m_iCurImageIndex], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][m_iCurImageIndex]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][m_iCurImageIndex]), "hsi");

	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(m_iRadioAlignProcessChType);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}

void CGlobalMatchingDlg::OnBnClickedButtonAffineImage()
{
	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex]) == FALSE)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("티칭 영상 그랩 후 진행해 주세요.");
		else
			strMessageBox.Format("Proceed after grab the teaching image.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	int nRes = 0;
	nRes = AfxMessageBox("티칭영상이 변경되오니 주의하십시요. 현재 설정으로 영상을 이동하시겠습니까?", MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	UpdateData();

	HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate;

	HomMat2dIdentity(&HomMat2DIdentity);
	HomMat2dTranslate(HomMat2DIdentity, m_dEditDeltaY, m_dEditDeltaX, &HomMat2DTranslate);

	HTuple HRadian, HWidth, HHeight;
	TupleRad(m_dEditRotationAngle, &HRadian);

	GetImageSize(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex], &HWidth, &HHeight);

	HomMat2dRotate(HomMat2DTranslate, HRadian, HHeight / 2, HWidth / 2, &HomMat2DRotate);

	CString strModelName, strFolder, strImageFolder, strImageName;

	strModelName = THEAPP.m_pModelDataManager->m_sModelName;
	strFolder = THEAPP.GetWorkingDirectory() + "\\Model\\";

	strImageFolder.Format("%s%s\\SW\\Vision_N%d\\TeachImage\\", strFolder, strModelName, THEAPP.m_iCurTeachVision + 1);

	CString TeachImageFileName, strTeachFileFullName;
	int i;

	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][i]) == FALSE)
			continue;

		if (m_bCheckAlignImage[i] == TRUE)
		{
			if (m_bCheckUseMatchingAffineConstant)
				AffineTransImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]), HomMat2DRotate, "constant", "false");
			else
				AffineTransImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]), HomMat2DRotate, "nearest_neighbor", "false");

			if (i == m_iCurImageIndex)
			{
				if (m_bCheckUseMatchingAffineConstant)
					AffineTransImage(m_HOrgTeachingImage, &m_HOrgTeachingImage, HomMat2DRotate, "constant", "false");
				else
					AffineTransImage(m_HOrgTeachingImage, &m_HOrgTeachingImage, HomMat2DRotate, "nearest_neighbor", "false");
			}

			Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i]));
			TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]), "hsi");

			TeachImageFileName.Format("Image_%d", i + 1);
			strTeachFileFullName = strImageFolder + "\\" + TeachImageFileName;
			WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], "jpeg 100", 0, HTuple(strTeachFileFullName));
		}
	}

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}

void CGlobalMatchingDlg::OnBnClickedButtonChangeAngleComp()
{
	if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
		return;

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex]) == FALSE)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("티칭 영상 그랩 후 진행해 주세요.");
		else
			strMessageBox.Format("Proceed after grab the teaching image.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("Global align ROI가 존재하지 않습니다.");
		else
			strMessageBox.Format("The global align ROI does not exist.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	GTRegion *pInspectROIRgn;

	int iNoAlignAngleRoi = 0;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != (m_iCurImageIndex + 1))
			continue;

		if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN_ANGLE)
			continue;

		++iNoAlignAngleRoi;
	}

	if (iNoAlignAngleRoi != 2)
	{
		AfxMessageBox("회전 각도를 계산하기 위해 Align Angle ROI를 2개만 생성해 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	UpdateData();

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

	CString sModelBasePath;
	sModelBasePath.Format("%s\\SW\\Vision_N%d\\ModelBase.ini", strModelFolder, THEAPP.m_iCurTeachVision + 1);

	CIniFileCS INI(sModelBasePath);
	CString strSection = "Local Align";

	CString sTemp;

	THEAPP.m_pModelDataManager->m_bUseMatchingAngleComp[m_iCurImageIndex] = m_bCheckUseMatchingAngleComp;
	sTemp.Format("m_bUseMatchingAngleComp_%d", m_iCurImageIndex + 1);
	INI.Set_Bool(strSection, sTemp, m_bCheckUseMatchingAngleComp);

	THEAPP.m_pModelDataManager->m_iMatchingAngleRefLine[m_iCurImageIndex] = m_iRadioMatchingAngleRefLine;
	sTemp.Format("m_iMatchingAngleRefLine_%d", m_iCurImageIndex + 1);
	INI.Set_Integer(strSection, sTemp, m_iRadioMatchingAngleRefLine);

	AfxMessageBox("수정 완료.", MB_SYSTEMMODAL);
}

void CGlobalMatchingDlg::OnBnClickedButtonChangeXyComp()
{
	if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
		return;

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex]][m_iCurImageIndex]) == FALSE)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("티칭 영상 그랩 후 진행해 주세요.");
		else
			strMessageBox.Format("Proceed after grab the teaching image.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("Global align ROI가 존재하지 않습니다.");
		else
			strMessageBox.Format("The global align ROI does not exist.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
		return;
	}

	GTRegion *pInspectROIRgn;

	int iNoAlignAngleRoi = 0;

	UpdateData(TRUE);
	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != (m_iCurImageIndex + 1))
			continue;

		if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
		{
			if (pInspectROIRgn->miLocalAlignID != m_iLocalIndexNumber)
			{
				continue;
			}
		}
		++iNoAlignAngleRoi;
	}

	if (iNoAlignAngleRoi < 1)
	{
		AfxMessageBox("기준점(X,Y)을 계산하기 위해 Local ROI를 1개만 생성해 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	UpdateData();

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

	CString sModelBasePath;
	sModelBasePath.Format("%s\\SW\\Vision_N%d\\ModelBase.ini", strModelFolder, THEAPP.m_iCurTeachVision + 1);

	CIniFileCS INI(sModelBasePath);
	CString strSection = "Local Align";

	CString sTemp;

	THEAPP.m_pModelDataManager->m_bCheckUseMatchingXYComp[m_iCurImageIndex] = m_bCheckUseMatchingXYComp;
	sTemp.Format("m_bCheckUseMatchingXYComp_%d", m_iCurImageIndex + 1);
	INI.Set_Bool(strSection, sTemp, m_bCheckUseMatchingXYComp);

	THEAPP.m_pModelDataManager->m_iLocalIndexNumber[m_iCurImageIndex] = m_iLocalIndexNumber;
	sTemp.Format("m_iLocalIndexNumber_%d", m_iCurImageIndex + 1);
	INI.Set_Integer(strSection, sTemp, m_iLocalIndexNumber);

	THEAPP.m_pModelDataManager->m_iRadioMatchingXYRefLine[m_iCurImageIndex] = m_iRadioMatchingXYRefLine;
	sTemp.Format("m_iRadioMatchingXYRefLine_%d", m_iCurImageIndex + 1);
	INI.Set_Integer(strSection, sTemp, m_iRadioMatchingXYRefLine);

	AfxMessageBox("수정 완료.", MB_SYSTEMMODAL);
}

void CGlobalMatchingDlg::OnBnClickedButtonDeleteGlobalMatchingModel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("현재 영상의 매칭 모델을 삭제하시겠습니까?");
	else
		strMessageBox.Format("Delete the matching model of the current image?");
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\";
	CString strSWVisionFolder = strModelFolder + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\";
	strSWVisionFolder.Format("%sVision_N%d\\", strSWVisionFolder, THEAPP.m_iCurTeachVision + 1);

	CString strTemplateFolder;
	strTemplateFolder.Format("%sMatchingModel\\LocalAlignModel_%d", strSWVisionFolder, m_iCurImageIndex + 1);

	DeleteFileA((LPCSTR)strTemplateFolder);

	THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] = -1;

	// Save Parameter
	CIniFileCS INI_ModelBase(strSWVisionFolder + "ModelBase.ini");
	CString strSection, strKey;
	strSection = "Local Align";
	THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTX[m_iCurImageIndex] = -1;
	strKey.Format("m_iLocalMatchingTeachingRectLTX_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, -1);
	THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTY[m_iCurImageIndex] = -1;
	strKey.Format("m_iLocalMatchingTeachingRectLTY_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, -1);
	THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBX[m_iCurImageIndex] = -1;
	strKey.Format("m_iLocalMatchingTeachingRectRBX_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, -1);
	THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBY[m_iCurImageIndex] = -1;
	strKey.Format("m_iLocalMatchingTeachingRectRBY_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, -1);

	THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[m_iCurImageIndex] = 0;
	strKey.Format("m_iLocalMatchingMethod_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 0);

	THEAPP.m_pModelDataManager->m_bUseMatchingFilter[m_iCurImageIndex] = 0;
	strKey.Format("m_bLocalUseMatchingFilter_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Bool(strSection, strKey, 0);
	THEAPP.m_pModelDataManager->m_iMatchingFilterType[m_iCurImageIndex] = 0;
	strKey.Format("m_iMatchingFilterType_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 0);
	THEAPP.m_pModelDataManager->m_dMatchingFilterTypeXSize[m_iCurImageIndex] = 7.000000;
	strKey.Format("m_dMatchingFilterTypeXSize_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Double(strSection, strKey, 7.000000);
	THEAPP.m_pModelDataManager->m_dMatchingFilterTypeYSize[m_iCurImageIndex] = 7.000000;
	strKey.Format("m_dMatchingFilterTypeYSize_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Double(strSection, strKey, 7.000000);

	THEAPP.m_pModelDataManager->m_iLocalMatchingPyramidLevel[m_iCurImageIndex] = 0;
	strKey.Format("m_iLocalMatchingPyramidLevel_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 0);
	THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex] = 5.000000;
	strKey.Format("m_iLocalMatchingAngleRange_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Double(strSection, strKey, 5.000000);
	THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[m_iCurImageIndex] = 0.980000;
	strKey.Format("m_dLocalMatchingScaleMin_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Double(strSection, strKey, 0.980000);
	THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[m_iCurImageIndex] = 1.020000;
	strKey.Format("m_dLocalMatchingScaleMax_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Double(strSection, strKey, 1.020000);
	THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrast[m_iCurImageIndex] = 40;
	strKey.Format("m_iLocalMatchingTeachingContrast_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 40);
	THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrastLow[m_iCurImageIndex] = 40;
	strKey.Format("m_iLocalMatchingTeachingContrastLow_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 40);
	THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrastMinSize[m_iCurImageIndex] = 1;
	strKey.Format("m_iLocalMatchingTeachingContrastMinSize_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 1);

	THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex] = 0.300000;
	strKey.Format("m_dLocalMatchingScore_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Double(strSection, strKey, 0.300000);
	THEAPP.m_pModelDataManager->m_iLocalMatchingInspectionMinContrast[m_iCurImageIndex] = 15;
	strKey.Format("m_iLocalMatchingInspectionMinContrast_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 15);
	THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex] = 300;
	strKey.Format("m_iLocalMatchingSearchMarginX_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 300);
	THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex] = 300;
	strKey.Format("m_iLocalMatchingSearchMarginY_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 300);

	THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[m_iCurImageIndex] = 0;
	strKey.Format("m_bUseMatchingAffineConstant_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Bool(strSection, strKey, 0);

	for (int j = 0; j < MAX_IMAGE_TAB; j++)
	{
		THEAPP.m_pModelDataManager->m_bLocalAlignImage[m_iCurImageIndex][j] = 0;
		strKey.Format("m_bLocalAlignImage_%d_%d", m_iCurImageIndex + 1, j + 1);
		INI_ModelBase.Set_Bool(strSection, strKey, 0);
	}

	THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_iCurImageIndex] = 6;
	strKey.Format("m_iLocalMatchingProcessChType_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 6);

	THEAPP.m_pModelDataManager->m_bCheckUseMatchingXYComp[m_iCurImageIndex] = 0;
	strKey.Format("m_bCheckUseMatchingXYComp_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Bool(strSection, strKey, 0);
	THEAPP.m_pModelDataManager->m_iRadioMatchingXYRefLine[m_iCurImageIndex] = 0;
	strKey.Format("m_iRadioMatchingXYRefLine_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 0);
	THEAPP.m_pModelDataManager->m_iLocalIndexNumber[m_iCurImageIndex] = 0;
	strKey.Format("m_iLocalIndexNumber_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 0);

	THEAPP.m_pModelDataManager->m_bUseMatchingAngleComp[m_iCurImageIndex] = 0;
	strKey.Format("m_bUseMatchingAngleComp_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Bool(strSection, strKey, 0);
	THEAPP.m_pModelDataManager->m_iMatchingAngleRefLine[m_iCurImageIndex] = 0;
	strKey.Format("m_iMatchingAngleRefLine_%d", m_iCurImageIndex + 1);
	INI_ModelBase.Set_Integer(strSection, strKey, 0);

	LoadViewParam();
}
