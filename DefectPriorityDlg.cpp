// DefectPriorityDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "DefectPriorityDlg.h"
#include "afxdialogex.h"


// CDefectPriorityDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDefectPriorityDlg, CDialog)

CDefectPriorityDlg::CDefectPriorityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefectPriorityDlg::IDD, pParent)
{
	m_iCurInspectionType = 0;

	for (int i = 0; i < MAX_DEFECT_NAME; i++)
		m_iPriority[i] = -1;
}

CDefectPriorityDlg::~CDefectPriorityDlg()
{
}

void CDefectPriorityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_PRIORITY_1, m_cbPriority[0]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_2, m_cbPriority[1]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_3, m_cbPriority[2]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_4, m_cbPriority[3]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_5, m_cbPriority[4]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_6, m_cbPriority[5]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_7, m_cbPriority[6]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_8, m_cbPriority[7]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_9, m_cbPriority[8]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_10, m_cbPriority[9]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_11, m_cbPriority[10]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_12, m_cbPriority[11]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_13, m_cbPriority[12]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_14, m_cbPriority[13]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_15, m_cbPriority[14]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_16, m_cbPriority[15]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_17, m_cbPriority[16]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_18, m_cbPriority[17]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_19, m_cbPriority[18]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_20, m_cbPriority[19]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_21, m_cbPriority[20]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_22, m_cbPriority[21]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_23, m_cbPriority[22]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_24, m_cbPriority[23]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_25, m_cbPriority[24]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_26, m_cbPriority[25]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_27, m_cbPriority[26]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_28, m_cbPriority[27]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_29, m_cbPriority[28]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_30, m_cbPriority[29]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_31, m_cbPriority[30]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_32, m_cbPriority[31]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_33, m_cbPriority[32]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_34, m_cbPriority[33]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_35, m_cbPriority[34]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_36, m_cbPriority[35]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_37, m_cbPriority[36]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_38, m_cbPriority[37]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_39, m_cbPriority[38]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_40, m_cbPriority[39]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_41, m_cbPriority[40]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_42, m_cbPriority[41]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_43, m_cbPriority[42]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_44, m_cbPriority[43]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_45, m_cbPriority[44]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_46, m_cbPriority[45]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_47, m_cbPriority[46]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_48, m_cbPriority[47]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_49, m_cbPriority[48]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_50, m_cbPriority[49]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_51, m_cbPriority[50]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_52, m_cbPriority[51]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_53, m_cbPriority[52]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_54, m_cbPriority[53]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_55, m_cbPriority[54]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_56, m_cbPriority[55]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_57, m_cbPriority[56]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_58, m_cbPriority[57]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_59, m_cbPriority[58]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_60, m_cbPriority[59]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_61, m_cbPriority[60]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_62, m_cbPriority[61]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_63, m_cbPriority[62]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_64, m_cbPriority[63]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_65, m_cbPriority[64]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_66, m_cbPriority[65]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_67, m_cbPriority[66]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_68, m_cbPriority[67]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_69, m_cbPriority[68]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_70, m_cbPriority[69]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_71, m_cbPriority[70]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_72, m_cbPriority[71]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_73, m_cbPriority[72]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_74, m_cbPriority[73]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_75, m_cbPriority[74]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_76, m_cbPriority[75]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_77, m_cbPriority[76]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_78, m_cbPriority[77]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_79, m_cbPriority[78]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_80, m_cbPriority[79]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_81, m_cbPriority[80]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_82, m_cbPriority[81]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_83, m_cbPriority[82]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_84, m_cbPriority[83]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_85, m_cbPriority[84]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_86, m_cbPriority[85]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_87, m_cbPriority[86]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_88, m_cbPriority[87]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_89, m_cbPriority[88]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_90, m_cbPriority[89]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_91, m_cbPriority[90]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_92, m_cbPriority[91]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_93, m_cbPriority[92]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_94, m_cbPriority[93]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_95, m_cbPriority[94]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_96, m_cbPriority[95]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_97, m_cbPriority[96]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_98, m_cbPriority[97]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_99, m_cbPriority[98]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_100, m_cbPriority[99]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_101, m_cbPriority[100]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_102, m_cbPriority[101]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_103, m_cbPriority[102]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_104, m_cbPriority[103]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_105, m_cbPriority[104]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_106, m_cbPriority[105]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_107, m_cbPriority[106]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_108, m_cbPriority[107]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_109, m_cbPriority[108]);
	DDX_Control(pDX, IDC_COMBO_PRIORITY_110, m_cbPriority[109]);

	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_1, m_iPriority[0]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_2, m_iPriority[1]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_3, m_iPriority[2]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_4, m_iPriority[3]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_5, m_iPriority[4]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_6, m_iPriority[5]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_7, m_iPriority[6]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_8, m_iPriority[7]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_9, m_iPriority[8]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_10, m_iPriority[9]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_11, m_iPriority[10]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_12, m_iPriority[11]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_13, m_iPriority[12]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_14, m_iPriority[13]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_15, m_iPriority[14]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_16, m_iPriority[15]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_17, m_iPriority[16]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_18, m_iPriority[17]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_19, m_iPriority[18]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_20, m_iPriority[19]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_21, m_iPriority[20]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_22, m_iPriority[21]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_23, m_iPriority[22]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_24, m_iPriority[23]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_25, m_iPriority[24]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_26, m_iPriority[25]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_27, m_iPriority[26]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_28, m_iPriority[27]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_29, m_iPriority[28]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_30, m_iPriority[29]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_31, m_iPriority[30]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_32, m_iPriority[31]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_33, m_iPriority[32]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_34, m_iPriority[33]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_35, m_iPriority[34]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_36, m_iPriority[35]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_37, m_iPriority[36]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_38, m_iPriority[37]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_39, m_iPriority[38]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_40, m_iPriority[39]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_41, m_iPriority[40]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_42, m_iPriority[41]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_43, m_iPriority[42]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_44, m_iPriority[43]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_45, m_iPriority[44]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_46, m_iPriority[45]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_47, m_iPriority[46]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_48, m_iPriority[47]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_49, m_iPriority[48]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_50, m_iPriority[49]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_51, m_iPriority[50]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_52, m_iPriority[51]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_53, m_iPriority[52]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_54, m_iPriority[53]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_55, m_iPriority[54]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_56, m_iPriority[55]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_57, m_iPriority[56]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_58, m_iPriority[57]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_59, m_iPriority[58]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_60, m_iPriority[59]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_61, m_iPriority[60]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_62, m_iPriority[61]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_63, m_iPriority[62]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_64, m_iPriority[63]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_65, m_iPriority[64]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_66, m_iPriority[65]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_67, m_iPriority[66]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_68, m_iPriority[67]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_69, m_iPriority[68]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_70, m_iPriority[69]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_71, m_iPriority[70]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_72, m_iPriority[71]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_73, m_iPriority[72]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_74, m_iPriority[73]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_75, m_iPriority[74]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_76, m_iPriority[75]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_77, m_iPriority[76]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_78, m_iPriority[77]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_79, m_iPriority[78]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_80, m_iPriority[79]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_81, m_iPriority[80]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_82, m_iPriority[81]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_83, m_iPriority[82]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_84, m_iPriority[83]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_85, m_iPriority[84]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_86, m_iPriority[85]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_87, m_iPriority[86]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_88, m_iPriority[87]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_89, m_iPriority[88]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_90, m_iPriority[89]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_91, m_iPriority[90]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_92, m_iPriority[91]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_93, m_iPriority[92]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_94, m_iPriority[93]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_95, m_iPriority[94]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_96, m_iPriority[95]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_97, m_iPriority[96]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_98, m_iPriority[97]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_99, m_iPriority[98]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_100, m_iPriority[99]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_101, m_iPriority[100]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_102, m_iPriority[101]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_103, m_iPriority[102]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_104, m_iPriority[103]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_105, m_iPriority[104]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_106, m_iPriority[105]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_107, m_iPriority[106]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_108, m_iPriority[107]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_109, m_iPriority[108]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_110, m_iPriority[109]);

	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_1, m_cbSpecialNGCode[0]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_2, m_cbSpecialNGCode[1]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_3, m_cbSpecialNGCode[2]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_4, m_cbSpecialNGCode[3]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_5, m_cbSpecialNGCode[4]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_6, m_cbSpecialNGCode[5]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_7, m_cbSpecialNGCode[6]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_8, m_cbSpecialNGCode[7]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_9, m_cbSpecialNGCode[8]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_10, m_cbSpecialNGCode[9]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_11, m_cbSpecialNGCode[10]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_12, m_cbSpecialNGCode[11]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_13, m_cbSpecialNGCode[12]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_14, m_cbSpecialNGCode[13]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_15, m_cbSpecialNGCode[14]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_16, m_cbSpecialNGCode[15]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_17, m_cbSpecialNGCode[16]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_18, m_cbSpecialNGCode[17]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_19, m_cbSpecialNGCode[18]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_20, m_cbSpecialNGCode[19]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_21, m_cbSpecialNGCode[20]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_22, m_cbSpecialNGCode[21]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_23, m_cbSpecialNGCode[22]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_24, m_cbSpecialNGCode[23]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_25, m_cbSpecialNGCode[24]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_26, m_cbSpecialNGCode[25]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_27, m_cbSpecialNGCode[26]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_28, m_cbSpecialNGCode[27]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_29, m_cbSpecialNGCode[28]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_30, m_cbSpecialNGCode[29]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_31, m_cbSpecialNGCode[30]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_32, m_cbSpecialNGCode[31]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_33, m_cbSpecialNGCode[32]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_34, m_cbSpecialNGCode[33]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_35, m_cbSpecialNGCode[34]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_36, m_cbSpecialNGCode[35]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_37, m_cbSpecialNGCode[36]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_38, m_cbSpecialNGCode[37]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_39, m_cbSpecialNGCode[38]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_40, m_cbSpecialNGCode[39]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_41, m_cbSpecialNGCode[40]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_42, m_cbSpecialNGCode[41]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_43, m_cbSpecialNGCode[42]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_44, m_cbSpecialNGCode[43]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_45, m_cbSpecialNGCode[44]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_46, m_cbSpecialNGCode[45]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_47, m_cbSpecialNGCode[46]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_48, m_cbSpecialNGCode[47]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_49, m_cbSpecialNGCode[48]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_50, m_cbSpecialNGCode[49]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_51, m_cbSpecialNGCode[50]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_52, m_cbSpecialNGCode[51]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_53, m_cbSpecialNGCode[52]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_54, m_cbSpecialNGCode[53]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_55, m_cbSpecialNGCode[54]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_56, m_cbSpecialNGCode[55]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_57, m_cbSpecialNGCode[56]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_58, m_cbSpecialNGCode[57]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_59, m_cbSpecialNGCode[58]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_60, m_cbSpecialNGCode[59]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_61, m_cbSpecialNGCode[60]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_62, m_cbSpecialNGCode[61]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_63, m_cbSpecialNGCode[62]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_64, m_cbSpecialNGCode[63]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_65, m_cbSpecialNGCode[64]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_66, m_cbSpecialNGCode[65]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_67, m_cbSpecialNGCode[66]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_68, m_cbSpecialNGCode[67]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_69, m_cbSpecialNGCode[68]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_70, m_cbSpecialNGCode[69]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_71, m_cbSpecialNGCode[70]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_72, m_cbSpecialNGCode[71]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_73, m_cbSpecialNGCode[72]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_74, m_cbSpecialNGCode[73]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_75, m_cbSpecialNGCode[74]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_76, m_cbSpecialNGCode[75]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_77, m_cbSpecialNGCode[76]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_78, m_cbSpecialNGCode[77]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_79, m_cbSpecialNGCode[78]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_80, m_cbSpecialNGCode[79]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_81, m_cbSpecialNGCode[80]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_82, m_cbSpecialNGCode[81]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_83, m_cbSpecialNGCode[82]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_84, m_cbSpecialNGCode[83]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_85, m_cbSpecialNGCode[84]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_86, m_cbSpecialNGCode[85]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_87, m_cbSpecialNGCode[86]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_88, m_cbSpecialNGCode[87]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_89, m_cbSpecialNGCode[88]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_90, m_cbSpecialNGCode[89]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_91, m_cbSpecialNGCode[90]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_92, m_cbSpecialNGCode[91]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_93, m_cbSpecialNGCode[92]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_94, m_cbSpecialNGCode[93]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_95, m_cbSpecialNGCode[94]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_96, m_cbSpecialNGCode[95]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_97, m_cbSpecialNGCode[96]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_98, m_cbSpecialNGCode[97]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_99, m_cbSpecialNGCode[98]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_100, m_cbSpecialNGCode[99]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_101, m_cbSpecialNGCode[100]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_102, m_cbSpecialNGCode[101]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_103, m_cbSpecialNGCode[102]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_104, m_cbSpecialNGCode[103]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_105, m_cbSpecialNGCode[104]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_106, m_cbSpecialNGCode[105]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_107, m_cbSpecialNGCode[106]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_108, m_cbSpecialNGCode[107]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_109, m_cbSpecialNGCode[108]);
	DDX_Control(pDX, IDC_COMBO_DEFECT_CODE_110, m_cbSpecialNGCode[109]);
}


BEGIN_MESSAGE_MAP(CDefectPriorityDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CDefectPriorityDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDefectPriorityDlg::OnBnClickedCancel)

	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_1, OnSelchangeComboPriority_1)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_2, OnSelchangeComboPriority_2)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_3, OnSelchangeComboPriority_3)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_4, OnSelchangeComboPriority_4)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_5, OnSelchangeComboPriority_5)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_6, OnSelchangeComboPriority_6)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_7, OnSelchangeComboPriority_7)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_8, OnSelchangeComboPriority_8)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_9, OnSelchangeComboPriority_9)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_10, OnSelchangeComboPriority_10)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_11, OnSelchangeComboPriority_11)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_12, OnSelchangeComboPriority_12)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_13, OnSelchangeComboPriority_13)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_14, OnSelchangeComboPriority_14)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_15, OnSelchangeComboPriority_15)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_16, OnSelchangeComboPriority_16)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_17, OnSelchangeComboPriority_17)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_18, OnSelchangeComboPriority_18)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_19, OnSelchangeComboPriority_19)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_20, OnSelchangeComboPriority_20)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_21, OnSelchangeComboPriority_21)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_22, OnSelchangeComboPriority_22)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_23, OnSelchangeComboPriority_23)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_24, OnSelchangeComboPriority_24)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_25, OnSelchangeComboPriority_25)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_26, OnSelchangeComboPriority_26)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_27, OnSelchangeComboPriority_27)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_28, OnSelchangeComboPriority_28)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_29, OnSelchangeComboPriority_29)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_30, OnSelchangeComboPriority_30)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_31, OnSelchangeComboPriority_31)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_32, OnSelchangeComboPriority_32)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_33, OnSelchangeComboPriority_33)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_34, OnSelchangeComboPriority_34)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_35, OnSelchangeComboPriority_35)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_36, OnSelchangeComboPriority_36)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_37, OnSelchangeComboPriority_37)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_38, OnSelchangeComboPriority_38)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_39, OnSelchangeComboPriority_39)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_40, OnSelchangeComboPriority_40)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_41, OnSelchangeComboPriority_41)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_42, OnSelchangeComboPriority_42)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_43, OnSelchangeComboPriority_43)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_44, OnSelchangeComboPriority_44)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_45, OnSelchangeComboPriority_45)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_46, OnSelchangeComboPriority_46)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_47, OnSelchangeComboPriority_47)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_48, OnSelchangeComboPriority_48)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_49, OnSelchangeComboPriority_49)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_50, OnSelchangeComboPriority_50)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_51, OnSelchangeComboPriority_51)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_52, OnSelchangeComboPriority_52)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_53, OnSelchangeComboPriority_53)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_54, OnSelchangeComboPriority_54)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_55, OnSelchangeComboPriority_55)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_56, OnSelchangeComboPriority_56)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_57, OnSelchangeComboPriority_57)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_58, OnSelchangeComboPriority_58)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_59, OnSelchangeComboPriority_59)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_60, OnSelchangeComboPriority_60)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_61, OnSelchangeComboPriority_61)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_62, OnSelchangeComboPriority_62)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_63, OnSelchangeComboPriority_63)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_64, OnSelchangeComboPriority_64)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_65, OnSelchangeComboPriority_65)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_66, OnSelchangeComboPriority_66)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_67, OnSelchangeComboPriority_67)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_68, OnSelchangeComboPriority_68)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_69, OnSelchangeComboPriority_69)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_70, OnSelchangeComboPriority_70)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_71, OnSelchangeComboPriority_71)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_72, OnSelchangeComboPriority_72)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_73, OnSelchangeComboPriority_73)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_74, OnSelchangeComboPriority_74)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_75, OnSelchangeComboPriority_75)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_76, OnSelchangeComboPriority_76)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_77, OnSelchangeComboPriority_77)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_78, OnSelchangeComboPriority_78)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_79, OnSelchangeComboPriority_79)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_80, OnSelchangeComboPriority_80)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_81, OnSelchangeComboPriority_81)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_82, OnSelchangeComboPriority_82)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_83, OnSelchangeComboPriority_83)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_84, OnSelchangeComboPriority_84)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_85, OnSelchangeComboPriority_85)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_86, OnSelchangeComboPriority_86)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_87, OnSelchangeComboPriority_87)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_88, OnSelchangeComboPriority_88)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_89, OnSelchangeComboPriority_89)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_90, OnSelchangeComboPriority_90)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_91, OnSelchangeComboPriority_91)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_92, OnSelchangeComboPriority_92)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_93, OnSelchangeComboPriority_93)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_94, OnSelchangeComboPriority_94)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_95, OnSelchangeComboPriority_95)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_96, OnSelchangeComboPriority_96)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_97, OnSelchangeComboPriority_97)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_98, OnSelchangeComboPriority_98)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_99, OnSelchangeComboPriority_99)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_100, OnSelchangeComboPriority_100)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_101, OnSelchangeComboPriority_101)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_102, OnSelchangeComboPriority_102)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_103, OnSelchangeComboPriority_103)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_104, OnSelchangeComboPriority_104)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_105, OnSelchangeComboPriority_105)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_106, OnSelchangeComboPriority_106)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_107, OnSelchangeComboPriority_107)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_108, OnSelchangeComboPriority_108)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_109, OnSelchangeComboPriority_109)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_110, OnSelchangeComboPriority_110)
END_MESSAGE_MAP()


// CDefectPriorityDlg 메시지 처리기입니다.

void CDefectPriorityDlg::SetInspectionType(int iInspectionType)
{
	m_iCurInspectionType = iInspectionType;
}

BOOL CDefectPriorityDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	int i, j;
	CString sTemp;

	for (i = 0; i < MAX_DEFECT_NAME; i++)
	{
		m_cbPriority[i].ResetContent();

		for (j = 0; j < MAX_DEFECT_NAME; j++)
		{
			sTemp.Format("%d", j + 1);
			m_cbPriority[i].AddString(sTemp);
		}
	}

	CString sItemName;

	SetDefectName();

	for (int i = 0; i < MAX_DEFECT_NAME; i++)
	{
		m_iPriority[i] = THEAPP.Struct_PreferenceStruct.iDefectPriority[i];
	}

	CString sSpecialNGCode;
	int iComboIdx;

	for (i = 0; i < MAX_DEFECT_NAME; i++)
	{
		// ComboBox가 DDX_Control로 윈도우 핸들에 연결되었는지 확인 (크래시 방지)
		if (m_cbSpecialNGCode[i].GetSafeHwnd() == NULL)
			continue;

		// 특수 NG 코드 목록을 ComboBox에 추가 (_T() 매크로 불필요 - CString은 자동 변환됨)
		for (j = 0; j < THEAPP.Struct_PreferenceStruct.iNoAssignSpecialCode; j++)
			m_cbSpecialNGCode[i].AddString(THEAPP.Struct_PreferenceStruct.m_sAssignSpecialCode[j]);

		// 저장된 선택 인덱스로 ComboBox 설정
		iComboIdx = THEAPP.Struct_PreferenceStruct.m_iSpecialNGCodeIndex[i];
		m_cbSpecialNGCode[i].SetCurSel(iComboIdx);
	}

	ChangeLanguage();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDefectPriorityDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("불량 항목 우선 순위 및 표시 색상 설정"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_1, _T("불량명"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_2, _T("스페셜 불량"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_3, _T("우선 순위"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_4, _T("불량명"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_5, _T("스페셜 불량"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_6, _T("우선 순위"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_7, _T("불량명"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_8, _T("스페셜 불량"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_9, _T("우선 순위"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_10, _T("불량명"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_11, _T("스페셜 불량"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_12, _T("우선 순위"));
		SetDlgItemText(IDOK, _T("저장"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Set defect type priority and display color"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_1, _T("Inspection type"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_2, _T("Special code"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_3, _T("Priority"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_4, _T("Inspection type"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_5, _T("Special code"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_6, _T("Priority"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_7, _T("Inspection type"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_8, _T("Special code"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_9, _T("Priority"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_10, _T("Inspection type"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_11, _T("Special code"));
		SetDlgItemText(IDC_STATIC_DEFECTPRIORITYDLG_12, _T("Priority"));
		SetDlgItemText(IDOK, _T("Save"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}


void CDefectPriorityDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();

	int i;
	int iComboIdx;

	for (i = 0; i < MAX_DEFECT_NAME; i++)
	{
		THEAPP.Struct_PreferenceStruct.iDefectPriority[i] = m_iPriority[i];

		iComboIdx = m_cbSpecialNGCode[i].GetCurSel();
		THEAPP.Struct_PreferenceStruct.m_iSpecialNGCodeIndex[i] = iComboIdx;
	}

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";

	CString sDefectInfoFileName;
	sDefectInfoFileName = strModelFolder + "DefectInfo.ini";
	CIniFileCS INI(sDefectInfoFileName);

	CString strSection, strTemp;
	CString sTemp;

	strSection = "Defect_Priority";

	for (i = 0; i < MAX_DEFECT_NAME; i++)
	{
		sTemp.Format("NG_PRIORITY_%d", i);
		INI.Set_Integer(strSection, sTemp, m_iPriority[i]);
	}

	strSection = "Defect_Special";

	for (i = 0; i < MAX_DEFECT_NAME; i++)
	{
		sTemp.Format("NG_SPECIAL_CODE_%d", i);
		INI.Set_Integer(strSection, sTemp, THEAPP.Struct_PreferenceStruct.m_iSpecialNGCodeIndex[i]);
	}

	CDialog::OnOK();
}


void CDefectPriorityDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}

void CDefectPriorityDlg::OnSelchangeComboPriority_1()
{
	ChangePriority(0);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_2()
{
	ChangePriority(1);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_3()
{
	ChangePriority(2);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_4()
{
	ChangePriority(3);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_5()
{
	ChangePriority(4);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_6()
{
	ChangePriority(5);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_7()
{
	ChangePriority(6);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_8()
{
	ChangePriority(7);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_9()
{
	ChangePriority(8);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_10()
{
	ChangePriority(9);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_11()
{
	ChangePriority(10);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_12()
{
	ChangePriority(11);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_13()
{
	ChangePriority(12);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_14()
{
	ChangePriority(13);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_15()
{
	ChangePriority(14);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_16()
{
	ChangePriority(15);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_17()
{
	ChangePriority(16);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_18()
{
	ChangePriority(17);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_19()
{
	ChangePriority(18);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_20()
{
	ChangePriority(19);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_21()
{
	ChangePriority(20);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_22()
{
	ChangePriority(21);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_23()
{
	ChangePriority(22);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_24()
{
	ChangePriority(23);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_25()
{
	ChangePriority(24);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_26()
{
	ChangePriority(25);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_27()
{
	ChangePriority(26);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_28()
{
	ChangePriority(27);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_29()
{
	ChangePriority(28);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_30()
{
	ChangePriority(29);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_31()
{
	ChangePriority(30);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_32()
{
	ChangePriority(31);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_33()
{
	ChangePriority(32);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_34()
{
	ChangePriority(33);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_35()
{
	ChangePriority(34);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_36()
{
	ChangePriority(35);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_37()
{
	ChangePriority(36);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_38()
{
	ChangePriority(37);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_39()
{
	ChangePriority(38);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_40()
{
	ChangePriority(39);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_41()
{
	ChangePriority(40);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_42()
{
	ChangePriority(41);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_43()
{
	ChangePriority(42);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_44()
{
	ChangePriority(43);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_45()
{
	ChangePriority(44);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_46()
{
	ChangePriority(45);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_47()
{
	ChangePriority(46);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_48()
{
	ChangePriority(47);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_49()
{
	ChangePriority(48);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_50()
{
	ChangePriority(49);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_51()
{
	ChangePriority(50);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_52()
{
	ChangePriority(51);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_53()
{
	ChangePriority(52);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_54()
{
	ChangePriority(53);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_55()
{
	ChangePriority(54);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_56()
{
	ChangePriority(55);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_57()
{
	ChangePriority(56);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_58()
{
	ChangePriority(57);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_59()
{
	ChangePriority(58);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_60()
{
	ChangePriority(59);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_61()
{
	ChangePriority(60);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_62()
{
	ChangePriority(61);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_63()
{
	ChangePriority(62);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_64()
{
	ChangePriority(63);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_65()
{
	ChangePriority(64);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_66()
{
	ChangePriority(65);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_67()
{
	ChangePriority(66);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_68()
{
	ChangePriority(67);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_69()
{
	ChangePriority(68);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_70()
{
	ChangePriority(69);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_71()
{
	ChangePriority(70);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_72()
{
	ChangePriority(71);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_73()
{
	ChangePriority(72);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_74()
{
	ChangePriority(73);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_75()
{
	ChangePriority(74);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_76()
{
	ChangePriority(75);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_77()
{
	ChangePriority(76);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_78()
{
	ChangePriority(77);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_79()
{
	ChangePriority(78);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_80()
{
	ChangePriority(79);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_81()
{
	ChangePriority(80);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_82()
{
	ChangePriority(81);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_83()
{
	ChangePriority(82);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_84()
{
	ChangePriority(83);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_85()
{
	ChangePriority(84);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_86()
{
	ChangePriority(85);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_87()
{
	ChangePriority(86);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_88()
{
	ChangePriority(87);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_89()
{
	ChangePriority(88);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_90()
{
	ChangePriority(89);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_91()
{
	ChangePriority(90);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_92()
{
	ChangePriority(91);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_93()
{
	ChangePriority(92);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_94()
{
	ChangePriority(93);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_95()
{
	ChangePriority(94);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_96()
{
	ChangePriority(95);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_97()
{
	ChangePriority(96);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_98()
{
	ChangePriority(97);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_99()
{
	ChangePriority(98);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_100()
{
	ChangePriority(99);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_101()
{
	ChangePriority(100);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_102()
{
	ChangePriority(101);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_103()
{
	ChangePriority(102);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_104()
{
	ChangePriority(103);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_105()
{
	ChangePriority(104);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_106()
{
	ChangePriority(105);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_107()
{
	ChangePriority(106);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_108()
{
	ChangePriority(107);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_109()
{
	ChangePriority(108);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_110()
{
	ChangePriority(109);
}

void CDefectPriorityDlg::ChangePriority(int iSelectedIndex)
{
	int i;
	int temp[MAX_DEFECT_NAME];

	for (i = 0; i < MAX_DEFECT_NAME; i++)
	{
		if (i == iSelectedIndex)
			continue;
		temp[i] = m_iPriority[i];
	}

	for (i = 0; i < MAX_DEFECT_NAME; i++)
	{
		if (i == iSelectedIndex)
			continue;

		if (temp[i] >= m_iPriority[iSelectedIndex])
			--(temp[i]);
	}

	UpdateData(TRUE);

	for (i = 0; i < MAX_DEFECT_NAME; i++)
	{
		if (i == iSelectedIndex)
			continue;

		if (temp[i] >= m_iPriority[iSelectedIndex])
			++(temp[i]);
	}

	for (i = 0; i < MAX_DEFECT_NAME; i++)
	{
		if (i == iSelectedIndex)
			continue;

		m_iPriority[i] = temp[i];
	}

	UpdateData(FALSE);
}

void CDefectPriorityDlg::SetDefectName()
{
	GetDlgItem(IDC_STATIC_DEFECT_NAME_1)->SetWindowTextA(THEAPP.m_strDefectName[0]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_2)->SetWindowTextA(THEAPP.m_strDefectName[1]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_3)->SetWindowTextA(THEAPP.m_strDefectName[2]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_4)->SetWindowTextA(THEAPP.m_strDefectName[3]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_5)->SetWindowTextA(THEAPP.m_strDefectName[4]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_6)->SetWindowTextA(THEAPP.m_strDefectName[5]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_7)->SetWindowTextA(THEAPP.m_strDefectName[6]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_8)->SetWindowTextA(THEAPP.m_strDefectName[7]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_9)->SetWindowTextA(THEAPP.m_strDefectName[8]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_10)->SetWindowTextA(THEAPP.m_strDefectName[9]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_11)->SetWindowTextA(THEAPP.m_strDefectName[10]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_12)->SetWindowTextA(THEAPP.m_strDefectName[11]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_13)->SetWindowTextA(THEAPP.m_strDefectName[12]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_14)->SetWindowTextA(THEAPP.m_strDefectName[13]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_15)->SetWindowTextA(THEAPP.m_strDefectName[14]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_16)->SetWindowTextA(THEAPP.m_strDefectName[15]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_17)->SetWindowTextA(THEAPP.m_strDefectName[16]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_18)->SetWindowTextA(THEAPP.m_strDefectName[17]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_19)->SetWindowTextA(THEAPP.m_strDefectName[18]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_20)->SetWindowTextA(THEAPP.m_strDefectName[19]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_21)->SetWindowTextA(THEAPP.m_strDefectName[20]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_22)->SetWindowTextA(THEAPP.m_strDefectName[21]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_23)->SetWindowTextA(THEAPP.m_strDefectName[22]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_24)->SetWindowTextA(THEAPP.m_strDefectName[23]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_25)->SetWindowTextA(THEAPP.m_strDefectName[24]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_26)->SetWindowTextA(THEAPP.m_strDefectName[25]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_27)->SetWindowTextA(THEAPP.m_strDefectName[26]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_28)->SetWindowTextA(THEAPP.m_strDefectName[27]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_29)->SetWindowTextA(THEAPP.m_strDefectName[28]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_30)->SetWindowTextA(THEAPP.m_strDefectName[29]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_31)->SetWindowTextA(THEAPP.m_strDefectName[30]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_32)->SetWindowTextA(THEAPP.m_strDefectName[31]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_33)->SetWindowTextA(THEAPP.m_strDefectName[32]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_34)->SetWindowTextA(THEAPP.m_strDefectName[33]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_35)->SetWindowTextA(THEAPP.m_strDefectName[34]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_36)->SetWindowTextA(THEAPP.m_strDefectName[35]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_37)->SetWindowTextA(THEAPP.m_strDefectName[36]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_38)->SetWindowTextA(THEAPP.m_strDefectName[37]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_39)->SetWindowTextA(THEAPP.m_strDefectName[38]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_40)->SetWindowTextA(THEAPP.m_strDefectName[39]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_41)->SetWindowTextA(THEAPP.m_strDefectName[40]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_42)->SetWindowTextA(THEAPP.m_strDefectName[41]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_43)->SetWindowTextA(THEAPP.m_strDefectName[42]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_44)->SetWindowTextA(THEAPP.m_strDefectName[43]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_45)->SetWindowTextA(THEAPP.m_strDefectName[44]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_46)->SetWindowTextA(THEAPP.m_strDefectName[45]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_47)->SetWindowTextA(THEAPP.m_strDefectName[46]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_48)->SetWindowTextA(THEAPP.m_strDefectName[47]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_49)->SetWindowTextA(THEAPP.m_strDefectName[48]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_50)->SetWindowTextA(THEAPP.m_strDefectName[49]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_51)->SetWindowTextA(THEAPP.m_strDefectName[50]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_52)->SetWindowTextA(THEAPP.m_strDefectName[51]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_53)->SetWindowTextA(THEAPP.m_strDefectName[52]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_54)->SetWindowTextA(THEAPP.m_strDefectName[53]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_55)->SetWindowTextA(THEAPP.m_strDefectName[54]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_56)->SetWindowTextA(THEAPP.m_strDefectName[55]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_57)->SetWindowTextA(THEAPP.m_strDefectName[56]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_58)->SetWindowTextA(THEAPP.m_strDefectName[57]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_59)->SetWindowTextA(THEAPP.m_strDefectName[58]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_60)->SetWindowTextA(THEAPP.m_strDefectName[59]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_61)->SetWindowTextA(THEAPP.m_strDefectName[60]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_62)->SetWindowTextA(THEAPP.m_strDefectName[61]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_63)->SetWindowTextA(THEAPP.m_strDefectName[62]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_64)->SetWindowTextA(THEAPP.m_strDefectName[63]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_65)->SetWindowTextA(THEAPP.m_strDefectName[64]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_66)->SetWindowTextA(THEAPP.m_strDefectName[65]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_67)->SetWindowTextA(THEAPP.m_strDefectName[66]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_68)->SetWindowTextA(THEAPP.m_strDefectName[67]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_69)->SetWindowTextA(THEAPP.m_strDefectName[68]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_70)->SetWindowTextA(THEAPP.m_strDefectName[69]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_71)->SetWindowTextA(THEAPP.m_strDefectName[70]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_72)->SetWindowTextA(THEAPP.m_strDefectName[71]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_73)->SetWindowTextA(THEAPP.m_strDefectName[72]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_74)->SetWindowTextA(THEAPP.m_strDefectName[73]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_75)->SetWindowTextA(THEAPP.m_strDefectName[74]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_76)->SetWindowTextA(THEAPP.m_strDefectName[75]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_77)->SetWindowTextA(THEAPP.m_strDefectName[76]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_78)->SetWindowTextA(THEAPP.m_strDefectName[77]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_79)->SetWindowTextA(THEAPP.m_strDefectName[78]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_80)->SetWindowTextA(THEAPP.m_strDefectName[79]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_81)->SetWindowTextA(THEAPP.m_strDefectName[80]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_82)->SetWindowTextA(THEAPP.m_strDefectName[81]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_83)->SetWindowTextA(THEAPP.m_strDefectName[82]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_84)->SetWindowTextA(THEAPP.m_strDefectName[83]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_85)->SetWindowTextA(THEAPP.m_strDefectName[84]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_86)->SetWindowTextA(THEAPP.m_strDefectName[85]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_87)->SetWindowTextA(THEAPP.m_strDefectName[86]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_88)->SetWindowTextA(THEAPP.m_strDefectName[87]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_89)->SetWindowTextA(THEAPP.m_strDefectName[88]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_90)->SetWindowTextA(THEAPP.m_strDefectName[89]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_91)->SetWindowTextA(THEAPP.m_strDefectName[90]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_92)->SetWindowTextA(THEAPP.m_strDefectName[91]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_93)->SetWindowTextA(THEAPP.m_strDefectName[92]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_94)->SetWindowTextA(THEAPP.m_strDefectName[93]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_95)->SetWindowTextA(THEAPP.m_strDefectName[94]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_96)->SetWindowTextA(THEAPP.m_strDefectName[95]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_97)->SetWindowTextA(THEAPP.m_strDefectName[96]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_98)->SetWindowTextA(THEAPP.m_strDefectName[97]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_99)->SetWindowTextA(THEAPP.m_strDefectName[98]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_100)->SetWindowTextA(THEAPP.m_strDefectName[99]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_101)->SetWindowTextA(THEAPP.m_strDefectName[100]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_102)->SetWindowTextA(THEAPP.m_strDefectName[101]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_103)->SetWindowTextA(THEAPP.m_strDefectName[102]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_104)->SetWindowTextA(THEAPP.m_strDefectName[103]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_105)->SetWindowTextA(THEAPP.m_strDefectName[104]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_106)->SetWindowTextA(THEAPP.m_strDefectName[105]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_107)->SetWindowTextA(THEAPP.m_strDefectName[106]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_108)->SetWindowTextA(THEAPP.m_strDefectName[107]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_109)->SetWindowTextA(THEAPP.m_strDefectName[108]);
	GetDlgItem(IDC_STATIC_DEFECT_NAME_110)->SetWindowTextA(THEAPP.m_strDefectName[109]);
}