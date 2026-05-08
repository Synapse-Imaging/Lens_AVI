#pragma once

#include <tbb/concurrent_unordered_map.h>
#include <vector>
#include <memory>
#include <iostream>
#include "LRequestedInspectionData1.h"

namespace LGIT::Model {
	class ImageROIManager {
	public:
		// ROI 데이터 추가 (thread_id → image_index 순서)
		void InsertData(int thread_id, int image_index, LRequestedInspectionData1Ptr roi_data);

		// 특정 thread_id의 특정 image_index에 대한 ROI 데이터 가져오기
		bool GetROIs(int thread_id, int image_index, std::vector<LRequestedInspectionData1Ptr>& output) const;

		// 특정 thread_id의 모든 image_index 및 ROI 데이터 가져오기
		bool GetAllROIs(int thread_id, std::vector<std::pair<int, std::vector<LRequestedInspectionData1Ptr>>>& output) const;

		// 특정 thread_id 및 image_index에 대한 데이터 삭제
		void RemoveImageData(int thread_id, int image_index);

		// 특정 thread_id의 모든 데이터 삭제
		void RemoveThreadData(int thread_id);

		// 특정 thread_id와 image_index의 모든 ROI 데이터를 사용하여 불량 영역(Defect Region) 생성
		bool GenerateDefectRegion(int thread_id, int image_index, HalconCpp::HObject& out_defect_region) const;

	private:
		// Key: `thread_id`, Value: `image_index 별 ROI 데이터 목록`
		tbb::concurrent_unordered_map<int, tbb::concurrent_unordered_map<int, std::vector<LRequestedInspectionData1Ptr>>> _thread_data_map;
	};
}
