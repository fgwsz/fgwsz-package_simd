#include <cstdint>      // uint8_t
#include <cstddef>      // size_t
#include <fstream>      // std::ifstream
#include <filesystem>   // std::filesystem

#include "io_ireader.hpp"

#include "io_stream_reader.hpp"

namespace io {

StreamReader::StreamReader(const std::filesystem::path& path) {
    fin_.open(path, std::ios::binary);
    if (fin_) {
        fin_.seekg(0, std::ios::end);
        len_ = static_cast<size_t>(fin_.tellg());
        fin_.seekg(0, std::ios::beg);
        open_ = true;
    }
}

bool StreamReader::read(uint8_t* dst, size_t len) {
    if (!open_) {
        return false;
    }
    fin_.read(reinterpret_cast<char*>(dst), len);
    if (fin_.gcount() != static_cast<std::streamsize>(len)) {
        return false;
    }
    pos_ += len;
    return true;
}

bool StreamReader::seek(size_t pos) {
    if (!open_ || pos > len_) {
        return false;
    }
    fin_.seekg(static_cast<std::streamoff>(pos));
    pos_ = pos;
    return true;
}

size_t StreamReader::tell() const {
    return pos_;
}

size_t StreamReader::size() const {
    return len_;
}

bool StreamReader::is_open() const {
    return open_;
}

} // namespace io
