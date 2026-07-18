#pragma once

// 标准库
#include <cstddef>
#include <filesystem>
#include <string>
#include <cstdint>

// 自定义模块
#include "package_error.hpp"
#include "path_utils.hpp"
#include "io.hpp"
#include "utils.hpp"
#include "entry_header.hpp"
#include "console.hpp"

// ---- Lister 实现 ----
class Lister {
public:
    static void run(const std::filesystem::path& package) {
        if (!std::filesystem::exists(package) || !std::filesystem::is_regular_file(package))
            throw PackageError("Package file does not exist: " + path_utils::to_utf8(package));

        auto reader = io::create_reader(package, true);
        if (!reader || !reader->is_open())
            throw PackageError("Failed to open package file");

        utils::Timer timer;
        size_t file_count = 0;
        uint64_t total_size = 0;

        while (reader->tell() < reader->size()) {
            auto header = EntryHeader::deserialize(*reader);
            console::println(header.path + "  (" + std::to_string(header.content_len) + " bytes)");
            file_count++;
            total_size += header.content_len;
            reader->seek(reader->tell() + header.content_len);
        }

        print_summary("列表", package, file_count, total_size, timer.elapsed());
    }

private:
    static void print_summary(const std::string& action, const std::filesystem::path& target,
                              size_t count, uint64_t total_size, double seconds) {
        uint64_t pkg_size = std::filesystem::exists(target) ? std::filesystem::file_size(target) : 0;
        double ratio = pkg_size ? (static_cast<double>(total_size)/pkg_size*100) : 0;
        console::println("\n总计: " + std::to_string(count) + " 个文件");
        console::println("内容总大小: " + utils::format_size(total_size) + " (" + std::to_string(total_size) + " 字节)");
        console::println("包文件大小: " + utils::format_size(pkg_size) + " (" + std::to_string(pkg_size) + " 字节)");
        console::println("内容占比: " + std::to_string(ratio) + "%");
        console::println("用时: " + std::to_string(seconds) + " 秒");
    }
};

