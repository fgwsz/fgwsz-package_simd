#pragma once

#include <cstddef>   // size_t
#include <cstdint>   // uint8_t

namespace simd {

using u8 = uint8_t;

void xor_block(u8* dst, const u8* src, size_t len, u8 key);

} // namespace simd
