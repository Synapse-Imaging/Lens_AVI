#pragma once
#include <string>
#include <vector>
#include <map>
#include <opencv2/opencv.hpp>
#include <../AIService/ImageUtile.h>

namespace LGIT::Model {

	/**
	 * @brief 2차 검사 결과 구조체
	 *
	 * - blob 단위 분류 결과
	 * - 불량 타입 index 및 마스크 이미지 포함
	 */
	struct LAIInspector2Result
	{
		/**
		 * @brief 검사 성공 여부
		 *
		 * true: 정상 처리됨, false: 실패 또는 무효한 결과
		 */
		bool result = false;

		/**
		 * @brief 검사된 blob 총 개수
		 */
		int blob_count = 0;

		/**
		 * @brief 각 blob의 고유 ID 리스트
		 */
		std::vector<int> blob_id_list;

		/**
		 * @brief 각 blob이 분류된 class 인덱스 리스트
		 */
		std::vector<int> class_idx_list;

		/**
		 * @brief 각 blob의 confidence score (0.0 ~ 1.0)
		 */
		std::vector<float> score_list;

		/**
		 * @brief class index → blob 마스크 매핑 정보
		 *
		 * Defect type index 를 key로, 해당 클래스의 blob 마스크 이미지를 value로 가짐
		 */
		std::map<int, cv::Mat> blob_mask_map;

		/**
		 * @brief 양품 판정 blob의 Primary 마스크 이미지
		 */
		cv::Mat blob_ok_mask; 

		bool is_ng() {
			return result && !blob_mask_map.empty();
		}
	};
}
