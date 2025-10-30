#include "chat_client.h"
#include <iostream>

ChatClient::ChatClient() {}

bool ChatClient::Login(const std::string& username, const std::string& password) {
    std::string payload = username + ":" + password;
    SendToConnection(server_fd_, MsgType::LOGIN, payload);
    return true;
}

void ChatClient::Logout() {
    SendToConnection(server_fd_, MsgType::LOGOUT, "");
}

void ChatClient::SendPrivateMessage(const std::string& target_user, const std::string& message) {
    std::string payload = target_user + ":" + message;
    SendToConnection(server_fd_, MsgType::PRIVATE_MSG, payload);
}

void ChatClient::SendGroupMessage(const std::string& message) {
    SendToConnection(server_fd_, MsgType::GROUP_MSG, message);
}

void ChatClient::RequestUserList() {
    SendToConnection(server_fd_, MsgType::USER_LIST, "");
}

void ChatClient::OnMessageReceived(std::shared_ptr<Connection> conn, uint16_t msg_type, const std::string& payload) {
    switch (msg_type) {
    case MsgType::LOGIN:
        std::cout << "Login response: " << payload << std::endl;
        break;
    case MsgType::LOGOUT:
        std::cout << "Logout response: " << payload << std::endl;
        break;
    case MsgType::PRIVATE_MSG:
        std::cout << "Private message: " << payload << std::endl;
        break;
    case MsgType::GROUP_MSG:
        std::cout << "Group message: " << payload << std::endl;
        break;
    case MsgType::USER_LIST:
        std::cout << "User list: " << payload << std::endl;
        break;
    default:
        std::cerr << "Unknown message type received." << std::endl;
    }
}