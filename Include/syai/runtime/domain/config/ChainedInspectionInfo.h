#pragma once

#include "syai/runtime/SimpleSmartType.h"
#include <string>

namespace syai::runtime::domain::config
{
	/**
	 * @brief Chained Inspection 정보 (공개 API용)
	 *
	 * 다른 Step의 결과를 기반으로 검사를 진행할 때 사용하는 설정
	 * 내부 ConfigFileChainedInspection 대신 공개 API에서 사용
	 */
	class ChainedInspectionInfo
	{
	public:
		/**
		 * @brief 기본 생성자
		 */
		ChainedInspectionInfo() = default;

		/**
		 * @brief 매개변수 생성자
		 * @param step_name Chained될 Step의 이름
		 * @param model_info_name Chained될 ModelInfo의 이름
		 * @param is_enabled 사용 여부
		 */
		ChainedInspectionInfo(const char* step_name, const char* model_info_name, bool is_enabled = false)
			: step_name_(step_name)
			, model_info_name_(model_info_name)
			, is_enabled_(is_enabled)
		{
		}

		/**
		 * @brief 복사 생성자
		 */
		ChainedInspectionInfo(const ChainedInspectionInfo& other) = default;

		/**
		 * @brief 대입 연산자
		 */
		ChainedInspectionInfo& operator=(const ChainedInspectionInfo& other) = default;

		// Getters
		const char* get_step_name() const { return step_name_.c_str(); }
		const char* get_model_info_name() const { return model_info_name_.c_str(); }
		bool get_is_enabled() const { return is_enabled_; }

		// Setters
		void set_step_name(const char* step_name) { step_name_ = step_name; }
		void set_model_info_name(const char* model_info_name) { model_info_name_ = model_info_name; }
		void set_is_enabled(bool is_enabled) { is_enabled_ = is_enabled; }

		/**
		 * @brief Chained Inspection이 설정되어 있는지 여부
		 * @return Step 이름과 ModelInfo 이름 모두 비어있지 않으면 true
		 */
		inline bool is_set() const
		{
			return !step_name_.empty() && !model_info_name_.empty();
		}

		/**
		 * @brief 설정 초기화
		 */
		inline void clear()
		{
			step_name_ = "";
			model_info_name_ = "";
			is_enabled_ = false;
		}

	private:
		SmartString step_name_;			//!< Chained될 Step의 이름
		SmartString model_info_name_;	//!< Chained될 ModelInfo의 이름
		bool is_enabled_ = false;		//!< 사용 여부
	};

} // namespace syai::runtime::domain::config
