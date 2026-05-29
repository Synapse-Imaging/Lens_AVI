// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A17A74E6_03C5_40B8_986D_80E599544869__INCLUDED_)//
#define AFX_STDAFX_H__A17A74E6_03C5_40B8_986D_80E599544869__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <SDKDDKVer.h>

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// M%FC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions
#include <Shlwapi.h>		// Shell API for PathFileExists and PathFindExtension
#pragma comment(lib, "shlwapi.lib")

#define THEAPP			(*((CuScanApp*) AfxGetApp()))
#define SAFE_DELETE(p) if(p) { delete p; p = NULL; }
#include "mil.h"
#pragma comment(lib,"mil.lib")

#define _CRT_SECURE_NO_WARNINGS

////////////////////////////////////////// If Machine is Connected ->
//#define INLINE_MODE
////////////////////////////////////////// <- If Machine is Connected

// #define POC_TEST

// Model Change 
// #define UAVI_CHS_KS
// #define UAVI_CHS_WZ
// #define UAVI_AKC
// #define UAVI_BOI
// #define UAVI_ACT
// #define UAVI_CHS_TV
// #define UAVI_ATW
// #define UAVI_BOS
// #define UAVI_RENO
// #define UAVI_KRIOS
#define SINGLE_LENS
//#define ASSY_LENS

// AI 라이브러리 선택
#define SYAI
// #define LGITAI

// FAI 기능 사용
#define USE_FAI

// 면적당 조건 불러오기, 저장
//#define DEFECT_CONDITION

#ifdef INLINE_MODE
#define AJIN_BOARD_USE
#define RS232_USE
#define MOTION_USE
#define HANDLER_USE
#define GRAB_USE
//#define GRAB_SIMULATION
#else
#define ONE_THREAD_INSPECTION
#endif

//#define SNZEROMQ

#if !defined(SINGLE_LENS) && !defined(ASSY_LENS)
#define USE_DIFFUSED
#endif

//#define CREVIS_USE

#define USE_MATROX_TIMER

#define USE_SUAKIT

#define USE_DUMP_FILE //211203 add

#include "Define.h"

#include "HalconCpp.h"
using namespace HalconCpp;
#pragma comment(lib,"halconcppxl.lib") 

#define USE_CUDA
#ifdef USE_CUDA
#pragma comment(lib,"cudart.lib") 
#endif

#include "SR7Link.h"
#pragma comment (lib, "SR7Link.lib")

#ifdef CREVIS_USE
#include "VirtualFG40.h"
#pragma comment (lib, "VirtualFG40.lib")
#endif

#ifdef SNZEROMQ
// zeromq 서비스 관련 
#include <opencv2/opencv.hpp>
#include <snzeromq.client/Service/ZeroMQClient.h>
#include <snzeromq.common/Models/SNZeroMQLoopback.h>
#undef min
#undef max

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif        
#else
#include <opencv2/opencv.hpp>
#undef min
#undef max

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif 
#endif  // SNZEROMQ

#include "Delegate.h"
#include "MainFrm.h"

#include "PBC.h"	// PClass
#include "GFunction.h"

#include "PFun.h"

//Added by PIH 2005.11.18
#include <deque>

#include <afxmt.h>

// Modified By Go
#include "AlgorithmParam.h"
#include "MeasureData.h"

// added by ParkJH
#include <vector>

// Helper Function
BOOL PeekAndPump();
void edSaveLog(char* szFormat, ...);
double similarity(const HObject& XldOrg, const HObject& XldTgt);
void gen_bin_image_xld_crop(const HObject& Xld, HObject* Image, long lMarginX = 100, long lMarginY = 100);
void gen_bin_image_xld(const HObject& Xld, HObject* Image, long lMarginRight = 100, long lMarginBottom = 100);
Herror intersection_contours_lc(const HObject& Region, const HObject& Contours, HObject* pContoursIntersection);

#define LOGGING(X)			{ TRACE(X + CString("\n")); }

//2017.07.15 hbk
#define DEF_OCR_WHOLELINE	0
#define DEF_OCR_FIRSTLINE	1
#define DEF_OCR_SECONDLINE	2
#define DEF_OCR_THIRDLINE	3


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A17A74E6_03C5_40B8_986D_80E599544869__INCLUDED_)

#include <afxwin.h>
#include <afxcontrolbars.h>
#include <afxdlgs.h>

// Library 추가
#include "CSControls.h"
#include <afxwin.h>

#ifdef _DEBUG
#pragma comment (lib, "CSControlsD.lib")
#else
#pragma comment (lib, "CSControlsR.lib")
#endif

#include <afxcontrolbars.h>

// spdlog 라이브러리 설정
#if __has_include("spdlog/spdlog.h")
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#else 
// spdlog 라이브러리를 찾을 수 없어 빌드를 중지합니다.
#error "spdlog library not found. Please check submodule or library path."
#endif // __has_include("spdlog/spdlog.h")
// nlohmann::json 라이브러리 설정
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#else
#error "nlohmann::json library not found. Please check submodule or library path."
#endif // __has_include(<nlohmann/json.hpp>)

// 사용자 라이브러리 설정
#include "AIService/ImageUtile.h"
#include "AIService/ThreadPool.h"

// AI 라이브러리 설정
// SYAI 라이브러리 설정
#if __has_include("syai/runtime/inspection/inspectionService.h")
#ifndef SYAI
#ifndef LGITAI
//#define SYAI
#endif // !LGITAI
#endif // !SYAI
#else
#undef SYAI
#endif // __has_include("syai/runtime/inspection/inspectionService.h")

#ifdef SYAI
#undef LGITAI
#endif // SYAI

#ifdef SYAI
#include <syai/runtime/SimpleSmartType.h>
#include <syai/runtime/inspection/inspectionService.h>
#include <syai/runtime/domain/config/Configuration.h>
#include <syai/runtime/domain/config/ConfigurationBuilder.h>
#include <syai/runtime/domain/inspection/InspectionStepResult.h>
#include <syai/runtime/domain/inspection/Request.h>
#include <syai/runtime/domain/inspection/Response.h>
#endif // SYAI

#include <syai/runtime/inspection/inspectionService.h>


// LGITAI 라이브러리 설정
#ifdef LGITAI
#pragma comment(lib, "VariationSVM_Run.lib")
#pragma comment(lib, "Classification_Runtime.lib")
#pragma comment(lib, "Segmentation_Runtime.lib")
#pragma comment(lib, "Variation_Runtime.lib")

#include "LGIT/types.h"
#include "LGIT/LAIInspector.h"
#include "LGIT/LAIInspector2.h"
#include "LGIT/Model/LRequestedInspectionData1.h"
#include <tbb/concurrent_unordered_map.h>
#include "LGIT/Model/ImageROIManager.h"
#include "LGIT/InspectorManager.h"
#endif // LGITAI
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxwin.h>
