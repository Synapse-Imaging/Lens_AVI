#pragma once

// DebugImageUtils.h - HALCON debug image dump utility
// Usage:
//   HALCON_DUMP_IMAGE("LensCircle", hImage);
//   HALCON_DUMP_REGION("DistancePl_Rgn", hRegion, 2048, 2048);
//   HALCON_DUMP_XLD("EdgeContour", hXld, 2048, 2048);
//
// Output path: C:\Debug\HalconDump\{tag}_{counter:03d}.bmp
// Active in DEBUG builds only. Release builds compile to no-ops.

#include "StdAfx.h"

#ifdef LOCAL_DUMP_MODE

#include <HalconCpp.h>
#include <string>
#include <shlwapi.h>   // PathFileExists, SHCreateDirectoryEx

#pragma comment(lib, "shlwapi.lib")

namespace DebugImageUtils
{
    // Global call counter (increments on every dump call, starting from 0)
    inline int& GetCounter()
    {
        static int s_nCounter = 0;
        return s_nCounter;
    }

    // Creates C:\Debug\HalconDump\ if it does not exist
    inline void EnsureOutputDir()
    {
        static bool s_bChecked = false;
        if (s_bChecked) return;
        s_bChecked = true;

        const char* pszDir = "C:\\Debug\\HalconDump";
        if (!::PathFileExistsA(pszDir))
        {
            ::SHCreateDirectoryExA(NULL, pszDir, NULL);
        }
    }

    // Builds output path: C:\Debug\HalconDump\{tag}_{counter:03d}.bmp
    inline std::string MakeFilePath(const char* pszTag)
    {
        char szPath[MAX_PATH];
        ::sprintf_s(szPath, "C:\\Debug\\HalconDump\\%s_%03d.bmp", pszTag, ++GetCounter());
        return std::string(szPath);
    }

    // Case 1 - HObject (Image type): WriteImage directly
    inline void DumpImage(const char* pszTag, const HObject& hImage)
    {
        EnsureOutputDir();
        try
        {
            std::string strPath = MakeFilePath(pszTag);
            WriteImage(hImage, "bmp", 0, HTuple(strPath.c_str()));
            TRACE("[HALCON_DUMP] Image saved: %s\n", strPath.c_str());
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

            TRACE("[HALCON_DUMP] DumpImage failed(%s): %s\n", pszTag, sErrMsg);
        }
    }

    // Case 2 - HObject (Region type): paint onto black canvas, then WriteImage
    inline void DumpRegion(const char* pszTag, const HObject& hRegion,
                           int nWidth, int nHeight)
    {
        EnsureOutputDir();
        try
        {
            HObject hCanvas, hResult;
            GenImageConst(&hCanvas, "byte",
                                  HTuple(nWidth), HTuple(nHeight));
            PaintRegion(hRegion, hCanvas, &hResult,
                                HTuple(255), "fill");

            std::string strPath = MakeFilePath(pszTag);
            WriteImage(hResult, "bmp", 0, HTuple(strPath.c_str()));
            TRACE("[HALCON_DUMP] Region saved: %s\n", strPath.c_str());
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

            TRACE("[HALCON_DUMP] DumpImage failed(%s): %s\n", pszTag, sErrMsg);
        }
    }

    // Case 3 - HObject (XLD type): paint onto black canvas, then WriteImage
    inline void DumpXld(const char* pszTag, const HObject& hXld,
                        int nWidth, int nHeight)
    {
        EnsureOutputDir();
        try
        {
            HObject hCanvas, hResult;
            GenImageConst(&hCanvas, "byte",
                                  HTuple(nWidth), HTuple(nHeight));
            PaintXld(hXld, hCanvas, &hResult, HTuple(255));

            std::string strPath = MakeFilePath(pszTag);
            WriteImage(hResult, "bmp", 0, HTuple(strPath.c_str()));
            TRACE("[HALCON_DUMP] XLD saved: %s\n", strPath.c_str());
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

            TRACE("[HALCON_DUMP] DumpImage failed(%s): %s\n", pszTag, sErrMsg);
        }
    }

} // namespace DebugImageUtils

// ------------------------------------------------------------------
// Public macros
// ------------------------------------------------------------------

// Dump HALCON image object to BMP file
#define HALCON_DUMP_IMAGE(tag, hImage) \
    do { ::DebugImageUtils::DumpImage((tag), (hImage)); } while (0)

// Dump HALCON region object to BMP file
// canvasWidth, canvasHeight: canvas size in pixels (match image resolution)
#define HALCON_DUMP_REGION(tag, hRegion, canvasWidth, canvasHeight) \
    do { ::DebugImageUtils::DumpRegion((tag), (hRegion), (canvasWidth), (canvasHeight)); } while (0)

// Dump HALCON XLD contour object to BMP file
// canvasWidth, canvasHeight: canvas size in pixels (match image resolution)
#define HALCON_DUMP_XLD(tag, hXld, canvasWidth, canvasHeight) \
    do { ::DebugImageUtils::DumpXld((tag), (hXld), (canvasWidth), (canvasHeight)); } while (0)

#else   // !_DEBUG

// Release build: all macros compile to no-ops (zero runtime overhead)
#define HALCON_DUMP_IMAGE(tag, hImage)                              ((void)0)
#define HALCON_DUMP_REGION(tag, hRegion, canvasWidth, canvasHeight) ((void)0)
#define HALCON_DUMP_XLD(tag, hXld, canvasWidth, canvasHeight)       ((void)0)

#endif  // _DEBUG
