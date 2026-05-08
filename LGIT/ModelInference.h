#pragma once
#ifdef LGITAI
#include <iostream>
#include <filesystem>
#include <array>
#include <vector>
#include <Windows.h>
#include <string>
#include <utility>
#include <opencv2/opencv.hpp>
#include <halconcpp/HalconCpp.h>
#include "InferenceEngine.h"
#include "Model/LAIInspectorResult.h"

namespace LGIT {
	// Class -> LGIT에서 제공하는 DLL을 수정함 그리고 또 변경될수 있다고 함 
	class ModelInference {
    public:
        // 기본 생성자
		// ModelInference() = delete; // 기본 생성자 삭제 (필요하면 = default로 변경 가능)

        // 매개변수 생성자: DLL 로드 및 함수 초기화
        ModelInference(
			const std::vector<std::string>& model_path_list, 
			const std::string& setup_json_path,
			const int inference_engine_logging_level,
			int div_type_idx = 1);

        // 소멸자: DLL 언로드
        ~ModelInference();

        // 추론 실행 메서드
        // std::vector<float> RunInference(const HalconCpp::HObject& h_image, UINT64& image_conv_duration, UINT64& inference_duration);

		// 추론 실행 메서드 
		Model::LAIInspectorResult RunInference(
			const std::string& product_name,
			const int vision_pos,
			const int image_no,
			const int roi_no,
			const int roi_inspection_type_idx,
			const cv::Mat cv_image,
			UINT64& inference_duration,
			bool test_mode = true);
	public:
		// Match Key 생성해주기 (변경되면 변경) 
		static std::string MakeInferenceEngineRunMemoryKey(
			const std::string& product_name,
			const int vision_pos,
			const int image_no,
			const int roi_no,
			const int roi_inspection_type_idx);

		static std::vector<std::filesystem::path> GetBinFiles(const std::filesystem::path& folder_path);

	private:        
		// 유틸리티: wstring을 string으로 변환
		std::string wstring_to_string(const std::wstring& wstr);
		// HImageToMat
		static cv::Mat HImage2Mat(const HalconCpp::HObject& h_img);
		// 알수 없는 패기물 읽기 (예정) 왜 읽어야 하지
		static std::map<std::string, float> ReadingUnknownFuckingFile(std::string fuck_file);
			
    private:
		InferenceEngineHandle _inferenceEngineHandle; // 검사 엔진이라고 하는 무언가 아마 OpenVINO Session과 연결하는 무언가 인듯하다
		const std::vector<std::string> _model_path_list; // 메모리 경로들
		const std::string _setup_json_path; // 임계치 정의한 json 
		const int _inference_engine_logging_level; // MFC 프로젝트 라고 하였는데 Consol CMD에 찍는 로깅
		std::map<std::string, float> _model_thresholds; // model thresholds (key, model thresholds) Json이 변경되면 또 변경

		inline static const std::array<std::string, 7> _div_type_list = {
			"GPU.0",
			"GPU.1",
			"GPU.2",
			"GPU.3",
			"CPU",
			"GPU",
			"NPU"
		};
    };
}
#endif // LGITAI