// InferenceEngine.h - 수정된 헤더 파일
#pragma once

#ifdef INFERENCEENGINE_EXPORTS
#define INFERENCEENGINE_API __declspec(dllexport)
#else
#define INFERENCEENGINE_API __declspec(dllimport)
#endif

#include <opencv2/opencv.hpp>

extern "C" {
    typedef void* InferenceEngineHandle;

    /**
     * @brief 추론 엔진 인스턴스를 생성합니다.
     * @param device 추론 장치 (예: "CPU", "GPU")
     * @param deviceId 장치 ID (GPU인 경우 카드 번호, -1은 기본값)
     * @param loggingLevel 로깅 레벨 (0: VERBOSE, 1: INFO, 2: WARNING, 3: ERROR, 4: FATAL)
     * @return 생성된 추론 엔진 핸들, 실패 시 NULL 반환
     */
    INFERENCEENGINE_API InferenceEngineHandle CreateInferenceEngine(
        const char* device,
        int deviceId,
        int loggingLevel
    );

    /**
     * @brief 추론 엔진 인스턴스를 해제합니다.
     * @param engine 추론 엔진 핸들
     */
    INFERENCEENGINE_API void DestroyInferenceEngine(
        InferenceEngineHandle engine
    );

    /**
     * @brief 메모리 데이터 파일을 로드합니다.
     * @param engine 추론 엔진 핸들
     * @param memoryFilePath 메모리 데이터 파일 경로
     */
    INFERENCEENGINE_API void InferenceEngine_LoadMemory(
        InferenceEngineHandle engine,
        const char* memoryFilePath
    );

    /**
     * @brief 임계값 설정 JSON 파일을 로드합니다.
     * @param engine 추론 엔진 핸들
     * @param thresholdsJsonPath 임계값 설정 JSON 파일 경로
     */
    INFERENCEENGINE_API void InferenceEngine_LoadThresholds(
        InferenceEngineHandle engine,
        const char* thresholdsJsonPath
    );

    /**
     * @brief 이미지에 대한 추론을 실행하고 결함 검출 결과를 반환합니다.
     * @param engine 추론 엔진 핸들
     * @param query 입력 이미지
     * @param dataPtr 추론 결과 데이터 포인터 (출력)
     * @param dataSize 추론 결과 데이터 크기 (개수) 를 받을 곳 (출력 파라미터)
     * @param verbose 상세 로그 사용 여부
     * @param memoryKey 사용할 메모리 데이터의 키
     * @param maskOut 결함 표시된 마스크 이미지 (출력)
     */
    INFERENCEENGINE_API void InferenceEngine_Run(
        InferenceEngineHandle engine,
        cv::Mat query,
        float*& dataPtr,
        size_t& dataSize,
        bool verbose,
        const char* memoryKey,
        cv::Mat& maskOut
    );

    /**
     * @brief 특정 메모리 데이터를 해제합니다.
     * @param engine 추론 엔진 핸들
     * @param memoryKey 해제할 메모리 데이터의 키
     */
    INFERENCEENGINE_API void InferenceEngine_FreeMemoryData(
        InferenceEngineHandle engine,
        const char* memoryKey
    );

    /**
     * @brief 모든 메모리 데이터를 해제합니다.
     * @param engine 추론 엔진 핸들
     */
    INFERENCEENGINE_API void InferenceEngine_FreeAllMemoryData(
        InferenceEngineHandle engine
    );

    /**
     * @brief 메모리 제한을 설정합니다.
     * @param engine 추론 엔진 핸들
     * @param memoryLimitMB 메모리 제한 (MB 단위)
     */
    INFERENCEENGINE_API void InferenceEngine_SetMemoryLimit(
        InferenceEngineHandle engine,
        int memoryLimitMB
    );

    /**
     * @brief 버퍼를 사용하여 추론을 실행합니다.
     * @param engine 추론 엔진 핸들
     * @param imageData 입력 이미지 버퍼 (예: BGR 8bit)
     * @param width 이미지 폭
     * @param height 이미지 높이
     * @param channels 채널 수 (예: 3이면 BGR, 1이면 GRAY 등)
     * @param outDataPtr 추론 결과(예: float 배열) 의 포인터를 받을 곳 (출력 파라미터)
     * @param outDataSize 추론 결과 배열의 길이(개수) 를 받을 곳 (출력 파라미터)
     * @param verbose 상세 로그 사용 여부
     * @param memoryKey 추가 메모리 키(문자열) - 필요 없으면 nullptr
     * @param maskDataPtr 마스크 결과(8bit) 의 포인터를 받을 곳 (출력 파라미터)
     * @param maskWidth 마스크 폭 (출력)
     * @param maskHeight 마스크 높이 (출력)
     * @return 0이면 성공, 0이 아니면 에러
     */
    INFERENCEENGINE_API int InferenceEngine_RunFromBuffer(
        InferenceEngineHandle engine,
        const unsigned char* imageData,
        int width,
        int height,
        int channels,
        float** outDataPtr,
        size_t* outDataSize,
        bool verbose,
        const char* memoryKey,
        unsigned char** maskDataPtr,
        int* maskWidth,
        int* maskHeight
    );
};