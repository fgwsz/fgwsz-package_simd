#pragma once

#include <cstdint>      // uint8_t
#include <cstddef>      // size_t
#include <fstream>      // std::ofstream
#include <filesystem>   // std::filesystem

#include "io_iwriter.hpp"       // io::IWriter
#include "package_error.hpp"    // PackageError

namespace io {

// ----- StreamWriter -----
class StreamWriter final : public IWriter {
    std::ofstream fout_;
    size_t pos_ = 0;
public:
    explicit StreamWriter(const std::filesystem::path& path) {
        fout_.open(path, std::ios::binary);
        if (!fout_) throw PackageError("Cannot open output file");
    }
    bool write(const uint8_t* src, size_t len) override {
        fout_.write(reinterpret_cast<const char*>(src), len);
        if (!fout_) return false;
        pos_ += len;
        return true;
    }
    bool seek(size_t) override { return false; }
    size_t tell() const override { return pos_; }
    bool truncate(size_t) override { return true; }
    bool is_open() const override { return fout_.good(); }
};

} // namespace io
