// AlgorithmROIAlignDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmROIAlignDlg.h"
#include "afxdialogex.h"


// CAlgorithmROIAlignDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmROIAlignDlg, CDialog)

CAlgorithmROIAlignDlg::CAlgorithmROIAlignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmROIAlignDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseROIAlign = FALSE;
	m_iEditROIAlignXSearchMargin = 0;
	m_iEditROIAlignYSearchMargin = 0;
	m_iEditROIAlignMatchingAngleRange = 0;
	m_dEditROIAlignMatchingScaleMin = 1.0;
	m_dEditROIAlignMatchingScaleMax = 1.0;
	m_iEditROIAlignMatchingMinContrast = 0;
	m_dEditROIAlignShiftTop = 0.0;
	m_dEditROIAlignShiftBottom = 0.0;
	m_dEditROIAlignShiftLeft = 0.0;
	m_dEditROIAlignShiftRight = 0.0;
	m_bCheckROIAlignLocalAlignUse = FALSE;
	m_dEditROIAlignMatchingScore = 0.0;
	m_bCheckROIColorImage = FALSE;

	m_bCheckROIAlignMultiModuleInspect = FALSE;
	m_iEditROIAlignMultiModuleXNumber = 1;
	m_iEditROIAlignMultiModuleYNumber = 1;
	m_iEditROIAlignMultiModuleXPitch = 1000;
	m_iEditROIAlignMultiModuleYPitch = 1000;

	m_iRadioROIAlignMultiModuleAlignTarget = 0;
	m_iRadioROIAlignMultiModuleVision = 1;
}

CAlgorithmROIAlignDlg::~CAlgorithmROIAlignDlg()
{
}

void CAlgorithmROIAlignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_ROI_ALIGN, m_bCheckUseROIAlign);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_X_SEARCH_MARGIN, m_iEditROIAlignXSearchMargin);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_Y_SEARCH_MARGIN, m_iEditROIAlignYSearchMargin);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_ANGLE_RANGE, m_iEditROIAlignMatchingAngleRange);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCALE_MIN, m_dEditROIAlignMatchingScaleMin);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCALE_MAX, m_dEditROIAlignMatchingScaleMax);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_MIN_CONTRAST, m_iEditROIAlignMatchingMinContrast);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_USE_POS_X, m_bCheckROIAlignUsePosX);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_USE_POS_Y, m_bCheckROIAlignUsePosY);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_INSPECT_SHIFT, m_bCheckROIAlignInspectShift);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_PLEVEL, m_iEditROIAlignMatchingPLevel);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_TOP, m_dEditROIAlignShiftTop);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_BOTTOM, m_dEditROIAlignShiftBottom);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_LEFT, m_dEditROIAlignShiftLeft);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_RIGHT, m_dEditROIAlignShiftRight);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_LOCAL_ALIGN_USE, m_bCheckROIAlignLocalAlignUse);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCORE, m_dEditROIAlignMatchingScore);
	DDX_Check(pDX, IDC_CHECK_USE_COLOR_IMAGE, m_bCheckROIColorImage);

	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_MULTI_MODULE_INSPECT, m_bCheckROIAlignMultiModuleInspect);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MULTI_MODULE_X_NUMBER, m_iEditROIAlignMultiModuleXNumber);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MULTI_MODULE_Y_NUMBER, m_iEditROIAlignMultiModuleYNumber);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MULTI_MODULE_X_PITCH, m_iEditROIAlignMultiModuleXPitch);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MULTI_MODULE_Y_PITCH, m_iEditROIAlignMultiModuleYPitch);

	DDX_Radio(pDX, IDC_RADIO_ROI_ALIGN_MULTI_MODULE_MATCHING, m_iRadioROIAlignMultiModuleAlignTarget);
	DDX_Radio(pDX, IDC_RADIO_ROI_ALIGN_MULTI_MODULE_TOP1, m_iRadioROIAlignMultiModuleVision);
}

BEGIN_MESSAGE_MAP(CAlgorithmROIAlignDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TEST_MATCHING_MODEL, &CAlgorithmROIAlignDlg::OnBnClickedButtonTestMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_MATCHING_MODEL, &CAlgorithmROIAlignDlg::OnBnClickedButtonCreateMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_TEST_MATCHING_MULTI_MODULE, &CAlgorithmROIAlignDlg::OnBnClickedButtonTestMatchingMultiModule)
	ON_BN_CLICKED(IDC_CHECK_ROI_ALIGN_LOCAL_ALIGN_USE, &CAlgorithmROIAlignDlg::OnBnClickedCheckRoiAlignLocalAlignUse)
	ON_BN_CLICKED(IDC_CHECK_ROI_ALIGN_MULTI_MODULE_INSPECT, &CAlgorithmROIAlignDlg::OnBnClickedCheckRoiAlignMultiModuleInspect)
END_MESSAGE_MAP()


// CAlgorithmROIAlignDlg 메시지 처리기입니다.

void CAlgorithmROIAlignDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
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

void CAlgorithmROIAlignDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseROIAlign = AlgorithmParam.m_bUseROIAlign;
	m_iEditROIAlignXSearchMargin = AlgorithmParam.m_iROIAlignXSearchMargin;
	m_iEditROIAlignYSearchMargin = AlgorithmParam.m_iROIAlignYSearchMargin;
	m_iEditROIAlignMatchingAngleRange = AlgorithmParam.m_iROIAlignMatchingAngleRange;
	m_dEditROIAlignMatchingScaleMin = AlgorithmParam.m_dROIAlignMatchingScaleMin;
	m_dEditROIAlignMatchingScaleMax = AlgorithmParam.m_dROIAlignMatchingScaleMax;
	m_iEditROIAlignMatchingMinContrast = AlgorithmParam.m_iROIAlignMatchingMinContrast;
	m_bCheckROIAlignUsePosX = AlgorithmParam.m_bROIAlignUsePosX;
	m_bCheckROIAlignUsePosY = AlgorithmParam.m_bROIAlignUsePosY;
	m_bCheckROIAlignInspectShift = AlgorithmParam.m_bROIAlignInspectShift;
	m_iEditROIAlignMatchingPLevel = AlgorithmParam.m_iROIAlignMatchingPLevel;
	m_dEditROIAlignShiftTop = AlgorithmParam.m_dROIAlignShiftTop;
	m_dEditROIAlignShiftBottom = AlgorithmParam.m_dROIAlignShiftBottom;
	m_dEditROIAlignShiftLeft = AlgorithmParam.m_dROIAlignShiftLeft;
	m_dEditROIAlignShiftRight = AlgorithmParam.m_dROIAlignShiftRight;
	m_bCheckROIAlignLocalAlignUse = AlgorithmParam.m_bROIAlignLocalAlignUse;
	m_dEditROIAlignMatchingScore = AlgorithmParam.m_dROIAlignMatchingScore;
	m_bCheckROIColorImage = AlgorithmParam.m_bUseIsoColorImage;
	m_bCheckROIAlignMultiModuleInspect = AlgorithmParam.m_bROIAlignMultiModuleInspect;
	m_iEditROIAlignMultiModuleXNumber = AlgorithmParam.m_iROIAlignMultiModuleXNumber;
	m_iEditROIAlignMultiModuleYNumber = AlgorithmParam.m_iROIAlignMultiModuleYNumber;
	m_iEditROIAlignMultiModuleXPitch = AlgorithmParam.m_iROIAlignMultiModuleXPitch;
	m_iEditROIAlignMultiModuleYPitch = AlgorithmParam.m_iROIAlignMultiModuleYPitch;
	m_iRadioROIAlignMultiModuleAlignTarget = AlgorithmParam.m_iROIAlignMultiModuleAlignTarget;
	m_iRadioROIAlignMultiModuleVision = AlgorithmParam.m_iROIAlignMultiModuleVision;

	UpdateData(FALSE);
}

void CAlgorithmROIAlignDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseROIAlign = m_bCheckUseROIAlign;
	pAlgorithmParam->m_iROIAlignXSearchMargin = m_iEditROIAlignXSearchMargin;
	pAlgorithmParam->m_iROIAlignYSearchMargin = m_iEditROIAlignYSearchMargin;
	pAlgorithmParam->m_iROIAlignMatchingAngleRange = m_iEditROIAlignMatchingAngleRange;
	pAlgorithmParam->m_dROIAlignMatchingScaleMin = m_dEditROIAlignMatchingScaleMin;
	pAlgorithmParam->m_dROIAlignMatchingScaleMax = m_dEditROIAlignMatchingScaleMax;
	pAlgorithmParam->m_iROIAlignMatchingMinContrast = m_iEditROIAlignMatchingMinContrast;
	pAlgorithmParam->m_bROIAlignUsePosX = m_bCheckROIAlignUsePosX;
	pAlgorithmParam->m_bROIAlignUsePosY = m_bCheckROIAlignUsePosY;
	pAlgorithmParam->m_bROIAlignInspectShift = m_bCheckROIAlignInspectShift;
	pAlgorithmParam->m_iROIAlignMatchingPLevel = m_iEditROIAlignMatchingPLevel;
	pAlgorithmParam->m_dROIAlignShiftTop = m_dEditROIAlignShiftTop;
	pAlgorithmParam->m_dROIAlignShiftBottom = m_dEditROIAlignShiftBottom;
	pAlgorithmParam->m_dROIAlignShiftLeft = m_dEditROIAlignShiftLeft;
	pAlgorithmParam->m_dROIAlignShiftRight = m_dEditROIAlignShiftRight;
	pAlgorithmParam->m_bROIAlignLocalAlignUse = m_bCheckROIAlignLocalAlignUse;
	pAlgorithmParam->m_dROIAlignMatchingScore = m_dEditROIAlignMatchingScore;
	pAlgorithmParam->m_bUseIsoColorImage = m_bCheckROIColorImage;
	pAlgorithmParam->m_bROIAlignMultiModuleInspect = m_bCheckROIAlignMultiModuleInspect;
	pAlgorithmParam->m_iROIAlignMultiModuleXNumber = m_iEditROIAlignMultiModuleXNumber;
	pAlgorithmParam->m_iROIAlignMultiModuleYNumber = m_iEditROIAlignMultiModuleYNumber;
	pAlgorithmParam->m_iROIAlignMultiModuleXPitch = m_iEditROIAlignMultiModuleXPitch;
	pAlgorithmParam->m_iROIAlignMultiModuleYPitch = m_iEditROIAlignMultiModuleYPitch;
	pAlgorithmParam->m_iROIAlignMultiModuleAlignTarget = m_iRadioROIAlignMultiModuleAlignTarget;
	pAlgorithmParam->m_iROIAlignMultiModuleVision = m_iRadioROIAlignMultiModuleVision;
}

BOOL CAlgorithmROIAlignDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmROIAlignDlg::OnBnClickedButtonTestMatchingModel()
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
		TupleRad((double)m_iEditROIAlignMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		HTuple lROIAlignModelID;

		HObject HInspectAreaXLD;
		GenContourRegionXld(HInspectAreaRgn, &HInspectAreaXLD, "border_holes");

		lROIAlignModelID = -1;

		if (m_bCheckROIColorImage)
		{
			int iColorMinContrast = (int)(sqrt(3.0) * (double)m_iEditROIAlignMatchingMinContrast);
			CreateScaledShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditROIAlignMatchingScaleMin, m_dEditROIAlignMatchingScaleMax, "auto", "auto", "ignore_color_polarity", iColorMinContrast, &lROIAlignModelID);
		}
		else
			CreateScaledShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditROIAlignMatchingScaleMin, m_dEditROIAlignMatchingScaleMax, "auto", "auto", "ignore_local_polarity", m_iEditROIAlignMatchingMinContrast, &lROIAlignModelID);

		if (lROIAlignModelID < 0)
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

		int iSearchMarginX = m_iEditROIAlignXSearchMargin;
		if (iSearchMarginX < 0)
			iSearchMarginX = 0;
		int iSearchMarginY = m_iEditROIAlignYSearchMargin;
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
		HTuple Row, Column, Angle, Scale, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;

		double dTransX, dTransY;
		HObject HModelContour;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Scale = HTuple(0.0);
		Score = HTuple(0.0);

		int iNoTeachNumLevel = 0;
		HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

		GetShapeModelParams(lROIAlignModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

		iNoTeachNumLevel = NumLevels[0].L();
		if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_SHAPE)
			iNoTeachNumLevel = MAX_PYRAMID_LEVEL_SHAPE;
		else
			iNoTeachNumLevel = m_iEditROIAlignMatchingPLevel;

		FindScaledShapeModel(HMatchingImageReduced,
			lROIAlignModelID,							// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,							// Extent of the rotation angles.
			m_dEditROIAlignMatchingScaleMin,
			m_dEditROIAlignMatchingScaleMax,
			m_dEditROIAlignMatchingScore,				// Minumum score of the instances of the model to be found
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

		HTuple HlNoFoundNumber;
		TupleLength(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();

		if (lNoFoundNumber <= 0)
		{
			AfxMessageBox("실패: 경계 Edge 검출 파라미터를 변경 후 재시도 하십시요.", MB_SYSTEMMODAL);
			return;
		}

		double dDeltaX, dDeltaY, dAngle, dScale, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();
		dScale = Scale[0].D();

		double dAbs;
		HObject HAlignRgn;
		HObject HResultXLD;
		long lMoveX, lMoveY;

		//if (m_iEditROIAlignMatchingAngleRange==0 && m_dEditROIAlignMatchingScaleMin==1 && m_dEditROIAlignMatchingScaleMax==1)
		//{
		//	if (m_bCheckROIAlignUsePosX)
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

		//	if (m_bCheckROIAlignUsePosY)
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
		HomMat2dIdentity(&HomMat2DIdentity);
		HomMat2dTranslate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
		HomMat2dRotate(HomMat2DTranslate, dAngle, dDeltaY, dDeltaX, &HomMat2DRotate);
		HomMat2dScale(HomMat2DRotate, dScale, dScale, dDeltaY, dDeltaX, &HomMat2DScale);

		GetShapeModelContours(&HModelContour, lROIAlignModelID, 1);
		AffineTransContourXld(HModelContour, &HResultXLD, HomMat2DScale);
		//		}

		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
			ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		ClearShapeModel(lROIAlignModelID);

		// For Message
		dTransX = dDeltaX - dROICenterX;
		dTransY = dDeltaY - dROICenterY;
		lMoveX = (long)(dTransX + 0.5);
		lMoveY = (long)(dTransY + 0.5);

		double dRotationAngle;
		TupleDeg(Angle, &Angle);
		dRotationAngle = Angle[0].D();

		CString sMsg;
		sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), 각도편차(Deg)=(%.3lf), Scale=(%.2lf), 매칭율(%%)=%d", lMoveX, lMoveY, dRotationAngle, dScale, (int)(dScore*100.0));
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

		strLog.Format("Halcon Exception [AlgorithmROIAlignDlg::OnBnClickedButtonTestMatchingModel] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmROIAlignDlg::OnBnClickedButtonCreateMatchingModel()
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
	TupleRad((double)m_iEditROIAlignMatchingAngleRange, &dAngleRangeRad);

	if (dAngleRangeRad < 0)
		dAngleRangeRad *= -1.0;

	HObject HInspectAreaXLD;
	GenContourRegionXld(HInspectAreaRgn, &HInspectAreaXLD, "border_holes");

	m_pSelectedROI->m_HROIAlignModelID = -1;

	if (m_bCheckROIColorImage)
	{
		int iColorMinContrast = (int)(sqrt(3.0) * (double)m_iEditROIAlignMatchingMinContrast);
		CreateScaledShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditROIAlignMatchingScaleMin, m_dEditROIAlignMatchingScaleMax, "auto", "auto", "ignore_color_polarity", iColorMinContrast, &(m_pSelectedROI->m_HROIAlignModelID));
	}
	else
		CreateScaledShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditROIAlignMatchingScaleMin, m_dEditROIAlignMatchingScaleMax, "auto", "auto", "ignore_local_polarity", m_iEditROIAlignMatchingMinContrast, &(m_pSelectedROI->m_HROIAlignModelID));

	if (m_pSelectedROI->m_HROIAlignModelID < 0)
		AfxMessageBox("모델 생성 실패.", MB_SYSTEMMODAL);
	else
		AfxMessageBox("모델 생성 성공.", MB_SYSTEMMODAL);
}

void CAlgorithmROIAlignDlg::OnBnClickedButtonTestMatchingMultiModule()
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
		TupleRad((double)m_iEditROIAlignMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		HTuple lROIAlignModelID;

		HObject HInspectAreaXLD;
		GenContourRegionXld(HInspectAreaRgn, &HInspectAreaXLD, "border_holes");

		lROIAlignModelID = -1;

		CreateScaledShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditROIAlignMatchingScaleMin, m_dEditROIAlignMatchingScaleMax, "auto", "auto", "ignore_local_polarity", m_iEditROIAlignMatchingMinContrast, &lROIAlignModelID);

		if (lROIAlignModelID == -1)
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

		int iSearchMarginX = m_iEditROIAlignXSearchMargin;
		if (iSearchMarginX < 0)
			iSearchMarginX = 0;
		int iSearchMarginY = m_iEditROIAlignYSearchMargin;
		if (iSearchMarginY < 0)
			iSearchMarginY = 0;

		HObject HInspectAreaDilatedRgn, HModuleSearchRgn;
		DilationRectangle1(HInspectAreaRgn, &HInspectAreaDilatedRgn, iSearchMarginX * 2 + 1, iSearchMarginY * 2 + 1);

		HObject HMatchingImageReduced;
		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, Scale, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;
		Hlong iMoveX, iMoveY;

		double dTransX, dTransY;
		double dDeltaX, dDeltaY, dAngle, dScale, dScore;
		Hlong lCropLTPointY, lCropLTPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

		HObject HModelContour, HModelAffineXLD;
		GetShapeModelContours(&HModelContour, lROIAlignModelID, 1);

		int iNoTeachNumLevel = 0;
		HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

		GetShapeModelParams(lROIAlignModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

		iNoTeachNumLevel = NumLevels[0].L();
		if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_SHAPE)
			iNoTeachNumLevel = MAX_PYRAMID_LEVEL_SHAPE;
		else
			iNoTeachNumLevel = m_iEditROIAlignMatchingPLevel;

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

				if (m_iRadioROIAlignMultiModuleVision == MULTI_MODULE_VISION_ALIGN)
				{
					CropDomain(HMatchingImageReduced, &HMatchingImageReduced);
					SmallestRectangle1(HModuleSearchRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					lCropLTPointY = HlLTPointY.L();
					lCropLTPointX = HlLTPointX.L();
				}

				Row = HTuple(0.0);
				Column = HTuple(0.0);
				Angle = HTuple(0.0);
				Scale = HTuple(0.0);
				Score = HTuple(0.0);

				FindScaledShapeModel(HMatchingImageReduced,
					lROIAlignModelID,							// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,							// Extent of the rotation angles.
					m_dEditROIAlignMatchingScaleMin,
					m_dEditROIAlignMatchingScaleMax,
					m_dEditROIAlignMatchingScore,				// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"least_squares",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&Scale,
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
				dScale = Scale[0].D();

				if (m_iRadioROIAlignMultiModuleVision == MULTI_MODULE_VISION_ALIGN)
				{
					dDeltaX = dDeltaX + lCropLTPointX;
					dDeltaY = dDeltaY + lCropLTPointY;
				}

				HomMat2dIdentity(&HomMat2DIdentity);
				HomMat2dTranslate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
				HomMat2dRotate(HomMat2DTranslate, dAngle, dDeltaY, dDeltaX, &HomMat2DRotate);
				HomMat2dScale(HomMat2DRotate, dScale, dScale, dDeltaY, dDeltaX, &HomMat2DScale);

				AffineTransContourXld(HModelContour, &HModelAffineXLD, HomMat2DScale);

				if (THEAPP.m_pGFunction->ValidHXLD(HModelAffineXLD))
					ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HModelAffineXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
			}
		}

		dTEnd = GetTickCount();

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		ClearShapeModel(lROIAlignModelID);

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

		strLog.Format("Halcon Exception [AlgorithmROIAlignDlg::OnBnClickedButtonTestMatchingMultiModule] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmROIAlignDlg::OnBnClickedCheckRoiAlignLocalAlignUse()
{
	UpdateData(TRUE);

	if (m_bCheckROIAlignLocalAlignUse)
	{
		m_bCheckROIAlignMultiModuleInspect = FALSE;
		UpdateData(FALSE);
	}
}


void CAlgorithmROIAlignDlg::OnBnClickedCheckRoiAlignMultiModuleInspect()
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
