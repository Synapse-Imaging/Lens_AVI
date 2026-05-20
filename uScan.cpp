// uScan.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef USE_DUMP_FILE
#include "MinidumpHelp.h" //211203 add
#endif

#include "uScan.h"

#include "MainFrm.h"
#include "uScanDoc.h"
#include "uScanView.h"

#include <cuda_runtime.h>
#include <fstream>
#include <Psapi.h>
#include <winver.h>

#ifdef SNZEROMQ
#include <snzeromq.client/Service/ZeroMQClient.h>
#endif // SNZEROMQ
#include "syai_service/ServiceDirector.h"
#include <sstream>

// FAI Inspector
#include "IFAIInspector.h"

/////////////////////////////////////////////////////////////////////////////
// CuScanApp

BEGIN_MESSAGE_MAP(CuScanApp, CWinApp)
	//{{AFX_MSG_MAP(CuScanApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP, OnHelp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
// Standard print setup command
ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CuScanApp construction

static CRITICAL_SECTION CS_LOTRESULTLOG;
static CRITICAL_SECTION CS_DAYSUMMARYLOG;
static CRITICAL_SECTION CS_COLORINFOLOG;
static CRITICAL_SECTION CS_DEFECTSHAPEFEATURELOG;
static CRITICAL_SECTION CS_DEFECTVARIATIONFEATURELOG;
static CRITICAL_SECTION CS_INSPECTCONDITIONLOG;
static CRITICAL_SECTION CS_ALLDEFECTSRESULTLOG;
static CRITICAL_SECTION CS_AUTOCALLIGHTRESULTLOG;
static CRITICAL_SECTION CS_MEASUREANALYSISLOG;	// 2025.04.08 - v2011 - LeeGW - CM AKC FAI

static CRITICAL_SECTION CS_ADJLOTRESULTLOG;
static CRITICAL_SECTION CS_ADJDAYSUMMARYLOG;
static CRITICAL_SECTION CS_ADJONLYRESULTLOG;

CuScanApp::CuScanApp() : m_pFAIDataManager(CFAIDataManager::GetInstance()),
						 m_StructFaiMeasure(CFAIDataManager::GetInstance().GetRawArray()),
						 m_eventStop(TRUE, TRUE) /*Manual Reset, Initially Owned*/
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	for (int iTempMzNo = 0; iTempMzNo < MAX_MAGAZINE_NO; iTempMzNo++)
		m_strChangeLASFolderPath[iTempMzNo] = "C:\\";

	m_pGFunction = NULL;
	m_pInspectService = NULL;
	m_pAutoCalService = NULL;

	m_iModeSwitch = MODE_INSPECT_VIEW_ADMIN;
	m_iCurTeachVision = VISION_NUMBER_1;
	m_iCurVisionCamType = VISION_NUMBER_2;
	m_iCurStageIndex = STAGE_NUMBER_1;
	m_iCurSelectedResultModule = SELECTED_RESULT_NONE;
	m_iCurAlignCamIndex = ALIGN_CAM_BOTTOM_1;
	m_iAppVersion = 0;
	m_iSwParamVersion = 1000;
	m_iHwParamVersion = 1000;

	m_iModelType = 0;

	m_bShowReviewWindow = FALSE;
	m_bReviewImageClick = FALSE;

	m_iOfflineCurTrayNo = 0;

	int i, j, k;

	g_bMacroStart = FALSE;
	g_nMacroCount = 0;

#ifndef INLINE_MODE
	InitializeCriticalSection(&m_csBarcodeRead);
#endif

#ifdef USE_SUAKIT
	for (int i = 0; i < MAX_MODULE_NUM; i++)
		g_arriThreadCnt[i] = 0;

	for (i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
		for (j = 0; j < MAX_MULTI_CONNECTION_NUM; j++)
			InitializeCriticalSection(&m_csSendADJ[i][j]);

	InitializeCriticalSection(&m_csADJResultLog);
	InitializeCriticalSection(&m_csADJAliveCheck);

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		for (int j = 0; j < MAX_MODULE_NUM; j++)
		{
			iADJLogResult[i][j] = -1;		//ADJ에서 결과에 대한 로그 취합할 때는 0-> NG 1-> G   딥러닝으로 판단하지 않은 모듈 -> -1
			strADJLog[i][j] = _T("");
			strADJDefectCode[i][j] = _T("");
			m_nADJCurDefectPriority[i][j] = DEEP_MODEL_NUM * MAX_ADJ_CONNECT_NUM;
			m_nADJModelIDApplyPriority[i][j] = 0;
		}
	}
	for (int k = 0; k < VISION_NUMBER_MAX; k++)
		for (int i = 0; i < MAX_MODEL_NUM; i++)
			for (int j = 0; j < MAX_MODULE_NUM; j++)
				iADJModelResult[k][i][j] = -1;

#endif

	for (i = 0; i < VISION_NUMBER_MAX; i++)
		m_iVisionInspectThreadNumber[i] = 1;

	m_iMaxRosSaveDefect = 0;

	m_bMirrorPositionChecked = FALSE;
	GenEmptyObj(&m_HMirrorAlignResultRgn);

	for (i = 0; i < MAX_MAGAZINE_NO; i++) //jwj add 20201026
		m_LotstartInfo[i].Reset();

	m_nOfflineBarcodeImageNo = 1;

	m_enumCurSaveLASStatus = eLASINIT;

	m_iTeachingModuleNo = TEACHING_MODULE_NO_UNDEFINED;
	m_iLastSelectVisionCamType = -1;
	m_iLastSelectPcVision = -1;
	// 기존 ResetFAI 유지
	m_pFAIDataManager.ResetTeachingMeasure();

	m_pCameraManagerSpecularMaster = NULL;

	for (i = 0; i < TOTAL_SUB_CAM; i++)
		m_pSubCameraManager[i] = NULL;

	m_pInspectLibraryDataManager = NULL;
	m_pLightLibraryDataManager = NULL;

	for (i = 0; i < MAX_MAGAZINE_NO; i++)
	{
		g_iGrabFailCount[i] = 0;
		g_iDualModelData[i] = MODEL_DATA_1;
	}

	for (i = 0; i < MODEL_DATA_MAX; i++)
		g_iDualModelDataRunCheck[i] = 0;

	strAutoSettingMode = "";
	iAutoSettingCountCurrent = 0;
	iAutoSettingCountEnd = 1;

	for (i = 0; i < VISION_NUMBER_MAX; i++)
	{
		bVariationModelSaveCheck[i] = FALSE;
		bTemplateModelSaveCheck[i] = FALSE;
	}

	for (i = 0; i < VISION_NUMBER_MAX; i++)
		m_pLightPwmManager[i] = NULL;

#ifdef SNZEROMQ
	m_bAutoROISegAvailable = TRUE;
#endif

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CuScanApp object

CuScanApp theApp;

void main()
{
	int device_count = 0;
	cudaError_t err = cudaGetDeviceCount(&device_count);

	if (err != cudaSuccess) {
		std::cerr << "cudaGetDeviceCount 실패: " << cudaGetErrorString(err) << std::endl;
		return;
	}

	std::cout << "총 GPU 장치 수: " << device_count << std::endl;
	int last_cuda_device_index;
	for (int device = 0; device < device_count; ++device) {
		cudaDeviceProp prop;
		cudaGetDeviceProperties(&prop, device);

		std::cout << "\n[GPU Index " << device << "]" << std::endl;
		std::cout << "  이름           : " << prop.name << std::endl;
		std::cout << "  총 글로벌 메모리: " << prop.totalGlobalMem / (1024 * 1024) << " MB" << std::endl;
		std::cout << "  SM 수          : " << prop.multiProcessorCount << std::endl;
		std::cout << "  CUDA Capability: " << prop.major << "." << prop.minor << std::endl;
		std::cout << "  최대 쓰레드 수  : " << prop.maxThreadsPerBlock << std::endl;
		last_cuda_device_index++;
	}
	THEAPP.m_roi_image_save_thread_pool = std::make_shared<AIService::ThreadPool>(); // ROI 이미지 저장 thread 관리 class
	THEAPP.m_roi_image_save_thread_pool->init(1);
	THEAPP.m_variation_feature_log_thread_pool = std::make_shared<AIService::ThreadPool>(); // Variation Feature 로그 저장 thread 관리 class
	THEAPP.m_variation_feature_log_thread_pool->init(1);

	std::string logger_name;
#ifdef LGITAI
#pragma region info file read
	std::filesystem::path lgit_ai_root = "C:\\lgit"; // 파일 위치
	std::filesystem::path log_root = lgit_ai_root / "logs"; // log 파일 위치

	std::filesystem::path ai_inspector_setup_file_dir = lgit_ai_root / "setup";
	std::filesystem::path ai_inspector_manager_file_name = "ai_inspector_manager_setup.json";
	std::filesystem::path ai_inspector_manager_file_path = ai_inspector_setup_file_dir / ai_inspector_manager_file_name;

	// 폴더 설정
	try {
		if (!std::filesystem::exists(ai_inspector_setup_file_dir)) {
			std::filesystem::create_directories(ai_inspector_setup_file_dir);
		}
	}
	catch (...) {
		throw std::runtime_error("설정 폴더 만들다가 죽어버려");
	}
	try {
		if (!std::filesystem::exists(ai_inspector_manager_file_path)) { // 설정 파일 없음
			// 없으면 초기값으로 설정 파일 생성
			LGIT::Model::InspectorManagerSetup temp_setup;
			// 1번 망 초기 setup
			temp_setup.inspector_setup.thread_number = 1;
			temp_setup.inspector_setup.log_level = 1;
			temp_setup.inspector_setup.inspector_setup_file_path = "C:\\lgit\\model1_setup\\thresholds.json";
			// 2번 망 초기 setup
			temp_setup.inspector2_setup.thread_number = 1;
			temp_setup.inspector2_setup.log_level = 1;
			temp_setup.inspector2_setup.inspector_model_dir = "C:\\lgit\\model2_setup";
			// svm setup
			temp_setup.inspector_svm_setup.thread_number = 1;
			temp_setup.inspector_svm_setup.log_level = 1;
			temp_setup.inspector_svm_setup.inspector_model_dir = "C:\\lgit\\model_svm_setup";
			// seg setup
			temp_setup.inspector_seg_setup.thread_number = 1;
			temp_setup.inspector_seg_setup.log_level = 1;
			temp_setup.inspector_seg_setup.inspector_model_dir = "C:\\lgit\\model_seg_setup";
			// val setup
			temp_setup.inspector_val_setup.thread_number = 1;
			temp_setup.inspector_val_setup.log_level = 1;
			temp_setup.inspector_val_setup.inspector_model_dir = "C:\\lgit\\model_val_setup";
			// model 파일 검색
			std::filesystem::path model_dir = "C:\\lgit\\model1_setup";
			std::vector<std::filesystem::path> model_list = LGIT::ModelInference::GetBinFiles(model_dir);
			// 검색된 모델 추가
			temp_setup.reset_inspector1_model_list(model_list);
			// 저장
			temp_setup.save_to_file(ai_inspector_manager_file_path.string());
		}
	}
	catch (...) {
		throw std::runtime_error("설정 파일 읽다가 죽어버려");
	}
	LGIT::Model::InspectorManagerSetup setup = LGIT::Model::InspectorManagerSetup::load_from_file(ai_inspector_manager_file_path.string());
	// model 파일 검색
	std::filesystem::path model_dir = "C:\\lgit\\model1_setup";
	std::vector<std::filesystem::path> model_list = LGIT::ModelInference::GetBinFiles(model_dir);
	// 검색된 모델 추가
	setup.reset_inspector1_model_list(model_list);
	// 저장
	setup.save_to_file(ai_inspector_manager_file_path.string());

#pragma endregion info file read
#pragma region ai inspector init
	using ConditionArray = std::decay_t<decltype(g_strInspectionTypeName)>;
	ConditionArray condition_name_list = g_strInspectionTypeName;
	ConditionArray condition_short_name_list = g_strInspectionTypeName_Short;
	/*
	constexpr size_t g_str_condition_name_size = std::size(g_strConditionName);
	std::array<std::string, 23> condition_name_list;
	std::array<std::string, 23> condition_short_name_list;
	USES_CONVERSION;
	std::transform(
		g_strConditionName, g_strConditionName + g_str_condition_name_size,
		condition_name_list.begin(),
		[](const CString& str) {
		return std::string(CT2A(str));
	}
	);
	// 코드와 동일 컴파일러 최적화 ==> for (size_t i = 0; i < g_condition_name_size; ++i) {
	//	condition_name_list[i] = CT2A(g_strConditionName[i]);
	//}
	std::copy(
		g_strInspectionTypeName_Short.begin(),
		g_strInspectionTypeName_Short.end(),
		condition_short_name_list.begin()
	);
	*/

	// THEAPP.g_strModelTypeName[THEAPP.GetModelType()]

	THEAPP.m_lgAIInspector = std::make_shared<LGIT::LAIInspector>(
		setup.inspector_setup.thread_number,
		condition_name_list,
		condition_short_name_list,
		setup.inspector_setup.model_path_list,
		setup.inspector_setup.inspector_setup_file_path,
		setup.inspector_setup.log_level,
		log_root,
		last_cuda_device_index);
	THEAPP.m_lgAIInspector2 = std::make_shared<LGIT::LAIInspector2>(
		setup.inspector2_setup.thread_number,
		condition_name_list,
		condition_short_name_list,
		setup.inspector2_setup.inspector_model_dir,
		setup.inspector2_setup.log_level,
		log_root,
		last_cuda_device_index);
	THEAPP.m_lgAIInspectorSVM = std::make_shared<LGIT::LAIInspectorSVM>(
		setup.inspector_svm_setup.thread_number,
		condition_name_list,
		condition_short_name_list,
		setup.inspector_svm_setup.inspector_model_dir,
		setup.inspector_svm_setup.log_level,
		log_root,
		last_cuda_device_index);
	THEAPP.m_lgAIInspectorSEG = std::make_shared<LGIT::LAIInspectorSEG>(
		setup.inspector_seg_setup.thread_number,
		condition_name_list,
		condition_short_name_list,
		setup.inspector_seg_setup.inspector_model_dir,
		setup.inspector_seg_setup.log_level,
		log_root,
		last_cuda_device_index);
	THEAPP.m_lgAIInspectorVAL = std::make_shared<LGIT::LAIInspectorVAL>(
		setup.inspector_val_setup.thread_number,
		condition_name_list,
		condition_short_name_list,
		setup.inspector_val_setup.inspector_model_dir,
		setup.inspector_val_setup.log_level,
		log_root,
		last_cuda_device_index);
	THEAPP.m_lg_inspectorManager = std::make_shared<LGIT::InspectorManager>(
		setup.thread_number,
		THEAPP.m_lgAIInspector,
		THEAPP.m_lgAIInspector2,
		THEAPP.m_TCPClientService,
		THEAPP.m_lgAIInspectorSVM,
		THEAPP.m_lgAIInspectorSEG,
		THEAPP.m_lgAIInspectorVAL,
		setup.log_level,
		setup.is_use_segmentation_visualization,
		log_root);
	THEAPP.m_isSetLGITAI = true;
#pragma endregion ai inspector init
#pragma region lg ai logger setup
	logger_name = "main_inspector_logger";
	std::filesystem::path log_folder = log_root / logger_name;

	try {
		std::filesystem::create_directories(log_folder);
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to create log directory: " << e.what() << '\n';
	}

	// 기존 로거 존재 시 재사용
	auto logger = spdlog::get(logger_name);
	if (logger) {
		THEAPP.m_lg_inspector_logger = logger;
		return;
	}

	// 비동기 로깅 풀 생성
	if (!spdlog::thread_pool()) {
		spdlog::init_thread_pool(8192, 1);
	}

	// 날짜 문자열 생성
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::tm tm;
	localtime_s(&tm, &now_c);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y%m%d");
	std::string date_str = oss.str();

	// 파일명에 날짜 포함
	std::string base_filename = "MainInspectorLog_" + date_str + ".txt";
	std::filesystem::path full_log_path = log_folder / base_filename;

	// rotating_logger 사용 (파일명에 날짜 포함)
	THEAPP.m_lg_inspector_logger = spdlog::rotating_logger_mt<spdlog::async_factory>(
		logger_name,
		full_log_path.string(),
		1024ull * 1024 * 500, // 500MB
		10                    // 최대 10개 보관 (하루 5GB 제한 등)
	);

	// 로그 포맷 설정
	THEAPP.m_lg_inspector_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");
	THEAPP.m_lg_inspector_logger->set_level(spdlog::level::debug);
	THEAPP.m_lg_inspector_logger->flush_on(spdlog::level::info);
	THEAPP.m_lg_inspector_logger->info("Logger initialized for date: {}", date_str);
#pragma endregion


#endif // LGITAI
#pragma region syai logger setup
	std::filesystem::path base_dir = "C:\\syai_avi"; // 파일 위치
	logger_name = "MS";
	THEAPP.InitializeLogger(logger_name.c_str(), base_dir.string().c_str(), static_cast<int>(syai::runtime::domain::config::LogLevel::INFO_SY));
#pragma endregion
#ifdef SYAI
	if (!THEAPP.InitializeSYAI()) {

	}
#endif // SYAI
}

/////////////////////////////////////////////////////////////////////////////
// CuScanApp initialization

BOOL CuScanApp::InitInstance()
{
	if (!AfxSocketInit()) return FALSE;	// UDP Socket 초기화

	srand((unsigned)time(NULL));

	BOOL bRestartCheck = FALSE;
#ifdef INLINE_MODE
	if (CheckProcessExist() > 1)
	{
		if (m_iPGMLanguageSelect == 0)
			strMessageBox.Format("Cosmetic AVI 시스템을 재시작합니다.");
		else
			strMessageBox.Format("Cosmetic AVI System Restart.");
		AfxMessageBox(strMessageBox, MB_SYSTEMMODAL);

		bRestartCheck = TRUE;
	}
#endif

	InitializeCriticalSection(&CS_LOTRESULTLOG);
	InitializeCriticalSection(&CS_DAYSUMMARYLOG);
	InitializeCriticalSection(&CS_COLORINFOLOG);
	InitializeCriticalSection(&CS_DEFECTSHAPEFEATURELOG);
	InitializeCriticalSection(&CS_DEFECTVARIATIONFEATURELOG);
	InitializeCriticalSection(&CS_INSPECTCONDITIONLOG);
	InitializeCriticalSection(&CS_ALLDEFECTSRESULTLOG);
	InitializeCriticalSection(&CS_AUTOCALLIGHTRESULTLOG);
	InitializeCriticalSection(&CS_MEASUREANALYSISLOG);	// 2025.04.08 - v2011 - LeeGW - CM AKC FAI

#ifdef USE_SUAKIT
	InitializeCriticalSection(&CS_ADJLOTRESULTLOG);
	InitializeCriticalSection(&CS_ADJDAYSUMMARYLOG);
	InitializeCriticalSection(&CS_ADJONLYRESULTLOG);
#endif

	SetCheck("give_error");
	SetSystem("store_empty_region", "true");
	SetSystem("clock_mode", "elapsed_time");
	SetSystem("max_connection", 200);
	SetSystem("clip_region", "false");
	SetSystem("no_object_result", "true");
	SetSystem("empty_region_result", "true");
	ResetObjDb(30000, 30000, 0);
	SetSystem("parallelize_operators", "false");
	SetSystem("reentrant", "true");
	SetSystem("global_mem_cache", "exclusive");
	SetSystem("temporary_mem_cache", "exclusive");

	//////////////////////////////////////////////////////////////////////////

#ifdef SINGLE_LENS
	free((void*)m_pszAppName);
	m_pszAppName = _tcsdup(_T("Single Lens AVI"));
#elif defined(ASSY_LENS)
	free((void*)m_pszAppName);
	m_pszAppName = _tcsdup(_T("Ass'y Lens AVI"));
#endif

	m_bInitComplete = FALSE;
	AfxEnableControlContainer();
	AfxInitRichEdit(); // 리치에디트 초기화..


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	AfxOleInit();

	m_bVisionSWRun = TRUE;

	// 실행 파일 전체 경로 구하기
	TCHAR szCurDir[MAX_PATH];
	TCHAR* lpFilePart = NULL;
	GetModuleFileName(NULL, szCurDir, MAX_PATH);
	GetFullPathName(szCurDir, MAX_PATH, szCurDir, &lpFilePart);
	PathRemoveFileSpec(szCurDir);
	SetCurrentDirectory(szCurDir);

	::GetCurrentDirectory(MAX_PATH - 1, szCurDir);
	m_szWorkingDir = szCurDir;

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	SetVersion();

#ifdef USE_DUMP_FILE
	// Dump생성
	MinidumpHelp test;
	test.install_self_mini_dump();
#endif

	ReadPGMLanguageSelectINI();
	ReadModelNameCodeINI();
	ReadEquipModelTypeINI();
	ReadEquipModelInfoINI();
	ReadInspectionTypeInfoINI();
	ReadPreferenceINI();
	ReadOverlayViewportINI();
	ReadDefectReViewInfoINI();
	ReadFaiDefectReViewInfo();
	//ReadADJReViewInfoINI();
	//ReadAdjModelAssign();

	InitializeVisionLogger();

	if (bRestartCheck)
	{
		strLog.Format("Universal AVI system re-start");
		m_log_system->warn("{}", LOG_CSTR(strLog));
	}

#ifdef INLINE_MODE
	strLog.Format("★★★★★★★★★★★★★★★★★★ Universal AVI INLINE system start ★★★★★★★★★★★★★★★★★★");
	m_log_system->warn("{}", LOG_CSTR(strLog));
#else
	strLog.Format("★★★★★★★★★★★★★★★★★★ Universal AVI OFFLINE system start ★★★★★★★★★★★★★★★★★★");
	m_log_system->warn("{}", LOG_CSTR(strLog));
#endif
	CString strEquipModel = g_strModelTypeName[GetModelType()];

	int iVisionCamType;
	CString sVisionCamType;

	strLog.Format("Vision software version: %d", GetProgramVersion());
	m_log_system->warn("{}", LOG_CSTR(strLog));

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CuScanDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CuScanView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// For Release

	m_bInitComplete = TRUE;

	m_eventStop.ResetEvent();

	m_pGFunction = CGFunction::GetInstance();

	///////// Defect Name 초기화
	CString strDefectNameFileName = GetWorkingDirectory() + "\\Data\\" + "DefectName.ini";
	CIniFileCS INI_DEFECTNAME(strDefectNameFileName);

	CString sSectionDefectName, sKey, sTemp;
	sSectionDefectName = "Defect_Name";

	for (int ijk_2 = 0; ijk_2 < MAX_DEFECT_NAME; ijk_2++)
	{
		sKey.Format("%d", ijk_2);
		sTemp.Format("res%d", ijk_2);
		m_strDefectName[ijk_2] = INI_DEFECTNAME.Get_String(sSectionDefectName, sKey, sTemp);
	}

	///////// Defect Code 초기화
	CString strDefectCodeFileName = GetWorkingDirectory() + "\\Data\\" + "DefectCode.ini";
	CIniFileCS INI_DEFECTCODE(strDefectCodeFileName);

	CString sSectionDefectCode;
	sSectionDefectCode = "Defect_Code";

	for (int ijk_2 = 0; ijk_2 < MAX_DEFECT_NAME; ijk_2++)
	{
		sKey.Format("%d", ijk_2);
		sTemp.Format("RES%d", ijk_2);
		m_strDefectCode[ijk_2] = INI_DEFECTCODE.Get_String(sSectionDefectCode, sKey, sTemp);
	}

	///////// Vision Thread Number Setting
	CString strVisionThreadFileName = GetWorkingDirectory() + "\\Data\\" + "Thread.ini";
	CIniFileCS INI_THREAD(strVisionThreadFileName);

	CString sSectionThread;
	sSectionThread = "VISION_THREAD_NUMBER";

	for (int ijk_3 = 0; ijk_3 < VISION_NUMBER_MAX; ijk_3++)
	{
		CString sVisionCamType;
		sVisionCamType = m_ModelDefineInfo.m_strVisionName[ijk_3];
		sKey.Format("%s", sVisionCamType);
		m_iVisionInspectThreadNumber[ijk_3] = INI_THREAD.Get_Integer(sSectionThread, sKey, 1);
	}

	//////////////////////////////////////////////////////////////////////////Dlg display
	m_pInspectSummary = CInspectSummary::GetInstance();
	m_pInspectSummary->Show();
	m_pInspectAdminViewDlg = CInspectAdminViewDlg::GetInstance();
	m_pInspectAdminViewToolbarDlg = CInspectAdminViewToolbarDlg::GetInstance();

	m_pInspectViewOverayImageDlg1 = CInspectViewOverayImageDlg1::GetInstance();
	m_pInspectViewOverayImageDlg2 = CInspectViewOverayImageDlg2::GetInstance();
	m_pInspectViewOverayImageDlg3 = CInspectViewOverayImageDlg3::GetInstance();
	m_pInspectViewOverayImageDlg4 = CInspectViewOverayImageDlg4::GetInstance();

	m_pInspectViewOverayImageToolbarDlg1 = CInspectViewOverayImageToolbarDlg1::GetInstance();
	m_pInspectViewOverayImageToolbarDlg2 = CInspectViewOverayImageToolbarDlg2::GetInstance();
	m_pInspectViewOverayImageToolbarDlg3 = CInspectViewOverayImageToolbarDlg3::GetInstance();
	m_pInspectViewOverayImageToolbarDlg4 = CInspectViewOverayImageToolbarDlg4::GetInstance();

	m_pInspectResultDlg = CInspectResultDlg::GetInstance();
	m_pInspectResultDlg->Show();
	m_pDefectListDlg = CDefectListDlg::GetInstance();
	m_pDefectListDlg->Show();

	m_pTabControlDlg = CTabControlDlg::GetInstance();
	m_pTabControlDlg->Show();
	m_pTabControlDlg->ShowWindow(SW_HIDE);

#ifdef USE_SUAKIT
	//m_pADJSendImageHideDlg = CADJSendImageHideDlg::GetInstance();	//adj dlg for sendmessage(to do MFC send on main thread)
	//m_hwndADJSend = m_pADJSendImageHideDlg->GetSafeHwnd();
	//m_pADJSendImageHideDlg->ShowWindow(SW_HIDE);
#endif
	////////////////////////////////////////////////////////////////////////////////////
	strLog.Format("Create dialog instance complete");
	m_log_system->debug("{}", LOG_CSTR(strLog));

	WriteDummyImage();

	int iDualModelData;
	int iPcVisionNo;

	for (iDualModelData = 0; iDualModelData < MODEL_DATA_MAX; iDualModelData++)
	{
		for (iPcVisionNo = VISION_NUMBER_1; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
		{
			m_pDualModelDataManager[iDualModelData][iPcVisionNo] = new CModelDataManager;
			m_pDualModelDataManager[iDualModelData][iPcVisionNo]->SetModelIdx(iPcVisionNo);
		}
	}

	m_pAlgorithm = Algorithm::GetInstance();
	strLog.Format("Create ModelDataManager & Algorithm class complete");
	m_log_system->debug("{}", LOG_CSTR(strLog));

	for (iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
		m_pDualCameraManager[iPcVisionNo] = new CCameraManager;

	m_pDualCameraManager[VISION_NUMBER_1]->SetVisionCamName(VISION_NUMBER_1);
	m_pDualCameraManager[VISION_NUMBER_2]->SetVisionCamName(VISION_NUMBER_2);
	m_pDualCameraManager[VISION_NUMBER_3]->SetVisionCamName(VISION_NUMBER_3);
	m_pDualCameraManager[VISION_NUMBER_4]->SetVisionCamName(VISION_NUMBER_4);

#ifdef ASSY_LENS
	for (iPcVisionNo = SUB_CAM_1; iPcVisionNo < TOTAL_SUB_CAM; iPcVisionNo++)
		m_pSubCameraManager[iPcVisionNo] = new CCameraManager;

	m_pSubCameraManager[SUB_CAM_1]->SetVisionCamName(VISION_NUMBER_4_2);
	m_pSubCameraManager[SUB_CAM_2]->SetVisionCamName(VISION_NUMBER_4_3);
#endif

	for (iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
	{
		if (m_ModelDefineInfo.m_bVisionPWM[iPcVisionNo])
			m_pLightPwmManager[iPcVisionNo] = new CLightPwmManager;
	}

	for (iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
		m_pCalDataService_N[iPcVisionNo] = new CCalDataService;

	m_pCalDataService_N[VISION_NUMBER_1]->SetVisionCamName(VISION_NUMBER_1);
	m_pCalDataService_N[VISION_NUMBER_2]->SetVisionCamName(VISION_NUMBER_2);
	m_pCalDataService_N[VISION_NUMBER_3]->SetVisionCamName(VISION_NUMBER_3);
	m_pCalDataService_N[VISION_NUMBER_4]->SetVisionCamName(VISION_NUMBER_4);

	m_pTriggerManager = CTriggerManager::GetInstance();
	m_pInspectService = CInspectService::GetInstance();
	m_pAutoCalService = CAutoCalService::GetInstance();

	m_pHandlerService = CHandlerService::GetInstance();

#if !defined(SINGLE_LENS) && !defined(ASSY_LENS)
	// Light Controller
	CString sLightControllerIP;
	sVisionCamType = m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_1];
	if (sVisionCamType != "Undefine" && !m_ModelDefineInfo.m_bVisionPWM[VISION_NUMBER_1])
	{
		sLightControllerIP.Format("%d.%d.%d.%d", Struct_PreferenceStruct.m_iVision1LightIP1, Struct_PreferenceStruct.m_iVision1LightIP2, Struct_PreferenceStruct.m_iVision1LightIP3, Struct_PreferenceStruct.m_iVision1LightIP4);
		m_pHandlerService->SetTCP_IP(LIGHT_CONTROLLER_NUMBER_1, sLightControllerIP);
		m_pHandlerService->SetTCP_PORT(LIGHT_CONTROLLER_NUMBER_1, Struct_PreferenceStruct.m_iVision1LightPort);
	}

	sVisionCamType = m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_2];
	if (sVisionCamType != "Undefine" && !m_ModelDefineInfo.m_bVisionPWM[VISION_NUMBER_2])
	{
		sLightControllerIP.Format("%d.%d.%d.%d", Struct_PreferenceStruct.m_iVision2LightIP1, Struct_PreferenceStruct.m_iVision2LightIP2, Struct_PreferenceStruct.m_iVision2LightIP3, Struct_PreferenceStruct.m_iVision2LightIP4);
		m_pHandlerService->SetTCP_IP(LIGHT_CONTROLLER_NUMBER_2, sLightControllerIP);
		m_pHandlerService->SetTCP_PORT(LIGHT_CONTROLLER_NUMBER_2, Struct_PreferenceStruct.m_iVision2LightPort);
	}

	sVisionCamType = m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_3];
	if (sVisionCamType != "Undefine" && !m_ModelDefineInfo.m_bVisionPWM[VISION_NUMBER_3])
	{
		sLightControllerIP.Format("%d.%d.%d.%d", Struct_PreferenceStruct.m_iVision3LightIP1, Struct_PreferenceStruct.m_iVision3LightIP2, Struct_PreferenceStruct.m_iVision3LightIP3, Struct_PreferenceStruct.m_iVision3LightIP4);
		m_pHandlerService->SetTCP_IP(LIGHT_CONTROLLER_NUMBER_3, sLightControllerIP);
		m_pHandlerService->SetTCP_PORT(LIGHT_CONTROLLER_NUMBER_3, Struct_PreferenceStruct.m_iVision3LightPort);
	}

	sVisionCamType = m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_4];
	if (sVisionCamType != "Undefine" && !m_ModelDefineInfo.m_bVisionPWM[VISION_NUMBER_4])
	{
		sLightControllerIP.Format("%d.%d.%d.%d", Struct_PreferenceStruct.m_iVision4LightIP1, Struct_PreferenceStruct.m_iVision4LightIP2, Struct_PreferenceStruct.m_iVision4LightIP3, Struct_PreferenceStruct.m_iVision4LightIP4);
		m_pHandlerService->SetTCP_IP(LIGHT_CONTROLLER_NUMBER_4, sLightControllerIP);
		m_pHandlerService->SetTCP_PORT(LIGHT_CONTROLLER_NUMBER_4, Struct_PreferenceStruct.m_iVision4LightPort);
	}
#endif

	BOOL bConnectCheck = FALSE;
	bConnectCheck = m_pHandlerService->Initialize_TcpHandler(FALSE);

	if (!bConnectCheck)
	{
		m_pHandlerService->Terminate_TcpHandler();
		m_pHandlerService->Initialize_TcpHandler(TRUE);
	}

	strLog.Format("Create HandlerService class complete");
	m_log_system->debug("{}", LOG_CSTR(strLog));

#ifdef SNZEROMQ

	//m_bAutoROISegAvailable = InitializeZeroMQ();

#endif

	///////// 마지막 모델 불러오기
	CString strStatusFileName = GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS INI(strStatusFileName);
	CString strSection = "Status";

	for (iDualModelData = 0; iDualModelData < MODEL_DATA_MAX; iDualModelData++)
	{
		m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName = INI.Get_String(strSection, "ModelID", "default");
		m_pDualModelDataManager[iDualModelData][VISION_NUMBER_2]->m_sModelName = m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;
		m_pDualModelDataManager[iDualModelData][VISION_NUMBER_3]->m_sModelName = m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;
		m_pDualModelDataManager[iDualModelData][VISION_NUMBER_4]->m_sModelName = m_pDualModelDataManager[iDualModelData][VISION_NUMBER_1]->m_sModelName;
	}

	CString strRecipeName = m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName;
	CString strEquipModelTypeCheck;
	strEquipModelTypeCheck = strRecipeName.Mid(strRecipeName.Find('_') + 1);
	strEquipModelTypeCheck = strEquipModelTypeCheck.Mid(0, strEquipModelTypeCheck.Find('_'));

	for (int i = 0; i <= MODEL_TYPE_MAX; i++)
	{
		if (i == MODEL_TYPE_MAX)
		{
			AfxMessageBox("비전 모델 파일에서 설비 검사 모델 정보를 찾을 수 없습니다. 설정 창의 설비 검사 모델을 직접 선택해주세요.", MB_ICONERROR | MB_SYSTEMMODAL);

			strLog.Format("Model type data change fail(initial)(Can't find model type), Recipe type: %s", strEquipModelTypeCheck);
			m_log_system->warn("{}", LOG_CSTR(strLog));

			break;
		}

		if (strEquipModelTypeCheck == g_strModelTypeName[i])
		{
			if (!IsSameModelType(i))
			{
				SetModelType(i);
				ReadInspectionTypeInfoINI();
				ReadOverlayViewportINI();
				ReadDefectReViewInfoINI();
				ReadFaiDefectReViewInfo();

				strLog.Format("Model type data change complete(initial), Model type: %s", g_strModelTypeName[i]);
				m_log_system->info("{}", LOG_CSTR(strLog));
			}

			break;
		}
	}

	BOOL bModelExist1, bModelExist2;
	for (iDualModelData = 0; iDualModelData < MODEL_DATA_MAX; iDualModelData++)
	{
		for (iPcVisionNo = VISION_NUMBER_MAX - 1; iPcVisionNo >= VISION_NUMBER_1; iPcVisionNo--)
		{
			sVisionCamType = m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
			if (sVisionCamType == "Undefine")
				continue;

			bModelExist1 = m_pDualModelDataManager[iDualModelData][iPcVisionNo]->LoadModelSWBaseData();
			bModelExist2 = m_pDualModelDataManager[iDualModelData][iPcVisionNo]->LoadModelSWData();

			bVariationModelSaveCheck[iPcVisionNo] = FALSE;
			bTemplateModelSaveCheck[iPcVisionNo] = FALSE;
		}
	}

	if (!bModelExist1 || !bModelExist2)
	{
		AfxMessageBox("비전 모델 파일을 찾을 수 없습니다. Model 폴더 내 모델 파일을 확인해주세요.", MB_ICONERROR | MB_SYSTEMMODAL);

		strLog.Format("Vision recipe load fail(initial), Recipe name: %s", m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);
		m_log_system->warn("{}", LOG_CSTR(strLog));

		if (m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName.GetLength() < 35)
			m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
		else
			m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 8, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_VISION_MODEL_NAME, m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);
	}
	else
	{
		strLog.Format("Vision recipe load complete(initial), Recipe name: %s", m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);
		m_log_system->info("{}", LOG_CSTR(strLog));

		if (m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName.GetLength() < 35)
			m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
		else
			m_pInspectResultDlg->m_LabelVisionModelName.Init_Ctrl(_T("Arial Black"), 8, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_VISION_MODEL_NAME, m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName);
	}

#ifdef INLINE_MODE
	MappAlloc(M_NULL, M_DEFAULT, &m_MilApplication);
	MappTimer(M_DEFAULT, M_TIMER_RESET + M_SYNCHRONOUS, M_NULL);
	MsysAlloc(M_DEFAULT, M_SYSTEM_RAPIXOCXP, M_DEV0, M_DEFAULT, &m_MilSystem[0]);
	if ((strEquipModel == "BOI" || strEquipModel == "BOS" || strEquipModel == "KRIOS") &&
		(Struct_PreferenceStruct.m_iPCType == PC_NUMBER_1 || Struct_PreferenceStruct.m_iPCType == PC_NUMBER_2))
		MsysAlloc(M_DEFAULT, M_SYSTEM_RAPIXOCXP, M_DEV1, M_DEFAULT, &m_MilSystem[1]);
	else if (strEquipModel == "ASSY_LENS")
		MsysAlloc(M_DEFAULT, M_SYSTEM_RAPIXOCXP, M_DEV1, M_DEFAULT, &m_MilSystem[1]);

	m_pDualCameraManager[VISION_NUMBER_1]->SetInitDiffusedVision();
	for (iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
	{
		sVisionCamType = m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
		if (sVisionCamType == "Undefine")
			continue;

		if (sVisionCamType == "Align")
		{
			BOOL bConnectCheck = FALSE;
			bConnectCheck = m_pDualCameraManager[iPcVisionNo]->InitMvEthernetCam("");
			if (!bConnectCheck)
				AfxMessageBox("얼라인 카메라 연결을 실패했습니다. SW를 재실행해주세요.", MB_ICONERROR | MB_SYSTEMMODAL);
		}
		else
		{
#ifdef SINGLE_LENS
			if (iPcVisionNo < 2)
				m_pDualCameraManager[iPcVisionNo]->InitGrabInterface_Mono(m_MilSystem[0]);
			else
				m_pDualCameraManager[iPcVisionNo]->InitGrabInterface_Mono(m_MilSystem[1]);
#elif defined(ASSY_LENS)
			if (iPcVisionNo < 2)
				m_pDualCameraManager[iPcVisionNo]->InitGrabInterface_Mono(m_MilSystem[0]);
			else
			{
				m_pDualCameraManager[iPcVisionNo]->InitGrabInterface_Mono(m_MilSystem[1]);

				if (iPcVisionNo == (VISION_NUMBER_MAX - 1))
				{
					m_pSubCameraManager[SUB_CAM_1]->InitGrabInterface_Mono(m_MilSystem[1]);
					m_pSubCameraManager[SUB_CAM_2]->InitGrabInterface_Mono(m_MilSystem[1]);
				}
			}
#else
			if (iPcVisionNo < 2)
				m_pDualCameraManager[iPcVisionNo]->InitGrabInterface(m_MilSystem[0]);
			else
				m_pDualCameraManager[iPcVisionNo]->InitGrabInterface(m_MilSystem[1]);
#endif
		}
	}

	m_pCameraManager = m_pDualCameraManager[VISION_NUMBER_1];

	strLog.Format("Create CameraManager class complete");
	m_log_system->debug("{}", LOG_CSTR(strLog));

	m_pTriggerManager->Initialize();
	m_pTriggerManager->SetTriggerSleepTime(Struct_PreferenceStruct.m_iTriggerSleepTime);
	strLog.Format("Create TriggerManager class complete");
	m_log_system->debug("{}", LOG_CSTR(strLog));
#else
	m_pDualCameraManager[VISION_NUMBER_1]->SetInitDiffusedVision();
	for (iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
	{
		sVisionCamType = m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
		if (sVisionCamType == "Undefine" || sVisionCamType == "Align")
			continue;

		m_pDualCameraManager[iPcVisionNo]->InitGrabInterface(m_MilSystem[0]);

#ifdef ASSY_LENS
		if (iPcVisionNo == (VISION_NUMBER_MAX - 1))
		{
			m_pSubCameraManager[SUB_CAM_1]->SetCamImageSizeX(m_pDualCameraManager[iPcVisionNo]->GetCamImageSizeX());
			m_pSubCameraManager[SUB_CAM_1]->SetCamImageSizeY(m_pDualCameraManager[iPcVisionNo]->GetCamImageSizeY());

			m_pSubCameraManager[SUB_CAM_2]->SetCamImageSizeX(m_pDualCameraManager[iPcVisionNo]->GetCamImageSizeX());
			m_pSubCameraManager[SUB_CAM_2]->SetCamImageSizeY(m_pDualCameraManager[iPcVisionNo]->GetCamImageSizeY());
		}
#endif
	}
#endif

	for (iPcVisionNo = VISION_NUMBER_MAX - 1; iPcVisionNo >= VISION_NUMBER_1; iPcVisionNo--)
	{
		sVisionCamType = m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
		if (sVisionCamType == "Undefine")
			continue;

		bModelExist1 = m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->LoadModelHWBaseData();
		bModelExist2 = m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->LoadModelHWData();
	}

	CString strOpticalModelName;
	strOpticalModelName.Format("%s_%s_PC%d", Struct_PreferenceStruct.m_strEquipNo, g_strModelTypeName[GetModelType()], Struct_PreferenceStruct.m_iPCType + 1);
	if (!bModelExist1 || !bModelExist2)
	{
		AfxMessageBox("광학 모델 파일을 찾을 수 없습니다. Optical 폴더 내 모델 파일을 확인해주세요.", MB_ICONERROR | MB_SYSTEMMODAL);

		strLog.Format("Optical recipe load fail(initial), Recipe name: %s", strOpticalModelName);
		m_log_system->warn("{}", LOG_CSTR(strLog));

		m_pInspectResultDlg->m_LabelOpticalModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_OPTICAL_MODEL_NAME, strOpticalModelName);
	}
	else
	{
		strLog.Format("Optical recipe load complete(initial), Recipe name: %s", strOpticalModelName);
		m_log_system->info("{}", LOG_CSTR(strLog));

		m_pInspectResultDlg->m_LabelOpticalModelName.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_OPTICAL_MODEL_NAME, strOpticalModelName);
	}

#ifdef DEFECT_CONDITION
	BOOL bLoadConditionCheck;
	bLoadConditionCheck = CountPerAreaCondition();
	if (bLoadConditionCheck)
	{
		strLog.Format("Count per area condition(json) load complete(initial)");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}
	else
	{
		LoadCountPerAreaCondition_ini();
		strLog.Format("Count per area condition(ini) load complete(initial)");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}
#endif

	ShowVersionText();

	// Assign 1st Vision
	m_pModelDataManager = m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1];
	m_pModelDataManager->UpdateTeachParamView();

	m_pInspectLibraryDataManager = CInspectLibraryDataManager::GetInstance();
	m_pInspectLibraryDataManager->Initialize();

	m_pLightLibraryDataManager = CLightLibraryDataManager::GetInstance();
	m_pLightLibraryDataManager->Initialize();

	m_pInspectAdminViewHideDlg1 = CInspectAdminViewHideDlg1::GetInstance();
	m_pInspectAdminViewHideDlg1->Show();
	m_pInspectAdminViewHideDlg1->ShowWindow(SW_SHOWNA);

	m_pInspectAdminViewHideDlg2 = CInspectAdminViewHideDlg2::GetInstance();
	m_pInspectAdminViewHideDlg2->Show();
	m_pInspectAdminViewHideDlg2->ShowWindow(SW_SHOWNA);

	m_pInspectAdminViewHideDlg3 = CInspectAdminViewHideDlg3::GetInstance();
	m_pInspectAdminViewHideDlg3->Show();
	m_pInspectAdminViewHideDlg3->ShowWindow(SW_SHOWNA);

	m_pInspectAdminViewHideDlg4 = CInspectAdminViewHideDlg4::GetInstance();
	m_pInspectAdminViewHideDlg4->Show();
	m_pInspectAdminViewHideDlg4->ShowWindow(SW_SHOWNA);

	/////////////////////////////// 모델 정보부터 로딩 - 모델의 갯수를 알아야 트레이 안에 모듈 갯수가 나옴

	m_pTrayAdminViewDlg1 = CTrayAdminViewDlg1::GetInstance();
	sVisionCamType = m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_1];
	if (sVisionCamType != "Undefine")
		m_pTrayAdminViewDlg1->Show();
	m_pTrayAdminViewDlg2 = CTrayAdminViewDlg2::GetInstance();
	sVisionCamType = m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_2];
	if (sVisionCamType != "Undefine")
		m_pTrayAdminViewDlg2->Show();
	m_pTrayAdminViewDlg3 = CTrayAdminViewDlg3::GetInstance();
	sVisionCamType = m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_3];
	if (sVisionCamType != "Undefine")
		m_pTrayAdminViewDlg3->Show();
	m_pTrayAdminViewDlg4 = CTrayAdminViewDlg4::GetInstance();
	sVisionCamType = m_ModelDefineInfo.m_strVisionName[VISION_NUMBER_4];
	if (sVisionCamType != "Undefine")
		m_pTrayAdminViewDlg4->Show();

	Initialize_Variables(FALSE);

	CuScanView* pView = (CuScanView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();

	if (Struct_PreferenceStruct.m_bUseVisionCopy)
	{
		pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N1)->SetWindowTextA("Vision #1");
		pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N2)->SetWindowTextA("Vision #2");
		pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N3)->SetWindowTextA("Vision #3");
		pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N4)->SetWindowTextA("Vision #4");
	}
	else
	{
		iVisionCamType = VISION_NUMBER_1;
		sVisionCamType = m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N1)->SetWindowTextA(sVisionCamType);
		if (sVisionCamType == "Undefine" || !Struct_PreferenceStruct.m_bUseVision[iVisionCamType])
			pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N1)->EnableWindow(FALSE);

		iVisionCamType = VISION_NUMBER_2;
		sVisionCamType = m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N2)->SetWindowTextA(sVisionCamType);
		if (sVisionCamType == "Undefine" || !Struct_PreferenceStruct.m_bUseVision[iVisionCamType])
			pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N2)->EnableWindow(FALSE);

		iVisionCamType = VISION_NUMBER_3;
		sVisionCamType = m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N3)->SetWindowTextA(sVisionCamType);
		if (sVisionCamType == "Undefine" || !Struct_PreferenceStruct.m_bUseVision[iVisionCamType])
			pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N3)->EnableWindow(FALSE);

		iVisionCamType = VISION_NUMBER_4;
		sVisionCamType = m_ModelDefineInfo.m_strVisionName[iVisionCamType];
		pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N4)->SetWindowTextA(sVisionCamType);
		if (sVisionCamType == "Undefine" || !Struct_PreferenceStruct.m_bUseVision[iVisionCamType])
			pView->GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING_VISION_N4)->EnableWindow(FALSE);
	}

	pView->OnBnClickedButtonModeChange();

	////// 디스크 용량 표시 //////////////////////////
	m_pInspectSummary->ShowDiskCapacity();


	////// 바코드 NoRead 저장 폴더 /////
	CString FolderName = Struct_PreferenceStruct.m_strEtcFolderPath + "CMI_Results";
	m_FileBase.CreateFolder(FolderName);
	CString FolderSub;
	FolderSub.Format("\\Barcode_NoRead");
	m_FileBase.CreateFolder(FolderName + FolderSub);
	FolderSub.Format("\\Barcode_Transfer_NoRead");
	m_FileBase.CreateFolder(FolderName + FolderSub);
	//////////////////////////////////////////////////////////////////////////
	FolderSub.Format("\\Delete_Lot_Check");
	m_FileBase.CreateFolder(FolderName + FolderSub);

	FolderSub.Format("\\ShipTrayAlign");
	m_FileBase.CreateFolder(FolderName + FolderSub);

	FolderSub.Format("\\LoadTrayAlign");
	m_FileBase.CreateFolder(FolderName + FolderSub);

	////// 4D 비전 RawImage 저장 폴더 /////
	FolderSub.Format("\\SpecularVision");
	m_FileBase.CreateFolder(FolderName + FolderSub);
	FolderSub.Format("\\DiffusedVision");
	m_FileBase.CreateFolder(FolderName + FolderSub);
	//////////////////////////////////////////////////////////////////////////

	////// LAS Data 저장 폴더 /////
	m_szLASDataFolder = "D:\\EVMS";
	m_FileBase.CreateFolder(m_szLASDataFolder);
	FolderSub.Format("\\TP");
	m_szLASDataFolder += FolderSub;
	m_FileBase.CreateFolder(m_szLASDataFolder);
	FolderSub.Format("\\Log");
	m_szLASDataFolder += FolderSub;
	m_FileBase.CreateFolder(m_szLASDataFolder);
	//////////////////////////////////////////////////////////////////////////

	////// PC ID /////////
	DWORD nSize = MAX_COMPUTERNAME_LENGTH + 1;
	char szBuffer[MAX_COMPUTERNAME_LENGTH + 1];
	::GetComputerName(szBuffer, &nSize);
	m_szPCID.Format("%s", szBuffer);
	//////////////////////////////////////////////////////////////////////////

	////// 모델 Backup 저장 폴더 /////
	FolderSub.Format("\\ModelDailyBackup");
	m_FileBase.CreateFolder(FolderName + FolderSub);
	//////////////////////////////////////////////////////////////////////////

	////// 검사 파라미터 저장 폴더 /////
	FolderSub.Format("\\TeachingParam");
	m_FileBase.CreateFolder(FolderName + FolderSub);
	//////////////////////////////////////////////////////////////////////////

	////// 검사 파라미터 변경 내역 저장 폴더 /////
	FolderSub.Format("\\ParameterChangeHistory");
	m_FileBase.CreateFolder(FolderName + FolderSub);
	//////////////////////////////////////////////////////////////////////////

	////// AI 재학습 ROI 정보 저장 폴더 /////
	FolderSub.Format("\\AiRetrainRoi");
	m_FileBase.CreateFolder(FolderName + FolderSub);
	//////////////////////////////////////////////////////////////////////////

#ifdef INLINE_MODE
	m_pInspectResultDlg->GetDlgItem(IDC_EDIT_LOT_ID)->EnableWindow(FALSE);
#endif

	if (Struct_PreferenceStruct.m_bRawImageSaveLAS || Struct_PreferenceStruct.m_bResultImageSaveLAS || Struct_PreferenceStruct.m_bReviewImageSaveLAS ||
		Struct_PreferenceStruct.m_bADJImageSaveLAS || Struct_PreferenceStruct.m_bResultLogSaveLAS)
		ConnectStatusLAS(0); //Ver 1026 add

#ifdef INLINE_MODE
	// PyADJ On/Off 확인 TRUE: On, FALSE: Off
	if (Struct_PreferenceStruct.m_bUseLocalADJ)
	{
		BOOL bPyADJConnectSuccess = TRUE;
		bPyADJConnectSuccess = ConnectStatusPyADJ();
		if (!bPyADJConnectSuccess)
		{
			Sleep(1000);
			BOOL bPyADJConnectSuccess = TRUE;
			bPyADJConnectSuccess = ConnectStatusPyADJ();
		}
	}

	// LASAggregator On/Off 확인 TRUE: On, FALSE: Off
	if (Struct_PreferenceStruct.m_bUseLASConnectionCheck)
	{
		BOOL bLASAggregatorConnectSuccess = TRUE;
		bLASAggregatorConnectSuccess = ConnectStatusLASAggregator();
		if (!bLASAggregatorConnectSuccess)
		{
			Sleep(1000);
			BOOL bLASAggregatorConnectSuccess = TRUE;
			bLASAggregatorConnectSuccess = ConnectStatusLASAggregator();
		}
	}
#endif
#pragma region 이미지 저장 thread pool 설정
	THEAPP.m_roi_image_save_thread_pool = std::make_shared<AIService::ThreadPool>(); // ROI 이미지 저장 thread 관리 class
	THEAPP.m_roi_image_save_thread_pool->init(1);
#pragma endregion
	m_variation_feature_log_thread_pool = std::make_shared<AIService::ThreadPool>(); // Variation Feature 로그 저장 thread 관리 class
	m_variation_feature_log_thread_pool->init(1);

	std::string logger_name;
#ifdef LGITAI
	std::filesystem::path lgit_ai_root = "C:\\lgit"; // 파일 위치
	std::filesystem::path log_root = lgit_ai_root / "logs"; // log 파일 위치
#pragma region lg ai logger setup
	logger_name = "main_inspector_logger";
	std::filesystem::path log_folder = log_root / logger_name;

	try {
		std::filesystem::create_directories(log_folder);
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to create log directory: " << e.what() << '\n';
	}

	// 기존 로거 존재 시 재사용
	auto logger = spdlog::get(logger_name);
	if (logger) {
		THEAPP.m_lg_inspector_logger = logger;
	}

	// 비동기 로깅 풀 생성
	if (!spdlog::thread_pool()) {
		spdlog::init_thread_pool(8192, 1);
	}

	// 날짜 문자열 생성
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::tm tm;
	localtime_s(&tm, &now_c);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y%m%d");
	std::string date_str = oss.str();

	// 파일명에 날짜 포함
	std::string base_filename = "MainInspectorLog_" + date_str + ".txt";
	std::filesystem::path full_log_path = log_folder / base_filename;

	// rotating_logger 사용 (파일명에 날짜 포함)
	THEAPP.m_lg_inspector_logger = spdlog::rotating_logger_mt<spdlog::async_factory>(
		logger_name,
		full_log_path.string(),
		1024ull * 1024 * 500, // 500MB
		10                    // 최대 10개 보관 (하루 5GB 제한 등)
	);

	// 로그 포맷 설정
	THEAPP.m_lg_inspector_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");
	THEAPP.m_lg_inspector_logger->set_level(spdlog::level::debug);
	THEAPP.m_lg_inspector_logger->flush_on(spdlog::level::info);
	THEAPP.m_lg_inspector_logger->info("Logger initialized for date: {}", date_str);
#pragma endregion
#pragma region info file read
	std::filesystem::path ai_inspector_setup_file_dir = lgit_ai_root / "setup";
	std::filesystem::path ai_inspector_manager_file_name = "ai_inspector_manager_setup.json";
	std::filesystem::path ai_inspector_manager_file_path = ai_inspector_setup_file_dir / ai_inspector_manager_file_name;

	// 폴더 설정
	try {
		if (!std::filesystem::exists(ai_inspector_setup_file_dir)) {
			std::filesystem::create_directories(ai_inspector_setup_file_dir);
		}
	}
	catch (...) {
		throw std::runtime_error("설정 폴더 만들다가 죽어버려");
	}
	try {
		if (!std::filesystem::exists(ai_inspector_manager_file_path)) { // 설정 파일 없음
			// 없으면 초기값으로 설정 파일 생성
			LGIT::Model::InspectorManagerSetup temp_setup;
			// 1번 망 초기 setup
			temp_setup.inspector_setup.thread_number = 1;
			temp_setup.inspector_setup.log_level = 1;
			temp_setup.inspector_setup.inspector_setup_file_path = "C:\\lgit\\model1_setup\\thresholds.json";
			// 2번 망 초기 setup
			temp_setup.inspector2_setup.thread_number = 1;
			temp_setup.inspector2_setup.log_level = 1;
			temp_setup.inspector2_setup.inspector_model_dir = "C:\\lgit\\model2_setup";
			// svm setup
			temp_setup.inspector_svm_setup.thread_number = 1;
			temp_setup.inspector_svm_setup.log_level = 1;
			temp_setup.inspector_svm_setup.inspector_model_dir = "C:\\lgit\\model_svm_setup";
			// seg setup
			temp_setup.inspector_seg_setup.thread_number = 1;
			temp_setup.inspector_seg_setup.log_level = 1;
			temp_setup.inspector_seg_setup.inspector_model_dir = "C:\\lgit\\model_seg_setup";
			// val setup
			temp_setup.inspector_val_setup.thread_number = 1;
			temp_setup.inspector_val_setup.log_level = 1;
			temp_setup.inspector_val_setup.inspector_model_dir = "C:\\lgit\\model_val_setup";
			// model 파일 검색
			std::filesystem::path model_dir = "C:\\lgit\\model1_setup";
			std::vector<std::filesystem::path> model_list = LGIT::ModelInference::GetBinFiles(model_dir);
			// 검색된 모델 추가
			temp_setup.reset_inspector1_model_list(model_list);
			// 저장
			temp_setup.save_to_file(ai_inspector_manager_file_path.string());
		}
	}
	catch (...) {
		throw std::runtime_error("설정 파일 읽다가 죽어버려");
	}
	LGIT::Model::InspectorManagerSetup setup = LGIT::Model::InspectorManagerSetup::load_from_file(ai_inspector_manager_file_path.string());
	// model 파일 검색
	std::filesystem::path model_dir = "C:\\lgit\\model1_setup";
	std::vector<std::filesystem::path> model_list = LGIT::ModelInference::GetBinFiles(model_dir);
	// 검색된 모델 추가
	setup.reset_inspector1_model_list(model_list);
	// 저장
	setup.save_to_file(ai_inspector_manager_file_path.string());

#pragma endregion info file read
#pragma region ai inspector init
	// 활성화된 CUDA GPU 확인
	int device_count = 0;
	cudaError_t err = cudaGetDeviceCount(&device_count);
	int last_cuda_device_index = -1;

	if (err == cudaSuccess) {
		THEAPP.m_lg_inspector_logger->info("총 CUDA GPU 장치 수 : {}", device_count);
		for (int device = 0; device < device_count; ++device) {
			cudaDeviceProp prop;
			cudaGetDeviceProperties(&prop, device);
			last_cuda_device_index++;
		}
		THEAPP.m_lg_inspector_logger->info("설정 CUDA GPU Index : {}", last_cuda_device_index);
	}
	else {
		THEAPP.m_lg_inspector_logger->error("cudaGetDeviceCount 실패: {}", cudaGetErrorString(err));
		THEAPP.m_lg_inspector_logger->warn("device index 기본값 0으로 초기화");
	}

	if (last_cuda_device_index == -1) {
		THEAPP.m_lg_inspector_logger->error("Select GPU Index {}", last_cuda_device_index);
		THEAPP.m_lg_inspector_logger->error("사용 가능한 CUDA GPU 없음, 치명적 오류 가능성 있음");
	}
	else if (last_cuda_device_index == 0) {
		THEAPP.m_lg_inspector_logger->error("Select GPU Index {}", last_cuda_device_index);
		THEAPP.m_lg_inspector_logger->error("비전과 AI 사용 GPU 중복, 치명적 오류 가능성 있음");
	}

	using ConditionArray = std::decay_t<decltype(g_strInspectionTypeName)>;
	ConditionArray condition_name_list = g_strInspectionTypeName;
	ConditionArray condition_short_name_list = g_strInspectionTypeName_Short;
	/*
	constexpr size_t g_str_condition_name_size = std::size(g_strConditionName);
	std::array<std::string, 23> condition_name_list;
	std::array<std::string, 23> condition_short_name_list;
	USES_CONVERSION;
	std::transform(
		g_strConditionName, g_strConditionName + g_str_condition_name_size,
		condition_name_list.begin(),
		[](const CString& str) {
		return std::string(CT2A(str));
	}
	);
	// 코드와 동일 컴파일러 최적화 ==> for (size_t i = 0; i < g_condition_name_size; ++i) {
	//	condition_name_list[i] = CT2A(g_strConditionName[i]);
	//}
	std::copy(
		g_strInspectionTypeName_Short.begin(),
		g_strInspectionTypeName_Short.end(),
		condition_short_name_list.begin()
	);
	*/

	auto init_1_start = std::chrono::high_resolution_clock::now();
	THEAPP.m_lgAIInspector = std::make_shared<LGIT::LAIInspector>(
		setup.inspector_setup.thread_number,
		condition_name_list,
		condition_short_name_list,
		setup.inspector_setup.model_path_list,
		setup.inspector_setup.inspector_setup_file_path,
		setup.inspector_setup.log_level,
		log_root,
		last_cuda_device_index);
	auto init_1_end = std::chrono::high_resolution_clock::now();
	auto total_1_time = std::chrono::duration_cast<std::chrono::milliseconds>(init_1_end - init_1_start).count();
	THEAPP.m_lg_inspector_logger->info("LGIT::LAIInspector init: {}ms", total_1_time);

	auto init_2_start = std::chrono::high_resolution_clock::now();
	THEAPP.m_lgAIInspector2 = std::make_shared<LGIT::LAIInspector2>(
		setup.inspector2_setup.thread_number,
		condition_name_list,
		condition_short_name_list,
		setup.inspector2_setup.inspector_model_dir,
		setup.inspector2_setup.log_level,
		log_root,
		last_cuda_device_index);
	auto init_2_end = std::chrono::high_resolution_clock::now();
	auto total_2_time = std::chrono::duration_cast<std::chrono::milliseconds>(init_2_end - init_2_start).count();
	THEAPP.m_lg_inspector_logger->info("LGIT::LAIInspector2 init: {}ms", total_2_time);

	auto init_svm_start = std::chrono::high_resolution_clock::now();
	THEAPP.m_lgAIInspectorSVM = std::make_shared<LGIT::LAIInspectorSVM>(
		setup.inspector_svm_setup.thread_number,
		condition_name_list,
		condition_short_name_list,
		setup.inspector_svm_setup.inspector_model_dir,
		setup.inspector_svm_setup.log_level,
		log_root,
		last_cuda_device_index);
	auto init_svm_end = std::chrono::high_resolution_clock::now();
	auto total_svm_time = std::chrono::duration_cast<std::chrono::milliseconds>(init_svm_end - init_svm_start).count();
	THEAPP.m_lg_inspector_logger->info("LGIT::LAIInspectorSVM init: {}ms", total_svm_time);

	auto init_seg_start = std::chrono::high_resolution_clock::now();
	THEAPP.m_lgAIInspectorSEG = std::make_shared<LGIT::LAIInspectorSEG>(
		setup.inspector_seg_setup.thread_number,
		condition_name_list,
		condition_short_name_list,
		setup.inspector_seg_setup.inspector_model_dir,
		setup.inspector_seg_setup.log_level,
		log_root,
		last_cuda_device_index);
	auto init_seg_end = std::chrono::high_resolution_clock::now();
	auto total_seg_time = std::chrono::duration_cast<std::chrono::milliseconds>(init_seg_end - init_seg_start).count();
	THEAPP.m_lg_inspector_logger->info("LGIT::LAIInspectorSEG init: {}ms", total_seg_time);

	auto init_val_start = std::chrono::high_resolution_clock::now();
	THEAPP.m_lgAIInspectorVAL = std::make_shared<LGIT::LAIInspectorVAL>(
		setup.inspector_val_setup.thread_number,
		condition_name_list,
		condition_short_name_list,
		setup.inspector_val_setup.inspector_model_dir,
		setup.inspector_val_setup.log_level,
		log_root,
		last_cuda_device_index);
	auto init_val_end = std::chrono::high_resolution_clock::now();
	auto total_val_time = std::chrono::duration_cast<std::chrono::milliseconds>(init_val_end - init_val_start).count();
	THEAPP.m_lg_inspector_logger->info("LGIT::LAIInspectorVAL init: {}ms", total_val_time);

	auto init_manager_start = std::chrono::high_resolution_clock::now();
	THEAPP.m_lg_inspectorManager = std::make_shared<LGIT::InspectorManager>(
		setup.thread_number,
		THEAPP.m_lgAIInspector,
		THEAPP.m_lgAIInspector2,
		THEAPP.m_TCPClientService,
		THEAPP.m_lgAIInspectorSVM,
		THEAPP.m_lgAIInspectorSEG,
		THEAPP.m_lgAIInspectorVAL,
		setup.log_level,
		setup.is_use_segmentation_visualization,
		log_root);
	auto init_manager_end = std::chrono::high_resolution_clock::now();
	auto total_manager_time = std::chrono::duration_cast<std::chrono::milliseconds>(init_manager_end - init_manager_start).count();
	THEAPP.m_lg_inspector_logger->info("LGIT::InspectorManager init: {}ms", total_manager_time);

	auto init_ai_total_end = std::chrono::high_resolution_clock::now();
	auto init_ai_total_time = std::chrono::duration_cast<std::chrono::milliseconds>(init_ai_total_end - init_1_start).count();
	THEAPP.m_lg_inspector_logger->info("total init: {}ms", init_ai_total_time);

	THEAPP.m_isSetLGITAI = true;

	THEAPP.m_lg_inspectorManager->try_model_reload();
#pragma endregion ai inspector init
#endif // LGITAI
#pragma region syai logger setup
	std::filesystem::path base_dir = "C:\\syai_avi"; // 파일 위치
	logger_name = "MS";
	InitializeLogger(logger_name.c_str(), base_dir.string().c_str(), static_cast<int>(syai::runtime::domain::config::LogLevel::INFO_SY));
#pragma endregion
#ifdef SYAI
	if (!InitializeSYAI()) {
		
	}
	// test_response.release();
	int b = 1;
#endif // SYAI

	if (Struct_PreferenceStruct.m_iPCType == PC_NUMBER_1)
	{
#ifdef SINGLE_LENS
		m_pHandlerService->SetHandlerTCP_IP("192.168.0.11");
		m_pHandlerService->SetHandlerTCP_PORT(12001);
#elif defined(ASSY_LENS)
		m_pHandlerService->SetHandlerTCP_IP("192.168.0.11");
		m_pHandlerService->SetHandlerTCP_PORT(8001);
#else
		m_pHandlerService->SetHandlerTCP_IP("192.168.0.11");
		m_pHandlerService->SetHandlerTCP_PORT(8001);
#endif
	}
	else if (Struct_PreferenceStruct.m_iPCType == PC_NUMBER_2)
	{
		m_pHandlerService->SetHandlerTCP_IP("192.168.0.11");
		m_pHandlerService->SetHandlerTCP_PORT(8002);
	}
	else if (Struct_PreferenceStruct.m_iPCType == PC_NUMBER_3)
	{
		m_pHandlerService->SetHandlerTCP_IP("192.168.0.11");
		m_pHandlerService->SetHandlerTCP_PORT(8003);
	}
	else if (Struct_PreferenceStruct.m_iPCType == PC_NUMBER_4)
	{
		m_pHandlerService->SetHandlerTCP_IP("192.168.0.11");
		m_pHandlerService->SetHandlerTCP_PORT(8004);
	}
	else if (Struct_PreferenceStruct.m_iPCType == PC_NUMBER_5)
	{
		m_pHandlerService->SetHandlerTCP_IP("192.168.0.11");
		m_pHandlerService->SetHandlerTCP_PORT(8005);
	}

	m_pHandlerService->SetHandlerTCP_Connect(TRUE);
	m_pHandlerService->Initialize_Handler();

	return TRUE;
}

//char* CuScanApp::UTF8toANSI(char *pszCode)
//{
//	BSTR    bstrWide;
//	char*   pszAnsi;
//	int     nLength;
//
//	// Get nLength of the Wide Char buffer   
//	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, NULL, NULL);
//	bstrWide = SysAllocStringLen(NULL, nLength);
//
//	// Change UTF-8 to Unicode (UTF-16)   
//	MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, bstrWide, nLength);
//
//
//	// Get nLength of the multi byte buffer    
//	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
//	pszAnsi = new char[nLength];
//
//
//	// Change from unicode to mult byte   
//	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
//
//	SysFreeString(bstrWide);
//
//
//	return pszAnsi;
//}

void CuScanApp::ReadDefectReViewInfoINI()
{
	int iVisionCamType1, iVisionCamType2, iVisionCamType3, iVisionCamType4;
	CString sVisionCamType1, sVisionCamType2, sVisionCamType3, sVisionCamType4;

	for (int i = 0; i < MAX_REVIEW_SAMPLE; i++)
		m_Struct_ReViewImageInfo[i].Reset();

	CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
	CString strEquipModel = g_strModelTypeName[GetModelType()];
	CString strIniFile;
	strIniFile.Format("ReViewInfo_%s.ini", strEquipModel);
	CIniFileCS INI_ReViewInfo(strDataFolder + strIniFile);
	CString strSection, strKey;

	ReViewImageInfoStruct TempReViewImageInfo;
	CString sTemp, sSampleInfo;
	char *sep = ",";

	m_iMaxRosSaveDefect = 0;
	for (int i = 1; i <= MAX_REVIEW_SAMPLE; i++)
	{
		m_Struct_ReViewImageInfo[i - 1].Reset();
		strSection = "REVIEW INFO";
		strKey.Format("%d", i);
		sSampleInfo = INI_ReViewInfo.Get_String(strSection, strKey, "Nan");

		if (sSampleInfo == "Nan")
			break;

		char *psTemp = (LPTSTR)(LPCTSTR)sSampleInfo;

		TempReViewImageInfo.m_sInspectionType = strtok(psTemp, sep);
		TempReViewImageInfo.m_sDefectName = strtok(NULL, sep);
		TempReViewImageInfo.m_sVisionCam = strtok(NULL, sep);
		sTemp = strtok(NULL, sep);
		TempReViewImageInfo.m_iImageNo = atoi(sTemp);
		sTemp = strtok(NULL, sep);
		TempReViewImageInfo.m_iStartX = atoi(sTemp);
		sTemp = strtok(NULL, sep);
		TempReViewImageInfo.m_iStartY = atoi(sTemp);
		sTemp = strtok(NULL, sep);
		TempReViewImageInfo.m_fZoomRatio = atof(sTemp);
		sTemp = strtok(NULL, sep);
		TempReViewImageInfo.m_iUseViewPortFlag = atoi(sTemp);

		if ((TempReViewImageInfo.m_sVisionCam == "TC" || TempReViewImageInfo.m_sVisionCam == "BC" || TempReViewImageInfo.m_sVisionCam == "3D") == FALSE)
			continue;

		m_Struct_ReViewImageInfo[m_iMaxRosSaveDefect].m_sInspectionType = TempReViewImageInfo.m_sInspectionType;
		m_Struct_ReViewImageInfo[m_iMaxRosSaveDefect].m_sDefectName = TempReViewImageInfo.m_sDefectName;
		m_Struct_ReViewImageInfo[m_iMaxRosSaveDefect].m_sVisionCam = TempReViewImageInfo.m_sVisionCam;
		m_Struct_ReViewImageInfo[m_iMaxRosSaveDefect].m_iImageNo = TempReViewImageInfo.m_iImageNo;
		m_Struct_ReViewImageInfo[m_iMaxRosSaveDefect].m_iStartX = TempReViewImageInfo.m_iStartX;
		m_Struct_ReViewImageInfo[m_iMaxRosSaveDefect].m_iStartY = TempReViewImageInfo.m_iStartY;
		m_Struct_ReViewImageInfo[m_iMaxRosSaveDefect].m_fZoomRatio = TempReViewImageInfo.m_fZoomRatio;
		m_Struct_ReViewImageInfo[m_iMaxRosSaveDefect].m_iUseViewPortFlag = TempReViewImageInfo.m_iUseViewPortFlag;

		++m_iMaxRosSaveDefect;
	}
}

void CuScanApp::ReadFaiDefectReViewInfo()
{
	CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
	CString strEquipModel = g_strModelTypeName[GetModelType()];
	CString strIniFile;
	strIniFile.Format("ReViewInfo_FAI_%s.ini", strEquipModel);
	CIniFileCS INI_ReViewInfo(strDataFolder + strIniFile);
	CString strSection, strKey;
	CString sSampleBuffer, sSampleInfo;
	char *sep = ",";

	// INI 파일에서 읽어올 내용
	CString sFaiName;
	int		iFaiNumber;
	int		iImageCount;	// Not Used. 아래 주석 참조
	CString sInspectType;	// Not Used.
	int		iImageNo;
	int		iStartX;
	int		iStartY;
	float	fZoomRatio;
	int		iViewportCenter;

	// FAI 리뷰이미지 정보 초기화
	for (int i = 0; i < MAX_FAI_ITEM; i++)
		m_Struct_FAI_ReViewImageInfo[i].Reset();

	BOOL bNoMoreData = FALSE;
	// INI 파일 인덱스가 1부터 시작.
	int iFaiItemCnt = 1;
	for (; iFaiItemCnt <= MAX_REVIEW_SAMPLE; iFaiItemCnt++)
	{
		strSection = "REVIEW INFO";
		strKey.Format("%d", iFaiItemCnt);
		sSampleInfo = INI_ReViewInfo.Get_String(strSection, strKey, "Nan");

		if (sSampleInfo == "Nan")
		{
			bNoMoreData = TRUE;
			break;
		}

		char *psTemp = (LPTSTR)(LPCTSTR)sSampleInfo;

		/**
		 * FAI 재검토 이미지 정보 INI 파일 포맷
		 * [REVIEW INFO]
		 * 1=FAI-71,71,1,D,5,0,0,0.6,0
			 -> FAI-71 : FAI 이름
			 -> 71 : FAI 번호
			 -> 1 : 이미지 개수
			 -> D : 검사 유형
			 -> 5 : 이미지 번호
			 -> 0 : 시작 X 좌표
			 -> 0 : 시작 Y 좌표
			 -> 0.6 : 줌 배율
			 -> 0 : 뷰포트 센터 좌표
		 */
		sFaiName = strtok(psTemp, sep);
		sSampleBuffer = strtok(NULL, sep);
		iFaiNumber = atoi(sSampleBuffer);
		sSampleBuffer = strtok(NULL, sep);
		// 아래 값(iImageCount) 코드 어디에도 사용되지 않음. 기존 INI 호환을 위해 남겨둠.
		// 단일 FAI 리뷰 이미지를 여러장 남기려면, INI 에 FAI 를 두 번 써야함
		// ex : FAI-1,[값들...]
		//      FAI-1,[값들...]
		iImageCount = atoi(sSampleBuffer);
		// 아래 값(sInspectType) 코드 어디에도 사용되지 않음. 기존 INI 호환을 위해 남겨둠.
		sInspectType = strtok(NULL, sep);
		sSampleBuffer = strtok(NULL, sep);
		iImageNo = atoi(sSampleBuffer);
		sSampleBuffer = strtok(NULL, sep);
		iStartX = atoi(sSampleBuffer);
		sSampleBuffer = strtok(NULL, sep);
		iStartY = atoi(sSampleBuffer);
		sSampleBuffer = strtok(NULL, sep);
		fZoomRatio = atof(sSampleBuffer);
		sSampleBuffer = strtok(NULL, sep);
		iViewportCenter = atoi(sSampleBuffer);

		// INI 에서 읽어온 정보 배열에 저장
		int iCurReviewImageIndex = m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iNoReviewImage;
		if (iCurReviewImageIndex == 0)
		{
			m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_bGenReviewImage = TRUE;
			m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_sFaiName = sFaiName;
		}

		m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iImageNo[iCurReviewImageIndex] = iImageNo;
		m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iStartX[iCurReviewImageIndex] = iStartX;
		m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iStartY[iCurReviewImageIndex] = iStartY;
		m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_fZoomRatio[iCurReviewImageIndex] = fZoomRatio;
		m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iViewportCenter[iCurReviewImageIndex] = iViewportCenter;
		++(m_Struct_FAI_ReViewImageInfo[iFaiNumber].m_iNoReviewImage);
	}

	if ((bNoMoreData == TRUE) && (iFaiItemCnt == 1))
	{
		TRACE("\"%s\" 파일에 유효한 데이터가 없습니다.\n", strIniFile);
	}
	else
	{
		TRACE("\"%s\" 파일에서 %d 개의 FAI 리뷰 이미지 정보를 읽어왔습니다.\n", strIniFile, iFaiItemCnt);
	}
}

void CuScanApp::ReadADJReViewInfoINI()
{
	CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
	// CIniFileCS INI_AdjImageInfo(strDataFolder + "AdjImageInfo.ini");
	CIniFileCS INI_AdjImageInfo(strDataFolder + "AdjImageInfo_NoUse.ini");
	CString strSection, strKey;

	strSection = "ADJ INFO";
	CString sTemp, sSampleInfo;
	char *sep = ",";

	int i;
	for (i = 0; i < MAX_REVIEW_SAMPLE; i++)
		m_Struct_AdjImageInfo[i].Reset();

	int iNoReViewInfo = 0;
	m_iAdjSelectImageCount = 0;
	for (i = 1; i <= MAX_REVIEW_SAMPLE; i++)
	{
		strKey.Format("%d", i);
		sSampleInfo = INI_AdjImageInfo.Get_String(strSection, strKey, "Nan");

		if (sSampleInfo == "Nan")
			break;

		char *psTemp = (LPTSTR)(LPCTSTR)sSampleInfo;

		m_Struct_AdjImageInfo[iNoReViewInfo].m_sInspectionType = strtok(psTemp, sep);
		m_Struct_AdjImageInfo[iNoReViewInfo].m_sDefectName = strtok(NULL, sep);
		m_Struct_AdjImageInfo[iNoReViewInfo].m_sVisionCam = strtok(NULL, sep);
		sTemp = strtok(NULL, sep);
		m_Struct_AdjImageInfo[iNoReViewInfo].m_iImageCount = atoi(sTemp);
		for (int i = 0; i < m_Struct_AdjImageInfo[iNoReViewInfo].m_iImageCount; i++)
		{
			sTemp = strtok(NULL, sep);
			m_Struct_AdjImageInfo[iNoReViewInfo].m_iImageIndex[i] = atoi(sTemp) - 1;
		}

		++iNoReViewInfo;
		m_iAdjSelectImageCount++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CuScanApp message handlers

int CuScanApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

#ifdef INLINE_MODE
	strLog.Format("★★★★★★★★★★★★★★★★★★ Universal AVI INLINE system end ★★★★★★★★★★★★★★★★★★");
	m_log_system->warn("{}", LOG_CSTR(strLog));
#else
	strLog.Format("★★★★★★★★★★★★★★★★★★ Universal AVI OFFLINE system end ★★★★★★★★★★★★★★★★★★");
	m_log_system->warn("{}", LOG_CSTR(strLog));
#endif

	int iPcVisionNo;

	if (m_bVisionSWRun)
	{
		m_pHandlerService->Set_StatusUpdate(HS_NOT_READY);
		Sleep(200);
		m_pHandlerService->Terminate_Handler();

		if (Struct_PreferenceStruct.m_bUseConvertSingleThread || Struct_PreferenceStruct.m_bUseGpuAffineTrans)
			m_pDualCameraManager[VISION_NUMBER_1]->StopDFMProcThread();

		for (iPcVisionNo = VISION_NUMBER_1; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
			delete m_pDualCameraManager[iPcVisionNo];

#ifdef ASSY_LENS
		for (iPcVisionNo = SUB_CAM_1; iPcVisionNo < TOTAL_SUB_CAM; iPcVisionNo++)
			delete m_pSubCameraManager[iPcVisionNo];
#endif

		m_pInspectLibraryDataManager->UnInitialize();
		m_pLightLibraryDataManager->UnInitialize();

#ifdef INLINE_MODE
		MsysFree(m_MilSystem[0]);
		MsysFree(m_MilSystem[1]);
		MappFree(m_MilApplication);

		m_pTriggerManager->DeleteInstance();
#endif

#ifdef SNZEROMQ
		THEAPP.m_zeromq_client.~ZeroMQClient();
#endif // SNZEROMQ

		m_pHandlerService->Terminate_TcpHandler();
		m_pHandlerService->DeleteInstance();

		m_pInspectSummary->DeleteInstance();
		m_pInspectAdminViewDlg->DeleteInstance();
		m_pInspectAdminViewToolbarDlg->DeleteInstance();
		m_pInspectViewOverayImageDlg1->DeleteInstance();
		m_pInspectViewOverayImageDlg2->DeleteInstance();
		m_pInspectViewOverayImageDlg3->DeleteInstance();
		m_pInspectViewOverayImageDlg4->DeleteInstance();
		m_pInspectViewOverayImageToolbarDlg1->DeleteInstance();
		m_pInspectViewOverayImageToolbarDlg2->DeleteInstance();
		m_pInspectViewOverayImageToolbarDlg3->DeleteInstance();
		m_pInspectViewOverayImageToolbarDlg4->DeleteInstance();
		m_pInspectAdminViewHideDlg1->DeleteInstance();
		m_pInspectAdminViewHideDlg2->DeleteInstance();
		m_pInspectAdminViewHideDlg3->DeleteInstance();
		m_pInspectAdminViewHideDlg4->DeleteInstance();
		m_pTrayAdminViewDlg1->DeleteInstance();
		m_pTrayAdminViewDlg2->DeleteInstance();
		m_pTrayAdminViewDlg3->DeleteInstance();
		m_pTrayAdminViewDlg4->DeleteInstance();
		m_pInspectResultDlg->DeleteInstance();
		m_pDefectListDlg->DeleteInstance();
		m_pTabControlDlg->DeleteInstance();
		m_pInspectLibraryDataManager->DeleteInstance();
		m_pLightLibraryDataManager->DeleteInstance();

#ifdef USE_SUAKIT
		//m_pADJSendImageHideDlg->DeleteInstance();
#endif

		int iDualModelData;
		for (iDualModelData = 0; iDualModelData < MODEL_DATA_MAX; iDualModelData++)
		{
			for (iPcVisionNo = VISION_NUMBER_1; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
				delete m_pDualModelDataManager[iDualModelData][iPcVisionNo];
		}

		for (iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
		{
			if (m_ModelDefineInfo.m_bVisionPWM[iPcVisionNo])
			{
				m_pLightPwmManager[iPcVisionNo]->UnInitialize();
				delete m_pLightPwmManager[iPcVisionNo];
			}
		}

		m_pAlgorithm->DeleteInstance();
		m_pInspectService->DeleteInstance();
		m_pAutoCalService->DeleteInstance();
		m_pGFunction->DeleteInstance();

		for (iPcVisionNo = 0; iPcVisionNo < VISION_NUMBER_MAX; iPcVisionNo++)
			delete m_pCalDataService_N[iPcVisionNo];

		m_bVisionSWRun = FALSE;
	}

	DeleteCriticalSection(&CS_LOTRESULTLOG);
	DeleteCriticalSection(&CS_DAYSUMMARYLOG);
	DeleteCriticalSection(&CS_COLORINFOLOG);
	DeleteCriticalSection(&CS_DEFECTSHAPEFEATURELOG);
	DeleteCriticalSection(&CS_DEFECTVARIATIONFEATURELOG);
	DeleteCriticalSection(&CS_INSPECTCONDITIONLOG);
	DeleteCriticalSection(&CS_ALLDEFECTSRESULTLOG);
	DeleteCriticalSection(&CS_AUTOCALLIGHTRESULTLOG);
	DeleteCriticalSection(&CS_MEASUREANALYSISLOG);	// 2025.04.08 - v2011 - LeeGW - CM AKC FAI
	DeleteCriticalSection(&CS_ADJLOTRESULTLOG);	
	DeleteCriticalSection(&CS_ADJDAYSUMMARYLOG);
	DeleteCriticalSection(&CS_ADJONLYRESULTLOG);

	return CWinApp::ExitInstance();
}

void CuScanApp::Initialize_Variables(BOOL bReset)
{
	if (bReset)
	{
		m_pTrayAdminViewDlg1->m_TrayImagePB.Reset();
		m_pTrayAdminViewDlg2->m_TrayImagePB.Reset();
		m_pTrayAdminViewDlg3->m_TrayImagePB.Reset();
		m_pTrayAdminViewDlg4->m_TrayImagePB.Reset();
	}

	for (int i = 0; i < MAX_MODULE_TRAY; i++)
	{
		m_pTrayAdminViewDlg1->m_TrayImagePB.OkNg[i] = DEFECT_TYPE_PROCESSING_ERROR;
		m_pTrayAdminViewDlg2->m_TrayImagePB.OkNg[i] = DEFECT_TYPE_PROCESSING_ERROR;
		m_pTrayAdminViewDlg3->m_TrayImagePB.OkNg[i] = DEFECT_TYPE_PROCESSING_ERROR;
		m_pTrayAdminViewDlg4->m_TrayImagePB.OkNg[i] = DEFECT_TYPE_PROCESSING_ERROR;
		m_pTrayAdminViewDlg1->m_TrayImagePB.ClickRegion[i] = FALSE;
		m_pTrayAdminViewDlg2->m_TrayImagePB.ClickRegion[i] = FALSE;
		m_pTrayAdminViewDlg3->m_TrayImagePB.ClickRegion[i] = FALSE;
		m_pTrayAdminViewDlg4->m_TrayImagePB.ClickRegion[i] = FALSE;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// FAI Inspector 초기화
	enMODEL_TYPE modelType = enMODEL_TYPE(GetModelType());

	// FAIDataManager를 통한 중앙 집중식 FAI 데이터 관리
	m_pFAIDataManager.Initialize();

	ReplaceFAIInspector(m_pFAIInspector, g_strModelTypeName[GetModelType()], m_pAlgorithm);
	//////////////////////////////////////////////////////////////////////////////////////////////
}

void CuScanApp::ReadPreferenceINI()
{
	CString strModelFolder = GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_Preference(strModelFolder + "\\Preference.ini");

	CString strSection, strKey;
	strSection = "EQUIPMENT";
	Struct_PreferenceStruct.m_strEquipNo = INI_Preference.Get_String(strSection, "Equip No", "-1");

	if (Struct_PreferenceStruct.m_strEquipNo != "-1")
	{
		strSection = "PC NUMBER";
		Struct_PreferenceStruct.m_iPCType = INI_Preference.Get_Integer(strSection, "PC No", 0);

		strSection = "INSPECT VISION";
		Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_1] = INI_Preference.Get_Bool(strSection, "Use Vision1", TRUE);
		Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_2] = INI_Preference.Get_Bool(strSection, "Use Vision2", TRUE);
		Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_3] = INI_Preference.Get_Bool(strSection, "Use Vision3", FALSE);
		Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_4] = INI_Preference.Get_Bool(strSection, "Use Vision4", FALSE);
		for (int i = 0; i < VISION_NUMBER_MAX; i++)
		{
			if (m_ModelDefineInfo.m_strVisionName[i] != "Undefine")
				m_ModelDefineInfo.m_iUseVisionNo++;
			else
				Struct_PreferenceStruct.m_bUseVision[i] = FALSE;
		}

		strSection = "FOLDER PATH";
		Struct_PreferenceStruct.m_bUseLAS1 = INI_Preference.Get_Bool(strSection, "Use LAS1", FALSE);
		Struct_PreferenceStruct.m_bUseLAS2 = INI_Preference.Get_Bool(strSection, "Use LAS2", FALSE);
		Struct_PreferenceStruct.m_bUseLAS3 = INI_Preference.Get_Bool(strSection, "Use LAS3", FALSE);

		Struct_PreferenceStruct.m_strSaveFolderPathLAS1 = INI_Preference.Get_String(strSection, "Save Folder Path LAS1", "R:\\");
		Struct_PreferenceStruct.m_strSaveFolderPathLAS2 = INI_Preference.Get_String(strSection, "Save Folder Path LAS2", "S:\\");
		Struct_PreferenceStruct.m_strSaveFolderPathLAS3 = INI_Preference.Get_String(strSection, "Save Folder Path LAS3", "T:\\");

		Struct_PreferenceStruct.m_bUseLASConnectionCheck = INI_Preference.Get_Bool(strSection, "Send LAS Connect Error Massage", FALSE);

		Struct_PreferenceStruct.m_strRawFolderPath = INI_Preference.Get_String(strSection, "Save Raw Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strResultFolderPath = INI_Preference.Get_String(strSection, "Save Result Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strReviewFolderPath = INI_Preference.Get_String(strSection, "Save Review Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strAdjFolderPath = INI_Preference.Get_String(strSection, "Save ADJ Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strResultLogFolderPath = INI_Preference.Get_String(strSection, "Save Result Log Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strEtcFolderPath = INI_Preference.Get_String(strSection, "Save ETC Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strOfflineTestFolderPath = INI_Preference.Get_String(strSection, "Offline Test Folder Path", "C:\\");

		strSection = "IMAGE SAVE OPTION";
		Struct_PreferenceStruct.m_bRawImageSaveLocal = INI_Preference.Get_Bool(strSection, "Raw Image Save Local", TRUE);
		Struct_PreferenceStruct.m_bResultImageSaveLocal = INI_Preference.Get_Bool(strSection, "Result Image Save Local", TRUE);
		Struct_PreferenceStruct.m_bReviewImageSaveLocal = INI_Preference.Get_Bool(strSection, "Review Image Save Local", TRUE);
		Struct_PreferenceStruct.m_bADJImageSaveLocal = INI_Preference.Get_Bool(strSection, "ADJ Image Save Local", TRUE);
		Struct_PreferenceStruct.m_bResultLogSaveLocal = INI_Preference.Get_Bool(strSection, "Result Log Save Local", TRUE);

		Struct_PreferenceStruct.m_bRawImageSaveLAS = INI_Preference.Get_Bool(strSection, "Raw Image Save LAS", FALSE);
		Struct_PreferenceStruct.m_bResultImageSaveLAS = INI_Preference.Get_Bool(strSection, "Result Image Save LAS", FALSE);
		Struct_PreferenceStruct.m_bReviewImageSaveLAS = INI_Preference.Get_Bool(strSection, "Review Image Save LAS", FALSE);
		Struct_PreferenceStruct.m_bADJImageSaveLAS = INI_Preference.Get_Bool(strSection, "ADJ Image Save LAS", FALSE);
		Struct_PreferenceStruct.m_bResultLogSaveLAS = INI_Preference.Get_Bool(strSection, "Result Log Save LAS", FALSE);

		Struct_PreferenceStruct.m_bUseRawImageThread = INI_Preference.Get_Bool(strSection, "Use Raw Image Thread", TRUE);
		Struct_PreferenceStruct.m_bUseResultImageThread = INI_Preference.Get_Bool(strSection, "Use Result Image Thread", FALSE);
		Struct_PreferenceStruct.m_bUseReviewImageThread = INI_Preference.Get_Bool(strSection, "Use Review Image Thread", FALSE);
		Struct_PreferenceStruct.m_bUseADJImageThread = INI_Preference.Get_Bool(strSection, "Use ADJ Image Thread", FALSE);
		Struct_PreferenceStruct.m_bUseResultLogThread = INI_Preference.Get_Bool(strSection, "Use Result Log Thread", TRUE);

		Struct_PreferenceStruct.m_bUseRawImageTempDrive = INI_Preference.Get_Bool(strSection, "Use Raw Image Temporary Drive", FALSE);

		Struct_PreferenceStruct.m_bSaveRawImage = INI_Preference.Get_Bool(strSection, "Save Raw Image", TRUE);
		Struct_PreferenceStruct.m_bRawImageType = INI_Preference.Get_Bool(strSection, "Raw Image Type(JPG)", TRUE);
		Struct_PreferenceStruct.m_bReduceRawImage = INI_Preference.Get_Bool(strSection, "Reduce Raw Image", FALSE);
		Struct_PreferenceStruct.m_dRawImageZoomRatio = INI_Preference.Get_Double(strSection, "Reduce Ratio", 0.3);
		Struct_PreferenceStruct.m_bCombineRawImage = INI_Preference.Get_Bool(strSection, "Combine Raw Image", FALSE);
		Struct_PreferenceStruct.m_bSaveResultGoodImage = INI_Preference.Get_Bool(strSection, "Save Result Good Image", TRUE);
		Struct_PreferenceStruct.m_bUseResultImageThreadOnlyGood = INI_Preference.Get_Bool(strSection, "Use Result Image Thread Only Good Sample", FALSE);
		Struct_PreferenceStruct.m_bRawImageSaveLogType = INI_Preference.Get_Bool(strSection, "RawImage Save Log Type", FALSE);
		Struct_PreferenceStruct.m_bAvoidDuplicateLotLocal = INI_Preference.Get_Bool(strSection, "Avoid Duplicate Lot Local", FALSE);
		Struct_PreferenceStruct.m_bAvoidDuplicateLotLas = INI_Preference.Get_Bool(strSection, "Avoid Duplicate Lot LAS", FALSE);

		Struct_PreferenceStruct.m_iRawImageThreadType = INI_Preference.Get_Integer(strSection, "Raw Image Thread Type", 1);

		Struct_PreferenceStruct.m_bUseMultiQueue = INI_Preference.Get_Bool(strSection, "Use Multi Queue", FALSE);
		Struct_PreferenceStruct.m_iThreadQueueNumber = INI_Preference.Get_Integer(strSection, "Thread Queue Number", 5);

		strSection = "RESULT IMAGE DISPLAY";
		Struct_PreferenceStruct.m_iResultImageDefectDisplayType = INI_Preference.Get_Integer(strSection, "Result Image Defect Display Type", 0);
		Struct_PreferenceStruct.m_bUseResultImageDefectSizeLimit = INI_Preference.Get_Bool(strSection, "Use Result Image Defect Size Limit", TRUE);
		Struct_PreferenceStruct.m_iResultImageDefectSizeMax = INI_Preference.Get_Integer(strSection, "Result Image Defect Size Max", 3);

		strSection = "ADJ OPTION";
		Struct_PreferenceStruct.m_bSaveADJ = INI_Preference.Get_Bool(strSection, "Save ADJ", FALSE);
		Struct_PreferenceStruct.m_bUseADJConnectionCheck = INI_Preference.Get_Bool(strSection, "Use ADJ Connection Check", FALSE);
		Struct_PreferenceStruct.m_bUseLocalADJ = INI_Preference.Get_Bool(strSection, "Use Local ADJ", FALSE);
		Struct_PreferenceStruct.m_bUseLocalSEG = INI_Preference.Get_Bool(strSection, "Use Local Segmentation", FALSE);
		Struct_PreferenceStruct.m_strCurrentADJ_IP = INI_Preference.Get_String(strSection, "AVI IP", "0.0.0.0");
		Struct_PreferenceStruct.m_bIsUseADJ[0] = INI_Preference.Get_Bool(strSection, "Use ADJ1", FALSE);
		Struct_PreferenceStruct.m_strADJ_IP[0] = INI_Preference.Get_String(strSection, "ADJ IP1", "127.0.0.1");
		Struct_PreferenceStruct.m_iADJ_Port[0] = INI_Preference.Get_Integer(strSection, "ADJ Port1", 9001);
		Struct_PreferenceStruct.m_bIsUseADJ[1] = INI_Preference.Get_Bool(strSection, "Use ADJ2", FALSE);
		Struct_PreferenceStruct.m_strADJ_IP[1] = INI_Preference.Get_String(strSection, "ADJ IP2", "127.0.0.1");
		Struct_PreferenceStruct.m_iADJ_Port[1] = INI_Preference.Get_Integer(strSection, "ADJ Port2", 9001);
		Struct_PreferenceStruct.m_bIsSendADJ = INI_Preference.Get_Bool(strSection, "Send ADJ", FALSE);
		Struct_PreferenceStruct.m_bUseADJMultiimage = INI_Preference.Get_Bool(strSection, "Use ADJ Multi Image", FALSE);
		Struct_PreferenceStruct.m_bIsApplyADJ = INI_Preference.Get_Bool(strSection, "Apply ADJ", FALSE);
		Struct_PreferenceStruct.m_bUseADJTimeOut = INI_Preference.Get_Bool(strSection, "Use ADJ Timeout", TRUE);
		Struct_PreferenceStruct.m_nADJTimeOut = INI_Preference.Get_Integer(strSection, "ADJ Judge Time", 500);
		Struct_PreferenceStruct.m_nADJ_MultiNetworkNO = INI_Preference.Get_Integer(strSection, "ADJ Multi Network Number", 1);
		Struct_PreferenceStruct.m_iADJImageSize = INI_Preference.Get_Integer(strSection, "ADJ Image Size", 256);
		Struct_PreferenceStruct.m_iADJImageReSize = INI_Preference.Get_Integer(strSection, "ADJ Image Resize", 256);
		Struct_PreferenceStruct.m_iMarkingImageRadius = INI_Preference.Get_Integer(strSection, "ADJ Marking Radius", 3);
		Struct_PreferenceStruct.m_iADJCombineDefectDistance = INI_Preference.Get_Integer(strSection, "ADJ Combine Defect Distance", 50);
		Struct_PreferenceStruct.m_bSaveROIImage = INI_Preference.Get_Bool(strSection, "Save ROI Image", FALSE);
		Struct_PreferenceStruct.m_iSaveROIImageType = INI_Preference.Get_Integer(strSection, "Save ROI Image Type", 1);
		Struct_PreferenceStruct.m_bAISetupUseVAL = INI_Preference.Get_Bool(strSection, "AI Setup Use Validation", FALSE);
		Struct_PreferenceStruct.m_bAllImageAiInspection = INI_Preference.Get_Bool(strSection, "ALL Image AI Inspection", FALSE);

		strSection = "MDJ OPTION";
		Struct_PreferenceStruct.m_bSaveMDJ = INI_Preference.Get_Bool(strSection, "Save MDJ", FALSE);
		Struct_PreferenceStruct.m_iMaxReviewImageNumber = INI_Preference.Get_Integer(strSection, "Max Review Image Number", 3);
		Struct_PreferenceStruct.m_iReviewCombineDefectDistance = INI_Preference.Get_Integer(strSection, "Review Combine Defect Distance", 200);
		Struct_PreferenceStruct.m_bUseSaveFaiReviewImage = INI_Preference.Get_Bool(strSection, "Save FAI Review Image", FALSE);

		Struct_PreferenceStruct.m_iReviewImageDisplayType = INI_Preference.Get_Integer(strSection, "Review Image Display Type", 0);

		strSection = "LIGHT CONTROLLER OPTION";
		Struct_PreferenceStruct.m_iVision1LightIP1 = INI_Preference.Get_Integer(strSection, "Vision1 Light IP1", 192);
		Struct_PreferenceStruct.m_iVision1LightIP2 = INI_Preference.Get_Integer(strSection, "Vision1 Light IP2", 168);
		Struct_PreferenceStruct.m_iVision1LightIP3 = INI_Preference.Get_Integer(strSection, "Vision1 Light IP3", 0);
		Struct_PreferenceStruct.m_iVision1LightIP4 = INI_Preference.Get_Integer(strSection, "Vision1 Light IP4", 101);
		Struct_PreferenceStruct.m_iVision1LightPort = INI_Preference.Get_Integer(strSection, "Vision1 Light Port", 5000);

		Struct_PreferenceStruct.m_iVision2LightIP1 = INI_Preference.Get_Integer(strSection, "Vision2 Light IP1", 192);
		Struct_PreferenceStruct.m_iVision2LightIP2 = INI_Preference.Get_Integer(strSection, "Vision2 Light IP2", 168);
		Struct_PreferenceStruct.m_iVision2LightIP3 = INI_Preference.Get_Integer(strSection, "Vision2 Light IP3", 0);
		Struct_PreferenceStruct.m_iVision2LightIP4 = INI_Preference.Get_Integer(strSection, "Vision2 Light IP4", 102);
		Struct_PreferenceStruct.m_iVision2LightPort = INI_Preference.Get_Integer(strSection, "Vision2 Light Port", 5000);

		Struct_PreferenceStruct.m_iVision3LightIP1 = INI_Preference.Get_Integer(strSection, "Vision3 Light IP1", 192);
		Struct_PreferenceStruct.m_iVision3LightIP2 = INI_Preference.Get_Integer(strSection, "Vision3 Light IP2", 168);
		Struct_PreferenceStruct.m_iVision3LightIP3 = INI_Preference.Get_Integer(strSection, "Vision3 Light IP3", 0);
		Struct_PreferenceStruct.m_iVision3LightIP4 = INI_Preference.Get_Integer(strSection, "Vision3 Light IP4", 103);
		Struct_PreferenceStruct.m_iVision3LightPort = INI_Preference.Get_Integer(strSection, "Vision3 Light Port", 5000);

		Struct_PreferenceStruct.m_iVision4LightIP1 = INI_Preference.Get_Integer(strSection, "Vision4 Light IP1", 192);
		Struct_PreferenceStruct.m_iVision4LightIP2 = INI_Preference.Get_Integer(strSection, "Vision4 Light IP2", 168);
		Struct_PreferenceStruct.m_iVision4LightIP3 = INI_Preference.Get_Integer(strSection, "Vision4 Light IP3", 0);
		Struct_PreferenceStruct.m_iVision4LightIP4 = INI_Preference.Get_Integer(strSection, "Vision4 Light IP4", 104);
		Struct_PreferenceStruct.m_iVision4LightPort = INI_Preference.Get_Integer(strSection, "Vision4 Light Port", 5000);

		strSection = "GRAB OPTION";
		Struct_PreferenceStruct.m_iGrabErrRetryNo = INI_Preference.Get_Integer(strSection, "Grab Error Retry Number", 1);
		Struct_PreferenceStruct.m_iGrabDelayTime = INI_Preference.Get_Integer(strSection, "Grab Delay Time", 20);
		Struct_PreferenceStruct.m_iTriggerSleepTime = INI_Preference.Get_Integer(strSection, "Trigger Sleep Time", 30);
		Struct_PreferenceStruct.m_iGrabDoneWaitTime = INI_Preference.Get_Integer(strSection, "Grab Done Wait Time", 500);
		Struct_PreferenceStruct.m_iTeachingLiveGrabPeriod = INI_Preference.Get_Integer(strSection, "Teaching Live Grab Period", 100);
		Struct_PreferenceStruct.m_iLightErrValue = INI_Preference.Get_Integer(strSection, "Light Error Value", 30);
		Struct_PreferenceStruct.m_iTeachingCameraLightType = INI_Preference.Get_Integer(strSection, "Teaching Camera Light Type", 0);
		Struct_PreferenceStruct.m_bUseGrabHold = INI_Preference.Get_Bool(strSection, "Use Grab Hold", TRUE);
		Struct_PreferenceStruct.m_iFocusMoveNoRetry = INI_Preference.Get_Integer(strSection, "Focus Move Retry Number", 3);
		Struct_PreferenceStruct.m_iFocusMoveCompleteWaitTime = INI_Preference.Get_Integer(strSection, "Focus Move Wait Time", 500);
		Struct_PreferenceStruct.m_bUse4dSaveRawImage = INI_Preference.Get_Bool(strSection, "Save 4D Raw Image", FALSE);
		Struct_PreferenceStruct.m_bUseAutoFocus = INI_Preference.Get_Bool(strSection, "Use Auto Focus", FALSE);
		Struct_PreferenceStruct.m_bUseFastGrab = INI_Preference.Get_Bool(strSection, "Use Fast Grab", FALSE);
		Struct_PreferenceStruct.m_bUseBatchInspection = INI_Preference.Get_Bool(strSection, "Use Batch Inspection", FALSE);	// 2025.01.06 - BATCH 검사 - LeeGW
		Struct_PreferenceStruct.m_bSendGrabFailError = INI_Preference.Get_Bool(strSection, "Send Grab Fail Error", FALSE);
		Struct_PreferenceStruct.m_iSendGrabFailErrorCount = INI_Preference.Get_Integer(strSection, "Send Grab Fail Error Count", 5);

		strSection = "OPTION ETC";
		Struct_PreferenceStruct.m_iDefectDispDist = INI_Preference.Get_Integer(strSection, "Defect Display Dist", 5);
		Struct_PreferenceStruct.m_iMatchingSearchMargin = INI_Preference.Get_Integer(strSection, "Matching Search Margin", 100);
		Struct_PreferenceStruct.m_iBarcodeNoReadImageType = INI_Preference.Get_Integer(strSection, "Barcode No Read Image Type(JPG)", 0);
		Struct_PreferenceStruct.m_bUseSaveDefectShapeFeature = INI_Preference.Get_Bool(strSection, "Save Defect Shape Feature", FALSE);
		Struct_PreferenceStruct.m_iMaxSameDefectShapeFeature = INI_Preference.Get_Integer(strSection, "Max Same Defect Shape Feature", 10);
		Struct_PreferenceStruct.m_bUseSaveGVEdgeMornitoringLog = INI_Preference.Get_Bool(strSection, "Save GV Edge Mornitoring Log", FALSE);
		Struct_PreferenceStruct.m_bUseGpuAffineTrans = INI_Preference.Get_Bool(strSection, "Use GPU Affine Trans", FALSE);
		Struct_PreferenceStruct.m_bUseDeleteCrossBar = INI_Preference.Get_Bool(strSection, "Use Delete Cross Bar ROI", TRUE);
		Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber = INI_Preference.Get_Integer(strSection, "Save Recently Complete Info Number", 30);

		strSection = "CONTINUOUS DEFECT ALARM OPTION";
		Struct_PreferenceStruct.m_bUseContDefectAlarm = INI_Preference.Get_Bool(strSection, "Use Continuous Defect Alarm", TRUE);
		Struct_PreferenceStruct.m_iContDefectPosTolerance = INI_Preference.Get_Integer(strSection, "Continuous Defect Pos Tolerance", 800);
		Struct_PreferenceStruct.m_iContDefectCount = INI_Preference.Get_Integer(strSection, "Continuous Defect Count", 3);

		strSection = "SAM2 OPTION";
		Struct_PreferenceStruct.m_iSam2IP_1 = INI_Preference.Get_Integer(strSection, "SAM2 IP1", 192);
		Struct_PreferenceStruct.m_iSam2IP_2 = INI_Preference.Get_Integer(strSection, "SAM2 IP2", 168);
		Struct_PreferenceStruct.m_iSam2IP_3 = INI_Preference.Get_Integer(strSection, "SAM2 IP3", 0);
		Struct_PreferenceStruct.m_iSam2IP_4 = INI_Preference.Get_Integer(strSection, "SAM2 IP4", 251);
		Struct_PreferenceStruct.m_iSam2Port = INI_Preference.Get_Integer(strSection, "SAM2 Port", 6000);

		strSection = "ROI COPY OPTION";
		Struct_PreferenceStruct.m_bUseVisionCopy = INI_Preference.Get_Bool(strSection, "Use Vision Copy", FALSE);

		Struct_PreferenceStruct.m_iDFMGrabComPortNumber = INI_Preference.Get_Integer(strSection, "DFM Grab COMPort Number", 5);
		Struct_PreferenceStruct.m_bUseConvertSingleThread = INI_Preference.Get_Bool(strSection, "Use DFM Convert Single Thread", TRUE);

		strSection = "IMAGE COMPARE TRAIN OPTION";
		Struct_PreferenceStruct.m_iVmTrain = INI_Preference.Get_Integer(strSection, "VM TRAIN", 0);
		Struct_PreferenceStruct.m_bVmTrainAll = INI_Preference.Get_Bool(strSection, "VM ALL TRAINING", FALSE);

		Struct_PreferenceStruct.m_bUseVmThresOpt = INI_Preference.Get_Bool(strSection, "USE VM THRES OPT", FALSE);
		Struct_PreferenceStruct.m_iVmSplitThres = INI_Preference.Get_Integer(strSection, "VM SPLIT THRES", 10);
		Struct_PreferenceStruct.m_dVmKSigma = INI_Preference.Get_Double(strSection, "VM K SIGMA", 3.0);

		Struct_PreferenceStruct.m_iVmOptThrDetect = INI_Preference.Get_Integer(strSection, "VM OPT THRES DETECT", 10);
		Struct_PreferenceStruct.m_iVmOptMinPixelRatio = INI_Preference.Get_Integer(strSection, "VM OPT MIN PIXEL RATIO", 10);
		Struct_PreferenceStruct.m_iVmOptAbsThresMin = INI_Preference.Get_Integer(strSection, "VM OPT ABS THRES MIN", 5);
		Struct_PreferenceStruct.m_iVmOptAbsThresStep = INI_Preference.Get_Integer(strSection, "VM OPT ABS THRES STEP", 1);
		Struct_PreferenceStruct.m_dVmOptVarThresMin = INI_Preference.Get_Double(strSection, "VM OPT VAR THRES MIN", 0.5);
		Struct_PreferenceStruct.m_dVmOptVarThresStep = INI_Preference.Get_Double(strSection, "VM OPT VAR THRES STEP", 0.2);
		Struct_PreferenceStruct.m_iVmOptIouTarget = INI_Preference.Get_Integer(strSection, "VM OPT IOU TARGET", 70);
	}
	else
	{
		strSection = "EQUIPMENT NO";
		Struct_PreferenceStruct.m_strEquipNo = INI_Preference.Get_String(strSection, "EquipNo", "#0");

		strSection = "INSPECT VISION";
		Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_1] = INI_Preference.Get_Bool(strSection, "Use Vision1", TRUE);
		Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_2] = INI_Preference.Get_Bool(strSection, "Use Vision2", TRUE);
		Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_3] = INI_Preference.Get_Bool(strSection, "Use Vision3", FALSE);
		Struct_PreferenceStruct.m_bUseVision[VISION_NUMBER_4] = INI_Preference.Get_Bool(strSection, "Use Vision4", FALSE);
		for (int i = 0; i < VISION_NUMBER_MAX; i++)
		{
			if (m_ModelDefineInfo.m_strVisionName[i] != "Undefine")
				m_ModelDefineInfo.m_iUseVisionNo++;
			else
				Struct_PreferenceStruct.m_bUseVision[i] = FALSE;
		}

		strSection = "FOLDER PATH";
		Struct_PreferenceStruct.m_bUseLAS1 = INI_Preference.Get_Bool(strSection, "Use LAS1", FALSE);
		Struct_PreferenceStruct.m_bUseLAS2 = INI_Preference.Get_Bool(strSection, "Use LAS2", FALSE);
		Struct_PreferenceStruct.m_bUseLAS3 = INI_Preference.Get_Bool(strSection, "Use LAS3", FALSE);

		Struct_PreferenceStruct.m_strSaveFolderPathLAS1 = INI_Preference.Get_String(strSection, "Save Folder Path LAS1", "R:\\");
		Struct_PreferenceStruct.m_strSaveFolderPathLAS2 = INI_Preference.Get_String(strSection, "Save Folder Path LAS2", "S:\\");
		Struct_PreferenceStruct.m_strSaveFolderPathLAS3 = INI_Preference.Get_String(strSection, "Save Folder Path LAS3", "T:\\");

		Struct_PreferenceStruct.m_bUseLASConnectionCheck = INI_Preference.Get_Bool(strSection, "USE_LAS_CONNECTION_CHECK", FALSE);

		Struct_PreferenceStruct.m_strRawFolderPath = INI_Preference.Get_String(strSection, "Save Raw Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strResultFolderPath = INI_Preference.Get_String(strSection, "Save Result Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strReviewFolderPath = INI_Preference.Get_String(strSection, "Save Review Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strAdjFolderPath = INI_Preference.Get_String(strSection, "Save ADJ Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strResultLogFolderPath = INI_Preference.Get_String(strSection, "Save Result Log Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strEtcFolderPath = INI_Preference.Get_String(strSection, "Save ETC Folder Path", "D:\\");
		Struct_PreferenceStruct.m_strOfflineTestFolderPath = INI_Preference.Get_String(strSection, "Debug Folder Path", "C:\\");

		strSection = "IMAGE SAVE OPTION";
		Struct_PreferenceStruct.m_bRawImageSaveLocal = INI_Preference.Get_Bool(strSection, "Raw Image Save Local", TRUE);
		Struct_PreferenceStruct.m_bResultImageSaveLocal = INI_Preference.Get_Bool(strSection, "Result Image Save Local", TRUE);
		Struct_PreferenceStruct.m_bReviewImageSaveLocal = INI_Preference.Get_Bool(strSection, "Review Image Save Local", TRUE);
		Struct_PreferenceStruct.m_bADJImageSaveLocal = INI_Preference.Get_Bool(strSection, "ADJ Image Save Local", TRUE);
		Struct_PreferenceStruct.m_bResultLogSaveLocal = INI_Preference.Get_Bool(strSection, "Result Log Save Local", TRUE);

		Struct_PreferenceStruct.m_bRawImageSaveLAS = INI_Preference.Get_Bool(strSection, "Raw Image Save LAS", FALSE);
		Struct_PreferenceStruct.m_bResultImageSaveLAS = INI_Preference.Get_Bool(strSection, "Result Image Save LAS", FALSE);
		Struct_PreferenceStruct.m_bReviewImageSaveLAS = INI_Preference.Get_Bool(strSection, "Review Image Save LAS", FALSE);
		Struct_PreferenceStruct.m_bADJImageSaveLAS = INI_Preference.Get_Bool(strSection, "ADJ Image Save LAS", FALSE);
		Struct_PreferenceStruct.m_bResultLogSaveLAS = INI_Preference.Get_Bool(strSection, "Result Log Save LAS", FALSE);

		Struct_PreferenceStruct.m_bUseRawImageThread = INI_Preference.Get_Bool(strSection, "Use Raw Image Thread", TRUE);
		Struct_PreferenceStruct.m_bUseResultImageThread = INI_Preference.Get_Bool(strSection, "Use Result Image Thread", FALSE);
		Struct_PreferenceStruct.m_bUseReviewImageThread = INI_Preference.Get_Bool(strSection, "Use Review Image Thread", FALSE);
		Struct_PreferenceStruct.m_bUseADJImageThread = INI_Preference.Get_Bool(strSection, "Use ADJ Image Thread", FALSE);
		Struct_PreferenceStruct.m_bUseResultLogThread = INI_Preference.Get_Bool(strSection, "Use Result Log Thread", TRUE);

		Struct_PreferenceStruct.m_bUseRawImageTempDrive = INI_Preference.Get_Bool(strSection, "Use Raw Image Temporary Drive", FALSE);

		Struct_PreferenceStruct.m_bRawImageType = INI_Preference.Get_Bool(strSection, "Raw Image Type(JPG)", TRUE);
		Struct_PreferenceStruct.m_bSaveResultGoodImage = INI_Preference.Get_Bool(strSection, "Save Result Good Image", TRUE);
		Struct_PreferenceStruct.m_bUseResultImageThreadOnlyGood = INI_Preference.Get_Bool(strSection, "Use Result Image Thread Only Good Sample", FALSE);
		Struct_PreferenceStruct.m_bRawImageSaveLogType = INI_Preference.Get_Bool(strSection, "RawImage Save Log Type", FALSE);
		Struct_PreferenceStruct.m_bAvoidDuplicateLotLocal = INI_Preference.Get_Bool(strSection, "Avoid Duplicate Lot Local", FALSE);
		Struct_PreferenceStruct.m_bAvoidDuplicateLotLas = INI_Preference.Get_Bool(strSection, "Avoid Duplicate Lot LAS", FALSE);

		Struct_PreferenceStruct.m_bUseMultiQueue = INI_Preference.Get_Bool(strSection, "Use Multi Queue", FALSE);
		Struct_PreferenceStruct.m_iThreadQueueNumber = INI_Preference.Get_Integer(strSection, "Thread Queue Number", 5);

		strSection = "FLUX DENT ALARM";
		Struct_PreferenceStruct.m_bUseContDefectAlarm = INI_Preference.Get_Bool(strSection, "USE_FLUX_DENT_ALARM", TRUE);
		Struct_PreferenceStruct.m_iContDefectPosTolerance = INI_Preference.Get_Integer(strSection, "FLUX_DENT_SAME_POS_TOLERANCE", 800);
		Struct_PreferenceStruct.m_iContDefectCount = INI_Preference.Get_Integer(strSection, "FLUX_DENT_CONT_DEFECT_NUMBER", 3);

		strSection = "Option_Etc";
		Struct_PreferenceStruct.m_iPCType = INI_Preference.Get_Integer(strSection, "INSPECTION_PC_TYPE", 0);

		Struct_PreferenceStruct.m_bSaveRawImage = INI_Preference.Get_Bool(strSection, "SAVE_RAW_IMAGE", TRUE);
		Struct_PreferenceStruct.m_bReduceRawImage = INI_Preference.Get_Bool(strSection, "REDUCE_RAW_IMAGE", FALSE);
		Struct_PreferenceStruct.m_dRawImageZoomRatio = INI_Preference.Get_Double(strSection, "RAW_IMAGE_ZOOM_RATIO", 0.3);
		Struct_PreferenceStruct.m_bCombineRawImage = INI_Preference.Get_Bool(strSection, "COMBINE_RAW_IMAGE", FALSE);

		Struct_PreferenceStruct.m_iRawImageThreadType = INI_Preference.Get_Integer(strSection, "RAW_IMAGE_THREAD_TYPE", 1);

		Struct_PreferenceStruct.m_iResultImageDefectDisplayType = INI_Preference.Get_Integer(strSection, "RESULT_IMAGE_DEFECT_DISPLAY_TYPE", 0);
		Struct_PreferenceStruct.m_bUseResultImageDefectSizeLimit = INI_Preference.Get_Bool(strSection, "USE_RESULT_IMAGE_DEFECT_SIZE_LIMIT", TRUE);
		Struct_PreferenceStruct.m_iResultImageDefectSizeMax = INI_Preference.Get_Integer(strSection, "RESULT_IMAGE_DEFECT_SIZE_MAX", 3);

		Struct_PreferenceStruct.m_bSaveADJ = INI_Preference.Get_Bool(strSection, "SAVE_ADJ", FALSE);
		Struct_PreferenceStruct.m_bUseADJConnectionCheck = INI_Preference.Get_Bool(strSection, "USE_ADJ_CONNECTION_CHECK", FALSE);
		Struct_PreferenceStruct.m_bUseLocalADJ = INI_Preference.Get_Bool(strSection, "USE_LOCAL_ADJ", FALSE);
		Struct_PreferenceStruct.m_bUseLocalSEG = INI_Preference.Get_Bool(strSection, "USE_LOCAL_SEG", FALSE);
		Struct_PreferenceStruct.m_strCurrentADJ_IP = INI_Preference.Get_String(strSection, "Current_IP", "0.0.0.0");
		Struct_PreferenceStruct.m_bIsUseADJ[0] = INI_Preference.Get_Bool(strSection, "USE_ADJ0", FALSE);
		Struct_PreferenceStruct.m_strADJ_IP[0] = INI_Preference.Get_String(strSection, "ADJ_IP0", "127.0.0.1");
		Struct_PreferenceStruct.m_iADJ_Port[0] = INI_Preference.Get_Integer(strSection, "ADJ_PORT0", 9001);
		Struct_PreferenceStruct.m_bIsUseADJ[1] = INI_Preference.Get_Bool(strSection, "USE_ADJ1", FALSE);
		Struct_PreferenceStruct.m_strADJ_IP[1] = INI_Preference.Get_String(strSection, "ADJ_IP0", "127.0.0.1");
		Struct_PreferenceStruct.m_iADJ_Port[1] = INI_Preference.Get_Integer(strSection, "ADJ_PORT1", 9001);
		Struct_PreferenceStruct.m_bIsSendADJ = INI_Preference.Get_Bool(strSection, "SEND_ADJ", FALSE);
		Struct_PreferenceStruct.m_bUseADJMultiimage = INI_Preference.Get_Bool(strSection, "USE_ADJ_MULTIIMAGE", FALSE);
		Struct_PreferenceStruct.m_bIsApplyADJ = INI_Preference.Get_Bool(strSection, "APPLY_ADJ", FALSE);
		Struct_PreferenceStruct.m_bUseADJTimeOut = INI_Preference.Get_Bool(strSection, "USE_ADJ_TIMEOUT", TRUE);
		Struct_PreferenceStruct.m_nADJTimeOut = INI_Preference.Get_Integer(strSection, "ADJ_TIMEOUT", 500);
		Struct_PreferenceStruct.m_nADJ_MultiNetworkNO = INI_Preference.Get_Integer(strSection, "MultiADJ MAX", 1);
		Struct_PreferenceStruct.m_iADJImageSize = INI_Preference.Get_Integer(strSection, "ADJ_IMAGE_SIZE", 256);
		Struct_PreferenceStruct.m_iADJImageReSize = INI_Preference.Get_Integer(strSection, "ADJ_IMAGE_RESIZE", 256);
		Struct_PreferenceStruct.m_iMarkingImageRadius = INI_Preference.Get_Integer(strSection, "ADJ_MARKING_RADIUS", 3);
		Struct_PreferenceStruct.m_iADJCombineDefectDistance = INI_Preference.Get_Integer(strSection, "ADJ_COMBINE_DEFECT_DISTANCE", 50);
		Struct_PreferenceStruct.m_bSaveROIImage = INI_Preference.Get_Bool(strSection, "SAVE_ROI_IMAGE", FALSE);
		Struct_PreferenceStruct.m_iSaveROIImageType = INI_Preference.Get_Integer(strSection, "SAVE_ROI_IMAGE_TYPE", 1);
		Struct_PreferenceStruct.m_bAISetupUseVAL = INI_Preference.Get_Bool(strSection, "AI_SETUP_USE_VAL", FALSE);
		Struct_PreferenceStruct.m_bAllImageAiInspection = INI_Preference.Get_Bool(strSection, "ALL Image AI Inspection", FALSE);

		Struct_PreferenceStruct.m_bSaveMDJ = INI_Preference.Get_Bool(strSection, "SAVE_MDJ", FALSE);
		Struct_PreferenceStruct.m_iMaxReviewImageNumber = INI_Preference.Get_Integer(strSection, "MAX_REVIEW_IMAGE_NUMBER", 3);
		Struct_PreferenceStruct.m_iReviewCombineDefectDistance = INI_Preference.Get_Integer(strSection, "REVIEW_COMBINE_DEFECT_DISTANCE", 200);
		Struct_PreferenceStruct.m_bUseSaveFaiReviewImage = INI_Preference.Get_Bool(strSection, "USE_SAVE_FAI_REVIEW_IMAGE", FALSE);

		Struct_PreferenceStruct.m_iReviewImageDisplayType = INI_Preference.Get_Integer(strSection, "REVIEWIMAGE_DISPLAY", 0);

		Struct_PreferenceStruct.m_iVision1LightIP1 = INI_Preference.Get_Integer(strSection, "TOP_LIGHT_IP_1", 192);
		Struct_PreferenceStruct.m_iVision1LightIP2 = INI_Preference.Get_Integer(strSection, "TOP_LIGHT_IP_2", 168);
		Struct_PreferenceStruct.m_iVision1LightIP3 = INI_Preference.Get_Integer(strSection, "TOP_LIGHT_IP_3", 0);
		Struct_PreferenceStruct.m_iVision1LightIP4 = INI_Preference.Get_Integer(strSection, "TOP_LIGHT_IP_4", 101);
		Struct_PreferenceStruct.m_iVision1LightPort = INI_Preference.Get_Integer(strSection, "TOP_LIGHT_PORT", 5000);

		Struct_PreferenceStruct.m_iVision2LightIP1 = INI_Preference.Get_Integer(strSection, "BOTTOM_LIGHT_IP_1", 192);
		Struct_PreferenceStruct.m_iVision2LightIP2 = INI_Preference.Get_Integer(strSection, "BOTTOM_LIGHT_IP_2", 168);
		Struct_PreferenceStruct.m_iVision2LightIP3 = INI_Preference.Get_Integer(strSection, "BOTTOM_LIGHT_IP_3", 0);
		Struct_PreferenceStruct.m_iVision2LightIP4 = INI_Preference.Get_Integer(strSection, "BOTTOM_LIGHT_IP_4", 102);
		Struct_PreferenceStruct.m_iVision2LightPort = INI_Preference.Get_Integer(strSection, "BOTTOM_LIGHT_PORT", 5000);

		Struct_PreferenceStruct.m_iGrabErrRetryNo = INI_Preference.Get_Integer(strSection, "GRAB_ERROR_RETRY_NUMBER", 1);
		Struct_PreferenceStruct.m_iGrabDelayTime = INI_Preference.Get_Integer(strSection, "GRAB_DELAY_TIME", 20);
		Struct_PreferenceStruct.m_iTriggerSleepTime = INI_Preference.Get_Integer(strSection, "TRIGGER_SLEEP_TIME", 30);
		Struct_PreferenceStruct.m_iGrabDoneWaitTime = INI_Preference.Get_Integer(strSection, "GRAB_DONE_WAIT_TIME", 500);
		Struct_PreferenceStruct.m_iTeachingLiveGrabPeriod = INI_Preference.Get_Integer(strSection, "TEACHING_LIVE_GRAB_PERIOD", 100);
		Struct_PreferenceStruct.m_iLightErrValue = INI_Preference.Get_Integer(strSection, "LIGHT_ERROR_VALUE", 30);
		Struct_PreferenceStruct.m_iTeachingCameraLightType = INI_Preference.Get_Integer(strSection, "TEACHING_CAMERA_LIGHT_TYPE", 0);
		Struct_PreferenceStruct.m_bUseGrabHold = INI_Preference.Get_Bool(strSection, "USE_GRAB_HOLD", TRUE);
		Struct_PreferenceStruct.m_iFocusMoveNoRetry = INI_Preference.Get_Integer(strSection, "FOCUS_MOVE_NO_RETRY", 3);
		Struct_PreferenceStruct.m_iFocusMoveCompleteWaitTime = INI_Preference.Get_Integer(strSection, "FOCUS_MOVE_COMPLETE_WAIT_TIME", 500);
		Struct_PreferenceStruct.m_bUse4dSaveRawImage = INI_Preference.Get_Bool(strSection, "USE_4D_SAVE_RAW_IMAGE", FALSE);
		Struct_PreferenceStruct.m_bUseAutoFocus = INI_Preference.Get_Bool(strSection, "USE_AUTO_FOCUS", FALSE);
		Struct_PreferenceStruct.m_bUseFastGrab = INI_Preference.Get_Bool(strSection, "USE_FAST_GRAB", FALSE);
		Struct_PreferenceStruct.m_bUseBatchInspection = INI_Preference.Get_Bool(strSection, "USE_BATCH_INSPECTION", FALSE);
		Struct_PreferenceStruct.m_bSendGrabFailError = INI_Preference.Get_Bool(strSection, "SEND_GRAB_FAIL_ERROR", FALSE);
		Struct_PreferenceStruct.m_iSendGrabFailErrorCount = INI_Preference.Get_Integer(strSection, "SEND_GRAB_FALI_ERROR_COUNT", 5);

		Struct_PreferenceStruct.m_iDefectDispDist = INI_Preference.Get_Integer(strSection, "Defect_Display_Dist", 5);
		Struct_PreferenceStruct.m_iMatchingSearchMargin = INI_Preference.Get_Integer(strSection, "MATCHING_SEARCH_MARGIN", 100);
		Struct_PreferenceStruct.m_iBarcodeNoReadImageType = INI_Preference.Get_Integer(strSection, "BARCODE_NO_READ_IMAGE_TYPE", 0);
		Struct_PreferenceStruct.m_bUseSaveDefectShapeFeature = INI_Preference.Get_Bool(strSection, "USE_SAVE_DEFECT_SHAPE_FEATURE", FALSE);
		Struct_PreferenceStruct.m_iMaxSameDefectShapeFeature = INI_Preference.Get_Integer(strSection, "MAX_SAME_DEFECT_SHAPE_FEATURE", 10);
		Struct_PreferenceStruct.m_bUseSaveGVEdgeMornitoringLog = INI_Preference.Get_Bool(strSection, "USE_SAVE_GV_EDGE_MORNITORING_LOG", FALSE);
		Struct_PreferenceStruct.m_bUseGpuAffineTrans = INI_Preference.Get_Bool(strSection, "USE_GPU_AFFINE_TRANS", FALSE);
		Struct_PreferenceStruct.m_bUseDeleteCrossBar = INI_Preference.Get_Bool(strSection, "USE_DELETE_CROSS_BAR", TRUE);
		Struct_PreferenceStruct.m_iSaveRecentlyCompleteInfoNumber = INI_Preference.Get_Integer(strSection, "SAVE_RECENTLY_COMPLETE_INFO_NUMBER", 30);

		Struct_PreferenceStruct.m_iSam2IP_1 = INI_Preference.Get_Integer(strSection, "SAM2_IP_1", 192);
		Struct_PreferenceStruct.m_iSam2IP_2 = INI_Preference.Get_Integer(strSection, "SAM2_IP_2", 168);
		Struct_PreferenceStruct.m_iSam2IP_3 = INI_Preference.Get_Integer(strSection, "SAM2_IP_3", 0);
		Struct_PreferenceStruct.m_iSam2IP_4 = INI_Preference.Get_Integer(strSection, "SAM2_IP_4", 251);
		Struct_PreferenceStruct.m_iSam2Port = INI_Preference.Get_Integer(strSection, "SAM2_PORT", 6000);

		Struct_PreferenceStruct.m_bUseVisionCopy = INI_Preference.Get_Bool(strSection, "USE_VISION_COPY", FALSE);

		Struct_PreferenceStruct.m_iDFMGrabComPortNumber = INI_Preference.Get_Integer(strSection, "DFM_GRAB_COM_PORT_NUMBER", 5);
		Struct_PreferenceStruct.m_bUseConvertSingleThread = INI_Preference.Get_Bool(strSection, "USE_DFM_CONVERT_SINGLE_THREAD", TRUE);

		Struct_PreferenceStruct.m_iVmTrain = INI_Preference.Get_Integer(strSection, "VM TRAIN", 0);
		Struct_PreferenceStruct.m_bVmTrainAll = INI_Preference.Get_Bool(strSection, "VM ALL TRAINING", FALSE);

		Struct_PreferenceStruct.m_bUseVmThresOpt = INI_Preference.Get_Bool(strSection, "USE VM THRES OPT", FALSE);
		Struct_PreferenceStruct.m_iVmSplitThres = INI_Preference.Get_Integer(strSection, "VM SPLIT THRES", 10);
		Struct_PreferenceStruct.m_dVmKSigma = INI_Preference.Get_Double(strSection, "VM K SIGMA", 3.0);

		Struct_PreferenceStruct.m_iVmOptThrDetect = INI_Preference.Get_Integer(strSection, "VM OPT THRES DETECT", 10);
		Struct_PreferenceStruct.m_iVmOptMinPixelRatio = INI_Preference.Get_Integer(strSection, "VM OPT MIN PIXEL RATIO", 10);
		Struct_PreferenceStruct.m_iVmOptAbsThresMin = INI_Preference.Get_Integer(strSection, "VM OPT ABS THRES MIN", 5);
		Struct_PreferenceStruct.m_iVmOptAbsThresStep = INI_Preference.Get_Integer(strSection, "VM OPT ABS THRES STEP", 1);
		Struct_PreferenceStruct.m_dVmOptVarThresMin = INI_Preference.Get_Double(strSection, "VM OPT VAR THRES MIN", 0.5);
		Struct_PreferenceStruct.m_dVmOptVarThresStep = INI_Preference.Get_Double(strSection, "VM OPT VAR THRES STEP", 0.2);
		Struct_PreferenceStruct.m_iVmOptIouTarget = INI_Preference.Get_Integer(strSection, "VM OPT IOU TARGET", 70);
	}


	//////////////////////////////////////////////////////////////////////////
	///  Defect Info Set
	CIniFileCS INI_DefectInfo(strModelFolder + "\\DefectInfo.ini");

	strSection = "Special_Code";
	strKey.Format("NO_ASSIGN_SPECIAL_CODE");
	Struct_PreferenceStruct.iNoAssignSpecialCode = INI_DefectInfo.Get_Integer(strSection, strKey, 0);
	for (int i = 0; i < MAX_DEFECT_NAME; i++)
	{
		strKey.Format("ASSIGN_SPECIAL_CODE_%d", i);
		Struct_PreferenceStruct.m_sAssignSpecialCode[i] = INI_DefectInfo.Get_String(strSection, strKey, "NA");

		if (i == Struct_PreferenceStruct.iNoAssignSpecialCode)
			break;
	}

	strSection = "Defect_Special";
	for (int i = 0; i < MAX_DEFECT_NAME; i++)
	{
		strKey.Format("NG_SPECIAL_CODE_%d", i);
		Struct_PreferenceStruct.m_iSpecialNGCodeIndex[i] = INI_DefectInfo.Get_Integer(strSection, strKey, 0);
	}

	strSection = "Defect_Priority";
	for (int i = 0; i < MAX_DEFECT_NAME; i++)
	{
		strKey.Format("NG_PRIORITY_%d", i);
		Struct_PreferenceStruct.iDefectPriority[i] = INI_DefectInfo.Get_Integer(strSection, strKey, i);
	}

	strSection = "InspectionType_Priority";
	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		strKey.Format("INSPECTION_TYPE_PRIORITY_%d", i + 1);
		Struct_PreferenceStruct.iInspectionTypePriority[i] = INI_DefectInfo.Get_Integer(strSection, strKey, i);
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	///  ROS Image Viewport Set
	CIniFileCS INI_RosViewport(strModelFolder + "\\RosViewport.ini");

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		strSection = m_ModelDefineInfo.m_strVisionName[i];
		strKey.Format("Left");
		Struct_PreferenceStruct.iRosImageViewportLeft[i] = INI_RosViewport.Get_Integer(strSection, strKey, 0);
		strKey.Format("Top");
		Struct_PreferenceStruct.iRosImageViewportTop[i] = INI_RosViewport.Get_Integer(strSection, strKey, 0);
		strKey.Format("Zoom");
		Struct_PreferenceStruct.dRosImageViewportZoomRatio[i] = INI_RosViewport.Get_Double(strSection, strKey, 1.0);
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	///  ADJ Set

	CIniFileCS INI_ADJDefectPriority(strModelFolder + "\\ADJDefectPriority.ini");

	strSection = "ADJDefectPriority";
	for (int cnt = 0; cnt < DEEP_MODEL_NUM * MAX_ADJ_CONNECT_NUM; cnt++)
	{
		strKey.Format("Model%d", cnt + 1);
		m_nADJDefectPriority[cnt] = INI_ADJDefectPriority.Get_Integer(strSection, strKey, DEEP_MODEL_NUM * MAX_ADJ_CONNECT_NUM); // i는 ADJ 모델 번호이고 들어가는 정수 값은 priority
	}

	CIniFileCS INI_ADJDefectSetting(strModelFolder + "\\ADJDefectSetting.ini");

	strSection = "Defect_ADJ_Assign";
	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			strKey.Format("ADJ_Defect_Name_%d", DEEP_MODEL_NUM * i + j + 1);
			Struct_PreferenceStruct.m_strADJ_Defect_Name[i][j] = INI_ADJDefectSetting.Get_String(strSection, strKey, "none");
			strKey.Format("ADJ_Defect_Pos_%d", DEEP_MODEL_NUM * i + j + 1);
			Struct_PreferenceStruct.m_strADJ_Defect_Pos[i][j] = INI_ADJDefectSetting.Get_String(strSection, strKey, "none");
			strKey.Format("ADJ_Defect_ThreadNO_%d", DEEP_MODEL_NUM * i + j + 1);
			Struct_PreferenceStruct.m_nADJThreadNO[i][j] = INI_ADJDefectSetting.Get_Integer(strSection, strKey, 0);
			strKey.Format("ADJ_Defect_SideTotal_%d", DEEP_MODEL_NUM * i + j + 1);
			Struct_PreferenceStruct.m_bSideTotal[i][j] = INI_ADJDefectSetting.Get_Bool(strSection, strKey, FALSE);
		}
	}

	strSection = "Total_defect_ADJ";
	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			int k = 0;
			strKey.Format("ADJ_Integrated_%d", DEEP_MODEL_NUM * i + j + 1);

			CString Integrated_Defect_Code = INI_ADJDefectSetting.Get_String(strSection, strKey, "none");
			char* sep = ",";
			char* sep2 = "_";
			char* psTemp = (LPTSTR)(LPCTSTR)Integrated_Defect_Code;
			char* Division_ADJ_Defect_Code = strtok_s(psTemp, sep, &psTemp);
			while (Division_ADJ_Defect_Code != NULL)
			{
				char* ADJCode = strtok(Division_ADJ_Defect_Code, sep2);
				Struct_PreferenceStruct.m_strADJ_Defect_Integrated_Pos[i][j][k] = strtok(NULL, sep2);
				Struct_PreferenceStruct.m_strADJ_Defect_Integrated_Name[i][j][k] = strtok(NULL, sep2);

				for (int l = 0; l < VISION_NUMBER_MAX; l++)
				{
					CString sVisionCamType_Short;
					sVisionCamType_Short = m_ModelDefineInfo.m_strVisionName_Short[l];
					if (sVisionCamType_Short == ADJCode)
					{
						Struct_PreferenceStruct.m_nADJThreadNO_Integrated[i][j][k] = l;
					}
				}
				k++;
				Division_ADJ_Defect_Code = strtok_s(NULL, sep, &psTemp);
			}
		}
	}
	strSection = "Total_defect_Segment_ADJ";
	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			int k = 0;
			strKey.Format("ADJ_Integrated_%d", DEEP_MODEL_NUM * i + j + 1);

			CString Integrated_Defect_Code = INI_ADJDefectSetting.Get_String(strSection, strKey, "none");
			char* sep = ",";
			char* sep2 = "_";
			char* psTemp = (LPTSTR)(LPCTSTR)Integrated_Defect_Code;
			char* Division_ADJ_Defect_Code = strtok_s(psTemp, sep, &psTemp);
			while (Division_ADJ_Defect_Code != NULL)
			{
				char* ADJCode = strtok(Division_ADJ_Defect_Code, sep2);
				Struct_PreferenceStruct.m_strADJ_Defect_Integrated_Segment_Pos[i][j][k] = strtok(NULL, sep2);
				Struct_PreferenceStruct.m_strADJ_Defect_Integrated_Segment_Name[i][j][k] = strtok(NULL, sep2);

				for (int l = 0; l < VISION_NUMBER_MAX; l++)
				{
					CString sVisionCamType_Short;
					sVisionCamType_Short = m_ModelDefineInfo.m_strVisionName_Short[l];
					if (sVisionCamType_Short == ADJCode)
					{
						Struct_PreferenceStruct.m_nADJThreadNO_Integrated_Segment[i][j][k] = l;
						break;
					}
				}
				k++;
				Division_ADJ_Defect_Code = strtok_s(NULL, sep, &psTemp);
			}
		}
	}

	strSection = "FAI_ITEM_MODEL";
	for (int i = 0; i < MAX_ADJ_CONNECT_NUM; i++)
	{
		for (int j = 0; j < DEEP_MODEL_NUM; j++)
		{
			int k = 0;
			strKey.Format("FAI_ADJ_Integrated_MODEL_%d", DEEP_MODEL_NUM * i + j + 1);

			CString cstrFAI_ADJ_Integrated_Model = INI_ADJDefectSetting.Get_String(strSection, strKey, "none");
			char* sep = ",";
			char* psTemp = (LPTSTR)(LPCTSTR)cstrFAI_ADJ_Integrated_Model;
			char* m_pchFAI_ADJ_Integrated_Model = strtok_s(psTemp, sep, &psTemp);
			while (m_pchFAI_ADJ_Integrated_Model != NULL)
			{
				Struct_PreferenceStruct.m_nADJFAIModel_Integrated[i][j][k] = atoi(strtok(m_pchFAI_ADJ_Integrated_Model, sep));

				k++;
				m_pchFAI_ADJ_Integrated_Model = strtok_s(NULL, sep, &psTemp);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	if (Struct_PreferenceStruct.m_bRawImageSaveLAS || Struct_PreferenceStruct.m_bResultImageSaveLAS || Struct_PreferenceStruct.m_bReviewImageSaveLAS ||
		Struct_PreferenceStruct.m_bADJImageSaveLAS || Struct_PreferenceStruct.m_bResultLogSaveLAS)
	{
		for (int i = 0; i < MAX_MAGAZINE_NO; i++)
			ChangeLAS(i);
	}
}


std::optional<SHELLEXECUTEINFOA> CuScanApp::StartAISetupSW(std::string mode, int step, std::string model_name)
{
	// --- 환경 변수 SYAIAI_VI_SETUP_PATH 읽기 ---
	DWORD dwSizeWithNull = GetEnvironmentVariableA("SYAIAI_VI_SETUP_PATH", NULL, 0);

	// 환경 변수가 설정되지 않은 경우 즉시 오류 처리 및 반환
	if (dwSizeWithNull == 0)
	{
		CString strMessage;
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessage.Format(_T("환경 변수 SYAIAI_VI_SETUP_PATH가 설정되지 않았습니다."));
		else
			strMessage.Format(_T("Environment variable SYAIAI_VI_SETUP_PATH is not set."));
		AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONWARNING);

		strLog.Format(_T("Environment variable SYAIAI_VI_SETUP_PATH is not set"));
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		return std::nullopt; // 즉시 함수 종료
	}

	// 환경 변수 값 읽기
	std::vector<char> buffer(dwSizeWithNull);
	GetEnvironmentVariableA("SYAIAI_VI_SETUP_PATH", buffer.data(), dwSizeWithNull);
	std::string strSetupPathA(buffer.data());

	strLog.Format(_T("SYAIAI_VI_SETUP_PATH: %s"), strSetupPathA.c_str());
	THEAPP.m_log_adj->info("{}", LOG_CSTR(strLog));

	std::string backupPath = "J:\\synapse_ai_cpp_runtime\\SYAI.Runtime.CS.Inspection.Setup\\bin\\Debug\\net8.0-windows\\SYAI.Runtime.CS.Inspection.Setup.exe";

	// 파일 존재 여부 확인
	// 파일이 존재하지 않는 경우 즉시 오류 처리 및 반환
	if (!PathFileExistsA(strSetupPathA.c_str()))
	{
		CString strMessage;
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessage.Format(_T("AI Setup Wizard 파일을 찾을 수 없습니다: %s"), strSetupPathA.c_str());
		else
			strMessage.Format(_T("AI Setup Wizard file not found: %s"), strSetupPathA.c_str());
		AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONWARNING);

		strLog.Format(_T("AI Setup Wizardfile not found, Path: %s"), strSetupPathA.c_str());
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		
		// 백업 경로 시도
		if (PathFileExistsA(backupPath.c_str())) {
			CString strMessage;
			if (THEAPP.m_iPGMLanguageSelect == 0)
				strMessage.Format(_T("환경 변수 SYAIAI_VI_SETUP_PATH가 설정되지 않았습니다. 백업 경로를 사용합니다."));
			else
				strMessage.Format(_T("Environment variable SYAIAI_VI_SETUP_PATH is not set. Using backup path."));
			AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONWARNING);
			strSetupPathA = backupPath;
			strLog.Format(_T("Using backup path for AI Setup Wizard: %s"), strSetupPathA.c_str());
			THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		}
		else {
			return std::nullopt; // 즉시 함수 종료
		}
	}

	// 파일 확장자 확인
	std::filesystem::path setupPathFs(strSetupPathA);
	std::string strExt = setupPathFs.extension().string();
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);

	// 확장자가 .exe가 아닌 경우 즉시 오류 처리 및 반환
	if (strExt != ".exe")
	{
		CString strMessage;
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessage.Format(_T("AI Setup Wizard 경로가 실행 파일(.exe)이 아닙니다: %s"), strSetupPathA.c_str());
		else
			strMessage.Format(_T("AI Setup Wizard path is not an executable file (.exe): %s"), strSetupPathA.c_str());
		AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONWARNING);

		strLog.Format(_T("AI Setup Wizardpath is not an exe file, Path: %s"), strSetupPathA.c_str());
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		return std::nullopt; // 즉시 함수 종료
	}

	// --- 경로 준비 (std::filesystem 사용) ---
	char path_buf[MAX_PATH];
	GetModuleFileNameA(NULL, path_buf, MAX_PATH);
	std::filesystem::path executable_path(path_buf);
	std::filesystem::path folder_path = executable_path.parent_path() / "Data";
	std::filesystem::path config_json_path = folder_path / "ai_inspection_config.json";
	std::filesystem::path model_list_temp_path = folder_path / "ai_model_list.temp";

	// --- AI 관리 불량 모델 생성 
	bool consider_vision_pos = false;
	bool consider_inspection_type = true;
	bool consider_defect_type = true;

	std::vector<std::string> camera_types;
	std::vector<std::string> vision_positions;
	std::vector<std::string> inspection_types;
	std::vector<std::string> defect_types;

	camera_types.reserve(VISION_NUMBER_MAX);
	inspection_types.reserve(MAX_INSPECTION_TYPE);
	defect_types.reserve(MAX_DEFECT_NAME);

	for (int i = 0; i < VISION_NUMBER_MAX; i++) {
		camera_types.push_back((LPCSTR)m_ModelDefineInfo.m_strVisionName_Short[i]);
	}

	for (int i = 0; i < g_strInspectionTypeName_Short.size(); i++) {
		inspection_types.push_back(g_strInspectionTypeName_Short[i]);
	}

	for (int i = 0; i < MAX_DEFECT_NAME; i++) {
		defect_types.push_back((LPCSTR)m_strDefectName[i]);
	}

	syai_service::NameBuildOptions build_options;
	build_options.consider_vision_pos = false;
	build_options.consider_inspection_type = true;
	build_options.consider_defect_type = false;
	build_options.ignore_case = false;
	build_options.allow_input_duplicates = false;
	build_options.distinct = true;

	//camera_types.clear();
	//vision_positions.clear();
	//inspection_types.clear();
	// defect_types.clear();

	auto models = syai_service::ServiceDirector::build_names(camera_types,
															 vision_positions,
															 inspection_types,
															 defect_types,
															 build_options);

	bool process_file = (models.size() > 100) ? true : false;
	std::string stdParameters;

	if (process_file) {
		std::ofstream model_list_temp_file(model_list_temp_path);

		if (model_list_temp_file.is_open()) {	// 파일이 성공적으로 열렸는지 확인
			for (const auto& str : models) {
				model_list_temp_file << str << "\n";
			}
			model_list_temp_file.close();		// 파일 닫기
		}
		else {
			CString strMessage;
			strMessage.Format(_T("파일을 열 수 없습니다!"));
			AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONERROR);
		}

		// --- std::stringstream을 사용하여 명령줄 인자 구성 ---
		std::stringstream ss;
		ss << "--config \"" << config_json_path.string() << "\"";

		// defectNameList가 비어있지 않으면 추가
		if (!model_list_temp_path.empty())
		{
			ss << " --model-info-path " << model_list_temp_path;
		}
		if (!mode.empty()) {
			ss << " --start-mode" << " -" << mode << " -" << std::to_string(step) << " -" << model_name;
		}
		stdParameters = ss.str();
	}
	else {
		// --- 불량 정보 가져오기 ---
		std::stringstream defectNameList;
		bool firstDefect = true;  // 첫 번째 항목 확인용
		for (int i = 0; i < models.size(); i++)
		{
			std::string trimmedName = models[i];

			// 비어있지 않은 경우에만 추가
			if (!trimmedName.empty())
			{
				if (!firstDefect)
					defectNameList << " ";  // 첫 번째가 아니면 공백 추가

				defectNameList << "-" << trimmedName;
				firstDefect = false;
			}
		}
		// --- std::stringstream을 사용하여 명령줄 인자 구성 ---
		std::stringstream ss;
		ss << "--config \"" << config_json_path.string() << "\"";

		// defectNameList가 비어있지 않으면 추가
		std::string defectListStr = defectNameList.str();
		if (!defectListStr.empty())
		{
			ss << " --model-info-list " << defectListStr;
		}
		if (!mode.empty()) {
			ss << " --start-mode" << " -" << mode << " -" << std::to_string(step) << " -" << model_name;
		}
		stdParameters = ss.str();
	}
	// --- ShellExecuteExA로 변경해서 프로세스 핸들을 얻는다 ---
	SHELLEXECUTEINFOA sei;
	ZeroMemory(&sei, sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;   // 종료될 때까지 핸들 유지
	sei.hwnd = NULL;
	sei.lpVerb = "open";
	sei.lpFile = strSetupPathA.c_str();
	sei.lpParameters = stdParameters.c_str();
	sei.lpDirectory = NULL;
	sei.nShow = SW_SHOW;

	if (!ShellExecuteExA(&sei))
	{
		DWORD dwErr = GetLastError();

		CString strMessage;
		if (THEAPP.m_iPGMLanguageSelect == 0)
			strMessage.Format(_T("AI Setup Wizard 프로그램 실행 실패: %s\n에러 코드: %d"), strSetupPathA.c_str(), (int)dwErr);
		else
			strMessage.Format(_T("Failed to execute AI Setup Wizard program: %s\nError code: %d"), strSetupPathA.c_str(), (int)dwErr);
		AfxMessageBox(strMessage, MB_SYSTEMMODAL | MB_ICONERROR);

		strLog.Format(_T("AI Setup Wizard program execution failed (ShellExecuteExA), Path: %s, Error code: %d"),
					  strSetupPathA.c_str(), (int)dwErr);
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));
		return std::nullopt;
	}

	return sei;
}

void CuScanApp::SetVersion()
{
	char AppFilePath[_MAX_PATH * 2];
	// 실행파일의 풀패스 경로를 구한다.  
	strcpy(AppFilePath, AfxGetApp()->m_pszHelpFilePath);
	int slength = strlen(AppFilePath);
	strcpy(AppFilePath + (slength - 4), ".exe");

	//
	// 주어진 파일의 버전 정보를 읽는다. (이렇게 길어지리라 생각 못했다... )
	//
	DWORD Dummy = NULL;
	BOOL bResult = FALSE;
	HRESULT hResult = S_OK;
	DWORD dwLen = 0;
	DWORD dwError = 0;
	PVOID pVersionData = NULL;
	PVOID pVersionInfo = NULL;

	// 버전정보의 크기를 알아 내고 데이터 버퍼를 할당한다.
	// 파일의 버전정보의 크기가 파일마다 제각기 다르기 땜시...
	dwLen = ::GetFileVersionInfoSize(AppFilePath, &Dummy);
	if (dwLen == 0)
	{
		return;
	}
	// 버전 정보를 담을 버퍼를 할당한다. (수천 바이트가 필요하다)
	pVersionData = ::HeapAlloc(::GetProcessHeap(), 0, dwLen);
	ZeroMemory(pVersionData, dwLen);

	// 버전 정보 데이터를 읽는다.
	bResult = ::GetFileVersionInfo(AppFilePath, NULL, dwLen, pVersionData);
	if (bResult == FALSE)
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return;
	}

	// VS_FIXEDFILEINFO 로 부터 버전정보를 읽는다.
	// (숫자로 된 버전정보는 이 구조체 안에 정보가 담긴다)
	bResult = ::VerQueryValue(pVersionData, TEXT("\\"), &pVersionInfo, (PUINT)&dwLen);
	if (bResult == FALSE)
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return;
	}

	VS_FIXEDFILEINFO *pFileInfo = (VS_FIXEDFILEINFO *)pVersionInfo;
	// 숫자로된 버전 정보를 알아냈으니 필요한 대로 쓰문 되겠다.
	DWORD dwMajorVer = HIWORD(pFileInfo->dwFileVersionMS);
	DWORD dwMinorVer = LOWORD(pFileInfo->dwFileVersionMS);
	DWORD dwReleaseVer = HIWORD(pFileInfo->dwFileVersionLS);
	DWORD dwBuildVer = LOWORD(pFileInfo->dwFileVersionLS);

	CString strVer;
	strVer.Format("%d%d%d%d", dwMajorVer, dwMinorVer, dwReleaseVer, dwBuildVer);

	// SW Version Copy
	m_strVerInfo = strVer;

	m_iAppVersion = atoi((LPCTSTR)strVer);

	::HeapFree(::GetProcessHeap(), 0, pVersionData);
}

void CuScanApp::InitProgress(int range, CString sText)
{

}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	BOOL GetAppVersion(void);
	void GetAppModify(void);

	CString GetFileDateStr(CString FilePath);
	void	ShowReleaseInformation();

	CBrush m_EditBGBrush;


	// Dialog Data
		//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CEdit	m_EditRelease;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//	virtual BOOL DestroyWindow();
	//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	//	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_EDIT_RELEASE, m_EditRelease);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
//	ON_WM_LBUTTONDOWN()
//ON_WM_LBUTTONDOWN()
//ON_WM_WINDOWPOSCHANGING()
//ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

// App command to run the dialog
void CuScanApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CuScanApp::OnHelp()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CuScanApp::SetViewStatusText(int idx, CString szMsg)
{
	if (m_bInitComplete) {
		CuScanView* pView = (CuScanView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();
		pView->SetStatusText(idx, szMsg);
	}
}

void CuScanApp::UpdateCurMode()
{
	if (m_bInitComplete) {
		CuScanView* pView = (CuScanView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();
		pView->UpdateCurMode();
	}
}

void CuScanApp::ShowVersionText()
{
	if (m_bInitComplete) {
		CuScanView* pView = (CuScanView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();
		pView->ShowVersionText();
	}
}

void CuScanApp::UpdateVersionText()
{
	if (m_bInitComplete) {
		CuScanView* pView = (CuScanView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();
		pView->UpdateVersionText();
	}
}

void CuScanApp::SetExitProgram()
{
	if (m_bInitComplete) {
		CuScanView* pView = (CuScanView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();
		pView->SetExitProgram();
	}
}

void CuScanApp::SetInspectionReady()
{
	CuScanView* pView = (CuScanView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();
	pView->TeachingShow();
}

void CuScanApp::DoEvents()
{
	MSG msg;
	if (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

void CuScanApp::DoEvents(DWORD dwSleep)
{
	MSG msg;
	DWORD dwStart = GetTickCount();

	do {

		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		Sleep(1);

	} while (dwSleep > (GetTickCount() - dwStart));
}

BOOL CuScanApp::IsVisionSWRun()
{
	return m_bVisionSWRun;
}

#include <tlhelp32.h>

int CuScanApp::CheckProcessExist()
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	int iFound = 0;

	CString sFileName = GetProgramFileName();

	if (Process32First(hSnap, &pe)) {
		do {
			wsprintf(pe.szExeFile, "%s", pe.szExeFile);
			if (stricmp(pe.szExeFile, (LPSTR)(LPCTSTR)sFileName) == 0) {
				iFound++;
			}
		} while (Process32Next(hSnap, &pe));
	}

	CloseHandle(hSnap);

	if (iFound != 0)
		ExitProcess(sFileName, GetCurrentProcessId());

	return iFound;
}

void CuScanApp::ExitProcess(CString strTargetProcName, DWORD dwExceptProcId)
{
	DWORD aProcesses[1024] = { 0, }, cbNeeded, cProcesses;
	unsigned int i = 0;

	//실행중인 프로세스를 모두 구한다
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return;

	//실행중인 프로세스의 개수를 구한다
	cProcesses = cbNeeded / sizeof(DWORD);
	//각 프로세스에 대한 이름 및 프로세스 아이디를 구하고 타겟 프로세스를 강제로 종료시킨다
	for (i = 0; i < cProcesses; i++)
	{
		TCHAR szProcessName[MAX_PATH] = _T("unknown");
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

		if (NULL != hProcess)
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
				GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName)); //프로세스 이름을 구한다(szProcessName)
		}

		BOOL bRet;
		CString strComp = szProcessName;
		//실행중인 프로세스(strComp)와 죽이려는 프로세스(strTargetProcName) 이름을 비교하여 같으면 해당 프로세스 종료
		//단 실행중인 나 자신은 죽이려는 프로세스에서 제외시킨다
		if ((strComp.MakeLower() == strTargetProcName.MakeLower()) && (aProcesses[i] != dwExceptProcId))
		{
			HANDLE hKillProc = OpenProcess(PROCESS_TERMINATE, FALSE, aProcesses[i]);

			if (hKillProc != NULL)
			{
				DWORD ExitCode = 0;

				GetExitCodeProcess(hKillProc, &ExitCode);
				bRet = TerminateProcess(hKillProc, ExitCode);
				if (bRet)
					WaitForSingleObject(hKillProc, INFINITE);

				CloseHandle(hKillProc);
			}
		}
		CloseHandle(hProcess);
	}
}

CString	CuScanApp::GetProgramFileName()
{
	char szBuffer[MAX_PATH * 2];
	int nPathLength = 0, nFileNameLength = 0;
	char *FirstIdx, *SecIdx;
	CString sFileName;

	nPathLength = GetModuleFileName(NULL, szBuffer, MAX_PATH);
	FirstIdx = strrchr(szBuffer, '\\');
	SecIdx = strrchr(szBuffer, '\0');
	nFileNameLength = SecIdx - FirstIdx;

	sFileName.Format("%s", FirstIdx + 1);

	return sFileName;
}

CCalDataService* CuScanApp::GetCalDataService(int nCamNumber)
{
	if (nCamNumber >= VISION_NUMBER_1 && nCamNumber < VISION_NUMBER_MAX)
		return m_pCalDataService_N[nCamNumber];
	else
		return NULL;
}

BOOL CuScanApp::ConnectStatusADJ(int iMzNo)
{
	CString strLog;
	BOOL bConnectFail = FALSE;
	for (int iClientNO = 0; iClientNO < MAX_ADJ_CONNECT_NUM; iClientNO++)	//각각 통신 연결.
	{
		if (Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
		{
			for (int j = 0; j < Struct_PreferenceStruct.m_nADJ_MultiNetworkNO; j++)
			{
				if (!m_TCPClientService.m_bConnect[iClientNO][j])
				{
					if (m_TCPClientService.m_arrClient[iClientNO][j].TCPConnect())
					{
						m_TCPClientService.m_bConnect[iClientNO][j] = TRUE;
					}
				}
			}
			CString m_strFailedConnect = "", strLog;
			for (int j = Struct_PreferenceStruct.m_nADJ_MultiNetworkNO - 1; j >= 0; j--)
			{
				if (!m_TCPClientService.m_bConnect[iClientNO][j])
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
					strLog.Format("Network Status: ADJ1 Connect Succeses");
					m_log_adj->info("{}", LOG_CSTR(strLog));
				}
				else
				{
					strLog.Format("Network Status: ADJ2 Connect Succeses");
					m_log_adj->info("{}", LOG_CSTR(strLog));
				}
			}
			else
			{
				if (iClientNO == 0)
				{
					if (Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
					{
						strLog.Format("Network Status: ADJ1 %s Connect Fail", m_strFailedConnect);
						m_log_adj->warn("{}", LOG_CSTR(strLog));
					}
					else
					{
						strLog.Format("Network Status: ADJ1 Not Use");
						m_log_adj->info("{}", LOG_CSTR(strLog));
					}
				}
				else
				{
					if (Struct_PreferenceStruct.m_bIsUseADJ[iClientNO])
					{
						strLog.Format("Network Status: ADJ2 #s Connect Fail", m_strFailedConnect);
						m_log_adj->warn("{}", LOG_CSTR(strLog));
						bConnectFail = TRUE;
					}
					else
					{
						strLog.Format("Network Status: ADJ2 Not Use");
						m_log_adj->info("{}", LOG_CSTR(strLog));
					}
				}
			}
		}
	}

	if (bConnectFail == TRUE)
		return FALSE;
	else
		return TRUE;
}


BOOL CuScanApp::ConnectStatusLAS(int iMzNo)
{
	CString strLog;
	BOOL bConnectFail = FALSE;
	// LAS 1, 2, 3 연결 상태 확인
	if (Struct_PreferenceStruct.m_bUseLAS1)
	{
		if (_access(Struct_PreferenceStruct.m_strSaveFolderPathLAS1, 0) != 0)
		{
			m_pInspectResultDlg->m_ConnectLas1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
			m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS1, "연결 실패");
			strLog.Format("Network Status: LAS1 Connect Fail");
			m_log_system->warn("{}", LOG_CSTR(strLog));
			bConnectFail = TRUE;
		}
		else
		{
			m_pInspectResultDlg->m_ConnectLas1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
			m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS1, "연결됨 : " + Struct_PreferenceStruct.m_strSaveFolderPathLAS1.Mid(0, Struct_PreferenceStruct.m_strSaveFolderPathLAS1.ReverseFind(':')));
			strLog.Format("Network Status: LAS1 Connect Succeses");
			m_log_system->info("{}", LOG_CSTR(strLog));
		}
	}
	else
	{
		m_pInspectResultDlg->m_ConnectLas1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS1, "사용 안함");
		strLog.Format("Network Status: LAS1 Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	if (Struct_PreferenceStruct.m_bUseLAS2)
	{
		if (_access(Struct_PreferenceStruct.m_strSaveFolderPathLAS2, 0) != 0)
		{
			m_pInspectResultDlg->m_ConnectLas2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
			m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS2, "연결 실패");
			strLog.Format("Network Status:LAS2 Connect Fail");
			m_log_system->warn("{}", LOG_CSTR(strLog));
			bConnectFail = TRUE;
		}
		else
		{
			m_pInspectResultDlg->m_ConnectLas2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
			m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS2, "연결됨 : " + Struct_PreferenceStruct.m_strSaveFolderPathLAS2.Mid(0, Struct_PreferenceStruct.m_strSaveFolderPathLAS2.ReverseFind(':')));
			strLog.Format("Network Status: LAS2 Connect Succeses");
			m_log_system->info("{}", LOG_CSTR(strLog));
		}
	}
	else
	{
		m_pInspectResultDlg->m_ConnectLas2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS2, "사용 안함");
		strLog.Format("Network Status: LAS2 Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	if (Struct_PreferenceStruct.m_bUseLAS3)
	{
		if (_access(Struct_PreferenceStruct.m_strSaveFolderPathLAS3, 0) != 0)
		{
			m_pInspectResultDlg->m_ConnectLas3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
			m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS3, "연결 실패");
			strLog.Format("Network Status: LAS3 Connect Fail");
			m_log_system->warn("{}", LOG_CSTR(strLog));
			bConnectFail = TRUE;
		}
		else
		{
			m_pInspectResultDlg->m_ConnectLas3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
			m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS3, "연결됨 : " + Struct_PreferenceStruct.m_strSaveFolderPathLAS3.Mid(0, Struct_PreferenceStruct.m_strSaveFolderPathLAS3.ReverseFind(':')));
			strLog.Format("Network Status: LAS3 Connect Succeses");
			m_log_system->info("{}", LOG_CSTR(strLog));
		}
	}
	else
	{
		m_pInspectResultDlg->m_ConnectLas3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS3, "사용 안함");
		strLog.Format("Network Status: LAS3 Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	// Raw Image Path 확인
	if (Struct_PreferenceStruct.m_bRawImageSaveLAS)
	{
		if (m_enumCurSaveLASStatus == eLASINIT || m_enumCurSaveLASStatus == eSAVELAS1)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS1 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_RawImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Raw Image Save Path(LAS1) Connect Succeses");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_RawImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Raw Image Save Path(LAS1) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}
		else if (m_enumCurSaveLASStatus == eSAVELAS2)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS2 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_RawImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Raw Image Save Path(LAS2) Connect Succeses");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_RawImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Raw Image Save Path(LAS2) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}
		else if (m_enumCurSaveLASStatus == eSAVELAS3)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS3 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_RawImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Raw Image Save Path(LAS3) Connect Succeses");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_RawImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Raw Image Save Path(LAS3) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}

		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RAW_IMAGE_SAVE_PATH_LAS, m_strChangeLASFolderPath[iMzNo].Mid(0, m_strChangeLASFolderPath[iMzNo].ReverseFind(':')));
	}
	else
	{
		m_pInspectResultDlg->m_RawImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RAW_IMAGE_SAVE_PATH_LAS, "사용 안함");
		strLog.Format("Network Status: Raw Image Save Path(LAS) Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
	{
		if (_access(Struct_PreferenceStruct.m_strRawFolderPath, 0) != 0)
		{
			m_pInspectResultDlg->m_RawImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
			strLog.Format("Network Status: Raw Image Save Path(Local) Connect Fail");
			m_log_system->warn("{}", LOG_CSTR(strLog));
		}
		else
		{
			m_pInspectResultDlg->m_RawImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
			strLog.Format("Network Status: Raw Image Save Path(Local) Connect Succeses");
			m_log_system->info("{}", LOG_CSTR(strLog));
		}

		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RAW_IMAGE_SAVE_PATH_LOCAL, Struct_PreferenceStruct.m_strRawFolderPath.Mid(0, Struct_PreferenceStruct.m_strRawFolderPath.ReverseFind(':')));
	}
	else
	{
		m_pInspectResultDlg->m_RawImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RAW_IMAGE_SAVE_PATH_LOCAL, "사용 안함");
		strLog.Format("Network Status: Raw Image Save Path(Local) Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	// Result Image Path 확인
	if (Struct_PreferenceStruct.m_bResultImageSaveLAS)
	{
		if (m_enumCurSaveLASStatus == eLASINIT || m_enumCurSaveLASStatus == eSAVELAS1)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS1 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ResultImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Result Image Save Path(LAS1) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ResultImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Result Image Save Path(LAS1) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}
		else if (m_enumCurSaveLASStatus == eSAVELAS2)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS2 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ResultImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Result Image Save Path(LAS2) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ResultImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Result Image Save Path(LAS2) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}
		else if (m_enumCurSaveLASStatus == eSAVELAS3)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS3 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ResultImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Result Image Save Path(LAS3) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ResultImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Result Image Save Path(LAS3) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}

		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RESULT_IMAGE_SAVE_PATH_LAS, m_strChangeLASFolderPath[iMzNo].Mid(0, m_strChangeLASFolderPath[iMzNo].ReverseFind(':')));
	}
	else
	{
		m_pInspectResultDlg->m_ResultImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RESULT_IMAGE_SAVE_PATH_LAS, "사용 안함");
		strLog.Format("Network Status: Result Image Save Path(LAS) Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	if (Struct_PreferenceStruct.m_bResultImageSaveLocal)
	{
		if (_access(Struct_PreferenceStruct.m_strResultFolderPath, 0) != 0)
		{
			m_pInspectResultDlg->m_ResultImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
			strLog.Format("Network Status: Result Image Save Path(Local) Connect Fail");
			m_log_system->warn("{}", LOG_CSTR(strLog));
		}
		else
		{
			m_pInspectResultDlg->m_ResultImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
			strLog.Format("Network Status: Result Image Save Path(Local) Connect Succeses");
			m_log_system->info("{}", LOG_CSTR(strLog));
		}

		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RESULT_IMAGE_SAVE_PATH_LOCAL, Struct_PreferenceStruct.m_strResultFolderPath.Mid(0, Struct_PreferenceStruct.m_strResultFolderPath.ReverseFind(':')));
	}
	else
	{
		m_pInspectResultDlg->m_ResultImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RESULT_IMAGE_SAVE_PATH_LOCAL, "사용 안함");
		strLog.Format("Network Status: Result Image Save Path(Local) Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	// Review Image Path 확인
	if (Struct_PreferenceStruct.m_bReviewImageSaveLAS)
	{
		if (m_enumCurSaveLASStatus == eLASINIT || m_enumCurSaveLASStatus == eSAVELAS1)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS1 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ReviewImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Review Image Save Path(LAS1) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ReviewImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Review Image Save Path(LAS1) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}
		else if (m_enumCurSaveLASStatus == eSAVELAS2)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS2 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ReviewImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Review Image Save Path(LAS2) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ReviewImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Review Image Save Path(LAS2) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}
		else if (m_enumCurSaveLASStatus == eSAVELAS3)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS3 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ReviewImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Review Image Save Path(LAS3) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ReviewImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Review Image Save Path(LAS3) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}

		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_REVIEW_IMAGE_SAVE_PATH_LAS, m_strChangeLASFolderPath[iMzNo].Mid(0, m_strChangeLASFolderPath[iMzNo].ReverseFind(':')));
	}
	else
	{
		m_pInspectResultDlg->m_ReviewImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_REVIEW_IMAGE_SAVE_PATH_LAS, "사용 안함");
		strLog.Format("Network Status: Review Image Save Path(LAS) Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	if (Struct_PreferenceStruct.m_bReviewImageSaveLocal)
	{
		if (_access(Struct_PreferenceStruct.m_strReviewFolderPath, 0) != 0)
		{
			m_pInspectResultDlg->m_ReviewImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
			strLog.Format("Network Status: Review Image Save Path(Local) Connect Fail");
			m_log_system->warn("{}", LOG_CSTR(strLog));
		}
		else
		{
			m_pInspectResultDlg->m_ReviewImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
			strLog.Format("Network Status: Review Image Save Path(Local) Connect Success");
			m_log_system->info("{}", LOG_CSTR(strLog));
		}

		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_REVIEW_IMAGE_SAVE_PATH_LOCAL, Struct_PreferenceStruct.m_strReviewFolderPath.Mid(0, Struct_PreferenceStruct.m_strReviewFolderPath.ReverseFind(':')));
	}
	else
	{
		m_pInspectResultDlg->m_ReviewImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_REVIEW_IMAGE_SAVE_PATH_LOCAL, "사용 안함");
		strLog.Format("Network Status: Review Image Save Path(Local) Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	// ADJ Image Path 확인
	if (Struct_PreferenceStruct.m_bADJImageSaveLAS)
	{
		if (m_enumCurSaveLASStatus == eLASINIT || m_enumCurSaveLASStatus == eSAVELAS1)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS1 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ADJImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: ADJ Image Save Path(LAS1) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ADJImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: ADJ Image Save Path(LAS1) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}
		else if (m_enumCurSaveLASStatus == eSAVELAS2)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS2 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ADJImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: ADJ Image Save Path(LAS2) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ADJImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: ADJ Image Save Path(LAS2) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}
		else if (m_enumCurSaveLASStatus == eSAVELAS3)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS3 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ADJImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: ADJ Image Save Path(LAS3) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ADJImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: ADJ Image Save Path(LAS3) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}

		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_ADJ_IMAGE_SAVE_PATH_LAS, m_strChangeLASFolderPath[iMzNo].Mid(0, m_strChangeLASFolderPath[iMzNo].ReverseFind(':')));
	}
	else
	{
		m_pInspectResultDlg->m_ADJImageSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_ADJ_IMAGE_SAVE_PATH_LAS, "사용 안함");
		strLog.Format("Network Status: ADJ Image Save Path(LAS) Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	if (Struct_PreferenceStruct.m_bADJImageSaveLocal)
	{
		if (_access(Struct_PreferenceStruct.m_strAdjFolderPath, 0) != 0)
		{
			m_pInspectResultDlg->m_ADJImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
			strLog.Format("Network Status: ADJ Image Save Path(Local) Connect Fail");
			m_log_system->warn("{}", LOG_CSTR(strLog));
		}
		else
		{
			m_pInspectResultDlg->m_ADJImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
			strLog.Format("Network Status: ADJ Image Save Path(Local) Connect Success");
			m_log_system->info("{}", LOG_CSTR(strLog));
		}

		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_ADJ_IMAGE_SAVE_PATH_LOCAL, Struct_PreferenceStruct.m_strAdjFolderPath.Mid(0, Struct_PreferenceStruct.m_strAdjFolderPath.ReverseFind(':')));
	}
	else
	{
		m_pInspectResultDlg->m_ADJImageSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_ADJ_IMAGE_SAVE_PATH_LOCAL, "사용 안함");
		strLog.Format("Network Status: ADJ Image Save Path(Local) Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	// Result Log Path 확인
	if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
	{
		if (m_enumCurSaveLASStatus == eLASINIT || m_enumCurSaveLASStatus == eSAVELAS1)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS1 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ResultLogSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Result Log Save Path(LAS1) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ResultLogSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Result Log Save Path(LAS1) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}
		else if (m_enumCurSaveLASStatus == eSAVELAS2)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS2 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ResultLogSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Result Log Save Path(LAS2) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ResultLogSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Result Log Save Path(LAS2) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}
		else if (m_enumCurSaveLASStatus == eSAVELAS3)
		{
			if (Struct_PreferenceStruct.m_strSaveFolderPathLAS3 == m_strChangeLASFolderPath[iMzNo])
			{
				m_pInspectResultDlg->m_ResultLogSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
				strLog.Format("Network Status: Result Log Save Path(LAS3) Connect Success");
				m_log_system->info("{}", LOG_CSTR(strLog));
			}
			else
			{
				m_pInspectResultDlg->m_ResultLogSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
				strLog.Format("Network Status: Result Log Save Path(LAS3) Connect Fail");
				m_log_system->warn("{}", LOG_CSTR(strLog));
				bConnectFail = TRUE;
			}
		}

		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RESULT_LOG_SAVE_PATH_LAS, m_strChangeLASFolderPath[iMzNo].Mid(0, m_strChangeLASFolderPath[iMzNo].ReverseFind(':')));
	}
	else
	{
		m_pInspectResultDlg->m_ResultLogSavePathLas.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RESULT_LOG_SAVE_PATH_LAS, "사용 안함");
		strLog.Format("Network Status: Result Log Save Path(LAS) Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
	{
		if (_access(Struct_PreferenceStruct.m_strResultLogFolderPath, 0) != 0)
		{
			m_pInspectResultDlg->m_ResultLogSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 1), RGB(255, 1, 1));
			strLog.Format("Network Status: Result Log Save Path(Local) Connect Fail");
			m_log_system->warn("{}", LOG_CSTR(strLog));
		}
		else
		{
			m_pInspectResultDlg->m_ResultLogSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(1, 1, 1), RGB(1, 255, 1));
			strLog.Format("Network Status: Result Log Save Path(Local) Connect Success");
			m_log_system->info("{}", LOG_CSTR(strLog));
		}

		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RESULT_LOG_SAVE_PATH_LOCAL, Struct_PreferenceStruct.m_strResultLogFolderPath.Mid(0, Struct_PreferenceStruct.m_strResultLogFolderPath.ReverseFind(':')));
	}
	else
	{
		m_pInspectResultDlg->m_ResultLogSavePathLocal.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 255), RGB(1, 1, 1));
		m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_RESULT_LOG_SAVE_PATH_LOCAL, "사용 안함");
		strLog.Format("Network Status: Result Log Save Path(Local) Not Use");
		m_log_system->info("{}", LOG_CSTR(strLog));
	}

	if (bConnectFail == TRUE)
		return FALSE;
	else
		return TRUE;
}


BOOL CuScanApp::ChangeLAS(int iMzNo)
{
	CString strStatus = "";
	int nLAS1End = Struct_PreferenceStruct.m_strSaveFolderPathLAS1.GetLength();
	if (Struct_PreferenceStruct.m_strSaveFolderPathLAS1.GetAt(nLAS1End - 1) != '\\')
		Struct_PreferenceStruct.m_strSaveFolderPathLAS1 += '\\';

	int nLAS2End = Struct_PreferenceStruct.m_strSaveFolderPathLAS2.GetLength();
	if (Struct_PreferenceStruct.m_strSaveFolderPathLAS2.GetAt(nLAS2End - 1) != '\\')
		Struct_PreferenceStruct.m_strSaveFolderPathLAS2 += '\\';

	int nLAS3End = Struct_PreferenceStruct.m_strSaveFolderPathLAS3.GetLength();
	if (Struct_PreferenceStruct.m_strSaveFolderPathLAS3.GetAt(nLAS3End - 1) != '\\')
		Struct_PreferenceStruct.m_strSaveFolderPathLAS3 += '\\';

	CIniFileCS INI_LAS1(Struct_PreferenceStruct.m_strSaveFolderPathLAS1 + "HDD_Status.ini");
	CIniFileCS INI_LAS2(Struct_PreferenceStruct.m_strSaveFolderPathLAS2 + "HDD_Status.ini");
	CIniFileCS INI_LAS3(Struct_PreferenceStruct.m_strSaveFolderPathLAS3 + "HDD_Status.ini");

	if (m_enumCurSaveLASStatus == eLASINIT || m_enumCurSaveLASStatus == eSAVELAS1) //Status 초기 이거나 LAS 1에 저장하고 있을 경우
	{
		m_enumCurSaveLASStatus = eSAVELAS1;

		strStatus = INI_LAS1.Get_String("HDD", "STATUS", "FULL");
		if (strStatus == "READY" && (Struct_PreferenceStruct.m_bUseLAS1) == TRUE)
		{
			m_enumCurSaveLASStatus = eSAVELAS1;
			m_strChangeLASFolderPath[iMzNo] = Struct_PreferenceStruct.m_strSaveFolderPathLAS1;
			return TRUE;
		}

		strStatus = INI_LAS2.Get_String("HDD", "STATUS", "FULL");
		if (strStatus == "READY" && (Struct_PreferenceStruct.m_bUseLAS2) == TRUE)
		{
			m_enumCurSaveLASStatus = eSAVELAS2;
			m_strChangeLASFolderPath[iMzNo] = Struct_PreferenceStruct.m_strSaveFolderPathLAS2;
			return TRUE;
		}

		strStatus = INI_LAS3.Get_String("HDD", "STATUS", "FULL");
		if (strStatus == "READY" && (Struct_PreferenceStruct.m_bUseLAS3) == TRUE)
		{
			m_enumCurSaveLASStatus = eSAVELAS3;
			m_strChangeLASFolderPath[iMzNo] = Struct_PreferenceStruct.m_strSaveFolderPathLAS3;
			return TRUE;
		}

		return FALSE;
	}
	else if (m_enumCurSaveLASStatus == eSAVELAS2) //LAS 2에 저장하고 있을 경우
	{
		m_enumCurSaveLASStatus = eSAVELAS2;

		strStatus = INI_LAS2.Get_String("HDD", "STATUS", "FULL");
		if (strStatus == "READY" && (Struct_PreferenceStruct.m_bUseLAS2) == TRUE)
		{
			m_enumCurSaveLASStatus = eSAVELAS2;
			m_strChangeLASFolderPath[iMzNo] = Struct_PreferenceStruct.m_strSaveFolderPathLAS2;
			return TRUE;
		}

		strStatus = INI_LAS3.Get_String("HDD", "STATUS", "FULL");
		if (strStatus == "READY" && (Struct_PreferenceStruct.m_bUseLAS3) == TRUE)
		{
			m_enumCurSaveLASStatus = eSAVELAS3;
			m_strChangeLASFolderPath[iMzNo] = Struct_PreferenceStruct.m_strSaveFolderPathLAS3;
			return TRUE;
		}

		strStatus = INI_LAS1.Get_String("HDD", "STATUS", "FULL");
		if (strStatus == "READY" && (Struct_PreferenceStruct.m_bUseLAS1) == TRUE)
		{
			m_enumCurSaveLASStatus = eSAVELAS1;
			m_strChangeLASFolderPath[iMzNo] = Struct_PreferenceStruct.m_strSaveFolderPathLAS1;
			return TRUE;
		}

		return FALSE;
	}
	else //LAS 3에 저장하고 있을 경우
	{
		m_enumCurSaveLASStatus = eSAVELAS3;

		strStatus = INI_LAS3.Get_String("HDD", "STATUS", "FULL");
		if (strStatus == "READY" && (Struct_PreferenceStruct.m_bUseLAS3) == TRUE)
		{
			m_enumCurSaveLASStatus = eSAVELAS3;
			m_strChangeLASFolderPath[iMzNo] = Struct_PreferenceStruct.m_strSaveFolderPathLAS3;
			return TRUE;
		}

		strStatus = INI_LAS1.Get_String("HDD", "STATUS", "FULL");
		if (strStatus == "READY" && (Struct_PreferenceStruct.m_bUseLAS1) == TRUE)
		{
			m_enumCurSaveLASStatus = eSAVELAS1;
			m_strChangeLASFolderPath[iMzNo] = Struct_PreferenceStruct.m_strSaveFolderPathLAS1;
			return TRUE;
		}

		strStatus = INI_LAS2.Get_String("HDD", "STATUS", "FULL");
		if (strStatus == "READY" && (Struct_PreferenceStruct.m_bUseLAS2) == TRUE)
		{
			m_enumCurSaveLASStatus = eSAVELAS2;
			m_strChangeLASFolderPath[iMzNo] = Struct_PreferenceStruct.m_strSaveFolderPathLAS2;
			return TRUE;
		}

		return FALSE;
	}

	return FALSE;
}

void CuScanApp::SaveTeachingParameter(CString sSaveRootFolderName)
{
	SYSTEMTIME time;

	GetLocalTime(&time);

	CString sTimeStamp;
	sTimeStamp.Format("%04d%02d%02d_%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	CString sSaveFileName;

	int iParamVersion;
	iParamVersion = GetSwParamVersion();

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		CString strVisionCamType = m_ModelDefineInfo.m_strVisionName[i];
		sSaveFileName.Format("%s_%s_ParamVer%d_%s.txt", m_pDualModelDataManager[TEACHING_MZ_NO - 1][i]->m_sModelName, sTimeStamp, iParamVersion, strVisionCamType);
		m_pDualModelDataManager[TEACHING_MZ_NO - 1][i]->SaveTeachingParam(i, sSaveRootFolderName, sSaveFileName);
	}
}

void CuScanApp::SaveAiRetrainRoi(int iPcVisionNo)
{
	SYSTEMTIME time;

	GetLocalTime(&time);

	CString sTimeStamp;
	sTimeStamp.Format("%04d%02d%02d", time.wYear, time.wMonth, time.wDay);

	CString sSaveFileName;

	CString strVisionCamType = m_ModelDefineInfo.m_strVisionName[iPcVisionNo];
	sSaveFileName.Format("AiRetrainRoi_%s_%s_%s.txt", m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_sModelName, strVisionCamType, sTimeStamp);
	m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->SaveAiRetrainRoi(sSaveFileName);
}

#ifdef USE_SUAKIT

CString CuScanApp::GetADJPriorityDefectName(int nPriority[VISION_NUMBER_MAX], int nDeepLearningModel[VISION_NUMBER_MAX])
{
	int nSelectPriority = DEEP_MODEL_NUM * MAX_ADJ_CONNECT_NUM;
	int nSelectModelID = 0;
	CString strResultName = "";
	CString strDefectName = "";
	CString strDefectPos = "";
	CString sVisionCamType_Short = "";

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		if (nPriority[i] <= nSelectPriority)
		{
			nSelectPriority = nPriority[i];
			nSelectModelID = nDeepLearningModel[i];
		}
	}

	if (nSelectModelID <= 0)
		return "NG";


	strDefectName = Struct_PreferenceStruct.m_strADJ_Defect_Name[(int)((nSelectModelID - 1) / DEEP_MODEL_NUM)][(nSelectModelID - 1) % DEEP_MODEL_NUM];
	strDefectPos = Struct_PreferenceStruct.m_strADJ_Defect_Pos[(int)((nSelectModelID - 1) / DEEP_MODEL_NUM)][(nSelectModelID - 1) % DEEP_MODEL_NUM];
	int nCam = Struct_PreferenceStruct.m_nADJThreadNO[(int)((nSelectModelID - 1) / DEEP_MODEL_NUM)][(nSelectModelID - 1) % DEEP_MODEL_NUM];
	sVisionCamType_Short = m_ModelDefineInfo.m_strVisionName_Short[nCam];

	return strDefectPos + '_' + sVisionCamType_Short + '_' + strDefectName;
}

void CuScanApp::SaveADJLotResultLog(CString sPath, CString strResult, CString strAdditionalTitle, BOOL bUseAdditionalLog, int iPCType, int iPcVisionNo, int iMzNo)
{
	EnterCriticalSection(&CS_ADJLOTRESULTLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);

	strFile.Format("%s\\ADJLotResult_PC%d.txt", sPath, Struct_PreferenceStruct.m_iPCType + 1);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_ADJLOTRESULTLOG);
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTray\tModuleNo\tBarcode\tTop(Var)\tBottom(Var)\tTop(AI)\tBottom(AI)\tVariationBlob(Top)\tAIJudgeCount(Top)\tAIGoodCount(Top)\tVariationBlob(Bottom)\tAIJudgeCount(Bottom)\tAIGoodCount(Bottom)\r\n");

			file.Write(strSave, strSave.GetLength());
		}

		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();

	}
	catch (CFileException *pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_ADJLOTRESULTLOG);
}

void CuScanApp::SaveOnlyADJLotResultLog(CString sPath, CString strResult, CString strAdditionalTitle, BOOL bUseAdditionalLog, int iPCType, int iPcVisionNo, int iMzNo)
{
	//return;
	EnterCriticalSection(&CS_ADJONLYRESULTLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);
	strFile.Format(sPath + "\\OnlyADJResult_PC%d.txt", iPCType + 1);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_ADJONLYRESULTLOG);
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTray\tModuleNo\tBarcode\tModel ID\t전송파일이름\tADJ Unit 결과\t확률값\r\n");

			file.Write(strSave, strSave.GetLength());
		}

		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();

	}
	catch (CFileException *pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_ADJONLYRESULTLOG);
}

void CuScanApp::SaveOnlyADJDayLotResultLog(CString sPath, CString strResult, int iPCType, int iMzNo)
{
	//return;

	EnterCriticalSection(&CS_ADJONLYRESULTLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);
	strFile.Format(sPath + "\\DayOnlyADJResult_PC%d.txt", iPCType + 1);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_ADJONLYRESULTLOG);
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTray\tModuleNo\tBarcode\tModel ID\t전송파일이름\tADJ Unit 결과\t확률값\r\n");

			file.Write(strSave, strSave.GetLength());
		}

		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();

	}
	catch (CFileException *pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_ADJONLYRESULTLOG);
}

void CuScanApp::SaveADJDayLotResultLog(CString sPath, CString strResult, int iPCType, int iMzNo)
{
	EnterCriticalSection(&CS_ADJLOTRESULTLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);
	CString sDayLotResultFileName;

	sDayLotResultFileName.Format("\\DayADJLotResult_PC%d.txt", iPCType + 1);

	strFile.Format(sPath + sDayLotResultFileName);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_ADJLOTRESULTLOG);
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			// TODO : 하드코딩 변경 필요
			strSave.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTray\tModuleNo\tBarcode\tTop(Var)\tBottom(Var)\tTop(AI)\tBottom(AI)\tVariationBlob(Top)\tAIJudgeCount(Top)\tAIGoodCount(Top)\tVariationBlob(Bottom)\tAIJudgeCount(Bottom)\tAIGoodCount(Bottom)\r\n");

			file.Write(strSave, strSave.GetLength());
		}

		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();

	}
	catch (CFileException *pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_ADJLOTRESULTLOG);
}

void CuScanApp::SaveADJDaySummarySkipRateINI(CString sPath, CString sSectionModel, BOOL bResultNG, int iMzNo)
{
	EnterCriticalSection(&CS_ADJDAYSUMMARYLOG);

	// ADJDaySummary.txt
	CString sDaySummaryFileName;
	CString sAdjFileName;
	sAdjFileName.Format("\\ADJDaySummary%d.txt", Struct_PreferenceStruct.m_iPCType + 1);
	m_FileBase.CreatePath(sPath);
	sDaySummaryFileName = sPath + sAdjFileName;
	CIniFileCS INI_DaySummary(sDaySummaryFileName);
	int iCurValue, iTotal, iTotalSkip;
	double dFilter;
	CString strFilter;
	//

	iTotal = INI_DaySummary.Get_Integer(sSectionModel, "딥러닝요청수량", 0);
	iTotalSkip = INI_DaySummary.Get_Integer(sSectionModel, "딥러닝스킵수량", 0);

	iTotal++;

	if (bResultNG)
		iTotalSkip++;

	if ((iTotal) == 0)
		dFilter = 0;
	else
		dFilter = ((double)iTotalSkip / (double)(iTotal))*100.0;

	strFilter.Format("%.2lf", dFilter);

	INI_DaySummary.Set_Integer(sSectionModel, "딥러닝요청수량", iTotal);
	INI_DaySummary.Set_Integer(sSectionModel, "딥러닝스킵수량", iTotalSkip);
	INI_DaySummary.Set_String(sSectionModel, "딥러닝스킵률(%)", strFilter);

	LeaveCriticalSection(&CS_ADJDAYSUMMARYLOG);
}

void CuScanApp::SaveADJDaySummaryINI(CString sPath, CString sSectionModel, BOOL bResultNG, int iMzNo)
{
	EnterCriticalSection(&CS_ADJDAYSUMMARYLOG);

	// ADJDaySummary.txt
	CString sDaySummaryFileName;
	CString sAdjFileName;
	sAdjFileName.Format("\\ADJDaySummary_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);
	m_FileBase.CreatePath(sPath);
	sDaySummaryFileName = sPath + sAdjFileName;
	CIniFileCS INI_DaySummary(sDaySummaryFileName);
	int iCurValue, iTotalOK, iTotalNG;
	double dFilter;
	CString strFilter;
	//

	iTotalOK = INI_DaySummary.Get_Integer(sSectionModel, "양품", 0);
	iTotalNG = INI_DaySummary.Get_Integer(sSectionModel, "불량", 0);

	if (bResultNG)
		iTotalNG++;
	else
		iTotalOK++;

	if ((iTotalOK + iTotalNG) == 0)
		dFilter = 0;
	else
		dFilter = ((double)iTotalOK / (double)(iTotalOK + iTotalNG))*100.0;

	strFilter.Format("%.2lf", dFilter);

	INI_DaySummary.Set_Integer(sSectionModel, "총량", iTotalOK + iTotalNG);
	INI_DaySummary.Set_Integer(sSectionModel, "양품", iTotalOK);
	INI_DaySummary.Set_Integer(sSectionModel, "불량", iTotalNG);
	INI_DaySummary.Set_String(sSectionModel, "필터링률(%)", strFilter);

	LeaveCriticalSection(&CS_ADJDAYSUMMARYLOG);
}


void CuScanApp::SaveADJTotalImageSummaryINI(CString sPath, CString sSectionModel, BOOL bResultNG, int iMzNo)
{
	EnterCriticalSection(&CS_ADJDAYSUMMARYLOG);

	// ADJDaySummary.txt
	CString sDaySummaryFileName;
	CString sAdjFileName;
	sAdjFileName.Format("\\ADJTotalImageSummary_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);
	m_FileBase.CreatePath(sPath);
	sDaySummaryFileName = sPath + sAdjFileName;
	CIniFileCS INI_DaySummary(sDaySummaryFileName);
	int iCurValue, iTotalOK, iTotalNG, nTotalOKImage, nTotalNGImage;
	double dFilter;
	CString strFilter;
	//

	iTotalOK = INI_DaySummary.Get_Integer(sSectionModel, "양품", 0);
	iTotalNG = INI_DaySummary.Get_Integer(sSectionModel, "불량", 0);

	if (bResultNG)
		iTotalNG++;
	else
		iTotalOK++;

	if ((iTotalOK + iTotalNG) == 0)
		dFilter = 0;
	else
		dFilter = ((double)iTotalOK / (double)(iTotalOK + iTotalNG))*100.0;

	strFilter.Format("%.2lf", dFilter);

	INI_DaySummary.Set_Integer(sSectionModel, "총량", iTotalOK + iTotalNG);
	INI_DaySummary.Set_Integer(sSectionModel, "양품", iTotalOK);
	INI_DaySummary.Set_Integer(sSectionModel, "불량", iTotalNG);
	INI_DaySummary.Set_String(sSectionModel, "필터링률(%)", strFilter);

	LeaveCriticalSection(&CS_ADJDAYSUMMARYLOG);
}

#endif

CString CuScanApp::FormatLotResultString()
{
	CString sDelimiter = _T("\t");
	CString sEol = _T("\r\n");
	CString sHeader = _T("");
	enMODEL_TYPE modelType = enMODEL_TYPE(GetModelType());
	// LOG 헤더 추가
	std::vector<CString> vecHeaderItems = m_pFAIDataManager.GetLotResultHeaders();
	for (CString sHeaderItem : vecHeaderItems)
	{
		sHeader += sHeaderItem + sDelimiter;
	}

	sHeader += sEol;

	return sHeader;
}

CString CuScanApp::FormatLotResultString(const LogHeaderInfoStruct& headerInfo)
{
	CString sDelimiter = _T("\t");
	CString sLog = _T("");

	// 1. InspDate
	sLog.AppendFormat("%s%s", headerInfo.sInspDate, sDelimiter);

	// 2. InspTime
	sLog.AppendFormat("%s%s", headerInfo.sInspTime, sDelimiter);

	// 3. Station
	sLog.AppendFormat("%s%s", headerInfo.sPCID, sDelimiter);

	// 4. EquipNo
	sLog.AppendFormat("%s%s", headerInfo.sEquipNo, sDelimiter);

	// 5. LotID
	sLog.AppendFormat("%s%s", headerInfo.sLotID, sDelimiter);

	// 6. Config
	sLog.AppendFormat("%s%s", headerInfo.sSaveConfig, sDelimiter);

	// 7. TrayNo
	sLog.AppendFormat("%d%s", headerInfo.iTrayNo, sDelimiter);

	// 8. ModuleNo
	sLog.AppendFormat("%d%s", headerInfo.iModuleNo, sDelimiter);

	// 9. Barcode
	sLog.AppendFormat("%s%s", headerInfo.sBarcode, sDelimiter);

	// 10. StageNo
	sLog.AppendFormat("%d%s", headerInfo.iStageNo, sDelimiter);

	// 11. JigNo
	sLog.AppendFormat("%d%s", headerInfo.iJigNo, sDelimiter);

	// 12 ~ 12 + n. 비전 결과
	for (auto& result : headerInfo.vecResultList)
	{
		sLog.AppendFormat("%s%s", result, sDelimiter);
	}

	// 13 + n + m. AI Result List
	for (auto& aiResult : headerInfo.vecAIResultList)
	{
		sLog.AppendFormat("%s%s", aiResult, sDelimiter);
	}

	//  14 + n + m. FAI NG Item	
	sLog.AppendFormat("%s%s", headerInfo.sFAINGItem, sDelimiter);

	// FAIItems
	const vector<double>& vecFAIItems = headerInfo.vecFAIItems;
	size_t iFAILength = vecFAIItems.size();
	for (size_t i = 0; i < iFAILength; i++)
	{
		sLog.AppendFormat("%.4lf", vecFAIItems[i]);

		if (i < iFAILength - 1)
			sLog += sDelimiter;
	}
	return sLog;
}

CString CuScanApp::FormatContactPointString()
{
	CString sDelimiter = _T("\t");
	CString sEol = _T("\r\n");
	CString sHeader = _T("");

	// 2. m_lotResultHeaders (모델 불문 공통 헤더 1-9)
	sHeader.AppendFormat(_T("Date") + sDelimiter);            // 1
	sHeader.AppendFormat(_T("Time") + sDelimiter);            // 2
	sHeader.AppendFormat(_T("Station") + sDelimiter);		  // 3
	sHeader.AppendFormat(_T("Machine_Code") + sDelimiter);	  // 4
	sHeader.AppendFormat(_T("LotNum") + sDelimiter);          // 5
	sHeader.AppendFormat(_T("Config") + sDelimiter);          // 6
	sHeader.AppendFormat(_T("Tray") + sDelimiter);            // 7
	sHeader.AppendFormat(_T("ModuleNo") + sDelimiter);        // 8
	sHeader.AppendFormat(_T("Barcode") + sDelimiter);         // 9
	sHeader.AppendFormat(_T("StageNo") + sDelimiter);         // 10
	sHeader.AppendFormat(_T("JigNo") + sDelimiter);           // 11

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		// Vision 헤더 추가
#if defined (UAVI_BOI) || defined (UAVI_BOS) || defined (UAVI_KRIOS)
		if (i > VISION_NUMBER_2)
			break;
#endif
		if (THEAPP.Struct_PreferenceStruct.m_bUseVision[i])
			sHeader.AppendFormat(THEAPP.m_ModelDefineInfo.m_strVisionName[i] + _T("(VAR)") + sDelimiter);
	}

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		// AI Step 헤더 추가
#if defined (UAVI_BOI) || defined (UAVI_BOS) || defined (UAVI_KRIOS)
		if (i > VISION_NUMBER_2)
			break;
#endif
		if (THEAPP.Struct_PreferenceStruct.m_bUseVision[i])
			sHeader.AppendFormat(THEAPP.m_ModelDefineInfo.m_strVisionName[i] + _T("(AI)") + sDelimiter);
	}

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
#if defined (UAVI_BOI) || defined (UAVI_BOS) || defined (UAVI_KRIOS)
		if (i > VISION_NUMBER_2)
			break;
#endif
		if (THEAPP.Struct_PreferenceStruct.m_bUseVision[i])
		{
			sHeader.AppendFormat(THEAPP.m_ModelDefineInfo.m_strVisionName[i] + _T(" InspectionType") + sDelimiter);
			sHeader.AppendFormat(THEAPP.m_ModelDefineInfo.m_strVisionName[i] + _T(" ResultNo") + sDelimiter);
			sHeader.AppendFormat(THEAPP.m_ModelDefineInfo.m_strVisionName[i] + _T(" CP_X") + sDelimiter);
			sHeader.AppendFormat(THEAPP.m_ModelDefineInfo.m_strVisionName[i] + _T(" CP_Y") + sDelimiter);
		}
	}

	sHeader += sEol;

	return sHeader;
}

CString CuScanApp::FormatContactPointString(const LogHeaderInfoStruct& headerInfo)
{
	CString sDelimiter = _T("\t");
	CString sLog = _T("");

	// 1. InspDate
	sLog.AppendFormat("%s%s", headerInfo.sInspDate, sDelimiter);

	// 2. InspTime
	sLog.AppendFormat("%s%s", headerInfo.sInspTime, sDelimiter);

	// 3. Station
	sLog.AppendFormat("%s%s", headerInfo.sPCID, sDelimiter);

	// 4. EquipNo
	sLog.AppendFormat("%s%s", headerInfo.sEquipNo, sDelimiter);

	// 5. LotID
	sLog.AppendFormat("%s%s", headerInfo.sLotID, sDelimiter);

	// 6. Config
	sLog.AppendFormat("%s%s", headerInfo.sSaveConfig, sDelimiter);

	// 7. TrayNo
	sLog.AppendFormat("%d%s", headerInfo.iTrayNo, sDelimiter);

	// 8. ModuleNo
	sLog.AppendFormat("%d%s", headerInfo.iModuleNo, sDelimiter);

	// 9. Barcode
	sLog.AppendFormat("%s%s", headerInfo.sBarcode, sDelimiter);

	// 10. StageNo
	sLog.AppendFormat("%d%s", headerInfo.iStageNo, sDelimiter);

	// 11. JigNo
	sLog.AppendFormat("%d%s", headerInfo.iJigNo, sDelimiter);

	// 11. 비전 결과
	for (auto& result : headerInfo.vecResultList)
	{
		sLog.AppendFormat("%s%s", result, sDelimiter);
	}

	// 12. AI Result List
	int groupSize = 3;
	for (size_t i = 0; i < headerInfo.vecAIResultList.size(); i += groupSize)
	{
		bool hasGood = false;
		bool hasNg = false;

		for (size_t j = i; j < i + groupSize && j < headerInfo.vecAIResultList.size(); ++j)
		{
			const CString& result = headerInfo.vecAIResultList[j];

			if (result == _T("G"))
			{
				hasGood = true;
				break;
			}

			if (result == _T("N"))
				hasNg = true;
		}

		if(hasGood)
			sLog.AppendFormat(_T("%s%s"), _T("G"), sDelimiter);
		else if (hasNg)
			sLog.AppendFormat(_T("%s%s"), _T("N"), sDelimiter);
		else
			sLog.AppendFormat(_T("%s%s"), _T("nan"), sDelimiter);
	}

	// 13. Defect Blob Info
	for (auto& result : headerInfo.vecDefectBlobInfoList)
	{
		sLog.AppendFormat("%s%s", result.sInspectType, sDelimiter);
		sLog.AppendFormat("%d%s", result.iResultImageNo, sDelimiter);
		sLog.AppendFormat("%d%s", result.iDefectCenterX, sDelimiter);
		sLog.AppendFormat("%d%s", result.iDefectCenterY, sDelimiter);
	}

	return sLog;
}

void CuScanApp::SaveContactPointLog(CString sPath, CString strResult, int iMzNo)
{
	EnterCriticalSection(&CS_LOTRESULTLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);
	CString sLotResultFileName;
	sLotResultFileName.Format("\\Contact_Point_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);

	strFile.Format(sPath + sLotResultFileName);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_LOTRESULTLOG);
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		// Set Header
		if (file.GetPosition() == 0)
		{
			strSave = FormatContactPointString();

			file.Write(strSave, strSave.GetLength());
		}

		// Set Data (Body)
		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();

	}
	catch (CFileException* pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_LOTRESULTLOG);
}


void CuScanApp::SaveLotResultLog(CString sPath, CString strResult, int iMzNo)
{
	EnterCriticalSection(&CS_LOTRESULTLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);
	CString sLotResultFileName;
	sLotResultFileName.Format("\\LotResult_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);

	strFile.Format(sPath + sLotResultFileName);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_LOTRESULTLOG);
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		// Set Header
		if (file.GetPosition() == 0)
		{
			CString strModelType = g_strModelTypeName[GetModelType()];
			if (strModelType == _T("CHS-W") || strModelType == _T("CHS-Z"))
				strSave.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTray\tModuleNo\tBarcode\tStageNo\tJigNo\tBottom\tTop\tFAI_NG_Item\tFAI-3(mm)\tFAI-3-1(mm)\tFAI-3-2(mm)m\tFAI-5(mm)\tFAI-6(mm)\tFAI-9(mm)\tFAI-9-1(mm)\tFAI-9-2(mm)\tFAI-9-3(mm)\tFAI-10(mm)\tFAI-10-1(mm)\tFAI-10-2(mm)\tFAI-11(mm)\tFAI-12(mm)\tFAI-13(mm)\tFAI-14(mm)\tFAI-14-1(mm)\tFAI-14-2(mm)\tFAI-17-1(mm)\tFAI-17-1-1(mm)\tFAI-17-1-2(mm)\tFAI-17-1-3(mm)\tFAI-17-1-4(mm)\tFAI-17-2(mm)\tFAI-17-2-1(mm)\tFAI-17-2-2(mm)\tFAI-17-2-3(mm)\tFAI-17-2-4(mm)\tFAI-17-3(mm)\tFAI-17-3-1(mm)\tFAI-17-3-2(mm)\tFAI-17-3-3(mm)\tFAI-17-3-4(mm)\tFAI-18(mm)\tFAI-18-1(mm)\tFAI-18-2(mm)\tFAI-18-3(mm)\tFAI-19(mm)\tFAI-19-1(mm)\tFAI-19-2(mm)\tFAI-19-3(mm)\tFAI-19-4(mm)\tFAI-21(mm)\tFAI-22(mm)\tD/C_2(deg)\tFAI-23(mm)\tFAI-24(mm)\tFAI-25(mm)\r\n");
			else if (strModelType == _T("CHS-K") || strModelType == _T("CHS-S") || strModelType == _T("CHS-T") || strModelType == _T("CHS-V"))
				strSave.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTray\tModuleNo\tBarcode\tStageNo\tJigNo\tBottom\tTop\tFAI_NG_Item\tFAI-5(mm)\tFAI-6(mm)\tFAI-7(mm)\tFAI-7-1(mm)\tFAI-7-2(mm)\tFAI-9(mm)\tFAI-9-1(mm)\tFAI-9-2(mm)\tFAI-9-3(mm)\tFAI-9-4(mm)\tFAI-9-5(mm)\tFAI-10(mm)\tFAI-10-1(mm)\tFAI-10-2(mm)\tFAI-10-3(mm)\tFAI-11(mm)\tFAI-11-1(mm)\tFAI-11-2(mm)\tFAI-11-3(mm)\tFAI-11-4(mm)\tFAI-11-5(mm)\tFAI-12(mm)\tFAI-13(mm)\tFAI-16-1(mm)\tFAI-16-1-1(mm)\tFAI-16-1-2(mm)\tFAI-16-1-3(mm)\tFAI-16-1-4(mm)\tFAI-16-2(mm)\tFAI-16-2-1(mm)\tFAI-16-2-2(mm)\tFAI-16-2-3(mm)\tFAI-16-2-4(mm)\tFAI-16-3(mm)\tFAI-16-3-1(mm)\tFAI-16-3-2(mm)\tFAI-16-3-3(mm)\tFAI-16-3-4(mm)\tFAI-18(mm)\tFAI-18-1(mm)\tFAI-18-2(mm)\tFAI-18-3(mm)\tFAI-19(mm)\tFAI-19-1(mm)\tFAI-19-2(mm)\tFAI-19-3(mm)\tFAI-19-4(mm)\tFAI-21(mm)\tFAI-23(mm)\tFAI-24(mm)\tFAI-25(mm)\tFAI-25-1(mm)\tFAI-25-2(mm)\tFAI-27(deg)\tD/C_2(deg)\r\n");
			else
				strSave = FormatLotResultString();

			file.Write(strSave, strSave.GetLength());
		}

		// Set Data (Body)
		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();

	}
	catch (CFileException* pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_LOTRESULTLOG);
}

void CuScanApp::SaveDayLotResultLog(CString sPath, CString strResult, int iMzNo)
{
	EnterCriticalSection(&CS_LOTRESULTLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);
	CString sDayLotResultFileName;
	sDayLotResultFileName.Format("\\DayLotResult_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);

	strFile.Format(sPath + sDayLotResultFileName);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_LOTRESULTLOG);
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		// Set Header
		if (file.GetPosition() == 0)
		{
			CString strModelType = g_strModelTypeName[GetModelType()];
			if (strModelType == _T("CHS-W") || strModelType == _T("CHS-Z"))
				strSave.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTray\tModuleNo\tBarcode\tStageNo\tJigNo\tBottom\tTop\tFAI_NG_Item\tFAI-3(mm)\tFAI-3-1(mm)\tFAI-3-2(mm)m\tFAI-5(mm)\tFAI-6(mm)\tFAI-9(mm)\tFAI-9-1(mm)\tFAI-9-2(mm)\tFAI-9-3(mm)\tFAI-10(mm)\tFAI-10-1(mm)\tFAI-10-2(mm)\tFAI-11(mm)\tFAI-12(mm)\tFAI-13(mm)\tFAI-14(mm)\tFAI-14-1(mm)\tFAI-14-2(mm)\tFAI-17-1(mm)\tFAI-17-1-1(mm)\tFAI-17-1-2(mm)\tFAI-17-1-3(mm)\tFAI-17-1-4(mm)\tFAI-17-2(mm)\tFAI-17-2-1(mm)\tFAI-17-2-2(mm)\tFAI-17-2-3(mm)\tFAI-17-2-4(mm)\tFAI-17-3(mm)\tFAI-17-3-1(mm)\tFAI-17-3-2(mm)\tFAI-17-3-3(mm)\tFAI-17-3-4(mm)\tFAI-18(mm)\tFAI-18-1(mm)\tFAI-18-2(mm)\tFAI-18-3(mm)\tFAI-19(mm)\tFAI-19-1(mm)\tFAI-19-2(mm)\tFAI-19-3(mm)\tFAI-19-4(mm)\tFAI-21(mm)\tFAI-22(mm)\tD/C_2(deg)\tFAI-23(mm)\tFAI-24(mm)\tFAI-25(mm)\r\n");
			else if (strModelType == _T("CHS-K") || strModelType == _T("CHS-S") ||
					 strModelType == _T("CHS-T") || strModelType == _T("CHS-V"))
				strSave.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTray\tModuleNo\tBarcode\tStageNo\tJigNo\tBottom\tTop\tFAI_NG_Item\tFAI-5(mm)\tFAI-6(mm)\tFAI-7(mm)\tFAI-7-1(mm)\tFAI-7-2(mm)\tFAI-9(mm)\tFAI-9-1(mm)\tFAI-9-2(mm)\tFAI-9-3(mm)\tFAI-9-4(mm)\tFAI-9-5(mm)\tFAI-10(mm)\tFAI-10-1(mm)\tFAI-10-2(mm)\tFAI-10-3(mm)\tFAI-11(mm)\tFAI-11-1(mm)\tFAI-11-2(mm)\tFAI-11-3(mm)\tFAI-11-4(mm)\tFAI-11-5(mm)\tFAI-12(mm)\tFAI-13(mm)\tFAI-16-1(mm)\tFAI-16-1-1(mm)\tFAI-16-1-2(mm)\tFAI-16-1-3(mm)\tFAI-16-1-4(mm)\tFAI-16-2(mm)\tFAI-16-2-1(mm)\tFAI-16-2-2(mm)\tFAI-16-2-3(mm)\tFAI-16-2-4(mm)\tFAI-16-3(mm)\tFAI-16-3-1(mm)\tFAI-16-3-2(mm)\tFAI-16-3-3(mm)\tFAI-16-3-4(mm)\tFAI-18(mm)\tFAI-18-1(mm)\tFAI-18-2(mm)\tFAI-18-3(mm)\tFAI-19(mm)\tFAI-19-1(mm)\tFAI-19-2(mm)\tFAI-19-3(mm)\tFAI-19-4(mm)\tFAI-21(mm)\tFAI-23(mm)\tFAI-24(mm)\tFAI-25(mm)\tFAI-25-1(mm)\tFAI-25-2(mm)\tFAI-27(deg)\tD/C_2(deg)\r\n");
			else
				strSave = FormatLotResultString();

			file.Write(strSave, strSave.GetLength());
		}

		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();

	}
	catch (CFileException* pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_LOTRESULTLOG);
}

void CuScanApp::SaveMultipleDefectListLog(CString sPath, CString strResult)
{
	CString strFile;
	m_FileBase.CreatePath(sPath);
	CString sLotResultFileName;

	sLotResultFileName.Format("\\SamePosDefectList_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);

	strFile.Format(sPath + sLotResultFileName);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave.Format("Barcode\tDate\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTray\tModuleNo\tVision\tDefectName\tSamePosDefectCount\r\n");

			file.Write(strSave, strSave.GetLength());
		}

		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();

	}
	catch (CFileException *pEx) {
		pEx->Delete();
	}
}

void CuScanApp::SaveMonitorGVEdgeLog(CString sPath, CString strResult)
{
	EnterCriticalSection(&CS_COLORINFOLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);
	strFile.Format(sPath + "\\Monitoring_GV_EdgeStr_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_COLORINFOLOG);
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTrayNo\tModuleNo\tBarcode\tVision\tImageNo\tMeasureROINo\tAvgGV\tEdgeStr\r\n");

			file.Write(strSave, strSave.GetLength());
		}

		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();
	}
	catch (CFileException *pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_COLORINFOLOG);
}

void CuScanApp::SaveMonitorContDefectLog(CString sPath, CString strResult)
{
	EnterCriticalSection(&CS_COLORINFOLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);
	strFile.Format(sPath + "\\Monitoring_Continuous_Defect.txt");

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_COLORINFOLOG);
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTrayNo\tModuleNo\tVision\t불량픽셀위치(X)\t불량픽셀위치(Y)\t연속불량횟수\r\n");

			file.Write(strSave, strSave.GetLength());
		}

		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();
	}
	catch (CFileException *pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_COLORINFOLOG);
}


void CuScanApp::NewSaveDefectVariationFeatureLog(CString sPath, CString strResult)
{
	EnterCriticalSection(&CS_DEFECTVARIATIONFEATURELOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);

	strFile.Format(sPath + "\\Defect_Variation_Feature_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_DEFECTVARIATIONFEATURELOG);
		return;
	}

	try {
		file.SeekToEnd();

		CString sVariationFeatureLog;
		if (file.GetPosition() == 0)
		{
			CString sRunInfo;
			sRunInfo.Format("Date\tTime\tStation\tEquipNo\tRecipeName\tSWParamVer\tHWParamVer\tMzNo\tLotNum\tTrayNo\tModuleNo\tBarcode");

			CString sInspectROIInfo;
			sInspectROIInfo.Format("Vision\tStage\tInspectionType\tDefectType\tImageNo\tROINo\tInspectionTabNo\t(TeachROIPosition)Left\t(TeachROIPosition)Top\t(TeachROIPosition)Right\t(TeachROIPosition)Bottom\t(LocalAlign)ShiftX\t(LocalAlign)ShiftY\t(LocalAlign)RotateAngle\t(ImagePreprocess)FilterType\t(ImagePreprocess)FilterX\t(ImagePreprocess)FilterY\tImageChannel");

			CString sVariationParameterFeature;
			sVariationParameterFeature.Format("(ImageCompareParam)BrightAbs\t(ImageCompareParam)BrightVar\t(ImageCompareParam)DarkAbs\t(ImageCompareParam)DarkVar");

			CString sReferenceROIVariationFeature;
			sReferenceROIVariationFeature.Format("(ReferenceROI)BrightAvgGV\t(ReferenceROI)MaxGV\t(ReferenceROI)MinGV\t(ReferenceROI)StdevGV");

			CString sCurrentROIVariationFeature;
			sCurrentROIVariationFeature.Format("(CurrentROI)BrightAvgGV\t(CurrentROI)MaxGV\t(CurrentROI)MinGV\t(CurrentROI)StdevGV\t(CurrentROI)DeviationAvgGV\t(CurrentROI)DeviationMaxGV\t(CurrentROI)DeviationMinGV\t(CurrentROI)PixelCountALL\t(CurrentROI)PixelCountOverAvg\t(CurrentROI)PixelCountUnderAvgGV\t(CurrentROI)PixelCountUnderAvgPlusStdev\t(CurrentROI)PixelCountOverAvgMinusStdev");

			CString sBlobVariationFeature;
			sBlobVariationFeature.Format("(Blob)BrightAvgGV\t(Blob)MaxGV\t(Blob)MinGV\t(Blob)StdevGV\t(Blob)DeviationAvgGV\t(Blob)DeviationMaxGV\t(Blob)DeviationMinGV\t(Blob)PixelCountALL\t(Blob)PixelCountOverAvg\t(Blob)PixelCountUnderAvgGV\t(Blob)PixelCountUnderAvgPlusStdev\t(Blob)PixelCountOverAvgMinusStdev");

			CString sBlobDefectShapeFeature1;
			sBlobDefectShapeFeature1.Format("(DefectShape)Area\t(DefectShape)Row\t(DefectShape)Column\t(DefectShape)Width\t(DefectShape)Height\t(DefectShape)Row1\t(DefectShape)Column1\t(DefectShape)Row2\t(DefectShape)Column2\t(DefectShape)LengthPerWidthRatio\t(DefectShape)MajorPerMinorAxisRatio\t(DefectShape)BlobPerROIAreaRatio\t(DefectShape)Contrast\t(DefectShape)DistanceFromROIToBlob\t(DefectShape)DistanceFromROILeftToBlobLeft\t(DefectShape)DistanceFromROITopToBlobTop\t(DefectShape)DistanceFromROIRightToBlobRight\t(DefectShape)DistanceFromROIBottomToBlobBottom\t(DefectShape)ConnectedPointToROI\t(DefectShape)BoundaryGradient\t(DefectShape)DistanceXFromROICenterToBlobCenter\t(DefectShape)DistanceYROICenterToBlobCenter");

			CString sBlobDefectShapeFeature2;
			sBlobDefectShapeFeature2.Format("(DefectShape)Circularity\t(DefectShape)Compactness\t(DefectShape)Contlength\t(DefectShape)Convexity\t(DefectShape)Rectangulary\t(DefectShape)ra\t(DefectShape)rb\t(DefectShape)phi\t(DefectShape)Anisometry\t(DefectShape)Bulkiness\t(DefectShape)StructFactor\t(DefectShape)OuterCircleRadius\t(DefectShape)InnerCircleRadius\t(DefectShape)InnerRectangleWidth\t(DefectShape)InnerRectangleHeight");

			CString sBlobDefectShapeFeature3;
			sBlobDefectShapeFeature3.Format("(DefectShape)DistanceMean\t(DefectShape)DistanceDeviation\t(DefectShape)Roundness\t(DefectShape)NumberOfSides\t(DefectShape)NumberOfConnect\t(DefectShape)NumberOfHole\t(DefectShape)HoleArea\t(DefectShape)MaxDiameter\t(DefectShape)Orientation\t(DefectShape)EulerNumber\t(DefectShape)Rectangle2Phi\t(DefectShape)Rectangle2Length1\t(DefectShape)Rectangle2Length2");

			CString sBlobDefectShapeFeature4;
			sBlobDefectShapeFeature4.Format("(DefectShape)MomentsM11\t(DefectShape)MomentsM20\t(DefectShape)MomentsM02\t(DefectShape)MomentsIa\t(DefectShape)MomentsIb\t(DefectShape)MomentsM11Invar\t(DefectShape)MomentsM20Invar\t(DefectShape)MomentsM02Invar\t(DefectShape)MomentsPhi1\t(DefectShape)MomentsPhi2\t(DefectShape)MomentsM21\t(DefectShape)MomentsM12\t(DefectShape)MomentsM03\t(DefectShape)MomentsM30\t(DefectShape)MomentsM21Invar\t(DefectShape)MomentsM12Invar\t(DefectShape)MomentsM03Invar\t(DefectShape)MomentsM30Invar\t(DefectShape)MomentsI1\t(DefectShape)MomentsI2\t(DefectShape)MomentsI3\t(DefectShape)MomentsI4\t(DefectShape)MomentsPsi1\t(DefectShape)MomentsPsi2\t(DefectShape)MomentsPsi3\t(DefectShape)MomentsPsi4");

			CString sDefectConditionParameterFeature1;
			sDefectConditionParameterFeature1.Format("(DefectCondition)UseArea\t(DefectCondition)AreaMin\t(DefectCondition)AreaMax\t(DefectCondition)UseLength\t(DefectCondition)LengthMin\t(DefectCondition)LengthMax\t(DefectCondition)UseWidth\t(DefectCondition)WidthMin\t(DefectCondition)WidthMax\t(DefectCondition)UseXLength\t(DefectCondition)XLengthMin\t(DefectCondition)XLengthMax\t(DefectCondition)UseYLength\t(DefectCondition)YLengthMin\t(DefectCondition)YLengthMax\t(DefectCondition)UseOrthogonalLength\t(DefectCondition)OrthoLengthRefSide\t(DefectCondition)UseGvMean\t(DefectCondition)GVMeanMin\t(DefectCondition)GVMeanMax\t(DefectCondition)UseGVStdev\t(DefectCondition)GVStdevMin\t(DefectCondition)GVStdevMax\t(DefectCondition)UseLengthPerWidthRatio\t(DefectCondition)LengthPerWidthRatioMin\t(DefectCondition)LengthPerWidthRatioMax");

			CString sDefectConditionParameterFeature2;
			sDefectConditionParameterFeature2.Format("(DefectCondition)UseCircularity\t(DefectCondition)CircularityMin\t(DefectCondition)CircularityMax\t(DefectCondition)UseRectangularity\t(DefectCondition)RectangularityMin\t(DefectCondition)RectangularityMax\t(DefectCondition)UseConvexity\t(DefectCondition)ConvexityMin\t(DefectCondition)ConvexityMax\t(DefectCondition)UseMajorPerMinorAxisRatio\t(DefectCondition)MajorPerMinorAxisRatioMin\t(DefectCondition)MajorPerMinorAxisRatioMax\t(DefectCondition)UseBlobPerROIAreaRatio\t(DefectCondition)BlobPerROIAreaMin\t(DefectCondition)BlobPerROIAreaMax\t(DefectCondition)UseContrast\t(DefectCondition)ContrastMin\t(DefectCondition)ContrastMax");

			CString sDefectConditionParameterFeature3;
			sDefectConditionParameterFeature3.Format("(DefectCondition)UseDistanceFromOuterROItoBlob\t(DefectCondition)DistanceFromOuter\t(DefectCondition)UseDistanceFromInnerROItoBlob\t(DefectCondition)DistanceFromInnerROI\t(DefectCondition)UseNumberOfBlobs\t(DefectCondition)BlobNumberMin\t(DefectCondition)BlobNumberMax\t(DefectCondition)UseConnectedToROI\t(DefectCondition)UseConnectedToROITop\t(DefectCondition)ConnectedTopOffset\t(DefectCondition)UseConnectedToROIBottom\t(DefectCondition)ConnectedBottomOffset\t(DefectCondition)UseConnectedToROIHorizontalCenter\t(DefectCondition)ConnectedHorizontalCenterOffset\t(DefectCondition)UseConnectedToROILeft\t(DefectCondition)ConnectedLeftOffset\t(DefectCondition)UseConnectedToROIRight\t(DefectCondition)ConnectedRightOffset\t(DefectCondition)UseConnectedToROIVerticalCenter\t(DefectCondition)ConnectedVerticalCenterOffset\t(DefectCondition)UseConnectedToROIAny\t(DefectCondition)ConnectedAnyOffset\t(DefectCondition)UseConnectedToROI2Sides\t(DefectCondition)UseConnectedFromROIInnerToROIOuter");

			CString sDefectConditionParameterFeature4;
			sDefectConditionParameterFeature4.Format("(DefectCondition)UseBoundaryGradient\t(DefectCondition)BoundaryGradientMin\t(DefectCondition)BoundaryGradientMax\t(DefectCondition)UseIncludedHoles\t(DefectCondition)IncludedHolesNumber\t(DefectCondition)UseSumXLength\t(DefectCondition)SumXLengthMin\t(DefectCondition)SumXLengthMax\t(DefectCondition)UseSumYLength\t(DefectCondition)SumYLengthMin\t(DefectCondition)SumYLengthMax\t(DefectCondition)UseDistanceFromROICenterToBlobCenter\t(DefectCondition)DistanceFromROICenterToBlobCenterMin\t(DefectCondition)DistanceFromROICenterToBlobCenterMax\t(DefectCondition)UseZeroAreaDetect\t(DefectCondition)UseDilationArea\t(DefectCondition)DilationValue");

			CString sSVMResult;
			sSVMResult.Format("(SVMResult)ClassResult");

			CString sSecondaryResult;
			sSecondaryResult.Format("(SecondaryResult)ClassResult\t(SecondaryResult)MaxScore");

			CString sTertiaryResult;
			sTertiaryResult.Format("(TertiaryResult)Area\t(TertiaryResult)Length\t(TertiaryResult)Width\t(TertiaryResult)Contrast\t(TertiaryResult)SizeSpecMin\t(TertiaryResult)SizeSpecMid\t(TertiaryResult)SizeSpecMax\t(TertiaryResult)CountSpecMinToMid\t(TertiaryResult)CountSpecMidToMax\t(TertiaryResult)CountSpecMax");

			sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\r\n",
				sRunInfo,
				sInspectROIInfo,
				sVariationParameterFeature,
				sReferenceROIVariationFeature,
				sCurrentROIVariationFeature,
				sBlobVariationFeature,
				sBlobDefectShapeFeature1,
				sBlobDefectShapeFeature2,
				sBlobDefectShapeFeature3,
				sBlobDefectShapeFeature4,
				sDefectConditionParameterFeature1,
				sDefectConditionParameterFeature2,
				sDefectConditionParameterFeature3,
				sDefectConditionParameterFeature4,
				sSVMResult,
				sSecondaryResult,
				sTertiaryResult
			);

			file.Write(sVariationFeatureLog, sVariationFeatureLog.GetLength());
		}

		sVariationFeatureLog.Format("%s\r\n", strResult);

		file.Write(sVariationFeatureLog, sVariationFeatureLog.GetLength());
		file.Close();
	}
	catch (CFileException* pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_DEFECTVARIATIONFEATURELOG);
}


void CuScanApp::TempSaveDefectVariationFeatureLog(CString sPath, CString strResult)
{
	EnterCriticalSection(&CS_DEFECTVARIATIONFEATURELOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);

	strFile.Format(sPath + "\\Defect_Variation_Feature_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_DEFECTVARIATIONFEATURELOG);
		return;
	}

	try {
		file.SeekToEnd();

		CString sVariationFeatureLog;
		if (file.GetPosition() == 0)
		{
			CString sRunInfo;
			sRunInfo.Format("Date\tTime\tStation\tEquipNo\tRecipeName\tSWParamVer\tHWParamVer\tMzNo\tLotNum\tTrayNo\tModuleNo\tBarcode");

			CString sInspectROIInfo;
			sInspectROIInfo.Format("Vision\tStage\tInspectionType\tDefectType\tImageNo\tROINo\tInspectionTabNo");

			sVariationFeatureLog.Format("%s\t%s\r\n",
				sRunInfo,
				sInspectROIInfo
			);

			file.Write(sVariationFeatureLog, sVariationFeatureLog.GetLength());
		}

		sVariationFeatureLog.Format("%s\r\n", strResult);

		file.Write(sVariationFeatureLog, sVariationFeatureLog.GetLength());
		file.Close();
	}
	catch (CFileException* pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_DEFECTVARIATIONFEATURELOG);
}


void CuScanApp::SaveInspectConditionLog(CString sPath, CString strResult)
{
	EnterCriticalSection(&CS_INSPECTCONDITIONLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);

	strFile.Format(sPath + "\\Inspect_Condition_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_INSPECTCONDITIONLOG);
		return;
	}

	try {
		file.SeekToEnd();

		CString sInspectConditionLog;
		if (file.GetPosition() == 0)
		{
			// TODO : 하드코딩 변경 필요
			CString sRunInfo;
			sRunInfo.Format("Date\tTime\tStation\tEquipNo\tRecipeName\tMzNo\tLotNum\tTrayNo\tModuleNo\tBarcode");

			CString sInspectROIInfo;
			sInspectROIInfo.Format("Vision\tStage\tInspectionType\tDefectType");

			CString sInspectConditionResult;
			sInspectConditionResult.Format("FinalResult\tTotalBlos\tSizeSpecMin\tCountSpecMid\tCountOverMin\tSizeSpecMid\tCountSpecMax\tCountOverMid\tSizeSpecMax\tCountOverMax");

			sInspectConditionLog.Format("%s\t%s\t%s\r\n",
				sRunInfo,
				sInspectROIInfo,
				sInspectConditionResult
			);

			file.Write(sInspectConditionLog, sInspectConditionLog.GetLength());
		}

		sInspectConditionLog.Format("%s\r\n", strResult);

		file.Write(sInspectConditionLog, sInspectConditionLog.GetLength());
		file.Close();
	}
	catch (CFileException* pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_INSPECTCONDITIONLOG);
}


void CuScanApp::SaveAllDefectsResultLog(CString sPath, CString strResult)
{
	EnterCriticalSection(&CS_ALLDEFECTSRESULTLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);

	strFile.Format(sPath + "\\All_Defects_Result_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_ALLDEFECTSRESULTLOG);
		return;
	}

	try {
		file.SeekToEnd();

		CString sAllDefectsResultLog;
		if (file.GetPosition() == 0)
		{
			sAllDefectsResultLog.Format("Date\tTime\tStation\tMachine_Code\tLotNum\tConfig\tTray\tModuleNo\tBarcode\tNG1\tNG2\tNG3\tNG4\tNG5\tNG6\tNG7\tNG8\tNG9\tNG10\tNG11\tNG12\tNG13\tNG14\tNG15\tNG16\tNG17\tNG18\tNG19\tNG20\r\n");

			file.Write(sAllDefectsResultLog, sAllDefectsResultLog.GetLength());
		}

		sAllDefectsResultLog.Format("%s\r\n", strResult);

		file.Write(sAllDefectsResultLog, sAllDefectsResultLog.GetLength());
		file.Close();
	}
	catch (CFileException* pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_ALLDEFECTSRESULTLOG);
}


void CuScanApp::SaveAutoCalLightResultLog(CString strResult, int iPcVisionNo, int iMzNo)
{
	EnterCriticalSection(&CS_AUTOCALLIGHTRESULTLOG);

	CString strFile;
	CString sFileTitle;

	if (iPcVisionNo == VISION_NUMBER_1)
		sFileTitle = "Vision1_AutoCalLightResult.txt";
	else if (iPcVisionNo == VISION_NUMBER_2)
		sFileTitle = "Vision2_AutoCalLightResult.txt";

	strFile.Format(m_FileBase.m_strDayResultLogFolder_Local[iPcVisionNo][iMzNo] + "\\" + sFileTitle);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&CS_AUTOCALLIGHTRESULTLOG);
		return;
	}

	try {
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave.Format("영상\t설정_LV1\t설정_GV1\t검사_LV1\t검사_LV1\tCH1 판정값\t설정_LV2\t설정_GV2\t검사_LV2\t검사_LV2\tCH2 판정값\t설정_LV3\t설정_GV3\t검사_LV3\t검사_LV3\tCH3 판정값\t설정_LV4\t설정_GV4\t검사_LV4\t검사_LV4\tCH4 판정값\t설정_LV5\t설정_GV5\t검사_LV5\t검사_LV5\tCH5 판정값\t설정_LV6\t설정_GV6\t검사_LV6\t검사_LV6\tCH6 판정값\t설정_TotalLV\t설정_TotalGV\t검사_TotalLV\t검사_TotalLV\tTotal 판정값\r\n");

			file.Write(strSave, strSave.GetLength());
		}

		strSave.Format("%s\r\n", strResult);

		file.Write(strSave, strSave.GetLength());
		file.Close();
	}
	catch (CFileException *pEx) {
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_AUTOCALLIGHTRESULTLOG);
}

void CuScanApp::SaveDaySummaryINI(CString sPath, CString sSectionDay, CString sDefectCode, BOOL bResultNG, int iMzNo, int iNoModuleRetryGrab, int iNoModuleRetryFocusMove)
{
	EnterCriticalSection(&CS_DAYSUMMARYLOG);

	// DaySummary.txt
	CString sDaySummaryFileName;
	m_FileBase.CreatePath(sPath);
	sDaySummaryFileName.Format(sPath + "\\DaySummary_PC%d.txt", Struct_PreferenceStruct.m_iPCType + 1);
	CIniFileCS INI_DaySummary(sDaySummaryFileName);
	int iCurValue, iTotalOK, iTotalNG;
	double dYield;
	CString strYield;

	int iRetryGrab, iRetryFocusMove;
	//

	iRetryGrab = INI_DaySummary.Get_Integer(sSectionDay, "RetryGrab", 0);
	iRetryFocusMove = INI_DaySummary.Get_Integer(sSectionDay, "RetryFocusMove", 0);
	iRetryGrab += iNoModuleRetryGrab;
	iRetryFocusMove += iNoModuleRetryFocusMove;

	iTotalOK = INI_DaySummary.Get_Integer(sSectionDay, "양품", 0);
	iTotalNG = INI_DaySummary.Get_Integer(sSectionDay, "불량", 0);

	if (bResultNG)
		iTotalNG++;
	else
		iTotalOK++;

	if ((iTotalOK + iTotalNG) == 0)
		dYield = 0;
	else
		dYield = ((double)iTotalOK / (double)(iTotalOK + iTotalNG))*100.0;

	strYield.Format("%.2lf", dYield);

	INI_DaySummary.Set_Integer(sSectionDay, "총량", iTotalOK + iTotalNG);
	INI_DaySummary.Set_Integer(sSectionDay, "양품", iTotalOK);
	INI_DaySummary.Set_Integer(sSectionDay, "불량", iTotalNG);
	INI_DaySummary.Set_String(sSectionDay, "수율(%)", strYield);

	INI_DaySummary.Set_Integer(sSectionDay, "RetryGrab", iRetryGrab);
	INI_DaySummary.Set_Integer(sSectionDay, "RetryFocusMove", iRetryFocusMove);

	if (bResultNG)
	{
		iCurValue = INI_DaySummary.Get_Integer(sSectionDay, sDefectCode, 0);
		INI_DaySummary.Set_Integer(sSectionDay, sDefectCode, iCurValue + 1);
	}

	LeaveCriticalSection(&CS_DAYSUMMARYLOG);
}

void CuScanApp::AddBarcodeOnImageFile(int iVisionCamType, CString strRawImageFolderPath_Local, CString strResultImageFolderPath_Local,
	CString strReviewImageFolderPath_Local, CString strADJImageFolderPath_Local,
	CString strRawImageFolderPath_LAS, CString strResultImageFolderPath_LAS,
	CString strReviewImageFolderPath_LAS, CString strADJImageFolderPath_LAS,
	int iNoInspectImage, int iMzNo, int iTrayNo, int iModuleNo, CString sBarcodeID)
{
	CString strFileFullName, ImageFileName, SearchImageFileName;
	CString strRenameFileFullName, RenameImageFileName;
	CString strSourceFileFullName;

	HANDLE				hFindFile;
	WIN32_FIND_DATA		FindFileData;

	int k;
	int iRenameRet = 0;

	CString sVisionCamType_Short;
	sVisionCamType_Short = m_ModelDefineInfo.m_strVisionName_Short[iVisionCamType];

	//Raw Image

	if (Struct_PreferenceStruct.m_bCombineRawImage)
	{
		ImageFileName.Format("Module%d_%s_Image%02d", iModuleNo, sVisionCamType_Short, iNoInspectImage);
		SearchImageFileName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, iNoInspectImage);

		if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
		{
			if (!Struct_PreferenceStruct.m_bRawImageType)
				strFileFullName = strRawImageFolderPath_Local + "\\" + SearchImageFileName + ".bmp";
			else
				strFileFullName = strRawImageFolderPath_Local + "\\" + SearchImageFileName + ".jpg";
		}
		else
		{
			if (!Struct_PreferenceStruct.m_bRawImageType)
				strFileFullName = strRawImageFolderPath_LAS + "\\" + SearchImageFileName + ".bmp";
			else
				strFileFullName = strRawImageFolderPath_LAS + "\\" + SearchImageFileName + ".jpg";
		}

		if ((hFindFile = FindFirstFile(strFileFullName, &FindFileData)) != INVALID_HANDLE_VALUE)
		{
			if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
			{
				strSourceFileFullName.Format("%s\\%s", strRawImageFolderPath_Local, (LPCTSTR)FindFileData.cFileName);
			}
			else
			{
				strSourceFileFullName.Format("%s\\%s", strRawImageFolderPath_LAS, (LPCTSTR)FindFileData.cFileName);
			}
			FindClose(hFindFile);

			RenameImageFileName.Format("%s_%s", ImageFileName, sBarcodeID);

			if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
			{
				if (!Struct_PreferenceStruct.m_bRawImageType)
					strRenameFileFullName = strRawImageFolderPath_Local + "\\" + RenameImageFileName + ".bmp";
				else
					strRenameFileFullName = strRawImageFolderPath_Local + "\\" + RenameImageFileName + ".jpg";
			}
			else
			{
				if (!Struct_PreferenceStruct.m_bRawImageType)
					strRenameFileFullName = strRawImageFolderPath_LAS + "\\" + RenameImageFileName + ".bmp";
				else
					strRenameFileFullName = strRawImageFolderPath_LAS + "\\" + RenameImageFileName + ".jpg";
			}

			iRenameRet = rename(strSourceFileFullName, strRenameFileFullName);
		}
	}
	else
	{
		for (k = 0; k < iNoInspectImage; k++)
		{
			ImageFileName.Format("Module%d_%s_Image%02d", iModuleNo, sVisionCamType_Short, k + 1);
			SearchImageFileName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, k + 1);

			if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
			{
				if (!Struct_PreferenceStruct.m_bRawImageType)
					strFileFullName = strRawImageFolderPath_Local + "\\" + SearchImageFileName + ".bmp";
				else
					strFileFullName = strRawImageFolderPath_Local + "\\" + SearchImageFileName + ".jpg";
			}
			else
			{
				if (!Struct_PreferenceStruct.m_bRawImageType)
					strFileFullName = strRawImageFolderPath_LAS + "\\" + SearchImageFileName + ".bmp";
				else
					strFileFullName = strRawImageFolderPath_LAS + "\\" + SearchImageFileName + ".jpg";
			}

			if ((hFindFile = FindFirstFile(strFileFullName, &FindFileData)) != INVALID_HANDLE_VALUE)
			{
				if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
				{
					strSourceFileFullName.Format("%s\\%s", strRawImageFolderPath_Local, (LPCTSTR)FindFileData.cFileName);
				}
				else
				{
					strSourceFileFullName.Format("%s\\%s", strRawImageFolderPath_LAS, (LPCTSTR)FindFileData.cFileName);
				}
				FindClose(hFindFile);

				RenameImageFileName.Format("%s_%s", ImageFileName, sBarcodeID);

				if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
				{
					if (!Struct_PreferenceStruct.m_bRawImageType)
						strRenameFileFullName = strRawImageFolderPath_Local + "\\" + RenameImageFileName + ".bmp";
					else
						strRenameFileFullName = strRawImageFolderPath_Local + "\\" + RenameImageFileName + ".jpg";
				}
				else
				{
					if (!Struct_PreferenceStruct.m_bRawImageType)
						strRenameFileFullName = strRawImageFolderPath_LAS + "\\" + RenameImageFileName + ".bmp";
					else
						strRenameFileFullName = strRawImageFolderPath_LAS + "\\" + RenameImageFileName + ".jpg";
				}

				iRenameRet = rename(strSourceFileFullName, strRenameFileFullName);
			}
		}
	}

	//Result Image
	CFileFind finderResult;
	SearchImageFileName.Format("Module%d_%s_Result*", iModuleNo, sVisionCamType_Short);

	if (Struct_PreferenceStruct.m_bResultImageSaveLocal)
		strFileFullName = strResultImageFolderPath_Local + "\\" + SearchImageFileName + ".jpg";
	else
		strFileFullName = strResultImageFolderPath_LAS + "\\" + SearchImageFileName + ".jpg";

	BOOL bExistFile = finderResult.FindFile(strFileFullName);

	while (bExistFile)
	{
		bExistFile = finderResult.FindNextFile();

		if (finderResult.IsDirectory() || finderResult.IsDots())
			continue;

		CString _fileName = finderResult.GetFileName();

		if (_fileName == _T("Thumbs.db")) continue;

		strSourceFileFullName = finderResult.GetFilePath();

		RenameImageFileName = _fileName;
		RenameImageFileName.Replace("NOINFO", sBarcodeID);

		if (Struct_PreferenceStruct.m_bResultImageSaveLocal)
			strRenameFileFullName = strResultImageFolderPath_Local + "\\" + RenameImageFileName;
		else
			strRenameFileFullName = strResultImageFolderPath_LAS + "\\" + RenameImageFileName;

		rename(strSourceFileFullName, strRenameFileFullName);
	}
	finderResult.Close();

	//ADJ Image
	CFileFind finderADJ;
	SearchImageFileName.Format("*_M%02d_%s_*", iModuleNo, sVisionCamType_Short);

	if (Struct_PreferenceStruct.m_bADJImageSaveLocal)
		strFileFullName = strADJImageFolderPath_Local + "\\" + SearchImageFileName + ".jpg";
	else
		strFileFullName = strADJImageFolderPath_LAS + "\\" + SearchImageFileName + ".jpg";

	bExistFile = finderADJ.FindFile(strFileFullName);

	while (bExistFile)
	{
		bExistFile = finderADJ.FindNextFile();

		if (finderADJ.IsDirectory() || finderADJ.IsDots())
			continue;

		CString _fileName = finderADJ.GetFileName();

		if (_fileName == _T("Thumbs.db")) continue;

		strSourceFileFullName = finderADJ.GetFilePath();

		RenameImageFileName = _fileName;
		RenameImageFileName.Replace("NOINFO", sBarcodeID);

		if (Struct_PreferenceStruct.m_bADJImageSaveLocal)
			strRenameFileFullName = strADJImageFolderPath_Local + "\\" + RenameImageFileName;
		else
			strRenameFileFullName = strADJImageFolderPath_LAS + "\\" + RenameImageFileName;

		rename(strSourceFileFullName, strRenameFileFullName);
	}
	finderADJ.Close();

	//Review Image
	CFileFind finderReview;
	SearchImageFileName.Format("Module%d_%s-*", iModuleNo, sVisionCamType_Short);

	if (Struct_PreferenceStruct.m_bReviewImageSaveLocal)
		strFileFullName = strReviewImageFolderPath_Local + "\\" + SearchImageFileName + ".jpg";
	else
		strFileFullName = strReviewImageFolderPath_LAS + "\\" + SearchImageFileName + ".jpg";

	bExistFile = finderReview.FindFile(strFileFullName);

	while (bExistFile)
	{
		bExistFile = finderReview.FindNextFile();

		if (finderReview.IsDirectory() || finderReview.IsDots())
			continue;

		CString _fileName = finderReview.GetFileName();

		if (_fileName == _T("Thumbs.db")) continue;

		strSourceFileFullName = finderReview.GetFilePath();

		RenameImageFileName = _fileName;
		RenameImageFileName.Replace("NOINFO", sBarcodeID);

		if (Struct_PreferenceStruct.m_bReviewImageSaveLocal)
			strRenameFileFullName = strReviewImageFolderPath_Local + "\\" + RenameImageFileName;
		else
			strRenameFileFullName = strReviewImageFolderPath_LAS + "\\" + RenameImageFileName;

		rename(strSourceFileFullName, strRenameFileFullName);
	}

	finderReview.Close();
}

void CuScanApp::AddHistoryOnImageFile(int iVisionCamType, CString strRawImageFolderPath_Local, CString strRawImageFolderPath_LAS, int iNoInspectImage, int iMzNo, int iTrayNo, int iModuleNo, CString sBarcodeID)
{
	CString strFileFullName, ImageFileName, SearchImageFileName;
	CString strRenameFileFullName, RenameImageFileName;
	CString strSourceFileFullName;

	HANDLE				hFindFile;
	WIN32_FIND_DATA		FindFileData;

	int k;
	int iRenameRet = 0;

	CString sVisionCamType_Short;
	sVisionCamType_Short = m_ModelDefineInfo.m_strVisionName_Short[iVisionCamType];

	if (Struct_PreferenceStruct.m_bCombineRawImage)
	{
		ImageFileName.Format("Module%d_%s_Image%02d", iModuleNo, sVisionCamType_Short, iNoInspectImage);
		SearchImageFileName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, iNoInspectImage);

		if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
		{
			if (!Struct_PreferenceStruct.m_bRawImageType)
				strFileFullName = strRawImageFolderPath_Local + "\\" + SearchImageFileName + ".bmp";
			else
				strFileFullName = strRawImageFolderPath_Local + "\\" + SearchImageFileName + ".jpg";
		}
		else
		{
			if (!Struct_PreferenceStruct.m_bRawImageType)
				strFileFullName = strRawImageFolderPath_LAS + "\\" + SearchImageFileName + ".bmp";
			else
				strFileFullName = strRawImageFolderPath_LAS + "\\" + SearchImageFileName + ".jpg";
		}
		if ((hFindFile = FindFirstFile(strFileFullName, &FindFileData)) != INVALID_HANDLE_VALUE)
		{
			if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
			{
				strSourceFileFullName.Format("%s\\%s", strRawImageFolderPath_Local, (LPCTSTR)FindFileData.cFileName);
			}
			else
			{
				strSourceFileFullName.Format("%s\\%s", strRawImageFolderPath_LAS, (LPCTSTR)FindFileData.cFileName);
			}

			FindClose(hFindFile);

			RenameImageFileName.Format("%s_%s_%d_%d_%d_%d_%d_%d_%s_%d_%d_%d_%d", ImageFileName, sBarcodeID,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iLoadPicker,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iLoadPickerIndex,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iVisionStageNo,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iVisionStageIndex,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iUnloadPickerNo,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iUnloadPickerIndex,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].sJudge,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iGoodTrayNo,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iGoodTrayPorket,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iNGTrayNo,
				m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iNGTrayPorket);

			if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
			{
				if (!Struct_PreferenceStruct.m_bRawImageType)
					strRenameFileFullName = strRawImageFolderPath_Local + "\\" + RenameImageFileName + ".bmp";
				else
					strRenameFileFullName = strRawImageFolderPath_Local + "\\" + RenameImageFileName + ".jpg";
			}
			else
			{
				if (!Struct_PreferenceStruct.m_bRawImageType)
					strRenameFileFullName = strRawImageFolderPath_LAS + "\\" + RenameImageFileName + ".bmp";
				else
					strRenameFileFullName = strRawImageFolderPath_LAS + "\\" + RenameImageFileName + ".jpg";
			}
			iRenameRet = rename(strSourceFileFullName, strRenameFileFullName);
		}
	}
	else
	{
		for (k = 0; k < iNoInspectImage; k++)
		{
			ImageFileName.Format("Module%d_%s_Image%02d", iModuleNo, sVisionCamType_Short, k + 1);
			SearchImageFileName.Format("Module%d_%s_Image%02d*", iModuleNo, sVisionCamType_Short, k + 1);

			if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
			{
				if (!Struct_PreferenceStruct.m_bRawImageType)
					strFileFullName = strRawImageFolderPath_Local + "\\" + SearchImageFileName + ".bmp";
				else
					strFileFullName = strRawImageFolderPath_Local + "\\" + SearchImageFileName + ".jpg";
			}
			else
			{
				if (!Struct_PreferenceStruct.m_bRawImageType)
					strFileFullName = strRawImageFolderPath_LAS + "\\" + SearchImageFileName + ".bmp";
				else
					strFileFullName = strRawImageFolderPath_LAS + "\\" + SearchImageFileName + ".jpg";
			}
			if ((hFindFile = FindFirstFile(strFileFullName, &FindFileData)) != INVALID_HANDLE_VALUE)
			{
				if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
				{
					strSourceFileFullName.Format("%s\\%s", strRawImageFolderPath_Local, (LPCTSTR)FindFileData.cFileName);
				}
				else
				{
					strSourceFileFullName.Format("%s\\%s", strRawImageFolderPath_LAS, (LPCTSTR)FindFileData.cFileName);
				}

				FindClose(hFindFile);

				RenameImageFileName.Format("%s_%s_%d_%d_%d_%d_%d_%d_%s_%d_%d_%d_%d", ImageFileName, sBarcodeID,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iLoadPicker,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iLoadPickerIndex,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iVisionStageNo,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iVisionStageIndex,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iUnloadPickerNo,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iUnloadPickerIndex,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].sJudge,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iGoodTrayNo,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iGoodTrayPorket,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iNGTrayNo,
					m_StructModuleHistoryInfo[iMzNo - 1][iTrayNo - 1][iModuleNo - 1].iNGTrayPorket);

				if (Struct_PreferenceStruct.m_bRawImageSaveLocal)
				{
					if (!Struct_PreferenceStruct.m_bRawImageType)
						strRenameFileFullName = strRawImageFolderPath_Local + "\\" + RenameImageFileName + ".bmp";
					else
						strRenameFileFullName = strRawImageFolderPath_Local + "\\" + RenameImageFileName + ".jpg";
				}
				else
				{
					if (!Struct_PreferenceStruct.m_bRawImageType)
						strRenameFileFullName = strRawImageFolderPath_LAS + "\\" + RenameImageFileName + ".bmp";
					else
						strRenameFileFullName = strRawImageFolderPath_LAS + "\\" + RenameImageFileName + ".jpg";
				}
				iRenameRet = rename(strSourceFileFullName, strRenameFileFullName);
			}
		}
	}
}


void CuScanApp::SetADJSaveDefectList()
{
	CString strModelFolderPath = GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_ADJ(strModelFolderPath + "\\ADJDefectSetting.ini");
	CString strADJTemp, sADJTemp;

	CString strADJSection = "ADJ_SAVE_DEFECT";

	for (int i = 0; i < MAX_DEFECT_NAME; i++)
	{
		CString str;
		str.Format("%d", i);
		m_strADJSaveDefectList[i] = INI_ADJ.Get_String(strADJSection, str, "");
		if (m_strADJSaveDefectList[i] == "")
			break;
	}
}

CString CuScanApp::GetDefectCode(int iDefectNameIndex, int *piDefectTransferCodeIndex)
{
	CString sDefectCode;

	sDefectCode = "G";
	*piDefectTransferCodeIndex = 0;

	if (iDefectNameIndex >= 0 && iDefectNameIndex < MAX_DEFECT_NAME)
	{
		sDefectCode = m_strDefectCode[iDefectNameIndex];
		*piDefectTransferCodeIndex = Struct_PreferenceStruct.m_iSpecialNGCodeIndex[iDefectNameIndex];
	}

	return sDefectCode;
}

CString CuScanApp::GetDefectCode(CString sDefectName)
{
	CString sDefectCode;

	sDefectCode = "NOUSE";

	for (int i = 0; i < MAX_DEFECT_NAME; i++)
	{
		if (sDefectName == m_strDefectName[i])
		{
			sDefectCode = m_strDefectCode[i];
			break;
		}
	}

	return sDefectCode;
}

CString CuScanApp::GetFaiDefectCode(int iMzNo, int iTrayNo, int iModuleNo, int *piSelectedFaiIndex)
{
	CString sFaiDefectCode;

	sFaiDefectCode = "FAI-NG";
	*piSelectedFaiIndex = -1;

	for (int iFaiParamIndex = 0; iFaiParamIndex < MAX_FAI_PARAMETER; iFaiParamIndex++)
	{
		for (int iFaiItemIndex = 0; iFaiItemIndex < MAX_FAI_ITEM; iFaiItemIndex++)
		{
			if (m_pFAIDataManager.GetMeasure(iMzNo, iTrayNo, iModuleNo, iFaiParamIndex).m_bFAI_ResultNG[iFaiItemIndex][0])
			{
				sFaiDefectCode.Format("FAI-%d", iFaiItemIndex);
				*piSelectedFaiIndex = iFaiItemIndex;
				break;
			}
		}

		if (*piSelectedFaiIndex > 0)
			break;
	}

	return sFaiDefectCode;
}


CString CuScanApp::CheckDefectNameChangeND(CString sInspectionType, CString sDefectName)
{
	CString sFullDefectName;
	sFullDefectName.Format("%s %s", sInspectionType, sDefectName);

	CString sComponentName;

	//if (sDefectName == "Burr" ||
	//	sDefectName == "Material NG" ||
	//	sDefectName == "Coil Contamination" ||
	//	sDefectName == "Coil Damage" ||
	//	sDefectName == "Coil Particle" ||
	//	sDefectName == "Pismo end Overlap" ||
	//	sDefectName == "Ball Dent" ||
	//	sDefectName == "Ball Discoloration" ||
	//	sDefectName == "Ball Foreign Material" ||
	//	sDefectName == "Ball Unfiling")
	//{
	//	AfxExtractSubString(sComponentName, sInspectionType, 0, ' ');
	//	sFullDefectName.Format("%s ND %s", sComponentName, sDefectName);
	//}
	//else if(sDefectName == "Particle")
	//{
	//	AfxExtractSubString(sComponentName, sInspectionType, 0, ' ');

	//	if (sComponentName == "Base" ||
	//		sComponentName == "AF-C" ||
	//		sComponentName == "Y-C")
	//	{
	//		sFullDefectName.Format("%s ND %s", sComponentName, sDefectName);
	//	}
	//}
	//else if (sDefectName == "FAI NG")
	//{
	//	sFullDefectName = "FAI NG";
	//}

	if (sDefectName == "FAI NG")
	{
		sFullDefectName = "FAI NG";
	}

	return sFullDefectName;
}

CString CuScanApp::GetBarcodePrintQualityGrade(int iBarcodeGradeNumber)
{
	CString sBarcodeGrade;

	if (iBarcodeGradeNumber == 4)
		sBarcodeGrade = "A";
	else if (iBarcodeGradeNumber == 3)
		sBarcodeGrade = "B";
	else if (iBarcodeGradeNumber == 2)
		sBarcodeGrade = "C";
	else if (iBarcodeGradeNumber == 1)
		sBarcodeGrade = "D";
	else if (iBarcodeGradeNumber == 0)
		sBarcodeGrade = "F";
	else
		sBarcodeGrade = "F";

	return sBarcodeGrade;
}

void CuScanApp::WriteDummyImage()
{
	try
	{
		int i;
		CString ImageFileName;
		HObject Image;

		GenImageConst(&Image, "byte", DUMMY_IMAGE_SIZE, DUMMY_IMAGE_SIZE);
		for (i = 0; i < MAX_IMAGE_TAB; i++)
		{
			ImageFileName.Format("D:\\Dummy_V1_Image%d", i + 1);
			WriteImage(Image, "bmp", 0, HTuple(ImageFileName));
		}

		GenImageConst(&Image, "byte", DUMMY_IMAGE_SIZE, DUMMY_IMAGE_SIZE);
		for (i = 0; i < MAX_IMAGE_TAB; i++)
		{
			ImageFileName.Format("D:\\Dummy_V2_Image%d", i + 1);
			WriteImage(Image, "bmp", 0, HTuple(ImageFileName));
		}

		GenImageConst(&Image, "byte", DUMMY_IMAGE_SIZE, DUMMY_IMAGE_SIZE);
		for (i = 0; i < MAX_IMAGE_TAB; i++)
		{
			ImageFileName.Format("D:\\Dummy_V3_Image%d", i + 1);
			WriteImage(Image, "bmp", 0, HTuple(ImageFileName));
		}
	}
	catch (HException &except)
	{
		return;
	}
}

#ifdef LGITAI
BOOL CuScanApp::NewSaveVariationFeature(const LGIT::InspectionResultSet& result, CString sModelName)
{
	try
	{
		HObject DefectRegion;
		DefectRegion = AIService::ImageUtile::Mask2HRegion(result.primary_result->mask);

		int iInspectionBufferIdx, iInspectionType;
		iInspectionBufferIdx = result.request->vision_inspection_thread_index;
		iInspectionType = result.request->roi_inspection_type_idx;

		// 1. Run Info (sRunInfo)
		CString sRunInfo;

		/// Date (sDay)
		CString sDay;
		SYSTEMTIME time;
		GetLocalTime(&time);
		sDay.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);

		/// Time (sTime)
		CString sTime;
		sTime.Format("%s %02d:%02d:%02d", sDay, time.wHour, time.wMinute, time.wSecond);

		/// Station (sPCID)
		CString sPCID;
		sPCID = CA2T(result.request->pc_id.c_str());

		/// Machine Code (sEquipNo)
		CString sEquipNo;
		sEquipNo = CA2T(result.request->equip_no.c_str());

		/// Recipe Name (sModelName)
		sModelName;

		/// SW Parameter Version (iSwParamVersion)
		int iSwParamVersion;
		iSwParamVersion = GetSwParamVersion();

		/// HW Parameter Version (iHwParamVersion)
		int iHwParamVersion;
		iHwParamVersion = GetHwParamVersion();

		/// Magazine No (iMzNo)
		int iMzNo;
		iMzNo = result.request->magazine_no;

		/// Lot ID (sLotID)
		CString sLotID;
		sLotID = CA2T(result.request->lot_id.c_str());

		/// Tray No (iTrayNo)
		int iTrayNo;
		iTrayNo = result.request->tray_no;

		/// Module No (iModuleNo)
		int iModuleNo;
		iModuleNo = result.request->module_no;

		/// Barcode (sBarcode)
		CString sBarcode;
		sBarcode = CA2T(result.request->barcode.c_str());

		// 1. Run Info End
		sRunInfo.Format("%s\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%d\t%d\t%s",
			sDay,
			sTime,
			sPCID,
			sEquipNo,
			sModelName,
			iSwParamVersion,
			iHwParamVersion,
			iMzNo,
			sLotID,
			iTrayNo,
			iModuleNo,
			sBarcode);

		// 2. Inspect ROI Info (sInspectROIInfo)
		CString sInspectROIInfo;

		int iVisionCamType;
		iVisionCamType = result.request->vision_cam;

		CString sVisionCamType_Short;
		sVisionCamType_Short = m_ModelDefineInfo.m_strVisionName_Short[iVisionCamType];

		int iROIIndex;
		iROIIndex = result.request->roi_idx;

		int iDualModelData = g_iDualModelData[iMzNo - 1];
		GTRegion* pInspectROIRgn;
		pInspectROIRgn = m_pDualModelDataManager[iDualModelData][iVisionCamType]->m_pInspectionArea->GetChildTRegion(iROIIndex);

		/// Vision (PC No) (sCamName)
		CString sCamName;
		sCamName = CA2T(result.request->vision_name.c_str());

		/// PC No (iPcNo)
		int iPcNo;
		iPcNo = result.request->pc_idx;

		/// Stage No (iStageNo)
		int iStageNo;
		iStageNo = result.request->stage_idx;

		/// Inspection Type (sInspectionType)
		CString sInspectionType;
		sInspectionType = CA2T(result.request->inspectiontype_name.c_str());

		/// Defect Code (sDefectType)
		int iDefectType, iDefectTransCode;
		iDefectType = pInspectROIRgn->m_AlgorithmParam[result.request->inspection_tab_idx].m_iDefectType;
		CString sDefectType;
		sDefectType = GetDefectCode(iDefectType, &iDefectTransCode);

		/// Image No (iNoInspectImage)
		int iNoInspectImage;
		iNoInspectImage = result.request->image_idx;

		/// ROI No (iROINo)
		int iROINo;
		iROINo = result.request->roi_id;

		/// Inspection Tab No (iInspectionTabIndex)
		int iInspectionTabIndex;
		iInspectionTabIndex = result.request->inspection_tab_idx;

		/// ROI Position Left (iTeachROILeft)
		/// ROI Position Top (iTeachROITop)
		/// ROI Position Right (iTeachROIRight)
		/// ROI Position Bottom (iTeachROIBottom)
		POINT LTPoint, RBPoint;
		int iTeachROILeft, iTeachROITop, iTeachROIRight, iTeachROIBottom;
		pInspectROIRgn->GetLTPoint(&LTPoint, m_pCalDataService_N[iVisionCamType]);
		pInspectROIRgn->GetRBPoint(&RBPoint, m_pCalDataService_N[iVisionCamType]);
		iTeachROILeft = LTPoint.x;
		iTeachROITop = LTPoint.y;
		iTeachROIRight = RBPoint.x;
		iTeachROIBottom = RBPoint.y;

		CAlgorithmParam TeachAlgorithmParam;
		TeachAlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iInspectionTabIndex];

		if (!TeachAlgorithmParam.m_bUseImageCompare)
			return FALSE;

		/// Local Align x (iROIDeltaX)
		/// Local Align y (iROIDeltaY)
		/// Local Align Angle (dROIDeltaAngle)
		int iROIDeltaX, iROIDeltaY;
		double dROIDeltaAngle;
		iROIDeltaX = 0;
		iROIDeltaY = 0;
		dROIDeltaAngle = 0.0;
		if (TeachAlgorithmParam.m_bUseImageProcessLocalAlign)
		{
			int iNoInspectROI, iLocalAlignImageNo;
			BOOL bFound = FALSE;
			iNoInspectROI = m_pDualModelDataManager[iDualModelData][iVisionCamType]->m_pInspectionArea->GetChildTRegionCount();

			GTRegion* pLocalAlignROIRgn;
			iLocalAlignImageNo = TeachAlgorithmParam.m_iImageProcessLocalAlignImageIndex + 1;

			for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
			{
				pLocalAlignROIRgn = m_pDualModelDataManager[iDualModelData][iVisionCamType]->m_pInspectionArea->GetChildTRegion(iROIIndex);

				if (pLocalAlignROIRgn == NULL)
					continue;

				if (pLocalAlignROIRgn->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
					continue;

				if (pLocalAlignROIRgn->miTeachImageIndex == iLocalAlignImageNo)
				{
					if (pLocalAlignROIRgn->miLocalAlignID == TeachAlgorithmParam.m_iImageProcessLocalAlignROINo)
					{
						bFound = TRUE;
						break;
					}
				}
			}

			if (bFound)
			{
				if (pLocalAlignROIRgn->m_iLocalAlignDeltaX[iInspectionBufferIdx] != INVALID_ALIGN_RESULT)
				{
					iROIDeltaX = pLocalAlignROIRgn->m_iLocalAlignDeltaX[iInspectionBufferIdx];
					iROIDeltaY = pLocalAlignROIRgn->m_iLocalAlignDeltaY[iInspectionBufferIdx];
					HTuple HAngleDeg;
					TupleDeg(HTuple(pLocalAlignROIRgn->m_dLocalAlignDeltaAngle[iInspectionBufferIdx]), &HAngleDeg);
					dROIDeltaAngle = HAngleDeg[0].D();
				}
			}
		}

		/// Image Filter Type (iImageFilterType)
		/// Image Filter Width (iImageProcessFilterX)
		/// Image Filter Height (iImageProcessFilterY)
		int iImageFilterType, iImageProcessFilterX, iImageProcessFilterY;
		if (TeachAlgorithmParam.m_bUseImageProcessFilter == FALSE) {
			iImageFilterType = FILTER_TYPE_NOT_USED;
			iImageProcessFilterX = 0;
			iImageProcessFilterY = 0;
		}
		else
		{
			iImageFilterType = TeachAlgorithmParam.m_iImageProcessFilterType1;
			iImageProcessFilterX = TeachAlgorithmParam.m_iImageProcessFilterType1X;
			iImageProcessFilterY = TeachAlgorithmParam.m_iImageProcessFilterType1Y;
		}

		/// Image process Channel Type (sProcessChType)
		CString sProcessChType;
		int iProcessChType;
		if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_COLOR)
		{
			sProcessChType = "Color";
			iProcessChType = 3;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_R)
		{
			sProcessChType = "R";
			iProcessChType = 0;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_G)
		{
			sProcessChType = "G";
			iProcessChType = 1;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_B)
		{
			sProcessChType = "B";
			iProcessChType = 2;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_H)
		{
			sProcessChType = "H";
			iProcessChType = 3;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_S)
		{
			sProcessChType = "S";
			iProcessChType = 3;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_I)
		{
			sProcessChType = "I";
			iProcessChType = 3;
		}

		// 2. Inspect ROI Info End
		sInspectROIInfo.Format("%s%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%lf\t%d\t%d\t%d\t%s",
			sCamName, iPcNo,
			iStageNo,
			sInspectionType,
			sDefectType,
			iNoInspectImage,
			iROINo,
			iInspectionTabIndex + 1,
			iTeachROILeft, iTeachROITop, iTeachROIRight, iTeachROIBottom,
			iROIDeltaX, iROIDeltaY, dROIDeltaAngle,
			iImageFilterType, iImageProcessFilterX, iImageProcessFilterY,
			sProcessChType);

		// 3. Variation Parameter Feature (sVariationParameterFeature)
		CString sVariationParameterFeature;

		/// Bright DIFF (iImageCompareBrightAbs)
		/// Bright STDEV (dImageCompareBrightVar)
		/// Dark DIFF (iImageCompareDarkAbs)
		/// Dark STDEV (dImageCompareDarkVar)
		int iImageCompareBrightAbs, iImageCompareDarkAbs;
		double dImageCompareBrightVar, dImageCompareDarkVar;
		iImageCompareBrightAbs = TeachAlgorithmParam.m_iImageCompareBrightAbs;
		dImageCompareBrightVar = TeachAlgorithmParam.m_dImageCompareBrightVar;
		iImageCompareDarkAbs = TeachAlgorithmParam.m_iImageCompareDarkAbs;
		dImageCompareDarkVar = TeachAlgorithmParam.m_dImageCompareDarkVar;

		// 3. Variation Parameter Feature End
		sVariationParameterFeature.Format("%d\t%lf\t%d\t%lf",
			iImageCompareBrightAbs, dImageCompareBrightVar, iImageCompareDarkAbs, dImageCompareDarkVar
		);

		HObject HMasterMeanImage, HMasterVariationImage;
		HObject HVariationInspectRgn;
		if (TeachAlgorithmParam.m_bUseImageCompareWindow)
		{
			CopyImage(pInspectROIRgn->m_HVarMeanImage[iInspectionTabIndex], &HMasterMeanImage);
			CopyImage(pInspectROIRgn->m_HVarStdevImage[iInspectionTabIndex], &HMasterVariationImage);
		}
		else
		{
			GetVariationModel(&HMasterMeanImage, &HMasterVariationImage, pInspectROIRgn->m_HVarModelID[iInspectionTabIndex]);
			ConvertImageType(HMasterVariationImage, &HMasterVariationImage, "byte");
		}

		if (0) // Variation Reference(Mean) Image Save
		{
			CString strFileName;
			strFileName.Format("%s_%s_%s_Tray%d_Module%d_%s%d_Stage%d_Image%d_ROI%d_%s_Tab%d_%s_V3",
				Struct_PreferenceStruct.m_strEquipNo, g_strModelTypeName[GetModelType()],
				sLotID, iTrayNo, iModuleNo,
				sVisionCamType_Short, Struct_PreferenceStruct.m_iPCType + 1, iStageNo,
				iNoInspectImage, iROINo, sInspectionType, iInspectionTabIndex, sBarcode);
			WriteImage(HMasterMeanImage, "jpeg 100", 0, HTuple(m_FileBase.m_strAIFolder_Local[iVisionCamType][iMzNo - 1][iTrayNo - 1] + "\\ROIImage\\" + strFileName));
		}

		// 4. Variation Reference ROI Feature (sReferenceROIVariationFeature)
		CString sReferenceROIVariationFeature;

		/// Ref Bright Avarage (dAvgGVReferenceROI)
		/// Ref Bright Max (dMaxGVReferenceROI)
		/// Ref Bright Min (dMinGVReferenceROI)
		/// Ref Bright STDEV (dStdevGVReferenceROI)
		HTuple HParam1, HParam2, HParam3, HParam4, HParam5;
		double dAvgGVReferenceROI, dMaxGVReferenceROI, dMinGVReferenceROI, dStdevGVReferenceROI;
		HVariationInspectRgn = pInspectROIRgn->m_HVariationInspectImage[iInspectionBufferIdx];
		Intensity(HVariationInspectRgn, HMasterMeanImage, &HParam1, &HParam2);
		MinMaxGray(HVariationInspectRgn, HMasterMeanImage, 0, &HParam3, &HParam4, &HParam5);
		dAvgGVReferenceROI = HParam1[0].D();
		dMaxGVReferenceROI = HParam4[0].D();
		dMinGVReferenceROI = HParam3[0].D();
		dStdevGVReferenceROI = HParam2[0].D();

		// 4. Variation Reference ROI Feature End
		sReferenceROIVariationFeature.Format("%.1lf\t%d\t%d\t%.1lf",
			dAvgGVReferenceROI, (int)dMaxGVReferenceROI, (int)dMinGVReferenceROI, dStdevGVReferenceROI
		);

		// 5. Variation Current ROI Feature (sCurrentROIVariationFeature)
		CString sCurrentROIVariationFeature;

		/// Cur ROI Bright Avarage (dAvgGVCurrentROI)
		/// Cur ROI Bright Max (dMaxGVCurrentROI)
		/// Cur ROI Bright Min (dMinGVCurrentROI)
		/// Cur ROI Bright STDEV (dStdevGVCurrentROI)
		double dAvgGVCurrentROI, dMaxGVCurrentROI, dMinGVCurrentROI, dStdevGVCurrentROI;
		Intensity(HVariationInspectRgn, HVariationInspectRgn, &HParam1, &HParam2);
		MinMaxGray(HVariationInspectRgn, HVariationInspectRgn, 0, &HParam3, &HParam4, &HParam5);
		dAvgGVCurrentROI = HParam1[0].D();
		dMaxGVCurrentROI = HParam4[0].D();
		dMinGVCurrentROI = HParam3[0].D();
		dStdevGVCurrentROI = HParam2[0].D();

		/// Deviation Average GV Reference - Current ROI (dDeviationAvgGVRefCur)
		/// Deviation Max GV Reference - Current ROI (iDeviationMaxGVRefCur)
		/// Deviation Mix GV Reference - Current ROI (iDeviationMinGVRefCur)
		double dDeviationAvgGVRefCur;
		int iDeviationMaxGVRefCur, iDeviationMinGVRefCur;
		dDeviationAvgGVRefCur = dAvgGVReferenceROI - dAvgGVCurrentROI;
		iDeviationMaxGVRefCur = (int)(dMaxGVReferenceROI - dMaxGVCurrentROI);
		iDeviationMinGVRefCur = (int)(dMinGVReferenceROI - dMinGVCurrentROI);

		/// Cur ROI Pixel Count All (lNoPixelAll)
		/// Cur ROI Pixel Count Over Average (lNoPixelOverAvg)
		/// Cur ROI Pixel Count Under Average (lNoPixelUnderAvg)
		/// Cur ROI Pixel Count Over Average Plus Stdev (lNoPixelUnderAvgPlusStdev)
		/// Cur ROI Pixel Count Under Average Minus Stdev (lNoPixelOverAvgMinusStdev)
		HObject HTempBlobRgn;
		long lNoPixelAll, lNoPixelOverAvg, lNoPixelUnderAvg, lNoPixelUnderAvgPlusStdev, lNoPixelOverAvgMinusStdev;
		int iTemp;
		AreaCenter(HVariationInspectRgn, &HParam1, &HParam2, &HParam3);
		lNoPixelAll = HParam1[0].L();
		Threshold(HVariationInspectRgn, &HTempBlobRgn, dAvgGVCurrentROI, 255);
		AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
		lNoPixelOverAvg = HParam1[0].L();
		Threshold(HVariationInspectRgn, &HTempBlobRgn, 0, dAvgGVCurrentROI);
		AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
		lNoPixelUnderAvg = HParam1[0].L();
		iTemp = (int)(dAvgGVCurrentROI + dStdevGVCurrentROI);
		if (iTemp > 255) iTemp = 255;
		Threshold(HVariationInspectRgn, &HTempBlobRgn, 0, iTemp);
		AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
		lNoPixelUnderAvgPlusStdev = HParam1[0].L();
		iTemp = (int)(dAvgGVCurrentROI - dStdevGVCurrentROI);
		if (iTemp < 0) iTemp = 0;
		Threshold(HVariationInspectRgn, &HTempBlobRgn, iTemp, 255);
		AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
		lNoPixelOverAvgMinusStdev = HParam1[0].L();

		if (0) // Variation Current Inspection Image Save
		{
			CString strFileName;
			strFileName.Format("%s_%s_%s_Tray%d_Module%d_%s%d_Stage%d_Image%d_ROI%d_%s_Tab%d_%s_V4",
				Struct_PreferenceStruct.m_strEquipNo, g_strModelTypeName[GetModelType()],
				sLotID, iTrayNo, iModuleNo,
				sVisionCamType_Short, Struct_PreferenceStruct.m_iPCType + 1, iStageNo,
				iNoInspectImage, iROINo, sInspectionType, iInspectionTabIndex, sBarcode);
			WriteImage(HVariationInspectRgn, "jpeg 100", 0, HTuple(m_FileBase.m_strAIFolder_Local[iVisionCamType][iMzNo - 1][iTrayNo - 1] + "\\ROIImage\\" + strFileName));
		}

		// 5. Variation Current ROI Feature End
		sCurrentROIVariationFeature.Format("%.1lf\t%d\t%d\t%.1lf\t%.1lf\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
			dAvgGVCurrentROI, (int)dMaxGVCurrentROI, (int)dMinGVCurrentROI, dStdevGVCurrentROI,
			dDeviationAvgGVRefCur, iDeviationMaxGVRefCur, iDeviationMinGVRefCur,
			lNoPixelAll, lNoPixelOverAvg, lNoPixelUnderAvg, lNoPixelUnderAvgPlusStdev, lNoPixelOverAvgMinusStdev
		);

		// 해당 mask내 모든 blob for 루프

		HObject region = AIService::ImageUtile::Mat2HImage(result.request->roi_image);
		HObject HRunImageRGBI[4];
		Decompose3(region, &(HRunImageRGBI[0]), &(HRunImageRGBI[1]), &(HRunImageRGBI[2]));
		Rgb1ToGray(region, &HRunImageRGBI[3]);
		HVariationInspectRgn = HRunImageRGBI[iProcessChType];
		if (0) // Variation Current ROI Image Save
		{
			CString strFileName;
			strFileName.Format("%s_%s_%s_Tray%d_Module%d_%s%d_Stage%d_Image%d_ROI%d_%s_Tab%d_%s_V5",
				Struct_PreferenceStruct.m_strEquipNo, g_strModelTypeName[GetModelType()],
				sLotID, iTrayNo, iModuleNo,
				sVisionCamType_Short, Struct_PreferenceStruct.m_iPCType + 1, iStageNo,
				iNoInspectImage, iROINo, sInspectionType, iInspectionTabIndex, sBarcode);
			WriteImage(HVariationInspectRgn, "jpeg 100", 0, HTuple(m_FileBase.m_strAIFolder_Local[iVisionCamType][iMzNo - 1][iTrayNo - 1] + "\\ROIImage\\" + strFileName));
		}

		BOOL bUseDefectSizeLimit;
		bUseDefectSizeLimit = FALSE;

		HObject HConnRgn, HSelectedBlob;
		HTuple CountDefect, HSortArea;
		long lNoDefectCount;
		long HSelectedSortArea;
		Connection(DefectRegion, &HConnRgn);
		CountObj(HConnRgn, &CountDefect);
		lNoDefectCount = CountDefect[0].I();

		// Mask내 blob이 엄청 많을 경우를 대비한 처리인데 Tuple Sorting을 하면 blob_id_list를 맞출 수 없음
		/*
		if (lNoDefectCount > Struct_PreferenceStruct.m_iMaxSameDefectShapeFeature)
		{
			bUseDefectSizeLimit = TRUE;

			AreaCenter(HConnRgn, &HParam1, &HParam2, &HParam3);
			TupleSortIndex(HParam1, &HSortArea);
			TupleInverse(HSortArea, &HSortArea);

			lNoDefectCount = Struct_PreferenceStruct.m_iMaxSameDefectShapeFeature;
		}
		*/

		for (int iPrimaryBlobIdx = 0; iPrimaryBlobIdx < lNoDefectCount; iPrimaryBlobIdx++)
		{
			if (bUseDefectSizeLimit)
			{
				HSelectedSortArea = HSortArea[iPrimaryBlobIdx].L();
				SelectObj(HConnRgn, &HSelectedBlob, HSelectedSortArea + 1);
			}
			else
				SelectObj(HConnRgn, &HSelectedBlob, iPrimaryBlobIdx + 1);

			// 6. Blob Variation Feature (sBlobVariationFeature)
			CString sBlobVariationFeature;

			/// Cur Defect Feature Bright Avarage (dAvgGVDefectRegion)
			/// Cur Defect Feature Bright Max (dMaxGVDefectRegion)
			/// Cur Defect Feature Bright Min (dMinGVDefectRegion)
			/// Cur Defect Feature Bright STDEV (dStdevGVDefectRegion)
			double dAvgGVDefectRegion, dMaxGVDefectRegion, dMinGVDefectRegion, dStdevGVDefectRegion;
			Intensity(HSelectedBlob, HVariationInspectRgn, &HParam1, &HParam2);
			MinMaxGray(HSelectedBlob, HVariationInspectRgn, 0, &HParam3, &HParam4, &HParam5);
			dAvgGVDefectRegion = HParam1[0].D();
			dMaxGVDefectRegion = HParam4[0].D();
			dMinGVDefectRegion = HParam3[0].D();
			dStdevGVDefectRegion = HParam2[0].D();

			/// Deviation Average GV Reference - Defect Region (dDeviationAvgGVRefDefect)
			/// Deviation Max GV Reference - Defect Region (iDeviationMaxGVRefDefect)
			/// Deviation Mix GV Reference - Defect Region (iDeviationMinGVRefDefect)
			double dDeviationAvgGVRefDefect;
			int iDeviationMaxGVRefDefect, iDeviationMinGVRefDefect;
			dDeviationAvgGVRefDefect = dAvgGVReferenceROI - dAvgGVDefectRegion;
			iDeviationMaxGVRefDefect = (int)(dMaxGVReferenceROI - dMaxGVDefectRegion);
			iDeviationMinGVRefDefect = (int)(dMinGVReferenceROI - dMinGVDefectRegion);

			/// Cur Defect Feature Pixel Count All (lNoPixelAll)
			/// Cur Defect Feature Pixel Count Over Average (lNoPixelOverAvg)
			/// Cur Defect Feature Pixel Count Under Average (lNoPixelUnderAvg)
			/// Cur Defect Feature Pixel Count Over Average Plus Stdev (lNoPixelUnderAvgPlusStdev)
			/// Cur Defect Feature Pixel Count Under Average Minus Stdev (lNoPixelOverAvgMinusStdev)
			HObject HImageReduced;
			ReduceDomain(HVariationInspectRgn, HSelectedBlob, &HImageReduced);
			AreaCenter(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			lNoPixelAll = HParam1[0].L();
			Threshold(HImageReduced, &HTempBlobRgn, dAvgGVDefectRegion, 255);
			AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
			lNoPixelOverAvg = HParam1[0].L();
			Threshold(HImageReduced, &HTempBlobRgn, 0, dAvgGVDefectRegion);
			AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
			lNoPixelUnderAvg = HParam1[0].L();
			iTemp = (int)(dAvgGVDefectRegion + dStdevGVDefectRegion);
			if (iTemp > 255) iTemp = 255;
			Threshold(HImageReduced, &HTempBlobRgn, 0, iTemp);
			AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
			lNoPixelUnderAvgPlusStdev = HParam1[0].L();
			iTemp = (int)(dAvgGVDefectRegion - dStdevGVDefectRegion);
			if (iTemp < 0) iTemp = 0;
			Threshold(HImageReduced, &HTempBlobRgn, iTemp, 255);
			AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
			lNoPixelOverAvgMinusStdev = HParam1[0].L();

			// 6. Blob Variation Feature End
			sBlobVariationFeature.Format("%.1lf\t%d\t%d\t%.1lf\t%.1lf\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
				dAvgGVDefectRegion, (int)dMaxGVDefectRegion, (int)dMinGVDefectRegion, dStdevGVDefectRegion,
				dDeviationAvgGVRefDefect, iDeviationMaxGVRefDefect, iDeviationMinGVRefDefect,
				lNoPixelAll, lNoPixelOverAvg, lNoPixelUnderAvg, lNoPixelUnderAvgPlusStdev, lNoPixelOverAvgMinusStdev
			);

			// 7-1. Blob Defect Shape Fueture Part.1 (sBlobDefectShapeFeature1)
			CString sBlobDefectShapeFeature1;

			/// Area (area)
			/// Row (row)
			/// Column (column)
			long area;
			double row, column;
			AreaCenter(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			area = HParam1[0].L();
			row = HParam2[0].D();
			column = HParam3[0].D();

			/// Width (width)
			/// Height (height)
			/// Row1 (row1)
			/// Column1 (column1)
			/// Row2 (row2)
			/// Column2 (column2)
			long width, height, row1, column1, row2, column2;
			SmallestRectangle1(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			width = HParam4[0].L() - HParam2[0].L() + 1;
			height = HParam3[0].L() - HParam1[0].L() + 1;
			row1 = HParam1[0].L();
			column1 = HParam2[0].L();
			row2 = HParam3[0].L();
			column2 = HParam4[0].L();

			/// Length / Width Ratio (dLengthWidthRatio)
			double dBlobLength, dCircleRadius, dLengthWidthRatio;
			DiameterRegion(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			dBlobLength = HParam5.D();
			InnerCircle(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dCircleRadius = HParam3.D() * 2.0;
			if (dCircleRadius <= 0)
				dCircleRadius = 1;
			dLengthWidthRatio = dBlobLength / dCircleRadius;

			/// Major / Minor Axis Ratio (dMajorMinorAxisRatio)
			double dRa, dRb, dMajorMinorAxisRatio;
			EllipticAxis(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dRa = HParam1[0].D();
			dRb = HParam2[0].D();
			if (dRb != 0)
				dMajorMinorAxisRatio = dRa / dRb;
			else
				dMajorMinorAxisRatio = 1.0;

			/// Defect / ROI area Ratio (dAreaRatio)
			long area2;
			double dAreaRatio;
			AreaCenter(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			area = HParam1.L();
			AreaCenter(HVariationInspectRgn, &HParam1, &HParam2, &HParam3);
			area2 = HParam1.L();
			if (area2 != 0)
				dAreaRatio = (double)area / (double)area2;
			else
				dAreaRatio = 1;

			/// Contrast (dContrast)
			HObject HFillupRgn;
			double dContrast;
			FillUp(HSelectedBlob, &HFillupRgn);
			DilationCircle(HFillupRgn, &HFillupRgn, 3.5);
			Intensity(HFillupRgn, HVariationInspectRgn, &HParam1, &HParam2);
			dContrast = HParam1.D();

			/// Distance from ROI to Defect (dROIDefectDistance)
			double dROIDefectDistance;
			DistanceRrMin(HVariationInspectRgn, HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			dROIDefectDistance = HParam1.D();

			/// Distance from ROI Left to Defect Left (iLeftDistance)
			/// Distance from ROI Top to Defect Top (iTopDistance)
			/// Distance from ROI Right to Defect Right (iRightDistance)
			/// Distance from ROI Bottom to Defect Bottom (iBottomDistance)
			long lROILTPointY, lROILTPointX, lROIRBPointY, lROIRBPointX;
			long lDefectLTPointY, lDefectLTPointX, lDefectRBPointY, lDefectRBPointX;
			int iLeftDistance, iTopDistance, iRightDistance, iBottomDistance;
			SmallestRectangle1(HVariationInspectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
			lROILTPointY = HParam1.L();
			lROILTPointX = HParam2.L();
			lROIRBPointY = HParam3.L();
			lROIRBPointX = HParam4.L();
			SmallestRectangle1(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			lDefectLTPointY = HParam1.L();
			lDefectLTPointX = HParam2.L();
			lDefectRBPointY = HParam3.L();
			lDefectRBPointX = HParam4.L();
			iLeftDistance = lDefectLTPointX - lROILTPointX - 1;
			if (iLeftDistance < 0) iLeftDistance = 0;
			iTopDistance = lDefectLTPointY - lROILTPointY - 1;
			if (iTopDistance < 0) iTopDistance = 0;
			iRightDistance = lROIRBPointX - lDefectRBPointX - 1;
			if (iRightDistance < 0) iRightDistance = 0;
			iBottomDistance = lROIRBPointY - lDefectRBPointY - 1;
			if (iBottomDistance < 0) iBottomDistance = 0;

			/// Point Count Connected to ROI (lNoPointConnectedtoROI)
			HObject HBoundaryRgn, HIntersectRgn;
			long lNoPointConnectedtoROI;
			Boundary(HVariationInspectRgn, &HBoundaryRgn, "inner");
			Intersection(HBoundaryRgn, HSelectedBlob, &HIntersectRgn);
			Connection(HIntersectRgn, &HIntersectRgn);
			CountObj(HIntersectRgn, &HParam1);
			lNoPointConnectedtoROI = HParam1.L();

			/// Boundary Gradient (dGradient)
			HObject HGradImage;
			double dGradient;
			SobelAmp(HVariationInspectRgn, &HGradImage, "sum_abs", 3);
			Boundary(HSelectedBlob, &HBoundaryRgn, "inner");
			Intensity(HBoundaryRgn, HGradImage, &HParam1, &HParam2);
			dGradient = HParam1.D();

			/// Distance x from ROI Center to Defect Center (dDistanceX)
			/// Distance y from ROI Center to Defect Center (dDistanceY))
			double dShiftInspectCenterX, dShiftInspectCenterY, dShiftDefectCenterX, dShiftDefectCenterY;
			double dDistanceX, dDistanceY;
			AreaCenter(HVariationInspectRgn, &HParam1, &HParam2, &HParam3);
			dShiftInspectCenterY = HParam2.D();
			dShiftInspectCenterX = HParam3.D();
			AreaCenter(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dShiftDefectCenterY = HParam2.D();
			dShiftDefectCenterX = HParam3.D();
			dDistanceX = fabs(dShiftInspectCenterX - dShiftDefectCenterX);
			dDistanceY = fabs(dShiftInspectCenterY - dShiftDefectCenterY);

			// 7-1. Blob Defect Shape Fueture Part.1 End
			sBlobDefectShapeFeature1.Format("%d\t%.3lf\t%.3lf\t%d\t%d\t%d\t%d\t%d\t%d\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%d\t%d\t%d\t%d\t%d\t%.3lf\t%.3lf\t%.3lf",
				area, row, column,
				width, height, row1, column1, row2, column2,
				dLengthWidthRatio,
				dMajorMinorAxisRatio,
				dAreaRatio,
				dContrast,
				dROIDefectDistance,
				iLeftDistance, iTopDistance, iRightDistance, iBottomDistance,
				lNoPointConnectedtoROI,
				dGradient,
				dDistanceX,
				dDistanceY
			);

			// 7-2. Blob Defect Shape Fueture Part.2 (sBlobDefectShapeFeature2)
			CString sBlobDefectShapeFeature2;

			/// Circularity (dCircularity)
			double dCircularity;
			Circularity(HSelectedBlob, &HParam1);
			dCircularity = HParam1[0].D();

			/// Compactness (dCompactness)
			double dCompactness;
			Compactness(HSelectedBlob, &HParam1);
			dCompactness = HParam1[0].D();

			/// Contlength (dContlength)
			double dContlength;
			Contlength(HSelectedBlob, &HParam1);
			dContlength = HParam1[0].D();

			/// Convexity (dConvexity)
			double dConvexity;
			Convexity(HSelectedBlob, &HParam1);
			dConvexity = HParam1[0].D();

			/// Rectangularity (dRectangularity)
			double dRectangularity;
			Rectangularity(HSelectedBlob, &HParam1);
			dRectangularity = HParam1[0].D();

			/// EclipticAxis ra (dRa)
			/// EclipticAxis rb (dRb)
			/// EclipticAxis phi (dPhi)
			double dPhi;
			EllipticAxis(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dRa = HParam1[0].D();
			dRb = HParam2[0].D();
			dPhi = HParam3[0].D();

			/// Eccentricity anisometry (dAnisometry)
			/// Eccentricity bulkiness (dBulkiness)
			/// Eccentricity struct factor (dStructfactor)
			double dAnisometry, dBulkiness, dStructfactor;
			Eccentricity(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dAnisometry = HParam1[0].D();
			dBulkiness = HParam2[0].D();
			dStructfactor = HParam3[0].D();

			/// Outer radius (dOuterRadius)
			double dOuterRadius;
			SmallestCircle(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dOuterRadius = HParam3[0].D();

			/// Inner radius (dInnerRadius)
			double dInnerRadius;
			InnerCircle(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dInnerRadius = HParam3[0].D();

			/// Inner width (dInnerWidth)
			/// Inner Height (dInnerHeight)
			double dInnerWidth, dInnerHeight;
			InnerRectangle1(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dInnerWidth = HParam4[0].L() - HParam2[0].L() + 1;
			dInnerHeight = HParam3[0].L() - HParam1[0].L() + 1;

			// 7-2. Blob Defect Shape Fueture Part.2 End
			sBlobDefectShapeFeature2.Format("%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf",
				dCircularity,
				dCompactness,
				dContlength,
				dConvexity,
				dRectangularity,
				dRa, dRb, dPhi,
				dAnisometry, dBulkiness, dStructfactor,
				dOuterRadius,
				dInnerRadius,
				dInnerWidth, dInnerHeight
			);

			// 7-3. Blob Defect Shape Fueture Part.3 (sBlobDefectShapeFeature3)
			CString sBlobDefectShapeFeature3;

			/// Roundness Distance Mean (dDistanceMean)
			/// Roundness Distance Deviation (dDistanceDeviation)
			/// Roundness (dRoundness)
			/// Roundness Number of Sides (dNumSides)
			double dDistanceMean, dDistanceDeviation, dRoundness, dNumSides;
			Roundness(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dDistanceMean = HParam1[0].D();
			dDistanceDeviation = HParam2[0].D();
			dRoundness = HParam3[0].D();
			dNumSides = HParam4[0].D();

			/// Connect Number (dConnectNum)
			/// Holes Number (dHolesNum)
			double dConnectNum, dHolesNum;
			ConnectAndHoles(HSelectedBlob, &HParam1, &HParam2);
			dConnectNum = HParam1[0].L();
			dHolesNum = HParam2[0].L();

			/// Area Holes (dAreaHoles)
			double dAreaHoles;
			AreaHoles(HSelectedBlob, &HParam1);
			dAreaHoles = HParam1[0].L();

			/// Max Diameter (dMaxDiameter)
			double dMaxDiameter;
			DiameterRegion(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			dMaxDiameter = HParam5[0].D();

			/// Orientation (dOrientation)
			double dOrientation;
			OrientationRegion(HSelectedBlob, &HParam1);
			dOrientation = HParam1[0].D();

			/// Number of Euler (dEulerNumber)
			double dEulerNumber;
			EulerNumber(HSelectedBlob, &HParam1);
			dEulerNumber = HParam1[0].L();

			/// Smallest Rectangle Phi (dRect2Phi)
			/// Smallest Rectangle Length1 (dRect2Lengn1)
			/// Smallest Rectangle Length2 (dRect2Lengn2)
			double dRect2Phi, dRect2Lengn1, dRect2Lengn2;
			SmallestRectangle2(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			dRect2Phi = HParam3[0].D();
			dRect2Lengn1 = HParam4[0].D();
			dRect2Lengn2 = HParam5[0].D();

			// 7-3. Blob Defect Shape Fueture Part.3 End
			sBlobDefectShapeFeature3.Format("%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf",
				dDistanceMean, dDistanceDeviation, dRoundness, dNumSides,
				dConnectNum, dHolesNum,
				dAreaHoles,
				dMaxDiameter,
				dOrientation,
				dEulerNumber,
				dRect2Phi,
				dRect2Lengn1,
				dRect2Lengn2
			);

			// 7-4. Blob Defect Shape Fueture Part.4 (sBlobDefectShapeFeature4)
			CString sBlobDefectShapeFeature4;

			/// Moments m11 (dMomentsM11)
			/// Moments m20 (dMomentsM20)
			/// Moments m02 (dMomentsM02)
			/// Moments ia (dMomentsIa)
			/// Moments ib (dMomentsIb)
			double dMomentsM11, dMomentsM20, dMomentsM02, dMomentsIa, dMomentsIb;
			MomentsRegion2nd(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			dMomentsM11 = HParam1[0].D();
			dMomentsM20 = HParam2[0].D();
			dMomentsM02 = HParam3[0].D();
			dMomentsIa = HParam4[0].D();
			dMomentsIb = HParam5[0].D();

			/// Moments m11 Invar (dMomentsM11Invar)
			/// Moments m20 Invar (dMomentsM20Invar)
			/// Moments m02 Invar (dMomentsM02Invar)
			double dMomentsM11Invar, dMomentsM20Invar, dMomentsM02Invar;
			MomentsRegion2ndInvar(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dMomentsM11Invar = HParam1[0].D();
			dMomentsM20Invar = HParam2[0].D();
			dMomentsM02Invar = HParam3[0].D();

			/// Moments phi1 (dMomentsPhi1)
			/// Moments phi2 (dMomentsPhi2)
			double dMomentsPhi1, dMomentsPhi2;
			MomentsRegion2ndRelInvar(HSelectedBlob, &HParam1, &HParam2);
			dMomentsPhi1 = HParam1[0].D();
			dMomentsPhi2 = HParam2[0].D();

			///	Moments m21 (dMomentsM21)
			/// Moments m12 (dMomentsM12)
			/// Moments m03 (dMomentsM03)
			/// Moments m30 (dMomentsM30)
			double dMomentsM21, dMomentsM12, dMomentsM03, dMomentsM30;
			MomentsRegion3rd(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dMomentsM21 = HParam1[0].D();
			dMomentsM12 = HParam2[0].D();
			dMomentsM03 = HParam3[0].D();
			dMomentsM30 = HParam4[0].D();

			/// Moments m21 Invar (dMomentsM21Invar)
			/// Moments m12 Invar (dMomentsM12Invar)
			/// Moments m03 Invar (dMomentsM03Invar)
			/// Moments m30 Invar (dMomentsM30Invar)
			double dMomentsM21Invar, dMomentsM12Invar, dMomentsM03Invar, dMomentsM30Invar;
			MomentsRegion3rdInvar(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dMomentsM21Invar = HParam1[0].D();
			dMomentsM12Invar = HParam2[0].D();
			dMomentsM03Invar = HParam3[0].D();
			dMomentsM30Invar = HParam4[0].D();

			/// Moments i1 (dMomentsI1)
			/// Moments i2 (dMomentsI2)
			/// Moments i3 (dMomentsI3)
			/// Moments i4 (dMomentsI4)
			double dMomentsI1, dMomentsI2, dMomentsI3, dMomentsI4;
			MomentsRegionCentral(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dMomentsI1 = HParam1[0].D();
			dMomentsI2 = HParam2[0].D();
			dMomentsI3 = HParam3[0].D();
			dMomentsI4 = HParam4[0].D();

			/// Moments psi1 (dMomentsPsi1)
			/// Moments psi2 (dMomentsPsi2)
			/// Moments psi3 (dMomentsPsi3)
			/// Moments psi4 (dMomentsPsi4)
			double dMomentsPsi1, dMomentsPsi2, dMomentsPsi3, dMomentsPsi4;
			MomentsRegionCentralInvar(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dMomentsPsi1 = HParam1[0].D();
			dMomentsPsi2 = HParam2[0].D();
			dMomentsPsi3 = HParam3[0].D();
			dMomentsPsi4 = HParam4[0].D();

			// 7-4. Blob Defect Shape Fueture Part.4 End
			sBlobDefectShapeFeature4.Format("%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf",
				dMomentsM11, dMomentsM20, dMomentsM02, dMomentsIa, dMomentsIb,
				dMomentsM11Invar, dMomentsM20Invar, dMomentsM02Invar,
				dMomentsPhi1, dMomentsPhi2,
				dMomentsM21, dMomentsM12, dMomentsM03, dMomentsM30,
				dMomentsM21Invar, dMomentsM12Invar, dMomentsM03Invar, dMomentsM30Invar,
				dMomentsI1, dMomentsI2, dMomentsI3, dMomentsI4,
				dMomentsPsi1, dMomentsPsi2, dMomentsPsi3, dMomentsPsi4
			);

			// 8-1. Defect Condition Parameter Part.1 (sDefectConditioinParameter1)
			CString sDefectConditioinParameter1;

			/// Used Area (iUsedArea)
			/// Area Min (iAreaMin)
			/// Area Max (iAreaMax)
			int iUsedArea, iAreaMin, iAreaMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionArea)
				iUsedArea = 1;
			else
				iUsedArea = 0;
			iAreaMin = TeachAlgorithmParam.m_iDefectConditionAreaMin;
			iAreaMax = TeachAlgorithmParam.m_iDefectConditionAreaMax;

			/// Used Length (iUsedLength)
			/// Length Min (iLengthMin)
			/// Length Max (iLengthMax)
			int iUsedLength, iLengthMin, iLengthMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionLength)
				iUsedLength = 1;
			else
				iUsedLength = 0;
			iLengthMin = TeachAlgorithmParam.m_iDefectConditionLengthMin;
			iLengthMax = TeachAlgorithmParam.m_iDefectConditionLengthMax;

			/// Used Width (iUsedWidth)
			/// Width Min (iWidthMin)
			/// Width Max (iWidthMax)
			int iUsedWidth, iWidthMin, iWidthMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionWidth)
				iUsedWidth = 1;
			else
				iUsedWidth = 0;
			iWidthMin = TeachAlgorithmParam.m_iDefectConditionWidthMin;
			iWidthMax = TeachAlgorithmParam.m_iDefectConditionWidthMax;

			/// Used x Length (iUsedXLength)
			/// x Length Min (iXLengthMin)
			/// x Length Max (iXLengthMax)
			int  iUsedXLength, iXLengthMin, iXLengthMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionXLength)
				iUsedXLength = 1;
			else
				iUsedXLength = 0;
			iXLengthMin = TeachAlgorithmParam.m_iDefectConditionXLengthMin;
			iXLengthMax = TeachAlgorithmParam.m_iDefectConditionXLengthMax;

			/// Used y Length (iUsedYLength)
			/// y Length Min (iYLengthMin)
			/// y Length Max (iYLengthMax)
			int iUsedYLength, iYLengthMin, iYLengthMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionYLength)
				iUsedYLength = 1;
			else
				iUsedYLength = 0;
			iYLengthMin = TeachAlgorithmParam.m_iDefectConditionYLengthMin;
			iYLengthMax = TeachAlgorithmParam.m_iDefectConditionYLengthMax;

			/// Used Orthogonal Length (iUsedOrthoLength)
			/// Reference Orthogonal Length (iOrthoLengthRefSide)
			int iUsedOrthoLength, iOrthoLengthRefSide;
			if (TeachAlgorithmParam.m_bDefectConditionOrthoLength)
				iUsedOrthoLength = 1;
			else
				iUsedOrthoLength = 0;
			iOrthoLengthRefSide = TeachAlgorithmParam.m_iOrthoLengthRefSide;

			/// Used GV Mean (iUsedGVMean)
			/// GV Mean Min (iGVMeanMin)
			/// GV Mean Max (iGVMeanMax)
			int iUsedGVMean, iGVMeanMin, iGVMeanMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionMean)
				iUsedGVMean = 1;
			else
				iUsedGVMean = 0;
			iGVMeanMin = TeachAlgorithmParam.m_iDefectConditionMeanMin;
			iGVMeanMax = TeachAlgorithmParam.m_iDefectConditionMeanMax;

			/// Used GV Stdev (iUsedGVStdev)
			/// GV Stdev Min (dGVStdevMin)
			/// GV Stdev Max (dGVStdevMax)
			int iUsedGVStdev;
			double dGVStdevMin, dGVStdevMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionStdev)
				iUsedGVStdev = 1;
			else
				iUsedGVStdev = 0;
			dGVStdevMin = TeachAlgorithmParam.m_dDefectConditionStdevMin;
			dGVStdevMax = TeachAlgorithmParam.m_dDefectConditionStdevMax;

			/// Used Length / Width Ratio (iUsedLengthWidthRatio)
			/// Length / Width Ratio Min (dLengthWidthRatioMin)
			/// Length / Width Ratio Max (dLengthWidthRatioMax)
			int iUsedLengthWidthRatio;
			double dLengthWidthRatioMin, dLengthWidthRatioMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionAnisometry)
				iUsedLengthWidthRatio = 1;
			else
				iUsedLengthWidthRatio = 0;
			dLengthWidthRatioMin = TeachAlgorithmParam.m_dDefectConditionAnisometryMin;
			dLengthWidthRatioMax = TeachAlgorithmParam.m_dDefectConditionAnisometryMax;

			// 8-1. Defect Condition Parameter Part.1 End
			sDefectConditioinParameter1.Format("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%.1lf\t%.1lf\t%d\t%.1lf\t%.1lf",
				iUsedArea, iAreaMin, iAreaMax,
				iUsedLength, iLengthMin, iLengthMax,
				iUsedWidth, iWidthMin, iWidthMax,
				iUsedXLength, iXLengthMin, iXLengthMax,
				iUsedYLength, iYLengthMin, iYLengthMax,
				iUsedOrthoLength, iOrthoLengthRefSide,
				iUsedGVMean, iGVMeanMin, iGVMeanMax,
				iUsedGVStdev, dGVStdevMin, dGVStdevMax,
				iUsedLengthWidthRatio, dLengthWidthRatioMin, dLengthWidthRatioMax
			);

			// 8-2. Defect Condition Parameter Part.2 (sDefectConditioinParameter2)
			CString sDefectConditioinParameter2;

			/// Used Circlularity (iUsedCircularity)
			/// Circlularity Min (dCircularityMin)
			/// Circlularity Max (dCircularityMax)
			int iUsedCircularity;
			double dCircularityMin, dCircularityMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionCircularity)
				iUsedCircularity = 1;
			else
				iUsedCircularity = 0;
			dCircularityMin = TeachAlgorithmParam.m_dDefectConditionCircularityMin;
			dCircularityMax = TeachAlgorithmParam.m_dDefectConditionCircularityMax;

			/// Used Rectangularity (iUsedRectangularity)
			/// Rectangularity Min (dRectangularityMin)
			/// Rectangularity Max (dRectangularityMax)
			int iUsedRectangularity;
			double dRectangularityMin, dRectangularityMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionRectangularity)
				iUsedRectangularity = 1;
			else
				iUsedRectangularity = 0;
			dRectangularityMin = TeachAlgorithmParam.m_dDefectConditionRectangularityMin;
			dRectangularityMax = TeachAlgorithmParam.m_dDefectConditionRectangularityMax;

			/// Used Convexity (iUsedConvexity)
			/// Convexity Min (dConvexityMin)
			/// Convexity Max (dConvexityMax)
			int iUsedConvexity;
			double dConvexityMin, dConvexityMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionConvexity)
				iUsedConvexity = 1;
			else
				iUsedConvexity = 0;
			dConvexityMin = TeachAlgorithmParam.m_dDefectConditionConvexityMin;
			dConvexityMax = TeachAlgorithmParam.m_dDefectConditionConvexityMax;

			/// Used Major / Minor Axis Ratio (iUsedMajorMinorAxisRatio)
			/// Major / Minor Axis Ratio Min (dMajorMinorAxisRatioMin)
			/// Major / Minor Axis Ratio Max (dMajorMinorAxisRatioMax)
			int iUsedMajorMinorAxisRatio;
			double dMajorMinorAxisRatioMin, dMajorMinorAxisRatioMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionEllipseRatio)
				iUsedMajorMinorAxisRatio = 1;
			else
				iUsedMajorMinorAxisRatio = 0;
			dMajorMinorAxisRatioMin = TeachAlgorithmParam.m_dDefectConditionEllipseRatioMin;
			dMajorMinorAxisRatioMax = TeachAlgorithmParam.m_dDefectConditionEllipseRatioMax;

			/// Used Area Ratio (iUsedAreaRatio)
			/// Area Ratio Min (iAreaRatioMin)
			/// Area Ratio Max (iAreaRatioMax)
			int iUsedAreaRatio, iAreaRatioMin, iAreaRatioMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionAreaRatio)
				iUsedAreaRatio = 1;
			else
				iUsedAreaRatio = 0;
			iAreaRatioMin = TeachAlgorithmParam.m_iDefectConditionAreaRatioMin;
			iAreaRatioMax = TeachAlgorithmParam.m_iDefectConditionAreaRatioMax;

			/// Used Contrast (iUsedContrast)
			/// Contrast Min (iContrastMin)
			/// Contrast Max (iContrastMax)
			int iUsedContrast, iContrastMin, iContrastMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionContrast)
				iUsedContrast = 1;
			else
				iUsedContrast = 0;
			iContrastMin = TeachAlgorithmParam.m_iDefectConditionContrastMin;
			iContrastMax = TeachAlgorithmParam.m_iDefectConditionContrastMax;

			// 8-2. Defect Condition Parameter Part.2 End
			sDefectConditioinParameter2.Format("%d\t%.1lf\t%.1lf\t%d\t%.1lf\t%.1lf\t%d\t%.1lf\t%.1lf\t%d\t%.1lf\t%.1lf\t%d\t%d\t%d\t%d\t%d\t%d",
				iUsedCircularity, dCircularityMin, dCircularityMax,
				iUsedRectangularity, dRectangularityMin, dRectangularityMax,
				iUsedConvexity, dConvexityMin, dConvexityMax,
				iUsedMajorMinorAxisRatio, dMajorMinorAxisRatioMin, dMajorMinorAxisRatioMax,
				iUsedAreaRatio, iAreaRatioMin, iAreaRatioMax,
				iUsedContrast, iContrastMin, iContrastMax
			);

			// 8-3. Defect Condition Parameter Part.3 (sDefectConditioinParameter3)
			CString sDefectConditioinParameter3;

			/// Used Distance from ROI outer to Defect (iUsedROIDefectDistance)
			/// Distance from ROI outer to Defect (iROIDefectDistance)
			int iUsedROIDefectDistance, iROIDefectDistance;
			if (TeachAlgorithmParam.m_bUseDefectConditionOuterDist)
				iUsedROIDefectDistance = 1;
			else
				iUsedROIDefectDistance = 0;
			iROIDefectDistance = TeachAlgorithmParam.m_iDefectConditionOuterDist;

			/// Used Distance from ROI inner to Defect (iUsedROIDefectInnerDistance)
			/// Distance from ROI inner to Defect (iROIDefectInnerDistance)
			int iUsedROIDefectInnerDistance, iROIDefectInnerDistance;
			if (TeachAlgorithmParam.m_bUseDefectConditionInnerDist)
				iUsedROIDefectInnerDistance = 1;
			else
				iUsedROIDefectInnerDistance = 0;
			iROIDefectInnerDistance = TeachAlgorithmParam.m_iDefectConditionInnerDist;

			/// Used Number of blobs (iUsedNoBlobs)
			/// Number of blobs Min (iNoBlobsMin)
			/// Number of blobs Max (iNoBlobsMax)
			int iUsedNoBlobs, iNoBlobsMin, iNoBlobsMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionBlob)
				iUsedNoBlobs = 1;
			else
				iUsedNoBlobs = 0;
			iNoBlobsMin = TeachAlgorithmParam.m_iDefectConditionBlobMin;
			iNoBlobsMax = TeachAlgorithmParam.m_iDefectConditionBlobMax;

			/// Used Connected to ROI Each Position (iUsedConnectedToROI)
			int iUsedConnectedToROI;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryConnection)
				iUsedConnectedToROI = 1;
			else
				iUsedConnectedToROI = 0;

			/// Used Connected to ROI Top (iUsedConnectedToROITop)
			/// Connected to ROI Top Margin (iConnectedToROITopMargin)
			int iUsedConnectedToROITop, iConnectedToROITopMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginTop)
				iUsedConnectedToROITop = 1;
			else
				iUsedConnectedToROITop = 0;
			iConnectedToROITopMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginTop;

			/// Used Connected to ROI Bottom (iUsedConnectedToROIBottom)
			/// Connected to ROI Bottom Margin (iConnectedToROIBottomMargin)
			int iUsedConnectedToROIBottom, iConnectedToROIBottomMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginBottom)
				iUsedConnectedToROIBottom = 1;
			else
				iUsedConnectedToROIBottom = 0;
			iConnectedToROIBottomMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginBottom;

			/// Used Connected to ROI Horizontal Center (iUsedConnectedToROIHorizontalCenter)
			/// Connected to ROI Horizontal Center Margin (iConnectedToROIHorizontalCenterMargin)
			int iUsedConnectedToROIHorizontalCenter, iConnectedToROIHorizontalCenterMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginHCenter)
				iUsedConnectedToROIHorizontalCenter = 1;
			else
				iUsedConnectedToROIHorizontalCenter = 0;
			iConnectedToROIHorizontalCenterMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginHCenter;

			/// Used Connected to ROI Left (iUsedConnectedToROILeft)
			/// Connected to ROI Left Margin (iConnectedToROILeftMargin)
			int iUsedConnectedToROILeft, iConnectedToROILeftMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginLeft)
				iUsedConnectedToROILeft = 1;
			else
				iUsedConnectedToROILeft = 0;
			iConnectedToROILeftMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginLeft;

			/// Used Connected to ROI Right (iUsedConnectedToROIRight)
			///	Connected to ROI Right Margin (iConnectedToROIRightMargin)
			int iUsedConnectedToROIRight, iConnectedToROIRightMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginRight)
				iUsedConnectedToROIRight = 1;
			else
				iUsedConnectedToROIRight = 0;
			iConnectedToROIRightMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginRight;

			/// Used Connected to ROI Vertical Center (iUsedConnectedToROIVerticalCenter)
			/// Connected to ROI Vertical Center Margin (iConnectedToROIVerticalCenterMargin)
			int iUsedConnectedToROIVerticalCenter, iConnectedToROIVerticalCenterMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginVCenter)
				iUsedConnectedToROIVerticalCenter = 1;
			else
				iUsedConnectedToROIVerticalCenter = 0;
			iConnectedToROIVerticalCenterMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginVCenter;

			/// Used Connected to ROI Any Position (iUsedConnectedToROIAnyPosition)
			/// Connected to ROI Any Position Margin (iConnectedToROIAnyPositionMargin)
			int iUsedConnectedToROIAnyPosition, iConnectedToROIAnyPositionMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryConnectionAny)
				iUsedConnectedToROIAnyPosition = 1;
			else
				iUsedConnectedToROIAnyPosition = 0;
			iConnectedToROIAnyPositionMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginAny;

			/// Used Connected to ROI 2 Sides (iUsedConnectedToROI2Sides)
			int iUsedConnectedToROI2Sides;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryConnection2Side)
				iUsedConnectedToROI2Sides = 1;
			else
				iUsedConnectedToROI2Sides = 0;

			/// Used Connected to ROI Inner and Outer (iUsedConnectedToROIInnerOuter)
			int iUsedConnectedToROIInnerOuter;
			if (TeachAlgorithmParam.m_bDefectConditionInnerOuterBoundaryConnection)
				iUsedConnectedToROIInnerOuter = 1;
			else
				iUsedConnectedToROIInnerOuter = 0;

			// 8-3. Defect Condition Parameter Part.3 End
			sDefectConditioinParameter3.Format("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
				iUsedROIDefectDistance, iROIDefectDistance,
				iUsedROIDefectInnerDistance, iROIDefectInnerDistance,
				iUsedNoBlobs, iNoBlobsMin, iNoBlobsMax,
				iUsedConnectedToROI,
				iUsedConnectedToROITop, iConnectedToROITopMargin,
				iUsedConnectedToROIBottom, iConnectedToROIBottomMargin,
				iUsedConnectedToROIHorizontalCenter, iConnectedToROIHorizontalCenterMargin,
				iUsedConnectedToROILeft, iConnectedToROILeftMargin,
				iUsedConnectedToROIRight, iConnectedToROIRightMargin,
				iUsedConnectedToROIVerticalCenter, iConnectedToROIVerticalCenterMargin,
				iUsedConnectedToROIAnyPosition, iConnectedToROIAnyPositionMargin,
				iUsedConnectedToROI2Sides,
				iUsedConnectedToROIInnerOuter
			);

			// 8-4. Defect Condition Parameter Part.4 (sDefectConditioinParameter4)
			CString sDefectConditioinParameter4;

			/// Used Boundary Gradient (iUsedBoundaryGradient)
			/// Boundary Gradient Min (iBoundaryGradientMin)
			/// Boundary Gradient Max (iBoundaryGradientMax)
			int iUsedBoundaryGradient, iBoundaryGradientMin, iBoundaryGradientMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionGrad)
				iUsedBoundaryGradient = 1;
			else
				iUsedBoundaryGradient = 0;
			iBoundaryGradientMin = TeachAlgorithmParam.m_iDefectConditionGradMin;
			iBoundaryGradientMax = TeachAlgorithmParam.m_iDefectConditionGradMax;

			/// Used Included Holes (iUsedIncludedHoles)
			/// Included Holes Area (iIncludedHolesArea)
			int iUsedIncludedHoles, iIncludedHolesArea;
			if (TeachAlgorithmParam.m_bUseDefectConditionHole)
				iUsedIncludedHoles = 1;
			else
				iUsedIncludedHoles = 0;
			iIncludedHolesArea = TeachAlgorithmParam.m_iDefectConditionHoleArea;

			/// Used Sum of x length (iUsedXLengthSum)
			/// Sum of x length Min (iXLengthSumMin)
			/// Sum of x length Max (iXLengthSumMax)
			int iUsedXLengthSum, iXLengthSumMin, iXLengthSumMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionXLengthSum)
				iUsedXLengthSum = 1;
			else
				iUsedXLengthSum = 0;
			iXLengthSumMin = TeachAlgorithmParam.m_iDefectConditionXLengthSumMin;
			iXLengthSumMax = TeachAlgorithmParam.m_iDefectConditionXLengthSumMax;

			/// Used Sum of y length (iUsedYLengthSum)
			/// Sum of y length Min (iYLengthSumMin)
			/// Sum of y length Max (iYLengthSumMax)
			int iUsedYLengthSum, iYLengthSumMin, iYLengthSumMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionYLengthSum)
				iUsedYLengthSum = 1;
			else
				iUsedYLengthSum = 0;
			iYLengthSumMin = TeachAlgorithmParam.m_iDefectConditionYLengthSumMin;
			iYLengthSumMax = TeachAlgorithmParam.m_iDefectConditionYLengthSumMax;

			/// Used Shift x from ROI Center to Defect Center (iUsedShiftX)
			/// Shift x from ROI Center to Defect Center Min (iShiftXMin)
			/// Shift x from ROI Center to Defect Center Max (iShiftXMax)
			int iUsedShiftX, iShiftXMin, iShiftXMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionShiftX)
				iUsedShiftX = 1;
			else
				iUsedShiftX = 0;
			iShiftXMin = TeachAlgorithmParam.m_iDefectConditionShiftXMin;
			iShiftXMax = TeachAlgorithmParam.m_iDefectConditionShiftXMax;

			/// Used Shift y from ROI Center to Defect Center (iUsedShiftY)
			/// Shift y from ROI Center to Defect Center Min (iShiftYMin)
			/// Shift y from ROI Center to Defect Center Max (iShiftYMax)
			int iUsedShiftY, iShiftYMin, iShiftYMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionShiftY)
				iUsedShiftY = 1;
			else
				iUsedShiftY = 0;
			iShiftYMin = TeachAlgorithmParam.m_iDefectConditionShiftYMin;
			iShiftYMax = TeachAlgorithmParam.m_iDefectConditionShiftYMax;

			/// Used Zero Area Detect (iUsedZeroAreaDetect)
			int iUsedZeroAreaDetect;
			if (TeachAlgorithmParam.m_bUseDefectConditionUseZeroArea)
				iUsedZeroAreaDetect = 1;
			else
				iUsedZeroAreaDetect = 0;

			/// Used Dilation Area Detect (iUsedDilationAreaDetect)
			/// Dilation value (iDilationValue)
			int iUsedDilationAreaDetect, iDilationValue;
			if (TeachAlgorithmParam.m_bDefectConditionDilation)
				iUsedDilationAreaDetect = 1;
			else
				iUsedDilationAreaDetect = 0;
			iDilationValue = TeachAlgorithmParam.m_iDefectConditionDilationSize;

			// 8-4. Defect Condition Parameter Part.4 End
			sDefectConditioinParameter4.Format("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
				iUsedBoundaryGradient, iBoundaryGradientMin, iBoundaryGradientMax,
				iUsedIncludedHoles, iIncludedHolesArea,
				iUsedXLengthSum, iXLengthSumMin, iXLengthSumMax,
				iUsedYLengthSum, iYLengthSumMin, iYLengthSumMax,
				iUsedShiftX, iShiftXMin, iShiftXMax,
				iUsedZeroAreaDetect,
				iUsedDilationAreaDetect,
				iDilationValue
			);

			// 9. LGIT SVM Result (sSVMResult)
			CString sSVMResult;

			/// SVM Result (sSVMClass)
			HObject HSVMResultRgn;
			CString sSVMClass = "";

			// 10. LGIT AI Classification Result (sSecondaryResult)
			CString sSecondaryResult;

			/// Classification Result (sSecondaryClass)
			/// Classification Max Score (dClassificationMaxScore)
			HObject HSecondaryResultRgn;
			int iSecondaryClass = -1;
			CString sSecondaryClass = "";
			double dSecondaryMaxScore = 0;

			// 11. LGIT AI Segmentation Result (sTertiaryResult)
			CString sTertiaryResult;

			/// Segmentation Area (lSegmentationArea)
			/// Segmentation Length (dSegmentationLength)
			/// Segmentation Width (dSegmentationWidth)
			/// Segmentation Contrast (dSegmentationContrast)
			/// Segmentation Size Spec Min (iSegmentationSizeSpecMin)
			/// Segmentation Size Spec Mid (iSegmentationSizeSpecMid)
			/// Segmentation Size Spec Max (iSegmentationSizeSpecMax)
			/// Segmentation Count Spec Min to Mid (iSegmentationCountSpecMinToMid)
			/// Segmentation Count Spec Mid to Max (iSegmentationCountSpecMidToMax)
			/// Segmentation Count Spec Max (iSegmentationCountSpecMax)
			HObject HTertiaryResultRgn, SelectedTertiaryBlob, HConnIntersectRgn;
			long lNoTertiaryBlob;
			long lSegmentationArea;
			double dSegmentationLength, dSegmentationWidth, dSegmentationContrast;
			double dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax;
			int iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax;

			// Total Variation Feature
			CString sVariationFeatureLog;

			if (result.svm_result->result && !AIService::ImageUtile::HasImageData(result.svm_result->blob_ng_mask))
			{
				sSVMClass = "Good";
				// 9. LGIT SVM Result End
				sSVMResult.Format("%s",
					sSVMClass
				);

				sSecondaryClass = "Good";
				iSecondaryClass = -1;
				dSecondaryMaxScore = 0;

				// 10. LGIT AI Classification Result End
				sSecondaryResult.Format("%s\t%.3lf",
					sSecondaryClass, dSecondaryMaxScore
				);

				lSegmentationArea = 0;
				dSegmentationLength = 0;
				dSegmentationWidth = 0;
				dSegmentationContrast = 0;

				dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][iDefectType];
				dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][iDefectType];
				dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][iDefectType];
				iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][iDefectType];
				iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][iDefectType];
				iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][iDefectType];

				// 11. LGIT AI Segmentation Result End
				sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
					lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
					dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
				);

				// Total Variation Feature
				sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
					sRunInfo,
					sInspectROIInfo,
					sVariationParameterFeature,
					sReferenceROIVariationFeature,
					sCurrentROIVariationFeature,
					sBlobVariationFeature,
					sBlobDefectShapeFeature1,
					sBlobDefectShapeFeature2,
					sBlobDefectShapeFeature3,
					sBlobDefectShapeFeature4,
					sDefectConditioinParameter1,
					sDefectConditioinParameter2,
					sDefectConditioinParameter3,
					sDefectConditioinParameter4,
					sSVMResult,
					sSecondaryResult,
					sTertiaryResult
				);

				if (!Struct_PreferenceStruct.m_bUseResultLogThread)
				{
					if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
						NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
					if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
						NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
				}
				else
				{
					if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
						pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
						pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
						pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
						m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
					}
					if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
						pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
						pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
						pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
						m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
					}
				}

				continue;
			}
			else if (result.svm_result->result && AIService::ImageUtile::HasImageData(result.svm_result->blob_ng_mask))
			{
				HSVMResultRgn = AIService::ImageUtile::Mask2HRegion(result.svm_result->blob_ng_mask);

				HObject HSVMConnRgn, HSVMSelectedBlob;
				long lNoSVMDefectCount;
				Connection(HSVMResultRgn, &HSVMConnRgn);
				CountObj(HSVMConnRgn, &CountDefect);
				lNoSVMDefectCount = CountDefect[0].I();

				for (int iSVMBlobIdx = 0; iSVMBlobIdx < lNoSVMDefectCount; iSVMBlobIdx++)
				{
					SelectObj(HSVMConnRgn, &HSVMSelectedBlob, iSVMBlobIdx + 1);
					Intersection(HSelectedBlob, HSVMSelectedBlob, &HIntersectRgn);
					if (m_pGFunction->ValidHRegion(HIntersectRgn))
					{
						sSVMClass = "NG";
						// 9. LGIT SVM Result End
						sSVMResult.Format("%s",
							sSVMClass
						);

						if (iSVMBlobIdx >= result.secondary_result->blob_count)
						{
							sSecondaryClass = "Error";
							iSecondaryClass = -1;
							dSecondaryMaxScore = -1;

							// 10. LGIT AI Classification Result End
							sSecondaryResult.Format("%s\t%.3lf",
								sSecondaryClass, dSecondaryMaxScore
							);

							lSegmentationArea = -1;
							dSegmentationLength = -1;
							dSegmentationWidth = -1;
							dSegmentationContrast = -1;

							dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][iDefectType];
							dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][iDefectType];
							dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][iDefectType];
							iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][iDefectType];
							iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][iDefectType];
							iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][iDefectType];

							// 11. LGIT AI Segmentation Result End
							sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
								lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
								dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
							);

							// Total Variation Feature
							sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
								sRunInfo,
								sInspectROIInfo,
								sVariationParameterFeature,
								sReferenceROIVariationFeature,
								sCurrentROIVariationFeature,
								sBlobVariationFeature,
								sBlobDefectShapeFeature1,
								sBlobDefectShapeFeature2,
								sBlobDefectShapeFeature3,
								sBlobDefectShapeFeature4,
								sDefectConditioinParameter1,
								sDefectConditioinParameter2,
								sDefectConditioinParameter3,
								sDefectConditioinParameter4,
								sSVMResult,
								sSecondaryResult,
								sTertiaryResult
							);

							if (!Struct_PreferenceStruct.m_bUseResultLogThread)
							{
								if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
									NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
								if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
									NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
							}
							else
							{
								if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
								{
									RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
									pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
									pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
									pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
									pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
									pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
									m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
								}
								if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
								{
									RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
									pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
									pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
									pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
									pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
									pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
									m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
								}
							}

							continue;
						}

						if (AIService::ImageUtile::HasImageData(result.secondary_result->blob_ok_mask))
						{
							HSecondaryResultRgn = AIService::ImageUtile::Mask2HRegion(result.secondary_result->blob_ok_mask);
							Intersection(HSecondaryResultRgn, HSVMSelectedBlob, &HIntersectRgn);
							if (m_pGFunction->ValidHRegion(HIntersectRgn))
							{
								sSecondaryClass = "Good";
								iSecondaryClass = -1;
								dSecondaryMaxScore = result.secondary_result->score_list[iSVMBlobIdx];
							}
						}

						if (sSecondaryClass == "" && result.secondary_result && result.secondary_result->result)
						{
							for (const auto&[type, mask] : result.secondary_result->blob_mask_map) {
								if (!AIService::ImageUtile::HasImageData(mask)) { continue; }
								HSecondaryResultRgn = AIService::ImageUtile::Mask2HRegion(mask);
								Intersection(HSecondaryResultRgn, HSVMSelectedBlob, &HIntersectRgn);
								if (m_pGFunction->ValidHRegion(HIntersectRgn))
								{
									sSecondaryClass = m_strDefectName[type];
									iSecondaryClass = type;
									dSecondaryMaxScore = result.secondary_result->score_list[iSVMBlobIdx];

									break;
								}
							}
						}

						// 10. LGIT AI Classification Result End
						sSecondaryResult.Format("%s\t%.3lf",
							sSecondaryClass, dSecondaryMaxScore
						);


						if (iSecondaryClass >= 0 && result.tertiary_result && result.tertiary_result->result)
						{
							for (const auto&[type, mask] : result.tertiary_result->blob_mask_map) {
								if (iSecondaryClass != type) { continue; }
								if (!AIService::ImageUtile::HasImageData(mask))
								{
									lSegmentationArea = 0;
									dSegmentationLength = 0;
									dSegmentationWidth = 0;
									dSegmentationContrast = 0;

									dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][type];
									dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][type];
									dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][type];
									iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][type];
									iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][type];
									iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][type];

									// 11. LGIT AI Segmentation Result End
									sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
										lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
										dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
									);

									// Total Variation Feature
									sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
										sRunInfo,
										sInspectROIInfo,
										sVariationParameterFeature,
										sReferenceROIVariationFeature,
										sCurrentROIVariationFeature,
										sBlobVariationFeature,
										sBlobDefectShapeFeature1,
										sBlobDefectShapeFeature2,
										sBlobDefectShapeFeature3,
										sBlobDefectShapeFeature4,
										sDefectConditioinParameter1,
										sDefectConditioinParameter2,
										sDefectConditioinParameter3,
										sDefectConditioinParameter4,
										sSVMResult,
										sSecondaryResult,
										sTertiaryResult
									);

									if (!Struct_PreferenceStruct.m_bUseResultLogThread)
									{
										if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
											NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
										if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
											NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
									}
									else
									{
										if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
										{
											RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
											pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
											pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
											pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
											pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
											pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
											m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
										}
										if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
										{
											RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
											pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
											pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
											pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
											pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
											pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
											m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
										}
									}
									break;
								}
								else
								{
									HTertiaryResultRgn = AIService::ImageUtile::Mask2HRegion(mask);
									Connection(HTertiaryResultRgn, &HConnIntersectRgn);
									CountObj(HConnIntersectRgn, &CountDefect);
									lNoTertiaryBlob = CountDefect[0].I();
									for (int iTertiaryBlobIdx = 0; iTertiaryBlobIdx < lNoTertiaryBlob; iTertiaryBlobIdx++)
									{
										SelectObj(HConnIntersectRgn, &SelectedTertiaryBlob, iTertiaryBlobIdx + 1);
										Intersection(HTertiaryResultRgn, HSVMSelectedBlob, &HIntersectRgn);
										if (m_pGFunction->ValidHRegion(HIntersectRgn))
										{
											AreaCenter(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3);
											lSegmentationArea = HParam1[0].L();

											DiameterRegion(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
											dSegmentationLength = HParam5.D();

											InnerCircle(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3);
											dSegmentationWidth = HParam3.D() * 2.0;

											FillUp(SelectedTertiaryBlob, &HFillupRgn);
											DilationCircle(HFillupRgn, &HFillupRgn, 3.5);
											Intensity(HFillupRgn, HVariationInspectRgn, &HParam1, &HParam2);
											dSegmentationContrast = HParam1.D();

											dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][type];
											dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][type];
											dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][type];
											iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][type];
											iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][type];
											iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][type];

											// 11. LGIT AI Segmentation Result End
											sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
												lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
												dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
											);

											// Total Variation Feature
											sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
												sRunInfo,
												sInspectROIInfo,
												sVariationParameterFeature,
												sReferenceROIVariationFeature,
												sCurrentROIVariationFeature,
												sBlobVariationFeature,
												sBlobDefectShapeFeature1,
												sBlobDefectShapeFeature2,
												sBlobDefectShapeFeature3,
												sBlobDefectShapeFeature4,
												sDefectConditioinParameter1,
												sDefectConditioinParameter2,
												sDefectConditioinParameter3,
												sDefectConditioinParameter4,
												sSVMResult,
												sSecondaryResult,
												sTertiaryResult
											);

											if (!Struct_PreferenceStruct.m_bUseResultLogThread)
											{
												if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
													NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
												if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
													NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
											}
											else
											{
												if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
												{
													RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
													pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
													pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
													pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
													pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
													pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
													m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
												}
												if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
												{
													RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
													pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
													pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
													pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
													pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
													pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
													m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
												}
											}
										}
									}
									break;
								}
							}
						}
						else
						{
							lSegmentationArea = 0;
							dSegmentationLength = 0;
							dSegmentationWidth = 0;
							dSegmentationContrast = 0;

							dSegmentationSizeSpecMin = 0;
							dSegmentationSizeSpecMid = 0;
							dSegmentationSizeSpecMax = 0;
							iSegmentationCountSpecMinToMid = 0;
							iSegmentationCountSpecMidToMax = 0;
							iSegmentationCountSpecMax = 0;

							// 11. LGIT AI Segmentation Result End
							sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
								lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
								dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
							);

							// Total Variation Feature
							sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
								sRunInfo,
								sInspectROIInfo,
								sVariationParameterFeature,
								sReferenceROIVariationFeature,
								sCurrentROIVariationFeature,
								sBlobVariationFeature,
								sBlobDefectShapeFeature1,
								sBlobDefectShapeFeature2,
								sBlobDefectShapeFeature3,
								sBlobDefectShapeFeature4,
								sDefectConditioinParameter1,
								sDefectConditioinParameter2,
								sDefectConditioinParameter3,
								sDefectConditioinParameter4,
								sSVMResult,
								sSecondaryResult,
								sTertiaryResult
							);

							if (!Struct_PreferenceStruct.m_bUseResultLogThread)
							{
								if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
									NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
								if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
									NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
							}
							else
							{
								if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
								{
									RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
									pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
									pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
									pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
									pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
									pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
									m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
								}
								if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
								{
									RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
									pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
									pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
									pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
									pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
									pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
									m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
								}
							}
						}

						break;
					}

					if (iSVMBlobIdx == lNoSVMDefectCount)
					{
						sSVMClass = "Good";
						// 9. LGIT SVM Result End
						sSVMResult.Format("%s",
							sSVMClass
						);

						sSecondaryClass = "Good";
						iSecondaryClass = -1;
						dSecondaryMaxScore = 0;

						// 10. LGIT AI Classification Result End
						sSecondaryResult.Format("%s\t%.3lf",
							sSecondaryClass, dSecondaryMaxScore
						);

						lSegmentationArea = 0;
						dSegmentationLength = 0;
						dSegmentationWidth = 0;
						dSegmentationContrast = 0;

						dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][iDefectType];
						dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][iDefectType];
						dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][iDefectType];
						iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][iDefectType];
						iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][iDefectType];
						iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][iDefectType];

						// 11. LGIT AI Segmentation Result End
						sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
							lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
							dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
						);

						// Total Variation Feature
						sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
							sRunInfo,
							sInspectROIInfo,
							sVariationParameterFeature,
							sReferenceROIVariationFeature,
							sCurrentROIVariationFeature,
							sBlobVariationFeature,
							sBlobDefectShapeFeature1,
							sBlobDefectShapeFeature2,
							sBlobDefectShapeFeature3,
							sBlobDefectShapeFeature4,
							sDefectConditioinParameter1,
							sDefectConditioinParameter2,
							sDefectConditioinParameter3,
							sDefectConditioinParameter4,
							sSVMResult,
							sSecondaryResult,
							sTertiaryResult
						);

						if (!Struct_PreferenceStruct.m_bUseResultLogThread)
						{
							if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
								NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
							if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
								NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
						}
						else
						{
							if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
							{
								RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
								pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
								pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
								pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
								pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
								pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
								m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
							}
							if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
							{
								RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
								pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
								pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
								pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
								pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
								pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
								m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
							}
						}
					}
				}
			}
			else
			{
				sSVMClass = "Unused";
				// 9. LGIT SVM Result End
				sSVMResult.Format("%s",
					sSVMClass
				);

				if (iPrimaryBlobIdx >= result.secondary_result->blob_count)
				{
					sSecondaryClass = "Error";
					iSecondaryClass = -1;
					dSecondaryMaxScore = -1;

					// 10. LGIT AI Classification Result End
					sSecondaryResult.Format("%s\t%.3lf",
						sSecondaryClass, dSecondaryMaxScore
					);

					lSegmentationArea = -1;
					dSegmentationLength = -1;
					dSegmentationWidth = -1;
					dSegmentationContrast = -1;

					dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][iDefectType];
					dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][iDefectType];
					dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][iDefectType];
					iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][iDefectType];
					iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][iDefectType];
					iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][iDefectType];

					// 11. LGIT AI Segmentation Result End
					sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
						lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
						dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
					);

					// Total Variation Feature
					sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
						sRunInfo,
						sInspectROIInfo,
						sVariationParameterFeature,
						sReferenceROIVariationFeature,
						sCurrentROIVariationFeature,
						sBlobVariationFeature,
						sBlobDefectShapeFeature1,
						sBlobDefectShapeFeature2,
						sBlobDefectShapeFeature3,
						sBlobDefectShapeFeature4,
						sDefectConditioinParameter1,
						sDefectConditioinParameter2,
						sDefectConditioinParameter3,
						sDefectConditioinParameter4,
						sSVMResult,
						sSecondaryResult,
						sTertiaryResult
					);

					if (!Struct_PreferenceStruct.m_bUseResultLogThread)
					{
						if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
							NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
						if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
							NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
					}
					else
					{
						if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
						{
							RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
							pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
							pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
							pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
							pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
							pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
							m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
						}
						if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
						{
							RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
							pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
							pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
							pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
							pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
							pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
							m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
						}
					}

					continue;
				}

				if (AIService::ImageUtile::HasImageData(result.secondary_result->blob_ok_mask))
				{
					HSecondaryResultRgn = AIService::ImageUtile::Mask2HRegion(result.secondary_result->blob_ok_mask);
					Intersection(HSecondaryResultRgn, HSelectedBlob, &HIntersectRgn);
					if (m_pGFunction->ValidHRegion(HIntersectRgn))
					{
						sSecondaryClass = "Good";
						iSecondaryClass = -1;
						dSecondaryMaxScore = result.secondary_result->score_list[iPrimaryBlobIdx];
					}
				}

				if (sSecondaryClass == "" && result.secondary_result && result.secondary_result->result)
				{
					for (const auto&[type, mask] : result.secondary_result->blob_mask_map) {
						if (!AIService::ImageUtile::HasImageData(mask)) { continue; }
						HSecondaryResultRgn = AIService::ImageUtile::Mask2HRegion(mask);
						Intersection(HSecondaryResultRgn, HSelectedBlob, &HIntersectRgn);
						if (m_pGFunction->ValidHRegion(HIntersectRgn))
						{
							sSecondaryClass = m_strDefectName[type];
							iSecondaryClass = type;
							dSecondaryMaxScore = result.secondary_result->score_list[iPrimaryBlobIdx];

							break;
						}
					}
				}

				// 10. LGIT AI Classification Result End
				sSecondaryResult.Format("%s\t%.3lf",
					sSecondaryClass, dSecondaryMaxScore
				);


				if (iSecondaryClass >= 0 && result.tertiary_result && result.tertiary_result->result)
				{
					for (const auto&[type, mask] : result.tertiary_result->blob_mask_map) {
						if (iSecondaryClass != type) { continue; }
						if (!AIService::ImageUtile::HasImageData(mask))
						{
							lSegmentationArea = 0;
							dSegmentationLength = 0;
							dSegmentationWidth = 0;
							dSegmentationContrast = 0;

							dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][type];
							dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][type];
							dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][type];
							iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][type];
							iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][type];
							iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][type];

							// 11. LGIT AI Segmentation Result End
							sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
								lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
								dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
							);

							// Total Variation Feature
							sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
								sRunInfo,
								sInspectROIInfo,
								sVariationParameterFeature,
								sReferenceROIVariationFeature,
								sCurrentROIVariationFeature,
								sBlobVariationFeature,
								sBlobDefectShapeFeature1,
								sBlobDefectShapeFeature2,
								sBlobDefectShapeFeature3,
								sBlobDefectShapeFeature4,
								sDefectConditioinParameter1,
								sDefectConditioinParameter2,
								sDefectConditioinParameter3,
								sDefectConditioinParameter4,
								sSVMResult,
								sSecondaryResult,
								sTertiaryResult
							);

							if (!Struct_PreferenceStruct.m_bUseResultLogThread)
							{
								if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
									NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
								if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
									NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
							}
							else
							{
								if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
								{
									RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
									pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
									pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
									pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
									pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
									pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
									m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
								}
								if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
								{
									RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
									pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
									pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
									pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
									pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
									pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
									m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
								}
							}
							break;
						}
						else
						{
							HTertiaryResultRgn = AIService::ImageUtile::Mask2HRegion(mask);
							Connection(HTertiaryResultRgn, &HConnIntersectRgn);
							CountObj(HConnIntersectRgn, &CountDefect);
							lNoTertiaryBlob = CountDefect[0].I();
							for (int iTertiaryBlobIdx = 0; iTertiaryBlobIdx < lNoTertiaryBlob; iTertiaryBlobIdx++)
							{
								SelectObj(HConnIntersectRgn, &SelectedTertiaryBlob, iTertiaryBlobIdx + 1);
								Intersection(SelectedTertiaryBlob, HSelectedBlob, &HIntersectRgn);
								if (m_pGFunction->ValidHRegion(HIntersectRgn))
								{
									AreaCenter(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3);
									lSegmentationArea = HParam1[0].L();

									DiameterRegion(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
									dSegmentationLength = HParam5.D();

									InnerCircle(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3);
									dSegmentationWidth = HParam3.D() * 2.0;

									FillUp(SelectedTertiaryBlob, &HFillupRgn);
									DilationCircle(HFillupRgn, &HFillupRgn, 3.5);
									Intensity(HFillupRgn, HVariationInspectRgn, &HParam1, &HParam2);
									dSegmentationContrast = HParam1.D();

									dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][type];
									dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][type];
									dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][type];
									iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][type];
									iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][type];
									iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][type];

									// 11. LGIT AI Segmentation Result End
									sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
										lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
										dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
									);

									// Total Variation Feature
									sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
										sRunInfo,
										sInspectROIInfo,
										sVariationParameterFeature,
										sReferenceROIVariationFeature,
										sCurrentROIVariationFeature,
										sBlobVariationFeature,
										sBlobDefectShapeFeature1,
										sBlobDefectShapeFeature2,
										sBlobDefectShapeFeature3,
										sBlobDefectShapeFeature4,
										sDefectConditioinParameter1,
										sDefectConditioinParameter2,
										sDefectConditioinParameter3,
										sDefectConditioinParameter4,
										sSVMResult,
										sSecondaryResult,
										sTertiaryResult
									);

									if (!Struct_PreferenceStruct.m_bUseResultLogThread)
									{
										if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
											NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
										if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
											NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
									}
									else
									{
										if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
										{
											RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
											pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
											pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
											pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
											pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
											pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
											m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
										}
										if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
										{
											RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
											pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
											pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
											pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
											pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
											pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
											m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
										}
									}
								}
							}
							break;
						}
					}
				}
				else
				{
					lSegmentationArea = 0;
					dSegmentationLength = 0;
					dSegmentationWidth = 0;
					dSegmentationContrast = 0;

					dSegmentationSizeSpecMin = 0;
					dSegmentationSizeSpecMid = 0;
					dSegmentationSizeSpecMax = 0;
					iSegmentationCountSpecMinToMid = 0;
					iSegmentationCountSpecMidToMax = 0;
					iSegmentationCountSpecMax = 0;

					// 11. LGIT AI Segmentation Result End
					sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
						lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
						dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
					);

					// Total Variation Feature
					sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
						sRunInfo,
						sInspectROIInfo,
						sVariationParameterFeature,
						sReferenceROIVariationFeature,
						sCurrentROIVariationFeature,
						sBlobVariationFeature,
						sBlobDefectShapeFeature1,
						sBlobDefectShapeFeature2,
						sBlobDefectShapeFeature3,
						sBlobDefectShapeFeature4,
						sDefectConditioinParameter1,
						sDefectConditioinParameter2,
						sDefectConditioinParameter3,
						sDefectConditioinParameter4,
						sSVMResult,
						sSecondaryResult,
						sTertiaryResult
					);

					if (!Struct_PreferenceStruct.m_bUseResultLogThread)
					{
						if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
							NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
						if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
							NewSaveDefectVariationFeatureLog(m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1], sVariationFeatureLog);
					}
					else
					{
						if (Struct_PreferenceStruct.m_bResultLogSaveLAS)
						{
							RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
							pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
							pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_LAS[iVisionCamType][iMzNo - 1];
							pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
							pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
							pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
							m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
						}
						if (Struct_PreferenceStruct.m_bResultLogSaveLocal)
						{
							RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
							pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
							pSaveLogThreadIDParam_Edge->strPath = m_FileBase.m_strResultLogFolder_Local[iVisionCamType][iMzNo - 1];
							pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
							pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
							pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
							m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
						}
					}
				}
			}
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

		strLog.Format("Halcon Exception [uScan::NewSaveVariationFeature] : <%s>%s", sOperatorName, sErrMsg);
		m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

void CuScanApp::NewSaveVariationFeature_Thread(const LGIT::InspectionResultSet& result, CString sModelName, SYSTEMTIME time, GTRegion *pInspectROIRgn, int iModelType, CString strLogSavePath)
{
	try
	{
		HObject DefectRegion;
		DefectRegion = AIService::ImageUtile::Mask2HRegion(result.primary_result->mask);

		int iInspectionBufferIdx, iInspectionType;
		iInspectionBufferIdx = result.request->vision_inspection_thread_index;
		iInspectionType = result.request->roi_inspection_type_idx;

		// 1. Run Info (sRunInfo)
		CString sRunInfo;

		/// Date (sDay)
		CString sDay;
		sDay.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);

		/// Time (sTime)
		CString sTime;
		sTime.Format("%02d:%02d:%02d", time.wHour, time.wMinute, time.wSecond);

		/// Machine Code (sEquipNo)
		CString sEquipNo;
		sEquipNo = CA2T(result.request->equip_no.c_str());

		/// Recipe Name (sModelName)
		sModelName;

		/// SW Parameter Version (iSwParamVersion)
		int iSwParamVersion;
		iSwParamVersion = GetSwParamVersion();

		/// HW Parameter Version (iHwParamVersion)
		int iHwParamVersion;
		iHwParamVersion = GetHwParamVersion();

		/// Magazine No (iMzNo)
		int iMzNo;
		iMzNo = result.request->magazine_no;

		/// Lot ID (sLotID)
		CString sLotID;
		sLotID = CA2T(result.request->lot_id.c_str());

		/// Tray No (iTrayNo)
		int iTrayNo;
		iTrayNo = result.request->tray_no;

		/// Module No (iModuleNo)
		int iModuleNo;
		iModuleNo = result.request->module_no;

		/// Barcode (sBarcode)
		CString sBarcode;
		sBarcode = CA2T(result.request->barcode.c_str());

		// 1. Run Info End
		sRunInfo.Format("%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%d\t%d\t%s",
			sDay,
			sTime,
			sEquipNo,
			sModelName,
			iSwParamVersion,
			iHwParamVersion,
			iMzNo,
			sLotID,
			iTrayNo,
			iModuleNo,
			sBarcode);

		// 2. Inspect ROI Info (sInspectROIInfo)
		CString sInspectROIInfo;

		int iVisionCamType;
		iVisionCamType = result.request->vision_cam;

		/// Vision (sCamName)
		CString sCamName;
		sCamName = CA2T(result.request->vision_name.c_str());

		/// PC No (iPcNo)
		int iPcNo;
		iPcNo = result.request->pc_idx;

		/// Stage No (iStageNo)
		int iStageNo;
		iStageNo = result.request->stage_idx;

		/// Inspection Type (sInspectionType)
		CString sInspectionType;
		sInspectionType = CA2T(result.request->inspectiontype_name.c_str());

		/// Defect Code (sDefectType)
		int iDefectType, iDefectTransCode;
		iDefectType = pInspectROIRgn->m_AlgorithmParam[result.request->inspection_tab_idx].m_iDefectType;
		CString sDefectType;
		sDefectType = GetDefectCode(iDefectType, &iDefectTransCode);

		/// Image No (iNoInspectImage)
		int iNoInspectImage;
		iNoInspectImage = result.request->image_idx;

		/// ROI No (iROINo)
		int iROINo;
		iROINo = result.request->roi_id;

		/// Inspection Tab No (iInspectionTabIndex)
		int iInspectionTabIndex;
		iInspectionTabIndex = result.request->inspection_tab_idx;

		/// ROI Position Left (iTeachROILeft)
		/// ROI Position Top (iTeachROITop)
		/// ROI Position Right (iTeachROIRight)
		/// ROI Position Bottom (iTeachROIBottom)
		POINT LTPoint, RBPoint;
		int iTeachROILeft, iTeachROITop, iTeachROIRight, iTeachROIBottom;
		pInspectROIRgn->GetLTPoint(&LTPoint, m_pCalDataService_N[iVisionCamType]);
		pInspectROIRgn->GetRBPoint(&RBPoint, m_pCalDataService_N[iVisionCamType]);
		iTeachROILeft = LTPoint.x;
		iTeachROITop = LTPoint.y;
		iTeachROIRight = RBPoint.x;
		iTeachROIBottom = RBPoint.y;

		CAlgorithmParam TeachAlgorithmParam;
		TeachAlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iInspectionTabIndex];

		/// Local Align x (iROIDeltaX)
		/// Local Align y (iROIDeltaY)
		/// Local Align Angle (dROIDeltaAngle)
		int iROIDeltaX, iROIDeltaY;
		double dROIDeltaAngle;
		iROIDeltaX = result.request->local_align_deltaX;
		iROIDeltaY = result.request->local_align_deltaY;
		dROIDeltaAngle = result.request->local_align_angle;

		/// Image Filter Type (iImageFilterType)
		/// Image Filter Width (iImageProcessFilterX)
		/// Image Filter Height (iImageProcessFilterY)
		int iImageFilterType, iImageProcessFilterX, iImageProcessFilterY;
		if (TeachAlgorithmParam.m_bUseImageProcessFilter == FALSE) {
			iImageFilterType = FILTER_TYPE_NOT_USED;
			iImageProcessFilterX = 0;
			iImageProcessFilterY = 0;
		}
		else
		{
			iImageFilterType = TeachAlgorithmParam.m_iImageProcessFilterType1;
			iImageProcessFilterX = TeachAlgorithmParam.m_iImageProcessFilterType1X;
			iImageProcessFilterY = TeachAlgorithmParam.m_iImageProcessFilterType1Y;
		}

		/// Image process Channel Type (sProcessChType)
		CString sProcessChType;
		int iProcessChType;
		if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_COLOR)
		{
			sProcessChType = "Color";
			iProcessChType = 3;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_R)
		{
			sProcessChType = "R";
			iProcessChType = 0;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_G)
		{
			sProcessChType = "G";
			iProcessChType = 1;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_B)
		{
			sProcessChType = "B";
			iProcessChType = 2;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_H)
		{
			sProcessChType = "H";
			iProcessChType = 3;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_S)
		{
			sProcessChType = "S";
			iProcessChType = 3;
		}
		else if (TeachAlgorithmParam.m_iProcessChType == CHANNEL_TYPE_I)
		{
			sProcessChType = "I";
			iProcessChType = 3;
		}

		// 2. Inspect ROI Info End
		sInspectROIInfo.Format("%s%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%lf\t%d\t%d\t%d\t%s",
			sCamName, iPcNo,
			iStageNo,
			sInspectionType,
			sDefectType,
			iNoInspectImage,
			iROINo,
			iInspectionTabIndex + 1,
			iTeachROILeft, iTeachROITop, iTeachROIRight, iTeachROIBottom,
			iROIDeltaX, iROIDeltaY, dROIDeltaAngle,
			iImageFilterType, iImageProcessFilterX, iImageProcessFilterY,
			sProcessChType);

		// 3. Variation Parameter Feature (sVariationParameterFeature)
		CString sVariationParameterFeature;

		/// Bright DIFF (iImageCompareBrightAbs)
		/// Bright STDEV (dImageCompareBrightVar)
		/// Dark DIFF (iImageCompareDarkAbs)
		/// Dark STDEV (dImageCompareDarkVar)
		int iImageCompareBrightAbs, iImageCompareDarkAbs;
		double dImageCompareBrightVar, dImageCompareDarkVar;
		if (TeachAlgorithmParam.m_bUseImageCompare)
		{
			iImageCompareBrightAbs = TeachAlgorithmParam.m_iImageCompareBrightAbs;
			dImageCompareBrightVar = TeachAlgorithmParam.m_dImageCompareBrightVar;
			iImageCompareDarkAbs = TeachAlgorithmParam.m_iImageCompareDarkAbs;
			dImageCompareDarkVar = TeachAlgorithmParam.m_dImageCompareDarkVar;
		}
		else
		{
			iImageCompareBrightAbs = 0;
			dImageCompareBrightVar = 0;
			iImageCompareDarkAbs = 0;
			dImageCompareDarkVar = 0;
		}

		// 3. Variation Parameter Feature End
		sVariationParameterFeature.Format("%d\t%lf\t%d\t%lf",
			iImageCompareBrightAbs, dImageCompareBrightVar, iImageCompareDarkAbs, dImageCompareDarkVar
		);

		HObject HMasterMeanImage, HMasterVariationImage;
		HObject HVariationInspectRgn;
		if (TeachAlgorithmParam.m_bUseImageCompare)
		{
			if (TeachAlgorithmParam.m_bUseImageCompareWindow)
				CopyImage(pInspectROIRgn->m_HVarMeanImage[iInspectionTabIndex], &HMasterMeanImage);
			else
				GetVariationModel(&HMasterMeanImage, &HMasterVariationImage, pInspectROIRgn->m_HVarModelID[iInspectionTabIndex]);
			HVariationInspectRgn = pInspectROIRgn->m_HVariationInspectImage[iInspectionBufferIdx];

			if (0) // Variation Current ROI Image Save
			{
				std::string local_align_angle_string = std::string("P0000000");
				if (dROIDeltaAngle != 0)
				{
					double angle = dROIDeltaAngle;
					char sign = (angle < 0) ? 'M' : 'P';
					angle = fabs(angle);

					int int_part = static_cast<int>(angle);
					int decimal_part = static_cast<int>((angle - int_part) * 10000);

					std::ostringstream oss;
					oss << sign
						<< std::setw(3) << std::setfill('0') << int_part
						<< std::setw(4) << std::setfill('0') << decimal_part;

					local_align_angle_string = oss.str();
				}
				CString strLocalAlignAngle = CA2T(local_align_angle_string.c_str());

				CString strFileName;
				strFileName.Format("%s_%s_%s_Tray%d_Module%d_%s%d_Stage%d_Image%d_ROI%d_%s_Tab%d_%s_%s_%s_V3",
					Struct_PreferenceStruct.m_strEquipNo, g_strModelTypeName[iModelType],
					sLotID, iTrayNo, iModuleNo,
					sCamName, Struct_PreferenceStruct.m_iPCType + 1, iStageNo,
					iNoInspectImage, iROINo, sInspectionType, iInspectionTabIndex + 1, sBarcode, strLocalAlignAngle, sProcessChType);
				WriteImage(HVariationInspectRgn, "jpeg 100", 0, HTuple(m_FileBase.m_strAIFolder_Local[iVisionCamType][iMzNo - 1][iTrayNo - 1] + "\\ROIImage\\" + strFileName));
			}

			if (0) // Variation Reference(Mean) Image Save
			{
				std::string local_align_angle_string = std::string("P0000000");
				if (dROIDeltaAngle != 0)
				{
					double angle = dROIDeltaAngle;
					char sign = (angle < 0) ? 'M' : 'P';
					angle = fabs(angle);

					int int_part = static_cast<int>(angle);
					int decimal_part = static_cast<int>((angle - int_part) * 10000);

					std::ostringstream oss;
					oss << sign
						<< std::setw(3) << std::setfill('0') << int_part
						<< std::setw(4) << std::setfill('0') << decimal_part;

					local_align_angle_string = oss.str();
				}
				CString strLocalAlignAngle = CA2T(local_align_angle_string.c_str());

				CString strFileName;
				strFileName.Format("%s_%s_%s_Tray%d_Module%d_%s%d_Stage%d_Image%d_ROI%d_%s_Tab%d_%s_%s_%s_V4",
					Struct_PreferenceStruct.m_strEquipNo, g_strModelTypeName[iModelType],
					sLotID, iTrayNo, iModuleNo,
					sCamName, Struct_PreferenceStruct.m_iPCType + 1, iStageNo,
					iNoInspectImage, iROINo, sInspectionType, iInspectionTabIndex + 1, sBarcode, strLocalAlignAngle, sProcessChType);
				WriteImage(HMasterMeanImage, "jpeg 100", 0, HTuple(m_FileBase.m_strAIFolder_Local[iVisionCamType][iMzNo - 1][iTrayNo - 1] + "\\ROIImage\\" + strFileName));
			}
		}
		else
		{
			HObject region = AIService::ImageUtile::Mat2HImage(result.request->roi_image);
			HObject HRunImageRGBI[4];
			Decompose3(region, &(HRunImageRGBI[0]), &(HRunImageRGBI[1]), &(HRunImageRGBI[2]));
			Rgb1ToGray(region, &HRunImageRGBI[3]);
			HVariationInspectRgn = HRunImageRGBI[iProcessChType];
			HObject region2 = pInspectROIRgn->GetROIHRegion(m_pCalDataService_N[iVisionCamType]);
			MoveRegion(region2, &region2, -iTeachROITop + 50, -iTeachROILeft + 50);
			ReduceDomain(HVariationInspectRgn, region2, &HVariationInspectRgn);
			CropDomain(HVariationInspectRgn, &HVariationInspectRgn);

			if (0) // Rule-based Current ROI Image Save
			{
				std::string local_align_angle_string = std::string("P0000000");
				if (dROIDeltaAngle != 0)
				{
					double angle = dROIDeltaAngle;
					char sign = (angle < 0) ? 'M' : 'P';
					angle = fabs(angle);

					int int_part = static_cast<int>(angle);
					int decimal_part = static_cast<int>((angle - int_part) * 10000);

					std::ostringstream oss;
					oss << sign
						<< std::setw(3) << std::setfill('0') << int_part
						<< std::setw(4) << std::setfill('0') << decimal_part;

					local_align_angle_string = oss.str();
				}
				CString strLocalAlignAngle = CA2T(local_align_angle_string.c_str());

				CString strFileName;
				strFileName.Format("%s_%s_%s_Tray%d_Module%d_%s%d_Stage%d_Image%d_ROI%d_%s_Tab%d_%s_%s_%s_V3",
					Struct_PreferenceStruct.m_strEquipNo, g_strModelTypeName[iModelType],
					sLotID, iTrayNo, iModuleNo,
					sCamName, Struct_PreferenceStruct.m_iPCType + 1, iStageNo,
					iNoInspectImage, iROINo, sInspectionType, iInspectionTabIndex + 1, sBarcode, strLocalAlignAngle, sProcessChType);
				WriteImage(HVariationInspectRgn, "jpeg 100", 0, HTuple(m_FileBase.m_strAIFolder_Local[iVisionCamType][iMzNo - 1][iTrayNo - 1] + "\\ROIImage\\" + strFileName));
			}
		}

		// 4. Variation Reference ROI Feature (sReferenceROIVariationFeature)
		CString sReferenceROIVariationFeature;

		/// Ref Bright Avarage (dAvgGVReferenceROI)
		/// Ref Bright Max (dMaxGVReferenceROI)
		/// Ref Bright Min (dMinGVReferenceROI)
		/// Ref Bright STDEV (dStdevGVReferenceROI)
		HTuple HParam1, HParam2, HParam3, HParam4, HParam5;
		double dAvgGVReferenceROI, dMaxGVReferenceROI, dMinGVReferenceROI, dStdevGVReferenceROI;
		if (TeachAlgorithmParam.m_bUseImageCompare)
		{
			Intensity(HVariationInspectRgn, HMasterMeanImage, &HParam1, &HParam2);
			MinMaxGray(HVariationInspectRgn, HMasterMeanImage, 0, &HParam3, &HParam4, &HParam5);
			dAvgGVReferenceROI = HParam1[0].D();
			dMaxGVReferenceROI = HParam4[0].D();
			dMinGVReferenceROI = HParam3[0].D();
			dStdevGVReferenceROI = HParam2[0].D();
		}
		else
		{
			dAvgGVReferenceROI = 0;
			dMaxGVReferenceROI = 0;
			dMinGVReferenceROI = 0;
			dStdevGVReferenceROI = 0;
		}

		// 4. Variation Reference ROI Feature End
		sReferenceROIVariationFeature.Format("%.1lf\t%d\t%d\t%.1lf",
			dAvgGVReferenceROI, (int)dMaxGVReferenceROI, (int)dMinGVReferenceROI, dStdevGVReferenceROI
		);

		// 5. Variation Current ROI Feature (sCurrentROIVariationFeature)
		CString sCurrentROIVariationFeature;

		/// Cur ROI Bright Avarage (dAvgGVCurrentROI)
		/// Cur ROI Bright Max (dMaxGVCurrentROI)
		/// Cur ROI Bright Min (dMinGVCurrentROI)
		/// Cur ROI Bright STDEV (dStdevGVCurrentROI)
		double dAvgGVCurrentROI, dMaxGVCurrentROI, dMinGVCurrentROI, dStdevGVCurrentROI;
		Intensity(HVariationInspectRgn, HVariationInspectRgn, &HParam1, &HParam2);
		MinMaxGray(HVariationInspectRgn, HVariationInspectRgn, 0, &HParam3, &HParam4, &HParam5);
		dAvgGVCurrentROI = HParam1[0].D();
		dMaxGVCurrentROI = HParam4[0].D();
		dMinGVCurrentROI = HParam3[0].D();
		dStdevGVCurrentROI = HParam2[0].D();

		/// Deviation Average GV Reference - Current ROI (dDeviationAvgGVRefCur)
		/// Deviation Max GV Reference - Current ROI (iDeviationMaxGVRefCur)
		/// Deviation Mix GV Reference - Current ROI (iDeviationMinGVRefCur)
		double dDeviationAvgGVRefCur;
		int iDeviationMaxGVRefCur, iDeviationMinGVRefCur;
		if (TeachAlgorithmParam.m_bUseImageCompare)
		{
			dDeviationAvgGVRefCur = dAvgGVReferenceROI - dAvgGVCurrentROI;
			iDeviationMaxGVRefCur = (int)(dMaxGVReferenceROI - dMaxGVCurrentROI);
			iDeviationMinGVRefCur = (int)(dMinGVReferenceROI - dMinGVCurrentROI);
		}
		else
		{
			dDeviationAvgGVRefCur = 0;
			iDeviationMaxGVRefCur = 0;
			iDeviationMinGVRefCur = 0;
		}

		/// Cur ROI Pixel Count All (lNoPixelAll)
		/// Cur ROI Pixel Count Over Average (lNoPixelOverAvg)
		/// Cur ROI Pixel Count Under Average (lNoPixelUnderAvg)
		/// Cur ROI Pixel Count Over Average Plus Stdev (lNoPixelUnderAvgPlusStdev)
		/// Cur ROI Pixel Count Under Average Minus Stdev (lNoPixelOverAvgMinusStdev)
		HObject HTempBlobRgn;
		long lNoPixelAll, lNoPixelOverAvg, lNoPixelUnderAvg, lNoPixelUnderAvgPlusStdev, lNoPixelOverAvgMinusStdev;
		int iTemp;
		AreaCenter(HVariationInspectRgn, &HParam1, &HParam2, &HParam3);
		lNoPixelAll = HParam1[0].L();
		Threshold(HVariationInspectRgn, &HTempBlobRgn, dAvgGVCurrentROI, 255);
		AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
		lNoPixelOverAvg = HParam1[0].L();
		Threshold(HVariationInspectRgn, &HTempBlobRgn, 0, dAvgGVCurrentROI);
		AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
		lNoPixelUnderAvg = HParam1[0].L();
		iTemp = (int)(dAvgGVCurrentROI + dStdevGVCurrentROI);
		if (iTemp > 255) iTemp = 255;
		Threshold(HVariationInspectRgn, &HTempBlobRgn, 0, iTemp);
		AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
		lNoPixelUnderAvgPlusStdev = HParam1[0].L();
		iTemp = (int)(dAvgGVCurrentROI - dStdevGVCurrentROI);
		if (iTemp < 0) iTemp = 0;
		Threshold(HVariationInspectRgn, &HTempBlobRgn, iTemp, 255);
		AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
		lNoPixelOverAvgMinusStdev = HParam1[0].L();

		// 5. Variation Current ROI Feature End
		sCurrentROIVariationFeature.Format("%.1lf\t%d\t%d\t%.1lf\t%.1lf\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
			dAvgGVCurrentROI, (int)dMaxGVCurrentROI, (int)dMinGVCurrentROI, dStdevGVCurrentROI,
			dDeviationAvgGVRefCur, iDeviationMaxGVRefCur, iDeviationMinGVRefCur,
			lNoPixelAll, lNoPixelOverAvg, lNoPixelUnderAvg, lNoPixelUnderAvgPlusStdev, lNoPixelOverAvgMinusStdev
		);

		// 해당 mask내 모든 blob for 루프
		BOOL bUseDefectSizeLimit;
		bUseDefectSizeLimit = FALSE;

		HObject HConnRgn, HSelectedBlob;
		HTuple CountDefect, HSortArea;
		long lNoDefectCount;
		long HSelectedSortArea;
		Connection(DefectRegion, &HConnRgn);
		CountObj(HConnRgn, &CountDefect);
		lNoDefectCount = CountDefect[0].I();

		// Mask내 blob이 엄청 많을 경우를 대비한 처리인데 Tuple Sorting을 하면 blob_id_list를 맞출 수 없음
		/*
		if (lNoDefectCount > Struct_PreferenceStruct.m_iMaxSameDefectShapeFeature)
		{
			bUseDefectSizeLimit = TRUE;

			AreaCenter(HConnRgn, &HParam1, &HParam2, &HParam3);
			TupleSortIndex(HParam1, &HSortArea);
			TupleInverse(HSortArea, &HSortArea);

			lNoDefectCount = Struct_PreferenceStruct.m_iMaxSameDefectShapeFeature;
		}
		*/

		for (int iPrimaryBlobIdx = 0; iPrimaryBlobIdx < lNoDefectCount; iPrimaryBlobIdx++)
		{
			if (bUseDefectSizeLimit)
			{
				HSelectedSortArea = HSortArea[iPrimaryBlobIdx].L();
				SelectObj(HConnRgn, &HSelectedBlob, HSelectedSortArea + 1);
			}
			else
				SelectObj(HConnRgn, &HSelectedBlob, iPrimaryBlobIdx + 1);

			// 6. Blob Variation Feature (sBlobVariationFeature)
			CString sBlobVariationFeature;

			/// Cur Defect Feature Bright Avarage (dAvgGVDefectRegion)
			/// Cur Defect Feature Bright Max (dMaxGVDefectRegion)
			/// Cur Defect Feature Bright Min (dMinGVDefectRegion)
			/// Cur Defect Feature Bright STDEV (dStdevGVDefectRegion)
			double dAvgGVDefectRegion, dMaxGVDefectRegion, dMinGVDefectRegion, dStdevGVDefectRegion;
			Intensity(HSelectedBlob, HVariationInspectRgn, &HParam1, &HParam2);
			MinMaxGray(HSelectedBlob, HVariationInspectRgn, 0, &HParam3, &HParam4, &HParam5);
			dAvgGVDefectRegion = HParam1[0].D();
			dMaxGVDefectRegion = HParam4[0].D();
			dMinGVDefectRegion = HParam3[0].D();
			dStdevGVDefectRegion = HParam2[0].D();

			/// Deviation Average GV Reference - Defect Region (dDeviationAvgGVRefDefect)
			/// Deviation Max GV Reference - Defect Region (iDeviationMaxGVRefDefect)
			/// Deviation Mix GV Reference - Defect Region (iDeviationMinGVRefDefect)
			double dDeviationAvgGVRefDefect;
			int iDeviationMaxGVRefDefect, iDeviationMinGVRefDefect;
			dDeviationAvgGVRefDefect = dAvgGVReferenceROI - dAvgGVDefectRegion;
			iDeviationMaxGVRefDefect = (int)(dMaxGVReferenceROI - dMaxGVDefectRegion);
			iDeviationMinGVRefDefect = (int)(dMinGVReferenceROI - dMinGVDefectRegion);

			/// Cur Defect Feature Pixel Count All (lNoPixelAll)
			/// Cur Defect Feature Pixel Count Over Average (lNoPixelOverAvg)
			/// Cur Defect Feature Pixel Count Under Average (lNoPixelUnderAvg)
			/// Cur Defect Feature Pixel Count Over Average Plus Stdev (lNoPixelUnderAvgPlusStdev)
			/// Cur Defect Feature Pixel Count Under Average Minus Stdev (lNoPixelOverAvgMinusStdev)
			HObject HImageReduced;
			ReduceDomain(HVariationInspectRgn, HSelectedBlob, &HImageReduced);
			AreaCenter(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			lNoPixelAll = HParam1[0].L();
			Threshold(HImageReduced, &HTempBlobRgn, dAvgGVDefectRegion, 255);
			AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
			lNoPixelOverAvg = HParam1[0].L();
			Threshold(HImageReduced, &HTempBlobRgn, 0, dAvgGVDefectRegion);
			AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
			lNoPixelUnderAvg = HParam1[0].L();
			iTemp = (int)(dAvgGVDefectRegion + dStdevGVDefectRegion);
			if (iTemp > 255) iTemp = 255;
			Threshold(HImageReduced, &HTempBlobRgn, 0, iTemp);
			AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
			lNoPixelUnderAvgPlusStdev = HParam1[0].L();
			iTemp = (int)(dAvgGVDefectRegion - dStdevGVDefectRegion);
			if (iTemp < 0) iTemp = 0;
			Threshold(HImageReduced, &HTempBlobRgn, iTemp, 255);
			AreaCenter(HTempBlobRgn, &HParam1, &HParam2, &HParam3);
			lNoPixelOverAvgMinusStdev = HParam1[0].L();

			// 6. Blob Variation Feature End
			sBlobVariationFeature.Format("%.1lf\t%d\t%d\t%.1lf\t%.1lf\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
				dAvgGVDefectRegion, (int)dMaxGVDefectRegion, (int)dMinGVDefectRegion, dStdevGVDefectRegion,
				dDeviationAvgGVRefDefect, iDeviationMaxGVRefDefect, iDeviationMinGVRefDefect,
				lNoPixelAll, lNoPixelOverAvg, lNoPixelUnderAvg, lNoPixelUnderAvgPlusStdev, lNoPixelOverAvgMinusStdev
			);

			// 7-1. Blob Defect Shape Fueture Part.1 (sBlobDefectShapeFeature1)
			CString sBlobDefectShapeFeature1;

			/// Area (area)
			/// Row (row)
			/// Column (column)
			long area;
			double row, column;
			AreaCenter(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			area = HParam1[0].L();
			row = HParam2[0].D();
			column = HParam3[0].D();

			/// Width (width)
			/// Height (height)
			/// Row1 (row1)
			/// Column1 (column1)
			/// Row2 (row2)
			/// Column2 (column2)
			long width, height, row1, column1, row2, column2;
			SmallestRectangle1(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			width = HParam4[0].L() - HParam2[0].L() + 1;
			height = HParam3[0].L() - HParam1[0].L() + 1;
			row1 = HParam1[0].L();
			column1 = HParam2[0].L();
			row2 = HParam3[0].L();
			column2 = HParam4[0].L();

			/// Length / Width Ratio (dLengthWidthRatio)
			double dBlobLength, dCircleRadius, dLengthWidthRatio;
			DiameterRegion(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			dBlobLength = HParam5.D();
			InnerCircle(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dCircleRadius = HParam3.D() * 2.0;
			if (dCircleRadius <= 0)
				dCircleRadius = 1;
			dLengthWidthRatio = dBlobLength / dCircleRadius;

			/// Major / Minor Axis Ratio (dMajorMinorAxisRatio)
			double dRa, dRb, dMajorMinorAxisRatio;
			EllipticAxis(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dRa = HParam1[0].D();
			dRb = HParam2[0].D();
			if (dRb != 0)
				dMajorMinorAxisRatio = dRa / dRb;
			else
				dMajorMinorAxisRatio = 1.0;

			/// Defect / ROI area Ratio (dAreaRatio)
			long area2;
			double dAreaRatio;
			AreaCenter(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			area = HParam1.L();
			AreaCenter(HVariationInspectRgn, &HParam1, &HParam2, &HParam3);
			area2 = HParam1.L();
			if (area2 != 0)
				dAreaRatio = (double)area / (double)area2;
			else
				dAreaRatio = 1;

			/// Contrast (dContrast)
			HObject HFillupRgn, HDilatedRgn, HDiffRgn;
			double dDefectIntensity, dBackGroundIntensity, dContrast;
			FillUp(HSelectedBlob, &HFillupRgn);
			Intersection(HFillupRgn, HVariationInspectRgn, &HFillupRgn);
			Intensity(HFillupRgn, HVariationInspectRgn, &HParam1, &HParam2);
			dDefectIntensity = HParam1.D();
			Difference(HVariationInspectRgn, HFillupRgn, &HDiffRgn);
			if (m_pGFunction->ValidHRegion(HDiffRgn))
			{
				Intensity(HDiffRgn, HVariationInspectRgn, &HParam1, &HParam2);
				dBackGroundIntensity = HParam1.D();
			}
			else
				dBackGroundIntensity = dDefectIntensity;
			dContrast = fabs(dBackGroundIntensity - dDefectIntensity);

			/// Distance from ROI to Defect (dROIDefectDistance)
			double dROIDefectDistance;
			DistanceRrMin(HVariationInspectRgn, HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			dROIDefectDistance = HParam1.D();

			/// Distance from ROI Left to Defect Left (iLeftDistance)
			/// Distance from ROI Top to Defect Top (iTopDistance)
			/// Distance from ROI Right to Defect Right (iRightDistance)
			/// Distance from ROI Bottom to Defect Bottom (iBottomDistance)
			long lROILTPointY, lROILTPointX, lROIRBPointY, lROIRBPointX;
			long lDefectLTPointY, lDefectLTPointX, lDefectRBPointY, lDefectRBPointX;
			int iLeftDistance, iTopDistance, iRightDistance, iBottomDistance;
			SmallestRectangle1(HVariationInspectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
			lROILTPointY = HParam1.L();
			lROILTPointX = HParam2.L();
			lROIRBPointY = HParam3.L();
			lROIRBPointX = HParam4.L();
			SmallestRectangle1(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			lDefectLTPointY = HParam1.L();
			lDefectLTPointX = HParam2.L();
			lDefectRBPointY = HParam3.L();
			lDefectRBPointX = HParam4.L();
			iLeftDistance = lDefectLTPointX - lROILTPointX - 1;
			if (iLeftDistance < 0) iLeftDistance = 0;
			iTopDistance = lDefectLTPointY - lROILTPointY - 1;
			if (iTopDistance < 0) iTopDistance = 0;
			iRightDistance = lROIRBPointX - lDefectRBPointX - 1;
			if (iRightDistance < 0) iRightDistance = 0;
			iBottomDistance = lROIRBPointY - lDefectRBPointY - 1;
			if (iBottomDistance < 0) iBottomDistance = 0;

			/// Point Count Connected to ROI (lNoPointConnectedtoROI)
			HObject HBoundaryRgn, HIntersectRgn;
			long lNoPointConnectedtoROI;
			Boundary(HVariationInspectRgn, &HBoundaryRgn, "inner");
			Intersection(HBoundaryRgn, HSelectedBlob, &HIntersectRgn);
			Connection(HIntersectRgn, &HIntersectRgn);
			CountObj(HIntersectRgn, &HParam1);
			lNoPointConnectedtoROI = HParam1.L();

			/// Boundary Gradient (dGradient)
			HObject HGradImage;
			double dGradient;
			SobelAmp(HVariationInspectRgn, &HGradImage, "sum_abs", 3);
			Boundary(HSelectedBlob, &HBoundaryRgn, "inner");
			Intensity(HBoundaryRgn, HGradImage, &HParam1, &HParam2);
			dGradient = HParam1.D();

			/// Distance x from ROI Center to Defect Center (dDistanceX)
			/// Distance y from ROI Center to Defect Center (dDistanceY))
			double dShiftInspectCenterX, dShiftInspectCenterY, dShiftDefectCenterX, dShiftDefectCenterY;
			double dDistanceX, dDistanceY;
			AreaCenter(HVariationInspectRgn, &HParam1, &HParam2, &HParam3);
			dShiftInspectCenterY = HParam2.D();
			dShiftInspectCenterX = HParam3.D();
			AreaCenter(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dShiftDefectCenterY = HParam2.D();
			dShiftDefectCenterX = HParam3.D();
			dDistanceX = fabs(dShiftInspectCenterX - dShiftDefectCenterX);
			dDistanceY = fabs(dShiftInspectCenterY - dShiftDefectCenterY);

			// 7-1. Blob Defect Shape Fueture Part.1 End
			sBlobDefectShapeFeature1.Format("%d\t%.3lf\t%.3lf\t%d\t%d\t%d\t%d\t%d\t%d\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%d\t%d\t%d\t%d\t%d\t%.3lf\t%.3lf\t%.3lf",
				area, row, column,
				width, height, row1, column1, row2, column2,
				dLengthWidthRatio,
				dMajorMinorAxisRatio,
				dAreaRatio,
				dContrast,
				dROIDefectDistance,
				iLeftDistance, iTopDistance, iRightDistance, iBottomDistance,
				lNoPointConnectedtoROI,
				dGradient,
				dDistanceX,
				dDistanceY
			);

			// 7-2. Blob Defect Shape Fueture Part.2 (sBlobDefectShapeFeature2)
			CString sBlobDefectShapeFeature2;

			/// Circularity (dCircularity)
			double dCircularity;
			Circularity(HSelectedBlob, &HParam1);
			dCircularity = HParam1[0].D();

			/// Compactness (dCompactness)
			double dCompactness;
			Compactness(HSelectedBlob, &HParam1);
			dCompactness = HParam1[0].D();

			/// Contlength (dContlength)
			double dContlength;
			Contlength(HSelectedBlob, &HParam1);
			dContlength = HParam1[0].D();

			/// Convexity (dConvexity)
			double dConvexity;
			Convexity(HSelectedBlob, &HParam1);
			dConvexity = HParam1[0].D();

			/// Rectangularity (dRectangularity)
			double dRectangularity;
			Rectangularity(HSelectedBlob, &HParam1);
			dRectangularity = HParam1[0].D();

			/// EclipticAxis ra (dRa)
			/// EclipticAxis rb (dRb)
			/// EclipticAxis phi (dPhi)
			double dPhi;
			EllipticAxis(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dRa = HParam1[0].D();
			dRb = HParam2[0].D();
			dPhi = HParam3[0].D();

			/// Eccentricity anisometry (dAnisometry)
			/// Eccentricity bulkiness (dBulkiness)
			/// Eccentricity struct factor (dStructfactor)
			double dAnisometry, dBulkiness, dStructfactor;
			Eccentricity(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dAnisometry = HParam1[0].D();
			dBulkiness = HParam2[0].D();
			dStructfactor = HParam3[0].D();

			/// Outer radius (dOuterRadius)
			double dOuterRadius;
			SmallestCircle(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dOuterRadius = HParam3[0].D();

			/// Inner radius (dInnerRadius)
			double dInnerRadius;
			InnerCircle(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dInnerRadius = HParam3[0].D();

			/// Inner width (dInnerWidth)
			/// Inner Height (dInnerHeight)
			double dInnerWidth, dInnerHeight;
			InnerRectangle1(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dInnerWidth = HParam4[0].L() - HParam2[0].L() + 1;
			dInnerHeight = HParam3[0].L() - HParam1[0].L() + 1;

			// 7-2. Blob Defect Shape Fueture Part.2 End
			sBlobDefectShapeFeature2.Format("%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf",
				dCircularity,
				dCompactness,
				dContlength,
				dConvexity,
				dRectangularity,
				dRa, dRb, dPhi,
				dAnisometry, dBulkiness, dStructfactor,
				dOuterRadius,
				dInnerRadius,
				dInnerWidth, dInnerHeight
			);

			// 7-3. Blob Defect Shape Fueture Part.3 (sBlobDefectShapeFeature3)
			CString sBlobDefectShapeFeature3;

			/// Roundness Distance Mean (dDistanceMean)
			/// Roundness Distance Deviation (dDistanceDeviation)
			/// Roundness (dRoundness)
			/// Roundness Number of Sides (dNumSides)
			double dDistanceMean, dDistanceDeviation, dRoundness, dNumSides;
			Roundness(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dDistanceMean = HParam1[0].D();
			dDistanceDeviation = HParam2[0].D();
			dRoundness = HParam3[0].D();
			dNumSides = HParam4[0].D();

			/// Connect Number (dConnectNum)
			/// Holes Number (dHolesNum)
			double dConnectNum, dHolesNum;
			ConnectAndHoles(HSelectedBlob, &HParam1, &HParam2);
			dConnectNum = HParam1[0].L();
			dHolesNum = HParam2[0].L();

			/// Area Holes (dAreaHoles)
			double dAreaHoles;
			AreaHoles(HSelectedBlob, &HParam1);
			dAreaHoles = HParam1[0].L();

			/// Max Diameter (dMaxDiameter)
			double dMaxDiameter;
			DiameterRegion(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			dMaxDiameter = HParam5[0].D();

			/// Orientation (dOrientation)
			double dOrientation;
			OrientationRegion(HSelectedBlob, &HParam1);
			dOrientation = HParam1[0].D();

			/// Number of Euler (dEulerNumber)
			double dEulerNumber;
			EulerNumber(HSelectedBlob, &HParam1);
			dEulerNumber = HParam1[0].L();

			/// Smallest Rectangle Phi (dRect2Phi)
			/// Smallest Rectangle Length1 (dRect2Lengn1)
			/// Smallest Rectangle Length2 (dRect2Lengn2)
			double dRect2Phi, dRect2Lengn1, dRect2Lengn2;
			SmallestRectangle2(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			dRect2Phi = HParam3[0].D();
			dRect2Lengn1 = HParam4[0].D();
			dRect2Lengn2 = HParam5[0].D();

			// 7-3. Blob Defect Shape Fueture Part.3 End
			sBlobDefectShapeFeature3.Format("%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf",
				dDistanceMean, dDistanceDeviation, dRoundness, dNumSides,
				dConnectNum, dHolesNum,
				dAreaHoles,
				dMaxDiameter,
				dOrientation,
				dEulerNumber,
				dRect2Phi,
				dRect2Lengn1,
				dRect2Lengn2
			);

			// 7-4. Blob Defect Shape Fueture Part.4 (sBlobDefectShapeFeature4)
			CString sBlobDefectShapeFeature4;

			/// Moments m11 (dMomentsM11)
			/// Moments m20 (dMomentsM20)
			/// Moments m02 (dMomentsM02)
			/// Moments ia (dMomentsIa)
			/// Moments ib (dMomentsIb)
			double dMomentsM11, dMomentsM20, dMomentsM02, dMomentsIa, dMomentsIb;
			MomentsRegion2nd(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			dMomentsM11 = HParam1[0].D();
			dMomentsM20 = HParam2[0].D();
			dMomentsM02 = HParam3[0].D();
			dMomentsIa = HParam4[0].D();
			dMomentsIb = HParam5[0].D();

			/// Moments m11 Invar (dMomentsM11Invar)
			/// Moments m20 Invar (dMomentsM20Invar)
			/// Moments m02 Invar (dMomentsM02Invar)
			double dMomentsM11Invar, dMomentsM20Invar, dMomentsM02Invar;
			MomentsRegion2ndInvar(HSelectedBlob, &HParam1, &HParam2, &HParam3);
			dMomentsM11Invar = HParam1[0].D();
			dMomentsM20Invar = HParam2[0].D();
			dMomentsM02Invar = HParam3[0].D();

			/// Moments phi1 (dMomentsPhi1)
			/// Moments phi2 (dMomentsPhi2)
			double dMomentsPhi1, dMomentsPhi2;
			MomentsRegion2ndRelInvar(HSelectedBlob, &HParam1, &HParam2);
			dMomentsPhi1 = HParam1[0].D();
			dMomentsPhi2 = HParam2[0].D();

			///	Moments m21 (dMomentsM21)
			/// Moments m12 (dMomentsM12)
			/// Moments m03 (dMomentsM03)
			/// Moments m30 (dMomentsM30)
			double dMomentsM21, dMomentsM12, dMomentsM03, dMomentsM30;
			MomentsRegion3rd(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dMomentsM21 = HParam1[0].D();
			dMomentsM12 = HParam2[0].D();
			dMomentsM03 = HParam3[0].D();
			dMomentsM30 = HParam4[0].D();

			/// Moments m21 Invar (dMomentsM21Invar)
			/// Moments m12 Invar (dMomentsM12Invar)
			/// Moments m03 Invar (dMomentsM03Invar)
			/// Moments m30 Invar (dMomentsM30Invar)
			double dMomentsM21Invar, dMomentsM12Invar, dMomentsM03Invar, dMomentsM30Invar;
			MomentsRegion3rdInvar(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dMomentsM21Invar = HParam1[0].D();
			dMomentsM12Invar = HParam2[0].D();
			dMomentsM03Invar = HParam3[0].D();
			dMomentsM30Invar = HParam4[0].D();

			/// Moments i1 (dMomentsI1)
			/// Moments i2 (dMomentsI2)
			/// Moments i3 (dMomentsI3)
			/// Moments i4 (dMomentsI4)
			double dMomentsI1, dMomentsI2, dMomentsI3, dMomentsI4;
			MomentsRegionCentral(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dMomentsI1 = HParam1[0].D();
			dMomentsI2 = HParam2[0].D();
			dMomentsI3 = HParam3[0].D();
			dMomentsI4 = HParam4[0].D();

			/// Moments psi1 (dMomentsPsi1)
			/// Moments psi2 (dMomentsPsi2)
			/// Moments psi3 (dMomentsPsi3)
			/// Moments psi4 (dMomentsPsi4)
			double dMomentsPsi1, dMomentsPsi2, dMomentsPsi3, dMomentsPsi4;
			MomentsRegionCentralInvar(HSelectedBlob, &HParam1, &HParam2, &HParam3, &HParam4);
			dMomentsPsi1 = HParam1[0].D();
			dMomentsPsi2 = HParam2[0].D();
			dMomentsPsi3 = HParam3[0].D();
			dMomentsPsi4 = HParam4[0].D();

			// 7-4. Blob Defect Shape Fueture Part.4 End
			sBlobDefectShapeFeature4.Format("%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf",
				dMomentsM11, dMomentsM20, dMomentsM02, dMomentsIa, dMomentsIb,
				dMomentsM11Invar, dMomentsM20Invar, dMomentsM02Invar,
				dMomentsPhi1, dMomentsPhi2,
				dMomentsM21, dMomentsM12, dMomentsM03, dMomentsM30,
				dMomentsM21Invar, dMomentsM12Invar, dMomentsM03Invar, dMomentsM30Invar,
				dMomentsI1, dMomentsI2, dMomentsI3, dMomentsI4,
				dMomentsPsi1, dMomentsPsi2, dMomentsPsi3, dMomentsPsi4
			);

			// 8-1. Defect Condition Parameter Part.1 (sDefectConditioinParameter1)
			CString sDefectConditioinParameter1;

			/// Used Area (iUsedArea)
			/// Area Min (iAreaMin)
			/// Area Max (iAreaMax)
			int iUsedArea, iAreaMin, iAreaMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionArea)
				iUsedArea = 1;
			else
				iUsedArea = 0;
			iAreaMin = TeachAlgorithmParam.m_iDefectConditionAreaMin;
			iAreaMax = TeachAlgorithmParam.m_iDefectConditionAreaMax;

			/// Used Length (iUsedLength)
			/// Length Min (iLengthMin)
			/// Length Max (iLengthMax)
			int iUsedLength, iLengthMin, iLengthMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionLength)
				iUsedLength = 1;
			else
				iUsedLength = 0;
			iLengthMin = TeachAlgorithmParam.m_iDefectConditionLengthMin;
			iLengthMax = TeachAlgorithmParam.m_iDefectConditionLengthMax;

			/// Used Width (iUsedWidth)
			/// Width Min (iWidthMin)
			/// Width Max (iWidthMax)
			int iUsedWidth, iWidthMin, iWidthMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionWidth)
				iUsedWidth = 1;
			else
				iUsedWidth = 0;
			iWidthMin = TeachAlgorithmParam.m_iDefectConditionWidthMin;
			iWidthMax = TeachAlgorithmParam.m_iDefectConditionWidthMax;

			/// Used x Length (iUsedXLength)
			/// x Length Min (iXLengthMin)
			/// x Length Max (iXLengthMax)
			int  iUsedXLength, iXLengthMin, iXLengthMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionXLength)
				iUsedXLength = 1;
			else
				iUsedXLength = 0;
			iXLengthMin = TeachAlgorithmParam.m_iDefectConditionXLengthMin;
			iXLengthMax = TeachAlgorithmParam.m_iDefectConditionXLengthMax;

			/// Used y Length (iUsedYLength)
			/// y Length Min (iYLengthMin)
			/// y Length Max (iYLengthMax)
			int iUsedYLength, iYLengthMin, iYLengthMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionYLength)
				iUsedYLength = 1;
			else
				iUsedYLength = 0;
			iYLengthMin = TeachAlgorithmParam.m_iDefectConditionYLengthMin;
			iYLengthMax = TeachAlgorithmParam.m_iDefectConditionYLengthMax;

			/// Used Orthogonal Length (iUsedOrthoLength)
			/// Reference Orthogonal Length (iOrthoLengthRefSide)
			int iUsedOrthoLength, iOrthoLengthRefSide;
			if (TeachAlgorithmParam.m_bDefectConditionOrthoLength)
				iUsedOrthoLength = 1;
			else
				iUsedOrthoLength = 0;
			iOrthoLengthRefSide = TeachAlgorithmParam.m_iOrthoLengthRefSide;

			/// Used GV Mean (iUsedGVMean)
			/// GV Mean Min (iGVMeanMin)
			/// GV Mean Max (iGVMeanMax)
			int iUsedGVMean, iGVMeanMin, iGVMeanMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionMean)
				iUsedGVMean = 1;
			else
				iUsedGVMean = 0;
			iGVMeanMin = TeachAlgorithmParam.m_iDefectConditionMeanMin;
			iGVMeanMax = TeachAlgorithmParam.m_iDefectConditionMeanMax;

			/// Used GV Stdev (iUsedGVStdev)
			/// GV Stdev Min (dGVStdevMin)
			/// GV Stdev Max (dGVStdevMax)
			int iUsedGVStdev;
			double dGVStdevMin, dGVStdevMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionStdev)
				iUsedGVStdev = 1;
			else
				iUsedGVStdev = 0;
			dGVStdevMin = TeachAlgorithmParam.m_dDefectConditionStdevMin;
			dGVStdevMax = TeachAlgorithmParam.m_dDefectConditionStdevMax;

			/// Used Length / Width Ratio (iUsedLengthWidthRatio)
			/// Length / Width Ratio Min (dLengthWidthRatioMin)
			/// Length / Width Ratio Max (dLengthWidthRatioMax)
			int iUsedLengthWidthRatio;
			double dLengthWidthRatioMin, dLengthWidthRatioMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionAnisometry)
				iUsedLengthWidthRatio = 1;
			else
				iUsedLengthWidthRatio = 0;
			dLengthWidthRatioMin = TeachAlgorithmParam.m_dDefectConditionAnisometryMin;
			dLengthWidthRatioMax = TeachAlgorithmParam.m_dDefectConditionAnisometryMax;

			// 8-1. Defect Condition Parameter Part.1 End
			sDefectConditioinParameter1.Format("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%.1lf\t%.1lf\t%d\t%.1lf\t%.1lf",
				iUsedArea, iAreaMin, iAreaMax,
				iUsedLength, iLengthMin, iLengthMax,
				iUsedWidth, iWidthMin, iWidthMax,
				iUsedXLength, iXLengthMin, iXLengthMax,
				iUsedYLength, iYLengthMin, iYLengthMax,
				iUsedOrthoLength, iOrthoLengthRefSide,
				iUsedGVMean, iGVMeanMin, iGVMeanMax,
				iUsedGVStdev, dGVStdevMin, dGVStdevMax,
				iUsedLengthWidthRatio, dLengthWidthRatioMin, dLengthWidthRatioMax
			);

			// 8-2. Defect Condition Parameter Part.2 (sDefectConditioinParameter2)
			CString sDefectConditioinParameter2;

			/// Used Circlularity (iUsedCircularity)
			/// Circlularity Min (dCircularityMin)
			/// Circlularity Max (dCircularityMax)
			int iUsedCircularity;
			double dCircularityMin, dCircularityMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionCircularity)
				iUsedCircularity = 1;
			else
				iUsedCircularity = 0;
			dCircularityMin = TeachAlgorithmParam.m_dDefectConditionCircularityMin;
			dCircularityMax = TeachAlgorithmParam.m_dDefectConditionCircularityMax;

			/// Used Rectangularity (iUsedRectangularity)
			/// Rectangularity Min (dRectangularityMin)
			/// Rectangularity Max (dRectangularityMax)
			int iUsedRectangularity;
			double dRectangularityMin, dRectangularityMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionRectangularity)
				iUsedRectangularity = 1;
			else
				iUsedRectangularity = 0;
			dRectangularityMin = TeachAlgorithmParam.m_dDefectConditionRectangularityMin;
			dRectangularityMax = TeachAlgorithmParam.m_dDefectConditionRectangularityMax;

			/// Used Convexity (iUsedConvexity)
			/// Convexity Min (dConvexityMin)
			/// Convexity Max (dConvexityMax)
			int iUsedConvexity;
			double dConvexityMin, dConvexityMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionConvexity)
				iUsedConvexity = 1;
			else
				iUsedConvexity = 0;
			dConvexityMin = TeachAlgorithmParam.m_dDefectConditionConvexityMin;
			dConvexityMax = TeachAlgorithmParam.m_dDefectConditionConvexityMax;

			/// Used Major / Minor Axis Ratio (iUsedMajorMinorAxisRatio)
			/// Major / Minor Axis Ratio Min (dMajorMinorAxisRatioMin)
			/// Major / Minor Axis Ratio Max (dMajorMinorAxisRatioMax)
			int iUsedMajorMinorAxisRatio;
			double dMajorMinorAxisRatioMin, dMajorMinorAxisRatioMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionEllipseRatio)
				iUsedMajorMinorAxisRatio = 1;
			else
				iUsedMajorMinorAxisRatio = 0;
			dMajorMinorAxisRatioMin = TeachAlgorithmParam.m_dDefectConditionEllipseRatioMin;
			dMajorMinorAxisRatioMax = TeachAlgorithmParam.m_dDefectConditionEllipseRatioMax;

			/// Used Area Ratio (iUsedAreaRatio)
			/// Area Ratio Min (iAreaRatioMin)
			/// Area Ratio Max (iAreaRatioMax)
			int iUsedAreaRatio, iAreaRatioMin, iAreaRatioMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionAreaRatio)
				iUsedAreaRatio = 1;
			else
				iUsedAreaRatio = 0;
			iAreaRatioMin = TeachAlgorithmParam.m_iDefectConditionAreaRatioMin;
			iAreaRatioMax = TeachAlgorithmParam.m_iDefectConditionAreaRatioMax;

			/// Used Contrast (iUsedContrast)
			/// Contrast Min (iContrastMin)
			/// Contrast Max (iContrastMax)
			int iUsedContrast, iContrastMin, iContrastMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionContrast)
				iUsedContrast = 1;
			else
				iUsedContrast = 0;
			iContrastMin = TeachAlgorithmParam.m_iDefectConditionContrastMin;
			iContrastMax = TeachAlgorithmParam.m_iDefectConditionContrastMax;

			// 8-2. Defect Condition Parameter Part.2 End
			sDefectConditioinParameter2.Format("%d\t%.1lf\t%.1lf\t%d\t%.1lf\t%.1lf\t%d\t%.1lf\t%.1lf\t%d\t%.1lf\t%.1lf\t%d\t%d\t%d\t%d\t%d\t%d",
				iUsedCircularity, dCircularityMin, dCircularityMax,
				iUsedRectangularity, dRectangularityMin, dRectangularityMax,
				iUsedConvexity, dConvexityMin, dConvexityMax,
				iUsedMajorMinorAxisRatio, dMajorMinorAxisRatioMin, dMajorMinorAxisRatioMax,
				iUsedAreaRatio, iAreaRatioMin, iAreaRatioMax,
				iUsedContrast, iContrastMin, iContrastMax
			);

			// 8-3. Defect Condition Parameter Part.3 (sDefectConditioinParameter3)
			CString sDefectConditioinParameter3;

			/// Used Distance from ROI outer to Defect (iUsedROIDefectDistance)
			/// Distance from ROI outer to Defect (iROIDefectDistance)
			int iUsedROIDefectDistance, iROIDefectDistance;
			if (TeachAlgorithmParam.m_bUseDefectConditionOuterDist)
				iUsedROIDefectDistance = 1;
			else
				iUsedROIDefectDistance = 0;
			iROIDefectDistance = TeachAlgorithmParam.m_iDefectConditionOuterDist;

			/// Used Distance from ROI inner to Defect (iUsedROIDefectInnerDistance)
			/// Distance from ROI inner to Defect (iROIDefectInnerDistance)
			int iUsedROIDefectInnerDistance, iROIDefectInnerDistance;
			if (TeachAlgorithmParam.m_bUseDefectConditionInnerDist)
				iUsedROIDefectInnerDistance = 1;
			else
				iUsedROIDefectInnerDistance = 0;
			iROIDefectInnerDistance = TeachAlgorithmParam.m_iDefectConditionInnerDist;

			/// Used Number of blobs (iUsedNoBlobs)
			/// Number of blobs Min (iNoBlobsMin)
			/// Number of blobs Max (iNoBlobsMax)
			int iUsedNoBlobs, iNoBlobsMin, iNoBlobsMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionBlob)
				iUsedNoBlobs = 1;
			else
				iUsedNoBlobs = 0;
			iNoBlobsMin = TeachAlgorithmParam.m_iDefectConditionBlobMin;
			iNoBlobsMax = TeachAlgorithmParam.m_iDefectConditionBlobMax;

			/// Used Connected to ROI Each Position (iUsedConnectedToROI)
			int iUsedConnectedToROI;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryConnection)
				iUsedConnectedToROI = 1;
			else
				iUsedConnectedToROI = 0;

			/// Used Connected to ROI Top (iUsedConnectedToROITop)
			/// Connected to ROI Top Margin (iConnectedToROITopMargin)
			int iUsedConnectedToROITop, iConnectedToROITopMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginTop)
				iUsedConnectedToROITop = 1;
			else
				iUsedConnectedToROITop = 0;
			iConnectedToROITopMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginTop;

			/// Used Connected to ROI Bottom (iUsedConnectedToROIBottom)
			/// Connected to ROI Bottom Margin (iConnectedToROIBottomMargin)
			int iUsedConnectedToROIBottom, iConnectedToROIBottomMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginBottom)
				iUsedConnectedToROIBottom = 1;
			else
				iUsedConnectedToROIBottom = 0;
			iConnectedToROIBottomMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginBottom;

			/// Used Connected to ROI Horizontal Center (iUsedConnectedToROIHorizontalCenter)
			/// Connected to ROI Horizontal Center Margin (iConnectedToROIHorizontalCenterMargin)
			int iUsedConnectedToROIHorizontalCenter, iConnectedToROIHorizontalCenterMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginHCenter)
				iUsedConnectedToROIHorizontalCenter = 1;
			else
				iUsedConnectedToROIHorizontalCenter = 0;
			iConnectedToROIHorizontalCenterMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginHCenter;

			/// Used Connected to ROI Left (iUsedConnectedToROILeft)
			/// Connected to ROI Left Margin (iConnectedToROILeftMargin)
			int iUsedConnectedToROILeft, iConnectedToROILeftMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginLeft)
				iUsedConnectedToROILeft = 1;
			else
				iUsedConnectedToROILeft = 0;
			iConnectedToROILeftMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginLeft;

			/// Used Connected to ROI Right (iUsedConnectedToROIRight)
			///	Connected to ROI Right Margin (iConnectedToROIRightMargin)
			int iUsedConnectedToROIRight, iConnectedToROIRightMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginRight)
				iUsedConnectedToROIRight = 1;
			else
				iUsedConnectedToROIRight = 0;
			iConnectedToROIRightMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginRight;

			/// Used Connected to ROI Vertical Center (iUsedConnectedToROIVerticalCenter)
			/// Connected to ROI Vertical Center Margin (iConnectedToROIVerticalCenterMargin)
			int iUsedConnectedToROIVerticalCenter, iConnectedToROIVerticalCenterMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryMarginVCenter)
				iUsedConnectedToROIVerticalCenter = 1;
			else
				iUsedConnectedToROIVerticalCenter = 0;
			iConnectedToROIVerticalCenterMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginVCenter;

			/// Used Connected to ROI Any Position (iUsedConnectedToROIAnyPosition)
			/// Connected to ROI Any Position Margin (iConnectedToROIAnyPositionMargin)
			int iUsedConnectedToROIAnyPosition, iConnectedToROIAnyPositionMargin;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryConnectionAny)
				iUsedConnectedToROIAnyPosition = 1;
			else
				iUsedConnectedToROIAnyPosition = 0;
			iConnectedToROIAnyPositionMargin = TeachAlgorithmParam.m_iDefectConditionBoundaryMarginAny;

			/// Used Connected to ROI 2 Sides (iUsedConnectedToROI2Sides)
			int iUsedConnectedToROI2Sides;
			if (TeachAlgorithmParam.m_bDefectConditionBoundaryConnection2Side)
				iUsedConnectedToROI2Sides = 1;
			else
				iUsedConnectedToROI2Sides = 0;

			/// Used Connected to ROI Inner and Outer (iUsedConnectedToROIInnerOuter)
			int iUsedConnectedToROIInnerOuter;
			if (TeachAlgorithmParam.m_bDefectConditionInnerOuterBoundaryConnection)
				iUsedConnectedToROIInnerOuter = 1;
			else
				iUsedConnectedToROIInnerOuter = 0;

			// 8-3. Defect Condition Parameter Part.3 End
			sDefectConditioinParameter3.Format("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
				iUsedROIDefectDistance, iROIDefectDistance,
				iUsedROIDefectInnerDistance, iROIDefectInnerDistance,
				iUsedNoBlobs, iNoBlobsMin, iNoBlobsMax,
				iUsedConnectedToROI,
				iUsedConnectedToROITop, iConnectedToROITopMargin,
				iUsedConnectedToROIBottom, iConnectedToROIBottomMargin,
				iUsedConnectedToROIHorizontalCenter, iConnectedToROIHorizontalCenterMargin,
				iUsedConnectedToROILeft, iConnectedToROILeftMargin,
				iUsedConnectedToROIRight, iConnectedToROIRightMargin,
				iUsedConnectedToROIVerticalCenter, iConnectedToROIVerticalCenterMargin,
				iUsedConnectedToROIAnyPosition, iConnectedToROIAnyPositionMargin,
				iUsedConnectedToROI2Sides,
				iUsedConnectedToROIInnerOuter
			);

			// 8-4. Defect Condition Parameter Part.4 (sDefectConditioinParameter4)
			CString sDefectConditioinParameter4;

			/// Used Boundary Gradient (iUsedBoundaryGradient)
			/// Boundary Gradient Min (iBoundaryGradientMin)
			/// Boundary Gradient Max (iBoundaryGradientMax)
			int iUsedBoundaryGradient, iBoundaryGradientMin, iBoundaryGradientMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionGrad)
				iUsedBoundaryGradient = 1;
			else
				iUsedBoundaryGradient = 0;
			iBoundaryGradientMin = TeachAlgorithmParam.m_iDefectConditionGradMin;
			iBoundaryGradientMax = TeachAlgorithmParam.m_iDefectConditionGradMax;

			/// Used Included Holes (iUsedIncludedHoles)
			/// Included Holes Area (iIncludedHolesArea)
			int iUsedIncludedHoles, iIncludedHolesArea;
			if (TeachAlgorithmParam.m_bUseDefectConditionHole)
				iUsedIncludedHoles = 1;
			else
				iUsedIncludedHoles = 0;
			iIncludedHolesArea = TeachAlgorithmParam.m_iDefectConditionHoleArea;

			/// Used Sum of x length (iUsedXLengthSum)
			/// Sum of x length Min (iXLengthSumMin)
			/// Sum of x length Max (iXLengthSumMax)
			int iUsedXLengthSum, iXLengthSumMin, iXLengthSumMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionXLengthSum)
				iUsedXLengthSum = 1;
			else
				iUsedXLengthSum = 0;
			iXLengthSumMin = TeachAlgorithmParam.m_iDefectConditionXLengthSumMin;
			iXLengthSumMax = TeachAlgorithmParam.m_iDefectConditionXLengthSumMax;

			/// Used Sum of y length (iUsedYLengthSum)
			/// Sum of y length Min (iYLengthSumMin)
			/// Sum of y length Max (iYLengthSumMax)
			int iUsedYLengthSum, iYLengthSumMin, iYLengthSumMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionYLengthSum)
				iUsedYLengthSum = 1;
			else
				iUsedYLengthSum = 0;
			iYLengthSumMin = TeachAlgorithmParam.m_iDefectConditionYLengthSumMin;
			iYLengthSumMax = TeachAlgorithmParam.m_iDefectConditionYLengthSumMax;

			/// Used Shift x from ROI Center to Defect Center (iUsedShiftX)
			/// Shift x from ROI Center to Defect Center Min (iShiftXMin)
			/// Shift x from ROI Center to Defect Center Max (iShiftXMax)
			int iUsedShiftX, iShiftXMin, iShiftXMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionShiftX)
				iUsedShiftX = 1;
			else
				iUsedShiftX = 0;
			iShiftXMin = TeachAlgorithmParam.m_iDefectConditionShiftXMin;
			iShiftXMax = TeachAlgorithmParam.m_iDefectConditionShiftXMax;

			/// Used Shift y from ROI Center to Defect Center (iUsedShiftY)
			/// Shift y from ROI Center to Defect Center Min (iShiftYMin)
			/// Shift y from ROI Center to Defect Center Max (iShiftYMax)
			int iUsedShiftY, iShiftYMin, iShiftYMax;
			if (TeachAlgorithmParam.m_bUseDefectConditionShiftY)
				iUsedShiftY = 1;
			else
				iUsedShiftY = 0;
			iShiftYMin = TeachAlgorithmParam.m_iDefectConditionShiftYMin;
			iShiftYMax = TeachAlgorithmParam.m_iDefectConditionShiftYMax;

			/// Used Zero Area Detect (iUsedZeroAreaDetect)
			int iUsedZeroAreaDetect;
			if (TeachAlgorithmParam.m_bUseDefectConditionUseZeroArea)
				iUsedZeroAreaDetect = 1;
			else
				iUsedZeroAreaDetect = 0;

			/// Used Dilation Area Detect (iUsedDilationAreaDetect)
			/// Dilation value (iDilationValue)
			int iUsedDilationAreaDetect, iDilationValue;
			if (TeachAlgorithmParam.m_bDefectConditionDilation)
				iUsedDilationAreaDetect = 1;
			else
				iUsedDilationAreaDetect = 0;
			iDilationValue = TeachAlgorithmParam.m_iDefectConditionDilationSize;

			// 8-4. Defect Condition Parameter Part.4 End
			sDefectConditioinParameter4.Format("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
				iUsedBoundaryGradient, iBoundaryGradientMin, iBoundaryGradientMax,
				iUsedIncludedHoles, iIncludedHolesArea,
				iUsedXLengthSum, iXLengthSumMin, iXLengthSumMax,
				iUsedYLengthSum, iYLengthSumMin, iYLengthSumMax,
				iUsedShiftX, iShiftXMin, iShiftXMax,
				iUsedZeroAreaDetect,
				iUsedDilationAreaDetect,
				iDilationValue
			);

			// 9. LGIT SVM Result (sSVMResult)
			CString sSVMResult;

			/// SVM Result (sSVMClass)
			HObject HSVMResultRgn;
			CString sSVMClass = "";

			// 10. LGIT AI Classification Result (sSecondaryResult)
			CString sSecondaryResult;

			/// Classification Result (sSecondaryClass)
			/// Classification Max Score (dClassificationMaxScore)
			HObject HSecondaryResultRgn;
			int iSecondaryClass = -1;
			CString sSecondaryClass = "";
			double dSecondaryMaxScore = 0;

			// 11. LGIT AI Segmentation Result (sTertiaryResult)
			CString sTertiaryResult;

			/// Segmentation Area (lSegmentationArea)
			/// Segmentation Length (dSegmentationLength)
			/// Segmentation Width (dSegmentationWidth)
			/// Segmentation Contrast (dSegmentationContrast)
			/// Segmentation Size Spec Min (iSegmentationSizeSpecMin)
			/// Segmentation Size Spec Mid (iSegmentationSizeSpecMid)
			/// Segmentation Size Spec Max (iSegmentationSizeSpecMax)
			/// Segmentation Count Spec Min to Mid (iSegmentationCountSpecMinToMid)
			/// Segmentation Count Spec Mid to Max (iSegmentationCountSpecMidToMax)
			/// Segmentation Count Spec Max (iSegmentationCountSpecMax)
			HObject HTertiaryResultRgn, SelectedTertiaryBlob, HConnIntersectRgn;
			long lNoTertiaryBlob;
			long lSegmentationArea;
			double dSegmentationLength, dSegmentationWidth, dSegmentationContrast;
			double dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax;
			int iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax;

			// Total Variation Feature
			CString sVariationFeatureLog;

			if (result.svm_result->result && !AIService::ImageUtile::HasImageData(result.svm_result->blob_ng_mask))
			{
				sSVMClass = "Good";
				// 9. LGIT SVM Result End
				sSVMResult.Format("%s",
					sSVMClass
				);

				sSecondaryClass = "Good";
				iSecondaryClass = -1;
				dSecondaryMaxScore = 0;

				// 10. LGIT AI Classification Result End
				sSecondaryResult.Format("%s\t%.3lf",
					sSecondaryClass, dSecondaryMaxScore
				);

				lSegmentationArea = 0;
				dSegmentationLength = 0;
				dSegmentationWidth = 0;
				dSegmentationContrast = 0;

				dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][iDefectType];
				dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][iDefectType];
				dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][iDefectType];
				iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][iDefectType];
				iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][iDefectType];
				iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][iDefectType];

				// 11. LGIT AI Segmentation Result End
				sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
					lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
					dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
				);

				// Total Variation Feature
				sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
					sRunInfo,
					sInspectROIInfo,
					sVariationParameterFeature,
					sReferenceROIVariationFeature,
					sCurrentROIVariationFeature,
					sBlobVariationFeature,
					sBlobDefectShapeFeature1,
					sBlobDefectShapeFeature2,
					sBlobDefectShapeFeature3,
					sBlobDefectShapeFeature4,
					sDefectConditioinParameter1,
					sDefectConditioinParameter2,
					sDefectConditioinParameter3,
					sDefectConditioinParameter4,
					sSVMResult,
					sSecondaryResult,
					sTertiaryResult
				);

				if (!Struct_PreferenceStruct.m_bUseResultLogThread)
					NewSaveDefectVariationFeatureLog(strLogSavePath, sVariationFeatureLog);
				else
				{
					RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
					pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
					pSaveLogThreadIDParam_Edge->strPath = strLogSavePath;
					pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
					pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
					pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
					m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
				}

				continue;
			}
			else if (result.svm_result->result && AIService::ImageUtile::HasImageData(result.svm_result->blob_ng_mask))
			{
				HSVMResultRgn = AIService::ImageUtile::Mask2HRegion(result.svm_result->blob_ng_mask);

				HObject HSVMConnRgn, HSVMSelectedBlob;
				long lNoSVMDefectCount;
				Connection(HSVMResultRgn, &HSVMConnRgn);
				CountObj(HSVMConnRgn, &CountDefect);
				lNoSVMDefectCount = CountDefect[0].I();

				for (int iSVMBlobIdx = 0; iSVMBlobIdx < lNoSVMDefectCount; iSVMBlobIdx++)
				{
					SelectObj(HSVMConnRgn, &HSVMSelectedBlob, iSVMBlobIdx + 1);
					Intersection(HSelectedBlob, HSVMSelectedBlob, &HIntersectRgn);
					if (m_pGFunction->ValidHRegion(HIntersectRgn))
					{
						sSVMClass = "NG";
						// 9. LGIT SVM Result End
						sSVMResult.Format("%s",
							sSVMClass
						);

						if (iSVMBlobIdx >= result.secondary_result->blob_count)
						{
							sSecondaryClass = "Error";
							iSecondaryClass = -1;
							dSecondaryMaxScore = -1;

							// 10. LGIT AI Classification Result End
							sSecondaryResult.Format("%s\t%.3lf",
								sSecondaryClass, dSecondaryMaxScore
							);

							lSegmentationArea = -1;
							dSegmentationLength = -1;
							dSegmentationWidth = -1;
							dSegmentationContrast = -1;

							dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][iDefectType];
							dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][iDefectType];
							dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][iDefectType];
							iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][iDefectType];
							iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][iDefectType];
							iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][iDefectType];

							// 11. LGIT AI Segmentation Result End
							sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
								lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
								dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
							);

							// Total Variation Feature
							sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
								sRunInfo,
								sInspectROIInfo,
								sVariationParameterFeature,
								sReferenceROIVariationFeature,
								sCurrentROIVariationFeature,
								sBlobVariationFeature,
								sBlobDefectShapeFeature1,
								sBlobDefectShapeFeature2,
								sBlobDefectShapeFeature3,
								sBlobDefectShapeFeature4,
								sDefectConditioinParameter1,
								sDefectConditioinParameter2,
								sDefectConditioinParameter3,
								sDefectConditioinParameter4,
								sSVMResult,
								sSecondaryResult,
								sTertiaryResult
							);

							if (!Struct_PreferenceStruct.m_bUseResultLogThread)
								NewSaveDefectVariationFeatureLog(strLogSavePath, sVariationFeatureLog);
							else
							{
								RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
								pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
								pSaveLogThreadIDParam_Edge->strPath = strLogSavePath;
								pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
								pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
								pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
								m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
							}

							continue;
						}

						if (AIService::ImageUtile::HasImageData(result.secondary_result->blob_ok_mask))
						{
							HSecondaryResultRgn = AIService::ImageUtile::Mask2HRegion(result.secondary_result->blob_ok_mask);
							Intersection(HSecondaryResultRgn, HSVMSelectedBlob, &HIntersectRgn);
							if (m_pGFunction->ValidHRegion(HIntersectRgn))
							{
								sSecondaryClass = "Good";
								iSecondaryClass = -1;
								dSecondaryMaxScore = result.secondary_result->score_list[iSVMBlobIdx];
							}
						}

						if (sSecondaryClass == "" && result.secondary_result && result.secondary_result->result)
						{
							for (const auto&[type, mask] : result.secondary_result->blob_mask_map) {
								if (!AIService::ImageUtile::HasImageData(mask)) { continue; }
								HSecondaryResultRgn = AIService::ImageUtile::Mask2HRegion(mask);
								Intersection(HSecondaryResultRgn, HSVMSelectedBlob, &HIntersectRgn);
								if (m_pGFunction->ValidHRegion(HIntersectRgn))
								{
									sSecondaryClass = m_strDefectName[type];
									iSecondaryClass = type;
									dSecondaryMaxScore = result.secondary_result->score_list[iSVMBlobIdx];

									break;
								}
							}
						}

						// 10. LGIT AI Classification Result End
						sSecondaryResult.Format("%s\t%.3lf",
							sSecondaryClass, dSecondaryMaxScore
						);


						if (iSecondaryClass >= 0 && result.tertiary_result && result.tertiary_result->result)
						{
							for (const auto&[type, mask] : result.tertiary_result->blob_mask_map) {
								if (iSecondaryClass != type) { continue; }
								if (!AIService::ImageUtile::HasImageData(mask))
								{
									lSegmentationArea = 0;
									dSegmentationLength = 0;
									dSegmentationWidth = 0;
									dSegmentationContrast = 0;

									dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][type];
									dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][type];
									dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][type];
									iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][type];
									iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][type];
									iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][type];

									// 11. LGIT AI Segmentation Result End
									sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
										lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
										dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
									);

									// Total Variation Feature
									sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
										sRunInfo,
										sInspectROIInfo,
										sVariationParameterFeature,
										sReferenceROIVariationFeature,
										sCurrentROIVariationFeature,
										sBlobVariationFeature,
										sBlobDefectShapeFeature1,
										sBlobDefectShapeFeature2,
										sBlobDefectShapeFeature3,
										sBlobDefectShapeFeature4,
										sDefectConditioinParameter1,
										sDefectConditioinParameter2,
										sDefectConditioinParameter3,
										sDefectConditioinParameter4,
										sSVMResult,
										sSecondaryResult,
										sTertiaryResult
									);

									if (!Struct_PreferenceStruct.m_bUseResultLogThread)
										NewSaveDefectVariationFeatureLog(strLogSavePath, sVariationFeatureLog);
									else
									{
										RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
										pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
										pSaveLogThreadIDParam_Edge->strPath = strLogSavePath;
										pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
										pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
										pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
										m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
									}
									break;
								}
								else
								{
									HTertiaryResultRgn = AIService::ImageUtile::Mask2HRegion(mask);
									Connection(HTertiaryResultRgn, &HConnIntersectRgn);
									CountObj(HConnIntersectRgn, &CountDefect);
									lNoTertiaryBlob = CountDefect[0].I();
									for (int iTertiaryBlobIdx = 0; iTertiaryBlobIdx < lNoTertiaryBlob; iTertiaryBlobIdx++)
									{
										SelectObj(HConnIntersectRgn, &SelectedTertiaryBlob, iTertiaryBlobIdx + 1);
										Intersection(HTertiaryResultRgn, HSVMSelectedBlob, &HIntersectRgn);
										if (m_pGFunction->ValidHRegion(HIntersectRgn))
										{
											AreaCenter(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3);
											lSegmentationArea = HParam1[0].L();

											DiameterRegion(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
											dSegmentationLength = HParam5.D();

											InnerCircle(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3);
											dSegmentationWidth = HParam3.D() * 2.0;

											FillUp(SelectedTertiaryBlob, &HFillupRgn);
											DilationCircle(HFillupRgn, &HFillupRgn, 3.5);
											Intensity(HFillupRgn, HVariationInspectRgn, &HParam1, &HParam2);
											dSegmentationContrast = HParam1.D();

											dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][type];
											dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][type];
											dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][type];
											iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][type];
											iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][type];
											iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][type];

											// 11. LGIT AI Segmentation Result End
											sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
												lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
												dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
											);

											// Total Variation Feature
											sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
												sRunInfo,
												sInspectROIInfo,
												sVariationParameterFeature,
												sReferenceROIVariationFeature,
												sCurrentROIVariationFeature,
												sBlobVariationFeature,
												sBlobDefectShapeFeature1,
												sBlobDefectShapeFeature2,
												sBlobDefectShapeFeature3,
												sBlobDefectShapeFeature4,
												sDefectConditioinParameter1,
												sDefectConditioinParameter2,
												sDefectConditioinParameter3,
												sDefectConditioinParameter4,
												sSVMResult,
												sSecondaryResult,
												sTertiaryResult
											);

											if (!Struct_PreferenceStruct.m_bUseResultLogThread)
												NewSaveDefectVariationFeatureLog(strLogSavePath, sVariationFeatureLog);
											else
											{
												RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
												pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
												pSaveLogThreadIDParam_Edge->strPath = strLogSavePath;
												pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
												pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
												pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
												m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
											}
										}
									}
									break;
								}
							}
						}
						else
						{
							lSegmentationArea = 0;
							dSegmentationLength = 0;
							dSegmentationWidth = 0;
							dSegmentationContrast = 0;

							dSegmentationSizeSpecMin = 0;
							dSegmentationSizeSpecMid = 0;
							dSegmentationSizeSpecMax = 0;
							iSegmentationCountSpecMinToMid = 0;
							iSegmentationCountSpecMidToMax = 0;
							iSegmentationCountSpecMax = 0;

							// 11. LGIT AI Segmentation Result End
							sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
								lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
								dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
							);

							// Total Variation Feature
							sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
								sRunInfo,
								sInspectROIInfo,
								sVariationParameterFeature,
								sReferenceROIVariationFeature,
								sCurrentROIVariationFeature,
								sBlobVariationFeature,
								sBlobDefectShapeFeature1,
								sBlobDefectShapeFeature2,
								sBlobDefectShapeFeature3,
								sBlobDefectShapeFeature4,
								sDefectConditioinParameter1,
								sDefectConditioinParameter2,
								sDefectConditioinParameter3,
								sDefectConditioinParameter4,
								sSVMResult,
								sSecondaryResult,
								sTertiaryResult
							);

							if (!Struct_PreferenceStruct.m_bUseResultLogThread)
								NewSaveDefectVariationFeatureLog(strLogSavePath, sVariationFeatureLog);
							else
							{
								RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
								pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
								pSaveLogThreadIDParam_Edge->strPath = strLogSavePath;
								pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
								pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
								pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
								m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
							}
						}

						break;
					}

					if (iSVMBlobIdx == lNoSVMDefectCount)
					{
						sSVMClass = "Good";
						// 9. LGIT SVM Result End
						sSVMResult.Format("%s",
							sSVMClass
						);

						sSecondaryClass = "Good";
						iSecondaryClass = -1;
						dSecondaryMaxScore = 0;

						// 10. LGIT AI Classification Result End
						sSecondaryResult.Format("%s\t%.3lf",
							sSecondaryClass, dSecondaryMaxScore
						);

						lSegmentationArea = 0;
						dSegmentationLength = 0;
						dSegmentationWidth = 0;
						dSegmentationContrast = 0;

						dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][iDefectType];
						dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][iDefectType];
						dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][iDefectType];
						iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][iDefectType];
						iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][iDefectType];
						iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][iDefectType];

						// 11. LGIT AI Segmentation Result End
						sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
							lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
							dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
						);

						// Total Variation Feature
						sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
							sRunInfo,
							sInspectROIInfo,
							sVariationParameterFeature,
							sReferenceROIVariationFeature,
							sCurrentROIVariationFeature,
							sBlobVariationFeature,
							sBlobDefectShapeFeature1,
							sBlobDefectShapeFeature2,
							sBlobDefectShapeFeature3,
							sBlobDefectShapeFeature4,
							sDefectConditioinParameter1,
							sDefectConditioinParameter2,
							sDefectConditioinParameter3,
							sDefectConditioinParameter4,
							sSVMResult,
							sSecondaryResult,
							sTertiaryResult
						);

						if (!Struct_PreferenceStruct.m_bUseResultLogThread)
							NewSaveDefectVariationFeatureLog(strLogSavePath, sVariationFeatureLog);
						else
						{
							RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
							pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
							pSaveLogThreadIDParam_Edge->strPath = strLogSavePath;
							pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
							pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
							pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
							m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
						}
					}
				}
			}
			else
			{
				sSVMClass = "Unused";
				// 9. LGIT SVM Result End
				sSVMResult.Format("%s",
					sSVMClass
				);

				if (iPrimaryBlobIdx >= result.secondary_result->blob_count)
				{
					sSecondaryClass = "Error";
					iSecondaryClass = -1;
					dSecondaryMaxScore = -1;

					// 10. LGIT AI Classification Result End
					sSecondaryResult.Format("%s\t%.3lf",
						sSecondaryClass, dSecondaryMaxScore
					);

					lSegmentationArea = -1;
					dSegmentationLength = -1;
					dSegmentationWidth = -1;
					dSegmentationContrast = -1;

					dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][iDefectType];
					dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][iDefectType];
					dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][iDefectType];
					iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][iDefectType];
					iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][iDefectType];
					iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][iDefectType];

					// 11. LGIT AI Segmentation Result End
					sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
						lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
						dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
					);

					// Total Variation Feature
					sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
						sRunInfo,
						sInspectROIInfo,
						sVariationParameterFeature,
						sReferenceROIVariationFeature,
						sCurrentROIVariationFeature,
						sBlobVariationFeature,
						sBlobDefectShapeFeature1,
						sBlobDefectShapeFeature2,
						sBlobDefectShapeFeature3,
						sBlobDefectShapeFeature4,
						sDefectConditioinParameter1,
						sDefectConditioinParameter2,
						sDefectConditioinParameter3,
						sDefectConditioinParameter4,
						sSVMResult,
						sSecondaryResult,
						sTertiaryResult
					);

					if (!Struct_PreferenceStruct.m_bUseResultLogThread)
						NewSaveDefectVariationFeatureLog(strLogSavePath, sVariationFeatureLog);
					else
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
						pSaveLogThreadIDParam_Edge->strPath = strLogSavePath;
						pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
						pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
						m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
					}

					continue;
				}

				if (AIService::ImageUtile::HasImageData(result.secondary_result->blob_ok_mask))
				{
					HSecondaryResultRgn = AIService::ImageUtile::Mask2HRegion(result.secondary_result->blob_ok_mask);
					Intersection(HSecondaryResultRgn, HSelectedBlob, &HIntersectRgn);
					if (m_pGFunction->ValidHRegion(HIntersectRgn))
					{
						sSecondaryClass = "Good";
						iSecondaryClass = -1;
						dSecondaryMaxScore = result.secondary_result->score_list[iPrimaryBlobIdx];
					}
				}

				if (sSecondaryClass == "" && result.secondary_result && result.secondary_result->result)
				{
					for (const auto&[type, mask] : result.secondary_result->blob_mask_map) {
						if (!AIService::ImageUtile::HasImageData(mask)) { continue; }
						HSecondaryResultRgn = AIService::ImageUtile::Mask2HRegion(mask);
						Intersection(HSecondaryResultRgn, HSelectedBlob, &HIntersectRgn);
						if (m_pGFunction->ValidHRegion(HIntersectRgn))
						{
							sSecondaryClass = m_strDefectName[type];
							iSecondaryClass = type;
							dSecondaryMaxScore = result.secondary_result->score_list[iPrimaryBlobIdx];

							break;
						}
					}
				}

				// 10. LGIT AI Classification Result End
				sSecondaryResult.Format("%s\t%.3lf",
					sSecondaryClass, dSecondaryMaxScore
				);


				if (iSecondaryClass >= 0 && result.tertiary_result && result.tertiary_result->result)
				{
					for (const auto&[type, mask] : result.tertiary_result->blob_mask_map) {
						if (iSecondaryClass != type) { continue; }
						if (!AIService::ImageUtile::HasImageData(mask))
						{
							lSegmentationArea = 0;
							dSegmentationLength = 0;
							dSegmentationWidth = 0;
							dSegmentationContrast = 0;

							dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][type];
							dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][type];
							dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][type];
							iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][type];
							iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][type];
							iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][type];

							// 11. LGIT AI Segmentation Result End
							sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
								lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
								dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
							);

							// Total Variation Feature
							sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
								sRunInfo,
								sInspectROIInfo,
								sVariationParameterFeature,
								sReferenceROIVariationFeature,
								sCurrentROIVariationFeature,
								sBlobVariationFeature,
								sBlobDefectShapeFeature1,
								sBlobDefectShapeFeature2,
								sBlobDefectShapeFeature3,
								sBlobDefectShapeFeature4,
								sDefectConditioinParameter1,
								sDefectConditioinParameter2,
								sDefectConditioinParameter3,
								sDefectConditioinParameter4,
								sSVMResult,
								sSecondaryResult,
								sTertiaryResult
							);

							if (!Struct_PreferenceStruct.m_bUseResultLogThread)
								NewSaveDefectVariationFeatureLog(strLogSavePath, sVariationFeatureLog);
							else
							{
								RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
								pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
								pSaveLogThreadIDParam_Edge->strPath = strLogSavePath;
								pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
								pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
								pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
								m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
							}
							break;
						}
						else
						{
							HTertiaryResultRgn = AIService::ImageUtile::Mask2HRegion(mask);
							Connection(HTertiaryResultRgn, &HConnIntersectRgn);
							CountObj(HConnIntersectRgn, &CountDefect);
							lNoTertiaryBlob = CountDefect[0].I();
							for (int iTertiaryBlobIdx = 0; iTertiaryBlobIdx < lNoTertiaryBlob; iTertiaryBlobIdx++)
							{
								SelectObj(HConnIntersectRgn, &SelectedTertiaryBlob, iTertiaryBlobIdx + 1);
								Intersection(SelectedTertiaryBlob, HSelectedBlob, &HIntersectRgn);
								if (m_pGFunction->ValidHRegion(HIntersectRgn))
								{
									AreaCenter(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3);
									lSegmentationArea = HParam1[0].L();

									DiameterRegion(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
									dSegmentationLength = HParam5.D();

									InnerCircle(SelectedTertiaryBlob, &HParam1, &HParam2, &HParam3);
									dSegmentationWidth = HParam3.D() * 2.0;

									FillUp(SelectedTertiaryBlob, &HFillupRgn);
									DilationCircle(HFillupRgn, &HFillupRgn, 3.5);
									Intensity(HFillupRgn, HVariationInspectRgn, &HParam1, &HParam2);
									dSegmentationContrast = HParam1.D();

									dSegmentationSizeSpecMin = m_StructCountPerAreaConditionInfo.m_dMinArea[iInspectionType][type];
									dSegmentationSizeSpecMid = m_StructCountPerAreaConditionInfo.m_dMidArea[iInspectionType][type];
									dSegmentationSizeSpecMax = m_StructCountPerAreaConditionInfo.m_dMaxArea[iInspectionType][type];
									iSegmentationCountSpecMinToMid = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[iInspectionType][type];
									iSegmentationCountSpecMidToMax = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[iInspectionType][type];
									iSegmentationCountSpecMax = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[iInspectionType][type];

									// 11. LGIT AI Segmentation Result End
									sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
										lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
										dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
									);

									// Total Variation Feature
									sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
										sRunInfo,
										sInspectROIInfo,
										sVariationParameterFeature,
										sReferenceROIVariationFeature,
										sCurrentROIVariationFeature,
										sBlobVariationFeature,
										sBlobDefectShapeFeature1,
										sBlobDefectShapeFeature2,
										sBlobDefectShapeFeature3,
										sBlobDefectShapeFeature4,
										sDefectConditioinParameter1,
										sDefectConditioinParameter2,
										sDefectConditioinParameter3,
										sDefectConditioinParameter4,
										sSVMResult,
										sSecondaryResult,
										sTertiaryResult
									);

									if (!Struct_PreferenceStruct.m_bUseResultLogThread)
										NewSaveDefectVariationFeatureLog(strLogSavePath, sVariationFeatureLog);
									else
									{
										RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
										pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
										pSaveLogThreadIDParam_Edge->strPath = strLogSavePath;
										pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
										pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
										pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
										m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
									}
								}
							}
							break;
						}
					}
				}
				else
				{
					lSegmentationArea = 0;
					dSegmentationLength = 0;
					dSegmentationWidth = 0;
					dSegmentationContrast = 0;

					dSegmentationSizeSpecMin = 0;
					dSegmentationSizeSpecMid = 0;
					dSegmentationSizeSpecMax = 0;
					iSegmentationCountSpecMinToMid = 0;
					iSegmentationCountSpecMidToMax = 0;
					iSegmentationCountSpecMax = 0;

					// 11. LGIT AI Segmentation Result End
					sTertiaryResult.Format("%d\t%.3lf\t%.3lf\t%.3lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
						lSegmentationArea, dSegmentationLength, dSegmentationWidth, dSegmentationContrast,
						dSegmentationSizeSpecMin, dSegmentationSizeSpecMid, dSegmentationSizeSpecMax, iSegmentationCountSpecMinToMid, iSegmentationCountSpecMidToMax, iSegmentationCountSpecMax
					);

					// Total Variation Feature
					sVariationFeatureLog.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
						sRunInfo,
						sInspectROIInfo,
						sVariationParameterFeature,
						sReferenceROIVariationFeature,
						sCurrentROIVariationFeature,
						sBlobVariationFeature,
						sBlobDefectShapeFeature1,
						sBlobDefectShapeFeature2,
						sBlobDefectShapeFeature3,
						sBlobDefectShapeFeature4,
						sDefectConditioinParameter1,
						sDefectConditioinParameter2,
						sDefectConditioinParameter3,
						sDefectConditioinParameter4,
						sSVMResult,
						sSecondaryResult,
						sTertiaryResult
					);

					if (!Struct_PreferenceStruct.m_bUseResultLogThread)
						NewSaveDefectVariationFeatureLog(strLogSavePath, sVariationFeatureLog);
					else
					{
						RESULT_LOG_SAVE_PARAM* pSaveLogThreadIDParam_Edge = new RESULT_LOG_SAVE_PARAM;
						pSaveLogThreadIDParam_Edge->iSaveLogType = SAVE_LOG_TYPE_DEFECT_VARIATION_FEATURE;
						pSaveLogThreadIDParam_Edge->strPath = strLogSavePath;
						pSaveLogThreadIDParam_Edge->strType1 = sVariationFeatureLog;
						pSaveLogThreadIDParam_Edge->iTrayNo = iTrayNo;
						pSaveLogThreadIDParam_Edge->iModuleNo = iModuleNo;
						m_pInspectService->AddListSaveResultLogParam(pSaveLogThreadIDParam_Edge, iMzNo);
					}
				}
			}
		}

		return;
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

		strLog.Format("Halcon Exception [uScan::NewSaveVariationFeature_Thread] : <%s>%s", sOperatorName, sErrMsg);
		m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}
#endif

#ifdef SYAI
/* Pseudocode plan:
1. Enhance the existing _DEBUG block inside CuScanApp::InitializeSYAI().
2. Build a localized confirmation message asking whether to load AI models in debug mode.
3. Show an AfxMessageBox with Yes/No choices.
4. If the user selects No, log/print that AI loading is skipped and return true before loading any models.
5. If the user selects Yes, print a localized console message indicating that models will load, then proceed as before.
*/
bool CuScanApp::InitializeSYAI()
{
	using namespace syai::runtime;
	using namespace syai::runtime::inspection;
	using namespace syai::runtime::domain::config;
	using namespace syai::runtime::domain::inspection;

	syai::runtime::SmartVector<ManagerInfo> manager_infos;

	std::cout << "메니저 시스템 초기화 중..." << std::endl;

	// --- 경로 준비 (std::filesystem 사용) ---
	char path_buf[MAX_PATH];
	GetModuleFileNameA(NULL, path_buf, MAX_PATH);
	std::filesystem::path executable_path(path_buf);
	std::filesystem::path folder_path = executable_path.parent_path() / "Data";
	std::filesystem::path config_json_path = folder_path / "ai_inspection_config.json";

	THEAPP.m_syai_service = std::make_shared<syai_service::ServiceDirector>(1, 0, true, "c:\\syai_avi", 3);
	//return THEAPP.m_syai_service->init("inspection_service", "c:\\syai_avi", LogLevel::TRACE_SY, manager_infos);
#ifdef _DEBUG
	CString strPrompt;
	CString strSkipLog;
	if (THEAPP.m_iPGMLanguageSelect == 0) { strPrompt = _T("디버그 모드입니다.\nAI 모델을 로드하시겠습니까?"); }
	else { strPrompt = _T("Debug mode detected.\nWould you like to load AI models?"); }

	const int iMsgResult = AfxMessageBox(strPrompt, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2 | MB_SYSTEMMODAL);
	if (iMsgResult == IDNO)
	{
		THEAPP.m_log_adj->info("Skipping AI model loading in debug mode as per user choice.");

		if (THEAPP.m_syai_logger)
			THEAPP.m_syai_logger->info("Skipping AI model loading in debug mode as per user choice.");
		return true;
	}
#endif // _DEBUG

	if (THEAPP.m_syai_service->init_config(config_json_path.string())) {
		THEAPP.m_syai_logger->info("SYAI ServiceDirector initialized successfully with config: {}", config_json_path.string());
		// THEAPP.m_syai_service->get_current_date_string(); // 날짜 초기화
	} 
	else {
		THEAPP.m_syai_logger->error("SYAI ServiceDirector initialization failed with config: {}", config_json_path.string());
		return false;
	}
	return true;
}
#endif // SYAI

bool CuScanApp::InitializeLogger(const char* log_name, const char* log_root, int log_level)
{
	try {
		std::string log_root_str = log_root ? std::string(log_root) : "C:\\sya\\logs";
		std::string logger_name = log_name ? std::string(log_name) : "service_log";

		std::filesystem::path log_folder = std::filesystem::path(log_root_str) / logger_name;

		try {
			std::filesystem::create_directories(log_folder);
		}
		catch (const std::exception& e) {
			std::cerr << "[Service:" << log_name << "] Failed to create log directory: " << e.what() << std::endl;
			return false;
		}

		// 이미 존재하는 로거 확인
		auto existing_logger = spdlog::get(logger_name);
		if (existing_logger) {
			THEAPP.m_syai_logger = existing_logger;
			THEAPP.m_syai_logger->warn("[Service] Initialize called but THEAPP.m_syai_logger already exists - using existing THEAPP.m_syai_logger");
			return true;
		}

		// 비동기 로깅 스레드 풀 초기화
		if (!spdlog::thread_pool()) {
			spdlog::init_thread_pool(8192, 1);
		}

		// Multi-sink 로거 생성
		std::vector<spdlog::sink_ptr> sinks;

		auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
			(log_folder / "daily_log.log").string(), 0, 0, false, 5);

		auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
			(log_folder / "size_rotated_log.log").string(), 50 * 1024 * 1024, 10);

		sinks.push_back(daily_sink);
		sinks.push_back(rotating_sink);

		THEAPP.m_syai_logger = std::make_shared<spdlog::async_logger>(
			logger_name, sinks.begin(), sinks.end(),
			spdlog::thread_pool(), spdlog::async_overflow_policy::block);

		THEAPP.m_syai_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");

		auto log_level_value = log_level;
		if (log_level_value >= static_cast<int>(spdlog::level::trace) &&
			log_level_value < static_cast<int>(spdlog::level::n_levels)) {
			auto set_log_level = static_cast<spdlog::level::level_enum>(log_level_value);
			THEAPP.m_syai_logger->set_level(set_log_level);
			THEAPP.m_syai_logger->flush_on(set_log_level);
		}
		else {
			THEAPP.m_syai_logger->set_level(spdlog::level::info);
			THEAPP.m_syai_logger->flush_on(spdlog::level::warn);
		}
		spdlog::register_logger(THEAPP.m_syai_logger);

		if (THEAPP.m_syai_logger) THEAPP.m_syai_logger->info("[Service] Logger initialized. Log folder: {}", log_folder.string());
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "[Service] Failed to initialize THEAPP.m_syai_logger: " << e.what() << std::endl;
		return false;
	}
}



GTRegion *CuScanApp::GetDefectTeachingInfo(HObject *pHDefectRgn, int iVisionCamType, int iMzNo, int iPcVisionNo, int iInspectionBufferIdx, int iInspectionTypeIndex, int iDefectNameIndex, int *piImageNo, int *piRoiLeft, int *piRoiTop, int *piRoiRight, int *piRoiBottom, int *piInspectionTabNo, CAlgorithmParam *pAlgorithmParam)
{
	try
	{
		int iROIIndex;
		int iNoInspectROI = 0;
		int iDualModelData = THEAPP.g_iDualModelData[iMzNo - 1];
		iNoInspectROI = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegionCount();

		GTRegion *pInspectROIRgn = NULL;

		int iFoundInspectionTabIndex = -1;
		BOOL bFound;
		HObject HIntersectRgn;
		int iFoundROIIndex = -1;

		bFound = FALSE;

		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->miInspectionType != iInspectionTypeIndex)
				continue;

			for (int iInspectTabIdx = 0; iInspectTabIdx < MAX_INSPECTION_TAB; iInspectTabIdx++)
			{
				if (pInspectROIRgn->m_AlgorithmParam[iInspectTabIdx].m_bInspect)
				{
					Intersection(*pHDefectRgn, pInspectROIRgn->m_MeasureData[iInspectionBufferIdx][iInspectTabIdx].m_HInspectTabDefectRgn, &HIntersectRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn))
					{
						iFoundInspectionTabIndex = iInspectTabIdx;
						bFound = TRUE;
						break;
					}
				}
			}

			if (bFound)
			{
				iFoundROIIndex = iROIIndex;
				break;
			}
		}

		pInspectROIRgn = NULL;

		if (bFound)
		{
			pInspectROIRgn = THEAPP.m_pDualModelDataManager[iDualModelData][iPcVisionNo]->m_pInspectionArea->GetChildTRegion(iFoundROIIndex);

			*piImageNo = pInspectROIRgn->miTeachImageIndex;

			POINT LTPoint, RBPoint;
			pInspectROIRgn->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService_N[iVisionCamType]);
			pInspectROIRgn->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService_N[iVisionCamType]);

			*piRoiLeft = LTPoint.x;
			*piRoiTop = LTPoint.y;
			*piRoiRight = RBPoint.x;
			*piRoiBottom = RBPoint.y;

			*piInspectionTabNo = iFoundInspectionTabIndex + 1;

			*pAlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iFoundInspectionTabIndex];
		}
		else
			return NULL;

		return pInspectROIRgn;
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

		strLog.Format("Halcon Exception [uScan::GetDefectTeachingInfo] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return NULL;
	}
}

BOOL CuScanApp::IsSameModelType(int iType)
{
	if (m_iModelType == iType)
		return TRUE;
	else
		return FALSE;
}

void CuScanApp::ReadOverlayViewportINI()
{
	CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
	CString strEquipModel = g_strModelTypeName[GetModelType()];
	CString strIniFile;
	strIniFile.Format("OverlayViewport_%s.ini", strEquipModel);
	CIniFileCS INI_OverlayViewport(strDataFolder + strIniFile);
	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		CString sVisionCamType;
		sVisionCamType = m_ModelDefineInfo.m_strVisionName[i];

		CString strSection, strKey;
		strSection.Format("%s", sVisionCamType);
		for (int j = 0; j < MAX_RESULT_IMAGE; j++)
		{
			strKey.Format("%d_Review capture image", j + 1);
			Struct_PreferenceStruct.iOverlayImageIndex[i][j] = INI_OverlayViewport.Get_Integer(strSection, strKey, 0);
			strKey.Format("%d_Left", j + 1);
			Struct_PreferenceStruct.iOverlayImageViewportLeft[i][j] = INI_OverlayViewport.Get_Integer(strSection, strKey, 0);
			strKey.Format("%d_Top", j + 1);
			Struct_PreferenceStruct.iOverlayImageViewportTop[i][j] = INI_OverlayViewport.Get_Integer(strSection, strKey, 0);
			strKey.Format("%d_Zoom", j + 1);
			Struct_PreferenceStruct.dOverlayImageViewportZoomRatio[i][j] = INI_OverlayViewport.Get_Double(strSection, strKey, 1.0);
		}
	}
}

//20201114 jwj add
//void CuScanApp::ChangeHDD(int iMzNo )
//{
//	CString strStatus = "";
//	CIniFileCS INI(Struct_PreferenceStruct.m_strBasic_FolderPath + "HDD_Status.ini");
//	//CIniFileCS INI_SubHDD(Struct_PreferenceStruct.m_strBasic_FolderPathSubHDD + "HDD_Status.ini");
//
//	//BOOL bCapaInsufficient = FALSE;
//	
//	if (m_enumCurHDDStatus == eINIT || m_enumCurHDDStatus == eMAINHDD) //Status 초기 이거나 Main HDD 저장하고 있을 경우
//	{
//		strStatus = INI.Get_String("HDD", "STATUS", "FULL");
//		m_enumCurHDDStatus = eMAINHDD;
//
//		if (strStatus == "READY")
//		{
//			m_enumCurHDDStatus = eMAINHDD;
//			m_strDualHddBasicFolderPath[iMzNo] = Struct_PreferenceStruct.m_strBasic_FolderPath;
//		}
//		else
//		{
//			strStatus = INI_SubHDD.Get_String("HDD", "STATUS", "FULL");
//			
//			if (strStatus == "READY")
//			{
//				m_enumCurHDDStatus = eSUBHDD;
//				/m_strDualHddBasicFolderPath[iMzNo] = Struct_PreferenceStruct.m_strBasic_FolderPathSubHDD;
//			}
//			else
//			{
//				m_enumCurHDDStatus = eMAINHDD;
//				m_strDualHddBasicFolderPath[iMzNo] = Struct_PreferenceStruct.m_strBasic_FolderPath; //둘다 FULL 상태일 경우 기존 방식처럼 동작.
//			}
//		}
//	}
//	else //Status Sub HDD 저장하고 있을 경우
//	{
//		strStatus = INI_SubHDD.Get_String("HDD", "STATUS", "FULL");
//		if (strStatus == "READY")
//		{
//			m_enumCurHDDStatus = eSUBHDD;
//			m_strDualHddBasicFolderPath[iMzNo] = Struct_PreferenceStruct.m_strBasic_FolderPathSubHDD;
//		}
//		else
//		{
//			strStatus = INI.Get_String("HDD", "STATUS", "FULL");
//
//			m_enumCurHDDStatus = eMAINHDD; //Status 가 eSUBHDD 일 때, Sub HDD 용량이 FULL이면 Main HDD는 꽉차있는지 없는지 여부 상관 없이 무조건 Main Hdd에 저장을 시도
//			if (strStatus == "READY")
//			{
//				m_enumCurHDDStatus = eMAINHDD;
//				m_strDualHddBasicFolderPath[iMzNo] = Struct_PreferenceStruct.m_strBasic_FolderPath; //MAIN만 READY 상태일 경우.
//			}
//			else
//			{
//				m_enumCurHDDStatus = eMAINHDD;
//				m_strDualHddBasicFolderPath[iMzNo] = Struct_PreferenceStruct.m_strBasic_FolderPath; //둘다 FULL 상태일 경우 기존 방식처럼 MAIN HDD저장.
//			}
//		}
//	}
//
//	return;
//}

void CuScanApp::CheckExistLogFileAndDelete(CString strLotID, int iMzNo, int iPCType)
{
	CFileStatus fs;
	CString FolderSub, LotFolderName;
	CString RawFolderPath = Struct_PreferenceStruct.m_strRawFolderPath + "CMI_Results";
	CString ResultFolderPath = Struct_PreferenceStruct.m_strResultFolderPath + "CMI_Results";
	CString ReviewFolderPath = Struct_PreferenceStruct.m_strReviewFolderPath + "CMI_Results";
	CString AdjFolderPath = Struct_PreferenceStruct.m_strAdjFolderPath + "CMI_Results";
	CString ResultLogFolderPath = Struct_PreferenceStruct.m_strResultLogFolderPath + "CMI_Results";
	CString LAS1FolderPath = Struct_PreferenceStruct.m_strSaveFolderPathLAS1 + "CMI_Results";
	CString LAS2FolderPath = Struct_PreferenceStruct.m_strSaveFolderPathLAS2 + "CMI_Results";
	CString LAS3FolderPath = Struct_PreferenceStruct.m_strSaveFolderPathLAS3 + "CMI_Results";

	FolderSub.Format("\\%d", m_LotstartInfo[iMzNo - 1].m_iCurYear);
	FolderSub.Format("%s\\%d", FolderSub, m_LotstartInfo[iMzNo - 1].m_iCurMonth);
	FolderSub.Format("%s\\%d", FolderSub, m_LotstartInfo[iMzNo - 1].m_iCurDay);

	RawFolderPath = RawFolderPath + FolderSub;
	ResultFolderPath = ResultFolderPath + FolderSub;
	ReviewFolderPath = ReviewFolderPath + FolderSub;
	AdjFolderPath = AdjFolderPath + FolderSub;
	ResultLogFolderPath = ResultLogFolderPath + FolderSub;
	LAS1FolderPath = LAS1FolderPath + FolderSub;
	LAS2FolderPath = LAS2FolderPath + FolderSub;
	LAS3FolderPath = LAS3FolderPath + FolderSub;

	CString LotResultSub, ADJLotResultSub;

	LotResultSub.Format("\\Tray-1");
	ADJLotResultSub.Format("\\Tray-1");

	CString LocalLotResultFile = ResultLogFolderPath + "\\" + strLotID + LotResultSub;
	CString LAS1LotResultFile = LAS1FolderPath + "\\" + strLotID + LotResultSub;
	CString LAS2LotResultFile = LAS2FolderPath + "\\" + strLotID + LotResultSub;
	CString LAS3LotResultFile = LAS3FolderPath + "\\" + strLotID + LotResultSub;
	CString LocalADJLotResultFile = ResultLogFolderPath + "\\" + strLotID + ADJLotResultSub;
	CString LAS1ADJLotResultFile = LAS1FolderPath + "\\" + strLotID + ADJLotResultSub;
	CString LAS2ADJLotResultFile = LAS2FolderPath + "\\" + strLotID + ADJLotResultSub;
	CString LAS3ADJLotResultFile = LAS3FolderPath + "\\" + strLotID + ADJLotResultSub;

	CTime ChangeTime = CTime::GetCurrentTime();
	CString strChangeTime;
	strChangeTime.Format("\\Duplicate_%02d%02d%02d", ChangeTime.GetHour(), ChangeTime.GetMinute(), ChangeTime.GetSecond());

	if (Struct_PreferenceStruct.m_bAvoidDuplicateLotLocal || Struct_PreferenceStruct.m_bAvoidDuplicateLotLas)
		Sleep(500 * iPCType);

	if (Struct_PreferenceStruct.m_bAvoidDuplicateLotLocal)
	{
		if (_access(LocalLotResultFile, 0) == 0)
		{
			chmod(RawFolderPath + "\\" + strLotID, 0777);
			chmod(ResultFolderPath + "\\" + strLotID, 0777);
			chmod(ReviewFolderPath + "\\" + strLotID, 0777);
			chmod(AdjFolderPath + "\\" + strLotID, 0777);
			chmod(ResultLogFolderPath + "\\" + strLotID, 0777);

			int nResult1 = rename(RawFolderPath + "\\" + strLotID, RawFolderPath + strChangeTime + "_" + strLotID);

			if (nResult1 == -1)
			{
				DeleteFileA((LPCSTR)LocalLotResultFile);
				DeleteFileA((LPCSTR)LocalADJLotResultFile);
			}

			if (RawFolderPath != ResultFolderPath)
				int nResult2 = rename(ResultFolderPath + "\\" + strLotID, ResultFolderPath + strChangeTime + "_" + strLotID);

			if (RawFolderPath != ReviewFolderPath && ResultFolderPath != ReviewFolderPath)
				int nResult3 = rename(ReviewFolderPath + "\\" + strLotID, ReviewFolderPath + strChangeTime + "_" + strLotID);

			if (RawFolderPath != AdjFolderPath && ResultFolderPath != AdjFolderPath && ReviewFolderPath != AdjFolderPath)
				int nResult4 = rename(AdjFolderPath + "\\" + strLotID, AdjFolderPath + strChangeTime + "_" + strLotID);

			if (RawFolderPath != ResultLogFolderPath && ResultFolderPath != ResultLogFolderPath && ReviewFolderPath != ResultLogFolderPath && AdjFolderPath != ResultLogFolderPath)
				int nResult5 = rename(ResultLogFolderPath + "\\" + strLotID, ResultLogFolderPath + strChangeTime + "_" + strLotID);
		}
	}

	if (Struct_PreferenceStruct.m_bAvoidDuplicateLotLas && Struct_PreferenceStruct.m_bUseLAS1 && _access(LAS1LotResultFile, 0) == 0)
	{
		chmod(LAS1FolderPath + "\\" + strLotID, 0777);
		int nResult6 = rename(LAS1FolderPath + "\\" + strLotID, LAS1FolderPath + strChangeTime + "_" + strLotID);

		if (nResult6 == -1)
		{
			DeleteFileA((LPCSTR)LAS1LotResultFile);
			DeleteFileA((LPCSTR)LAS1ADJLotResultFile);
		}
	}

	if (Struct_PreferenceStruct.m_bAvoidDuplicateLotLas && Struct_PreferenceStruct.m_bUseLAS2 && _access(LAS2LotResultFile, 0) == 0)
	{
		chmod(LAS2FolderPath + "\\" + strLotID, 0777);
		int nResult7 = rename(LAS2FolderPath + "\\" + strLotID, LAS2FolderPath + strChangeTime + "_" + strLotID);

		if (nResult7 == -1)
		{
			DeleteFileA((LPCSTR)LAS2LotResultFile);
			DeleteFileA((LPCSTR)LAS2ADJLotResultFile);
		}
	}

	if (Struct_PreferenceStruct.m_bAvoidDuplicateLotLas && Struct_PreferenceStruct.m_bUseLAS3 && _access(LAS3LotResultFile, 0) == 0)
	{
		chmod(LAS3FolderPath + "\\" + strLotID, 0777);
		int nResult8 = rename(LAS3FolderPath + "\\" + strLotID, LAS3FolderPath + strChangeTime + "_" + strLotID);

		if (nResult8 == -1)
		{
			DeleteFileA((LPCSTR)LAS3LotResultFile);
			DeleteFileA((LPCSTR)LAS3ADJLotResultFile);
		}
	}
}

char* CuScanApp::TempUTF8toANSI(char *pszCode)
{
	BSTR    bstrWide;
	char*   pszAnsi;
	int     nLength;

	// Get nLength of the Wide Char buffer   
	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, nLength);

	// Change UTF-8 to Unicode (UTF-16)   
	MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, bstrWide, nLength);


	// Get nLength of the multi byte buffer    
	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	pszAnsi = new char[nLength];


	// Change from unicode to mult byte   
	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);

	SysFreeString(bstrWide);

	return pszAnsi;
}

BOOL CuScanApp::IsNumberCheck(CString str)
{
	BOOL bNumber = TRUE;

	for (int i = 0; i < str.GetLength(); i++)
	{
		if (str.GetAt(i) < '0' || str.GetAt(i) > '9')
		{
			bNumber = FALSE;
			break;
		}

		if (!isdigit(str.GetAt(i)))
		{
			bNumber = FALSE;
			break;
		}
	}

	return bNumber;
}

#ifdef SNZEROMQ

BOOL CuScanApp::InitializeZeroMQ()
{
	using namespace synapse::network::cpp::zeromq;

	CString sNetworkConnInfo;
	sNetworkConnInfo.Format("tcp://%d.%d.%d.%d:%d", Struct_PreferenceStruct.m_iSam2IP_1, Struct_PreferenceStruct.m_iSam2IP_2, Struct_PreferenceStruct.m_iSam2IP_3, Struct_PreferenceStruct.m_iSam2IP_4, Struct_PreferenceStruct.m_iSam2Port);

	m_zeromq_client.Init((LPCSTR)sNetworkConnInfo, 2000, 2000);
	// m_zeromq_client.Init("tcp://localhost:6000", 2000, 2000);
	const char* id = m_zeromq_client.GetID();
	strcpy_s(m_zeromq_client_id, sizeof(m_zeromq_client_id), id);

	// RequestConnection 요청
	common::models::entry::RequestConnection request_message(id, 1);
	common::models::entry::ResponseConnection response_connection;

	if (!m_zeromq_client.TrySendRequestConnection(request_message, response_connection))
	{
		strLog.Format("zeroMQ RequestConnection fails");
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

		AfxMessageBox("ROI 자동생성 딥러닝 서버에 연결할 수 없습니다.", MB_ICONERROR | MB_SYSTEMMODAL);
		return FALSE;
	}

	// SAM2 ConnectionInfo 추출
	common::models::entry::ConnectionInfo sam2_connection_info;
	for (size_t i = 0; i < response_connection.GetConnectionInfoSize(); ++i)
	{
		const auto* connection_info = response_connection.GetConnectionInfo(i);
		if (connection_info && connection_info->GetFeatureType() == common::enums::FeatureType::SAM2)
		{
			sam2_connection_info = *connection_info;
			break;
		}
	}

	// TryBindFeature
	if (!m_zeromq_client.TryBindFeature(sam2_connection_info, 30000, 30000))
	{
		strLog.Format("Failed to bind feature for SAM2.");
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

		AfxMessageBox("SAM2 연결 실패.", MB_ICONERROR | MB_SYSTEMMODAL);
		return FALSE;
	}

	// Loopback 테스트
	time_t send_info_loop_back_time = std::time(nullptr);
	common::models::SNZeroMQLoopback send_info_loop_back(id, send_info_loop_back_time);
	common::models::SNZeroMQLoopback receive_info_loop_back;

	if (!m_zeromq_client.TryLoopback(common::enums::FeatureType::Connection, send_info_loop_back, receive_info_loop_back))
	{
		strLog.Format("Failed Connection Loopback.");
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

		AfxMessageBox("Loopback 연결 실패.", MB_ICONERROR | MB_SYSTEMMODAL);
		return FALSE;
	}

	time_t send_sam2_loop_back_time = std::time(nullptr);
	common::models::SNZeroMQLoopback send_sam2_loop_back(id, send_sam2_loop_back_time);
	common::models::SNZeroMQLoopback receive_sam2_loop_back;

	if (!m_zeromq_client.TryLoopback(common::enums::FeatureType::SAM2, send_sam2_loop_back, receive_sam2_loop_back))
	{
		strLog.Format("Failed SAM2 Loopback.");
		THEAPP.m_log_adj->warn("{}", LOG_CSTR(strLog));

		AfxMessageBox("SAM2 Loopback 실패.", MB_ICONERROR | MB_SYSTEMMODAL);
		return FALSE;
	}

	return TRUE;
}

#endif

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_EditBGBrush.CreateSolidBrush(RGB(255, 255, 255));

	GetAppVersion();
	GetAppModify();
	ShowReleaseInformation();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::ShowReleaseInformation()
{
	CString	sReleaseText;
	char pszPathName[_MAX_PATH * 2];
	::GetModuleFileName(::AfxGetInstanceHandle(), pszPathName, _MAX_PATH * 2);

	//PathRemoveFileSpec(pszPathName);
	pszPathName[strlen(pszPathName) - strlen(strrchr(pszPathName, '\\'))] = (char)'\0';

	strcat(pszPathName, "\\Release.txt");

	FILE *fp;

	if ((fp = fopen(pszPathName, "rt")) != NULL) {
		char caTempString[500];
		while (fgets(caTempString, 499, fp) != NULL) {
			caTempString[strlen(caTempString) - 1] = '\0';
			sReleaseText += caTempString;
			sReleaseText += "\r\n";
		}
		m_EditRelease.SetSel(0, -1);
		m_EditRelease.ReplaceSel(LPCTSTR(sReleaseText));
		m_EditRelease.SendMessage(WM_VSCROLL, MAKEWPARAM(SB_TOP, NULL), NULL);

		fclose(fp);
	}
}

//////////////////
// Get file version info for a given module
// Allocates storage for all info, fills "this" with
// VS_FIXEDFILEINFO, and sets codepage.
//
BOOL CAboutDlg::GetAppVersion(void)
{
	char AppFilePath[_MAX_PATH * 2];
	// 실행파일의 풀패스 경로를 구한다.  
	strcpy(AppFilePath, AfxGetApp()->m_pszHelpFilePath);
	int slength = strlen(AppFilePath);
	strcpy(AppFilePath + (slength - 4), ".exe");

	//
	// 주어진 파일의 버전 정보를 읽는다. (이렇게 길어지리라 생각 못했다... )
	//
	DWORD Dummy = NULL;
	BOOL bResult = FALSE;
	HRESULT hResult = S_OK;
	DWORD dwLen = 0;
	DWORD dwError = 0;
	PVOID pVersionData = NULL;
	PVOID pVersionInfo = NULL;

	// 버전정보의 크기를 알아 내고 데이터 버퍼를 할당한다.
	// 파일의 버전정보의 크기가 파일마다 제각기 다르기 땜시...
	dwLen = ::GetFileVersionInfoSize(AppFilePath, &Dummy);
	if (dwLen == 0)
	{
		return GetLastError();
	}
	// 버전 정보를 담을 버퍼를 할당한다. (수천 바이트가 필요하다)
	pVersionData = ::HeapAlloc(::GetProcessHeap(), 0, dwLen);
	ZeroMemory(pVersionData, dwLen);

	// 버전 정보 데이터를 읽는다.
	bResult = ::GetFileVersionInfo(AppFilePath, NULL, dwLen, pVersionData);
	if (bResult == FALSE)
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return dwError;
	}

	// VS_FIXEDFILEINFO 로 부터 버전정보를 읽는다.
	// (숫자로 된 버전정보는 이 구조체 안에 정보가 담긴다)
	bResult = ::VerQueryValue(pVersionData, TEXT("\\"), &pVersionInfo, (PUINT)&dwLen);
	if (bResult == FALSE)
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return dwError;
	}
	VS_FIXEDFILEINFO *pFileInfo = (VS_FIXEDFILEINFO *)pVersionInfo;
	// 숫자로된 버전 정보를 알아냈으니 필요한 대로 쓰문 되겠다.
	DWORD dwMajorVer = HIWORD(pFileInfo->dwFileVersionMS);
	DWORD dwMinorVer = LOWORD(pFileInfo->dwFileVersionMS);
	DWORD dwReleaseVer = HIWORD(pFileInfo->dwFileVersionLS);
	DWORD dwBuildVer = LOWORD(pFileInfo->dwFileVersionLS);

	CString strVer;
	strVer.Format("CMAI2100 System - Version %d.%d.%d.%d", dwMajorVer, dwMinorVer, dwReleaseVer, dwBuildVer);

	GetDlgItem(IDC_STATIC_ABOUTBOXDLG_1)->SetWindowText(strVer);

	// pVersionInfo 변수가 문자열 버전정보를 갖고 있다.
	// 쓰고 잡은 대로 쓰문 되긋다.
	// 숫자정보만 표시하고 말자. 
	// 필요로 하다면 추가적인 파일 버전 정보(저작권, 제품명, 기타 등등)를 읽고자 하면
	// 바로 위 코드를 반복적으로 사용하되 FileVersion 대신 ProductName, CompanyName 등을
	// 사용하면 되겠다. 랭귀지 코드ㅡ 알아야 한다.
	// 사용 다한 메모리는 해제해 줘야 함다...
	::HeapFree(::GetProcessHeap(), 0, pVersionData);

	return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CAboutDlg::GetAppModify(void)
{
	char AppFilePath[_MAX_PATH];
	// 실행파일의 풀패스 경로를 구한다.  
	strcpy(AppFilePath, AfxGetApp()->m_pszHelpFilePath);
	int slength = strlen(AppFilePath);
	strcpy(AppFilePath + (slength - 4), ".exe");
	// 파일정보중 수정된 날짜만 가지고 온다. 
	CString ModifyText;
	ModifyText = "프로그램 생성 시간: " + GetFileDateStr(AppFilePath);

	GetDlgItem(IDC_STATIC_ABOUTBOXDLG_3)->SetWindowText(ModifyText);
}
//---------------------------------------------------------------------------
CString CAboutDlg::GetFileDateStr(CString FilePath)
{
	HANDLE hfilehandle;	// identifies the file 
	FILETIME CreationTime;	// address of creation time 
	FILETIME LastAccessTime;	// address of last access time  
	FILETIME LastWriteTime; 	// address of last write time 
	CString FileDateStr = _T("----.--.--, --:--:--");
	// 파일의 경로명을 주고 핸들을 얻는다. 
	hfilehandle = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	// 얻어온 핸들로 부터 파일의 시간정보 3가지 알수 있다. 생성시간, 마지막접근시간, 마지막수정시간
	// 이중 마지막 수정시간을 사용한다. 
	int bRet = GetFileTime(hfilehandle, &CreationTime, &LastAccessTime, &LastWriteTime);
	// 핸들다 썼으니 닫아줘야한다. 
	CloseHandle(hfilehandle);
	// 정보를 제대로 얻어왔다면 리턴값이 0이 아니다.	
	if (bRet)
	{
		CTime FileTime = CTime(LastWriteTime);
		// 날자 및 시간 정보를 보기 좋게 출력한다. 
		FileDateStr.Format("%4d-%02d-%02d, %02d:%02d:%02d", FileTime.GetYear(), FileTime.GetMonth(),
			FileTime.GetDay(), FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond());
	}
	else   // 정보를 제대로 얻어오지 못하면 알수 없음을 출력한다. 
		FileDateStr = "Unknown";

	return FileDateStr;
}
//---------------------------------------------------------------------------

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here

	CWnd *pEditWnd = (CWnd *)GetDlgItem(IDC_EDIT_RELEASE);

	if (pEditWnd == pWnd) {
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->SetBkColor(RGB(255, 255, 255));

		return m_EditBGBrush;
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CuScanApp::ConnectStatusPyADJ()
{
	CString strDataFolder;
	for (int i = 0; i < PC_NUMBER_MAX; i++)
	{
		// strDataFolder = "D:\\CMI_Results\\";
		strDataFolder.Format("\\\\192.168.0.2%d\\CMI_Results\\", i + 1);
		CIniFileCS INI_PyADJStatus(strDataFolder + "PyADJStatus.ini");
		CString strSection, strKey, strLogTime, strTemp;

		strSection = "STATUS";
		strKey = "Time";
		strLogTime = INI_PyADJStatus.Get_String(strSection, strKey, "");

		if (strLogTime == "")
		{
			bPyADJStatus[i] = FALSE;

			if (i == 0)
			{
				m_pInspectResultDlg->m_ConnectAdj1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ1, "OFF");
			}
			else if (i == 1)
			{
				m_pInspectResultDlg->m_ConnectAdj2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ2, "OFF");
			}
			else if (i == 2)
			{
				m_pInspectResultDlg->m_ConnectAdj3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ3, "OFF");
			}
			else if (i == 3)
			{
				m_pInspectResultDlg->m_ConnectAdj4.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ4, "OFF");
			}
			else if (i == 4)
			{
				m_pInspectResultDlg->m_ConnectAdj5.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ5, "OFF");
			}

			continue;
		}

		SYSTEMTIME SaveLogTime;
		strTemp = strLogTime.Mid(0, 4);
		SaveLogTime.wYear = atoi(strTemp);				// Year
		strTemp = strLogTime.Mid(5, 2);
		SaveLogTime.wMonth = atoi(strTemp);				// Month
		strTemp = strLogTime.Mid(8, 2);
		SaveLogTime.wDay = atoi(strTemp);				// Day
		strTemp = strLogTime.Mid(11, 2);
		SaveLogTime.wHour = atoi(strTemp);				// Hour
		strTemp = strLogTime.Mid(14, 2);
		SaveLogTime.wMinute = atoi(strTemp);			// Minite
		strTemp = strLogTime.Mid(17, 2);
		SaveLogTime.wSecond = atoi(strTemp);			// Second
		SaveLogTime.wMilliseconds = atoi("000");		// MilliSecond

		FILETIME ft;
		SystemTimeToFileTime(&SaveLogTime, &ft);        // 변환 (요일 계산 X)
		FileTimeToSystemTime(&ft, &SaveLogTime);        // 다시 SYSTEMTIME으로 변환 → wDayOfWeek 계산됨

		SYSTEMTIME CurrentTime;
		GetLocalTime(&CurrentTime);

		LONGLONG diffSec = GetTimeDiffseconds(SaveLogTime, CurrentTime);

		int iOffCheckMinute = 30;	// 옵션으로 뺄지 말지
		if (diffSec > 60 * iOffCheckMinute)
		{
			bPyADJStatus[i] = FALSE;

			if (i == 0)
			{
				m_pInspectResultDlg->m_ConnectAdj1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ1, "OFF");
			}
			else if (i == 1)
			{
				m_pInspectResultDlg->m_ConnectAdj2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ2, "OFF");
			}
			else if (i == 2)
			{
				m_pInspectResultDlg->m_ConnectAdj3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ3, "OFF");
			}
			else if (i == 3)
			{
				m_pInspectResultDlg->m_ConnectAdj4.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ4, "OFF");
			}
			else if (i == 4)
			{
				m_pInspectResultDlg->m_ConnectAdj5.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ5, "OFF");
			}
		}
		else
		{
			bPyADJStatus[i] = TRUE;

			if (i == 0)
			{
				m_pInspectResultDlg->m_ConnectAdj1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(0, 0, 0), RGB(1, 255, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ1, "ON");
			}
			else if (i == 1)
			{
				m_pInspectResultDlg->m_ConnectAdj2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(0, 0, 0), RGB(1, 255, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ2, "ON");
			}
			else if (i == 2)
			{
				m_pInspectResultDlg->m_ConnectAdj3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(0, 0, 0), RGB(1, 255, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ3, "ON");
			}
			else if (i == 3)
			{
				m_pInspectResultDlg->m_ConnectAdj4.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(0, 0, 0), RGB(1, 255, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ4, "ON");
			}
			else if (i == 4)
			{
				m_pInspectResultDlg->m_ConnectAdj5.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(0, 0, 0), RGB(1, 255, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_ADJ5, "ON");
			}
		}
	}

	if (bPyADJStatus[Struct_PreferenceStruct.m_iPCType])
		return TRUE;
	else
		return FALSE;
}

BOOL CuScanApp::ConnectStatusLASAggregator()
{
	CString strDataFolder;
	for (int i = 0; i < PC_NUMBER_MAX; i++)
	{
		// strDataFolder = "D:\\CMI_Results\\";
		strDataFolder.Format("\\\\192.168.0.2%d\\CMI_Results\\", i + 1);
		CIniFileCS INI_LASAggregatorStatus(strDataFolder + "LASAggregatorStatus.ini");
		CString strSection, strKey, strLogTime, strTemp;

		strSection = "STATUS";
		strKey = "Time";
		strLogTime = INI_LASAggregatorStatus.Get_String(strSection, strKey, "");

		if (strLogTime == "")
		{
			bLASAggregatorStatus[i] = FALSE;

			if (i == 0)
			{
				m_pInspectResultDlg->m_ConnectLas1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS1, "OFF");
			}
			else if (i == 1)
			{
				m_pInspectResultDlg->m_ConnectLas2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS2, "OFF");
			}
			else if (i == 2)
			{
				m_pInspectResultDlg->m_ConnectLas3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS3, "OFF");
			}
			else if (i == 3)
			{
				m_pInspectResultDlg->m_ConnectLas4.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS4, "OFF");
			}
			else if (i == 4)
			{
				m_pInspectResultDlg->m_ConnectLas5.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS5, "OFF");
			}

			continue;
		}

		SYSTEMTIME SaveLogTime;
		strTemp = strLogTime.Mid(0, 4);
		SaveLogTime.wYear = atoi(strTemp);				// Year
		strTemp = strLogTime.Mid(5, 2);
		SaveLogTime.wMonth = atoi(strTemp);				// Month
		strTemp = strLogTime.Mid(8, 2);
		SaveLogTime.wDay = atoi(strTemp);				// Day
		strTemp = strLogTime.Mid(11, 2);
		SaveLogTime.wHour = atoi(strTemp);				// Hour
		strTemp = strLogTime.Mid(14, 2);
		SaveLogTime.wMinute = atoi(strTemp);			// Minite
		strTemp = strLogTime.Mid(17, 2);
		SaveLogTime.wSecond = atoi(strTemp);			// Second
		SaveLogTime.wMilliseconds = atoi("000");		// MilliSecond

		FILETIME ft;
		SystemTimeToFileTime(&SaveLogTime, &ft);        // 변환 (요일 계산 X)
		FileTimeToSystemTime(&ft, &SaveLogTime);        // 다시 SYSTEMTIME으로 변환 → wDayOfWeek 계산됨

		SYSTEMTIME CurrentTime;
		GetLocalTime(&CurrentTime);

		LONGLONG diffSec = GetTimeDiffseconds(SaveLogTime, CurrentTime);

		int iOffCheckMinute = 30;	// 옵션으로 뺄지 말지
		if (diffSec > 60 * iOffCheckMinute)
		{
			bLASAggregatorStatus[i] = FALSE;

			if (i == 0)
			{
				m_pInspectResultDlg->m_ConnectLas1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS1, "OFF");
			}
			else if (i == 1)
			{
				m_pInspectResultDlg->m_ConnectLas2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS2, "OFF");
			}
			else if (i == 2)
			{
				m_pInspectResultDlg->m_ConnectLas3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS3, "OFF");
			}
			else if (i == 3)
			{
				m_pInspectResultDlg->m_ConnectLas4.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS4, "OFF");
			}
			else if (i == 4)
			{
				m_pInspectResultDlg->m_ConnectLas5.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(255, 255, 0), RGB(255, 1, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS5, "OFF");
			}
		}
		else
		{
			bLASAggregatorStatus[i] = TRUE;

			if (i == 0)
			{
				m_pInspectResultDlg->m_ConnectLas1.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(0, 0, 0), RGB(1, 255, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS1, "ON");
			}
			else if (i == 1)
			{
				m_pInspectResultDlg->m_ConnectLas2.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(0, 0, 0), RGB(1, 255, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS2, "ON");
			}
			else if (i == 2)
			{
				m_pInspectResultDlg->m_ConnectLas3.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(0, 0, 0), RGB(1, 255, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS3, "ON");
			}
			else if (i == 3)
			{
				m_pInspectResultDlg->m_ConnectLas4.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(0, 0, 0), RGB(1, 255, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS4, "ON");
			}
			else if (i == 4)
			{
				m_pInspectResultDlg->m_ConnectLas5.Init_Ctrl(_T("Arial Black"), 10, TRUE, RGB(0, 0, 0), RGB(1, 255, 1));
				m_pInspectResultDlg->SetDlgItemText(IDC_STATIC_CONNECT_LAS5, "ON");
			}
		}
	}

	if (bLASAggregatorStatus[Struct_PreferenceStruct.m_iPCType])
		return TRUE;
	else
		return FALSE;
}


LONGLONG CuScanApp::GetTimeDiffseconds(SYSTEMTIME SaveLogTime, SYSTEMTIME CurrentTime)
{
	FILETIME ft1, ft2;
	ULARGE_INTEGER ui1, ui2;

	// SYSTEMTIME -> FILETIME
	SystemTimeToFileTime(&SaveLogTime, &ft1);
	SystemTimeToFileTime(&CurrentTime, &ft2);

	// FILETIME -> ULARGE_INTEGER (64비트 정수로 시간 계산)
	ui1.LowPart = ft1.dwLowDateTime;
	ui1.HighPart = ft1.dwHighDateTime;

	ui2.LowPart = ft2.dwLowDateTime;
	ui2.HighPart = ft2.dwHighDateTime;

	// FILETIME은 100ns 단위 → 1초 = 10,000,000 단위
	return (LONGLONG)((LONGLONG)ui2.QuadPart - (LONGLONG)ui1.QuadPart) / 10000000;
}

void CuScanApp::ReadPGMLanguageSelectINI()
{
	CPGMLanguageSelectDlg PGMLanguageSelectDlg;

	CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_PGMLanguageSelect(strDataFolder + "PGMLanguageSelect.ini");
	CString strSection, strKey;

	strSection = "LANGUAGE";
	strKey = "Select";
	m_iPGMLanguageSelect = INI_PGMLanguageSelect.Get_Integer(strSection, strKey, FALSE);

	strSection = "DIALOG POPUP";
	strKey = "Hide";
	m_bPGMLanguageDialogHide = INI_PGMLanguageSelect.Get_Bool(strSection, strKey, FALSE);
	PGMLanguageSelectDlg.m_bCheckPGMLanguageDialogHide = m_bPGMLanguageDialogHide;

	if (!m_bPGMLanguageDialogHide && PGMLanguageSelectDlg.DoModal() != IDOK)
		return;
}

void CuScanApp::ReadModelNameCodeINI()
{
	CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_ModelNameCode(strDataFolder + "ModelNameCode.ini");
	CString strKey;

	for (int i = 0; i < MODEL_TYPE_MAX; i++)
	{
		strKey.Format("Model_%02d", i);
		g_strModelTypeName[i] = INI_ModelNameCode.Get_String("MODEL TYPE NAME", strKey, "-");
		g_strModelTypeLotCode[i] = INI_ModelNameCode.Get_String("MODEL TYPE LOT CODE", strKey, "-");
	}
}

void CuScanApp::ReadEquipModelTypeINI()
{
	CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_EquipModelType(strDataFolder + "EquipModelType.ini");

	SetModelType(INI_EquipModelType.Get_Integer("CURRENT MODEL TYPE", "Model Type Number", 0));
}


void CuScanApp::LoadCountPerAreaCondition_ini()
{
	if (m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName == ".")
		return;

	CString strModelFolder = GetWorkingDirectory() + "\\Model\\" + m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName;

	CString sCondition;
	sCondition.Format("%s\\SW\\Vision_N%d\\InspectCondition.ini", strModelFolder, VISION_NUMBER_1 + 1);

	CIniFileCS INICondition(sCondition);

	CString strSection, strItem;

	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		for (int j = 0; j < MAX_DEFECT_NAME; j++)
		{
			strSection.Format("InspectType_%d", i + 1);

			strItem.Format("MinArea_Defect_%d", j + 1);
			m_StructCountPerAreaConditionInfo.m_dMinArea[i][j] = INICondition.Get_Double(strSection, strItem, 0);

			strItem.Format("MidArea_Defect_%d", j + 1);
			m_StructCountPerAreaConditionInfo.m_dMidArea[i][j] = INICondition.Get_Double(strSection, strItem, 0);

			strItem.Format("MaxArea_Defect_%d", j + 1);
			m_StructCountPerAreaConditionInfo.m_dMaxArea[i][j] = INICondition.Get_Double(strSection, strItem, 0);

			strItem.Format("AcceptMinNumber_Defect_%d", j + 1);
			m_StructCountPerAreaConditionInfo.m_iMinNumAccept[i][j] = INICondition.Get_Integer(strSection, strItem, 100);

			strItem.Format("AcceptMidNumber_Defect_%d", j + 1);
			m_StructCountPerAreaConditionInfo.m_iMidNumAccept[i][j] = INICondition.Get_Integer(strSection, strItem, 2);

			strItem.Format("AcceptMaxNumber_Defect_%d", j + 1);
			m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[i][j] = INICondition.Get_Integer(strSection, strItem, 1);
		}
	}
}

BOOL CuScanApp::CountPerAreaCondition()
{
	if (m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName == ".")
		return TRUE;

	CString jsonFileName = "C:\\lgit\\InspectParameter\\InspectAreaCountCondition.json";
	std::ifstream file(jsonFileName);
	if (!file)
		return FALSE;

	nlohmann::json JInspectCondition;
	file >> JInspectCondition;

	m_StructCountPerAreaConditionInfo.Reset();
	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		string strKey_InspectionType = g_strInspectionTypeName[i];

		for (int j = 0; j < MAX_DEFECT_NAME; j++)
		{
			string strKey_DefectType = CT2A(m_strDefectName[j]);

			if (!JInspectCondition[strKey_InspectionType][strKey_DefectType].contains("MinArea"))
				m_StructCountPerAreaConditionInfo.m_dMinArea[i][j] = 0;
			else
				m_StructCountPerAreaConditionInfo.m_dMinArea[i][j] = JInspectCondition[strKey_InspectionType][strKey_DefectType]["MinArea"];

			if (!JInspectCondition[strKey_InspectionType][strKey_DefectType].contains("MidArea"))
				m_StructCountPerAreaConditionInfo.m_dMidArea[i][j] = 0;
			else
				m_StructCountPerAreaConditionInfo.m_dMidArea[i][j] = JInspectCondition[strKey_InspectionType][strKey_DefectType]["MidArea"];

			if (!JInspectCondition[strKey_InspectionType][strKey_DefectType].contains("MaxArea"))
				m_StructCountPerAreaConditionInfo.m_dMaxArea[i][j] = 0;
			else
				m_StructCountPerAreaConditionInfo.m_dMaxArea[i][j] = JInspectCondition[strKey_InspectionType][strKey_DefectType]["MaxArea"];

			if (!JInspectCondition[strKey_InspectionType][strKey_DefectType].contains("MinNumAccept"))
				m_StructCountPerAreaConditionInfo.m_iMinNumAccept[i][j] = 100;
			else
				m_StructCountPerAreaConditionInfo.m_iMinNumAccept[i][j] = JInspectCondition[strKey_InspectionType][strKey_DefectType]["MinNumAccept"];

			if (!JInspectCondition[strKey_InspectionType][strKey_DefectType].contains("MidNumAccept"))
				m_StructCountPerAreaConditionInfo.m_iMidNumAccept[i][j] = 2;
			else
				m_StructCountPerAreaConditionInfo.m_iMidNumAccept[i][j] = JInspectCondition[strKey_InspectionType][strKey_DefectType]["MidNumAccept"];

			if (!JInspectCondition[strKey_InspectionType][strKey_DefectType].contains("MaxNumAccept"))
				m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[i][j] = 1;
			else
				m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[i][j] = JInspectCondition[strKey_InspectionType][strKey_DefectType]["MaxNumAccept"];
		}
	}

	return TRUE;
}

void CuScanApp::SaveCountPerAreaCondition()
{
	if (m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_sModelName == ".")
		return;

	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;
	CString sLGITParameterFolder;
	sLGITParameterFolder.Format("C:\\lgit\\InspectParameter");
	if ((hFindFile = FindFirstFile(sLGITParameterFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
		CreateDirectory(sLGITParameterFolder, NULL);

	CString jsonFileName = sLGITParameterFolder + "\\InspectAreaCountCondition.json";
	nlohmann::json JInspectCondition;
	std::ifstream inFile(jsonFileName);
	if (inFile)
		inFile >> JInspectCondition;
	inFile.close();

	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		string strKey_InspectionType = g_strInspectionTypeName[i];

		for (int j = 0; j < MAX_DEFECT_NAME; j++)
		{
			string strKey_DefectType = CT2A(m_strDefectName[j]);

			CString sTemp;
			m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickGetText(2, j, &sTemp);
			m_StructCountPerAreaConditionInfo.m_dMinArea[i][j] = atof((LPSTR)(LPCSTR)sTemp);
			JInspectCondition[strKey_InspectionType][strKey_DefectType]["MinArea"] = m_StructCountPerAreaConditionInfo.m_dMinArea[i][j];

			m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickGetText(3, j, &sTemp);
			m_StructCountPerAreaConditionInfo.m_dMidArea[i][j] = atof((LPSTR)(LPCSTR)sTemp);
			JInspectCondition[strKey_InspectionType][strKey_DefectType]["MidArea"] = m_StructCountPerAreaConditionInfo.m_dMidArea[i][j];

			m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickGetText(4, j, &sTemp);
			m_StructCountPerAreaConditionInfo.m_dMaxArea[i][j] = atof((LPSTR)(LPCSTR)sTemp);
			JInspectCondition[strKey_InspectionType][strKey_DefectType]["MaxArea"] = m_StructCountPerAreaConditionInfo.m_dMaxArea[i][j];

			m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickGetText(5, j, &sTemp);
			m_StructCountPerAreaConditionInfo.m_iMinNumAccept[i][j] = atoi((LPSTR)(LPCSTR)sTemp);
			JInspectCondition[strKey_InspectionType][strKey_DefectType]["MinNumAccept"] = m_StructCountPerAreaConditionInfo.m_iMinNumAccept[i][j];

			m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickGetText(6, j, &sTemp);
			m_StructCountPerAreaConditionInfo.m_iMidNumAccept[i][j] = atoi((LPSTR)(LPCSTR)sTemp);
			JInspectCondition[strKey_InspectionType][strKey_DefectType]["MidNumAccept"] = m_StructCountPerAreaConditionInfo.m_iMidNumAccept[i][j];

			m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionDlg[i]->m_Grid.QuickGetText(7, j, &sTemp);
			m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[i][j] = atoi((LPSTR)(LPCSTR)sTemp);
			JInspectCondition[strKey_InspectionType][strKey_DefectType]["MaxNumAccept"] = m_StructCountPerAreaConditionInfo.m_iMaxNumAccept[i][j];
		}
	}

	std::ofstream outFile(jsonFileName);
	outFile << JInspectCondition.dump(4);
	outFile.close();

	return;
}

void CuScanApp::SaveVariationParameter(int iVisionIdx, CString sModelType)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = m_ModelDefineInfo.m_strVisionName_Short[iVisionIdx];

	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;
	CString sLGITParameterFolder, sModelFolder;
	sLGITParameterFolder.Format("C:\\lgit\\InspectParameter");
	if ((hFindFile = FindFirstFile(sLGITParameterFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
		CreateDirectory(sLGITParameterFolder, NULL);
	sModelFolder.Format("%s\\%s", sLGITParameterFolder, sModelType);
	if ((hFindFile = FindFirstFile(sModelFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
		CreateDirectory(sModelFolder, NULL);

	CString jsonFileName;
	jsonFileName.Format("%s\\VariationParameter_%s.json", sModelFolder, sVisionCamType_Short);
	nlohmann::json JVariationParameter;
	std::ifstream inFile(jsonFileName);
	if (inFile)
		inFile >> JVariationParameter;
	inFile.close();

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		CString sImageIdx;
		sImageIdx.Format("Image%02d", i);
		string strKey_ImageIdx = CT2A(sImageIdx);

		for (int j = 0; j < 10; j++)
		{
			CString sROINo;
			sROINo.Format("ROI%d", j);
			string strKey_ROINo = CT2A(sROINo);

			for (int k = 0; k < MAX_INSPECTION_TAB; k++)
			{
				CString sTabIdx;
				sTabIdx.Format("Tab%d", k + 1);
				string strKey_TabIdx = CT2A(sTabIdx);

				if (m_StructVariationParameter.bUseImageCompare[iVisionIdx][i][j][k])
				{
					/// Image process Channel Type (sProcessChType)
					CString sProcessChType;
					if (m_StructVariationParameter.iProcessChType[iVisionIdx][i][j][k] == CHANNEL_TYPE_COLOR)
						sProcessChType = "C";
					else if (m_StructVariationParameter.iProcessChType[iVisionIdx][i][j][k] == CHANNEL_TYPE_R)
						sProcessChType = "R";
					else if (m_StructVariationParameter.iProcessChType[iVisionIdx][i][j][k] == CHANNEL_TYPE_G)
						sProcessChType = "G";
					else if (m_StructVariationParameter.iProcessChType[iVisionIdx][i][j][k] == CHANNEL_TYPE_B)
						sProcessChType = "B";
					else if (m_StructVariationParameter.iProcessChType[iVisionIdx][i][j][k] == CHANNEL_TYPE_H)
						sProcessChType = "H";
					else if (m_StructVariationParameter.iProcessChType[iVisionIdx][i][j][k] == CHANNEL_TYPE_S)
						sProcessChType = "S";
					else
						sProcessChType = "I";

					JVariationParameter[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["Channel"] = sProcessChType;
					JVariationParameter[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["BrightUse"] = m_StructVariationParameter.bUseImageCompareBright[iVisionIdx][i][j][k];
					JVariationParameter[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["BrightAbs"] = m_StructVariationParameter.iImageCompareBrightAbs[iVisionIdx][i][j][k];
					JVariationParameter[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["BrightVar"] = m_StructVariationParameter.dImageCompareBrightVar[iVisionIdx][i][j][k];
					JVariationParameter[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["DarkUse"] = m_StructVariationParameter.bUseImageCompareDark[iVisionIdx][i][j][k];
					JVariationParameter[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["DarkAbs"] = m_StructVariationParameter.iImageCompareDarkAbs[iVisionIdx][i][j][k];
					JVariationParameter[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["DarkVar"] = m_StructVariationParameter.dImageCompareDarkVar[iVisionIdx][i][j][k];
				}
			}
		}
	}

	std::ofstream outFile(jsonFileName);
	outFile << JVariationParameter.dump(4);
	outFile.close();

	return;
}

void CuScanApp::ClearVariationParameter(int iVisionIdx, CString sModelType)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = m_ModelDefineInfo.m_strVisionName_Short[iVisionIdx];

	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;
	CString sLGITParameterFolder, sModelFolder;
	sLGITParameterFolder.Format("C:\\lgit\\InspectParameter");
	if ((hFindFile = FindFirstFile(sLGITParameterFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
		CreateDirectory(sLGITParameterFolder, NULL);
	sModelFolder.Format("%s\\%s", sLGITParameterFolder, sModelType);
	if ((hFindFile = FindFirstFile(sModelFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
		CreateDirectory(sModelFolder, NULL);

	CString jsonFileName;
	jsonFileName.Format("%s\\VariationParameter_%s.json", sModelFolder, sVisionCamType_Short);
	std::ofstream outFile(jsonFileName);
	if (!outFile.is_open())
		return;

	outFile << "{}";
	outFile.close();
}

void CuScanApp::SaveDefectCondition(int iVisionIdx, CString sModelType)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = m_ModelDefineInfo.m_strVisionName_Short[iVisionIdx];

	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;
	CString sLGITParameterFolder, sModelFolder;
	sLGITParameterFolder.Format("C:\\lgit\\InspectParameter");
	if ((hFindFile = FindFirstFile(sLGITParameterFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
		CreateDirectory(sLGITParameterFolder, NULL);
	sModelFolder.Format("%s\\%s", sLGITParameterFolder, sModelType);
	if ((hFindFile = FindFirstFile(sModelFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
		CreateDirectory(sModelFolder, NULL);

	CString jsonFileName;
	jsonFileName.Format("%s\\DefectCondition_%s.json", sModelFolder, sVisionCamType_Short);
	nlohmann::json JDefectCondition;
	std::ifstream inFile(jsonFileName);
	if (inFile)
		inFile >> JDefectCondition;
	inFile.close();

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		CString sImageIdx;
		sImageIdx.Format("Image%02d", i);
		string strKey_ImageIdx = CT2A(sImageIdx);

		for (int j = 0; j < 10; j++)
		{
			CString sROINo;
			sROINo.Format("ROI%d", j);
			string strKey_ROINo = CT2A(sROINo);

			for (int k = 0; k < MAX_INSPECTION_TAB; k++)
			{
				CString sTabIdx;
				sTabIdx.Format("Tab%d", k + 1);
				string strKey_TabIdx = CT2A(sTabIdx);

				if (m_StructDefectCondition.bUseImageCompare[iVisionIdx][i][j][k])
				{
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["AreaUse"] = m_StructDefectCondition.bUseDefectConditionArea[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["AreaMin"] = m_StructDefectCondition.iDefectConditionAreaMin[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["AreaMax"] = m_StructDefectCondition.iDefectConditionAreaMax[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["LengthUse"] = m_StructDefectCondition.bUseDefectConditionLength[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["LengthMin"] = m_StructDefectCondition.iDefectConditionLengthMin[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["LengthMax"] = m_StructDefectCondition.iDefectConditionLengthMax[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["WidthUse"] = m_StructDefectCondition.bUseDefectConditionWidth[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["WidthMin"] = m_StructDefectCondition.iDefectConditionWidthMin[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["WidthMax"] = m_StructDefectCondition.iDefectConditionWidthMax[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["EllipseRatioUse"] = m_StructDefectCondition.bUseDefectConditionEllipseRatio[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["EllipseRatioMin"] = m_StructDefectCondition.dDefectConditionEllipseRatioMin[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["EllipseRatioMax"] = m_StructDefectCondition.dDefectConditionEllipseRatioMax[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["ContrastUse"] = m_StructDefectCondition.bUseDefectConditionContrast[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["ContrastMin"] = m_StructDefectCondition.iDefectConditionContrastMin[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["ContrastMax"] = m_StructDefectCondition.iDefectConditionContrastMax[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["OuterDistUse"] = m_StructDefectCondition.bUseDefectConditionOuterDist[iVisionIdx][i][j][k];
					JDefectCondition[strKey_ImageIdx][strKey_ROINo][strKey_TabIdx]["OuterDistMin"] = m_StructDefectCondition.iDefectConditionOuterDist[iVisionIdx][i][j][k];
				}
			}
		}
	}

	std::ofstream outFile(jsonFileName);
	outFile << JDefectCondition.dump(4);
	outFile.close();

	return;
}

void CuScanApp::ClearDefectCondition(int iVisionIdx, CString sModelType)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = m_ModelDefineInfo.m_strVisionName_Short[iVisionIdx];

	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;
	CString sLGITParameterFolder, sModelFolder;
	sLGITParameterFolder.Format("C:\\lgit\\InspectParameter");
	if ((hFindFile = FindFirstFile(sLGITParameterFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
		CreateDirectory(sLGITParameterFolder, NULL);
	sModelFolder.Format("%s\\%s", sLGITParameterFolder, sModelType);
	if ((hFindFile = FindFirstFile(sModelFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
		CreateDirectory(sModelFolder, NULL);

	CString jsonFileName;
	jsonFileName.Format("%s\\DefectCondition_%s.json", sModelFolder, sVisionCamType_Short);
	std::ofstream outFile(jsonFileName);
	if (!outFile.is_open())
		return;

	outFile << "{}";
	outFile.close();
}

void CuScanApp::ReadEquipModelInfoINI()
{
	CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_ModelInfo(strDataFolder + "ModelInfo.ini");
	CString strSection, strKey;

	strSection = "MODEL_INFO";
	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		strKey.Format("Vision Full Name_%02d", i + 1);
		m_ModelDefineInfo.m_strVisionName[i] = INI_ModelInfo.Get_String(strSection, strKey, "Undefine");

		strKey.Format("Vision Short Name_%02d", i + 1);
		m_ModelDefineInfo.m_strVisionName_Short[i] = INI_ModelInfo.Get_String(strSection, strKey, "UD");

		for (int j = 0; j < STAGE_NUMBER_MAX; j++)
		{
			strKey.Format("Vision Comm Name Stage%d_%02d", j + 1, i + 1);
			m_ModelDefineInfo.m_strVisionName_Comm[i][j] = INI_ModelInfo.Get_String(strSection, strKey, "UD");
		}

		strKey.Format("PWM_%02d", i + 1);
		m_ModelDefineInfo.m_bVisionPWM[i] = INI_ModelInfo.Get_Bool(strSection, strKey, FALSE);

		strKey.Format("Barcode Inspect_%02d", i + 1);
		m_ModelDefineInfo.m_bVisionBarcode[i] = INI_ModelInfo.Get_Bool(strSection, strKey, FALSE);

		strKey.Format("Grab Circular_%02d", i + 1);
		m_ModelDefineInfo.m_iVisionGrabCircular[i] = INI_ModelInfo.Get_Integer(strSection, strKey, GRAB_CIRCULAR_MAX);

		strKey.Format("Image Width_%02d", i + 1);
		m_ModelDefineInfo.m_iVisionImageWidth[i] = INI_ModelInfo.Get_Integer(strSection, strKey, DUMMY_IMAGE_SIZE);

		strKey.Format("Image Height_%02d", i + 1);
		m_ModelDefineInfo.m_iVisionImageHeight[i] = INI_ModelInfo.Get_Integer(strSection, strKey, DUMMY_IMAGE_SIZE);

		strKey.Format("Pixel Resolution_%02d", i + 1);
		m_ModelDefineInfo.m_iPixelResolution[i] = INI_ModelInfo.Get_Double(strSection, strKey, 5);

#ifdef INLINE_MODE
		strKey.Format("Inspection Buffer_%02d", i + 1);
		m_ModelDefineInfo.m_iMaxInspectionBufferCount[i] = INI_ModelInfo.Get_Integer(strSection, strKey, INSPECTION_BUFFER_COUNT_MAX);
#else
		strKey.Format("Inspection Buffer_Offline");
		m_ModelDefineInfo.m_iMaxInspectionBufferCount[i] = INI_ModelInfo.Get_Integer(strSection, strKey, INSPECTION_BUFFER_COUNT_MAX);
#endif

		for (int j = 0; j < MAX_RESULT_IMAGE; j++)
		{
			strKey.Format("Result Image InspectionType Name_%02d_%02d", i + 1, j + 1);
			m_ModelDefineInfo.m_strResultInspectionTypeName[i][j] = INI_ModelInfo.Get_String(strSection, strKey, "Unused");
		}

		strKey.Format("Barcode Result Image Number_%02d", i + 1);
		m_ModelDefineInfo.m_iBarcodeResultImageNo[i] = INI_ModelInfo.Get_Integer(strSection, strKey, 0);
	}
}

std::array<std::string, MAX_INSPECTION_TYPE> g_strInspectionTypeName;
std::array<std::string, MAX_INSPECTION_TYPE> g_strInspectionTypeName_Short;
void CuScanApp::ReadInspectionTypeInfoINI()
{
	CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
	CString strEquipModel = g_strModelTypeName[GetModelType()];
	CString strIniFile;
	strIniFile.Format("InspectionTypeInfo_%s.ini", strEquipModel);
	CIniFileCS INI_InspectionTypeInfo(strDataFolder + strIniFile);
	CString strSection, strKey;

	strSection = "INSPECTION TYPE";
	for (int i = 0; i < MAX_INSPECTION_TYPE; i++)
	{
		strKey.Format("Inspection Type Name_%02d", i + 1);
		g_strInspectionTypeName[i] = CT2A(INI_InspectionTypeInfo.Get_String(strSection, strKey, "Undefine"));

		strKey.Format("Inspection Type Short Name_%02d", i + 1);
		g_strInspectionTypeName_Short[i] = CT2A(INI_InspectionTypeInfo.Get_String(strSection, strKey, "UD"));
	}
}

inline const char* ToFolderName(LogCategory c)
{
	switch (c)
	{
		case LogCategory::System:     return "System";
		case LogCategory::Handler:    return "Handler";
		case LogCategory::Inspection: return "Inspection";
		case LogCategory::Halcon:     return "Halcon";
		case LogCategory::Scan:       return "Scan";
		case LogCategory::ADJ:        return "ADJ";
		case LogCategory::Image:      return "Image";
		case LogCategory::Thread:     return "Thread";
		case LogCategory::ETC:        return "ETC";
		default:                      return "ETC";
	}
}

static int ExtractDateYYYYMMDD(const std::string& filename)
{
	for (size_t i = 0; i + 8 <= filename.size(); ++i)
	{
		bool all_digit = true;
		for (size_t k = 0; k < 8; ++k)
		{
			if (!std::isdigit(static_cast<unsigned char>(filename[i + k])))
			{
				all_digit = false;
				break;
			}
		}
		if (!all_digit) continue;

		int yyyymmdd = std::stoi(filename.substr(i, 8));
		int yyyy = yyyymmdd / 10000;
		int mm = (yyyymmdd / 100) % 100;
		int dd = yyyymmdd % 100;

		if (yyyy >= 1990 && yyyy <= 2100 && mm >= 1 && mm <= 12 && dd >= 1 && dd <= 31)
			return yyyymmdd;
	}

	return -1;
}

namespace fs = std::filesystem;
static void CleanupLogsByFilenameDate(const std::string& root_folder_str, LogCategory category,	size_t keep_number)
{
	if (keep_number == 0)
		return;

	const char* catName = ToFolderName(category);

    fs::path folder = fs::path(root_folder_str) / catName;

	std::error_code ec;
	if (!fs::exists(folder, ec))
		return;

	struct Item
	{
		fs::path path;
		int date;
	};

	std::vector<Item> items;

	std::string log_name = std::string(catName) + "Log_";

	for (const auto& e : fs::directory_iterator(folder, ec))
	{
		if (ec)
			break;
		if (!e.is_regular_file())
			continue;

		const std::string name = e.path().filename().string();

		if (name.rfind(log_name, 0) != 0)
			continue;

		int d = ExtractDateYYYYMMDD(name);
		if (d < 0)
			continue;

		items.push_back({ e.path(), d });
	}

	// 날짜 기준 내림차순
	std::sort(items.begin(), items.end(),
			  [](const Item& a, const Item& b) {
				  return a.date > b.date;
			  });

	for (size_t i = keep_number; i < items.size(); ++i)
		fs::remove(items[i].path, ec);
}

struct daily_mid_date_filename_calculator
{
	static spdlog::filename_t calc_filename(const spdlog::filename_t& filename, const tm& now_tm)
	{
		spdlog::filename_t date = fmt::format(
			SPDLOG_FILENAME_T("{:04d}{:02d}{:02d}"),
			now_tm.tm_year + 1900,
			now_tm.tm_mon + 1,
			now_tm.tm_mday
		);

		spdlog::filename_t out = filename;
		const spdlog::filename_t token = SPDLOG_FILENAME_T("__");

		const auto pos = out.find(token);
		if (pos != spdlog::filename_t::npos)
		{
			// "__"를 "_YYYYMMDD_"로 교체
			spdlog::filename_t insert = fmt::format(SPDLOG_FILENAME_T("_{}_"), date);
			out.replace(pos, token.size(), insert);
		}

		return out;
	}
};

std::shared_ptr<spdlog::logger> CuScanApp::GetOrCreateCategoryLogger(const std::string& root_folder_str, int log_level, LogCategory category)
{
	std::string catName = ToFolderName(category);
	std::string logger_key = "Vision_" + catName;

	if (auto existing = spdlog::get(logger_key))
		return existing;

	std::filesystem::path cat_folder = std::filesystem::path(root_folder_str) / catName;
	std::filesystem::create_directories(cat_folder);

	int iPcNo = Struct_PreferenceStruct.m_iPCType + 1;

	std::string log_file_name = catName + "Log__PC" + std::to_string(iPcNo) + ".log";

	using my_daily_sink_t =
		spdlog::sinks::daily_file_sink<std::mutex, daily_mid_date_filename_calculator>;

	auto daily_sink = std::make_shared<my_daily_sink_t>(
		(cat_folder / log_file_name).string(), 0, 0, false, 365);

	std::vector<spdlog::sink_ptr> sinks{ daily_sink };

	auto logger = std::make_shared<spdlog::async_logger>(
		logger_key, sinks.begin(), sinks.end(),
		spdlog::thread_pool(), spdlog::async_overflow_policy::block);

	logger->set_pattern("[%H:%M:%S.%e] [%l] %v");

	if (log_level >= (int)spdlog::level::trace && log_level < (int)spdlog::level::n_levels)
		logger->set_level((spdlog::level::level_enum)log_level);
	else
		logger->set_level(spdlog::level::info);

	logger->flush_on(spdlog::level::info);

	spdlog::register_logger(logger);
	return logger;
}

BOOL CuScanApp::InitializeVisionLogger()
{
	try
	{
		std::string log_root_str = CT2A(GetWorkingDirectory() + "\\Data\\Log\\");

		CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
		CIniFileCS INI_ModelInfo(strDataFolder + "LogSetting.ini");

		int thread_number = INI_ModelInfo.Get_Integer("LOG SETTING", "Thread Number", 1);
		int keep_log_number = INI_ModelInfo.Get_Integer("LOG SETTING", "Keep Log Number", 30);
		int log_level;

		if (!spdlog::thread_pool())
			spdlog::init_thread_pool(8192, thread_number);

		CleanupLogsByFilenameDate(log_root_str, LogCategory::System, keep_log_number);
		log_level = INI_ModelInfo.Get_Integer("LOG SETTING", "System Log Level", 2);
		m_log_system = GetOrCreateCategoryLogger(log_root_str, log_level, LogCategory::System);
		if (m_log_system) m_log_system->info("Logger initialized. Log folder: {}\\System", log_root_str);

		CleanupLogsByFilenameDate(log_root_str, LogCategory::Handler, keep_log_number);
		log_level = INI_ModelInfo.Get_Integer("LOG SETTING", "Handler Log Level", 2);
		m_log_handler = GetOrCreateCategoryLogger(log_root_str, log_level, LogCategory::Handler);
		if (m_log_handler) m_log_handler->info("Logger initialized. Log folder: {}\\Handler", log_root_str);

		CleanupLogsByFilenameDate(log_root_str, LogCategory::Inspection, keep_log_number);
		log_level = INI_ModelInfo.Get_Integer("LOG SETTING", "Inspection Log Level", 2);
		m_log_inspection = GetOrCreateCategoryLogger(log_root_str, log_level, LogCategory::Inspection);
		if (m_log_inspection) m_log_inspection->info("Logger initialized. Log folder: {}\\Inspection", log_root_str);

		CleanupLogsByFilenameDate(log_root_str, LogCategory::Halcon, keep_log_number);
		log_level = INI_ModelInfo.Get_Integer("LOG SETTING", "Halcon Log Level", 2);
		m_log_halcon = GetOrCreateCategoryLogger(log_root_str, log_level, LogCategory::Halcon);
		if (m_log_halcon) m_log_halcon->info("Logger initialized. Log folder: {}\\Halcon", log_root_str);

		CleanupLogsByFilenameDate(log_root_str, LogCategory::Scan, keep_log_number);
		log_level = INI_ModelInfo.Get_Integer("LOG SETTING", "Scan Log Level", 2);
		m_log_scan = GetOrCreateCategoryLogger(log_root_str, log_level, LogCategory::Scan);
		if (m_log_scan) m_log_scan->info("Logger initialized. Log folder: {}\\Scan", log_root_str);

		CleanupLogsByFilenameDate(log_root_str, LogCategory::ADJ, keep_log_number);
		log_level = INI_ModelInfo.Get_Integer("LOG SETTING", "ADJ Log Level", 2);
		m_log_adj = GetOrCreateCategoryLogger(log_root_str, log_level, LogCategory::ADJ);
		if (m_log_adj) m_log_adj->info("Logger initialized. Log folder: {}\\ADJ", log_root_str);

		CleanupLogsByFilenameDate(log_root_str, LogCategory::Image, keep_log_number);
		log_level = INI_ModelInfo.Get_Integer("LOG SETTING", "Image Log Level", 2);
		m_log_image = GetOrCreateCategoryLogger(log_root_str, log_level, LogCategory::Image);
		if (m_log_image) m_log_image->info("Logger initialized. Log folder: {}\\Image", log_root_str);

		CleanupLogsByFilenameDate(log_root_str, LogCategory::Thread, keep_log_number);
		log_level = INI_ModelInfo.Get_Integer("LOG SETTING", "Thread Log Level", 2);
		m_log_thread = GetOrCreateCategoryLogger(log_root_str, log_level, LogCategory::Thread);
		if (m_log_thread) m_log_thread->info("Logger initialized. Log folder: {}\\Thread", log_root_str);

		CleanupLogsByFilenameDate(log_root_str, LogCategory::ETC, keep_log_number);
		log_level = INI_ModelInfo.Get_Integer("LOG SETTING", "ETC Log Level", 2);
		m_log_etc = GetOrCreateCategoryLogger(log_root_str, log_level, LogCategory::ETC);
		if (m_log_etc) m_log_etc->info("Logger initialized. Log folder: {}\\ETC", log_root_str);

		// m_log_system->flush();

		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to initialize vision loggers: " << e.what() << std::endl;
		return false;
	}
}
