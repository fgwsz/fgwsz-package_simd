// 标准库
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <map>
#include <sstream>
#include <random>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <memory>
#include <format>

// 自定义模块
#include "constants.hpp"
#include "package_error.hpp"
#include "io.hpp"
#include "utils.hpp"
#include "simd.hpp"
#include "console.hpp"
#include "path_utils.hpp"
#include "entry_header.hpp"

#include "packer.hpp"

void Packer::run(
    const std::vector<std::filesystem::path>& inputs,
    const std::filesystem::path& output
) {
    auto out_abs = std::filesystem::absolute(output);
    if (std::filesystem::exists(out_abs) && !confirm_overwrite(out_abs)) {
        throw PackageError("User cancelled");
    }
    if (std::filesystem::exists(out_abs)) {
        std::filesystem::remove(out_abs);  // 删除旧文件，避免锁定
    }

    auto items = collect_items(inputs, out_abs);
    if (items.empty()) {
        throw PackageError("No files to pack");
    }

    std::sort(
        items.begin(), items.end(),
        [](const auto& a, const auto& b) {
            return a.rel_path < b.rel_path;
        }
    );

    uint64_t total_size = 0;
    for (const auto& item : items) {
        total_size += constants::KEY_SIZE + constants::LENGTH_FIELD_SIZE
                    + item.rel_path.size() + constants::LENGTH_FIELD_SIZE;
        total_size += std::filesystem::file_size(item.abs_path);
    }

    std::filesystem::create_directories(out_abs.parent_path());
    auto writer = io::create_writer(out_abs, total_size, true);
    if (!writer) {
        throw PackageError("Failed to create output writer");
    }

    utils::Timer timer;
    uint64_t total_content_size = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 255);

    std::string const items_size_str = std::to_string(items.size());
    for (size_t idx = 0; idx < items.size(); ++idx) {
        const auto& item = items[idx];
        // 规避 std::format 多次解析 items.size() 带来的IO开销
        console::print(
            "\r[" + std::to_string(idx + 1) + "/" + items_size_str
            + "] Packing: " + item.rel_path
        );
        std::flush(std::cout);

        uint8_t key = static_cast<uint8_t>(dis(gen));
        uint64_t content_len = std::filesystem::file_size(item.abs_path);
        total_content_size += content_len;

        EntryHeader h{key, item.rel_path.size(), item.rel_path, content_len};
        h.serialize(*writer);

        write_content(*writer, item.abs_path, key);
    }

    writer.reset();
    console::print("\n");
    print_summary(
        "Pack",out_abs, items.size(), total_content_size, timer.elapsed()
    );
    set_readonly(out_abs);
}

void Packer::write_content(
    io::IWriter& writer, const std::filesystem::path& file, uint8_t key
) {
    auto reader = io::create_reader(file, true);
    if (!reader) {
        throw PackageError(
            std::format(
                "Cannot open input file: {}", path_utils::to_utf8(file)
            )
        );
    }

    size_t file_size = reader->size();
    if (file_size == 0) {
        return;
    }

    if (reader->is_mmap() && writer.is_mmap()) {
        auto* r = dynamic_cast<io::MmapReader*>(reader.get());
        auto* w = dynamic_cast<io::MmapWriter*>(&writer);
        if (r && w) {
            const uint8_t* src = r->data();
            uint8_t* dst = w->data() + w->tell();
            simd::xor_block(dst, src, file_size, key);
            w->advance(file_size);
            return;
        }
    }

    const size_t BUF_SIZE = constants::IO_BUFFER_SIZE;
    std::vector<uint8_t> buf(BUF_SIZE);
    size_t remaining = file_size;
    while (remaining > 0) {
        size_t to_read = (std::min)(remaining, BUF_SIZE);
            // (std::min)的涵义:避免windows min宏和std::min函数冲突
        if (!reader->read(buf.data(), to_read)) {
            throw PackageError("Read input file failed");
        }
        simd::xor_block(buf.data(), buf.data(), to_read, key);
        if (!writer.write(buf.data(), to_read)) {
            throw PackageError("Write to output failed");
        }
        remaining -= to_read;
    }
}

std::vector<Packer::Item> Packer::collect_items(
    const std::vector<std::filesystem::path>& inputs,
    const std::filesystem::path& out_abs
) {
    std::vector<Item> items;
    std::vector<std::filesystem::path> abs_inputs;
    for (const auto& p : inputs) {
        auto abs_p = std::filesystem::absolute(p);
        if (!std::filesystem::exists(abs_p)) {
            throw PackageError(
                std::format(
                    "Path does not exist: {}", path_utils::to_utf8(abs_p)
                )
            );
        }
        if (
            std::find(abs_inputs.begin(), abs_inputs.end(), abs_p)
            != abs_inputs.end()
        ) {
            throw PackageError(
                std::format("Duplicate input: {}", path_utils::to_utf8(p))
            );
        }
        abs_inputs.push_back(abs_p);
    }

    std::vector<std::filesystem::path> files, dirs;
    for (const auto& p : abs_inputs) {
        if (std::filesystem::is_symlink(p)) {
            console::println(
                std::format(
                    "Symbolic link skipped: {}", path_utils::to_utf8(p)
                )
            );
            continue;
        }
        if (std::filesystem::is_regular_file(p)) {
            files.push_back(p);
        } else if (std::filesystem::is_directory(p)) {
            dirs.push_back(p);
        } else {
            throw PackageError(
                std::format("Unknown file type: {}", path_utils::to_utf8(p))
            );
        }
    }

    std::map<std::string, std::vector<std::filesystem::path>> name_map;
    for (const auto& f : files) {
        name_map[path_utils::to_utf8(f.filename())].push_back(f);
    }
    for (const auto& [fname, vec] : name_map) {
        if (vec.size() > 1) {
            std::ostringstream msg;
            msg << "Duplicate direct filenames: " << fname << "\n";
            for (size_t i = 0; i < vec.size(); ++i) {
                msg << "  " << (i+1) << ". "
                    << path_utils::to_utf8(vec[i]) << "\n";
            }
            throw PackageError(msg.str());
        }
    }

    // 修正：包含顶层目录名
    for (const auto& d : dirs) {
        std::filesystem::path base = d.parent_path();
        for (auto& entry : std::filesystem::recursive_directory_iterator(d)) {
            if (
                std::filesystem::is_symlink(entry)
                || !std::filesystem::is_regular_file(entry)
            ) {
                continue;
            }
            if (std::filesystem::absolute(entry.path()) == out_abs) {
                continue;
            }
            std::filesystem::path rel =
                std::filesystem::relative(entry.path(), base);
            items.push_back({path_utils::to_posix_utf8(rel), entry.path()});
        }
    }

    for (const auto& f : files) {
        if (std::filesystem::absolute(f) == out_abs) {
            throw PackageError("Cannot pack output file itself");
        }
        items.push_back({path_utils::to_utf8(f.filename()), f});
    }
    return items;
}

bool Packer::confirm_overwrite(const std::filesystem::path& file) {
    console::print(
        std::format(
            "Warning: Output file '{}' already exists and will be"
                " overwritten.\n"
            "Continue? (y/N): ",
            path_utils::to_utf8(file)
        )
    );
    console::flush(); // 确保打印信息立刻回显,先显示提示信息后输入
    std::string line;
    std::getline(std::cin, line);
    return line == "y" || line == "Y";
}

void Packer::print_summary(
    const std::string& action, const std::filesystem::path& target,
    size_t count, uint64_t content_size, double seconds
) {
    uint64_t pkg_size = std::filesystem::exists(target)
        ? std::filesystem::file_size(target)
        : 0;
    double ratio = pkg_size
        ? (static_cast<double>(content_size)/pkg_size*100)
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
            utils::format_size(pkg_size), pkg_size,
            ratio,
            seconds
        )
    );
}

void Packer::set_readonly(const std::filesystem::path& p) {
    try {
        std::filesystem::permissions(
            p,
            std::filesystem::perms::owner_read
            | std::filesystem::perms::group_read
            | std::filesystem::perms::others_read,
            std::filesystem::perm_options::replace
        );
    } catch (...) {}
}
