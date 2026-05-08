// AlgorithmEpoxyVoidHoleDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmEpoxyVoidHoleDlg.h"
#include "afxdialogex.h"


// CAlgorithmEpoxyVoidHoleDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmEpoxyVoidHoleDlg, CDialog)

CAlgorithmEpoxyVoidHoleDlg::CAlgorithmEpoxyVoidHoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmEpoxyVoidHoleDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseEpoxyVoidHole = FALSE;
	m_dEditEpoxyVoidHoleEdgeStr = 1.0;
	m_iEditEpoxyVoidHoleInspectMargin = 0;
	m_bCheckEpoxyVoidHoleTiltInspect = FALSE;
	m_iEditEpoxyVoidHoleTiltTolerance = 30;
	m_bCheckEpoxyVoidHoleGapInspect = FALSE;
	m_iEditEpoxyVoidHoleGapTolerance = 5;
}

CAlgorithmEpoxyVoidHoleDlg::~CAlgorithmEpoxyVoidHoleDlg()
{
}

void CAlgorithmEpoxyVoidHoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_EPOXY_VOID_HOLE, m_bCheckUseEpoxyVoidHole);
	DDX_Control(pDX, IDC_COMBO_EPOXY_VOID_HOLE_EDGE_IMAGE, m_cbEpoxyVoidHoleImage);
	DDX_Text(pDX, IDC_EDIT_EPOXY_VOID_HOLE_EDGE_STRENGTH, m_dEditEpoxyVoidHoleEdgeStr);
	DDX_Text(pDX, IDC_EDIT_EPOXY_VOID_HOLE_INSPECT_MARGIN, m_iEditEpoxyVoidHoleInspectMargin);
	DDX_Check(pDX, IDC_CHECK_EPOXY_VOID_HOLE_TILT_INSPECT, m_bCheckEpoxyVoidHoleTiltInspect);
	DDX_Text(pDX, IDC_EDIT_EPOXY_VOID_HOLE_TILT_TOLERANCE, m_iEditEpoxyVoidHoleTiltTolerance);
	DDX_Check(pDX, IDC_CHECK_EPOXY_VOID_HOLE_GAP_INSPECT, m_bCheckEpoxyVoidHoleGapInspect);
	DDX_Text(pDX, IDC_EDIT_EPOXY_VOID_HOLE_GAP_TOLERANCE, m_iEditEpoxyVoidHoleGapTolerance);
}


BEGIN_MESSAGE_MAP(CAlgorithmEpoxyVoidHoleDlg, CDialog)
	ON_BN_CLICKED(IDC_MFCBUTTON_NEXT_PROCESSING, &CAlgorithmEpoxyVoidHoleDlg::OnBnClickedMfcbuttonNextProcessing)
END_MESSAGE_MAP()


// CAlgorithmEpoxyVoidHoleDlg 메시지 처리기입니다.

void CAlgorithmEpoxyVoidHoleDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmEpoxyVoidHoleDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseEpoxyVoidHole = AlgorithmParam.m_bUseEpoxyVoidHole;
	m_cbEpoxyVoidHoleImage.SetCurSel(AlgorithmParam.m_iEpoxyVoidHoleImage);

	m_dEditEpoxyVoidHoleEdgeStr = AlgorithmParam.m_dEpoxyVoidHoleEdgeStr;
	m_iEditEpoxyVoidHoleInspectMargin = AlgorithmParam.m_iEpoxyVoidHoleInspectMargin;
	m_bCheckEpoxyVoidHoleTiltInspect = AlgorithmParam.m_bEpoxyVoidHoleTiltInspect;
	m_iEditEpoxyVoidHoleTiltTolerance = AlgorithmParam.m_iEpoxyVoidHoleTiltTolerance;
	m_bCheckEpoxyVoidHoleGapInspect = AlgorithmParam.m_bEpoxyVoidHoleGapInspect;
	m_iEditEpoxyVoidHoleGapTolerance = AlgorithmParam.m_iEpoxyVoidHoleGapTolerance;

	UpdateData(FALSE);
}

void CAlgorithmEpoxyVoidHoleDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseEpoxyVoidHole = m_bCheckUseEpoxyVoidHole;
	pAlgorithmParam->m_iEpoxyVoidHoleImage = m_cbEpoxyVoidHoleImage.GetCurSel();

	pAlgorithmParam->m_dEpoxyVoidHoleEdgeStr = m_dEditEpoxyVoidHoleEdgeStr;
	pAlgorithmParam->m_iEpoxyVoidHoleInspectMargin = m_iEditEpoxyVoidHoleInspectMargin;
	pAlgorithmParam->m_bEpoxyVoidHoleTiltInspect = m_bCheckEpoxyVoidHoleTiltInspect;
	pAlgorithmParam->m_iEpoxyVoidHoleTiltTolerance = m_iEditEpoxyVoidHoleTiltTolerance;
	pAlgorithmParam->m_bEpoxyVoidHoleGapInspect = m_bCheckEpoxyVoidHoleGapInspect;
	pAlgorithmParam->m_iEpoxyVoidHoleGapTolerance = m_iEditEpoxyVoidHoleGapTolerance;
}


BOOL CAlgorithmEpoxyVoidHoleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;
	CString sTemp;

	m_cbEpoxyVoidHoleImage.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbEpoxyVoidHoleImage.AddString(sTemp);
	}

	m_cbEpoxyVoidHoleImage.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmEpoxyVoidHoleDlg::OnBnClickedMfcbuttonNextProcessing()
{
	THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->UpdateAlgorithmTab(8);
}
