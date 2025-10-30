#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct MessageHeader {
    uint16_t msg_type;   // 消息分类
    uint16_t msg_len;    // 消息边界
    uint32_t sequence;   // 消息顺序
    uint32_t timestamp;  // 消息时效

    static const std::size_t kSize = 12;  // 2 + 2 + 4 + 4 = 12 byte
};

namespace MsgType {
// constexpr uint16_t UNDEFINED = 0;
constexpr uint16_t LOGIN = 1;
constexpr uint16_t LOGOUT = 2;
constexpr uint16_t PRIVATE_MSG = 3;
constexpr uint16_t GROUP_MSG = 4;
constexpr uint16_t USER_LIST = 5;
};  // namespace MegType

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
};