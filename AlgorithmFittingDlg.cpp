// AlgorithmFittingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmFittingDlg.h"
#include "afxdialogex.h"


// CAlgorithmFittingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmFittingDlg, CDialog)

CAlgorithmFittingDlg::CAlgorithmFittingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmFittingDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseLineFitting = FALSE;
	m_bCheckLineFitUseOtherImageFitting = FALSE;
	m_iEditLineFitOtherImageNumber = 0;
	m_dEditLineFitFittingEdgeSF = 0.0;
	m_dEditLineFitObjectEdgeSF = 0.0;
	m_iEditLineFitContourConnectionLength = 0;
	m_iEditLineFitMinContourLength = 0;
	m_bCheckLineFitUseBurInspection = FALSE;
	m_dEditLineFitBurDefectLength = 0.0;
	m_dEditLineFitBurDefectLengthMax = 10000.0;
	m_bCheckLineFitUseChippingInspection = FALSE;
	m_dEditLineFitChippingDefectLength = 0.0;
	m_dEditLineFitChippingDefectLengthMax = 10000.0;
	m_iEditLineFitObjectLowerThres = 0;
	m_iEditLineFitObjectUpperThres = 0;
	m_bCheckLineFitUseRotationInspection = FALSE;
	m_iRadioLineFitRotationReference = 0;
	m_dEditLineFitRotationDefectAngle = 0.0;
	m_iRadioLineFitEdgeMethod = 0;
	m_iEditLineFitFittingEdgeHigh = 0;
	m_iEditLineFitFittingContourConnectionLength = 0;
	m_iEditLineFitFittingMinContourLength = 0;
	m_iEditLineFitEdgeHigh = 0;
	m_bCheckLineFitLengthSave = FALSE;
	m_bCheckNoUseLineFit = FALSE;
	m_bCheckLineFitLocalAlignUse = FALSE;
}

CAlgorithmFittingDlg::~CAlgorithmFittingDlg()
{
}

void CAlgorithmFittingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_LINE_FITTING, m_bCheckUseLineFitting);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_CONTOUR_CONNECTION_LENGTH, m_iEditLineFitContourConnectionLength);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_MIN_CONTOUR_LENGTH, m_iEditLineFitMinContourLength);
	DDX_Check(pDX, IDC_CHECK_USE_LINE_FIT_OTHER_IMAGE, m_bCheckLineFitUseOtherImageFitting);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_OTHER_IMAGE_NUMBER, m_iEditLineFitOtherImageNumber);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_FITTING_EDGE_SF, m_dEditLineFitFittingEdgeSF);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_OBJECT_EDGE_SF, m_dEditLineFitObjectEdgeSF);
	DDX_Check(pDX, IDC_CHECK_USE_LINE_FIT_BUR_INSPECTION, m_bCheckLineFitUseBurInspection);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_BUR_DEFECT_LENGTH, m_dEditLineFitBurDefectLength);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_BUR_DEFECT_LENGTH_MAX, m_dEditLineFitBurDefectLengthMax);
	DDX_Check(pDX, IDC_CHECK_USE_LINE_FIT_CHIPPING_INSPECTION, m_bCheckLineFitUseChippingInspection);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_CHIPPING_DEFECT_LENGTH, m_dEditLineFitChippingDefectLength);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_CHIPPING_DEFECT_LENGTH_MAX, m_dEditLineFitChippingDefectLengthMax);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_OBJECT_LOWER_THRES, m_iEditLineFitObjectLowerThres);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_OBJECT_UPPER_THRES, m_iEditLineFitObjectUpperThres);
	DDX_Check(pDX, IDC_CHECK_USE_LINE_FIT_ROTATION_INSPECTION, m_bCheckLineFitUseRotationInspection);
	DDX_Radio(pDX, IDC_RADIO_LINE_FIT_ROTATION_REF_H, m_iRadioLineFitRotationReference);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_ROTATION_DEFECT_ANGLE, m_dEditLineFitRotationDefectAngle);
	DDX_Radio(pDX, IDC_RADIO_LINE_FIT_AUTO, m_iRadioLineFitEdgeMethod);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_FITTING_EDGE_HIGH, m_iEditLineFitFittingEdgeHigh);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_FITTING_CONTOUR_CONNECTION_LENGTH, m_iEditLineFitFittingContourConnectionLength);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_FITTING_MIN_CONTOUR_LENGTH, m_iEditLineFitFittingMinContourLength);
	DDX_Text(pDX, IDC_EDIT_LINE_FIT_EDGE_HIGH, m_iEditLineFitEdgeHigh);
	DDX_Check(pDX, IDC_CHECK_USE_LINE_FIT_SAVE_LOG, m_bCheckLineFitLengthSave);
	DDX_Check(pDX, IDC_CHECK_NOUSE_LINE_FIT, m_bCheckNoUseLineFit);
	DDX_Check(pDX, IDC_CHECK_LINE_FITTING_LOCAL_ALIGN_USE, m_bCheckLineFitLocalAlignUse);
}


BEGIN_MESSAGE_MAP(CAlgorithmFittingDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmFittingDlg 메시지 처리기입니다.

void CAlgorithmFittingDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmFittingDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseLineFitting = AlgorithmParam.m_bUseLineFitting;
	m_iEditLineFitObjectLowerThres = AlgorithmParam.m_iLineFitObjectLowerThres;
	m_iEditLineFitObjectUpperThres = AlgorithmParam.m_iLineFitObjectUpperThres;
	m_bCheckLineFitUseOtherImageFitting = AlgorithmParam.m_bLineFitUseOtherImageFitting;
	m_iEditLineFitOtherImageNumber = AlgorithmParam.m_iLineFitOtherImageNumber;
	m_dEditLineFitFittingEdgeSF = AlgorithmParam.m_dLineFitFittingEdgeSF;
	m_dEditLineFitObjectEdgeSF = AlgorithmParam.m_dLineFitObjectEdgeSF;
	m_iEditLineFitContourConnectionLength = AlgorithmParam.m_iLineFitContourConnectionLength;
	m_iEditLineFitMinContourLength = AlgorithmParam.m_iLineFitMinContourLength;
	m_bCheckLineFitUseBurInspection = AlgorithmParam.m_bLineFitUseBurInspection;
	m_dEditLineFitBurDefectLength = AlgorithmParam.m_dLineFitBurDefectLength;
	m_dEditLineFitBurDefectLengthMax = AlgorithmParam.m_dLineFitBurDefectLengthMax;
	m_bCheckLineFitUseChippingInspection = AlgorithmParam.m_bLineFitUseChippingInspection;
	m_dEditLineFitChippingDefectLength = AlgorithmParam.m_dLineFitChippingDefectLength;
	m_dEditLineFitChippingDefectLengthMax = AlgorithmParam.m_dLineFitChippingDefectLengthMax;
	m_bCheckLineFitUseRotationInspection = AlgorithmParam.m_bLineFitUseRotationInspection;
	m_iRadioLineFitRotationReference = AlgorithmParam.m_iLineFitRotationReference;
	m_dEditLineFitRotationDefectAngle = AlgorithmParam.m_dLineFitRotationDefectAngle;
	m_iRadioLineFitEdgeMethod = AlgorithmParam.m_iLineFitEdgeMethod;
	m_iEditLineFitFittingEdgeHigh = AlgorithmParam.m_iLineFitFittingEdgeHigh;
	m_iEditLineFitFittingContourConnectionLength = AlgorithmParam.m_iLineFitFittingContourConnectionLength;
	m_iEditLineFitFittingMinContourLength = AlgorithmParam.m_iLineFitFittingMinContourLength;
	m_iEditLineFitEdgeHigh = AlgorithmParam.m_iLineFitEdgeHigh;
	m_bCheckLineFitLengthSave = AlgorithmParam.m_bLineFitLengthSave;
	m_bCheckNoUseLineFit = AlgorithmParam.m_bNoUseLineFit;
	m_bCheckLineFitLocalAlignUse = AlgorithmParam.m_bLineFitLocalAlignUse;

	UpdateData(FALSE);
}

void CAlgorithmFittingDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseLineFitting = m_bCheckUseLineFitting;
	pAlgorithmParam->m_iLineFitObjectLowerThres = m_iEditLineFitObjectLowerThres;
	pAlgorithmParam->m_iLineFitObjectUpperThres = m_iEditLineFitObjectUpperThres;
	pAlgorithmParam->m_bLineFitUseOtherImageFitting = m_bCheckLineFitUseOtherImageFitting;
	pAlgorithmParam->m_iLineFitOtherImageNumber = m_iEditLineFitOtherImageNumber;
	pAlgorithmParam->m_dLineFitFittingEdgeSF = m_dEditLineFitFittingEdgeSF;
	pAlgorithmParam->m_dLineFitObjectEdgeSF = m_dEditLineFitObjectEdgeSF;
	pAlgorithmParam->m_iLineFitContourConnectionLength = m_iEditLineFitContourConnectionLength;
	pAlgorithmParam->m_iLineFitMinContourLength = m_iEditLineFitMinContourLength;
	pAlgorithmParam->m_bLineFitUseBurInspection = m_bCheckLineFitUseBurInspection;
	pAlgorithmParam->m_dLineFitBurDefectLength = m_dEditLineFitBurDefectLength;
	pAlgorithmParam->m_dLineFitBurDefectLengthMax = m_dEditLineFitBurDefectLengthMax;
	pAlgorithmParam->m_bLineFitUseChippingInspection = m_bCheckLineFitUseChippingInspection;
	pAlgorithmParam->m_dLineFitChippingDefectLength = m_dEditLineFitChippingDefectLength;
	pAlgorithmParam->m_dLineFitChippingDefectLengthMax = m_dEditLineFitChippingDefectLengthMax;
	pAlgorithmParam->m_bLineFitUseRotationInspection = m_bCheckLineFitUseRotationInspection;
	pAlgorithmParam->m_iLineFitRotationReference = m_iRadioLineFitRotationReference;
	pAlgorithmParam->m_dLineFitRotationDefectAngle = m_dEditLineFitRotationDefectAngle;
	pAlgorithmParam->m_iLineFitEdgeMethod = m_iRadioLineFitEdgeMethod;
	pAlgorithmParam->m_iLineFitFittingEdgeHigh = m_iEditLineFitFittingEdgeHigh;
	pAlgorithmParam->m_iLineFitFittingContourConnectionLength = m_iEditLineFitFittingContourConnectionLength;
	pAlgorithmParam->m_iLineFitFittingMinContourLength = m_iEditLineFitFittingMinContourLength;
	pAlgorithmParam->m_iLineFitEdgeHigh = m_iEditLineFitEdgeHigh;
	pAlgorithmParam->m_bLineFitLengthSave = m_bCheckLineFitLengthSave;
	pAlgorithmParam->m_bNoUseLineFit = m_bCheckNoUseLineFit;
	pAlgorithmParam->m_bLineFitLocalAlignUse = m_bCheckLineFitLocalAlignUse;
}


BOOL CAlgorithmFittingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
