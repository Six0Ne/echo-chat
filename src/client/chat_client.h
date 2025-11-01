#pragma once
#include "../network/client_network_service.h"
#include <string>

class ChatClient : public ClientNetworkService {
public:
    ChatClient();
    ~ChatClient() = default;

    void SetMessageHandlers();
    bool Login(const std::string& username, const std::string& password);
    void Logout();
    void SendPrivateMessage(const std::string& target_user, const std::string& message);
    void SendGroupMessage(const std::string& message);
    void RequestUserList();

protected:
    void OnConnectionEstablished(std::shared_ptr<Connection> conn) override;
    void OnConnectionClosed(std::shared_ptr<Connection> conn) override;
    void HandleStandardInputEvent() override;

private:
    bool logged_in_ = false;
    std::string username_;
};