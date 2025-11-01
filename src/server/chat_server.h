#pragma once

#include "../network/network_service.h"
#include <unordered_map>
#include <string>

class ChatServer : public NetWorkService {
public:
    ChatServer();
    ~ChatServer() = default;

protected:
    void InitializeUserDatabase();
    void SetMessageHandlers();
    void OnConnectionEstablished(std::shared_ptr<Connection> conn) override;
    void OnConnectionClosed(std::shared_ptr<Connection> conn) override;

private:
    void HandleLogin(std::shared_ptr<Connection> conn, const std::string& payload);
    void HandleLogout(std::shared_ptr<Connection> conn, const std::string& payload);
    void HandlePrivateMessage(std::shared_ptr<Connection> conn, const std::string& payload);
    void HandleGroupMessage(std::shared_ptr<Connection> conn, const std::string& payload);
    void HandleUserListRequest(std::shared_ptr<Connection> conn, const std::string& payload);

    std::unordered_map<std::string, std::string> user_database_;                 // 用户名 -> 密码
    std::unordered_map<std::string, std::shared_ptr<Connection>> online_users_;  // 用户名 -> 连接
    std::unordered_map<int, std::string> socket_users_;                          // socket -> 用户名
};