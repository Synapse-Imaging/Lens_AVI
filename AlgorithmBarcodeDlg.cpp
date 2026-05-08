// AlgorithmBarcodeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmBarcodeDlg.h"
#include "afxdialogex.h"


// CAlgorithmBarcodeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmBarcodeDlg, CDialog)

CAlgorithmBarcodeDlg::CAlgorithmBarcodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmBarcodeDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseBarcode = FALSE;
	m_dEditBarcodeShiftToleranceX = 0.0;
	m_dEditBarcodeShiftToleranceY = 0.0;

	m_bCheckInspectBarcodeShift = FALSE;
	m_bCheckInspectBarcodeRotation = FALSE;

	m_bCheckUseBarcodeOtherImage = FALSE;
	m_iEditBarcodeOtherImageNumber = 1;
}

CAlgorithmBarcodeDlg::~CAlgorithmBarcodeDlg()
{
}

void CAlgorithmBarcodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_BARCODE, m_bCheckUseBarcode);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_TOLERANCE_X, m_dEditBarcodeShiftToleranceX);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_TOLERANCE_Y, m_dEditBarcodeShiftToleranceY);
	DDX_Check(pDX, IDC_CHECK_USE_BARCODE_SHIFT, m_bCheckInspectBarcodeShift);
	DDX_Check(pDX, IDC_CHECK_USE_BARCODE_ROTATION, m_bCheckInspectBarcodeRotation);
	DDX_Check(pDX, IDC_CHECK_USE_BARCODE_OTHER_IMAGE, m_bCheckUseBarcodeOtherImage);
	DDX_Text(pDX, IDC_EDIT_BARCODE_OTHER_IMAGE_NUMBER, m_iEditBarcodeOtherImageNumber);
}


BEGIN_MESSAGE_MAP(CAlgorithmBarcodeDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmBarcodeDlg 메시지 처리기입니다.

void CAlgorithmBarcodeDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmBarcodeDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseBarcode = AlgorithmParam.m_bUseBarcode;

	m_dEditBarcodeShiftToleranceX = AlgorithmParam.m_dBarcodeShiftToleranceX;
	m_dEditBarcodeShiftToleranceY = AlgorithmParam.m_dBarcodeShiftToleranceY;

	m_bCheckInspectBarcodeShift = AlgorithmParam.m_bInspectBarcodeShift;
	m_bCheckInspectBarcodeRotation = AlgorithmParam.m_bInspectBarcodeRotation;

	m_bCheckUseBarcodeOtherImage = AlgorithmParam.m_bUseBarcodeOtherImage;
	m_iEditBarcodeOtherImageNumber = AlgorithmParam.m_iBarcodeOtherImageNumber;

	UpdateData(FALSE);
}

void CAlgorithmBarcodeDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseBarcode = m_bCheckUseBarcode;
	pAlgorithmParam->m_dBarcodeShiftToleranceX = m_dEditBarcodeShiftToleranceX;
	pAlgorithmParam->m_dBarcodeShiftToleranceY = m_dEditBarcodeShiftToleranceY;

	pAlgorithmParam->m_bInspectBarcodeShift = m_bCheckInspectBarcodeShift;
	pAlgorithmParam->m_bInspectBarcodeRotation = m_bCheckInspectBarcodeRotation;

	pAlgorithmParam->m_bUseBarcodeOtherImage = m_bCheckUseBarcodeOtherImage;
	pAlgorithmParam->m_iBarcodeOtherImageNumber = m_iEditBarcodeOtherImageNumber;
}

BOOL CAlgorithmBarcodeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
