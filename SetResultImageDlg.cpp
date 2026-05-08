// SetResultImageDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "SetResultImageDlg.h"
#include "afxdialogex.h"


// CSetResultImageDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetResultImageDlg, CDialog)

CSetResultImageDlg::CSetResultImageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetResultImageDlg::IDD, pParent)
{

}

CSetResultImageDlg::~CSetResultImageDlg()
{
}

void CSetResultImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_VISION1_1, m_cVisionImage[0][0]);
	DDX_Control(pDX, IDC_COMBO_VISION1_2, m_cVisionImage[0][1]);
	DDX_Control(pDX, IDC_COMBO_VISION1_3, m_cVisionImage[0][2]);
	DDX_Control(pDX, IDC_COMBO_VISION1_4, m_cVisionImage[0][3]);
	DDX_Control(pDX, IDC_COMBO_VISION1_5, m_cVisionImage[0][4]);
	DDX_Control(pDX, IDC_COMBO_VISION1_6, m_cVisionImage[0][5]);
	DDX_Control(pDX, IDC_COMBO_VISION1_7, m_cVisionImage[0][6]);
	DDX_Control(pDX, IDC_COMBO_VISION1_8, m_cVisionImage[0][7]);
	DDX_Control(pDX, IDC_COMBO_VISION1_9, m_cVisionImage[0][8]);
	DDX_Control(pDX, IDC_COMBO_VISION1_10, m_cVisionImage[0][9]);
	DDX_Control(pDX, IDC_COMBO_VISION1_11, m_cVisionImage[0][10]);
	DDX_Control(pDX, IDC_COMBO_VISION1_12, m_cVisionImage[0][11]);
	DDX_Control(pDX, IDC_COMBO_VISION1_13, m_cVisionImage[0][12]);
	DDX_Control(pDX, IDC_COMBO_VISION1_14, m_cVisionImage[0][13]);
	DDX_Control(pDX, IDC_COMBO_VISION1_15, m_cVisionImage[0][14]);
	DDX_Control(pDX, IDC_COMBO_VISION1_16, m_cVisionImage[0][15]);
	DDX_Control(pDX, IDC_COMBO_VISION1_17, m_cVisionImage[0][16]);
	DDX_Control(pDX, IDC_COMBO_VISION1_18, m_cVisionImage[0][17]);

	DDX_Control(pDX, IDC_COMBO_VISION2_1, m_cVisionImage[1][0]);
	DDX_Control(pDX, IDC_COMBO_VISION2_2, m_cVisionImage[1][1]);
	DDX_Control(pDX, IDC_COMBO_VISION2_3, m_cVisionImage[1][2]);
	DDX_Control(pDX, IDC_COMBO_VISION2_4, m_cVisionImage[1][3]);
	DDX_Control(pDX, IDC_COMBO_VISION2_5, m_cVisionImage[1][4]);
	DDX_Control(pDX, IDC_COMBO_VISION2_6, m_cVisionImage[1][5]);
	DDX_Control(pDX, IDC_COMBO_VISION2_7, m_cVisionImage[1][6]);
	DDX_Control(pDX, IDC_COMBO_VISION2_8, m_cVisionImage[1][7]);
	DDX_Control(pDX, IDC_COMBO_VISION2_9, m_cVisionImage[1][8]);
	DDX_Control(pDX, IDC_COMBO_VISION2_10, m_cVisionImage[1][9]);
	DDX_Control(pDX, IDC_COMBO_VISION2_11, m_cVisionImage[1][10]);
	DDX_Control(pDX, IDC_COMBO_VISION2_12, m_cVisionImage[1][11]);
	DDX_Control(pDX, IDC_COMBO_VISION2_13, m_cVisionImage[1][12]);
	DDX_Control(pDX, IDC_COMBO_VISION2_14, m_cVisionImage[1][13]);
	DDX_Control(pDX, IDC_COMBO_VISION2_15, m_cVisionImage[1][14]);
	DDX_Control(pDX, IDC_COMBO_VISION2_16, m_cVisionImage[1][15]);
	DDX_Control(pDX, IDC_COMBO_VISION2_17, m_cVisionImage[1][16]);
	DDX_Control(pDX, IDC_COMBO_VISION2_18, m_cVisionImage[1][17]);

	DDX_Control(pDX, IDC_COMBO_VISION3_1, m_cVisionImage[2][0]);
	DDX_Control(pDX, IDC_COMBO_VISION3_2, m_cVisionImage[2][1]);
	DDX_Control(pDX, IDC_COMBO_VISION3_3, m_cVisionImage[2][2]);
	DDX_Control(pDX, IDC_COMBO_VISION3_4, m_cVisionImage[2][3]);
	DDX_Control(pDX, IDC_COMBO_VISION3_5, m_cVisionImage[2][4]);
	DDX_Control(pDX, IDC_COMBO_VISION3_6, m_cVisionImage[2][5]);
	DDX_Control(pDX, IDC_COMBO_VISION3_7, m_cVisionImage[2][6]);
	DDX_Control(pDX, IDC_COMBO_VISION3_8, m_cVisionImage[2][7]);
	DDX_Control(pDX, IDC_COMBO_VISION3_9, m_cVisionImage[2][8]);
	DDX_Control(pDX, IDC_COMBO_VISION3_10, m_cVisionImage[2][9]);
	DDX_Control(pDX, IDC_COMBO_VISION3_11, m_cVisionImage[2][10]);
	DDX_Control(pDX, IDC_COMBO_VISION3_12, m_cVisionImage[2][11]);
	DDX_Control(pDX, IDC_COMBO_VISION3_13, m_cVisionImage[2][12]);
	DDX_Control(pDX, IDC_COMBO_VISION3_14, m_cVisionImage[2][13]);
	DDX_Control(pDX, IDC_COMBO_VISION3_15, m_cVisionImage[2][14]);
	DDX_Control(pDX, IDC_COMBO_VISION3_16, m_cVisionImage[2][15]);
	DDX_Control(pDX, IDC_COMBO_VISION3_17, m_cVisionImage[2][16]);
	DDX_Control(pDX, IDC_COMBO_VISION3_18, m_cVisionImage[2][17]);

	DDX_Control(pDX, IDC_COMBO_VISION4_1, m_cVisionImage[3][0]);
	DDX_Control(pDX, IDC_COMBO_VISION4_2, m_cVisionImage[3][1]);
	DDX_Control(pDX, IDC_COMBO_VISION4_3, m_cVisionImage[3][2]);
	DDX_Control(pDX, IDC_COMBO_VISION4_4, m_cVisionImage[3][3]);
	DDX_Control(pDX, IDC_COMBO_VISION4_5, m_cVisionImage[3][4]);
	DDX_Control(pDX, IDC_COMBO_VISION4_6, m_cVisionImage[3][5]);
	DDX_Control(pDX, IDC_COMBO_VISION4_7, m_cVisionImage[3][6]);
	DDX_Control(pDX, IDC_COMBO_VISION4_8, m_cVisionImage[3][7]);
	DDX_Control(pDX, IDC_COMBO_VISION4_9, m_cVisionImage[3][8]);
	DDX_Control(pDX, IDC_COMBO_VISION4_10, m_cVisionImage[3][9]);
	DDX_Control(pDX, IDC_COMBO_VISION4_11, m_cVisionImage[3][10]);
	DDX_Control(pDX, IDC_COMBO_VISION4_12, m_cVisionImage[3][11]);
	DDX_Control(pDX, IDC_COMBO_VISION4_13, m_cVisionImage[3][12]);
	DDX_Control(pDX, IDC_COMBO_VISION4_14, m_cVisionImage[3][13]);
	DDX_Control(pDX, IDC_COMBO_VISION4_15, m_cVisionImage[3][14]);
	DDX_Control(pDX, IDC_COMBO_VISION4_16, m_cVisionImage[3][15]);
	DDX_Control(pDX, IDC_COMBO_VISION4_17, m_cVisionImage[3][16]);
	DDX_Control(pDX, IDC_COMBO_VISION4_18, m_cVisionImage[3][17]);
}


BEGIN_MESSAGE_MAP(CSetResultImageDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSetResultImageDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSetResultImageDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSetResultImageDlg 메시지 처리기입니다.


BOOL CSetResultImageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString sVisionCamType;

	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_1];
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG2, sVisionCamType);
	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_2];
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG20, sVisionCamType);
	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_3];
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG39, sVisionCamType);
	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_4];
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG59, sVisionCamType);

	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG2, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][0]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG3, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][1]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG4, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][2]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG5, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][3]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG6, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][4]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG7, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][5]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG8, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][6]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG9, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][7]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG10, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][8]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG11, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][9]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG12, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][10]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG13, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][11]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG14, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][12]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG15, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][13]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG16, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][14]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG17, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][15]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG18, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][16]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG19, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[0][17]));

	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG21, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][0]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG22, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][1]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG23, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][2]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG24, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][3]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG25, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][4]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG26, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][5]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG27, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][6]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG28, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][7]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG29, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][8]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG30, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][9]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG31, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][10]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG32, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][11]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG33, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][12]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG34, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][13]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG35, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][14]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG36, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][15]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG37, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][16]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG38, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[1][17]));

	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG40, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][0]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG41, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][1]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG42, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][2]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG43, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][3]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG44, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][4]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG45, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][5]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG46, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][6]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG47, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][7]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG48, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][8]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG49, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][9]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG50, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][10]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG51, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][11]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG52, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][12]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG53, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][13]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG54, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][14]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG55, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][15]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG56, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][16]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG57, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[2][17]));

	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG59, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][0]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG60, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][1]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG61, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][2]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG62, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][3]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG63, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][4]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG64, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][5]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG65, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][6]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG66, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][7]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG67, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][8]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG68, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][9]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG69, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][10]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG70, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][11]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG71, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][12]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG72, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][13]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG73, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][14]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG74, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][15]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG75, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][16]));
	SetDlgItemText(IDC_STATIC_SETRESULTIMAGEDLG76, _T(THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[3][17]));

	ChangeLanguage();

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		for (int j = 0; j < MAX_RESULT_IMAGE; j++)
			m_cVisionImage[i][j].SetCurSel(THEAPP.Struct_PreferenceStruct.iOverlayImageIndex[i][j]);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSetResultImageDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("결과 영상 번호 설정"));
		SetDlgItemText(IDOK, _T("저장"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Set result image number"));
		SetDlgItemText(IDOK, _T("Save"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		for (int j = 0; j < MAX_RESULT_IMAGE; j++)
		{
			m_cVisionImage[i][j].ResetContent();

			for (int k = 0; k < MAX_IMAGE_TAB; k++)
			{
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strDialog.Format("영상 %d", k + 1);
				else
					strDialog.Format("Image %d", k + 1);

				m_cVisionImage[i][j].AddString(strDialog);
			}
		}
	}
}


void CSetResultImageDlg::OnBnClickedOk()
{
	UpdateData();

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
	CString strIniFile;
	strIniFile.Format("OverlayViewport_%s.ini", strEquipModel);
	CIniFileCS INI_OverlayViewport(strDataFolder + strIniFile);
	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		CString sVisionCamType;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[i];

		CString strSection, strKey;
		strSection.Format("%s", sVisionCamType);
		for (int j = 0; j < MAX_RESULT_IMAGE; j++)
		{
			THEAPP.Struct_PreferenceStruct.iOverlayImageIndex[i][j] = m_cVisionImage[i][j].GetCurSel();

			strKey.Format("%d_Review capture image", j + 1);
			INI_OverlayViewport.Set_Integer(strSection, strKey, THEAPP.Struct_PreferenceStruct.iOverlayImageIndex[i][j]);
		}
	}

	CDialog::OnOK();
}


void CSetResultImageDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
