// SelectEquipModelTypeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "SelectEquipModelTypeDlg.h"
#include "afxdialogex.h"


// CSelectEquipModelTypeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSelectEquipModelTypeDlg, CDialog)

CSelectEquipModelTypeDlg::CSelectEquipModelTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectEquipModelTypeDlg::IDD, pParent)
{
	m_iRadioEquipModelType = 0;
}

CSelectEquipModelTypeDlg::~CSelectEquipModelTypeDlg()
{
}

void CSelectEquipModelTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO_EQUIP_MODEL_TYPE_1, m_iRadioEquipModelType);
}


BEGIN_MESSAGE_MAP(CSelectEquipModelTypeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSelectEquipModelTypeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSelectEquipModelTypeDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSelectEquipModelTypeDlg 메시지 처리기입니다.


BOOL CSelectEquipModelTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iRadioEquipModelType = THEAPP.GetModelType();

	for (int i = 0; i < MODEL_TYPE_MAX; i++)
	{
		// TODO : 모델 타입이 추가될 때마다 INI 파일에 모델명 추가 필요 (ModelNameCode.ini) - jhkim, 260210
		SetDlgItemText(IDC_RADIO_EQUIP_MODEL_TYPE_1 + i, THEAPP.g_strModelTypeName[i]);

		if (THEAPP.g_strModelTypeName[i] == "-")
			GetDlgItem(IDC_RADIO_EQUIP_MODEL_TYPE_1 + i)->EnableWindow(FALSE);
	}

	ChangeLanguage();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSelectEquipModelTypeDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("설비 검사 모델 선택"));
		SetDlgItemText(IDOK, _T("선택"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Select equipment model"));
		SetDlgItemText(IDOK, _T("Select"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}


void CSelectEquipModelTypeDlg::OnBnClickedOk()
{
	UpdateData();

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_EquipModelType(strDataFolder + "EquipModelType.ini");
	INI_EquipModelType.Set_Integer("CURRENT MODEL TYPE", "Model Type Number", m_iRadioEquipModelType);

	THEAPP.ReadEquipModelTypeINI();
	THEAPP.ReadInspectionTypeInfoINI();
	THEAPP.ReadOverlayViewportINI();
	THEAPP.ReadDefectReViewInfoINI();
	THEAPP.ReadFaiDefectReViewInfo();

	CDialog::OnOK();
}


void CSelectEquipModelTypeDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}
