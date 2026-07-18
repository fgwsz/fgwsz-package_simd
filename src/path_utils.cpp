#include <string>
#include <string_view>
#include <filesystem>

#include "path_utils.hpp"

namespace path_utils {

[[nodiscard]] std::string to_utf8(const std::filesystem::path& p) {
    auto u8 = p.u8string();
    return std::string(reinterpret_cast<const char*>(u8.c_str()), u8.size());
}

[[nodiscard]] std::filesystem::path from_utf8(std::string_view sv) {
    return std::filesystem::path(reinterpret_cast<const char8_t*>(sv.data()));
}

[[nodiscard]] std::string to_posix_utf8(const std::filesystem::path& p) {
    auto u8 = p.generic_u8string();
    return std::string(reinterpret_cast<const char*>(u8.c_str()), u8.size());
}

} // namespace path_utils
