#pragma once

#include <cstdint>      // uint8_t
#include <cstddef>      // size_t
#include <fstream>      // std::ifstream
#include <filesystem>   // std::filesystem

#include "io_ireader.hpp"


namespace io {

// ----- StreamReader -----
class StreamReader final : public IReader {
    std::ifstream fin_;
    size_t len_ = 0, pos_ = 0;
    bool open_ = false;
public:
    explicit StreamReader(const std::filesystem::path& path) {
        fin_.open(path, std::ios::binary);
        if (fin_) {
            fin_.seekg(0, std::ios::end);
            len_ = static_cast<size_t>(fin_.tellg());
            fin_.seekg(0, std::ios::beg);
            open_ = true;
        }
    }
    bool read(uint8_t* dst, size_t len) override {
        if (!open_) return false;
        fin_.read(reinterpret_cast<char*>(dst), len);
        if (fin_.gcount() != static_cast<std::streamsize>(len)) return false;
        pos_ += len;
        return true;
    }
    bool seek(size_t pos) override {
        if (!open_ || pos > len_) return false;
        fin_.seekg(static_cast<std::streamoff>(pos));
        pos_ = pos;
        return true;
    }
    size_t tell() const override { return pos_; }
    size_t size() const override { return len_; }
    bool is_open() const override { return open_; }
};

} // namespace io
