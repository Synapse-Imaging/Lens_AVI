#pragma once
#include "InspectionStepResult.h"
#include "syai/runtime/syai_runtime_inspection_export.h"
#include "syai/runtime/SimpleSmartType.h"
#include "opencv2/core.hpp"

namespace syai::runtime::domain::inspection
{
/**
 * @brief 검사 응답 클래스 (참조 카운팅 기반 자동 메모리 관리)
 *
 * 검사 응답에 필요한 모든 정보를 포함하며,
 * 참조 카운팅을 통해 메모리를 자동으로 관리함.
 * 동일한 내부 데이터를 공유하지 않고 변경이 필요할때만 복제함.
 *
 * 특징:
 * - 자동 메모리 관리: 참조 카운트가 0이 되면 자동으로 메모리 해제
 * - 안전한 참조 카운팅: 동일한 데이터를 여러 인스턴스가 공유
 * - Copy-on-Write: 변경 시에만 데이터를 복제
 * - 단계별 결과 관리: 검사 단계별로 개별 결과를 관리
 *
 * 예제:
 * @code
 *     Response resp("inspection_001", "lot_123");
 *     Response copy = resp;
 *
 *     resp.set_step_result("step1", true, "검사 성공", 0, result_mask1);
 *     resp.set_step_result("step2", false, "불량 발견", -1, result_mask2);
 *
 *     bool overall_success = resp.get_overall_success();
 * @endcode
 */
class SYAIRUNTIMEINSPECTION_API Response
{
private:
struct ResponseData;
ResponseData* data_;

void release();
void add_ref();
void ensure_unique();
void ensure_data();
static SmartString combine_step_model_key(const char* step_name, const char* model_name);

public:
Response();
Response(const char* inspection_id,
 const char* log_id = nullptr);
Response(const Response& other);
Response& operator=(const Response& other);
~Response();

// ========== 기본 Getter 메서드 ==========
const char* get_inspection_id() const;
const char* get_log_id() const;

// ========== 기본 Setter 메서드 ==========
void set_inspection_id(const char* inspection_id);
void set_log_id(const char* log_id);

// ========== 단계별 결과 관리 메서드 ==========
void set_step_result(const char* step_name, bool success, const char* message, int predicted_index, const cv::Mat& mask = cv::Mat());
void set_step_result(const char* step_name, const InspectionStepResult& result);
void register_step(const char* step_name, bool initial_success = false, const char* initial_message = "");
void register_steps(const SmartList& step_names, bool initial_success = false, const char* initial_message = "");
bool set_step_success(const char* step_name, bool success);
bool set_step_message(const char* step_name, const char* message);
bool set_step_mask(const char* step_name, const cv::Mat& mask);
bool set_step_success_and_message(const char* step_name, bool success, const char* message);
bool update_step_result(const char* step_name, bool success, const char* message, int predicted_index, const cv::Mat& mask);
InspectionStepResult& get_step_result_ref(const char* step_name);
InspectionStepResult get_step_result(const char* step_name) const;
bool get_step_success(const char* step_name) const;
const char* get_step_message(const char* step_name) const;
cv::Mat get_step_mask(const char* step_name) const;
bool remove_step_result(const char* step_name);
void clear_step_results();
bool has_step_result(const char* step_name) const;
int get_step_count() const;

// ========== 전체 결과 관련 메서드 ==========
bool get_overall_success() const;
SmartString get_overall_message(const char* separator = "; ") const;
cv::Mat get_overall_mask() const;

// ========== 단계 리스트 메서드 ==========
SmartList get_step_names() const;

// ========== 최종 결과 관련 메서드 ==========
InspectionStepResult get_final_success_step() const;
bool get_final_result() const;

// ========== 유틸리티 메서드 ==========
bool is_valid() const;
int ref_count() const;
bool empty() const;

// ========== 호환성을 위한 레거시 메서드 (deprecated) ==========
bool get_success() const;

// ========== Step-Model 복합 키 메서드 ==========
void set_step_model_result(const char* step_name, const char* model_name, bool success, const char* message, int predicted_index, const cv::Mat& mask = cv::Mat());
void set_step_model_result(const char* step_name, const char* model_name, const InspectionStepResult& result);
void register_step_model(const char* step_name, const char* model_name, bool initial_success = false, const char* initial_message = "");
bool set_step_model_success(const char* step_name, const char* model_name, bool success);
bool set_step_model_message(const char* step_name, const char* model_name, const char* message);
bool set_step_model_mask(const char* step_name, const char* model_name, const cv::Mat& mask);
bool set_step_model_success_and_message(const char* step_name, const char* model_name, bool success, const char* message);
bool update_step_model_result(const char* step_name, const char* model_name, bool success, const char* message, int predicted_index, const cv::Mat& mask);
InspectionStepResult& get_step_model_result_ref(const char* step_name, const char* model_name);
InspectionStepResult get_step_model_result(const char* step_name, const char* model_name) const;
bool get_step_model_success(const char* step_name, const char* model_name) const;
const char* get_step_model_message(const char* step_name, const char* model_name) const;
cv::Mat get_step_model_mask(const char* step_name, const char* model_name) const;
int get_step_model_predicted_index(const char* step_name, const char* model_name) const;
bool remove_step_model_result(const char* step_name, const char* model_name);
bool has_step_model_result(const char* step_name, const char* model_name) const;
};
}