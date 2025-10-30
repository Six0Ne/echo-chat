#pragma once

#include <iostream>
#include <cstring>
#include <system_error>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class SocketWrapper {
public:
    static int CreateSocket(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0) {
        int fd = socket(domain, type, protocol);
        if (fd < 0) {
            throw std::system_error(errno, std::system_category(), "Socket creation failed");
        }
        return fd;
    }

    static void BindSocket(int socket_fd, const std::string& ip, int port) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);

        if (bind(socket_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
            throw std::system_error(errno, std::system_category(), "Socket bind failed");
        }
    }

    static void ListenSocket(int socket_fd, int backlog = SOMAXCONN) {
        if (listen(socket_fd, backlog) < 0) {
            throw std::system_error(errno, std::system_category(), "Socket listen failed");
        }
    }

    static int AcceptConnection(int socket_fd) {
        int client_socket = accept(socket_fd, nullptr, nullptr);

        if (client_socket < 0) {
            throw std::system_error(errno, std::system_category(), "Socket accept failed");
        }

        return client_socket;
    }

    static void ConnectSocket(int socket_fd, const std::string& ip, int port) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);

        if (connect(socket_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
            throw std::system_error(errno, std::system_category(), "Socket connect failed");
        }
    }
    // 获取远程连接地址（用于已连接的socket）
    static void GetPeerAddress(int socket_fd, std::string& peer_ip, int& peer_port) {
        struct sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);

        if (getpeername(socket_fd, (struct sockaddr*) &peer_addr, &peer_addr_len) < 0) {
            throw std::system_error(errno, std::system_category(), "Get peer address failed");
        }

        peer_ip = inet_ntoa(peer_addr.sin_addr);
        peer_port = ntohs(peer_addr.sin_port);
    }

    // 获取本地地址
    static void GetLocalAddress(int socket_fd, std::string& local_ip, int& local_port) {
        struct sockaddr_in local_addr;
        socklen_t local_addr_len = sizeof(local_addr);

        if (getsockname(socket_fd, (struct sockaddr*) &local_addr, &local_addr_len) < 0) {
            throw std::system_error(errno, std::system_category(), "Get local address failed");
        }

        local_ip = inet_ntoa(local_addr.sin_addr);
        local_port = ntohs(local_addr.sin_port);
    }

    // 设置Socket 为非阻塞模式
    static void SetNonBlocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags < 0) {
            throw std::system_error(errno, std::system_category(), "Socket F_GETFL failed");
        }

        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
            throw std::system_error(errno, std::system_category(), "Socket F_SETFL failed");
        }
    }

    static void CloseSocket(int fd) {
        if (fd >= 0) {
            close(fd);
        }
    }
};