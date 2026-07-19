# fgwsz-package_simd

**高性能跨平台文件打包/解包工具**，使用 C++20 编写，支持 mmap 零拷贝、SIMD 加速 XOR 混淆，**速度远超 Python 原版**，适用于大规模文件归档与分发。

---

## ✨ 特性

- **极致性能**：利用 `mmap` 零拷贝 + AVX2/SSE2/NEON 批量异或，打包/解包速度大幅提升（实测 8.3GB 数据 **4.1 秒打包，3.5 秒解包**，约 2 倍于 Python 版）。
- **跨平台**：支持 Windows、Linux、macOS，路径编码使用 UTF-8，控制台输出自动适配编码。
- **二进制兼容**：生成的 `.fgwsz` 包与 Python 原版完全互通，可混合使用。
- **轻量级**：仅依赖 C++ 标准库，无第三方依赖，内存占用恒定（64MB 缓冲区）。
- **多种 I/O 后端**：读取优先 mmap，回退到标准流；写入优先 mmap，回退到原生系统 API（FileWriter），确保稳定可靠。

---

## 🚀 性能对比

测试环境：Intel i7-10750H, NVMe SSD, Ubuntu 22.04  
测试数据：**8.30 GB**（237 个文件，包含视频大文件）

| 操作 | C++ SIMD 版 (`fgwsz-package_simd`) | Python 原版 (`fgwsz-package`) | 提升 |
|------|----------------------------------|----------------------------|------|
| 打包 | **4.09 秒** | 6.16 秒 | **≈1.5x** |
| 解包 | **3.45 秒** | 6.53 秒 | **≈1.9x** |

> 💡 260MB / 1084 个文件测试中，C++ 版打包仅需 **0.1~0.3 秒**，解包约 0.3~0.5 秒，远超 Python 版。

---

## 📦 安装与构建

### 前置条件
- C++20 编译器（GCC 11+ / Clang 14+ / MSVC 2019+）
- CMake 3.15+

### 快速构建（推荐）

项目提供了便捷的构建脚本：

**Linux / macOS**:
```bash
./cmakego.sh build          # 编译 Debug 版本
./cmakego.sh build_release  # 编译 Release 版本
./cmakego.sh run_release -c test.fgwsz /path/to/files  # 编译并运行 Release
```

**Windows (PowerShell)**:
```powershell
.\cmakego.ps1 build          # Debug
.\cmakego.ps1 build_release  # Release
.\cmakego.ps1 run_release -c test.fgwsz C:\path\to\files
```

### 手动构建（若不想使用脚本）

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

编译后生成可执行文件 `fgwsz-package_simd`（Windows 下为 `.exe`）。

> **优化选项**：Release 模式默认启用 `-O3 -march=native`，请确保编译器支持。

---

## 🛠️ 使用方法

命令行用法与 Python 版一致，支持三种操作模式：

### 打包
```bash
fgwsz-package_simd -c <包文件> <路径1> [路径2 ...]
```
- 打包多个文件/目录到 `.fgwsz` 包中。
- 路径存储规则：目录输入会包含目录名，直接文件仅存储文件名。

示例：
```bash
fgwsz-package_simd -c archive.fgwsz ./docs/ ./README.md
```

### 解包
```bash
fgwsz-package_simd -x <包文件> <输出目录>
```
解压所有文件到指定目录，自动创建子目录。

示例：
```bash
fgwsz-package_simd -x archive.fgwsz ./output
```

### 列表
```bash
fgwsz-package_simd -l <包文件>
```
显示包内所有文件的相对路径和大小。

示例：
```bash
fgwsz-package_simd -l archive.fgwsz
```

---

## 🔧 高级选项

- 所有路径支持 UTF-8（Windows 自动转换）。
- 输出文件自动设置为只读（防止误修改）。
- 覆盖前会提示确认（可交互输入 `y`/`Y`）。

---

## 📂 项目结构（主要模块）

| 模块 | 说明 |
|------|------|
| `io` 命名空间 | 抽象 I/O 接口（IReader / IWriter）及实现：MmapReader/Writer, StreamReader, FileWriter |
| `simd` 命名空间 | SIMD 加速 XOR（自动选择 AVX2 / SSE2 / NEON） |
| `Packer` / `Unpacker` / `Lister` | 核心打包/解包/列表逻辑 |
| `EntryHeader` | 包内条目头信息（密钥、路径、长度）序列化 |
| `FileWriter` | 跨平台原生文件写入（直接调用 WriteFile/write） |

---

## 🔄 与 Python 版的兼容性

- 包格式完全一致：条目使用 `[KEY (1B)] [PATH_LEN (8B) XOR] [PATH (XOR)] [CONTENT_LEN (8B) XOR] [CONTENT (XOR)]`。
- 两个版本可以互相打包/解包，无任何兼容问题。
- Python 版可在此仓库找到：`fgwsz-package`（原版）。

---

## 🧪 测试

运行单元测试（如有）：
```bash
ctest
```
或手动验证：
```bash
fgwsz-package_simd -c test.fgwsz /some/files
fgwsz-package_simd -l test.fgwsz
fgwsz-package_simd -x test.fgwsz ./out
diff -r /some/files ./out   # 应无差异
```

---

## 📜 许可证

本项目采用 **MIT 许可证**，详见 `LICENSE` 文件。

---

## 🤝 贡献

欢迎提交 Issue 和 Pull Request。请确保代码符合现有风格（大驼峰类名，snake_case 函数/变量）。

---

## 📞 联系方式

如有问题，请提交 Issue 或联系作者。

---

**Enjoy high-speed packaging with fgwsz-package_simd!** 🚀
