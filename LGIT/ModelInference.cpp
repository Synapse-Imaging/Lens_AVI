#include "stdafx.h"
#ifdef LGITAI
#include "ModelInference.h"
#include <chrono>
#include <exception>
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <thread>

namespace LGIT {
	using namespace HalconCpp;

	ModelInference::ModelInference(
		const std::vector<std::string>& model_path_list, 
		const std::string& setup_json_path, 
		const int inference_engine_logging_level, 
		int div_type_idx) :
		_model_path_list(model_path_list), 
		_setup_json_path(setup_json_path),
		_inference_engine_logging_level(inference_engine_logging_level)
	{
		if (div_type_idx >= _div_type_list.size()) { div_type_idx = 1; }

		_inferenceEngineHandle = CreateInferenceEngine(_div_type_list[div_type_idx].data(), div_type_idx - 1, _inference_engine_logging_level);
		if (_inferenceEngineHandle == nullptr) { throw std::runtime_error("엔진 생성 실패"); }

		try {
			InferenceEngine_LoadThresholds(_inferenceEngineHandle, _setup_json_path.c_str()); // json 파일 읽기
		}
		catch (...) {
			throw std::runtime_error(" json 파일 읽기 실패");
		}
		
		for (const std::string& model_path : model_path_list) {
			try {
				InferenceEngine_LoadMemory(_inferenceEngineHandle, model_path.c_str()); // model 
			}
			catch (...) {
				throw std::runtime_error("model 로드 실패: " + model_path);
			}
		}
	}

	ModelInference::~ModelInference() {
		DestroyInferenceEngine(_inferenceEngineHandle);
	}

	//std::vector<float> ModelInference::RunInference(const HalconCpp::HObject& h_image, UINT64& image_conv_duration, UINT64& inference_duration) 
	//{
	//	//if (!_inference_func) {
	//	//    throw std::runtime_error("Inference function not loaded");
	//	//}
	//	using namespace std::chrono;
	//	auto f1 = high_resolution_clock::now();
	//	cv::Mat cv_image = ModelInference::HImage2Mat(h_image);
	//	auto f2 = high_resolution_clock::now();
	//	float* output_tensor_values;
	//	size_t output_tensor_size;
	//	InferenceEngine_Run(_inferenceEngineHandle, _result_patch_size.first, _result_patch_size.second, cv_image, output_tensor_values, output_tensor_size, true);
	//	auto f3 = high_resolution_clock::now();
	//	image_conv_duration = duration_cast<milliseconds>(f2 - f1).count();
	//	inference_duration = duration_cast<milliseconds>(f3 - f2).count();

	//	std::vector<float> data(output_tensor_values, output_tensor_values + output_tensor_size);
	//	// delete[] output_tensor_values;
	//	return data;
	//}

	Model::LAIInspectorResult ModelInference::RunInference(
		const std::string& product_name,
		const int vision_pos,
		const int image_no,
		const int roi_no,
		const int roi_inspection_type_idx,
		const cv::Mat cv_image,
		UINT64& inference_duration,
		bool test_mode)
	{
		using namespace std::chrono;

		float* output_tensor_values = nullptr;
		size_t output_tensor_size = 0;
		cv::Mat mask_image;

		auto t_start = high_resolution_clock::now();
		std::string key = MakeInferenceEngineRunMemoryKey(product_name, vision_pos, image_no, roi_no, roi_inspection_type_idx);
		if (test_mode) { key = "BC1_IMG1_ROI2.bin"; }

		InferenceEngine_Run(
			_inferenceEngineHandle,
			cv_image,
			output_tensor_values,
			output_tensor_size,
			false,
			key.c_str(),
			mask_image);

		auto t_end = high_resolution_clock::now();
		inference_duration = duration_cast<milliseconds>(t_end - t_start).count();

		std::vector<float> data(output_tensor_values, output_tensor_values + output_tensor_size);

		//try {
		//	double minVal = -1;
		//	double maxVal = -1;
		//	if (mask_image.channels() == 1) {
		//		// 1채널(그레이스케일)
		//		cv::Point minLoc, maxLoc;
		//		cv::minMaxLoc(mask_image, &minVal, &maxVal, &minLoc, &maxLoc);
		//	}
		//	else if (mask_image.channels() == 3) {
		//		// 3채널(BGR)
		//		cv::Point minLoc, maxLoc;
		//		std::vector<cv::Mat> channels;
		//		cv::split(mask_image, channels); // B, G, R 순서로 분리됨
		//		const char* channel_names[] = { "Blue", "Green", "Red" };
		//		for (int i = 0; i < 3; ++i) {
		//			cv::minMaxLoc(channels[i], &minVal, &maxVal, &minLoc, &maxLoc);
		//		}
		//	}
		//	else {
		//		minVal = 0;
		//		maxVal = 0;
		//	}

		//	// maxVal 출력
		//	std::ostringstream oss1;
		//	oss1 << "[# Mask Max] " << maxVal << "\n";
		//	OutputDebugStringA(oss1.str().c_str());

		//	// data 전체 출력
		//	std::ostringstream oss2;
		//	oss2 << "[# Result List] ";
		//	for (size_t i = 0; i < data.size(); ++i) {
		//		oss2 << data[i];
		//		if (i != data.size() - 1) oss2 << ", ";
		//	}
		//	oss2 << "\n";
		//	OutputDebugStringA(oss2.str().c_str());
		//}
		//catch (const std::exception& ex) {
		//	// OutputDebugStringA(("Exception in maxVal/data logging: " + std::string(ex.what()) + "\n").c_str());
		//}
		return Model::LAIInspectorResult{ true,std::move(mask_image), data };
	}

	std::string ModelInference::wstring_to_string(const std::wstring& wstr) {
		return std::string(wstr.begin(), wstr.end());
	}

	cv::Mat ModelInference::HImage2Mat(const HObject& h_img)
	{
		cv::Mat cv_img;
		HTuple channels, w, h;
		HObject h_image = h_img;

		HalconCpp::ConvertImageType(h_image, &h_image, "byte");
		HalconCpp::CountChannels(h_image, &channels);

		if (channels.I() == 1) {
			HTuple pointer;
			GetImagePointer1(h_image, &pointer, nullptr, &w, &h);
			int width = w.I(), height = h.I();
			cv_img = cv::Mat(height, width, CV_8UC1, (void*)(pointer.L())).clone();
		}
		else if (channels.I() == 3) {
			HTuple pointer_r, pointer_g, pointer_b;
			HalconCpp::GetImagePointer3(h_image, &pointer_r, &pointer_g, &pointer_b, nullptr, &w, &h);
			int width = w.I(), height = h.I();
			cv::Mat r(height, width, CV_8UC1, (void*)(pointer_r.L()));
			cv::Mat g(height, width, CV_8UC1, (void*)(pointer_g.L()));
			cv::Mat b(height, width, CV_8UC1, (void*)(pointer_b.L()));
			std::vector<cv::Mat> channels_vec = { b, g, r };
			cv::merge(channels_vec, cv_img);
		}
		return cv_img;
	}

	std::map<std::string, float> ModelInference::ReadingUnknownFuckingFile(std::string fuck_file)
	{
		return std::map<std::string, float>();
	}

	std::string ModelInference::MakeInferenceEngineRunMemoryKey(
		const std::string& product_name,
		const int vision_pos,
		const int image_no,
		const int roi_no, 
		const int roi_inspection_type_idx)
	{
		std::string vision_pos_str = product_name;

		switch (vision_pos)
		{
		case 1:
			vision_pos_str += "_BC";
			break;
		default:
			vision_pos_str += "_TC";
			break;
		}
		std::ostringstream oss;
		oss << vision_pos_str
			<< "_Image" << image_no
			<< "_ROI" << roi_no;
		oss << ".bin";
		return oss.str();
	}

	std::vector<std::filesystem::path> ModelInference::GetBinFiles(const std::filesystem::path& folder_path)
	{
		std::vector<std::filesystem::path> bin_files;
		if (!std::filesystem::exists(folder_path) || !std::filesystem::is_directory(folder_path)) {
			return bin_files;
		}
		for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
			if (entry.is_regular_file() && entry.path().extension() == ".bin") {
				bin_files.push_back(entry.path());
			}
		}
		return bin_files;
	}
}
#endif // LGITAI
