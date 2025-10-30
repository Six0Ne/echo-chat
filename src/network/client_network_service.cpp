// ClientNetworkService 的实现文件
// 提供客户端的核心功能，包括连接服务器、发送消息、断开连接等。

#include "client_network_service.h"
#include "socket_wrapper.h"
#include <iostream>

ClientNetworkService::ClientNetworkService() : server_connection_(nullptr) {}

ClientNetworkService::~ClientNetworkService() {
    Disconnect();
}

bool ClientNetworkService::Connect(const std::string& ip, int port) {
    std::cout << ip << "\n";
    std::cout << port << "\n";
    try {
        int client_fd = SocketWrapper::CreateSocket();
        SocketWrapper::SetNonBlocking(client_fd);
        SocketWrapper::ConnectSocket(client_fd, ip, port);

        server_connection_ = std::make_shared<Connection>(client_fd, ip);
        server_connection_->SetDataCallback(
                [this](auto conn, auto data) { protocol_handler_->HandleData(conn, data); });

        server_connection_->SetCloseCallback([this](auto conn) { OnConnectionClosed(conn); });

        server_connection_->SetErrorCallback([this](auto conn, auto error) { OnError("Connection error: " + error); });

        connection_manager_->AddConnection(server_connection_);
        event_dispatcher_->AddEvent(client_fd, EventDispatcher::READ);

        if (connection_status_callback_) {
            connection_status_callback_(true, "Connected to server");
        }

        return true;
    } catch (const std::exception& e) {
        if (connection_status_callback_) {
            connection_status_callback_(false, e.what());
            std::cerr << "Connection failed: " << e.what() << std::endl;
        }
        return false;
    }
}

void ClientNetworkService::SendMessage(uint16_t msg_type, const std::string& payload) {
    if (server_connection_) {
        protocol_handler_->SendMessage(server_connection_, msg_type, payload);
    }
}

void ClientNetworkService::Disconnect() {
    if (server_connection_) {
        connection_manager_->RemoveConnection(server_connection_->GetFd());
        server_connection_->Close();
        server_connection_ = nullptr;
    }
}

void ClientNetworkService::SetConnectionStatusCallback(ConnectionStatusCallback callback) {
    connection_status_callback_ = callback;
}

void ClientNetworkService::SetMessageReceivedCallback(MessageReceivedCallback callback) {
    message_received_callback_ = callback;
}

void ClientNetworkService::OnConnectionEstablished(std::shared_ptr<Connection> conn) {
    std::cout << "Connection established with server." << std::endl;
}

void ClientNetworkService::OnConnectionClosed(std::shared_ptr<Connection> conn) {
    std::cout << "Connection closed." << std::endl;
    if (connection_status_callback_) {
        connection_status_callback_(false, "Connection closed by server");
    }
}

void ClientNetworkService::OnMessageReceived(std::shared_ptr<Connection> conn,
                                             uint16_t msg_type,
                                             const std::string& payload) {
    if (message_received_callback_) {
        message_received_callback_(msg_type, payload);
    }
}
