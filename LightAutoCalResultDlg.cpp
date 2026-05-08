// LightAutoCalResultDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LightAutoCalResultDlg.h"
#include "afxdialogex.h"


// CLightAutoCalResultDlg 대화 상자입니다.

CLightAutoCalResultDlg* CLightAutoCalResultDlg::m_pInstance = NULL;

CLightAutoCalResultDlg* CLightAutoCalResultDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CLightAutoCalResultDlg();
		if (!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_LIGHT_AUTOCAL_RESULT_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CLightAutoCalResultDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

void CLightAutoCalResultDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

IMPLEMENT_DYNAMIC(CLightAutoCalResultDlg, CDialog)

CLightAutoCalResultDlg::CLightAutoCalResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightAutoCalResultDlg::IDD, pParent)
{
	m_bMasterGV = FALSE;
}

CLightAutoCalResultDlg::~CLightAutoCalResultDlg()
{
}

void CLightAutoCalResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_bnOK);
	DDX_Control(pDX, IDC_BUTTON_APPLY_CAL_RESULT, m_bnApplyCalResult);

	DDX_Control(pDX, IDC_STATIC_TITLE_IMAGE, m_LabelTitleImage);
	DDX_Control(pDX, IDC_STATIC_TITLE_VALUE, m_LabelTitleValue);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH1, m_LabelTitleCH1);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH2, m_LabelTitleCH2);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH3, m_LabelTitleCH3);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH4, m_LabelTitleCH4);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH5, m_LabelTitleCH5);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH6, m_LabelTitleCH6);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH7, m_LabelTitleCH7);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH8, m_LabelTitleCH8);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH9, m_LabelTitleCH9);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH10, m_LabelTitleCH10);
	DDX_Control(pDX, IDC_STATIC_TITLE_TOTAL, m_LabelTitleTotal);

	DDX_Control(pDX, IDC_STATIC_IMAGE_1, m_LabelImage[0]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_2, m_LabelImage[1]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_3, m_LabelImage[2]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_4, m_LabelImage[3]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_5, m_LabelImage[4]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_6, m_LabelImage[5]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_7, m_LabelImage[6]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_8, m_LabelImage[7]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_9, m_LabelImage[8]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_10, m_LabelImage[9]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_11, m_LabelImage[10]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_12, m_LabelImage[11]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_13, m_LabelImage[12]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_14, m_LabelImage[13]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_15, m_LabelImage[14]);
	DDX_Control(pDX, IDC_STATIC_IMAGE_16, m_LabelImage[15]);

	DDX_Control(pDX, IDC_STATIC_TITLE_LV_1, m_LabelTitleLV[0]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_1, m_LabelTitleGV[0]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_2, m_LabelTitleLV[1]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_2, m_LabelTitleGV[1]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_3, m_LabelTitleLV[2]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_3, m_LabelTitleGV[2]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_4, m_LabelTitleLV[3]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_4, m_LabelTitleGV[3]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_5, m_LabelTitleLV[4]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_5, m_LabelTitleGV[4]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_6, m_LabelTitleLV[5]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_6, m_LabelTitleGV[5]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_7, m_LabelTitleLV[6]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_7, m_LabelTitleGV[6]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_8, m_LabelTitleLV[7]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_8, m_LabelTitleGV[7]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_9, m_LabelTitleLV[8]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_9, m_LabelTitleGV[8]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_10, m_LabelTitleLV[9]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_10, m_LabelTitleGV[9]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_11, m_LabelTitleLV[10]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_11, m_LabelTitleGV[10]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_12, m_LabelTitleLV[11]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_12, m_LabelTitleGV[11]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_13, m_LabelTitleLV[12]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_13, m_LabelTitleGV[12]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_14, m_LabelTitleLV[13]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_14, m_LabelTitleGV[13]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_15, m_LabelTitleLV[14]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_15, m_LabelTitleGV[14]);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_16, m_LabelTitleLV[15]);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_16, m_LabelTitleGV[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_1, m_LabelTeachCH1_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_1, m_LabelTeachCH1_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_1, m_LabelInspectCH1_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_1, m_LabelInspectCH1_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_1, m_LabelJudgeCH1[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_2, m_LabelTeachCH1_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_2, m_LabelTeachCH1_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_2, m_LabelInspectCH1_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_2, m_LabelInspectCH1_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_2, m_LabelJudgeCH1[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_3, m_LabelTeachCH1_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_3, m_LabelTeachCH1_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_3, m_LabelInspectCH1_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_3, m_LabelInspectCH1_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_3, m_LabelJudgeCH1[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_4, m_LabelTeachCH1_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_4, m_LabelTeachCH1_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_4, m_LabelInspectCH1_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_4, m_LabelInspectCH1_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_4, m_LabelJudgeCH1[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_5, m_LabelTeachCH1_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_5, m_LabelTeachCH1_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_5, m_LabelInspectCH1_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_5, m_LabelInspectCH1_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_5, m_LabelJudgeCH1[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_6, m_LabelTeachCH1_LV[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_6, m_LabelTeachCH1_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_6, m_LabelInspectCH1_LV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_6, m_LabelInspectCH1_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_6, m_LabelJudgeCH1[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_7, m_LabelTeachCH1_LV[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_7, m_LabelTeachCH1_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_7, m_LabelInspectCH1_LV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_7, m_LabelInspectCH1_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_7, m_LabelJudgeCH1[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_8, m_LabelTeachCH1_LV[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_8, m_LabelTeachCH1_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_8, m_LabelInspectCH1_LV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_8, m_LabelInspectCH1_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_8, m_LabelJudgeCH1[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_9, m_LabelTeachCH1_LV[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_9, m_LabelTeachCH1_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_9, m_LabelInspectCH1_LV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_9, m_LabelInspectCH1_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_9, m_LabelJudgeCH1[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_10, m_LabelTeachCH1_LV[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_10, m_LabelTeachCH1_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_10, m_LabelInspectCH1_LV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_10, m_LabelInspectCH1_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_10, m_LabelJudgeCH1[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_11, m_LabelTeachCH1_LV[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_11, m_LabelTeachCH1_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_11, m_LabelInspectCH1_LV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_11, m_LabelInspectCH1_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_11, m_LabelJudgeCH1[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_12, m_LabelTeachCH1_LV[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_12, m_LabelTeachCH1_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_12, m_LabelInspectCH1_LV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_12, m_LabelInspectCH1_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_12, m_LabelJudgeCH1[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_13, m_LabelTeachCH1_LV[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_13, m_LabelTeachCH1_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_13, m_LabelInspectCH1_LV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_13, m_LabelInspectCH1_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_13, m_LabelJudgeCH1[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_14, m_LabelTeachCH1_LV[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_14, m_LabelTeachCH1_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_14, m_LabelInspectCH1_LV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_14, m_LabelInspectCH1_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_14, m_LabelJudgeCH1[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_15, m_LabelTeachCH1_LV[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_15, m_LabelTeachCH1_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_15, m_LabelInspectCH1_LV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_15, m_LabelInspectCH1_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_15, m_LabelJudgeCH1[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV_16, m_LabelTeachCH1_LV[15]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV_16, m_LabelTeachCH1_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV_16, m_LabelInspectCH1_LV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV_16, m_LabelInspectCH1_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1_16, m_LabelJudgeCH1[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_1, m_LabelTeachCH2_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_1, m_LabelTeachCH2_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_1, m_LabelInspectCH2_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_1, m_LabelInspectCH2_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_1, m_LabelJudgeCH2[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_2, m_LabelTeachCH2_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_2, m_LabelTeachCH2_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_2, m_LabelInspectCH2_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_2, m_LabelInspectCH2_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_2, m_LabelJudgeCH2[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_3, m_LabelTeachCH2_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_3, m_LabelTeachCH2_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_3, m_LabelInspectCH2_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_3, m_LabelInspectCH2_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_3, m_LabelJudgeCH2[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_4, m_LabelTeachCH2_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_4, m_LabelTeachCH2_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_4, m_LabelInspectCH2_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_4, m_LabelInspectCH2_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_4, m_LabelJudgeCH2[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_5, m_LabelTeachCH2_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_5, m_LabelTeachCH2_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_5, m_LabelInspectCH2_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_5, m_LabelInspectCH2_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_5, m_LabelJudgeCH2[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_6, m_LabelTeachCH2_LV[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_6, m_LabelTeachCH2_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_6, m_LabelInspectCH2_LV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_6, m_LabelInspectCH2_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_6, m_LabelJudgeCH2[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_7, m_LabelTeachCH2_LV[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_7, m_LabelTeachCH2_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_7, m_LabelInspectCH2_LV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_7, m_LabelInspectCH2_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_7, m_LabelJudgeCH2[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_8, m_LabelTeachCH2_LV[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_8, m_LabelTeachCH2_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_8, m_LabelInspectCH2_LV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_8, m_LabelInspectCH2_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_8, m_LabelJudgeCH2[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_9, m_LabelTeachCH2_LV[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_9, m_LabelTeachCH2_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_9, m_LabelInspectCH2_LV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_9, m_LabelInspectCH2_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_9, m_LabelJudgeCH2[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_10, m_LabelTeachCH2_LV[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_10, m_LabelTeachCH2_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_10, m_LabelInspectCH2_LV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_10, m_LabelInspectCH2_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_10, m_LabelJudgeCH2[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_11, m_LabelTeachCH2_LV[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_11, m_LabelTeachCH2_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_11, m_LabelInspectCH2_LV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_11, m_LabelInspectCH2_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_11, m_LabelJudgeCH2[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_12, m_LabelTeachCH2_LV[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_12, m_LabelTeachCH2_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_12, m_LabelInspectCH2_LV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_12, m_LabelInspectCH2_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_12, m_LabelJudgeCH2[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_13, m_LabelTeachCH2_LV[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_13, m_LabelTeachCH2_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_13, m_LabelInspectCH2_LV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_13, m_LabelInspectCH2_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_13, m_LabelJudgeCH2[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_14, m_LabelTeachCH2_LV[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_14, m_LabelTeachCH2_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_14, m_LabelInspectCH2_LV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_14, m_LabelInspectCH2_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_14, m_LabelJudgeCH2[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_15, m_LabelTeachCH2_LV[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_15, m_LabelTeachCH2_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_15, m_LabelInspectCH2_LV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_15, m_LabelInspectCH2_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_15, m_LabelJudgeCH2[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV_16, m_LabelTeachCH2_LV[15]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV_16, m_LabelTeachCH2_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV_16, m_LabelInspectCH2_LV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV_16, m_LabelInspectCH2_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2_16, m_LabelJudgeCH2[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_1, m_LabelTeachCH3_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_1, m_LabelTeachCH3_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_1, m_LabelInspectCH3_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_1, m_LabelInspectCH3_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_1, m_LabelJudgeCH3[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_2, m_LabelTeachCH3_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_2, m_LabelTeachCH3_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_2, m_LabelInspectCH3_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_2, m_LabelInspectCH3_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_2, m_LabelJudgeCH3[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_3, m_LabelTeachCH3_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_3, m_LabelTeachCH3_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_3, m_LabelInspectCH3_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_3, m_LabelInspectCH3_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_3, m_LabelJudgeCH3[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_4, m_LabelTeachCH3_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_4, m_LabelTeachCH3_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_4, m_LabelInspectCH3_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_4, m_LabelInspectCH3_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_4, m_LabelJudgeCH3[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_5, m_LabelTeachCH3_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_5, m_LabelTeachCH3_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_5, m_LabelInspectCH3_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_5, m_LabelInspectCH3_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_5, m_LabelJudgeCH3[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_6, m_LabelTeachCH3_LV[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_6, m_LabelTeachCH3_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_6, m_LabelInspectCH3_LV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_6, m_LabelInspectCH3_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_6, m_LabelJudgeCH3[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_7, m_LabelTeachCH3_LV[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_7, m_LabelTeachCH3_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_7, m_LabelInspectCH3_LV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_7, m_LabelInspectCH3_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_7, m_LabelJudgeCH3[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_8, m_LabelTeachCH3_LV[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_8, m_LabelTeachCH3_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_8, m_LabelInspectCH3_LV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_8, m_LabelInspectCH3_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_8, m_LabelJudgeCH3[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_9, m_LabelTeachCH3_LV[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_9, m_LabelTeachCH3_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_9, m_LabelInspectCH3_LV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_9, m_LabelInspectCH3_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_9, m_LabelJudgeCH3[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_10, m_LabelTeachCH3_LV[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_10, m_LabelTeachCH3_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_10, m_LabelInspectCH3_LV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_10, m_LabelInspectCH3_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_10, m_LabelJudgeCH3[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_11, m_LabelTeachCH3_LV[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_11, m_LabelTeachCH3_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_11, m_LabelInspectCH3_LV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_11, m_LabelInspectCH3_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_11, m_LabelJudgeCH3[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_12, m_LabelTeachCH3_LV[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_12, m_LabelTeachCH3_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_12, m_LabelInspectCH3_LV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_12, m_LabelInspectCH3_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_12, m_LabelJudgeCH3[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_13, m_LabelTeachCH3_LV[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_13, m_LabelTeachCH3_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_13, m_LabelInspectCH3_LV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_13, m_LabelInspectCH3_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_13, m_LabelJudgeCH3[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_14, m_LabelTeachCH3_LV[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_14, m_LabelTeachCH3_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_14, m_LabelInspectCH3_LV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_14, m_LabelInspectCH3_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_14, m_LabelJudgeCH3[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_15, m_LabelTeachCH3_LV[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_15, m_LabelTeachCH3_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_15, m_LabelInspectCH3_LV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_15, m_LabelInspectCH3_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_15, m_LabelJudgeCH3[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV_16, m_LabelTeachCH3_LV[15]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV_16, m_LabelTeachCH3_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV_16, m_LabelInspectCH3_LV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV_16, m_LabelInspectCH3_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3_16, m_LabelJudgeCH3[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_1, m_LabelTeachCH4_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_1, m_LabelTeachCH4_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_1, m_LabelInspectCH4_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_1, m_LabelInspectCH4_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_1, m_LabelJudgeCH4[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_2, m_LabelTeachCH4_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_2, m_LabelTeachCH4_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_2, m_LabelInspectCH4_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_2, m_LabelInspectCH4_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_2, m_LabelJudgeCH4[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_3, m_LabelTeachCH4_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_3, m_LabelTeachCH4_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_3, m_LabelInspectCH4_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_3, m_LabelInspectCH4_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_3, m_LabelJudgeCH4[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_4, m_LabelTeachCH4_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_4, m_LabelTeachCH4_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_4, m_LabelInspectCH4_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_4, m_LabelInspectCH4_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_4, m_LabelJudgeCH4[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_5, m_LabelTeachCH4_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_5, m_LabelTeachCH4_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_5, m_LabelInspectCH4_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_5, m_LabelInspectCH4_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_5, m_LabelJudgeCH4[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_6, m_LabelTeachCH4_LV[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_6, m_LabelTeachCH4_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_6, m_LabelInspectCH4_LV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_6, m_LabelInspectCH4_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_6, m_LabelJudgeCH4[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_7, m_LabelTeachCH4_LV[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_7, m_LabelTeachCH4_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_7, m_LabelInspectCH4_LV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_7, m_LabelInspectCH4_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_7, m_LabelJudgeCH4[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_8, m_LabelTeachCH4_LV[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_8, m_LabelTeachCH4_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_8, m_LabelInspectCH4_LV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_8, m_LabelInspectCH4_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_8, m_LabelJudgeCH4[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_9, m_LabelTeachCH4_LV[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_9, m_LabelTeachCH4_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_9, m_LabelInspectCH4_LV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_9, m_LabelInspectCH4_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_9, m_LabelJudgeCH4[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_10, m_LabelTeachCH4_LV[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_10, m_LabelTeachCH4_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_10, m_LabelInspectCH4_LV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_10, m_LabelInspectCH4_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_10, m_LabelJudgeCH4[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_11, m_LabelTeachCH4_LV[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_11, m_LabelTeachCH4_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_11, m_LabelInspectCH4_LV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_11, m_LabelInspectCH4_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_11, m_LabelJudgeCH4[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_12, m_LabelTeachCH4_LV[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_12, m_LabelTeachCH4_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_12, m_LabelInspectCH4_LV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_12, m_LabelInspectCH4_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_12, m_LabelJudgeCH4[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_13, m_LabelTeachCH4_LV[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_13, m_LabelTeachCH4_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_13, m_LabelInspectCH4_LV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_13, m_LabelInspectCH4_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_13, m_LabelJudgeCH4[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_14, m_LabelTeachCH4_LV[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_14, m_LabelTeachCH4_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_14, m_LabelInspectCH4_LV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_14, m_LabelInspectCH4_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_14, m_LabelJudgeCH4[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_15, m_LabelTeachCH4_LV[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_15, m_LabelTeachCH4_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_15, m_LabelInspectCH4_LV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_15, m_LabelInspectCH4_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_15, m_LabelJudgeCH4[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV_16, m_LabelTeachCH4_LV[15]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV_16, m_LabelTeachCH4_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV_16, m_LabelInspectCH4_LV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV_16, m_LabelInspectCH4_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4_16, m_LabelJudgeCH4[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_1, m_LabelTeachCH5_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_1, m_LabelTeachCH5_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_1, m_LabelInspectCH5_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_1, m_LabelInspectCH5_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_1, m_LabelJudgeCH5[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_2, m_LabelTeachCH5_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_2, m_LabelTeachCH5_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_2, m_LabelInspectCH5_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_2, m_LabelInspectCH5_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_2, m_LabelJudgeCH5[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_3, m_LabelTeachCH5_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_3, m_LabelTeachCH5_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_3, m_LabelInspectCH5_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_3, m_LabelInspectCH5_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_3, m_LabelJudgeCH5[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_4, m_LabelTeachCH5_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_4, m_LabelTeachCH5_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_4, m_LabelInspectCH5_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_4, m_LabelInspectCH5_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_4, m_LabelJudgeCH5[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_5, m_LabelTeachCH5_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_5, m_LabelTeachCH5_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_5, m_LabelInspectCH5_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_5, m_LabelInspectCH5_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_5, m_LabelJudgeCH5[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_6, m_LabelTeachCH5_LV[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_6, m_LabelTeachCH5_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_6, m_LabelInspectCH5_LV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_6, m_LabelInspectCH5_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_6, m_LabelJudgeCH5[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_7, m_LabelTeachCH5_LV[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_7, m_LabelTeachCH5_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_7, m_LabelInspectCH5_LV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_7, m_LabelInspectCH5_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_7, m_LabelJudgeCH5[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_8, m_LabelTeachCH5_LV[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_8, m_LabelTeachCH5_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_8, m_LabelInspectCH5_LV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_8, m_LabelInspectCH5_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_8, m_LabelJudgeCH5[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_9, m_LabelTeachCH5_LV[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_9, m_LabelTeachCH5_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_9, m_LabelInspectCH5_LV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_9, m_LabelInspectCH5_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_9, m_LabelJudgeCH5[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_10, m_LabelTeachCH5_LV[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_10, m_LabelTeachCH5_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_10, m_LabelInspectCH5_LV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_10, m_LabelInspectCH5_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_10, m_LabelJudgeCH5[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_11, m_LabelTeachCH5_LV[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_11, m_LabelTeachCH5_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_11, m_LabelInspectCH5_LV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_11, m_LabelInspectCH5_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_11, m_LabelJudgeCH5[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_12, m_LabelTeachCH5_LV[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_12, m_LabelTeachCH5_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_12, m_LabelInspectCH5_LV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_12, m_LabelInspectCH5_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_12, m_LabelJudgeCH5[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_13, m_LabelTeachCH5_LV[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_13, m_LabelTeachCH5_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_13, m_LabelInspectCH5_LV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_13, m_LabelInspectCH5_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_13, m_LabelJudgeCH5[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_14, m_LabelTeachCH5_LV[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_14, m_LabelTeachCH5_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_14, m_LabelInspectCH5_LV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_14, m_LabelInspectCH5_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_14, m_LabelJudgeCH5[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_15, m_LabelTeachCH5_LV[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_15, m_LabelTeachCH5_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_15, m_LabelInspectCH5_LV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_15, m_LabelInspectCH5_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_15, m_LabelJudgeCH5[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV_16, m_LabelTeachCH5_LV[15]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV_16, m_LabelTeachCH5_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV_16, m_LabelInspectCH5_LV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV_16, m_LabelInspectCH5_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5_16, m_LabelJudgeCH5[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_1, m_LabelTeachCH6_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_1, m_LabelTeachCH6_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_1, m_LabelInspectCH6_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_1, m_LabelInspectCH6_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_1, m_LabelJudgeCH6[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_2, m_LabelTeachCH6_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_2, m_LabelTeachCH6_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_2, m_LabelInspectCH6_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_2, m_LabelInspectCH6_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_2, m_LabelJudgeCH6[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_3, m_LabelTeachCH6_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_3, m_LabelTeachCH6_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_3, m_LabelInspectCH6_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_3, m_LabelInspectCH6_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_3, m_LabelJudgeCH6[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_4, m_LabelTeachCH6_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_4, m_LabelTeachCH6_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_4, m_LabelInspectCH6_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_4, m_LabelInspectCH6_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_4, m_LabelJudgeCH6[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_5, m_LabelTeachCH6_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_5, m_LabelTeachCH6_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_5, m_LabelInspectCH6_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_5, m_LabelInspectCH6_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_5, m_LabelJudgeCH6[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_6, m_LabelTeachCH6_LV[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_6, m_LabelTeachCH6_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_6, m_LabelInspectCH6_LV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_6, m_LabelInspectCH6_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_6, m_LabelJudgeCH6[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_7, m_LabelTeachCH6_LV[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_7, m_LabelTeachCH6_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_7, m_LabelInspectCH6_LV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_7, m_LabelInspectCH6_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_7, m_LabelJudgeCH6[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_8, m_LabelTeachCH6_LV[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_8, m_LabelTeachCH6_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_8, m_LabelInspectCH6_LV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_8, m_LabelInspectCH6_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_8, m_LabelJudgeCH6[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_9, m_LabelTeachCH6_LV[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_9, m_LabelTeachCH6_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_9, m_LabelInspectCH6_LV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_9, m_LabelInspectCH6_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_9, m_LabelJudgeCH6[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_10, m_LabelTeachCH6_LV[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_10, m_LabelTeachCH6_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_10, m_LabelInspectCH6_LV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_10, m_LabelInspectCH6_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_10, m_LabelJudgeCH6[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_11, m_LabelTeachCH6_LV[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_11, m_LabelTeachCH6_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_11, m_LabelInspectCH6_LV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_11, m_LabelInspectCH6_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_11, m_LabelJudgeCH6[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_12, m_LabelTeachCH6_LV[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_12, m_LabelTeachCH6_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_12, m_LabelInspectCH6_LV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_12, m_LabelInspectCH6_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_12, m_LabelJudgeCH6[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_13, m_LabelTeachCH6_LV[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_13, m_LabelTeachCH6_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_13, m_LabelInspectCH6_LV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_13, m_LabelInspectCH6_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_13, m_LabelJudgeCH6[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_14, m_LabelTeachCH6_LV[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_14, m_LabelTeachCH6_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_14, m_LabelInspectCH6_LV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_14, m_LabelInspectCH6_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_14, m_LabelJudgeCH6[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_15, m_LabelTeachCH6_LV[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_15, m_LabelTeachCH6_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_15, m_LabelInspectCH6_LV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_15, m_LabelInspectCH6_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_15, m_LabelJudgeCH6[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_LV_16, m_LabelTeachCH6_LV[15]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH6_GV_16, m_LabelTeachCH6_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_LV_16, m_LabelInspectCH6_LV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH6_GV_16, m_LabelInspectCH6_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH6_16, m_LabelJudgeCH6[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_1, m_LabelTeachCH7_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_1, m_LabelTeachCH7_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_1, m_LabelInspectCH7_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_1, m_LabelInspectCH7_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_1, m_LabelJudgeCH7[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_2, m_LabelTeachCH7_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_2, m_LabelTeachCH7_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_2, m_LabelInspectCH7_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_2, m_LabelInspectCH7_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_2, m_LabelJudgeCH7[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_3, m_LabelTeachCH7_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_3, m_LabelTeachCH7_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_3, m_LabelInspectCH7_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_3, m_LabelInspectCH7_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_3, m_LabelJudgeCH7[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_4, m_LabelTeachCH7_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_4, m_LabelTeachCH7_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_4, m_LabelInspectCH7_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_4, m_LabelInspectCH7_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_4, m_LabelJudgeCH7[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_5, m_LabelTeachCH7_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_5, m_LabelTeachCH7_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_5, m_LabelInspectCH7_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_5, m_LabelInspectCH7_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_5, m_LabelJudgeCH7[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_6, m_LabelTeachCH7_LV[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_6, m_LabelTeachCH7_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_6, m_LabelInspectCH7_LV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_6, m_LabelInspectCH7_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_6, m_LabelJudgeCH7[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_7, m_LabelTeachCH7_LV[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_7, m_LabelTeachCH7_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_7, m_LabelInspectCH7_LV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_7, m_LabelInspectCH7_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_7, m_LabelJudgeCH7[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_8, m_LabelTeachCH7_LV[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_8, m_LabelTeachCH7_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_8, m_LabelInspectCH7_LV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_8, m_LabelInspectCH7_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_8, m_LabelJudgeCH7[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_9, m_LabelTeachCH7_LV[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_9, m_LabelTeachCH7_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_9, m_LabelInspectCH7_LV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_9, m_LabelInspectCH7_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_9, m_LabelJudgeCH7[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_10, m_LabelTeachCH7_LV[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_10, m_LabelTeachCH7_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_10, m_LabelInspectCH7_LV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_10, m_LabelInspectCH7_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_10, m_LabelJudgeCH7[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_11, m_LabelTeachCH7_LV[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_11, m_LabelTeachCH7_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_11, m_LabelInspectCH7_LV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_11, m_LabelInspectCH7_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_11, m_LabelJudgeCH7[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_12, m_LabelTeachCH7_LV[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_12, m_LabelTeachCH7_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_12, m_LabelInspectCH7_LV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_12, m_LabelInspectCH7_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_12, m_LabelJudgeCH7[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_13, m_LabelTeachCH7_LV[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_13, m_LabelTeachCH7_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_13, m_LabelInspectCH7_LV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_13, m_LabelInspectCH7_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_13, m_LabelJudgeCH7[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_14, m_LabelTeachCH7_LV[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_14, m_LabelTeachCH7_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_14, m_LabelInspectCH7_LV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_14, m_LabelInspectCH7_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_14, m_LabelJudgeCH7[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_15, m_LabelTeachCH7_LV[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_15, m_LabelTeachCH7_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_15, m_LabelInspectCH7_LV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_15, m_LabelInspectCH7_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_15, m_LabelJudgeCH7[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_LV_16, m_LabelTeachCH7_LV[15]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH7_GV_16, m_LabelTeachCH7_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_LV_16, m_LabelInspectCH7_LV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH7_GV_16, m_LabelInspectCH7_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH7_16, m_LabelJudgeCH7[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_1, m_LabelTeachCH8_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_1, m_LabelTeachCH8_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_1, m_LabelInspectCH8_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_1, m_LabelInspectCH8_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_1, m_LabelJudgeCH8[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_2, m_LabelTeachCH8_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_2, m_LabelTeachCH8_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_2, m_LabelInspectCH8_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_2, m_LabelInspectCH8_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_2, m_LabelJudgeCH8[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_3, m_LabelTeachCH8_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_3, m_LabelTeachCH8_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_3, m_LabelInspectCH8_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_3, m_LabelInspectCH8_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_3, m_LabelJudgeCH8[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_4, m_LabelTeachCH8_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_4, m_LabelTeachCH8_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_4, m_LabelInspectCH8_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_4, m_LabelInspectCH8_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_4, m_LabelJudgeCH8[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_5, m_LabelTeachCH8_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_5, m_LabelTeachCH8_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_5, m_LabelInspectCH8_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_5, m_LabelInspectCH8_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_5, m_LabelJudgeCH8[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_6, m_LabelTeachCH8_LV[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_6, m_LabelTeachCH8_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_6, m_LabelInspectCH8_LV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_6, m_LabelInspectCH8_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_6, m_LabelJudgeCH8[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_7, m_LabelTeachCH8_LV[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_7, m_LabelTeachCH8_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_7, m_LabelInspectCH8_LV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_7, m_LabelInspectCH8_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_7, m_LabelJudgeCH8[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_8, m_LabelTeachCH8_LV[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_8, m_LabelTeachCH8_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_8, m_LabelInspectCH8_LV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_8, m_LabelInspectCH8_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_8, m_LabelJudgeCH8[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_9, m_LabelTeachCH8_LV[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_9, m_LabelTeachCH8_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_9, m_LabelInspectCH8_LV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_9, m_LabelInspectCH8_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_9, m_LabelJudgeCH8[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_10, m_LabelTeachCH8_LV[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_10, m_LabelTeachCH8_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_10, m_LabelInspectCH8_LV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_10, m_LabelInspectCH8_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_10, m_LabelJudgeCH8[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_11, m_LabelTeachCH8_LV[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_11, m_LabelTeachCH8_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_11, m_LabelInspectCH8_LV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_11, m_LabelInspectCH8_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_11, m_LabelJudgeCH8[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_12, m_LabelTeachCH8_LV[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_12, m_LabelTeachCH8_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_12, m_LabelInspectCH8_LV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_12, m_LabelInspectCH8_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_12, m_LabelJudgeCH8[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_13, m_LabelTeachCH8_LV[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_13, m_LabelTeachCH8_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_13, m_LabelInspectCH8_LV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_13, m_LabelInspectCH8_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_13, m_LabelJudgeCH8[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_14, m_LabelTeachCH8_LV[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_14, m_LabelTeachCH8_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_14, m_LabelInspectCH8_LV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_14, m_LabelInspectCH8_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_14, m_LabelJudgeCH8[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_15, m_LabelTeachCH8_LV[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_15, m_LabelTeachCH8_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_15, m_LabelInspectCH8_LV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_15, m_LabelInspectCH8_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_15, m_LabelJudgeCH8[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_LV_16, m_LabelTeachCH8_LV[15]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH8_GV_16, m_LabelTeachCH8_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_LV_16, m_LabelInspectCH8_LV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH8_GV_16, m_LabelInspectCH8_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH8_16, m_LabelJudgeCH8[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_1, m_LabelTeachCH9_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_1, m_LabelTeachCH9_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_1, m_LabelInspectCH9_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_1, m_LabelInspectCH9_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_1, m_LabelJudgeCH9[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_2, m_LabelTeachCH9_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_2, m_LabelTeachCH9_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_2, m_LabelInspectCH9_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_2, m_LabelInspectCH9_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_2, m_LabelJudgeCH9[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_3, m_LabelTeachCH9_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_3, m_LabelTeachCH9_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_3, m_LabelInspectCH9_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_3, m_LabelInspectCH9_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_3, m_LabelJudgeCH9[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_4, m_LabelTeachCH9_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_4, m_LabelTeachCH9_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_4, m_LabelInspectCH9_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_4, m_LabelInspectCH9_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_4, m_LabelJudgeCH9[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_5, m_LabelTeachCH9_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_5, m_LabelTeachCH9_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_5, m_LabelInspectCH9_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_5, m_LabelInspectCH9_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_5, m_LabelJudgeCH9[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_6, m_LabelTeachCH9_LV[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_6, m_LabelTeachCH9_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_6, m_LabelInspectCH9_LV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_6, m_LabelInspectCH9_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_6, m_LabelJudgeCH9[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_7, m_LabelTeachCH9_LV[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_7, m_LabelTeachCH9_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_7, m_LabelInspectCH9_LV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_7, m_LabelInspectCH9_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_7, m_LabelJudgeCH9[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_8, m_LabelTeachCH9_LV[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_8, m_LabelTeachCH9_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_8, m_LabelInspectCH9_LV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_8, m_LabelInspectCH9_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_8, m_LabelJudgeCH9[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_9, m_LabelTeachCH9_LV[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_9, m_LabelTeachCH9_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_9, m_LabelInspectCH9_LV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_9, m_LabelInspectCH9_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_9, m_LabelJudgeCH9[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_10, m_LabelTeachCH9_LV[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_10, m_LabelTeachCH9_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_10, m_LabelInspectCH9_LV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_10, m_LabelInspectCH9_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_10, m_LabelJudgeCH9[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_11, m_LabelTeachCH9_LV[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_11, m_LabelTeachCH9_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_11, m_LabelInspectCH9_LV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_11, m_LabelInspectCH9_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_11, m_LabelJudgeCH9[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_12, m_LabelTeachCH9_LV[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_12, m_LabelTeachCH9_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_12, m_LabelInspectCH9_LV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_12, m_LabelInspectCH9_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_12, m_LabelJudgeCH9[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_13, m_LabelTeachCH9_LV[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_13, m_LabelTeachCH9_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_13, m_LabelInspectCH9_LV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_13, m_LabelInspectCH9_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_13, m_LabelJudgeCH9[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_14, m_LabelTeachCH9_LV[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_14, m_LabelTeachCH9_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_14, m_LabelInspectCH9_LV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_14, m_LabelInspectCH9_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_14, m_LabelJudgeCH9[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_15, m_LabelTeachCH9_LV[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_15, m_LabelTeachCH9_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_15, m_LabelInspectCH9_LV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_15, m_LabelInspectCH9_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_15, m_LabelJudgeCH9[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_LV_16, m_LabelTeachCH9_LV[15]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH9_GV_16, m_LabelTeachCH9_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_LV_16, m_LabelInspectCH9_LV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH9_GV_16, m_LabelInspectCH9_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH9_16, m_LabelJudgeCH9[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_1, m_LabelTeachCH10_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_1, m_LabelTeachCH10_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_1, m_LabelInspectCH10_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_1, m_LabelInspectCH10_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_1, m_LabelJudgeCH10[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_2, m_LabelTeachCH10_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_2, m_LabelTeachCH10_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_2, m_LabelInspectCH10_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_2, m_LabelInspectCH10_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_2, m_LabelJudgeCH10[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_3, m_LabelTeachCH10_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_3, m_LabelTeachCH10_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_3, m_LabelInspectCH10_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_3, m_LabelInspectCH10_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_3, m_LabelJudgeCH10[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_4, m_LabelTeachCH10_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_4, m_LabelTeachCH10_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_4, m_LabelInspectCH10_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_4, m_LabelInspectCH10_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_4, m_LabelJudgeCH10[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_5, m_LabelTeachCH10_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_5, m_LabelTeachCH10_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_5, m_LabelInspectCH10_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_5, m_LabelInspectCH10_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_5, m_LabelJudgeCH10[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_6, m_LabelTeachCH10_LV[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_6, m_LabelTeachCH10_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_6, m_LabelInspectCH10_LV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_6, m_LabelInspectCH10_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_6, m_LabelJudgeCH10[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_7, m_LabelTeachCH10_LV[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_7, m_LabelTeachCH10_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_7, m_LabelInspectCH10_LV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_7, m_LabelInspectCH10_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_7, m_LabelJudgeCH10[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_8, m_LabelTeachCH10_LV[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_8, m_LabelTeachCH10_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_8, m_LabelInspectCH10_LV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_8, m_LabelInspectCH10_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_8, m_LabelJudgeCH10[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_9, m_LabelTeachCH10_LV[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_9, m_LabelTeachCH10_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_9, m_LabelInspectCH10_LV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_9, m_LabelInspectCH10_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_9, m_LabelJudgeCH10[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_10, m_LabelTeachCH10_LV[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_10, m_LabelTeachCH10_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_10, m_LabelInspectCH10_LV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_10, m_LabelInspectCH10_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_10, m_LabelJudgeCH10[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_11, m_LabelTeachCH10_LV[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_11, m_LabelTeachCH10_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_11, m_LabelInspectCH10_LV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_11, m_LabelInspectCH10_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_11, m_LabelJudgeCH10[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_12, m_LabelTeachCH10_LV[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_12, m_LabelTeachCH10_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_12, m_LabelInspectCH10_LV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_12, m_LabelInspectCH10_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_12, m_LabelJudgeCH10[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_13, m_LabelTeachCH10_LV[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_13, m_LabelTeachCH10_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_13, m_LabelInspectCH10_LV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_13, m_LabelInspectCH10_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_13, m_LabelJudgeCH10[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_14, m_LabelTeachCH10_LV[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_14, m_LabelTeachCH10_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_14, m_LabelInspectCH10_LV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_14, m_LabelInspectCH10_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_14, m_LabelJudgeCH10[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_15, m_LabelTeachCH10_LV[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_15, m_LabelTeachCH10_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_15, m_LabelInspectCH10_LV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_15, m_LabelInspectCH10_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_15, m_LabelJudgeCH10[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_LV_16, m_LabelTeachCH10_LV[15]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH10_GV_16, m_LabelTeachCH10_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_LV_16, m_LabelInspectCH10_LV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH10_GV_16, m_LabelInspectCH10_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH10_16, m_LabelJudgeCH10[15]);

	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_1, m_LabelTeachTOTAL_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_1, m_LabelInspectTOTAL_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_1, m_LabelJudgeTOTAL[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_2, m_LabelTeachTOTAL_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_2, m_LabelInspectTOTAL_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_2, m_LabelJudgeTOTAL[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_3, m_LabelTeachTOTAL_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_3, m_LabelInspectTOTAL_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_3, m_LabelJudgeTOTAL[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_4, m_LabelTeachTOTAL_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_4, m_LabelInspectTOTAL_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_4, m_LabelJudgeTOTAL[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_5, m_LabelTeachTOTAL_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_5, m_LabelInspectTOTAL_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_5, m_LabelJudgeTOTAL[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_6, m_LabelTeachTOTAL_GV[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_6, m_LabelInspectTOTAL_GV[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_6, m_LabelJudgeTOTAL[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_7, m_LabelTeachTOTAL_GV[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_7, m_LabelInspectTOTAL_GV[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_7, m_LabelJudgeTOTAL[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_8, m_LabelTeachTOTAL_GV[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_8, m_LabelInspectTOTAL_GV[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_8, m_LabelJudgeTOTAL[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_9, m_LabelTeachTOTAL_GV[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_9, m_LabelInspectTOTAL_GV[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_9, m_LabelJudgeTOTAL[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_10, m_LabelTeachTOTAL_GV[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_10, m_LabelInspectTOTAL_GV[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_10, m_LabelJudgeTOTAL[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_11, m_LabelTeachTOTAL_GV[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_11, m_LabelInspectTOTAL_GV[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_11, m_LabelJudgeTOTAL[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_12, m_LabelTeachTOTAL_GV[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_12, m_LabelInspectTOTAL_GV[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_12, m_LabelJudgeTOTAL[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_13, m_LabelTeachTOTAL_GV[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_13, m_LabelInspectTOTAL_GV[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_13, m_LabelJudgeTOTAL[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_14, m_LabelTeachTOTAL_GV[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_14, m_LabelInspectTOTAL_GV[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_14, m_LabelJudgeTOTAL[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_15, m_LabelTeachTOTAL_GV[14]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_15, m_LabelInspectTOTAL_GV[14]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_15, m_LabelJudgeTOTAL[14]);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV_16, m_LabelTeachTOTAL_GV[15]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV_16, m_LabelInspectTOTAL_GV[15]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL_16, m_LabelJudgeTOTAL[15]);
}


BEGIN_MESSAGE_MAP(CLightAutoCalResultDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLightAutoCalResultDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_CAL_RESULT, &CLightAutoCalResultDlg::OnBnClickedButtonApplyCalResult)
END_MESSAGE_MAP()


// CLightAutoCalResultDlg 메시지 처리기입니다.


void CLightAutoCalResultDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnOK();
}

BOOL CLightAutoCalResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//m_bnOK.Set_Text(_T("닫기"));
	m_bnOK.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	//m_bnApplyCalResult.Set_Text(_T("조명 캘리브레이션 결과를 모델에 적용"));
	m_bnApplyCalResult.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xFF, 0x4F, 0x4F), 0, 0);

	m_LabelTitleImage.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleValue.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleCH1.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleCH2.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleCH3.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleCH4.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleCH5.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleCH6.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleCH7.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleCH8.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleCH9.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleCH10.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));

	m_LabelTitleTotal.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));

	/// 조명 채널명 
	CString strChNameFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LightChannel.ini";
	CIniFileCS INI_LIGHTCH(strChNameFileName);

	CString sSection, sKey;
	sSection = THEAPP.m_ModelDefineInfo.m_strVisionName[THEAPP.m_iCurTeachVision];

	CString sLightChannelName;
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "1", "미사용");
	GetDlgItem(IDC_STATIC_TITLE_CH1)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "2", "미사용");
	GetDlgItem(IDC_STATIC_TITLE_CH2)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "3", "미사용");
	GetDlgItem(IDC_STATIC_TITLE_CH3)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "4", "미사용");
	GetDlgItem(IDC_STATIC_TITLE_CH4)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "5", "미사용");
	GetDlgItem(IDC_STATIC_TITLE_CH5)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "6", "미사용");
	GetDlgItem(IDC_STATIC_TITLE_CH6)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "7", "미사용");
	GetDlgItem(IDC_STATIC_TITLE_CH7)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "8", "미사용");
	GetDlgItem(IDC_STATIC_TITLE_CH8)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "9", "미사용");
	GetDlgItem(IDC_STATIC_TITLE_CH9)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "10", "미사용");
	GetDlgItem(IDC_STATIC_TITLE_CH10)->SetWindowTextA(sLightChannelName);

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_LabelImage[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTitleLV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTitleLV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachCH1_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTeachCH1_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH1_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH1_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeCH1[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachCH2_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTeachCH2_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH2_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH2_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeCH2[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachCH3_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTeachCH3_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH3_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH3_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeCH3[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachCH4_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTeachCH4_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH4_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH4_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeCH4[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachCH5_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTeachCH5_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH5_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH5_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeCH5[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachCH6_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTeachCH6_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH6_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH6_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeCH6[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachCH7_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTeachCH7_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH7_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH7_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeCH7[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachCH8_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTeachCH8_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH8_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH8_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeCH8[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachCH9_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTeachCH9_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH9_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH9_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeCH9[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachCH10_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelTeachCH10_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH10_LV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectCH10_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeCH10[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));

		m_LabelTeachTOTAL_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelInspectTOTAL_GV[i].Init_Ctrl(_T("Arial"), 10, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
		m_LabelJudgeTOTAL[i].Init_Ctrl(_T("Arial"), 15, FALSE, RGB(1, 1, 1), RGB(255, 255, 255));
	}

	if (m_bMasterGV)
		GetDlgItem(IDC_BUTTON_APPLY_CAL_RESULT)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BUTTON_APPLY_CAL_RESULT)->EnableWindow(TRUE);

	UpdateResultView();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLightAutoCalResultDlg::UpdateResultView()
{
	CString sTemp, sAutoCalResultLog;
	sAutoCalResultLog = _T("");

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (THEAPP.m_pAutoCalService->m_bAutoCal_Done[i])
		{
			sTemp.Format("영상 %d", i);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i][0]);
			m_LabelTeachCH1_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i][0]);
			m_LabelTeachCH1_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][0]);
			m_LabelInspectCH1_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i][0]);
			m_LabelInspectCH1_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeCH[i][0])
			{
				m_LabelJudgeCH1[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH1[i].SetWindowText("G");
				sTemp += "G\t";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeCH1[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH1[i].SetWindowText("NG");
				sTemp += "NG\t";
				sAutoCalResultLog += sTemp;
			}

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i][1]);
			m_LabelTeachCH2_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i][1]);
			m_LabelTeachCH2_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][1]);
			m_LabelInspectCH2_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i][1]);
			m_LabelInspectCH2_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeCH[i][1])
			{
				m_LabelJudgeCH2[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH2[i].SetWindowText("G");
				sTemp += "G\t";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeCH2[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH2[i].SetWindowText("NG");
				sTemp += "NG\t";
				sAutoCalResultLog += sTemp;
			}

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i][2]);
			m_LabelTeachCH3_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i][2]);
			m_LabelTeachCH3_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][2]);
			m_LabelInspectCH3_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i][2]);
			m_LabelInspectCH3_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeCH[i][2])
			{
				m_LabelJudgeCH3[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH3[i].SetWindowText("G");
				sTemp += "G\t";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeCH3[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH3[i].SetWindowText("NG");
				sTemp += "NG\t";
				sAutoCalResultLog += sTemp;
			}

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i][3]);
			m_LabelTeachCH4_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i][3]);
			m_LabelTeachCH4_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][3]);
			m_LabelInspectCH4_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i][3]);
			m_LabelInspectCH4_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeCH[i][3])
			{
				m_LabelJudgeCH4[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH4[i].SetWindowText("G");
				sTemp += "G\t";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeCH4[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH4[i].SetWindowText("NG");
				sTemp += "NG\t";
				sAutoCalResultLog += sTemp;
			}

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i][4]);
			m_LabelTeachCH5_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i][4]);
			m_LabelTeachCH5_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][4]);
			m_LabelInspectCH5_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i][4]);
			m_LabelInspectCH5_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeCH[i][4])
			{
				m_LabelJudgeCH5[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH5[i].SetWindowText("G");
				sTemp += "G\t";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeCH5[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH5[i].SetWindowText("NG");
				sTemp += "NG\t";
				sAutoCalResultLog += sTemp;
			}

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i][5]);
			m_LabelTeachCH6_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i][5]);
			m_LabelTeachCH6_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][5]);
			m_LabelInspectCH6_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i][5]);
			m_LabelInspectCH6_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeCH[i][5])
			{
				m_LabelJudgeCH6[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH6[i].SetWindowText("G");
				sTemp += "G\t";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeCH6[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH6[i].SetWindowText("NG");
				sTemp += "NG\t";
				sAutoCalResultLog += sTemp;
			}

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i][6]);
			m_LabelTeachCH7_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i][6]);
			m_LabelTeachCH7_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][6]);
			m_LabelInspectCH7_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i][6]);
			m_LabelInspectCH7_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeCH[i][6])
			{
				m_LabelJudgeCH7[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH7[i].SetWindowText("G");
				sTemp += "G\t";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeCH7[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH7[i].SetWindowText("NG");
				sTemp += "NG\t";
				sAutoCalResultLog += sTemp;
			}

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i][7]);
			m_LabelTeachCH8_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i][7]);
			m_LabelTeachCH8_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][7]);
			m_LabelInspectCH8_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i][7]);
			m_LabelInspectCH8_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeCH[i][7])
			{
				m_LabelJudgeCH8[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH8[i].SetWindowText("G");
				sTemp += "G\t";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeCH8[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH8[i].SetWindowText("NG");
				sTemp += "NG\t";
				sAutoCalResultLog += sTemp;
			}

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i][8]);
			m_LabelTeachCH9_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i][8]);
			m_LabelTeachCH9_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][8]);
			m_LabelInspectCH9_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i][8]);
			m_LabelInspectCH9_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeCH[i][8])
			{
				m_LabelJudgeCH9[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH9[i].SetWindowText("G");
				sTemp += "G\t";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeCH9[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH9[i].SetWindowText("NG");
				sTemp += "NG\t";
				sAutoCalResultLog += sTemp;
			}

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i][9]);
			m_LabelTeachCH10_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i][9]);
			m_LabelTeachCH10_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][9]);
			m_LabelInspectCH10_LV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i][9]);
			m_LabelInspectCH10_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeCH[i][9])
			{
				m_LabelJudgeCH10[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH10[i].SetWindowText("G");
				sTemp += "G\t";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeCH10[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeCH10[i].SetWindowText("NG");
				sTemp += "NG\t";
				sAutoCalResultLog += sTemp;
			}

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachTOTAL_GV[i]);
			m_LabelTeachTOTAL_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectTOTAL_GV[i]);
			m_LabelInspectTOTAL_GV[i].SetWindowText(sTemp);
			sTemp += "\t";
			sAutoCalResultLog += sTemp;
			if (THEAPP.m_pAutoCalService->m_bJudgeTOTAL[i])
			{
				m_LabelJudgeTOTAL[i].Set_Color(RGB(1, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeTOTAL[i].SetWindowText("G");
				sTemp += "G";
				sAutoCalResultLog += sTemp;
			}
			else
			{
				m_LabelJudgeTOTAL[i].Set_Color(RGB(255, 1, 1), RGB(255, 255, 255));
				m_LabelJudgeTOTAL[i].SetWindowText("NG");
				sTemp += "NG";
				sAutoCalResultLog += sTemp;
			}
		}
	}
}

void CLightAutoCalResultDlg::OnBnClickedButtonApplyCalResult()
{
	int nRes = 0;
	nRes = AfxMessageBox("조명 캘리브레이션 결과를 현재 모델에 적용하시겠습니까?", MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	BOOL bLightControllerPortOpen = FALSE;
	int iComPortNumber = THEAPP.m_pModelDataManager->m_PageControlData.m_iComPortNumber;
	if (THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.OpenPort(iComPortNumber, 19200))
		bLightControllerPortOpen = TRUE;

	if (bLightControllerPortOpen == FALSE)
	{
		AfxMessageBox("조명 Controller Port Open 오류!!.", MB_ICONERROR | MB_SYSTEMMODAL);
		return;
	}

	int iCurPageIndex = 0;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (THEAPP.m_pAutoCalService->m_bAutoCal_Done[i])
		{
			iCurPageIndex = THEAPP.m_pModelDataManager->m_iLightPageIdx[i];

			THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iCurPageIndex].uiChannel[0] = THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][0];
			THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iCurPageIndex].uiChannel[1] = THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][1];
			THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iCurPageIndex].uiChannel[2] = THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][2];
			THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iCurPageIndex].uiChannel[3] = THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][3];
			THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iCurPageIndex].uiChannel[4] = THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][4];
			THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iCurPageIndex].uiChannel[5] = THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][5];
			THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iCurPageIndex].uiChannel[6] = THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][6];
			THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iCurPageIndex].uiChannel[7] = THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][7];
			THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iCurPageIndex].uiChannel[8] = THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][8];
			THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iCurPageIndex].uiChannel[9] = THEAPP.m_pAutoCalService->m_iInspectCH_LV[i][9];

#ifdef INLINE_MODE
			THEAPP.m_pModelDataManager->m_PageControlData.SetIllumination_10CH(iCurPageIndex);
#endif
		}
	}

	THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.ClosePort();

	//**********  조명 값을 파일에 써줌
	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	CString strInspectLightInfo;
	strInspectLightInfo.Format("%s\\HW\\Vision_N%d\\InspectLightInfo.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveLightInfo(strInspectLightInfo, -1);

	AfxMessageBox("캘리브레이션 결과가 현재 모델에 적용되었습니다.", MB_ICONINFORMATION | MB_SYSTEMMODAL);
}


BOOL CLightAutoCalResultDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
