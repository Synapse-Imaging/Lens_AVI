// TeachFaiActDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "TeachFaiActDlg.h"
#include "afxdialogex.h"


// CTeachFaiActDlg 대화 상자

IMPLEMENT_DYNAMIC(CTeachFaiActDlg, CDialog)

CTeachFaiActDlg::CTeachFaiActDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TEACH_FAI_ACT_DLG, pParent)
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

CTeachFaiActDlg::~CTeachFaiActDlg()
{
}

void CTeachFaiActDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_bnOK);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);

	DDX_Check(pDX, IDC_CHECK_USE_FAI_INSPECTION, m_bCheckUseFai);

	int iFAINumber;

	// FAI-5
	iFAINumber = 5;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_5, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_5, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_5, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_5, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_5, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_5, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_5, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_5, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_5, m_cbSpecialNGCode[iFAINumber]);

	// FAI-9
	iFAINumber = 9;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_9, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_9, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_9, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_9, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_9, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_9, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_9, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_9, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_9, m_cbSpecialNGCode[iFAINumber]);

	// FAI-10
	iFAINumber = 10;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_10, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_10, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_10, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_10, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_10, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_10, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_10, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_10, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_10, m_cbSpecialNGCode[iFAINumber]);

	// FAI-11
	iFAINumber = 11;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_11, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_11, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_11, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_11, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_11, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_11, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_11, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_11, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_11, m_cbSpecialNGCode[iFAINumber]);

	// FAI-13
	iFAINumber = 13;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_13, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_13, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_13, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_13, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_13, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_13, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_13, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_13, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_13, m_cbSpecialNGCode[iFAINumber]);

	// FAI-14
	iFAINumber = 14;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_14, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_14, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_14, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_14, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_14, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_14, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_14, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_14, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_14, m_cbSpecialNGCode[iFAINumber]);

	// FAI-19
	iFAINumber = 19;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_19, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_19, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_19, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_19, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_19, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_19, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_19, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_19, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_19, m_cbSpecialNGCode[iFAINumber]);

	// FAI-20
	iFAINumber = 20;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_20, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_20, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_20, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_20, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_20, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_20, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_20, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_20, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_20, m_cbSpecialNGCode[iFAINumber]);

	// FAI-21
	iFAINumber = 21;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_21, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_21, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_21, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_21, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_21, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_21, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_21, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_21, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_21, m_cbSpecialNGCode[iFAINumber]);

	// FAI-22
	iFAINumber = 22;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_22, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_22, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_22, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_22, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_22, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_22, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_22, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_22, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_22, m_cbSpecialNGCode[iFAINumber]);

	// FAI-24
	iFAINumber = 24;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_24, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_24, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_24, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_24, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_24, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_24, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_24, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_24, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_24, m_cbSpecialNGCode[iFAINumber]);

	// FAI-26
	iFAINumber = 26;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_26, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_26, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_26, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_26, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_26, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_26, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_26, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_26, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_26, m_cbSpecialNGCode[iFAINumber]);

	// FAI-27
	iFAINumber = 27;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_27, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_27, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_27, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_27, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_27, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_27, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_27, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_27, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_27, m_cbSpecialNGCode[iFAINumber]);

	// FAI-29
	iFAINumber = 29;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_29, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_29, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_29, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_29, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_29, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_29, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_29, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_29, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_29, m_cbSpecialNGCode[iFAINumber]);

	// FAI-31
	iFAINumber = 31;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_31, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_31, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_31, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_31, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_31, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_31, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_31, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_31, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_31, m_cbSpecialNGCode[iFAINumber]);

	// FAI-32
	iFAINumber = 32;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_32, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_32, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_32, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_32, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_32, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_32, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_32, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_32, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_32, m_cbSpecialNGCode[iFAINumber]);

	// FAI-33
	iFAINumber = 33;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_33, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_33, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_33, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_33, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_33, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_33, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_33, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_33, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_33, m_cbSpecialNGCode[iFAINumber]);

	// FAI-34
	iFAINumber = 34;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_34, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_34, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_34, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_34, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_34, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_34, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_34, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_34, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_34, m_cbSpecialNGCode[iFAINumber]);

	// FAI-44
	iFAINumber = 44;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_44, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_44, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_44, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_44, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_44, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_44, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_44, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_44, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_44, m_cbSpecialNGCode[iFAINumber]);

	// FAI-124
	iFAINumber = 124;
	DDX_Check(pDX, IDC_CHECK_ACT_INSPECT_FAI_124, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_NOMINAL_FAI_124, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MAX_FAI_124, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_TOLERANCE_MIN_FAI_124, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG1_FAI_124, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_MULTIPLE_STG2_FAI_124, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG1_FAI_124, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_ACT_OFFSET_STG2_FAI_124, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_ACT_FAI_DEFECT_CODE_124, m_cbSpecialNGCode[iFAINumber]);

}

BEGIN_MESSAGE_MAP(CTeachFaiActDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTeachFaiActDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTeachFaiActDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// CTeachFaiActDlg 메시지 처리기

BOOL CTeachFaiActDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CTeachFaiActDlg::OnInitDialog()
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
			// ComboBox가 DDX_Control로 윈도우 핸들에 연결되었는지 확인 (크래시 방지)
			if (m_cbSpecialNGCode[i].GetSafeHwnd() == NULL)
				continue;

			// 특수 NG 코드 목록을 ComboBox에 추가 (_T() 매크로 불필요 - CString은 자동 변환됨)
			for (j = 0; j < THEAPP.Struct_PreferenceStruct.iNoAssignSpecialCode; j++)
				m_cbSpecialNGCode[i].AddString(THEAPP.Struct_PreferenceStruct.m_sAssignSpecialCode[j]);

			// 저장된 선택 인덱스로 ComboBox 설정
			iComboIdx = THEAPP.m_pModelDataManager->m_iTransferCodeIndexFai[i];
			m_cbSpecialNGCode[i].SetCurSel(iComboIdx);
		}
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTeachFaiActDlg::OnBnClickedOk()
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


void CTeachFaiActDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}