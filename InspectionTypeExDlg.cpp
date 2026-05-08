// InspectionTypeExDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "FAIDefine.h"
#include "InspectionTypeExDlg.h"
#include "afxdialogex.h"


// CInspectionTypeExDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInspectionTypeExDlg, CDialog)

CInspectionTypeExDlg::CInspectionTypeExDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectionTypeExDlg::IDD, pParent)
{
	m_iRadioInspectionType = 0;
	m_iRadioFaiRoiNumber = 0;

	m_iPositionID = 0;
	m_iPointID = 0;

	m_bFromPolygon = FALSE;
	m_bGenLineRegion = FALSE;
}

CInspectionTypeExDlg::~CInspectionTypeExDlg()
{
}

void CInspectionTypeExDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_INSPECTION_TYPE_0, m_iRadioInspectionType);
	DDX_Radio(pDX, IDC_RADIO_FAI_PARAMETER_NUMBER_1, m_iRadioFaiRoiNumber);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TYPE2, m_LabelInspectType);
	DDX_Control(pDX, IDC_STATIC_TEACHING, m_LabelTeaching);
	DDX_Control(pDX, IDC_STATIC_MASK, m_LabelMask);
	DDX_Control(pDX, IDC_STATIC_ALIGN2, m_LabelAlign);
	DDX_Control(pDX, IDC_STATIC_CAL2, m_LabelCal);
	DDX_Control(pDX, IDC_STATIC_MEASURE, m_LabelMeasure);
	DDX_Control(pDX, IDC_STATIC_DONTCARE, m_LabelDontCare);
	DDX_Control(pDX, IDC_STATIC_GENERATE, m_LabelGenerate);
	DDX_Control(pDX, IDC_STATIC_AUTOFOCUS, m_LabelAutoFocus);
	DDX_Control(pDX, IDC_STATIC_MEASURE2, m_LabelMeasure2);
	DDX_Control(pDX, IDC_STATIC_MEASURE3, m_LabelMeasure3);

	DDX_Control(pDX, IDC_STATIC_GENERATE_COLOR, m_LabelGenerateColor);
	DDX_Control(pDX, IDC_STATIC_AUTOFOCUS_COLOR, m_LabelAutoFocusColor);
	DDX_Control(pDX, IDC_STATIC_DONTCARE_COLOR, m_LabelDontCareColor);
	DDX_Control(pDX, IDC_STATIC_LALIGN_COLOR, m_LabelLAlignColor);
	DDX_Control(pDX, IDC_STATIC_GALIGN_COLOR, m_LabelGAlignColor);
	DDX_Control(pDX, IDC_STATIC_AALIGN_COLOR, m_LabelAAlignColor);
	DDX_Control(pDX, IDC_STATIC_MEASURE_COLOR, m_LabelMeasureColor);
	DDX_Control(pDX, IDC_STATIC_MEASURE_COLOR2, m_LabelMeasureColor2);
	DDX_Control(pDX, IDC_STATIC_MASK_COLOR, m_LabelMaskColor);

	DDX_Control(pDX, IDC_COMBO_MEASURE_POSITION, m_cbMeasurePos);
	DDX_Control(pDX, IDC_COMBO_MEASURE_POINT, m_cbMeasurePoint);
	DDX_Control(pDX, IDC_COMBO_BLEND_POSITION, m_cbBlendPos);
	DDX_Control(pDX, IDC_COMBO_BLEND_POINT, m_cbBlendPoint);

	DDX_CBIndex(pDX, IDC_COMBO_MEASURE_POSITION, m_iPositionID);
	DDX_CBIndex(pDX, IDC_COMBO_MEASURE_POINT, m_iPointID);
}


BEGIN_MESSAGE_MAP(CInspectionTypeExDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CInspectionTypeExDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CInspectionTypeExDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_BUTTON_GEN_LINE, &CInspectionTypeExDlg::OnBnClickedButtonGenLine)
END_MESSAGE_MAP()


// CInspectionTypeExDlg 메시지 처리기입니다.


BOOL CInspectionTypeExDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


void CInspectionTypeExDlg::OnBnClickedOk()
{
	UpdateData();

	CDialog::OnOK();
}


void CInspectionTypeExDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}


BOOL CInspectionTypeExDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_1, _T(g_strInspectionTypeName[0].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_2, _T(g_strInspectionTypeName[1].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_3, _T(g_strInspectionTypeName[2].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_4, _T(g_strInspectionTypeName[3].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_5, _T(g_strInspectionTypeName[4].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_6, _T(g_strInspectionTypeName[5].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_7, _T(g_strInspectionTypeName[6].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_8, _T(g_strInspectionTypeName[7].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_9, _T(g_strInspectionTypeName[8].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_10, _T(g_strInspectionTypeName[9].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_11, _T(g_strInspectionTypeName[10].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_12, _T(g_strInspectionTypeName[11].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_13, _T(g_strInspectionTypeName[12].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_14, _T(g_strInspectionTypeName[13].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_15, _T(g_strInspectionTypeName[14].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_16, _T(g_strInspectionTypeName[15].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_17, _T(g_strInspectionTypeName[16].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_18, _T(g_strInspectionTypeName[17].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_19, _T(g_strInspectionTypeName[18].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_20, _T(g_strInspectionTypeName[19].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_21, _T(g_strInspectionTypeName[20].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_22, _T(g_strInspectionTypeName[21].c_str()));
	SetDlgItemText(IDC_RADIO_INSPECTION_TYPE_23, _T(g_strInspectionTypeName[22].c_str()));

	m_LabelInspectType.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelTeaching.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelMask.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelAlign.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));

	m_LabelCal.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelMeasure.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelDontCare.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelGenerate.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelAutoFocus.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelMeasure2.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelMeasure3.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));

	m_LabelGenerateColor.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 0, 128), RGB(255, 0, 128));
	m_LabelAutoFocusColor.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(0, 255, 255), RGB(0, 255, 255));
	m_LabelDontCareColor.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(128, 0, 255), RGB(128, 0, 255));
	m_LabelLAlignColor.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 0, 255), RGB(255, 0, 255));
	m_LabelGAlignColor.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(0, 255, 255), RGB(0, 255, 255));
	m_LabelAAlignColor.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(0, 255, 255), RGB(0, 255, 255));
	m_LabelMeasureColor.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 255, 255), RGB(255, 255, 255));
	m_LabelMeasureColor2.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(255, 128, 0), RGB(255, 128, 0));

	m_LabelMaskColor.Init_Ctrl(_T("Arial Black"), 9, FALSE, RGB(0, 0, 255), RGB(0, 0, 255));

	int iVisionCamIdx = THEAPP.m_pCameraManager->GetVisionCamName();

	GetDlgItem(ID_BUTTON_GEN_LINE)->EnableWindow(FALSE);
	if (m_bFromPolygon)
		GetDlgItem(ID_BUTTON_GEN_LINE)->EnableWindow(TRUE);

	// 측정 Point 초기화
	for (const auto &measurePosName : CFAIDataManager::GetInstance().GetMeasurePosNames())
		m_cbMeasurePos.AddString(measurePosName);

	for (int i = 0; i < MAX_MEASURE_POINT_NUM; i++)
	{
		CString strPointNo;
		strPointNo.Format("P%d", i + 1);
		m_cbMeasurePoint.AddString(strPointNo);
	}

	m_cbMeasurePos.SetCurSel(0);
	m_cbMeasurePoint.SetCurSel(0);

	// 합성 Point 초기화
	for (int i = 0; i < POSITION_BLEND_END; i++)
		m_cbBlendPos.AddString(g_strAkcBlendPosName[i]);

	for (int i = 0; i < BLEND_POINT_END; i++)
		m_cbBlendPoint.AddString(g_strBlendPointName[i]);

	m_cbBlendPos.SetCurSel(0);
	m_cbBlendPoint.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CInspectionTypeExDlg::OnBnClickedButtonGenLine()
{
	m_bGenLineRegion = TRUE;

	UpdateData();

	CDialog::OnOK();
}
