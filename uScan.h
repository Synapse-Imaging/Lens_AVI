// uScan.h : main header file for the uScan application
//

#if !defined(AFX_uScan_H__7AC743F0_B386_4D5B_9E6B_09F72F50F422__INCLUDED_)
#define AFX_uScan_H__7AC743F0_B386_4D5B_9E6B_09F72F50F422__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "FAIDefine.h"
#include "FAIDataManager.h"

#include "InspectAdminViewDlg.h"
#include "InspectAdminViewToolbarDlg.h"
#include "InspectAdminViewHideDlg1.h"
#include "InspectAdminViewHideDlg2.h"
#include "InspectAdminViewHideDlg3.h"
#include "InspectAdminViewHideDlg4.h"
#include "InspectSummary.h"
#include "InspectResultDlg.h"
#include "CalDataService.h"
#include "DefectListDlg.h"

#include "TrayAdminViewDlg1.h"
#include "TrayAdminViewDlg2.h"
#include "TrayAdminViewDlg3.h"
#include "TrayAdminViewDlg4.h"

#include "InspectViewOverayImageDlg1.h"
#include "InspectViewOverayImageDlg2.h"
#include "InspectViewOverayImageDlg3.h"
#include "InspectViewOverayImageDlg4.h"
#include "InspectViewOverayImageToolbarDlg1.h"
#include "InspectViewOverayImageToolbarDlg2.h"
#include "InspectViewOverayImageToolbarDlg3.h"
#include "InspectViewOverayImageToolbarDlg4.h"

#include "TabControlDlg.h"

#include "FileBase.h"
#include "ModelDataManager.h"
#include "InspectLibraryDataManager.h"
#include "LightLibraryDataManager.h"

#include "Algorithm.h"

#include "HandlerService.h"
#include "CameraManager.h"
#include "TriggerManager.h"
#include "LightPwmManager.h"
#include "InspectService.h"
#include "AutoCalService.h"	
#include "MIInterface.h"
#include "IniFileCS.h"

#include "PGMLanguageSelectDlg.h"

#ifdef USE_SUAKIT
#include "ADJClientService.h"
#endif

#include <array>
#include <string>
#include <type_traits>

#pragma region LGIT AI Sercive
#include "AIService/ImageUtile.h"
#include "AIService/ThreadPool.h"

#ifdef LGITAI
#include "LGIT/types.h"
#include "LGIT/LAIInspector.h"
#include "LGIT/LAIInspector2.h"
#include "LGIT/LAIInspectorSVM.h"
#include "LGIT/LAIInspectorVAL.h"
#include "LGIT/Model/LRequestedInspectionData1.h"
#include <tbb/concurrent_unordered_map.h>
#include "LGIT/Model/ImageROIManager.h"
#include "LGIT/InspectorManager.h"
#endif // LGITAI
#pragma endregion

#ifdef SYAI
#if __has_include("syai/runtime/inspection/inspectionService.h")
#include <syai/runtime/inspection/inspectionService.h>
#include <syai/runtime/domain/config/ConfigurationBuilder.h>
#include "syai_service/ServiceDirector.h"
#else
#undef SYAI
#endif // __has_include("syai/runtime/inspection/inspectionService.h")
#endif // SYAI

// TODO : 추후 아래 폴더선택 콜백 함수 헤더파일에 이관할 예정 - jhkim, 260129
inline int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg) {
		// 폴더선택 다이얼로그의 초기화가 끝난 경우
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;
}

enum class LogCategory {
	System, Inspection, Handler, Halcon, Scan, ADJ, Image, Thread, ETC
};

typedef struct _MES_MODULE_DATA
{
	int			iNo;
	CString		sModuleBarcode;
	CString		sSite;
	CString		sEqpID;
	CString		sEqpName;
	CString		sPara;
	CString		sToolCavity;
} MES_MODULE_DATA;

/////////////////////////////////////////////////////////////////////////////
// CuScanApp:
// See uScan.cpp for the implementation of this class
//
#include <afxadv.h>

class CRecentFileList;

// FAI Forward Declaration
class IFAIInspector;

class CuScanApp : public CWinApp
{
public:
	CuScanApp();

	BOOL IsVisionSWRun();
	void DoEvents();
	void DoEvents(DWORD dwSleep);
	void SetInspectionReady();
	void SetExitProgram();

	enum eAppState { eNull = 0, eInspect, eTeaching, eReview, eSPC, eManualOP };

	CString GetWorkingDirectory() { return m_szWorkingDir; }
	int GetProgramVersion() { return m_iAppVersion; }

	CString GetLASDataDirectory() { return m_szLASDataFolder; }
	CString GetPCID() { return m_szPCID; }

	int GetSwParamVersion() { return m_iSwParamVersion; }
	void SetSwParamVersion(int iParamVersion) { m_iSwParamVersion = iParamVersion; }

	int GetHwParamVersion() { return m_iHwParamVersion; }
	void SetHwParamVersion(int iParamVersion) { m_iHwParamVersion = iParamVersion; }

	CString strAutoSettingMode;
	int iAutoSettingCountCurrent;
	int iAutoSettingCountEnd;

	BOOL bVariationModelSaveCheck[VISION_NUMBER_MAX];
	BOOL bTemplateModelSaveCheck[VISION_NUMBER_MAX];

	typedef struct DefectBlobInfo;
	struct LogHeaderInfoStruct
	{
		CString sInspDate;
		CString sInspTime;
		CString sPCID;
		CString sEquipNo;
		CString sLotID;
		CString sSaveConfig;
		CString sTrayID;
		int iTrayNo;
		int iModuleNo;
		int iStageNo;
		int iJigNo;
		CString sBarcode;
		const std::vector<CString>& vecResultList;
		const std::vector<CString>& vecAIResultList;
		const std::vector<CuScanApp::DefectBlobInfo>& vecDefectBlobInfoList;
		CString sFAINGItem;
		const std::vector<double>& vecFAIItems;
	};

	CString FormatLotResultString();
	CString FormatLotResultString(const LogHeaderInfoStruct& headerInfo);

	CString FormatContactPointString();
	CString FormatContactPointString(const LogHeaderInfoStruct& headerInfo);

	CString strLog;
	CString strMessageBox;

	void SaveLotResultLog(CString sPath, CString strResult, int iMzNo);
	void SaveContactPointLog(CString sPath, CString strResult, int iMzNo);
	void SaveMonitorGVEdgeLog(CString sPath, CString strResult);
	void SaveMonitorContDefectLog(CString sPath, CString strResult);

	void NewSaveDefectVariationFeatureLog(CString sPath, CString strResult);
	void TempSaveDefectVariationFeatureLog(CString sPath, CString strResult);

	void SaveInspectConditionLog(CString sPath, CString strResult);

	void SaveAllDefectsResultLog(CString sPath, CString strResult);

	void SaveAutoCalLightResultLog(CString strResult, int iPcVisionNo, int iMzNo);

	void SaveDayLotResultLog(CString sPath, CString strResult, int iMzNo);

	void SaveMultipleDefectListLog(CString sPath, CString strResult);

	void SaveDaySummaryINI(CString sPath, CString sSectionDay, CString sDefectCode, BOOL bResultNG, int iMzNo, int iNoModuleRetryGrab, int iNoModuleRetryFocusMove);

	void	SetViewStatusText(int idx, CString szMsg);
	void    UpdateCurMode();
	void    ShowVersionText();
	void    UpdateVersionText();

	void WriteDummyImage();

#ifdef LGITAI
	BOOL NewSaveVariationFeature(const LGIT::InspectionResultSet& result, CString sModelName);
	void NewSaveVariationFeature_Thread(const LGIT::InspectionResultSet& result, CString sModelName, SYSTEMTIME time, GTRegion* pInspectROIRgn, int iModelType, CString strLogSavePath);
#endif

#ifdef SYAI
	bool InitializeSYAI();
#endif // SYAI
	// 로거 초기화
	bool InitializeLogger(const char* log_name, const char* log_root, int log_level);


	std::shared_ptr<spdlog::logger> GetOrCreateCategoryLogger(const std::string& root_folder_str, int log_level, LogCategory category);
	std::shared_ptr<spdlog::logger> m_log_system;
	std::shared_ptr<spdlog::logger> m_log_handler;
	std::shared_ptr<spdlog::logger> m_log_inspection;
	std::shared_ptr<spdlog::logger> m_log_halcon;
	std::shared_ptr<spdlog::logger> m_log_scan;
	std::shared_ptr<spdlog::logger> m_log_adj;
	std::shared_ptr<spdlog::logger> m_log_image;
	std::shared_ptr<spdlog::logger> m_log_thread;
	std::shared_ptr<spdlog::logger> m_log_etc;
	BOOL InitializeVisionLogger();

	CString GetErrorMessageStr(DWORD dwErr);

	GTRegion* GetDefectTeachingInfo(HObject* pHDefectRgn, int iVisionCamType, int iMzNo, int iPcVisionNo, int iInspectionBufferIdx, int iInspectionTypeIndex, int iDefectNameIndex, int* piImageNo, int* piRoiLeft, int* piRoiTop, int* piRoiRight, int* piRoiBottom, int* piInspectionTabNo, CAlgorithmParam* pAlgorithmParam);

	int	CheckProcessExist();
	CString	GetProgramFileName();

	CCalDataService* GetCalDataService(int nCamNumber);

	//Ver 1026 add
	BOOL ConnectStatusADJ(int iMzNo);
	BOOL ConnectStatusLAS(int iMzNo);
	BOOL ChangeLAS(int iMzNo);
	enum eSaveLASStatus { eLASINIT = 0, eSAVELAS1, eSAVELAS2, eSAVELAS3 };
	eSaveLASStatus m_enumCurSaveLASStatus;
	CString m_strChangeLASFolderPath[MAX_MAGAZINE_NO];
	//Ver 1026 end

	BOOL ConnectStatusPyADJ();
	BOOL bPyADJStatus[PC_NUMBER_MAX];
	BOOL ConnectStatusLASAggregator();
	BOOL bLASAggregatorStatus[PC_NUMBER_MAX];
	LONGLONG GetTimeDiffseconds(SYSTEMTIME SaveLogTime, SYSTEMTIME CurrentTime);

	void CheckExistLogFileAndDelete(CString strLotID, int iMzNo, int iPCType);

#ifdef SNZEROMQ
	synapse::network::cpp::zeromq::client::service::ZeroMQClient m_zeromq_client; // zeromq 클라이언트 서비스 class
	char m_zeromq_client_id[256] = "";

	BOOL InitializeZeroMQ();
	BOOL m_bAutoROISegAvailable;

#endif // SNZEROMQ

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CuScanApp)
public:
	virtual BOOL InitInstance();
	void ExitProcess(CString strTargetProcName, DWORD dwExceptProcId);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	void Initialize_Variables(BOOL bReset);
	int m_iModeSwitch;
	int	m_iCurTeachVision;
	int m_iCurVisionCamType;
	int m_iCurStageIndex;
	int m_iCurSelectedResultModule;
	int m_iCurAlignCamIndex;
	// SW Version
	CString m_strVerInfo;

	CGFunction* m_pGFunction;

	BOOL g_bMacroStart;
	int g_nMacroCount;

	//jwj test
#ifndef INLINE_MODE
	CRITICAL_SECTION	   m_csBarcodeRead;
#endif

#ifdef USE_SUAKIT
	//ADJ
	CADJClientService		m_TCPClientService;

	CRITICAL_SECTION		m_csSendADJ[MAX_ADJ_CONNECT_NUM][MAX_MULTI_CONNECTION_NUM];				//쓰레드 단위로 초기화 된 MFC함수의 Send가 엇갈리는 것을 막기 위한 cs  
	CRITICAL_SECTION		m_csADJResultLog;
	CRITICAL_SECTION		m_csADJAliveCheck;
	void SaveADJLotResultLog(CString sPath, CString strResult, CString strAdditionalTitle, BOOL bUseAdditionalLog, int iPCType, int iPcVisionNo, int iMzNo);
	void SaveADJDayLotResultLog(CString sPath, CString strResult, int iPCType, int iMzNo);
	void SaveADJDaySummaryINI(CString sPath, CString sSectionModel, BOOL bResultNG, int iMzNo);
	void SaveADJTotalImageSummaryINI(CString sPath, CString sSectionModel, BOOL bResultNG, int iMzNo);
	void SaveADJDaySummarySkipRateINI(CString sPath, CString sSectionModel, BOOL bResultNG, int iMzNo);

	void SaveOnlyADJLotResultLog(CString sPath, CString strResult, CString strAdditionalTitle, BOOL bUseAdditionalLog, int iPCType, int iPcVisionNo, int iMzNo);
	void SaveOnlyADJDayLotResultLog(CString sPath, CString strResult, int iPCType, int iMzNo);

	//int	 m_nADJResult[MAX_MODEL_NUM][MAX_MODULE_NUM][MAX_DEFECT_CNT];	//ADJ Result //20201016 commented
	//float m_fADJMaxClassProb[MAX_MODEL_NUM][MAX_MODULE_NUM][MAX_DEFECT_CNT]; //ADJ Max Class Probablity //20200809 add //20200912 commented

	//20190625 jwj add
	CString GetADJPriorityDefectName(int nPriority[VISION_NUMBER_MAX], int nDeepLearningModel[VISION_NUMBER_MAX]);
	int g_arriThreadCnt[MAX_MODULE_NUM];
	BOOL bIsTimeOut = FALSE;
	int m_nADJCurDefectPriority[VISION_NUMBER_MAX][MAX_MODULE_NUM];
	int m_nADJModelIDApplyPriority[VISION_NUMBER_MAX][MAX_MODULE_NUM];

	HObject HADJFilteredDefectRgn[VISION_NUMBER_MAX][MAX_IMAGE_TAB];

	CString strADJLog[VISION_NUMBER_MAX][MAX_MODULE_NUM];
	CString strADJDefectCode[VISION_NUMBER_MAX][MAX_MODULE_NUM];
	int	iADJLogResult[VISION_NUMBER_MAX][MAX_MODULE_NUM];	//모듈의 ADJ 판정 결과에 대한 취합. 
	int iADJModelResult[VISION_NUMBER_MAX][MAX_MODEL_NUM][MAX_MODULE_NUM];	//ADJDaySummary에 저장하기 위한 정수형 배열
#endif
	std::shared_ptr<AIService::ThreadPool> m_roi_image_save_thread_pool;
	std::shared_ptr<AIService::ThreadPool> m_variation_feature_log_thread_pool;
#ifdef LGITAI
	std::shared_ptr<spdlog::logger> m_lg_inspector_logger;
	bool m_isSetLGITAI = false;
	std::shared_ptr<LGIT::LAIInspector> m_lgAIInspector;
	std::shared_ptr<LGIT::LAIInspector2> m_lgAIInspector2;
	std::shared_ptr<LGIT::LAIInspectorSVM> m_lgAIInspectorSVM;
	std::shared_ptr<LGIT::InspectorManager> m_lg_inspectorManager;
	std::shared_ptr<LGIT::LAIInspectorSEG> m_lgAIInspectorSEG;
	std::shared_ptr<LGIT::LAIInspectorVAL> m_lgAIInspectorVAL;
#endif // LGITAI

	std::shared_ptr<spdlog::logger> m_syai_logger;
#ifdef SYAI
	std::shared_ptr<syai_service::ServiceDirector> m_syai_service;
#endif // SYAI


	//211113 add
	BOOL IsNumberCheck(CString str);

	void    SetADJSaveDefectList();
	CString m_strADJSaveDefectList[MAX_DEFECT_NAME];

	CInspectAdminViewDlg*		m_pInspectAdminViewDlg;
	CInspectAdminViewToolbarDlg*		m_pInspectAdminViewToolbarDlg;
	CInspectAdminViewHideDlg1*	m_pInspectAdminViewHideDlg1;
	CInspectAdminViewHideDlg2*	m_pInspectAdminViewHideDlg2;
	CInspectAdminViewHideDlg3*	m_pInspectAdminViewHideDlg3;
	CInspectAdminViewHideDlg4*	m_pInspectAdminViewHideDlg4;

	CCalDataService* m_pCalDataService;		// Teaching Calibration Data
	CCalDataService* m_pCalDataService_N[VISION_NUMBER_MAX];	// Calibration Data for 6 Vision Cam

	//////////////////////////////////////////////////////////////////////다이알로그 클래스 선언부
	CTrayAdminViewDlg1* m_pTrayAdminViewDlg1;
	CTrayAdminViewDlg2* m_pTrayAdminViewDlg2;
	CTrayAdminViewDlg3* m_pTrayAdminViewDlg3;
	CTrayAdminViewDlg4* m_pTrayAdminViewDlg4;

	CInspectSummary*  m_pInspectSummary;
	CInspectResultDlg* m_pInspectResultDlg;
	CDefectListDlg* m_pDefectListDlg;

	CInspectViewOverayImageDlg1* m_pInspectViewOverayImageDlg1;
	CInspectViewOverayImageDlg2* m_pInspectViewOverayImageDlg2;
	CInspectViewOverayImageDlg3* m_pInspectViewOverayImageDlg3;
	CInspectViewOverayImageDlg4* m_pInspectViewOverayImageDlg4;
	CInspectViewOverayImageToolbarDlg1* m_pInspectViewOverayImageToolbarDlg1;
	CInspectViewOverayImageToolbarDlg2* m_pInspectViewOverayImageToolbarDlg2;
	CInspectViewOverayImageToolbarDlg3* m_pInspectViewOverayImageToolbarDlg3;
	CInspectViewOverayImageToolbarDlg4* m_pInspectViewOverayImageToolbarDlg4;

	CTabControlDlg* m_pTabControlDlg;

	//////////////////////////////////////////////////////////////////////다이알로그 클래스 선언부

	void ReadPreferenceINI();
	typedef struct
	{
		CString m_strEquipNo;

		int m_iPCType;

		BOOL m_bUseVision[VISION_NUMBER_MAX];

		BOOL m_bUseLAS1;
		BOOL m_bUseLAS2;
		BOOL m_bUseLAS3;

		CString m_strSaveFolderPathLAS1;
		CString m_strSaveFolderPathLAS2;
		CString m_strSaveFolderPathLAS3;

		BOOL m_bUseLASConnectionCheck;

		CString m_strRawFolderPath;
		CString m_strResultFolderPath;
		CString m_strReviewFolderPath;
		CString m_strAdjFolderPath;
		CString m_strResultLogFolderPath;
		CString m_strEtcFolderPath;
		CString m_strOfflineTestFolderPath;

		BOOL m_bRawImageSaveLocal;
		BOOL m_bResultImageSaveLocal;
		BOOL m_bReviewImageSaveLocal;
		BOOL m_bADJImageSaveLocal;
		BOOL m_bResultLogSaveLocal;

		BOOL m_bRawImageSaveLAS;
		BOOL m_bResultImageSaveLAS;
		BOOL m_bReviewImageSaveLAS;
		BOOL m_bADJImageSaveLAS;
		BOOL m_bResultLogSaveLAS;

		BOOL m_bUseRawImageThread;
		BOOL m_bUseResultImageThread;
		BOOL m_bUseReviewImageThread;
		BOOL m_bUseADJImageThread;
		BOOL m_bUseResultLogThread;

		BOOL m_bUseRawImageTempDrive;

		BOOL m_bSaveRawImage;
		BOOL m_bRawImageType;
		BOOL m_bReduceRawImage;
		double m_dRawImageZoomRatio;
		BOOL m_bCombineRawImage;
		BOOL m_bSaveResultGoodImage;
		BOOL m_bUseResultImageThreadOnlyGood;
		BOOL m_bRawImageSaveLogType;
		BOOL m_bAvoidDuplicateLotLocal;
		BOOL m_bAvoidDuplicateLotLas;

		int m_iRawImageThreadType;

		BOOL m_bUseMultiQueue;
		int m_iThreadQueueNumber;

		int m_iResultImageDefectDisplayType;
		BOOL m_bUseResultImageDefectSizeLimit;
		int m_iResultImageDefectSizeMax;

		BOOL m_bSaveADJ;
		BOOL m_bUseADJConnectionCheck;
		BOOL m_bUseLocalADJ;
		BOOL m_bUseLocalSEG;
		CString m_strCurrentADJ_IP;
		BOOL m_bIsUseADJ[MAX_ADJ_CONNECT_NUM];
		CString m_strADJ_IP[MAX_ADJ_CONNECT_NUM];
		int		m_iADJ_Port[MAX_ADJ_CONNECT_NUM];
		BOOL m_bIsSendADJ;
		BOOL m_bUseADJMultiimage;
		BOOL m_bIsApplyADJ;
		BOOL m_bUseADJTimeOut;
		int m_nADJTimeOut;
		int m_nADJ_MultiNetworkNO;
		int m_iADJImageSize;
		int m_iADJImageReSize;
		int m_iMarkingImageRadius;
		int m_iADJCombineDefectDistance;
		BOOL m_bSaveROIImage;
		int m_iSaveROIImageType;
		BOOL m_bAISetupUseVAL;
		BOOL m_bAllImageAiInspection;

		BOOL m_bSaveMDJ;
		int m_iMaxReviewImageNumber;
		int m_iReviewCombineDefectDistance;
		BOOL m_bUseSaveFaiReviewImage;

		int m_iReviewImageDisplayType;

		int m_iVision1LightIP1;
		int m_iVision1LightIP2;
		int m_iVision1LightIP3;
		int m_iVision1LightIP4;
		int m_iVision1LightPort;

		int m_iVision2LightIP1;
		int m_iVision2LightIP2;
		int m_iVision2LightIP3;
		int m_iVision2LightIP4;
		int m_iVision2LightPort;

		int m_iVision3LightIP1;
		int m_iVision3LightIP2;
		int m_iVision3LightIP3;
		int m_iVision3LightIP4;
		int m_iVision3LightPort;

		int m_iVision4LightIP1;
		int m_iVision4LightIP2;
		int m_iVision4LightIP3;
		int m_iVision4LightIP4;
		int m_iVision4LightPort;

		int m_iGrabErrRetryNo;
		int m_iGrabDelayTime;
		int m_iTriggerSleepTime;
		int m_iGrabDoneWaitTime;
		int m_iTeachingLiveGrabPeriod;
		int m_iLightErrValue;
		int m_iTeachingCameraLightType;
		BOOL m_bUseGrabHold;
		int m_iFocusMoveNoRetry;
		int m_iFocusMoveCompleteWaitTime;
		BOOL m_bUse4dSaveRawImage;
		BOOL m_bUseAutoFocus;
		BOOL m_bUseFastGrab;
		BOOL m_bUseBatchInspection;
		BOOL m_bSendGrabFailError;
		int m_iSendGrabFailErrorCount;

		int m_iDefectDispDist;
		int m_iMatchingSearchMargin;
		int m_iBarcodeNoReadImageType;
		BOOL m_bUseSaveDefectShapeFeature;
		int m_iMaxSameDefectShapeFeature;
		BOOL m_bUseSaveGVEdgeMornitoringLog;
		BOOL m_bUseGpuAffineTrans;
		BOOL m_bUseDeleteCrossBar;
		int m_iSaveRecentlyCompleteInfoNumber;

		BOOL m_bUseContDefectAlarm;
		int m_iContDefectPosTolerance;
		int m_iContDefectCount;

		int m_iSam2IP_1;
		int m_iSam2IP_2;
		int m_iSam2IP_3;
		int m_iSam2IP_4;
		int m_iSam2Port;

		BOOL m_bUseVisionCopy;

		int	m_iVmTrain;
		BOOL m_bVmTrainAll;

		BOOL m_bUseVmThresOpt;
		int m_iVmSplitThres;
		double m_dVmKSigma;

		int m_iVmOptThrDetect;
		int m_iVmOptMinPixelRatio;
		int m_iVmOptAbsThresMin;
		int m_iVmOptAbsThresStep;
		double m_dVmOptVarThresMin;
		double m_dVmOptVarThresStep;
		int m_iVmOptIouTarget;

#ifdef USE_SUAKIT
		int m_nADJThreadNO[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM];
		CString m_strADJ_Defect_Pos[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM];
		CString m_strADJ_Defect_Name[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM];
		BOOL    m_bSideTotal[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM];		// 230323 기능 변경(망 통합기능)

		int m_nADJThreadNO_Integrated[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM][MAX_DEFECT_CNT];
		CString m_strADJ_Defect_Integrated_Pos[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM][MAX_DEFECT_CNT];
		CString m_strADJ_Defect_Integrated_Name[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM][MAX_DEFECT_CNT];

		int m_nADJThreadNO_Integrated_Segment[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM][MAX_DEFECT_CNT];
		CString m_strADJ_Defect_Integrated_Segment_Pos[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM][MAX_DEFECT_CNT];
		CString m_strADJ_Defect_Integrated_Segment_Name[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM][MAX_DEFECT_CNT];

		int m_nADJFAIModel_Integrated[MAX_ADJ_CONNECT_NUM][DEEP_MODEL_NUM][MAX_FAI_ITEM];
#endif

		int	iDefectPriority[MAX_DEFECT_NAME];
		int iInspectionTypePriority[MAX_INSPECTION_TYPE];

		int  iNoAssignSpecialCode;
		CString m_sAssignSpecialCode[MAX_DEFECT_NAME];
		int m_iSpecialNGCodeIndex[MAX_DEFECT_NAME];

		int iOverlayImageIndex[VISION_NUMBER_MAX][MAX_RESULT_IMAGE];
		int iOverlayImageViewportLeft[VISION_NUMBER_MAX][MAX_RESULT_IMAGE];
		int iOverlayImageViewportTop[VISION_NUMBER_MAX][MAX_RESULT_IMAGE];
		double dOverlayImageViewportZoomRatio[VISION_NUMBER_MAX][MAX_RESULT_IMAGE];

		int iRosImageViewportLeft[VISION_NUMBER_MAX];
		int iRosImageViewportTop[VISION_NUMBER_MAX];
		double dRosImageViewportZoomRatio[VISION_NUMBER_MAX];

		int m_iDFMGrabDoneWaitTime;
		int m_iDFMTriggerPeriod;
		int m_iDFMGrabComPortNumber;
		BOOL m_bUseConvertSingleThread;

	}PreferenceStruct;
	PreferenceStruct Struct_PreferenceStruct;

	//////////////////////////////////////////////////////////////////////////	Model Type
	void ReadModelNameCodeINI();
	// Read from ModelNameCode.ini
	CString g_strModelTypeName[MODEL_TYPE_MAX];
	// Read from ModelNameCode.ini
	CString g_strModelTypeLotCode[MODEL_TYPE_MAX];

	int g_iGrabFailCount[MAX_MAGAZINE_NO];
	int g_iDualModelData[MAX_MAGAZINE_NO];
	int g_iDualModelDataRunCheck[MODEL_DATA_MAX];

	char* TempUTF8toANSI(char* pszCode);

	CModelDataManager* m_pModelDataManager;
	CModelDataManager* m_pDualModelDataManager[MODEL_DATA_MAX][VISION_NUMBER_MAX];

	CCameraManager* m_pCameraManager;
	CCameraManager* m_pDualCameraManager[VISION_NUMBER_MAX];
	CCameraManager* m_pCameraManagerSpecularMaster;
	CCameraManager* m_pSubCameraManager[TOTAL_SUB_CAM];

	CInspectLibraryDataManager* m_pInspectLibraryDataManager;
	CLightLibraryDataManager* m_pLightLibraryDataManager;


	//////////////////////////////////////////////////////////////////////////	Vision SW Language

	void ReadPGMLanguageSelectINI();
	BOOL m_iPGMLanguageSelect;
	BOOL m_bPGMLanguageDialogHide;

	//////////////////////////////////////////////////////////////////////파일 폴더 클래스 선언부
	CFileBase m_FileBase;
	//////////////////////////////////////////////////////////////////////파일 폴더 클래스 선언부

	//////////////////////////////////////////////////////////////////////////	Offline
	int m_iOfflineCurTrayNo;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////	No. Vision Thread
	int m_iVisionInspectThreadNumber[VISION_NUMBER_MAX];
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////DFM Thread Stop Event
	CEvent m_eventStop;
	//////////////////////////////////////////////////////////////////////DFM Thread Stop Event

	//////////////////////////////////////////////////////////////////////////
	// Barcode Update
	void AddBarcodeOnImageFile(int iVisionCamType, CString strRawImageFolderPath_Local, CString strResultImageFolderPath_Local,
		CString strReviewImageFolderPath_Local, CString strADJImageFolderPath_Local,
		CString strRawImageFolderPath_LAS, CString strResultImageFolderPath_LAS,
		CString strReviewImageFolderPath_LAS, CString strADJImageFolderPath_LAS,
		int iNoInspectImage, int iMzNo, int iTrayNo, int iModuleNo, CString sBarcodeID);

	void AddHistoryOnImageFile(int iVisionCamType, CString strImageFolderPath_Local, CString strImageFolderPath_LAS, int iNoInspectImage, int iMzNo, int iTrayNo, int iModuleNo, CString sBarcodeID);

	//////////////////////////////////////////////////////////////////////////
	void SaveTeachingParameter(CString sSaveRootFolderName);
	//////////////////////////////////////////////////////////////////////////

	void SaveAiRetrainRoi(int iPcVisionNo);

	//////////////////////////////////////////////////////////////////////////	Defect Name (from INI file)
	CString m_strInspectionType[MAX_INSPECTION_TYPE];
	CString m_strDefectName[MAX_DEFECT_NAME];
	CString m_strDefectCode[MAX_DEFECT_NAME];

	CString GetDefectCode(int iDefectNameIndex, int* piDefectTransferCodeIndex);
	CString GetDefectCode(CString sDefectName);
	CString GetFaiDefectCode(int iMzNo, int iTrayNo, int iModuleNo, int *piSelectedFaiIndex);
	CString CheckDefectNameChangeND(CString sInspectionType, CString sDefectName);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////	Barcode Print Quality Grade
	CString GetBarcodePrintQualityGrade(int iBarcodeGradeNumber);

	//////////////////////////////////////////////////////////////////////////	Top1 / Mirror Position Info
	BOOL m_bMirrorPositionChecked;
	HObject m_HMirrorAlignResultRgn;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////	Barcode
	typedef struct
	{
		BOOL m_bBarcodeRegResult;
		BOOL m_bBarcodeShiftNG;
		BOOL m_bBarcodeRotationNG;
		CString m_sBarcodeResult;
		double m_dBarcodePosOffsetX;
		double m_dBarcodePosOffsetY;
		int m_iBarcodeRotationResult;
		HObject m_HBarcodeDetectRgn;

		void Reset()
		{
			m_bBarcodeRegResult = FALSE;
			m_bBarcodeShiftNG = FALSE;
			m_bBarcodeRotationNG = FALSE;
			m_sBarcodeResult = _T("NOREAD");
			m_dBarcodePosOffsetX = 0;
			m_dBarcodePosOffsetY = 0;
			m_iBarcodeRotationResult = 0;
			GenEmptyObj(&m_HBarcodeDetectRgn);
		};
	}BarcodeResultStruct;
	BarcodeResultStruct m_StructModuleBarcodeResult[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY]; ///

	int m_iModuleMixResult[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY];
	//////////////////////////////////////////////////////////////////////////	Datacode Print Quality
	typedef struct
	{
		BOOL m_bInspectPrintQuality;
		double	m_dPrintQualityGrade[MAX_PRINT_QUALITY_GRADE];
		CString	m_sPrintQualityValue[MAX_PRINT_QUALITY_GRADE];

		void Reset()
		{
			m_bInspectPrintQuality = FALSE;
			for (int iii = 0; iii < MAX_PRINT_QUALITY_GRADE; iii++)
			{
				m_dPrintQualityGrade[iii] = -1;
				m_sPrintQualityValue[iii] = _T("");
			}
		};
	}PrintQualityStruct;
	PrintQualityStruct m_StructModulePrintQuality[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY]; ///

	//////////////////////////////////////////////////////////////////////////	ROS Image

	//////////////////////////////////////////////////////////////////////////	Module History Info
	typedef struct
	{
		int iLoadPicker;
		int iLoadPickerIndex;
		int iVisionStageNo;
		int iVisionStageIndex;
		int iUnloadPickerNo;
		int iUnloadPickerIndex;
		CString sJudge;
		int iGoodTrayNo;
		int iGoodTrayPorket;
		int iNGTrayNo;
		int iNGTrayPorket;

		void Reset()
		{
			iLoadPicker = -1;
			iLoadPickerIndex = -1;
			iVisionStageNo = -1;
			iVisionStageIndex = -1;
			iUnloadPickerNo = -1;
			iUnloadPickerIndex = -1;
			sJudge = _T("G");
			iGoodTrayNo = -1;
			iGoodTrayPorket = -1;
			iNGTrayNo = -1;
			iNGTrayPorket = -1;
		};
	}ModuleHistoryStructInfo;
	ModuleHistoryStructInfo m_StructModuleHistoryInfo[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY]; ///

	//////////////////////////////////////////////////////////////////////////	Module Data (Feedback)
	typedef struct
	{
		BOOL			bUsed;
		CString			sLotID;
		MES_MODULE_DATA	MesModuleData[MAX_MODULE_TRAY];

		void Reset()
		{
			bUsed = FALSE;
			sLotID = _T("");

			for (int i = 0; i < MAX_MODULE_TRAY; i++)
			{
				MesModuleData[i].iNo = -1;
				MesModuleData[i].sModuleBarcode = _T("");
				MesModuleData[i].sSite = _T("");
				MesModuleData[i].sEqpID = _T("");
				MesModuleData[i].sEqpName = _T("");
				MesModuleData[i].sPara = _T("");
				MesModuleData[i].sToolCavity = _T("");
			}
		};
	}ModuleDataStructInfo;
	ModuleDataStructInfo m_StructModuleDataInfo[MAX_MAGAZINE_NO]; ///

	//////////////////////////////////////////////////////////////////////////	ReviewImage and Defectshape Feature Matching Index;
	typedef struct
	{

		int iRosReviewSpecApplyDefectindex[MAX_INSPECTION_TYPE][MAX_DEFECT_NAME][MAX_REVIEW_INDEX];
		void Reset()
		{

			for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
			{
				for (int j = 0; j < MAX_DEFECT_NAME; j++)
				{

					for (int z = 0; z < MAX_REVIEW_INDEX; z++)
					{

						iRosReviewSpecApplyDefectindex[i][j][z] = 0;
					}
				}
			}
		}

	}ModuleDefectIndexStructInfo;
	ModuleDefectIndexStructInfo m_StructModuleDefectIndexInfo[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY];


	typedef struct
	{

		int ImageNo[MAX_REVIEW_INDEX];
		int InspectType[MAX_REVIEW_INDEX];
		int DefectType[MAX_REVIEW_INDEX];
		int Buffer[MAX_REVIEW_INDEX];
		int ReviewCheck[MAX_REVIEW_INDEX];
		void Reset()
		{
			for (int i = 0; i < MAX_REVIEW_INDEX; i++)
			{
				ImageNo[i] = 0;
				InspectType[i] = 0;
				DefectType[i] = 0;
				Buffer[i] = 0;
				ReviewCheck[i] = 0;
			}
		}

	}ModuleReviewIndexStructInfo;
	ModuleReviewIndexStructInfo m_StructModuleReviewInfo[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY];

	//////////////////////////////////////////////////////////////////////////	ROS Image

	int m_iMaxRosSaveDefect;

	typedef struct
	{
		CString m_sInspectionType;		// 검사 영역
		CString m_sDefectName;			// 불량 명
		CString m_sVisionCam;			// 카메라 명
		int		m_iImageNo;			// 영상 Index
		int		m_iStartX;				// 시작 위치
		int		m_iStartY;
		float	m_fZoomRatio;
		int     m_iUseViewPortFlag;

		void Reset()
		{
			m_sInspectionType = _T("");
			m_sDefectName = _T("");
			m_sVisionCam = _T("");

			m_iImageNo = 0;
			m_iStartX = 0;
			m_iStartY = 0;
			m_fZoomRatio = 0.0;
			m_iUseViewPortFlag = 1;
		};
	}ReViewImageInfoStruct;
	ReViewImageInfoStruct m_Struct_ReViewImageInfo[MAX_REVIEW_SAMPLE];

	//////////////////////////////////////////////////////////////////////////	FAI ROS Image
	typedef struct
	{
		BOOL	m_bGenReviewImage;							// Review 이미지 생성 유무
		CString m_sFaiName;									// FAI 이름
		BOOL	m_bInspectType;								// FINAL(FALSE), DFA(TRUE)
		int		m_iNoReviewImage;							// Review 생성 이미지 수량
		int		m_iImageNo[MAX_FAI_REVIEW_IMAGE];			// 각 Review 영상 생성 번호
		int		m_iStartX[MAX_FAI_REVIEW_IMAGE];			// 시작 위치
		int		m_iStartY[MAX_FAI_REVIEW_IMAGE];
		float	m_fZoomRatio[MAX_FAI_REVIEW_IMAGE];
		int		m_iViewportCenter[MAX_FAI_REVIEW_IMAGE];	// How to assign the center of viewport (0: Blob Center, 1: Calculated Point)

		void Reset()
		{
			m_bGenReviewImage = FALSE;
			m_sFaiName = _T("");
			m_bInspectType = TRUE;
			m_iNoReviewImage = 0;

			for (int i = 0; i < MAX_FAI_REVIEW_IMAGE; i++)
			{
				m_iImageNo[i] = 0;
				m_iStartX[i] = 0;
				m_iStartY[i] = 0;
				m_fZoomRatio[i] = 0.0;
				m_iViewportCenter[i] = 0;
			}
		};
	}FaiReViewImageInfoStruct;
	FaiReViewImageInfoStruct m_Struct_FAI_ReViewImageInfo[MAX_FAI_ITEM];

	typedef struct
	{
		CString m_sInspectionType;		// 검사 영역
		CString m_sDefectName;			// 불량 명
		CString m_sVisionCam;			// 카메라 명
		int		m_iImageIndex[MAX_IMAGE_COUNT];			// 영상 Index
		int		m_iImageCount;		// 영상 Index

		void Reset()
		{
			m_sInspectionType = _T("");
			m_sDefectName = _T("");
			m_sVisionCam = _T("");
			for (int i = 0; i < MAX_IMAGE_COUNT; i++)
				m_iImageIndex[i] = -1;
			m_iImageCount = -1;

		};
	}ADJImageInfoStruct;
	ADJImageInfoStruct m_Struct_AdjImageInfo[MAX_REVIEW_SAMPLE];

	int	m_iAdjSelectImageCount;
	void ReadDefectReViewInfoINI();
	void ReadFaiDefectReViewInfo();
	//char* CuScanApp::UTF8toANSI(char *pszCode);
	void ReadADJReViewInfoINI();
	//////////////////////////////////////////////////////////////////////////
	BOOL m_bLoadingAlignGrabFailFlag[MAX_MAGAZINE_NO][MAX_TRAY_LOT];

	//////////////////////////////////////////////////////////////////////////	Stiffener Barcode-ID (All Module in a LOT)
	CString m_strModuleBarcodeID[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY];

	//////////////////////////////////////////////////////////////////////////	Load Complete Flag (All Module in a LOT)
	BOOL m_bLoadCompleteFlag[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][VISION_NUMBER_MAX];

	//////////////////////////////////////////////////////////////////////////	Vision -> Handler Scan Complete Flag
	BOOL m_bScanCompleteFlag[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][VISION_NUMBER_MAX];

	////////////////////////////////////////////////////////////////////////// Defect -> Handler Alarm Flag
	BOOL m_bHandlerAlarmFlag[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY];

	////////////////////////////////////////////////////////////////////////// Camera Inspection result Data
	double m_dAlignModuleDeltaX[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY];
	double m_dAlignModuleDeltaY[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY];
	double m_dAlignModuleDeltaAngle[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY];

	CString m_sInspectionResult[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][VISION_NUMBER_MAX];
	CString m_sFinalInspectionResult[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][VISION_NUMBER_MAX];

	CString m_sROSDefectResult[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][VISION_NUMBER_MAX];
	CString m_sAllDefectsResult[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][VISION_NUMBER_MAX];

	CString m_sAI1DefectResult[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][VISION_NUMBER_MAX];
	CString m_sAI2DefectResult[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][VISION_NUMBER_MAX];
	CString m_sAI3DefectResult[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][VISION_NUMBER_MAX];

	//////////////////////////////////////////////////////////////////////////	Corner Measurement
	typedef struct
	{
		double	m_dMeasureValueMm[CORNER_MEASURE_POINT_NUMBER];

		void Reset()
		{
			for (int iii = 0; iii < CORNER_MEASURE_POINT_NUMBER; iii++)
			{
				m_dMeasureValueMm[iii] = -1;
			}
		};
	}CornerMeasureStruct;
	CornerMeasureStruct m_StructModuleCornerMeasure[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY]; ///

	//////////////////////////////////////////////////////////////////////////	ShieldCan/Stiffener Gap Z-Directional Measurement (2 Points)

	typedef struct
	{
		BOOL m_bMeasureDone[DISTANCE_MEASURE_POINT_NUMBER];
		int	m_iSaveID[DISTANCE_MEASURE_POINT_NUMBER];
		double	m_dDistanceMeasureValueUm[DISTANCE_MEASURE_POINT_NUMBER];
		POINT	m_iDistanceUpperImagePoint[DISTANCE_MEASURE_POINT_NUMBER];
		POINT	m_iDistanceLowerImagePoint[DISTANCE_MEASURE_POINT_NUMBER];

		void Reset()
		{
			for (int iii = 0; iii < DISTANCE_MEASURE_POINT_NUMBER; iii++)
			{
				m_bMeasureDone[iii] = FALSE;
				m_iSaveID[iii] = 1;
				m_dDistanceMeasureValueUm[iii] = -1;
				m_iDistanceUpperImagePoint[iii].x = 0;
				m_iDistanceUpperImagePoint[iii].y = 0;
				m_iDistanceLowerImagePoint[iii].x = 0;
				m_iDistanceLowerImagePoint[iii].y = 0;
			}
		};
	}DistanceMeasureStruct;
	DistanceMeasureStruct m_StructDistanceMeasure[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY]; ///

	//////////////////////////////////////////////////////////////////////////	ShieldCan/Stiffener Gap X/Y-Directional Measurement (2 Points, 2 ROIs)
	typedef struct
	{
		int	m_iSaveID[GAP_MEASURE_POINT_NUMBER];
		BOOL m_bMeasureDone[GAP_MEASURE_POINT_NUMBER];
		double	m_dGapMeasureValueMm[GAP_MEASURE_POINT_NUMBER];
		BOOL	m_bGapResultNG[GAP_MEASURE_POINT_NUMBER];

		void Reset()
		{
			for (int iii = 0; iii < GAP_MEASURE_POINT_NUMBER; iii++)
			{
				m_iSaveID[iii] = 1;
				m_bMeasureDone[iii] = FALSE;
				m_dGapMeasureValueMm[iii] = -9999;
				m_bGapResultNG[iii] = FALSE;
			}
		};
	}XYGapMeasureStruct;
	XYGapMeasureStruct m_StructModuleGapMeasureXY[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY]; ///


	//////////////////////////////////////////////////////////////////////////	FAI Measurement
	typedef struct
	{
		double	m_dStartX;
		double	m_dStartY;
		double	m_dEndX;
		double	m_dEndY;

		void Reset()
		{
			m_dStartX = -1;
			m_dStartY = -1;
			m_dEndX = -1;
			m_dEndY = -1;
		};
	}LineStruct;

	int m_iTeachingModuleNo;
	int m_iLastSelectVisionCamType;
	int m_iLastSelectPcVision;

	
	// FAI 측정 데이터: CFAIDataManager로 이관 완료
	// 기존 CenterlinMeaser 구조체 및 기타 구조체를 모두 갖고있는 래퍼 클래스
	// /FAI/ModelData/FAIDataManager.h 참고
	CFAIDataManager &m_pFAIDataManager;
	// AlgorithmPipeline.cpp 등 기존 코드와의 호환성 유지 (CFAIDataManager 배열 별칭)

	CenterlineMeasureStruct (&m_StructFaiMeasure)[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][MAX_FAI_PARAMETER];
	//////////////////////////////////////////////////////////////////////////	Continuous Defect Check (All Module in a LOT)

	typedef struct
	{
		BOOL	m_bModuleDefect;
		BOOL	m_bInspectionTypeDefect[MAX_INSPECTION_TYPE];
		HObject m_HDefectRgn[MAX_INSPECTION_TYPE];

		void Reset()
		{
			m_bModuleDefect = FALSE;

			for (int jjj = 0; jjj < (MAX_INSPECTION_TYPE); jjj++)
			{
				m_bInspectionTypeDefect[jjj] = FALSE;
				GenEmptyObj(m_HDefectRgn + jjj);
			}
		};
	}ContDefectStruct;
	ContDefectStruct m_StructModuleContDefect[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY]; ///

	//////////////////////////////////////////////////////////////////////////	Defect Blob Info
	struct DefectBlobInfo
	{
		int iArea;
		int iDefectCenterX;
		int iDefectCenterY;
		int iResultImageNo;

		CString sInspectType;
		CString sDefectName;

		void Reset()
		{
			iDefectCenterX = -1;
			iDefectCenterY = -1;
			iResultImageNo = -1;
			sInspectType = _T("nan");
			sDefectName = _T("nan");
		}
	};
	DefectBlobInfo m_StructDefectBlobInfo[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][VISION_NUMBER_MAX];

	//////////////////////////////////////////////////////////////////////////	Count per Area Condition
	void LoadCountPerAreaCondition_ini();
	BOOL CountPerAreaCondition();
	void SaveCountPerAreaCondition();

	typedef struct
	{
		double m_dMinArea[MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];
		double m_dMidArea[MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];
		double m_dMaxArea[MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];

		int m_iMinNumAccept[MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];
		int m_iMidNumAccept[MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];
		int m_iMaxNumAccept[MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];

		void Reset()
		{
			for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
			{
				for (int j = 0; j < MAX_DEFECT_NAME; j++)
				{
					m_dMinArea[i][j] = 0.0;
					m_dMidArea[i][j] = 0.0;
					m_dMaxArea[i][j] = 0.0;
					m_iMinNumAccept[i][j] = 100;
					m_iMidNumAccept[i][j] = 2;
					m_iMaxNumAccept[i][j] = 1;
				}
			}
		}
	}CountPerAreaConditionData;
	CountPerAreaConditionData m_StructCountPerAreaConditionInfo;

	//////////////////////////////////////////////////////////////////////////	Variation Parameter
	BOOL LoadVariationParameter();
	void SaveVariationParameter(int iVisionIdx, CString sModelType);
	void ClearVariationParameter(int iVisionIdx, CString sModelType);

	typedef struct
	{
		BOOL bUseImageCompare[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];

		int iProcessChType[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		BOOL bUseImageCompareBright[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iImageCompareBrightAbs[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		double dImageCompareBrightVar[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		BOOL bUseImageCompareDark[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iImageCompareDarkAbs[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		double dImageCompareDarkVar[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];

		void Reset()
		{
			for (int i = 0; i < VISION_NUMBER_MAX; i++)
			{
				for (int j = 0; j < MAX_IMAGE_TAB; j++)
				{
					for (int k = 0; k < 10; k++)
					{
						for (int l = 0; l < MAX_INSPECTION_TAB; l++)
						{
							bUseImageCompare[i][j][k][l] = FALSE;
							iProcessChType[i][j][k][l] = CHANNEL_TYPE_I;
							bUseImageCompareBright[i][j][k][l] = FALSE;
							iImageCompareBrightAbs[i][j][k][l] = 0;
							dImageCompareBrightVar[i][j][k][l] = 0;
							bUseImageCompareDark[i][j][k][l] = FALSE;
							iImageCompareDarkAbs[i][j][k][l] = 0;
							dImageCompareDarkVar[i][j][k][l] = 0;
						}
					}
				}
			}
		}
	}VariationParameter;
	VariationParameter m_StructVariationParameter;

	//////////////////////////////////////////////////////////////////////////	Defect Condition
	BOOL LoadDefectCondition();
	void SaveDefectCondition(int iVisionIdx, CString sModelType);
	void ClearDefectCondition(int iVisionIdx, CString sModelType);

	typedef struct
	{
		BOOL bUseImageCompare[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];

		BOOL bUseDefectConditionArea[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iDefectConditionAreaMin[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iDefectConditionAreaMax[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];

		BOOL bUseDefectConditionLength[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iDefectConditionLengthMin[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iDefectConditionLengthMax[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];

		BOOL bUseDefectConditionWidth[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iDefectConditionWidthMin[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iDefectConditionWidthMax[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];

		BOOL bUseDefectConditionEllipseRatio[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		double dDefectConditionEllipseRatioMin[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		double dDefectConditionEllipseRatioMax[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];

		BOOL bUseDefectConditionContrast[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iDefectConditionContrastMin[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iDefectConditionContrastMax[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];

		BOOL bUseDefectConditionOuterDist[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];
		int iDefectConditionOuterDist[VISION_NUMBER_MAX][MAX_IMAGE_TAB][10][MAX_INSPECTION_TAB];

		void Reset()
		{
			for (int i = 0; i < VISION_NUMBER_MAX; i++)
			{
				for (int j = 0; j < MAX_IMAGE_TAB; j++)
				{
					for (int k = 0; k < 10; k++)
					{
						for (int l = 0; l < MAX_INSPECTION_TAB; l++)
						{
							bUseImageCompare[i][j][k][l] = FALSE;
							bUseDefectConditionArea[i][j][k][l] = FALSE;
							iDefectConditionAreaMin[i][j][k][l] = 0;
							iDefectConditionAreaMax[i][j][k][l] = 0;

							bUseDefectConditionLength[i][j][k][l] = FALSE;
							iDefectConditionLengthMin[i][j][k][l] = 0;
							iDefectConditionLengthMax[i][j][k][l] = 0;

							bUseDefectConditionWidth[i][j][k][l] = FALSE;
							iDefectConditionWidthMin[i][j][k][l] = 0;
							iDefectConditionWidthMax[i][j][k][l] = 0;

							bUseDefectConditionEllipseRatio[i][j][k][l] = FALSE;
							dDefectConditionEllipseRatioMin[i][j][k][l] = 0;
							dDefectConditionEllipseRatioMax[i][j][k][l] = 0;

							bUseDefectConditionContrast[i][j][k][l] = FALSE;
							iDefectConditionContrastMin[i][j][k][l] = 0;
							iDefectConditionContrastMax[i][j][k][l] = 0;

							bUseDefectConditionOuterDist[i][j][k][l] = FALSE;
							iDefectConditionOuterDist[i][j][k][l] = 0;
						}
					}
				}
			}
		}
	}DefectCondition;
	DefectCondition m_StructDefectCondition;

	BOOL m_bShowReviewWindow;
	BOOL m_bReviewImageClick;

	Algorithm* m_pAlgorithm;
	CInspectService* m_pInspectService;
	// TODO : FAI 적절한 곳으로 이관
	// FAI Inspector (Strategy Pattern)
	// std::unique_ptr<IFAIInspector> m_pFAIInspector;
	IFAIInspector* m_pFAIInspector;

	CHandlerService* m_pHandlerService;	// Handler Service 초기화
	CAutoCalService* m_pAutoCalService; // AutoCal (Maybe 조명쪽?) 초기화
	CTriggerManager* m_pTriggerManager; // Trigger (Maybe 조명쪽?) Manager 초기화
	CLightPwmManager* m_pLightPwmManager[VISION_NUMBER_MAX];

	void InitProgress(int range, CString sText);

	MIL_ID m_MilApplication;
	MIL_ID m_MilSystem[2];

	//190625 jwj add ( ADJ Defect priority setting for lotresult.txt log )
	int m_nADJDefectPriority[DEEP_MODEL_NUM * MAX_ADJ_CONNECT_NUM];

	void ReadEquipModelTypeINI();
	int GetModelType() { return m_iModelType; }
	void SetModelType(int iType) { m_iModelType = iType; }
	BOOL IsSameModelType(int iType);


	//////////////////////////////////////////////////////////////////////////	Inspection Type
	void ReadInspectionTypeInfoINI();

	void ReadOverlayViewportINI();

	int m_nOfflineBarcodeImageNo;

	BOOL bImageCompareAutoTrain;
	BOOL bImageCompareTeachingDetect;
	typedef struct //jwj add 20201026
	{
		//CString	m_strCurLotID;
		int  	m_iCurYear;
		int  	m_iCurMonth;
		int  	m_iCurDay;

		void Reset()
		{
			SYSTEMTIME time;
			GetLocalTime(&time);
			m_iCurYear = time.wYear;
			m_iCurMonth = time.wMonth;
			m_iCurDay = time.wDay;
		};

		void GetTime()
		{
			SYSTEMTIME time;
			GetLocalTime(&time);
			m_iCurYear = time.wYear;
			m_iCurMonth = time.wMonth;
			m_iCurDay = time.wDay;
		};

		void SetTime(int iYear, int iMonth, int iDay)
		{
			m_iCurYear = iYear;
			m_iCurMonth = iMonth;
			m_iCurDay = iDay;
		};

	}LOTSTART_TIME_INFO;
	LOTSTART_TIME_INFO m_LotstartInfo[MAX_MAGAZINE_NO];


	//////////////////////////////////////////////////////////////////////////	Equip Model Info
	void ReadEquipModelInfoINI();
	typedef struct
	{
		int m_iUseVisionNo;
		int m_iVisionTrayDlgIndex;
		CString m_strVisionName[VISION_NUMBER_MAX];
		CString m_strVisionName_Short[VISION_NUMBER_MAX];
		CString m_strVisionName_Comm[VISION_NUMBER_MAX][STAGE_NUMBER_MAX];
		BOOL m_bVisionPWM[VISION_NUMBER_MAX];
		BOOL m_bVisionBarcode[VISION_NUMBER_MAX];
		int m_iVisionGrabCircular[VISION_NUMBER_MAX];
		int m_iVisionImageWidth[VISION_NUMBER_MAX];
		int m_iVisionImageHeight[VISION_NUMBER_MAX];
		double m_iPixelResolution[VISION_NUMBER_MAX];
		int m_iMaxInspectionBufferCount[PC_NUMBER_MAX * VISION_NUMBER_MAX];
		CString m_strResultInspectionTypeName[VISION_NUMBER_MAX][MAX_RESULT_IMAGE];
		int m_iBarcodeResultImageNo[VISION_NUMBER_MAX];

		void Reset()
		{
			m_iUseVisionNo = 4;
			for (int nVision = 0; nVision < VISION_NUMBER_MAX; nVision++)
			{
				m_strVisionName[nVision] = "Undefine";
				m_strVisionName_Short[nVision] = "UD";
				for (int nStage = 0; nStage < STAGE_NUMBER_MAX; nStage++)
					m_strVisionName_Comm[nVision][nStage] = "UD";
				m_bVisionPWM[nVision] = FALSE;
				m_bVisionBarcode[nVision] = FALSE;
				m_iVisionGrabCircular[nVision] = GRAB_CIRCULAR_MAX;
				m_iVisionImageWidth[nVision] = DUMMY_IMAGE_SIZE;
				m_iVisionImageHeight[nVision] = DUMMY_IMAGE_SIZE;
				m_iPixelResolution[nVision] = 5;
				m_iMaxInspectionBufferCount[nVision] = 10;
				for (int nResult = 0; nResult < MAX_RESULT_IMAGE; nResult++)
					m_strResultInspectionTypeName[nVision][nResult] = "Unused";
				m_iBarcodeResultImageNo[nVision] = 0;
			}
		};
	}MODEL_DEFINE_INFO;
	MODEL_DEFINE_INFO m_ModelDefineInfo;

	std::optional<SHELLEXECUTEINFOA> StartAISetupSW(std::string mode, int step, std::string model_name);

protected:
	CString		m_szWorkingDir;
	CString		m_szLASDataFolder;
	CString		m_szPCID;

	BOOL		m_bVisionSWRun;
	BOOL		m_bInitComplete;

	int			m_iAppVersion;
	int			m_iSwParamVersion;
	int			m_iHwParamVersion;

	int			m_iModelType;

	void		SetVersion();

	//{{AFX_MSG(CuScanApp)
	afx_msg void OnAppAbout();
	afx_msg void OnHelp();
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_uScan_H__7AC743F0_B386_4D5B_9E6B_09F72F50F422__INCLUDED_)
