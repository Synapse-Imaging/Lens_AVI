// AlgorithmHeightMeasureDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmHeightMeasureDlg.h"
#include "afxdialogex.h"


// CAlgorithmHeightMeasureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmHeightMeasureDlg, CDialog)

CAlgorithmHeightMeasureDlg::CAlgorithmHeightMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmHeightMeasureDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseHeightMeasure = FALSE;
	m_iRadioHeightMeasureType = 0;
}

CAlgorithmHeightMeasureDlg::~CAlgorithmHeightMeasureDlg()
{
}

void CAlgorithmHeightMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_HEIGHT_MEASURE, m_bCheckUseHeightMeasure);
	DDX_Radio(pDX, IDC_RADIO_HEIGHT_MEASURE_TYPE_1, m_iRadioHeightMeasureType);
}


BEGIN_MESSAGE_MAP(CAlgorithmHeightMeasureDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmHeightMeasureDlg 메시지 처리기입니다.

void CAlgorithmHeightMeasureDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmHeightMeasureDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseHeightMeasure = AlgorithmParam.m_bUseHeightMeasure;
	m_iRadioHeightMeasureType = AlgorithmParam.m_iHeightMeasureType;

	UpdateData(FALSE);
}

void CAlgorithmHeightMeasureDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseHeightMeasure = m_bCheckUseHeightMeasure;
	pAlgorithmParam->m_iHeightMeasureType = m_iRadioHeightMeasureType;
}

BOOL CAlgorithmHeightMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
