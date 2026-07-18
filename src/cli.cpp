#include <cstddef>
#include <string>
#include <vector>
#include <filesystem>
#include <exception>

#include "console.hpp"
#include "path_utils.hpp"
#include "packer.hpp"
#include "unpacker.hpp"
#include "lister.hpp"
#include "package_error.hpp"

#include "cli.hpp"

namespace cli {

void print_usage(const std::string& prog) {
    console::println_err("用法:");
    console::println_err(
        "  打包: " + prog + " -c 输出包名 输入路径1 [输入路径2 ...]"
    );
    console::println_err("  解包: " + prog + " -x 包文件 输出目录");
    console::println_err("  列表: " + prog + " -l 包文件");
}

int execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        print_usage(args.empty() ? "fgwsz-package" : args[0]); return 1;
    }
    std::string mode = args[1];
    try {
        if (mode == "-c") {
            if (args.size() < 4) {
                console::println_err("需要输出包名和至少一个输入路径");
                return 1;
            }
            std::filesystem::path output = path_utils::from_utf8(args[2]);
            std::vector<std::filesystem::path> inputs;
            for (size_t i = 3; i < args.size(); ++i){
                inputs.push_back(path_utils::from_utf8(args[i]));
            }
            Packer::run(inputs, output);
        } else if (mode == "-x") {
            if (args.size() != 4) {
                console::println_err("需要包文件和输出目录"); return 1;
            }
            std::filesystem::path pkg = path_utils::from_utf8(args[2]);
            std::filesystem::path out = path_utils::from_utf8(args[3]);
            Unpacker::run(pkg, out);
        } else if (mode == "-l") {
            if (args.size() != 3) {
                console::println_err("需要包文件"); return 1;
            }
            std::filesystem::path pkg = path_utils::from_utf8(args[2]);
            Lister::run(pkg);
        } else {
            console::println_err("未知模式: " + mode);
            print_usage(args[0]);
            return 1;
        }
    } catch (const PackageError& e) {
        console::println_err("错误: " + std::string(e.what()));
        return 1;
    } catch (const std::exception& e) {
        console::println_err("异常: " + std::string(e.what()));
        return 1;
    }
    return 0;
}

} // namespace cli
