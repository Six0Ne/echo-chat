#include "chat_server.h"
#include <iostream>

ChatServer::ChatServer() {
    InitializeUserDatabase();
    SetMessageHandlers();
}

void ChatServer::InitializeUserDatabase() {
    // 初始化用户数据库（简易）
    user_database_["root"] = "root_password";
    user_database_["UserA"] = "123";
    user_database_["UserB"] = "123";
    user_database_["UserC"] = "123";
    user_database_["UserD"] = "123";
    user_database_["UserE"] = "123";
}

void ChatServer::SetMessageHandlers() {
    // 注册消息处理器
    protocol_handler_->RegisterMessageHandler(MsgType::LOGIN,
                                              [this](auto conn, auto payload) { HandleLogin(conn, payload); });

    protocol_handler_->RegisterMessageHandler(MsgType::LOGOUT,
                                              [this](auto conn, auto payload) { HandleLogout(conn, payload); });

    protocol_handler_->RegisterMessageHandler(MsgType::PRIVATE_MSG,
                                              [this](auto conn, auto payload) { HandlePrivateMessage(conn, payload); });
    protocol_handler_->RegisterMessageHandler(MsgType::GROUP_MSG,
                                              [this](auto conn, auto payload) { HandleGroupMessage(conn, payload); });

    protocol_handler_->RegisterMessageHandler(
            MsgType::USER_LIST, [this](auto conn, auto payload) { HandleUserListRequest(conn, payload); });
}

void ChatServer::OnConnectionEstablished(std::shared_ptr<Connection> conn) {
    std::cout << "New connection established." << "\n";
}

void ChatServer::OnConnectionClosed(std::shared_ptr<Connection> conn) {
    for (auto it = online_users_.begin(); it != online_users_.end(); ++it) {
        if (it->second == conn) {
            std::cout << it->first << ": User logged out due to connection close." << "\n";
            online_users_.erase(it);
            break;
        }
    }
}

void ChatServer::HandleLogin(std::shared_ptr<Connection> conn, const std::string& payload) {
    auto delimiter_pos = payload.find(':');
    if (delimiter_pos == std::string::npos) {
        SendToConnection(conn->GetFd(), MsgType::LOGIN, "Invalid login format.");
        std::cout << "Invalid login format received." << "\n";
        // conn->Send("Invalid login format.");
        return;
    }

    std::string username = payload.substr(0, delimiter_pos);
    std::string password = payload.substr(delimiter_pos + 1);

    if (user_database_.count(username) && user_database_[username] == password) {
        online_users_[username] = conn;
        socket_users_[conn->GetFd()] = username;
        SendToConnection(conn->GetFd(), MsgType::LOGIN, "SUCCESS");
        std::cout << username << ": User logged in successfully." << "\n";
        // conn->Send("Login successful.");
    } else {
        SendToConnection(conn->GetFd(), MsgType::LOGIN, "Invalid username or password.");
        std::cout << "Failed login attempt for user: " << username << "\n";
        // conn->Send("Invalid username or password.");
    }
}

void ChatServer::HandleLogout(std::shared_ptr<Connection> conn, const std::string& payload = "") {
    for (auto it = online_users_.begin(); it != online_users_.end(); ++it) {
        if (it->second == conn) {
            online_users_.erase(it);
            SendToConnection(conn->GetFd(), MsgType::LOGOUT, "Logout successful.");
            std::cout << it->first << ": User logged out successfully." << "\n";
            // conn->Send("Logout successful.");
            return;
        }
    }
    SendToConnection(conn->GetFd(), MsgType::LOGOUT, "You are not logged in.");
    std::cout << "Logout attempt from non-logged-in connection." << "\n";
    // conn->Send("You are not logged in.");
}

void ChatServer::HandlePrivateMessage(std::shared_ptr<Connection> conn, const std::string& payload) {
    auto delimiter_pos = payload.find(':');
    if (delimiter_pos == std::string::npos) {
        SendToConnection(conn->GetFd(), MsgType::PRIVATE_MSG, "Invalid private message format.");
        std::cout << "Invalid private message format received." << "\n";
        // conn->Send("Invalid private message format.");
        return;
    }

    std::string target_user = payload.substr(0, delimiter_pos);
    std::string message = payload.substr(delimiter_pos + 1);

    if (online_users_.count(target_user)) {
        SendToConnection(online_users_[target_user]->GetFd(),
                         MsgType::PRIVATE_MSG,
                         socket_users_[conn->GetFd()] + ": " + message);
        std::cout << "Private message sent to " << target_user << "." << "\n";
        // online_users_[target_user]->Send(message);
    } else {
        SendToConnection(conn->GetFd(), MsgType::PRIVATE_MSG, "Invalid private message format.");
        std::cout << "Private message target user not online: " << target_user << "." << "\n";
        // conn->Send("User not online.");
    }
}

void ChatServer::HandleGroupMessage(std::shared_ptr<Connection> conn, const std::string& payload) {
    for (const auto& [username, user_conn] : online_users_) {
        if (user_conn != conn) {
            SendToConnection(user_conn->GetFd(), MsgType::GROUP_MSG, payload);
            std::cout << "Group message sent to " << username << "->" << payload << "\n";
            // user_conn->Send(payload);
        }
    }
}

void ChatServer::HandleUserListRequest(std::shared_ptr<Connection> conn, const std::string& payload = "") {
    std::string user_list = "Online users:\n";
    for (const auto& [username, _] : online_users_) {
        user_list += username + "\n";
    }
    std::cout << "User list :" << user_list;
    SendToConnection(conn->GetFd(), MsgType::USER_LIST, user_list);
    // conn->Send(user_list);
}
