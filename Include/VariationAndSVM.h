// VARIATION_AND_SVM.h - 외부 노출 API 헤더
#include <opencv2/opencv.hpp>
#ifndef VARIATION_AND_SVM_API_H
#define VARIATION_AND_SVM_API_H
// DLL export/import 매크로
#ifdef VARIATION_AND_SVM_EXPORTS
#define VARIATION_AND_SVM_API __declspec(dllexport)
#else
#define VARIATION_AND_SVM_API __declspec(dllimport)
#endif
#ifdef __cplusplus
extern "C" {
#endif    
    VARIATION_AND_SVM_API bool VariationModelLoad(
        const char* model_path
    );    

    // 이미 반영됨
    VARIATION_AND_SVM_API bool SVMModelLoad(
        const char* model_path
    );

    VARIATION_AND_SVM_API void VariationRun_Raw(
        const unsigned char* inputImageData,       // BGR로 들어옴        
        int width,
        int height,
        const char* modelKey,
        double rotationAngleDegree,
        unsigned char* outMask
    );        
    
    VARIATION_AND_SVM_API void VariationRun_Raw_For_Train(
        const unsigned char* inputImageData,       // BGR로 들어옴        
        int width,
        int height,
        const char* modelKey,
        double rotationAngleDegree,
        unsigned char* outMask,
        double bright_min_area, double bright_min_width, double bright_min_height,
        double dark_min_area, double dark_min_width, double dark_min_height,
        double bright_abs_threshold, double bright_var_threshold, double dark_abs_threshold, double dark_var_threshold, double use_channel
    );

    VARIATION_AND_SVM_API int FeatureExtractAndSVMPredict_Raw(
        const unsigned char* bgr_data, int width, int height,
        const unsigned char* mask_data, int mask_width,int mask_height,
        const char* model_key,
        float* result_buffer,
        int buffer_size,

        const char* lot,
        const char* barcode,
        const char* vision,
        const char* imageNo,
        const char* roiNo,
        const char* tabNo,
        const char* inspectionArea
    );

    // 수정 반영 필요함
    // 키는 이전에 CHS-K_BC_Image8_ROI2_Tab1
    VARIATION_AND_SVM_API int FeatureExtractAndSVMPredict_CVMat(
        const cv::Mat& bgr_image,
        const cv::Mat& mask_image,
        const char* model_key,  // 외부에서는 const char*로 받고
        float* result_buffer,
        int buffer_size,
        const char* lot,
        const char* barcode,
        const char* vision,
        const char* imageNo,
        const char* roiNo,
        const char* tabNo,
        const char* inspectionArea
    );

    // 업데이트된 모델 이름들 반환
    // count: 반환된 모델 개수
    // 반환값: 모델 이름 문자열 배열 (없으면 nullptr)
    VARIATION_AND_SVM_API char** CheckingSVMUpdated(int* count);


    // 특정 모델들만 파라미터 업데이트하는 함수
    // model_names: 파라미터 업데이트할 모델 이름 배열 
    //              (CheckingVariationUpdated 함수에서 count를 받고, 1 이상이면 문자배열을 그대로 넣어주시면 될것같습니다)
    // model_dir : onnx 모델 있는 디렉토리
    // count: 배열 크기
    // 반환: 성공적으로 로드된 모델 개수
    VARIATION_AND_SVM_API int SVMModelUpdate(char** model_names, const char* model_dir, int count);


    VARIATION_AND_SVM_API void FeatureExtract_Raw(
        const unsigned char* inputImageData,       // BGR로 들어옴        
        int width,
        int height,
        const unsigned char* inputMaskData,
        double* outFeatures,
        int* outBlobCount
    );
    VARIATION_AND_SVM_API bool SVMPredict_Raw(
        const double* inputFeatures,
        int blobCount,
        const char* modelKey,
        int* outPredictions,
        double* outProbabilities
    );
    VARIATION_AND_SVM_API int TestSimple(
    );
    VARIATION_AND_SVM_API void VariationAndSVM_Run_Raw(
        const unsigned char* inputImageData,    // BGR 이미지
        int width,
        int height,
        const char* modelKey,                   // Variation과 SVM 동일 모델키 사용
        double rotationAngleDegree,
        unsigned char* outMask,                 // 최종 불량 마스크 (재라벨링됨)
        int* outDefectCount);
    VARIATION_AND_SVM_API void SetErrorMessage(const char* message);
    VARIATION_AND_SVM_API const char* GetLastErrorMessage();
#ifdef __cplusplus
}
#endif
#endif // VARIATION_AND_SVM_API_H