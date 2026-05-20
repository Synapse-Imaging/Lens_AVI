#include "stdafx.h"
#include "ModelDataManager.h"
#include "uScan.h"
#include "IniFileCS.h"

CModelDataManager* CModelDataManager::m_pInstance = NULL;

CModelDataManager* CModelDataManager::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CModelDataManager();
		m_pInstance->InitModelData();
	}
	return m_pInstance;
}

void CModelDataManager::DeleteInstance()
{
	SAFE_DELETE(m_pInstance);
}

CModelDataManager::CModelDataManager(void)
{
	SetFilePtr(&m_File);

	m_sModelName = ".";
	m_iModelVersion = 1000;
	m_iTrayArrayX = 0;
	m_iTrayArrayY = 0;
	m_iTrayModuleAmt = 0;
	m_iTrayModuleMax = 0;

	m_iModelIdx = 0;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_bUseLocalAlignMatching[i] = FALSE;
		m_lLAlignModelID[i] = -1;

		m_bUseDiffusedImage[i] = FALSE;
		m_bUseSpecularImage[i] = FALSE;
		m_bUseSeqStartImage[i] = FALSE;
		for (int j = 0; j < STAGE_NUMBER_MAX; j++)
			m_iSeqAddrIndex[j][i] = 0;

		m_bUseSpecularRoiCrop[i] = FALSE;
		m_dSpecularResizeFactor[i] = 1.0;

		m_lSpecularCropLTPointX_Roi[i] = 0;
		m_lSpecularCropLTPointY_Roi[i] = 0;
		m_lSpecularCropRBPointX_Roi[i] = 5120;
		m_lSpecularCropRBPointY_Roi[i] = 5120;
	}

	m_iDFMLightIntensity1 = 10;
	m_iDFMLightIntensity2 = 10;
	m_dDFMPsx = 0.0;
	m_dDFMPsy = 40.0;
	m_dDFMTex = 0.0;
	m_dDFMTey = 0.0;

	m_dDFMSpecularImageSF[0] = 4.0;

	m_iDFMMeanFilterSize = 4;

	for (int i = 0; i < DFM_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		m_dDFMHigh[i] = 5.0;
		m_dDFMLow[i] = -5.0;
		m_dDFMDerivativeSmFactor[i] = 5.0;
	}

	for (int i = 0; i < DIFFUSED_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		m_dDiffusedHigh[i] = 2.5;
		m_dDiffusedLow[i] = -1.07;
		m_dDiffusedSmFactor[i] = 14.5;
	}

	for (int i = 0; i < STAGE_NUMBER_MAX; i++)
	{
		m_dZigCenterCamZ[i] = MOTION_NOUSE;
		m_bUseZigCenterPos[i] = FALSE;
		m_iAfZigCenterX[i] = 2560;
		m_iAfZigCenterY[i] = 2560;
		m_iAFZigCenterLightPage[i] = 0;
		m_dAfZigRotationCenterX[i] = -1;
		m_dAfZigRotationCenterY[i] = -1;
		m_iAfLeftTopCornerPointX[i] = -1;
		m_iAfLeftTopCornerPointY[i] = -1;
	}

	m_iAfLensStartImageNo = 0;
	m_iAfLensEndImageNo = 0;
	m_iAfModuleRotationAngleImageNo = 0;
	m_iAfEastSideStartImageNo = 0;
	m_iAfEastSideEndImageNo = 0;
	m_iAfWestSideStartImageNo = 0;
	m_iAfWestSideEndImageNo = 0;
	m_iAfNorthSideStartImageNo = 0;
	m_iAfNorthSideEndImageNo = 0;
	m_iAfSouthSideStartImageNo = 0;
	m_iAfSouthSideEndImageNo = 0;

	m_bMUseFai = FALSE;
	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		m_bMInspectFai[i] = FALSE;
		m_dMNominalFai[i] = 1;
		m_dMToleranceMaxFai[i] = 0.1;
		m_dMToleranceMinFai[i] = -0.1;
		m_dMOffsetStg1Fai[i] = 0;
		m_dMTiltStg1Fai[i] = 0;
		m_dMMultipleStg1Fai[i] = 1;	// 2025.04.08 - v2011 - LeeGW - CM AKC FAI
		m_dMOffsetStg2Fai[i] = 0;
		m_dMTiltStg2Fai[i] = 0;
		m_dMMultipleStg2Fai[i] = 1;	// 2025.04.08 - v2011 - LeeGW - CM AKC FAI
		m_iTransferCodeIndexFai[i] = 0;
		m_bMultiFai[i] = FALSE;
	}

	m_pInspectionArea = NULL;
	m_pInspectionArea = new GTRegion;
	m_pInspectionArea->SetVisible(FALSE, FALSE);
	m_pInspectionArea->SetSelectable(FALSE, FALSE);
	m_pInspectionArea->SetMovable(FALSE, FALSE);
	m_pInspectionArea->SetSizable(FALSE, FALSE);

	//2017.04.11 hbk
	m_lOCRHandle = -1;
	m_strFontName = _T("");

	m_iLightValueStart = 10;
	m_iLightValueEnd = 10;
	m_iLightValueInterval = 2;
	m_iLightValueInTol = 10;
	m_iTotalImageValueTol = 5;

	m_dFocusValueStart = 0.5;
	m_dFocusValueEnd = 0.5;
	m_dFocusValueInterval = 0.1;
	m_iSelectFocusTol = 0;
	m_dFocusZValueTol = 0.5;
	m_dFocusEdgeValueTol = 0.7;
}

CModelDataManager::~CModelDataManager(void)
{
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (m_lLAlignModelID[i] >= 0)
		{
			if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_TEMPLATE)
				ClearNccModel(m_lLAlignModelID[i]);
			else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_SHAPE)
				ClearShapeModel(m_lLAlignModelID[i]);
			else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_PERSPECTIVE)
				ClearDeformableModel(m_lLAlignModelID[i]);

			m_lLAlignModelID[i] = -1;
		}
	}

	SAFE_DELETE(m_pInspectionArea);
}

void CModelDataManager::InitModelData()
{
	m_sModelName = ".";
	m_iModelVersion = 1000;
	m_iTrayArrayX = 0;
	m_iTrayArrayY = 0;
	m_iTrayModuleAmt = 0;
	m_iTrayModuleMax = 0;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_bUseDiffusedImage[i] = FALSE;
		m_bUseSpecularImage[i] = FALSE;
		m_bUseSeqStartImage[i] = FALSE;
		for (int j = 0; j < STAGE_NUMBER_MAX; j++)
			m_iSeqAddrIndex[j][i] = 0;

		m_bUseLocalAlignMatching[i] = FALSE;
		m_iLocalMatchingTeachingRectLTX[i] = 0;
		m_iLocalMatchingTeachingRectLTY[i] = 0;
		m_iLocalMatchingTeachingRectRBX[i] = 0;
		m_iLocalMatchingTeachingRectRBY[i] = 0;

		m_iLocalMatchingPyramidLevel[i] = 0;
		m_iLocalMatchingAngleRange[i] = 5.0;
		m_dLocalMatchingScaleMin[i] = 0.98;
		m_dLocalMatchingScaleMax[i] = 1.02;
		m_iLocalMatchingTeachingContrast[i] = 40;
		m_iLocalMatchingTeachingContrastLow[i] = 40;
		m_iLocalMatchingTeachingContrastMinSize[i] = 1;
		m_dLocalMatchingScore[i] = 0.3;
		m_iLocalMatchingInspectionMinContrast[i] = 15;
		m_iLocalMatchingSearchMarginX[i] = 300;
		m_iLocalMatchingSearchMarginY[i] = 300;
		m_dLocalTeachAlignRefX[i] = -1;
		m_dLocalTeachAlignRefY[i] = -1;
		m_bUseMatchingAffineConstant[i] = FALSE;

		for (int j = 0; j < MAX_IMAGE_TAB; j++)
			m_bLocalAlignImage[i][j] = FALSE;

		m_iLocalMatchingProcessChType[i] = CHANNEL_TYPE_I;

		m_bUseMatchingFilter[i] = FALSE;

		m_iMatchingFilterType[i] = FILTER_TYPE_NOT_USED;
		m_dMatchingFilterTypeXSize[i] = 7;
		m_dMatchingFilterTypeYSize[i] = 7;

		m_bUseMatchingAngleComp[i] = FALSE;
		m_iMatchingAngleRefLine[i] = MATCHING_ANGLE_REF_LINE_X;

		m_bCheckUseMatchingXYComp[i] = FALSE;
		m_iLocalIndexNumber[i] = 0;
		m_iRadioMatchingXYRefLine[i] = MATCHING_ANGLE_REF_LINE_X;

		if (m_lLAlignModelID[i] >= 0)
		{
			if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_TEMPLATE)
				ClearNccModel(m_lLAlignModelID[i]);
			else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_SHAPE)
				ClearShapeModel(m_lLAlignModelID[i]);
			else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_PERSPECTIVE)
				ClearDeformableModel(m_lLAlignModelID[i]);

			m_lLAlignModelID[i] = -1;
		}

		m_bUseSpecularRoiCrop[i] = FALSE;
		m_dSpecularResizeFactor[i] = 1.0;

		m_lSpecularCropLTPointX_Roi[i] = 0;
		m_lSpecularCropLTPointY_Roi[i] = 0;
		m_lSpecularCropRBPointX_Roi[i] = 5120;
		m_lSpecularCropRBPointY_Roi[i] = 5120;
	}

	m_dDFMSpecularImageSF[0] = 4.0;

	m_iDFMMeanFilterSize = 4;

	for (int i = 0; i < DFM_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		m_dDFMHigh[i] = 5.0;
		m_dDFMLow[i] = -5.0;
		m_dDFMDerivativeSmFactor[i] = 5.0;
	}

	for (int i = 0; i < DIFFUSED_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		m_dDiffusedHigh[i] = 2.5;
		m_dDiffusedLow[i] = -1.07;
		m_dDiffusedSmFactor[i] = 14.5;
	}

	for (int i = 0; i < STAGE_NUMBER_MAX; i++)
	{
		m_dZigCenterCamZ[i] = 57.6;
		m_bUseZigCenterPos[i] = FALSE;
		m_iAfZigCenterX[i] = 2560;
		m_iAfZigCenterY[i] = 2560;
		m_iAFZigCenterLightPage[i] = 0;
		m_dAfZigRotationCenterX[i] = -1;
		m_dAfZigRotationCenterY[i] = -1;
		m_iAfLeftTopCornerPointX[i] = -1;
		m_iAfLeftTopCornerPointY[i] = -1;
	}
	m_iAfLensStartImageNo = 1;
	m_iAfLensEndImageNo = 2;
	m_iAfModuleRotationAngleImageNo = 3;
	m_iAfEastSideStartImageNo = -1;
	m_iAfEastSideEndImageNo = -1;
	m_iAfWestSideStartImageNo = -1;
	m_iAfWestSideEndImageNo = -1;
	m_iAfNorthSideStartImageNo = -1;
	m_iAfNorthSideEndImageNo = -1;
	m_iAfSouthSideStartImageNo = -1;
	m_iAfSouthSideEndImageNo = -1;

	InitLightInfo();
	InitLightAverageValue();	//WCS

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_dTeachingFocusValue[i] = 0;
	}

	m_iNoUsedImageGrab = 0;
	for (int i = 0; i < STAGE_NUMBER_MAX; i++)
	{
		m_dLightZPositionRef[i] = MOTION_NOUSE;
		m_dTiltPositionRef[i] = MOTION_NOUSE;
		m_dRotatePositionRef[i] = MOTION_NOUSE;
		m_dStageXPositionRef[i] = MOTION_NOUSE;
		m_dStageYPositionRef[i] = MOTION_NOUSE;
	}

	m_iTriggerImageNumber = 4;
	m_iTriggerPeriod = 25;

	m_iBottomAlign1EndImageNumber = 3;
	m_iBottomAlign2EndImageNumber = 6;
	m_iTopAlignEndImageNumber = 8;

	m_iLightValueStart = 10;
	m_iLightValueEnd = 10;
	m_iLightValueInterval = 2;
	m_iLightValueInTol = 10;
	m_iTotalImageValueTol = 5;

	m_dFocusValueStart = 0.5;
	m_dFocusValueEnd = 0.5;
	m_dFocusValueInterval = 0.1;
	m_iSelectFocusTol = 0;
	m_dFocusZValueTol = 0.5;
	m_dFocusEdgeValueTol = 0.7;

	m_bMUseFai = FALSE;
	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		m_bMInspectFai[i] = FALSE;
		m_dMNominalFai[i] = 1;
		m_dMToleranceMaxFai[i] = 0.1;
		m_dMToleranceMinFai[i] = -0.1;
		m_dMOffsetStg1Fai[i] = 0;
		m_dMTiltStg1Fai[i] = 0;
		m_dMMultipleStg1Fai[i] = 1;	// 2025.04.08 - v2011 - LeeGW - CM AKC FAI
		m_dMOffsetStg2Fai[i] = 0;
		m_dMTiltStg2Fai[i] = 0;
		m_dMMultipleStg2Fai[i] = 1;	// 2025.04.08 - v2011 - LeeGW - CM AKC FAI
		m_iTransferCodeIndexFai[i] = 0;
		m_bMultiFai[i] = FALSE;
	}
}

void CModelDataManager::SaveModelData()
{
	if (m_sModelName == ".")
		return;

	THEAPP.m_StructVariationParameter.Reset();
	THEAPP.m_StructDefectCondition.Reset();

	////////////////////////////// Update parameter-variables //////////////////////////////

	THEAPP.m_pTabControlDlg->m_pTeachParamDlg->GetInspectParam(TRUE);
	THEAPP.DoEvents();

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	CString strSwModelFolder;
	strSwModelFolder.Format("%s\\SW\\Vision_N%d", strModelFolder, m_iModelIdx + 1);

	SetFolderFile(strSwModelFolder, "InspectParam.dat");

	if (!m_bOpened)
		OpenDataFile(CFile::modeCreate | CFile::modeWrite);

	if (m_bOpened)
	{
		HTuple HFileHandle = -1;
		CString szTemplateModelFilename;
		szTemplateModelFilename = strSwModelFolder + "\\InspectTemplate.dat";

		OpenFile(HTuple(szTemplateModelFilename), "output_binary", &HFileHandle);

		CString sModelType;
		for (int j = 0; j <= MODEL_TYPE_MAX; j++)
		{
			if (j == MODEL_TYPE_MAX)
			{
				sModelType = "ETC";
				break;
			}
			else if (m_sModelName.Find(THEAPP.g_strModelTypeName[j]) >= 0)
			{
				sModelType = THEAPP.g_strModelTypeName[j];
				break;
			}
		}

		CString strConfig = m_sModelName.Mid(m_sModelName.Find('_') + 1);
		strConfig = strConfig.Mid(strConfig.Find('_') + 1);
		strConfig = strConfig.Mid(strConfig.Find('_') + 1);
		strConfig = strConfig.Mid(0, strConfig.Find('_'));

		m_pInspectionArea->Save((HANDLE)m_File.m_hFile, HFileHandle, THEAPP.bVariationModelSaveCheck[m_iModelIdx], THEAPP.bTemplateModelSaveCheck[m_iModelIdx], m_iModelIdx, strConfig);

		if (HFileHandle != -1)
			CloseFile(HFileHandle);

		CloseDataFile();

		// Variation Paramter Json 파일 저장
		THEAPP.ClearVariationParameter(m_iModelIdx, sModelType);
		THEAPP.SaveVariationParameter(m_iModelIdx, sModelType);

		// Defect Condition Json 파일 저장
		THEAPP.ClearDefectCondition(m_iModelIdx, sModelType);
		THEAPP.SaveDefectCondition(m_iModelIdx, sModelType);
	}

	THEAPP.DoEvents(100);

	// Model SW Version 저장
	CIniFileCS INI(strSwModelFolder + "\\ModelBase.ini");
	CString strSection = "Model Base";

	INI.Set_Integer(strSection, "VERSION", THEAPP.GetProgramVersion());
	m_iModelVersion = THEAPP.GetProgramVersion();

	//2017.04.11 hbk
	strSection = "Ocr FontName";
	INI.Set_String(strSection, "m_strFontName", m_strFontName);
}

void CModelDataManager::InitLightInfo()
{
	for (int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
	{
		for (int j = 0; j < LIGHT_CH_CNT; j++)
			m_PageControlData.m_Page[i].uiChannel[j] = 0;
	}

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		m_iLightPageIdx[i] = 0;

	for (int i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		m_PageControlData.m_iAddrCount[i] = LIGHTCTRL_GRAB_ADDR_PAGE_COUNT;
		m_PageControlData.m_sAddrDesc[i] = _T("");

		for (int j = 0; j < LIGHTCTRL_PAGE_COUNT; j++)
			m_PageControlData.m_iAddrSeq[i][j] = j;

		m_PageControlData.m_iWSSDelayTime[i] = 0;
	}
}

// 1개 비전 모든 조명 정보
int CModelDataManager::LoadLightInfo(CString sLinfoPath)
{
	CIniFileCS INIInspectionLight(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	int iInspectLightInfoVer;
	iInspectLightInfoVer = INIInspectionLight.Get_Integer(strSection, strKey, 1001);

	strSection.Format("ComPort");
	strKey.Format("PortNumber");
	m_PageControlData.m_iComPortNumber = INIInspectionLight.Get_Integer(strSection, strKey, 1);

	strSection.Format("SubComPort");
	strKey.Format("PortNumber");
	m_PageControlData.m_iSubComPortNumber = INIInspectionLight.Get_Integer(strSection, strKey, -1);

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		strSection.Format("Image_%d", i + 1);
		if (iInspectLightInfoVer == 1001)
		{
			strKey.Format("PageIndex");
			m_iLightPageIdx[i] = INIInspectionLight.Get_Integer(strSection, strKey, 0);
		}
		else
		{
			strKey.Format("PageNo");
			m_iLightPageIdx[i] = INIInspectionLight.Get_Integer(strSection, strKey, 1) - 1;
		}
	}

	for (int iPageIdx = 0; iPageIdx < LIGHTCTRL_PAGE_COUNT; iPageIdx++)
	{
		strSection.Format("Page_%d", iPageIdx + 1);
		strKey.Format("Desc");
		m_PageControlData.m_Page[iPageIdx].m_sPageDesc = INIInspectionLight.Get_String(strSection, strKey, "");

		for (int iChIdx = 0; iChIdx < LIGHT_CH_CNT; iChIdx++)
		{
			strKey.Format("Ch%d", iChIdx + 1);
			m_PageControlData.m_Page[iPageIdx].uiChannel[iChIdx] = INIInspectionLight.Get_Integer(strSection, strKey, 0);
		}
	}

	if (iInspectLightInfoVer == 1001)
	{
		std::remove(sLinfoPath);
		SaveLightInfo(sLinfoPath, -1);
	}

	return 1;
}

void CModelDataManager::SaveLightInfo(CString sLinfoPath, int iCurPageIndex)
{
	CIniFileCS INIInspectionLight(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	INIInspectionLight.Set_Integer(strSection, strKey, 1002);

	strSection.Format("ComPort");
	strKey.Format("PortNumber");
	INIInspectionLight.Set_Integer(strSection, strKey, m_PageControlData.m_iComPortNumber);

#ifdef ASSY_LENS
	strSection.Format("SubComPort");
	strKey.Format("PortNumber");
	INIInspectionLight.Set_Integer(strSection, strKey, m_PageControlData.m_iSubComPortNumber);
#endif

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		strSection.Format("Image_%d", i + 1);
		strKey.Format("PageNo");
		INIInspectionLight.Set_Integer(strSection, strKey, m_iLightPageIdx[i] + 1);
	}

	for (int iPageIdx = 0; iPageIdx < LIGHTCTRL_PAGE_COUNT; iPageIdx++)
	{
		if (iPageIdx != iCurPageIndex && iCurPageIndex >= 0)
			continue;

		strSection.Format("Page_%d", iPageIdx + 1);
		strKey.Format("Desc");
		INIInspectionLight.Set_String(strSection, strKey, m_PageControlData.m_Page[iPageIdx].m_sPageDesc);

		for (int iChIdx = 0; iChIdx < LIGHT_CH_CNT; iChIdx++)
		{
			strKey.Format("Ch%d", iChIdx + 1);
			INIInspectionLight.Set_Integer(strSection, strKey, m_PageControlData.m_Page[iPageIdx].uiChannel[iChIdx]);
		}
	}
}

int CModelDataManager::LoadSequenceInfo(CString sLinfoPath)
{
	CIniFileCS INISequence(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	int iSequenceInfoVer;
	iSequenceInfoVer = INISequence.Get_Integer(strSection, strKey, 1001);

	for (int i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		if (iSequenceInfoVer == 1001)
			strSection.Format("Grab_Sequence_Addr_%d", i);
		else
			strSection.Format("Grab_Sequence_Addr_%d", i + 1);
		strKey.Format("Seq_Count");
		m_PageControlData.m_iAddrCount[i] = INISequence.Get_Integer(strSection, strKey, LIGHTCTRL_GRAB_ADDR_PAGE_COUNT);

		strKey.Format("Seq_Desc");
		m_PageControlData.m_sAddrDesc[i] = INISequence.Get_String(strSection, strKey, "");

		for (int j = 0; j < LIGHTCTRL_PAGE_COUNT; j++)
		{
			if (iSequenceInfoVer == 1001)
			{
				strKey.Format("Seq_%d", j);
				m_PageControlData.m_iAddrSeq[i][j] = INISequence.Get_Integer(strSection, strKey, j);
			}
			else
			{
				strKey.Format("Seq_%d", j + 1);
				m_PageControlData.m_iAddrSeq[i][j] = INISequence.Get_Integer(strSection, strKey, j + 1) - 1;
			}
		}

		strKey.Format("WSS_DelayTime");
		m_PageControlData.m_iWSSDelayTime[i] = INISequence.Get_Integer(strSection, strKey, 0);
	}

	if (iSequenceInfoVer == 1001)
	{
		std::remove(sLinfoPath);
		SaveSequenceInfo(sLinfoPath);
	}

	return 1;
}

void CModelDataManager::SaveSequenceInfo(CString sLinfoPath)
{
	CIniFileCS INISequence(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	INISequence.Set_Integer(strSection, strKey, 1002);

	for (int i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		strSection.Format("Grab_Sequence_Addr_%d", i + 1);
		strKey.Format("Seq_Count");
		INISequence.Set_Integer(strSection, strKey, m_PageControlData.m_iAddrCount[i]);

		strKey.Format("Seq_Desc");
		INISequence.Set_String(strSection, strKey, m_PageControlData.m_sAddrDesc[i]);

		for (int j = 0; j < LIGHTCTRL_PAGE_COUNT; j++)
		{
			strKey.Format("Seq_%d", j + 1);
			INISequence.Set_Integer(strSection, strKey, m_PageControlData.m_iAddrSeq[i][j] + 1);
		}
	}
}

int CModelDataManager::LoadPwmLightInfo(CString sLinfoPath)
{
	CIniFileCS INIInspectionLight(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	int iInspectLightInfoVer;
	iInspectLightInfoVer = INIInspectionLight.Get_Integer(strSection, strKey, 1001);

	strSection.Format("ComPort");
	strKey.Format("PortNumber");
	m_PageControlData.m_iComPortNumber = INIInspectionLight.Get_Integer(strSection, strKey, 1);

	for (int i = 0; i < ALIGN_CAM_GRAB_NUMBER; i++)
	{
		strSection.Format("Image_%d", i + 1);
		if (iInspectLightInfoVer == 1001)
		{
			strKey.Format("PageIndex");
			m_iLightPageIdx[i] = INIInspectionLight.Get_Integer(strSection, strKey, 0);
		}
		else
		{
			strKey.Format("PageNo");
			m_iLightPageIdx[i] = INIInspectionLight.Get_Integer(strSection, strKey, 1) - 1;
		}
	}

	for (int iPageIdx = 0; iPageIdx < PWM_PAGE_COUNT; iPageIdx++)
	{
		strSection.Format("Page_%d", iPageIdx + 1);
		for (int iChIdx = 0; iChIdx < PWM_CH_CNT; iChIdx++)
		{
			strKey.Format("Ch%d", iChIdx + 1);
			m_PageControlData.m_Page[iPageIdx].uiChannel[iChIdx] = INIInspectionLight.Get_Integer(strSection, strKey, 0);
		}
	}

	if (iInspectLightInfoVer == 1001)
	{
		std::remove(sLinfoPath);
		SavePwmLightInfo(sLinfoPath);
	}

	return 1;
}

void CModelDataManager::SavePwmLightInfo(CString sLinfoPath)
{
	CIniFileCS INIInspectionLight(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	INIInspectionLight.Set_Integer(strSection, strKey, 1002);

	strSection.Format("ComPort");
	strKey.Format("PortNumber");
	INIInspectionLight.Set_Integer(strSection, strKey, m_PageControlData.m_iComPortNumber);

	for (int i = 0; i < ALIGN_CAM_GRAB_NUMBER; i++)
	{
		strSection.Format("Image_%d", i + 1);
		strKey.Format("PageNo");
		INIInspectionLight.Set_Integer(strSection, strKey, 1);
	}

	for (int iPageIdx = 0; iPageIdx < PWM_PAGE_COUNT; iPageIdx++)
	{
		strSection.Format("Page_%d", iPageIdx + 1);
		for (int iChIdx = 0; iChIdx < PWM_CH_CNT; iChIdx++)
		{
			strKey.Format("Ch%d", iChIdx + 1);
			INIInspectionLight.Set_Integer(strSection, strKey, m_PageControlData.m_Page[iPageIdx].uiChannel[iChIdx]);
		}
	}
}

int CModelDataManager::LoadHWModelBase(CString sLinfoPath)
{
	CIniFileCS INI_HWModelBase(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	int iHWModelBaseVer;
	iHWModelBaseVer = INI_HWModelBase.Get_Integer(strSection, strKey, 1001);
	THEAPP.SetHwParamVersion(iHWModelBaseVer);

	strSection = "Grab Image Copy";
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		strKey.Format("m_bUseGrabImageHeight_%d", i + 1);
		m_bUseDiffusedImage[i] = INI_HWModelBase.Get_Bool(strSection, strKey, FALSE);

		strKey.Format("m_bUseSpecularImage_%d", i + 1);
		m_bUseSpecularImage[i] = INI_HWModelBase.Get_Bool(strSection, strKey, FALSE);

		strKey.Format("m_bUseSeqStartImage_%d", i + 1);
		m_bUseSeqStartImage[i] = INI_HWModelBase.Get_Bool(strSection, strKey, FALSE);
	}

	for (int i = 0; i < STAGE_NUMBER_MAX; i++)
	{
		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			strSection.Format("Sequence Address %d", i + 1);
			if (iHWModelBaseVer == 1001)
			{
				strKey.Format("m_iSeqAddrIndex_%d", j + 1);
				m_iSeqAddrIndex[i][j] = INI_HWModelBase.Get_Integer(strSection, strKey, 0);
			}
			else
			{
				strKey.Format("m_iSeqAddrNo_%d", j + 1);
				m_iSeqAddrIndex[i][j] = INI_HWModelBase.Get_Integer(strSection, strKey, 1) - 1;
			}

			if (m_iSeqAddrIndex[i][j] == -1)
			{
				strSection.Format("Grab Image Copy", i + 1);
				strKey.Format("m_iSeqAddrIndex_%d", j + 1);
				m_iSeqAddrIndex[i][j] = INI_HWModelBase.Get_Integer(strSection, strKey, 0);
			}
		}
	}

	if (iHWModelBaseVer == 1001)
	{
		std::remove(sLinfoPath);
		SaveHWModelBase(sLinfoPath);
	}

	return 1;
}

void CModelDataManager::SaveHWModelBase(CString sLinfoPath, int iCurStage)
{
	CIniFileCS INI_HWModelBase(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	INI_HWModelBase.Set_Integer(strSection, strKey, 1002);

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		strSection = "Grab Image Copy";
		strKey.Format("m_bUseGrabImageHeight_%d", i + 1);
		INI_HWModelBase.Set_Bool(strSection, strKey, m_bUseDiffusedImage[i]);

		strKey.Format("m_bUseSpecularImage_%d", i + 1);
		INI_HWModelBase.Set_Bool(strSection, strKey, m_bUseSpecularImage[i]);

		strKey.Format("m_bUseSeqStartImage_%d", i + 1);
		INI_HWModelBase.Set_Bool(strSection, strKey, m_bUseSeqStartImage[i]);
	}

	for (int i = 0; i < STAGE_NUMBER_MAX; i++)
	{
		if (i != iCurStage && iCurStage > 0)
			continue;

		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			strSection.Format("Sequence Address %d", i + 1);
			strKey.Format("m_iSeqAddrNo_%d", j + 1);
			INI_HWModelBase.Set_Integer(strSection, strKey, m_iSeqAddrIndex[i][j] + 1);
		}
	}
}

int CModelDataManager::LoadSpecularParam(CString sLinfoPath)
{
	CIniFileCS INI_SpecularParam(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	int iSpecularParamVer;
	iSpecularParamVer = INI_SpecularParam.Get_Integer(strSection, strKey, 1001);

	strSection = "Specular Vision";
	strKey.Format("m_iDFMLightIntensity1");
	m_iDFMLightIntensity1 = INI_SpecularParam.Get_Integer(strSection, strKey, 10);
	strKey.Format("m_iDFMLightIntensity2");
	m_iDFMLightIntensity2 = INI_SpecularParam.Get_Integer(strSection, strKey, 10);

	strKey.Format("m_dDFMPsx");
	m_dDFMPsx = INI_SpecularParam.Get_Double(strSection, strKey, 0.0);
	strKey.Format("m_dDFMPsy");
	m_dDFMPsy = INI_SpecularParam.Get_Double(strSection, strKey, 40.0);
	strKey.Format("m_dDFMTex");
	m_dDFMTex = INI_SpecularParam.Get_Double(strSection, strKey, 0.0);
	strKey.Format("m_dDFMTey");
	m_dDFMTey = INI_SpecularParam.Get_Double(strSection, strKey, 0.0);

	strKey.Format("m_iDFMMeanFilterSize");
	m_iDFMMeanFilterSize = INI_SpecularParam.Get_Integer(strSection, strKey, 4);

	for (int i = 0; i < DFM_MAX_SPECULAR_IMAGE_NUMBER; i++)
	{
		if (iSpecularParamVer == 1001)
			strKey.Format("m_dDFMSpecularImageSF%d", i);
		else
			strKey.Format("m_dDFMSpecularImageSF_%d", i + 1);
		m_dDFMSpecularImageSF[i] = INI_SpecularParam.Get_Double(strSection, strKey, 4.0);
	}

	for (int i = 0; i < DFM_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		if (iSpecularParamVer == 1001)
			strKey.Format("m_dDFMHigh%d", i);
		else
			strKey.Format("m_dDFMHigh_%d", i + 1);
		m_dDFMHigh[i] = INI_SpecularParam.Get_Double(strSection, strKey, 5.0);

		if (iSpecularParamVer == 1001)
			strKey.Format("m_dDFMLow%d", i);
		else
			strKey.Format("m_dDFMLow_%d", i + 1);
		m_dDFMLow[i] = INI_SpecularParam.Get_Double(strSection, strKey, -5.0);

		if (iSpecularParamVer == 1001)
			strKey.Format("m_dDFMDerivativeSmFactor%d", i);
		else
			strKey.Format("m_dDFMDerivativeSmFactor_%d", i + 1);
		m_dDFMDerivativeSmFactor[i] = INI_SpecularParam.Get_Double(strSection, strKey, 5.0);
	}

	for (int i = 0; i < DIFFUSED_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		if (iSpecularParamVer == 1001)
			strKey.Format("m_dDiffusedHigh%d", i);
		else
			strKey.Format("m_dDiffusedHigh_%d", i + 1);
		m_dDiffusedHigh[i] = INI_SpecularParam.Get_Double(strSection, strKey, 1.5);

		if (iSpecularParamVer == 1001)
			strKey.Format("m_dDiffusedLow%d", i);
		else
			strKey.Format("m_dDiffusedLow_%d", i + 1);
		m_dDiffusedLow[i] = INI_SpecularParam.Get_Double(strSection, strKey, -1.5);

		if (iSpecularParamVer == 1001)
			strKey.Format("m_dDiffusedSmFactor%d", i);
		else
			strKey.Format("m_dDiffusedSmFactor_%d", i + 1);
		m_dDiffusedSmFactor[i] = INI_SpecularParam.Get_Double(strSection, strKey, 10);
	}

	strSection = "Crop Image";
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		strKey.Format("Use Crop_%02d", i + 1);
		m_bUseSpecularRoiCrop[i] = INI_SpecularParam.Get_Bool(strSection, strKey, FALSE);
		strKey.Format("Resize Factor_%02d", i + 1);
		m_dSpecularResizeFactor[i] = INI_SpecularParam.Get_Double(strSection, strKey, 1.0);
		strKey.Format("LT-x_%02d", i + 1);
		m_lSpecularCropLTPointX_Roi[i] = INI_SpecularParam.Get_Integer(strSection, strKey, 0);
		strKey.Format("LT-y_%02d", i + 1);
		m_lSpecularCropLTPointY_Roi[i] = INI_SpecularParam.Get_Integer(strSection, strKey, 0);
		strKey.Format("RB-x_%02d", i + 1);
		m_lSpecularCropRBPointX_Roi[i] = INI_SpecularParam.Get_Integer(strSection, strKey, 5120);
		strKey.Format("RB-y_%02d", i + 1);
		m_lSpecularCropRBPointY_Roi[i] = INI_SpecularParam.Get_Integer(strSection, strKey, 5120);
	}

	if (iSpecularParamVer == 1001)
	{
		std::remove(sLinfoPath);
		SaveSpecularParam(sLinfoPath);
	}

	return 1;
}

void CModelDataManager::SaveSpecularParam(CString sLinfoPath)
{
	CIniFileCS INI_SpecularParam(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	INI_SpecularParam.Set_Integer(strSection, strKey, 1002);

	strSection = "Specular Vision";
	strKey = "m_iDFMLightIntensity1";
	INI_SpecularParam.Set_Integer(strSection, strKey, m_iDFMLightIntensity1);
	strKey = "m_iDFMLightIntensity2";
	INI_SpecularParam.Set_Integer(strSection, strKey, m_iDFMLightIntensity2);

	strKey = "m_dDFMPsx";
	INI_SpecularParam.Set_Double(strSection, strKey, m_dDFMPsx);
	strKey = "m_dDFMPsy";
	INI_SpecularParam.Set_Double(strSection, strKey, m_dDFMPsy);
	strKey = "m_dDFMTex";
	INI_SpecularParam.Set_Double(strSection, strKey, m_dDFMTex);
	strKey = "m_dDFMTey";
	INI_SpecularParam.Set_Double(strSection, strKey, m_dDFMTey);

	strKey.Format("m_iDFMMeanFilterSize");
	INI_SpecularParam.Set_Integer(strSection, strKey, m_iDFMMeanFilterSize);

	for (int i = 0; i < DFM_MAX_SPECULAR_IMAGE_NUMBER; i++)
	{
		strKey.Format("m_dDFMSpecularImageSF_%d", i + 1);
		INI_SpecularParam.Set_Double(strSection, strKey, m_dDFMSpecularImageSF[i]);
	}

	for (int i = 0; i < DFM_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		strKey.Format("m_dDFMHigh_%d", i + 1);
		INI_SpecularParam.Set_Double(strSection, strKey, m_dDFMHigh[i]);
		strKey.Format("m_dDFMLow_%d", i + 1);
		INI_SpecularParam.Set_Double(strSection, strKey, m_dDFMLow[i]);
		strKey.Format("m_dDFMDerivativeSmFactor_%d", i + 1);
		INI_SpecularParam.Set_Double(strSection, strKey, m_dDFMDerivativeSmFactor[i]);
	}

	for (int i = 0; i < DIFFUSED_MAX_SHAPE_IMAGE_NUMBER; i++)
	{
		strKey.Format("m_dDiffusedHigh_%d", i + 1);
		INI_SpecularParam.Set_Double(strSection, strKey, m_dDiffusedHigh[i]);
		strKey.Format("m_dDiffusedLow_%d", i + 1);
		INI_SpecularParam.Set_Double(strSection, strKey, m_dDiffusedLow[i]);
		strKey.Format("m_dDiffusedSmFactor_%d", i + 1);
		INI_SpecularParam.Set_Double(strSection, strKey, m_dDiffusedSmFactor[i]);
	}
}

int CModelDataManager::LoadAutoFocus(CString sLinfoPath)
{
	CIniFileCS INI_AutoFocus(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	int iAutoFocusVer;
	iAutoFocusVer = INI_AutoFocus.Get_Integer(strSection, strKey, 1001);

	strSection = "Reference";
	for (int i = 0; i < STAGE_NUMBER_MAX; i++)
	{
		if (iAutoFocusVer == 1001)
			strKey.Format("AfZigCenterCamZ_%d", i);
		else
			strKey.Format("AfZigCenterCamZ_%d", i + 1);
		m_dZigCenterCamZ[i] = INI_AutoFocus.Get_Double(strSection, strKey, MOTION_NOUSE);

		if (iAutoFocusVer == 1001)
			strKey.Format("AfUseZigCenterPos_%d", i);
		else
			strKey.Format("AfUseZigCenterPos_%d", i);
		m_bUseZigCenterPos[i] = INI_AutoFocus.Get_Bool(strSection, strKey, FALSE);

		if (iAutoFocusVer == 1001)
			strKey.Format("AfZigCenterX_%d", i);
		else
			strKey.Format("AfZigCenterX_%d", i + 1);
		m_iAfZigCenterX[i] = INI_AutoFocus.Get_Integer(strSection, strKey, 2560);

		if (iAutoFocusVer == 1001)
			strKey.Format("AfZigCenterY_%d", i);
		else
			strKey.Format("AfZigCenterY_%d", i + 1);
		m_iAfZigCenterY[i] = INI_AutoFocus.Get_Integer(strSection, strKey, 2560);

		if (iAutoFocusVer == 1001)
		{
			strKey.Format("AfZigCenterLightPage_%d", i);
			m_iAFZigCenterLightPage[i] = INI_AutoFocus.Get_Integer(strSection, strKey, 0);
		}
		else
		{
			strKey.Format("AfZigCenterLightPage_%d", i + 1);
			m_iAFZigCenterLightPage[i] = INI_AutoFocus.Get_Integer(strSection, strKey, 1) - 1;
		}

		if (iAutoFocusVer == 1001)
			strKey.Format("AfZigRotationCenterX_%d", i);
		else
			strKey.Format("AfZigRotationCenterX_%d", i + 1);
		m_dAfZigRotationCenterX[i] = INI_AutoFocus.Get_Double(strSection, strKey, 2560);

		if (iAutoFocusVer == 1001)
			strKey.Format("AfZigRotationCenterY_%d", i);
		else
			strKey.Format("AfZigRotationCenterY_%d", i + 1);
		m_dAfZigRotationCenterY[i] = INI_AutoFocus.Get_Double(strSection, strKey, 2560);

		if (iAutoFocusVer == 1001)
			strKey.Format("AfLeftTopCornerPointX_%d", i);
		else
			strKey.Format("AfLeftTopCornerPointX_%d", i + 1);
		m_iAfLeftTopCornerPointX[i] = INI_AutoFocus.Get_Integer(strSection, strKey, -1);

		if (iAutoFocusVer == 1001)
			strKey.Format("AfLeftTopCornerPointY_%d", i);
		else
			strKey.Format("AfLeftTopCornerPointY_%d", i + 1);
		m_iAfLeftTopCornerPointY[i] = INI_AutoFocus.Get_Integer(strSection, strKey, -1);
	}

	strSection = "Grab Image Number";
	strKey.Format("AfLensStartImageNo");
	m_iAfLensStartImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);
	strKey.Format("AfLensEndImageNo");
	m_iAfLensEndImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);

	strKey.Format("AfModuleRotationAngleImageNo");
	m_iAfModuleRotationAngleImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);

	strKey.Format("AfEastSideStartImageNo");
	m_iAfEastSideStartImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);
	strKey.Format("AfEastSideEndImageNo");
	m_iAfEastSideEndImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);
	strKey.Format("AfWestSideStartImageNo");
	m_iAfWestSideStartImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);
	strKey.Format("AfWestSideEndImageNo");
	m_iAfWestSideEndImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);
	strKey.Format("AfNorthSideStartImageNo");
	m_iAfNorthSideStartImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);
	strKey.Format("AfNorthSideEndImageNo");
	m_iAfNorthSideEndImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);
	strKey.Format("AfSouthSideStartImageNo");
	m_iAfSouthSideStartImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);
	strKey.Format("AfSouthSideEndImageNo");
	m_iAfSouthSideEndImageNo = INI_AutoFocus.Get_Integer(strSection, strKey, 0);

	if (iAutoFocusVer == 1001)
	{
		std::remove(sLinfoPath);
		SaveAutoFocus(sLinfoPath);
	}

	return 1;
}

void CModelDataManager::SaveAutoFocus(CString sLinfoPath, int iCurStage)
{
	CIniFileCS INI_AutoFocus(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	INI_AutoFocus.Set_Integer(strSection, strKey, 1002);

	strSection = "Reference";
	for (int i = 0; i < STAGE_NUMBER_MAX; i++)
	{
		if (i != iCurStage && iCurStage > 0)
			continue;

		strKey.Format("AfZigCenterCamZ_%d", i + 1);
		INI_AutoFocus.Set_Double(strSection, strKey, m_dZigCenterCamZ[i]);

		strKey.Format("AfUseZigCenterPos_%d", i + 1);
		INI_AutoFocus.Set_Bool(strSection, strKey, m_bUseZigCenterPos[i]);
		strKey.Format("AfZigCenterX_%d", i + 1);
		INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfZigCenterX[i]);
		strKey.Format("AfZigCenterY_%d", i + 1);
		INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfZigCenterY[i]);

		strKey.Format("AfZigCenterLightPage_%d", i + 1);
		INI_AutoFocus.Set_Integer(strSection, strKey, m_iAFZigCenterLightPage[i] + 1);

		strKey.Format("AfZigRotationCenterX_%d", i + 1);
		INI_AutoFocus.Set_Double(strSection, strKey, m_dAfZigRotationCenterX[i]);
		strKey.Format("AfZigRotationCenterY_%d", i + 1);
		INI_AutoFocus.Set_Double(strSection, strKey, m_dAfZigRotationCenterY[i]);

		strKey.Format("AfLeftTopCornerPointX_%d", i + 1);
		INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfLeftTopCornerPointX[i]);
		strKey.Format("AfLeftTopCornerPointY_%d", i + 1);
		INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfLeftTopCornerPointY[i]);
	}

	strSection = "Grab Image Number";
	strKey = "AfLensStartImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfLensStartImageNo);
	strKey = "AfLensEndImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfLensEndImageNo);

	strKey = "AfModuleRotationAngleImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfModuleRotationAngleImageNo);

	strKey = "AfEastSideStartImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfEastSideStartImageNo);
	strKey = "AfEastSideEndImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfEastSideEndImageNo);

	strKey = "AfWestSideStartImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfWestSideStartImageNo);
	strKey = "AfWestSideEndImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfWestSideEndImageNo);

	strKey = "AfNorthSideStartImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfNorthSideStartImageNo);
	strKey = "AfNorthSideEndImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfNorthSideEndImageNo);

	strKey = "AfSouthSideStartImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfSouthSideStartImageNo);
	strKey = "AfSouthSideEndImageNo";
	INI_AutoFocus.Set_Integer(strSection, strKey, m_iAfSouthSideEndImageNo);
}

int CModelDataManager::LoadMotionMovingPosition(CString sLinfoPath)
{
	CIniFileCS INI_MotionMovingPosition(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	int iMotionMovingPositionVer;
	iMotionMovingPositionVer = INI_MotionMovingPosition.Get_Integer(strSection, strKey, 1001);

	strSection = "Image";
	strKey = "ImageGrabCount";
	m_iNoUsedImageGrab = INI_MotionMovingPosition.Get_Integer(strSection, strKey, 1);

	if (m_iNoUsedImageGrab > 0)
	{
		for (int iii = 0; iii < STAGE_NUMBER_MAX; iii++)
		{
			m_dZFocusPosition[iii].resize(m_iNoUsedImageGrab);
			if (iMotionMovingPositionVer == 1001)
			{
				m_dLightZPosition[iii].resize(m_iNoUsedImageGrab);
				m_dTiltPosition[iii].resize(m_iNoUsedImageGrab);
				m_dRotatePosition[iii].resize(m_iNoUsedImageGrab);
				m_dStageXPosition[iii].resize(m_iNoUsedImageGrab);
				m_dStageYPosition[iii].resize(m_iNoUsedImageGrab);
			}

			strSection.Format("Moving Position %d", iii + 1);
			BOOL bReferenceCheck = TRUE;
			if (iMotionMovingPositionVer == 1001)
			{
				strKey.Format("LZ-ref");
				m_dLightZPositionRef[iii] = INI_MotionMovingPosition.Get_Double(strSection, strKey, -1000);
				strKey.Format("T-ref");
				m_dTiltPositionRef[iii] = INI_MotionMovingPosition.Get_Double(strSection, strKey, -1000);
				strKey.Format("R-ref");
				m_dRotatePositionRef[iii] = INI_MotionMovingPosition.Get_Double(strSection, strKey, -1000);
				strKey.Format("X-ref");
				m_dStageXPositionRef[iii] = INI_MotionMovingPosition.Get_Double(strSection, strKey, -1000);
				strKey.Format("Y-ref");
				m_dStageYPositionRef[iii] = INI_MotionMovingPosition.Get_Double(strSection, strKey, -1000);

				if (m_dLightZPositionRef[iii] == -1000 && m_dTiltPositionRef[iii] == -1000 && m_dRotatePositionRef[iii] == -1000 && m_dStageXPositionRef[iii] == -1000 && m_dStageYPositionRef[iii] == -1000)
				{
					bReferenceCheck = FALSE;

					if (iMotionMovingPositionVer == 1001)
						strKey.Format("LZ-0");
					else
						strKey.Format("LZ-1");
					m_dLightZPositionRef[iii] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);

					if (iMotionMovingPositionVer == 1001)
						strKey.Format("T-0");
					else
						strKey.Format("T-1");
					m_dTiltPositionRef[iii] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);

					if (iMotionMovingPositionVer == 1001)
						strKey.Format("R-0");
					else
						strKey.Format("R-1");
					m_dRotatePositionRef[iii] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);

					if (iMotionMovingPositionVer == 1001)
						strKey.Format("X-0");
					else
						strKey.Format("X-1");
					m_dStageXPositionRef[iii] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);

					if (iMotionMovingPositionVer == 1001)
						strKey.Format("Y-0");
					else
						strKey.Format("Y-1");
					m_dStageYPositionRef[iii] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);
				}
			}

			for (int i = 0; i < m_iNoUsedImageGrab; i++)
			{
				if (iMotionMovingPositionVer == 1001)
					strKey.Format("Z-%d", i);
				else
					strKey.Format("Z-%d", i + 1);
				m_dZFocusPosition[iii][i] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);

				double dTemp;
				if (iMotionMovingPositionVer == 1001)
				{
					strKey.Format("LZ-%d", i);
					if (bReferenceCheck)
					{
						dTemp = INI_MotionMovingPosition.Get_Double(strSection, strKey, 0);
						m_dLightZPosition[iii][i] = m_dLightZPositionRef[iii] + dTemp;
					}
					else
						m_dLightZPosition[iii][i] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);

					strKey.Format("T-%d", i);
					if (bReferenceCheck)
					{
						dTemp = INI_MotionMovingPosition.Get_Double(strSection, strKey, 0);
						m_dTiltPosition[iii][i] = m_dTiltPositionRef[iii] + dTemp;
					}
					else
						m_dTiltPosition[iii][i] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);

					strKey.Format("R-%d", i);
					if (bReferenceCheck)
					{
						dTemp = INI_MotionMovingPosition.Get_Double(strSection, strKey, 0);
						m_dRotatePosition[iii][i] = m_dRotatePositionRef[iii] + dTemp;
					}
					else
						m_dRotatePosition[iii][i] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);

					strKey.Format("X-%d", i);
					if (bReferenceCheck)
					{
						dTemp = INI_MotionMovingPosition.Get_Double(strSection, strKey, 0);
						m_dStageXPosition[iii][i] = m_dStageXPositionRef[iii] + dTemp;
					}
					else
						m_dStageXPosition[iii][i] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);

					strKey.Format("Y-%d", i);
					if (bReferenceCheck)
					{
						dTemp = INI_MotionMovingPosition.Get_Double(strSection, strKey, 0);
						m_dStageYPosition[iii][i] = m_dStageYPositionRef[iii] + dTemp;
					}
					else
						m_dStageYPosition[iii][i] = INI_MotionMovingPosition.Get_Double(strSection, strKey, MOTION_NOUSE);
				}
			}
		}
	}

#ifdef SINGLE_LENS
	strSection.Format("Single Lens Trigger");
	strKey.Format("Trigger-Image-Number");
	m_iTriggerImageNumber = INI_MotionMovingPosition.Get_Integer(strSection, strKey, 4);
	strKey.Format("Trigger-Period");
	m_iTriggerPeriod = INI_MotionMovingPosition.Get_Integer(strSection, strKey, 25);
#elif defined (ASSY_LENS)
	strSection.Format("Assy Lens Align End Image Number");
	strKey.Format("Bottom-Align-1");
	m_iBottomAlign1EndImageNumber = INI_MotionMovingPosition.Get_Integer(strSection, strKey, 3);
	strKey.Format("Bottom-Align-2");
	m_iBottomAlign2EndImageNumber = INI_MotionMovingPosition.Get_Integer(strSection, strKey, 6);
	strKey.Format("Top-Align");
	m_iTopAlignEndImageNumber = INI_MotionMovingPosition.Get_Integer(strSection, strKey, 8);
#endif

	if (iMotionMovingPositionVer == 1001)
	{
		std::remove(sLinfoPath);
		SaveMotionMovingPosition(sLinfoPath);

		CString strMotionMovingPositionOffsetPath = _T(sLinfoPath);
		strMotionMovingPositionOffsetPath.Replace(_T("MotionMovingPosition"), _T("MotionMovingPosition_Offset"));
		SaveMotionMovingPosition_Offset(strMotionMovingPositionOffsetPath);
	}

	return 1;
}

void CModelDataManager::SaveMotionMovingPosition(CString sLinfoPath, int iCurStage)
{
	CIniFileCS INI_MotionMovingPosition(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	INI_MotionMovingPosition.Set_Integer(strSection, strKey, 1002);

	strSection = "Image";
	strKey = "ImageGrabCount";
	INI_MotionMovingPosition.Set_Integer(strSection, strKey, m_iNoUsedImageGrab);

	for (int i = 0; i < STAGE_NUMBER_MAX; i++)
	{
		if (i != iCurStage && iCurStage > 0)
			continue;

		m_dZFocusPosition[i].resize(m_iNoUsedImageGrab);
		strSection.Format("Moving Position %d", i + 1);
		for (int j = 0; j < m_iNoUsedImageGrab; j++)
		{
			strKey.Format("Z-%d", j + 1);
			INI_MotionMovingPosition.Set_Double(strSection, strKey, m_dZFocusPosition[i][j]);
		}
	}

#ifdef SINGLE_LENS
	strSection.Format("Single Lens Trigger");
	strKey.Format("Trigger-Image-Number");
	INI_MotionMovingPosition.Set_Integer(strSection, strKey, m_iTriggerImageNumber);
	strKey.Format("Trigger-Period");
	INI_MotionMovingPosition.Set_Integer(strSection, strKey, m_iTriggerPeriod);
#elif defined (ASSY_LENS)
	strSection.Format("Assy Lens Align End Image Number");
	strKey.Format("Bottom-Align-1");
	INI_MotionMovingPosition.Set_Integer(strSection, strKey, m_iBottomAlign1EndImageNumber);
	strKey.Format("Bottom-Align-2");
	INI_MotionMovingPosition.Set_Integer(strSection, strKey, m_iBottomAlign2EndImageNumber);
	strKey.Format("Top-Align");
	INI_MotionMovingPosition.Set_Integer(strSection, strKey, m_iTopAlignEndImageNumber);
#endif


}

int CModelDataManager::LoadMotionMovingPosition_Offset(CString sLinfoPath)
{
	CIniFileCS INI_MotionMovingPosition_Offset(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	int iMotionMovingPositionOffsetVer;
	iMotionMovingPositionOffsetVer = INI_MotionMovingPosition_Offset.Get_Integer(strSection, strKey, 1001);

	if (m_iNoUsedImageGrab > 0)
	{
		for (int iii = 0; iii < STAGE_NUMBER_MAX; iii++)
		{
			m_dLightZPosition[iii].resize(m_iNoUsedImageGrab);
			m_dTiltPosition[iii].resize(m_iNoUsedImageGrab);
			m_dRotatePosition[iii].resize(m_iNoUsedImageGrab);
			m_dStageXPosition[iii].resize(m_iNoUsedImageGrab);
			m_dStageYPosition[iii].resize(m_iNoUsedImageGrab);

			strSection.Format("Moving Position %d", iii + 1);
			strKey.Format("LZ-ref");
			m_dLightZPositionRef[iii] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, -1000);
			strKey.Format("T-ref");
			m_dTiltPositionRef[iii] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, -1000);
			strKey.Format("R-ref");
			m_dRotatePositionRef[iii] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, -1000);
			strKey.Format("X-ref");
			m_dStageXPositionRef[iii] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, -1000);
			strKey.Format("Y-ref");
			m_dStageYPositionRef[iii] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, -1000);

			BOOL bReferenceCheck = TRUE;
			if (m_dLightZPositionRef[iii] == -1000 && m_dTiltPositionRef[iii] == -1000 && m_dRotatePositionRef[iii] == -1000 && m_dStageXPositionRef[iii] == -1000 && m_dStageYPositionRef[iii] == -1000)
			{
				bReferenceCheck = FALSE;

				strKey.Format("LZ-1");
				m_dLightZPositionRef[iii] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, MOTION_NOUSE);

				strKey.Format("T-1");
				m_dTiltPositionRef[iii] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, MOTION_NOUSE);

				strKey.Format("R-1");
				m_dRotatePositionRef[iii] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, MOTION_NOUSE);

				strKey.Format("X-1");
				m_dStageXPositionRef[iii] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, MOTION_NOUSE);

				strKey.Format("Y-1");
				m_dStageYPositionRef[iii] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, MOTION_NOUSE);
			}

			for (int i = 0; i < m_iNoUsedImageGrab; i++)
			{
				double dTemp;
				strKey.Format("LZ-%d", i + 1);
				if (bReferenceCheck)
				{
					dTemp = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, 0);
					m_dLightZPosition[iii][i] = m_dLightZPositionRef[iii] + dTemp;
				}
				else
					m_dLightZPosition[iii][i] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, MOTION_NOUSE);

				strKey.Format("T-%d", i + 1);
				if (bReferenceCheck)
				{
					dTemp = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, 0);
					m_dTiltPosition[iii][i] = m_dTiltPositionRef[iii] + dTemp;
				}
				else
					m_dTiltPosition[iii][i] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, MOTION_NOUSE);

				strKey.Format("R-%d", i + 1);
				if (bReferenceCheck)
				{
					dTemp = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, 0);
					m_dRotatePosition[iii][i] = m_dRotatePositionRef[iii] + dTemp;
				}
				else
					m_dRotatePosition[iii][i] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, MOTION_NOUSE);

				strKey.Format("X-%d", i + 1);
				if (bReferenceCheck)
				{
					dTemp = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, 0);
					m_dStageXPosition[iii][i] = m_dStageXPositionRef[iii] + dTemp;
				}
				else
					m_dStageXPosition[iii][i] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, MOTION_NOUSE);

				strKey.Format("Y-%d", i + 1);
				if (bReferenceCheck)
				{
					dTemp = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, 0);
					m_dStageYPosition[iii][i] = m_dStageYPositionRef[iii] + dTemp;
				}
				else
					m_dStageYPosition[iii][i] = INI_MotionMovingPosition_Offset.Get_Double(strSection, strKey, MOTION_NOUSE);
			}
		}
	}

	if (iMotionMovingPositionOffsetVer == 1001)
	{
		std::remove(sLinfoPath);
		SaveMotionMovingPosition_Offset(sLinfoPath);
	}

	return 1;
}

void CModelDataManager::SaveMotionMovingPosition_Offset(CString sLinfoPath, int iCurStage)
{
	CIniFileCS INI_MotionMovingPosition_Offset(sLinfoPath);

	CString strSection, strKey;
	strSection.Format("Model Base");
	strKey.Format("HWParameterVersion");
	INI_MotionMovingPosition_Offset.Set_Integer(strSection, strKey, 1002);

	for (int i = 0; i < STAGE_NUMBER_MAX; i++)
	{
		if (i != iCurStage && iCurStage > 0)
			continue;

		strSection.Format("Moving Position %d", i + 1);
		strKey.Format("LZ-ref");
		INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dLightZPositionRef[i]);
		strKey.Format("T-ref");
		INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dTiltPositionRef[i]);
		strKey.Format("R-ref");
		INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dRotatePositionRef[i]);
		strKey.Format("X-ref");
		INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dStageXPositionRef[i]);
		strKey.Format("Y-ref");
		INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dStageYPositionRef[i]);

		m_dLightZPosition[i].resize(m_iNoUsedImageGrab);
		m_dTiltPosition[i].resize(m_iNoUsedImageGrab);
		m_dRotatePosition[i].resize(m_iNoUsedImageGrab);
		m_dStageXPosition[i].resize(m_iNoUsedImageGrab);
		m_dStageYPosition[i].resize(m_iNoUsedImageGrab);
		for (int j = 0; j < m_iNoUsedImageGrab; j++)
		{
			strKey.Format("LZ-%d", j + 1);
			INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dLightZPosition[i][j] - m_dLightZPositionRef[i]);

			strKey.Format("T-%d", j + 1);
			INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dTiltPosition[i][j] - m_dTiltPositionRef[i]);

			strKey.Format("R-%d", j + 1);
			INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dRotatePosition[i][j] - m_dRotatePositionRef[i]);

			strKey.Format("X-%d", j + 1);
			INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dStageXPosition[i][j] - m_dStageXPositionRef[i]);

			strKey.Format("Y-%d", j + 1);
			INI_MotionMovingPosition_Offset.Set_Double(strSection, strKey, m_dStageYPosition[i][j] - m_dStageYPositionRef[i]);
		}
	}
}

void CModelDataManager::InitLightAverageValue()
{
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		for (int j = 0; j < LIGHT_CH_CNT; j++)
			m_iLightChannelRefGV[i][j] = 0;

		m_iLightAverageValueTotal[i] = 0;
	}
}

int CModelDataManager::LoadLightAverageValue(CString sLinfoPath)
{
	CIniFileCS INIInspectionLight(sLinfoPath);

	CString sSection, strItem;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sSection.Format("Image_%d", i + 1);

		strItem.Format("Average_Value_1");
		m_iLightChannelRefGV[i][0] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Average_Value_2");
		m_iLightChannelRefGV[i][1] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Average_Value_3");
		m_iLightChannelRefGV[i][2] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Average_Value_4");
		m_iLightChannelRefGV[i][3] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Average_Value_5");
		m_iLightChannelRefGV[i][4] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Average_Value_6");
		m_iLightChannelRefGV[i][5] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Average_Value_7");
		m_iLightChannelRefGV[i][6] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Average_Value_8");
		m_iLightChannelRefGV[i][7] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Average_Value_9");
		m_iLightChannelRefGV[i][8] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Average_Value_10");
		m_iLightChannelRefGV[i][9] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Average_Value_Total");
		m_iLightAverageValueTotal[i] = INIInspectionLight.Get_Integer(sSection, strItem, 0);

		strItem.Format("Teaching_Focus_Value");
		m_dTeachingFocusValue[i] = INIInspectionLight.Get_Double(sSection, strItem, 0);
	}

	return 1;
}

void CModelDataManager::SaveLightAverageValue(CString sLinfoPath)
{
	CIniFileCS INIInspectionLight(sLinfoPath);

	CString sSection, strItem;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		sSection.Format("Image_%d", i + 1);

		strItem.Format("Average_Value_1");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightChannelRefGV[i][0]);

		strItem.Format("Average_Value_2");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightChannelRefGV[i][1]);

		strItem.Format("Average_Value_3");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightChannelRefGV[i][2]);

		strItem.Format("Average_Value_4");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightChannelRefGV[i][3]);

		strItem.Format("Average_Value_5");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightChannelRefGV[i][4]);

		strItem.Format("Average_Value_6");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightChannelRefGV[i][5]);

		strItem.Format("Average_Value_7");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightChannelRefGV[i][6]);

		strItem.Format("Average_Value_8");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightChannelRefGV[i][7]);

		strItem.Format("Average_Value_9");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightChannelRefGV[i][8]);

		strItem.Format("Average_Value_10");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightChannelRefGV[i][9]);

		strItem.Format("Average_Value_Total");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightAverageValueTotal[i]);

		strItem.Format("Teaching_Focus_Value");
		INIInspectionLight.Set_Double(sSection, strItem, m_dTeachingFocusValue[i]);
	}
}

BOOL CModelDataManager::LoadModelSWData()
{
	try
	{
		BOOL bModelExist = TRUE;
		if (m_sModelName == ".")
		{
			bModelExist = FALSE;
			return bModelExist;
		}

		CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
		CString strSwModelFolder;
		strSwModelFolder.Format("%s\\SW\\Vision_N%d", strModelFolder, m_iModelIdx + 1);

		if (GetFileAttributes(strSwModelFolder) == -1)
		{
			bModelExist = FALSE;
			return bModelExist;
		}

		SetFolderFile(strSwModelFolder, "InspectParam.dat");

		if (!m_bOpened)
			OpenDataFile();

		if (m_bOpened)
		{
			SAFE_DELETE(m_pInspectionArea);
			m_pInspectionArea = new GTRegion;

			CCalDataService* pCalDataService;

			if (m_iModelIdx == VISION_NUMBER_1)
				pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_1];
			else if (m_iModelIdx == VISION_NUMBER_2)
				pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_2];
			else if (m_iModelIdx == VISION_NUMBER_3)
				pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_3];
			else if (m_iModelIdx == VISION_NUMBER_4)
				pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_4];

			HTuple HFileHandle = -1;
			CString szTemplateModelFilename;
			szTemplateModelFilename = strSwModelFolder + "\\InspectTemplate.dat";

			HANDLE				hFindFile;
			WIN32_FIND_DATA		FindFileData;

			if ((hFindFile = FindFirstFile(szTemplateModelFilename, &FindFileData)) != INVALID_HANDLE_VALUE)
			{
				OpenFile(HTuple(szTemplateModelFilename), "input_binary", &HFileHandle);
				FindClose(hFindFile);
			}

			CString strConfig = m_sModelName.Mid(m_sModelName.Find('_') + 1);
			strConfig = strConfig.Mid(strConfig.Find('_') + 1);
			strConfig = strConfig.Mid(strConfig.Find('_') + 1);
			strConfig = strConfig.Mid(0, strConfig.Find('_'));

			m_pInspectionArea->Load((HANDLE)m_File.m_hFile, m_iModelVersion, HFileHandle, pCalDataService, m_iModelIdx, strConfig);

			if (HFileHandle != -1)
				CloseFile(HFileHandle);

			CloseDataFile();
		}

		CheckColorSpace();

		return bModelExist;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [ModelDataManager::LoadModelSWData] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL CModelDataManager::LoadModelHWData()
{
	try
	{
		BOOL bModelExist = TRUE;

		if (THEAPP.g_strModelTypeName[THEAPP.GetModelType()] == "-")
		{
			bModelExist = FALSE;
			return bModelExist;
		}

		CString strOpticModelFolder;
		strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
		if (GetFileAttributes(strOpticModelFolder) == -1)
		{
			bModelExist = FALSE;
			return bModelExist;
		}

		CString strHWModelBase;
		strHWModelBase.Format("%s\\HW\\Vision_N%d\\ModelBase.ini", strOpticModelFolder, m_iModelIdx + 1);
		LoadHWModelBase(strHWModelBase);

		CString strInspectLightInfo, strSequenceInfo;
		if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[m_iModelIdx])
		{
			strInspectLightInfo.Format("%s\\HW\\Vision_N%d\\InspectLightInfo_Pwm.ini", strOpticModelFolder, m_iModelIdx + 1);
			LoadPwmLightInfo(strInspectLightInfo);

#ifdef INLINE_MODE
			if (THEAPP.m_pLightPwmManager[m_iModelIdx]->OpenPort(m_PageControlData.m_iComPortNumber) == FALSE)
				AfxMessageBox("통신 포트 열기 실패!!!. 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
#endif

			THEAPP.m_pLightPwmManager[m_iModelIdx]->SetCh1LightValue(m_PageControlData.m_Page[0].uiChannel[0]);
			THEAPP.m_pLightPwmManager[m_iModelIdx]->SetCh2LightValue(m_PageControlData.m_Page[0].uiChannel[1]);
			THEAPP.m_pLightPwmManager[m_iModelIdx]->SetCh3LightValue(m_PageControlData.m_Page[0].uiChannel[2]);
			THEAPP.m_pLightPwmManager[m_iModelIdx]->SetCh4LightValue(m_PageControlData.m_Page[0].uiChannel[3]);
		}
		else
		{
			strInspectLightInfo.Format("%s\\HW\\Vision_N%d\\InspectLightInfo.ini", strOpticModelFolder, m_iModelIdx + 1);
			LoadLightInfo(strInspectLightInfo);

			strSequenceInfo.Format("%s\\HW\\Vision_N%d\\SequenceInfo.ini", strOpticModelFolder, m_iModelIdx + 1);
			LoadSequenceInfo(strSequenceInfo);

#ifdef INLINE_MODE

#ifdef SINGLE_LENS

			int iPageIdx;

			if (m_PageControlData.m_ComPort.OpenPort(m_PageControlData.m_iComPortNumber, 19200))
			{
				for (iPageIdx = 0; iPageIdx < LIGHTCTRL_PAGE_COUNT_8CH; iPageIdx++)
				{
					m_PageControlData.SetIllumination_8CH(iPageIdx);
				}

				m_PageControlData.m_ComPort.ClosePort();
			}
			else
			{
				if (m_iModelIdx == VISION_NUMBER_1)
					AfxMessageBox("통신 포트 열기 실패!!!. Viison#1 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
				else if (m_iModelIdx == VISION_NUMBER_2)
					AfxMessageBox("통신 포트 열기 실패!!!. Viison#2 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
			}

#elif defined(ASSY_LENS)

			int iPageIdx;
			int iPortNumber;

			if (m_iModelIdx == VISION_NUMBER_4)
			{
				if (m_PageControlData.m_ComPort.OpenPort(m_PageControlData.m_iComPortNumber, 19200))
				{
					for (iPageIdx = 0; iPageIdx < LIGHTCTRL_PAGE_COUNT_BTM_ALIGN; iPageIdx++)
					{
						m_PageControlData.SetIllumination_6CH(iPageIdx);
					}
					m_PageControlData.m_ComPort.ClosePort();
				}
				else
				{
					AfxMessageBox("통신 포트 열기 실패!!!. Vision#4, Bottom Align 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
				}

				if (m_PageControlData.m_ComPort.OpenPort(m_PageControlData.m_iSubComPortNumber, 19200))
				{
					for (iPageIdx = LIGHTCTRL_PAGE_COUNT_BTM_ALIGN; iPageIdx < (LIGHTCTRL_PAGE_COUNT_BTM_ALIGN+ LIGHTCTRL_PAGE_COUNT_TOP_ALIGN); iPageIdx++)
					{
						m_PageControlData.SetIllumination_2CH(iPageIdx);
					}
					m_PageControlData.m_ComPort.ClosePort();
				}
				else
				{
					AfxMessageBox("통신 포트 열기 실패!!!. Vision#4, Top Align 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
				}
			}
			else
			{
				if (m_PageControlData.m_ComPort.OpenPort(m_PageControlData.m_iComPortNumber, 19200))
				{
					if (m_iModelIdx == VISION_NUMBER_1 || m_iModelIdx == VISION_NUMBER_2)
					{
						for (iPageIdx = 0; iPageIdx < LIGHTCTRL_PAGE_COUNT_8CH; iPageIdx++)
						{
							m_PageControlData.SetIllumination_8CH(iPageIdx);
						}
					}
					else if (m_iModelIdx == VISION_NUMBER_3)
					{
						for (iPageIdx = 0; iPageIdx < LIGHTCTRL_PAGE_COUNT_4CH; iPageIdx++)
						{
							m_PageControlData.SetIllumination_4CH(iPageIdx);
						}
					}

					m_PageControlData.m_ComPort.ClosePort();
				}
				else
				{
					if (m_iModelIdx == VISION_NUMBER_1)
						AfxMessageBox("통신 포트 열기 실패!!!. Viison#1 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
					else if (m_iModelIdx == VISION_NUMBER_2)
						AfxMessageBox("통신 포트 열기 실패!!!. Viison#2 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
					else if (m_iModelIdx == VISION_NUMBER_3)
						AfxMessageBox("통신 포트 열기 실패!!!. Viison#3 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
				}
			}
#else
			for (int i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
				m_PageControlData.SetGrabSequence(m_iModelIdx, i, m_PageControlData.m_iAddrSeq[i], m_PageControlData.m_iAddrCount[i]);

			for (int iPageIdx = 0; iPageIdx < LIGHTCTRL_PAGE_COUNT; iPageIdx++)
				m_PageControlData.SetOnTime_Page(m_iModelIdx, iPageIdx);
#endif
#endif
		}
		CString strSpecularParam;
		strSpecularParam.Format("%s\\HW\\Vision_N%d\\SpecularParam.ini", strOpticModelFolder, m_iModelIdx + 1);
		LoadSpecularParam(strSpecularParam);

		CString strAutoFocus;
		strAutoFocus.Format("%s\\HW\\Vision_N%d\\AutoFocus.ini", strOpticModelFolder, m_iModelIdx + 1);
		LoadAutoFocus(strAutoFocus);

		CString strMotionMovingPosition;
		strMotionMovingPosition.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition.ini", strOpticModelFolder, m_iModelIdx + 1);
		LoadMotionMovingPosition(strMotionMovingPosition);

		CString strMotionMovingPosition_Offset;
		strMotionMovingPosition_Offset.Format("%s\\HW\\Vision_N%d\\MotionMovingPosition_Offset.ini", strOpticModelFolder, m_iModelIdx + 1);
		LoadMotionMovingPosition_Offset(strMotionMovingPosition_Offset);

		CString strAutoCal;
		strAutoCal.Format("%s\\HW\\Vision_N%d\\AutoCal.ini", strOpticModelFolder, m_iModelIdx + 1);
		LoadLightAverageValue(strAutoCal);

		return bModelExist;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [ModelDataManager::LoadModelHWData] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}


void CModelDataManager::UpdateTeachParamView()
{
	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		THEAPP.m_pTabControlDlg->m_pTeachParamDlg->AlgorithmParamUpDate(FALSE, FALSE, i);
}

void CModelDataManager::ClearAiRetrainFlag()
{
	if (m_pInspectionArea)
		m_pInspectionArea->ClearAiRetrainFlag();
}

BOOL CModelDataManager::LoadModelSWBaseData()
{
	try
	{
		BOOL bModelExist = TRUE;

		if (m_sModelName == ".")
		{
			bModelExist = FALSE;
			return bModelExist;
		}

		CString strVisionModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
		if (GetFileAttributes(strVisionModelFolder) == -1)
		{
			bModelExist = FALSE;
			return bModelExist;
		}

		CString strModelBase;
		strModelBase.Format("%s\\SW\\Vision_N%d\\ModelBase.ini", strVisionModelFolder, m_iModelIdx + 1);

		CIniFileCS INI_SWModelBase(strModelBase);

		CString strSection, strKey;
		strSection = "Model Base";
		strKey = "VERSION";
		m_iModelVersion = INI_SWModelBase.Get_Integer(strSection, strKey, 1000);

		if (m_iModelIdx == VISION_NUMBER_1)
		{
			int iParamVersion;
			strKey = "ParameterVersion";
			iParamVersion = INI_SWModelBase.Get_Integer(strSection, strKey, 9999);
			if (iParamVersion == 9999)
			{
				INI_SWModelBase.Set_Integer(strSection, strKey, 1001);
				THEAPP.SetSwParamVersion(1001);
			}
			else
				THEAPP.SetSwParamVersion(iParamVersion);
		}

		strKey = "TrayArrayX";
		m_iTrayArrayX = INI_SWModelBase.Get_Integer(strSection, strKey, 3);
		strKey = "TrayArrayY";
		m_iTrayArrayY = INI_SWModelBase.Get_Integer(strSection, strKey, 20);
		m_iTrayModuleMax = m_iTrayArrayX * m_iTrayArrayY;
		m_iTrayModuleAmt = m_iTrayModuleMax;

		THEAPP.DoEvents();

		//////////////////////////////////////////////////////////////////////////
		// Load Align Model
		strSection = "Local Align";
		for (int i = 0; i < MAX_IMAGE_TAB; i++)
		{
			if (m_lLAlignModelID[i] >= 0)
			{
				if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_TEMPLATE)
					ClearNccModel(m_lLAlignModelID[i]);
				else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_SHAPE)
					ClearShapeModel(m_lLAlignModelID[i]);
				else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_PERSPECTIVE)
					ClearDeformableModel(m_lLAlignModelID[i]);

				m_lLAlignModelID[i] = -1;
			}
		}

		for (int i = 0; i < MAX_IMAGE_TAB; i++)
		{
			strKey.Format("m_bUseLocalAlignMatching_%d", i + 1);
			m_bUseLocalAlignMatching[i] = INI_SWModelBase.Get_Bool(strSection, strKey, FALSE);

			strKey.Format("m_iLocalMatchingMethod_%d", i + 1);
			m_iLocalMatchingMethod[i] = INI_SWModelBase.Get_Integer(strSection, strKey, ALIGN_MATCHING_SHAPE);

			strKey.Format("m_iLocalMatchingTeachingRectLTX_%d", i + 1);
			m_iLocalMatchingTeachingRectLTX[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 0);
			strKey.Format("m_iLocalMatchingTeachingRectLTY_%d", i + 1);
			m_iLocalMatchingTeachingRectLTY[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 0);
			strKey.Format("m_iLocalMatchingTeachingRectRBX_%d", i + 1);
			m_iLocalMatchingTeachingRectRBX[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 0);
			strKey.Format("m_iLocalMatchingTeachingRectRBY_%d", i + 1);
			m_iLocalMatchingTeachingRectRBY[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 0);

			strKey.Format("m_iLocalMatchingPyramidLevel_%d", i + 1);
			m_iLocalMatchingPyramidLevel[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 0);
			strKey.Format("m_iLocalMatchingAngleRange_%d", i + 1);
			m_iLocalMatchingAngleRange[i] = INI_SWModelBase.Get_Double(strSection, strKey, 5.0);
			strKey.Format("m_dLocalMatchingScaleMin_%d", i + 1);
			m_dLocalMatchingScaleMin[i] = INI_SWModelBase.Get_Double(strSection, strKey, 0.98);
			strKey.Format("m_dLocalMatchingScaleMax_%d", i + 1);
			m_dLocalMatchingScaleMax[i] = INI_SWModelBase.Get_Double(strSection, strKey, 1.02);

			strKey.Format("m_iLocalMatchingTeachingContrast_%d", i + 1);
			m_iLocalMatchingTeachingContrast[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 40);

			strKey.Format("m_iLocalMatchingTeachingContrastLow_%d", i + 1);
			m_iLocalMatchingTeachingContrastLow[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 40);

			strKey.Format("m_iLocalMatchingTeachingContrastMinSize_%d", i + 1);
			m_iLocalMatchingTeachingContrastMinSize[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 1);

			strKey.Format("m_dLocalMatchingScore_%d", i + 1);
			m_dLocalMatchingScore[i] = INI_SWModelBase.Get_Double(strSection, strKey, 0.3);
			strKey.Format("m_iLocalMatchingInspectionMinContrast_%d", i + 1);
			m_iLocalMatchingInspectionMinContrast[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 15);
			strKey.Format("m_iLocalMatchingSearchMarginX_%d", i + 1);
			m_iLocalMatchingSearchMarginX[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 300);
			strKey.Format("m_iLocalMatchingSearchMarginY_%d", i + 1);
			m_iLocalMatchingSearchMarginY[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 300);
			strKey.Format("m_dLocalTeachAlignRefX_%d", i + 1);
			m_dLocalTeachAlignRefX[i] = INI_SWModelBase.Get_Double(strSection, strKey, -1);
			strKey.Format("m_dLocalTeachAlignRefY_%d", i + 1);
			m_dLocalTeachAlignRefY[i] = INI_SWModelBase.Get_Double(strSection, strKey, -1);
			strKey.Format("m_bUseMatchingAffineConstant_%d", i + 1);
			m_bUseMatchingAffineConstant[i] = INI_SWModelBase.Get_Bool(strSection, strKey, FALSE);

			for (int j = 0; j < MAX_IMAGE_TAB; j++)
			{
				strKey.Format("m_bLocalAlignImage_%d_%d", i + 1, j + 1);
				m_bLocalAlignImage[i][j] = INI_SWModelBase.Get_Bool(strSection, strKey, FALSE);
			}

			strKey.Format("m_iLocalMatchingProcessChType_%d", i + 1);
			m_iLocalMatchingProcessChType[i] = INI_SWModelBase.Get_Integer(strSection, strKey, CHANNEL_TYPE_I);

			strKey.Format("m_bLocalUseMatchingFilter_%d", i + 1);
			m_bUseMatchingFilter[i] = INI_SWModelBase.Get_Bool(strSection, strKey, FALSE);
			strKey.Format("m_iMatchingFilterType_%d", i + 1);
			m_iMatchingFilterType[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 0);
			strKey.Format("m_dMatchingFilterTypeXSize_%d", i + 1);
			m_dMatchingFilterTypeXSize[i] = INI_SWModelBase.Get_Double(strSection, strKey, 7);
			strKey.Format("m_dMatchingFilterTypeYSize_%d", i + 1);
			m_dMatchingFilterTypeYSize[i] = INI_SWModelBase.Get_Double(strSection, strKey, 7);

			strKey.Format("m_bUseMatchingAngleComp_%d", i + 1);
			m_bUseMatchingAngleComp[i] = INI_SWModelBase.Get_Bool(strSection, strKey, FALSE);
			strKey.Format("m_iMatchingAngleRefLine_%d", i + 1);
			m_iMatchingAngleRefLine[i] = INI_SWModelBase.Get_Integer(strSection, strKey, MATCHING_ANGLE_REF_LINE_X);

			strKey.Format("m_bCheckUseMatchingXYComp_%d", i + 1);
			m_bCheckUseMatchingXYComp[i] = INI_SWModelBase.Get_Bool(strSection, strKey, FALSE);
			strKey.Format("m_iLocalIndexNumber_%d", i + 1);
			m_iLocalIndexNumber[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 0);
			strKey.Format("m_iRadioMatchingXYRefLine_%d", i + 1);
			m_iRadioMatchingXYRefLine[i] = INI_SWModelBase.Get_Integer(strSection, strKey, 0);
		}

		CString strMachingModelFolderPath, strMachingModel;
		strMachingModelFolderPath.Format("%s\\SW\\Vision_N%d\\MatchingModel", strVisionModelFolder, m_iModelIdx + 1);

		for (int i = 0; i < MAX_IMAGE_TAB; i++)
		{
			if (m_bUseLocalAlignMatching[i])
			{
				strMachingModel.Format("%s\\LocalAlignModel_%d", strMachingModelFolderPath, i + 1);

				CFileFind finder;

				if (finder.FindFile((LPCTSTR)strMachingModel))
				{
					if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_TEMPLATE)
						ReadNccModel(HTuple(strMachingModel), &(m_lLAlignModelID[i]));
					else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_SHAPE)
						ReadShapeModel(HTuple(strMachingModel), &(m_lLAlignModelID[i]));
					else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_PERSPECTIVE)
						ReadDeformableModel(HTuple(strMachingModel), &(m_lLAlignModelID[i]));

					THEAPP.DoEvents();
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Load FAI Spec
		CString strFaiMeasureSpec;
		strFaiMeasureSpec.Format("%s\\SW\\Vision_N%d\\FaiMeasureSpec_DFA.ini", strVisionModelFolder, m_iModelIdx + 1);

		CIniFileCS INI_FAIMeasureSpec(strFaiMeasureSpec);

		strSection = "FAI";
		strKey = "m_bMUseFai";
		m_bMUseFai = INI_FAIMeasureSpec.Get_Bool(strSection, strKey, FALSE);

		for (int i = 0; i < MAX_FAI_ITEM; i++)
		{
			if (m_iModelVersion < 2022)
			{
				strKey.Format("m_bMInspectFai_%d", i);
				m_bMInspectFai[i] = INI_FAIMeasureSpec.Get_Bool(strSection, strKey, FALSE);
				strKey.Format("m_dMNominalFai_%d", i);
				m_dMNominalFai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 1.0);
				strKey.Format("m_dMToleranceMaxFai_%d", i);
				m_dMToleranceMaxFai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 0.1);
				strKey.Format("m_dMToleranceMinFai_%d", i);
				m_dMToleranceMinFai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, -0.1);
				strKey.Format("m_dMOffsetLeftFai_%d", i);
				m_dMOffsetStg1Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 0);
				strKey.Format("m_dMTiltLeftFai_%d", i);
				m_dMTiltStg1Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 0);
				strKey.Format("m_dMMultipleLeftFai_%d", i);
				m_dMMultipleStg1Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 1);
				strKey.Format("m_dMOffsetRightFai_%d", i);
				m_dMOffsetStg2Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 0);
				strKey.Format("m_dMTiltRightFai_%d", i);
				m_dMTiltStg2Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 0);
				strKey.Format("m_dMMultipleRightFai_%d", i);
				m_dMMultipleStg2Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 1);
				strKey.Format("m_iTransferCodeIndexFai_%d", i);
				m_iTransferCodeIndexFai[i] = INI_FAIMeasureSpec.Get_Integer(strSection, strKey, 0);
				strKey.Format("m_bMultiFai_%d", i);
				m_bMultiFai[i] = INI_FAIMeasureSpec.Get_Bool(strSection, strKey, FALSE);
			}
			else
			{
				strKey.Format("m_bMInspectFai_%d", i);
				m_bMInspectFai[i] = INI_FAIMeasureSpec.Get_Bool(strSection, strKey, FALSE);
				strKey.Format("m_dMNominalFai_%d", i);
				m_dMNominalFai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 1.0);
				strKey.Format("m_dMToleranceMaxFai_%d", i);
				m_dMToleranceMaxFai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 0.1);
				strKey.Format("m_dMToleranceMinFai_%d", i);
				m_dMToleranceMinFai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, -0.1);
				strKey.Format("m_dMOffsetStg1Fai_%d", i);
				m_dMOffsetStg1Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 0);
				strKey.Format("m_dMTiltStg1Fai_%d", i);
				m_dMTiltStg1Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 0);
				strKey.Format("m_dMMultipleStg1Fai_%d", i);
				m_dMMultipleStg1Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 1);
				strKey.Format("m_dMOffsetStg2Fai_%d", i);
				m_dMOffsetStg2Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 0);
				strKey.Format("m_dMTiltStg2Fai_%d", i);
				m_dMTiltStg2Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 0);
				strKey.Format("m_dMMultipleStg2Fai_%d", i);
				m_dMMultipleStg2Fai[i] = INI_FAIMeasureSpec.Get_Double(strSection, strKey, 1);
				strKey.Format("m_iTransferCodeIndexFai_%d", i);
				m_iTransferCodeIndexFai[i] = INI_FAIMeasureSpec.Get_Integer(strSection, strKey, 0);
				strKey.Format("m_bMultiFai_%d", i);
				m_bMultiFai[i] = INI_FAIMeasureSpec.Get_Bool(strSection, strKey, FALSE);
			}
		}

		THEAPP.DoEvents();
		//////////////////////////////////////////////////////////////////////////

		return bModelExist;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [ModelDataManager::LoadModelSWBaseData] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL CModelDataManager::LoadModelHWBaseData()
{
	try
	{
		CString sVisionCamType;
		sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[m_iModelIdx];

		BOOL bModelExist = TRUE;

		if (THEAPP.g_strModelTypeName[THEAPP.GetModelType()] == "-")
		{
			bModelExist = FALSE;
			return bModelExist;
		}

		CString strOpticModelFolder;
		strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
		if (GetFileAttributes(strOpticModelFolder) == -1)
		{
			bModelExist = FALSE;
			return bModelExist;
		}

		THEAPP.DoEvents();
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Cal Data Initialization
		CString strCalData;
		strCalData.Format("%s\\HW\\Vision_N%d\\CalData.ini", strOpticModelFolder, m_iModelIdx + 1);

		CIniFileCS INI_CalData(strCalData);

		double dPixelRes;
		int iCamImageSizeX, iCamImageSizeY;
		CString strSection, strKey;
		strSection = "PIXEL_RESOLUTION";
		strKey = sVisionCamType;
#ifdef INLINE_MODE
		dPixelRes = INI_CalData.Get_Double(strSection, strKey, 5.0);
#else
		dPixelRes = THEAPP.m_ModelDefineInfo.m_iPixelResolution[m_iModelIdx];
#endif
		iCamImageSizeX = THEAPP.m_ModelDefineInfo.m_iVisionImageWidth[m_iModelIdx];
		iCamImageSizeY = THEAPP.m_ModelDefineInfo.m_iVisionImageHeight[m_iModelIdx];
		THEAPP.m_pCalDataService_N[m_iModelIdx]->SetCalData(iCamImageSizeX, iCamImageSizeY, dPixelRes);
		THEAPP.m_pCalDataService_N[m_iModelIdx]->SetVisionCamName(m_iModelIdx);

		THEAPP.m_pCalDataService = THEAPP.m_pCalDataService_N[m_iModelIdx];
		//////////////////////////////////////////////////////////////////////////

#ifdef INLINE_MODE
		//////////////////////////////////////////////////////////////////////////
		// Load White Balance
		THEAPP.m_pCameraManager->UpdateWhiteBalance(m_iModelIdx);
		//////////////////////////////////////////////////////////////////////////
#endif

		//////////////////////////////////////////////////////////////////////////
		// Load Auto Cal
		CString strAutoCal;
		strAutoCal.Format("%s\\HW\\Vision_N%d\\AutoCal.ini", strOpticModelFolder, m_iModelIdx + 1);

		CIniFileCS INI_AutoCal(strAutoCal);
		strSection = "Auto Cal Setting";

		strKey.Format("m_iLightValueStart");
		m_iLightValueStart = INI_AutoCal.Get_Integer(strSection, strKey, 10);

		strKey.Format("m_iLightValueEnd");
		m_iLightValueEnd = INI_AutoCal.Get_Integer(strSection, strKey, 10);

		strKey.Format("m_iLightValueInterval");
		m_iLightValueInterval = INI_AutoCal.Get_Integer(strSection, strKey, 2);

		strKey.Format("m_iLightValueInTol");
		m_iLightValueInTol = INI_AutoCal.Get_Integer(strSection, strKey, 10);

		strKey.Format("m_iTotalImageValueTol");
		m_iTotalImageValueTol = INI_AutoCal.Get_Integer(strSection, strKey, 5);

		strKey.Format("m_dFocusValueStart");
		m_dFocusValueStart = INI_AutoCal.Get_Double(strSection, strKey, 0.5);

		strKey.Format("m_dFocusValueEnd");
		m_dFocusValueEnd = INI_AutoCal.Get_Double(strSection, strKey, 0.5);

		strKey.Format("m_dFocusValueInterval");
		m_dFocusValueInterval = INI_AutoCal.Get_Double(strSection, strKey, 0.1);

		strKey.Format("m_iSelectFocusTol");
		m_iSelectFocusTol = INI_AutoCal.Get_Integer(strSection, strKey, 0);

		strKey.Format("m_dFocusZValueTol");
		m_dFocusZValueTol = INI_AutoCal.Get_Double(strSection, strKey, 0.5);

		strKey.Format("m_dFocusEdgeValueTol");
		m_dFocusEdgeValueTol = INI_AutoCal.Get_Double(strSection, strKey, 0.7);
		////////////////////////////////////////////////////////////////////////////

		return bModelExist;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [ModelDataManager::LoadModelHWBaseData] : <%s>%s", (const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

void CModelDataManager::UpdateModelParamVersion()
{
	if (m_sModelName == ".")
		return;

	if (m_iModelIdx != VISION_NUMBER_1)
		return;

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	if (GetFileAttributes(strModelFolder) == -1)
	{
		AfxMessageBox("모델 파일을 찾을 수 없습니다. 상단 툴바 [Load] 버튼으로 모델을 수동으로 로딩해 주세요.", MB_ICONERROR | MB_SYSTEMMODAL);

		return;
	}

	CString strSWModelBase;
	strSWModelBase.Format("%s\\SW\\Vision_N%d\\ModelBase.ini", strModelFolder, m_iModelIdx + 1);

	CIniFileCS INI(strSWModelBase);

	CString strSection = "Model Base";

	if (m_iModelIdx == VISION_NUMBER_1)
	{
		int iCurParamVersion, iNewParamVersion;

		iCurParamVersion = THEAPP.GetSwParamVersion();
		iNewParamVersion = iCurParamVersion + 1;

		INI.Set_Integer(strSection, "ParameterVersion", iNewParamVersion);
		THEAPP.SetSwParamVersion(iNewParamVersion);

		THEAPP.ShowVersionText();
	}
}

//2017.04.11 hbk
BOOL CModelDataManager::ReadOCR_SVM(void)
{
	CString strFontFolder = THEAPP.GetWorkingDirectory() + "\\Data\\Font";
	CString FontName = strFontFolder + "\\" + m_strFontName + ".svm";

	HANDLE				hFindFile;
	WIN32_FIND_DATA		FindFileData;

	if ((hFindFile = FindFirstFile(FontName, &FindFileData)) == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (m_lOCRHandle >= 0)
		ClearOcrClassSvm(m_lOCRHandle);
	try
	{
		HTuple HlModelTemp;
		ReadOcrClassSvm(HTuple(FontName), &HlModelTemp);
		m_lOCRHandle = HlModelTemp.L();
	}
	catch (HException& except)
	{
		return FALSE;
	}

	return TRUE;

}

void CModelDataManager::SaveChangeParamHistory()
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	int iSWVersion;
	iSWVersion = THEAPP.GetProgramVersion();

	int iParamVersion;
	iParamVersion = THEAPP.GetSwParamVersion();

	CString strFile;
	CString FolderName = THEAPP.Struct_PreferenceStruct.m_strEtcFolderPath + "CMI_Results\\ParameterChangeHistory";
	CString sFileName;
	sFileName.Format("파라미터변경내역_설비코드%s_레시피%s_SW버전%d_파라미터버전%d_%04d%02d%02d_%02d%02d%02d.txt", THEAPP.Struct_PreferenceStruct.m_strEquipNo, m_sModelName, iSWVersion, iParamVersion, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	strFile.Format("%s\\%s", FolderName, sFileName);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		CString sMsg;
		sMsg.Format("[CMI_Results]->[ParameterChangeHistory] 폴더에 %s 파일이 생성되지 않았습니다.", sFileName);
		AfxMessageBox(sMsg, MB_ICONERROR | MB_SYSTEMMODAL);
		return;
	}

	try
	{
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave.Format("변경시간\t설비 코드\t모델명\tSW 버전\tVision명\t변경Type\t영상번호\t검사대상\t검사영역위치(LT/RB)\t검사탭\t파라미터\t변경전\t변경후\r\n");
			file.Write(strSave, strSave.GetLength());
		}

		// 변경Type: 검사영역(추가,삭제,변경), 파라미터, 검사조건

		CompareParameter(&file, time);

		/************* 파일 닫기 **************/
		file.Close();

	}
	catch (CFileException* pEx) {
		pEx->Delete();
	}
}

void CModelDataManager::CompareParameter(CFile* file, SYSTEMTIME time)
{
	if (m_sModelName == ".")
		return;

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;

	CString strParseSection = _T("");

	// ********************** Equip No *****************************
	CString strPreferenceFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_Pref(strPreferenceFolder + "\\Preference.ini");

	CString strEquipNo = THEAPP.Struct_PreferenceStruct.m_strEquipNo;
	// *************************************************************

	// ********************** Vision명 *****************************

	CString strVisionName = _T("");
	strVisionName = THEAPP.m_ModelDefineInfo.m_strVisionName[THEAPP.m_iCurTeachVision];

	CString sBaseVal = _T("");
	sBaseVal.Format("[%04d%02d%02d %02d:%02d:%02d]\t%s\t%s\t%s\t%s", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
					strEquipNo, m_sModelName, THEAPP.m_strVerInfo, strVisionName);

	// Load File
	strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;

	strModelFolder.Format("%s\\SW\\Vision_N%d", strModelFolder, THEAPP.m_iCurTeachVision + 1);

	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(strModelFolder, &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	FindClose(hFind);

	CString szFilePath = strModelFolder + "\\" + "InspectParam.dat";

	CFile ParamFile;
	if (!ParamFile.Open(szFilePath, CFile::modeRead))
		return;

	CString strConfig = m_sModelName.Mid(m_sModelName.Find('_') + 1);
	strConfig = strConfig.Mid(strConfig.Find('_') + 1);
	strConfig = strConfig.Mid(strConfig.Find('_') + 1);
	strConfig = strConfig.Mid(0, strConfig.Find('_'));

	HTuple HFileHandle = -1;
	GTRegion* pFileInspectionArea = NULL;
	pFileInspectionArea = new GTRegion;
	pFileInspectionArea->Load((HANDLE)ParamFile.m_hFile, m_iModelVersion, HFileHandle, NULL, THEAPP.m_iCurTeachVision, strConfig);

	ParamFile.Close();

	// Compare ROI / Param

	GTRegion* pFileInspectROIRgn;
	GTRegion* pInspectROIRgn;

	int iNoFileInspectROI = 0;
	iNoFileInspectROI = pFileInspectionArea->GetChildTRegionCount();

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	int iFileIdx, iIdx;
	int iFileInspectionROIID, iFileTeachImageIndex, iInspectionROIID, iTeachImageIndex;
	POINT LTPoint, RBPoint, OrgLTPoint, OrgRBPoint;

	BOOL bROIFound = FALSE;
	CString sFixString = _T("");
	CString sROIChange = _T("");
	CString sFixStringTab = _T("");

	CString sInspectionTypeName;

	for (iFileIdx = 0; iFileIdx < iNoFileInspectROI; iFileIdx++)
	{
		pFileInspectROIRgn = pFileInspectionArea->GetChildTRegion(iFileIdx);
		if (pFileInspectROIRgn == NULL)
			continue;

		if (pFileInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pFileInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			iFileInspectionROIID = pFileInspectROIRgn->miInspectionROIID;
			iFileTeachImageIndex = pFileInspectROIRgn->miTeachImageIndex;

			bROIFound = FALSE;

			for (iIdx = 0; iIdx < iNoInspectROI; iIdx++)
			{
				pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST)
				{
					iInspectionROIID = pInspectROIRgn->miInspectionROIID;
					iTeachImageIndex = pInspectROIRgn->miTeachImageIndex;

					if (iFileInspectionROIID == iInspectionROIID && iFileTeachImageIndex == iTeachImageIndex)
					{
						bROIFound = TRUE;
						break;
					}
				}
			}

			pFileInspectROIRgn->GetLTPoint(&OrgLTPoint, THEAPP.m_pCalDataService);
			pFileInspectROIRgn->GetRBPoint(&OrgRBPoint, THEAPP.m_pCalDataService);

			if (bROIFound == FALSE)
			{
				sInspectionTypeName = g_strInspectionTypeName[pFileInspectROIRgn->miInspectionType - 1].c_str();

				sROIChange.Format("%s\tROI 삭제\t영상 %d\t%s\t(%d,%d)/(%d,%d)\r\n", sBaseVal, pFileInspectROIRgn->miTeachImageIndex, sInspectionTypeName, OrgLTPoint.x, OrgLTPoint.y, OrgRBPoint.x, OrgRBPoint.y);
				file->Write(sROIChange, sROIChange.GetLength());

				continue;
			}

			pInspectROIRgn->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
			pInspectROIRgn->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

			sInspectionTypeName = g_strInspectionTypeName[pFileInspectROIRgn->miInspectionType - 1].c_str();

			if (OrgLTPoint.x != LTPoint.x || OrgLTPoint.y != LTPoint.y || OrgRBPoint.x != RBPoint.x || OrgRBPoint.y != RBPoint.y)
			{
				// ROI 변경 기록

				sROIChange.Format("%s\tROI위치 변경\t영상 %d\t%s\t(%d,%d)/(%d,%d)\t\t\t(%d,%d)/(%d,%d)\r\n", sBaseVal, pInspectROIRgn->miTeachImageIndex, sInspectionTypeName, LTPoint.x, LTPoint.y, RBPoint.x, RBPoint.y, OrgLTPoint.x, OrgLTPoint.y, OrgRBPoint.x, OrgRBPoint.y);
				file->Write(sROIChange, sROIChange.GetLength());
			}

			sFixString.Format("%s\t파라미터 변경\t영상 %d\t%s\t(%d,%d)/(%d,%d)", sBaseVal, pInspectROIRgn->miTeachImageIndex, sInspectionTypeName, LTPoint.x, LTPoint.y, RBPoint.x, RBPoint.y);

			int iSaveTabNo = 3;
			for (int ii = 0; ii < MAX_INSPECTION_TAB; ii++)
			{
				sFixStringTab.Format("%s\t검사탭%d", sFixString, ii + 1);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bInspect, pInspectROIRgn->m_AlgorithmParam[ii].m_bInspect, "검사적용", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter1, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter1, "1차망 적용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter2, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter2, "2차망 적용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter3, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter3, "3차망 적용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter4, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter4, "SVM 적용", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bCheckAIUseConsiderImageNo, pInspectROIRgn->m_AlgorithmParam[ii].m_bCheckAIUseConsiderImageNo, "AI 이미지 번호 고려", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bCheckAIUseConsiderDefectCode, pInspectROIRgn->m_AlgorithmParam[ii].m_bCheckAIUseConsiderDefectCode, "AI 결함 코드 고려", sFixStringTab, file);

				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectType, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectType, "불량항목", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseInspectArea[0], pInspectROIRgn->m_AlgorithmParam[ii].m_bUseInspectArea[0], "검사영역1 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour1Margin[0], pInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour1Margin[0], "검사영역1 마진1", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour2Margin[0], pInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour2Margin[0], "검사영역1 마진2", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseInspectArea[1], pInspectROIRgn->m_AlgorithmParam[ii].m_bUseInspectArea[1], "검사영역2 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour1Margin[1], pInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour1Margin[1], "검사영역2 마진1", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour2Margin[1], pInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour2Margin[1], "검사영역2 마진2", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseInspectArea[2], pInspectROIRgn->m_AlgorithmParam[ii].m_bUseInspectArea[2], "검사영역3 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour1Margin[2], pInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour1Margin[2], "검사영역3 마진1", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour2Margin[2], pInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour2Margin[2], "검사영역3 마진2", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceInspection, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceInspection, "표면검사 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceMeasureSaveLog, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceMeasureSaveLog, "면적, X방향 길이, Y방향 길이 로그 저장", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceInspectEachROI, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceInspectEachROI, "분할 ROI 개별 검사", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iSurfaceInspectEachROINgNumber, pInspectROIRgn->m_AlgorithmParam[ii].m_iSurfaceInspectEachROINgNumber, "분할 ROI 개별 검사/불량 ROI 개수", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bApplyCircleFitting, pInspectROIRgn->m_AlgorithmParam[ii].m_bApplyCircleFitting, "Circle Fitting 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iCircleFittingEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_iCircleFittingEdgeStr, "Circle Fitting 적용 / Edge Threshold", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iCircleFittingEdgeMargin, pInspectROIRgn->m_AlgorithmParam[ii].m_iCircleFittingEdgeMargin, "Circle Fitting Edge추출 Margin", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDTPolarTrans, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDTPolarTrans, "Dynamic Threshold 원형 필터링 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnected, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnected, "고정 Threshold 결과와 연결된 Blob만 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnectedArea, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnectedArea, "고정 Threshold 검출 조건/면적 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnectedLength, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnectedLength, "고정 Threshold 검출 조건/길이 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnectedWidth, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnectedWidth, "고정 Threshold 검출 조건/폭 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iFTConnectedAreaMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iFTConnectedAreaMin, "고정 Threshold 검출 조건/최소면적", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iFTConnectedLengthMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iFTConnectedLengthMin, "고정 Threshold 검출 조건/최소길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iFTConnectedWidthMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iFTConnectedWidthMin, "고정 Threshold 검출 조건/최소폭", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBarcodeBlob, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBarcodeBlob, "바코드 인식 영역을 Blob으로 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBlobCornerMeasure, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBlobCornerMeasure, "전체 Blob을 사각형 변환 후 코너 위치 측정", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSaveBNEOverflowTop, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseSaveBNEOverflowTop, "Blob분석/상단 BNE Overflow 길이 측정값 저장", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSaveBNEOverflowBottom, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseSaveBNEOverflowBottom, "Blob분석/하단 BNE Overflow 길이 측정값 저장", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightFixedThres, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightFixedThres, "밝은 불량 검출/고정 Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightLowerThres, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightLowerThres, "밝은 불량 검출/고정 Threshold 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUpperThres, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUpperThres, "밝은 불량 검출/고정 Threshold 최대값", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightDT, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightDT, "밝은 불량 검출/Dynamic Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightMedianFilterSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightMedianFilterSize, "밝은 불량 검출/Dynamic Threshold/노이즈제거 Median 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightClosingFilterSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightClosingFilterSize, "밝은 불량 검출/Dynamic Threshold/불량 붙임 Closing 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterType, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterType, "밝은 불량 검출/Dynamic Threshold/필터 종류", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize, "밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(X)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize2, "밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(Y)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTValue, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTValue, "밝은 불량 검출/Dynamic Threshold/DT 밝기차", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightUniformityCheck, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightUniformityCheck, "밝은 불량 검출/균일도 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityPeakMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityPeakMin, "밝은 불량 검출/균일도 검사/Peak검출범위 최소", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityPeakMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityPeakMax, "밝은 불량 검출/균일도 검사/Peak검출범위 최대", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityOffset, "밝은 불량 검출/균일도 검사/히스토그램 옵셋", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityHystLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityHystLength, "밝은 불량 검출/균일도 검사/Hyst Length", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityHystOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityHystOffset, "밝은 불량 검출/균일도 검사/Hyst 히스토그램 옵셋", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightHystThres, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightHystThres, "밝은 불량 검출/Hysteresis Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystSecureThres, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystSecureThres, "밝은 불량 검출/Hysteresis Threshold/Secure Threshold", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialThres, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialThres, "밝은 불량 검출/Hysteresis Threshold/Potential Threshold", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialLength, "밝은 불량 검출/Hysteresis Threshold/Hyst Length", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkFixedThres, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkFixedThres, "어두운 불량 검출/고정 Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkLowerThres, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkLowerThres, "어두운 불량 검출/고정 Threshold 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUpperThres, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUpperThres, "어두운 불량 검출/고정 Threshold 최대값", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkDT, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkDT, "어두운 불량 검출/Dynamic Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkMedianFilterSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkMedianFilterSize, "어두운 불량 검출/Dynamic Threshold/노이즈제거 Median 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkClosingFilterSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkClosingFilterSize, "어두운 불량 검출/Dynamic Threshold/불량 붙임 Closing 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTFilterType, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTFilterType, "어두운 불량 검출/Dynamic Threshold/필터 종류", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTFilterSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTFilterSize, "어두운 불량 검출/Dynamic Threshold/DT Mean 필터 크기(X)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTFilterSize2, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTFilterSize2, "어두운 불량 검출/Dynamic Threshold/DT Mean 필터 크기(Y)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTValue, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTValue, "어두운 불량 검출/Dynamic Threshold/DT 밝기차", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkUniformityCheck, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkUniformityCheck, "어두운 불량 검출/균일도 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityPeakMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityPeakMin, "어두운 불량 검출/균일도 검사/Peak검출범위 최소", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityPeakMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityPeakMax, "어두운 불량 검출/균일도 검사/Peak검출범위 최대", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityOffset, "어두운 불량 검출/균일도 검사/히스토그램 옵셋", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityHystLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityHystLength, "어두운 불량 검출/균일도 검사/Hyst Length", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityHystOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityHystOffset, "어두운 불량 검출/균일도 검사/Hyst 히스토그램 옵셋", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkHystThres, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkHystThres, "어두운 불량 검출/Hysteresis Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkHystSecureThres, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkHystSecureThres, "어두운 불량 검출/Hysteresis Threshold/Secure Threshold", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkHystPotentialThres, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkHystPotentialThres, "어두운 불량 검출/Hysteresis Threshold/Potential Threshold", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkHystPotentialLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iDarkHystPotentialLength, "어두운 불량 검출/Hysteresis Threshold/Hyst Length", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightBinThres, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightBinThres, "밝은 불량 검출/Binary Threshold 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkBinThres, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkBinThres, "어두운 불량 검출/Binary Threshold 사용", sFixStringTab, file);

				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iCandidateMinArea, pInspectROIRgn->m_AlgorithmParam[ii].m_iCandidateMinArea, "(후처리)최소면적", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bCombineBrightDarkBlob, pInspectROIRgn->m_AlgorithmParam[ii].m_bCombineBrightDarkBlob, "밝은 불량과 어두운 불량을 합친 후 후처리", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bMaxBlob, pInspectROIRgn->m_AlgorithmParam[ii].m_bMaxBlob, "최대 크기만 선택", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bClosingOpening, pInspectROIRgn->m_AlgorithmParam[ii].m_bClosingOpening, "Closing->Opening", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bApplyOneDirMorp, pInspectROIRgn->m_AlgorithmParam[ii].m_bApplyOneDirMorp, "단일방향에만 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iOneDirMorpXOpening, pInspectROIRgn->m_AlgorithmParam[ii].m_iOneDirMorpXOpening, "X방향 Opening", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iOneDirMorpXClosing, pInspectROIRgn->m_AlgorithmParam[ii].m_iOneDirMorpXClosing, "X방향 Closing", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iThresholdOpeningSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iThresholdOpeningSize, "Threshold Opening", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iThresholdClosingSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iThresholdClosingSize, "Threshold Closing", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iOpeningSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iOpeningSize, "Opening", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iClosingSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iClosingSize, "Closing", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bFillup, pInspectROIRgn->m_AlgorithmParam[ii].m_bFillup, "내부 채움", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseConvex, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseConvex, "Convex 변환", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSubConvex, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseSubConvex, "Convex 형상 대비 차(Difference) 영역 추출", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseRect, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseRect, "사각형 변환", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseConnection, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseConnection, "불량 연결 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionMinSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionMinSize, "연결 후보 최소 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionMinXLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionMinXLength, "(연결 조건)X방향길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionLength, "연결할 후보간 거리", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseAngle, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseAngle, "연결 후보간 각도 정보 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionMaxWidth, pInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionMaxWidth, "(불량 연결)스크래치 최대 폭", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHoleFillCondition, pInspectROIRgn->m_AlgorithmParam[ii].m_iHoleFillCondition, "내부 Hole 채움 조건", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iFillHoleArea, pInspectROIRgn->m_AlgorithmParam[ii].m_iFillHoleArea, "Hole 면적 이하", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bXYLengthMeasureIgnoreHole, pInspectROIRgn->m_AlgorithmParam[ii].m_bXYLengthMeasureIgnoreHole, "길이 측정 시 내부 Hole 무시", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bXYLengthMaxLineLength, pInspectROIRgn->m_AlgorithmParam[ii].m_bXYLengthMaxLineLength, "최대길이를 라인 별로 측정", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryConnectionAny, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryConnectionAny, "(불량검출조건)ROI 외측(내측) 기준선과 연결 체크", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryConnectionOuter, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryConnectionOuter, "(불량검출조건)ROI 외측(내측) 기준선과 연결 체크/최외측 내측 선택", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginAny, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginAny, "(불량검출조건)ROI 외측(내측) 기준선과 연결 체크/기준선 마진", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginMinConnectionAny, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginMinConnectionAny, "(불량검출조건)ROI 외측(내측) 기준선과 연결 체크/최소 연결 픽셀수", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageScaling, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageScaling, "(영상 전처리)Contrast 향상 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageScaleMethod, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageScaleMethod, "(영상 전처리)자동변환", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageScaleMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageScaleMin, "(영상 전처리)고정 GV 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageScaleMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageScaleMax, "(영상 전처리)고정 GV 최대값", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseEdgeImage, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseEdgeImage, "(영상 전처리)Edge 영상 변환 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeFilterType, pInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeFilterType, "(영상 전처리)Edge 필터 방향", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dEdgeImageScale, pInspectROIRgn->m_AlgorithmParam[ii].m_dEdgeImageScale, "(영상 전처리)Edge Scale", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUsePeak, pInspectROIRgn->m_AlgorithmParam[ii].m_bUsePeak, "(대표 밝기 체크)사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iNormalPeakThresMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iNormalPeakThresMin, "(대표 밝기 체크)Peak 밝기 검출 범위 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iNormalPeakThresMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iNormalPeakThresMax, "(대표 밝기 체크)Peak 밝기 검출 범위 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iAbnormalPeakHistogramOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_iAbnormalPeakHistogramOffset, "(대표 밝기 체크)비정상 Peak 불량 히스토그램 옵셋", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionArea, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionArea, "(불량검출조건)면적", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionLength, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionLength, "(불량검출조건)길이", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionWidth, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionWidth, "(불량검출조건)폭", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionXLength, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionXLength, "(불량검출조건)X방향길이", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionYLength, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionYLength, "(불량검출조건)Y방향길이", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionMean, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionMean, "(불량검출조건)평균밝기", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionStdev, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionStdev, "(불량검출조건)밝기표준편차", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionAnisometry, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionAnisometry, "(불량검출조건)길이/폭 비율", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionCircularity, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionCircularity, "(불량검출조건)원형 유사도", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionRectangularity, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionRectangularity, "(불량검출조건)사각형 유사도", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionConvexity, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionConvexity, "(불량검출조건)컨벡스 유사도", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionEllipseRatio, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionEllipseRatio, "(불량검출조건)장축/단축 비율", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionAreaRatio, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionAreaRatio, "(불량검출조건)ROI 면적비율", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionContrast, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionContrast, "(불량검출조건)Contrast", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionGrad, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionGrad, "(불량검출조건)경계밝기변화", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionBlob, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionBlob, "(불량검출조건)Blob 갯수", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionShiftX, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionShiftX, "(불량검출조건)X 중심 Shift", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionShiftY, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionShiftY, "(불량검출조건)Y 중심 Shift", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionOuterDist, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionOuterDist, "(불량검출조건)외측 기준선으로부터 거리", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionInnerDist, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionInnerDist, "(불량검출조건)ROI 중심으로부터 거리", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionUseZeroArea, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionUseZeroArea, "(불량검출조건)미검출 시 검사 ROI를 불량 영역으로 검출", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryConnection, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryConnection, "(불량검출조건)사각 외측 기준선과 연결 체크", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginTop, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginTop, "(불량검출조건)사각 외측 기준선과 연결 체크/상", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginBottom, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginBottom, "(불량검출조건)사각 외측 기준선과 연결 체크/하", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginLeft, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginLeft, "(불량검출조건)사각 외측 기준선과 연결 체크/좌", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginRight, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginRight, "(불량검출조건)사각 외측 기준선과 연결 체크/우", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginHCenter, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginHCenter, "(불량검출조건)사각 외측 기준선과 연결 체크/수평중심", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginVCenter, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginVCenter, "(불량검출조건)사각 외측 기준선과 연결 체크/수직중심", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLengthWidthCondition, pInspectROIRgn->m_AlgorithmParam[ii].m_iLengthWidthCondition, "(불량검출조건)길이, 폭 And/Or 조건", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iXYLengthCondition, pInspectROIRgn->m_AlgorithmParam[ii].m_iXYLengthCondition, "(불량검출조건)X방향길이, Y방향길이 And/Or 조건", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryConnection2Side, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryConnection2Side, "(불량검출조건)외측 기준선과 2면이상 연결", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryConnectionCondition, pInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryConnectionCondition, "(불량검출조건)사각 외측 기준선과 연결 체크 And/Or 조건", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionInnerOuterBoundaryConnection, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionInnerOuterBoundaryConnection, "(불량검출조건)내측 기준선과 외측 기준선에 모두 연결", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iXLengthCondition, pInspectROIRgn->m_AlgorithmParam[ii].m_iXLengthCondition, "(불량검출조건)X방향길이/길이측정기준", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iXLengthConditionMinRef, pInspectROIRgn->m_AlgorithmParam[ii].m_iXLengthConditionMinRef, "(불량검출조건)X방향길이/최소길이 측정 시 기준면", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iYLengthCondition, pInspectROIRgn->m_AlgorithmParam[ii].m_iYLengthCondition, "(불량검출조건)Y방향길이/길이측정기준", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iYLengthConditionMinRef, pInspectROIRgn->m_AlgorithmParam[ii].m_iYLengthConditionMinRef, "(불량검출조건)Y방향길이/최소길이 측정 시 기준면", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionHole, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionHole, "(불량검출조건)Hole 검출", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionHoleArea, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionHoleArea, "(불량검출조건)Hole 검출면적", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionUnionBlob, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionUnionBlob, "(불량검출조건)1개 Blob으로 합친 후 불량 검출 조건 적용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionDilation, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionDilation, "(불량검출조건)검출 영역 +확대 / -축소", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionOuterDiff, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionOuterDiff, "(불량검출조건)최외곽 근방 검증", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionOuterDiffInterval, pInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionOuterDiffInterval, "(불량검출조건)최외곽 근방 검증 간격", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionOuterDiffMinDiff, pInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionOuterDiffMinDiff, "(불량검출조건)최외곽 근방 검증 최소높이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nOuterDiffUpper, pInspectROIRgn->m_AlgorithmParam[ii].m_nOuterDiffUpper, "(불량검출조건)최외곽 근방 검증 방향", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionOuterDiffMaxRepeat, pInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionOuterDiffMaxRepeat, "(불량검출조건)최외곽 근방 검증 반복수", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionConerLengthUse, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionConerLengthUse, "(불량검출조건)코너길이 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionConerLengthDir, pInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionConerLengthDir, "(불량검출조건)코너길이 방향", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionConerLengthMin, pInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionConerLengthMin, "(불량검출조건)코너 최소 길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionConerLengthMax, pInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionConerLengthMax, "(불량검출조건)코너 최대 길이", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionConerLengthMaxUse, pInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionConerLengthMaxUse, "(불량검출조건)코너길이 최대길이사용", sFixStringTab, file);

				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionDilationSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionDilationSize, "(불량검출조건)검출 영역 +확대 / -축소 사이즈", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionXLengthSum, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionXLengthSum, "(불량검출조건)합산 X방향 길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthSumMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthSumMin, "(불량검출조건)합산 X방향 길이 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthSumMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthSumMax, "(불량검출조건)합산 X방향 길이 최대값", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionYLengthSum, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionYLengthSum, "(불량검출조건)합산 Y방향 길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthSumMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthSumMin, "(불량검출조건)합산 Y방향 길이 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthSumMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthSumMax, "(불량검출조건)합산 Y방향 길이 최대값", sFixStringTab, file);

				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaMin, "(불량검출조건)면적 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionLengthMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionLengthMin, "(불량검출조건)길이 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionWidthMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionWidthMin, "(불량검출조건)폭 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthMin, "(불량검출조건)X방향길이 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthMin, "(불량검출조건)Y방향길이 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionMeanMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionMeanMin, "(불량검출조건)평균밝기 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionStdevMin, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionStdevMin, "(불량검출조건)밝기표준편차 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionAnisometryMin, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionAnisometryMin, "(불량검출조건)길이/폭 비율 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionCircularityMin, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionCircularityMin, "(불량검출조건)원형 유사도 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionRectangularityMin, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionRectangularityMin, "(불량검출조건)사각형 유사도 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionConvexityMin, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionConvexityMin, "(불량검출조건)컨벡스 유사도 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionEllipseRatioMin, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionEllipseRatioMin, "(불량검출조건)장축/단축 비율 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaRatioMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaRatioMin, "(불량검출조건)ROI 면적비율 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionContrastMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionContrastMin, "(불량검출조건)Contrast 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionGradMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionGradMin, "(불량검출조건)경계밝기변화 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBlobMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBlobMin, "(불량검출조건)Blob 갯수 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftXMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftXMin, "(불량검출조건)X 중심 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftYMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftYMin, "(불량검출조건)Y 중심 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionOuterDist, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionOuterDist, "(불량검출조건)외측 기준선으로부터 거리 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionInnerDist, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionInnerDist, "(불량검출조건)ROI 중심으로부터 거리 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginTop, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginTop, "(불량검출조건)사각 외측 기준선과 연결 체크/상 기준선 마진", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginBottom, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginBottom, "(불량검출조건)사각 외측 기준선과 연결 체크/하 기준선 마진", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginLeft, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginLeft, "(불량검출조건)사각 외측 기준선과 연결 체크/좌 기준선 마진", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginRight, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginRight, "(불량검출조건)사각 외측 기준선과 연결 체크/우 기준선 마진", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginHCenter, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginHCenter, "(불량검출조건)사각 외측 기준선과 연결 체크/수평중심 기준선 마진", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginVCenter, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginVCenter, "(불량검출조건)사각 외측 기준선과 연결 체크/수직중심 기준선 마진", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginMinConnection, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginMinConnection, "(불량검출조건)사각 외측 기준선과 연결 체크/최소 연결 픽셀수", sFixStringTab, file);

				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaMax, "(불량검출조건)면적 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionLengthMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionLengthMax, "(불량검출조건)길이 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionWidthMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionWidthMax, "(불량검출조건)폭 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthMax, "(불량검출조건)X방향길이 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthMax, "(불량검출조건)Y방향길이 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionMeanMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionMeanMax, "(불량검출조건)평균밝기 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionStdevMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionStdevMax, "(불량검출조건)밝기표준편차 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionAnisometryMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionAnisometryMax, "(불량검출조건)길이/폭 비율 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionCircularityMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionCircularityMax, "(불량검출조건)원형 유사도 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionRectangularityMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionRectangularityMax, "(불량검출조건)사각형 유사도 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionConvexityMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionConvexityMax, "(불량검출조건)컨벡스 유사도 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionEllipseRatioMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionEllipseRatioMax, "(불량검출조건)장축/단축 비율 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaRatioMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaRatioMax, "(불량검출조건)ROI 면적비율 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionContrastMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionContrastMax, "(불량검출조건)Contrast 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionGradMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionGradMax, "(불량검출조건)경계밝기변화 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBlobMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBlobMax, "(불량검출조건)Blob 갯수 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftXMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftXMax, "(불량검출조건)X 중심 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftYMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftYMax, "(불량검출조건)Y 중심 최대값", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseLineFitting, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseLineFitting, "Line Fitting 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseOtherImageFitting, pInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseOtherImageFitting, "(Line Fitting 알고리즘)다른 조명 영상 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitOtherImageNumber, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitOtherImageNumber, "(Line Fitting 알고리즘)영상 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitEdgeMethod, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitEdgeMethod, "(Line Fitting 알고리즘)Zero Crossing", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitFittingEdgeSF, pInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitFittingEdgeSF, "(Line Fitting 알고리즘)Edge Detection Smoothing Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitFittingEdgeHigh, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitFittingEdgeHigh, "(Line Fitting 알고리즘)Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitFittingMinContourLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitFittingMinContourLength, "(Line Fitting 알고리즘)최소 Contour 길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitFittingContourConnectionLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitFittingContourConnectionLength, "(Line Fitting 알고리즘)Contour 연결 길이", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseRotationInspection, pInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseRotationInspection, "(Line Fitting 알고리즘)회전 각도 계산", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitRotationReference, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitRotationReference, "(Line Fitting 알고리즘)수평 라인", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitRotationDefectAngle, pInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitRotationDefectAngle, "(Line Fitting 알고리즘)회전 불량 각도", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitObjectLowerThres, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitObjectLowerThres, "(Chipping/Bur 불량검출)Object 검출/고정 Threshold 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitObjectUpperThres, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitObjectUpperThres, "(Chipping/Bur 불량검출)Object 검출/고정 Threshold 최대값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitObjectEdgeSF, pInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitObjectEdgeSF, "(Chipping/Bur 불량검출)Edge Detection Smoothing Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitEdgeHigh, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitEdgeHigh, "(Chipping/Bur 불량검출)Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitMinContourLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitMinContourLength, "(Chipping/Bur 불량검출)최소 Contour 길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitContourConnectionLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitContourConnectionLength, "(Chipping/Bur 불량검출)Contour 연결 길이", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseBurInspection, pInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseBurInspection, "(Chipping/Bur 불량검출)Bur 검출", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitBurDefectLength, pInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitBurDefectLength, "(Chipping/Bur 불량검출)Bur 불량 검출/Fitting Line으로부터 길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitBurDefectLengthMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitBurDefectLengthMax, "(Chipping/Bur 불량검출)Bur 불량 검출/길이 상한", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseChippingInspection, pInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseChippingInspection, "(Chipping/Bur 불량검출)Chipping 검출", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitChippingDefectLength, pInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitChippingDefectLength, "(Chipping/Bur 불량검출)Chipping 불량 검출/Fitting Line으로부터 길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitChippingDefectLengthMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitChippingDefectLengthMax, "(Chipping/Bur 불량검출)Chipping 불량 검출/길이 상한", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bNoUseLineFit, pInspectROIRgn->m_AlgorithmParam[ii].m_bNoUseLineFit, "(Line Fitting 알고리즘)Contour 끝점을 Fitting 라인으로 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitLocalAlignUse, pInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitLocalAlignUse, "(Line Fitting 알고리즘)Local Align 데이터 계산", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseOCR, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseOCR, "OCR 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_lHorizontalDivide, pInspectROIRgn->m_AlgorithmParam[ii].m_lHorizontalDivide, "(OCR)가로 분할 평균 밝기값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_lSecondDifferent, pInspectROIRgn->m_AlgorithmParam[ii].m_lSecondDifferent, "(OCR)세로 분할 평균 밝기값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_ldiff, pInspectROIRgn->m_AlgorithmParam[ii].m_ldiff, "(OCR)Segment erosion", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iOCRSegmentDilation, pInspectROIRgn->m_AlgorithmParam[ii].m_iOCRSegmentDilation, "(OCR)Segment dilation", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBarcode, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBarcode, "바코드 검사 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bInspectBarcodeShift, pInspectROIRgn->m_AlgorithmParam[ii].m_bInspectBarcodeShift, "바코드 Shift 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dBarcodeShiftToleranceX, pInspectROIRgn->m_AlgorithmParam[ii].m_dBarcodeShiftToleranceX, "(바코드)X위치 Shift 허용오차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dBarcodeShiftToleranceY, pInspectROIRgn->m_AlgorithmParam[ii].m_dBarcodeShiftToleranceY, "(바코드)Y위치 Shift 허용오차", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bInspectBarcodeRotation, pInspectROIRgn->m_AlgorithmParam[ii].m_bInspectBarcodeRotation, "바코드 회전 검사 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBarcodeOtherImage, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBarcodeOtherImage, "인식 실패 시 다른 조명 영상 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBarcodeOtherImageNumber, pInspectROIRgn->m_AlgorithmParam[ii].m_iBarcodeOtherImageNumber, "(인식 실패 시 다른 조명 영상 사용)영상번호", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUsePinLength, pInspectROIRgn->m_AlgorithmParam[ii].m_bUsePinLength, "Connector Pin 휘어짐 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinPos, pInspectROIRgn->m_AlgorithmParam[ii].m_iPinPos, "(커넥터 핀)Pin 검출 위치", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinDTFilterSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iPinDTFilterSize, "(커넥터 핀)DT Mean 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinOpeningClosingSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iPinOpeningClosingSize, "(커넥터 핀)Opening -> Closing Size", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinMinArea, pInspectROIRgn->m_AlgorithmParam[ii].m_iPinMinArea, "(커넥터 핀)Pin 최소 면적", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinDistanceTolerance, pInspectROIRgn->m_AlgorithmParam[ii].m_iPinDistanceTolerance, "(커넥터 핀)기준선으로부터 Pin까지 허용 거리", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinDTValue, pInspectROIRgn->m_AlgorithmParam[ii].m_iPinDTValue, "(커넥터 핀)DT 밝기차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinDefectNumberTolerance, pInspectROIRgn->m_AlgorithmParam[ii].m_iPinDefectNumberTolerance, "(커넥터 핀)Pin 불량 개수", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinPitch, pInspectROIRgn->m_AlgorithmParam[ii].m_iPinPitch, "(커넥터 핀)Pin 중심 간격", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDent, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDent, "Dent 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentMedFilterSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iDentMedFilterSize, "(Dent 후보 영역 검출)노이즈 제거 Median 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentDtFilterSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iDentDtFilterSize, "(Dent 후보 영역 검출)DT Mean 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentBrightDtValue, pInspectROIRgn->m_AlgorithmParam[ii].m_iDentBrightDtValue, "(Dent 후보 영역 검출)밝은 Dent 후보 DT 밝기차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentDarkDtValue, pInspectROIRgn->m_AlgorithmParam[ii].m_iDentDarkDtValue, "(Dent 후보 영역 검출)어두운 Dent 후보 DT 밝기차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentCandidateMinArea, pInspectROIRgn->m_AlgorithmParam[ii].m_iDentCandidateMinArea, "(Dent 후보 영역 검출)Dent 후보 최소 면적", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentCandidateMinWidth, pInspectROIRgn->m_AlgorithmParam[ii].m_iDentCandidateMinWidth, "(Dent 후보 영역 검출)Dent 후보 최소 폭", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentCandidateMinDistance, pInspectROIRgn->m_AlgorithmParam[ii].m_iDentCandidateMinDistance, "(Dent 불량 판정)(밝은/어두운) Dent 후보간 거리", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentMinArea, pInspectROIRgn->m_AlgorithmParam[ii].m_iDentMinArea, "(Dent 불량 판정)Dent 최소 면적", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompare, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompare, "영상비교 검사 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bImageCompareSaveFeatureLog, pInspectROIRgn->m_AlgorithmParam[ii].m_bImageCompareSaveFeatureLog, "(영상비교)Feature 로그 저장 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareWindow, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareWindow, "(영상비교)최근 N개 학습", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareWindowSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareWindowSize, "(영상비교)Window Size (N)", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareEdgeImage, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareEdgeImage, "(영상비교)Edge 영상으로 학습", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareEdgeImage, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareNormalizer, "(영상비교)이미지 밝기 보정 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareBrightAbs, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareNormalizeWeight, "(영상비교)이미지 밝기 보정 강도", sFixStringTab, file);			
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareBright, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareBright, "(영상비교)밝은 불량 검출", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareDark, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareDark, "(영상비교)어두운 불량 검출", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareBrightAbs, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareBrightAbs, "(영상비교/밝은 불량)절대 밝기차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareDarkAbs, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareDarkAbs, "(영상비교/어두운 불량)절대 밝기차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareBrightVar, pInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareBrightVar, "(영상비교/밝은 불량)V * 표준편차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareDarkVar, pInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareDarkVar, "(영상비교/어두운 불량)V * 표준편차", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareAutoTrain, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareAutoTrain, "(영상비교)검사결과 양품 시 자동 학습 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareMod, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareMod, "영상비교 영역 조정 후 검사/영상비교 검사 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareModBright, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareModBright, "영상비교 영역 조정 후 검사/(영상비교)밝은 불량 검출", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareModDark, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareModDark, "영상비교 영역 조정 후 검사/(영상비교)어두운 불량 검출", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModBrightAbs, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModBrightAbs, "영상비교 영역 조정 후 검사/(영상비교/밝은 불량)절대 밝기차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModDarkAbs, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModDarkAbs, "영상비교 영역 조정 후 검사/(영상비교/어두운 불량)절대 밝기차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareModBrightVar, pInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareModBrightVar, "영상비교 영역 조정 후 검사/(영상비교/밝은 불량)V * 표준편차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareModDarkVar, pInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareModDarkVar, "영상비교 영역 조정 후 검사/(영상비교/어두운 불량)V * 표준편차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModTop, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModTop, "영상비교 영역 조정 후 검사/(외곽 비검사 마진)Top", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModBottom, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModBottom, "영상비교 영역 조정 후 검사/(외곽 비검사 마진)Top", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModLeft, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModLeft, "영상비교 영역 조정 후 검사/(외곽 비검사 마진)Top", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModRight, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModRight, "영상비교 영역 조정 후 검사/(외곽 비검사 마진)Top", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUsePartCheck, pInspectROIRgn->m_AlgorithmParam[ii].m_bUsePartCheck, "(Part 유무체크)Part 유무체크 검사 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckMultipleModel, pInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckMultipleModel, "(Part 유무체크)멀티 모듈 각각 모델 생성 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingAngleRange, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingAngleRange, "(Part 유무체크)Part 유무체크 각도 범위", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingPLevel, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingPLevel, "(Part 유무체크)매칭 이미지 축소 단계", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingScore, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingScore, "(Part 유무체크)Part 유무 판정 매칭률", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginX, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginX, "(Part 유무체크)Left 매칭 Search 마진", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginY, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginY, "(Part 유무체크)Top 매칭 Search 마진", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginX2, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginX2, "(Part 유무체크)Right 매칭 Search 마진", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginY2, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginY2, "(Part 유무체크)Bottom 매칭 Search 마진", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckInspectExistence, pInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckInspectExistence, "(Part 유무체크)Part 유무 검사 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckBlob, pInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckBlob, "(Part 유무체크)Blob 분석 알고리즘으로 Part 유무 사전 체크 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckInspectShift, pInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckInspectShift, "(Part 유무체크)Shift 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckShiftX, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckShiftX, "(Part 유무체크)X 위치 Shift 허용 오차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckShiftY, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckShiftY, "(Part 유무체크)Y 위치 Shift 허용 오차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dPartCheckRotationAngle, pInspectROIRgn->m_AlgorithmParam[ii].m_dPartCheckRotationAngle, "(Part 유무체크)Angle 허용 오차", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckLocalAlignUse, pInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckLocalAlignUse, "(Part 유무체크)Local Align 데이터 계산", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckApplyComp, pInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckApplyComp, "(Part 유무체크)Leg Shift 보상식 적용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckMeasureEdge, pInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckMeasureEdge, "(Part 유무체크)매칭 후 Leg Edge 측정 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckLegXSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckLegXSize, "(Part 유무체크)Leg 1개 X방향 길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckLegYSize, pInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckLegYSize, "(Part 유무체크)Leg 1개 Y방향 길이", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dPartCheckEdgeMeasueSmFactor, pInspectROIRgn->m_AlgorithmParam[ii].m_dPartCheckEdgeMeasueSmFactor, "(Part 유무체크)Edge Detection Smoothing Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dPartCheckEdgeMeasueEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_dPartCheckEdgeMeasueEdgeStr, "(Part 유무체크)Edge Strength", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckInspectLegDamage, pInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckInspectLegDamage, "(Part 유무체크)Leg Y방향 Edge 미검출 시 Leg Damage 불량으로 처리", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseConcenter, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseConcenter, "동심도 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConcenterMatchingScore, pInspectROIRgn->m_AlgorithmParam[ii].m_iConcenterMatchingScore, "(동심도 검사)원 검출 최소 매칭율", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConcenterInnerCircleDiameter, pInspectROIRgn->m_AlgorithmParam[ii].m_iConcenterInnerCircleDiameter, "(동심도 검사)원 지름", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterMatchingScaleMin, pInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterMatchingScaleMin, "(동심도 검사)스케일 조정 최소", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterMatchingScaleMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterMatchingScaleMax, "(동심도 검사)스케일 조정 최대", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConcenterMatchingMinContrast, pInspectROIRgn->m_AlgorithmParam[ii].m_iConcenterMatchingMinContrast, "(동심도 검사)Edge Contrast", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterCenterDiffSpecX, pInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterCenterDiffSpecX, "(동심도 검사)X 방향 중심간 편차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterCenterDiffSpecY, pInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterCenterDiffSpecY, "(동심도 검사)Y 방향 중심간 편차", sFixStringTab, file);


				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseHomerMark, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseHomerMark, "Homer Mark 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHomerMarkPos, pInspectROIRgn->m_AlgorithmParam[ii].m_iHomerMarkPos, "(Homer Mark)Row / Column 지정", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHomerMarkOrder, pInspectROIRgn->m_AlgorithmParam[ii].m_iHomerMarkOrder, "(Homer Mark)코드 순서 지정", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseROIAlign, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseROIAlign, "ROI Align 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignXSearchMargin, pInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignXSearchMargin, "(ROI Align)X 방향 이동 범위", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignYSearchMargin, pInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignYSearchMargin, "(ROI Align)Y 방향 이동 범위", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignMatchingAngleRange, pInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignMatchingAngleRange, "(ROI Align)각도 범위", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignMatchingScaleMin, pInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignMatchingScaleMin, "(ROI Align)스케일 조정 최소", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignMatchingScaleMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignMatchingScaleMax, "(ROI Align)스케일 조정 최대", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignMatchingMinContrast, pInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignMatchingMinContrast, "(ROI Align)Edge Contrast", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignUsePosX, pInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignUsePosX, "(ROI Align)X 방향 적용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignUsePosY, pInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignUsePosY, "(ROI Align)Y 방향 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignMatchingScore, pInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignMatchingScore, "(ROI Align)경계 매칭률", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignInspectShift, pInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignInspectShift, "(ROI Align)Shift 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignMatchingPLevel, pInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignMatchingPLevel, "(ROI Align)매칭 이미지 축소 단계", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftTop, pInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftTop, "(ROI Align)Top 위치 Shift 허용 오차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftBottom, pInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftBottom, "(ROI Align)Bottom 위치 Shift 허용 오차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftLeft, pInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftLeft, "(ROI Align)Left 위치 Shift 허용 오차", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftRight, pInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftRight, "(ROI Align)Right 위치 Shift 허용 오차", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignLocalAlignUse, pInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignLocalAlignUse, "(ROI Align)Local Align 데이터 계산", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseIsoColorImage, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseIsoColorImage, "(ROI Align)Color 영상 매칭 적용", sFixStringTab, file);

				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iProcessChType, pInspectROIRgn->m_AlgorithmParam[ii].m_iProcessChType, "알고리즘 처리 영상 선택", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iProcessResType, pInspectROIRgn->m_AlgorithmParam[ii].m_iProcessResType, "알고리즘 처리 영상 해상도 변경", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcess, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcess, "영상 전처리 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticType, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticType, "(영상 전처리)2개 영상 연산 타입", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage1, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage1, "(영상 전처리)Image 1", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage2, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage2, "(영상 전처리)Image 2", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage3, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage3, "(영상 전처리)Image 3", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage4, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage4, "(영상 전처리)Image 4", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dImageProcessArithmeticMulti, pInspectROIRgn->m_AlgorithmParam[ii].m_dImageProcessArithmeticMulti, "(영상 전처리)Multi", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticAdd, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticAdd, "(영상 전처리)Add", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessFilter, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessFilter, "필터링 전처리 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType1, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType1, "(영상 전처리)Filter 1", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType2, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType2, "(영상 전처리)Filter 2", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType3, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType3, "(영상 전처리)Filter 3", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType1X, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType1X, "(영상 전처리)Filter 1 Width", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType2X, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType2X, "(영상 전처리)Filter 2 Width", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType3X, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType3X, "(영상 전처리)Filter 3 Width", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType1Y, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType1Y, "(영상 전처리)Filter 1 Height", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType2Y, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType2Y, "(영상 전처리)Filter 2 Height", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType3Y, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType3Y, "(영상 전처리)Filter 3 Height", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessLocalAlign, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessLocalAlign, "(영상 전처리/Local Align)Local Align 연결 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignImageIndex, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignImageIndex, "(영상 전처리/Local Align)영상 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignROINo, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignROINo, "(영상 전처리/Local Align)ROI 번호", sFixStringTab, file);

				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessPostLocalAlignROINo, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessPostLocalAlignROINo, "(영상 전처리/PostLocal Align)ROI 번호", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessPostLocalAlign, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessPostLocalAlign, "(영상 전처리/PostLocal Align)Local Align 연결 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessPostLocalAlignImageIndex, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessPostLocalAlignImageIndex, "(영상 전처리/PostLocal Align)영상 번호", sFixStringTab, file);

				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignROIType, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignROIType, "(영상 전처리/Local Align)Local Align 타입 설정", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignMatchingApplyMethod, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignMatchingApplyMethod, "(영상 전처리/Local Align)ROI Align 결과 적용 방법", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bImageProcessLocalAlignPosX, pInspectROIRgn->m_AlgorithmParam[ii].m_bImageProcessLocalAlignPosX, "(영상 전처리/Local Align)Delta X 적용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bImageProcessLocalAlignPosY, pInspectROIRgn->m_AlgorithmParam[ii].m_bImageProcessLocalAlignPosY, "(영상 전처리/Local Align)Delta Y 적용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bImageProcessLocalAlignAngle, pInspectROIRgn->m_AlgorithmParam[ii].m_bImageProcessLocalAlignAngle, "(영상 전처리/Local Align)Angle 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignFitPos, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignFitPos, "(영상 전처리/Local Align)Fitting 라인에 맞출 ROI 위치 선택", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessPostLocalAlignFitPos, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessPostLocalAlignFitPos, "(영상 전처리/Local Align)로컬얼라인 회전중심", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessDontCare, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessDontCare, "(영상 전처리/검사제외 영역 #1)검사제외 영역 #1 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareImageIndex, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareImageIndex, "(영상 전처리/검사제외 영역 #1)영상 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareROINo, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareROINo, "(영상 전처리/검사제외 영역 #1)ROI 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMargin, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMargin, "(영상 전처리/검사제외 영역 #1)마진1", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMarginInner, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMarginInner, "(영상 전처리/검사제외 영역 #1)마진2", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessDontCare2, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessDontCare2, "(영상 전처리/검사제외 영역 #2)검사제외 영역 #2 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareImageIndex2, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareImageIndex2, "(영상 전처리/검사제외 영역 #2)영상 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareROINo2, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareROINo2, "(영상 전처리/검사제외 영역 #2)ROI 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMargin2, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMargin2, "(영상 전처리/검사제외 영역 #2)마진1", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMargin2Inner, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMargin2Inner, "(영상 전처리/검사제외 영역 #2)마진2", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessGenerate, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessGenerate, "(영상 전처리/검사영역 생성)검사영역 생성 적용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessGenerateInspectFlag, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessGenerateInspectFlag, "(영상 전처리/검사영역 생성)검사영역이 생성될 경우만 원본 ROI 검사", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessGenerateImageIndex, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessGenerateImageIndex, "(영상 전처리/검사영역 생성)영상 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessGenerateROINo, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessGenerateROINo, "(영상 전처리/검사영역 생성)ROI 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessGenerateMargin, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessGenerateMargin, "(영상 전처리/검사영역 생성)검사영역 생성 확장 마진", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessMask, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessMask, "(영상 전처리/검사결과 제외 1st)검사결과 제외 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskImageIndex, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskImageIndex, "(영상 전처리/검사결과 제외 1st)영상 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskROINo, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskROINo, "(영상 전처리/검사결과 제외 1st)ROI 번호", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessMask2, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessMask2, "(영상 전처리/검사결과 제외 2nd)검사결과 제외 적용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskImageIndex2, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskImageIndex2, "(영상 전처리/검사결과 제외 2nd)영상 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskROINo2, pInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskROINo2, "(영상 전처리/검사결과 제외 2nd)ROI 번호", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseShape, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseShape, "요철 영상 생성 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iShapeIlluminationFilterX, pInspectROIRgn->m_AlgorithmParam[ii].m_iShapeIlluminationFilterX, "(요철 영상)필터 X 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iShapeIlluminationFilterY, pInspectROIRgn->m_AlgorithmParam[ii].m_iShapeIlluminationFilterY, "(요철 영상)필터 Y 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeIlluminationScaleFactor, pInspectROIRgn->m_AlgorithmParam[ii].m_dShapeIlluminationScaleFactor, "(요철 영상)Scale Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeCurvatureSmFactor, pInspectROIRgn->m_AlgorithmParam[ii].m_dShapeCurvatureSmFactor, "(요철 영상)Smoothing Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeScalingMulti, pInspectROIRgn->m_AlgorithmParam[ii].m_dShapeScalingMulti, "(요철 영상)Multi", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeScalingAdd, pInspectROIRgn->m_AlgorithmParam[ii].m_dShapeScalingAdd, "(요철 영상)Add", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeImageReduceRatio, pInspectROIRgn->m_AlgorithmParam[ii].m_dShapeImageReduceRatio, "(요철 영상)요철영상 축소 비율", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeLedAngle, pInspectROIRgn->m_AlgorithmParam[ii].m_dShapeLedAngle, "(요철 영상)조명 각도", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iShapeImageMargin, pInspectROIRgn->m_AlgorithmParam[ii].m_iShapeImageMargin, "(요철 영상)검사영역 마진", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseEdgeMeasure, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseEdgeMeasure, "(Edge 측정)Edge 측정 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureOnePoint, pInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureOnePoint, "(Edge 측정)Edge Point 측정 수량", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureMultiPointNumber, pInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureMultiPointNumber, "(Edge 측정)Multi Points 분할 갯수", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bEdgeMeasureEndPoint, pInspectROIRgn->m_AlgorithmParam[ii].m_bEdgeMeasureEndPoint, "(Edge 측정)Multi Points 분할 영역의 양쪽 끝 영역으로만 Line 생성", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bEdgeMeasureUseMaxMinPoint, pInspectROIRgn->m_AlgorithmParam[ii].m_bEdgeMeasureUseMaxMinPoint, "(Edge 측정)Multi Points 모드 중 최대 또는 최소 2Point만 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureUseMaxPoint, pInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureUseMaxPoint, "(Edge 측정)Multi Points 모드 중 최대 2Point 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureDir, pInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureDir, "(Edge 측정)Edge 측정 방향", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureGv, pInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureGv, "(Edge 측정)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasurePos, pInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasurePos, "(Edge 측정)Edge 위치 선택", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bEdgeMeasureLocalAlignUse, pInspectROIRgn->m_AlgorithmParam[ii].m_bEdgeMeasureLocalAlignUse, "(Edge 측정)Local Align 데이터 계산", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dEdgeMeasureSmFactor, pInspectROIRgn->m_AlgorithmParam[ii].m_dEdgeMeasureSmFactor, "(Edge 측정)Edge Detection Smoothing Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureEdgeStr, "(Edge 측정)Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasurePosOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasurePosOffset, "(Edge 측정)Edge 포인트로부터 좌표 이동", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseHomerTilt, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseHomerTilt, "(Homer Tilt)Homer Tilt 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHomerTiltHomerEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_iHomerTiltHomerEdgeStr, "(Homer Tilt)Homer 라인 Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHomerTiltBenvolioImage, pInspectROIRgn->m_AlgorithmParam[ii].m_iHomerTiltBenvolioImage, "(Homer Tilt)Benvolio 라인 측정 영상 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHomerTiltBenvolioEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_iHomerTiltBenvolioEdgeStr, "(Homer Tilt)Benvolio 라인 Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dHomerTiltHomerLength, pInspectROIRgn->m_AlgorithmParam[ii].m_dHomerTiltHomerLength, "(Homer Tilt)Homer X방향 길이 (픽셀)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dHomerTiltTol, pInspectROIRgn->m_AlgorithmParam[ii].m_dHomerTiltTol, "(Homer Tilt)Homer Tilt 허용 높이 (픽셀)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dHomerTiltDistanceTol, pInspectROIRgn->m_AlgorithmParam[ii].m_dHomerTiltDistanceTol, "(Homer Tilt)Homer / Benvolio 간 허용 거리 (픽셀)", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseEpoxyVoidHole, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseEpoxyVoidHole, "(Epoxy Void Hole)Benvolio Epoxy Void Hole 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleImage, pInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleImage, "(Epoxy Void Hole)Edge 측정 영상 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dEpoxyVoidHoleEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_dEpoxyVoidHoleEdgeStr, "(Epoxy Void Hole)Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleInspectMargin, pInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleInspectMargin, "(Epoxy Void Hole)상/하부 검사영역 마진", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bEpoxyVoidHoleTiltInspect, pInspectROIRgn->m_AlgorithmParam[ii].m_bEpoxyVoidHoleTiltInspect, "(Epoxy Void Hole)좌측 / 우측간 Gap 편차 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleTiltTolerance, pInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleTiltTolerance, "(Epoxy Void Hole)편차 허용치", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bEpoxyVoidHoleGapInspect, pInspectROIRgn->m_AlgorithmParam[ii].m_bEpoxyVoidHoleGapInspect, "(Epoxy Void Hole)Benvolio / HTCC간 Gap 높이 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleGapTolerance, pInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleGapTolerance, "(Epoxy Void Hole)높이 최소 허용치", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUsePrintQuality, pInspectROIRgn->m_AlgorithmParam[ii].m_bUsePrintQuality, "(바코드 품질검사)바코드 품질검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityContrastMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityContrastMin, "(바코드 품질검사)contrast_min", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolColsMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolColsMin, "(바코드 품질검사)symbol_cols_min", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolRowsMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolRowsMin, "(바코드 품질검사)symbol_rows_min", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolColsMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolColsMax, "(바코드 품질검사)symbol_cols_max", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolRowsMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolRowsMax, "(바코드 품질검사)symbol_rows_max", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleSizeMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleSizeMin, "(바코드 품질검사)module_size_min", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleSizeMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleSizeMax, "(바코드 품질검사)module_size_max", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dPrintQualitySlantMax, pInspectROIRgn->m_AlgorithmParam[ii].m_dPrintQualitySlantMax, "(바코드 품질검사)slant_max", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPrintQualityOverallQuality, pInspectROIRgn->m_AlgorithmParam[ii].m_bPrintQualityOverallQuality, "(바코드 품질검사)Overall Quality 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityGradeOverallQuality, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityGradeOverallQuality, "(바코드 품질검사)Overall Quality 품질 불량 판정 Grade", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPrintQualityUnusedErrorCorrection, pInspectROIRgn->m_AlgorithmParam[ii].m_bPrintQualityUnusedErrorCorrection, "(바코드 품질검사)Unused Error Correction 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityGradeUnusedErrorCorrection, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityGradeUnusedErrorCorrection, "(바코드 품질검사)Unused Error Correction 품질 불량 판정 Grade", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityStrictModel, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityStrictModel, "(바코드 품질검사)strict_model", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityPolarity, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityPolarity, "(바코드 품질검사)polarity", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityMirrored, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityMirrored, "(바코드 품질검사)mirrored", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityContrastTolerance, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityContrastTolerance, "(바코드 품질검사)contrast_tolerance", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityStrictQuietZone, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityStrictQuietZone, "(바코드 품질검사)strict_quiet_zone", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolShape, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolShape, "(바코드 품질검사)symbol_shape", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySmallModulesRobustness, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySmallModulesRobustness, "(바코드 품질검사)small_modules_robustness", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleGrid, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleGrid, "(바코드 품질검사)module_grid", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleGapMin, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleGapMin, "(바코드 품질검사)module_gap_min", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleGapMax, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleGapMax, "(바코드 품질검사)module_gap_max", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityFinderPatternTolerance, pInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityFinderPatternTolerance, "(바코드 품질검사)finder_pattern_tolerance", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDistanceMeasure, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDistanceMeasure, "(거리 측정)Y방향 거리 측정 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperGv, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperGv, "(거리 측정/상부)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperPos, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperPos, "(거리 측정/상부)Edge 위치 선택", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureUpperSmFactor, pInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureUpperSmFactor, "(거리 측정/상부)Edge Detection Smoothing Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperEdgeStr, "(거리 측정/상부)Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerGv, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerGv, "(거리 측정/하부)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerPos, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerPos, "(거리 측정/하부)Edge 위치 선택", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureLowerSmFactor, pInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureLowerSmFactor, "(거리 측정/하부)Edge Detection Smoothing Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerEdgeStr, "(거리 측정/하부)Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperPosOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperPosOffset, "(거리 측정/상부)Edge 포인트로부터 좌표 이동", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerPosOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerPosOffset, "(거리 측정/하부)Edge 포인트로부터 좌표 이동", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUsageType, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUsageType, "(거리 측정)검사 타입 (용도)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureSaveID, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureSaveID, "(거리 측정)측정값 저장 ID 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureGapInspectionUpper, pInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureGapInspectionUpper, "(거리 측정)거리 허용 상한", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureGapInspectionLower, pInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureGapInspectionLower, "(거리 측정)거리 허용 하한", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureROIType, pInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureROIType, "(거리 측정)측정 ROI 타입", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseGapMeasure, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseGapMeasure, "(Gap 측정)Gap 측정 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperGv, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperGv, "(Gap 측정/상부)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperPos, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperPos, "(Gap 측정/상부)Edge 위치 선택", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureUpperSmFactor, pInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureUpperSmFactor, "(Gap 측정/상부)Edge Detection Smoothing Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperEdgeStr, "(Gap 측정/상부)Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerGv, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerGv, "(Gap 측정/하부)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerPos, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerPos, "(Gap 측정/하부)Edge 위치 선택", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureLowerSmFactor, pInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureLowerSmFactor, "(Gap 측정/하부)Edge Detection Smoothing Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerEdgeStr, "(Gap 측정/하부)Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperPosOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperPosOffset, "(Gap 측정/상부)Edge 포인트로부터 좌표 이동", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerPosOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerPosOffset, "(Gap 측정/하부)Edge 포인트로부터 좌표 이동", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureSaveID, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureSaveID, "(Gap 측정)측정값 저장 ID 번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureImageNo1, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureImageNo1, "(Gap 측정/상측) 영상번호", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureImageNo2, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureImageNo2, "(Gap 측정/하측) 영상번호", sFixStringTab, file);

				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureGapInspectionUpper, pInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureGapInspectionUpper, "(Gap 측정)Gap 허용 상한", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureGapInspectionLower, pInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureGapInspectionLower, "(Gap 측정)Gap 허용 하한", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureGapMeasureOffset, pInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureGapMeasureOffset, "(Gap 측정)Gap 측정 옵셋", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureDir, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureDir, "(Gap 측정)Gap 측정 방향", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bGapMeasureAlarm, pInspectROIRgn->m_AlgorithmParam[ii].m_bGapMeasureAlarm, "(Gap 측정)불량 판정 시 핸들러 알람 발생 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasurePointNumber, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasurePointNumber, "(Gap 측정)측정 포인트 개수", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureSelectType, pInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureSelectType, "(Gap 측정)측정 포인트 개수가 2개 이상인 경우 측정값 선택", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceDualInspection, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceDualInspection, "(Dual Blob 분석)2개 영상으로부터 Blob 분석 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iSurfaceDualImageIndex_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iSurfaceDualImageIndex_1, "(Dual Blob 분석)영상 번호 1", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iSurfaceDualImageIndex_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iSurfaceDualImageIndex_2, "(Dual Blob 분석)영상 번호 2", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightFixedThres_1, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightFixedThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/고정 Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightLowerThres_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightLowerThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/고정 Threshold 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUpperThres_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUpperThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/고정 Threshold 최대값", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightDT_1, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightDT_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightMedianFilterSize_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightMedianFilterSize_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/노이즈제거 Median 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightClosingFilterSize_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightClosingFilterSize_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/불량 붙임 Closing 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterType_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterType_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/필터 종류", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(X)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize2_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize2_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(Y)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTValue_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTValue_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/DT 밝기차", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDTPolarTrans_1, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDTPolarTrans_1, "(Dual Blob 분석)영상1 Dynamic Threshold 원형 필터링 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightHystThres_1, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightHystThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Hysteresis Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystSecureThres_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystSecureThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Hysteresis Threshold/Secure Threshold", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialThres_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Hysteresis Threshold/Potential Threshold", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialLength_1, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialLength_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Hysteresis Threshold/Hyst Length", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightFixedThres_2, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightFixedThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/고정 Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightLowerThres_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightLowerThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/고정 Threshold 최소값", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUpperThres_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUpperThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/고정 Threshold 최대값", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightDT_2, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightDT_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightMedianFilterSize_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightMedianFilterSize_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/노이즈제거 Median 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightClosingFilterSize_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightClosingFilterSize_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/불량 붙임 Closing 필터 크기", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterType_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterType_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/필터 종류", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(X)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize2_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize2_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(Y)", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTValue_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTValue_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/DT 밝기차", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDTPolarTrans_2, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseDTPolarTrans_2, "(Dual Blob 분석)영상2 Dynamic Threshold 원형 필터링 사용", sFixStringTab, file);
				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightHystThres_2, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightHystThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Hysteresis Threshold 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystSecureThres_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystSecureThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Hysteresis Threshold/Secure Threshold", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialThres_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Hysteresis Threshold/Potential Threshold", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialLength_2, pInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialLength_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Hysteresis Threshold/Hyst Length", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDualBlobProcessType, pInspectROIRgn->m_AlgorithmParam[ii].m_iDualBlobProcessType, "(Dual Blob 분석)영상 1 / 영상 2 후보 Blob 처리 방식", sFixStringTab, file);

				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDnnInspectionType, pInspectROIRgn->m_AlgorithmParam[ii].m_iDnnInspectionType, "(딥러닝 검사)검사 방식 선택", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDnnInspectionModel, pInspectROIRgn->m_AlgorithmParam[ii].m_iDnnInspectionModel, "(딥러닝 검사)딥러닝 기반 검사에 사용할 모델", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseHeightMeasure, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseHeightMeasure, "(높이 측정)높이 측정 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHeightMeasureType, pInspectROIRgn->m_AlgorithmParam[ii].m_iHeightMeasureType, "(높이 측정)측정 타입", sFixStringTab, file);

				CompareNRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBoundaryInspection, pInspectROIRgn->m_AlgorithmParam[ii].m_bUseBoundaryInspection, "(경계 검사)경계 검사 사용", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureDir, pInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureDir, "(경계 검사)Edge 검색 시작 위치", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureGv, pInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureGv, "(경계 검사)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasurePos, pInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasurePos, "(경계 검사)Edge 위치 선택", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dBoundaryEdgeMeasureSmFactor, pInspectROIRgn->m_AlgorithmParam[ii].m_dBoundaryEdgeMeasureSmFactor, "(경계 검사)Edge Detection Smoothing Factor", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dBoundaryEdgeMeasureEdgeStr, pInspectROIRgn->m_AlgorithmParam[ii].m_dBoundaryEdgeMeasureEdgeStr, "(경계 검사)Edge Strength", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureSampling, pInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureSampling, "(경계 검사)Edge 측정 포인트 샘플링", sFixStringTab, file);
				CompareNRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureSensorLength, pInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureSensorLength, "(경계 검사)Edge 검출 센서 길이", sFixStringTab, file);
			}
		}
	}

	// ROI 추가 검색 (Mem 기준 ==> File 검색)

	for (iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			iInspectionROIID = pInspectROIRgn->miInspectionROIID;
			iTeachImageIndex = pInspectROIRgn->miTeachImageIndex;

			bROIFound = FALSE;

			for (iFileIdx = 0; iFileIdx < iNoFileInspectROI; iFileIdx++)
			{
				pFileInspectROIRgn = pFileInspectionArea->GetChildTRegion(iFileIdx);
				if (pFileInspectROIRgn == NULL)
					continue;

				if (pFileInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pFileInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST)
				{
					iFileInspectionROIID = pFileInspectROIRgn->miInspectionROIID;
					iFileTeachImageIndex = pFileInspectROIRgn->miTeachImageIndex;

					if (iFileInspectionROIID == iInspectionROIID && iFileTeachImageIndex == iTeachImageIndex)
					{
						bROIFound = TRUE;
						break;
					}
				}
			}

			if (bROIFound == FALSE)
			{
				// ROI 추가 기록
				sInspectionTypeName = g_strInspectionTypeName[pInspectROIRgn->miInspectionType - 1].c_str();

				pInspectROIRgn->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
				pInspectROIRgn->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

				sROIChange.Format("%s\tROI 추가\t영상 %d\t%s\t(%d,%d)/(%d,%d)\r\n", sBaseVal, pInspectROIRgn->miTeachImageIndex, sInspectionTypeName, LTPoint.x, LTPoint.y, RBPoint.x, RBPoint.y);
				file->Write(sROIChange, sROIChange.GetLength());
			}
		}
	}

	// Compare Condition

	SAFE_DELETE(pFileInspectionArea);
}

void CModelDataManager::INI_CompareNRecord_BOOL(BOOL bNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile* file)
{
	BOOL bPrev;
	bPrev = INI.Get_Bool(sSection, sKey, FALSE);

	CString sPrev, sNow;

	if (bPrev != bNow)
	{
		if (bPrev == TRUE) sPrev = "사용"; else sPrev = "미사용";
		if (bNow == TRUE) sNow = "사용"; else sNow = "미사용";

		CString sFullPath;
		sFullPath.Format("%s\t%s\t%s\t%s\r\n", sFixVal, sParam, sPrev, sNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

void CModelDataManager::INI_CompareNRecord(int iNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile* file)
{
	int iPrev;
	iPrev = INI.Get_Integer(sSection, sKey, 0);

	if (iPrev != iNow)
	{
		CString sFullPath;
		sFullPath.Format("%s\t%s\t%d\t%d\r\n", sFixVal, sParam, iPrev, iNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

void CModelDataManager::INI_CompareNRecord(double dNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile* file)
{
	double dPrev;
	dPrev = INI.Get_Double(sSection, sKey, 0.0);

	if (dPrev != dNow)
	{
		CString sFullPath;
		sFullPath.Format("%s\t%s\t%.3lf\t%.3lf\r\n", sFixVal, sParam, dPrev, dNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

void CModelDataManager::CompareNRecord_BOOL(BOOL bPrev, BOOL bNow, CString sParam, CString sFixVal, CFile* file)
{
	CString sPrev, sNow;

	if (bPrev != bNow)
	{
		if (bPrev == TRUE) sPrev = "사용"; else sPrev = "미사용";
		if (bNow == TRUE) sNow = "사용"; else sNow = "미사용";

		CString sFullPath;
		sFullPath.Format("%s\t%s\t%s\t%s\r\n", sFixVal, sParam, sPrev, sNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

void CModelDataManager::CompareNRecord(int iPrev, int iNow, CString sParam, CString sFixVal, CFile* file)
{
	if (iPrev != iNow)
	{
		CString sFullPath;
		sFullPath.Format("%s\t%s\t%d\t%d\r\n", sFixVal, sParam, iPrev, iNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

void CModelDataManager::CompareNRecord(double dPrev, double dNow, CString sParam, CString sFixVal, CFile* file)
{
	if (dPrev != dNow)
	{
		CString sFullPath;
		sFullPath.Format("%s\t%s\t%.3lf\t%.3lf\r\n", sFixVal, sParam, dPrev, dNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

int CModelDataManager::GetLastPadID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iLastPadID = 0;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			if (pInspectROIRgn->miInspectionROIID > iLastPadID)
			{
				iLastPadID = pInspectROIRgn->miInspectionROIID;
			}
		}
	}

	return iLastPadID;
}

BOOL CModelDataManager::CheckInspectionROIID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			if (pInspectROIRgn->miInspectionROIID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CModelDataManager::GetLastLocalAlignID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iLastLocalAlignID = 0;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
		{
			if (pInspectROIRgn->miLocalAlignID > iLastLocalAlignID)
			{
				iLastLocalAlignID = pInspectROIRgn->miLocalAlignID;
			}
		}
	}

	return iLastLocalAlignID;
}

BOOL CModelDataManager::CheckLocalAlignID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
		{
			if (pInspectROIRgn->miLocalAlignID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CModelDataManager::GetLastDontCareID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iLastDontCareID = 0;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_DONTCARE)
		{
			if (pInspectROIRgn->miDontCareID > iLastDontCareID)
			{
				iLastDontCareID = pInspectROIRgn->miDontCareID;
			}
		}
	}

	return iLastDontCareID;
}

int CModelDataManager::GetLastGenerateID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iLastGenerateID = 0;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_GENERATE)
		{
			if (pInspectROIRgn->miGenerateID > iLastGenerateID)
			{
				iLastGenerateID = pInspectROIRgn->miGenerateID;
			}
		}
	}

	return iLastGenerateID;
}

int CModelDataManager::GetLastMaskID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iLastMaskID = 0;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_MASK)
		{
			if (pInspectROIRgn->miMaskID > iLastMaskID)
			{
				iLastMaskID = pInspectROIRgn->miMaskID;
			}
		}
	}

	return iLastMaskID;
}

BOOL CModelDataManager::CheckDontCareID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_DONTCARE)
		{
			if (pInspectROIRgn->miDontCareID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CModelDataManager::CheckGenerateID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_GENERATE)
		{
			if (pInspectROIRgn->miGenerateID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CModelDataManager::CheckMaskID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_MASK)
		{
			if (pInspectROIRgn->miMaskID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CModelDataManager::GetLastColorInfoID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iLastColorInfoID = 0;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_COLOR)
		{
			if (pInspectROIRgn->miColorInfoID > iLastColorInfoID)
			{
				iLastColorInfoID = pInspectROIRgn->miColorInfoID;
			}
		}
	}

	return iLastColorInfoID;
}

BOOL CModelDataManager::CheckColorInfoID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->miInspectionType == INSPECTION_TYPE_COLOR)
		{
			if (pInspectROIRgn->miColorInfoID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// Point ID
int CModelDataManager::GetLastPointID(int iInspectionType, int iPositionID)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iLastPointID = 0;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miPositionID != iPositionID)
			continue;

		if (pInspectROIRgn->miInspectionType == iInspectionType)
		{
			if (pInspectROIRgn->miPointID > iLastPointID)
			{
				iLastPointID = pInspectROIRgn->miPointID;
			}
		}
	}

	return iLastPointID;
}

BOOL CModelDataManager::CheckPointID(int iInspectionType, int iPositionID, int iCheckNo)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miPositionID != iPositionID)
			continue;

		if (pInspectROIRgn->miInspectionType == iInspectionType)
		{
			if (pInspectROIRgn->miPointID == iCheckNo)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CModelDataManager::UpdatePolygonROIBoundary()
{
	if (m_pInspectionArea == NULL)
		return;

	GTRegion* pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;
	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_bRegionROI)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(pInspectROIRgn->m_HTeachPolygonRgn))
			{
				SmallestRectangle1(pInspectROIRgn->m_HTeachPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);
				LTPoint.x = HlCol1.L();
				LTPoint.y = HlRow1.L();
				RBPoint.x = HlCol2.L();
				RBPoint.y = HlRow2.L();

				pInspectROIRgn->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
				pInspectROIRgn->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
			}
		}
	}
}

void CModelDataManager::SaveTeachingParam(int iVisionNo, CString sSaveFolderName, CString sSaveFileName)
{
	CString strFile;
	strFile.Format("%s\\%s", sSaveFolderName, sSaveFileName);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		AfxMessageBox("CMI_Results 폴더가 생성되지 않았습니다.", MB_ICONERROR | MB_SYSTEMMODAL);
		return;
	}

	try
	{
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave.Format("영상번호\t검사영역위치(LT/RB)\t검사대상\t검사탭\t불량항목\t파라미터명\t파라미터값\r\n");
			file.Write(strSave, strSave.GetLength());
		}

		WriteParameter(iVisionNo, &file);

		/************* 파일 닫기 **************/
		file.Close();

	}
	catch (CFileException* pEx) {
		pEx->Delete();
	}
}

void CModelDataManager::SaveAiRetrainRoi(CString sSaveFileName)
{
	if (m_pInspectionArea == NULL)
		return;

	CString sSaveRootFolderName = THEAPP.Struct_PreferenceStruct.m_strEtcFolderPath + "CMI_Results\\AiRetrainRoi";

	CString strFile;
	strFile.Format("%s\\%s", sSaveRootFolderName, sSaveFileName);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		AfxMessageBox("CMI_Results 폴더가 생성되지 않았습니다.", MB_ICONERROR | MB_SYSTEMMODAL);
		return;
	}

	GTRegion* pInspectROIRgn;

	try
	{
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave.Format("영상번호\tRoiNo\r\n");
			file.Write(strSave, strSave.GetLength());
		}

		int iNoInspectROI = 0;
		iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

		for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
		{
			pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST)
			{
				if (pInspectROIRgn->mbAiRetrainFlag)
				{
					strSave.Format("%d\t%d\r\n", pInspectROIRgn->miTeachImageIndex, pInspectROIRgn->miInspectionROIID);
					file.Write(strSave, strSave.GetLength());
				}
			}
		}

		/************* 파일 닫기 **************/
		file.Close();

	}
	catch (CFileException* pEx) {
		pEx->Delete();
	}
}

void CModelDataManager::WriteParameter(int iVisionNo, CFile* file)
{
	if (m_sModelName == ".")
		return;

	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	CString strSwModelFolder;
	GTRegion* pFileInspectionArea = NULL;

	strSwModelFolder.Format("%s\\SW\\Vision_N%d", strModelFolder, m_iModelIdx + 1);

	SetFolderFile(strSwModelFolder, "InspectParam.dat");

	if (!m_bOpened)
		OpenDataFile();

	if (m_bOpened)
	{
		SAFE_DELETE(pFileInspectionArea);
		pFileInspectionArea = new GTRegion;

		CCalDataService* pCalDataService;

		if (m_iModelIdx == VISION_NUMBER_1)
			pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_1];
		else if (m_iModelIdx == VISION_NUMBER_2)
			pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_2];
		else if (m_iModelIdx == VISION_NUMBER_3)
			pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_3];
		else if (m_iModelIdx == VISION_NUMBER_4)
			pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_4];

		HTuple HFileHandle = -1;
		CString szTemplateModelFilename;
		szTemplateModelFilename = strSwModelFolder + "\\InspectTemplate.dat";

		HANDLE				hFindFile;
		WIN32_FIND_DATA		FindFileData;

		if ((hFindFile = FindFirstFile(szTemplateModelFilename, &FindFileData)) != INVALID_HANDLE_VALUE)
		{
			OpenFile(HTuple(szTemplateModelFilename), "input_binary", &HFileHandle);
			FindClose(hFindFile);
		}

		CString strConfig = m_sModelName.Mid(m_sModelName.Find('_') + 1);
		strConfig = strConfig.Mid(strConfig.Find('_') + 1);
		strConfig = strConfig.Mid(strConfig.Find('_') + 1);
		strConfig = strConfig.Mid(0, strConfig.Find('_'));

		pFileInspectionArea->Load((HANDLE)m_File.m_hFile, m_iModelVersion, HFileHandle, pCalDataService, m_iModelIdx, strConfig);

		if (HFileHandle != -1)
			CloseFile(HFileHandle);

		CloseDataFile();
	}

	// Write ROI / Param

	CCalDataService* pCalDataService;

	if (iVisionNo == VISION_NUMBER_1)
		pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_1];
	else if (iVisionNo == VISION_NUMBER_2)
		pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_2];
	else if (iVisionNo == VISION_NUMBER_3)
		pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_3];
	else if (iVisionNo == VISION_NUMBER_4)
		pCalDataService = THEAPP.m_pCalDataService_N[VISION_NUMBER_4];

	GTRegion* pFileInspectROIRgn;

	int iNoFileInspectROI = 0;
	iNoFileInspectROI = pFileInspectionArea->GetChildTRegionCount();

	int iFileIdx;
	POINT LTPoint, RBPoint;
	CString sDefectName;

	CString sFixString = _T("");
	CString sFixStringTab = _T("");

	CString sInspectionTypeName;

	for (iFileIdx = 0; iFileIdx < iNoFileInspectROI; iFileIdx++)
	{
		pFileInspectROIRgn = pFileInspectionArea->GetChildTRegion(iFileIdx);
		if (pFileInspectROIRgn == NULL)
			continue;

		if (pFileInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pFileInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			pFileInspectROIRgn->GetLTPoint(&LTPoint, pCalDataService);
			pFileInspectROIRgn->GetRBPoint(&RBPoint, pCalDataService);

			sInspectionTypeName = g_strInspectionTypeName[pFileInspectROIRgn->miInspectionType - 1].c_str();

			sFixString.Format("%d\t(%d,%d)/(%d,%d)\t%s", pFileInspectROIRgn->miTeachImageIndex, LTPoint.x, LTPoint.y, RBPoint.x, RBPoint.y, sInspectionTypeName);

			int iSaveTabNo = 3;
			for (int ii = 0; ii < MAX_INSPECTION_TAB; ii++)
			{
				if (pFileInspectROIRgn->m_AlgorithmParam[ii].m_bInspect == FALSE && pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter1 == FALSE)
					continue;

				sDefectName = THEAPP.m_strDefectName[pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectType];

				sFixStringTab.Format("%s\t%d\t%s", sFixString, ii + 1, sDefectName);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter1, "1차망 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter2, "2차망 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter3, "3차망 사용", sFixStringTab, file);
				//WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFilter4, "SVM 사용", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bCheckAIUseConsiderImageNo, "AI 이미지 번호 고려", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bCheckAIUseConsiderDefectCode, "AI 결함 코드 고려", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseInspectArea[0], "검사영역1 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour1Margin[0], "검사영역1 마진1", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour2Margin[0], "검사영역1 마진2", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseInspectArea[1], "검사영역2 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour1Margin[1], "검사영역2 마진1", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour2Margin[1], "검사영역2 마진2", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseInspectArea[2], "검사영역3 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour1Margin[2], "검사영역3 마진1", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iInspectAreaContour2Margin[2], "검사영역3 마진2", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceInspection, "표면검사 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceMeasureSaveLog, "면적, X방향 길이, Y방향 길이 로그 저장", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceInspectEachROI, "분할 ROI 개별 검사", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iSurfaceInspectEachROINgNumber, "분할 ROI 개별 검사/불량 ROI 개수", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bApplyCircleFitting, "Circle Fitting 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iCircleFittingEdgeStr, "Circle Fitting 적용 / Edge Threshold", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iCircleFittingEdgeMargin, "Circle Fitting Edge추출 Margin", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDTPolarTrans, "Dynamic Threshold 원형 필터링 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnected, "고정 Threshold 결과와 연결된 Blob만 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnectedArea, "고정 Threshold 검출 조건/면적 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnectedLength, "고정 Threshold 검출 조건/길이 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseFTConnectedWidth, "고정 Threshold 검출 조건/폭 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iFTConnectedAreaMin, "고정 Threshold 검출 조건/최소면적", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iFTConnectedLengthMin, "고정 Threshold 검출 조건/최소길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iFTConnectedWidthMin, "고정 Threshold 검출 조건/최소폭", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBarcodeBlob, "바코드 인식 영역을 Blob으로 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBlobCornerMeasure, "전체 Blob을 사각형 변환 후 코너 위치 측정", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSaveBNEOverflowTop, "Blob분석/상단 BNE Overflow 길이 측정값 저장", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSaveBNEOverflowBottom, "Blob분석/하단 BNE Overflow 길이 측정값 저장", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightFixedThres, "밝은 불량 검출/고정 Threshold 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightDT, "밝은 불량 검출/Dynamic Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightMedianFilterSize, "밝은 불량 검출/Dynamic Threshold/노이즈제거 Median 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightClosingFilterSize, "밝은 불량 검출/Dynamic Threshold/불량 붙임 Closing 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterType, "밝은 불량 검출/Dynamic Threshold/필터 종류", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize, "밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(X)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize2, "밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(Y)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTValue, "밝은 불량 검출/Dynamic Threshold/DT 밝기차", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightUniformityCheck, "밝은 불량 검출/균일도 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityPeakMin, "밝은 불량 검출/균일도 검사/Peak검출범위 최소", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityPeakMax, "밝은 불량 검출/균일도 검사/Peak검출범위 최대", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityOffset, "밝은 불량 검출/균일도 검사/히스토그램 옵셋", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityHystLength, "밝은 불량 검출/균일도 검사/Hyst Length", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUniformityHystOffset, "밝은 불량 검출/균일도 검사/Hyst 히스토그램 옵셋", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightHystThres, "밝은 불량 검출/Hysteresis Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystSecureThres, "밝은 불량 검출/Hysteresis Threshold/Secure Threshold", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialThres, "밝은 불량 검출/Hysteresis Threshold/Potential Threshold", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialLength, "밝은 불량 검출/Hysteresis Threshold/Hyst Length", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkFixedThres, "어두운 불량 검출/고정 Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkLowerThres, "어두운 불량 검출/고정 Threshold 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUpperThres, "어두운 불량 검출/고정 Threshold 최대값", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkDT, "어두운 불량 검출/Dynamic Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkMedianFilterSize, "어두운 불량 검출/Dynamic Threshold/노이즈제거 Median 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkClosingFilterSize, "어두운 불량 검출/Dynamic Threshold/불량 붙임 Closing 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTFilterType, "어두운 불량 검출/Dynamic Threshold/필터 종류", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTFilterSize, "어두운 불량 검출/Dynamic Threshold/DT Mean 필터 크기(X)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTFilterSize2, "어두운 불량 검출/Dynamic Threshold/DT Mean 필터 크기(Y)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkDTValue, "어두운 불량 검출/Dynamic Threshold/DT 밝기차", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkUniformityCheck, "어두운 불량 검출/균일도 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityPeakMin, "어두운 불량 검출/균일도 검사/Peak검출범위 최소", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityPeakMax, "어두운 불량 검출/균일도 검사/Peak검출범위 최대", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityOffset, "어두운 불량 검출/균일도 검사/히스토그램 옵셋", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityHystLength, "어두운 불량 검출/균일도 검사/Hyst Length", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkUniformityHystOffset, "어두운 불량 검출/균일도 검사/Hyst 히스토그램 옵셋", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkHystThres, "어두운 불량 검출/Hysteresis Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkHystSecureThres, "어두운 불량 검출/Hysteresis Threshold/Secure Threshold", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkHystPotentialThres, "어두운 불량 검출/Hysteresis Threshold/Potential Threshold", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDarkHystPotentialLength, "어두운 불량 검출/Hysteresis Threshold/Hyst Length", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightBinThres, "밝은 불량 검출/Binary Threshold 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDarkBinThres, "어두운 불량 검출/Binary Threshold 사용", sFixStringTab, file);

				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iCandidateMinArea, "(후처리)최소면적", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bCombineBrightDarkBlob, "밝은 불량과 어두운 불량을 합친 후 후처리", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bMaxBlob, "최대 크기만 선택", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bClosingOpening, "Closing->Opening", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bApplyOneDirMorp, "단일방향에만 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iOneDirMorpXOpening, "X방향 Opening", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iOneDirMorpXClosing, "X방향 Closing", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iThresholdOpeningSize, "Threshold Opening", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iThresholdClosingSize, "Threshold Closing", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iOpeningSize, "Opening", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iClosingSize, "Closing", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bFillup, "내부 채움", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseConvex, "Convex 변환", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSubConvex, "Convex 형상 대비 차(Difference) 영역 추출", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseRect, "사각형 변환", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseConnection, "불량 연결 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionMinSize, "연결 후보 최소 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionMinXLength, "(연결 조건)X방향길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionLength, "연결할 후보간 거리", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseAngle, "연결 후보간 각도 정보 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConnectionMaxWidth, "(불량 연결)스크래치 최대 폭", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHoleFillCondition, "내부 채움 조건", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iFillHoleArea, "Hole 면적 이하", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bXYLengthMeasureIgnoreHole, "길이 측정 시 내부 Hole 무시", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bXYLengthMaxLineLength, "최대길이를 라인 별로 측정", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryConnectionAny, "(불량검출조건)ROI 외측(내측) 기준선과 연결 체크", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryConnectionOuter, "(불량검출조건)ROI 외측(내측) 기준선과 연결 체크/최외측 내측 선택", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginAny, "(불량검출조건)ROI 외측(내측) 기준선과 연결 체크/기준선 마진", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginMinConnectionAny, "(불량검출조건)ROI 외측(내측) 기준선과 연결 체크/최소 연결 픽셀수", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageScaling, "(영상 전처리)Contrast 향상 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageScaleMethod, "(영상 전처리)자동변환", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageScaleMin, "(영상 전처리)고정 GV 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageScaleMax, "(영상 전처리)고정 GV 최대값", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseEdgeImage, "(영상 전처리)Edge 영상 변환 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeFilterType, "(영상 전처리)Edge 필터 방향", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dEdgeImageScale, "(영상 전처리)Edge Scale", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUsePeak, "(대표 밝기 체크)사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iNormalPeakThresMin, "(대표 밝기 체크)Peak 밝기 검출 범위 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iNormalPeakThresMax, "(대표 밝기 체크)Peak 밝기 검출 범위 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iAbnormalPeakHistogramOffset, "(대표 밝기 체크)비정상 Peak 불량 히스토그램 옵셋", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionArea, "(불량검출조건)면적", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionLength, "(불량검출조건)길이", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionWidth, "(불량검출조건)폭", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionXLength, "(불량검출조건)X방향길이", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionYLength, "(불량검출조건)Y방향길이", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionMean, "(불량검출조건)평균밝기", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionStdev, "(불량검출조건)밝기표준편차", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionAnisometry, "(불량검출조건)길이/폭 비율", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionCircularity, "(불량검출조건)원형 유사도", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionRectangularity, "(불량검출조건)사각형 유사도", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionConvexity, "(불량검출조건)컨벡스 유사도", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionEllipseRatio, "(불량검출조건)장축/단축 비율", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionAreaRatio, "(불량검출조건)ROI 면적비율", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionContrast, "(불량검출조건)Contrast", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionGrad, "(불량검출조건)경계밝기변화", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionBlob, "(불량검출조건)Blob 갯수", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionShiftX, "(불량검출조건)X 중심 Shift", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionShiftY, "(불량검출조건)Y 중심 Shift", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionOuterDist, "(불량검출조건)외측 기준선으로부터 거리", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionInnerDist, "(불량검출조건)ROI 중심으로부터 거리", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionUseZeroArea, "(불량검출조건)미검출 시 검사 ROI를 불량 영역으로 검출", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryConnection, "(불량검출조건)사각 외측 기준선과 연결 체크", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginTop, "(불량검출조건)사각 외측 기준선과 연결 체크/상", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginBottom, "(불량검출조건)사각 외측 기준선과 연결 체크/하", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginLeft, "(불량검출조건)사각 외측 기준선과 연결 체크/좌", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginRight, "(불량검출조건)사각 외측 기준선과 연결 체크/우", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginHCenter, "(불량검출조건)사각 외측 기준선과 연결 체크/수평중심", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryMarginVCenter, "(불량검출조건)사각 외측 기준선과 연결 체크/수직중심", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLengthWidthCondition, "(불량검출조건)길이, 폭 And/Or 조건", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iXYLengthCondition, "(불량검출조건)X방향길이, Y방향길이 And/Or 조건", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionBoundaryConnection2Side, "(불량검출조건)외측 기준선과 2면이상 연결", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryConnectionCondition, "(불량검출조건)사각 외측 기준선과 연결 체크 And/Or 조건", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionInnerOuterBoundaryConnection, "(불량검출조건)내측 기준선과 외측 기준선에 모두 연결", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iXLengthCondition, "(불량검출조건)X방향길이/길이측정기준", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iXLengthConditionMinRef, "(불량검출조건)X방향길이/최소길이 측정 시 기준면", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iYLengthCondition, "(불량검출조건)Y방향길이/길이측정기준", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iYLengthConditionMinRef, "(불량검출조건)Y방향길이/최소길이 측정 시 기준면", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionHole, "(불량검출조건)Hole 검출", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionHoleArea, "(불량검출조건)Hole 검출면적", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionUnionBlob, "(불량검출조건)1개 Blob으로 합친 후 불량 검출 조건 적용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionDilation, "(불량검출조건)검출 영역 +확대 / -축소", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionDilationSize, "(불량검출조건)검출 영역 +확대 / -축소 사이즈", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionOuterDiff, "(불량검출조건)최외곽 근방 검증", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionOuterDiffInterval, "(불량검출조건)최외곽 근방 검증 간격", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionOuterDiffMinDiff, "(불량검출조건)최외곽 근방 검증 최소높이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nOuterDiffUpper, "(불량검출조건)최외곽 근방 검증 방향", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionOuterDiffMaxRepeat, "(불량검출조건)최외곽 근방 검증 반복수", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionConerLengthUse, "(불량검출조건))코너길이 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionConerLengthDir, "(불량검출조건)코너길이 방향", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionConerLengthMin, "(불량검출조건)코너 최소 길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_nDefectConditionConerLengthMax, "(불량검출조건)코너 최대 길이", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bDefectConditionConerLengthMaxUse, "(불량검출조건))코너길이 최대길이사용", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionXLengthSum, "(불량검출조건)합산 X방향 길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthSumMin, "(불량검출조건)합산 X방향 길이 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthSumMax, "(불량검출조건)합산 X방향 길이 최대값", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDefectConditionYLengthSum, "(불량검출조건)합산 Y방향 길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthSumMin, "(불량검출조건)합산 Y방향 길이 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthSumMax, "(불량검출조건)합산 Y방향 길이 최대값", sFixStringTab, file);

				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaMin, "(불량검출조건)면적 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionLengthMin, "(불량검출조건)길이 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionWidthMin, "(불량검출조건)폭 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthMin, "(불량검출조건)X방향길이 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthMin, "(불량검출조건)Y방향길이 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionMeanMin, "(불량검출조건)평균밝기 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionStdevMin, "(불량검출조건)밝기표준편차 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionAnisometryMin, "(불량검출조건)길이/폭 비율 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionCircularityMin, "(불량검출조건)원형 유사도 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionRectangularityMin, "(불량검출조건)사각형 유사도 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionConvexityMin, "(불량검출조건)컨벡스 유사도 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionEllipseRatioMin, "(불량검출조건)장축/단축 비율 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaRatioMin, "(불량검출조건)ROI 면적비율 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionContrastMin, "(불량검출조건)Contrast 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionGradMin, "(불량검출조건)경계밝기변화 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBlobMin, "(불량검출조건)Blob 갯수 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftXMin, "(불량검출조건)X 중심 Shift 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftYMin, "(불량검출조건)Y 중심 Shift 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionOuterDist, "(불량검출조건)외측 기준선으로부터 거리 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionInnerDist, "(불량검출조건)ROI 중심으로부터 거리 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginTop, "(불량검출조건)사각 외측 기준선과 연결 체크/상 기준선 마진", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginBottom, "(불량검출조건)사각 외측 기준선과 연결 체크/하 기준선 마진", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginLeft, "(불량검출조건)사각 외측 기준선과 연결 체크/좌 기준선 마진", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginRight, "(불량검출조건)사각 외측 기준선과 연결 체크/우 기준선 마진", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginHCenter, "(불량검출조건)사각 외측 기준선과 연결 체크/수평중심 기준선 마진", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginVCenter, "(불량검출조건)사각 외측 기준선과 연결 체크/수직중심 기준선 마진", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBoundaryMarginMinConnection, "(불량검출조건)사각 외측 기준선과 연결 체크/최소 연결 픽셀수", sFixStringTab, file);

				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaMax, "(불량검출조건)면적 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionLengthMax, "(불량검출조건)길이 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionWidthMax, "(불량검출조건)폭 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionXLengthMax, "(불량검출조건)X방향길이 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionYLengthMax, "(불량검출조건)Y방향길이 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionMeanMax, "(불량검출조건)평균밝기 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionStdevMax, "(불량검출조건)밝기표준편차 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionAnisometryMax, "(불량검출조건)길이/폭 비율 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionCircularityMax, "(불량검출조건)원형 유사도 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionRectangularityMax, "(불량검출조건)사각형 유사도 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionConvexityMax, "(불량검출조건)컨벡스 유사도 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDefectConditionEllipseRatioMax, "(불량검출조건)장축/단축 비율 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionAreaRatioMax, "(불량검출조건)ROI 면적비율 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionContrastMax, "(불량검출조건)Contrast 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionGradMax, "(불량검출조건)경계밝기변화 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionBlobMax, "(불량검출조건)Blob 갯수 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftXMax, "(불량검출조건)X 중심 Shift 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDefectConditionShiftYMax, "(불량검출조건)Y 중심 Shift 최대값", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseLineFitting, "Line Fitting 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseOtherImageFitting, "(Line Fitting 알고리즘)다른 조명 영상 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitOtherImageNumber, "(Line Fitting 알고리즘)영상 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitEdgeMethod, "(Line Fitting 알고리즘)Zero Crossing", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitFittingEdgeSF, "(Line Fitting 알고리즘)Edge Detection Smoothing Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitFittingEdgeHigh, "(Line Fitting 알고리즘)Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitFittingMinContourLength, "(Line Fitting 알고리즘)최소 Contour 길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitFittingContourConnectionLength, "(Line Fitting 알고리즘)Contour 연결 길이", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseRotationInspection, "(Line Fitting 알고리즘)회전 각도 계산", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitRotationReference, "(Line Fitting 알고리즘)수평 라인", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitRotationDefectAngle, "(Line Fitting 알고리즘)회전 불량 각도", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitObjectLowerThres, "(Chipping/Bur 불량검출)Object 검출/고정 Threshold 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitObjectUpperThres, "(Chipping/Bur 불량검출)Object 검출/고정 Threshold 최대값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitObjectEdgeSF, "(Chipping/Bur 불량검출)Edge Detection Smoothing Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitEdgeHigh, "(Chipping/Bur 불량검출)Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitMinContourLength, "(Chipping/Bur 불량검출)최소 Contour 길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iLineFitContourConnectionLength, "(Chipping/Bur 불량검출)Contour 연결 길이", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseBurInspection, "(Chipping/Bur 불량검출)Bur 검출", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitBurDefectLength, "(Chipping/Bur 불량검출)Bur 불량 검출/Fitting Line으로부터 길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitBurDefectLengthMax, "(Chipping/Bur 불량검출)Bur 불량 검출/길이 상한", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitUseChippingInspection, "(Chipping/Bur 불량검출)Chipping 검출", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitChippingDefectLength, "(Chipping/Bur 불량검출)Chipping 불량 검출/Fitting Line으로부터 길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dLineFitChippingDefectLengthMax, "(Chipping/Bur 불량검출)Chipping 불량 검출/길이 상한", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bNoUseLineFit, "(Line Fitting 알고리즘)Contour 끝점을 Fitting 라인으로 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bLineFitLocalAlignUse, "(Line Fitting 알고리즘)Local Align 데이터 계산", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseOCR, "OCR 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_lHorizontalDivide, "(OCR)가로 분할 평균 밝기값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_lSecondDifferent, "(OCR)세로 분할 평균 밝기값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_ldiff, "(OCR)Segment erosion", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iOCRSegmentDilation, "(OCR)Segment dilation", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBarcode, "바코드 검사 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bInspectBarcodeShift, "바코드 Shift 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dBarcodeShiftToleranceX, "(바코드)X위치 Shift 허용오차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dBarcodeShiftToleranceY, "(바코드)Y위치 Shift 허용오차", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bInspectBarcodeRotation, "바코드 회전 검사 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBarcodeOtherImage, "인식 실패 시 다른 조명 영상 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBarcodeOtherImageNumber, "(인식 실패 시 다른 조명 영상 사용)영상번호", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUsePinLength, "Connector Pin 휘어짐 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinPos, "(커넥터 핀)Pin 검출 위치", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinDTFilterSize, "(커넥터 핀)DT Mean 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinOpeningClosingSize, "(커넥터 핀)Opening -> Closing Size", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinMinArea, "(커넥터 핀)Pin 최소 면적", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinDistanceTolerance, "(커넥터 핀)기준선으로부터 Pin까지 허용 거리", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinDTValue, "(커넥터 핀)DT 밝기차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinDefectNumberTolerance, "(커넥터 핀)Pin 불량 개수", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPinPitch, "(커넥터 핀)Pin 중심 간격", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDent, "Dent 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentMedFilterSize, "(Dent 후보 영역 검출)노이즈 제거 Median 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentDtFilterSize, "(Dent 후보 영역 검출)DT Mean 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentBrightDtValue, "(Dent 후보 영역 검출)밝은 Dent 후보 DT 밝기차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentDarkDtValue, "(Dent 후보 영역 검출)어두운 Dent 후보 DT 밝기차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentCandidateMinArea, "(Dent 후보 영역 검출)Dent 후보 최소 면적", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentCandidateMinWidth, "(Dent 후보 영역 검출)Dent 후보 최소 폭", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentCandidateMinDistance, "(Dent 불량 판정)(밝은/어두운) Dent 후보간 거리", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDentMinArea, "(Dent 불량 판정)Dent 최소 면적", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompare, "영상비교 검사 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bImageCompareSaveFeatureLog, "(영상비교)Feature 로그 저장 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareWindow, "(영상비교)최근 N개 학습", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareWindowSize, "(영상비교)Window Size (N)", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareEdgeImage, "(영상비교)Edge 영상으로 학습", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareNormalizer, "(영상비교)이미지 밝기 보정 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareNormalizeWeight, "(영상비교)이미지 밝기 보정 강도", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareBright, "(영상비교)밝은 불량 검출", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareDark, "(영상비교)어두운 불량 검출", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareBrightAbs, "(영상비교/밝은 불량)절대 밝기차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareDarkAbs, "(영상비교/어두운 불량)절대 밝기차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareBrightVar, "(영상비교/밝은 불량)V * 표준편차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareDarkVar, "(영상비교/어두운 불량)V * 표준편차", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareAutoTrain, "(영상비교)검사결과 양품 시 자동 학습 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareMod, "영상비교 영역 조정 후 검사/영상비교 검사 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareModBright, "영상비교 영역 조정 후 검사/(영상비교)밝은 불량 검출", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompareModDark, "영상비교 영역 조정 후 검사/(영상비교)어두운 불량 검출", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModBrightAbs, "영상비교 영역 조정 후 검사/(영상비교/밝은 불량)절대 밝기차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModDarkAbs, "영상비교 영역 조정 후 검사/(영상비교/어두운 불량)절대 밝기차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareModBrightVar, "영상비교 영역 조정 후 검사/(영상비교/밝은 불량)V * 표준편차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareModDarkVar, "영상비교 영역 조정 후 검사/(영상비교/어두운 불량)V * 표준편차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModTop, "영상비교 영역 조정 후 검사/(외곽 비검사 마진)Top", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModBottom, "영상비교 영역 조정 후 검사/(외곽 비검사 마진)Bottom", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModLeft, "영상비교 영역 조정 후 검사/(외곽 비검사 마진)Left", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareModRight, "영상비교 영역 조정 후 검사/(외곽 비검사 마진)Right", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUsePartCheck, "(Part 유무체크)Part 유무체크 검사 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckMultipleModel, "(Part 유무체크)멀티 모듈 각각 모델 생성 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingAngleRange, "(Part 유무체크)Part 유무체크 각도 범위", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingPLevel, "(Part 유무체크)매칭 이미지 축소 단계", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingScore, "(Part 유무체크)Part 유무 판정 매칭률", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginX, "(Part 유무체크)Left 매칭 Search 마진", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginY, "(Part 유무체크)Top 매칭 Search 마진", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginX2, "(Part 유무체크)Right 매칭 Search 마진", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckMatchingSearchMarginY2, "(Part 유무체크)Bottom 매칭 Search 마진", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckInspectExistence, "(Part 유무체크)Part 유무 검사 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckBlob, "(Part 유무체크)Blob 분석 알고리즘으로 Part 유무 사전 체크 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckInspectShift, "(Part 유무체크)Shift 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckShiftX, "(Part 유무체크)X 위치 Shift 허용 오차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckShiftY, "(Part 유무체크)Y 위치 Shift 허용 오차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dPartCheckRotationAngle, "(Part 유무체크)Angle 허용 오차", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckLocalAlignUse, "(Part 유무체크)Local Align 데이터 계산", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckApplyComp, "(Part 유무체크)Leg Shift 보상식 적용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckMeasureEdge, "(Part 유무체크)매칭 후 Leg Edge 측정 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckLegXSize, "(Part 유무체크)Leg 1개 X방향 길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPartCheckLegYSize, "(Part 유무체크)Leg 1개 Y방향 길이", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dPartCheckEdgeMeasueSmFactor, "(Part 유무체크)Edge Detection Smoothing Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dPartCheckEdgeMeasueEdgeStr, "(Part 유무체크)Edge Strength", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPartCheckInspectLegDamage, "(Part 유무체크)Leg Y방향 Edge 미검출 시 Leg Damage 불량으로 처리", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseConcenter, "동심도 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConcenterMatchingScore, "(동심도 검사)원 검출 최소 매칭율", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConcenterInnerCircleDiameter, "(동심도 검사)원 지름", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterMatchingScaleMin, "(동심도 검사)스케일 조정 최소", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterMatchingScaleMax, "(동심도 검사)스케일 조정 최대", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iConcenterMatchingMinContrast, "(동심도 검사)Edge Contrast", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterCenterDiffSpecX, "(동심도 검사)X 방향 중심간 편차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dConcenterCenterDiffSpecY, "(동심도 검사)Y 방향 중심간 편차", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseHomerMark, "Homer Mark 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHomerMarkPos, "(Homer Mark)Row / Column 지정", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHomerMarkOrder, "(Homer Mark)코드 순서 지정", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseROIAlign, "ROI Align 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignXSearchMargin, "(ROI Align)X 방향 이동 범위", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignYSearchMargin, "(ROI Align)Y 방향 이동 범위", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignMatchingAngleRange, "(ROI Align)각도 범위", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignMatchingScaleMin, "(ROI Align)스케일 조정 최소", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignMatchingScaleMax, "(ROI Align)스케일 조정 최대", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignMatchingMinContrast, "(ROI Align)Edge Contrast", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignUsePosX, "(ROI Align)X 방향 적용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignUsePosY, "(ROI Align)Y 방향 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignMatchingScore, "(ROI Align)경계 매칭률", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignInspectShift, "(ROI Align)Shift 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iROIAlignMatchingPLevel, "(ROI Align)매칭 이미지 축소 단계", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftTop, "(ROI Align)Top 위치 Shift 허용 오차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftBottom, "(ROI Align)Bottom 위치 Shift 허용 오차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftLeft, "(ROI Align)Left 위치 Shift 허용 오차", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dROIAlignShiftRight, "(ROI Align)Right 위치 Shift 허용 오차", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bROIAlignLocalAlignUse, "(ROI Align)Local Align 데이터 계산", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseIsoColorImage, "(ROI Align)Color 영상 매칭 적용", sFixStringTab, file);

				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iProcessChType, "알고리즘 처리 영상 선택", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iProcessResType, "알고리즘 처리 영상 해상도 변경", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcess, "영상 전처리 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticType, "(영상 전처리)2개 영상 연산 타입", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage1, "(영상 전처리)Image 1", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage2, "(영상 전처리)Image 2", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage3, "(영상 전처리)Image 3", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticImage4, "(영상 전처리)Image 4", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dImageProcessArithmeticMulti, "(영상 전처리)Multi", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessArithmeticAdd, "(영상 전처리)Add", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessFilter, "(영상 전처리)필터링 전처리 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType1, "(영상 전처리)Filter 1", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType2, "(영상 전처리)Filter 2", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType3, "(영상 전처리)Filter 3", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType1X, "(영상 전처리)Filter 1 Width", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType2X, "(영상 전처리)Filter 2 Width", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType3X, "(영상 전처리)Filter 3 Width", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType1Y, "(영상 전처리)Filter 1 Height", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType2Y, "(영상 전처리)Filter 2 Height", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessFilterType3Y, "(영상 전처리)Filter 3 Height", sFixStringTab, file);

				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessPostLocalAlignROINo, "(영상 전처리/PostLocal Align)영상 번호", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessPostLocalAlign, "(영상 전처리/PostLocal Align)Local Align 연결 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessPostLocalAlignImageIndex, "(영상 전처리/PostLocal Align)영상 번호", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessLocalAlign, "(영상 전처리/Local Align)Local Align 연결 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignImageIndex, "(영상 전처리/Local Align)영상 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignROINo, "(영상 전처리/Local Align)ROI 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignROIType, "(영상 전처리/Local Align)Local Align 타입 설정", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignMatchingApplyMethod, "(영상 전처리/Local Align)ROI Align 결과 적용 방법", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bImageProcessLocalAlignPosX, "(영상 전처리/Local Align)Delta X 적용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bImageProcessLocalAlignPosY, "(영상 전처리/Local Align)Delta Y 적용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bImageProcessLocalAlignAngle, "(영상 전처리/Local Align)Angle 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessLocalAlignFitPos, "(영상 전처리/Local Align)Fitting 라인에 맞출 ROI 위치 선택", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessPostLocalAlignFitPos, "(영상 전처리/Local Align)로컬얼라인 회전중심", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessDontCare, "(영상 전처리/검사제외 영역 #1)검사제외 영역 #1 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareImageIndex, "(영상 전처리/검사제외 영역 #1)영상 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareROINo, "(영상 전처리/검사제외 영역 #1)ROI 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMargin, "(영상 전처리/검사제외 영역 #1)마진1", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMarginInner, "(영상 전처리/검사제외 영역 #1)마진2", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessDontCare2, "(영상 전처리/검사제외 영역 #2)검사제외 영역 #2 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareImageIndex2, "(영상 전처리/검사제외 영역 #2)영상 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareROINo2, "(영상 전처리/검사제외 영역 #2)ROI 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMargin2, "(영상 전처리/검사제외 영역 #2)마진1", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessDontCareMargin2Inner, "(영상 전처리/검사제외 영역 #2)마진2", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessGenerate, "(영상 전처리/검사영역 생성)검사영역 생성 적용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessGenerateInspectFlag, "(영상 전처리/검사영역 생성)검사영역이 생성될 경우만 원본 ROI 검사", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessGenerateImageIndex, "(영상 전처리/검사영역 생성)영상 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessGenerateROINo, "(영상 전처리/검사영역 생성)ROI 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessGenerateMargin, "(영상 전처리/검사영역 생성)생성된 검사영역 확장 마진", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessMask, "(영상 전처리/검사결과 제외 1st)검사결과 제외 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskImageIndex, "(영상 전처리/검사결과 제외 1st)영상 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskROINo, "(영상 전처리/검사결과 제외 1st)ROI 번호", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageProcessMask2, "(영상 전처리/검사결과 제외 2nd)검사결과 제외 적용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskImageIndex2, "(영상 전처리/검사결과 제외 2nd)영상 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iImageProcessMaskROINo2, "(영상 전처리/검사결과 제외 2nd)ROI 번호", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseShape, "요철 영상 생성 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iShapeIlluminationFilterX, "(요철 영상)필터 X 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iShapeIlluminationFilterY, "(요철 영상)필터 Y 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeIlluminationScaleFactor, "(요철 영상)Scale Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeCurvatureSmFactor, "(요철 영상)Smoothing Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeScalingMulti, "(요철 영상)Multi", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeScalingAdd, "(요철 영상)Add", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeImageReduceRatio, "(요철 영상)요철영상 축소 비율", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dShapeLedAngle, "(요철 영상)조명 각도", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iShapeImageMargin, "(요철 영상)검사영역 마진", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseEdgeMeasure, "(Edge 측정)Edge 측정 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureOnePoint, "(Edge 측정)Edge Point 측정 수량", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureMultiPointNumber, "(Edge 측정)Multi Points 분할 갯수", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bEdgeMeasureEndPoint, "(Edge 측정)Multi Points 분할 영역의 양쪽 끝 영역으로만 Line 생성", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bEdgeMeasureUseMaxMinPoint, "(Edge 측정)Multi Points 모드 중 최대 또는 최소 2Point만 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureUseMaxPoint, "(Edge 측정)Multi Points 모드 중 최대 2Point 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureDir, "(Edge 측정)Edge 측정 방향", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureGv, "(Edge 측정)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasurePos, "(Edge 측정)Edge 위치 선택", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bEdgeMeasureLocalAlignUse, "(Edge 측정)Local Align 데이터 계산", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dEdgeMeasureSmFactor, "(Edge 측정)Edge Detection Smoothing Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasureEdgeStr, "(Edge 측정)Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEdgeMeasurePosOffset, "(Edge 측정)Edge 포인트로부터 좌표 이동", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseHomerTilt, "(Homer Tilt)Homer Tilt 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHomerTiltHomerEdgeStr, "(Homer Tilt)Homer 라인 Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHomerTiltBenvolioImage, "(Homer Tilt)Benvolio 라인 측정 영상 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHomerTiltBenvolioEdgeStr, "(Homer Tilt)Benvolio 라인 Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dHomerTiltHomerLength, "(Homer Tilt)Homer X방향 길이 (픽셀)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dHomerTiltTol, "(Homer Tilt)Homer Tilt 허용 높이 (픽셀)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dHomerTiltDistanceTol, "(Homer Tilt)Homer / Benvolio 간 허용 거리 (픽셀)", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseEpoxyVoidHole, "(Epoxy Void Hole)Benvolio Epoxy Void Hole 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleImage, "(Epoxy Void Hole)Edge 측정 영상 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dEpoxyVoidHoleEdgeStr, "(Epoxy Void Hole)Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleInspectMargin, "(Epoxy Void Hole)상/하부 검사영역 마진", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bEpoxyVoidHoleTiltInspect, "(Epoxy Void Hole)좌측 / 우측간 Gap 편차 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleTiltTolerance, "(Epoxy Void Hole)편차 허용치", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bEpoxyVoidHoleGapInspect, "(Epoxy Void Hole)Benvolio / HTCC간 Gap 높이 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iEpoxyVoidHoleGapTolerance, "(Epoxy Void Hole)높이 최소 허용치", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUsePrintQuality, "(바코드 품질검사)바코드 품질검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityContrastMin, "(바코드 품질검사)contrast_min", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolColsMin, "(바코드 품질검사)symbol_cols_min", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolRowsMin, "(바코드 품질검사)symbol_rows_min", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolColsMax, "(바코드 품질검사)symbol_cols_max", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolRowsMax, "(바코드 품질검사)symbol_rows_max", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleSizeMin, "(바코드 품질검사)module_size_min", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleSizeMax, "(바코드 품질검사)module_size_max", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dPrintQualitySlantMax, "(바코드 품질검사)slant_max", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPrintQualityOverallQuality, "(바코드 품질검사)Overall Quality 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityGradeOverallQuality, "(바코드 품질검사)Overall Quality 품질 불량 판정 Grade", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bPrintQualityUnusedErrorCorrection, "(바코드 품질검사)Unused Error Correction 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityGradeUnusedErrorCorrection, "(바코드 품질검사)Unused Error Correction 품질 불량 판정 Grade", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityStrictModel, "(바코드 품질검사)strict_model", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityPolarity, "(바코드 품질검사)polarity", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityMirrored, "(바코드 품질검사)mirrored", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityContrastTolerance, "(바코드 품질검사)contrast_tolerance", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityStrictQuietZone, "(바코드 품질검사)strict_quiet_zone", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySymbolShape, "(바코드 품질검사)symbol_shape", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualitySmallModulesRobustness, "(바코드 품질검사)small_modules_robustness", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleGrid, "(바코드 품질검사)module_grid", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleGapMin, "(바코드 품질검사)module_gap_min", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityModuleGapMax, "(바코드 품질검사)module_gap_max", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iPrintQualityFinderPatternTolerance, "(바코드 품질검사)finder_pattern_tolerance", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDistanceMeasure, "(거리 측정)Y방향 거리 측정 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperGv, "(거리 측정/상부)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperPos, "(거리 측정/상부)Edge 위치 선택", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureUpperSmFactor, "(거리 측정/상부)Edge Detection Smoothing Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperEdgeStr, "(거리 측정/상부)Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerGv, "(거리 측정/하부)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerPos, "(거리 측정/하부)Edge 위치 선택", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureLowerSmFactor, "(거리 측정/하부)Edge Detection Smoothing Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerEdgeStr, "(거리 측정/하부)Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUpperPosOffset, "(거리 측정/상부)Edge 포인트로부터 좌표 이동", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureLowerPosOffset, "(거리 측정/하부)Edge 포인트로부터 좌표 이동", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureUsageType, "(거리 측정)검사 타입 (용도)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureSaveID, "(거리 측정)측정값 저장 ID 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureGapInspectionUpper, "(거리 측정)거리 허용 상한", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dDistanceMeasureGapInspectionLower, "(거리 측정)거리 허용 하한", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDistanceMeasureROIType, "(거리 측정)측정 ROI 타입", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseGapMeasure, "(Gap 측정)Gap 측정 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperGv, "(Gap 측정/상부)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperPos, "(Gap 측정/상부)Edge 위치 선택", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureUpperSmFactor, "(Gap 측정/상부)Edge Detection Smoothing Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperEdgeStr, "(Gap 측정/상부)Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerGv, "(Gap 측정/하부)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerPos, "(Gap 측정/하부)Edge 위치 선택", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureLowerSmFactor, "(Gap 측정/하부)Edge Detection Smoothing Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerEdgeStr, "(Gap 측정/하부)Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureUpperPosOffset, "(Gap 측정/상부)Edge 포인트로부터 좌표 이동", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureLowerPosOffset, "(Gap 측정/하부)Edge 포인트로부터 좌표 이동", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureSaveID, "(Gap 측정)측정값 저장 ID 번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureImageNo1, "(Gap 측정/상측) 영상번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureImageNo2, "(Gap 측정/하측) 영상번호", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureGapInspectionUpper, "(Gap 측정)Gap 허용 상한", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureGapInspectionLower, "(Gap 측정)Gap 허용 하한", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dGapMeasureGapMeasureOffset, "(Gap 측정)Gap 측정 옵셋", sFixStringTab, file);

				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureDir, "(Gap 측정)Gap 측정 방향", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bGapMeasureAlarm, "(Gap 측정)불량 판정 시 핸들러 알람 발생 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasurePointNumber, "(Gap 측정)측정 포인트 개수", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iGapMeasureSelectType, "(Gap 측정)측정 포인트 개수가 2개 이상인 경우 측정값 선택", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseSurfaceDualInspection, "(Dual Blob 분석)2개 영상으로부터 Blob 분석 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iSurfaceDualImageIndex_1, "(Dual Blob 분석)영상 번호 1", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iSurfaceDualImageIndex_2, "(Dual Blob 분석)영상 번호 2", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightFixedThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/고정 Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightLowerThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/고정 Threshold 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUpperThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/고정 Threshold 최대값", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightDT_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightMedianFilterSize_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/노이즈제거 Median 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightClosingFilterSize_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/불량 붙임 Closing 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterType_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/필터 종류", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(X)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize2_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(Y)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTValue_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Dynamic Threshold/DT 밝기차", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDTPolarTrans_1, "(Dual Blob 분석)영상1 Dynamic Threshold 원형 필터링 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightHystThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Hysteresis Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystSecureThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Hysteresis Threshold/Secure Threshold", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialThres_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Hysteresis Threshold/Potential Threshold", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialLength_1, "(Dual Blob 분석)영상1 밝은 불량 검출/Hysteresis Threshold/Hyst Length", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightFixedThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/고정 Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightLowerThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/고정 Threshold 최소값", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightUpperThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/고정 Threshold 최대값", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightDT_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightMedianFilterSize_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/노이즈제거 Median 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightClosingFilterSize_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/불량 붙임 Closing 필터 크기", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterType_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/필터 종류", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(X)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTFilterSize2_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/DT Mean 필터 크기(Y)", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightDTValue_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Dynamic Threshold/DT 밝기차", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseDTPolarTrans_2, "(Dual Blob 분석)영상2 Dynamic Threshold 원형 필터링 사용", sFixStringTab, file);
				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBrightHystThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Hysteresis Threshold 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystSecureThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Hysteresis Threshold/Secure Threshold", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialThres_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Hysteresis Threshold/Potential Threshold", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBrightHystPotentialLength_2, "(Dual Blob 분석)영상2 밝은 불량 검출/Hysteresis Threshold/Hyst Length", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDualBlobProcessType, "(Dual Blob 분석)영상 1 / 영상 2 후보 Blob 처리 방식", sFixStringTab, file);

				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDnnInspectionType, "(딥러닝 검사)검사 방식 선택", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iDnnInspectionModel, "(딥러닝 검사)딥러닝 기반 검사에 사용할 모델", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseHeightMeasure, "(높이 측정)높이 측정 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iHeightMeasureType, "(높이 측정)측정 타입", sFixStringTab, file);

				WriteRecord_BOOL(pFileInspectROIRgn->m_AlgorithmParam[ii].m_bUseBoundaryInspection, "(경계 검사)경계 검사 사용", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureDir, "(경계 검사)Edge 검색 시작 위치", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureGv, "(경계 검사)Edge 측정 표면 밝기 변화", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasurePos, "(경계 검사)Edge 위치 선택", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dBoundaryEdgeMeasureSmFactor, "(경계 검사)Edge Detection Smoothing Factor", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_dBoundaryEdgeMeasureEdgeStr, "(경계 검사)Edge Strength", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureSampling, "(경계 검사)Edge 측정 포인트 샘플링", sFixStringTab, file);
				WriteRecord(pFileInspectROIRgn->m_AlgorithmParam[ii].m_iBoundaryEdgeMeasureSensorLength, "(경계 검사)Edge 검출 센서 길이", sFixStringTab, file);
			}
		}
	}

	SAFE_DELETE(pFileInspectionArea);
}

void CModelDataManager::WriteRecord_BOOL(BOOL bNow, CString sParam, CString sFixVal, CFile* file)
{
	CString sNow;

	if (bNow == TRUE) sNow = "사용"; else sNow = "미사용";

	CString sFullPath;
	sFullPath.Format("%s\t%s\t%s\r\n", sFixVal, sParam, sNow);
	file->Write(sFullPath, sFullPath.GetLength());
}

void CModelDataManager::WriteRecord(int iNow, CString sParam, CString sFixVal, CFile* file)
{
	CString sFullPath;
	sFullPath.Format("%s\t%s\t%d\r\n", sFixVal, sParam, iNow);
	file->Write(sFullPath, sFullPath.GetLength());
}

void CModelDataManager::WriteRecord(double dNow, CString sParam, CString sFixVal, CFile* file)
{
	CString sFullPath;
	sFullPath.Format("%s\t%s\t%.3lf\r\n", sFixVal, sParam, dNow);
	file->Write(sFullPath, sFullPath.GetLength());
}

void CModelDataManager::CheckColorSpace()
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[m_iModelIdx];

	int i, iIdx, iTabIdx;

	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_bUseColorSpace[i] = FALSE;
	}

	if (m_pInspectionArea == NULL)
		return;

	BOOL bUseColorSpace, bUseImageV;

	GTRegion* pInspectROIRgn;
	BOOL bFound;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		bUseColorSpace = FALSE;
		bFound = FALSE;

		for (iIdx = 0; iIdx < iNoInspectROI; iIdx++)
		{
			pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
			if (pInspectROIRgn == NULL)
				continue;

			if ((i + 1) != pInspectROIRgn->miTeachImageIndex)
				continue;

			for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
			{
				if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect)
				{
					if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType == CHANNEL_TYPE_S || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType == CHANNEL_TYPE_H ||
						((pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseDC == TRUE) && (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC == CHANNEL_TYPE_S || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChTypeDC == CHANNEL_TYPE_H)))
					{
						bUseColorSpace = TRUE;
						bFound = TRUE;
						break;
					}
				}
			}

			if (bFound)
				break;
		}

		if (bUseColorSpace)
			m_bUseColorSpace[i] = TRUE;
	}

	int iDualBlobImageIndex1, iDualBlobImageIndex2;

	for (iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		for (iTabIdx = 0; iTabIdx < MAX_INSPECTION_TAB; iTabIdx++)
		{
			if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bInspect && pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_bUseSurfaceDualInspection)
			{
				if (pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType == CHANNEL_TYPE_S || pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iProcessChType == CHANNEL_TYPE_H)
				{
					iDualBlobImageIndex1 = pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iSurfaceDualImageIndex_1;
					iDualBlobImageIndex2 = pInspectROIRgn->m_AlgorithmParam[iTabIdx].m_iSurfaceDualImageIndex_2;

					m_bUseColorSpace[iDualBlobImageIndex1] = TRUE;
					m_bUseColorSpace[iDualBlobImageIndex2] = TRUE;
				}
			}
		}
	}

	for (i = 0; i < m_iNoUsedImageGrab; i++)
	{
		if (m_bUseColorSpace[i])
		{
			strLog.Format("Vision recipe load, Used H, S channel, Vision: %s, Image: %d", sVisionCamType_Short, i + 1);
			THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
		}
	}
}

int CModelDataManager::GetWhichSpecularImage(int iImageIdx, int* piGrabCycleIndex)
{
	int iWhichSpecularImage = DFM_IMAGE_COMBINE_CURVATURE;

	int iNoSpecularGrab = 0;
	int iSpecularStartImageIdx[10];

	BOOL bPrevSpecular = FALSE;
	BOOL bCurSpecular;

	*piGrabCycleIndex = -1;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		bCurSpecular = m_bUseSpecularImage[i];

		if (i > iImageIdx)
			break;

		if (bPrevSpecular == FALSE && bCurSpecular == TRUE)
		{
			iSpecularStartImageIdx[iNoSpecularGrab] = i;
			iWhichSpecularImage = iImageIdx - iSpecularStartImageIdx[iNoSpecularGrab];

			++iNoSpecularGrab;

			i += (DFM_USED_CONV_IMAGE_NUMBER - 1);

			bPrevSpecular = FALSE;

			++(*piGrabCycleIndex);

			continue;
		}
	}

	if (*piGrabCycleIndex < 0)
		*piGrabCycleIndex = 0;

	return iWhichSpecularImage;
}

int CModelDataManager::GetWhichDiffusedImage(int iImageIdx, int* piGrabCycleIndex)
{
	int iWhichDiffusedImage = 0;

	int iNoDiffusedGrab = 0;
	int iDiffusedStartImageIdx[10];

	BOOL bPrevDiffused = FALSE;
	BOOL bCurDiffused;

	*piGrabCycleIndex = -1;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		bCurDiffused = m_bUseDiffusedImage[i];

		if (i > iImageIdx)
			break;

		if (bPrevDiffused == FALSE && bCurDiffused == TRUE)
		{
			iDiffusedStartImageIdx[iNoDiffusedGrab] = i;
			iWhichDiffusedImage = iImageIdx - iDiffusedStartImageIdx[iNoDiffusedGrab];

			++iNoDiffusedGrab;

			i += (DIFFUSED_USED_CONV_IMAGE_NUMBER - 1);

			bPrevDiffused = FALSE;

			++(*piGrabCycleIndex);

			continue;
		}
	}

	if (*piGrabCycleIndex < 0)
		*piGrabCycleIndex = 0;

	return iWhichDiffusedImage;
}

int CModelDataManager::GetSequenceStartGrabBufferIndex(int iStageIdx, int iImageIdx)
{
	int iSequenceStartGrabBufferIndex = -1;

	int iSeqAddressIndex = m_iSeqAddrIndex[iStageIdx][iImageIdx];
	int iCurSeqAddressIndex = -1;

	for (int i = iImageIdx; i >= 0; i--)
	{
		if (m_bUseSeqStartImage[i])
		{
			iCurSeqAddressIndex = m_iSeqAddrIndex[iStageIdx][i];

			if (iCurSeqAddressIndex == iSeqAddressIndex)
			{
				iSequenceStartGrabBufferIndex = i;
				break;
			}
		}
	}

	return iSequenceStartGrabBufferIndex;
}

int CModelDataManager::GetPageIndex(int iStageIdx, int iImageIdx)
{
	int iPageIndex = 0;

	int iSequenceStartImageIndex = -1;

	int iSeqAddressIndex = m_iSeqAddrIndex[iStageIdx][iImageIdx];
	int iCurSeqAddressIndex = -1;

	int iGrabType = -1;

	for (int i = iImageIdx; i >= 0; i--)
	{
		if (m_bUseSeqStartImage[i])
		{
			iCurSeqAddressIndex = m_iSeqAddrIndex[iStageIdx][i];

			if (iCurSeqAddressIndex == iSeqAddressIndex)
			{
				iSequenceStartImageIndex = i;
				iGrabType = 0;
				break;
			}
		}
		else if (m_bUseSpecularImage[i])
		{
			iCurSeqAddressIndex = m_iSeqAddrIndex[iStageIdx][i];

			if (iCurSeqAddressIndex == iSeqAddressIndex)
			{
				iSequenceStartImageIndex = i - 1;
				iGrabType = 1;
				break;
			}
		}
		else if (m_bUseDiffusedImage[i])
		{
			iCurSeqAddressIndex = m_iSeqAddrIndex[iStageIdx][i];

			if (iCurSeqAddressIndex == iSeqAddressIndex)
			{
				iSequenceStartImageIndex = i;
				iGrabType = 2;
				break;
			}
		}
	}

	if (iGrabType < 0)
		return iPageIndex;

	int iOrderFromSeq0 = 0;

	if (iGrabType == 1 || iGrabType == 2)
	{
		iOrderFromSeq0 = 0;
	}
	else
	{
		iOrderFromSeq0 = iImageIdx - iSequenceStartImageIndex;
	}

	iPageIndex = m_PageControlData.m_iAddrSeq[iSeqAddressIndex][iOrderFromSeq0];

	return iPageIndex;
}

int CModelDataManager::GetGrabingNumber(int iStageIdx, int iNoCurImageGrab)
{
	int iSeqAddrIndex, iNoSeqGrabImage;
	int iGrabCount = 0;

	while (TRUE)
	{
		if (iNoCurImageGrab >= m_iNoUsedImageGrab)
			break;

		iSeqAddrIndex = m_iSeqAddrIndex[iStageIdx][iNoCurImageGrab];
		iNoSeqGrabImage = m_PageControlData.m_iAddrCount[iSeqAddrIndex];

		if (m_bUseSpecularImage[iNoCurImageGrab] == TRUE)
		{
			iNoCurImageGrab += DFM_USED_CONV_IMAGE_NUMBER;
			iGrabCount += SPV_RAW_IMAGE_NUMBER;
		}
		else if (m_bUseDiffusedImage[iNoCurImageGrab] == TRUE)
		{
			iNoCurImageGrab += DIFFUSED_USED_CONV_IMAGE_NUMBER;
			iGrabCount += DIFFUSED_RAW_IMAGE_NUMBER;
		}
		else
		{
			iNoCurImageGrab += iNoSeqGrabImage;
			iGrabCount += iNoSeqGrabImage;
		}
	}

	return iGrabCount;
}

int CModelDataManager::GetGrabingNumber(int iStageIdx, int iNoCurImageGrab, int iNoUsedImageGrab)
{
	int iSeqAddrIndex, iNoSeqGrabImage;
	int iGrabCount = 0;

	while (TRUE)
	{
		if (iNoCurImageGrab >= iNoUsedImageGrab)
			break;

		iSeqAddrIndex = m_iSeqAddrIndex[iStageIdx][iNoCurImageGrab];
		iNoSeqGrabImage = m_PageControlData.m_iAddrCount[iSeqAddrIndex];

		if (m_bUseSpecularImage[iNoCurImageGrab] == TRUE)
		{
			iNoCurImageGrab += DFM_USED_CONV_IMAGE_NUMBER;
			iGrabCount += SPV_RAW_IMAGE_NUMBER;
		}
		else if (m_bUseDiffusedImage[iNoCurImageGrab] == TRUE)
		{
			iNoCurImageGrab += DIFFUSED_USED_CONV_IMAGE_NUMBER;
			iGrabCount += DIFFUSED_RAW_IMAGE_NUMBER;
		}
		else
		{
			iNoCurImageGrab += iNoSeqGrabImage;
			iGrabCount += iNoSeqGrabImage;
		}
	}

	return iGrabCount;
}


void CModelDataManager::GetLinkInfo_LocalAlign(int iSearchImageTabNo, int iSearchROINo, int* piNoLinkROI, int* piImageTabNo, int* piROICenterX, int* piROICenterY, int* piInspectTabNo)
{
	*piNoLinkROI = 0;

	if (m_pInspectionArea == NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	GTRegion* pInspectROIRgn;
	int iImageNo, iROINo;
	POINT CenterPoint;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			if (pInspectROIRgn->m_AlgorithmParam[i].m_bInspect)
			{
				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessLocalAlign)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessLocalAlignImageIndex + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessLocalAlignROINo;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						piImageTabNo[*piNoLinkROI] = pInspectROIRgn->miTeachImageIndex;		// Image Tab No

						pInspectROIRgn->GetCenterPoint(&CenterPoint, THEAPP.m_pCalDataService);
						piROICenterX[*piNoLinkROI] = CenterPoint.x;
						piROICenterY[*piNoLinkROI] = CenterPoint.y;							// ROI Center Pos

						piInspectTabNo[*piNoLinkROI] = i + 1;								// Inspection Tab

						++(*piNoLinkROI);
					}

					if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessLocalAlignAddLineFit)
					{
						iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessLocalAlignImageIndex_2 + 1;
						iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessLocalAlignROINo_2;

						if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
						{
							piImageTabNo[*piNoLinkROI] = pInspectROIRgn->miTeachImageIndex;		// Image Tab No

							pInspectROIRgn->GetCenterPoint(&CenterPoint, THEAPP.m_pCalDataService);
							piROICenterX[*piNoLinkROI] = CenterPoint.x;
							piROICenterY[*piNoLinkROI] = CenterPoint.y;							// ROI Center Pos

							piInspectTabNo[*piNoLinkROI] = i + 1;								// Inspection Tab

							++(*piNoLinkROI);
						}
					}
				}
			}
		}
	}
}

void CModelDataManager::GetLinkInfo_DontCare(int iSearchImageTabNo, int iSearchROINo, int* piNoLinkROI, int* piImageTabNo, int* piROICenterX, int* piROICenterY, int* piInspectTabNo)
{
	*piNoLinkROI = 0;

	if (m_pInspectionArea == NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	GTRegion* pInspectROIRgn;
	int iImageNo, iROINo;
	POINT CenterPoint;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			if (pInspectROIRgn->m_AlgorithmParam[i].m_bInspect)
			{
				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessDontCare)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessDontCareImageIndex + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessDontCareROINo;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						piImageTabNo[*piNoLinkROI] = pInspectROIRgn->miTeachImageIndex;		// Image Tab No

						pInspectROIRgn->GetCenterPoint(&CenterPoint, THEAPP.m_pCalDataService);
						piROICenterX[*piNoLinkROI] = CenterPoint.x;
						piROICenterY[*piNoLinkROI] = CenterPoint.y;							// ROI Center Pos

						piInspectTabNo[*piNoLinkROI] = i + 1;								// Inspection Tab

						++(*piNoLinkROI);
					}
				}

				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessDontCare2)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessDontCareImageIndex2 + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessDontCareROINo2;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						piImageTabNo[*piNoLinkROI] = pInspectROIRgn->miTeachImageIndex;		// Image Tab No

						pInspectROIRgn->GetCenterPoint(&CenterPoint, THEAPP.m_pCalDataService);
						piROICenterX[*piNoLinkROI] = CenterPoint.x;
						piROICenterY[*piNoLinkROI] = CenterPoint.y;							// ROI Center Pos

						piInspectTabNo[*piNoLinkROI] = i + 1;								// Inspection Tab

						++(*piNoLinkROI);
					}
				}
			}
		}
	}
}

void CModelDataManager::GetLinkInfo_Generate(int iSearchImageTabNo, int iSearchROINo, int* piNoLinkROI, int* piImageTabNo, int* piROICenterX, int* piROICenterY, int* piInspectTabNo)
{
	*piNoLinkROI = 0;

	if (m_pInspectionArea == NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	GTRegion* pInspectROIRgn;
	int iImageNo, iROINo;
	POINT CenterPoint;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			if (pInspectROIRgn->m_AlgorithmParam[i].m_bInspect)
			{
				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessGenerate)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessGenerateImageIndex + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessGenerateROINo;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						piImageTabNo[*piNoLinkROI] = pInspectROIRgn->miTeachImageIndex;		// Image Tab No

						pInspectROIRgn->GetCenterPoint(&CenterPoint, THEAPP.m_pCalDataService);
						piROICenterX[*piNoLinkROI] = CenterPoint.x;
						piROICenterY[*piNoLinkROI] = CenterPoint.y;							// ROI Center Pos

						piInspectTabNo[*piNoLinkROI] = i + 1;								// Inspection Tab

						++(*piNoLinkROI);
					}
				}
			}
		}
	}
}

void CModelDataManager::GetLinkInfo_Mask(int iSearchImageTabNo, int iSearchROINo, int* piNoLinkROI, int* piImageTabNo, int* piROICenterX, int* piROICenterY, int* piInspectTabNo)
{
	*piNoLinkROI = 0;

	if (m_pInspectionArea == NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	GTRegion* pInspectROIRgn;
	int iImageNo, iROINo;
	POINT CenterPoint;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			if (pInspectROIRgn->m_AlgorithmParam[i].m_bInspect)
			{
				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessMask)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessMaskImageIndex + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessMaskROINo;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						piImageTabNo[*piNoLinkROI] = pInspectROIRgn->miTeachImageIndex;		// Image Tab No

						pInspectROIRgn->GetCenterPoint(&CenterPoint, THEAPP.m_pCalDataService);
						piROICenterX[*piNoLinkROI] = CenterPoint.x;
						piROICenterY[*piNoLinkROI] = CenterPoint.y;							// ROI Center Pos

						piInspectTabNo[*piNoLinkROI] = i + 1;								// Inspection Tab

						++(*piNoLinkROI);
					}
				}

				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessMask2)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessMaskImageIndex2 + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessMaskROINo2;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						piImageTabNo[*piNoLinkROI] = pInspectROIRgn->miTeachImageIndex;		// Image Tab No

						pInspectROIRgn->GetCenterPoint(&CenterPoint, THEAPP.m_pCalDataService);
						piROICenterX[*piNoLinkROI] = CenterPoint.x;
						piROICenterY[*piNoLinkROI] = CenterPoint.y;							// ROI Center Pos

						piInspectTabNo[*piNoLinkROI] = i + 1;								// Inspection Tab

						++(*piNoLinkROI);
					}
				}

			}
		}
	}
}

void CModelDataManager::UpdateLinkInfo_LocalAlign(int iSearchImageTabNo, int iSearchROINo, int iNewImageTabNo)
{
	if (m_pInspectionArea == NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	GTRegion* pInspectROIRgn;
	int iImageNo, iROINo;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			if (pInspectROIRgn->m_AlgorithmParam[i].m_bInspect)
			{
				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessLocalAlign)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessLocalAlignImageIndex + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessLocalAlignROINo;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessLocalAlignImageIndex = iNewImageTabNo - 1;
					}

					if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessLocalAlignAddLineFit)
					{
						iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessLocalAlignImageIndex_2 + 1;
						iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessLocalAlignROINo_2;

						if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
						{
							pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessLocalAlignImageIndex_2 = iNewImageTabNo - 1;
						}
					}
				}
			}
		}
	}
}

void CModelDataManager::UpdateLinkInfo_DontCare(int iSearchImageTabNo, int iSearchROINo, int iNewImageTabNo)
{
	if (m_pInspectionArea == NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	GTRegion* pInspectROIRgn;
	int iImageNo, iROINo;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			if (pInspectROIRgn->m_AlgorithmParam[i].m_bInspect)
			{
				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessDontCare)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessDontCareImageIndex + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessDontCareROINo;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessDontCareImageIndex = iNewImageTabNo - 1;
					}
				}

				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessDontCare2)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessDontCareImageIndex2 + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessDontCareROINo2;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessDontCareImageIndex2 = iNewImageTabNo - 1;
					}
				}
			}
		}
	}
}

void CModelDataManager::UpdateLinkInfo_Generate(int iSearchImageTabNo, int iSearchROINo, int iNewImageTabNo)
{
	if (m_pInspectionArea == NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	GTRegion* pInspectROIRgn;
	int iImageNo, iROINo;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			if (pInspectROIRgn->m_AlgorithmParam[i].m_bInspect)
			{
				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessGenerate)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessGenerateImageIndex + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessGenerateROINo;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessGenerateImageIndex = iNewImageTabNo - 1;
					}
				}
			}
		}
	}
}

void CModelDataManager::UpdateLinkInfo_Mask(int iSearchImageTabNo, int iSearchROINo, int iNewImageTabNo)
{
	if (m_pInspectionArea == NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	GTRegion* pInspectROIRgn;
	int iImageNo, iROINo;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			if (pInspectROIRgn->m_AlgorithmParam[i].m_bInspect)
			{
				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessMask)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessMaskImageIndex + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessMaskROINo;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessMaskImageIndex = iNewImageTabNo - 1;
					}
				}

				if (pInspectROIRgn->m_AlgorithmParam[i].m_bUseImageProcessMask2)
				{
					iImageNo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessMaskImageIndex2 + 1;
					iROINo = pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessMaskROINo2;

					if ((iImageNo == iSearchImageTabNo) && (iROINo == iSearchROINo))
					{
						pInspectROIRgn->m_AlgorithmParam[i].m_iImageProcessMaskImageIndex2 = iNewImageTabNo - 1;
					}
				}
			}
		}
	}
}

void CModelDataManager::ResetVMThresOptData()
{
	if (m_pInspectionArea == NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	GTRegion* pInspectROIRgn;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST)
			pInspectROIRgn->ResetVmOptData();
	}
}

void CModelDataManager::UpdateVMThres()
{
	if (m_pInspectionArea == NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI <= 0)
		return;

	GTRegion* pInspectROIRgn;
	HTuple HNoData, HOptAbsThreshold, HOptVarThreshold;
	HTuple HAbsMean, HAbsStd, HRatioMean, HRatioStd;
	Hlong lNoDiffData, lNoDiffVarData;

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->miInspectionType >= INSPECTION_TYPE_FIRST && pInspectROIRgn->miInspectionType <= INSPECTION_TYPE_LAST)
		{
			for (int ii = 0; ii < MAX_INSPECTION_TAB; ii++)
			{
				if (pInspectROIRgn->m_AlgorithmParam[ii].m_bInspect && pInspectROIRgn->m_AlgorithmParam[ii].m_bUseImageCompare && pInspectROIRgn->m_HVarModelID[ii] > 0)
				{
					TupleLength(pInspectROIRgn->m_HBrightAbsDiffs[ii], &HNoData);
					lNoDiffData = HNoData.L();

					if (lNoDiffData > 0)
					{
						TupleMean(pInspectROIRgn->m_HBrightAbsDiffs[ii], &HAbsMean);
						TupleDeviation(pInspectROIRgn->m_HBrightAbsDiffs[ii], &HAbsStd);
						HOptAbsThreshold = HAbsMean + THEAPP.Struct_PreferenceStruct.m_dVmKSigma * HAbsStd;

						if (HOptAbsThreshold < 10)
							HOptAbsThreshold = 10;
						pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareBrightAbs = (int)HOptAbsThreshold.D();
					}

					TupleLength(pInspectROIRgn->m_HBrightRatioDiffs[ii], &HNoData);
					lNoDiffVarData = HNoData.L();

					if (lNoDiffVarData > 0)
					{
						TupleMean(pInspectROIRgn->m_HBrightRatioDiffs[ii], &HRatioMean);
						TupleDeviation(pInspectROIRgn->m_HBrightRatioDiffs[ii], &HRatioStd);
						HOptVarThreshold = HRatioMean + THEAPP.Struct_PreferenceStruct.m_dVmKSigma * HRatioStd;

						if (HOptVarThreshold < 1)
							HOptVarThreshold = 1;

						double dOptVarThres;
						CString sTemp;
						dOptVarThres = HOptVarThreshold.D();
						sTemp.Format("%.2lf", dOptVarThres);
						dOptVarThres = atof((LPSTR)(LPCSTR)sTemp);

						pInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareBrightVar = dOptVarThres;
					}

					TupleLength(pInspectROIRgn->m_HDarkAbsDiffs[ii], &HNoData);
					lNoDiffData = HNoData.L();

					if (lNoDiffData > 0)
					{
						TupleMean(pInspectROIRgn->m_HDarkAbsDiffs[ii], &HAbsMean);
						TupleDeviation(pInspectROIRgn->m_HDarkAbsDiffs[ii], &HAbsStd);
						HOptAbsThreshold = HAbsMean + THEAPP.Struct_PreferenceStruct.m_dVmKSigma * HAbsStd;

						if (HOptAbsThreshold < 10)
							HOptAbsThreshold = 10;
						pInspectROIRgn->m_AlgorithmParam[ii].m_iImageCompareDarkAbs = (int)HOptAbsThreshold.D();
					}

					TupleLength(pInspectROIRgn->m_HDarkRatioDiffs[ii], &HNoData);
					lNoDiffVarData = HNoData.L();

					if (lNoDiffVarData > 0)
					{
						TupleMean(pInspectROIRgn->m_HDarkRatioDiffs[ii], &HRatioMean);
						TupleDeviation(pInspectROIRgn->m_HDarkRatioDiffs[ii], &HRatioStd);
						HOptVarThreshold = HRatioMean + THEAPP.Struct_PreferenceStruct.m_dVmKSigma * HRatioStd;

						if (HOptVarThreshold < 1)
							HOptVarThreshold = 1;

						double dOptVarThres;
						CString sTemp;
						dOptVarThres = HOptVarThreshold.D();
						sTemp.Format("%.2lf", dOptVarThres);
						dOptVarThres = atof((LPSTR)(LPCSTR)sTemp);

						pInspectROIRgn->m_AlgorithmParam[ii].m_dImageCompareDarkVar = dOptVarThres;
					}
				}
			}
		}
	}
}
