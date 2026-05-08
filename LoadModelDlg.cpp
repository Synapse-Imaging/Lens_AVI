// LoadModelDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LoadModelDlg.h"
#include "afxdialogex.h"
#include "IniFileCS.h"

// CLoadModelDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLoadModelDlg, CDialog)

CLoadModelDlg::CLoadModelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadModelDlg::IDD, pParent)
{
	m_EditSelectedModel = _T("");
	m_EditModelDescription = _T("");
}

CLoadModelDlg::~CLoadModelDlg()
{
}

void CLoadModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODELNAMES, m_ModelList);
	DDX_Text(pDX, IDC_EDIT_MODELNAME, m_EditSelectedModel);
	DDX_Text(pDX, IDC_EDIT_MODEL_DESCRIPTION, m_EditModelDescription);
}


BEGIN_MESSAGE_MAP(CLoadModelDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_MODELNAMES, &CLoadModelDlg::OnSelchangeListModelnames)
	ON_BN_CLICKED(IDOK, &CLoadModelDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLoadModelDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLoadModelDlg 메시지 처리기입니다.

BOOL CLoadModelDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CLoadModelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_EditSelectedModel = _T("");
	m_EditModelDescription = _T("");
	LoadModelList();

	ChangeLanguage();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLoadModelDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("모델 불러오기"));
		SetDlgItemText(IDC_STATIC_LOADDLG_1, _T("모델명:"));
		SetDlgItemText(IDOK, _T("불러오기"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Load model"));
		SetDlgItemText(IDC_STATIC_LOADDLG_1, _T("Model name:"));
		SetDlgItemText(IDOK, _T("Load"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}

void CLoadModelDlg::LoadModelList()
{
	m_ModelList.ResetContent();

	CFileFind ff;
	CString strModelFile = THEAPP.GetWorkingDirectory() + "\\Model\\*.*";

	if (!ff.FindFile(strModelFile, 0)) return;

	BOOL bExist = ff.FindNextFile();

	int nCnt = 0, nNameLen;
	CString strExt, strName;

	while (TRUE) {
		if (ff.IsDirectory()) {
			strExt = ff.GetFileName();
			if (strExt.GetLength() > 2)
			{
				m_ModelList.AddString(strExt);
			}

		}
		if (!bExist) break;
		bExist = ff.FindNextFile();
	}
}

void CLoadModelDlg::OnSelchangeListModelnames()
{
	int index = m_ModelList.GetCurSel();
	m_ModelList.GetText(index, m_EditSelectedModel);

	m_EditModelDescription = _T("");
	UpdateData(FALSE);

	CString strModelName;
	int iCurrent_ModelVersion, iCurrent_TrayArrayX, iCurrent_TrayArrayY, iCurrent_TrayModuleAmt;

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_EditSelectedModel;

	CString strSWModelBase;
	strSWModelBase.Format("%s\\SW\\Vision_N%d\\ModelBase.ini", strModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	CIniFileCS INI(strSWModelBase);
	CString strSection = "Model Base";

	strModelName = INI.Get_String(strSection, "MODEL NAME", "DEFAULT");

	iCurrent_ModelVersion = INI.Get_Integer(strSection, "VERSION", 1000);
	iCurrent_TrayArrayX = INI.Get_Integer(strSection, "TrayArrayX", 3);
	iCurrent_TrayArrayY = INI.Get_Integer(strSection, "TrayArrayY", 20);
	iCurrent_TrayModuleAmt = INI.Get_Integer(strSection, "TrayLotModuleAmt", iCurrent_TrayArrayX*iCurrent_TrayArrayY);

	m_EditModelDescription += "[Model Name] : " + strModelName;
	CString Temp;

	Temp.Format("\r\n[Model Version] : %d", iCurrent_ModelVersion);
	m_EditModelDescription += Temp;
	Temp.Format("\r\n[TrayArrayX] : %d", iCurrent_TrayArrayX);
	m_EditModelDescription += Temp;
	Temp.Format("\r\n[TrayArrayY] : %d", iCurrent_TrayArrayY);
	m_EditModelDescription += Temp;

	UpdateData(FALSE);
}

void CLoadModelDlg::OnBnClickedOk()
{
	CDialog::OnOK();
}


void CLoadModelDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}
