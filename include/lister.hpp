#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <cstdint>

class Lister {

public:

    static void run(const std::filesystem::path& package);

private:

    static void print_summary(
        const std::string& action, const std::filesystem::path& target,
        size_t count, uint64_t total_size, double seconds
    );

};
