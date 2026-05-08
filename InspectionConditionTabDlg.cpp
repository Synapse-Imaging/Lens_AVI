// InspectionConditionTabDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectionConditionTabDlg.h"
#include "afxdialogex.h"


// CInspectionConditionTabDlg 대화 상자입니다.

CInspectionConditionTabDlg* CInspectionConditionTabDlg::m_pInstance = NULL;

CInspectionConditionTabDlg* CInspectionConditionTabDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectionConditionTabDlg();
		if (!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_TEACH_INSPECTION_CONDITION_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectionConditionTabDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectionConditionTabDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CInspectionConditionTabDlg, CDialog)

CInspectionConditionTabDlg::CInspectionConditionTabDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectionConditionTabDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT + 120, VIEW1_DLG3_TOP + 50 + 50 + VIEW1_DLG3_HEIGHT + 270 + 170 + 45, VIEW1_DLG3_WIDTH - 150, VIEW1_DLG3_HEIGHT - 35);

	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
		m_pConditionDlg[i] = NULL;
}

CInspectionConditionTabDlg::~CInspectionConditionTabDlg()
{
	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
		SAFE_DELETE(m_pConditionDlg[i]);
}

void CInspectionConditionTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONDITION_TAB, m_InspectionConditionTabCtrl);
}


BEGIN_MESSAGE_MAP(CInspectionConditionTabDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_CONDITION_TAB, &CInspectionConditionTabDlg::OnSelchangeInspectionConditionTab)
END_MESSAGE_MAP()


// CInspectionConditionTabDlg 메시지 처리기입니다.

BOOL CInspectionConditionTabDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_InspectionConditionTabCtrl.DeleteAllItems();
	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
		m_InspectionConditionTabCtrl.InsertItem(i, g_strInspectionTypeName[i].c_str());

	CRect DlgRect;

	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		m_pConditionDlg[i] = new CConditionDlg;
		m_pConditionDlg[i]->Create(IDD_CONDITION_DIALOG, &m_InspectionConditionTabCtrl);
		m_pConditionDlg[i]->GetWindowRect(&DlgRect);
		m_pConditionDlg[i]->MoveWindow(5, 25, DlgRect.Width(), DlgRect.Height());

		if (i == 0)
			m_pConditionDlg[i]->ShowWindow(SW_SHOW);
		else
			m_pConditionDlg[i]->ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CInspectionConditionTabDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CInspectionConditionTabDlg::OnSelchangeInspectionConditionTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iSelected = m_InspectionConditionTabCtrl.GetCurSel();

	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		if (i == iSelected)
			m_pConditionDlg[i]->ShowWindow(SW_SHOW);
		else
			m_pConditionDlg[i]->ShowWindow(SW_HIDE);
	}

	*pResult = 0;
}