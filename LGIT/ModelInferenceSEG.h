#pragma once
#ifdef LGITAI
#include <iostream>
#include <vector>
#include <array>
#include <Windows.h>
#include <string>
#include <utility>
#include <opencv2/opencv.hpp>
#include <halconcpp/HalconCpp.h>

#include "Segmentation_Runtime.h"

namespace LGIT {
	class ModelInferenceSEG {
	public:
		ModelInferenceSEG(
			const std::array<std::string, 23>& condition_name_list,
			const std::array<std::string, 23>& condition_short_name_list,
			const std::string model_dir,
			int div_type_idx = 1);
		~ModelInferenceSEG();

		//// 추론 실행 메서드 (기존 C 배열 방식)
		//int RunInference(
		//	const std::string& product_name,
		//	const std::string& lot_id,
		//	const std::string& barcode,
		//	const int vision_pos,
		//	const int image_no,
		//	const int roi_no,
		//	const int inspection_tab_idx,
		//	const int roi_inspection_type_idx,
		//	const std::string& defect_name,
		//	const cv::Mat images[8],
		//	cv::Mat masks[8],
		//	UINT64& inference_duration);

		// 추론 실행 메서드 (std::array 방식 - 권장)
		int RunInferenceArray(
			const std::string& product_name,
			const std::string& lot_id,
			const std::string& barcode,
			const int vision_pos,
			const int image_no,
			const int roi_no,
			const int inspection_tab_idx,
			const int roi_inspection_type_idx,
			const std::string& defect_name,
			const std::array<cv::Mat, 8>& input_images,
			std::array<cv::Mat, 8>& output_masks,
			UINT64& inference_duration);

		// 추론 실행 메서드 (std::vector 방식 - 유연한 크기)
		int RunInferenceVector(
			const std::string& product_name,
			const std::string& lot_id,
			const std::string& barcode,
			const int vision_pos,
			const int image_no,
			const int roi_no,
			const int inspection_tab_idx,
			const int roi_inspection_type_idx,
			const std::string& defect_name,
			const std::vector<cv::Mat>& input_images,
			std::vector<cv::Mat>& output_masks,
			UINT64& inference_duration);

		bool TryModelReload();

		// Match Key 생성해주기 (변경되면 변경)
		static std::string MakeInferenceEngineRunMemoryKey(
			const std::string& product_name,
			const std::array<std::string, 23>& condition_name_list,
			const int vision_pos,
			const int image_no,
			const int roi_no,
			const int roi_inspection_type_idx,
			const std::string& defect_name);

	private:
		const std::array<std::string, 23> _condition_name_list; // key 
		const std::array<std::string, 23> _condition_short_name_list; // key 
		const std::string _model_dir;

	private:
		inline static const std::array<std::string, 7> _div_type_list = {
			"GPU.0",
			"GPU.1",
			"GPU.2",
			"GPU.3",
			"CPU",
			"GPU",
			"NPU"
		};
	};
}
#endif // LGITAI
