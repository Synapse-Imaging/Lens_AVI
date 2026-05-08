// AlgorithmPartCheckDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmPartCheckDlg.h"
#include "afxdialogex.h"


// CAlgorithmPartCheckDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmPartCheckDlg, CDialog)

CAlgorithmPartCheckDlg::CAlgorithmPartCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmPartCheckDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUsePartCheck = FALSE;
	m_bCheckPartCheckMultipleModel = FALSE;
	m_iEditPartCheckMatchingAngleRange = 0;
	m_iEditPartCheckMatchingPLevel = 0;
	m_iEditPartCheckMatchingScore = 0;
	m_iEditPartCheckMatchingSearchMarginX = 0;
	m_iEditPartCheckMatchingSearchMarginX2 = 0;
	m_iEditPartCheckMatchingSearchMarginY = 0;
	m_iEditPartCheckMatchingSearchMarginY2 = 0;

	m_bCheckPartCheckInspectExistence = FALSE;
	m_bCheckPartCheckBlob = FALSE;
	m_bCheckPartCheckInspectShift = FALSE;
	m_iEditPartCheckShiftX = 0;
	m_iEditPartCheckShiftY = 0;
	m_dEditPartCheckRotationAngle = 0;
	m_bCheckPartCheckLocalAlignUse = FALSE;

	m_bCheckPartCheckApplyComp = FALSE;
	m_bCheckPartCheckSaveLog = FALSE;

	m_bCheckPartCheckMeasureEdge = FALSE;
	m_iEditPartCheckLegXSize = 24;
	m_iEditPartCheckLegYSize = 80;
	m_dEditPartCheckEdgeMeasueSmFactor = 1.0;
	m_dEditPartCheckEdgeMeasueEdgeStr = 10.0;
	m_bCheckPartCheckInspectLegDamage = FALSE;

	m_bCheckPartCheckMultiModuleInspect = FALSE;
	m_iEditPartCheckMultiModuleXNumber = 1;
	m_iEditPartCheckMultiModuleYNumber = 1;
	m_iEditPartCheckMultiModuleXPitch = 1000;
	m_iEditPartCheckMultiModuleYPitch = 1000;

	m_bCheckPartCheckPickerPosUse = FALSE;
	m_bCheckPartCheckMultiModuleMoveOffset = FALSE;
	m_iRadioPartCheckMultiModuleTarget = MULTI_MODULE_PART_CHECK_MODULE;
}

CAlgorithmPartCheckDlg::~CAlgorithmPartCheckDlg()
{
}

void CAlgorithmPartCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_PART_CHECK, m_bCheckUsePartCheck);
	DDX_Check(pDX, IDC_CHECK_PART_CHECK_MULTIPLE_MODEL, m_bCheckPartCheckMultipleModel);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_ANGLE_RANGE, m_iEditPartCheckMatchingAngleRange);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_PLEVEL, m_iEditPartCheckMatchingPLevel);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_SCORE, m_iEditPartCheckMatchingScore);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_SEARCH_MARGIN_X, m_iEditPartCheckMatchingSearchMarginX);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_SEARCH_MARGIN_X2, m_iEditPartCheckMatchingSearchMarginX2);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_SEARCH_MARGIN_Y, m_iEditPartCheckMatchingSearchMarginY);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_SEARCH_MARGIN_Y2, m_iEditPartCheckMatchingSearchMarginY2);
	DDX_Check(pDX, IDC_CHECK_PART_CHECK_MODEL_EXIST, m_bCheckPartCheckModelExist);
	DDX_Check(pDX, IDC_CHECK_PART_CHECK_INSPECT_EXISTENCE, m_bCheckPartCheckInspectExistence);
	DDX_Check(pDX, IDC_CHECK_PART_CHECK_INSPECT_EXISTENCE_USING_BLOB, m_bCheckPartCheckBlob);
	DDX_Check(pDX, IDC_CHECK_PART_CHECK_INSPECT_SHIFT, m_bCheckPartCheckInspectShift);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_SHIFT_X, m_iEditPartCheckShiftX);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_SHIFT_Y, m_iEditPartCheckShiftY);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_ROTATION_ANGLE, m_dEditPartCheckRotationAngle);
	DDX_Check(pDX, IDC_CHECK_PART_CHECK_LOCAL_ALIGN_USE, m_bCheckPartCheckLocalAlignUse);
	DDX_Check(pDX, IDC_CHECK_PART_CHECK_APPLY_COMP, m_bCheckPartCheckApplyComp);
	DDX_Check(pDX, IDC_CHECK_PART_CHECK_SAVE_LOG, m_bCheckPartCheckSaveLog);

	DDX_Check(pDX, IDC_CHECK_PART_CHECK_MEASURE_EDGE, m_bCheckPartCheckMeasureEdge);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_LEG_X_SIZE, m_iEditPartCheckLegXSize);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_LEG_Y_SIZE, m_iEditPartCheckLegYSize);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_EDGE_MEASURE_SMOOTHING_FACTOR, m_dEditPartCheckEdgeMeasueSmFactor);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_EDGE_MEASURE_EDGE_STRENGTH, m_dEditPartCheckEdgeMeasueEdgeStr);
	DDX_Check(pDX, IDC_CHECK_PART_CHECK_INSPECT_LEG_DAMAGE, m_bCheckPartCheckInspectLegDamage);

	DDX_Check(pDX, IDC_CHECK_PART_CHECK_MULTI_MODULE_INSPECT, m_bCheckPartCheckMultiModuleInspect);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MULTI_MODULE_X_NUMBER, m_iEditPartCheckMultiModuleXNumber);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MULTI_MODULE_Y_NUMBER, m_iEditPartCheckMultiModuleYNumber);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MULTI_MODULE_X_PITCH, m_iEditPartCheckMultiModuleXPitch);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MULTI_MODULE_Y_PITCH, m_iEditPartCheckMultiModuleYPitch);

	DDX_Check(pDX, IDC_CHECK_PART_CHECK_PICKER_POS_USE, m_bCheckPartCheckPickerPosUse);

	DDX_Check(pDX, IDC_CHECK_PART_CHECK_MULTI_MODULE_MOVE_OFFSET, m_bCheckPartCheckMultiModuleMoveOffset);

	DDX_Radio(pDX, IDC_RADIO_PART_CHECK_MULTI_MODULE_MODULE, m_iRadioPartCheckMultiModuleTarget);
}

BEGIN_MESSAGE_MAP(CAlgorithmPartCheckDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_MATCHING_MODEL, &CAlgorithmPartCheckDlg::OnBnClickedButtonCreateMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_TEST_MATCHING_MODEL, &CAlgorithmPartCheckDlg::OnBnClickedButtonTestMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_MATCHING_MODEL, &CAlgorithmPartCheckDlg::OnBnClickedButtonDeleteMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_TEST_MATCHING_MULTI_MODULE, &CAlgorithmPartCheckDlg::OnBnClickedButtonTestMatchingMultiModule)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_MODULE_MOVE_OFFSET, &CAlgorithmPartCheckDlg::OnBnClickedButtonMultiModuleMoveOffset)
END_MESSAGE_MAP()


// CAlgorithmPartCheckDlg 메시지 처리기입니다.

void CAlgorithmPartCheckDlg::SetSelectedInspection(int iImageType, GTRegion* pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;

	if (pSelectedROI->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		GetDlgItem(IDC_CHECK_USE_PART_CHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_CREATE_MATCHING_MODEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DELETE_MATCHING_MODEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_TEST_MATCHING_MODEL)->EnableWindow(TRUE);
	}
	else
	{
		if (m_iSelectedInspectionTabIndex == 2)
		{
			GetDlgItem(IDC_CHECK_USE_PART_CHECK)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_CREATE_MATCHING_MODEL)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_DELETE_MATCHING_MODEL)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_TEST_MATCHING_MODEL)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_PART_CHECK)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_CREATE_MATCHING_MODEL)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_DELETE_MATCHING_MODEL)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_TEST_MATCHING_MODEL)->EnableWindow(FALSE);
		}
	}

	GetDlgItem(IDC_CHECK_PART_CHECK_MULTIPLE_MODEL)->EnableWindow(FALSE);
}

void CAlgorithmPartCheckDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	TempAlgoParam = AlgorithmParam;

	m_bCheckUsePartCheck = AlgorithmParam.m_bUsePartCheck;
	m_bCheckPartCheckMultipleModel = AlgorithmParam.m_bPartCheckMultipleModel;
	m_iEditPartCheckMatchingAngleRange = AlgorithmParam.m_iPartCheckMatchingAngleRange;
	m_iEditPartCheckMatchingPLevel = AlgorithmParam.m_iPartCheckMatchingPLevel;
	m_iEditPartCheckMatchingScore = AlgorithmParam.m_iPartCheckMatchingScore;
	m_iEditPartCheckMatchingSearchMarginX = AlgorithmParam.m_iPartCheckMatchingSearchMarginX;
	m_iEditPartCheckMatchingSearchMarginY = AlgorithmParam.m_iPartCheckMatchingSearchMarginY;
	m_iEditPartCheckMatchingSearchMarginX2 = AlgorithmParam.m_iPartCheckMatchingSearchMarginX2;
	m_iEditPartCheckMatchingSearchMarginY2 = AlgorithmParam.m_iPartCheckMatchingSearchMarginY2;

	m_bCheckPartCheckInspectExistence = AlgorithmParam.m_bPartCheckInspectExistence;
	m_bCheckPartCheckBlob = AlgorithmParam.m_bPartCheckBlob;
	m_bCheckPartCheckInspectShift = AlgorithmParam.m_bPartCheckInspectShift;
	m_iEditPartCheckShiftX = AlgorithmParam.m_iPartCheckShiftX;
	m_iEditPartCheckShiftY = AlgorithmParam.m_iPartCheckShiftY;
	m_dEditPartCheckRotationAngle = AlgorithmParam.m_dPartCheckRotationAngle;
	m_bCheckPartCheckLocalAlignUse = AlgorithmParam.m_bPartCheckLocalAlignUse;

	m_bCheckPartCheckApplyComp = AlgorithmParam.m_bPartCheckApplyComp;
	m_bCheckPartCheckSaveLog = AlgorithmParam.m_bPartCheckSaveLog;

	m_bCheckPartCheckMeasureEdge = AlgorithmParam.m_bPartCheckMeasureEdge;
	m_iEditPartCheckLegXSize = AlgorithmParam.m_iPartCheckLegXSize;
	m_iEditPartCheckLegYSize = AlgorithmParam.m_iPartCheckLegYSize;
	m_dEditPartCheckEdgeMeasueSmFactor = AlgorithmParam.m_dPartCheckEdgeMeasueSmFactor;
	m_dEditPartCheckEdgeMeasueEdgeStr = AlgorithmParam.m_dPartCheckEdgeMeasueEdgeStr;
	m_bCheckPartCheckInspectLegDamage = AlgorithmParam.m_bPartCheckInspectLegDamage;

	m_bCheckPartCheckMultiModuleInspect = AlgorithmParam.m_bPartCheckMultiModuleInspect;
	m_iEditPartCheckMultiModuleXNumber = AlgorithmParam.m_iPartCheckMultiModuleXNumber;
	m_iEditPartCheckMultiModuleYNumber = AlgorithmParam.m_iPartCheckMultiModuleYNumber;
	m_iEditPartCheckMultiModuleXPitch = AlgorithmParam.m_iPartCheckMultiModuleXPitch;
	m_iEditPartCheckMultiModuleYPitch = AlgorithmParam.m_iPartCheckMultiModuleYPitch;

	m_bCheckPartCheckPickerPosUse = AlgorithmParam.m_bPartCheckPickerPosUse;

	m_bCheckPartCheckMultiModuleMoveOffset = AlgorithmParam.m_bPartCheckMultiModuleMoveOffset;

	m_iRadioPartCheckMultiModuleTarget = AlgorithmParam.m_iPartCheckMultiModuleTarget;

	if (m_pSelectedROI->m_HPartModelID >= 0)
		m_bCheckPartCheckModelExist = TRUE;
	else
		m_bCheckPartCheckModelExist = FALSE;

	UpdateData(FALSE);
}

void CAlgorithmPartCheckDlg::GetParam(CAlgorithmParam* pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUsePartCheck = m_bCheckUsePartCheck;
	pAlgorithmParam->m_bPartCheckMultipleModel = m_bCheckPartCheckMultipleModel;
	pAlgorithmParam->m_iPartCheckMatchingAngleRange = m_iEditPartCheckMatchingAngleRange;
	pAlgorithmParam->m_iPartCheckMatchingPLevel = m_iEditPartCheckMatchingPLevel;
	pAlgorithmParam->m_iPartCheckMatchingScore = m_iEditPartCheckMatchingScore;
	pAlgorithmParam->m_iPartCheckMatchingSearchMarginX = m_iEditPartCheckMatchingSearchMarginX;
	pAlgorithmParam->m_iPartCheckMatchingSearchMarginY = m_iEditPartCheckMatchingSearchMarginY;
	pAlgorithmParam->m_iPartCheckMatchingSearchMarginX2 = m_iEditPartCheckMatchingSearchMarginX2;
	pAlgorithmParam->m_iPartCheckMatchingSearchMarginY2 = m_iEditPartCheckMatchingSearchMarginY2;

	pAlgorithmParam->m_bPartCheckInspectExistence = m_bCheckPartCheckInspectExistence;
	pAlgorithmParam->m_bPartCheckBlob = m_bCheckPartCheckBlob;
	pAlgorithmParam->m_bPartCheckInspectShift = m_bCheckPartCheckInspectShift;
	pAlgorithmParam->m_iPartCheckShiftX = m_iEditPartCheckShiftX;
	pAlgorithmParam->m_iPartCheckShiftY = m_iEditPartCheckShiftY;
	pAlgorithmParam->m_dPartCheckRotationAngle = m_dEditPartCheckRotationAngle;
	pAlgorithmParam->m_bPartCheckLocalAlignUse = m_bCheckPartCheckLocalAlignUse;

	pAlgorithmParam->m_bPartCheckApplyComp = m_bCheckPartCheckApplyComp;
	pAlgorithmParam->m_bPartCheckSaveLog = m_bCheckPartCheckSaveLog;

	pAlgorithmParam->m_bPartCheckMeasureEdge = m_bCheckPartCheckMeasureEdge;
	pAlgorithmParam->m_iPartCheckLegXSize = m_iEditPartCheckLegXSize;
	pAlgorithmParam->m_iPartCheckLegYSize = m_iEditPartCheckLegYSize;
	pAlgorithmParam->m_dPartCheckEdgeMeasueSmFactor = m_dEditPartCheckEdgeMeasueSmFactor;
	pAlgorithmParam->m_dPartCheckEdgeMeasueEdgeStr = m_dEditPartCheckEdgeMeasueEdgeStr;
	pAlgorithmParam->m_bPartCheckInspectLegDamage = m_bCheckPartCheckInspectLegDamage;

	pAlgorithmParam->m_bPartCheckMultiModuleInspect = m_bCheckPartCheckMultiModuleInspect;
	pAlgorithmParam->m_iPartCheckMultiModuleXNumber = m_iEditPartCheckMultiModuleXNumber;
	pAlgorithmParam->m_iPartCheckMultiModuleYNumber = m_iEditPartCheckMultiModuleYNumber;
	pAlgorithmParam->m_iPartCheckMultiModuleXPitch = m_iEditPartCheckMultiModuleXPitch;
	pAlgorithmParam->m_iPartCheckMultiModuleYPitch = m_iEditPartCheckMultiModuleYPitch;

	pAlgorithmParam->m_bPartCheckPickerPosUse = m_bCheckPartCheckPickerPosUse;

	pAlgorithmParam->m_bPartCheckMultiModuleMoveOffset = m_bCheckPartCheckMultiModuleMoveOffset;

	pAlgorithmParam->m_iPartCheckMultiModuleTarget = m_iRadioPartCheckMultiModuleTarget;

	for (int i = 0; i < ALIGN_MODULE_Y; i++)
	{
		for (int j = 0; j < ALIGN_MODULE_X; j++)
		{
			pAlgorithmParam->m_iPartCheckMoveOffsetX[i][j] = TempAlgoParam.m_iPartCheckMoveOffsetX[i][j];
			pAlgorithmParam->m_iPartCheckMoveOffsetY[i][j] = TempAlgoParam.m_iPartCheckMoveOffsetY[i][j];
		}
	}
}

BOOL CAlgorithmPartCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAlgorithmPartCheckDlg::OnBnClickedButtonCreateMatchingModel()
{
	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		if (m_bCheckPartCheckMultipleModel)
		{
			if (AfxMessageBox("[멀티 모듈 각각 모델 생성 적용] 옵션이 On 되었습니다. 멀티 모듈 검사에 필요한 설정이 되어 있습니까?", MB_OKCANCEL) == IDCANCEL)
				return;

			HObject HInspectAreaRgn;
			HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

			HTuple HNoTemplate;
			Hlong lNoTemplate;
			HTuple HSelectedTemplate;

			TupleLength(m_pSelectedROI->m_HPartModelID, &HNoTemplate);
			lNoTemplate = HNoTemplate.L();

			for (int iIndex = 0; iIndex < lNoTemplate; iIndex++)
			{
				if (m_pSelectedROI->m_HPartModelID[iIndex] >= 0)
				{
					ClearNccModel(m_pSelectedROI->m_HPartModelID[iIndex]);
					m_pSelectedROI->m_HPartModelID[iIndex] = -1;
				}
			}

			for (int iIndex = 0; iIndex < lNoTemplate; iIndex++)
			{
				if (m_pSelectedROI->m_HPartModelID[iIndex] >= 0)
				{
					ClearNccModel(m_pSelectedROI->m_HPartModelID[iIndex]);
					m_pSelectedROI->m_HPartModelID[iIndex] = -1;
				}
			}

			// Model Ref: G-Center of Domain
			double dTeachAlignRefX, dTeachAlignRefY;
			Hlong lArea;
			HTuple HlArea, HdTeachAlignRefY, HdTeachAlignRefX;
			AreaCenter(HInspectAreaRgn, &HlArea, &HdTeachAlignRefY, &HdTeachAlignRefX);
			lArea = HlArea.L();
			dTeachAlignRefY = HdTeachAlignRefY.D();
			dTeachAlignRefX = HdTeachAlignRefX.D();

			HTuple dAngleRangeRad;
			TupleRad(m_iEditPartCheckMatchingAngleRange, &dAngleRangeRad);
			if (dAngleRangeRad < 0)
				dAngleRangeRad *= -1.0;

			int i, j, iMoveX, iMoveY;
			double dModuleAlignRefX, dModuleAlignRefY;
			HObject HTemplateTeachRgn;

			TupleGenConst(0, 0, &(m_pSelectedROI->m_HPartModelID));

			for (i = 0; i < m_iEditPartCheckMultiModuleYNumber; i++)
			{
				for (j = 0; j < m_iEditPartCheckMultiModuleXNumber; j++)
				{
					dModuleAlignRefX = dTeachAlignRefX + j * m_iEditPartCheckMultiModuleXPitch;
					dModuleAlignRefY = dTeachAlignRefY + i * m_iEditPartCheckMultiModuleYPitch;

					iMoveX = j * m_iEditPartCheckMultiModuleXPitch;
					iMoveY = i * m_iEditPartCheckMultiModuleYPitch;

					if (m_bCheckPartCheckMultiModuleMoveOffset)
					{
						iMoveX = TempAlgoParam.m_iPartCheckMoveOffsetX[i][j];
						iMoveY = TempAlgoParam.m_iPartCheckMoveOffsetY[i][j];
					}
					else
					{
						iMoveX = j * m_iEditPartCheckMultiModuleXPitch;
						iMoveY = i * m_iEditPartCheckMultiModuleYPitch;
					}

					dModuleAlignRefX = dTeachAlignRefX + iMoveX;
					dModuleAlignRefY = dTeachAlignRefY + iMoveY;

					MoveRegion(HInspectAreaRgn, &HTemplateTeachRgn, iMoveY, iMoveX);

					HObject HImageReduced;
					ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HTemplateTeachRgn, &HImageReduced);

					HSelectedTemplate = -1;
					CreateNccModel(HImageReduced, "auto", -dAngleRangeRad, dAngleRangeRad * 2.0, "auto", "use_polarity", &HSelectedTemplate);

					if (HSelectedTemplate == -1)
					{
						CString sMsg;
						sMsg.Format("X-%d Y-%d 번째 매칭 모델 생성 실패.", j + 1, i + 1);
						AfxMessageBox(sMsg, MB_SYSTEMMODAL);
						return;
					}

					TupleConcat(m_pSelectedROI->m_HPartModelID, HSelectedTemplate, &(m_pSelectedROI->m_HPartModelID));
				}
			}

			m_bCheckPartCheckModelExist = TRUE;
			UpdateData(FALSE);

			CString sMsg;
			sMsg.Format("%d 개 모델 생성 완료!!", m_iEditPartCheckMultiModuleXNumber * m_iEditPartCheckMultiModuleYNumber);
			AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);
		}
		else
		{
			HObject HInspectAreaRgn;
			HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

			if (m_pSelectedROI->m_HPartModelID >= 0)
			{
				ClearNccModel(m_pSelectedROI->m_HPartModelID);
				m_pSelectedROI->m_HPartModelID = -1;
			}

			// Model Ref: G-Center of Domain
			double dTeachAlignRefX, dTeachAlignRefY;
			Hlong lArea;
			HTuple HlArea, HdTeachAlignRefY, HdTeachAlignRefX;
			AreaCenter(HInspectAreaRgn, &HlArea, &HdTeachAlignRefY, &HdTeachAlignRefX);
			lArea = HlArea.L();
			dTeachAlignRefY = HdTeachAlignRefY.D();
			dTeachAlignRefX = HdTeachAlignRefX.D();

			HObject HImageReduced;
			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HInspectAreaRgn, &HImageReduced);

			HTuple dAngleRangeRad;
			TupleRad(m_iEditPartCheckMatchingAngleRange, &dAngleRangeRad);

			if (dAngleRangeRad < 0)
				dAngleRangeRad *= -1.0;

			CreateNccModel(HImageReduced, "auto", -dAngleRangeRad, dAngleRangeRad * 2.0, "auto", "use_polarity", &(m_pSelectedROI->m_HPartModelID));

			if (m_pSelectedROI->m_HPartModelID == -1)
			{
				AfxMessageBox("모델 생성 실패.", MB_SYSTEMMODAL);
				return;
			}

			// Test
			int iSearchMarginLeft = m_iEditPartCheckMatchingSearchMarginX;
			if (iSearchMarginLeft < 0)
				iSearchMarginLeft = 0;
			int iSearchMarginRight = m_iEditPartCheckMatchingSearchMarginX2;
			if (iSearchMarginRight < 0)
				iSearchMarginRight = 0;
			int iSearchMarginTop = m_iEditPartCheckMatchingSearchMarginY;
			if (iSearchMarginTop < 0)
				iSearchMarginTop = 0;
			int iSearchMarginBottom = m_iEditPartCheckMatchingSearchMarginY2;
			if (iSearchMarginBottom < 0)
				iSearchMarginBottom = 0;

			HObject HInspectAreaDilatedRgn;
			ShapeTrans(HInspectAreaRgn, &HInspectAreaDilatedRgn, "rectangle1");

			Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
			HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
			SmallestRectangle1(HInspectAreaDilatedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
			lLTPointY = HlLTPointY.L();
			lLTPointX = HlLTPointX.L();
			lRBPointY = HlRBPointY.L();
			lRBPointX = HlRBPointX.L();

			Hlong lImageWidth, lImageHeight;
			HTuple HImageWidth, HImageHeight;
			GetImageSize(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], &HImageWidth, &HImageHeight);
			lImageWidth = HImageWidth.L();
			lImageHeight = HImageHeight.L();

			Hlong lCropLTPointX, lCropLTPointY, lCropRBPointX, lCropRBPointY;

			lCropLTPointX = lLTPointX - iSearchMarginLeft;
			if (lCropLTPointX < 0)
				lCropLTPointX = 0;
			lCropLTPointY = lLTPointY - iSearchMarginTop;
			if (lCropLTPointY < 0)
				lCropLTPointY = 0;
			lCropRBPointX = lRBPointX + iSearchMarginRight;
			if (lCropRBPointX >= lImageWidth)
				lCropRBPointX = lImageWidth - 1;
			lCropRBPointY = lRBPointY + iSearchMarginBottom;
			if (lCropRBPointY >= lImageHeight)
				lCropRBPointY = lImageHeight - 1;

			HObject HMatchingImageReduced;
			CropRectangle1(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], &HMatchingImageReduced, lCropLTPointY, lCropLTPointX, lCropRBPointY, lCropRBPointX);

			Hlong lNoFoundNumber;
			HTuple Row, Column, Angle, Score;
			double dMatchingScore;
			dMatchingScore = (double)m_iEditPartCheckMatchingScore * 0.01;

			Row = HTuple(0.0);
			Column = HTuple(0.0);
			Angle = HTuple(0.0);
			Score = HTuple(0.0);

			HTuple NumLevels, AngleStart, AngleExtent, AngleStep, Metric;
			GetNccModelParams(m_pSelectedROI->m_HPartModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &Metric);

			int iNoTeachNumLevel = 0;
			iNoTeachNumLevel = NumLevels[0].L();
			if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_TEMPALTE)
				iNoTeachNumLevel = MAX_PYRAMID_LEVEL_TEMPALTE;
			else
				iNoTeachNumLevel = 0;

			FindNccModel(HMatchingImageReduced,
						 m_pSelectedROI->m_HPartModelID,				// Handle of the model
						 -dAngleRangeRad,							// Smallest rotation of the model
						 dAngleRangeRad * 2.0,							// Extent of the rotation angles.
						 0.1,										// Minumum score of the instances of the model to be found
						 1,											// Number of instances of the model to be found
						 0.5,										// Maximum overlap of the instances of the model to be found
						 "true",										// Subpixel accuracy
						 iNoTeachNumLevel,							// Number of pyramid levels used in the matching
						 &Row,										// Row coordinate of the found instances of the model.
						 &Column,									// Column coordinate of the found instances of the model.
						 &Angle,										// Rotation angle of the found instances of the model. (radian)
						 &Score);									// Score of the found instances of the model.

			HTuple HlNoFoundNumber;
			TupleLength(Score, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			if (lNoFoundNumber <= 0)
			{
				AfxMessageBox("매칭 실패.", MB_SYSTEMMODAL);
				return;
			}

			double dDeltaX, dDeltaY, dAngle, dScore;
			dDeltaX = Column[0].D();
			dDeltaY = Row[0].D();
			dAngle = Angle[0].D();
			dScore = Score[0].D();

			dDeltaX = dDeltaX + lCropLTPointX;
			dDeltaY = dDeltaY + lCropLTPointY;

			HTuple HomMat2DRotate;
			VectorAngleToRigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);

			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			HObject HResultXLD;
			AffineTransRegion(HInspectAreaRgn, &HResultXLD, HomMat2DRotate, "nearest_neighbor");
			GenContourRegionXld(HResultXLD, &HResultXLD, "border");

			if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
				ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			THEAPP.m_pInspectAdminViewDlg->UpdateView();

			double dTransX, dTransY, dRotationAngle;
			dTransX = Column[0].D() - dTeachAlignRefX;
			dTransY = Row[0].D() - dTeachAlignRefY;
			TupleDeg(Angle, &Angle);
			dRotationAngle = Angle[0].D();

			CString sMsg;
			sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), 회전각도(Deg)=(%.2lf), 매칭율(%%)=%d", (int)dTransX, (int)dTransY, dRotationAngle, (int)(dScore * 100.0));
			AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);

			m_bCheckPartCheckModelExist = TRUE;
			UpdateData(FALSE);
		}

		THEAPP.bTemplateModelSaveCheck[THEAPP.m_iCurTeachVision] = TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [AlgorithmPartCheckDlg::OnBnClickedButtonCreateMatchingModel] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}


void CAlgorithmPartCheckDlg::OnBnClickedButtonTestMatchingModel()
{
	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		if (m_pSelectedROI->m_HPartModelID < 0)
		{
			AfxMessageBox("매칭 모델이 생성되지 않았습니다. 먼저 매칭 모델을 생성해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		HObject HInspectAreaRgn;
		HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		// Model Ref: G-Center of Domain
		double dTeachAlignRefX, dTeachAlignRefY;
		Hlong lArea;
		HTuple HlArea, HdTeachAlignRefY, HdTeachAlignRefX;
		AreaCenter(HInspectAreaRgn, &HlArea, &HdTeachAlignRefY, &HdTeachAlignRefX);
		lArea = HlArea.L();
		dTeachAlignRefY = HdTeachAlignRefY.D();
		dTeachAlignRefX = HdTeachAlignRefX.D();

		// Test
		int iSearchMarginLeft = m_iEditPartCheckMatchingSearchMarginX;
		if (iSearchMarginLeft < 0)
			iSearchMarginLeft = 0;
		int iSearchMarginRight = m_iEditPartCheckMatchingSearchMarginX2;
		if (iSearchMarginRight < 0)
			iSearchMarginRight = 0;
		int iSearchMarginTop = m_iEditPartCheckMatchingSearchMarginY;
		if (iSearchMarginTop < 0)
			iSearchMarginTop = 0;
		int iSearchMarginBottom = m_iEditPartCheckMatchingSearchMarginY2;
		if (iSearchMarginBottom < 0)
			iSearchMarginBottom = 0;

		HObject HInspectAreaDilatedRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaDilatedRgn, "rectangle1");

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		SmallestRectangle1(HInspectAreaDilatedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
		lLTPointY = HlLTPointY.L();
		lLTPointX = HlLTPointX.L();
		lRBPointY = HlRBPointY.L();
		lRBPointX = HlRBPointX.L();

		GenRectangle1(&HInspectAreaDilatedRgn, lLTPointY - iSearchMarginTop, lLTPointX - iSearchMarginLeft, lRBPointY + iSearchMarginBottom, lRBPointX + iSearchMarginRight);

		HObject HMatchingImageReduced;
		ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HInspectAreaDilatedRgn, &HMatchingImageReduced);

		HTuple dAngleRangeRad;
		TupleRad(m_iEditPartCheckMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		Hlong lNoFoundNumber;
		HTuple Row, Column, Angle, Score;
		double dMatchingScore;
		dMatchingScore = (double)m_iEditPartCheckMatchingScore * 0.01;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Score = HTuple(0.0);

		HTuple NumLevels, AngleStart, AngleExtent, AngleStep, Metric;
		GetNccModelParams(m_pSelectedROI->m_HPartModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &Metric);

		int iNoTeachNumLevel = 0;
		iNoTeachNumLevel = NumLevels[0].L();
		if (iNoTeachNumLevel > MAX_PYRAMID_LEVEL_TEMPALTE)
			iNoTeachNumLevel = MAX_PYRAMID_LEVEL_TEMPALTE;
		else
			iNoTeachNumLevel = m_iEditPartCheckMatchingPLevel;

		FindNccModel(HMatchingImageReduced,
					 m_pSelectedROI->m_HPartModelID,				// Handle of the model
					 -dAngleRangeRad,							// Smallest rotation of the model
					 dAngleRangeRad * 2.0,							// Extent of the rotation angles.
					 0.5,										// Minumum score of the instances of the model to be found
					 1,											// Number of instances of the model to be found
					 0.5,										// Maximum overlap of the instances of the model to be found
					 "true",										// Subpixel accuracy
					 iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					 &Row,										// Row coordinate of the found instances of the model.
					 &Column,									// Column coordinate of the found instances of the model.
					 &Angle,										// Rotation angle of the found instances of the model. (radian)
					 &Score);									// Score of the found instances of the model.

		HTuple HlNoFoundNumber;
		TupleLength(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();

		if (lNoFoundNumber <= 0)
		{
			AfxMessageBox("매칭 실패", MB_SYSTEMMODAL);
			return;
		}

		double dDeltaX, dDeltaY, dAngle, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();

		if (dScore >= dMatchingScore)
		{
			HTuple HomMat2DRotate;
			VectorAngleToRigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);

			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			HObject HResultXLD;
			AffineTransRegion(HInspectAreaRgn, &HResultXLD, HomMat2DRotate, "nearest_neighbor");
			GenContourRegionXld(HResultXLD, &HResultXLD, "border");

			if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
				ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			THEAPP.m_pInspectAdminViewDlg->UpdateView();

			double dTransX, dTransY, dRotationAngle;
			dTransX = Column[0].D() - dTeachAlignRefX;
			dTransY = Row[0].D() - dTeachAlignRefY;
			TupleDeg(Angle, &Angle);
			dRotationAngle = Angle[0].D();

			CString sMsg;
			sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), 각도편차(Deg)=(%.3lf), 매칭율(%%)=%d", (int)dTransX, (int)dTransY, dRotationAngle, (int)(dScore * 100.0));
			AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);
		}
		else
		{
			GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
			THEAPP.m_pInspectAdminViewDlg->UpdateView();
			CString sMsg;
			sMsg.Format("매칭율(%%)=%d", (int)(dScore * 100.0));
			AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);
		}

	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [AlgorithmPartCheckDlg::OnBnClickedButtonTestMatchingModel] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}

void CAlgorithmPartCheckDlg::OnBnClickedButtonDeleteMatchingModel()
{
	if (AfxMessageBox("현재 매칭 모델을 삭제하시겠습니까?", MB_OKCANCEL) == IDOK)
	{
		HTuple HNoTemplate;
		Hlong lNoTemplate;

		TupleLength(m_pSelectedROI->m_HPartModelID, &HNoTemplate);
		lNoTemplate = HNoTemplate.L();

		for (int iIndex = 0; iIndex < lNoTemplate; iIndex++)
		{
			if (m_pSelectedROI->m_HPartModelID[iIndex] >= 0)
			{
				ClearNccModel(m_pSelectedROI->m_HPartModelID[iIndex]);
				m_pSelectedROI->m_HPartModelID[iIndex] = -1;
			}
		}

		m_bCheckPartCheckModelExist = FALSE;
		UpdateData(FALSE);
	}
}

void CAlgorithmPartCheckDlg::OnBnClickedButtonTestMatchingMultiModule()
{
	try
	{
		BOOL bDebugSave = FALSE;

		DWORD dTStart = 0, dTEnd = 0;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType]) == FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		if (m_pSelectedROI->m_HPartModelID < 0)
		{
			AfxMessageBox("매칭 모델이 생성되지 않았습니다. 먼저 매칭 모델을 생성해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		HTuple HNoTemplate;
		Hlong lNoTemplate;
		HTuple HSelectedTemplate;

		TupleLength(m_pSelectedROI->m_HPartModelID, &HNoTemplate);
		lNoTemplate = HNoTemplate.L();

		if (m_bCheckPartCheckMultipleModel)
		{
			if (lNoTemplate <= 1)
			{
				AfxMessageBox("[멀티 모듈 각각 모델 생성 적용] 옵션이 On 될 경우 매칭 모델 개수가 2개 이상이어야 합니다.", MB_SYSTEMMODAL);
				return;
			}
		}
		else
		{
			if (lNoTemplate > 1)
			{
				AfxMessageBox("[멀티 모듈 각각 모델 생성 적용] 옵션이 Off 될 경우 매칭 모델 개수가 1개이어야 합니다.", MB_SYSTEMMODAL);
				return;
			}
		}

		HObject HInspectAreaRgn;
		HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		// Model Ref: G-Center of Domain
		double dTeachAlignRefX, dTeachAlignRefY;
		Hlong lArea;
		HTuple HlArea, HdTeachAlignRefY, HdTeachAlignRefX;
		AreaCenter(HInspectAreaRgn, &HlArea, &HdTeachAlignRefY, &HdTeachAlignRefX);
		lArea = HlArea.L();
		dTeachAlignRefY = HdTeachAlignRefY.D();
		dTeachAlignRefX = HdTeachAlignRefX.D();

		// Test
		int iSearchMarginLeft = m_iEditPartCheckMatchingSearchMarginX;
		if (iSearchMarginLeft < 0)
			iSearchMarginLeft = 0;
		int iSearchMarginRight = m_iEditPartCheckMatchingSearchMarginX2;
		if (iSearchMarginRight < 0)
			iSearchMarginRight = 0;
		int iSearchMarginTop = m_iEditPartCheckMatchingSearchMarginY;
		if (iSearchMarginTop < 0)
			iSearchMarginTop = 0;
		int iSearchMarginBottom = m_iEditPartCheckMatchingSearchMarginY2;
		if (iSearchMarginBottom < 0)
			iSearchMarginBottom = 0;

		HObject HInspectAreaDilatedRgn;
		ShapeTrans(HInspectAreaRgn, &HInspectAreaDilatedRgn, "rectangle1");

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		SmallestRectangle1(HInspectAreaDilatedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
		lLTPointY = HlLTPointY.L();
		lLTPointX = HlLTPointX.L();
		lRBPointY = HlRBPointY.L();
		lRBPointX = HlRBPointX.L();

		GenRectangle1(&HInspectAreaDilatedRgn, lLTPointY - iSearchMarginTop, lLTPointX - iSearchMarginLeft, lRBPointY + iSearchMarginBottom, lRBPointX + iSearchMarginRight);

		HTuple dAngleRangeRad;
		TupleRad(m_iEditPartCheckMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad *= -1.0;

		int iNoTeachNumLevel = 0;
		iNoTeachNumLevel = m_iEditPartCheckMatchingPLevel;

		double dMatchingScore;
		dMatchingScore = (double)m_iEditPartCheckMatchingScore * 0.01;

		HObject HMatchingImageReduced;

		Hlong lNoFoundNumber;
		HTuple Row, Column, Angle, Score;
		int i, j, iMoveX, iMoveY;
		HObject HModuleSearchRgn;
		Hlong lCropLTPointY, lCropLTPointX;
		double dModuleAlignRefX, dModuleAlignRefY;

		GenEmptyObj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		int iMatchCount = 0;
		int iDirectionAxis;

		dTStart = GetTickCount();

		for (int ij = 0; ij < m_iEditPartCheckMultiModuleYNumber * m_iEditPartCheckMultiModuleXNumber; ij++)
		{
			// Tray 안착 순서에 따라서 옵션을 다르게 처리해야함
			// if (y 방향으로 먼저 진행한다면)
			{
				iDirectionAxis = m_iEditPartCheckMultiModuleYNumber;
				i = ij % iDirectionAxis;
				j = ij / iDirectionAxis;
			}
			// else if (x 방향으로 먼저 진행한다면)
			// {
			//	iDirectionAxis = m_iEditPartCheckMultiModuleXNumber;
			//	i = ij / iDirectionAxis;
			//	j = ij % iDirectionAxis;
			// }

			dModuleAlignRefX = dTeachAlignRefX + j * m_iEditPartCheckMultiModuleXPitch;
			dModuleAlignRefY = dTeachAlignRefY + i * m_iEditPartCheckMultiModuleYPitch;

			iMoveX = j * m_iEditPartCheckMultiModuleXPitch;
			iMoveY = i * m_iEditPartCheckMultiModuleYPitch;

			if (m_bCheckPartCheckMultiModuleMoveOffset)
			{
				iMoveX = TempAlgoParam.m_iPartCheckMoveOffsetX[i][j];
				iMoveY = TempAlgoParam.m_iPartCheckMoveOffsetY[i][j];
			}
			else
			{
				iMoveX = j * m_iEditPartCheckMultiModuleXPitch;
				iMoveY = i * m_iEditPartCheckMultiModuleYPitch;
			}

			// if (이미지 기준 시작 모듈이 좌 하단 (BOI))
			{
				iMoveX = iMoveX * 1;
				iMoveY = iMoveY * -1;
			}
			// else if (이미지 기준 시작 모듈이 좌 상단)
			// {
			//	iMoveX = iMoveX * 1;
			//	iMoveY = iMoveY * 1;
			// }
			// else if (이미지 기준 시작 모듈이 우 하단)
			// {
			//	iMoveX = iMoveX * 1;
			//	iMoveY = iMoveY * 1;
			// }
			// else if (이미지 기준 시작 모듈이 우 상단)
			// {
			//	iMoveX = iMoveX * 1;
			//	iMoveY = iMoveY * 1;
			// }

			dModuleAlignRefX = dTeachAlignRefX + iMoveX;
			dModuleAlignRefY = dTeachAlignRefY + iMoveY;

			MoveRegion(HInspectAreaDilatedRgn, &HModuleSearchRgn, iMoveY, iMoveX);

			ReduceDomain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pInspectAdminViewDlg->GetDisplayChannelType()][m_iSelectedImageType], HModuleSearchRgn, &HMatchingImageReduced);

			CropDomain(HMatchingImageReduced, &HMatchingImageReduced);

			if (bDebugSave)
			{
				CString sFileName;
				sFileName.Format("c:\\DualTest\\LoadingAlignVision_Module_X%d_Y%d", j + 1, i + 1);
				WriteImage(HMatchingImageReduced, "jpg", 0, HTuple(sFileName));
			}

			SmallestRectangle1(HModuleSearchRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
			lCropLTPointY = HlLTPointY.L();
			lCropLTPointX = HlLTPointX.L();

			Row = HTuple(0.0);
			Column = HTuple(0.0);
			Angle = HTuple(0.0);
			Score = HTuple(0.0);

			if (m_bCheckPartCheckMultipleModel)
			{
				FindNccModel(HMatchingImageReduced,
							 m_pSelectedROI->m_HPartModelID[ij],		// Handle of the model
							 -dAngleRangeRad,							// Smallest rotation of the model
							 dAngleRangeRad * 2.0,							// Extent of the rotation angles.
							 dMatchingScore,								// Minumum score of the instances of the model to be found
							 1,											// Number of instances of the model to be found
							 0.5,										// Maximum overlap of the instances of the model to be found
							 "true",										// Subpixel accuracy
							 iNoTeachNumLevel,							// Number of pyramid levels used in the matching
							 &Row,										// Row coordinate of the found instances of the model.
							 &Column,									// Column coordinate of the found instances of the model.
							 &Angle,										// Rotation angle of the found instances of the model. (radian)
							 &Score);									// Score of the found instances of the model.
			}
			else
			{
				FindNccModel(HMatchingImageReduced,
							 m_pSelectedROI->m_HPartModelID,				// Handle of the model
							 -dAngleRangeRad,							// Smallest rotation of the model
							 dAngleRangeRad * 2.0,							// Extent of the rotation angles.
							 dMatchingScore,								// Minumum score of the instances of the model to be found
							 1,											// Number of instances of the model to be found
							 0.5,										// Maximum overlap of the instances of the model to be found
							 "true",										// Subpixel accuracy
							 iNoTeachNumLevel,							// Number of pyramid levels used in the matching
							 &Row,										// Row coordinate of the found instances of the model.
							 &Column,									// Column coordinate of the found instances of the model.
							 &Angle,										// Rotation angle of the found instances of the model. (radian)
							 &Score);									// Score of the found instances of the model.
			}

			HTuple HlNoFoundNumber;
			TupleLength(Score, &HlNoFoundNumber);
			lNoFoundNumber = HlNoFoundNumber.L();

			if (lNoFoundNumber <= 0)
			{
				continue;
			}
			else
			{
				double dDeltaX, dDeltaY, dAngle, dScore;
				dDeltaX = Column[0].D();
				dDeltaY = Row[0].D();
				dAngle = Angle[0].D();
				dScore = Score[0].D();

				if (m_bCheckPartCheckInspectExistence)
				{
					if (dScore >= dMatchingScore)
					{
						++iMatchCount;

						dDeltaX = dDeltaX + lCropLTPointX;
						dDeltaY = dDeltaY + lCropLTPointY;

						HTuple HomMat2DRotate;
						VectorAngleToRigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);

						HObject HResultXLD;
						AffineTransRegion(HInspectAreaRgn, &HResultXLD, HomMat2DRotate, "nearest_neighbor");
						GenContourRegionXld(HResultXLD, &HResultXLD, "border");

						if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
							ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
					}
				}
				else
				{
					++iMatchCount;

					dDeltaX = dDeltaX + lCropLTPointX;
					dDeltaY = dDeltaY + lCropLTPointY;

					HTuple HomMat2DRotate;
					VectorAngleToRigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);

					HObject HResultXLD;
					AffineTransRegion(HInspectAreaRgn, &HResultXLD, HomMat2DRotate, "nearest_neighbor");
					GenContourRegionXld(HResultXLD, &HResultXLD, "border");

					if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
						ConcatObj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
				}
			}
		}

		dTEnd = GetTickCount();

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		CString sMsg;
		sMsg.Format("매칭 성공 %d 회, 처리시간 %d ms", iMatchCount, dTEnd - dTStart);
		AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [AlgorithmPartCheckDlg::OnBnClickedButtonTestMatchingMultiModule] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));
	}
}

#include "MultiModuleMoveOffsetDlg.h"

void CAlgorithmPartCheckDlg::OnBnClickedButtonMultiModuleMoveOffset()
{
	CMultiModuleMoveOffsetDlg dlg;

	dlg.Param = TempAlgoParam;

	if (dlg.DoModal() == IDOK)
	{
		TempAlgoParam = dlg.Param;
	}
}
