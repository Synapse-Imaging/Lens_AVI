#include "stdafx.h"
#include "ImageROIManager.h"

namespace LGIT::Model {

	// ROI 데이터 추가 (thread_id → image_index 순서)
	void ImageROIManager::InsertData(int thread_id, int image_index, LRequestedInspectionData1Ptr roi_data) {
		auto& image_map = _thread_data_map[thread_id];  // `thread_id` 별 데이터 접근
		auto& roi_list = image_map[image_index];  // `image_index` 별 데이터 접근
		roi_list.push_back(roi_data);
	}

	// 특정 thread_id의 특정 image_index에 대한 ROI 데이터 가져오기
	bool ImageROIManager::GetROIs(int thread_id, int image_index, std::vector<LRequestedInspectionData1Ptr>& output) const {
		auto thread_it = _thread_data_map.find(thread_id);
		if (thread_it != _thread_data_map.end()) {
			auto image_it = thread_it->second.find(image_index);
			if (image_it != thread_it->second.end()) {
				output = image_it->second;
				return true;
			}
		}
		return false;
	}

	// 특정 thread_id의 모든 image_index 및 ROI 데이터 가져오기
	bool ImageROIManager::GetAllROIs(int thread_id, std::vector<std::pair<int, std::vector<LRequestedInspectionData1Ptr>>>& output) const {
		auto thread_it = _thread_data_map.find(thread_id);
		if (thread_it != _thread_data_map.end()) {
			for (const auto&[image_index, roi_list] : thread_it->second) {
				output.emplace_back(image_index, roi_list);
			}
			return true;
		}
		return false;
	}

	// 특정 thread_id 및 image_index에 대한 데이터 삭제
	void ImageROIManager::RemoveImageData(int thread_id, int image_index) {
		auto thread_it = _thread_data_map.find(thread_id);
		if (thread_it != _thread_data_map.end()) {
			thread_it->second.unsafe_erase(image_index);
		}
	}

	// 특정 thread_id의 모든 데이터 삭제
	void ImageROIManager::RemoveThreadData(int thread_id) {
		_thread_data_map.unsafe_erase(thread_id);
	}

	// 특정 thread_id와 image_index의 모든 ROI 데이터를 사용하여 불량 영역(Defect Region) 생성
	bool ImageROIManager::GenerateDefectRegion(int thread_id, int image_index, HalconCpp::HObject& out_defect_region) const {
		auto thread_it = _thread_data_map.find(thread_id);
		if (thread_it == _thread_data_map.end()) return false;

		auto image_it = thread_it->second.find(image_index);
		if (image_it == thread_it->second.end()) return false;

		// 빈 Region 생성
		HalconCpp::GenEmptyObj(&out_defect_region);

		HalconCpp::HObject temp_region, union_region;
		HalconCpp::GenEmptyObj(&union_region);

		// 모든 ROI 데이터에 대해 GenerateDefectRegion 실행
		for (const auto& roi_data : image_it->second) {
			HalconCpp::HObject roi_defect;
			if (roi_data->GenerateDefectRegion(roi_defect)) {
				HalconCpp::Union2(union_region, roi_defect, &union_region);
			}
		}

		out_defect_region = union_region;
		return true;
	}

}
