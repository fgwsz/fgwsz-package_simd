#include <format>

#include "console.hpp"
#include "path_utils.hpp"
#include "utils.hpp"

#include "summary.hpp"

namespace summary {

void print_summary(
    std::string_view action,
    const std::filesystem::path& target,
    size_t count,
    uint64_t content_size,
    uint64_t package_size,
    double seconds
) {
    double ratio = package_size
        ? (static_cast<double>(content_size) / package_size * 100)
        : 0;

    console::println(
        std::format(
            "\n{} completed, target: {}\n"
            "Total: {} files\n"
            "Content: {} ({} bytes)\n"
            "Package: {} ({} bytes)\n"
            "Ratio: {:.6f} %\n"
            "Time: {:.6f} s",
            action,
            path_utils::to_utf8(target),
            count,
            utils::format_size(content_size),
            content_size,
            utils::format_size(package_size),
            package_size,
            ratio,
            seconds
        )
    );
}

} // namespace summary
