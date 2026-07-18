#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <filesystem>

// ---- Unpacker 实现 ----
class Unpacker {

public:

    static void run(
        const std::filesystem::path& package,
        const std::filesystem::path& output_dir
    );

private:

    static void print_summary(
        const std::string& action, const std::filesystem::path& target,
        size_t count, uint64_t content_size, double seconds
    );

};
