#pragma once
#include <vector>
#include <list>

// 할콘 없음

/*
namespace synapse::network::cpp::zeromq::client::service::helpers {
	class HalconHelper13
	{
	public:
		// Region의 면적과 중심 좌표를 계산하는 함수
		struct RegionData {
			HRegion region;
			double area;
			double row_center;
			double col_center;

			RegionData(const HRegion& reg) {
				HTuple temp_area, temp_row, temp_col;
				AreaCenter(reg, &temp_area, &temp_row, &temp_col);
				area = temp_area.D();
				row_center = temp_row.D();
				col_center = temp_col.D();
				region = reg;
			}
		};

		// 두 Region이 비슷한지 확인하는 함수
		bool AreRegionsSimilar(const RegionData& r1, const RegionData& r2, double area_threshold = 20000.0, double distance_threshold = 100.0) {
			// 면적 차이 비교
			double area_diff = std::abs(r1.area - r2.area);
			if (area_diff > area_threshold) return false;

			// 중심 좌표 거리 비교
			double distance = std::sqrt(std::pow(r1.row_center - r2.row_center, 2) +
				std::pow(r1.col_center - r2.col_center, 2));
			return distance <= distance_threshold;
		}
		
		/// <summary>
		/// 그룹화된 전체 Region 간의 유사성을 비교하고, 그룹 내 가장 큰 면적의 Region을 유지합니다.
		/// </summary>
		/// <param name="regions">비교 및 병합할 HRegion 리스트입니다.</param>
		/// <param name="min_area_threshold">최소 면적 기준으로 필터링할 임계값입니다.</param>
		/// <remarks>
		/// 주요 특징:
		/// - 입력된 Region 리스트에서 비슷한 Region을 그룹화합니다.
		/// - 그룹 내 가장 큰 면적의 Region만 선택합니다.
		/// - 최소 면적 기준 필터링을 수행합니다.
		/// 
		/// 한계:
		/// - 연결된 요소 단위로 비교하지 못하며, 전체 Region 간 비교에 국한됩니다.
		/// </remarks>
		void MergeSimilarRegions(std::list<HRegion>& regions, double min_area_threshold) {
			// Step 1: Region 데이터를 준비
			std::list<RegionData> region_data_list;
			for (const auto& region : regions) {
				region_data_list.emplace_back(region);
			}

			// Step 2: 비슷한 Region 그룹화 및 대체
			std::list<HRegion> merged_regions;

			while (!region_data_list.empty()) {
				// 기준 Region을 가져옴
				RegionData current = region_data_list.front();
				region_data_list.pop_front();

				std::list<RegionData> similar_regions = { current };

				// 기준 Region과 비슷한 Region 찾기
				for (auto it = region_data_list.begin(); it != region_data_list.end();) {
					if (AreRegionsSimilar(current, *it)) {
						similar_regions.push_back(*it);
						it = region_data_list.erase(it);
					}
					else {
						++it;
					}
				}

				// 그룹에서 가장 큰 Region을 선택
				auto largest_region = std::max_element(
					similar_regions.begin(), similar_regions.end(),
					[](const RegionData& r1, const RegionData& r2) { return r1.area < r2.area; });

				merged_regions.push_back(largest_region->region);
			}

			// Step 3: 리전 크기 기준으로 필터링
			std::list<HRegion> filtered_regions;
			for (const auto& region : merged_regions) {
				double area, row, col;
				area = region.AreaCenter(&row, &col); // 면적 계산
				if (area >= min_area_threshold) {    // 최소 면적 조건 확인
					filtered_regions.push_back(region);
				}
			}

			// Step 4: 최종 결과를 원본 리스트에 저장
			regions = std::move(filtered_regions);
		}

		/// <summary>
		/// 연결된 요소 단위로 각 Region을 분리하여 유사성을 비교하고, 그룹 내 가장 큰 요소를 유지합니다.
		/// </summary>
		/// <param name="regions">비교 및 병합할 HRegion 리스트입니다.</param>
		/// <param name="min_area_threshold">최소 면적 기준으로 필터링할 임계값입니다.</param>
		/// <remarks>
		/// 주요 특징:
		/// - `Connection()`을 통해 각 Region을 세부 요소로 분리합니다.
		/// - 분리된 요소 간 유사성을 비교하여 그룹화합니다.
		/// - 그룹 내 가장 큰 요소만 병합하며, 최소 면적 기준 필터링을 수행합니다.
		/// 
		/// 개선점:
		/// - Region 요소별 비교 지원.
		/// - 비교 정확도가 기존보다 향상되었습니다.
		/// 
		/// 한계:
		/// - 그룹 내 일부 요소만 병합합니다.
		/// - 빈 Region 초기화 방식(`GenRectangle1`)이 불명확하며 잠재적인 오류가 있을 수 있습니다.
		/// </remarks>
		void MergeSimilarRegionsV2(std::list<HRegion>& regions, double min_area_threshold) {
			// Step 1: Region 데이터를 준비
			std::list<RegionData> region_data_list;

			for (const auto& region : regions) {
				// 각 Region을 개별 요소로 분리
				HRegion connected_regions = region.Connection();
				HTuple num_regions = connected_regions.CountObj();

				for (int i = 1; i <= num_regions.I(); ++i) {
					HRegion single_region = connected_regions.SelectObj(i);
					region_data_list.emplace_back(single_region);
				}
			}

			// Step 2: 비슷한 Region 그룹화 및 대체
			std::list<HRegion> merged_regions;

			while (!region_data_list.empty()) {
				// 기준 Region을 가져옴
				RegionData current = region_data_list.front();
				region_data_list.pop_front();

				std::list<RegionData> similar_regions = { current };

				// 기준 Region과 비슷한 Region 찾기
				for (auto it = region_data_list.begin(); it != region_data_list.end();) {
					if (AreRegionsSimilar(current, *it)) {
						similar_regions.push_back(*it);
						it = region_data_list.erase(it);
					}
					else {
						++it;
					}
				}

				// 그룹에서 가장 큰 Region을 선택
				auto largest_region = std::max_element(
					similar_regions.begin(), similar_regions.end(),
					[](const RegionData& r1, const RegionData& r2) { return r1.area < r2.area; });

				merged_regions.push_back(largest_region->region);
			}

			// Step 3: 리전 크기 기준으로 필터링
			std::list<HRegion> filtered_regions;
			for (const auto& region : merged_regions) {
				HTuple area, row, col;
				AreaCenter(region, &area, &row, &col); // 면적 계산
				if (area >= min_area_threshold) {     // 최소 면적 조건 확인
					filtered_regions.push_back(region);
				}
			}

			// Step 4: 최종 결과를 원본 리스트에 저장
			regions = std::move(filtered_regions);
		}

		/// <summary>
		/// 연결된 요소 단위로 각 Region을 분리하고, 그룹 내 모든 요소를 병합하여 최종 결과를 생성합니다.
		/// </summary>
		/// <param name="regions">비교 및 병합할 HRegion 리스트입니다.</param>
		/// <param name="min_area_threshold">최소 면적 기준으로 필터링할 임계값입니다.</param>
		/// <remarks>
		/// 주요 특징:
		/// - `Connection()`을 통해 각 Region을 세부 요소로 분리합니다.
		/// - 요소 간 비교를 통해 유사성을 판단하고 그룹화합니다.
		/// - 그룹 내 모든 요소를 `Union2`를 통해 병합하여 단일 Region을 생성합니다.
		/// - 최소 면적 기준 필터링을 수행합니다.
		/// 
		/// 개선점:
		/// - `GenEmptyObj`를 사용하여 안정적이고 명확한 빈 Region 초기화를 수행합니다.
		/// - 그룹 내 모든 요소를 병합하여 병합 정확도가 향상되었습니다.
		/// - 코드 안정성이 강화되었습니다.
		/// 
		/// 최종 결과:
		/// - 요소 단위 비교 및 병합 정확도가 크게 개선되었습니다.
		/// </remarks>
		void MergeSimilarRegionsV3(std::list<HRegion>& regions, double min_area_threshold) {
			// Step 1: Region 데이터를 준비
			std::list<RegionData> region_data_list;

			for (const auto& region : regions) {
				// 각 Region을 개별 요소로 분리
				HRegion connected_regions = region.Connection();
				HTuple num_regions = connected_regions.CountObj();

				for (int i = 1; i <= num_regions.I(); ++i) {
					HRegion single_region = connected_regions.SelectObj(i);
					region_data_list.emplace_back(single_region);
				}
			}

			// Step 2: 비슷한 Region 그룹화 및 대체
			std::list<HRegion> merged_regions;

			while (!region_data_list.empty()) {
				// 기준 Region을 가져옴
				RegionData current = region_data_list.front();
				region_data_list.pop_front();

				std::list<RegionData> similar_regions = { current };

				// 기준 Region과 비슷한 Region 찾기
				for (auto it = region_data_list.begin(); it != region_data_list.end();) {
					// 기준 Region과 비교
					if (AreRegionsSimilar(current, *it)) {
						similar_regions.push_back(*it);
						it = region_data_list.erase(it);
					}
					else {
						++it;
					}
				}

				// 그룹에서 각 요소별로 비교 수행
				HRegion merged_group;
				GenEmptyObj(&merged_group); // 빈 Region 초기화

				for (const auto& region_data : similar_regions) {
					Union2(merged_group, region_data.region, &merged_group);
				}

				merged_regions.push_back(merged_group);
			}

			// Step 3: 리전 크기 기준으로 필터링
			std::list<HRegion> filtered_regions;
			for (const auto& region : merged_regions) {
				HTuple area, row, col;
				AreaCenter(region, &area, &row, &col); // 면적 계산
				if (area >= min_area_threshold) {     // 최소 면적 조건 확인
					filtered_regions.push_back(region);
				}
			}

			// Step 4: 최종 결과를 원본 리스트에 저장
			regions = std::move(filtered_regions);
		}

		// 작은 리전을 제거하는 함수
		HRegion RemoveSmallRegions(const HRegion& inputRegion, double minArea)
		{
			HRegion filteredRegion;
			try
			{
				// 연결된 영역으로 분리
				HRegion connectedRegions = inputRegion.Connection();
				HTuple numRegions = connectedRegions.CountObj();

				// 결과 저장용 빈 Region 생성
				GenEmptyRegion(&filteredRegion);

				// 모든 Region 비교 및 필터링
				for (int i = 1; i <= numRegions.I(); ++i)
				{
					HRegion region1 = connectedRegions.SelectObj(i); // 첫 번째 Region 선택

					// 첫 번째 Region이 너무 작으면 건너뜀
					HTuple area1, row1, col1;
					AreaCenter(region1, &area1, &row1, &col1);

					if (area1 < minArea)
						continue; // 최소 크기 조건에 미달 시 스킵

					bool isContained = false;

					// 다른 Region들과 포함 관계 비교
					for (int j = 1; j <= numRegions.I(); ++j)
					{
						if (i == j) continue; // 같은 Region은 비교하지 않음

						HRegion region2 = connectedRegions.SelectObj(j); // 두 번째 Region 선택

						// region1이 region2에 포함되어 있는지 확인
						HRegion intersection = region1.Intersection(region2);
						HTuple intersectionArea;
						AreaCenter(intersection, &intersectionArea, nullptr, nullptr);

						// intersection이 region1과 동일하면 포함됨
						if (intersectionArea == area1)
						{
							isContained = true;
							break;
						}
					}

					// 포함되지 않은 Region만 결과에 추가
					if (!isContained)
					{
						Union2(filteredRegion, region1, &filteredRegion);
					}
				}
			}
			catch (HOperatorException& ex)
			{
				// Halcon 예외 처리
				CString errorMsg;
				errorMsg.Format(_T("Halcon 오류 발생: %s\n"), ex.ErrorMessage().Text());
				OutputDebugString(errorMsg);
				throw; // 예외를 호출자에게 다시 전달
			}

			return filteredRegion;
		}

		static void ConvertList2Region(const std::list<std::list<std::pair<int, int>>>& external_contours, const std::list<std::list<std::pair<int, int>>>& internal_contours, HRegion& output_region)
		{
			HRegion external_region, internal_region;

			// 외부 컨투어 처리
			for (const auto& contour : external_contours) {
				HTuple rows, cols;

				// 좌표를 Halcon의 튜플로 변환
				for (const auto& point : contour) {
					rows.Append(point.second); // y 좌표
					cols.Append(point.first);  // x 좌표
				}

				// 외부 컨투어를 리전으로 생성
				HRegion single_region;
				single_region.GenRegionPolygonFilled(rows, cols);
				external_region = (external_region.IsInitialized() ? external_region.Union2(single_region) : single_region);
			}

			// 내부 컨투어 처리
			for (const auto& contour : internal_contours) {
				HTuple rows, cols;

				// 좌표를 Halcon의 튜플로 변환
				for (const auto& point : contour) {
					rows.Append(point.second); // y 좌표
					cols.Append(point.first);  // x 좌표
				}

				// 내부 컨투어를 리전으로 생성
				HRegion single_region;
				single_region.GenRegionPolygonFilled(rows, cols);
				internal_region = (internal_region.IsInitialized() ? internal_region.Union2(single_region) : single_region);
			}

			// 최종 리전 생성: 외부 - 내부
			if (external_region.IsInitialized()) {
				if (internal_region.IsInitialized()) {
					output_region = external_region.Difference(internal_region);
				}
				else {
					output_region = external_region; // 내부 컨투어가 없으면 외부만 반환
				}
			}
			else {
				output_region = internal_region; // 외부 컨투어가 없으면 내부만 반환
			}
		}
	};
}
*/
