#include <cstring>
#include <string>
#include <filesystem>
#include <format>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
#endif

#include "package_error.hpp"

#include "io_file_writer.hpp"


namespace io {

namespace detail{

// ---------- 平台相关常量 ----------
#ifdef _WIN32
static const/*expr*/ auto INVALID_NATIVE_HANDLE =
    reinterpret_cast<void*>(INVALID_HANDLE_VALUE);
    // windows不支持在constexpr表达式中使用强制类型转换reinterpret_cast
#else
static constexpr int INVALID_NATIVE_HANDLE = -1;
#endif

} // namespace detail

// ---------- 构造函数与析构 ----------
FileWriter::FileWriter(const std::filesystem::path& path)
    : handle_(detail::INVALID_NATIVE_HANDLE), open_(false), pos_(0)
{
    open_file(path);
}

FileWriter::~FileWriter() {
    close_internal();
}

// ---------- 移动语义 ----------
FileWriter::FileWriter(FileWriter&& other) noexcept
    : handle_(other.handle_), open_(other.open_), pos_(other.pos_)
{
    other.handle_ = detail::INVALID_NATIVE_HANDLE;
    other.open_ = false;
    other.pos_ = 0;
}

FileWriter& FileWriter::operator=(FileWriter&& other) noexcept {
    if (this != &other) {
        close_internal();
        handle_ = other.handle_;
        open_ = other.open_;
        pos_ = other.pos_;
        other.handle_ = detail::INVALID_NATIVE_HANDLE;
        other.open_ = false;
        other.pos_ = 0;
    }
    return *this;
}

// ---------- IWriter 接口 ----------
bool FileWriter::write(const uint8_t* data, size_t size) {
    if (!open_) {
        return false;
    }
#ifdef _WIN32
    DWORD written = 0;
    if (
        !WriteFile(
            static_cast<HANDLE>(handle_), data, static_cast<DWORD>(size),
            &written, nullptr
        )
    ) {
        return false;
    }
    if (written != static_cast<DWORD>(size)) {
        return false;
    }
#else
    ssize_t written = ::write(handle_, data, size);
    if (written != static_cast<ssize_t>(size)) {
        return false;
    }
#endif
    pos_ += size;
    return true;
}

bool FileWriter::seek(size_t /*pos*/) {
    return false;   // 不支持随机写入
}

size_t FileWriter::tell() const {
    return pos_;
}

bool FileWriter::truncate(size_t /*size*/) {
    return true;   // 无操作，文件大小由创建时决定
}

bool FileWriter::is_open() const {
    return open_;
}

bool FileWriter::is_mmap() const {
    return false;
}

void FileWriter::close() {
    close_internal();
}

// ---------- 私有方法 ----------

void FileWriter::open_file(const std::filesystem::path& path) {
#ifdef _WIN32
    HANDLE h = CreateFileW(
        path.c_str(),
        GENERIC_WRITE,
        0,               // 独占
        nullptr,
        CREATE_ALWAYS,   // 总是创建新文件
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr
    );
    if (h == INVALID_HANDLE_VALUE) {
        throw PackageError(
            std::format("Cannot create file: {}", path.string())
        );
    }
    handle_ = static_cast<native_handle>(h);
    open_ = true;
#else
    int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        throw PackageError(
            std::format(
                "Cannot create file: {}: {}",
                path.string(),
                strerror(errno)
            )
        );
    }
    handle_ = fd;
    open_ = true;
#endif
}

void FileWriter::close_internal() {
    if (!open_) {
        return;
    }
#ifdef _WIN32
    CloseHandle(static_cast<HANDLE>(handle_));
#else
    ::close(handle_);
#endif
    open_ = false;
    handle_ = detail::INVALID_NATIVE_HANDLE;
    pos_ = 0;
}

} // namespace io
