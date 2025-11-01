// ClientNetworkService 的实现文件
// 提供客户端的核心功能，包括连接服务器、发送消息、断开连接等。

#include "client_network_service.h"
#include "socket_wrapper.h"
#include <iostream>

ClientNetworkService::ClientNetworkService() : server_connection_(nullptr) {}

ClientNetworkService::~ClientNetworkService() {
    Disconnect();
}

bool ClientNetworkService::Init(const std::string& ip, int port) {
    try {
        int client_fd = SocketWrapper::CreateSocket();
        std::cout << "Socket created with fd: " << client_fd << "\n";
        SocketWrapper::ConnectSocket(client_fd, ip, port);
        SocketWrapper::SetNonBlocking(client_fd);

        this->SetEventHandlers();

        server_connection_ = std::make_shared<Connection>(client_fd, ip);
        server_connection_->SetDataCallback(
                [this](auto conn, auto data) { protocol_handler_->HandleData(conn, data); });

        server_connection_->SetCloseCallback([this](auto conn) { OnConnectionClosed(conn); });

        server_connection_->SetErrorCallback([this](auto conn, auto error) { OnError("Connection error: " + error); });

        connection_manager_->AddConnection(server_connection_);
        event_dispatcher_->AddEvent(client_fd, EventDispatcher::READ);
        event_dispatcher_->AddEvent(STDIN_FILENO, EventDispatcher::READ);

        return true;
    } catch (const std::runtime_error& e) {
        std::cerr << "Connection failed: " << e.what() << "\n";
        return false;
    }
}

bool ClientNetworkService::IsConnected() const {
    if (server_connection_ && server_connection_->GetState() == Connection::CONNECTED) {
        return true;
    }
    return false;
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

void ClientNetworkService::OnConnectionEstablished(std::shared_ptr<Connection> conn) {
    std::cout << "Connection established with server." << "\n";
}

void ClientNetworkService::OnConnectionClosed(std::shared_ptr<Connection> conn) {
    this->Disconnect();
}

void ClientNetworkService::SetEventHandlers() {
    // 注册读取事件处理器
    event_dispatcher_->RegisterEventHandler(EventDispatcher::READ, [this](const EventDispatcher::EventContext& ctx) {
        if (ctx.fd == STDIN_FILENO) {
            this->HandleStandardInputEvent();
        } else {
            this->HandleConnectionEvent(ctx);
        }
    });

    // 注册关闭事件处理器
    event_dispatcher_->RegisterEventHandler(EventDispatcher::CLOSE, [this](const EventDispatcher::EventContext& ctx) {
        auto conn = connection_manager_->GetConnection(ctx.fd);
        if (conn) {
            OnConnectionClosed(conn);
            connection_manager_->RemoveConnection(ctx.fd);
        }
    });
}

void ClientNetworkService::HandleConnectionEvent(const EventDispatcher::EventContext& ctx) {
    auto conn = connection_manager_->GetConnection(ctx.fd);
    if (conn) {
        if (ctx.events & EventDispatcher::READ) {
            conn->OnDataAvailable();
        }
        if (ctx.events & EventDispatcher::WRITE) {
            conn->OnWriteAvailable();
        }
    }
}
