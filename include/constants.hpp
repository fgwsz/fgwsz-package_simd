#pragma once

#include <cstddef>
#include <cstdint>

namespace constants {

constexpr uint64_t LENGTH_FIELD_SIZE = 8;

constexpr uint64_t KEY_SIZE = 1;

constexpr size_t   IO_BUFFER_SIZE = 64 * 1024 * 1024; // 64MB

} // namespace constants
