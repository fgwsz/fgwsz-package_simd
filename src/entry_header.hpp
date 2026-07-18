#pragma once

#include <cstdint>      // uint8_t, uint64_t
#include <string>       // std::string

#include "io_iwriter.hpp"      // io::IWriter
#include "io_ireader.hpp"      // io::IReader

// ---- 辅助：头信息结构 ----
struct EntryHeader {

    uint8_t key;

    uint64_t path_len;

    std::string path;

    uint64_t content_len;

    void serialize(io::IWriter& w) const;

    static EntryHeader deserialize(io::IReader& r);

};
