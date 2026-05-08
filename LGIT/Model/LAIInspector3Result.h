#pragma once
#include <string>    
#include <vector>
#include <map>
#include <opencv2/opencv.hpp>
#include <../AIService/ImageUtile.h>

namespace LGIT::Model {
	/**
	 * @brief 3차 검사 결과 구조체
	 *
	 * - blob 단위 분류 결과
	 * - 불량 타입 index 및 마스크 이미지 포함
	 */
	struct LAIInspector3Result {
		/**
		 * @brief 검사 성공 여부
		 *
		 * true: 정상 처리됨, false: 실패 또는 무효한 결과
		 */
		bool result = false;
		/**
		 * @brief class index → blob 마스크 매핑 정보
		 *
		 * Defect type index 를 key로, 해당 클래스의 blob 마스크 이미지를 value로 가짐
		 */
		std::map<int, cv::Mat> blob_mask_map;  
	};
}