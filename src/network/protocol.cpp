#include "protocol.h"
#include <arpa/inet.h>
#include <cstring>
#include <chrono>
#include <atomic>
#include <stdexcept>
#include <limits>

std::string ProtocolDecoder::EncodeMessage(uint16_t msg_type, const std::string& payload) {
    // 检查载荷大小
    if (payload.size() > std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("Payload too large");
    }

    std::string result;
    result.resize(MessageHeader::kSize + payload.size());

    // 逐个字段写入，确保正确的字节序
    char* result_ptr = &result[0];

    uint16_t net_msg_type = htons(msg_type);
    memcpy(result_ptr, &net_msg_type, sizeof(net_msg_type));

    uint16_t net_msg_len = htons(static_cast<uint16_t>(payload.size()));
    memcpy(result_ptr + 2, &net_msg_len, sizeof(net_msg_len));

    uint32_t net_sequence = htonl(GenerateSequence());
    memcpy(result_ptr + 4, &net_sequence, sizeof(net_sequence));

    uint32_t net_timestamp = htonl(GetCurrentTimestamp());
    memcpy(result_ptr + 8, &net_timestamp, sizeof(net_timestamp));

    // 拷贝载荷
    if (!payload.empty()) {
        memcpy(result_ptr + MessageHeader::kSize, payload.data(), payload.size());
    }

    return result;
}

bool ProtocolDecoder::DecodeMessage(const std::string& data, MessageHeader& header, std::string& payload) {
    if (data.size() < MessageHeader::kSize) {
        return false;
    }

    // 安全地逐个字段拷贝，避免对齐问题
    const char* data_ptr = data.data();

    uint16_t raw_msg_type, raw_msg_len;
    uint32_t raw_sequence, raw_timestamp;

    memcpy(&raw_msg_type, data_ptr, sizeof(raw_msg_type));
    memcpy(&raw_msg_len, data_ptr + 2, sizeof(raw_msg_len));
    memcpy(&raw_sequence, data_ptr + 4, sizeof(raw_sequence));
    memcpy(&raw_timestamp, data_ptr + 8, sizeof(raw_timestamp));

    // 字节序转换
    header.msg_type = ntohs(raw_msg_type);
    header.msg_len = ntohs(raw_msg_len);
    header.sequence = ntohl(raw_sequence);
    header.timestamp = ntohl(raw_timestamp);

    // 检查消息长度合理性
    if (header.msg_len > MAX_PAYLOAD_SIZE) {
        return false;
    }

    // 检查数据完整性
    if (data.size() < MessageHeader::kSize + header.msg_len) {
        return false;
    }

    payload.assign(data.data() + MessageHeader::kSize, header.msg_len);
    return true;
}

bool ProtocolDecoder::TryParseMessage(std::string& buffer, MessageHeader& header, std::string& payload) {
    if (buffer.size() < MessageHeader::kSize) {
        return false;
    }

    // 先解析头部检查消息长度
    uint16_t raw_msg_len;
    memcpy(&raw_msg_len, buffer.data() + 2, sizeof(raw_msg_len));  // msg_len在偏移量2的位置
    uint16_t msg_len = ntohs(raw_msg_len);

    std::size_t total_length = MessageHeader::kSize + msg_len;

    if (buffer.size() < total_length) {
        return false;
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
    uint32_t current = sequence.fetch_add(1, std::memory_order_relaxed);
    // 避免0值，并在溢出时重置
    return (current == std::numeric_limits<uint32_t>::max()) ? 1 : current + 1;
}

uint32_t ProtocolDecoder::GetCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}