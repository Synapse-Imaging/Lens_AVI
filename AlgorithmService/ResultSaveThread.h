#pragma once
#include "stdafx.h"
#include "..\InspectService.h"
#include "..\Algorithm.h"
#include "..\uScan.h"

typedef struct
{
	int iPcVisionNo;
	int iVisionCamType;

} RESULT_SAVE_THREAD_PARAM;


//////////////////////////////////////////////////////////////////////////
// Helper Function
//////////////////////////////////////////////////////////////////////////
inline IInspectAdminViewHideDlg* GetInspectViewDlg(int visionNo)
{
	if (visionNo == VISION_NUMBER_1) return THEAPP.m_pInspectAdminViewHideDlg1;
	if (visionNo == VISION_NUMBER_2) return THEAPP.m_pInspectAdminViewHideDlg2;
	if (visionNo == VISION_NUMBER_3) return THEAPP.m_pInspectAdminViewHideDlg3;
	return THEAPP.m_pInspectAdminViewHideDlg4;
}

inline ITrayAdminViewDlg* GetTrayViewDlg(int visionNo)
{
	if (visionNo == VISION_NUMBER_1) return THEAPP.m_pTrayAdminViewDlg1;
	if (visionNo == VISION_NUMBER_2) return THEAPP.m_pTrayAdminViewDlg2;
	if (visionNo == VISION_NUMBER_3) return THEAPP.m_pTrayAdminViewDlg3;
	return THEAPP.m_pTrayAdminViewDlg4;
}

// 로그 저장 조건 체크
inline bool ShouldSaveLog(int iPcVisionNo)
{
#if defined (UAVI_BOI) || defined (UAVI_BOS) || defined (UAVI_KRIOS)
	// PWM 체크 + 다음 Vision 비활성화 체크 (N2, N4 다음 Vision 체크x)

	bool bPWMCondition = !THEAPP.m_ModelDefineInfo.m_bVisionPWM[iPcVisionNo];
	bool bCurrentVisionActive = THEAPP.Struct_PreferenceStruct.m_bUseVision[iPcVisionNo];

	bool bNextVisionInactive = true;
	if (iPcVisionNo == VISION_NUMBER_1)
		bNextVisionInactive = !THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_2];
	else if (iPcVisionNo == VISION_NUMBER_3)
		bNextVisionInactive = !THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_4];


	return bPWMCondition && bCurrentVisionActive && bNextVisionInactive;
#else
	// 현재 Vision 활성화 & 이후 모든 Vision 비활성화 (마지막 활성 Vision)
	if (!THEAPP.Struct_PreferenceStruct.m_bUseVision[iPcVisionNo])
		return false;

	for (int iNextVision = iPcVisionNo + 1; iNextVision <= VISION_NUMBER_4; iNextVision++)
	{
		if (THEAPP.Struct_PreferenceStruct.m_bUseVision[iNextVision])
			return false;
	}

	return true;
#endif
}

inline void GetAllDefectsVisionRange(int iPcVisionNo, int& iStartVision, int& iEndVision)
{
#if defined (UAVI_BOI) || defined (UAVI_BOS) || defined (UAVI_KRIOS)
	// N2, N4
	switch (iPcVisionNo)
	{
	case VISION_NUMBER_1:
		iStartVision = VISION_NUMBER_1;
		iEndVision = VISION_NUMBER_1;
		break;
	case VISION_NUMBER_2:
		iStartVision = VISION_NUMBER_1;
		iEndVision = VISION_NUMBER_2;
		break;
	case VISION_NUMBER_3:
		iStartVision = VISION_NUMBER_3;
		iEndVision = VISION_NUMBER_3;
		break;
	case VISION_NUMBER_4:
		iStartVision = VISION_NUMBER_3;
		iEndVision = VISION_NUMBER_4;
		break;
	}
#else
	// N1 ~ N4
	iStartVision = VISION_NUMBER_1;
	iEndVision = iPcVisionNo;
#endif
}

inline int GetResultVisionCount(int iPcVisionNo)
{
#if defined (UAVI_BOI) || defined (UAVI_BOS) || defined (UAVI_KRIOS)
	int count = 0;

	if (iPcVisionNo <= VISION_NUMBER_2)
	{
		// N1, N2
		if (THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_1] &&
			THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_1] != "Undefine")
			count++;

		if (THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_2] &&
			THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_2] != "Undefine")
			count++;
	}
	else
	{
		// N3, N4
		if (THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_3] &&
			THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_3] != "Undefine")
			count++;

		if (THEAPP.Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_4] &&
			THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_4] != "Undefine")
			count++;
	}

	return count;
#else
	return THEAPP.m_ModelDefineInfo.m_iUseVisionNo;
#endif
}

inline int GetResultPairOption(int iPcVisionNo)
{
#if defined (UAVI_BOI) || defined (UAVI_BOS) || defined (UAVI_KRIOS)
	// Vision 1-2: offset 0, Vision 3-4: offset 2
	return (iPcVisionNo <= VISION_NUMBER_2) ? 0 : 2;
#else
	return 0;
#endif
}

// 비전 결과, AI 결과 벡터 생성 (인덱스 범위 초과, 매칭 실패 시 기본값 설정)
inline bool GetAllResult(int visionCount,
	std::vector<CString>& vecVisionResultList,
	std::vector<CString>& vecAIResultList,
	std::vector<CuScanApp::DefectBlobInfo>& vecDefectBlobInfoList,
	bool isMC, int iMzNo, int iTrayNo, int iModuleNo, int isPairOption)  // 누락된 매개변수 추가
{
	for (int iVisionIdx = 0; iVisionIdx < visionCount; iVisionIdx++)
	{
		if (isMC) {
			// 매칭 실패 시 "MC" 설정
			vecVisionResultList.at(iVisionIdx) = _T("MC");
			vecAIResultList.at(iVisionIdx * 3) = _T("MC");
			vecAIResultList.at(iVisionIdx * 3 + 1) = _T("MC");
			vecAIResultList.at(iVisionIdx * 3 + 2) = _T("MC");
			vecDefectBlobInfoList.at(iVisionIdx).Reset();
		}
		else {
			// 정상적인 결과 설정
			vecVisionResultList.at(iVisionIdx) = THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionIdx + isPairOption];
			vecAIResultList.at(iVisionIdx * 3) = THEAPP.m_sAI1DefectResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionIdx + isPairOption];
			vecAIResultList.at(iVisionIdx * 3 + 1) = THEAPP.m_sAI2DefectResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionIdx + isPairOption];
			vecAIResultList.at(iVisionIdx * 3 + 2) = THEAPP.m_sAI3DefectResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionIdx + isPairOption];
			vecDefectBlobInfoList.at(iVisionIdx) = THEAPP.m_StructDefectBlobInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionIdx + isPairOption];

		}
	}

	return true;
}

// FAI 벡터 생성 (런타임 모델 타입 기반)
inline std::vector<double> GetFAIValueVector(int iMzNo, int iTrayNo, int iModuleNo, CString strModelType)
{
	std::vector<double> vecFaiValue;

	CenterlineMeasureStruct& measureStruct = CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1);
	const FAIModelConfig& config = CFAIDataManager::GetInstance().GetModelConfig();

	for (const FAIItemDescriptor& fi : config.faiItems)
	{
		// FAI 항목 수에 따라 벡터에 값 추가
		int valueCount = (int)fi.logHeaderNames.size();
		for (int j = 0; j < valueCount; j++)
			vecFaiValue.push_back(measureStruct.m_dFAIMeasureValue[fi.faiNumber][j]);
	}

	return vecFaiValue;
}

// FAI 결과 포맷 문자열 생성 (런타임 모델 타입 기반)
inline std::string GetFAIResultFormatString()
{
	CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];

	if (strModelType == _T("CHS-W") || strModelType == _T("CHS-Z"))
	{
		// CHS_WZ 모델: 48개 값
		return "%s\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t";
	}
	else
	{
		// CHS_KS / CHS_TV 모델: 55개 값
		return "%s\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf";
	}
}

inline std::vector<double> GetFAIDefaultValueVector(CString strModelType)
{
	// FAI default value vector by model type string
	// TODO : hardcoded counts - refactor using FAIUsed flags
	if (strModelType == _T("CHS-W") || strModelType == _T("CHS-Z"))
		return std::vector<double>(48, FAI_MEASURE_ERROR);
	if (strModelType == _T("CHS-K") || strModelType == _T("CHS-S") || strModelType == _T("CHS-T") || strModelType == _T("CHS-V"))
		return std::vector<double>(55, FAI_MEASURE_ERROR);
	if (strModelType == _T("BOI"))
		return std::vector<double>(5, FAI_MEASURE_ERROR);
	if (strModelType == _T("BOI"))
		return std::vector<double>(5, FAI_MEASURE_ERROR);
	if (strModelType == _T("ACT"))
		return std::vector<double>(28, FAI_MEASURE_ERROR);
	if (strModelType == _T("AKC"))
		return std::vector<double>(22, FAI_MEASURE_ERROR);
	if (strModelType == _T("ATW-D"))
		return std::vector<double>(1, FAI_MEASURE_ERROR);
	if (strModelType == _T("BOS"))
		return std::vector<double>(1, FAI_MEASURE_ERROR);
	if (strModelType == _T("RENO"))
		return std::vector<double>(1, FAI_MEASURE_ERROR);
	if (strModelType == _T("KRIOS"))
		return std::vector<double>(1, FAI_MEASURE_ERROR);
	return std::vector<double>();
}


inline UINT ResultSaveThread(LPVOID lp)
{
	CString strLog;

	RESULT_SAVE_THREAD_PARAM* pParam = (RESULT_SAVE_THREAD_PARAM*)lp;
	int iPcVisionNo = pParam->iPcVisionNo;
	int iVisionCamType = pParam->iVisionCamType;
	SAFE_DELETE(pParam);

	Algorithm* pAlgorithm = THEAPP.m_pInspectService->m_pInspectAlgorithm + iVisionCamType;

	pAlgorithm->ChangeResultSaveThreadRunning(TRUE);

	CInspectService* pInspectService = THEAPP.m_pInspectService;

	int iInspectionBufferIdx;
	BOOL bLogSaved;
	CString sModuleFAIDefectName = _T("");

	int iNoSaveReviewImage, iNoCosmeticReviewImage, iNoFaiReviewImage, iReviewSaveCnt;
	iNoSaveReviewImage = iNoCosmeticReviewImage = iNoFaiReviewImage = iReviewSaveCnt = 0;

	try
	{
		CString sLotID, sTrayID;
		int iMzNo, iStageNo, iJigNo;
		int iTrayNo, iModuleNo, iNoInspectImage;
		int iNoGrabRetry, iNoFocusMoveRetry;

		CString sVisionCamType_Short;
		sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iPcVisionNo];

		strLog.Format("%s/ ResultSaveThread_V%d start", sVisionCamType_Short, iVisionCamType + 1);
		THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

		iNoInspectImage = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage;

		while (TRUE)
		{
			if (pAlgorithm->IsScanThreadParamEmpty())
			{
				strLog.Format("%s/ ResultSaveThread_N%d end", sVisionCamType_Short, iVisionCamType + 1);
				THEAPP.m_log_thread->debug("{}", LOG_CSTR(strLog));

				pAlgorithm->ChangeResultSaveThreadRunning(FALSE);

				return 1;
			}

			SCAN_THREAD_PARAM* pScanThreadParam = pAlgorithm->GetNextScanThreadParam();
			if (pScanThreadParam == NULL)
				continue;

			iInspectionBufferIdx = pScanThreadParam->iScanBufferIdx;
			if (iInspectionBufferIdx < 0 || iInspectionBufferIdx >= THEAPP.m_ModelDefineInfo.m_iMaxInspectionBufferCount[iVisionCamType])
				continue;

			SAFE_DELETE(pScanThreadParam);

			while (1)
			{
				if (pAlgorithm->IsInspectDone(iInspectionBufferIdx))
					break;

				Sleep(50);
			}

			iNoSaveReviewImage = iNoCosmeticReviewImage = iNoFaiReviewImage = iReviewSaveCnt = 0;
			bLogSaved = FALSE;

			sLotID = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_sLotID[iInspectionBufferIdx];
			iMzNo = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_nMzNo[iInspectionBufferIdx];
			iStageNo = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_nStageNo[iInspectionBufferIdx];
			iJigNo = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_nJigNo[iInspectionBufferIdx];
			sTrayID = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_sTrayID[iInspectionBufferIdx];
			iTrayNo = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_nTrayNo[iInspectionBufferIdx];
			iModuleNo = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_nModuleNo[iInspectionBufferIdx];
			iNoGrabRetry = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoGrabRetry[iInspectionBufferIdx];
			iNoFocusMoveRetry = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoFocusMoveRetry[iInspectionBufferIdx];
			int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

			int iMaxModuleTray = THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_iTrayArrayX * THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_iTrayArrayY;

			BOOL bGrabFail = FALSE;
			bGrabFail = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_bGrabFail[iInspectionBufferIdx];

			// FAI 데이터를 위한 참조체
			int& refMzNo = iMzNo;
			int& refTrayNo = iTrayNo;
			int& refModuleNo = iModuleNo;
			CenterlineMeasureStruct& measureStruct = CFAIDataManager::GetInstance().GetMeasure(refMzNo, refTrayNo, refModuleNo, FAI_PARAMETER_NUMBER_1);

			strLog.Format("%s/ Result save start, Time(ms): -, LotID: %s, Port: %d, Tray: %d, Module: %d, iInspectionBufferIdx: %d", sVisionCamType_Short, sLotID, iMzNo, iTrayNo, iModuleNo, iInspectionBufferIdx);
			THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

			//////////////////////////////////////////////////////////////////////////
			// 변수 초기화
			//////////////////////////////////////////////////////////////////////////
			int iDefectCodeIndex = -1;
			CString sDefectCode, sFaiDefectCode, sDefectDetail, sDefectCodeInspectionType[MAX_INSPECTION_TYPE], sAdjDefectName[MAX_INSPECTION_TYPE];
			sDefectCode = _T("");

			for (int iii = 0; iii < MAX_INSPECTION_TYPE; iii++)
				sDefectCodeInspectionType[iii] = _T("G");

			CString sSection = "Result";
			CString sKey;
			CString sTrayResult;
			sTrayResult = "G";

			BOOL bSpecialDefect = FALSE;
			CString sSpecialDefectTransCode = _T("N");
			int iDefectTransCode, iCosmeticDefectTransCode, iFaiDefectTransCode;

			int i, j, k;
			BOOL bResultSet;
			int iDefectPriorityNum, iDefectIndex;

			int iInspectionTypeResult;
			int iInspectionTypeDefect[MAX_INSPECTION_TYPE];
			int iInspectionTypeResultNo[MAX_INSPECTION_TYPE];
			int iInspectionTypeDefectPosX[MAX_INSPECTION_TYPE];
			int iInspectionTypeDefectPosY[MAX_INSPECTION_TYPE];
			int iInspectionTypeDefectArea[MAX_INSPECTION_TYPE];

			HObject HVisionAllDefectRgn;
			BOOL bCheckContinuousDefect = FALSE;

			auto total_log_time_start = std::chrono::high_resolution_clock::now();

			//////////////////////////////////////////////////////////////////////////
			// PWM Align 처리
			//////////////////////////////////////////////////////////////////////////
			BOOL bAlignNGCheck = FALSE;
			if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[iVisionCamType])
			{
				int		iAlignNoInspectModule;
				HTuple	HAlignModuleExist;
				HTuple	HAlignModuleIsDefect;
				HTuple	HAlignModuleDefectPosX;
				HTuple	HAlignModuleDefectPosY;
				HTuple	HAlignModuleDeltaX;
				HTuple	HAlignModuleDeltaY;
				HTuple	HAlignModuleDeltaAngle;

				iAlignNoInspectModule = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iAlignNoInspectModule[iInspectionBufferIdx];
				HAlignModuleExist = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HAlignModuleExist[iInspectionBufferIdx];
				HAlignModuleIsDefect = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HAlignModuleIsDefect[iInspectionBufferIdx];
				HAlignModuleDefectPosX = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HAlignModuleDefectPosX[iInspectionBufferIdx];
				HAlignModuleDefectPosY = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HAlignModuleDefectPosY[iInspectionBufferIdx];
				HAlignModuleDeltaX = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HAlignModuleDeltaX[iInspectionBufferIdx];
				HAlignModuleDeltaY = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HAlignModuleDeltaY[iInspectionBufferIdx];
				HAlignModuleDeltaAngle = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HAlignModuleDeltaAngle[iInspectionBufferIdx];

				int iDefectError = DEFECT_TYPE_GOOD;

				if (pInspectService->m_pInspectAlgorithm[iVisionCamType].m_bLightDisorder[iInspectionBufferIdx])	/// 조명 이상인 경우
					iDefectError = DEFECT_TYPE_LIGHT_ERROR;
				else if (bGrabFail)
					iDefectError = DEFECT_TYPE_GRAB_FAIL;
				else
				{
					CString sAlignResult = "";
					bGrabFail = FALSE;

					for (int i = 0; i < iAlignNoInspectModule; i++)
					{
						if (HAlignModuleExist[i] == 0)
						{
							bAlignNGCheck = TRUE;

							if (i == 0)
								sAlignResult.Format("0");
							else
								sAlignResult.Format("%s,0", sAlignResult);
						}
						else
						{
							if (HAlignModuleIsDefect[i] == 1)
							{
								bAlignNGCheck = TRUE;

								if (i == 0)
									sAlignResult.Format("2");
								else
									sAlignResult.Format("%s,2", sAlignResult);
							}
							else
							{
								if (i == 0)
									sAlignResult.Format("1");
								else
									sAlignResult.Format("%s,1", sAlignResult);
							}
						}
					}

					THEAPP.m_pHandlerService->Set_TrayComplete(sLotID, iMzNo, iTrayNo, sAlignResult);
					if (bAlignNGCheck)
						iDefectError = DEFECT_TYPE_MODULE;
					else
						iDefectError = DEFECT_TYPE_GOOD;
				}
			}

			iCosmeticDefectTransCode = 0;
			iFaiDefectTransCode = 0;

			//////////////////////////////////////////////////////////////////////////
			// 매칭 성공 시 검사결과 처리
			//////////////////////////////////////////////////////////////////////////
			if (pInspectService->m_pInspectAlgorithm[iVisionCamType].m_bMatchingSuccess[iInspectionBufferIdx])
			{
				IInspectAdminViewHideDlg* pViewDlg = GetInspectViewDlg(iPcVisionNo);

				pViewDlg->m_pMzNo = iMzNo;
				pViewDlg->m_pTrayNo = iTrayNo;
				pViewDlg->m_pModuleNo = iModuleNo;

				int iTempNoFAIADJReviewImage = 0;

				// Start: FAI ROS Decision
				measureStruct.m_bDoROS = FALSE;
				BOOL bFAI_ROS_Skip = FALSE;
				BOOL bMixedCaseExist = FALSE;

				for (i = 0; i < MAX_FAI_ITEM; i++)
				{
					if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMInspectFai[i] == FALSE)
						continue;

					if (measureStruct.m_bParamResultNG[i])
					{
						measureStruct.m_iFAIResultCase[i] = FAI_RESULT_CASE_NG;

						bFAI_ROS_Skip = TRUE;
					}
					else
						measureStruct.m_iFAIResultCase[i] = FAI_RESULT_CASE_GOOD;
				}

				if (bFAI_ROS_Skip == FALSE && bMixedCaseExist == TRUE)
					measureStruct.m_bDoROS = TRUE;
				// End: FAI ROS Decision

				//////////////////////////////////////////////////////////////////////////
				// 검사타입별 처리
				//////////////////////////////////////////////////////////////////////////
				GenEmptyObj(&HVisionAllDefectRgn);

				iInspectionTypeResult = DEFECT_TYPE_GOOD;
				for (i = 0; i < MAX_INSPECTION_TYPE; i++)
				{
					iInspectionTypeDefect[i] = DEFECT_TYPE_GOOD;
					iInspectionTypeDefectPosX[i] = -1;
					iInspectionTypeDefectPosY[i] = -1;
					iInspectionTypeResultNo[i] = -1;
					iInspectionTypeDefectArea[i] = -1;
				}

				for (i = 0; i < MAX_INSPECTION_TYPE; i++)
				{
					for (j = 0; j < MAX_DEFECT_NAME; j++)
						GenEmptyObj(&(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j]));
				}

				Hlong lNoReviewDefect;
				HTuple HNoReviewDefect;

				auto log_time_start = std::chrono::high_resolution_clock::now();
				for (i = 0; i < MAX_INSPECTION_TYPE; i++)
				{
					//////////////////////////////////////////////////////////////////////////
					// Barcode 처리
					//////////////////////////////////////////////////////////////////////////
					if ((i + 1) == INSPECTION_TYPE_BARCODE && THEAPP.m_ModelDefineInfo.m_bVisionBarcode[iPcVisionNo])		// Barcode
					{
						GenEmptyObj(&(pViewDlg->m_HBarcodeDetectRgn));

						pViewDlg->m_bBarcodeRegResult = THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult;
						pViewDlg->m_bBarcodeShiftNG = THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeShiftNG;
						pViewDlg->m_bBarcodeRotationNG = THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRotationNG;

#ifndef INLINE_MODE
						EnterCriticalSection(&THEAPP.m_csBarcodeRead);
#endif

						pViewDlg->m_sBarcodeResult = THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1];

#ifndef INLINE_MODE
						LeaveCriticalSection(&THEAPP.m_csBarcodeRead);
#endif

						pViewDlg->m_dBarcodePosOffsetX = THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dBarcodePosOffsetX;
						pViewDlg->m_dBarcodePosOffsetY = THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_dBarcodePosOffsetY;
						pViewDlg->m_iBarcodeRotationResult = THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_iBarcodeRotationResult;
						pViewDlg->m_HBarcodeDetectRgn = THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_HBarcodeDetectRgn;

						if (THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRegResult == FALSE)	// 바코드 미인식
						{
							iInspectionTypeDefect[i] = i;
							iInspectionTypeResult = i;

							iDefectCodeIndex = DEFECT_CODE_BARCODE_READ;

							sDefectDetail = THEAPP.GetDefectCode(iDefectCodeIndex, &iDefectTransCode);
							if (iDefectTransCode > iCosmeticDefectTransCode)
								iCosmeticDefectTransCode = iDefectTransCode;

							sDefectCodeInspectionType[i].Format("BA_B_%s", sDefectDetail);
							sAdjDefectName[i] = sDefectDetail;
						}
						else		// 바코드 인식 성공
						{
							if (THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeShiftNG == TRUE || THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRotationNG == TRUE)		// 바코드 Shift / Rotation
							{
								iInspectionTypeDefect[i] = i;
								iInspectionTypeResult = i;

								if (THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeShiftNG)
									iDefectCodeIndex = DEFECT_CODE_BARCODE_SHIFT;
								else if (THEAPP.m_StructModuleBarcodeResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bBarcodeRotationNG)
									iDefectCodeIndex = DEFECT_CODE_BARCODE_ROTATION;

								sDefectDetail = THEAPP.GetDefectCode(iDefectCodeIndex, &iDefectTransCode);
								if (iDefectTransCode > iCosmeticDefectTransCode)
									iCosmeticDefectTransCode = iDefectTransCode;

								sDefectCodeInspectionType[i].Format("BA_B_%s", sDefectDetail);
								sAdjDefectName[i] = sDefectDetail;
							}
						}
					}
					else
					{
						//////////////////////////////////////////////////////////////////////////
						// 일반 검사타입 처리
						//////////////////////////////////////////////////////////////////////////
						for (j = 0; j < MAX_DEFECT_NAME; j++)
						{
							pViewDlg->iDefectSortingNumber[j] = -1;

							GenEmptyObj(&(pViewDlg->m_HSpecApplyDefectRgn[j]));

							if (CGFunction::ValidHRegion(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_DefectName[iInspectionBufferIdx][i][j]))
							{
								pViewDlg->iDefectSortingNumber[j] = THEAPP.m_pAlgorithm->ApplyInspectionCondition(iVisionCamType,
									pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_DefectName[iInspectionBufferIdx][i][j], &(pViewDlg->m_HSpecApplyDefectRgn[j]),
									THEAPP.m_StructCountPerAreaConditionInfo.m_dMinArea[i][j], THEAPP.m_StructCountPerAreaConditionInfo.m_dMidArea[i][j], THEAPP.m_StructCountPerAreaConditionInfo.m_dMaxArea[i][j],
									THEAPP.m_StructCountPerAreaConditionInfo.m_iMinNumAccept[i][j], THEAPP.m_StructCountPerAreaConditionInfo.m_iMidNumAccept[i][j], THEAPP.m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[i][j],
									THEAPP.GetPCID(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.m_pDualModelDataManager[iDualModelData][iVisionCamType]->m_sModelName,
									iMzNo, sLotID, iTrayNo, iModuleNo, THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1],
									sVisionCamType_Short, THEAPP.Struct_PreferenceStruct.m_iPCType + 1, iStageNo + 1,
									g_strInspectionTypeName_Short[i].c_str(), THEAPP.GetDefectCode(j, &iDefectTransCode));

								if (CGFunction::ValidHRegion(pViewDlg->m_HSpecApplyDefectRgn[j]))	// 불량 존재
								{
									ConcatObj(HVisionAllDefectRgn, pViewDlg->m_HSpecApplyDefectRgn[j], &HVisionAllDefectRgn);

									ConcatObj(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j], pViewDlg->m_HSpecApplyDefectRgn[j], &(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j]));

									iInspectionTypeDefect[i] = i;
									iInspectionTypeResult = i;

									// Continuous Defect Alarm
									if (THEAPP.Struct_PreferenceStruct.m_bUseContDefectAlarm == TRUE)
									{
										if ((iTrayNo - 1) >= 0 && (iTrayNo - 1) < MAX_TRAY_LOT && (iModuleNo - 1) >= 0 && (iModuleNo - 1) < MAX_MODULE_TRAY)
										{
											ConcatObj(THEAPP.m_StructModuleContDefect[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_HDefectRgn[i], pViewDlg->m_HSpecApplyDefectRgn[j], &(THEAPP.m_StructModuleContDefect[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_HDefectRgn[i]));

											THEAPP.m_StructModuleContDefect[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bInspectionTypeDefect[i] = TRUE;

											bCheckContinuousDefect = TRUE;
										}
									}
								}
							}
						}	// for (j=0; j<MAX_DEFECT_NAME; j++)
					}

					//////////////////////////////////////////////////////////////////////////
					// Inspection Type 결과 이미지 저장
					//////////////////////////////////////////////////////////////////////////
					int iResultImageIndex = -1;
					for (j = 0; j <= MAX_RESULT_IMAGE; j++)
					{
						if (j == MAX_RESULT_IMAGE)
						{
							iResultImageIndex = -1;
							break;
						}

						if (g_strInspectionTypeName[i] != _T("") && g_strInspectionTypeName[i].c_str() == THEAPP.m_ModelDefineInfo.m_strResultInspectionTypeName[iPcVisionNo][j])
						{
							iResultImageIndex = j;
							break;
						}
					}

					if (iResultImageIndex == -1)
						continue;

					pViewDlg->OverlaySetViewportManager(THEAPP.Struct_PreferenceStruct.iOverlayImageViewportLeft[iVisionCamType][iResultImageIndex], THEAPP.Struct_PreferenceStruct.iOverlayImageViewportTop[iVisionCamType][iResultImageIndex], THEAPP.Struct_PreferenceStruct.dOverlayImageViewportZoomRatio[iVisionCamType][iResultImageIndex]);
					pViewDlg->WriteResultFile(i, iPcVisionNo, iVisionCamType, iResultImageIndex, pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HInspectCImage[iInspectionBufferIdx][CHANNEL_TYPE_COLOR][THEAPP.Struct_PreferenceStruct.iOverlayImageIndex[iVisionCamType][iResultImageIndex]], pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_InspectionType[iInspectionBufferIdx][i], &iDefectCodeIndex, THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1]);

					// 검사타입별 불량 위치 확인 - ContactPoint 로그
					if (CGFunction::ValidHRegion(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_InspectionType[iInspectionBufferIdx][i]) == TRUE)
					{
						HObject HBlob;
						HTuple HArea, HRow, HCol;

						Union1(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_InspectionType[iInspectionBufferIdx][i], &HBlob);
						AreaCenter(HBlob, &HArea, &HRow, &HCol);
						iInspectionTypeResultNo[i] = iResultImageIndex + 1;
						iInspectionTypeDefectPosX[i] = HCol.D();
						iInspectionTypeDefectPosY[i] = HRow.D();
						iInspectionTypeDefectArea[i] = HArea.D();
					}

					sDefectDetail = THEAPP.GetDefectCode(iDefectCodeIndex, &iDefectTransCode);
					if (iDefectTransCode > iCosmeticDefectTransCode)
						iCosmeticDefectTransCode = iDefectTransCode;

					CString strVisionNameShortcut = sVisionCamType_Short.Left(1);
					sDefectCodeInspectionType[i].Format("%s_%s_%s", g_strInspectionTypeName_Short[i].c_str(), strVisionNameShortcut, sDefectDetail);
					sAdjDefectName[i] = sDefectDetail;

					// Continuous Defect Alarm
					if (THEAPP.Struct_PreferenceStruct.m_bUseContDefectAlarm)
					{
						if ((i + 1) >= INSPECTION_TYPE_FIRST && (i + 1) <= INSPECTION_TYPE_LAST)
						{
							if (THEAPP.m_StructModuleContDefect[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bInspectionTypeDefect[i])
								Union1(THEAPP.m_StructModuleContDefect[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_HDefectRgn[i], &(THEAPP.m_StructModuleContDefect[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_HDefectRgn[i]));
						}
					}
				}

				auto log_time_end = std::chrono::high_resolution_clock::now();
				auto log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

				strLog.Format("%s/ Save result image, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Short, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo);
				THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));

				// Continuous Defect Alarm
				if (THEAPP.Struct_PreferenceStruct.m_bUseContDefectAlarm == TRUE)
				{
					if (bCheckContinuousDefect)
						THEAPP.m_StructModuleContDefect[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].m_bModuleDefect = TRUE;
				}

				//////////////////////////////////////////////////////////////////////////
				// Inspection Type 별 불량 우선 순위
				//////////////////////////////////////////////////////////////////////////
				if (iInspectionTypeResult != DEFECT_TYPE_GOOD)
				{
					bResultSet = FALSE;

					for (iDefectPriorityNum = 0; iDefectPriorityNum < MAX_INSPECTION_TYPE; iDefectPriorityNum++)
					{
						for (iDefectIndex = 0; iDefectIndex < MAX_INSPECTION_TYPE; iDefectIndex++)
						{
							if (THEAPP.Struct_PreferenceStruct.iInspectionTypePriority[iDefectIndex] == iDefectPriorityNum)
							{
								if (iInspectionTypeDefect[iDefectIndex] >= 0)
								{
									iInspectionTypeResult = iInspectionTypeDefect[iDefectIndex];
									bResultSet = TRUE;
								}
							}

							if (bResultSet == TRUE)
								break;
						}

						if (bResultSet == TRUE)
							break;
					}
				}

				/////////// Vision Result Data
				if (iInspectionTypeResult >= DEFECT_TYPE_FIRST && iInspectionTypeResult <= DEFECT_TYPE_LAST)
					THEAPP.m_sInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = sDefectCodeInspectionType[iInspectionTypeResult];
				else
					THEAPP.m_sInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "G";
				///////////

				if (THEAPP.Struct_PreferenceStruct.m_iADJCombineDefectDistance > 0)
				{
					GenEmptyObj(&HVisionAllDefectRgn);

					for (i = 0; i < MAX_INSPECTION_TYPE; i++)
					{
						if ((i + 1) == INSPECTION_TYPE_BARCODE)		// Barcode
							continue;

						for (j = 0; j < MAX_DEFECT_NAME; j++)
						{
							if (CGFunction::ValidHRegion(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j]) == TRUE)
							{
								pAlgorithm->BlobUnion(&(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j]), THEAPP.Struct_PreferenceStruct.m_iADJCombineDefectDistance);
								ConcatObj(HVisionAllDefectRgn, pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j], &HVisionAllDefectRgn);
							}
						}
					}
				}

				if (CGFunction::ValidHRegion(HVisionAllDefectRgn))
					Union1(HVisionAllDefectRgn, &HVisionAllDefectRgn);

				//////////////////////////////////////////////////////////////////////////
				// Save ADJ Image
				for (int nTabIdx = 0; nTabIdx < MAX_IMAGE_TAB; nTabIdx++)
					GenEmptyObj(&THEAPP.HADJFilteredDefectRgn[iPcVisionNo][nTabIdx]);
				iReviewSaveCnt = 0;
				HTuple HNoDefect;
				Hlong lNoDefectCount;
				if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[0] || THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[1])
				{
					if (CGFunction::ValidHRegion(HVisionAllDefectRgn))
					{
						CountObj(HVisionAllDefectRgn, &HNoDefect);
						lNoDefectCount = HNoDefect[0].I();
						strLog.Format("ADJ Inspection start, count: %d", lNoDefectCount);
						THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));

						DWORD iModuleTotalStartTime = GetTickCount();

#ifdef USE_SUAKIT
						iReviewSaveCnt = THEAPP.m_TCPClientService.DoDeeplearningInspection(iVisionCamType,
							iInspectionBufferIdx,
							iVisionCamType,
							HVisionAllDefectRgn,
							iPcVisionNo,
							iNoInspectImage,
							iMzNo,
							iTrayNo,
							iModuleNo,
							sLotID,
							THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1],
							iModuleTotalStartTime,
							THEAPP.HADJFilteredDefectRgn[iPcVisionNo],
							iInspectionTypeResult);
#endif
						CountObj(HVisionAllDefectRgn, &HNoDefect);
						lNoDefectCount = HNoDefect[0].I();

						DWORD iModuleTotalEndTime = GetTickCount() - iModuleTotalStartTime;
						strLog.Format("ResultSaveThread_N%d inspection end, Module elapsed time(ms):%d, LotID: %s, Tray: %d, Module: %d, CountResult: %d, ReviewImage save: %d", iVisionCamType + 1, iModuleTotalEndTime, sLotID, iTrayNo, iModuleNo, lNoDefectCount, iReviewSaveCnt);
						THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));
					}
				}	// if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[0] || THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[1])

				/////////// Vision Adj Result Data
				if (iInspectionTypeResult >= DEFECT_TYPE_FIRST && iInspectionTypeResult <= DEFECT_TYPE_LAST)
				{
					THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = sDefectCodeInspectionType[iInspectionTypeResult];

					// 검사타입별 불량 위치 확인 - ContactPoint 로그
					THEAPP.m_StructDefectBlobInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo].sInspectType = g_strInspectionTypeName[iInspectionTypeResult].c_str();
					THEAPP.m_StructDefectBlobInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo].iArea = iInspectionTypeDefectArea[iInspectionTypeResult];
					THEAPP.m_StructDefectBlobInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo].iDefectCenterX = iInspectionTypeDefectPosX[iInspectionTypeResult];
					THEAPP.m_StructDefectBlobInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo].iDefectCenterY = iInspectionTypeDefectPosY[iInspectionTypeResult];
					THEAPP.m_StructDefectBlobInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo].iResultImageNo = iInspectionTypeResultNo[iInspectionTypeResult];
				}
				else
				{
					if (iInspectionTypeResult == DEFECT_TYPE_LIGHT_ERROR)
						THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "LI";
					else if (iInspectionTypeResult == DEFECT_TYPE_GRAB_FAIL)
						THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "GF";
					else if (iInspectionTypeResult == DEFECT_TYPE_ALIGN_ERROR)
						THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "MC";
					else if (iInspectionTypeResult == DEFECT_TYPE_GOOD)
						THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "G";
				}
				///////////

				//////////////////////////////////////////////////////////////////////////
				if (THEAPP.Struct_PreferenceStruct.m_bUseSaveFaiReviewImage)
				{
					HObject HReviewXLD_FAI_Item_Copied[MAX_FAI_ITEM][MAX_FAI_REVIEW_IMAGE * MAX_FAI_PARAMETER];
					POINT iViewportCenter_FAI_Item_Copied[MAX_FAI_ITEM][MAX_FAI_REVIEW_IMAGE * MAX_FAI_PARAMETER];

					for (int i = 0; i < MAX_FAI_ITEM; i++)
					{
						if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMInspectFai[i] == FALSE)
							continue;

						for (int j = 0; j < MAX_FAI_PARAMETER; j++)
						{
							for (int k = 0; k < MAX_FAI_REVIEW_IMAGE; k++)
							{
								GenEmptyObj(&HReviewXLD_FAI_Item_Copied[i][j * MAX_FAI_REVIEW_IMAGE + k]);

								if (THEAPP.m_pGFunction->ValidHXLD(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HReviewXLD_FAI_Item[iInspectionBufferIdx][i][j][k]))
									HReviewXLD_FAI_Item_Copied[i][j * MAX_FAI_REVIEW_IMAGE + k] = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HReviewXLD_FAI_Item[iInspectionBufferIdx][i][j][k];

								iViewportCenter_FAI_Item_Copied[i][j * MAX_FAI_REVIEW_IMAGE + k] = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iViewportCenter_FAI_Item[iInspectionBufferIdx][i][j][k];
							}
						}
					}

					int iTempNoReviewImage = 0;
					pViewDlg->WriteSelectedRosReviewFile_FAI(iMzNo, iTrayNo, iModuleNo, iPcVisionNo, iVisionCamType, iNoInspectImage, pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HInspectCImage[iInspectionBufferIdx][CHANNEL_TYPE_COLOR], HReviewXLD_FAI_Item_Copied, iViewportCenter_FAI_Item_Copied, THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1], &iTempNoReviewImage);
				}

				// 불량 판정
				if (CGFunction::ValidHRegion(HVisionAllDefectRgn))
				{
					sTrayResult = "N";

					HObject HRemainVisionAllDefectRgn;
					GenEmptyObj(&HRemainVisionAllDefectRgn);

					HObject HDefectRgn_ImageNo_Copied[MAX_IMAGE_TAB];
					for (int nTabIdx = 0; nTabIdx < MAX_IMAGE_TAB; nTabIdx++)
					{
						GenEmptyObj(&HDefectRgn_ImageNo_Copied[nTabIdx]);
						if (CGFunction::ValidHRegion(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][nTabIdx]))
						{
							HDefectRgn_ImageNo_Copied[nTabIdx] = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HDefectRgn_ImageNo[iInspectionBufferIdx][nTabIdx];
							if (THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ && CGFunction::ValidHRegion(THEAPP.HADJFilteredDefectRgn[iPcVisionNo][nTabIdx]))
								Difference(HDefectRgn_ImageNo_Copied[nTabIdx], THEAPP.HADJFilteredDefectRgn[iPcVisionNo][nTabIdx], &HDefectRgn_ImageNo_Copied[nTabIdx]);

							if (CGFunction::ValidHRegion(HDefectRgn_ImageNo_Copied[nTabIdx]))
								ConcatObj(HRemainVisionAllDefectRgn, HDefectRgn_ImageNo_Copied[nTabIdx], &HRemainVisionAllDefectRgn);
						}

						for (i = 0; i < MAX_INSPECTION_TYPE; i++)
						{
							for (j = 0; j < MAX_DEFECT_NAME; j++)
							{
								if (CGFunction::ValidHRegion(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j]) == TRUE)
								{
									Union1(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j], &(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j]));

									if (THEAPP.Struct_PreferenceStruct.m_bIsApplyADJ && CGFunction::ValidHRegion(THEAPP.HADJFilteredDefectRgn[iPcVisionNo][nTabIdx]))
										Difference(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j], THEAPP.HADJFilteredDefectRgn[iPcVisionNo][nTabIdx], &(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j]));
								}
							}
						}
					}

					CString sAllDefectsResultCode = "";
					for (i = 0; i < MAX_INSPECTION_TYPE; i++)
					{
						for (j = 0; j < MAX_DEFECT_NAME; j++)
						{
							if (CGFunction::ValidHRegion(pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j]))
							{
								HObject HIntersectRgn;
								for (int k = 0; k < MAX_IMAGE_TAB; k++)
								{
									Intersection(HDefectRgn_ImageNo_Copied[k], pViewDlg->m_HRosReviewSpecApplyDefectRgn[i][j], &HIntersectRgn);

									if (CGFunction::ValidHRegion(HIntersectRgn))
										break;
								}

								if (CGFunction::ValidHRegion(HIntersectRgn) == FALSE)
									continue;

								CString strVisionNameShortcut = sVisionCamType_Short.Left(1);
								sAllDefectsResultCode.Format("%s\t%s-%s-%s", sAllDefectsResultCode, g_strInspectionTypeName_Short[i].c_str(), strVisionNameShortcut, THEAPP.m_strDefectCode[j]);
							}
						}
					}

					THEAPP.m_sAllDefectsResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = sAllDefectsResultCode;

					if (CGFunction::ValidHRegion(HRemainVisionAllDefectRgn) == FALSE)
						sTrayResult = "G";

					// ROS
					if (THEAPP.Struct_PreferenceStruct.m_bSaveMDJ)
					{
						log_time_start = std::chrono::high_resolution_clock::now();

						pViewDlg->OverlaySetViewportManager(THEAPP.Struct_PreferenceStruct.iRosImageViewportLeft[iVisionCamType], THEAPP.Struct_PreferenceStruct.iRosImageViewportTop[iVisionCamType], THEAPP.Struct_PreferenceStruct.dRosImageViewportZoomRatio[iVisionCamType]);
						pViewDlg->WriteSelectedRosReviewFile(iPcVisionNo, iVisionCamType, iNoInspectImage, pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HInspectCImage[iInspectionBufferIdx][CHANNEL_TYPE_COLOR], HDefectRgn_ImageNo_Copied, THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1], &iNoCosmeticReviewImage);

						if (THEAPP.Struct_PreferenceStruct.m_bUseSaveFaiReviewImage == FALSE)
						{
							if (measureStruct.m_bInspectFail[iPcVisionNo] == TRUE)
							{
								HObject HReviewXLD_FAI_Item_Copied[MAX_FAI_ITEM][MAX_FAI_REVIEW_IMAGE * MAX_FAI_PARAMETER];
								POINT iViewportCenter_FAI_Item_Copied[MAX_FAI_ITEM][MAX_FAI_REVIEW_IMAGE * MAX_FAI_PARAMETER];

								for (int i = 0; i < MAX_FAI_ITEM; i++)
								{
									if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMInspectFai[i] == FALSE)
										continue;

									if (measureStruct.m_bParamResultNG[i] == FALSE)
										continue;

									for (int j = 0; j < MAX_FAI_PARAMETER; j++)
									{
										for (int k = 0; k < MAX_FAI_REVIEW_IMAGE; k++)
										{
											GenEmptyObj(&HReviewXLD_FAI_Item_Copied[i][j * MAX_FAI_REVIEW_IMAGE + k]);

											if (THEAPP.m_pGFunction->ValidHXLD(pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HReviewXLD_FAI_Item[iInspectionBufferIdx][i][j][k]))
												HReviewXLD_FAI_Item_Copied[i][j * MAX_FAI_REVIEW_IMAGE + k] = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HReviewXLD_FAI_Item[iInspectionBufferIdx][i][j][k];

											iViewportCenter_FAI_Item_Copied[i][j * MAX_FAI_REVIEW_IMAGE + k] = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iViewportCenter_FAI_Item[iInspectionBufferIdx][i][j][k];
										}
									}
								}

								pViewDlg->WriteSelectedRosReviewFile_FAI(iMzNo, iTrayNo, iModuleNo, iPcVisionNo, iVisionCamType, iNoInspectImage, pInspectService->m_pInspectAlgorithm[iVisionCamType].m_HInspectCImage[iInspectionBufferIdx][CHANNEL_TYPE_COLOR], HReviewXLD_FAI_Item_Copied, iViewportCenter_FAI_Item_Copied, THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1], &iNoFaiReviewImage);
							}
						}

						log_time_end = std::chrono::high_resolution_clock::now();
						log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(log_time_end - log_time_start).count();

						strLog.Format("%s/ Save review image, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d", sVisionCamType_Short, log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo);
						THEAPP.m_log_image->info("{}", LOG_CSTR(strLog));
					}
				}
				else
					sTrayResult = "G";

				for (int nTabIdx = 0; nTabIdx < MAX_IMAGE_TAB; nTabIdx++)
					GenEmptyObj(&THEAPP.HADJFilteredDefectRgn[iPcVisionNo][nTabIdx]);
			}
			else		// 매칭 실패이면
			{
				int iDefectError = DEFECT_TYPE_ALIGN_ERROR;

				if (pInspectService->m_pInspectAlgorithm[iVisionCamType].m_bLightDisorder[iInspectionBufferIdx])	/// 조명 이상인 경우
					iDefectError = DEFECT_TYPE_LIGHT_ERROR;
				else if (bGrabFail)
					iDefectError = DEFECT_TYPE_GRAB_FAIL;
				else
					iDefectError = DEFECT_TYPE_ALIGN_ERROR;

				iInspectionTypeResult = iDefectError;

				if (iInspectionTypeResult == DEFECT_TYPE_LIGHT_ERROR)
				{
					THEAPP.m_sInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "LI";
					THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "LI";
				}
				else if (iInspectionTypeResult == DEFECT_TYPE_GRAB_FAIL)
				{
					THEAPP.m_sInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "GF";
					THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "GF";
				}
				else if (iInspectionTypeResult == DEFECT_TYPE_ALIGN_ERROR)
				{
					THEAPP.m_sInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "MC";
					THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iPcVisionNo] = "MC";
				}

				sTrayResult = "N";
			}

			if (THEAPP.m_ModelDefineInfo.m_bVisionPWM[iVisionCamType] && bAlignNGCheck)
				iInspectionTypeResult = DEFECT_TYPE_MODULE;

			//////////////////////////////////////////////////////////////////////////
		   // TrayAdminViewDlg 업데이트 (화면에 표시할 문자 설정, 검사 진행 다이어그램 업데이트)
		   //////////////////////////////////////////////////////////////////////////
			auto pTrayDlg = GetTrayViewDlg(iPcVisionNo);
			pTrayDlg->GetTrayImagePB().OkNg[iModuleNo - 1] = iInspectionTypeResult;
			pTrayDlg->GetTrayImagePB().InvalidateRect(false);

			// Defect Code (To 제어 프로그램)
			if (iInspectionTypeResult == DEFECT_TYPE_GOOD)
				sDefectCode = _T("G");
			else
			{
				if (iInspectionTypeResult == DEFECT_TYPE_LIGHT_ERROR)
					sDefectCode.Format("LI");
				else if (iInspectionTypeResult == DEFECT_TYPE_GRAB_FAIL)
					sDefectCode.Format("GF");
				else if (iInspectionTypeResult == DEFECT_TYPE_ALIGN_ERROR)
					sDefectCode.Format("MC");
				else
				{
					if (iInspectionTypeResult >= DEFECT_TYPE_FIRST && iInspectionTypeResult <= DEFECT_TYPE_LAST)
						sDefectCode = sDefectCodeInspectionType[iInspectionTypeResult];
				}
			}

			pAlgorithm->ChangeInspectDone(iInspectionBufferIdx, BUFFER_STATUS_AVAILABLE);

			//////////////////////////////////////////////////////////////////////////
			// 로그 저장
			//////////////////////////////////////////////////////////////////////////

			// LotResult.txt 저장
			CString sTrayResultFileName;
			CString sLotSection = "Result";

			CString sSaveModuleResults, sSaveModuleFinalResults;
			CString sSaveModuleResult_Vision;

			SYSTEMTIME ModuleInspectTime;
			CString sModuleInspectDate, sModuleInspectTime, sSaveConfig;
			CString strLotSave, strPrintQualitySave;
			CString strADJLotResultLog;

			int iReadCnt = 0;

			// DaySummary.txt
			CString sSectionDay;
			BOOL bResultNG = FALSE;
			//

			CString sDefectName;

			sSaveModuleResults = "";
			sSaveModuleFinalResults = "";
			sSaveModuleResult_Vision = _T("");

			sSaveConfig = THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;

			if (ShouldSaveLog(iPcVisionNo))
			{
				//////////////////////////////////////////////////////////////////////////
				// Vision 1~4까지 순차적으로 로그 저장
				//////////////////////////////////////////////////////////////////////////
				for (int iVisionNo = VISION_NUMBER_1; iVisionNo <= VISION_NUMBER_4; iVisionNo++)
				{
					if (THEAPP.m_bScanCompleteFlag[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionNo] && THEAPP.Struct_PreferenceStruct.m_bUseVision[iVisionNo])
					{
						iReadCnt = 0;
						while (1)
						{
							if (THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionNo] != "nan")
							{
								sSaveModuleResult_Vision = THEAPP.m_sInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionNo];

								bResultNG = FALSE;
								if (sSaveModuleResult_Vision != "G")
									bResultNG = TRUE;

								//////////////////////////////////////////////////////////////////////////
								// DaySummary.txt
								sSectionDay = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iVisionNo];

								if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
								{
									if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
										THEAPP.SaveDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], sSectionDay, sSaveModuleResult_Vision, bResultNG, iMzNo, iNoGrabRetry, iNoFocusMoveRetry);

									if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
										THEAPP.SaveDaySummaryINI(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], sSectionDay, sSaveModuleResult_Vision, bResultNG, iMzNo, iNoGrabRetry, iNoFocusMoveRetry);
								}
								else
								{
									if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
									{
										RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
										pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_DAY_SUMMARY;
										pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
										pSaveLogThreadIDParam->strType1 = sSectionDay;
										pSaveLogThreadIDParam->strType2 = sSaveModuleResult_Vision;
										pSaveLogThreadIDParam->bType = bResultNG;
										pSaveLogThreadIDParam->iMzNo = iMzNo;
										pSaveLogThreadIDParam->iTrayNo = iTrayNo;
										pSaveLogThreadIDParam->iModuleNo = iModuleNo;
										pSaveLogThreadIDParam->iNoGrabRetry = iNoGrabRetry;
										pSaveLogThreadIDParam->iNoFocusMoveRetry = iNoFocusMoveRetry;
										THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
									}

									if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
									{
										RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
										pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_DAY_SUMMARY;
										pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
										pSaveLogThreadIDParam_Dual->strType1 = sSectionDay;
										pSaveLogThreadIDParam_Dual->strType2 = sSaveModuleResult_Vision;
										pSaveLogThreadIDParam_Dual->bType = bResultNG;
										pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
										pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
										pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
										pSaveLogThreadIDParam_Dual->iNoGrabRetry = iNoGrabRetry;
										pSaveLogThreadIDParam_Dual->iNoFocusMoveRetry = iNoFocusMoveRetry;
										THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
									}
								}

								sSaveModuleResults = sSaveModuleResults + "\t" + THEAPP.m_sInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionNo];
								sSaveModuleFinalResults = sSaveModuleFinalResults + "\t" + THEAPP.m_sFinalInspectionResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionNo];

								break;
							}

							Sleep(MAX_RESULT_WAIT_SLEEP_TIME);

							++iReadCnt;
							if (iReadCnt >= MAX_RESULT_WAIT_ITERATION)
							{
								sSaveModuleResult_Vision = "nan";

								sSaveModuleResults = sSaveModuleResults + "\t" + sSaveModuleResult_Vision;
								sSaveModuleFinalResults = sSaveModuleFinalResults + "\t" + sSaveModuleResult_Vision;

								break;
							}
						}
					}
				}

				GetLocalTime(&ModuleInspectTime);
				sModuleInspectDate.Format("%04d-%02d-%02d", ModuleInspectTime.wYear, ModuleInspectTime.wMonth, ModuleInspectTime.wDay);
				sModuleInspectTime.Format("%s %02d:%02d:%02d", sModuleInspectDate, ModuleInspectTime.wHour, ModuleInspectTime.wMinute, ModuleInspectTime.wSecond);

				// All Defect Log
				CString sAllDefectsResultModuleInfo;
				sAllDefectsResultModuleInfo.Format("%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t%s",
					sModuleInspectDate,
					sModuleInspectTime,
					THEAPP.GetPCID(),
					THEAPP.Struct_PreferenceStruct.m_strEquipNo,
					sLotID,
					THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName,
					iTrayNo,
					iModuleNo,
					THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1]);

				int iStartVision, iEndVision;
				GetAllDefectsVisionRange(iPcVisionNo, iStartVision, iEndVision);

				BOOL bHasDefects = FALSE;
				for (int iVisionIdx = iStartVision; iVisionIdx <= iEndVision; iVisionIdx++)
				{
					if (THEAPP.m_sAllDefectsResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionIdx] != "")
					{
						bHasDefects = TRUE;
						break;
					}
				}

				CString sAllDefectsResultLog = "";
				if (bHasDefects)
				{
					sAllDefectsResultLog = sAllDefectsResultModuleInfo;

					for (int iVisionIdx = iStartVision; iVisionIdx <= iEndVision; iVisionIdx++)
					{
						sAllDefectsResultLog += THEAPP.m_sAllDefectsResult[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iVisionIdx];
					}
				}

				if (sAllDefectsResultLog != "")
				{
					if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
					{
						if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
							THEAPP.SaveAllDefectsResultLog(THEAPP.m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sAllDefectsResultLog);
						if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
							THEAPP.SaveAllDefectsResultLog(THEAPP.m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sAllDefectsResultLog);
					}
					else
					{
						if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
						{
							RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
							pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_ALL_DEFECTS_RESULT;
							pSaveLogThreadIDParam_Edge->strPath = THEAPP.m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
							pSaveLogThreadIDParam_Edge->strType1 = sAllDefectsResultLog;
							pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
							pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
							THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
						}
						if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
						{
							RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
							pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_ALL_DEFECTS_RESULT;
							pSaveLogThreadIDParam_Edge->strPath = THEAPP.m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
							pSaveLogThreadIDParam_Edge->strType1 = sAllDefectsResultLog;
							pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
							pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
							THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
						}
					}
				}

				// Start: FAI Defect Name
				sModuleFAIDefectName = _T("");
				for (i = 0; i < MAX_FAI_ITEM; i++)
				{
					if (measureStruct.m_bFAI_ResultNG[i][0] == TRUE)
					{
						if (measureStruct.m_dFAIMeasureValue[i][0] == FAI_MEASURE_ERROR)
							sModuleFAIDefectName.AppendFormat("/F%s", g_strFAILogName[i]);
						else
							sModuleFAIDefectName.AppendFormat("/N%s", g_strFAILogName[i]);
					}
				}
				// End: FAI Defect Name

				CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
				std::vector<double> vecFaiValue = GetFAIValueVector(iMzNo, iTrayNo, iModuleNo, strModelType);

				// 결과 구성
				int visionCount = GetResultVisionCount(iPcVisionNo);
				int isPairOption = GetResultPairOption(iPcVisionNo);

				std::vector<CString> vecVisionResultList(visionCount); // Vision Result List 구성 (Vision 수에 따라 동적으로 변경)
				std::vector<CString> vecAIResultList(visionCount * 3); // AI Result List 구성 (Vision 수 * AI Step 종류 수)
				std::vector<CuScanApp::DefectBlobInfo> vecDefectBlobInfoList(visionCount);
				if (!GetAllResult(visionCount, vecVisionResultList, vecAIResultList, vecDefectBlobInfoList, false, iMzNo, iTrayNo, iModuleNo, isPairOption)) {

				}
				
				// LotResult Log
				CuScanApp::LogHeaderInfoStruct logHeaders =
				{
					sModuleInspectDate,
					sModuleInspectTime,
					THEAPP.GetPCID(),
					THEAPP.Struct_PreferenceStruct.m_strEquipNo,
					sLotID,
					sSaveConfig,
					iTrayNo,
					iModuleNo,
					iStageNo,
					iJigNo,
					THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1],
					vecVisionResultList,
					vecAIResultList,
					vecDefectBlobInfoList,
					sModuleFAIDefectName,
					vecFaiValue
				};

				strLotSave = THEAPP.FormatLotResultString(logHeaders);

				if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
						THEAPP.SaveLotResultLog(THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1], strLotSave, iMzNo);

					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
						THEAPP.SaveLotResultLog(THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1], strLotSave, iMzNo);

					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
						THEAPP.SaveDayLotResultLog(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], strLotSave, iMzNo);

					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
						THEAPP.SaveDayLotResultLog(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], strLotSave, iMzNo);
				}
				else
				{
					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_LOT_RESULT;
						pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1];
						pSaveLogThreadIDParam->strType1 = strLotSave;
						pSaveLogThreadIDParam->strType2 = _T("");
						pSaveLogThreadIDParam->bType = FALSE;
						pSaveLogThreadIDParam->iPcVisionNo = iPcVisionNo;
						pSaveLogThreadIDParam->iMzNo = iMzNo;
						pSaveLogThreadIDParam->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam->iModuleNo = iModuleNo;
						THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
					}

					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_LOT_RESULT_LASPC;
						pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1];
						pSaveLogThreadIDParam_Dual->strType1 = strLotSave;
						pSaveLogThreadIDParam_Dual->strType2 = _T("");
						pSaveLogThreadIDParam_Dual->bType = FALSE;
						pSaveLogThreadIDParam_Dual->iPcVisionNo = iPcVisionNo;
						pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
						pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
						THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
					}

					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_DAY_LOT_RESULT;
						pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
						pSaveLogThreadIDParam->strType1 = strLotSave;
						pSaveLogThreadIDParam->strType2 = _T("");
						pSaveLogThreadIDParam->iPcVisionNo = iPcVisionNo;
						pSaveLogThreadIDParam->iMzNo = iMzNo;
						pSaveLogThreadIDParam->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam->iModuleNo = iModuleNo;
						THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
					}

					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_DAY_LOT_RESULT;
						pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
						pSaveLogThreadIDParam_Dual->strType1 = strLotSave;
						pSaveLogThreadIDParam_Dual->strType2 = _T("");
						pSaveLogThreadIDParam_Dual->iPcVisionNo = iPcVisionNo;
						pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
						pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
						THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
					}
				}

				// ======================== Detect Info Log ========================
				CString sDetectInfo = THEAPP.FormatContactPointString(logHeaders);

				if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
						THEAPP.SaveContactPointLog(THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1], strLotSave, iMzNo);

					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
						THEAPP.SaveContactPointLog(THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1], strLotSave, iMzNo);
				}
				else
				{
					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_CONTACT_POINT;
						pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1];
						pSaveLogThreadIDParam->strType1 = sDetectInfo;
						pSaveLogThreadIDParam->strType2 = _T("");
						pSaveLogThreadIDParam->bType = FALSE;
						pSaveLogThreadIDParam->iPcVisionNo = iPcVisionNo;
						pSaveLogThreadIDParam->iMzNo = iMzNo;
						pSaveLogThreadIDParam->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam->iModuleNo = iModuleNo;
						THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
					}

					if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_CONTACT_POINT;
						pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1];
						pSaveLogThreadIDParam_Dual->strType1 = sDetectInfo;
						pSaveLogThreadIDParam_Dual->strType2 = _T("");
						pSaveLogThreadIDParam_Dual->bType = FALSE;
						pSaveLogThreadIDParam_Dual->iPcVisionNo = iPcVisionNo;
						pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
						pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
						THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
					}

				}
			}

			bLogSaved = TRUE;

			//////////////////////////////////////////////////////////////////////////

			if (measureStruct.m_bInspectFail[iPcVisionNo] == TRUE && sTrayResult == "N")
			{
				int iSelectedFaiIndex;
				sDefectCode = THEAPP.GetFaiDefectCode(iMzNo, iTrayNo, iModuleNo, &iSelectedFaiIndex);

				if (measureStruct.m_bInspectFail[iPcVisionNo] == TRUE)		// FAI NG
				{
					if (iSelectedFaiIndex > 0)
						iFaiDefectTransCode = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iTransferCodeIndexFai[iSelectedFaiIndex];
					else
					{
						iFaiDefectTransCode = 0;
						int iDefectTransCode = 0;
						for (int i = 0; i < MAX_FAI_ITEM; i++)
						{
							BOOL bFAIResultNG = FALSE;

							if (measureStruct.m_iFAIResultCase[i] == FAI_RESULT_CASE_NG)
								bFAIResultNG = TRUE;

							if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMInspectFai[i] && bFAIResultNG)
							{
								iDefectTransCode = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iTransferCodeIndexFai[i];

								if (iDefectTransCode > iFaiDefectTransCode)
									iFaiDefectTransCode = iDefectTransCode;
							}
						}
					}
				}

				if (iCosmeticDefectTransCode > iFaiDefectTransCode)
					sTrayResult = THEAPP.Struct_PreferenceStruct.m_sAssignSpecialCode[iCosmeticDefectTransCode];
				else
					sTrayResult = THEAPP.Struct_PreferenceStruct.m_sAssignSpecialCode[iFaiDefectTransCode];
			}
			else if (measureStruct.m_bInspectFail[iPcVisionNo] == TRUE && sTrayResult != "N")
			{
				int iSelectedFaiIndex;
				sDefectCode = THEAPP.GetFaiDefectCode(iMzNo, iTrayNo, iModuleNo, &iSelectedFaiIndex);

				if (measureStruct.m_bInspectFail[iPcVisionNo] == TRUE)		// FAI NG
				{
					if (iSelectedFaiIndex > 0)
						iFaiDefectTransCode = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iTransferCodeIndexFai[iSelectedFaiIndex];
					else
					{
						iFaiDefectTransCode = 0;
						int iDefectTransCode = 0;
						for (int i = 0; i < MAX_FAI_ITEM; i++)
						{
							BOOL bFAIResultNG = FALSE;

							if (measureStruct.m_iFAIResultCase[i] == FAI_RESULT_CASE_NG)
								bFAIResultNG = TRUE;

							if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMInspectFai[i] && bFAIResultNG)
							{
								iDefectTransCode = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iTransferCodeIndexFai[i];

								if (iDefectTransCode > iFaiDefectTransCode)
									iFaiDefectTransCode = iDefectTransCode;
							}
						}
					}
				}

				sTrayResult = THEAPP.Struct_PreferenceStruct.m_sAssignSpecialCode[iFaiDefectTransCode];
			}
			else if (measureStruct.m_bInspectFail[iPcVisionNo] == FALSE && sTrayResult == "N")
				sTrayResult = THEAPP.Struct_PreferenceStruct.m_sAssignSpecialCode[iCosmeticDefectTransCode];
			else
				sTrayResult = "G";

			// Universal InspectComplete Defect Code 변경
			CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
			if (strModelType == _T("CHS-W") || strModelType == _T("CHS-Z") ||
				strModelType == _T("CHS-K") || strModelType == _T("CHS-S") ||
				strModelType == _T("CHS-T") || strModelType == _T("CHS-V"))
			{
				if (sDefectCode.Find("BA") == 0)	sDefectCode = "BA";
				else if (sDefectCode.Find("BR") == 0)	sDefectCode = "BL";
				else if (sDefectCode.Find("LS") == 0)	sDefectCode = "BL";
				else if (sDefectCode.Find("VT") == 0)	sDefectCode = "VT";
				else if (sDefectCode.Find("SB") == 0)	sDefectCode = "SB";
				else if (sDefectCode.Find("FP") == 0)	sDefectCode = "FP";
				else if (sDefectCode.Find("VS") == 0)	sDefectCode = "VS";
				else if (sDefectCode.Find("SS") == 0)	sDefectCode = "SS";
				else if (sDefectCode.Find("BS") == 0)	sDefectCode = "BS";
				else if (sDefectCode.Find("CO") == 0)	sDefectCode = "CO";
				else if (sDefectCode.Find("CS") == 0)	sDefectCode = "CS";
			}

			CString sVisionCamType_Comm;
			sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionCamType][iStageNo];

			if (!THEAPP.m_ModelDefineInfo.m_bVisionPWM[iVisionCamType])
			{
				THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, sTrayResult, sDefectCode);
				if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
					THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, sTrayResult, sDefectCode);
			}
#ifdef INLINE_MODE

			if (THEAPP.Struct_PreferenceStruct.m_bSendGrabFailError && sDefectCode == "GF")
			{
				++THEAPP.g_iGrabFailCount[iMzNo - 1];

				if (THEAPP.g_iGrabFailCount[iMzNo - 1] >= THEAPP.Struct_PreferenceStruct.m_iSendGrabFailErrorCount)
				{
					THEAPP.m_pHandlerService->Set_ErrorRequest(HANDLER_ERROR_GRAB_FAIL);
					THEAPP.g_iGrabFailCount[iMzNo - 1] = 0;
				}
			}
#endif

#ifdef USE_SUAKIT
			for (int i = 0; i < VISION_NUMBER_MAX; i++)
			{
				THEAPP.iADJLogResult[i][iModuleNo - 1] = -1;
				THEAPP.strADJLog[i][iModuleNo - 1] = _T("");
				THEAPP.m_nADJModelIDApplyPriority[i][iModuleNo - 1] = DEEP_MODEL_NUM * MAX_ADJ_CONNECT_NUM; //0625 jwj add
				THEAPP.m_nADJCurDefectPriority[i][iModuleNo - 1] = DEEP_MODEL_NUM * MAX_ADJ_CONNECT_NUM;
			}
#endif

#ifndef INLINE_MODE
#ifdef ONE_THREAD_INSPECTION
			pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////

			// Continuous Defect Alarm (For the same inspection type)
			if (THEAPP.Struct_PreferenceStruct.m_bUseContDefectAlarm == TRUE)
			{
				if (bCheckContinuousDefect)
				{
					int iTestTrayNoIndex, iTestModuleNoIndex, iPrevIndex;
					int iCurTrayNoIndex, iCurModuleNoIndex, iCurIndex;
					BOOL bBackwardCheck, bForwardCheck;

					HObject HDilatedDentRgn, HDentIntersectRgn;

					HObject HContDefectPosRgn;
					HTuple HContDefectArea, HContDefectCenterX, HContDefectCenterY;
					double dContDefectCenterX, dContDefectCenterY;

					BOOL bFluxDentAlarm = FALSE;

					iCurTrayNoIndex = iTrayNo - 1;
					iCurModuleNoIndex = iModuleNo - 1;
					iCurIndex = iCurTrayNoIndex * iMaxModuleTray + iCurModuleNoIndex;

					if (iCurIndex >= (THEAPP.Struct_PreferenceStruct.m_iContDefectCount - 1))		// 최소 연속 불량 갯수만큼은 있어야 함.
					{
						bBackwardCheck = TRUE;
						for (i = 1; i < THEAPP.Struct_PreferenceStruct.m_iContDefectCount; i++)
						{
							iPrevIndex = iCurIndex - i;
							iTestTrayNoIndex = iPrevIndex / iMaxModuleTray;
							iTestModuleNoIndex = iPrevIndex % iMaxModuleTray;

							if (THEAPP.m_StructModuleContDefect[iMzNo - 1][iTestTrayNoIndex][iTestModuleNoIndex].m_bModuleDefect == FALSE)
							{
								bBackwardCheck = FALSE;
								break;
							}
						}

						bForwardCheck = TRUE;
						for (i = 1; i < THEAPP.Struct_PreferenceStruct.m_iContDefectCount; i++)
						{
							iPrevIndex = iCurIndex + i;
							iTestTrayNoIndex = iPrevIndex / iMaxModuleTray;
							iTestModuleNoIndex = iPrevIndex % iMaxModuleTray;

							if (THEAPP.m_StructModuleContDefect[iMzNo - 1][iTestTrayNoIndex][iTestModuleNoIndex].m_bModuleDefect == FALSE)
							{
								bForwardCheck = FALSE;
								break;
							}
						}

						if (bBackwardCheck)
						{
							for (int iT = 0; iT < MAX_INSPECTION_TYPE; iT++)
							{
								if (THEAPP.m_StructModuleContDefect[iMzNo - 1][iCurTrayNoIndex][iCurModuleNoIndex].m_bInspectionTypeDefect[iT] == FALSE)
									continue;

								if (CGFunction::ValidHRegion(THEAPP.m_StructModuleContDefect[iMzNo - 1][iCurTrayNoIndex][iCurModuleNoIndex].m_HDefectRgn[iT]) == TRUE)	// 현재 모듈에서 불량이 있으면
								{
									DilationCircle(THEAPP.m_StructModuleContDefect[iMzNo - 1][iCurTrayNoIndex][iCurModuleNoIndex].m_HDefectRgn[iT], &HDilatedDentRgn, (double)THEAPP.Struct_PreferenceStruct.m_iContDefectPosTolerance + 0.5);

									BOOL bContDefect = TRUE;

									for (i = 1; i < THEAPP.Struct_PreferenceStruct.m_iContDefectCount; i++)
									{
										iPrevIndex = iCurIndex - i;
										iTestTrayNoIndex = iPrevIndex / iMaxModuleTray;
										iTestModuleNoIndex = iPrevIndex % iMaxModuleTray;

										if (THEAPP.m_StructModuleContDefect[iMzNo - 1][iTestTrayNoIndex][iTestModuleNoIndex].m_bInspectionTypeDefect[iT] == FALSE)
										{
											bContDefect = FALSE;
											break;
										}

										Intersection(HDilatedDentRgn, THEAPP.m_StructModuleContDefect[iMzNo - 1][iTestTrayNoIndex][iTestModuleNoIndex].m_HDefectRgn[iT], &HDentIntersectRgn);
										if (CGFunction::ValidHRegion(HDentIntersectRgn) == FALSE)
										{
											bContDefect = FALSE;
											break;
										}
									}

									if (bContDefect)
									{
										Union1(THEAPP.m_StructModuleContDefect[iMzNo - 1][iCurTrayNoIndex][iCurModuleNoIndex].m_HDefectRgn[iT], &HContDefectPosRgn);
										AreaCenter(HContDefectPosRgn, &HContDefectArea, &HContDefectCenterY, &HContDefectCenterX);
										dContDefectCenterX = HContDefectCenterX[0].D();
										dContDefectCenterY = HContDefectCenterY[0].D();

										bFluxDentAlarm = TRUE;
									}
								}
							}
						}	// if (bBackwardCheck)

						if (bForwardCheck)
						{
							for (int iT = 0; iT < MAX_INSPECTION_TYPE; iT++)
							{
								if (THEAPP.m_StructModuleContDefect[iMzNo - 1][iCurTrayNoIndex][iCurModuleNoIndex].m_bInspectionTypeDefect[iT] == FALSE)
									continue;

								if (CGFunction::ValidHRegion(THEAPP.m_StructModuleContDefect[iMzNo - 1][iCurTrayNoIndex][iCurModuleNoIndex].m_HDefectRgn[iT]) == TRUE)	// 현재 모듈에서 불량이 있으면
								{
									DilationCircle(THEAPP.m_StructModuleContDefect[iMzNo - 1][iCurTrayNoIndex][iCurModuleNoIndex].m_HDefectRgn[iT], &HDilatedDentRgn, (double)THEAPP.Struct_PreferenceStruct.m_iContDefectPosTolerance + 0.5);

									BOOL bContDefect = TRUE;

									for (i = 1; i < THEAPP.Struct_PreferenceStruct.m_iContDefectCount; i++)
									{
										iPrevIndex = iCurIndex + i;
										iTestTrayNoIndex = iPrevIndex / iMaxModuleTray;
										iTestModuleNoIndex = iPrevIndex % iMaxModuleTray;

										if (THEAPP.m_StructModuleContDefect[iMzNo - 1][iTestTrayNoIndex][iTestModuleNoIndex].m_bInspectionTypeDefect[iT] == FALSE)
										{
											bContDefect = FALSE;
											break;
										}

										Intersection(HDilatedDentRgn, THEAPP.m_StructModuleContDefect[iMzNo - 1][iTestTrayNoIndex][iTestModuleNoIndex].m_HDefectRgn[iT], &HDentIntersectRgn);
										if (CGFunction::ValidHRegion(HDentIntersectRgn) == FALSE)
										{
											bContDefect = FALSE;
											break;
										}
									}

									if (bContDefect)
									{
										Union1(THEAPP.m_StructModuleContDefect[iMzNo - 1][iCurTrayNoIndex][iCurModuleNoIndex].m_HDefectRgn[iT], &HContDefectPosRgn);
										AreaCenter(HContDefectPosRgn, &HContDefectArea, &HContDefectCenterY, &HContDefectCenterX);
										dContDefectCenterX = HContDefectCenterX[0].D();
										dContDefectCenterY = HContDefectCenterY[0].D();

										bFluxDentAlarm = TRUE;
									}
								}
							}
						}	// if (bBackwardCheck)
					}	// if (iCurIndex >= (THEAPP.Struct_PreferenceStruct.m_iContDefectCount - 1))

					if (bFluxDentAlarm == TRUE)		// 연속 Flux Dent 발생
					{
						CString sCamName, sDay, sTime;
						SYSTEMTIME time;

						GetLocalTime(&time);
						sDay.Format("%04d%02d%02d", time.wYear, time.wMonth, time.wDay);
						sTime.Format("%s %02d%02d%02d", sDay, time.wHour, time.wMinute, time.wSecond);
						sCamName.Format("%s", sVisionCamType_Short);

						CString sContDefectLog;
						sContDefectLog.Format("%s\t%s\t%s\t%s\t%s\t%d\t%d\t%s\t%d\t%d\t%d",
							sDay, sTime, THEAPP.GetPCID(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, sLotID, iTrayNo, iModuleNo, sCamName, 
							(int)dContDefectCenterX, (int)dContDefectCenterY, THEAPP.Struct_PreferenceStruct.m_iContDefectCount);

						if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
						{
							if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
								THEAPP.SaveMonitorContDefectLog(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], sContDefectLog);

							if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
								THEAPP.SaveMonitorContDefectLog(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], sContDefectLog);
						}
						else
						{
							if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
							{
								RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
								pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_MONITORING_CONT_DEFECT;
								pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
								pSaveLogThreadIDParam->strType1 = sContDefectLog;
								pSaveLogThreadIDParam->iTrayNo = iTrayNo;
								pSaveLogThreadIDParam->iModuleNo = iModuleNo;
								THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
							}

							if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
							{
								RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
								pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_MONITORING_CONT_DEFECT;
								pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
								pSaveLogThreadIDParam_Dual->strType1 = sContDefectLog;
								pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
								pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
								THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
							}
						}

						THEAPP.m_pHandlerService->Set_ErrorRequest(HANDLER_ERROR_CONT_DEFECT);
					}

				} // if (bCheckContinuousDefect)
			} // if (THEAPP.Struct_PreferenceStruct.m_bUseContDefectAlarm == TRUE)
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////

			if (THEAPP.Struct_PreferenceStruct.m_bUseResultImageThread || THEAPP.Struct_PreferenceStruct.m_bUseResultImageThreadOnlyGood)
				THEAPP.m_pInspectService->SaveResultImage(iPcVisionNo, iMzNo, iTrayNo, iModuleNo);

			if (THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread)
				THEAPP.m_pInspectService->SaveReviewImage(iPcVisionNo, iMzNo, iTrayNo, iModuleNo);

			if (THEAPP.Struct_PreferenceStruct.m_bUseReviewImageThread)
				THEAPP.m_pInspectService->SaveFAIImage(iPcVisionNo, iMzNo, iTrayNo, iModuleNo);

			if (THEAPP.Struct_PreferenceStruct.m_bUseADJImageThread)
				THEAPP.m_pInspectService->SaveADJImage(iPcVisionNo, iMzNo, iTrayNo, iModuleNo);

			if (THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
				THEAPP.m_pInspectService->SaveResultLog(iMzNo, sLotID);

			auto total_log_time_end = std::chrono::high_resolution_clock::now();
			auto total_log_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(total_log_time_end - total_log_time_start).count();

			strLog.Format("%s/ Total result save, Time(ms): %d, LotID: %s, Port: %d, Tray: %d, Module: %d, Inspect result: %s", sVisionCamType_Short, total_log_time_ms, sLotID, iMzNo, iTrayNo, iModuleNo, sTrayResult);
			THEAPP.m_log_inspection->info("{}", LOG_CSTR(strLog));

			Sleep(10);

		}	// while (TRUE)

		return 1;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AlgorithmThreadDefine::ResultSaveThread_N%d] : <%s>%s", iVisionCamType + 1, sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		//////////////////////////////////////////////////////////////////////////
		/// Error Processing

		pAlgorithm->ChangeInspectDone(iInspectionBufferIdx, BUFFER_STATUS_AVAILABLE);

		CString sLotID, sTrayID;
		int iMzNo, iStageNo, iJigNo;
		int iTrayNo, iModuleNo, iNoInspectImage;

		sLotID = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_sLotID[iInspectionBufferIdx];
		iMzNo = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_nMzNo[iInspectionBufferIdx];
		iStageNo = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_nStageNo[iInspectionBufferIdx];
		iJigNo = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_nJigNo[iInspectionBufferIdx];
		sTrayID = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_sTrayID[iInspectionBufferIdx];
		iTrayNo = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_nTrayNo[iInspectionBufferIdx];
		iModuleNo = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_nModuleNo[iInspectionBufferIdx];
		iNoInspectImage = pInspectService->m_pInspectAlgorithm[iVisionCamType].m_iNoInspectImage;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];

		CString sTrayResult;
		sTrayResult = "N";

		int iInspectionTypeResult;
		iInspectionTypeResult = DEFECT_TYPE_ALIGN_ERROR;

		//////////////////////////////////////////////////////////////////////////
	   // TrayAdminViewDlg 업데이트 (화면에 표시할 문자 설정, 검사 진행 다이어그램 업데이트)
	   //////////////////////////////////////////////////////////////////////////
		auto pTrayDlg = GetTrayViewDlg(iPcVisionNo);
		pTrayDlg->GetTrayImagePB().OkNg[iModuleNo - 1] = iInspectionTypeResult;
		pTrayDlg->GetTrayImagePB().InvalidateRect(false);

		// Defect Code (to 제어 프로그램)
		CString sDefectCode, sFaiDefectCode;
		sDefectCode.Format("MC");

		CString strLotSave, sSaveConfig, strADJLotResultLog;
		SYSTEMTIME ModuleInspectTime;
		CString sModuleInspectDate, sModuleInspectTime;

		if (bLogSaved == FALSE && ShouldSaveLog(iPcVisionNo))
		{
			GetLocalTime(&ModuleInspectTime);
			sModuleInspectDate.Format("%04d-%02d-%02d", ModuleInspectTime.wYear, ModuleInspectTime.wMonth, ModuleInspectTime.wDay);
			sModuleInspectTime.Format("%02d:%02d:%02d", ModuleInspectTime.wHour, ModuleInspectTime.wMinute, ModuleInspectTime.wSecond);

			sSaveConfig = THEAPP.m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;

			sModuleFAIDefectName = _T("MC");

			CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
			std::vector<double> vecFaiValue = GetFAIDefaultValueVector(strModelType);

			// 결과 구성
			int visionCount = GetResultVisionCount(iPcVisionNo);
			int isPairOption = GetResultPairOption(iPcVisionNo);

			std::vector<CString> vecVisionResultList(visionCount); // Vision Result List 구성 (Vision 수에 따라 동적으로 변경)
			std::vector<CString> vecAIResultList(visionCount * 3); // AI Result List 구성 (Vision 수 * AI Step 종류 수)
			std::vector<CuScanApp::DefectBlobInfo> vecDefectBlobInfoList(visionCount);
			if (!GetAllResult(visionCount, vecVisionResultList, vecAIResultList, vecDefectBlobInfoList, true, iMzNo, iTrayNo, iModuleNo, isPairOption)) {

			}
			// 가독성을 위해 구조체로 변경
			CuScanApp::LogHeaderInfoStruct logHeaders =
			{
				sModuleInspectDate,
				sModuleInspectTime,
				THEAPP.GetPCID(),
				THEAPP.Struct_PreferenceStruct.m_strEquipNo,
				sLotID,
				sSaveConfig,
				iTrayNo,
				iModuleNo,
				iStageNo,
				iJigNo,
				THEAPP.m_strModuleBarcodeID[iMzNo - 1][iTrayNo - 1][iModuleNo - 1],
				vecVisionResultList,
				vecAIResultList,
				vecDefectBlobInfoList,
				sModuleFAIDefectName,
				vecFaiValue
			};

			strLotSave = THEAPP.FormatLotResultString(logHeaders);

			if (!THEAPP.Struct_PreferenceStruct.m_bUseResultLogThread)
			{
				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
					THEAPP.SaveLotResultLog(THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1], strLotSave, iMzNo);

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
					THEAPP.SaveLotResultLog(THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1], strLotSave, iMzNo);

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
					THEAPP.SaveDayLotResultLog(THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1], strLotSave, iMzNo);

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
					THEAPP.SaveDayLotResultLog(THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1], strLotSave, iMzNo);
			}
			else
			{
				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_LOT_RESULT;
					pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strResultLogFolder_Local[iPcVisionNo][iMzNo - 1];
					pSaveLogThreadIDParam->strType1 = strLotSave;
					pSaveLogThreadIDParam->strType2 = _T("");
					pSaveLogThreadIDParam->bType = FALSE;
					pSaveLogThreadIDParam->iPcVisionNo = iPcVisionNo;
					pSaveLogThreadIDParam->iMzNo = iMzNo;
					pSaveLogThreadIDParam->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
				}

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_LOT_RESULT_LASPC;
					pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strResultLogFolder_LAS[iPcVisionNo][iMzNo - 1];
					pSaveLogThreadIDParam_Dual->strType1 = strLotSave;
					pSaveLogThreadIDParam_Dual->strType2 = _T("");
					pSaveLogThreadIDParam_Dual->bType = FALSE;
					pSaveLogThreadIDParam_Dual->iPcVisionNo = iPcVisionNo;
					pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
					pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
				}

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLocal)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam->iSaveLogType = SAVE_LOG_TYPE_DAY_LOT_RESULT;
					pSaveLogThreadIDParam->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_Local[iMzNo - 1];
					pSaveLogThreadIDParam->strType1 = strLotSave;
					pSaveLogThreadIDParam->strType2 = _T("");
					pSaveLogThreadIDParam->iPcVisionNo = iPcVisionNo;
					pSaveLogThreadIDParam->iMzNo = iMzNo;
					pSaveLogThreadIDParam->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam, iMzNo);
				}

				if (THEAPP.Struct_PreferenceStruct.m_bResultLogSaveLAS)
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Dual = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam_Dual->iSaveLogType = SAVE_LOG_TYPE_DAY_LOT_RESULT;
					pSaveLogThreadIDParam_Dual->strPath = THEAPP.m_FileBase.m_strDayResultLogFolder_LAS[iMzNo - 1];
					pSaveLogThreadIDParam_Dual->strType1 = strLotSave;
					pSaveLogThreadIDParam_Dual->strType2 = _T("");
					pSaveLogThreadIDParam_Dual->iPcVisionNo = iPcVisionNo;
					pSaveLogThreadIDParam_Dual->iMzNo = iMzNo;
					pSaveLogThreadIDParam_Dual->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam_Dual->iModuleNo = iModuleNo;
					THEAPP.m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Dual, iMzNo);
				}
			}
		}

		int iFaiDefectTransCode = 0;
		CenterlineMeasureStruct& measureStruct = CFAIDataManager::GetInstance().GetMeasure(iMzNo, iTrayNo, iModuleNo, FAI_PARAMETER_NUMBER_1);

		if (measureStruct.m_bInspectFail[iPcVisionNo] == TRUE && sTrayResult == "N")
		{
			int iSelectedFaiIndex;
			sDefectCode = THEAPP.GetFaiDefectCode(iMzNo, iTrayNo, iModuleNo, &iSelectedFaiIndex);

			if (measureStruct.m_bInspectFail[iPcVisionNo] == TRUE)		// FAI NG
			{
				if (iSelectedFaiIndex > 0)
					iFaiDefectTransCode = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iTransferCodeIndexFai[iSelectedFaiIndex];
				else
				{
					iFaiDefectTransCode = 0;
					int iDefectTransCode = 0;
					for (int i = 0; i < MAX_FAI_ITEM; i++)
					{
						BOOL bFAIResultNG = FALSE;

						if (measureStruct.m_iFAIResultCase[i] == FAI_RESULT_CASE_NG)
							bFAIResultNG = TRUE;

						if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMInspectFai[i] && bFAIResultNG)
						{
							iDefectTransCode = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iTransferCodeIndexFai[i];

							if (iDefectTransCode > iFaiDefectTransCode)
								iFaiDefectTransCode = iDefectTransCode;
						}
					}
				}
			}

			sTrayResult = THEAPP.Struct_PreferenceStruct.m_sAssignSpecialCode[iFaiDefectTransCode];
		}
		else if (measureStruct.m_bInspectFail[iPcVisionNo] == TRUE && sTrayResult != "N")
		{
			int iSelectedFaiIndex;
			sDefectCode = THEAPP.GetFaiDefectCode(iMzNo, iTrayNo, iModuleNo, &iSelectedFaiIndex);

			if (measureStruct.m_bInspectFail[iPcVisionNo] == TRUE)		// FAI NG
			{
				if (iSelectedFaiIndex > 0)
					iFaiDefectTransCode = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iTransferCodeIndexFai[iSelectedFaiIndex];
				else
				{
					iFaiDefectTransCode = 0;
					int iDefectTransCode = 0;
					for (int i = 0; i < MAX_FAI_ITEM; i++)
					{
						BOOL bFAIResultNG = FALSE;

						if (measureStruct.m_iFAIResultCase[i] == FAI_RESULT_CASE_NG)
							bFAIResultNG = TRUE;

						if (THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_bMInspectFai[i] && bFAIResultNG)
						{
							iDefectTransCode = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_iTransferCodeIndexFai[i];
							if (iDefectTransCode > iFaiDefectTransCode)
								iFaiDefectTransCode = iDefectTransCode;
						}
					}
				}
			}

			sTrayResult = THEAPP.Struct_PreferenceStruct.m_sAssignSpecialCode[iFaiDefectTransCode];
		}
		else if (measureStruct.m_bInspectFail[iPcVisionNo] == FALSE && sTrayResult == "N")
		{
			;
		}
		else
			sTrayResult = "G";

		CString sVisionCamType_Comm;
		sVisionCamType_Comm = THEAPP.m_ModelDefineInfo.m_strVisionName_Comm[iVisionCamType][iStageNo];

		if (!THEAPP.m_ModelDefineInfo.m_bVisionPWM[iVisionCamType])
		{
			THEAPP.m_pHandlerService->Set_InspectComplete(sLotID, iMzNo, iTrayNo, iModuleNo, sVisionCamType_Comm, sTrayResult, sDefectCode);
			if (THEAPP.Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber > 0)
				THEAPP.m_pHandlerService->Save_InspectComplete(sLotID, iMzNo, iJigNo, iTrayNo, iModuleNo, sVisionCamType_Comm, sTrayResult, sDefectCode);
		}

		//////////////////////////////////////////////////////////////////////////
		strLog.Format("ResultSaveThread_V%d End (Halcon Exception)", iVisionCamType + 1);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

#ifndef INLINE_MODE
#ifdef ONE_THREAD_INSPECTION
		pInspectService->m_bOfflineModuleInspectDone[iPcVisionNo] = TRUE;
#endif
#endif

		pAlgorithm->ChangeResultSaveThreadRunning(FALSE);

		return 0;
	}
}
