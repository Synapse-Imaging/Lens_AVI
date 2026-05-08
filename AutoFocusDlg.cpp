// AutoFocusDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "AutoFocusDlg.h"
#include "afxdialogex.h"


// CAutoFocusDlg 대화 상자

IMPLEMENT_DYNAMIC(CAutoFocusDlg, CDialog)

CAutoFocusDlg* CAutoFocusDlg::m_pInstance = NULL;

CAutoFocusDlg* CAutoFocusDlg::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CAutoFocusDlg;

		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

			m_pInstance->Create(IDD_AUTO_FOCUS_DLG, pFrame->GetActiveView());
		}
	}

	return m_pInstance;
}

void CAutoFocusDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

void CAutoFocusDlg::Show()
{
	SetWindowPos(&wndTopMost, 1000, 5, 0, 0, SWP_NOSIZE);
	ShowWindow(SW_SHOW);

	m_bShowDlg = TRUE;

	UpdateParam();
}

void CAutoFocusDlg::Hide()
{
	ShowWindow(SW_HIDE);

	m_bShowDlg = FALSE;
}

struct AF_ZIG_CENTER_RESULT
{
	double x;
	double y;
};

UINT ZigRotationCenterSearch_Thread(LPVOID lp)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[THEAPP.m_pCameraManager->GetVisionCamName()];

	CString strLog;

#ifndef INLINE_MODE
	return 1;
#endif

	THEAPP.m_pInspectAdminViewDlg->StopLive();
	Sleep(10);

	CAutoFocusDlg* pAutoFocusDlg = (CAutoFocusDlg*)lp;

	double dCurZPosition, dCurLightZPosition, dCurTPosition, dCurRPosition, dCurXPosition, dCurYPosition;
	DPOINT dBlobCenterPointAry[36];
	int iCheckCount;
	HTuple HBlobCenterX, HBlobCenterY;
	DWORD dwMoveStart = 0, dwMoveEnd = 0;

	int iRotationStep = 10;
	int iRotationAngle = 0;
	int iTeachingGrab = THEAPP.m_pTabControlDlg->m_iCurrentTab;
	int iImageIndex = iTeachingGrab - 1;
	if (iImageIndex < 0)
		iImageIndex = 0;

	int iStartGrabBufferIndex = iImageIndex;
	BOOL bGrabSuccess = FALSE;

	THEAPP.m_pCameraManager->SetTeachImageSave(FALSE);

	iCheckCount = 0;
	TupleGenConst(0, 0, &HBlobCenterX);
	TupleGenConst(0, 0, &HBlobCenterY);

	GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn));
	GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCenterRgn));
	double dSetRPosition = THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageRotatePosRef + THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_dEditImageRotatePos[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1];
	for (iRotationAngle = 0; iRotationAngle <= 360; iRotationAngle += iRotationStep)
	{
		int iJigNo;
		if (THEAPP.m_pCameraManager->GetVisionCamName() == VISION_NUMBER_1 || THEAPP.m_pCameraManager->GetVisionCamName() == VISION_NUMBER_2)
			iJigNo = (THEAPP.Struct_PreferenceStruct.m_iPCType * 2) + 1;
		else
			iJigNo = (THEAPP.Struct_PreferenceStruct.m_iPCType * 2) + 2;

		dCurZPosition = MOTION_NOUSE;
		dCurLightZPosition = MOTION_NOUSE;
		dCurTPosition = MOTION_NOUSE;
		dCurXPosition = MOTION_NOUSE;
		dCurYPosition = MOTION_NOUSE;
		if (iRotationAngle == 360)
		{
			dCurRPosition = dSetRPosition;

			THEAPP.m_pHandlerService->m_bMotionMoveComplete[THEAPP.m_pCameraManager->GetVisionCamName()] = FALSE;
			THEAPP.m_pHandlerService->Set_AMoveRequest(THEAPP.m_pCameraManager->GetVisionCamName(), THEAPP.m_iCurStageIndex, iJigNo, dCurZPosition, dCurLightZPosition, dCurXPosition, dCurYPosition, dCurTPosition, dCurRPosition);

			break;
		}
		dBlobCenterPointAry[iCheckCount].x = -1;
		dBlobCenterPointAry[iCheckCount].y = -1;

		dCurRPosition = dSetRPosition + (double)iRotationAngle;

		THEAPP.m_pHandlerService->m_bMotionMoveComplete[THEAPP.m_pCameraManager->GetVisionCamName()] = FALSE;
		THEAPP.m_pHandlerService->Set_AMoveRequest(THEAPP.m_pCameraManager->GetVisionCamName(), THEAPP.m_iCurStageIndex, iJigNo, dCurZPosition, dCurLightZPosition, dCurXPosition, dCurYPosition, dCurTPosition, dCurRPosition);

		dwMoveStart = GetTickCount();

		while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[THEAPP.m_pCameraManager->GetVisionCamName()]) // 무브 컴플리트 기다림
		{
			dwMoveEnd = GetTickCount();

			if ((dwMoveEnd - dwMoveStart) > 1000)
			{
				strLog.Format("%s/ Manual move complete timeout", sVisionCamType_Short);
				THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));

				AfxMessageBox("핸들러로부터 모션 완료 신호가 없습니다. 핸들러 상태 확인해 주세요", MB_SYSTEMMODAL | MB_ICONERROR);
				return 0;
			}

			Sleep(1);
		}

		int iAddrSeq[LIGHTCTRL_PAGE_COUNT];
		int iSelectPage = pAutoFocusDlg->m_cbAFZigCenterLightPage.GetCurSel();
		for (int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
			iAddrSeq[i] = iSelectPage;
		THEAPP.m_pModelDataManager->m_PageControlData.SetGrabSequence(THEAPP.m_pCameraManager->GetVisionCamName(), LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, iAddrSeq, 1);

		bGrabSuccess = THEAPP.m_pCameraManager->CameraStartGrab(iStartGrabBufferIndex, LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, 1);

		for (int i = iStartGrabBufferIndex; i < (iStartGrabBufferIndex + THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iImageIndex]]); i++)
		{
			if (THEAPP.m_pCameraManager->m_sBayerType == "GB")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
			else if (THEAPP.m_pCameraManager->m_sBayerType == "BG")
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_BG);
			else
				MbufBayer(THEAPP.m_pCameraManager->MilMonoImageBuf[0][i], THEAPP.m_pCameraManager->MilColorImageBuf[0][i], THEAPP.m_pCameraManager->m_MilWBCoeff, M_BAYER_GB);
		}

		if (bGrabSuccess)
		{
			int iGrabCircularIdx = THEAPP.m_pCameraManager->GetGrabCircularIndex();

			if (THEAPP.m_iModeSwitch == MODE_ADMIN_TEACH_VIEW)								// 티칭화면이면 ....
			{
				CopyImage(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][0], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R]));
				CopyImage(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][1], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G]));
				CopyImage(THEAPP.m_pCameraManager->m_hoCallBackImage[iGrabCircularIdx][iImageIndex][2], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B]));

				Compose3(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR]));
				TransFromRgb(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_R], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_G], THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_B],
					&(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_H]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_S]), &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]), "hsi");
				Rgb1ToGray(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_COLOR], &(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[CHANNEL_TYPE_I]));

				for (int i = 0; i < CHANNEL_NUM; i++)
				{
					CopyImage(THEAPP.m_pInspectAdminViewDlg->m_pHLiveImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][THEAPP.m_pTabControlDlg->m_iCurrentTab - 1]));
				}

				THEAPP.m_pInspectAdminViewDlg->UpdateView();

				DPOINT CenterPoint;

				CenterPoint = THEAPP.m_pTabControlDlg->GetAutoFocusBlobCenterPoint(-iRotationAngle, pAutoFocusDlg->m_bCheckUseZigCenterPos, pAutoFocusDlg->m_iEditZigCenterX, pAutoFocusDlg->m_iEditZigCenterY);
				if (CenterPoint.x >= 0)
				{
					dBlobCenterPointAry[iCheckCount].x = CenterPoint.x;
					dBlobCenterPointAry[iCheckCount].y = CenterPoint.y;

					TupleConcat(HBlobCenterX, HTuple(CenterPoint.x), &HBlobCenterX);
					TupleConcat(HBlobCenterY, HTuple(CenterPoint.y), &HBlobCenterY);
				}
			}
		}  // if (bGrabSuccess)

		++iCheckCount;
	}

	HTuple HNoFoundPoint;
	Hlong lNoFoundPoint;
	TupleLength(HBlobCenterX, &HNoFoundPoint);
	lNoFoundPoint = HNoFoundPoint.L();

	if (lNoFoundPoint < (iCheckCount / 2))
	{
		AfxMessageBox("Zig Rotation Center Search Fails: Too small number of points found", MB_SYSTEMMODAL | MB_ICONERROR);

		return 0;
	}

	DPOINT dPoint1, dPoint2;
	HTuple HDistance;
	double dDistance;
	int iNoValidPoint;

	double dOuterNormalRadius = 0;
	iNoValidPoint = 0;

	for (int iii = 0; iii < (iCheckCount / 2); iii++)
	{
		dPoint1.x = dBlobCenterPointAry[iii].x;
		dPoint1.y = dBlobCenterPointAry[iii].y;

		dPoint2.x = dBlobCenterPointAry[iii + iCheckCount / 2].x;
		dPoint2.y = dBlobCenterPointAry[iii + iCheckCount / 2].y;

		if (dPoint1.x < 0 || dPoint2.x < 0)
			continue;

		DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
		dDistance = HDistance.D();
		dOuterNormalRadius += dDistance;

		++iNoValidPoint;
	}

	if (iNoValidPoint > 9)
	{
		dOuterNormalRadius /= iNoValidPoint;
		dOuterNormalRadius *= 0.5;
	}
	else
	{
		AfxMessageBox("Zig Rotation Center Search Fails: Too small valid points", MB_SYSTEMMODAL | MB_ICONERROR);

		return 0;
	}

	BOOL bCircleFound;
	double dOuterCircleCenterX, dOuterCircleCenterY, dOuterCircleRadius;

	bCircleFound = THEAPP.m_pAlgorithm->LeastSquareCircleFitting(HBlobCenterX, HBlobCenterY, dOuterNormalRadius, &dOuterCircleCenterX, &dOuterCircleCenterY, &dOuterCircleRadius);
	if (bCircleFound)
	{
		pAutoFocusDlg->m_dEditAfZigRotationCenterX = dOuterCircleCenterX;
		pAutoFocusDlg->m_dEditAfZigRotationCenterY = dOuterCircleCenterY;

		AF_ZIG_CENTER_RESULT* pRes = new AF_ZIG_CENTER_RESULT;
		pRes->x = dOuterCircleCenterX;
		pRes->y = dOuterCircleCenterY;
		if (pAutoFocusDlg && ::IsWindow(pAutoFocusDlg->GetSafeHwnd()))
			::PostMessage(pAutoFocusDlg->GetSafeHwnd(), WM_AF_ZIG_ROT_CENTER_RESULT, 0, (LPARAM)pRes);
		else
			delete pRes;

		HObject HROIRgn;
		GenEmptyObj(&HROIRgn);
		GenCircle(&HROIRgn, dOuterCircleCenterY, dOuterCircleCenterX, 10);
		ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCenterRgn, HROIRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCenterRgn));
		Union1(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCenterRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCenterRgn));
		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		CString sMsg;
		sMsg.Format("Zig Rotation Center Search Done! X,Y = (%d, %d)", (int)dOuterCircleCenterX, (int)dOuterCircleCenterY);
		AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);
	}
	else
	{
		AfxMessageBox("Circle fit fails", MB_SYSTEMMODAL | MB_ICONINFORMATION);
	}

	return 1;
}

LRESULT CAutoFocusDlg::OnAfZigRotCenterResult(WPARAM wParam, LPARAM lParam)
{
	AF_ZIG_CENTER_RESULT* p = reinterpret_cast<AF_ZIG_CENTER_RESULT*>(lParam);
	if (p)
	{
		m_dEditAfZigRotationCenterX = p->x;
		m_dEditAfZigRotationCenterY = p->y;

		UpdateData(FALSE);

		delete p;
	}
	return 0;
}

CAutoFocusDlg::CAutoFocusDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_AUTO_FOCUS_DLG, pParent)
{
	m_dEditZigCenterCamZ = 5;
	m_bCheckUseZigCenterPos = FALSE;
	m_iEditZigCenterX = 2560;
	m_iEditZigCenterY = 2560;
	m_dEditAfZigRotationCenterX = -1;
	m_dEditAfZigRotationCenterY = -1;

	m_iEditAfLeftTopCornerX = -1;
	m_iEditAfLeftTopCornerY = -1;
	m_dEditAfRotationAngle = 0;

	m_iEditAfLensStartImageNo = 1;
	m_iEditAfLensEndImageNo = 2;
	m_iEditAfModuleRotationAngleImageNo = 3;
	m_iEditAfEastSideStartImageNo = -1;
	m_iEditAfEastSideEndImageNo = -1;
	m_iEditAfWestSideStartImageNo = -1;
	m_iEditAfWestSideEndImageNo = -1;
	m_iEditAfNorthSideStartImageNo = -1;
	m_iEditAfNorthSideEndImageNo = -1;
	m_iEditAfSouthSideStartImageNo = -1;
	m_iEditAfSouthSideEndImageNo = -1;

	m_bShowDlg = FALSE;

	m_pThread = NULL;
}

CAutoFocusDlg::~CAutoFocusDlg()
{
}

void CAutoFocusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ZIG_CENTER_CAM_Z, m_dEditZigCenterCamZ);
	DDX_Check(pDX, IDC_CHECK_USE_ZIG_CENTER_POS, m_bCheckUseZigCenterPos);
	DDX_Text(pDX, IDC_EDIT_ZIG_CENTER_X, m_iEditZigCenterX);
	DDX_Text(pDX, IDC_EDIT_ZIG_CENTER_Y, m_iEditZigCenterY);
	DDX_Control(pDX, IDC_COMBO_SELECT_CALIBRATION_PAGE, m_cbAFZigCenterLightPage);
	DDX_Control(pDX, IDC_BUTTON_START_CENTER_CALIBRATION, m_bnRunCalibration);
	DDX_Text(pDX, IDC_EDIT_ZIG_ROTATION_CENTER_X, m_dEditAfZigRotationCenterX);
	DDX_Text(pDX, IDC_EDIT_ZIG_ROTATION_CENTER_Y, m_dEditAfZigRotationCenterY);

	DDX_Control(pDX, IDC_BUTTON_TOP_SAVE_LT_POS, m_bnSaveLeftTopCornerPosition);
	DDX_Text(pDX, IDC_EDIT_LEFT_TOP_CORNER_X, m_iEditAfLeftTopCornerX);
	DDX_Text(pDX, IDC_EDIT_LEFT_TOP_CORNER_Y, m_iEditAfLeftTopCornerY);
	DDX_Text(pDX, IDC_EDIT_ROTATION_ANGLE, m_dEditAfRotationAngle);

	DDX_Text(pDX, IDC_EDIT_AF_LENS_START_IMAGE_NO, m_iEditAfLensStartImageNo);
	DDX_Text(pDX, IDC_EDIT_AF_LENS_END_IMAGE_NO, m_iEditAfLensEndImageNo);
	DDX_Text(pDX, IDC_EDIT_AF_MODULE_ROTATION_ANGLE_IMAGE_NO, m_iEditAfModuleRotationAngleImageNo);
	DDX_Text(pDX, IDC_EDIT_AF_EAST_SIDE_START_IMAGE_NO, m_iEditAfEastSideStartImageNo);
	DDX_Text(pDX, IDC_EDIT_AF_EAST_SIDE_END_IMAGE_NO, m_iEditAfEastSideEndImageNo);
	DDX_Text(pDX, IDC_EDIT_AF_WEST_SIDE_END_IMAGE_NO, m_iEditAfWestSideEndImageNo);
	DDX_Text(pDX, IDC_EDIT_AF_WEST_SIDE_START_IMAGE_NO, m_iEditAfWestSideStartImageNo);
	DDX_Text(pDX, IDC_EDIT_AF_NORTH_SIDE_START_IMAGE_NO, m_iEditAfNorthSideStartImageNo);
	DDX_Text(pDX, IDC_EDIT_AF_NORTH_SIDE_END_IMAGE_NO, m_iEditAfNorthSideEndImageNo);
	DDX_Text(pDX, IDC_EDIT_AF_SOUTH_SIDE_START_IMAGE_NO, m_iEditAfSouthSideStartImageNo);
	DDX_Text(pDX, IDC_EDIT_AF_SOUTH_SIDE_END_IMAGE_NO, m_iEditAfSouthSideEndImageNo);

	DDX_Control(pDX, IDC_BUTTON_SAVE, m_bnSave);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_bnClose);
}


BEGIN_MESSAGE_MAP(CAutoFocusDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_START_CENTER_CALIBRATION, &CAutoFocusDlg::OnBnClickedButtonStartCenterCalibration)
	ON_BN_CLICKED(IDC_BUTTON_TOP_SAVE_LT_POS, &CAutoFocusDlg::OnBnClickedButtonTopSaveLtPos)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CAutoFocusDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CAutoFocusDlg::OnBnClickedButtonClose)

	ON_MESSAGE(WM_AF_ZIG_ROT_CENTER_RESULT, &CAutoFocusDlg::OnAfZigRotCenterResult)
END_MESSAGE_MAP()

BOOL CAutoFocusDlg::PreTranslateMessage(MSG* pMsg)
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

// CAutoFocusDlg 메시지 처리기
BOOL CAutoFocusDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTopMost, 1000, 5, 0, 0, SWP_NOSIZE);

	m_bnRunCalibration.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnSaveLeftTopCornerPosition.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnClose.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnSave.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_cbAFZigCenterLightPage.ResetContent();
	for (int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
	{
		CString sTemp;
		sTemp.Format("Page%d", i + 1);
		m_cbAFZigCenterLightPage.AddString(sTemp);
	}

	m_cbAFZigCenterLightPage.SetCurSel(0);

	ChangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAutoFocusDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("오토 포커스 설정"));
		SetDlgItemText(IDC_GROUP_AUTOFOCUSDLG_1, _T("포켓 지그 중심 캘리브레이션"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_1, _T("카메라 Z축 이동 (mm)"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_2, _T("지그 중심 이미지 포지션 (x, y)"));
		SetDlgItemText(IDC_CHECK_USE_ZIG_CENTER_POS, _T("사용"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_3, _T("(픽셀)"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_4, _T("조명 페이지 선택"));
		SetDlgItemText(IDC_BUTTON_START_CENTER_CALIBRATION, _T("캘리브레이션 실행"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_5, _T("지그 회전 중심 (x, y)"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_6, _T("(픽셀)"));

		SetDlgItemText(IDC_GROUP_AUTOFOCUSDLG_2, _T("마스터 샘플 티칭"));
		SetDlgItemText(IDC_BUTTON_TOP_SAVE_LT_POS, _T("VCM 코너 기준 위치 티칭"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_7, _T("VCM 코너 기준 위치 (x, y)"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_8, _T("(픽셀)"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_9, _T("회전 각도"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_10, _T("(도)"));

		SetDlgItemText(IDC_GROUP_AUTOFOCUSDLG_3, _T("적용 영상 설정"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_11, _T("Top 비전 / 렌즈 오토 포커스 적용 시작 영상 번호"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_12, _T("Top 비전 / 렌즈 오토 포커스 적용 마지막 영상 번호"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_13, _T("제품 회전 각도 계산 영상 번호"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_14, _T("Top 영상 기준 East side 스캔 시작/마지막 영상 번호"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_15, _T("Top 영상 기준 West side 스캔 시작/마지막 영상 번호"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_16, _T("Top 영상 기준 North side 스캔 시작/마지막 영상 번호"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_17, _T("Top 영상 기준 South side 스캔 시작/마지막 영상 번호"));

		SetDlgItemText(IDC_BUTTON_SAVE, _T("저장"));
		SetDlgItemText(IDC_BUTTON_CLOSE, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Set the auto focus"));
		SetDlgItemText(IDC_GROUP_AUTOFOCUSDLG_1, _T("Calibration to pocket jig center"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_1, _T("Move to camera Z (mm)"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_2, _T("Jig center image position (x, y)"));
		SetDlgItemText(IDC_CHECK_USE_ZIG_CENTER_POS, _T("Use"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_3, _T("(pixel)"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_4, _T("Select light page"));
		SetDlgItemText(IDC_BUTTON_START_CENTER_CALIBRATION, _T("Run the calibration"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_5, _T("Jig rotation center (x, y)"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_6, _T("(pixel)"));

		SetDlgItemText(IDC_GROUP_AUTOFOCUSDLG_2, _T("Teaching to master sample"));
		SetDlgItemText(IDC_BUTTON_TOP_SAVE_LT_POS, _T("Teaching to VCM corner reference position"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_7, _T("VCM corner reference position (x, y)"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_8, _T("(pixel)"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_9, _T("Rotation angle"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_10, _T("(degree)"));

		SetDlgItemText(IDC_GROUP_AUTOFOCUSDLG_3, _T("Set the apply image"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_11, _T("Top vision / Lens auto focus start image number"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_12, _T("Top vision / Lens auto focus end image number"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_13, _T("Image number for calculating product rotation angle"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_14, _T("Scan start/end image number on the East based on Top image"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_15, _T("Scan start/end image number on the West based on Top image"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_16, _T("Scan start/end image number on the North based on Top image"));
		SetDlgItemText(IDC_STATIC_AUTOFOCUSDLG_17, _T("Scan start/end image number on the South based on Top image"));

		SetDlgItemText(IDC_BUTTON_SAVE, _T("Save"));
		SetDlgItemText(IDC_BUTTON_CLOSE, _T("Close"));
	}
}

void CAutoFocusDlg::OnBnClickedButtonClose()
{
	Hide();
}

void CAutoFocusDlg::OnBnClickedButtonStartCenterCalibration()
{
	UpdateData(TRUE);

	BOOL nRes = FALSE;
	nRes = THEAPP.m_pTabControlDlg->m_pLightControlDlg->MoveZigRotationCenterPos(m_dEditZigCenterCamZ);

	if (nRes)
	{
		Sleep(2000);

		AfxBeginThread(ZigRotationCenterSearch_Thread, this);
	}
}

void CAutoFocusDlg::OnBnClickedButtonTopSaveLtPos()
{
	POINT LTCornerPoint;
	double dRotationAngle;

	LTCornerPoint = THEAPP.m_pTabControlDlg->GetAutoFocusLeftTopCornerPoint(&dRotationAngle);
	if (LTCornerPoint.x < 0)
	{
		AfxMessageBox("Error", MB_ICONERROR | MB_SYSTEMMODAL);
	}
	else
	{
		m_iEditAfLeftTopCornerX = LTCornerPoint.x;
		m_iEditAfLeftTopCornerY = LTCornerPoint.y;
		m_dEditAfRotationAngle = dRotationAngle;

		UpdateData(FALSE);

		AfxMessageBox("Success", MB_SYSTEMMODAL);
	}
}

void CAutoFocusDlg::OnBnClickedButtonSave()
{
	UpdateData(TRUE);

	THEAPP.m_pModelDataManager->m_dZigCenterCamZ[THEAPP.m_iCurStageIndex] = m_dEditZigCenterCamZ;

	THEAPP.m_pModelDataManager->m_bUseZigCenterPos[THEAPP.m_iCurStageIndex] = m_bCheckUseZigCenterPos;
	THEAPP.m_pModelDataManager->m_iAfZigCenterX[THEAPP.m_iCurStageIndex] = m_iEditZigCenterX;
	THEAPP.m_pModelDataManager->m_iAfZigCenterY[THEAPP.m_iCurStageIndex] = m_iEditZigCenterY;

	THEAPP.m_pModelDataManager->m_iAFZigCenterLightPage[THEAPP.m_iCurStageIndex] = m_cbAFZigCenterLightPage.GetCurSel();

	THEAPP.m_pModelDataManager->m_dAfZigRotationCenterX[THEAPP.m_iCurStageIndex] = m_dEditAfZigRotationCenterX;
	THEAPP.m_pModelDataManager->m_dAfZigRotationCenterY[THEAPP.m_iCurStageIndex] = m_dEditAfZigRotationCenterY;

	THEAPP.m_pModelDataManager->m_iAfLeftTopCornerPointX[THEAPP.m_iCurStageIndex] = m_iEditAfLeftTopCornerX;
	THEAPP.m_pModelDataManager->m_iAfLeftTopCornerPointY[THEAPP.m_iCurStageIndex] = m_iEditAfLeftTopCornerY;

	THEAPP.m_pModelDataManager->m_iAfLensStartImageNo = m_iEditAfLensStartImageNo;
	THEAPP.m_pModelDataManager->m_iAfLensEndImageNo = m_iEditAfLensEndImageNo;

	THEAPP.m_pModelDataManager->m_iAfModuleRotationAngleImageNo = m_iEditAfModuleRotationAngleImageNo;

	THEAPP.m_pModelDataManager->m_iAfEastSideStartImageNo = m_iEditAfEastSideStartImageNo;
	THEAPP.m_pModelDataManager->m_iAfEastSideEndImageNo = m_iEditAfEastSideEndImageNo;

	THEAPP.m_pModelDataManager->m_iAfWestSideStartImageNo = m_iEditAfWestSideStartImageNo;
	THEAPP.m_pModelDataManager->m_iAfWestSideEndImageNo = m_iEditAfWestSideEndImageNo;

	THEAPP.m_pModelDataManager->m_iAfNorthSideStartImageNo = m_iEditAfNorthSideStartImageNo;
	THEAPP.m_pModelDataManager->m_iAfNorthSideEndImageNo = m_iEditAfNorthSideEndImageNo;

	THEAPP.m_pModelDataManager->m_iAfSouthSideStartImageNo = m_iEditAfSouthSideStartImageNo;
	THEAPP.m_pModelDataManager->m_iAfSouthSideEndImageNo = m_iEditAfSouthSideEndImageNo;

	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	CString strAutoFocus;
	strAutoFocus.Format("%s\\HW\\Vision_N%d\\AutoFocus.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);
	THEAPP.m_pModelDataManager->SaveAutoFocus(strAutoFocus, THEAPP.m_iCurStageIndex);

	UpdateData(FALSE);
}

void CAutoFocusDlg::UpdateParam()
{
	int iStageIndex = THEAPP.m_iCurStageIndex;

	m_dEditZigCenterCamZ = THEAPP.m_pModelDataManager->m_dZigCenterCamZ[iStageIndex];
	m_bCheckUseZigCenterPos = THEAPP.m_pModelDataManager->m_bUseZigCenterPos[iStageIndex];
	m_iEditZigCenterX = THEAPP.m_pModelDataManager->m_iAfZigCenterX[iStageIndex];
	m_iEditZigCenterY = THEAPP.m_pModelDataManager->m_iAfZigCenterY[iStageIndex];
	m_cbAFZigCenterLightPage.SetCurSel(THEAPP.m_pModelDataManager->m_iAFZigCenterLightPage[iStageIndex]);
	m_dEditAfZigRotationCenterX = THEAPP.m_pModelDataManager->m_dAfZigRotationCenterX[iStageIndex];
	m_dEditAfZigRotationCenterY = THEAPP.m_pModelDataManager->m_dAfZigRotationCenterY[iStageIndex];
	m_iEditAfLeftTopCornerX = THEAPP.m_pModelDataManager->m_iAfLeftTopCornerPointX[iStageIndex];
	m_iEditAfLeftTopCornerY = THEAPP.m_pModelDataManager->m_iAfLeftTopCornerPointY[iStageIndex];

	m_iEditAfLensStartImageNo = THEAPP.m_pModelDataManager->m_iAfLensStartImageNo;
	m_iEditAfLensEndImageNo = THEAPP.m_pModelDataManager->m_iAfLensEndImageNo;
	m_iEditAfModuleRotationAngleImageNo = THEAPP.m_pModelDataManager->m_iAfModuleRotationAngleImageNo;
	m_iEditAfEastSideStartImageNo = THEAPP.m_pModelDataManager->m_iAfEastSideStartImageNo;
	m_iEditAfEastSideEndImageNo = THEAPP.m_pModelDataManager->m_iAfEastSideEndImageNo;
	m_iEditAfWestSideStartImageNo = THEAPP.m_pModelDataManager->m_iAfWestSideStartImageNo;
	m_iEditAfWestSideEndImageNo = THEAPP.m_pModelDataManager->m_iAfWestSideEndImageNo;
	m_iEditAfNorthSideStartImageNo = THEAPP.m_pModelDataManager->m_iAfNorthSideStartImageNo;
	m_iEditAfNorthSideEndImageNo = THEAPP.m_pModelDataManager->m_iAfNorthSideEndImageNo;
	m_iEditAfSouthSideStartImageNo = THEAPP.m_pModelDataManager->m_iAfSouthSideStartImageNo;
	m_iEditAfSouthSideEndImageNo = THEAPP.m_pModelDataManager->m_iAfSouthSideEndImageNo;

	CString strVisionName = _T("");
	strVisionName = g_strInspectionTypeName_Short[THEAPP.m_iCurVisionCamType].c_str();
	CString strEquipModel = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
	if (strVisionName == "Top" &&
		(strEquipModel == "AKC" ||
		 strEquipModel == "CHS-W" || strEquipModel == "CHS-Z" ||
		 strEquipModel == "CHS-K" || strEquipModel == "CHS-S" ||
		 strEquipModel == "CHS-T" || strEquipModel == "CHS-V"))
	{
		GetDlgItem(IDC_EDIT_AF_LENS_START_IMAGE_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_AF_LENS_END_IMAGE_NO)->EnableWindow(TRUE);
	}
	else
	{
		THEAPP.m_pModelDataManager->m_iAfLensStartImageNo = 0;
		m_iEditAfLensStartImageNo = THEAPP.m_pModelDataManager->m_iAfLensStartImageNo;

		THEAPP.m_pModelDataManager->m_iAfLensEndImageNo = 0;
		m_iEditAfLensEndImageNo = THEAPP.m_pModelDataManager->m_iAfLensEndImageNo;

		GetDlgItem(IDC_EDIT_AF_LENS_START_IMAGE_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AF_LENS_END_IMAGE_NO)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}
