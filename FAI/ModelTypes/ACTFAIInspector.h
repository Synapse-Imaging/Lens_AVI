#pragma once

#include "IFAIInspector.h"
#include "FAIDefine.h"
#include "uScan.h"
#include <vector>

/**
 * @file ACTFAIInspector.h
 * @brief Actuator 프로젝트용 FAI 검사 구현 (리팩토링 버전)
 * @description Strategy 패턴 기반의 확장 가능한 FAI 검사 구현
 */

class Algorithm;
class FAIMeasureRegistry;

class ACTFAIInspector : public IFAIInspector
{
public:
	explicit ACTFAIInspector(Algorithm *pAlgorithm);
	virtual ~ACTFAIInspector() override = default;
	virtual void SetAlgorithm(Algorithm* pAlgorithm) override;

	virtual void Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo) override;

	/**
	 * @brief FAI 측정 설정 초기화
	 * @description 레지스트리에 모든 FAI 설정을 등록
	 */
	void InitializeFAIMeasures();

private:
	Algorithm *m_pAlgorithm;
	FAIMeasureRegistry &m_registry;

    ////////////////////////////////////////////////////////////////
	HObject m_HMeasureRgn[MEASURE_POS_TOTAL][MAX_MEASURE_POINT_NUM];
    // TODO : Blend Point 는 BOI 에서 필요하지 않을 것으로 보임. 분석 후 필요 없을 시 제거 필요. - 251222, jhkim
    DPOINT m_dBlendCPoint[VISION_NUMBER_MAX][POSITION_BLEND_END][BLEND_POINT_END];
	double m_dHeight[MEASURE_POS_TOTAL][MEASURE_POS_TOTAL];
	double m_dDiameter[MEASURE_POS_TOTAL];

	/**
	 * @brief 보정값(Multiple, Offset) 설정
	 * @description 검사 버퍼 인덱스에 따른 보정값 포인터 설정
	 */
	void SetCalibrationValues(int iInspectionBufferIdx, double *&dMultiple, double *&dOffset);
};
