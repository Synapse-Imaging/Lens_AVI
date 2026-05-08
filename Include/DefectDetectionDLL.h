#pragma once
#ifndef DEFECT_DETECTION_DLL_H
#define DEFECT_DETECTION_DLL_H

#ifdef DEFECTDETECTIONDLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// C 스타일 인터페이스 함수 (외부에서 호출 가능)
#include <opencv2/opencv.hpp>
extern "C" {
    /**
     * 모델 초기화 함수: 모델 파일들이 있는 폴더 경로를 전달받아 모델들을 로드합니다.
     * modelDirectory가 NULL이거나 빈 문자열이면 DLL에 내장된 모델을 사용합니다.
     * deviceName을 통해 추론 장치를 선택할 수 있습니다("CPU", "GPU", "GPU.1" 등).
     *
     * @param modelDirectory 모델 파일들이 있는 폴더 경로 (NULL 또는 빈 문자열이면 내장 모델 사용)
     * @param deviceName 사용할 장치 이름 (NULL 또는 빈 문자열이면 CPU 사용)
     * @return 성공하면 true, 실패하면 false 반환
     */
    DLL_API bool InitializeModels(const char* modelDirectory, const char* deviceName);

    /**
     * Inference 함수: 이미지와 마스크를 받아 결함 분석을 수행합니다.
     *
     * @param imageData 이미지의 픽셀 데이터 (BGR 형식, CV_8UC3)
     * @param imageWidth 이미지 너비
     * @param imageHeight 이미지 높이
     * @param imageChannels 이미지 채널 수 (보통 3)
     * @param maskData 결함 마스크의 픽셀 데이터 (0: 배경, 1~100: 결함 blob ID)
     * @param maskWidth 마스크 너비
     * @param maskHeight 마스크 높이
     * @param modelName 사용할 모델 이름 (예: "A", "B")
     * @param outBlobCount [출력] 검출된 blob의 개수
     * @return 동적 할당된 float 배열 포인터. 각 blob마다 3개 값이 연속 저장됨:
     *         [blob ID, 최대 클래스 인덱스, 최대 클래스 점수]
     *         사용 후 반드시 FreeInferenceResultFloat로 해제해야 함
     */
    DLL_API float* Inference(
        const unsigned char* imageData, int imageWidth, int imageHeight, int imageChannels,
        const unsigned char* maskData, int maskWidth, int maskHeight,
        const char* modelName,
        int* outBlobCount,

        const char* lot,
        const char* barcode,
        const char* vision,
        const char* imageNo,
        const char* roiNo,
        const char* tabNo,
        const char* inspectionArea
    );

    /**
     * FreeInferenceResultFloat 함수: 추론 결과 메모리를 해제하는 함수
     *
     * @param result Inference 또는 InferenceCVMat 함수가 반환한 포인터
     */
    DLL_API void FreeInferenceResultFloat(float* result);

    /**
     * 모델 리소스 해제 함수: 프로그램 종료 전에 호출하여 모든 모델 자원을 해제합니다.
     */
    DLL_API void DisposeModels();

    /**
     * InferenceCVMat 함수: OpenCV Mat 형식의 이미지와 마스크를 직접 받아 결함 분석을 수행합니다.
     * @param lot : Lot 번호
     * @param barcode : Barcode 번호
     * @param vision : vision (TC / BC)
     * @param imageNo : 이미지 번호
     * @param roiNo : ROI 번호
     * @param tabNo : tab 번호
     * @param inspectionArea : 검사 영역 (BL (배럴),,,, 약어 있잖아요 ? 그거 넣어주세요. 그 영어 두글자.)
     * @param image 이미지 (CV_8UC3, BGR 형식)
     * @param mask 결함 마스크 (CV_8UC1, 0: 배경, 1~100: 결함 blob ID)
     * @param modelName 사용할 모델 이름 (예: "A", "B")
     * @param outBlobCount [출력] 검출된 blob의 개수
     * @return 동적 할당된 float 배열 포인터. 각 blob마다 3개 값이 연속 저장됨:
     *         [blob ID, 최대 클래스 인덱스, 최대 클래스 점수]
     *         사용 후 반드시 FreeInferenceResultFloat로 해제해야 함
     */
    DLL_API float* InferenceCVMat(
        const char* lot,
        const char* barcode,
        const char* vision,
        const char* imageNo,
        const char* roiNo,
        const char* tabNo,
        const char* inspectionArea,

        const cv::Mat& image,
        const cv::Mat& mask,
        const char* modelName,
        int* outBlobCount
    );
}

#endif // DEFECT_DETECTION_DLL_H