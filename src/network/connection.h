#pragma once

#include <memory>
#include <string>
#include <functional>

class Connection : public std::enable_shared_from_this<Connection> {
public:
    // clang-format off
    enum State {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        CLOSING
    };
    // clang-format on

    using DataCallback = std::function<void(std::shared_ptr<Connection>, const std::string&)>;
    using CloseCallback = std::function<void(std::shared_ptr<Connection>)>;
    using ErrorCallback = std::function<void(std::shared_ptr<Connection>, const std::string&)>;

    Connection(int fd, const std::string& remote_addr = "");
    ~Connection();

    // 发送数据
    void Send(const std::string& data);

    // 关闭连接
    void Close();

    // 获取连接信息
    int GetFd() const;
    const std::string& GetRemoteAddress() const;
    State GetState() const;

    // 设置回调函数, 提供给上层设置
    void SetDataCallback(DataCallback cb);
    void SetCloseCallback(CloseCallback cb);
    void SetErrorCallback(ErrorCallback cb);

    // 事件循环触发
    void OnDataAvailable();
    void OnWriteAvailable();
    void OnError();

private:
    int fd_;
    State state_;
    std::string remote_addr_;
    std::string input_buffer_;
    std::string output_buffer_;

    DataCallback data_callback_;
    CloseCallback close_callback_;
    ErrorCallback error_callback_;
};