#include "event_dispatcher.h"
#include <system_error>
#include <unistd.h>
#include <iostream>
#include "connection.h"

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
bool EventDispatcher::AddEvent(int fd, EventType events) {
    epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
}

// 修改事件
bool EventDispatcher::ModifyEvent(int fd, EventType events) {
    epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
}

// 删除事件
bool EventDispatcher::RemoveEvent(int fd) {
    return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) == 0;
}

// 时间循环
void EventDispatcher::RunEventLoop(int timeout_ms) {
    // std::cout << "EventDispatcher: Starting event loop." << "\n";
    running_ = true;

    while (running_) {
        int num_events = epoll_wait(epoll_fd_, events_.data(), events_.size(), timeout_ms);
        // std::cout << "EventDispatcher: epoll_wait returned " << num_events << " events." << "\n";
        if (num_events < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::system_category(), "Epoll wait failed");
        }

        for (int i = 0; i < num_events; i++) {
            EventContext ctx;
            ctx.events = static_cast<EventType>(events_[i].events);
            ctx.fd = events_[i].data.fd;
            // std::cout << "EventDispatcher: Handling event for fd " << events_[i].data.fd << " with events "
            //           << ctx.events << "\n";

            // 分发到对应的事件处理器
            for (const auto& [event_type, event_callback] : events_handler_) {
                // 如何触发的事件注册到了handler上
                if (events_[i].events & event_type) {  // 应当是检查 events_[i].events 是否包含 event_type
                    if (event_callback) {
                        event_callback(ctx);
                    }
                    // std::cout << "EventDispatcher: Dispatched event for fd " << ctx.fd << " to handler." <<
                    // "\n";
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