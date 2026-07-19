#include <cstdint>      // uint8_t
#include <cstddef>      // size_t
#include <cstring>      // memcpy
#include <filesystem>   // std::filesystem

#ifdef _WIN32
    #include <windows.h>    // Windows API
#else
    #include <fcntl.h>      // O_RDONLY
    #include <unistd.h>     // close, ftruncate, etc.
    #include <sys/mman.h>   // mmap, munmap
    #include <sys/stat.h>   // fstat, struct stat
#endif

#include "io_ireader.hpp"

#include "io_mmap_reader.hpp"


namespace io {

MmapReader::MmapReader(const std::filesystem::path& path) {
    open(path);
}

MmapReader::~MmapReader() {
    close();
}

bool MmapReader::open(const std::filesystem::path& path) {
    close();
#ifdef _WIN32
    HANDLE hFile = CreateFileW(
        path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL| FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    LARGE_INTEGER li;
    if (!GetFileSizeEx(hFile, &li)) {
        CloseHandle(hFile); return false;
    }
    len_ = static_cast<size_t>(li.QuadPart);
    HANDLE hMap = CreateFileMappingW(
        hFile, nullptr, PAGE_READONLY, 0, 0, nullptr
    );
    if (!hMap) {
        CloseHandle(hFile); return false;
    }
    void* ptr = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, len_);
    if (!ptr) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        return false;
    }
    data_ = static_cast<const uint8_t*>(ptr);
    hFile_ = hFile;
    hMap_ = hMap;
    open_ = true;
    return true;
#else
    int fd = ::open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        return false;
    }
    struct stat st;
    if (fstat(fd, &st) == -1) {
        ::close(fd);
        return false;
    }
    len_ = static_cast<size_t>(st.st_size);
    void* ptr = mmap(nullptr, len_, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        ::close(fd);
        return false;
    }
    data_ = static_cast<const uint8_t*>(ptr);
    fd_ = fd;
    open_ = true;

    // 增加 unix mmap 顺序优化
    #ifdef __linux__
        posix_fadvise(fd_, 0, len_, POSIX_FADV_SEQUENTIAL);
        madvise(
            const_cast<void*>(static_cast<const void*>(data_)),
            len_, MADV_SEQUENTIAL
        );
    #elif defined(__APPLE__)
        int enable = 1;
        fcntl(fd_, F_RDAHEAD, &enable);
        madvise(
            const_cast<void*>(static_cast<const void*>(data_)),
            len_, MADV_SEQUENTIAL
        );
    #endif

    return true;
#endif
}

void MmapReader::close() {
#ifdef _WIN32
    if (data_) {
        UnmapViewOfFile(const_cast<uint8_t*>(data_));
        data_ = nullptr;
    }
    if (hMap_ != INVALID_HANDLE_VALUE) {
        CloseHandle(hMap_);
        hMap_ = INVALID_HANDLE_VALUE;
    }
    if (hFile_ != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile_);
        hFile_ = INVALID_HANDLE_VALUE;
    }
#else
    if (data_) {
        munmap(const_cast<uint8_t*>(data_), len_);
        data_ = nullptr;
    }
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
#endif
    pos_ = 0;
    len_ = 0;
    open_ = false;
}

bool MmapReader::read(uint8_t* dst, size_t len) {
    if (!open_ || pos_ + len > len_) return false;
    memcpy(dst, data_ + pos_, len);
    pos_ += len;
    return true;
}

bool MmapReader::seek(size_t pos) {
    if (!open_ || pos > len_) {
        return false;
    }
    pos_ = pos;
    return true;
}

size_t MmapReader::tell() const {
    return pos_;
}

size_t MmapReader::size() const {
    return len_;
}

bool MmapReader::is_open() const {
    return open_;
}

bool MmapReader::is_mmap() const {
    return open_;
}

const uint8_t* MmapReader::data() const {
    return data_;
}

}// namespace io
