#pragma once
#include <zmq.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <optional>
#include <snzeromq.common/Models/Enums/FeatureType.h>

namespace synapse::network::cpp::zeromq::client::service::features
{
    class SocketManager {
    public:
        SocketManager(int frame_size, int recv_timeout, int send_timeout);

        // 이동 생성자 및 이동 할당 연산자
        SocketManager(SocketManager&& other) noexcept;
        SocketManager& operator=(SocketManager&& other) noexcept;

        // 복사 금지
        SocketManager(const SocketManager& other) = delete;
        SocketManager& operator=(const SocketManager& other) = delete;

        bool SocketConnect(std::string server_address);
        std::optional<std::vector<zmq::message_t>> VerifyReceive() const;
        std::optional<bool> Send(std::vector<zmq::message_t>& send_message) const;
        bool ClearMessage();

    private:
        int _frame_size;
        int _recv_timeout;
        int _send_timeout;
        zmq::context_t _context;
        std::unique_ptr<zmq::socket_t> _socket;
    };
}
