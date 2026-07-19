#include <cstddef>
#include <string>
#include <vector>
#include <filesystem>
#include <exception>
#include <format>

#include "console.hpp"
#include "path_utils.hpp"
#include "packer.hpp"
#include "unpacker.hpp"
#include "lister.hpp"
#include "package_error.hpp"

#include "cli.hpp"

namespace cli {

void print_usage(const std::string& prog) {
    std::string const prog_name=path_utils::to_utf8(
        path_utils::from_utf8(prog).filename()
    );
    console::println_err(
        std::format(
            "Usages:\n"
            "    Pack  : {} -c/-C <output-package-path> <input-path-1>"
                " [<input-path-2> ...]\n"
            "    Unpack: {} -x/-X <input-package-path>"
                " <output-directory-path>\n"
            "    List  : {} -l/-L <input-package-path>\n"
            "    Help  : {} -h/-H\n"
            "\n"
            "Examples:\n"
            "    Pack a file and directory: {} -c 0.fgwsz README.md source\n"
            "    Unpack                   : {} -x 0.fgwsz output\n"
            "    List package contents    : {} -l 0.fgwsz\n"
            "    Help                     : {} -h",
            prog_name, prog_name, prog_name, prog_name,
            prog_name, prog_name, prog_name, prog_name
        )
    );
}

int execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        print_usage(args.empty() ? "fgwsz-package" : args[0]);
        return 1;
    }
    std::string mode = args[1];
    try {
        if (mode == "-c" || mode == "-C") {
            if (args.size() < 4) {
                console::println_err(
                    "Output package file path and at least one input path are"
                        " required."
                );
                return 1;
            }
            std::filesystem::path output = path_utils::from_utf8(args[2]);
            std::vector<std::filesystem::path> inputs;
            for (size_t i = 3; i < args.size(); ++i){
                inputs.push_back(path_utils::from_utf8(args[i]));
            }
            Packer::run(inputs, output);
        } else if (mode == "-x" || mode == "-X") {
            if (args.size() != 4) {
                console::println_err(
                    "Package file path and output directory are required."
                );
                return 1;
            }
            std::filesystem::path pkg = path_utils::from_utf8(args[2]);
            std::filesystem::path out = path_utils::from_utf8(args[3]);
            Unpacker::run(pkg, out);
        } else if (mode == "-l" || mode == "-L") {
            if (args.size() != 3) {
                console::println_err("Package file path is required.");
                return 1;
            }
            std::filesystem::path pkg = path_utils::from_utf8(args[2]);
            Lister::run(pkg);
        } else if (mode == "-h" || mode == "-H") {
            print_usage(args[0]);
            return 0;
        } else {
            console::println_err(std::format("Unknown mode: {}", mode));
            print_usage(args[0]);
            return 1;
        }
    } catch (const PackageError& e) {
        console::println_err(std::format("PackageError: {}", e.what()));
        return 1;
    } catch (const std::exception& e) {
        console::println_err(std::format("std::exception: {}", e.what()));
        return 1;
    }
    return 0;
}

} // namespace cli
