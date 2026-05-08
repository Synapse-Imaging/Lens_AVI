#pragma once

#include "StdAfx.h"
#include "IFAIInspector.h"
#include "PFunction.h"
#include "uScan.h"

/**
 * @file AKCFAIInspector.h
 * @brief AKC 프로젝트용 FAI 검사 구현
 * @description 블렌딩 + 축 변환 + 다중 FAI 항목 측정
 */

class Algorithm;

class AKCFAIInspector : public IFAIInspector
{
public:
	explicit AKCFAIInspector(Algorithm* pAlgorithm);
	virtual ~AKCFAIInspector() = default;
	virtual void SetAlgorithm(Algorithm* pAlgorithm) override;

	virtual void Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo) override;

private:
	Algorithm* m_pAlgorithm;
};
