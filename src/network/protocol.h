#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct MessageHeader {
    uint16_t msg_type;   // 消息类型
    uint16_t msg_len;    // 消息长度
    uint32_t sequence;   // 消息序列号
    uint32_t timestamp;  // 消息时间戳

    static constexpr std::size_t kSize = 12;  // 2 + 2 + 4 + 4 = 12 bytes
};

namespace MsgType {
constexpr uint16_t LOGIN = 1;
constexpr uint16_t LOGOUT = 2;
constexpr uint16_t PRIVATE_MSG = 3;
constexpr uint16_t GROUP_MSG = 4;
constexpr uint16_t USER_LIST = 5;
};  // namespace MsgType

class ProtocolDecoder {
public:
    // 编码消息
    static std::string EncodeMessage(uint16_t msg_type, const std::string& payload);

    // 解码消息
    static bool DecodeMessage(const std::string& data, MessageHeader& header, std::string& payload);

    // 尝试从缓存区中解析完整消息
    static bool TryParseMessage(std::string& buffer, MessageHeader& header, std::string& payload);

private:
    static uint32_t GenerateSequence();
    static uint32_t GetCurrentTimestamp();

    // 最大载荷大小限制 (10MB)
    static constexpr size_t MAX_PAYLOAD_SIZE = 10 * 1024 * 1024;
};