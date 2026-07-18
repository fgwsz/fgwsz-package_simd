#pragma once

#include <cstddef>   // size_t
#include <cstdint>   // uint8_t, uint64_t
#include <array>     // std::array
#include <string>    // std::string
#include <sstream>   // std::ostringstream
#include <iomanip>   // std::fixed, std::setprecision
#include <chrono>    // std::chrono

// ========================== 工具函数 ==========================
namespace utils {
using u8 = uint8_t;
using u64 = uint64_t;

// 仅用于小数据（路径、长度），非性能关键
inline void xor_range(u8* data, size_t len, u8 key) {
    for (size_t i = 0; i < len; ++i) data[i] ^= key;
}

[[nodiscard]] inline std::array<u8, 8> to_be64(u64 v) {
    std::array<u8, 8> bytes;
    for (int i = 7; i >= 0; --i) { bytes[i] = static_cast<u8>(v & 0xFF); v >>= 8; }
    return bytes;
}

[[nodiscard]] inline u64 from_be64(const u8* bytes) {
    u64 v = 0;
    for (int i = 0; i < 8; ++i) v = (v << 8) | bytes[i];
    return v;
}

[[nodiscard]] inline std::string format_size(u64 bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int idx = 0;
    double val = static_cast<double>(bytes);
    while (val >= 1024.0 && idx < 4) { val /= 1024.0; ++idx; }
    std::ostringstream oss;
    if (idx == 0) oss << static_cast<u64>(val) << " " << units[idx];
    else oss << std::fixed << std::setprecision(2) << val << " " << units[idx];
    return oss.str();
}

struct Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start;
    Timer() : start(Clock::now()) {}
    [[nodiscard]] double elapsed() const {
        return std::chrono::duration<double>(Clock::now() - start).count();
    }
};

} // namespace utils
