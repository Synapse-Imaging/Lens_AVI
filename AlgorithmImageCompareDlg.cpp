// AlgorithmImageCompareDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmImageCompareDlg.h"
#include "afxdialogex.h"

#include "Variation\VariationNormalizer.h"

// CAlgorithmImageCompareDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmImageCompareDlg, CDialog)

CAlgorithmImageCompareDlg::CAlgorithmImageCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmImageCompareDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseImageCompare = FALSE;
	m_bCheckImageCompareSaveFeatureLog = FALSE;
	m_bCheckUseImageCompareBright = FALSE;
	m_bCheckUseImageCompareDark = FALSE;
	m_iEditImageCompareBrightAbs = 0;
	m_iEditImageCompareDarkAbs = 0;
	m_dEditImageCompareBrightVar = 0.0;
	m_dEditImageCompareDarkVar = 0.0;
	m_bCheckUseAutoTrain = FALSE;

	m_bCheckUseWindow = FALSE;
	m_iEditWindowSize = 10;
	m_bCheckUseImageCompareEdgeImage = FALSE;

	m_bCheckUseImageCompareNormalizer = FALSE;
	m_iEditImageCompareNormalizeWeight = 20;

	m_bCheckUseImageCompareMod = FALSE;
	m_bCheckUseImageCompareModBright = TRUE;
	m_bCheckUseImageCompareModDark = TRUE;
	m_iEditImageCompareModBrightAbs = 50;
	m_iEditImageCompareModDarkAbs = 50;
	m_dEditImageCompareModBrightVar = 4;
	m_dEditImageCompareModDarkVar = 4;
	m_iEditImageCompareModTop = 0;
	m_iEditImageCompareModBottom = 0;
	m_iEditImageCompareModLeft = 0;
	m_iEditImageCompareModRight = 0;

	GenEmptyObj(&m_HMeanImage);
	GenEmptyObj(&m_HVarImage);
	GenEmptyObj(&m_HLowerThresImage);
	GenEmptyObj(&m_HUpperThresImage);

	miToolState = NC_TS_SELECT;

	m_iScanImageWidth = 100;
	m_iScanImageHeight = 100;

	mpSelectPartTRegion = NULL;

	m_bPolygonMode = FALSE;
	m_iPolygonCnt = 0;
	mpActiveTRegion = NULL;

	m_bDisplayInspectImage = FALSE;
	GenEmptyObj(&m_HInspectImage);
	GenEmptyObj(&m_HInspectDomainImage);
}

CAlgorithmImageCompareDlg::~CAlgorithmImageCompareDlg()
{
}

void CAlgorithmImageCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_COMPARE, m_bCheckUseImageCompare);

	DDX_Check(pDX, IDC_CHECK_USE_WINDOW, m_bCheckUseWindow);
	DDX_Text(pDX, IDC_EDIT_WINDOW_SIZE, m_iEditWindowSize);
	DDX_Check(pDX, IDC_CHECK_USE_EDGE_IMAGE, m_bCheckUseImageCompareEdgeImage);

	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_NORMALIZER, m_bCheckUseImageCompareNormalizer);
	DDX_Text(pDX, IDC_EDIT_IMAGE_NORMALIZE_WEIGHT, m_iEditImageCompareNormalizeWeight);

	DDX_Check(pDX, IDC_CHECK_IMAGE_COMPARE_SAVE_FEATURE_LOG, m_bCheckImageCompareSaveFeatureLog);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_COMPARE_BRIGHT, m_bCheckUseImageCompareBright);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_COMPARE_DARK, m_bCheckUseImageCompareDark);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_BRIGHT_ABS, m_iEditImageCompareBrightAbs);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_DARK_ABS, m_iEditImageCompareDarkAbs);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_BRIGHT_VAR, m_dEditImageCompareBrightVar);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_DARK_VAR, m_dEditImageCompareDarkVar);
	DDX_Check(pDX, IDC_CHECK_USE_AUTO_TRAIN, m_bCheckUseAutoTrain);

	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_COMPARE_MOD, m_bCheckUseImageCompareMod);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_COMPARE_MOD_BRIGHT, m_bCheckUseImageCompareModBright);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_COMPARE_MOD_DARK, m_bCheckUseImageCompareModDark);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_MOD_BRIGHT_ABS, m_iEditImageCompareModBrightAbs);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_MOD_DARK_ABS, m_iEditImageCompareModDarkAbs);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_MOD_BRIGHT_VAR, m_dEditImageCompareModBrightVar);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_MOD_DARK_VAR, m_dEditImageCompareModDarkVar);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_MOD_TOP, m_iEditImageCompareModTop);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_MOD_BOTTOM, m_iEditImageCompareModBottom);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_MOD_LEFT, m_iEditImageCompareModLeft);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COMPARE_MOD_RIGHT, m_iEditImageCompareModRight);

	DDX_Control(pDX, IDC_STATIC_MEAN_IMAGE, m_HWinPanelMean);
	DDX_Control(pDX, IDC_STATIC_STDEV_IMAGE, m_HWinPanelVar);
	DDX_Control(pDX, IDC_STATIC_LOWER_THRES_IMAGE, m_HWinPanelLowerThres);
	DDX_Control(pDX, IDC_STATIC_UPPER_THRES_IMAGE, m_HWinPanelUpperThres);
}

BEGIN_MESSAGE_MAP(CAlgorithmImageCompareDlg, CDialog)
	ON_BN_CLICKED(IDC_MFCBUTTON_NEXT_PROCESSING, &CAlgorithmImageCompareDlg::OnBnClickedMfcbuttonNextProcessing)
	ON_BN_CLICKED(IDC_BUTTON_TRAIN_CURRENT_IMAGE, &CAlgorithmImageCompareDlg::OnBnClickedButtonTrainCurrentImage)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_MODEL_IMAGE, &CAlgorithmImageCompareDlg::OnBnClickedButtonShowModelImage)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_MODEL, &CAlgorithmImageCompareDlg::OnBnClickedButtonDeleteModel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_MODEL, &CAlgorithmImageCompareDlg::OnBnClickedButtonSaveModel)
	ON_COMMAND(ID_HIMAGEVIEW_FULL, OnNcWidthfit)
	ON_COMMAND(ID_HIMAGEVIEW_100, OnNc100)
	ON_COMMAND(ID_HIMAGEVIEW_SELECT_PART, OnNcSelectPart)
	ON_COMMAND(ID_HIMAGEVIEW_CHANGE_IMAGE, OnNcChangeImage)
	ON_COMMAND(ID_HIMAGEVIEW_DEFECT_MASKING, OnNcDefectMasking)
	ON_COMMAND(ID_HIMAGEVIEW_DELETE_DEFECT_MASKING, OnNcDeleteDefectMasking)
	ON_COMMAND(ID_HIMAGEVIEW_PARAM_OPT, OnNcParamOpt)
	ON_COMMAND(ID_HIMAGEVIEW_PARAM_SET, OnNcParamSet)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SHOW_NORMALIZE_IMAGE, &CAlgorithmImageCompareDlg::OnBnClickedButtonShowNormalizeImage)
END_MESSAGE_MAP()


// CAlgorithmImageCompareDlg 메시지 처리기입니다.

void CAlgorithmImageCompareDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;

	GenEmptyObj(&m_HMeanImage);
	GenEmptyObj(&m_HVarImage);
	GenEmptyObj(&m_HLowerThresImage);
	GenEmptyObj(&m_HUpperThresImage);

	GenEmptyObj(&m_HInspectImage);

	SAFE_DELETE(mpActiveTRegion);

	m_bDisplayInspectImage = FALSE;
}

void CAlgorithmImageCompareDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseImageCompare = AlgorithmParam.m_bUseImageCompare;
	m_bCheckImageCompareSaveFeatureLog = AlgorithmParam.m_bImageCompareSaveFeatureLog;
	m_bCheckUseWindow = AlgorithmParam.m_bUseImageCompareWindow;
	m_iEditWindowSize = AlgorithmParam.m_iImageCompareWindowSize;
	m_bCheckUseImageCompareEdgeImage = AlgorithmParam.m_bUseImageCompareEdgeImage;

	m_bCheckUseImageCompareNormalizer = AlgorithmParam.m_bUseImageCompareNormalizer;
	m_iEditImageCompareNormalizeWeight = AlgorithmParam.m_iImageCompareNormalizeWeight;

	m_bCheckUseImageCompareBright = AlgorithmParam.m_bUseImageCompareBright;
	m_iEditImageCompareBrightAbs = AlgorithmParam.m_iImageCompareBrightAbs;
	m_dEditImageCompareBrightVar = AlgorithmParam.m_dImageCompareBrightVar;
	m_bCheckUseImageCompareDark = AlgorithmParam.m_bUseImageCompareDark;
	m_iEditImageCompareDarkAbs = AlgorithmParam.m_iImageCompareDarkAbs;
	m_dEditImageCompareDarkVar = AlgorithmParam.m_dImageCompareDarkVar;
	m_bCheckUseAutoTrain = AlgorithmParam.m_bUseImageCompareAutoTrain;

	m_bCheckUseImageCompareMod = AlgorithmParam.m_bUseImageCompareMod;
	m_bCheckUseImageCompareModBright = AlgorithmParam.m_bUseImageCompareModBright;
	m_bCheckUseImageCompareModDark = AlgorithmParam.m_bUseImageCompareModDark;
	m_iEditImageCompareModBrightAbs = AlgorithmParam.m_iImageCompareModBrightAbs;
	m_iEditImageCompareModDarkAbs = AlgorithmParam.m_iImageCompareModDarkAbs;
	m_dEditImageCompareModBrightVar = AlgorithmParam.m_dImageCompareModBrightVar;
	m_dEditImageCompareModDarkVar = AlgorithmParam.m_dImageCompareModDarkVar;
	m_iEditImageCompareModTop = AlgorithmParam.m_iImageCompareModTop;
	m_iEditImageCompareModBottom = AlgorithmParam.m_iImageCompareModBottom;
	m_iEditImageCompareModLeft = AlgorithmParam.m_iImageCompareModLeft;
	m_iEditImageCompareModRight = AlgorithmParam.m_iImageCompareModRight;

	UpdateData(FALSE);
}

void CAlgorithmImageCompareDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseImageCompare = m_bCheckUseImageCompare;
	pAlgorithmParam->m_bImageCompareSaveFeatureLog = m_bCheckImageCompareSaveFeatureLog;
	pAlgorithmParam->m_bUseImageCompareWindow = m_bCheckUseWindow;
	pAlgorithmParam->m_iImageCompareWindowSize = m_iEditWindowSize;
	pAlgorithmParam->m_bUseImageCompareEdgeImage = m_bCheckUseImageCompareEdgeImage;

	pAlgorithmParam->m_bUseImageCompareNormalizer = m_bCheckUseImageCompareNormalizer;
	pAlgorithmParam->m_iImageCompareNormalizeWeight = m_iEditImageCompareNormalizeWeight;

	pAlgorithmParam->m_bUseImageCompareBright = m_bCheckUseImageCompareBright;
	pAlgorithmParam->m_iImageCompareBrightAbs = m_iEditImageCompareBrightAbs;
	pAlgorithmParam->m_dImageCompareBrightVar = m_dEditImageCompareBrightVar;
	pAlgorithmParam->m_bUseImageCompareDark = m_bCheckUseImageCompareDark;
	pAlgorithmParam->m_iImageCompareDarkAbs = m_iEditImageCompareDarkAbs;
	pAlgorithmParam->m_dImageCompareDarkVar = m_dEditImageCompareDarkVar;
	pAlgorithmParam->m_bUseImageCompareAutoTrain = m_bCheckUseAutoTrain;

	pAlgorithmParam->m_bUseImageCompareMod = m_bCheckUseImageCompareMod;
	pAlgorithmParam->m_bUseImageCompareModBright = m_bCheckUseImageCompareModBright;
	pAlgorithmParam->m_bUseImageCompareModDark = m_bCheckUseImageCompareModDark;
	pAlgorithmParam->m_iImageCompareModBrightAbs = m_iEditImageCompareModBrightAbs;
	pAlgorithmParam->m_iImageCompareModDarkAbs = m_iEditImageCompareModDarkAbs;
	pAlgorithmParam->m_dImageCompareModBrightVar = m_dEditImageCompareModBrightVar;
	pAlgorithmParam->m_dImageCompareModDarkVar = m_dEditImageCompareModDarkVar;
	pAlgorithmParam->m_iImageCompareModTop = m_iEditImageCompareModTop;
	pAlgorithmParam->m_iImageCompareModBottom = m_iEditImageCompareModBottom;
	pAlgorithmParam->m_iImageCompareModLeft = m_iEditImageCompareModLeft;
	pAlgorithmParam->m_iImageCompareModRight = m_iEditImageCompareModRight;
}

BOOL CAlgorithmImageCompareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Menu.LoadMenu(IDR_MENU_IMAGE_COMPARE_VIEW);

	m_HWinPanelMean.CreateHWindow();
	m_HWinPanelMean.GetWindowRect(&m_ViewRect[eMEAN]);
	this->ScreenToClient(&m_ViewRect[eMEAN]);
	m_ViewOffset[eMEAN].x = m_ViewRect[eMEAN].left;
	m_ViewOffset[eMEAN].y = m_ViewRect[eMEAN].top;

	m_HWinPanelVar.CreateHWindow();
	m_HWinPanelVar.GetWindowRect(&m_ViewRect[eSTDEV]);
	this->ScreenToClient(&m_ViewRect[eSTDEV]);
	m_ViewOffset[eSTDEV].x = m_ViewRect[eSTDEV].left;
	m_ViewOffset[eSTDEV].y = m_ViewRect[eSTDEV].top;

	m_HWinPanelLowerThres.CreateHWindow();
	m_HWinPanelLowerThres.GetWindowRect(&m_ViewRect[eLTHRES]);
	this->ScreenToClient(&m_ViewRect[eLTHRES]);
	m_ViewOffset[eLTHRES].x = m_ViewRect[eLTHRES].left;
	m_ViewOffset[eLTHRES].y = m_ViewRect[eLTHRES].top;

	m_HWinPanelUpperThres.CreateHWindow();
	m_HWinPanelUpperThres.GetWindowRect(&m_ViewRect[eUTHRES]);
	this->ScreenToClient(&m_ViewRect[eUTHRES]);
	m_ViewOffset[eUTHRES].x = m_ViewRect[eUTHRES].left;
	m_ViewOffset[eUTHRES].y = m_ViewRect[eUTHRES].top;

	SetViewportManager();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmImageCompareDlg::OnBnClickedButtonTrainCurrentImage()
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		CAlgorithmParam AlgorithmParam;

		AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information

		THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->m_pAlgorithmImageProcessDlg->GetParam(&AlgorithmParam);	// Image Processing Tab Data
		THEAPP.m_pTabControlDlg->AlgorithmPreProcessing();

		UpdateData();

		HObject HROIRgn, HInspectAreaRgn;
		HROIRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		// 1. 검사 ROI 전처리
		if (AlgorithmParam.m_bUseImageProcessLocalAlign)
			THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIRgn, TEACHING_THREAD_INDEX);

		if (AlgorithmParam.m_bUseImageProcessGenerate)
			THEAPP.m_pAlgorithm->ApplyGenerateResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIRgn, TEACHING_THREAD_INDEX);

		if (AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
			THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIRgn, TEACHING_THREAD_INDEX);

		THEAPP.m_pAlgorithm->GetInspectArea(HROIRgn, AlgorithmParam, &HInspectAreaRgn);

		HTuple HlOriginLTPointY, HlOriginLTPointX, HlOriginRBPointY, HlOriginRBPointX;
		SmallestRectangle1(HInspectAreaRgn, &HlOriginLTPointY, &HlOriginLTPointX, &HlOriginRBPointY, &HlOriginRBPointX);

		HTuple HomMat2DRotate, HomMat2DRotate2;
		HObject HCropImage;
		GTRegion* pInspectROIRgn;
		double dDeltaAngle = 0;
		BOOL bLocalAlignROIFindCheck = FALSE;
		if (AlgorithmParam.m_bUseImageProcessLocalAlign &&
			AlgorithmParam.m_iImageProcessLocalAlignROIType == LOCAL_ALIGN_MATCHING_ROI_TYPE_MATCHING)
		{
			int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
			int iLocalAlignImageNo = AlgorithmParam.m_iImageProcessLocalAlignImageIndex + 1;

			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				if (pInspectROIRgn->miTeachImageIndex == iLocalAlignImageNo)
				{
					if (pInspectROIRgn->miLocalAlignID == AlgorithmParam.m_iImageProcessLocalAlignROINo)
					{
						bLocalAlignROIFindCheck = TRUE;
						break;
					}
				}
			}

			// 2. 연산량 감소를 위해 전체 영상에서 검사 ROI 영역 크롭
			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HInspectAreaRgn, &HCropImage);
			CropDomain(HCropImage, &HCropImage);
		}
		else
		{
			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HInspectAreaRgn, &HCropImage);
			CropDomain(HCropImage, &HCropImage);
		}

		if (bLocalAlignROIFindCheck)
			dDeltaAngle = -pInspectROIRgn->m_dLocalAlignDeltaAngle[TEACHING_THREAD_INDEX];

		// 3. 이동 및 회전 변환 행렬 계산
		int iTempY = 0;
		int iTempX = 0;
		if (dDeltaAngle != 0)
		{
			HTuple HArea, HCenterX, HCenterY;
			AreaCenter(HCropImage, &HArea, &HCenterY, &HCenterX);

			HomMat2dIdentity(&HomMat2DRotate);
			HomMat2dRotate(HomMat2DRotate, dDeltaAngle, HCenterY, HCenterX, &HomMat2DRotate);
			AffineTransImage(HCropImage, &HCropImage, HomMat2DRotate, "bicubic", "false");

			HomMat2dIdentity(&HomMat2DRotate2);
			HomMat2dRotate(HomMat2DRotate2, -dDeltaAngle, HCenterY, HCenterX, &HomMat2DRotate2);

			// 4. Local align을 통해 회전된 ROI의 크기 보정
			int CropLTPointY, CropLTPointX, CropRBPointY, CropRBPointX;
			HObject HResizeInspectAreaRgn;
			CropLTPointY = (HCenterY - (HlOriginRBPointY - HlOriginLTPointY) / 2).D();
			CropRBPointY = (HCenterY + (HlOriginRBPointY - HlOriginLTPointY) / 2).D();
			CropLTPointX = (HCenterX - (HlOriginRBPointX - HlOriginLTPointX) / 2).D();
			CropRBPointX = (HCenterX + (HlOriginRBPointX - HlOriginLTPointX) / 2).D();
			if (CropLTPointY != 0)
			{
				CropRBPointY -= CropLTPointY;
				CropLTPointY = 0;
			}
			if (CropLTPointX != 0)
			{
				CropRBPointX -= CropLTPointX;
				CropLTPointX = 0;
			}

			if (m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex] >= 0 &&
				m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex] >= 0 &&
				(CropRBPointY != m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex] ||
				CropRBPointX != m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex]))
			{
				iTempY = (CropRBPointY - m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex]) / 2;
				CropLTPointY += iTempY;
				CropRBPointY -= iTempY;

				iTempX = (CropRBPointX - m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex]) / 2;
				CropLTPointX += iTempX;
				CropRBPointX -= iTempX;
			}

			GenRectangle1(&HResizeInspectAreaRgn, CropLTPointY, CropLTPointX, CropRBPointY, CropRBPointX);
			ReduceDomain(HCropImage, HResizeInspectAreaRgn, &HCropImage);
			CropDomain(HCropImage, &HCropImage);
		}

		if (bDebugSave)
			WriteImage(HCropImage, "bmp", 0, "c:\\DualTest\\HCropImage");

		if (m_bCheckUseImageCompareEdgeImage)
			SobelAmp(HCropImage, &HCropImage, "sum_abs", 3);

		// 5. 크롭된 영역 학습
		HTuple HImageWidth, HImageHeight;
		GetImageSize(HCropImage, &HImageWidth, &HImageHeight);

		if (m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex] < 0)
		{
			if (m_bCheckUseWindow)
			{
				CreateVariationModel(HImageWidth, HImageHeight, "byte", "direct", &(m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]));
				GenEmptyObj(&(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex]));
				GenEmptyObj(&(m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex]));
				m_pSelectedROI->m_iVarNoTrainSample[m_iSelectedInspectionTabIndex] = 0;
			}
			else
				CreateVariationModel(HImageWidth, HImageHeight, "byte", "standard", &(m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]));

			m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex] = HImageWidth[0].L();
			m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex] = HImageHeight[0].L();
		}
		else
		{
			HObject HLowerThresImage, HUpperThresImage;
			HTuple HLowerThresImageWidth, HLowerThresImageHeight;
			GetThreshImagesVariationModel(&HLowerThresImage, &HUpperThresImage, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]);
			GetImageSize(HLowerThresImage, &HLowerThresImageWidth, &HLowerThresImageHeight);
			m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex] = HLowerThresImageWidth[0].L();
			m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex] = HLowerThresImageHeight[0].L();

			if ((m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex] != HImageWidth[0].L()) || (m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex] != HImageHeight[0].L()))
			{
				if (m_pSelectedROI->m_iVarModelImageSizeX > 0 && m_pSelectedROI->m_iVarModelImageSizeY > 0)
					ZoomImageSize(HCropImage, &HCropImage, m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex], m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex], "nearest_neighbor");
			}
		}

		if (bDebugSave)
			WriteImage(HCropImage, "bmp", 0, "c:\\DualTest\\HCropImage2");

		if (m_bCheckUseWindow)
			THEAPP.m_pAlgorithm->TrainVariationModelWindow(HCropImage, m_pSelectedROI, m_iSelectedInspectionTabIndex, AlgorithmParam.m_iImageCompareWindowSize);
		else
			TrainVariationModel(HCropImage, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]);

		HTuple HAbsThreshold, HVarThreshold;
		TupleGenConst(0, 0, &HAbsThreshold);
		TupleGenConst(0, 0, &HVarThreshold);

		TupleConcat(HAbsThreshold, HTuple(m_iEditImageCompareBrightAbs), &HAbsThreshold);
		TupleConcat(HAbsThreshold, HTuple(m_iEditImageCompareDarkAbs), &HAbsThreshold);

		TupleConcat(HVarThreshold, HTuple(m_dEditImageCompareBrightVar), &HVarThreshold);
		TupleConcat(HVarThreshold, HTuple(m_dEditImageCompareDarkVar), &HVarThreshold);

		if (m_bCheckUseWindow)
			PrepareDirectVariationModel(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex], m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex], m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], HAbsThreshold, HVarThreshold);
		else
			PrepareVariationModel(m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], HAbsThreshold, HVarThreshold);

		if (m_bCheckUseWindow)
		{
			CopyImage(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex], &m_HMeanImage);
			CopyImage(m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex], &m_HVarImage);
		}
		else
		{
			GetVariationModel(&m_HMeanImage, &m_HVarImage, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]);
			ConvertImageType(m_HVarImage, &m_HVarImage, "byte");
		}

		GetThreshImagesVariationModel(&m_HLowerThresImage, &m_HUpperThresImage, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]);

		THEAPP.bVariationModelSaveCheck[THEAPP.m_iCurTeachVision] = TRUE;

		UpdateVarModelImage();
		InvalidateRect(NULL, TRUE);
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

		strLog.Format("Halcon Exception [AlgorithmImageCompareDlg::OnBnClickedButtonTrainCurrentImage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmImageCompareDlg::OnBnClickedButtonShowModelImage()
{
	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		if (m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex] < 0)
		{
			AfxMessageBox("영상비교 모델이 존재하지 않습니다. 먼저, 현재 영상 학습으로 모델을 생성해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		if (m_bCheckUseWindow)
		{
			if (THEAPP.m_pGFunction->ValidHImage(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex]))
				CopyImage(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex], &m_HMeanImage);
			else
				return;

			if (THEAPP.m_pGFunction->ValidHImage(m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex]))
				CopyImage(m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex], &m_HVarImage);
			else
				return;
		}
		else
		{
			GetVariationModel(&m_HMeanImage, &m_HVarImage, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]);
			ConvertImageType(m_HVarImage, &m_HVarImage, "byte");
		}

		HTuple HAbsThreshold, HVarThreshold;
		TupleGenConst(0, 0, &HAbsThreshold);
		TupleGenConst(0, 0, &HVarThreshold);

		TupleConcat(HAbsThreshold, HTuple(m_iEditImageCompareBrightAbs), &HAbsThreshold);
		TupleConcat(HAbsThreshold, HTuple(m_iEditImageCompareDarkAbs), &HAbsThreshold);

		TupleConcat(HVarThreshold, HTuple(m_dEditImageCompareBrightVar), &HVarThreshold);
		TupleConcat(HVarThreshold, HTuple(m_dEditImageCompareDarkVar), &HVarThreshold);

		if (m_bCheckUseWindow)
			PrepareDirectVariationModel(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex], m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex], m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], HAbsThreshold, HVarThreshold);
		else
			PrepareVariationModel(m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], HAbsThreshold, HVarThreshold);

		GetThreshImagesVariationModel(&m_HLowerThresImage, &m_HUpperThresImage, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]);

		UpdateVarModelImage();
		InvalidateRect(NULL, TRUE);
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

		strLog.Format("Halcon Exception [AlgorithmImageCompareDlg::OnBnClickedButtonShowModelImage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmImageCompareDlg::OnBnClickedButtonDeleteModel()
{
	if (AfxMessageBox("현재 영상비교 모델을 삭제하시겠습니까?", MB_OKCANCEL) == IDOK)
	{
		if (m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex] >= 0)
		{
			ClearVariationModel(m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]);
			m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex] = -1;

			m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex] = -1;
			m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex] = -1;

			GenEmptyObj(&(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex]));
			GenEmptyObj(&(m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex]));
			m_pSelectedROI->m_iVarNoTrainSample[m_iSelectedInspectionTabIndex] = 0;
		}

		GenEmptyObj(&m_HMeanImage);
		GenEmptyObj(&m_HVarImage);
		GenEmptyObj(&m_HLowerThresImage);
		GenEmptyObj(&m_HUpperThresImage);

		InvalidateRect(NULL, TRUE);
	}
}


void CAlgorithmImageCompareDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

	HTuple HImageWidth, HImageHeight;

	POINT WHPoint;

	if (THEAPP.m_pGFunction->ValidHImage(m_HMeanImage))
	{
		CDC* pDC = GetDC();

		POINT ClientOffset;
		ClientOffset.x = m_ViewOffset[eMEAN].x;
		ClientOffset.y = m_ViewOffset[eMEAN].y;
		pDC->SetViewportOrg(ClientOffset);

		dc.OffsetViewportOrg(m_ViewOffset[eMEAN].x, m_ViewOffset[eMEAN].y);

		HTuple lDC = (INT)(pDC->m_hDC);
		SetWindowDc(m_HWinPanelMean.mlWindowHandle, lDC);

		CRgn ClipRgn;
		ClipRgn.CreateRectRgn(m_ViewOffset[eMEAN].x, m_ViewOffset[eMEAN].y, m_ViewOffset[eMEAN].x + mViewportManager.miViewWidth, m_ViewOffset[eMEAN].y + mViewportManager.miViewHeight);
		pDC->SelectClipRgn(&ClipRgn);

		mViewportManager.GetImageWHPoint(&WHPoint);
		SetPart(m_HWinPanelMean.mlWindowHandle, mViewportManager.miStartYPos, mViewportManager.miStartXPos, mViewportManager.miStartYPos + WHPoint.y, mViewportManager.miStartXPos + WHPoint.x);
		SetWindowExtents(m_HWinPanelMean.mlWindowHandle, 0, 0, mViewportManager.miViewWidth, mViewportManager.miViewHeight);

		if (m_bDisplayInspectImage)
			DispObj(m_HInspectDomainImage, m_HWinPanelMean.mlWindowHandle);
		else
			DispObj(m_HMeanImage, m_HWinPanelMean.mlWindowHandle);

		if (m_bPolygonMode)
		{
			for (int iii = 0; iii < m_iPolygonCnt; iii++)
			{
				THEAPP.m_pGFunction->DisplayCross(m_HWinPanelMean.mlWindowHandle, m_PolygonPt[iii].x, m_PolygonPt[iii].y, RGB(0, 255, 0), 10);
			}
		}

		if (mpActiveTRegion)
		{
			if (mpActiveTRegion->GetVisible())
			{
				if (mpActiveTRegion->m_bRegionROI)
				{
					SetDraw(m_HWinPanelMean.mlWindowHandle, "margin");
					SetColor(m_HWinPanelMean.mlWindowHandle, "green");

					if (THEAPP.m_pGFunction->ValidHRegion(mpActiveTRegion->m_HTeachPolygonRgn))
					{
						DispObj(mpActiveTRegion->m_HTeachPolygonRgn, m_HWinPanelMean.mlWindowHandle);
					}
				}
			}
		}

		SetWindowDc(m_HWinPanelMean.mlWindowHandle, 0);
		ReleaseDC(pDC);
	}
	else
		ClearWindow(m_HWinPanelMean.mlWindowHandle);

	if (THEAPP.m_pGFunction->ValidHImage(m_HVarImage))
	{
		mViewportManager.GetImageWHPoint(&WHPoint);
		SetPart(m_HWinPanelVar.mlWindowHandle, mViewportManager.miStartYPos, mViewportManager.miStartXPos, mViewportManager.miStartYPos + WHPoint.y, mViewportManager.miStartXPos + WHPoint.x);
		SetWindowExtents(m_HWinPanelVar.mlWindowHandle, 0, 0, mViewportManager.miViewWidth, mViewportManager.miViewHeight);
		DispObj(m_HVarImage, m_HWinPanelVar.mlWindowHandle);
	}
	else
		ClearWindow(m_HWinPanelVar.mlWindowHandle);

	if (THEAPP.m_pGFunction->ValidHImage(m_HLowerThresImage))
	{
		mViewportManager.GetImageWHPoint(&WHPoint);
		SetPart(m_HWinPanelLowerThres.mlWindowHandle, mViewportManager.miStartYPos, mViewportManager.miStartXPos, mViewportManager.miStartYPos + WHPoint.y, mViewportManager.miStartXPos + WHPoint.x);
		SetWindowExtents(m_HWinPanelLowerThres.mlWindowHandle, 0, 0, mViewportManager.miViewWidth, mViewportManager.miViewHeight);
		DispObj(m_HLowerThresImage, m_HWinPanelLowerThres.mlWindowHandle);
	}
	else
		ClearWindow(m_HWinPanelLowerThres.mlWindowHandle);

	if (THEAPP.m_pGFunction->ValidHImage(m_HUpperThresImage))
	{
		mViewportManager.GetImageWHPoint(&WHPoint);
		SetPart(m_HWinPanelUpperThres.mlWindowHandle, mViewportManager.miStartYPos, mViewportManager.miStartXPos, mViewportManager.miStartYPos + WHPoint.y, mViewportManager.miStartXPos + WHPoint.x);
		SetWindowExtents(m_HWinPanelUpperThres.mlWindowHandle, 0, 0, mViewportManager.miViewWidth, mViewportManager.miViewHeight);
		DispObj(m_HUpperThresImage, m_HWinPanelUpperThres.mlWindowHandle);
	}
	else
		ClearWindow(m_HWinPanelUpperThres.mlWindowHandle);
}

void CAlgorithmImageCompareDlg::UpdateVarModelImage()
{
	try
	{
		if (m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex] < 0)
			return;

		UpdateData();

		if (m_bCheckUseWindow)
		{
			if (THEAPP.m_pGFunction->ValidHImage(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex]))
				CopyImage(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex], &m_HMeanImage);
			else
				return;

			if (THEAPP.m_pGFunction->ValidHImage(m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex]))
				CopyImage(m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex], &m_HVarImage);
			else
				return;
		}
		else
		{
			GetVariationModel(&m_HMeanImage, &m_HVarImage, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]);
			ConvertImageType(m_HVarImage, &m_HVarImage, "byte");
		}

		HTuple HAbsThreshold, HVarThreshold;
		TupleGenConst(0, 0, &HAbsThreshold);
		TupleGenConst(0, 0, &HVarThreshold);

		TupleConcat(HAbsThreshold, HTuple(m_iEditImageCompareBrightAbs), &HAbsThreshold);
		TupleConcat(HAbsThreshold, HTuple(m_iEditImageCompareDarkAbs), &HAbsThreshold);

		TupleConcat(HVarThreshold, HTuple(m_dEditImageCompareBrightVar), &HVarThreshold);
		TupleConcat(HVarThreshold, HTuple(m_dEditImageCompareDarkVar), &HVarThreshold);

		if (m_bCheckUseWindow)
			PrepareDirectVariationModel(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex], m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex], m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], HAbsThreshold, HVarThreshold);
		else
			PrepareVariationModel(m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], HAbsThreshold, HVarThreshold);

		GetThreshImagesVariationModel(&m_HLowerThresImage, &m_HUpperThresImage, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]);

		HTuple HImageSizeX, HImageSizeY;
		GetImageSize(m_HMeanImage, &HImageSizeX, &HImageSizeY);
		m_iScanImageWidth = HImageSizeX.L();
		m_iScanImageHeight = HImageSizeY.L();

		SetViewportManager();
		mViewportManager.mdZoomRatio = m_dFitZoomRatio;
		UpdateViewportManager();

		ClearViewWindow();

		InvalidateRect(NULL, TRUE);
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

		strLog.Format("Halcon Exception [AlgorithmImageCompareDlg::UpdateVarModelImage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmImageCompareDlg::OnBnClickedMfcbuttonNextProcessing()
{
	THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->UpdateAlgorithmTab(10);
}


void CAlgorithmImageCompareDlg::OnBnClickedButtonSaveModel()
{
	try
	{
		if (m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex] < 0)
		{
			AfxMessageBox("Variation 모델이 생성되지 않아 저장할 수 없습니다.", MB_SYSTEMMODAL | MB_ICONINFORMATION);
			return;
		}

		CString strFileName;
		CString csTmp, csData;

		char szFilter[] = "All Files (*.*)|*.*|Var Model Files (*.var)|*.var|";

		CFileDialog FileDialog(FALSE, NULL, NULL, OFN_HIDEREADONLY, szFilter, NULL);
		FileDialog.m_ofn.lpstrTitle = _T("Save variation model as file");
		FileDialog.m_ofn.lpstrInitialDir = _T("");

		if (FileDialog.DoModal() == IDOK)
		{
			strFileName = FileDialog.GetPathName();
			strFileName += ".var";

			WriteVariationModel(m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], (HTuple)strFileName);
		}
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [AlgorithmImageCompareDlg::OnBnClickedButtonSaveModel] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		AfxMessageBox("Variation 모델 저장 오류.", MB_ICONERROR | MB_SYSTEMMODAL);
	}
}

void CAlgorithmImageCompareDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here

	m_Menu.DestroyMenu();
	m_HWinPanelMean.DestroyHWindow();
	m_HWinPanelVar.DestroyHWindow();
	m_HWinPanelLowerThres.DestroyHWindow();
	m_HWinPanelUpperThres.DestroyHWindow();
}

void CAlgorithmImageCompareDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (!m_ViewRect[eMEAN].PtInRect(point)) {
		CDialog::OnLButtonDown(nFlags, point);
		return;
	}

	CPoint ClientViewPoint1 = point;

	ClientToScreen(&ClientViewPoint1);
	m_HWinPanelMean.ScreenToClient(&ClientViewPoint1);

	POINT IPoint = ClientViewPoint1;
	maMVPoints[0] = point;
	mViewportManager.VPtoIP(&IPoint, 1);
	maMIPoints[0] = IPoint;

	switch (miToolState) {
	case NC_TS_SELECT:
		ClearViewWindow();
		InvalidateRect(NULL, FALSE);
		break;

	case NC_TS_SELECT_PART:
		SetCapture();

		mpSelectPartTRegion = new CSelectPartRegion;
		if (mpSelectPartTRegion)
		{
			mpSelectPartTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->SetSelect(TRUE, FALSE);
			DrawSelectPartTRegion();
		}
		break;

	default:
		break;
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CAlgorithmImageCompareDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (!m_ViewRect[eMEAN].PtInRect(point)) {
		CDialog::OnMouseMove(nFlags, point);
		return;
	}

	CPoint viewPoint1 = point;

	ClientToScreen(&viewPoint1);
	m_HWinPanelMean.ScreenToClient(&viewPoint1);

	POINT IPoint = viewPoint1;
	maMVPoints[1] = point;
	mViewportManager.VPtoIP(&IPoint, 1);
	maMIPoints[1] = IPoint;

	double dX, dY;

	if (IS_SETFLAG(nFlags, MK_LBUTTON))
	{
		switch (miToolState) {

		case NC_TS_SELECT:

			dX = ((double)(maMVPoints[0].x - maMVPoints[1].x) / mViewportManager.mdZoomRatio);

			if (fabs(dX) < 1.0)
				break;

			dY = ((double)(maMVPoints[0].y - maMVPoints[1].y) / mViewportManager.mdZoomRatio);

			mViewportManager.miStartXPos += (int)(dX);
			mViewportManager.miStartYPos += (int)(dY);

			UpdateViewportManager();

			maMVPoints[0] = maMVPoints[1];
			maMIPoints[0] = maMIPoints[1];

			break;

		case NC_TS_SELECT_PART:

			if (!mpSelectPartTRegion)
				return;

			DrawSelectPartTRegion();

			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			DrawSelectPartTRegion();

			break;

		default:
			break;
		}
	}
	else
	{
		CGFunction *pGFun = THEAPP.m_pGFunction;
		CString str;
		double dGrayValue;
		POINT Pos;
		HTuple HGrayValue;

		if (IPoint.x >= 0 && IPoint.y >= 0 && IPoint.x < m_iScanImageWidth && IPoint.y < m_iScanImageHeight) 
		{
			Pos = IPoint;
			HObject HPointRgn[MAX_VM_IMAGE];

			if (m_bDisplayInspectImage)
			{
				if (pGFun->ValidHImage(m_HInspectDomainImage))
				{
					GetGrayval(m_HInspectDomainImage, Pos.y, Pos.x, &HGrayValue);
					dGrayValue = HGrayValue[0].D();
					str.Format("검사 영상 (%d)", (int)dGrayValue);
					GetDlgItem(IDC_STATIC_MEAN)->SetWindowText(str);
				}
			}
			else
			{
				if (pGFun->ValidHImage(m_HMeanImage))
				{
					GetGrayval(m_HMeanImage, Pos.y, Pos.x, &HGrayValue);
					dGrayValue = HGrayValue[0].D();
					str.Format("평균 영상 (%d)", (int)dGrayValue);
					GetDlgItem(IDC_STATIC_MEAN)->SetWindowText(str);
				}
			}

			if (pGFun->ValidHImage(m_HVarImage))
			{
				GetGrayval(m_HVarImage, Pos.y, Pos.x, &HGrayValue);
				dGrayValue = HGrayValue[0].D();
				str.Format("표준편차 영상 (%d)", (int)dGrayValue);
				GetDlgItem(IDC_STATIC_STDEV)->SetWindowText(str);
			}

			if (pGFun->ValidHImage(m_HLowerThresImage))
			{
				GetGrayval(m_HLowerThresImage, Pos.y, Pos.x, &HGrayValue);
				dGrayValue = HGrayValue[0].D();
				str.Format("Lower Threshold 영상 (%d)", (int)dGrayValue);
				GetDlgItem(IDC_STATIC_LTHRES)->SetWindowText(str);
			}

			if (pGFun->ValidHImage(m_HUpperThresImage))
			{
				GetGrayval(m_HUpperThresImage, Pos.y, Pos.x, &HGrayValue);
				dGrayValue = HGrayValue[0].D();
				str.Format("Upper Threshold 영상 (%d)", (int)dGrayValue);
				GetDlgItem(IDC_STATIC_UTHRES)->SetWindowText(str);
			}
		}
	}

	maMVPoints[0] = maMVPoints[1];
	maMIPoints[0] = maMIPoints[1];

	CDialog::OnMouseMove(nFlags, point);
}

void CAlgorithmImageCompareDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (!m_ViewRect[eMEAN].PtInRect(point)) {
		CDialog::OnLButtonUp(nFlags, point);
		return;
	}

	if (miToolState == NC_TS_SELECT_PART)
	{
		ReleaseCapture();

		POINT dLTPoint, dRBPoint;
		POINT WHPoint;

		if (mpSelectPartTRegion) {

			mpSelectPartTRegion->GetWHPoint(&WHPoint, THEAPP.m_pCalDataService);

			//길이가 10보다 작으면 무시. 
			if (WHPoint.x < 10 || WHPoint.y < 10) {
				delete mpSelectPartTRegion;
				mpSelectPartTRegion = NULL;

				ClearViewWindow();
				InvalidateRect(NULL, FALSE);
				return;
			}

			mpSelectPartTRegion->ArrangePoints();

			mpSelectPartTRegion->GetLTPoint(&dLTPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->GetRBPoint(&dRBPoint, THEAPP.m_pCalDataService);

			delete mpSelectPartTRegion;
			mpSelectPartTRegion = NULL;

			if (dLTPoint.x < 0 || dLTPoint.x >= m_iScanImageWidth || dLTPoint.y < 0 || dLTPoint.y >= m_iScanImageHeight ||
				dRBPoint.x < 0 || dRBPoint.x >= m_iScanImageWidth || dRBPoint.y < 0 || dRBPoint.y >= m_iScanImageHeight)
				return;

			int iSelectImageWidth, iSelectImageHeight;

			if (dLTPoint.x < 0) iSelectImageWidth = (int)(dRBPoint.x + 1);
			else if (dRBPoint.x >= m_iScanImageWidth) iSelectImageWidth = (int)(m_iScanImageWidth - dLTPoint.x);
			else
				iSelectImageWidth = (int)(dRBPoint.x - dLTPoint.x + 1);

			if (dLTPoint.y < 0) iSelectImageHeight = (int)(dRBPoint.y + 1);
			else if (dRBPoint.y >= m_iScanImageHeight) iSelectImageHeight = (int)(m_iScanImageHeight - dLTPoint.y);
			else
				iSelectImageHeight = (int)(dRBPoint.y - dLTPoint.y + 1);

			if (iSelectImageHeight >= iSelectImageWidth)
				mViewportManager.mdZoomRatio = (float)m_ViewRect[eMEAN].Width() / (float)iSelectImageWidth;
			else
				mViewportManager.mdZoomRatio = (float)m_ViewRect[eMEAN].Height() / (float)iSelectImageHeight;

			mViewportManager.miStartYPos = (int)dLTPoint.y;
			mViewportManager.miStartXPos = (int)dLTPoint.x;

			UpdateViewportManager();
		}

		miToolState = NC_TS_SELECT;
		PostMessage(WM_SETCURSOR);
	}
	else if (miToolState == NC_CREATE_ROI_POLYGON)
	{
		CPoint ClientViewPoint1 = point;

		ClientToScreen(&ClientViewPoint1);
		m_HWinPanelMean.ScreenToClient(&ClientViewPoint1);

		POINT IPoint = ClientViewPoint1;
		mViewportManager.VPtoIP(&IPoint, 1);

		m_PolygonPt[m_iPolygonCnt].x = IPoint.x;
		m_PolygonPt[m_iPolygonCnt].y = IPoint.y;
		++m_iPolygonCnt;

		InvalidateRect(NULL, FALSE);
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CAlgorithmImageCompareDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (!m_ViewRect[eMEAN].PtInRect(point))
		return;

	if (m_bPolygonMode && miToolState == NC_CREATE_ROI_POLYGON)
	{
		AddPolygonROI();

		m_bPolygonMode = FALSE;
		m_iPolygonCnt = 0;

		miToolState = NC_TS_SELECT;
		PostMessage(WM_SETCURSOR);

		InvalidateRect(NULL, FALSE);
	}
	else
	{
		CMenu* pSubMenu;
		pSubMenu = m_Menu.GetSubMenu(0);

		pSubMenu->EnableMenuItem(ID_HIMAGEVIEW_FULL, MF_ENABLED | MF_BYCOMMAND);
		pSubMenu->EnableMenuItem(ID_HIMAGEVIEW_100, MF_ENABLED | MF_BYCOMMAND);

		if (m_bDisplayInspectImage)
			pSubMenu->ModifyMenu(ID_HIMAGEVIEW_CHANGE_IMAGE, MF_BYCOMMAND | MF_STRING,
								 ID_HIMAGEVIEW_CHANGE_IMAGE, _T("평균영상으로 전환"));
		else
			pSubMenu->ModifyMenu(ID_HIMAGEVIEW_CHANGE_IMAGE, MF_BYCOMMAND | MF_STRING,
								 ID_HIMAGEVIEW_CHANGE_IMAGE, _T("검사영상으로 전환"));

		CPoint ClientPoint;
		ClientPoint = point;
		ClientToScreen(&ClientPoint);

		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, ClientPoint.x, ClientPoint.y, this);
	}

}

void CAlgorithmImageCompareDlg::DrawSelectPartTRegion()
{
	CDC *pDC = GetDC();
	
	CRgn ClipRgn;

	if (mpSelectPartTRegion)
	{
		ClipRgn.CreateRectRgn(m_ViewOffset[eMEAN].x, m_ViewOffset[eMEAN].y, m_ViewOffset[eMEAN].x+mViewportManager.miViewWidth, m_ViewOffset[eMEAN].y+mViewportManager.miViewHeight);
		pDC->SelectClipRgn(&ClipRgn);
		mpSelectPartTRegion->Draw(pDC->m_hDC, &mViewportManager, GTR_DS_SELECTPART, FALSE, THEAPP.m_pCalDataService, m_ViewOffset[eMEAN].x, m_ViewOffset[eMEAN].y);
	}
	ReleaseDC(pDC);
}

BOOL CAlgorithmImageCompareDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	switch (miToolState)
	{
	case NC_TS_SELECT_PART:
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SELECTPART));
		return TRUE;
	}

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CAlgorithmImageCompareDlg::SetViewportManager()
{
	mViewportManager.miImageWidth = m_iScanImageWidth;
	mViewportManager.miImageHeight = m_iScanImageHeight;
	mViewportManager.miStartXPos = 0;
	mViewportManager.miStartYPos = 0;
	mViewportManager.miViewWidth = m_ViewRect[eMEAN].Width();
	mViewportManager.miViewHeight = m_ViewRect[eMEAN].Height();

	if (m_iScanImageHeight >= m_iScanImageWidth)
		m_dFitZoomRatio = (double)m_ViewRect[eMEAN].Height() / (double)mViewportManager.miImageHeight;
	else
		m_dFitZoomRatio = (double)m_ViewRect[eMEAN].Width() / (double)mViewportManager.miImageWidth;
}

void CAlgorithmImageCompareDlg::UpdateViewportManager()
{
	POINT WHPoint;

	mViewportManager.GetImageWHPoint(&WHPoint);

	if (mViewportManager.miStartXPos + WHPoint.x > mViewportManager.miImageWidth)
		mViewportManager.miStartXPos = mViewportManager.miImageWidth - WHPoint.x;
	if (mViewportManager.miStartYPos + WHPoint.y > mViewportManager.miImageHeight)
		mViewportManager.miStartYPos = mViewportManager.miImageHeight - WHPoint.y;

	if (mViewportManager.miStartXPos < 0) mViewportManager.miStartXPos = 0;
	if (mViewportManager.miStartYPos < 0) mViewportManager.miStartYPos = 0;

	InvalidateRect(NULL, FALSE);
}

void CAlgorithmImageCompareDlg::ClearViewWindow()
{
	ClearWindow(m_HWinPanelMean.mlWindowHandle);
	ClearWindow(m_HWinPanelVar.mlWindowHandle);
	ClearWindow(m_HWinPanelLowerThres.mlWindowHandle);
	ClearWindow(m_HWinPanelUpperThres.mlWindowHandle);
}

void CAlgorithmImageCompareDlg::OnNcWidthfit()
{
	ClearViewWindow();
	mViewportManager.mdZoomRatio = m_dFitZoomRatio;
	UpdateViewportManager();

	miToolState = NC_TS_SELECT;
	PostMessage(WM_SETCURSOR);
}

void CAlgorithmImageCompareDlg::OnNc100()
{
	ClearViewWindow();
	mViewportManager.mdZoomRatio = 1.0;
	UpdateViewportManager();

	miToolState = NC_TS_SELECT;
	PostMessage(WM_SETCURSOR);
}

void CAlgorithmImageCompareDlg::OnNcSelectPart()
{
	miToolState = NC_TS_SELECT_PART;
}

void CAlgorithmImageCompareDlg::OnNcChangeImage()
{
	if (m_bDisplayInspectImage)
		m_bDisplayInspectImage = FALSE;
	else
	{
		SetInspectImage();
		m_bDisplayInspectImage = TRUE;
	}

	InvalidateRect(NULL, FALSE);
}

void CAlgorithmImageCompareDlg::OnNcDefectMasking()
{
	miToolState = NC_CREATE_ROI_POLYGON;

	m_bPolygonMode = TRUE;
	m_iPolygonCnt = 0;

	InvalidateRect(NULL, FALSE);
}

void CAlgorithmImageCompareDlg::OnNcDeleteDefectMasking()
{
	SAFE_DELETE(mpActiveTRegion);

	InvalidateRect(NULL, FALSE);
}

void CAlgorithmImageCompareDlg::OnNcParamOpt()
{
	try
	{
		if (mpActiveTRegion == NULL || THEAPP.m_pGFunction->ValidHRegion(mpActiveTRegion->m_HTeachPolygonRgn) == FALSE)
		{
			AfxMessageBox("불량 마스킹이 존재하지 않습니다. 먼저 불량 위치를 지정해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		if (m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex] < 0)
		{
			AfxMessageBox("영상비교 모델이 존재하지 않습니다. 먼저, 양품 이미지로 모델을 생성해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		if (AfxMessageBox("영상비교 모델 Threshold 파라미터 최적화를 진행하시겠습니까?", MB_OKCANCEL) == IDCANCEL)
			return;

		SetInspectImage();

		if (THEAPP.m_pGFunction->ValidHImage(m_HInspectDomainImage) == FALSE || THEAPP.m_pGFunction->ValidHRegion(m_HInspectDomainRgn) == FALSE)
		{
			AfxMessageBox("검사 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		HObject HMaskRgn;
		Intersection(m_HInspectDomainRgn, mpActiveTRegion->m_HTeachPolygonRgn, &HMaskRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HMaskRgn) == FALSE)
		{
			AfxMessageBox("불량 마스킹을 검사영역에 위치하도록 설정해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		HObject HDefectRgn;
		HTuple HArea, HDummy1, HDummy2;
		Hlong lMaskArea, lBrightDetectArea, lDarkDetectArea, lDefectArea;
		double dRatio, dBrightDetectRatio, dDarkDetectRatio;
		BOOL bFound[4];
		int i;

		for (i = 0; i < 4; i++)
			bFound[i] = FALSE;

		AreaCenter(HMaskRgn, &HArea, &HDummy1, &HDummy2);
		lMaskArea = HArea.L();

		int iThrDetect = THEAPP.Struct_PreferenceStruct.m_iVmOptThrDetect;
		double dMinPixelRatio = (double)THEAPP.Struct_PreferenceStruct.m_iVmOptMinPixelRatio;

		double dAbsThrMax;
		double dAbsThrMin = (double)THEAPP.Struct_PreferenceStruct.m_iVmOptAbsThresMin;
		double dAbsThrStep = (double)THEAPP.Struct_PreferenceStruct.m_iVmOptAbsThresStep;
		double dVarThrMax;
		double dVarThrMin = THEAPP.Struct_PreferenceStruct.m_dVmOptVarThresMin;
		double dVarThrStep = THEAPP.Struct_PreferenceStruct.m_dVmOptVarThresStep;
		double dIoUTarget = (double)THEAPP.Struct_PreferenceStruct.m_iVmOptIouTarget * 0.01;

		HObject HDiffSignedImage, HDiffSignedReducedImage;
		SubImage(m_HInspectDomainImage, m_HMeanImage, &HDiffSignedImage, 1, 128);
		ReduceDomain(HDiffSignedImage, HMaskRgn, &HDiffSignedReducedImage);

		HObject HBrightDetectRgn, HDarkDetectRgn;
		Threshold(HDiffSignedReducedImage, &HBrightDetectRgn, 128 + iThrDetect, 255);
		Threshold(HDiffSignedReducedImage, &HDarkDetectRgn, 0, 128 - iThrDetect);

		AreaCenter(HBrightDetectRgn, &HArea, &HDummy1, &HDummy2);
		lBrightDetectArea = HArea.L();
		AreaCenter(HDarkDetectRgn, &HArea, &HDummy1, &HDummy2);
		lDarkDetectArea = HArea.L();

		dBrightDetectRatio = (double)lBrightDetectArea / (double)lMaskArea * 100.0;
		dDarkDetectRatio = (double)lDarkDetectArea / (double)lMaskArea * 100.0;

		BOOL bOptimizeBright, bOptimizeDark;

		bOptimizeBright = bOptimizeDark = FALSE;

		if (m_bCheckUseImageCompareBright && dBrightDetectRatio >= dMinPixelRatio)
			bOptimizeBright = TRUE;
		if (m_bCheckUseImageCompareDark && dDarkDetectRatio >= dMinPixelRatio)
			bOptimizeDark = TRUE;

		if (!bOptimizeBright && !bOptimizeDark)
		{
			AfxMessageBox("마스킹 영역에서 유의미한 불량 픽셀이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		double dBestAbsU = (double)m_iEditImageCompareBrightAbs;
		double dBestAbsL = (double)m_iEditImageCompareDarkAbs;
		double dBestVarU = m_dEditImageCompareBrightVar;
		double dBestVarL = m_dEditImageCompareDarkVar;

		CAlgorithmParam AlgorithmParam;
		AlgorithmParam = THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetCurTabAlgorithmParam();

		if (bOptimizeBright)
		{
			dAbsThrMax = 60.0;

			for (double a = dAbsThrMax; a >= dAbsThrMin; a -= dAbsThrStep)
			{
				HDefectRgn = GetDefectRgn(FALSE, AlgorithmParam, a, 0.0, dBestAbsL, dBestVarL, m_bCheckUseImageCompareBright, m_bCheckUseImageCompareDark);

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					Intersection(HMaskRgn, HDefectRgn, &HDefectRgn);

					AreaCenter(HDefectRgn, &HArea, &HDummy1, &HDummy2);
					lDefectArea = HArea.L();

					dRatio = (double)lDefectArea / (double)lMaskArea;

					if (dRatio >= dIoUTarget)
					{
						dBestAbsU = a;
						bFound[0] = TRUE;
						break;
					}
				}
			}

			if (bFound[0] == FALSE)
				dBestAbsU = (double)m_iEditImageCompareBrightAbs;
		}

		if (bOptimizeDark)
		{
			dAbsThrMax = 60.0;

			for (double a = dAbsThrMax; a >= dAbsThrMin; a -= dAbsThrStep)
			{
				HDefectRgn = GetDefectRgn(FALSE, AlgorithmParam, dBestAbsU, dBestVarU, a, 0.0, m_bCheckUseImageCompareBright, m_bCheckUseImageCompareDark);

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					Intersection(HMaskRgn, HDefectRgn, &HDefectRgn);

					AreaCenter(HDefectRgn, &HArea, &HDummy1, &HDummy2);
					lDefectArea = HArea.L();

					dRatio = (double)lDefectArea / (double)lMaskArea;

					if (dRatio >= dIoUTarget)
					{
						dBestAbsL = a;
						bFound[1] = TRUE;
						break;
					}
				}
			}

			if (bFound[1] == FALSE)
				dBestAbsL = (double)m_iEditImageCompareDarkAbs;
		}

		if (bOptimizeBright)
		{
			dVarThrMax = 6.0;

			for (double b = dVarThrMax; b >= dVarThrMin; b -= dVarThrStep)
			{
				HDefectRgn = GetDefectRgn(FALSE, AlgorithmParam, dBestAbsU, b, dBestAbsL, dBestVarL, m_bCheckUseImageCompareBright, m_bCheckUseImageCompareDark);

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					Intersection(HMaskRgn, HDefectRgn, &HDefectRgn);

					AreaCenter(HDefectRgn, &HArea, &HDummy1, &HDummy2);
					lDefectArea = HArea.L();

					dRatio = (double)lDefectArea / (double)lMaskArea;

					if (dRatio >= dIoUTarget)
					{
						dBestVarU = b;
						bFound[2] = TRUE;
						break;
					}
				}
			}

			if (bFound[2] == FALSE)
				dBestVarU = m_dEditImageCompareBrightVar;
		}

		if (bOptimizeDark)
		{
			dVarThrMax = 6.0;

			for (double b = dVarThrMax; b >= dVarThrMin; b -= dVarThrStep)
			{
				HDefectRgn = GetDefectRgn(FALSE, AlgorithmParam, dBestAbsU, dBestVarU, dBestAbsL, b, m_bCheckUseImageCompareBright, m_bCheckUseImageCompareDark);

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					Intersection(HMaskRgn, HDefectRgn, &HDefectRgn);

					AreaCenter(HDefectRgn, &HArea, &HDummy1, &HDummy2);
					lDefectArea = HArea.L();

					dRatio = (double)lDefectArea / (double)lMaskArea;

					if (dRatio >= dIoUTarget)
					{
						dBestVarL = b;
						bFound[3] = TRUE;
						break;
					}
				}
			}

			if (bFound[3] == FALSE)
				dBestVarL = m_dEditImageCompareDarkVar;
		}

		BOOL bUpdateMsg = FALSE;
		for (i = 0; i < 4; i++)
		{
			if (bFound[i])
			{
				bUpdateMsg = TRUE;
				break;
			}
		}

		if (bUpdateMsg)
		{
			BOOL bFoundDefect = FALSE;

			HDefectRgn = GetDefectRgn(TRUE, AlgorithmParam, dBestAbsU, dBestVarU, dBestAbsL, dBestVarL, m_bCheckUseImageCompareBright, m_bCheckUseImageCompareDark);

			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			{
				Intersection(HMaskRgn, HDefectRgn, &HDefectRgn);

				AreaCenter(HDefectRgn, &HArea, &HDummy1, &HDummy2);
				lDefectArea = HArea.L();

				dRatio = (double)lDefectArea / (double)lMaskArea;

				if (dRatio >= (dIoUTarget * 0.9))
					bFoundDefect = TRUE;
			}

			if (bFoundDefect)
			{
				CString sMsg;
				sMsg.Format("밝은 불량: 절대밝기차 %d, V*표준편차: %.2lf\r\n어두운 불량: 절대밝기차 %d, V*표준편차: %.2lf\r\n파라미터로 적용하시겠습니까?", (int)dBestAbsU, dBestVarU, (int)dBestAbsL, dBestVarL);
				if (AfxMessageBox(sMsg, MB_OKCANCEL) == IDOK)
				{
					m_iEditImageCompareBrightAbs = (int)dBestAbsU;
					m_iEditImageCompareDarkAbs = (int)dBestAbsL;
					m_dEditImageCompareBrightVar = dBestVarU;
					m_dEditImageCompareDarkVar = dBestVarL;

					UpdateData(FALSE);
				}
			}
			else
			{
				AfxMessageBox("영상비교 파라미터는 조건을 만족하였지만, 불량검출조건으로 인해 최종 불량검출이 되지 않았습니다.\r\n불량검출조건을 변경 후 다시 시도해 주십시요.", MB_SYSTEMMODAL);
			}
		}
		else
		{
			AfxMessageBox("마스킹 영역에서 유의미한 불량 픽셀이 존재하지 않습니다.\r\n불량 마스킹을 적절히 조정해 주십시요.", MB_SYSTEMMODAL);
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

		strLog.Format("Halcon Exception [AlgorithmImageCompareDlg::OnNcParamOpt] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		AfxMessageBox("처리 오류 발생. 로그를 확인해 주세요.", MB_SYSTEMMODAL);
		return;
	}
}

#include "VmParamSetDlg.h"

void CAlgorithmImageCompareDlg::OnNcParamSet()
{
	CVmParamSetDlg dlg;
	dlg.DoModal();
}

HObject CAlgorithmImageCompareDlg::GetDefectRgn(BOOL bApplyCondition, CAlgorithmParam AlgorithmParam, double dAbsThrU, double dVarThrU, double dAbsThrL, double dVarThrL, BOOL bUseBright, BOOL bUseDark)
{
	HObject HDefectRgn;
	GenEmptyObj(&HDefectRgn);

	try
	{
		HTuple HAbsThreshold, HVarThreshold;
		HObject HBrightRgn, HDarkRgn, HThresRgn;
		HObject HTempBrightRgn, HTempDarkRgn;
		int iArea;
		double dXLength, dYLength;

		TupleGenConst(0, 0, &HAbsThreshold);
		TupleGenConst(0, 0, &HVarThreshold);

		TupleConcat(HAbsThreshold, HTuple(dAbsThrU), &HAbsThreshold);
		TupleConcat(HAbsThreshold, HTuple(dAbsThrL), &HAbsThreshold);

		TupleConcat(HVarThreshold, HTuple(dVarThrU), &HVarThreshold);
		TupleConcat(HVarThreshold, HTuple(dVarThrL), &HVarThreshold);

		if (m_bCheckUseWindow)
			PrepareDirectVariationModel(m_pSelectedROI->m_HVarMeanImage[m_iSelectedInspectionTabIndex], m_pSelectedROI->m_HVarStdevImage[m_iSelectedInspectionTabIndex], m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], HAbsThreshold, HVarThreshold);
		else
			PrepareVariationModel(m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], HAbsThreshold, HVarThreshold);

		GenEmptyObj(&HBrightRgn);
		GenEmptyObj(&HDarkRgn);
		GenEmptyObj(&HThresRgn);

		if (bUseBright == TRUE && bUseDark == TRUE)
		{
			CompareExtVariationModel(m_HInspectDomainImage, &HThresRgn, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], "light_dark");
			SelectObj(HThresRgn, &HTempBrightRgn, 1);
			SelectObj(HThresRgn, &HTempDarkRgn, 2);

			if (THEAPP.m_pGFunction->ValidHRegion(HTempBrightRgn))
				ConcatObj(HBrightRgn, HTempBrightRgn, &HBrightRgn);

			if (THEAPP.m_pGFunction->ValidHRegion(HTempDarkRgn))
				ConcatObj(HDarkRgn, HTempDarkRgn, &HDarkRgn);
		}
		else if (bUseBright == TRUE && bUseDark == FALSE)
		{
			CompareExtVariationModel(m_HInspectDomainImage, &HTempBrightRgn, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], "light");

			if (THEAPP.m_pGFunction->ValidHRegion(HTempBrightRgn))
				ConcatObj(HBrightRgn, HTempBrightRgn, &HBrightRgn);
		}
		else if (bUseBright == FALSE && bUseDark == TRUE)
		{
			CompareExtVariationModel(m_HInspectDomainImage, &HTempDarkRgn, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex], "dark");

			if (THEAPP.m_pGFunction->ValidHRegion(HTempDarkRgn))
				ConcatObj(HDarkRgn, HTempDarkRgn, &HDarkRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
		{
			Union1(HBrightRgn, &HBrightRgn);
			ConcatObj(HDefectRgn, HBrightRgn, &HDefectRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
		{
			Union1(HDarkRgn, &HDarkRgn);
			ConcatObj(HDefectRgn, HDarkRgn, &HDefectRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == FALSE && THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == FALSE)
			return HDefectRgn;

		if (bApplyCondition)
			HDefectRgn = THEAPP.m_pAlgorithm->DefectContidion(0, m_HInspectDomainImage, m_HInspectDomainRgn, HBrightRgn, HDarkRgn, AlgorithmParam, &iArea, &dXLength, &dYLength);

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			Union1(HDefectRgn, &HDefectRgn);

		return HDefectRgn;
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

		strLog.Format("Halcon Exception [AlgorithmImageCompareDlg::GetDefectRgn] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return HDefectRgn;
	}
}

void CAlgorithmImageCompareDlg::DisplayCross(HDC hDestDC, GViewportManager* pVManager, double dXPos, double dYPos, COLORREF Color, long lRadiusPxl)
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

void CAlgorithmImageCompareDlg::AddPolygonROI()
{
	if (m_iPolygonCnt <= 2)
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

	if (!mpActiveTRegion)
	{
		pPolygonRgn = new GTRegion;
		pPolygonRgn->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
		pPolygonRgn->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
		pPolygonRgn->SetVisible(TRUE, FALSE);
		pPolygonRgn->SetSelectable(FALSE, FALSE);
		pPolygonRgn->SetMovable(FALSE, FALSE);
		pPolygonRgn->SetSizable(FALSE, FALSE);
		pPolygonRgn->SetLineColor(RGB(0, 255, 0));
		pPolygonRgn->m_bRegionROI = TRUE;
		CopyObj(HPolygonRgn, &(pPolygonRgn->m_HTeachPolygonRgn), 1, -1);

		mpActiveTRegion = pPolygonRgn;
	}
	else
	{
		Union2(mpActiveTRegion->m_HTeachPolygonRgn, HPolygonRgn, &mpActiveTRegion->m_HTeachPolygonRgn);
	}
}

void CAlgorithmImageCompareDlg::SetInspectImage()
{
	try
	{
		GenEmptyObj(&m_HInspectDomainImage);
		GenEmptyObj(&m_HInspectDomainRgn);

		BOOL bDebugSave = FALSE;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			return;
		}

		CAlgorithmParam AlgorithmParam;

		AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information

		THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->m_pAlgorithmImageProcessDlg->GetParam(&AlgorithmParam);	// Image Processing Tab Data
		THEAPP.m_pTabControlDlg->AlgorithmPreProcessing();

		UpdateData();

		HObject HROIRgn, HInspectAreaRgn;
		HROIRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		// 1. 검사 ROI 전처리
		if (AlgorithmParam.m_bUseImageProcessLocalAlign)
			THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIRgn, TEACHING_THREAD_INDEX);

		if (AlgorithmParam.m_bUseImageProcessGenerate)
			THEAPP.m_pAlgorithm->ApplyGenerateResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIRgn, TEACHING_THREAD_INDEX);

		if (AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
			THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIRgn, TEACHING_THREAD_INDEX);

		THEAPP.m_pAlgorithm->GetInspectArea(HROIRgn, AlgorithmParam, &HInspectAreaRgn);

		HTuple HlOriginLTPointY, HlOriginLTPointX, HlOriginRBPointY, HlOriginRBPointX;
		SmallestRectangle1(HInspectAreaRgn, &HlOriginLTPointY, &HlOriginLTPointX, &HlOriginRBPointY, &HlOriginRBPointX);

		HTuple HomMat2DRotate, HomMat2DRotate2;
		HObject HCropImage;
		GTRegion* pInspectROIRgn;
		double dDeltaAngle = 0;
		BOOL bLocalAlignROIFindCheck = FALSE;
		if (AlgorithmParam.m_bUseImageProcessLocalAlign &&
			AlgorithmParam.m_iImageProcessLocalAlignROIType == LOCAL_ALIGN_MATCHING_ROI_TYPE_MATCHING)
		{
			int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
			int iLocalAlignImageNo = AlgorithmParam.m_iImageProcessLocalAlignImageIndex + 1;

			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				if (pInspectROIRgn->miTeachImageIndex == iLocalAlignImageNo)
				{
					if (pInspectROIRgn->miLocalAlignID == AlgorithmParam.m_iImageProcessLocalAlignROINo)
					{
						bLocalAlignROIFindCheck = TRUE;
						break;
					}
				}
			}

			// 2. 연산량 감소를 위해 전체 영상에서 검사 ROI 영역 크롭
			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HInspectAreaRgn, &HCropImage);
			CropDomain(HCropImage, &HCropImage);
		}
		else
		{
			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HInspectAreaRgn, &HCropImage);
			CropDomain(HCropImage, &HCropImage);
		}

		GetDomain(HCropImage, &m_HInspectDomainRgn);

		if (bLocalAlignROIFindCheck)
			dDeltaAngle = -pInspectROIRgn->m_dLocalAlignDeltaAngle[TEACHING_THREAD_INDEX];

		// 3. 이동 및 회전 변환 행렬 계산
		int iTempY = 0;
		int iTempX = 0;
		if (dDeltaAngle != 0)
		{
			HTuple HArea, HCenterX, HCenterY;
			AreaCenter(HCropImage, &HArea, &HCenterY, &HCenterX);

			HomMat2dIdentity(&HomMat2DRotate);
			HomMat2dRotate(HomMat2DRotate, dDeltaAngle, HCenterY, HCenterX, &HomMat2DRotate);
			AffineTransImage(HCropImage, &HCropImage, HomMat2DRotate, "bicubic", "false");
			AffineTransRegion(m_HInspectDomainRgn, &m_HInspectDomainRgn, HomMat2DRotate, "nearest_neighbor");

			HomMat2dIdentity(&HomMat2DRotate2);
			HomMat2dRotate(HomMat2DRotate2, -dDeltaAngle, HCenterY, HCenterX, &HomMat2DRotate2);

			// 4. Local align을 통해 회전된 ROI의 크기 보정
			int CropLTPointY, CropLTPointX, CropRBPointY, CropRBPointX;
			HObject HResizeInspectAreaRgn;
			CropLTPointY = (HCenterY - (HlOriginRBPointY - HlOriginLTPointY) / 2).D();
			CropRBPointY = (HCenterY + (HlOriginRBPointY - HlOriginLTPointY) / 2).D();
			CropLTPointX = (HCenterX - (HlOriginRBPointX - HlOriginLTPointX) / 2).D();
			CropRBPointX = (HCenterX + (HlOriginRBPointX - HlOriginLTPointX) / 2).D();

			int iShiftPixelY, iShiftPixelX;
			iShiftPixelY = CropLTPointY;
			iShiftPixelX = CropLTPointX;
			MoveRegion(m_HInspectDomainRgn, &m_HInspectDomainRgn, -iShiftPixelY, -iShiftPixelX);

			if (CropLTPointY != 0)
			{
				CropRBPointY -= CropLTPointY;
				CropLTPointY = 0;
			}
			if (CropLTPointX != 0)
			{
				CropRBPointX -= CropLTPointX;
				CropLTPointX = 0;
			}

			if (m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex] >= 0 &&
				m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex] >= 0 &&
				(CropRBPointY != m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex] ||
				 CropRBPointX != m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex]))
			{
				iTempY = (CropRBPointY - m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex]) / 2;
				CropLTPointY += iTempY;
				CropRBPointY -= iTempY;

				iTempX = (CropRBPointX - m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex]) / 2;
				CropLTPointX += iTempX;
				CropRBPointX -= iTempX;
			}

			GenRectangle1(&HResizeInspectAreaRgn, CropLTPointY, CropLTPointX, CropRBPointY, CropRBPointX);
			ReduceDomain(HCropImage, HResizeInspectAreaRgn, &HCropImage);
			CropDomain(HCropImage, &HCropImage);
		}

		if (bDebugSave)
			WriteImage(HCropImage, "bmp", 0, "c:\\DualTest\\HCropImage");

		if (m_bCheckUseImageCompareEdgeImage)
			SobelAmp(HCropImage, &HCropImage, "sum_abs", 3);

		// 5. 크롭된 영역 학습
		HTuple HImageWidth, HImageHeight;
		GetImageSize(HCropImage, &HImageWidth, &HImageHeight);

		if (m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex] > 0)
		{
			HObject HLowerThresImage, HUpperThresImage;
			HTuple HLowerThresImageWidth, HLowerThresImageHeight;
			GetThreshImagesVariationModel(&HLowerThresImage, &HUpperThresImage, m_pSelectedROI->m_HVarModelID[m_iSelectedInspectionTabIndex]);
			GetImageSize(HLowerThresImage, &HLowerThresImageWidth, &HLowerThresImageHeight);
			m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex] = HLowerThresImageWidth[0].L();
			m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex] = HLowerThresImageHeight[0].L();

			if ((m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex] != HImageWidth[0].L()) || (m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex] != HImageHeight[0].L()))
			{
				if (m_pSelectedROI->m_iVarModelImageSizeX > 0 && m_pSelectedROI->m_iVarModelImageSizeY > 0)
					ZoomImageSize(HCropImage, &HCropImage, m_pSelectedROI->m_iVarModelImageSizeX[m_iSelectedInspectionTabIndex], m_pSelectedROI->m_iVarModelImageSizeY[m_iSelectedInspectionTabIndex], "nearest_neighbor");
			}
		}

		if (bDebugSave)
			WriteImage(HCropImage, "bmp", 0, "c:\\DualTest\\HCropImage2");

		CopyImage(HCropImage, &m_HInspectDomainImage);
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

		strLog.Format("Halcon Exception [AlgorithmImageCompareDlg::SetInspectImage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmImageCompareDlg::OnBnClickedButtonShowNormalizeImage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	try
	{
		BOOL bDebugSave = FALSE;

		if (THEAPP.m_pGFunction->ValidHImage(m_HInspectImage) == FALSE)
		{	
			if (!THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]))
			{
				AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
				return;
			}

			CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], &m_HInspectImage);
		}

		CAlgorithmParam AlgorithmParam;
		AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information

		THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->m_pAlgorithmImageProcessDlg->GetParam(&AlgorithmParam);	// Image Processing Tab Data
		THEAPP.m_pTabControlDlg->AlgorithmPreProcessing();

		BOOL bCheckedShape;
		bCheckedShape = THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetCheckedShapeImage();

		UpdateData();

		HObject HROIRgn, HInspectAreaRgn;
		HROIRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		// 1. 검사 ROI 전처리
		if (AlgorithmParam.m_bUseImageProcessLocalAlign)
			THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIRgn, TEACHING_THREAD_INDEX);

		if (AlgorithmParam.m_bUseImageProcessGenerate)
			THEAPP.m_pAlgorithm->ApplyGenerateResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIRgn, TEACHING_THREAD_INDEX);

		if (AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
			THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIRgn, TEACHING_THREAD_INDEX);

		THEAPP.m_pAlgorithm->GetInspectArea(HROIRgn, AlgorithmParam, &HInspectAreaRgn);

		HTuple HomMat2DRotate, HomMat2DRotate2;
		HObject HCropImage;
		GTRegion *pInspectROIRgn;
		double dDeltaAngle = 0;
		BOOL bLocalAlignROIFindCheck = FALSE;
		if (AlgorithmParam.m_bUseImageProcessLocalAlign &&
			AlgorithmParam.m_iImageProcessLocalAlignROIType == LOCAL_ALIGN_MATCHING_ROI_TYPE_MATCHING)
		{
			int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
			int iLocalAlignImageNo = AlgorithmParam.m_iImageProcessLocalAlignImageIndex + 1;

			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				if (pInspectROIRgn->miTeachImageIndex == iLocalAlignImageNo)
				{
					if (pInspectROIRgn->miLocalAlignID == AlgorithmParam.m_iImageProcessLocalAlignROINo)
					{
						bLocalAlignROIFindCheck = TRUE;
						break;
					}
				}
			}		
		}

		// 2. 연산량 감소를 위해 전체 영상에서 검사 ROI 영역 크롭
		ReduceDomain(m_HInspectImage, HInspectAreaRgn, &HCropImage);
		CropDomain(HCropImage, &HCropImage);

		if (bLocalAlignROIFindCheck)
			dDeltaAngle = -pInspectROIRgn->m_dLocalAlignDeltaAngle[TEACHING_THREAD_INDEX];

		// 3. 이동 및 회전 변환 행렬 계산
		HObject HROIRegionMoved;
		if (dDeltaAngle != 0)
		{
			HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
			SmallestRectangle1(HCropImage, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

			HomMat2dIdentity(&HomMat2DRotate);
			HomMat2dRotate(HomMat2DRotate, dDeltaAngle, (HlRBPointY - HlLTPointY) / 2, (HlRBPointX - HlLTPointX) / 2, &HomMat2DRotate);
			AffineTransImage(HCropImage, &HCropImage, HomMat2DRotate, "bicubic", "false");

			HomMat2dIdentity(&HomMat2DRotate2);
			HomMat2dRotate(HomMat2DRotate2, -dDeltaAngle, (HlRBPointY - HlLTPointY) / 2, (HlRBPointX - HlLTPointX) / 2, &HomMat2DRotate2);

			// 4. Local align을 통해 회전된 ROI와 회전전 원래의 ROI의 SmallestRectangle1 크기 보정
			HTuple HArea, HCenterX, HCenterY;
			AreaCenter(HCropImage, &HArea, &HCenterY, &HCenterX);

			HObject HResizeInspectAreaRgn, HCropResizeInspectAreaRgn;
			AffineTransRegion(HInspectAreaRgn, &HResizeInspectAreaRgn, HomMat2DRotate, "constant");
			SmallestRectangle1(HResizeInspectAreaRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
			GenRectangle1(&HCropResizeInspectAreaRgn, HCenterY - (HlRBPointY - HlLTPointY) / 2, HCenterX - (HlRBPointX - HlLTPointX) / 2, HCenterY + (HlRBPointY - HlLTPointY) / 2, HCenterX + (HlRBPointX - HlLTPointX) / 2);

			ReduceDomain(HCropImage, HCropResizeInspectAreaRgn, &HCropImage);
			CropDomain(HCropImage, &HCropImage);
		}

		if (m_bCheckUseImageCompareEdgeImage)
			SobelAmp(HCropImage, &HCropImage, "sum_abs", 3);

		HTuple HMeanWidth, HMeanHeight, HCropWidth, HCropHeight;
		GetImageSize(m_HMeanImage, &HMeanWidth, &HMeanHeight);
		GetImageSize(HCropImage, &HCropWidth, &HCropHeight);

		if (HCropWidth[0].L() != HMeanWidth[0].L() || HCropHeight[0].L() != HMeanHeight[0].L())
		{
			if (HMeanWidth[0].L() > 0 && HMeanHeight[0].L() > 0)
				ZoomImageSize(HCropImage, &HCropImage, HMeanWidth, HMeanHeight, "bilinear");
		}

		HObject HROIMask;
		GenRectangle1(&HROIMask, 0, 0, HMeanHeight[0].D() - 1, HMeanWidth[0].D() - 1);

		VariationNormalizerParam param;
		param.sigma_b = m_iEditImageCompareNormalizeWeight;

		HObject HNormalized;
		CVariationNormalizer normalizer;

		if (normalizer.ApplyAdaptiveNormalization(HCropImage, m_HMeanImage, HROIMask, param, &HNormalized))
		{
			HTuple HRow1, HCol1, HRow2, HCol2;
			SmallestRectangle1(HInspectAreaRgn, &HRow1, &HCol1, &HRow2, &HCol2);

			HTuple HomMat2D;
			HomMat2dIdentity(&HomMat2D);

			if (dDeltaAngle != 0)
			{
				HomMat2dCompose(HomMat2DRotate2, HomMat2D, &HomMat2D);
			}

			HomMat2dTranslate(HomMat2D, HRow1, HCol1, &HomMat2D);

			HTuple HWidth, HHeight;
			GetImageSize(m_HInspectImage, &HWidth, &HHeight);

			HObject HResultImage;
			AffineTransImageSize(HNormalized, &HResultImage, HomMat2D, "bicubic", HWidth, HHeight);

			CopyImage(HResultImage, &THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]);

			THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();

			if (bDebugSave)
			{
				WriteImage(HCropImage, "bmp", 0, "C:\\DualTest\\01_Original");
				WriteImage(m_HMeanImage, "bmp", 0, "C:\\DualTest\\02_MeanTemplate");
				WriteImage(HNormalized, "bmp", 0, "C:\\DualTest\\03_Rectified");
			}

			AfxMessageBox("조명 보정 완료.", MB_OK | MB_ICONINFORMATION);
		}
		else
			AfxMessageBox("조명 보정 실패.", MB_OK);

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

		strLog.Format("Halcon Exception [AlgorithmImageCompareDlg::OnBnClickedButtonTrainCurrentImage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}
