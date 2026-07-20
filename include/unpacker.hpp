#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <filesystem>

class Unpacker {

public:

    static void run(
        const std::filesystem::path& package,
        const std::filesystem::path& output_dir
    );

};
