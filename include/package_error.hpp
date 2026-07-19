#pragma once

#include <stdexcept>
#include <string>
#include <source_location>

class PackageError : public std::runtime_error {

public:

    PackageError(
        const std::string& msg,
        std::source_location const location=std::source_location::current()
    );

};
