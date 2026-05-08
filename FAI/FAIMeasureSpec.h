#pragma once
#include "FAIDefine.h"
/**
 * 아직 개발 중... - 260213, jhkim
 */


/**
 * @file FAIMeasureSpec.h
 * @brief FAI 측정 설정 데이터 구조
 * @description 각 FAI 측정의 메타데이터를 정의
 */

enum class FAIMeasureType
{
	DISTANCE_TO_DATUM = 0,      // 기준선까지의 거리
	CIRCLE_FITTING = 1,         // 원 피팅 기반
	COMPOSITE = 2               // 조합 계산 (여러 값의 조합)
};

struct FAIMeasureSpec
{
	int id;                              // FAI 인덱스 (FAI 번호와는 다름. SW 내부 FAI 배열의 인덱스)
	FAIMeasureType type;             // 측정 타입
	const char* name;                    // FAI 이름 (예: "FAI-5")
	enMeasurePos measurePointType;       // enMeasurePos::MEASURE_TOP_LENS_INNER, enMeasurePos::MEASURE_TOP_DATUM_WEST 등
	double specValue;                    // 규격값 (거리 기반 측정용)
	int dependentFAIIds[2];              // 의존하는 FAI ID (조합 계산용. -1 = 없음)
	bool bRequiresCircleFitting; 		 // 원형 피팅 필요 여부
	
	// 보정 계수 및 오프셋 포인터
	const double *dMultiple;
	const double *dOffset;
	
	FAIMeasureSpec()
				: id(-1), type(FAIMeasureType::DISTANCE_TO_DATUM), name("UNKNOWN"),
					measurePointType(enMeasurePos::MEASURE_POSITION_END), specValue(0.0), bRequiresCircleFitting(false),
		  dMultiple(nullptr), dOffset(nullptr)
	{
		dependentFAIIds[0] = -1;
		dependentFAIIds[1] = -1;
	}
};
