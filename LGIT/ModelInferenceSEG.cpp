#include "stdafx.h"
#ifdef LGITAI
#include "ModelInferenceSEG.h"
#include <algorithm>  // std::transform
#include <cctype>     // std::tolower
#include <string>
#include <iostream>
#include <random>   // std::mt19937, std::uniform_int_distribution
#include <cstddef>  // std::size_t (엄밀하게는 대부분의 경우 필요)
#include <chrono>   // std::chrono

namespace LGIT {
	ModelInferenceSEG::ModelInferenceSEG(
		const std::array<std::string, 23>& condition_name_list,
		const std::array<std::string, 23>& condition_short_name_list,
		const std::string model_dir,
		int div_type_idx) :
		_condition_name_list(condition_name_list),
		_condition_short_name_list(condition_short_name_list),
		_model_dir(model_dir)
	{
		if (div_type_idx >= _div_type_list.size()) { div_type_idx = 1; }
		if (!Segmentation_InitializeModels(model_dir.c_str(), _div_type_list[div_type_idx].data())) {
			throw std::runtime_error("엔진 생성 실패");
		}
	}

	ModelInferenceSEG::~ModelInferenceSEG()
	{
		// 모델 자원 해제 방법 추가시 
		// 20250812 기준 없음
	}

	//// 추론 실행 메서드 (기존 C 배열 방식)
	//int ModelInferenceSEG::RunInference(
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
	//	UINT64& inference_duration) {
	//	using namespace std::chrono;
	//	auto t_start = high_resolution_clock::now();
	//	std::string key = MakeInferenceEngineRunMemoryKey(product_name, _condition_name_list, vision_pos, image_no, roi_no, roi_inspection_type_idx, defect_name);
	//	int result = Segmentation_InferenceCVMat(images, masks, key.c_str());
	//	auto t_end = high_resolution_clock::now();
	//	inference_duration = duration_cast<microseconds>(t_end - t_start).count();
	//	return result;
	//}

	// 추론 실행 메서드 (std::array 방식 - 권장)
	int ModelInferenceSEG::RunInferenceArray(
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
		UINT64& inference_duration) {
		
		// std::array를 C 배열로 변환
		const cv::Mat* images_ptr = input_images.data();
		cv::Mat* masks_ptr = output_masks.data();

		int count = 1;

		//std::ostringstream filename;
		//filename
		//	<< product_name
		//	<< "_" << lot_id
		//	<< "_" << barcode
		//	<< "_" << vision_pos
		//	<< "_" << image_no
		//	<< "_" << roi_no
		//	<< "_" << inspection_tab_idx
		//	<< "_" << roi_inspection_type_idx
		//	<< "_" << defect_name;

		//for (auto image: input_images)
		//{
		//	std::ostringstream dir_stream;
		//	dir_stream << "D:\\TEMP\\TEMPSAVEIMAGE\\org_image_" << filename.str() << count++ << ".png";
		//	cv::imwrite(dir_stream.str(), image);
		//}
		
		using namespace std::chrono;
		auto t_start = high_resolution_clock::now();
		std::string key = MakeInferenceEngineRunMemoryKey(product_name, _condition_short_name_list, vision_pos, image_no, roi_no, roi_inspection_type_idx, defect_name);
		int result = Segmentation_InferenceCVMat(images_ptr, masks_ptr, key.c_str());
		auto t_end = high_resolution_clock::now();
		inference_duration = duration_cast<microseconds>(t_end - t_start).count();

		//for (auto image : output_masks)
		//{
		//	std::ostringstream dir_stream;
		//	dir_stream << "D:\\TEMP\\TEMPSAVEIMAGE\\mask_image_" << filename.str() << count++ << ".png";
		//	cv::imwrite(dir_stream.str(), image);
		//}

		return result;
	}

	// 추론 실행 메서드 (std::vector 방식 - 유연한 크기)
	int ModelInferenceSEG::RunInferenceVector(
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
		UINT64& inference_duration) {
		
		// 크기 검증
		if (input_images.size() != 8) {
			throw std::invalid_argument("Input images vector must contain exactly 8 images");
		}
		
		// output_masks 크기 조정
		if (output_masks.size() != 8) {
			output_masks.resize(8);
		}

		// 이미지들을 JPEG로 인코딩하여 새로운 cv::Mat 배열에 저장
		std::vector<cv::Mat> encoded_images(8);
		std::vector<uchar> buf;
		for (size_t i = 0; i < 8; ++i) {
			buf.clear();
			cv::imencode(".jpg", input_images[i], buf);
			encoded_images[i] = cv::imdecode(buf, cv::IMREAD_COLOR);
		}

		// std::vector를 C 배열로 변환
		const cv::Mat* images_ptr = encoded_images.data();
		cv::Mat* masks_ptr = output_masks.data();
		
		using namespace std::chrono;
		auto t_start = high_resolution_clock::now();
		std::string key = MakeInferenceEngineRunMemoryKey(product_name, _condition_short_name_list, vision_pos, image_no, roi_no, roi_inspection_type_idx, defect_name);
		int result = Segmentation_InferenceCVMat(images_ptr, masks_ptr, key.c_str());
		auto t_end = high_resolution_clock::now();
		inference_duration = duration_cast<microseconds>(t_end - t_start).count();
		return result;
	}

	bool ModelInferenceSEG::TryModelReload()
	{
		int request_model_update_count = 0;
		auto update_model_name_array = CheckingSegmentationUpdated(&request_model_update_count);
		if (request_model_update_count <= 0) {
			return true;
		}

		int update_model_count = modelLoad(_model_dir.c_str(), update_model_name_array, request_model_update_count);
		if (update_model_count != request_model_update_count)
		{
			return false;
		}

		return true;
	}

	// Match Key 생성해주기 (변경되면 변경)
	std::string ModelInferenceSEG::MakeInferenceEngineRunMemoryKey(
		const std::string& product_name,
		const std::array<std::string, 23>& condition_name_list,
		const int vision_pos,
		const int image_no,
		const int roi_no,
		const int roi_inspection_type_idx,
		const std::string& defect_name) {
		if (roi_inspection_type_idx >= condition_name_list.size()) { return std::string(); }
		return condition_name_list[roi_inspection_type_idx];
	}
}
#endif // LGITAI