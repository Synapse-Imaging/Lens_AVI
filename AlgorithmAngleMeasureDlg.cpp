// AlgorithmAngleMeasureDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmAngleMeasureDlg.h"
#include "afxdialogex.h"


// CAlgorithmAngleMeasureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmAngleMeasureDlg, CDialog)

CAlgorithmAngleMeasureDlg::CAlgorithmAngleMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmAngleMeasureDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseAngleMeasure = FALSE;
	m_iEditAngleMeasureDetectArea11 = 0;
	m_iEditAngleMeasureDetectArea21 = 50;
	m_iEditAngleMeasureDetectArea12 = 0;
	m_iEditAngleMeasureDetectArea22 = 50;
	m_iRadioAngleMeasureDir = 0;
	m_iRadioAngleMeasureGv1 = 0;
	m_iRadioAngleMeasureGv2 = 0;
	m_iRadioAngleMeasurePos1 = 0;
	m_iRadioAngleMeasurePos2 = 0;
	m_bCheckAngleMeasureLocalAlignUse = FALSE;
	m_dEditAngleMeasureSmFactor1 = 1.0;
	m_dEditAngleMeasureSmFactor2 = 1.0;
	m_iEditAngleMeasureAngleStr1 = 10;
	m_iEditAngleMeasureAngleStr2 = 10;
	m_iEditAngleMeasurePosOffset1 = 0;
	m_iEditAngleMeasurePosOffset2 = 0;
}

CAlgorithmAngleMeasureDlg::~CAlgorithmAngleMeasureDlg()
{
}

void CAlgorithmAngleMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_ANGLE_MEASURE, m_bCheckUseAngleMeasure);
	DDX_Text(pDX, IDC_EDIT_ANGLE_MEASURE_DETECT_AREA1_1, m_iEditAngleMeasureDetectArea11);
	DDX_Text(pDX, IDC_EDIT_ANGLE_MEASURE_DETECT_AREA2_1, m_iEditAngleMeasureDetectArea21);
	DDX_Text(pDX, IDC_EDIT_ANGLE_MEASURE_DETECT_AREA1_2, m_iEditAngleMeasureDetectArea12);
	DDX_Text(pDX, IDC_EDIT_ANGLE_MEASURE_DETECT_AREA2_2, m_iEditAngleMeasureDetectArea22);
	DDX_Radio(pDX, IDC_RADIO_ANGLE_MEASURE_DIR_X, m_iRadioAngleMeasureDir);
	DDX_Radio(pDX, IDC_RADIO_ANGLE_MEASURE_GV_ANY_1, m_iRadioAngleMeasureGv1);
	DDX_Radio(pDX, IDC_RADIO_ANGLE_MEASURE_GV_ANY_2, m_iRadioAngleMeasureGv2);
	DDX_Radio(pDX, IDC_RADIO_ANGLE_MEASURE_POS_BEST_1, m_iRadioAngleMeasurePos1);
	DDX_Radio(pDX, IDC_RADIO_ANGLE_MEASURE_POS_BEST_2, m_iRadioAngleMeasurePos2);
	DDX_Check(pDX, IDC_CHECK_ANGLE_MEASURE_LOCAL_ALIGN_USE, m_bCheckAngleMeasureLocalAlignUse);
	DDX_Text(pDX, IDC_EDIT_ANGLE_MEASURE_SMOOTHING_FACTOR_1, m_dEditAngleMeasureSmFactor1);
	DDX_Text(pDX, IDC_EDIT_ANGLE_MEASURE_SMOOTHING_FACTOR_2, m_dEditAngleMeasureSmFactor2);
	DDX_Text(pDX, IDC_EDIT_ANGLE_MEASURE_ANGLE_STRENGTH_1, m_iEditAngleMeasureAngleStr1);
	DDX_Text(pDX, IDC_EDIT_ANGLE_MEASURE_ANGLE_STRENGTH_2, m_iEditAngleMeasureAngleStr2);
	DDX_Text(pDX, IDC_EDIT_ANGLE_MEASURE_POS_OFFSET_1, m_iEditAngleMeasurePosOffset1);
	DDX_Text(pDX, IDC_EDIT_ANGLE_MEASURE_POS_OFFSET_2, m_iEditAngleMeasurePosOffset2);
}


BEGIN_MESSAGE_MAP(CAlgorithmAngleMeasureDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmAngleMeasureDlg 메시지 처리기입니다.

void CAlgorithmAngleMeasureDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmAngleMeasureDlg::SetParam(CAlgorithmParam AlgorithmParam)
{

	UpdateData(FALSE);
}

void CAlgorithmAngleMeasureDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

}


BOOL CAlgorithmAngleMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
