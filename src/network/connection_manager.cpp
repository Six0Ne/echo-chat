#include "connection_manager.h"

ConnectionManager::~ConnectionManager() {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.clear();
}

// 添加连接
void ConnectionManager::AddConnection(const ConnectionPtr& conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_[conn->GetFd()] = conn;
}

// 移除连接
void ConnectionManager::RemoveConnection(int fd) {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.erase(fd);
}

// 获取连接
ConnectionManager::ConnectionPtr ConnectionManager::GetConnection(int fd) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = connections_.find(fd);
    return (it != connections_.end()) ? it->second : nullptr;
}

// 获取所有连接
std::vector<ConnectionManager::ConnectionPtr> ConnectionManager::GetAllConnections() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ConnectionPtr> result;
    result.reserve(connections_.size());

    for (const auto& [fd, connection_ptr] : connections_) {
        result.push_back(connection_ptr);
    }
    return result;
}

// 获取连接数量
size_t ConnectionManager::GetConnectionCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connections_.size();
}

// 广播消息
void ConnectionManager::Broadcast(const std::string& data) {
    auto connections = GetAllConnections();  // 获取副本避免死锁
    for (const auto& conn : connections) {
        if (conn->GetState() == Connection::CONNECTED) {
            conn->Send(data);
        }
    }
}

void ConnectionManager::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.clear();
}