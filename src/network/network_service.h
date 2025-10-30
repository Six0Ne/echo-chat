// NetWorkService 类是一个基础网络服务类，
// 提供了事件分发、连接管理、协议处理等功能。
// 子类可以继承并扩展其功能。

#pragma once

#include "socket_wrapper.h"
#include "event_dispatcher.h"
#include "connection_manager.h"
#include "protocol_handler.h"

class NetWorkService {
public:
    NetWorkService();
    virtual ~NetWorkService();

    // 启动服务
    virtual bool Start(const std::string& ip, int port);

    // 停止服务
    virtual void Stop();

    // 发送消息到指定连接
    virtual void SendToConnection(int fd, uint16_t msg_type, const std::string& payload);

    // 广播消息
    virtual void BroadcastMessage(uint16_t msg_type, const std::string& payload);

protected:
    // 子类可以选择重写
    virtual void OnConnectionEstablished(std::shared_ptr<Connection> conn);

    virtual void OnConnectionClosed(std::shared_ptr<Connection> conn);

    virtual void OnMessageReceived(std::shared_ptr<Connection> conn, uint16_t msg_type, const std::string& payload);

    virtual void OnError(const std::string& error_msg);

    // 组件
    std::unique_ptr<EventDispatcher> event_dispatcher_;
    std::unique_ptr<ConnectionManager> connection_manager_;
    std::unique_ptr<ProtocolHandler> protocol_handler_;

    int server_fd_;
    bool running_;

private:
    void SetEventHandlers();
    void HandleNewConnection();
    void HandleConnectionEvent(const EventDispatcher::EventContext& ctx);
};