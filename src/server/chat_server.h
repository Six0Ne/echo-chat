#pragma once

#include "../network/network_service.h"
#include <unordered_map>
#include <string>

class ChatServer : public NetWorkService {
public:
    ChatServer();
    ~ChatServer() = default;

    void Run();  // 启动服务器的事件循环

protected:
    void OnConnectionEstablished(std::shared_ptr<Connection> conn) override;
    void OnConnectionClosed(std::shared_ptr<Connection> conn) override;
    void OnMessageReceived(std::shared_ptr<Connection> conn, uint16_t msg_type, const std::string& payload) override;

private:
    void HandleLogin(std::shared_ptr<Connection> conn, const std::string& payload);
    void HandleLogout(std::shared_ptr<Connection> conn);
    void HandlePrivateMessage(std::shared_ptr<Connection> conn, const std::string& payload);
    void HandleGroupMessage(std::shared_ptr<Connection> conn, const std::string& payload);
    void HandleUserListRequest(std::shared_ptr<Connection> conn);

    std::unordered_map<std::string, std::string> user_database_;                 // 用户名 -> 密码
    std::unordered_map<std::string, std::shared_ptr<Connection>> online_users_;  // 用户名 -> 连接
};