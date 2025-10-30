#include "chat_server.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // 检查参数数量
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP_ADDRESS> <PORT>" << "\n";
        std::cerr << "Example: " << argv[0] << " 127.0.0.1 8088" << "\n";
        return 1;
    }

    // 获取 IP 地址和端口
    char* ip_address = argv[1];
    int port = std::atoi(argv[2]);

    // 验证端口范围
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Port must be between 1 and 65535" << std::endl;
        return 1;
    }

    try {
        // 创建并运行服务器
        ChatServer server;
        if (!server.Start(ip_address, port)) {
            std::cerr << "Failed to start server." << std::endl;
            return 1;
        }

        std::cout << "Server is running on " << ip_address << ":" << port << std::endl;
        server.Run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}