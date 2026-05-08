#pragma once
#include <string>    
#include <opencv2/opencv.hpp>
#include <vector>
#include <../AIService/ImageUtile.h>

namespace LGIT::Model {
	struct LAIInspectorResult
	{
		bool result = false;
		cv::Mat mask;
		std::vector<float> result_list;
	};
}