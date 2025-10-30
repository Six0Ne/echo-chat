#include "chat_server.h"
#include <iostream>

ChatServer::ChatServer() {
    // 初始化用户数据库（示例）
    user_database_["root"] = "root_password";
}

void ChatServer::OnConnectionEstablished(std::shared_ptr<Connection> conn) {
    std::cout << "New connection established." << std::endl;
}

void ChatServer::OnConnectionClosed(std::shared_ptr<Connection> conn) {
    for (auto it = online_users_.begin(); it != online_users_.end(); ++it) {
        if (it->second == conn) {
            online_users_.erase(it);
            break;
        }
    }
    std::cout << "Connection closed." << std::endl;
}

void ChatServer::OnMessageReceived(std::shared_ptr<Connection> conn, uint16_t msg_type, const std::string& payload) {
    switch (msg_type) {
    case MsgType::LOGIN:
        HandleLogin(conn, payload);
        break;
    case MsgType::LOGOUT:
        HandleLogout(conn);
        break;
    case MsgType::PRIVATE_MSG:
        HandlePrivateMessage(conn, payload);
        break;
    case MsgType::GROUP_MSG:
        HandleGroupMessage(conn, payload);
        break;
    case MsgType::USER_LIST:
        HandleUserListRequest(conn);
        break;
    default:
        std::cerr << "Unknown message type received." << std::endl;
    }
}

void ChatServer::HandleLogin(std::shared_ptr<Connection> conn, const std::string& payload) {
    auto delimiter_pos = payload.find(':');
    if (delimiter_pos == std::string::npos) {
        conn->Send("Invalid login format.");
        return;
    }

    std::string username = payload.substr(0, delimiter_pos);
    std::string password = payload.substr(delimiter_pos + 1);

    if (user_database_.count(username) && user_database_[username] == password) {
        online_users_[username] = conn;
        conn->Send("Login successful.");
    } else {
        conn->Send("Invalid username or password.");
    }
}

void ChatServer::HandleLogout(std::shared_ptr<Connection> conn) {
    for (auto it = online_users_.begin(); it != online_users_.end(); ++it) {
        if (it->second == conn) {
            online_users_.erase(it);
            conn->Send("Logout successful.");
            return;
        }
    }
    conn->Send("You are not logged in.");
}

void ChatServer::HandlePrivateMessage(std::shared_ptr<Connection> conn, const std::string& payload) {
    auto delimiter_pos = payload.find(':');
    if (delimiter_pos == std::string::npos) {
        conn->Send("Invalid private message format.");
        return;
    }

    std::string target_user = payload.substr(0, delimiter_pos);
    std::string message = payload.substr(delimiter_pos + 1);

    if (online_users_.count(target_user)) {
        online_users_[target_user]->Send(message);
    } else {
        conn->Send("User not online.");
    }
}

void ChatServer::HandleGroupMessage(std::shared_ptr<Connection> conn, const std::string& payload) {
    for (const auto& [username, user_conn] : online_users_) {
        if (user_conn != conn) {
            user_conn->Send(payload);
        }
    }
}

void ChatServer::HandleUserListRequest(std::shared_ptr<Connection> conn) {
    std::string user_list = "Online users:\n";
    for (const auto& [username, _] : online_users_) {
        user_list += username + "\n";
    }
    conn->Send(user_list);
}

void ChatServer::Run() {
    std::cout << "Server is running. Waiting for connections..." << std::endl;

    // 启动事件循环
    event_dispatcher_->RunEventLoop();
}