#pragma once

#include "connection.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>

class ConnectionManager {
public:
    using ConnectionPtr = std::shared_ptr<Connection>;

    ConnectionManager() = default;
    ~ConnectionManager();

    // 添加连接
    void AddConnection(const ConnectionPtr& conn);

    // 移除连接
    void RemoveConnection(int fd);

    // 获取连接
    ConnectionPtr GetConnection(int fd);

    // 获取所有连接
    std::vector<ConnectionPtr> GetAllConnections();

    // 获取连接数量
    size_t GetConnectionCount() const;

    // 广播消息
    void Broadcast(const std::string& data);

    // 清理资源
    void Clear();

private:
    std::unordered_map<int, ConnectionPtr> connections_;
    mutable std::mutex mutex_;
};