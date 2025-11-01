cmake_minimum_required(VERSION 3.10)
project(EchoChat)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CLIENT
set(CHAT_CLIENT
    src/client/main.cpp
    src/client/chat_client.cpp
    src/client/menu.cpp
)

# SERVER
set(CHAT_SERVER
    src/server/main.cpp
    src/server/chat_server.cpp
)

# NETWORK
set(CHAT_NETWORK
    src/network/event_dispatcher.cpp
    src/network/connection.cpp
    src/network/connection_manager.cpp
    src/network/protocol.cpp
    src/network/protocol_handler.cpp
    src/network/network_service.cpp
    src/network/client_network_service.cpp
)

# 创建可执行文件
add_executable(chat_client ${CHAT_CLIENT} ${CHAT_NETWORK})
add_executable(chat_server ${CHAT_SERVER} ${CHAT_NETWORK})