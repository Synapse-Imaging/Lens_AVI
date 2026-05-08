// NewModelDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "NewModelDlg.h"
#include "IniFileCS.h"
#include "afxdialogex.h"


// CNewModelDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNewModelDlg, CDialog)

CNewModelDlg::CNewModelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewModelDlg::IDD, pParent)
{

	m_Edit_strModelName = _T("");
	m_Edit_iTrayArrayX = 6;
	m_Edit_iTrayArrayY = 7;
}

CNewModelDlg::~CNewModelDlg()
{
}

void CNewModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MODEL_NAME, m_Edit_strModelName);
	DDX_Text(pDX, IDC_EDIT_TRAY_ARRAY_X, m_Edit_iTrayArrayX);
	DDX_Text(pDX, IDC_EDIT_TRAY_ARRAY_Y, m_Edit_iTrayArrayY);

}


BEGIN_MESSAGE_MAP(CNewModelDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CNewModelDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CNewModelDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CNewModelDlg 메시지 처리기입니다.
BOOL CNewModelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_Edit_strModelName = _T("");
	m_Edit_iTrayArrayX = 6;
	m_Edit_iTrayArrayY = 7;

	ChangeLanguage();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CNewModelDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CNewModelDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("새로운 모델 생성"));
		SetDlgItemText(IDC_GROUPBOX_NEWMODELDLG_1, _T("모델 이름 설정"));
		SetDlgItemText(IDC_STATIC_NEWMODELDLG_1, _T("모델 이름:"));
		SetDlgItemText(IDC_GROUPBOX_NEWMODELDLG_2, _T("트레이 배열 설정"));
		SetDlgItemText(IDC_STATIC_NEWMODELDLG_2, _T("트레이 X축 배열:"));
		SetDlgItemText(IDC_STATIC_NEWMODELDLG_3, _T("트레이 Y축 배열:"));
		SetDlgItemText(IDOK, _T("생성"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Create new model"));
		SetDlgItemText(IDC_GROUPBOX_NEWMODELDLG_1, _T("Set model name"));
		SetDlgItemText(IDC_STATIC_NEWMODELDLG_1, _T("Model name:"));
		SetDlgItemText(IDC_GROUPBOX_NEWMODELDLG_2, _T("Set tray array"));
		SetDlgItemText(IDC_STATIC_NEWMODELDLG_2, _T("Tray array X-axis:"));
		SetDlgItemText(IDC_STATIC_NEWMODELDLG_3, _T("Tray array Y-axis:"));
		SetDlgItemText(IDOK, _T("Create"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}


void CNewModelDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (CheckInfomation())
	{
		for (int i = 0; i < VISION_NUMBER_MAX; i++)
		{
			CString sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[i];
			if (sVisionCamType != "Undefine")
			{
				CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_Edit_strModelName;
				CString strSwFolder = strModelFolder + "\\SW";

				CString strSWVisionFolder, strTeachImageFolder, strMatchingModelFolder;
				strSWVisionFolder.Format("%s\\Vision_N%d", strSwFolder, i + 1);
				strTeachImageFolder.Format("%s\\TeachImage", strSWVisionFolder, i + 1);
				strMatchingModelFolder.Format("%s\\MatchingModel", strSWVisionFolder, i + 1);

				THEAPP.m_FileBase.CreateFolder(strModelFolder);			// 모델 폴더 만들기
				THEAPP.m_FileBase.CreateFolder(strSwFolder);			// 모델 폴더 만들기

				THEAPP.m_FileBase.CreateFolder(strSWVisionFolder);		// 모델 폴더 만들기
				THEAPP.m_FileBase.CreateFolder(strTeachImageFolder);	// 티칭 이미지 저장폴더 만들기
				THEAPP.m_FileBase.CreateFolder(strMatchingModelFolder);	// 매칭 모델 저장폴더 만들기

				/////////////////////////////////////////////////////////////Modelbase INI_Vision_N1 file
				CString strSWModelBase;
				strSWModelBase.Format("%s\\ModelBase.ini", strSWVisionFolder, i + 1);
				CIniFileCS INI_SWModelBase(strSWModelBase);

				CString strSection;
				strSection = "Model Base";
				INI_SWModelBase.Set_String(strSection, "MODEL NAME", m_Edit_strModelName);
				INI_SWModelBase.Set_Integer(strSection, "VERSION", THEAPP.GetProgramVersion());
				INI_SWModelBase.Set_Integer(strSection, "ParameterVersion", 1000);
				INI_SWModelBase.Set_Integer(strSection, "TrayArrayX", m_Edit_iTrayArrayX);
				INI_SWModelBase.Set_Integer(strSection, "TrayArrayY", m_Edit_iTrayArrayY);
				INI_SWModelBase.Set_Integer(strSection, "TrayLotModuleAmt", m_Edit_iTrayArrayX*m_Edit_iTrayArrayY);

				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][i]->m_sModelName = m_Edit_strModelName;
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][i]->m_iTrayArrayX = m_Edit_iTrayArrayX;
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][i]->m_iTrayArrayY = m_Edit_iTrayArrayY;
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][i]->m_iTrayModuleMax = m_Edit_iTrayArrayX * m_Edit_iTrayArrayY;

				THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_MODEL_NAME, m_Edit_strModelName);

				CString strOpticModelFolder;
				strOpticModelFolder.Format("%s\\Optical\\%s", THEAPP.GetWorkingDirectory(), m_Edit_strModelName);

				CString strHwFolder = strOpticModelFolder + "\\HW\\";

				CString strHwVisionFolder;
				strHwVisionFolder.Format("%s\\Vision_N%d", strHwFolder, i + 1);
				THEAPP.m_FileBase.CreateFolder(strHwFolder);
				THEAPP.m_FileBase.CreateFolder(strHwVisionFolder);
			}
		}
	}
	else
	{
		return;
	}

	CDialog::OnOK();
}


void CNewModelDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}


BOOL CNewModelDlg::CheckInfomation()
{
	if (m_Edit_strModelName == "")
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("모델 이름을 입력해주세요");
		else
			strMessageBox.Format("Input the model name");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

		return FALSE;
	}

	if (m_Edit_iTrayArrayX == 0)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("트레이 X축 배열을 입력해주세요");
		else
			strMessageBox.Format("Input the tray array X-axis");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

		return FALSE;
	}

	if (m_Edit_iTrayArrayY == 0)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("트레이 Y축 배열을 입력해주세요");
		else
			strMessageBox.Format("Input the tray array Y-axis");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

		return FALSE;
	}

	return TRUE;
}