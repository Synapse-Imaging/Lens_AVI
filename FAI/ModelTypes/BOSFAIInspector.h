#pragma once
#include "IFAIInspector.h"
#include "FAIDefine.h"
#include "uScan.h"

class Algorithm;

class BOSFAIInspector : public IFAIInspector
{
public:
    BOSFAIInspector(Algorithm* pAlgorithm) : m_pAlgorithm(pAlgorithm) {};
    virtual ~BOSFAIInspector() = default;

    virtual void Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo) override;
    virtual void SetAlgorithm(Algorithm* pAlgorithm) override;

private:
    Algorithm* m_pAlgorithm;

	void SetCalibrationValues(int iInspectionBufferIdx, double*& dMultiple, double*& dOffset);
    
	// 주어진 측정 위치에 대해 측정된 edge points에 원을 맞춥니다.
	// center와 radius 출력 매개변수를 채웁니다. 실패하면 FALSE를 반환합니다.
	BOOL FitCircle(
		CenterlineMeasureStruct* pStructFAIMeasure,
		enMeasurePos measurePointType,
		double& dCircleCenterX,
		double& dCircleCenterY,
		double& dCircleRadius
	);
};
