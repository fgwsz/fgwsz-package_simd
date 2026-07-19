#pragma once

#include <string>
#include <string_view>
#include <filesystem>

namespace path_utils {

[[nodiscard]] std::string to_utf8(const std::filesystem::path& p);

[[nodiscard]] std::filesystem::path from_utf8(std::string_view sv);

[[nodiscard]] std::string to_posix_utf8(const std::filesystem::path& p);

} // namespace path_utils
