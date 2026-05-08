#pragma once
#ifndef SEGMENTATION_RUNTIME_DLL_H
#define SEGMENTATION_RUNTIME_DLL_H

#ifdef SEGMENTATIONRUNTIME_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include <opencv2/opencv.hpp>

extern "C" {
    /**
     * 모델 초기화 함수: 모델 파일들이 있는 폴더 경로를 전달받아 모델들을 로드합니다.
     * modelDirectory : onnx 있는 디렉토리.
     * deviceName : 아무거나 넣어도 Arc -> Nvidia -> CPU 순으로 찾아서 적용함     
     * @return 성공하면 true, 실패하면 false 반환
     */
    DLL_API bool Segmentation_InitializeModels(const char* modelDirectory, const char* deviceName);


    /**
     * 추론 함수 : Crop 이미지 (256x256) 8개를 받아 세그먼테이션 판정합니다.
     * images : 판정하고자 하는 8개의 이미지를 받습니다 (CV_8UC3, BGR, 256x256)
     * masks : 8개의 빈 cv Mat을 받습니다 (CV_8UC1, 256x256)
     * modelName : Component를 받습니다 (ex : CO, PF, BR...) -> 영역별로 8개씩 이미지 넣어 줘야 합니다. 그게 어려우면 말씀주세요.
     *             한번 판정할 때 (8개 이미지) 모델 하나가 가동됩니다.
     */
    DLL_API int Segmentation_InferenceCVMat(
        const cv::Mat images[8],
        cv::Mat masks[8],
        const char* modelName
    );

    DLL_API int Segmentation_InferenceCVMat_ROI(
        const cv::Mat imageROI,
        const cv::Mat maskROI,
        cv::Mat& maskROI_Result,
        const char* modelName
    );


    DLL_API int Segmentation_Inference_Raw(
        unsigned char* imageBuffer,    // Python numpy array의 raw pointer
        unsigned char* maskBuffer,      // Python numpy array의 raw pointer (출력용)
        const char* modelName
    );

    DLL_API int Segmentation_InferenceCVMat_ROI_Raw(
        unsigned char* imageBuffer,
        unsigned char* maskBuffer,
        unsigned char* resultBuffer,
        int width,
        int height,
        const char* modelName
    );

    // 업데이트된 모델 이름들 반환
    // count: 반환된 모델 개수
    // 반환값: 모델 이름 문자열 배열 (없으면 nullptr)
    DLL_API char** CheckingSegmentationUpdated(int* count);


    // 특정 모델들만 업데이트하는 함수
    // modelDirectory: 모델 파일들이 있는 폴더 경로
    // model_names: 업데이트할 모델 이름 배열 
    //              (CheckingClassificationUpdated 함수에서 count를 받고, 1 이상이면 문자배열을 그대로 넣어주시면 될것같습니다)
    // count: 배열 크기
    // 반환: 성공적으로 로드된 모델 개수
    DLL_API int modelLoad(const char* modelDirectory, char** model_names, int count);



}

#endif // DEFECT_DETECTION_DLL_H