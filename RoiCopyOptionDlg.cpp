// RoiCopyOptionDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "RoiCopyOptionDlg.h"
#include "afxdialogex.h"


// CRoiCopyOptionDlg 대화 상자

IMPLEMENT_DYNAMIC(CRoiCopyOptionDlg, CDialog)

CRoiCopyOptionDlg::CRoiCopyOptionDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ROI_COPY_OPTION_DLG, pParent)
{
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		m_bCheckCopyImage[i] = FALSE;

	m_bCheckDeleteRoiFirst = TRUE;
}

CRoiCopyOptionDlg::~CRoiCopyOptionDlg()
{
}

void CRoiCopyOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_1, m_bCheckCopyImage[0]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_2, m_bCheckCopyImage[1]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_3, m_bCheckCopyImage[2]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_4, m_bCheckCopyImage[3]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_5, m_bCheckCopyImage[4]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_6, m_bCheckCopyImage[5]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_7, m_bCheckCopyImage[6]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_8, m_bCheckCopyImage[7]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_9, m_bCheckCopyImage[8]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_10, m_bCheckCopyImage[9]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_11, m_bCheckCopyImage[10]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_12, m_bCheckCopyImage[11]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_13, m_bCheckCopyImage[12]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_14, m_bCheckCopyImage[13]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_15, m_bCheckCopyImage[14]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_16, m_bCheckCopyImage[15]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_17, m_bCheckCopyImage[16]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_18, m_bCheckCopyImage[17]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_19, m_bCheckCopyImage[18]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_20, m_bCheckCopyImage[19]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_21, m_bCheckCopyImage[20]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_22, m_bCheckCopyImage[21]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_23, m_bCheckCopyImage[22]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_24, m_bCheckCopyImage[23]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_25, m_bCheckCopyImage[24]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_26, m_bCheckCopyImage[25]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_27, m_bCheckCopyImage[26]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_28, m_bCheckCopyImage[27]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_29, m_bCheckCopyImage[28]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_30, m_bCheckCopyImage[29]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_31, m_bCheckCopyImage[30]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_32, m_bCheckCopyImage[31]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_33, m_bCheckCopyImage[32]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_34, m_bCheckCopyImage[33]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_35, m_bCheckCopyImage[34]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_36, m_bCheckCopyImage[35]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_37, m_bCheckCopyImage[36]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_38, m_bCheckCopyImage[37]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_39, m_bCheckCopyImage[38]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_40, m_bCheckCopyImage[39]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_41, m_bCheckCopyImage[40]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_42, m_bCheckCopyImage[41]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_43, m_bCheckCopyImage[42]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_44, m_bCheckCopyImage[43]);
	DDX_Check(pDX, IDC_CHECK_COPY_IMAGE_45, m_bCheckCopyImage[44]);

	DDX_Check(pDX, IDC_CHECK_DELETE_ROI_FIRST, m_bCheckDeleteRoiFirst);
}


BEGIN_MESSAGE_MAP(CRoiCopyOptionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRoiCopyOptionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRoiCopyOptionDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CRoiCopyOptionDlg 메시지 처리기

BOOL CRoiCopyOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRoiCopyOptionDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnOK();
}


void CRoiCopyOptionDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
