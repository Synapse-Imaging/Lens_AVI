#pragma once
#include "IFAIInspector.h"
#include "FAIDefine.h"
#include "uScan.h"
#include <vector>

class Algorithm;
class FAIMeasureRegistry;

class ATWFAIInspector :  public IFAIInspector
{
public:

    ATWFAIInspector(Algorithm* pAlgorithm) : m_pAlgorithm(pAlgorithm) {};
    virtual ~ATWFAIInspector() = default;

    virtual void Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo) override;
    virtual void SetAlgorithm(Algorithm* pAlgorithm) override;

private:
    Algorithm* m_pAlgorithm;

	void SetCalibrationValues(int iInspectionBufferIdx, double*& dMultiple, double*& dOffset);
};

