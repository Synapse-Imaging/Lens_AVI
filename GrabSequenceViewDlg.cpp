// GrabSequenceViewDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "GrabSequenceViewDlg.h"
#include "afxdialogex.h"


// CGrabSequenceViewDlg 대화 상자

IMPLEMENT_DYNAMIC(CGrabSequenceViewDlg, CDialog)

CGrabSequenceViewDlg* CGrabSequenceViewDlg::m_pInstance = NULL;

CGrabSequenceViewDlg* CGrabSequenceViewDlg::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CGrabSequenceViewDlg;

		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

			m_pInstance->Create(IDD_GRAB_SEQUENCE_VIEW_DLG, pFrame->GetActiveView());
		}
	}

	return m_pInstance;
}

void CGrabSequenceViewDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

void CGrabSequenceViewDlg::Show()
{
	SetWindowPos(&wndTopMost, 780, 200, 0, 0, SWP_NOSIZE);
	ShowWindow(SW_SHOW);

	m_bShowDlg = TRUE;

	int i, j;

	for (i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		m_iEditAddrCount[i] = THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[i];
		m_sEditDesc[i] = THEAPP.m_pModelDataManager->m_PageControlData.m_sAddrDesc[i];
	}

	UpdateData(FALSE);

}

void CGrabSequenceViewDlg::Hide()
{
	ShowWindow(SW_HIDE);

	m_bShowDlg = FALSE;
}

CGrabSequenceViewDlg::CGrabSequenceViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_GRAB_SEQUENCE_VIEW_DLG, pParent)
{
	m_bShowDlg = FALSE;

	for (int i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		m_iEditAddrCount[i] = LIGHTCTRL_GRAB_ADDR_PAGE_COUNT;
		m_sEditDesc[i] = _T("");
	}
}

CGrabSequenceViewDlg::~CGrabSequenceViewDlg()
{
}

void CGrabSequenceViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ADDR0_COUNT, m_iEditAddrCount[0]);
	DDX_Text(pDX, IDC_EDIT_ADDR1_COUNT, m_iEditAddrCount[1]);
	DDX_Text(pDX, IDC_EDIT_ADDR2_COUNT, m_iEditAddrCount[2]);
	DDX_Text(pDX, IDC_EDIT_ADDR3_COUNT, m_iEditAddrCount[3]);
	DDX_Text(pDX, IDC_EDIT_ADDR4_COUNT, m_iEditAddrCount[4]);
	DDX_Text(pDX, IDC_EDIT_ADDR5_COUNT, m_iEditAddrCount[5]);
	DDX_Text(pDX, IDC_EDIT_ADDR6_COUNT, m_iEditAddrCount[6]);
	DDX_Text(pDX, IDC_EDIT_ADDR7_COUNT, m_iEditAddrCount[7]);
	DDX_Text(pDX, IDC_EDIT_ADDR8_COUNT, m_iEditAddrCount[8]);
	DDX_Text(pDX, IDC_EDIT_ADDR9_COUNT, m_iEditAddrCount[9]);
	DDX_Text(pDX, IDC_EDIT_ADDR10_COUNT, m_iEditAddrCount[10]);
	DDX_Text(pDX, IDC_EDIT_ADDR11_COUNT, m_iEditAddrCount[11]);
	DDX_Text(pDX, IDC_EDIT_ADDR12_COUNT, m_iEditAddrCount[12]);
	DDX_Text(pDX, IDC_EDIT_ADDR13_COUNT, m_iEditAddrCount[13]);
	DDX_Text(pDX, IDC_EDIT_ADDR14_COUNT, m_iEditAddrCount[14]);
	DDX_Text(pDX, IDC_EDIT_ADDR15_COUNT, m_iEditAddrCount[15]);
	DDX_Text(pDX, IDC_EDIT_ADDR16_COUNT, m_iEditAddrCount[16]);

	DDX_Text(pDX, IDC_EDIT_DESC_ADDR0, m_sEditDesc[0]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR1, m_sEditDesc[1]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR2, m_sEditDesc[2]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR3, m_sEditDesc[3]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR4, m_sEditDesc[4]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR5, m_sEditDesc[5]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR6, m_sEditDesc[6]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR7, m_sEditDesc[7]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR8, m_sEditDesc[8]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR9, m_sEditDesc[9]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR10, m_sEditDesc[10]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR11, m_sEditDesc[11]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR12, m_sEditDesc[12]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR13, m_sEditDesc[13]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR14, m_sEditDesc[14]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR15, m_sEditDesc[15]);
	DDX_Text(pDX, IDC_EDIT_DESC_ADDR16, m_sEditDesc[16]);
}


BEGIN_MESSAGE_MAP(CGrabSequenceViewDlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CGrabSequenceViewDlg 메시지 처리기

BOOL CGrabSequenceViewDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_NCLBUTTONDOWN:
		SetActiveWindow();
		return FALSE;

	case WM_KEYDOWN:
		if ((int)pMsg->wParam == VK_ESCAPE || (int)pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}

	case WM_SYSKEYDOWN:
		if (pMsg->wParam == VK_F4)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CGrabSequenceViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTopMost, 100, 100, 0, 0, SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CGrabSequenceViewDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	Hide();

	CDialog::OnClose();
}
