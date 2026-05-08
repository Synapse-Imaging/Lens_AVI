// AlgorithmShapeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmShapeDlg.h"
#include "afxdialogex.h"


// CAlgorithmShapeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmShapeDlg, CDialog)

CAlgorithmShapeDlg::CAlgorithmShapeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmShapeDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseShape = FALSE;
	m_iEditShapeIlluminationFilterX = 0;
	m_iEditShapeIlluminationFilterY = 0;
	m_dEditShapeIlluminationScaleFactor = 0.0;
	m_dEditShapeCurvatureSmFactor = 0.0;

	GenEmptyObj(&m_HShapeImage);
	GenEmptyObj(&m_HRawImageTop);
	GenEmptyObj(&m_HRawImageBottom);
	GenEmptyObj(&m_HRawImageLeft);
	GenEmptyObj(&m_HRawImageRight);
	m_dEditShapeScalingMulti = 1.0;
	m_dEditShapeScalingAdd = 0;
	m_dEditShapeImageReduceRatio = 2.0;
	m_dEditShapeLedAngle = 33;
	m_iEditShapeImageMargin = 0;
}

CAlgorithmShapeDlg::~CAlgorithmShapeDlg()
{
}

void CAlgorithmShapeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_SHAPE, m_bCheckUseShape);
	DDX_Text(pDX, IDC_EDIT_SHAPE_ILLUMINATION_FILTER_X, m_iEditShapeIlluminationFilterX);
	DDX_Text(pDX, IDC_EDIT_SHAPE_ILLUMINATION_FILTER_Y, m_iEditShapeIlluminationFilterY);
	DDX_Text(pDX, IDC_EDIT_SHAPE_ILLUMINATION_SCALE_FACTOR, m_dEditShapeIlluminationScaleFactor);
	DDX_Text(pDX, IDC_EDIT_SHAPE_CURVATURE_SM_FACTOR, m_dEditShapeCurvatureSmFactor);
	DDX_Control(pDX, IDC_STATIC_SHAPE_IMAGE, m_HWinPanelShape);
	DDX_Control(pDX, IDC_STATIC_RAW_IMAGE_TOP, m_HWinPanelRawImageTop);
	DDX_Control(pDX, IDC_STATIC_RAW_IMAGE_BOTTOM, m_HWinPanelRawImageBottom);
	DDX_Control(pDX, IDC_STATIC_RAW_IMAGE_LEFT, m_HWinPanelRawImageLeft);
	DDX_Control(pDX, IDC_STATIC_RAW_IMAGE_RIGHT, m_HWinPanelRawImageRight);
	DDX_Text(pDX, IDC_EDIT_SHAPE_SCALING_MULTI, m_dEditShapeScalingMulti);
	DDX_Text(pDX, IDC_EDIT_SHAPE_SCALING_ADD, m_dEditShapeScalingAdd);
	DDX_Text(pDX, IDC_EDIT_SHAPE_IMAGE_REDUCE_RATIO, m_dEditShapeImageReduceRatio);
	DDX_Text(pDX, IDC_EDIT_SHAPE_IMAGE_MARGIN, m_iEditShapeImageMargin);
	DDX_Text(pDX, IDC_EDIT_SHAPE_LED_ANGLE, m_dEditShapeLedAngle);

	DDX_Control(pDX, IDC_STATIC_SHAPE, m_StaticShapeImageTitle);
	DDX_Control(pDX, IDC_STATIC_RAWIMAGE_TOP, m_StaticRawImageTopTitle);
	DDX_Control(pDX, IDC_STATIC_RAWIMAGE_BOTTOM, m_StaticRawImageBottomTitle);
	DDX_Control(pDX, IDC_STATIC_RAWIMAGE_LEFT, m_StaticRawImageLeftTitle);
	DDX_Control(pDX, IDC_STATIC_RAWIMAGE_RIGHT, m_StaticRawImageRightTitle);

	DDX_Control(pDX, IDC_BUTTON_SHOW_SHAPE_IMAGE, m_bnShowShapeImage);
	DDX_Control(pDX, IDC_BUTTON_SAVE_SHAPE_IMAGE, m_bnSaveShapeImage);
}


BEGIN_MESSAGE_MAP(CAlgorithmShapeDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_SHAPE_IMAGE, &CAlgorithmShapeDlg::OnBnClickedButtonShowShapeImage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_MFCBUTTON_NEXT_PROCESSING2, &CAlgorithmShapeDlg::OnBnClickedMfcbuttonNextProcessing2)
	ON_BN_CLICKED(IDC_MFCBUTTON_NEXT_PROCESSING3, &CAlgorithmShapeDlg::OnBnClickedMfcbuttonNextProcessing3)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_SHAPE_IMAGE, &CAlgorithmShapeDlg::OnBnClickedButtonSaveShapeImage)
END_MESSAGE_MAP()


// CAlgorithmShapeDlg 메시지 처리기입니다.

void CAlgorithmShapeDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;

	GenEmptyObj(&m_HShapeImage);
	GenEmptyObj(&m_HRawImageTop);
	GenEmptyObj(&m_HRawImageBottom);
	GenEmptyObj(&m_HRawImageLeft);
	GenEmptyObj(&m_HRawImageRight);
}

void CAlgorithmShapeDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseShape = AlgorithmParam.m_bUseShape;
	m_iEditShapeIlluminationFilterX = AlgorithmParam.m_iShapeIlluminationFilterX;
	m_iEditShapeIlluminationFilterY = AlgorithmParam.m_iShapeIlluminationFilterY;
	m_dEditShapeIlluminationScaleFactor = AlgorithmParam.m_dShapeIlluminationScaleFactor;
	m_dEditShapeCurvatureSmFactor = AlgorithmParam.m_dShapeCurvatureSmFactor;
	m_dEditShapeScalingMulti = AlgorithmParam.m_dShapeScalingMulti;
	m_dEditShapeScalingAdd = AlgorithmParam.m_dShapeScalingAdd;
	m_dEditShapeImageReduceRatio = AlgorithmParam.m_dShapeImageReduceRatio;
	m_dEditShapeLedAngle = AlgorithmParam.m_dShapeLedAngle;
	m_iEditShapeImageMargin = AlgorithmParam.m_iShapeImageMargin;

	UpdateData(FALSE);
}

void CAlgorithmShapeDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseShape = m_bCheckUseShape;
	pAlgorithmParam->m_iShapeIlluminationFilterX = m_iEditShapeIlluminationFilterX;
	pAlgorithmParam->m_iShapeIlluminationFilterY = m_iEditShapeIlluminationFilterY;
	pAlgorithmParam->m_dShapeIlluminationScaleFactor = m_dEditShapeIlluminationScaleFactor;
	pAlgorithmParam->m_dShapeCurvatureSmFactor = m_dEditShapeCurvatureSmFactor;
	pAlgorithmParam->m_dShapeScalingMulti = m_dEditShapeScalingMulti;
	pAlgorithmParam->m_dShapeScalingAdd = m_dEditShapeScalingAdd;
	pAlgorithmParam->m_dShapeImageReduceRatio = m_dEditShapeImageReduceRatio;
	pAlgorithmParam->m_dShapeLedAngle = m_dEditShapeLedAngle;
	pAlgorithmParam->m_iShapeImageMargin = m_iEditShapeImageMargin;
}

BOOL CAlgorithmShapeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT ViewRect;

	m_HWinPanelShape.CreateHWindow();
	m_HWinPanelShape.GetClientRect(&ViewRect);
	SetWindowExtents(m_HWinPanelShape.mlWindowHandle, 0, 0, ViewRect.right, ViewRect.bottom);

	m_HWinPanelRawImageTop.CreateHWindow();
	m_HWinPanelRawImageTop.GetClientRect(&ViewRect);
	SetWindowExtents(m_HWinPanelRawImageTop.mlWindowHandle, 0, 0, ViewRect.right, ViewRect.bottom);

	m_HWinPanelRawImageBottom.CreateHWindow();
	m_HWinPanelRawImageBottom.GetClientRect(&ViewRect);
	SetWindowExtents(m_HWinPanelRawImageBottom.mlWindowHandle, 0, 0, ViewRect.right, ViewRect.bottom);

	m_HWinPanelRawImageLeft.CreateHWindow();
	m_HWinPanelRawImageLeft.GetClientRect(&ViewRect);
	SetWindowExtents(m_HWinPanelRawImageLeft.mlWindowHandle, 0, 0, ViewRect.right, ViewRect.bottom);

	m_HWinPanelRawImageRight.CreateHWindow();
	m_HWinPanelRawImageRight.GetClientRect(&ViewRect);
	SetWindowExtents(m_HWinPanelRawImageRight.mlWindowHandle, 0, 0, ViewRect.right, ViewRect.bottom);

	m_StaticShapeImageTitle.Init_Ctrl(_T("Arial"), 9, TRUE, RGB(255, 255, 0), RGB(1, 1, 1));
	m_StaticRawImageTopTitle.Init_Ctrl(_T("Arial"), 9, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_StaticRawImageBottomTitle.Init_Ctrl(_T("Arial"), 9, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_StaticRawImageLeftTitle.Init_Ctrl(_T("Arial"), 9, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_StaticRawImageRightTitle.Init_Ctrl(_T("Arial"), 9, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));

	m_bnShowShapeImage.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnSaveShapeImage.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAlgorithmShapeDlg::OnBnClickedButtonShowShapeImage()
{
	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		HObject HInspectAreaRgn;
		HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		int iShapeRawImageIdx[NO_USED_SHAPE_RAWIMAGE];
		int iShapeImageCnt = 0;
		for (int i = 0; i < MAX_IMAGE_TAB; i++)
		{
			if (THEAPP.m_pModelDataManager->m_bUseDiffusedImage[i])
			{
				iShapeRawImageIdx[iShapeImageCnt] = i;
				++iShapeImageCnt;
			}

			if (iShapeImageCnt >= NO_USED_SHAPE_RAWIMAGE)
				break;
		}

		if (iShapeImageCnt < NO_USED_SHAPE_RAWIMAGE)
		{
			AfxMessageBox("요철 영상 생성을 위한 Raw Image 갯수가 4개보다 작습니다. [정보] 탭의 요철 영상 체크를 확인해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		CAlgorithmParam Param;
		Param.m_bUseShape = m_bCheckUseShape;
		Param.m_iShapeIlluminationFilterX = m_iEditShapeIlluminationFilterX;
		Param.m_iShapeIlluminationFilterY = m_iEditShapeIlluminationFilterY;
		Param.m_dShapeIlluminationScaleFactor = m_dEditShapeIlluminationScaleFactor;
		Param.m_dShapeCurvatureSmFactor = m_dEditShapeCurvatureSmFactor;
		Param.m_dShapeScalingMulti = m_dEditShapeScalingMulti;
		Param.m_dShapeScalingAdd = m_dEditShapeScalingAdd;
		Param.m_dShapeImageReduceRatio = m_dEditShapeImageReduceRatio;
		Param.m_dShapeLedAngle = m_dEditShapeLedAngle;
		Param.m_iShapeImageMargin = m_iEditShapeImageMargin;

		double dTimeStart = 0, dTimeEnd = 0;
		dTimeStart = GetTickCount();

		HObject HRectTransRgn;
		ShapeTrans(HInspectAreaRgn, &HRectTransRgn, "rectangle1");

		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iShapeRawImageIdx[0]], HRectTransRgn, &m_HRawImageTop);
		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iShapeRawImageIdx[1]], HRectTransRgn, &m_HRawImageBottom);
		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iShapeRawImageIdx[2]], HRectTransRgn, &m_HRawImageLeft);
		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iShapeRawImageIdx[3]], HRectTransRgn, &m_HRawImageRight);

		CropDomain(m_HRawImageTop, &m_HRawImageTop);
		CropDomain(m_HRawImageBottom, &m_HRawImageBottom);
		CropDomain(m_HRawImageLeft, &m_HRawImageLeft);
		CropDomain(m_HRawImageRight, &m_HRawImageRight);

		InvalidateRect(NULL, TRUE);

		CString sTimeCheck;
		dTimeEnd = GetTickCount();
		sTimeCheck.Format("요철영상 생성시간: %.3lf (s)", (dTimeEnd - dTimeStart) / 1000);
		AfxMessageBox((LPCTSTR)sTimeCheck, MB_SYSTEMMODAL | MB_ICONINFORMATION);
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

		strLog.Format("Halcon Exception [AlgorithmShapeDlg::OnBnClickedButtonShowShapeImage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmShapeDlg::OnBnClickedButtonSaveShapeImage()
{
	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		HObject HInspectAreaRgn;
		HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		int iShapeRawImageIdx[NO_USED_SHAPE_RAWIMAGE];
		int iShapeImageCnt = 0;
		for (int i = 0; i < MAX_IMAGE_TAB; i++)
		{
			if (THEAPP.m_pModelDataManager->m_bUseDiffusedImage[i])
			{
				iShapeRawImageIdx[iShapeImageCnt] = i;
				++iShapeImageCnt;
			}

			if (iShapeImageCnt >= NO_USED_SHAPE_RAWIMAGE)
				break;
		}

		if (iShapeImageCnt < NO_USED_SHAPE_RAWIMAGE)
		{
			AfxMessageBox("요철 영상 생성을 위한 Raw Image 갯수가 4개보다 작습니다. [정보] 탭의 요철 영상 체크를 확인해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		TCHAR m_szFolderDisplayName[MAX_PATH] = { 0 };
		TCHAR m_szFolderPath[MAX_PATH] = { 0 };
		LPITEMIDLIST m_pidlSelected = NULL;

		BROWSEINFOA m_bi;
		memset(&m_bi, 0, sizeof(m_bi));
		m_bi.hwndOwner = m_hWnd;

		CString sStartFolder;
		sStartFolder = _T("");

		m_bi.pszDisplayName = (LPSTR)(LPCSTR)sStartFolder;
		m_bi.lpszTitle = _T("Image Save Folder");
		m_bi.ulFlags = BIF_RETURNONLYFSDIRS;
		m_pidlSelected = ::SHBrowseForFolder(&m_bi);

		CString bbb;
		if (m_pidlSelected != NULL)
		{
			::SHGetPathFromIDList(m_pidlSelected, m_szFolderPath);
		}

		CString sSaveFolder, sTemp;
		sTemp = m_szFolderPath;
		sSaveFolder = sTemp;

		CAlgorithmParam Param;
		Param.m_bUseShape = m_bCheckUseShape;
		Param.m_iShapeIlluminationFilterX = m_iEditShapeIlluminationFilterX;
		Param.m_iShapeIlluminationFilterY = m_iEditShapeIlluminationFilterY;
		Param.m_dShapeIlluminationScaleFactor = m_dEditShapeIlluminationScaleFactor;
		Param.m_dShapeCurvatureSmFactor = m_dEditShapeCurvatureSmFactor;
		Param.m_dShapeScalingMulti = m_dEditShapeScalingMulti;
		Param.m_dShapeScalingAdd = m_dEditShapeScalingAdd;
		Param.m_dShapeImageReduceRatio = m_dEditShapeImageReduceRatio;
		Param.m_dShapeLedAngle = m_dEditShapeLedAngle;
		Param.m_iShapeImageMargin = m_iEditShapeImageMargin;

		double dTimeStart = 0, dTimeEnd = 0;
		dTimeStart = GetTickCount();

		HObject HRectTransRgn;
		ShapeTrans(HInspectAreaRgn, &HRectTransRgn, "rectangle1");

		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iShapeRawImageIdx[0]], HRectTransRgn, &m_HRawImageTop);
		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iShapeRawImageIdx[1]], HRectTransRgn, &m_HRawImageBottom);
		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iShapeRawImageIdx[2]], HRectTransRgn, &m_HRawImageLeft);
		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[CHANNEL_TYPE_I][iShapeRawImageIdx[3]], HRectTransRgn, &m_HRawImageRight);

		CropDomain(m_HRawImageTop, &m_HRawImageTop);
		CropDomain(m_HRawImageBottom, &m_HRawImageBottom);
		CropDomain(m_HRawImageLeft, &m_HRawImageLeft);
		CropDomain(m_HRawImageRight, &m_HRawImageRight);

		dTimeEnd = GetTickCount();

		CString sSaveFileName;

		sSaveFileName.Format("%sTop", sSaveFolder);
		WriteImage(m_HRawImageTop, "jpeg 100", 0, HTuple(sSaveFileName));
		sSaveFileName.Format("%sBottom", sSaveFolder);
		WriteImage(m_HRawImageBottom, "jpeg 100", 0, HTuple(sSaveFileName));
		sSaveFileName.Format("%sLeft", sSaveFolder);
		WriteImage(m_HRawImageLeft, "jpeg 100", 0, HTuple(sSaveFileName));
		sSaveFileName.Format("%sRight", sSaveFolder);
		WriteImage(m_HRawImageRight, "jpeg 100", 0, HTuple(sSaveFileName));
		sSaveFileName.Format("%sShape", sSaveFolder);
		WriteImage(m_HShapeImage, "jpg", 0, HTuple(sSaveFileName));

		CString sTimeCheck;
		sTimeCheck.Format("영상 저장 완료 / 요철영상 생성시간: %.3lf (s)", (dTimeEnd - dTimeStart) / 1000);
		AfxMessageBox(sTimeCheck, MB_SYSTEMMODAL | MB_ICONINFORMATION);

		return;
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

		strLog.Format("Halcon Exception [AlgorithmImageCompareDlg::OnBnClickedButtonSaveShapeImage] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

void CAlgorithmShapeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

	HTuple HImageWidth, HImageHeight;

	if (THEAPP.m_pGFunction->ValidHImage(m_HShapeImage))
	{
		GetImageSize(m_HShapeImage, &HImageWidth, &HImageHeight);
		SetPart(m_HWinPanelShape.mlWindowHandle, 0, 0, HImageHeight, HImageWidth);
		DispObj(m_HShapeImage, m_HWinPanelShape.mlWindowHandle);
	}
	else
		ClearWindow(m_HWinPanelShape.mlWindowHandle);

	if (THEAPP.m_pGFunction->ValidHImage(m_HRawImageTop))
	{
		GetImageSize(m_HRawImageTop, &HImageWidth, &HImageHeight);
		SetPart(m_HWinPanelRawImageTop.mlWindowHandle, 0, 0, HImageHeight, HImageWidth);
		DispObj(m_HRawImageTop, m_HWinPanelRawImageTop.mlWindowHandle);
	}
	else
		ClearWindow(m_HWinPanelRawImageTop.mlWindowHandle);

	if (THEAPP.m_pGFunction->ValidHImage(m_HRawImageBottom))
	{
		GetImageSize(m_HRawImageBottom, &HImageWidth, &HImageHeight);
		SetPart(m_HWinPanelRawImageBottom.mlWindowHandle, 0, 0, HImageHeight, HImageWidth);
		DispObj(m_HRawImageBottom, m_HWinPanelRawImageBottom.mlWindowHandle);
	}
	else
		ClearWindow(m_HWinPanelRawImageBottom.mlWindowHandle);

	if (THEAPP.m_pGFunction->ValidHImage(m_HRawImageLeft))
	{
		GetImageSize(m_HRawImageLeft, &HImageWidth, &HImageHeight);
		SetPart(m_HWinPanelRawImageLeft.mlWindowHandle, 0, 0, HImageHeight, HImageWidth);
		DispObj(m_HRawImageLeft, m_HWinPanelRawImageLeft.mlWindowHandle);
	}
	else
		ClearWindow(m_HWinPanelRawImageLeft.mlWindowHandle);

	if (THEAPP.m_pGFunction->ValidHImage(m_HRawImageRight))
	{
		GetImageSize(m_HRawImageRight, &HImageWidth, &HImageHeight);
		SetPart(m_HWinPanelRawImageRight.mlWindowHandle, 0, 0, HImageHeight, HImageWidth);
		DispObj(m_HRawImageRight, m_HWinPanelRawImageRight.mlWindowHandle);
	}
	else
		ClearWindow(m_HWinPanelRawImageRight.mlWindowHandle);
}


void CAlgorithmShapeDlg::OnBnClickedMfcbuttonNextProcessing2()
{
	THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->UpdateAlgorithmTab(8);
}


void CAlgorithmShapeDlg::OnBnClickedMfcbuttonNextProcessing3()
{
	THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->UpdateAlgorithmTab(9);
}

