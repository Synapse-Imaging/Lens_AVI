#pragma once

// C++ 버전 확인 - C++17 이상 필요
// MSVC의 경우 _MSVC_LANG를 우선적으로 확인
#ifdef _MSC_VER
#if _MSVC_LANG < 201703L && __cplusplus < 201703L
#error "ThreadPool.h requires C++17 or later with MSVC. Please set /std:c++17 or /std:c++latest in compiler options."
#endif
#else
#if __cplusplus < 201703L
#error "ThreadPool.h requires C++17 or later. Please update your compiler settings to use C++17 standard or higher."
#endif
#endif

// Include header files for classes
#include "ConfigurationEnum.h"
#include "ModelInfoItem.h"
#include "ManagerSetup.h"
#include "ManagerInfo.h"

#if __has_include("nlohmann/json.hpp")
#include "nlohmann/json.hpp"
#ifndef INCLUDE_NLOHMANN_JSON_HPP
#define INCLUDE_NLOHMANN_JSON_HPP
#endif // !INCLUDE_NLOHMANN_JSON_HPP
#endif // __has_include("nlohmann/json.hpp")

namespace syai::runtime::domain::config
{
	// ========== 전역 헬퍼 함수들 ==========

	/**
	 * @brief DeviceType을 C 스타일 문자열로 변환
	 * @param type 변환할 디바이스 타입
	 * @return 디바이스 타입 문자열 ("cpu", "cuda", "dml", "opencl" , "openvino")
	 */
	inline const char* device_type_to_string(DeviceType type)
	{
		switch (type) {
		case DeviceType::CPU: return "cpu";
		case DeviceType::CUDA: return "cuda";
		case DeviceType::DML: return "dml";
		case DeviceType::OPENCL: return "opencl";
		case DeviceType::OPENVINO: return "openvino";
		case DeviceType::TENSORRT: return "tensorrt";
		default: return "cpu";
		}
	}

	/**
	 * @brief LogLevel을 정수로 변환
	 * @param level 변환할 로그 레벨
	 * @return 로그 레벨 정수값 (0-6)
	 */
	inline int log_level_to_int(LogLevel level)
	{
		return static_cast<int>(level);
	}

#ifdef INCLUDE_NLOHMANN_JSON_HPP
	// JSON serialization for enums
	NLOHMANN_JSON_SERIALIZE_ENUM(InspectionType, {
		{InspectionType::NONE, "NONE"},
		{InspectionType::CLASSIFICATION, "CLASSIFICATION"},
		{InspectionType::SEGMENTATION, "SEGMENTATION"},
		{InspectionType::DD, "DD"},
		{InspectionType::AD, "AD"}
	})

	NLOHMANN_JSON_SERIALIZE_ENUM(DeviceType, {
		{DeviceType::CPU, "CPU"},
		{DeviceType::CUDA, "CUDA"},
		{DeviceType::DML, "DML"},
		{DeviceType::OPENCL, "OPENCL"}
	})

	NLOHMANN_JSON_SERIALIZE_ENUM(LogLevel, {
		{LogLevel::TRACE_SY, "TRACE_SY"},
		{LogLevel::DEBUG_SY, "DEBUG_SY"},
		{LogLevel::INFO_SY, "INFO_SY"},
		{LogLevel::WARN_SY, "WARN_SY"},
		{LogLevel::ERR_SY, "ERR_SY"},
		{LogLevel::CRITICAL_SY, "CRITICAL_SY"},
		{LogLevel::OFF_SY, "OFF_SY"}
	})
#endif

} // namespace syai::runtime::domain::config