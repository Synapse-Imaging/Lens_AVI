#pragma once

#ifdef SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_EXPORTS
#define SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API __declspec(dllexport)
#else
#define SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API __declspec(dllimport)
#endif

#include <cppzmq/zmq.hpp>
#include <optional>
#include <vector>
#include <list>
#include <utility>
#include <cstring>
#include <stdexcept>
#include <memory>

#include <snzeromq.common/Models/Enums/MessageType.h>
#include <snzeromq.common/Models/Enums/FeatureType.h>

#include <snzeromq.common/Data/PointListCollection.h>

#include <snzeromq.common/Models/SNZeroMQLoopback.h>
#include <snzeromq.common/Models/Entry/ConnectionInfo.h>
#include <snzeromq.common/Models/Entry/RequestConnection.h>
#include <snzeromq.common/Models/Entry/ResponseConnection.h>
#include <snzeromq.common/Models/SAM2/RequestSam2.h>
#include <snzeromq.common/Models/SAM2/ResponseSam2.h>

#include <snzeromq.common/Helpers/JwtHelper.h>
#include <snzeromq.common/Helpers/TimeHelper.h>

namespace synapse::network::cpp::zeromq::client::service
{
    class ZeroMQClient
    {
    public:
        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ZeroMQClient();
        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ZeroMQClient(const char* server_address);
        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ~ZeroMQClient();

        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ZeroMQClient(const ZeroMQClient&) = delete;
        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API ZeroMQClient& operator=(const ZeroMQClient&) = delete;

        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API bool IsConnect() const noexcept;
        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API bool IsReadySAM2() const noexcept;

        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API const char* Init(const char* server_address, int recv_timeout = 1000, int send_timeout = 1000);

        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API bool TryLoopback(
            const common::enums::FeatureType featureType,
            const common::models::SNZeroMQLoopback& send_loopback,
            common::models::SNZeroMQLoopback& receive_loopback);

        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API bool TrySendRequestConnection(
            const common::models::entry::RequestConnection& request_connection,
            common::models::entry::ResponseConnection& response_connection);

        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API bool TryBindFeature(const common::models::entry::ConnectionInfo& connection_info, int recv_timeout = 1000, int send_timeout = 1000);

        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API bool TrySendSAM2ReRequest(
            const common::models::sam2::RequestSam2& request_sam2,
            const std::vector<uint8_t>& image_data,
            common::models::sam2::ResponseSam2& response_sam2,
            common::data::PointListCollection& response_sam2_data);

        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API bool TrySendSAM2ReRequest(
            const common::models::sam2::RequestSam2& request_sam2,
            const uint8_t* image_data,
            size_t image_data_size,
            common::models::sam2::ResponseSam2& response_sam2,
            uint8_t*& response_sam2_data,
            size_t& response_sam2_data_size);

        SYNAPSE_NETWORK_CPP_ZEROMQ_CLIENT_API const char* GetID() const noexcept;

    private:
        void FreeMemory(); // 메모리 해제

        bool SendData(zmq::socket_t& socket, zmq::message_t& frame_1, zmq::message_t& frame_3, zmq::message_t& frame_4, zmq::message_t& frame_5);
        bool SendData(zmq::socket_t& socket, zmq::message_t& frame_1, zmq::message_t& frame_3, zmq::message_t& frame_4);

        const char* ReceiveData(const common::enums::FeatureType& featureType, std::vector<uint8_t>& receive_data);
        const char* ReceiveData4(zmq::socket_t& socket);
        const char* ReceiveData5(zmq::socket_t& socket, std::vector<uint8_t>& receive_data);

        bool TryFindAddress(const char* info, char* find_info);

        std::vector<uint8_t> MessageToVector(const zmq::message_t& message);

        void ClearMessageQueue(zmq::socket_t& socket);

    private:
        char* client_token_ = nullptr;
        char* server_address_ = nullptr;
        char* connection_managed_server_address_ = nullptr;
        char* sam2_managed_server_address_ = nullptr;

        bool is_init_ = false;
        bool is_ready_sam2 = false;

        zmq::context_t context_;
        zmq::socket_t connection_socket_;
        zmq::socket_t sam2_socket_;
        std::shared_ptr<common::helpers::JwtHelper> jwt_helper_;
    };
}