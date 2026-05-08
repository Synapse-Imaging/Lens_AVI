// TeachingAlgorithmTabDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TeachingAlgorithmTabDlg.h"
#include "afxdialogex.h"
#include "AIService/ImageUtile.h"

// CTeachingAlgorithmTabDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachingAlgorithmTabDlg, CDialog)

CTeachingAlgorithmTabDlg* CTeachingAlgorithmTabDlg::m_pInstance = NULL;

CTeachingAlgorithmTabDlg* CTeachingAlgorithmTabDlg::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CTeachingAlgorithmTabDlg;

		if (!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

			m_pInstance->Create(IDD_TEACH_ALGORITHM_TAB_DLG, pFrame->GetActiveView());
		}
	}

	return m_pInstance;
}

void CTeachingAlgorithmTabDlg::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	if (ai_setup_process_handle_ != nullptr) // 프로세스 핸들 정리
	{
		CloseHandle(ai_setup_process_handle_);
		ai_setup_process_handle_ = nullptr;
	}
	SAFE_DELETE(m_pInstance);
}

void CTeachingAlgorithmTabDlg::Show()
{
	m_AlgorithmTabCtrl.SetCurSel(0);
	m_pAlgorithmImageProcessDlg->ShowWindow(SW_SHOW);
	m_pAlgorithmDontcareDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmSurfaceDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmDefectConditionDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmFittingDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmOCRDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmBarcodeDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmConnectorPinDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmDentDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmImageCompareDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmPartCheckDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmROIAlignDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmConcenterMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmShapeDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmEdgeMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmSurfaceDualDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmHomerTiltDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmEpoxyVoidHoleDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmPrintQualityDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmROIAnisoAlignDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmStiffenerEpoxyDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmDistanceMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmGapMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmComponentDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmNeckEpoxyMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmNeckEpoxyCrackDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmCornerMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmHeightMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmBoundaryDlg->ShowWindow(SW_HIDE);

	SetWindowPos(&wndTopMost, 1000, 5, 0, 0, SWP_NOSIZE);
	ShowWindow(SW_SHOW);

	m_bShowDlg = TRUE;
}

void CTeachingAlgorithmTabDlg::Hide()
{
	ShowWindow(SW_HIDE);

	m_bShowDlg = FALSE;
}

CTeachingAlgorithmTabDlg::CTeachingAlgorithmTabDlg(CWnd* pParent /*=NULL*/) : CDialog(CTeachingAlgorithmTabDlg::IDD, pParent)
{
	m_pAlgorithmDontcareDlg = NULL;
	m_pAlgorithmSurfaceDlg = NULL;
	m_pAlgorithmDefectConditionDlg = NULL;
	m_pAlgorithmImageProcessDlg = NULL;
	m_pAlgorithmFittingDlg = NULL;
	m_pAlgorithmOCRDlg = NULL;
	m_pAlgorithmBarcodeDlg = NULL;
	m_pAlgorithmConnectorPinDlg = NULL;
	m_pAlgorithmDentDlg = NULL;
	m_pAlgorithmImageCompareDlg = NULL;
	m_pAlgorithmPartCheckDlg = NULL;
	m_pAlgorithmROIAlignDlg = NULL;
	m_pAlgorithmConcenterMeasureDlg = NULL;
	m_pAlgorithmShapeDlg = NULL;
	m_pAlgorithmEdgeMeasureDlg = NULL;
	m_pAlgorithmSurfaceDualDlg = NULL;
	m_pAlgorithmHomerTiltDlg = NULL;
	m_pAlgorithmEpoxyVoidHoleDlg = NULL;
	m_pAlgorithmPrintQualityDlg = NULL;
	m_pAlgorithmROIAnisoAlignDlg = NULL;
	m_pAlgorithmStiffenerEpoxyDlg = NULL;
	m_pAlgorithmDistanceMeasureDlg = NULL;
	m_pAlgorithmGapMeasureDlg = NULL;
	m_pAlgorithmComponentDlg = NULL;
	m_pAlgorithmNeckEpoxyMeasureDlg = NULL;
	m_pAlgorithmNeckEpoxyCrackDlg = NULL;
	m_pAlgorithmCornerMeasureDlg = NULL;
	m_pAlgorithmHeightMeasureDlg = NULL;
	m_pAlgorithmBoundaryDlg = NULL;

	m_bShowDlg = FALSE;

	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;
}

CTeachingAlgorithmTabDlg::~CTeachingAlgorithmTabDlg()
{
	SAFE_DELETE(m_pAlgorithmDontcareDlg);
	SAFE_DELETE(m_pAlgorithmSurfaceDlg);
	SAFE_DELETE(m_pAlgorithmDefectConditionDlg);
	SAFE_DELETE(m_pAlgorithmImageProcessDlg);
	SAFE_DELETE(m_pAlgorithmFittingDlg);
	SAFE_DELETE(m_pAlgorithmOCRDlg);
	SAFE_DELETE(m_pAlgorithmBarcodeDlg);
	SAFE_DELETE(m_pAlgorithmConnectorPinDlg);
	SAFE_DELETE(m_pAlgorithmDentDlg);
	SAFE_DELETE(m_pAlgorithmImageCompareDlg);
	SAFE_DELETE(m_pAlgorithmPartCheckDlg);
	SAFE_DELETE(m_pAlgorithmROIAlignDlg);
	SAFE_DELETE(m_pAlgorithmConcenterMeasureDlg);
	SAFE_DELETE(m_pAlgorithmShapeDlg);
	SAFE_DELETE(m_pAlgorithmEdgeMeasureDlg);
	SAFE_DELETE(m_pAlgorithmSurfaceDualDlg);
	SAFE_DELETE(m_pAlgorithmHomerTiltDlg);
	SAFE_DELETE(m_pAlgorithmEpoxyVoidHoleDlg);
	SAFE_DELETE(m_pAlgorithmPrintQualityDlg);
	SAFE_DELETE(m_pAlgorithmROIAnisoAlignDlg);
	SAFE_DELETE(m_pAlgorithmStiffenerEpoxyDlg);
	SAFE_DELETE(m_pAlgorithmDistanceMeasureDlg);
	SAFE_DELETE(m_pAlgorithmGapMeasureDlg);
	SAFE_DELETE(m_pAlgorithmComponentDlg);
	SAFE_DELETE(m_pAlgorithmNeckEpoxyMeasureDlg);
	SAFE_DELETE(m_pAlgorithmNeckEpoxyCrackDlg);
	SAFE_DELETE(m_pAlgorithmCornerMeasureDlg);
	SAFE_DELETE(m_pAlgorithmHeightMeasureDlg);
	SAFE_DELETE(m_pAlgorithmBoundaryDlg);
}

void CTeachingAlgorithmTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ALGORITHM_TAB, m_AlgorithmTabCtrl);
	DDX_Control(pDX, IDC_BUTTON_TEST, m_bnTest);
	DDX_Control(pDX, IDC_BUTTON_VISION_TEST, m_bnVisionTest);
	DDX_Control(pDX, IDC_BUTTON_AI_TEST, m_bnAiTest);
	DDX_Control(pDX, IDC_BUTTON_SAVE_AND_CLOSE, m_bnSaveClose);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_bnClose);
	DDX_Control(pDX, IDC_BUTTON_CONVERT_UNIT, m_bnConvertUnit);
	DDX_Control(pDX, IDC_BUTTON_AISETUP, m_bnAiSetup);
	DDX_Control(pDX, IDC_BUTTON_AISETUP_APPLY, m_bnAiApply);
}


BEGIN_MESSAGE_MAP(CTeachingAlgorithmTabDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_ALGORITHM_TAB, &CTeachingAlgorithmTabDlg::OnSelchangeAlgorithmTab)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CTeachingAlgorithmTabDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_VISION_TEST, &CTeachingAlgorithmTabDlg::OnBnClickedMfcbuttonVisionTest)
	ON_BN_CLICKED(IDC_BUTTON_AI_TEST, &CTeachingAlgorithmTabDlg::OnBnClickedMfcbuttonAiTest)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_AND_CLOSE, &CTeachingAlgorithmTabDlg::OnBnClickedButtonSaveAndClose)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CTeachingAlgorithmTabDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT_UNIT, &CTeachingAlgorithmTabDlg::OnBnClickedButtonConvertUnit)
	ON_BN_CLICKED(IDC_BUTTON_AISETUP, &CTeachingAlgorithmTabDlg::OnBnClickedButtonAisetup)
	ON_BN_CLICKED(IDC_BUTTON_AISETUP_APPLY, &CTeachingAlgorithmTabDlg::OnBnClickedButtonAisetupApply)
END_MESSAGE_MAP()


// CTeachingAlgorithmTabDlg 메시지 처리기입니다.

BOOL CTeachingAlgorithmTabDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

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


BOOL CTeachingAlgorithmTabDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTopMost, 1000, 5, 0, 0, SWP_NOSIZE);

	m_AlgorithmTabCtrl.InsertItem(0, "영상 전처리");
	m_AlgorithmTabCtrl.InsertItem(1, "ROI Align");
	m_AlgorithmTabCtrl.InsertItem(2, "비등방 ROI Align");
	m_AlgorithmTabCtrl.InsertItem(3, "검사제외 Blob");
	m_AlgorithmTabCtrl.InsertItem(4, "Part 유무체크");
	m_AlgorithmTabCtrl.InsertItem(5, "Edge 측정");
	m_AlgorithmTabCtrl.InsertItem(6, "Dual Blob 분석");
	m_AlgorithmTabCtrl.InsertItem(7, "요철영상 변환");
	m_AlgorithmTabCtrl.InsertItem(8, "Blob 분석");
	m_AlgorithmTabCtrl.InsertItem(9, "영상비교");
	m_AlgorithmTabCtrl.InsertItem(10, "불량 검출 조건");
	m_AlgorithmTabCtrl.InsertItem(11, "라인 Fitting");
	m_AlgorithmTabCtrl.InsertItem(12, "OCR");
	m_AlgorithmTabCtrl.InsertItem(13, "바코드");
	m_AlgorithmTabCtrl.InsertItem(14, "바코드 품질검사");
	m_AlgorithmTabCtrl.InsertItem(15, "커넥터 핀 휨");
	m_AlgorithmTabCtrl.InsertItem(16, "Dent");
	m_AlgorithmTabCtrl.InsertItem(17, "동심도 검사");
	m_AlgorithmTabCtrl.InsertItem(18, "부품 검사");
	m_AlgorithmTabCtrl.InsertItem(19, "Tilt 검사");
	m_AlgorithmTabCtrl.InsertItem(20, "Epoxy Void Hole");
	m_AlgorithmTabCtrl.InsertItem(21, "Stiffener Epoxy");
	m_AlgorithmTabCtrl.InsertItem(22, "거리 측정");
	m_AlgorithmTabCtrl.InsertItem(23, "Gap 측정");
	m_AlgorithmTabCtrl.InsertItem(24, "Neck Epoxy 길이");
	m_AlgorithmTabCtrl.InsertItem(25, "Neck Epoxy 들뜸");
	m_AlgorithmTabCtrl.InsertItem(26, "Corner 검사");
	m_AlgorithmTabCtrl.InsertItem(27, "높이 측정");
	m_AlgorithmTabCtrl.InsertItem(28, "경계 검사");

	CRect DlgRect;

	m_pAlgorithmImageProcessDlg = new CAlgorithmImageProcessDlg;
	m_pAlgorithmImageProcessDlg->Create(IDD_TEACH_ALGORITHM_IMAGE_PROCESS_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmImageProcessDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmImageProcessDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmImageProcessDlg->ShowWindow(SW_SHOW);

	m_pAlgorithmROIAlignDlg = new CAlgorithmROIAlignDlg;
	m_pAlgorithmROIAlignDlg->Create(IDD_TEACH_ALGORITHM_ROI_ALIGN_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmROIAlignDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmROIAlignDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmROIAlignDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmROIAnisoAlignDlg = new CAlgorithmROIAnisoAlignDlg;
	m_pAlgorithmROIAnisoAlignDlg->Create(IDD_TEACH_ALGORITHM_ROI_ANISO_ALIGN_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmROIAnisoAlignDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmROIAnisoAlignDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmROIAnisoAlignDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmDontcareDlg = new CAlgorithmDontcareDlg;
	m_pAlgorithmDontcareDlg->Create(IDD_TEACH_ALGORITHM_DONTCARE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmDontcareDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmDontcareDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmDontcareDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmPartCheckDlg = new CAlgorithmPartCheckDlg;
	m_pAlgorithmPartCheckDlg->Create(IDD_TEACH_ALGORITHM_PART_CHECK_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmPartCheckDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmPartCheckDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmPartCheckDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmEdgeMeasureDlg = new CAlgorithmEdgeMeasureDlg;
	m_pAlgorithmEdgeMeasureDlg->Create(IDD_TEACH_ALGORITHM_EDGE_MEASURE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmEdgeMeasureDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmEdgeMeasureDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmEdgeMeasureDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmSurfaceDualDlg = new CAlgorithmSurfaceDualDlg;
	m_pAlgorithmSurfaceDualDlg->Create(IDD_TEACH_ALGORITHM_SURFACE_DUAL_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmSurfaceDualDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmSurfaceDualDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmSurfaceDualDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmShapeDlg = new CAlgorithmShapeDlg;
	m_pAlgorithmShapeDlg->Create(IDD_TEACH_ALGORITHM_SHAPE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmShapeDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmShapeDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmShapeDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmSurfaceDlg = new CAlgorithmSurfaceDlg;
	m_pAlgorithmSurfaceDlg->Create(IDD_TEACH_ALGORITHM_SURFACE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmSurfaceDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmSurfaceDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmSurfaceDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmImageCompareDlg = new CAlgorithmImageCompareDlg;
	m_pAlgorithmImageCompareDlg->Create(IDD_TEACH_ALGORITHM_IMAGE_COMPARE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmImageCompareDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmImageCompareDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmImageCompareDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmDefectConditionDlg = new CAlgorithmDefectConditionDlg;
	m_pAlgorithmDefectConditionDlg->Create(IDD_TEACH_ALGORITHM_DEFECT_CONDITION_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmDefectConditionDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmDefectConditionDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmDefectConditionDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmFittingDlg = new CAlgorithmFittingDlg;
	m_pAlgorithmFittingDlg->Create(IDD_TEACH_ALGORITHM_FITTING_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmFittingDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmFittingDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmFittingDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmOCRDlg = new CAlgorithmOCRDlg;
	m_pAlgorithmOCRDlg->Create(IDD_TEACH_ALGORITHM_OCR_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmOCRDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmOCRDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmOCRDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmBarcodeDlg = new CAlgorithmBarcodeDlg;
	m_pAlgorithmBarcodeDlg->Create(IDD_TEACH_ALGORITHM_BARCODE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmBarcodeDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmBarcodeDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmBarcodeDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmPrintQualityDlg = new CAlgorithmPrintQualityDlg;
	m_pAlgorithmPrintQualityDlg->Create(IDD_TEACH_ALGORITHM_PRINT_QUALITY_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmPrintQualityDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmPrintQualityDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmPrintQualityDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmConnectorPinDlg = new CAlgorithmConnectorPinDlg;
	m_pAlgorithmConnectorPinDlg->Create(IDD_TEACH_ALGORITHM_CONNECTOR_PIN_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmConnectorPinDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmConnectorPinDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmConnectorPinDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmDentDlg = new CAlgorithmDentDlg;
	m_pAlgorithmDentDlg->Create(IDD_TEACH_ALGORITHM_DENT_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmDentDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmDentDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmDentDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmConcenterMeasureDlg = new CAlgorithmConcenterMeasureDlg;
	m_pAlgorithmConcenterMeasureDlg->Create(IDD_TEACH_ALGORITHM_CONCENTER_MEASURE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmConcenterMeasureDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmConcenterMeasureDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmConcenterMeasureDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmHomerTiltDlg = new CAlgorithmTiltDlg;
	m_pAlgorithmHomerTiltDlg->Create(IDD_TEACH_ALGORITHM_TILT_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmHomerTiltDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmHomerTiltDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmHomerTiltDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmEpoxyVoidHoleDlg = new CAlgorithmEpoxyVoidHoleDlg;
	m_pAlgorithmEpoxyVoidHoleDlg->Create(IDD_TEACH_ALGORITHM_EPOXY_VOID_HOLE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmEpoxyVoidHoleDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmEpoxyVoidHoleDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmEpoxyVoidHoleDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmStiffenerEpoxyDlg = new CAlgorithmStiffenerEpoxyDlg;
	m_pAlgorithmStiffenerEpoxyDlg->Create(IDD_TEACH_ALGORITHM_STIFFENER_EPOXY_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmStiffenerEpoxyDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmStiffenerEpoxyDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmStiffenerEpoxyDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmDistanceMeasureDlg = new CAlgorithmDistanceMeasureDlg;
	m_pAlgorithmDistanceMeasureDlg->Create(IDD_TEACH_ALGORITHM_DISTANCE_MEASURE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmDistanceMeasureDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmDistanceMeasureDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmDistanceMeasureDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmGapMeasureDlg = new CAlgorithmGapMeasureDlg;
	m_pAlgorithmGapMeasureDlg->Create(IDD_TEACH_ALGORITHM_GAP_MEASURE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmGapMeasureDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmGapMeasureDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmGapMeasureDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmComponentDlg = new CAlgorithmComponentDlg;
	m_pAlgorithmComponentDlg->Create(IDD_TEACH_ALGORITHM_COMPONENT_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmComponentDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmComponentDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmComponentDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmNeckEpoxyMeasureDlg = new CAlgorithmNeckEpoxyMeasureDlg;
	m_pAlgorithmNeckEpoxyMeasureDlg->Create(IDD_TEACH_ALGORITHM_NECK_EPOXY_MEASURE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmNeckEpoxyMeasureDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmNeckEpoxyMeasureDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmNeckEpoxyMeasureDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmNeckEpoxyCrackDlg = new CAlgorithmNeckEpoxyCrackDlg;
	m_pAlgorithmNeckEpoxyCrackDlg->Create(IDD_TEACH_ALGORITHM_NECK_EPOXY_CRACK_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmNeckEpoxyCrackDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmNeckEpoxyCrackDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmNeckEpoxyCrackDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmCornerMeasureDlg = new CAlgorithmCornerMeasureDlg;
	m_pAlgorithmCornerMeasureDlg->Create(IDD_TEACH_ALGORITHM_CORNER_MEASURE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmCornerMeasureDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmCornerMeasureDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmCornerMeasureDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmHeightMeasureDlg = new CAlgorithmHeightMeasureDlg;
	m_pAlgorithmHeightMeasureDlg->Create(IDD_TEACH_ALGORITHM_HEIGHT_MEASURE_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmHeightMeasureDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmHeightMeasureDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmHeightMeasureDlg->ShowWindow(SW_HIDE);

	m_pAlgorithmBoundaryDlg = new CAlgorithmBoundaryDlg;
	m_pAlgorithmBoundaryDlg->Create(IDD_TEACH_ALGORITHM_BOUNDARY_DLG, &m_AlgorithmTabCtrl);
	m_pAlgorithmBoundaryDlg->GetWindowRect(&DlgRect);
	m_pAlgorithmBoundaryDlg->MoveWindow(5, 100, DlgRect.Width(), DlgRect.Height());
	m_pAlgorithmBoundaryDlg->ShowWindow(SW_HIDE);

	//m_bnTest.Set_Text(_T("테스트"));
	m_bnTest.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(134, 172, 227), 0, 0);

	//m_bnVisionTest.Set_Text(_T("비전 테스트"));
	m_bnVisionTest.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(201, 220, 203), 0, 0);

	//m_bnAiTest.Set_Text(_T("Ai 테스트"));
	m_bnAiTest.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(201, 220, 203), 0, 0);

	//m_bnSaveClose.Set_Text(_T("저장 후 닫기"));
	m_bnSaveClose.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(134, 172, 227), 0, 0);

	//m_bnClose.Set_Text(_T("닫기"));
	m_bnClose.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(240, 240, 240), 0, 0);

	//m_bnConvertUnit.Set_Text(_T("단위 변환기"));
	m_bnConvertUnit.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(242, 167, 168), 0, 0);

	//m_bnAiSetup.Set_Text(_T("AI Setup"));
	m_bnAiSetup.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(243, 226, 165), 0, 0);

	//m_bnAiApply.Set_Text(_T("AI Apply"));
	m_bnAiApply.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0, 0, 0), RGB(243, 226, 165), 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTeachingAlgorithmTabDlg::OnSelchangeAlgorithmTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	int iSelected = m_AlgorithmTabCtrl.GetCurSel();

	m_pAlgorithmImageProcessDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmROIAlignDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmROIAnisoAlignDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmDontcareDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmPartCheckDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmEdgeMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmSurfaceDualDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmShapeDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmSurfaceDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmImageCompareDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmDefectConditionDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmFittingDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmOCRDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmBarcodeDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmPrintQualityDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmConnectorPinDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmDentDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmConcenterMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmComponentDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmHomerTiltDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmEpoxyVoidHoleDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmStiffenerEpoxyDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmDistanceMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmGapMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmNeckEpoxyMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmNeckEpoxyCrackDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmCornerMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmHeightMeasureDlg->ShowWindow(SW_HIDE);
	m_pAlgorithmBoundaryDlg->ShowWindow(SW_HIDE);

	switch (iSelected)
	{
	case 0:
		m_pAlgorithmImageProcessDlg->ShowWindow(SW_SHOW);
		break;

	case 1:
		m_pAlgorithmROIAlignDlg->ShowWindow(SW_SHOW);
		break;

	case 2:
		m_pAlgorithmROIAnisoAlignDlg->ShowWindow(SW_SHOW);
		break;

	case 3:
		m_pAlgorithmDontcareDlg->ShowWindow(SW_SHOW);
		break;

	case 4:
		m_pAlgorithmPartCheckDlg->ShowWindow(SW_SHOW);
		break;

	case 5:
		m_pAlgorithmEdgeMeasureDlg->ShowWindow(SW_SHOW);
		break;

	case 6:
		m_pAlgorithmSurfaceDualDlg->ShowWindow(SW_SHOW);
		break;

	case 7:
		m_pAlgorithmShapeDlg->ShowWindow(SW_SHOW);
		break;

	case 8:
		m_pAlgorithmSurfaceDlg->ShowWindow(SW_SHOW);
		break;

	case 9:
		m_pAlgorithmImageCompareDlg->ShowWindow(SW_SHOW);
		m_pAlgorithmImageCompareDlg->UpdateVarModelImage();
		break;

	case 10:
		m_pAlgorithmDefectConditionDlg->ShowWindow(SW_SHOW);
		break;

	case 11:
		m_pAlgorithmFittingDlg->ShowWindow(SW_SHOW);
		break;

	case 12:
		m_pAlgorithmOCRDlg->ShowWindow(SW_SHOW);
		break;

	case 13:
		m_pAlgorithmBarcodeDlg->ShowWindow(SW_SHOW);
		break;

	case 14:
		m_pAlgorithmPrintQualityDlg->ShowWindow(SW_SHOW);
		break;

	case 15:
		m_pAlgorithmConnectorPinDlg->ShowWindow(SW_SHOW);
		break;

	case 16:
		m_pAlgorithmDentDlg->ShowWindow(SW_SHOW);
		break;

	case 17:
		m_pAlgorithmConcenterMeasureDlg->ShowWindow(SW_SHOW);
		break;

	case 18:
		m_pAlgorithmComponentDlg->CheckParamStatus();
		m_pAlgorithmComponentDlg->ShowWindow(SW_SHOW);
		break;

	case 19:
		m_pAlgorithmHomerTiltDlg->ShowWindow(SW_SHOW);
		break;

	case 20:
		m_pAlgorithmEpoxyVoidHoleDlg->ShowWindow(SW_SHOW);
		break;

	case 21:
		m_pAlgorithmStiffenerEpoxyDlg->ShowWindow(SW_SHOW);
		break;

	case 22:
		m_pAlgorithmDistanceMeasureDlg->ShowWindow(SW_SHOW);
		break;

	case 23:
		m_pAlgorithmGapMeasureDlg->ShowWindow(SW_SHOW);
		break;

	case 24:
		m_pAlgorithmNeckEpoxyMeasureDlg->ShowWindow(SW_SHOW);
		break;

	case 25:
		m_pAlgorithmNeckEpoxyCrackDlg->ShowWindow(SW_SHOW);
		break;

	case 26:
		m_pAlgorithmCornerMeasureDlg->ShowWindow(SW_SHOW);
		break;

	case 27:
		m_pAlgorithmHeightMeasureDlg->ShowWindow(SW_SHOW);
		break;

	case 28:
		m_pAlgorithmBoundaryDlg->ShowWindow(SW_SHOW);
		break;

	default:
		break;
	}

	*pResult = 0;
}

void CTeachingAlgorithmTabDlg::UpdateAlgorithmTab(int iSelectedTabIndex)
{
	NMHDR nmhdr;

	switch (iSelectedTabIndex)
	{
	case 8:
		m_AlgorithmTabCtrl.SetCurSel(iSelectedTabIndex);
		nmhdr.code = TCN_SELCHANGE;
		nmhdr.idFrom = IDD_TEACH_ALGORITHM_SURFACE_DLG;
		nmhdr.hwndFrom = m_AlgorithmTabCtrl.m_hWnd;
		SendMessage(WM_NOTIFY, IDD_TEACH_ALGORITHM_SURFACE_DLG, (LPARAM)&nmhdr);
		break;

	case 9:
		m_AlgorithmTabCtrl.SetCurSel(iSelectedTabIndex);
		nmhdr.code = TCN_SELCHANGE;
		nmhdr.idFrom = IDD_TEACH_ALGORITHM_IMAGE_COMPARE_DLG;
		nmhdr.hwndFrom = m_AlgorithmTabCtrl.m_hWnd;
		SendMessage(WM_NOTIFY, IDD_TEACH_ALGORITHM_IMAGE_COMPARE_DLG, (LPARAM)&nmhdr);
		break;

	case 10:
		m_AlgorithmTabCtrl.SetCurSel(iSelectedTabIndex);
		nmhdr.code = TCN_SELCHANGE;
		nmhdr.idFrom = IDD_TEACH_ALGORITHM_DEFECT_CONDITION_DLG;
		nmhdr.hwndFrom = m_AlgorithmTabCtrl.m_hWnd;
		SendMessage(WM_NOTIFY, IDD_TEACH_ALGORITHM_DEFECT_CONDITION_DLG, (LPARAM)&nmhdr);
		break;

	default:
		break;
	}
}

BOOL CTeachingAlgorithmTabDlg::GetCheckedShapeImage()
{
	UpdateData();

	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information

	m_pAlgorithmShapeDlg->GetParam(&AlgorithmParam);

	if (AlgorithmParam.m_bUseShape)
		return TRUE;
	else
		return FALSE;
}

CAlgorithmParam CTeachingAlgorithmTabDlg::GetCurTabAlgorithmParam()
{
	UpdateData();

	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information

	m_pAlgorithmImageProcessDlg->GetParam(&AlgorithmParam);	// Image Processing Tab Data
	m_pAlgorithmSurfaceDlg->GetParam(&AlgorithmParam);	// Surface Inspection Tab Data
	m_pAlgorithmDefectConditionDlg->GetParam(&AlgorithmParam);	// 2nd Surface Inspection Tab Data
	m_pAlgorithmFittingDlg->GetParam(&AlgorithmParam);	// Fitting Inspection Tab Data
	m_pAlgorithmOCRDlg->GetParam(&AlgorithmParam);	// OCR Tab Data
	m_pAlgorithmBarcodeDlg->GetParam(&AlgorithmParam);	// Barcode Tab Data
	m_pAlgorithmConnectorPinDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDentDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmImageCompareDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmPartCheckDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmROIAlignDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmConcenterMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmShapeDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDontcareDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmEdgeMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmSurfaceDualDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmHomerTiltDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmEpoxyVoidHoleDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmPrintQualityDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmROIAnisoAlignDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmStiffenerEpoxyDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDistanceMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmGapMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmComponentDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmNeckEpoxyMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmNeckEpoxyCrackDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmCornerMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmHeightMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmBoundaryDlg->GetParam(&AlgorithmParam);

	return AlgorithmParam;
}

void CTeachingAlgorithmTabDlg::OnBnClickedButtonTest()
{
	// 전체 검사 테스트
	InspectionTest(true, true);
}

void CTeachingAlgorithmTabDlg::OnBnClickedMfcbuttonVisionTest()
{
	InspectionTest(true, false);
}

void CTeachingAlgorithmTabDlg::OnBnClickedMfcbuttonAiTest()
{
	InspectionTest(false, true);
}



void CTeachingAlgorithmTabDlg::OnBnClickedButtonSaveAndClose()
{
	for (int i = 0; i < CHANNEL_NUM; i++)
		CopyImage(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][m_iSelectedImageType]));

	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();

	GetParam();

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	Hide();
}

void CTeachingAlgorithmTabDlg::OnBnClickedButtonClose()
{
	for (int i = 0; i < CHANNEL_NUM; i++)
		CopyImage(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i][m_iSelectedImageType]));

	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	Hide();
}

void CTeachingAlgorithmTabDlg::SetSelectedInspection(int iImageType, GTRegion* pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;

	m_pAlgorithmImageProcessDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmSurfaceDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmDefectConditionDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmOCRDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmROIAlignDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmROIAnisoAlignDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmImageCompareDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmPartCheckDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmShapeDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmComponentDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmConcenterMeasureDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
	m_pAlgorithmBoundaryDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iSelectedInspectionTabIndex);
}

void CTeachingAlgorithmTabDlg::SetParam()
{
	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];

	m_pAlgorithmImageProcessDlg->SetParam(AlgorithmParam);
	m_pAlgorithmSurfaceDlg->SetParam(AlgorithmParam);
	m_pAlgorithmDefectConditionDlg->SetParam(AlgorithmParam);
	m_pAlgorithmFittingDlg->SetParam(AlgorithmParam);
	m_pAlgorithmOCRDlg->SetParam(AlgorithmParam);
	m_pAlgorithmBarcodeDlg->SetParam(AlgorithmParam);
	m_pAlgorithmConnectorPinDlg->SetParam(AlgorithmParam);
	m_pAlgorithmDentDlg->SetParam(AlgorithmParam);
	m_pAlgorithmImageCompareDlg->SetParam(AlgorithmParam);
	m_pAlgorithmPartCheckDlg->SetParam(AlgorithmParam);
	m_pAlgorithmROIAlignDlg->SetParam(AlgorithmParam);
	m_pAlgorithmConcenterMeasureDlg->SetParam(AlgorithmParam);
	m_pAlgorithmShapeDlg->SetParam(AlgorithmParam);
	m_pAlgorithmDontcareDlg->SetParam(AlgorithmParam);
	m_pAlgorithmEdgeMeasureDlg->SetParam(AlgorithmParam);
	m_pAlgorithmSurfaceDualDlg->SetParam(AlgorithmParam);
	m_pAlgorithmHomerTiltDlg->SetParam(AlgorithmParam);
	m_pAlgorithmEpoxyVoidHoleDlg->SetParam(AlgorithmParam);
	m_pAlgorithmPrintQualityDlg->SetParam(AlgorithmParam);
	m_pAlgorithmROIAnisoAlignDlg->SetParam(AlgorithmParam);
	m_pAlgorithmStiffenerEpoxyDlg->SetParam(AlgorithmParam);
	m_pAlgorithmDistanceMeasureDlg->SetParam(AlgorithmParam);
	m_pAlgorithmGapMeasureDlg->SetParam(AlgorithmParam);
	m_pAlgorithmComponentDlg->SetParam(AlgorithmParam);
	m_pAlgorithmNeckEpoxyMeasureDlg->SetParam(AlgorithmParam);
	m_pAlgorithmNeckEpoxyCrackDlg->SetParam(AlgorithmParam);
	m_pAlgorithmCornerMeasureDlg->SetParam(AlgorithmParam);
	m_pAlgorithmHeightMeasureDlg->SetParam(AlgorithmParam);
	m_pAlgorithmBoundaryDlg->SetParam(AlgorithmParam);
}

void CTeachingAlgorithmTabDlg::GetParam()
{
	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information

	m_pAlgorithmImageProcessDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmSurfaceDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDefectConditionDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmFittingDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmOCRDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmBarcodeDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmConnectorPinDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDentDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmImageCompareDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmPartCheckDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmROIAlignDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmConcenterMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmShapeDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDontcareDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmEdgeMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmSurfaceDualDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmHomerTiltDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmEpoxyVoidHoleDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmPrintQualityDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmROIAnisoAlignDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmStiffenerEpoxyDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDistanceMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmGapMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmComponentDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmNeckEpoxyMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmNeckEpoxyCrackDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmCornerMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmHeightMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmBoundaryDlg->GetParam(&AlgorithmParam);

	m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex] = AlgorithmParam;

	THEAPP.m_pTabControlDlg->UpdateList(m_iSelectedImageType, m_iSelectedInspectionTabIndex);
}

#include "UnitConvertDlg.h"

void CTeachingAlgorithmTabDlg::OnBnClickedButtonConvertUnit()
{
	CUnitConvertDlg dlg;

	dlg.DoModal();
}

int CTeachingAlgorithmTabDlg::GetPeak()
{
	BOOL bDebugSave = FALSE;

	UpdateData();

	double tStartTime = GetTickCount();

	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information

	m_pAlgorithmImageProcessDlg->GetParam(&AlgorithmParam);	// Image Processing Tab Data
	m_pAlgorithmSurfaceDlg->GetParam(&AlgorithmParam);	// Surface Inspection Tab Data
	m_pAlgorithmDefectConditionDlg->GetParam(&AlgorithmParam);	// 2nd Surface Inspection Tab Data
	m_pAlgorithmFittingDlg->GetParam(&AlgorithmParam);	// Fitting Inspection Tab Data
	m_pAlgorithmOCRDlg->GetParam(&AlgorithmParam);	// OCR Tab Data
	m_pAlgorithmBarcodeDlg->GetParam(&AlgorithmParam);	// Barcode Tab Data
	m_pAlgorithmConnectorPinDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDentDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmImageCompareDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmPartCheckDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmROIAlignDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmConcenterMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmShapeDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDontcareDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmEdgeMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmSurfaceDualDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmHomerTiltDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmEpoxyVoidHoleDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmPrintQualityDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmROIAnisoAlignDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmStiffenerEpoxyDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDistanceMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmGapMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmComponentDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmNeckEpoxyMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmNeckEpoxyCrackDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmCornerMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmHeightMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmBoundaryDlg->GetParam(&AlgorithmParam);

	THEAPP.m_pAlgorithm->InitGapResult();
	THEAPP.m_pAlgorithm->InitCornerResult();

	THEAPP.m_pTabControlDlg->AlgorithmPreProcessing();

	HObject HROIHRegion;
	HROIHRegion = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

	if (bDebugSave)
	{
		WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn_BeforeLocalAlign.reg");
	}

	if (AlgorithmParam.m_bUseImageProcessLocalAlign)
		THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIHRegion, TEACHING_THREAD_INDEX);

	if (bDebugSave)
	{
		WriteImage(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], "bmp", 0, "c:\\DualTest\\InspectImage");
		WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
	}

	if (AlgorithmParam.m_bUseImageProcessGenerate)
		THEAPP.m_pAlgorithm->ApplyGenerateResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIHRegion, TEACHING_THREAD_INDEX);

	if (bDebugSave)
	{
		WriteImage(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], "bmp", 0, "c:\\DualTest\\InspectImage");
		WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
	}

	if (AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
		THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIHRegion, TEACHING_THREAD_INDEX);

	if (bDebugSave)
	{
		WriteImage(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], "bmp", 0, "c:\\DualTest\\InspectImage");
		WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
	}

	double dAlignMatchingScore;

	CMeasureData MeasureData;
	HObject HDefectAllRgn, HResultXLD;
	HObject HAlignRgn;
	HObject HPreProcessImage;

	int iImgCnt;
	BOOL bShapeImageFail;
	int iShapeRawImageIdx[NO_USED_SHAPE_RAWIMAGE];
	int iShapeImageCnt = 0;

	int iPeakValue = -1;

	if (AlgorithmParam.m_bUseROIAlign && AlgorithmParam.m_bROIAlignMultiModuleInspect == FALSE)
	{
		BOOL bROIAlignShiftResult = FALSE;

		if (AlgorithmParam.m_bUseIsoColorImage)
			HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[CHANNEL_TYPE_COLOR], HROIHRegion, AlgorithmParam, &(m_pSelectedROI->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);
		else
			HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], HROIHRegion, AlgorithmParam, &(m_pSelectedROI->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);

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

	// 비등방 CHL
	if (AlgorithmParam.m_bUseROIAnisoAlign && AlgorithmParam.m_bROIAnisoAlignMultiModuleInspect == FALSE)
	{
		BOOL bROIAlignShiftResult = FALSE;

		if (AlgorithmParam.m_bUseAnisoColorImage)
			HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[CHANNEL_TYPE_COLOR], HROIHRegion, AlgorithmParam, &(m_pSelectedROI->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);
		else
			HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], HROIHRegion, AlgorithmParam, &(m_pSelectedROI->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);

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
		CopyImage(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], &THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]);

		THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], HROIHRegion, AlgorithmParam, &HPreProcessImage);

		iPeakValue = THEAPP.m_pAlgorithm->FindHistogramPeak(HPreProcessImage, m_pAlgorithmSurfaceDlg->m_HOrgScanImage[AlgorithmParam.m_iProcessChTypeDC], HROIHRegion, AlgorithmParam);
	}
	else
	{
		iPeakValue = THEAPP.m_pAlgorithm->FindHistogramPeak(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], m_pAlgorithmSurfaceDlg->m_HOrgScanImage[AlgorithmParam.m_iProcessChTypeDC], HROIHRegion, AlgorithmParam);
	}

	return iPeakValue;
}

void CTeachingAlgorithmTabDlg::OnBnClickedButtonAisetup()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strLog; // 로그 변수는 함수 시작 시 선언

	// --- 중복 실행 방지: 이미 AI Setup Wizard가 실행 중인지 확인 ---
	if (ai_setup_process_handle_ != nullptr)
	{
		// 프로세스가 여전히 실행 중인지 확인
		DWORD dwExitCode = 0;
		if (GetExitCodeProcess(ai_setup_process_handle_, &dwExitCode))
		{
			if (dwExitCode == STILL_ACTIVE)
			{
				// 아직 실행 중
				CString strMessage;
				if (THEAPP.m_iPGMLanguageSelect == 0)
					strMessage.Format(_T("AI Setup Wizard가 이미 실행 중입니다.\n먼저 실행 중인 프로그램을 종료해 주세요."));
				else
					strMessage.Format(_T("AI Setup Wizard is already running.\nPlease close the running program first."));
				AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONWARNING);

				strLog.Format(_T("AI Setup Wizard launch blocked - already running"));
				THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
				return; // 중복 실행 방지
			}
		}

		// 프로세스가 종료되었으면 핸들 정리
		CloseHandle(ai_setup_process_handle_);
		ai_setup_process_handle_ = nullptr;
	}

	CAlgorithmParam AlgorithmParam;
	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information
#ifdef SYAI
	// --- AI 모델 Key 생성
	syai_service::NameBuildOptions build_options;
	build_options.consider_vision_pos = false;
	build_options.consider_inspection_type = true;
	build_options.consider_defect_type = AlgorithmParam.m_bCheckAIUseConsiderDefectCode ? true : false;
	build_options.consider_image_index = AlgorithmParam.m_bCheckAIUseConsiderImageNo ? true : false;
	build_options.ignore_case = false;
	build_options.allow_input_duplicates = false;
	build_options.distinct = true;

	std::string camera_type = (LPCSTR)THEAPP.m_ModelDefineInfo.m_strVisionName_Short[THEAPP.m_iCurVisionCamType];
	std::string inspection_type = std::string(g_strInspectionTypeName_Short[m_pSelectedROI->miInspectionType - 1]);
	std::string image_index_str = std::to_string(THEAPP.m_pTabControlDlg->m_iCurrentTab);
	std::string defect_name = std::string(CT2A(THEAPP.m_strDefectName[AlgorithmParam.m_iDefectType]));
	std::string defect_type = std::string(CT2A(THEAPP.m_strDefectCode[AlgorithmParam.m_iDefectType]));
	std::string model_key = syai_service::ServiceDirector::build_name(camera_type, "", inspection_type, image_index_str, defect_type, build_options);
#else
	std::string model_key = "SYAI_MODEL_KEY_UNAVAILABLE";
#endif // SYAI
	std::optional<SHELLEXECUTEINFOA> sei = THEAPP.StartAISetupSW("SpecificStepModelEdit", 1, model_key);

	if (sei == std::nullopt)
	{
		// 실행 실패
		CString strMessage;
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessage.Format(_T("AI Setup Wizard 실행에 실패했습니다.\n프로그램 파일이 존재하는지 확인해 주세요."));
		else
			strMessage.Format(_T("Failed to launch AI Setup Wizard.\nPlease check if the program file exists."));
		AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONERROR);
		strLog.Format(_T("AI Setup Wizard launch FAILED"));
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		return;
	}

	// 프로세스 핸들 저장
	ai_setup_process_handle_ = sei->hProcess;

	strLog.Format(_T("AI Setup Wizard launched successfully"));
	THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));

	//// 프로세스 종료를 기다리는 쓰레드 시작
	AfxBeginThread(AiSetupWaitThreadProc, this);
}

void CTeachingAlgorithmTabDlg::OnBnClickedButtonAisetupApply()
{
	// 변경된 AI 설정 적용
	CString strLog;
	strLog = _T("Reinitializing AI inspection with updated configuration...");
	THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));

	// --- 경로 준비 (std::filesystem 사용) ---
	char path_buf[MAX_PATH];
	GetModuleFileNameA(NULL, path_buf, MAX_PATH);
	std::filesystem::path executable_path(path_buf);
	std::filesystem::path folder_path = executable_path.parent_path() / "Data";
	std::filesystem::path config_json_path = folder_path / "ai_inspection_config.json";
	// 구성 파일이 존재하는지 확인
	if (!PathFileExistsA(config_json_path.string().c_str()))
	{
		strLog.Format(_T("AI inspection configuration file not found: %s"), config_json_path.string().c_str());
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

		CString strMessage;
		strMessage.Format(_T("AI inspection configuration file not found: %s"));
		AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONWARNING);
	}
#ifdef SYAI
	if (THEAPP.m_syai_service == nullptr)
	{
		strLog.Format(_T("SYAI service is not initialized."));
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		CString strMessage;
		strMessage.Format(_T("SYAI service is not initialized."));
		AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONWARNING);
	}
	if (!THEAPP.m_syai_service->re_init_config(config_json_path.string()) != 0)
	{
		strLog.Format(_T("Failed to reinitialize AI inspection with configuration file: %s"), config_json_path.string().c_str());
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		CString strMessage;
		strMessage.Format(_T("Failed to reinitialize AI inspection with configuration file: %s"), config_json_path.string().c_str());
		AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONWARNING);
		return;
	}
	strLog = _T("AI inspection reinitialized successfully with updated configuration.");
	THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));
	CString strMessage;
	strMessage.Format(_T("AI 설정이 성공적으로 적용되었습니다."));
#else
	CString strMessage;
	strMessage.Format(_T("SYAI service is not available in this build."));
	THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
#endif // SYAI
	AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONINFORMATION);
}

UINT __cdecl CTeachingAlgorithmTabDlg::AiSetupWaitThreadProc(LPVOID pParam)
{
	CTeachingAlgorithmTabDlg* pView = reinterpret_cast<CTeachingAlgorithmTabDlg*>(pParam);
	if (!pView)
		return 0;

	HANDLE hProcess = pView->ai_setup_process_handle_;
	if (hProcess == nullptr)
		return 0;

	// 프로세스 종료까지 대기
	WaitForSingleObject(hProcess, INFINITE);

	// View에 "끝났다"는 메시지 전달 (UI 스레드에서 처리)
	// 핸들 정리는 OnAiSetupExited에서 수행
	::PostMessage(pView->GetSafeHwnd(), WM_AISETUP_EXITED, 0, 0);

	return 0;
}

LRESULT CTeachingAlgorithmTabDlg::OnAiSetupExited(WPARAM wParam, LPARAM lParam)
{
	CString strLog;

	// 프로세스 핸들 정리
	if (ai_setup_process_handle_ != nullptr)
	{
		CloseHandle(ai_setup_process_handle_);
		ai_setup_process_handle_ = nullptr;
	}
	return 0;
}

bool CTeachingAlgorithmTabDlg::ReinitializeAiInspection()
{
	CString strLog;
	strLog = _T("Reinitializing AI inspection with updated configuration...");
	THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));

	// --- 경로 준비 (std::filesystem 사용) ---
	char path_buf[MAX_PATH];
	GetModuleFileNameA(NULL, path_buf, MAX_PATH);
	std::filesystem::path executable_path(path_buf);
	std::filesystem::path folder_path = executable_path.parent_path() / "Data";
	std::filesystem::path config_json_path = folder_path / "ai_inspection_config.json";
	// 구성 파일이 존재하는지 확인
	if (!PathFileExistsA(config_json_path.string().c_str()))
	{
		strLog.Format(_T("AI inspection configuration file not found: %s"), config_json_path.string().c_str());
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		return false;
	}
	// 구성 파일이 존재하면, 재초기화 로직 수행
#ifdef SYAI
	return THEAPP.m_syai_service->re_init_config(config_json_path.string());
#else
	strLog.Format(_T("SYAI service is not initialized."));
	THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
	return false;
#endif // SYAI
}

bool CTeachingAlgorithmTabDlg::InspectionTest(bool enableVision, bool enableAi)
{
	BOOL bDebugSave = FALSE;

	int iVisionNo = THEAPP.m_iCurTeachVision;
	THEAPP.m_pAlgorithm->m_iPcVisionNo = iVisionNo;
	THEAPP.m_pAlgorithm->m_iVisionCamType = iVisionNo;

	UpdateData();

	HObject HMaskRgn;
	GenEmptyObj(&HMaskRgn);

	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspectionTabIndex];		// To keep the Inspection tab information

	m_pAlgorithmImageProcessDlg->GetParam(&AlgorithmParam);	// Image Processing Tab Data
	m_pAlgorithmSurfaceDlg->GetParam(&AlgorithmParam);	// Surface Inspection Tab Data
	m_pAlgorithmDefectConditionDlg->GetParam(&AlgorithmParam);	// 2nd Surface Inspection Tab Data
	m_pAlgorithmFittingDlg->GetParam(&AlgorithmParam);	// Fitting Inspection Tab Data
	m_pAlgorithmOCRDlg->GetParam(&AlgorithmParam);	// OCR Tab Data
	m_pAlgorithmBarcodeDlg->GetParam(&AlgorithmParam);	// Barcode Tab Data
	m_pAlgorithmConnectorPinDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDentDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmImageCompareDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmPartCheckDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmROIAlignDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmConcenterMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmShapeDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDontcareDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmEdgeMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmSurfaceDualDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmHomerTiltDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmEpoxyVoidHoleDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmPrintQualityDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmROIAnisoAlignDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmStiffenerEpoxyDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmDistanceMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmGapMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmComponentDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmNeckEpoxyMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmNeckEpoxyCrackDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmCornerMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmHeightMeasureDlg->GetParam(&AlgorithmParam);
	m_pAlgorithmBoundaryDlg->GetParam(&AlgorithmParam);

	THEAPP.m_pAlgorithm->InitGapResult();
	THEAPP.m_pAlgorithm->InitCornerResult();

	THEAPP.m_pTabControlDlg->AlgorithmPreProcessing();

	HObject HROIHRegion;
	HROIHRegion = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

	if (bDebugSave)
	{
		WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn_BeforeLocalAlign.reg");
	}

#pragma region Image Preprocessing 
	// * ROI 크롭 이미지 사이즈를 고정으로 갖기 위해 얼라인 전의 ROI 크기를 측정
	HTuple length1, length2, trash;
	SmallestRectangle2(HROIHRegion, &trash, &trash, &trash, &length1, &length2);
#pragma endregion Image Preprocessing 

	if (AlgorithmParam.m_bUseImageProcessLocalAlign)
		THEAPP.m_pAlgorithm->ApplyLocalAlignResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIHRegion, TEACHING_THREAD_INDEX);

	if (bDebugSave)
	{
		WriteImage(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], "bmp", 0, "c:\\DualTest\\InspectImage");
		WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
	}

	if (AlgorithmParam.m_bUseImageProcessGenerate)
		THEAPP.m_pAlgorithm->ApplyGenerateResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIHRegion, TEACHING_THREAD_INDEX);

	if (bDebugSave)
	{
		WriteImage(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], "bmp", 0, "c:\\DualTest\\InspectImage");
		WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
	}

	if (AlgorithmParam.m_bUseImageProcessDontCare || AlgorithmParam.m_bUseImageProcessDontCare2)
		THEAPP.m_pAlgorithm->ApplyDontCareResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HROIHRegion, TEACHING_THREAD_INDEX);

	if (bDebugSave)
	{
		WriteImage(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], "bmp", 0, "c:\\DualTest\\InspectImage");
		WriteRegion(HROIHRegion, "c:\\DualTest\\HROIRgn.reg");
	}

	if (AlgorithmParam.m_bUseImageProcessMask || AlgorithmParam.m_bUseImageProcessMask2)
		THEAPP.m_pAlgorithm->ApplyMaskResult(TEACHING_MZ_NO, THEAPP.m_iCurTeachVision, AlgorithmParam, &HMaskRgn, TEACHING_THREAD_INDEX);
	else
		GenEmptyObj(&HMaskRgn);

	double dAlignMatchingScore;

	double tStartTime = GetTickCount();

	CMeasureData MeasureData;
	HObject HDefectAllRgn, HResultXLD;
	HObject HAlignRgn;
	HObject HPreProcessImage;

	int iImgCnt;
	BOOL bShapeImageFail;
	int iShapeRawImageIdx[NO_USED_SHAPE_RAWIMAGE];
	int iShapeImageCnt = 0;

	GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
	GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAIDefectRgn));
	GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusRgn));
	GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCenterRgn));

	if (AlgorithmParam.m_bUseOCR)
	{
		THEAPP.m_pAlgorithm->m_bManualBarcodeForOcrInspect = TRUE;
	}

	if (AlgorithmParam.m_bUseROIAlign && AlgorithmParam.m_bROIAlignMultiModuleInspect == FALSE)
	{
		BOOL bROIAlignShiftResult = FALSE;

		if (AlgorithmParam.m_bUseIsoColorImage)
			HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[CHANNEL_TYPE_COLOR], HROIHRegion, AlgorithmParam, &(m_pSelectedROI->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);
		else
			HAlignRgn = THEAPP.m_pAlgorithm->ROIAlignAlgorithm(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], HROIHRegion, AlgorithmParam, &(m_pSelectedROI->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);

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

	// 비등방 CHL
	if (AlgorithmParam.m_bUseROIAnisoAlign && AlgorithmParam.m_bROIAnisoAlignMultiModuleInspect == FALSE)
	{
		BOOL bROIAlignShiftResult = FALSE;

		if (AlgorithmParam.m_bUseAnisoColorImage)
			HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[CHANNEL_TYPE_COLOR], HROIHRegion, AlgorithmParam, &(m_pSelectedROI->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);
		else
			HAlignRgn = THEAPP.m_pAlgorithm->ROIAnisoAlignAlgorithm(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], HROIHRegion, AlgorithmParam, &(m_pSelectedROI->m_HROIAlignModelID), &bROIAlignShiftResult, &dAlignMatchingScore);

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

	THEAPP.bImageCompareTeachingDetect = TRUE;
	if (AlgorithmParam.m_bUseImageProcess || AlgorithmParam.m_bUseImageProcessFilter)
	{
		CopyImage(m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], &THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]);

		THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], HROIHRegion, AlgorithmParam, &HPreProcessImage);

		HDefectAllRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, m_pSelectedROI, m_iSelectedInspectionTabIndex, HPreProcessImage, HROIHRegion, AlgorithmParam, &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], m_pAlgorithmSurfaceDlg->m_HOrgScanImage[AlgorithmParam.m_iProcessChTypeDC], &HMaskRgn);
	}
	else
	{
		HDefectAllRgn = THEAPP.m_pAlgorithm->InspectAVI(-1, 0, m_pSelectedROI, m_iSelectedInspectionTabIndex, m_pAlgorithmSurfaceDlg->m_HOrgScanImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], HROIHRegion, AlgorithmParam, &HResultXLD, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()], m_pAlgorithmSurfaceDlg->m_HOrgScanImage[AlgorithmParam.m_iProcessChTypeDC], &HMaskRgn);
	}
	THEAPP.bImageCompareTeachingDetect = FALSE;

	if (THEAPP.m_pGFunction->ValidHRegion(HDefectAllRgn) == TRUE)
	{
		ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, HDefectAllRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
		Union1(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));
	}

	if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
		THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD = HResultXLD;
	else
		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

	if (AlgorithmParam.m_bUseBarcode)
	{
		if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_StructModuleBarcodeResult[0][0][0].m_HBarcodeDetectRgn) == TRUE)
			THEAPP.m_pInspectAdminViewDlg->m_HBarcodeDetectRgn = THEAPP.m_StructModuleBarcodeResult[0][0][0].m_HBarcodeDetectRgn;
		else
			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HBarcodeDetectRgn));
	}

	if (AlgorithmParam.m_bUseOCR)//2017.04.10 hbk
	{
		if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pAlgorithm->m_HOCRDetectRgn) == TRUE)
			THEAPP.m_pInspectAdminViewDlg->m_HOCRDetectRgn = THEAPP.m_pAlgorithm->m_HOCRDetectRgn;
		else
			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HOCRDetectRgn));
	}

#ifdef SYAI
	if (enableAi && THEAPP.m_syai_service != nullptr &&
		m_pSelectedROI->miInspectionType >= INSPECTION_TYPE_FIRST && m_pSelectedROI->miInspectionType <= INSPECTION_TYPE_LAST)
	{
#pragma region Image Preprocessing
		// THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]
		HObject image = m_pAlgorithmSurfaceDlg->m_HOrgScanImage[CHANNEL_TYPE_COLOR];
		HObject ho_Cropped, ho_Cropped_v2;
		cv::Mat cv_roi_img, cv_roi_img_v2;
		HalconCpp::HTuple raw_image_width, raw_image_height;
		HTuple crop_row1, crop_col1, crop_row2, crop_col2;
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

		// Burr는 패딩을 크게 줌 (ROI보다 실제 검사 영역이 큼)
		int iPadding = 128; // 50
		int dilation = 5; // 50
		if (THEAPP.m_strDefectName[AlgorithmParam.m_iDefectType] == "Burr") {
			iPadding = 128; // 100
			dilation = 10; // 50
		}

		// ROI 이미지 전처리 수행 여부 확인
		bool all_roi_inspection = THEAPP.Struct_PreferenceStruct.m_bAllImageAiInspection;
		bool enable_roi_processing = false;
		if (all_roi_inspection ||
			THEAPP.Struct_PreferenceStruct.m_bSaveROIImage ||
			AlgorithmParam.m_bUseFilter1 ||
			AlgorithmParam.m_bUseFilter2 ||
			AlgorithmParam.m_bUseFilter3) {
			enable_roi_processing = true;
		}

		// ROI 이미지 전처리 비활성화 시 결함 영역 저장 후 이후 단계 패스
		if (!enable_roi_processing) {
			THEAPP.m_syai_logger->debug("[TEST Inspection] ROI Image Preprocessing skipped. Skip AI inspection.");
		}
		// ROI 이미지 전처리 실패시 결함 영역 저장 후 이후 단계 패스
		if (!AIService::ImageUtile::RoiImagePreprocessing(image, HROIHRegion,
														  length1, length2,
														  ho_Cropped, cv_roi_img,
														  ho_Cropped_v2, cv_roi_img_v2,
														  raw_image_width, raw_image_height,
														  crop_row1, crop_col1, crop_row2, crop_col2,
														  iPadding, dilation, THEAPP.m_syai_logger)) {
			THEAPP.m_syai_logger->error("[TEST Inspection] ROI Image Preprocessing failed. Skip AI inspection.");
			cv_roi_img = cv::Mat();
			cv_roi_img_v2 = cv::Mat();
		}
		x1 = static_cast<int>(crop_col1.D());
		y1 = static_cast<int>(crop_row1.D());
		x2 = static_cast<int>(crop_col2.D());
		y2 = static_cast<int>(crop_row2.D());

#pragma endregion Image Preprocessing
		int indpection_type_idx = m_pSelectedROI->miInspectionType - 1;
		HObject inspection_req_rgn;
#pragma region AI Inspection
		bool can_submit = true;
		cv::Mat ai_inspection_image;
		if (!cv_roi_img.empty()) {
			THEAPP.m_syai_logger->debug("[TEST Inspection] Using first ROI image for AI inspection.");
			ai_inspection_image = cv_roi_img;
		}
		else if (!cv_roi_img_v2.empty()) {
			THEAPP.m_syai_logger->debug("[TEST Inspection] Using second ROI image for AI inspection.");
			ai_inspection_image = cv_roi_img_v2;
		}
		else if (enable_roi_processing) {
			THEAPP.m_syai_logger->debug("[TEST Inspection] Both ROI images are empty. Creating blank image for AI inspection.");
			ai_inspection_image = cv::Mat::zeros(static_cast<int>(length2.D()), static_cast<int>(length1.D()), CV_8UC3);
		}
		else {
			THEAPP.m_syai_logger->debug("[TEST Inspection] ROI Image Preprocessing disabled and no valid ROI image. Skip AI inspection.");
			can_submit = false;
		}

		// 마스크 정보 생성
		cv::Size orignal_image_size = cv::Size(raw_image_width.I(), raw_image_height.I());
		cv::Size roi_image_size = cv::Size(ai_inspection_image.cols, ai_inspection_image.rows);
		cv::Point left_top(x1, y1);
		cv::Point right_bottom(x2, y2);
		cv::Rect roi_rect_full = cv::Rect(x1, y1, x2 - x1, y2 - y1);
		std::optional<cv::Mat> mask_image;
		if (!all_roi_inspection) {
			mask_image = AIService::ImageUtile::RegionToLabeledImageV2(HDefectAllRgn,
																	   orignal_image_size,
																	   roi_image_size,
																	   left_top,
																	   right_bottom);
			THEAPP.m_syai_logger->debug("[TEST Inspection] Using defect region for mask generation.");
			inspection_req_rgn = HDefectAllRgn;
		}
		else {
			mask_image = AIService::ImageUtile::RegionToLabeledImageV2(HROIHRegion,
																	   orignal_image_size,
																	   roi_image_size,
																	   left_top,
																	   right_bottom);
			THEAPP.m_syai_logger->debug("[TEST Inspection] Using full ROI region for mask generation.");
			inspection_req_rgn = HROIHRegion;
		}
		if (!mask_image.has_value()) {
			THEAPP.m_syai_logger->error("[TEST Inspection] Mask image generation failed.");
			can_submit = false;
		}
		if (!enable_roi_processing) {
			THEAPP.m_syai_logger->debug("[TEST Inspection] ROI Image Preprocessing disabled. Skip AI inspection.");
			can_submit = false;
		}
		if ((THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType - 1 >= INSPECTION_TYPE_FIRST &&
			 THEAPP.m_pInspectAdminViewDlg->mpLastSelectedRegion->miInspectionType - 1 <= INSPECTION_TYPE_LAST) == FALSE) {
			can_submit = false;
			THEAPP.m_syai_logger->error("[TEST Inspection] Invalid inspection type for AI inspection.");
		}
		if (can_submit && CGFunction::ValidHRegion(inspection_req_rgn)) {
			// 마스크 이진화
			cv::Mat mask_binary;
			cv::threshold(mask_image.value(), mask_binary, 0, 255, cv::THRESH_BINARY);
			// 검사 정보 확인
			std::string equip_no = "TEST EQ";
			std::string product_name = "TEST PRODUCT";
			std::string vision_name = "TEST VISION";
			int pc_idx = THEAPP.Struct_PreferenceStruct.m_iPCType + 1;
			std::string inspectiontype_name;
			if (MAX_INSPECTION_TYPE < m_pSelectedROI->miInspectionType - 1) {
				inspectiontype_name = "Other Inspection Type";
			}
			else {
				inspectiontype_name = g_strInspectionTypeName_Short[m_pSelectedROI->miInspectionType - 1];
			}
			// --- AI 모델 Key 생성
			syai_service::NameBuildOptions build_options;
			build_options.consider_vision_pos = false;
			build_options.consider_inspection_type = true;
			build_options.consider_image_index = AlgorithmParam.m_bCheckAIUseConsiderImageNo ? true : false;
			build_options.consider_defect_type = AlgorithmParam.m_bCheckAIUseConsiderDefectCode ? true : false;
			build_options.ignore_case = false;
			build_options.allow_input_duplicates = false;
			build_options.distinct = true;
			// --- 관련 정보 가져와서 Model Key 생성
			std::string camera_type = (LPCSTR)THEAPP.m_ModelDefineInfo.m_strVisionName_Short[THEAPP.m_iCurVisionCamType];
			std::string inspection_type = inspectiontype_name;
			std::string image_index_str = std::to_string(THEAPP.m_pTabControlDlg->m_iCurrentTab);
			std::string defect_name = std::string(CT2A(THEAPP.m_strDefectName[AlgorithmParam.m_iDefectType]));
			std::string defect_type = std::string(CT2A(THEAPP.m_strDefectCode[AlgorithmParam.m_iDefectType]));
			std::string model_key = syai_service::ServiceDirector::build_name(camera_type, "", inspection_type, image_index_str, defect_type, build_options);
			// 이미지 정보
			cv::Rect roi_rect = cv::Rect(x1, y1, x2 - x1, y2 - y1);
			// 검사 ID, 로그 ID, 모델 키 생성
			std::string inspection_id = equip_no;
			// std::string log_id = barcode_id + "_" + std::to_string(roi_id) + "_" + inspectiontype_name + "_" + model_key + "_" + std::to_string(iTabIdx);
			std::string log_id = "TEST_LOG_ID_" + inspectiontype_name + "_" + model_key + "_TEST_TAB";
			// 요청 객체 생성
			syai::runtime::domain::inspection::Request request(inspection_id.c_str(), log_id.c_str(), model_key.c_str());
			// 이미지 데이터 설정
			request.set_image(ai_inspection_image);
			request.set_mask(mask_binary);
			request.set_area(roi_rect_full);
			//cv::imwrite("C:\\Temp\\syai_teaching_mode_input_image.bmp", cv_roi_img_v2);
			//request.set_original_image_size(orignal_image_size);
			//request.left_top = left_top;
			//request.right_bottom = right_bottom;
		   // 검사 조건 확인
			if (AlgorithmParam.m_bUseFilter1) { request.add_inspection_step_enable(true); } // 1차
			else { request.add_inspection_step_enable(false); }
			if (AlgorithmParam.m_bUseFilter2) { request.add_inspection_step_enable(true); } // 2차
			else { request.add_inspection_step_enable(false); }
			if (AlgorithmParam.m_bUseFilter3) { request.add_inspection_step_enable(true); } // 3차
			else { request.add_inspection_step_enable(false); }
			// addon 설정
			syai_service::JobInfoAddon addon;
			addon.file_name_prefix = "teaching_mode_test_";
			addon.vision_inspection_thread_number = 0;
			addon.image_index = 0;
			addon.roi_index = 0;
			addon.inspection_tab_index = 0;
			// 검사 요청
			auto response = THEAPP.m_syai_service->add_job(request, addon);

			//cv::Size patch_size = cv::Size(256, 256);
			//int stride = 192;

			//THEAPP.m_syai_service->save_train_image_save(
			//	"C:\\Temp\\Crop\\",
			//	cv_roi_img_v2,
			//	patch_size,
			//	stride,
			//	"CropImage"
			//);

			try {
				/*if (response.wait_for(std::chrono::seconds(5)) == std::future_status::timeout) {
					THEAPP.m_syai_logger->error("SYAI result timed out after 5 seconds");
					throw std::runtime_error("SYAI result timed out after 5 seconds");
				}*/
				response.wait();

				cv::Mat result_mask;
				syai_service::JobInfo result = response.get();
				bool image_processed = true;
				THEAPP.m_syai_logger->info("[TEST Inspection] Start concatenating defect regions.");
				if (!result.request.has_value()) {
					THEAPP.m_syai_logger->error("[TEST Inspection] Invalid JobInfo: missing request");
					image_processed = false;
				}
				if (!result.addon.has_value()) {
					THEAPP.m_syai_logger->error("[TEST Inspection] Invalid JobInfo: missing addon");
					image_processed = false;
				}
				if (!result.response.has_value()) {
					THEAPP.m_syai_logger->error("[TEST Inspection] Invalid JobInfo: missing response");
					image_processed = false;
				}
				cv::Mat mask = result.get_last_step_mask();
				if (mask.empty()) {
					THEAPP.m_syai_logger->info("[TEST Inspection] No defect regions found in the last step mask.");
					image_processed = false;
				}
				if (!AIService::ImageUtile::HasImageData(mask)) {
					THEAPP.m_syai_logger->info("[TEST Inspection] Last step mask has no image data.");
					image_processed = false;
				}
				HObject region = AIService::ImageUtile::Mask2HRegion(mask);
				if (!CGFunction::ValidHRegion(region)) {
					THEAPP.m_syai_logger->info("[TEST Inspection] Converted HRegion is invalid.");
					image_processed = false;
				}
				HObject restored_region;
				AIService::ImageUtile::RestoreRegionToOriginal(region, result.request->get_area(), restored_region);
				if (!CGFunction::ValidHRegion(restored_region)) {
					THEAPP.m_syai_logger->info("[TEST Inspection] Restored region is invalid.");
					image_processed = false;
				}
				THEAPP.m_syai_logger->info("[TEST Inspection] Defect regions concatenated successfully.");

				if (image_processed) {
					ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HAIDefectRgn, restored_region, &(THEAPP.m_pInspectAdminViewDlg->m_HAIDefectRgn));
					Union1(THEAPP.m_pInspectAdminViewDlg->m_HAIDefectRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HAIDefectRgn));
				}
				result.release();
			}
			catch (const std::exception& e) {
				THEAPP.m_syai_logger->error("Error in delayed SYAI processing: {}", e.what());
			}
			catch (...) {
				THEAPP.m_syai_logger->error("Unknown error in delayed SYAI processing");
			}
		}
#pragma endregion AI Inspection
	}
#endif // SYAI

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
	THEAPP.m_pAlgorithm->m_bManualBarcodeForOcrInspect = FALSE;

	double tEndTime = GetTickCount();
	double tElapsedTime = tEndTime - tStartTime;

	strLog.Format("Teaching mode, Test button inspect time(ms): %.2lf", tElapsedTime);
	THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));
	SetWindowText((LPCTSTR)strLog);

	return true;
}
