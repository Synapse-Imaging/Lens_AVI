#include "stdafx.h"
#include "../AIService/ImageUtile.h"
#include "../AIService/ThreadPool.h"
#include "Algorithm.h"
#include "GTRegion.h"
#include "ServiceDirector.h"
#include "ServiceOperator.h"
#include <fstream>
#include <future>
#include <iosfwd>
#include <map>
#include <memory>
#include <opencv2/imgcodecs.hpp>
#include <optional>
#include <syai/runtime/SimpleSmartType.h>
#include <unordered_set>
#include <vector>

namespace syai_service
{
	using namespace syai::runtime;
	using namespace syai::runtime::inspection;
	using namespace syai::runtime::domain::config;
	using namespace syai::runtime::domain::inspection;

	ServiceDirector::ServiceDirector(size_t num_thread_count,
									 int logging_level,
									 bool is_use_visualization,
									 std::filesystem::path log_root,
									 size_t syai_num_thread_count)
	{
		std::string log_name = "SD";
		// 로거 초기화
		if (!initialize_logger(log_name.c_str(), log_root.string().c_str(), logging_level)) {
		}
		// service_manager_ = std::make_unique<ServiceManager>(_logger, syai_num_thread_count);
		thread_pool_ = std::make_shared<AIService::ThreadPool>();
		thread_pool_->init(num_thread_count);

		// 전처리 전용 쓰레드 풀 초기화
		preprocessing_thread_pool_ = std::make_shared<AIService::ThreadPool>();
		//preprocessing_thread_pool_->init(num_thread_count * 2);
		preprocessing_thread_pool_->init(6);

		save_image_thread_pool_ = std::make_shared<AIService::ThreadPool>();
		//save_image_thread_pool_->init(num_thread_count * 2);
		save_image_thread_pool_->init(6);

		// service_manager_ = std::make_unique<ServiceManager>(_logger, syai_num_thread_count);
		service_operator_ = std::make_unique<ServiceOperator>(_logger);
	}

	ServiceDirector::~ServiceDirector()
	{
		service_operator_.reset();
	}

	// 로거 초기화
	bool ServiceDirector::initialize_logger(const char* log_name, const char* log_root, int log_level)
	{
		try {
			std::string log_root_str = log_root ? std::string(log_root) : "C:\\sya\\logs";
			std::string logger_name = log_name ? std::string(log_name) : "service_log";

			std::filesystem::path log_folder = std::filesystem::path(log_root_str) / logger_name;

			try {
				std::filesystem::create_directories(log_folder);
			}
			catch (const std::exception& e) {
				std::cerr << "[Service:" << log_name << "] Failed to create log directory: " << e.what() << std::endl;
				return false;
			}

			// 이미 존재하는 로거 확인
			auto existing_logger = spdlog::get(logger_name);
			if (existing_logger) {
				_logger = existing_logger;
				_logger->warn("[Service] Initialize called but _logger already exists - using existing _logger");
				return true;
			}

			// 비동기 로깅 스레드 풀 초기화
			if (!spdlog::thread_pool()) {
				spdlog::init_thread_pool(8192, 1);
			}

			// Multi-sink 로거 생성
			std::vector<spdlog::sink_ptr> sinks;

			auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
				(log_folder / "daily_log.log").string(), 0, 0, false, 5);

			auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
				(log_folder / "size_rotated_log.log").string(), 50 * 1024 * 1024, 10);

			sinks.push_back(daily_sink);
			sinks.push_back(rotating_sink);

			_logger = std::make_shared<spdlog::async_logger>(
				logger_name, sinks.begin(), sinks.end(),
				spdlog::thread_pool(), spdlog::async_overflow_policy::block);

			_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");

			auto log_level_value = log_level;
			if (log_level_value >= static_cast<int>(spdlog::level::trace) &&
				log_level_value < static_cast<int>(spdlog::level::n_levels)) {
				auto set_log_level = static_cast<spdlog::level::level_enum>(log_level_value);
				_logger->set_level(set_log_level);
				_logger->flush_on(set_log_level);
			}
			else {
				_logger->set_level(spdlog::level::info);
				_logger->flush_on(spdlog::level::warn);
			}

			spdlog::register_logger(_logger);

			if (_logger) _logger->info("[Service] Logger initialized. Log folder: {}", log_folder.string());
			return true;
		}
		catch (const std::exception& e) {
			std::cerr << "[Service] Failed to initialize _logger: " << e.what() << std::endl;
			return false;
		}
	}

	std::vector<std::string> ServiceDirector::get_managed_names()
	{
		return service_operator_->get_manager_names();
	}

	bool ServiceDirector::init(std::string syai_log_name,
							   std::string syai_log_root,
							   LogLevel log_level,
							   SmartVector<ManagerInfo> manager_setups)
	{
		_logger->info("[Director] Initializing ServiceDirector with log_name='{}', log_root='{}', log_level={}, manager_setups size={}",
					  syai_log_name,
					  syai_log_root,
					  static_cast<int>(log_level),
					  manager_setups.size());
		auto status = service_operator_->init(syai_log_name,
											  syai_log_root,
											  log_level,
											  manager_setups);
		if (status != ServiceStatus::SUCCESS) {
			_logger->error("[Director] ServiceOperator initialization failed with status={}", static_cast<int>(status));
			return false;
		}
		_logger->info("[Director] ServiceDirector initialized successfully.");
		is_initialize_completed_ = true;
		return true;
	}

	bool ServiceDirector::init_config(std::string config_path)
	{
		_logger->info("[Director] Initializing ServiceDirector with config_path='{}'", config_path);
		auto status = service_operator_->init_config(config_path);
		if (status != ServiceStatus::SUCCESS) {
			_logger->error("[Director] ServiceOperator configuration initialization failed with status={}", static_cast<int>(status));
			return false;
		}
		_logger->info("[Director] ServiceDirector configuration initialized successfully.");
		is_initialize_completed_ = true;
		return true;
	}

	bool ServiceDirector::re_init_config(std::string config_path)
	{
		_logger->info("[Director] Re-initializing ServiceDirector with config_path='{}'", config_path);
		auto status = service_operator_->re_init_config(config_path);
		if (status != ServiceStatus::SUCCESS) {
			_logger->error("[Director] ServiceOperator configuration re-initialization failed with status={}", static_cast<int>(status));
			return false;
		}
		_logger->info("[Director] ServiceDirector configuration re-initialized successfully.");
		is_initialize_completed_ = true;
		return true;
	}

	std::future<JobInfo> ServiceDirector::add_job(const Request& request, const JobInfoAddon& job_info_addon)
	{
		auto job_counter = job_counter_++;
		auto response_result = std::make_shared<ResponseResult>();
		auto future_response = response_result->response_job.get_future();

		thread_pool_->enqueue_job([this, job_counter, request, response_result, job_info_addon]() {
			try {
				JobInfo job_info;
				job_info.request = request;
				job_info.addon = job_info_addon;

				if (job_info.request.has_value()) {
					const auto& req = job_info.request.value();
					std::string inspection_id = req.get_inspection_id();
					std::string log_id = req.get_log_id();

					_logger->info("[Director] Started job {}: inspection_id='{}', log_id='{}'",
								  job_counter,
								  inspection_id,
								  log_id);

					auto response = process_image(request);

					if (response.has_value()) {
						job_info.response = response.value();
						_logger->info("[Director] Completed job {}: inspection_id='{}', log_id='{}'",
									  job_counter,
									  inspection_id,
									  log_id);
					}
					else {
						_logger->error("[Director] Failed job {}: inspection_id='{}', log_id='{}'",
									   job_counter,
									   inspection_id,
									   log_id);
					}

					if (!response_result->set_value(std::move(job_info))) {
						_logger->warn("[Director] Failed to set result for job {}: promise already satisfied",
									  job_counter);
					}
				}
				else {
					_logger->error("[Director] Invalid request for job {}", job_counter);
					response_result->release_with_error("Invalid request: request object is empty");
				}
			}
			catch (const std::exception& e) {
				_logger->error("[Director] Exception in job {}: error='{}'", job_counter, e.what());
				try {
					response_result->release_with_exception(std::current_exception());
				}
				catch (...) {
					_logger->error("[Director] Failed to set exception for job {}", job_counter);
				}
			}
			catch (...) {
				_logger->error("[Director] Unknown exception in job {}", job_counter);
				try {
					response_result->release_with_error("Unknown exception occurred during job processing");
				}
				catch (...) {
					_logger->error("[Director] Failed to set error for job {}", job_counter);
				}
			}
								  });

		return future_response;
	}

	// ===== Helper 함수들 구현 =====

	bool ServiceDirector::preprocess_roi_image_(
		uint64_t job_counter,
		const PreprocessingJobInput& input,
		cv::Mat& out_cv_roi_img,
		cv::Mat& out_cv_roi_img_v2,
		int& out_x1, int& out_y1, int& out_x2, int& out_y2,
		int& out_original_width, int& out_original_height,
		std::string& out_error_msg)
	{
		try {
			_logger->debug("[Director] Preprocessing job {}: Stage 1 - ROI Image Preprocessing starting", job_counter);

			HObject image = input.algorithm->m_HInspectCImage[input.buffer_idx][CHANNEL_TYPE_COLOR][input.image_idx - 1];
			HObject ho_Cropped, ho_Cropped_v2;
			HalconCpp::HTuple raw_image_width, raw_image_height;
			HTuple crop_row1, crop_col1, crop_row2, crop_col2;

			int iPadding = 128;
			int dilation = 5;

			if (!AIService::ImageUtile::RoiImagePreprocessing(image, input.roi_reg,
															  input.length1, input.length2,
															  ho_Cropped, out_cv_roi_img,
															  ho_Cropped_v2, out_cv_roi_img_v2,
															  raw_image_width, raw_image_height,
															  crop_row1, crop_col1, crop_row2, crop_col2,
															  iPadding, dilation, _logger)) {
				_logger->warn("[Director] Preprocessing job {}: Stage 1 - RoiImagePreprocessing returned false",
							  job_counter);

				try {
					if (AIService::ImageUtile::ValidHRegion(input.defect_reg)) {
						input.algorithm->m_HDefectRgn_ImageNo[input.buffer_idx][input.image_idx - 1].ConcatObj(input.defect_reg);
						input.algorithm->m_HDefectRgn_DefectName[input.buffer_idx][input.gt_region->miInspectionType - 1][input.gt_region->m_AlgorithmParam[input.tab_idx].m_iDefectType].ConcatObj(input.defect_reg);
					}
				}
				catch (const std::exception& e) {
					_logger->warn("[Director] Preprocessing job {}: Failed to save defect regions: {}", job_counter, e.what());
				}

				out_error_msg = "RoiImagePreprocessing failed";
				return false;
			}

			out_x1 = static_cast<int>(crop_col1.D());
			out_y1 = static_cast<int>(crop_row1.D());
			out_x2 = static_cast<int>(crop_col2.D());
			out_y2 = static_cast<int>(crop_row2.D());
			out_original_width = raw_image_width.I();
			out_original_height = raw_image_height.I();

			_logger->info("[Director] Preprocessing job {}: Stage 1 - Completed. ROI coords: ({},{}) - ({},{}), Original size: {}x{}",
						  job_counter, out_x1, out_y1, out_x2, out_y2, out_original_width, out_original_height);

			// ROI 이미지 저장이 필요한 경우 저장
			// 조건 반전으로 가독성 향상: 저장이 필요하지 않은 경우를 먼저 체크
			if (!input.enable_save_roi_image) {
				_logger->debug("[Director] Preprocessing job {}: Stage 1 - ROI image saving is disabled, skipping save step", job_counter);
				return true;
			}
			if (input.save_dir_list.empty()) {
				_logger->warn("[Director] Preprocessing job {}: Stage 1 - enable_save_roi_image is true but save_dir_list is empty, skipping save step", job_counter);
				return true;
			}
			if (input.save_roi_type < 0 || !AIService::ImageUtile::ValidHRegion(input.defect_reg)) {
				_logger->warn("[Director] Preprocessing job {}: Stage 1 - enable_save_roi_image is true but save_roi_type is invalid and defect_reg is not valid, skipping save step", job_counter);
				return true;
			}
			for (std::string dir : input.save_dir_list) {
				if (ho_Cropped.IsInitialized()) {
					bool can_save_image = true;
					std::ostringstream dir_stream;
					dir_stream << dir << "\\" << "ROIImage";
					std::filesystem::path dir_path(dir_stream.str());
					if (!std::filesystem::exists(dir_path)) {
						if (!std::filesystem::create_directories(dir_path)) {
							can_save_image = false;
						}
					}
					if (can_save_image) {
						AIService::ImageUtile::SaveImage(dir_path.string(), input.save_prefix, ho_Cropped);
					}
				}
				if (ho_Cropped_v2.IsInitialized()) {
					bool can_save_image = true;
					std::ostringstream dir_stream;
					dir_stream << dir << "\\" << "ROIImage";
					std::filesystem::path dir_path(dir_stream.str());
					if (!std::filesystem::exists(dir_path)) {
						if (!std::filesystem::create_directories(dir_path)) {
							can_save_image = false;
						}
					}
					if (can_save_image) {
						AIService::ImageUtile::SaveImage(dir_path.string(), input.save_prefix + "_V2", ho_Cropped_v2);
					}
				}
			}
			return true;
		}
		catch (const std::exception& e) {
			out_error_msg = std::string("Exception: ") + e.what();
			_logger->error("[Director] Preprocessing job {}: Stage 1 exception - {}", job_counter, out_error_msg);
			return false;
		}
		catch (...) {
			out_error_msg = "Unknown exception";
			_logger->error("[Director] Preprocessing job {}: Stage 1 unknown exception", job_counter);
			return false;
		}
	}

	bool ServiceDirector::select_ai_inspection_image_(
		uint64_t job_counter,
		const PreprocessingJobInput& input,
		const cv::Mat& cv_roi_img,
		const cv::Mat& cv_roi_img_v2,
		cv::Mat& out_ai_image,
		std::string& out_error_msg)
	{
		try {
			_logger->debug("[Director] Preprocessing job {}: Stage 2 - Select AI Inspection Image starting", job_counter);

			if (!cv_roi_img.empty()) {
				_logger->info("[Director] Preprocessing job {}: Stage 2 - Using cv_roi_img ({}x{})",
							  job_counter, cv_roi_img.cols, cv_roi_img.rows);
				out_ai_image = cv_roi_img;
				return true;
			}
			else if (!cv_roi_img_v2.empty()) {
				_logger->info("[Director] Preprocessing job {}: Stage 2 - Using cv_roi_img_v2 ({}x{})",
							  job_counter, cv_roi_img_v2.cols, cv_roi_img_v2.rows);
				out_ai_image = cv_roi_img_v2;
				return true;
			}
			else if (input.enable_roi_processing) {
				int width = static_cast<int>(input.length1.D());
				int height = static_cast<int>(input.length2.D());
				_logger->info("[Director] Preprocessing job {}: Stage 2 - Creating empty ROI image with size {}x{}",
							  job_counter, width, height);
				out_ai_image = cv::Mat::zeros(height, width, CV_8UC3);
				return true;
			}
			else {
				out_error_msg = "No valid AI inspection image available";
				_logger->error("[Director] Preprocessing job {}: Stage 2 - {}", job_counter, out_error_msg);
				return false;
			}
		}
		catch (const std::exception& e) {
			out_error_msg = std::string("Exception: ") + e.what();
			_logger->error("[Director] Preprocessing job {}: Stage 2 exception - {}", job_counter, out_error_msg);
			return false;
		}
		catch (...) {
			out_error_msg = "Unknown exception";
			_logger->error("[Director] Preprocessing job {}: Stage 2 unknown exception", job_counter);
			return false;
		}
	}

	bool ServiceDirector::create_mask_image_(
		uint64_t job_counter,
		const PreprocessingJobInput& input,
		const cv::Mat& ai_image,
		int roi_x1, int roi_y1, int roi_x2, int roi_y2,
		int original_width, int original_height,
		cv::Mat& out_mask_binary,
		cv::Rect& out_roi_rect,
		std::string& out_error_msg)
	{
		try {
			_logger->debug("[Director] Preprocessing job {}: Stage 3 - Create Mask Image starting", job_counter);

			cv::Size orignal_image_size = cv::Size(original_width, original_height);
			cv::Size roi_image_size = cv::Size(ai_image.cols, ai_image.rows);
			cv::Point left_top(roi_x1, roi_y1);
			cv::Point right_bottom(roi_x2, roi_y2);
			out_roi_rect = cv::Rect(roi_x1, roi_y1, roi_x2 - roi_x1, roi_y2 - roi_y1);

			if (!AIService::ImageUtile::ValidHRegion(input.defect_reg)) {
				out_error_msg = "Invalid defect region";
				_logger->error("[Director] Preprocessing job {}: Stage 3 - {}", job_counter, out_error_msg);
				return false;
			}
			// 여기로 빠지는것이 문제가 없는지 확인
			_logger->debug("[Director] Preprocessing job {}: Stage 3 - Converting region to labeled image", job_counter);

			auto mask_image = AIService::ImageUtile::RegionToLabeledImageV2(input.defect_reg,
																			orignal_image_size,
																			roi_image_size,
																			left_top,
																			right_bottom);
			if (!mask_image.has_value()) {
				out_error_msg = "Mask image generation failed";
				_logger->error("[Director] Preprocessing job {}: Stage 3 - {}", job_counter, out_error_msg);
				return false;
			}

			_logger->debug("[Director] Preprocessing job {}: Stage 3 - Binarizing mask", job_counter);

			cv::threshold(mask_image.value(), out_mask_binary, 0, 255, cv::THRESH_BINARY);

			_logger->info("[Director] Preprocessing job {}: Stage 3 - Completed. Mask size: {}x{}",
						  job_counter, out_mask_binary.cols, out_mask_binary.rows);

			// 마스크 이미지 저장이 필요한 경우 저장
			if (input.enable_save_roi_image) {
				for (std::string dir : input.save_dir_list) {
					bool can_save_image = true;
					std::ostringstream dir_stream;
					dir_stream << dir << "\\" << "ROIImage" << "\\" << "VisionMask";
					std::filesystem::path dir_path(dir_stream.str());
					if (!std::filesystem::exists(dir_path)) {
						if (!std::filesystem::create_directories(dir_path)) {
							can_save_image = false;
						}
					}
					if (can_save_image) {
						try {
							cv::imwrite((dir_path / (input.save_prefix + ".png")).string(), out_mask_binary);
						}
						catch (const std::exception& e) {
							_logger->warn("[Director] Preprocessing job {}: Failed to save mask image: {}", job_counter, e.what());
						}
					}
				}
				return true;
			}
		}
		catch (const std::exception& e) {
			out_error_msg = std::string("Exception: ") + e.what();
			_logger->error("[Director] Preprocessing job {}: Stage 3 exception - {}", job_counter, out_error_msg);
			return false;
		}
		catch (...) {
			out_error_msg = "Unknown exception";
			_logger->error("[Director] Preprocessing job {}: Stage 3 unknown exception", job_counter);
			return false;
		}
	}

	bool ServiceDirector::generate_model_key_(
		uint64_t job_counter,
		const PreprocessingJobInput& input,
		std::string& out_model_key,
		std::string& out_error_msg)
	{
		try {
			_logger->debug("[Director] Preprocessing job {}: Stage 5 - Generate Model Key starting", job_counter);

			NameBuildOptions build_options;
			build_options.consider_vision_pos = false;
			build_options.consider_inspection_type = true;
			// input.gt_region->m_AlgorithmParam[input.tab_idx].m_bCheckAIUseConsiderImageNo
			if (input.gt_region && input.tab_idx >= 0) {
				build_options.consider_image_index = input.gt_region->m_AlgorithmParam[input.tab_idx].m_bCheckAIUseConsiderImageNo;
				build_options.consider_defect_type = input.gt_region->m_AlgorithmParam[input.tab_idx].m_bCheckAIUseConsiderDefectCode;
				_logger->debug("[Director] Preprocessing job {}: Model key options - consider_vision_pos={}, consider_inspection_type={}, consider_image_index={}, consider_defect_type={}",
							   job_counter,
							   build_options.consider_vision_pos,
							   build_options.consider_inspection_type,
							   build_options.consider_image_index,
							   build_options.consider_defect_type);
			}
			else {
				build_options.consider_image_index = false;
				build_options.consider_defect_type = false;
				_logger->warn("[Director] Preprocessing job {}: GTRegion or tab_idx is invalid, defaulting model key options - consider_image_index=false, consider_defect_type=false",
							  job_counter);
			}
			build_options.ignore_case = false;
			build_options.allow_input_duplicates = false;
			build_options.distinct = true;

			std::string camera_type = input.camera_type;
			std::string inspection_type = input.inspection_type;
			std::string image_index_str = std::to_string(input.image_idx);
			std::string defect_type = input.defect_type;

			out_model_key = build_name(camera_type, "", inspection_type, image_index_str, defect_type, build_options);

			_logger->info("[Director] Preprocessing job {}: Stage 5 - Completed. Model key: '{}'",
						  job_counter, out_model_key);

			return true;
		}
		catch (const std::exception& e) {
			out_error_msg = std::string("Exception: ") + e.what();
			out_model_key = "DEFAULT_MODEL";
			_logger->warn("[Director] Preprocessing job {}: Stage 5 exception - {}, using default",
						  job_counter, out_error_msg);
			return true;  // 기본값으로 계속 진행
		}
		catch (...) {
			out_model_key = "DEFAULT_MODEL";
			_logger->warn("[Director] Preprocessing job {}: Stage 5 unknown exception, using default", job_counter);
			return true;  // 기본값으로 계속 진행
		}
	}

	bool ServiceDirector::create_request_(
		uint64_t job_counter,
		const PreprocessingJobInput& input,
		const cv::Mat& ai_image,
		const cv::Mat& mask_binary,
		const cv::Rect& roi_rect,
		const std::string& model_key,
		Request& out_request,
		std::string& out_error_msg)
	{
		try {
			_logger->debug("[Director] Preprocessing job {}: Stage 6 - Create Request Object starting", job_counter);

			std::string inspection_id = input.eq_id;
			std::string log_id = input.lot_id + "-" + input.tray_id + "-" + input.barcode_id + "-" + "_imageNo-" + std::to_string(input.image_idx);
			std::string save_dir;
			if (input.save_dir_list.size() > 0) {
				save_dir = input.save_dir_list[0] + "/ADJImage";
			}
			else {
				std::filesystem::path folder_path = std::filesystem::path("D:\\CMI_Results");
				// 날짜별 폴더 생성
				auto now = std::chrono::system_clock::now();
				auto time_t = std::chrono::system_clock::to_time_t(now);

				// 안전한 localtime_s 사용
				std::tm local_tm;
				localtime_s(&local_tm, &time_t);

				// 연/월/일 형식으로 폴더 경로 생성 (예: 2026\2\26)
				int year = local_tm.tm_year + 1900;
				int month = local_tm.tm_mon + 1;
				int day = local_tm.tm_mday;
				folder_path /= std::to_string(year);
				folder_path /= std::to_string(month);
				folder_path /= std::to_string(day);
				folder_path /= input.lot_id; 				// lot_id 기반 서브폴더 생성
				folder_path /= input.tray_id; 				// tray_id 기반 서브폴더 생성
				folder_path /= "ADJImage"; 					// 최종적으로 ADJImage 폴더 생성
				save_dir = folder_path.string();
			}

			out_request = Request(inspection_id.c_str(), log_id.c_str(), model_key.c_str());
			out_request.set_image(ai_image);
			out_request.set_mask(mask_binary);
			out_request.set_area(roi_rect);
			out_request.set_save_dir(save_dir.c_str());

			// AI 검사 조건 처리 (3개의 Filter)
			out_request.add_inspection_step_enable(input.gt_region->m_AlgorithmParam[input.tab_idx].m_bUseFilter1);
			out_request.add_inspection_step_enable(input.gt_region->m_AlgorithmParam[input.tab_idx].m_bUseFilter2);
			out_request.add_inspection_step_enable(input.gt_region->m_AlgorithmParam[input.tab_idx].m_bUseFilter3);

			_logger->info("[Director] Preprocessing job {}: Stage 6 - Completed. inspection_id='{}', log_id='{}', model_key='{}'",
						  job_counter, inspection_id, log_id, model_key);

			return true;
		}
		catch (const std::exception& e) {
			out_error_msg = std::string("Exception: ") + e.what();
			_logger->error("[Director] Preprocessing job {}: Stage 6 exception - {}", job_counter, out_error_msg);
			return false;
		}
		catch (...) {
			out_error_msg = "Unknown exception";
			_logger->error("[Director] Preprocessing job {}: Stage 6 unknown exception", job_counter);
			return false;
		}
	}

	void ServiceDirector::setup_addon_info_(
		uint64_t job_counter,
		const PreprocessingJobInput& input,
		JobInfoAddon& out_addon)
	{
		try {
			_logger->debug("[Director] Preprocessing job {}: Stage 7 - Setup Addon Info starting", job_counter);

			out_addon.file_name_prefix = input.prefix;
			out_addon.vision_inspection_thread_number = input.buffer_idx;
			out_addon.image_index = input.image_idx - 1;
			out_addon.roi_index = input.roi_idx;
			out_addon.inspection_tab_index = input.tab_idx;
			out_addon.vision_cam_type = input.vision_cam_type;
			out_addon.magazine_no = input.magazine_no;
			out_addon.eq_id = input.eq_id;
			out_addon.lot_id = input.lot_id;
			out_addon.barcode_id = input.barcode_id;

			_logger->info("[Director] Preprocessing job {}: Stage 7 - Completed. thread={}, img={}, roi={}, tab={}",
						  job_counter, out_addon.vision_inspection_thread_number, out_addon.image_index,
						  out_addon.roi_index, out_addon.inspection_tab_index);
		}
		catch (const std::exception& e) {
			_logger->error("[Director] Preprocessing job {}: Stage 7 exception - {}", job_counter, e.what());
		}
		catch (...) {
			_logger->error("[Director] Preprocessing job {}: Stage 7 unknown exception", job_counter);
		}
	}

	// ===== Main add_job with preprocessing =====

	std::future<JobInfo> ServiceDirector::add_job(const PreprocessingJobInput& input)
	{
		auto job_counter = job_counter_++;
		auto response_result = std::make_shared<ResponseResult>();
		auto future_response = response_result->response_job.get_future();

		preprocessing_thread_pool_->enqueue_job([this, job_counter, input, response_result]() {
			try {
				_logger->info("[Director] Preprocessing job {} started: lot_id='{}', image_idx={}, roi_idx={}, tab_idx={}",
							  job_counter, input.lot_id, input.image_idx, input.roi_idx, input.tab_idx);

				// ===== 입력 검증 =====
				if (!input.algorithm || !input.gt_region) {
					_logger->error("[Director] Preprocessing job {}: Invalid input - algorithm or gt_region is null",
								   job_counter);
					response_result->release_with_error("Invalid input parameters");
					return;
				}

				// ===== 1단계: ROI 이미지 전처리 =====
				cv::Mat cv_roi_img, cv_roi_img_v2;
				int roi_x1 = 0, roi_y1 = 0, roi_x2 = 0, roi_y2 = 0;
				int original_width = 0, original_height = 0;
				std::string error_msg;

				if (!preprocess_roi_image_(job_counter, input, cv_roi_img, cv_roi_img_v2,
										   roi_x1, roi_y1, roi_x2, roi_y2,
										   original_width, original_height, error_msg)) {
					_logger->error("[Director] Preprocessing job {}: Stage 1 failed - {}", job_counter, error_msg);
					response_result->release_with_error("Stage 1 failed");
					return;
				}

				// ===== 2단계: AI 검사 이미지 선택 =====
				cv::Mat ai_inspection_image;
				if (!select_ai_inspection_image_(job_counter, input, cv_roi_img, cv_roi_img_v2,
												 ai_inspection_image, error_msg)) {
					_logger->error("[Director] Preprocessing job {}: Stage 2 failed - {}", job_counter, error_msg);
					response_result->release_with_error("Stage 2 failed");
					return;
				}

				// ===== 3단계: 마스크 정보 생성 =====
				cv::Mat mask_binary;
				cv::Rect roi_rect_full;
				if (!create_mask_image_(job_counter, input, ai_inspection_image,
										roi_x1, roi_y1, roi_x2, roi_y2,
										original_width, original_height,
										mask_binary, roi_rect_full, error_msg)) {
					_logger->error("[Director] Preprocessing job {}: Stage 3 failed - {}", job_counter, error_msg);
					response_result->release_with_error("Stage 3 failed");
					return;
				}

				if (!input.enable_inspection_ai) {
					int image_index = input.image_idx - 1;
					int inspection_type = input.gt_region->miInspectionType - 1;
					int defect_type = input.gt_region->m_AlgorithmParam[input.tab_idx].m_iDefectType;
					// 유효한 결함 영역인 경우 처리
					ConcatObj(input.algorithm->m_HDefectRgn_ImageNo[input.buffer_idx][image_index], input.defect_reg, &(input.algorithm->m_HDefectRgn_ImageNo[input.buffer_idx][image_index]));
					ConcatObj(input.algorithm->m_HDefectRgn_DefectName[input.buffer_idx][inspection_type][defect_type], input.defect_reg, &(input.algorithm->m_HDefectRgn_DefectName[input.buffer_idx][inspection_type][defect_type]));
					return;
				}

				// ===== 5단계: AI 모델 Key 생성 =====
				std::string model_key;
				if (!generate_model_key_(job_counter, input, model_key, error_msg)) {
					_logger->error("[Director] Preprocessing job {}: Stage 5 failed - {}", job_counter, error_msg);
					response_result->release_with_error("Stage 5 failed");
					return;
				}

				// ===== 6단계: Request 객체 생성 =====
				Request request("temp", "temp", "temp");
				if (!create_request_(job_counter, input, ai_inspection_image, mask_binary, roi_rect_full,
									 model_key, request, error_msg)) {
					_logger->error("[Director] Preprocessing job {}: Stage 6 failed - {}", job_counter, error_msg);
					response_result->release_with_error("Stage 6 failed");
					return;
				}

				// ===== 7단계: Addon 정보 설정 =====
				JobInfoAddon addon;
				setup_addon_info_(job_counter, input, addon);

				// ===== 8단계: 검사 스레드풀에 작업 제출 =====
				_logger->info("[Director] Preprocessing job {}: Stage 8 - Submitting to inspection thread pool", job_counter);

				auto inspection_future = add_job(request, addon);

				_logger->debug("[Director] Preprocessing job {}: Waiting for inspection to complete...", job_counter);
				auto job_info = inspection_future.get();

				if (!response_result->set_value(std::move(job_info))) {
					_logger->warn("[Director] Preprocessing job {}: Failed to set result", job_counter);
				}
				else {
					_logger->info("[Director] Preprocessing job {}: Completed successfully", job_counter);
				}
			}
			catch (const std::exception& e) {
				_logger->error("[Director] Preprocessing job {}: Exception - {}", job_counter, e.what());
				try {
					response_result->release_with_exception(std::current_exception());
				}
				catch (...) {
					_logger->error("[Director] Preprocessing job {}: Failed to set exception", job_counter);
				}
			}
			catch (...) {
				_logger->error("[Director] Preprocessing job {}: Unknown exception", job_counter);
				try {
					response_result->release_with_error("Unknown exception");
				}
				catch (...) {
					_logger->error("[Director] Preprocessing job {}: Failed to set error", job_counter);
				}
			}
												});

		return future_response;
	}

	std::optional<Response> ServiceDirector::process_image(const Request& request)
	{
		using namespace std::chrono;
		auto start_time = high_resolution_clock::now();
		try {
			auto response = service_operator_->inspection(request);
			auto end_time = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end_time - start_time).count();
			_logger->info("[Director] process_image succeeded: inspection_id='{}', log_id='{}', duration={} ms",
						  request.get_inspection_id(),
						  request.get_log_id(),
						  duration);
			return response;
		}
		catch (const std::exception& e) {
			_logger->error("[Director] process_image exception: inspection_id='{}', log_id='{}', error='{}'",
						   request.get_inspection_id(),
						   request.get_log_id(),
						   e.what());
			return std::nullopt;
		}
		catch (...) {
			_logger->error("[Director] process_image unknown exception: inspection_id='{}', log_id='{}'",
						   request.get_inspection_id(),
						   request.get_log_id());
			return std::nullopt;
		}
	}

	void ServiceDirector::save_train_image_save(const std::string& save_dir,
												const cv::Mat& image,
												const cv::Size& patch_size,
												int stride,
												const std::string& prefix)
	{
		service_operator_->save_train_image_save(save_dir, image, patch_size, stride, prefix);
	}

	void ServiceDirector::save_train_image_save_with_mask(const std::string& save_dir,
														  const cv::Mat& image,
														  const cv::Mat& mask,
														  const cv::Size& patch_size,
														  int stride,
														  const std::string& prefix)
	{
		service_operator_->save_train_image_save_with_mask(save_dir, image, mask, patch_size, stride, prefix);
	}

	std::map<int, int> ServiceDirector::get_manager_initialization_status(const char* model_key)
	{
		return service_operator_->get_manager_initialization_status(model_key);
	}

	std::vector<std::string> ServiceDirector::build_names(const std::vector<std::string>& camera_types,
														  const std::vector<std::string>& vision_positions,
														  const std::vector<std::string>& inspection_types,
														  const std::vector<std::string>& defect_types,
														  const NameBuildOptions& opt)
	{
		std::vector<std::string> cams = sanitize_list_(camera_types,
													   [&](const std::string& s) {
														   if (opt.ignore_case)
															   return starts_with_icase_(s, "ud");
														   return starts_with_(s, "UD");
													   },
													   opt.ignore_case, opt.allow_input_duplicates);

		std::vector<std::string> poss = sanitize_list_(vision_positions,
													   [](const std::string&) { return false; },
													   opt.ignore_case, opt.allow_input_duplicates);

		std::vector<std::string> insps = sanitize_list_(inspection_types,
														[](const std::string&) { return false; },
														opt.ignore_case, opt.allow_input_duplicates);

		std::vector<std::string> defs = sanitize_list_(defect_types,
													   [](const std::string& s) { return is_res_number_(s); },
													   opt.ignore_case, opt.allow_input_duplicates);

		static const std::vector<std::string> dummy_one_empty{ "" };
		const auto& cams_ref = cams.empty() ? dummy_one_empty : cams;
		const auto& poss_ref = poss.empty() ? dummy_one_empty : poss;
		const auto& insps_ref = insps.empty() ? dummy_one_empty : insps;
		const auto& defs_ref = defs.empty() ? dummy_one_empty : defs;

		std::vector<std::string> result;
		result.reserve(cams_ref.size() * poss_ref.size() * insps_ref.size() * defs_ref.size());

		std::unordered_set<std::string> seen_final;
		if (opt.distinct)
			seen_final.reserve(result.capacity());

		for (const auto& cam : cams_ref) {
			for (const auto& pos : poss_ref) {
				for (const auto& insp : insps_ref) {
					for (const auto& def : defs_ref) {
						std::string name;
						name.reserve(cam.size() + pos.size() + insp.size() + def.size() + 3);

						append_part_(name, cam);
						if (opt.consider_vision_pos)      append_part_(name, pos);
						if (opt.consider_inspection_type) append_part_(name, insp);
						if (opt.consider_defect_type)     append_part_(name, def);

						if (name.empty())
							name = "ALL";

						if (!opt.distinct) {
							result.push_back(std::move(name));
						}
						else {
							std::string key = opt.ignore_case ? to_lower_copy_(name) : name;

							if (seen_final.insert(key).second)
								result.push_back(std::move(name));
						}
					}
				}
			}
		}

		return result;
	}

	std::string ServiceDirector::build_name(const std::string& camera_type,
											const std::string& vision_position,
											const std::string& inspection_type,
											const std::string& image_index_str,
											const std::string& defect_type,
											const NameBuildOptions& opt)
	{
		std::string cam = trim_copy_(camera_type);
		const std::string pos = trim_copy_(vision_position);
		const std::string insp = trim_copy_(inspection_type);
		const std::string img_idx = trim_copy_(image_index_str);
		const std::string def0 = trim_copy_(defect_type);

		if (!cam.empty()) {
			const bool is_ud_prefix = opt.ignore_case ? starts_with_icase_(cam, "ud")
				: starts_with_(cam, "UD");
			if (is_ud_prefix)
				cam.clear();
		}

		std::string def = def0;
		if (!def.empty() && is_res_number_(def))
			def.clear();

		std::string name;
		name.reserve(cam.size() + pos.size() + insp.size() + def.size() + 3);

		append_part_(name, cam);

		if (opt.consider_vision_pos && !pos.empty())
			append_part_(name, pos);

		if (opt.consider_inspection_type && !insp.empty())
			append_part_(name, insp);

		if (opt.consider_image_index && !img_idx.empty())
			append_part_(name, img_idx);

		if (opt.consider_defect_type && !def.empty())
			append_part_(name, def);

		if (name.empty())
			name = "ALL";

		return name;
	}

}
