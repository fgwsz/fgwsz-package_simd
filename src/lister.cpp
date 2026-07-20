// 标准库
#include <cstddef>
#include <filesystem>
#include <string>
#include <cstdint>
#include <format>

// 自定义模块
#include "package_error.hpp"
#include "path_utils.hpp"
#include "io.hpp"
#include "utils.hpp"
#include "entry_header.hpp"
#include "console.hpp"
#include "summary.hpp"

#include "lister.hpp"

void Lister::run(const std::filesystem::path& package) {
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

    auto reader = io::create_reader(package, true);
    if (!reader || !reader->is_open()) {
        throw PackageError("Failed to open package file");
    }

    utils::Timer timer;
    size_t file_count = 0;
    uint64_t total_size = 0;

    while (reader->tell() < reader->size()) {
        auto header = EntryHeader::deserialize(*reader);
        console::println(
            std::format("{} ({} bytes)", header.path, header.content_len)
        );
        file_count++;
        total_size += header.content_len;
        reader->seek(reader->tell() + header.content_len);
    }

    uint64_t pkg_size = std::filesystem::exists(package)
        ? std::filesystem::file_size(package)
        : 0;
    summary::print_summary(
        "List", package, file_count, total_size, pkg_size, timer.elapsed()
    );
}
