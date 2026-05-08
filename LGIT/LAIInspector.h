#pragma once
#ifdef LGITAI
#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <thread>
#include <nlohmann/json.hpp>
#include "ModelInference.h"
#include "../AIService/ThreadPool.h"
#include "Model/LAIInspectorSetup.h"
#include "Model/LAIInspectorResult.h"

namespace LGIT {
	class LAIInspector
	{
	public:
		LAIInspector(
			size_t num_threads, 
			const std::array<std::string, 23>& condition_name_list,
			const std::array<std::string, 23>& condition_short_name_list,
			const std::vector<std::string>& model_path_list,
			const std::string setup_json_path,
			int inference_engine_logging_level,
			std::filesystem::path log_root,
			int div_type_idx = 1);
		~LAIInspector();

		std::future<LGIT::Model::LAIInspectorResult> LAIInspector::add_job(
			const std::string& product_name,
			const int vision_pos,
			const int tray_no,
			const int module_no,
			const int image_no,
			const int roi_no,
			const int roi_inspection_type_idx,
			const cv::Mat& img,
			bool test_mode = false);

		bool Keycontain(const std::string& key);

	private:
		LGIT::Model::LAIInspectorResult process_image(
			const std::string& product_name,
			const int vision_pos,
			const int image_no,
			const int roi_no,
			const int roi_inspection_type_idx,
			const cv::Mat& img, 
			size_t idx,
			bool test_mode = false);
		std::string get_current_date_string();

	private:
		AIService::ThreadPool _pool;
		std::vector<std::shared_ptr<ModelInference>> _models;
		std::atomic<size_t> _next_idx{ 0 };  // 원자적(atomic)으로 관리되는 인덱스 변수
		std::shared_ptr<spdlog::logger> _logger;
		const std::vector<std::string> _keys; // 초기화에서만 쓰고 수정 불가 
		std::atomic_bool _inspector_initialize = false; // 초기화 상태를 나타내는 플래그

	public:
		const std::array<std::string, 23> _condition_name_list;
		const std::array<std::string, 23> _condition_short_name_list;
	};
}
#endif // LGITAIs