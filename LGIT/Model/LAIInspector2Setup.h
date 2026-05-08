#pragma once
#include <fstream>      
#include <iostream>   
#include <string>      
#include <nlohmann/json.hpp>

namespace LGIT::Model {


	struct LAIInspector2Setup
	{
	public:
		static constexpr const char* kThreadNumberKey = "thread_number";
		static constexpr const char* kLogLevelKey = "log_level";
		static constexpr const char* kInspectorModelDirKey = "inspector_model_dir";

	public:
		int thread_number = 1;
		int log_level = 1;
		std::string inspector_model_dir;

		static LAIInspector2Setup from_json(const nlohmann::json& json_obj) {
			LAIInspector2Setup setup;
			setup.thread_number = json_obj.at(kThreadNumberKey).get<int>();
			setup.log_level = json_obj.at(kLogLevelKey).get<int>();
			setup.inspector_model_dir = json_obj.at(kInspectorModelDirKey).get<std::string>();
			return setup;
		}

		static LAIInspector2Setup try_from_json(const nlohmann::json& json_obj) {
			LAIInspector2Setup setup;
			try {
				if (json_obj.contains(kThreadNumberKey) && json_obj[kThreadNumberKey].is_number_integer()) {
					setup.thread_number = json_obj[kThreadNumberKey].get<int>();
				}
				if (json_obj.contains(kLogLevelKey) && json_obj[kLogLevelKey].is_number_integer()) {
					setup.log_level = json_obj[kLogLevelKey].get<int>();
				}
				if (json_obj.contains(kInspectorModelDirKey) && json_obj[kInspectorModelDirKey].is_string()) {
					setup.inspector_model_dir = json_obj[kInspectorModelDirKey].get<std::string>();
				}
			}
			catch (const std::exception& e) {
				std::cerr << "[LAIInspector2Setup::try_from_json] JSON 파싱 예외: " << e.what() << std::endl;
				// 예외 발생 시 기본값 유지
			}
			return setup;
		}

		nlohmann::json to_json() const {
			nlohmann::json json_obj;
			json_obj["thread_number"] = thread_number;
			json_obj["log_level"] = log_level;
			json_obj["inspector_model_dir"] = inspector_model_dir;
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

		static LAIInspector2Setup load_from_file(const std::string& path) {
			std::ifstream file(path, std::ios::binary);
			if (!file.is_open()) {
				std::cerr << "Failed to open file for reading: " << path << std::endl;
				return LAIInspector2Setup{}; // 기본값 리턴
			}

			try {
				nlohmann::json json_obj;
				file >> json_obj;
				return try_from_json(json_obj); // 내부에서 유효성 체크하며 파싱
			}
			catch (const std::exception& e) {
				std::cerr << "[LAIInspector2Setup::load_from_file] JSON 파싱 오류: " << e.what() << std::endl;
				return LAIInspector2Setup{}; // 예외 시 기본값 리턴
			}
		}
	};
}