#include "stdafx.h"
#ifdef LGITAI
#include "ModelInferenceVAL.h"
#include <algorithm>  // std::transform
#include <cctype>     // std::tolower
#include <string>
#include <iostream>
#include <random>   // std::mt19937, std::uniform_int_distribution
#include <cstddef>  // std::size_t (엄밀하게는 대부분의 경우 필요)
#include "Model/LAIInspectorResult.h"

namespace LGIT {
	ModelInferenceVAL::ModelInferenceVAL(
		const std::array<std::string, 23>& condition_name_list,
		const std::array<std::string, 23>& condition_short_name_list,
		const std::string model_dir,
		int div_type_idx) :
		_condition_name_list(condition_name_list),
		_condition_short_name_list(condition_short_name_list),
		_model_dir(model_dir)
	{
		if (div_type_idx >= _div_type_list.size()) { div_type_idx = 1; }
		if (!VariationModelLoad_OnlyVariation(model_dir.c_str())) {
			throw std::runtime_error("엔진 생성 실패");
		}
	}

	ModelInferenceVAL::~ModelInferenceVAL()
	{
		// 모델 자원 해제 방법 추가시 
		// 20250701 기준 없음
	}

	bool ModelInferenceVAL::RunInference(
		const std::string& product_name,
		const std::string& lot_id,
		const std::string& barcode,
		const int vision_pos,
		const int image_no,
		const int roi_no,
		const int inspection_tab_idx,
		const int roi_inspection_type_idx,
		const double local_align_angle_value,
		const std::string& defect_name,
		const cv::Mat& image,
		cv::Mat& mask_image,
		UINT64 & inference_duration)
	{
		using namespace std::chrono;
		auto t_start = high_resolution_clock::now();

		std::string key = MakeInferenceEngineRunMemoryKey(product_name, _condition_name_list, vision_pos, image_no, roi_no, inspection_tab_idx, roi_inspection_type_idx, defect_name);

		// key = "CHS_K_TC_Image16_ROI1";

		// int blob_count = 0;
		int blob_count_ = 0;

		std::string vision_pos_str = std::to_string(vision_pos);
		std::string image_no_str = std::to_string(image_no);
		std::string roi_no_str = std::to_string(roi_no);
		std::string roi_inspection_type_idx_str = std::to_string(roi_inspection_type_idx);

		std::string condition_short_name;
		if (roi_inspection_type_idx < _condition_short_name_list.size()) {
			condition_short_name = _condition_short_name_list[roi_inspection_type_idx];
		}
		else {
			condition_short_name = "NONE";
		}

		std::vector<float> result_buffer(300, 0.0f);  // 100 blobs 예상
		std::vector<uchar> jpg_buffer;
		std::vector<int> jpg_params = { cv::IMWRITE_JPEG_QUALITY, 100 };
		// 압축 + 디코딩 (메모리 내에서 열화)
		cv::imencode(".jpg", image, jpg_buffer, jpg_params);
		cv::Mat degraded_image = cv::imdecode(jpg_buffer, image.channels() == 1 ? cv::IMREAD_GRAYSCALE : cv::IMREAD_COLOR);
		// 열화된 이미지로 SVM 추론
		int blob_count = VariationRun_CVMat_OnlyVariation(
			-degraded_image,
			key.c_str(),
			local_align_angle_value,
			mask_image
		);
		if (blob_count <= -1) {
			return false;
		}
		auto t_end = high_resolution_clock::now();
		inference_duration = duration_cast<milliseconds>(t_end - t_start).count();
		return true;
	}

	bool ModelInferenceVAL::TryModelReload()
	{
		int request_model_update_count = 0;
		auto update_model_name_array = CheckingVariationUpdated(&request_model_update_count);
		if (request_model_update_count <= 0) {
			return true;
		}

		int update_model_count = variationParamUpdate(update_model_name_array, request_model_update_count);
		if (update_model_count != request_model_update_count)
		{
			return false;
		}

		return true;
	}

	std::string ModelInferenceVAL::MakeInferenceEngineRunMemoryKey(
		const std::string& product_name,
		const std::array<std::string, 23>& condition_name_list,
		const int vision_pos,
		const int image_no,
		const int roi_no,
		const int inspection_tab_idx,
		const int roi_inspection_type_idx,
		const std::string& defect_name)
	{
//#pragma region 임시
//		std::string sanitized_product_name = product_name;
//		// TODO 메시지 출력 (컴파일 시)
//		__pragma(message(__FILE__ "(" "142" "): TODO: ⚠️ [임시 코드] '-' to '_' 치환 코드입니다. 검토 후 정식 적용 필요."))
//		// 기능 적용: '-' → '_'
//		std::replace(sanitized_product_name.begin(), sanitized_product_name.end(), '-', '_');
		//std::string vision_pos_str = (vision_pos == 0) ? "BC" : "TC";
		//// 예: CHS-K_TC_Image16_ROI1
		//std::string key = sanitized_product_name + "_" + vision_pos_str + "_Image" +
		//	std::to_string(image_no) + "_ROI" + std::to_string(roi_no);
//#pragma endregion

		std::ostringstream image_no_stream;
		image_no_stream << std::setfill('0') << std::setw(2) << image_no;
		std::string image_no_str = image_no_stream.str();

		std::string vision_pos_str = (vision_pos == 0) ? "BC" : "TC";
		// 예: CHS-K_TC_Image16_ROI1
		std::string key2 = product_name + "_" + vision_pos_str + "_Img" + image_no_str + "_ROI" + std::to_string(roi_no) + "_Tab" + std::to_string(inspection_tab_idx + 1);
		std::string key = product_name + "_" + vision_pos_str + "_Img" + std::to_string(image_no) + "_ROI" + std::to_string(roi_no) + "_Tab" + std::to_string(inspection_tab_idx + 1);
		// std::string key = vision_pos_str + "_Img" + image_no_str + "_ROI" + std::to_string(roi_no) + "_Tab" + std::to_string(inspection_tab_idx);

		return key;
	}
}
#endif LGITAI