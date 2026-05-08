#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API __declspec(dllimport)
#endif

#include <nlohmann/json.hpp>
#include "../Enums/FeatureType.h"
#include <cstring>

namespace synapse::network::cpp::zeromq::common::models::entry {
    class ConnectionInfo {
    public:
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ConnectionInfo();
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API ConnectionInfo(const char* address, const char* port, enums::FeatureType feature_type);

        // Address Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetAddress() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetAddress(const char* address) noexcept;

        // Port Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* GetPort() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetPort(const char* port) noexcept;

        // FeatureType Getter and Setter
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API enums::FeatureType GetFeatureType() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API void SetFeatureType(enums::FeatureType featureType) noexcept;

        // JSON Serialization
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static ConnectionInfo FromJson(const nlohmann::json& j);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API static ConnectionInfo FromString(const char* str);
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API nlohmann::json ToJson() const;

        // Convert to String
        SYNAPSE_NETWORK_CPP_ZEROMQ_COMMON_API const char* ToString() const;

        // Equality operator
        bool operator==(const ConnectionInfo& other) const noexcept {
            return strcmp(address_, other.address_) == 0 && strcmp(port_, other.port_) == 0;
        }

    private:
        char address_[128] = ""; // Default to empty string
        char port_[16] = "";     // Default to empty string
        enums::FeatureType feature_type_ = enums::FeatureType::None;

        struct Keys
        {
            static constexpr const char* Address = "address";
            static constexpr const char* Port = "port";
            static constexpr const char* FeatureType = "feature_type";
        };
    };

} // namespace synapse::network::cpp::zeromq::common::models::entry
