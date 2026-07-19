#pragma once

#include <string>
#include <vector>

namespace cli {

void print_usage(const std::string& prog);

int execute(const std::vector<std::string>& args);

} // namespace cli
