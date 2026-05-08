// LightLibraryDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "LightLibraryDlg.h"
#include "afxdialogex.h"


// CLightLibraryDlg 대화 상자

IMPLEMENT_DYNAMIC(CLightLibraryDlg, CDialog)

CLightLibraryDlg* CLightLibraryDlg::m_pInstance = NULL;

CLightLibraryDlg* CLightLibraryDlg::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CLightLibraryDlg;

		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

			m_pInstance->Create(IDD_LIGHT_LIBRARY_DLG, pFrame->GetActiveView());
		}
	}

	return m_pInstance;
}

void CLightLibraryDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

void CLightLibraryDlg::Show()
{
	SetWindowPos(&wndTopMost, 780, 200, 0, 0, SWP_NOSIZE);
	ShowWindow(SW_SHOW);

	m_bShowDlg = TRUE;

	LoadLibrary();
}

void CLightLibraryDlg::Hide()
{
	ShowWindow(SW_HIDE);

	m_bShowDlg = FALSE;
}

CLightLibraryDlg::CLightLibraryDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_INSPECT_LIBRARY_DLG, pParent)
{
	m_bShowDlg = FALSE;
	m_iCurSelectedPageIndex = 0;
}

CLightLibraryDlg::~CLightLibraryDlg()
{
	THEAPP.m_pLightLibraryDataManager->Save();
}

void CLightLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_LIGHT_LIBRARY_ADD, m_bnLightLibraryAdd);
	DDX_Control(pDX, IDC_BUTTON_LIGHT_LIBRARY_APPLY, m_bnLightLibraryApply);
	DDX_Control(pDX, IDC_BUTTON_LIGHT_LIBRARY_DELETE, m_bnLightLibraryDelete);
	DDX_Control(pDX, IDC_BUTTON_LIGHT_LIBRARY_RENAME, m_bnLightLibraryRename);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_bnClose);
}


BEGIN_MESSAGE_MAP(CLightLibraryDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(UM_LIST_SELECT, OnListSelect)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_LIBRARY_ADD, &CLightLibraryDlg::OnBnClickedButtonLightLibraryAdd)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_LIBRARY_APPLY, &CLightLibraryDlg::OnBnClickedButtonLightLibraryApply)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_LIBRARY_DELETE, &CLightLibraryDlg::OnBnClickedButtonLightLibraryDelete)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_LIBRARY_RENAME, &CLightLibraryDlg::OnBnClickedButtonLightLibraryRename)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CLightLibraryDlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()

BOOL CLightLibraryDlg::PreTranslateMessage(MSG* pMsg)
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

// CLightLibraryDlg 메시지 처리기
BOOL CLightLibraryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTopMost, 5, 5, 0, 0, SWP_NOSIZE);

	m_bnLightLibraryAdd.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);
	m_bnLightLibraryApply.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);
	m_bnLightLibraryDelete.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);
	m_bnLightLibraryRename.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);
	m_bnClose.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);

	m_iCurListIdx = -1;

	m_GridLibraryList.SetListType(LIST_TYPE_LIGHT_LIBRARY);
	m_GridLibraryList.SetHwnd(this->m_hWnd);
	m_GridLibraryList.AttachGrid(this, IDC_GRID_LIGHT_LIBRARY_LIST);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLightLibraryDlg::OnPaint()
{
	// TODO: Add your message handler code here
	CPaintDC dc(this);

	// Do not call CDialog::OnPaint() for painting messages
}


BOOL CLightLibraryDlg::OnEraseBkgnd(CDC *pDC)
{
	CRect r;
	GetClientRect(&r);
	pDC->FillSolidRect(r, TS_COLOR_BLACK_BG);
	pDC->SetBkColor(TS_COLOR_BLACK_BG);

	return TRUE;
}

void CLightLibraryDlg::LoadLibrary()
{
	CLightLibraryData *pLightLibraryData;
	pLightLibraryData = THEAPP.m_pLightLibraryDataManager->GetLightLibraryDataPtr();
	if (pLightLibraryData->GetLightLibraryCount() <= 0)
	{
		m_GridLibraryList.SetNumberRows(0);
		m_GridLibraryList.RedrawAll();
		return;
	}

	long lNoLibrary = pLightLibraryData->GetLightLibraryCount();

	m_GridLibraryList.SetNumberRows(lNoLibrary);

	int iRowNumber = 0;

	CLightLibrary *pLightLibrary;
	CString str;

	for (int iIdx = 0; iIdx < lNoLibrary; iIdx++)
	{
		pLightLibrary = pLightLibraryData->GetLightLibrary(iIdx);

		str.Format("%d", iIdx + 1);
		m_GridLibraryList.QuickSetText(0, iRowNumber, str);
		m_GridLibraryList.QuickSetText(1, iRowNumber, pLightLibrary->m_sLibraryName);
		m_GridLibraryList.QuickSetText(2, iRowNumber, pLightLibrary->m_sLibraryComment);

		++iRowNumber;
	}

	int iNoColumns;
	iNoColumns = m_GridLibraryList.GetNumberCols();

	m_GridLibraryList.BestFit(0, iNoColumns - 1, -1, UG_BESTFIT_TOPHEADINGS);
	m_GridLibraryList.RedrawAll();

	if (lNoLibrary > 0 && m_iCurListIdx < 0)
		m_GridLibraryList.MoveFirst();
}

void CLightLibraryDlg::UpdateLibrary(int iRowNumber, CString sLibName)
{
	CLightLibraryData *pLightLibraryData;
	pLightLibraryData = THEAPP.m_pLightLibraryDataManager->GetLightLibraryDataPtr();
	if (pLightLibraryData->GetLightLibraryCount() <= 0)
	{
		m_GridLibraryList.RedrawAll();
		return;
	}

	CString str;

	str.Format("%d", iRowNumber + 1);
	m_GridLibraryList.QuickSetText(0, iRowNumber, str);
	m_GridLibraryList.QuickSetText(1, iRowNumber, sLibName);

	m_GridLibraryList.RedrawRow(iRowNumber);
}

void CLightLibraryDlg::OnBnClickedButtonLightLibraryAdd()
{
	CSetLibraryNameDlg dlg;
	dlg.SetType(0);
	dlg.SetLibraryType(1);

	if (dlg.DoModal() == IDOK)
	{
		SPage PageInfo;
		for (int i = 0; i < 32; i++)
			PageInfo.uiChannel[i] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurSelectedPageIndex].uiChannel[i];

		CLightLibraryData *pLightLibraryData;
		pLightLibraryData = THEAPP.m_pLightLibraryDataManager->GetLightLibraryDataPtr();

		CLightLibrary *pLightLibrary;

		pLightLibrary = new CLightLibrary;
		pLightLibrary->SetLibrary(dlg.GetLibraryName(), &PageInfo, dlg.GetLibraryComment());

		pLightLibraryData->AddLightLibrary(pLightLibrary);

		LoadLibrary();

		m_GridLibraryList.MoveLast();
	}

}

void CLightLibraryDlg::OnBnClickedButtonLightLibraryApply()
{
	if (m_iCurListIdx < 0)
	{
		AfxMessageBox("선택한 Page에 적용할 조명을 리스트에서 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	CLightLibraryData *pLightLibraryData;
	pLightLibraryData = THEAPP.m_pLightLibraryDataManager->GetLightLibraryDataPtr();

	CLightLibrary *pLightLibrary;
	pLightLibrary = pLightLibraryData->GetLightLibrary(m_iCurListIdx);

	for (int i = 0; i < 32; i++)
		m_UpdatePage.uiChannel[i] = pLightLibrary->m_Page.uiChannel[i];

	delegateSelectedListApply.FireEvent((WPARAM)1);

	AfxMessageBox("조명 채널값이 적용되었습니다.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void CLightLibraryDlg::OnBnClickedButtonLightLibraryDelete()
{
	if (m_iCurListIdx < 0)
	{
		AfxMessageBox("삭제할 조명을 리스트에서 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	CString sLibName;
	sLibName = m_GridLibraryList.QuickGetText(1, m_iCurListIdx);

	if (sLibName == "")
		return;

	CLightLibraryData *pLightLibraryData;
	pLightLibraryData = THEAPP.m_pLightLibraryDataManager->GetLightLibraryDataPtr();
	pLightLibraryData->DeleteSelectedLibrary(sLibName);

	LoadLibrary();

	int iNoList = pLightLibraryData->GetLightLibraryCount();

	if (iNoList == 0)
	{
		m_iCurListIdx = -1;
	}
	else if (iNoList >= (m_iCurListIdx + 1))
	{
		m_GridLibraryList.MoveCurrent(m_iCurListIdx);
	}
	else
	{
		m_GridLibraryList.MoveCurrent(m_iCurListIdx - 1);
	}
}

void CLightLibraryDlg::OnBnClickedButtonLightLibraryRename()
{
	if (m_iCurListIdx < 0)
	{
		AfxMessageBox("명칭을 변경할 조명을 리스트에서 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	CLightLibraryData *pLightLibraryData;
	pLightLibraryData = THEAPP.m_pLightLibraryDataManager->GetLightLibraryDataPtr();

	CLightLibrary *pLightLibrary;
	pLightLibrary = pLightLibraryData->GetLightLibrary(m_iCurListIdx);

	CSetLibraryNameDlg dlg;
	dlg.SetType(1);
	dlg.SetLibraryType(1);
	dlg.SetLibraryName(pLightLibrary->m_sLibraryName);
	dlg.SetLibraryComment(pLightLibrary->m_sLibraryComment);

	if (dlg.DoModal() == IDOK)
	{
		pLightLibrary->m_sLibraryName = dlg.GetLibraryName();
		pLightLibrary->m_sLibraryComment = dlg.GetLibraryComment();

		LoadLibrary();
	}
}

void CLightLibraryDlg::OnBnClickedButtonClose()
{
	THEAPP.m_pLightLibraryDataManager->Save();

	Hide();
}

LRESULT CLightLibraryDlg::OnListSelect(WPARAM wParam, LPARAM lParam)
{
	CUGCell cell;
	m_GridLibraryList.GetCell(0, (int)wParam, &cell);
	int k = cell.GetNumber();
	k--;

	if (k < 0)
	{
		m_iCurListIdx = -1;
		return 1;
	}

	m_iCurListIdx = k;

	CLightLibraryData *pLightLibraryData;
	pLightLibraryData = THEAPP.m_pLightLibraryDataManager->GetLightLibraryDataPtr();

	CLightLibrary *pLightLibrary;
	pLightLibrary = pLightLibraryData->GetLightLibrary(m_iCurListIdx);

	if (pLightLibrary == NULL)
		return 0;

	return 1;
}
