#include <cstddef>   // size_t
#include <cstdint>   // uint8_t, uint64_t
#include <array>     // std::array
#include <string>    // std::string
#include <sstream>   // std::ostringstream
#include <iomanip>   // std::fixed, std::setprecision
#include <chrono>    // std::chrono

#include "utils.hpp"

namespace utils {

void xor_range(u8* data, size_t len, u8 key) {
    for (size_t i = 0; i < len; ++i){
        data[i] ^= key;
    }
}

[[nodiscard]] std::array<u8, 8> to_be64(u64 v) {
    std::array<u8, 8> bytes;
    for (int i = 7; i >= 0; --i) {
        bytes[i] = static_cast<u8>(v & 0xFF);
        v >>= 8;
    }
    return bytes;
}

[[nodiscard]] u64 from_be64(const u8* bytes) {
    u64 v = 0;
    for (int i = 0; i < 8; ++i) {
        v = (v << 8) | bytes[i];
    }
    return v;
}

[[nodiscard]] std::string format_size(u64 bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int idx = 0;
    double val = static_cast<double>(bytes);
    while (val >= 1024.0 && idx < 4) {
        val /= 1024.0; ++idx;
    }
    std::ostringstream oss;
    if (idx == 0) {
        oss << static_cast<u64>(val) << " " << units[idx];
    }
    else {
        oss << std::fixed << std::setprecision(2) << val << " " << units[idx];
    }
    return oss.str();
}

Timer::Timer()
    : start(Timer::Clock::now())
{}

[[nodiscard]] double Timer::elapsed() const{
    return std::chrono::duration<double>(Timer::Clock::now() - start).count();
}

} // namespace utils
