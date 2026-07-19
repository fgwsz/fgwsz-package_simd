#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>

#include "io_iwriter.hpp"
#include "package_error.hpp"

namespace io {

class FileWriter : public io::IWriter {
public:

    explicit FileWriter(const std::filesystem::path& path);

    ~FileWriter() override;

    FileWriter(const FileWriter&) = delete;
    FileWriter& operator=(const FileWriter&) = delete;

    FileWriter(FileWriter&& other) noexcept;
    FileWriter& operator=(FileWriter&& other) noexcept;

    bool write(const uint8_t* data, size_t size) override;

    bool seek(size_t pos) override;

    size_t tell() const override;

    bool truncate(size_t size) override;

    bool is_open() const override;

    bool is_mmap() const override;

    void close();

private:
#ifdef _WIN32
    using native_handle = void*;
#else
    using native_handle = int;
#endif

    native_handle handle_;
    bool open_;
    size_t pos_;

    void open_file(const std::filesystem::path& path);
    void close_internal();
};

} // namespace io
