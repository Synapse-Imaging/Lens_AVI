// RoiIdChangeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "RoiIdChangeDlg.h"
#include "afxdialogex.h"


// CRoiIdChangeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRoiIdChangeDlg, CDialog)

CRoiIdChangeDlg::CRoiIdChangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoiIdChangeDlg::IDD, pParent)
{
	m_iEditId = 1;
	m_iROIType = INSPECTION_TYPE_LOCAL_ALIGN;
}

CRoiIdChangeDlg::~CRoiIdChangeDlg()
{
}

void CRoiIdChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ID, m_iEditId);
	DDV_MinMaxInt(pDX, m_iEditId, 1, 1000);
}


BEGIN_MESSAGE_MAP(CRoiIdChangeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRoiIdChangeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRoiIdChangeDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CRoiIdChangeDlg 메시지 처리기입니다.

BOOL CRoiIdChangeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CRoiIdChangeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CRoiIdChangeDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	BOOL bIDUsed = FALSE;

	if (m_iROIType >= INSPECTION_TYPE_FIRST && m_iROIType <= INSPECTION_TYPE_LAST)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckInspectionROIID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckLocalAlignID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_DONTCARE)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckDontCareID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_GENERATE)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckGenerateID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_MASK)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckMaskID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_COLOR)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckColorInfoID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}

	if (bIDUsed)
	{
		if (m_iROIType == INSPECTION_TYPE_LOCAL_ALIGN || m_iROIType == INSPECTION_TYPE_DONTCARE || m_iROIType == INSPECTION_TYPE_GENERATE || m_iROIType == INSPECTION_TYPE_MASK)
		{
			int nRes = 0;
			nRes = AfxMessageBox("변경할 ID는 이미 사용중입니다. 설정한 ID를 지정하시겠습니까?", MB_YESNO | MB_ICONWARNING | MB_SYSTEMMODAL);

			if ((nRes == IDNO))
			{
				return;
			}
		}
		else
		{
			AfxMessageBox("변경할 ID는 이미 사용중입니다. 다른 ID를 지정하여 주십시요.", MB_SYSTEMMODAL);
			return;
		}
	}

	CDialog::OnOK();
}


void CRoiIdChangeDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
