// AlgorithmNeckEpoxyCrackDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmNeckEpoxyCrackDlg.h"
#include "afxdialogex.h"


// CAlgorithmNeckEpoxyCrackDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmNeckEpoxyCrackDlg, CDialog)

CAlgorithmNeckEpoxyCrackDlg::CAlgorithmNeckEpoxyCrackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmNeckEpoxyCrackDlg::IDD, pParent)
	, m_bCheckNeckEpoxyCrackUseXDirMode(FALSE)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseNeckEpoxyCrack = FALSE;
	m_iEditNeckEpoxyCrackOpeningSize = 2;
	m_iEditNeckEpoxyCrackYLength = 150;
	m_iEditNeckEpoxyCrackBoundaryWidth = 4;
	m_iRadioNeckEpoxyCrackDir = NECK_EPOXY_CRACK_DIR_LEFT;
	m_bCheckNeckEpoxyCrackUseXDirMode = FALSE;
}

CAlgorithmNeckEpoxyCrackDlg::~CAlgorithmNeckEpoxyCrackDlg()
{
}

void CAlgorithmNeckEpoxyCrackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_NECK_EPOXY_CRACK, m_bCheckUseNeckEpoxyCrack);
	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_CRACK_OPENING_SIZE, m_iEditNeckEpoxyCrackOpeningSize);
	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_CRACK_Y_LENGTH, m_iEditNeckEpoxyCrackYLength);
	DDX_Text(pDX, IDC_EDIT_NECK_EPOXY_CRACK_BOUNDARY_WIDTH, m_iEditNeckEpoxyCrackBoundaryWidth);
	DDX_Radio(pDX, IDC_RADIO_NECK_EPOXY_LEFT, m_iRadioNeckEpoxyCrackDir);
	DDX_Check(pDX, IDC_CHECK_NECK_EPOXY_CRACK_XDIR_MODE, m_bCheckNeckEpoxyCrackUseXDirMode);
}


BEGIN_MESSAGE_MAP(CAlgorithmNeckEpoxyCrackDlg, CDialog)
	ON_BN_CLICKED(IDC_MFCBUTTON_NEXT_PROCESSING2, &CAlgorithmNeckEpoxyCrackDlg::OnBnClickedMfcbuttonNextProcessing2)
END_MESSAGE_MAP()


// CAlgorithmNeckEpoxyCrackDlg 메시지 처리기입니다.

void CAlgorithmNeckEpoxyCrackDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmNeckEpoxyCrackDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseNeckEpoxyCrack = AlgorithmParam.m_bUseNeckEpoxyCrack;
	m_iEditNeckEpoxyCrackOpeningSize = AlgorithmParam.m_iNeckEpoxyCrackOpeningSize;
	m_iEditNeckEpoxyCrackYLength = AlgorithmParam.m_iNeckEpoxyCrackYLength;
	m_iEditNeckEpoxyCrackBoundaryWidth = AlgorithmParam.m_iNeckEpoxyCrackBoundaryWidth;
	m_iRadioNeckEpoxyCrackDir = AlgorithmParam.m_iNeckEpoxyCrackDir;
	m_bCheckNeckEpoxyCrackUseXDirMode = AlgorithmParam.m_bNeckEpoxyCrackUseXDirMode;


	UpdateData(FALSE);
}

void CAlgorithmNeckEpoxyCrackDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseNeckEpoxyCrack = m_bCheckUseNeckEpoxyCrack;
	pAlgorithmParam->m_iNeckEpoxyCrackOpeningSize = m_iEditNeckEpoxyCrackOpeningSize;
	pAlgorithmParam->m_iNeckEpoxyCrackYLength = m_iEditNeckEpoxyCrackYLength;
	pAlgorithmParam->m_iNeckEpoxyCrackBoundaryWidth = m_iEditNeckEpoxyCrackBoundaryWidth;
	pAlgorithmParam->m_iNeckEpoxyCrackDir = m_iRadioNeckEpoxyCrackDir;
	pAlgorithmParam->m_bNeckEpoxyCrackUseXDirMode = m_bCheckNeckEpoxyCrackUseXDirMode;

}

BOOL CAlgorithmNeckEpoxyCrackDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAlgorithmNeckEpoxyCrackDlg::OnBnClickedMfcbuttonNextProcessing2()
{
	THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->UpdateAlgorithmTab(8);
}
