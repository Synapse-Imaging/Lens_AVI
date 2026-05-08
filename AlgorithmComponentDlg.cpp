// AlgorithmComponentDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmComponentDlg.h"
#include "afxdialogex.h"


// CAlgorithmComponentDlg 대화 상자

IMPLEMENT_DYNAMIC(CAlgorithmComponentDlg, CDialog)

CAlgorithmComponentDlg::CAlgorithmComponentDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TEACH_ALGORITHM_COMPONENT_DLG, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseComponent = FALSE;
	m_bCheckComponentApply_1 = TRUE;
	m_bCheckComponentApply_2 = TRUE;
	m_bCheckComponentApply_3 = TRUE;
	m_bCheckComponentApply_5 = TRUE;

	m_dEditComponentScore = 0;
	m_dEditComponentShiftX = 0;
	m_dEditComponentShiftY = 0;
	m_dEditComponentRotationAngle = 0;
	m_dEditComponentRotationAngle2 = 0;
}

CAlgorithmComponentDlg::~CAlgorithmComponentDlg()
{
}

void CAlgorithmComponentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_COMPONENT, m_bCheckUseComponent);

	DDX_Control(pDX, IDC_STATIC_ALIGN_STATUS, m_StaticAlignStatus);

	DDX_Check(pDX, IDC_CHECK_COMPONENT_APPLY_1, m_bCheckComponentApply_1);
	DDX_Control(pDX, IDC_STATIC_DEFECT_1, m_LabelDefect_1);
	DDX_Control(pDX, IDC_COMBO_COMPONENT_DEFECT_TYPE_1, m_cbDefectType_1);

	DDX_Check(pDX, IDC_CHECK_COMPONENT_APPLY_2, m_bCheckComponentApply_2);
	DDX_Text(pDX, IDC_EDIT_COMPONENT_SCORE, m_dEditComponentScore);
	DDX_Control(pDX, IDC_STATIC_DEFECT_2, m_LabelDefect_2);
	DDX_Control(pDX, IDC_COMBO_COMPONENT_DEFECT_TYPE_2, m_cbDefectType_2);

	DDX_Check(pDX, IDC_CHECK_COMPONENT_APPLY_3, m_bCheckComponentApply_3);
	DDX_Text(pDX, IDC_EDIT_COMPONENT_SHIFT_X, m_dEditComponentShiftX);
	DDX_Text(pDX, IDC_EDIT_COMPONENT_SHIFT_Y, m_dEditComponentShiftY);
	DDX_Text(pDX, IDC_EDIT_COMPONENT_ROTATION_ANGLE, m_dEditComponentRotationAngle);
	DDX_Text(pDX, IDC_EDIT_COMPONENT_ROTATION_ANGLE2, m_dEditComponentRotationAngle2);
	DDX_Control(pDX, IDC_STATIC_DEFECT_3, m_LabelDefect_3);
	DDX_Control(pDX, IDC_COMBO_COMPONENT_DEFECT_TYPE_3, m_cbDefectType_3);

	DDX_Check(pDX, IDC_CHECK_COMPONENT_APPLY_5, m_bCheckComponentApply_5);
	DDX_Control(pDX, IDC_STATIC_DEFECT_5, m_LabelDefect_5);
	DDX_Control(pDX, IDC_COMBO_COMPONENT_DEFECT_TYPE_5, m_cbDefectType_5);

}


BEGIN_MESSAGE_MAP(CAlgorithmComponentDlg, CDialog)
END_MESSAGE_MAP()


// CAlgorithmComponentDlg 메시지 처리기

void CAlgorithmComponentDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmComponentDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseComponent = AlgorithmParam.m_bUseComponent;

	m_dEditComponentScore = AlgorithmParam.m_dComponentScore;
	m_dEditComponentShiftX = AlgorithmParam.m_dComponentShiftX;
	m_dEditComponentShiftY = AlgorithmParam.m_dComponentShiftY;
	m_dEditComponentRotationAngle = AlgorithmParam.m_dComponentRotationAngle;

	m_bCheckComponentApply_1 = AlgorithmParam.m_bComponentApply_1;
	m_bCheckComponentApply_2 = AlgorithmParam.m_bComponentApply_2;
	m_bCheckComponentApply_3 = AlgorithmParam.m_bComponentApply_3;

	m_cbDefectType_1.SetCurSel(AlgorithmParam.m_iComponentDefectType1);
	m_cbDefectType_2.SetCurSel(AlgorithmParam.m_iComponentDefectType2);
	m_cbDefectType_3.SetCurSel(AlgorithmParam.m_iComponentDefectType3);

	m_dEditComponentRotationAngle2 = AlgorithmParam.m_dComponentRotationAngle2;
	m_bCheckComponentApply_5 = AlgorithmParam.m_bComponentApply_5;
	m_cbDefectType_5.SetCurSel(AlgorithmParam.m_iComponentDefectType5);
	UpdateData(FALSE);
}

void CAlgorithmComponentDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseComponent = m_bCheckUseComponent;

	pAlgorithmParam->m_dComponentScore = m_dEditComponentScore;
	pAlgorithmParam->m_dComponentShiftX = m_dEditComponentShiftX;
	pAlgorithmParam->m_dComponentShiftY = m_dEditComponentShiftY;
	pAlgorithmParam->m_dComponentRotationAngle = m_dEditComponentRotationAngle;

	pAlgorithmParam->m_bComponentApply_1 = m_bCheckComponentApply_1;
	pAlgorithmParam->m_bComponentApply_2 = m_bCheckComponentApply_2;
	pAlgorithmParam->m_bComponentApply_3 = m_bCheckComponentApply_3;

	pAlgorithmParam->m_iComponentDefectType1 = m_cbDefectType_1.GetCurSel();
	pAlgorithmParam->m_iComponentDefectType2 = m_cbDefectType_2.GetCurSel();
	pAlgorithmParam->m_iComponentDefectType3 = m_cbDefectType_3.GetCurSel();

	pAlgorithmParam->m_dComponentRotationAngle2 = m_dEditComponentRotationAngle2;
	pAlgorithmParam->m_bComponentApply_5 = m_bCheckComponentApply_5;
	pAlgorithmParam->m_iComponentDefectType5 = m_cbDefectType_5.GetCurSel();

}

void CAlgorithmComponentDlg::CheckParamStatus()
{
	CAlgorithmParam AlgorithmParam;
	AlgorithmParam = THEAPP.m_pTabControlDlg->m_pTeachingAlgorithmTabDlg->GetCurTabAlgorithmParam();

	if (AlgorithmParam.m_bUseImageProcessLocalAlign)
	{
		m_StaticAlignStatus.SetWindowText("등록");
		m_StaticAlignStatus.Init_Ctrl(_T("Arial Black"), 11, TRUE, RGB(1, 1, 1), RGB(0, 255, 0));
	}
	else
	{
		m_StaticAlignStatus.SetWindowText("미등록");
		m_StaticAlignStatus.Init_Ctrl(_T("Arial Black"), 11, TRUE, RGB(255, 255, 255), RGB(255, 0, 0));
	}

	UpdateData(FALSE);
}

BOOL CAlgorithmComponentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_LabelDefect_1.Init_Ctrl(_T("Arial Black"), 8, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelDefect_2.Init_Ctrl(_T("Arial Black"), 8, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelDefect_3.Init_Ctrl(_T("Arial Black"), 8, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	m_LabelDefect_5.Init_Ctrl(_T("Arial Black"), 8, FALSE, RGB(255, 255, 255), RGB(1, 1, 1));
	int i;

	m_cbDefectType_1.ResetContent();
	for (i = 0; i < MAX_DEFECT_NAME; i++)
		m_cbDefectType_1.AddString(THEAPP.m_strDefectName[i]);

	m_cbDefectType_2.ResetContent();
	for (i = 0; i < MAX_DEFECT_NAME; i++)
		m_cbDefectType_2.AddString(THEAPP.m_strDefectName[i]);

	m_cbDefectType_3.ResetContent();
	for (i = 0; i < MAX_DEFECT_NAME; i++)
		m_cbDefectType_3.AddString(THEAPP.m_strDefectName[i]);

	m_cbDefectType_5.ResetContent();
	for (i = 0; i < MAX_DEFECT_NAME; i++)
		m_cbDefectType_5.AddString(THEAPP.m_strDefectName[i]);

	m_cbDefectType_1.SetCurSel(0);
	m_cbDefectType_2.SetCurSel(0);
	m_cbDefectType_3.SetCurSel(0);
	m_cbDefectType_5.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}