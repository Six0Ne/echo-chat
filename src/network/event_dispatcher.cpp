#include "event_dispatcher.h"
#include <system_error>
#include <unistd.h>
// #include <callback.h>

EventDispatcher::EventDispatcher() : running_(false) {
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ < 0) {
        throw std::system_error(errno, std::system_category(), "Epoll creation failed");
    }
    events_.resize(kMaxEvents);
}

EventDispatcher::~EventDispatcher() {
    Clear();
}

// 添加事件监听
bool EventDispatcher::AddEvent(int fd, EventType events, void* user_data) {
    epoll_event ev;
    ev.events = events;
    ev.data.ptr = user_data;

    return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
}

// 修改事件
bool EventDispatcher::ModifyEvent(int fd, EventType events, void* user_data) {
    epoll_event ev;
    ev.events = events;
    ev.data.ptr = user_data;

    return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
}

// 删除事件
bool EventDispatcher::RemoveEvent(int fd) {
    return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) == 0;
}

// 时间循环
void EventDispatcher::RunEventLoop(int timeout_ms) {
    running_ = true;

    while (running_) {
        int num_events = epoll_wait(epoll_fd_, events_.data(), events_.size(), timeout_ms);

        if (num_events < 0) {
            if (num_events == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::system_category(), "Epoll wait failed");
        }

        for (int i = 0; i < num_events; i++) {
            EventContext ctx;
            ctx.fd = events_[i].data.fd;
            ctx.events = static_cast<EventType>(events_[i].events);
            ctx.user_data = events_[i].data.ptr;

            // 分发到对应的事件处理器
            for (const auto& [event_ytpe, event_callback] : events_handler_) {
                // 如何触发的事件注册到了handler上
                if (event_ytpe & events_[i].events) {
                    event_callback(ctx);
                }
            }
        }

        if (num_events == static_cast<int>(events_.size())) {
            events_.resize(2 * events_.size());
        }
    }
}

// 停止事件循环
void EventDispatcher::StopEventLoop() {
    running_ = false;
}

// 注册事件处理器
void EventDispatcher::RegisterEventHandler(EventType event_type, EventCallback callback) {
    events_handler_[event_type] = callback;
}

void EventDispatcher::Clear() {
    StopEventLoop();
    if (epoll_fd_ >= 0) {
        close(epoll_fd_);
        epoll_fd_ = -1;
    }
    events_.clear();
    events_handler_.clear();
}