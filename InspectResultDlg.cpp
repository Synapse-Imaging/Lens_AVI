// InspectResultDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectResultDlg.h"
#include "afxdialogex.h"


// CInspectResultDlg 대화 상자입니다.
CInspectResultDlg* CInspectResultDlg::m_pInstance = NULL;

CInspectResultDlg* CInspectResultDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectResultDlg();
		if (!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_RESULT_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectResultDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectResultDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}


IMPLEMENT_DYNAMIC(CInspectResultDlg, CDialog)

CInspectResultDlg::CInspectResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectResultDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT + 540, VIEW1_DLG3_TOP + VIEW1_DLG3_HEIGHT + 60 + 100, VIEW1_DLG3_WIDTH - 550, VIEW1_DLG3_HEIGHT + 400);
}

CInspectResultDlg::~CInspectResultDlg()
{

}

void CInspectResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VISION_MODEL_ID, m_LabelVisionModelStatic);
	DDX_Control(pDX, IDC_STATIC_VISION_MODEL_NAME, m_LabelVisionModelName);
	DDX_Control(pDX, IDC_STATIC_OPTICAL_MODEL_ID, m_LabelOpticalModelStatic);
	DDX_Control(pDX, IDC_STATIC_OPTICAL_MODEL_NAME, m_LabelOpticalModelName);

	DDX_Control(pDX, IDC_STATIC_LOTID, m_LabelLotID);
	DDX_Control(pDX, IDC_STATIC_TRAYCOUNT, m_LabelTrayCount);

	DDX_Control(pDX, IDC_STATIC_CONNECT_ADJ1, m_ConnectAdj1);
	DDX_Control(pDX, IDC_STATIC_CONNECT_ADJ2, m_ConnectAdj2);
	DDX_Control(pDX, IDC_STATIC_CONNECT_ADJ3, m_ConnectAdj3);
	DDX_Control(pDX, IDC_STATIC_CONNECT_ADJ4, m_ConnectAdj4);
	DDX_Control(pDX, IDC_STATIC_CONNECT_ADJ5, m_ConnectAdj5);

	DDX_Control(pDX, IDC_STATIC_CONNECT_LAS1, m_ConnectLas1);
	DDX_Control(pDX, IDC_STATIC_CONNECT_LAS2, m_ConnectLas2);
	DDX_Control(pDX, IDC_STATIC_CONNECT_LAS3, m_ConnectLas3);
	DDX_Control(pDX, IDC_STATIC_CONNECT_LAS4, m_ConnectLas4);
	DDX_Control(pDX, IDC_STATIC_CONNECT_LAS5, m_ConnectLas5);

	DDX_Control(pDX, IDC_STATIC_RAW_IMAGE_SAVE_PATH_LAS, m_RawImageSavePathLas);
	DDX_Control(pDX, IDC_STATIC_RAW_IMAGE_SAVE_PATH_LOCAL, m_RawImageSavePathLocal);
	DDX_Control(pDX, IDC_STATIC_RESULT_IMAGE_SAVE_PATH_LAS, m_ResultImageSavePathLas);
	DDX_Control(pDX, IDC_STATIC_RESULT_IMAGE_SAVE_PATH_LOCAL, m_ResultImageSavePathLocal);
	DDX_Control(pDX, IDC_STATIC_REVIEW_IMAGE_SAVE_PATH_LAS, m_ReviewImageSavePathLas);
	DDX_Control(pDX, IDC_STATIC_REVIEW_IMAGE_SAVE_PATH_LOCAL, m_ReviewImageSavePathLocal);
	DDX_Control(pDX, IDC_STATIC_ADJ_IMAGE_SAVE_PATH_LAS, m_ADJImageSavePathLas);
	DDX_Control(pDX, IDC_STATIC_ADJ_IMAGE_SAVE_PATH_LOCAL, m_ADJImageSavePathLocal);
	DDX_Control(pDX, IDC_STATIC_RESULT_LOG_SAVE_PATH_LAS, m_ResultLogSavePathLas);
	DDX_Control(pDX, IDC_STATIC_RESULT_LOG_SAVE_PATH_LOCAL, m_ResultLogSavePathLocal);
}


BEGIN_MESSAGE_MAP(CInspectResultDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_WINDOWPOSCHANGING()
	ON_EN_CHANGE(IDC_EDIT_LOT_ID, &CInspectResultDlg::OnEnChangeEditLotId)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CInspectResultDlg 메시지 처리기입니다.

BOOL CInspectResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	InitStaticText();

	CString strDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_LastStatus(strDataFolder + "LastStatus.txt");
	CString strSection, strKey;

	CString strLastModelName = INI_LastStatus.Get_String(strSection, "ModelID", "Default");
	if (strLastModelName.GetLength() < 35)
		m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(1, 1, 1));
	else
		m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 8, TRUE, RGB(255, 255, 0), RGB(1, 1, 1));
	SetDlgItemText(IDC_STATIC_VISION_MODEL_NAME, strLastModelName);

	CString strOpticalModelName;
	strOpticalModelName.Format("%s_%s_PC%d", THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	m_LabelOpticalModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(1, 1, 1));
	SetDlgItemText(IDC_STATIC_OPTICAL_MODEL_NAME, strOpticalModelName);

	strSection = "Status";
	m_sLastLotID = INI_LastStatus.Get_String(strSection, "LotID", "Default");
	SetDlgItemText(IDC_EDIT_LOT_ID, m_sLastLotID);

	SetDlgItemText(IDC_EDIT_COUNTTRAY, "1");

	ChangeLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CInspectResultDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		SetDlgItemText(IDC_STATIC_VISION_MODEL_ID, _T("비전 모델명"));
		SetDlgItemText(IDC_STATIC_OPTICAL_MODEL_ID, _T("광학 모델명"));
		SetDlgItemText(IDC_STATIC_LOTID, _T("랏 아이디"));
		SetDlgItemText(IDC_STATIC_TRAYCOUNT, _T("트레이 번호"));
		SetDlgItemText(IDC_GROUPBOX_INSPECTRESULTDLG_1, _T("ADJ 연결 상태"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_1, _T("ADJ 1"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_2, _T("ADJ 2"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_3, _T("ADJ 3"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_4, _T("ADJ 4"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_5, _T("ADJ 5"));
		SetDlgItemText(IDC_GROUPBOX_INSPECTRESULTDLG_2, _T("LAS 연결 상태"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_6, _T("LAS 1"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_7, _T("LAS 2"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_8, _T("LAS 3"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_9, _T("LAS 4"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_10, _T("LAS 5"));
		SetDlgItemText(IDC_GROUPBOX_INSPECTRESULTDLG_3, _T("각 항목별 저장 경로"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_11, _T("LAS"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_12, _T("로컬"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_13, _T("원본 영상"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_14, _T("결과 영상"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_15, _T("리뷰 영상"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_16, _T("ADJ 영상"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_17, _T("결과 로그"));
	}
	else
	{
		SetDlgItemText(IDC_STATIC_VISION_MODEL_ID, _T("Vision"));
		SetDlgItemText(IDC_STATIC_OPTICAL_MODEL_ID, _T("Optical"));
		SetDlgItemText(IDC_STATIC_LOTID, _T("Lot ID"));
		SetDlgItemText(IDC_STATIC_TRAYCOUNT, _T("Tray No."));
		SetDlgItemText(IDC_GROUPBOX_INSPECTRESULTDLG_1, _T("ADJ connect status"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_1, _T("ADJ 1"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_2, _T("ADJ 2"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_3, _T("ADJ 3"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_4, _T("ADJ 4"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_5, _T("ADJ 5"));
		SetDlgItemText(IDC_GROUPBOX_INSPECTRESULTDLG_2, _T("LAS connect status"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_6, _T("LAS 1"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_7, _T("LAS 2"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_8, _T("LAS 3"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_9, _T("LAS 4"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_10, _T("LAS 5"));
		SetDlgItemText(IDC_GROUPBOX_INSPECTRESULTDLG_3, _T("Save path each images"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_11, _T("LAS"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_12, _T("Local"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_13, _T("Raw Image"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_14, _T("Result Image"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_15, _T("Review Image"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_16, _T("ADJ Image"));
		SetDlgItemText(IDC_STATIC_INSPECTRESULTDLG_17, _T("Result Log"));
	}
}

void CInspectResultDlg::InitStaticText()
{
	m_LabelVisionModelStatic.Init_Ctrl(_T("Arial Black"), 11, TRUE, RGB(1, 1, 1), RGB(255, 255, 255));
	m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(1, 1, 1));
	m_LabelOpticalModelStatic.Init_Ctrl(_T("Arial Black"), 11, TRUE, RGB(1, 1, 1), RGB(255, 255, 255));
	m_LabelOpticalModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(1, 1, 1));

	m_LabelLotID.Init_Ctrl(_T("Arial Black"), 11, TRUE, RGB(1, 1, 1), RGB(255, 255, 255));
	m_LabelTrayCount.Init_Ctrl(_T("Arial Black"), 11, TRUE, RGB(1, 1, 1), RGB(255, 255, 255));

	m_ConnectAdj1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ConnectAdj2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ConnectAdj3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ConnectAdj4.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ConnectAdj5.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));

	m_ConnectLas1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ConnectLas2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ConnectLas3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ConnectLas4.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ConnectLas5.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));

	m_RawImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_RawImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ResultImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ResultImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ReviewImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ReviewImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ADJImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ADJImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ResultLogSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_ResultLogSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
}

CFont* CInspectResultDlg::GetFont(LONG lfWidth, LONG lfHeight, LONG lfWeight)
{
	CFont* pFont;
	LOGFONT logFont;

	pFont = new CFont();
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfWidth = lfWidth;
	logFont.lfHeight = lfHeight;
	logFont.lfWeight = lfWeight;
	_tcscpy(logFont.lfFaceName, _T("사용자글자"));
	pFont->CreateFontIndirectA(&logFont);

	return pFont;
}

HBRUSH CInspectResultDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

void CInspectResultDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	lpwndpos->flags |= SWP_NOMOVE;

	CDialog::OnWindowPosChanging(lpwndpos);
}

void CInspectResultDlg::OnEnChangeEditLotId()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	// 하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.
}

BOOL CInspectResultDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL bTimeReset = FALSE;
void CInspectResultDlg::OnTimer(UINT_PTR nIDEvent)
{

	CDialog::OnTimer(nIDEvent);
}
