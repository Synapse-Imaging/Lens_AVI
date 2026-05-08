#pragma once
#include "../domain/config/Configuration.h"
#include "../domain/inspection/Request.h"
#include "../domain/inspection/Response.h"
#include "../syai_runtime_inspection_export.h"

// #include <memory>

namespace syai::runtime::inspection
{
	// 전방 선언
	class IInspectionManager;

	/**
	 * @brief 검사 서비스 결과 상태 열거형
	 *
	 * 각 작업의 성공/실패 상태와 구체적인 실패 원인을 나타냄.
	 */
	enum class ServiceStatus
	{
		SUCCESS = 0,        //!< 작업 성공
		INVALID_CONFIG = 1, //!< 잘못된 설정
		MANAGER_NOT_FOUND = 2, //!< 매니저를 찾을 수 없음
		INITIALIZATION_FAILED = 3, //!< 초기화 실패
		RUNTIME_ERROR = 4   //!< 런타임 오류
	};

	/**
	* @brief Step 상세 정보 구조체
	*
	* 모델에 설정된 각 Step의 이름, 활성화 여부, 심각도를 포함합니다.
	*/
	struct SYAIRUNTIMEINSPECTION_API StepInfo
	{
		SmartString step_name;      //!< Step 이름
		bool is_enabled;            //!< 활성화 여부
		int severity;               //!< 심각도 (높을수록 우선 실행)

		StepInfo() : is_enabled(false), severity(0) {}
		StepInfo(const char* name, bool enabled, int sev)
			: step_name(name), is_enabled(enabled), severity(sev) {
		}
	};

	/**
	* @brief C++ 검사 서비스 클래스
	*
	* 멀티 인스턴스 지원, RAII 패턴 적용
	* 각 인스턴스는 독립적인 매니저 컬렉션과 로거를 관리
	*
	* 특징:
	* - 인스턴스별 독립적인 상태 관리
	* - 자동 리소스 정리 (RAII)
	* - 스레드 안전성 보장
	* - 완전한 구현 은닉 (Pimpl 패턴)
	*
	* 사용 예:
	* @code
	*     InspectionService service;
	*     auto status = service.Initialize("log", "C:\\logs", LogLevel::INFO, setups);
	*
	*     Response resp;
	*     service.InspectImage(img, steps, key, false, true, id, resp);
	*
	*     service.Finalize(); // 또는 자동 소멸자 호출
	* @endcode
		*/
	class SYAIRUNTIMEINSPECTION_API InspectionService
	{
	public:
		InspectionService();
		~InspectionService();

		// 복사 방지 (매니저 상태의 복잡성 때문)
		InspectionService(const InspectionService&) = delete;
		InspectionService& operator=(const InspectionService&) = delete;

		// 이동 지원
		InspectionService(InspectionService&&) noexcept;
		InspectionService& operator=(InspectionService&&) noexcept;

		// ========== Getter ==========

		/**
		* @brief 서비스 ID 반환
		* @return 서비스 고유 ID
		*/
		const char* GetServiceID() const noexcept;

		/**
		* @brief 초기화 여부 확인
		* @return 초기화되었으면 true
		*/
		bool IsInitialized() const noexcept;

		/**
		* @brief 현재 등록된 매니저 개수 반환
		* @return 매니저 개수
		*/
		int GetManagerCount() const noexcept;

		/**
		* @brief 현재 등록된 모든 매니저 이름 목록 반환
		* @return 매니저 이름 목록
		*/
		SmartList GetManagerNames() const;

		/**
		* @brief 특정 모델 이름에 대한 정렬된 Step 순서 조회
		*
		* 모델의 is_enabled 여부와 severity 기준으로 정렬된 Step 순서를 반환합니다.
		* severity가 높을수록 먼저 실행됩니다.
		*
		* @param model_name 조회할 모델 이름
		* @return 정렬된 Step 이름 목록
		*
		* @code
		*     auto sorted_steps = service.GetStepOrderForModel("Contamination");
		*     // sorted_steps = {"Step3", "Step1"} (severity 5, 1 순서)
		* @endcode
		*/
		SmartList GetStepOrderForModel(const char* model_name) const;

		/**
		* @brief 특정 모델에 대한 정렬된 Step의 첫 번째 값 조회
		*
		* 모델의 is_enabled 여부와 severity 기준으로 정렬된 Step 중에서
		* 첫 번째(가장 높은 우선순위) Step을 반환합니다.
		* 정렬된 Step이 없으면 빈 문자열을 반환합니다.
		*
		* @param model_name 조회할 모델 이름
		* @return 첫 번째 정렬된 Step 이름 (없으면 빈 문자열)
		*
		* @code
		*     auto first_step = service.GetFirstStepForModel("Contamination");
		*     // first_step = "Step3" (가장 높은 severity)
		* @endcode
		*/
		SmartString GetFirstStepForModel(const char* model_name) const;

		/**
		* @brief 특정 모델의 모든 Step 상세 정보 조회
		*
		* 모델에 설정된 모든 Step의 이름, is_enabled, severity 정보를 반환합니다.
		* is_enabled가 false인 Step도 포함됩니다.
		*
		* @param model_name 조회할 모델 이름
		* @return Step 상세 정보 목록 (SmartVector<StepInfo>)
		*
		* @code
		*     auto step_details = service.GetStepDetailsForModel("Contamination");
		*     for (int i = 0; i < step_details.size(); i++) {
		*         const auto& info = step_details[i];
		*         // info.step_name, info.is_enabled, info.severity 사용
		*     }
		* @endcode
		*/
		SmartVector<StepInfo> GetStepDetailsForModel(const char* model_name) const;

		/**
		* @brief 설정된 모든 모델 이름 목록 조회
		*
		* InitializeFromConfig 또는 build_model_step_order_map을 통해 설정된
		* 모든 모델의 이름을 반환합니다.
		*
		* @return 모델 이름 목록
		*
		* @code
		*     auto model_names = service.GetAllModelNames();
		*     for (int i = 0; i < model_names.size(); i++) {
		*         auto details = service.GetStepDetailsForModel(model_names.get(i));
		*     }
		* @endcode
		*/
		SmartList GetAllModelNames() const;

		// ========== 초기화 및 종료 ==========

		/**
		* @brief 검사 서비스를 초기화합니다.
		*
		* @param log_name 로거 이름
		* @param log_root 로그 파일이 저장될 루트 디렉토리
		* @param log_level 로그 레벨
		* @param manager_setups 초기화할 매니저들의 설정 정보
		* @return ServiceStatus 초기화 결과
		*
		* @note 이 함수는 스레드 안전합니다.
		*/
		ServiceStatus Initialize(const char* log_name,
								 const char* log_root,
								 domain::config::LogLevel log_level,
								 SmartVector<domain::config::ManagerInfo> manager_setups);

		/**
		* @brief JSON 설정 파일로부터 검사 서비스를 초기화합니다.
		*
		* JSON 파일에는 C# ConfigurationData 구조와 호환되는 설정이 포함되어야 합니다:
		* - version: 설정 파일 버전
		* - lastModified: 마지막 수정 시간
		* - aiSteps: AI 검사 단계 목록
		*
		* @param config_file_path JSON 설정 파일 경로
		* @return ServiceStatus 초기화 결과
		*
		* @note 이 함수는 스레드 안전합니다.
		* @note JSON 파일이 존재하지 않거나 파싱에 실패하면 INVALID_CONFIG를 반환합니다.
		*
		* @code
		*     InspectionService service;
		*     auto status = service.InitializeFromConfig("C:\\config\\inspection.json");
		*     if (status == ServiceStatus::SUCCESS) {
		*         // 초기화 성공
		*     }
		* @endcode
		*/
		ServiceStatus InitializeFromConfig(const char* config_file_path);

		/**
		* @brief 기존 서비스를 종료하고 새로운 설정으로 재초기화합니다.
		*
		* 이 함수는 다음 순서로 동작합니다:
		* 1. 기존 서비스 종료 (Finalize)
		* 2. 새로운 설정으로 초기화 (Initialize)
		*
		* @param log_name 로거 이름
		* @param log_root 로그 파일이 저장될 루트 디렉토리
		* @param log_level 로그 레벨
		* @param manager_setups 초기화할 매니저들의 설정 정보
		* @return ServiceStatus 재초기화 결과
		*
		* @note 이 함수는 스레드 안전합니다.
		* @note 종료 또는 초기화 중 하나라도 실패하면 실패 상태를 반환합니다.
		*/
		ServiceStatus Reinitialize(const char* log_name,
								   const char* log_root,
								   domain::config::LogLevel log_level,
								   SmartVector<domain::config::ManagerInfo> manager_setups);

		/**
		* @brief JSON 설정 파일로부터 서비스를 재초기화합니다.
		*
		* 이 함수는 다음 순서로 동작합니다:
		* 1. 기존 서비스 종료 (Finalize)
		* 2. JSON 설정으로 초기화 (InitializeFromConfig)
		*
		* @param config_file_path JSON 설정 파일 경로
		* @return ServiceStatus 재초기화 결과
		*
		* @note 이 함수는 스레드 안전합니다.
		* @note JSON 파일이 존재하지 않거나 파싱에 실패하면 INVALID_CONFIG를 반환합니다.
		*
		* @code
		*  InspectionService service;
		*     service.InitializeFromConfig("C:\\config\\old_config.json");
		*     // ... 검사 수행 ...
		*     // 새로운 설정으로 재초기화
		*     auto status = service.ReinitializeFromConfig("C:\\config\\new_config.json");
		* @endcode
		*/
		ServiceStatus ReinitializeFromConfig(const char* config_file_path);

		/**
		* @brief 검사 서비스를 종료하고 모든 리소스를 정리합니다.
		*
		* @return ServiceStatus 종료 결과
		*
		* @note 이 함수는 스레드 안전합니다.
		* @note 종료 후 다시 Initialize를 호출하여 재초기화할 수 있습니다.
		*/
		ServiceStatus Finalize();

		// ========== 매니저 관리 ==========

		/**
		* @brief 지정된 이름의 매니저를 제거합니다.
		*
		* @param manager_name 제거할 매니저의 이름
		* @return ServiceStatus 제거 결과
		*
		* @note 이 함수는 스레드 안전합니다.
		*/
		ServiceStatus RemoveManager(const char* manager_name);

		// ========== 검사 작업 ==========

		/**
		* @brief 이미지 검사를 수행합니다.
		*
		* @param img 검사할 이미지
		* @param key 모델 키
		* @param normalize 이미지 정규화 여부
		* @param bgr_to_rgb BGR을 RGB로 변환할지 여부
		* @param id 검사 ID
		* @param response 검사 결과를 담을 Response 객체
		* @return 0이면 성공, 그 외는 실패
		*
		* @note 이 함수는 스레드 안전합니다.
		*/
		int InspectImage(const cv::Mat& img,
						 const SmartString& key,
						 bool normalize,
						 bool bgr_to_rgb,
						 const SmartString& id,
						 domain::inspection::Response& response);

		int InspectImageChainedStep(const cv::Mat& img,
									const SmartString& key,
									bool normalize,
									bool bgr_to_rgb,
									const SmartString& id,
									domain::inspection::Response& response);

		/**
		* @brief 원본 이미지와 마스크 이미지를 이용한 추론을 수행합니다.
		*
		* @param request 추론 요청 정보
		* @param response 추론 결과를 담을 Response 객체 (외부에서 초기화됨)
		* @return bool 추론 성공 여부
		*
		* @note 이 함수는 스레드 안전합니다.
		* @note response 객체는 호출자가 초기화해서 전달해야 합니다.
		*/
		bool InferenceRawMaskImage(const domain::inspection::Request& request, domain::inspection::Response& response);

		// ========== 메모리 관리 ==========

		/**
		* @brief domain::inspection::Request 객체의 메모리를 강제로 해제합니다.
		*
		* @param request 해제할 domain::inspection::Request 객체의 참조
		* @return bool 해제 성공 여부
		*
		* @note 이 함수는 스레드 안전합니다.
		* @warning 해제 후 해당 domain::inspection::Request 객체 사용 시 예상치 못한 동작이 발생할 수 있습니다.
		*/
		bool ReleaseRequest(domain::inspection::Request& request);

		/**
		* @brief Response 객체의 메모리를 강제로 해제합니다.
		*
		* @param response 해제할 Response 객체의 참조
		* @return bool 해제 성공 여부
		*
		* @note 이 함수는 스레드 안전합니다.
		* @warning 해제 후 해당 Response 객체 사용 시 예상치 못한 동작이 발생할 수 있습니다.
		*/
		bool ReleaseResponse(domain::inspection::Response& response);

		// ========== 유틸리티 ==========

		/**
		* @brief 학습용 크롭 이미지를 저장합니다.
		*
		* @param save_dir 이미지를 저장할 디렉토리 경로
		* @param image 크롭할 원본 이미지
		* @param patch_size 크롭할 패치의 크기
		* @param stride 크롭 간격
		* @param prefix 저장될 파일명의 접두사
		*/
		void SaveTrainCropImage(const std::string& save_dir,
								const cv::Mat& image,
								const cv::Size& patch_size,
								int stride,
								const std::string& prefix);

		/**
		* @brief 학습용 크롭 이미지를 저장합니다.
		*
		* @param save_dir 이미지를 저장할 디렉토리 경로
		* @param image 크롭할 원본 이미지
		* @param mask 크롭할 마스크 이미지
		* @param patch_size 크롭할 패치의 크기
		* @param stride 크롭 간격
		* @param prefix 저장될 파일명의 접두사
		*/
		void SaveTrainCropImageWithMask(const std::string& save_dir,
										const cv::Mat& image,
										const cv::Mat& mask,
										const cv::Size& patch_size,
										int stride,
										const std::string& prefix);

		/**
		* @brief 이미지를 저장합니다.
		*
		* @param save_dir 이미지를 저장할 디렉토리 경로
		* @param filename 저장할 파일명
		* @param image 저장할 이미지
		* @param prefix 저장될 파일명의 접두사 (선택 사항)
		*/
		void SaveImage(const std::string& save_dir,
					   const std::string& filename,
					   const cv::Mat& image,
					   const std::string& prefix);

	private:
		struct OpaqueImpl;
		OpaqueImpl* pImpl_;
	};
} // namespace syai::runtime::inspection