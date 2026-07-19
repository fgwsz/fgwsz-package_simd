#include <cstddef>     // size_t
#include <vector>      // std::vector
#include <string>      // std::string

#ifdef _WIN32
    #include <windows.h>   // WideCharToMultiByte, CP_UTF8, etc.
#endif

#include "cli.hpp"

// 确保 64 位文件大小支持
static_assert(
    sizeof(size_t) >= 8, "This program requires 64-bit file size support"
);

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        int len = WideCharToMultiByte(
            CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr
        );
        if (len > 0) {
            std::string s(len-1, '\0');
            WideCharToMultiByte(
                CP_UTF8, 0, argv[i], -1, s.data(), len, nullptr, nullptr
            );
            args.push_back(s);
        }
    }
    return cli::execute(args);
}
#else
int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv+argc);
    return cli::execute(args);
}
#endif
