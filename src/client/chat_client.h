#pragma once
#include "../network/client_network_service.h"
#include <string>

class ChatClient : public ClientNetworkService {
public:
    ChatClient();
    ~ChatClient() = default;

    // bool ConnectToServer(const std::string& ip, int port);
    // void SendChatMessage(const std::string& message);

    bool Login(const std::string& username, const std::string& password);
    void Logout();
    void SendPrivateMessage(const std::string& target_user, const std::string& message);
    void SendGroupMessage(const std::string& message);
    void RequestUserList();

protected:
    void OnMessageReceived(std::shared_ptr<Connection> conn, uint16_t msg_type, const std::string& payload) override;
};