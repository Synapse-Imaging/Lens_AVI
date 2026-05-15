// AlgorithmImageProcessDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmImageProcessDlg.h"
#include "afxdialogex.h"


// CAlgorithmImageProcessDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmImageProcessDlg, CDialog)

CAlgorithmImageProcessDlg::CAlgorithmImageProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmImageProcessDlg::IDD, pParent)
	, m_bCheckUseImageProcessPostLocalAlign(FALSE)
	, m_iEditImageProcessPostLocalAlignROINo(0)
	, m_iRadioImageProcessPostLocalAlignFitPos(0)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseImageProcess = FALSE;
	m_iRadioImageProcessArithmeticType = 0;
	m_iEditImageProcessArithmeticAdd = 0;
	m_dEditImageProcessArithmeticMulti = 0.0;
	m_iRadioProcessChType = CHANNEL_TYPE_I;
	m_iRadioProcessResType = RES_TYPE_5;

	m_bCheckUseImageProcessLocalAlign = FALSE;
	m_iEditImageProcessLocalAlignROINo = 1;
	m_bCheckImageProcessLocalAlignPosX = TRUE;
	m_bCheckImageProcessLocalAlignPosY = TRUE;
	m_bCheckImageProcessLocalAlignAngle = TRUE;
	m_iRadioImageProcessLocalAlignFitPos = 0;
	m_iRadioImageProcessPostLocalAlignFitPos = 0;
	m_iRadioImageProcessLocalAlignROIType = 0;
	m_iRadioImageProcessLocalAlignMatchingApplyMethod = 0;
	m_iEditImageProcessFilterType1X = 0;
	m_iEditImageProcessFilterType2X = 0;
	m_iEditImageProcessFilterType3X = 0;
	m_iEditImageProcessFilterType1Y = 0;
	m_iEditImageProcessFilterType2Y = 0;
	m_iEditImageProcessFilterType3Y = 0;
	m_bCheckUseImageProcessDontCare = FALSE;
	m_iEditImageProcessDontCareROINo = 1;
	m_iEditImageProcessDontCareMargin = 0;
	m_iEditImageProcessDontCareMarginInner = -5000;
	m_bCheckUseImageProcessDontCare2 = FALSE;
	m_iEditImageProcessDontCareROINo2 = 1;
	m_iEditImageProcessDontCareMargin2 = 0;
	m_iEditImageProcessDontCareMargin2Inner = -5000;
	m_bCheckUseImageProcessGenerate = FALSE;
	m_bCheckUseImageProcessGenerateInspectFlag = FALSE;
	m_iEditImageProcessGenerateROINo = 1;
	m_iEditImageProcessGenerateMargin = 0;
	m_bCheckUseImageProcessMask = FALSE;
	m_iEditImageProcessMaskROINo = 1;
	m_iEditImageProcessMaskROINo2 = 1;
	m_bCheckUseImageProcessMask2 = FALSE;
	m_bCheckUseImageProcessPostLocalAlign = FALSE;
	m_iEditImageProcessPostLocalAlignROINo = 1;

}

CAlgorithmImageProcessDlg::~CAlgorithmImageProcessDlg()
{
	for (int i = 0; i < CHANNEL_NUM; i++)
	{
		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			GenEmptyObj(&(m_HImageProcessImage[i][j]));
		}
	}
}

void CAlgorithmImageProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_PROCESS, m_bCheckUseImageProcess);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_PROCESS_ARITHMETIC_1, m_iRadioImageProcessArithmeticType);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_ARITHMETIC_IMAGE1, m_cbImageProcessArithmeticImage1);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_ARITHMETIC_IMAGE2, m_cbImageProcessArithmeticImage2);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_ARITHMETIC_IMAGE3, m_cbImageProcessArithmeticImage3);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_ARITHMETIC_IMAGE4, m_cbImageProcessArithmeticImage4);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_ARITHMETIC_ADD, m_iEditImageProcessArithmeticAdd);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_ARITHMETIC_MULTI, m_dEditImageProcessArithmeticMulti);

	DDX_Control(pDX, IDC_BUTTON_DISPLAY_PROCESS_IMAGE, m_bnDisplayProcessImage);
	DDX_Radio(pDX, IDC_RADIO_PROCESS_IMAGE_COLOR, m_iRadioProcessChType);
	DDX_Radio(pDX, IDC_RADIO_PROCESS_IMAGE_RES_1, m_iRadioProcessResType);

	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_PROCESS_LOCAL_ALIGN, m_bCheckUseImageProcessLocalAlign);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_LOCAL_ALIGN_IMAGE, m_cbImageProcessLocalAlignImageIndex);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_LOCAL_ALIGN_ROI, m_iEditImageProcessLocalAlignROINo);
	DDX_Check(pDX, IDC_CHECK_IMAGE_PROCESS_LOCAL_ALIGN_POS_X, m_bCheckImageProcessLocalAlignPosX);
	DDX_Check(pDX, IDC_CHECK_IMAGE_PROCESS_LOCAL_ALIGN_POS_Y, m_bCheckImageProcessLocalAlignPosY);
	DDX_Check(pDX, IDC_CHECK_IMAGE_PROCESS_LOCAL_ALIGN_ANGLE, m_bCheckImageProcessLocalAlignAngle);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_PROCESS_LOCAL_ALIGN_FIT_TOP, m_iRadioImageProcessLocalAlignFitPos);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_PROCESS_LOCAL_ALIGN_ROI_TYPE_MATCHING, m_iRadioImageProcessLocalAlignROIType);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_PROCESS_LOCAL_ALIGN_POS, m_iRadioImageProcessLocalAlignMatchingApplyMethod);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_PROCESS_FILTER, m_bCheckUseImageProcessFilter);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_FILTER_TYPE_1, m_cbImageProcessFilterType1);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_FILTER_TYPE_2, m_cbImageProcessFilterType2);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_FILTER_TYPE_3, m_cbImageProcessFilterType3);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_X_SIZE_1, m_iEditImageProcessFilterType1X);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_X_SIZE_2, m_iEditImageProcessFilterType2X);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_X_SIZE_3, m_iEditImageProcessFilterType3X);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_Y_SIZE_1, m_iEditImageProcessFilterType1Y);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_Y_SIZE_2, m_iEditImageProcessFilterType2Y);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_Y_SIZE_3, m_iEditImageProcessFilterType3Y);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_PROCESS_DONTCARE, m_bCheckUseImageProcessDontCare);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_DONTCARE_IMAGE, m_cbImageProcessDontCareImageIndex);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_DONTCARE_ROI, m_iEditImageProcessDontCareROINo);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_DONTCARE_MARGIN, m_iEditImageProcessDontCareMargin);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_DONTCARE_MARGIN_INNER, m_iEditImageProcessDontCareMarginInner);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_PROCESS_DONTCARE2, m_bCheckUseImageProcessDontCare2);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_DONTCARE_IMAGE2, m_cbImageProcessDontCareImageIndex2);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_DONTCARE_ROI2, m_iEditImageProcessDontCareROINo2);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_DONTCARE_MARGIN2, m_iEditImageProcessDontCareMargin2);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_DONTCARE_MARGIN2_INNER, m_iEditImageProcessDontCareMargin2Inner);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_PROCESS_GENERATE, m_bCheckUseImageProcessGenerate);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_PROCESS_GENERATE_INSPECT_FLAG, m_bCheckUseImageProcessGenerateInspectFlag);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_GENERATE_IMAGE, m_cbImageProcessGenerateImageIndex);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_GENERATE_ROI, m_iEditImageProcessGenerateROINo);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_GENERATE_MARGIN, m_iEditImageProcessGenerateMargin);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_PROCESS_MASK, m_bCheckUseImageProcessMask);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_MASK_IMAGE, m_cbImageProcessMaskImageIndex);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_MASK_ROI, m_iEditImageProcessMaskROINo);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_PROCESS_MASK2, m_bCheckUseImageProcessMask2);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_MASK_IMAGE2, m_cbImageProcessMaskImageIndex2);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_MASK_ROI2, m_iEditImageProcessMaskROINo2);

	DDX_Check(pDX, IDC_CHECK_IMAGE_PROCESS_LOCAL_ALIGN_ADD_LINE_FIT, m_bCheckUseImageProcessLocalAlignAddLineFit);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_LOCAL_ALIGN_IMAGE_2, m_cbImageProcessLocalAlignImageIndex_2);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_LOCAL_ALIGN_ROI_2, m_iEditImageProcessLocalAlignROINo_2);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_PROCESS_LOCAL_ALIGN_FIT_TOP_2, m_iRadioImageProcessLocalAlignFitPos_2);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_POST_ALING, m_bCheckUseImageProcessPostLocalAlign);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_LOCAL_POST_ALIGN_IMAGE, m_cbImageProcessPostLocalAlignImageIndex);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_POST_LOCAL_ALIGN_ROI, m_iEditImageProcessPostLocalAlignROINo);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_PROCESS_POST_LOCAL_ALIGN_FIT_MODELCENTER, m_iRadioImageProcessPostLocalAlignFitPos);
}

BEGIN_MESSAGE_MAP(CAlgorithmImageProcessDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAY_PROCESS_IMAGE, &CAlgorithmImageProcessDlg::OnBnClickedButtonDisplayProcessImage)
	ON_BN_CLICKED(IDC_RADIO_PROCESS_IMAGE_R, &CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageR)
	ON_BN_CLICKED(IDC_RADIO_PROCESS_IMAGE_G, &CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageG)
	ON_BN_CLICKED(IDC_RADIO_PROCESS_IMAGE_B, &CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageB)
	ON_BN_CLICKED(IDC_RADIO_PROCESS_IMAGE_H, &CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageH)
	ON_BN_CLICKED(IDC_RADIO_PROCESS_IMAGE_S, &CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageS)
	ON_BN_CLICKED(IDC_RADIO_PROCESS_IMAGE_I, &CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageI)
END_MESSAGE_MAP()


// CAlgorithmImageProcessDlg 메시지 처리기입니다.

void CAlgorithmImageProcessDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;

	for (int i = 0; i < CHANNEL_NUM; i++)
	{
		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][j]))
				CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][j], &(m_HImageProcessImage[i][j]));
			else
				GenEmptyObj(&(m_HImageProcessImage[i][j]));
		}
	}
}

void CAlgorithmImageProcessDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_iRadioProcessChType = AlgorithmParam.m_iProcessChType;
	m_iRadioProcessResType = AlgorithmParam.m_iProcessResType;
	m_bCheckUseImageProcess = AlgorithmParam.m_bUseImageProcess;
	m_iRadioImageProcessArithmeticType = AlgorithmParam.m_iImageProcessArithmeticType;
	m_cbImageProcessArithmeticImage1.SetCurSel(AlgorithmParam.m_iImageProcessArithmeticImage1);
	m_cbImageProcessArithmeticImage2.SetCurSel(AlgorithmParam.m_iImageProcessArithmeticImage2);
	m_cbImageProcessArithmeticImage3.SetCurSel(AlgorithmParam.m_iImageProcessArithmeticImage3);
	m_cbImageProcessArithmeticImage4.SetCurSel(AlgorithmParam.m_iImageProcessArithmeticImage4);
	m_iEditImageProcessArithmeticAdd = AlgorithmParam.m_iImageProcessArithmeticAdd;
	m_dEditImageProcessArithmeticMulti = AlgorithmParam.m_dImageProcessArithmeticMulti;

	m_bCheckUseImageProcessLocalAlign = AlgorithmParam.m_bUseImageProcessLocalAlign;
	m_cbImageProcessLocalAlignImageIndex.SetCurSel(AlgorithmParam.m_iImageProcessLocalAlignImageIndex);
	m_iEditImageProcessLocalAlignROINo = AlgorithmParam.m_iImageProcessLocalAlignROINo;
	m_bCheckImageProcessLocalAlignPosX = AlgorithmParam.m_bImageProcessLocalAlignPosX;
	m_bCheckImageProcessLocalAlignPosY = AlgorithmParam.m_bImageProcessLocalAlignPosY;
	m_bCheckImageProcessLocalAlignAngle = AlgorithmParam.m_bImageProcessLocalAlignAngle;
	m_iRadioImageProcessLocalAlignFitPos = AlgorithmParam.m_iImageProcessLocalAlignFitPos;
	m_iRadioImageProcessPostLocalAlignFitPos = AlgorithmParam.m_iImageProcessPostLocalAlignFitPos;
	m_iRadioImageProcessLocalAlignMatchingApplyMethod = AlgorithmParam.m_iImageProcessLocalAlignMatchingApplyMethod;
	m_iRadioImageProcessLocalAlignROIType = AlgorithmParam.m_iImageProcessLocalAlignROIType;
	m_bCheckUseImageProcessDontCare = AlgorithmParam.m_bUseImageProcessDontCare;
	m_cbImageProcessDontCareImageIndex.SetCurSel(AlgorithmParam.m_iImageProcessDontCareImageIndex);
	m_iEditImageProcessDontCareROINo = AlgorithmParam.m_iImageProcessDontCareROINo;
	m_iEditImageProcessDontCareMargin = AlgorithmParam.m_iImageProcessDontCareMargin;
	m_iEditImageProcessDontCareMarginInner = AlgorithmParam.m_iImageProcessDontCareMarginInner;
	m_bCheckUseImageProcessDontCare2 = AlgorithmParam.m_bUseImageProcessDontCare2;
	m_cbImageProcessDontCareImageIndex2.SetCurSel(AlgorithmParam.m_iImageProcessDontCareImageIndex2);
	m_iEditImageProcessDontCareROINo2 = AlgorithmParam.m_iImageProcessDontCareROINo2;
	m_iEditImageProcessDontCareMargin2 = AlgorithmParam.m_iImageProcessDontCareMargin2;
	m_iEditImageProcessDontCareMargin2Inner = AlgorithmParam.m_iImageProcessDontCareMargin2Inner;
	m_bCheckUseImageProcessGenerate = AlgorithmParam.m_bUseImageProcessGenerate;
	m_bCheckUseImageProcessGenerateInspectFlag = AlgorithmParam.m_bUseImageProcessGenerateInspectFlag;
	m_cbImageProcessGenerateImageIndex.SetCurSel(AlgorithmParam.m_iImageProcessGenerateImageIndex);
	m_iEditImageProcessGenerateROINo = AlgorithmParam.m_iImageProcessGenerateROINo;
	m_iEditImageProcessGenerateMargin = AlgorithmParam.m_iImageProcessGenerateMargin;
	m_bCheckUseImageProcessMask = AlgorithmParam.m_bUseImageProcessMask;
	m_cbImageProcessMaskImageIndex.SetCurSel(AlgorithmParam.m_iImageProcessMaskImageIndex);
	m_iEditImageProcessMaskROINo = AlgorithmParam.m_iImageProcessMaskROINo;
	m_bCheckUseImageProcessMask2 = AlgorithmParam.m_bUseImageProcessMask2;
	m_cbImageProcessMaskImageIndex2.SetCurSel(AlgorithmParam.m_iImageProcessMaskImageIndex2);
	m_iEditImageProcessMaskROINo2 = AlgorithmParam.m_iImageProcessMaskROINo2;
	m_bCheckUseImageProcessLocalAlignAddLineFit = AlgorithmParam.m_bUseImageProcessLocalAlignAddLineFit;
	m_cbImageProcessLocalAlignImageIndex_2.SetCurSel(AlgorithmParam.m_iImageProcessLocalAlignImageIndex_2);
	m_iEditImageProcessLocalAlignROINo_2 = AlgorithmParam.m_iImageProcessLocalAlignROINo_2;
	m_iRadioImageProcessLocalAlignFitPos_2 = AlgorithmParam.m_iImageProcessLocalAlignFitPos_2;

	m_bCheckUseImageProcessFilter = AlgorithmParam.m_bUseImageProcessFilter;
	m_cbImageProcessFilterType1.SetCurSel(AlgorithmParam.m_iImageProcessFilterType1);
	m_cbImageProcessFilterType2.SetCurSel(AlgorithmParam.m_iImageProcessFilterType2);
	m_cbImageProcessFilterType3.SetCurSel(AlgorithmParam.m_iImageProcessFilterType3);
	m_iEditImageProcessFilterType1X = AlgorithmParam.m_iImageProcessFilterType1X;
	m_iEditImageProcessFilterType1Y = AlgorithmParam.m_iImageProcessFilterType1Y;
	m_iEditImageProcessFilterType2X = AlgorithmParam.m_iImageProcessFilterType2X;
	m_iEditImageProcessFilterType2Y = AlgorithmParam.m_iImageProcessFilterType2Y;
	m_iEditImageProcessFilterType3X = AlgorithmParam.m_iImageProcessFilterType3X;
	m_iEditImageProcessFilterType3Y = AlgorithmParam.m_iImageProcessFilterType3Y;

	m_bCheckUseImageProcessPostLocalAlign = AlgorithmParam.m_bUseImageProcessPostLocalAlign;
	m_iEditImageProcessPostLocalAlignROINo = AlgorithmParam.m_iImageProcessPostLocalAlignROINo;
	m_cbImageProcessPostLocalAlignImageIndex.SetCurSel(AlgorithmParam.m_iImageProcessPostLocalAlignImageIndex);

	UpdateData(FALSE);
}

void CAlgorithmImageProcessDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_iProcessChType = m_iRadioProcessChType;
	pAlgorithmParam->m_iProcessResType = m_iRadioProcessResType;
	pAlgorithmParam->m_bUseImageProcess = m_bCheckUseImageProcess;
	pAlgorithmParam->m_iImageProcessArithmeticType = m_iRadioImageProcessArithmeticType;
	pAlgorithmParam->m_iImageProcessArithmeticImage1 = m_cbImageProcessArithmeticImage1.GetCurSel();
	pAlgorithmParam->m_iImageProcessArithmeticImage2 = m_cbImageProcessArithmeticImage2.GetCurSel();
	pAlgorithmParam->m_iImageProcessArithmeticImage3 = m_cbImageProcessArithmeticImage3.GetCurSel();
	pAlgorithmParam->m_iImageProcessArithmeticImage4 = m_cbImageProcessArithmeticImage4.GetCurSel();
	pAlgorithmParam->m_iImageProcessArithmeticAdd = m_iEditImageProcessArithmeticAdd;
	pAlgorithmParam->m_dImageProcessArithmeticMulti = m_dEditImageProcessArithmeticMulti;

	pAlgorithmParam->m_bUseImageProcessLocalAlign = m_bCheckUseImageProcessLocalAlign;
	pAlgorithmParam->m_iImageProcessLocalAlignImageIndex = m_cbImageProcessLocalAlignImageIndex.GetCurSel();
	pAlgorithmParam->m_iImageProcessLocalAlignROINo = m_iEditImageProcessLocalAlignROINo;
	pAlgorithmParam->m_bImageProcessLocalAlignPosX = m_bCheckImageProcessLocalAlignPosX;
	pAlgorithmParam->m_bImageProcessLocalAlignPosY = m_bCheckImageProcessLocalAlignPosY;
	pAlgorithmParam->m_bImageProcessLocalAlignAngle = m_bCheckImageProcessLocalAlignAngle;
	pAlgorithmParam->m_iImageProcessLocalAlignFitPos = m_iRadioImageProcessLocalAlignFitPos;
	pAlgorithmParam->m_iImageProcessPostLocalAlignFitPos = m_iRadioImageProcessPostLocalAlignFitPos;
	pAlgorithmParam->m_iImageProcessLocalAlignROIType = m_iRadioImageProcessLocalAlignROIType;
	pAlgorithmParam->m_iImageProcessLocalAlignMatchingApplyMethod = m_iRadioImageProcessLocalAlignMatchingApplyMethod;
	pAlgorithmParam->m_bUseImageProcessDontCare = m_bCheckUseImageProcessDontCare;
	pAlgorithmParam->m_iImageProcessDontCareImageIndex = m_cbImageProcessDontCareImageIndex.GetCurSel();
	pAlgorithmParam->m_iImageProcessDontCareROINo = m_iEditImageProcessDontCareROINo;
	pAlgorithmParam->m_iImageProcessDontCareMargin = m_iEditImageProcessDontCareMargin;
	pAlgorithmParam->m_iImageProcessDontCareMarginInner = m_iEditImageProcessDontCareMarginInner;
	pAlgorithmParam->m_bUseImageProcessDontCare2 = m_bCheckUseImageProcessDontCare2;
	pAlgorithmParam->m_iImageProcessDontCareImageIndex2 = m_cbImageProcessDontCareImageIndex2.GetCurSel();
	pAlgorithmParam->m_iImageProcessDontCareROINo2 = m_iEditImageProcessDontCareROINo2;
	pAlgorithmParam->m_iImageProcessDontCareMargin2 = m_iEditImageProcessDontCareMargin2;
	pAlgorithmParam->m_iImageProcessDontCareMargin2Inner = m_iEditImageProcessDontCareMargin2Inner;
	pAlgorithmParam->m_bUseImageProcessGenerate = m_bCheckUseImageProcessGenerate;
	pAlgorithmParam->m_bUseImageProcessGenerateInspectFlag = m_bCheckUseImageProcessGenerateInspectFlag;
	pAlgorithmParam->m_iImageProcessGenerateImageIndex = m_cbImageProcessGenerateImageIndex.GetCurSel();
	pAlgorithmParam->m_iImageProcessGenerateROINo = m_iEditImageProcessGenerateROINo;
	pAlgorithmParam->m_iImageProcessGenerateMargin = m_iEditImageProcessGenerateMargin;
	pAlgorithmParam->m_bUseImageProcessMask = m_bCheckUseImageProcessMask;
	pAlgorithmParam->m_iImageProcessMaskImageIndex = m_cbImageProcessMaskImageIndex.GetCurSel();
	pAlgorithmParam->m_iImageProcessMaskROINo = m_iEditImageProcessMaskROINo;
	pAlgorithmParam->m_bUseImageProcessMask2 = m_bCheckUseImageProcessMask2;
	pAlgorithmParam->m_iImageProcessMaskImageIndex2 = m_cbImageProcessMaskImageIndex2.GetCurSel();
	pAlgorithmParam->m_iImageProcessMaskROINo2 = m_iEditImageProcessMaskROINo2;
	pAlgorithmParam->m_bUseImageProcessLocalAlignAddLineFit = m_bCheckUseImageProcessLocalAlignAddLineFit;
	pAlgorithmParam->m_iImageProcessLocalAlignImageIndex_2 = m_cbImageProcessLocalAlignImageIndex_2.GetCurSel();
	pAlgorithmParam->m_iImageProcessLocalAlignROINo_2 = m_iEditImageProcessLocalAlignROINo_2;
	pAlgorithmParam->m_iImageProcessLocalAlignFitPos_2 = m_iRadioImageProcessLocalAlignFitPos_2;

	pAlgorithmParam->m_bUseImageProcessFilter = m_bCheckUseImageProcessFilter;
	pAlgorithmParam->m_iImageProcessFilterType1 = m_cbImageProcessFilterType1.GetCurSel();
	pAlgorithmParam->m_iImageProcessFilterType2 = m_cbImageProcessFilterType2.GetCurSel();
	pAlgorithmParam->m_iImageProcessFilterType3 = m_cbImageProcessFilterType3.GetCurSel();
	pAlgorithmParam->m_iImageProcessFilterType1X = m_iEditImageProcessFilterType1X;
	pAlgorithmParam->m_iImageProcessFilterType1Y = m_iEditImageProcessFilterType1Y;
	pAlgorithmParam->m_iImageProcessFilterType2X = m_iEditImageProcessFilterType2X;
	pAlgorithmParam->m_iImageProcessFilterType2Y = m_iEditImageProcessFilterType2Y;
	pAlgorithmParam->m_iImageProcessFilterType3X = m_iEditImageProcessFilterType3X;
	pAlgorithmParam->m_iImageProcessFilterType3Y = m_iEditImageProcessFilterType3Y;

	pAlgorithmParam->m_iImageProcessPostLocalAlignImageIndex = m_cbImageProcessPostLocalAlignImageIndex.GetCurSel();
	pAlgorithmParam->m_bUseImageProcessPostLocalAlign = m_bCheckUseImageProcessPostLocalAlign;
	pAlgorithmParam->m_iImageProcessPostLocalAlignROINo = m_iEditImageProcessPostLocalAlignROINo;

	if (pAlgorithmParam->m_iImageProcessArithmeticImage1 >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab || pAlgorithmParam->m_iImageProcessArithmeticImage1 < 0)
	{
		AfxMessageBox("영상 전처리 탭의 Image 1 영상 번호가 현재 비전에서 사용하는 영상 개수를 초과하였습니다. 사용 영상 개수 확인 후 재설정 하십시요.", MB_ICONWARNING | MB_SYSTEMMODAL);
	}

	if (pAlgorithmParam->m_iImageProcessArithmeticImage2 >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab || pAlgorithmParam->m_iImageProcessArithmeticImage2 < 0)
	{
		AfxMessageBox("영상 전처리 탭의 Image 2 영상 번호가 현재 비전에서 사용하는 영상 개수를 초과하였습니다. 사용 영상 개수 확인 후 재설정 하십시요.", MB_ICONWARNING | MB_SYSTEMMODAL);
	}

	if (pAlgorithmParam->m_iImageProcessArithmeticImage3 >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab || pAlgorithmParam->m_iImageProcessArithmeticImage3 < 0)
	{
		AfxMessageBox("영상 전처리 탭의 Image 3 영상 번호가 현재 비전에서 사용하는 영상 개수를 초과하였습니다. 사용 영상 개수 확인 후 재설정 하십시요.", MB_ICONWARNING | MB_SYSTEMMODAL);
	}

	if (pAlgorithmParam->m_iImageProcessArithmeticImage4 >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab || pAlgorithmParam->m_iImageProcessArithmeticImage4 < 0)
	{
		AfxMessageBox("영상 전처리 탭의 Image 4 영상 번호가 현재 비전에서 사용하는 영상 개수를 초과하였습니다. 사용 영상 개수 확인 후 재설정 하십시요.", MB_ICONWARNING | MB_SYSTEMMODAL);
	}
}

BOOL CAlgorithmImageProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_bnDisplayProcessImage.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	CString sTemp;
	int i;

	m_cbImageProcessArithmeticImage1.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessArithmeticImage1.AddString(sTemp);
	}

	m_cbImageProcessArithmeticImage2.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessArithmeticImage2.AddString(sTemp);
	}

	m_cbImageProcessArithmeticImage3.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessArithmeticImage3.AddString(sTemp);
	}

	m_cbImageProcessArithmeticImage4.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessArithmeticImage4.AddString(sTemp);
	}

	m_cbImageProcessLocalAlignImageIndex.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessLocalAlignImageIndex.AddString(sTemp);
	}

	m_cbImageProcessLocalAlignImageIndex_2.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessLocalAlignImageIndex_2.AddString(sTemp);
	}

	m_cbImageProcessPostLocalAlignImageIndex.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessPostLocalAlignImageIndex.AddString(sTemp);
	}

	m_cbImageProcessDontCareImageIndex.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessDontCareImageIndex.AddString(sTemp);
	}

	m_cbImageProcessDontCareImageIndex2.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessDontCareImageIndex2.AddString(sTemp);
	}

	m_cbImageProcessGenerateImageIndex.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessGenerateImageIndex.AddString(sTemp);
	}

	m_cbImageProcessMaskImageIndex.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessMaskImageIndex.AddString(sTemp);
	}

	m_cbImageProcessMaskImageIndex2.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbImageProcessMaskImageIndex2.AddString(sTemp);
	}

	m_cbImageProcessArithmeticImage1.SetCurSel(0);
	m_cbImageProcessArithmeticImage2.SetCurSel(0);
	m_cbImageProcessArithmeticImage3.SetCurSel(0);
	m_cbImageProcessArithmeticImage4.SetCurSel(0);

	m_cbImageProcessLocalAlignImageIndex.SetCurSel(0);
	m_cbImageProcessLocalAlignImageIndex_2.SetCurSel(0);
	m_cbImageProcessPostLocalAlignImageIndex.SetCurSel(0);

	m_cbImageProcessDontCareImageIndex.SetCurSel(0);
	m_cbImageProcessDontCareImageIndex2.SetCurSel(0);
	m_cbImageProcessGenerateImageIndex.SetCurSel(0);
	m_cbImageProcessMaskImageIndex.SetCurSel(0);
	m_cbImageProcessMaskImageIndex2.SetCurSel(0);

	SetDlgStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmImageProcessDlg::SetDlgStatus()
{
#if defined(SINGLE_LENS) || defined(ASSY_LENS)
	m_iRadioProcessChType = CHANNEL_TYPE_I;

	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_COLOR)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_R)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_G)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_B)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_H)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_S)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PROCESS_IMAGE_I)->EnableWindow(FALSE);
#endif
}

void CAlgorithmImageProcessDlg::OnBnClickedButtonDisplayProcessImage()
{
	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];

	UpdateData(TRUE);

	AlgorithmParam.m_bUseImageProcess = m_bCheckUseImageProcess;
	AlgorithmParam.m_iProcessChType = m_iRadioProcessChType;
	AlgorithmParam.m_iProcessResType = m_iRadioProcessResType;
	AlgorithmParam.m_iImageProcessArithmeticType = m_iRadioImageProcessArithmeticType;
	AlgorithmParam.m_iImageProcessArithmeticImage1 = m_cbImageProcessArithmeticImage1.GetCurSel();
	AlgorithmParam.m_iImageProcessArithmeticImage2 = m_cbImageProcessArithmeticImage2.GetCurSel();
	AlgorithmParam.m_iImageProcessArithmeticImage3 = m_cbImageProcessArithmeticImage3.GetCurSel();
	AlgorithmParam.m_iImageProcessArithmeticImage4 = m_cbImageProcessArithmeticImage4.GetCurSel();
	AlgorithmParam.m_iImageProcessArithmeticAdd = m_iEditImageProcessArithmeticAdd;
	AlgorithmParam.m_dImageProcessArithmeticMulti = m_dEditImageProcessArithmeticMulti;
	AlgorithmParam.m_bUseImageProcessFilter = m_bCheckUseImageProcessFilter;
	AlgorithmParam.m_iImageProcessFilterType1 = m_cbImageProcessFilterType1.GetCurSel();
	AlgorithmParam.m_iImageProcessFilterType2 = m_cbImageProcessFilterType2.GetCurSel();
	AlgorithmParam.m_iImageProcessFilterType3 = m_cbImageProcessFilterType3.GetCurSel();
	AlgorithmParam.m_iImageProcessFilterType1X = m_iEditImageProcessFilterType1X;
	AlgorithmParam.m_iImageProcessFilterType2X = m_iEditImageProcessFilterType2X;
	AlgorithmParam.m_iImageProcessFilterType3X = m_iEditImageProcessFilterType3X;
	AlgorithmParam.m_iImageProcessFilterType1Y = m_iEditImageProcessFilterType1Y;
	AlgorithmParam.m_iImageProcessFilterType2Y = m_iEditImageProcessFilterType2Y;
	AlgorithmParam.m_iImageProcessFilterType3Y = m_iEditImageProcessFilterType3Y;

	BOOL bRet;
	HObject HProcessingImage;

	HObject HROIHRegion;
	HROIHRegion = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

	for (int i = 0; i < CHANNEL_NUM; i++)
	{
		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			if (THEAPP.m_pGFunction->ValidHImage(m_HImageProcessImage[i][j]))
				CopyImage(m_HImageProcessImage[i][j], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][j]));
			else
				GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][j]));
		}
	}

	bRet = THEAPP.m_pAlgorithm->GetPreprocessImage(FALSE, m_HImageProcessImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], m_HImageProcessImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], HROIHRegion, AlgorithmParam, &HProcessingImage);

	if (bRet)
	{
		if (THEAPP.m_pGFunction->ValidHImage(HProcessingImage))
		{
			int iCurChIndex = THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType();

			CopyImage(HProcessingImage, &THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iCurChIndex][m_iSelectedImageType]);
		}
	}

	THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();
}


void CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageR()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_R);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageG()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_G);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageB()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_B);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageH()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_H);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageS()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_S);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}


void CAlgorithmImageProcessDlg::OnBnClickedRadioProcessImageI()
{
	THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(CHANNEL_TYPE_I);
	THEAPP.m_pInspectAdminViewDlg->UpdateView(FALSE);
}
