#pragma once

// 标准库
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

#include "io_iwriter.hpp"

// ---- Packer 实现 ----
class Packer {

public:

    static void run(
        const std::vector<std::filesystem::path>& inputs,
        const std::filesystem::path& output
    );

private:

    struct Item {

        std::string rel_path;

        std::filesystem::path abs_path;

    };

    static void write_content(
        io::IWriter& writer, const std::filesystem::path& file, uint8_t key
    );

    static std::vector<Item> collect_items(
        const std::vector<std::filesystem::path>& inputs,
        const std::filesystem::path& out_abs
    );

    static bool confirm_overwrite(const std::filesystem::path& file);

    static void print_summary(
        const std::string& action, const std::filesystem::path& target,
        size_t count, uint64_t content_size, double seconds
    );

    static void set_readonly(const std::filesystem::path& p);

};
