#include "StdAfx.h"
#include "FAIModelConfig.h"
#include "FAIDataManager.h"

/**
 * @file FAIModelConfig.cpp
 * @brief FAI 관련 변수 한 곳에서 관리하는 CPP 파일
 *
 * FAI 추가 및 제거는 이 파일에서 관리.
 *
 * 각 모델별(BOI, ACT, AKC, ATW 등)로 FAI 검사 항목, 측정 위치, LOT 헤더를 설정하는 함수들을 포함.
 * 여기서 정의한 FAI 컨피그는 UI 표시, 데이터 저장, 검사 항목 관리에 사용된다.
 */
// 공통 FAI 모델 설정
std::vector<MeasurePosDescriptor> GetCommonMeasurePositions()
{
    return {
        {enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH, _T("Top Chasis Hole North")},
        {enMeasurePos::MEASURE_TOP_CHASIS_HOLE_WEST, _T("Top Chasis Hole West")},
        {enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST, _T("Top Chasis Hole East")},
        {enMeasurePos::MEASURE_TOP_CHASIS_TAB_NORTH, _T("Top Chasis Tab North")},
        {enMeasurePos::MEASURE_TOP_CHASIS_TAB_WEST, _T("Top Chasis Tab West")},
        {enMeasurePos::MEASURE_TOP_CHASIS_TAB_EAST, _T("Top Chasis Tab East")},
        {enMeasurePos::MEASURE_TOP_DATUM_NORTH, _T("Top Datum North")},
        {enMeasurePos::MEASURE_TOP_DATUM_WEST, _T("Top Datum West")},
        {enMeasurePos::MEASURE_TOP_DATUM_EAST, _T("Top Datum East")},
        {enMeasurePos::MEASURE_TOP_LENS_INNER, _T("Top Lens Inner")},
        {enMeasurePos::MEASURE_TOP_LENS_INNER_ANGLE, _T("Top Lens Inner Angle")},
        {enMeasurePos::MEASURE_TOP_LENS_OUTER, _T("Top Lens Outer")},
        {enMeasurePos::MEASURE_TOP_WELDING_SPOT, _T("Top Welding Spot")},
        {enMeasurePos::MEASURE_TOP_WELDING_POCKET, _T("Top Welding Pocket")},
        {enMeasurePos::MEASURE_TOP_SIDE_LENS_INNER, _T("Top Side Lens Inner")},
        {enMeasurePos::MEASURE_TOP_SIDE_LENS_OUTER, _T("Top Side Lens Outer")},
        {enMeasurePos::MEASURE_TOP_SIDE_VCM_BOTTOM, _T("Top Side VCM Bottom")},
        {enMeasurePos::MEASURE_TOP_SHIELD_CAN_NORTH, _T("Top Shield Can North")},
        {enMeasurePos::MEASURE_TOP_SHIELD_CAN_SOUTH, _T("Top Shield Can South")},
        {enMeasurePos::MEASURE_TOP_HARRISON_FLEX_NORTH, _T("Top Harrison Flex North")},
        {enMeasurePos::MEASURE_TOP_HARRISON_FLEX_SOUTH, _T("Top Harrison Flex South")},
        {enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_NORTH, _T("Bottom Chasis Hole North")},
        {enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_WEST, _T("Bottom Chasis Hole West")},
        {enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_EAST, _T("Bottom Chasis Hole East")},
        {enMeasurePos::MEASURE_BOTTOM_CHASIS_TAB_NORTH, _T("Bottom Chasis Tab North")},
        {enMeasurePos::MEASURE_BOTTOM_CHASIS_TAB_WEST, _T("Bottom Chasis Tab West")},
        {enMeasurePos::MEASURE_BOTTOM_CHASIS_TAB_EAST, _T("Bottom Chasis Tab East")},
        {enMeasurePos::MEASURE_BOTTOM_DATUM_OUTER_NORTH, _T("Bottom Datum Outer North")},
        {enMeasurePos::MEASURE_BOTTOM_DATUM_OUTER_WEST, _T("Bottom Datum Outer West")},
        {enMeasurePos::MEASURE_BOTTOM_DATUM_OUTER_EAST, _T("Bottom Datum Outer East")},
        {enMeasurePos::MEASURE_BOTTOM_DATUM_INNER_NORTH, _T("Bottom Datum Inner North")},
        {enMeasurePos::MEASURE_BOTTOM_DATUM_INNER_WEST, _T("Bottom Datum Inner West")},
        {enMeasurePos::MEASURE_BOTTOM_DATUM_INNER_EAST, _T("Bottom Datum Inner East")},
        {enMeasurePos::MEASURE_BOTTOM_VCM_NORTH, _T("Bottom VCM North")},
        {enMeasurePos::MEASURE_BOTTOM_VCM_WEST, _T("Bottom VCM West")},
        {enMeasurePos::MEASURE_BOTTOM_VCM_EAST, _T("Bottom VCM East")},
        {enMeasurePos::MEASURE_BOTTOM_FPCB_WEST, _T("Bottom FPCB West")},
        {enMeasurePos::MEASURE_BOTTOM_FPCB_EAST, _T("Bottom FPCB East")},
        {enMeasurePos::MEASURE_BOTTOM_FPCB_END, _T("Bottom FPCB End")},
        {enMeasurePos::MEASURE_BOTTOM_FPCB_SOUTH, _T("Bottom FPCB South")},
        {enMeasurePos::MEASURE_BOTTOM_CONNECTOR, _T("Bottom Connector")},
        {enMeasurePos::MEASURE_POSITION_ETC, _T("ETC")}
    };
}

////////////////////////////////////////////////////////////////
// BOI Model Config
////////////////////////////////////////////////////////////////
FAIModelConfig CreateBOIModelConfig()
{
    FAIModelConfig config;

    // ===== Measurement Positions =====
    // Enum 값과 UI (ROI 세팅-> 측정 위치 콤보박스) 에 표시되는 이름을 매핑
    config.measurePositions = GetCommonMeasurePositions();

    // ===== BOI FAI Items =====
    // FAI 번호, LOG 헤더 이름, 측정 위치 매핑.
    //   : {faiNumber, {logHeaderNames}, measurePosition}
    config.faiItems = {
        {{BOI_FAI::LENS_INNER_DIAMETER},        // FAI 상수 (번호)
         {_T("FAI-5")},                         // FAI 로그 헤더
         enMeasurePos::MEASURE_TOP_LENS_INNER}, // 측정 위치
        {{BOI_FAI::LENS_INNER_X},
         {_T("FAI-5-X")},
         enMeasurePos::MEASURE_TOP_DATUM_WEST},
        {{BOI_FAI::LENS_INNER_Y},
         {_T("FAI-5-Y")},
         enMeasurePos::MEASURE_TOP_DATUM_NORTH},
        {{BOI_FAI::LENS_OUTER_DIAMETER},
         {_T("FAI-11")},
         enMeasurePos::MEASURE_TOP_LENS_OUTER},
        {{BOI_FAI::SHIELD_CAN_DISTANCE},
         {_T("FAI-38(degree)")},
         enMeasurePos::MEASURE_TOP_LENS_INNER_ANGLE},
    };

    return config;
}

////////////////////////////////////////////////////////////////
// ACT Model Config
////////////////////////////////////////////////////////////////
FAIModelConfig CreateACTModelConfig()
{
    FAIModelConfig config;

    // ===== Measurement Positions =====
    config.measurePositions = {
        {enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER, _T("Datum A Side Upper")},
        {enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER, _T("Datum A Side Lower")},
        {enMeasurePos::MEASURE_DATUM_B_TOP, _T("Datum B Top")},
        {enMeasurePos::MEASURE_DATUM_B_BOTTOM, _T("Datum B Bottom")},
        {enMeasurePos::MEASURE_DATUM_B_SIDE_UPPER, _T("Datum B Side Upper")},
        {enMeasurePos::MEASURE_DATUM_B_SIDE_LOWER, _T("Datum B Side Lower")},
        {enMeasurePos::MEASURE_DATUM_C_TOP, _T("Datum C Top")},
        {enMeasurePos::MEASURE_DATUM_C_SIDE_UPPER, _T("Datum C Side Upper")},
        {enMeasurePos::MEASURE_DATUM_C_SIDE_LOWER, _T("Datum C Side Lower")},
        {enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER, _T("Datum D Side Upper")},
        {enMeasurePos::MEASURE_DATUM_D_SIDE_LOWER, _T("Datum D Side Lower")},
        {enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A, _T("Fiducial Pismo Side Line A")},
        {enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_B, _T("Fiducial Pismo Side Line B")},
        {enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_C, _T("Fiducial Pismo Side Line C")},
        {enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_D, _T("Fiducial Pismo Side Line D")},
        {enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_E, _T("Fiducial Pismo Side Line E")},
        {enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_F, _T("Fiducial Pismo Side Line F")},
        {enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_A, _T("Fiducial Side3 Line A")},
        {enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_B, _T("Fiducial Side3 Line B")},
        {enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_C, _T("Fiducial Side3 Line C")},
        {enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_D, _T("Fiducial Side3 Line D")},
        {enMeasurePos::MEASURE_DATUM_FAI_14, _T("Datum FAI-14")},
        {enMeasurePos::MEASURE_FAI_11, _T("FAI-11")},
        {enMeasurePos::MEASURE_FAI_13, _T("FAI-13")},
        {enMeasurePos::MEASURE_FAI_14, _T("FAI-14")},
        {enMeasurePos::MEASURE_FAI_19, _T("FAI-19")},
        {enMeasurePos::MEASURE_FAI_20, _T("FAI-20")},
        {enMeasurePos::MEASURE_FAI_21, _T("FAI-21")},
        {enMeasurePos::MEASURE_FAI_22, _T("FAI-22")},
        {enMeasurePos::MEASURE_FAI_24, _T("FAI-24")},
        {enMeasurePos::MEASURE_FAI_26, _T("FAI-26")},
        {enMeasurePos::MEASURE_FAI_27, _T("FAI-27")},
        {enMeasurePos::MEASURE_FAI_32, _T("FAI-32")},
        {enMeasurePos::MEASURE_FAI_33, _T("FAI-33")},
        {enMeasurePos::MEASURE_FAI_44, _T("FAI-44")},
        {enMeasurePos::MEASURE_FAI_124, _T("FAI-124")},
    };

    // ===== FAI Items =====
    config.faiItems = {
        {ACT_FAI::PISMO_SIDE_LINE_A, {_T("FAI-5")}, enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A},
        {ACT_FAI::PISMO_SIDE_LINE_A_2, {_T("FAI-9")}, enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A},
        {ACT_FAI::PISMO_SIDE_LINE_C, {_T("FAI-10")}, enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_C},
        {ACT_FAI::FAI_24, {_T("FAI-24")}, enMeasurePos::MEASURE_FAI_24},
        {ACT_FAI::SIDE3_LINE_A, {_T("FAI-29")}, enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_A},
        {ACT_FAI::SIDE3_LINE_C, {_T("FAI-31")}, enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_C},
        {ACT_FAI::SIDE3_LINE_C_2, {_T("FAI-34")}, enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_C},
        {ACT_FAI::FAI_11, {_T("FAI-11A"), _T("FAI-11B")}, enMeasurePos::MEASURE_FAI_11},
        {ACT_FAI::FAI_13, {_T("FAI-13A"), _T("FAI-13B")}, enMeasurePos::MEASURE_FAI_13},
        {ACT_FAI::FAI_14, {_T("FAI-14A"), _T("FAI-14B")}, enMeasurePos::MEASURE_FAI_14},
        {ACT_FAI::FAI_26, {_T("FAI-26A"), _T("FAI-26B")}, enMeasurePos::MEASURE_FAI_26},
        {ACT_FAI::FAI_27, {_T("FAI-27A"), _T("FAI-27B")}, enMeasurePos::MEASURE_FAI_27},
        {ACT_FAI::FAI_32, {_T("FAI-32A"), _T("FAI-32B")}, enMeasurePos::MEASURE_FAI_32},
        {ACT_FAI::FAI_33, {_T("FAI-33A"), _T("FAI-33B")}, enMeasurePos::MEASURE_FAI_33},
        {ACT_FAI::FAI_19, {_T("FAI-19")}, enMeasurePos::MEASURE_FAI_19},
        {ACT_FAI::FAI_20, {_T("FAI-20")}, enMeasurePos::MEASURE_FAI_20},
        {ACT_FAI::FAI_21, {_T("FAI-21")}, enMeasurePos::MEASURE_FAI_21},
        {ACT_FAI::FAI_22, {_T("FAI-22")}, enMeasurePos::MEASURE_FAI_22},
        {ACT_FAI::FAI_44, {_T("FAI-44")}, enMeasurePos::MEASURE_FAI_44},
        {ACT_FAI::FAI_124, {_T("FAI-124A"), _T("FAI-124B")}, enMeasurePos::MEASURE_FAI_124},
    };

    return config;
}

////////////////////////////////////////////////////////////////
// AKC Model Config
////////////////////////////////////////////////////////////////
FAIModelConfig CreateAKCModelConfig()
{
    FAIModelConfig config;

    // ===== Measurement Positions =====
    config.measurePositions = GetCommonMeasurePositions();

    // ===== FAI Items =====
    config.faiItems = {
        {AKC_FAI::LENS_INNER_DIAMETER, {_T("FAI-14")}, enMeasurePos::MEASURE_TOP_LENS_INNER},
        {AKC_FAI::LENS_INNER_X, {_T("FAI-14-X")}},
        {AKC_FAI::LENS_INNER_Y, {_T("FAI-14-Y")}},
        {AKC_FAI::LENS_DATUM_WEST, {_T("FAI-43")}, enMeasurePos::MEASURE_TOP_DATUM_WEST},
        {AKC_FAI::LENS_DATUM_NORTH, {_T("FAI-44")}, enMeasurePos::MEASURE_TOP_DATUM_NORTH},
        {AKC_FAI::FAI_126, {_T("FAI-126")}},
        {AKC_FAI::FAI_127, {_T("FAI-127")}},
        {AKC_FAI::FAI_128, {_T("FAI-128")}},
        {AKC_FAI::FAI_129, {_T("FAI-129")}},
        {AKC_FAI::FAI_130, {_T("FAI-130")}},
        {AKC_FAI::VCM_NORTH, {_T("FAI-19")}, enMeasurePos::MEASURE_BOTTOM_VCM_NORTH},
        {AKC_FAI::VCM_WEST, {_T("FAI-20")}, enMeasurePos::MEASURE_BOTTOM_VCM_WEST},
        {AKC_FAI::FPCB_WEST, {_T("FAI-119")}, enMeasurePos::MEASURE_BOTTOM_FPCB_WEST},
        {AKC_FAI::FPCB_EAST, {_T("FAI-120")}, enMeasurePos::MEASURE_BOTTOM_FPCB_EAST},
        {AKC_FAI::CONNECTOR_HEIGHT, {_T("FAI-71")}, enMeasurePos::MEASURE_BOTTOM_CONNECTOR},
        {AKC_FAI::CONNECTOR_WIDTH, {_T("FAI-113")}, enMeasurePos::MEASURE_BOTTOM_CONNECTOR},
        {AKC_FAI::FPCB_SOUTH, {_T("FAI-70")}, enMeasurePos::MEASURE_BOTTOM_FPCB_SOUTH},
        {AKC_FAI::FPCB_END, {_T("FAI-81")}, enMeasurePos::MEASURE_BOTTOM_FPCB_END},
        // Test FAI (no lot headers)
        {AKC_FAI::TEST_140, {}},
        {AKC_FAI::TEST_141, {}},
        {AKC_FAI::TEST_142, {}},
        {AKC_FAI::TEST_143, {}},
        {AKC_FAI::TEST_144, {}},
        {AKC_FAI::TEST_145, {}},
        {AKC_FAI::TOP_CHASIS_HOLE_EAST, {}, enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST},
        {AKC_FAI::TOP_CHASIS_HOLE_NORTH, {}, enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH},
        {AKC_FAI::BOTTOM_CHASIS_HOLE_EAST, {}, enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_EAST},
        {AKC_FAI::BOTTOM_CHASIS_HOLE_NORTH, {}, enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_NORTH},
    };

    // ===== Extra LOT Headers =====
    config.extraLogHeaders = {
        _T("Top North-East Chasis Distance"),
        _T("Top North Chasis Radius"),
        _T("Bottom North-West Chasis Distance"),
        _T("Bottom North Chasis Radius"),
    };

    return config;
}

////////////////////////////////////////////////////////////////
// ATW Model Config
////////////////////////////////////////////////////////////////
FAIModelConfig CreateATWModelConfig()
{
    /**
     * @brief ATW Model FAI Model Config 생성
     */
    FAIModelConfig config;

    // ===== Measurement Positions =====
    config.measurePositions = GetCommonMeasurePositions();

    // ===== ATW FAI Items =====
    config.faiItems = {
        {{ATW_FAI::LENS_INNER_DIAMETER},        // FAI 상수 (번호)
         {_T("FAI-4")},                         // FAI 로그 헤더
         enMeasurePos::MEASURE_TOP_LENS_INNER}, // 측정 위치
    };

    return config;
}

////////////////////////////////////////////////////////////////
// BOS Model Config
////////////////////////////////////////////////////////////////
FAIModelConfig CreateBOSModelConfig()
{
    FAIModelConfig config;

    // ===== Measurement Positions =====
    config.measurePositions = GetCommonMeasurePositions();

    // ===== BOS FAI Items =====
    config.faiItems = {
        {{BOS_FAI::BRACKET_HOLE_CONCENTRICITY}, 
         {_T("FAI-1")}, 
         enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST},
        {{BOS_FAI::DATUM_E}},
        {{BOS_FAI::LENS_INNER}},
    };

    return config;
}

////////////////////////////////////////////////////////////////
// RENO Model Config
////////////////////////////////////////////////////////////////
FAIModelConfig CreateRENOModelConfig()
{
	/**
	 * @brief RENO Model FAI Model Config ?앹꽦
	 */
	FAIModelConfig config;

	// ===== Measurement Positions =====
	config.measurePositions = GetCommonMeasurePositions();

	// ===== RENO FAI Items =====
	config.faiItems = {
		{{RENO_FAI::LENS_INNER_DIAMETER},        // FAI ?곸닔 (踰덊샇)
		 {_T("FAI-1")},                         // FAI 濡쒓렇 ?ㅻ뜑
		 enMeasurePos::MEASURE_TOP_LENS_INNER}, // 痢≪젙 ?꾩튂
	};

	return config;
}

////////////////////////////////////////////////////////////////
// KRIOS Model Config
////////////////////////////////////////////////////////////////
FAIModelConfig CreateKRIOSModelConfig()
{
    /**
     * @brief KRIOS Model FAI Model Config ?앹꽦
     */
    FAIModelConfig config;

    // ===== Measurement Positions =====
    config.measurePositions = GetCommonMeasurePositions();

    // ===== KRIOS FAI Items =====
    config.faiItems = {
        {{KRIOS_FAI::LENS_INNER_DIAMETER},        // FAI ?곸닔 (踰덊샇)
         {_T("FAI-1")},                         // FAI 濡쒓렇 ?ㅻ뜑
         enMeasurePos::MEASURE_TOP_LENS_INNER}, // 痢≪젙 ?꾩튂
    };

    return config;
}
