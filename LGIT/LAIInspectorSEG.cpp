#include "stdafx.h"
#ifdef LGITAI
#include "LAIInspectorSEG.h"
#include <exception>
#include <stdexcept> // std::runtime_error
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <vector>

namespace LGIT {
	LAIInspectorSEG::LAIInspectorSEG(
		size_t num_threads,
		const std::array<std::string, 23>& condition_name_list,
		const std::array<std::string, 23>& condition_short_name_list, 
		const std::string model_dir,
		int inference_engine_logging_level, 
		std::filesystem::path log_root,
		int div_type_idx):
		_condition_name_list(condition_name_list),
		_condition_short_name_list(condition_short_name_list)
	{
		std::string logger_name = "inspector_seg_module_logger";
		std::filesystem::path log_folder = log_root / logger_name;

		try {
			std::filesystem::create_directories(log_folder);
		}
		catch (const std::exception& e) {
			_logger->error("Failed to create log directory: {}", e.what());
		}

		// 이미 존재하는 로거 가져오기
		auto logger = spdlog::get(logger_name);
		if (logger) {
			_logger = logger;
		}
		// 비동기 로깅 생성
		if (!spdlog::thread_pool()) {
			spdlog::init_thread_pool(8192, 1); // queue size, threads
		}
		std::string log_file = "InspectorSEGModuleLog_" + get_current_date_string() + ".txt"; // 파일 싱크 설정

		std::filesystem::path full_log_path = log_folder / log_file;
		_logger = spdlog::basic_logger_mt<spdlog::async_factory>(logger_name, full_log_path.string());
		_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v"); // 포맷 커스터마이징
		if (inference_engine_logging_level >= spdlog::level::trace &&
			inference_engine_logging_level < spdlog::level::n_levels) {

			auto log_level = static_cast<spdlog::level::level_enum>(inference_engine_logging_level);
			_logger->set_level(log_level);
			_logger->flush_on(log_level);
		}
		else {
			_logger->set_level(spdlog::level::info);
			_logger->flush_on(spdlog::level::warn);
			_logger->warn("Invalid log level: {}. Defaulting to info level.", inference_engine_logging_level);
		}

		_logger->info("LAIInspectorSEG: num_threads={}, model_dir={}, inference_engine_logging_level={}",
			num_threads,
			model_dir,
			inference_engine_logging_level);

		if (num_threads == 0) {
			_inspector_initialize = false;
			_logger->error("num_threads must be greater than 0.");
		}
		_pool.init(num_threads);

		try {
			for (size_t i = 0; i < num_threads; ++i) {
				_logger->info("ModelInference: num_threads={}, model_dir={}", i, model_dir);
				auto model = std::make_shared<ModelInferenceSEG>(condition_name_list, condition_short_name_list, model_dir, div_type_idx);
				_models.push_back(model);
			}
			_inspector_initialize = true;
			_logger->info("LAIInspectorSEG initialized with {} models.", _models.size());
		}
		catch (const std::exception& e) {
			std::string error_msg = "Exception in LAIInspectorSEG constructor: ";
			_logger->error(error_msg + e.what());
			_inspector_initialize = false;
			_logger->error("SEG models initialization failed: {}", error_msg);
			// throw std::runtime_error(error_msg);  // std::runtime_error로 예외 변환 후 던짐 // 이 부분은 주석 처리
		}
	}

	std::future<std::array<cv::Mat, 8>> LAIInspectorSEG::add_job(
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
		const std::array<cv::Mat, 8>& images) 
	{
		if (_inspector_initialize == false) {
			_logger->error("SEG models are not initialized.");
			return std::async(std::launch::deferred, []() {
				return std::array<cv::Mat, 8>();
			});
		}

		std::ostringstream oss;
		oss << std::this_thread::get_id();
		std::string thread_id_str = oss.str();

		_logger->info("add_job: vision_pos={}, tray_no={}, module_no={}, image_no={}, roi_no={}, type={}, thread_id={}",
			vision_pos,
			tray_no,
			module_no,
			image_no,
			roi_no,
			_condition_short_name_list[roi_inspection_type_idx],
			thread_id_str);


		size_t idx = _next_idx.fetch_add(1) % _models.size();
		
		auto func = [this,
			product_name, lot_id, barcode,
			vision_pos, image_no, module_no,
			roi_no, inspection_tab_idx, roi_inspection_type_idx,
			defect_name, images]() {
			if (_models.size() == 0) {
				return std::array<cv::Mat, 8>();
			}
			size_t idx = _next_idx.fetch_add(1) % _models.size();
			return process_image(
				product_name, lot_id, barcode,
				vision_pos, image_no, module_no,
				roi_no, inspection_tab_idx, roi_inspection_type_idx,
				defect_name,
				images, idx);
		};

		return _pool.enqueue_job(func);
	}

	bool LAIInspectorSEG::try_model_reload() {
		if (_inspector_initialize == false) {
			_logger->error("SEG models are not initialized.");
			return false;
		}
		if (_models.size() == 0) {
			_logger->error("No models available for reloading.");
			return false;
		}

		for (size_t i = 0; i < _models.size(); ++i) {
			try {
				if (_models[i]->TryModelReload()) {
					_logger->info("Model {} reloaded successfully.", i);
				}
				else {
					_logger->info("Model {} reload not needed or failed.", i);
				}
			}
			catch (const std::exception& e) {
				_logger->error("Exception during model {} reload: {}", i, e.what());
			}
		}

		return true;
	}

	std::array<cv::Mat, 8> LAIInspectorSEG::process_image(
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
		const std::array<cv::Mat, 8>& images,
		size_t idx) 
	{
		if (_inspector_initialize == false) {
			_logger->error("SEG models are not initialized.");
			throw std::runtime_error("SEG models are not initialized.");
		}
		
		std::array<cv::Mat, 8> result_masks;
		UINT64 inference_duration = 0;
		
		try {
			if (idx < _models.size()) {
				int inference_result = _models[idx]->RunInferenceArray(
					product_name, lot_id, barcode, vision_pos, image_no, roi_no,
					inspection_tab_idx, roi_inspection_type_idx, defect_name,
					images, result_masks, inference_duration
				);
				
				_logger->info("Inference completed for product: {}, result: {}, duration: {} μs", 
					product_name, inference_result, inference_duration);
			}
			else {
				_logger->error("Invalid model index: {}", idx);
			}
		}
		catch (const std::exception& e) {
			_logger->error("Exception during inference: {}", e.what());
		}
		
		return result_masks;
	}

	std::string LAIInspectorSEG::get_current_date_string()
	{
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);

		std::tm tm;
		localtime_s(&tm, &now_c);  // 안전한 localtime

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y%m%d");  // "20250321"
		return oss.str();
	}
}
#endif // LGITAI