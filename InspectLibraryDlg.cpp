// InspectLibraryDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectLibraryDlg.h"
#include "afxdialogex.h"


// CInspectLibraryDlg 대화 상자

IMPLEMENT_DYNAMIC(CInspectLibraryDlg, CDialog)

CInspectLibraryDlg* CInspectLibraryDlg::m_pInstance = NULL;

CInspectLibraryDlg* CInspectLibraryDlg::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CInspectLibraryDlg;

		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

			m_pInstance->Create(IDD_INSPECT_LIBRARY_DLG, pFrame->GetActiveView());
		}
	}

	return m_pInstance;
}

void CInspectLibraryDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

void CInspectLibraryDlg::Show()
{
	SetWindowPos(&wndTopMost, 780, 200, 0, 0, SWP_NOSIZE);
	ShowWindow(SW_SHOW);

	m_bShowDlg = TRUE;

	LoadLibrary();
}

void CInspectLibraryDlg::Hide()
{
	ShowWindow(SW_HIDE);

	m_bShowDlg = FALSE;
}

CInspectLibraryDlg::CInspectLibraryDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_INSPECT_LIBRARY_DLG, pParent)
{
	m_bShowDlg = FALSE;

	m_iUnitImageWidth = -1;
	m_iUnitImageHeight = -1;
	GenEmptyObj(&m_HUnitImage);

	m_bAutoRoiCreateTriger = FALSE;
}

CInspectLibraryDlg::~CInspectLibraryDlg()
{
}

void CInspectLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INSPECT_LIBRARY_UNIT_VIEW, m_UnitView);

	DDX_Control(pDX, IDC_BUTTON_INSPECT_LIBRARY_ADD, m_bnInspectLibraryAdd);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_LIBRARY_APPLY, m_bnInspectLibraryApply);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_LIBRARY_CREATE, m_bnInspectLibraryCreate);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_LIBRARY_DELETE, m_bnInspectLibraryDelete);
	DDX_Control(pDX, IDC_BUTTON_INSPECT_LIBRARY_RENAME, m_bnInspectLibraryRename);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_bnClose);
	DDX_Control(pDX, IDC_COMBO_AUTO_SEG_MODEL, m_cbAutoSegModel);

}


BEGIN_MESSAGE_MAP(CInspectLibraryDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(UM_LIST_SELECT, OnListSelect)
	ON_BN_CLICKED(IDC_BUTTON_INSPECT_LIBRARY_ADD, &CInspectLibraryDlg::OnBnClickedButtonInspectLibraryAdd)
	ON_BN_CLICKED(IDC_BUTTON_INSPECT_LIBRARY_APPLY, &CInspectLibraryDlg::OnBnClickedButtonInspectLibraryApply)
	ON_BN_CLICKED(IDC_BUTTON_INSPECT_LIBRARY_CREATE, &CInspectLibraryDlg::OnBnClickedButtonInspectLibraryCreate)
	ON_BN_CLICKED(IDC_BUTTON_INSPECT_LIBRARY_DELETE, &CInspectLibraryDlg::OnBnClickedButtonInspectLibraryDelete)
	ON_BN_CLICKED(IDC_BUTTON_INSPECT_LIBRARY_RENAME, &CInspectLibraryDlg::OnBnClickedButtonInspectLibraryRename)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CInspectLibraryDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_INSPECT_LIBRARY_AUTO_SEG_CUSTOM, &CInspectLibraryDlg::OnBnClickedButtonInspectLibraryAutoSegCustom)
	ON_BN_CLICKED(IDC_BUTTON_INSPECT_LIBRARY_AUTO_SEG_START, &CInspectLibraryDlg::OnBnClickedButtonInspectLibraryAutoSegStart)
	ON_BN_CLICKED(IDC_BUTTON_INSPECT_LIBRARY_AUTO_SEG_END, &CInspectLibraryDlg::OnBnClickedButtonInspectLibraryAutoSegEnd)
END_MESSAGE_MAP()

BOOL CInspectLibraryDlg::PreTranslateMessage(MSG* pMsg)
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

// CInspectLibraryDlg 메시지 처리기
BOOL CInspectLibraryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTopMost, 1000, 5, 0, 0, SWP_NOSIZE);

	m_bnInspectLibraryAdd.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);
	m_bnInspectLibraryApply.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);
	m_bnInspectLibraryCreate.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);
	m_bnInspectLibraryDelete.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);
	m_bnInspectLibraryRename.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);
	m_bnClose.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);

	m_UnitView.CreateHWindow();
	m_UnitView.GetWindowRect(&m_UnitViewRect);
	this->ScreenToClient(&m_UnitViewRect);
	m_UnitView.ShowWindow(SW_SHOW);

	m_iCurListIdx = -1;

	m_GridLibraryList.SetListType(LIST_TYPE_INSPECT_LIBRARY);
	m_GridLibraryList.SetHwnd(this->m_hWnd);
	m_GridLibraryList.AttachGrid(this, IDC_GRID_INSPECT_LIBRARY_LIST);

	CString strTemp;
	for (int i = 0; i < 5; i++)
	{
		strTemp.Format("Sensitive %d", i + 1);
		m_cbAutoSegModel.AddString(strTemp);
	}
	m_cbAutoSegModel.AddString("Custom");

	m_cbAutoSegModel.SetCurSel(2);

	m_RoiAutoCreateDlg = new CRoiAutoCreateDlg();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CInspectLibraryDlg::OnPaint()
{
	// TODO: Add your message handler code here
	CPaintDC dc(this);

	if (m_iUnitImageWidth < 0)
		return;

	ClearWindow(m_UnitView.mlWindowHandle);

	if (THEAPP.m_pGFunction->ValidHImage(m_HUnitImage))
	{
		DispObj(m_HUnitImage, m_UnitView.mlWindowHandle);
	}

	// Do not call CDialog::OnPaint() for painting messages
}


BOOL CInspectLibraryDlg::OnEraseBkgnd(CDC *pDC)
{
	CRect r;
	GetClientRect(&r);
	pDC->FillSolidRect(r, TS_COLOR_BLACK_BG);
	pDC->SetBkColor(TS_COLOR_BLACK_BG);

	return TRUE;
}

void CInspectLibraryDlg::LoadLibrary()
{
	CInspectLibraryData *pInspectLibraryData;
	pInspectLibraryData = THEAPP.m_pInspectLibraryDataManager->GetInspectLibraryDataPtr();
	if (pInspectLibraryData->GetInspectLibraryCount() <= 0)
	{
		m_GridLibraryList.SetNumberRows(0);
		m_GridLibraryList.RedrawAll();
		return;
	}

	long lNoLibrary = pInspectLibraryData->GetInspectLibraryCount();

	m_GridLibraryList.SetNumberRows(lNoLibrary);

	int iRowNumber = 0;

	CInspectLibrary *pInspectLibrary;
	CString str;

	for (int iIdx = 0; iIdx < lNoLibrary; iIdx++)
	{
		pInspectLibrary = pInspectLibraryData->GetInspectLibrary(iIdx);

		str.Format("%d", iIdx + 1);
		m_GridLibraryList.QuickSetText(0, iRowNumber, str);
		m_GridLibraryList.QuickSetText(1, iRowNumber, pInspectLibrary->m_sLibraryName);
		if (pInspectLibrary->m_pROI->m_AlgorithmParam[0].m_bInspect)
			m_GridLibraryList.QuickSetText(2, iRowNumber, THEAPP.m_strDefectName[pInspectLibrary->m_pROI->m_AlgorithmParam[0].m_iDefectType]);
		else
			m_GridLibraryList.QuickSetText(2, iRowNumber, "");

		if (pInspectLibrary->m_pROI->m_AlgorithmParam[1].m_bInspect)
			m_GridLibraryList.QuickSetText(3, iRowNumber, THEAPP.m_strDefectName[pInspectLibrary->m_pROI->m_AlgorithmParam[1].m_iDefectType]);
		else
			m_GridLibraryList.QuickSetText(3, iRowNumber, "");

		if (pInspectLibrary->m_pROI->m_AlgorithmParam[2].m_bInspect)
			m_GridLibraryList.QuickSetText(4, iRowNumber, THEAPP.m_strDefectName[pInspectLibrary->m_pROI->m_AlgorithmParam[2].m_iDefectType]);
		else
			m_GridLibraryList.QuickSetText(4, iRowNumber, "");

		m_GridLibraryList.QuickSetText(5, iRowNumber, pInspectLibrary->m_sLibraryComment);

		++iRowNumber;
	}

	int iNoColumns;
	iNoColumns = m_GridLibraryList.GetNumberCols();

	m_GridLibraryList.BestFit(0, iNoColumns - 1, -1, UG_BESTFIT_TOPHEADINGS);
	m_GridLibraryList.RedrawAll();

	if (lNoLibrary > 0 && m_iCurListIdx < 0)
		m_GridLibraryList.MoveFirst();
}

void CInspectLibraryDlg::UpdateLibrary(int iRowNumber, CString sLibName)
{
	CInspectLibraryData *pInspectLibraryData;
	pInspectLibraryData = THEAPP.m_pInspectLibraryDataManager->GetInspectLibraryDataPtr();
	if (pInspectLibraryData->GetInspectLibraryCount() <= 0)
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

void CInspectLibraryDlg::OnBnClickedButtonInspectLibraryAdd()
{
	if (THEAPP.m_pInspectAdminViewDlg->DoCheckROISelected() == FALSE)
	{
		AfxMessageBox("라이브러리에 추가할 ROI를 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	CSetLibraryNameDlg dlg;
	dlg.SetType(0);
	dlg.SetLibraryType(0);

	if (dlg.DoModal() == IDOK)
	{
		GTRegion *pSelectedROI = NULL;
		pSelectedROI = THEAPP.m_pInspectAdminViewDlg->GetSelectedROI();
		if (pSelectedROI == NULL)
		{
			AfxMessageBox("라이브러리에 추가할 ROI를 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			return;
		}

		CInspectLibraryData *pInspectLibraryData;
		pInspectLibraryData = THEAPP.m_pInspectLibraryDataManager->GetInspectLibraryDataPtr();

		CInspectLibrary *pInspectLibrary;

		pInspectLibrary = new CInspectLibrary;
		pInspectLibrary->SetLibrary(dlg.GetLibraryName(), pSelectedROI, dlg.GetLibraryComment());

		pInspectLibraryData->AddInspectLibrary(pInspectLibrary);

		LoadLibrary();

		m_GridLibraryList.MoveLast();
	}

}

void CInspectLibraryDlg::OnBnClickedButtonInspectLibraryApply()
{
	if (THEAPP.m_pInspectAdminViewDlg->DoCheckROISelected() == FALSE)
	{
		AfxMessageBox("검사 알고리즘을 적용할 ROI를 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	if (m_iCurListIdx < 0)
	{
		AfxMessageBox("선택한 ROI에 적용할 검사 알고리즘을 리스트에서 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	GTRegion *pSelectedROI = NULL;
	pSelectedROI = THEAPP.m_pInspectAdminViewDlg->GetSelectedROI();
	if (pSelectedROI == NULL)
	{
		AfxMessageBox("검사 알고리즘을 적용할 ROI를 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	CInspectLibraryData *pInspectLibraryData;
	pInspectLibraryData = THEAPP.m_pInspectLibraryDataManager->GetInspectLibraryDataPtr();

	CInspectLibrary *pInspectLibrary;
	pInspectLibrary = pInspectLibraryData->GetInspectLibrary(m_iCurListIdx);

	pInspectLibrary->m_pROI->Duplicate(&pSelectedROI, TRUE);

	THEAPP.m_pInspectAdminViewDlg->UpdateView(TRUE);

	AfxMessageBox("검사 알고리즘이 적용되었습니다.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void CInspectLibraryDlg::OnBnClickedButtonInspectLibraryCreate()
{
	if (m_iCurListIdx < 0)
	{
		AfxMessageBox("ROI를 생성할 검사 알고리즘을 리스트에서 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	CInspectLibraryData *pInspectLibraryData;
	pInspectLibraryData = THEAPP.m_pInspectLibraryDataManager->GetInspectLibraryDataPtr();

	CInspectLibrary *pInspectLibrary;
	pInspectLibrary = pInspectLibraryData->GetInspectLibrary(m_iCurListIdx);

	GTRegion *pCopyRegion;

	pInspectLibrary->m_pROI->Duplicate(&pCopyRegion);

	pCopyRegion->miTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;

	DPOINT dLTPointM, dRBPointM, dNewLTPointM, dNewRBPointM, dNewCenterPoint;

	if (pCopyRegion->m_bRegionROI)
	{
		POINT LTPoint, RBPoint;
		Hlong lRow1, lRow2, lCol1, lCol2;

		HTuple HlRow1, HlRow2, HlCol1, HlCol2;

		SmallestRectangle1(pCopyRegion->m_HTeachPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

		LTPoint.x = HlCol1.L();
		LTPoint.y = HlRow1.L();

		MoveRegion(pCopyRegion->m_HTeachPolygonRgn, &(pCopyRegion->m_HTeachPolygonRgn), -LTPoint.y + 50, -LTPoint.x + 50);

		SmallestRectangle1(pCopyRegion->m_HTeachPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

		LTPoint.x = HlCol1.L();
		LTPoint.y = HlRow1.L();
		RBPoint.x = HlCol2.L();
		RBPoint.y = HlRow2.L();

		pCopyRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
		pCopyRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
	}
	else
	{
		pCopyRegion->GetLTPointM(&dLTPointM);

		dNewLTPointM.x = 50.0 * THEAPP.m_pCalDataService->GetPixelSize();
		dNewLTPointM.y = 50.0 * THEAPP.m_pCalDataService->GetPixelSize();

		pCopyRegion->SetLTPointM(dNewLTPointM);

		pCopyRegion->GetRBPointM(&dRBPointM);

		dNewRBPointM.x = dRBPointM.x - dLTPointM.x + 50.0 * THEAPP.m_pCalDataService->GetPixelSize();
		dNewRBPointM.y = dRBPointM.y - dLTPointM.y + 50.0 * THEAPP.m_pCalDataService->GetPixelSize();

		pCopyRegion->SetRBPointM(dNewRBPointM);
	}

	if (pCopyRegion->miInspectionType >= INSPECTION_TYPE_FIRST && pCopyRegion->miInspectionType <= INSPECTION_TYPE_LAST)
	{
		int iLastInspectionROIID = 0;
		iLastInspectionROIID = THEAPP.m_pModelDataManager->GetLastPadID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miInspectionROIID = iLastInspectionROIID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		int iLastLocalAlignID = 0;
		iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miLocalAlignID = iLastLocalAlignID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_DONTCARE)
	{
		int iLastDontCareID = 0;
		iLastDontCareID = THEAPP.m_pModelDataManager->GetLastDontCareID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miDontCareID = iLastDontCareID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_GENERATE)
	{
		int iLastGenerateID = 0;
		iLastGenerateID = THEAPP.m_pModelDataManager->GetLastGenerateID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miGenerateID = iLastGenerateID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_MASK)
	{
		int iLastMaskID = 0;
		iLastMaskID = THEAPP.m_pModelDataManager->GetLastMaskID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miMaskID = iLastMaskID + 1;
	}

	if (pCopyRegion->miInspectionType == INSPECTION_TYPE_COLOR)
	{
		int iLastColorInfoID = 0;
		iLastColorInfoID = THEAPP.m_pModelDataManager->GetLastColorInfoID(pCopyRegion->miTeachImageIndex);
		pCopyRegion->miColorInfoID = iLastColorInfoID + 1;
	}

	BOOL bTbLock = THEAPP.m_pInspectAdminViewDlg->GetTbLock();

	pCopyRegion->SetMovable(!bTbLock, FALSE);
	pCopyRegion->SetSizable(!bTbLock, FALSE);

	THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pCopyRegion);

	THEAPP.m_pInspectAdminViewDlg->UpdateView(TRUE);

	AfxMessageBox("신규 검사 ROI가 생성되었습니다.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

void CInspectLibraryDlg::OnBnClickedButtonInspectLibraryDelete()
{
	if (m_iCurListIdx < 0)
	{
		AfxMessageBox("삭제할 검사 알고리즘을 리스트에서 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	CString sLibName;
	sLibName = m_GridLibraryList.QuickGetText(1, m_iCurListIdx);

	if (sLibName == "")
		return;

	CInspectLibraryData *pInspectLibraryData;
	pInspectLibraryData = THEAPP.m_pInspectLibraryDataManager->GetInspectLibraryDataPtr();
	pInspectLibraryData->DeleteSelectedLibrary(sLibName);

	LoadLibrary();

	int iNoList = pInspectLibraryData->GetInspectLibraryCount();

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

void CInspectLibraryDlg::OnBnClickedButtonInspectLibraryRename()
{
	if (m_iCurListIdx < 0)
	{
		AfxMessageBox("명칭을 변경할 검사 알고리즘을 리스트에서 선택하여 주십시요.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	CInspectLibraryData *pInspectLibraryData;
	pInspectLibraryData = THEAPP.m_pInspectLibraryDataManager->GetInspectLibraryDataPtr();

	CInspectLibrary *pInspectLibrary;
	pInspectLibrary = pInspectLibraryData->GetInspectLibrary(m_iCurListIdx);

	CSetLibraryNameDlg dlg;
	dlg.SetType(1);
	dlg.SetLibraryType(0);

	dlg.SetLibraryName(pInspectLibrary->m_sLibraryName);
	dlg.SetLibraryComment(pInspectLibrary->m_sLibraryComment);

	if (dlg.DoModal() == IDOK)
	{
		pInspectLibrary->m_sLibraryName = dlg.GetLibraryName();
		pInspectLibrary->m_sLibraryComment = dlg.GetLibraryComment();

		LoadLibrary();
	}
}

void CInspectLibraryDlg::OnBnClickedButtonClose()
{
	THEAPP.m_pInspectLibraryDataManager->Save();

	Hide();
}

LRESULT CInspectLibraryDlg::OnListSelect(WPARAM wParam, LPARAM lParam)
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

	CInspectLibraryData *pInspectLibraryData;
	pInspectLibraryData = THEAPP.m_pInspectLibraryDataManager->GetInspectLibraryDataPtr();

	CInspectLibrary *pInspectLibrary;
	pInspectLibrary = pInspectLibraryData->GetInspectLibrary(m_iCurListIdx);

	if (pInspectLibrary == NULL)
		return 0;

	HObject HROIHRegion;
	HROIHRegion = pInspectLibrary->m_pROI->GetROIHRegion(THEAPP.m_pCalDataService);
	if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion) == FALSE)
		return 0;

	GenEmptyObj(&m_HUnitImage);

	Hlong lLTPoint_x, lLTPoint_y, lRBPoint_x, lRBPoint_y;
	HTuple hLTPoint_x, hLTPoint_y, hRBPoint_x, hRBPoint_y;
	Hlong lViewImageWidth, lViewImageHeight;

	SmallestRectangle1(HROIHRegion, &(hLTPoint_y), &(hLTPoint_x), &(hRBPoint_y), &(hRBPoint_x));
	lLTPoint_y = hLTPoint_y.L();
	lLTPoint_x = hLTPoint_x.L();
	lRBPoint_y = hRBPoint_y.L();
	lRBPoint_x = hRBPoint_x.L();

	MoveRegion(HROIHRegion, &HROIHRegion, -lLTPoint_y, -lLTPoint_x);

	lViewImageWidth = lRBPoint_x - lLTPoint_x + 1;
	lViewImageHeight = lRBPoint_y - lLTPoint_y + 1;

	HObject HBlackImage, HWhiteImage;
	RegionToBin(HROIHRegion, &HWhiteImage, 255, 0, lViewImageWidth, lViewImageHeight);
	GenImageConst(&HBlackImage, "byte", lViewImageWidth, lViewImageHeight);

	GenEmptyObj(&m_HUnitImage);
	AppendChannel(m_HUnitImage, HBlackImage, &m_HUnitImage);
	AppendChannel(m_HUnitImage, HWhiteImage, &m_HUnitImage);
	AppendChannel(m_HUnitImage, HBlackImage, &m_HUnitImage);

	m_iUnitImageWidth = (int)lViewImageWidth;
	m_iUnitImageHeight = (int)lViewImageHeight;

	HTuple HWidth, HHeight;
	Hlong lWidth, lHeight;
	GetImageSize(m_HUnitImage, &HWidth, &HHeight);
	lWidth = HWidth.L();
	lHeight = HHeight.L();

	if (lWidth > 10 && lHeight > 10)
	{
		SetPart(m_UnitView.mlWindowHandle, 0, 0, lHeight - 1, lWidth - 1);
		InvalidateRect(m_UnitViewRect, FALSE);
	}

	return 1;
}


void CInspectLibraryDlg::OnBnClickedButtonInspectLibraryAutoSegCustom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_RoiAutoCreateDlg->DoModal();
}

UINT CInspectLibraryDlg::AutoROINetworkThread(LPVOID lp)
{
	CInspectLibraryDlg* pInspectLibraryDlg = (CInspectLibraryDlg*)lp;

	int imageindex = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;
	if (imageindex < 0)
		imageindex = 0;

	THEAPP.m_pInspectAdminViewDlg->SendImageToServer(imageindex, pInspectLibraryDlg->m_cbAutoSegModel.GetCurSel(), CHANNEL_TYPE_COLOR);

	pInspectLibraryDlg->m_bAutoRoiCreateTriger = FALSE;
	return 1;
}

void CInspectLibraryDlg::OnBnClickedButtonInspectLibraryAutoSegStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// m_RoiAutoCreateDlg->SendImageToServer(m_ComboSourceImage.GetCurSel(), THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_ComboSourceImage.GetCurSel()]);
	// m_RoiAutoCreateDlg->DoModal();

	// THEAPP.m_pInspectAdminViewDlg->SendImageToServer(m_ComboSourceImage.GetCurSel(), THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[m_ComboSourceImage.GetCurSel()]);

	if (!m_bAutoRoiCreateTriger)
	{
		m_bAutoRoiCreateTriger = TRUE;
		// int imageindex = m_ComboSourceImage.GetCurSel();
		// THEAPP.m_pInspectAdminViewDlg->SendImageToServer(imageindex, m_ComboSelectAutoRoiModel.GetCurSel(), THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[imageindex]);
		AfxBeginThread(AutoROINetworkThread, this);
		AfxMessageBox("현재 선택된 이미지 작업 진행", MB_SYSTEMMODAL);
	}
	else
	{
		AfxMessageBox("작업이 진행 중입니다.", MB_SYSTEMMODAL);
	}
}


void CInspectLibraryDlg::OnBnClickedButtonInspectLibraryAutoSegEnd()
{
	THEAPP.m_pInspectAdminViewDlg->DeleteAutoCreateROI();
}
