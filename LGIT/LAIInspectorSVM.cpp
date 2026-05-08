#include "stdafx.h"
#ifdef LGITAI
#include "LAIInspectorSVM.h"
#include <exception>
#include <stdexcept> // std::runtime_error
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace LGIT {
	LAIInspectorSVM::LAIInspectorSVM(
		size_t num_threads,
		const std::array<std::string, 23>& condition_name_list,
		const std::array<std::string, 23>& condition_short_name_list,
		const std::string model_dir,
		int inference_engine_logging_level,
		std::filesystem::path log_root,
		int div_type_idx) :
		_condition_name_list(condition_name_list),
		_condition_short_name_list(condition_short_name_list)
	{
		std::string logger_name = "inspector_svm_module_logger";
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
		std::string log_file = "InspectorSVMModuleLog_" + get_current_date_string() + ".txt"; // 파일 싱크 설정

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

		_logger->info("LAIInspectorS: num_threads={}, model_dir={}, inference_engine_logging_level={}",
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
				auto model = std::make_shared<ModelInferenceSVM>(condition_name_list, condition_short_name_list, model_dir, div_type_idx);
				_models.push_back(model);
			}
			_inspector_initialize = true;
			_logger->info("LAIInspectorSVM initialized with {} models.", _models.size());
		}
		catch (const std::exception& e) {
			std::string error_msg = "Exception in LAIInspector2 constructor: ";
			_logger->error(error_msg + e.what());
			_inspector_initialize = false;
			_logger->error("SVM models initialization failed: {}", error_msg);			
			// throw std::runtime_error(error_msg);  // std::runtime_error로 예외 변환 후 던짐 // 이 부분은 주석 처리
		}
	}

	LAIInspectorSVM::~LAIInspectorSVM()
	{
		// 모든 작업이 완료될 때까지 대기
		while (!_pool.is_job_empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		// 멀티스레드 환경에서 풀을 안전하게 종료
		_pool.shutdown(); // ThreadPool이 안전하게 종료되도록 보장
		_models.clear(); // 모델 및 디바이스 목록 정리
	}

	std::future<LGIT::Model::LAIInspectorSVMResult> LAIInspectorSVM::add_job(
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
		const cv::Mat& mask)
	{
		if (_inspector_initialize == false) {
			_logger->error("SVM models are not initialized.");
			return std::async(std::launch::deferred, []() {
				return LGIT::Model::LAIInspectorSVMResult{};
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

		auto func = [this,
			product_name, lot_id, barcode,
			vision_pos, image_no, module_no,
			roi_no, inspection_tab_idx, roi_inspection_type_idx,
			defect_name, img = img.clone(), mask = mask.clone()]() {  // img 깊은 복사하여 안전하게 사용
			if (_models.size() == 0) {
				return LGIT::Model::LAIInspectorSVMResult{};
			}
			size_t idx = _next_idx.fetch_add(1) % _models.size();
			return process_image(
				product_name, lot_id, barcode,
				vision_pos, image_no, module_no,
				roi_no, inspection_tab_idx, roi_inspection_type_idx,
				defect_name,
				img, mask, idx);
		};

		return _pool.enqueue_job(func);
	}

	bool LAIInspectorSVM::try_model_reload() {
		if (_inspector_initialize == false) {
			_logger->error("SVM models are not initialized.");
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

	LGIT::Model::LAIInspectorSVMResult LAIInspectorSVM::process_image(
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
		size_t idx)
	{
		if (_inspector_initialize == false) {
			_logger->error("SVM models are not initialized.");
			throw std::runtime_error("SVM models are not initialized.");
		}

		using namespace std::chrono;

		auto total_start = high_resolution_clock::now();

		cv::Mat inspect_image;
		auto prep_start = high_resolution_clock::now();
		if (img.channels() == 1) {
			std::vector<cv::Mat> channels(3, img);
			cv::merge(channels, inspect_image);
		}
		else if (img.channels() == 3) {
			inspect_image = img;  // 복사 불필요
		}
		else {
			std::string msg = fmt::format("Unsupported channel count: {}", img.channels());
			_logger->error(msg);
			throw std::runtime_error(msg);
		}
		auto prep_end = high_resolution_clock::now();

		try {
			auto infer_start = high_resolution_clock::now();

			UINT64 inference_duration = -1;
			LGIT::Model::LAIInspectorSVMResult outputs = _models[idx]->RunInference(
				product_name, lot_id, barcode,
				vision_pos, image_no, roi_no,
				inspection_tab_idx, roi_inspection_type_idx, defect_name,
				inspect_image, mask, inference_duration);

			auto infer_end = high_resolution_clock::now();
			auto total_end = high_resolution_clock::now();

			auto prep_time = duration_cast<milliseconds>(prep_end - prep_start).count();
			auto infer_time = duration_cast<milliseconds>(infer_end - infer_start).count();
			auto total_time = duration_cast<milliseconds>(total_end - total_start).count();

			std::ostringstream oss;
			oss << std::this_thread::get_id();
			std::string thread_id_str = oss.str();

			std::string key = LGIT::ModelInferenceSVM::MakeInferenceEngineRunMemoryKey(product_name, _condition_name_list, vision_pos, image_no, roi_no, inspection_tab_idx, roi_inspection_type_idx, defect_name);

			_logger->info("process_image: vision_pos={}, image_no={}, module_no={}, roi_no={}, type={}, img_size=({}, {}), thread_id={}, idx={}, key={}, prep={}ms, infer={}ms, total={}ms, pool_size={}",
				vision_pos,
				image_no,
				module_no,
				roi_no,
				_condition_short_name_list[roi_inspection_type_idx],
				img.cols, img.rows,
				thread_id_str,
				idx,
				key,
				prep_time,
				inference_duration,
				total_time,
				_pool.get_job_count());
			std::ostringstream blob_id_oss, class_idx_oss, score_oss;
			for (auto blob_id : outputs.blob_id_list)
			{
				blob_id_oss << blob_id << ", ";
			}
			for (auto class_idx : outputs.class_idx_list)
			{
				class_idx_oss << class_idx << ", ";
			}
			for (auto score : outputs.score_list)
			{
				score_oss << score << ", ";
			}
			_logger->debug("process_image: vision_pos={}, image_no={}, module_no={}, roi_no={}, type={}, img_size=({}, {}), thread_id={}, idx={}, result={}, blob_count={}, blob_id_list={}, class_idx_list={}, score_list={}",
				vision_pos,
				image_no,
				module_no,
				roi_no,
				_condition_short_name_list[roi_inspection_type_idx],
				img.cols, img.rows,
				thread_id_str,
				idx,
				outputs.result,
				outputs.blob_count,
				blob_id_oss.str(),
				class_idx_oss.str(),
				score_oss.str());
			return outputs;
		}
		catch (const std::exception& e) {
			_logger->error("Exception in process_image(idx={}): {}", idx, e.what());
			std::cerr << "General error: " << e.what() << std::endl;
			return {};
		}
	}


	std::string LAIInspectorSVM::get_current_date_string()
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