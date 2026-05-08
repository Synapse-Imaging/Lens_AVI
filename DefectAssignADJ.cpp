// DefectAssignADJ.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "DefectAssignADJ.h"
#include "afxdialogex.h"


// CDefectAssignADJ 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDefectAssignADJ, CDialogEx)

CDefectAssignADJ::CDefectAssignADJ(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDefectAssignADJ::IDD, pParent)
{

}

CDefectAssignADJ::~CDefectAssignADJ()
{
}

void CDefectAssignADJ::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ClientNO, m_comboClientNO);

	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_1, m_comboVision[0]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_2, m_comboVision[1]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_3, m_comboVision[2]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_4, m_comboVision[3]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_5, m_comboVision[4]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_6, m_comboVision[5]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_7, m_comboVision[6]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_8, m_comboVision[7]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_9, m_comboVision[8]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_10, m_comboVision[9]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_11, m_comboVision[10]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_12, m_comboVision[11]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_13, m_comboVision[12]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_14, m_comboVision[13]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_15, m_comboVision[14]);
	DDX_Control(pDX, IDC_COMBO_DEFECTASSIGNADJ_VISION_16, m_comboVision[15]);

	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_1, m_strDefectPos[0]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_2, m_strDefectPos[1]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_3, m_strDefectPos[2]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_4, m_strDefectPos[3]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_5, m_strDefectPos[4]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_6, m_strDefectPos[5]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_7, m_strDefectPos[6]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_8, m_strDefectPos[7]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_9, m_strDefectPos[8]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_10, m_strDefectPos[9]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_11, m_strDefectPos[10]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_12, m_strDefectPos[11]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_13, m_strDefectPos[12]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_14, m_strDefectPos[13]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_15, m_strDefectPos[14]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_POSITION_16, m_strDefectPos[15]);



	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_1, m_strDefectName[0]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_2, m_strDefectName[1]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_3, m_strDefectName[2]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_4, m_strDefectName[3]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_5, m_strDefectName[4]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_6, m_strDefectName[5]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_7, m_strDefectName[6]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_8, m_strDefectName[7]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_9, m_strDefectName[8]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_10, m_strDefectName[9]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_11, m_strDefectName[10]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_12, m_strDefectName[11]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_13, m_strDefectName[12]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_14, m_strDefectName[13]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_15, m_strDefectName[14]);
	DDX_Text(pDX, IDC_EDIT_DEFECTASSIGNADJ_DEFECTNAME_16, m_strDefectName[15]);

	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_1, m_bCheckADJOption[0]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_2, m_bCheckADJOption[1]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_3, m_bCheckADJOption[2]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_4, m_bCheckADJOption[3]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_5, m_bCheckADJOption[4]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_6, m_bCheckADJOption[5]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_7, m_bCheckADJOption[6]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_8, m_bCheckADJOption[7]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_9, m_bCheckADJOption[8]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_10, m_bCheckADJOption[9]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_11, m_bCheckADJOption[10]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_12, m_bCheckADJOption[11]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_13, m_bCheckADJOption[12]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_14, m_bCheckADJOption[13]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_15, m_bCheckADJOption[14]);
	DDX_Check(pDX, IDC_CHECK_DEFECTASSIGNADJ_OPTION_16, m_bCheckADJOption[15]);

}


BEGIN_MESSAGE_MAP(CDefectAssignADJ, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDefectAssignADJ::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDefectAssignADJ::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_Apply, &CDefectAssignADJ::OnBnClickedButtonApply)
	ON_CBN_SELCHANGE(IDC_COMBO_ClientNO, &CDefectAssignADJ::OnCbnSelchangeComboClientno)
END_MESSAGE_MAP()


// CDefectAssignADJ 메시지 처리기입니다.

void CDefectAssignADJ::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CDefectAssignADJ::OnBnClickedOk()
{

	OnBnClickedButtonApply();
	CDialogEx::OnOK();
}


BOOL CDefectAssignADJ::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_comboClientNO.AddString(_T("ADJ_1-1"));                   //텍스트 추가
	m_comboClientNO.AddString(_T("ADJ_1-2"));
	m_comboClientNO.AddString(_T("ADJ_2-1"));
	m_comboClientNO.AddString(_T("ADJ_2-2"));
	m_comboClientNO.SetCurSel(0);

	for (int i = 0; i < DEEP_MODEL_VIEW; i++)
	{
		m_comboVision[i].AddString(_T("TOP"));
		m_comboVision[i].AddString(_T("BOTTOM"));
	}

	nClientNO = 0;
#ifdef USE_SUAKIT
	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			m_str2ArrayName[i][j] = THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Name[i][j];
			m_str2ArrayPos[i][j] = THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Pos[i][j];
			m_n2ArrayThreadNO[i][j] = THEAPP.Struct_PreferenceStruct.m_nADJThreadNO[i][j];
			m_bADJOption[i][j] = THEAPP.Struct_PreferenceStruct.m_bSideTotal[i][j];
		}
	}

	for (int i = 0; i < DEEP_MODEL_VIEW; i++)
	{
		m_strDefectPos[i] = THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Pos[0][i];
		m_strDefectName[i] = THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Name[0][i];
		m_comboVision[i].SetCurSel(THEAPP.Struct_PreferenceStruct.m_nADJThreadNO[0][i]);
		m_bCheckADJOption[i] = THEAPP.Struct_PreferenceStruct.m_bSideTotal[0][i];
	}

#endif

	ChangeLanguage();

	UpdateData(FALSE);

	return TRUE;
}

void CDefectAssignADJ::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("ADJ로 보낼 불량 선택"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_1, _T("ADJ 서버"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_2, _T("비전"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_3, _T("검사 대상"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_4, _T("불량(코드)"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_5, _T("옵션"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_6, _T("모델 1"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_7, _T("모델 2"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_8, _T("모델 3"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_9, _T("모델 4"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_10, _T("모델 5"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_11, _T("모델 6"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_12, _T("모델 7"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_13, _T("모델 8"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_14, _T("모델 9"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_15, _T("모델 10"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_16, _T("모델 11"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_17, _T("모델 12"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_18, _T("모델 13"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_19, _T("모델 14"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_20, _T("모델 15"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_21, _T("모델 16"));
		SetDlgItemText(IDOK, _T("저장"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Select defects to be judged"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_1, _T("ADJ Server"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_2, _T("Vision"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_3, _T("Inspection type"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_4, _T("Defect(Code)"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_5, _T("Option"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_6, _T("Model 1"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_7, _T("Model 2"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_8, _T("Model 3"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_9, _T("Model 4"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_10, _T("Model 5"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_11, _T("Model 6"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_12, _T("Model 7"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_13, _T("Model 8"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_14, _T("Model 9"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_15, _T("Model 10"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_16, _T("Model 11"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_17, _T("Model 12"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_18, _T("Model 13"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_19, _T("Model 14"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_20, _T("Model 15"));
		SetDlgItemText(IDC_STATIC_DEFECTASSIGNADJDLG_21, _T("Model 16"));
		SetDlgItemText(IDOK, _T("Save"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}


void CDefectAssignADJ::OnBnClickedButtonApply()
{
#ifdef USE_SUAKIT
	UpdateData();

	for (int j = 0; j < DEEP_MODEL_VIEW; j++)
	{
		m_str2ArrayName[(nClientNO >= 2) ? 1 : 0][j + (nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW] = m_strDefectName[j];
		m_str2ArrayPos[(nClientNO >= 2) ? 1 : 0][j + (nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW] = m_strDefectPos[j];
		m_n2ArrayThreadNO[(nClientNO >= 2) ? 1 : 0][j + (nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW] = m_comboVision[j].GetCurSel();
		m_bADJOption[(nClientNO >= 2) ? 1 : 0][j + (nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW] = m_bCheckADJOption[j];
	}

	nClientNO = m_comboClientNO.GetCurSel();

	for (int j = 0; j < DEEP_MODEL_VIEW; j++)
	{
		m_strDefectPos[j] = m_str2ArrayPos[(nClientNO >= 2) ? 1 : 0][j + (nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW];
		m_strDefectName[j] = m_str2ArrayName[(nClientNO >= 2) ? 1 : 0][j + (nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW];
		m_comboVision[j].SetCurSel(m_n2ArrayThreadNO[(nClientNO >= 2) ? 1 : 0][j + (nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW]);
		m_bCheckADJOption[j] = m_bADJOption[(nClientNO >= 2) ? 1 : 0][j + (nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW];
	}

	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Name[i][j] = m_str2ArrayName[i][j];
			THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Pos[i][j] = m_str2ArrayPos[i][j];
			THEAPP.Struct_PreferenceStruct.m_nADJThreadNO[i][j] = m_n2ArrayThreadNO[i][j];
			THEAPP.Struct_PreferenceStruct.m_bSideTotal[i][j] = m_bADJOption[i][j];
		}
	}


	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI(strModelFolder + "\\ADJDefectSetting.ini");
	CString strSection, strTemp;
	CString sTemp;

	strSection = "Defect_ADJ_Assign";

	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			sTemp.Format("ADJ_Defect_Name_%d", DEEP_MODEL_NUM*i + j + 1);
			INI.Set_String(strSection, sTemp, m_str2ArrayName[i][j]);
			sTemp.Format("ADJ_Defect_Pos_%d", DEEP_MODEL_NUM*i + j + 1);
			INI.Set_String(strSection, sTemp, m_str2ArrayPos[i][j]);
			sTemp.Format("ADJ_Defect_ThreadNO_%d", DEEP_MODEL_NUM*i + j + 1);
			INI.Set_Integer(strSection, sTemp, m_n2ArrayThreadNO[i][j]);
			sTemp.Format("ADJ_Defect_SideTotal_%d", DEEP_MODEL_NUM*i + j + 1);
			INI.Set_Bool(strSection, sTemp, m_bADJOption[i][j]);
		}
	}

	strSection = "Total_defect_ADJ";
	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			int k = 0;
			sTemp.Format("ADJ_Integrated_%d", DEEP_MODEL_NUM*i + j + 1);

			CString Integrated_Defect_Code = INI.Get_String(strSection, sTemp, "none");
			char* sep = ",";
			char* sep2 = "_";
			char *psTemp = (LPTSTR)(LPCTSTR)Integrated_Defect_Code;
			char *Division_ADJ_Defect_Code = strtok_s(psTemp, sep, &psTemp);
			while (Division_ADJ_Defect_Code != NULL)
			{
				char* ADJCode = strtok(Division_ADJ_Defect_Code, sep2);
				THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Integrated_Pos[i][j][k] = strtok(NULL, sep2);
				THEAPP.Struct_PreferenceStruct.m_strADJ_Defect_Integrated_Name[i][j][k] = strtok(NULL, sep2);

				for (int l = 0; l < VISION_NUMBER_MAX; l++)
				{
					CString sVisionCamType_Short;
					sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[l];
					if (sVisionCamType_Short == ADJCode)
					{
						THEAPP.Struct_PreferenceStruct.m_nADJThreadNO_Integrated[i][j][k] = l;
						break;
					}
				}
				k++;
				Division_ADJ_Defect_Code = strtok_s(NULL, sep, &psTemp);
			}
		}
	}

	strSection = "FAI_ITEM_MODEL";
	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			int k = 0;
			sTemp.Format("FAI_ADJ_Integrated_MODEL_%d", DEEP_MODEL_NUM*i + j + 1);

			CString cstrFAI_ADJ_Integrated_Model = INI.Get_String(strSection, sTemp, "none");
			char* sep = ",";
			char *psTemp = (LPTSTR)(LPCTSTR)cstrFAI_ADJ_Integrated_Model;
			char *m_pchFAI_ADJ_Integrated_Model = strtok_s(psTemp, sep, &psTemp);
			while (m_pchFAI_ADJ_Integrated_Model != "")
			{
				THEAPP.Struct_PreferenceStruct.m_nADJFAIModel_Integrated[i][j][k] = atoi(strtok(m_pchFAI_ADJ_Integrated_Model, sep));

				k++;
				m_pchFAI_ADJ_Integrated_Model = strtok_s(NULL, sep, &psTemp);
			}
		}
	}
#endif
}


void CDefectAssignADJ::OnCbnSelchangeComboClientno()
{
	UpdateData(TRUE);

	for (int j = 0; j < DEEP_MODEL_VIEW; j++)
	{
		m_str2ArrayName[(nClientNO >= 2) ? 1 : 0][j + ((nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW)] = m_strDefectName[j];
		m_str2ArrayPos[(nClientNO >= 2) ? 1 : 0][j + ((nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW)] = m_strDefectPos[j];
		m_n2ArrayThreadNO[(nClientNO >= 2) ? 1 : 0][j + ((nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW)] = m_comboVision[j].GetCurSel();
		m_bADJOption[(nClientNO >= 2) ? 1 : 0][j + ((nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW)] = m_bCheckADJOption[j];
	}

	nClientNO = m_comboClientNO.GetCurSel();

	for (int j = 0; j < DEEP_MODEL_VIEW; j++)
	{
		m_strDefectPos[j] = m_str2ArrayPos[(nClientNO >= 2) ? 1 : 0][j + ((nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW)];
		m_strDefectName[j] = m_str2ArrayName[(nClientNO >= 2) ? 1 : 0][j + ((nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW)];
		m_comboVision[j].SetCurSel(m_n2ArrayThreadNO[(nClientNO >= 2) ? 1 : 0][j + ((nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW)]);
		m_bCheckADJOption[j] = m_bADJOption[(nClientNO >= 2) ? 1 : 0][j + ((nClientNO % MAX_ADJ_CONNECT_NUM) * DEEP_MODEL_VIEW)];
	}

	UpdateData(FALSE);
}
