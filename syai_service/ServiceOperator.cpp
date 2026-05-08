#include "StdAfx.h"
#include "ServiceOperator.h"
#include <map>
#include <memory>
#include <optional>
#include <spdlog/logger.h>
#include <string>
#include <syai/runtime/domain/config/ConfigurationEnum.h>
#include <syai/runtime/domain/config/ManagerInfo.h>
#include <syai/runtime/inspection/inspectionService.h>
#include <syai/runtime/SimpleSmartType.h>
#include <vector>

namespace syai_service
{
	using namespace syai::runtime::inspection;
	using namespace syai::runtime::domain::config;
	using namespace syai::runtime::domain::inspection;

	ServiceOperator::ServiceOperator(std::shared_ptr<spdlog::logger> logger) : _logger(logger)
	{
		_inspection_service = InspectionService();
	}

	ServiceOperator::~ServiceOperator()
	{
		if (_inspection_service.IsInitialized())
		{
			_inspection_service.Finalize();
			_logger->info("[Operator] Service finalized.");
		}
	}

	ServiceStatus ServiceOperator::init(
		std::string syai_log_name,
		std::string syai_log_root,
		LogLevel log_level,
		syai::runtime::SmartVector<ManagerInfo> manager_setups)
	{
		_logger->debug("[Operator] initialize called with log_name='{}', syai_log_root='{}', syai_log_level={}, manager_setups size={}",
					   syai_log_name,
					   syai_log_root,
					   static_cast<int>(log_level),
					   manager_setups.size());

		ServiceStatus init_status = _inspection_service.Initialize(syai_log_name.c_str(), syai_log_root.c_str(), log_level, manager_setups);
		switch (init_status)
		{
		case ServiceStatus::SUCCESS:
			_logger->info("[Operator] Service initialized successfully.");
			_initialized = true;
			break;
		case ServiceStatus::INVALID_CONFIG:
			_logger->error("[Operator] Service initialization failed due to invalid configuration.");
			_initialized = false;
			break;
		case ServiceStatus::MANAGER_NOT_FOUND:
			_logger->error("[Operator] Service initialization failed because a manager was not found.");
			_initialized = false;
			break;
		case ServiceStatus::INITIALIZATION_FAILED:
			_logger->error("[Operator] Service initialization failed due to an internal error.");
			_initialized = false;
			break;
		case ServiceStatus::RUNTIME_ERROR:
			_logger->error("[Operator] Service initialization failed due to a runtime error.");
			_initialized = false;
			break;
		default:
			_logger->error("[Operator] Service initialization failed due to an unknown error.");
			_initialized = false;
			break;
		}
		_manager_names = _inspection_service.GetManagerNames();
		return init_status;
	}

	ServiceStatus ServiceOperator::init_config(std::string config_path)
	{
		_logger->debug("[Operator] initialize config called with config_path='{}'", config_path);
		ServiceStatus init_status = _inspection_service.InitializeFromConfig(config_path.c_str());
		switch (init_status)
		{
		case ServiceStatus::SUCCESS:
			_logger->info("[Operator] Service initialized successfully.");
			_initialized = true;
			break;
		case ServiceStatus::INVALID_CONFIG:
			_logger->error("[Operator] Service initialization failed due to invalid configuration.");
			_initialized = false;
			break;
		case ServiceStatus::MANAGER_NOT_FOUND:
			_logger->error("[Operator] Service initialization failed because a manager was not found.");
			_initialized = false;
			break;
		case ServiceStatus::INITIALIZATION_FAILED:
			_logger->error("[Operator] Service initialization failed due to an internal error.");
			_initialized = false;
			break;
		case ServiceStatus::RUNTIME_ERROR:
			_logger->error("[Operator] Service initialization failed due to a runtime error.");
			_initialized = false;
			break;
		default:
			_logger->error("[Operator] Service initialization failed due to an unknown error.");
			_initialized = false;
			break;
		}
		_manager_names = _inspection_service.GetManagerNames();
		return init_status;
	}

	ServiceStatus ServiceOperator::re_init_config(std::string config_path)
	{
		_logger->debug("[Operator] re-initialize config called with config_path='{}'", config_path);
		ServiceStatus reinit_status = _inspection_service.ReinitializeFromConfig(config_path.c_str());
		if (reinit_status == ServiceStatus::SUCCESS)
		{
			_logger->info("[Operator] Service re-initialized successfully.");
			_initialized = true;
			_manager_names = _inspection_service.GetManagerNames();
			return reinit_status;
		}
		else
		{
			_logger->error("[Operator] Service re-initialization failed with status code: {}", static_cast<int>(reinit_status));
			_initialized = false;
			return reinit_status;
		}

		return ServiceStatus();
	}

	std::vector<std::string> ServiceOperator::get_manager_names()
	{
		auto manager_list = _inspection_service.GetManagerNames();
		std::vector<std::string> names;
		for (size_t i = 0; i < manager_list.size(); i++)
		{
			names.push_back(manager_list.get(i));
		}
		return names;
	}

	std::optional<Response> ServiceOperator::inspection(const Request& request)
	{
		if (!_initialized)
		{
			_logger->warn("[Operator] Inspection request received but service is not initialized.");
			return {}; // Explicit empty optional construction
		}

		_logger->debug("[Operator] Processing inspection request for inspection id: '{}', lot id : '{}'",
					   request.get_inspection_id(),
					   request.get_log_id());

		Response response(request.get_inspection_id(), request.get_log_id());

		if (_inspection_service.InferenceRawMaskImage(request, response)) {
			_logger->info("[Operator] Inspection completed successfully for inspection id: '{}'", request.get_inspection_id());
		}
		else
		{
			_logger->error("[Operator] Inspection failed for inspection id: '{}'", request.get_inspection_id());
		}
		return response;
		// return std::nullopt;
	}

	void ServiceOperator::save_train_image_save(const std::string& save_dir, const cv::Mat& image, const cv::Size& patch_size, int stride, const std::string& prefix)
	{
		_inspection_service.SaveTrainCropImage(save_dir, image, patch_size, stride, prefix);
	}

	void ServiceOperator::save_train_image_save_with_mask(const std::string& save_dir,
														  const cv::Mat& image,
														  const cv::Mat& mask,
														  const cv::Size& patch_size, 
														  int stride, 
														  const std::string& prefix)
	{
		_inspection_service.SaveTrainCropImageWithMask(save_dir, image, mask, patch_size, stride, prefix);
	}

	

	std::map<int, int> ServiceOperator::get_manager_initialization_status(const char* model_key)
	{
		_logger->debug("[Operator] get_manager_initialization_status called with model_key='{}'", model_key);
		std::map<int, int> status_list;
		//auto manager_list = _inspection_service.GetManagerNames();
		//auto model_list = _inspection_service.GetStepOrderForModel(model_key);

		//// manager_list를 순회하여 각 manager가 model_list에서 몇 번째 우선순위인지 확인
		//for (size_t manager_idx = 0; manager_idx < manager_list.size(); manager_idx++)
		//{
		//	std::string manager_name = manager_list.get(manager_idx);
		//	int priority = -1; // 기본값: model_list에 없음

		//	// model_list에서 해당 manager의 우선순위 찾기
		//	for (size_t model_idx = 0; model_idx < model_list.size(); model_idx++)
		//	{
		//		if (manager_name == model_list.get(model_idx))
		//		{
		//			priority = static_cast<int>(model_idx);
		//			break;
		//		}
		//	}

		//	if (priority >= 0)
		//	{
		//		status_list[static_cast<int>(manager_idx)] = priority;
		//		_logger->debug("[Operator] Manager '{}' at index {} has priority {}", manager_name, manager_idx, priority);
		//	}
		//}
		auto model_list = _inspection_service.GetStepOrderForModel(model_key);
		for (size_t model_idx = 0; model_idx < model_list.size(); model_idx++)
		{
			std::string manager_name = model_list.get(model_idx);
			status_list[model_idx] = static_cast<int>(model_idx);
			_logger->debug("[Operator] Manager '{}' has priority {}", manager_name, model_idx);
		}
		return status_list;
	}
}


