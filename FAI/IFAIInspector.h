#pragma once
#include "stdafx.h"

/**
 * @file IFAIInspector.h
 * @brief FAI 검사 인터페이스 - Strategy 패턴
 * @description UAVI 프로젝트의 FAI 검사를 분리하기 위한 인터페이스
 */

class Algorithm;

class IFAIInspector
{
public:
	virtual ~IFAIInspector() = default;

	/**
	 * @brief 현재 Algorithm 인스턴스 갱신
	 * @param pAlgorithm 현재 검사 스레드의 Algorithm
	 */
	virtual void SetAlgorithm(Algorithm* pAlgorithm) { (void)pAlgorithm; }

	/**
	 * @brief FAI 검사 실행
	 * @param bIsAutoInsp 자동 검사 여부
	 * @param iInspectionBufferIdx 검사 버퍼 인덱스
	 * @param iFAIParamNo FAI 파라미터 번호
	 */
	virtual void Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo) = 0;

protected:
	IFAIInspector() = default;
};

enum class enMODEL_TYPE;

// FAI Inspector 클래스를 변경하는 팩토리 함수 선언
// SW 내부에서 모델 변경 시 사용되는 함수.
void ReplaceFAIInspector(IFAIInspector*& pCurrentInspector, CString strModelType, Algorithm* pAlgorithm);
