#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include "../ResponseServer.h"
#include "ConnectionInfo.h"
#include <vector>

namespace synapse::network::cpp::zeromq::common::models::entry
{
    class ResponseConnection : public ResponseServer
    {
    public:
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseConnection();
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseConnection(const char* id, bool result, const char* message, const char* reason);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ~ResponseConnection();
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseConnection(const ResponseConnection& other);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseConnection& operator=(const ResponseConnection& other);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseConnection(ResponseConnection&& other) noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ResponseConnection& operator=(ResponseConnection&& other) noexcept;

        // Connection Info Management
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void AddConnectionInfo(const ConnectionInfo& connection) noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const ConnectionInfo* GetConnectionInfo(size_t index) const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API size_t GetConnectionInfoSize() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API bool IsConnectionInfoEmpty() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API bool RemoveConnectionInfoByIndex(size_t index) noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API bool RemoveConnectionInfoByValue(const ConnectionInfo& connection) noexcept;

        // JSON Serialization
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static ResponseConnection FromJson(const nlohmann::json& j);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static ResponseConnection FromString(const char* str);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API nlohmann::json ToJson() const override;

        // Convert to String
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* ToString() const override;

    private:
        std::vector<ConnectionInfo> connection_info_;

    protected:
        struct Keys : public ResponseServer::Keys
        {
            static constexpr const char* ConnectionInfo = "connection_info";
        };
    };
}
