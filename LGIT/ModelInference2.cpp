#include "stdafx.h"
#ifdef LGITAI
#include "ModelInference2.h"
#include <algorithm>  // std::transform
#include <cctype>     // std::tolower
#include <string>
#include <iostream>
#include <random>   // std::mt19937, std::uniform_int_distribution
#include <cstddef>  // std::size_t (엄밀하게는 대부분의 경우 필요)

namespace LGIT {
	ModelInference2::ModelInference2(
		const std::array<std::string, 23>& condition_name_list,
		const std::array<std::string, 23>& condition_short_name_list,
		const std::string model_dir, 
		int div_type_idx) :
		_condition_name_list(condition_name_list), 
		_condition_short_name_list(condition_short_name_list),
		_model_dir(model_dir)
	{
		if (div_type_idx >= _div_type_list.size()) { div_type_idx = 1; }
		if (!InitializeModels(model_dir.c_str(), _div_type_list[div_type_idx].data())) {
			throw std::runtime_error("엔진 생성 실패");
		}
	}

	ModelInference2::~ModelInference2()
	{
		try {
			DisposeModels();
		}
		catch (...) {
			// 소멸자에서는 예외를 던지지 않도록 주의
		}
	}

	LGIT::Model::LAIInspector2Result ModelInference2::RunInference(
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

		std::string key = MakeInferenceEngineRunMemoryKey(product_name, _condition_name_list, vision_pos, image_no, roi_no, roi_inspection_type_idx, defect_name);

		int blob_count = 0;

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

		try {
			float* result = InferenceCVMat(
				lot_id.c_str(),
				barcode.c_str(),
				vision_pos_str.c_str(),
				image_no_str.c_str(),
				roi_no_str.c_str(),
				roi_inspection_type_idx_str.c_str(),
				condition_short_name.c_str(),
				image,
				mask,
				key.c_str(),
				&blob_count
			);

			if (!result || blob_count <= 0) {
				return LGIT::Model::LAIInspector2Result{};
			}
			auto t_end = high_resolution_clock::now();
			inference_duration = duration_cast<milliseconds>(t_end - t_start).count();

			std::vector<int> blob_id_list;
			std::vector<int> class_idx_list;
			std::vector<float> score_list;
			blob_id_list.reserve(blob_count);
			class_idx_list.reserve(blob_count);
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
			FreeInferenceResultFloat(result);

			// cv::Mat visual_image = VisualizeResult(image, mask, result, blob_count);
			// return Inference2Result{ true, blob_count, blob_id_list, class_idx_list, score_list, visual_image};
			return LGIT::Model::LAIInspector2Result{
				true,
				blob_count,
				std::move(blob_id_list),
				std::move(class_idx_list),
				std::move(score_list)
			};
		}
		catch (const std::exception& ex) {
			std::string msg = fmt::format("RunInference exception: {}", ex.what());
			throw std::runtime_error(msg);
		}
		catch (...) {
			std::string msg = "RunInference unknown exception";
			throw std::runtime_error(msg);
		}
		return LGIT::Model::LAIInspector2Result{};
	}

	std::string ModelInference2::MakeInferenceEngineRunMemoryKey(
		const std::string& product_name,
		const std::array<std::string, 23>& condition_name_list, 
		const int vision_pos,
		const int image_no,
		const int roi_no,
		const int roi_inspection_type_idx,
		const std::string& defect_name)
	{
		std::string key = product_name + "_" + condition_name_list[roi_inspection_type_idx] + "_" + defect_name;
		return key;
	}

	bool ModelInference2::TryModelReload() 
	{
		int request_model_update_count = 0;
		auto update_model_name_array = CheckingClassificationUpdated(&request_model_update_count);
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


	cv::Mat ModelInference2::VisualizeResult(const cv::Mat& image, const cv::Mat& mask, float* result, int blobCount)
	{
		// 컬러맵 정의 (8개 클래스에 대한 색상)
		std::vector<cv::Scalar> colors = {
			cv::Scalar(255, 0, 0),     // 클래스 0: 파랑
			cv::Scalar(0, 255, 0),     // 클래스 1: 초록
			cv::Scalar(0, 0, 255),     // 클래스 2: 빨강
			cv::Scalar(255, 255, 0),   // 클래스 3: 청록
			cv::Scalar(255, 0, 255),   // 클래스 4: 마젠타
			cv::Scalar(0, 255, 255),   // 클래스 5: 노랑
			cv::Scalar(128, 128, 128), // 클래스 6: 회색
			cv::Scalar(255, 255, 255)  // 클래스 7: 흰색
		};

		try {
			// 결과 시각화를 위한 이미지 복사
			cv::Mat visualImage = cv::Mat::zeros(mask.size(), CV_8UC3);

			// 결과에 따라 각 blob에 클래스 색상 적용
			for (int i = 0; i < blobCount; i++) {
				int index = i * 3; // 수정됨: 각 blob당 3개 값 (blobId, maxClassIdx, maxScore)
				int blobId = static_cast<int>(result[index++]);
				int maxClassIdx = static_cast<int>(result[index++]);

				// 마스크에서 현재 blob ID를 찾아 색상 적용
				for (int r = 0; r < mask.rows; r++) {
					for (int c = 0; c < mask.cols; c++) {
						if (mask.at<uchar>(r, c) == blobId) {
							// 해당 픽셀에 클래스의 색상 적용 (50% 투명도)
							cv::Vec3b& pixel = visualImage.at<cv::Vec3b>(r, c);
							pixel[0] = pixel[0] * 0.5 + colors[maxClassIdx][0] * 0.5;
							pixel[1] = pixel[1] * 0.5 + colors[maxClassIdx][1] * 0.5;
							pixel[2] = pixel[2] * 0.5 + colors[maxClassIdx][2] * 0.5;
						}
					}
				}

				// blob의 중심 좌표 계산
				cv::Moments m = cv::moments(mask == blobId, true);
				if (m.m00 > 0) {
					int cx = static_cast<int>(m.m10 / m.m00);
					int cy = static_cast<int>(m.m01 / m.m00);

					// 클래스 번호를 중심에 텍스트로 표시   
					cv::putText(visualImage, "Class: " + std::to_string(maxClassIdx),
						cv::Point(cx, cy), cv::FONT_HERSHEY_SIMPLEX, 0.5,
						cv::Scalar(255, 255, 255), 2);
				}
			}
			return visualImage;
		}
		catch (const cv::Exception& e) {
			// LogError("이미지 시각화 중 OpenCV 예외 발생: " + std::string(e.what()));
			return cv::Mat();
		}
		catch (const std::exception& e) {
			// LogError("이미지 시각화 중 예외 발생: " + std::string(e.what()));
			return cv::Mat();
		}
	}
}
#endif // LGITAI