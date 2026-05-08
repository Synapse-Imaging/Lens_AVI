#pragma once

#include "syai/runtime/SimpleSmartType.h"
#include "ConfigurationEnum.h"
#include "ModelInfoItem.h"
#include <vector>

#if __has_include("nlohmann/json.hpp")
#include "nlohmann/json.hpp"
#include <string>
#include <fstream>
#define INCLUDE_NLOHMANN_JSON_HPP
#endif

namespace syai::runtime::domain::config
{
	// Forward declarations for helper functions
	inline const char* device_type_to_string(DeviceType type);

	/**
	 * @brief Manager 설정을 위한 구성 클래스
	 *
	 * AI 모델 추론 매니저의 초기화에 필요한 모든 설정을 포함함.
	 * 캡슐화와 유효성 검증을 통해 안전한 설정 관리를 제공함.
	 *
	 * 특징:
	 * - 완전한 캡슐화: 모든 멤버 변수는 private, getter/setter를 통한 접근
	 * - 유효성 검증: setter에서 잘못된 값 방지
	 * - Builder 패턴: 가독성 높은 객체 생성
	 * - 자동 메모리 관리: SmartString 사용으로 자동 해제
	 * - 타입 안전성: enum class 사용으로 컴파일 타임 타입 검증
	 *
	 * 사용 예제:
	 * @code
	 *     // Builder 패턴 사용
	 *     auto setup = ManagerSetup::Builder()
	 *         .with_type(InspectionType::CLASSIFICATION)
	 *         .with_model_path("C:\\models\\cls")
	 *         .with_device(DeviceType::CUDA, 0)
	 *         .with_batch_size(8)
	 *         .build();
	 *
	 *     // 전통적인 방식
	 *     ManagerSetup setup2(InspectionType::SEGMENTATION, "C:\\models\\seg");
	 *     setup2.set_device_type(DeviceType::CUDA);
	 *     setup2.set_device_id(1);
	 *
	 *     // 값 접근
	 *     int batch_size = setup.get_max_batch_size();
	 *     DeviceType device = setup.get_device_type();
	 * @endcode
	 *
	 * @see InspectionType, DeviceType, LogLevel, SmartString
	 */
	class ManagerSetup
	{
	public:
		// 전방 선언
		class Builder;

		//! 기본 생성자 (모든 필드를 기본값으로 초기화)
		ManagerSetup() = default;

		/**
		 * @brief 매개변수 생성자
		 * @param type 검사 타입
		 * @param model_path 모델 파일 또는 폴더 경로
		 * @param device_type 실행 디바이스 타입 (기본: CUDA)
		 * @param device_id GPU 번호 등 디바이스 ID (기본: 0)
		 * @param max_batch_size 최대 배치 크기 (기본: 4)
		 * @param log_name 로그 이름 (기본: nullptr)
		 * @param log_root 로그 저장 폴더 (기본: "C:\\sya\\logs")
		 * @param log_level 로그 레벨 (기본: INFO)
		 * @param is_save_image 이미지 저장 활성화 여부 (기본: false)
		 * @param is_save_visualization_image 시각화 이미지 저장 활성화 여부 (기본: false)
		 * @param save_image_root 이미지 저장 폴더 (기본: "C:\\sya\\image")
		 * @param is_save_analysis_log 분석 로그 저장 활성화 여부 (기본: false)
		 * @param analysis_log_root 분석 로그 저장 폴더 (기본: "C:\\sya\\analysis_logs")
		 * @param thread_count 내부 스레드 개수 (기본: 1)
		 * @throws std::invalid_argument 유효하지 않은 파라미터가 전달된 경우
		 */
		ManagerSetup(InspectionType type,
					 const char* model_path,
					 DeviceType device_type = DeviceType::CUDA,
					 int device_id = 0,
					 int max_batch_size = 4,
					 const char* log_name = nullptr,
					 const char* log_root = "C:\\sya\\logs",
					 LogLevel log_level = LogLevel::INFO_SY,
					 bool is_save_image = false,
					 bool is_save_visualization_image = false,
					 const char* save_image_root = "C:\\sya\\image",
					 bool is_save_analysis_log = false,
					 const char* analysis_log_root = "C:\\sya\\analysis_logs",
					 int thread_count = 1)
			: type_(type)
			, model_path_(model_path)
			, device_type_(device_type)
			, log_name_(log_name)
			, log_root_(log_root)
			, log_level_(log_level)
			, is_save_image_(is_save_image)
			, is_save_visualization_image_(is_save_visualization_image)
			, save_image_root_(save_image_root)
			, is_save_analysis_log_(is_save_analysis_log)
			, analysis_log_root_(analysis_log_root)
		{
			set_device_id(device_id);
			set_max_batch_size(max_batch_size);
			set_thread_count(thread_count);
		}

		/**
		 * @brief 복사 생성자 (스마트 포인터 자동 참조 카운팅)
		 * @param other 복사할 ManagerSetup 인스턴스
		 */
		ManagerSetup(const ManagerSetup& other) = default;

		/**
		 * @brief 복사 할당 연산자 (스마트 포인터 자동 참조 카운팅)
		 * @param other 할당할 ManagerSetup 인스턴스
		 * @return 자기 자신 참조
		 */
		ManagerSetup& operator=(const ManagerSetup& other) = default;

		/**
		 * @brief 동등성 비교 연산자
		 * @param other 비교할 ManagerSetup 인스턴스
		 * @return 모든 멤버가 같으면 true
		 */
		inline bool operator==(const ManagerSetup& other) const
		{
			if (type_ != other.type_ ||
				model_path_ != other.model_path_ ||
				device_type_ != other.device_type_ ||
				device_id_ != other.device_id_ ||
				max_batch_size_ != other.max_batch_size_ ||
				log_name_ != other.log_name_ ||
				log_root_ != other.log_root_ ||
				log_level_ != other.log_level_ ||
				is_save_image_ != other.is_save_image_ ||
				is_save_visualization_image_ != other.is_save_visualization_image_ ||
				save_image_root_ != other.save_image_root_ ||
				is_save_analysis_log_ != other.is_save_analysis_log_ ||
					analysis_log_root_ != other.analysis_log_root_ ||
				thread_count_ != other.thread_count_ ||
				image_save_format_ != other.image_save_format_ ||
				is_save_train_image_ != other.is_save_train_image_ ||
				model_infos_.size() != other.model_infos_.size())
				return false;

			// Compare model_infos_ vector
			for (size_t i = 0; i < model_infos_.size(); ++i)
			{
				if (model_infos_[i] != other.model_infos_[i])
					return false;
			}

			return true;
		}

		// ========== Getter 메서드 ==========

		/**
		 * @brief 검사 타입 반환
		 * @return 검사 타입
		 */
		inline InspectionType get_type() const { return type_; }

		/**
		 * @brief 모델 경로를 C 스타일 문자열로 반환
		 * @return 모델 경로 문자열
		 */
		inline const char* get_model_path() const { return model_path_.c_str(); }

		/**
		 * @brief 디바이스 타입 반환
		 * @return 디바이스 타입
		 */
		inline DeviceType get_device_type() const { return device_type_; }

		/**
		 * @brief 디바이스 ID 반환
		 * @return 디바이스 ID
		 */
		inline int get_device_id() const { return device_id_; }

		/**
		 * @brief 최대 배치 크기 반환
		 * @return 최대 배치 크기
		 */
		inline int get_max_batch_size() const { return max_batch_size_; }

		/**
		 * @brief 로그 이름을 C 스타일 문자열로 반환
		 * @return 로그 이름 문자열
		 */
		inline const char* get_log_name() const { return log_name_.c_str(); }

		/**
		 * @brief 로그 루트 디렉터리를 C 스타일 문자열로 반환
		 * @return 로그 저장 폴더 문자열
		 */
		inline const char* get_log_root() const { return log_root_.c_str(); }

		/**
		 * @brief 로그 레벨 반환
		 * @return 로그 레벨
		 */
		inline LogLevel get_log_level() const { return log_level_; }

		/**
		 * @brief 이미지 저장 활성화 여부 반환
		 * @return 이미지 저장 활성화 시 true
		 */
		inline bool get_is_save_image() const { return is_save_image_; }

		/**
		 * @brief 시각화 이미지 저장 활성화 여부 반환
		 * @return 시각화 이미지 저장 활성화 시 true
		 */
		inline bool get_is_save_visualization_image() const { return is_save_visualization_image_; }

		/**
		 * @brief 이미지 저장 루트 디렉터리를 C 스타일 문자열로 반환
		 * @return 이미지 저장 폴더 문자열
		 */
		inline const char* get_save_image_root() const { return save_image_root_.c_str(); }

		/**
		 * @brief 분석 로그 저장 활성화 여부 반환
		 * @return 분석 로그 저장 활성화 시 true
		 */
		inline bool get_is_save_analysis_log() const { return is_save_analysis_log_; }

		/**
		 * @brief 분석 로그 루트 디렉터리를 C 스타일 문자열로 반환
		 * @return 분석 로그 저장 폴더 문자열
		 */
		inline const char* get_analysis_log_root() const { return analysis_log_root_.c_str(); }

		/**
		 * @brief 스레드 개수 반환
		 * @return 스레드 개수
		 */
		inline int get_thread_count() const { return thread_count_; }

		/**
		 * @brief 이미지 저장 포맷 반환
		 * @return 이미지 저장 포맷 문자열 (예: "png", "jpg", "bmp")
		 */
		inline const char* get_image_save_format() const { return image_save_format_.c_str(); }

		/**
		 * @brief 학습용 이미지 저장 활성화 여부 반환
		 * @return 학습용 이미지 저장 활성화 시 true
		 */
		inline bool get_is_save_train_image() const { return is_save_train_image_; }

		/**
		 * @brief 모델 정보 목록 반환 (읽기 전용)
		 * @return 모델 정보 벡터 상수 참조
		 */
		inline const std::vector<ModelInfoItem>& get_model_infos() const { return model_infos_; }

		/**
		 * @brief 모델 정보 목록 반환 (수정 가능)
		 * @return 모델 정보 벡터 참조
		 */
		inline std::vector<ModelInfoItem>& get_model_infos() { return model_infos_; }

		/**
		 * @brief 모델 정보 개수 반환
		 * @return 모델 정보 개수
		 */
		inline int get_model_info_count() const { return static_cast<int>(model_infos_.size()); }

		// ========== 타입 변환 메서드 ==========

		/**
		 * @brief 디바이스 타입을 C 스타일 문자열로 변환
		 * @return 디바이스 타입 문자열 ("cpu", "cuda", "dml", "opencl")
		 */
		inline const char* get_device_type_string() const
		{
			return device_type_to_string(device_type_);
		}

		/**
		 * @brief 로그 레벨을 정수로 변환
		 * @return 로그 레벨 정수값 (0-6)
		 */
		inline int get_log_level_int() const
		{
			return static_cast<int>(log_level_);
		}

		// ========== Setter 메서드 (유효성 검증 포함) ==========

		/**
		 * @brief 검사 타입 설정
		 * @param type 검사 타입
		 */
		inline void set_type(InspectionType type)
		{
			type_ = type;
		}

		/**
		 * @brief 모델 경로 설정
		 * @param path 모델 파일 또는 폴더 경로
		 */
		inline void set_model_path(const char* path)
		{
			model_path_ = path;
		}

		/**
		 * @brief 디바이스 타입 설정
		 * @param dev_type 디바이스 타입 열거형 값
		 */
		inline void set_device_type(DeviceType dev_type)
		{
			device_type_ = dev_type;
		}

		/**
		 * @brief C 스타일 문자열에서 디바이스 타입으로 변환
		 * @param device_str 디바이스 타입 문자열 (nullptr 허용)
		 * @param consider_upper_lower 대소문자 구분 여부 (기본: true)
		 */
		inline void set_device_type_from_string(const char* device_str, bool consider_upper_lower = true)
		{
			if (!device_str) {
				device_type_ = DeviceType::CPU;
				return;
			}

			if (consider_upper_lower) {
				if (string_equals(device_str, "cuda")) {
					device_type_ = DeviceType::CUDA;
				}
				else if (string_equals(device_str, "dml")) {
					device_type_ = DeviceType::DML;
				}
				else if (string_equals(device_str, "opencl")) {
					device_type_ = DeviceType::OPENCL;
				}
				else {
					device_type_ = DeviceType::CPU;
				}
			}
			else {
				if (string_equals_ignore_case(device_str, "cuda")) {
					device_type_ = DeviceType::CUDA;
				}
				else if (string_equals_ignore_case(device_str, "dml")) {
					device_type_ = DeviceType::DML;
				}
				else if (string_equals_ignore_case(device_str, "opencl")) {
					device_type_ = DeviceType::OPENCL;
				}
				else {
					device_type_ = DeviceType::CPU;
				}
			}
		}

		/**
		 * @brief 디바이스 ID 설정 (유효성 검증 포함)
		 * @param id 디바이스 ID (0 이상)
		 * @throws std::invalid_argument ID가 음수인 경우
		 */
		inline void set_device_id(int id)
		{
			if (id < 0) {
				// 예외를 던지는 대신 기본값 설정 (DLL 경계 고려)
				device_id_ = 0;
				return;
			}
			device_id_ = id;
		}

		/**
		 * @brief 최대 배치 크기 설정 (유효성 검증 포함)
		 * @param size 최대 배치 크기 (1-256)
		 * @throws std::invalid_argument 범위를 벗어난 경우
		 */
		inline void set_max_batch_size(int size)
		{
			if (size <= 0) {
				max_batch_size_ = 1;
				return;
			}
			if (size > 256) {
				max_batch_size_ = 256;
				return;
			}
			max_batch_size_ = size;
		}

		/**
		 * @brief 로그 이름 설정
		 * @param name 로그 이름
		 */
		inline void set_log_name(const char* name)
		{
			log_name_ = name;
		}

		/**
		 * @brief 로그 루트 디렉터리 설정
		 * @param root 로그 저장 폴더
		 */
		inline void set_log_root(const char* root)
		{
			log_root_ = root;
		}

		/**
		 * @brief 로그 레벨 설정
		 * @param level 로그 레벨
		 */
		inline void set_log_level(LogLevel level)
		{
			log_level_ = level;
		}

		/**
		 * @brief 정수에서 로그 레벨로 설정
		 * @param level 로그 레벨 (0-6, 범위 외 값은 INFO로 설정)
		 */
		inline void set_log_level_from_int(int level)
		{
			if (level >= 0 && level <= 6) {
				log_level_ = static_cast<LogLevel>(level);
			}
			else {
				log_level_ = LogLevel::INFO_SY;
			}
		}

		/**
		 * @brief 이미지 저장 활성화 여부 설정
		 * @param enable 활성화 시 true
		 */
		inline void set_is_save_image(bool enable)
		{
			is_save_image_ = enable;
		}

		/**
		 * @brief 시각화 이미지 저장 활성화 여부 설정
		 * @param enable 활성화 시 true
		 */
		inline void set_is_save_visualization_image(bool enable)
		{
			is_save_visualization_image_ = enable;
		}

		/**
		 * @brief 이미지 저장 루트 디렉터리 설정
		 * @param root 이미지 저장 폴더
		 */
		inline void set_save_image_root(const char* root)
		{
			save_image_root_ = root;
		}

		/**
		 * @brief 분석 로그 저장 활성화 여부 설정
		 * @param enable 활성화 시 true
		 */
		inline void set_is_save_analysis_log(bool enable)
		{
			is_save_analysis_log_ = enable;
		}

		/**
		 * @brief 분석 로그 루트 디렉터리 설정
		 * @param root 분석 로그 저장 폴더
		 */
		inline void set_analysis_log_root(const char* root)
		{
			analysis_log_root_ = root;
		}

		/**
		 * @brief 스레드 개수 설정 (유효성 검증 포함)
		 * @param count 스레드 개수 (1-64)
		 */
		inline void set_thread_count(int count)
		{
			if (count <= 0) {
				thread_count_ = 1;
				return;
			}
			if (count > 64) {
				thread_count_ = 64;
				return;
			}
			thread_count_ = count;
		}

		/**
		 * @brief 이미지 저장 포맷 설정
		 * @param format 이미지 저장 포맷 (예: "png", "jpg", "bmp")
		 */
		inline void set_image_save_format(const char* format)
		{
			image_save_format_ = format ? format : "";
		}

		/**
		 * @brief 학습용 이미지 저장 활성화 여부 설정
		 * @param enable 활성화 시 true
		 */
		inline void set_is_save_train_image(bool enable)
		{
			is_save_train_image_ = enable;
		}

		/**
		 * @brief 모델 정보 목록 설정
		 * @param model_infos 모델 정보 벡터
		 */
		inline void set_model_infos(const std::vector<ModelInfoItem>& model_infos)
		{
			model_infos_ = model_infos;
		}

		/**
		 * @brief 모델 정보 추가
		 * @param model_info 추가할 모델 정보
		 */
		inline void add_model_info(const ModelInfoItem& model_info)
		{
			model_infos_.push_back(model_info);
		}

		/**
		 * @brief 모델 정보 추가 (매개변수로 직접 생성)
		 * @param name 모델 이름
		 * @param enabled 활성화 여부 (기본: true)
		 * @param threshold 임계값 (기본: 0.5)
		 * @param severity 심각도 (기본: 3)
		 */
		inline void add_model_info(const char* name, bool enabled = true, double threshold = 0.5, int severity = 3)
		{
			model_infos_.emplace_back(name, enabled, threshold, severity);
		}

		/**
		 * @brief 모델 정보 제거 (이름으로 검색)
		 * @param name 제거할 모델 이름
		 * @return 제거되었으면 true
		 */
		inline bool remove_model_info(const char* name)
		{
			if (!name) return false;

			for (auto it = model_infos_.begin(); it != model_infos_.end(); ++it)
			{
				if (strcmp(it->get_model_info_name(), name) == 0)
				{
					model_infos_.erase(it);
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief 모든 모델 정보 제거
		 */
		inline void clear_model_infos()
		{
			model_infos_.clear();
		}

		/**
		 * @brief 특정 인덱스의 모델 정보 반환
		 * @param index 인덱스
		 * @return 모델 정보 포인터 (범위 밖이면 nullptr)
		 */
		inline const ModelInfoItem* get_model_info(int index) const
		{
			if (index < 0 || index >= static_cast<int>(model_infos_.size()))
				return nullptr;
			return &model_infos_[index];
		}

		/**
		 * @brief 특정 이름의 모델 정보 찾기
		 * @param name 모델 이름
		 * @return 모델 정보 포인터 (찾지 못하면 nullptr)
		 */
		inline const ModelInfoItem* find_model_info(const char* name) const
		{
			if (!name) return nullptr;

			for (const auto& info : model_infos_)
			{
				if (strcmp(info.get_model_info_name(), name) == 0)
					return &info;
			}
			return nullptr;
		}

		// ========== 유효성 검증 메서드 ==========

		/**
		 * @brief 디바이스 ID 유효성 검증
		 * @param id 검증할 디바이스 ID
		 * @return 유효하면 true
		 */
		inline bool is_valid_device_id(int id) const
		{
			return id >= 0;
		}

		/**
		 * @brief 배치 크기 유효성 검증
		 * @param size 검증할 배치 크기
		 * @return 유효하면 true
		 */
		inline bool is_valid_batch_size(int size) const
		{
			return size > 0 && size <= 256;
		}

		/**
		 * @brief 스레드 개수 유효성 검증
		 * @param count 검증할 스레드 개수
		 * @return 유효하면 true
		 */
		inline bool is_valid_thread_count(int count) const
		{
			return count > 0 && count <= 64;
		}

		/**
		 * @brief 전체 설정 유효성 검증
		 * @return 모든 설정이 유효하면 true
		 */
		inline bool is_valid() const
		{
			return type_ != InspectionType::NONE &&
				!model_path_.empty() &&
				is_valid_device_id(device_id_) &&
				is_valid_batch_size(max_batch_size_) &&
				is_valid_thread_count(thread_count_);
		}

#ifdef INCLUDE_NLOHMANN_JSON_HPP
		/**
		 * @brief ManagerSetup 객체를 JSON 문자열로 변환
		 * @param pretty JSON을 예쁘게 포맷할지 여부
		 * @return JSON 문자열
		 */
		inline std::string to_json_string(bool pretty = false) const
		{
			nlohmann::json j = *this;
			return j.dump(pretty ? 4 : -1);
		}

		/**
		 * @brief JSON 문자열에서 ManagerSetup 객체 생성
		 * @param json_string JSON 데이터
		 * @return ManagerSetup 객체
		 */
		static inline ManagerSetup from_json_string(const std::string& json_string)
		{
			ManagerSetup setup;
			nlohmann::json::parse(json_string).get_to(setup);
			return setup;
		}

		/**
		 * @brief ManagerSetup 객체를 JSON 파일로 저장
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
		 * @brief JSON 파일에서 ManagerSetup 객체 로드
		 * @param file_path 로드할 파일 경로
		 * @return ManagerSetup 객체
		 * @throws std::runtime_error 파일 읽기 또는 파싱 실패 시
		 */
		static inline ManagerSetup load_from_file(const std::string& file_path)
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
		friend void to_json(nlohmann::json& j, const ManagerSetup& p);
		friend void from_json(const nlohmann::json& j, ManagerSetup& p);
#endif
		// ========== 멤버 변수 (private) ==========

		InspectionType type_ = InspectionType::NONE;							//!< 검사 타입
		SmartString model_path_;												//!< 모델 파일 또는 폴더 경로
		DeviceType device_type_ = DeviceType::CUDA;								//!< 실행할 디바이스 타입
		int device_id_ = 0;														//!< GPU 번호 등
		int max_batch_size_ = 4;												//!< 최대 배치 크기
		SmartString log_name_;													//!< 로그 이름
		SmartString log_root_ = SmartString("C:\\sya\\logs");					//!< 로그 저장 폴더
		LogLevel log_level_ = LogLevel::INFO_SY;								//!< 로그 레벨
		bool is_save_image_ = false;											//!< 이미지 저장 활성화 여부
		bool is_save_visualization_image_ = false;								//!< 시각화 이미지 저장 활성화 여부
		SmartString save_image_root_ = SmartString("C:\\sya\\image");			//!< 이미지 저장 폴더
		bool is_save_analysis_log_ = false;										//!< 분석 로그 저장 활성화 여부
		SmartString analysis_log_root_ = SmartString("C:\\sya\\analysis_logs"); //!< 분석 로그 저장 폴더
		int thread_count_ = 1;													//!< 내부 스레드 개수
		std::vector<ModelInfoItem> model_infos_;								//!< 모델 정보 목록
		std::string image_save_format_;											//!< 이미지 저장 포맷
		bool is_save_train_image_ = false;										//!< 학습용 이미지 저장 여부


		// ========== 헬퍼 함수 (private) ==========

		/**
		 * @brief 문자열 비교 헬퍼 함수
		 * @param str1 첫 번째 문자열
		 * @param str2 두 번째 문자열
		 * @return 문자열이 같으면 true, 다르거나 nullptr이면 false
		 */
		inline bool string_equals(const char* str1, const char* str2) const
		{
			if (!str1 || !str2) return false;

			int i = 0;
			while (str1[i] != '\0' && str2[i] != '\0') {
				if (str1[i] != str2[i]) return false;
				++i;
			}
			return str1[i] == str2[i];
		}

		/**
		 * @brief 대소문자를 무시하고 문자열 비교
		 * @param str1 첫 번째 문자열
		 * @param str2 두 번째 문자열
		 * @return 대소문자 무시하고 같으면 true, 다르면 false
		 */
		inline bool string_equals_ignore_case(const char* str1, const char* str2) const
		{
			if (!str1 || !str2) return false;

			int i = 0;
			while (str1[i] != '\0' && str2[i] != '\0') {
				char c1 = str1[i];
				char c2 = str2[i];

				// 소문자로 변환
				if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
				if (c2 >= 'A' && c2 <= 'Z') c2 += 32;

				if (c1 != c2) return false;
				++i;
			}
			return str1[i] == str2[i];
		}
	};

#ifdef INCLUDE_NLOHMANN_JSON_HPP
	/**
	 * @brief ManagerSetup을 nlohmann::json으로 변환
	 */
	inline void to_json(nlohmann::json& j, const ManagerSetup& p) {
		j = nlohmann::json{
			{"type", p.get_type()},
			{"model_path", p.get_model_path()},
			{"device_type", p.get_device_type()},
			{"device_id", p.get_device_id()},
			{"max_batch_size", p.get_max_batch_size()},
			{"log_name", p.get_log_name()},
			{"log_root", p.get_log_root()},
			{"log_level", p.get_log_level()},
			{"is_save_image", p.get_is_save_image()},
			{"is_save_visualization_image", p.get_is_save_visualization_image()},
			{"save_image_root", p.get_save_image_root()},
			{"is_save_analysis_log", p.get_is_save_analysis_log()},
			{"analysis_log_root", p.get_analysis_log_root()},
			{"thread_count", p.get_thread_count()},
			{"image_save_format", p.get_image_save_format()},
			{"is_save_train_image", p.get_is_save_train_image()},
			{"modelInfos", p.get_model_infos()}
		};
	}

	/**
	 * @brief nlohmann::json에서 ManagerSetup으로 변환
	 */
	inline void from_json(const nlohmann::json& j, ManagerSetup& p) {
		p.set_type(j.at("type").get<InspectionType>());
		p.set_model_path(j.at("model_path").get<std::string>().c_str());
		p.set_device_type(j.at("device_type").get<DeviceType>());
		p.set_device_id(j.at("device_id").get<int>());
		p.set_max_batch_size(j.at("max_batch_size").get<int>());
		p.set_log_name(j.at("log_name").get<std::string>().c_str());
		p.set_log_root(j.at("log_root").get<std::string>().c_str());
		p.set_log_level(j.at("log_level").get<LogLevel>());
		p.set_is_save_image(j.at("is_save_image").get<bool>());
		p.set_is_save_visualization_image(j.at("is_save_visualization_image").get<bool>());
		p.set_save_image_root(j.at("save_image_root").get<std::string>().c_str());
		p.set_is_save_analysis_log(j.at("is_save_analysis_log").get<bool>());
		p.set_analysis_log_root(j.at("analysis_log_root").get<std::string>().c_str());
		p.set_thread_count(j.at("thread_count").get<int>());

		if (j.contains("image_save_format")) {
			p.set_image_save_format(j.at("image_save_format").get<std::string>().c_str());
		}

		if (j.contains("is_save_train_image")) {
			p.set_is_save_train_image(j.at("is_save_train_image").get<bool>());
		}

		if (j.contains("modelInfos")) {
			p.set_model_infos(j.at("modelInfos").get<std::vector<ModelInfoItem>>());
		}
	}
#endif

} // namespace syai::runtime::domain::config
