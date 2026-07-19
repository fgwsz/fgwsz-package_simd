#include <string>
#include <source_location>
#include <format>

#include "package_error.hpp"

PackageError::PackageError(
    std::string_view message, std::source_location const location
):what_str_(
    std::format(
        "file: {}({}:{}): {}"
        ,location.file_name()
        ,location.line()
        ,location.column()
        ,message
    )
){}

char const* PackageError::what(void) const noexcept{
    return this->what_str_.c_str();
}
