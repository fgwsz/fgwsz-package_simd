#pragma once

#include <stdexcept>
#include <string>

// ========================== 异常类型 ==========================
class PackageError : public std::runtime_error {
public:
    explicit PackageError(const std::string& msg) : std::runtime_error(msg) {}
};
