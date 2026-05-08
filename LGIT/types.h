#pragma once
#include <string>
#include <vector>
#include <variant>
#include <future>
#include <memory>
#include <optional>
#include <opencv2/opencv.hpp>
#include <halconcpp/HalconCpp.h>
#include "Model/LAIInspectorResult.h"
#include "Model/LAIInspector2Result.h"
#include "Model/LAIInspector3Result.h"
#include "Model/LAIInspectorSVMResult.h"
#include "Model/BoundingBox.h"
#include "../ADJClientService.h"

namespace LGIT {

	struct InspectionRequest {
		std::string save_path = "D:\\CMI_Results";
		bool primary_inspect_enable = false;
		bool secondary_inspect_enable = false;
		bool tertiary_inspect_enable = false;
		bool svm_inspect_enable = false;
		std::string pc_id = "";
		std::string equip_no = "";
		std::string product_name = "";
		int vision_inspection_thread_index = 0;
		std::string lot_id;
		int vision_cam = 0;
		int magazine_no = 0;
		int tray_no = 0;
		int module_no = 0;
		std::string vision_name = "";
		int pc_idx = 0;
		int stage_idx = 0;
		int image_idx = 0;
		int roi_idx = 0;
		int roi_id = 0;
		std::string inspectiontype_name = "";
		int roi_inspection_type_idx = 0;
		int inspection_tab_idx = 0;
		Model::BoundingBox roi_bbox;
		int org_image_width;
		int org_image_height;
		cv::Mat image;
		cv::Mat roi_image;
		HalconCpp::HObject vision_defect_region;
		bool test_mode = false;
		uint64_t job_id = -1;
		std::string barcode;
		int64_t inspect_avi_ms = -1;
		int defect_name_index = -1;
		std::string defect_name = "";
		std::string local_align_angle_string = "P0000000";
		double local_align_angle = 0.0;
		int local_align_deltaX = 0;
		int local_align_deltaY = 0;
		std::string channel_type = "I";

		bool is_use_val = false; // VAL 사용 여부
		bool is_use_local_segmentation = false; // 지역 세그먼테이션 사용 여부
		
		InspectionRequest(
			const std::string& save_path,
			bool primary, bool secondary, bool tertiary, bool svm_enable,
			const std::string& pc_id,
			const std::string& equip_no,
			const std::string& product_name,
			int thread_idx,
			std::string lot_id, int vision_cam, int magazine_no, 
			int tray_no, int module_no,
			const std::string& vision_name, int pc_idx, int stage_idx,
			int image_idx, int roi_idx, int roi_id,
			const std::string& inspectiontype_name, int roi_inspection_type_idx,
			int inspection_tab_idx, int defect_name_index, std::string defect_name,
			std::string local_align_angle_string, double local_align_angle, int local_align_deltaX, int local_align_deltaY, std::string channel_type,
			const cv::Mat& img_mat, 
			const cv::Mat& roi_image,
			const HalconCpp::HObject& vision_defect_region) :
			save_path(save_path),
			primary_inspect_enable(primary),
			secondary_inspect_enable(secondary),
			tertiary_inspect_enable(tertiary),
			svm_inspect_enable(svm_enable),
			pc_id(pc_id),
			equip_no(equip_no),
			product_name(product_name),
			vision_inspection_thread_index(thread_idx),
			vision_cam(vision_cam),
			magazine_no(magazine_no),
			lot_id(lot_id),
			tray_no(tray_no),
			module_no(module_no),
			vision_name(vision_name),
			pc_idx(pc_idx),
			stage_idx(stage_idx),
			image_idx(image_idx),
			roi_idx(roi_idx),
			roi_id(roi_id),
			inspectiontype_name(inspectiontype_name),
			roi_inspection_type_idx(roi_inspection_type_idx),
			inspection_tab_idx(inspection_tab_idx),
			defect_name_index(defect_name_index),
			defect_name(defect_name),
			local_align_angle_string(local_align_angle_string),
			local_align_angle(local_align_angle),
			local_align_deltaX(local_align_deltaX),
			local_align_deltaY(local_align_deltaY),
			channel_type(channel_type),
			image(std::move(img_mat)),
			roi_image(std::move(roi_image)),
			vision_defect_region(std::move(vision_defect_region))
		{
		}

		void release_images() {
			image.release();
			try {
				vision_defect_region.Clear();
			}
			catch (...) {

			}
		}
	};

	enum class JobStatus {
		Pending,
		Running,
		Step1_Preprocessing,
		SVM_Preprocessing,
		Step2_Preprocessing,
		Step3_Preprocessing,
		StepSEG_Preprocessing,
		StepSEG_Inspection,
		Step1_Done,
		StepS_Done,
		Step2_Done,
		Step3_Done,
		Completed,
		Failed,
		Vision,
		Step_val_Preprocessing,
	};

	struct InspectionResultSet {
		std::shared_ptr<InspectionRequest> request;
		std::optional<Model::LAIInspectorResult> primary_result;
		std::optional<Model::LAIInspectorSVMResult> svm_result;
		std::optional<Model::LAIInspector2Result> secondary_result;
		std::optional<Model::LAIInspector3Result> tertiary_result;
		JobStatus inspect = JobStatus::Pending;

		// 명시적 데이터 해제 함수 추가
		void clear() {
			request.reset();
			primary_result.reset();
			svm_result.reset();
			secondary_result.reset();
			tertiary_result.reset();
			inspect = JobStatus::Pending;
		}
	};
}
