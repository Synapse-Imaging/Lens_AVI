#pragma once

#include "../AIService/UniversalAIData.h"
#include "AlgorithmContext.h"
#include <LGIT/types.h>

class Algorithm;

class AlgorithmHelper
{
public:
	explicit AlgorithmHelper(Algorithm* pAlgorithm);

	// Context 
	AlgorithmContext BuildContext(int iCurThreadIdx, int iCurVisionCamIdx, int iCurInspectionBufferIdx);
	void BuildROIContext(ROIAlgorithmContext& roiCtx);

	// Handle
	void HandleTrayChange(const AlgorithmContext& algCtx);

	// Directory 
	std::vector<std::string> GetSaveDirectories(const AlgorithmContext& algCtx);

	// Buffer Copy Helpers 
	void CopyGrabImages(int iScanBufferIdx, const AlgorithmCopyParam& param);

	BOOL ProcessImageConversion(
		int iScanBufferIdx, int iPcVisionNo,
		int iMzNo, int iModuleNo, int iStageNo,
		int iStartImageIdx, int iEndImageIdx,
		int iStartSpecularIdx, int iStartDiffusedIdx,
		CString sVisionCamType_Short, CString sLotID, int iTrayNo);

	// Preprocessing 
	void ApplyPreprocessing(ROIAlgorithmContext& roiCtx);

	// Inspection 
	void InspectAlign(ROIAlgorithmContext& roiCtx);
	BOOL InspectCosmetic(ROIAlgorithmContext& roiCtx);

	// AI Pipeline 
	void SubmitLGAI(ROIAlgorithmContext& roiCtx, ResultCollector& collector,
		const std::string& saveDir, const std::vector<std::string>& saveDirectories);
	void SubmitSYAI(ROIAlgorithmContext& roiCtx, ResultCollector& collector,
		const std::vector<std::string>& saveDirectories);

	void CollectLGAIResults(const AlgorithmContext& algCtx, ResultCollector& collector);
	void CollectSYAIResults(ResultCollector& collector,
		const std::string& localDir, const std::string& lasDir,	bool isSameDir, bool saveLocal, bool saveLAS);

	void StoreDefectRegion(int iBufferIdx, int iImageIdx,
		int iInspectionType, int iDefectType, const HObject& HRegion);
private:
	Algorithm* m_pAlgorithm;

	// Image Save 
	static void SaveROIImage(const std::string& save_dir,
		const std::string& prefix,
		HalconCpp::HObject h_image,
		HalconCpp::HObject h_image_v2);

	void SaveDefectCropImage(const std::string& save_dir,
		const cv::Size& patch_size, int stride,
		const std::string& prefix,
		HalconCpp::HObject h_image,
		const std::string& barcode_id,
		const std::string& inspection_type_name);

#ifdef SYAI
	static void SaveAIImage(const std::string& save_dir,
		const cv::Mat& image, const cv::Mat& image_v2,
		const cv::Size& patch_size, int stride,
		const std::string& prefix,
		const std::string& barcode_id,
		const std::string& inspection_type_name,
		HalconCpp::HObject h_mask_reg,
		cv::Size orignal_image_size, cv::Size roi_image_size,
		cv::Point roi_left_top, cv::Point roi_right_bottom,
		bool save_train_image);

	void SaveSyaiResultDebugInfo(const syai_service::JobInfo& result, const std::string& prefix);

	static void SaveResultMask(std::string local_dir, std::string las_dir,
		std::string file_name, bool is_same_dir,
		bool save_local, bool save_las, const cv::Mat& mask);

	void SaveStepResultMask(const syai_service::JobInfo& result,
		const std::string& local_dir, const std::string& las_dir,
		bool is_same_dir, bool save_local, bool save_las);

	void ConcatDefectRegionVision(Algorithm* algo, const syai_service::JobInfo& result);
	void ConcatDefectRegion(Algorithm* algo, const syai_service::JobInfo& result);
	void ConcatDefectResult(Algorithm* algo, const syai_service::JobInfo& result);
#endif // SYAI
	// AI Universal Data Convert Helpers

	bool ConvertToUniversalInfo(const LGIT::InspectionResultSet& resultSet, std::shared_ptr<AIService::UniversalAIData::UniversalInfo>& outInfo);
#ifdef SYAI
	bool ConvertToUniversalInfo(const syai_service::JobInfo& resultSet, std::shared_ptr<AIService::UniversalAIData::UniversalInfo>& outInfo);
#endif // SYAI
};