#include <cstddef>      // size_t
#include <memory>       // std::unique_ptr, std::make_unique
#include <filesystem>   // std::filesystem

#include "package_error.hpp" // PackageError
#include "io_ireader.hpp"
#include "io_iwriter.hpp"
#include "io_mmap_reader.hpp"
#include "io_mmap_writer.hpp"
#include "io_stream_reader.hpp"
#include "io_stream_writer.hpp"

#include "io_create.hpp"

namespace io {

std::unique_ptr<IReader> create_reader(
    const std::filesystem::path& path,
    bool prefer_mmap
) {
    if (prefer_mmap) {
        auto r = std::make_unique<MmapReader>(path);
        if (r->is_open()) return r;
    }
    auto r = std::make_unique<StreamReader>(path);
    if (r->is_open()) return r;
    return nullptr;
}

std::unique_ptr<IWriter> create_writer(
    const std::filesystem::path& path,
    size_t size, bool prefer_mmap
) {
    if (prefer_mmap) {
        auto w = std::make_unique<MmapWriter>(path, size);
        if (w->is_open()) return w;
    }
    try {
        return std::make_unique<StreamWriter>(path);
    } catch (const PackageError&) {
        return nullptr;
    }
}

} // namespace io
