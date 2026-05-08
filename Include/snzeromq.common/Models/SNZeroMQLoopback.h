#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include <nlohmann/json.hpp>
#include <ctime>

namespace synapse::network::cpp::zeromq::common::models
{
    class SNZeroMQLoopback
    {
    public:
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API SNZeroMQLoopback();
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API SNZeroMQLoopback(const char* id, time_t send_time);

        // Get the send time
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API time_t GetSendTime() const;

        // Set the send time
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetSendTime(time_t send_time);

        // Get ID
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetID() const;

        // Set ID
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetID(const char* v);

        // JSON Serialization
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static SNZeroMQLoopback FromJson(const nlohmann::json& j);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static SNZeroMQLoopback FromString(const char* str);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API virtual nlohmann::json ToJson() const;


        // Convert to String
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API virtual const char* ToString() const;

    private:
        char id_[256] = "";
        time_t send_time_;

    protected:
        struct Keys
        {
            static constexpr const char* ID = "id";
            static constexpr const char* SendTime = "send_time";
        };
    };
}
