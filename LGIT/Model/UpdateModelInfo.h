#pragma once
#include <vector>                 
#include <fstream>      
#include <iostream>   
#include <string>      
#include <nlohmann/json.hpp>

namespace LGIT::Model {
	struct UpdateModelInfo
	{
	public:
		static constexpr const char* kUpdatedModelsKey = "updated_models";

	public:
		std::vector<std::string> updated_models;

		static UpdateModelInfo from_json(const nlohmann::json& json_obj) {
			UpdateModelInfo info;
			info.updated_models = json_obj.at(kUpdatedModelsKey).get<std::vector<std::string>>();
			return info;
		}

		static UpdateModelInfo try_from_json(const nlohmann::json& json_obj) {
			UpdateModelInfo info;
			try {
				if (json_obj.contains(kUpdatedModelsKey) && json_obj[kUpdatedModelsKey].is_array()) {
					info.updated_models = json_obj[kUpdatedModelsKey].get<std::vector<std::string>>();
				}
			}
			catch (const std::exception& e) {
				// 로그 또는 에러 메시지
				std::cerr << "[UpdateModelInfo::try_from_json] JSON 파싱 오류 발생: " << e.what() << std::endl;
				// 예외가 발생해도 기본값으로 계속
			}
			return info;
		}

		nlohmann::json to_json() const {
			nlohmann::json json_obj;
			json_obj[kUpdatedModelsKey] = updated_models;
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

		static UpdateModelInfo load_from_file(const std::string& path) {
			std::ifstream file(path, std::ios::binary);
			if (!file.is_open()) {
				std::cerr << "Failed to open file for reading: " << path << std::endl;
				return UpdateModelInfo{}; // 기본값 반환
			}

			try {
				nlohmann::json json_obj;
				file >> json_obj;
				return try_from_json(json_obj); // 안전하게 유효성 체크하며 파싱
			}
			catch (const std::exception& e) {
				std::cerr << "[UpdateModelInfo::load_from_file] JSON 파싱 오류: " << e.what() << std::endl;
				return UpdateModelInfo{}; // 오류 시 기본값 반환
			}
		}

		// 편의 메서드들
		bool is_empty() const {
			return updated_models.empty();
		}

		size_t size() const {
			return updated_models.size();
		}

		bool contains_model(const std::string& model_name) const {
			return std::find(updated_models.begin(), updated_models.end(), model_name) != updated_models.end();
		}

		void add_model(const std::string& model_name) {
			if (!contains_model(model_name)) {
				updated_models.push_back(model_name);
			}
		}

		void remove_model(const std::string& model_name) {
			updated_models.erase(
				std::remove(updated_models.begin(), updated_models.end(), model_name),
				updated_models.end()
			);
		}

		void clear() {
			updated_models.clear();
		}
	};
}