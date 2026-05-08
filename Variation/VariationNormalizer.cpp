// VariationNormalizer.cpp
#include "stdafx.h"
#include "uScan.h"
#include "VariationNormalizer.h"

#include <omp.h>

CVariationNormalizer::CVariationNormalizer()
{
	bLUTValid = FALSE;

	memset(m_dLUT_WeightS, 0, sizeof(m_dLUT_WeightS));
}

CVariationNormalizer::~CVariationNormalizer()
{
}

void CVariationNormalizer::BuildLUT(const VariationNormalizerParam& param)
{
	float inv_sigma_b_sq = 1.0 / (2.0 * param.sigma_b * param.sigma_b);
	float inv_sigma_r_sq = 1.0 / (2.0 * param.sigma_r * param.sigma_r);

	for (int i = 0; i < 256; i++)
	{
		// sigma_b LUT 갱신
		if (param.sigma_b != m_currentParam.sigma_b || bLUTValid == FALSE)
			m_dLUT_WeightB[i] = exp(-(float)(i * i) * inv_sigma_b_sq);

		// sigma_r LUT 갱신
		if (param.sigma_r != m_currentParam.sigma_r || bLUTValid == FALSE)
			m_dLUT_WeightR[i] = exp(-(float)(i * i) * inv_sigma_r_sq);

		// Log LUT: log(value + epsilon)
		if (bLUTValid == FALSE)
			m_dLUT_Log[i] = log((float)i + param.epsilon);
	}

	// sigma_s LUT 갱신
	if (param.kernel_size != m_currentParam.kernel_size ||
		param.sigma_s != m_currentParam.sigma_s || bLUTValid == FALSE)
	{
		int half = param.kernel_size / 2;
		float sigma2 = 2.0 * param.sigma_s * param.sigma_s;

		for (int dy = -half; dy <= half; dy++)
		{
			for (int dx = -half; dx <= half; dx++)
			{
				float dist2 = (float)(dx * dx + dy * dy);
				int iy = dy + half;
				int ix = dx + half;
				m_dLUT_WeightS[iy][ix] = exp(-dist2 / sigma2);
			}
		}
	}

	m_currentParam = param;

	bLUTValid = TRUE;
}

void CVariationNormalizer::BuildScaleLUT(
	float offset,
	float sigma_ref,
	const VariationNormalizerParam& param)
{
	double sign_off = (offset >= 0) ? 1.0 : -1.0;
	double k_over_sigma = param.k * sign_off / sigma_ref;

	for (int i = 0; i < 511; i++)
	{
		int d = i - 255;

		double s = (double)d * k_over_sigma;
		double w = 1.0 + tanh(s);

		if (w < param.weightMin) w = param.weightMin;
		if (w > param.weightMax) w = param.weightMax;

		m_dLUT_Scale[i] = w;
	}
}

BOOL CVariationNormalizer::ApplyNormalization(
	const HObject& HTestImage,
	const HObject& HMeanTemplate,
	const HObject& HROIRegion,
	const VariationNormalizerParam& param,
	HObject* pHRectified)
{
	try
	{
		if (!CGFunction::ValidHImage(HTestImage) ||
			!CGFunction::ValidHImage(HMeanTemplate) ||
			!CGFunction::ValidHRegion(HROIRegion) ||
			!pHRectified)
			return FALSE;

		BuildLUT(param);

		HObject HTestResized;
		HTuple HWidthTest, HHeightTest;
		HTuple HWidthMean, HHeightMean;
		HTuple HPtrTest, HPtrMean, HPtrMask;
		GetImageSize(HTestImage, &HWidthTest, &HHeightTest);

		int iWidthTest = (int)HWidthTest[0].L();
		int iHeightTest = (int)HHeightTest[0].L();
		
		GetImagePointer1(HMeanTemplate, &HPtrMean, NULL, &HWidthMean, &HHeightMean);

		int width = (int)HWidthMean[0].L();
		int height = (int)HHeightMean[0].L();
		
		// 검사 이미지 크기 조정
		if (iWidthTest != width || iHeightTest != height)
			ZoomImageSize(HTestImage, &HTestResized, width, height, "bilinear");
		else
			HTestResized = HTestImage;

		GetImagePointer1(HTestResized, &HPtrTest, NULL, NULL, NULL);

		const BYTE* __restrict pTest = (BYTE*)HPtrTest[0].L();
		const BYTE* __restrict pMean = (BYTE*)HPtrMean[0].L();

		// 마스크 처리
		BYTE* pMask = NULL;
		HTuple HRegionCount;
		CountObj(HROIRegion, &HRegionCount);

		HObject HMaskImage;
		if (HRegionCount[0].L() > 0)
		{
			RegionToBin(HROIRegion, &HMaskImage, 255, 0, width, height);
			GetImagePointer1(HMaskImage, &HPtrMask, NULL, NULL, NULL);
			pMask = (BYTE*)HPtrMask[0].L();
		}

		HTuple HRow1, HCol1, HRow2, HCol2;
		SmallestRectangle1(HROIRegion, &HRow1, &HCol1, &HRow2, &HCol2);

		// 결과 이미지 생성
		HObject HResult;
		GenImageConst(&HResult, "byte", width, height);
		HTuple HPtrResult;
		GetImagePointer1(HResult, &HPtrResult, NULL, NULL, NULL);
		BYTE* __restrict pResult = (BYTE*)HPtrResult[0].L();

		int half = param.kernel_size / 2;

		// ========== 1. 내부 영역 ==========
#pragma omp parallel 
		{
#pragma omp for nowait schedule(dynamic)
			for (int y = half; y < height - half; y++)
			{
				int rowOffset = y * width;
				const BYTE* __restrict pTestRow = pTest + rowOffset;
				const BYTE* __restrict pMeanRow = pMean + rowOffset;
				BYTE* __restrict pResultRow = pResult + rowOffset;
				const BYTE* __restrict pMaskRow = pMask ? pMask + rowOffset : NULL;

				for (int x = half; x < width - half; x++)
				{
					if (pMaskRow && pMaskRow[x] == 0)
					{
						pResultRow[x] = pTestRow[x];
						continue;
					}

					BYTE testCenter = pTestRow[x];
					BYTE meanCenter = pMeanRow[x];

					float numer = 0.0;
					float denom = 0.0;

					for (int ky = -half; ky <= half; ky++)
					{
						int kyOffset = (y + ky) * width;
						const BYTE* __restrict pTestKRow = pTest + kyOffset;
						const BYTE* __restrict pMeanKRow = pMean + kyOffset;

						for (int kx = -half; kx <= half; kx++)
						{
							int nx = x + kx;
							BYTE testValue = pTestKRow[nx];
							BYTE meanValue = pMeanKRow[nx];

							float w_s = m_dLUT_WeightS[ky + half][kx + half];
							int rangeDiff = abs((int)meanValue - (int)meanCenter);
							float w_r = m_dLUT_WeightR[rangeDiff];
							float weight = w_s * w_r;

							int defectDiff = abs((int)testValue - (int)meanValue);
							float b_k = m_dLUT_WeightB[defectDiff];
							float C_k = 1.0 - b_k * (m_dLUT_Log[testValue] - m_dLUT_Log[meanValue]);

							numer += weight * testValue * C_k;
							denom += weight;
						}
					}

					float rectified = (denom > 0) ? (numer / denom) : testCenter;
					pResultRow[x] = (BYTE)max(0, min(255, (int)(rectified + 0.5)));
				}
			}

			// ========== 2. 경계 영역 (원본 복사) ==========
			// 상단 경계
#pragma omp for nowait
			for (int y = 0; y < half; y++)
				for (int x = 0; x < width; x++)
					pResult[y * width + x] = pTest[y * width + x];

			// 하단 경계
#pragma omp for nowait
			for (int y = height - half; y < height; y++)
				for (int x = 0; x < width; x++)
					pResult[y * width + x] = pTest[y * width + x];

			// 좌측 경계
#pragma omp for nowait
			for (int y = half; y < height - half; y++)
				for (int x = 0; x < half; x++)
					pResult[y * width + x] = pTest[y * width + x];

			// 우측 경계
#pragma omp for
			for (int y = half; y < height - half; y++)
				for (int x = width - half; x < width; x++)
					pResult[y * width + x] = pTest[y * width + x];
		}

		*pHRectified = HResult;

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp, HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		CString strLog;
		strLog.Format("[VariationNormalizer::ApplyNormalization] Halcon Exception: <%s> %s",
			(const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->error("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

BOOL CVariationNormalizer::ApplyAdaptiveNormalization(
	const HObject& HTestImage,
	const HObject& HMeanTemplate,
	const HObject& HROIRegion,
	const VariationNormalizerParam& param,
	HObject* pHRectified)
{
	try
	{
		if (!CGFunction::ValidHImage(HTestImage) ||
			!CGFunction::ValidHImage(HMeanTemplate) ||
			!CGFunction::ValidHRegion(HROIRegion) ||
			!pHRectified)
			return FALSE;

		BuildLUT(param);

		HObject HTestResized;
		HTuple HWidthTest, HHeightTest;
		HTuple HWidthMean, HHeightMean;
		HTuple HPtrTest, HPtrMean, HPtrMask;
		GetImageSize(HTestImage, &HWidthTest, &HHeightTest);

		int iWidthTest = (int)HWidthTest[0].L();
		int iHeightTest = (int)HHeightTest[0].L();

		GetImagePointer1(HMeanTemplate, &HPtrMean, NULL, &HWidthMean, &HHeightMean);

		int width = (int)HWidthMean[0].L();
		int height = (int)HHeightMean[0].L();

		// 검사 이미지 크기 조정
		if (iWidthTest != width || iHeightTest != height)
			ZoomImageSize(HTestImage, &HTestResized, width, height, "bilinear");
		else
			HTestResized = HTestImage;

		GetImagePointer1(HTestResized, &HPtrTest, NULL, NULL, NULL);

		const BYTE* __restrict pTest = (BYTE*)HPtrTest[0].L();
		const BYTE* __restrict pMean = (BYTE*)HPtrMean[0].L();

		// 마스크 처리
		BYTE* pMask = NULL;
		HTuple HRegionCount;
		CountObj(HROIRegion, &HRegionCount);

		HObject HMaskImage;
		if (HRegionCount[0].L() > 0)
		{
			RegionToBin(HROIRegion, &HMaskImage, 255, 0, width, height);
			GetImagePointer1(HMaskImage, &HPtrMask, NULL, NULL, NULL);
			pMask = (BYTE*)HPtrMask[0].L();
		}

		HTuple HRow1, HCol1, HRow2, HCol2;
		SmallestRectangle1(HROIRegion, &HRow1, &HCol1, &HRow2, &HCol2);

		// 결과 이미지 생성
		HObject HResult;
		GenImageConst(&HResult, "byte", width, height);
		HTuple HPtrResult;
		GetImagePointer1(HResult, &HPtrResult, NULL, NULL, NULL);
		BYTE* __restrict pResult = (BYTE*)HPtrResult[0].L();

		int half = param.kernel_size / 2;

		HTuple HMeanTest, HDevTest, HMeanTemp, HDevTemp;
		Intensity(HROIRegion, HTestResized, &HMeanTest, &HDevTest);
		Intensity(HROIRegion, HMeanTemplate, &HMeanTemp, &HDevTemp);

		float mu_test = HMeanTest[0].D();
		float mu_temp = HMeanTemp[0].D();
		float offset = mu_temp - mu_test;
		float sigma_ref = max(HDevTest[0].D(), 1.0);

		BuildScaleLUT(offset, sigma_ref, param);

		HObject HSmoothTest;
		HTuple HPtrSmooth;
		MedianRect(HTestResized, &HSmoothTest, 3, 3);
		GetImagePointer1(HSmoothTest, &HPtrSmooth, NULL, NULL, NULL);
		const BYTE* __restrict pSmooth = (BYTE*)HPtrSmooth[0].L();

		int mu_test_int = (int)(mu_test + 0.5);

//		//////////// 디버깅 ////////////
//		{
//			HObject HScaled;
//			GenImageConst(&HScaled, "byte", width, height);
//			HTuple HPtrScaled;
//			GetImagePointer1(HScaled, &HPtrScaled, NULL, NULL, NULL);
//			BYTE* __restrict pScaled = (BYTE*)HPtrScaled[0].L();
//
//#pragma omp parallel for
//			for (int y = 0; y < height; y++)
//			{
//				int rowOffset = y * width;
//				const BYTE* __restrict pT = pTest + rowOffset;
//				const BYTE* __restrict pS = pSmooth + rowOffset;
//				BYTE*       __restrict pO = pScaled + rowOffset;
//
//				for (int x = 0; x < width; x++)
//				{
//					int d = max(-255, min(255, (int)pS[x] - mu_test_int));
//					double v = max(0.0, min(255.0, pT[x] + offset * m_dLUT_Scale[d + 255]));
//					pO[x] = (BYTE)(v + 0.5);
//				}
//			}
//
//			
//			WriteImage(HScaled, "bmp", 0, "C:\\DualTest\\00_HScaled");
//			
//		}
//		///////////////////////////////

		// ========== 1. 내부 영역 ==========
#pragma omp parallel 
		{
#pragma omp for nowait schedule(dynamic)
			for (int y = half; y < height - half; y++)
			{
				int rowOffset = y * width;
				const BYTE* __restrict pTestRow = pTest + rowOffset;
				const BYTE* __restrict pMeanRow = pMean + rowOffset;
				const BYTE* __restrict pSmoothRow = pSmooth + rowOffset;
				BYTE* __restrict pResultRow = pResult + rowOffset;
				const BYTE* __restrict pMaskRow = pMask ? pMask + rowOffset : NULL;

				for (int x = half; x < width - half; x++)
				{
					if (pMaskRow && pMaskRow[x] == 0)
					{
						pResultRow[x] = pTestRow[x];
						continue;
					}

					BYTE testCenter = pTestRow[x];
					BYTE meanCenter = pMeanRow[x];

					int dC = max(-255, min(255, (int)pSmoothRow[x] - mu_test_int));
					float scaledCenter = max(0.0, min(255.0, testCenter + offset * m_dLUT_Scale[dC + 255]));

					float numer = 0.0;
					float denom = 0.0;

					for (int ky = -half; ky <= half; ky++)
					{
						int kyOffset = (y + ky) * width;
						const BYTE* __restrict pTestKRow = pTest + kyOffset;
						const BYTE* __restrict pMeanKRow = pMean + kyOffset;
						const BYTE* __restrict pSmoothKRow = pSmooth + kyOffset;

						for (int kx = -half; kx <= half; kx++)
						{
							int nx = x + kx;
							BYTE testValue = pTestKRow[nx];
							BYTE meanValue = pMeanKRow[nx];

							int dK = max(-255, min(255, (int)pSmoothKRow[nx] - mu_test_int));
							float scaledK = max(0.0, min(255.0, testValue + offset * m_dLUT_Scale[dK + 255]));
							int scaledK_int = (int)(scaledK + 0.5);

							float w_s = m_dLUT_WeightS[ky + half][kx + half];
							int rangeDiff = abs((int)meanValue - (int)meanCenter);
							float w_r = m_dLUT_WeightR[rangeDiff];
							float weight = w_s * w_r;

							int defectDiff = abs(scaledK_int - (int)meanValue);
							float b_k = m_dLUT_WeightB[defectDiff];
							float C_k = 1.0 - b_k * (m_dLUT_Log[scaledK_int] - m_dLUT_Log[meanValue]);

							numer += weight * scaledK * C_k;
							denom += weight;
						}
					}

					float rectified = (denom > 0) ? (numer / denom) : scaledCenter;
					pResultRow[x] = (BYTE)max(0, min(255, (int)(rectified + 0.5)));
				}
			}

			// ========== 2. 경계 영역 (원본 복사) ==========
			// 상단 경계
#pragma omp for nowait
			for (int y = 0; y < half; y++)
				for (int x = 0; x < width; x++)
					pResult[y * width + x] = pTest[y * width + x];

			// 하단 경계
#pragma omp for nowait
			for (int y = height - half; y < height; y++)
				for (int x = 0; x < width; x++)
					pResult[y * width + x] = pTest[y * width + x];

			// 좌측 경계
#pragma omp for nowait
			for (int y = half; y < height - half; y++)
				for (int x = 0; x < half; x++)
					pResult[y * width + x] = pTest[y * width + x];

			// 우측 경계
#pragma omp for
			for (int y = half; y < height - half; y++)
				for (int x = width - half; x < width; x++)
					pResult[y * width + x] = pTest[y * width + x];
		}

		*pHRectified = HResult;

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp, HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		CString strLog;
		strLog.Format("[VariationNormalizer::ApplyNormalization] Halcon Exception: <%s> %s",
			(const char*)HOperatorName.S(), (const char*)HErrMsg.S());
		THEAPP.m_log_halcon->error("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}