// TeachFaiBosDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "TeachFaiBosDlg.h"
#include "afxdialogex.h"


// CTeachFaiBosDlg 대화 상자

IMPLEMENT_DYNAMIC(CTeachFaiBosDlg, CDialog)

CTeachFaiBosDlg::CTeachFaiBosDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TEACH_FAI_BOS_DLG, pParent)
{
	m_bCheckUseFai = FALSE;

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (CFAIDataManager::GetInstance().IsFAIUsed(i))
		{
			m_bCheckInspectFai[i] = FALSE;
			m_dEditNominalFai[i] = 1;
			m_dEditToleranceMaxFai[i] = 0;
			m_dEditToleranceMinFai[i] = 0;

			m_dEditMultipleStg1Fai[i] = 1;
			m_dEditMultipleStg2Fai[i] = 1;
			m_dEditOffsetStg1Fai[i] = 0;
			m_dEditOffsetStg2Fai[i] = 0;
		}
	}
}

CTeachFaiBosDlg::~CTeachFaiBosDlg()
{
}

void CTeachFaiBosDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_bnOK);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);

	DDX_Check(pDX, IDC_CHECK_USE_FAI_INSPECTION, m_bCheckUseFai);

	int iFAINumber;

	// FAI-1
	// TODO : FAI 추가할 때마다 리소스 9개씩 추가하고 이름 수정하고 해야함...
	// 		: 말도 안되는 작업량

	// TODO : 아래 하드코딩된 FAI Number 도 문제.
	iFAINumber = 1;
	DDX_Check(pDX, IDC_CHECK_BOS_INSPECT_FAI_1, m_bCheckInspectFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_BOS_NOMINAL_FAI_1, m_dEditNominalFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_BOS_TOLERANCE_MAX_FAI_1, m_dEditToleranceMaxFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_BOS_TOLERANCE_MIN_FAI_1, m_dEditToleranceMinFai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_BOS_MULTIPLE_STG1_FAI_1, m_dEditMultipleStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_BOS_MULTIPLE_STG2_FAI_1, m_dEditMultipleStg2Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_BOS_OFFSET_STG1_FAI_1, m_dEditOffsetStg1Fai[iFAINumber]);
	DDX_Text(pDX, IDC_EDIT_BOS_OFFSET_STG2_FAI_1, m_dEditOffsetStg2Fai[iFAINumber]);
	DDX_Control(pDX, IDC_COMBO_BOS_FAI_DEFECT_CODE_1, m_cbSpecialNGCode[iFAINumber]);
}


BEGIN_MESSAGE_MAP(CTeachFaiBosDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTeachFaiBosDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTeachFaiBosDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CTeachFaiBosDlg 메시지 처리기

BOOL CTeachFaiBosDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}
BOOL CTeachFaiBosDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_bnOK.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnCancel.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

	m_bCheckUseFai = THEAPP.m_pModelDataManager->m_bMUseFai;

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (CFAIDataManager::GetInstance().IsFAIUsed(i))
		{
			m_bCheckInspectFai[i] = THEAPP.m_pModelDataManager->m_bMInspectFai[i];
			m_dEditNominalFai[i] = THEAPP.m_pModelDataManager->m_dMNominalFai[i];
			m_dEditToleranceMaxFai[i] = THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i];
			m_dEditToleranceMinFai[i] = THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i];
			m_dEditMultipleStg1Fai[i] = THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai[i];
			m_dEditMultipleStg2Fai[i] = THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai[i];
			m_dEditOffsetStg1Fai[i] = THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i];
			m_dEditOffsetStg2Fai[i] = THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i];
		}
	}

	CString sSpecialNGCode;
	int iComboIdx;

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (CFAIDataManager::GetInstance().IsFAIUsed(i))
		{
			// ComboBox가 DDX_Control로 윈도우 핸들에 연결되었는지 확인 (크래시 방지)
			if (m_cbSpecialNGCode[i].GetSafeHwnd() == NULL)
				continue;

			// 특수 NG 코드 목록을 ComboBox에 추가 (_T() 매크로 불필요 - CString은 자동 변환됨)
			for (int j = 0; j < THEAPP.Struct_PreferenceStruct.iNoAssignSpecialCode; j++)
				m_cbSpecialNGCode[i].AddString(THEAPP.Struct_PreferenceStruct.m_sAssignSpecialCode[j]);

			// 저장된 선택 인덱스로 ComboBox 설정
			iComboIdx = THEAPP.m_pModelDataManager->m_iTransferCodeIndexFai[i];
			m_cbSpecialNGCode[i].SetCurSel(iComboIdx);
		}
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTeachFaiBosDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();

	int iComboIdx;

	THEAPP.m_pModelDataManager->m_bMUseFai = m_bCheckUseFai;

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (CFAIDataManager::GetInstance().IsFAIUsed(i))
		{
			THEAPP.m_pModelDataManager->m_bMInspectFai[i] = m_bCheckInspectFai[i];
			THEAPP.m_pModelDataManager->m_dMNominalFai[i] = m_dEditNominalFai[i];
			THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i] = m_dEditToleranceMaxFai[i];
			THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i] = m_dEditToleranceMinFai[i];
			THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai[i] = m_dEditMultipleStg1Fai[i];
			THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai[i] = m_dEditMultipleStg2Fai[i];
			THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i] = m_dEditOffsetStg1Fai[i];
			THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i] = m_dEditOffsetStg2Fai[i];

			// ComboBox가 DDX_Control로 윈도우 핸들에 연결되었는지 확인 (크래시 방지)
			if (m_cbSpecialNGCode[i].GetSafeHwnd() == NULL)
				continue;

			iComboIdx = m_cbSpecialNGCode[i].GetCurSel();
			THEAPP.m_pModelDataManager->m_iTransferCodeIndexFai[i] = iComboIdx;

			for (int iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
			{
				if (THEAPP.m_iCurTeachVision)
					continue;

				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bMInspectFai[i] = THEAPP.m_pModelDataManager->m_bMInspectFai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMNominalFai[i] = THEAPP.m_pModelDataManager->m_dMNominalFai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMToleranceMaxFai[i] = THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMToleranceMinFai[i] = THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMMultipleStg1Fai[i] = THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMMultipleStg2Fai[i] = THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMOffsetStg1Fai[i] = THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i];
				THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_dMOffsetStg2Fai[i] = THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i];

			}

		}
	}

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;


	CString sModelSwPath;
	CString strSection;
	CString sTemp;

	for (int iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
	{
		sModelSwPath.Format("%s\\SW\\Vision_N%d\\FaiMeasureSpec_DFA.ini", strModelFolder, iPcVisionNo + 1);

		CIniFileCS INI_FAI_SW(sModelSwPath);

		strSection = "FAI";
		sTemp.Format("m_bMUseFai");
		INI_FAI_SW.Set_Bool(strSection, sTemp, THEAPP.m_pModelDataManager->m_bMUseFai);

		for (int i = 0; i < MAX_FAI_ITEM; i++)
		{
			if (CFAIDataManager::GetInstance().IsFAIUsed(i))
			{
				sTemp.Format("m_bMInspectFai_%d", i);
				INI_FAI_SW.Set_Bool(strSection, sTemp, THEAPP.m_pModelDataManager->m_bMInspectFai[i]);
				sTemp.Format("m_dMNominalFai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMNominalFai[i]);
				sTemp.Format("m_dMToleranceMaxFai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMToleranceMaxFai[i]);
				sTemp.Format("m_dMToleranceMinFai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMToleranceMinFai[i]);
				sTemp.Format("m_dMMultipleStg1Fai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai[i]);
				sTemp.Format("m_dMMultipleStg2Fai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai[i]);
				sTemp.Format("m_dMOffsetStg1Fai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai[i]);
				sTemp.Format("m_dMOffsetStg2Fai_%d", i);
				INI_FAI_SW.Set_Double(strSection, sTemp, THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai[i]);
				sTemp.Format("m_iTransferCodeIndexFai_%d", i);
				INI_FAI_SW.Set_Integer(strSection, sTemp, THEAPP.m_pModelDataManager->m_iTransferCodeIndexFai[i]);
				sTemp.Format("m_bMultiFai_%d", i);
				INI_FAI_SW.Set_Bool(strSection, sTemp, THEAPP.m_pModelDataManager->m_bMultiFai[i]);
			}
		}

	}


	CDialog::OnOK();
}


void CTeachFaiBosDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
