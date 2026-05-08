#pragma once

#include "syai/runtime/SimpleSmartType.h"
#include <vector>

namespace syai::runtime::domain::config
{
	/**
	 * @brief Blob Analysis 모델 정보 (공개 API용)
	 *
	 * Segmentation 전용 - Trainer에서 생성된 Blob Analysis 메타데이터
	 * 내부 ConfigFileBlobAnalysisModel 대신 공개 API에서 사용
	 */
	class BlobAnalysisModelInfo
	{
	public:
		/**
		 * @brief 기본 생성자
		 */
		BlobAnalysisModelInfo() = default;

		/**
		 * @brief 매개변수 생성자
		 * @param class_name 클래스 이름
		 * @param min_area_toggle 최소 영역 토글
		 * @param min_area_text 최소 영역 텍스트
		 * @param xy_axis_toggle XY 축 토글
		 * @param blob_x_axis_text Blob X 축 텍스트
		 * @param blob_y_axis_text Blob Y 축 텍스트
		 * @param m_maxis_toggle M 축 토글
		 * @param minor_axis_text 소축 텍스트
		 * @param major_axis_text 대축 텍스트
		 */
		BlobAnalysisModelInfo(
			const char* class_name = "",
			bool min_area_toggle = false,
			const char* min_area_text = "",
			bool xy_axis_toggle = false,
			const char* blob_x_axis_text = "",
			const char* blob_y_axis_text = "",
			bool m_maxis_toggle = false,
			const char* minor_axis_text = "",
			const char* major_axis_text = "")
			: class_name_(class_name)
			, min_area_toggle_(min_area_toggle)
			, min_area_text_(min_area_text)
			, xy_axis_toggle_(xy_axis_toggle)
			, blob_x_axis_text_(blob_x_axis_text)
			, blob_y_axis_text_(blob_y_axis_text)
			, m_maxis_toggle_(m_maxis_toggle)
			, minor_axis_text_(minor_axis_text)
			, major_axis_text_(major_axis_text)
		{
		}

		/**
		 * @brief 복사 생성자
		 */
		BlobAnalysisModelInfo(const BlobAnalysisModelInfo& other) = default;

		/**
		 * @brief 대입 연산자
		 */
		BlobAnalysisModelInfo& operator=(const BlobAnalysisModelInfo& other) = default;

		// Getters
		const char* get_class_name() const { return class_name_.c_str(); }
		bool get_min_area_toggle() const { return min_area_toggle_; }
		const char* get_min_area_text() const { return min_area_text_.c_str(); }
		bool get_xy_axis_toggle() const { return xy_axis_toggle_; }
		const char* get_blob_x_axis_text() const { return blob_x_axis_text_.c_str(); }
		const char* get_blob_y_axis_text() const { return blob_y_axis_text_.c_str(); }
		bool get_m_maxis_toggle() const { return m_maxis_toggle_; }
		const char* get_minor_axis_text() const { return minor_axis_text_.c_str(); }
		const char* get_major_axis_text() const { return major_axis_text_.c_str(); }

		// Setters
		void set_class_name(const char* class_name) { class_name_ = class_name; }
		void set_min_area_toggle(bool toggle) { min_area_toggle_ = toggle; }
		void set_min_area_text(const char* text) { min_area_text_ = text; }
		void set_xy_axis_toggle(bool toggle) { xy_axis_toggle_ = toggle; }
		void set_blob_x_axis_text(const char* text) { blob_x_axis_text_ = text; }
		void set_blob_y_axis_text(const char* text) { blob_y_axis_text_ = text; }
		void set_m_maxis_toggle(bool toggle) { m_maxis_toggle_ = toggle; }
		void set_minor_axis_text(const char* text) { minor_axis_text_ = text; }
		void set_major_axis_text(const char* text) { major_axis_text_ = text; }

		/**
		 * @brief 유효성 검증
		 * @return 클래스 이름이 비어있지 않으면 true
		 */
		inline bool is_valid() const
		{
			return !class_name_.empty();
		}

	private:
		SmartString class_name_;			//!< 클래스 이름
		bool min_area_toggle_ = false;		//!< 최소 영역 토글
		SmartString min_area_text_;			//!< 최소 영역 텍스트
		bool xy_axis_toggle_ = false;		//!< XY 축 토글
		SmartString blob_x_axis_text_;		//!< Blob X 축 텍스트
		SmartString blob_y_axis_text_;		//!< Blob Y 축 텍스트
		bool m_maxis_toggle_ = false;		//!< M 축 토글
		SmartString minor_axis_text_;		//!< 소축 텍스트
		SmartString major_axis_text_;		//!< 대축 텍스트
	};

} // namespace syai::runtime::domain::config
