#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <cstdint>

class Lister {

public:

    static void run(const std::filesystem::path& package);

};
