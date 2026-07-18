#pragma once

#include <cstdint>      // uint8_t
#include <cstddef>      // size_t
#include <cstring>      // memcpy
#include <filesystem>   // std::filesystem

#ifdef _WIN32
#include <windows.h>    // Windows API
#else
#include <fcntl.h>      // O_RDWR, O_CREAT, O_TRUNC
#include <unistd.h>     // close, ftruncate
#include <sys/mman.h>   // mmap, munmap, PROT_*, MAP_*
#endif

#include "io_iwriter.hpp"

namespace io {

// ----- MmapWriter -----
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
    explicit MmapWriter(const std::filesystem::path& path, size_t size) { create(path, size); }
    ~MmapWriter() override { close(); }

    bool create(const std::filesystem::path& path, size_t size) {
        close();
#ifdef _WIN32
        HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                                   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) return false;
        LARGE_INTEGER li; li.QuadPart = size;
        if (!SetFilePointerEx(hFile, li, nullptr, FILE_BEGIN) || !SetEndOfFile(hFile)) {
            CloseHandle(hFile); return false;
        }
        HANDLE hMap = CreateFileMappingW(hFile, nullptr, PAGE_READWRITE,
                                         static_cast<DWORD>(size >> 32),
                                         static_cast<DWORD>(size & 0xFFFFFFFF),
                                         nullptr);
        if (!hMap) { CloseHandle(hFile); return false; }
        void* ptr = MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, size);
        if (!ptr) { CloseHandle(hMap); CloseHandle(hFile); return false; }
        data_ = static_cast<uint8_t*>(ptr);
        len_ = size;
        hFile_ = hFile; hMap_ = hMap; open_ = true; return true;
#else
        int fd = ::open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) return false;
        if (ftruncate(fd, static_cast<off_t>(size)) == -1) { ::close(fd); return false; }
        void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) { ::close(fd); return false; }
        data_ = static_cast<uint8_t*>(ptr);
        len_ = size;
        fd_ = fd; open_ = true; return true;
#endif
    }

    void close() {
#ifdef _WIN32
        if (data_) { UnmapViewOfFile(data_); data_ = nullptr; }
        if (hMap_ != INVALID_HANDLE_VALUE) { CloseHandle(hMap_); hMap_ = INVALID_HANDLE_VALUE; }
        if (hFile_ != INVALID_HANDLE_VALUE) { CloseHandle(hFile_); hFile_ = INVALID_HANDLE_VALUE; }
#else
        if (data_) { munmap(data_, len_); data_ = nullptr; }
        if (fd_ != -1) { ::close(fd_); fd_ = -1; }
#endif
        len_ = 0; pos_ = 0; open_ = false;
    }

    bool write(const uint8_t* src, size_t len) override {
        if (!open_ || pos_ + len > len_) return false;
        memcpy(data_ + pos_, src, len);
        pos_ += len;
        return true;
    }
    bool seek(size_t pos) override { if (!open_ || pos > len_) return false; pos_ = pos; return true; }
    size_t tell() const override { return pos_; }
    bool truncate(size_t size) override { return true; }
    bool is_open() const override { return open_; }
    bool is_mmap() const override { return open_; }

    uint8_t* data() const { return data_; }
    size_t capacity() const { return len_; }
    void advance(size_t n) { pos_ += n; }
};

} // namespace io
