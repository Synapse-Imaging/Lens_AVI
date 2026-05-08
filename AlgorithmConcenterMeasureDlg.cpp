// AlgorithmConcenterMeasureDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmConcenterMeasureDlg.h"
#include "afxdialogex.h"


// CAlgorithmConcenterMeasureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmConcenterMeasureDlg, CDialog)

CAlgorithmConcenterMeasureDlg::CAlgorithmConcenterMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmConcenterMeasureDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseConcenter = FALSE;
	m_iEditConcenterInnerCircleDiameter = 100;
	m_dEditConcenterMatchingScaleMin = 1;
	m_dEditConcenterMatchingScaleMax = 1;
	m_iEditConcenterMatchingMinContrast = 15;
	m_iEditConcenterMatchingScore = 50;
	m_dEditConcenterCenterDiffSpecX = 20;
	m_dEditConcenterCenterDiffSpecY = 20;
}

CAlgorithmConcenterMeasureDlg::~CAlgorithmConcenterMeasureDlg()
{
}

void CAlgorithmConcenterMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_CONCENTER, m_bCheckUseConcenter);
	DDX_Text(pDX, IDC_EDIT_CONCENTER_INNER_CIRCLE_DIAMETER, m_iEditConcenterInnerCircleDiameter);
	DDX_Text(pDX, IDC_EDIT_CONCENTER_MATCHING_SCALE_MIN, m_dEditConcenterMatchingScaleMin);
	DDX_Text(pDX, IDC_EDIT_CONCENTER_MATCHING_SCALE_MAX, m_dEditConcenterMatchingScaleMax);
	DDX_Text(pDX, IDC_EDIT_CONCENTER_MATCHING_MIN_CONTRAST, m_iEditConcenterMatchingMinContrast);
	DDX_Text(pDX, IDC_EDIT_CONCENTER_MATCHING_MATCHING_SCORE, m_iEditConcenterMatchingScore);
	DDX_Text(pDX, IDC_EDIT_CONCENTER_CENTER_DIFF_SPEC_X, m_dEditConcenterCenterDiffSpecX);
	DDX_Text(pDX, IDC_EDIT_CONCENTER_CENTER_DIFF_SPEC_Y, m_dEditConcenterCenterDiffSpecY);
}


BEGIN_MESSAGE_MAP(CAlgorithmConcenterMeasureDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TEST_INNER_CIRCLE, &CAlgorithmConcenterMeasureDlg::OnBnClickedButtonTestInnerCircle)
END_MESSAGE_MAP()


// CAlgorithmConcenterMeasureDlg 메시지 처리기입니다.

void CAlgorithmConcenterMeasureDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmConcenterMeasureDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseConcenter = AlgorithmParam.m_bUseConcenter;
	m_iEditConcenterInnerCircleDiameter = AlgorithmParam.m_iConcenterInnerCircleDiameter;
	m_dEditConcenterMatchingScaleMin = AlgorithmParam.m_dConcenterMatchingScaleMin;
	m_dEditConcenterMatchingScaleMax = AlgorithmParam.m_dConcenterMatchingScaleMax;
	m_iEditConcenterMatchingMinContrast = AlgorithmParam.m_iConcenterMatchingMinContrast;
	m_iEditConcenterMatchingScore = AlgorithmParam.m_iConcenterMatchingScore;
	m_dEditConcenterCenterDiffSpecX = AlgorithmParam.m_dConcenterCenterDiffSpecX;
	m_dEditConcenterCenterDiffSpecY = AlgorithmParam.m_dConcenterCenterDiffSpecY;

	UpdateData(FALSE);
}

void CAlgorithmConcenterMeasureDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseConcenter = m_bCheckUseConcenter;
	pAlgorithmParam->m_iConcenterInnerCircleDiameter = m_iEditConcenterInnerCircleDiameter;
	pAlgorithmParam->m_dConcenterMatchingScaleMin = m_dEditConcenterMatchingScaleMin;
	pAlgorithmParam->m_dConcenterMatchingScaleMax = m_dEditConcenterMatchingScaleMax;
	pAlgorithmParam->m_iConcenterMatchingMinContrast = m_iEditConcenterMatchingMinContrast;
	pAlgorithmParam->m_iConcenterMatchingScore = m_iEditConcenterMatchingScore;
	pAlgorithmParam->m_dConcenterCenterDiffSpecX = m_dEditConcenterCenterDiffSpecX;
	pAlgorithmParam->m_dConcenterCenterDiffSpecY = m_dEditConcenterCenterDiffSpecY;
}

BOOL CAlgorithmConcenterMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmConcenterMeasureDlg::OnBnClickedButtonTestInnerCircle()
{
	try
	{
		UpdateData();

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		HObject HInspectAreaRgn;
		HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		Hlong lROIArea;
		double dROICenterX, dROICenterY;
		HTuple HlROIArea, HdROICenterX, HdROICenterY;
		HObject HInspectAreaRectRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaRectRgn, "rectangle1");
		AreaCenter(HInspectAreaRectRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
		HlROIArea = HlROIArea.L();
		dROICenterY = HdROICenterY.D();
		dROICenterX = HdROICenterX.D();

		HTuple HLeftPoint, HTopPoint, HRightPoint, HBottomPoint;
		SmallestRectangle1(HInspectAreaRectRgn, &HLeftPoint, &HTopPoint, &HRightPoint, &HBottomPoint);

		Hlong lLeftPoint, lTopPoint;
		lLeftPoint = HLeftPoint.L();
		lTopPoint = HTopPoint.L();

		HObject HInspectAreaXLD;
		GenCircleContourXld(&HInspectAreaXLD, HdROICenterY, HdROICenterX, HTuple((double)m_iEditConcenterInnerCircleDiameter*0.5), 0, 6.28318, "positive", 1.0);

		HTuple lROIAlignModelID;
		lROIAlignModelID = -1;

		CreateScaledShapeModelXld(HInspectAreaXLD, "auto", 0, 0, "auto", m_dEditConcenterMatchingScaleMin, m_dEditConcenterMatchingScaleMax, "auto", "auto", "ignore_local_polarity", m_iEditConcenterMatchingMinContrast, &lROIAlignModelID);

		if (lROIAlignModelID < 0)
		{
			AfxMessageBox("매칭 모델 생성 실패.", MB_SYSTEMMODAL);
			return;
		}

		int iSearchMarginX = 50;
		int iSearchMarginY = 50;

		lLeftPoint -= iSearchMarginX;
		lTopPoint -= iSearchMarginY;

		HObject HInspectAreaDilatedRgn;
		DilationRectangle1(HInspectAreaRectRgn, &HInspectAreaDilatedRgn, iSearchMarginX * 2 + 1, iSearchMarginY * 2 + 1);

		HObject HMatchingImageReduced;
		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HInspectAreaDilatedRgn, &HMatchingImageReduced);

		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, Scale, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DScale;

		double dTransX, dTransY;
		HObject HModelContour;

		double dMinMatchingScore;
		dMinMatchingScore = (double)m_iEditConcenterMatchingScore * 0.01;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Scale = HTuple(0.0);
		Score = HTuple(0.0);

		FindScaledShapeModel(HMatchingImageReduced,
			lROIAlignModelID,							// Handle of the model
			0,											// Smallest rotation of the model
			0,											// Extent of the rotation angles.
			m_dEditConcenterMatchingScaleMin,
			m_dEditConcenterMatchingScaleMax,
			dMinMatchingScore,							// Minumum score of the instances of the model to be found
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
			AfxMessageBox("원 매칭 실패.", MB_SYSTEMMODAL);
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

		HomMat2dIdentity(&HomMat2DIdentity);
		HomMat2dTranslate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
		HomMat2dScale(HomMat2DTranslate, dScale, dScale, dDeltaY, dDeltaX, &HomMat2DScale);

		GetShapeModelContours(&HModelContour, lROIAlignModelID, 1);
		AffineTransContourXld(HModelContour, &HResultXLD, HomMat2DScale);

		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
			ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		ClearShapeModel(lROIAlignModelID);

		// For Message
		dTransX = dDeltaX - dROICenterX;
		dTransY = dDeltaY - dROICenterY;

		CString sMsg;
		sMsg.Format("(X,Y) 원 중심편차(um)=(%.1lf,%.1lf), 매칭율(%%)=%d", dTransX*THEAPP.m_pCalDataService->GetPixelSize(), dTransY*THEAPP.m_pCalDataService->GetPixelSize(), (int)(dScore*100.0));
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

		strLog.Format("Halcon Exception [AlgorithmConcenterMeasureDlg::OnBnClickedButtonTestInnerCircle] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		CString sMsg;
		sMsg.Format("원 매칭 실패.");
		AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);

		return;
	}
}