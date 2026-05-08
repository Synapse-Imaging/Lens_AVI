#include "HalconCpp.h"
#include "ImageUtile.h"
#include "StdAfx.h"
#include <algorithm>
#include <cstring>   // std::strcmp
#include <memory>
#include <opencv2/opencv.hpp>
#include <ratio>
#include <spdlog/logger.h>

namespace AIService {

	using namespace HalconCpp;

	cv::Mat ImageUtile::HImage2Mat(const HObject& h_img)
	{
		cv::Mat cv_img;
		HTuple channels, w, h;
		HObject h_image = h_img;

		ConvertImageType(h_image, &h_image, "byte");
		CountChannels(h_image, &channels);

		if (channels.I() == 1) {
			HTuple pointer;
			GetImagePointer1(h_image, &pointer, nullptr, &w, &h);
			cv_img = cv::Mat(h.I(), w.I(), CV_8UC1, (void*)(pointer.L())).clone();
		}
		else if (channels.I() == 3) {
			HTuple pr, pg, pb;
			GetImagePointer3(h_image, &pr, &pg, &pb, nullptr, &w, &h);

			cv::Mat r(h.I(), w.I(), CV_8UC1, (void*)pr.L());
			cv::Mat g(h.I(), w.I(), CV_8UC1, (void*)pg.L());
			cv::Mat b(h.I(), w.I(), CV_8UC1, (void*)pb.L());

			std::vector<cv::Mat> channels = { b, g, r };
			cv::merge(channels, cv_img);
			for (cv::Mat channel : channels) {
				channel = cv::Mat();
			}
			channels.clear();
		}
		return cv_img;
	}

	// 빠른 버전: out 재사용 + 불필요 변환 최소 + 3채널 1회 복사
	inline void ImageUtile::HImage2MatFastCopy(const HalconCpp::HObject& h_img, cv::Mat& out)
	{
		using namespace HalconCpp;

		HObject img_byte = h_img;

		HTuple type;
		GetImageType(h_img, &type);

		// Halcon 문자열 비교는 Text()로
		const char* type_cstr = type.S().Text();
		if (type_cstr == nullptr || std::strcmp(type_cstr, "byte") != 0) {
			ConvertImageType(h_img, &img_byte, "byte");
		}

		HTuple ch;
		CountChannels(img_byte, &ch);

		if (ch.I() == 1) {
			HImage2Mat_GrayCopy(img_byte, out);
			return;
		}

		if (ch.I() == 3) {
			HImage2Mat_BGRMergeCopy(img_byte, out);
			return;
		}

		out.release();
	}

	// out을 재사용하도록 인터페이스 변경 추천
	// (기존 인터페이스 유지해야 하면 내부 static thread_local 버퍼 등도 가능하지만 비추)
	inline void ImageUtile::HImage2Mat_GrayCopy(const HObject& img_byte, cv::Mat& out)
	{
		using namespace HalconCpp;

		HTuple ptr, w, h;
		GetImagePointer1(img_byte, &ptr, nullptr, &w, &h);

		const int width = w.I();
		const int height = h.I();

		out.create(height, width, CV_8UC1);
		std::memcpy(out.data, (void*)ptr.L(), (size_t)width * (size_t)height);
	}

	inline void ImageUtile::HImage2Mat_BGRMergeCopy(const HObject& img_byte, cv::Mat& out)
	{
		using namespace HalconCpp;

		HTuple pr, pg, pb, w, h;
		GetImagePointer3(img_byte, &pr, &pg, &pb, nullptr, &w, &h);

		const int width = w.I();
		const int height = h.I();

		// Halcon planar 포인터를 "뷰"로만 감싼다 (복사 없음)
		cv::Mat r(height, width, CV_8UC1, (void*)pr.L());
		cv::Mat g(height, width, CV_8UC1, (void*)pg.L());
		cv::Mat b(height, width, CV_8UC1, (void*)pb.L());

		out.create(height, width, CV_8UC3);

		// clone 없이 merge -> out으로 딱 1번 복사
		cv::Mat planes[3] = { b, g, r }; // BGR 순서
		cv::merge(planes, 3, out);
	}

	HObject ImageUtile::Mat2HImage(const cv::Mat& mat)
	{
		HObject h_image;
		int width = mat.cols, height = mat.rows, channels = mat.channels();

		if (channels == 1) {
			GenImage1(&h_image, "byte", width, height, (Hlong)mat.data);
		}
		else if (channels == 3) {
			std::vector<cv::Mat> bgr;
			cv::split(mat, bgr);
			GenImage3(&h_image, "byte", width, height,
					  (Hlong)bgr[2].data, (Hlong)bgr[1].data, (Hlong)bgr[0].data);
		}
		else {
			throw std::runtime_error("Unsupported number of channels in cv::Mat.");
		}
		return h_image;
	}

	cv::Mat ImageUtile::HRegion2Mask(const HObject& region)
	{
		if (!CGFunction::ValidHRegion(region)) {
			return cv::Mat();
		}

		HTuple row1, col1, row2, col2;
		SmallestRectangle1(region, &row1, &col1, &row2, &col2);
		int width = col2.I() - col1.I() + 1;
		int height = row2.I() - row1.I() + 1;
		HObject bin_img;
		RegionToBin(region, &bin_img, 255, 0, width, height);
		HTuple pointer, type, w, h;
		GetImagePointer1(bin_img, &pointer, &type, &w, &h);
		return cv::Mat(h.I(), w.I(), CV_8UC1, (void*)pointer.L()).clone();
	}

	HObject ImageUtile::Mask2HRegion(const cv::Mat& mask)
	{
		if (mask.empty() || mask.channels() != 1 || mask.type() != CV_8UC1)
			throw std::runtime_error("Mask must be non-empty CV_8UC1 image");
		HObject h_image;
		GenImage1(&h_image, "byte", mask.cols, mask.rows, (Hlong)mask.data);
		HObject region;
		Threshold(h_image, &region, 1, 255);
		return region;
	}

	bool ImageUtile::CropImageWithPadding(const cv::Mat& input, const cv::Size& crop_size, int stride, std::vector<cv::Mat>& crops)
	{
		if (input.empty() || crop_size.width <= 0 || crop_size.height <= 0 || stride <= 0)
			return false;

		int input_w = input.cols;
		int input_h = input.rows;

		int pad_right = ((input_w - crop_size.width) % stride != 0) ? stride - ((input_w - crop_size.width) % stride) : 0;
		int pad_bottom = ((input_h - crop_size.height) % stride != 0) ? stride - ((input_h - crop_size.height) % stride) : 0;
		int pad_left = crop_size.width / 2;
		int pad_top = crop_size.height / 2;

		int total_pad_right = pad_right + pad_left;
		int total_pad_bottom = pad_bottom + pad_top;

		cv::Mat padded;
		cv::copyMakeBorder(input, padded, pad_top, total_pad_bottom - pad_top, pad_left, total_pad_right - pad_left, cv::BORDER_REFLECT_101);

		for (int y = 0; y + crop_size.height <= padded.rows; y += stride)
		{
			for (int x = 0; x + crop_size.width <= padded.cols; x += stride)
			{
				cv::Rect roi(x, y, crop_size.width, crop_size.height);
				crops.push_back(padded(roi).clone());
			}
		}
		return true;
	}

	void ImageUtile::CvSaveImage(
		const std::string& root_path,
		const std::string& lot_id,
		const std::string& tray_id,
		const std::string& module_id,
		const std::string& pos_no,
		const std::string& image_index,
		const std::string& roi_index,
		const std::string& tab_index,
		const std::string& barcode,
		cv::Mat cv_image) {
		try {
			// 현재 시간을 가져옴
			auto now = std::chrono::system_clock::now();
			std::time_t now_time = std::chrono::system_clock::to_time_t(now);

			// std::tm 구조체로 변환
			std::tm local_time;
#if defined(_MSC_VER) || defined(__MINGW32__) // MSVC 또는 MinGW를 사용하는 경우
			localtime_s(&local_time, &now_time);
#else // 다른 표준 C++ 컴파일러의 경우
			localtime_r(&now_time, &local_time);
#endif

			// 날짜 경로 생성 (0이 붙지 않도록 수정)
			std::ostringstream dir_stream;
			dir_stream << root_path << "\\"
				<< (local_time.tm_year + 1900) << "\\"
				<< (local_time.tm_mon + 1) << "\\"
				<< local_time.tm_mday << "\\"
				<< lot_id << "\\"
				<< "Tray-" << tray_id << "\\"
				<< "ROIImage";

			std::filesystem::path dir_path(dir_stream.str());

			// 디렉터리 생성 (필요한 경우)
			if (!std::filesystem::exists(dir_path)) {
				if (!std::filesystem::create_directories(dir_path)) {
					std::cerr << "디렉터리 생성 실패: " << dir_path.string() << std::endl;
					return;
				}
			}

			// 파일명 설정 (Module_id, pos_no, roi_index 조합)
			std::ostringstream filename_stream;
			filename_stream << "Module" << module_id
				<< "_POS" << pos_no
				<< "_IMG" << image_index
				<< "_ROI" << roi_index
				<< "_TAB" << tab_index
				<< "_" << barcode;

			// 최종 저장 경로 설정
			std::string full_path = (dir_path / filename_stream.str()).string();

			// 이미지 저장 (추가 처리 없이 입력된 데이터 그대로 저장)
			cv::imwrite(full_path, cv_image);

			std::cout << "이미지가 저장되었습니다: " << full_path << std::endl;
		}
		catch (const HalconCpp::HException& ex) {
			std::cerr << "Halcon 오류: " << ex.ErrorMessage() << std::endl;
		}
		catch (const std::exception& ex) {
			std::cerr << "예외 발생: " << ex.what() << std::endl;
		}
	}

	void ImageUtile::SaveImage(
		const std::string& root_path, const std::string& equip_id, const std::string& model_type,
		const std::string& lot_id, const std::string& tray_id, const std::string& module_id,
		const std::string& vision_name, const std::string& pc_index, const std::string& stage_index,
		const std::string& image_index, const std::string& roi_index, const std::string& inspectiontype_name, const std::string& tab_index,
		const std::string& barcode, HalconCpp::HObject h_image, HalconCpp::HObject h_image_v2,
		const std::string& local_align_angle_string, const std::string& channel_type) {
		try {
			// 현재 시간을 가져옴
			auto now = std::chrono::system_clock::now();
			std::time_t now_time = std::chrono::system_clock::to_time_t(now);

			// std::tm 구조체로 변환
			std::tm local_time;
#if defined(_MSC_VER) || defined(__MINGW32__) // MSVC 또는 MinGW를 사용하는 경우
			localtime_s(&local_time, &now_time);
#else // 다른 표준 C++ 컴파일러의 경우
			localtime_r(&now_time, &local_time);
#endif

			std::ostringstream dir_stream;
			dir_stream << root_path << "\\"
				<< "ROIImage";

			std::filesystem::path dir_path(dir_stream.str());

			// 디렉터리 생성 (필요한 경우)
			if (!std::filesystem::exists(dir_path)) {
				if (!std::filesystem::create_directories(dir_path)) {
					std::cerr << "디렉터리 생성 실패: " << dir_path.string() << std::endl;
					return;
				}
			}

			// 파일명 설정 (Module_id, pos_no, roi_index 조합)
			std::ostringstream filename_stream;
			filename_stream
				<< "" << equip_id
				<< "_" << model_type
				<< "_" << lot_id
				<< "_Tray" << tray_id
				<< "_Module" << module_id
				<< "_" << vision_name
				<< "" << pc_index
				<< "_Stage" << stage_index
				<< "_Image" << image_index
				<< "_ROI" << roi_index
				<< "_" << inspectiontype_name
				<< "_Tab" << tab_index
				<< "_" << barcode
				<< "_" << local_align_angle_string
				<< "_" << channel_type;
			// << "_" << roi_unique_id;

		// 최종 저장 경로 설정
			std::string full_path = (dir_path / filename_stream.str()).string();
			std::string full_path_v2 = (dir_path / filename_stream.str()).string() + "_V2";

			// 이미지 저장 (추가 처리 없이 입력된 데이터 그대로 저장)
			HalconCpp::WriteImage(h_image, "jpeg 100", 0, full_path.c_str());
			HalconCpp::WriteImage(h_image_v2, "jpeg 100", 0, full_path_v2.c_str());

			std::cout << "이미지가 저장되었습니다: " << full_path << std::endl;
		}
		catch (const HalconCpp::HException& ex) {
			std::cerr << "Halcon 오류: " << ex.ErrorMessage() << std::endl;
		}
		catch (const std::exception& ex) {
			std::cerr << "예외 발생: " << ex.what() << std::endl;
		}
	}


	void ImageUtile::SaveImage(const std::string root_path,
							   const std::string prefix,
							   HalconCpp::HObject h_image)
	{
		if (prefix.empty()) {
			std::cerr << "저장 불가: 저장 경로 접두사가 비어 있습니다." << std::endl;
			return;
		}

		std::filesystem::path root_base(root_path);
		std::filesystem::path full_path = root_base / prefix;
		std::filesystem::path parent_dir = full_path.parent_path();

		try {
			if (!parent_dir.empty() && !std::filesystem::exists(parent_dir)) {
				if (!std::filesystem::create_directories(parent_dir)) {
					std::cerr << "저장 불가: 디렉터리 생성 실패 - " << parent_dir.string() << std::endl;
					return;
				}
			}

			if (!h_image.IsInitialized()) {
				std::cerr << "저장 불가: 유효하지 않은 Halcon 이미지입니다." << std::endl;
				return;
			}

			HTuple width, height;
			HalconCpp::GetImageSize(h_image, &width, &height);
			if (width <= 0 || height <= 0) {
				std::cerr << "저장 불가: 이미지 크기가 올바르지 않습니다." << std::endl;
				return;
			}

			HalconCpp::WriteImage(h_image, "jpeg 100", 0, full_path.string().c_str());
		}
		catch (const HalconCpp::HException& ex) {
			std::cerr << "Halcon 오류: " << ex.ErrorMessage() << std::endl;
		}
		catch (const std::exception& ex) {
			std::cerr << "예외 발생: " << ex.what() << std::endl;
		}
	}


	HObject ImageUtile::Reg2Image(const HalconCpp::HObject& reg, const int& Width, const int& Height)
	{
		HObject mask_image;
		// 리전 영역에 해당하는 이미지 생성 (배경: 검정, 리전: 흰색)
		GenImageConst(&mask_image, "byte", Width, Height);  // 사이즈는 필요에 따라 조정
		PaintRegion(reg, mask_image, &mask_image, 255, "fill");
		return std::move(mask_image);
	}

	void ImageUtile::SaveRegionAsImage(const HalconCpp::HObject& region, const std::string& filePath, int width, int height) {
		// HObject가 Region인지 확인
		HalconCpp::HTuple objClass;
		HalconCpp::GetObjClass(region, &objClass);

		//if (objClass.ToString().Text() != std::string("region")) { 
		//	std::cerr << "Error: HObject is not a region. Skipping save: " << filePath << std::endl;
		//	return;
		//}


		// 자동 크기 조정: Width, Height가 0이면 Region 크기로 설정
		if (width == 0 || height == 0) {
			HalconCpp::HTuple row1, col1, row2, col2;
			HalconCpp::SmallestRectangle1(region, &row1, &col1, &row2, &col2);
			width = (col2 - col1).I();
			height = (row2 - row1).I();
		}

		// 리전을 그레이스케일 이미지로 변환 (배경: 0, 리전: 255)
		HalconCpp::HObject regionImage;
		HalconCpp::RegionToBin(region, &regionImage, 255, 0, width, height);

		// 이미지 저장 (PNG 형식)
		HalconCpp::WriteImage(regionImage, "png", 0, filePath.c_str());

		std::cout << "Saved: " << filePath << " (" << width << "x" << height << ")" << std::endl;
	}

	void ImageUtile::SaveRegionsFromMap(
		const std::unordered_map<int, HalconCpp::HObject>& regionData,
		const std::string& folderPath,
		int width, int height,
		std::string addImageName)
	{
		for (const auto& [imageIndex, region] : regionData) {
			// HObject가 Region인지 확인
			HalconCpp::HTuple objClass;
			HalconCpp::GetObjClass(region, &objClass);
			//if (objClass.ToString().Text() != std::string("region")) {
			//	std::cerr << "Skipping non-region object at index " << imageIndex << std::endl;
			//	continue;  // Region이 아니면 저장하지 않음
			//}

			// 파일 경로 생성
			std::string filePath = folderPath + "/" + addImageName + "_image_index" + std::to_string(imageIndex) + ".png";

			// 개별 Region 저장
			SaveRegionAsImage(region, filePath, width, height);
		}
	}

	bool ImageUtile::HasIntersection(const HObject& region1, const HObject& region2)
	{
		HTuple row, col, inside;
		GetRegionPoints(region2, &row, &col);
		for (int i = 0; i < row.Length(); ++i) {
			TestRegionPoint(region1, row[i], col[i], &inside);
			if (inside.I() != 0) return true;
		}
		return false;
	}

	bool ImageUtile::HasIntersection2(const HObject& region1, const HObject& region2)
	{
		HObject intersected;
		Intersection(region1, region2, &intersected);
		HTuple area;
		AreaCenter(intersected, &area, nullptr, nullptr);
		return (area > 0);
	}


	void ImageUtile::RestoreRegionToOriginal(
		const HObject& roi_region,
		const LGIT::Model::BoundingBox& roi_box,
		HObject& restored_region)
	{
		try {
			HTuple hom_mat;
			// (0,0) → (left, top)로 이동하는 변환행렬 생성
			VectorAngleToRigid(0, 0, 0, roi_box.top, roi_box.left, 0, &hom_mat);
			// 리전을 원본 좌표로 이동
			AffineTransRegion(roi_region, &restored_region, hom_mat, "nearest_neighbor");
		}
		catch (const HalconCpp::HException& ex) {
			std::cerr << "Halcon 예외 (RestoreRegionToOriginal): " << ex.ErrorMessage() << std::endl;
			throw;  // 또는 무시하거나 로깅만
		}
	}

	// cv::Rect를 사용하는 새로운 오버로딩 함수
	void ImageUtile::RestoreRegionToOriginal(
		const HObject& roi_region,
		const cv::Rect& roi_rect,
		HObject& restored_region)
	{
		try {
			HTuple hom_mat;
			// (0,0) → (roi_rect.x, roi_rect.y)로 이동하는 변환행렬 생성
			VectorAngleToRigid(0, 0, 0, roi_rect.y, roi_rect.x, 0, &hom_mat);
			// 리전을 원본 좌표로 이동
			AffineTransRegion(roi_region, &restored_region, hom_mat, "nearest_neighbor");
		}
		catch (const HalconCpp::HException& ex) {
			std::cerr << "Halcon 예외 (RestoreRegionToOriginal with cv::Rect): " << ex.ErrorMessage() << std::endl;
			throw;  // 또는 무시하거나 로깅만
		}
	}

	bool ImageUtile::ValidHRegion(const HalconCpp::HObject& region)
	{
		/*try
		{
			bool bValid = region.IsInitialized();

			if (bValid)
			{
				HTuple HObjectNum;
				CountObj(region, &HObjectNum);

				if (HObjectNum > 0)
				{
					HTuple Area, Row, Column;

					AreaCenter(region, &Area, &Row, &Column);

					for (int i = 0; i < HObjectNum; i++)
					{
						if (Area[i].L() > 0)
							return true;
					}
				}
			}

			return false;
		}
		catch (HException& except)
		{
			return false;
		}*/
		if (!region.IsInitialized())
			return false;

		try
		{
			HalconCpp::HTuple area;
			HalconCpp::AreaCenter(region, &area, nullptr, nullptr);

			const int count = area.Length();
			for (int i = 0; i < count; ++i)
			{
				if (area[i].L() > 0)
					return true;
			}
		}
		catch (const HalconCpp::HException&)
		{
			// 예외 발생 시 유효하지 않은 Region으로 판단
		}

		return false;
	}

	HalconCpp::HObject AIService::ImageUtile::RegionToLabeledImage(const HalconCpp::HObject& region, int width, int height)
	{
		using namespace HalconCpp;

		HObject connected_regions, single_region, temp_image, result_image;

		// 연결 성분 분리
		Connection(region, &connected_regions);

		// 전체 객체 수 확인
		HTuple num_objects;
		CountObj(connected_regions, &num_objects);

		// 배경이 0인 byte 타입 이미지 생성
		GenImageConst(&result_image, "byte", width, height);

		for (int i = 1; i <= num_objects.I() && i <= 255; ++i)
		{
			SelectObj(connected_regions, &single_region, i);
			PaintRegion(single_region, result_image, &temp_image, i, "fill");
			result_image = temp_image;
		}

		return result_image;
	}

	std::optional<cv::Mat> ImageUtile::RegionToLabeledImageV2(const HalconCpp::HObject& region,
															  const cv::Size& orignal_image_size,
															  const cv::Size& roi_image_size,
															  const cv::Point& roi_left_top,
															  const cv::Point& roi_right_bottom)
	{
		// 유효 Region 여부 확인
		if (!ValidHRegion(region)) {
			if (!CGFunction::ValidHRegion(region)) {
				return std::nullopt;
			}
		}

		try {
			// Region → 라벨링된 Halcon 이미지로 변환
			auto halcon_labeled_mask_image = RegionToLabeledImage(region,
																  orignal_image_size.width,
																  orignal_image_size.height);

			// 유효한 ROI 범위 계산 (원본 이미지 경계 내로 클램핑)
			int valid_left = max(0, roi_left_top.x);
			int valid_top = max(0, roi_left_top.y);
			int valid_right = min(orignal_image_size.width - 1, roi_right_bottom.x);
			int valid_bottom = min(orignal_image_size.height - 1, roi_right_bottom.y);

			// 유효 영역이 없는 경우
			if (valid_left > valid_right || valid_top > valid_bottom) {
				// 전체가 범위 밖이면 0으로 채워진 이미지 반환
				return cv::Mat::zeros(roi_image_size, CV_8UC1);
			}

			// 유효 영역만 Halcon 이미지에서 자르기
			HalconCpp::HObject cropped_image;
			CropRectangle1(halcon_labeled_mask_image, &cropped_image, valid_top, valid_left, valid_bottom, valid_right);

			// Halcon 이미지 → OpenCV 변환
			cv::Mat cropped_mat = HImage2Mat(cropped_image);

			// 결과 이미지 생성 (0으로 초기화)
			int roi_width = roi_right_bottom.x - roi_left_top.x + 1;
			int roi_height = roi_right_bottom.y - roi_left_top.y + 1;
			cv::Mat result = cv::Mat::zeros(roi_height, roi_width, cropped_mat.type());

			// 유효 영역을 결과 이미지의 올바른 위치에 복사
			int dst_x = valid_left - roi_left_top.x;
			int dst_y = valid_top - roi_left_top.y;
			cv::Rect dst_roi(dst_x, dst_y, cropped_mat.cols, cropped_mat.rows);
			cropped_mat.copyTo(result(dst_roi));

			// ROI와 이미지 사이즈 조정
			if (roi_image_size.width != result.cols ||
				roi_image_size.height != result.rows) {
				cv::resize(
					result,
					result,
					roi_image_size,
					0, 0,
					cv::INTER_NEAREST  // 선명함 유지, 마스크/라벨에 적합
				);
			}

			// 리소스 해제
			halcon_labeled_mask_image.Clear();
			cropped_image.Clear();
			return result;
		}
		catch (const std::exception& e) {
		}
		catch (...) {
		}
		return std::nullopt;
	}


	cv::Mat AIService::ImageUtile::ExtractSpecificValue(const cv::Mat& input, int value)
	{
		CV_Assert(input.type() == CV_8UC1 || input.type() == CV_8UC3);

		cv::Mat mask;

		if (input.type() == CV_8UC1) {
			// 단일 채널: 그대로 비교
			cv::compare(input, value, mask, cv::CMP_EQ);
		}
		else {
			// 3채널: 모든 채널이 value인 경우만 해당
			std::vector<cv::Mat> channels;
			cv::split(input, channels);

			cv::Mat mask0, mask1, mask2;
			cv::compare(channels[0], value, mask0, cv::CMP_EQ);
			cv::compare(channels[1], value, mask1, cv::CMP_EQ);
			cv::compare(channels[2], value, mask2, cv::CMP_EQ);

			// AND 연산으로 모든 채널이 동일한 값인 위치만 추출
			cv::bitwise_and(mask0, mask1, mask);
			cv::bitwise_and(mask, mask2, mask);
		}
		CV_Assert(mask.size() == input.size());

		return mask; // 0 또는 255가 있는 CV_8UC1 마스크
	}

	bool ImageUtile::HasImageData(const cv::Mat& mat) {
		if (mat.empty() || mat.total() == 0 || mat.data == nullptr)
			return false;

		if (mat.channels() == 1) { // 단일
			return cv::countNonZero(mat) > 0;
		}

		// 다채널 변환
		cv::Mat gray;
		cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
		return cv::countNonZero(gray) > 0;
	}

	std::vector<ImageUtile::PatchInfo> ImageUtile::ExtractValidPatches(
		const cv::Mat& image,
		const cv::Mat& mask,
		const cv::Size& patch_size,
		int stride,
		double min_valid_ratio)
	{
		std::vector<ImageUtile::PatchInfo> patches;

		if (image.empty() || mask.empty() || patch_size.width <= 0 || patch_size.height <= 0)
			return patches;

		if (mask.type() != CV_8UC1)
			throw std::invalid_argument("mask must be CV_8UC1");

		// 이미지 전체 크기 정의
		const cv::Rect image_bounds(0, 0, image.cols, image.rows);

		for (int y = 0; y <= image.rows - 1; y += stride) {
			for (int x = 0; x <= image.cols - 1; x += stride) {
				const cv::Rect roi(x, y, patch_size.width, patch_size.height);
				const cv::Rect valid_roi = roi & image_bounds;

				if (valid_roi.width <= 0 || valid_roi.height <= 0)
					continue;

				const cv::Mat mask_patch = mask(valid_roi);
				double valid_ratio = static_cast<double>(cv::countNonZero(mask_patch)) / valid_roi.area();
				if (valid_ratio <= min_valid_ratio)
					continue;

				cv::Mat patch(patch_size, image.type(), cv::Scalar::all(0));
				image(valid_roi).copyTo(patch(cv::Rect(0, 0, valid_roi.width, valid_roi.height)));

				patches.push_back({ roi, patch });
			}
		}

		return patches;
	}

	cv::Mat AIService::ImageUtile::RestoreMaskFromPatches(
		const std::vector<PatchInfo>& patches,
		const cv::Size& full_image_size)
	{
		if (patches.empty())
			return cv::Mat();

		cv::Mat restored = cv::Mat::zeros(full_image_size, CV_8UC1);

		for (const auto& patch : patches) {
			cv::Rect image_bounds(0, 0, full_image_size.width, full_image_size.height);
			cv::Rect target_roi = patch.roi & image_bounds;

			if (target_roi.area() <= 0)
				continue;

			cv::Rect patch_roi(0, 0, target_roi.width, target_roi.height);

			cv::Mat patch_part = patch.patch_image(patch_roi);
			//if (patch_part.channels() == 3) {
			//	cv::Mat gray_patch;
			//	cv::cvtColor(patch_part, gray_patch, cv::COLOR_BGR2GRAY); -> GRAY 로 변경 수식에 의해서 데이터 변로됨. 
			//	patch_part = std::move(gray_patch);
			//}
			if (patch_part.channels() == 3) {
				// R 채널만 사용 (R=G=B 라고 가정됨)
				std::vector<cv::Mat> channels;
				cv::split(patch_part, channels);
				patch_part = channels[2]; // R 채널
			}
			else if (patch_part.channels() != 1) {
				// std::cerr << "Unsupported channel count: " << patch_part.channels() << std::endl;
				continue;
			}

			cv::Mat dest_part = restored(target_roi);
			//patch_part.copyTo(dest_part);
			cv::bitwise_or(dest_part, patch_part, dest_part);
		}

		return restored;
	}

	cv::Mat AIService::ImageUtile::LabelConnectedComponents(const cv::Mat& binary_mask)
	{
		if (binary_mask.empty() || binary_mask.type() != CV_8UC1) {
			throw std::invalid_argument("LabelConnectedComponentsLimited: 입력은 CV_8UC1 타입의 바이너리 마스크여야 합니다.");
		}

		// 16비트 라벨링으로 먼저 연결 성분 추출
		cv::Mat labels_16u;
		int num_labels = cv::connectedComponents(binary_mask, labels_16u, 8, CV_16U);

		if (num_labels <= 1) {
			return cv::Mat::zeros(binary_mask.size(), CV_8UC1);
		}

		// 라벨 0: 배경, 라벨 1~num_labels-1: 객체
		struct BlobInfo {
			int label;
			int top;
			int left;
		};

		std::vector<BlobInfo> blob_infos;
		for (int lbl = 1; lbl < num_labels; ++lbl) {
			cv::Mat blob_mask = (labels_16u == lbl);
			cv::Rect bbox = cv::boundingRect(blob_mask);
			blob_infos.push_back({ lbl, bbox.y, bbox.x });
		}

		// 좌상단 기준 정렬
		std::sort(blob_infos.begin(), blob_infos.end(), [](const BlobInfo& a, const BlobInfo& b) {
			if (a.top == b.top) return a.left < b.left;
			return a.top < b.top;
				  });

		// 최대 라벨 개수 제한: 254개 (1~254)
		const int max_allowed_labels = 254;
		const int actual_labels = (blob_infos.size() < max_allowed_labels)
			? static_cast<int>(blob_infos.size())
			: max_allowed_labels;

		// 라벨 재매핑 테이블
		std::unordered_map<int, uchar> label_remap;
		for (int i = 0; i < actual_labels; ++i) {
			label_remap[blob_infos[i].label] = static_cast<uchar>(i + 1); // 라벨 1부터 시작
		}

		// 결과 출력용 CV_8UC1 이미지 생성
		cv::Mat labeled = cv::Mat::zeros(labels_16u.size(), CV_8UC1);
		for (int y = 0; y < labels_16u.rows; ++y) {
			for (int x = 0; x < labels_16u.cols; ++x) {
				ushort orig_label = labels_16u.at<ushort>(y, x);
				auto it = label_remap.find(orig_label);
				if (it != label_remap.end()) {
					labeled.at<uchar>(y, x) = it->second;
				}
			}
		}

		return labeled;
	}


	bool ImageUtile::RoiImagePreprocessing(const HalconCpp::HObject& image, const HalconCpp::HObject& roi_region,
										   const HalconCpp::HTuple length1, const HalconCpp::HTuple length2,
										   HalconCpp::HObject& roi_image, cv::Mat& cv_roi_image,
										   HalconCpp::HObject& roi_image_v2, cv::Mat& cv_roi_image_v2,
										   HalconCpp::HTuple& out_org_image_width, HalconCpp::HTuple& out_org_image_height,
										   HalconCpp::HTuple& out_crop_row1, HalconCpp::HTuple& out_crop_col1,
										   HalconCpp::HTuple& out_crop_row2, HalconCpp::HTuple& out_crop_col2,
										   int padding, int dilation, std::shared_ptr<spdlog::logger> logger)
	{
		using clock_t = std::chrono::steady_clock;

		auto elapsed_ms = [](clock_t::time_point t0) -> double {
			return std::chrono::duration<double, std::milli>(clock_t::now() - t0).count();
			};

		auto log_ms = [&](const char* tag, double ms) {
			if (logger) {
				logger->info("[RoiImagePreprocessing] {}: {:.3f} ms", tag, ms);
			}
			};

		auto log_scope_start = [&](const char* tag) {
			if (logger) {
				logger->info("[RoiImagePreprocessing] {} start", tag);
			}
			};

		const auto t_total_start = clock_t::now();

		try {
			// -----------------------------
			// 1) ROI bounding box + image size
			// -----------------------------
			log_scope_start("SmallestRectangle1 + GetImageSize");
			auto t = clock_t::now();

			HalconCpp::HTuple center_row, center_col;
			HalconCpp::HTuple row1, col1, row2, col2;
			HalconCpp::HTuple crop_row1, crop_col1, crop_row2, crop_col2;

			SmallestRectangle1(roi_region, &row1, &col1, &row2, &col2);
			HalconCpp::HTuple width, height;
			GetImageSize(image, &width, &height);

			log_ms("SmallestRectangle1 + GetImageSize", elapsed_ms(t));

			// -----------------------------
			// 2) crop 좌표 계산 (경계 보정 포함)
			// -----------------------------
			log_scope_start("crop coordinate compute");
			t = clock_t::now();

			crop_row1 = row1 - padding;
			if (crop_row1 >= 0) {
				crop_row2 = row2 + padding;
				if (crop_row2 > height) {
					crop_row1 -= crop_row2 - height;
					crop_row2 = height;
				}
			}
			else {
				crop_row2 = row2 + padding - crop_row1;
				crop_row1 = 0;
			}

			crop_col1 = col1 - padding;
			if (crop_col1 >= 0) {
				crop_col2 = col2 + padding;
				if (crop_col2 > width) {
					crop_col1 -= crop_col2 - width;
					crop_col2 = width;
				}
			}
			else {
				crop_col2 = col2 + padding - crop_col1;
				crop_col1 = 0;
			}

			log_ms("crop coordinate compute", elapsed_ms(t));

			// -----------------------------
			// 3) ROI Image Type 1: CropRectangle1
			// -----------------------------
			log_scope_start("Type1 CropRectangle1");
			t = clock_t::now();

			HalconCpp::HObject ho_cropped;
			CropRectangle1(image, &ho_cropped, crop_row1, crop_col1, crop_row2, crop_col2);

			log_ms("Type1 CropRectangle1", elapsed_ms(t));

			// -----------------------------
			// 4) Type 1: HImage2Mat 변환
			// -----------------------------
			log_scope_start("Type1 HImage2Mat");
			t = clock_t::now();

			roi_image = ho_cropped;
			// AIService::ImageUtile::HImage2MatFastCopy(ho_cropped, cv_roi_image);
			cv_roi_image = AIService::ImageUtile::HImage2Mat(ho_cropped);

			log_ms("Type1 HImage2Mat", elapsed_ms(t));

#ifdef LGITAI
			bool type2_use_dilation = false;

			if (type2_use_dilation) {
				// -----------------------------
				// 5-A) Type2(dilation): Dilation + Closing
				// -----------------------------
				log_scope_start("Type2(dilation) DilationRectangle1 + ClosingCircle");
				t = clock_t::now();

				HalconCpp::HObject ho_roi_padded, ho_roi_smooth, ho_reduced_image;
				HalconCpp::DilationRectangle1(roi_region, &ho_roi_padded, dilation, dilation);
				HalconCpp::ClosingCircle(ho_roi_padded, &ho_roi_smooth, 4.0);

				log_ms("Type2(dilation) DilationRectangle1 + ClosingCircle", elapsed_ms(t));

				// -----------------------------
				// 5-B) Type2(dilation): ReduceDomain
				// -----------------------------
				log_scope_start("Type2(dilation) ReduceDomain");
				t = clock_t::now();

				HalconCpp::ReduceDomain(image, ho_roi_smooth, &ho_reduced_image);

				log_ms("Type2(dilation) ReduceDomain", elapsed_ms(t));

				// -----------------------------
				// 5-C) Type2(dilation): CropRectangle1
				// -----------------------------
				log_scope_start("Type2(dilation) CropRectangle1");
				t = clock_t::now();

				HalconCpp::HObject ho_cropped_reduced_image;
				HalconCpp::CropRectangle1(ho_reduced_image, &ho_cropped_reduced_image,
										  crop_row1, crop_col1, crop_row2, crop_col2);

				log_ms("Type2(dilation) CropRectangle1", elapsed_ms(t));

				// -----------------------------
				// 5-D) Type2(dilation): HImage2Mat
				// -----------------------------
				log_scope_start("Type2(dilation) HImage2Mat");
				t = clock_t::now();

				roi_image_v2 = ho_cropped_reduced_image;
				cv_roi_image_v2 = AIService::ImageUtile::HImage2Mat(ho_cropped_reduced_image);

				log_ms("Type2(dilation) HImage2Mat", elapsed_ms(t));
			}
			else {
				// -----------------------------
				// 5-E) Type2: ReduceDomain
				// -----------------------------
				log_scope_start("Type2 ReduceDomain");
				t = clock_t::now();

				HalconCpp::HObject ho_reduced_image;
				ReduceDomain(image, roi_region, &ho_reduced_image);

				log_ms("Type2 ReduceDomain", elapsed_ms(t));

				// -----------------------------
				// 5-F) Type2: CropRectangle1
				// -----------------------------
				log_scope_start("Type2 CropRectangle1");
				t = clock_t::now();

				HalconCpp::HObject ho_cropped_reduced_image;
				CropRectangle1(ho_reduced_image, &ho_cropped_reduced_image,
							   crop_row1, crop_col1, crop_row2, crop_col2);

				log_ms("Type2 CropRectangle1", elapsed_ms(t));

				// -----------------------------
				// 5-G) Type2: HImage2Mat
				// -----------------------------
				log_scope_start("Type2 HImage2Mat");
				t = clock_t::now();

				roi_image_v2 = ho_cropped_reduced_image;
				cv_roi_image_v2 = AIService::ImageUtile::HImage2Mat(ho_cropped_reduced_image);

				log_ms("Type2 HImage2Mat", elapsed_ms(t));
			}
#endif // LGITAI

			// -----------------------------
			// 6) output set
			// -----------------------------
			log_scope_start("output set");
			t = clock_t::now();

			out_org_image_width = width;
			out_org_image_height = height;
			out_crop_row1 = crop_row1;
			out_crop_col1 = crop_col1;
			out_crop_row2 = crop_row2;
			out_crop_col2 = crop_col2;

			log_ms("output set", elapsed_ms(t));

			// -----------------------------
			// total
			// -----------------------------
			log_ms("TOTAL", elapsed_ms(t_total_start));
		}
		catch (const std::exception& e) {
			if (logger) {
				logger->info("[RoiImagePreprocessing] Exception: {}", e.what());
				logger->info("[RoiImagePreprocessing] TOTAL(until exception): {:.3f} ms", elapsed_ms(t_total_start));
			}
			return false;
		}
		catch (const HalconCpp::HException& ex) {
			if (logger) {
				logger->info("[RoiImagePreprocessing] Halcon Error: {}", ex.ErrorMessage().Text());
				logger->info("[RoiImagePreprocessing] TOTAL(until halcon exception): {:.3f} ms", elapsed_ms(t_total_start));
			}
			return false;
		}
		catch (...) {
			if (logger) {
				logger->info("[RoiImagePreprocessing] Unknown exception.");
				logger->info("[RoiImagePreprocessing] TOTAL(until unknown exception): {:.3f} ms", elapsed_ms(t_total_start));
			}
			return false;
		}

		return true;
	}
} // namespace AIService
