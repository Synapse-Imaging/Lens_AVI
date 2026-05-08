#pragma once
#include "InspectionStepResult.h"
#include "syai/runtime/syai_runtime_inspection_export.h"
#include "syai/runtime/SimpleSmartType.h"
#include "opencv2/core.hpp"

namespace syai::runtime::domain::inspection
{
	/**
	 * @brief 검사 요청 클래스 (참조 카운팅 기반 자동 메모리 관리)
	 *
	 * 검사 요청에 필요한 모든 정보를 포함하며,
	 * 참조 카운팅을 통해 메모리를 자동으로 관리함.
	 *
	 * 예제:
	 * @code
	 *     Request req("inspection_001", "lot_123", "model_v1");
	 *     SmartVector<bool> step_enable;
	 *     step_enable.push_back(true);
	 *     step_enable.push_back(false);
	 *     req.set_inspection_step_enable(step_enable);
	 * @endcode
	 */
	class SYAIRUNTIMEINSPECTION_API Request
	{
	private:
		struct RequestData;
		RequestData* data_; //!< 공유 요청 데이터 포인터

		void release();
		void add_ref();
		void ensure_unique();
		void ensure_data();

	public:
		Request();
		Request(const char* inspection_id,
				const char* log_id = nullptr,
				const char* model_key = nullptr);
		Request(const Request& other);
		Request& operator=(const Request& other);
		~Request();

		// ========== Getter 메서드 ==========
		const char* get_inspection_id() const;
		const char* get_log_id() const;
		const char* get_model_key() const;
		SmartVector<bool> get_inspection_step_enable() const;
		bool get_inspection_step_enable_at(int index) const;
		int get_inspection_step_count() const;
		cv::Rect get_area() const;
		cv::Mat get_mask() const;
		cv::Mat get_roi_mask() const;
		cv::Mat get_image() const;
		const char* get_save_dir() const;

		// ========== Setter 메서드 ==========
		void set_inspection_id(const char* inspection_id);
		void set_log_id(const char* log_id);
		void set_model_key(const char* model_key);
		void set_inspection_step_enable(const SmartVector<bool>& inspection_step_enable);
		void set_area(const cv::Rect& area);
		void set_mask(const cv::Mat& mask);
		void set_roi_mask(const cv::Mat& roi_mask);
		void set_image(const cv::Mat& image);
		void set_save_dir(const char* save_dir);

		// ========== 검사 단계 관리 메서드 ==========
		void add_inspection_step_enable(bool enabled);
		void set_inspection_step_enable_at(int index, bool enabled);
		void remove_inspection_step_enable_at(int index);
		void clear_inspection_step_enable();

		// ========== 유틸리티 메서드 ==========
		bool is_valid() const;
		int ref_count() const;
		bool empty() const;
	};
}
