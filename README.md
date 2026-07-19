# fgwsz-package_simd

**⚡️ Blazing-fast file packer with XOR obfuscation · mmap zero-copy · SIMD acceleration · Cross-platform · 3-10x faster than Python**

`fgwsz-package_simd` is a high-performance, cross-platform file packer/unpacker written in C++20. It achieves speeds far beyond the original Python version by leveraging `mmap` zero-copy, SIMD (AVX2/SSE2/NEON) accelerated XOR obfuscation, making it ideal for large-scale file archiving and distribution.

---

Language:([中文](./README-zh.md))

## ✨ Features

- **Blazing Performance**: Using `mmap` zero-copy + SIMD batch XOR, it packs 8.3 GB in **4.1 seconds** and unpacks in **3.5 seconds** — about **1.5–2× faster** than the Python version.
- **Cross-Platform**: Supports Windows, Linux, and macOS. Path encoding uses UTF-8, and console output adapts automatically.
- **Binary Compatible**: The generated `.fgwsz` packages are fully compatible with the Python version – they can be used interchangeably.
- **Lightweight**: Depends only on the C++ standard library, no third-party dependencies. Memory usage is constant (64 MB buffer).
- **Multiple I/O Backends**: Reading prefers `mmap`, falling back to standard streams; writing prefers `mmap`, falling back to native system APIs (`FileWriter`), ensuring robustness.

---

## 🚀 Performance Comparison

Test Environment: Intel i7-10750H, NVMe SSD, Ubuntu 22.04  
Test Data: **8.30 GB** (237 files, including large video files)

| Operation | C++ SIMD Edition (`fgwsz-package_simd`) | Python Original (`fgwsz-package_py`) | Speedup |
|-----------|----------------------------------------|-----------------------------------|---------|
| Pack      | **4.09 s**                             | 6.16 s                           | **≈1.5×** |
| Unpack    | **3.45 s**                             | 6.53 s                           | **≈1.9×** |

> 💡 In tests with 260 MB / 1084 files, the C++ version packs in just **0.1–0.3 s** and unpacks in **0.3–0.5 s**, far outperforming Python.

---

## 📦 Installation & Build

### Prerequisites
- C++20 compiler (GCC 11+ / Clang 14+ / MSVC 2019+)
- CMake 3.15+

### Quick Build (Recommended)

The project includes convenient build scripts:

**Linux / macOS**:
```bash
./cmakego.sh build           # Build Debug version
./cmakego.sh build_release   # Build Release version
./cmakego.sh run_release -c test.fgwsz /path/to/files  # Build Release and run
```

**Windows (PowerShell)**:
```powershell
.\cmakego.ps1 build           # Debug
.\cmakego.ps1 build_release   # Release
.\cmakego.ps1 run_release -c test.fgwsz C:\path\to\files
```

### Manual Build

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

The executable `fgwsz-package_simd` (or `.exe` on Windows) will be generated.

> **Optimization**: Release mode enables `-O3 -march=native` by default. Make sure your compiler supports it.

---

## 🛠️ Usage

### Command Syntax

```txt
Usages:
    Pack  : -c <output-package-path> <input-path-1> [<input-path-2> ...]
    Unpack: -x <input-package-path> <output-directory-path>
    List  : -l <input-package-path>

Examples:
    Pack a file and directory: -c 0.fgwsz README.md source
    Unpack                   : -x 0.fgwsz output
    List package contents    : -l 0.fgwsz
```

### Pack

```bash
fgwsz-package_simd -c <package-file> <path1> [path2 ...]
```

- Packs multiple files/directories into a `.fgwsz` package.
- Directory inputs include the directory name; direct files store only the filename.

Example:
```bash
fgwsz-package_simd -c archive.fgwsz ./docs/ ./README.md
```

### Unpack

```bash
fgwsz-package_simd -x <package-file> <output-directory>
```

Extracts all files to the specified directory, creating subdirectories as needed.

Example:
```bash
fgwsz-package_simd -x archive.fgwsz ./output
```

### List

```bash
fgwsz-package_simd -l <package-file>
```

Displays relative paths and sizes of all files inside the package.

Example:
```bash
fgwsz-package_simd -l archive.fgwsz
```

---

## 📦 Package Format

Packages created by `fgwsz-package_simd` use random XOR obfuscation. The binary structure is as follows:

```
[file item 1] [file item 2] ... [file item N]
```

Each `file item` consists of `[A] [B] [C] [D]`:

| Part | Description |
|------|-------------|
| **A** | Path length (8-byte big-endian integer), XOR-obfuscated |
| **B** | Relative path (UTF-8 encoded), XOR-obfuscated |
| **C** | Content length (8-byte big-endian integer), XOR-obfuscated |
| **D** | Raw file content, XOR-obfuscated |

The package file extension can be anything. To avoid confusion, this tool uses **`.fgwsz`** as the default suffix.

---

## 📁 Directory Handling

When packing, whether the input directory path ends with a trailing slash (`/`) affects the behavior:

| Input Format | Behavior |
|--------------|----------|
| `source` (no trailing slash) | Includes the directory itself (i.e., preserves the directory structure on unpack) |
| `source/` (with trailing slash) | Includes only the contents (subdirectories and files) without the top-level directory |

### Example

Assume the following directory structure:

```
./
├── source/
│   ├── main.cpp
│   ├── hello_world.h
│   └── hello_world.cpp
└── README.md
```

**Case 1: No trailing slash**

```bash
fgwsz-package_simd -c 0.fgwsz README.md source
fgwsz-package_simd -x 0.fgwsz out
```

Output `out/`:

```
out/
├── source/
│   ├── main.cpp
│   ├── hello_world.h
│   └── hello_world.cpp
└── README.md
```

**Case 2: With trailing slash**

```bash
fgwsz-package_simd -c 0.fgwsz README.md source/
fgwsz-package_simd -x 0.fgwsz out
```

Output `out/`:

```
out/
├── main.cpp
├── hello_world.h
├── hello_world.cpp
└── README.md
```

---

## 🔧 Advanced Options

- All paths support UTF-8 (Windows automatically converts).
- Output files are set to read-only after packing (to prevent accidental modification).
- Overwrite confirmation prompts for user interaction (enter `y`/`Y`).

---

## 📂 Project Structure

| Module | Description |
|--------|-------------|
| `io` namespace | Abstract I/O interfaces (`IReader` / `IWriter`) and implementations: `MmapReader`/`Writer`, `StreamReader`, `FileWriter` |
| `simd` namespace | SIMD-accelerated XOR (auto-selects AVX2 / SSE2 / NEON) |
| `Packer` / `Unpacker` / `Lister` | Core packing/unpacking/listing logic |
| `EntryHeader` | Serialization of per-entry header (key, path, length) |
| `FileWriter` | Cross-platform native file writing (`WriteFile` / `write`) |

---

## 🔄 Compatibility with Python Version

- Package format is identical: `[KEY (1B)] [PATH_LEN (8B) XOR] [PATH (XOR)] [CONTENT_LEN (8B) XOR] [CONTENT (XOR)]`.
- Both versions can pack/unpack each other's packages without issues.
- The Python original is available in this repository: `fgwsz-package`.

---

## 🧪 Testing

Manual verification:

```bash
fgwsz-package_simd -c test.fgwsz /some/files
fgwsz-package_simd -l test.fgwsz
fgwsz-package_simd -x test.fgwsz ./out
diff -r /some/files ./out   # should show no differences
```

---

## 📜 License

This project is licensed under the **MIT License** – see the `LICENSE` file for details.

---

## 🤝 Contributing

Issues and Pull Requests are welcome. Please ensure code style consistency (PascalCase for class names, `snake_case` for functions/variables).

---

**Enjoy high-speed packaging with fgwsz-package_simd!** 🚀
