// AlgorithmBoundaryDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmBoundaryDlg.h"
#include "afxdialogex.h"


// CAlgorithmBoundaryDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmBoundaryDlg, CDialog)

CAlgorithmBoundaryDlg::CAlgorithmBoundaryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmBoundaryDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseBoundaryInspection = FALSE;
	m_iRadioBoundaryEdgeMeasureDir = 0;
	m_iRadioBoundaryEdgeMeasureGv = 0;
	m_iRadioBoundaryEdgeMeasurePos = 0;
	m_dEditBoundaryEdgeMeasureSmFactor = 1.0;
	m_dEditBoundaryEdgeMeasureEdgeStr = 10;
	m_iEditBoundaryEdgeMeasureSampling = 1;
	m_iEditBoundaryEdgeMeasureSensorLength = 40;
	m_iRadioBoundaryDisplay = 0;

	for (int i = 0; i < 2; i++)
	{
		m_bCheckBoundaryCondition[i] = FALSE;
		m_iEditBoundaryMedianFilterSize[i] = 15;
		m_dEditBoundaryDefectLength[i] = 5;
		m_dEditBoundaryDefectMBLength[i] = 5;
		m_dEditBoundaryDefectWidth[i] = 1;
		m_bCheckBoundaryUseConnection[i] = TRUE;
		m_iEditBoundaryConnectionLength[i] = 1;
	}

	m_bCheckBoundaryCondition[2] = FALSE;
	m_dEditBoundaryDefectLength[2] = 15;
	m_dEditBoundaryDefectWidth[2] = 5;

	m_bCheckBoundaryUseRoiConnected = FALSE;
}

CAlgorithmBoundaryDlg::~CAlgorithmBoundaryDlg()
{
}

void CAlgorithmBoundaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_BOUNDARY_INSPECTION, m_bCheckUseBoundaryInspection);
	DDX_Radio(pDX, IDC_RADIO_BOUNDARY_EDGE_MEASURE_DIR_CW, m_iRadioBoundaryEdgeMeasureDir);
	DDX_Radio(pDX, IDC_RADIO_BOUNDARY_EDGE_MEASURE_GV_ANY, m_iRadioBoundaryEdgeMeasureGv);
	DDX_Radio(pDX, IDC_RADIO_BOUNDARY_EDGE_MEASURE_POS_BEST, m_iRadioBoundaryEdgeMeasurePos);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_EDGE_MEASURE_SMOOTHING_FACTOR, m_dEditBoundaryEdgeMeasureSmFactor);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_EDGE_MEASURE_EDGE_STRENGTH, m_dEditBoundaryEdgeMeasureEdgeStr);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_EDGE_MEASURE_SAMPLING, m_iEditBoundaryEdgeMeasureSampling);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_EDGE_MEASURE_SENSOR_LENGTH, m_iEditBoundaryEdgeMeasureSensorLength);

	DDX_Radio(pDX, IDC_RADIO_BOUNDARY_DISPLAY_DEFECT, m_iRadioBoundaryDisplay);

	DDX_Check(pDX, IDC_CHECK_BOUNDARY_USE_TYPE_1, m_bCheckBoundaryCondition[0]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_MEDIAN_FILTER_SIZE, m_iEditBoundaryMedianFilterSize[0]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_DEFECT_LENGTH, m_dEditBoundaryDefectLength[0]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_DEFECT_MB_LENGTH, m_dEditBoundaryDefectMBLength[0]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_DEFECT_WIDTH, m_dEditBoundaryDefectWidth[0]);
	DDX_Check(pDX, IDC_CHECK_BOUNDARY_USE_CONNECTION, m_bCheckBoundaryUseConnection[0]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_CONNECT_LENGTH, m_iEditBoundaryConnectionLength[0]);

	DDX_Check(pDX, IDC_CHECK_BOUNDARY_USE_TYPE_2, m_bCheckBoundaryCondition[1]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_MEDIAN_FILTER_SIZE2, m_iEditBoundaryMedianFilterSize[1]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_DEFECT_LENGTH2, m_dEditBoundaryDefectLength[1]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_DEFECT_MB_LENGTH2, m_dEditBoundaryDefectMBLength[1]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_DEFECT_WIDTH2, m_dEditBoundaryDefectWidth[1]);
	DDX_Check(pDX, IDC_CHECK_BOUNDARY_USE_CONNECTION2, m_bCheckBoundaryUseConnection[1]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_CONNECT_LENGTH2, m_iEditBoundaryConnectionLength[1]);

	DDX_Check(pDX, IDC_CHECK_BOUNDARY_USE_TYPE_3, m_bCheckBoundaryCondition[2]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_DEFECT_LENGTH3, m_dEditBoundaryDefectLength[2]);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_DEFECT_WIDTH3, m_dEditBoundaryDefectWidth[2]);

	DDX_Check(pDX, IDC_CHECK_BOUNDARY_USE_ROI_CONNECTED, m_bCheckBoundaryUseRoiConnected);

	DDX_Control(pDX, IDC_BUTTON_AUTO_PARAM_CAL, m_bnAutoParamCal);
	DDX_Control(pDX, IDC_BUTTON_SCORE, m_bnScore);
}


BEGIN_MESSAGE_MAP(CAlgorithmBoundaryDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_PARAM_CAL, &CAlgorithmBoundaryDlg::OnBnClickedButtonAutoParamCal)
	ON_BN_CLICKED(IDC_BUTTON_SCORE, &CAlgorithmBoundaryDlg::OnBnClickedButtonScore)
END_MESSAGE_MAP()


// CAlgorithmBoundaryDlg 메시지 처리기입니다.

void CAlgorithmBoundaryDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmBoundaryDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseBoundaryInspection = AlgorithmParam.m_bUseBoundaryInspection;
	m_iRadioBoundaryEdgeMeasureDir = AlgorithmParam.m_iBoundaryEdgeMeasureDir;
	m_iRadioBoundaryEdgeMeasureGv = AlgorithmParam.m_iBoundaryEdgeMeasureGv;
	m_iRadioBoundaryEdgeMeasurePos = AlgorithmParam.m_iBoundaryEdgeMeasurePos;
	m_dEditBoundaryEdgeMeasureSmFactor = AlgorithmParam.m_dBoundaryEdgeMeasureSmFactor;
	m_dEditBoundaryEdgeMeasureEdgeStr = AlgorithmParam.m_dBoundaryEdgeMeasureEdgeStr;
	m_iEditBoundaryEdgeMeasureSampling = AlgorithmParam.m_iBoundaryEdgeMeasureSampling;
	m_iEditBoundaryEdgeMeasureSensorLength = AlgorithmParam.m_iBoundaryEdgeMeasureSensorLength;
	m_iRadioBoundaryDisplay = AlgorithmParam.m_iBoundaryDisplay;

	for (int i = 0; i < 2; i++)
	{
		m_bCheckBoundaryCondition[i] = AlgorithmParam.m_bBoundaryCondition[i];
		m_iEditBoundaryMedianFilterSize[i] = AlgorithmParam.m_iBoundaryMedianFilterSize[i];
		m_dEditBoundaryDefectLength[i] = AlgorithmParam.m_dBoundaryDefectDepth[i];
		m_dEditBoundaryDefectMBLength[i] = AlgorithmParam.m_dBoundaryDefectMBDepth[i];
		m_dEditBoundaryDefectWidth[i] = AlgorithmParam.m_dBoundaryDefectLength[i];
		m_bCheckBoundaryUseConnection[i] = AlgorithmParam.m_bBoundaryUseConnection[i];
		m_iEditBoundaryConnectionLength[i] = AlgorithmParam.m_iBoundaryConnectionLength[i];
	}

	m_bCheckBoundaryCondition[2] = AlgorithmParam.m_bBoundaryCondition[2];
	m_dEditBoundaryDefectLength[2] = AlgorithmParam.m_dBoundaryDefectDepth[2];
	m_dEditBoundaryDefectWidth[2] = AlgorithmParam.m_dBoundaryDefectLength[2];

	m_bCheckBoundaryUseRoiConnected = AlgorithmParam.m_bBoundaryUseRoiConnected;

	UpdateData(FALSE);
}

void CAlgorithmBoundaryDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseBoundaryInspection = m_bCheckUseBoundaryInspection;
	pAlgorithmParam->m_iBoundaryEdgeMeasureDir = m_iRadioBoundaryEdgeMeasureDir;
	pAlgorithmParam->m_iBoundaryEdgeMeasureGv = m_iRadioBoundaryEdgeMeasureGv;
	pAlgorithmParam->m_iBoundaryEdgeMeasurePos = m_iRadioBoundaryEdgeMeasurePos;
	pAlgorithmParam->m_dBoundaryEdgeMeasureSmFactor = m_dEditBoundaryEdgeMeasureSmFactor;
	pAlgorithmParam->m_dBoundaryEdgeMeasureEdgeStr = m_dEditBoundaryEdgeMeasureEdgeStr;
	pAlgorithmParam->m_iBoundaryEdgeMeasureSampling = m_iEditBoundaryEdgeMeasureSampling;
	pAlgorithmParam->m_iBoundaryEdgeMeasureSensorLength = m_iEditBoundaryEdgeMeasureSensorLength;
	pAlgorithmParam->m_iBoundaryDisplay = m_iRadioBoundaryDisplay;

	for (int i = 0; i < 2; i++)
	{
		pAlgorithmParam->m_bBoundaryCondition[i] = m_bCheckBoundaryCondition[i];
		pAlgorithmParam->m_iBoundaryMedianFilterSize[i] = m_iEditBoundaryMedianFilterSize[i];
		pAlgorithmParam->m_dBoundaryDefectDepth[i] = m_dEditBoundaryDefectLength[i];
		pAlgorithmParam->m_dBoundaryDefectMBDepth[i] = m_dEditBoundaryDefectMBLength[i];
		pAlgorithmParam->m_dBoundaryDefectLength[i] = m_dEditBoundaryDefectWidth[i];
		pAlgorithmParam->m_bBoundaryUseConnection[i] = m_bCheckBoundaryUseConnection[i];
		pAlgorithmParam->m_iBoundaryConnectionLength[i] = m_iEditBoundaryConnectionLength[i];
	}

	pAlgorithmParam->m_bBoundaryCondition[2] = m_bCheckBoundaryCondition[2];
	pAlgorithmParam->m_dBoundaryDefectDepth[2] = m_dEditBoundaryDefectLength[2];
	pAlgorithmParam->m_dBoundaryDefectLength[2] = m_dEditBoundaryDefectWidth[2];

	pAlgorithmParam->m_bBoundaryUseRoiConnected = m_bCheckBoundaryUseRoiConnected;
}

BOOL CAlgorithmBoundaryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bnAutoParamCal.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(210, 180, 120), 0, 0);
	m_bnScore.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(180, 195, 210), 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAlgorithmBoundaryDlg::OnBnClickedButtonAutoParamCal()
{
	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		CAlgorithmParam AlgorithmParam;

		AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information

		THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->m_pAlgorithmImageProcessDlg->GetParam(&AlgorithmParam);	// Image Processing Tab Data
		THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->m_pAlgorithmBoundaryDlg->GetParam(&AlgorithmParam);	// Boundary Inspection Tab Data
		THEAPP.m_pTabControlDlg->AlgorithmPreProcessing();

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

		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return;

		int iRet = 0;
		iRet = THEAPP.m_pAlgorithm->BoundaryInspectionAutoCalibration(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HROIRgn, &AlgorithmParam);

		CString sMsg;
		switch (iRet)
		{
		case 0: sMsg.Format("자동 파라미터 캘리브레이션 완료! 파라미터를 적용하시겠습니까?"); break;
		case -1: sMsg.Format("검사영역 없음! 검사영역을 확인 후 재시도하세요."); break;
		case -2: sMsg.Format("검사영역 Contour 2개 이상! 검사영역을 확인 후 재시도하세요."); break;
		case -3: sMsg.Format("검사영역 Contour 포인트 없음! 검사영역을 확인 후 재시도하세요."); break;
		case -4: sMsg.Format("할콘 예외 발생!"); break;
		default: break;
		}

		if (iRet == 0)
		{
			if (AfxMessageBox(sMsg, MB_OKCANCEL) == IDOK)
			{
				m_iRadioBoundaryEdgeMeasureDir = AlgorithmParam.m_iBoundaryEdgeMeasureDir;
				m_iRadioBoundaryEdgeMeasureGv = AlgorithmParam.m_iBoundaryEdgeMeasureGv;
				m_iRadioBoundaryEdgeMeasurePos = AlgorithmParam.m_iBoundaryEdgeMeasurePos;
				m_dEditBoundaryEdgeMeasureSmFactor = AlgorithmParam.m_dBoundaryEdgeMeasureSmFactor;
				m_dEditBoundaryEdgeMeasureEdgeStr = AlgorithmParam.m_dBoundaryEdgeMeasureEdgeStr;

				UpdateData(FALSE);
			}
		}
		else
			AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONERROR);

		return;
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

		strLog.Format("Halcon Exception [CAlgorithmBoundaryDlg::OnBnClickedButtonAutoParamCal] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmBoundaryDlg::OnBnClickedButtonScore()
{
	try
	{
		if (AfxMessageBox("이 기능은 양품 이미지에서 진행하여야 정확한 결과를 얻을 수 있습니다. 진행하시겠습니까?", MB_OKCANCEL) == IDCANCEL)
			return;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		CAlgorithmParam AlgorithmParam;

		AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information

		THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->m_pAlgorithmImageProcessDlg->GetParam(&AlgorithmParam);	// Image Processing Tab Data
		THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->m_pAlgorithmBoundaryDlg->GetParam(&AlgorithmParam);	// Boundary Inspection Tab Data
		THEAPP.m_pTabControlDlg->AlgorithmPreProcessing();

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

		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return;

		HObject HEdgeContourRgn;
		double dScore;

		HEdgeContourRgn = THEAPP.m_pAlgorithm->BoundaryInspectionAssessment(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HROIRgn, AlgorithmParam, &dScore);
		if (THEAPP.m_pGFunction->ValidHRegion(HEdgeContourRgn) == FALSE)
		{
			AfxMessageBox("Edge 검출 오류", MB_SYSTEMMODAL | MB_ICONERROR);
		}
		else
		{
			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAIDefectRgn));
			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HEdgeContourRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
			Union1(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));

			THEAPP.m_pInspectAdminViewDlg->UpdateView();
		}

		return;
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

		strLog.Format("Halcon Exception [CAlgorithmBoundaryDlg::OnBnClickedButtonScore] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}
