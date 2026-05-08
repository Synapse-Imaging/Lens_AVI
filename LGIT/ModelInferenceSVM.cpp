#include "stdafx.h"
#ifdef LGITAI
#include "ModelInferenceSVM.h"
#include <algorithm>  // std::transform
#include <cctype>     // std::tolower
#include <string>
#include <iostream>
#include <random>   // std::mt19937, std::uniform_int_distribution
#include <cstddef>  // std::size_t (엄밀하게는 대부분의 경우 필요)

namespace LGIT {
	ModelInferenceSVM::ModelInferenceSVM(
		const std::array<std::string, 23>& condition_name_list,
		const std::array<std::string, 23>& condition_short_name_list,
		const std::string model_dir,
		int div_type_idx) :
		_condition_name_list(condition_name_list),
		_condition_short_name_list(condition_short_name_list),
		_model_dir(model_dir)
	{
		if (div_type_idx >= _div_type_list.size()) { div_type_idx = 1; }
		if (!SVMModelLoad(model_dir.c_str())) {
			throw std::runtime_error("엔진 생성 실패");
		}
	}

	ModelInferenceSVM::~ModelInferenceSVM()
	{
		// 모델 자원 해제 방법 추가시 
		// 20250701 기준 없음
	}

	LGIT::Model::LAIInspectorSVMResult ModelInferenceSVM::RunInference(
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
		int blob_count = FeatureExtractAndSVMPredict_Raw(
			degraded_image.data,
			degraded_image.cols,
			degraded_image.rows,
			mask.data,
			mask.cols,
			mask.rows,
			key.c_str(),
			result_buffer.data(),
			static_cast<int>(result_buffer.size()),
			lot_id.c_str(),
			barcode.c_str(),
			vision_pos_str.c_str(),
			image_no_str.c_str(),
			roi_no_str.c_str(),
			roi_inspection_type_idx_str.c_str(),
			condition_short_name.c_str()
		);
		//int blob_count = FeatureExtractAndSVMPredict_CVMat(
		//	image,
		//	mask,
		//	key.c_str(),
		//	result_buffer.data(),
		//	static_cast<int>(result_buffer.size())
		//);
		if (blob_count <= 0) {
			return LGIT::Model::LAIInspectorSVMResult{};
		}

		float* result = result_buffer.data();

		if (!result || blob_count <= 0) {
			return LGIT::Model::LAIInspectorSVMResult{};
		}
		auto t_end = high_resolution_clock::now();
		inference_duration = duration_cast<milliseconds>(t_end - t_start).count();

		std::vector<int> blob_id_list;
		std::vector<int> class_idx_list;
		std::vector<float> score_list;
		blob_id_list.reserve(blob_count);
		class_idx_list.reserve(blob_count); // 결과 index 0 양품, 1 불량
		score_list.reserve(blob_count);

		for (int i = 0; i < blob_count; i++) {
			int index = i * 3;
			int blob_id = static_cast<int>(result[index]);
			int max_class_idx = static_cast<int>(result[index + 1]);
			float max_score = result[index + 2];
			blob_id_list.push_back(blob_id);
			class_idx_list.push_back(max_class_idx);
			score_list.push_back(max_score);
		}
		return LGIT::Model::LAIInspectorSVMResult{
			true,
			blob_count,
			std::move(blob_id_list),
			std::move(class_idx_list),
			std::move(score_list)
		};
	}

	bool ModelInferenceSVM::TryModelReload() {
		int request_model_update_count = 0;
		auto update_model_name_array = CheckingSVMUpdated(&request_model_update_count);
		if (request_model_update_count <= 0) {
			return true;
		}

		int update_model_count = SVMModelUpdate(update_model_name_array, _model_dir.c_str(), request_model_update_count);
		if (update_model_count != request_model_update_count)
		{
			return false;
		}

		return true;
	}

	std::string ModelInferenceSVM::MakeInferenceEngineRunMemoryKey(
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

		std::string vision_pos_str = (vision_pos == 0) ? "BC" : "TC";
		// 예: CHS-K_TC_Image16_ROI1
		std::string key = product_name + "_" + vision_pos_str + "_Image" +
			std::to_string(image_no) + "_ROI" + std::to_string(roi_no) + "_Tab" + std::to_string(inspection_tab_idx);

		return key;
	}
}
#endif // LGITAI