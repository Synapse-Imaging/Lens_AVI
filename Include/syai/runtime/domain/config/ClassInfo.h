#pragma once

#include "syai/runtime/SimpleSmartType.h"
#include "ChainedInspectionInfo.h"
#include "BlobAnalysisModelInfo.h"
#include <optional>

namespace syai::runtime::domain::config
{
	/**
	 * @brief 단일 클래스의 모든 정보를 담는 구조체
	 *
	 * C#의 ClassInfo와 동일한 개념으로 각 클래스의 모든 정보를 통합 관리합니다:
	 * - 클래스 인덱스 및 레이블
	 * - 탐지 임계값
	 * - Chained Inspection 설정 (이 클래스에만 적용)
	 * - Blob Analysis 모델 설정 (이 클래스에만 적용, Segmentation 전용)
	 *
	 * @see ModelInfoItem, ChainedInspectionInfo, BlobAnalysisModelInfo
	 */
	struct ClassInfo
	{
		int class_index = 0;                                  //!< 클래스 인덱스 (0부터 시작)
		SmartString label;                                    //!< 클래스 레이블 (예: "OK", "NG", "Scratch")
		double threshold = 0.5;                               //!< 탐지 임계값 (0.0 ~ 1.0)
		ChainedInspectionInfo chained_inspection;            //!< Chained Inspection 설정 (이 클래스에만 적용)
		std::optional<BlobAnalysisModelInfo> blob_analysis_model;           //!< Blob Analysis 모델 설정 (이 클래스에만 적용)

		// ========== 생성자 ==========

		/**
		 * @brief 기본 생성자
		 */
		ClassInfo() = default;

		/**
		 * @brief 매개변수 생성자 (기본 정보만)
		 * @param class_idx 클래스 인덱스
		 * @param lbl 클래스 레이블
		 * @param thr 임계값 (0.0 ~ 1.0)
		 */
		ClassInfo(int class_idx, const char* lbl = "", double thr = 0.5)
			: class_index(class_idx), label(lbl), threshold(thr)
		{
			validate_threshold();
		}

		/**
		 * @brief 전체 정보 포함 생성자
		 * @param class_idx 클래스 인덱스
		 * @param lbl 클래스 레이블
		 * @param thr 임계값
		 * @param chained Chained Inspection 정보
		 * @param blob Blob Analysis 모델 정보
		 */
		ClassInfo(int class_idx, const char* lbl, double thr,
				  const ChainedInspectionInfo& chained,
				  const std::optional<BlobAnalysisModelInfo>& blob)
			: class_index(class_idx)
			, label(lbl)
			, threshold(thr)
			, chained_inspection(chained)
			, blob_analysis_model(blob)
		{
			validate_threshold();
		}

		/**
		 * @brief 전체 정보 포함 생성자 (BlobAnalysisModelInfo 직접 전달)
		 * @param class_idx 클래스 인덱스
		 * @param lbl 클래스 레이블
		 * @param thr 임계값
		 * @param chained Chained Inspection 정보
		 * @param blob Blob Analysis 모델 정보 (직접 값)
		 */
		ClassInfo(int class_idx, const char* lbl, double thr,
				  const ChainedInspectionInfo& chained,
				  const BlobAnalysisModelInfo& blob)
			: class_index(class_idx)
			, label(lbl)
			, threshold(thr)
			, chained_inspection(chained)
			, blob_analysis_model(blob)
		{
			validate_threshold();
		}

		/**
		 * @brief 복사 생성자
		 */
		ClassInfo(const ClassInfo& other)
			: class_index(other.class_index)
			, label(other.label)
			, threshold(other.threshold)
			, chained_inspection(other.chained_inspection)
			, blob_analysis_model(other.blob_analysis_model)
		{
		}

		/**
		 * @brief 이동 생성자
		 */
		ClassInfo(ClassInfo&& other) noexcept
			: class_index(other.class_index)
			, label(std::move(other.label))
			, threshold(other.threshold)
			, chained_inspection(std::move(other.chained_inspection))
			, blob_analysis_model(std::move(other.blob_analysis_model))
		{
		}

		// ========== 연산자 ==========

		/**
		 * @brief 복사 할당 연산자
		 */
		ClassInfo& operator=(const ClassInfo& other)
		{
			if (this != &other) {
				class_index = other.class_index;
				label = other.label;
				threshold = other.threshold;
				chained_inspection = other.chained_inspection;
				blob_analysis_model = other.blob_analysis_model;
			}
			return *this;
		}

		/**
		 * @brief 이동 할당 연산자
		 */
		ClassInfo& operator=(ClassInfo&& other) noexcept
		{
			if (this != &other) {
				class_index = other.class_index;
				label = std::move(other.label);
				threshold = other.threshold;
				chained_inspection = std::move(other.chained_inspection);
				blob_analysis_model = std::move(other.blob_analysis_model);
			}
			return *this;
		}

		/**
		 * @brief 동등성 비교
		 */
		inline bool operator==(const ClassInfo& other) const
		{
			return class_index == other.class_index &&
				   label == other.label &&
				   threshold == other.threshold;
			   // Chained Inspection과 Blob Analysis는 비교에서 제외 (복잡성 때문에)
		}

		/**
		 * @brief 부등성 비교
		 */
		inline bool operator!=(const ClassInfo& other) const
		{
			return !(*this == other);
		}

		// ========== Getter/Setter ==========

		/**
		 * @brief 클래스 인덱스 반환
		 */
		inline int get_class_index() const { return class_index; }

		/**
		 * @brief 클래스 레이블 반환
		 */
		inline const char* get_label() const { return label.c_str(); }

		/**
		 * @brief 임계값 반환
		 */
		inline double get_threshold() const { return threshold; }

		/**
		 * @brief 임계값 설정 (자동 범위 검증)
		 */
		inline void set_threshold(double value)
		{
			threshold = value;
			validate_threshold();
		}

		/**
		 * @brief 클래스 레이블 설정
		 */
		inline void set_label(const char* lbl) 
		{ 
			label = lbl; 
		}

		/**
		 * @brief Chained Inspection 반환
		 */
		inline const ChainedInspectionInfo& get_chained_inspection() const
		{
			return chained_inspection;
		}

		/**
		 * @brief Chained Inspection 설정
		 */
		inline void set_chained_inspection(const ChainedInspectionInfo& info)
		{
			chained_inspection = info;
		}

		/**
		 * @brief Blob Analysis 모델 반환
		 */
		inline const std::optional<BlobAnalysisModelInfo>& get_blob_analysis_model() const
		{
			return blob_analysis_model;
		}

		/**
		 * @brief Blob Analysis 모델 설정
		 */
		inline void set_blob_analysis_model(const BlobAnalysisModelInfo& model)
		{
			blob_analysis_model = model;
		}

		/**
		 * @brief Blob Analysis 모델 설정 (optional)
		 */
		inline void set_blob_analysis_model(const std::optional<BlobAnalysisModelInfo>& model)
		{
			blob_analysis_model = model;
		}

		/**
		 * @brief Blob Analysis 모델이 설정되어 있는지 확인
		 */
		inline bool has_blob_analysis_model() const
		{
			return blob_analysis_model.has_value();
		}

		// ========== 유효성 검증 ==========

		/**
		 * @brief 임계값 범위 검증 및 보정
		 */
		inline void validate_threshold()
		{
			if (threshold < 0.0) threshold = 0.0;
			else if (threshold > 1.0) threshold = 1.0;
		}

		/**
		 * @brief 클래스 정보 유효성 검증
		 */
		inline bool is_valid() const
		{
			return class_index >= 0 &&
				   !label.empty() &&
				   threshold >= 0.0 && threshold <= 1.0;
		}
	};

} // namespace syai::runtime::domain::config
