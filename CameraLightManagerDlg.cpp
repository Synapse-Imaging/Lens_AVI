// CameraLightManagerDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "CameraLightManagerDlg.h"
#include "afxdialogex.h"


// CCameraLightManagerDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCameraLightManagerDlg, CDialog)

CCameraLightManagerDlg* CCameraLightManagerDlg::m_pInstance = NULL;

CCameraLightManagerDlg* CCameraLightManagerDlg::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CCameraLightManagerDlg;

		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

			m_pInstance->Create(IDD_CAMERA_LIGHT_MANAGER_DLG, pFrame->GetActiveView());
		}
	}

	return m_pInstance;
}

void CCameraLightManagerDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

void CCameraLightManagerDlg::Show()
{
	SetWindowPos(&wndTopMost, 200, 100, 0, 0, SWP_NOSIZE);
	ShowWindow(SW_SHOW);
}

void CCameraLightManagerDlg::Hide()
{
	ShowWindow(SW_HIDE);
}

CCameraLightManagerDlg::CCameraLightManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraLightManagerDlg::IDD, pParent)
{
	m_iRadioLastInspectionImageIndex = 0;
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_bCheckMatchingImage[i] = FALSE;

		m_dEditImageLightZPosRef = MOTION_NOUSE;
		m_dEditImageTiltPosRef = MOTION_NOUSE;
		m_dEditImageRotatePosRef = MOTION_NOUSE;
		m_dEditImageStageXPosRef = MOTION_NOUSE;
		m_dEditImageStageYPosRef = MOTION_NOUSE;

		m_dEditImageZPos[i] = 5.0;
		m_dEditImageLightZPos[i] = 0.0;
		m_dEditImageTiltPos[i] = 0.0;
		m_dEditImageRotatePos[i] = 0.0;
		m_dEditImageStageXPos[i] = 0.0;
		m_dEditImageStageYPos[i] = 0.0;

		m_bCheckDiffusedImage[i] = FALSE;
		m_bCheckSpecularImage[i] = FALSE;

		m_bCheckSeqStartImage[i] = FALSE;

	}
}

CCameraLightManagerDlg::~CCameraLightManagerDlg()
{
}

void CCameraLightManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_CHANGE_LIGHTVALUE, m_bnChangeLight);
	DDX_Control(pDX, IDC_BUTTON_GRAB_SEQUENCE, m_bnGrabSequence);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_SPECULAR, m_bnChangeSpecular);
	DDX_Control(pDX, IDC_BUTTON_LOAD_CAL_DATA, m_bnLoadCalData);
	DDX_Control(pDX, IDC_BUTTON_SAVE_POSITION_SETTING, m_bnSavePositionSetting);
	DDX_Control(pDX, IDC_BUTTON_SAVE_LIGHT_SETTING, m_bnSaveLightSetting);
	DDX_Control(pDX, IDC_BUTTON_SAVE_POSITION_LIGHT_SETTING, m_bnSavePositionLightSetting);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);

	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_0, m_dEditImageLightZPosRef);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_0, m_dEditImageTiltPosRef);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_0, m_dEditImageRotatePosRef);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_0, m_dEditImageStageXPosRef);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_0, m_dEditImageStageYPosRef);

	DDX_Radio(pDX, IDC_RADIO_IMAGE_1, m_iRadioLastInspectionImageIndex);

	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_1, m_dEditImageZPos[0]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_2, m_dEditImageZPos[1]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_3, m_dEditImageZPos[2]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_4, m_dEditImageZPos[3]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_5, m_dEditImageZPos[4]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_6, m_dEditImageZPos[5]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_7, m_dEditImageZPos[6]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_8, m_dEditImageZPos[7]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_9, m_dEditImageZPos[8]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_10, m_dEditImageZPos[9]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_11, m_dEditImageZPos[10]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_12, m_dEditImageZPos[11]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_13, m_dEditImageZPos[12]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_14, m_dEditImageZPos[13]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_15, m_dEditImageZPos[14]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_16, m_dEditImageZPos[15]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_17, m_dEditImageZPos[16]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_18, m_dEditImageZPos[17]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_19, m_dEditImageZPos[18]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_20, m_dEditImageZPos[19]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_21, m_dEditImageZPos[20]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_22, m_dEditImageZPos[21]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_23, m_dEditImageZPos[22]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_24, m_dEditImageZPos[23]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_25, m_dEditImageZPos[24]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_26, m_dEditImageZPos[25]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_27, m_dEditImageZPos[26]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_28, m_dEditImageZPos[27]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_29, m_dEditImageZPos[28]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_30, m_dEditImageZPos[29]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_31, m_dEditImageZPos[30]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_32, m_dEditImageZPos[31]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_33, m_dEditImageZPos[32]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_34, m_dEditImageZPos[33]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_35, m_dEditImageZPos[34]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_36, m_dEditImageZPos[35]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_37, m_dEditImageZPos[36]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_38, m_dEditImageZPos[37]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_39, m_dEditImageZPos[38]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_40, m_dEditImageZPos[39]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_41, m_dEditImageZPos[40]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_42, m_dEditImageZPos[41]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_43, m_dEditImageZPos[42]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_44, m_dEditImageZPos[43]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ZPOS_45, m_dEditImageZPos[44]);

	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_1, m_dEditImageLightZPos[0]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_2, m_dEditImageLightZPos[1]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_3, m_dEditImageLightZPos[2]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_4, m_dEditImageLightZPos[3]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_5, m_dEditImageLightZPos[4]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_6, m_dEditImageLightZPos[5]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_7, m_dEditImageLightZPos[6]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_8, m_dEditImageLightZPos[7]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_9, m_dEditImageLightZPos[8]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_10, m_dEditImageLightZPos[9]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_11, m_dEditImageLightZPos[10]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_12, m_dEditImageLightZPos[11]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_13, m_dEditImageLightZPos[12]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_14, m_dEditImageLightZPos[13]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_15, m_dEditImageLightZPos[14]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_16, m_dEditImageLightZPos[15]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_17, m_dEditImageLightZPos[16]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_18, m_dEditImageLightZPos[17]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_19, m_dEditImageLightZPos[18]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_20, m_dEditImageLightZPos[19]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_21, m_dEditImageLightZPos[20]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_22, m_dEditImageLightZPos[21]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_23, m_dEditImageLightZPos[22]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_24, m_dEditImageLightZPos[23]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_25, m_dEditImageLightZPos[24]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_26, m_dEditImageLightZPos[25]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_27, m_dEditImageLightZPos[26]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_28, m_dEditImageLightZPos[27]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_29, m_dEditImageLightZPos[28]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_30, m_dEditImageLightZPos[29]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_31, m_dEditImageLightZPos[30]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_32, m_dEditImageLightZPos[31]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_33, m_dEditImageLightZPos[32]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_34, m_dEditImageLightZPos[33]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_35, m_dEditImageLightZPos[34]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_36, m_dEditImageLightZPos[35]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_37, m_dEditImageLightZPos[36]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_38, m_dEditImageLightZPos[37]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_39, m_dEditImageLightZPos[38]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_40, m_dEditImageLightZPos[39]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_41, m_dEditImageLightZPos[40]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_42, m_dEditImageLightZPos[41]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_43, m_dEditImageLightZPos[42]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_44, m_dEditImageLightZPos[43]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_LIGHT_Z_45, m_dEditImageLightZPos[44]);

	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_1, m_dEditImageTiltPos[0]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_2, m_dEditImageTiltPos[1]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_3, m_dEditImageTiltPos[2]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_4, m_dEditImageTiltPos[3]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_5, m_dEditImageTiltPos[4]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_6, m_dEditImageTiltPos[5]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_7, m_dEditImageTiltPos[6]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_8, m_dEditImageTiltPos[7]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_9, m_dEditImageTiltPos[8]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_10, m_dEditImageTiltPos[9]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_11, m_dEditImageTiltPos[10]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_12, m_dEditImageTiltPos[11]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_13, m_dEditImageTiltPos[12]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_14, m_dEditImageTiltPos[13]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_15, m_dEditImageTiltPos[14]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_16, m_dEditImageTiltPos[15]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_17, m_dEditImageTiltPos[16]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_18, m_dEditImageTiltPos[17]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_19, m_dEditImageTiltPos[18]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_20, m_dEditImageTiltPos[19]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_21, m_dEditImageTiltPos[20]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_22, m_dEditImageTiltPos[21]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_23, m_dEditImageTiltPos[22]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_24, m_dEditImageTiltPos[23]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_25, m_dEditImageTiltPos[24]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_26, m_dEditImageTiltPos[25]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_27, m_dEditImageTiltPos[26]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_28, m_dEditImageTiltPos[27]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_29, m_dEditImageTiltPos[28]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_30, m_dEditImageTiltPos[29]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_31, m_dEditImageTiltPos[30]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_32, m_dEditImageTiltPos[31]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_33, m_dEditImageTiltPos[32]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_34, m_dEditImageTiltPos[33]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_35, m_dEditImageTiltPos[34]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_36, m_dEditImageTiltPos[35]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_37, m_dEditImageTiltPos[36]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_38, m_dEditImageTiltPos[37]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_39, m_dEditImageTiltPos[38]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_40, m_dEditImageTiltPos[39]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_41, m_dEditImageTiltPos[40]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_42, m_dEditImageTiltPos[41]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_43, m_dEditImageTiltPos[42]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_44, m_dEditImageTiltPos[43]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_TILT_45, m_dEditImageTiltPos[44]);

	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_1, m_dEditImageRotatePos[0]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_2, m_dEditImageRotatePos[1]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_3, m_dEditImageRotatePos[2]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_4, m_dEditImageRotatePos[3]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_5, m_dEditImageRotatePos[4]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_6, m_dEditImageRotatePos[5]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_7, m_dEditImageRotatePos[6]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_8, m_dEditImageRotatePos[7]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_9, m_dEditImageRotatePos[8]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_10, m_dEditImageRotatePos[9]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_11, m_dEditImageRotatePos[10]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_12, m_dEditImageRotatePos[11]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_13, m_dEditImageRotatePos[12]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_14, m_dEditImageRotatePos[13]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_15, m_dEditImageRotatePos[14]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_16, m_dEditImageRotatePos[15]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_17, m_dEditImageRotatePos[16]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_18, m_dEditImageRotatePos[17]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_19, m_dEditImageRotatePos[18]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_20, m_dEditImageRotatePos[19]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_21, m_dEditImageRotatePos[20]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_22, m_dEditImageRotatePos[21]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_23, m_dEditImageRotatePos[22]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_24, m_dEditImageRotatePos[23]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_25, m_dEditImageRotatePos[24]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_26, m_dEditImageRotatePos[25]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_27, m_dEditImageRotatePos[26]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_28, m_dEditImageRotatePos[27]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_29, m_dEditImageRotatePos[28]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_30, m_dEditImageRotatePos[29]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_31, m_dEditImageRotatePos[30]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_32, m_dEditImageRotatePos[31]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_33, m_dEditImageRotatePos[32]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_34, m_dEditImageRotatePos[33]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_35, m_dEditImageRotatePos[34]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_36, m_dEditImageRotatePos[35]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_37, m_dEditImageRotatePos[36]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_38, m_dEditImageRotatePos[37]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_39, m_dEditImageRotatePos[38]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_40, m_dEditImageRotatePos[39]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_41, m_dEditImageRotatePos[40]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_42, m_dEditImageRotatePos[41]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_43, m_dEditImageRotatePos[42]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_44, m_dEditImageRotatePos[43]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_ROTATE_45, m_dEditImageRotatePos[44]);

	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_1, m_dEditImageStageXPos[0]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_2, m_dEditImageStageXPos[1]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_3, m_dEditImageStageXPos[2]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_4, m_dEditImageStageXPos[3]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_5, m_dEditImageStageXPos[4]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_6, m_dEditImageStageXPos[5]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_7, m_dEditImageStageXPos[6]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_8, m_dEditImageStageXPos[7]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_9, m_dEditImageStageXPos[8]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_10, m_dEditImageStageXPos[9]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_11, m_dEditImageStageXPos[10]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_12, m_dEditImageStageXPos[11]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_13, m_dEditImageStageXPos[12]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_14, m_dEditImageStageXPos[13]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_15, m_dEditImageStageXPos[14]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_16, m_dEditImageStageXPos[15]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_17, m_dEditImageStageXPos[16]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_18, m_dEditImageStageXPos[17]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_19, m_dEditImageStageXPos[18]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_20, m_dEditImageStageXPos[19]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_21, m_dEditImageStageXPos[20]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_22, m_dEditImageStageXPos[21]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_23, m_dEditImageStageXPos[22]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_24, m_dEditImageStageXPos[23]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_25, m_dEditImageStageXPos[24]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_26, m_dEditImageStageXPos[25]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_27, m_dEditImageStageXPos[26]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_28, m_dEditImageStageXPos[27]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_29, m_dEditImageStageXPos[28]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_30, m_dEditImageStageXPos[29]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_31, m_dEditImageStageXPos[30]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_32, m_dEditImageStageXPos[31]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_33, m_dEditImageStageXPos[32]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_34, m_dEditImageStageXPos[33]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_35, m_dEditImageStageXPos[34]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_36, m_dEditImageStageXPos[35]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_37, m_dEditImageStageXPos[36]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_38, m_dEditImageStageXPos[37]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_39, m_dEditImageStageXPos[38]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_40, m_dEditImageStageXPos[39]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_41, m_dEditImageStageXPos[40]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_42, m_dEditImageStageXPos[41]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_43, m_dEditImageStageXPos[42]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_44, m_dEditImageStageXPos[43]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_X_45, m_dEditImageStageXPos[44]);

	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_1, m_dEditImageStageYPos[0]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_2, m_dEditImageStageYPos[1]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_3, m_dEditImageStageYPos[2]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_4, m_dEditImageStageYPos[3]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_5, m_dEditImageStageYPos[4]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_6, m_dEditImageStageYPos[5]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_7, m_dEditImageStageYPos[6]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_8, m_dEditImageStageYPos[7]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_9, m_dEditImageStageYPos[8]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_10, m_dEditImageStageYPos[9]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_11, m_dEditImageStageYPos[10]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_12, m_dEditImageStageYPos[11]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_13, m_dEditImageStageYPos[12]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_14, m_dEditImageStageYPos[13]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_15, m_dEditImageStageYPos[14]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_16, m_dEditImageStageYPos[15]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_17, m_dEditImageStageYPos[16]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_18, m_dEditImageStageYPos[17]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_19, m_dEditImageStageYPos[18]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_20, m_dEditImageStageYPos[19]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_21, m_dEditImageStageYPos[20]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_22, m_dEditImageStageYPos[21]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_23, m_dEditImageStageYPos[22]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_24, m_dEditImageStageYPos[23]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_25, m_dEditImageStageYPos[24]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_26, m_dEditImageStageYPos[25]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_27, m_dEditImageStageYPos[26]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_28, m_dEditImageStageYPos[27]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_29, m_dEditImageStageYPos[28]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_30, m_dEditImageStageYPos[29]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_31, m_dEditImageStageYPos[30]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_32, m_dEditImageStageYPos[31]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_33, m_dEditImageStageYPos[32]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_34, m_dEditImageStageYPos[33]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_35, m_dEditImageStageYPos[34]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_36, m_dEditImageStageYPos[35]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_37, m_dEditImageStageYPos[36]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_38, m_dEditImageStageYPos[37]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_39, m_dEditImageStageYPos[38]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_40, m_dEditImageStageYPos[39]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_41, m_dEditImageStageYPos[40]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_42, m_dEditImageStageYPos[41]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_43, m_dEditImageStageYPos[42]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_44, m_dEditImageStageYPos[43]);
	DDX_Text(pDX, IDC_EDIT_IMAGE_STAGE_Y_45, m_dEditImageStageYPos[44]);

	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_1, m_bCheckMatchingImage[0]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_2, m_bCheckMatchingImage[1]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_3, m_bCheckMatchingImage[2]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_4, m_bCheckMatchingImage[3]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_5, m_bCheckMatchingImage[4]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_6, m_bCheckMatchingImage[5]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_7, m_bCheckMatchingImage[6]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_8, m_bCheckMatchingImage[7]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_9, m_bCheckMatchingImage[8]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_10, m_bCheckMatchingImage[9]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_11, m_bCheckMatchingImage[10]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_12, m_bCheckMatchingImage[11]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_13, m_bCheckMatchingImage[12]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_14, m_bCheckMatchingImage[13]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_15, m_bCheckMatchingImage[14]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_16, m_bCheckMatchingImage[15]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_17, m_bCheckMatchingImage[16]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_18, m_bCheckMatchingImage[17]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_19, m_bCheckMatchingImage[18]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_20, m_bCheckMatchingImage[19]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_21, m_bCheckMatchingImage[20]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_22, m_bCheckMatchingImage[21]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_23, m_bCheckMatchingImage[22]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_24, m_bCheckMatchingImage[23]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_25, m_bCheckMatchingImage[24]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_26, m_bCheckMatchingImage[25]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_27, m_bCheckMatchingImage[26]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_28, m_bCheckMatchingImage[27]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_29, m_bCheckMatchingImage[28]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_30, m_bCheckMatchingImage[29]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_31, m_bCheckMatchingImage[30]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_32, m_bCheckMatchingImage[31]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_33, m_bCheckMatchingImage[32]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_34, m_bCheckMatchingImage[33]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_35, m_bCheckMatchingImage[34]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_36, m_bCheckMatchingImage[35]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_37, m_bCheckMatchingImage[36]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_38, m_bCheckMatchingImage[37]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_39, m_bCheckMatchingImage[38]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_40, m_bCheckMatchingImage[39]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_41, m_bCheckMatchingImage[40]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_42, m_bCheckMatchingImage[41]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_43, m_bCheckMatchingImage[42]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_44, m_bCheckMatchingImage[43]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_45, m_bCheckMatchingImage[44]);

	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_1, m_bCheckDiffusedImage[0]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_2, m_bCheckDiffusedImage[1]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_3, m_bCheckDiffusedImage[2]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_4, m_bCheckDiffusedImage[3]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_5, m_bCheckDiffusedImage[4]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_6, m_bCheckDiffusedImage[5]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_7, m_bCheckDiffusedImage[6]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_8, m_bCheckDiffusedImage[7]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_9, m_bCheckDiffusedImage[8]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_10, m_bCheckDiffusedImage[9]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_11, m_bCheckDiffusedImage[10]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_12, m_bCheckDiffusedImage[11]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_13, m_bCheckDiffusedImage[12]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_14, m_bCheckDiffusedImage[13]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_15, m_bCheckDiffusedImage[14]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_16, m_bCheckDiffusedImage[15]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_17, m_bCheckDiffusedImage[16]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_18, m_bCheckDiffusedImage[17]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_19, m_bCheckDiffusedImage[18]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_20, m_bCheckDiffusedImage[19]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_21, m_bCheckDiffusedImage[20]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_22, m_bCheckDiffusedImage[21]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_23, m_bCheckDiffusedImage[22]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_24, m_bCheckDiffusedImage[23]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_25, m_bCheckDiffusedImage[24]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_26, m_bCheckDiffusedImage[25]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_27, m_bCheckDiffusedImage[26]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_28, m_bCheckDiffusedImage[27]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_29, m_bCheckDiffusedImage[28]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_30, m_bCheckDiffusedImage[29]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_31, m_bCheckDiffusedImage[30]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_32, m_bCheckDiffusedImage[31]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_33, m_bCheckDiffusedImage[32]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_34, m_bCheckDiffusedImage[33]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_35, m_bCheckDiffusedImage[34]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_36, m_bCheckDiffusedImage[35]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_37, m_bCheckDiffusedImage[36]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_38, m_bCheckDiffusedImage[37]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_39, m_bCheckDiffusedImage[38]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_40, m_bCheckDiffusedImage[39]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_41, m_bCheckDiffusedImage[40]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_42, m_bCheckDiffusedImage[41]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_43, m_bCheckDiffusedImage[42]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_44, m_bCheckDiffusedImage[43]);
	DDX_Check(pDX, IDC_CHECK_DIFFUSED_IMAGE_45, m_bCheckDiffusedImage[44]);

	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_1, m_bCheckSpecularImage[0]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_2, m_bCheckSpecularImage[1]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_3, m_bCheckSpecularImage[2]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_4, m_bCheckSpecularImage[3]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_5, m_bCheckSpecularImage[4]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_6, m_bCheckSpecularImage[5]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_7, m_bCheckSpecularImage[6]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_8, m_bCheckSpecularImage[7]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_9, m_bCheckSpecularImage[8]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_10, m_bCheckSpecularImage[9]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_11, m_bCheckSpecularImage[10]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_12, m_bCheckSpecularImage[11]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_13, m_bCheckSpecularImage[12]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_14, m_bCheckSpecularImage[13]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_15, m_bCheckSpecularImage[14]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_16, m_bCheckSpecularImage[15]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_17, m_bCheckSpecularImage[16]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_18, m_bCheckSpecularImage[17]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_19, m_bCheckSpecularImage[18]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_20, m_bCheckSpecularImage[19]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_21, m_bCheckSpecularImage[20]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_22, m_bCheckSpecularImage[21]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_23, m_bCheckSpecularImage[22]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_24, m_bCheckSpecularImage[23]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_25, m_bCheckSpecularImage[24]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_26, m_bCheckSpecularImage[25]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_27, m_bCheckSpecularImage[26]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_28, m_bCheckSpecularImage[27]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_29, m_bCheckSpecularImage[28]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_30, m_bCheckSpecularImage[29]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_31, m_bCheckSpecularImage[30]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_32, m_bCheckSpecularImage[31]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_33, m_bCheckSpecularImage[32]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_34, m_bCheckSpecularImage[33]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_35, m_bCheckSpecularImage[34]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_36, m_bCheckSpecularImage[35]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_37, m_bCheckSpecularImage[36]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_38, m_bCheckSpecularImage[37]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_39, m_bCheckSpecularImage[38]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_40, m_bCheckSpecularImage[39]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_41, m_bCheckSpecularImage[40]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_42, m_bCheckSpecularImage[41]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_43, m_bCheckSpecularImage[42]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_44, m_bCheckSpecularImage[43]);
	DDX_Check(pDX, IDC_CHECK_SPECULAR_IMAGE_45, m_bCheckSpecularImage[44]);

	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_1, m_bCheckSeqStartImage[0]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_2, m_bCheckSeqStartImage[1]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_3, m_bCheckSeqStartImage[2]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_4, m_bCheckSeqStartImage[3]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_5, m_bCheckSeqStartImage[4]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_6, m_bCheckSeqStartImage[5]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_7, m_bCheckSeqStartImage[6]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_8, m_bCheckSeqStartImage[7]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_9, m_bCheckSeqStartImage[8]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_10, m_bCheckSeqStartImage[9]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_11, m_bCheckSeqStartImage[10]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_12, m_bCheckSeqStartImage[11]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_13, m_bCheckSeqStartImage[12]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_14, m_bCheckSeqStartImage[13]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_15, m_bCheckSeqStartImage[14]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_16, m_bCheckSeqStartImage[15]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_17, m_bCheckSeqStartImage[16]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_18, m_bCheckSeqStartImage[17]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_19, m_bCheckSeqStartImage[18]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_20, m_bCheckSeqStartImage[19]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_21, m_bCheckSeqStartImage[20]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_22, m_bCheckSeqStartImage[21]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_23, m_bCheckSeqStartImage[22]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_24, m_bCheckSeqStartImage[23]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_25, m_bCheckSeqStartImage[24]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_26, m_bCheckSeqStartImage[25]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_27, m_bCheckSeqStartImage[26]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_28, m_bCheckSeqStartImage[27]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_29, m_bCheckSeqStartImage[28]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_30, m_bCheckSeqStartImage[29]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_31, m_bCheckSeqStartImage[30]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_32, m_bCheckSeqStartImage[31]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_33, m_bCheckSeqStartImage[32]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_34, m_bCheckSeqStartImage[33]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_35, m_bCheckSeqStartImage[34]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_36, m_bCheckSeqStartImage[35]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_37, m_bCheckSeqStartImage[36]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_38, m_bCheckSeqStartImage[37]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_39, m_bCheckSeqStartImage[38]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_40, m_bCheckSeqStartImage[39]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_41, m_bCheckSeqStartImage[40]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_42, m_bCheckSeqStartImage[41]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_43, m_bCheckSeqStartImage[42]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_44, m_bCheckSeqStartImage[43]);
	DDX_Check(pDX, IDC_CHECK_SEQ_START_IMAGE_45, m_bCheckSeqStartImage[44]);

	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_1, m_cbSeqAddrIndex[0]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_2, m_cbSeqAddrIndex[1]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_3, m_cbSeqAddrIndex[2]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_4, m_cbSeqAddrIndex[3]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_5, m_cbSeqAddrIndex[4]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_6, m_cbSeqAddrIndex[5]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_7, m_cbSeqAddrIndex[6]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_8, m_cbSeqAddrIndex[7]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_9, m_cbSeqAddrIndex[8]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_10, m_cbSeqAddrIndex[9]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_11, m_cbSeqAddrIndex[10]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_12, m_cbSeqAddrIndex[11]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_13, m_cbSeqAddrIndex[12]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_14, m_cbSeqAddrIndex[13]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_15, m_cbSeqAddrIndex[14]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_16, m_cbSeqAddrIndex[15]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_17, m_cbSeqAddrIndex[16]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_18, m_cbSeqAddrIndex[17]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_19, m_cbSeqAddrIndex[18]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_20, m_cbSeqAddrIndex[19]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_21, m_cbSeqAddrIndex[20]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_22, m_cbSeqAddrIndex[21]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_23, m_cbSeqAddrIndex[22]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_24, m_cbSeqAddrIndex[23]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_25, m_cbSeqAddrIndex[24]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_26, m_cbSeqAddrIndex[25]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_27, m_cbSeqAddrIndex[26]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_28, m_cbSeqAddrIndex[27]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_29, m_cbSeqAddrIndex[28]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_30, m_cbSeqAddrIndex[29]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_31, m_cbSeqAddrIndex[30]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_32, m_cbSeqAddrIndex[31]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_33, m_cbSeqAddrIndex[32]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_34, m_cbSeqAddrIndex[33]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_35, m_cbSeqAddrIndex[34]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_36, m_cbSeqAddrIndex[35]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_37, m_cbSeqAddrIndex[36]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_38, m_cbSeqAddrIndex[37]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_39, m_cbSeqAddrIndex[38]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_40, m_cbSeqAddrIndex[39]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_41, m_cbSeqAddrIndex[40]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_42, m_cbSeqAddrIndex[41]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_43, m_cbSeqAddrIndex[42]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_44, m_cbSeqAddrIndex[43]);
	DDX_Control(pDX, IDC_COMBO_SEQ_ADDR_INDEX_45, m_cbSeqAddrIndex[44]);

	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_1, m_cbPageIndex[0]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_2, m_cbPageIndex[1]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_3, m_cbPageIndex[2]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_4, m_cbPageIndex[3]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_5, m_cbPageIndex[4]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_6, m_cbPageIndex[5]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_7, m_cbPageIndex[6]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_8, m_cbPageIndex[7]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_9, m_cbPageIndex[8]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_10, m_cbPageIndex[9]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_11, m_cbPageIndex[10]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_12, m_cbPageIndex[11]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_13, m_cbPageIndex[12]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_14, m_cbPageIndex[13]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_15, m_cbPageIndex[14]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_16, m_cbPageIndex[15]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_17, m_cbPageIndex[16]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_18, m_cbPageIndex[17]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_19, m_cbPageIndex[18]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_20, m_cbPageIndex[19]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_21, m_cbPageIndex[20]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_22, m_cbPageIndex[21]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_23, m_cbPageIndex[22]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_24, m_cbPageIndex[23]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_25, m_cbPageIndex[24]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_26, m_cbPageIndex[25]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_27, m_cbPageIndex[26]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_28, m_cbPageIndex[27]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_29, m_cbPageIndex[28]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_30, m_cbPageIndex[29]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_31, m_cbPageIndex[30]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_32, m_cbPageIndex[31]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_33, m_cbPageIndex[32]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_34, m_cbPageIndex[33]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_35, m_cbPageIndex[34]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_36, m_cbPageIndex[35]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_37, m_cbPageIndex[36]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_38, m_cbPageIndex[37]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_39, m_cbPageIndex[38]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_40, m_cbPageIndex[39]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_41, m_cbPageIndex[40]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_42, m_cbPageIndex[41]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_43, m_cbPageIndex[42]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_44, m_cbPageIndex[43]);
	DDX_Control(pDX, IDC_COMBO_PAGE_INDEX_45, m_cbPageIndex[44]);
}


BEGIN_MESSAGE_MAP(CCameraLightManagerDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_LIGHTVALUE, &CCameraLightManagerDlg::OnBnClickedButtonChangeLightvalue)
	ON_BN_CLICKED(IDC_BUTTON_GRAB_SEQUENCE, &CCameraLightManagerDlg::OnBnClickedButtonGrabSequence)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_SPECULAR, &CCameraLightManagerDlg::OnBnClickedButtonChangeSpecular)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_CAL_DATA, &CCameraLightManagerDlg::OnBnClickedButtonLoadCalData)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_POSITION_SETTING, &CCameraLightManagerDlg::OnBnClickedButtonSavePositionSetting)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_LIGHT_SETTING, &CCameraLightManagerDlg::OnBnClickedButtonSaveLightSetting)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_POSITION_LIGHT_SETTING, &CCameraLightManagerDlg::OnBnClickedButtonSavePositionLightSetting)
	ON_BN_CLICKED(IDCANCEL, &CCameraLightManagerDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_1, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage1)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_2, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage2)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_3, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage3)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_4, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage4)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_5, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage5)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_6, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage6)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_7, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage7)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_8, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage8)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_9, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage9)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_10, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage10)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_11, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage11)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_12, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage12)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_13, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage13)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_14, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage14)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_15, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage15)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_16, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage16)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_17, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage17)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_18, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage18)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_19, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage19)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_20, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage10)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_21, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage21)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_22, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage22)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_23, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage23)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_24, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage24)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_25, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage25)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_26, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage26)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_27, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage27)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_28, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage28)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_29, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage29)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_30, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage30)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_31, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage31)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_32, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage32)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_33, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage33)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_34, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage34)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_35, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage35)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_36, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage36)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_37, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage37)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_38, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage38)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_39, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage39)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_40, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage40)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_41, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage41)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_42, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage42)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_43, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage43)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_44, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage44)
	ON_BN_CLICKED(IDC_CHECK_DIFFUSED_IMAGE_45, &CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage45)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_1, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage1)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_2, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage2)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_3, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage3)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_4, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage4)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_5, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage5)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_6, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage6)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_7, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage7)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_8, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage8)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_9, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage9)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_10, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage10)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_11, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage11)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_12, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage12)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_13, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage13)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_14, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage14)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_15, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage15)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_16, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage16)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_17, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage17)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_18, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage18)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_19, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage19)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_20, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage20)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_21, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage21)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_22, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage22)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_23, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage23)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_24, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage24)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_25, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage25)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_26, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage26)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_27, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage27)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_28, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage28)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_29, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage29)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_30, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage30)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_31, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage31)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_32, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage32)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_33, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage33)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_34, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage34)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_35, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage35)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_36, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage36)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_37, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage37)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_38, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage38)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_39, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage39)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_40, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage40)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_41, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage41)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_42, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage42)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_43, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage43)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_44, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage44)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR_IMAGE_45, &CCameraLightManagerDlg::OnBnClickedCheckSpecularImage45)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_1, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage1)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_2, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage2)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_3, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage3)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_4, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage4)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_5, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage5)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_6, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage6)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_7, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage7)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_8, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage8)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_9, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage9)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_10, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage10)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_11, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage11)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_12, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage12)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_13, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage13)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_14, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage14)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_15, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage15)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_16, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage16)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_17, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage17)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_18, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage18)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_19, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage19)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_20, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage20)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_21, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage21)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_22, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage22)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_23, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage23)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_24, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage24)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_25, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage25)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_26, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage26)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_27, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage27)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_28, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage28)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_29, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage29)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_30, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage30)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_31, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage31)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_32, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage32)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_33, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage33)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_34, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage34)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_35, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage35)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_36, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage36)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_37, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage37)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_38, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage38)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_39, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage39)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_40, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage40)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_41, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage41)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_42, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage42)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_43, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage43)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_44, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage44)
	ON_BN_CLICKED(IDC_CHECK_SEQ_START_IMAGE_45, &CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage45)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_1, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex1)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_2, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex2)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_3, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex3)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_4, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex4)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_5, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex5)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_6, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex6)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_7, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex7)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_8, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex8)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_9, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex9)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_10, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex10)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_11, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex11)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_12, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex12)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_13, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex13)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_14, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex14)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_15, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex15)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_16, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex16)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_17, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex17)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_18, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex18)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_19, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex19)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_20, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex20)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_21, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex21)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_22, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex22)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_23, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex23)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_24, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex24)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_25, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex25)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_26, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex26)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_27, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex27)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_28, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex28)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_29, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex29)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_30, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex30)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_31, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex31)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_32, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex32)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_33, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex33)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_34, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex34)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_35, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex35)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_36, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex36)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_37, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex37)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_38, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex38)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_39, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex39)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_40, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex40)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_41, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex41)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_42, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex42)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_43, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex43)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_44, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex44)
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE_INDEX_45, &CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex45)
END_MESSAGE_MAP()


// CCameraLightManagerDlg 메시지 처리기입니다.


BOOL CCameraLightManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_bnChangeLight.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnGrabSequence.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnChangeSpecular.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnLoadCalData.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnSavePositionSetting.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnSaveLightSetting.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnSavePositionLightSetting.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x9C, 0x9A, 0xB6), 0, 0);
	m_bnCancel.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	CString sTemp;
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_cbSeqAddrIndex[i].ResetContent();
		for (int j = 0; j < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; j++)
		{
			sTemp.Format("%02d. ", j + 1);
			m_cbSeqAddrIndex[i].AddString(sTemp);
		}

		m_cbPageIndex[i].ResetContent();
		for (int j = 0; j < LIGHTCTRL_PAGE_COUNT; j++)
		{
			sTemp.Format("%02d. ", j + 1);
			m_cbPageIndex[i].AddString(sTemp);
		}
	}

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_cbSeqAddrIndex[i].SetCurSel(0);
		m_cbPageIndex[i].SetCurSel(0);
	}

	ChangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CCameraLightManagerDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("카메라, 조명 설정"));
		SetDlgItemText(IDC_GROUPBOX_CAMERALIGHTMANAGERDLG_1, _T("카메라 모션 기준"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_1, _T("Reference"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_2, _T("Light Z"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_3, _T("Tilt"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_4, _T("Rotate"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_5, _T("Stage-X"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_6, _T("Stage-Y"));
		SetDlgItemText(IDC_GROUPBOX_CAMERALIGHTMANAGERDLG_2, _T("검사 영상 설정"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_7, _T("Image"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_8, _T("Align"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_9, _T("Cam Z"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_10, _T("Light Z"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_11, _T("Tilt"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_12, _T("Rotate"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_13, _T("Stage-X"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_14, _T("Stage-Y"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_15, _T("DF"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_16, _T("SP"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_17, _T("SEQ"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_18, _T("SEQ Addr"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_19, _T("Page"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_20, _T("Image"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_21, _T("Align"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_22, _T("Cam Z"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_23, _T("Light Z"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_24, _T("Tilt"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_25, _T("Rotate"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_26, _T("Stage-X"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_27, _T("Stage-Y"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_28, _T("DF"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_29, _T("SP"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_30, _T("SEQ"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_31, _T("SEQ Addr"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_32, _T("Page"));
		SetDlgItemText(IDC_RADIO_IMAGE_1, _T("1"));
		SetDlgItemText(IDC_RADIO_IMAGE_2, _T("2"));
		SetDlgItemText(IDC_RADIO_IMAGE_3, _T("3"));
		SetDlgItemText(IDC_RADIO_IMAGE_4, _T("4"));
		SetDlgItemText(IDC_RADIO_IMAGE_5, _T("5"));
		SetDlgItemText(IDC_RADIO_IMAGE_6, _T("6"));
		SetDlgItemText(IDC_RADIO_IMAGE_7, _T("7"));
		SetDlgItemText(IDC_RADIO_IMAGE_8, _T("8"));
		SetDlgItemText(IDC_RADIO_IMAGE_9, _T("9"));
		SetDlgItemText(IDC_RADIO_IMAGE_10, _T("10"));
		SetDlgItemText(IDC_RADIO_IMAGE_11, _T("11"));
		SetDlgItemText(IDC_RADIO_IMAGE_12, _T("12"));
		SetDlgItemText(IDC_RADIO_IMAGE_13, _T("13"));
		SetDlgItemText(IDC_RADIO_IMAGE_14, _T("14"));
		SetDlgItemText(IDC_RADIO_IMAGE_15, _T("15"));
		SetDlgItemText(IDC_RADIO_IMAGE_16, _T("16"));
		SetDlgItemText(IDC_RADIO_IMAGE_17, _T("17"));
		SetDlgItemText(IDC_RADIO_IMAGE_18, _T("18"));
		SetDlgItemText(IDC_RADIO_IMAGE_19, _T("19"));
		SetDlgItemText(IDC_RADIO_IMAGE_20, _T("20"));
		SetDlgItemText(IDC_RADIO_IMAGE_21, _T("21"));
		SetDlgItemText(IDC_RADIO_IMAGE_22, _T("22"));
		SetDlgItemText(IDC_RADIO_IMAGE_23, _T("23"));
		SetDlgItemText(IDC_RADIO_IMAGE_24, _T("24"));
		SetDlgItemText(IDC_RADIO_IMAGE_25, _T("25"));
		SetDlgItemText(IDC_RADIO_IMAGE_26, _T("26"));
		SetDlgItemText(IDC_RADIO_IMAGE_27, _T("27"));
		SetDlgItemText(IDC_RADIO_IMAGE_28, _T("28"));
		SetDlgItemText(IDC_RADIO_IMAGE_29, _T("29"));
		SetDlgItemText(IDC_RADIO_IMAGE_30, _T("30"));
		SetDlgItemText(IDC_RADIO_IMAGE_31, _T("31"));
		SetDlgItemText(IDC_RADIO_IMAGE_32, _T("32"));
		SetDlgItemText(IDC_RADIO_IMAGE_33, _T("33"));
		SetDlgItemText(IDC_RADIO_IMAGE_34, _T("34"));
		SetDlgItemText(IDC_RADIO_IMAGE_35, _T("35"));
		SetDlgItemText(IDC_RADIO_IMAGE_36, _T("36"));
		SetDlgItemText(IDC_RADIO_IMAGE_37, _T("37"));
		SetDlgItemText(IDC_RADIO_IMAGE_38, _T("38"));
		SetDlgItemText(IDC_RADIO_IMAGE_39, _T("39"));
		SetDlgItemText(IDC_RADIO_IMAGE_40, _T("40"));
		SetDlgItemText(IDC_RADIO_IMAGE_41, _T("41"));
		SetDlgItemText(IDC_RADIO_IMAGE_42, _T("42"));
		SetDlgItemText(IDC_RADIO_IMAGE_43, _T("43"));
		SetDlgItemText(IDC_RADIO_IMAGE_44, _T("44"));
		SetDlgItemText(IDC_RADIO_IMAGE_45, _T("45"));
		SetDlgItemText(IDC_BUTTON_CHANGE_LIGHTVALUE, _T("조명 페이지 설정"));
		SetDlgItemText(IDC_BUTTON_GRAB_SEQUENCE, _T("그랩 시퀀스 설정"));
		SetDlgItemText(IDC_BUTTON_CHANGE_SPECULAR, _T("4D 파라미터 설정"));
		SetDlgItemText(IDC_BUTTON_LOAD_CAL_DATA, _T("Caldata.ini 파일 불러오기"));
		SetDlgItemText(IDC_BUTTON_SAVE_POSITION_SETTING, _T("포지션 설정 저장"));
		SetDlgItemText(IDC_BUTTON_SAVE_LIGHT_SETTING, _T("조명 설정 저장"));
		SetDlgItemText(IDC_BUTTON_SAVE_POSITION_LIGHT_SETTING, _T("전체 설정 저장"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
	}
	else
	{
		this->SetWindowText(_T("Set the camera, light"));
		SetDlgItemText(IDC_GROUPBOX_CAMERALIGHTMANAGERDLG_1, _T("Set motion reference"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_1, _T("Reference"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_2, _T("Light Z"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_3, _T("Tilt"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_4, _T("Rotate"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_5, _T("Stage-X"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_6, _T("Stage-Y"));
		SetDlgItemText(IDC_GROUPBOX_CAMERALIGHTMANAGERDLG_2, _T("Set the inspection image"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_7, _T("Image"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_8, _T("Align"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_9, _T("Cam Z"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_10, _T("Light Z"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_11, _T("Tilt"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_12, _T("Rotate"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_13, _T("Stage-X"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_14, _T("Stage-Y"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_15, _T("DF"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_16, _T("SP"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_17, _T("SEQ"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_18, _T("SEQ Addr"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_19, _T("Page"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_20, _T("Image"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_21, _T("Align"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_22, _T("Cam Z"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_23, _T("Light Z"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_24, _T("Tilt"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_25, _T("Rotate"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_26, _T("Stage-X"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_27, _T("Stage-Y"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_28, _T("DF"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_29, _T("SP"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_30, _T("SEQ"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_31, _T("SEQ Addr"));
		SetDlgItemText(IDC_STATIC_CAMERALIGHTMANAGERDLG_32, _T("Page"));
		SetDlgItemText(IDC_RADIO_IMAGE_1, _T("1"));
		SetDlgItemText(IDC_RADIO_IMAGE_2, _T("2"));
		SetDlgItemText(IDC_RADIO_IMAGE_3, _T("3"));
		SetDlgItemText(IDC_RADIO_IMAGE_4, _T("4"));
		SetDlgItemText(IDC_RADIO_IMAGE_5, _T("5"));
		SetDlgItemText(IDC_RADIO_IMAGE_6, _T("6"));
		SetDlgItemText(IDC_RADIO_IMAGE_7, _T("7"));
		SetDlgItemText(IDC_RADIO_IMAGE_8, _T("8"));
		SetDlgItemText(IDC_RADIO_IMAGE_9, _T("9"));
		SetDlgItemText(IDC_RADIO_IMAGE_10, _T("10"));
		SetDlgItemText(IDC_RADIO_IMAGE_11, _T("11"));
		SetDlgItemText(IDC_RADIO_IMAGE_12, _T("12"));
		SetDlgItemText(IDC_RADIO_IMAGE_13, _T("13"));
		SetDlgItemText(IDC_RADIO_IMAGE_14, _T("14"));
		SetDlgItemText(IDC_RADIO_IMAGE_15, _T("15"));
		SetDlgItemText(IDC_RADIO_IMAGE_16, _T("16"));
		SetDlgItemText(IDC_RADIO_IMAGE_17, _T("17"));
		SetDlgItemText(IDC_RADIO_IMAGE_18, _T("18"));
		SetDlgItemText(IDC_RADIO_IMAGE_19, _T("19"));
		SetDlgItemText(IDC_RADIO_IMAGE_20, _T("20"));
		SetDlgItemText(IDC_RADIO_IMAGE_21, _T("21"));
		SetDlgItemText(IDC_RADIO_IMAGE_22, _T("22"));
		SetDlgItemText(IDC_RADIO_IMAGE_23, _T("23"));
		SetDlgItemText(IDC_RADIO_IMAGE_24, _T("24"));
		SetDlgItemText(IDC_RADIO_IMAGE_25, _T("25"));
		SetDlgItemText(IDC_RADIO_IMAGE_26, _T("26"));
		SetDlgItemText(IDC_RADIO_IMAGE_27, _T("27"));
		SetDlgItemText(IDC_RADIO_IMAGE_28, _T("28"));
		SetDlgItemText(IDC_RADIO_IMAGE_29, _T("29"));
		SetDlgItemText(IDC_RADIO_IMAGE_30, _T("30"));
		SetDlgItemText(IDC_RADIO_IMAGE_31, _T("31"));
		SetDlgItemText(IDC_RADIO_IMAGE_32, _T("32"));
		SetDlgItemText(IDC_RADIO_IMAGE_33, _T("33"));
		SetDlgItemText(IDC_RADIO_IMAGE_34, _T("34"));
		SetDlgItemText(IDC_RADIO_IMAGE_35, _T("35"));
		SetDlgItemText(IDC_RADIO_IMAGE_36, _T("36"));
		SetDlgItemText(IDC_RADIO_IMAGE_37, _T("37"));
		SetDlgItemText(IDC_RADIO_IMAGE_38, _T("38"));
		SetDlgItemText(IDC_RADIO_IMAGE_39, _T("39"));
		SetDlgItemText(IDC_RADIO_IMAGE_40, _T("40"));
		SetDlgItemText(IDC_RADIO_IMAGE_41, _T("41"));
		SetDlgItemText(IDC_RADIO_IMAGE_42, _T("42"));
		SetDlgItemText(IDC_RADIO_IMAGE_43, _T("43"));
		SetDlgItemText(IDC_RADIO_IMAGE_44, _T("44"));
		SetDlgItemText(IDC_RADIO_IMAGE_45, _T("45"));
		SetDlgItemText(IDC_BUTTON_CHANGE_LIGHTVALUE, _T("Set the light page"));
		SetDlgItemText(IDC_BUTTON_GRAB_SEQUENCE, _T("Set the grab sequence"));
		SetDlgItemText(IDC_BUTTON_CHANGE_SPECULAR, _T("Set the 4D parameters"));
		SetDlgItemText(IDC_BUTTON_LOAD_CAL_DATA, _T("Load Caldata.ini file"));
		SetDlgItemText(IDC_BUTTON_SAVE_POSITION_SETTING, _T("Save position setting"));
		SetDlgItemText(IDC_BUTTON_SAVE_LIGHT_SETTING, _T("Save light setting"));
		SetDlgItemText(IDC_BUTTON_SAVE_POSITION_LIGHT_SETTING, _T("Save total setting"));
		SetDlgItemText(IDCANCEL, _T("Close"));
	}
}

void CCameraLightManagerDlg::OnBnClickedCancel()
{
	Hide();
}

#include "LightPageControlDlg.h"
void CCameraLightManagerDlg::OnBnClickedButtonChangeLightvalue()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	CLightPageControlDlg dlg;
	dlg.DoModal();
}

#include "SPVImageParamDlg.h"
void CCameraLightManagerDlg::OnBnClickedButtonChangeSpecular()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	CSPVImageParamDlg dlg;
	dlg.DoModal();
}

#include "GrabSequenceDlg.h"
void CCameraLightManagerDlg::OnBnClickedButtonGrabSequence()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	CGrabSequenceDlg dlg;
	dlg.DoModal();
}

void CCameraLightManagerDlg::LoadViewParam()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	CString strSection, strKey;

	CString strMotionMovingPosition;
	strMotionMovingPosition.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	CIniFileCS INI_MotionMovingPosition(strMotionMovingPosition);

	strSection = "Image";
	strKey = "ImageGrabCount";
	int iImageGrabCount = INI_MotionMovingPosition.Get_Integer(strSection, strKey, 1);
	m_iRadioLastInspectionImageIndex = iImageGrabCount - 1;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_dEditImageZPos[i] = MOTION_NOUSE;
	}

	strSection.Format("Moving Position %d", THEAPP.m_iCurStageIndex + 1);
	for (int i = 0; i < iImageGrabCount; i++)
	{
		strKey.Format("Z-%d", i + 1);
		m_dEditImageZPos[i] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);
	}

	CString strMotionMovingPosition_Offset;
	strMotionMovingPosition_Offset.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition_Offset.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	CIniFileCS INI_MotionMovingPosition_Offset(strMotionMovingPosition_Offset);

	strSection.Format("Moving Position %d", THEAPP.m_iCurStageIndex + 1);
	strKey.Format("LZ-ref");
	m_dEditImageLightZPosRef = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, -1000);
	strKey.Format("T-ref");
	m_dEditImageTiltPosRef = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, -1000);
	strKey.Format("R-ref");
	m_dEditImageRotatePosRef = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, -1000);
	strKey.Format("X-ref");
	m_dEditImageStageXPosRef = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, -1000);
	strKey.Format("Y-ref");
	m_dEditImageStageYPosRef = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, -1000);

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_dEditImageLightZPos[i] = 0;
		m_dEditImageTiltPos[i] = 0;
		m_dEditImageRotatePos[i] = 0;
		m_dEditImageStageXPos[i] = 0;
		m_dEditImageStageYPos[i] = 0;
	}

	BOOL bReferenceCheck = TRUE;
	if (m_dEditImageLightZPosRef == -1000 && m_dEditImageTiltPosRef == -1000 && m_dEditImageRotatePosRef == -1000 && m_dEditImageStageXPosRef == -1000 && m_dEditImageStageYPosRef == -1000)
	{
		bReferenceCheck = FALSE;
		m_dEditImageLightZPosRef = INI_MotionMovingPosition_Offset.Get_Double(strSection, "LZ-1", MOTION_NOUSE);
		m_dEditImageTiltPosRef = INI_MotionMovingPosition_Offset.Get_Double(strSection, "T-1", MOTION_NOUSE);
		m_dEditImageRotatePosRef = INI_MotionMovingPosition_Offset.Get_Double(strSection, "R-1", MOTION_NOUSE);
		m_dEditImageStageXPosRef = INI_MotionMovingPosition_Offset.Get_Double(strSection, "X-1", MOTION_NOUSE);
		m_dEditImageStageYPosRef = INI_MotionMovingPosition_Offset.Get_Double(strSection, "Y-1", MOTION_NOUSE);
	}

	for (int i = 0; i < iImageGrabCount; i++)
	{
		double dTemp;
		strKey.Format("LZ-%d", i + 1);
		if (!bReferenceCheck)
		{
			dTemp = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, m_dEditImageLightZPosRef);
			m_dEditImageLightZPos[i] = dTemp - m_dEditImageLightZPosRef;
		}
		else
			m_dEditImageLightZPos[i] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, 0);

		strKey.Format("T-%d", i + 1);
		if (!bReferenceCheck)
		{
			dTemp = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, m_dEditImageTiltPosRef);
			m_dEditImageTiltPos[i] = dTemp - m_dEditImageTiltPosRef;
		}
		else
			m_dEditImageTiltPos[i] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, 0);

		strKey.Format("R-%d", i + 1);
		if (!bReferenceCheck)
		{
			dTemp = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, m_dEditImageRotatePosRef);
			m_dEditImageRotatePos[i] = dTemp - m_dEditImageRotatePosRef;
		}
		else
			m_dEditImageRotatePos[i] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, 0);

		strKey.Format("X-%d", i + 1);
		if (!bReferenceCheck)
		{
			dTemp = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, m_dEditImageStageXPosRef);
			m_dEditImageStageXPos[i] = dTemp - m_dEditImageStageXPosRef;
		}
		else
			m_dEditImageStageXPos[i] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, 0);

		strKey.Format("Y-%d", i + 1);
		if (!bReferenceCheck)
		{
			dTemp = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, m_dEditImageStageYPosRef);
			m_dEditImageStageYPos[i] = dTemp - m_dEditImageStageYPosRef;
		}
		else
			m_dEditImageStageYPos[i] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, 0);
	}

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_bCheckMatchingImage[i] = FALSE;
		m_bCheckDiffusedImage[i] = FALSE;
		m_bCheckSpecularImage[i] = FALSE;
		m_bCheckSeqStartImage[i] = FALSE;
		m_cbSeqAddrIndex[i].SetCurSel(0);
		m_cbPageIndex[i].SetCurSel(0);
		m_cbSeqAddrIndex[i].EnableWindow(TRUE);
		m_cbPageIndex[i].EnableWindow(TRUE);
	}

	CString sTemp;
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_cbSeqAddrIndex[i].ResetContent();
		for (int j = 0; j < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; j++)
		{
			CString sSeqDesc = THEAPP.m_pModelDataManager->m_PageControlData.m_sAddrDesc[j];
			sTemp.Format("%02d. %s", j + 1, sSeqDesc);
			m_cbSeqAddrIndex[i].AddString(sTemp);
		}

		m_cbPageIndex[i].ResetContent();
		for (int j = 0; j < LIGHTCTRL_PAGE_COUNT; j++)
		{
			CString sPageDesc = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[j].m_sPageDesc;
			sTemp.Format("%02d. %s", j + 1, sPageDesc);
			m_cbPageIndex[i].AddString(sTemp);
		}

		m_cbSeqAddrIndex[i].SetCurSel(0);
		m_cbPageIndex[i].SetCurSel(0);
	}

	int iPrevSeqIndex, iCurSeqIndex, iPageIndex;
	iPrevSeqIndex = -1;
	iPageIndex = 0;
	for (int i = 0; i < iImageGrabCount; i++)
	{
		m_bCheckMatchingImage[i] = THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i];
		m_bCheckDiffusedImage[i] = THEAPP.m_pModelDataManager->m_bUseDiffusedImage[i];
		m_bCheckSpecularImage[i] = THEAPP.m_pModelDataManager->m_bUseSpecularImage[i];
		m_bCheckSeqStartImage[i] = THEAPP.m_pModelDataManager->m_bUseSeqStartImage[i];

		if (m_bCheckDiffusedImage[i])
		{
			iCurSeqIndex = -1;
			m_cbSeqAddrIndex[i].SetCurSel(1);
			m_cbPageIndex[i].SetCurSel(8);
			m_cbSeqAddrIndex[i].EnableWindow(FALSE);
			m_cbPageIndex[i].EnableWindow(FALSE);
		}
		else if (m_bCheckSpecularImage[i])
		{
			iCurSeqIndex = -1;
			m_cbSeqAddrIndex[i].SetCurSel(0);
			m_cbPageIndex[i].SetCurSel(0);
			m_cbSeqAddrIndex[i].EnableWindow(FALSE);
			m_cbPageIndex[i].EnableWindow(FALSE);
		}
		else
		{
			iCurSeqIndex = THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][i];
			m_cbSeqAddrIndex[i].SetCurSel(iCurSeqIndex);
			if (m_bCheckSeqStartImage[i])
				iPageIndex = 0;
			else
				iPageIndex++;
			m_cbPageIndex[i].SetCurSel(THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[iCurSeqIndex][iPageIndex]);
		}
		iPrevSeqIndex = iCurSeqIndex;
	}

	UpdateData(FALSE);
}

void CCameraLightManagerDlg::UpdateViewParam()
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}

void CCameraLightManagerDlg::OnBnClickedButtonSavePositionSetting()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	UpdateViewParam();

	UpdateData();

	CString strVisionModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	if (GetFileAttributes(strVisionModelFolder) == -1)
		return;

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	THEAPP.m_pModelDataManager->m_iNoUsedImageGrab = m_iRadioLastInspectionImageIndex + 1;

	THEAPP.m_pModelDataManager->m_dLightZPositionRef[THEAPP.m_iCurStageIndex] = m_dEditImageLightZPosRef;
	THEAPP.m_pModelDataManager->m_dTiltPositionRef[THEAPP.m_iCurStageIndex] = m_dEditImageTiltPosRef;
	THEAPP.m_pModelDataManager->m_dRotatePositionRef[THEAPP.m_iCurStageIndex] = m_dEditImageRotatePosRef;
	THEAPP.m_pModelDataManager->m_dStageXPositionRef[THEAPP.m_iCurStageIndex] = m_dEditImageStageXPosRef;
	THEAPP.m_pModelDataManager->m_dStageYPositionRef[THEAPP.m_iCurStageIndex] = m_dEditImageStageYPosRef;

	THEAPP.m_pModelDataManager->m_dZFocusPosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);
	THEAPP.m_pModelDataManager->m_dLightZPosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);
	THEAPP.m_pModelDataManager->m_dTiltPosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);
	THEAPP.m_pModelDataManager->m_dRotatePosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);
	THEAPP.m_pModelDataManager->m_dStageXPosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);
	THEAPP.m_pModelDataManager->m_dStageYPosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);

	for (int i = 0; i < (m_iRadioLastInspectionImageIndex + 1); i++)
	{
		THEAPP.m_pModelDataManager->m_dZFocusPosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageZPos[i];
		THEAPP.m_pModelDataManager->m_dLightZPosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageLightZPosRef + m_dEditImageLightZPos[i];
		THEAPP.m_pModelDataManager->m_dTiltPosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageTiltPosRef + m_dEditImageTiltPos[i];
		THEAPP.m_pModelDataManager->m_dRotatePosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageRotatePosRef + m_dEditImageRotatePos[i];
		THEAPP.m_pModelDataManager->m_dStageXPosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageStageXPosRef + m_dEditImageStageXPos[i];
		THEAPP.m_pModelDataManager->m_dStageYPosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageStageYPosRef + m_dEditImageStageYPos[i];
	}

	CString strMotionMovingPosition;
	strMotionMovingPosition.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveMotionMovingPosition(strMotionMovingPosition, THEAPP.m_iCurStageIndex);

	CString strMotionMovingPosition_Offset;
	strMotionMovingPosition_Offset.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition_Offset.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveMotionMovingPosition_Offset(strMotionMovingPosition_Offset, THEAPP.m_iCurStageIndex);

	THEAPP.m_pTabControlDlg->m_pJogSetDlg->LoadViewParam();

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("저장 완료.");
	else
		strMessageBox.Format("Save complete");
	AfxMessageBox(strMessageBox, MB_ICONINFORMATION);
}

void CCameraLightManagerDlg::OnBnClickedButtonSaveLightSetting()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	UpdateViewParam();

	UpdateData();

	CString strVisionModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	if (GetFileAttributes(strVisionModelFolder) == -1)
		return;

	CString sTemp = "";
	BOOL bSaveCheck = TRUE;
	for (int i = 0; i < (m_iRadioLastInspectionImageIndex + 1); i++)
	{
		if (m_bCheckDiffusedImage[i] && m_cbSeqAddrIndex[i].GetCurSel() != 1 && m_cbPageIndex[i].GetCurSel() != 8)
		{
			bSaveCheck = FALSE;
			if (sTemp == "")
				sTemp.Format("%d", i + 1);
			else
				sTemp.Format("%s, %d", sTemp, i + 1);
		}
		if (!m_bCheckDiffusedImage[i] && (m_cbSeqAddrIndex[i].GetCurSel() == 1 || m_cbPageIndex[i].GetCurSel() == 8))
		{
			bSaveCheck = FALSE;
			if (sTemp == "")
				sTemp.Format("%d", i + 1);
			else
				sTemp.Format("%s, %d", sTemp, i + 1);
		}
		if (m_bCheckSpecularImage[i] && m_cbSeqAddrIndex[i].GetCurSel() != 0 && m_cbPageIndex[i].GetCurSel() != 0)
		{
			bSaveCheck = FALSE;
			if (sTemp == "")
				sTemp.Format("%d", i + 1);
			else
				sTemp.Format("%s, %d", sTemp, i + 1);
		}
		if (!m_bCheckSpecularImage[i] && (m_cbSeqAddrIndex[i].GetCurSel() == 0 || m_cbPageIndex[i].GetCurSel() == 0))
		{
			bSaveCheck = FALSE;
			if (sTemp == "")
				sTemp.Format("%d", i + 1);
			else
				sTemp.Format("%s, %d", sTemp, i + 1);
		}
	}
	if (!bSaveCheck)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("아래 영상의 조명 시퀀스와 페이지를 확인해주세요\n영상 %s", sTemp);
		else
			strMessageBox.Format("Check the lighting sequence and page\nimage %s", sTemp);
		AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	BOOL bSeqCheck[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT] = { FALSE };
	int iSeqImageCount[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT] = { 0 };
	int iSeqImageCountCheck = 0;
	int iPrevSeqAddrIndex = -1;
	for (int i = 0; i <= (m_iRadioLastInspectionImageIndex + 1); i++)
	{
		int iSeqAddrIndex = m_cbSeqAddrIndex[i].GetCurSel();

		if (iPrevSeqAddrIndex == iSeqAddrIndex)
		{
			if (m_bCheckSeqStartImage[i])
			{
				bSeqCheck[iPrevSeqAddrIndex] = TRUE;
				iSeqImageCountCheck = 1;
			}
			else
			{
				if (!m_bCheckDiffusedImage[i] && !m_bCheckSpecularImage[i])
				{
					if (!bSeqCheck[iSeqAddrIndex])
						iSeqImageCount[iSeqAddrIndex]++;
					else
						iSeqImageCountCheck++;
				}
			}

			iPrevSeqAddrIndex = iSeqAddrIndex;
		}
		else
		{
			if (i > 0)
			{
				if (!bSeqCheck[iPrevSeqAddrIndex])
				{
					bSeqCheck[iPrevSeqAddrIndex] = TRUE;

					if (m_bCheckDiffusedImage[i - 1])
						iSeqImageCount[iPrevSeqAddrIndex] = 4;
					if (m_bCheckSpecularImage[i - 1])
						iSeqImageCount[iPrevSeqAddrIndex] = 8;
				}
				else
				{
					if (!m_bCheckDiffusedImage[i] && !m_bCheckSpecularImage[i])
					{
						if (iSeqImageCount[iPrevSeqAddrIndex] != iSeqImageCountCheck)
						{
							if (THEAPP.m_iPGMLanguageSelect == 0)
								strMessageBox.Format("아래 조명 시퀀스의 이미지 수량을 확인해주세요\n%02d. %s", iPrevSeqAddrIndex, THEAPP.m_pModelDataManager->m_PageControlData.m_sAddrDesc[iPrevSeqAddrIndex]);
							else
								strMessageBox.Format("Check the image count of lighting sequence\n%02d. %s", iPrevSeqAddrIndex, THEAPP.m_pModelDataManager->m_PageControlData.m_sAddrDesc[iPrevSeqAddrIndex]);
							AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
							return;
						}
					}
				}
			}

			if (i == m_iRadioLastInspectionImageIndex + 1)
				break;

			if (!bSeqCheck[iSeqAddrIndex])
			{
				if (!m_bCheckDiffusedImage[i] && !m_bCheckSpecularImage[i])
					iSeqImageCount[iSeqAddrIndex]++;
			}
			else
			{
				if (m_bCheckDiffusedImage[i])
					iSeqImageCountCheck = 4;
				else if (m_bCheckSpecularImage[i])
					iSeqImageCountCheck = 8;
				else
					iSeqImageCountCheck = 1;
			}

			iPrevSeqAddrIndex = iSeqAddrIndex;
		}
	}

	CString strSWModelBase;
	strSWModelBase.Format("%s\\SW\\Vision_N%d\\ModelBase.ini", strVisionModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	CString strSection, strKey;
	CIniFileCS INI_SWModelBase(strSWModelBase);
	strSection = "Local Align";
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i] = m_bCheckMatchingImage[i];
		strKey.Format("m_bUseLocalAlignMatching_%d", i + 1);
		INI_SWModelBase.Set_Bool(strSection, strKey, m_bCheckMatchingImage[i]);
	}

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		THEAPP.m_pModelDataManager->m_bUseDiffusedImage[i] = m_bCheckDiffusedImage[i];
		THEAPP.m_pModelDataManager->m_bUseSpecularImage[i] = m_bCheckSpecularImage[i];
		THEAPP.m_pModelDataManager->m_bUseSeqStartImage[i] = m_bCheckSeqStartImage[i];
		THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][i] = m_cbSeqAddrIndex[i].GetCurSel();
	}

	CString strHWModelBase;
	strHWModelBase.Format("%s\\HW\\Vision_N%d\\ModelBase.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveHWModelBase(strHWModelBase, THEAPP.m_iCurStageIndex);

	for (int i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		if (i == 0 || i == 1)
			continue;

		int iPageAddrIndex = 0;
		BOOL bFindCheck = FALSE;
		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			int iSeqAddrIndex = m_cbSeqAddrIndex[j].GetCurSel();

			if (i == iSeqAddrIndex)
			{
				if (bFindCheck && m_bCheckSeqStartImage[j])
				{
					for (int k = iPageAddrIndex; k < LIGHTCTRL_GRAB_ADDR_PAGE_COUNT; k++)
						THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i][k] = 0;

					THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[i] = iPageAddrIndex;

					break;
				}

				bFindCheck = TRUE;
				THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i][iPageAddrIndex] = m_cbPageIndex[j].GetCurSel();
				iPageAddrIndex++;
			}
			else if (i != iSeqAddrIndex && bFindCheck)
			{
				for (int k = iPageAddrIndex; k < LIGHTCTRL_GRAB_ADDR_PAGE_COUNT; k++)
					THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i][k] = 0;

				THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[i] = iPageAddrIndex;

				break;
			}
		}
	}

#ifdef INLINE_MODE
	for (int i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
		THEAPP.m_pModelDataManager->m_PageControlData.SetGrabSequence(THEAPP.m_pModelDataManager->GetModelIdx(), i, THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i], THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[i]);
#endif

	CString strSequenceInfo;
	strSequenceInfo.Format("%s\\HW\\Vision_N%d\\SequenceInfo.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveSequenceInfo(strSequenceInfo);

	THEAPP.m_pTabControlDlg->m_pJogSetDlg->LoadViewParam();

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("저장 완료.");
	else
		strMessageBox.Format("Save complete");
	AfxMessageBox(strMessageBox, MB_ICONINFORMATION);
}

void CCameraLightManagerDlg::OnBnClickedButtonSavePositionLightSetting()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	UpdateViewParam();

	UpdateData();

	CString strVisionModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	if (GetFileAttributes(strVisionModelFolder) == -1)
		return;

	CString sTemp = "";
	BOOL bSaveCheck = TRUE;
	for (int i = 0; i < (m_iRadioLastInspectionImageIndex + 1); i++)
	{
		if (m_bCheckDiffusedImage[i] && m_cbSeqAddrIndex[i].GetCurSel() != 1 && m_cbPageIndex[i].GetCurSel() != 8)
		{
			bSaveCheck = FALSE;
			if (sTemp == "")
				sTemp.Format("%d", i + 1);
			else
				sTemp.Format("%s, %d", sTemp, i + 1);
		}
		if (!m_bCheckDiffusedImage[i] && (m_cbSeqAddrIndex[i].GetCurSel() == 1 || m_cbPageIndex[i].GetCurSel() == 8))
		{
			bSaveCheck = FALSE;
			if (sTemp == "")
				sTemp.Format("%d", i + 1);
			else
				sTemp.Format("%s, %d", sTemp, i + 1);
		}
		if (m_bCheckSpecularImage[i] && m_cbSeqAddrIndex[i].GetCurSel() != 0 && m_cbPageIndex[i].GetCurSel() != 0)
		{
			bSaveCheck = FALSE;
			if (sTemp == "")
				sTemp.Format("%d", i + 1);
			else
				sTemp.Format("%s, %d", sTemp, i + 1);
		}
		if (!m_bCheckSpecularImage[i] && (m_cbSeqAddrIndex[i].GetCurSel() == 0 || m_cbPageIndex[i].GetCurSel() == 0))
		{
			bSaveCheck = FALSE;
			if (sTemp == "")
				sTemp.Format("%d", i + 1);
			else
				sTemp.Format("%s, %d", sTemp, i + 1);
		}
	}
	if (!bSaveCheck)
	{
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("아래 영상의 조명 시퀀스와 페이지를 확인해주세요\n영상 %s", sTemp);
		else
			strMessageBox.Format("Check the lighting sequence and page\nimage %s", sTemp);
		AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	BOOL bSeqCheck[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT] = { FALSE };
	int iSeqImageCount[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT] = { 0 };
	int iSeqImageCountCheck = 0;
	int iPrevSeqAddrIndex = -1;
	for (int i = 0; i <= (m_iRadioLastInspectionImageIndex + 1); i++)
	{
		int iSeqAddrIndex = m_cbSeqAddrIndex[i].GetCurSel();

		if (iPrevSeqAddrIndex == iSeqAddrIndex)
		{
			if (m_bCheckSeqStartImage[i])
			{
				bSeqCheck[iPrevSeqAddrIndex] = TRUE;
				iSeqImageCountCheck = 1;
			}
			else
			{
				if (!m_bCheckDiffusedImage[i] && !m_bCheckSpecularImage[i])
				{
					if (!bSeqCheck[iSeqAddrIndex])
						iSeqImageCount[iSeqAddrIndex]++;
					else
						iSeqImageCountCheck++;
				}
			}

			iPrevSeqAddrIndex = iSeqAddrIndex;
		}
		else
		{
			if (i > 0)
			{
				if (!bSeqCheck[iPrevSeqAddrIndex])
				{
					bSeqCheck[iPrevSeqAddrIndex] = TRUE;

					if (m_bCheckDiffusedImage[i - 1])
						iSeqImageCount[iPrevSeqAddrIndex] = 4;
					if (m_bCheckSpecularImage[i - 1])
						iSeqImageCount[iPrevSeqAddrIndex] = 8;
				}
				else
				{
					if (!m_bCheckDiffusedImage[i] && !m_bCheckSpecularImage[i])
					{
						if (iSeqImageCount[iPrevSeqAddrIndex] != iSeqImageCountCheck)
						{
							if (THEAPP.m_iPGMLanguageSelect == 0)
								strMessageBox.Format("아래 조명 시퀀스의 이미지 수량을 확인해주세요\n%02d. %s", iPrevSeqAddrIndex, THEAPP.m_pModelDataManager->m_PageControlData.m_sAddrDesc[iPrevSeqAddrIndex]);
							else
								strMessageBox.Format("Check the image count of lighting sequence\n%02d. %s", iPrevSeqAddrIndex, THEAPP.m_pModelDataManager->m_PageControlData.m_sAddrDesc[iPrevSeqAddrIndex]);
							AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
							return;
						}
					}
				}
			}

			if (i == m_iRadioLastInspectionImageIndex + 1)
				break;

			if (!bSeqCheck[iSeqAddrIndex])
			{
				if (!m_bCheckDiffusedImage[i] && !m_bCheckSpecularImage[i])
					iSeqImageCount[iSeqAddrIndex]++;
			}
			else
			{
				if (m_bCheckDiffusedImage[i])
					iSeqImageCountCheck = 4;
				else if (m_bCheckSpecularImage[i])
					iSeqImageCountCheck = 8;
				else
					iSeqImageCountCheck = 1;
			}

			iPrevSeqAddrIndex = iSeqAddrIndex;
		}
	}

	CString strSWModelBase;
	strSWModelBase.Format("%s\\SW\\Vision_N%d\\ModelBase.ini", strVisionModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	CString strSection, strKey;
	CIniFileCS INI_SWModelBase(strSWModelBase);
	strSection = "Local Align";
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i] = m_bCheckMatchingImage[i];
		strKey.Format("m_bUseLocalAlignMatching_%d", i + 1);
		INI_SWModelBase.Set_Bool(strSection, strKey, m_bCheckMatchingImage[i]);
	}

	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		THEAPP.m_pModelDataManager->m_bUseDiffusedImage[i] = m_bCheckDiffusedImage[i];
		THEAPP.m_pModelDataManager->m_bUseSpecularImage[i] = m_bCheckSpecularImage[i];
		THEAPP.m_pModelDataManager->m_bUseSeqStartImage[i] = m_bCheckSeqStartImage[i];
		THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][i] = m_cbSeqAddrIndex[i].GetCurSel();
	}

	CString strHWModelBase;
	strHWModelBase.Format("%s\\HW\\Vision_N%d\\ModelBase.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveHWModelBase(strHWModelBase, THEAPP.m_iCurStageIndex);

	THEAPP.m_pModelDataManager->m_iNoUsedImageGrab = m_iRadioLastInspectionImageIndex + 1;

	THEAPP.m_pModelDataManager->m_dLightZPositionRef[THEAPP.m_iCurStageIndex] = m_dEditImageLightZPosRef;
	THEAPP.m_pModelDataManager->m_dTiltPositionRef[THEAPP.m_iCurStageIndex] = m_dEditImageTiltPosRef;
	THEAPP.m_pModelDataManager->m_dRotatePositionRef[THEAPP.m_iCurStageIndex] = m_dEditImageRotatePosRef;
	THEAPP.m_pModelDataManager->m_dStageXPositionRef[THEAPP.m_iCurStageIndex] = m_dEditImageStageXPosRef;
	THEAPP.m_pModelDataManager->m_dStageYPositionRef[THEAPP.m_iCurStageIndex] = m_dEditImageStageYPosRef;

	THEAPP.m_pModelDataManager->m_dZFocusPosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);
	THEAPP.m_pModelDataManager->m_dLightZPosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);
	THEAPP.m_pModelDataManager->m_dTiltPosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);
	THEAPP.m_pModelDataManager->m_dRotatePosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);
	THEAPP.m_pModelDataManager->m_dStageXPosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);
	THEAPP.m_pModelDataManager->m_dStageYPosition[THEAPP.m_iCurStageIndex].resize(THEAPP.m_pModelDataManager->m_iNoUsedImageGrab);

	for (int i = 0; i < (m_iRadioLastInspectionImageIndex + 1); i++)
	{
		THEAPP.m_pModelDataManager->m_dZFocusPosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageZPos[i];
		THEAPP.m_pModelDataManager->m_dLightZPosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageLightZPosRef + m_dEditImageLightZPos[i];
		THEAPP.m_pModelDataManager->m_dTiltPosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageTiltPosRef + m_dEditImageTiltPos[i];
		THEAPP.m_pModelDataManager->m_dRotatePosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageRotatePosRef + m_dEditImageRotatePos[i];
		THEAPP.m_pModelDataManager->m_dStageXPosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageStageXPosRef + m_dEditImageStageXPos[i];
		THEAPP.m_pModelDataManager->m_dStageYPosition[THEAPP.m_iCurStageIndex][i] = m_dEditImageStageYPosRef + m_dEditImageStageYPos[i];
	}

	CString strMotionMovingPosition;
	strMotionMovingPosition.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveMotionMovingPosition(strMotionMovingPosition, THEAPP.m_iCurStageIndex);

	CString strMotionMovingPosition_Offset;
	strMotionMovingPosition_Offset.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition_Offset.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveMotionMovingPosition_Offset(strMotionMovingPosition_Offset, THEAPP.m_iCurStageIndex);

	for (int i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		if (i == 0 || i == 1)
			continue;

		int iPageAddrIndex = 0;
		BOOL bFindCheck = FALSE;
		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			int iSeqAddrIndex = m_cbSeqAddrIndex[j].GetCurSel();

			if (i == iSeqAddrIndex)
			{
				if (bFindCheck && m_bCheckSeqStartImage[j])
				{
					for (int k = iPageAddrIndex; k < LIGHTCTRL_GRAB_ADDR_PAGE_COUNT; k++)
						THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i][k] = 0;

					THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[i] = iPageAddrIndex;

					break;
				}

				bFindCheck = TRUE;
				THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i][iPageAddrIndex] = m_cbPageIndex[j].GetCurSel();
				iPageAddrIndex++;
			}
			else if (i != iSeqAddrIndex && bFindCheck)
			{
				for (int k = iPageAddrIndex; k < LIGHTCTRL_GRAB_ADDR_PAGE_COUNT; k++)
					THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i][k] = 0;

				THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[i] = iPageAddrIndex;

				break;
			}
		}
	}

#ifdef INLINE_MODE
	for (int i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
		THEAPP.m_pModelDataManager->m_PageControlData.SetGrabSequence(THEAPP.m_pModelDataManager->GetModelIdx(), i, THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[i], THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[i]);
#endif

	CString strSequenceInfo;
	strSequenceInfo.Format("%s\\HW\\Vision_N%d\\SequenceInfo.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveSequenceInfo(strSequenceInfo);

	THEAPP.m_pTabControlDlg->m_pJogSetDlg->LoadViewParam();

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("저장 완료.");
	else
		strMessageBox.Format("Save complete");
	AfxMessageBox(strMessageBox, MB_ICONINFORMATION);
}

void CCameraLightManagerDlg::OnBnClickedButtonLoadCalData()
{
	CString sVisionCamType;
	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[THEAPP.m_pModelDataManager->GetModelIdx()];

	int nRes = 0;
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("Caldata.ini 파일을 다시 로딩 후 %s 비전의 픽셀 사이즈를 변경합니다. 진행하시겠습니까?", sVisionCamType);
	else
		strMessageBox.Format("After reloading the Caldata.ini file, the pixel size of %s Vision will be changed. Do you want to continue?", sVisionCamType);
	nRes = AfxMessageBox(strMessageBox, MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes == IDYES))
	{
		//////////////////////////////////////////////////////////////////////////
		// Cal Data Initialization
		CString strOpticModelFolder;
		strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);

		CString strCalData;
		strCalData.Format("%s\\HW\\Vision_N%d\\CalData.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

		CIniFileCS INI_CalData(strCalData);

		double dPixelRes;
		int iCamImageSizeX, iCamImageSizeY;
		CString strSection, strKey;
		strSection = "PIXEL_RESOLUTION";
		strKey = sVisionCamType;
#ifdef INLINE_MODE
		dPixelRes = INI_CalData.Get_Double(strSection, strKey, 5.0);
#else
		dPixelRes = THEAPP.m_ModelDefineInfo.m_iPixelResolution[THEAPP.m_pModelDataManager->GetModelIdx()];
#endif
		iCamImageSizeX = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[THEAPP.m_pModelDataManager->GetModelIdx()];
		iCamImageSizeY = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[THEAPP.m_pModelDataManager->GetModelIdx()];
		THEAPP.m_pCalDataService_N[THEAPP.m_pModelDataManager->GetModelIdx()]->SetCalData(iCamImageSizeX, iCamImageSizeY, dPixelRes);
		THEAPP.m_pCalDataService_N[THEAPP.m_pModelDataManager->GetModelIdx()]->SetVisionCamName(THEAPP.m_pModelDataManager->GetModelIdx());

		THEAPP.m_pCalDataService = THEAPP.m_pCalDataService_N[THEAPP.m_pModelDataManager->GetModelIdx()];

		strLog.Format("Calibration data load complete");
		THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("CalData 불러오기 성공");
		else
			strMessageBox.Format("CalData load complete");
		AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
	}
}

void CCameraLightManagerDlg::SetMotionParameter(int iInspectImageIndex, double dCurPos_Z, double dCurPos_LightZ, double dCurPos_T, double dCurPos_R, double dCurPos_X, double dCurPos_Y)
{
	if (dCurPos_Z != MOTION_NOUSE)
		m_dEditImageZPos[iInspectImageIndex] = dCurPos_Z;
	if (dCurPos_LightZ != MOTION_NOUSE)
		m_dEditImageLightZPos[iInspectImageIndex] = dCurPos_LightZ - m_dEditImageLightZPosRef;
	if (dCurPos_T != MOTION_NOUSE)
		m_dEditImageTiltPos[iInspectImageIndex] = dCurPos_T - m_dEditImageTiltPosRef;
	if (dCurPos_R != MOTION_NOUSE)
		m_dEditImageRotatePos[iInspectImageIndex] = dCurPos_R - m_dEditImageRotatePosRef;
	if (dCurPos_X != MOTION_NOUSE)
		m_dEditImageStageXPos[iInspectImageIndex] = dCurPos_X - m_dEditImageStageXPosRef;
	if (dCurPos_Y != MOTION_NOUSE)
		m_dEditImageStageYPos[iInspectImageIndex] = dCurPos_Y - m_dEditImageStageYPosRef;

	UpdateData(FALSE);
}

void CCameraLightManagerDlg::CheckBoxSetDiffused(int iImageIndex, BOOL bCheckBoxStatus)
{
	UpdateData(TRUE);
	if (!bCheckBoxStatus)
	{
		CheckBoxSetSpecular(iImageIndex, TRUE);
		CheckBoxSetSequence(iImageIndex, TRUE);
		m_bCheckDiffusedImage[iImageIndex] = TRUE;
		m_cbSeqAddrIndex[iImageIndex].SetCurSel(1);
		m_cbPageIndex[iImageIndex].SetCurSel(8);
		m_cbSeqAddrIndex[iImageIndex].EnableWindow(FALSE);
		m_cbPageIndex[iImageIndex].EnableWindow(FALSE);
	}
	else
	{
		m_bCheckDiffusedImage[iImageIndex] = FALSE;
		m_cbSeqAddrIndex[iImageIndex].EnableWindow(TRUE);
		m_cbPageIndex[iImageIndex].EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}

void CCameraLightManagerDlg::CheckBoxSetSpecular(int iImageIndex, BOOL bCheckBoxStatus)
{
	if (!bCheckBoxStatus)
	{
		UpdateData(TRUE);
		if (iImageIndex == MAX_IMAGE_TAB - 1)
		{
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("검사 영상 수량을 확인해주세요.");
			else
				strMessageBox.Format("Check last inspect image index");
			AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);

			m_bCheckSpecularImage[iImageIndex] = FALSE;
			UpdateData(FALSE);
			return;
		}

		if (m_bCheckSpecularImage[iImageIndex + 1])
		{
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("%d번 영상이 이미 스펙큘러 영상으로 사용중입니다.", iImageIndex + 2);
			else
				strMessageBox.Format("Image %d is already being used as a specular image.", iImageIndex + 2);
			AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);

			m_bCheckSpecularImage[iImageIndex] = FALSE;
			UpdateData(FALSE);
			return;
		}

		CheckBoxSetDiffused(iImageIndex, TRUE);
		CheckBoxSetSequence(iImageIndex, TRUE);
		m_bCheckSpecularImage[iImageIndex] = TRUE;
		m_cbSeqAddrIndex[iImageIndex].SetCurSel(0);
		m_cbPageIndex[iImageIndex].SetCurSel(0);
		m_cbSeqAddrIndex[iImageIndex].EnableWindow(FALSE);
		m_cbPageIndex[iImageIndex].EnableWindow(FALSE);

		CheckBoxSetDiffused(iImageIndex + 1, TRUE);
		CheckBoxSetSequence(iImageIndex + 1, TRUE);
		m_bCheckSpecularImage[iImageIndex + 1] = TRUE;
		m_cbSeqAddrIndex[iImageIndex + 1].SetCurSel(0);
		m_cbPageIndex[iImageIndex + 1].SetCurSel(0);
		m_cbSeqAddrIndex[iImageIndex + 1].EnableWindow(FALSE);
		m_cbPageIndex[iImageIndex + 1].EnableWindow(FALSE);
	}
	else
	{
		if (m_bCheckSpecularImage[iImageIndex])
		{
			UpdateData(TRUE);
			m_bCheckSpecularImage[iImageIndex] = FALSE;
			m_cbSeqAddrIndex[iImageIndex].EnableWindow(TRUE);
			m_cbPageIndex[iImageIndex].EnableWindow(TRUE);

			if (iImageIndex == 0)
			{
				m_bCheckSpecularImage[iImageIndex + 1] = FALSE;
				m_cbSeqAddrIndex[iImageIndex + 1].EnableWindow(TRUE);
				m_cbPageIndex[iImageIndex + 1].EnableWindow(TRUE);
			}
			else
			{
				if (iImageIndex == MAX_IMAGE_TAB - 1)
				{
					m_bCheckSpecularImage[iImageIndex - 1] = FALSE;
					m_cbSeqAddrIndex[iImageIndex - 1].EnableWindow(TRUE);
					m_cbPageIndex[iImageIndex - 1].EnableWindow(TRUE);
				}
				else
				{
					if (!m_bCheckSpecularImage[iImageIndex - 1] && m_bCheckSpecularImage[iImageIndex + 1])
					{
						m_bCheckSpecularImage[iImageIndex + 1] = FALSE;
						m_cbSeqAddrIndex[iImageIndex + 1].EnableWindow(TRUE);
						m_cbPageIndex[iImageIndex + 1].EnableWindow(TRUE);
					}
					else if (m_bCheckSpecularImage[iImageIndex - 1] && !m_bCheckSpecularImage[iImageIndex + 1])
					{
						m_bCheckSpecularImage[iImageIndex - 1] = FALSE;
						m_cbSeqAddrIndex[iImageIndex - 1].EnableWindow(TRUE);
						m_cbPageIndex[iImageIndex - 1].EnableWindow(TRUE);
					}
					else
					{
						if (THEAPP.m_iPGMLanguageSelect == 0)
							strMessageBox.Format("해당 영상과 연결된 스펙큘러 영상의 체크를 해제해주세요.");
						else
							strMessageBox.Format("Please uncheck the specular image linked to the selected image.");
						AfxMessageBox(strMessageBox, MB_ICONINFORMATION | MB_SYSTEMMODAL);
					}
				}
			}
		}
	}
	UpdateData(FALSE);
}

void CCameraLightManagerDlg::CheckBoxSetSequence(int iImageIndex, BOOL bCheckBoxStatus)
{
	UpdateData(TRUE);
	if (!bCheckBoxStatus)
	{
		CheckBoxSetDiffused(iImageIndex, TRUE);
		CheckBoxSetSpecular(iImageIndex, TRUE);
		m_bCheckSeqStartImage[iImageIndex] = TRUE;
	}
	else
		m_bCheckSeqStartImage[iImageIndex] = FALSE;
	UpdateData(FALSE);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage1()
{
	CheckBoxSetDiffused(0, m_bCheckDiffusedImage[0]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage2()
{
	CheckBoxSetDiffused(1, m_bCheckDiffusedImage[1]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage3()
{
	CheckBoxSetDiffused(2, m_bCheckDiffusedImage[2]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage4()
{
	CheckBoxSetDiffused(3, m_bCheckDiffusedImage[3]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage5()
{
	CheckBoxSetDiffused(4, m_bCheckDiffusedImage[4]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage6()
{
	CheckBoxSetDiffused(5, m_bCheckDiffusedImage[5]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage7()
{
	CheckBoxSetDiffused(6, m_bCheckDiffusedImage[6]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage8()
{
	CheckBoxSetDiffused(7, m_bCheckDiffusedImage[7]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage9()
{
	CheckBoxSetDiffused(8, m_bCheckDiffusedImage[8]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage10()
{
	CheckBoxSetDiffused(9, m_bCheckDiffusedImage[9]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage11()
{
	CheckBoxSetDiffused(10, m_bCheckDiffusedImage[10]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage12()
{
	CheckBoxSetDiffused(11, m_bCheckDiffusedImage[11]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage13()
{
	CheckBoxSetDiffused(12, m_bCheckDiffusedImage[12]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage14()
{
	CheckBoxSetDiffused(13, m_bCheckDiffusedImage[13]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage15()
{
	CheckBoxSetDiffused(14, m_bCheckDiffusedImage[14]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage16()
{
	CheckBoxSetDiffused(15, m_bCheckDiffusedImage[15]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage17()
{
	CheckBoxSetDiffused(16, m_bCheckDiffusedImage[16]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage18()
{
	CheckBoxSetDiffused(17, m_bCheckDiffusedImage[17]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage19()
{
	CheckBoxSetDiffused(18, m_bCheckDiffusedImage[18]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage20()
{
	CheckBoxSetDiffused(19, m_bCheckDiffusedImage[19]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage21()
{
	CheckBoxSetDiffused(20, m_bCheckDiffusedImage[20]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage22()
{
	CheckBoxSetDiffused(21, m_bCheckDiffusedImage[21]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage23()
{
	CheckBoxSetDiffused(22, m_bCheckDiffusedImage[22]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage24()
{
	CheckBoxSetDiffused(23, m_bCheckDiffusedImage[23]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage25()
{
	CheckBoxSetDiffused(24, m_bCheckDiffusedImage[24]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage26()
{
	CheckBoxSetDiffused(25, m_bCheckDiffusedImage[25]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage27()
{
	CheckBoxSetDiffused(26, m_bCheckDiffusedImage[26]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage28()
{
	CheckBoxSetDiffused(27, m_bCheckDiffusedImage[27]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage29()
{
	CheckBoxSetDiffused(28, m_bCheckDiffusedImage[28]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage30()
{
	CheckBoxSetDiffused(29, m_bCheckDiffusedImage[29]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage31()
{
	CheckBoxSetDiffused(30, m_bCheckDiffusedImage[30]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage32()
{
	CheckBoxSetDiffused(31, m_bCheckDiffusedImage[31]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage33()
{
	CheckBoxSetDiffused(32, m_bCheckDiffusedImage[32]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage34()
{
	CheckBoxSetDiffused(33, m_bCheckDiffusedImage[33]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage35()
{
	CheckBoxSetDiffused(34, m_bCheckDiffusedImage[34]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage36()
{
	CheckBoxSetDiffused(35, m_bCheckDiffusedImage[35]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage37()
{
	CheckBoxSetDiffused(36, m_bCheckDiffusedImage[36]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage38()
{
	CheckBoxSetDiffused(37, m_bCheckDiffusedImage[37]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage39()
{
	CheckBoxSetDiffused(38, m_bCheckDiffusedImage[38]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage40()
{
	CheckBoxSetDiffused(39, m_bCheckDiffusedImage[39]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage41()
{
	CheckBoxSetDiffused(40, m_bCheckDiffusedImage[40]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage42()
{
	CheckBoxSetDiffused(41, m_bCheckDiffusedImage[41]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage43()
{
	CheckBoxSetDiffused(42, m_bCheckDiffusedImage[42]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage44()
{
	CheckBoxSetDiffused(43, m_bCheckDiffusedImage[43]);
}

void CCameraLightManagerDlg::OnBnClickedCheckDiffusedImage45()
{
	CheckBoxSetDiffused(44, m_bCheckDiffusedImage[44]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage1()
{
	CheckBoxSetSpecular(0, m_bCheckSpecularImage[0]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage2()
{
	CheckBoxSetSpecular(1, m_bCheckSpecularImage[1]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage3()
{
	CheckBoxSetSpecular(2, m_bCheckSpecularImage[2]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage4()
{
	CheckBoxSetSpecular(3, m_bCheckSpecularImage[3]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage5()
{
	CheckBoxSetSpecular(4, m_bCheckSpecularImage[4]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage6()
{
	CheckBoxSetSpecular(5, m_bCheckSpecularImage[5]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage7()
{
	CheckBoxSetSpecular(6, m_bCheckSpecularImage[6]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage8()
{
	CheckBoxSetSpecular(7, m_bCheckSpecularImage[7]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage9()
{
	CheckBoxSetSpecular(8, m_bCheckSpecularImage[8]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage10()
{
	CheckBoxSetSpecular(9, m_bCheckSpecularImage[9]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage11()
{
	CheckBoxSetSpecular(10, m_bCheckSpecularImage[10]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage12()
{
	CheckBoxSetSpecular(11, m_bCheckSpecularImage[11]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage13()
{
	CheckBoxSetSpecular(12, m_bCheckSpecularImage[12]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage14()
{
	CheckBoxSetSpecular(13, m_bCheckSpecularImage[13]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage15()
{
	CheckBoxSetSpecular(14, m_bCheckSpecularImage[14]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage16()
{
	CheckBoxSetSpecular(15, m_bCheckSpecularImage[15]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage17()
{
	CheckBoxSetSpecular(16, m_bCheckSpecularImage[16]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage18()
{
	CheckBoxSetSpecular(17, m_bCheckSpecularImage[17]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage19()
{
	CheckBoxSetSpecular(18, m_bCheckSpecularImage[18]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage20()
{
	CheckBoxSetSpecular(19, m_bCheckSpecularImage[19]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage21()
{
	CheckBoxSetSpecular(20, m_bCheckSpecularImage[20]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage22()
{
	CheckBoxSetSpecular(21, m_bCheckSpecularImage[21]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage23()
{
	CheckBoxSetSpecular(22, m_bCheckSpecularImage[22]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage24()
{
	CheckBoxSetSpecular(23, m_bCheckSpecularImage[23]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage25()
{
	CheckBoxSetSpecular(24, m_bCheckSpecularImage[24]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage26()
{
	CheckBoxSetSpecular(25, m_bCheckSpecularImage[25]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage27()
{
	CheckBoxSetSpecular(26, m_bCheckSpecularImage[26]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage28()
{
	CheckBoxSetSpecular(27, m_bCheckSpecularImage[27]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage29()
{
	CheckBoxSetSpecular(28, m_bCheckSpecularImage[28]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage30()
{
	CheckBoxSetSpecular(29, m_bCheckSpecularImage[29]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage31()
{
	CheckBoxSetSpecular(30, m_bCheckSpecularImage[30]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage32()
{
	CheckBoxSetSpecular(31, m_bCheckSpecularImage[31]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage33()
{
	CheckBoxSetSpecular(32, m_bCheckSpecularImage[32]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage34()
{
	CheckBoxSetSpecular(33, m_bCheckSpecularImage[33]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage35()
{
	CheckBoxSetSpecular(34, m_bCheckSpecularImage[34]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage36()
{
	CheckBoxSetSpecular(35, m_bCheckSpecularImage[35]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage37()
{
	CheckBoxSetSpecular(36, m_bCheckSpecularImage[36]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage38()
{
	CheckBoxSetSpecular(37, m_bCheckSpecularImage[37]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage39()
{
	CheckBoxSetSpecular(38, m_bCheckSpecularImage[38]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage40()
{
	CheckBoxSetSpecular(39, m_bCheckSpecularImage[39]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage41()
{
	CheckBoxSetSpecular(40, m_bCheckSpecularImage[40]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage42()
{
	CheckBoxSetSpecular(41, m_bCheckSpecularImage[41]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage43()
{
	CheckBoxSetSpecular(42, m_bCheckSpecularImage[42]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage44()
{
	CheckBoxSetSpecular(43, m_bCheckSpecularImage[43]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSpecularImage45()
{
	CheckBoxSetSpecular(44, m_bCheckSpecularImage[44]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage1()
{
	CheckBoxSetSequence(0, m_bCheckSeqStartImage[0]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage2()
{
	CheckBoxSetSequence(1, m_bCheckSeqStartImage[1]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage3()
{
	CheckBoxSetSequence(2, m_bCheckSeqStartImage[2]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage4()
{
	CheckBoxSetSequence(3, m_bCheckSeqStartImage[3]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage5()
{
	CheckBoxSetSequence(4, m_bCheckSeqStartImage[4]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage6()
{
	CheckBoxSetSequence(5, m_bCheckSeqStartImage[5]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage7()
{
	CheckBoxSetSequence(6, m_bCheckSeqStartImage[6]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage8()
{
	CheckBoxSetSequence(7, m_bCheckSeqStartImage[7]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage9()
{
	CheckBoxSetSequence(8, m_bCheckSeqStartImage[8]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage10()
{
	CheckBoxSetSequence(9, m_bCheckSeqStartImage[9]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage11()
{
	CheckBoxSetSequence(10, m_bCheckSeqStartImage[10]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage12()
{
	CheckBoxSetSequence(11, m_bCheckSeqStartImage[11]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage13()
{
	CheckBoxSetSequence(12, m_bCheckSeqStartImage[12]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage14()
{
	CheckBoxSetSequence(13, m_bCheckSeqStartImage[13]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage15()
{
	CheckBoxSetSequence(14, m_bCheckSeqStartImage[14]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage16()
{
	CheckBoxSetSequence(15, m_bCheckSeqStartImage[15]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage17()
{
	CheckBoxSetSequence(16, m_bCheckSeqStartImage[16]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage18()
{
	CheckBoxSetSequence(17, m_bCheckSeqStartImage[17]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage19()
{
	CheckBoxSetSequence(18, m_bCheckSeqStartImage[18]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage20()
{
	CheckBoxSetSequence(19, m_bCheckSeqStartImage[19]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage21()
{
	CheckBoxSetSequence(20, m_bCheckSeqStartImage[20]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage22()
{
	CheckBoxSetSequence(21, m_bCheckSeqStartImage[21]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage23()
{
	CheckBoxSetSequence(22, m_bCheckSeqStartImage[22]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage24()
{
	CheckBoxSetSequence(23, m_bCheckSeqStartImage[23]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage25()
{
	CheckBoxSetSequence(24, m_bCheckSeqStartImage[24]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage26()
{
	CheckBoxSetSequence(25, m_bCheckSeqStartImage[25]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage27()
{
	CheckBoxSetSequence(26, m_bCheckSeqStartImage[26]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage28()
{
	CheckBoxSetSequence(27, m_bCheckSeqStartImage[27]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage29()
{
	CheckBoxSetSequence(28, m_bCheckSeqStartImage[28]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage30()
{
	CheckBoxSetSequence(29, m_bCheckSeqStartImage[29]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage31()
{
	CheckBoxSetSequence(30, m_bCheckSeqStartImage[30]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage32()
{
	CheckBoxSetSequence(31, m_bCheckSeqStartImage[31]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage33()
{
	CheckBoxSetSequence(32, m_bCheckSeqStartImage[32]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage34()
{
	CheckBoxSetSequence(33, m_bCheckSeqStartImage[33]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage35()
{
	CheckBoxSetSequence(34, m_bCheckSeqStartImage[34]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage36()
{
	CheckBoxSetSequence(35, m_bCheckSeqStartImage[35]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage37()
{
	CheckBoxSetSequence(36, m_bCheckSeqStartImage[36]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage38()
{
	CheckBoxSetSequence(37, m_bCheckSeqStartImage[37]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage39()
{
	CheckBoxSetSequence(38, m_bCheckSeqStartImage[38]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage40()
{
	CheckBoxSetSequence(39, m_bCheckSeqStartImage[39]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage41()
{
	CheckBoxSetSequence(40, m_bCheckSeqStartImage[40]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage42()
{
	CheckBoxSetSequence(41, m_bCheckSeqStartImage[41]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage43()
{
	CheckBoxSetSequence(42, m_bCheckSeqStartImage[42]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage44()
{
	CheckBoxSetSequence(43, m_bCheckSeqStartImage[43]);
}

void CCameraLightManagerDlg::OnBnClickedCheckSeqStartImage45()
{
	CheckBoxSetSequence(44, m_bCheckSeqStartImage[44]);
}

void CCameraLightManagerDlg::PageChange(int iImageIndex)
{
	UpdateData(TRUE);

	int iPageAddr = 0;
	int iPageAddrCheck = 0;
	int iSeqAddrIndex = m_cbSeqAddrIndex[iImageIndex].GetCurSel();
	int iPageIndex = m_cbPageIndex[iImageIndex].GetCurSel();

	for (int i = 1; i < MAX_IMAGE_TAB; i++)
	{
		if (iImageIndex - i < 0)
			break;

		if (m_cbSeqAddrIndex[iImageIndex - i].GetCurSel() == m_cbSeqAddrIndex[iImageIndex].GetCurSel())
			iPageAddr++;
		else
			break;
	}

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		int iSeqAddrIndexCheck = m_cbSeqAddrIndex[i].GetCurSel();

		if (iSeqAddrIndex == iSeqAddrIndexCheck)
		{
			if (m_bCheckSeqStartImage[i])
				iPageAddrCheck = 0;

			if (iPageAddrCheck == iPageAddr)
			{
				m_cbPageIndex[i].SetCurSel(iPageIndex);
				iPageAddrCheck++;
			}
			else
				iPageAddrCheck++;
		}
		else
			iPageAddrCheck = 0;
	}

	UpdateData(FALSE);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex1()
{
	PageChange(0);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex2()
{
	PageChange(1);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex3()
{
	PageChange(2);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex4()
{
	PageChange(3);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex5()
{
	PageChange(4);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex6()
{
	PageChange(5);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex7()
{
	PageChange(6);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex8()
{
	PageChange(7);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex9()
{
	PageChange(8);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex10()
{
	PageChange(9);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex11()
{
	PageChange(10);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex12()
{
	PageChange(11);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex13()
{
	PageChange(12);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex14()
{
	PageChange(13);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex15()
{
	PageChange(14);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex16()
{
	PageChange(15);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex17()
{
	PageChange(16);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex18()
{
	PageChange(17);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex19()
{
	PageChange(18);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex20()
{
	PageChange(19);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex21()
{
	PageChange(20);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex22()
{
	PageChange(21);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex23()
{
	PageChange(22);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex24()
{
	PageChange(23);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex25()
{
	PageChange(24);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex26()
{
	PageChange(25);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex27()
{
	PageChange(26);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex28()
{
	PageChange(27);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex29()
{
	PageChange(28);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex30()
{
	PageChange(29);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex31()
{
	PageChange(30);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex32()
{
	PageChange(31);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex33()
{
	PageChange(32);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex34()
{
	PageChange(33);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex35()
{
	PageChange(34);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex36()
{
	PageChange(35);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex37()
{
	PageChange(36);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex38()
{
	PageChange(37);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex39()
{
	PageChange(38);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex40()
{
	PageChange(39);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex41()
{
	PageChange(40);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex42()
{
	PageChange(41);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex43()
{
	PageChange(42);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex44()
{
	PageChange(43);
}

void CCameraLightManagerDlg::OnCbnSelchangeComboPageIndex45()
{
	PageChange(44);
}
