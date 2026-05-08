// MacroPathList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "MacroPathList.h"
#include "afxdialogex.h"

// CMacroPathList 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMacroPathList, CDialogEx)

CMacroPathList::CMacroPathList(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMacroPathList::IDD, pParent)
{
}

CMacroPathList::~CMacroPathList()
{
}

void CMacroPathList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LOT_ID_MODE, m_comboLotIdMode);
	DDX_Text(pDX, IDC_EDIT_LOT_ID_LENGTH, m_strLotIdLength);

	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_1, m_strMacroPath[0]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_2, m_strMacroPath[1]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_3, m_strMacroPath[2]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_4, m_strMacroPath[3]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_5, m_strMacroPath[4]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_6, m_strMacroPath[5]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_7, m_strMacroPath[6]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_8, m_strMacroPath[7]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_9, m_strMacroPath[8]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_10, m_strMacroPath[9]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_11, m_strMacroPath[10]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_12, m_strMacroPath[11]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_13, m_strMacroPath[12]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_14, m_strMacroPath[13]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_15, m_strMacroPath[14]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_16, m_strMacroPath[15]);

	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_17, m_strMacroPath[16]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_18, m_strMacroPath[17]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_19, m_strMacroPath[18]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_20, m_strMacroPath[19]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_21, m_strMacroPath[20]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_22, m_strMacroPath[21]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_23, m_strMacroPath[22]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_24, m_strMacroPath[23]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_25, m_strMacroPath[24]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_26, m_strMacroPath[25]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_27, m_strMacroPath[26]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_28, m_strMacroPath[27]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_29, m_strMacroPath[28]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_30, m_strMacroPath[29]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_31, m_strMacroPath[30]);
	DDX_Text(pDX, IDC_EDIT_MACRO_PATH_32, m_strMacroPath[31]);
}


BEGIN_MESSAGE_MAP(CMacroPathList, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_MACRO_PATH_SAVE, &CMacroPathList::OnBnClickedButtonMacroPathSave)
	ON_BN_CLICKED(IDC_BUTTON_MACRO_PATH_SAVE_CLOSE, &CMacroPathList::OnBnClickedButtonMacroPathSaveClose)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_INI, &CMacroPathList::OnBnClickedButtonMakeINI)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_INSPECT, &CMacroPathList::OnBnClickedButtonMakeAndInspect)
	ON_CBN_SELCHANGE(IDC_COMBO_LOT_ID_MODE, &CMacroPathList::OnCbnSelchangeComboLotIdMode)
	ON_BN_CLICKED(IDCANCEL, &CMacroPathList::OnBnClickedCancel)
END_MESSAGE_MAP()

// CMacroPathList 메시지 처리기입니다.
void CMacroPathList::OnBnClickedCancel()
{
	bAutoStart = FALSE;
	CDialogEx::OnCancel();
}

BOOL CMacroPathList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	bAutoStart = FALSE;

	m_comboLotIdMode.AddString(_T("Manual"));                   //텍스트 추가
	m_comboLotIdMode.AddString(_T("Auto"));

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_MacroPathList(strDataFolder + "MacroPathList.ini");
	CString strSection, strKey;

	// 저장된 ini의 Lot ID Mode 파악
	strSection = "Lot_ID";
	strKey = "Mode";
	m_strMacroLotMode = INI_MacroPathList.Get_String(strSection, strKey, "");
	if (m_strMacroLotMode == "Auto")
		bLot_ID = 0;
	else
		bLot_ID = 1;
	m_comboLotIdMode.SetCurSel(bLot_ID);
	OnCbnSelchangeComboLotIdMode();

	// 저장된 ini의 Lot ID Length 파악
	strKey = "Length";
	m_strLotIdLength = INI_MacroPathList.Get_String(strSection, strKey, "");
	SetDlgItemText(IDC_EDIT_LOT_ID_LENGTH, m_strLotIdLength);

	// 저장된 ini의 Macro Path 파악
	if (bLot_ID == 0)	// Auto
	{
		strSection = "FilePath_A";
		for (int j = 0; j < PATH_LIST_NUM; j++)
		{
			strKey.Format("Path_%d", j);
			strArrayPath_A[j] = INI_MacroPathList.Get_String(strSection, strKey, "");
		}

		SetDlgItemText(IDC_EDIT_MACRO_PATH_1, strArrayPath_A[0]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_2, strArrayPath_A[1]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_3, strArrayPath_A[2]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_4, strArrayPath_A[3]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_5, strArrayPath_A[4]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_6, strArrayPath_A[5]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_7, strArrayPath_A[6]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_8, strArrayPath_A[7]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_9, strArrayPath_A[8]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_10, strArrayPath_A[9]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_11, strArrayPath_A[10]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_12, strArrayPath_A[11]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_13, strArrayPath_A[12]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_14, strArrayPath_A[13]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_15, strArrayPath_A[14]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_16, strArrayPath_A[15]);

		SetDlgItemText(IDC_EDIT_MACRO_PATH_17, strArrayPath_A[16]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_18, strArrayPath_A[17]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_19, strArrayPath_A[18]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_20, strArrayPath_A[19]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_21, strArrayPath_A[20]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_22, strArrayPath_A[21]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_23, strArrayPath_A[22]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_24, strArrayPath_A[23]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_25, strArrayPath_A[24]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_26, strArrayPath_A[25]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_27, strArrayPath_A[26]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_28, strArrayPath_A[27]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_29, strArrayPath_A[28]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_30, strArrayPath_A[29]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_31, strArrayPath_A[30]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_32, strArrayPath_A[31]);
	}
	else		// Manual
	{
		strSection = "FilePath_M";
		for (int j = 0; j < PATH_LIST_NUM; j++)
		{
			strKey.Format("Path_%d", j);
			strArrayPath_M[j] = INI_MacroPathList.Get_String(strSection, strKey, "");
		}

		SetDlgItemText(IDC_EDIT_MACRO_PATH_1, strArrayPath_M[0]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_2, strArrayPath_M[1]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_3, strArrayPath_M[2]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_4, strArrayPath_M[3]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_5, strArrayPath_M[4]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_6, strArrayPath_M[5]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_7, strArrayPath_M[6]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_8, strArrayPath_M[7]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_9, strArrayPath_M[8]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_10, strArrayPath_M[9]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_11, strArrayPath_M[10]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_12, strArrayPath_M[11]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_13, strArrayPath_M[12]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_14, strArrayPath_M[13]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_15, strArrayPath_M[14]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_16, strArrayPath_M[15]);

		SetDlgItemText(IDC_EDIT_MACRO_PATH_17, strArrayPath_M[16]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_18, strArrayPath_M[17]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_19, strArrayPath_M[18]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_20, strArrayPath_M[19]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_21, strArrayPath_M[20]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_22, strArrayPath_M[21]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_23, strArrayPath_M[22]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_24, strArrayPath_M[23]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_25, strArrayPath_M[24]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_26, strArrayPath_M[25]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_27, strArrayPath_M[26]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_28, strArrayPath_M[27]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_29, strArrayPath_M[28]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_30, strArrayPath_M[29]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_31, strArrayPath_M[30]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_32, strArrayPath_M[31]);
	}
	//UpdateData(FALSE);

	ChangeLanguage();

	return TRUE;
}

void CMacroPathList::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("매크로 모드 랏 리스트"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_1, _T("모드"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_2, _T("랏 아이디 길이"));
		SetDlgItemText(IDC_GROUPBOX_MACROPATHDLG_1, _T("경로 설정"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_3, _T("경로 1"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_4, _T("경로 2"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_5, _T("경로 3"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_6, _T("경로 4"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_7, _T("경로 5"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_8, _T("경로 6"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_9, _T("경로 7"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_10, _T("경로 8"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_11, _T("경로 9"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_12, _T("경로 10"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_13, _T("경로 11"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_14, _T("경로 12"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_15, _T("경로 13"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_16, _T("경로 14"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_17, _T("경로 15"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_18, _T("경로 16"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_19, _T("경로 17"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_20, _T("경로 18"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_21, _T("경로 19"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_22, _T("경로 20"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_23, _T("경로 21"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_24, _T("경로 22"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_25, _T("경로 23"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_26, _T("경로 24"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_27, _T("경로 25"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_28, _T("경로 26"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_29, _T("경로 27"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_30, _T("경로 28"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_31, _T("경로 29"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_32, _T("경로 30"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_33, _T("경로 31"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_34, _T("경로 32"));
		SetDlgItemText(IDC_BUTTON_MACRO_PATH_SAVE, _T("저장"));
		SetDlgItemText(IDC_BUTTON_MACRO_PATH_SAVE_CLOSE, _T("저장 후 닫기"));
		SetDlgItemText(IDC_BUTTON_MAKE_INI, _T("INI 생성"));
		SetDlgItemText(IDC_BUTTON_MAKE_INSPECT, _T("INI 생성 후 실행"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Macro mode lot list"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_1, _T("Mode"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_2, _T("Lot ID length"));
		SetDlgItemText(IDC_GROUPBOX_MACROPATHDLG_1, _T("Set path"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_3, _T("Path 1"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_4, _T("Path 2"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_5, _T("Path 3"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_6, _T("Path 4"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_7, _T("Path 5"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_8, _T("Path 6"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_9, _T("Path 7"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_10, _T("Path 8"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_11, _T("Path 9"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_12, _T("Path 10"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_13, _T("Path 11"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_14, _T("Path 12"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_15, _T("Path 13"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_16, _T("Path 14"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_17, _T("Path 15"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_18, _T("Path 16"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_19, _T("Path 17"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_20, _T("Path 18"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_21, _T("Path 19"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_22, _T("Path 20"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_23, _T("Path 21"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_24, _T("Path 22"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_25, _T("Path 23"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_26, _T("Path 24"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_27, _T("Path 25"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_28, _T("Path 26"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_29, _T("Path 27"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_30, _T("Path 28"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_31, _T("Path 29"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_32, _T("Path 30"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_33, _T("Path 31"));
		SetDlgItemText(IDC_STATIC_MACROPATHDLG_34, _T("Path 32"));
		SetDlgItemText(IDC_BUTTON_MACRO_PATH_SAVE, _T("Save"));
		SetDlgItemText(IDC_BUTTON_MACRO_PATH_SAVE_CLOSE, _T("Save and close"));
		SetDlgItemText(IDC_BUTTON_MAKE_INI, _T("Make INI"));
		SetDlgItemText(IDC_BUTTON_MAKE_INSPECT, _T("Make INI and start"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}


void CMacroPathList::OnBnClickedButtonMacroPathSave()
{
	UpdateData();

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_MacroPathList(strDataFolder + "MacroPathList.ini");
	CString strSection, strKey;

	bLot_ID = m_comboLotIdMode.GetCurSel();

	strSection = "Lot_ID";
	strKey = "Length";
	INI_MacroPathList.Set_String(strSection, strKey, m_strLotIdLength);

	if (bLot_ID == 0)
	{
		strSection = "Lot_ID";
		strKey = "Mode";
		INI_MacroPathList.Set_String(strSection, strKey, "Auto");

		strSection = "FilePath_A";
		for (int j = 0; j < PATH_LIST_NUM; j++)
		{
			strKey.Format("Path_%d", j);
			INI_MacroPathList.Set_String(strSection, strKey, m_strMacroPath[j]);
		}
	}
	else
	{
		strSection = "Lot_ID";
		strKey = "Mode";
		INI_MacroPathList.Set_String(strSection, strKey, "Manual");

		strSection = "FilePath_M";
		for (int j = 0; j < PATH_LIST_NUM; j++)
		{
			strKey.Format("Path_%d", j);
			INI_MacroPathList.Set_String(strSection, strKey, m_strMacroPath[j]);
		}
	}
}

void CMacroPathList::OnBnClickedButtonMacroPathSaveClose()
{
	bAutoStart = FALSE;
	OnBnClickedButtonMacroPathSave();
	CDialogEx::OnOK();
}

void CMacroPathList::OnCbnSelchangeComboLotIdMode()
{
	UpdateData(TRUE);

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_MacroPathList(strDataFolder + "MacroPathList.ini");
	CString strSection, strKey;

	strSection = "Lot_ID";
	strKey = "Length";
	m_strLotIdLength = INI_MacroPathList.Get_String(strSection, strKey, "");
	SetDlgItemText(IDC_EDIT_LOT_ID_LENGTH, m_strLotIdLength);

	bLot_ID = m_comboLotIdMode.GetCurSel();

	if (bLot_ID == 0)
	{
		strSection = "FilePath_A";
		for (int j = 0; j < PATH_LIST_NUM; j++)
		{
			strKey.Format("Path_%d", j);
			strArrayPath_A[j] = INI_MacroPathList.Get_String(strSection, strKey, "");
		}

		SetDlgItemText(IDC_EDIT_MACRO_PATH_1, strArrayPath_A[0]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_2, strArrayPath_A[1]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_3, strArrayPath_A[2]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_4, strArrayPath_A[3]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_5, strArrayPath_A[4]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_6, strArrayPath_A[5]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_7, strArrayPath_A[6]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_8, strArrayPath_A[7]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_9, strArrayPath_A[8]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_10, strArrayPath_A[9]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_11, strArrayPath_A[10]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_12, strArrayPath_A[11]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_13, strArrayPath_A[12]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_14, strArrayPath_A[13]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_15, strArrayPath_A[14]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_16, strArrayPath_A[15]);

		SetDlgItemText(IDC_EDIT_MACRO_PATH_17, strArrayPath_A[16]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_18, strArrayPath_A[17]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_19, strArrayPath_A[18]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_20, strArrayPath_A[19]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_21, strArrayPath_A[20]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_22, strArrayPath_A[21]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_23, strArrayPath_A[22]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_24, strArrayPath_A[23]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_25, strArrayPath_A[24]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_26, strArrayPath_A[25]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_27, strArrayPath_A[26]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_28, strArrayPath_A[27]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_29, strArrayPath_A[28]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_30, strArrayPath_A[29]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_31, strArrayPath_A[30]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_32, strArrayPath_A[31]);
	}
	else
	{
		strSection = "FilePath_M";
		for (int j = 0; j < PATH_LIST_NUM; j++)
		{
			strKey.Format("Path_%d", j);
			strArrayPath_M[j] = INI_MacroPathList.Get_String(strSection, strKey, "");
		}

		SetDlgItemText(IDC_EDIT_MACRO_PATH_1, strArrayPath_M[0]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_2, strArrayPath_M[1]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_3, strArrayPath_M[2]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_4, strArrayPath_M[3]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_5, strArrayPath_M[4]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_6, strArrayPath_M[5]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_7, strArrayPath_M[6]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_8, strArrayPath_M[7]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_9, strArrayPath_M[8]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_10, strArrayPath_M[9]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_11, strArrayPath_M[10]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_12, strArrayPath_M[11]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_13, strArrayPath_M[12]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_14, strArrayPath_M[13]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_15, strArrayPath_M[14]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_16, strArrayPath_M[15]);

		SetDlgItemText(IDC_EDIT_MACRO_PATH_17, strArrayPath_M[16]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_18, strArrayPath_M[17]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_19, strArrayPath_M[18]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_20, strArrayPath_M[19]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_21, strArrayPath_M[20]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_22, strArrayPath_M[21]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_23, strArrayPath_M[22]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_24, strArrayPath_M[23]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_25, strArrayPath_M[24]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_26, strArrayPath_M[25]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_27, strArrayPath_M[26]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_28, strArrayPath_M[27]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_29, strArrayPath_M[28]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_30, strArrayPath_M[29]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_31, strArrayPath_M[30]);
		SetDlgItemText(IDC_EDIT_MACRO_PATH_32, strArrayPath_M[31]);
	}
	OnBnClickedButtonMacroPathSave();
	//UpdateData(FALSE);
}

void CMacroPathList::GetAdjFileList(CString strFolder)
{
	CFileFind file;
	BOOL b = file.FindFile(strFolder + _T("\\*.*"));
	CString strFolderItem, strFileExt, strFileExt2, strFileExt3, strFileExt4, strLotFolderName, strTrayFolderNumber, strTempString;
	int nL;

	while (b)
	{
		b = file.FindNextFile();

		if (file.IsDirectory() && !file.IsDots())
			strFolderItem = file.GetFilePath();

		if (_access(strFolderItem + "\\RawImage", 0) == 0)
		{
			strLotFolderName = strFolder.Mid(strFolder.ReverseFind('\\') + 1);
			strTrayFolderNumber = strFolderItem.Mid(strFolderItem.ReverseFind('-') + 1);

			m_strLotFolderName[g_npathnum] = strLotFolderName;
			m_strLotPath[g_npathnum] = strFolderItem + "\\RawImage";
			m_strTrayNumber[g_npathnum] = strTrayFolderNumber;

			g_npathnum++;
		}
	}
}

void CMacroPathList::TraySorting()
{
	int st1, st2;
	CString temp1, temp2;

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_RunMacro(strDataFolder + "RunMacro.ini");
	CString strSection, strKey;

	for (int check = cut; check < g_npathnum; check++)
	{
		if (m_strLotFolderName[check] != m_strLotFolderName[check + 1])
		{
			for (st1 = 0; st1 < check - cut; st1++)
			{
				for (st2 = 0; st2 < check - cut - st1; st2++)
				{
					if (_ttoi(m_strTrayNumber[st2 + cut]) > _ttoi(m_strTrayNumber[st2 + cut + 1]))
					{
						temp1 = m_strTrayNumber[st2 + cut];
						m_strTrayNumber[st2 + cut] = m_strTrayNumber[st2 + cut + 1];
						m_strTrayNumber[st2 + cut + 1] = temp1;

						temp2 = m_strLotPath[st2 + cut];
						m_strLotPath[st2 + cut] = m_strLotPath[st2 + cut + 1];
						m_strLotPath[st2 + cut + 1] = temp2;
					}
				}
			}
			cut = check + 1;
		}
	}

	for (int wi = 0; wi < g_npathnum; wi++)
	{
		if (m_strMacroLotMode == "TRUE")
		{
			strSection = "FilePath";
			strKey.Format("Path_%d", wi);
			INI_RunMacro.Set_String(strSection, strKey, m_strLotPath[wi]);

			strSection = "LotID";
			strKey.Format("ID_%d", wi);
			INI_RunMacro.Set_String(strSection, strKey, m_strLotFolderName[wi]);
		}
		else
		{
			strSection = "FilePath";
			strKey.Format("Path_%d", wi);
			INI_RunMacro.Set_String(strSection, strKey, m_strLotPath[wi]);

			strSection = "LotID";
			strKey.Format("ID_%d", wi);
			INI_RunMacro.Set_String(strSection, strKey, m_strLotFolderName[wi]);
		}
	}
}

void CMacroPathList::OnBnClickedButtonMakeINI()
{
	OnBnClickedButtonMacroPathSave();

	g_npathnum = 0;
	cut = 0;

	if (THEAPP.g_nMacroCount == 0)
	{
		CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
		CIniFileCS INI_MacroPathList(strDataFolder + "MacroPathList.ini");
		CString strSection, strKey;

		strSection = "Lot_ID";
		strKey = "Mode";
		m_strMacroLotMode = INI_MacroPathList.Get_String(strSection, strKey, "");
		if (m_strMacroLotMode == "Auto")
			bLot_ID = 0;
		else
			bLot_ID = 1;

		if (bLot_ID == 0)
			strSection = "FilePath_A";
		else
			strSection = "FilePath_M";

		for (int j = 0; j < 32; j++)
		{
			strKey.Format("Path_%d", j);
			strArrayPath[j] = INI_MacroPathList.Get_String(strSection, strKey, "");

			if (!strArrayPath[j].Compare(""))
				break;

			GetAdjFileList(strArrayPath[j]);
			TraySorting();
		}

		if (bAutoStart == FALSE)
			AfxMessageBox("Searching end");
	}
}

void CMacroPathList::OnBnClickedButtonMakeAndInspect()
{
	bAutoStart = TRUE;
	OnBnClickedButtonMakeINI();
	OnBnClickedCancel();
	THEAPP.m_pInspectSummary->OnBnClickedMfcbuttonInspectStart();
}
