#pragma once

#include "AlgorithmHelper.h"

class Algorithm;

class AlgorithmPipeline
{
public:
	explicit AlgorithmPipeline(Algorithm* pAlgorithm);
	~AlgorithmPipeline() = default;

	AlgorithmPipeline(const AlgorithmPipeline&) = delete;
	AlgorithmPipeline& operator=(const AlgorithmPipeline&) = delete;

	// Phase 0: 버퍼 준비
	int PrepareAlgorithmBuffer(const AlgorithmCopyParam& param);

	// Phase 1: 이미지 전처리
	void ProcessRawImageSave(const AlgorithmContext& algCtx);
	BOOL ProcessGlobalAlign(AlgorithmContext& algCtx, BOOL bInitHomMat);
	void ProcessHsiTrans(const AlgorithmContext& algCtx);

	// Phase 2: ROI 전처리
	void ProcessLocalAlign(const AlgorithmContext& algCtx);
	void ProcessFitting(const AlgorithmContext& algCtx);
	void ProcessFittingAdd(const AlgorithmContext& algCtx);
	void ProcessDontCare(const AlgorithmContext& algCtx);
	void ProcessGenerate(const AlgorithmContext& algCtx);
	void ProcessMask(const AlgorithmContext& algCtx);

	// Phase 3: 메인 검사
	void ProcessFAIMeasure(const AlgorithmContext& algCtx, BOOL bInitFAIItem);
	void ProcessBarcodeOcr(const AlgorithmContext& algCtx);
	void ProcessInspection(const AlgorithmContext& algCtx, BOOL bInitDefectRgn);

	// Phase 4: 결과 처리
	void PostProcess(const AlgorithmContext& algCtx);
	void ProcessDefectMerge(const AlgorithmContext& algCtx);
	void ProcessMonitoringLog(const AlgorithmContext& algCtx);
	void ProcessResultSave(const AlgorithmContext& algCtx);

private:
	Algorithm* m_pAlgorithm;
	AlgorithmHelper m_AlgorithmHelper;
};