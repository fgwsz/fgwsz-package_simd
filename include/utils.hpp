#pragma once

#include <cstddef>   // size_t
#include <cstdint>   // uint8_t, uint64_t
#include <array>     // std::array
#include <string>    // std::string
#include <chrono>    // std::chrono

// ========================== 工具函数 ==========================
namespace utils {

using u8 = uint8_t;

using u64 = uint64_t;

// 仅用于小数据（路径、长度），非性能关键
void xor_range(u8* data, size_t len, u8 key);

[[nodiscard]] std::array<u8, 8> to_be64(u64 v);

[[nodiscard]] u64 from_be64(const u8* bytes);

[[nodiscard]] std::string format_size(u64 bytes);

struct Timer {

    using Clock = std::chrono::high_resolution_clock;

    Clock::time_point start;

    Timer();

    [[nodiscard]] double elapsed() const;

};

} // namespace utils
