// UnitConvertDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "UnitConvertDlg.h"
#include "afxdialogex.h"


// CUnitConvertDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUnitConvertDlg, CDialog)

CUnitConvertDlg::CUnitConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUnitConvertDlg::IDD, pParent)
{

	m_dEditLengthInputUm = 0.0;
	m_iEditLengthOutputPixelUm = 0;
	m_iEditLengthInputPixel = 0;
	m_dEditLengthOutputUm = 0.0;
	m_dEditAreaInputMm = 0.0;
	m_iEditAreaOutputPixel = 0;
	m_iEditAreaInputPixel = 0;
	m_dEditAreaOutputMm = 0.0;
}

CUnitConvertDlg::~CUnitConvertDlg()
{
}

void CUnitConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LENGTH_INPUT_UM, m_dEditLengthInputUm);
	DDX_Text(pDX, IDC_EDIT_LENGTH_OUTPUT_PIXEL, m_iEditLengthOutputPixelUm);
	DDX_Text(pDX, IDC_EDIT_LENGTH_INPUT_PIXEL, m_iEditLengthInputPixel);
	DDX_Text(pDX, IDC_EDIT_LENGTH_OUTPUT_UM, m_dEditLengthOutputUm);
	DDX_Text(pDX, IDC_EDIT_AREA_INPUT_MM, m_dEditAreaInputMm);
	DDX_Text(pDX, IDC_EDIT_AREA_OUTPUT_PIXEL, m_iEditAreaOutputPixel);
	DDX_Text(pDX, IDC_EDIT_AREA_INPUT_PIXEL, m_iEditAreaInputPixel);
	DDX_Text(pDX, IDC_EDIT_AREA_OUTPUT_MM, m_dEditAreaOutputMm);

	DDX_Control(pDX, IDC_BUTTON_CONVERT_LENGTH_UM, m_bnConvertLengthUm);
	DDX_Control(pDX, IDC_BUTTON_CONVERT_LENGTH_PIXEL, m_bnConvertLengthPixel);
	DDX_Control(pDX, IDC_BUTTON_CONVERT_AREA_MM, m_bnConvertAreaMm);
	DDX_Control(pDX, IDC_BUTTON_CONVERT_AREA_PIXEL, m_bnConvertAreaPixel);
	DDX_Control(pDX, IDOK, m_bnOK);
}


BEGIN_MESSAGE_MAP(CUnitConvertDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT_LENGTH_UM, &CUnitConvertDlg::OnBnClickedButtonConvertLengthUm)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT_LENGTH_PIXEL, &CUnitConvertDlg::OnBnClickedButtonConvertLengthPixel)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT_AREA_MM, &CUnitConvertDlg::OnBnClickedButtonConvertAreaMm)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT_AREA_PIXEL, &CUnitConvertDlg::OnBnClickedButtonConvertAreaPixel)
	ON_BN_CLICKED(IDOK, &CUnitConvertDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CUnitConvertDlg 메시지 처리기입니다.


void CUnitConvertDlg::OnBnClickedButtonConvertLengthUm()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	double dTemp = m_dEditLengthInputUm / THEAPP.m_pCalDataService->GetPixelSize();
	m_iEditLengthOutputPixelUm = (int)(dTemp + 0.5);

	UpdateData(FALSE);
}


void CUnitConvertDlg::OnBnClickedButtonConvertLengthPixel()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditLengthOutputUm = (double)m_iEditLengthInputPixel * THEAPP.m_pCalDataService->GetPixelSize();
	CString sTemp;
	sTemp.Format("%.1lf", m_dEditLengthOutputUm);
	m_dEditLengthOutputUm = atof(sTemp);

	UpdateData(FALSE);
}


void CUnitConvertDlg::OnBnClickedButtonConvertAreaMm()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	double dTemp = m_dEditAreaInputMm / (THEAPP.m_pCalDataService->GetPixelArea() * 0.001 * 0.001);
	m_iEditAreaOutputPixel = (int)(dTemp + 0.5);

	UpdateData(FALSE);
}


void CUnitConvertDlg::OnBnClickedButtonConvertAreaPixel()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditAreaOutputMm = (double)m_iEditAreaInputPixel * (THEAPP.m_pCalDataService->GetPixelArea() * 0.001 * 0.001);
	CString sTemp;
	sTemp.Format("%.8lf", m_dEditAreaOutputMm);
	m_dEditAreaOutputMm = atof(sTemp);

	UpdateData(FALSE);
}


void CUnitConvertDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnOK();
}


BOOL CUnitConvertDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CUnitConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//m_bnConvertLengthUm.Set_Text(_T("단위 변환기"));
	m_bnConvertLengthUm.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	//m_bnConvertLengthPixel.Set_Text(_T("단위 변환기"));
	m_bnConvertLengthPixel.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	//m_bnConvertAreaMm.Set_Text(_T("단위 변환기"));
	m_bnConvertAreaMm.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	//m_bnConvertAreaPixel.Set_Text(_T("단위 변환기"));
	m_bnConvertAreaPixel.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	//m_bnOK.Set_Text(_T("단위 변환기"));
	m_bnOK.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
