#pragma once

#include <cstdint>   // uint8_t
#include <cstddef>   // size_t

namespace io {

class IReader {
public:
    virtual ~IReader() = default;
    virtual bool read(uint8_t* dst, size_t len) = 0;
    virtual bool seek(size_t pos) = 0;
    virtual size_t tell() const = 0;
    virtual size_t size() const = 0;
    virtual bool is_open() const = 0;
    virtual bool is_mmap() const { return false; }
};

} // namespace io
