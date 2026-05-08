// AlgorithmTiltDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmTiltDlg.h"
#include "afxdialogex.h"


// CAlgorithmTiltDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmTiltDlg, CDialog)

CAlgorithmTiltDlg::CAlgorithmTiltDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmTiltDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseHomerTilt = FALSE;
	m_iEditHomerTiltHomerEdgeStr = 15;
	m_iEditHomerTiltBenvolioEdgeStr = 15;
	m_dEditHomerTiltHomerLength = 0;
	m_dEditHomerTiltTol = 0;
	m_dEditHomerTiltDistanceTol = 0;
}

CAlgorithmTiltDlg::~CAlgorithmTiltDlg()
{
}

void CAlgorithmTiltDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_HOMER_TILT, m_bCheckUseHomerTilt);
	DDX_Text(pDX, IDC_EDIT_HOMER_TILT_HOMER_EDGE_STRENGTH, m_iEditHomerTiltHomerEdgeStr);
	DDX_Control(pDX, IDC_COMBO_HOMER_TILT_BENVOLIO_IMAGE, m_cbHomerTiltBenvolioImage);
	DDX_Text(pDX, IDC_EDIT_HOMER_TILT_BENVOLIO_EDGE_STRENGTH, m_iEditHomerTiltBenvolioEdgeStr);
	DDX_Text(pDX, IDC_EDIT_HOMER_TILT_HOMER_LENGTH, m_dEditHomerTiltHomerLength);
	DDX_Text(pDX, IDC_EDIT_HOMER_TILT_TOLERANCE, m_dEditHomerTiltTol);
	DDX_Text(pDX, IDC_EDIT_HOMER_TILT_DISTANCE_TOLERANCE, m_dEditHomerTiltDistanceTol);
}


BEGIN_MESSAGE_MAP(CAlgorithmTiltDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmTiltDlg 메시지 처리기입니다.

void CAlgorithmTiltDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmTiltDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseHomerTilt = AlgorithmParam.m_bUseHomerTilt;
	m_iEditHomerTiltHomerEdgeStr = AlgorithmParam.m_iHomerTiltHomerEdgeStr;
	m_cbHomerTiltBenvolioImage.SetCurSel(AlgorithmParam.m_iHomerTiltBenvolioImage);
	m_iEditHomerTiltBenvolioEdgeStr = AlgorithmParam.m_iHomerTiltBenvolioEdgeStr;
	m_dEditHomerTiltHomerLength = AlgorithmParam.m_dHomerTiltHomerLength;
	m_dEditHomerTiltTol = AlgorithmParam.m_dHomerTiltTol;
	m_dEditHomerTiltDistanceTol = AlgorithmParam.m_dHomerTiltDistanceTol;

	UpdateData(FALSE);
}

void CAlgorithmTiltDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseHomerTilt = m_bCheckUseHomerTilt;
	pAlgorithmParam->m_iHomerTiltHomerEdgeStr = m_iEditHomerTiltHomerEdgeStr;
	pAlgorithmParam->m_iHomerTiltBenvolioImage = m_cbHomerTiltBenvolioImage.GetCurSel();
	pAlgorithmParam->m_iHomerTiltBenvolioEdgeStr = m_iEditHomerTiltBenvolioEdgeStr;
	pAlgorithmParam->m_dHomerTiltHomerLength = m_dEditHomerTiltHomerLength;
	pAlgorithmParam->m_dHomerTiltTol = m_dEditHomerTiltTol;
	pAlgorithmParam->m_dHomerTiltDistanceTol = m_dEditHomerTiltDistanceTol;
}


BOOL CAlgorithmTiltDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;
	CString sTemp;

	m_cbHomerTiltBenvolioImage.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbHomerTiltBenvolioImage.AddString(sTemp);
	}

	m_cbHomerTiltBenvolioImage.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
