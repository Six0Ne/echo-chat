#include "protocol.h"
#include <arpa/inet.h>
#include <cstring>
#include <chrono>
#include <atomic>

// 编码消息
std::string ProtocolDecoder::EncodeMessage(uint16_t mes_type, const std::string& payload) {
    // 转化成网络字节序 注意数据类型不一样使用的转换函数不一样
    MessageHeader header;
    header.msg_type = htons(mes_type);
    header.msg_len = htons(static_cast<uint16_t>(payload.size()));
    header.sequence = htonl(GenerateSequence());
    header.timestamp = htonl(GetCurrentTimestamp());

    std::string result;
    result.resize(MessageHeader::kSize + payload.size());

    // 拷贝头部
    memcpy(&result[0], &header, MessageHeader::kSize);

    // 拷贝载荷
    if (!payload.empty()) {
        memcpy(&result[MessageHeader::kSize], payload.data(), payload.size());
    }

    return result;
}

// 解码消息
bool ProtocolDecoder::DecodeMessage(const std::string& data, MessageHeader& header, std::string& payload) {
    if (data.size() < MessageHeader::kSize) {  // 需要解码的数据长度小于头部长度
        return false;
    }

    memcpy(&header, &data, MessageHeader::kSize);

    // 将网络传输字节序 转化从本机字节序
    header.msg_type = ntohs(header.msg_type);
    header.msg_len = ntohs(header.msg_len);
    header.sequence = ntohl(header.sequence);
    header.timestamp = ntohl(header.timestamp);

    // 提取载荷
    if (data.size() < MessageHeader::kSize + header.msg_len) {
        return false;
    }

    payload.assign(data.data() + MessageHeader::kSize, header.msg_len);

    return true;
}

// 尝试从缓存区中解析完整消息
bool ProtocolDecoder::TryParseMessage(std::string& buffer, MessageHeader& header, std::string& payload) {
    if (buffer.size() < MessageHeader::kSize) {  // 需要解码的数据长度小于头部长度
        return false;
    }

    memcpy(&header, &buffer, MessageHeader::kSize);

    // 将网络传输字节序 转化从本机字节序
    header.msg_len = ntohs(header.msg_len);

    std::size_t total_length = MessageHeader::kSize + header.msg_len;

    if (buffer.size() < total_length) {
        return false;  // 数据不完整
    }

    // 解析完整消息
    if (DecodeMessage(std::string(buffer.data(), total_length), header, payload)) {
        buffer.erase(0, total_length);
        return true;
    }

    return false;
}

uint32_t ProtocolDecoder::GenerateSequence() {
    static std::atomic<uint32_t> sequence{0};
    return ++sequence;
}

uint32_t ProtocolDecoder::GetCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}