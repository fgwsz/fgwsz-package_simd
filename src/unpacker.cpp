// 标准库
#include <filesystem>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <string>
#include <format>

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
            std::format(
                "Package file does not exist: {}",
                path_utils::to_utf8(package)
            )
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

    const size_t BUF_SIZE = constants::IO_BUFFER_SIZE;
    std::vector<uint8_t> buf(BUF_SIZE);

    while (reader->tell() < reader->size()) {
        auto header = EntryHeader::deserialize(*reader);
        console::print(std::format("\rUnpacking: {}", header.path));

        std::filesystem::path target =
            out_root/ path_utils::from_utf8(header.path);
        std::filesystem::create_directories(target.parent_path());

        auto writer = io::create_writer(target, header.content_len, true);
        if (!writer) {
            throw PackageError(
                std::format(
                    "Cannot create file: {}", path_utils::to_utf8(target)
                )
            );
        }

        uint64_t remaining = header.content_len;
        while (remaining > 0) {
            size_t to_read = (std::min)(remaining, BUF_SIZE);
                // (std::min)的涵义:避免windows min宏和std::min函数冲突
            if (!reader->read(buf.data(), to_read)) {
                throw PackageError("Incomplete content data");
            }
            simd::xor_block(buf.data(), buf.data(), to_read, header.key);
            if (!writer->write(buf.data(), to_read)) {
                throw PackageError(
                    std::format(
                        "Write to file failed: {}",
                        path_utils::to_utf8(target)
                    )
                );
            }
            remaining -= to_read;
        }
        // out 析构自动关闭文件

        total_content_size += header.content_len;
        ++file_count;
    }

    console::print("\n");
    print_summary(
        "Unpack", out_root, file_count,
        total_content_size, std::filesystem::file_size(package),
        timer.elapsed()
    );
}

void Unpacker::print_summary(
    const std::string& action, const std::filesystem::path& target,
    size_t count, uint64_t content_size, uint64_t package_size, double seconds
) {
    double ratio = package_size
        ? (static_cast<double>(content_size)/package_size*100)
        : 0;

    console::println(
        std::format(
            "\n{} completed, target: {}\n"
            "Total: {} files\n"
            "Content: {} ({} bytes)\n"
            "Package: {} ({} bytes)\n"
            "Ratio: {} %\n"
            "Time: {} s",
            action, path_utils::to_utf8(target),
            count,
            utils::format_size(content_size), content_size,
            utils::format_size(package_size), package_size,
            ratio,
            seconds
        )
    );
}
