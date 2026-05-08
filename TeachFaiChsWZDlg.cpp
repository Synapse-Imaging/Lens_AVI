// TeachFaiChsWZDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "TeachFaiChsWZDlg.h"
#include "afxdialogex.h"


// CTeachFaiChsWZDlg 대화 상자

IMPLEMENT_DYNAMIC(CTeachFaiChsWZDlg, CDialog)

CTeachFaiChsWZDlg::CTeachFaiChsWZDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TEACH_FAI_CHS_WZ_DLG, pParent)
{
	m_bCheckUseFai = FALSE;

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		m_bCheckInspectFai[i] = FALSE;
		m_dEditNominalFai[i] = 1;
		m_dEditToleranceMaxFai[i] = 0;
		m_dEditToleranceMinFai[i] = 0;

		m_dEditOffsetStg1Fai[i] = 0;
		m_dEditTiltStg1Fai[i] = 0;
		m_dEditOffsetStg2Fai[i] = 0;
		m_dEditTiltStg2Fai[i] = 0;
	}
}

CTeachFaiChsWZDlg::~CTeachFaiChsWZDlg()
{
}

void CTeachFaiChsWZDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_bnOK);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);

	DDX_Check(pDX, IDC_CHECK_USE_FAI_INSPECTION, m_bCheckUseFai);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_1, m_bCheckInspectFai[1]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_1, m_cbSpecialNGCode[1]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_1, m_dEditNominalFai[1]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_1, m_dEditToleranceMaxFai[1]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_1, m_dEditToleranceMinFai[1]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_1, m_dEditOffsetStg1Fai[1]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_1, m_dEditTiltStg1Fai[1]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_1, m_dEditOffsetStg2Fai[1]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_1, m_dEditTiltStg2Fai[1]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_2, m_bCheckInspectFai[2]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_2, m_cbSpecialNGCode[2]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_2, m_dEditNominalFai[2]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_2, m_dEditToleranceMaxFai[2]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_2, m_dEditToleranceMinFai[2]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_2, m_dEditOffsetStg1Fai[2]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_2, m_dEditTiltStg1Fai[2]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_2, m_dEditOffsetStg2Fai[2]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_2, m_dEditTiltStg2Fai[2]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_3, m_bCheckInspectFai[3]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_3, m_cbSpecialNGCode[3]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_3, m_dEditNominalFai[3]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_3, m_dEditToleranceMaxFai[3]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_3, m_dEditToleranceMinFai[3]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_3, m_dEditOffsetStg1Fai[3]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_3, m_dEditTiltStg1Fai[3]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_3, m_dEditOffsetStg2Fai[3]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_3, m_dEditTiltStg2Fai[3]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_4, m_bCheckInspectFai[4]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_4, m_cbSpecialNGCode[4]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_4, m_dEditNominalFai[4]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_4, m_dEditToleranceMaxFai[4]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_4, m_dEditToleranceMinFai[4]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_4, m_dEditOffsetStg1Fai[4]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_4, m_dEditTiltStg1Fai[4]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_4, m_dEditOffsetStg2Fai[4]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_4, m_dEditTiltStg2Fai[4]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_5, m_bCheckInspectFai[5]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_5, m_cbSpecialNGCode[5]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_5, m_dEditNominalFai[5]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_5, m_dEditToleranceMaxFai[5]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_5, m_dEditToleranceMinFai[5]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_5, m_dEditOffsetStg1Fai[5]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_5, m_dEditTiltStg1Fai[5]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_5, m_dEditOffsetStg2Fai[5]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_5, m_dEditTiltStg2Fai[5]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_6, m_bCheckInspectFai[6]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_6, m_cbSpecialNGCode[6]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_6, m_dEditNominalFai[6]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_6, m_dEditToleranceMaxFai[6]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_6, m_dEditToleranceMinFai[6]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_6, m_dEditOffsetStg1Fai[6]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_6, m_dEditTiltStg1Fai[6]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_6, m_dEditOffsetStg2Fai[6]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_6, m_dEditTiltStg2Fai[6]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_7, m_bCheckInspectFai[7]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_7, m_cbSpecialNGCode[7]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_7, m_dEditNominalFai[7]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_7, m_dEditToleranceMaxFai[7]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_7, m_dEditToleranceMinFai[7]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_7, m_dEditOffsetStg1Fai[7]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_7, m_dEditTiltStg1Fai[7]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_7, m_dEditOffsetStg2Fai[7]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_7, m_dEditTiltStg2Fai[7]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_8, m_bCheckInspectFai[8]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_8, m_cbSpecialNGCode[8]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_8, m_dEditNominalFai[8]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_8, m_dEditToleranceMaxFai[8]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_8, m_dEditToleranceMinFai[8]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_8, m_dEditOffsetStg1Fai[8]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_8, m_dEditTiltStg1Fai[8]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_8, m_dEditOffsetStg2Fai[8]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_8, m_dEditTiltStg2Fai[8]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_9, m_bCheckInspectFai[9]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_9, m_cbSpecialNGCode[9]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_9, m_dEditNominalFai[9]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_9, m_dEditToleranceMaxFai[9]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_9, m_dEditToleranceMinFai[9]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_9, m_dEditOffsetStg1Fai[9]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_9, m_dEditTiltStg1Fai[9]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_9, m_dEditOffsetStg2Fai[9]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_9, m_dEditTiltStg2Fai[9]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_10, m_bCheckInspectFai[10]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_10, m_cbSpecialNGCode[10]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_10, m_dEditNominalFai[10]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_10, m_dEditToleranceMaxFai[10]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_10, m_dEditToleranceMinFai[10]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_10, m_dEditOffsetStg1Fai[10]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_10, m_dEditTiltStg1Fai[10]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_10, m_dEditOffsetStg2Fai[10]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_10, m_dEditTiltStg2Fai[10]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_11, m_bCheckInspectFai[11]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_11, m_cbSpecialNGCode[11]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_11, m_dEditNominalFai[11]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_11, m_dEditToleranceMaxFai[11]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_11, m_dEditToleranceMinFai[11]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_11, m_dEditOffsetStg1Fai[11]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_11, m_dEditTiltStg1Fai[11]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_11, m_dEditOffsetStg2Fai[11]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_11, m_dEditTiltStg2Fai[11]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_12, m_bCheckInspectFai[12]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_12, m_cbSpecialNGCode[12]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_12, m_dEditNominalFai[12]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_12, m_dEditToleranceMaxFai[12]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_12, m_dEditToleranceMinFai[12]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_12, m_dEditOffsetStg1Fai[12]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_12, m_dEditTiltStg1Fai[12]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_12, m_dEditOffsetStg2Fai[12]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_12, m_dEditTiltStg2Fai[12]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_13, m_bCheckInspectFai[13]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_13, m_cbSpecialNGCode[13]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_13, m_dEditNominalFai[13]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_13, m_dEditToleranceMaxFai[13]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_13, m_dEditToleranceMinFai[13]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_13, m_dEditOffsetStg1Fai[13]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_13, m_dEditTiltStg1Fai[13]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_13, m_dEditOffsetStg2Fai[13]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_13, m_dEditTiltStg2Fai[13]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_14, m_bCheckInspectFai[14]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_14, m_cbSpecialNGCode[14]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_14, m_dEditNominalFai[14]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_14, m_dEditToleranceMaxFai[14]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_14, m_dEditToleranceMinFai[14]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_14, m_dEditOffsetStg1Fai[14]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_14, m_dEditTiltStg1Fai[14]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_14, m_dEditOffsetStg2Fai[14]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_14, m_dEditTiltStg2Fai[14]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_15, m_bCheckInspectFai[15]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_15, m_cbSpecialNGCode[15]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_15, m_dEditNominalFai[15]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_15, m_dEditToleranceMaxFai[15]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_15, m_dEditToleranceMinFai[15]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_15, m_dEditOffsetStg1Fai[15]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_15, m_dEditTiltStg1Fai[15]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_15, m_dEditOffsetStg2Fai[15]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_15, m_dEditTiltStg2Fai[15]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_16, m_bCheckInspectFai[16]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_16, m_cbSpecialNGCode[16]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_16, m_dEditNominalFai[16]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_16, m_dEditToleranceMaxFai[16]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_16, m_dEditToleranceMinFai[16]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_16, m_dEditOffsetStg1Fai[16]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_16, m_dEditTiltStg1Fai[16]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_16, m_dEditOffsetStg2Fai[16]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_16, m_dEditTiltStg2Fai[16]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_86, m_bCheckInspectFai[86]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_86, m_cbSpecialNGCode[86]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_86, m_dEditNominalFai[86]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_86, m_dEditToleranceMaxFai[86]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_86, m_dEditToleranceMinFai[86]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_86, m_dEditOffsetStg1Fai[86]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_86, m_dEditTiltStg1Fai[86]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_86, m_dEditOffsetStg2Fai[86]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_86, m_dEditTiltStg2Fai[86]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_87, m_bCheckInspectFai[87]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_87, m_cbSpecialNGCode[87]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_87, m_dEditNominalFai[87]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_87, m_dEditToleranceMaxFai[87]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_87, m_dEditToleranceMinFai[87]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_87, m_dEditOffsetStg1Fai[87]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_87, m_dEditTiltStg1Fai[87]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_87, m_dEditOffsetStg2Fai[87]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_87, m_dEditTiltStg2Fai[87]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_88, m_bCheckInspectFai[88]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_88, m_cbSpecialNGCode[88]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_88, m_dEditNominalFai[88]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_88, m_dEditToleranceMaxFai[88]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_88, m_dEditToleranceMinFai[88]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_88, m_dEditOffsetStg1Fai[88]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_88, m_dEditTiltStg1Fai[88]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_88, m_dEditOffsetStg2Fai[88]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_88, m_dEditTiltStg2Fai[88]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_18, m_bCheckInspectFai[18]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_18, m_cbSpecialNGCode[18]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_18, m_dEditNominalFai[18]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_18, m_dEditToleranceMaxFai[18]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_18, m_dEditToleranceMinFai[18]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_18, m_dEditOffsetStg1Fai[18]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_18, m_dEditTiltStg1Fai[18]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_18, m_dEditOffsetStg2Fai[18]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_18, m_dEditTiltStg2Fai[18]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_19, m_bCheckInspectFai[19]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_19, m_cbSpecialNGCode[19]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_19, m_dEditNominalFai[19]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_19, m_dEditToleranceMaxFai[19]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_19, m_dEditToleranceMinFai[19]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_19, m_dEditOffsetStg1Fai[19]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_19, m_dEditTiltStg1Fai[19]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_19, m_dEditOffsetStg2Fai[19]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_19, m_dEditTiltStg2Fai[19]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_20, m_bCheckInspectFai[20]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_20, m_cbSpecialNGCode[20]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_20, m_dEditNominalFai[20]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_20, m_dEditToleranceMaxFai[20]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_20, m_dEditToleranceMinFai[20]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_20, m_dEditOffsetStg1Fai[20]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_20, m_dEditTiltStg1Fai[20]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_20, m_dEditOffsetStg2Fai[20]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_20, m_dEditTiltStg2Fai[20]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_21, m_bCheckInspectFai[21]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_21, m_cbSpecialNGCode[21]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_21, m_dEditNominalFai[21]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_21, m_dEditToleranceMaxFai[21]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_21, m_dEditToleranceMinFai[21]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_21, m_dEditOffsetStg1Fai[21]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_21, m_dEditTiltStg1Fai[21]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_21, m_dEditOffsetStg2Fai[21]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_21, m_dEditTiltStg2Fai[21]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_22, m_bCheckInspectFai[22]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_22, m_cbSpecialNGCode[22]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_22, m_dEditNominalFai[22]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_22, m_dEditToleranceMaxFai[22]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_22, m_dEditToleranceMinFai[22]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_22, m_dEditOffsetStg1Fai[22]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_22, m_dEditTiltStg1Fai[22]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_22, m_dEditOffsetStg2Fai[22]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_22, m_dEditTiltStg2Fai[22]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_23, m_bCheckInspectFai[23]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_23, m_cbSpecialNGCode[23]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_23, m_dEditNominalFai[23]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_23, m_dEditToleranceMaxFai[23]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_23, m_dEditToleranceMinFai[23]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_23, m_dEditOffsetStg1Fai[23]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_23, m_dEditTiltStg1Fai[23]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_23, m_dEditOffsetStg2Fai[23]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_23, m_dEditTiltStg2Fai[23]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_24, m_bCheckInspectFai[24]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_24, m_cbSpecialNGCode[24]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_24, m_dEditNominalFai[24]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_24, m_dEditToleranceMaxFai[24]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_24, m_dEditToleranceMinFai[24]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_24, m_dEditOffsetStg1Fai[24]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_24, m_dEditTiltStg1Fai[24]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_24, m_dEditOffsetStg2Fai[24]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_24, m_dEditTiltStg2Fai[24]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_25, m_bCheckInspectFai[25]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_25, m_cbSpecialNGCode[25]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_25, m_dEditNominalFai[25]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_25, m_dEditToleranceMaxFai[25]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_25, m_dEditToleranceMinFai[25]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_25, m_dEditOffsetStg1Fai[25]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_25, m_dEditTiltStg1Fai[25]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_25, m_dEditOffsetStg2Fai[25]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_25, m_dEditTiltStg2Fai[25]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_84, m_bCheckInspectFai[84]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_84, m_cbSpecialNGCode[84]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_84, m_dEditNominalFai[84]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_84, m_dEditToleranceMaxFai[84]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_84, m_dEditToleranceMinFai[84]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_84, m_dEditOffsetStg1Fai[84]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_84, m_dEditTiltStg1Fai[84]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_84, m_dEditOffsetStg2Fai[84]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_84, m_dEditTiltStg2Fai[84]);

	DDX_Check(pDX, IDC_CHECK_CHS_WZ_INSPECT_FAI_85, m_bCheckInspectFai[85]);
	DDX_Control(pDX, IDC_COMBO_CHS_WZ_FAI_DEFECT_CODE_85, m_cbSpecialNGCode[85]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_NOMINAL_FAI_85, m_dEditNominalFai[85]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MAX_FAI_85, m_dEditToleranceMaxFai[85]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TOLERANCE_MIN_FAI_85, m_dEditToleranceMinFai[85]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG1_FAI_85, m_dEditOffsetStg1Fai[85]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG1_FAI_85, m_dEditTiltStg1Fai[85]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_OFFSET_STG2_FAI_85, m_dEditOffsetStg2Fai[85]);
	DDX_Text(pDX, IDC_EDIT_CHS_WZ_TILT_STG2_FAI_85, m_dEditTiltStg2Fai[85]);
}

BEGIN_MESSAGE_MAP(CTeachFaiChsWZDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTeachFaiChsWZDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTeachFaiChsWZDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// CTeachFaiChsWZDlg 메시지 처리기

BOOL CTeachFaiChsWZDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CTeachFaiChsWZDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_bnOK.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnCancel.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_bCheckUseFai = THEAPP.m_pModelDataManager->m_bMUseFai;

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		m_bCheckInspectFai[i] = THEAPP.m_pModelDataManager->m_bMInspectFai[i];
		m_dEditNominalFai[i] = THEAPP.m_pModelDataManager->m_dMNominalFai[i];
		m_dEditToleranceMaxFai[i] = THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i];
		m_dEditToleranceMinFai[i] = THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i];
		m_dEditOffsetStg1Fai[i] = THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i];
		m_dEditTiltStg1Fai[i] = THEAPP.m_pModelDataManager->m_dMTiltStg1Fai[i];
		m_dEditOffsetStg2Fai[i] = THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i];
		m_dEditTiltStg2Fai[i] = THEAPP.m_pModelDataManager->m_dMTiltStg2Fai[i];
	}

	CString sSpecialNGCode;
	int iComboIdx;
	int i, j;

	for (i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (i == 1 || i == 2 || i == 3 || i == 4 || i == 5 || i == 6 || i == 7 || i == 8 || i == 9 || i == 10 ||
			i == 11 || i == 12 || i == 13 || i == 14 || i == 15 || i == 16 || i == 86 || i == 87 || i == 88 || i == 18 || i == 19 || i == 20 ||
			i == 21 || i == 22 || i == 23 || i == 24 || i == 25 || i == 84 || i == 85)
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

void CTeachFaiChsWZDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();

	int iComboIdx;

	THEAPP.m_pModelDataManager->m_bMUseFai = m_bCheckUseFai;

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (i == 1 || i == 2 || i == 3 || i == 4 || i == 5 || i == 6 || i == 7 || i == 8 || i == 9 || i == 10 ||
			i == 11 || i == 12 || i == 13 || i == 14 || i == 15 || i == 16 || i == 86 || i == 87 || i == 88 || i == 18 || i == 19 || i == 20 ||
			i == 21 || i == 22 || i == 23 || i == 24 || i == 25 || i == 84 || i == 85)
		{
			THEAPP.m_pModelDataManager->m_bMInspectFai[i] = m_bCheckInspectFai[i];
			THEAPP.m_pModelDataManager->m_dMNominalFai[i] = m_dEditNominalFai[i];
			THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i] = m_dEditToleranceMaxFai[i];
			THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i] = m_dEditToleranceMinFai[i];
			THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i] = m_dEditOffsetStg1Fai[i];
			THEAPP.m_pModelDataManager->m_dMTiltStg1Fai[i] = m_dEditTiltStg1Fai[i];
			THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i] = m_dEditOffsetStg2Fai[i];
			THEAPP.m_pModelDataManager->m_dMTiltStg2Fai[i] = m_dEditTiltStg2Fai[i];

			iComboIdx = m_cbSpecialNGCode[i].GetCurSel();
			THEAPP.m_pModelDataManager->m_iTransferCodeIndexFai[i] = iComboIdx;
		}
	}

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

	CString sModelSwPath;
	sModelSwPath.Format("%s\\SW\\Vision_N%d\\FaiMeasureSpec_DFA.ini", strModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	CIniFileCS INI_FAI_SW(sModelSwPath);
	CString strSection;
	CString sTemp;

	strSection = "FAI";

	sTemp.Format("m_bMUseFai");
	INI_FAI_SW.Set_Bool(strSection, sTemp, THEAPP.m_pModelDataManager->m_bMUseFai);

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		sTemp.Format("m_bMInspectFai_%d", i);
		INI_FAI_SW.Set_Bool(strSection, sTemp, THEAPP.m_pModelDataManager->m_bMInspectFai[i]);
		sTemp.Format("m_dMNominalFai_%d", i);
		INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMNominalFai[i]);
		sTemp.Format("m_dMToleranceMaxFai_%d", i);
		INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i]);
		sTemp.Format("m_dMToleranceMinFai_%d", i);
		INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i]);
		sTemp.Format("m_dMOffsetStg1Fai_%d", i);
		INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i]);
		sTemp.Format("m_dMTiltStg1Fai_%d", i);
		INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMTiltStg1Fai[i]);
		sTemp.Format("m_dMOffsetStg2Fai_%d", i);
		INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i]);
		sTemp.Format("m_dMTiltStg2Fai_%d", i);
		INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMTiltStg2Fai[i]);
		sTemp.Format("m_iTransferCodeIndexFai_%d", i);
		INI_FAI_SW.Set_Integer(strSection, sTemp, THEAPP.m_pModelDataManager->m_iTransferCodeIndexFai[i]);
		sTemp.Format("m_bMultiFai_%d", i);
		INI_FAI_SW.Set_Bool(strSection, sTemp, THEAPP.m_pModelDataManager->m_bMultiFai[i]);
	}

	CDialog::OnOK();
}


void CTeachFaiChsWZDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
