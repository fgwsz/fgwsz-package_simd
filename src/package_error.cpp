#include <stdexcept>
#include <string>
#include <source_location>
#include <format>

#include "package_error.hpp"

PackageError::PackageError(
    const std::string& msg, std::source_location const location
)
    : std::runtime_error(
        std::format(
            "file: {}({}:{}): {}"
            ,location.file_name()
            ,location.line()
            ,location.column()
            ,msg
        )
    )
{}
