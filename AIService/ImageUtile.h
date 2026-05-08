#pragma once
#include "../LGIT/Model/BoundingBox.h"
#include <chrono>
#include <filesystem>
#include <halconcpp/HalconCpp.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include <optional>
#include <spdlog/logger.h>
#include <unordered_map>
#include <variant>

namespace AIService {
	class ImageUtile
	{
	public:
		/**
		 * @brief 이미지 패치 정보 구조체
		 */
		struct PatchInfo {
			cv::Rect roi;         ///< 원본 이미지 내 위치
			cv::Mat patch_image;  ///< 패치 이미지 (1채널 또는 3채널)

			PatchInfo(const cv::Rect& roi_, const cv::Mat& patch_)
				: roi(roi_), patch_image(patch_) {
			}
		};

	public:
		/**
		 * @brief HObject를 OpenCV Mat 형식으로 변환
		 * @param h_img 변환할 Halcon HObject 이미지
		 * @return 변환된 OpenCV Mat 이미지
		 */
		static cv::Mat HImage2Mat(const HalconCpp::HObject& h_img);

		/**
		 * @brief Halcon HObject 이미지를 OpenCV Mat 형식으로 빠르게 복사
		 * @param h_img 변환할 Halcon HObject 이미지
		 * @param out 변환된 OpenCV Mat 이미지가 저장될 참조
		 */
		static inline void HImage2MatFastCopy(const HalconCpp::HObject& h_img, cv::Mat& out);

		/**
		 * @brief Halcon HObject (Gray 이미지)를 OpenCV Mat 형식으로 빠르게 복사
		 * @param img_byte 변환할 Halcon HObject 이미지 (Gray, Byte 타입)
		 * @param out 변환된 OpenCV Mat 이미지가 저장될 참조
		 */
		static inline void HImage2Mat_GrayCopy(const HObject& img_byte, cv::Mat& out);

		/**
		 * @brief Halcon HObject (BGR 이미지)를 OpenCV Mat 형식으로 빠르게 복사
		 * @param img_byte 변환할 Halcon HObject 이미지 (BGR, Byte 타입)
		 * @param out 변환된 OpenCV Mat 이미지가 저장될 참조
		 */
		static inline void HImage2Mat_BGRMergeCopy(const HObject& img_byte, cv::Mat& out);

		/**
		 * @brief OpenCV Mat을 Halcon HObject 형식으로 변환
		 * @param mat 변환할 OpenCV Mat 이미지
		 * @return 변환된 Halcon HObject 이미지
		 */
		static HalconCpp::HObject Mat2HImage(const cv::Mat& mat);

		/**
		 * @brief Halcon Region (HObject)을 OpenCV 이진 마스크(cv::Mat)로 변환
		 * @param region 변환할 Halcon Region 객체
		 * @return 변환된 OpenCV 이진 마스크 (255=Region, 0=Background)
		 */
		static cv::Mat HRegion2Mask(const HalconCpp::HObject& region);

		/**
		 * @brief OpenCV 이진 마스크(cv::Mat) → Halcon Region(HObject) 변환
		 * @param mask 이진 마스크 이미지 (CV_8UC1)
		 * @return 변환된 Halcon Region
		 */
		static HalconCpp::HObject Mask2HRegion(const cv::Mat& mask);

		/**
		 * @brief OpenCV 이미지에서 지정된 크기와 스트라이드로 잘라서 std::vector에 저장 (필요시 패딩)
		 * @param input 입력 이미지
		 * @param crop_size 크롭할 크기 (예: 200x200)
		 * @param stride 슬라이딩 간격 (예: 200)
		 * @param crops 잘라낸 결과 이미지들이 저장될 벡터
		 * @return true: 성공 / false: 실패 (입력 유효성 문제 등)
		 */
		static bool CropImageWithPadding(const cv::Mat& input, const cv::Size& crop_size, int stride, std::vector<cv::Mat>& crops);

		/**
		 * @brief OpenCV 이미지를 지정된 경로에 저장
		 * @param root_path 저장 루트 경로
		 * @param lot_id LOT ID
		 * @param tray_id Tray ID
		 * @param module_id Module ID
		 * @param pos_no 위치 번호
		 * @param image_index 이미지 인덱스
		 * @param roi_index ROI 인덱스
		 * @param tab_index Tab 인덱스
		 * @param barcode 바코드 문자열
		 * @param cv_image 저장할 OpenCV 이미지
		 */
		static void CvSaveImage(
			const std::string& root_path, const std::string& lot_id, const std::string& tray_id,
			const std::string& module_id, const std::string& pos_no, const std::string& image_index,
			const std::string& roi_index, const std::string& tab_index, const std::string& barcode,
			cv::Mat cv_image);

		/**
		 * @brief Halcon HObject 이미지를 지정된 경로에 저장
		 * @param root_path 저장 루트 경로
		 * @param equip_id 장비 ID
		 * @param lot_id LOT ID
		 * @param tray_id Tray ID
		 * @param module_id Module ID
		 * @param pos_no 위치 번호
		 * @param image_index 이미지 인덱스
		 * @param roi_index ROI 인덱스
		 * @param tab_index Tab 인덱스
		 * @param barcode 바코드 문자열
		 * @param h_image 저장할 Halcon HObject 이미지
		 */
		static void SaveImage(const std::string& root_path, const std::string& equip_id, const std::string& model_type,
							  const std::string& lot_id, const std::string& tray_id, const std::string& module_id,
							  const std::string& vision_name, const std::string& pc_index, const std::string& stage_index,
							  const std::string& image_index, const std::string& roi_index, const std::string& inspectiontype_name, const std::string& tab_index,
							  const std::string& barcode, HalconCpp::HObject h_image, HalconCpp::HObject h_image_v2,
							  const std::string& local_align_angle_string, const std::string& channel_type);

		static void SaveImage(const std::string root_path,
							  const std::string prefix,
							  HalconCpp::HObject h_image);

		static HObject Reg2Image(const HalconCpp::HObject& reg, const int& Width, const int& Height);

		/**
		 * @brief Halcon Region을 이미지로 저장
		 * @param region 저장할 Halcon Region 객체
		 * @param filePath 저장할 경로
		 * @param width 저장할 이미지 너비 (기본값 0이면 자동 계산)
		 * @param height 저장할 이미지 높이 (기본값 0이면 자동 계산)
		 */
		static void SaveRegionAsImage(const HalconCpp::HObject& region,
									  const std::string& filePath,
									  int width = 0,
									  int height = 0);

		/**
		 * @brief Region map을 순회하며 이미지로 저장
		 * @param regionData 저장할 Region 데이터 (unordered_map)
		 * @param folderPath 저장할 폴더 경로
		 * @param width 저장할 이미지 너비 (기본값: 0 → 자동 크기)
		 * @param height 저장할 이미지 높이 (기본값: 0 → 자동 크기)
		 * @param addImageName 저장 파일명에 추가할 접두 문자열
		 */
		static void SaveRegionsFromMap(const std::unordered_map<int, HalconCpp::HObject>& regionData,
									   const std::string& folderPath,
									   int width = 0,
									   int height = 0,
									   std::string addImageName = "");

		/**
		 * @brief 두 번째 리전의 한 점이 첫 번째 리전에 속하는지 검사
		 * @param region1 비교 대상 첫 번째 Region (HObject)
		 * @param region2 비교 대상 두 번째 Region (HObject)
		 * @return 두 리전이 겹치면 true, 아니면 false 반환
		 */
		static bool HasIntersection(const HalconCpp::HObject& region1, const HalconCpp::HObject& region2);

		/**
		 * @brief 교집합을 계산 후, 영역 크기를 확인하여 겹치는지 검사
		 * @param region1 비교 대상 첫 번째 Region (HObject)
		 * @param region2 비교 대상 두 번째 Region (HObject)
		 * @return 두 리전의 교집합 영역이 0보다 크면 true, 아니면 false 반환
		 */
		static bool HasIntersection2(const HalconCpp::HObject& region1, const HalconCpp::HObject& region2);

		/**
		 * @brief ROI 영역(HObject)과 BoundingBox 정보를 바탕으로, 원본 이미지 좌표계에 맞게 리전을 복원
		 * @param roi_region ROI 내 상대 좌표로 구성된 리전
		 * @param roi_box ROI의 위치 정보 (BoundingBox)
		 * @param restored_region 원본 이미지 좌표계로 복원된 리전 (출력)
		 */
		static void RestoreRegionToOriginal(const HalconCpp::HObject& roi_region, const LGIT::Model::BoundingBox& roi_box, HalconCpp::HObject& restored_region);

		static void RestoreRegionToOriginal(const HalconCpp::HObject& roi_region, const cv::Rect& roi_rect, HalconCpp::HObject& restored_region);

		/**
		 * @brief 입력된 HObject가 유효한 Region인지 검사합니다 -> CGFunction::ValidHRegion 같음

		 * 이 함수는 HObject가 초기화되어 있고, 하나 이상의 유효한 (면적이 0보다 큰) Region을 포함하고 있는지 확인합니다.
		 * 예외가 발생하거나 조건을 만족하지 않으면 false를 반환합니다.
		 *
		 * @param region 검사할 HObject (Region으로 예상되는 객체)
		 * @return true 유효한 Region이 존재할 경우
		 * @return false 유효하지 않거나, 예외가 발생하거나, 면적이 0 이하인 경우
		 */
		static bool ValidHRegion(const HalconCpp::HObject& region);


		/**
		 * @brief Region 객체의 연결 성분에 대해 1~255 값으로 라벨링된 Halcon Grayscale 이미지 생성
		 *
		 * 입력된 Region에서 연결 성분을 추출하고, 각 성분에 대해 고유한 라벨 값(1~255)을 할당하여
		 * 원본 이미지와 같은 크기의 8비트 Grayscale 이미지로 출력합니다.
		 *
		 * @param region 입력 Region (HObject)
		 * @param width 결과 이미지의 너비
		 * @param height 결과 이미지의 높이
		 * @return 연결 성분별 라벨이 적용된 Halcon 이미지 (byte 타입)
		 */
		static HalconCpp::HObject RegionToLabeledImage(const HalconCpp::HObject& region, int width, int height);

		static std::optional<cv::Mat> RegionToLabeledImageV2(const HalconCpp::HObject& region,
															 const cv::Size& orignal_image_size,
															 const cv::Size& roi_image_size,
															 const cv::Point& roi_left_top,
															 const cv::Point& roi_right_bottom);

		/**
		 * @brief 입력된 단일 채널 이미지에서 지정된 하나의 값만 추출한 새 이미지 생성
		 * @param input 입력 이미지 (CV_8UC1, 단일 채널)
		 * @param value 추출할 픽셀 값 (e.g., 1)
		 * @return 지정된 값만 유지된 새 이미지 (기타 영역은 0)
		 */
		static cv::Mat ExtractSpecificValue(const cv::Mat& input, int value);

		/**
		 * @brief 이미지에 유효한 데이터가 있는지 확인
		 * @param mat 검사할 OpenCV 이미지
		 * @return true: 데이터 있음 / false: 비어 있음
		 */
		static bool HasImageData(const cv::Mat& mat);

		/**
		 * @brief 마스크가 있는 영역만 이미지 패치 추출 (경계는 0으로 패딩)
		 *
		 * @param image 입력 이미지
		 * @param mask 동일 크기의 마스크 이미지
		 * @param patch_size 패치 크기 (예: 200x200)
		 * @param stride 패치 간격
		 * @return 유효 마스크를 가진 이미지 패치 목록
		 */
		static std::vector<PatchInfo> ExtractValidPatches(const cv::Mat& image,
														  const cv::Mat& mask,
														  const cv::Size& patch_size,
														  int stride,
														  double min_valid_ratio = 0.0);

		/**
		 * @brief 패치 정보를 기반으로 마스크 이미지를 복원
		 *
		 * @param patches 추출된 패치 목록
		 * @param full_image_size 원래 이미지의 크기
		 * @return 복원된 1채널 마스크 이미지
		 */
		static cv::Mat RestoreMaskFromPatches(
			const std::vector<PatchInfo>& patches,
			const cv::Size& full_image_size);

		static cv::Mat LabelConnectedComponents(const cv::Mat& binary_mask);

		/**
		 * @brief AI ROI별 검사 전, ROI 이미지 처리
		 *        원본 이미지에서 ROI 리전 영역을 잘라내고, 해당 영역의 외접 사각형 좌표 및 ROI 이미지(OpenCV Mat) 출력
		 *
		 * @param image         원본 Halcon 이미지
		 * @param pInspectROIRgn    ROI 리전 (검사 대상 영역)
		 * @param roi_image     Halcon 객체 형태로 처리된 ROI 이미지 (출력)
		 * @param row1          원본 이미지에서 ROI 리전의 최소 외접 사각형 Top 좌표 (출력)
		 * @param col1          원본 이미지에서 ROI 리전의 최소 외접 사각형 Left 좌표 (출력)
		 * @param row2          원본 이미지에서 ROI 리전의 최소 외접 사각형 Bottom 좌표 (출력)
		 * @param col2          원본 이미지에서 ROI 리전의 최소 외접 사각형 Right 좌표 (출력)
		 * @param cv_roi_image  ROI 영역에 해당하는 OpenCV 이미지 (출력)
		 * @param padding       ROI 영역에 적용할 여유 패딩 값 (기본값 0)
		 *
		 * @return 처리 성공 여부
		 */
		static bool RoiImagePreprocessing(const HalconCpp::HObject& image, const HalconCpp::HObject& roi_region,
										  const HalconCpp::HTuple length1, const HalconCpp::HTuple length2,
										  HalconCpp::HObject& roi_image, cv::Mat& cv_roi_image,
										  HalconCpp::HObject& roi_image_v2, cv::Mat& cv_roi_image_v2,
										  HalconCpp::HTuple& out_org_image_width, HalconCpp::HTuple& out_org_image_height,
										  HalconCpp::HTuple& out_crop_row1, HalconCpp::HTuple& out_crop_col1,
										  HalconCpp::HTuple& out_crop_row2, HalconCpp::HTuple& out_crop_col2,
										  int padding = 4, int dilation = 5, std::shared_ptr<spdlog::logger> logger = nullptr);

		/**
		 * @brief optional variant에서 HalconCpp::HObject를 추출하는 내부 헬퍼
		 * @param[in]  src        원본 optional variant
		 * @param[out] out_region 추출된 HObject가 담길 참조
		 * @return 값이 존재하고 HObject를 보유하면 true, 아니면 false
		 */
		static bool GetHObject(const std::optional<std::variant<HalconCpp::HObject, cv::Mat>>& src,
							   HalconCpp::HObject& out_region)
		{
			if (!src.has_value())
				return false;
			if (std::holds_alternative<HalconCpp::HObject>(*src))
			{
				const auto* ptr = std::get_if<HalconCpp::HObject>(&src.value());
				if (ptr == nullptr)
					return false;
				out_region = *ptr;
			}
			else if (std::holds_alternative<cv::Mat>(*src))
			{
				const auto* ptr = std::get_if<cv::Mat>(&src.value());
				if (ptr == nullptr)
					return false;
				out_region = AIService::ImageUtile::Mask2HRegion(*ptr);
			}
			return true;
		}

		/**
		 * @brief optional variant에서 cv::Mat를 추출하는 내부 헬퍼
		 * @param[in]  src     원본 optional variant
		 * @param[out] out_mat 추출된 cv::Mat가 담길 참조
		 * @return 값이 존재하고 cv::Mat를 보유하면 true, 아니면 false
		 */
		static bool GetMat(const std::optional<std::variant<HalconCpp::HObject, cv::Mat>>& src,
						   cv::Mat& out_mat)
		{
			if (!src.has_value())
				return false;
			if (std::holds_alternative<HalconCpp::HObject>(*src))
			{
				const auto* ptr = std::get_if<HalconCpp::HObject>(&src.value());
				if (ptr == nullptr)
					return false;
				out_mat = AIService::ImageUtile::HRegion2Mask(*ptr);
				return true;
			}
			else if (std::holds_alternative<cv::Mat>(*src))
			{
				const auto* ptr = std::get_if<cv::Mat>(&src.value());
				if (ptr == nullptr)
					return false;
				out_mat = *ptr;
			}
			return true;
		}
	};
}

// 구현은 별도 CPP 파일에 위치해야 합니다.
