#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include <nlohmann/json.hpp>
#include "../Enums/Sam2DataType.h"
#include "../Enums/Sam2ModelType.h"
#include <cstring>

namespace synapse::network::cpp::zeromq::common::models::sam2
{
    class RequestSam2
    {
    public:
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestSam2();
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestSam2(const char* id, enums::Sam2DataType data_type, enums::Sam2ModelType model_type, const char* file_path);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ~RequestSam2(); // Destructor
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestSam2(const RequestSam2& other); // Copy constructor
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestSam2& operator=(const RequestSam2& other); // Copy assignment operator
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestSam2(RequestSam2&& other) noexcept; // Move constructor
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestSam2& operator=(RequestSam2&& other) noexcept; // Move assignment operator

        // ID Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetID() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetID(const char* v);

        // DataType Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API enums::Sam2DataType GetDataType() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetDataType(enums::Sam2DataType v) noexcept;

        // ModelType Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API enums::Sam2ModelType GetModelType() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetModelType(enums::Sam2ModelType v) noexcept;

        // FilePath Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetFilePath() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetFilePath(const char* v);

        // JSON Serialization
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static RequestSam2 FromJson(const nlohmann::json& j);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static RequestSam2 FromString(const char* str);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API nlohmann::json ToJson() const;

        // Convert to String
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* ToString() const;

    private:
        char* id_ = nullptr; // Dynamic allocation
        enums::Sam2DataType data_type_ = enums::Sam2DataType::None;
        enums::Sam2ModelType model_type_ = enums::Sam2ModelType::Undefined;
        char* file_path_ = nullptr; // Dynamic allocation

        void FreeID();      // Utility to free id_
        void FreeFilePath(); // Utility to free file_path_

        struct Keys
        {
            static constexpr const char* ID = "id";
            static constexpr const char* DataType = "data_type";
            static constexpr const char* ModelType = "model_type";
            static constexpr const char* FilePath = "file_path";
        };
    };
}
