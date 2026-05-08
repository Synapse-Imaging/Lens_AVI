#pragma once

#include "IFAIInspector.h"
#include "FAIDefine.h"
#include "uScan.h"

/**
 * @file BOIFAIInspector.h
 * @brief BOI 제품 유형을 위한 FAI 인스펙터 구현
 */

class Algorithm;

class BOIFAIInspector : public IFAIInspector
{
public:
	explicit BOIFAIInspector(Algorithm* pAlgorithm);
	virtual ~BOIFAIInspector() override = default;
	virtual void SetAlgorithm(Algorithm* pAlgorithm) override;

	virtual void Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo) override;

private:
	Algorithm* m_pAlgorithm;

	// Stage/Module 번호에 따라 dMultiple과 dOffset 포인터를 설정합니다
	void SetCalibrationValues(int iInspectionBufferIdx, double*& dMultiple, double*& dOffset);

	// 주어진 측정 위치에 대해 측정된 edge points에 원을 맞춥니다.
	// center와 radius 출력 매개변수를 채웁니다. 실패하면 FALSE를 반환합니다.
	BOOL FitCircle(
		CenterlineMeasureStruct* pStructFAIMeasure,
		enMeasurePos measurePointType,
		int iFAINumber,
		double& dCircleCenterX,
		double& dCircleCenterY,
		double& dCircleRadius,
		int iInspectionBufferIdx,
		int iFAIParamNo
	);
};
