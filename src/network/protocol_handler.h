#pragma once

#include "protocol.h"
#include "connection.h"
#include <functional>
#include <unordered_map>

class ProtocolHandler {
public:
    using MessageHandler = std::function<void(std::shared_ptr<Connection>, const MessageHeader&, const std::string&)>;

    ProtocolHandler() = default;
    ~ProtocolHandler() = default;
    // 处理接收到的数据
    void HandleData(std::shared_ptr<Connection> conn, const std::string& data);

    // 发送消息
    void SendMessage(std::shared_ptr<Connection> conn, const uint16_t& msg_type, const std::string& payload);

    // 注册消息处理器
    void RegisterMessageHandler(uint16_t msg_type, MessageHandler handler);

private:
    std::unordered_map<uint16_t, MessageHandler> message_handler_;
};