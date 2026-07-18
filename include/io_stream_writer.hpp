#pragma once

#include <cstdint>      // uint8_t
#include <cstddef>      // size_t
#include <fstream>      // std::ofstream
#include <filesystem>   // std::filesystem

#include "io_iwriter.hpp"       // io::IWriter

namespace io {

// ----- StreamWriter -----
class StreamWriter final : public IWriter {

    std::ofstream fout_;

    size_t pos_ = 0;

public:

    explicit StreamWriter(const std::filesystem::path& path);

    bool write(const uint8_t* src, size_t len) override;

    bool seek(size_t) override;

    size_t tell() const override;

    bool truncate(size_t) override;

    bool is_open() const override;

};

} // namespace io
