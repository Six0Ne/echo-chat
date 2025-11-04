#include "chat_client.h"
#include "menu.h"
#include <iostream>

ChatClient::ChatClient() {
    SetMessageHandlers();
}

void ChatClient::SetMessageHandlers() {
    protocol_handler_->RegisterMessageHandler(MsgType::LOGIN, [this](auto conn, auto payload) {
        if (payload == "SUCCESS") {
            logged_in_ = true;
            std::cout << "Login successful!" << "\n";
        } else {
            std::cout << "Login failed: " << payload << "\n";
        }
    });

    protocol_handler_->RegisterMessageHandler(MsgType::LOGOUT, [this](auto conn, auto payload) {
        logged_in_ = false;
        std::cout << "Logout: " << payload << "\n";
    });

    protocol_handler_->RegisterMessageHandler(
            MsgType::PRIVATE_MSG, [this](auto conn, auto payload) { std::cout << "[Private] " << payload << "\n"; });
    protocol_handler_->RegisterMessageHandler(
            MsgType::GROUP_MSG, [this](auto conn, auto payload) { std::cout << "[Group] " << payload << "\n"; });

    protocol_handler_->RegisterMessageHandler(
            MsgType::USER_LIST, [this](auto conn, auto payload) { std::cout << "Online users: " << payload << "\n"; });
}

bool ChatClient::Login(const std::string& username, const std::string& password) {
    if (!IsConnected()) {
        std::cerr << "Not connected to server" << "\n";
        return false;
    }

    username_ = username;
    std::string payload = username + ":" + password;
    SendMessage(MsgType::LOGIN, payload);
    std::cout << "Login request sent for user: " << username << "\n";
    return true;
}

void ChatClient::Logout() {
    if (logged_in_) {
        SendMessage(MsgType::LOGOUT, "");
        logged_in_ = false;
        std::cout << "Logout request sent" << "\n";
    }
}

void ChatClient::SendPrivateMessage(const std::string& target_user, const std::string& message) {
    if (!logged_in_) {
        std::cerr << "Please login first" << "\n";
        return;
    }
    std::string payload = target_user + ":" + message;
    SendMessage(MsgType::PRIVATE_MSG, payload);
}

void ChatClient::SendGroupMessage(const std::string& message) {
    if (!logged_in_) {
        std::cerr << "Please login first" << "\n";
        return;
    }
    SendMessage(MsgType::GROUP_MSG, message);
}

void ChatClient::RequestUserList() {
    if (!logged_in_) {
        std::cerr << "Please login first" << "\n";
        return;
    }
    SendMessage(MsgType::USER_LIST, "");
}

void ChatClient::OnConnectionEstablished(std::shared_ptr<Connection> conn) {
    std::cout << "Connected to server successfully!" << "\n";
}

void ChatClient::OnConnectionClosed(std::shared_ptr<Connection> conn) {
    std::cout << "Disconnected from server" << "\n";
    logged_in_ = false;
}

void ChatClient::HandleStandardInputEvent() {
    Menu::ShowMainMenu();
    int choice = Menu::GetUserChoice();

    switch (choice) {
    case Menu::LOGIN: {  // Login
        std::string username = Menu::GetUserInput("Enter username: ");
        std::string password = Menu::GetUserInput("Enter password: ");
        Login(username, password);
        break;
    }
    case Menu::LOGOUT: {  // Logout
        Logout();
        break;
    }
    case Menu::PRIVATE_MSG: {  // Send Private Message
        std::string target_user = Menu::GetUserInput("Enter target username: ");
        std::string message = Menu::GetUserInput("Enter message: ");
        SendPrivateMessage(target_user, message);
        break;
    }
    case Menu::GROUP_MSG: {  // Send Group Message
        std::string message = Menu::GetUserInput("Enter message: ");
        SendGroupMessage(message);
        break;
    }
    case Menu::USER_LIST: {  // View Online Users
        RequestUserList();
        break;
    }
    case Menu::EXIT: {  // Exit
        // running = false;
        Logout();
        break;
    }
    default:
        std::cerr << "Invalid choice. Please try again." << "\n";
    }
}