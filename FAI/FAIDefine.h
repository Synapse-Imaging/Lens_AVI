#pragma once
#include "StdAfx.h"
#include <array>

// ============================================================
//  FAIDefine.h  -  FAI 검사 공통 정의 파일
// ============================================================
//
//  FAI(First Article Inspection) 검사 모듈 전반에서 공통으로
//  사용하는 상수, 열거형, 헬퍼 함수를 정의한다.
//
//  [구분]
//  1. 모델 공통 상수
//    - 측정 포인트 수, FAI 항목 수, 에러값, 단위 변환 등
//    - 이미지 블렌딩 위치/포인트 열거형 및 이름 배열
//    - FAI 로그 이름 생성 함수 (GetFAILogNames)
//
//  2. 측정 위치 (enMeasurePos) 및 헬퍼 함수
//    - 모델별 측정 위치 열거값
//      - ACT 모델 측정 위치 열거값 (ACT 전용 측정위치, 정상동작 하는 지 테스트 필요)
//    - int <-> enMeasurePos 변환 헬퍼 (MtoI / ItoM)
//    - 모델별 측정 위치 범위 상수 (START / END / TOTAL)
//
//  [사용 예시]
//    enMeasurePos pos = ItoM(3);
//    int idx = MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER);
//    double err = FAI_MEASURE_ERROR;   // 측정 실패값 (NaN)
//
//  [주의]
//    - enMeasurePos는 enum class이므로 정수와 직접 비교 불가.
//      반드시 MtoI() / ItoM() 변환 함수를 사용할 것.
//    - 새 측정 위치는 MEASURE_POSITION_ETC 앞에 삽입하고,
//      MEASURE_POSITION_END 이후에는 절대 추가하지 말 것.
//    - 측정 위치 UI 표시명은 FAIModelConfig.cpp 의
//      Create[ModelType]ModelConfig() 에서 관리한다.
// ============================================================


// ============================================================
//  1. 모델 공통 상수
// ============================================================

// 측정 포인트 최대 수
#define MAX_MEASURE_POINT_NUM 8

// FAI 항목 수 (FAI-0 ~ FAI-150)
constexpr int TOTAL_FAI_NUMBER = 151;
// 레거시 코드 호환용. "Unused" 로 표시할 FAI 항목 수
constexpr int TOTAL_NAMED_FAI_NUMBER = 111;

// 레거시 코드 호환용. 필요 없을 시 제거
#define MAX_FAI_ITEM            TOTAL_FAI_NUMBER // 최대 FAI 항목 수 150
// TODO : 기존 MAX_MEASURE_POINT_NUM 과 동일한지 여부를 확인
#define MAX_FAI_CIRCLE_FIT_POINT 8
#define MAX_FAI_REVIEW_IMAGE    10
#define NO_X_FAI_REVIEW_IMAGE   2

// FAI 측정값 초기화값 (에러값)
// 정의 : -9999 / 변경 : NaN (Not a Number) - FAI 측정값에서 유효하지 않은 값을 정확하게 표시하기 위해 NaN 사용
constexpr double FAI_MEASURE_ERROR = std::numeric_limits<double>::quiet_NaN();
// 레거시 코드 호환용. 필요 없을 시 제거
#define FAI_MEASURE_ERROR_VALUE FAI_MEASURE_ERROR

// 단위 변환 (um -> mm)
#define UNIT_SCALE_0_001 0.001

// FAI 번호에 해당 측정 위치를 넣어 해당 FAI를 결정하는데 쓰는 변환 (레거시 코드 호환용. 필요 없을 시 제거)
#define FAI_PARAMETER_NUMBER_1 0
#define FAI_PARAMETER_NUMBER_2 1
#define FAI_PARAMETER_NUMBER_3 2

// --- 이미지 블렌딩 위치/포인트 -------------------------------------------

// 이미지 합성 포인트
enum enBlendPos
{
    POSITION_BLEND_1_X,
    POSITION_BLEND_1_Y,
    POSITION_BLEND_2_X,
    POSITION_BLEND_2_Y,
    POSITION_REFER_1_X,
    POSITION_REFER_1_Y,
    POSITION_REFER_2_X,
    POSITION_REFER_2_Y,
    POSITION_BLEND_END
};

inline static CString g_strAkcBlendPosName[] = {
    _T("Blend 1 X"),
    _T("Blend 1 Y"),
    _T("Blend 2 X"),
    _T("Blend 2 Y"),
    _T("Refer 1 X"),
    _T("Refer 1 Y"),
    _T("Refer 2 X"),
    _T("Refer 2 Y"),
};

enum enBlendPoint
{
    BLEND_POINT_ST,
    BLEND_POINT_ED,
    BLEND_POINT_END
};

inline static CString g_strBlendPointName[] = {
    _T("Start"),
    _T("End"),
};

// --- FAI 로그 이름 -------------------------------------------------------

// FAI-0 ~ FAI-(TOTAL_NAMED_FAI_NUMBER-1) : "FAI-N" 형식
// FAI-TOTAL_NAMED_FAI_NUMBER ~ FAI-(TOTAL_FAI_NUMBER-1) : "Unused"
inline CString* GetFAILogNames()
{
    static CString g_strFAILogName[TOTAL_FAI_NUMBER];
    static bool bInitialized = false;

    if (!bInitialized)
    {
        for (int i = 0; i < TOTAL_NAMED_FAI_NUMBER; i++)
        {
            g_strFAILogName[i].Format(_T("FAI-%d"), i);
        }
        for (int i = TOTAL_NAMED_FAI_NUMBER; i < TOTAL_FAI_NUMBER; i++)
        {
            g_strFAILogName[i] = _T("Unused");
        }
        bInitialized = true;
    }

    return g_strFAILogName;
}

// 레거시 코드 호환용. 필요 없을 시 제거
#define g_strFAILogName GetFAILogNames()

// ============================================================
//  2. 측정 위치 (enMeasurePos) 및 헬퍼 함수
// ============================================================

/* enum class 를 사용하는 이유
 * - 정확한 타입 안전: enMeasurePos로 측정 위치를 나타내므로, 다른 int 값과 혼동을 방지할 수 있음
 * - 의도적인 타입변환 (static_cast<int>) 이외에는 int로 사용할 수 없어 실수 방지 가능
 * ex1 : IsFAIUsed(MEASURE_TOP_LENS_INNER)
 *       -> 컴파일 오류 (enMeasurePos 를 써야함)
 * ex2 : IsFAIUsed(enMeasurePos::MEASURE_TOP_LENS_INNER)
 *       -> 컴파일 오류 (Int 로 변환해야 함)
 * ex3 : IsFAIUsed(static_cast<int>(enMeasurePos::MEASURE_TOP_LENS_INNER))
 *       -> 올바른 사용
 * ex4 : IsFAIUsed(MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER))
 *       -> 올바른 사용 (권장)
 */
// NOTE : MEASURE_POSITION_END 는 측정 위치의 총 개수를 나타내므로, 열거 값에 대입하면 안됨 (범위 체크 용도로만 사용)

// !!! WARNNING - 측정 위치 추가시 주의점 !!!
//    1. 기존 사용되는 Position 값 이후와 MEASURE_POSITION_END 사이 추가 해야함. (안그러면 한 칸씩 밀려서 FAI 측정 실패)
//    2. 아래 enum 만 주의하고 UI 표시는 신경 안써도 됨. (FAIModelConfig.cpp 의 Create[ModelType]ModelConfig() 에서 관리)
enum class enMeasurePos
{
    // ===== Common Model Positions =====
    MEASURE_TOP_CHASIS_HOLE_NORTH = 0,
    MEASURE_TOP_CHASIS_HOLE_WEST,
    MEASURE_TOP_CHASIS_HOLE_EAST,
    MEASURE_TOP_CHASIS_TAB_NORTH,
    MEASURE_TOP_CHASIS_TAB_WEST,
    MEASURE_TOP_CHASIS_TAB_EAST,
    MEASURE_TOP_DATUM_NORTH,
    MEASURE_TOP_DATUM_WEST,
    MEASURE_TOP_DATUM_EAST,
    MEASURE_TOP_LENS_INNER,
    MEASURE_TOP_LENS_OUTER,
    MEASURE_TOP_WELDING_SPOT,
    MEASURE_TOP_WELDING_POCKET,
    MEASURE_TOP_SIDE_LENS_INNER,
    MEASURE_TOP_SIDE_LENS_OUTER,
    MEASURE_TOP_SIDE_VCM_BOTTOM,
    MEASURE_TOP_SHIELD_CAN_NORTH,
    MEASURE_TOP_SHIELD_CAN_SOUTH,
    MEASURE_TOP_HARRISON_FLEX_NORTH,
    MEASURE_TOP_HARRISON_FLEX_SOUTH,
    MEASURE_BOTTOM_CHASIS_HOLE_NORTH,
    MEASURE_BOTTOM_CHASIS_HOLE_WEST,
    MEASURE_BOTTOM_CHASIS_HOLE_EAST,
    MEASURE_BOTTOM_CHASIS_TAB_NORTH,
    MEASURE_BOTTOM_CHASIS_TAB_WEST,
    MEASURE_BOTTOM_CHASIS_TAB_EAST,
    MEASURE_BOTTOM_DATUM_OUTER_NORTH,
    MEASURE_BOTTOM_DATUM_OUTER_WEST,
    MEASURE_BOTTOM_DATUM_OUTER_EAST,
    MEASURE_BOTTOM_DATUM_INNER_NORTH,
    MEASURE_BOTTOM_DATUM_INNER_WEST,
    MEASURE_BOTTOM_DATUM_INNER_EAST,
    MEASURE_BOTTOM_VCM_NORTH,
    MEASURE_BOTTOM_VCM_WEST,
    MEASURE_BOTTOM_VCM_EAST,
    MEASURE_BOTTOM_FPCB_WEST,
    MEASURE_BOTTOM_FPCB_EAST,
    MEASURE_BOTTOM_FPCB_END,
    MEASURE_BOTTOM_FPCB_SOUTH,
    MEASURE_BOTTOM_CONNECTOR,

    // ===== BOI Model Positions =====
    // 현재까지 BOI 모델에서만 사용되는 측정 위치 - 260310
    // (FAI-38: 이미지가 다르고 Shield can(Horizontal) 을 측정)
    MEASURE_TOP_LENS_INNER_ANGLE,

    // ===== ACT Model Positions =====
    MEASURE_DATUM_A_SIDE_UPPER,
    MEASURE_DATUM_A_SIDE_LOWER,
    MEASURE_DATUM_B_TOP,
    MEASURE_DATUM_B_BOTTOM,
    MEASURE_DATUM_B_SIDE_UPPER,
    MEASURE_DATUM_B_SIDE_LOWER,
    MEASURE_DATUM_C_TOP,
    MEASURE_DATUM_C_SIDE_UPPER,
    MEASURE_DATUM_C_SIDE_LOWER,
    MEASURE_DATUM_D_SIDE_UPPER,
    MEASURE_DATUM_D_SIDE_LOWER,
    MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A,
    MEASURE_FIDUCIAL_PISMO_SIDE_LINE_B,
    MEASURE_FIDUCIAL_PISMO_SIDE_LINE_C,
    MEASURE_FIDUCIAL_PISMO_SIDE_LINE_D,
    MEASURE_FIDUCIAL_PISMO_SIDE_LINE_E,
    MEASURE_FIDUCIAL_PISMO_SIDE_LINE_F,
    MEASURE_FIDUCIAL_SIDE3_LINE_A,
    MEASURE_FIDUCIAL_SIDE3_LINE_B,
    MEASURE_FIDUCIAL_SIDE3_LINE_C,
    MEASURE_FIDUCIAL_SIDE3_LINE_D,
    MEASURE_DATUM_FAI_14,
    MEASURE_FAI_11,
    MEASURE_FAI_13,
    MEASURE_FAI_14,
    MEASURE_FAI_19,
    MEASURE_FAI_20,
    MEASURE_FAI_21,
    MEASURE_FAI_22,
    MEASURE_FAI_24,
    MEASURE_FAI_26,
    MEASURE_FAI_27,
    MEASURE_FAI_32,
    MEASURE_FAI_33,
    MEASURE_FAI_44,
    MEASURE_FAI_124,

    // ===== Common =====
    MEASURE_POSITION_ETC,

    // NOT USED - 측정 위치 추가 시 이 아래에 추가 금지 (범위 체크 용도)
    MEASURE_POSITION_NONE,
    MEASURE_POSITION_END
};

// --- enMeasurePos <-> int 변환 헬퍼 -------------------------------------

// enMeasurePos -> int (Measure to Int)
inline constexpr int MtoI(enMeasurePos enPos) { return static_cast<int>(enPos); }

// int -> enMeasurePos (Int to Measure). 범위 초과 시 MEASURE_POSITION_END 반환
inline constexpr enMeasurePos ItoM(int iPosID)
{
    if (iPosID >= 0 && iPosID < MtoI(enMeasurePos::MEASURE_POSITION_END))
    {
        return static_cast<enMeasurePos>(iPosID);
    }
    return enMeasurePos::MEASURE_POSITION_END;
}

// --- 모델별 측정 위치 범위 상수 ------------------------------------------

constexpr int COMMON_MEASURE_POS_START = MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH);
constexpr int COMMON_MEASURE_POS_END   = MtoI(enMeasurePos::MEASURE_BOTTOM_CONNECTOR) + 1;
constexpr int COMMON_MEASURE_POS_TOTAL = COMMON_MEASURE_POS_END - COMMON_MEASURE_POS_START;

constexpr int ACT_MEASURE_POS_START = MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER);
constexpr int ACT_MEASURE_POS_END   = MtoI(enMeasurePos::MEASURE_FAI_124) + 1;
constexpr int ACT_MEASURE_POS_TOTAL = ACT_MEASURE_POS_END - ACT_MEASURE_POS_START;

constexpr int MEASURE_POS_TOTAL = MtoI(enMeasurePos::MEASURE_POSITION_END);


