#pragma once
#include <map>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <optional>
#include <spdlog/logger.h>
#include <string>
#include <syai/runtime/domain/inspection/Request.h>
#include <syai/runtime/domain/inspection/Response.h>
#include <syai/runtime/inspection/inspectionService.h>
#include <syai/runtime/SimpleSmartType.h>
#include <vector>

namespace syai_service
{
	class ServiceOperator
	{
	public:
		ServiceOperator(std::shared_ptr<spdlog::logger> logger);
		~ServiceOperator();

		syai::runtime::inspection::ServiceStatus init(std::string syai_log_name,
													  std::string syai_log_root,
													  syai::runtime::domain::config::LogLevel log_level,
													  syai::runtime::SmartVector<syai::runtime::domain::config::ManagerInfo> manager_setups);

		syai::runtime::inspection::ServiceStatus init_config(std::string config_path);
		syai::runtime::inspection::ServiceStatus re_init_config(std::string config_path);

		std::vector<std::string> get_manager_names();

		std::optional<syai::runtime::domain::inspection::Response> inspection(const syai::runtime::domain::inspection::Request& request);

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
	private:
		bool _initialized = false;
		std::shared_ptr<spdlog::logger> _logger;
		syai::runtime::SmartList _manager_names;
		syai::runtime::inspection::InspectionService _inspection_service;

	}; // Fixed: Added the missing semicolon
}