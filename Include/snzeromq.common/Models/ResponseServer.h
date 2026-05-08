#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include <nlohmann/json.hpp>
#include <cstring>

namespace synapse::network::cpp::zeromq::common::models
{
    class ResponseServer
    {
    public:
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseServer();
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseServer(const char* id, bool result, const char* message, const char* reason);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ~ResponseServer(); // Destructor
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseServer(const ResponseServer& other); // Copy constructor
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseServer& operator=(const ResponseServer& other); // Copy assignment operator
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseServer(ResponseServer&& other) noexcept; // Move constructor
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseServer& operator=(ResponseServer&& other) noexcept; // Move assignment operator

        // ID Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetID() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetID(const char* v);

        // Result Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API bool GetResult() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetResult(bool v) noexcept;

        // Message Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetMessage() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetMessage(const char* v);

        // Reason Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetReason() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetReason(const char* v);

        // JSON Serialization
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static ResponseServer FromJson(const nlohmann::json& j);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static ResponseServer FromString(const char* str);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API virtual nlohmann::json ToJson() const;

        // Convert to String
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API virtual const char* ToString() const;

    protected:
        char* id_ = nullptr; // Dynamic allocation
        bool result_ = false;
        char* message_ = nullptr; // Dynamic allocation
        char* reason_ = nullptr;  // Dynamic allocation

        void FreeID();      // Utility to free id_
        void FreeMessage(); // Utility to free message_
        void FreeReason();  // Utility to free reason_

        struct Keys
        {
            static constexpr const char* ID = "id";
            static constexpr const char* Result = "result";
            static constexpr const char* Message = "message";
            static constexpr const char* Reason = "reason";
        };
    };
}
