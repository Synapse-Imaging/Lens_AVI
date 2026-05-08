// AlgorithmROIAnisoAlignDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmROIAnisoAlignDlg.h"
#include "afxdialogex.h"


// CAlgorithmROIAnisoAlignDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmROIAnisoAlignDlg, CDialog)

CAlgorithmROIAnisoAlignDlg::CAlgorithmROIAnisoAlignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmROIAnisoAlignDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseROIAnisoAlign = FALSE;
	m_iEditROIAnisoAlignXSearchMargin = 0;
	m_iEditROIAnisoAlignYSearchMargin = 0;
	m_iEditROIAnisoAlignMatchingAngleRange = 0;
	m_dEditROIAnisoAlignMatchingScaleMinY = 1.0;
	m_dEditROIAnisoAlignMatchingScaleMaxY = 1.0;
	m_dEditROIAnisoAlignMatchingScaleMinX = 1.0;
	m_dEditROIAnisoAlignMatchingScaleMaxX = 1.0;
	m_iEditROIAnisoAlignMatchingMinContrast = 0;
	m_dEditROIAnisoAlignShiftTop = 0.0;
	m_dEditROIAnisoAlignShiftBottom = 0.0;
	m_dEditROIAnisoAlignShiftLeft = 0.0;
	m_dEditROIAnisoAlignShiftRight = 0.0;
	m_bCheckROIAnisoAlignLocalAlignUse = FALSE;
	m_dEditROIAnisoAlignMatchingScore = 0.0;
	m_bCheckROIColorImage = FALSE;

	m_bCheckROIAlignMultiModuleInspect = FALSE;
	m_iEditROIAlignMultiModuleXNumber = 1;
	m_iEditROIAlignMultiModuleYNumber = 1;
	m_iEditROIAlignMultiModuleXPitch = 1000;
	m_iEditROIAlignMultiModuleYPitch = 1000;

	m_iRadioROIAnisoAlignMultiModuleAlignTarget = 0;
	m_iRadioROIAnisoAlignMultiModuleVision = 1;
}

CAlgorithmROIAnisoAlignDlg::~CAlgorithmROIAnisoAlignDlg()
{
}

void CAlgorithmROIAnisoAlignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_ROI_ALIGN, m_bCheckUseROIAnisoAlign);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_X_SEARCH_MARGIN, m_iEditROIAnisoAlignXSearchMargin);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_Y_SEARCH_MARGIN, m_iEditROIAnisoAlignYSearchMargin);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_ANGLE_RANGE, m_iEditROIAnisoAlignMatchingAngleRange);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCALE_MIN_Y, m_dEditROIAnisoAlignMatchingScaleMinY);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCALE_MAX_Y, m_dEditROIAnisoAlignMatchingScaleMaxY);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCALE_MIN_X, m_dEditROIAnisoAlignMatchingScaleMinX);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCALE_MAX_X, m_dEditROIAnisoAlignMatchingScaleMaxX);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_MIN_CONTRAST, m_iEditROIAnisoAlignMatchingMinContrast);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_USE_POS_X, m_bCheckROIAnisoAlignUsePosX);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_USE_POS_Y, m_bCheckROIAnisoAlignUsePosY);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_INSPECT_SHIFT, m_bCheckROIAnisoAlignInspectShift);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_PLEVEL, m_iEditROIAnisoAlignMatchingPLevel);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_TOP, m_dEditROIAnisoAlignShiftTop);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_BOTTOM, m_dEditROIAnisoAlignShiftBottom);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_LEFT, m_dEditROIAnisoAlignShiftLeft);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_RIGHT, m_dEditROIAnisoAlignShiftRight);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_LOCAL_ALIGN_USE, m_bCheckROIAnisoAlignLocalAlignUse);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCORE, m_dEditROIAnisoAlignMatchingScore);
	DDX_Check(pDX, IDC_CHECK_USE_COLOR_IMAGE, m_bCheckROIColorImage);

	DDX_Check(pDX, IDC_CHECK_ROI_ANISO_ALIGN_MULTI_MODULE_INSPECT, m_bCheckROIAlignMultiModuleInspect);
	DDX_Text(pDX, IDC_EDIT_ROI_ANISO_ALIGN_MULTI_MODULE_X_NUMBER, m_iEditROIAlignMultiModuleXNumber);
	DDX_Text(pDX, IDC_EDIT_ROI_ANISO_ALIGN_MULTI_MODULE_Y_NUMBER, m_iEditROIAlignMultiModuleYNumber);
	DDX_Text(pDX, IDC_EDIT_ROI_ANISO_ALIGN_MULTI_MODULE_X_PITCH, m_iEditROIAlignMultiModuleXPitch);
	DDX_Text(pDX, IDC_EDIT_ROI_ANISO_ALIGN_MULTI_MODULE_Y_PITCH, m_iEditROIAlignMultiModuleYPitch);

	DDX_Radio(pDX, IDC_RADIO_ROI_ANISO_ALIGN_MULTI_MODULE_MATCHING, m_iRadioROIAnisoAlignMultiModuleAlignTarget);
	DDX_Radio(pDX, IDC_RADIO_ROI_ANISO_ALIGN_MULTI_MODULE_TOP1, m_iRadioROIAnisoAlignMultiModuleVision);
}

BEGIN_MESSAGE_MAP(CAlgorithmROIAnisoAlignDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TEST_MATCHING_MODEL, &CAlgorithmROIAnisoAlignDlg::OnBnClickedButtonTestMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_MATCHING_MODEL, &CAlgorithmROIAnisoAlignDlg::OnBnClickedButtonCreateMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_TEST_MATCHING_MULTI_MODULE, &CAlgorithmROIAnisoAlignDlg::OnBnClickedButtonTestMatchingMultiModule)
	ON_BN_CLICKED(IDC_CHECK_ROI_ALIGN_LOCAL_ALIGN_USE, &CAlgorithmROIAnisoAlignDlg::OnBnClickedCheckRoiAlignLocalAlignUse)
	ON_BN_CLICKED(IDC_CHECK_ROI_ANISO_ALIGN_MULTI_MODULE_INSPECT, &CAlgorithmROIAnisoAlignDlg::OnBnClickedCheckRoiAnisoAlignMultiModuleInspect)
END_MESSAGE_MAP()


// CAlgorithmROIAnisoAlignDlg 메시지 처리기입니다.

void CAlgorithmROIAnisoAlignDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;

	if (pSelectedROI->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		GetDlgItem(IDC_CHECK_USE_ROI_ALIGN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_TEST_MATCHING_MODEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_CREATE_MATCHING_MODEL)->EnableWindow(TRUE);
	}
	else
	{
		if (m_iSelectedInspectionTabIndex == 0)
		{
			GetDlgItem(IDC_CHECK_USE_ROI_ALIGN)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_TEST_MATCHING_MODEL)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_CREATE_MATCHING_MODEL)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_ROI_ALIGN)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_TEST_MATCHING_MODEL)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_CREATE_MATCHING_MODEL)->EnableWindow(FALSE);
		}
	}
}

void CAlgorithmROIAnisoAlignDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseROIAnisoAlign = AlgorithmParam.m_bUseROIAnisoAlign;
	m_iEditROIAnisoAlignXSearchMargin = AlgorithmParam.m_iROIAnisoAlignXSearchMargin;
	m_iEditROIAnisoAlignYSearchMargin = AlgorithmParam.m_iROIAnisoAlignYSearchMargin;
	m_iEditROIAnisoAlignMatchingAngleRange = AlgorithmParam.m_iROIAnisoAlignMatchingAngleRange;
	m_dEditROIAnisoAlignMatchingScaleMinY = AlgorithmParam.m_dROIAnisoAlignMatchingScaleMinY;
	m_dEditROIAnisoAlignMatchingScaleMaxY = AlgorithmParam.m_dROIAnisoAlignMatchingScaleMaxY;
	m_dEditROIAnisoAlignMatchingScaleMinX = AlgorithmParam.m_dROIAnisoAlignMatchingScaleMinX;
	m_dEditROIAnisoAlignMatchingScaleMaxX = AlgorithmParam.m_dROIAnisoAlignMatchingScaleMaxX;
	m_iEditROIAnisoAlignMatchingMinContrast = AlgorithmParam.m_iROIAnisoAlignMatchingMinContrast;
	m_bCheckROIAnisoAlignUsePosX = AlgorithmParam.m_bROIAnisoAlignUsePosX;
	m_bCheckROIAnisoAlignUsePosY = AlgorithmParam.m_bROIAnisoAlignUsePosY;
	m_bCheckROIAnisoAlignInspectShift = AlgorithmParam.m_bROIAnisoAlignInspectShift;
	m_iEditROIAnisoAlignMatchingPLevel = AlgorithmParam.m_iROIAnisoAlignMatchingPLevel;
	m_dEditROIAnisoAlignShiftTop = AlgorithmParam.m_dROIAnisoAlignShiftTop;
	m_dEditROIAnisoAlignShiftBottom = AlgorithmParam.m_dROIAnisoAlignShiftBottom;
	m_dEditROIAnisoAlignShiftLeft = AlgorithmParam.m_dROIAnisoAlignShiftLeft;
	m_dEditROIAnisoAlignShiftRight = AlgorithmParam.m_dROIAnisoAlignShiftRight;
	m_bCheckROIAnisoAlignLocalAlignUse = AlgorithmParam.m_bROIAnisoAlignLocalAlignUse;
	m_dEditROIAnisoAlignMatchingScore = AlgorithmParam.m_dROIAnisoAlignMatchingScore;
	m_bCheckROIColorImage = AlgorithmParam.m_bUseAnisoColorImage;
	m_bCheckROIAlignMultiModuleInspect = AlgorithmParam.m_bROIAnisoAlignMultiModuleInspect;
	m_iEditROIAlignMultiModuleXNumber = AlgorithmParam.m_iROIAnisoAlignMultiModuleXNumber;
	m_iEditROIAlignMultiModuleYNumber = AlgorithmParam.m_iROIAnisoAlignMultiModuleYNumber;
	m_iEditROIAlignMultiModuleXPitch = AlgorithmParam.m_iROIAnisoAlignMultiModuleXPitch;
	m_iEditROIAlignMultiModuleYPitch = AlgorithmParam.m_iROIAnisoAlignMultiModuleYPitch;
	m_iRadioROIAnisoAlignMultiModuleAlignTarget = AlgorithmParam.m_iROIAnisoAlignMultiModuleAlignTarget;
	m_iRadioROIAnisoAlignMultiModuleVision = AlgorithmParam.m_iROIAnisoAlignMultiModuleVision;

	UpdateData(FALSE);
}

void CAlgorithmROIAnisoAlignDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseROIAnisoAlign = m_bCheckUseROIAnisoAlign;
	pAlgorithmParam->m_iROIAnisoAlignXSearchMargin = m_iEditROIAnisoAlignXSearchMargin;
	pAlgorithmParam->m_iROIAnisoAlignYSearchMargin = m_iEditROIAnisoAlignYSearchMargin;
	pAlgorithmParam->m_iROIAnisoAlignMatchingAngleRange = m_iEditROIAnisoAlignMatchingAngleRange;
	pAlgorithmParam->m_dROIAnisoAlignMatchingScaleMinY = m_dEditROIAnisoAlignMatchingScaleMinY;
	pAlgorithmParam->m_dROIAnisoAlignMatchingScaleMaxY = m_dEditROIAnisoAlignMatchingScaleMaxY;
	pAlgorithmParam->m_dROIAnisoAlignMatchingScaleMinX = m_dEditROIAnisoAlignMatchingScaleMinX;
	pAlgorithmParam->m_dROIAnisoAlignMatchingScaleMaxX = m_dEditROIAnisoAlignMatchingScaleMaxX;
	pAlgorithmParam->m_iROIAnisoAlignMatchingMinContrast = m_iEditROIAnisoAlignMatchingMinContrast;
	pAlgorithmParam->m_bROIAnisoAlignUsePosX = m_bCheckROIAnisoAlignUsePosX;
	pAlgorithmParam->m_bROIAnisoAlignUsePosY = m_bCheckROIAnisoAlignUsePosY;
	pAlgorithmParam->m_bROIAnisoAlignInspectShift = m_bCheckROIAnisoAlignInspectShift;
	pAlgorithmParam->m_iROIAnisoAlignMatchingPLevel = m_iEditROIAnisoAlignMatchingPLevel;
	pAlgorithmParam->m_dROIAnisoAlignShiftTop = m_dEditROIAnisoAlignShiftTop;
	pAlgorithmParam->m_dROIAnisoAlignShiftBottom = m_dEditROIAnisoAlignShiftBottom;
	pAlgorithmParam->m_dROIAnisoAlignShiftLeft = m_dEditROIAnisoAlignShiftLeft;
	pAlgorithmParam->m_dROIAnisoAlignShiftRight = m_dEditROIAnisoAlignShiftRight;
	pAlgorithmParam->m_bROIAnisoAlignLocalAlignUse = m_bCheckROIAnisoAlignLocalAlignUse;
	pAlgorithmParam->m_dROIAnisoAlignMatchingScore = m_dEditROIAnisoAlignMatchingScore;
	pAlgorithmParam->m_bUseAnisoColorImage = m_bCheckROIColorImage;
	pAlgorithmParam->m_bROIAnisoAlignMultiModuleInspect = m_bCheckROIAlignMultiModuleInspect;
	pAlgorithmParam->m_iROIAnisoAlignMultiModuleXNumber = m_iEditROIAlignMultiModuleXNumber;
	pAlgorithmParam->m_iROIAnisoAlignMultiModuleYNumber = m_iEditROIAlignMultiModuleYNumber;
	pAlgorithmParam->m_iROIAnisoAlignMultiModuleXPitch = m_iEditROIAlignMultiModuleXPitch;
	pAlgorithmParam->m_iROIAnisoAlignMultiModuleYPitch = m_iEditROIAlignMultiModuleYPitch;
	pAlgorithmParam->m_iROIAnisoAlignMultiModuleAlignTarget = m_iRadioROIAnisoAlignMultiModuleAlignTarget;
	pAlgorithmParam->m_iROIAnisoAlignMultiModuleVision = m_iRadioROIAnisoAlignMultiModuleVision;
}

BOOL CAlgorithmROIAnisoAlignDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmROIAnisoAlignDlg::OnBnClickedButtonTestMatchingModel()
{
	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		HObject HInspectAreaRgn;
		HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		HTuple dAngleRangeRad;
		TupleRad((double)m_iEditROIAnisoAlignMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		HTuple lROIAnisoAlignModelID;

		HObject HInspectAreaXLD;
		GenContourRegionXld(HInspectAreaRgn, &HInspectAreaXLD, "border_holes");

		lROIAnisoAlignModelID = -1;

		if (m_bCheckROIColorImage)
		{
			int iColorMinContrast = (int)(sqrt(3.0) * (double)m_iEditROIAnisoAlignMatchingMinContrast);
			CreateAnisoShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto",
				m_dEditROIAnisoAlignMatchingScaleMinY, m_dEditROIAnisoAlignMatchingScaleMaxY, "auto",
				m_dEditROIAnisoAlignMatchingScaleMinX, m_dEditROIAnisoAlignMatchingScaleMaxX, "auto",
				"auto", "ignore_color_polarity", iColorMinContrast, &lROIAnisoAlignModelID);
		}
		else
			CreateAnisoShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto",
				m_dEditROIAnisoAlignMatchingScaleMinY, m_dEditROIAnisoAlignMatchingScaleMaxY, "auto",
				m_dEditROIAnisoAlignMatchingScaleMinX, m_dEditROIAnisoAlignMatchingScaleMaxX, "auto",
				"auto", "ignore_local_polarity", m_iEditROIAnisoAlignMatchingMinContrast, &lROIAnisoAlignModelID);

		if (lROIAnisoAlignModelID < 0)
		{
			AfxMessageBox("모델 생성 실패.", MB_SYSTEMMODAL);
			return;
		}

		Hlong lROIArea;
		double dROICenterX, dROICenterY;
		HTuple HlROIArea, HdROICenterX, HdROICenterY;
		HObject HInspectAreaRectRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaRectRgn, "rectangle1");
		AreaCenter(HInspectAreaRectRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
		HlROIArea = HlROIArea.L();
		dROICenterY = HdROICenterY.D();
		dROICenterX = HdROICenterX.D();

		int iSearchMarginX = m_iEditROIAnisoAlignXSearchMargin;
		if (iSearchMarginX < 0)
			iSearchMarginX = 0;
		int iSearchMarginY = m_iEditROIAnisoAlignYSearchMargin;
		if (iSearchMarginY < 0)
			iSearchMarginY = 0;

		HObject HInspectAreaDilatedRgn;
		DilationRectangle1(HInspectAreaRgn, &HInspectAreaDilatedRgn, iSearchMarginX * 2 + 1, iSearchMarginY * 2 + 1);

		HObject HMatchingImageReduced;
		if (m_bCheckROIColorImage)
			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][m_iSelectedImageType], HInspectAreaDilatedRgn, &HMatchingImageReduced);
		else
			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][m_iSelectedImageType], HInspectAreaDilatedRgn, &HMatchingImageReduced);

		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, ScaleX, ScaleY, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;

		double dTransX, dTransY;
		HObject HModelContour;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		ScaleX = HTuple(0.0);
		ScaleY = HTuple(0.0);
		Score = HTuple(0.0);

		int iNoTeachNumLevel = 0;
		HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

		GetShapeModelParams(lROIAnisoAlignModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

		iNoTeachNumLevel = NumLevels[0].L();
		if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_SHAPE)
			iNoTeachNumLevel = MAX_PYRAMID_LEVEL_SHAPE;
		else
			iNoTeachNumLevel = m_iEditROIAnisoAlignMatchingPLevel;

		FindAnisoShapeModel(HMatchingImageReduced,
			lROIAnisoAlignModelID,							// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,
			m_dEditROIAnisoAlignMatchingScaleMinY,
			m_dEditROIAnisoAlignMatchingScaleMaxY,
			m_dEditROIAnisoAlignMatchingScaleMinX,
			m_dEditROIAnisoAlignMatchingScaleMaxX,
			m_dEditROIAnisoAlignMatchingScore,				// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
			iNoTeachNumLevel,							// Number of pyramid levels used in the matching
			0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&ScaleY,
			&ScaleX,
			&Score);									// Score of the found instances of the model.

		HTuple HlNoFoundNumber;
		TupleLength(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();

		if (lNoFoundNumber <= 0)
		{
			AfxMessageBox("실패: 경계 Edge 검출 파라미터를 변경 후 재시도 하십시요.", MB_SYSTEMMODAL);
			return;
		}

		double dDeltaX, dDeltaY, dAngle, dScaleY, dScaleX, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();
		dScaleY = ScaleY[0].D();
		dScaleX = ScaleX[0].D();
		double dAbs;
		HObject HAlignRgn;
		HObject HResultXLD;
		long lMoveX, lMoveY;

		//if (m_iEditROIAnisoAlignMatchingAngleRange==0 && m_dEditROIAnisoAlignMatchingScaleMinY==1 && m_dEditROIAnisoAlignMatchingScaleMaxY==1&& m_dEditROIAnisoAlignMatchingScaleMinX==1 && m_dEditROIAnisoAlignMatchingScaleMaxX==1)
		//{
		//	if (m_bCheckROIAnisoAlignUsePosX)
		//	{
		//		dTransX = dDeltaX - dROICenterX;
		//		dAbs = fabs(dTransX);
		//		if (dAbs > (double)iSearchMarginX)
		//			dTransX = 0;
		//	}
		//	else
		//	{
		//		dTransX = 0;
		//	}

		//	if (m_bCheckROIAnisoAlignUsePosY)
		//	{
		//		dTransY = dDeltaY - dROICenterY;
		//		dAbs = fabs(dTransY);
		//		if (dAbs > (double)iSearchMarginY)
		//			dTransY = 0;
		//	}
		//	else
		//	{
		//		dTransY = 0;
		//	}

		//	lMoveX = (long)(dTransX+0.5);
		//	lMoveY = (long)(dTransY+0.5);

		//	MoveRegion(HInspectAreaRgn, &HAlignRgn, lMoveY, lMoveX);
		//	GenContourRegionXld(HAlignRgn, &HResultXLD, "border_holes");
		//}
		//else
		//{
		HomMat2dIdentity(&HomMat2DIdentity); // 확인 필요..CHL
		HomMat2dTranslate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
		HomMat2dRotate(HomMat2DTranslate, dAngle, dDeltaY, dDeltaX, &HomMat2DRotate);
		HomMat2dScale(HomMat2DRotate, dScaleY, dScaleX, dDeltaY, dDeltaX, &HomMat2DScale);

		GetShapeModelContours(&HModelContour, lROIAnisoAlignModelID, 1);
		AffineTransContourXld(HModelContour, &HResultXLD, HomMat2DScale);
		//		}

		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
			ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		ClearShapeModel(lROIAnisoAlignModelID);

		// For Message
		dTransX = dDeltaX - dROICenterX;
		dTransY = dDeltaY - dROICenterY;
		lMoveX = (long)(dTransX + 0.5);
		lMoveY = (long)(dTransY + 0.5);

		double dRotationAngle;
		TupleDeg(Angle, &Angle);
		dRotationAngle = Angle[0].D();

		CString sMsg;
		sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), 각도편차(Deg)=(%.3lf), (X,Y)Scale=(%.2lf,%.2lf), 매칭율(%%)=%d", lMoveX, lMoveY, dRotationAngle, dScaleX, dScaleY, (int)(dScore*100.0));
		AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);

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

		strLog.Format("Halcon Exception [AlgorithmROIAnisoAlignDlg::OnBnClickedButtonTestMatchingModel] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmROIAnisoAlignDlg::OnBnClickedButtonCreateMatchingModel()
{
	UpdateData();

	if (m_pSelectedROI->m_HROIAlignModelID >= 0)
	{
		ClearShapeModel(m_pSelectedROI->m_HROIAlignModelID);
		m_pSelectedROI->m_HROIAlignModelID = -1;
	}

	HObject HInspectAreaRgn;
	HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

	HTuple dAngleRangeRad;
	TupleRad((double)m_iEditROIAnisoAlignMatchingAngleRange, &dAngleRangeRad);

	if (dAngleRangeRad < 0)
		dAngleRangeRad *= -1.0;

	HObject HInspectAreaXLD;
	GenContourRegionXld(HInspectAreaRgn, &HInspectAreaXLD, "border_holes");

	m_pSelectedROI->m_HROIAlignModelID = -1;

	if (m_bCheckROIColorImage)
	{
		int iColorMinContrast = (int)(sqrt(3.0) * (double)m_iEditROIAnisoAlignMatchingMinContrast);

		CreateAnisoShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditROIAnisoAlignMatchingScaleMinY, m_dEditROIAnisoAlignMatchingScaleMaxY, "auto", m_dEditROIAnisoAlignMatchingScaleMinX, m_dEditROIAnisoAlignMatchingScaleMaxX, "auto", "auto", "ignore_color_polarity", iColorMinContrast, &(m_pSelectedROI->m_HROIAlignModelID));
	}
	else
		CreateAnisoShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditROIAnisoAlignMatchingScaleMinY, m_dEditROIAnisoAlignMatchingScaleMaxY, "auto", m_dEditROIAnisoAlignMatchingScaleMinX, m_dEditROIAnisoAlignMatchingScaleMaxX, "auto", "auto", "ignore_local_polarity", m_iEditROIAnisoAlignMatchingMinContrast, &(m_pSelectedROI->m_HROIAlignModelID));

	if (m_pSelectedROI->m_HROIAlignModelID < 0)
		AfxMessageBox("모델 생성 실패.", MB_SYSTEMMODAL);
	else
		AfxMessageBox("모델 생성 성공.", MB_SYSTEMMODAL);
}


void CAlgorithmROIAnisoAlignDlg::OnBnClickedButtonTestMatchingMultiModule()
{
	try
	{
		int i, j, k;
		DWORD dTStart = 0, dTEnd = 0;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		HObject HInspectAreaRgn;
		HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		HTuple dAngleRangeRad;
		TupleRad((double)m_iEditROIAnisoAlignMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		HTuple lROIAnisoAlignModelID;

		HObject HInspectAreaXLD;
		GenContourRegionXld(HInspectAreaRgn, &HInspectAreaXLD, "border_holes");

		lROIAnisoAlignModelID = -1;

		CreateAnisoShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto",
			m_dEditROIAnisoAlignMatchingScaleMinY, m_dEditROIAnisoAlignMatchingScaleMaxY, "auto",
			m_dEditROIAnisoAlignMatchingScaleMinX, m_dEditROIAnisoAlignMatchingScaleMaxX, "auto",
			"auto", "ignore_local_polarity", m_iEditROIAnisoAlignMatchingMinContrast, &lROIAnisoAlignModelID);

		if (lROIAnisoAlignModelID < 0)
		{
			AfxMessageBox("모델 생성 실패.", MB_SYSTEMMODAL);
			return;
		}

		Hlong lROIArea;
		double dROICenterX, dROICenterY;
		HTuple HlROIArea, HdROICenterX, HdROICenterY;
		HObject HInspectAreaRectRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaRectRgn, "rectangle1");
		AreaCenter(HInspectAreaRectRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
		HlROIArea = HlROIArea.L();
		dROICenterY = HdROICenterY.D();
		dROICenterX = HdROICenterX.D();

		int iSearchMarginX = m_iEditROIAnisoAlignXSearchMargin;
		if (iSearchMarginX < 0)
			iSearchMarginX = 0;
		int iSearchMarginY = m_iEditROIAnisoAlignYSearchMargin;
		if (iSearchMarginY < 0)
			iSearchMarginY = 0;

		HObject HInspectAreaDilatedRgn, HModuleSearchRgn;
		DilationRectangle1(HInspectAreaRgn, &HInspectAreaDilatedRgn, iSearchMarginX * 2 + 1, iSearchMarginY * 2 + 1);

		HObject HMatchingImageReduced;
		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, ScaleX, ScaleY, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;
		Hlong iMoveX, iMoveY;

		double dTransX, dTransY;
		double dDeltaX, dDeltaY, dAngle, dScaleY, dScaleX, dScore;
		Hlong lCropLTPointY, lCropLTPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

		HObject HModelContour, HModelAffineXLD;
		GetShapeModelContours(&HModelContour, lROIAnisoAlignModelID, 1);

		int iNoTeachNumLevel = 0;
		HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

		GetShapeModelParams(lROIAnisoAlignModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

		iNoTeachNumLevel = NumLevels[0].L();
		if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_SHAPE)
			iNoTeachNumLevel = MAX_PYRAMID_LEVEL_SHAPE;
		else
			iNoTeachNumLevel = m_iEditROIAnisoAlignMatchingPLevel;

		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		int iMatchCount = 0;

		dTStart = GetTickCount();

		for (i = 0; i < m_iEditROIAlignMultiModuleYNumber; i++)
		{
			for (j = 0; j < m_iEditROIAlignMultiModuleXNumber; j++)
			{
				iMoveX = j * m_iEditROIAlignMultiModuleXPitch;
				iMoveY = i * m_iEditROIAlignMultiModuleYPitch;

				MoveRegion(HInspectAreaDilatedRgn, &HModuleSearchRgn, iMoveY, iMoveX);

				ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][m_iSelectedImageType], HModuleSearchRgn, &HMatchingImageReduced);

				if (m_iRadioROIAnisoAlignMultiModuleVision == MULTI_MODULE_VISION_ALIGN)
				{
					CropDomain(HMatchingImageReduced, &HMatchingImageReduced);
					SmallestRectangle1(HModuleSearchRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					lCropLTPointY = HlLTPointY.L();
					lCropLTPointX = HlLTPointX.L();
				}

				Row = HTuple(0.0);
				Column = HTuple(0.0);
				Angle = HTuple(0.0);
				ScaleX = HTuple(0.0);
				ScaleY = HTuple(0.0);
				Score = HTuple(0.0);

				FindAnisoShapeModel(HMatchingImageReduced,
					lROIAnisoAlignModelID,							// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,
					m_dEditROIAnisoAlignMatchingScaleMinY,
					m_dEditROIAnisoAlignMatchingScaleMaxY,
					m_dEditROIAnisoAlignMatchingScaleMinX,
					m_dEditROIAnisoAlignMatchingScaleMaxX,
					m_dEditROIAnisoAlignMatchingScore,				// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&ScaleY,
					&ScaleX,
					&Score);									// Score of the found instances of the model.

				++iMatchCount;

				HTuple HlNoFoundNumber;
				TupleLength(Score, &HlNoFoundNumber);
				lNoFoundNumber = HlNoFoundNumber.L();

				if (lNoFoundNumber <= 0)
				{
					continue;
				}

				dDeltaX = Column[0].D();
				dDeltaY = Row[0].D();
				dAngle = Angle[0].D();
				dScore = Score[0].D();
				dScaleY = ScaleY[0].D();
				dScaleX = ScaleX[0].D();

				if (m_iRadioROIAnisoAlignMultiModuleVision == MULTI_MODULE_VISION_ALIGN)
				{
					dDeltaX = dDeltaX + lCropLTPointX;
					dDeltaY = dDeltaY + lCropLTPointY;
				}

				HomMat2dIdentity(&HomMat2DIdentity);
				HomMat2dTranslate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
				HomMat2dRotate(HomMat2DTranslate, dAngle, dDeltaY, dDeltaX, &HomMat2DRotate);
				HomMat2dScale(HomMat2DRotate, dScaleY, dScaleX, dDeltaY, dDeltaX, &HomMat2DScale);

				AffineTransContourXld(HModelContour, &HModelAffineXLD, HomMat2DScale);

				if (THEAPP.m_pGFunction->ValidHXLD(HModelAffineXLD))
					ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HModelAffineXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
			}
		}

		dTEnd = GetTickCount();

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		ClearShapeModel(lROIAnisoAlignModelID);

		CString sMsg;
		sMsg.Format("매칭 성공 %d 회, 처리시간 %d ms", iMatchCount, dTEnd - dTStart);
		AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);

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

		strLog.Format("Halcon Exception [AlgorithmROIAnisoAlignDlg::OnBnClickedButtonTestMatchingMultiModule] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmROIAnisoAlignDlg::OnBnClickedCheckRoiAlignLocalAlignUse()
{
	UpdateData(TRUE);

	if (m_bCheckROIAnisoAlignLocalAlignUse)
	{
		m_bCheckROIAlignMultiModuleInspect = FALSE;
		UpdateData(FALSE);
	}
}


void CAlgorithmROIAnisoAlignDlg::OnBnClickedCheckRoiAnisoAlignMultiModuleInspect()
{
	UpdateData(TRUE);

	if (m_bCheckROIAlignMultiModuleInspect)
	{
		if (m_bCheckROIColorImage)
		{
			m_bCheckROIColorImage = FALSE;
			UpdateData(FALSE);

			AfxMessageBox("[Color 영상 매칭 적용] 체크 해제. 멀티 모듈 검사에서는 Color 매칭 미적용", MB_SYSTEMMODAL | MB_ICONINFORMATION);
		}
	}
}
