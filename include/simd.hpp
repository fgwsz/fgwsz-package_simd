#pragma once

#include <cstddef>   // size_t
#include <cstdint>   // uint8_t

// ========================== SIMD 加速 XOR ==========================
namespace simd {

using u8 = uint8_t;

// 主入口:自动选择最佳指令集,处理任意长度
void xor_block(u8* dst, const u8* src, size_t len, u8 key);

} // namespace simd
