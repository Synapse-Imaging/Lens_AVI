#include "stdafx.h"
#include "AutoCalService.h"


#include "uScan.h"

UINT AutoCalLightThread_Camera(LPVOID lp);
UINT AutoCalFocusThread(LPVOID lp);


CAutoCalService *CAutoCalService::m_pInstance = NULL;

CAutoCalService *CAutoCalService::GetInstance()
{
	if (!m_pInstance) {
		m_pInstance = new CAutoCalService();
	}
	return m_pInstance;
}

void CAutoCalService::DeleteInstance()
{
	if (m_pInstance) delete m_pInstance;
	m_pInstance = NULL;
}

CAutoCalService::CAutoCalService(void)
{
	InitAutoCalResult();
}


CAutoCalService::~CAutoCalService(void)
{

}

void CAutoCalService::InitAutoCalResult()
{
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_bAutoCal_Done[i] = false;

		for (int j = 0; j < LIGHT_CH_CNT; j++)
		{
			m_iTeachCH_LV[i][j] = 0;
			m_iTeachCH_GV[i][j] = 0;
			m_iInspectCH_LV[i][j] = 0;
			m_iInspectCH_GV[i][j] = 0;
			m_bJudgeCH[i][j] = true;
		}

		m_iTeachTOTAL_GV[i] = 0;
		m_iInspectTOTAL_GV[i] = 0;
		m_bJudgeTOTAL[i] = true;

		m_dTeachZPos[i] = 0.0;
		m_dTeachEdgeValue[i] = 0.0;
		m_dInspectZPos[i] = 0.0;
		m_dInspectEdgeValue[i] = 0.0;
		m_bJudge[i] = true;
	}

	m_iLightAutoCalProgressPercent = 0;
	m_iFocusAutoCalProgressPercent = 0;
}

#include "LightAutoCalResultDlg.h"
#include "FocusAutoCalResultDlg.h"

UINT AutoCalLightThread_Camera(LPVOID lp)
{
	CString strLog;

	CAutoCalService* pAutoCalService = (CAutoCalService*)lp;
	CCameraManager* pCameraManager = THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType];

	try
	{
		strLog.Format("Auto Calibration start");
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		double dTimeStart = 0, dTimeEnd = 0;
		dTimeStart = GetTickCount();

		DWORD dwGrabStart = 0, dwGrabEnd = 0;
		BOOL bGrabFail = FALSE;

		////////////////////////////// 스캔 시작 ////////////////////////////////////
		double dPrevPosZ = -100000000;
		double dInspPosZ;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		int i, j, k;

		int iPcVisionNo;
		iPcVisionNo = pCameraManager->GetVisionCamName();

		//////////////////////////////////////////////////////////////////////////
		/// Calculate Matching Info 

		// Grab Images Using Current Light Condition

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab)
				break;

			CString sGrabTime, sGrabTime_total;
			double tGrabStart = 0, tGrabEnd = 0;
			double tGrabStart_total = 0, tGrabEnd_total = 0;
			CString strLog;

			dInspPosZ = THEAPP.m_pModelDataManager->m_dZFocusPosition[0][iNoCurImageGrab];

			if (fabs(dInspPosZ - dPrevPosZ) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iPcVisionNo, STAGE_NUMBER_1, 0, dInspPosZ, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo]) // 무브 컴플리트 기다림
				{
					Sleep(1);
				}

				dPrevPosZ = dInspPosZ;
			}

			tGrabStart = GetTickCount();
			tGrabStart_total = tGrabStart;

			BOOL bGrabSuccess = FALSE;

			CString sDelay;
			double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

			iRetryCnt = 0;

			for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo, THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]);

				Sleep(20);

				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
					{
						break;
					}

					dwGrabEnd = GetTickCount();

					if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
					{
						THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

						strLog.Format("Auto Calibration grabdone timeout");
						THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
						break;
					}

					Sleep(1);
				}

				if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
				{
					bGrabSuccess = TRUE;
					break;
				}

			}	// Grab Retry

			if (!bGrabSuccess)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

			tGrabEnd = GetTickCount();
			strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
			THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

			++iNoCurImageGrab;
		}

		if (bGrabFail == TRUE)
		{
			AfxMessageBox("매칭 영상 Grab 시퀀스 오류 발생!!.", MB_ICONERROR | MB_SYSTEMMODAL);
			return 0;
		}

		HObject HMatchingGrabColorImage[MAX_IMAGE_TAB];

		for (i = 0; i < MAX_IMAGE_TAB; i++)
			GenEmptyObj(HMatchingGrabColorImage + i);

		HObject HColorImage;

		for (i = 0; i < THEAPP.m_pModelDataManager->m_iNoUsedImageGrab; i++)
		{
			Compose3(THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][0],
				THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][1],
				THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][2], &HColorImage);
			CopyImage(HColorImage, HMatchingGrabColorImage + i);
		}

		// Get Matching Info for Global Align

		HTuple HMatchingHomMat[MAX_IMAGE_TAB];
		for (i = 0; i < MAX_IMAGE_TAB; i++)
			TupleGenConst(0, 0, HMatchingHomMat + i);

		pAutoCalService->GetMatchingAlignInfo(HMatchingGrabColorImage, HMatchingHomMat);

		//////////////////////////////////////////////////////////////////////////

		int iProgressBarInterval = 100;
		if (THEAPP.m_pModelDataManager->m_iNoUsedImageGrab > 0)
			iProgressBarInterval = 100 / THEAPP.m_pModelDataManager->m_iNoUsedImageGrab;

		BOOL bLightCalROIExist = FALSE;
		int iCurLightAvgGV;
		int iUsedPageIndex = 0;

		iNoCurImageGrab = 0;

		BOOL bLightControllerPortOpen = FALSE;
		int iComPortNumber = THEAPP.m_pModelDataManager->m_PageControlData.m_iComPortNumber;
		if (THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.OpenPort(iComPortNumber, 19200))
			bLightControllerPortOpen = TRUE;

		if (bLightControllerPortOpen == FALSE)
		{
			AfxMessageBox("조명 Controller Port Open 오류!!.", MB_ICONERROR | MB_SYSTEMMODAL);
			return 0;
		}

		HObject HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS;
		HObject HGrayImage;

		int iNoLightCalData;
		int iLightCal_LV_Data[10000];
		int iLightCal_GV_Data[10000];

		int iLightCHRefGV;
		int iGvDiff, iMinGvDiff, iBestGv;
		int iMinGVDiffStartLV, iMinGVDiffEndLV;
		BOOL bStartLvWithBestGv;
		int iStartLv, iEndLv, iFoundMidLV;

		while (TRUE)
		{
			THEAPP.m_pTabControlDlg->m_pExtraDlg->m_ctrlProgressAutoCal.SetPos(pAutoCalService->m_iLightAutoCalProgressPercent);

			pAutoCalService->m_iLightAutoCalProgressPercent += iProgressBarInterval;

			if (iNoCurImageGrab >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab)
				break;

			CString sGrabTime, sGrabTime_total;
			double tGrabStart = 0, tGrabEnd = 0;
			double tGrabStart_total = 0, tGrabEnd_total = 0;
			CString strLog;

			dInspPosZ = THEAPP.m_pModelDataManager->m_dZFocusPosition[0][iNoCurImageGrab];

			if (fabs(dInspPosZ - dPrevPosZ) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iPcVisionNo, STAGE_NUMBER_1, 0, dInspPosZ, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo]) // 무브 컴플리트 기다림
				{
					Sleep(1);
				}

				dPrevPosZ = dInspPosZ;
			}

			GTRegion *pInspectROIRgn;
			int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

			HObject HCrossROIHRegion, HCrossImageReduced, HEdgeAmp;
			double dAvgValue;

			bLightCalROIExist = FALSE;

			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iNoCurImageGrab + 1)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LIGHT)
					continue;

				HCrossROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

				bLightCalROIExist = TRUE;
				break;
			}

			if (bLightCalROIExist == FALSE)	// 조명 Cal ROI가 없으면 작업 Skip
			{
				++iNoCurImageGrab;
				continue;
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HCrossROIHRegion) == FALSE)
			{
				++iNoCurImageGrab;
				continue;
			}

			//////////////////////////// Start ////////////////////////////////////

			pAutoCalService->m_bAutoCal_Done[iNoCurImageGrab] = true;	// Cal 진행

			// 현재 설정되어 있는 채널별 조명값
			int iCurLVChannel[LIGHT_CH_CNT];

			iCurLVChannel[0] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[0];
			iCurLVChannel[1] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[1];
			iCurLVChannel[2] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[2];
			iCurLVChannel[3] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[3];
			iCurLVChannel[4] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[4];
			iCurLVChannel[5] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[5];
			iCurLVChannel[6] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[6];
			iCurLVChannel[7] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[7];
			iCurLVChannel[8] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[8];
			iCurLVChannel[9] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[9];

			int iLVChannelStart[LIGHT_CH_CNT];
			int iLVChannelEnd[LIGHT_CH_CNT];

			for (i = 0; i < LIGHT_CH_CNT; i++)
			{
				iLVChannelStart[i] = iCurLVChannel[i] - THEAPP.m_pModelDataManager->m_iLightValueStart;
				if (iLVChannelStart[i] < 0)
					iLVChannelStart[i] = 0;
				if (iLVChannelStart[i] > LIGHT_BRIGHT_MAX)
					iLVChannelStart[i] = LIGHT_BRIGHT_MAX;

				iLVChannelEnd[i] = iCurLVChannel[i] + THEAPP.m_pModelDataManager->m_iLightValueEnd;
				if (iLVChannelEnd[i] < 0)
					iLVChannelEnd[i] = 0;
				if (iLVChannelEnd[i] > LIGHT_BRIGHT_MAX)
					iLVChannelEnd[i] = LIGHT_BRIGHT_MAX;
			}

			int iLVChannelInterval = THEAPP.m_pModelDataManager->m_iLightValueInterval;

			for (i = 0; i < LIGHT_CH_CNT; i++)
			{
				if (iCurLVChannel[i] > 0)		// 조명값이 설정되어 있는 경우에만
				{
					iNoLightCalData = 0;

					for (int iLV = iLVChannelStart[i]; iLV <= iLVChannelEnd[i]; iLV += iLVChannelInterval)
					{
						if (bLightControllerPortOpen)
						{
							THEAPP.m_pModelDataManager->m_PageControlData.SetChannelllumination_10CH(iUsedPageIndex, i, iLV);
						}

						tGrabStart = GetTickCount();
						tGrabStart_total = tGrabStart;

						BOOL bGrabSuccess = FALSE;

						CString sDelay;
						double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

						iRetryCnt = 0;

						for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
						{
							++iRetryCnt;

							// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo, iUsedPageIndex);

							Sleep(20);

							dwGrabStart = GetTickCount();
							while (1)
							{
								if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
								{
									break;
								}

								dwGrabEnd = GetTickCount();

								if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
								{
									THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

									strLog.Format("Auto Calibration grabdone timeout");
									THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
									break;
								}

								Sleep(1);
							}

							if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
							{
								bGrabSuccess = TRUE;
								break;
							}

						}	// Grab Retry

						if (!bGrabSuccess)		// Skip
						{
							pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
							pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];
							pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
							pAutoCalService->m_iInspectCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];

							pAutoCalService->m_bJudgeCH[iNoCurImageGrab][i] = true;			// G

							continue;
						}

						tGrabEnd = GetTickCount();
						strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
						THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

						///////////////////////////////////////////////////////////////////////////

						iCurLightAvgGV = 255;
						dAvgValue = 255;

						CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][0], &HTempImageR);
						CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][1], &HTempImageG);
						CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][2], &HTempImageB);
						Compose3(HTempImageR, HTempImageG, HTempImageB, &HColorImage);

						pAutoCalService->ApplyMatchingAlignInfo(iNoCurImageGrab, &HColorImage, HMatchingHomMat);

						Rgb1ToGray(HColorImage, &HGrayImage);

						HTuple HdAvgValue, HdSd;
						Intensity(HCrossROIHRegion, HGrayImage, &HdAvgValue, &HdSd);

						dAvgValue = HdAvgValue.D();

						iCurLightAvgGV = (int)(dAvgValue + 0.5);
						if (iCurLightAvgGV < 0)
							iCurLightAvgGV = 0;
						if (iCurLightAvgGV > 255)
							iCurLightAvgGV = 255;

						iLightCal_LV_Data[iNoLightCalData] = iLV;
						iLightCal_GV_Data[iNoLightCalData] = iCurLightAvgGV;

						++iNoLightCalData;
					}

					if (iNoLightCalData == 0)		// Cal Fail
					{
						pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
						pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];
						pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
						pAutoCalService->m_iInspectCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];

						pAutoCalService->m_bJudgeCH[iNoCurImageGrab][i] = true;			// G
					}
					else
					{
						iLightCHRefGV = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];

						iMinGvDiff = 100000;
						for (j = 0; j < iNoLightCalData; j++)
						{
							iGvDiff = abs(iLightCal_GV_Data[j] - iLightCHRefGV);

							if (iGvDiff < iMinGvDiff)
							{
								iMinGvDiff = iGvDiff;
								iBestGv = iLightCal_GV_Data[j];
							}
						}

						bStartLvWithBestGv = FALSE;

						iEndLv = -1;

						for (j = 0; j < iNoLightCalData; j++)
						{
							if (iBestGv == iLightCal_GV_Data[j])
							{
								bStartLvWithBestGv = TRUE;
								iStartLv = iLightCal_LV_Data[j];
								continue;
							}

							if (bStartLvWithBestGv && (iBestGv != iLightCal_GV_Data[j]))
							{
								iEndLv = iLightCal_LV_Data[j - 1];
								break;
							}
						}

						if (iEndLv == -1)
							iEndLv = iStartLv;

						iFoundMidLV = (iStartLv + iEndLv) / 2;

						pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
						pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];
						pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][i] = iFoundMidLV;
						pAutoCalService->m_iInspectCH_GV[iNoCurImageGrab][i] = iBestGv;

						// 찾은 조명값이 이전 조명값보다 조명 밝기 허용치보다 크게 찾았을 경우 
						if (abs(pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][i] - pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i]) > THEAPP.m_pModelDataManager->m_iLightValueInTol)
						{
							pAutoCalService->m_bJudgeCH[iNoCurImageGrab][i] = false;		// NG
						}
						else
						{
							pAutoCalService->m_bJudgeCH[iNoCurImageGrab][i] = true;		// G
						}
					}
				}
				else		// Not Used Channel
				{
					pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = 0;
					pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = 0;
					pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][i] = 0;
					pAutoCalService->m_iInspectCH_GV[iNoCurImageGrab][i] = 0;

					pAutoCalService->m_bJudgeCH[iNoCurImageGrab][i] = true;			// G
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Page (Channel 전체 테스트)			

			if (bLightControllerPortOpen)
			{
				THEAPP.m_pModelDataManager->m_PageControlData.SetChannelClear_10CH();
				THEAPP.m_pModelDataManager->m_PageControlData.SetTotalChannelllumination_10CH(pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][0], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][1], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][2], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][3],
					pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][4], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][5], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][6], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][7], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][8], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][9]);
			}

			tGrabStart = GetTickCount();
			tGrabStart_total = tGrabStart;

			BOOL bGrabSuccess = FALSE;

			CString sDelay;
			double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

			iRetryCnt = 0;

			for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo, iUsedPageIndex);

				Sleep(20);

				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
					{
						break;
					}

					dwGrabEnd = GetTickCount();

					if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
					{
						THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

						strLog.Format("Auto Calibration grabdone timeout");
						THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
						break;
					}

					Sleep(1);
				}

				if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
				{
					bGrabSuccess = TRUE;
					break;
				}

			}	// Grab Retry

			if (!bGrabSuccess)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

			tGrabEnd = GetTickCount();
			strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
			THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

			///////////////////////////////////////////////////////////////////////////

			iCurLightAvgGV = 255;
			dAvgValue = 255;

			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][0], &HTempImageR);
			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][1], &HTempImageG);
			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][2], &HTempImageB);
			Compose3(HTempImageR, HTempImageG, HTempImageB, &HColorImage);

			pAutoCalService->ApplyMatchingAlignInfo(iNoCurImageGrab, &HColorImage, HMatchingHomMat);

			Rgb1ToGray(HColorImage, &HGrayImage);

			HTuple HdAvgValue, HdSd;
			Intensity(HCrossROIHRegion, HGrayImage, &HdAvgValue, &HdSd);

			dAvgValue = HdAvgValue.D();

			iCurLightAvgGV = (int)(dAvgValue + 0.5);
			if (iCurLightAvgGV < 0)
				iCurLightAvgGV = 0;
			if (iCurLightAvgGV > 255)
				iCurLightAvgGV = 255;

			pAutoCalService->m_iTeachTOTAL_GV[iNoCurImageGrab] = THEAPP.m_pModelDataManager->m_iLightAverageValueTotal[iNoCurImageGrab];
			pAutoCalService->m_iInspectTOTAL_GV[iNoCurImageGrab] = iCurLightAvgGV;

			if (abs(pAutoCalService->m_iInspectTOTAL_GV[iNoCurImageGrab] - pAutoCalService->m_iTeachTOTAL_GV[iNoCurImageGrab]) > THEAPP.m_pModelDataManager->m_iTotalImageValueTol)
			{
				pAutoCalService->m_bJudgeTOTAL[iNoCurImageGrab] = false;
			}
			else
			{
				pAutoCalService->m_bJudgeTOTAL[iNoCurImageGrab] = true;
			}

			++iNoCurImageGrab;
		}

		if (bGrabFail == TRUE)
		{
			strLog.Format("Auto calibration image grab fail(No signal)");
			THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
		}

		dTimeEnd = GetTickCount();
		strLog.Format("Auto calibration done, Time(ms): %.4lf", (dTimeEnd - dTimeStart) / 1000);
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		pAutoCalService->m_iLightAutoCalProgressPercent = 100;

		//////////////////////////////////////////////////////////////////////////
		/// Closing

		// 현재 설정되어 있는 채널별 조명값
		int iUsedPageLVChannel[LIGHT_CH_CNT];

		iUsedPageLVChannel[0] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[0];
		iUsedPageLVChannel[1] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[1];
		iUsedPageLVChannel[2] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[2];
		iUsedPageLVChannel[3] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[3];
		iUsedPageLVChannel[4] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[4];
		iUsedPageLVChannel[5] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[5];
		iUsedPageLVChannel[6] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[6];
		iUsedPageLVChannel[7] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[7];
		iUsedPageLVChannel[8] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[8];
		iUsedPageLVChannel[9] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[9];

		if (bLightControllerPortOpen)
		{
			THEAPP.m_pModelDataManager->m_PageControlData.SetChannelClear_10CH();
			THEAPP.m_pModelDataManager->m_PageControlData.SetTotalChannelllumination_10CH(iUsedPageLVChannel[0], iUsedPageLVChannel[1], iUsedPageLVChannel[2], iUsedPageLVChannel[3], iUsedPageLVChannel[4], iUsedPageLVChannel[5], iUsedPageLVChannel[6], iUsedPageLVChannel[7], iUsedPageLVChannel[8], iUsedPageLVChannel[9]);
		}

		if (bLightControllerPortOpen)
			THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.ClosePort();

		//////////////////////////////////////////////////////////////////////////

		CLightAutoCalResultDlg dlg;

		dlg.SetMasterGVMode(FALSE);
		dlg.DoModal();

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AutoCalService::AutoCalLightThread_Camera] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}
}

UINT AutoCalLightThread_Specular(LPVOID lp)
{
	CString strLog;

	CAutoCalService* pAutoCalService = (CAutoCalService*)lp;
	CCameraManager* pCameraManager = THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType];

	try
	{
		strLog.Format("Auto Calibration start");
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		double dTimeStart = 0, dTimeEnd = 0;
		dTimeStart = GetTickCount();

		DWORD dwGrabStart = 0, dwGrabEnd = 0;
		BOOL bGrabFail = FALSE;

		////////////////////////////// 스캔 시작 ////////////////////////////////////
		double dPrevPosZ = -100000000;
		double dInspPosZ;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		int i, j, k;

		int iPcVisionNo;
		iPcVisionNo = pCameraManager->GetVisionCamName();

		//////////////////////////////////////////////////////////////////////////
		/// Calculate Matching Info 

		// Grab Images Using Current Light Condition

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab)
				break;

			CString sGrabTime, sGrabTime_total;
			double tGrabStart = 0, tGrabEnd = 0;
			double tGrabStart_total = 0, tGrabEnd_total = 0;
			CString strLog;

			dInspPosZ = THEAPP.m_pModelDataManager->m_dZFocusPosition[0][iNoCurImageGrab];

			if (fabs(dInspPosZ - dPrevPosZ) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iPcVisionNo, STAGE_NUMBER_1, 0, dInspPosZ, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo]) // 무브 컴플리트 기다림
				{
					Sleep(1);
				}

				dPrevPosZ = dInspPosZ;
			}

			tGrabStart = GetTickCount();
			tGrabStart_total = tGrabStart;

			BOOL bGrabSuccess = FALSE;

			CString sDelay;
			double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

			iRetryCnt = 0;

			for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo, THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]);

				Sleep(20);

				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
					{
						break;
					}

					dwGrabEnd = GetTickCount();

					if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
					{
						THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

						strLog.Format("Auto Calibration grabdone timeout");
						THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
						break;
					}

					Sleep(1);
				}

				if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
				{
					bGrabSuccess = TRUE;
					break;
				}

			}	// Grab Retry

			if (!bGrabSuccess)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

			tGrabEnd = GetTickCount();
			strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
			THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

			++iNoCurImageGrab;
		}

		if (bGrabFail == TRUE)
		{
			AfxMessageBox("매칭 영상 Grab 시퀀스 오류 발생!!.", MB_ICONERROR | MB_SYSTEMMODAL);
			return 0;
		}

		HObject HMatchingGrabColorImage[MAX_IMAGE_TAB];

		for (i = 0; i < MAX_IMAGE_TAB; i++)
			GenEmptyObj(HMatchingGrabColorImage + i);

		HObject HColorImage;

		for (i = 0; i < THEAPP.m_pModelDataManager->m_iNoUsedImageGrab; i++)
		{
			Compose3(THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][0],
				THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][1],
				THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][2], &HColorImage);
			CopyImage(HColorImage, HMatchingGrabColorImage + i);
		}

		// Get Matching Info for Global Align

		HTuple HMatchingHomMat[MAX_IMAGE_TAB];
		for (i = 0; i < MAX_IMAGE_TAB; i++)
			TupleGenConst(0, 0, HMatchingHomMat + i);

		pAutoCalService->GetMatchingAlignInfo(HMatchingGrabColorImage, HMatchingHomMat);

		//////////////////////////////////////////////////////////////////////////

		int iProgressBarInterval = 100;
		if (THEAPP.m_pModelDataManager->m_iNoUsedImageGrab > 0)
			iProgressBarInterval = 100 / THEAPP.m_pModelDataManager->m_iNoUsedImageGrab;

		BOOL bLightCalROIExist = FALSE;
		int iCurLightAvgGV;
		int iUsedPageIndex = 0;

		iNoCurImageGrab = 0;

		BOOL bLightControllerPortOpen = FALSE;
		int iComPortNumber = THEAPP.m_pModelDataManager->m_PageControlData.m_iComPortNumber;
		if (THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.OpenPort(iComPortNumber, 19200))
			bLightControllerPortOpen = TRUE;

		if (bLightControllerPortOpen == FALSE)
		{
			AfxMessageBox("조명 Controller Port Open 오류!!.", MB_ICONERROR | MB_SYSTEMMODAL);
			return 0;
		}

		HObject HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS;
		HObject HGrayImage;

		int iNoLightCalData;
		int iLightCal_LV_Data[10000];
		int iLightCal_GV_Data[10000];

		int iLightCHRefGV;
		int iGvDiff, iMinGvDiff, iBestGv;
		int iMinGVDiffStartLV, iMinGVDiffEndLV;
		BOOL bStartLvWithBestGv;
		int iStartLv, iEndLv, iFoundMidLV;

		while (TRUE)
		{
			THEAPP.m_pTabControlDlg->m_pExtraDlg->m_ctrlProgressAutoCal.SetPos(pAutoCalService->m_iLightAutoCalProgressPercent);

			pAutoCalService->m_iLightAutoCalProgressPercent += iProgressBarInterval;

			if (iNoCurImageGrab >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab)
				break;

			CString sGrabTime, sGrabTime_total;
			double tGrabStart = 0, tGrabEnd = 0;
			double tGrabStart_total = 0, tGrabEnd_total = 0;
			CString strLog;

			dInspPosZ = THEAPP.m_pModelDataManager->m_dZFocusPosition[0][iNoCurImageGrab];

			if (fabs(dInspPosZ - dPrevPosZ) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iPcVisionNo, STAGE_NUMBER_1, 0, dInspPosZ, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo]) // 무브 컴플리트 기다림
				{
					Sleep(1);
				}

				dPrevPosZ = dInspPosZ;
			}

			GTRegion *pInspectROIRgn;
			int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

			HObject HCrossROIHRegion, HCrossImageReduced, HEdgeAmp;
			double dAvgValue;

			bLightCalROIExist = FALSE;

			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iNoCurImageGrab + 1)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LIGHT)
					continue;

				HCrossROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

				bLightCalROIExist = TRUE;
				break;
			}

			if (bLightCalROIExist == FALSE)	// 조명 Cal ROI가 없으면 작업 Skip
			{
				++iNoCurImageGrab;
				continue;
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HCrossROIHRegion) == FALSE)
			{
				++iNoCurImageGrab;
				continue;
			}

			//////////////////////////// Start ////////////////////////////////////

			pAutoCalService->m_bAutoCal_Done[iNoCurImageGrab] = true;	// Cal 진행

			// 현재 설정되어 있는 채널별 조명값
			int iCurLVChannel[LIGHT_CH_CNT];

			iCurLVChannel[0] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[0];
			iCurLVChannel[1] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[1];
			iCurLVChannel[2] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[2];
			iCurLVChannel[3] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[3];
			iCurLVChannel[4] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[4];
			iCurLVChannel[5] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[5];
			iCurLVChannel[6] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[6];
			iCurLVChannel[7] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[7];
			iCurLVChannel[8] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[8];
			iCurLVChannel[9] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[9];

			int iLVChannelStart[LIGHT_CH_CNT];
			int iLVChannelEnd[LIGHT_CH_CNT];

			for (i = 0; i < LIGHT_CH_CNT; i++)
			{
				iLVChannelStart[i] = iCurLVChannel[i] - THEAPP.m_pModelDataManager->m_iLightValueStart;
				if (iLVChannelStart[i] < 0)
					iLVChannelStart[i] = 0;
				if (iLVChannelStart[i] > LIGHT_BRIGHT_MAX)
					iLVChannelStart[i] = LIGHT_BRIGHT_MAX;

				iLVChannelEnd[i] = iCurLVChannel[i] + THEAPP.m_pModelDataManager->m_iLightValueEnd;
				if (iLVChannelEnd[i] < 0)
					iLVChannelEnd[i] = 0;
				if (iLVChannelEnd[i] > LIGHT_BRIGHT_MAX)
					iLVChannelEnd[i] = LIGHT_BRIGHT_MAX;
			}

			int iLVChannelInterval = THEAPP.m_pModelDataManager->m_iLightValueInterval;

			for (i = 0; i < LIGHT_CH_CNT; i++)
			{
				if (iCurLVChannel[i] > 0)		// 조명값이 설정되어 있는 경우에만
				{
					iNoLightCalData = 0;

					for (int iLV = iLVChannelStart[i]; iLV <= iLVChannelEnd[i]; iLV += iLVChannelInterval)
					{
						if (bLightControllerPortOpen)
						{
							THEAPP.m_pModelDataManager->m_PageControlData.SetChannelllumination_10CH(iUsedPageIndex, i, iLV);
						}

						tGrabStart = GetTickCount();
						tGrabStart_total = tGrabStart;

						BOOL bGrabSuccess = FALSE;

						CString sDelay;
						double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

						iRetryCnt = 0;

						for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
						{
							++iRetryCnt;

							// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo, iUsedPageIndex);

							Sleep(20);

							dwGrabStart = GetTickCount();
							while (1)
							{
								if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
								{
									break;
								}

								dwGrabEnd = GetTickCount();

								if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
								{
									THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

									strLog.Format("Auto Calibration grabdone timeout");
									THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
									break;
								}

								Sleep(1);
							}

							if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
							{
								bGrabSuccess = TRUE;
								break;
							}

						}	// Grab Retry

						if (!bGrabSuccess)		// Skip
						{
							pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
							pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];
							pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
							pAutoCalService->m_iInspectCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];

							pAutoCalService->m_bJudgeCH[iNoCurImageGrab][i] = true;			// G

							continue;
						}

						tGrabEnd = GetTickCount();
						strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
						THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

						///////////////////////////////////////////////////////////////////////////

						iCurLightAvgGV = 255;
						dAvgValue = 255;

						CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][0], &HTempImageR);
						CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][1], &HTempImageG);
						CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][2], &HTempImageB);
						Compose3(HTempImageR, HTempImageG, HTempImageB, &HColorImage);

						pAutoCalService->ApplyMatchingAlignInfo(iNoCurImageGrab, &HColorImage, HMatchingHomMat);

						Rgb1ToGray(HColorImage, &HGrayImage);

						HTuple HdAvgValue, HdSd;
						Intensity(HCrossROIHRegion, HGrayImage, &HdAvgValue, &HdSd);

						dAvgValue = HdAvgValue.D();

						iCurLightAvgGV = (int)(dAvgValue + 0.5);
						if (iCurLightAvgGV < 0)
							iCurLightAvgGV = 0;
						if (iCurLightAvgGV > 255)
							iCurLightAvgGV = 255;

						iLightCal_LV_Data[iNoLightCalData] = iLV;
						iLightCal_GV_Data[iNoLightCalData] = iCurLightAvgGV;

						++iNoLightCalData;
					}

					if (iNoLightCalData == 0)		// Cal Fail
					{
						pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
						pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];
						pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
						pAutoCalService->m_iInspectCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];

						pAutoCalService->m_bJudgeCH[iNoCurImageGrab][i] = true;			// G
					}
					else
					{
						iLightCHRefGV = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];

						iMinGvDiff = 100000;
						for (j = 0; j < iNoLightCalData; j++)
						{
							iGvDiff = abs(iLightCal_GV_Data[j] - iLightCHRefGV);

							if (iGvDiff < iMinGvDiff)
							{
								iMinGvDiff = iGvDiff;
								iBestGv = iLightCal_GV_Data[j];
							}
						}

						bStartLvWithBestGv = FALSE;

						iEndLv = -1;

						for (j = 0; j < iNoLightCalData; j++)
						{
							if (iBestGv == iLightCal_GV_Data[j])
							{
								bStartLvWithBestGv = TRUE;
								iStartLv = iLightCal_LV_Data[j];
								continue;
							}

							if (bStartLvWithBestGv && (iBestGv != iLightCal_GV_Data[j]))
							{
								iEndLv = iLightCal_LV_Data[j - 1];
								break;
							}
						}

						if (iEndLv == -1)
							iEndLv = iStartLv;

						iFoundMidLV = (iStartLv + iEndLv) / 2;

						pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
						pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];
						pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][i] = iFoundMidLV;
						pAutoCalService->m_iInspectCH_GV[iNoCurImageGrab][i] = iBestGv;

						// 찾은 조명값이 이전 조명값보다 조명 밝기 허용치보다 크게 찾았을 경우 
						if (abs(pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][i] - pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i]) > THEAPP.m_pModelDataManager->m_iLightValueInTol)
						{
							pAutoCalService->m_bJudgeCH[iNoCurImageGrab][i] = false;		// NG
						}
						else
						{
							pAutoCalService->m_bJudgeCH[iNoCurImageGrab][i] = true;		// G
						}
					}
				}
				else		// Not Used Channel
				{
					pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = 0;
					pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = 0;
					pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][i] = 0;
					pAutoCalService->m_iInspectCH_GV[iNoCurImageGrab][i] = 0;

					pAutoCalService->m_bJudgeCH[iNoCurImageGrab][i] = true;			// G
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Page (Channel 전체 테스트)			

			if (bLightControllerPortOpen)
			{
				THEAPP.m_pModelDataManager->m_PageControlData.SetChannelClear_10CH();
				THEAPP.m_pModelDataManager->m_PageControlData.SetTotalChannelllumination_10CH(pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][0], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][1], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][2], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][3],
					pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][4], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][5], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][6], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][7], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][8], pAutoCalService->m_iInspectCH_LV[iNoCurImageGrab][9]);
			}

			tGrabStart = GetTickCount();
			tGrabStart_total = tGrabStart;

			BOOL bGrabSuccess = FALSE;

			CString sDelay;
			double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

			iRetryCnt = 0;

			for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo, iUsedPageIndex);

				Sleep(20);

				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
					{
						break;
					}

					dwGrabEnd = GetTickCount();

					if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
					{
						THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

						strLog.Format("Auto Calibration grabdone timeout");
						THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
						break;
					}

					Sleep(1);
				}

				if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
				{
					bGrabSuccess = TRUE;
					break;
				}

			}	// Grab Retry

			if (!bGrabSuccess)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

			tGrabEnd = GetTickCount();
			strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
			THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

			///////////////////////////////////////////////////////////////////////////

			iCurLightAvgGV = 255;
			dAvgValue = 255;

			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][0], &HTempImageR);
			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][1], &HTempImageG);
			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][2], &HTempImageB);
			Compose3(HTempImageR, HTempImageG, HTempImageB, &HColorImage);

			pAutoCalService->ApplyMatchingAlignInfo(iNoCurImageGrab, &HColorImage, HMatchingHomMat);

			Rgb1ToGray(HColorImage, &HGrayImage);

			HTuple HdAvgValue, HdSd;
			Intensity(HCrossROIHRegion, HGrayImage, &HdAvgValue, &HdSd);

			dAvgValue = HdAvgValue.D();

			iCurLightAvgGV = (int)(dAvgValue + 0.5);
			if (iCurLightAvgGV < 0)
				iCurLightAvgGV = 0;
			if (iCurLightAvgGV > 255)
				iCurLightAvgGV = 255;

			pAutoCalService->m_iTeachTOTAL_GV[iNoCurImageGrab] = THEAPP.m_pModelDataManager->m_iLightAverageValueTotal[iNoCurImageGrab];
			pAutoCalService->m_iInspectTOTAL_GV[iNoCurImageGrab] = iCurLightAvgGV;

			if (abs(pAutoCalService->m_iInspectTOTAL_GV[iNoCurImageGrab] - pAutoCalService->m_iTeachTOTAL_GV[iNoCurImageGrab]) > THEAPP.m_pModelDataManager->m_iTotalImageValueTol)
			{
				pAutoCalService->m_bJudgeTOTAL[iNoCurImageGrab] = false;
			}
			else
			{
				pAutoCalService->m_bJudgeTOTAL[iNoCurImageGrab] = true;
			}

			++iNoCurImageGrab;
		}

		if (bGrabFail == TRUE)
		{
			strLog.Format("Auto calibration image grab fail(No signal)");
			THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
		}

		dTimeEnd = GetTickCount();
		strLog.Format("Auto calibration done, Time(ms): %.4lf", (dTimeEnd - dTimeStart) / 1000);
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		pAutoCalService->m_iLightAutoCalProgressPercent = 100;

		//////////////////////////////////////////////////////////////////////////
		/// Closing

		// 현재 설정되어 있는 채널별 조명값
		int iUsedPageLVChannel[LIGHT_CH_CNT];

		iUsedPageLVChannel[0] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[0];
		iUsedPageLVChannel[1] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[1];
		iUsedPageLVChannel[2] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[2];
		iUsedPageLVChannel[3] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[3];
		iUsedPageLVChannel[4] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[4];
		iUsedPageLVChannel[5] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[5];
		iUsedPageLVChannel[6] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[6];
		iUsedPageLVChannel[7] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[7];
		iUsedPageLVChannel[8] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[8];
		iUsedPageLVChannel[9] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[9];

		if (bLightControllerPortOpen)
		{
			THEAPP.m_pModelDataManager->m_PageControlData.SetChannelClear_10CH();
			THEAPP.m_pModelDataManager->m_PageControlData.SetTotalChannelllumination_10CH(iUsedPageLVChannel[0], iUsedPageLVChannel[1], iUsedPageLVChannel[2], iUsedPageLVChannel[3], iUsedPageLVChannel[4], iUsedPageLVChannel[5], iUsedPageLVChannel[6], iUsedPageLVChannel[7], iUsedPageLVChannel[8], iUsedPageLVChannel[9]);
		}

		if (bLightControllerPortOpen)
			THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.ClosePort();

		//////////////////////////////////////////////////////////////////////////

		CLightAutoCalResultDlg dlg;

		dlg.SetMasterGVMode(FALSE);
		dlg.DoModal();

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AutoCalService::AutoCalLightThread_Specular] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}
}

UINT AutoCalMasterGVThread_Camera(LPVOID lp)
{
	CString strLog;

	CAutoCalService* pAutoCalService = (CAutoCalService*)lp;
	CCameraManager* pCameraManager = THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType];

	try
	{
		strLog.Format("Auto Calibration start");
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		double dTimeStart = 0, dTimeEnd = 0;
		dTimeStart = GetTickCount();

		DWORD dwGrabStart = 0, dwGrabEnd = 0;
		BOOL bGrabFail = FALSE;

		////////////////////////////// 스캔 시작 ////////////////////////////////////
		double dPrevPosZ = -100000000;
		double dInspPosZ;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		int i, j, k;

		int iPcVisionNo;
		iPcVisionNo = pCameraManager->GetVisionCamName();

		//////////////////////////////////////////////////////////////////////////
		/// Calculate Matching Info 

		// Grab Images Using Current Light Condition

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab)
				break;

			CString sGrabTime, sGrabTime_total;
			double tGrabStart = 0, tGrabEnd = 0;
			double tGrabStart_total = 0, tGrabEnd_total = 0;
			CString strLog;

			dInspPosZ = THEAPP.m_pModelDataManager->m_dZFocusPosition[0][iNoCurImageGrab];

			if (dInspPosZ != dPrevPosZ)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iPcVisionNo, STAGE_NUMBER_1, 0, dInspPosZ, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo]) // 무브 컴플리트 기다림
				{
					Sleep(1);
				}

				dPrevPosZ = dInspPosZ;
			}

			tGrabStart = GetTickCount();
			tGrabStart_total = tGrabStart;

			BOOL bGrabSuccess = FALSE;

			CString sDelay;
			double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

			iRetryCnt = 0;

			for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo);

				Sleep(20);

				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
					{
						break;
					}

					dwGrabEnd = GetTickCount();

					if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
					{
						THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

						strLog.Format("Auto Calibration grabdone timeout");
						THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
						break;
					}

					Sleep(1);
				}

				if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
				{
					bGrabSuccess = TRUE;
					break;
				}

			}	// Grab Retry

			if (!bGrabSuccess)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

			tGrabEnd = GetTickCount();
			strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
			THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

			++iNoCurImageGrab;
		}

		if (bGrabFail == TRUE)
		{
			AfxMessageBox("매칭 영상 Grab 시퀀스 오류 발생!!.", MB_ICONERROR | MB_SYSTEMMODAL);
			return 0;
		}

		HObject HMatchingGrabColorImage[MAX_IMAGE_TAB];

		for (i = 0; i < MAX_IMAGE_TAB; i++)
			GenEmptyObj(HMatchingGrabColorImage + i);

		HObject HColorImage;

		for (i = 0; i < THEAPP.m_pModelDataManager->m_iNoUsedImageGrab; i++)
		{
			Compose3(THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][0],
				THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][1],
				THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][2], &HColorImage);
			CopyImage(HColorImage, HMatchingGrabColorImage + i);
		}

		if (0)
		{
			CString sAlignGrabImage;
			for (i = 0; i < THEAPP.m_pModelDataManager->m_iNoUsedImageGrab; i++)
			{
				sAlignGrabImage.Format("c:\\DualTest\\AutoCalGrabImage_%02d", i);
				WriteImage(HMatchingGrabColorImage[i], "bmp", 0, (HTuple)sAlignGrabImage);
			}
		}

		// Get Matching Info for Global Align

		HTuple HMatchingHomMat[MAX_IMAGE_TAB];
		for (i = 0; i < MAX_IMAGE_TAB; i++)
			TupleGenConst(0, 0, HMatchingHomMat + i);

		pAutoCalService->GetMatchingAlignInfo(HMatchingGrabColorImage, HMatchingHomMat);

		//////////////////////////////////////////////////////////////////////////

		int iProgressBarInterval = 100;
		if (THEAPP.m_pModelDataManager->m_iNoUsedImageGrab > 0)
			iProgressBarInterval = 100 / THEAPP.m_pModelDataManager->m_iNoUsedImageGrab;

		BOOL bLightCalROIExist = FALSE;
		int iCurLightAvgGV;
		int iUsedPageIndex = 0;

		iNoCurImageGrab = 0;

		BOOL bLightControllerPortOpen = FALSE;
		int iComPortNumber = THEAPP.m_pModelDataManager->m_PageControlData.m_iComPortNumber;
		if (THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.OpenPort(iComPortNumber, 19200))
			bLightControllerPortOpen = TRUE;

		if (bLightControllerPortOpen == FALSE)
		{
			AfxMessageBox("조명 Controller Port Open 오류!!.", MB_ICONERROR | MB_SYSTEMMODAL);
			return 0;
		}

		HObject HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS;
		HObject HGrayImage;
		int iLV;

		int iLightCHRefGV;
		int iGvDiff, iMinGvDiff, iBestGv;
		int iMinGVDiffStartLV, iMinGVDiffEndLV;
		BOOL bStartLvWithBestGv;
		int iStartLv, iEndLv, iFoundMidLV;

		while (TRUE)
		{
			THEAPP.m_pTabControlDlg->m_pExtraDlg->m_ctrlProgressAutoCal.SetPos(pAutoCalService->m_iLightAutoCalProgressPercent);

			pAutoCalService->m_iLightAutoCalProgressPercent += iProgressBarInterval;

			if (iNoCurImageGrab >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab)
				break;

			CString sGrabTime, sGrabTime_total;
			double tGrabStart = 0, tGrabEnd = 0;
			double tGrabStart_total = 0, tGrabEnd_total = 0;
			CString strLog;

			dInspPosZ = THEAPP.m_pModelDataManager->m_dZFocusPosition[0][iNoCurImageGrab];

			if (fabs(dInspPosZ - dPrevPosZ) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iPcVisionNo, STAGE_NUMBER_1, 0, dInspPosZ, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo]) // 무브 컴플리트 기다림
				{
					Sleep(1);
				}

				dPrevPosZ = dInspPosZ;
			}

			GTRegion *pInspectROIRgn;
			int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

			HObject HCrossROIHRegion, HCrossImageReduced, HEdgeAmp;
			double dAvgValue;

			bLightCalROIExist = FALSE;

			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iNoCurImageGrab + 1)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LIGHT)
					continue;

				HCrossROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

				bLightCalROIExist = TRUE;
				break;
			}

			if (bLightCalROIExist == FALSE)	// 조명 Cal ROI가 없으면 작업 Skip
			{
				++iNoCurImageGrab;
				continue;
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HCrossROIHRegion) == FALSE)
			{
				++iNoCurImageGrab;
				continue;
			}

			//////////////////////////// Start ////////////////////////////////////

			pAutoCalService->m_bAutoCal_Done[iNoCurImageGrab] = true;	// Cal 진행

			// 현재 설정되어 있는 채널별 조명값
			int iCurLVChannel[LIGHT_CH_CNT];

			iCurLVChannel[0] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[0];
			iCurLVChannel[1] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[1];
			iCurLVChannel[2] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[2];
			iCurLVChannel[3] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[3];
			iCurLVChannel[4] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[4];
			iCurLVChannel[5] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[5];
			iCurLVChannel[6] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[6];
			iCurLVChannel[7] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[7];
			iCurLVChannel[8] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[8];
			iCurLVChannel[9] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[9];

			for (i = 0; i < LIGHT_CH_CNT; i++)
			{
				if (iCurLVChannel[i] > 0)		// 조명값이 설정되어 있는 경우에만
				{
					iLV = iCurLVChannel[i];

					if (bLightControllerPortOpen)
					{
						THEAPP.m_pModelDataManager->m_PageControlData.SetChannelllumination_10CH(iUsedPageIndex, i, iLV);
					}

					tGrabStart = GetTickCount();
					tGrabStart_total = tGrabStart;

					BOOL bGrabSuccess = FALSE;

					CString sDelay;
					double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

					iRetryCnt = 0;

					for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
					{
						++iRetryCnt;

						// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo, iUsedPageIndex);

						Sleep(20);

						dwGrabStart = GetTickCount();
						while (1)
						{
							if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
							{
								break;
							}

							dwGrabEnd = GetTickCount();

							if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
							{
								THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

								strLog.Format("Auto Calibration grabdone timeout");
								THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
								break;
							}

							Sleep(1);
						}

						if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
						{
							bGrabSuccess = TRUE;
							break;
						}

					}	// Grab Retry

					if (!bGrabSuccess)		// Skip
					{
						pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
						pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];

						continue;
					}

					tGrabEnd = GetTickCount();
					strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
					THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

					///////////////////////////////////////////////////////////////////////////

					iCurLightAvgGV = 255;
					dAvgValue = 255;

					CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][0], &HTempImageR);
					CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][1], &HTempImageG);
					CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][2], &HTempImageB);
					Compose3(HTempImageR, HTempImageG, HTempImageB, &HColorImage);

					pAutoCalService->ApplyMatchingAlignInfo(iNoCurImageGrab, &HColorImage, HMatchingHomMat);

					Rgb1ToGray(HColorImage, &HGrayImage);

					HTuple HdAvgValue, HdSd;
					Intensity(HCrossROIHRegion, HGrayImage, &HdAvgValue, &HdSd);

					dAvgValue = HdAvgValue.D();

					iCurLightAvgGV = (int)(dAvgValue + 0.5);
					if (iCurLightAvgGV < 0)
						iCurLightAvgGV = 0;
					if (iCurLightAvgGV > 255)
						iCurLightAvgGV = 255;

					pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
					pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = iCurLightAvgGV;
				}
				else		// Not Used Channel
				{
					pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = 0;
					pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = 0;
				}
			}	// for (i=0; i<LIGHT_CH_CNT; i++)

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Page (Channel 전체 테스트)			

			if (bLightControllerPortOpen)
			{
				THEAPP.m_pModelDataManager->m_PageControlData.SetChannelClear_10CH();
				THEAPP.m_pModelDataManager->m_PageControlData.SetTotalChannelllumination_10CH(pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][0], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][1], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][2], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][3],
					pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][4], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][5], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][6], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][7], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][8], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][9]);
			}

			tGrabStart = GetTickCount();
			tGrabStart_total = tGrabStart;

			BOOL bGrabSuccess = FALSE;

			CString sDelay;
			double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

			iRetryCnt = 0;

			for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo, iUsedPageIndex);

				Sleep(20);

				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
					{
						break;
					}

					dwGrabEnd = GetTickCount();

					if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
					{
						THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

						strLog.Format("Auto Calibration grabdone timeout");
						THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
						break;
					}

					Sleep(1);
				}

				if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
				{
					bGrabSuccess = TRUE;
					break;
				}

			}	// Grab Retry

			if (!bGrabSuccess)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

			tGrabEnd = GetTickCount();
			strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
			THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

			///////////////////////////////////////////////////////////////////////////

			iCurLightAvgGV = 255;
			dAvgValue = 255;

			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][0], &HTempImageR);
			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][1], &HTempImageG);
			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][2], &HTempImageB);
			Compose3(HTempImageR, HTempImageG, HTempImageB, &HColorImage);

			pAutoCalService->ApplyMatchingAlignInfo(iNoCurImageGrab, &HColorImage, HMatchingHomMat);

			if (0)
			{
				CString sAlignGrabImage;
				sAlignGrabImage.Format("c:\\DualTest\\CalAlignImage_%02d", iNoCurImageGrab);
				WriteImage(HColorImage, "bmp", 0, (HTuple)sAlignGrabImage);
			}

			Rgb1ToGray(HColorImage, &HGrayImage);

			HTuple HdAvgValue, HdSd;
			Intensity(HCrossROIHRegion, HGrayImage, &HdAvgValue, &HdSd);

			dAvgValue = HdAvgValue.D();

			iCurLightAvgGV = (int)(dAvgValue + 0.5);
			if (iCurLightAvgGV < 0)
				iCurLightAvgGV = 0;
			if (iCurLightAvgGV > 255)
				iCurLightAvgGV = 255;

			pAutoCalService->m_iTeachTOTAL_GV[iNoCurImageGrab] = iCurLightAvgGV;

			++iNoCurImageGrab;
		}

		if (bGrabFail == TRUE)
		{
			strLog.Format("Auto calibration image grab fail(No signal)");
			THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
		}

		dTimeEnd = GetTickCount();
		strLog.Format("Auto calibration done, Time(ms): %.4lf", (dTimeEnd - dTimeStart) / 1000);
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		pAutoCalService->m_iLightAutoCalProgressPercent = 100;

		//////////////////////////////////////////////////////////////////////////
		/// Closing

		// 현재 설정되어 있는 채널별 조명값
		int iUsedPageLVChannel[LIGHT_CH_CNT];

		iUsedPageLVChannel[0] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[0];
		iUsedPageLVChannel[1] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[1];
		iUsedPageLVChannel[2] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[2];
		iUsedPageLVChannel[3] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[3];
		iUsedPageLVChannel[4] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[4];
		iUsedPageLVChannel[5] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[5];
		iUsedPageLVChannel[6] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[6];
		iUsedPageLVChannel[7] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[7];
		iUsedPageLVChannel[8] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[8];
		iUsedPageLVChannel[9] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[9];

		if (bLightControllerPortOpen)
		{
			THEAPP.m_pModelDataManager->m_PageControlData.SetChannelClear_10CH();
			THEAPP.m_pModelDataManager->m_PageControlData.SetTotalChannelllumination_10CH(iUsedPageLVChannel[0], iUsedPageLVChannel[1], iUsedPageLVChannel[2], iUsedPageLVChannel[3],
				iUsedPageLVChannel[4], iUsedPageLVChannel[5], iUsedPageLVChannel[6], iUsedPageLVChannel[7], iUsedPageLVChannel[8], iUsedPageLVChannel[9]);
		}

		if (bLightControllerPortOpen)
			THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.ClosePort();

		//////////////////////////////////////////////////////////////////////////

		for (i = 0; i < MAX_IMAGE_TAB; i++)
		{
			if (pAutoCalService->m_bAutoCal_Done[i] == true)
			{
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][0] = pAutoCalService->m_iTeachCH_GV[i][0];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][1] = pAutoCalService->m_iTeachCH_GV[i][1];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][2] = pAutoCalService->m_iTeachCH_GV[i][2];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][3] = pAutoCalService->m_iTeachCH_GV[i][3];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][4] = pAutoCalService->m_iTeachCH_GV[i][4];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][5] = pAutoCalService->m_iTeachCH_GV[i][5];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][6] = pAutoCalService->m_iTeachCH_GV[i][6];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][7] = pAutoCalService->m_iTeachCH_GV[i][7];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][8] = pAutoCalService->m_iTeachCH_GV[i][8];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][9] = pAutoCalService->m_iTeachCH_GV[i][9];

				THEAPP.m_pModelDataManager->m_iLightAverageValueTotal[i] = pAutoCalService->m_iTeachTOTAL_GV[i];
			}
		}

		CLightAutoCalResultDlg dlg;

		dlg.SetMasterGVMode(TRUE);
		dlg.DoModal();

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AutoCalService::AutoCalMasterGVThread_Camera] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}
}

UINT AutoCalMasterGVThread_Specular(LPVOID lp)
{
	CString strLog;

	CAutoCalService* pAutoCalService = (CAutoCalService*)lp;
	CCameraManager* pCameraManager = THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType];

	try
	{
		strLog.Format("Auto Calibration start");
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		double dTimeStart = 0, dTimeEnd = 0;
		dTimeStart = GetTickCount();

		DWORD dwGrabStart = 0, dwGrabEnd = 0;
		BOOL bGrabFail = FALSE;

		////////////////////////////// 스캔 시작 ////////////////////////////////////
		double dPrevPosZ = -100000000;
		double dInspPosZ;

		int iNoCurImageGrab = 0;
		int iRetryCnt;
		int i, j, k;

		int iPcVisionNo;
		iPcVisionNo = pCameraManager->GetVisionCamName();

		//////////////////////////////////////////////////////////////////////////
		/// Calculate Matching Info 

		// Grab Images Using Current Light Condition

		while (TRUE)
		{
			if (iNoCurImageGrab >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab)
				break;

			CString sGrabTime, sGrabTime_total;
			double tGrabStart = 0, tGrabEnd = 0;
			double tGrabStart_total = 0, tGrabEnd_total = 0;
			CString strLog;

			dInspPosZ = THEAPP.m_pModelDataManager->m_dZFocusPosition[0][iNoCurImageGrab];

			if (fabs(dInspPosZ - dPrevPosZ) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iPcVisionNo, STAGE_NUMBER_1, 0, dInspPosZ, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo]) // 무브 컴플리트 기다림
				{
					Sleep(1);
				}

				dPrevPosZ = dInspPosZ;
			}

			tGrabStart = GetTickCount();
			tGrabStart_total = tGrabStart;

			BOOL bGrabSuccess = FALSE;

			CString sDelay;
			double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

			iRetryCnt = 0;

			for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo);

				Sleep(20);

				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
					{
						break;
					}

					dwGrabEnd = GetTickCount();

					if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
					{
						THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

						strLog.Format("Auto Calibration grabdone timeout");
						THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
						break;
					}

					Sleep(1);
				}

				if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
				{
					bGrabSuccess = TRUE;
					break;
				}

			}	// Grab Retry

			if (!bGrabSuccess)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

			tGrabEnd = GetTickCount();
			strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
			THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

			++iNoCurImageGrab;
		}

		if (bGrabFail == TRUE)
		{
			AfxMessageBox("매칭 영상 Grab 시퀀스 오류 발생!!.", MB_ICONERROR | MB_SYSTEMMODAL);
			return 0;
		}

		HObject HMatchingGrabColorImage[MAX_IMAGE_TAB];

		for (i = 0; i < MAX_IMAGE_TAB; i++)
			GenEmptyObj(HMatchingGrabColorImage + i);

		HObject HColorImage;

		for (i = 0; i < THEAPP.m_pModelDataManager->m_iNoUsedImageGrab; i++)
		{
			Compose3(THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][0],
				THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][1],
				THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_hoCallBackImage[THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->GetGrabCircularIndex()][i][2], &HColorImage);
			CopyImage(HColorImage, HMatchingGrabColorImage + i);
		}

		if (0)
		{
			CString sAlignGrabImage;
			for (i = 0; i < THEAPP.m_pModelDataManager->m_iNoUsedImageGrab; i++)
			{
				sAlignGrabImage.Format("c:\\DualTest\\AutoCalGrabImage_%02d", i);
				WriteImage(HMatchingGrabColorImage[i], "bmp", 0, (HTuple)sAlignGrabImage);
			}
		}

		// Get Matching Info for Global Align

		HTuple HMatchingHomMat[MAX_IMAGE_TAB];
		for (i = 0; i < MAX_IMAGE_TAB; i++)
			TupleGenConst(0, 0, HMatchingHomMat + i);

		pAutoCalService->GetMatchingAlignInfo(HMatchingGrabColorImage, HMatchingHomMat);

		//////////////////////////////////////////////////////////////////////////

		int iProgressBarInterval = 100;
		if (THEAPP.m_pModelDataManager->m_iNoUsedImageGrab > 0)
			iProgressBarInterval = 100 / THEAPP.m_pModelDataManager->m_iNoUsedImageGrab;

		BOOL bLightCalROIExist = FALSE;
		int iCurLightAvgGV;
		int iUsedPageIndex = 0;

		iNoCurImageGrab = 0;

		BOOL bLightControllerPortOpen = FALSE;
		int iComPortNumber = THEAPP.m_pModelDataManager->m_PageControlData.m_iComPortNumber;
		if (THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.OpenPort(iComPortNumber, 19200))
			bLightControllerPortOpen = TRUE;

		if (bLightControllerPortOpen == FALSE)
		{
			AfxMessageBox("조명 Controller Port Open 오류!!.", MB_ICONERROR | MB_SYSTEMMODAL);
			return 0;
		}

		HObject HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS;
		HObject HGrayImage;
		int iLV;

		int iLightCHRefGV;
		int iGvDiff, iMinGvDiff, iBestGv;
		int iMinGVDiffStartLV, iMinGVDiffEndLV;
		BOOL bStartLvWithBestGv;
		int iStartLv, iEndLv, iFoundMidLV;

		while (TRUE)
		{
			THEAPP.m_pTabControlDlg->m_pExtraDlg->m_ctrlProgressAutoCal.SetPos(pAutoCalService->m_iLightAutoCalProgressPercent);

			pAutoCalService->m_iLightAutoCalProgressPercent += iProgressBarInterval;

			if (iNoCurImageGrab >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab)
				break;

			CString sGrabTime, sGrabTime_total;
			double tGrabStart = 0, tGrabEnd = 0;
			double tGrabStart_total = 0, tGrabEnd_total = 0;
			CString strLog;

			dInspPosZ = THEAPP.m_pModelDataManager->m_dZFocusPosition[0][iNoCurImageGrab];

			if (fabs(dInspPosZ - dPrevPosZ) > Z_MOVE_OFFSET)
			{
				THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo] = FALSE;
				THEAPP.m_pHandlerService->Set_AMoveRequest(iPcVisionNo, STAGE_NUMBER_1, 0, dInspPosZ, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE, MOTION_NOUSE);

				while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete[iPcVisionNo]) // 무브 컴플리트 기다림
				{
					Sleep(1);
				}

				dPrevPosZ = dInspPosZ;
			}

			GTRegion *pInspectROIRgn;
			int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

			HObject HCrossROIHRegion, HCrossImageReduced, HEdgeAmp;
			double dAvgValue;

			bLightCalROIExist = FALSE;

			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pInspectROIRgn == NULL)
					continue;

				if (pInspectROIRgn->miTeachImageIndex != iNoCurImageGrab + 1)
					continue;

				if (pInspectROIRgn->miInspectionType != INSPECTION_TYPE_LIGHT)
					continue;

				HCrossROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

				bLightCalROIExist = TRUE;
				break;
			}

			if (bLightCalROIExist == FALSE)	// 조명 Cal ROI가 없으면 작업 Skip
			{
				++iNoCurImageGrab;
				continue;
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HCrossROIHRegion) == FALSE)
			{
				++iNoCurImageGrab;
				continue;
			}

			//////////////////////////// Start ////////////////////////////////////

			pAutoCalService->m_bAutoCal_Done[iNoCurImageGrab] = true;	// Cal 진행

			// 현재 설정되어 있는 채널별 조명값
			int iCurLVChannel[LIGHT_CH_CNT];

			iCurLVChannel[0] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[0];
			iCurLVChannel[1] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[1];
			iCurLVChannel[2] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[2];
			iCurLVChannel[3] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[3];
			iCurLVChannel[4] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[4];
			iCurLVChannel[5] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[5];
			iCurLVChannel[6] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[6];
			iCurLVChannel[7] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[7];
			iCurLVChannel[8] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[8];
			iCurLVChannel[9] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[THEAPP.m_pModelDataManager->m_iLightPageIdx[iNoCurImageGrab]].uiChannel[9];

			for (i = 0; i < LIGHT_CH_CNT; i++)
			{
				if (iCurLVChannel[i] > 0)		// 조명값이 설정되어 있는 경우에만
				{
					iLV = iCurLVChannel[i];

					if (bLightControllerPortOpen)
					{
						THEAPP.m_pModelDataManager->m_PageControlData.SetChannelllumination_10CH(iUsedPageIndex, i, iLV);
					}

					tGrabStart = GetTickCount();
					tGrabStart_total = tGrabStart;

					BOOL bGrabSuccess = FALSE;

					CString sDelay;
					double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

					iRetryCnt = 0;

					for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
					{
						++iRetryCnt;

						// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo, iUsedPageIndex);

						Sleep(20);

						dwGrabStart = GetTickCount();
						while (1)
						{
							if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
							{
								break;
							}

							dwGrabEnd = GetTickCount();

							if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
							{
								THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

								strLog.Format("Auto Calibration grabdone timeout");
								THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
								break;
							}

							Sleep(1);
						}

						if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
						{
							bGrabSuccess = TRUE;
							break;
						}

					}	// Grab Retry

					if (!bGrabSuccess)		// Skip
					{
						pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
						pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = THEAPP.m_pModelDataManager->m_iLightChannelRefGV[iNoCurImageGrab][i];

						continue;
					}

					tGrabEnd = GetTickCount();
					strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
					THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

					///////////////////////////////////////////////////////////////////////////

					iCurLightAvgGV = 255;
					dAvgValue = 255;

					CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][0], &HTempImageR);
					CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][1], &HTempImageG);
					CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][2], &HTempImageB);
					Compose3(HTempImageR, HTempImageG, HTempImageB, &HColorImage);

					pAutoCalService->ApplyMatchingAlignInfo(iNoCurImageGrab, &HColorImage, HMatchingHomMat);

					Rgb1ToGray(HColorImage, &HGrayImage);

					HTuple HdAvgValue, HdSd;
					Intensity(HCrossROIHRegion, HGrayImage, &HdAvgValue, &HdSd);

					dAvgValue = HdAvgValue.D();

					iCurLightAvgGV = (int)(dAvgValue + 0.5);
					if (iCurLightAvgGV < 0)
						iCurLightAvgGV = 0;
					if (iCurLightAvgGV > 255)
						iCurLightAvgGV = 255;

					pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = iCurLVChannel[i];
					pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = iCurLightAvgGV;
				}
				else		// Not Used Channel
				{
					pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][i] = 0;
					pAutoCalService->m_iTeachCH_GV[iNoCurImageGrab][i] = 0;
				}
			}	// for (i=0; i<LIGHT_CH_CNT; i++)

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Page (Channel 전체 테스트)			

			if (bLightControllerPortOpen)
			{
				THEAPP.m_pModelDataManager->m_PageControlData.SetChannelClear_10CH();
				THEAPP.m_pModelDataManager->m_PageControlData.SetTotalChannelllumination_10CH(pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][0], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][1], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][2], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][3],
					pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][4], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][5], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][6], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][7], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][8], pAutoCalService->m_iTeachCH_LV[iNoCurImageGrab][9]);
			}

			tGrabStart = GetTickCount();
			tGrabStart_total = tGrabStart;

			BOOL bGrabSuccess = FALSE;

			CString sDelay;
			double dWattingGrab_StartTime = 0, dWattingGrab_EndtTime = 0;

			iRetryCnt = 0;

			for (int K = 0; K < THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry
			{
				++iRetryCnt;

				// THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->CameraStartGrab(TEACHING_MZ_NO, pAutoCalService->m_iAutoCalPCType, iNoCurImageGrab, iPcVisionNo, iUsedPageIndex);

				Sleep(20);

				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bGrabDone)		// CallBack 된 경우
					{
						break;
					}

					dwGrabEnd = GetTickCount();

					if ((dwGrabEnd - dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
					{
						THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->MdigProcessGrabErrorPostProcess(iNoCurImageGrab);

						strLog.Format("Auto Calibration grabdone timeout");
						THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
						break;
					}

					Sleep(1);
				}

				if (THEAPP.m_pDualCameraManager[pAutoCalService->m_iAutoCalPCType]->m_bReGrab == FALSE)
				{
					bGrabSuccess = TRUE;
					break;
				}

			}	// Grab Retry

			if (!bGrabSuccess)
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다

			tGrabEnd = GetTickCount();
			strLog.Format("Auto calibration matching step, Image: %d, Grab Time(ms): %.0lf ms, Retry: %d", iNoCurImageGrab + 1, tGrabEnd - tGrabStart, iRetryCnt);
			THEAPP.m_log_etc->debug("{}", LOG_CSTR(strLog));

			///////////////////////////////////////////////////////////////////////////

			iCurLightAvgGV = 255;
			dAvgValue = 255;

			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][0], &HTempImageR);
			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][1], &HTempImageG);
			CopyImage(pCameraManager->m_hoCallBackImage[pCameraManager->GetGrabCircularIndex()][iNoCurImageGrab][2], &HTempImageB);
			Compose3(HTempImageR, HTempImageG, HTempImageB, &HColorImage);

			pAutoCalService->ApplyMatchingAlignInfo(iNoCurImageGrab, &HColorImage, HMatchingHomMat);

			if (0)
			{
				CString sAlignGrabImage;
				sAlignGrabImage.Format("c:\\DualTest\\CalAlignImage_%02d", iNoCurImageGrab);
				WriteImage(HColorImage, "bmp", 0, (HTuple)sAlignGrabImage);
			}

			Rgb1ToGray(HColorImage, &HGrayImage);

			HTuple HdAvgValue, HdSd;
			Intensity(HCrossROIHRegion, HGrayImage, &HdAvgValue, &HdSd);

			dAvgValue = HdAvgValue.D();

			iCurLightAvgGV = (int)(dAvgValue + 0.5);
			if (iCurLightAvgGV < 0)
				iCurLightAvgGV = 0;
			if (iCurLightAvgGV > 255)
				iCurLightAvgGV = 255;

			pAutoCalService->m_iTeachTOTAL_GV[iNoCurImageGrab] = iCurLightAvgGV;

			++iNoCurImageGrab;
		}

		if (bGrabFail == TRUE)
		{
			strLog.Format("Auto calibration image grab fail(No signal)");
			THEAPP.m_log_etc->warn("{}", LOG_CSTR(strLog));
		}

		dTimeEnd = GetTickCount();
		strLog.Format("Auto calibration done, Time(ms): %.4lf", (dTimeEnd - dTimeStart) / 1000);
		THEAPP.m_log_etc->info("{}", LOG_CSTR(strLog));

		pAutoCalService->m_iLightAutoCalProgressPercent = 100;

		//////////////////////////////////////////////////////////////////////////
		/// Closing

		// 현재 설정되어 있는 채널별 조명값
		int iUsedPageLVChannel[LIGHT_CH_CNT];

		iUsedPageLVChannel[0] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[0];
		iUsedPageLVChannel[1] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[1];
		iUsedPageLVChannel[2] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[2];
		iUsedPageLVChannel[3] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[3];
		iUsedPageLVChannel[4] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[4];
		iUsedPageLVChannel[5] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[5];
		iUsedPageLVChannel[6] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[6];
		iUsedPageLVChannel[7] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[7];
		iUsedPageLVChannel[8] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[8];
		iUsedPageLVChannel[9] = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[iUsedPageIndex].uiChannel[9];

		if (bLightControllerPortOpen)
		{
			THEAPP.m_pModelDataManager->m_PageControlData.SetChannelClear_10CH();
			THEAPP.m_pModelDataManager->m_PageControlData.SetTotalChannelllumination_10CH(iUsedPageLVChannel[0], iUsedPageLVChannel[1], iUsedPageLVChannel[2], iUsedPageLVChannel[3],
				iUsedPageLVChannel[4], iUsedPageLVChannel[5], iUsedPageLVChannel[6], iUsedPageLVChannel[7], iUsedPageLVChannel[8], iUsedPageLVChannel[9]);
		}

		if (bLightControllerPortOpen)
			THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.ClosePort();

		//////////////////////////////////////////////////////////////////////////

		for (i = 0; i < MAX_IMAGE_TAB; i++)
		{
			if (pAutoCalService->m_bAutoCal_Done[i] == true)
			{
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][0] = pAutoCalService->m_iTeachCH_GV[i][0];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][1] = pAutoCalService->m_iTeachCH_GV[i][1];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][2] = pAutoCalService->m_iTeachCH_GV[i][2];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][3] = pAutoCalService->m_iTeachCH_GV[i][3];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][4] = pAutoCalService->m_iTeachCH_GV[i][4];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][5] = pAutoCalService->m_iTeachCH_GV[i][5];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][6] = pAutoCalService->m_iTeachCH_GV[i][6];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][7] = pAutoCalService->m_iTeachCH_GV[i][7];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][8] = pAutoCalService->m_iTeachCH_GV[i][8];
				THEAPP.m_pModelDataManager->m_iLightChannelRefGV[i][9] = pAutoCalService->m_iTeachCH_GV[i][9];

				THEAPP.m_pModelDataManager->m_iLightAverageValueTotal[i] = pAutoCalService->m_iTeachTOTAL_GV[i];
			}
		}

		CLightAutoCalResultDlg dlg;

		dlg.SetMasterGVMode(TRUE);
		dlg.DoModal();

		return 0;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AutoCalService::AutoCalMasterGVThread_Specular] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return 0;
	}
}

UINT AutoCalFocusThread(LPVOID lp)
{
	return 0;
}

void CAutoCalService::AutoCalLightStart(int iPCType)
{
#ifndef INLINE_MODE

	m_iLightAutoCalProgressPercent = 100;

	CLightAutoCalResultDlg dlg;

	dlg.SetMasterGVMode(FALSE);
	dlg.DoModal();

	return;

#endif

	m_iAutoCalPCType = iPCType;

	if (THEAPP.m_iCurVisionCamType == VISION_NUMBER_1 || THEAPP.m_iCurVisionCamType == VISION_NUMBER_2 ||
		THEAPP.m_iCurVisionCamType == VISION_NUMBER_3 || THEAPP.m_iCurVisionCamType == VISION_NUMBER_4)
		AfxBeginThread(AutoCalLightThread_Specular, this);
	else
		AfxBeginThread(AutoCalLightThread_Camera, this);
}

void CAutoCalService::AutoCalMasterGvStart(int iPCType)
{
#ifndef INLINE_MODE

	m_iLightAutoCalProgressPercent = 100;

	CLightAutoCalResultDlg dlg;

	dlg.SetMasterGVMode(TRUE);
	dlg.DoModal();

	return;

#endif

	m_iAutoCalPCType = iPCType;

	if (THEAPP.m_iCurVisionCamType == VISION_NUMBER_1 || THEAPP.m_iCurVisionCamType == VISION_NUMBER_2 ||
		THEAPP.m_iCurVisionCamType == VISION_NUMBER_3 || THEAPP.m_iCurVisionCamType == VISION_NUMBER_4)
		AfxBeginThread(AutoCalMasterGVThread_Specular, this);
	else
		AfxBeginThread(AutoCalMasterGVThread_Camera, this);
}

void CAutoCalService::AutoCalFocusStart(int iPCType)
{
#ifndef INLINE_MODE

	m_iFocusAutoCalProgressPercent = 100;

	return;
#endif

	m_iAutoCalPCType = iPCType;
	AfxBeginThread(AutoCalFocusThread, this);
}

BOOL CAutoCalService::GetMatchingAlignInfo(HObject *pHGrabColorImage, HTuple *pHMatchingHomMat)
{
	try
	{
		int iMatchingFailImageIndex = -1;
		double dRotationAngleDeg = 0.0;
		double dDeltaX = 0.0;
		double dDeltaY = 0.0;

		BOOL bMatchingSuccess = FALSE;

		int i, j, k;
		HObject HGrayImage, HTempImageR, HTempImageG, HTempImageB, HTempImageH, HTempImageS, HTempImageI;
		int iProcessChType = CHANNEL_TYPE_I;

		for (i = 0; i < THEAPP.m_pModelDataManager->m_iNoUsedImageGrab; i++)
		{
			if (THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i] == TRUE)
			{
				iProcessChType = THEAPP.m_pModelDataManager->m_iLocalMatchingProcessChType[i];

				if (iProcessChType == CHANNEL_TYPE_I)
					Rgb1ToGray(pHGrabColorImage[i], &HGrayImage);
				else if (iProcessChType == CHANNEL_TYPE_R)
					Decompose3(pHGrabColorImage[i], &HGrayImage, &HTempImageG, &HTempImageB);
				else if (iProcessChType == CHANNEL_TYPE_G)
					Decompose3(pHGrabColorImage[i], &HTempImageR, &HGrayImage, &HTempImageB);
				else if (iProcessChType == CHANNEL_TYPE_B)
					Decompose3(pHGrabColorImage[i], &HTempImageR, &HTempImageG, &HGrayImage);
				else if (iProcessChType == CHANNEL_TYPE_H)
				{
					Decompose3(pHGrabColorImage[i], &HTempImageR, &HTempImageG, &HTempImageB);
					TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HGrayImage, &HTempImageS, &HTempImageI, "hsi");
				}
				else if (iProcessChType == CHANNEL_TYPE_S)
				{
					Decompose3(pHGrabColorImage[i], &HTempImageR, &HTempImageG, &HTempImageB);
					TransFromRgb(HTempImageR, HTempImageG, HTempImageB, &HTempImageH, &HGrayImage, &HTempImageI, "hsi");
				}
				else if (iProcessChType == CHANNEL_TYPE_COLOR)
				{
					CopyImage(pHGrabColorImage[i], &HGrayImage);
				}

				if (THEAPP.m_pModelDataManager->m_bUseMatchingFilter[i])
				{
					THEAPP.m_pAlgorithm->GetMatchingPreprocessImage(HGrayImage, THEAPP.m_pModelDataManager->m_iMatchingFilterType[i], THEAPP.m_pModelDataManager->m_dMatchingFilterTypeXSize[i], THEAPP.m_pModelDataManager->m_dMatchingFilterTypeYSize[i], &HGrayImage);
				}

				bMatchingSuccess = THEAPP.m_pAlgorithm->ImageAlignShapeMatching(
					TRUE,
					TEACHING_MZ_NO,
					HGrayImage,
					pHGrabColorImage,
					&(THEAPP.m_pModelDataManager->m_lLAlignModelID[i]),
					THEAPP.m_pModelDataManager->m_bLocalAlignImage[i],
					THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[i],
					THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[i],
					THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[i],
					THEAPP.m_pModelDataManager->m_dLocalMatchingScore[i],
					THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[i],
					THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[i],
					THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[i],
					THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTX[i],
					THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTY[i],
					THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBX[i],
					THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBY[i],
					THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[i],
					THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[i],
					THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[i],
					THEAPP.m_iCurTeachVision,
					THEAPP.m_iCurVisionCamType,
					0,
					i,
					THEAPP.m_pModelDataManager->m_bCheckUseMatchingXYComp[i],
					THEAPP.m_pModelDataManager->m_iLocalIndexNumber[i],
					THEAPP.m_pModelDataManager->m_iRadioMatchingXYRefLine[i],
					THEAPP.m_pModelDataManager->m_bUseMatchingAngleComp[i],
					THEAPP.m_pModelDataManager->m_iMatchingAngleRefLine[i],
					&dRotationAngleDeg,
					&dDeltaX,
					&dDeltaY,
					pHMatchingHomMat + i);

				if (bMatchingSuccess == FALSE)
				{
					iMatchingFailImageIndex = i;
					break;
				}
			}
			else
			{
				TupleGenConst(0, 0, pHMatchingHomMat + i);
			}
		}

		if (bMatchingSuccess == FALSE)
		{
			CString sMsg;
			sMsg.Format("매칭 실패: 영상 탭 %d번", iMatchingFailImageIndex + 1);
			AfxMessageBox(sMsg, MB_SYSTEMMODAL);

			return FALSE;
		}

		return TRUE;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AutoCalService::GetMatchingAlignInfo] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL CAutoCalService::ApplyMatchingAlignInfo(int iAffineTransImageIndex, HObject *pHGrabColorImage, HTuple *pHMatchingHomMat)
{
	try {
		int i, j, k;
		BOOL bRet = FALSE;

		for (i = 0; i < THEAPP.m_pModelDataManager->m_iNoUsedImageGrab; i++)
		{
			if (THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i] == TRUE)	// Global Align
			{
				bRet = THEAPP.m_pAlgorithm->ImageTransformUsingHomMat(
					i,
					iAffineTransImageIndex,
					pHGrabColorImage,
					THEAPP.m_pModelDataManager->m_bLocalAlignImage[i],
					THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[i],
					THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[i],
					pHMatchingHomMat);
			}
		}

		if (bRet == FALSE)
		{
			return FALSE;
		}

		return TRUE;
	}
	catch (HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName.S();
		sErrMsg = HErrMsg.S();

		strLog.Format("Halcon Exception [AutoCalService::ApplyMatchingAlignInfo] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}
