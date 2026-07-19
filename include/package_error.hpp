#pragma once

#include<exception>
#include <string>
#include <string_view>
#include <source_location>

class PackageError : public std::exception {

public:

    PackageError(
        std::string_view message,
        std::source_location const location=std::source_location::current()
    );

    virtual char const* what(void) const noexcept override;

private:

    std::string const what_str_;

};
