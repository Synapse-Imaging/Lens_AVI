// AlgorithmPrintQualityDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmPrintQualityDlg.h"
#include "afxdialogex.h"


// CAlgorithmPrintQualityDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmPrintQualityDlg, CDialog)

CAlgorithmPrintQualityDlg::CAlgorithmPrintQualityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmPrintQualityDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUsePrintQuality = FALSE;

	m_iEditPrintQualityContrastMin = 14;

	m_iEditPrintQualitySymbolColsMin = 18;
	m_iEditPrintQualitySymbolRowsMin = 18;
	m_iEditPrintQualitySymbolColsMax = 18;
	m_iEditPrintQualitySymbolRowsMax = 18;

	m_iEditPrintQualityModuleSizeMin = 2;
	m_iEditPrintQualityModuleSizeMax = 18;

	m_dEditPrintQualitySlantMax = 0.0944843;

	m_bCheckPrintQualityOverallQuality = TRUE;
	m_iEditPrintQualityGradeOverallQuality = 1;
	m_bCheckPrintQualityUnusedErrorCorrection = TRUE;
	m_iEditPrintQualityGradeUnusedErrorCorrection = 1;
}

CAlgorithmPrintQualityDlg::~CAlgorithmPrintQualityDlg()
{
}

void CAlgorithmPrintQualityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_PRINT_QUALITY, m_bCheckUsePrintQuality);

	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_STRICT_MODEL, m_cbPrintQualityStrictModel);
	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_POLARITY, m_cbPrintQualityPolarity);
	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_MIRRORED, m_cbPrintQualityMirrored);

	DDX_Text(pDX, IDC_EDIT_PRINT_QUALITY_CONTRAST_MIN, m_iEditPrintQualityContrastMin);

	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_CONTRAST_TOLERANCE, m_cbPrintQualityContrastTolerance);
	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_STRICT_QUIET_ZONE, m_cbPrintQualityStrictQuietZone);

	DDX_Text(pDX, IDC_EDIT_PRINT_QUALITY_SYMBOL_COLS_MIN, m_iEditPrintQualitySymbolColsMin);
	DDX_Text(pDX, IDC_EDIT_PRINT_QUALITY_SYMBOL_ROWS_MIN, m_iEditPrintQualitySymbolRowsMin);
	DDX_Text(pDX, IDC_EDIT_PRINT_QUALITY_SYMBOL_COLS_MAX, m_iEditPrintQualitySymbolColsMax);
	DDX_Text(pDX, IDC_EDIT_PRINT_QUALITY_SYMBOL_ROWS_MAX, m_iEditPrintQualitySymbolRowsMax);

	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_SYMBOL_SHAPE, m_cbPrintQualitySymbolShape);

	DDX_Text(pDX, IDC_EDIT_PRINT_QUALITY_MODULE_SIZE_MIN, m_iEditPrintQualityModuleSizeMin);
	DDX_Text(pDX, IDC_EDIT_PRINT_QUALITY_MODULE_SIZE_MAX, m_iEditPrintQualityModuleSizeMax);

	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_SMALL_MODULES_ROBUSTNESS, m_cbPrintQualitySmallModulesRobustness);
	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_MODULE_GRID, m_cbPrintQualityModuleGrid);
	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_MODULE_GAP_MIN, m_cbPrintQualityGapMin);
	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_MODULE_GAP_MAX, m_cbPrintQualityGapMax);
	DDX_Control(pDX, IDC_COMBO_PRINT_QUALITY_FINDER_PATTERN_TOLERANCE, m_cbPrintQualityFinderPatternTolerance);

	DDX_Text(pDX, IDC_EDIT_PRINT_QUALITY_SLANT_MAX, m_dEditPrintQualitySlantMax);

	DDX_Check(pDX, IDC_CHECK_PRINT_QUALITY_FIXED_PATTERN_DAMAGE, m_bCheckPrintQualityOverallQuality);
	DDX_Text(pDX, IDC_EDIT_PRINT_QUALITY_GRADE_FIXED_PATTERN_DAMAGE, m_iEditPrintQualityGradeOverallQuality);

	DDX_Check(pDX, IDC_CHECK_PRINT_QUALITY_UNUSED_ERROR_CORRECTION, m_bCheckPrintQualityUnusedErrorCorrection);
	DDX_Text(pDX, IDC_EDIT_PRINT_QUALITY_GRADE_UNUSED_ERROR_CORRECTION, m_iEditPrintQualityGradeUnusedErrorCorrection);

}


BEGIN_MESSAGE_MAP(CAlgorithmPrintQualityDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmPrintQualityDlg 메시지 처리기입니다.

void CAlgorithmPrintQualityDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmPrintQualityDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUsePrintQuality = AlgorithmParam.m_bUsePrintQuality;

	m_iEditPrintQualityContrastMin = AlgorithmParam.m_iPrintQualityContrastMin;

	m_iEditPrintQualitySymbolColsMin = AlgorithmParam.m_iPrintQualitySymbolColsMin;
	m_iEditPrintQualitySymbolRowsMin = AlgorithmParam.m_iPrintQualitySymbolRowsMin;
	m_iEditPrintQualitySymbolColsMax = AlgorithmParam.m_iPrintQualitySymbolColsMax;
	m_iEditPrintQualitySymbolRowsMax = AlgorithmParam.m_iPrintQualitySymbolRowsMax;

	m_iEditPrintQualityModuleSizeMin = AlgorithmParam.m_iPrintQualityModuleSizeMin;
	m_iEditPrintQualityModuleSizeMax = AlgorithmParam.m_iPrintQualityModuleSizeMax;

	m_dEditPrintQualitySlantMax = AlgorithmParam.m_dPrintQualitySlantMax;

	m_bCheckPrintQualityOverallQuality = AlgorithmParam.m_bPrintQualityOverallQuality;
	m_iEditPrintQualityGradeOverallQuality = AlgorithmParam.m_iPrintQualityGradeOverallQuality;
	m_bCheckPrintQualityUnusedErrorCorrection = AlgorithmParam.m_bPrintQualityUnusedErrorCorrection;
	m_iEditPrintQualityGradeUnusedErrorCorrection = AlgorithmParam.m_iPrintQualityGradeUnusedErrorCorrection;

	m_cbPrintQualityStrictModel.SetCurSel(AlgorithmParam.m_iPrintQualityStrictModel);
	m_cbPrintQualityPolarity.SetCurSel(AlgorithmParam.m_iPrintQualityPolarity);
	m_cbPrintQualityMirrored.SetCurSel(AlgorithmParam.m_iPrintQualityMirrored);
	m_cbPrintQualityContrastTolerance.SetCurSel(AlgorithmParam.m_iPrintQualityContrastTolerance);
	m_cbPrintQualityStrictQuietZone.SetCurSel(AlgorithmParam.m_iPrintQualityStrictQuietZone);
	m_cbPrintQualitySymbolShape.SetCurSel(AlgorithmParam.m_iPrintQualitySymbolShape);
	m_cbPrintQualitySmallModulesRobustness.SetCurSel(AlgorithmParam.m_iPrintQualitySmallModulesRobustness);
	m_cbPrintQualityModuleGrid.SetCurSel(AlgorithmParam.m_iPrintQualityModuleGrid);
	m_cbPrintQualityGapMin.SetCurSel(AlgorithmParam.m_iPrintQualityModuleGapMin);
	m_cbPrintQualityGapMax.SetCurSel(AlgorithmParam.m_iPrintQualityModuleGapMax);
	m_cbPrintQualityFinderPatternTolerance.SetCurSel(AlgorithmParam.m_iPrintQualityFinderPatternTolerance);

	UpdateData(FALSE);
}

void CAlgorithmPrintQualityDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUsePrintQuality = m_bCheckUsePrintQuality;

	pAlgorithmParam->m_iPrintQualityContrastMin = m_iEditPrintQualityContrastMin;

	pAlgorithmParam->m_iPrintQualitySymbolColsMin = m_iEditPrintQualitySymbolColsMin;
	pAlgorithmParam->m_iPrintQualitySymbolRowsMin = m_iEditPrintQualitySymbolRowsMin;
	pAlgorithmParam->m_iPrintQualitySymbolColsMax = m_iEditPrintQualitySymbolColsMax;
	pAlgorithmParam->m_iPrintQualitySymbolRowsMax = m_iEditPrintQualitySymbolRowsMax;

	pAlgorithmParam->m_iPrintQualityModuleSizeMin = m_iEditPrintQualityModuleSizeMin;
	pAlgorithmParam->m_iPrintQualityModuleSizeMax = m_iEditPrintQualityModuleSizeMax;

	pAlgorithmParam->m_dPrintQualitySlantMax = m_dEditPrintQualitySlantMax;

	pAlgorithmParam->m_bPrintQualityOverallQuality = m_bCheckPrintQualityOverallQuality;
	pAlgorithmParam->m_iPrintQualityGradeOverallQuality = m_iEditPrintQualityGradeOverallQuality;
	pAlgorithmParam->m_bPrintQualityUnusedErrorCorrection = m_bCheckPrintQualityUnusedErrorCorrection;
	pAlgorithmParam->m_iPrintQualityGradeUnusedErrorCorrection = m_iEditPrintQualityGradeUnusedErrorCorrection;

	pAlgorithmParam->m_iPrintQualityStrictModel = m_cbPrintQualityStrictModel.GetCurSel();
	pAlgorithmParam->m_iPrintQualityPolarity = m_cbPrintQualityPolarity.GetCurSel();
	pAlgorithmParam->m_iPrintQualityMirrored = m_cbPrintQualityMirrored.GetCurSel();
	pAlgorithmParam->m_iPrintQualityContrastTolerance = m_cbPrintQualityContrastTolerance.GetCurSel();
	pAlgorithmParam->m_iPrintQualityStrictQuietZone = m_cbPrintQualityStrictQuietZone.GetCurSel();
	pAlgorithmParam->m_iPrintQualitySymbolShape = m_cbPrintQualitySymbolShape.GetCurSel();
	pAlgorithmParam->m_iPrintQualitySmallModulesRobustness = m_cbPrintQualitySmallModulesRobustness.GetCurSel();
	pAlgorithmParam->m_iPrintQualityModuleGrid = m_cbPrintQualityModuleGrid.GetCurSel();
	pAlgorithmParam->m_iPrintQualityModuleGapMin = m_cbPrintQualityGapMin.GetCurSel();
	pAlgorithmParam->m_iPrintQualityModuleGapMax = m_cbPrintQualityGapMax.GetCurSel();
	pAlgorithmParam->m_iPrintQualityFinderPatternTolerance = m_cbPrintQualityFinderPatternTolerance.GetCurSel();

}

BOOL CAlgorithmPrintQualityDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_cbPrintQualityStrictModel.SetCurSel(0);
	m_cbPrintQualityPolarity.SetCurSel(0);
	m_cbPrintQualityMirrored.SetCurSel(0);
	m_cbPrintQualityContrastTolerance.SetCurSel(0);
	m_cbPrintQualityStrictQuietZone.SetCurSel(0);
	m_cbPrintQualitySymbolShape.SetCurSel(0);
	m_cbPrintQualitySmallModulesRobustness.SetCurSel(0);
	m_cbPrintQualityModuleGrid.SetCurSel(0);
	m_cbPrintQualityGapMin.SetCurSel(0);
	m_cbPrintQualityGapMax.SetCurSel(0);
	m_cbPrintQualityFinderPatternTolerance.SetCurSel(0);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
