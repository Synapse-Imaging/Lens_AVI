#pragma once
#include "FAIDefine.h"
#include <vector>

// ============================================================
//  FAIModelConfig.h  -  FAI 모델별 설정 정의 파일
// ============================================================
//
//  모델별 FAI 설정 구조체 및 생성 함수, 그리고
//  FAI 모델 설정에 종속된 상수 namespace를 정의한다.
//
//  [구분]
//  1. 모델 설정 구조체
//    - MeasurePosDescriptor : 측정 위치 UI 표시 정보
//    - FAIItemDescriptor    : FAI 항목 (번호, 헤더명, 측정위치)
//    - FAIModelConfig       : 모델 전체 FAI 설정
//    - Create[Model]ModelConfig() 생성 함수 선언
//
//  2. LOT 결과 헤더 인덱스 (LOT_HEADER_INDEX)
//    - LOT 결과 CSV/로그 파일의 열(column) 인덱스
//
//  3. 모델별 FAI 번호
//    - BOI_FAI : BOI 모델 FAI 항목 번호
//    - ATW_FAI : ATW 모델 FAI 항목 번호
//    - AKC_FAI : AKC 모델 FAI 항목 번호
//    - ACT_FAI : ACT 모델 FAI 항목 번호
// ============================================================

// ============================================================
//  1. 모델 설정 구조체
// ============================================================

// 측정 위치를 정의하는 구조체 (ROI 용도변경 -> 측정위치 UI 표시에 사용)
struct MeasurePosDescriptor
{
    enMeasurePos pos;
    LPCTSTR displayName;
};

// FAI 항목을 정의하는 구조체 (로그 저장 및 UI 표시 사용)
struct FAIItemDescriptor
{
    int faiNumber;
    // TODO : logHeaderNames 는 하나. 단일 자료형으로 수정
    std::vector<LPCTSTR> logHeaderNames;
    // UI 표시에 사용됨. (FAI 측정 결과 디스플레이 -> FAI#n = xx.xx mm)
    // 측정 위치가 없는 항목은 MEASURE_POSITION_NONE 를 사용한다.
    enMeasurePos measurePosition = enMeasurePos::MEASURE_POSITION_NONE;
};

// 위 두 구조체를 포함하는 모델별 FAI 설정 구조체
struct FAIModelConfig
{
    std::vector<MeasurePosDescriptor> measurePositions;
    std::vector<FAIItemDescriptor> faiItems;
    // TODO : 모델별로 추가적인 LOG 헤더가 필요한 경우를 고려하여 벡터로 선언.
    std::vector<LPCTSTR> extraLogHeaders;
};

// 모델별 FAI 설정을 생성하는 함수 선언
FAIModelConfig CreateBOIModelConfig();
FAIModelConfig CreateACTModelConfig();
FAIModelConfig CreateAKCModelConfig();
FAIModelConfig CreateATWModelConfig();
FAIModelConfig CreateBOSModelConfig();
FAIModelConfig CreateRENOModelConfig();
FAIModelConfig CreateKRIOSModelConfig();

// 공통 FAI 모델 설정
std::vector<MeasurePosDescriptor> GetCommonMeasurePositions();

// ============================================================
//  2. LOT 결과 헤더 인덱스
// ============================================================

// LOT 결과 CSV/로그 파일의 열(column) 인덱스
namespace LOT_HEADER_INDEX
{
    inline const int BARCODE       = 0;
    inline const int DATE          = 1;
    inline const int TIME          = 2;
    inline const int MACHINE_CODE  = 3;
    inline const int LOT_NUM       = 4;
    inline const int CONFIG        = 5;
    inline const int TRAY          = 6;
    inline const int MODULE_NO     = 7;
    inline const int STAGE_NO      = 8;
    inline const int JIG_NO        = 9;
    inline const int TOP_RESULT    = 10;
    inline const int BOTTOM_RESULT = 11;
    inline const int FAI_NG_ITEM   = 12;
    inline const int FAI_START     = 13; // FAI 데이터는 13번째부터 시작
}

// ============================================================
//  3. 모델별 FAI 번호 
//  TODO : 코드 상의 FAI 번호는 실제 MSOP 문서상의 FAI 번호와는 의미가 조금 다르다.
//       : 코드 상의 FAI 번호는 (문서상의 FAI 번호) + (UI 혹은 로그 출력에 필요한 FAI 관련 값을 인덱싱 하기 위한 상수)
//       : 추후 혼동이 생길 우려가 있음.
// ============================================================

// BOI 모델 FAI 항목 번호
namespace BOI_FAI
{
    inline const int LENS_INNER_DIAMETER = 5;  // FAI-5 : Top Lens Inner 직경
    inline const int LENS_INNER_X        = 6;  // FAI-5X : Top Lens Inner X 좌표 (Datum West 기준)
    inline const int LENS_INNER_Y        = 7;  // FAI-5Y : Top Lens Inner Y 좌표 (Datum North 기준)
    inline const int LENS_OUTER_DIAMETER = 11; // FAI-11: Top Lens Outer 직경
    inline const int SHIELD_CAN_DISTANCE = 38; // FAI-38: Shield Can ~ Harrison Flex 거리
}

// ATW 모델 FAI 항목 번호
namespace ATW_FAI
{
    inline const int LENS_INNER_DIAMETER = 4;  // FAI-4 : Top Lens Inner 직경
}

// BOS 모델 FAI 항목 번호
namespace BOS_FAI
{
    inline const int BRACKET_HOLE_CONCENTRICITY = 1;  // FAI-1   : Top Bracket Hole 동심도
    inline const int LENS_INNER = 2;                  // DATUM_D : Top Lens 중심좌표를 원점으로 삼음
    inline const int DATUM_E = 3;                     // DATUM_E : X 축으로 사용되는 Datum (MSOP 문서 참고)
}

// AKC 모델 FAI 항목 번호
namespace AKC_FAI
{
    inline const int LENS_INNER_DIAMETER      = 14;  // FAI-14 : Top Lens Inner 직경
    inline const int LENS_INNER_X             = 15;  // FAI-15 : Top Lens Inner X
    inline const int VCM_NORTH                = 19;  // FAI-19 : Bottom VCM North
    inline const int VCM_WEST                 = 20;  // FAI-20 : Bottom VCM West
    inline const int LENS_INNER_Y             = 21;  // FAI-21 : Top Lens Inner Y
    inline const int LENS_DATUM_WEST          = 43;  // FAI-43 : Top Lens Inner to Datum West
    inline const int LENS_DATUM_NORTH         = 44;  // FAI-44 : Top Lens Inner to Datum North
    inline const int FPCB_SOUTH               = 70;  // FAI-70 : Bottom FPCB South
    inline const int CONNECTOR_HEIGHT         = 71;  // FAI-71 : Bottom Connector 높이
    inline const int FPCB_END                 = 81;  // FAI-81 : Bottom FPCB End
    inline const int CONNECTOR_WIDTH          = 113; // FAI-113: Bottom Connector 폭
    inline const int FPCB_WEST                = 119; // FAI-119: Bottom FPCB West
    inline const int FPCB_EAST                = 120; // FAI-120: Bottom FPCB East
    inline const int FAI_126                  = 126; // FAI-126
    inline const int FAI_127                  = 127; // FAI-127
    inline const int FAI_128                  = 128; // FAI-128
    inline const int FAI_129                  = 129; // FAI-129
    inline const int FAI_130                  = 130; // FAI-130

    // 테스트용 FAI 번호 (140~149)
    inline const int TEST_140                 = 140;
    inline const int TEST_141                 = 141;
    inline const int TEST_142                 = 142;
    inline const int TEST_143                 = 143;
    inline const int TEST_144                 = 144;
    inline const int TEST_145                 = 145;
    inline const int TOP_CHASIS_HOLE_EAST     = 146; // FAI-146: Top Chasis Hole East (TEST)
    inline const int TOP_CHASIS_HOLE_NORTH    = 147; // FAI-147: Top Chasis Hole North (TEST)
    inline const int BOTTOM_CHASIS_HOLE_EAST  = 148; // FAI-148: Bottom Chasis Hole East (TEST)
    inline const int BOTTOM_CHASIS_HOLE_NORTH = 149; // FAI-149: Bottom Chasis Hole North (TEST)
}

// ACT 모델 FAI 항목 번호
namespace ACT_FAI
{
    inline const int PISMO_SIDE_LINE_A   = 5;  // FAI-5 : Fiducial Pismo Side Line A
    inline const int PISMO_SIDE_LINE_A_2 = 9;  // FAI-9 : Fiducial Pismo Side Line A (2nd)
    inline const int PISMO_SIDE_LINE_C   = 10; // FAI-10: Fiducial Pismo Side Line C
    inline const int FAI_11              = 11;
    inline const int FAI_13              = 13;
    inline const int FAI_14              = 14;
    inline const int FAI_19              = 19;
    inline const int FAI_20              = 20;
    inline const int FAI_21              = 21;
    inline const int FAI_22              = 22;
    inline const int FAI_24              = 24;
    inline const int FAI_26              = 26;
    inline const int FAI_27              = 27;
    inline const int SIDE3_LINE_A        = 29; // FAI-29: Fiducial Side3 Line A
    inline const int SIDE3_LINE_C        = 31; // FAI-31: Fiducial Side3 Line C
    inline const int FAI_32              = 32;
    inline const int FAI_33              = 33;
    inline const int SIDE3_LINE_C_2      = 34; // FAI-34: Fiducial Side3 Line C (2nd)
    inline const int FAI_44              = 44;
    inline const int FAI_124             = 124;
}

// RENO 紐⑤뜽 FAI ??ぉ 踰덊샇
namespace RENO_FAI
{
    inline const int LENS_INNER_DIAMETER = 4;  // FAI-4 : Top Lens Inner 吏곴꼍
}

// KRIOS 紐⑤뜽 FAI ??ぉ 踰덊샇
namespace KRIOS_FAI
{
    inline const int LENS_INNER_DIAMETER = 4;  // FAI-4 : Top Lens Inner 吏곴꼍
}
