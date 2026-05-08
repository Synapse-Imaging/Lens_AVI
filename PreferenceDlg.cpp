// PreferenceDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "PreferenceDlg.h"
#include "afxdialogex.h"

#include "BOIFAIInspector.h"
#include "ACTFAIInspector.h"
#include "AKCFAIInspector.h"
#include "ATWFAIInspector.h"
#include "RenoFAIInspector.h"


// CPreferenceDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPreferenceDlg, CDialog)

CString BrowseFolderHelper(HWND hwnd, CString strInitialPath, CString strTitle)
{
	CString strReturn = _T("");
	// COM 라이브러리 초기화는 필수입니다.
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	LPTSTR pszDisplayName = NULL;
	BROWSEINFOA browseInfo;
	memset(&browseInfo, 0, sizeof(browseInfo));

	browseInfo.hwndOwner = hwnd;
	browseInfo.pszDisplayName = pszDisplayName;
	browseInfo.lpszTitle = strTitle;
	// **[핵심 변경]** 콜백 함수와 LPARAM을 설정합니다.
	browseInfo.lpfn = BrowseCallbackProc;

	DWORD attributes = GetFileAttributes(strInitialPath);
	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		// 폴더가 존재하지 않거나, 파일이거나, 접근할 수 없음.
		// -> bi.lParam에 NULL 또는 안전한 기본 경로를 전달
		// (예: 바탕 화면 경로를 찾아서 설정하거나, NULL 유지)
		AfxMessageBox(_T("경로가 유효하지 않아 기본 경로에서 시작합니다."));
		// bi.lParam = 0; 또는 bi.lParam = (LPARAM)(LPCTSTR)strFallbackPath;
		strInitialPath = _T("");
	}

	// CString을 LPCTSTR (char* in MBCS)로 캐스팅하여 콜백 함수로 전달합니다.
	// CString 은 (MBCS 환경에서는 내부적으로 char* 임)
	browseInfo.lParam = (LPARAM)(LPCTSTR)strInitialPath;
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	LPITEMIDLIST pIdSelected = NULL;
	// pIdSelected 포인터에 메모리 할당됨.
	// LPITEMIDLIST 로 변환된 주소값.
	pIdSelected = ::SHBrowseForFolder(&browseInfo);

	if (pIdSelected != NULL)
	{
		TCHAR szFolderPath[MAX_PATH] = {0};

		if (::SHGetPathFromIDList(pIdSelected, szFolderPath))
		{
			strReturn = CString(szFolderPath);
		}
		else
		{
			AfxMessageBox(_T("파일 시스템 경로로 변환할 수 없습니다."), MB_OK | MB_ICONWARNING);
		}

		// 5. 메모리 해제
		// SHBrowseForFolder 와 같은 Windows Shell API 는 COM 기반 함수.
		// COM 기반 함수는 CoTaskMemFree 로 메모리를 해제해야 한다.
		// 일반적인 c++ (delete()) 혹은 c 스타일 (free()) 와는 메모리 힙이 다르기때문에 에러.
		CoTaskMemFree(pIdSelected);
	}

	if (pszDisplayName != NULL)
		CoTaskMemFree(pszDisplayName);

	CoUninitialize();

	return strReturn;
}

CPreferenceDlg::CPreferenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferenceDlg::IDD, pParent)
{
	m_strEquipNo = _T("#1");
	m_strEditEquipModelType = "";

	m_iRadioPCType = PC_NUMBER_1;

	m_bCheckUseVision[VISION_NUMBER_1] = TRUE;
	m_bCheckUseVision[VISION_NUMBER_2] = TRUE;
	m_bCheckUseVision[VISION_NUMBER_3] = TRUE;
	m_bCheckUseVision[VISION_NUMBER_4] = TRUE;

	m_bCheckUseLAS1 = FALSE;
	m_bCheckUseLAS2 = FALSE;
	m_bCheckUseLAS3 = FALSE;

	m_strSaveFolderPathLAS1 = _T("");
	m_strSaveFolderPathLAS2 = _T("");
	m_strSaveFolderPathLAS3 = _T("");

	m_EditSaveLASStatus = _T("");
	m_bCheckUseLASConnectionCheck = FALSE;

	m_strRawFolderPath = _T("");
	m_strResultFolderPath = _T("");
	m_strReviewFolderPath = _T("");
	m_strAdjFolderPath = _T("");
	m_strResultLogFolderPath = _T("");
	m_strEtcFolderPath = _T("");
	m_strOfflineTestFolderPath = _T("");

	m_bCheckRawImageSaveLocal = FALSE;
	m_bCheckResultImageSaveLocal = FALSE;
	m_bCheckReviewImageSaveLocal = FALSE;
	m_bCheckADJImageSaveLocal = FALSE;
	m_bCheckResultLogSaveLocal = FALSE;

	m_bCheckRawImageSaveLAS = FALSE;
	m_bCheckResultImageSaveLAS = FALSE;
	m_bCheckReviewImageSaveLAS = FALSE;
	m_bCheckADJImageSaveLAS = FALSE;
	m_bCheckResultLogSaveLAS = FALSE;

	m_bCheckUseRawImageThread = FALSE;
	m_bCheckUseResultImageThread = FALSE;
	m_bCheckUseReviewImageThread = FALSE;
	m_bCheckUseADJImageThread = FALSE;
	m_bCheckUseResultLogThread = FALSE;

	m_bCheckUseRawImageTempDrive = FALSE;

	m_bCheckSaveRawImage = TRUE;
	m_bRadioRawImageType = TRUE;
	m_bCheckReduceRawImage = FALSE;
	m_dEditRawImageZoomRatio = 0.3;
	m_bCheckCombineRawImage = FALSE;
	m_bCheckSaveResultGoodImage = TRUE;
	m_bCheckUseResultImageThreadOnlyGood = FALSE;
	m_bRadioRawImageSaveLogType = FALSE;
	m_bCheckAvoidDuplicateLotLocal = FALSE;
	m_bCheckAvoidDuplicateLotLas = FALSE;

	m_iRadioRawImageThreadType = 0;

	m_bCheckUseMultiQueue = FALSE;
	m_iEditThreadQueueNumber = 5;

	m_iRadioResultImageDefectDisplayType = RESULT_IMAGE_DEFECT_DISPLAY_SIZE;
	m_bCheckUseResultImageDefectSizeLimit = TRUE;
	m_iEditResultImageDefectSizeMax = 3;

	m_bCheckSaveADJ = FALSE;
	m_bCheckUseADJConnectionCheck = FALSE;
	m_bCheckUseLocalADJ = FALSE;
	m_bCheckUseLocalSEG = FALSE;
	m_strEditAdjCurrentIP = "127.0.0.1";
	m_bCheckUseADJ[0] = FALSE;
	m_strADJ_IP[0] = _T("");
	m_nEditAdjPort[0] = 10001;
	m_bCheckUseADJ[1] = FALSE;
	m_strADJ_IP[1] = _T("");
	m_nEditAdjPort[1] = 10002;
	m_bCheckIsSendADJ = FALSE;
	m_bCheckUseADJMultiimage = FALSE;
	m_bCheckIsApplyADJ = FALSE;
	m_bCheckUseADJTimeOut = TRUE;
	m_nADJTimeOut = 500;
	m_nEditADJMultiNetworkNO = 1;
	m_iEditADJImageSize = 256;
	m_iEditADJImageReSize = 256;
	m_iEditAdjMarkingRad = 3;
	m_iEditADJCombineDefectDistance = 50;
	m_bCheckSaveROIImage = FALSE;
	m_iRadioSaveROIImageType = 0;
	m_bCheckAISetupUseVAL = FALSE;
	m_bCheckAllImageAi = FALSE;

	m_bCheckSaveMDJ = FALSE;
	m_iEditMaxReviewImageNumber = 3;
	m_iEditReviewCombineDefectDistance = 200;
	m_bCheckUseSaveFaiReviewImage = FALSE;

	m_iRadioReviewImageDisplayType = 0;

	m_iEditVision1LightIP1 = 192;
	m_iEditVision1LightIP2 = 168;
	m_iEditVision1LightIP3 = 0;
	m_iEditVision1LightIP4 = 101;
	m_iEditVision1LightPort = 5000;

	m_iEditVision2LightIP1 = 192;
	m_iEditVision2LightIP2 = 168;
	m_iEditVision2LightIP3 = 0;
	m_iEditVision2LightIP4 = 102;
	m_iEditVision2LightPort = 5000;

	m_iEditVision3LightIP1 = 192;
	m_iEditVision3LightIP2 = 168;
	m_iEditVision3LightIP3 = 0;
	m_iEditVision3LightIP4 = 103;
	m_iEditVision3LightPort = 5000;

	m_iEditVision4LightIP1 = 192;
	m_iEditVision4LightIP2 = 168;
	m_iEditVision4LightIP3 = 0;
	m_iEditVision4LightIP4 = 104;
	m_iEditVision4LightPort = 5000;

	m_iEditGrabErrRetryNo = 1;
	m_iEditGrabDelayTime = 0;
	m_iEditTriggerSleepTime = 30;
	m_iEditGrabDoneWaitTime = 500;
	m_iEditTeachingLiveGrabPeriod = 100;
	m_iEditLightErrValue = 30;
	m_iRadioTeachingCameraLightType = 0;
	m_bCheckUseGrabHold = FALSE;
	m_iEditFocusMoveNoRetry = 3;
	m_iEditMoveCompleteWaitTime = 500;
	m_bCheckUse4dSaveRawImage = FALSE;
	m_bCheckUseAutoFocus = FALSE;
	m_bCheckUseFastGrab = FALSE;
	m_bCheckUseBatchInspection = FALSE;
	m_bCheckSendGrabFailError = FALSE;
	m_iEditSendGrabFailErrorCount = 5;

	m_iDefectDispDist = 0;
	m_iEditMatchingSearchMargin = 100;
	m_iRadioBarcodeNoReadImageType = 0;
	m_bCheckUseSaveDefectShapeFeature = FALSE;
	m_iEditMaxSameDefectShapeFeature = 10;
	m_bCheckUseSaveGVEdgeMornitoringLog = FALSE;
	m_bCheckUseGpuAffineTrans = FALSE;
	m_bCheckUseDeleteCrossBar = FALSE;
	m_iEditSaveRecentlyCompleteInfoNumber = 30;

	m_bCheckUseContDefectAlarm = FALSE;
	m_iEditContDefectPosTolerance = 30;
	m_iEditContDefectCount = 5;

	m_iEditSam2IP_1 = 192;
	m_iEditSam2IP_2 = 168;
	m_iEditSam2IP_3 = 0;
	m_iEditSam2IP_4 = 251;
	m_iEditSam2Port = 6000;

	m_bCheckUseVisionCopy = FALSE;
	m_bCheckVmTrainAll = FALSE;

	m_bCheckUseVmThresOpt = FALSE;
	m_iEditVmSplitThres = 10;
	m_dEditVmKSigma = 3.0;

	m_iRadioVmTrain = VM_TRAIN_NOUSE;
}

CPreferenceDlg::~CPreferenceDlg()
{
}

void CPreferenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_EQUIPNO, m_strEquipNo);
	DDX_Text(pDX, IDC_EDIT_CURRENT_INSPECTION_MODEL_TYPE, m_strEditEquipModelType);

	DDX_Radio(pDX, IDC_RADIO_INSPECTION_PC1, m_iRadioPCType);

	DDX_Check(pDX, IDC_CHECK_USE_VISION1, m_bCheckUseVision[VISION_NUMBER_1]);
	DDX_Check(pDX, IDC_CHECK_USE_VISION2, m_bCheckUseVision[VISION_NUMBER_2]);
	DDX_Check(pDX, IDC_CHECK_USE_VISION3, m_bCheckUseVision[VISION_NUMBER_3]);
	DDX_Check(pDX, IDC_CHECK_USE_VISION4, m_bCheckUseVision[VISION_NUMBER_4]);

	DDX_Check(pDX, IDC_CHECK_USE_LAS1, m_bCheckUseLAS1);
	DDX_Check(pDX, IDC_CHECK_USE_LAS2, m_bCheckUseLAS2);
	DDX_Check(pDX, IDC_CHECK_USE_LAS3, m_bCheckUseLAS3);

	DDX_Text(pDX, IDC_EDIT_SAVE_FOLDERPATH_LAS1, m_strSaveFolderPathLAS1);
	DDX_Text(pDX, IDC_EDIT_SAVE_FOLDERPATH_LAS2, m_strSaveFolderPathLAS2);
	DDX_Text(pDX, IDC_EDIT_SAVE_FOLDERPATH_LAS3, m_strSaveFolderPathLAS3);

	DDX_Text(pDX, IDC_EDIT_SAVE_LAS_STATUS, m_EditSaveLASStatus);
	DDX_Check(pDX, IDC_CHECK_USE_LAS_CONNECTION_CHECK, m_bCheckUseLASConnectionCheck);

	DDX_Text(pDX, IDC_EDIT_RAW_FOLDERPATH, m_strRawFolderPath);
	DDX_Text(pDX, IDC_EDIT_RESULT_FOLDERPATH, m_strResultFolderPath);
	DDX_Text(pDX, IDC_EDIT_REVIEW_FOLDERPATH, m_strReviewFolderPath);
	DDX_Text(pDX, IDC_EDIT_ADJ_FOLDERPATH, m_strAdjFolderPath);
	DDX_Text(pDX, IDC_EDIT_RESULT_LOG_FOLDERPATH, m_strResultLogFolderPath);
	DDX_Text(pDX, IDC_EDIT_ETC_LOG_FOLDERPATH, m_strEtcFolderPath);
	DDX_Text(pDX, IDC_EDIT_OFFLINE_TEST_FOLDERPATH, m_strOfflineTestFolderPath);

	DDX_Check(pDX, IDC_CHECK_RAW_IMAGE_SAVE_LOCAL, m_bCheckRawImageSaveLocal);
	DDX_Check(pDX, IDC_CHECK_RESULT_IMAGE_SAVE_LOCAL, m_bCheckResultImageSaveLocal);
	DDX_Check(pDX, IDC_CHECK_REVIEW_IMAGE_SAVE_LOCAL, m_bCheckReviewImageSaveLocal);
	DDX_Check(pDX, IDC_CHECK_ADJ_IMAGE_SAVE_LOCAL, m_bCheckADJImageSaveLocal);
	DDX_Check(pDX, IDC_CHECK_RESULT_LOG_SAVE_LOCAL, m_bCheckResultLogSaveLocal);

	DDX_Check(pDX, IDC_CHECK_RAW_IMAGE_SAVE_LAS, m_bCheckRawImageSaveLAS);
	DDX_Check(pDX, IDC_CHECK_RESULT_IMAGE_SAVE_LAS, m_bCheckResultImageSaveLAS);
	DDX_Check(pDX, IDC_CHECK_REVIEW_IMAGE_SAVE_LAS, m_bCheckReviewImageSaveLAS);
	DDX_Check(pDX, IDC_CHECK_ADJ_IMAGE_SAVE_LAS, m_bCheckADJImageSaveLAS);
	DDX_Check(pDX, IDC_CHECK_RESULT_LOG_SAVE_LAS, m_bCheckResultLogSaveLAS);

	DDX_Check(pDX, IDC_CHECK_USE_RAW_IMAGE_THREAD, m_bCheckUseRawImageThread);
	DDX_Check(pDX, IDC_CHECK_USE_RESULT_IMAGE_THREAD, m_bCheckUseResultImageThread);
	DDX_Check(pDX, IDC_CHECK_USE_REVIEW_IMAGE_THREAD, m_bCheckUseReviewImageThread);
	DDX_Check(pDX, IDC_CHECK_USE_ADJ_IMAGE_THREAD, m_bCheckUseADJImageThread);
	DDX_Check(pDX, IDC_CHECK_USE_RESULT_LOG_THREAD, m_bCheckUseResultLogThread);

	DDX_Check(pDX, IDC_CHECK_USE_RAW_IMAGE_TEMP_DRIVE, m_bCheckUseRawImageTempDrive);

	DDX_Check(pDX, IDC_CHECK_SAVE_RAW_IMAGE, m_bCheckSaveRawImage);
	DDX_Radio(pDX, IDC_RADIO_RAW_IMAGE_BMP, m_bRadioRawImageType);
	DDX_Check(pDX, IDC_CHECK_REDUCE_RAW_IMAGE, m_bCheckReduceRawImage);
	DDX_Text(pDX, IDC_EDIT_RAW_IMAGE_ZOOM_RATIO, m_dEditRawImageZoomRatio);
	DDX_Check(pDX, IDC_CHECK_COMBINE_RAW_IMAGE, m_bCheckCombineRawImage);
	DDX_Check(pDX, IDC_CHECK_SAVE_GOOD_RESULT_IMAGE, m_bCheckSaveResultGoodImage);
	DDX_Check(pDX, IDC_CHECK_USE_THREAD_ONLY_GOOD_RESULT_IMAGE, m_bCheckUseResultImageThreadOnlyGood);
	DDX_Radio(pDX, IDC_RADIO_RAW_IMAGE_SAVE_LOG_MODULE, m_bRadioRawImageSaveLogType);
	DDX_Check(pDX, IDC_AVOID_DUPLICATE_LOT_LOCAL, m_bCheckAvoidDuplicateLotLocal);
	DDX_Check(pDX, IDC_AVOID_DUPLICATE_LOT_LAS, m_bCheckAvoidDuplicateLotLas);

	DDX_Radio(pDX, IDC_RADIO_RAW_IMAGE_THREAD_TYPE_1, m_iRadioRawImageThreadType);

	DDX_Check(pDX, IDC_CHECK_USE_MULTI_QUEUE, m_bCheckUseMultiQueue);
	DDX_Text(pDX, IDC_EDIT_THREAD_QUEUE_NUMBER, m_iEditThreadQueueNumber);

	DDX_Radio(pDX, IDC_RADIO_RESULT_DEFECT_DISPLAY_SIZE, m_iRadioResultImageDefectDisplayType);
	DDX_Check(pDX, IDC_CHECK_USE_RESULT_IMAGE_DEFECT_SIZE_LIMIT, m_bCheckUseResultImageDefectSizeLimit);
	DDX_Text(pDX, IDC_EDIT_RESULT_IMAGE_DEFECT_SIZE_MAX, m_iEditResultImageDefectSizeMax);

	DDX_Check(pDX, IDC_CHECK_SAVE_ADJ, m_bCheckSaveADJ);
	DDX_Check(pDX, IDC_CHECK_USE_ADJ_CONNECTION_CHECK, m_bCheckUseADJConnectionCheck);
	DDX_Check(pDX, IDC_CHECK_USE_LOCAL_ADJ, m_bCheckUseLocalADJ);
	DDX_Check(pDX, IDC_CHECK_USE_LOCAL_SEG, m_bCheckUseLocalSEG);
	DDX_Text(pDX, IDC_EDIT_ADJ_CURRENT_IP, m_strEditAdjCurrentIP);
	DDX_Check(pDX, IDC_CHECK_USE_ADJ1, m_bCheckUseADJ[0]);
	DDX_Text(pDX, IDC_IPADDRESS_ADJ_IP, m_strADJ_IP[0]);
	DDX_Text(pDX, IDC_EDIT_ADJ_PORT_0, m_nEditAdjPort[0]);
	DDX_Check(pDX, IDC_CHECK_USE_ADJ2, m_bCheckUseADJ[1]);
	DDX_Text(pDX, IDC_IPADDRESS_ADJ_IP2, m_strADJ_IP[1]);
	DDX_Text(pDX, IDC_EDIT_ADJ_PORT_1, m_nEditAdjPort[1]);
	DDX_Check(pDX, IDC_CHECK_SEND_ADJ, m_bCheckIsSendADJ);
	DDX_Check(pDX, IDC_CHECK_USE_ADJ_MULTIIMAGE, m_bCheckUseADJMultiimage);
	DDX_Check(pDX, IDC_CHECK_APPLY_ADJ, m_bCheckIsApplyADJ);
	DDX_Check(pDX, IDC_CHECK_ADJ_TIMEOUT, m_bCheckUseADJTimeOut);
	DDX_Text(pDX, IDC_EDIT_ADJTIMEOUT, m_nADJTimeOut);
	DDX_Text(pDX, IDC_EDIT_ADJ_MULTI_NETWORK_CONNECTION, m_nEditADJMultiNetworkNO);
	DDX_Text(pDX, IDC_EDIT_ADJ_IMAGE_SIZE, m_iEditADJImageSize);
	DDX_Text(pDX, IDC_EDIT_ADJ_IMAGE_RESIZE, m_iEditADJImageReSize);
	DDX_Text(pDX, IDC_EDIT_ADJ_MARKING_RADIUS, m_iEditAdjMarkingRad);
	DDX_Text(pDX, IDC_EDIT_ADJ_COMBINE_DEFECT_DISTANCE, m_iEditADJCombineDefectDistance);
	DDX_Check(pDX, IDC_CHECK_SAVE_ROI_IMAGE, m_bCheckSaveROIImage);
	DDX_Radio(pDX, IDC_RADIO_SAVE_ROI_IMAGE_TYPE_1, m_iRadioSaveROIImageType);
	DDX_Check(pDX, IDC_CHECK_AI_SETUP_USE_VAL, m_bCheckAISetupUseVAL);
	DDX_Check(pDX, IDC_CHECK_ALL_IMAGE_AI, m_bCheckAllImageAi);

	DDX_Check(pDX, IDC_CHECK_SAVE_MDJ, m_bCheckSaveMDJ);
	DDX_Text(pDX, IDC_EDIT_MAX_REVIEW_IMAGE_NUMBER, m_iEditMaxReviewImageNumber);
	DDX_Text(pDX, IDC_EDIT_ROS_COMBINE_DEFECT_DISTANCE, m_iEditReviewCombineDefectDistance);
	DDX_Check(pDX, IDC_CHECK_USE_SAVE_FAI_REVIEW_IMAGE, m_bCheckUseSaveFaiReviewImage);

	DDX_Radio(pDX, IDC_RADIO_REVIEWIMAGE_DISPLAY1, m_iRadioReviewImageDisplayType);

	DDX_Text(pDX, IDC_EDIT_VISION1_LIGHT_IP1, m_iEditVision1LightIP1);
	DDX_Text(pDX, IDC_EDIT_VISION1_LIGHT_IP2, m_iEditVision1LightIP2);
	DDX_Text(pDX, IDC_EDIT_VISION1_LIGHT_IP3, m_iEditVision1LightIP3);
	DDX_Text(pDX, IDC_EDIT_VISION1_LIGHT_IP4, m_iEditVision1LightIP4);
	DDX_Text(pDX, IDC_EDIT_VISION1_LIGHT_PORT, m_iEditVision1LightPort);

	DDX_Text(pDX, IDC_EDIT_VISION2_LIGHT_IP1, m_iEditVision2LightIP1);
	DDX_Text(pDX, IDC_EDIT_VISION2_LIGHT_IP2, m_iEditVision2LightIP2);
	DDX_Text(pDX, IDC_EDIT_VISION2_LIGHT_IP3, m_iEditVision2LightIP3);
	DDX_Text(pDX, IDC_EDIT_VISION2_LIGHT_IP4, m_iEditVision2LightIP4);
	DDX_Text(pDX, IDC_EDIT_VISION2_LIGHT_PORT, m_iEditVision2LightPort);

	DDX_Text(pDX, IDC_EDIT_VISION3_LIGHT_IP1, m_iEditVision3LightIP1);
	DDX_Text(pDX, IDC_EDIT_VISION3_LIGHT_IP2, m_iEditVision3LightIP2);
	DDX_Text(pDX, IDC_EDIT_VISION3_LIGHT_IP3, m_iEditVision3LightIP3);
	DDX_Text(pDX, IDC_EDIT_VISION3_LIGHT_IP4, m_iEditVision3LightIP4);
	DDX_Text(pDX, IDC_EDIT_VISION3_LIGHT_PORT, m_iEditVision3LightPort);

	DDX_Text(pDX, IDC_EDIT_VISION4_LIGHT_IP1, m_iEditVision4LightIP1);
	DDX_Text(pDX, IDC_EDIT_VISION4_LIGHT_IP2, m_iEditVision4LightIP2);
	DDX_Text(pDX, IDC_EDIT_VISION4_LIGHT_IP3, m_iEditVision4LightIP3);
	DDX_Text(pDX, IDC_EDIT_VISION4_LIGHT_IP4, m_iEditVision4LightIP4);
	DDX_Text(pDX, IDC_EDIT_VISION4_LIGHT_PORT, m_iEditVision4LightPort);

	DDX_Text(pDX, IDC_EDIT_GRAB_ERROR_NO_RETRY, m_iEditGrabErrRetryNo);
	DDX_Text(pDX, IDC_EDIT_GRAB_DELAY_TIME, m_iEditGrabDelayTime);
	DDX_Text(pDX, IDC_EDIT_TRIGGER_SLEEP_TIME, m_iEditTriggerSleepTime);
	DDX_Text(pDX, IDC_EDIT_GRAB_DONE_WAIT_TIME, m_iEditGrabDoneWaitTime);
	DDX_Text(pDX, IDC_EDIT_TEACHING_LIVE_GRAB_PERIOD, m_iEditTeachingLiveGrabPeriod);
	DDX_Text(pDX, IDC_EDIT_LIGHT_ERRVALUE, m_iEditLightErrValue);
	DDX_Radio(pDX, IDC_RADIO_CAMERA_LIGHT_PAGE, m_iRadioTeachingCameraLightType);
	DDX_Check(pDX, IDC_CHECK_USE_GRAB_HOLD, m_bCheckUseGrabHold);
	DDX_Text(pDX, IDC_EDIT_FOCUS_MOVE_NO_RETRY, m_iEditFocusMoveNoRetry);
	DDX_Text(pDX, IDC_EDIT_MOVE_COMPLETE_WAIT_TIME, m_iEditMoveCompleteWaitTime);
	DDX_Check(pDX, IDC_CHECK_USE_4D_SAVE_RAW_IMAGE, m_bCheckUse4dSaveRawImage);
	DDX_Check(pDX, IDC_CHECK_USE_AUTO_FOCUS, m_bCheckUseAutoFocus);
	DDX_Check(pDX, IDC_CHECK_USE_FAST_GRAB, m_bCheckUseFastGrab);
	DDX_Check(pDX, IDC_CHECK_USE_BATCH_INSPECTION, m_bCheckUseBatchInspection);	
	DDX_Check(pDX, IDC_CHECK_SEND_GRAB_FAIL_ERROR, m_bCheckSendGrabFailError);
	DDX_Text(pDX, IDC_EDIT_SEND_GRAB_FAIL_ERROR_COUNT, m_iEditSendGrabFailErrorCount);

	DDX_Text(pDX, IDC_EDIT_DISP_SET_DIST, m_iDefectDispDist);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SEARCH_MARGIN, m_iEditMatchingSearchMargin);
	DDX_Radio(pDX, IDC_RADIO_BARCODE_NOREAD_IMAGE_BMP, m_iRadioBarcodeNoReadImageType);
	DDX_Check(pDX, IDC_CHECK_USE_SAVE_DEFECT_SHAPE_FEATURE, m_bCheckUseSaveDefectShapeFeature);
	DDX_Text(pDX, IDC_EDIT_MAX_SAME_DEFECT_SHAPE_FEATURE, m_iEditMaxSameDefectShapeFeature);
	DDX_Check(pDX, IDC_CHECK_USE_SAVE_GV_EDGE_MORNITORING_LOG, m_bCheckUseSaveGVEdgeMornitoringLog);
	DDX_Check(pDX, IDC_CHECK_USE_GPU_AFFINE_TRANS, m_bCheckUseGpuAffineTrans);
	DDX_Check(pDX, IDC_CHECK_USE_DELETE_CROSS_BAR, m_bCheckUseDeleteCrossBar);
	DDX_Text(pDX, IDC_EDIT_SAVE_RECENTLY_COMPLETE_INFO_NUMBER, m_iEditSaveRecentlyCompleteInfoNumber);

	DDX_Check(pDX, IDC_CHECK_USE_CONT_DEFECT_ALARM, m_bCheckUseContDefectAlarm);
	DDX_Text(pDX, IDC_EDIT_CONT_DEFECT_POS_TOLERANCE, m_iEditContDefectPosTolerance);
	DDX_Text(pDX, IDC_EDIT_CONT_DEFECT_COUNT, m_iEditContDefectCount);

	DDX_Text(pDX, IDC_EDIT_SAM2_IP_1, m_iEditSam2IP_1);
	DDX_Text(pDX, IDC_EDIT_SAM2_IP_2, m_iEditSam2IP_2);
	DDX_Text(pDX, IDC_EDIT_SAM2_IP_3, m_iEditSam2IP_3);
	DDX_Text(pDX, IDC_EDIT_SAM2_IP_4, m_iEditSam2IP_4);
	DDX_Text(pDX, IDC_EDIT_SAM2_PORT, m_iEditSam2Port);

	DDX_Check(pDX, IDC_CHECK_USE_VISION_COPY, m_bCheckUseVisionCopy);

	DDX_Radio(pDX, IDC_RADIO_VM_TRAIN_1, m_iRadioVmTrain);
	DDX_Check(pDX, IDC_CHECK_VM_TRAIN_ALL, m_bCheckVmTrainAll);

	DDX_Check(pDX, IDC_CHECK_USE_VM_THRES_OPT, m_bCheckUseVmThresOpt);
	DDX_Text(pDX, IDC_EDIT_VM_VAR_SPLIT_THRES, m_iEditVmSplitThres);
	DDX_Text(pDX, IDC_EDIT_VM_K_SIGMA, m_dEditVmKSigma);

	DDX_Control(pDX, IDOK, m_bnOK);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);
}


BEGIN_MESSAGE_MAP(CPreferenceDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPreferenceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPreferenceDlg::OnBnClickedCancel)

	ON_BN_CLICKED(IDC_BUTTON_SELECT_INSPECTION_MODEL_TYPE, &CPreferenceDlg::OnBnClickedButtonSelectInspectionModelType)
	ON_BN_CLICKED(IDC_BUTTON_LANGUAGE_CHANGE, &CPreferenceDlg::OnBnClickedButtonLanguageChange)

	ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_LAS1, &CPreferenceDlg::OnBnClickedButtonFindfolderLas1)
	ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_LAS2, &CPreferenceDlg::OnBnClickedButtonFindfolderLas2)
	ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_LAS3, &CPreferenceDlg::OnBnClickedButtonFindfolderLas3)

	ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_RAW, &CPreferenceDlg::OnBnClickedButtonFindfolderRaw)
	ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_RESULT, &CPreferenceDlg::OnBnClickedButtonFindfolderResult)
	ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_REVIEW, &CPreferenceDlg::OnBnClickedButtonFindfolderReview)
	ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_ADJ, &CPreferenceDlg::OnBnClickedButtonFindfolderAdj)
	ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_RESULTLOG, &CPreferenceDlg::OnBnClickedButtonFindfolderResultlog)
	ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_ETC, &CPreferenceDlg::OnBnClickedButtonFindfolderEtc)
	ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_DEBUG, &CPreferenceDlg::OnBnClickedButtonFindfolderDebug)

	ON_BN_CLICKED(IDC_CHECK_RAW_IMAGE_SAVE_LOCAL, &CPreferenceDlg::OnBnClickedCheckRawImageSaveLocal)
	ON_BN_CLICKED(IDC_CHECK_RESULT_IMAGE_SAVE_LOCAL, &CPreferenceDlg::OnBnClickedCheckResultImageSaveLocal)
	ON_BN_CLICKED(IDC_CHECK_REVIEW_IMAGE_SAVE_LOCAL, &CPreferenceDlg::OnBnClickedCheckReviewImageSaveLocal)
	ON_BN_CLICKED(IDC_CHECK_ADJ_IMAGE_SAVE_LOCAL, &CPreferenceDlg::OnBnClickedCheckAdjImageSaveLocal)
	ON_BN_CLICKED(IDC_CHECK_RESULT_LOG_SAVE_LOCAL, &CPreferenceDlg::OnBnClickedCheckResultLogSaveLocal)

	ON_BN_CLICKED(IDC_CHECK_RAW_IMAGE_SAVE_LAS, &CPreferenceDlg::OnBnClickedCheckRawImageSaveLas)
	ON_BN_CLICKED(IDC_CHECK_RESULT_IMAGE_SAVE_LAS, &CPreferenceDlg::OnBnClickedCheckResultImageSaveLas)
	ON_BN_CLICKED(IDC_CHECK_REVIEW_IMAGE_SAVE_LAS, &CPreferenceDlg::OnBnClickedCheckReviewImageSaveLas)
	ON_BN_CLICKED(IDC_CHECK_ADJ_IMAGE_SAVE_LAS, &CPreferenceDlg::OnBnClickedCheckAdjImageSaveLas)
	ON_BN_CLICKED(IDC_CHECK_RESULT_LOG_SAVE_LAS, &CPreferenceDlg::OnBnClickedCheckResultLogSaveLas)

	ON_BN_CLICKED(IDC_CHECK_USE_RAW_IMAGE_THREAD, &CPreferenceDlg::OnBnClickedCheckUseRawImageThread)
	ON_BN_CLICKED(IDC_CHECK_USE_RESULT_IMAGE_THREAD, &CPreferenceDlg::OnBnClickedCheckUseResultImageThread)

	ON_BN_CLICKED(IDC_CHECK_USE_THREAD_ONLY_GOOD_RESULT_IMAGE, &CPreferenceDlg::OnBnClickedCheckUseResultImageThreadOnlyGood)

	ON_BN_CLICKED(IDC_BUTTON_SET_PRIORITY, &CPreferenceDlg::OnBnClickedButtonSetDefectPriority)
	ON_BN_CLICKED(IDC_BUTTON_SET_RESULT_IMAGE, &CPreferenceDlg::OnBnClickedButtonSetResultImage)

	ON_BN_CLICKED(IDC_BUTTON_CONNECT_ADJ1, &CPreferenceDlg::OnBnClickedButtonAdjconnect)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT_ADJ2, &CPreferenceDlg::OnBnClickedButtonAdjconnect2)
	ON_BN_CLICKED(IDC_BUTTON_SET_DEFECT_ADJ, &CPreferenceDlg::OnBnClickedButtonSetDefectAdj)

	ON_BN_CLICKED(IDC_RADIO_VM_TRAIN_1, &CPreferenceDlg::OnBnClickedRadioVmTrain1)
	ON_BN_CLICKED(IDC_RADIO_VM_TRAIN_2, &CPreferenceDlg::OnBnClickedRadioVmTrain2)
	ON_BN_CLICKED(IDC_RADIO_VM_TRAIN_3, &CPreferenceDlg::OnBnClickedRadioVmTrain3)
END_MESSAGE_MAP()



BOOL CPreferenceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bnOK.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnCancel.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);

#ifndef INLINE_MODE
	GetDlgItem(IDC_CHECK_USE_CONT_DEFECT_ALARM)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_ADJ_TIMEOUT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_USE_GPU_AFFINE_TRANS)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_USE_VISION_COPY)->EnableWindow(TRUE);
#endif

	CString sVisionCamType;

	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_1];
	SetDlgItemText(IDC_CHECK_USE_VISION1, sVisionCamType);
	SetDlgItemText(IDC_STATIC_PREFERENCEDLG_29, sVisionCamType);
	if (sVisionCamType == "Undefine" || sVisionCamType == "Align")
	{
		GetDlgItem(IDC_CHECK_USE_VISION1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION1_LIGHT_IP1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION1_LIGHT_IP2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION1_LIGHT_IP3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION1_LIGHT_IP4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION1_LIGHT_PORT)->EnableWindow(FALSE);
	}

	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_2];
	SetDlgItemText(IDC_CHECK_USE_VISION2, sVisionCamType);
	SetDlgItemText(IDC_STATIC_PREFERENCEDLG_30, sVisionCamType);
	if (sVisionCamType == "Undefine" || sVisionCamType == "Align")
	{
		GetDlgItem(IDC_CHECK_USE_VISION2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION2_LIGHT_IP1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION2_LIGHT_IP2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION2_LIGHT_IP3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION2_LIGHT_IP4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION2_LIGHT_PORT)->EnableWindow(FALSE);
	}

	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_3];
	SetDlgItemText(IDC_CHECK_USE_VISION3, sVisionCamType);
	SetDlgItemText(IDC_STATIC_PREFERENCEDLG_31, sVisionCamType);
	if (sVisionCamType == "Undefine" || sVisionCamType == "Align")
	{
		GetDlgItem(IDC_CHECK_USE_VISION3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION3_LIGHT_IP1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION3_LIGHT_IP2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION3_LIGHT_IP3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION3_LIGHT_IP4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION3_LIGHT_PORT)->EnableWindow(FALSE);
	}

	sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_4];
	SetDlgItemText(IDC_CHECK_USE_VISION4, sVisionCamType);
	SetDlgItemText(IDC_STATIC_PREFERENCEDLG_32, sVisionCamType);
	if (sVisionCamType == "Undefine" || sVisionCamType == "Align")
	{
		GetDlgItem(IDC_CHECK_USE_VISION4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION4_LIGHT_IP1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION4_LIGHT_IP2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION4_LIGHT_IP3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION4_LIGHT_IP4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_VISION4_LIGHT_PORT)->EnableWindow(FALSE);
	}

	ChangeLanguage();

	if (m_iRadioVmTrain > VM_TRAIN_NOUSE)
	{
		GetDlgItem(IDC_CHECK_USE_VM_THRES_OPT)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_USE_VM_THRES_OPT))->SetCheck(BST_UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


// CPreferenceDlg 메시지 처리기입니다.
void CPreferenceDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CDialog::OnOK();
}


void CPreferenceDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}


void CPreferenceDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("설정"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_1, _T("설비 번호:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_2, _T("설비 모델:"));
		SetDlgItemText(IDC_BUTTON_SELECT_INSPECTION_MODEL_TYPE, _T("설비 검사 모델 선택"));
		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_1, _T("검사 PC 선택"));
		SetDlgItemText(IDC_RADIO_INSPECTION_PC1, _T("PC 1번"));
		SetDlgItemText(IDC_RADIO_INSPECTION_PC2, _T("PC 2번"));
		SetDlgItemText(IDC_RADIO_INSPECTION_PC3, _T("PC 3번"));
		SetDlgItemText(IDC_RADIO_INSPECTION_PC4, _T("PC 4번"));
		SetDlgItemText(IDC_RADIO_INSPECTION_PC5, _T("PC 5번"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_2, _T("검사 비전 선택"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_3, _T("저장 경로"));
		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_4, _T("LAS 저장 경로 설정"));
		SetDlgItemText(IDC_CHECK_USE_LAS1, _T("LAS 1 사용"));
		SetDlgItemText(IDC_CHECK_USE_LAS2, _T("LAS 2 사용"));
		SetDlgItemText(IDC_CHECK_USE_LAS3, _T("LAS 3 사용"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_LAS1, _T("폴더 찾기"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_LAS2, _T("폴더 찾기"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_LAS3, _T("폴더 찾기"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_3, _T("현재 사용중인 LAS:"));

		SetDlgItemText(IDC_CHECK_USE_LAS_CONNECTION_CHECK, _T("LAS 업로드 연결 체크 사용"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_5, _T("로컬 저장 경로 설정"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_4, _T("경로"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_5, _T("로컬"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_6, _T("LAS"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_7, _T("쓰레드"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_8, _T("임시"));

		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_9, _T("원본 영상"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_RAW, _T("폴더 찾기"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_10, _T("결과 영상"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_RESULT, _T("폴더 찾기"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_11, _T("리뷰 영상"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_REVIEW, _T("폴더 찾기"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_12, _T("ADJ 영상"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_ADJ, _T("폴더 찾기"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_13, _T("결과 로그"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_RESULTLOG, _T("폴더 찾기"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_14, _T("기타"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_ETC, _T("폴더 찾기"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_6, _T("기타 저장 옵션"));
		SetDlgItemText(IDC_CHECK_SAVE_RAW_IMAGE, _T("원본 영상 저장"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_15, _T("포맷:"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_BMP, _T("BMP"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_JPG, _T("JPG"));

		SetDlgItemText(IDC_CHECK_REDUCE_RAW_IMAGE, _T("원본 영상 축소 사용"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_16, _T("축소율:"));
		SetDlgItemText(IDC_CHECK_COMBINE_RAW_IMAGE, _T("원본 영상 붙여서 저장 사용"));

		SetDlgItemText(IDC_CHECK_SAVE_GOOD_RESULT_IMAGE, _T("양품 결과 영상 저장"));
		SetDlgItemText(IDC_CHECK_USE_THREAD_ONLY_GOOD_RESULT_IMAGE, _T("양품 결과 영상만 쓰레드 사용"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_17, _T("이미지 로그 원본 영상 단위"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_SAVE_LOG_MODULE, _T("모듈"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_SAVE_LOG_IMAGE, _T("영상"));

		SetDlgItemText(IDC_AVOID_DUPLICATE_LOT_LOCAL, _T("로컬 중복 랏 방지(Duplicate_hhmmss_LotID)"));
		SetDlgItemText(IDC_AVOID_DUPLICATE_LOT_LAS, _T("LAS 중복 랏 방지(Duplicate_hhmmss_LotID)"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_7, _T("원본 영상 저장 쓰레드 형식"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_THREAD_TYPE_1, _T("PC"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_THREAD_TYPE_2, _T("카메라"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_THREAD_TYPE_3, _T("검사"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_THREAD_TYPE_4, _T("예비"));
		SetDlgItemText(IDC_CHECK_USE_MULTI_QUEUE, _T("다중 쓰레드 리스트 사용"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_18, _T("리스트 수:"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_8, _T("결과 영상"));
		SetDlgItemText(IDC_BUTTON_SET_RESULT_IMAGE, _T("결과 영상 번호 설정"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_9, _T("불량 우선 순위"));
		SetDlgItemText(IDC_BUTTON_SET_PRIORITY, _T("불량 우선 순위 및 표시 색상 설정"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_10, _T("결과 영상 불량 표시 형식"));
		SetDlgItemText(IDC_RADIO_RESULT_DEFECT_DISPLAY_SIZE, _T("길이"));
		SetDlgItemText(IDC_RADIO_RESULT_DEFECT_DISPLAY_AREA, _T("면적"));
		SetDlgItemText(IDC_CHECK_USE_RESULT_IMAGE_DEFECT_SIZE_LIMIT, _T("결과 영상 측정값 표시 갯수 제한 사용"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_11, _T("ADJ 설정"));
		SetDlgItemText(IDC_CHECK_SAVE_ADJ, _T("ADJ 영상 저장"));
		SetDlgItemText(IDC_CHECK_USE_ADJ_CONNECTION_CHECK, _T("ADJ 연결 체크 사용"));
		SetDlgItemText(IDC_CHECK_USE_LOCAL_ADJ, _T("로컬 시냅스 AI(pyADJ) 사용"));
		SetDlgItemText(IDC_CHECK_USE_LOCAL_SEG, _T("로컬 LGIT AI(Segmentation) 사용"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_19, _T("설비 IP 주소"));
		SetDlgItemText(IDC_CHECK_USE_ADJ1, _T("ADJ 1 사용"));
		SetDlgItemText(IDC_CHECK_USE_ADJ2, _T("ADJ 2 사용"));

#ifdef USE_SUAKIT
		for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
		{
			CString strConnectionCheck;
			strConnectionCheck.Empty();
			if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[i])
			{
				for (int j = THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO - 1; j >= 0; j--)
				{
					if (!THEAPP.m_TCPClientService.m_bConnect[i][j])
					{
						if (strConnectionCheck.IsEmpty())
							strConnectionCheck.Format("#%d", j + 1);
						else
							strConnectionCheck.Format("#%d,%s", j + 1, strConnectionCheck);
					}
				}
				if (strConnectionCheck.IsEmpty())
					SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1 + (4 * i), "연결 중");
				else
					SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1 + (4 * i), strConnectionCheck + _T(" 실패"));
			}
			else
			{
				SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1 + (4 * i), "사용 안함");
			}
		}
#endif

		SetDlgItemText(IDC_CHECK_SEND_ADJ, _T("ADJ 서버로 이미지 전송"));
		SetDlgItemText(IDC_CHECK_USE_ADJ_MULTIIMAGE, _T("멀티 이미지 사용"));
		SetDlgItemText(IDC_CHECK_APPLY_ADJ, _T("ADJ 판정 결과 적용"));
		SetDlgItemText(IDC_BUTTON_SET_DEFECT_ADJ, _T("ADJ로 판정할 불량 선택"));
		SetDlgItemText(IDC_CHECK_ADJ_TIMEOUT, _T("ADJ 검사 시간 타임아웃 사용 (ms)"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_20, _T("멀티통신 사용 개수:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_21, _T("ADJ 영상 크기 (픽셀) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_22, _T("ADJ 영상 리사이즈 크기(픽셀) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_23, _T("불량 영역 표시 크기 (픽셀) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_24, _T("ADJ 영상 생성 시 통합할 불량 간격 (픽셀) :"));
		SetDlgItemText(IDC_CHECK_SAVE_ROI_IMAGE, _T("ROI 영상 저장 / 저장 ROI 선택"));
		SetDlgItemText(IDC_RADIO_SAVE_ROI_IMAGE_TYPE_1, _T("전체 ROI"));
		SetDlgItemText(IDC_RADIO_SAVE_ROI_IMAGE_TYPE_2, _T("검출 ROI"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_12, _T("MDJ 설정"));
		SetDlgItemText(IDC_CHECK_SAVE_MDJ, _T("리뷰 영상 저장"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_25, _T("검사 영상 당 최대 리뷰 영상 저장 수량:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_26, _T("리뷰 영상 생성 시 통합할 불량 간격 (픽셀) :"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_13, _T("리뷰 영상 불량 표시 형식"));
		SetDlgItemText(IDC_RADIO_REVIEWIMAGE_DISPLAY1, _T("원"));
		SetDlgItemText(IDC_RADIO_REVIEWIMAGE_DISPLAY2, _T("사각형"));
		SetDlgItemText(IDC_RADIO_REVIEWIMAGE_DISPLAY3, _T("불량 형상"));
		SetDlgItemText(IDC_CHECK_USE_SAVE_FAI_REVIEW_IMAGE, _T("검사 설정된 FAI 항목 양/불 상관없이 리뷰 영상 생성"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_14, _T("비전 조명 컨트롤러 설정"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_27, _T("IP 주소"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_28, _T("포트"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_29, _T("비전 1:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_30, _T("비전 2:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_31, _T("비전 3:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_32, _T("비전 4:"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_15, _T("카메라 그랩"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_33, _T("최대 그랩 시도 횟수"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_34, _T("그랩 시퀀스 전환 대기 시간 (ms) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_35, _T("트리거 대기 시간 (ms) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_36, _T("카메라 그랩 완료 대기 시간 (ms) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_37, _T("티칭 라이브 그랩 주기 (ms) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_38, _T("조명 이상 밝기 임계값 (GV) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_39, _T("티칭 모드 그랩 형식:"));
		SetDlgItemText(IDC_RADIO_CAMERA_LIGHT_PAGE, _T("영상 탭 페이지"));
		SetDlgItemText(IDC_RADIO_CAMERA_LIGHT_SEQ, _T("시퀀스 주소"));
		SetDlgItemText(IDC_CHECK_USE_GRAB_HOLD, _T("검사 버퍼의 검사가 완료되지 않은 경우 스캔 완료 명령 대기"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_40, _T("최대 포커스 이동 시도 횟수:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_41, _T("포커스 이동 완료 대기 시간 (ms) :"));
		SetDlgItemText(IDC_CHECK_USE_4D_SAVE_RAW_IMAGE, _T("4D 비전 원본 영상 저장"));
		SetDlgItemText(IDC_CHECK_USE_AUTO_FOCUS, _T("오토 포커스 모드 적용"));
		SetDlgItemText(IDC_CHECK_USE_FAST_GRAB, _T("Fast 그랩 모드 적용"));
		SetDlgItemText(IDC_CHECK_SEND_GRAB_FAIL_ERROR, _T("동일 랏에서 N개 GF 발생시 알람 발생 사용"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_16, _T("기타 설정"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_42, _T("디스플레이 설정 마진 (픽셀) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_43, _T("바코드 인식 마진 (픽셀) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_44, _T("바코드 미인식 영상 저장 포맷:"));
		SetDlgItemText(IDC_RADIO_BARCODE_NOREAD_IMAGE_BMP, _T("BMP"));
		SetDlgItemText(IDC_RADIO_BARCODE_NOREAD_IMAGE_JPG, _T("JPG"));
		SetDlgItemText(IDC_CHECK_USE_SAVE_DEFECT_SHAPE_FEATURE, _T("불량 블롭의 Defect shape feature 로그 저장"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_45, _T("동일 불량명의 Defect shape feature 최대 저장 갯수:"));
		SetDlgItemText(IDC_CHECK_USE_SAVE_GV_EDGE_MORNITORING_LOG, _T("밝기값 / 엣지 모니터링 로그 저장"));
		SetDlgItemText(IDC_CHECK_USE_GPU_AFFINE_TRANS, _T("GPU 연산으로 Global Align 이미지 변환 사용"));
		SetDlgItemText(IDC_CHECK_USE_DELETE_CROSS_BAR, _T("티칭 모드에서 검사 모드 전환 시 십자선 ROI 모두 삭제 사용"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_46, _T("저장할 스캔, 검사 완료된 모듈 정보의 수"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_17, _T("연속 불량 알람 기능"));
		SetDlgItemText(IDC_CHECK_USE_CONT_DEFECT_ALARM, _T("동일 위치 연속으로 불량 발생 시 알람 기능 사용"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_47, _T("동일 위치로 판단할 불량간 최소 거리 (um) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_48, _T("연속 불량 수:"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_18, _T("ROI 자동생성 딥러닝 서버 설정"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_49, _T("IP 번호:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_50, _T("포트 번호:"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_19, _T("비전간 ROI 복사 모드"));
		SetDlgItemText(IDC_CHECK_USE_VISION_COPY, _T("복사 사용"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_20, _T("오프라인 테스트 영상 폴더 경로"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_DEBUG, _T("폴더 찾기"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_21, _T("영상비교 모델 자동학습"));
		SetDlgItemText(IDC_RADIO_VM_TRAIN_1, _T("사용 안함"));
		SetDlgItemText(IDC_RADIO_VM_TRAIN_2, _T("전체 영상비교"));
		SetDlgItemText(IDC_RADIO_VM_TRAIN_3, _T("자동학습 옵션 체크된 영상비교"));
		SetDlgItemText(IDC_CHECK_VM_TRAIN_ALL, _T("검사결과에 상관없이 학습 (미체크 시 양품만 학습)"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_22, _T("영상비교 모델 Threshold 파라미터 최적화"));
		SetDlgItemText(IDC_CHECK_USE_VM_THRES_OPT, _T("파라미터 최적화 사용"));

		SetDlgItemText(IDOK, _T("저장"));
		SetDlgItemText(IDCANCEL, _T("닫기"));

		SetDlgItemText(IDC_BUTTON_LANGUAGE_CHANGE, _T("Language Change"));
	}
	else
	{
		this->SetWindowText(_T("Preference"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_1, _T("Equip No.:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_2, _T("Equip model:"));
		SetDlgItemText(IDC_BUTTON_SELECT_INSPECTION_MODEL_TYPE, _T("Select equip model"));
		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_1, _T("Select inspection PC No."));
		SetDlgItemText(IDC_RADIO_INSPECTION_PC1, _T("PC 1"));
		SetDlgItemText(IDC_RADIO_INSPECTION_PC2, _T("PC 2"));
		SetDlgItemText(IDC_RADIO_INSPECTION_PC3, _T("PC 3"));
		SetDlgItemText(IDC_RADIO_INSPECTION_PC4, _T("PC 4"));
		SetDlgItemText(IDC_RADIO_INSPECTION_PC5, _T("PC 5"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_2, _T("Select inspection Visions"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_3, _T("Save path"));
		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_4, _T("Set LAS save path"));
		SetDlgItemText(IDC_CHECK_USE_LAS1, _T("Use LAS 1"));
		SetDlgItemText(IDC_CHECK_USE_LAS2, _T("Use LAS 2"));
		SetDlgItemText(IDC_CHECK_USE_LAS3, _T("Use LAS 3"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_LAS1, _T("Folder"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_LAS2, _T("Folder"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_LAS3, _T("Folder"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_3, _T("Current LAS in use:"));

		SetDlgItemText(IDC_CHECK_USE_LAS_CONNECTION_CHECK, _T("Check to LAS uploader connection fails"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_5, _T("Set local save path"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_4, _T("Path"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_5, _T("Local"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_6, _T("LAS"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_7, _T("Thread"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_8, _T("Temp"));

		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_9, _T("Raw image"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_RAW, _T("Folder"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_10, _T("Result image"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_RESULT, _T("Folder"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_11, _T("Review image"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_REVIEW, _T("Folder"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_12, _T("ADJ image"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_ADJ, _T("Folder"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_13, _T("Result log"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_RESULTLOG, _T("Folder"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_14, _T("ETC"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_ETC, _T("Folder"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_6, _T("Save option(ETC)"));
		SetDlgItemText(IDC_CHECK_SAVE_RAW_IMAGE, _T("Save raw image"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_15, _T("format:"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_BMP, _T("BMP"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_JPG, _T("JPG"));

		SetDlgItemText(IDC_CHECK_REDUCE_RAW_IMAGE, _T("Reduce raw image"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_16, _T("Ratio:"));
		SetDlgItemText(IDC_CHECK_COMBINE_RAW_IMAGE, _T("Save after the original image combine"));

		SetDlgItemText(IDC_CHECK_SAVE_GOOD_RESULT_IMAGE, _T("Save good result image"));
		SetDlgItemText(IDC_CHECK_USE_THREAD_ONLY_GOOD_RESULT_IMAGE, _T("Use thread only good result image"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_17, _T("Raw image unit for image log"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_SAVE_LOG_MODULE, _T("Module"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_SAVE_LOG_IMAGE, _T("Image"));

		SetDlgItemText(IDC_AVOID_DUPLICATE_LOT_LOCAL, _T("Prevent duplicate for Local Lot(Duplicate_hhmmss_LotID)"));
		SetDlgItemText(IDC_AVOID_DUPLICATE_LOT_LAS, _T("Prevent duplicate for LAS Lot(Duplicate_hhmmss_LotID)"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_7, _T("Raw image save thread type"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_THREAD_TYPE_1, _T("PC"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_THREAD_TYPE_2, _T("Capera"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_THREAD_TYPE_3, _T("Inspection"));
		SetDlgItemText(IDC_RADIO_RAW_IMAGE_THREAD_TYPE_4, _T("Temp"));
		SetDlgItemText(IDC_CHECK_USE_MULTI_QUEUE, _T("Use multi queue"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_18, _T("Lists Qty:"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_8, _T("Result image"));
		SetDlgItemText(IDC_BUTTON_SET_RESULT_IMAGE, _T("Set result image number"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_9, _T("Defect priority"));
		SetDlgItemText(IDC_BUTTON_SET_PRIORITY, _T("Set defect priority and display color"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_10, _T("Result image defect display type"));
		SetDlgItemText(IDC_RADIO_RESULT_DEFECT_DISPLAY_SIZE, _T("Size"));
		SetDlgItemText(IDC_RADIO_RESULT_DEFECT_DISPLAY_AREA, _T("Area"));
		SetDlgItemText(IDC_CHECK_USE_RESULT_IMAGE_DEFECT_SIZE_LIMIT, _T("Use the limit Qty of defects in result image"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_11, _T("Set ADJ"));
		SetDlgItemText(IDC_CHECK_SAVE_ADJ, _T("Save ADJ image"));
		SetDlgItemText(IDC_CHECK_USE_ADJ_CONNECTION_CHECK, _T("Use ADJ connection check"));
		SetDlgItemText(IDC_CHECK_USE_LOCAL_ADJ, _T("Use local synapse AI(pyADJ)"));
		SetDlgItemText(IDC_CHECK_USE_LOCAL_SEG, _T("Use local LGIT AI(Segmentation)"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_19, _T("AVI IP adress"));
		SetDlgItemText(IDC_CHECK_USE_ADJ1, _T("Use ADJ1"));
		SetDlgItemText(IDC_CHECK_USE_ADJ2, _T("Use ADJ2"));

#ifdef USE_SUAKIT
		for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
		{
			CString strConnectionCheck;
			strConnectionCheck.Empty();
			if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[i])
			{
				for (int j = THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO - 1; j >= 0; j--)
				{
					if (!THEAPP.m_TCPClientService.m_bConnect[i][j])
					{
						if (strConnectionCheck.IsEmpty())
							strConnectionCheck.Format("#%d", j + 1);
						else
							strConnectionCheck.Format("#%d,%s", j + 1, strConnectionCheck);
					}
				}
				if (strConnectionCheck.IsEmpty())
					SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1 + (4 * i), "Connect");
				else
					SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1 + (4 * i), strConnectionCheck + _T(" fail"));
			}
			else
			{
				SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1 + (4 * i), "Unused");
			}
		}
#endif

		SetDlgItemText(IDC_CHECK_SEND_ADJ, _T("Send ADJ Image"));
		SetDlgItemText(IDC_CHECK_USE_ADJ_MULTIIMAGE, _T("Use ADJ multi image"));
		SetDlgItemText(IDC_CHECK_APPLY_ADJ, _T("Apply ADJ judgement"));
		SetDlgItemText(IDC_BUTTON_SET_DEFECT_ADJ, _T("Select defects to be judged"));
		SetDlgItemText(IDC_CHECK_ADJ_TIMEOUT, _T("Use ADJ judge timeout"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_20, _T("Number of multi network"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_21, _T("ADJ image size (Pixel) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_22, _T("ADJ image resize (Pixel) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_23, _T("Marking circle size (Pixel) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_24, _T("Union distance each blob for ADJ image (Pixel) :"));
		SetDlgItemText(IDC_CHECK_SAVE_ROI_IMAGE, _T("Save ROI image / Save option"));
		SetDlgItemText(IDC_RADIO_SAVE_ROI_IMAGE_TYPE_1, _T("All ROI"));
		SetDlgItemText(IDC_RADIO_SAVE_ROI_IMAGE_TYPE_2, _T("Detected ROI"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_12, _T("Set MDJ"));
		SetDlgItemText(IDC_CHECK_SAVE_MDJ, _T("Save review image"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_25, _T("Max Qty of review images per raw image:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_26, _T("Union distance each blob for review image (Pixel) :"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_13, _T("Review image defect display type"));
		SetDlgItemText(IDC_RADIO_REVIEWIMAGE_DISPLAY1, _T("Circle"));
		SetDlgItemText(IDC_RADIO_REVIEWIMAGE_DISPLAY2, _T("Rectangle"));
		SetDlgItemText(IDC_RADIO_REVIEWIMAGE_DISPLAY3, _T("Defect blob"));
		SetDlgItemText(IDC_CHECK_USE_SAVE_FAI_REVIEW_IMAGE, _T("Save all FAI review images"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_14, _T("Vision light controller set"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_27, _T("IP adress"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_28, _T("Port"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_29, _T("VIsion 1:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_30, _T("Vision 2:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_31, _T("Vision 3:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_32, _T("Vision 4:"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_15, _T("Camera grab"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_33, _T("Max Qty of grab retry"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_34, _T("Wait time between grab sequence (ms) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_35, _T("Trigger sleep time (ms) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_36, _T("Camera grab done wait time (ms) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_37, _T("Live grab period in teaching mode (ms) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_38, _T("Threshold of light error (GV) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_39, _T("Grab image type:"));
		SetDlgItemText(IDC_RADIO_CAMERA_LIGHT_PAGE, _T("Image tab page"));
		SetDlgItemText(IDC_RADIO_CAMERA_LIGHT_SEQ, _T("SEQ adress"));
		SetDlgItemText(IDC_CHECK_USE_GRAB_HOLD, _T("Hold the scan complete if inspection is not done"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_40, _T("Max Qty of focus move attempts:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_41, _T("Focus move wait time (ms) :"));
		SetDlgItemText(IDC_CHECK_USE_4D_SAVE_RAW_IMAGE, _T("Save raw images of 4D vision"));
		SetDlgItemText(IDC_CHECK_USE_AUTO_FOCUS, _T("Use Aotu Focus mode"));
		SetDlgItemText(IDC_CHECK_USE_FAST_GRAB, _T("Use Fast grab mode"));
		SetDlgItemText(IDC_CHECK_SEND_GRAB_FAIL_ERROR, _T("Send Handler alram for n-th grab fail each lot"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_16, _T("Other setting"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_42, _T("Display settings margin (Pixel) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_43, _T("Barcode read margin (Pixel) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_44, _T("Barcode noread image save format:"));
		SetDlgItemText(IDC_RADIO_BARCODE_NOREAD_IMAGE_BMP, _T("BMP"));
		SetDlgItemText(IDC_RADIO_BARCODE_NOREAD_IMAGE_JPG, _T("JPG"));
		SetDlgItemText(IDC_CHECK_USE_SAVE_DEFECT_SHAPE_FEATURE, _T("Save Defect shape feature log for defect blobs"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_45, _T("Max Qty of Defect shape feature log for same defect name:"));
		SetDlgItemText(IDC_CHECK_USE_SAVE_GV_EDGE_MORNITORING_LOG, _T("Save GV / Edge monitoring log"));
		SetDlgItemText(IDC_CHECK_USE_GPU_AFFINE_TRANS, _T("Global align's image transform using GPU"));
		SetDlgItemText(IDC_CHECK_USE_DELETE_CROSS_BAR, _T("Delete the Cross bar ROI when inspection mode"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_46, _T("Qty of Complete info to be saved"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_17, _T("Continuous defect alarm"));
		SetDlgItemText(IDC_CHECK_USE_CONT_DEFECT_ALARM, _T("Sent error message to handler if continuous defect occurs"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_47, _T("Min distance for check same location (um) :"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_48, _T("Continuous defects Qty:"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_18, _T("ROI auto generate server set"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_49, _T("IP adress:"));
		SetDlgItemText(IDC_STATIC_PREFERENCEDLG_50, _T("Port number:"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_19, _T("ROI copy mode between visions"));
		SetDlgItemText(IDC_CHECK_USE_VISION_COPY, _T("Use copy"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_20, _T("Offline test image folder path"));
		SetDlgItemText(IDC_BUTTON_FINDFOLDER_DEBUG, _T("Folder"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_21, _T("Image compare model auto train"));
		SetDlgItemText(IDC_RADIO_VM_TRAIN_1, _T("No use"));
		SetDlgItemText(IDC_RADIO_VM_TRAIN_2, _T("All image compare"));
		SetDlgItemText(IDC_RADIO_VM_TRAIN_3, _T("In case of auto train option"));
		SetDlgItemText(IDC_CHECK_VM_TRAIN_ALL, _T("Train regardless of inspection results (only good if not checked)"));

		SetDlgItemText(IDC_GROUPBOX_PREFERENCEDLG_22, _T("Image compare model threshold parameter optimization"));
		SetDlgItemText(IDC_CHECK_USE_VM_THRES_OPT, _T("Use parameter optimization"));

		SetDlgItemText(IDOK, _T("Save"));
		SetDlgItemText(IDCANCEL, _T("Close"));

		SetDlgItemText(IDC_BUTTON_LANGUAGE_CHANGE, _T("언어 변경"));
	}

	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedButtonFindfolderDebug()
{
	CString strInitPath = m_strOfflineTestFolderPath;
	CString strFolderPath = _T("");

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("오프라인 테스트 영상 폴더 경로");
	else
		strDialog.Format("Offline test image folder path");
		
	strFolderPath = BrowseFolderHelper(m_hWnd, strInitPath, _T(strDialog));
	if (strFolderPath.IsEmpty())
	{
		// AfxMessageBox(_T("폴더 경로를 찾지 못했습니다."), MB_OK | MB_ICONWARNING);
	}
	else
	{
		m_strOfflineTestFolderPath = strFolderPath;
		UpdateData(FALSE);
	}

	UpdateData(FALSE);
}

#include "InspectionPriorityDlg.h"

void CPreferenceDlg::OnBnClickedButtonSetDefectPriority()
{
	CInspectionPriorityDlg dlg;
	dlg.DoModal();
}

#include "SetResultImageDlg.h"

void CPreferenceDlg::OnBnClickedButtonSetResultImage()
{
	CSetResultImageDlg dlg;
	dlg.DoModal();
}

void CPreferenceDlg::OnBnClickedButtonAdjconnect()
{
#ifdef USE_SUAKIT
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("ADJ #1 IP주소를 변경하시려면 IP주소 변경 후 저장을 먼저 한 뒤 다시 연결을 해주세요.");
	else
		strMessageBox.Format("To change the ADJ #1 IP address, please change the IP address, save it first, and then reconnect.");
	AfxMessageBox(strMessageBox);
	int iClientNO = 0;

	CString m_strFailedConnect = "";

	if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
	{
		int m_bIsConnectCheck = 0;
		for (int i = 0; i < MAX_MULTI_CONNECTION_NUM; i++)
		{
			if (THEAPP.m_TCPClientService.m_bConnect[iClientNO][i])
			{
				m_bIsConnectCheck += 1;
			}
		}

		if (m_bIsConnectCheck == THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO)
		{
			for (int i = 0; i < MAX_MULTI_CONNECTION_NUM; i++)
			{
				if (THEAPP.m_TCPClientService.m_bConnect[iClientNO][i])
					THEAPP.m_TCPClientService.m_arrClient[iClientNO][i].TCPonClose();
				THEAPP.m_TCPClientService.m_bConnect[iClientNO][i] = FALSE;
			}
			THEAPP.m_TCPClientService.ReStart();
			THEAPP.m_TCPClientService.Initialize();

			strLog.Format("ADJ1 disconnect success");
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

			if (THEAPP.m_iPGMLanguageSelect == 0)
				strDialog.Format("연결 해제");
			else
				strDialog.Format("Disconnected");
			SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1, _T(strDialog));
		}
		else
		{
			THEAPP.m_TCPClientService.Initialize();

			for (int i = 0; i < MAX_MULTI_CONNECTION_NUM; i++)
			{
				if (THEAPP.m_TCPClientService.m_bConnect[iClientNO][i])
					THEAPP.m_TCPClientService.m_arrClient[iClientNO][i].TCPonClose();
				THEAPP.m_TCPClientService.m_bConnect[iClientNO][i] = FALSE;
			}

			for (int i = 0; i < THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO; i++)
			{
				if (!THEAPP.m_TCPClientService.m_bConnect[iClientNO][i])
				{
					if (THEAPP.m_TCPClientService.m_arrClient[iClientNO][i].TCPConnect())
					{
						THEAPP.m_TCPClientService.m_bConnect[iClientNO][i] = TRUE;
					}
					else
					{
						THEAPP.m_TCPClientService.m_bConnect[iClientNO][i] = FALSE;
					}
				}

			}

			for (int j = THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO - 1; j >= 0; j--)
			{
				if (!THEAPP.m_TCPClientService.m_bConnect[iClientNO][j])
				{
					if (m_strFailedConnect == "")
						m_strFailedConnect.Format("#%d", j + 1);
					else
						m_strFailedConnect.Format("#%d,%s", j + 1, m_strFailedConnect);
				}
			}

			if (m_strFailedConnect == "")
			{
				if (iClientNO == 0)
				{
					strLog.Format("ADJ1 connect succeses");
					THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

					if (THEAPP.m_iPGMLanguageSelect == 0)
						strDialog.Format("연결중");
					else
						strDialog.Format("Connected");
					SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1, _T(strDialog));
				}
				else
				{
					strLog.Format("ADJ2 connect succeses");
					THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

					if (THEAPP.m_iPGMLanguageSelect == 0)
						strDialog.Format("연결중");
					else
						strDialog.Format("Connected");
					SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ2, _T(strDialog));
				}
			}
			else
			{
				if (iClientNO == 0)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
					{
						strLog.Format("ADJ1 connect fail");
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

						if (THEAPP.m_iPGMLanguageSelect == 0)
							strDialog.Format("연결 실패");
						else
							strDialog.Format("Failed");
						SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1, strDialog);
					}
					else
					{
						strLog.Format("ADJ1 disconnect success");
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

						if (THEAPP.m_iPGMLanguageSelect == 0)
							strDialog.Format("연결 해제");
						else
							strDialog.Format("Disconnected");
						SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1, strDialog);
					}
				}
				else
				{
					if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
					{
						strLog.Format("ADJ2 connect fail");
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

						if (THEAPP.m_iPGMLanguageSelect == 0)
							strDialog.Format("연결 실패");
						else
							strDialog.Format("Failed");
						SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ2, strDialog);
					}
					else
					{
						strLog.Format("ADJ2 disconnect success");
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

						if (THEAPP.m_iPGMLanguageSelect == 0)
							strDialog.Format("연결 해제");
						else
							strDialog.Format("Disconnected");
						SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ2, strDialog);
					}
				}
			}
		}
		AfxMessageBox(strLog);
	}
	else
	{
		strLog.Format("ADJ1 not use");
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strDialog.Format("사용 안함");
		else
			strDialog.Format("Unsed");
		SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1, strDialog);

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("ADJ #1 사용 체크가 해제되어있습니다. 사용하시려면 체크해주세요.");
		else
			strMessageBox.Format("The ADJ #1 Enable option is unchecked. Please check it to enable use.");
		AfxMessageBox(strMessageBox);
	}
#endif
}

void CPreferenceDlg::OnBnClickedButtonAdjconnect2()
{
#ifdef USE_SUAKIT
	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("ADJ2 IP주소를 변경하시려면 IP주소 변경 후 저장을 먼저 한 뒤 다시 연결을 해주세요.");
	else
		strMessageBox.Format("To change the ADJ #2 IP address, please change the IP address, save it first, and then reconnect.");
	AfxMessageBox(strMessageBox);
	int iClientNO = 1;
	CString m_strFailedConnect = "";

	if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
	{
		int m_bIsConnectCheck = 0;
		for (int i = 0; i < MAX_MULTI_CONNECTION_NUM; i++)
		{
			if (THEAPP.m_TCPClientService.m_bConnect[iClientNO][i])
			{
				m_bIsConnectCheck += 1;
			}
		}

		if (m_bIsConnectCheck == THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO)
		{
			for (int i = 0; i < MAX_MULTI_CONNECTION_NUM; i++)
			{
				if (THEAPP.m_TCPClientService.m_bConnect[iClientNO][i])
					THEAPP.m_TCPClientService.m_arrClient[iClientNO][i].TCPonClose();
				THEAPP.m_TCPClientService.m_bConnect[iClientNO][i] = FALSE;
			}
			THEAPP.m_TCPClientService.ReStart();
			THEAPP.m_TCPClientService.Initialize();

			strLog.Format("ADJ2 disconnect success");
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

			if (THEAPP.m_iPGMLanguageSelect == 0)
				strDialog.Format("연결 해제");
			else
				strDialog.Format("Disconnected");
			SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ2, strDialog);
		}
		else
		{
			THEAPP.m_TCPClientService.Initialize();

			for (int i = 0; i < MAX_MULTI_CONNECTION_NUM; i++)
			{
				if (THEAPP.m_TCPClientService.m_bConnect[iClientNO][i])
					THEAPP.m_TCPClientService.m_arrClient[iClientNO][i].TCPonClose();
				THEAPP.m_TCPClientService.m_bConnect[iClientNO][i] = FALSE;
			}

			for (int i = 0; i < THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO; i++)
			{
				if (!THEAPP.m_TCPClientService.m_bConnect[iClientNO][i])
				{
					if (THEAPP.m_TCPClientService.m_arrClient[iClientNO][i].TCPConnect())
					{
						THEAPP.m_TCPClientService.m_bConnect[iClientNO][i] = TRUE;
					}
					else
					{
						THEAPP.m_TCPClientService.m_bConnect[iClientNO][i] = FALSE;
					}
				}
			}

			for (int j = THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO - 1; j >= 0; j--)
			{
				if (!THEAPP.m_TCPClientService.m_bConnect[iClientNO][j])
				{
					if (m_strFailedConnect == "")
						m_strFailedConnect.Format("#%d", j + 1);
					else
						m_strFailedConnect.Format("#%d,%s", j + 1, m_strFailedConnect);
				}
			}

			if (m_strFailedConnect == "")
			{
				if (iClientNO == 0)
				{
					strLog.Format("ADJ1 connect succeses");
					THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

					if (THEAPP.m_iPGMLanguageSelect == 0)
						strDialog.Format("연결중");
					else
						strDialog.Format("Connected");
					SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1, strDialog);
				}
				else
				{
					strLog.Format("ADJ2 connect succeses");
					THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

					if (THEAPP.m_iPGMLanguageSelect == 0)
						strDialog.Format("연결중");
					else
						strDialog.Format("Connected");
					SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ2, strDialog);
				}
			}
			else
			{
				if (iClientNO == 0)
				{
					if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
					{
						strLog.Format("ADJ1 connect fail");
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

						if (THEAPP.m_iPGMLanguageSelect == 0)
							strDialog.Format("연결 실패");
						else
							strDialog.Format("Failed");
						SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1, strDialog);
					}
					else
					{
						strLog.Format("ADJ1 disconnect success");
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

						if (THEAPP.m_iPGMLanguageSelect == 0)
							strDialog.Format("연결 해제");
						else
							strDialog.Format("Disconnected");
						SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ1, strDialog);
					}
				}
				else
				{
					if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
					{
						strLog.Format("ADJ2 connect fail");
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

						if (THEAPP.m_iPGMLanguageSelect == 0)
							strDialog.Format("연결 실패");
						else
							strDialog.Format("Failed");
						SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ2, strDialog);
					}
					else
					{
						strLog.Format("ADJ2 disconnect success");
						THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

						if (THEAPP.m_iPGMLanguageSelect == 0)
							strDialog.Format("연결 해제");
						else
							strDialog.Format("Disconnected");
						SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ2, strDialog);
					}
				}
			}
		}
		AfxMessageBox(strLog);
	}
	else
	{
		strLog.Format("ADJ2 not use");
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strDialog.Format("사용 안함");
		else
			strDialog.Format("Unsed");
		SetDlgItemTextA(IDC_BUTTON_CONNECT_ADJ2, strDialog);

		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessageBox.Format("ADJ #2 사용 체크가 해제되어있습니다. 사용하시려면 체크해주세요.");
		else
			strMessageBox.Format("The ADJ #2 Enable option is unchecked. Please check it to enable use.");
		AfxMessageBox(strMessageBox);
	}
#endif
}

#include "DefectAssignADJ.h"

void CPreferenceDlg::OnBnClickedButtonSetDefectAdj()
{
	CDefectAssignADJ dlg;
	dlg.DoModal();
}

void CPreferenceDlg::OnBnClickedCheckRawImageSaveLocal()
{
	UpdateData(TRUE);
	if (m_bCheckRawImageSaveLocal && m_bCheckRawImageSaveLAS)
		m_bCheckRawImageSaveLAS = FALSE;

	if (!m_bCheckRawImageSaveLocal && !m_bCheckRawImageSaveLAS)
		m_bCheckRawImageSaveLAS = TRUE;
	UpdateData(FALSE);
}

void CPreferenceDlg::OnBnClickedCheckRawImageSaveLas()
{
	UpdateData(TRUE);
	if (m_bCheckRawImageSaveLocal && m_bCheckRawImageSaveLAS)
		m_bCheckRawImageSaveLocal = FALSE;

	if (!m_bCheckRawImageSaveLocal && !m_bCheckRawImageSaveLAS)
		m_bCheckRawImageSaveLocal = TRUE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckUseRawImageThread()
{
	UpdateData(TRUE);
	if (!m_bCheckUseRawImageThread && m_bCheckUseRawImageTempDrive)
		m_bCheckUseRawImageTempDrive = FALSE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckResultImageSaveLocal()
{
	UpdateData(TRUE);
	if (!m_bCheckResultImageSaveLocal && !m_bCheckResultImageSaveLAS)
		m_bCheckResultImageSaveLAS = TRUE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckResultImageSaveLas()
{
	UpdateData(TRUE);
	if (!m_bCheckResultImageSaveLocal && !m_bCheckResultImageSaveLAS)
		m_bCheckResultImageSaveLocal = TRUE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckUseResultImageThread()
{
	UpdateData(TRUE);
	if (m_bCheckUseResultImageThreadOnlyGood)
		m_bCheckUseResultImageThreadOnlyGood = FALSE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckReviewImageSaveLocal()
{
	UpdateData(TRUE);
	if (!m_bCheckReviewImageSaveLocal && !m_bCheckReviewImageSaveLAS)
		m_bCheckReviewImageSaveLAS = TRUE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckReviewImageSaveLas()
{
	UpdateData(TRUE);
	if (!m_bCheckReviewImageSaveLocal && !m_bCheckReviewImageSaveLAS)
		m_bCheckReviewImageSaveLocal = TRUE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckAdjImageSaveLocal()
{
	UpdateData(TRUE);
	if (!m_bCheckADJImageSaveLocal && !m_bCheckADJImageSaveLAS)
		m_bCheckADJImageSaveLAS = TRUE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckAdjImageSaveLas()
{
	UpdateData(TRUE);
	if (!m_bCheckADJImageSaveLocal && !m_bCheckADJImageSaveLAS)
		m_bCheckADJImageSaveLocal = TRUE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckResultLogSaveLocal()
{
	UpdateData(TRUE);
	if (!m_bCheckResultLogSaveLocal && !m_bCheckResultLogSaveLAS)
		m_bCheckResultLogSaveLAS = TRUE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckResultLogSaveLas()
{
	UpdateData(TRUE);
	if (!m_bCheckResultLogSaveLocal && !m_bCheckResultLogSaveLAS)
		m_bCheckResultLogSaveLocal = TRUE;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedButtonFindfolderRaw()
{
	TCHAR m_szFolderDisplayName[MAX_PATH] = { 0 };
	TCHAR m_szFolderPath[MAX_PATH] = { 0 };
	LPITEMIDLIST m_pidlSelected = NULL;

	BROWSEINFOA m_bi;
	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.hwndOwner = m_hWnd;

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("원본 영상 저장 경로");
	else
		strDialog.Format("Raw image save path");
	m_bi.lpszTitle = _T(strDialog);
	m_bi.ulFlags = BIF_RETURNONLYFSDIRS;

	m_pidlSelected = ::SHBrowseForFolder(&m_bi);

	if (m_pidlSelected != NULL)
		::SHGetPathFromIDList(m_pidlSelected, m_szFolderPath);

	CString m_edt;
	m_edt = m_szFolderPath;

	m_strRawFolderPath = m_edt;

	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedButtonFindfolderResult()
{
	TCHAR m_szFolderDisplayName[MAX_PATH] = { 0 };
	TCHAR m_szFolderPath[MAX_PATH] = { 0 };
	LPITEMIDLIST m_pidlSelected = NULL;

	BROWSEINFOA m_bi;
	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.hwndOwner = m_hWnd;

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("결과 영상 저장 경로");
	else
		strDialog.Format("Result image save path");
	m_bi.lpszTitle = _T(strDialog);
	m_bi.ulFlags = BIF_RETURNONLYFSDIRS;

	m_pidlSelected = ::SHBrowseForFolder(&m_bi);

	if (m_pidlSelected != NULL)
		::SHGetPathFromIDList(m_pidlSelected, m_szFolderPath);

	CString m_edt;
	m_edt = m_szFolderPath;

	m_strResultFolderPath = m_edt;

	UpdateData(FALSE);
}

void CPreferenceDlg::OnBnClickedButtonFindfolderReview()
{
	TCHAR m_szFolderDisplayName[MAX_PATH] = { 0 };
	TCHAR m_szFolderPath[MAX_PATH] = { 0 };
	LPITEMIDLIST m_pidlSelected = NULL;

	BROWSEINFOA m_bi;
	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.hwndOwner = m_hWnd;

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("리뷰 영상 저장 경로");
	else
		strDialog.Format("Review image save path");
	m_bi.lpszTitle = _T(strDialog);
	m_bi.ulFlags = BIF_RETURNONLYFSDIRS;

	m_pidlSelected = ::SHBrowseForFolder(&m_bi);

	if (m_pidlSelected != NULL)
		::SHGetPathFromIDList(m_pidlSelected, m_szFolderPath);

	CString m_edt;
	m_edt = m_szFolderPath;

	m_strReviewFolderPath = m_edt;

	UpdateData(FALSE);
}

void CPreferenceDlg::OnBnClickedButtonFindfolderAdj()
{
	TCHAR m_szFolderDisplayName[MAX_PATH] = { 0 };
	TCHAR m_szFolderPath[MAX_PATH] = { 0 };
	LPITEMIDLIST m_pidlSelected = NULL;

	BROWSEINFOA m_bi;
	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.hwndOwner = m_hWnd;

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("ADJ 영상 저장 경로");
	else
		strDialog.Format("ADJ image save path");
	m_bi.lpszTitle = _T(strDialog);
	m_bi.ulFlags = BIF_RETURNONLYFSDIRS;

	m_pidlSelected = ::SHBrowseForFolder(&m_bi);

	if (m_pidlSelected != NULL)
		::SHGetPathFromIDList(m_pidlSelected, m_szFolderPath);

	CString m_edt;
	m_edt = m_szFolderPath;

	m_strAdjFolderPath = m_edt;

	UpdateData(FALSE);
}

void CPreferenceDlg::OnBnClickedButtonFindfolderResultlog()
{
	TCHAR m_szFolderDisplayName[MAX_PATH] = { 0 };
	TCHAR m_szFolderPath[MAX_PATH] = { 0 };
	LPITEMIDLIST m_pidlSelected = NULL;

	BROWSEINFOA m_bi;
	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.hwndOwner = m_hWnd;

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("결과 로그 저장 경로");
	else
		strDialog.Format("Result log save path");
	m_bi.lpszTitle = _T(strDialog);
	m_bi.ulFlags = BIF_RETURNONLYFSDIRS;

	m_pidlSelected = ::SHBrowseForFolder(&m_bi);

	if (m_pidlSelected != NULL)
		::SHGetPathFromIDList(m_pidlSelected, m_szFolderPath);

	CString m_edt;
	m_edt = m_szFolderPath;

	m_strResultLogFolderPath = m_edt;

	UpdateData(FALSE);
}

void CPreferenceDlg::OnBnClickedButtonFindfolderEtc()
{
	TCHAR m_szFolderDisplayName[MAX_PATH] = { 0 };
	TCHAR m_szFolderPath[MAX_PATH] = { 0 };
	LPITEMIDLIST m_pidlSelected = NULL;

	BROWSEINFOA m_bi;
	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.hwndOwner = m_hWnd;

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("기타 항목 저장 경로");
	else
		strDialog.Format("Etc save path");
	m_bi.lpszTitle = _T(strDialog);
	m_bi.ulFlags = BIF_RETURNONLYFSDIRS;

	m_pidlSelected = ::SHBrowseForFolder(&m_bi);

	if (m_pidlSelected != NULL)
		::SHGetPathFromIDList(m_pidlSelected, m_szFolderPath);

	CString m_edt;
	m_edt = m_szFolderPath;

	m_strEtcFolderPath = m_edt;

	UpdateData(FALSE);
}

void CPreferenceDlg::OnBnClickedButtonFindfolderLas1()
{
	TCHAR m_szFolderDisplayName[MAX_PATH] = { 0 };
	TCHAR m_szFolderPath[MAX_PATH] = { 0 };
	LPITEMIDLIST m_pidlSelected = NULL;

	BROWSEINFOA m_bi;
	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.hwndOwner = m_hWnd;

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("LAS 1 드라이브 경로");
	else
		strDialog.Format("LAS 1 drive path");
	m_bi.lpszTitle = _T(strDialog);
	m_bi.ulFlags = BIF_RETURNONLYFSDIRS;

	m_pidlSelected = ::SHBrowseForFolder(&m_bi);

	if (m_pidlSelected != NULL)
		::SHGetPathFromIDList(m_pidlSelected, m_szFolderPath);

	CString m_edt;
	m_edt = m_szFolderPath;

	m_strSaveFolderPathLAS1 = m_edt;

	if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1)
	{
		for (int i = 0; i < MAX_MAGAZINE_NO; i++)
			THEAPP.ChangeLAS(i);
	}

	m_bCheckUseLAS1 = 1;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedButtonFindfolderLas2()
{
	TCHAR m_szFolderDisplayName[MAX_PATH] = { 0 };
	TCHAR m_szFolderPath[MAX_PATH] = { 0 };
	LPITEMIDLIST m_pidlSelected = NULL;

	BROWSEINFOA m_bi;
	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.hwndOwner = m_hWnd;

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("LAS 2 드라이브 경로");
	else
		strDialog.Format("LAS 2 drive path");
	m_bi.lpszTitle = _T(strDialog);
	m_bi.ulFlags = BIF_RETURNONLYFSDIRS;

	m_pidlSelected = ::SHBrowseForFolder(&m_bi);

	if (m_pidlSelected != NULL)
		::SHGetPathFromIDList(m_pidlSelected, m_szFolderPath);

	CString m_edt;
	m_edt = m_szFolderPath;

	m_strSaveFolderPathLAS2 = m_edt;

	if (THEAPP.Struct_PreferenceStruct.m_bUseLAS2)
	{
		for (int i = 0; i < MAX_MAGAZINE_NO; i++)
			THEAPP.ChangeLAS(i);
	}

	m_bCheckUseLAS2 = 1;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedButtonFindfolderLas3()
{
	TCHAR m_szFolderDisplayName[MAX_PATH] = { 0 };
	TCHAR m_szFolderPath[MAX_PATH] = { 0 };
	LPITEMIDLIST m_pidlSelected = NULL;

	BROWSEINFOA m_bi;
	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.hwndOwner = m_hWnd;

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strDialog.Format("LAS 3 드라이브 경로");
	else
		strDialog.Format("LAS 3 drive path");
	m_bi.lpszTitle = _T(strDialog);
	m_bi.ulFlags = BIF_RETURNONLYFSDIRS;

	m_pidlSelected = ::SHBrowseForFolder(&m_bi);

	if (m_pidlSelected != NULL)
		::SHGetPathFromIDList(m_pidlSelected, m_szFolderPath);

	CString m_edt;
	m_edt = m_szFolderPath;

	m_strSaveFolderPathLAS3 = m_edt;

	if (THEAPP.Struct_PreferenceStruct.m_bUseLAS3)
	{
		for (int i = 0; i < MAX_MAGAZINE_NO; i++)
			THEAPP.ChangeLAS(i);
	}

	m_bCheckUseLAS3 = 1;
	UpdateData(FALSE);
}


void CPreferenceDlg::OnBnClickedCheckUseResultImageThreadOnlyGood()
{
	UpdateData(TRUE);
	if (m_bCheckUseResultImageThread)
		m_bCheckUseResultImageThread = FALSE;
	UpdateData(FALSE);
}


#include "uScanView.h"
void CPreferenceDlg::OnBnClickedButtonLanguageChange()
{
	CPGMLanguageSelectDlg PGMLanguageSelectDlg;
	PGMLanguageSelectDlg.m_bCheckPGMLanguageDialogHide = THEAPP.m_bPGMLanguageDialogHide;
	if (PGMLanguageSelectDlg.DoModal() != IDOK)
		return;

	ChangeLanguage();

	CuScanView* puScanView = (CuScanView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();
	puScanView->ChangeLanguage();

	THEAPP.m_pInspectSummary->ChangeLanguage();
	THEAPP.m_pInspectResultDlg->ChangeLanguage();
	//THEAPP.m_pDefectListDlg->ChangeLanguage();

#ifdef INLINE_MODE
	if (THEAPP.Struct_PreferenceStruct.m_bUseLocalADJ)
		THEAPP.ConnectStatusPyADJ();

	if (THEAPP.Struct_PreferenceStruct.m_bUseLASConnectionCheck)
		THEAPP.ConnectStatusLASAggregator();
#endif

	//THEAPP.m_pInspectViewOverayImageDlg1->ChangeLanguage();

	THEAPP.m_pInspectAdminViewDlg->ChangeLanguage();

	THEAPP.m_pTabControlDlg->ChangeLanguage();
	THEAPP.m_pTabControlDlg->ResetTabPosition();
	THEAPP.m_pTabControlDlg->m_pThresholdHistogramDlg->ChangeLanguage();
	THEAPP.m_pTabControlDlg->m_pLightControlDlg->ChangeLanguage();
	THEAPP.m_pTabControlDlg->m_pJogSetDlg->ChangeLanguage();
	THEAPP.m_pTabControlDlg->m_pCameraLightManagerDlg->ChangeLanguage();
	THEAPP.m_pTabControlDlg->m_pMotionControlDlg->ChangeLanguage();
	THEAPP.m_pTabControlDlg->m_pAutoFocusDlg->ChangeLanguage();
	//THEAPP.m_pTabControlDlg->m_pExtraDlg->ChangeLanguage();

	THEAPP.m_pTabControlDlg->m_pTeachParamDlg->ChangeLanguage();
	//THEAPP.m_pTabControlDlg->m_pTeachInspectionConditionDlg->ChangeLanguage();
}


#include "SelectEquipModelTypeDlg.h"
void CPreferenceDlg::OnBnClickedButtonSelectInspectionModelType()
{
	CSelectEquipModelTypeDlg dlg;
	dlg.DoModal();

	m_strEditEquipModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
	UpdateData(FALSE);

	// TODO : 아래 로직 UI 가 아니라 별도의 매니저 클래스 등에서 처리하는게 좋을 듯
	//////////////////////////////////////////////////////////////////////////////////////////////
	// FAI Inspector
	CString strModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
	THEAPP.m_pFAIDataManager.Initialize();
	ReplaceFAIInspector(THEAPP.m_pFAIInspector, strModelType, THEAPP.m_pAlgorithm);
	//////////////////////////////////////////////////////////////////////////////////////////////

	// 2. ModelManager 초기화 (데이터)
	THEAPP.ReadInspectionTypeInfoINI();
	THEAPP.ReadOverlayViewportINI();
	THEAPP.ReadDefectReViewInfoINI();
	THEAPP.ReadFaiDefectReViewInfo();

	// 3. UI 갱신 (뷰어, 설정 등)
	if (THEAPP.m_pModelDataManager)
		THEAPP.m_pModelDataManager->UpdateTeachParamView();
	if (THEAPP.m_pTabControlDlg)
		THEAPP.m_pTabControlDlg->LoadModelConditionParam();
	if (THEAPP.m_pInspectAdminViewDlg)
		THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();

	if (THEAPP.m_iPGMLanguageSelect == 0)
		strMessageBox.Format("광학 데이터를 새로 로드 하시겠습니까?");
	else
		strMessageBox.Format("Would you like to reload the optical data?");
	if (AfxMessageBox(strMessageBox, MB_OKCANCEL) == IDOK)
	{
		CString sVisionCamType;
		BOOL bModelExist1, bModelExist2;
		for (int iPcVisionNo = VISION_NUMBER_MAX - 1; iPcVisionNo >= VISION_NUMBER_1; iPcVisionNo--)
		{
			sVisionCamType = THEAPP.m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
			if (sVisionCamType == "Undefine")
				continue;

			bModelExist1 = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->LoadModelHWBaseData();
			bModelExist2 = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->LoadModelHWData();
		}

		if (!bModelExist1 || !bModelExist2)
		{
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessageBox.Format("광학 모델 파일을 찾을 수 없습니다. Optical 폴더 내 모델 파일을 확인해주세요.");
			else
				strMessageBox.Format("The optical model file cannot be found. Please check the model file in the Optical folder.");
			AfxMessageBox(strMessageBox, MB_ICONERROR | MB_SYSTEMMODAL);

			strLog.Format("Optical recipe load fail(manual), Recipe name: %s_%s_PC%d", THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
			THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
		}
		else
		{
			strLog.Format("Optical recipe load complete(manual), Recipe name: %s_%s_PC%d", THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
			THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));
		}
	}
}

void CPreferenceDlg::OnBnClickedRadioVmTrain1()
{
	GetDlgItem(IDC_CHECK_USE_VM_THRES_OPT)->EnableWindow(TRUE);
}

void CPreferenceDlg::OnBnClickedRadioVmTrain2()
{
	GetDlgItem(IDC_CHECK_USE_VM_THRES_OPT)->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_USE_VM_THRES_OPT))->SetCheck(BST_UNCHECKED);
}

void CPreferenceDlg::OnBnClickedRadioVmTrain3()
{
	GetDlgItem(IDC_CHECK_USE_VM_THRES_OPT)->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_USE_VM_THRES_OPT))->SetCheck(BST_UNCHECKED);
}
