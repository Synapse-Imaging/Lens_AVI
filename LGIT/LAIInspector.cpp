#include "stdafx.h"
#ifdef LGITAI
#include "LAIInspector.h"
#include <exception>
#include <stdexcept> // std::runtime_error
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <unordered_set>

namespace LGIT {
	LAIInspector::LAIInspector(
		size_t num_threads,
		const std::array<std::string, 23>& condition_name_list,
		const std::array<std::string, 23>& condition_short_name_list,
		const std::vector<std::string>& model_path_list,
		const std::string setup_json_path,
		int inference_engine_logging_level,
		std::filesystem::path log_root,
		int div_type_idx) :
		_condition_name_list(condition_name_list),
		_condition_short_name_list(condition_short_name_list),
		_keys([&]() -> std::vector<std::string> 
			{
				std::vector<std::string> keys;
				std::unordered_set<std::string> key_set;
		
				for (const auto& model_path : model_path_list) 
				{
					std::filesystem::path path(model_path);
					std::string filename = path.filename().string(); // 확장자 포함 이름
					
					if (key_set.insert(filename).second) { 
						keys.emplace_back(filename);
					}
				}
				return keys;
			}())
	{
#pragma region logger 초기화
		std::string logger_name = "inspector_module_logger";
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
		std::string log_file = "InspectorModuleLog_" + get_current_date_string() + ".txt"; // 파일 싱크 설정

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
		_logger->info("LAIInspector: num_threads={}, setup_json_path={}, inference_engine_logging_level={}",
			num_threads,
			setup_json_path,
			inference_engine_logging_level);
#pragma endregion logger 초기화
#pragma region worker 초기화
		if (num_threads == 0) {
			_logger->critical("num_threads must be greater than 0.");
			_inspector_initialize = false;
		}
		_pool.init(num_threads);
		try {
			for (size_t i = 0; i < num_threads; ++i) {
				_logger->info("ModelInference: num_threads={}, setup_json_path={}", i, setup_json_path);
				auto model = std::make_shared<ModelInference>(model_path_list, setup_json_path, inference_engine_logging_level, div_type_idx);
				_models.push_back(model);
			}
			_inspector_initialize = true;
			_logger->info("LAIInspector initialized with {} models.", _models.size());
		}
		catch (const std::exception& e) {
			std::string error_msg = "Exception in LAIInspector constructor: ";
			_logger->error(error_msg + e.what());
			_inspector_initialize = false;
			_logger->error("Inspector models initialization failed: {}", error_msg);
			// throw std::runtime_error(error_msg);  // std::runtime_error로 예외 변환 후 던짐
		}
#pragma endregion worker 초기화

	}

	LAIInspector::~LAIInspector()
	{
		// 모든 작업이 완료될 때까지 대기
		while (!_pool.is_job_empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		// 멀티스레드 환경에서 풀을 안전하게 종료
		_pool.shutdown(); // ThreadPool이 안전하게 종료되도록 보장
		_models.clear(); // 모델 및 디바이스 목록 정리
	}

	std::future<LGIT::Model::LAIInspectorResult> LAIInspector::add_job(
		const std::string& product_name,
		const int vision_pos,
		const int tray_no,
		const int module_no,
		const int image_no,
		const int roi_no,
		const int roi_inspection_type_idx,
		const cv::Mat& img, 
		bool test_mode)
	{
		if (!_inspector_initialize) {
			_logger->error("Inspector is not initialized. Cannot add job.");
			return std::async(std::launch::deferred, []() {
				return LGIT::Model::LAIInspectorResult{};
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


		auto func = [this, product_name, vision_pos, image_no, roi_no, roi_inspection_type_idx, test_mode,  img = img.clone()]() {  // img 깊은 복사하여 안전하게 사용
			if (_models.size() == 0) {
				return LGIT::Model::LAIInspectorResult{};
			}
			size_t idx = _next_idx.fetch_add(1) % _models.size();
			return process_image(product_name, vision_pos, image_no, roi_no, roi_inspection_type_idx, img, idx, test_mode);
		};
		return _pool.enqueue_job(func);
	}

	bool LAIInspector::Keycontain(const std::string& key)
	{
		return std::find(_keys.begin(), _keys.end(), key) != _keys.end();
	}

	LGIT::Model::LAIInspectorResult LAIInspector::process_image(
		const std::string& product_name,
		const int vision_pos,
		const int image_no,
		const int roi_no,
		const int roi_inspection_type_idx,
		const cv::Mat & img,
		size_t idx,
		bool test_mode)
	{
		using namespace std::chrono;

		auto total_start = high_resolution_clock::now();

		if (img.empty()) {
			_logger->error("Input image is empty!");
			throw std::runtime_error("Input image is empty");
		}

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
			auto output = _models[idx]->RunInference(product_name, vision_pos, image_no, roi_no, roi_inspection_type_idx, inspect_image, inference_duration, test_mode);

			auto infer_end = high_resolution_clock::now();
			auto total_end = high_resolution_clock::now();

			auto prep_time = duration_cast<milliseconds>(prep_end - prep_start).count();
			auto infer_time = duration_cast<milliseconds>(infer_end - infer_start).count();
			auto total_time = duration_cast<milliseconds>(total_end - total_start).count();
			
			std::ostringstream oss;
			oss << std::this_thread::get_id();
			std::string thread_id_str = oss.str();

			std::ostringstream result_list_oss;
			for (auto result_l : output.result_list)
			{
				result_list_oss << result_l << ", ";
			}

			_logger->info("process_image: vision_pos={}, image_no={}, roi_no={}, type={}, img_size=({}, {}), thread_id={}, idx={}, prep={}ms, infer={}ms, total={}ms",
				vision_pos,
				image_no,
				roi_no,
				_condition_short_name_list[roi_inspection_type_idx],
				img.cols, img.rows,
				thread_id_str,
				idx,
				prep_time,
				inference_duration, 
				total_time);
			_logger->debug("process_image: vision_pos={}, image_no={}, roi_no={}, type={}, img_size=({}, {}), thread_id={}, idx={}, result_list={}",
				vision_pos,
				image_no,
				roi_no,
				_condition_short_name_list[roi_inspection_type_idx],
				img.cols, img.rows,
				thread_id_str,
				idx,
				result_list_oss.str());
			return output;
		}
		catch (const std::exception& e) {
			_logger->error("Exception in process_image(idx={}): {}", idx, e.what());
			std::cerr << "General error: " << e.what() << std::endl;
			
			return LGIT::Model::LAIInspectorResult{};
		}
	}


	std::string LAIInspector::get_current_date_string()
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