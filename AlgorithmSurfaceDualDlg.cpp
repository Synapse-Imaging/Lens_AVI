// AlgorithmSurfaceDualDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmSurfaceDualDlg.h"
#include "afxdialogex.h"


// CAlgorithmSurfaceDualDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmSurfaceDualDlg, CDialog)

CAlgorithmSurfaceDualDlg::CAlgorithmSurfaceDualDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmSurfaceDualDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseSurfaceDualInspection = FALSE;

	m_iRadioSurfaceDualDefectType_1 = 0;
	m_iRadioSurfaceDualDefectType_2 = 0;

	m_bCheckUseBrightFixedThres_1 = FALSE;
	m_iEditBrightLowerThres_1 = 0;
	m_iEditBrightUpperThres_1 = 0;
	m_bCheckUseBrightDT_1 = FALSE;
	m_iEditBrightMedianFilterSize_1 = 0;
	m_iEditBrightClosingFilterSize_1 = 0;
	m_iRadioBrightDTFilterType_1 = 0;
	m_iEditBrightDTFilterSize_1 = 0;
	m_iEditBrightDTFilterSize2_1 = 0;
	m_iEditBrightDTValue_1 = 0;
	m_bCheckUseDTPolarTrans_1 = FALSE;
	m_bCheckUseBrightHystThres_1 = FALSE;
	m_iEditBrightHystSecureThres_1 = 190;
	m_iEditBrightHystPotentialThres_1 = 175;
	m_iEditBrightHystPotentialLength_1 = 5;

	m_bCheckUseBrightFixedThres_2 = FALSE;
	m_iEditBrightLowerThres_2 = 0;
	m_iEditBrightUpperThres_2 = 0;
	m_bCheckUseBrightDT_2 = FALSE;
	m_iEditBrightMedianFilterSize_2 = 0;
	m_iEditBrightClosingFilterSize_2 = 0;
	m_iRadioBrightDTFilterType_2 = 0;
	m_iEditBrightDTFilterSize_2 = 0;
	m_iEditBrightDTFilterSize2_2 = 0;
	m_iEditBrightDTValue_2 = 0;
	m_bCheckUseDTPolarTrans_2 = FALSE;
	m_bCheckUseBrightHystThres_2 = FALSE;
	m_iEditBrightHystSecureThres_2 = 190;
	m_iEditBrightHystPotentialThres_2 = 175;
	m_iEditBrightHystPotentialLength_2 = 5;

	m_iRadioDualBlobProcessType = 0;
	m_bCheckDualBlobDefectConditionEach = FALSE;
}

CAlgorithmSurfaceDualDlg::~CAlgorithmSurfaceDualDlg()
{
	for (int i = 0; i < CHANNEL_NUM; i++)
		GenEmptyObj(&(m_HOrgScanImage[i]));
}

void CAlgorithmSurfaceDualDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_SURFACE_DUAL_INSPECTION, m_bCheckUseSurfaceDualInspection);

	DDX_Control(pDX, IDC_COMBO_IMAGE_1, m_cbSurfaceDualImageIndex_1);
	DDX_Control(pDX, IDC_COMBO_IMAGE_2, m_cbSurfaceDualImageIndex_2);

	DDX_Radio(pDX, IDC_RADIO_DEFECT_TYPE_BRIGHT_1, m_iRadioSurfaceDualDefectType_1);
	DDX_Radio(pDX, IDC_RADIO_DEFECT_TYPE_BRIGHT_2, m_iRadioSurfaceDualDefectType_2);

	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_FIXED_THRESHOLD_1, m_bCheckUseBrightFixedThres_1);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_LOWER_THRES_1, m_iEditBrightLowerThres_1);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UPPER_THRES_1, m_iEditBrightUpperThres_1);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_DT_1, m_bCheckUseBrightDT_1);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_MED_FILTER_SIZE_1, m_iEditBrightMedianFilterSize_1);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_CLOSING_FILTER_SIZE_1, m_iEditBrightClosingFilterSize_1);
	DDX_Radio(pDX, IDC_RADIO_BRIGHT_DT_FILTER_TYPE_MEAN_1, m_iRadioBrightDTFilterType_1);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_FILTER_SIZE_1, m_iEditBrightDTFilterSize_1);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_FILTER_SIZE2_1, m_iEditBrightDTFilterSize2_1);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_VALUE_1, m_iEditBrightDTValue_1);
	DDX_Check(pDX, IDC_CHECK_USE_DT_POLAR_TRANS_1, m_bCheckUseDTPolarTrans_1);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_HYST_1, m_bCheckUseBrightHystThres_1);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_SECURE_THRES_1, m_iEditBrightHystSecureThres_1);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_POTENTIAL_THRES_1, m_iEditBrightHystPotentialThres_1);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_POTENTIAL_LENGTH_1, m_iEditBrightHystPotentialLength_1);

	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_FIXED_THRESHOLD_2, m_bCheckUseBrightFixedThres_2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_LOWER_THRES_2, m_iEditBrightLowerThres_2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UPPER_THRES_2, m_iEditBrightUpperThres_2);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_DT_2, m_bCheckUseBrightDT_2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_MED_FILTER_SIZE_2, m_iEditBrightMedianFilterSize_2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_CLOSING_FILTER_SIZE_2, m_iEditBrightClosingFilterSize_2);
	DDX_Radio(pDX, IDC_RADIO_BRIGHT_DT_FILTER_TYPE_MEAN_2, m_iRadioBrightDTFilterType_2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_FILTER_SIZE_2, m_iEditBrightDTFilterSize_2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_FILTER_SIZE2_2, m_iEditBrightDTFilterSize2_2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_VALUE_2, m_iEditBrightDTValue_2);
	DDX_Check(pDX, IDC_CHECK_USE_DT_POLAR_TRANS_2, m_bCheckUseDTPolarTrans_2);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_HYST_2, m_bCheckUseBrightHystThres_2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_SECURE_THRES_2, m_iEditBrightHystSecureThres_2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_POTENTIAL_THRES_2, m_iEditBrightHystPotentialThres_2);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_POTENTIAL_LENGTH_2, m_iEditBrightHystPotentialLength_2);

	DDX_Radio(pDX, IDC_RADIO_DUAL_BLOB_PROCESS_TYPE_INTERSECTION, m_iRadioDualBlobProcessType);
	DDX_Check(pDX, IDC_CHECK_DUAL_BLOB_DEFECT_CONDITION_EACH, m_bCheckDualBlobDefectConditionEach);
}


BEGIN_MESSAGE_MAP(CAlgorithmSurfaceDualDlg, CDialog)
	ON_BN_CLICKED(IDC_MFCBUTTON_NEXT_PROCESSING, &CAlgorithmSurfaceDualDlg::OnBnClickedMfcbuttonNextProcessing)
END_MESSAGE_MAP()


// CAlgorithmSurfaceDualDlg 메시지 처리기입니다.

BOOL CAlgorithmSurfaceDualDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;
	CString sTemp;

	m_cbSurfaceDualImageIndex_1.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbSurfaceDualImageIndex_1.AddString(sTemp);
	}

	m_cbSurfaceDualImageIndex_2.ResetContent();
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("영상 %d", i + 1);
		m_cbSurfaceDualImageIndex_2.AddString(sTemp);
	}

	m_cbSurfaceDualImageIndex_1.SetCurSel(0);
	m_cbSurfaceDualImageIndex_2.SetCurSel(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmSurfaceDualDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;

	for (int i = 0; i < CHANNEL_NUM; i++)
		CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][m_iSelectedImageType], &(m_HOrgScanImage[i]));
}

void CAlgorithmSurfaceDualDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseSurfaceDualInspection = AlgorithmParam.m_bUseSurfaceDualInspection;
	m_cbSurfaceDualImageIndex_1.SetCurSel(AlgorithmParam.m_iSurfaceDualImageIndex_1);
	m_cbSurfaceDualImageIndex_2.SetCurSel(AlgorithmParam.m_iSurfaceDualImageIndex_2);

	m_iRadioSurfaceDualDefectType_1 = AlgorithmParam.m_iSurfaceDualDefectType_1;
	m_iRadioSurfaceDualDefectType_2 = AlgorithmParam.m_iSurfaceDualDefectType_2;

	m_bCheckUseBrightFixedThres_1 = AlgorithmParam.m_bUseBrightFixedThres_1;
	m_iEditBrightLowerThres_1 = AlgorithmParam.m_iBrightLowerThres_1;
	m_iEditBrightUpperThres_1 = AlgorithmParam.m_iBrightUpperThres_1;
	m_bCheckUseBrightDT_1 = AlgorithmParam.m_bUseBrightDT_1;
	m_iEditBrightMedianFilterSize_1 = AlgorithmParam.m_iBrightMedianFilterSize_1;
	m_iEditBrightClosingFilterSize_1 = AlgorithmParam.m_iBrightClosingFilterSize_1;
	m_iRadioBrightDTFilterType_1 = AlgorithmParam.m_iBrightDTFilterType_1;
	m_iEditBrightDTFilterSize_1 = AlgorithmParam.m_iBrightDTFilterSize_1;
	m_iEditBrightDTFilterSize2_1 = AlgorithmParam.m_iBrightDTFilterSize2_1;
	m_iEditBrightDTValue_1 = AlgorithmParam.m_iBrightDTValue_1;
	m_bCheckUseDTPolarTrans_1 = AlgorithmParam.m_bUseDTPolarTrans_1;
	m_bCheckUseBrightHystThres_1 = AlgorithmParam.m_bUseBrightHystThres_1;
	m_iEditBrightHystSecureThres_1 = AlgorithmParam.m_iBrightHystSecureThres_1;
	m_iEditBrightHystPotentialThres_1 = AlgorithmParam.m_iBrightHystPotentialThres_1;
	m_iEditBrightHystPotentialLength_1 = AlgorithmParam.m_iBrightHystPotentialLength_1;

	m_bCheckUseBrightFixedThres_2 = AlgorithmParam.m_bUseBrightFixedThres_2;
	m_iEditBrightLowerThres_2 = AlgorithmParam.m_iBrightLowerThres_2;
	m_iEditBrightUpperThres_2 = AlgorithmParam.m_iBrightUpperThres_2;
	m_bCheckUseBrightDT_2 = AlgorithmParam.m_bUseBrightDT_2;
	m_iEditBrightMedianFilterSize_2 = AlgorithmParam.m_iBrightMedianFilterSize_2;
	m_iEditBrightClosingFilterSize_2 = AlgorithmParam.m_iBrightClosingFilterSize_2;
	m_iRadioBrightDTFilterType_2 = AlgorithmParam.m_iBrightDTFilterType_2;
	m_iEditBrightDTFilterSize_2 = AlgorithmParam.m_iBrightDTFilterSize_2;
	m_iEditBrightDTFilterSize2_2 = AlgorithmParam.m_iBrightDTFilterSize2_2;
	m_iEditBrightDTValue_2 = AlgorithmParam.m_iBrightDTValue_2;
	m_bCheckUseDTPolarTrans_2 = AlgorithmParam.m_bUseDTPolarTrans_2;
	m_bCheckUseBrightHystThres_2 = AlgorithmParam.m_bUseBrightHystThres_2;
	m_iEditBrightHystSecureThres_2 = AlgorithmParam.m_iBrightHystSecureThres_2;
	m_iEditBrightHystPotentialThres_2 = AlgorithmParam.m_iBrightHystPotentialThres_2;
	m_iEditBrightHystPotentialLength_2 = AlgorithmParam.m_iBrightHystPotentialLength_2;

	m_iRadioDualBlobProcessType = AlgorithmParam.m_iDualBlobProcessType;
	m_bCheckDualBlobDefectConditionEach = AlgorithmParam.m_bDualBlobDefectConditionEach;

	UpdateData(FALSE);
}

void CAlgorithmSurfaceDualDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseSurfaceDualInspection = m_bCheckUseSurfaceDualInspection;
	pAlgorithmParam->m_iSurfaceDualImageIndex_1 = m_cbSurfaceDualImageIndex_1.GetCurSel();
	pAlgorithmParam->m_iSurfaceDualImageIndex_2 = m_cbSurfaceDualImageIndex_2.GetCurSel();

	pAlgorithmParam->m_iSurfaceDualDefectType_1 = m_iRadioSurfaceDualDefectType_1;
	pAlgorithmParam->m_iSurfaceDualDefectType_2 = m_iRadioSurfaceDualDefectType_2;

	pAlgorithmParam->m_bUseBrightFixedThres_1 = m_bCheckUseBrightFixedThres_1;
	pAlgorithmParam->m_iBrightLowerThres_1 = m_iEditBrightLowerThres_1;
	pAlgorithmParam->m_iBrightUpperThres_1 = m_iEditBrightUpperThres_1;
	pAlgorithmParam->m_bUseBrightDT_1 = m_bCheckUseBrightDT_1;
	pAlgorithmParam->m_iBrightMedianFilterSize_1 = m_iEditBrightMedianFilterSize_1;
	pAlgorithmParam->m_iBrightClosingFilterSize_1 = m_iEditBrightClosingFilterSize_1;
	pAlgorithmParam->m_iBrightDTFilterType_1 = m_iRadioBrightDTFilterType_1;
	pAlgorithmParam->m_iBrightDTFilterSize_1 = m_iEditBrightDTFilterSize_1;
	pAlgorithmParam->m_iBrightDTFilterSize2_1 = m_iEditBrightDTFilterSize2_1;
	pAlgorithmParam->m_iBrightDTValue_1 = m_iEditBrightDTValue_1;
	pAlgorithmParam->m_bUseDTPolarTrans_1 = m_bCheckUseDTPolarTrans_1;
	pAlgorithmParam->m_bUseBrightHystThres_1 = m_bCheckUseBrightHystThres_1;
	pAlgorithmParam->m_iBrightHystSecureThres_1 = m_iEditBrightHystSecureThres_1;
	pAlgorithmParam->m_iBrightHystPotentialThres_1 = m_iEditBrightHystPotentialThres_1;
	pAlgorithmParam->m_iBrightHystPotentialLength_1 = m_iEditBrightHystPotentialLength_1;

	pAlgorithmParam->m_bUseBrightFixedThres_2 = m_bCheckUseBrightFixedThres_2;
	pAlgorithmParam->m_iBrightLowerThres_2 = m_iEditBrightLowerThres_2;
	pAlgorithmParam->m_iBrightUpperThres_2 = m_iEditBrightUpperThres_2;
	pAlgorithmParam->m_bUseBrightDT_2 = m_bCheckUseBrightDT_2;
	pAlgorithmParam->m_iBrightMedianFilterSize_2 = m_iEditBrightMedianFilterSize_2;
	pAlgorithmParam->m_iBrightClosingFilterSize_2 = m_iEditBrightClosingFilterSize_2;
	pAlgorithmParam->m_iBrightDTFilterType_2 = m_iRadioBrightDTFilterType_2;
	pAlgorithmParam->m_iBrightDTFilterSize_2 = m_iEditBrightDTFilterSize_2;
	pAlgorithmParam->m_iBrightDTFilterSize2_2 = m_iEditBrightDTFilterSize2_2;
	pAlgorithmParam->m_iBrightDTValue_2 = m_iEditBrightDTValue_2;
	pAlgorithmParam->m_bUseDTPolarTrans_2 = m_bCheckUseDTPolarTrans_2;
	pAlgorithmParam->m_bUseBrightHystThres_2 = m_bCheckUseBrightHystThres_2;
	pAlgorithmParam->m_iBrightHystSecureThres_2 = m_iEditBrightHystSecureThres_2;
	pAlgorithmParam->m_iBrightHystPotentialThres_2 = m_iEditBrightHystPotentialThres_2;
	pAlgorithmParam->m_iBrightHystPotentialLength_2 = m_iEditBrightHystPotentialLength_2;

	pAlgorithmParam->m_iDualBlobProcessType = m_iRadioDualBlobProcessType;
	pAlgorithmParam->m_bDualBlobDefectConditionEach = m_bCheckDualBlobDefectConditionEach;
}

void CAlgorithmSurfaceDualDlg::OnBnClickedMfcbuttonNextProcessing()
{
	THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->UpdateAlgorithmTab(10);
}
