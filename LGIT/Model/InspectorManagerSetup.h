#pragma once
#include <vector>                 
#include <fstream>      
#include <iostream>   
#include <string>      
#include <nlohmann/json.hpp>
#include <filesystem>
#include "LAIInspectorSetup.h"
#include "LAIInspector2Setup.h"

namespace LGIT::Model {
	struct InspectorManagerSetup {
	public:
		static constexpr const char* kThreadNumberKey = "thread_number";
		static constexpr const char* kLogLevelKey = "log_level";
		static constexpr const char* kUseSegmentationVisualization = "is_use_segmentation_visualization";
		static constexpr const char* kInspector1Setup = "inspector1_setup";
		static constexpr const char* kInspector2Setup = "inspector2_setup";
		static constexpr const char* kInspectorSVMSetup = "inspector_svm_setup";
		static constexpr const char* kInspectorSEGSetup = "inspector_seg_setup";
		static constexpr const char* kInspectorVALSetup = "inspector_val_setup";

	public:
		int thread_number = 1;
		int log_level = 1;
		bool is_use_segmentation_visualization = false; // 세그먼테이션 시각화 사용 여부
		LAIInspectorSetup inspector_setup;
		LAIInspector2Setup inspector2_setup{ 1, 1, "C:\\lgit\\model2_setup" };
		LAIInspector2Setup inspector_svm_setup{1, 1, "C:\\lgit\\model_svm_setup" };
		LAIInspector2Setup inspector_seg_setup{1, 1, "C:\\lgit\\model_seg_setup" };
		LAIInspector2Setup inspector_val_setup{1, 1, "C:\\lgit\\model_val_setup" };

		void reset_inspector1_model_list(const std::vector<std::filesystem::path>& model_list) {
			inspector_setup.model_path_list.clear();
			for (auto& model_path : model_list) {
				inspector_setup.model_path_list.emplace_back(model_path.string());
			}
		}

		static InspectorManagerSetup from_json(const nlohmann::json& json_obj) {
			InspectorManagerSetup setup;
			setup.thread_number = json_obj.at(kThreadNumberKey).get<int>();
			setup.log_level = json_obj.at(kLogLevelKey).get<int>();
			setup.is_use_segmentation_visualization = json_obj.at(kUseSegmentationVisualization).get<bool>();
			setup.inspector_setup = LAIInspectorSetup::from_json(json_obj[kInspector1Setup]);
			setup.inspector2_setup = LAIInspector2Setup::from_json(json_obj[kInspector2Setup]);
			setup.inspector_svm_setup = LAIInspector2Setup::from_json(json_obj[kInspectorSVMSetup]);
			setup.inspector_seg_setup = LAIInspector2Setup::from_json(json_obj[kInspectorSEGSetup]);
			setup.inspector_val_setup = LAIInspector2Setup::from_json(json_obj[kInspectorVALSetup]);
			return setup;
		}

		static InspectorManagerSetup try_from_json(const nlohmann::json& json_obj) {
			InspectorManagerSetup setup;
			try {
				if (json_obj.contains(kThreadNumberKey) && json_obj[kThreadNumberKey].is_number_integer()) {
					setup.thread_number = json_obj[kThreadNumberKey].get<int>();
				}
				if (json_obj.contains(kLogLevelKey) && json_obj[kLogLevelKey].is_number_integer()) {
					setup.log_level = json_obj[kLogLevelKey].get<int>();
				}
				if (json_obj.contains(kUseSegmentationVisualization) && json_obj[kUseSegmentationVisualization].is_boolean()) {
					setup.is_use_segmentation_visualization = json_obj[kUseSegmentationVisualization].get<int>();
				}
				if (json_obj.contains(kInspector1Setup) && json_obj[kInspector1Setup].is_object()) {
					setup.inspector_setup = LAIInspectorSetup::try_from_json(json_obj[kInspector1Setup]);
				}
				if (json_obj.contains(kInspector2Setup) && json_obj[kInspector2Setup].is_object()) {
					setup.inspector2_setup = LAIInspector2Setup::try_from_json(json_obj[kInspector2Setup]);
				}
				if (json_obj.contains(kInspectorSVMSetup) && json_obj[kInspectorSVMSetup].is_object()) {
					setup.inspector_svm_setup = LAIInspector2Setup::try_from_json(json_obj[kInspectorSVMSetup]);
				}
				else{
					setup.inspector_svm_setup = LAIInspector2Setup{ 1, 1, "C:\\lgit\\model_svm_setup" }; // 기본값 설정
				}
				if (json_obj.contains(kInspectorSEGSetup) && json_obj[kInspectorSEGSetup].is_object()) {
					setup.inspector_seg_setup = LAIInspector2Setup::try_from_json(json_obj[kInspectorSEGSetup]);
				}
				else{
					setup.inspector_seg_setup = LAIInspector2Setup{ 1, 1, "C:\\lgit\\model_seg_setup" }; // 기본값 설정
				}
				if (json_obj.contains(kInspectorVALSetup) && json_obj[kInspectorVALSetup].is_object()) {
					setup.inspector_val_setup = LAIInspector2Setup::try_from_json(json_obj[kInspectorVALSetup]);
				}
				else {
					setup.inspector_val_setup = LAIInspector2Setup{ 1, 1, "C:\\lgit\\model_val_setup" }; // 기본값 설정
				}
			}
			catch (const std::exception& e) {
				// 로깅 또는 디버깅용 메시지
				std::cerr << "[InspectorManagerSetup::try_from_json] JSON 파싱 예외 발생: " << e.what() << std::endl;
				// 예외가 발생해도 기본값을 유지
			}
			return setup;
		}

		nlohmann::json to_json() const {
			nlohmann::json json_obj;
			json_obj[kThreadNumberKey] = thread_number;
			json_obj[kLogLevelKey] = log_level;
			json_obj[kUseSegmentationVisualization] = is_use_segmentation_visualization;
			json_obj[kInspector1Setup] = inspector_setup.to_json();
			json_obj[kInspector2Setup] = inspector2_setup.to_json();
			json_obj[kInspectorSVMSetup] = inspector_svm_setup.to_json();
			json_obj[kInspectorSEGSetup] = inspector_seg_setup.to_json();
			json_obj[kInspectorVALSetup] = inspector_val_setup.to_json();
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

		static InspectorManagerSetup load_from_file(const std::string& path) {
			std::ifstream file(path, std::ios::binary);
			if (!file.is_open()) {
				std::cerr << "Failed to open file for reading: " << path << std::endl;
				return InspectorManagerSetup{}; // 기본값 리턴
			}

			try {
				nlohmann::json json_obj;
				file >> json_obj;
				return try_from_json(json_obj); // 내부에서 유효성 체크하며 파싱
			}
			catch (const std::exception& e) {
				std::cerr << "[InspectorManagerSetup::load_from_file] JSON 파싱 오류: " << e.what() << std::endl;
				return InspectorManagerSetup{}; // 예외 시 기본값 리턴
			}
		}

	};
}