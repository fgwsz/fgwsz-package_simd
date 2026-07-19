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

#include "io_mmap_writer.hpp"

namespace io {

MmapWriter::MmapWriter(const std::filesystem::path& path, size_t size) {
    create(path, size);
}

MmapWriter::~MmapWriter() {
    close();
}

bool MmapWriter::create(const std::filesystem::path& path, size_t size) {
    close();
#ifdef _WIN32
    HANDLE hFile = CreateFileW(
        path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    LARGE_INTEGER li;
    li.QuadPart = size;
    if (
        !SetFilePointerEx(hFile, li, nullptr, FILE_BEGIN)
        || !SetEndOfFile(hFile)
    ) {
        CloseHandle(hFile);
        return false;
    }
    HANDLE hMap = CreateFileMappingW(
        hFile, nullptr, PAGE_READWRITE,
        static_cast<DWORD>(size >> 32),
        static_cast<DWORD>(size & 0xFFFFFFFF),
        nullptr
    );
    if (!hMap) {
        CloseHandle(hFile);
        return false;
    }
    void* ptr = MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, size);
    if (!ptr) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        return false;
    }
    data_ = static_cast<uint8_t*>(ptr);
    len_ = size;
    hFile_ = hFile;
    hMap_ = hMap;
    open_ = true;
    return true;
#else
    int fd = ::open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        return false;
    }
    if (ftruncate(fd, static_cast<off_t>(size)) == -1) {
        ::close(fd);
        return false;
    }
    void* ptr = mmap(
        nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0
    );
    if (ptr == MAP_FAILED) {
        ::close(fd); return false;
    }
    data_ = static_cast<uint8_t*>(ptr);
    len_ = size;
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

void MmapWriter::close() {
#ifdef _WIN32
    if (data_) {
        UnmapViewOfFile(data_);
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
        munmap(data_, len_); data_ = nullptr;
    }
    if (fd_ != -1) {
    ::close(fd_); fd_ = -1;
    }
#endif
    len_ = 0; pos_ = 0; open_ = false;
}

bool MmapWriter::write(const uint8_t* src, size_t len) {
    if (!open_ || pos_ + len > len_) {
        return false;
    }
    memcpy(data_ + pos_, src, len);
    pos_ += len;
    return true;
}

bool MmapWriter::seek(size_t pos) {
    if (!open_ || pos > len_) {
        return false;
    }
    pos_ = pos;
    return true;
}

size_t MmapWriter::tell() const {
    return pos_;
}

bool MmapWriter::truncate(size_t size) {
    return true;
}

bool MmapWriter::is_open() const {
    return open_;
}

bool MmapWriter::is_mmap() const {
    return open_;
}

uint8_t* MmapWriter::data() const {
    return data_;
}

size_t MmapWriter::capacity() const {
    return len_;
}

void MmapWriter::advance(size_t n) {
    pos_ += n;
}

} // namespace io
