#pragma once

#include <cstddef>   // size_t
#include <cstdint>   // uint8_t

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>   // AVX/SSE (包含 _mm256_* 和 _mm_*)
#elif defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>    // ARM NEON
#endif

// ========================== SIMD 加速 XOR ==========================
namespace simd {
    using u8 = uint8_t;

#if defined(__AVX2__)
    inline void xor_block_avx2(u8* dst, const u8* src, u8 key) {
        __m256i kv = _mm256_set1_epi8(static_cast<char>(key));
        __m256i sv = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
        __m256i dv = _mm256_xor_si256(sv, kv);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst), dv);
    }
#endif

#if defined(__SSE2__) || defined(__AVX2__)
    inline void xor_block_sse2(u8* dst, const u8* src, u8 key) {
        __m128i kv = _mm_set1_epi8(static_cast<char>(key));
        __m128i sv = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
        __m128i dv = _mm_xor_si128(sv, kv);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), dv);
    }
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
    inline void xor_block_neon(u8* dst, const u8* src, u8 key) {
        uint8x16_t kv = vdupq_n_u8(key);
        uint8x16_t sv = vld1q_u8(src);
        uint8x16_t dv = veorq_u8(sv, kv);
        vst1q_u8(dst, dv);
    }
#endif

    // 主入口：自动选择最佳指令集，处理任意长度
    inline void xor_block(u8* dst, const u8* src, size_t len, u8 key) {
        size_t i = 0;

#if defined(__AVX2__)
        constexpr size_t AVX2_SIZE = 32;
        for (; i + AVX2_SIZE <= len; i += AVX2_SIZE)
            xor_block_avx2(dst + i, src + i, key);
#endif

#if defined(__SSE2__) || defined(__AVX2__)
        constexpr size_t SSE_SIZE = 16;
        for (; i + SSE_SIZE <= len; i += SSE_SIZE)
            xor_block_sse2(dst + i, src + i, key);
#elif defined(__aarch64__) || defined(_M_ARM64)
        constexpr size_t NEON_SIZE = 16;
        for (; i + NEON_SIZE <= len; i += NEON_SIZE)
            xor_block_neon(dst + i, src + i, key);
#endif

        // 剩余不足 16 字节的尾部，逐字节处理
        for (; i < len; ++i) dst[i] = src[i] ^ key;
    }
} // namespace simd
