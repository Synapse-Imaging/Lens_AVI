// AlgorithmConnectorPinDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmConnectorPinDlg.h"
#include "afxdialogex.h"


// CAlgorithmConnectorPinDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmConnectorPinDlg, CDialog)

CAlgorithmConnectorPinDlg::CAlgorithmConnectorPinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmConnectorPinDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUsePinLength = FALSE;
	m_iRadioPinPos = 0;
	m_iEditPinDTFilterSize = 0;
	m_iEditPinOpeningClosingSize = 0;
	m_iEditPinMinArea = 0;
	m_iEditPinDistanceTolerance = 0;
	m_iEditPinDTValue = 0;
	m_iEditPinDefectNumberTolerance = 0;
	m_iEditPinPitch = 0;
}

CAlgorithmConnectorPinDlg::~CAlgorithmConnectorPinDlg()
{
}

void CAlgorithmConnectorPinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_PIN_LENGTH, m_bCheckUsePinLength);
	DDX_Radio(pDX, IDC_RADIO_PIN_POS_TOP, m_iRadioPinPos);
	DDX_Text(pDX, IDC_EDIT_PIN_DT_FILTER_SIZE, m_iEditPinDTFilterSize);
	DDX_Text(pDX, IDC_EDIT_PIN_OPENING_CLOSING_SIZE, m_iEditPinOpeningClosingSize);
	DDX_Text(pDX, IDC_EDIT_PIN_MIN_AREA, m_iEditPinMinArea);
	DDX_Text(pDX, IDC_EDIT_PIN_DISTANCE_TOLERANCE, m_iEditPinDistanceTolerance);
	DDX_Text(pDX, IDC_EDIT_PIN_DT_VALUE, m_iEditPinDTValue);
	DDX_Text(pDX, IDC_EDIT_PIN_DEFECT_NUMBER_TOLERANCE, m_iEditPinDefectNumberTolerance);
	DDX_Text(pDX, IDC_EDIT_PIN_PITCH, m_iEditPinPitch);
}


BEGIN_MESSAGE_MAP(CAlgorithmConnectorPinDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmConnectorPinDlg 메시지 처리기입니다.

void CAlgorithmConnectorPinDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmConnectorPinDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUsePinLength = AlgorithmParam.m_bUsePinLength;
	m_iRadioPinPos = AlgorithmParam.m_iPinPos;
	m_iEditPinDTFilterSize = AlgorithmParam.m_iPinDTFilterSize;
	m_iEditPinOpeningClosingSize = AlgorithmParam.m_iPinOpeningClosingSize;
	m_iEditPinMinArea = AlgorithmParam.m_iPinMinArea;
	m_iEditPinDistanceTolerance = AlgorithmParam.m_iPinDistanceTolerance;
	m_iEditPinDTValue = AlgorithmParam.m_iPinDTValue;
	m_iEditPinDefectNumberTolerance = AlgorithmParam.m_iPinDefectNumberTolerance;
	m_iEditPinPitch = AlgorithmParam.m_iPinPitch;

	UpdateData(FALSE);
}

void CAlgorithmConnectorPinDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUsePinLength = m_bCheckUsePinLength;
	pAlgorithmParam->m_iPinPos = m_iRadioPinPos;
	pAlgorithmParam->m_iPinDTFilterSize = m_iEditPinDTFilterSize;
	pAlgorithmParam->m_iPinOpeningClosingSize = m_iEditPinOpeningClosingSize;
	pAlgorithmParam->m_iPinMinArea = m_iEditPinMinArea;
	pAlgorithmParam->m_iPinDistanceTolerance = m_iEditPinDistanceTolerance;
	pAlgorithmParam->m_iPinDTValue = m_iEditPinDTValue;
	pAlgorithmParam->m_iPinDefectNumberTolerance = m_iEditPinDefectNumberTolerance;
	pAlgorithmParam->m_iPinPitch = m_iEditPinPitch;
}

BOOL CAlgorithmConnectorPinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
