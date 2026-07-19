#pragma once

#include <string_view>   // std::string_view

namespace console {

void print(std::string_view s);

void print_err(std::string_view s);

void println(std::string_view s = "");

void println_err(std::string_view s = "");

void flush();

void init();

} // namespace console
