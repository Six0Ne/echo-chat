#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <sys/epoll.h>

class EventDispatcher {
public:
    // clang-format off
    enum EventType { 
        READ = EPOLLIN, 
        WRITE = EPOLLOUT, 
        CLOSE = EPOLLRDHUP, 
        ERROR = EPOLLERR 
    };
    // clang-format on

    struct EventContext {
        int fd;
        EventType events;
    };

    using EventCallback = std::function<void(const EventContext&)>;

    EventDispatcher();
    ~EventDispatcher();

    // 添加事件监听
    bool AddEvent(int fd, EventType events);

    // 修改事件
    bool ModifyEvent(int fd, EventType events);

    // 删除事件
    bool RemoveEvent(int fd);

    // 时间循环
    void RunEventLoop(int timeout_ms = -1);

    // 停止事件循环
    void StopEventLoop();

    // 注册事件处理器
    void RegisterEventHandler(EventType event_type, EventCallback callback);

    // 清理资源
    void Clear();

private:
    int epoll_fd_;
    bool running_;
    std::unordered_map<EventType, EventCallback> events_handler_;
    std::vector<epoll_event> events_;
    static const int kMaxEvents = 64;
};