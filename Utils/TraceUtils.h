#pragma once

// TraceUtils.h - 간단한 함수 실행 시간 측정 유틸리티
// 사용법:
//   TRACE_CALL("MyFunction", MyFunction());  // MyFunction() 실행 시간 측정 및 TRACE 출력

#include "StdAfx.h"
#include <utility>

namespace TraceUtils
{
    template <typename Func>
    inline void TraceCall(const TCHAR* tag, Func&& func)
    {
        DWORD startTick = ::GetTickCount();
        std::forward<Func>(func)();
        DWORD elapsedTick = ::GetTickCount() - startTick;
        TRACE(_T("[TRACE_CALL] %s elapsed=%lu ms\n"), tag, (unsigned long)elapsedTick);
    }
}

#define TRACE_CALL(tag, expr) \
    do { \
        ::TraceUtils::TraceCall(_T(tag), [&]() { expr; }); \
    } while (0)
