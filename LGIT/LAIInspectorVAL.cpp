#include "stdafx.h"
#ifdef LGITAI
#include "LAIInspectorVAL.h"
#include <exception>
#include <stdexcept> // std::runtime_error
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <vector>

namespace LGIT {
	LAIInspectorVAL::LAIInspectorVAL(
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
		std::string logger_name = "inspector_val_module_logger";
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
		std::string log_file = "InspectorVALModuleLog_" + get_current_date_string() + ".txt"; // 파일 싱크 설정

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

		_logger->info("LAIInspectorVAL: num_threads={}, model_dir={}, inference_engine_logging_level={}",
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
				auto model = std::make_shared<ModelInferenceVAL>(condition_name_list, condition_short_name_list, model_dir, div_type_idx);
				_models.push_back(model);
			}
			_inspector_initialize = true;
			_logger->info("LAIInspectorVAL initialized with {} models.", _models.size());
		}
		catch (const std::exception& e) {
			std::string error_msg = "Exception in LAIInspectorVAL constructor: ";
			_logger->error(error_msg + e.what());
			_inspector_initialize = false;
			_logger->error("SEG models initialization failed: {}", error_msg);
			// throw std::runtime_error(error_msg);  // std::runtime_error로 예외 변환 후 던짐 // 이 부분은 주석 처리
		}
	}

	std::future<Model::LAIInspectorResult> LAIInspectorVAL::add_job(
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
		const double local_align_angle_value,
		const std::string& defect_name,
		const cv::Mat image) 
	{
		if (_inspector_initialize == false) {
			_logger->error("SEG models are not initialized.");
			return std::async(std::launch::deferred, []() {
				return Model::LAIInspectorResult{};
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
		
		auto func = [
			this,
			product_name,
			lot_id, 
			barcode,
			vision_pos,
			image_no, 
			module_no,
			roi_no,
			inspection_tab_idx,
			roi_inspection_type_idx,
			local_align_angle_value,
			defect_name, 
			image
		]() {
			if (_models.size() == 0) {
				return Model::LAIInspectorResult{};
			}
			size_t idx = _next_idx.fetch_add(1) % _models.size();
			return process_image(
				product_name,
				lot_id,
				barcode,
				vision_pos,
				image_no, 
				module_no,
				roi_no, 
				inspection_tab_idx, 
				roi_inspection_type_idx,
				local_align_angle_value,
				defect_name,
				image,
				idx
			);
		};

		return _pool.enqueue_job(func);
	}

	bool LAIInspectorVAL::try_model_reload()
	{
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

	Model::LAIInspectorResult LAIInspectorVAL::process_image(
		const std::string& product_name,
		const std::string& lot_id,
		const std::string& barcode,
		const int vision_pos,
		const int image_no,
		const int module_no,
		const int roi_no,
		const int inspection_tab_idx,
		const int roi_inspection_type_idx,
		const double local_align_angle_value,
		const std::string& defect_name,
		const cv::Mat& image,
		size_t idx) 
	{
		if (_inspector_initialize == false) {
			_logger->error("SEG models are not initialized.");
			throw std::runtime_error("SEG models are not initialized.");
		}
		
		cv::Mat result_mask;
		UINT64 inference_duration = 0;
		bool inference_result = false;
		
		try {
			if (idx < _models.size()) {
				std::string vision_pos_str = (vision_pos == 0) ? "BC" : "TC";
				std::string key = product_name + "_" + vision_pos_str + "_Img" +
					std::to_string(image_no) + "_ROI" + std::to_string(roi_no) + "_Tab" + std::to_string(roi_inspection_type_idx);
				_logger->debug("TEMP Key for Debug: {}", key);

				inference_result = _models[idx]->RunInference(
					product_name, 
					lot_id, 
					barcode, 
					vision_pos, 
					image_no, roi_no,
					inspection_tab_idx,
					roi_inspection_type_idx,
					local_align_angle_value,
					defect_name,
					image,
					result_mask, 
					inference_duration
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
		
		Model::LAIInspectorResult result{ inference_result, result_mask };
		return result;
	}

	std::string LAIInspectorVAL::get_current_date_string()
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

