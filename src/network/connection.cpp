#include "socket_wrapper.h"
#include "connection.h"
#include <unistd.h>
#include <system_error>

Connection::Connection(int fd, const std::string& remote_addr) : fd_(fd), state_(CONNECTED), remote_addr_(remote_addr) {
    // 设置socket 为非阻塞模式
    SocketWrapper::SetNonBlocking(fd);
}

Connection::~Connection() {
    Close();
}

void Connection::Send(const std::string& data) {
    if (state_ != CONNECTED) {
        if (error_callback_) {
            error_callback_(shared_from_this(), "Connection not connected");
        }
        return;
    }

    // 简单实现, 直接发送, 实际环境肯定会更复杂
    ssize_t sent = send(fd_, data.data(), data.size(), 0);
    if (sent < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
        if (error_callback_) {
            error_callback_(shared_from_this(), "Send failed with error");
        }
    } else if (sent < 0) {
        if (error_callback_) {
            error_callback_(shared_from_this(), "Connection not connected");
        }
    } else if (sent < static_cast<ssize_t>(data.size())) {
        // 部分发送,存储到缓存区中
        output_buffer_.append(data.substr(sent, data.size()));
    }
}

void Connection::Close() {
    if (state_ != CLOSING) {
        state_ = CLOSING;
        if (fd_ > 0) {
            SocketWrapper::CloseSocket(fd_);
            fd_ = -1;  // 防止重复关闭
        }

        if (close_callback_) {
            close_callback_(shared_from_this());
        }
    }
}

int Connection::GetFd() const {
    return fd_;
}

const std::string& Connection::GetRemoteAddress() const {
    return remote_addr_;
}

Connection::State Connection::GetState() const {
    return state_;
}

void Connection::SetDataCallback(DataCallback cb) {
    data_callback_ = cb;
}

void Connection::SetCloseCallback(CloseCallback cb) {
    close_callback_ = cb;
}

void Connection::SetErrorCallback(ErrorCallback cb) {
    error_callback_ = cb;
}

void Connection::OnDataAvailable() {
    char buffer[4096];
    ssize_t received = recv(fd_, buffer, sizeof(buffer) - 1, 0);

    if (received > 0) {
        buffer[received] = '\0';
        input_buffer_.append(buffer, received);

        // 如果有设置数据回调
        if (data_callback_) {
            data_callback_(shared_from_this(), input_buffer_);
            input_buffer_.clear();
        }
    } else if (received == 0) {
        Close();
    } else {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            OnError();
        }
    }
}

void Connection::OnWriteAvailable() {
    if (!output_buffer_.empty()) {
        ssize_t sent = send(fd_, output_buffer_.data(), output_buffer_.size(), 0);
        if (sent > 0) {
            output_buffer_.erase(0, sent);
        } else if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            OnError();
        }
    }
}

void Connection::OnError() {
    if (error_callback_) {
        error_callback_(shared_from_this(), "Socket error occurred");
    }
}

void Connection::Clear() {
    if (fd_ >= 0) {
        SocketWrapper::CloseSocket(fd_);
        fd_ = -1;
    }
    state_ = DISCONNECTED;
    input_buffer_.clear();
    output_buffer_.clear();
}