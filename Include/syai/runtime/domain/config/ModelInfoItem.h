#pragma once

#include "syai/runtime/SimpleSmartType.h"
#include "ConfigurationEnum.h"
#include "ClassInfo.h"
#include <vector>
#include <memory>

#if __has_include("nlohmann/json.hpp")
#include "nlohmann/json.hpp"
#ifndef INCLUDE_NLOHMANN_JSON_HPP
#define INCLUDE_NLOHMANN_JSON_HPP
#endif // !INCLUDE_NLOHMANN_JSON_HPP
#endif

namespace syai::runtime::domain::config
{
	/**
	 * @brief 모델 정보 항목 클래스
	 *
	 * 개별 모델의 이름, 활성화 여부, 클래스별 정보, 심각도 등을 관리합니다.
	 * C#의 ModelInfoConfig와 동일한 개념으로 클래스 정보를 구조화하여 관리합니다.
	 *
	 * 특징:
	 * - 모델별 설정 관리: 이름, 활성화, 심각도, OK 클래스 인덱스
	 * - 구조화된 클래스 관리: ClassInfo 벡터로 각 클래스의 모든 정보 통합 관리
	 * - 클래스별 임계값, 레이블, Chained Inspection, Blob Analysis 설정
	 * - 유효성 검증: 임계값 및 심각도 범위 체크
	 * - 자동 메모리 관리: SmartString 사용
	 * - OK 클래스 추적: OK(정상/합격) 클래스의 인덱스 관리
	 *
	 * @see ClassInfo, ManagerSetup, ConfigurationModelInfo
	 */
	class ModelInfoItem
	{
	public:
		/**
		 * @brief 기본 생성자
		 */
		ModelInfoItem() = default;

		/**
		 * @brief 매개변수 생성자 (단일 임계값 - 하위 호환성)
		 * @param name 모델 이름
		 * @param enabled 활성화 여부 (기본: true)
		 * @param threshold 모든 클래스에 적용할 단일 임계값 0.0~1.0 (기본: 0.5)
		 * @param severity 심각도 1~5 (기본: 3)
		 * @param ok_class_index OK 클래스 인덱스 (기본: -1, 미설정)
		 */
		ModelInfoItem(const char* name, bool enabled = true, double threshold = 0.5, int severity = 3, int ok_class_index = -1)
			: model_info_name_(name)
			, is_enabled_(enabled)
			, ok_class_index_(ok_class_index)
		{
			set_severity(severity);
			// 단일 임계값으로 클래스 정보 초기화 (하위 호환성)
			ClassInfo class_info(0, "", threshold);
			classes_.push_back(class_info);
		}

		/**
		 * @brief 매개변수 생성자 (클래스 정보 벡터)
		 * @param name 모델 이름
		 * @param enabled 활성화 여부
		 * @param classes 클래스 정보 벡터
		 * @param severity 심각도 1~5
		 * @param ok_class_index OK 클래스 인덱스 (기본: -1, 미설정)
		 */
		ModelInfoItem(const char* name, bool enabled, const std::vector<ClassInfo>& classes, int severity = 3, int ok_class_index = -1)
			: model_info_name_(name)
			, is_enabled_(enabled)
			, classes_(classes)
			, ok_class_index_(ok_class_index)
		{
			set_severity(severity);
		}

		/**
		 * @brief 복사 생성자
		 */
		ModelInfoItem(const ModelInfoItem& other)
			: model_info_name_(other.model_info_name_)
			, is_enabled_(other.is_enabled_)
			, classes_(other.classes_)
			, severity_(other.severity_)
			, ok_class_index_(other.ok_class_index_)
		{
		}

		/**
		 * @brief 이동 생성자
		 */
		ModelInfoItem(ModelInfoItem&& other) noexcept
			: model_info_name_(std::move(other.model_info_name_))
			, is_enabled_(other.is_enabled_)
			, classes_(std::move(other.classes_))
			, severity_(other.severity_)
			, ok_class_index_(other.ok_class_index_)
		{
		}

		/**
		 * @brief 복사 할당 연산자
		 */
		ModelInfoItem& operator=(const ModelInfoItem& other)
		{
			if (this != &other) {
				model_info_name_ = other.model_info_name_;
				is_enabled_ = other.is_enabled_;
				classes_ = other.classes_;
				severity_ = other.severity_;
				ok_class_index_ = other.ok_class_index_;
			}
			return *this;
		}

		/**
		 * @brief 이동 할당 연산자
		 */
		ModelInfoItem& operator=(ModelInfoItem&& other) noexcept
		{
			if (this != &other) {
				model_info_name_ = std::move(other.model_info_name_);
				is_enabled_ = other.is_enabled_;
				classes_ = std::move(other.classes_);
				severity_ = other.severity_;
				ok_class_index_ = other.ok_class_index_;
			}
			return *this;
		}

		// ========== Getter 메서드 ==========

		/**
		 * @brief 모델 이름 반환
		 * @return 모델 이름 문자열
		 */
		inline const char* get_model_info_name() const { return model_info_name_.c_str(); }

		/**
		 * @brief 활성화 여부 반환
		 * @return 활성화 시 true
		 */
		inline bool get_is_enabled() const { return is_enabled_; }

		/**
		 * @brief 기본 임계값 반환 (하위 호환성)
		 * @return 첫 번째 클래스 임계값 또는 0.5 (기본값)
		 * @deprecated 클래스별 임계값을 사용하려면 get_class_info() 사용
		 */
		inline double get_threshold() const
		{
			return classes_.empty() ? 0.5 : classes_[0].threshold;
		}

		/**
		 * @brief 특정 클래스의 임계값 반환
		 * @param class_index 클래스 인덱스 (0-based)
		 * @return 해당 클래스의 임계값 (범위 밖이면 0.5 반환)
		 */
		inline double get_class_threshold(int class_index) const
		{
			if (class_index < 0 || class_index >= static_cast<int>(classes_.size())) {
				return 0.5;
			}
			return classes_[class_index].threshold;
		}

		/**
		 * @brief 모든 클래스의 임계값 벡터 반환
		 * @return 클래스별 임계값 벡터
		 */
		std::vector<double> get_class_thresholds() const
		{
			std::vector<double> thresholds;
			thresholds.reserve(classes_.size());
			for (const auto& cls : classes_) {
				thresholds.push_back(cls.threshold);
			}
			return thresholds;
		}

		/**
		 * @brief 클래스 개수 반환
		 * @return 설정된 클래스 개수
		 */
		inline int get_class_count() const
		{
			return static_cast<int>(classes_.size());
		}

		/**
		 * @brief 특정 클래스의 정보 반환
		 * @param class_index 클래스 인덱스
		 * @return 해당 클래스의 정보 상수 참조 (범위 밖이면 nullptr)
		 */
		inline const ClassInfo* get_class_info(int class_index) const
		{
			if (class_index < 0 || class_index >= static_cast<int>(classes_.size())) {
				return nullptr;
			}
			return &classes_[class_index];
		}

		/**
		 * @brief 특정 클래스의 정보 반환 (수정 가능)
		 * @param class_index 클래스 인덱스
		 * @return 해당 클래스의 정보 참조 (범위 밖이면 nullptr)
		 */
		inline ClassInfo* get_class_info_mutable(int class_index)
		{
			if (class_index < 0 || class_index >= static_cast<int>(classes_.size())) {
				return nullptr;
			}
			return &classes_[class_index];
		}

		/**
		 * @brief 모든 클래스 정보 반환
		 * @return 클래스 정보 벡터 상수 참조
		 */
		inline const std::vector<ClassInfo>& get_classes() const
		{
			return classes_;
		}

		/**
		 * @brief 모든 클래스 정보 반환 (수정 가능)
		 * @return 클래스 정보 벡터 참조
		 */
		inline std::vector<ClassInfo>& get_classes_mutable()
		{
			return classes_;
		}

		/**
		 * @brief 심각도 반환
		 * @return 심각도 (1-5)
		 */
		inline int get_severity() const { return severity_; }

		/**
		 * @brief OK 클래스 인덱스 반환
		 * @return OK 클래스 인덱스 (-1: 미설정, 0 이상: 유효한 클래스 인덱스)
		 */
		inline int get_ok_class_index() const { return ok_class_index_; }

		/**
		 * @brief OK 클래스 설정 여부 확인
		 * @return OK 클래스가 설정되어 있으면 true
		 */
		inline bool has_ok_class() const { return ok_class_index_ >= 0; }

		// ========== Setter 메서드 ==========

		/**
		 * @brief 모델 이름 설정
		 * @param name 모델 이름
		 */
		inline void set_model_info_name(const char* name) { model_info_name_ = name; }

		/**
		 * @brief 활성화 여부 설정
		 * @param enabled 활성화 시 true
		 */
		inline void set_is_enabled(bool enabled) { is_enabled_ = enabled; }

		/**
		 * @brief 단일 임계값 설정 (첫 번째 클래스에 적용 - 하위 호환성)
		 * @param threshold 임계값 (0.0 ~ 1.0, 범위 밖 값은 자동 조정)
		 */
		inline void set_threshold(double threshold)
		{
			if (threshold < 0.0) threshold = 0.0;
			else if (threshold > 1.0) threshold = 1.0;

			if (classes_.empty()) {
				ClassInfo class_info(0, "", threshold);
				classes_.push_back(class_info);
			}
			else {
				classes_[0].set_threshold(threshold);
			}
		}

		/**
		 * @brief 특정 클래스의 임계값 설정
		 * @param class_index 클래스 인덱스
		 * @param threshold 임계값 (0.0 ~ 1.0)
		 * @return 성공 시 true, 실패 시 false
		 */
		inline bool set_class_threshold(int class_index, double threshold)
		{
			if (class_index < 0) return false;

			// 벡터 크기 확장
			if (class_index >= static_cast<int>(classes_.size())) {
				for (int i = classes_.size(); i <= class_index; ++i) {
					classes_.push_back(ClassInfo(i, "", 0.5));
				}
			}

			classes_[class_index].set_threshold(threshold);
			return true;
		}

		/**
		 * @brief 모든 클래스의 임계값 설정
		 * @param thresholds 클래스별 임계값 벡터
		 */
		inline void set_class_thresholds(const std::vector<double>& thresholds)
		{
			classes_.clear();
			classes_.reserve(thresholds.size());

			for (size_t i = 0; i < thresholds.size(); ++i) {
				ClassInfo class_info(static_cast<int>(i), "", thresholds[i]);
				classes_.push_back(class_info);
			}
		}

		/**
		 * @brief 모든 클래스 정보 설정
		 * @param classes 클래스 정보 벡터
		 */
		inline void set_classes(const std::vector<ClassInfo>& classes)
		{
			classes_ = classes;
		}

		/**
		 * @brief 심각도 설정 (유효성 검증 포함)
		 * @param severity 심각도 (1 ~ 5, 범위 밖 값은 자동 조정)
		 */
		inline void set_severity(int severity)
		{
			if (severity < 1) severity_ = 1;
			else if (severity > 5) severity_ = 5;
			else severity_ = severity;
		}

		/**
		 * @brief OK 클래스 인덱스 설정
		 * @param ok_class_index OK 클래스 인덱스 (-1: 미설정)
		 */
		inline void set_ok_class_index(int ok_class_index)
		{
			ok_class_index_ = ok_class_index;
		}

		/**
		 * @brief OK 클래스 인덱스 유효성 검증 및 설정
		 * @param ok_class_index 설정할 OK 클래스 인덱스
		 * @return 유효한 범위이면 true (설정됨), 범위 밖이면 false (-1로 초기화)
		 */
		inline bool set_ok_class_index_validated(int ok_class_index)
		{
			// -1은 항상 유효 (미설정)
			if (ok_class_index == -1) {
				ok_class_index_ = -1;
				return true;
			}

			// 0 이상 class_count 미만이어야 함
			if (ok_class_index >= 0 && ok_class_index < static_cast<int>(classes_.size())) {
				ok_class_index_ = ok_class_index;
				return true;
			}

			// 범위 밖: 초기화
			ok_class_index_ = -1;
			return false;
		}

		// ========== 유효성 검증 ==========

		/**
		 * @brief 모델 정보 유효성 검증
		 * @return 유효하면 true
		 */
		inline bool is_valid() const
		{
			if (model_info_name_.empty() || severity_ < 1 || severity_ > 5 || classes_.empty()) {
				return false;
			}

			// 모든 클래스 검증
			for (const auto& cls : classes_) {
				if (!cls.is_valid()) {
					return false;
				}
			}

			// OK 클래스 인덱스 검증
			if (ok_class_index_ < -1 || ok_class_index_ >= static_cast<int>(classes_.size())) {
				return false;
			}

			return true;
		}

		// ========== 비교 연산자 ==========

		/**
		 * @brief 동등성 비교
		 * @param other 비교할 ModelInfoItem
		 * @return 모든 필드가 같으면 true
		 */
		inline bool operator==(const ModelInfoItem& other) const
		{
			if (model_info_name_ != other.model_info_name_ ||
				is_enabled_ != other.is_enabled_ ||
				severity_ != other.severity_ ||
				ok_class_index_ != other.ok_class_index_ ||
				classes_.size() != other.classes_.size()) {
				return false;
			}

			// 모든 클래스 비교
			for (size_t i = 0; i < classes_.size(); ++i) {
				if (classes_[i] != other.classes_[i]) {
					return false;
				}
			}

			return true;
		}

		/**
		 * @brief 부등성 비교
		 * @param other 비교할 ModelInfoItem
		 * @return 하나라도 다르면 true
		 */
		inline bool operator!=(const ModelInfoItem& other) const
		{
			return !(*this == other);
		}

	private:
#ifdef INCLUDE_NLOHMANN_JSON_HPP
		friend void to_json(nlohmann::json& j, const ModelInfoItem& item);
		friend void from_json(const nlohmann::json& j, ModelInfoItem& item);
#endif

		SmartString model_info_name_;          //!< 모델 이름
		bool is_enabled_ = true;               //!< 활성화 여부
		std::vector<ClassInfo> classes_;       //!< 클래스 정보 벡터 (구조화된 클래스 관리)
		int severity_ = 3;                     //!< 심각도 (1-5)
		int ok_class_index_ = -1;              //!< OK 클래스 인덱스 (-1: 미설정)
	};

#ifdef INCLUDE_NLOHMANN_JSON_HPP
	/**
	 * @brief ModelInfoItem을 nlohmann::json으로 변환
	 */
	inline void to_json(nlohmann::json& j, const ModelInfoItem& item) {
		nlohmann::json classes_json = nlohmann::json::array();
		for (const auto& cls : item.get_classes()) {
			nlohmann::json cls_json;
			cls_json["classIndex"] = cls.class_index;
			cls_json["label"] = cls.label.c_str();
			cls_json["threshold"] = cls.threshold;
			classes_json.push_back(cls_json);
		}

		j["modelInfoName"] = item.get_model_info_name();
		j["isEnabled"] = item.get_is_enabled();
		j["classes"] = classes_json;
		j["classThresholds"] = item.get_class_thresholds();
		j["threshold"] = item.get_threshold();
		j["severity"] = item.get_severity();
		j["okClassIndex"] = item.get_ok_class_index();
	}

	/**
	 * @brief nlohmann::json에서 ModelInfoItem으로 변환
	 */
	inline void from_json(const nlohmann::json& j, ModelInfoItem& item) {
		item.set_model_info_name(j.at("modelInfoName").get<std::string>().c_str());
		item.set_is_enabled(j.value("isEnabled", true));

		// 새로운 구조화된 클래스 정보가 있으면 사용
		if (j.contains("classes") && j.at("classes").is_array()) {
			std::vector<ClassInfo> classes;
			for (const auto& cls_json : j.at("classes")) {
				ClassInfo cls(
					cls_json.value("classIndex", 0),
					cls_json.value("label", "").c_str(),
					cls_json.value("threshold", 0.5)
				);
				classes.push_back(cls);
			}
			item.set_classes(classes);
		}
		// 하위 호환성: classThresholds가 있으면 사용
		else if (j.contains("classThresholds")) {
			item.set_class_thresholds(j.at("classThresholds").get<std::vector<double>>());
		}
		// 하위 호환성: 단일 threshold 사용
		else if (j.contains("threshold")) {
			item.set_threshold(j.value("threshold", 0.5));
		}
		else {
			item.set_threshold(0.5); // 기본값
		}

		item.set_severity(j.value("severity", 3));

		// OK 클래스 인덱스 로드 (기본값: -1)
		int ok_class_index = j.value("okClassIndex", -1);
		item.set_ok_class_index(ok_class_index);
	}
#endif

} // namespace syai::runtime::domain::config
