// 标准库
#include <filesystem>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <string>

// 自定义模块
#include "package_error.hpp"
#include "path_utils.hpp"
#include "io.hpp"
#include "utils.hpp"
#include "entry_header.hpp"
#include "simd.hpp"
#include "console.hpp"
#include "constants.hpp"

#include "unpacker.hpp"

void Unpacker::run(
    const std::filesystem::path& package,
    const std::filesystem::path& output_dir
) {
    if (
        !std::filesystem::exists(package)
        || !std::filesystem::is_regular_file(package)
    ) {
        throw PackageError(
            "Package file does not exist: " + path_utils::to_utf8(package)
        );
    }

    std::filesystem::path out_root = std::filesystem::absolute(output_dir);
    std::filesystem::create_directories(out_root);

    auto reader = io::create_reader(package, true);
    if (!reader || !reader->is_open()) {
        throw PackageError("Failed to open package file");
    }

    utils::Timer timer;
    uint64_t total_content_size = 0;
    size_t file_count = 0;

    while (reader->tell() < reader->size()) {
        auto header = EntryHeader::deserialize(*reader);
        console::print("\r正在解包: " + header.path);

        std::filesystem::path target =
            out_root/ path_utils::from_utf8(header.path);
        std::filesystem::create_directories(target.parent_path());
        std::ofstream fout(target, std::ios::binary);
        if (!fout) {
            throw PackageError(
                "Cannot create file: " + path_utils::to_utf8(target)
            );
        }

        const size_t BUF_SIZE = constants::IO_BUFFER_SIZE;
        std::vector<uint8_t> buf(BUF_SIZE);
        uint64_t remaining = header.content_len;
        while (remaining > 0) {
            size_t to_read = std::min(remaining, BUF_SIZE);
            if (!reader->read(buf.data(), to_read)) {
                throw PackageError("Incomplete content data");
            }
            simd::xor_block(buf.data(), buf.data(), to_read, header.key);
            fout.write(reinterpret_cast<const char*>(buf.data()), to_read);
            remaining -= to_read;
        }

        total_content_size += header.content_len;
        ++file_count;
    }

    console::print("\n");
    print_summary(
        "解包完成", out_root, file_count, total_content_size, timer.elapsed()
    );
}

void Unpacker::print_summary(
    const std::string& action, const std::filesystem::path& target,
    size_t count, uint64_t content_size, double seconds
) {
    uint64_t pkg_size = std::filesystem::exists(target)
        ? std::filesystem::file_size(target)
        : 0;
    double ratio = pkg_size
        ? (static_cast<double>(content_size)/pkg_size*100)
        : 0;
    console::println(action + "，输出目录: " + path_utils::to_utf8(target));
    console::println("共解包 " + std::to_string(count) + " 个文件。");
    console::println(
        "内容总大小: " + utils::format_size(content_size)
        + " (" + std::to_string(content_size) + " 字节)"
    );
    console::println(
        "包文件大小: " + utils::format_size(pkg_size)
        + " (" + std::to_string(pkg_size) + " 字节)"
    );
    console::println("内容占比: " + std::to_string(ratio) + "%");
    console::println("用时: " + std::to_string(seconds) + " 秒");
}
