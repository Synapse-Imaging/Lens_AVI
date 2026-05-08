#pragma once
#include <vector>                 
#include <fstream>      
#include <iostream>   
#include <string>      
#include <nlohmann/json.hpp>


namespace LGIT::Model {
	struct LAIInspectorSetup
	{
	public:
		static constexpr const char* kThreadNumberKey = "thread_number";
		static constexpr const char* kLogLevelKey = "log_level";
		static constexpr const char* kModelPathListKey = "model_path_list";
		static constexpr const char* kInspectorSetupFilePathKey = "inspector_setup_file_path";

	public:
		int thread_number = 1;
		int log_level = 1;
		std::vector<std::string> model_path_list;
		std::string inspector_setup_file_path;

		static LAIInspectorSetup from_json(const nlohmann::json& json_obj) {
			LAIInspectorSetup setup;
			setup.thread_number = json_obj.at(kThreadNumberKey).get<int>();
			setup.log_level = json_obj.at(kLogLevelKey).get<int>();
			setup.model_path_list = json_obj.at(kModelPathListKey).get<std::vector<std::string>>();
			setup.inspector_setup_file_path = json_obj.at(kInspectorSetupFilePathKey).get<std::string>();
			return setup;
		}

		static LAIInspectorSetup try_from_json(const nlohmann::json& json_obj) {
			LAIInspectorSetup setup;
			try {
				if (json_obj.contains(kThreadNumberKey) && json_obj[kThreadNumberKey].is_number_integer()) {
					setup.thread_number = json_obj[kThreadNumberKey].get<int>();
				}
				if (json_obj.contains(kLogLevelKey) && json_obj[kLogLevelKey].is_number_integer()) {
					setup.log_level = json_obj[kLogLevelKey].get<int>();
				}
				if (json_obj.contains(kModelPathListKey) && json_obj[kModelPathListKey].is_array()) {
					setup.model_path_list = json_obj[kModelPathListKey].get<std::vector<std::string>>();
				}
				if (json_obj.contains(kInspectorSetupFilePathKey) && json_obj[kInspectorSetupFilePathKey].is_string()) {
					setup.inspector_setup_file_path = json_obj[kInspectorSetupFilePathKey].get<std::string>();
				}
			}
			catch (const std::exception& e) {
				// 로깅 또는 디버깅용 메시지
				std::cerr << "[LAIInspectorSetup::try_from_json] JSON 파싱 예외 발생: " << e.what() << std::endl;
				// 예외가 발생해도 기본값을 유지
			}
			return setup;
		}

		nlohmann::json to_json() const {
			nlohmann::json json_obj;
			json_obj[kThreadNumberKey] = thread_number;
			json_obj[kLogLevelKey] = log_level;
			json_obj[kModelPathListKey] = model_path_list;
			json_obj[kInspectorSetupFilePathKey] = inspector_setup_file_path;
			return json_obj;
		}

		bool save_to_file(const std::string& path) const {
			std::ofstream file(path, std::ios::binary); // utf-8 - json
			if (!file.is_open()) {
				std::cerr << "Failed to open file for writing: " << path << std::endl;
				return false;
			}
			file << to_json().dump(4);
			file.close();
			return true;
		}

		static LAIInspectorSetup load_from_file(const std::string& path) {
			std::ifstream file(path, std::ios::binary);
			if (!file.is_open()) {
				std::cerr << "Failed to open file for reading: " << path << std::endl;
				return LAIInspectorSetup{}; // 기본값 리턴
			}

			try {
				nlohmann::json json_obj;
				file >> json_obj;
				return try_from_json(json_obj); // 내부에서 유효성 체크하며 파싱
			}
			catch (const std::exception& e) {
				std::cerr << "[LAIInspectorSetup::load_from_file] JSON 파싱 오류: " << e.what() << std::endl;
				return LAIInspectorSetup{}; // 예외 시 기본값 리턴
			}
		}
	};
}