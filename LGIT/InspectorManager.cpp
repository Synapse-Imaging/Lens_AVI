#include "stdafx.h"
#ifdef LGITAI
#include "../uScan.h"
#include "InspectorManager.h"
#include "../AIService/ImageUtile.h"
#undef min;


namespace LGIT {
	InspectorManager::InspectorManager(
		size_t num_threads,
		std::shared_ptr<LAIInspector> inspector1,
		std::shared_ptr<LAIInspector2> inspector2,
		CADJClientService& inspector3,
		std::shared_ptr<LAIInspectorSVM> inspectorSVM,
		std::shared_ptr<LAIInspectorSEG> inspectorSEG,
		std::shared_ptr<LAIInspectorVAL> inspectorVAL,
		int logging_level,
		bool is_use_segmentation_visualization,
		std::filesystem::path log_root)
		: primary_inspector_(std::move(inspector1)),
		secondary_inspector_(std::move(inspector2)),
		thirdly_inspector_(inspector3),
		svm_inspector_(std::move(inspectorSVM)),
		seg_inspector_(std::move(inspectorSEG)),
		val_inspector_(std::move(inspectorVAL)),
		is_use_segmentation_visualization_(is_use_segmentation_visualization)
	{
		std::string logger_name = "inspector_manager_logger";
		std::filesystem::path log_folder = log_root / logger_name;

		try {
			std::filesystem::create_directories(log_folder);
		}
		catch (const std::exception& e) {
			std::cerr << "Failed to create log directory: " << e.what() << '\n';
		}

		// 이미 존재하는 로거 가져오기
		auto logger = spdlog::get(logger_name);
		if (logger) {
			_logger = logger;
			return;
		}
		// 비동기 로깅 생성
		if (!spdlog::thread_pool()) {
			spdlog::init_thread_pool(8192, 1); // queue size, threads
		}
		std::string log_file = "InspectorManagerLog_" + get_current_date_string() + ".txt"; // 파일 싱크 설정

		std::filesystem::path full_log_path = log_folder / log_file;
		_logger = spdlog::basic_logger_mt<spdlog::async_factory>(logger_name, full_log_path.string());
		_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v"); // 포맷 커스터마이징
		_logger->set_level(spdlog::level::debug);
		_logger->flush_on(spdlog::level::info); // level 설정

		_logger->info("InspectorManager: num_threads={}, logging_level={}",
			num_threads,
			logging_level);

		thread_pool_ = std::make_shared<AIService::ThreadPool>();
		thread_pool_->init(num_threads);
		save_image_thread_pool_ = std::make_shared<AIService::ThreadPool>();
		save_image_thread_pool_->init(num_threads*2);

		secondary_watchdog_timer_ = std::make_shared<AIService::WatchdogTimer>(
			30000,
			[this]() { this->test_secondary_inspection_with_random_data(); }
		);
		secondary_watchdog_timer_->start();
	}

	std::future<InspectionResultSet> InspectorManager::submit(
		std::shared_ptr<InspectionRequest> request,
		uint64_t& out_job_id)
	{
		// Model reload 진행 중이면 대기

		wait_for_model_reload_completion();

		uint64_t job_id = job_id_counter_++;
		out_job_id = job_id;
		request->job_id = job_id;

		auto job_info = std::make_shared<JobInfo>();
		job_info->request = std::move(request);
		std::future<InspectionResultSet> future = job_info->promise.get_future();

		thread_pool_->enqueue_job([this, job_id, job_info]() {
			InspectionResultSet result_set;
			result_set.request = job_info->request;

			try {
				using namespace std::chrono;
				const auto& req = job_info->request;
				auto t0_total = high_resolution_clock::now();

				std::ostringstream oss;
				oss << std::this_thread::get_id();
				std::string thread_id_str = oss.str();

				std::string defect_type_str = secondary_inspector_->_condition_short_name_list[req->roi_inspection_type_idx];

				_logger->info("[job_id={}] START submit: vision_pos={}, tray_no={}, module_no={}, image_no={}, roi_no={}, tab_idx={}, defect_type={}, thread_id={}",
					job_id,
					req->vision_cam,
					req->tray_no,
					req->module_no,
					req->image_idx,
					req->roi_id,
					req->inspection_tab_idx,
					defect_type_str,
					thread_id_str);

				job_info->status = JobStatus::Running;
				cv::Mat primary_mask;

				// STEP 1: 1차 검사 또는 Region 변환
				auto t1_start = high_resolution_clock::now();
				_logger->info("[job_id={}] Step1: Start Primary - use_inspection={}", job_id, req->primary_inspect_enable);
				_logger->info("[job_id={}] Step1: Inspect AVI time(temp) - function_time={}ms", job_id, req->inspect_avi_ms);

				if (req->primary_inspect_enable) {
					auto step1_func_start = high_resolution_clock::now();
					auto result = handle_primary_inspection(job_info);
					auto step1_func_end = high_resolution_clock::now();
					primary_mask = result.mask.clone();
					result_set.primary_result = std::move(result);
					_logger->info("[job_id={}] Step1: Primary Inspection done - result={}, mask_valid={}, function_time={}ms", job_id, result.result, !primary_mask.empty(), duration_cast<milliseconds>(step1_func_end - step1_func_start).count());
				}
				else {
					auto step1_func_start = high_resolution_clock::now();
					auto result = handle_primary_mask_from_region(job_info);
					auto step1_func_end = high_resolution_clock::now();
					primary_mask = result.mask.clone();
					result_set.primary_result = std::move(result);
					_logger->info("[job_id={}] Step1: Region to Mask done - result={}, mask_valid={}, function_time={}ms", job_id, result.result, !primary_mask.empty(), duration_cast<milliseconds>(step1_func_end - step1_func_start).count());
				}
				auto t1_end = high_resolution_clock::now();
				_logger->info("[job_id={}] Step1: Time Elapsed = {}ms", job_id, duration_cast<milliseconds>(t1_end - t1_start).count());

				save_primary_mask_image(primary_mask, job_info);
				_logger->info("[job_id={}] Step1: Primary mask image saved (empty={}, total_sum={})",
					job_id, primary_mask.empty(), cv::sum(primary_mask)[0]);

				if (!req->primary_inspect_enable && req->is_use_val) {
					auto val_func_start = high_resolution_clock::now();
					auto val_result = handle_val_inspection(job_info);
					auto val_func_end = high_resolution_clock::now();

					if (val_result.result) {
						primary_mask = val_result.mask.clone();
						save_val_mask_image(primary_mask, job_info);
						_logger->info("[job_id={}] Step1: Primary mask image saved (empty={}, total_sum={})",
							job_id, primary_mask.empty(), cv::sum(primary_mask)[0]);
					}

					_logger->info("[job_id={}] StepVAL: Inspection done - result={}, mask_valid={}, function_time={}ms", job_id, val_result.result, !primary_mask.empty(), duration_cast<milliseconds>(val_func_end - val_func_start).count());
				}
				

				// 양품 처리 되었는지 확인후 결과 리턴
				if (!AIService::ImageUtile::HasImageData(primary_mask)) {
					_logger->info("[job_id={}] Step1: Pass", job_id);
					result_set.inspect = job_info->inspect;
					job_info->inspect = JobStatus::Completed;
					job_info->status = JobStatus::Completed;
					job_info->promise.set_value(std::move(result_set));

					auto total_time = duration_cast<milliseconds>(high_resolution_clock::now() - t0_total).count();
					_logger->info("[job_id={}] COMPLETED: vision_pos={}, tray_no={}, module_no={}, image_no={}, roi_no={}, tab_idx={}, defect_type={}, total_time={}ms, thread_id={}",
						job_id,
						req->vision_cam,
						req->tray_no,
						req->module_no,
						req->image_idx,
						req->roi_id,
						req->inspection_tab_idx,
						defect_type_str,
						total_time,
						thread_id_str);
					return;
				}

				// req->svm_inspect_enable = true;
				// STEP SVM: svm Inspection
				auto svm_start = high_resolution_clock::now();
				if (req->svm_inspect_enable && AIService::ImageUtile::HasImageData(primary_mask)) {
					_logger->info("[job_id={}] StepS: Start Secondary Inspection", job_id);

					auto step_svm_func_start = high_resolution_clock::now();
					auto result_svm = handle_svm_inspection(primary_mask, job_info);
					auto step_svm_func_end = high_resolution_clock::now();

					result_set.svm_result = std::move(result_svm);
					_logger->info("[job_id={}] StepS: Secondary done - blob_count={}, result={}, function_time={}ms",
						job_id, result_svm.blob_count, result_svm.result,
						duration_cast<milliseconds>(step_svm_func_end - step_svm_func_start).count());
				}
				else {
					_logger->info("[job_id={}] StepS: Skipped - condition not met", job_id);
				}
				auto svm_end = high_resolution_clock::now();
				_logger->info("[job_id={}] StepS: Time Elapsed = {}ms", job_id, duration_cast<milliseconds>(svm_end - svm_start).count());


				if (result_set.svm_result.has_value() && result_set.svm_result->result) {
					if (result_set.svm_result->is_ng()) {
						primary_mask = AIService::ImageUtile::LabelConnectedComponents(result_set.svm_result->blob_ng_mask);
					}
					else {
						_logger->info("[job_id={}] StepS: Pass", job_id);
						result_set.inspect = job_info->inspect;
						job_info->inspect = JobStatus::Completed;
						job_info->status = JobStatus::Completed;
						job_info->promise.set_value(std::move(result_set));

						auto total_time = duration_cast<milliseconds>(high_resolution_clock::now() - t0_total).count();
						_logger->info("[job_id={}] COMPLETED: vision_pos={}, tray_no={}, module_no={}, image_no={}, roi_no={}, tab_idx={}, defect_type={}, total_time={}ms, thread_id={}",
							job_id,
							req->vision_cam,
							req->tray_no,
							req->module_no,
							req->image_idx,
							req->roi_id,
							req->inspection_tab_idx,
							defect_type_str,
							total_time,
							thread_id_str);
						return;
					}
				}

				// STEP 2: Secondary Inspection
				auto t2_start = high_resolution_clock::now();
				if (req->secondary_inspect_enable && !primary_mask.empty() && cv::sum(primary_mask)[0] > 0) {
					_logger->info("[job_id={}] Step2: Start Secondary Inspection", job_id);
					// Model::LAIInspector2Result result2;
					auto step2_func_start = high_resolution_clock::now();
					auto result2 = handle_secondary_inspection(primary_mask, job_info);
					auto step2_func_end = high_resolution_clock::now();
					result_set.secondary_result = std::move(result2);
					_logger->info("[job_id={}] Step2: Secondary done - blob_count={}, result={}, function_time={}ms",
						job_id, result2.blob_count, result2.result,
						duration_cast<milliseconds>(step2_func_end - step2_func_start).count());
				}
				else {
					_logger->info("[job_id={}] Step2: Skipped - condition not met", job_id);
				}
				auto t2_end = high_resolution_clock::now();
				_logger->info("[job_id={}] Step2: Time Elapsed = {}ms", job_id, duration_cast<milliseconds>(t2_end - t2_start).count());

				// 양품 처리 되었는지 확인
				if (result_set.secondary_result.has_value() &&
					!result_set.secondary_result->is_ng()) 
				{
					_logger->info("[job_id={}] Step2: Pass", job_id);
					result_set.inspect = job_info->inspect;
					job_info->inspect = JobStatus::Completed;
					job_info->status = JobStatus::Completed;
					job_info->promise.set_value(std::move(result_set));

					auto total_time = duration_cast<milliseconds>(high_resolution_clock::now() - t0_total).count();
					_logger->info("[job_id={}] COMPLETED: vision_pos={}, tray_no={}, module_no={}, image_no={}, roi_no={}, tab_idx={}, defect_type={}, total_time={}ms, thread_id={}",
						job_id,
						req->vision_cam,
						req->tray_no,
						req->module_no,
						req->image_idx,
						req->roi_id,
						req->inspection_tab_idx,
						defect_type_str,
						total_time,
						thread_id_str);
					return;
				}

				// STEP 3: Tertiary Inspection
				auto t3_start = high_resolution_clock::now();
				if (req->tertiary_inspect_enable && !job_info->request->is_use_local_segmentation) {
					_logger->info("[job_id={}] Step3: Start Online Tertiary Inspection", job_id);
					if (result_set.request->secondary_inspect_enable && result_set.secondary_result.has_value() && result_set.secondary_result->result && result_set.secondary_result->blob_mask_map.empty()) {
						_logger->info("[job_id={}] Step3: Skipped - tertiary not enabled", job_id);
					}
					else if (result_set.request->secondary_inspect_enable && result_set.secondary_result.has_value() && result_set.secondary_result->result) {
						auto step3_func_start = high_resolution_clock::now();
						_logger->info("[job_id={}] Step3: Start Tertiary Inspection", job_id);
						auto result3 = handle_tertiary_inspection(primary_mask, result_set.secondary_result.value(), job_info);
						result_set.tertiary_result = std::move(result3);
						_logger->info("[job_id={}] Step3: Tertiary done - used secondary", job_id);
						auto step3_func_end = high_resolution_clock::now();
						_logger->info("[job_id={}] Step3: Tertiary function_time={}ms", job_id,
							duration_cast<milliseconds>(step3_func_end - step3_func_start).count());
					}
					else {
						auto step3_func_start = high_resolution_clock::now();
						_logger->info("[job_id={}] Step3: Start Tertiary Inspection", job_id);
						auto result3 = handle_tertiary_inspection(primary_mask, job_info);
						result_set.tertiary_result = std::move(result3);
						_logger->info("[job_id={}] Step3: Tertiary done - used primary mask", job_id);
						auto step3_func_end = high_resolution_clock::now();
						_logger->info("[job_id={}] Step3: Tertiary function_time={}ms", job_id,
							duration_cast<milliseconds>(step3_func_end - step3_func_start).count());
					}
				}
				else if (req->tertiary_inspect_enable && job_info->request->is_use_local_segmentation) {
					_logger->info("[job_id={}] Step3: Start Local Tertiary Inspection", job_id);
					if (result_set.request->secondary_inspect_enable && result_set.secondary_result.has_value() && result_set.secondary_result->result && result_set.secondary_result->blob_mask_map.empty()) {
						_logger->info("[job_id={}] Step3: Skipped - tertiary not enabled", job_id);
					}
					else if (result_set.request->secondary_inspect_enable && result_set.secondary_result.has_value() && result_set.secondary_result->result) {
						auto step3_func_start = high_resolution_clock::now();
						_logger->info("[job_id={}] Step3: Start Tertiary Inspection", job_id);
						auto result3 = handle_tertiary_inspection_local(primary_mask, result_set.secondary_result.value(), job_info);
						result_set.tertiary_result = std::move(result3);
						_logger->info("[job_id={}] Step3: Tertiary done - used secondary", job_id);
						auto step3_func_end = high_resolution_clock::now();
						_logger->info("[job_id={}] Step3: Tertiary function_time={}ms", job_id,
							duration_cast<milliseconds>(step3_func_end - step3_func_start).count());
					}
					else {
						auto step3_func_start = high_resolution_clock::now();
						_logger->info("[job_id={}] Step3: Start Tertiary Inspection", job_id);
						auto result3 = handle_tertiary_inspection_local(primary_mask, job_info);
						result_set.tertiary_result = std::move(result3);
						_logger->info("[job_id={}] Step3: Tertiary done - used primary mask", job_id);
						auto step3_func_end = high_resolution_clock::now();
						_logger->info("[job_id={}] Step3: Tertiary function_time={}ms", job_id,
							duration_cast<milliseconds>(step3_func_end - step3_func_start).count());
					}
				}
				else {
					_logger->info("[job_id={}] Step3: Skipped - tertiary not enabled", job_id);
				}
				auto t3_end = high_resolution_clock::now();
				_logger->info("[job_id={}] Step3: Time Elapsed = {}ms", job_id, duration_cast<milliseconds>(t3_end - t3_start).count());

				result_set.inspect = job_info->inspect;
				job_info->inspect = JobStatus::Completed;
				job_info->status = JobStatus::Completed;
				job_info->promise.set_value(std::move(result_set));

				auto total_time = duration_cast<milliseconds>(high_resolution_clock::now() - t0_total).count();
				_logger->info("[job_id={}] COMPLETED: vision_pos={}, tray_no={}, module_no={}, image_no={}, roi_no={}, tab_idx={}, defect_type={}, total_time={}ms, thread_id={}",
					job_id,
					req->vision_cam,
					req->tray_no,
					req->module_no,
					req->image_idx,
					req->roi_id,
					req->inspection_tab_idx,
					defect_type_str,
					total_time,
					thread_id_str);
			}
			catch (const std::exception& e) {
				result_set.inspect = job_info->inspect;
				job_info->status = JobStatus::Failed;
				job_info->error.store(job_info->inspect.load());
				job_info->promise.set_value(std::move(result_set));

				_logger->error("[job_id={}] submit: FAILED with exception - {}, last_step={}",
					job_id, e.what(), static_cast<int>(job_info->inspect.load()));
			}
			catch (...) {
				//job_info->status = JobStatus::Failed;
				//job_info->error.store(job_info->inspect.load());
				//job_info->promise.set_exception(std::current_exception());
				result_set.inspect = job_info->inspect;
				job_info->status = JobStatus::Failed;
				job_info->error.store(job_info->inspect.load());
				job_info->promise.set_value(std::move(result_set));

				_logger->error("[job_id={}] submit: FAILED with unknown exception, last_step={}",
					job_id, static_cast<int>(job_info->inspect.load()));
			}
		});

		return future;
	}

	bool InspectorManager::try_model_reload() 
	{
		_logger->info("Model reload 요청 시작");

		// 1. 이미 진행 중인 model reload가 있는지 확인
		{
			std::unique_lock<std::mutex> lock(model_reload_mutex_);
			if (is_model_reloading_.load()) {
				_logger->warn("이미 model reload가 진행 중입니다 - 완료까지 대기");
				// 이미 진행 중인 reload 완료 대기
				model_reload_cv_.wait(lock, [this] { 
					return !is_model_reloading_.load(); 
				});
				_logger->info("진행 중이던 model reload 완료됨");
				return true; // 이미 완료된 reload로 간주
			}
			is_model_reloading_.store(true);
		}

		_logger->info("Model reload 시작 - 조건 확인 및 준비");

		// 2. WatchdogTimer 일시정지 (항상 먼저 실행)
		if (secondary_watchdog_timer_) {
			if (!secondary_watchdog_timer_->is_paused()) {
				secondary_watchdog_timer_->pause();
				_logger->info("WatchdogTimer 일시정지 완료");
			} else {
				_logger->info("WatchdogTimer 이미 일시정지 상태");
			}
		}

		bool reload_success = true;

		try {
			// 3. ThreadPool의 모든 작업이 완료될 때까지 적극적으로 대기
			_logger->info("ThreadPool 작업 완료 대기 시작 - 현재 작업: {} 개", thread_pool_->get_job_count());
			const int max_wait_seconds = 60; // 대기 시간을 60초로 증가
			int wait_count = 0;
			int last_job_count = -1;
			
			while (!thread_pool_->is_job_empty() && wait_count < max_wait_seconds * 10) {
				int current_job_count = thread_pool_->get_job_count();
				
				// 작업 수가 변경되었거나 주기적으로 로그 출력
				if (current_job_count != last_job_count || wait_count % 50 == 0) {
					_logger->info("ThreadPool 작업 대기 중... 남은 작업: {} 개, 대기 시간: {:.1f}초", 
						current_job_count, wait_count * 0.1);
					last_job_count = current_job_count;
				}
				
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				wait_count++;
			}

			if (!thread_pool_->is_job_empty()) {
				int remaining_jobs = thread_pool_->get_job_count();
				_logger->warn("ThreadPool 작업 완료 대기 시간 초과 ({}초) - 남은 작업: {} 개", 
					max_wait_seconds, remaining_jobs);
				_logger->warn("남은 작업이 있지만 model reload를 강제로 진행합니다");
			} else {
				_logger->info("ThreadPool 모든 작업 완료 확인");
			}

			// 4. save_image_thread_pool도 확인하고 대기
			_logger->info("이미지 저장 ThreadPool 작업 완료 대기 - 현재 작업: {} 개", save_image_thread_pool_->get_job_count());
			wait_count = 0;
			last_job_count = -1;
			
			while (!save_image_thread_pool_->is_job_empty() && wait_count < max_wait_seconds * 10) {
				int current_job_count = save_image_thread_pool_->get_job_count();
				
				if (current_job_count != last_job_count || wait_count % 50 == 0) {
					_logger->info("이미지 저장 ThreadPool 작업 대기 중... 남은 작업: {} 개, 대기 시간: {:.1f}초", 
						current_job_count, wait_count * 0.1);
					last_job_count = current_job_count;
				}
				
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				wait_count++;
			}

			if (!save_image_thread_pool_->is_job_empty()) {
				int remaining_jobs = save_image_thread_pool_->get_job_count();
				_logger->warn("이미지 저장 ThreadPool 작업 완료 대기 시간 초과 - 남은 작업: {} 개", remaining_jobs);
			} else {
				_logger->info("이미지 저장 ThreadPool 모든 작업 완료 확인");
			}

			// 5. 각 검사기 모델 재로드 수행
			_logger->info("검사기 모델 재로드 시작");

			// 2차 검사기 모델 재로드
			if (secondary_inspector_) {
				try {
					_logger->info("2차 검사기 모델 재로드 시작...");
					if (secondary_inspector_->try_model_reload()) { 
						_logger->info("2차 검사기 모델 재로드 성공"); 
					}
					else { 
						_logger->error("2차 검사기 모델 재로드 실패");
						reload_success = false;
					}
				}
				catch (const std::exception& e) {
					_logger->error("2차 검사기 모델 재로드 중 예외 발생: {}", e.what());
					reload_success = false;
				}
			}

			// seg 검사기 모델 재로드
			if (seg_inspector_) {
				try {
					_logger->info("SEG 검사기 모델 재로드 시작...");
					if (seg_inspector_->try_model_reload()) { 
						_logger->info("SEG 검사기 모델 재로드 성공"); 
					}
					else { 
						_logger->error("SEG 검사기 모델 재로드 실패");
						reload_success = false;
					}
				}
				catch (const std::exception& e) {
					_logger->error("SEG 검사기 모델 재로드 중 예외 발생: {}", e.what());
					reload_success = false;
				}
			}

			// svm 검사기 모델 재로드
			if (svm_inspector_) {
				try {
					_logger->info("SVM 검사기 모델 재로드 시작...");
					if (svm_inspector_->try_model_reload()) { 
						_logger->info("SVM 검사기 모델 재로드 성공"); 
					}
					else { 
						_logger->error("SVM 검사기 모델 재로드 실패");
						reload_success = false;
					}
				}
				catch (const std::exception& e) {
					_logger->error("SVM 검사기 모델 재로드 중 예외 발생: {}", e.what());
					reload_success = false;
				}
			}

			// val 검사기 모델 재로드
			if (val_inspector_) {
				try {
					_logger->info("VAL 검사기 모델 재로드 시작...");
					if (val_inspector_->try_model_reload()) { 
						_logger->info("VAL 검사기 모델 재로드 성공"); 
					}
					else { 
						_logger->error("VAL 검사기 모델 재로드 실패");
						reload_success = false;
					}
				}
				catch (const std::exception& e) {
					_logger->error("VAL 검사기 모델 재로드 중 예외 발생: {}", e.what());
					reload_success = false;
				}
			}
		}
		catch (const std::exception& e) {
			_logger->error("Model reload 중 예외 발생: {}", e.what());
			reload_success = false;
		}
		catch (...) {
			_logger->error("Model reload 중 알 수 없는 예외 발생");
			reload_success = false;
		}

		// 6. WatchdogTimer 재시작
		if (secondary_watchdog_timer_) {
			secondary_watchdog_timer_->resume();
			_logger->info("WatchdogTimer 재시작 완료");
		}

		// 7. Model reload 완료 플래그 해제 및 대기 중인 submit 요청들에게 알림
		{
			std::unique_lock<std::mutex> lock(model_reload_mutex_);
			is_model_reloading_.store(false);
		}
		model_reload_cv_.notify_all();

		if (reload_success) {
			_logger->info("Model reload 완료 - 모든 검사기 모델 재로드 성공");
		} else {
			_logger->error("Model reload 완료 - 일부 검사기 모델 재로드 실패");
		}

		return reload_success;
	}

	void InspectorManager::wait_for_model_reload_completion() {
		std::unique_lock<std::mutex> lock(model_reload_mutex_);
		if (is_model_reloading_.load()) {
			_logger->info("Model reload 진행 중 - submit 요청 대기");
			model_reload_cv_.wait(lock, [this] { 
				return !is_model_reloading_.load(); 
			});
			_logger->info("Model reload 완료 - submit 요청 재개");
		}
	}

	Model::LAIInspectorResult InspectorManager::handle_primary_inspection(const std::shared_ptr<JobInfo>& job_info)
	{
		auto result = primary_inspector_->add_job(
			job_info->request->product_name,
			job_info->request->vision_cam,
			job_info->request->tray_no,
			job_info->request->module_no,
			job_info->request->image_idx,
			job_info->request->roi_id,
			job_info->request->roi_inspection_type_idx,
			job_info->request->image,
			job_info->request->test_mode).get();

		job_info->inspect = JobStatus::Step1_Done;
		job_info->status = JobStatus::Step1_Done;
		return result;
	}

	Model::LAIInspectorResult InspectorManager::handle_primary_mask_from_region(const std::shared_ptr<JobInfo>& job_info)
	{
		job_info->status = JobStatus::Step1_Preprocessing;

		const auto& region = job_info->request->vision_defect_region;

		// 유효 Region 여부 확인
		if (!AIService::ImageUtile::ValidHRegion(region)) {
			_logger->warn("유효하지 않은 Region이 입력됨.");
			return {};
		}

		try {
			// Region → 라벨링된 Halcon 이미지로 변환
			auto halcon_labeled_mask_image = AIService::ImageUtile::RegionToLabeledImage(
				region,
				job_info->request->org_image_width,
				job_info->request->org_image_height);

			// ROI 범위 검증
			const auto& roi = job_info->request->roi_bbox;
			if (roi.right >= job_info->request->org_image_width ||
				roi.bottom >= job_info->request->org_image_height) {
				throw std::runtime_error("ROI 범위가 이미지 영역을 벗어났습니다.");
			}

			// ROI로 Halcon 이미지 자르기
			CropRectangle1(halcon_labeled_mask_image, &halcon_labeled_mask_image,
				roi.top, roi.left, roi.bottom, roi.right);

			// Halcon 이미지 → OpenCV 변환
			cv::Mat mat_labeled_mask = AIService::ImageUtile::HImage2Mat(halcon_labeled_mask_image);

			// ROi 와 이미지 사이즈 조정
			if (job_info->request->image.rows != mat_labeled_mask.rows ||
				job_info->request->image.cols != mat_labeled_mask.cols) {

				cv::resize(
					mat_labeled_mask,
					mat_labeled_mask,
					job_info->request->image.size(),
					0, 0,
					cv::INTER_NEAREST  // 선명함 유지, 마스크/라벨에 적합
				);
			}

			// 리소스 해제
			halcon_labeled_mask_image.Clear();

			// 결과 추가 및 상태 설정
			job_info->inspect = JobStatus::Step1_Done;
			job_info->status = JobStatus::Step1_Done;
			return Model::LAIInspectorResult{ true, mat_labeled_mask, {} };
		}
		catch (const std::exception& e) {
			_logger->error("handle_primary_mask_from_region 실패: {}", e.what());
		}
		catch (...) {
			_logger->error("handle_primary_mask_from_region 예외 발생 (알 수 없음)");
		}

		// 실패 시 빈 결과 처리
		job_info->error = JobStatus::Step1_Preprocessing;
		return {};
	}

	void InspectorManager::save_primary_mask_image(const cv::Mat& mask, const std::shared_ptr<JobInfo>& job_info)
	{
		if (!AIService::ImageUtile::HasImageData(mask)) {
			return;
		}

		std::ostringstream dir_stream;
		dir_stream << job_info->request->save_path << "\\MaskImage1";

		std::filesystem::path dir_path(dir_stream.str());
		std::filesystem::create_directories(dir_path);

		std::ostringstream filename;
		filename
			<< job_info->request->equip_no
			<< "_" << job_info->request->product_name
			<< "_" << job_info->request->lot_id
			<< "_Tray" << job_info->request->tray_no
			<< "_Module" << job_info->request->module_no
			<< "_" << job_info->request->vision_name
			<< "" << job_info->request->pc_idx
			<< "_Stage" << job_info->request->stage_idx
			<< "_Image" << job_info->request->image_idx
			<< "_ROI" << job_info->request->roi_id
			<< "_" << job_info->request->inspectiontype_name
			<< "_Tab" << job_info->request->inspection_tab_idx + 1
			<< "_" << job_info->request->barcode
			<< "_" << job_info->request->local_align_angle_string
			<< "_" << job_info->request->channel_type
			<< "_Mask1"
			<< ".png";

		std::filesystem::path full_path = dir_path / filename.str();

		cv::Mat binary; // 단순 이진화
		cv::threshold(mask, binary, 0, 255, cv::THRESH_BINARY); // 0보다 크면 255, 아니면 0으로 만드는 이진 마스크 생성
		save_image_thread_pool_->enqueue_job_no_return(
			[](const std::string& path, const cv::Mat& img) {
				cv::imwrite(path, img); // bool 리턴값을 무시
			},
			full_path.string(), std::move(binary));
	}

	void InspectorManager::save_val_mask_image(const cv::Mat& mask, const std::shared_ptr<JobInfo>& job_info)
	{
		if (!AIService::ImageUtile::HasImageData(mask)) {
			return;
		}

		std::ostringstream dir_stream;
		dir_stream << job_info->request->save_path << "\\MaskImage1Val";

		std::filesystem::path dir_path(dir_stream.str());
		std::filesystem::create_directories(dir_path);

		std::ostringstream filename;
		filename
			<< job_info->request->equip_no
			<< "_" << job_info->request->product_name
			<< "_" << job_info->request->lot_id
			<< "_Tray" << job_info->request->tray_no
			<< "_Module" << job_info->request->module_no
			<< "_" << job_info->request->vision_name
			<< "" << job_info->request->pc_idx
			<< "_Stage" << job_info->request->stage_idx
			<< "_Image" << job_info->request->image_idx
			<< "_ROI" << job_info->request->roi_id
			<< "_" << job_info->request->inspectiontype_name
			<< "_Tab" << job_info->request->inspection_tab_idx + 1
			<< "_" << job_info->request->barcode
			<< "_" << job_info->request->local_align_angle_string
			<< "_" << job_info->request->channel_type
			<< "_Mask1_Val"
			<< ".png";

		std::filesystem::path full_path = dir_path / filename.str();

		cv::Mat binary; // 단순 이진화
		cv::threshold(mask, binary, 0, 255, cv::THRESH_BINARY); // 0보다 크면 255, 아니면 0으로 만드는 이진 마스크 생성
		save_image_thread_pool_->enqueue_job_no_return(
			[](const std::string& path, const cv::Mat& img) {
				cv::imwrite(path, img); // bool 리턴값을 무시
			},
			full_path.string(), std::move(binary));
	}

	Model::LAIInspectorResult InspectorManager::handle_val_inspection(const std::shared_ptr<JobInfo>& job_info)
	{
		//std::ostringstream dir_stream;
		//dir_stream << job_info->request->save_path << "\\TEMP";

		//std::filesystem::path dir_path(dir_stream.str());
		//std::filesystem::create_directories(dir_path);

		//std::ostringstream filename;
		//filename
		//	<< "" << job_info->request->equip_no
		//	<< "_" << job_info->request->product_name
		//	<< "_" << job_info->request->lot_id
		//	<< "_Tray" << job_info->request->tray_no
		//	<< "_Module" << job_info->request->module_no
		//	<< "_" << job_info->request->vision_name
		//	<< "" << job_info->request->pc_idx
		//	<< "_Stage" << job_info->request->stage_idx
		//	<< "_Image" << job_info->request->image_idx
		//	<< "_ROI" << job_info->request->roi_id
		//	<< "_" << job_info->request->inspectiontype_name
		//	<< "_Tab" << job_info->request->inspection_tab_idx + 1
		//	<< "_" << job_info->request->barcode
		//	<< "_" << job_info->request->local_align_angle_string
		//	<< "_" << job_info->request->channel_type
		//	<< "_VAL" << ".png";

		//cv::Mat save_image = job_info->request->image.clone();
		//std::filesystem::path full_path = dir_path / filename.str();
		//cv::imwrite(full_path.string(), save_image);

		job_info->status = JobStatus::Step_val_Preprocessing;
		const auto& req = job_info->request;
		auto result = val_inspector_->add_job(
			req->product_name,
			req->lot_id,
			req->barcode,
			req->vision_cam,
			req->tray_no,
			req->module_no,
			req->image_idx,
			req->roi_id,
			req->inspection_tab_idx,
			req->roi_inspection_type_idx,
			req->local_align_angle,
			req->defect_name,
			req->image).get();
		return result;
	}

	Model::LAIInspector2Result InspectorManager::handle_secondary_inspection(
		const cv::Mat& primary_mask,
		const std::shared_ptr<JobInfo>& job_info)
	{
		const auto& req = job_info->request;
		const uint64_t job_id = job_info->request->job_id; // JobInfo 또는 Request에 job_id 저장된 것으로 가정
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		std::string thread_id_str = oss.str();

		std::string defect_type_str = secondary_inspector_->_condition_short_name_list[req->roi_inspection_type_idx];

		_logger->info("[job_id={}] Step2: Running secondary inspection - vision_pos={}, tray_no={}, module_no={}, image_no={}, roi_no={}, defect_type={}, thread_id={}",
			job_id,
			req->vision_cam,
			req->tray_no,
			req->module_no,
			req->image_idx,
			req->roi_id,
			defect_type_str,
			thread_id_str);

		auto t_start = std::chrono::high_resolution_clock::now();

		auto result2 = secondary_inspector_->add_job(
			req->product_name,
			req->lot_id,
			req->barcode,
			req->vision_cam,
			req->tray_no,
			req->module_no,
			req->image_idx,
			req->roi_id,
			req->inspection_tab_idx,
			req->roi_inspection_type_idx,
			req->defect_name,
			req->roi_image,
			primary_mask).get();

		job_info->status = JobStatus::Step2_Preprocessing;

		const auto& blob_ids = result2.blob_id_list;
		const auto& class_indices = result2.class_idx_list;
		auto& blob_mask_map = result2.blob_mask_map;
		auto& blob_ok_mask = result2.blob_ok_mask;

		for (size_t i = 0; i < blob_ids.size(); ++i)
		{
			const int class_idx = class_indices[i];
			try {
				cv::Mat find_mask = AIService::ImageUtile::ExtractSpecificValue(primary_mask, static_cast<uchar>(blob_ids[i]));

				if (!AIService::ImageUtile::HasImageData(find_mask)) {
					_logger->warn("[job_id={}] Step2: Empty mask found for class_idx={}", job_id, class_idx);
					continue;
				}

				if (class_idx < 0) {
					if (blob_ok_mask.empty()) {
						blob_ok_mask = std::move(find_mask);
					}
					else {
						if (blob_ok_mask.size() != find_mask.size() || blob_ok_mask.type() != find_mask.type()) {
							_logger->error("[job_id={}] Step2: Mask size/type mismatch for class_idx={}", job_id, class_idx);
							continue;
						}
						cv::bitwise_or(blob_ok_mask, find_mask, blob_ok_mask);
					}
				}
				else {
					auto it = blob_mask_map.find(class_idx);
					if (it != blob_mask_map.end()) {
						cv::Mat& existing_mask = it->second;

						if (existing_mask.empty()) {
							existing_mask = std::move(find_mask);
						}
						else {
							if (existing_mask.size() != find_mask.size() || existing_mask.type() != find_mask.type()) {
								_logger->error("[job_id={}] Step2: Mask size/type mismatch for class_idx={}", job_id, class_idx);
								continue;
							}
							cv::bitwise_or(existing_mask, find_mask, existing_mask);
						}
					}
					else {
						blob_mask_map[class_idx] = std::move(find_mask);
					}
				}
			}
			catch (const std::exception& e) {
				_logger->error("[job_id={}] Step2: Exception while processing class_idx={} - {}", job_id, class_idx, e.what());
			}
			catch (...) {
				_logger->error("[job_id={}] Step2: Unknown exception while processing class_idx={}", job_id, class_idx);
			}
		}

		if (result2.result) {
			secondary_watchdog_timer_->reset(); // 2차 검사 성공 시 타이머 리셋
			job_info->inspect = JobStatus::Step2_Done;
		}
		job_info->status = JobStatus::Step2_Done;

		save_secondary_masks(result2.blob_mask_map, job_info);

		auto t_end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();

		_logger->info("[job_id={}] Step2: Secondary inspection completed - blob_count={}, result={}, elapsed={}ms",
			job_id,
			result2.blob_count,
			result2.result,
			elapsed);

		return result2;
	}

	void InspectorManager::save_secondary_masks(
		const std::map<int, cv::Mat>& blob_mask_list,
		const std::shared_ptr<JobInfo>& job_info)
	{
		if (blob_mask_list.empty())
			return;

		std::ostringstream dir_stream;
		dir_stream << job_info->request->save_path << "\\MaskImage2";

		std::filesystem::path dir_path(dir_stream.str());
		std::filesystem::create_directories(dir_path);

		// 각 클래스별 마스크 저장
		for (const auto&[class_idx, mat] : blob_mask_list) {
			if (mat.empty()) continue;

			std::ostringstream filename;
			filename
				<< job_info->request->equip_no
				<< "_" << job_info->request->product_name
				<< "_" << job_info->request->lot_id
				<< "_Tray" << job_info->request->tray_no
				<< "_Module" << job_info->request->module_no
				<< "_" << job_info->request->vision_name
				<< "" << job_info->request->pc_idx
				<< "_Stage" << job_info->request->stage_idx
				<< "_Image" << job_info->request->image_idx
				<< "_ROI" << job_info->request->roi_id
				<< "_" << job_info->request->inspectiontype_name
				<< "_Tab" << job_info->request->inspection_tab_idx + 1
				<< "_" << job_info->request->barcode
				<< "_" << job_info->request->local_align_angle_string
				<< "_" << job_info->request->channel_type
				<< "_Mask2_DefectType" << class_idx
				<< ".png";

			std::filesystem::path full_path = dir_path / filename.str();

			cv::Mat normalized;
			cv::normalize(mat, normalized, 0, 255, cv::NORM_MINMAX, CV_8UC1);
			save_image_thread_pool_->enqueue_job_no_return(
				[](const std::string& path, cv::Mat img) {
					cv::imwrite(path, img);
				},
				full_path.string(), std::move(normalized));
		}
	}

	cv::Mat InspectorManager::handle_tertiary_inspection(int defect_type_idx, const cv::Mat & mask, const std::shared_ptr<JobInfo>& job_info)
	{
		auto image_info_list = AIService::ImageUtile::ExtractValidPatches(job_info->request->roi_image, mask, cv::Size(256, 256), 236);
		std::vector<AIService::ImageUtile::PatchInfo> image_seg_patch_info_list;
		_logger->info("[] THRDLY_INSPECTION: vision_pos={}, tray_no={}, module_no={}, image_no={}, roi_no={}, tab_idx={}", "req_size={}",
			job_info->request->vision_cam,
			job_info->request->tray_no,
			job_info->request->module_no,
			job_info->request->image_idx,
			job_info->request->roi_id,
			job_info->request->inspection_tab_idx,
			image_info_list.size());
		bool adj_inspect_false = false;

		int patch_idx = 0;
		for (const auto& patch_info : image_info_list) {
			int counter = job_counter_++;
			patch_idx++;
			std::ostringstream filename;
			filename
				<< "" << job_info->request->equip_no
				<< "_" << job_info->request->product_name
				<< "_" << job_info->request->lot_id
				<< "_Tray" << job_info->request->tray_no
				<< "_Module" << job_info->request->module_no
				<< "_" << job_info->request->vision_name
				<< "" << job_info->request->pc_idx
				<< "_Stage" << job_info->request->stage_idx
				<< "_Image" << job_info->request->image_idx
				<< "_ROI" << job_info->request->roi_id
				<< "_" << job_info->request->inspectiontype_name
				<< "_Tab" << job_info->request->inspection_tab_idx + 1
				<< "_" << job_info->request->barcode
				<< "_" << job_info->request->local_align_angle_string
				<< "_" << job_info->request->channel_type
				<< "_Mask3_DefectType" << defect_type_idx
				<< "_Patch" << patch_idx;

			//std::ostringstream dir_stream;
			//dir_stream << "D:\\TEMP";
			//std::string filename_ = filename.str();

			//std::filesystem::path dir_path(dir_stream.str());

			//std::filesystem::path full_path = dir_path / (filename_ + ".png");
			//cv::imwrite(full_path.string(), patch_info.patch_image);

			int secial_defect_trans_code = -1;
			int model_index = thirdly_inspector_.AssignDeepLearningSegmentModel(
				0,
				job_info->request->vision_cam,
				g_strInspectionTypeName_Short[job_info->request->roi_inspection_type_idx].c_str(),
				THEAPP.GetDefectCode(defect_type_idx, &secial_defect_trans_code));
			if (model_index <= 0) 
			{
				adj_inspect_false = true;
				return cv::Mat();
			}

			auto rec_image = thirdly_inspector_.SendSegmentImagetoADJ(
				1,
				job_info->request->magazine_no,
				job_info->request->tray_no,
				job_info->request->module_no,
				job_info->request->lot_id.data(),
				counter,
				model_index,
				filename.str().c_str(),
				GetTickCount(),
				patch_info.patch_image
			);
			if (rec_image.empty()) 
			{ 
				// ADJ ERROR
				adj_inspect_false = true;
				return cv::Mat(); 
			}

			image_seg_patch_info_list.emplace_back(
				AIService::ImageUtile::PatchInfo(patch_info.roi, rec_image)
			);
		}

		cv::Mat restore_image = AIService::ImageUtile::RestoreMaskFromPatches(image_seg_patch_info_list, job_info->request->image.size());
		if (is_use_segmentation_visualization_) {
			auto visualizing_start = std::chrono::high_resolution_clock::now();
			tertiary_result_visualrising(job_info, image_info_list, restore_image, mask);
			auto visualizing_end = std::chrono::high_resolution_clock::now();
			auto visualizing_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(visualizing_end - visualizing_start).count();
			_logger->info("[] THRDLY_INSPECTION: Visualizing completed - elapsed={}ms", visualizing_elapsed);
		}
		return restore_image;
	}

	cv::Mat InspectorManager::handle_tertiary_inspection_local(int defect_type_idx, const cv::Mat & mask, const std::shared_ptr<JobInfo>& job_info)
	{
		job_info->status = JobStatus::StepSEG_Preprocessing;
		auto image_info_list = AIService::ImageUtile::ExtractValidPatches(job_info->request->roi_image, mask, cv::Size(256, 256), 236);
		std::vector<AIService::ImageUtile::PatchInfo> image_seg_patch_info_list;
		_logger->info("[] THRDLY_INSPECTION: vision_pos={}, tray_no={}, module_no={}, image_no={}, roi_no={}, tab_idx={}, req_size={}",
			job_info->request->vision_cam,
			job_info->request->tray_no,
			job_info->request->module_no,
			job_info->request->image_idx,
			job_info->request->roi_id,
			job_info->request->inspection_tab_idx,
			image_info_list.size());

		if (image_info_list.empty()) {
			_logger->warn("[] THRDLY_INSPECTION: No valid patches found");
			return cv::Mat();
		}

		job_info->status = JobStatus::StepSEG_Inspection;
		const auto& req = job_info->request;
		
		// 이미지를 8개씩 나누어서 처리
		const size_t batch_size = 8;
		size_t total_patches = image_info_list.size();
		size_t num_batches = (total_patches + batch_size - 1) / batch_size; // 올림 계산

		for (size_t batch_idx = 0; batch_idx < num_batches; ++batch_idx) {
			std::array<cv::Mat, 8> batch_images;
			std::array<cv::Rect, 8> batch_rois;
			
			size_t start_idx = batch_idx * batch_size;
			size_t end_idx = std::min(start_idx + batch_size, total_patches);
			size_t actual_count = end_idx - start_idx;

			_logger->info("[] THRDLY_INSPECTION: Processing batch {}/{}, patches {}-{} (count={})",
				batch_idx + 1, num_batches, start_idx, end_idx - 1, actual_count);

			// 배치에 이미지 채우기
			for (size_t i = 0; i < batch_size; ++i) {
				if (i < actual_count) {
					// 실제 패치 이미지 사용
					size_t patch_idx = start_idx + i;
					batch_images[i] = image_info_list[patch_idx].patch_image.clone();
					batch_rois[i] = image_info_list[patch_idx].roi;
				} else {
					// 마지막 이미지로 부족한 부분 채우기
					size_t last_patch_idx = end_idx - 1;
					batch_images[i] = image_info_list[last_patch_idx].patch_image.clone();
					batch_rois[i] = image_info_list[last_patch_idx].roi;
					_logger->debug("[] THRDLY_INSPECTION: Padding with last image for index {}", i);
				}
			}

			try {
				// SEG 검사 요청
				auto seg_result = seg_inspector_->add_job(
					req->product_name,
					req->lot_id,
					req->barcode,
					req->vision_cam,
					req->tray_no,
					req->module_no,
					req->image_idx,
					req->roi_id,
					req->inspection_tab_idx,
					req->roi_inspection_type_idx,
					req->defect_name,
					batch_images
				).get();

				// 결과 처리 - 실제 패치 개수만큼만 결과 사용
				for (size_t i = 0; i < actual_count; ++i) {
					if (!seg_result[i].empty()) {
						size_t original_patch_idx = start_idx + i;
						image_seg_patch_info_list.emplace_back(
							AIService::ImageUtile::PatchInfo(
								image_info_list[original_patch_idx].roi, 
								seg_result[i]
							)
						);
					}
				}

				_logger->info("[] THRDLY_INSPECTION: Batch {} completed, added {} results",
					batch_idx + 1, actual_count);
			}
			catch (const std::exception& e) {
				_logger->error("[] THRDLY_INSPECTION: Exception in batch {} - {}", batch_idx + 1, e.what());
				return cv::Mat(); // 검사 실패
			}
		}

		if (image_seg_patch_info_list.empty()) {
			_logger->warn("[] THRDLY_INSPECTION: No segmentation results obtained");
			return cv::Mat();
		}

		// 패치 결과들을 원본 이미지 크기로 복원
		cv::Mat restore_image = AIService::ImageUtile::RestoreMaskFromPatches(image_seg_patch_info_list, job_info->request->image.size());
		if (is_use_segmentation_visualization_) {
			auto visualizing_start = std::chrono::high_resolution_clock::now();
			tertiary_result_visualrising(job_info, image_info_list, restore_image, mask);
			auto visualizing_end = std::chrono::high_resolution_clock::now();
			auto visualizing_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(visualizing_end - visualizing_start).count();
			_logger->info("[] THRDLY_INSPECTION: Visualizing completed - elapsed={}ms", visualizing_elapsed);
		}
		return restore_image;
	}

	Model::LAIInspector3Result InspectorManager::handle_tertiary_inspection_local(const cv::Mat& primary_mask, const std::shared_ptr<JobInfo>& job_info)
	{
		if (job_info->request->defect_name_index < 0) { return Model::LAIInspector3Result{ false }; }
		auto inspector3_result_mask = handle_tertiary_inspection_local(job_info->request->defect_name_index, primary_mask, job_info);
		if (inspector3_result_mask.empty()) {
			return Model::LAIInspector3Result{ false };
		} // 검사 오류
		if (!AIService::ImageUtile::HasImageData(inspector3_result_mask)) {
			return Model::LAIInspector3Result{ true };
		} // 검사 하였으나 양품 판정
		auto filtered_mask = tertiary_result_mask_filtering(primary_mask, inspector3_result_mask, 0); // 결과 마스크 필터링
		if (!filtered_mask.has_value()) {
			return Model::LAIInspector3Result{ true };
		} // 필터링 후 양품 판별

		std::map<int, cv::Mat> blob_mask_map; // 결과 마스크 저장
		blob_mask_map[job_info->request->defect_name_index] = std::move(filtered_mask.value());
		save_tertiary_masks(blob_mask_map, job_info);
		Model::LAIInspector3Result inspector3_result = { true, std::move(blob_mask_map) };
		return inspector3_result;
	}

	Model::LAIInspector3Result InspectorManager::handle_tertiary_inspection_local(const cv::Mat& primary_mask, const Model::LAIInspector2Result & inspector2_result, const std::shared_ptr<JobInfo>& job_info)
	{
		std::map<int, cv::Mat> blob_mask_map;
		bool run_inspection3 = true;

		if (inspector2_result.blob_mask_map.empty()) {
			return Model::LAIInspector3Result{ false };
		}

		for (const auto&[defect_type_idx, mask] : inspector2_result.blob_mask_map) {
			if (defect_type_idx == -1) { continue; }
			auto inspector3_result_mask = handle_tertiary_inspection_local(defect_type_idx, mask, job_info);
			if (inspector3_result_mask.empty()) { 
				return Model::LAIInspector3Result{ false };
			}
			if (!AIService::ImageUtile::HasImageData(inspector3_result_mask)) {
				continue;
			} // 검사 하였으나 양품 판정
			auto filtered_mask = tertiary_result_mask_filtering(primary_mask, inspector3_result_mask, 0); // 결과 마스크 필터링
			if (!filtered_mask.has_value()) {
				continue;
			} // 필터링 후 양품 판별

			try { // 결과 처리
				auto it = blob_mask_map.find(defect_type_idx);
				if (it != blob_mask_map.end()) {
					cv::Mat& existing_mask = it->second;
					if (existing_mask.empty()) {
						existing_mask = std::move(filtered_mask.value());
					}
					else {
						if (existing_mask.size() != filtered_mask.value().size() || existing_mask.type() != filtered_mask.value().type()) {
							_logger->error("마스크 크기 또는 타입 불일치: defect_type_idx={}", defect_type_idx);
							continue;
						}
						cv::bitwise_or(existing_mask, filtered_mask.value(), existing_mask);
					}
				}
				else {
					blob_mask_map[defect_type_idx] = std::move(filtered_mask.value());
				}
			}
			catch (const std::exception& e) {
				_logger->error("3차 로컬 마스크 처리 중 예외 발생: defect_type_idx={}, what={}", defect_type_idx, e.what());
				continue;
			}
			catch (...) {
				_logger->error("3차 로컬 마스크 처리 중 알 수 없는 예외 발생: defect_type_idx={}", defect_type_idx);
				continue;
			}
			run_inspection3 = true;
		}

		save_tertiary_masks(blob_mask_map, job_info);

		Model::LAIInspector3Result inspector3_result = { run_inspection3, std::move(blob_mask_map) };
		return inspector3_result;
	}

	Model::LAIInspector3Result InspectorManager::handle_tertiary_inspection(const cv::Mat& primary_mask, const std::shared_ptr<JobInfo>& job_info)
	{
		if (job_info->request->defect_name_index < 0) { return Model::LAIInspector3Result{ false }; }
		auto inspector3_result_mask = handle_tertiary_inspection(job_info->request->defect_name_index, primary_mask, job_info);
		if (inspector3_result_mask.empty()) {
			return Model::LAIInspector3Result{ false };
		} // 검사 오류
		if (!AIService::ImageUtile::HasImageData(inspector3_result_mask)) {
			return Model::LAIInspector3Result{ true };
		} // 검사 하였으나 양품 판정
		auto filtered_mask = tertiary_result_mask_filtering(primary_mask, inspector3_result_mask); // 결과 마스크 필터링
		if (!filtered_mask.has_value()) {
			return Model::LAIInspector3Result{ true };
		} // 필터링 후 양품 판정

		std::map<int, cv::Mat> blob_mask_map; // 결과 마스크 저장
		blob_mask_map[job_info->request->defect_name_index] = std::move(filtered_mask.value());
		save_tertiary_masks(blob_mask_map, job_info);
		Model::LAIInspector3Result inspector3_result = { true, std::move(blob_mask_map) };
		return inspector3_result;
	}

	Model::LAIInspector3Result InspectorManager::handle_tertiary_inspection(const cv::Mat& primary_mask, const Model::LAIInspector2Result & inspector2_result, const std::shared_ptr<JobInfo>& job_info)
	{
		std::map<int, cv::Mat> blob_mask_map;
		bool run_inspection3 = true;

		if (inspector2_result.blob_mask_map.empty()) {
			return Model::LAIInspector3Result{ false };
		}

#ifdef LGITAI
		if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJ[0] && !THEAPP.m_TCPClientService.m_bConnect[0][0]) {
			for (int i = 0; i < MAX_MULTI_CONNECTION_NUM; i++)
			{
				if (THEAPP.m_TCPClientService.m_bConnect[0][i])
					THEAPP.m_TCPClientService.m_arrClient[0][i].TCPonClose();
				THEAPP.m_TCPClientService.m_bConnect[0][i] = FALSE;
			}
			for (int i = 0; i < THEAPP.Struct_PreferenceStruct.m_nADJ_MultiNetworkNO; i++)
			{
				if (!THEAPP.m_TCPClientService.m_bConnect[0][i])
				{
					if (THEAPP.m_TCPClientService.m_arrClient[0][i].TCPConnect())
					{
						THEAPP.m_TCPClientService.m_bConnect[0][i] = TRUE;
					}
					else
					{
						THEAPP.m_TCPClientService.m_bConnect[0][i] = FALSE;
					}
				}
			}
		}
#endif // LGITAI

		for (const auto&[defect_type_idx, mask] : inspector2_result.blob_mask_map) {
			if (defect_type_idx == -1) { continue; }
			auto inspector3_result_mask = handle_tertiary_inspection(defect_type_idx, mask, job_info);
			if (inspector3_result_mask.empty()) { 
				return Model::LAIInspector3Result{ false };
			}
			if (!AIService::ImageUtile::HasImageData(inspector3_result_mask)) {
				continue;
			} // 검사 하였으나 양품 판별
			auto filtered_mask = tertiary_result_mask_filtering(primary_mask, inspector3_result_mask); // 결과 마스크 필터링
			if (!filtered_mask.has_value()) {
				continue;
			} // 필터링 후 양품 판별

			try { // 결과 처리
				auto it = blob_mask_map.find(defect_type_idx);
				if (it != blob_mask_map.end()) {
					cv::Mat& existing_mask = it->second;
					if (existing_mask.empty()) {
						existing_mask = std::move(filtered_mask.value());
					}
					else {
						if (existing_mask.size() != filtered_mask.value().size() || existing_mask.type() != filtered_mask.value().type()) {
							_logger->error("마스크 크기 또는 타입 불일치: defect_type_idx={}", defect_type_idx);
							continue;
						}
						cv::bitwise_or(existing_mask, filtered_mask.value(), existing_mask);
					}
				}
				else {
					blob_mask_map[defect_type_idx] = std::move(filtered_mask.value());
				}
			}
			catch (const std::exception& e) {
				_logger->error("3차 마스크 처리 중 예외 발생: defect_type_idx={}, what={}", defect_type_idx, e.what());
				continue;
			}
			catch (...) {
				_logger->error("3차 마스크 처리 중 알 수 없는 예회 발생: defect_type_idx={}", defect_type_idx);
				continue;
			}
			run_inspection3 = true;
		}

		save_tertiary_masks(blob_mask_map, job_info);

		Model::LAIInspector3Result inspector3_result = { run_inspection3, std::move(blob_mask_map) };
		return inspector3_result;
	}

	void InspectorManager::save_tertiary_masks(
		const std::map<int, cv::Mat>& blob_mask_list,
		const std::shared_ptr<JobInfo>& job_info)
	{
		if (blob_mask_list.empty())
			return;

		std::ostringstream dir_stream;
		dir_stream << job_info->request->save_path << "\\MaskImage3";

		std::filesystem::path dir_path(dir_stream.str());
		std::filesystem::create_directories(dir_path);

		// 각 클래스별 마스크 저장
		for (const auto&[class_idx, mat] : blob_mask_list) {
			if (mat.empty()) continue;

			std::ostringstream filename;
			filename
				<< job_info->request->equip_no
				<< "_" << job_info->request->product_name
				<< "_" << job_info->request->lot_id
				<< "_Tray" << job_info->request->tray_no
				<< "_Module" << job_info->request->module_no
				<< "_" << job_info->request->vision_name
				<< "" << job_info->request->pc_idx
				<< "_Stage" << job_info->request->stage_idx
				<< "_Image" << job_info->request->image_idx
				<< "_ROI" << job_info->request->roi_id
				<< "_" << job_info->request->inspectiontype_name
				<< "_Tab" << job_info->request->inspection_tab_idx + 1
				<< "_" << job_info->request->barcode
				<< "_" << job_info->request->local_align_angle_string
				<< "_" << job_info->request->channel_type
				<< "_Mask3_DefectType" << class_idx
				<< ".png";

			std::filesystem::path full_path = dir_path / filename.str();

			cv::Mat normalized;
			cv::normalize(mat, normalized, 0, 255, cv::NORM_MINMAX, CV_8UC1);
			save_image_thread_pool_->enqueue_job_no_return(
				[](const std::string& path, cv::Mat img) {
					cv::imwrite(path, img);
				},
				full_path.string(), std::move(normalized));
		}
	}

	std::optional<cv::Mat> InspectorManager::tertiary_result_mask_filtering(const cv::Mat& primary_mask, const cv::Mat& tertiary_adj_mask, int compare)
	{
		/* 결과 Mask에 특정 데이터가 있는지 확인 */
		// 현 1고정 LGIT 에서 1만 불량으로 검출;
		cv::Mat specific_mask;
		if (compare != 0) {
			specific_mask = AIService::ImageUtile::ExtractSpecificValue(tertiary_adj_mask, compare);
		}
		else {
			specific_mask = tertiary_adj_mask;
		}
		// 값이 있는지 확인
		if (!AIService::ImageUtile::HasImageData(specific_mask)) {
			return std::nullopt;
		}

		/* 1차 mask와 3차 mask가 겹치는지 판단하여 겹치지 않을 경우 해당 mask 버리기
		cv::Mat intersect;
		cv::bitwise_and(mask, specific_mask, intersect);
		if (cv::countNonZero(intersect) <= 0) { continue; }
		이거는 폐기, 한개의 패치 내 여러 mask가 하나의 mask로 라벨링되어 있어 동시에 처리됨
		다만 아래 방법이 속도에 문제를 발생시킨다면 이 방법이 적절 */

		// 3차 mask 라벨링 후 1차 mask와 겹치는 영역이 있는 mask만 추출하기
		cv::Mat filtered_connected_mask = cv::Mat::zeros(specific_mask.size(), CV_8UC1);
		/// 3차 mask 라벨링
		cv::Mat labels, stats, centroids;
		int nLabels = cv::connectedComponentsWithStats(specific_mask, labels, stats, centroids, 8, CV_32S);

		for (int label = 1; label < nLabels; label++) // 0은 background이니까 1부터
		{
			cv::Mat label_mask;
			cv::inRange(labels, label, label, label_mask);

			// mask와 겹치는 영역 있는지 확인하여 filtered_connected_mask로 넘기기
			cv::Mat intersection;
			cv::bitwise_and(primary_mask, label_mask, intersection);
			if (cv::countNonZero(intersection) > 0)
				cv::bitwise_or(filtered_connected_mask, label_mask, filtered_connected_mask);
		}

		// filtered_connected_mask에 아무런 mask가 존재하지 않는다면 continue
		if (cv::countNonZero(filtered_connected_mask) == 0)
			return std::nullopt;

		return filtered_connected_mask;
	}

	std::string InspectorManager::get_current_date_string()
	{
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);

		std::tm tm;
		localtime_s(&tm, &now_c);  // 안전한 localtime

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y%m%d");  // "20250321"
		return oss.str();
	}

	Model::LAIInspectorSVMResult InspectorManager::handle_svm_inspection(
		const cv::Mat& primary_mask,
		const std::shared_ptr<JobInfo>& job_info) 
	{
		const auto& req = job_info->request;
		const uint64_t job_id = job_info->request->job_id; // JobInfo 또는 Request에 job_id 저장된 것으로 가정
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		std::string thread_id_str = oss.str();

		std::string defect_type_str = svm_inspector_->_condition_short_name_list[req->roi_inspection_type_idx];

		_logger->info("[job_id={}] StepS: Running secondary inspection - vision_pos={}, tray_no={}, module_no={}, image_no={}, roi_no={}, defect_type={}, thread_id={}",
			job_id,
			req->vision_cam,
			req->tray_no,
			req->module_no,
			req->image_idx,
			req->roi_id,
			defect_type_str,
			thread_id_str);

		auto t_start = std::chrono::high_resolution_clock::now();

		auto result2 = svm_inspector_->add_job(
			req->product_name,
			req->lot_id,
			req->barcode,
			req->vision_cam,
			req->tray_no,
			req->module_no,
			req->image_idx,
			req->roi_id,
			req->inspection_tab_idx,
			req->roi_inspection_type_idx,
			req->defect_name,
			req->image,
			primary_mask).get();

		job_info->status = JobStatus::SVM_Preprocessing;

		const auto& blob_ids = result2.blob_id_list;
		const auto& class_indices = result2.class_idx_list;
		auto& blob_ng_mask = result2.blob_ng_mask;
		auto& blob_ok_mask = result2.blob_ok_mask;

		for (size_t i = 0; i < blob_ids.size(); ++i)
		{
			int class_idx = class_indices[i];
			//std::filesystem::path dir_path = "D:\\TEMP\\";
			try {
				cv::Mat find_mask = AIService::ImageUtile::ExtractSpecificValue(primary_mask, static_cast<uchar>(blob_ids[i]));
				//cv::Mat normalized;
				//cv::normalize(find_mask, normalized, 0, 255, cv::NORM_MINMAX, CV_8UC1);

				//std::filesystem::path temp_image = dir_path / ("find_mask" + std::to_string(i) + ".png");
				//std::filesystem::path temp_normal_image = dir_path / ("find_mask" + std::to_string(i) + "N.png");

				//cv::imwrite(temp_image.string(), find_mask);
				//cv::imwrite(temp_normal_image.string(), normalized);

				if (!AIService::ImageUtile::HasImageData(find_mask)) {
					_logger->warn("[job_id={}] StepS: Empty mask found for class_idx={}", job_id, class_idx);
					continue;
				}

				if (class_idx <= 0) {
					if (blob_ok_mask.empty()) {
						blob_ok_mask = std::move(find_mask);
					}
					else {
						if (blob_ok_mask.size() != find_mask.size() || blob_ok_mask.type() != find_mask.type()) {
							_logger->error("[job_id={}] StepS: Mask size/type mismatch for class_idx={}", job_id, class_idx);
							continue;
						}
						cv::bitwise_or(blob_ok_mask, find_mask, blob_ok_mask);
					}
				}
				else {
					if (blob_ng_mask.empty()) {
						blob_ng_mask = std::move(find_mask);
					}
					else {
						if (blob_ng_mask.size() != find_mask.size() || blob_ng_mask.type() != find_mask.type()) {
							_logger->error("[job_id={}] StepS: Mask size/type mismatch for class_idx={}", job_id, class_idx);
							continue;
						}
						cv::bitwise_or(blob_ng_mask, find_mask, blob_ng_mask);
					}
				}
			}
			catch (const std::exception& e) {
				_logger->error("[job_id={}] StepS: Exception while processing class_idx={} - {}", job_id, class_idx, e.what());
			}
			catch (...) {
				_logger->error("[job_id={}] StepS: Unknown exception while processing class_idx={}", job_id, class_idx);
			}

			//std::filesystem::path blob_ok_image_path = dir_path / ("blob_ok" + std::to_string(i) + ".png");
			//std::filesystem::path blob_ng_image_path = dir_path / ("blob_ng" + std::to_string(i) + ".png");

			//if (!blob_ok_mask.empty()) {
			//	cv::imwrite(blob_ok_image_path.string(), blob_ok_mask);
			//}
			//if (!blob_ng_mask.empty()) {
			//	cv::imwrite(blob_ng_image_path.string(), blob_ng_mask);
			//}
		}

		if (result2.result) {
			job_info->inspect = JobStatus::StepS_Done;
		}
		job_info->status = JobStatus::StepS_Done;

		save_svm_masks(result2.blob_ng_mask, result2.blob_ok_mask, job_info);

		auto t_end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();

		_logger->info("[job_id={}] StepS: Secondary inspection completed - blob_count={}, result={}, elapsed={}ms",
			job_id,
			result2.blob_count,
			result2.result,
			elapsed);

		return result2;
	}

	void InspectorManager::save_svm_masks(const cv::Mat & ng_mask, const cv::Mat & ok_mask, const std::shared_ptr<JobInfo>& job_info)
	{
		if (ng_mask.empty() && ok_mask.empty())
			return;

		std::ostringstream dir_stream;
		dir_stream << job_info->request->save_path << "\\MaskImage1SVM";

		std::filesystem::path dir_path(dir_stream.str());
		std::filesystem::create_directories(dir_path);

		std::ostringstream filename;
		filename
			<< job_info->request->equip_no
			<< "_" << job_info->request->product_name
			<< "_" << job_info->request->lot_id
			<< "_Tray" << job_info->request->tray_no
			<< "_Module" << job_info->request->module_no
			<< "_" << job_info->request->vision_name
			<< "" << job_info->request->pc_idx
			<< "_Stage" << job_info->request->stage_idx
			<< "_Image" << job_info->request->image_idx
			<< "_ROI" << job_info->request->roi_id
			<< "_" << job_info->request->inspectiontype_name
			<< "_Tab" << job_info->request->inspection_tab_idx + 1
			<< "_" << job_info->request->barcode
			<< "_" << job_info->request->local_align_angle_string
			<< "_" << job_info->request->channel_type
			<< "_Mask1_SVM"
			<< ".png";

		std::filesystem::path full_path = dir_path / filename.str();

		auto save_mask = [&](const cv::Mat& mask, const std::string& suffix) {
			//if (suffix == "OK")
			//	full_path = dir_path / filename.str().replace(filename.str().find("Mask1_SVM"), 10, "Mask1_SVM(OK)");

			save_image_thread_pool_->enqueue_job_no_return(
				[](const std::string& path, cv::Mat img) {
					cv::imwrite(path, img);
				},
				full_path.string(), std::move(mask.clone()));
		};

		if (!ng_mask.empty())
			save_mask(ng_mask, "NG");

		//if (!ok_mask.empty())
		//	save_mask(ok_mask, "OK");
	}

	// 랜덤 마스크와 랜덤 BGR 이미지를 생성하여 secondary_inspector_->add_job()으로 검사 요청하는 함수 예시
	void InspectorManager::test_secondary_inspection_with_random_data() {
		std::string product_name = std::string(CT2A(THEAPP.g_strModelTypeName[THEAPP.GetModelType()]));
		// secondary_inspector_->


		// 임의 파라미터 설정
		// std::string product_name = "TestProduct";
		std::string lot_id = "TEST";
		std::string barcode = "BARCODE123";
		int vision_cam = 0;
		int tray_no = 0;
		int module_no = 0;
		int image_idx = 2;
		int roi_id = 4;
		int inspection_tab_idx = 0;
		int roi_inspection_type_idx = 21;
		std::string defect_name = "Contamination";
		int width = 913, height = 281;

		// 랜덤 BGR 이미지 생성
		cv::Mat random_bgr(height, width, CV_8UC3);
		cv::randu(random_bgr, cv::Scalar::all(0), cv::Scalar::all(255));

		// 랜덤 마스크(0~2 값) 생성
		cv::Mat random_mask(height, width, CV_8UC1);
		cv::randu(random_mask, 0, 3);

		// 검사 요청
		auto result2 = secondary_inspector_->add_job(
			product_name,
			lot_id,
			barcode,
			vision_cam,
			tray_no,
			module_no,
			image_idx,
			roi_id,
			inspection_tab_idx,
			roi_inspection_type_idx,
			defect_name,
			random_bgr,
			random_mask
		).get();

		_logger->info("test_secondary_inspection_with_random_data: result={}, blob_count={}", result2.result, result2.blob_count);
	}

	void InspectorManager::tertiary_result_visualrising(
		const std::shared_ptr<JobInfo>& job_info,
		const std::vector<AIService::ImageUtile::PatchInfo>& patch_image_list,
		const cv::Mat & restore_image,
		const cv::Mat & mask)
	{
		std::ostringstream dir_stream;
		dir_stream << job_info->request->save_path << "\\MaskImage3Visualrising\\";

		std::filesystem::path dir_path(dir_stream.str());
		std::filesystem::create_directories(dir_path);

		std::ostringstream filename;
		filename << job_info->request->lot_id
			<< "_Tray" << job_info->request->tray_no
			<< "_Module" << job_info->request->module_no
			<< "_Vision" << job_info->request->vision_cam
			<< "_Image" << job_info->request->image_idx
			<< "_ROI" << job_info->request->roi_id
			<< "_Tab" << job_info->request->inspection_tab_idx + 1
			<< "_DefectType" << job_info->request->roi_inspection_type_idx;

		int i = 0;
		//for (AIService::ImageUtile::PatchInfo patch_image : image_info_list) {
		//	std::string save_path = "D:\\TEMP\\patch_image_" + std::to_string(i) + ".jpg";
		//	cv::imwrite(save_path, patch_image.patch_image);
		//	i++;
		//}
		std::string restore_image_path = dir_stream.str() + filename.str() + "_restore_image.png";
		cv::Mat restore_image_normalized;
		cv::normalize(restore_image, restore_image_normalized, 0, 255, cv::NORM_MINMAX, CV_8UC1);
		// cv::imwrite(restore_image_path, restore_image_normalized);
		save_image_thread_pool_->enqueue_job_no_return(
			[](const std::string& path, cv::Mat img) {
				cv::imwrite(path, img);
			},
			restore_image_path, std::move(restore_image_normalized));

		std::string mask_normalized_image_path = dir_stream.str() + filename.str() + "_mask_normalized_image.png";
		cv::Mat normalized;
		cv::normalize(mask, normalized, 0, 255, cv::NORM_MINMAX, CV_8UC1);
		// cv::imwrite(mask_normalized_image_path, normalized);
		save_image_thread_pool_->enqueue_job_no_return(
			[](const std::string& path, cv::Mat img) {
				cv::imwrite(path, img);
			},
			mask_normalized_image_path, std::move(normalized));

		// mask 정규화 및 컬러로 변환
		cv::Mat normalized_mask;
		cv::normalize(mask, normalized_mask, 0, 255, cv::NORM_MINMAX);
		cv::Mat mask_color;
		cv::cvtColor(normalized_mask, mask_color, cv::COLOR_GRAY2BGR);

		// 원본 이미지 복사 (컬러로 변환)
		cv::Mat overlay_image;
		if (job_info->request->image.channels() == 1)
			cv::cvtColor(job_info->request->image, overlay_image, cv::COLOR_GRAY2BGR);
		else
			overlay_image = job_info->request->image.clone();

		// 1. 마스크와 원본 이미지 합성 (mask는 단순 밝기 변화만 적용, 색상 tint 없음)
		cv::Mat blend;
		cv::addWeighted(overlay_image, 0.8, mask_color, 0.8, 0.0, blend);

		// 2. 색상 다양화된 패치 경계 시각화
		auto get_distinct_color = [](int index) -> cv::Scalar {
			int hue = (index * 47) % 360;
			cv::Mat hsv(1, 1, CV_8UC3, cv::Scalar(hue / 2, 200, 255));
			cv::Mat bgr;
			cv::cvtColor(hsv, bgr, cv::COLOR_HSV2BGR);
			cv::Vec3b color = bgr.at<cv::Vec3b>(0, 0);
			return cv::Scalar(color[0], color[1], color[2]);
		};

		for (size_t i = 0; i < patch_image_list.size(); ++i) {
			const auto& patch = patch_image_list[i];
			cv::Scalar color = get_distinct_color(static_cast<int>(i));
			cv::rectangle(blend, patch.roi, color, 1, cv::LINE_8);
		}

		// 3. 저장
		std::string overlay_patch_with_mask_path = dir_stream.str() + filename.str() + "_overlay_patch_with_mask_path.png";
		// cv::imwrite(overlay_patch_with_mask_path, blend);
		save_image_thread_pool_->enqueue_job_no_return(
			[](const std::string& path, cv::Mat img) {
				cv::imwrite(path, img);
			},
			overlay_patch_with_mask_path, std::move(blend));
	}
}
#endif // LGITAI