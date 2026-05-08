#pragma once

/***************************************************************************
 *   FeatureBase
 *
 *   Base class for handling features in the ZeroMQ client service.
 *   Provides common functionality such as token management,
 *   initialization, and communication via SocketManager.
 *
 *   Author: Choi-Dongjin
 *   Date: 2024-01-06
 ***************************************************************************/

#include <string>
#include <cppzmq/zmq.hpp>

#include <snzeromq.common/Models/Enums/FeatureType.h>
#include <snzeromq.common/Models/SNZeroMQLoopback.h>
#include <snzeromq.common/Models/Entry/RequestConnection.h>
#include <snzeromq.common/Models/Entry/ResponseConnection.h>
#include <snzeromq.common/Helpers/JwtHelper.h>
#include <snzeromq.client/Features/SocketManager.h>

namespace synapse::network::cpp::zeromq::client::service::features
{
    class FeatureBase {
    public:
        /**
         * @brief Constructor for FeatureBase.
         * @param jwt_helper Shared pointer to the JwtHelper instance.
         * @param server_address Server address for connection.
         * @param frame_size Number of frames in the socket messages.
         * @param recv_timeout Timeout for receiving messages (ms).
         * @param send_timeout Timeout for sending messages (ms).
         */
        FeatureBase(
            std::shared_ptr<common::helpers::JwtHelper> jwt_helper,
            std::string server_address,
            const int frame_size,
            int recv_timeout = 1000,
            int send_timeout = 1000);

        /**
         * @brief Virtual destructor for FeatureBase.
         */
        virtual ~FeatureBase() = default;

        /**
         * @brief Move constructor.
         * @param other Another FeatureBase object to move from.
         */
        FeatureBase(FeatureBase&& other) noexcept;

        /**
         * @brief Move assignment operator.
         * @param other Another FeatureBase object to move from.
         * @return Reference to this object.
         */
        FeatureBase& operator=(FeatureBase&& other) noexcept;

        /**
         * @brief Deleted copy constructor.
         */
        FeatureBase(const FeatureBase& other) = delete;

        /**
         * @brief Deleted copy assignment operator.
         */
        FeatureBase& operator=(const FeatureBase& other) = delete;

    protected:
        virtual const char* GetNewToken();
        virtual std::optional<std::string> Initialize();
        virtual bool ClearMessage();

        virtual bool Loopback(
            const common::models::SNZeroMQLoopback& request,
            common::models::SNZeroMQLoopback& response) = 0;

    protected:
        bool _is_init;
        int _recv_timeout;
        int _send_timeout;
        int _frame_size;

        std::optional<std::string> _client_token;
        std::optional<std::string> _server_address;

        SocketManager _socket_manager;
        std::shared_ptr<common::helpers::JwtHelper> _jwt_helper;
    };
}
