#pragma once
#include "network_service.h"
#include <functional>

// ClientNetworkService 类继承自 NetWorkService，
// 提供客户端特定的功能，包括连接服务器、发送消息、断开连接等。
// 支持设置连接状态和消息接收的回调函数。

class ClientNetworkService : public NetWorkService {
public:
    ClientNetworkService();
    virtual ~ClientNetworkService();

    // 连接到服务器
    bool Init(const std::string& ip, int port) override;

    // 连接状态检查
    bool IsConnected() const;

    // 发送消息
    void SendMessage(uint16_t msg_type, const std::string& payload);

    // 断开连接
    void Disconnect();

protected:
    void OnConnectionEstablished(std::shared_ptr<Connection> conn) override;
    void OnConnectionClosed(std::shared_ptr<Connection> conn) override;

    virtual void HandleStandardInputEvent() = 0;

private:
    std::shared_ptr<Connection> server_connection_;

private:
    void SetEventHandlers();

    void HandleConnectionEvent(const EventDispatcher::EventContext& ctx);
};
