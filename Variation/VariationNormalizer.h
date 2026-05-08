// VariationNormalizer.h
#pragma once

#define MAX_KERNEL_SIZE 15

struct VariationNormalizerParam
{
	float sigma_b;		// 템플릿 대비 밝기 보정 가중치(가우시안 함수 -> 밝기차가 클수록 보정 적게)
	float sigma_r;		// 밝기 보정 범위 가중치(가우시안 함수 -> 밝기차가 클수록 보정 적게)
	float sigma_s;		// 밝기 보정 거리 가중치(가우시안 함수 -> 거리차가 클수록 보정 적게)
	int kernel_size;	// 필터 사이즈
	float epsilon;		// 로그 계산용

	// Adaptive Offset 파라미터
	float k;
	float weightMin;
	float weightMax;

	VariationNormalizerParam()
		: sigma_b(20.0), sigma_r(20.0), sigma_s(3.0)
		, kernel_size(5), epsilon(1e-6), k(2.0)
		, weightMin(0.1), weightMax(2) {}
};

class CVariationNormalizer
{
public:
	CVariationNormalizer();
	~CVariationNormalizer();

	// 조명 보정
	BOOL ApplyNormalization(
		const HObject& HTestImage,
		const HObject& HMeanTemplate,
		const HObject& HROIRegion,
		const VariationNormalizerParam& param,
		HObject* pHRectified
	);

	BOOL ApplyAdaptiveNormalization(
		const HObject& HTestImage,
		const HObject& HMeanTemplate,
		const HObject& HROIRegion,
		const VariationNormalizerParam& param,
		HObject* pHRectified
	);

private:

	float m_dLUT_WeightS[MAX_KERNEL_SIZE][MAX_KERNEL_SIZE];
	float m_dLUT_WeightB[256];    // 결함 보호 가중치 (sigma_b)
	float m_dLUT_WeightR[256];    // 범위 가중치 (sigma_r)
	float m_dLUT_Log[256];        // log(i + epsilon)
	float m_dLUT_Scale[511];

	BOOL bLUTValid;

	VariationNormalizerParam m_currentParam;

	void BuildLUT(const VariationNormalizerParam& param);
	void BuildScaleLUT(float offset, float sigma_ref, const VariationNormalizerParam& param);

};