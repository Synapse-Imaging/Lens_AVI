// VARIATION_AND_SVM.h - 외부 노출 API 헤더
#include <opencv2/opencv.hpp>
#ifndef Variation_Runtime_API_H
#define Variation_Runtime_API_H
// DLL export/import 매크로
#ifdef Variation_Runtime_EXPORTS
#define Variation_RuntimeAPI __declspec(dllexport)
#else
#define Variation_RuntimeAPI __declspec(dllimport)
#endif
#ifdef __cplusplus
extern "C" {
#endif    
    // model_path는 lgit/VariationModel 이거 경로주시면 됩니다.
    Variation_RuntimeAPI bool VariationModelLoad_OnlyVariation(
        const char* model_path
    );

    
    Variation_RuntimeAPI int VariationRun_Raw_OnlyVariation(
        const unsigned char* inputImageData,       // BGR로 들어옴        
        int width,
        int height,
        const char* modelKey,
        double rotationAngleDegree,
        unsigned char* outMask
    );


    // rotationAngleDegree는 0으로 주시면 됩니다.
    // modelKey 는 CHS-S_BC_Img01_ROI1_Tab1  이런 형태로 주시면 됩니다.
    Variation_RuntimeAPI int VariationRun_CVMat_OnlyVariation(
        const cv::Mat& inputImage,      // 입력 BGR 이미지
        const char* modelKey,
        double rotationAngleDegree,
        cv::Mat& outMask               // 출력 마스크
    );
    

        
    // 업데이트된 모델 이름들 반환
    // count: 반환된 모델 개수
    // 반환값: 모델 이름 문자열 배열 (없으면 nullptr)
    Variation_RuntimeAPI char** CheckingVariationUpdated(int* count);


    // 특정 모델들만 파라미터 업데이트하는 함수
    // model_names: 파라미터 업데이트할 모델 이름 배열 
    //              (CheckingVariationUpdated 함수에서 count를 받고, 1 이상이면 문자배열을 그대로 넣어주시면 될것같습니다)
    // count: 배열 크기
    // 반환: 성공적으로 로드된 모델 개수
    Variation_RuntimeAPI int variationParamUpdate(char** model_names, int count);

#ifdef __cplusplus
}
#endif
#endif // VARIATION_AND_SVM_API_H