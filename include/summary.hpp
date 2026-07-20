#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <filesystem>

namespace summary {

/**
 * 打印打包/解包/列表操作的统计摘要
 * @param action 操作名称(如 "Pack completed")
 * @param target 目标路径(包文件或输出目录)
 * @param count  文件数量
 * @param content_size 内容总大小(字节)
 * @param package_size 包文件大小(字节)
 * @param seconds 耗时(秒)
 */
void print_summary(
    std::string_view action,
    const std::filesystem::path& target,
    size_t count,
    uint64_t content_size,
    uint64_t package_size,
    double seconds
);

} // namespace summary
