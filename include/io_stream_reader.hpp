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

    explicit StreamReader(const std::filesystem::path& path);

    bool read(uint8_t* dst, size_t len) override;

    bool seek(size_t pos) override;

    size_t tell() const override;

    size_t size() const override;

    bool is_open() const override;

};

} // namespace io
