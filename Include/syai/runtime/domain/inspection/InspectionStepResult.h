#pragma once
#include "syai/runtime/SimpleSmartType.h"
#include "syai/runtime/syai_runtime_inspection_export.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/types.hpp>
#include <syai/runtime/domain/config/ConfigurationEnum.h>

namespace syai::runtime::domain::inspection
{
	/**
	 * @brief 검사 단계 결과 클래스
	 *
	 * 각 검사 단계의 성공 여부, 메시지, 결과 마스크를 함께 관리하는 클래스.
	 * 동일한 키를 사용하는 세 가지 데이터를 하나로 묶어서 처리함.
	 *
	 * 특징:
	 * - 단계별 결과 통합 관리: 성공/실패, 메시지, 마스크를 하나의 객체로 관리
	 * - 자동 메모리 관리: 값 타입으로 사용되어 자동으로 메모리 관리됨
	 *
	 * 예제:
	 * @code
	 *     InspectionStepResult step_result;
	 *     step_result.success = true;
	 *     step_result.message = "검사 통과";
	 *     step_result.mask = result_mask_image;
	 *     step_result.ok_mask = ok_region_mask_image;
	 * @endcode
	 */
	class SYAIRUNTIMEINSPECTION_API InspectionStepResult
	{
	public:
		bool success;																//!< 해당 단계의 검사 성공 여부
		SmartString message;														//!< 해당 단계의 검사 결과 메시지
		cv::Mat mask;																//!< 해당 단계의 결과 마스크 (단일 채널, 0 또는 255 값) - NG/결함 영역
		cv::Mat ok_mask;															//!< 해당 단계의 OK 결과 마스크 (단일 채널, 0 또는 255 값) - 정상 영역
		int predicted_index;														//!< 예측된 클래스 인덱스
		syai::runtime::domain::config::InspectionType inspection_type;				//!< 검사 유형 (예: Classification, Segmentation 등)

		InspectionStepResult();
		InspectionStepResult(bool success, const char* message);
		InspectionStepResult(bool success, const char* message, const cv::Mat& mask);
		InspectionStepResult(bool success, const char* message, const cv::Mat& mask, const cv::Mat& ok_mask);
		InspectionStepResult(bool success, const char* message, int predicted_index, const cv::Mat& mask = cv::Mat());
		InspectionStepResult(bool success, const char* message, int predicted_index, const cv::Mat& mask, const cv::Mat& ok_mask);
		InspectionStepResult(const InspectionStepResult& other);
		InspectionStepResult& operator=(const InspectionStepResult& other);

		bool operator==(const InspectionStepResult& other) const;
		bool operator!=(const InspectionStepResult& other) const;
		bool is_valid() const;
	};
}
