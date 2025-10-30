// NetWorkService 的实现文件
// 实现了基础网络服务的核心功能，包括事件分发、连接管理等。
// 子类可以通过重写虚函数扩展功能。

#include "network_service.h"

NetWorkService::NetWorkService() : server_fd_(-1), running_(false) {
    event_dispatcher_ = std::make_unique<EventDispatcher>();
    connection_manager_ = std::make_unique<ConnectionManager>();
    protocol_handler_ = std::make_unique<ProtocolHandler>();
}

NetWorkService::~NetWorkService() {
    Stop();
}

// 启动服务
bool NetWorkService::Start(const std::string& ip, int port) {
    try {
        server_fd_ = SocketWrapper::CreateSocket();
        SocketWrapper::SetNonBlocking(server_fd_);
        SocketWrapper::BindSocket(server_fd_, ip, port);
        SocketWrapper::ListenSocket(server_fd_);

        // 设置事件处理器
        SetEventHandlers();

        // 添加服务器Socket到事件分发器
        event_dispatcher_->AddEvent(server_fd_, EventDispatcher::EventType::READ);

        running_ = true;
        std::cout << "Network service started on " << ip << ":" << port << std::endl;

        // 启动事件循环（在单独的线程）
        event_dispatcher_->RunEventLoop();

        return true;
    } catch (const std::exception& e) {
        OnError(e.what());
        return false;
    }
}

// 停止服务
void NetWorkService::Stop() {
    running_ = false;
    if (event_dispatcher_) {
        event_dispatcher_->StopEventLoop();
    }

    if (server_fd_ > 0) {
        SocketWrapper::CloseSocket(server_fd_);
        server_fd_ = -1;
    }
}

// 发送消息到指定连接
void NetWorkService::SendToConnection(int fd, uint16_t msg_type, const std::string& payload) {
    auto conn = connection_manager_->GetConnection(fd);
    if (conn) {
        protocol_handler_->SendMessage(conn, msg_type, payload);
    }
}

// 广播消息
void NetWorkService::BroadcastMessage(uint16_t msg_type, const std::string& payload) {
    auto conns = connection_manager_->GetAllConnections();
    for (const auto& conn : conns) {
        if (conn) {
            protocol_handler_->SendMessage(conn, msg_type, payload);
        }
    }
}

void NetWorkService::SetEventHandlers() {
    // 注册读取事件处理器
    event_dispatcher_->RegisterEventHandler(EventDispatcher::READ, [this](const EventDispatcher::EventContext& ctx) {
        if (ctx.fd == server_fd_) {  // server_fd_ 可读，说明有新连接
            HandleNewConnection();
        } else {
            HandleConnectionEvent(ctx);
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

void NetWorkService::HandleNewConnection() {
    try {
        int client_fd = SocketWrapper::AcceptConnection(server_fd_);
        SocketWrapper::SetNonBlocking(client_fd);

        std::string remote_addr = "unknown";  // 实际应该从socket获取
        int remote_port = 0;                  // 实际应该从socket获取
        SocketWrapper::GetPeerAddress(client_fd, remote_addr, remote_port);
        auto conn = std::make_shared<Connection>(client_fd, remote_addr);

        // 设置连接的回调
        conn->SetDataCallback([this](auto conn, auto data) { protocol_handler_->HandleData(conn, data); });

        conn->SetCloseCallback([this](auto conn) {
            OnConnectionClosed(conn);
            connection_manager_->RemoveConnection(conn->GetFd());
        });

        conn->SetErrorCallback([this](auto conn, auto error) { OnError("Connection error: " + error); });

        // 添加到连接管理器
        connection_manager_->AddConnection(conn);

        // 添加到事件分发器
        event_dispatcher_->AddEvent(client_fd, EventDispatcher::READ);
        event_dispatcher_->AddEvent(client_fd, EventDispatcher::CLOSE);

        // 通知子类
        OnConnectionEstablished(conn);

    } catch (const std::exception& e) {
        OnError(std::string("Accept connection failed: ") + e.what());
    }
}

void NetWorkService::HandleConnectionEvent(const EventDispatcher::EventContext& ctx) {
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

// 空实现 子类可以选择重写
void NetWorkService::OnConnectionEstablished(std::shared_ptr<Connection> conn) {}

void NetWorkService::OnConnectionClosed(std::shared_ptr<Connection> conn) {}

void NetWorkService::OnMessageReceived(std::shared_ptr<Connection> conn,
                                       uint16_t msg_type,
                                       const std::string& payload) {}

void NetWorkService::OnError(const std::string& error_msg) {
    std::cerr << "NetworkService Error: " << error_msg << std::endl;
}