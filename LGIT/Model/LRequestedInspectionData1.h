#pragma once
#include <vector>
#include <string>
#include <future>
#include <utility>
#include <memory>
#include "HalconCpp.h"
#include "BoundingBox.h"

namespace LGIT::Model {
	class LRequestedInspectionData1
	{
	public:
		// 생성자
		LRequestedInspectionData1(
			int image_index,
			int roi_index,
			std::vector<std::future<std::vector<float>>> result_futures,
			const std::pair<int, int>& result_patch_size,
			int result_threshold,
			const std::pair<int, int>& inspect_original_image_size,
			const BoundingBox& inspect_bounding_box)
			: _image_index(image_index),
			_roi_index(roi_index),
			_result_futures(std::move(result_futures)), // 이동 연산 적용
			_inspect_patch_size(result_patch_size),
			_result_threshold(result_threshold),
			_original_image_size(inspect_original_image_size),
			_roi_bounding_box(inspect_bounding_box)
		{}

		~LRequestedInspectionData1() = default;

		LRequestedInspectionData1(const LRequestedInspectionData1&) = delete;
		LRequestedInspectionData1& operator=(const LRequestedInspectionData1&) = delete;

		LRequestedInspectionData1(LRequestedInspectionData1&&) noexcept = default;
		LRequestedInspectionData1& operator=(LRequestedInspectionData1&&) noexcept = default;

		// Getter
		int GetImageIndex() const { return _image_index; }
		int GetROIIndex() const { return _roi_index; }
		const std::pair<int, int> GetResultPatchSize() const { return _inspect_patch_size; }
		int GetResultThreshold() const { return _result_threshold; }

		// BoundingBox Getter (복사 방지)
		const BoundingBox& GetBoundingBox() const { return _roi_bounding_box; }

		// Get result 결과 리턴 & 리턴 값을 동기적으로 가져오도록 시도
		bool GetResult(std::vector<std::vector<float>>& output) {
			if (_is_process_result_futures) {
				output = _process_result_futures; // 이미 처리된 결과를 반환
				return true;
			}

			if (_result_futures.empty()) {
				return false; // 처리할 데이터가 없음
			}

			_process_result_futures.clear(); // 기존 결과 초기화

			for (auto& future : _result_futures) {
				auto result = future.get(); // Future 결과 가져오기 (동기화)
				_process_result_futures.push_back(std::move(result)); // 이동 연산으로 성능 최적화
			}

			_is_process_result_futures = true;
			output = _process_result_futures; // 결과를 외부 변수에 전달
			return true;
		}

		// 불량 위치를 생성하고 Halcon Region을 반환하는 함수
		bool GenerateDefectRegion(HalconCpp::HObject& out_defect_region) {
			// 결과 데이터를 동기적으로 가져오기
			if (!_is_process_result_futures) {
				if (!GetResult(_process_result_futures)) {
					return false; // 결과 데이터를 가져오지 못하면 false 반환
				}
			}

			HalconCpp::GenEmptyObj(&out_defect_region); // 빈 객체 초기화

			int patch_width = _inspect_patch_size.first;
			int patch_height = _inspect_patch_size.second;
			int stride = _image_stride;  // 패치 간 간격

			// 원본 이미지 크기
			int img_width = _original_image_size.first;
			int img_height = _original_image_size.second;

			// ROI BoundingBox 정보를 사용하여 원본 이미지 기준 좌표 변환
			int roi_left = _roi_bounding_box.left;
			int roi_top = _roi_bounding_box.top;
			int roi_right = _roi_bounding_box.right;
			int roi_bottom = _roi_bounding_box.bottom;

			// ROI 크기 확인
			int roi_width = roi_right - roi_left;
			int roi_height = roi_bottom - roi_top;

			HalconCpp::HObject temp_region, union_region;
			HalconCpp::GenEmptyObj(&union_region);

			int patch_idx = 0;
			for (int row = 0; row < roi_height - patch_height; row += stride) {
				for (int col = 0; col < roi_width - patch_width; col += stride) {
					if (patch_idx >= _process_result_futures.size()) {
						break;
					}

					const auto& patch_result = _process_result_futures[patch_idx];
					float score = patch_result[0]; // 예측된 불량 점수

					if (score >= _result_threshold) { // 불량 판정
						// ROI 좌표를 원본 이미지 좌표로 변환
						int abs_row = roi_top + row;
						int abs_col = roi_left + col;
						int abs_row_end = abs_row + patch_height - 1;
						int abs_col_end = abs_col + patch_width - 1;

						HalconCpp::GenRectangle1(&temp_region, abs_row, abs_col, abs_row_end, abs_col_end);
						HalconCpp::Union2(union_region, temp_region, &union_region); // 영역 병합
					}

					patch_idx++;
				}
			}

			out_defect_region = union_region;
			return true;
		}


	private:
		int _image_index; // 검사 의뢰된 이미지 인덱스
		int _roi_index; // 검사 의뢰된 ROI 인덱스

		int _image_stride; // 스트라이드 정보
		const std::pair<int, int> _inspect_patch_size; // 결과 정보가 어떠한 패치 사이즈인지 확인 W * H
		const std::pair<int, int> _original_image_size; // 검사 원본 이미지 W * H

		const BoundingBox _roi_bounding_box; // ROI 영역 정보

		std::vector<std::future<std::vector<float>>> _result_futures; // 검사 결과 future
		int _result_threshold;

		bool _is_process_result_futures = false;
		std::vector<std::vector<float>> _process_result_futures; // 결과 처리 버퍼
	};

	// 스마트 포인터를 이용한 자동 메모리 관리 (shared_ptr)
	using LRequestedInspectionData1Ptr = std::shared_ptr<LRequestedInspectionData1>;

} // namespace LGIT::Model
