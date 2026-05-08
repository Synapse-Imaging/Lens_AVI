#pragma once
#ifdef LGITAI
#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <fstream>        // JSON 파일 읽기를 위해 추가
#include <algorithm>      // std::find, std::remove를 위해 추가
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <thread>
#include <random>         // std::mt19937, std::uniform_int_distribution
#include <functional>     // std::hash
#include <nlohmann\json.hpp>
#include "ModelInference2.h"
#include "../AIService/ThreadPool.h"
#include "Model/LAIInspector2Setup.h"
#include "Model/LAIInspector2Result.h"

namespace LGIT {
	class LAIInspector2
	{
	public:
		LAIInspector2(
			size_t num_threads,
			const std::array<std::string, 23>& condition_name_list,
			const std::array<std::string, 23>& condition_short_name_list,
			const std::string model_dir,
			int inference_engine_logging_level,
			std::filesystem::path log_root,
			int div_type_idx = 1);
		~LAIInspector2();

		std::future<LGIT::Model::LAIInspector2Result> add_job(
			const std::string& product_name,
			const std::string& lot_id,
			const std::string& barcode,
			const int vision_pos,
			const int tray_no,
			const int module_no,
			const int image_no,
			const int roi_no,
			const int inspection_tab_idx,
			const int roi_inspection_type_idx,
			const std::string& defect_name,
			const cv::Mat& img,
			const cv::Mat& mask);

		bool try_model_reload();

	private:
		LGIT::Model::LAIInspector2Result process_image(
			const std::string& product_name,
			const std::string& lot_id,
			const std::string& barcode,
			const int vision_pos,
			const int image_no,
			const int module_no,
			const int roi_no,
			const int inspection_tab_idx,
			const int roi_inspection_type_idx,
			const std::string& defect_name,
			const cv::Mat& img,
			const cv::Mat& mask,
			size_t idx);
		std::string get_current_date_string();

	private:
		AIService::ThreadPool _pool; // 멀티스레드 작업을 위한 스레드 풀
		std::vector<std::shared_ptr<ModelInference2>> _models; // 모델 목록
		std::atomic<size_t> _next_idx{ 0 };  // 원자적(atomic)으로 관리되는 인덱스 변수
		std::shared_ptr<spdlog::logger> _logger; // 초기화된 로거
		std::atomic_bool _inspector_initialize = false; // 초기화 상태를 나타내는 플래그

		std::string _model_dir; // 모델 디렉토리 경로
		std::string _update_model_info_json_path; // 모델 업데이트 정보 JSON 파일 경로

	public:
		const std::array<std::string, 23> _condition_name_list;
		const std::array<std::string, 23> _condition_short_name_list;

	};
}
#endif // LGITAI