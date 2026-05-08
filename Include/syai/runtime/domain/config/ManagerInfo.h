#pragma once

#include "syai/runtime/SimpleSmartType.h"
#include "ConfigurationEnum.h"
#include "ManagerSetup.h"
#include "ChainedInspectionInfo.h"
#include "BlobAnalysisModelInfo.h"

#if __has_include("nlohmann/json.hpp")
#include "nlohmann/json.hpp"
#include <string>
#include <fstream>
#define INCLUDE_NLOHMANN_JSON_HPP
#endif // __has_include("nlohmann/json.hpp")

namespace syai::runtime::domain::config
{
	/**
	 * @brief Manager 정보를 담는 클래스
	 *
	 * ManagerSetup과 추가적인 관리 정보를 포함하는 클래스.
	 * 매니저의 이름, 설정 정보, 그리고 관리하는 모델 목록을 함께 관리하여
	 * InspectionService에서 매니저들을 효율적으로 관리할 수 있도록 함.
	 *
	 * 특징:
	 * - ManagerSetup 포함: 매니저 초기화에 필요한 모든 설정 정보
	 * - 매니저 이름 관리: 고유한 식별자로 사용
	 * - 관리 모델 목록: 해당 매니저가 처리하는 모델들의 리스트
	 * - 자동 메모리 관리: SmartString과 SmartList 사용으로 메모리 자동 해제
	 * - 복사 안전성: 참조 카운팅을 통한 효율적인 복사
	 *
	 * 사용 예제:
	 * @code
	 *     ManagerSetup setup(InspectionType::CLASSIFICATION, "C:\\models\\cls");
	 *     ManagerInfo info("classifier", setup);
	 *
	 *     // 모델 추가
	 *     info.addManagedModel("scratch_detection");
	 *     info.addManagedModel("dent_detection");
	 *
	 *     // 이름 접근
	 *     const char* name = info.getManagerName();
	 *
	 *     // 설정 접근
	 *     const ManagerSetup& config = info.getSetup();
	 *
	 *     // 모델 목록 접근
	 *     const SmartList& models = info.getManagedModels();
	 *
	 *     // 복사 (참조 카운팅)
	 *     ManagerInfo copy = info;  // 효율적인 복사
	 * @endcode
	 *
	 * @see ManagerSetup, SmartString, SmartList
	 */
	class ManagerInfo
	{
	public:
		/**
		 * @brief 기본 생성자
		 *
		 * 빈 매니저 정보를 생성함. 이름, 설정, 모델 목록 모두 기본값으로 초기화됨.
		 */
		ManagerInfo() = default;

		/**
		 * @brief 매니저 이름만으로 생성
		 * @param name 매니저 이름 (nullptr 허용)
		 */
		explicit ManagerInfo(const char* name)
			: manager_name_(name)
		{
		}

		/**
		 * @brief 매니저 이름과 설정으로 생성
		 * @param name 매니저 이름 (nullptr 허용)
		 * @param setup 매니저 설정 정보
		 */
		ManagerInfo(const char* name, const ManagerSetup& setup)
			: manager_name_(name), setup_(setup)
		{
		}

		/**
		 * @brief 매니저 이름, 설정, 모델 목록으로 생성
		 * @param name 매니저 이름 (nullptr 허용)
		 * @param setup 매니저 설정 정보
		 * @param models 관리할 모델 목록
		 */
		ManagerInfo(const char* name, const ManagerSetup& setup, const SmartList& models)
			: manager_name_(name), setup_(setup), managed_models_(models)
		{
		}

		/**
		 * @brief 복사 생성자 (자동 참조 카운팅)
		 * @param other 복사할 ManagerInfo 인스턴스
		 */
		ManagerInfo(const ManagerInfo& other) = default;

		/**
		 * @brief 복사 할당 연산자 (자동 참조 카운팅)
		 * @param other 할당할 ManagerInfo 인스턴스
		 * @return 자기 자신 참조
		 */
		ManagerInfo& operator=(const ManagerInfo& other) = default;

		// ========== 매니저 이름 관리 메서드 ==========

		/**
		 * @brief 매니저 이름 설정
		 * @param name 새로운 매니저 이름 (nullptr 허용)
		 */
		inline void set_manager_name(const char* name)
		{
			manager_name_ = name;
		}

		/**
		 * @brief 매니저 이름을 C 스타일 문자열로 반환
		 * @return 매니저 이름 문자열 (빈 문자열이면 "")
		 */
		inline const char* get_manager_name() const
		{
			return manager_name_.c_str();
		}

		/**
		 * @brief 매니저 이름이 비어있는지 확인
		 * @return 비어있으면 true, 그렇지 않으면 false
		 */
		inline bool is_manager_name_empty() const
		{
			return manager_name_.empty();
		}

		// ========== ManagerSetup 관리 메서드 ==========

		/**
		 * @brief ManagerSetup 설정
		 * @param setup 새로운 매니저 설정
		 */
		inline void set_setup(const ManagerSetup& setup)
		{
			setup_ = setup;
		}

		/**
		 * @brief ManagerSetup 참조 반환 (읽기 전용)
		 * @return ManagerSetup 상수 참조
		 */
		inline const ManagerSetup& get_setup() const
		{
			return setup_;
		}

		/**
		 * @brief ManagerSetup 참조 반환 (수정 가능)
		 * @return ManagerSetup 참조
		 */
		inline ManagerSetup& get_setup()
		{
			return setup_;
		}

		// ========== 관리 모델 목록 메서드 ==========

		/**
		 * @brief 관리 모델 목록 설정
		 * @param models 새로운 모델 목록
		 */
		inline void set_managed_models(const SmartList& models)
		{
			managed_models_ = models;
		}

		/**
		 * @brief 관리 모델 목록 참조 반환 (읽기 전용)
		 * @return SmartList 상수 참조
		 */
		inline const SmartList& get_managed_models() const
		{
			return managed_models_;
		}

		/**
		 * @brief 관리 모델 목록 참조 반환 (수정 가능)
		 * @return SmartList 참조
		 */
		inline SmartList& get_managed_models()
		{
			return managed_models_;
		}

		/**
		 * @brief 새 모델을 관리 목록에 추가
		 * @param model 추가할 모델 이름 (nullptr 허용)
		 */
		inline void add_managed_model(const char* model)
		{
			managed_models_.add(model);
		}

		/**
		 * @brief 모델을 관리 목록에서 제거
		 * @param model 제거할 모델 이름
		 * @return 제거되었으면 true, 찾을 수 없으면 false
		 */
		inline bool remove_managed_model(const char* model)
		{
			return managed_models_.remove(model);
		}

		/**
		 * @brief 특정 모델이 관리 목록에 포함되어 있는지 확인
		 * @param model 확인할 모델 이름
		 * @return 포함되어 있으면 true, 없으면 false
		 */
		inline bool has_managed_model(const char* model) const
		{
			return managed_models_.contains(model);
		}

		/**
		 * @brief 관리 모델 개수 반환
		 * @return 관리하는 모델의 개수
		 */
		inline int get_managed_model_count() const
		{
			return managed_models_.size();
		}

		/**
		 * @brief 특정 인덱스의 관리 모델 반환
		 * @param index 모델 인덱스
		 * @return 모델 이름 (범위 밖이면 nullptr)
		 */
		inline const char* get_managed_model(int index) const
		{
			return managed_models_.get(index);
		}

		/**
		 * @brief 모든 관리 모델 제거
		 */
		inline void clear_managed_models()
		{
			managed_models_.clear();
		}

		/**
		 * @brief 특정 모델의 클래스별 임계값 반환
		 * @param model_index 모델 인덱스
		 * @param class_index 클래스 인덱스
		 * @return 해당 클래스의 임계값 (범위 밖이면 0.5 반환)
		 */
		inline double get_class_threshold(int model_index, int class_index) const
		{
			const ModelInfoItem* item = get_setup().find_model_info(
				get_setup().get_model_info(model_index)->get_model_info_name()
			);
			if (!item) return 0.5;
			return item->get_class_threshold(class_index);
		}

		/**
		 * @brief 특정 모델의 모든 클래스 임계값 배열 반환
		 * @param model_index 모델 인덱스
		 * @return 클래스별 임계값 벡터 상수 참조
		 */
		inline const std::vector<double>& get_class_thresholds(int model_index) const
		{
			static const std::vector<double> empty_vec;
			const ModelInfoItem* item = get_setup().get_model_info(model_index);
			if (!item) return empty_vec;
			return item->get_class_thresholds();
		}

		/**
		 * @brief 특정 모델의 클래스 개수 반환
		 * @param model_index 모델 인덱스
		 * @return 클래스 개수
		 */
		inline int get_class_count(int model_index) const
		{
			const ModelInfoItem* item = get_setup().get_model_info(model_index);
			if (!item) return 0;
			return item->get_class_count();
		}

		/**
		 * @brief 특정 모델의 OK 클래스 인덱스 반환
		 * @param model_index 모델 인덱스
		 * @return OK 클래스 인덱스 (-1: 미설정)
		 */
		inline int get_ok_class_index(int model_index) const
		{
			const ModelInfoItem* item = get_setup().get_model_info(model_index);
			if (!item) return -1;
			return item->get_ok_class_index();
		}

		/**
		 * @brief 특정 모델의 심각도 반환
		 * @param model_index 모델 인덱스
		 * @return 심각도 (1-5)
		 */
		inline int get_severity(int model_index) const
		{
			const ModelInfoItem* item = get_setup().get_model_info(model_index);
			if (!item) return 3; // 기본값
			return item->get_severity();
		}

		// ========== 편의 메서드 (ManagerSetup 속성 직접 접근) ==========

		/**
		 * @brief 검사 타입 반환
		 * @return 검사 타입
		 */
		inline InspectionType get_inspection_type() const
		{
			return setup_.get_type();
		}

		/**
		 * @brief 모델 경로 반환
		 * @return 모델 경로 문자열
		 */
		inline const char* get_model_path() const
		{
			return setup_.get_model_path();
		}

		/**
		 * @brief 디바이스 타입 반환
		 * @return 디바이스 타입
		 */
		inline DeviceType get_device_type() const
		{
			return setup_.get_device_type();
		}

		/**
		 * @brief 디바이스 타입을 문자열로 반환
		 * @return 디바이스 타입 문자열
		 */
		inline const char* get_device_type_string() const
		{
			return setup_.get_device_type_string();
		}

		/**
		 * @brief 디바이스 ID 반환
		 * @return 디바이스 ID
		 */
		inline int get_device_id() const
		{
			return setup_.get_device_id();
		}

		/**
		 * @brief 최대 배치 크기 반환
		 * @return 최대 배치 크기
		 */
		inline int get_max_batch_size() const
		{
			return setup_.get_max_batch_size();
		}

		// ========== 비교 연산자 ==========

		/**
		 * @brief 동등성 비교 연산자
		 * @param other 비교할 ManagerInfo 인스턴스
		 * @return 매니저 이름, 설정, 모델 목록이 모두 같으면 true, 다르면 false
		 */
		inline bool operator==(const ManagerInfo& other) const
		{
			return manager_name_ == other.manager_name_ &&
				setup_ == other.setup_ &&
				managed_models_ == other.managed_models_;
		}

		/**
		 * @brief 부등성 비교 연산자
		 * @param other 비교할 ManagerInfo 인스턴스
		 * @return 매니저 이름이나 설정, 모델 목록이 다르면 true, 같으면 false
		 */
		inline bool operator!=(const ManagerInfo& other) const
		{
			return !(*this == other);
		}

		// ========== 유효성 검증 메서드 ==========

		/**
		 * @brief ManagerInfo가 유효한지 확인
		 * @return 매니저 이름이 비어있지 않고 검사 타입이 NONE이 아니면 true
		 */
		inline bool is_valid() const
		{
			return !is_manager_name_empty() && setup_.get_type() != InspectionType::NONE;
		}

		/**
		 * @brief 매니저 이름의 유효성 검증
		 * @return 유효한 이름이면 true, 그렇지 않으면 false
		 */
		inline bool is_valid_manager_name() const
		{
			const char* name = get_manager_name();
			if (!name || name[0] == '\0') return false;

			// 길이 검증 (최대 100자)
			int length = 0;
			while (name[length] != '\0' && length < 100) {
				++length;
			}

			if (length == 0 || length >= 100) return false;

			// 유효한 문자 검증 (영숫자, 밑줄, 하이픈)
			for (int i = 0; i < length; ++i) {
				char c = name[i];
				if (!((c >= 'a' && c <= 'z') ||
					  (c >= 'A' && c <= 'Z') ||
					  (c >= '0' && c <= '9') ||
					  c == '_' || c == '-')) {
					return false;
				}
			}

			return true;
		}

		// ========== 디버그/로그 지원 메서드 ==========

		/**
		 * @brief 매니저 정보를 간단한 문자열로 요약
		 * @return 매니저 정보 요약 (주의: 반환된 문자열은 임시적이므로 즉시 사용할 것)
		 */
		inline const char* to_string() const
		{
			// 주의: 이 메서드는 정적 버퍼를 사용하므로 멀티스레드에서 안전하지 않음
			// 실제 구현에서는 더 안전한 방법을 고려해야 함
			static char buffer[512];
			const char* name = get_manager_name();
			const char* type_str = "";

			switch (setup_.get_type())
			{
			case InspectionType::CLASSIFICATION: type_str = "CLASSIFICATION"; break;
			case InspectionType::SEGMENTATION: type_str = "SEGMENTATION"; break;
			case InspectionType::DD: type_str = "DD"; break;
			case InspectionType::AD: type_str = "AD"; break;
			default: type_str = "NONE"; break;
			}

			// 간단한 문자열 포맷팅 (sprintf 대체)
			int pos = 0;
			const char* prefix = "ManagerInfo(name=";
			for (int i = 0; prefix[i] != '\0' && pos < 400; ++i, ++pos)
			{
				buffer[pos] = prefix[i];
			}

			for (int i = 0; name[i] != '\0' && pos < 420; ++i, ++pos)
			{
				buffer[pos] = name[i];
			}

			const char* middle = ", type=";
			for (int i = 0; middle[i] != '\0' && pos < 430; ++i, ++pos)
			{
				buffer[pos] = middle[i];
			}

			for (int i = 0; type_str[i] != '\0' && pos < 450; ++i, ++pos)
			{
				buffer[pos] = type_str[i];
			}

			// 모델 목록 추가
			const char* models_prefix = ", models=[";
			for (int i = 0; models_prefix[i] != '\0' && pos < 465; ++i, ++pos)
			{
				buffer[pos] = models_prefix[i];
			}

			for (int i = 0; i < managed_models_.size() && pos < 500; ++i)
			{
				if (i > 0 && pos < 509)
				{
					buffer[pos++] = ',';
				}

				const char* model = managed_models_.get(i);
				if (model)
				{
					for (int j = 0; model[j] != '\0' && pos < 509; ++j, ++pos)
					{
						buffer[pos] = model[j];
					}
				}
			}

			if (pos < 510) buffer[pos++] = ']';
			if (pos < 511) buffer[pos++] = ')';
			buffer[pos] = '\0';

			return buffer;
		}

		/**
		 * @brief 관리 모델 목록을 콤마로 구분된 문자열로 반환
		 * @return 모델 목록 문자열 (주의: 반환된 문자열은 임시적이므로 즉시 사용할 것)
		 */
		inline const char* get_managed_models_as_string() const
		{
			static char buffer[256];
			int pos = 0;

			for (int i = 0; i < managed_models_.size() && pos < 250; ++i)
			{
				if (i > 0 && pos < 254)
				{
					buffer[pos++] = ',';
					buffer[pos++] = ' ';
				}

				const char* model = managed_models_.get(i);
				if (model)
				{
					for (int j = 0, len = strlen(model); j < len && pos < 254; ++j, ++pos)
					{
						buffer[pos] = model[j];
					}
				}
			}

			buffer[pos] = '\0';
			return buffer;
		}

#ifdef INCLUDE_NLOHMANN_JSON_HPP
		/**
		 * @brief ManagerInfo 객체를 JSON 문자열로 변환
		 * @param pretty JSON을 예쁘게 포맷할지 여부
		 * @return JSON 문자열
		 */
		inline std::string to_json_string(bool pretty = false) const
		{
			nlohmann::json j = *this;
			return j.dump(pretty ? 4 : -1);
		}

		/**
		 * @brief JSON 문자열에서 ManagerInfo 객체 생성
		 * @param json_string JSON 데이터
		 * @return ManagerInfo 객체
		 */
		static inline ManagerInfo from_json_string(const std::string& json_string)
		{
			ManagerInfo info;
			nlohmann::json::parse(json_string).get_to(info);
			return info;
		}

		/**
		 * @brief ManagerInfo 객체를 JSON 파일로 저장
		 * @param file_path 저장할 파일 경로
		 * @param pretty JSON을 예쁘게 포맷할지 여부
		 * @return 성공 시 true, 실패 시 false
		 */
		inline bool save_to_file(const std::string& file_path, bool pretty = true) const
		{
			try {
				std::ofstream file(file_path);
				if (!file.is_open()) {
					return false;
				}
				file << to_json_string(pretty);
				file.close();
				return true;
			}
			catch (...) {
				return false;
			}
		}

		/**
		 * @brief JSON 파일에서 ManagerInfo 객체 로드
		 * @param file_path 로드할 파일 경로
		 * @return ManagerInfo 객체
		 * @throws std::runtime_error 파일 읽기 또는 파싱 실패 시
		 */
		static inline ManagerInfo load_from_file(const std::string& file_path)
		{
			std::ifstream file(file_path);
			if (!file.is_open()) {
				throw std::runtime_error("Failed to open file: " + file_path);
			}
			std::string json_string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();
			return from_json_string(json_string);
		}
#endif

	private:
#ifdef INCLUDE_NLOHMANN_JSON_HPP
		friend void to_json(nlohmann::json& j, const ManagerInfo& p);
		friend void from_json(const nlohmann::json& j, ManagerInfo& p);
#endif
		SmartString manager_name_;  //!< 매니저 이름 (자동 메모리 관리)
		ManagerSetup setup_;        //!< 매니저 설정 정보
		SmartList managed_models_;  //!< 관리하는 모델 목록 (자동 메모리 관리)

	};

#ifdef INCLUDE_NLOHMANN_JSON_HPP
	/**
	 * @brief ManagerInfo를 nlohmann::json으로 변환
	 */
	inline void to_json(nlohmann::json& j, const ManagerInfo& p) {
		SmartList models = p.get_managed_models();
		std::vector<std::string> model_list;
		for (int i = 0; i < models.size(); ++i)
		{
			model_list.push_back(models.get(i));
		}

		j = nlohmann::json{
			{"manager_name", p.get_manager_name()},
			{"setup", p.get_setup()},
			{"managed_models", model_list}
		};
	}

	/**
	 * @brief nlohmann::json에서 ManagerInfo로 변환
	 */
	inline void from_json(const nlohmann::json& j, ManagerInfo& p) {
		p.set_manager_name(j.at("manager_name").get<std::string>().c_str());

		j.at("setup").get_to(p.get_setup());

		SmartList models;
		std::vector<std::string> model_list = j.at("managed_models").get<std::vector<std::string>>();
		for (const auto& model : model_list)
		{
			models.add(model.c_str());
		}
		p.set_managed_models(models);
	}
#endif

} // namespace syai::runtime::domain::config
