#pragma once
#include "network_service.h"
#include <functional>

// ClientNetworkService 类继承自 NetWorkService，
// 提供客户端特定的功能，包括连接服务器、发送消息、断开连接等。
// 支持设置连接状态和消息接收的回调函数。

class ClientNetworkService : public NetWorkService {
public:
    using ConnectionStatusCallback = std::function<void(bool connected, const std::string)>;
    using MessageReceivedCallback = std::function<void(uint16_t msg_type, const std::string& payload)>;

    ClientNetworkService();
    virtual ~ClientNetworkService();

    // 连接到服务器
    bool Connect(const std::string& ip, int port);

    // 发送消息
    void SendMessage(uint16_t msg_type, const std::string& payload);

    // 断开连接
    void Disconnect();

    // 设置回调
    void SetConnectionStatusCallback(ConnectionStatusCallback callback);
    void SetMessageReceivedCallback(MessageReceivedCallback callback);

protected:
    void OnConnectionEstablished(std::shared_ptr<Connection> conn) override;
    void OnConnectionClosed(std::shared_ptr<Connection> conn) override;
    void OnMessageReceived(std::shared_ptr<Connection> conn, uint16_t msg_type, const std::string& payload) override;

private:
    std::shared_ptr<Connection> server_connection_;
    ConnectionStatusCallback connection_status_callback_;
    MessageReceivedCallback message_received_callback_;
};
