// RoiAutoCreateDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include <afxtempl.h> 
#include <afxcmn.h>
#include <afxwin.h>
#include "uScan.h"
#include "RoiAutoCreateDlg.h"
#include "afxdialogex.h"

// SNZEROMQ
#ifdef SNZEROMQ
#include <snzeromq.client/Service/ZeroMQClient.h>
#include <snzeromq.client/Helpers/ConvertHelper.h>
#include <snzeromq.client/Helpers/ContourProcessHelper.h>
#endif // SNZEROMQ

// CRoiAutoCreateDlg 대화 상자

IMPLEMENT_DYNAMIC(CRoiAutoCreateDlg, CDialogEx)

CRoiAutoCreateDlg::CRoiAutoCreateDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ROI_AUTO_CREATE_DLG, pParent)
{
	m_iModel_Type_index = 2;
	m_fPred_iou_threshold = 0.8;
	m_fStability_score_thresh = 0.95;
	m_fStability_score_offset = 1.0;
	m_fMask_threshold = 0.0;
	m_fBox_nms_thresh = 0.7;
	m_iMin_mask_region_area = 0;
	m_bUse_m2m = 0;
}

CRoiAutoCreateDlg::~CRoiAutoCreateDlg()
{
}

void CRoiAutoCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MODEL_SELECT, m_ComboSelectManualRoiModel);

	DDX_Text(pDX, IDC_EDIT_PRED_IOU_THRESHOLD, m_fPred_iou_threshold);
	DDX_Text(pDX, IDC_EDIT_STABILITY_SCORE_THRESHOLD, m_fStability_score_thresh);
	DDX_Text(pDX, IDC_EDIT_STABILITY_SCORE_OFFSET, m_fStability_score_offset);
	DDX_Text(pDX, IDC_EDIT_MASK_THRESHOLD, m_fMask_threshold);
	DDX_Text(pDX, IDC_EDIT_BOX_NMS_THRESH, m_fBox_nms_thresh);
	DDX_Text(pDX, IDC_EDIT_MIN_MASK_REGION_AREA, m_iMin_mask_region_area);
	DDX_Check(pDX, IDC_CHECK_USE_M2M, m_bUse_m2m); // 체크박스와 멤버 변수 연결
}


BEGIN_MESSAGE_MAP(CRoiAutoCreateDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CRoiAutoCreateDlg::OnBnClickedButtonOk)
END_MESSAGE_MAP()


// CRoiAutoCreateDlg 메시지 처리기

BOOL CRoiAutoCreateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (!m_ToolTip.Create(this, TTS_ALWAYSTIP | TTS_BALLOON))
	{
		TRACE("툴팁 컨트롤 생성 실패\n");
		return FALSE;
	}

	m_ToolTip.Activate(TRUE);
	CString temp;

	m_ComboSelectManualRoiModel.AddString("Tiny");
	m_ComboSelectManualRoiModel.AddString("Small");
	m_ComboSelectManualRoiModel.AddString("Base+");
	m_ComboSelectManualRoiModel.AddString("Large");

	m_ComboSelectManualRoiModel.SetCurSel(m_iModel_Type_index);

	m_ToolTip.AddTool(GetDlgItem(IDC_COMBO_MODEL_SELECT), _T("사용할 SAM 2 모델을 지정합니다."));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_PRED_IOU_THRESHOLD), _T("모델이 예측한 마스크 품질을 사용하여 필터링할 때 사용하는 IoU(Intersection over Union) 임계값입니다. 0과 1 사이의 값을 가집니다."));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_STABILITY_SCORE_THRESHOLD), _T("모델의 마스크 예측을 이진화하는 데 사용되는 컷오프 값의 변화에 대한 마스크 안정성을 사용하여 필터링할 때 사용하는 임계값입니다. 0과 1 사이의 값을 가집니다."));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_STABILITY_SCORE_OFFSET), _T("안정성 점수를 계산할 때 컷오프 값을 이동시키는 양입니다."));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_MASK_THRESHOLD), _T("마스크 로짓을 이진화하기 위한 임계값입니다."));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_BOX_NMS_THRESH), _T("중복 마스크를 필터링하기 위해 NMS(Non-Maximum Suppression)에서 사용하는 Box IoU 컷오프 값입니다."));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_MIN_MASK_REGION_AREA), _T("0보다 큰 경우, min_mask_region_area보다 작은 영역을 가진 마스크에서 연결되지 않은 영역과 구멍을 제거하는 후처리가 적용됩니다."));
	m_ToolTip.AddTool(GetDlgItem(IDC_CHECK_USE_M2M), _T("이전 마스크 예측을 사용하여 한 단계 정제 단계를 추가할지 여부를 지정합니다."));
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 0);   // 툴팁을 즉시 표시 (지연 시간 0)
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 10000); // 툴팁을 5초 동안 표시
	m_ToolTip.SetDelayTime(TTDT_RESHOW, 100);
	return TRUE;
}

void CRoiAutoCreateDlg::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다. 
	CString temp;

	m_iModel_Type_index = m_ComboSelectManualRoiModel.GetCurSel();
	GetDlgItemText(IDC_EDIT_PRED_IOU_THRESHOLD, temp);
	m_fPred_iou_threshold = _ttof(temp);
	GetDlgItemText(IDC_EDIT_STABILITY_SCORE_THRESHOLD, temp);
	m_fStability_score_thresh = _ttof(temp);
	GetDlgItemText(IDC_EDIT_STABILITY_SCORE_OFFSET, temp);
	m_fStability_score_offset = _ttof(temp);
	GetDlgItemText(IDC_EDIT_MASK_THRESHOLD, temp);
	m_fMask_threshold = _ttof(temp);
	GetDlgItemText(IDC_EDIT_BOX_NMS_THRESH, temp);
	m_fBox_nms_thresh = _ttof(temp);
	GetDlgItemText(IDC_EDIT_MIN_MASK_REGION_AREA, temp);
	m_iMin_mask_region_area = _ttof(temp);

	CDialogEx::OnOK();
}


BOOL CRoiAutoCreateDlg::PreTranslateMessage(MSG * pMsg)
{
	if (m_ToolTip.GetSafeHwnd()) // 툴팁 컨트롤이 생성되었는지 확인
	{
		m_ToolTip.RelayEvent(pMsg);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CRoiAutoCreateDlg::OnEnChangeMyEdit()
{
	//CString strText;
	//GetDlgItemText(IDC_EDIT_PRED_IOU_THRESHOLD, strText);

	// 툴팁 텍스트 업데이트
	//m_ToolTip.UpdateTipText(_T("현재 입력된 텍스트: ") + strText, GetDlgItem(IDC_EDIT_PRED_IOU_THRESHOLD));
}
