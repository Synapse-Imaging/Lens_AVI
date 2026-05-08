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
#include "Classification_Runtime.h"
#include "Model/LAIInspector2Result.h"


namespace LGIT {
	class ModelInference2 {
	public:
		ModelInference2(
			const std::array<std::string, 23>& condition_name_list, 
			const std::array<std::string, 23>& condition_short_name_list,
			const std::string model_dir,
			int div_type_idx = 1);
		~ModelInference2();

		// 추론 실행 메서드
		LGIT::Model::LAIInspector2Result RunInference(
			const std::string& product_name,
			const std::string& lot_id,
			const std::string& barcode,
			const int vision_pos,
			const int image_no,
			const int roi_no,
			const int inspection_tab_idx,
			const int roi_inspection_type_idx,
			const std::string& defect_name,
			const cv::Mat& image,
			const cv::Mat& mask,
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

		inline static std::string ToLowerString(const std::string& str) {
			std::string lower = str;
			std::transform(lower.begin(), lower.end(), lower.begin(),
				[](unsigned char c) { return std::tolower(c); });
			return lower;
		}
	private:
		cv::Mat VisualizeResult(const cv::Mat& image, const cv::Mat& mask, float* result, int blobCount);

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