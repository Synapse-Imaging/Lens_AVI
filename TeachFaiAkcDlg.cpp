// TeachFaiAkcDlg.cpp: 구현 파일
//

// Date : 2025.02.24
// Version : v2007
// Edited by : LeeGW

#include "stdafx.h"
#include "uScan.h"
#include "TeachFaiAkcDlg.h"
#include "afxdialogex.h"


// CTeachFaiAkcDlg 대화 상자

IMPLEMENT_DYNAMIC(CTeachFaiAkcDlg, CDialog)

CTeachFaiAkcDlg::CTeachFaiAkcDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TEACH_FAI_AKC_DLG, pParent)
{
	m_bCheckUseFai = FALSE;

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (CFAIDataManager::GetInstance().IsFAIUsed(i))
		{
			m_bCheckInspectFai[i] = FALSE;
			m_dEditNominalFai[i] = 1;
			m_dEditToleranceMaxFai[i] = 0;
			m_dEditToleranceMinFai[i] = 0;

			m_dEditMultipleStg1Fai[i] = 1;
			m_dEditMultipleStg2Fai[i] = 1;
			m_dEditOffsetStg1Fai[i] = 0;
			m_dEditOffsetStg2Fai[i] = 0;
		}
	}

}

CTeachFaiAkcDlg::~CTeachFaiAkcDlg()
{
}

void CTeachFaiAkcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_bnOK);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);

	DDX_Check(pDX, IDC_CHECK_USE_FAI_INSPECTION, m_bCheckUseFai);

	int iFAINumber;

	// FAI-6
	iFAINumber = 6;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_6, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_6, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_6, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_6, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_6, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_6, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_6, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_6, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_6, m_cbSpecialNGCode[iFAINumber]);

	// FAI-14
	iFAINumber = 14;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_14, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_14, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_14, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_14, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_14, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_14, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_14, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_14, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_14, m_cbSpecialNGCode[iFAINumber]);

	// FAI-14-X(21)
	iFAINumber = 21;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_21, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_21, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_21, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_21, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_21, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_21, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_21, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_21, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_21, m_cbSpecialNGCode[iFAINumber]);

	// FAI-14-Y
	iFAINumber = 15;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_15, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_15, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_15, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_15, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_15, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_15, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_15, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_15, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_15, m_cbSpecialNGCode[iFAINumber]);

	// FAI-43
	iFAINumber = 43;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_43, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_43, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_43, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_43, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_43, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_43, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_43, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_43, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_43, m_cbSpecialNGCode[iFAINumber]);

	// FAI-44
	iFAINumber = 44;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_44, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_44, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_44, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_44, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_44, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_44, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_44, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_44, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_44, m_cbSpecialNGCode[iFAINumber]);

	// FAI-19
	iFAINumber = 19;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_19, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_19, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_19, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_19, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_19, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_19, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_19, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_19, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_19, m_cbSpecialNGCode[iFAINumber]);

	// FAI-20
	iFAINumber = 20;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_20, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_20, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_20, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_20, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_20, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_20, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_20, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_20, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_20, m_cbSpecialNGCode[iFAINumber]);

	// FAI-70
	iFAINumber = 70;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_70, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_70, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_70, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_70, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_70, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_70, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_70, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_70, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_70, m_cbSpecialNGCode[iFAINumber]);

	// FAI-71
	iFAINumber = 71;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_71, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_71, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_71, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_71, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_71, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_71, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_71, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_71, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_71, m_cbSpecialNGCode[iFAINumber]);

	// FAI-81
	iFAINumber = 81;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_81, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_81, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_81, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_81, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_81, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_81, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_81, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_81, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_81, m_cbSpecialNGCode[iFAINumber]);

	// FAI-113
	iFAINumber = 113;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_113, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_113, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_113, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_113, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_113, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_113, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_113, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_113, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_113, m_cbSpecialNGCode[iFAINumber]);

	// FAI-119
	iFAINumber = 119;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_119, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_119, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_119, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_119, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_119, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_119, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_119, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_119, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_119, m_cbSpecialNGCode[iFAINumber]);

	// FAI-120
	iFAINumber = 120;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_120, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_120, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_120, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_120, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_120, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_120, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_120, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_120, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_120, m_cbSpecialNGCode[iFAINumber]);

	// FAI-126
	iFAINumber = 126;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_126, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_126, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_126, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_126, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_126, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_126, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_126, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_126, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_126, m_cbSpecialNGCode[iFAINumber]);

	// FAI-127
	iFAINumber = 127;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_127, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_127, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_127, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_127, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_127, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_127, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_127, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_127, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_127, m_cbSpecialNGCode[iFAINumber]);

	// FAI-128
	iFAINumber = 128;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_128, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_128, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_128, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_128, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_128, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_128, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_128, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_128, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_128, m_cbSpecialNGCode[iFAINumber]);

	// FAI-129
	iFAINumber = 129;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_129, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_129, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_129, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_129, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_129, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_129, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_129, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_129, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_129, m_cbSpecialNGCode[iFAINumber]);

	// FAI-130
	iFAINumber = 130;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_130, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_130, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_130, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_130, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_130, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_130, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_130, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_130, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_130, m_cbSpecialNGCode[iFAINumber]);

	// FAI-140
	iFAINumber = 140;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_140, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_140, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_140, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_140, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_140, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_140, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_140, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_140, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_140, m_cbSpecialNGCode[iFAINumber]);

	// FAI-141
	iFAINumber = 141;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_141, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_141, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_141, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_141, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_141, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_141, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_141, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_141, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_141, m_cbSpecialNGCode[iFAINumber]);

	// FAI-142
	iFAINumber = 142;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_142, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_142, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_142, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_142, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_142, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_142, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_142, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_142, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_142, m_cbSpecialNGCode[iFAINumber]);

	// FAI-143
	iFAINumber = 143;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_143, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_143, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_143, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_143, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_143, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_143, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_143, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_143, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_143, m_cbSpecialNGCode[iFAINumber]);

	// FAI-144
	iFAINumber = 144;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_144, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_144, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_144, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_144, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_144, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_144, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_144, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_144, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_144, m_cbSpecialNGCode[iFAINumber]);

	// FAI-145
	iFAINumber = 145;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_145, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_145, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_145, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_145, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_145, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_145, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_145, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_145, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_145, m_cbSpecialNGCode[iFAINumber]);

	// FAI-146
	iFAINumber = 146;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_146, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_146, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_146, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_146, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_146, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_146, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_146, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_146, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_146, m_cbSpecialNGCode[iFAINumber]);

	// FAI-147
	iFAINumber = 147;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_147, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_147, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_147, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_147, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_147, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_147, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_147, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_147, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_147, m_cbSpecialNGCode[iFAINumber]);

	// FAI-148
	iFAINumber = 148;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_148, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_148, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_148, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_148, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_148, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_148, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_148, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_148, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_148, m_cbSpecialNGCode[iFAINumber]);

	// FAI-149
	iFAINumber = 149;
	DDX_Check(pDX, IDC_CHECK_AKC_INSPECT_FAI_149, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_NOMINAL_FAI_149, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MAX_FAI_149, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_TOLERANCE_MIN_FAI_149, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG1_FAI_149, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_MULTIPLE_STG2_FAI_149, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG1_FAI_149, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_AKC_OFFSET_STG2_FAI_149, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_AKC_FAI_DEFECT_CODE_149, m_cbSpecialNGCode[iFAINumber]);
}

BEGIN_MESSAGE_MAP(CTeachFaiAkcDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTeachFaiAkcDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTeachFaiAkcDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// CTeachFaiAkcDlg 메시지 처리기

BOOL CTeachFaiAkcDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CTeachFaiAkcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_bnOK.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnCancel.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_bCheckUseFai = THEAPP.m_pModelDataManager->m_bMUseFai;

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (CFAIDataManager::GetInstance().IsFAIUsed(i))
		{
			m_bCheckInspectFai[i] = THEAPP.m_pModelDataManager->m_bMInspectFai[i];
			m_dEditNominalFai[i] = THEAPP.m_pModelDataManager->m_dMNominalFai[i];
			m_dEditToleranceMaxFai[i] = THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i];
			m_dEditToleranceMinFai[i] = THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i];
			m_dEditMultipleStg1Fai[i] = THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai[i];
			m_dEditMultipleStg2Fai[i] = THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai[i];
			m_dEditOffsetStg1Fai[i] = THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i];
			m_dEditOffsetStg2Fai[i] = THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i];
		}
	}

	CString sSpecialNGCode;
	int iComboIdx;
	int i, j;

	for (i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (CFAIDataManager::GetInstance().IsFAIUsed(i))
		{
			// ComboBox가 DDX_Control로 연결되었는지 확인 (NULL 체크)
			if (m_cbSpecialNGCode[i].GetSafeHwnd() == NULL)
				continue;

			for (j = 0; j < THEAPP.Struct_PreferenceStruct.iNoAssignSpecialCode; j++)
				m_cbSpecialNGCode[i].AddString(THEAPP.Struct_PreferenceStruct.m_sAssignSpecialCode[j]);

			iComboIdx = THEAPP.m_pModelDataManager->m_iTransferCodeIndexFai[i];

			m_cbSpecialNGCode[i].SetCurSel(iComboIdx);
		}
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTeachFaiAkcDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();

	int iComboIdx;

	THEAPP.m_pModelDataManager->m_bMUseFai = m_bCheckUseFai;

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (CFAIDataManager::GetInstance().IsFAIUsed(i))
		{
			THEAPP.m_pModelDataManager->m_bMInspectFai[i] = m_bCheckInspectFai[i];
			THEAPP.m_pModelDataManager->m_dMNominalFai[i] = m_dEditNominalFai[i];
			THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i] = m_dEditToleranceMaxFai[i];
			THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i] = m_dEditToleranceMinFai[i];
			THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai[i] = m_dEditMultipleStg1Fai[i];
			THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai[i] = m_dEditMultipleStg2Fai[i];
			THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i] = m_dEditOffsetStg1Fai[i];
			THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i] = m_dEditOffsetStg2Fai[i];

			iComboIdx = m_cbSpecialNGCode[i].GetCurSel();
			THEAPP.m_pModelDataManager->m_iTransferCodeIndexFai[i] = iComboIdx;

			for (int iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
			{
				if (THEAPP.m_iCurTeachVision)
					continue;

				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bMInspectFai[i] = THEAPP.m_pModelDataManager->m_bMInspectFai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMNominalFai[i] = THEAPP.m_pModelDataManager->m_dMNominalFai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMToleranceMaxFai[i] = THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMToleranceMinFai[i] = THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMMultipleStg1Fai[i] = THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMMultipleStg2Fai[i] = THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMOffsetStg1Fai[i] = THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMOffsetStg2Fai[i] = THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i];

			}

		}
	}

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;


	CString sModelSwPath;
	CString strSection;
	CString sTemp;

	for (int iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
	{
		sModelSwPath.Format("%s\\SW\\Vision_N%d\\FaiMeasureSpec_DFA.ini", strModelFolder, iPcVisionNo + 1);

		CIniFileCS INI_FAI_SW(sModelSwPath);

		strSection = "FAI";
		sTemp.Format("m_bMUseFai");
		INI_FAI_SW.Set_Bool(strSection, sTemp, THEAPP.m_pModelDataManager->m_bMUseFai);

		for (int i = 0; i < MAX_FAI_ITEM; i++)
		{
			if (CFAIDataManager::GetInstance().IsFAIUsed(i))
			{
				sTemp.Format("m_bMInspectFai_%d", i);
				INI_FAI_SW.Set_Bool(strSection, sTemp, THEAPP.m_pModelDataManager->m_bMInspectFai[i]);
				sTemp.Format("m_dMNominalFai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMNominalFai[i]);
				sTemp.Format("m_dMToleranceMaxFai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i]);
				sTemp.Format("m_dMToleranceMinFai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i]);
				sTemp.Format("m_dMMultipleStg1Fai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai[i]);
				sTemp.Format("m_dMMultipleStg2Fai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai[i]);
				sTemp.Format("m_dMOffsetStg1Fai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i]);
				sTemp.Format("m_dMOffsetStg2Fai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i]);
				sTemp.Format("m_iTransferCodeIndexFai_%d", i);
				INI_FAI_SW.Set_Integer(strSection, sTemp, THEAPP.m_pModelDataManager->m_iTransferCodeIndexFai[i]);
				sTemp.Format("m_bMultiFai_%d", i);
				INI_FAI_SW.Set_Bool(strSection, sTemp, THEAPP.m_pModelDataManager->m_bMultiFai[i]);
			}
		}

	}


	CDialog::OnOK();
}


void CTeachFaiAkcDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}