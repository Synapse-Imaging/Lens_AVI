#pragma once
#include "AIService/ImageUtile.h"
#include <chrono>
#include <HalconCpp.h>
#include <HObject.h>
#include <map>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <optional>
#include <string>
#include <variant>
#include <vector>


namespace AIService::UniversalAIData {
	/**
	 * @brief 로깅 정보 구조체
	 *
	 * AI 검사 수행 시 로깅에 필요한 장비 및 LOT 식별 정보를 담는 구조체.
	 * 검사 이력 추적 및 로그 기록 시 사용된다.
	 *
	 * @note 모든 문자열 필드는 기본값으로 빈 문자열("")이 설정됨
	 *
	 * 예제:
	 * @code
	 *     LoggingInfo info;
	 *     info.equip_no = "EQP-001";
	 *     info.lot_id = "LOT-20240101-A";
	 * @endcode
	 */
	struct LoggingInfo
	{
		std::string model_name;					///< 비전 모델 이름? (Model Name)
		std::string ai_model_name;				///< AI 모델 이름? (AI Model Name)
		std::string equip_no;					///< 장비 번호 (Equipment Number)
		std::string lot_id;						///< LOT 식별자 (Lot ID)
		std::string barcode;					///< 바코드 정보 (Barcode)
	};

	/**
	 * @brief 검색 조건 정보 구조체
	 *
	 * AI 검사 대상 이미지를 특정하기 위한 인덱스 정보를 담는 구조체.
	 * 버퍼, 카메라 타입, 이미지, ROI, 탭 등 다차원 인덱스를 통해
	 * 검사 대상을 고유하게 식별한다.
	 *
	 * @note 모든 인덱스의 기본값은 -1이며, 이는 미설정 상태를 의미함
	 *
	 * @endcode
	 */
	struct SearchInfo
	{
		int buffer_idx = -1;					///< 버퍼 인덱스
		int vision_camera_type = -1;			///< 비전 카메라 타입
		int magazine_no = -1;					///< 매거진 번호
		int image_idx = -1;						///< 이미지 인덱스
		int roi_idx = -1;						///< ROI(관심 영역) 인덱스
		int tab_idx = -1;						///< 탭 인덱스
	};

	/**
	 * @brief 1차 검사 결과 정보 구조체
	 *
	 * 1차 검사에서 생성된 결함 영역 마스크를 담는 구조체.
	 */
	struct PrimaryInspectionInfo
	{
		bool result = false;					///< 검사 성공 여부
		cv::Mat defect_mask;					///< 1차 검사에서 생성된 결함 영역 마스크
	};

	/**
	 * @brief SVM 검사 결과 정보 구조체
	 *
	 * SVM 검사에서 생성된 결함 영역 마스크를 담는 구조체.
	 */
	struct SVMInfo
	{
		bool result = false;
		cv::Mat ng_mask;
	};

	/**
	 * @brief 분류 검사 결과 정보 구조체
	 *
	 * 분류 검사에서 생성된 결함 영역 마스크 및 Blob 정보를 담는 구조체.
	 */
	struct ClassificationInfo
	{
		bool result = false;					///< 검사 성공 여부
		int blob_count = 0;						///< Blob 총 개수
		std::vector<float> score_list;			///< 각 Blob의 confidence score 리스트
		cv::Mat ok_mask;						///< 양품 Blob 마스크
		std::map<int, cv::Mat> ng_mask_map;		///< Blob ID → Blob 마스크 매핑 정보
	};

	/**
	 * @brief 세그멘테이션 검사 결과 정보 구조체
	 *
	 * 세그멘테이션 검사에서 생성된 결함 영역 마스크 및 Blob 정보를 담는 구조체.
	 */
	struct SegmentationInfo
	{
		bool result = false;					///< 검사 성공 여부
		std::map<int, cv::Mat> mask_map;		///< Blob ID와 해당 Blob 마스크의 리스트
	};
	
	/**
	 * @brief 검사 결과 정보 구조체
	 *
	 * 다단계 AI 검사(1차~3차)의 성공 여부 및 각 단계에서 생성된
	 * 결함 영역(Defect Region) 정보를 담는 구조체.
	 * 비전 검사, SVM 분류, 딥러닝 등 각 단계별 검사 결과를
	 * HalconCpp::HObject 또는 cv::Mat 형태의 리전으로 보관한다.
	 *
	 * @note 결함 영역 필드는 std::optional<std::variant<HalconCpp::HObject, cv::Mat>>로 래핑되어 있으며,
	 *       해당 단계의 검사가 수행되지 않았거나 variant 내부 타입이 요청 타입과 다른 경우
	 *       getter 함수가 false를 반환한다.
	 *
	 * @endcode
	 */
	struct InspectionInfo{
		PrimaryInspectionInfo primary_info;		///< 1차 검사 결과 정보
		SVMInfo svm_info;						///< SVM 검사 결과 정보
		ClassificationInfo cls_info;			///< 분류 검사 결과 정보
		SegmentationInfo seg_info;				///< 세그멘테이션 검사 결과 정보
	};

	/**
	 * @brief AI 검사 통합 데이터 구조체
	 *
	 * 로깅 정보, 검색 조건, 검사 결과를 하나로 묶어
	 * AI 검사 파이프라인 전반에서 사용되는 범용 데이터 구조체.
	 * 검사 요청부터 결과 반환까지 단일 객체로 데이터를 전달한다.
	 *
	 */
	struct UniversalInfo {
		LoggingInfo logging_info;				///< 로깅 정보
		SearchInfo search_info;					///< 검색 조건 정보
		InspectionInfo inspection_info;			///< 검사 결과 정보
	};
}