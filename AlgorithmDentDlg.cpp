// AlgorithmDentDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmDentDlg.h"
#include "afxdialogex.h"


// CAlgorithmDentDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmDentDlg, CDialog)

CAlgorithmDentDlg::CAlgorithmDentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmDentDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseDent = FALSE;
	m_iEditDentMedFilterSize = 0;
	m_iEditDentDtFilterSize = 0;
	m_iEditDentBrightDtValue = 0;
	m_iEditDentDarkDtValue = 0;
	m_iEditDentCandidateMinArea = 0;
	m_iEditDentCandidateMinWidth = 0;
	m_iEditDentCandidateMinDistance = 0;
	m_iEditDentMinArea = 0;
}

CAlgorithmDentDlg::~CAlgorithmDentDlg()
{
}

void CAlgorithmDentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_DENT, m_bCheckUseDent);
	DDX_Text(pDX, IDC_EDIT_DENT_MED_FILTER_SIZE, m_iEditDentMedFilterSize);
	DDX_Text(pDX, IDC_EDIT_DENT_DT_FILTER_SIZE, m_iEditDentDtFilterSize);
	DDX_Text(pDX, IDC_EDIT_DENT_BRIGHT_DT_VALUE, m_iEditDentBrightDtValue);
	DDX_Text(pDX, IDC_EDIT_DENT_DARK_DT_VALUE, m_iEditDentDarkDtValue);
	DDX_Text(pDX, IDC_EDIT_DENT_CANDIDATE_MIN_AREA, m_iEditDentCandidateMinArea);
	DDX_Text(pDX, IDC_EDIT_DENT_CANDIDATE_MIN_WIDTH, m_iEditDentCandidateMinWidth);
	DDX_Text(pDX, IDC_EDIT_DENT_CANDIDATE_MIN_DISTANCE, m_iEditDentCandidateMinDistance);
	DDX_Text(pDX, IDC_EDIT_DENT_MIN_AREA, m_iEditDentMinArea);
}


BEGIN_MESSAGE_MAP(CAlgorithmDentDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmDentDlg 메시지 처리기입니다.

void CAlgorithmDentDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmDentDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseDent = AlgorithmParam.m_bUseDent;
	m_iEditDentMedFilterSize = AlgorithmParam.m_iDentMedFilterSize;
	m_iEditDentDtFilterSize = AlgorithmParam.m_iDentDtFilterSize;
	m_iEditDentBrightDtValue = AlgorithmParam.m_iDentBrightDtValue;
	m_iEditDentDarkDtValue = AlgorithmParam.m_iDentDarkDtValue;
	m_iEditDentCandidateMinArea = AlgorithmParam.m_iDentCandidateMinArea;
	m_iEditDentCandidateMinWidth = AlgorithmParam.m_iDentCandidateMinWidth;
	m_iEditDentCandidateMinDistance = AlgorithmParam.m_iDentCandidateMinDistance;
	m_iEditDentMinArea = AlgorithmParam.m_iDentMinArea;

	UpdateData(FALSE);
}

void CAlgorithmDentDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseDent = m_bCheckUseDent;
	pAlgorithmParam->m_iDentMedFilterSize = m_iEditDentMedFilterSize;
	pAlgorithmParam->m_iDentDtFilterSize = m_iEditDentDtFilterSize;
	pAlgorithmParam->m_iDentBrightDtValue = m_iEditDentBrightDtValue;
	pAlgorithmParam->m_iDentDarkDtValue = m_iEditDentDarkDtValue;
	pAlgorithmParam->m_iDentCandidateMinArea = m_iEditDentCandidateMinArea;
	pAlgorithmParam->m_iDentCandidateMinWidth = m_iEditDentCandidateMinWidth;
	pAlgorithmParam->m_iDentCandidateMinDistance = m_iEditDentCandidateMinDistance;
	pAlgorithmParam->m_iDentMinArea = m_iEditDentMinArea;
}

BOOL CAlgorithmDentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
