// TabControlDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TabControlDlg.h"
#include "afxdialogex.h"

// CTabControlDlg 대화 상자입니다.
CTabControlDlg* CTabControlDlg::m_pInstance = NULL;

CTabControlDlg* CTabControlDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CTabControlDlg();
		if (!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_TEACHING_TAB_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CTabControlDlg::DeleteInstance()
{
	m_pThresholdHistogramDlg->DeleteInstance();
	m_pLightControlDlg->DeleteInstance();
	m_pJogSetDlg->DeleteInstance();
	m_pInspectionConditionTabDlg->DeleteInstance();
	m_pExtraDlg->DeleteInstance();

	m_pTeachParamDlg->DeleteInstance();

	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CTabControlDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CTabControlDlg, CDialog)

CTabControlDlg::CTabControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabControlDlg::IDD, pParent)
{
	//SetPosition(VIEW1_DLG3_LEFT+90, VIEW1_DLG3_TOP+50, VIEW1_DLG3_WIDTH+800, VIEW1_DLG3_HEIGHT+800);
	SetPosition(VIEW1_DLG3_LEFT + 50, VIEW1_DLG3_TOP + 50, VIEW1_DLG3_WIDTH + 810, VIEW1_DLG3_HEIGHT + 800);
	m_iCurrentTab = 0;

	m_pTeachingAlgorithmTabDlg = NULL;
	m_pAutoFocusDlg = NULL;
	m_pInspectLibraryDlg = NULL;
	m_pGrabSequenceViewDlg = NULL;
	m_pMotionControlDlg = NULL;
	m_pCameraLightManagerDlg = NULL;
}

CTabControlDlg::~CTabControlDlg()
{
}

void CTabControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC__TEACHING_TAB, m_TabControl);
	DDX_Control(pDX, IDC_BTN_SCROLL_UP, m_btnPrev);
	DDX_Control(pDX, IDC_BTN_SCROLL_DOWN, m_btnNext);
}


BEGIN_MESSAGE_MAP(CTabControlDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC__TEACHING_TAB, &CTabControlDlg::OnSelchangeTeachingTab)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SCROLL_UP, &CTabControlDlg::OnBnClickedBtnPrev)
	ON_BN_CLICKED(IDC_BTN_SCROLL_DOWN, &CTabControlDlg::OnBnClickedBtnNext)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


// CTabControlDlg 메시지 처리기입니다.


BOOL CTabControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iCurrentTab = 0;

	m_pThresholdHistogramDlg = CThresholdHistogramDlg::GetInstance();
	m_pLightControlDlg = CLightControlDlg::GetInstance();
	m_pJogSetDlg = CJogSetDlg::GetInstance();
	m_pInspectionConditionTabDlg = CInspectionConditionTabDlg::GetInstance();
	m_pExtraDlg = CExtraDlg::GetInstance();

	m_pTeachParamDlg = CTeachParamDlg::GetInstance();

	m_pTeachingAlgorithmTabDlg = CTeachingAlgorithmTabDlg::GetInstance();
	HideAlgorithmDlg();

	m_pAutoFocusDlg = CAutoFocusDlg::GetInstance();
	HideAutoFocusDlg();

	m_pInspectLibraryDlg = CInspectLibraryDlg::GetInstance();
	HideInspectLibraryDlg();

	m_pGrabSequenceViewDlg = CGrabSequenceViewDlg::GetInstance();
	HideGrabSequenceViewDlg();

	m_pMotionControlDlg = CMotionControlDlg::GetInstance();
	HideMotionControlDlg();

	m_pCameraLightManagerDlg = CCameraLightManagerDlg::GetInstance();
	HideCameraLightManagerDlg();

	m_nScrollPos = 0;
	m_TabControl.ModifyStyle(0, TCS_OWNERDRAWFIXED | TCS_MULTILINE | TCS_FIXEDWIDTH | TCS_BUTTONS | TCS_FLATBUTTONS, SWP_FRAMECHANGED);
	InitFonts();

	ChangeLanguage();

	// m_TabControl.SetItemSize(CSize(85, 22));
	
	UpdateTabDisplay();
	TabPositionFitting(TABS_PER_LINE, 22);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTabControlDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_TabControl.DeleteAllItems();
	m_arrAllTabs.clear();

	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		CString strTemp;
		for (int i = 0; i <= MAX_IMAGE_TAB; i++)
		{
			if (i == 0)
			{
				m_TabControl.InsertItem(i, "기본정보 ");
				m_arrAllTabs.push_back("기본정보 ");
			}
			else
			{
				strTemp.Format("영상 %d ", i);
				m_TabControl.InsertItem(i, strTemp);
				m_arrAllTabs.push_back(strTemp);
			}
		}
	}
	else
	{
		CString strTemp;
		for (int i = 0; i <= MAX_IMAGE_TAB; i++)
		{
			if (i == 0)
			{
				m_TabControl.InsertItem(i, "Info ");
				m_arrAllTabs.push_back("Info ");
			}
			else
			{
				strTemp.Format("Img %d", i);
				m_TabControl.InsertItem(i, strTemp);
				m_arrAllTabs.push_back(strTemp);
			}
		}
	}

	m_TabControl.SetCurSel(m_iCurrentTab);
}

void CTabControlDlg::OnBnClickedBtnPrev()
{
	if (m_nScrollPos >= TABS_PER_LINE)
	{
		m_nScrollPos -= TABS_PER_LINE;
		UpdateTabDisplay();
	}
}

void CTabControlDlg::OnBnClickedBtnNext()
{
	if (m_nScrollPos + VISIBLE_TABS < (int)m_arrAllTabs.size())
	{
		m_nScrollPos += TABS_PER_LINE;
		UpdateTabDisplay();
	}
}

void CTabControlDlg::UpdateTabDisplay()
{
	while (m_TabControl.GetItemCount() > 0)
	{
		m_TabControl.DeleteItem(0);
	}

	int nStartIdx = m_nScrollPos;
	int nEndIdx = min(nStartIdx + VISIBLE_TABS, (int)m_arrAllTabs.size());

	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT | TCIF_PARAM;

	int nOriginalIndex = m_iCurrentTab;
	int nNewSelIndex = -1;

	for (int i = nStartIdx; i < nEndIdx; i++)
	{
		tcItem.pszText = m_arrAllTabs[i].GetBuffer();
		tcItem.lParam = i;

		int nInsertedIndex = m_TabControl.InsertItem(i - nStartIdx, &tcItem);
		m_arrAllTabs[i].ReleaseBuffer();

		if (i == nOriginalIndex)
		{
			nNewSelIndex = nInsertedIndex;
		}
	}

	if (nNewSelIndex >= 0)
	{
		m_TabControl.SetCurSel(nNewSelIndex);
	}
	else if (m_TabControl.GetItemCount() > 0)
	{
		m_TabControl.SetCurSel(-1);
	}

	m_btnPrev.EnableWindow(m_nScrollPos > 0);
	m_btnNext.EnableWindow(m_nScrollPos + VISIBLE_TABS < (int)m_arrAllTabs.size());

	m_TabControl.Invalidate();
	m_TabControl.UpdateWindow();
}

void CTabControlDlg::TabPositionFitting(int iTabsPerLine, int iTabHeight)
{
	if (iTabsPerLine <= 0)
		return;
	if (m_TabControl.GetItemCount() <= 0)
		return;

	CRect rcTabDialog;
	m_TabControl.GetClientRect(&rcTabDialog);
	int iTabDialogWidth = rcTabDialog.Width();
	if (iTabDialogWidth <= 0)
		return;

	m_TabControl.SendMessage(TCM_SETPADDING, 0, MAKELPARAM(0, 0));

	int iGap = 0;
	if (m_TabControl.GetItemCount() >= 2)
	{
		CRect rcTab0, rcTab1;
		m_TabControl.GetItemRect(0, &rcTab0);
		m_TabControl.GetItemRect(1, &rcTab1);

		if (rcTab0.top == rcTab1.top)
			iGap = max(0, rcTab1.left - rcTab0.right);
	}

	int iEachTabWidth = (iTabDialogWidth - (iTabsPerLine - 1) * iGap) / iTabsPerLine;
	iEachTabWidth = max(50, iEachTabWidth);

	m_TabControl.SetItemSize(CSize(iEachTabWidth, iTabHeight));
	m_TabControl.Invalidate();
}

void CTabControlDlg::InitFonts()
{
	// 일반 폰트
	m_fontNormal.CreateFont(
		14, 0, 0, 0, FW_NORMAL,
		FALSE, FALSE, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial")
	);

	// 굵은 폰트
	m_fontBold.CreateFont(
		14, 0, 0, 0, FW_BOLD,
		FALSE, FALSE, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial")
	);
}

void CTabControlDlg::ResetTabPosition()
{
	m_nScrollPos = 0;
	int nOriginalIndex = 0;

	while (m_TabControl.GetItemCount() > 0)
	{
		m_TabControl.DeleteItem(0);
	}

	int nStartIdx = m_nScrollPos;
	int nEndIdx = min(nStartIdx + VISIBLE_TABS, (int)m_arrAllTabs.size());

	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT | TCIF_PARAM;

	int nNewSelIndex = -1;

	for (int i = nStartIdx; i < nEndIdx; i++)
	{
		tcItem.pszText = m_arrAllTabs[i].GetBuffer();
		tcItem.lParam = i;

		int nInsertedIndex = m_TabControl.InsertItem(i - nStartIdx, &tcItem);
		m_arrAllTabs[i].ReleaseBuffer();

		if (i == nOriginalIndex)
		{
			nNewSelIndex = nInsertedIndex;
		}
	}

	if (nNewSelIndex >= 0)
	{
		m_TabControl.SetCurSel(nNewSelIndex);
	}
	else if (m_TabControl.GetItemCount() > 0)
	{
		m_TabControl.SetCurSel(-1);
	}

	m_btnPrev.EnableWindow(m_nScrollPos > 0);
	m_btnNext.EnableWindow(m_nScrollPos + VISIBLE_TABS < (int)m_arrAllTabs.size());

	m_TabControl.Invalidate();
}

void CTabControlDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC__TEACHING_TAB)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CRect rect = lpDrawItemStruct->rcItem;
		int nTabIndex = lpDrawItemStruct->itemID;

		BOOL bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED);

		TCITEM tcItem;
		TCHAR szText[256];
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = szText;
		tcItem.cchTextMax = 256;
		m_TabControl.GetItem(nTabIndex, &tcItem);

		CString strCurrentTabName;
		if (THEAPP.m_iPGMLanguageSelect == 0)
		{
			if (m_iCurrentTab == 0)
				strCurrentTabName.Format("기본정보 ");
			else
				strCurrentTabName.Format("영상 %d ", m_iCurrentTab);
		}
		else
		{
			if (m_iCurrentTab == 0)
				strCurrentTabName.Format("Info ");
			else
				strCurrentTabName.Format("Img %d ", m_iCurrentTab);
		}

		if (bSelected && (szText == strCurrentTabName))
			pDC->FillSolidRect(&rect, RGB(0x86, 0xAC, 0xE3));
		else
			pDC->FillSolidRect(&rect, RGB(240, 240, 240));

		pDC->SetBkMode(TRANSPARENT);

		if (bSelected && (szText == strCurrentTabName))
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			CFont* pOldFont = pDC->SelectObject(&m_fontBold);
			pDC->DrawText(szText, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pDC->SelectObject(pOldFont);
		}
		else
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			CFont* pOldFont = pDC->SelectObject(&m_fontNormal);
			pDC->DrawText(szText, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pDC->SelectObject(pOldFont);
		}

		return;
	}

	__super::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CTabControlDlg::ShowFirstPage(BOOL bReadTeachingImage)
{
	try {
		// Align Image Loading (1st Tab)

		CString strModelName, strFolder, strImageFolder, strImageName;

		strModelName = THEAPP.m_pModelDataManager->m_sModelName;
		strFolder = THEAPP.GetWorkingDirectory() + "\\Model\\";

		strImageFolder.Format("%s%s\\SW\\Vision_N%d\\TeachImage\\", strFolder, strModelName, THEAPP.m_iCurTeachVision + 1);

		THEAPP.m_FileBase.CreateFolder(strImageFolder);

		BOOL bMatchingSuccess = FALSE;
		BOOL bGlobalAlignExist = FALSE;

		int iMatchingFailImageIndex = -1;
		double dRotationAngleDeg = 0.0;
		double dDeltaX = 0.0;
		double dDeltaY = 0.0;

		int i, j, k;
		HObject HGrayImage, HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS, HTempImageI, HTempColorImage;
		HTuple HNoChannel;

		int iProcessChType = CHANNEL_TYPE_I;

		if (bReadTeachingImage && THEAPP.m_bShowReviewWindow && THEAPP.m_bReviewImageClick)	// Review Images are copied to teaching images
		{
			for (k = 0; k < MAX_IMAGE_TAB; k++)
			{
				for (i = 0; i < CHANNEL_NUM; i++)
					GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][k]));
			}

			BOOL bResizeImage = FALSE;

			if (THEAPP.m_iCurTeachVision == VISION_NUMBER_1 || THEAPP.m_iCurTeachVision == VISION_NUMBER_2 || THEAPP.m_iCurTeachVision == VISION_NUMBER_3 || THEAPP.m_iCurTeachVision == VISION_NUMBER_4)
			{
				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[0]))
				{
					HTuple htImageHeight = 0;
					HTuple htImageWidth = 0;
					HalconCpp::GetImageSize(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[0], &htImageWidth, &htImageHeight);

					if (htImageWidth.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[THEAPP.m_iCurVisionCamType] || htImageHeight.L() != THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[THEAPP.m_iCurVisionCamType])
						bResizeImage = TRUE;
				}
			}

			if (THEAPP.m_iCurTeachVision == VISION_NUMBER_1 || THEAPP.m_iCurTeachVision == VISION_NUMBER_2 || THEAPP.m_iCurTeachVision == VISION_NUMBER_3 || THEAPP.m_iCurTeachVision == VISION_NUMBER_4)
			{
				for (i = 0; i < THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage; i++)
				{
					if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i]))
					{
						if (bResizeImage)
							ZoomImageSize(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]), THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[THEAPP.m_iCurVisionCamType], THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[THEAPP.m_iCurVisionCamType], "bicubic");
						else
							CopyImage(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]));
					}
				}
			}
			else
			{
				for (i = 0; i < THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage; i++)
				{
					if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i]))
					{
						if ((i % 2) == 0)
							CopyImage(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]));
						else
						{
							if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage + i / 2]))
								CopyImage(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage + i / 2], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]));
						}
					}
				}
			}

			if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][0]) == TRUE)
			{
				for (i = 0; i < THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage; i++)
				{
					if (THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i] == TRUE)
					{
						bGlobalAlignExist = TRUE;

						iProcessChType = THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[i];

						if (iProcessChType == CHANNEL_TYPE_I)
							Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HGrayImage);
						else if (iProcessChType == CHANNEL_TYPE_R)
							Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HGrayImage, &HTempImageG, &HTempImageB);
						else if (iProcessChType == CHANNEL_TYPE_G)
							Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HTempImageR, &HGrayImage, &HTempImageB);
						else if (iProcessChType == CHANNEL_TYPE_B)
							Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HTempImageR, &HTempImageG, &HGrayImage);
						else if (iProcessChType == CHANNEL_TYPE_H)
						{
							Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HTempImageR, &HTempImageG, &HTempImageB);
							TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HGrayImage, &HTempImageS, &HTempImageI, "hsi");
						}
						else if (iProcessChType == CHANNEL_TYPE_S)
						{
							Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HTempImageR, &HTempImageG, &HTempImageB);
							TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HGrayImage, &HTempImageI, "hsi");
						}
						else if (iProcessChType == CHANNEL_TYPE_COLOR)
						{
							CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HGrayImage);
						}

						if (THEAPP.m_pModelDataManager->m_bUseMatchingFilter[i])
						{
							THEAPP.m_pAlgorithm->GetMatchingPreprocessImage(HGrayImage, THEAPP.m_pModelDataManager->m_iMatchingFilterType[i], THEAPP.m_pModelDataManager->m_dMatchingFilterTypeXSize[i], THEAPP.m_pModelDataManager->m_dMatchingFilterTypeYSize[i], &HGrayImage);
						}

						bMatchingSuccess = THEAPP.m_pAlgorithm->ImageAlignShapeMatching(
							TRUE,
							TEACHING_MZ_NO,
							HGrayImage,
							THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR],
							&(THEAPP.m_pModelDataManager->m_lLAlignModelID[i]),
							THEAPP.m_pModelDataManager->m_bLocalAlignImage[i],
							THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[i],
							THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[i],
							THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[i],
							THEAPP.m_pModelDataManager->m_dLocalMatchingScore[i],
							THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[i],
							THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[i],
							THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[i],
							THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTX[i],
							THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTY[i],
							THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBX[i],
							THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBY[i],
							THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[i],
							THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[i],
							THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[i],
							THEAPP.m_iCurTeachVision,
							THEAPP.m_iCurVisionCamType,
							0,
							i,
							THEAPP.m_pModelDataManager->m_bCheckUseMatchingXYComp[i],
							THEAPP.m_pModelDataManager->m_iLocalIndexNumber[i],
							THEAPP.m_pModelDataManager->m_iRadioMatchingXYRefLine[i],
							THEAPP.m_pModelDataManager->m_bUseMatchingAngleComp[i],
							THEAPP.m_pModelDataManager->m_iMatchingAngleRefLine[i],
							&dRotationAngleDeg,
							&dDeltaX,
							&dDeltaY);

						if (bMatchingSuccess == FALSE)
						{
							iMatchingFailImageIndex = i;
							break;
						}
					}
				}	// for (i=0; i<THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage; i++)

				if (bGlobalAlignExist)
				{
					if (bMatchingSuccess)
					{
						for (i = 0; i < THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage; i++)
						{
							if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]))
							{
								CountChannels(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HNoChannel);

								if (HNoChannel == 3)
								{
									Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HTempImageR, &HTempImageG, &HTempImageB);
									CopyImage(HTempImageR, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i]));
									CopyImage(HTempImageG, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i]));
									CopyImage(HTempImageB, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i]));
									TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]), "hsi");
								}
								else
								{
									CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i]));
									CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i]));
									CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i]));
									Compose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]));
									TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]), "hsi");
								}
							}
						}

						strLog.Format("Teaching mode, Global align matching success");
						THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));
					}
					else
					{
						for (i = 0; i < THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage; i++)
						{
							if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]))
							{
								CountChannels(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HNoChannel);

								if (HNoChannel == 3)
								{
									Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HTempImageR, &HTempImageG, &HTempImageB);
									CopyImage(HTempImageR, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i]));
									CopyImage(HTempImageG, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i]));
									CopyImage(HTempImageB, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i]));
									TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]), "hsi");
								}
								else if (HNoChannel == 1)
								{
									CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i]));
									CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i]));
									CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i]));
									Compose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]));
									TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]), "hsi");
								}
							}
						}

						strLog.Format("Teaching mode, Global align matching fail, Image: %d", iMatchingFailImageIndex + 1);
						THEAPP.m_log_inspection->warn("{}", LOG_CSTR(strLog));

						if (THEAPP.m_iPGMLanguageSelect == 0)
							strMessageBox.Format("매칭 실패: 영상 탭 %d번", iMatchingFailImageIndex + 1);
						else
							strMessageBox.Format("Matching fail: Image tab No.%d", iMatchingFailImageIndex + 1);
						AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);
					}
				}
				else
				{
					for (i = 0; i < THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage; i++)
					{
						if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]))
						{
							CountChannels(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HNoChannel);

							if (HNoChannel == 3)
							{
								Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HTempImageR, &HTempImageG, &HTempImageB);
								CopyImage(HTempImageR, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i]));
								CopyImage(HTempImageG, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i]));
								CopyImage(HTempImageB, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i]));
								TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]), "hsi");
							}
							else if (HNoChannel == 1)
							{
								CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i]));
								CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i]));
								CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i]));
								Compose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]));
								TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]), "hsi");
							}
						}
					}
				}
			}
			else
			{
				bMatchingSuccess = FALSE;
				strLog.Format("Teaching mode, Global align matching skip");
				THEAPP.m_log_inspection->debug("{}", LOG_CSTR(strLog));
			}

			CString TeachImageFileName, strTeachFileFullName;

			if ((bGlobalAlignExist && bMatchingSuccess) || bGlobalAlignExist == FALSE)	// 매칭 성공 시에만 티칭 이미지 파일로 저장
			{
				for (i = 0; i < THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage; i++)
				{
					if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]) == FALSE)
						continue;

					TeachImageFileName.Format("Image_%d", i + 1);
					strTeachFileFullName = strImageFolder + "\\" + TeachImageFileName;

					HTuple HImageType;
					GetImageType(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HImageType);

					if (HImageType == "int4")
						WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i], "ima", 0, HTuple(strTeachFileFullName));
					else
					{
						if (!THEAPP.Struct_PreferenceStruct.m_bRawImageType)
							WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], "bmp", 0, HTuple(strTeachFileFullName));
						else
							WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], "jpeg 100", 0, HTuple(strTeachFileFullName));
					}
				}
			}
		}

		if (bReadTeachingImage && THEAPP.m_bShowReviewWindow == FALSE)	// 리뷰창 없는 상태에서 티칭모드 진입
		{
			for (k = 0; k < CHANNEL_NUM; k++)
			{
				for (i = 0; i < MAX_IMAGE_TAB; i++)
					GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[k][i]));
			}

			HANDLE				hFindFile;
			WIN32_FIND_DATA		FindFileData;
			CString sImageFilePath;

			if (THEAPP.m_pModelDataManager->m_sModelName != ".")
			{
				for (i = 0; i < MAX_IMAGE_TAB; i++)
				{
					strImageName.Format("Image_%d.jpg", i + 1);
					sImageFilePath = strImageFolder + strImageName;

					if ((hFindFile = FindFirstFile(sImageFilePath, &FindFileData)) != INVALID_HANDLE_VALUE)
					{
						ReadImage(&(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]), HTuple(sImageFilePath));

						CountChannels(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HNoChannel);

						if (HNoChannel == 3)
						{
							Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &HTempImageR, &HTempImageG, &HTempImageB);
							CopyImage(HTempImageR, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i]));
							CopyImage(HTempImageG, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i]));
							CopyImage(HTempImageB, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i]));

							TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][i]), &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]), "hsi");
						}
						else if (HNoChannel == 1)
						{
							CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]));
						}

						FindClose(hFindFile);
					}

					strImageName.Format("Image_%d.ima", i + 1);
					sImageFilePath = strImageFolder + strImageName;

					if ((hFindFile = FindFirstFile(sImageFilePath, &FindFileData)) != INVALID_HANDLE_VALUE)
					{
						HObject HTempColorImage;

						ReadImage(&(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i]), HTuple(sImageFilePath));
						CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i]));
						CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i]));
						CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i]));
						CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_H][i]));
						CopyImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_S][i]));
						Compose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_R][i],
							THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_G][i],
							THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_B][i],
							&HTempColorImage);
						CopyImage(HTempColorImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][i]));

						FindClose(hFindFile);
					}
				}
			}
		}

		int iCurDisplayChannelType;
		iCurDisplayChannelType = THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType();

		m_pThresholdHistogramDlg->ShowWindow(SW_HIDE);

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iCurDisplayChannelType][0]))
		{
			if (iCurDisplayChannelType == CHANNEL_TYPE_COLOR)
			{
				Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][0], &HTempImageR, &HTempImageG, &HTempImageB);
				TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");

				m_pThresholdHistogramDlg->SetImage(HGrayImage);	// 1번 영상
			}
			else
				m_pThresholdHistogramDlg->SetImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iCurDisplayChannelType][0]);	// 1번 영상
		}

		m_pThresholdHistogramDlg->UpdateBar(0, 1);
		m_pLightControlDlg->ShowWindow(SW_HIDE);
		m_pJogSetDlg->Show();
		m_pInspectionConditionTabDlg->ShowWindow(SW_HIDE);
		//m_pExtraDlg->Show();
		m_iCurrentTab = 0;

		m_TabControl.SetCurSel(0);

		ResetTabPosition();

		THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();
		UpdateData(FALSE);
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::ShowFirstPage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CTabControlDlg::HideAllPage()
{
	m_iCurrentTab = 0;

	m_pThresholdHistogramDlg->ShowWindow(SW_HIDE);
	m_pLightControlDlg->ShowWindow(SW_HIDE);
	m_pJogSetDlg->ShowWindow(SW_HIDE);
	m_pInspectionConditionTabDlg->ShowWindow(SW_HIDE);
	m_pExtraDlg->ShowWindow(SW_HIDE);

	m_pTeachParamDlg->ShowWindow(SW_HIDE);
}

void CTabControlDlg::OnSelchangeTeachingTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nSel = m_TabControl.GetCurSel();

	if (nSel < 0)
	{
		*pResult = 0;
		return;
	}

	TCITEM tcItem;
	tcItem.mask = TCIF_PARAM;

	int TabIndex = 0;

	if (m_TabControl.GetItem(nSel, &tcItem))
	{
		TabIndex = (int)tcItem.lParam;
	}
	else
	{
		*pResult = 0;
		return;
	}

	//int TabIndex = m_TabControl.GetCurSel();

	//////////////////////////////////////////////////////////////////////////
	// Teaching View Clear
	GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_pThresRgn));
	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();

	THEAPP.m_pInspectAdminViewDlg->SaveInspectTabOverLastSelectRegion();

	THEAPP.m_pInspectAdminViewDlg->ClearActiveTRegion();
	THEAPP.m_pInspectAdminViewDlg->ClearAllSelectRegion();
	THEAPP.m_pInspectAdminViewDlg->ClearAllLastSelectRegion();
	THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion = NULL;

	THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();
	//////////////////////////////////////////////////////////////////////////

	if (m_iCurrentTab == 0)	// 이전 탭상태 (0=매칭이면 1st Tab의 모든 데이터 업데이트)
	{
		m_pJogSetDlg->UpdateViewParam();
		m_pExtraDlg->UpdateViewParam();
	}

	m_pThresholdHistogramDlg->UpdateViewParam();

	m_iCurrentTab = TabIndex;

	if (TabIndex == 0)
		ShowFirstPage(FALSE);
	else
	{
		m_pInspectionConditionTabDlg->Show();

		m_pTeachParamDlg->Show();
		m_pTeachParamDlg->UpdateInspectCheckStatus();

		m_pJogSetDlg->ShowWindow(SW_HIDE);
		m_pExtraDlg->ShowWindow(SW_HIDE);

		ReadTeachingImage(TabIndex - 1);

		m_pLightControlDlg->ShowWindow(SW_SHOW);
		m_pLightControlDlg->UpdateLightValue(m_iCurrentTab);

		m_pThresholdHistogramDlg->Show();
	}

	*pResult = 0;
}

void CTabControlDlg::ReadTeachingImage(int iTeachingImageTabIndex)
{
	try
	{
		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return;

		int iCurDisplayChannelType;
		iCurDisplayChannelType = THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType();

		HObject HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS;
		HObject HGrayImage;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iCurDisplayChannelType][iTeachingImageTabIndex]))
		{
			if (iCurDisplayChannelType == CHANNEL_TYPE_COLOR)
			{
				Decompose3(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iTeachingImageTabIndex], &HTempImageR, &HTempImageG, &HTempImageB);
				TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HTempImageS, &HGrayImage, "hsi");

				m_pThresholdHistogramDlg->SetImage(HGrayImage);
			}
			else
				m_pThresholdHistogramDlg->SetImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iCurDisplayChannelType][iTeachingImageTabIndex]);

			m_pThresholdHistogramDlg->UpdateBar(0, 1);
		}

		THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();
	}
	catch (HException &except)
	{
		THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();
	}
}

void CTabControlDlg::LoadModelConditionParam()
{
	CString sTemp;
	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		for (int j = 0; j < MAX_DEFECT_NAME; j++)
		{
			sTemp.Format("%.4f", THEAPP.m_StructCountPerAreaConditionInfo.m_dMinArea[i][j]); m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickSetText(2, j, sTemp);
			sTemp.Format("%.4f", THEAPP.m_StructCountPerAreaConditionInfo.m_dMidArea[i][j]); m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickSetText(3, j, sTemp);
			sTemp.Format("%.4f", THEAPP.m_StructCountPerAreaConditionInfo.m_dMaxArea[i][j]); m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickSetText(4, j, sTemp);
			sTemp.Format("%d", THEAPP.m_StructCountPerAreaConditionInfo.m_iMinNumAccept[i][j]); m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickSetText(5, j, sTemp);
			sTemp.Format("%d", THEAPP.m_StructCountPerAreaConditionInfo.m_iMidNumAccept[i][j]); m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickSetText(6, j, sTemp);
			sTemp.Format("%d", THEAPP.m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[i][j]); m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickSetText(7, j, sTemp);
		}
	}
}

BOOL CTabControlDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CTabControlDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_pTeachingAlgorithmTabDlg->DeleteInstance();
	m_pTeachingAlgorithmTabDlg = NULL;

	m_pAutoFocusDlg->DeleteInstance();
	m_pAutoFocusDlg = NULL;

	m_pInspectLibraryDlg->DeleteInstance();
	m_pInspectLibraryDlg = NULL;

	m_pGrabSequenceViewDlg->DeleteInstance();
	m_pGrabSequenceViewDlg = NULL;

	m_pMotionControlDlg->DeleteInstance();
	m_pMotionControlDlg = NULL;

	m_pCameraLightManagerDlg->DeleteInstance();
	m_pCameraLightManagerDlg = NULL;
}

void CTabControlDlg::ShowAlgorithmDlg()
{
	m_pTeachingAlgorithmTabDlg->Show();
}

void CTabControlDlg::HideAlgorithmDlg()
{
	if (m_pTeachingAlgorithmTabDlg->m_hWnd)
	{
		m_pTeachingAlgorithmTabDlg->Hide();
	}
}

void CTabControlDlg::ShowAutoFocusDlg()
{
	m_pAutoFocusDlg->Show();
}

void CTabControlDlg::HideAutoFocusDlg()
{
	if (m_pAutoFocusDlg->m_hWnd)
	{
		m_pAutoFocusDlg->Hide();
	}
}

void CTabControlDlg::ShowInspectLibraryDlg()
{
	m_pInspectLibraryDlg->Show();
}

void CTabControlDlg::HideInspectLibraryDlg()
{
	if (m_pInspectLibraryDlg->m_hWnd)
	{
		m_pInspectLibraryDlg->Hide();
	}
}

void CTabControlDlg::ShowGrabSequenceViewDlg()
{
	m_pGrabSequenceViewDlg->Show();
}

void CTabControlDlg::HideGrabSequenceViewDlg()
{
	if (m_pGrabSequenceViewDlg->m_hWnd)
	{
		m_pGrabSequenceViewDlg->Hide();
	}
}

void CTabControlDlg::ShowMotionControlDlg()
{
	m_pMotionControlDlg->Show();
}

void CTabControlDlg::HideMotionControlDlg()
{
	if (m_pMotionControlDlg->m_hWnd)
	{
		m_pMotionControlDlg->Hide();
	}
}

void CTabControlDlg::ShowCameraLightManagerDlg()
{
	m_pCameraLightManagerDlg->LoadViewParam();
	m_pCameraLightManagerDlg->Show();
}

void CTabControlDlg::HideCameraLightManagerDlg()
{
	if (m_pCameraLightManagerDlg->m_hWnd)
	{
		m_pCameraLightManagerDlg->Hide();
	}
}

void CTabControlDlg::ShowAlgorithmWnd(int iImageType, GTRegion *pROIRegion, int iInspectionTabIndex)
{
	try
	{
		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return;

		// Update View Image Channel
		THEAPP.m_pInspectAdminViewToolbarDlg->SetDisplayChannel(pROIRegion->m_AlgorithmParam[iInspectionTabIndex].m_iProcessChType);

		m_pTeachingAlgorithmTabDlg->SetSelectedInspection(iImageType, pROIRegion, iInspectionTabIndex);
		m_pTeachingAlgorithmTabDlg->SetParam();

		ShowAlgorithmDlg();
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [TabControlDlg::ShowAlgorithmWnd] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}

void CTabControlDlg::UpdateList(int iImageType, int iInspectionTabIndex)
{
	m_pTeachParamDlg->UpdateUsedAlgorithmList(iInspectionTabIndex);
}

void CTabControlDlg::GetROIUpdate(int iTabIdx, BOOL bCheckArea1, int iArea1Contour1Margin, int iArea1Contour2Margin, BOOL bCheckArea2, int iArea2Contour1Margin, int iArea2Contour2Margin, BOOL bCheckArea3, int iArea3Contour1Margin, int iArea3Contour2Margin)
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
		return;

	UpdateData(TRUE);

	CAlgorithmParam AlgorithmParam;
	AlgorithmParam = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iTabIdx];

	AlgorithmParam.m_bUseInspectArea[0] = bCheckArea1;
	AlgorithmParam.m_iInspectAreaContour1Margin[0] = iArea1Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Margin[0] = iArea1Contour2Margin;
	AlgorithmParam.m_bUseInspectArea[1] = bCheckArea2;
	AlgorithmParam.m_iInspectAreaContour1Margin[1] = iArea2Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Margin[1] = iArea2Contour2Margin;
	AlgorithmParam.m_bUseInspectArea[2] = bCheckArea3;
	AlgorithmParam.m_iInspectAreaContour1Margin[2] = iArea3Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Margin[2] = iArea3Contour2Margin;

	if (AlgorithmParam.m_bUseImageProcessLocalAlign || AlgorithmParam.m_bUseImageProcessGenerate || AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
		AlgorithmPreProcessing();

	HObject HROIHRegion;
	HROIHRegion = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->GetROIHRegion(THEAPP.m_pCalDataService);

	if (AlgorithmParam.m_bUseImageProcessLocalAlign)
		THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIHRegion, TEACHING_THREAD_INDEX);

	if (AlgorithmParam.m_bUseImageProcessGenerate)
		THEAPP.m_pAlgorithm->ApplyGenerateResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIHRegion, TEACHING_THREAD_INDEX);

	if (AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
		THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIHRegion, TEACHING_THREAD_INDEX);

	THEAPP.m_pAlgorithm->GetInspectArea(HROIHRegion, AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn));

	if (0)
	{
		WriteObject(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn, "c:\\DualTest\\InspectROIUpdate");
	}

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}

void CTabControlDlg::TeachAlgorithmTest_Tab(int iImageType, int iAlgorithmTab)
{
	try
	{
		BOOL bDebugSave = FALSE;

		int iVisionNo = THEAPP.m_iCurTeachVision;
		THEAPP.m_pAlgorithm->m_iPcVisionNo = iVisionNo;
		THEAPP.m_pAlgorithm->m_iVisionCamType = iVisionNo;

		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return;

		if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
			return;

		m_pTeachParamDlg->GetInspectParam(FALSE);

		AlgorithmPreProcessing();

		HObject HDefectAllRgn, HDefectRgn;

		GenEmptyObj(&HDefectAllRgn);
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAIDefectRgn));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCenterRgn));

		if (((THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType >= INSPECTION_TYPE_FIRST && THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType <= INSPECTION_TYPE_LAST) ||
			 (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) ||
			 (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST)) == FALSE)
		{
			THEAPP.m_pInspectAdminViewDlg->UpdateView();

			return;
		}

		THEAPP.m_pAlgorithm->InitGapResult();
		THEAPP.m_pAlgorithm->InitCornerResult();

		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HMaskRgn;
		GenEmptyObj(&HMaskRgn);

		CMeasureData MeasureData;
		CAlgorithmParam AlgorithmParam;

		HObject HPreProcessImage;
		int iImgCnt;
		BOOL bShapeImageFail;
		int iShapeRawImageIdx[NO_USED_SHAPE_RAWIMAGE];
		int iShapeImageCnt = 0;

		double dAlignMatchingScore;

		if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab].m_bInspect)
		{
			HROIHRegion = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->GetROIHRegion(THEAPP.m_pCalDataService);

			if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab].m_bUseImageProcessLocalAlign)
				THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab], &HROIHRegion, TEACHING_THREAD_INDEX);

			if (bDebugSave)
			{
				WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], "bmp", 0, "c:\\DualTest\\InspectImage");
				WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
			}

			if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab].m_bUseImageProcessGenerate)
				THEAPP.m_pAlgorithm->ApplyGenerateResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab], &HROIHRegion, TEACHING_THREAD_INDEX);

			if (bDebugSave)
			{
				WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], "bmp", 0, "c:\\DualTest\\InspectImage");
				WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
			}

			if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab].m_bUseImageProcessDontCare || THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab].m_bUseImageProcessDontCare2)
				THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab], &HROIHRegion, TEACHING_THREAD_INDEX);

			if (bDebugSave)
			{
				WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], "bmp", 0, "c:\\DualTest\\InspectImage");
				WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
			}

			if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab].m_bUseImageProcessMask || THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab].m_bUseImageProcessMask2)
				THEAPP.m_pAlgorithm->ApplyMaskResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab], &HMaskRgn, TEACHING_THREAD_INDEX);
			else
				GenEmptyObj(&HMaskRgn);

			AlgorithmParam = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[iAlgorithmTab];

			if (THEAPP.bImageCompareAutoTrain && !AlgorithmParam.m_bUseImageCompare)
				return;

			if (iAlgorithmTab == 0 && AlgorithmParam.m_bUseROIAlign && AlgorithmParam.m_bROIAlignMultiModuleInspect == FALSE)
			{
				HObject HAlignRgn;
				BOOL bROIAlignShiftResult = FALSE;

				if (AlgorithmParam.m_bUseIsoColorImage)
					HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iImageType], HROIHRegion, AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);
				else
					HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], HROIHRegion, AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);

				if (AlgorithmParam.m_bROIAlignInspectShift)
				{
					if (bROIAlignShiftResult)	// Shift NG
					{
						ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HROIHRegion, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
					}
				}
				if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
					HROIHRegion = HAlignRgn;
			}

			// 비등방
			if (iAlgorithmTab == 0 && AlgorithmParam.m_bUseROIAnisoAlign && AlgorithmParam.m_bROIAnisoAlignMultiModuleInspect == FALSE)
			{
				HObject HAlignRgn;
				BOOL bROIAlignShiftResult = FALSE;

				if (AlgorithmParam.m_bUseAnisoColorImage)
					HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iImageType], HROIHRegion, AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);
				else
					HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], HROIHRegion, AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);

				if (AlgorithmParam.m_bROIAnisoAlignInspectShift)
				{
					if (bROIAlignShiftResult)	// Shift NG
					{
						ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HROIHRegion, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
					}
				}
				if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
					HROIHRegion = HAlignRgn;
			}

			if (AlgorithmParam.m_bUseImageProcess || AlgorithmParam.m_bUseImageProcessFilter)
			{
				THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChType][iImageType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChType], HROIHRegion, AlgorithmParam, &HPreProcessImage);

				HDefectRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion, iAlgorithmTab, HPreProcessImage, HROIHRegion, AlgorithmParam, &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChTypeDC][iImageType], &HMaskRgn);
			}
			else
			{
				if (iAlgorithmTab == 6)
				{
					int lklk = 1;
					lklk++;
				}
				HDefectRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion, iAlgorithmTab, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChType][iImageType], HROIHRegion, AlgorithmParam, &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChTypeDC][iImageType], &HMaskRgn);
			}

			if (AlgorithmParam.m_bUseBarcode)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_StructModuleBarcodeResult[0][0][0].m_HBarcodeDetectRgn) == TRUE)
					THEAPP.m_pInspectAdminViewDlg->m_HBarcodeDetectRgn = THEAPP.m_StructModuleBarcodeResult[0][0][0].m_HBarcodeDetectRgn;
				else
					GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HBarcodeDetectRgn));
			}

			if (AlgorithmParam.m_bUseOCR)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pAlgorithm->m_HOCRDetectRgn) == TRUE)
					THEAPP.m_pInspectAdminViewDlg->m_HOCRDetectRgn = THEAPP.m_pAlgorithm->m_HOCRDetectRgn;
				else
					GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HOCRDetectRgn));
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				ConcatObj(HDefectAllRgn, HDefectRgn, &HDefectAllRgn);

			if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
				ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectAllRgn) == TRUE)
		{
			ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HDefectAllRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
			Union1(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
		}

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::TeachAlgorithmTest_Tab] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CTabControlDlg::TeachAlgorithmTest_ROI(int iImageType)
{
	try
	{
		BOOL bDebugSave = FALSE;

		int iVisionNo = THEAPP.m_iCurTeachVision;
		THEAPP.m_pAlgorithm->m_iPcVisionNo = iVisionNo;
		THEAPP.m_pAlgorithm->m_iVisionCamType = iVisionNo;

		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return;

		if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion == NULL)
			return;

		m_pTeachParamDlg->GetInspectParam(TRUE);

		AlgorithmPreProcessing();

		HObject HDefectAllRgn, HDefectRgn;

		GenEmptyObj(&HDefectAllRgn);
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAIDefectRgn));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCenterRgn));

		if (((THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType >= INSPECTION_TYPE_FIRST && THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType <= INSPECTION_TYPE_LAST) ||
			 (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) ||
			 (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST)) == FALSE)
		{
			THEAPP.m_pInspectAdminViewDlg->UpdateView();

			return;
		}

		THEAPP.m_pAlgorithm->InitGapResult();
		THEAPP.m_pAlgorithm->InitCornerResult();

		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HMaskRgn;
		GenEmptyObj(&HMaskRgn);

		CMeasureData MeasureData;
		CAlgorithmParam AlgorithmParam;

		HObject HPreProcessImage;
		int iImgCnt;
		BOOL bShapeImageFail;
		int iShapeRawImageIdx[NO_USED_SHAPE_RAWIMAGE];
		int iShapeImageCnt = 0;

		double dAlignMatchingScore;

		if (THEAPP.bImageCompareAutoTrain)
		{
			for (int j = 0; j <= MAX_INSPECTION_TAB; j++)
			{
				if (j == MAX_INSPECTION_TAB)
					return;

				AlgorithmParam = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[j];
				if (AlgorithmParam.m_bUseImageCompare)
					break;
			}
		}

		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			if (THEAPP.bImageCompareAutoTrain && !THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i].m_bUseImageCompare)
				continue;

			if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i].m_bInspect)
			{
				HROIHRegion = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->GetROIHRegion(THEAPP.m_pCalDataService);

				if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i].m_bUseImageProcessLocalAlign)
					THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i], &HROIHRegion, TEACHING_THREAD_INDEX);

				if (bDebugSave)
				{
					WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], "bmp", 0, "c:\\DualTest\\InspectImage");
					WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
				}

				if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i].m_bUseImageProcessGenerate)
					THEAPP.m_pAlgorithm->ApplyGenerateResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i], &HROIHRegion, TEACHING_THREAD_INDEX);

				if (bDebugSave)
				{
					WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], "bmp", 0, "c:\\DualTest\\InspectImage");
					WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
				}

				if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i].m_bUseImageProcessDontCare || THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i].m_bUseImageProcessDontCare2)
					THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i], &HROIHRegion, TEACHING_THREAD_INDEX);

				if (bDebugSave)
				{
					WriteImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], "bmp", 0, "c:\\DualTest\\InspectImage");
					WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
				}

				if (THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i].m_bUseImageProcessMask || THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i].m_bUseImageProcessMask2)
					THEAPP.m_pAlgorithm->ApplyMaskResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i], &HMaskRgn, TEACHING_THREAD_INDEX);
				else
					GenEmptyObj(&HMaskRgn);

				AlgorithmParam = THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_AlgorithmParam[i];

				if (i == 0 && AlgorithmParam.m_bUseROIAlign && AlgorithmParam.m_bROIAlignMultiModuleInspect == FALSE)
				{
					HObject HAlignRgn;
					BOOL bROIAlignShiftResult = FALSE;

					if (AlgorithmParam.m_bUseIsoColorImage)
						HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iImageType], HROIHRegion, AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);
					else
						HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], HROIHRegion, AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);

					if (AlgorithmParam.m_bROIAlignInspectShift)
					{
						if (bROIAlignShiftResult)	// Shift NG
						{
							ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HROIHRegion, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
						}
					}
					if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
						HROIHRegion = HAlignRgn;
				}

				// 비등방
				if (i == 0 && AlgorithmParam.m_bUseROIAnisoAlign && AlgorithmParam.m_bROIAnisoAlignMultiModuleInspect == FALSE)
				{
					HObject HAlignRgn;
					BOOL bROIAlignShiftResult = FALSE;

					if (AlgorithmParam.m_bUseAnisoColorImage)
						HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iImageType], HROIHRegion, AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);
					else
						HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], HROIHRegion, AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);

					if (AlgorithmParam.m_bROIAnisoAlignInspectShift)
					{
						if (bROIAlignShiftResult)	// Shift NG
						{
							ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HROIHRegion, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
						}
					}
					if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
						HROIHRegion = HAlignRgn;
				}

				if (AlgorithmParam.m_bUseImageProcess || AlgorithmParam.m_bUseImageProcessFilter)
				{
					THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChType][iImageType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChType], HROIHRegion, AlgorithmParam, &HPreProcessImage);

					HDefectRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion, i, HPreProcessImage, HROIHRegion, AlgorithmParam, &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChTypeDC][iImageType], &HMaskRgn);
				}
				else
				{
					if (i == 6)
					{
						int lklk = 1;
						lklk++;
					}
					HDefectRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion, i, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChType][iImageType], HROIHRegion, AlgorithmParam, &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[AlgorithmParam.m_iProcessChTypeDC][iImageType], &HMaskRgn);
				}

				if (AlgorithmParam.m_bUseBarcode)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_StructModuleBarcodeResult[0][0][0].m_HBarcodeDetectRgn) == TRUE)
						THEAPP.m_pInspectAdminViewDlg->m_HBarcodeDetectRgn = THEAPP.m_StructModuleBarcodeResult[0][0][0].m_HBarcodeDetectRgn;
					else
						GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HBarcodeDetectRgn));
				}

				if (AlgorithmParam.m_bUseOCR)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pAlgorithm->m_HOCRDetectRgn) == TRUE)
						THEAPP.m_pInspectAdminViewDlg->m_HOCRDetectRgn = THEAPP.m_pAlgorithm->m_HOCRDetectRgn;
					else
						GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HOCRDetectRgn));
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
					ConcatObj(HDefectAllRgn, HDefectRgn, &HDefectAllRgn);

				if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
					ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectAllRgn) == TRUE)
		{
			ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HDefectAllRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
			Union1(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
		}

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::TeachAlgorithmTest_ROI] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CTabControlDlg::TeachAlgorithmTest_Image(int iImageType)
{
	try
	{
		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return;

		if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
			return;

		int iVisionNo = THEAPP.m_iCurTeachVision;
		THEAPP.m_pAlgorithm->m_iPcVisionNo = iVisionNo;
		THEAPP.m_pAlgorithm->m_iVisionCamType = iVisionNo;

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return;

		m_pTeachParamDlg->GetInspectParam(TRUE);

		AlgorithmPreProcessing();

		if (THEAPP.m_pInspectAdminViewDlg->GetDisplayFAIMeasureValue() == TRUE)
			AlgorithmPreMeasurement();

		GTRegion *pInspectROIRgn;
		HObject HDefectAllRgn, HDefectRgn;
		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HMaskRgn;
		GenEmptyObj(&HMaskRgn);

		CMeasureData MeasureData;

		GenEmptyObj(&HDefectAllRgn);
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAIDefectRgn));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn));
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCenterRgn));

		THEAPP.m_pAlgorithm->InitGapResult();
		THEAPP.m_pAlgorithm->InitCornerResult();

		HObject HAlignRgn;

		double dAlignMatchingScore;

		HObject HPreProcessImage;
		int iImgCnt;
		BOOL bShapeImageFail;
		int iShapeRawImageIdx[NO_USED_SHAPE_RAWIMAGE];
		int iShapeImageCnt = 0;

		if (THEAPP.bImageCompareAutoTrain)
		{
			BOOL bImageCompareCheck = FALSE;
			for (int i = 0; i <= iNoInspectROI; i++)
			{
				if (i == iNoInspectROI)
					return;

				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(i);
				for (int j = 0; j < MAX_INSPECTION_TAB; j++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[j].m_bUseImageCompare)
					{
						bImageCompareCheck = TRUE;
						break;
					}

					if (bImageCompareCheck)
						break;
				}

				if (bImageCompareCheck)
					break;
			}
		}

		for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miTeachImageIndex != (iImageType + 1))
				continue;

			if (((pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST) ||
				 (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_AF_ALIGN && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) ||
				 (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_MEASURE_LAST)) == FALSE)
				continue;

			for (int i = 0; i < MAX_INSPECTION_TAB; i++)
			{
				if (THEAPP.bImageCompareAutoTrain && !pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageCompare)
					continue;

				if (pInspectROIRgn->m_AlgorithmParam[i].m_bInspect)
				{
					HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

					if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessLocalAlign)
						THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[i], &HROIHRegion, TEACHING_THREAD_INDEX);

					if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessGenerate)
						THEAPP.m_pAlgorithm->ApplyGenerateResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[i], &HROIHRegion, TEACHING_THREAD_INDEX);

					if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessDontCare2)
						THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[i], &HROIHRegion, TEACHING_THREAD_INDEX);

					if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessMask || pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessMask2)
						THEAPP.m_pAlgorithm->ApplyMaskResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[i], &HMaskRgn, TEACHING_THREAD_INDEX);
					else
						GenEmptyObj(&HMaskRgn);

					if (i == 0 && pInspectROIRgn->m_AlgorithmParam[i].m_bUseROIAlign && pInspectROIRgn->m_AlgorithmParam[i].m_bROIAlignMultiModuleInspect == FALSE)
					{
						BOOL bROIAlignShiftResult = FALSE;

						if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseIsoColorImage)
							HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iImageType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[i], &(pInspectROIRgn->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);
						else
							HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[i], &(pInspectROIRgn->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);

						if (pInspectROIRgn->m_AlgorithmParam[i].m_bROIAlignInspectShift)
						{
							if (bROIAlignShiftResult)	// Shift NG
							{
								ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HROIHRegion, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
							}
						}

						if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
							HROIHRegion = HAlignRgn;
					}

					// 비등방 CHL
					if (i == 0 && pInspectROIRgn->m_AlgorithmParam[i].m_bUseROIAnisoAlign && pInspectROIRgn->m_AlgorithmParam[i].m_bROIAnisoAlignMultiModuleInspect == FALSE)
					{
						BOOL bROIAlignShiftResult = FALSE;

						if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseAnisoColorImage)
							HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iImageType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[i], &(pInspectROIRgn->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);
						else
							HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[i], &(pInspectROIRgn->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);

						if (pInspectROIRgn->m_AlgorithmParam[i].m_bROIAnisoAlignInspectShift)
						{
							if (bROIAlignShiftResult)	// Shift NG
							{
								ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HROIHRegion, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
							}
						}

						if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
							HROIHRegion = HAlignRgn;
					}

					if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessFilter)
					{
						THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[i].m_iProcessChType][iImageType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[i].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[i], &HPreProcessImage);

						HDefectRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, i, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[i], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[i].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[i].m_iProcessChTypeDC][iImageType], &HMaskRgn);
					}
					else
					{
						HDefectRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, i, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[i].m_iProcessChType][iImageType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[i], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[i].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[i].m_iProcessChTypeDC][iImageType], &HMaskRgn);
					}

					if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseBarcode)
					{
						if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_StructModuleBarcodeResult[0][0][0].m_HBarcodeDetectRgn) == TRUE)
							THEAPP.m_pInspectAdminViewDlg->m_HBarcodeDetectRgn = THEAPP.m_StructModuleBarcodeResult[0][0][0].m_HBarcodeDetectRgn;
						else
							GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HBarcodeDetectRgn));
					}

					if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseOCR)
					{
						if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pAlgorithm->m_HOCRDetectRgn) == TRUE)
							THEAPP.m_pInspectAdminViewDlg->m_HOCRDetectRgn = THEAPP.m_pAlgorithm->m_HOCRDetectRgn;
						else
							GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HOCRDetectRgn));
					}

					if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
						ConcatObj(HDefectAllRgn, HDefectRgn, &HDefectAllRgn);

					if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
						ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
				}
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectAllRgn) == TRUE)
		{
			ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HDefectAllRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
			Union1(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
		}

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::TeachAlgorithmTest_Image] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CTabControlDlg::AlgorithmPreProcessing()
{
	try
	{
		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return;

		int iImageIdx, iROIIndex, iTabIdx;
		GTRegion *pInspectROIRgn;
		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HAlignRgn;
		HObject HDontCareResultRgn, HGenerateResultRgn, HMaskResultRgn;
		HObject HPreProcessImage;

		// Local Align Preprocessing

		BOOL bTempROIAlignShiftResult = FALSE;
		int iLoaclAlignDeltaX, iLoaclAlignDeltaY;
		double dLoaclAlignDeltaAngle, dLoaclAlignDeltaAngleFixedPointX, dLoaclAlignDeltaAngleFixedPointY;
		iLoaclAlignDeltaX = iLoaclAlignDeltaY = INVALID_ALIGN_RESULT;
		dLoaclAlignDeltaAngle = dLoaclAlignDeltaAngleFixedPointX = dLoaclAlignDeltaAngleFixedPointY = INVALID_ALIGN_RESULT;

		double dAlignMatchingScore;

		int iMatchingPosOffsetX, iMatchingPosOffsetY;
		iMatchingPosOffsetX = iMatchingPosOffsetY = 0;

		for (iImageIdx = 1; iImageIdx <= MAX_IMAGE_TAB; iImageIdx++)
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign == FALSE)
					{
						pInspectROIRgn->ResetLocalAlignResult(TEACHING_THREAD_INDEX);

						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

						// ROI Align
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseROIAlign && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAlignLocalAlignUse && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAlignMultiModuleInspect == FALSE)
						{
							iLoaclAlignDeltaX = iLoaclAlignDeltaY = INVALID_ALIGN_RESULT;
							dLoaclAlignDeltaAngle = dLoaclAlignDeltaAngleFixedPointX = dLoaclAlignDeltaAngleFixedPointY = INVALID_ALIGN_RESULT;

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseIsoColorImage) // Color Image 사용
								HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &(pInspectROIRgn->m_HROIAlignModelID), &bTempROIAlignShiftResult, &dAlignMatchingScore, &iLoaclAlignDeltaX, &iLoaclAlignDeltaY, &dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY);
							else
								HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &(pInspectROIRgn->m_HROIAlignModelID), &bTempROIAlignShiftResult, &dAlignMatchingScore, &iLoaclAlignDeltaX, &iLoaclAlignDeltaY, &dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY);

							if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
							{
								pInspectROIRgn->m_dLocalAlignMatchingScore[TEACHING_THREAD_INDEX] = dAlignMatchingScore * 100.0;
								pInspectROIRgn->m_iLocalAlignDeltaX[TEACHING_THREAD_INDEX] = iLoaclAlignDeltaX;
								pInspectROIRgn->m_iLocalAlignDeltaY[TEACHING_THREAD_INDEX] = iLoaclAlignDeltaY;
								pInspectROIRgn->m_dLocalAlignDeltaAngle[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngle;
								pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngleFixedPointX;
								pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngleFixedPointY;
								pInspectROIRgn->m_HLocalAlignShapeRgn[TEACHING_THREAD_INDEX] = HAlignRgn;
							}
						}

						//비등방
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseROIAnisoAlign && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAnisoAlignLocalAlignUse && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAnisoAlignMultiModuleInspect == FALSE)
						{
							iLoaclAlignDeltaX = iLoaclAlignDeltaY = INVALID_ALIGN_RESULT;
							dLoaclAlignDeltaAngle = dLoaclAlignDeltaAngleFixedPointX = dLoaclAlignDeltaAngleFixedPointY = INVALID_ALIGN_RESULT;

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseAnisoColorImage)
								HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &(pInspectROIRgn->m_HROIAlignModelID), &bTempROIAlignShiftResult, &dAlignMatchingScore, &iLoaclAlignDeltaX, &iLoaclAlignDeltaY, &dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY);
							else
								HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &(pInspectROIRgn->m_HROIAlignModelID), &bTempROIAlignShiftResult, &dAlignMatchingScore, &iLoaclAlignDeltaX, &iLoaclAlignDeltaY, &dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY);

							if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
							{
								pInspectROIRgn->m_dLocalAlignMatchingScore[TEACHING_THREAD_INDEX] = dAlignMatchingScore * 100.0;
								pInspectROIRgn->m_iLocalAlignDeltaX[TEACHING_THREAD_INDEX] = iLoaclAlignDeltaX;
								pInspectROIRgn->m_iLocalAlignDeltaY[TEACHING_THREAD_INDEX] = iLoaclAlignDeltaY;
								pInspectROIRgn->m_dLocalAlignDeltaAngle[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngle;
								pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngleFixedPointX;
								pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngleFixedPointY;
								pInspectROIRgn->m_HLocalAlignShapeRgn[TEACHING_THREAD_INDEX] = HAlignRgn;
							}
						}

						// Template
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUsePartCheck && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bPartCheckLocalAlignUse)
						{
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
							{
								THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
								THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
							else
							{
								THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
						}

						break;
					}
				}
			}
		}

		for (iImageIdx = 1; iImageIdx <= MAX_IMAGE_TAB; iImageIdx++)
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
					{
						pInspectROIRgn->ResetLocalAlignResult(TEACHING_THREAD_INDEX);

						// ROI Align
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseROIAlign && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAlignLocalAlignUse && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAlignMultiModuleInspect == FALSE)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

							THEAPP.m_pAlgorithm->GetPreLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX, &iMatchingPosOffsetX, &iMatchingPosOffsetY);

							iLoaclAlignDeltaX = iLoaclAlignDeltaY = INVALID_ALIGN_RESULT;
							dLoaclAlignDeltaAngle = dLoaclAlignDeltaAngleFixedPointX = dLoaclAlignDeltaAngleFixedPointY = INVALID_ALIGN_RESULT;

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseIsoColorImage) // Color Image 사용
								HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &(pInspectROIRgn->m_HROIAlignModelID), &bTempROIAlignShiftResult, &dAlignMatchingScore, &iLoaclAlignDeltaX, &iLoaclAlignDeltaY, &dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY, iMatchingPosOffsetX, iMatchingPosOffsetY);
							else
								HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &(pInspectROIRgn->m_HROIAlignModelID), &bTempROIAlignShiftResult, &dAlignMatchingScore, &iLoaclAlignDeltaX, &iLoaclAlignDeltaY, &dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY, iMatchingPosOffsetX, iMatchingPosOffsetY);

							if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
							{
								pInspectROIRgn->m_dLocalAlignMatchingScore[TEACHING_THREAD_INDEX] = dAlignMatchingScore * 100.0;
								pInspectROIRgn->m_iLocalAlignDeltaX[TEACHING_THREAD_INDEX] = iLoaclAlignDeltaX;
								pInspectROIRgn->m_iLocalAlignDeltaY[TEACHING_THREAD_INDEX] = iLoaclAlignDeltaY;
								pInspectROIRgn->m_dLocalAlignDeltaAngle[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngle;
								pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngleFixedPointX;
								pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngleFixedPointY;
								pInspectROIRgn->m_HLocalAlignShapeRgn[TEACHING_THREAD_INDEX] = HAlignRgn;
							}
						}

						//비등방
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseROIAnisoAlign && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAnisoAlignLocalAlignUse && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAnisoAlignMultiModuleInspect == FALSE)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

							THEAPP.m_pAlgorithm->GetPreLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX, &iMatchingPosOffsetX, &iMatchingPosOffsetY);

							iLoaclAlignDeltaX = iLoaclAlignDeltaY = INVALID_ALIGN_RESULT;
							dLoaclAlignDeltaAngle = dLoaclAlignDeltaAngleFixedPointX = dLoaclAlignDeltaAngleFixedPointY = INVALID_ALIGN_RESULT;

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseAnisoColorImage)
								HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_COLOR][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &(pInspectROIRgn->m_HROIAlignModelID), &bTempROIAlignShiftResult, &dAlignMatchingScore, &iLoaclAlignDeltaX, &iLoaclAlignDeltaY, &dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY, iMatchingPosOffsetX, iMatchingPosOffsetY);
							else
								HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &(pInspectROIRgn->m_HROIAlignModelID), &bTempROIAlignShiftResult, &dAlignMatchingScore, &iLoaclAlignDeltaX, &iLoaclAlignDeltaY, &dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY, iMatchingPosOffsetX, iMatchingPosOffsetY);

							if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
							{
								pInspectROIRgn->m_dLocalAlignMatchingScore[TEACHING_THREAD_INDEX] = dAlignMatchingScore * 100.0;
								pInspectROIRgn->m_iLocalAlignDeltaX[TEACHING_THREAD_INDEX] = iLoaclAlignDeltaX;
								pInspectROIRgn->m_iLocalAlignDeltaY[TEACHING_THREAD_INDEX] = iLoaclAlignDeltaY;
								pInspectROIRgn->m_dLocalAlignDeltaAngle[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngle;
								pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngleFixedPointX;
								pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[TEACHING_THREAD_INDEX] = dLoaclAlignDeltaAngleFixedPointY;
								pInspectROIRgn->m_HLocalAlignShapeRgn[TEACHING_THREAD_INDEX] = HAlignRgn;
							}
						}

						// Template
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUsePartCheck && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bPartCheckLocalAlignUse)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

							THEAPP.m_pAlgorithm->GetPreLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX, &iMatchingPosOffsetX, &iMatchingPosOffsetY);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
							{
								THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
								THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
							else
							{
								THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
						}

						break;
					}
				}
			}
		}

		// Fitting
		for (iImageIdx = 1; iImageIdx <= MAX_IMAGE_TAB; iImageIdx++)
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					{
						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

						// ROI Align
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseROIAlign && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAlignMultiModuleInspect == FALSE)
							continue;

						//비등방
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseROIAnisoAlign && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAnisoAlignMultiModuleInspect == FALSE)
							continue;

						// Template
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUsePartCheck && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bPartCheckLocalAlignUse)
							continue;

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign == TRUE)
						{
							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlignAddLineFit == FALSE)
								THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX);
						}

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
						{
							THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
							THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1], NULL, NULL, iMatchingPosOffsetX, iMatchingPosOffsetY);
						}
						else
						{
							THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1], NULL, NULL, iMatchingPosOffsetX, iMatchingPosOffsetY);
						}

						break;
					}
				}
			}
		}

		// Fitting (Add)
		for (iImageIdx = 1; iImageIdx <= MAX_IMAGE_TAB; iImageIdx++)
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					{
						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

						// ROI Align
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseROIAlign && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAlignMultiModuleInspect == FALSE)
							continue;

						//비등방
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseROIAnisoAlign && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bROIAnisoAlignMultiModuleInspect == FALSE)
							continue;

						// Template
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUsePartCheck && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bPartCheckLocalAlignUse)
							continue;

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign == TRUE && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlignAddLineFit == TRUE)
						{
							THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX);
						}
						else
							continue;

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
						{
							THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
							THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}
						else
						{
							THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}

						break;
					}
				}
			}
		}

		// DontCare Preprocessing

		for (iImageIdx = 1; iImageIdx <= MAX_IMAGE_TAB; iImageIdx++)
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_DONTCARE)
					continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					{
						pInspectROIRgn->ResetDontCareResult(TEACHING_THREAD_INDEX);

						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
							THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
						{
							THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
							HDontCareResultRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}
						else
						{
							HDontCareResultRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}

						if (THEAPP.m_pGFunction->ValidHRegion(HDontCareResultRgn) == TRUE)
						{
							pInspectROIRgn->m_HDontCareResultRgn[TEACHING_THREAD_INDEX] = HDontCareResultRgn;
						}

						break;
					}
				}
			}
		}

		// Generate Preprocessing

		for (iImageIdx = 1; iImageIdx <= MAX_IMAGE_TAB; iImageIdx++)
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_GENERATE)
					continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					{
						pInspectROIRgn->ResetGenerateResult(TEACHING_THREAD_INDEX);

						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
							THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
							THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
						{
							THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
							HGenerateResultRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}
						else
						{
							HGenerateResultRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}

						if (THEAPP.m_pGFunction->ValidHRegion(HGenerateResultRgn) == TRUE)
							pInspectROIRgn->m_HGenerateResultRgn[TEACHING_THREAD_INDEX] = HGenerateResultRgn;
						else
							GenEmptyObj(&(pInspectROIRgn->m_HGenerateResultRgn[TEACHING_THREAD_INDEX]));


						break;
					}
				}
			}
		}

		// Mask Preprocessing

		for (iImageIdx = 1; iImageIdx <= MAX_IMAGE_TAB; iImageIdx++)
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_MASK)
					continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					{
						pInspectROIRgn->ResetMaskResult(TEACHING_THREAD_INDEX);

						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
							THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
							THEAPP.m_pAlgorithm->ApplyGenerateResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
							THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, TEACHING_THREAD_INDEX);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
						{
							THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
							HMaskResultRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}
						else
						{
							HMaskResultRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}


						if (THEAPP.m_pGFunction->ValidHRegion(HMaskResultRgn) == TRUE)
							pInspectROIRgn->m_HMaskResultRgn[TEACHING_THREAD_INDEX] = HMaskResultRgn;
						else
							GenEmptyObj(&(pInspectROIRgn->m_HMaskResultRgn[TEACHING_THREAD_INDEX]));

						break;
					}
				}
			}
		}

	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::AlgorithmPreProcessing] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

#if defined (UAVI_AKC) || defined (UAVI_BOI) || defined (UAVI_ACT) || defined (UAVI_ATW) || defined (UAVI_BOS) || defined (UAVI_RENO) || defined (UAVI_KRIOS) || defined (SINGLE_LENS) || defined (ASSY_LENS)
void CTabControlDlg::AlgorithmPreMeasurement()
{
	try
	{
		int i;
		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return;

		int iImageIdx, iROIIndex, iTabIdx;
		GTRegion *pInspectROIRgn;
		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HAlignRgn;
		HObject HPreProcessImage;

		BOOL bMeasureROIFound;
		HObject HMeasureRetRgn;
		int iMzNo, iTrayNo, iModuleNo, iCurInspectionBufferIdx, iPcVisionNo, iVisionCamType, iNoInspectImage, iCurThreadIdx;

		HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
		double dMeasureLineStartX, dMeasureLineStartY, dMeasureLineEndX, dMeasureLineEndY;
		HTuple HlNoFoundNumber;
		Hlong lNoFoundNumber;
		double dIntersectionPosY;
		int iFAINumber, iMeasureCount;

		iVisionCamType = THEAPP.m_iCurVisionCamType;
		iCurInspectionBufferIdx = TEACHING_INSPECT_BUFFER_INDEX;
		iMzNo = TEACHING_MZ_NO;
		iTrayNo = TEACHING_TRAY_NO;
		if (THEAPP.m_iTeachingModuleNo > 0)
			iModuleNo = THEAPP.m_iTeachingModuleNo;
		else
			iModuleNo = TEACHING_MODULE_NO;
		iPcVisionNo = THEAPP.m_iCurTeachVision;
		iNoInspectImage = MAX_IMAGE_TAB;
		iCurThreadIdx = TEACHING_THREAD_INDEX;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		Algorithm* pAlgorithm = THEAPP.m_pAlgorithm;

		CFAIDataManager::GetInstance().ResetTeachingMeasure();

		double dModuleRotationAngleDeg, dAngleCompMeasure;
		dModuleRotationAngleDeg = 0;

		HTuple HRoiArea, HRoiCenterX, HRoiCenterY;
		DPOINT dRoiCenterPoint;

		if (iVisionCamType == VISION_NUMBER_1 || iVisionCamType == VISION_NUMBER_2 || iVisionCamType == VISION_NUMBER_3 || iVisionCamType == VISION_NUMBER_4)
		{
			for (iImageIdx = 1; iImageIdx <= iNoInspectImage; iImageIdx++)	// 영상 1~티칭영상수, Align Tab 제외
			{
				for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
				{
					pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

					if (pInspectROIRgn == NULL)
						continue;

					if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
						continue;

					if (pInspectROIRgn->miInspectionType < INSPECTION_TYPE_MEASURE_FIRST || pInspectROIRgn->miInspectionType > INSPECTION_TYPE_MEASURE_LAST)
						continue;

					for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
								pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
								pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
								pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
							{
								pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
								HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
							else
							{
								HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}

							if (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
							{
								if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_MEASURE_POINT)	// 측정 Point
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureCPoint[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureCPoint[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;

									// 측정 Contour
									if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
									{
										ConcatObj(CFAIDataManager::GetInstance().GetTeachingMeasure().m_HMeasureXLD[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID], HResultXLD, &(CFAIDataManager::GetInstance().GetTeachingMeasure().m_HMeasureXLD[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID]));
									}

									// 측정 Region
									if (THEAPP.m_pGFunction->ValidHRegion(HMeasureRetRgn))
									{
										ConcatObj(CFAIDataManager::GetInstance().GetTeachingMeasure().m_HMeasureRgn[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID], HMeasureRetRgn, &(CFAIDataManager::GetInstance().GetTeachingMeasure().m_HMeasureRgn[pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID]));
									}
								}
								else if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_BLEND_POINT)	// 합성 Point
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dBlendCPoint[iPcVisionNo][pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dBlendCPoint[iPcVisionNo][pInspectROIRgn->miPositionID][pInspectROIRgn->miPointID].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
							}
						}
					}
				}
			}
		}

		// FAI 치수 측정
		pAlgorithm->InspectFAI(FALSE);

	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::AlgorithmPreMeasurement] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}
#endif

#if defined (UAVI_CHS_KS) || defined (UAVI_CHS_TV)
void CTabControlDlg::AlgorithmPreMeasurement()
{
	try
	{
		int i;
		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return;

		int iImageIdx, iROIIndex, iTabIdx;
		GTRegion *pInspectROIRgn;
		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HAlignRgn;
		HObject HPreProcessImage;

		BOOL bMeasureROIFound;
		HObject HMeasureRetRgn;
		int iMzNo, iTrayNo, iModuleNo, iCurInspectionBufferIdx, iPcVisionNo, iVisionCamType, iNoInspectImage, iCurThreadIdx;

		HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
		double dMeasureLineStartX, dMeasureLineStartY, dMeasureLineEndX, dMeasureLineEndY;
		HTuple HlNoFoundNumber;
		Hlong lNoFoundNumber;
		double dIntersectionPosY;
		int iFAINumber, iMeasureCount;

		iVisionCamType = THEAPP.m_iCurVisionCamType;
		iCurInspectionBufferIdx = TEACHING_INSPECT_BUFFER_INDEX;
		iMzNo = TEACHING_MZ_NO;
		iTrayNo = TEACHING_TRAY_NO;
		if (THEAPP.m_iTeachingModuleNo > 0)
			iModuleNo = THEAPP.m_iTeachingModuleNo;
		else
			iModuleNo = TEACHING_MODULE_NO;
		iPcVisionNo = THEAPP.m_iCurTeachVision;
		iNoInspectImage = MAX_IMAGE_TAB;
		iCurThreadIdx = TEACHING_THREAD_INDEX;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		Algorithm* pAlgorithm = THEAPP.m_pAlgorithm;

		THEAPP.m_pFAIDataManager.ResetTeachingMeasure();

		double dModuleRotationAngleDeg, dAngleCompMeasure;
		dModuleRotationAngleDeg = 0;

		HTuple HRoiArea, HRoiCenterX, HRoiCenterY;
		DPOINT dRoiCenterPoint;

		if (iVisionCamType == VISION_NUMBER_2)	// Top
		{
			for (iImageIdx = 1; iImageIdx <= iNoInspectImage; iImageIdx++)	// 영상 1~티칭영상수, Align Tab 제외
			{
				for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
				{
					pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

					if (pInspectROIRgn == NULL)
						continue;

					if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
						continue;

					if (((pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE2_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE2_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE3_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE3_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE4_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE4_P2)) == FALSE)
						continue;

					for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
								pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
								pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
								pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
							{
								pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
								HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
							else
							{
								HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}

							if (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
							{
								if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE4_P2)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
							}
						}
					}
				}
			}
		}

		if (iVisionCamType == VISION_NUMBER_1 || iVisionCamType == VISION_NUMBER_2 || iVisionCamType == VISION_NUMBER_3 || iVisionCamType == VISION_NUMBER_4)
		{
			for (iImageIdx = 1; iImageIdx <= iNoInspectImage; iImageIdx++)	// 영상 1~티칭영상수, Align Tab 제외
			{
				for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
				{
					pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

					if (pInspectROIRgn == NULL)
						continue;

					if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
						continue;

					if (((pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P5) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P6) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P7) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P8) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P5) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P6) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P7) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P8) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P5) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_C) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_C) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_24_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_24_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_25_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_25_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_25_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_19_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_19_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_19_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_19_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_19_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_FAI_19_P4)) == FALSE)
						continue;

					for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
								pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
								pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
								pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
							{
								pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
								HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
							else
							{
								HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}

							if (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
							{
								if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P8)
								{
									iFAINumber = 5;

									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P8)
								{
									iFAINumber = 6;

									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P2)
								{
									iFAINumber = 7;

									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P5)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_C)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_C)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P3)
								{
									iFAINumber = 10;

									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P3)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI18_NeckEpoxyPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI18_NeckEpoxyPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI23_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI23_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_24_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_24_P2)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_24_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_24_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_25_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_25_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_25_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_25_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_25_P2)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_1_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_1_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_1_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_1_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_2_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_2_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_2_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_2_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_3_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_3_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_3_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_3_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_19_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_19_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_19_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_19_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_FAI_19_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_FAI_19_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_19_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_FAI_19_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
							}
						}
					}
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Measurement
			double dDistance, dAngle, dMaxDistance, dAvgDistance;
			HTuple HDistance, HAngle;
			DPOINT dPoint1, dPoint2, dPoint3, dPoint4;
			DPOINT dLineStart, dLineEnd;
			DPOINT dLineStart2, dLineEnd2;
			DPOINT dMeasurePoint1, dMeasurePoint2;
			double dDistance1, dDistance2;
			double dDistanceArray[5];

			// Datum
			DPOINT dDatumBCornerPoint[4];
			HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
			DPOINT dLinePoint1, dLinePoint2, dLinePoint3, dLinePoint4, dIntersectPoint;

			for (i = 0; i < 8; i += 2)
			{
				dDatumBCornerPoint[i].Reset();

				dLinePoint1 = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[i % 8];
				dLinePoint2 = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[(i + 1) % 8];
				dLinePoint3 = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[(i + 2) % 8];
				dLinePoint4 = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[(i + 3) % 8];

				if (dLinePoint1.x >= 0 && dLinePoint2.x >= 0 && dLinePoint3.x >= 0 && dLinePoint4.x >= 0)
				{
					IntersectionLines(dLinePoint1.y, dLinePoint1.x, dLinePoint2.y, dLinePoint2.x, dLinePoint3.y, dLinePoint3.x, dLinePoint4.y, dLinePoint4.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);
					dIntersectPoint.x = HIntersectPointX[0].D();
					dIntersectPoint.y = HIntersectPointY[0].D();

					dDatumBCornerPoint[i / 2] = dIntersectPoint;
				}
			}

			dLinePoint1 = dDatumBCornerPoint[0];
			dLinePoint2 = dDatumBCornerPoint[2];
			dLinePoint3 = dDatumBCornerPoint[1];
			dLinePoint4 = dDatumBCornerPoint[3];

			if (dLinePoint1.x >= 0 && dLinePoint2.x >= 0 && dLinePoint3.x >= 0 && dLinePoint4.x >= 0)
			{
				IntersectionLines(dLinePoint1.y, dLinePoint1.x, dLinePoint2.y, dLinePoint2.x, dLinePoint3.y, dLinePoint3.x, dLinePoint4.y, dLinePoint4.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);
				dIntersectPoint.x = HIntersectPointX[0].D();
				dIntersectPoint.y = HIntersectPointY[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint = dIntersectPoint;
			}

			HTuple HInnerEdgeConcatX, HInnerEdgeConcatY;
			HTuple HOuterEdgeConcatX, HOuterEdgeConcatY;
			double dInnerNormalRadius, dOuterNormalRadius;
			int iNoInnerEdge, iNoOuterEdge;
			HTuple HNoCircleFitEdge;
			double dInnerCircleCenterX, dInnerCircleCenterY, dInnerCircleRadius;
			double dOuterCircleCenterX, dOuterCircleCenterY, dOuterCircleRadius;
			BOOL bInnerCircleFound, bOuterCircleFound;
			HObject HCircleXLD;

			bInnerCircleFound = bOuterCircleFound = FALSE;

			// FAI-5
			TupleGenConst(0, 0, &HOuterEdgeConcatX);
			TupleGenConst(0, 0, &HOuterEdgeConcatY);

			for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++)
			{
				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii].y;

				if (dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					TupleConcat(HOuterEdgeConcatX, HTuple(dPoint1.x), &HOuterEdgeConcatX);
					TupleConcat(HOuterEdgeConcatY, HTuple(dPoint1.y), &HOuterEdgeConcatY);
				}
			}

			TupleLength(HOuterEdgeConcatX, &HNoCircleFitEdge);
			iNoOuterEdge = HNoCircleFitEdge.L();

			if (iNoOuterEdge == MAX_FAI_CIRCLE_FIT_POINT)
			{
				dOuterNormalRadius = 0;
				for (int iii = 0; iii < (MAX_FAI_CIRCLE_FIT_POINT / 2); iii++)
				{
					dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii].x;
					dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii].y;

					dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
					dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

					DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
					dDistance = HDistance.D();
					dOuterNormalRadius += dDistance;
				}
				dOuterNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
				dOuterNormalRadius *= 0.5;

				bOuterCircleFound = pAlgorithm->LeastSquareCircleFitting(HOuterEdgeConcatX, HOuterEdgeConcatY, dOuterNormalRadius, &dOuterCircleCenterX, &dOuterCircleCenterY, &dOuterCircleRadius);
				if (bOuterCircleFound)
				{
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI5_LensBarrelTopEdgePoint.x = dOuterCircleCenterX;
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI5_LensBarrelTopEdgePoint.y = dOuterCircleCenterY;
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[5][0] = (dOuterCircleRadius * 2.0) * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[5];
				}
			}

			// FAI-6
			TupleGenConst(0, 0, &HInnerEdgeConcatX);
			TupleGenConst(0, 0, &HInnerEdgeConcatY);

			for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++)
			{
				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii].y;

				if (dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					TupleConcat(HInnerEdgeConcatX, HTuple(dPoint1.x), &HInnerEdgeConcatX);
					TupleConcat(HInnerEdgeConcatY, HTuple(dPoint1.y), &HInnerEdgeConcatY);
				}
			}

			TupleLength(HInnerEdgeConcatX, &HNoCircleFitEdge);
			iNoInnerEdge = HNoCircleFitEdge.L();

			if (iNoInnerEdge == MAX_FAI_CIRCLE_FIT_POINT)
			{
				dInnerNormalRadius = 0;
				for (int iii = 0; iii < (MAX_FAI_CIRCLE_FIT_POINT / 2); iii++)
				{
					dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii].x;
					dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii].y;

					dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
					dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

					DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
					dDistance = HDistance.D();
					dInnerNormalRadius += dDistance;
				}
				dInnerNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
				dInnerNormalRadius *= 0.5;

				bInnerCircleFound = pAlgorithm->LeastSquareCircleFitting(HInnerEdgeConcatX, HInnerEdgeConcatY, dInnerNormalRadius, &dInnerCircleCenterX, &dInnerCircleCenterY, &dInnerCircleRadius);
				if (bInnerCircleFound)
				{
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI6_LensInnerEdgePoint.x = dInnerCircleCenterX;
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI6_LensInnerEdgePoint.y = dInnerCircleCenterY;
				}
			}

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI6_LensInnerEdgePoint.x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI6_LensInnerEdgePoint.y;

			if (dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint2.x >= 0 && dPoint2.y >= 0)
			{
				DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance[0].D();
				dDistance *= 2.0;
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[6][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[6];
			}

			// FAI-7
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[1].y;

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_EastPoint[0].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_EastPoint[0].y;

			dDistance1 = -1;
			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance1 = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[7][1] = dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[7];
			}

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_EastPoint[1].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_EastPoint[1].y;

			dDistance2 = -1;
			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance2 = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[7][2] = dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[7];
			}

			dMaxDistance = -1;
			if (dDistance1 >= 0 && dDistance2 >= 0)
			{
				if (dDistance1 >= dDistance2)
					dMaxDistance = dDistance1;
				else
					dMaxDistance = dDistance2;
			}
			else if (dDistance1 >= 0 && dDistance2 < 0)
				dMaxDistance = dDistance1;
			else if (dDistance2 >= 0 && dDistance1 < 0)
				dMaxDistance = dDistance2;

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[7][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[7];

			// FAI-9
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[2].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[2].y;

			for (i = 0; i < 5; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[9][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[9];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 5; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[9][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[9];

			// FAI-10
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[2].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[2].y;

			for (i = 0; i < 3; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[10][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[10];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 3; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[10][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[10];

			// FAI-11
			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			for (i = 0; i < 5; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[i].y;

				if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					dDistanceArray[i] = fabs(dPoint2.x - dPoint1.x);

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[11][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[11];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 5; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[11][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[11];

			// FAI-12
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[2].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[2].y;

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[12][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[12];
			}

			// FAI-13
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[2].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[2].y;

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[13][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[13];
			}

			// FAI-18
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[1].y;

			for (i = 0; i < 3; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI18_NeckEpoxyPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI18_NeckEpoxyPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[18][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[18];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 3; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[18][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[18];

			// FAI-21
			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[0].x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[0].y;

			dPoint3.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[1].x;
			dPoint3.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[1].y;

			if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint3.x >= 0 && dPoint3.y >= 0)
			{
				dDistance = fabs((dPoint2.x + dPoint3.x) * 0.5 - dPoint1.x);

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[21][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[21];
			}

			// FAI-23
			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI23_FlexPoint[0].x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI23_FlexPoint[0].y;

			dPoint3.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI23_FlexPoint[1].x;
			dPoint3.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI23_FlexPoint[1].y;

			if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint3.x >= 0 && dPoint3.y >= 0)
			{
				dDistance = fabs((dPoint2.y + dPoint3.y) * 0.5 - dPoint1.y);

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[23][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[23];
			}

			// FAI-24
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[1].y;

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[24][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[24];
			}

			// FAI-25
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_NorthPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_NorthPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_NorthPoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_NorthPoint[1].y;

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_SouthPoint[0].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_SouthPoint[0].y;

			dDistance1 = -1;
			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance1 = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[25][1] = dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[25];
			}

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_SouthPoint[1].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_SouthPoint[1].y;

			dDistance2 = -1;
			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance2 = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[25][2] = dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[25];
			}

			dAvgDistance = -1;
			if (dDistance1 >= 0 && dDistance2 >= 0)
				dAvgDistance = (dDistance1 + dDistance2) * 0.5;
			else if (dDistance1 >= 0 && dDistance2 < 0)
				dAvgDistance = dDistance1;
			else if (dDistance2 >= 0 && dDistance1 < 0)
				dAvgDistance = dDistance2;

			if (dAvgDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[25][0] = dAvgDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[25];

			// D/C_2
			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_NorthPoint[0].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_NorthPoint[0].y;
			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_NorthPoint[1].x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_NorthPoint[1].y;
			dPoint3.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_SouthPoint[0].x;
			dPoint3.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_SouthPoint[0].y;
			dPoint4.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_SouthPoint[1].x;
			dPoint4.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_SouthPoint[1].y;

			if (dPoint1.x >= 0 && dPoint2.x >= 0 && dPoint3.x >= 0 && dPoint4.x >= 0)
			{
				DPOINT dM1Point, dM2Point;

				dM1Point.x = (dPoint1.x + dPoint3.x) * 0.5;
				dM1Point.y = (dPoint1.y + dPoint3.y) * 0.5;
				dM2Point.x = (dPoint2.x + dPoint4.x) * 0.5;
				dM2Point.y = (dPoint2.y + dPoint4.y) * 0.5;

				dLineStart.x = dM1Point.x;
				dLineStart.y = dM1Point.y;
				dLineEnd.x = dM2Point.x;
				dLineEnd.y = dM2Point.y;

				dLineStart2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[0].x;
				dLineStart2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[0].y;
				dLineEnd2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[2].x;
				dLineEnd2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[2].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineStart2.y >= 0)
				{
					AngleLl(dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HAngle);

					TupleDeg(HAngle, &HAngle);
					dAngle = HAngle[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[85][0] = dAngle + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[85];
				}
			}

			// FAI-27
			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[0].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[0].y;
			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[1].x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_WestPoint[1].y;
			dPoint3.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_EastPoint[0].x;
			dPoint3.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_EastPoint[0].y;
			dPoint4.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_EastPoint[1].x;
			dPoint4.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI7_EastPoint[1].y;

			if (dPoint1.x >= 0 && dPoint2.x >= 0 && dPoint3.x >= 0 && dPoint4.x >= 0)
			{
				DPOINT dM1Point, dM2Point;

				dM1Point.x = (dPoint1.x + dPoint3.x) * 0.5;
				dM1Point.y = (dPoint1.y + dPoint3.y) * 0.5;
				dM2Point.x = (dPoint2.x + dPoint4.x) * 0.5;
				dM2Point.y = (dPoint2.y + dPoint4.y) * 0.5;

				dLineStart.x = dM1Point.x;
				dLineStart.y = dM1Point.y;
				dLineEnd.x = dM2Point.x;
				dLineEnd.y = dM2Point.y;

				dLineStart2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[0].x;
				dLineStart2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[0].y;
				dLineEnd2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[1].x;
				dLineEnd2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[1].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineStart2.y >= 0)
				{
					AngleLl(dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HAngle);

					TupleDeg(HAngle, &HAngle);
					dAngle = HAngle[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[27][0] = dAngle + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[27];
				}
			}

			// FAI-16-1
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_1_LinePoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_1_LinePoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_1_LinePoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_1_LinePoint[1].y;

			for (i = 0; i < 4; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_1_TopPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_1_TopPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[86][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[86];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 4; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[86][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[86];

			// FAI-16-2
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_2_LinePoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_2_LinePoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_2_LinePoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_2_LinePoint[1].y;

			for (i = 0; i < 4; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_2_TopPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_2_TopPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[87][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[87];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 4; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[87][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[87];

			// FAI-16-3
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_3_LinePoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_3_LinePoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_3_LinePoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_3_LinePoint[1].y;

			for (i = 0; i < 4; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_3_TopPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI16_3_TopPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[88][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[88];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 4; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[88][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[88];

			// FAI-19
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[1].y;

			for (i = 0; i < 4; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_TopPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_TopPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[19][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[19];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 4; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[19][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[19];

		}	// if (iVisionCamType == VISION_NUMBER_1 || iVisionCamType == VISION_NUMBER_2 || iVisionCamType == VISION_NUMBER_3 || iVisionCamType == VISION_NUMBER_4)

	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::AlgorithmPreMeasurement] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}
#endif

#ifdef UAVI_CHS_WZ
void CTabControlDlg::AlgorithmPreMeasurement()
{
	try
	{
		int i;
		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return;

		int iImageIdx, iROIIndex, iTabIdx;
		GTRegion *pInspectROIRgn;
		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HAlignRgn;
		HObject HPreProcessImage;

		BOOL bMeasureROIFound;
		HObject HMeasureRetRgn;
		int iMzNo, iTrayNo, iModuleNo, iCurInspectionBufferIdx, iPcVisionNo, iVisionCamType, iNoInspectImage, iCurThreadIdx;

		HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
		double dMeasureLineStartX, dMeasureLineStartY, dMeasureLineEndX, dMeasureLineEndY;
		HTuple HlNoFoundNumber;
		Hlong lNoFoundNumber;
		double dIntersectionPosY;
		int iFAINumber, iMeasureCount;

		iVisionCamType = THEAPP.m_iCurVisionCamType;
		iCurInspectionBufferIdx = TEACHING_INSPECT_BUFFER_INDEX;
		iMzNo = TEACHING_MZ_NO;
		iTrayNo = TEACHING_TRAY_NO;
		if (THEAPP.m_iTeachingModuleNo > 0)
			iModuleNo = THEAPP.m_iTeachingModuleNo;
		else
			iModuleNo = TEACHING_MODULE_NO;
		iPcVisionNo = THEAPP.m_iCurTeachVision;
		iNoInspectImage = MAX_IMAGE_TAB;
		iCurThreadIdx = TEACHING_THREAD_INDEX;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		Algorithm* pAlgorithm = THEAPP.m_pAlgorithm;

		THEAPP.m_pFAIDataManager.ResetTeachingMeasure();

		double dModuleRotationAngleDeg, dAngleCompMeasure;
		dModuleRotationAngleDeg = 0;

		HTuple HRoiArea, HRoiCenterX, HRoiCenterY;
		DPOINT dRoiCenterPoint;

		if (iVisionCamType == VISION_NUMBER_2)	// Top
		{
			for (iImageIdx = 1; iImageIdx <= iNoInspectImage; iImageIdx++)	// 영상 1~티칭영상수, Align Tab 제외
			{
				for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
				{
					pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

					if (pInspectROIRgn == NULL)
						continue;

					if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
						continue;

					if (((pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE2_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE2_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE3_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE3_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE4_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE4_P2)) == FALSE)
						continue;

					for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
								pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
								pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
								pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
							{
								pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
								HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
							else
							{
								HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}

							if (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
							{
								if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE4_P2)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
							}
						}
					}
				}
			}
		}

		if (iVisionCamType == VISION_NUMBER_1 || iVisionCamType == VISION_NUMBER_2 || iVisionCamType == VISION_NUMBER_3 || iVisionCamType == VISION_NUMBER_4)
		{
			for (iImageIdx = 1; iImageIdx <= iNoInspectImage; iImageIdx++)	// 영상 1~티칭영상수, Align Tab 제외
			{
				for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
				{
					pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

					if (pInspectROIRgn == NULL)
						continue;

					if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
						continue;

					if (((pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P5) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P6) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P7) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P8) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P5) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P6) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P7) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P8) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_C) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_C) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_C) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P4) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_A) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_B) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P1) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P2) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P3) ||
						(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P4)) == FALSE)
						continue;

					for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
					{
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
						{
							HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
								pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
								pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
								pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

							if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
							{
								pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
								HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}
							else
							{
								HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
							}

							if (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
							{
								if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P8)
								{
									iFAINumber = 5;

									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P8)
								{
									iFAINumber = 6;

									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P2)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P3)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_C)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_C)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P2)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P2)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_NorthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P3 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P3].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_SouthPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P3].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_C)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI18_NeckEpoxyPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI18_NeckEpoxyPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P2)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P2)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI22_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI22_FlexPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_Point[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_1_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_1_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_1_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_1_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_2_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_2_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_2_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_2_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_3_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_3_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_3_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_3_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_A && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_B)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_A].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_A].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
								else if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P1 && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P4)
								{
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P1].x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
									CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_TopPoint[pInspectROIRgn->miInspectionType - INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P1].y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;
								}
							}
						}
					}
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Measurement
			double dDistance, dAngle, dMaxDistance, dAvgDistance;
			HTuple HDistance, HAngle;
			DPOINT dPoint1, dPoint2, dPoint3, dPoint4;
			DPOINT dLineStart, dLineEnd;
			DPOINT dLineStart2, dLineEnd2;
			DPOINT dMeasurePoint1, dMeasurePoint2;
			double dDistance1, dDistance2;
			double dDistanceArray[5];

			// Datum
			DPOINT dDatumBCornerPoint[4];
			HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
			DPOINT dLinePoint1, dLinePoint2, dLinePoint3, dLinePoint4, dIntersectPoint;

			for (i = 0; i < 8; i += 2)
			{
				dDatumBCornerPoint[i].Reset();

				dLinePoint1 = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[i % 8];
				dLinePoint2 = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[(i + 1) % 8];
				dLinePoint3 = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[(i + 2) % 8];
				dLinePoint4 = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBLinePoint[(i + 3) % 8];

				if (dLinePoint1.x >= 0 && dLinePoint2.x >= 0 && dLinePoint3.x >= 0 && dLinePoint4.x >= 0)
				{
					IntersectionLines(dLinePoint1.y, dLinePoint1.x, dLinePoint2.y, dLinePoint2.x, dLinePoint3.y, dLinePoint3.x, dLinePoint4.y, dLinePoint4.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);
					dIntersectPoint.x = HIntersectPointX[0].D();
					dIntersectPoint.y = HIntersectPointY[0].D();

					dDatumBCornerPoint[i / 2] = dIntersectPoint;
				}
			}

			dLinePoint1 = dDatumBCornerPoint[0];
			dLinePoint2 = dDatumBCornerPoint[2];
			dLinePoint3 = dDatumBCornerPoint[1];
			dLinePoint4 = dDatumBCornerPoint[3];

			if (dLinePoint1.x >= 0 && dLinePoint2.x >= 0 && dLinePoint3.x >= 0 && dLinePoint4.x >= 0)
			{
				IntersectionLines(dLinePoint1.y, dLinePoint1.x, dLinePoint2.y, dLinePoint2.x, dLinePoint3.y, dLinePoint3.x, dLinePoint4.y, dLinePoint4.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);
				dIntersectPoint.x = HIntersectPointX[0].D();
				dIntersectPoint.y = HIntersectPointY[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint = dIntersectPoint;
			}

			HTuple HInnerEdgeConcatX, HInnerEdgeConcatY;
			HTuple HOuterEdgeConcatX, HOuterEdgeConcatY;
			double dInnerNormalRadius, dOuterNormalRadius;
			int iNoInnerEdge, iNoOuterEdge;
			HTuple HNoCircleFitEdge;
			double dInnerCircleCenterX, dInnerCircleCenterY, dInnerCircleRadius;
			double dOuterCircleCenterX, dOuterCircleCenterY, dOuterCircleRadius;
			BOOL bInnerCircleFound, bOuterCircleFound;
			HObject HCircleXLD;

			bInnerCircleFound = bOuterCircleFound = FALSE;

			// FAI-5
			TupleGenConst(0, 0, &HOuterEdgeConcatX);
			TupleGenConst(0, 0, &HOuterEdgeConcatY);

			for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++)
			{
				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii].y;

				if (dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					TupleConcat(HOuterEdgeConcatX, HTuple(dPoint1.x), &HOuterEdgeConcatX);
					TupleConcat(HOuterEdgeConcatY, HTuple(dPoint1.y), &HOuterEdgeConcatY);
				}
			}

			TupleLength(HOuterEdgeConcatX, &HNoCircleFitEdge);
			iNoOuterEdge = HNoCircleFitEdge.L();

			if (iNoOuterEdge == MAX_FAI_CIRCLE_FIT_POINT)
			{
				dOuterNormalRadius = 0;
				for (int iii = 0; iii < (MAX_FAI_CIRCLE_FIT_POINT / 2); iii++)
				{
					dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii].x;
					dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii].y;

					dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
					dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI5_LensBarrelTopEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

					DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
					dDistance = HDistance.D();
					dOuterNormalRadius += dDistance;
				}
				dOuterNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
				dOuterNormalRadius *= 0.5;

				bOuterCircleFound = pAlgorithm->LeastSquareCircleFitting(HOuterEdgeConcatX, HOuterEdgeConcatY, dOuterNormalRadius, &dOuterCircleCenterX, &dOuterCircleCenterY, &dOuterCircleRadius);
				if (bOuterCircleFound)
				{
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI5_LensBarrelTopEdgePoint.x = dOuterCircleCenterX;
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI5_LensBarrelTopEdgePoint.y = dOuterCircleCenterY;
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[5][0] = (dOuterCircleRadius * 2.0) * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[5];
				}
			}

			// FAI-6
			TupleGenConst(0, 0, &HInnerEdgeConcatX);
			TupleGenConst(0, 0, &HInnerEdgeConcatY);

			for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++)
			{
				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii].y;

				if (dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					TupleConcat(HInnerEdgeConcatX, HTuple(dPoint1.x), &HInnerEdgeConcatX);
					TupleConcat(HInnerEdgeConcatY, HTuple(dPoint1.y), &HInnerEdgeConcatY);
				}
			}

			TupleLength(HInnerEdgeConcatX, &HNoCircleFitEdge);
			iNoInnerEdge = HNoCircleFitEdge.L();

			if (iNoInnerEdge == MAX_FAI_CIRCLE_FIT_POINT)
			{
				dInnerNormalRadius = 0;
				for (int iii = 0; iii < (MAX_FAI_CIRCLE_FIT_POINT / 2); iii++)
				{
					dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii].x;
					dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii].y;

					dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
					dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI6_LensInnerEdgePoint[iii + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

					DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
					dDistance = HDistance.D();
					dInnerNormalRadius += dDistance;
				}
				dInnerNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
				dInnerNormalRadius *= 0.5;

				bInnerCircleFound = pAlgorithm->LeastSquareCircleFitting(HInnerEdgeConcatX, HInnerEdgeConcatY, dInnerNormalRadius, &dInnerCircleCenterX, &dInnerCircleCenterY, &dInnerCircleRadius);
				if (bInnerCircleFound)
				{
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI6_LensInnerEdgePoint.x = dInnerCircleCenterX;
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI6_LensInnerEdgePoint.y = dInnerCircleCenterY;
				}
			}

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI6_LensInnerEdgePoint.x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dMeasureFAI6_LensInnerEdgePoint.y;

			if (dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint2.x >= 0 && dPoint2.y >= 0)
			{
				DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance[0].D();
				dDistance *= 2.0;
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[6][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[6];
			}

			// FAI-3
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_WestPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_WestPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_WestPoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_WestPoint[1].y;

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_EastPoint[0].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_EastPoint[0].y;

			dDistance1 = -1;
			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance1 = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[3][1] = dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[3];
			}

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_EastPoint[1].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_EastPoint[1].y;

			dDistance2 = -1;
			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance2 = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[3][2] = dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[3];
			}

			dAvgDistance = -1;
			if (dDistance1 >= 0 && dDistance2 >= 0)
				dAvgDistance = (dDistance1 + dDistance2) * 0.5;
			else if (dDistance1 >= 0 && dDistance2 < 0)
				dAvgDistance = dDistance1;
			else if (dDistance2 >= 0 && dDistance1 < 0)
				dAvgDistance = dDistance2;

			if (dAvgDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[3][0] = dAvgDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[3];

			// FAI-9
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[2].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[2].y;

			for (i = 0; i < 3; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[9][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[9];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 3; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[9][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[9];

			// FAI-10
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[2].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[2].y;

			for (i = 0; i < 2; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[10][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[10];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 2; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[10][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[10];

			// FAI-11
			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			for (i = 0; i < 3; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_WestPoint[i].y;

				if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					dDistanceArray[i] = fabs(dPoint2.x - dPoint1.x);

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[11][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[11];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 3; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[11][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[11];

			// FAI-12
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[2].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI9_EastPoint[2].y;

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[12][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[12];
			}

			// FAI-13
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[2].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_NorthPoint[2].y;

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance = HDistance[0].D();

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[13][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[13];
			}

			// FAI-14
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_NorthPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_NorthPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_NorthPoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_NorthPoint[1].y;

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_SouthPoint[0].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_SouthPoint[0].y;

			dDistance1 = -1;
			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance1 = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[14][1] = dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[14];
				else
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[14][1] = dDistance1 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[14];
			}

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_SouthPoint[1].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_SouthPoint[1].y;

			dDistance2 = -1;
			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance2 = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[14][2] = dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[14];
				else
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[14][2] = dDistance2 * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[14];
			}

			dAvgDistance = -1;
			if (dDistance1 >= 0 && dDistance2 >= 0)
				dAvgDistance = (dDistance1 + dDistance2) * 0.5;
			else if (dDistance1 >= 0 && dDistance2 < 0)
				dAvgDistance = dDistance1;
			else if (dDistance2 >= 0 && dDistance1 < 0)
				dAvgDistance = dDistance2;

			if (dAvgDistance >= 0)
			{
				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[14][0] = dAvgDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[14];
				else
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[14][0] = dAvgDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[14];
			}

			// FAI-18
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_SouthPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_SouthPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_SouthPoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI14_SouthPoint[1].y;

			for (i = 0; i < 3; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI18_NeckEpoxyPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI18_NeckEpoxyPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[18][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[18];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 3; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[18][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[18];

			// FAI-21
			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[0].x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[0].y;

			dPoint3.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[1].x;
			dPoint3.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI21_FlexPoint[1].y;

			if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint3.x >= 0 && dPoint3.y >= 0)
			{
				dDistance = fabs((dPoint2.x + dPoint3.x) * 0.5 - dPoint1.x);

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[21][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[21];
			}

			// FAI-22
			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI22_FlexPoint[0].x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI22_FlexPoint[0].y;

			dPoint3.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI22_FlexPoint[1].x;
			dPoint3.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI22_FlexPoint[1].y;

			if (dPoint2.x >= 0 && dPoint2.y >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0 && dPoint3.x >= 0 && dPoint3.y >= 0)
			{
				dDistance = fabs((dPoint2.y + dPoint3.y) * 0.5 - dPoint1.y);

				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[22][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[22];
			}

			// FAI-23
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI10_SouthPoint[1].y;

			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

			if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
				dDistance = HDistance[0].D();

				if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[23][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[23];
				else
					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[23][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[23];
			}

			// FAI-24
			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[0].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[0].y;
			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[1].x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[1].y;
			dPoint3.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[3].x;
			dPoint3.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[3].y;
			dPoint4.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[4].x;
			dPoint4.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI24_Point[4].y;

			if (dPoint1.x >= 0 && dPoint2.x >= 0 && dPoint3.x >= 0 && dPoint4.x >= 0)
			{
				DPOINT dM1Point, dM2Point;
				dM1Point.x = (dPoint1.x + dPoint2.x) * 0.5;
				dM1Point.y = (dPoint1.y + dPoint2.y) * 0.5;
				dM2Point.x = (dPoint3.x + dPoint4.x) * 0.5;
				dM2Point.y = (dPoint3.y + dPoint4.y) * 0.5;

				dLineStart.x = dM1Point.x;
				dLineStart.y = dM1Point.y;
				dLineEnd.x = dM2Point.x;
				dLineEnd.y = dM2Point.y;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistance = HDistance[0].D();

					if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
						CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[24][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[24];
					else
						CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[24][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[24];
				}
			}

			// FAI-25
			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_Point[0].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_Point[0].y;
			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_Point[1].x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_Point[1].y;
			dPoint3.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_Point[3].x;
			dPoint3.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_Point[3].y;
			dPoint4.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_Point[4].x;
			dPoint4.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI25_Point[4].y;

			if (dPoint1.x >= 0 && dPoint2.x >= 0 && dPoint3.x >= 0 && dPoint4.x >= 0)
			{
				DPOINT dM1Point, dM2Point;
				dM1Point.x = (dPoint1.x + dPoint2.x) * 0.5;
				dM1Point.y = (dPoint1.y + dPoint2.y) * 0.5;
				dM2Point.x = (dPoint3.x + dPoint4.x) * 0.5;
				dM2Point.y = (dPoint3.y + dPoint4.y) * 0.5;

				dLineStart.x = dM1Point.x;
				dLineStart.y = dM1Point.y;
				dLineEnd.x = dM2Point.x;
				dLineEnd.y = dM2Point.y;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dDatumBPoint.y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistance = HDistance[0].D();

					if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
						CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[25][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[25];
					else
						CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[25][0] = dDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[25];
				}
			}

			// D/C_2
			dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_WestPoint[0].x;
			dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_WestPoint[0].y;
			dPoint2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_WestPoint[1].x;
			dPoint2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_WestPoint[1].y;
			dPoint3.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_EastPoint[0].x;
			dPoint3.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_EastPoint[0].y;
			dPoint4.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_EastPoint[1].x;
			dPoint4.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI3_EastPoint[1].y;

			if (dPoint1.x >= 0 && dPoint2.x >= 0 && dPoint3.x >= 0 && dPoint4.x >= 0)
			{
				DPOINT dM1Point, dM2Point;

				dM1Point.x = (dPoint1.x + dPoint3.x) * 0.5;
				dM1Point.y = (dPoint1.y + dPoint3.y) * 0.5;
				dM2Point.x = (dPoint2.x + dPoint4.x) * 0.5;
				dM2Point.y = (dPoint2.y + dPoint4.y) * 0.5;

				dLineStart.x = dM1Point.x;
				dLineStart.y = dM1Point.y;
				dLineEnd.x = dM2Point.x;
				dLineEnd.y = dM2Point.y;

				dLineStart2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI22_FlexPoint[0].x;
				dLineStart2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI22_FlexPoint[0].y;
				dLineEnd2.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI22_FlexPoint[1].x;
				dLineEnd2.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI22_FlexPoint[1].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineStart2.y >= 0)
				{
					AngleLl(dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HAngle);

					TupleDeg(HAngle, &HAngle);
					dAngle = HAngle[0].D();

					if (THEAPP.m_pInspectService->m_iStageNo_H[iVisionCamType] == STAGE_NUMBER_1)
						CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[85][0] = dAngle + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[85];
					else
						CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[85][0] = dAngle + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg2Fai[85];
				}
			}

			// FAI-17-1
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_1_LinePoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_1_LinePoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_1_LinePoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_1_LinePoint[1].y;

			for (i = 0; i < 4; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_1_TopPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_1_TopPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[86][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[86];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 4; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[86][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[86];

			// FAI-17-2
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_2_LinePoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_2_LinePoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_2_LinePoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_2_LinePoint[1].y;

			for (i = 0; i < 4; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_2_TopPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_2_TopPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[87][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[87];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 4; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[87][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[87];

			// FAI-17-3
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_3_LinePoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_3_LinePoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_3_LinePoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_3_LinePoint[1].y;

			for (i = 0; i < 4; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_3_TopPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI17_3_TopPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[88][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[88];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 4; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[88][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[88];

			// FAI-19
			dLineStart.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[0].x;
			dLineStart.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[0].y;
			dLineEnd.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[1].x;
			dLineEnd.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_LinePoint[1].y;

			for (i = 0; i < 4; i++)
			{
				dDistanceArray[i] = -1;

				dPoint1.x = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_TopPoint[i].x;
				dPoint1.y = CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAI19_TopPoint[i].y;

				if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
				{
					DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
					dDistanceArray[i] = HDistance[0].D();

					CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[19][1 + i] = dDistanceArray[i] * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[19];
				}
			}

			dMaxDistance = -1;
			for (i = 0; i < 4; i++)
			{
				if (dDistanceArray[i] >= 0)
				{
					if (dDistanceArray[i] > dMaxDistance)
						dMaxDistance = dDistanceArray[i];
				}
			}

			if (dMaxDistance >= 0)
				CFAIDataManager::GetInstance().GetTeachingMeasure().m_dFAIMeasureValue[19][0] = dMaxDistance * THEAPP.m_pCalDataService_N[iVisionCamType]->GetPixelSize() * 0.001 + THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_dMOffsetStg1Fai[19];

		}	// if (iVisionCamType == VISION_NUMBER_1 || iVisionCamType == VISION_NUMBER_2 || iVisionCamType == VISION_NUMBER_3 || iVisionCamType == VISION_NUMBER_4)

	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::AlgorithmPreMeasurement] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}
#endif

int CTabControlDlg::GetAutoFocusLensRefHeight()
{
	try
	{
		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return HEIGHT_INIT;

		if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
			return HEIGHT_INIT;

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return HEIGHT_INIT;

		AlgorithmPreProcessing();

		int iImageIdx, iROIIndex, iTabIdx;
		GTRegion *pInspectROIRgn;
		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HAlignRgn;
		HObject HPreProcessImage;

		BOOL bMeasureROIFound;
		HObject HMeasureRetRgn;
		int iMzNo, iTrayNo, iModuleNo, iCurInspectionBufferIdx, iPcVisionNo, iVisionCamType, iNoInspectImage, iCurThreadIdx;

		iVisionCamType = THEAPP.m_iCurVisionCamType;
		iCurInspectionBufferIdx = TEACHING_INSPECT_BUFFER_INDEX;
		iMzNo = TEACHING_MZ_NO;
		iTrayNo = TEACHING_TRAY_NO;
		if (THEAPP.m_iTeachingModuleNo > 0)
			iModuleNo = THEAPP.m_iTeachingModuleNo;
		else
			iModuleNo = TEACHING_MODULE_NO;
		iPcVisionNo = THEAPP.m_iCurTeachVision;
		iNoInspectImage = MAX_IMAGE_TAB;
		iCurThreadIdx = TEACHING_THREAD_INDEX;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		Algorithm* pAlgorithm = THEAPP.m_pAlgorithm;

		int iRefHeight = HEIGHT_INIT;

		for (iImageIdx = 1; iImageIdx <= iNoInspectImage; iImageIdx++)	// 영상 1~티칭영상수, Align Tab 제외
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P3)
					continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					{
						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessLocalAlign)
							pAlgorithm->ApplyLocalAlignResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessGenerate)
							pAlgorithm->ApplyGenerateResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessDontCare2)
							pAlgorithm->ApplyDontCareResult(iMzNo, iPcVisionNo, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HROIHRegion, iCurThreadIdx);

						HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);

						if (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dHeightMm > HEIGHT_INIT)
						{
							iRefHeight = (int)(pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dHeightMm * 1000.0);
						}
					}
				}
			}
		}

		return iRefHeight;

	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::GetAutoFocusLensRefHeight] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return HEIGHT_INIT;
	}
}

POINT CTabControlDlg::GetAutoFocusLeftTopCornerPoint(double *pdRotationAngle)
{
	POINT LTCornerPoint;
	LTCornerPoint.x = -1;
	LTCornerPoint.y = -1;

	try
	{
		*pdRotationAngle = 0;

		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return LTCornerPoint;

		if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
			return LTCornerPoint;

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return LTCornerPoint;

		int iImageIdx, iROIIndex, iTabIdx;
		GTRegion *pInspectROIRgn;
		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HAlignRgn;
		HObject HPreProcessImage;

		BOOL bMeasureROIFound;
		HObject HMeasureRetRgn;
		int iMzNo, iTrayNo, iModuleNo, iCurInspectionBufferIdx, iPcVisionNo, iVisionCamType, iNoInspectImage, iCurThreadIdx;

		iVisionCamType = THEAPP.m_iCurVisionCamType;
		iCurInspectionBufferIdx = TEACHING_INSPECT_BUFFER_INDEX;
		iMzNo = TEACHING_MZ_NO;
		iTrayNo = TEACHING_TRAY_NO;
		if (THEAPP.m_iTeachingModuleNo > 0)
			iModuleNo = THEAPP.m_iTeachingModuleNo;
		else
			iModuleNo = TEACHING_MODULE_NO;
		iPcVisionNo = THEAPP.m_iCurTeachVision;
		iNoInspectImage = MAX_IMAGE_TAB;
		iCurThreadIdx = TEACHING_THREAD_INDEX;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		DPOINT dNorthLineStartPoint, dNorthLineEndPoint;
		DPOINT dWestLineStartPoint, dWestLineEndPoint;

		Algorithm* pAlgorithm = THEAPP.m_pAlgorithm;

		for (iImageIdx = 1; iImageIdx <= iNoInspectImage; iImageIdx++)	// 영상 1~티칭영상수, Align Tab 제외
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (((pInspectROIRgn->miInspectionType == INSPECTION_TYPE_AF_NORTH_LINE) ||
					(pInspectROIRgn->miInspectionType == INSPECTION_TYPE_AF_WEST_LINE)) == FALSE)
					continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					{
						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcess || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseImageProcessFilter)
						{
							pAlgorithm->GetPreprocessImage(TRUE, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HPreProcessImage);
							HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, HPreProcessImage, HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}
						else
						{
							HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);
						}

						if (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
						{
							if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_AF_NORTH_LINE)
							{
								dNorthLineStartPoint.x = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX;
								dNorthLineStartPoint.y = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY;
								dNorthLineEndPoint.x = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX;
								dNorthLineEndPoint.y = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY;
							}
							else if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_AF_WEST_LINE)
							{
								dWestLineStartPoint.x = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX;
								dWestLineStartPoint.y = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY;
								dWestLineEndPoint.x = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX;
								dWestLineEndPoint.y = pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY;
							}
						}
					}
				}
			}
		}

		HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
		DPOINT dIntersectPoint;
		HTuple HAngle;

		if (dNorthLineStartPoint.x >= 0 && dNorthLineEndPoint.x >= 0 && dWestLineStartPoint.x >= 0 && dWestLineEndPoint.x >= 0)
		{
			IntersectionLines(dNorthLineStartPoint.y, dNorthLineStartPoint.x, dNorthLineEndPoint.y, dNorthLineEndPoint.x, dWestLineStartPoint.y, dWestLineStartPoint.x, dWestLineEndPoint.y, dWestLineEndPoint.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);
			dIntersectPoint.x = HIntersectPointX[0].D();
			dIntersectPoint.y = HIntersectPointY[0].D();

			LTCornerPoint.x = (int)dIntersectPoint.x;
			LTCornerPoint.y = (int)dIntersectPoint.y;

			AngleLl(dNorthLineStartPoint.y, dNorthLineStartPoint.x, dNorthLineStartPoint.y, dNorthLineEndPoint.x, dNorthLineStartPoint.y, dNorthLineStartPoint.x, dNorthLineEndPoint.y, dNorthLineEndPoint.x, &HAngle);

			TupleDeg(HAngle, &HAngle);
			*pdRotationAngle = HAngle[0].D();
		}

		return LTCornerPoint;

	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::GetAutoFocusLeftTopCornerPoint] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return LTCornerPoint;
	}
}

DPOINT CTabControlDlg::GetAutoFocusBlobCenterPoint(int iRotationAngle, BOOL bUseZigCenterPos, int iZigCenterX, int iZigCenterY)
{
	DPOINT CenterPoint;
	CenterPoint.x = -1;
	CenterPoint.y = -1;

	try
	{
		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return CenterPoint;

		if (THEAPP.m_pModelDataManager->m_pInspectionArea == NULL)
			return CenterPoint;

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return CenterPoint;

		int iImageIdx, iROIIndex, iTabIdx;
		GTRegion *pInspectROIRgn;
		HObject HROIHRegion;
		HObject HResultXLD;
		HObject HAlignRgn;
		HObject HPreProcessImage;

		BOOL bMeasureROIFound;
		HObject HMeasureRetRgn;
		int iMzNo, iTrayNo, iModuleNo, iCurInspectionBufferIdx, iPcVisionNo, iVisionCamType, iNoInspectImage, iCurThreadIdx;

		iVisionCamType = THEAPP.m_iCurVisionCamType;
		iCurInspectionBufferIdx = TEACHING_INSPECT_BUFFER_INDEX;
		iMzNo = TEACHING_MZ_NO;
		iTrayNo = TEACHING_TRAY_NO;
		if (THEAPP.m_iTeachingModuleNo > 0)
			iModuleNo = THEAPP.m_iTeachingModuleNo;
		else
			iModuleNo = TEACHING_MODULE_NO;
		iPcVisionNo = THEAPP.m_iCurTeachVision;
		iNoInspectImage = MAX_IMAGE_TAB;
		iCurThreadIdx = TEACHING_THREAD_INDEX;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		HTuple HImageSizeX, HImageSizeY;
		double dImageCenterX, dImageCenterY, dRAngleRad;
		HTuple HomMat2DIdentity, HomMat2DRotate, HRAngleRad;

		Algorithm* pAlgorithm = THEAPP.m_pAlgorithm;

		for (iImageIdx = 1; iImageIdx <= iNoInspectImage; iImageIdx++)	// 영상 1~티칭영상수, Align Tab 제외
		{
			for (iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iImageIdx)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_AF_ZIG_ROTATION_CENTER)
					continue;

				for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
					{
						HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService_N[iVisionCamType]);

						if (iRotationAngle != 0)
						{
							if (bUseZigCenterPos)
							{
								dImageCenterX = (double)iZigCenterX;
								dImageCenterY = (double)iZigCenterY;
							}
							else
							{
								GetImageSize(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], &HImageSizeX, &HImageSizeY);
								dImageCenterX = HImageSizeX.L() * 0.5;
								dImageCenterY = HImageSizeX.L() * 0.5;
							}

							TupleRad((HTuple)iRotationAngle, &HRAngleRad);
							dRAngleRad = HRAngleRad.D();

							HomMat2dIdentity(&HomMat2DIdentity);
							HomMat2dRotate(HomMat2DIdentity, dRAngleRad, dImageCenterY, dImageCenterX, &HomMat2DRotate);
							AffineTransRegion(HROIHRegion, &HROIHRegion, HomMat2DRotate, "nearest_neighbor");
						}

						double dAlignMatchingScore;
						BOOL bTempROIAlignShiftResult = FALSE;
						int iLoaclAlignDeltaX, iLoaclAlignDeltaY;
						double dLoaclAlignDeltaAngle, dLoaclAlignDeltaAngleFixedPointX, dLoaclAlignDeltaAngleFixedPointY;
						double dLoaclAlignFindX, dLoaclAlignFindY;
						if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseROIAlign) // LocalAlign으로 시도
						{
							HMeasureRetRgn = pAlgorithm->ROIAlignAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &(pInspectROIRgn->m_HROIAlignModelID), &bTempROIAlignShiftResult, &dAlignMatchingScore, &iLoaclAlignDeltaX, &iLoaclAlignDeltaY, &dLoaclAlignDeltaAngle, &dLoaclAlignDeltaAngleFixedPointX, &dLoaclAlignDeltaAngleFixedPointY, 0, 0, &dLoaclAlignFindX, &dLoaclAlignFindY);

							if (dLoaclAlignFindX >= 0 && dLoaclAlignFindY >= 0)
							{
								CenterPoint.x = dLoaclAlignFindX;
								CenterPoint.y = dLoaclAlignFindY;

								if (THEAPP.m_pGFunction->ValidHRegion(HMeasureRetRgn) == TRUE)
								{
									ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn, HMeasureRetRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn));
									Union1(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn));
								}

								THEAPP.m_pInspectAdminViewDlg->UpdateView();

								return CenterPoint;
							}
						}
						else
						{
							HMeasureRetRgn = pAlgorithm->InspectAVI(-1, iCurThreadIdx, pInspectROIRgn, iTabIdx, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType][iImageIdx - 1], HROIHRegion, pInspectROIRgn->m_AlgorithmParam[iTabIdx], &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC][iImageIdx - 1]);

							if (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX >= 0 && pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX >= 0)
							{
								CenterPoint.x = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartX + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndX) * 0.5;
								CenterPoint.y = (pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineStartY + pInspectROIRgn->m_MeasureData[iCurInspectionBufferIdx][iTabIdx].m_dEdgeLineEndY) * 0.5;

								if (THEAPP.m_pGFunction->ValidHRegion(HMeasureRetRgn) == TRUE)
								{
									ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn, HMeasureRetRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn));
									Union1(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn));
								}

								THEAPP.m_pInspectAdminViewDlg->UpdateView();

								return CenterPoint;
							}
						}
					}
				}
			}
		}

		return CenterPoint;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [TabControlDlg::GetAutoFocusBlobCenterPoint] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return CenterPoint;
	}
}
