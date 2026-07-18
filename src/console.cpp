#include <string_view>   // std::string_view
#include <iostream>      // std::cout, std::cerr
#include <string>        // std::wstring (Windows)

#ifdef _WIN32
#include <windows.h>     // HANDLE, etc.
#endif

#include "console.hpp"

namespace console {

#ifdef _WIN32

namespace detail {

void write_utf8(std::string_view str, HANDLE h) {
    if (h == INVALID_HANDLE_VALUE || !h) {
        std::cout.write(str.data(), str.size());
        return;
    }
    int len = MultiByteToWideChar(
        CP_UTF8, 0, str.data(),static_cast<int>(str.size()), nullptr, 0
    );
    if (len > 0) {
        std::wstring wstr(len, L'\0');
        MultiByteToWideChar(
            CP_UTF8, 0, str.data(),
            static_cast<int>(str.size()), wstr.data(), len
        );
        DWORD written;
        WriteConsoleW(
            h, wstr.c_str(), static_cast<DWORD>(wstr.size()),
            &written, nullptr
        );
    }
}

} // namespace detail

void print(std::string_view s) {
    detail::write_utf8(s, GetStdHandle(STD_OUTPUT_HANDLE));
}

void print_err(std::string_view s) {
    detail::write_utf8(s, GetStdHandle(STD_ERROR_HANDLE));
}

#else

void print(std::string_view s) {
    std::cout << s;
}

void print_err(std::string_view s) {
    std::cerr << s;
}

#endif

void println(std::string_view s) {
    print(s); print("\n");
}

void println_err(std::string_view s) {
    print_err(s); print_err("\n");
}

} // namespace console
