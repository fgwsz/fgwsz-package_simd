#pragma once

#include <string_view>   // std::string_view

// ========================== 控制台输出（跨平台 Unicode） ==========================
namespace console {

void print(std::string_view s);

void print_err(std::string_view s);

void println(std::string_view s = "");

void println_err(std::string_view s = "");

} // namespace console
