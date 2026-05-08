// FocusAutoCalResultDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "FocusAutoCalResultDlg.h"
#include "afxdialogex.h"


// CFocusAutoCalResultDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFocusAutoCalResultDlg, CDialog)

CFocusAutoCalResultDlg::CFocusAutoCalResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFocusAutoCalResultDlg::IDD, pParent)
{

}

CFocusAutoCalResultDlg::~CFocusAutoCalResultDlg()
{
}

void CFocusAutoCalResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_bnOK);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);

	DDX_Control(pDX, IDC_STATIC_TITLE, m_LabelTitle);
	DDX_Control(pDX, IDC_STATIC_TITLE_ZPOS, m_LabelTitleZPos);
	DDX_Control(pDX, IDC_STATIC_TITLE_EV, m_LabelTitleEdgeValue);
	DDX_Control(pDX, IDC_STATIC_TITLE_JUDGE, m_LabelTitleJudge);
	DDX_Control(pDX, IDC_STATIC_TEACH, m_LabelTitleTeach[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT, m_LabelTitleInspect[0]);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_LabelImageIndex[0]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS, m_LabelTeachZPos[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV, m_LabelTeachEdgeValue[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS, m_LabelInspectZPos[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV, m_LabelInspectEdgeValue[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE, m_LabelJudge[0]);

	DDX_Control(pDX, IDC_STATIC_TEACH2, m_LabelTitleTeach[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT2, m_LabelTitleInspect[1]);
	DDX_Control(pDX, IDC_STATIC_IMAGE2, m_LabelImageIndex[1]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS2, m_LabelTeachZPos[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV2, m_LabelTeachEdgeValue[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS2, m_LabelInspectZPos[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV2, m_LabelInspectEdgeValue[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE2, m_LabelJudge[1]);

	DDX_Control(pDX, IDC_STATIC_TEACH3, m_LabelTitleTeach[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT3, m_LabelTitleInspect[2]);
	DDX_Control(pDX, IDC_STATIC_IMAGE3, m_LabelImageIndex[2]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS3, m_LabelTeachZPos[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV3, m_LabelTeachEdgeValue[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS3, m_LabelInspectZPos[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV3, m_LabelInspectEdgeValue[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE3, m_LabelJudge[2]);

	DDX_Control(pDX, IDC_STATIC_TEACH4, m_LabelTitleTeach[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT4, m_LabelTitleInspect[3]);
	DDX_Control(pDX, IDC_STATIC_IMAGE4, m_LabelImageIndex[3]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS4, m_LabelTeachZPos[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV4, m_LabelTeachEdgeValue[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS4, m_LabelInspectZPos[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV4, m_LabelInspectEdgeValue[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE4, m_LabelJudge[3]);

	DDX_Control(pDX, IDC_STATIC_TEACH5, m_LabelTitleTeach[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT5, m_LabelTitleInspect[4]);
	DDX_Control(pDX, IDC_STATIC_IMAGE5, m_LabelImageIndex[4]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS5, m_LabelTeachZPos[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV5, m_LabelTeachEdgeValue[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS5, m_LabelInspectZPos[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV5, m_LabelInspectEdgeValue[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE5, m_LabelJudge[4]);

	DDX_Control(pDX, IDC_STATIC_TEACH6, m_LabelTitleTeach[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT6, m_LabelTitleInspect[5]);
	DDX_Control(pDX, IDC_STATIC_IMAGE6, m_LabelImageIndex[5]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS6, m_LabelTeachZPos[5]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV6, m_LabelTeachEdgeValue[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS6, m_LabelInspectZPos[5]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV6, m_LabelInspectEdgeValue[5]);
	DDX_Control(pDX, IDC_STATIC_JUDGE6, m_LabelJudge[5]);

	DDX_Control(pDX, IDC_STATIC_TEACH7, m_LabelTitleTeach[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT7, m_LabelTitleInspect[6]);
	DDX_Control(pDX, IDC_STATIC_IMAGE7, m_LabelImageIndex[6]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS7, m_LabelTeachZPos[6]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV7, m_LabelTeachEdgeValue[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS7, m_LabelInspectZPos[6]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV7, m_LabelInspectEdgeValue[6]);
	DDX_Control(pDX, IDC_STATIC_JUDGE7, m_LabelJudge[6]);

	DDX_Control(pDX, IDC_STATIC_TEACH8, m_LabelTitleTeach[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT8, m_LabelTitleInspect[7]);
	DDX_Control(pDX, IDC_STATIC_IMAGE8, m_LabelImageIndex[7]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS8, m_LabelTeachZPos[7]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV8, m_LabelTeachEdgeValue[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS8, m_LabelInspectZPos[7]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV8, m_LabelInspectEdgeValue[7]);
	DDX_Control(pDX, IDC_STATIC_JUDGE8, m_LabelJudge[7]);

	DDX_Control(pDX, IDC_STATIC_TEACH9, m_LabelTitleTeach[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT9, m_LabelTitleInspect[8]);
	DDX_Control(pDX, IDC_STATIC_IMAGE9, m_LabelImageIndex[8]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS9, m_LabelTeachZPos[8]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV9, m_LabelTeachEdgeValue[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS9, m_LabelInspectZPos[8]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV9, m_LabelInspectEdgeValue[8]);
	DDX_Control(pDX, IDC_STATIC_JUDGE9, m_LabelJudge[8]);

	DDX_Control(pDX, IDC_STATIC_TEACH10, m_LabelTitleTeach[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT10, m_LabelTitleInspect[9]);
	DDX_Control(pDX, IDC_STATIC_IMAGE10, m_LabelImageIndex[9]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS10, m_LabelTeachZPos[9]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV10, m_LabelTeachEdgeValue[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS10, m_LabelInspectZPos[9]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV10, m_LabelInspectEdgeValue[9]);
	DDX_Control(pDX, IDC_STATIC_JUDGE10, m_LabelJudge[9]);

	DDX_Control(pDX, IDC_STATIC_TEACH11, m_LabelTitleTeach[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT11, m_LabelTitleInspect[10]);
	DDX_Control(pDX, IDC_STATIC_IMAGE11, m_LabelImageIndex[10]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS11, m_LabelTeachZPos[10]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV11, m_LabelTeachEdgeValue[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS11, m_LabelInspectZPos[10]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV11, m_LabelInspectEdgeValue[10]);
	DDX_Control(pDX, IDC_STATIC_JUDGE11, m_LabelJudge[10]);

	DDX_Control(pDX, IDC_STATIC_TEACH12, m_LabelTitleTeach[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT12, m_LabelTitleInspect[11]);
	DDX_Control(pDX, IDC_STATIC_IMAGE12, m_LabelImageIndex[11]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS12, m_LabelTeachZPos[11]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV12, m_LabelTeachEdgeValue[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS12, m_LabelInspectZPos[11]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV12, m_LabelInspectEdgeValue[11]);
	DDX_Control(pDX, IDC_STATIC_JUDGE12, m_LabelJudge[11]);

	DDX_Control(pDX, IDC_STATIC_TEACH13, m_LabelTitleTeach[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT13, m_LabelTitleInspect[12]);
	DDX_Control(pDX, IDC_STATIC_IMAGE13, m_LabelImageIndex[12]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS13, m_LabelTeachZPos[12]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV13, m_LabelTeachEdgeValue[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS13, m_LabelInspectZPos[12]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV13, m_LabelInspectEdgeValue[12]);
	DDX_Control(pDX, IDC_STATIC_JUDGE13, m_LabelJudge[12]);

	DDX_Control(pDX, IDC_STATIC_TEACH14, m_LabelTitleTeach[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT14, m_LabelTitleInspect[13]);
	DDX_Control(pDX, IDC_STATIC_IMAGE14, m_LabelImageIndex[13]);

	DDX_Control(pDX, IDC_STATIC_TEACH_ZPOS14, m_LabelTeachZPos[13]);
	DDX_Control(pDX, IDC_STATIC_TEACH_EV14, m_LabelTeachEdgeValue[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_ZPOS14, m_LabelInspectZPos[13]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_EV14, m_LabelInspectEdgeValue[13]);
	DDX_Control(pDX, IDC_STATIC_JUDGE14, m_LabelJudge[13]);
}


BEGIN_MESSAGE_MAP(CFocusAutoCalResultDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFocusAutoCalResultDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFocusAutoCalResultDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CFocusAutoCalResultDlg 메시지 처리기입니다.


void CFocusAutoCalResultDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnOK();
}


void CFocusAutoCalResultDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}


BOOL CFocusAutoCalResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//m_bnOK.Set_Text(_T("확인"));
	m_bnOK.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	//m_bnCancel.Set_Text(_T("취소"));
	m_bnCancel.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_LabelTitle.Init_Ctrl(_T("Arial"), 18, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleZPos.Init_Ctrl(_T("Arial"), 18, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleEdgeValue.Init_Ctrl(_T("Arial"), 18, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTitleJudge.Init_Ctrl(_T("Arial"), 18, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_LabelTitleTeach[i].Init_Ctrl(_T("Arial"), 18, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_LabelTitleInspect[i].Init_Ctrl(_T("Arial"), 18, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_LabelTeachZPos[i].Init_Ctrl(_T("Arial"), 18, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_LabelTeachEdgeValue[i].Init_Ctrl(_T("Arial"), 18, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_LabelInspectZPos[i].Init_Ctrl(_T("Arial"), 18, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_LabelInspectEdgeValue[i].Init_Ctrl(_T("Arial"), 18, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_LabelJudge[i].Init_Ctrl(_T("Arial"), 22, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	}

	UpdateResultView();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFocusAutoCalResultDlg::UpdateResultView()
{
	CString sTemp;
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sTemp.Format("%.3lf", THEAPP.m_pAutoCalService->m_dTeachZPos[i]);
		m_LabelTeachZPos[i].SetWindowText(sTemp);

		sTemp.Format("%.3lf", THEAPP.m_pAutoCalService->m_dTeachEdgeValue[i]);
		m_LabelTeachEdgeValue[i].SetWindowText(sTemp);

		sTemp.Format("%.3lf", THEAPP.m_pAutoCalService->m_dInspectZPos[i]);
		m_LabelInspectZPos[i].SetWindowText(sTemp);

		sTemp.Format("%.3lf", THEAPP.m_pAutoCalService->m_dInspectEdgeValue[i]);
		m_LabelInspectEdgeValue[i].SetWindowText(sTemp);

		if (THEAPP.m_pAutoCalService->m_bJudge[i])
		{
			m_LabelJudge[i].Set_Color(RGB(255, 255, 255), RGB(1, 1, 1));
			m_LabelJudge[i].SetWindowText("G");
		}
		else
		{
			m_LabelJudge[i].Set_Color(RGB(255, 0, 0), RGB(1, 1, 1));
			m_LabelJudge[i].SetWindowText("NG");
		}
	}
}
