#pragma once
#include <string>
#include <tuple>
#include <list>
#include <opencv2/opencv.hpp>

namespace synapse::network::cpp::zeromq::client::service::helpers {
	class ContourProcessHelper
	{
	public:
		// 중심점 계산 함수
		static std::pair<float, float> GetCentroid(const std::list<std::pair<int, int>>& contour) {
			float x_sum = 0, y_sum = 0;
			int n = contour.size();
			for (const auto& point : contour) {
				x_sum += point.first;
				y_sum += point.second;
			}
			return { x_sum / n, y_sum / n };
		}

		// 점이 다각형 내부에 있는지 확인 (Ray-Casting Algorithm)
		static bool IsPointInPolygon(const std::pair<int, int>& point, const std::list<std::pair<int, int>>& polygon) {
			int x = point.first, y = point.second;
			bool inside = false;

			auto it = polygon.begin();
			auto prev = std::prev(polygon.end());

			for (; it != polygon.end(); prev = it++) {
				int xi = it->first, yi = it->second;
				int xj = prev->first, yj = prev->second;

				bool intersect = ((yi > y) != (yj > y)) &&
					(x < (xj - xi) * (y - yi) / (yj - yi) + xi);
				if (intersect)
					inside = !inside;
			}

			return inside;
		}

		// 컨투어 분류 함수
		static void ClassifyContours(
			const std::list<std::list<std::pair<int, int>>>& contours,
			std::list<std::list<std::pair<int, int>>>& external_contours,
			std::list<std::list<std::pair<int, int>>>& internal_contours)
		{
			// 각 컨투어의 중심점 계산
			std::vector<std::pair<std::pair<float, float>, const std::list<std::pair<int, int>>*>> centroids;
			for (const auto& contour : contours) {
				centroids.emplace_back(GetCentroid(contour), &contour);
			}

			// 외부/내부 컨투어 분류
			for (const auto& [current_centroid, current_contour] : centroids) {
				bool is_internal = false;

				for (const auto& [other_centroid, other_contour] : centroids) {
					if (current_contour == other_contour)
						continue; // 동일 컨투어는 스킵

					// 중심점이 다른 다각형 내부에 있는지 확인
					if (IsPointInPolygon(
						{ static_cast<int>(current_centroid.first), static_cast<int>(current_centroid.second) },
						*other_contour)) {
						is_internal = true;
						break;
					}
				}

				if (is_internal)
					internal_contours.push_back(*current_contour);
				else
					external_contours.push_back(*current_contour);
			}
		}

		//// Point-in-Polygon Test (Ray-Casting Algorithm)
		//static bool IsPointInPolygon(const std::pair<int, int>& point, const std::list<std::pair<int, int>>& polygon) {
		//	int x = point.first, y = point.second;
		//	bool inside = false;

		//	auto it = polygon.begin();
		//	auto prev = std::prev(polygon.end());

		//	for (; it != polygon.end(); prev = it++) {
		//		int xi = it->first, yi = it->second;
		//		int xj = prev->first, yj = prev->second;

		//		bool intersect = ((yi > y) != (yj > y)) &&
		//			(x < (xj - xi) * (y - yi) / (yj - yi) + xi);
		//		if (intersect) inside = !inside;
		//	}

		//	return inside;
		//}

		//// 복구 함수
		//static void ClassifyContours(
		//	const std::list<std::list<std::pair<int, int>>>& contours,
		//	std::list<std::list<std::pair<int, int>>>& external_contours,
		//	std::list<std::list<std::pair<int, int>>>& internal_contours) {

		//	for (const auto& contour : contours) {
		//		bool is_internal = false;

		//		for (const auto& other_contour : contours) {
		//			if (&contour == &other_contour) continue; // 같은 컨투어는 스킵

		//			// 임의의 점으로 포함 여부 확인
		//			auto point = *contour.begin();
		//			if (IsPointInPolygon(point, other_contour)) {
		//				is_internal = true;
		//				break;
		//			}
		//		}

		//		if (is_internal)
		//			internal_contours.push_back(contour);
		//		else
		//			external_contours.push_back(contour);
		//	}
		//}

		//void GenerateHalconRegion(
		//	const std::list<std::list<std::pair<int, int>>>& external_contours,
		//	const std::list<std::list<std::pair<int, int>>>& internal_contours,
		//	HRegion& output_region) {
		//
		//	HRegion external_region, internal_region;
		//
		//	// 외부 컨투어 처리
		//	for (const auto& contour : external_contours) {
		//		HTuple rows, cols;
		//
		//		// 좌표를 Halcon의 튜플로 변환
		//		for (const auto& point : contour) {
		//			rows.Append(point.second); // y 좌표
		//			cols.Append(point.first);  // x 좌표
		//		}
		//
		//		// 외부 컨투어를 리전으로 생성
		//		HRegion single_region;
		//		single_region.GenRegionPolygonFilled(rows, cols);
		//		external_region = (external_region.IsInitialized() ? external_region.Union2(single_region) : single_region);
		//	}
		//
		//	// 내부 컨투어 처리
		//	for (const auto& contour : internal_contours) {
		//		HTuple rows, cols;
		//
		//		// 좌표를 Halcon의 튜플로 변환
		//		for (const auto& point : contour) {
		//			rows.Append(point.second); // y 좌표
		//			cols.Append(point.first);  // x 좌표
		//		}
		//
		//		// 내부 컨투어를 리전으로 생성
		//		HRegion single_region;
		//		single_region.GenRegionPolygonFilled(rows, cols);
		//		internal_region = (internal_region.IsInitialized() ? internal_region.Union2(single_region) : single_region);
		//	}
		//
		//	// 최종 리전 생성: 외부 - 내부
		//	if (external_region.IsInitialized()) {
		//		if (internal_region.IsInitialized()) {
		//			output_region = external_region.Difference(internal_region);
		//		}
		//		else {
		//			output_region = external_region; // 내부 컨투어가 없으면 외부만 반환
		//		}
		//	}
		//	else {
		//		output_region = internal_region; // 외부 컨투어가 없으면 내부만 반환
		//	}
		//}

	};

}