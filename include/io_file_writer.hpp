#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>

#include "io_iwriter.hpp"
#include "package_error.hpp"

namespace io {

/**
 * 基于原生系统 API 的文件写入器
 * - Windows: CreateFileW / WriteFile
 * - POSIX:   open / write
 * 无内部缓冲，适合与外部大缓冲区配合使用（如解包多个文件）。
 * 实现 io::IWriter 接口，可作为 MmapWriter 的回退或单独使用。
 */
class FileWriter : public io::IWriter {
public:
    /**
     * 构造函数，创建或覆盖文件。
     * @param path 文件路径
     * @throws PackageError 如果打开失败
     */
    explicit FileWriter(const std::filesystem::path& path);

    /**
     * 析构函数，自动关闭文件句柄。
     */
    ~FileWriter() override;

    // 禁止拷贝
    FileWriter(const FileWriter&) = delete;
    FileWriter& operator=(const FileWriter&) = delete;

    // 移动语义
    FileWriter(FileWriter&& other) noexcept;
    FileWriter& operator=(FileWriter&& other) noexcept;

    // ----- IWriter 接口实现 -----

    /**
     * 写入数据块。
     * @param data 数据指针
     * @param size 要写入的字节数
     * @return true 成功，false 失败
     */
    bool write(const uint8_t* data, size_t size) override;

    /**
     * 随机寻址（不支持顺序写入文件）。
     * @return 始终返回 false
     */
    bool seek(size_t pos) override;

    /**
     * 返回当前写入位置（已写入的字节数）。
     */
    size_t tell() const override;

    /**
     * 截断文件（无操作，文件大小由打开模式决定）。
     * @return 始终返回 true
     */
    bool truncate(size_t size) override;

    /**
     * 检查文件是否打开。
     */
    bool is_open() const override;

    /**
     * 是否为 mmap 后端（不是）。
     * @return 始终返回 false
     */
    bool is_mmap() const override;

    /**
     * 手动关闭文件（可选，析构自动调用）。
     */
    void close();

private:
#ifdef _WIN32
    using native_handle = void*;   // 实际为 HANDLE
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
