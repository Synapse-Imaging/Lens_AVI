// SPVImageParamDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "SPVImageParamDlg.h"
#include "afxdialogex.h"


// CSPVImageParamDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSPVImageParamDlg, CDialog)

CSPVImageParamDlg::CSPVImageParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSPVImageParamDlg::IDD, pParent)
{
	m_dEditDFMPsx = 0.0;
	m_dEditDFMPsy = 0.0;
	m_dEditDFMTex = 0.0;
	m_dEditDFMTey = 0.0;

	int i;

	for (i = 0; i < DFM_MAX_SPECULAR_IMAGE_NUMBER; i++)
		m_dEditDFMSpecularImageSF[i] = 1.0;

	m_iEditDFMMeanFilterSize = 0;

	for (i = 0; i < DFM_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		m_dEditDFMHigh[i] = 5;
		m_dEditDFMLow[i] = -5;
		m_dEditDFMDerivativeSmFactor[i] = 0.0;
	}

	for (i = 0; i < DIFFUSED_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		m_dEditDiffusedHigh[i] = 2.5;
		m_dEditDiffusedLow[i] = -1.07;
		m_dEditDiffusedSmFactor[i] = 14.5;
	}
}

CSPVImageParamDlg::~CSPVImageParamDlg()
{
}

void CSPVImageParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_DFM_PSX, m_dEditDFMPsx);
	DDX_Text(pDX, IDC_EDIT_DFM_PSY, m_dEditDFMPsy);

	DDX_Text(pDX, IDC_EDIT_DFM_TEX, m_dEditDFMTex);
	DDX_Text(pDX, IDC_EDIT_DFM_TEY, m_dEditDFMTey);
	DDX_Text(pDX, IDC_EDIT_DFM_SPECULAR_IMAGE_SF0, m_dEditDFMSpecularImageSF[0]);

	DDX_Text(pDX, IDC_EDIT_DFM_MEAN_FILTER_SIZE, m_iEditDFMMeanFilterSize);

	DDX_Text(pDX, IDC_EDIT_DFM_HIGH_0, m_dEditDFMHigh[0]);
	DDX_Text(pDX, IDC_EDIT_DFM_LOW_0, m_dEditDFMLow[0]);
	DDX_Text(pDX, IDC_EDIT_DFM_DERIVATIVE_SM_FACTOR_0, m_dEditDFMDerivativeSmFactor[0]);

	DDX_Text(pDX, IDC_EDIT_DFM_HIGH_1, m_dEditDFMHigh[1]);
	DDX_Text(pDX, IDC_EDIT_DFM_LOW_1, m_dEditDFMLow[1]);
	DDX_Text(pDX, IDC_EDIT_DFM_DERIVATIVE_SM_FACTOR_1, m_dEditDFMDerivativeSmFactor[1]);

	DDX_Text(pDX, IDC_EDIT_DIFFUSED_HIGH_0, m_dEditDiffusedHigh[0]);
	DDX_Text(pDX, IDC_EDIT_DIFFUSED_LOW_0, m_dEditDiffusedLow[0]);
	DDX_Text(pDX, IDC_EDIT_DIFFUSED_SM_FACTOR_0, m_dEditDiffusedSmFactor[0]);

	DDX_Text(pDX, IDC_EDIT_DIFFUSED_HIGH_1, m_dEditDiffusedHigh[1]);
	DDX_Text(pDX, IDC_EDIT_DIFFUSED_LOW_1, m_dEditDiffusedLow[1]);
	DDX_Text(pDX, IDC_EDIT_DIFFUSED_SM_FACTOR_1, m_dEditDiffusedSmFactor[1]);

}

BEGIN_MESSAGE_MAP(CSPVImageParamDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSPVImageParamDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSPVImageParamDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_OFFLINE_SPECULAR, &CSPVImageParamDlg::OnBnClickedButtonOfflineSpecular)
	ON_BN_CLICKED(IDC_BUTTON_OFFLINE_DIFFUSED, &CSPVImageParamDlg::OnBnClickedButtonOfflineDiffused)
END_MESSAGE_MAP()

// CSPVImageParamDlg 메시지 처리기입니다.

BOOL CSPVImageParamDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CSPVImageParamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_dEditDFMPsx = THEAPP.m_pModelDataManager->m_dDFMPsx;
	m_dEditDFMPsy = THEAPP.m_pModelDataManager->m_dDFMPsy;
	m_dEditDFMTex = THEAPP.m_pModelDataManager->m_dDFMTex;
	m_dEditDFMTey = THEAPP.m_pModelDataManager->m_dDFMTey;

	int i;

	for (i = 0; i < DFM_MAX_SPECULAR_IMAGE_NUMBER; i++)
		m_dEditDFMSpecularImageSF[i] = THEAPP.m_pModelDataManager->m_dDFMSpecularImageSF[i];

	m_iEditDFMMeanFilterSize = THEAPP.m_pModelDataManager->m_iDFMMeanFilterSize;

	for (i = 0; i < DFM_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		m_dEditDFMHigh[i] = THEAPP.m_pModelDataManager->m_dDFMHigh[i];
		m_dEditDFMLow[i] = THEAPP.m_pModelDataManager->m_dDFMLow[i];
		m_dEditDFMDerivativeSmFactor[i] = THEAPP.m_pModelDataManager->m_dDFMDerivativeSmFactor[i];
	}

	for (i = 0; i < DIFFUSED_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		m_dEditDiffusedHigh[i] = THEAPP.m_pModelDataManager->m_dDiffusedHigh[i];
		m_dEditDiffusedLow[i] = THEAPP.m_pModelDataManager->m_dDiffusedLow[i];
		m_dEditDiffusedSmFactor[i] = THEAPP.m_pModelDataManager->m_dDiffusedSmFactor[i];
	}

	ChangeLanguage();

	UpdateData(FALSE);

#ifdef INLINE_MODE
	GetDlgItem(IDC_BUTTON_OFFLINE_SPECULAR)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OFFLINE_DIFFUSED)->ShowWindow(FALSE);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSPVImageParamDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("4D 비전 파라미터 설정"));
		SetDlgItemText(IDOK, _T("저장"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Set 4D vision parameter"));
		SetDlgItemText(IDOK, _T("Save"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}

void CSPVImageParamDlg::OnBnClickedOk()
{
	UpdateData();

	THEAPP.m_pModelDataManager->m_dDFMPsx = m_dEditDFMPsx;
	THEAPP.m_pModelDataManager->m_dDFMPsy = m_dEditDFMPsy;
	THEAPP.m_pModelDataManager->m_dDFMTex = m_dEditDFMTex;
	THEAPP.m_pModelDataManager->m_dDFMTey = m_dEditDFMTey;

	int i;

	for (i = 0; i < DFM_MAX_SPECULAR_IMAGE_NUMBER; i++)
		THEAPP.m_pModelDataManager->m_dDFMSpecularImageSF[i] = m_dEditDFMSpecularImageSF[i];

	THEAPP.m_pModelDataManager->m_iDFMMeanFilterSize = m_iEditDFMMeanFilterSize;

	for (i = 0; i < DFM_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		THEAPP.m_pModelDataManager->m_dDFMHigh[i] = m_dEditDFMHigh[i];
		THEAPP.m_pModelDataManager->m_dDFMLow[i] = m_dEditDFMLow[i];
		THEAPP.m_pModelDataManager->m_dDFMDerivativeSmFactor[i] = m_dEditDFMDerivativeSmFactor[i];
	}

	for (i = 0; i < DIFFUSED_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		THEAPP.m_pModelDataManager->m_dDiffusedHigh[i] = m_dEditDiffusedHigh[i];
		THEAPP.m_pModelDataManager->m_dDiffusedLow[i] = m_dEditDiffusedLow[i];
		THEAPP.m_pModelDataManager->m_dDiffusedSmFactor[i] = m_dEditDiffusedSmFactor[i];
	}

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	CString strSpecularParam;
	strSpecularParam.Format("%s\\HW\\Vision_N%d\\SpecularParam.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);
	THEAPP.m_pModelDataManager->SaveSpecularParam(strSpecularParam);
}

void CSPVImageParamDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

void CSPVImageParamDlg::OnBnClickedButtonOfflineSpecular()
{
	THEAPP.m_pCameraManager->OfflineSpecularPhaseConv();
	//THEAPP.m_pCameraManager->OfflineSpecularPhaseConvForSubFolder();
}


void CSPVImageParamDlg::OnBnClickedButtonOfflineDiffused()
{
	THEAPP.m_pCameraManager->OfflineDiffusedVisionConv();
	//THEAPP.m_pCameraManager->OfflineDiffusedVisionConvForSubFolder();
}
