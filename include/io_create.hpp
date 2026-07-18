#pragma once

#include <cstddef>      // size_t
#include <memory>       // std::unique_ptr, std::make_unique
#include <filesystem>   // std::filesystem

#include "io_ireader.hpp"
#include "io_iwriter.hpp"

namespace io {

// ----- 工厂函数（修正） -----
std::unique_ptr<IReader> create_reader(
    const std::filesystem::path& path, bool prefer_mmap = true
);

std::unique_ptr<IWriter> create_writer(
    const std::filesystem::path& path, size_t size, bool prefer_mmap = true
);

} // namespace io
