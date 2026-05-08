// AlgorithmStiffenerEpoxyDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmStiffenerEpoxyDlg.h"
#include "afxdialogex.h"


// CAlgorithmStiffenerEpoxyDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmStiffenerEpoxyDlg, CDialog)

CAlgorithmStiffenerEpoxyDlg::CAlgorithmStiffenerEpoxyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmStiffenerEpoxyDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;
}

CAlgorithmStiffenerEpoxyDlg::~CAlgorithmStiffenerEpoxyDlg()
{
}

void CAlgorithmStiffenerEpoxyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_STIFFENER_EPOXY, m_bCheckUseStiffenerEpoxy);
	DDX_Text(pDX, IDC_EDIT_STIFFENER_EPOXY_STIFFENER_OUTER_BOUNDARY_THRESHOLD, m_iEditThresholdParam_StiffenerRegion);
	DDX_Text(pDX, IDC_EDIT_STIFFENER_EPOXY_STIFFENER_OUTER_BOUNDARY_DT_FILTER_SIZE, m_iEditDTFilterX_StiffenerRegion);
	DDX_Text(pDX, IDC_EDIT_STIFFENER_EPOXY_STIFFENER_OUTER_BOUNDARY_DT_FILTER_SIZE2, m_iEditDTFilterY_StiffenerRegion);
	DDX_Text(pDX, IDC_EDIT_STIFFENER_EPOXY_STIFFENER_OUTER_BOUNDARY_DT_VALUE, m_iEditDTValue_StiffenerRegion);
	DDX_Text(pDX, IDC_EDIT_STIFFENER_EPOXY_STIFFENER_OUTER_BOUNDARY_CLOSING, m_iEditClosing_StiffenerRegion);
	DDX_Text(pDX, IDC_EDIT_STIFFENER_EPOXY_STIFFENER_INNER_BOUNDARY_EROSION, m_dEditErosionParam_StiffenerBoundary);
	DDX_Text(pDX, IDC_EDIT_STIFFENER_EPOXY_STIFFENER_INNER_REGION_THRESHOLD, m_iEditThresholdParam_SheildcanRegion);
	DDX_Text(pDX, IDC_EDIT_STIFFENER_EPOXY_SHIELDCAN_BOUNDARY_DILATION, m_dEditDilationParam_SheildcanBoundary);
	DDX_Text(pDX, IDC_EDIT_STIFFENER_EPOXY_CORNER_BOUNDARY_OPENING, m_dEditOpeningParam_CornerBoundary);
}

BEGIN_MESSAGE_MAP(CAlgorithmStiffenerEpoxyDlg, CDialog)
	ON_BN_CLICKED(IDC_MFCBUTTON_NEXT_PROCESSING, &CAlgorithmStiffenerEpoxyDlg::OnBnClickedMfcbuttonNextProcessing)
END_MESSAGE_MAP()

// CAlgorithmStiffenerEpoxyDlg 메시지 처리기입니다.


void CAlgorithmStiffenerEpoxyDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmStiffenerEpoxyDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseStiffenerEpoxy = AlgorithmParam.m_bUseStiffenerEpoxy;
	m_iEditThresholdParam_StiffenerRegion = AlgorithmParam.m_iThresholdParam_StiffenerRegion;
	m_iEditDTFilterX_StiffenerRegion = AlgorithmParam.m_iDTFilterX_StiffenerRegion;
	m_iEditDTFilterY_StiffenerRegion = AlgorithmParam.m_iDTFilterY_StiffenerRegion;
	m_iEditDTValue_StiffenerRegion = AlgorithmParam.m_iDTValue_StiffenerRegion;
	m_iEditClosing_StiffenerRegion = AlgorithmParam.m_iClosing_StiffenerRegion;
	m_dEditErosionParam_StiffenerBoundary = AlgorithmParam.m_dErosionParam_StiffenerBoundary;
	m_iEditThresholdParam_SheildcanRegion = AlgorithmParam.m_iThresholdParam_SheildcanRegion;
	m_dEditDilationParam_SheildcanBoundary = AlgorithmParam.m_dDilationParam_SheildcanBoundary;
	m_dEditOpeningParam_CornerBoundary = AlgorithmParam.m_dOpeningParam_CornerBoundary;

	UpdateData(FALSE);
}

void CAlgorithmStiffenerEpoxyDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseStiffenerEpoxy = m_bCheckUseStiffenerEpoxy;
	pAlgorithmParam->m_iThresholdParam_StiffenerRegion = m_iEditThresholdParam_StiffenerRegion;
	pAlgorithmParam->m_iDTFilterX_StiffenerRegion = m_iEditDTFilterX_StiffenerRegion;
	pAlgorithmParam->m_iDTFilterY_StiffenerRegion = m_iEditDTFilterY_StiffenerRegion;
	pAlgorithmParam->m_iDTValue_StiffenerRegion = m_iEditDTValue_StiffenerRegion;
	pAlgorithmParam->m_iClosing_StiffenerRegion = m_iEditClosing_StiffenerRegion;
	pAlgorithmParam->m_dErosionParam_StiffenerBoundary = m_dEditErosionParam_StiffenerBoundary;
	pAlgorithmParam->m_iThresholdParam_SheildcanRegion = m_iEditThresholdParam_SheildcanRegion;
	pAlgorithmParam->m_dDilationParam_SheildcanBoundary = m_dEditDilationParam_SheildcanBoundary;
	pAlgorithmParam->m_dOpeningParam_CornerBoundary = m_dEditOpeningParam_CornerBoundary;
}

BOOL CAlgorithmStiffenerEpoxyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmStiffenerEpoxyDlg::OnBnClickedMfcbuttonNextProcessing()
{
	THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->UpdateAlgorithmTab(8);
}
