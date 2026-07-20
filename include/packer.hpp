#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

#include "io_iwriter.hpp"

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

    static void set_readonly(const std::filesystem::path& p);

};
