#include <cstddef>      // size_t
#include <cstdint>      // uint8_t, uint64_t
#include <string>       // std::string
#include <array>        // std::array
#include <vector>       // std::vector

#include "io_ireader.hpp"      // io::IReader
#include "io_iwriter.hpp"      // io::IWriter
#include "utils.hpp"           // utils::to_be64, from_be64, xor_range
#include "package_error.hpp"   // PackageError

#include "entry_header.hpp"

void EntryHeader::serialize(io::IWriter& w) const {
    if (!w.write(&key, 1)) {
        throw PackageError("Write key failed");
    }

    auto len_be = utils::to_be64(path_len);
    std::array<uint8_t, 8> len_enc;
    for (size_t i = 0; i < 8; ++i) {
        len_enc[i] = len_be[i] ^ key;
    }
    if (!w.write(len_enc.data(), 8)) {
        throw PackageError("Write path_len failed");
    }

    std::vector<uint8_t> path_bytes(path.begin(), path.end());
    utils::xor_range(path_bytes.data(), path_bytes.size(), key);
    if (!w.write(path_bytes.data(), path_bytes.size())) {
        throw PackageError("Write path failed");
    }

    auto cl_be = utils::to_be64(content_len);
    std::array<uint8_t, 8> cl_enc;
    for (size_t i = 0; i < 8; ++i) {
        cl_enc[i] = cl_be[i] ^ key;
    }
    if (!w.write(cl_enc.data(), 8)) {
        throw PackageError("Write content_len failed");
    }
}

EntryHeader EntryHeader::deserialize(io::IReader& r) {
    EntryHeader h{};
    if (!r.read(&h.key, 1)) {
        throw PackageError("Read key failed");
    }

    std::array<uint8_t, 8> len_buf;
    if (!r.read(len_buf.data(), 8)) {
        throw PackageError("Read path_len failed");
    }
    std::array<uint8_t, 8> len_be;
    for (size_t i = 0; i < 8; ++i) {
        len_be[i] = len_buf[i] ^ h.key;
    }
    h.path_len = utils::from_be64(len_be.data());

    std::vector<uint8_t> path_enc(h.path_len);
    if (!r.read(path_enc.data(), h.path_len)) {
        throw PackageError("Read path failed");
    }
    utils::xor_range(path_enc.data(), h.path_len, h.key);
    h.path.assign(path_enc.begin(), path_enc.end());

    if (!r.read(len_buf.data(), 8)) {
        throw PackageError("Read content_len failed");
    }
    for (size_t i = 0; i < 8; ++i) {
        len_be[i] = len_buf[i] ^ h.key;
    }
    h.content_len = utils::from_be64(len_be.data());

    return h;
}
