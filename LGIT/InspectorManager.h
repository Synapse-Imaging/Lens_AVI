#pragma once
#ifdef LGITAI
#define USE_SUAKIT
#include <tbb/concurrent_unordered_map.h>
#include <atomic>
#include <memory>
#include <optional>
#include <filesystem>
#include <mutex>
#include <condition_variable>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "types.h"
#include "LAIInspector.h"
#include "LAIInspector2.h"
#include "LAIInspectorSVM.h"
#include "LAIInspectorSEG.h"
#include "LAIInspectorVAL.h"
#include "Model/InspectorManagerSetup.h"
#include "../AIService/ThreadPool.h"
#include "../ADJClientService.h"
#include "../AIService/WatchdogTimer.h"

namespace LGIT {

	/**
	 * @brief 검사 작업을 관리하고 비동기적으로 수행하는 관리자 클래스
	 */
	class InspectorManager {
	public:
		/**
		 * @brief InspectorManager 생성자
		 *
		 * @param num_threads 사용할 쓰레드 개수
		 * @param inspector1 1차 검사기
		 * @param inspector2 2차 검사기
		 * @param logging_level 로깅 레벨
		 * @param log_root 로그 파일 저장 루트 경로
		 */
		InspectorManager(
			size_t num_threads,
			std::shared_ptr<LAIInspector> inspector1,
			std::shared_ptr<LAIInspector2> inspector2,
			CADJClientService& inspector3,
			std::shared_ptr<LAIInspectorSVM> inspectorSVM,
			std::shared_ptr<LAIInspectorSEG> inspectorSEG, 
			std::shared_ptr<LAIInspectorVAL> inspectorVAL, 
			int logging_level,
			bool is_use_segmentation_visualization,
			std::filesystem::path log_root);

		/**
		 * @brief 검사 요청을 제출하고 처리 결과를 비동기로 받음
		 *
		 * @param request 검사 요청 객체
		 * @param out_job_id 생성된 작업 ID 출력
		 * @return std::future<InspectionResultSet> 처리 결과에 대한 future 객체
		 */
		std::future<InspectionResultSet> submit(
			std::shared_ptr<InspectionRequest> request,
			uint64_t& out_job_id);

		/**
		 * @brief 모델 재로드 시도
		 * @details 조건을 확인하고 필요시 조건을 만족시킨 후 모델 재로드를 수행합니다.
		 * @return true if successful, false otherwise
		 */
		bool try_model_reload();

	private:
		/**
		 * @brief 개별 검사 작업 단위 정보를 저장하는 구조체
		 */
		struct JobInfo {
			std::shared_ptr<InspectionRequest> request;  ///< 검사 요청
			std::promise<InspectionResultSet> promise;   ///< 결과 전달용 promise
			std::atomic<JobStatus> status = JobStatus::Pending; ///< 현재 상태
			std::atomic<JobStatus> inspect = JobStatus::Pending; ///< 현재 검사 단계
			std::atomic<JobStatus> error = JobStatus::Pending;   ///< 에러 상태
		};

	private:
		/**
		 * @brief 현재 날짜를 문자열로 반환 (YYYY-MM-DD 형식)
		 *
		 * @return 날짜 문자열
		 */
		std::string get_current_date_string();

		/**
		 * @brief 1차 검사기에게 직접 이미지를 전달해 검사 수행
		 *
		 * @param job_info 작업 정보
		 * @param results 결과 저장 리스트
		 * @return 생성된 마스크 이미지
		 */
		Model::LAIInspectorResult handle_primary_inspection(const std::shared_ptr<JobInfo>& job_info);

		/**
		 * @brief Region 정보로부터 1차 마스크 생성
		 *
		 * @param job_info 작업 정보
		 * @param results 결과 저장 리스트
		 * @return 생성된 마스크 이미지
		 */
		Model::LAIInspectorResult handle_primary_mask_from_region(const std::shared_ptr<JobInfo>& job_info);

		/**
		 * @brief 1차 마스크를 로컬 파일로 저장
		 *
		 * @param mask 저장할 마스크 이미지
		 * @param job_info 작업 정보
		 */
		void save_primary_mask_image(const cv::Mat& mask, const std::shared_ptr<JobInfo>& job_info);


		void save_val_mask_image(const cv::Mat& mask, const std::shared_ptr<JobInfo>& job_info);

		/**
		 * @brief val 검사기 수행
		 *
		 * @param job_info 작업 정보
		 * @param results 결과 저장 리스트
		 */
		Model::LAIInspectorResult handle_val_inspection(const std::shared_ptr<JobInfo>& job_info);

		/**
		 * @brief 2차 검사기 수행 및 blob 마스크 병합 처리
		 *
		 * @param primary_mask 1차 마스크 이미지
		 * @param job_info 작업 정보
		 * @param results 결과 저장 리스트
		 */
		Model::LAIInspector2Result handle_secondary_inspection(
			const cv::Mat& primary_mask,
			const std::shared_ptr<JobInfo>& job_info);

		/**
		 * @brief 2차 검사 결과 마스크들을 로컬에 저장
		 *
		 * @param blob_mask_list class_idx → 마스크 맵
		 * @param job_info 작업 정보
		 */
		void save_secondary_masks(
			const std::map<int, cv::Mat>& blob_mask_list,
			const std::shared_ptr<JobInfo>& job_info);

		cv::Mat handle_tertiary_inspection(
			int defect_type_idx,
			const cv::Mat& mask,
			const std::shared_ptr<JobInfo>& job_info);

		cv::Mat handle_tertiary_inspection_local(
			int defect_type_idx,
			const cv::Mat& mask,
			const std::shared_ptr<JobInfo>& job_info);

		Model::LAIInspector3Result handle_tertiary_inspection(
			const cv::Mat& primary_mask,
			const std::shared_ptr<JobInfo>& job_info);

		Model::LAIInspector3Result handle_tertiary_inspection(
			const cv::Mat& primary_mask,
			const Model::LAIInspector2Result& inspector2_result,
			const std::shared_ptr<JobInfo>& job_info);

		Model::LAIInspector3Result handle_tertiary_inspection_local(
			const cv::Mat& primary_mask,
			const std::shared_ptr<JobInfo>& job_info);

		Model::LAIInspector3Result handle_tertiary_inspection_local(
			const cv::Mat& primary_mask,
			const Model::LAIInspector2Result& inspector2_result,
			const std::shared_ptr<JobInfo>& job_info);

		void save_tertiary_masks(
			const std::map<int, cv::Mat>& blob_mask_list,
			const std::shared_ptr<JobInfo>& job_info);

		static std::optional<cv::Mat> tertiary_result_mask_filtering(
			const cv::Mat& primary_mask, 
			const cv::Mat& tertiary_adj_mask,
			int compare = 1);

		Model::LAIInspectorSVMResult handle_svm_inspection(
			const cv::Mat& primary_mask,
			const std::shared_ptr<JobInfo>& job_info);

		void save_svm_masks(
			const cv::Mat& ng_mask,
			const cv::Mat& ok_mask,
			const std::shared_ptr<JobInfo>& job_info);

		void test_secondary_inspection_with_random_data();

		void tertiary_result_visualrising(
			const std::shared_ptr<JobInfo>& job_info,
			const std::vector<AIService::ImageUtile::PatchInfo>& patch_image_list,
			const cv::Mat& restore_image,
			const cv::Mat& mask);

		/**
		 * @brief submit 요청 시 model reload 진행 중인지 확인하고 대기
		 */
		void wait_for_model_reload_completion();

	private:
		std::shared_ptr<LAIInspector> primary_inspector_;   ///< 1차 검사기
		std::shared_ptr<LAIInspector2> secondary_inspector_; ///< 2차 검사기
		std::shared_ptr<LAIInspectorSVM> svm_inspector_; ///< svm 검사기
		CADJClientService& thirdly_inspector_; ///< 3차 통신 Class
		std::shared_ptr<LAIInspectorSEG> seg_inspector_; ///< seg 검사기
		std::shared_ptr<LAIInspectorVAL> val_inspector_; ///< val 검사기

		std::shared_ptr<AIService::ThreadPool> thread_pool_; ///< 내부 쓰레드 풀
		std::shared_ptr<AIService::ThreadPool> save_image_thread_pool_; ///< 내부 이미지 저장 쓰레드 풀
		std::shared_ptr<AIService::WatchdogTimer> secondary_watchdog_timer_; /// 2 차 검사를 이용하여 GPU 깨우기

		std::atomic<uint64_t> job_id_counter_ = 1; ///< 작업 ID 생성기

		std::shared_ptr<spdlog::logger> _logger; ///< 로깅 인스턴스
		std::atomic<size_t> job_counter_ = 0;    ///< 작업 수 카운터

		const bool is_use_segmentation_visualization_ = false; ///< 세그먼테이션 시각화 사용 여부

		// Model reload 동기화를 위한 멤버 변수들
		mutable std::mutex model_reload_mutex_;                    ///< model reload 동기화 뮤텍스
		mutable std::condition_variable model_reload_cv_;          ///< model reload 조건 변수
		std::atomic<bool> is_model_reloading_{false};             ///< model reload 진행 중 플래그
	};

}
#endif // LGITAI