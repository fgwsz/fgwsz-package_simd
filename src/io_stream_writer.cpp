#include <cstdint>      // uint8_t
#include <cstddef>      // size_t
#include <fstream>      // std::ofstream
#include <filesystem>   // std::filesystem

#include "io_iwriter.hpp"       // io::IWriter
#include "package_error.hpp"    // PackageError

#include "io_stream_writer.hpp"

namespace io {

StreamWriter::StreamWriter(const std::filesystem::path& path) {
    fout_.open(path, std::ios::binary);
    if (!fout_) {
        throw PackageError("Cannot open output file");
    }
}

bool StreamWriter::write(const uint8_t* src, size_t len) {
    fout_.write(reinterpret_cast<const char*>(src), len);
    if (!fout_) {
        return false;
    }
    pos_ += len;
    return true;
}

bool StreamWriter::seek(size_t) {
    return false;
}

size_t StreamWriter::tell() const {
    return pos_;
}

bool StreamWriter::truncate(size_t) {
    return true;
}

bool StreamWriter::is_open() const {
    return fout_.good();
}

} // namespace io
