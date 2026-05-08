#pragma once
#include "Configuration.h"

namespace syai::runtime::domain::config
{
	/**
	 * @brief ManagerSetup Builder 클래스
	 *
	 * 가독성 높고 유연한 ManagerSetup 객체 생성을 위한 Builder 패턴 구현.
	 * 메서드 체이닝을 통해 설정을 단계적으로 구성할 수 있음.
	 *
	 * 사용 예제:
	 * @code
	 *     auto setup = ManagerSetupBuilder()
	 *         .with_type(InspectionType::CLASSIFICATION)
	 *         .with_model_path("C:\\models\\cls")
	 *         .with_device(DeviceType::CUDA, 0)
	 *         .with_batch_size(8)
	 *         .with_logging("classifier", "C:\\logs", LogLevel::INFO_SY)
	 *         .with_image_saving(true, true, "C:\\images")
	 *         .with_thread_count(4)
	 *         .build();
	 * @endcode
	 */
	class ManagerSetupBuilder
	{
	public:
		//! 기본 생성자
		ManagerSetupBuilder() = default;

		/**
		 * @brief 검사 타입 설정
		 * @param type 검사 타입
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_type(InspectionType type)
		{
			setup_.set_type(type);
			return *this;
		}

		/**
		 * @brief 모델 경로 설정
		 * @param path 모델 파일 또는 폴더 경로
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_model_path(const char* path)
		{
			setup_.set_model_path(path);
			return *this;
		}

		/**
		 * @brief 디바이스 설정 (타입만)
		 * @param type 디바이스 타입
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_device_type(DeviceType type)
		{
			setup_.set_device_type(type);
			return *this;
		}

		/**
		 * @brief 디바이스 설정 (타입 + ID)
		 * @param type 디바이스 타입
		 * @param id 디바이스 ID
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_device(DeviceType type, int id)
		{
			setup_.set_device_type(type);
			setup_.set_device_id(id);
			return *this;
		}

		/**
		 * @brief 디바이스 설정 (문자열)
		 * @param device_str 디바이스 타입 문자열 ("cpu", "cuda", "dml", "opencl")
		 * @param id 디바이스 ID (기본: 0)
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_device_string(const char* device_str, int id = 0)
		{
			setup_.set_device_type_from_string(device_str);
			setup_.set_device_id(id);
			return *this;
		}

		/**
		 * @brief 배치 크기 설정
		 * @param size 최대 배치 크기
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_batch_size(int size)
		{
			setup_.set_max_batch_size(size);
			return *this;
		}

		/**
		 * @brief 로깅 설정
		 * @param log_name 로그 이름
		 * @param log_root 로그 루트 디렉터리 (기본: nullptr - 기본값 사용)
		 * @param level 로그 레벨 (기본: INFO)
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_logging(const char* log_name,
										  const char* log_root = nullptr,
										  LogLevel level = LogLevel::INFO_SY)
		{
			setup_.set_log_name(log_name);
			if (log_root) {
				setup_.set_log_root(log_root);
			}
			setup_.set_log_level(level);
			return *this;
		}

		/**
		 * @brief 로그 레벨만 설정
		 * @param level 로그 레벨
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_log_level(LogLevel level)
		{
			setup_.set_log_level(level);
			return *this;
		}

		/**
		 * @brief 로그 레벨 설정 (정수)
		 * @param level 로그 레벨 정수 (0-6)
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_log_level_int(int level)
		{
			setup_.set_log_level_from_int(level);
			return *this;
		}

		/**
		 * @brief 이미지 저장 설정
		 * @param enable 이미지 저장 활성화 여부
		 * @param enable_visualization 시각화 이미지 저장 활성화 여부
		 * @param root 이미지 저장 루트 디렉터리 (기본: nullptr - 기본값 사용)
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_image_saving(bool enable,
											   bool enable_visualization = false,
											   const char* root = nullptr)
		{
			setup_.set_is_save_image(enable);
			setup_.set_is_save_visualization_image(enable_visualization);
			if (root) {
				setup_.set_save_image_root(root);
			}
			return *this;
		}

		/**
		 * @brief 분석 로그 저장 설정
		 * @param enable 분석 로그 저장 활성화 여부
		 * @param root 분석 로그 루트 디렉터리 (기본: nullptr - 기본값 사용)
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_analysis_log(bool enable, const char* root = nullptr)
		{
			setup_.set_is_save_analysis_log(enable);
			if (root) {
				setup_.set_analysis_log_root(root);
			}
			return *this;
		}

		/**
		 * @brief 스레드 개수 설정
		 * @param count 스레드 개수
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_thread_count(int count)
		{
			setup_.set_thread_count(count);
			return *this;
		}

		/**
		 * @brief 모든 설정을 한 번에 지정 (전통적인 방식)
		 * @param type 검사 타입
		 * @param model_path 모델 경로
		 * @param device_type 디바이스 타입
		 * @param device_id 디바이스 ID
		 * @param max_batch_size 최대 배치 크기
		 * @return Builder 참조 (메서드 체이닝용)
		 */
		ManagerSetupBuilder& with_basic_config(InspectionType type,
											   const char* model_path,
											   DeviceType device_type = DeviceType::CUDA,
											   int device_id = 0,
											   int max_batch_size = 4)
		{
			setup_.set_type(type);
			setup_.set_model_path(model_path);
			setup_.set_device_type(device_type);
			setup_.set_device_id(device_id);
			setup_.set_max_batch_size(max_batch_size);
			return *this;
		}

		/**
		 * @brief ManagerSetup 객체 생성 및 반환
		 * @return 구성된 ManagerSetup 객체
		 */
		ManagerSetup build() const
		{
			return setup_;
		}

		/**
		 * @brief 현재 구성 상태 확인
		 * @return 유효한 구성이면 true
		 */
		bool is_valid() const
		{
			return setup_.is_valid();
		}

	private:
		ManagerSetup setup_;  //!< 구성 중인 ManagerSetup 객체
	};

} // namespace syai::runtime::domain::config
