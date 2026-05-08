#pragma once
#include <atomic>
#include <exception>
#include <filesystem>
#include <future>
#include <memory>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>

#include "../AIService/ThreadPool.h"
#include "../AIService/WatchdogTimer.h"

#include "ServiceOperator.h"

#include <chrono>
#include <map>
#include <opencv2/core/mat.hpp>
#include <spdlog/logger.h>
#include <syai/runtime/domain/config/ConfigurationEnum.h>
#include <syai/runtime/domain/config/ManagerInfo.h>
#include <syai/runtime/domain/inspection/InspectionStepResult.h>
#include <syai/runtime/domain/inspection/Request.h>
#include <syai/runtime/domain/inspection/Response.h>
#include <syai/runtime/SimpleSmartType.h>
#include <vector>

// Forward declarations for Halcon types
namespace HalconCpp {
	class HObject;
	class HTuple;
}
using HalconCpp::HObject;
using HalconCpp::HTuple;

// Forward declarations
class Algorithm;
class GTRegion;

namespace syai_service
{
	struct JobInfoAddon
	{
		std::string file_name_prefix;
		int vision_inspection_thread_number = -1;
		int vision_cam_type = -1;
		int magazine_no = -1;
		int image_index = -1;
		int roi_index = -1;
		int inspection_tab_index = -1;

		std::string eq_id;
		std::string lot_id;
		std::string barcode_id;
	};

	struct PreprocessingJobInput
	{
		Algorithm* algorithm = nullptr;
		GTRegion* gt_region = nullptr;
		HObject roi_reg;
		HTuple length1;
		HTuple length2;
		HObject defect_reg;
		std::string lot_id;
		std::string eq_id;
		std::string barcode_id;
		std::string tray_id;
		std::string prefix;
		std::string camera_type;
		std::string inspection_type;
		std::string defect_name;
		std::string defect_type;
		bool enable_roi_processing = false;
		int vision_cam_type = 0;
		int buffer_idx = 0;
		int magazine_no = -1;
		int image_idx = 0;
		int roi_idx = 0;
		int tab_idx = 0;
		std::string save_prefix;
		bool enable_save_roi_image = false;
		int save_roi_type = -1;
		std::vector<std::string> save_dir_list;
		bool enable_inspection_ai = false;
	};

	struct JobInfo
	{
		std::optional<JobInfoAddon> addon;
		std::optional<syai::runtime::domain::inspection::Request> request;
		std::optional<syai::runtime::domain::inspection::Response> response;

		void release()
		{
			try {
				if (request.has_value()) {
					request.reset();
				}
				if (response.has_value()) {
					response.reset();
				}
			}
			catch (...) {}
		}

		cv::Mat get_last_step_mask() const
		{
			if (response.has_value()) {
				auto& resp = response.value();
				auto& setp_names = resp.get_step_names();
				for (int i = resp.get_step_count() - 1; i >= 0; --i) {
					auto step_result = resp.get_step_result(setp_names.get(i));
					if (step_result.success) {
						return step_result.mask.clone();
					}
				}
			}
			return cv::Mat();
		}
	};

	struct ResponseResult
	{
		std::promise<JobInfo> response_job;

		void release()
		{
			if (is_released_.exchange(true)) {
				return;
			}

			try {
				JobInfo empty_job;
				response_job.set_value(std::move(empty_job));
			}
			catch (const std::future_error&) {}
			catch (...) {}
		}

		void release_with_exception(std::exception_ptr exception)
		{
			if (is_released_.exchange(true)) {
				return;
			}

			try {
				response_job.set_exception(exception);
			}
			catch (const std::future_error&) {}
			catch (...) {}
		}

		void release_with_error(const std::string& error_message)
		{
			if (is_released_.exchange(true)) {
				return;
			}

			try {
				JobInfo error_job;
				syai::runtime::domain::inspection::Response error_response("error_inspection", "error_log");
				error_response.set_step_result("error", false, error_message.c_str(), -1);
				error_job.response = std::move(error_response);
				response_job.set_value(std::move(error_job));
			}
			catch (const std::future_error&) {}
			catch (...) {}
		}

		bool set_value(JobInfo&& job_info)
		{
			if (is_released_.exchange(true)) {
				return false;
			}

			try {
				response_job.set_value(std::move(job_info));
				return true;
			}
			catch (const std::future_error&) {
				return false;
			}
			catch (...) {
				return false;
			}
		}

		bool is_released() const
		{
			return is_released_.load();
		}

		void force_release()
		{
			is_released_.store(true);
		}

		void release_with_timeout(std::chrono::milliseconds timeout = std::chrono::milliseconds(100))
		{
			if (is_released_.exchange(true)) {
				return;
			}

			std::thread([this, timeout]() {
				std::this_thread::sleep_for(timeout);
				try {
					if (!is_released_.load()) {
						JobInfo empty_job;
						response_job.set_value(std::move(empty_job));
					}
				}
				catch (...) {}
			}).detach();
		}

	private:
		std::atomic<bool> is_released_{ false };
	};

	struct NameBuildOptions
	{
		bool consider_vision_pos = false;
		bool consider_inspection_type = false;
		bool consider_defect_type = false;
		bool consider_image_index = false;
		bool ignore_case = false;
		bool allow_input_duplicates = false;
		bool distinct = false;
	};

	class ServiceDirector
	{
	public:
		ServiceDirector(size_t num_thread_count,
						int logging_level,
						bool is_use_visualization,
						std::filesystem::path log_root,
						size_t syai_num_thread_count);
		~ServiceDirector();

		bool ServiceDirector::initialize_logger(const char* log_name, const char* log_root, int log_level);

		std::vector<std::string> get_managed_names();

		bool init(std::string syai_log_name,
				  std::string syai_log_root,
				  syai::runtime::domain::config::LogLevel log_level,
				  syai::runtime::SmartVector<syai::runtime::domain::config::ManagerInfo> manager_setups);

		bool init_config(std::string config_path);

		bool re_init_config(std::string config_path);

		std::future<JobInfo> add_job(const syai::runtime::domain::inspection::Request& request, const JobInfoAddon& job_info_addon);

		std::future<JobInfo> add_job(const PreprocessingJobInput& input);

		void save_train_image_save(const std::string& save_dir,
								   const cv::Mat& image,
								   const cv::Size& patch_size,
								   int stride, 
								   const std::string& prefix);

		void save_train_image_save_with_mask(const std::string& save_dir,
											 const cv::Mat& image,
											 const cv::Mat& mask,
											 const cv::Size& patch_size,
											 int stride,
											 const std::string& prefix);

		std::map<int, int> get_manager_initialization_status(const char* model_key);

		static std::vector<std::string> build_names(const std::vector<std::string>& camera_types,
													const std::vector<std::string>& vision_positions,
													const std::vector<std::string>& inspection_types,
													const std::vector<std::string>& defect_types,
													const NameBuildOptions& opt);

		static std::string build_name(const std::string& camera_type,
									  const std::string& vision_position,
									  const std::string& inspection_type,
									  const std::string& image_index_str,
									  const std::string& defect_type,
									  const NameBuildOptions& opt);

	private:
		// 전처리 파이프라인 helper 함수들 (각 단계별 구현)
		bool preprocess_roi_image_(
			uint64_t job_counter,
			const PreprocessingJobInput& input,
			cv::Mat& out_cv_roi_img,
			cv::Mat& out_cv_roi_img_v2,
			int& out_x1, int& out_y1, int& out_x2, int& out_y2,
			int& out_original_width, int& out_original_height,
			std::string& out_error_msg);

		bool select_ai_inspection_image_(
			uint64_t job_counter,
			const PreprocessingJobInput& input,
			const cv::Mat& cv_roi_img,
			const cv::Mat& cv_roi_img_v2,
			cv::Mat& out_ai_image,
			std::string& out_error_msg);

		bool create_mask_image_(
			uint64_t job_counter,
			const PreprocessingJobInput& input,
			const cv::Mat& ai_image,
			int roi_x1, int roi_y1, int roi_x2, int roi_y2,
			int original_width, int original_height,
			cv::Mat& out_mask_binary,
			cv::Rect& out_roi_rect,
			std::string& out_error_msg);

		bool generate_model_key_(
			uint64_t job_counter,
			const PreprocessingJobInput& input,
			std::string& out_model_key,
			std::string& out_error_msg);

		bool create_request_(
			uint64_t job_counter,
			const PreprocessingJobInput& input,
			const cv::Mat& ai_image,
			const cv::Mat& mask_binary,
			const cv::Rect& roi_rect,
			const std::string& model_key,
			syai::runtime::domain::inspection::Request& out_request,
			std::string& out_error_msg);

		void setup_addon_info_(
			uint64_t job_counter,
			const PreprocessingJobInput& input,
			JobInfoAddon& out_addon);

		std::optional<syai::runtime::domain::inspection::Response> process_image(const syai::runtime::domain::inspection::Request& request);

		std::string get_current_date_string()
		{
			auto now = std::chrono::system_clock::now();
			std::time_t now_c = std::chrono::system_clock::to_time_t(now);

			std::tm tm;
			localtime_s(&tm, &now_c);

			std::ostringstream oss;
			oss << std::put_time(&tm, "%Y%m%d");
			return oss.str();
		}

	private:
		static inline void append_part_(std::string& out, const std::string& part)
		{
			if (part.empty()) return;
			if (!out.empty()) out += "_";
			out += part;
		}

		static inline std::string trim_copy_(const std::string& s)
		{
			auto is_space = [](unsigned char c) { return std::isspace(c) != 0; };
			size_t b = 0;
			while (b < s.size() && is_space(static_cast<unsigned char>(s[b]))) ++b;
			size_t e = s.size();
			while (e > b && is_space(static_cast<unsigned char>(s[e - 1]))) --e;
			return s.substr(b, e - b);
		}

		static inline std::string to_lower_copy_(const std::string& s)
		{
			std::string out;
			out.resize(s.size());
			std::transform(s.begin(), s.end(), out.begin(),
						   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return out;
		}

		static inline bool is_res_number_(const std::string& defect_name)
		{
			if (defect_name.size() < 4) return false;
			std::string head = defect_name.substr(0, 3);
			std::transform(head.begin(), head.end(), head.begin(),
						   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			if (head != "res") return false;
			for (size_t i = 3; i < defect_name.size(); ++i)
				if (!std::isdigit(static_cast<unsigned char>(defect_name[i])))
					return false;
			return true;
		}

		template <typename SkipPredicate>
		static inline std::vector<std::string> sanitize_list_(const std::vector<std::string>& in,
															  SkipPredicate&& skip_predicate,
															  bool ignore_case,
															  bool allow_duplicates)
		{
			std::vector<std::string> out;
			out.reserve(in.size());
			std::unordered_set<std::string> seen;
			if (!allow_duplicates)
				seen.reserve(in.size());
			for (const auto& raw : in) {
				std::string v = trim_copy_(raw);
				if (v.empty()) continue;
				if (skip_predicate(v)) continue;
				if (allow_duplicates) {
					out.push_back(std::move(v));
					continue;
				}
				std::string key = ignore_case ? to_lower_copy_(v) : v;
				if (seen.insert(key).second)
					out.push_back(std::move(v));
			}
			return out;
		}

		static inline bool starts_with_(const std::string& s, const std::string& prefix)
		{
			if (s.size() < prefix.size()) return false;
			return std::equal(prefix.begin(), prefix.end(), s.begin());
		}

		static inline bool starts_with_icase_(const std::string& s, const std::string& prefix_lower)
		{
			if (s.size() < prefix_lower.size()) return false;
			for (size_t i = 0; i < prefix_lower.size(); ++i) {
				const char a = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
				if (a != prefix_lower[i]) return false;
			}
			return true;
		}

	private:
		std::shared_ptr<spdlog::logger> _logger;
		std::shared_ptr<AIService::ThreadPool> thread_pool_;
		std::shared_ptr<AIService::ThreadPool> preprocessing_thread_pool_;
		std::shared_ptr<AIService::ThreadPool> save_image_thread_pool_;
		std::shared_ptr<AIService::WatchdogTimer> secondary_watchdog_timer_;

		std::atomic_uint_fast64_t job_counter_ = 0;
		std::atomic_uint_fast64_t job_id_counter_ = 0;

		std::unique_ptr<ServiceOperator> service_operator_;

		bool is_initialize_completed_ = false;
	};

}
