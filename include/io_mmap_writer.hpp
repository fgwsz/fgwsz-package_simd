#pragma once

#include <cstdint>      // uint8_t
#include <cstddef>      // size_t
#include <filesystem>   // std::filesystem

#ifdef _WIN32
#include <windows.h>    // Windows API
#endif

#include "io_iwriter.hpp"

namespace io {

class MmapWriter final : public IWriter {
    uint8_t* data_ = nullptr;
    size_t len_ = 0, pos_ = 0;
    bool open_ = false;
#ifdef _WIN32
    HANDLE hFile_ = INVALID_HANDLE_VALUE, hMap_ = INVALID_HANDLE_VALUE;
#else
    int fd_ = -1;
#endif

public:
    explicit MmapWriter(const std::filesystem::path& path, size_t size);

    ~MmapWriter() override;

    bool create(const std::filesystem::path& path, size_t size);

    void close();

    bool write(const uint8_t* src, size_t len) override;

    bool seek(size_t pos) override;

    size_t tell() const override;

    bool truncate(size_t size) override;

    bool is_open() const override;

    bool is_mmap() const override;

    uint8_t* data() const;

    size_t capacity() const;

    void advance(size_t n);
};

} // namespace io
