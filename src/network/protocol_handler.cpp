#include "protocol_handler.h"
#include <iostream>

// 处理接收到的数据
void ProtocolHandler::HandleData(std::shared_ptr<Connection> conn, const std::string& data) {
    static std::string buffer;
    buffer.append(data);
    MessageHeader header;
    std::string payload;

    // 尝试从缓存区解析所有完整消息
    while (ProtocolDecoder::TryParseMessage(buffer, header, payload)) {
        // 查看该消息是否有注册， 是否要执行对应的回调函数
        auto it = message_handler_.find(header.msg_type);
        if (it != message_handler_.end()) {
            it->second(conn, header, payload);
        } else {
            std::cerr << "Unknown message type: " << header.msg_type << "\n";
        }
    }
}

// 发送消息
void ProtocolHandler::SendMessage(std::shared_ptr<Connection> conn,
                                  const uint16_t& msg_type,
                                  const std::string& payload) {
    std::string buffer = ProtocolDecoder::EncodeMessage(msg_type, payload);
    conn->Send(buffer);
}

// 注册消息处理器
void ProtocolHandler::RegisterMessageHandler(uint16_t msg_type, MessageHandler handler) {
    message_handler_[msg_type] = handler;
}
