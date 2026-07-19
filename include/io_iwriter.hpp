#pragma once

#include <cstdint>   // uint8_t
#include <cstddef>   // size_t

namespace io {

class IWriter {
public:
    virtual ~IWriter() = default;
    virtual bool write(const uint8_t* src, size_t len) = 0;
    virtual bool seek(size_t pos) = 0;
    virtual size_t tell() const = 0;
    virtual bool truncate(size_t size) = 0;
    virtual bool is_open() const = 0;
    virtual bool is_mmap() const { return false; }
};

} // namespace io
