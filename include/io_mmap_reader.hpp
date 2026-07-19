#pragma once

#include <cstdint>      // uint8_t
#include <cstddef>      // size_t
#include <filesystem>   // std::filesystem

#ifdef _WIN32
#include <windows.h>    // Windows API
#endif

#include "io_ireader.hpp"

namespace io {

class MmapReader final : public IReader {
    const uint8_t* data_ = nullptr;
    size_t len_ = 0, pos_ = 0;
    bool open_ = false;
#ifdef _WIN32
    HANDLE hFile_ = INVALID_HANDLE_VALUE, hMap_ = INVALID_HANDLE_VALUE;
#else
    int fd_ = -1;
#endif

public:

    explicit MmapReader(const std::filesystem::path& path);

    ~MmapReader() override;

    bool open(const std::filesystem::path& path);

    void close();

    bool read(uint8_t* dst, size_t len) override;

    bool seek(size_t pos) override;

    size_t tell() const override;

    size_t size() const override;

    bool is_open() const override;

    bool is_mmap() const override;

    const uint8_t* data() const;

};

}// namespace io
