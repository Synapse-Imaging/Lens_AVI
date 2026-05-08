#pragma once
namespace syai::runtime::domain::config
{
	/**
	 * @brief 검사 타입 열거형
	 *
	 * 각 검사 타입은 특정한 AI 모델 유형과 연결되며, 처리 방식이 다름.
	 * NONE은 초기화되지 않은 상태를 나타내며, 사용 시 오류가 발생할 수 있음.
	 *
	 * @see ManagerSetup::type
	 */
	enum class InspectionType
	{
		NONE = 0,           //!< 타입이 설정되지 않은 상태 (기본값)
		CLASSIFICATION = 1, //!< 분류 모델 검사 (이미지 클래스 분류)
		SEGMENTATION = 2,   //!< 세그멘테이션 모델 검사 (픽셀 단위 분할)
		DD = 3,
		AD = 4              //!< 
	};

	/**
	 * @brief 디바이스 실행 타입 열거형
	 *
	 * AI 모델 추론을 실행할 하드웨어 디바이스를 지정함.
	 * 성능과 메모리 사용량이 디바이스 타입에 따라 크게 달라질 수 있음.
	 *
	 * @note CUDA 사용 시 NVIDIA GPU와 CUDA 드라이버가 필요함
	 * @note DML 사용 시 DirectX 12 호환 GPU가 필요함
	 * @see ManagerSetup::device_type
	 */
	enum class DeviceType
	{
		CPU = 0,      //!< CPU 실행 (느리지만 호환성 높음)
		CUDA = 1,     //!< NVIDIA CUDA GPU 실행 (빠른 성능)
		DML = 2,      //!< DirectML GPU 실행 (Windows 전용)
		OPENCL = 3,   //!< OpenCL GPU 실행 (크로스 플랫폼)
		OPENVINO = 4, //!< OpenVINO 실행 (인텔 하드웨어)
		TENSORRT = 5  //!< TensorRT 실행 (NVIDIA GPU)
	};

	/**
	 * @brief 로그 레벨 열거형
	 *
	 * spdlog 라이브러리의 로그 레벨과 호환됨.
	 * 숫자가 낮을수록 더 상세한 로그를 출력함.
	 *
	 * @note 운영 환경에서는 INFO 이상의 레벨 사용을 권장
	 * @see ManagerSetup::log_level
	 */
	enum class LogLevel
	{
		TRACE_SY = 0,    //!< 가장 상세한 추적 로그 (개발 디버깅용)
		DEBUG_SY = 1,    //!< 디버그 정보 로그 (개발/테스트용)
		INFO_SY = 2,     //!< 일반적인 정보 로그 (기본 권장값)
		WARN_SY = 3,     //!< 경고 메시지 로그
		ERR_SY = 4,      //!< 오류 메시지 로그
		CRITICAL_SY = 5, //!< 치명적인 오류 로그
		OFF_SY = 6       //!< 모든 로그 비활성화
	};
}
