#pragma once

#include <cstdint>   // uint8_t
#include <cstddef>   // size_t

namespace io {

// ----- 抽象接口 -----
class IReader {
public:
    virtual ~IReader() = default;
    virtual bool read(uint8_t* dst, size_t len) = 0;
    virtual bool seek(size_t pos) = 0;
    virtual size_t tell() const = 0;
    virtual size_t size() const = 0;
    virtual bool is_open() const = 0;
    virtual bool is_mmap() const { return false; }  // 用于零拷贝检测
};

} // namespace io
