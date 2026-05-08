#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include <nlohmann/json.hpp>
#include <cstring>

namespace synapse::network::cpp::zeromq::common::models::entry {
    class RequestConnection
    {
    public:
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestConnection();
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestConnection(const char* id, int pathway_count);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ~RequestConnection(); // Destructor to release dynamic memory
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestConnection(const RequestConnection& other); // Copy constructor
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestConnection& operator=(const RequestConnection& other); // Copy assignment
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestConnection(RequestConnection&& other) noexcept; // Move constructor
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API RequestConnection& operator=(RequestConnection&& other) noexcept; // Move assignment

        // ID Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetID() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetID(const char* v);

        // PathwayCount Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API int GetPathwayCount() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetPathwayCount(int v) noexcept;

        // JSON Serialization
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static RequestConnection FromJson(const nlohmann::json& j);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static RequestConnection FromString(const char* str);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API nlohmann::json ToJson() const;

        // Convert to String
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* ToString() const;

    private:
        char* id_ = nullptr; // Dynamic allocation for ID
        int pathway_count_ = 1;

        struct Keys
        {
            static constexpr const char* ID = "id";
            static constexpr const char* PathwayCount = "pathway_count";
        };

        void FreeID(); // Utility to free id_ memory
    };
}
